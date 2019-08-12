/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.  
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.
  7/7/2014 Added it for VCE++

  ===========================================================================*/

#ifdef _MSC_VER
    // enable memory leak detection under msvc
    #define _CRTDBG_MAP_ALLOC
    #include <stdlib.h>
    #include <crtdbg.h>
#endif

#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "inmap.h"
#include "npa_resource.h"

#include "rpmclient.h"
#include "rpm_mq.h"
#include "rpm_resource.h"
#include "rpm_wire.h"

#include "DALSys.h"
#include "smd_lite.h"
#include "err.h"
#include "ULogFront.h"
#include "CoreVerify.h"

//This limit is put in place because when sleep forces the NAS and sleep set across, 
//there is a finite amount of space for the acks. If we reach this limit, we need to churn.
#define SLEEP_PLUS_NAS_ACK_LIMIT 24

typedef struct
{
    inmap_link_t         link;
    unsigned             service_id;
    rpmclient_service_cb cb;
    void                *context;
} rpmclient_service_t;

typedef struct
{
    void    *buffer;
    unsigned size;
} raw_smd_packet;
typedef struct
{
    rpm_service_type service; 
    unsigned         total_size;
    unsigned         msg_id;
    unsigned         set;
    unsigned         resource_type;
    unsigned         resource_id;
    unsigned         data_len;
} NASPacket;
static struct rpm_s
{
    unsigned          rpmclient_initialized;
    rpm_mq_t         *rpm_mq;

    unsigned          num_resources;
    rpm_resource_t  **resources;

    inmap_t          *services;

    smdl_handle_type  smd_port;
    kvp_t            *rx_kvp;
	unsigned          num_resources_dirty[RPM_NUM_SETS];
} rpm;

DALSYSSyncHandle rpm_lock;
DALSYSEventHandle     rpm_sync_event; 
DALSYSEventObj        rpm_sync_event_memory;
ULogHandle       rpm_log;
static raw_smd_packet nas_bundle;

static const char *rpm_set_names[RPM_NUM_SETS] =
{
  "active",
  "sleep",
  "next active"
};

void (*rpm_fifo_full_handler)(rpm_mq_t *, unsigned);

rpm_resource_t *rpm_find_resource(rpm_resource_type resource, unsigned id)
{
    uint64_t resource_id = (((uint64_t)resource) << 32) | id;
    unsigned low = 0, high = rpm.num_resources - 1;

    if(!rpm.num_resources)
        return NULL;

    while(high < rpm.num_resources && high >= low)
    {
        unsigned mid = (low + high) / 2;
        uint64_t current_id = rpm.resources[mid]->resource_identifier;

        if(current_id < resource_id)
            low = mid + 1;
        else if(current_id > resource_id)
            high = mid - 1;
        else
            return rpm.resources[mid];
    }

    return NULL;
}

rpm_resource_t *rpm_add_resource(rpm_resource_type resource, unsigned id)
{
    unsigned i, old_num_resources;
    uint64_t resource_id = (((uint64_t)resource) << 32) | id;
    rpm_resource_t *new_resource;

    new_resource = (rpm_resource_t *)malloc(sizeof(rpm_resource_t));
    CORE_VERIFY_PTR(new_resource);
    memset(new_resource, 0, sizeof(rpm_resource_t));
    rpm_resource_init(new_resource, resource, id);

    old_num_resources = rpm.num_resources;
    rpm.num_resources++;
    rpm.resources = realloc(rpm.resources, rpm.num_resources * sizeof(rpm_resource_t *));
    CORE_VERIFY_PTR(rpm.resources);

    CORE_VERIFY_PTR(rpm.resources);
    for(i = 0; i < old_num_resources; ++i)
    {
        rpm_resource_t *r = rpm.resources[i];

        CORE_VERIFY_PTR(r);
        if(r->resource_identifier > resource_id)
        {
            // Found entries that belong after us.  Move them and insert here.
            memmove(&rpm.resources[i+1], &rpm.resources[i], (old_num_resources - i) * sizeof(rpm_resource_t *));

            break;
        }
    }

    // Must be largest in the list; insert at the end.
    rpm.resources[i] = new_resource;

    // Refactor the queues to update any resource pointers in pending messages
    rpm_mq_update_resource_pointers(rpm.rpm_mq);

    return new_resource;
}

unsigned rpm_get_request_size(rpm_set_type set, rpm_resource_type resource, unsigned id)
{
    rpm_resource_t *resource_data; 
	
	if(set >= RPM_NUM_SETS)
        return 0;
		
	resource_data = rpm_find_resource(resource, id);
	
    if(!resource_data)
        return 0;

    return rpm_resource_get_request_size(set, resource_data);
}

unsigned rpm_post_request(rpm_set_type set, rpm_resource_type resource, unsigned id, kvp_t *kvps)
{
    rpm_resource_t *resource_data;
    unsigned        msg_id = 0;

    if(set >= RPM_NUM_SETS)
        return 0;

    DALSYS_SyncEnter(rpm_lock);
    
    ULOG_RT_PRINTF_3(rpm_log, "rpm_post_request (resource: 0x%08x) (id: 0x%08x) (set: \"%s\")", resource, id, rpm_set_names[set]);

    resource_data = rpm_find_resource(resource, id);
    if(!resource_data)
        resource_data = rpm_add_resource(resource, id);

    rpm_resource_update(resource_data, set, kvps);
  
    if(!kvps ||((RPM_ACTIVE_SET == set) && resource_data->dirty[set]))
    {
        msg_id = rpm_mq_put(rpm.rpm_mq, set, resource, id, resource_data);
        ULOG_RT_PRINTF_3(rpm_log, "\trpm_posted (resource: 0x%08x) (id: 0x%08x) (msg_id: 0x%08x)", resource, id, msg_id);
    }

    DALSYS_SyncLeave(rpm_lock);

    return msg_id;
}

void rpm_barrier(unsigned message_id)
{
    DALSYS_SyncEnter(rpm_lock);

    ULOG_RT_PRINTF_1(rpm_log, "rpm_barrier (msg_id: 0x%08x)", message_id);
    rpm_mq_wait(rpm.rpm_mq, message_id);
    ULOG_RT_PRINTF_1(rpm_log, "\trpm_barrier_return (msg_id: 0x%08x)", message_id);

    DALSYS_SyncLeave(rpm_lock);
}

static void rpmclient_force_nas(void)
{
    int  written;
    smdl_iovec_type header;
    header.next = 0;
    header.length = nas_bundle.size;
    header.buffer = nas_bundle.buffer;

    written = smdl_writev(rpm.smd_port,
                          &header,
                          SMDL_WRITE_FLAGS_NONE);
						  
    CORE_VERIFY( written >= 0 );
    free(nas_bundle.buffer);
    nas_bundle.buffer = NULL;
    nas_bundle.size = 0;
}

unsigned rpm_force_sync(rpm_set_type set)
{
    unsigned last_msg_id = 0;

    if(set >= RPM_NUM_SETS)
        return 0;

    DALSYS_SyncEnter(rpm_lock);

    ULOG_RT_PRINTF_4(rpm_log, "rpm_force_sync (set: %d) (dirty: %d,%d,%d)", set, rpm.num_resources_dirty[0], rpm.num_resources_dirty[1], rpm.num_resources_dirty[2]);


    if((set == RPM_NEXT_ACTIVE_SET) && (rpm.num_resources_dirty[set] > 0))
    {
	    unsigned oldest_msg_id;
        //if we are sending the NAS set, churn all the active/sleep messages so we have plenty of room
        while((oldest_msg_id = rpm_mq_get_oldest_in_flight_msg_id(rpm.rpm_mq)))
        {
            rpm_churn_queue(oldest_msg_id);
        }  
    }

     // if the set has been dirty, do sync
    if(rpm.num_resources_dirty[set])
    {
        unsigned i;

        for(i = 0; i < rpm.num_resources; ++i)
        {
            rpm_resource_t *resource = rpm.resources[i];
            if(resource->dirty[set])
            {
                rpm_resource_type type = (rpm_resource_type)(resource->resource_identifier >> 32);
                unsigned          id   = resource->resource_identifier & 0xFFFFFFFFU;
                unsigned          msg_id;

                ULOG_RT_PRINTF_2(rpm_log, "\trpm_flushing (resource: 0x%08x) (id: 0x%08x)", type, id);
                msg_id = rpm_mq_put(rpm.rpm_mq, set, type, id, resource);
                if(msg_id)
                    last_msg_id = msg_id;
            }
        }
    }
    CORE_VERIFY(!(rpm.num_resources_dirty[set]));

    if ((last_msg_id) && (set == RPM_NEXT_ACTIVE_SET))
    {
        rpmclient_force_nas();
    }

    ULOG_RT_PRINTF_2(rpm_log, "rpm_flushed (set: %d) (msg_id: 0x%08x)", set, last_msg_id);

    DALSYS_SyncLeave(rpm_lock);

    return last_msg_id;
}

void rpmclient_register_handler(rpm_service_type service, rpmclient_service_cb cb, void *context)
{
    rpmclient_service_t *service_obj;

    DALSYS_SyncEnter(rpm_lock);

    service_obj = (rpmclient_service_t *)malloc(sizeof(rpmclient_service_t));
    CORE_VERIFY_PTR(service_obj);

    service_obj->service_id = service;
    service_obj->cb         = cb;
    service_obj->context    = context;
    inmap_insert(rpm.services, service_obj);
    DALSYS_SyncLeave(rpm_lock);
}
void rpmclient_put_nas(rpm_service_type service, smdl_iovec_type *data, smdl_iovec_type *kvps, RPMMessageHeader msg_header)
{
    NASPacket       nas_info;
    unsigned        header_data[2];
    DALSYS_SyncEnter(rpm_lock);
    header_data[0] = service;
    header_data[1] = 0;
    while(data)
    {
        header_data[1] += data->length;
        data = data->next;
    }

    //temp structure to hold everything except the kvps
    nas_info.service = service;
    nas_info.total_size = header_data[1] + sizeof(header_data);
    nas_info.msg_id = msg_header.msg_id;
    nas_info.set = RPM_NEXT_ACTIVE_SET;
    nas_info.resource_type = msg_header.resource_type;
    nas_info.resource_id = msg_header.resource_id;
    nas_info.data_len = msg_header.data_len;
    unsigned header_size = nas_info.total_size - nas_info.data_len;
    if(!nas_bundle.buffer) 
    {
        nas_bundle.size = nas_info.total_size;
        nas_bundle.buffer = (void *)malloc(nas_info.total_size);
        CORE_VERIFY_PTR(nas_bundle.buffer);
        nas_info.total_size -= sizeof(header_data);
        memcpy(nas_bundle.buffer, &nas_info, header_size); //memcpy NAS packet header
        memcpy((char *)nas_bundle.buffer + header_size, kvps->buffer, nas_info.data_len); //memcpy NAS packet kvps
    }
    else
    {
        void *temp_ptr = realloc(nas_bundle.buffer, nas_info.total_size + nas_bundle.size);
        CORE_VERIFY_PTR(temp_ptr);
        nas_bundle.buffer = temp_ptr;
        nas_info.total_size -= sizeof(header_data);
        memcpy(((char *)nas_bundle.buffer) + nas_bundle.size, &nas_info, header_size); //memcpy new NAS packet header
        memcpy(((char *)nas_bundle.buffer) + nas_bundle.size + header_size, kvps->buffer, nas_info.data_len); //memcpy new NAS packet kvps
        nas_bundle.size += (nas_info.total_size + sizeof(header_data));
    }

    DALSYS_SyncLeave(rpm_lock);
}

void rpmclient_put(rpm_service_type service, smdl_iovec_type *data)
{
    unsigned        header_data[2];
    smdl_iovec_type header;
    int             written;

    header.next = data;
    header.length = sizeof(header_data);
    header.buffer = header_data;

    DALSYS_SyncEnter(rpm_lock);

    header_data[0] = service;
    
    header_data[1] = 0;
    while(data)
    {
        header_data[1] += data->length;
        data = data->next;
    }

    // FIXME: in the future, we should use smdl_tx_peek and a condition
    // variable to block the thread until sending is allowed; for now, to get
    // code out the door, just fail when we have too much backpressure.
    while((smdl_tx_peek(rpm.smd_port) < (header_data[1] + sizeof(header_data))) || 
          (rpm_mq_check_num_in_flight(rpm.rpm_mq) > SLEEP_PLUS_NAS_ACK_LIMIT + 1))
    {
        rpm_fifo_full_handler(rpm.rpm_mq, header_data[1] + sizeof(header_data));
    }

    written = smdl_writev(rpm.smd_port,
                          &header,
                          SMDL_WRITE_FLAGS_NONE);
    CORE_VERIFY( written >= 0 ); 
    DALSYS_SyncLeave(rpm_lock);
}

void rpmclient_smdl_isr(smdl_handle_type port, smdl_event_type event, void *unused)
{
    char          rx_buffer[64];
    const char   *msg;
    int           to_read, read;
    unsigned      service_id, length;
    rpmclient_service_t *service;

    /* SMDL_EVENT_OPEN event is the indication for succesful  SMD channel creation */
    if((port == rpm.smd_port) && (SMDL_EVENT_OPEN == event))
    {
        /* event notification for SMD channel open complete */	    
        DALSYS_EventCtrl(rpm_sync_event, DALSYS_EVENT_CTRL_TRIGGER);
    }

    if(port != rpm.smd_port || SMDL_EVENT_READ != event)
        return;

    DALSYS_SyncEnter(rpm_lock);

    while((to_read = smdl_rx_peek(rpm.smd_port)) != 0)
    {
        if(to_read > sizeof(rx_buffer))
            ERR_FATAL("Need larger RPM rx buffer.", to_read, sizeof(rx_buffer), 0);

        read = smdl_read(rpm.smd_port,
                         sizeof(rx_buffer),
                         rx_buffer,
                         SMDL_READ_FLAGS_NONE);
        if(read < 0)
        {
            ERR_FATAL("smdl_read failed", to_read, read, sizeof(rx_buffer));
        }

        kvp_swapbuffer(rpm.rx_kvp, rx_buffer, to_read);
        if(!kvp_get(rpm.rx_kvp, &service_id, &length, &msg))
            ERR_FATAL("Malformed RPM message.", 0, 0, 0); // FIXME: just log/drop

        service = inmap_search(rpm.services, service_id);
        if(!service)
            ERR_FATAL("RPM message to unknown service.", 0, 0, 0); // FIXME: just log/drop

        kvp_swapbuffer(rpm.rx_kvp, msg, length);
        service->cb(rpm.rx_kvp, service->context);
    }

    DALSYS_SyncLeave(rpm_lock);
}

void rpm_churn_queue(unsigned message_id)
{
    DALSYS_SyncEnter(rpm_lock);

    ULOG_RT_PRINTF_1(rpm_log, "rpm_churn_queue (msg_id: 0x%08x)", message_id);

    while(rpm_mq_is_in_flight(rpm.rpm_mq, message_id))
    {
        ULOG_RT_PRINTF_1(rpm_log, "\tchurning (msg_id: 0x%08x)", message_id);
        rpmclient_smdl_isr(rpm.smd_port, SMDL_EVENT_READ, 0);
    }

    ULOG_RT_PRINTF_1(rpm_log, "rpm_churning_complete (msg_id: 0x%08x)", message_id);

    DALSYS_SyncLeave(rpm_lock);
}

void rpmclient_init(void)
{
    static const unsigned REQ_KEY  = 0x00716572;

    if(rpm.rpmclient_initialized)
        return;
    rpm.rpmclient_initialized = 1;
    rpm_fifo_full_handler = &rpm_mq_force_churn_cb;


    if(DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_RPM_SYNC_TYPE, &rpm_lock, 0))
        ERR_FATAL("Couldn't create RPM lock.", 0, 0, 0);

    if(DAL_SUCCESS != DALSYS_EventCreate(DALSYS_EVENT_ATTR_CLIENT_DEFAULT, 
                                &(rpm_sync_event),
                                &(rpm_sync_event_memory)))
        ERR_FATAL("Couldn't create RPM Sync event.", 0, 0, 0);


    CORE_DAL_VERIFY(ULogFront_RealTimeInit(&rpm_log,
                                           "RPMMasterLog",
                                           8192,
                                           ULOG_MEMORY_LOCAL,
                                           ULOG_RPM_LOCK_TYPE));

    rpm.rpm_mq = rpm_mq_create();

    rpm.services = inmap_create(offsetof(rpmclient_service_t, link), offsetof(rpmclient_service_t, service_id));
    rpmclient_register_handler((rpm_service_type)REQ_KEY, (rpmclient_service_cb)rpm_wire_recv, rpm.rpm_mq);

    rpm.rx_kvp = kvp_create(0);

    if(!is_standalone())
    {
      //make sure RPM has opened the port
      while(!smd_is_port_open("rpm_requests", RPM_EDGE_TYPE));
    }

    rpm.smd_port = smdl_open("rpm_requests",
                             RPM_EDGE_TYPE,
                             SMDL_OPEN_FLAGS_MODE_PACKET | SMDL_OPEN_FLAGS_PRIORITY_HIGH,
                             1024,
                             rpmclient_smdl_isr,
                             NULL);

    if(!rpm.smd_port)
        ERR_FATAL("Failed to establish communication with RPM.", (int)rpm.smd_port, 0, 0);
    else
        ULOG_RT_PRINTF_0(rpm_log, "rpm_smd_up");

    /* Wait for rpm_sync_event, 
     * will be release after SMDL_EVENT_OPEN is received */ 
	if(!is_standalone())
	{
	  DALSYS_EventWait(rpm_sync_event);
	}
	npa_define_marker("/init/rpm");
}

bool rpm_is_up(void)
{
    return (smdl_sig_get(rpm.smd_port, SMDL_SIG_CD) > 0);
}

bool rpm_is_mq_empty(void)
{
    return rpm_mq_check_empty(rpm.rpm_mq);
}

void rpm_signal_aborted_sleep(unsigned asserted)
{
    smdl_sig_set(rpm.smd_port, SMDL_SIG_RI, asserted);
}

void rpm_preallocate_resource_kvp(rpm_resource_type resource, unsigned id, unsigned key, unsigned expected_length)
{
    rpm_resource_t *resource_data = rpm_find_resource(resource, id);
    if(!resource_data)
        resource_data = rpm_add_resource(resource, id);

    rpm_resource_preallocate(resource_data, key, expected_length);
}
void rpm_update_num_dirty(rpm_set_type set, bool pre_dirty, bool post_dirty)
{
	if(set >= RPM_NUM_SETS)
        return;
		
    if(pre_dirty != post_dirty)
    {
       if(!post_dirty)
       {
           CORE_VERIFY(rpm.num_resources_dirty[set] > 0);
       }
       rpm.num_resources_dirty[set] += (post_dirty) ? 1 : -1; 
    }
}
#if 0 // used for testing on new targets
void rpm_test_task(unsigned long int unused_param)
{
  unsigned msg_id, data;
  kvp_t   *test_data = kvp_create(0);

  data = 0xf005ba11;
  kvp_put(test_data, 0x0ddba11, 4, (char *)&data);

  data = 0xfeeb1e;
  kvp_put(test_data, 0xfab1e5, 4, (char *)&data);

  msg_id = rpm_post_request(RPM_ACTIVE_SET, RPM_TEST_REQ, 0, test_data);
  rpm_barrier(msg_id);

  //while(1);
  // Call this when I'm started, I guess.  For now I'm going to ignore this
  // part since I just need to do some one-off testing.
  //rcinit_handshake_startup();
}
#endif

