#include "swevent.h"
#include "swevent_internal.h"
#include "comdef.h"
#include <stdlib.h>
#include "time_service.h"
#include "QDSSLite.h"
#include "rpm_ulog.h"
#include "rpmserver.h"
#include "image_layout.h"
#include "CoreVerify.h"

static rpm_ULogHandle rpm_log;
uint32 qdss_port;

static uint64 ram_enabled_bmsk[16];
static uint64 qdss_enabled_bmsk[16];

static uint8 qdss_output_enabled = 0;
static uint8 rpm_log_silent_mode = 0;

static boolean qdss_clocks_enabled = FALSE;

static unsigned ram_log_count = 0;
static unsigned qdss_log_count = 0;

#define QDSS_SWEVT_ENTITY    2
#define QDSS_SWEVT_PROTOCOL  1
#define QDSS_LOG_LENGTH 1024
#define RAM_LOG_LENGTH 256

void rpm_log_event_to_ram(tracer_event_id_t id, uint32 d0, uint32 d1, uint32 d2, uint32 d3)
{
    uint32 message[7]; // data
    uint64 timestamp = time_service_now();

    if(rpm_log_silent_mode)
        return;

    message[0] = (uint32)timestamp;
    message[1] = timestamp >> 32;
    message[2] = id;
    message[3] = d0;
    message[4] = d1;
    message[5] = d2;
    message[6] = d3;

    ram_log_count++;
    rpm_ULog_Fast7WordWrite(rpm_log, message);

    if(!(ram_log_count % RAM_LOG_LENGTH))
    {
        SWEVENT(RPM_HEAP_CHECK, get_free_heap_space()); 
    }
}

void rpm_log_event_to_qdss(tracer_event_id_t id, uint32 d0, uint32 d1, uint32 d2, uint32 d3)
{
    DALResult RetVal;

    uint32 message[5];
 
    if(rpm_log_silent_mode)
        return;

    message[0] = id;
    message[1] = d0;
    message[2] = d1;
    message[3] = d2;
    message[4] = d3;

    qdss_log_count++;
    RetVal = QDSSWriteOSTPacket(qdss_port,
                       QDSSOSTHeader(QDSS_SWEVT_ENTITY,QDSS_SWEVT_PROTOCOL),
                       (byte *)message,
                       sizeof(message));

    if(DAL_SUCCESS != RetVal)
    {
      rpm_log_event_to_ram(id, d0, d1, d2, d3);
    }

    if(!(qdss_log_count % QDSS_LOG_LENGTH))
    {
        SWEVENT(RPM_HEAP_CHECK, get_free_heap_space()); 
    }
}


void SWEVENT_log(tracer_event_id_t id, uint32 d0, uint32 d1, uint32 d2, uint32 d3)
{
    uint64 msg_id = 0;
    uint64 shifter = 1;
    msg_id = (shifter << (id%64));
    
    // 16, 64 bit (ints) used to store ids -> 1024 bits total
    CORE_VERIFY(id < 1024);
    
    if (qdss_output_enabled && (qdss_enabled_bmsk[id/64] & msg_id))
        rpm_log_event_to_qdss(id, d0, d1, d2, d3);
    else if (ram_enabled_bmsk[id/64] & msg_id)
        rpm_log_event_to_ram(id, d0, d1, d2, d3);
}

boolean qdss_is_swevent_enabled(tracer_event_id_t id) 
{
    uint64 msg_id = 0;
    uint64 shifter = 1;
    msg_id = (shifter << (id%64));
    
    // 16, 64 bit (ints) used to store ids -> 1024 bits total
    CORE_VERIFY(id < 1024);
    
    return (qdss_output_enabled && (qdss_enabled_bmsk[id/64] & msg_id));
}

static void swevent_update_qdss_clocks(void)
{
    unsigned i;
    for(i = 0; i < SWEVENT_TYPE_MAX; i++)
    {
        if (qdss_enabled_bmsk[i] != 0)
        {
            if(!qdss_clocks_enabled)
            {
                qdss_clocks_enabled = TRUE;
                QDSSEnableTrace();
            }
            return;
        }
    }

    if(qdss_clocks_enabled)
    {
        qdss_clocks_enabled = FALSE;
        QDSSDisableTrace();
    }
}

void swevent_cfg_bmsk(swevent_setting setting, swevent_type type, uint64 bmsk)
{
    if ((type < SWEVENT_TYPE_MAX) && (setting < SWEVENT_SETTING_UNDEFINED)) 
    {
        switch(setting)
        {
            case SWEVENT_SETTING_RAM:
                ram_enabled_bmsk[type] = bmsk;
                break;
            case SWEVENT_SETTING_QDSS:
                qdss_enabled_bmsk[type] = bmsk;
                break;
        }
    }

    swevent_update_qdss_clocks();
} 

void rpm_swevent_xlate(rpm_translation_info *info)
{
    unsigned               type, length;
    const char             *value_ptr;
    swevent_inrep *req = info->dest_buffer;

    // Read until we run out of KVPs
    while( !kvp_eof(info->new_kvps) )
    {
        if(!kvp_get( info->new_kvps, &type, &length, &value_ptr ))
        {
            abort();
        }

        CORE_VERIFY_PTR(value_ptr);

        switch(type)
        {
            case SWEVENT_REQ_BMSK:
                if (sizeof(uint64) != length)
                  abort();
                req->bmsk = *(uint64_t*)value_ptr;
                break;
            case SWEVENT_REQ_TYPE:
                if (sizeof(unsigned) != length)
                  abort();
                req->type = *(swevent_type*)value_ptr;
                break;
            case SWEVENT_REQ_SETT:
                if (sizeof(unsigned) != length)
                  abort();
                req->setting = *(swevent_setting*)value_ptr;
                break;
            default:
            {
                //unknown key
            }
        }
    }
}

/* This function is used for controlling RPM SW event and ETM trace */
static void rpm_trace_control(rpm_application_info *info)
{
    //for now, assume only one master so no need to query
    swevent_inrep *new_req = (swevent_inrep *)info->new_state;

    if (SWEVENT_SETTING_ETM==new_req->setting) {
       QDSSETMControl((uint32)new_req->type, new_req->bmsk); 
    }
    else {
       swevent_cfg_bmsk(new_req->setting, new_req->type, new_req->bmsk);
    }
}

void swevent_qdss_init(void)
{
    DALResult RetVal;
    RetVal = QDSSAllocPort(&qdss_port); 
    if(DAL_SUCCESS != RetVal) 
    {
        qdss_output_enabled = 0;
    }
    else
    {
        qdss_output_enabled = 1;
    }

    //this needs to happen later in the boot sequence so put it here instead of swevent_init
    rpm_register_resource(RPM_SWEVENT_REQ, 1, sizeof(swevent_inrep), rpm_swevent_xlate, rpm_trace_control, 0);
}

void swevent_init(void)
{
    uint8 i;

    // initialize log output settings - all events enabled to ram for now
    for (i = 0; i<SWEVENT_TYPE_MAX; i++)
    {
        ram_enabled_bmsk[i] = 0xFFFFFFFFFFFFFFFF;
        qdss_enabled_bmsk[i] = 0;
    }
    // disable logging to ram for system_db events, enable main system_db events to QDSS
    ram_enabled_bmsk[10] = 0x0FFFFFFFFFFFFFFF;
    ram_enabled_bmsk[11] = 0xFFFFFFFFFFFFFFC0;
	qdss_enabled_bmsk[11] = 0xE;

	const uint32 size_of_rpm_log = sizeof(uint32)*2304;
	char* const rpm_log_ptr = rpm_image_section_array[RPM_LOG].section_base;
	assert(&rpm_log_ptr[size_of_rpm_log]<=rpm_image_section_array[RPM_LOG].section_end);

    // create log with ULog 
    rpm_ULog_CreateNew(&rpm_log, "RPM External Log", rpm_log_ptr, NULL, size_of_rpm_log);
}

