/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include <assert.h>
#include <string.h>

#include "time_service.h"
#include "CoreVerify.h"

#include "rpm_config.h"
#include "rpm_estimator.h"
#include "rpm_handler.h"
#include "rpmserver_internal.h"
#include "swevent.h"
#include "rpm_settling_timer.h"
#include "rpm_spm_handshake.h"
#include <stddef.h>
#include "DALSys.h"


static const unsigned ERR_KEY  = 0x00727265;
static const unsigned MSG__KEY  = 0x2367736d;
static const unsigned TRNS__KEY  = 0x736e7274;

static boolean disable_settling = TRUE;

static void rpm_smd_handler(smdl_handle_type port, smdl_event_type event, Handler *handler)
{
    if(SMDL_EVENT_READ == event || SMDL_EVENT_RI == event)
        handler->enqueue();
}

static void rpm_settling_handler(Handler *handler)
{
    handler->requeue_settling();
}

Handler::Handler(unsigned ee, unsigned buflen, uint8_t priority)
    : Task(priority), smdlPort_(0), ee_(ee), haveMessage_(false), isIdle_(true),
      haveMessageLen_ (0), rxBuf_ (0), rxBufLen_ (buflen), messageHeader_ (NULL), error_(0),
      resource_(0), state_(IDLE), isHandlerSettling_(false), isHandlerDispatching_(false),
      handlerStart_(0), handlerEnd_(0), immediateLatencyExceeded_(0)
{
    DALSYS_Malloc (buflen, (void **)&rxBuf_);
    rxKvp_ = kvp_create(0);
    txKvp_ = kvp_create(64);

    for (unsigned i = 0; i<NUM_SETTLING_CONTEXTS; i++)
    {
        settlingMsg_[i].r = NULL;
        settlingMsg_[i].msg_id = 0;
        settlingMsg_[i].settling_time = 0;
    }
}

Handler::~Handler()
{
    kvp_destroy(rxKvp_);
    kvp_destroy(txKvp_);
}

void Handler::init()
{
    smdlPort_ = smdl_open("rpm_requests",
                          rpm->ees[ee_].edge,
                          SMDL_OPEN_FLAGS_MODE_PACKET,
                          rpm->ees[ee_].smd_fifo_sz,
                          (smdl_callback_t)rpm_smd_handler,
                          this);
}

void Handler::requeue_settling(void)
{
    if(isIdle_)
    {
        for(unsigned i = 0; i<NUM_SETTLING_CONTEXTS; i++)
        {
            if(settlingMsg_[i].msg_id)
            {
                SWEVENT(RPM_SETTLING_MESSAGE_REQUEUED, ee_);
                state_ = SETTLING;
                isIdle_ = false;
                resource_ = settlingMsg_[i].r;
                schedule_me(); // Plop us into the immediate queue.
                break;
            }
        }
    }
}

void Handler::enqueue(uint64_t deadline)
{

    // If we're actively queued to process another message, don't do anything right now.
    if(!isIdle_)
        return;

    // If we don't have a message we're currently working on, load the message from message RAM now.
    while(!haveMessage_ && smdl_rx_peek(smdlPort_))
        haveMessage_ = loadMessage();
    if(!haveMessage_)
        return; // Can't continue if we didn't get a message.

    SWEVENT(RPM_MESSAGE_RECEIVED, ee_, messageHeader_->msg_id);

    // We have a message to process.  Now we need to decide if we handle it now
    // or later--NAS requests don't actually get processed immediately unless
    // there's an aborted wakeup indicated.
    if(messageHeader_->set == RPM_NEXT_ACTIVE_SET && !smdl_sig_get(smdlPort_, SMDL_SIG_RI))
        return; // Don't process this yet.
    else
    {
        isIdle_ = false;
        isHandlerDispatching_ = true;
        state_ = DISPATCH;

        schedule_me(); // Plop us into the immediate queue.
    }
}

uint64_t Handler::get_length() const
{
    if(isIdle_)
        return 0;

    unsigned estimate = resource_estimate(resource_, theEstimator().currentOperatingPoint());
    if(disable_settling)
    {
        return (estimate + resource_settling_estimate(resource_));
    }
    return estimate;
}

bool Handler::hasImmediateWork() const
{
    return !isIdle_;
}

boolean Handler::register_settling_slot(unsigned msg_id, uint64 settling_time)
{
    boolean slot_found = FALSE;
    slot_found =  settling_timer_register((settling_callback)rpm_settling_handler, (void *)this, settling_time);
    if(slot_found)
    {
        slot_found = FALSE;
        for (unsigned i = 0; i < NUM_SETTLING_CONTEXTS; i++)
        {
            if(!settlingMsg_[i].msg_id)
            {
                slot_found = TRUE;
                settlingMsg_[i].msg_id = msg_id;
                settlingMsg_[i].r = resource_;
                settlingMsg_[i].settling_time = settling_time;
                return TRUE;
            }
        }
    }
    return FALSE;
}

/*===========================================================================
FUNCTION: Handler::execute_until

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void Handler::execute_until(volatile bool &preempt, uint64_t stop_time)
{
    uint64 settling_time = 0;
    INTLOCK();

    RPMMessageHeader *header = messageHeader_;

    // If we're dispatching NAS from here, it's because there's a rude wakeup.  Treat it as active set.
    if(RPM_NEXT_ACTIVE_SET == header->set)
        header->set = RPM_ACTIVE_SET;


    switch(state_)
    {
        case DISPATCH:
        {
            settling_time = processMessage (preempt, stop_time, header, true);
            isHandlerDispatching_ = false;
            haveMessage_ = false;
            if(!settling_time)
            {
                bool handler_settling = false;
                //if the current message was set up for prior settling, clear it
                for(unsigned i = 0; i<NUM_SETTLING_CONTEXTS; i++)
                {
                    if(settlingMsg_[i].msg_id == header->msg_id)
                    {
                        settlingMsg_[i].r = NULL;
                        settlingMsg_[i].msg_id = 0;
                        settlingMsg_[i].settling_time = 0;
                    }
                    else if(settlingMsg_[i].msg_id)
                    {
                        handler_settling = true;
                    }
                }

                isHandlerSettling_ = handler_settling;
                //no settling time for this message, but lets check if there are other resources settling
                if(isHandlerSettling_)
                    state_ = SETTLING;
                else
                    state_ = CLEANUP;
            }
            else
            {
                //make sure we have room for this and register a a settling timer
                CORE_VERIFY(register_settling_slot(header->msg_id, settling_time));
                SWEVENT(RPM_RESOURCE_SETTLING, ee_, (rpm_resource_type)header->resource_type, header->resource_id, header->msg_id);
                SWEVENT(RPM_RESOURCE_SETTLING_TIME, (uint32)settling_time, (uint32)(settling_time >> 32));

                //if there are other messages settling, go to settling, otherwise jump out
                if(isHandlerSettling_)
                {
                    state_ = SETTLING;
                }
                else
                {
                    //jump to cleanup, check for more messages
                    isHandlerSettling_ = true;
                    state_ = CLEANUP;
                }
            }
        }

        case SETTLING:
        {
            if(state_ == SETTLING)
            {
                uint64 current_time = time_service_now();
                uint64 new_time = 0;
                bool settling_flag = false;
                //try to handle anything that is currently settilng
                for(unsigned i = 0; i<NUM_SETTLING_CONTEXTS; i++)
                {
                    if(settlingMsg_[i].settling_time && (settlingMsg_[i].settling_time <= current_time))
                    {
                        //found a request that should be done settling. Re-call the apply function and get rid of the local data store
                        new_time = resource_ee_settling_apply(settlingMsg_[i].r, ee_, settlingMsg_[i].settling_time);
                        //if its settled, finish the message. Otherwise re-queue
                        if(!new_time)
                        {
                            settling_flag |= false;
                            SWEVENT(RPM_RESOURCE_SETTLING_COMPLETE, ee_, (rpm->classes[settlingMsg_[i].r->class_idx]).type,							
                                                                   settlingMsg_[i].r->id, settlingMsg_[i].msg_id);							

                            writeResponse(settlingMsg_[i].msg_id, 0, RPM_REQUEST_SERVICE);

                            settlingMsg_[i].r = NULL;
                            settlingMsg_[i].msg_id = 0;
                            settlingMsg_[i].settling_time = 0;

                        }
                        else
                        {
                            settlingMsg_[i].settling_time = new_time;
                            settling_flag = true;
                            CORE_VERIFY(settling_timer_register((settling_callback)rpm_settling_handler, (void *)this, new_time));

                            SWEVENT(RPM_RESOURCE_SETTLING, ee_, (rpm->classes[settlingMsg_[i].r->class_idx]).type, settlingMsg_[i].r->id, settlingMsg_[i].msg_id);
                            SWEVENT(RPM_RESOURCE_SETTLING_TIME, (uint32)new_time, (uint32)(new_time >> 32));
                        }
                    }
                }
                isHandlerSettling_ = settling_flag;
                state_ = CLEANUP;
            }
        }

        case CLEANUP:
        {
            handlerEnd_ = time_service_now();
            // Check to see if we're done.
            while(!haveMessage_ && smdl_rx_peek(smdlPort_))
                haveMessage_ = loadMessage();
            if(!haveMessage_ || (header->set == RPM_NEXT_ACTIVE_SET && !smdl_sig_get(smdlPort_, SMDL_SIG_RI)))
            {
                isIdle_ = true;
                state_ = IDLE;
            }
            else
            {
                CORE_VERIFY(!isIdle_);
                state_ = DISPATCH;
                isHandlerDispatching_ = true;
            }
        }
    }
    INTFREE();
}

void Handler::processCommand(RPMCommandHeader *header)
{
    unsigned               type, length, cmd_data;
    const char             *value_ptr;

    SWEVENT(RPM_COMMAND_RECEIVED, ee_, messageHeader_->msg_id);

    // Point the rxKvp at the start of the data.  Note that (header + 1) is
    // code that skips the header, not the first byte (pointer math!).
    kvp_swapbuffer(rxKvp_, (char *)(header + 1), header->msg_size);
    if(!kvp_get( rxKvp_, &type, &length, &value_ptr ) || !value_ptr)
    {
        writeResponse(type, "Malformed command", RPM_COMMAND_SERVICE);
        return;
    }

    switch(type)
    {
        case RPM_TRANSITION_COMMAND:
            cmd_data = *(unsigned *)value_ptr;
            rpm_spm_soft_transition(ee_, (rpm_spm_entry_reason)cmd_data);
            break;
        default:
            abort();
    }
}

const char *Handler::processHeader(RPMMessageHeader *header)
{
    // Validate the incoming message, and ignore it if there is any "bad" data.
    if((header->set > RPM_NEXT_ACTIVE_SET) ||
     (header->data_len != ((header->msg_size - sizeof(RPMMessageHeader)) + offsetof(RPMMessageHeader, msg_id))))
    {
        return "invalid header";
    }

    // Try to find the target resource.
    resource_ = rpmserver_lookup_resource((rpm_resource_type)header->resource_type, header->resource_id);
    if(!resource_)
    {
        return "resource does not exist";
    }

    // Point the rxKvp at the start of the data.  Note that (header + 1) is
    // code that skips the header, not the first byte (pointer math!).
    kvp_swapbuffer(rxKvp_, (char *)(header + 1), header->data_len);
    return 0;
}

void Handler::writeResponse(unsigned data, const char *error, rpm_service_type service)
{
    smdl_iovec_type service_head_vec, result_vec;
    unsigned data_key = MSG__KEY;

    if(service == RPM_COMMAND_SERVICE)
        data_key = TRNS__KEY;

    // Build the response KVP's.
    kvp_clear(txKvp_);
    kvp_put(txKvp_, data_key, sizeof(unsigned), (char *)&(data));
    if(error)
        kvp_put(txKvp_, ERR_KEY, strlen(error), error);

    // Mark this message as a response to a request.
    unsigned service_head[2] = {service, kvp_get_raw_size(txKvp_)};
    service_head_vec.next = &result_vec;
    service_head_vec.buffer = &service_head;
    service_head_vec.length = sizeof(service_head);

    // Packetize the KVP into SMD's format.
    result_vec.next = 0;
    result_vec.buffer = (void *)kvp_get_raw_bytes(txKvp_);
    result_vec.length = service_head[1];

    SWEVENT(RPM_SEND_RESPONSE, ee_, service);

    // Write it out.
    int written = smdl_writev(smdlPort_,
                              &service_head_vec,
                              SMDL_WRITE_FLAGS_NONE);

    // FIXME: probably need to add code to handle backpressure from a master.
    CORE_VERIFY(written == (kvp_get_raw_size(txKvp_) + sizeof(service_head)));
}

bool Handler::loadMessage()
{
    unsigned msg_type = 0;
    bool retval = true;
    handlerStart_ = time_service_now();

    // Pull the message from message RAM.
    int msg_size = smdl_rx_peek(smdlPort_);
    CORE_VERIFY(msg_size <= rxBufLen_);

    int bytes_read = smdl_read(smdlPort_,
                               msg_size,
                               rxBuf_,
                               SMDL_READ_FLAGS_NONE);
    CORE_VERIFY(bytes_read == msg_size);

    haveMessageLen_ = bytes_read;

    msg_type = *(unsigned *)rxBuf_;

    if((msg_size < sizeof(RPMMessageHeader) && (msg_type != RPM_COMMAND_SERVICE)) ||
       (msg_size < sizeof(RPMCommandHeader) && (msg_type == RPM_COMMAND_SERVICE)))
    {
        writeResponse(0, "invalid message", RPM_REQUEST_SERVICE);
        return false;
    }

    switch (msg_type)
    {
        case RPM_REQUEST_SERVICE:
            messageHeader_ = (RPMMessageHeader *)rxBuf_;
            if (messageHeader_->set != RPM_NEXT_ACTIVE_SET)
            {
                const char *error = processHeader(messageHeader_);
                if(error)
                {
                    writeResponse(messageHeader_->msg_id, error, RPM_REQUEST_SERVICE);
                    retval = false;
                }
            }
            break;

        case RPM_COMMAND_SERVICE:
            commandHeader_ = (RPMCommandHeader *)rxBuf_;
            processCommand(commandHeader_);
            retval = false;
            break;

        default:
            handleAlternateServiceMessage(rxBuf_[0], &rxBuf_[1], msg_size - 4);
            retval = false;
            break;
    }
    return retval;
}

void Handler::handleAlternateServiceMessage(unsigned service, void *data, unsigned length)
{
    INTLOCK();

    rpmserver_service_t *s = rpm_alt_service_list;

    for(; s; s = s->next)
    {
        if(service == s->service_id)
        {
            s->cb(ee_, data, length, s->context);
            break;
        }
    }

    INTFREE();
}

uint64 Handler::processMessage (volatile bool &preempt, uint64_t &stop_time, RPMMessageHeader *header, bool allow_settling)
{
    uint64 settling_time = 0;
    bool applied = false;
  // Temporarily commenting this section out - it is causing freezes on LA. Need to debug further.
#if 0
  if (preempt ||
      (time_service_now () + resource_estimate (resource_, theEstimator().currentOperatingPoint()) > stop_time))
  {
    return;
  }
#endif

    // Start timing how long this takes.
    Estimator  &estimator = theEstimator();
    Observation obs       = estimator.startObservation();
    uint64 observed_settling = 0;

    // Issue the request to the resource.
    SWEVENT(RPM_PROCESS_REQUEST, ee_, (rpm_resource_type)header->resource_type, header->resource_id);
    applied = resource_ee_request(resource_, ee_, (rpm_set_type)header->set, (kvp_eof(rxKvp_) ? NULL : rxKvp_), settling_time);

    if(settling_time)
    {
        uint64 now = time_service_now();
        if ((!allow_settling) || disable_settling)
        {
            while(settling_time)
                settling_time = resource_ee_settling_apply(resource_, ee_, settling_time);
            observed_settling = time_service_now() - now;
        }
        else if(settling_time <= now)
        {
            settling_time = resource_ee_settling_apply(resource_, ee_, settling_time);
        }
        else
        {
            observed_settling = settling_time - now;
        }
    }

    if(!settling_time)
    {
        writeResponse(header->msg_id, 0, RPM_REQUEST_SERVICE);
    }

    // Finish timing and record the observation.
    obs = estimator.completeObservation(obs);
    if ((!allow_settling) || disable_settling)
    {
        obs.time -= observed_settling;
    }

    if(applied)
    {
        resource_observe(resource_, theEstimator().currentOperatingPoint(), obs, observed_settling);
    }

    return settling_time;
}

// Process from the queue until it is empty, or the next request is *not* a
// NAS request.  Returns true if completed successfully, or false if
// preempt becomes true or stop time is reached.
bool Handler::processNAS(volatile bool &preempt, uint64_t &stop_time)
{

    SWEVENT(RPM_NAS_START);
    while(!preempt && (time_service_now() < stop_time))
    {
        if(!haveMessage_)
        {
            if (!smdl_rx_peek(smdlPort_))
            {
                return true; // No messages to process.
            }
            haveMessage_ = loadMessage ();
        }

        if(haveMessage_ && messageHeader_->set != RPM_NEXT_ACTIVE_SET)
            return true; // Found non-NAS message.

        CORE_VERIFY (messageHeader_->msg_size <= haveMessageLen_);
        const char *error = processHeader(messageHeader_);
        if(error)
        {
            writeResponse(messageHeader_->msg_id, error, RPM_REQUEST_SERVICE);
        }
        else if (preempt)
        {
            //loadMessage may have caused pre-emption from a soft transition command. Bail if that happened		
            return false;                        
        }		
        else
        {
            processMessage (preempt, stop_time, messageHeader_, false);
        }

        messageHeader_ = (RPMMessageHeader *)((uint8 *)messageHeader_ + messageHeader_->msg_size +
                                              offsetof(RPMMessageHeader, msg_id));
        if ((uint8 *)messageHeader_ >= ((uint8 *)rxBuf_ + haveMessageLen_))
        {
            haveMessage_ = false;
            messageHeader_ = NULL;
            haveMessageLen_ = 0;
        }
    }

    // Didn't hit a completion condition.
    return false;
}

