/*===========================================================================

  rpm_handler.h - objects for incoming request handling

  Copyright (c) 2012 Qualcomm Technologies Incorporated.  
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef RPM_HANDLER_H
#define RPM_HANDLER_H

#include <stdint.h>

#include "rpm_task.h"
#include "rpm_resources.h"
#include "kvp.h"
#include "smd_lite.h"

//number of resources per master that are allowed to settle at a given time
#define NUM_SETTLING_CONTEXTS 3

typedef struct
{
    unsigned msg_type;
    unsigned msg_size;
    unsigned msg_id;
    unsigned set;
    unsigned resource_type;
    unsigned resource_id;
    unsigned data_len;
} RPMMessageHeader;

typedef struct
{
    unsigned msg_type;
    unsigned msg_size;
} RPMCommandHeader;

typedef struct
{
    ResourceData     *r;
    uint32_t         msg_id;
    uint64_t         settling_time;
} SettlingInfo;

class Handler : public Task
{
public:
  Handler(unsigned ee, unsigned buflen, uint8_t priority = 127);
    virtual ~Handler();

    void init();

    // This EE's has at least one pending request.
    void enqueue(uint64_t deadline = 0);

    void requeue_settling(void);

    virtual uint64_t get_length() const;

    virtual bool hasImmediateWork() const;

    // Process from the queue until it is empty, or the next request is *not* a
    // NAS request.  Returns true if completed successfully, or false if
    // preempt becomes true or stop time is reached.
    bool processNAS(volatile bool &preempt, uint64_t &stop_time);

    void writeResponse(unsigned data, const char *error, rpm_service_type service);

    smdl_handle_type smdlPort_;

protected:
    // Prepares, dispatches, and finalizes the request handling.
    virtual void execute_until(volatile bool &preempt, uint64_t stop_time);

private:
    enum execution_state
    {
        IDLE,
        DISPATCH,
        SETTLING,
        CLEANUP,
    };
    const char *processHeader(RPMMessageHeader *header);
    void handleAlternateServiceMessage(unsigned service, void *data, unsigned length);
    boolean register_settling_slot(unsigned msg_id, uint64_t settling_time);

    bool loadMessage();
    uint64 processMessage (volatile bool &preempt, uint64_t &stop_time, RPMMessageHeader *header, bool allow_settling);
    void processCommand(RPMCommandHeader *header);

    unsigned         ee_;

    bool             haveMessage_;
    bool             isIdle_;

    uint32_t         haveMessageLen_; // in bytes
    uint32_t        *rxBuf_;
    unsigned         rxBufLen_; // in bytes
    RPMMessageHeader *messageHeader_; // points to the next message to process
    RPMCommandHeader *commandHeader_; // points to the next command to process
    kvp_t           *rxKvp_;
    const char      *error_;
    ResourceData    *resource_;

    kvp_t           *txKvp_;

    execution_state  state_;
    bool             isHandlerSettling_;
    bool             isHandlerDispatching_;

    SettlingInfo     settlingMsg_[NUM_SETTLING_CONTEXTS];

    // Profiling data.
    uint64_t handlerStart_;
    uint64_t handlerEnd_;
    uint32_t immediateLatencyExceeded_;
};

#endif // RPM_HANDLER_H

