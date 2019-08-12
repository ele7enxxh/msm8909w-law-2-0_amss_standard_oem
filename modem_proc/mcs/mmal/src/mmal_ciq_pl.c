/*!
  @file mmal_ciq_pl.c

  @brief
  This file implements the Carrier IQ Porting layer.

  This file provides a glue in between the MMAL module and Carrier IQ S/W.

  @ingroup per_implementation
*/

/*=============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/mmal/src/mmal_ciq_pl.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/02/11   rj      Updates to the SlaveBridge Implementation
09/15/11   rj      Added SMD Implementation for SlaveBridge
06/29/11   ars     Fix for IQPorting_GetGPSTime
05/26/11   ars     Changed IQPorting_GetGPSTime to return UNKNOWN
05/25/11   ars     Set IQPorting_SlaveBridgeWrite to a max of 4000 bytes
05/24/11   ars     Added stats for nv buffer, configuration string etc
05/10/11   ars     Fixed NV buffer size to 50K
05/06/11   ars     Added assert in IQPorting_Stop
05/03/11   ars     Fix for CR 286318. Moved IQBackend_Initialize to porting 
                   layer
04/26/11   ars     In mmal_ciq_pl_sio_rx_enqueue_cb, if mmal_msg_send returned
                   failure, then free DSM item
04/26/11   ars     Code review comments
04/26/11   ars     Fixed IQPorting_GetNonVolatileBuffer to retain content
                   upon reset
04/18/11   sm      Added support to read less number bytes by IO API compared
                   to what was read by sio port. Also, added couple of stats
                   to track number of bytes read from sio vs IO APIs.
04/15/11   sm      Call sio_control_transmit() from IQPorting_SlaveBridgeWrite
                   instead of sio_transmit()
04/14/11   sm      Fixed port_info in IQPorting_SlaveBridgeOpen as per 
                   recommendation and call sio_control_open instead of sio_open
04/14/11   sk      Fixed the functions to return IQ_PORTING_OK instead of
                   returning IQ_PORTING_NOT_SUPPORTED
04/12/11   ars     Added support for SlaveBridge APIs
04/12/11   ars     Fixed IQPorting_Log to output in correct format
04/11/11   ars     Removed sio_close and addded suport for b2b open/close
04/11/11   ars     Featurized IQ Porting Serial APIs
03/24/11   ars     Initial Checkin

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "mcs_variation.h"
#include <mmal_cfg.h>
#include <comdef.h>
#ifdef FEATURE_MMAL_CIQ
#include <mmal_util.h>
#include <mmal_ciq_pl.h>
#include <mmal_ciq_fend.h>
#include <mmal_msg_i.h>
#include <sio.h>
#include <smd_lite.h>
#include <modem_mem.h>
#include <mmal_ciq_pl.h>
#include <mobile.h>
#include <timetick.h>
#include <sys.h>
#include <cm.h>
#include <time_genoff.h>
#include <time_svc.h>
#include <time_conv.h>
#include <IQPortingTime.h>
#include <IQPortingIO.h>
#include <IQPortingBackend.h>
#include <IQPortingThreads.h>
#include <IQPortingConfiguration.h>
#include <IQPortingCommon.h>
#include <string.h>
#include <stringl/stringl.h>

/*=============================================================================

                   FORWARD DECLARATIONS

=============================================================================*/

LOCAL void mmal_ciq_pl_bkend_timer_exp_cb
(
  unsigned long cb_data
);
LOCAL void mmal_ciq_pl_sio_rx_enqueue_cb
(
  dsm_item_type**   dsm_item_ptr2ptr
);
LOCAL void mmal_ciq_pl_smd_rx_enqueue_cb
(
  smdl_handle_type port,
  smdl_event_type  event,
  void            *data
);
LOCAL void mmal_cm_ss_event_cb
(
  cm_ss_event_e_type           ss_event,     
  const cm_mm_ss_info_s_type   *ss_info_ptr  
);

/* On target these files reside in core\securemsm\smecom\com\inc */
extern void dbgprint(const char * pFormat, ...);

extern void dbgprint_format( const char * pFilePath, int nLineNum);

/*! @brief MMAL task rex control block
 * On target these declarations reside in service/task/task.c */
extern rex_tcb_type mmal_tcb;
/*=============================================================================

                    MACRO DEFINITIONS

=============================================================================*/
/*!
    @brief
    Converts GMT offset from increments of 15 minutes to offset in seconds.
    Note time zone from CM is offset from GMT in increments of 15 min

*/
/*===========================================================================*/
#define MMAL_CONV_TIME_ZONE_OFFSET_TO_SECS(time) (time*900);

/*===========================================================================*/
/*!
    @brief
    Size of the NV buffer and padding in bytes.

*/
/*===========================================================================*/

/* HACK: To be removed after IQPorting_GetNonVolatileBuffer works on CSFB */

#ifndef CIQ_BUFFER_SIZE
#define CIQ_BUFFER_SIZE 51200
#endif
/* end HACK */

#define MMAL_CIQ_NV_BUFF_SIZE      CIQ_BUFFER_SIZE
#define MMAL_CIQ_NV_BUFF_PAD_SIZE  32

/*===========================================================================*/
/*!
    @brief
    Conveys the number of 15 minute increments while adjusting for daylight 
    savings. If time is advanced by 1 hour then we denote that 
    as four 15 minute units.

*/
/*===========================================================================*/
#define MMAL_TZ_OFFSET_ADJ_ONE_HOUR     4
#define MMAL_TZ_OFFSET_ADJ_TWO_HOURS    8


/*=============================================================================*/
/*!
    @brief
    A unique ID for MMAL task

*/
/*===========================================================================*/
#define MMAL_TASK_ID 1234


/*===========================================================================*/
/*!
    @brief
    MMAL_MAX_RAM_LIMIT denotes the max memory IQ agent can ask for during heap 
    allocation

*/
/*===========================================================================*/

#define MMAL_MAX_RAM_LIMIT 300*1024

/*===========================================================================*/
/*!
    @brief
    MMAL_HW_REV_STRING_SIZE denotes the hardware rev of the phone 
    (i.e first 6 chars of M9200B-SCAQDBFD-3.0.222304T

*/
/*===========================================================================*/
#define MMAL_HW_REV_STRING_SIZE     6

/*===========================================================================*/
/*!
    @brief
    MMAL_CIQ_BRIDGE_MAX_WRITE_BYTES denotes the maximun number of bytes that
    can be written on the bridge in one shot. SDIO can only accomdate 
    this much in one write.

*/
/*===========================================================================*/
#define MMAL_CIQ_BRIDGE_MAX_WRITE_BYTES   8192

/*===========================================================================*/
/*!
    @brief
    MMAL_CIQ_SMD_MAX_MSG_SIZE denotes the maximun number of bytes that
    can be written on the bridge in one shot.
*/
/*===========================================================================*/
#define MMAL_CIQ_SMD_MAX_MSG_SIZE   8192   

/*===========================================================================*/
/*!
    @brief
    MMAL_CIQ_SMD_FIFO_SIZE denotes the size of the SMD FIFO Bridge.
*/
/*===========================================================================*/
#define MMAL_CIQ_SMD_FIFO_SIZE       8192 

/*=============================================================================

                   INTERNAL DEFINITIONS AND TYPES

=============================================================================*/

/*-----------------------------------------------------------------------------
  Type for SIOLIB port related information
-----------------------------------------------------------------------------*/
typedef struct
{
  sio_stream_id_type           stream_id;   /* SIO stream id */
  sio_port_id_type             port_id;     /* SIO port id */
  dsm_item_type*               rx_item_ptr; /* SIO Rx DSM item ptr */
  dsm_watermark_type           tx_wm;   /* SIO TX Watermark Q */
  q_type                       tx_queue;
  iq_io_callback_t             iq_io_callback_func; /* IQ Agent cb */
  void*                        iq_io_callback_data;
  /* IQ Agent attempted read when there is no data. Next time data comes
  from sio, we need to notify the IQ Agent to read again. */
  boolean                      iq_read_pending; 
  /* Keep track of DSM queue init so that it is done only once during init */
  boolean                      port_init;
  /* Keep track of sio_open so that it is done only once the first time */
  boolean                      port_opened;
  /* Keep track of IQPorting Close so that we flush data after that */
  boolean                      port_closed;
} mmal_ciq_pl_port_info_s;

typedef struct
{
  /* number of times sio_open is called */
  uint32                       sio_open_counter; 
  uint32                       io_read_buf_overflow_cnt;
  uint32                       sio_write_cnt;
  uint32                       sio_write_bytes_cnt;
  uint32                       sio_max_one_shot_write_byte_cnt;
  uint32                       pkt_drop_after_sio_close;
  uint32                       pkt_bytes_drop_after_sio_close;  
  uint32                       sio_read_cnt;
  uint32                       sio_read_bytes_cnt;
  uint32                       io_sio_read_cnt;
  uint32                       io_sio_read_bytes_cnt;
  /*! Number of times sio read happened when there is still a data avaialble
      to be read by CIQ */
  uint32                       sio_read_with_pending_data_cnt;
  uint32                       error_cnt;
  /*! Heap allocatie and dispose statistics */
  uint32                       heap_alloc_attempt_cnt;
  uint32                       heap_alloc_bytes_cnt;
  uint32                       heap_dispose_attempt_cnt;
  /*! Bytes returned by IQPorting_GetConfigInteger */
  uint32                       get_conf_int_max_ram_bytes;
  uint32                       nv_buffer_bytes;
  uint32                       gps_time_h32;
  uint32                       gps_time_l32;
}mmal_ciq_pl_stat_s;

/*-----------------------------------------------------------------------------
  Type for SMDLIB port related information
-----------------------------------------------------------------------------*/
typedef struct
{
  smdl_handle_type             smd_port_id;
  dsm_item_type*               rx_item_ptr; /* Internal Rx DSM item ptr */
  iq_io_callback_t             iq_io_callback_func; /* IQ Agent cb */
  void*                        iq_io_callback_data;
  /* IQ Agent attempted read when there is no data. Next time data comes
  from smd, we need to notify the IQ Agent to read again. */
  boolean                      iq_read_pending; 
  /* Keep track of if Port has been initialized or not */
  boolean                      port_init;
  /* Keep track of smdl_open so that it is done only once the first time */
  boolean                      port_opened;
  /* Keep track of IQPorting Close so that we flush data after that */
  boolean                      port_closed;
} mmal_ciq_pl_smd_port_info_s;

typedef struct
{
  /* number of times smdl_open is called */
  uint32                       smd_open_counter; 
  uint32                       io_read_buf_overflow_cnt;
  uint32                       smd_write_cnt;
  uint32                       smd_write_bytes_cnt;
  uint32                       smd_max_one_shot_write_byte_cnt;
  uint32                       pkt_drop_after_smd_close;
  uint32                       pkt_bytes_drop_after_smd_close;  
  uint32                       smd_read_cnt;
  uint32                       smd_read_bytes_cnt;
  uint32                       io_smd_read_cnt;
  uint32                       io_smd_read_bytes_cnt;
  /*! Number of times smd read happened when there is still a data avaialble
      to be read by CIQ */
  uint32                       smd_read_with_pending_data_cnt;
  uint32                       error_cnt;
  /*! Heap allocatie and dispose statistics */
  uint32                       heap_alloc_attempt_cnt;
  uint32                       heap_alloc_bytes_cnt;
  uint32                       heap_dispose_attempt_cnt;
  /*! Bytes returned by IQPorting_GetConfigInteger */
  uint32                       get_conf_int_max_ram_bytes;
  uint32                       nv_buffer_bytes;
  uint32                       gps_time_h32;
  uint32                       gps_time_l32;
}mmal_ciq_pl_smd_stat_s;

typedef struct
{
  /* Timer to kick start backend execution */
  rex_timer_type              mmal_ciq_pl_bkend_timer;
  /* Structure to keep track of port information */
  mmal_ciq_pl_port_info_s     port_info;
  /* Statistics structure for debugging purpose */
  mmal_ciq_pl_stat_s          pl_stats;
  /* CM client Id */
  cm_client_id_type           mmal_cm_client_id;
  /* Time zone offset from GMT in seconds */
  long                        time_zone_offset_secs;
  /* Is current time zone in DST */
  boolean                     is_daylight_savings;
  uint8                       ciq_nv_buff_padding[MMAL_CIQ_NV_BUFF_PAD_SIZE];
  /* Tracks if IQBackend and frontend Initialize was successful */
  boolean                      iq_agent_init_success;
  /* Structure to keep track of SMD port information */
  mmal_ciq_pl_smd_port_info_s  port_info_smd;
  /* Statistics structure for SMD debugging purpose */
  mmal_ciq_pl_smd_stat_s       pl_stats_smd;

}mmal_ciq_pl_s;

/* Main variable for this file */
mmal_ciq_pl_s    mmal_ciq_pl_info;

/* HACK: To be removed once IQPorting_GetNonVOlatileBuffer works on CSFB */

#ifndef CIQ_BUFFER_ADDRESS
uint8   mmal_ciq_nv_buffer[MMAL_CIQ_NV_BUFF_SIZE+(2*MMAL_CIQ_NV_BUFF_PAD_SIZE)];
uint8*  mmal_ciq_nv_buffer_ptr = mmal_ciq_nv_buffer;
#else  /* End HACK */

/* Pointer for NV buffer. This is initialized to that portion of memory that
is not reset by the OS upon phone resets */
uint8  *mmal_ciq_nv_buffer_ptr = (uint8 *)CIQ_BUFFER_ADDRESS;
#endif

/* DSM item to store Transmit Data */
dsm_item_type *mmal_ciq_txData = NULL;
/*=============================================================================

  FUNCTION:  mmal_ciq_pl_init

=============================================================================*/
/*!
    @brief
    Init routine of this file. This is called during MMAL task init and 
    it does the initialization of timers.

*/
/*===========================================================================*/
boolean mmal_ciq_pl_init()
{
  cm_client_status_e_type ret_val;
  int i;
  iq_porting_status_t     ciq_bkend_init_result;

  /* Definition for Backend Execute Timer */
  rex_def_timer_ex( &mmal_ciq_pl_info.mmal_ciq_pl_bkend_timer, 
                    mmal_ciq_pl_bkend_timer_exp_cb, 
                    NULL);

/* Needs to be addressed Later */
#if 0
  /* Register necessary events with CM */
  ret_val = cm_mm_client_ss_reg( mmal_ciq_pl_info.mmal_cm_client_id,
                                 mmal_cm_ss_event_cb,
                                 CM_CLIENT_EVENT_REG,
                                 CM_SS_EVENT_SRV_CHANGED,
                                 CM_SS_EVENT_SRV_CHANGED,
                                 NULL );
  if(ret_val != CM_CLIENT_OK)
  {
    MMAL_MSG_SPRINTF_1("cm_mm_client_ss_reg error %d",ret_val);
    return FALSE;
  }
#endif    

  /* Initialize the padding for NV buffer */
  for( i=0; i<MMAL_CIQ_NV_BUFF_PAD_SIZE; i = i + 4 )
  {
    mmal_ciq_pl_info.ciq_nv_buff_padding[i]   = 0xDE;
    mmal_ciq_pl_info.ciq_nv_buff_padding[i+1] = 0xAD;
    mmal_ciq_pl_info.ciq_nv_buff_padding[i+2] = 0xC0;
    mmal_ciq_pl_info.ciq_nv_buff_padding[i+3] = 0xFE;
  }

  /* Add the leading and trailing padding for the buffer */
  memcpy(mmal_ciq_nv_buffer_ptr, mmal_ciq_pl_info.ciq_nv_buff_padding, 
                             MMAL_CIQ_NV_BUFF_PAD_SIZE);

  memcpy(mmal_ciq_nv_buffer_ptr+MMAL_CIQ_NV_BUFF_SIZE-
                               MMAL_CIQ_NV_BUFF_PAD_SIZE+1,
                               mmal_ciq_pl_info.ciq_nv_buff_padding, 
                               MMAL_CIQ_NV_BUFF_PAD_SIZE);

  /* Initialize the port init flag */
  mmal_ciq_pl_info.port_info.port_init = FALSE;

  /*---------------------------------------------------------------------------
    Initialize IQ Backend. Call this after all porting layer related init has
    happened
  ---------------------------------------------------------------------------*/
  ciq_bkend_init_result = IQBackend_Initialize();

  /* Start off by setting flag to true. Reset to false upon failure case */
  mmal_ciq_pl_info.iq_agent_init_success = TRUE;

  if (ciq_bkend_init_result != IQ_PORTING_OK) 
  {
    MMAL_MSG_SPRINTF_1("IQBackend Initialize failed. %d", 
                                       ciq_bkend_init_result);

    mmal_ciq_pl_info.iq_agent_init_success = FALSE;

  }
  else
  {
    if (!mmal_ciq_fend_init() )
    {
      MMAL_MSG("IQFrontend Initialize failed. "   );
      mmal_ciq_pl_info.iq_agent_init_success = FALSE;
    }

  }

  return TRUE; 
}

/*=============================================================================

  FUNCTION:  mmal_ciq_pl_bkend_timer_exp_cb

=============================================================================*/
/*!
    @brief
    This function is called in Timer task context when the backend execute
    timer expires. We send a message to MMAL task and processing is done in
    MMAL task context.

*/
/*===========================================================================*/
void mmal_ciq_pl_bkend_timer_exp_cb
(
  unsigned long cb_data
)
{
  boolean ret_val;
  mmal_msg_hdr_s   mmal_ciq_bkend_timer_exp_req;

  MMAL_MSG_INIT_HDR(
      &mmal_ciq_bkend_timer_exp_req,
      MMAL_CIQ_BKEND_EXECUTE_TIMER_EXP_REQ );

  ret_val = mmal_msg_send (&mmal_ciq_bkend_timer_exp_req );  
  if(ret_val == FALSE)
  {
    MMAL_MSG_ERROR("Fail to post Backend Timer Exp Message");
    MMAL_ASSERT(0); //todo: Restart timer with prev cache value
  }
  return;
}

/*=============================================================================

  FUNCTION:  mmal_ciq_pl_process_bkend_timer_exp_req

=============================================================================*/
/*!
    @brief
    This function processes the MMAL_CIQ_BKEND_EXECUTE_TIMER_EXP_REQ message.
    It invokes IQBackend_Execute and restarts the timer.

*/
/*===========================================================================*/
void mmal_ciq_pl_process_bkend_timer_exp_req (void)
{
  mmal_ciq_pl_invoke_bkend_exec();
  return;
}


/*=============================================================================

  FUNCTION:  mmal_cm_ss_event_cb

=============================================================================*/
/*!
    @brief
    This function handles the callback from CM for CM_SS_EVENT_SRV_CHANGED
    event. This will post a command to MMAL task for processing.

*/
/*===========================================================================*/
void mmal_cm_ss_event_cb
(
  cm_ss_event_e_type           ss_event,        /* The event that occurred */
  const cm_mm_ss_info_s_type   *ss_info_ptr   /* Serving system information */
)
{
  mmal_time_zone_info_s    time_zone_info;
  boolean                  ret_val;

  if (ss_event != CM_SS_EVENT_SRV_CHANGED) 
  {
    MMAL_MSG_SPRINTF_1("CM SS callback received in wrong event: %d",ss_event);
    return;
  }

  if(ss_info_ptr == NULL)
  {
    MMAL_ASSERT(0);
  }

  /* Proceed only if time zone info is available from network */
  if(ss_info_ptr->mode_info.gw_info.mm_information.time_zone_avail == TRUE)
  {
    time_zone_info.time_zone = ss_info_ptr->mode_info.gw_info.
                                               mm_information.time_zone;

    time_zone_info.daylight_saving_adj_avail = ss_info_ptr->
                   mode_info.gw_info.mm_information.daylight_saving_adj_avail;

    time_zone_info.daylight_saving_adj = ss_info_ptr->
                    mode_info.gw_info.mm_information.daylight_saving_adj;

    /* Post message to MMAL task */
    MMAL_MSG_INIT_HDR( (mmal_msg_hdr_s*) &time_zone_info.hdr, 
                       MMAL_CIQ_PL_TIME_ZONE_CHG_REQ );

    ret_val = mmal_msg_send( (mmal_msg_hdr_s*) &time_zone_info.hdr );
    if(ret_val == FALSE)
    {
      MMAL_MSG_ERROR("Fail to post Backend Timer Exp Message");
    }
  }
  else
  {
    MMAL_MSG("CM SS event received but no time zone info");
  }
  return;
}
/*=============================================================================

  FUNCTION:  IQPorting_BackendWakeup

=============================================================================*/
/*!
    @brief
    This function requests that IQBackend_Execute() be invoked as soon as 
    possible

*/
/*===========================================================================*/
void IQPorting_BackendWakeup( IQ_VOID_PARAMLIST )
{
  rex_timer_cnt_type current_timer;

  /* Clear current timer */
  current_timer = rex_clr_timer(&mmal_ciq_pl_info.mmal_ciq_pl_bkend_timer);

  /* Call Backend Execute and restart next timer */
  mmal_ciq_pl_invoke_bkend_exec();
}


/*=============================================================================

  FUNCTION:  IQPorting_SystemInitialize

=============================================================================*/
/*!
    @brief
    This function is used to initialize any state and allocate any resources
    required in order to service System porting module functions.  The agent 
    will call this function before calling any other System porting module 
    functions.

   @return		The result of the attempt to initialize the System porting 
                module implementation.
   @retval	::IQ_PORTING_OK		initialization was successful
   @retval	::IQ_PORTING_ERROR	there was an error during initialization

*/
/*===========================================================================*/
iq_porting_status_t		IQPorting_SystemInitialize( IQ_VOID_PARAMLIST )
{
  return IQ_PORTING_OK;
}


/*=============================================================================

  FUNCTION:  IQPorting_Beep

=============================================================================*/
/*!
    @brief
    This function is used to play a beep. Since we dont have the capability
    on modem to invoke this, we will just print the message Beep

*/
/*===========================================================================*/
void       IQPorting_Beep( IQ_VOID_PARAMLIST )
{
  MMAL_MSG("Beep");
}

/*=============================================================================

  FUNCTION:  IQPorting_SystemShutdown

=============================================================================*/
/*!
    @brief
    This function is used to shutdown the system and release any resources
    acquired in initialize.  The agent will call this func during shutdown
    after it has finished with calling any System porting module functions.

   @return		The result of the attempt to shutdown the System porting 
                module implementation.
   @retval	::IQ_PORTING_OK		shutdown was successful
   @retval	::IQ_PORTING_ERROR	there was an error during shutdown

*/
/*===========================================================================*/
iq_porting_status_t		IQPorting_SystemShutdown( IQ_VOID_PARAMLIST )
{
  return IQ_PORTING_OK;
}
/*=============================================================================

  FUNCTION:  IQPorting_SerialInitialize

=============================================================================*/
/*!
    @brief
    This function is used to initialize any state and allocate any resources

   @return		The result of the attempt to initialize the System porting 
                module implementation.
   @retval	::IQ_PORTING_OK		initialization was successful
   @retval	::IQ_PORTING_ERROR	there was an error during initialization

*/
/*===========================================================================*/
iq_porting_status_t IQPorting_SerialInitialize( IQ_VOID_PARAMLIST )
{

#ifdef FEATURE_MMAL_CIQ_FUSION
  /* CIQ library expects serial APIs to return this value irrespective of its
   * supported or not */
  return IQ_PORTING_OK;
#else
  /* Initialize port_info with some default values */

  mmal_ciq_pl_info.port_info.port_id = SIO_PORT_USB_SER3;
  mmal_ciq_pl_info.port_info.rx_item_ptr = NULL;
  mmal_ciq_pl_info.port_info.iq_io_callback_func = NULL;
  mmal_ciq_pl_info.port_info.iq_io_callback_data = NULL;

  if(mmal_ciq_pl_info.port_info.port_init == FALSE)
  {
    MMAL_MSG("Init tx queue");
    dsm_queue_init(&mmal_ciq_pl_info.port_info.tx_wm, 
                   350000, 
                 &mmal_ciq_pl_info.port_info.tx_queue);

    mmal_ciq_pl_info.port_info.tx_wm.hi_watermark = 75000;
    mmal_ciq_pl_info.port_info.port_init = TRUE;
  }


  return IQ_PORTING_OK;
#endif /* FEATURE_MMAL_CIQ_FUSION */
}

/*=============================================================================

  FUNCTION:  IQPorting_SerialShutDown

=============================================================================*/
/*!
    @brief
    This function is used to shut down the serial implementation, freeing
    any resources allocated during IQPorting_SerialInitialize()

   @return		The result of the attempt to shutdown the System porting 
                module implementation.
   @retval	::IQ_PORTING_OK		initialization was successful
   @retval	::IQ_PORTING_ERROR	there was an error during initialization

*/
/*===========================================================================*/
iq_porting_status_t	IQPorting_SerialShutdown( IQ_VOID_PARAMLIST )
{
#ifdef FEATURE_MMAL_CIQ_FUSION
  /* CIQ library expects serial APIs to return this value irrespective of its
   * supported or not */
  return IQ_PORTING_OK;
#else
  return IQ_PORTING_OK;
#endif /* FEATURE_MMAL_CIQ_FUSION */
}


/*=============================================================================

  FUNCTION:  IQPorting_HeapAllocate

=============================================================================*/
/*!
    @brief
    This function is used to allocate a block of memory from the heap

    @param[in]	size 	the size (in bytes) of the requested allocation
  
    @return		A pointer to the new block, or NULL (zero) if a block of
  				the requested size is not available

*/
/*===========================================================================*/
void *      IQPorting_HeapAllocate
( 
  unsigned long size 
)
{
  void*      mmal_heap_ptr;

  MMAL_MSG_SPRINTF_1("IQ Agent called Heap Allocate for %d bytes",size);

  mmal_ciq_pl_info.pl_stats.heap_alloc_attempt_cnt++;

  if(size > MMAL_MAX_RAM_LIMIT)
  {
    MMAL_MSG_ERROR("Memory size exceeded max. Assert");
    MMAL_ASSERT(0);
  }

  mmal_heap_ptr = MMAL_MEM_ALLOC(size, MODEM_MEM_CLIENT_UTILS);

  if(mmal_heap_ptr != NULL)
  {
    /* Heap alloc was successful. Increment the byes */
    mmal_ciq_pl_info.pl_stats.heap_alloc_bytes_cnt = 
         mmal_ciq_pl_info.pl_stats.heap_alloc_bytes_cnt + size;
  }

  return mmal_heap_ptr;
}


/*=============================================================================

  FUNCTION:  IQPorting_HeapDispose

=============================================================================*/
/*!
    @brief
    This function is used to free a previously allocated block

    @param[in] 	oldBlock	a pointer to the allocation to be freed

*/
/*===========================================================================*/
void      IQPorting_HeapDispose( void *oldBlock )
{
  mmal_ciq_pl_info.pl_stats.heap_dispose_attempt_cnt++;
  MMAL_MEM_FREE(oldBlock, MODEM_MEM_CLIENT_UTILS);
}



/*=============================================================================

  FUNCTION:  IQPorting_SerialOpen

=============================================================================*/
/*!
    @brief
    This function is used to open a serial port to comunicate with CADeT

   @return		The result of open attempt.
   @retval	::IQ_PORTING_OK		initialization was successful
   @retval	::IQ_PORTING_ERROR	there was an error during initialization

*/
/*===========================================================================*/
iq_porting_status_t IQPorting_SerialOpen
(
  iq_io_callback_t    callbackFunc, 
  void*               callbackData
)
{
#ifdef FEATURE_MMAL_CIQ_FUSION
  /* CIQ library expects serial APIs to return this value irrespective of its
   * supported or not */
  return IQ_PORTING_OK;
#else
  sio_open_type           open_param;

  memset(&open_param, 0, sizeof(open_param));

  if( callbackFunc == NULL)
  {
     MMAL_MSG_ERROR("IQ Agent did not specify an I/O Callback func. Assert");
     MMAL_ASSERT(0);
  }

  if( mmal_ciq_pl_info.port_info.port_init == FALSE )
  {
    MMAL_MSG("Port open attempt without init. Assert");
    MMAL_ASSERT(0);
  }
      
  if( mmal_ciq_pl_info.port_info.port_opened == TRUE )
  {
    MMAL_MSG("SER3 port already openend.Skip");
    /* Cache the callback func and data to be used later */
    mmal_ciq_pl_info.port_info.iq_io_callback_func = callbackFunc;
    mmal_ciq_pl_info.port_info.iq_io_callback_data = callbackData;

    /* Increment counter (for ref purpose only) */
    mmal_ciq_pl_info.pl_stats.sio_open_counter++;

    mmal_ciq_pl_info.port_info.port_closed = FALSE;

    return IQ_PORTING_OK;
  }

  /*---------------------------------------------------------------------------
    Set the port id, Rx callback fn and other parameters for sio_open
  ---------------------------------------------------------------------------*/
  open_param.port_id = mmal_ciq_pl_info.port_info.port_id;
  open_param.stream_mode = SIO_DS_RAWDATA_MODE;
  open_param.rx_func_ptr = mmal_ciq_pl_sio_rx_enqueue_cb;
  open_param.tx_queue = &(mmal_ciq_pl_info.port_info.tx_wm);

  /* Store the stream ID */
  mmal_ciq_pl_info.port_info.stream_id = sio_open(&open_param);

  if( mmal_ciq_pl_info.port_info.stream_id == SIO_NO_STREAM_ID )
  {
    MMAL_MSG_ERROR("Unable to open SIO Port");
    return IQ_PORTING_ERROR;
  }

  /* Set port_opened and closed flag */
  mmal_ciq_pl_info.port_info.port_opened = TRUE;
  mmal_ciq_pl_info.port_info.port_closed = FALSE;

  MMAL_MSG_SPRINTF_1("Stream ID: %d",mmal_ciq_pl_info.port_info.stream_id);
  /* Cache the callback func and data to be used later */
  mmal_ciq_pl_info.port_info.iq_io_callback_func = callbackFunc;
  mmal_ciq_pl_info.port_info.iq_io_callback_data = callbackData;

  /* Increment counter (for ref purpose only) */
  mmal_ciq_pl_info.pl_stats.sio_open_counter++;

  return IQ_PORTING_OK;
#endif /* FEATURE_MMAL_CIQ_FUSION */
}

/*=============================================================================

  FUNCTION:  mmal_ciq_pl_sio_flush_tx_done_cb

=============================================================================*/
/*!
    @brief
    This function is called by SIO to indicate that tx flush has been 
    completed.

*/
/*===========================================================================*/
void mmal_ciq_pl_sio_flush_tx_done_cb( void )
{
  MMAL_MSG("SIO tx flush done");
}
/*=============================================================================

  FUNCTION:  IQPorting_SerialClose

=============================================================================*/
/*!
    @brief
    This function is used to close the serial port to comunicate with CADeT

   @return		The result of close attempt.
   @retval	::IQ_PORTING_OK		Close was successful
   @retval	::IQ_PORTING_ERROR	there was an error during closing

*/
/*===========================================================================*/
iq_porting_status_t	IQPorting_SerialClose( IQ_VOID_PARAMLIST )
{
#ifdef FEATURE_MMAL_CIQ_FUSION
  /* CIQ library expects serial APIs to return this value irrespective of its
   * supported or not */
  return IQ_PORTING_OK;
#else

  mmal_ciq_pl_info.port_info.port_closed = TRUE;

  if( mmal_ciq_pl_info.port_info.stream_id != SIO_NO_STREAM_ID)
  {
    MMAL_MSG("Flushing tx data before port close");
    sio_flush_tx(mmal_ciq_pl_info.port_info.stream_id, 
                 mmal_ciq_pl_sio_flush_tx_done_cb);
  }

  mmal_ciq_pl_info.port_info.iq_io_callback_func = NULL;
  mmal_ciq_pl_info.port_info.iq_io_callback_data = NULL;

  return IQ_PORTING_OK;
#endif /* FEATURE_MMAL_CIQ_FUSION */
}


/*=============================================================================

  FUNCTION:  mmal_ciq_pl_sio_rx_enqueue_cb

=============================================================================*/
/*!
    @brief
    This function is used to post SIO callback message to MMAL task

*/
/*===========================================================================*/
void mmal_ciq_pl_sio_rx_enqueue_cb
(
  dsm_item_type**   dsm_item_ptr2ptr
)
{
  boolean ret_val;

  mmal_pl_sio_rx_cb_s  mmal_pl_sio_rx_cb_info;

  mmal_pl_sio_rx_cb_info.dsm_item_ptr = *dsm_item_ptr2ptr;

  /* Post message to MMAL task */
  MMAL_MSG_INIT_HDR( (mmal_msg_hdr_s*) &mmal_pl_sio_rx_cb_info.hdr, 
                     MMAL_CIQ_PL_SIO_RX_CB_REQ );

  ret_val = mmal_msg_send( (mmal_msg_hdr_s*) &mmal_pl_sio_rx_cb_info.hdr );

  if(ret_val == FALSE)
  {
    MMAL_MSG_ERROR("Fail to post message to MMAL message q. Free DSM item");
    dsm_free_packet(dsm_item_ptr2ptr);
  }

  return;

}

/*=============================================================================

  FUNCTION:  mmal_ciq_pl_smd_rx_enqueue_cb

=============================================================================*/
/*!
    @brief
    This function is used to post SMD callback message to MMAL task

*/
/*===========================================================================*/
void mmal_ciq_pl_smd_rx_enqueue_cb
(
  smdl_handle_type port,
  smdl_event_type  event,
  void            *data
)
{
  boolean ret_val;

  mmal_pl_smd_rx_cb_s  mmal_pl_smd_rx_cb_info;

  mmal_pl_smd_rx_cb_info.event = event;
  mmal_pl_smd_rx_cb_info.port  = port;
  mmal_pl_smd_rx_cb_info.data  = data;

  /* Post message to MMAL task */
  MMAL_MSG_INIT_HDR( (mmal_msg_hdr_s*) &mmal_pl_smd_rx_cb_info.hdr, 
                     MMAL_CIQ_PL_SMD_RX_CB_REQ );

  ret_val = mmal_msg_send( (mmal_msg_hdr_s*) &mmal_pl_smd_rx_cb_info.hdr );

  if(ret_val == FALSE)
  {
    MMAL_MSG_ERROR("Fail to post message to MMAL message q.");
  }

  return;
}

/*=============================================================================
  FUNCTION:  mmal_ciq_pl_process_sio_rx_cb_req

=============================================================================*/
/*!
    @brief
    This function processes the MMAL_CIQ_PL_SIO_RX_CB_REQ message.
    It sends notification to IQ Agent that data is waiting to be read.

*/
/*===========================================================================*/
void mmal_ciq_pl_process_sio_rx_cb_req 
( 
  dsm_item_type*   dsm_item_ptr
)
{
  uint32 dsm_item_length;

  dsm_item_length = dsm_length_packet(dsm_item_ptr);
  
  if (mmal_ciq_pl_info.port_info.rx_item_ptr != NULL)
  {
    dsm_append(&mmal_ciq_pl_info.port_info.rx_item_ptr,
      &dsm_item_ptr);
  }
  else
  {
    mmal_ciq_pl_info.port_info.rx_item_ptr = dsm_item_ptr;
  }

  if ( mmal_ciq_pl_info.port_info.port_closed == TRUE ) 
  {
    /* Port is closed. Drop any data on rx */
    MMAL_MSG("Port is closed. Drop DSM item");
    dsm_free_packet(&mmal_ciq_pl_info.port_info.rx_item_ptr);
    mmal_ciq_pl_info.pl_stats.pkt_drop_after_sio_close++;
    mmal_ciq_pl_info.pl_stats.pkt_bytes_drop_after_sio_close += dsm_item_length;    
    return;
  }

  mmal_ciq_pl_info.pl_stats.sio_read_cnt++;
  mmal_ciq_pl_info.pl_stats.sio_read_bytes_cnt += dsm_item_length;


  if ( (mmal_ciq_pl_info.port_info.iq_io_callback_func != NULL) &&
      (mmal_ciq_pl_info.port_info.iq_read_pending == TRUE) )
  {
    mmal_ciq_pl_info.port_info.iq_read_pending = FALSE;
    /* Send notification to IQ Agent */
    mmal_ciq_pl_info.port_info.iq_io_callback_func(
        mmal_ciq_pl_info.port_info.iq_io_callback_data,
        IQ_PORTING_IOEVENT_READABLE,
        NULL,
        NULL
        );
  }
  else
  {
    mmal_ciq_pl_info.pl_stats.error_cnt++;
    MMAL_MSG("IQ Agent never attempted to read. Drop DSM item");
    dsm_free_packet(&mmal_ciq_pl_info.port_info.rx_item_ptr);
  }

  return;
}

/*=============================================================================

  FUNCTION:  mmal_ciq_pl_process_smd_rx_cb_req

=============================================================================*/
/*!
    @brief
    This function processes the MMAL_CIQ_PL_SMD_RX_CB_REQ message.
    It sends notification to IQ Agent that data is waiting to be read.

*/
/*===========================================================================*/
void mmal_ciq_pl_process_smd_rx_cb_req 
( 
  smdl_handle_type   port,
  smdl_event_type    event,
  void              *data
)
{
  dsm_item_type      *dsm_item_ptr = NULL;
  int32               peek_size;
  int32               bytes_read = 0;
  void*               rcv_buf;
  void*               rcv_save_buf;
  void*               tx_buf;
  void*               save_buf;
  uint32              smd_write_length, data_remaining;
  uint32              bytes_pushed, bytes_pulled, dsm_len_packet;


  ASSERT( NULL != port );

  switch( event )
  {
    /* data is ready to be read from the port */
    case SMDL_EVENT_READ:
    {
      /* check for data */
      peek_size = smdl_rx_peek( port );
      ASSERT( (int32)MMAL_CIQ_SMD_FIFO_SIZE >= peek_size );

      /* if there is data */
      if( peek_size > 0 )
      {
        /* get memory for recv buffer from the heap */
        rcv_buf = modem_mem_alloc( peek_size,
                                    MODEM_MEM_CLIENT_MCS_CRIT );

        rcv_save_buf = rcv_buf;

        /* read the data */
        bytes_read = smdl_read( port,
                                (uint32)peek_size,
                                rcv_buf,
                                (uint32)SMDL_READ_FLAGS_NONE );
        
        ASSERT( bytes_read == peek_size );

        /* Load DSM item with the data to be written*/
        bytes_read = dsm_pushdown_tail(&dsm_item_ptr,
                                        rcv_buf,
                                        (uint16)peek_size,
                                        MMAL_DSM_ITEM_POOL); 

        modem_mem_free( rcv_save_buf, MODEM_MEM_CLIENT_MCS_CRIT );
        ASSERT( bytes_read == peek_size );
      }
    }
    break;

    case SMDL_EVENT_WRITE:
    {
      dsm_len_packet = dsm_length_packet(mmal_ciq_txData);

      if( dsm_len_packet != 0)
      {
        tx_buf = modem_mem_alloc( dsm_len_packet,
                                  MODEM_MEM_CLIENT_MCS_CRIT );
        if (tx_buf == NULL) 
        {
          ERR_FATAL("Block could not be allocated of size %d",dsm_len_packet,0,0); 
        }

        save_buf = tx_buf;

        bytes_pulled = dsm_pullup_long(
                      &mmal_ciq_txData, 
                      tx_buf, 
                      dsm_len_packet);

        smd_write_length = smdl_write( mmal_ciq_pl_info.port_info_smd.smd_port_id, bytes_pulled, tx_buf, 0 );

        if(smd_write_length < dsm_len_packet) 
        {
          data_remaining = dsm_len_packet - smd_write_length;
          tx_buf = (uint8*)tx_buf + smd_write_length;
          bytes_pushed = dsm_pushdown_tail(&mmal_ciq_txData,
                                           tx_buf,
                                           (uint16)data_remaining,
                                           MMAL_DSM_ITEM_POOL); 

          ASSERT( bytes_pushed == data_remaining );
         }
          modem_mem_free( save_buf, MODEM_MEM_CLIENT_MCS_CRIT );
      }
      return;
    }
     break;

    default:
    {
      /* to handle all other cases like, SMDL_EVENT_NONE, 
           SMDL_EVENT_WRITE, SMDL_EVENT_CLOSE and misc */
    }
  }

  if (mmal_ciq_pl_info.port_info_smd.rx_item_ptr != NULL)
  {
    dsm_append(&mmal_ciq_pl_info.port_info_smd.rx_item_ptr,
      &dsm_item_ptr);
  }
  else
  {
    mmal_ciq_pl_info.port_info_smd.rx_item_ptr = dsm_item_ptr;
  }

  if ( mmal_ciq_pl_info.port_info_smd.port_closed == TRUE ) 
  {
    /* Port is closed. Drop any data on rx */
    MMAL_MSG("Port is closed. Drop DSM item");
    dsm_free_packet(&mmal_ciq_pl_info.port_info_smd.rx_item_ptr);
    mmal_ciq_pl_info.pl_stats_smd.pkt_drop_after_smd_close++;
    mmal_ciq_pl_info.pl_stats_smd.pkt_bytes_drop_after_smd_close += bytes_read;    
    return;
  }

  mmal_ciq_pl_info.pl_stats_smd.smd_read_cnt++;
  mmal_ciq_pl_info.pl_stats_smd.smd_read_bytes_cnt += bytes_read;


  if ( (mmal_ciq_pl_info.port_info_smd.iq_io_callback_func != NULL) &&
      (mmal_ciq_pl_info.port_info_smd.iq_read_pending == TRUE) )
  {
    mmal_ciq_pl_info.port_info_smd.iq_read_pending = FALSE;
    /* Send notification to IQ Agent */
    mmal_ciq_pl_info.port_info_smd.iq_io_callback_func(
        mmal_ciq_pl_info.port_info_smd.iq_io_callback_data,
        IQ_PORTING_IOEVENT_READABLE,
        NULL,
        NULL
        );
  }
  else
  {
    mmal_ciq_pl_info.pl_stats_smd.error_cnt++;
    MMAL_MSG("IQ Agent never attempted to read. Drop DSM item");
    dsm_free_packet(&mmal_ciq_pl_info.port_info_smd.rx_item_ptr);
  }

  return;
}

/*=============================================================================

  FUNCTION:  mmal_ciq_pl_process_time_zone_chg_req

=============================================================================*/
/*!
    @brief
    This function processes the MMAL_CIQ_PL_TIME_ZONE_CHG_REQ message.
    It updates the locally stored time zone. Note that CIQ states to exclude 
    any daylight saving offset in time zone. Offset should always correspond 
    to Standard time.

*/
/*===========================================================================*/
void mmal_ciq_pl_process_time_zone_chg_req
(
  int                             time_zone_offset,
  boolean                         daylight_saving_adj_avail,
  sys_daylight_saving_adj_e_type  daylight_saving_adj
)
{
  int adj_time_zone_offset;

  if(time_zone_offset == NULL ||
     daylight_saving_adj_avail == NULL ||
     daylight_saving_adj == NULL)
  {
    MMAL_MSG_ERROR("Invalid parametes in mmal_ciq_pl_process_time_zone_chg_req");
  }

  /* If no daylight saving is available, then it is straightforward to convert
  the time zone obtained from CM. Note time zone from CM is offset from GMT in
  increments of 15 min */

  if(!daylight_saving_adj_avail)
  {
    MMAL_MSG("Daylight saving info not included by network");
    mmal_ciq_pl_info.time_zone_offset_secs = 
                              MMAL_CONV_TIME_ZONE_OFFSET_TO_SECS(time_zone_offset);

    mmal_ciq_pl_info.is_daylight_savings = FALSE;
  }
  /* If daylight saving is available, we need to account for it */
  else
  {
    MMAL_MSG("Daylight saving info is included by network");
    switch (daylight_saving_adj) 
    {
    case SYS_DAY_SAV_ADJ_PLUS_ONE_HOUR:
        mmal_ciq_pl_info.is_daylight_savings = TRUE;
        /* Daylight saving has added 1 hour to the standard time. We need to
        subtract 1 hour to get the offset based on Standard time.  */
        adj_time_zone_offset = time_zone_offset - MMAL_TZ_OFFSET_ADJ_ONE_HOUR;

        mmal_ciq_pl_info.time_zone_offset_secs = 
                      MMAL_CONV_TIME_ZONE_OFFSET_TO_SECS(adj_time_zone_offset);
        break;

    case SYS_DAY_SAV_ADJ_PLUS_TWO_HOUR:
        mmal_ciq_pl_info.is_daylight_savings = TRUE;
        /* Daylight saving has added 2 hour to the standard time. We need to
        subtract 2 hours to get the offset based on Standard time.  */
        adj_time_zone_offset = time_zone_offset - MMAL_TZ_OFFSET_ADJ_TWO_HOURS;

        mmal_ciq_pl_info.time_zone_offset_secs = 
                      MMAL_CONV_TIME_ZONE_OFFSET_TO_SECS(adj_time_zone_offset);
        break;

    case SYS_DAY_SAV_ADJ_NO_ADJUSTMENT:
        mmal_ciq_pl_info.is_daylight_savings = FALSE;
        mmal_ciq_pl_info.time_zone_offset_secs = 
                          MMAL_CONV_TIME_ZONE_OFFSET_TO_SECS(time_zone_offset);
        break;
    default:
        break;
    }
  }

  return;
  
}
/*=============================================================================

  FUNCTION:  IQPorting_SerialRead

=============================================================================*/
/*!
    @brief
    This function is used to read from a serial port

   @return		The result of read attempt.
   @retval	::IQ_PORTING_OK		Read was successful
   @retval	::IQ_PORTING_ERROR	there was an error during reading
   @retval	::IQ_PORTING_IO_WOULDBLOCK	there is not data to read yet.
*/
/*===========================================================================*/
iq_porting_status_t IQPorting_SerialRead
( 
  void            *ioBuffer, 
  unsigned long   *ioLen 
)
{
#ifdef FEATURE_MMAL_CIQ_FUSION
  /* CIQ library expects serialRead to return this value irrespective of its
   * supported or not */
  return IQ_PORTING_IO_WOULDBLOCK;
#else
  uint32 dsm_packet_len;

  /* Validate params */
  if( ioBuffer == NULL )
  {
    MMAL_MSG_ERROR("Read buffer provided by IQ Agent is NULL.");
    return IQ_PORTING_INVALID_PARAM;
  }

  if(*ioLen == 0)
  {
    MMAL_MSG("No. of bytes to read provided by IQ Agent is 0. Skip read");
    return IQ_PORTING_INVALID_PARAM;
  }

  /* Check if we have data to read */
  if( mmal_ciq_pl_info.port_info.rx_item_ptr == NULL)
  {
    MMAL_MSG("There is no data in SIO rx to read");
    mmal_ciq_pl_info.port_info.iq_read_pending = TRUE;
    return IQ_PORTING_IO_WOULDBLOCK;
  }

  dsm_packet_len = dsm_length_packet(
          mmal_ciq_pl_info.port_info.rx_item_ptr);

  /* If there could be a possible buffer overflow during read, capture
  that in our stats */
  if( *ioLen <  (uint64)dsm_packet_len)
  {
    MMAL_MSG_ERROR("IQ Agent Read Buffer Overflow!!!");
    mmal_ciq_pl_info.pl_stats.io_read_buf_overflow_cnt++;
    MMAL_ASSERT(0);
  }

  /* Now we know we have data and a buffer to read it into. */
  *ioLen = dsm_extract(mmal_ciq_pl_info.port_info.rx_item_ptr,
                       0,
                       ioBuffer,
                       (uint16)*ioLen
                       );
  /* Record stats */
  mmal_ciq_pl_info.pl_stats.sio_read_cnt++;
  mmal_ciq_pl_info.pl_stats.sio_read_bytes_cnt += *ioLen;

  /* Free DSM item */
  dsm_free_packet(&mmal_ciq_pl_info.port_info.rx_item_ptr);

  return IQ_PORTING_OK;
#endif /* FEATURE_MMAL_CIQ_FUSION */
}

/*=============================================================================

  FUNCTION:  IQPorting_SerialWrite

=============================================================================*/
/*!
    @brief
    This function is used to write to a serial port

   @return		The result of write attempt.
   @retval	::IQ_PORTING_OK		Write was successful
   @retval	::IQ_PORTING_ERROR	there was an error during writing
   @retval	::IQ_PORTING_IO_WOULDBLOCK	port not ready to accespt write yet.
*/
/*===========================================================================*/
iq_porting_status_t IQPorting_SerialWrite
(
  IQ_CONST_PARAM void *ioBuffer, 
  unsigned long *ioLen
)
{
#ifdef FEATURE_MMAL_CIQ_FUSION
  /* CIQ library expects serial Write to return this value irrespective of its
   * supported or not */
  return IQ_PORTING_IO_WOULDBLOCK;
#else
  dsm_item_type*   tx_item_ptr=NULL;
  unsigned long    iq_ioLen;

  /* Validate incoming parameters */
  if(ioBuffer == NULL || *ioLen == 0)
  {
    MMAL_MSG_ERROR("Invalid Write parameters from IQ Agent");
    return IQ_PORTING_INVALID_PARAM;
  }

  /* Increment number of writes stat */
  mmal_ciq_pl_info.pl_stats.sio_write_cnt++;

  /* Cache ioLen to check if we may run out of DSM items */
  iq_ioLen = *ioLen;

  /* Load DSM item with the data to be written*/
  *ioLen = dsm_pushdown_tail(&tx_item_ptr,
                    ioBuffer,
                    (uint16)*ioLen,
                    MMAL_DSM_ITEM_POOL
                    );

  if( *ioLen == 0 )
  {
    MMAL_MSG_ERROR("Fail to write. Worte 0 bytes");
    MMAL_ASSERT(0);
  }

  if(*ioLen < iq_ioLen)
  {
    MMAL_MSG_ERROR("Ran out of DSM items while writing.");
    MMAL_ASSERT(0);
  }

//todo:reg hi watermark cb and inc count.

  /*---------------------------------------------------------------------------
    Send the input data to the underlying SIO device.  If we haven't been 
    assigned a valid stream ID by SIO, trash the input data.
  ---------------------------------------------------------------------------*/
  if( mmal_ciq_pl_info.port_info.stream_id != SIO_NO_STREAM_ID )
  {
    sio_transmit( mmal_ciq_pl_info.port_info.stream_id, 
                  tx_item_ptr );
    /* Add to total number of bytes */ 
    mmal_ciq_pl_info.pl_stats.sio_write_bytes_cnt += *ioLen;
  }
  else
  {
    dsm_free_packet ( &tx_item_ptr );
    MMAL_MSG_ERROR ("Transmitting data with bad stream id. Freeing packet");
    return IQ_PORTING_ERROR;
  }
  return IQ_PORTING_OK;
#endif /* FEATURE_MMAL_CIQ_FUSION */
}

/*=============================================================================

  FUNCTION:  mmal_ciq_pl_invoke_bkend_exec

=============================================================================*/
/*!
    @brief
    This function calls IQBackend_Execute to give the IQ Agent a chance to run
    When finished running, a timer value will be provided and the IQ Agent 
    expects another call to IQBackend_Execute upon timer expiry
*/
/*===========================================================================*/
void mmal_ciq_pl_invoke_bkend_exec( void )
{
  iq_porting_status_t           ciq_bkend_exec_result;
  iq_porting_timeout_t          timeout_val;

  ciq_bkend_exec_result = IQBackend_Execute(&timeout_val);

  if(ciq_bkend_exec_result == IQ_PORTING_OK)
  {
    /*-------------------------------------------------------------------------
     IQ_INFINITE_TIMEOUT indicates that the agent does not need to execute 
    again until some outside event occurs (such as a metric submission or 
    an I/O callback). So we dont need to start timer in that case.
    -------------------------------------------------------------------------*/
    if(timeout_val != IQ_INFINITE_TIMEOUT)
    {
      /* Start timer */
      (void)rex_set_timer(&mmal_ciq_pl_info.mmal_ciq_pl_bkend_timer,
                        (rex_timer_cnt_type)timeout_val);
    }
    else
    {
      MMAL_MSG("IQ Agent returned IQ_INFINITE_TIMEOUT. No need to start "
               "backend timer");
    }
  }
  else if( ciq_bkend_exec_result == IQ_PORTING_SHUTDOWN_COMPLETE )
  {
    MMAL_MSG("IQ Agent has Shutdown. No need to start backend timer");
  }
}

/*=============================================================================

  FUNCTION:  IQPorting_GetMillisecondTicks

=============================================================================*/
/*!
    @brief
    This function retrieves the value of a millisecond-incremented counter.
*/
/*===========================================================================*/
unsigned long      IQPorting_GetMillisecondTicks( IQ_VOID_PARAMLIST )
{
  return timetick_get_ms();
}

/*=============================================================================

  FUNCTION:  IQPorting_GetGPSTime

=============================================================================*/
/*!
    @brief
    This function retrieves the GPS time.
    @param[out]	high        high 32 bits of time in ms
 
    @param[out]	low			low 32 bits of time in ms
*/
/*===========================================================================*/
iq_time_result_t    IQPorting_GetGPSTime
( 
  unsigned long *high, 
  unsigned long *low 
)
{
#ifdef FEATURE_MMAL_CIQ_MODEM_GPS_TIME
  time_type                           time_value;

  /* Read the current time of day */
  (void) time_get( time_value );

  /* Convert time-stamp format to milliseconds */
  time_conv_to_ms( time_value );


  /* Extract the high and low 32 bits */
  *low = time_value[0];
  *high = time_value[1];

  mmal_ciq_pl_info.pl_stats.gps_time_h32 = *high;
  mmal_ciq_pl_info.pl_stats.gps_time_l32 = *low;

  return IQ_TIME_OK;
#else
  /* GPS Time not expected to be returned by modem as AP does it */
  return IQ_TIME_UNKNOWN;
#endif /* FEATURE_MMAL_CIQ_MODEM_GPS_TIME */
}


/*=============================================================================

  FUNCTION:  IQPorting_GetTimezone

=============================================================================*/
/*!
    @brief
    This function retrieves the current time zone of UE not including daylight
    savings offset.

    @param[out]	secondsOffset   set to the local timezone offset in number 
                                of seconds from GMT/UTC,
 								@b not including the daylight savings offset
 
    @param[out]	isDST			set to IQ_TRUE if we are currently in daylight 
                                savings time, IQ_FALSE otherwise
*/
/*===========================================================================*/
iq_time_result_t    IQPorting_GetTimezone
( 
  long *secondsOffset, 
  iq_boolean_t *isDST 
)
{
  MMAL_MSG("IQ Agent querying for Time zone");

  if (mmal_ciq_pl_info.is_daylight_savings) 
  {
    *isDST = IQ_TRUE;
  }
  else
  {
    *isDST = IQ_FALSE;
  }
  /* Report the offset from GMT in seconds */
  *secondsOffset = mmal_ciq_pl_info.time_zone_offset_secs;

  return IQ_TIME_OK;
}


/*=============================================================================

  FUNCTION:  IQPorting_Log

=============================================================================*/
/*!
    @brief
    This function displays or logs a string and continues execution
    @param fileName	the name of the source file from which this call is being made,
  					or NULL (zero) if the file name is not available
    @param lineNum	the source line number from which this call is being made, 
 					or zero if the line number is not available
    @param logText	the message to display or log
*/
/*===========================================================================*/
void   IQPorting_Log
(
  IQ_CONST_PARAM char* fileName, 
  unsigned long        lineNum, 
  IQ_CONST_PARAM char* logText
)
{
  if (NULL != fileName) 
  { 
    dbgprint_format(fileName, (int)lineNum); 
  } 
  else 
  { 
    dbgprint_format("UnkonwFile", (int)lineNum); 
  } 
  dbgprint("%s", logText);  
}

/*=============================================================================

  FUNCTION:  IQPorting_Stop

=============================================================================*/
/*!
    @brief
    This function stops execution, displays or logs the provided string, 
    and halts execution
    @param fileName	the name of the source file from which this call is being made,
  					or NULL (zero) if the file name is not available
    @param lineNum	the source line number from which this call is being made, 
 					or zero if the line number is not available
    @param logText	the message to display or log
*/
/*===========================================================================*/
void    IQPorting_Stop
(
  IQ_CONST_PARAM char* fileName, 
  unsigned long        lineNum, 
  IQ_CONST_PARAM char* stopText
)
{
  if (NULL != fileName) 
  { 
    dbgprint_format(fileName, (int)lineNum); 
  } 
  else 
  { 
    dbgprint_format("UnkonwFile", (int)lineNum); 
  } 
  dbgprint("STOPPED : %s", stopText); 

  //Commenting out for now and asserting instead for debugging purpose
  //rex_kill_task(&mmal_tcb); 

  MMAL_ASSERT(0);

}


/*=============================================================================

  FUNCTION:  IQPorting_CriticalSectionGetSize

=============================================================================*/
/*!
    @brief
    This function gets the size of critical section object

*/
/*===========================================================================*/
unsigned long       IQPorting_CriticalSectionGetSize( IQ_VOID_PARAMLIST )
{
  uint32 crit_sect_size;

  crit_sect_size = sizeof(mmal_crit_sect_s);

  return (crit_sect_size);
}

/*=============================================================================

  FUNCTION:  IQPorting_CriticalSectionInitialize

=============================================================================*/
/*!
    @brief
    This function Initializes an allocated critical section object.
    The space for the object will be allocated by the IQ Agent
    @param		cs	the critical section object
  
    @return		The result of the attempt to initialize the cs object
 	@retval	::IQ_PORTING_OK		initialization was successful
  	@retval	::IQ_PORTING_ERROR	there was an error during initialization
*/
/*===========================================================================*/
iq_porting_status_t   IQPorting_CriticalSectionInitialize
( 
  iq_critical_section_t cs 
)
{
  mmal_crit_sect_init((mmal_crit_sect_s*) cs);

  return IQ_PORTING_OK;
}

/*=============================================================================

  FUNCTION:  IQPorting_CriticalSectionUninitialize

=============================================================================*/
/*!
    @brief
    This function unnitializes an initialized critical section object.

    @param		cs	the critical section object
  
    @return		The result of the attempt to uninitialize the cs object
 	@retval	::IQ_PORTING_OK		uninitialization was successful
  	@retval	::IQ_PORTING_ERROR	there was an error during uninitialization
*/
/*===========================================================================*/
iq_porting_status_t	  IQPorting_CriticalSectionUninitialize
( 
  iq_critical_section_t cs 
)
{
  mmal_crit_sect_deinit((mmal_crit_sect_s*) cs);

  return IQ_PORTING_OK;
}


/*=============================================================================

  FUNCTION:  IQPorting_CriticalSectionEnter

=============================================================================*/
/*!
    @brief
    This function enters the given critical section.

    @param		cs	the critical section object
  
    @return		The result of the attempt to enter the cs
 	@retval	::IQ_PORTING_OK		enter was successful
  	@retval	::IQ_PORTING_ERROR	there was an error during entering
*/
/*===========================================================================*/
iq_porting_status_t   IQPorting_CriticalSectionEnter
( 
  iq_critical_section_t cs 
)
{
  mmal_crit_sect_enter((mmal_crit_sect_s*) cs);

  return IQ_PORTING_OK;
}

/*=============================================================================

  FUNCTION:  IQPorting_CriticalSectionLeave

=============================================================================*/
/*!
    @brief
    This function leaves the given critical section.

    @param		cs	the critical section object
  
    @return		The result of the attempt to leave the cs
 	@retval	::IQ_PORTING_OK		leave was successful
  	@retval	::IQ_PORTING_ERROR	there was an error during leaving
*/
/*===========================================================================*/
iq_porting_status_t   IQPorting_CriticalSectionLeave
( 
  iq_critical_section_t cs 
)
{
  mmal_crit_sect_leave((mmal_crit_sect_s*) cs);

  return IQ_PORTING_OK;
}

/*=============================================================================

  FUNCTION:  IQPorting_GetThreadID

=============================================================================*/
/*!
    @brief
    This function return a 32-bit ID that uniquely identifies the calling 
    thread.

    @return		This thread's unique identifier
*/
/*===========================================================================*/
iq_thread_id_t    IQPorting_GetThreadID( IQ_VOID_PARAMLIST )
{
  return MMAL_TASK_ID;
}


/*=============================================================================

  FUNCTION:  IQPorting_GetConfigurationInteger

=============================================================================*/
/*!
    @brief
    This function retrieves the value of a numerical configuration value.
    The desired value is indicated by the key parameter, and the result 
    should be placed into the long pointed to by value.
 
    @param[in]	key		the enumerated key indicating the value to be retrieved
    @param[out]	value	the long into which to copy the parameter value
  
    @return	The result of the attempt to retrieve the configuration parameter.
  		@retval	::IQ_PORTING_OK		the value was retrieved successfully
  		@retval	::IQ_PORTING_ERROR	there was an error retrieving the value

*/
/*===========================================================================*/
iq_porting_status_t	   IQPorting_GetConfigurationInteger
(	
  iq_config_int_key_t key,
  long *value
)
{
  if(key != IQ_CFG_INT_MAX_RAM_USAGE)
  {
    MMAL_MSG_SPRINTF_1("IQ Agent called IQPorting_GetConfigurationInteger for a"
             "irrelevant key %d", key);
    return IQ_PORTING_OK;
  }

  *value = MMAL_MAX_RAM_LIMIT;
  MMAL_MSG_SPRINTF_1("Configuration Int: Max ram usage: %d",*value);

  mmal_ciq_pl_info.pl_stats.get_conf_int_max_ram_bytes+= *value;

  return IQ_PORTING_OK;
}


/*=============================================================================

  FUNCTION:  IQPorting_GetConfigurationString

=============================================================================*/
/*!
    @brief
    This function retrieves the value of a configuration string.
    The desired value is indicated by the key parameter, and the result 
    should be placed into the long pointed to by value.
 
    @param[in]	key		the enumerated key indicating the value to be retrieved
    @param[out]	value	the long into which to copy the parameter value
  
    @return	The result of the attempt to retrieve the configuration parameter.
  		@retval	::IQ_PORTING_OK		the value was retrieved successfully
  		@retval	::IQ_PORTING_ERROR	there was an error retrieving the value

*/
/*===========================================================================*/
iq_porting_status_t  IQPorting_GetConfigurationString
(	
  iq_config_str_key_t key,
  char *value, 
  unsigned long maxSize
)
{
  if (key == IQ_CFG_STR_FIRMWARE_REV) 
  {
    if(maxSize >= strlen(mob_sw_rev)+1)
    {
      strlcpy(value, mob_sw_rev, strlen(mob_sw_rev)+1);
    }
    else
    {
      strlcpy(value, mob_sw_rev, maxSize);
    }
  }

  else if (key == IQ_CFG_STR_HARDWARE_REV) 
  {
    if(maxSize >= MMAL_HW_REV_STRING_SIZE+1)
    {
      strlcpy(value, mob_sw_rev, MMAL_HW_REV_STRING_SIZE+1);
    }
    else
    {
      strlcpy(value, mob_sw_rev, maxSize);
    }    
  }

  else 
  {
    MMAL_MSG_ERROR("Invalid key in GetConfigurationString");
    return IQ_PORTING_ERROR;
  }

  return IQ_PORTING_OK;
}

/*=============================================================================

  FUNCTION:  IQPorting_GetNonvolatileBuffer

=============================================================================*/
/*!
    @brief
    This function retrieves a reserved non-volatile memory buffer to be used 
    for crash information recovery

   @param[out]	outPtr			should be set to point to the start of the 
                                buffer
   @param[out]	outSize			should be set to the size of the returned 
                                buffer
   @param[out]	outKnownDirty	should be set to IQ_TRUE if the buffer 
                                is known to not contain
  								useful data, or IQ_FALSE otherwise
  
   @return	The result of the attempt to retrieve the configuration parameter.
  		@retval	::IQ_PORTING_OK		the buffer was retrieved successfully
  		@retval	::IQ_PORTING_ERROR	there was an error retrieving the buffer
*/
/*===========================================================================*/
iq_porting_status_t IQPorting_GetNonvolatileBuffer
(
  void **outPtr, 
  unsigned long *outSize, 
  iq_boolean_t *outKnownDirty 
)
{
  /* Exclude the 32 bytes leading and trailing padding of the buffer */
  *outSize = MMAL_CIQ_NV_BUFF_SIZE - 2*MMAL_CIQ_NV_BUFF_PAD_SIZE;
  *outKnownDirty = IQ_FALSE;

  /* Exclude the leading padding bytes while returning the buffer address */
  *outPtr = mmal_ciq_nv_buffer_ptr + MMAL_CIQ_NV_BUFF_PAD_SIZE;
  
  MMAL_MSG_SPRINTF_2("NV buffer size: %d, NV buffer address: 0x%x",*outSize, *outPtr);

  mmal_ciq_pl_info.pl_stats.nv_buffer_bytes+= *outSize;

  return IQ_PORTING_OK;
}

/*---------------------------------------------------------------------------
Stubs for compilation to work for now
--------------------------------------------------------------------------*/

void  IQPorting_DisplayMessage( const char *msgString )
{
  return;
}

/*=============================================================================

  FUNCTION:  IQPorting_SIOSlaveBridgeInitialize

=============================================================================*/
/*!
    @brief
    This function is used to initialize any state and allocate any resources

   @return		The result of the attempt to initialize the System porting 
                module implementation.
   @retval	::IQ_PORTING_OK		initialization was successful
   @retval	::IQ_PORTING_ERROR	there was an error during initialization

*/
/*===========================================================================*/
iq_porting_status_t IQPorting_SIOSlaveBridgeInitialize( IQ_VOID_PARAMLIST )
{
#ifdef FEATURE_MMAL_CIQ_FUSION
  /* Initialize port_info with some default values */
  mmal_ciq_pl_info.port_info.port_id = SIO_PORT_SDIOC_CIQ;
  mmal_ciq_pl_info.port_info.rx_item_ptr = NULL;
  mmal_ciq_pl_info.port_info.iq_io_callback_func = NULL;
  mmal_ciq_pl_info.port_info.iq_io_callback_data = NULL;

  if(mmal_ciq_pl_info.port_info.port_init == FALSE)
  {
    MMAL_MSG("Init tx queue");
    dsm_queue_init(&mmal_ciq_pl_info.port_info.tx_wm, 
                   350000, 
                   &mmal_ciq_pl_info.port_info.tx_queue);

    mmal_ciq_pl_info.port_info.tx_wm.hi_watermark = 75000;
    mmal_ciq_pl_info.port_info.port_init = TRUE;
  }


  return IQ_PORTING_OK;
#else 
  return IQ_PORTING_NOT_IMPLEMENTED;
#endif /* FEATURE_MMAL_CIQ_FUSION */
}

/*=============================================================================

  FUNCTION:  IQPorting_SIOSlaveBridgeShutdown

=============================================================================*/
/*!
    @brief
    This function is used to shut down the serial implementation, freeing
    any resources allocated during IQPorting_SlaveBridgeInitialize()

   @return		The result of the attempt to shutdown the System porting 
                module implementation.
   @retval	::IQ_PORTING_OK		initialization was successful
   @retval	::IQ_PORTING_ERROR	there was an error during initialization

*/
/*===========================================================================*/
iq_porting_status_t	IQPorting_SIOSlaveBridgeShutdown( IQ_VOID_PARAMLIST )
{
#ifdef FEATURE_MMAL_CIQ_FUSION
  return IQ_PORTING_OK;
#else 
  return IQ_PORTING_NOT_IMPLEMENTED;
#endif /* FEATURE_MMAL_CIQ_FUSION */
}


/*=============================================================================

  FUNCTION:  IQPorting_SIOSlaveBridgeOpen

=============================================================================*/
/*!
    @brief
    This function is used to open a SDIO port to interafce with APPS proc

   @return		The result of open attempt.
   @retval	::IQ_PORTING_OK		initialization was successful
   @retval	::IQ_PORTING_ERROR	there was an error during initialization

*/
/*===========================================================================*/
iq_porting_status_t IQPorting_SIOSlaveBridgeOpen
(
 iq_io_callback_t callbackFunc, 
 void *callbackData
)
{
#ifdef FEATURE_MMAL_CIQ_FUSION
  sio_open_type           open_param;
  memset(&open_param, 0, sizeof(open_param));

  if( callbackFunc == NULL)
  {
     MMAL_MSG_ERROR("IQ Agent did not specify an I/O Callback func. Assert");
     MMAL_ASSERT(0);
  }
  if( mmal_ciq_pl_info.port_info.port_init == FALSE )
  {
    MMAL_MSG("Port open attempt without init. Assert");
    MMAL_ASSERT(0);
  }
      
  if( mmal_ciq_pl_info.port_info.port_opened == TRUE )
  {
    MMAL_MSG("SlaveBridge port already openend.Skip");
    /* Cache the callback func and data to be used later */
    mmal_ciq_pl_info.port_info.iq_io_callback_func = callbackFunc;
    mmal_ciq_pl_info.port_info.iq_io_callback_data = callbackData;
    /* Increment counter (for ref purpose only) */
    mmal_ciq_pl_info.pl_stats.sio_open_counter++;
    mmal_ciq_pl_info.port_info.port_closed = FALSE;
    return IQ_PORTING_OK;
  }
  /*---------------------------------------------------------------------------
    Set the port id, Rx callback fn and other parameters for sio_open
  ---------------------------------------------------------------------------*/
  open_param.port_id = mmal_ciq_pl_info.port_info.port_id;
  open_param.stream_mode = SIO_GENERIC_MODE;// as per sdio sio test code
  open_param.rx_func_ptr = mmal_ciq_pl_sio_rx_enqueue_cb;
  open_param.tx_queue = &(mmal_ciq_pl_info.port_info.tx_wm);
  open_param.rx_queue = NULL;
  open_param.tail_char_used = FALSE;
  open_param.rx_flow        = SIO_FCTL_OFF;
  open_param.tx_flow        = SIO_FCTL_OFF;
  /* Store the stream ID */
  mmal_ciq_pl_info.port_info.stream_id = sio_control_open(&open_param);
  if( mmal_ciq_pl_info.port_info.stream_id == SIO_NO_STREAM_ID )
  {
    MMAL_MSG_ERROR("Unable to open SIO Port");
    mmal_ciq_pl_info.pl_stats.error_cnt++;
    return IQ_PORTING_ERROR;
  }
  /* Set port_opened and closed flag */
  mmal_ciq_pl_info.port_info.port_opened = TRUE;
  mmal_ciq_pl_info.port_info.port_closed = FALSE;
  MMAL_MSG_SPRINTF_1("Stream ID: %d",mmal_ciq_pl_info.port_info.stream_id);
  /* Cache the callback func and data to be used later */
  mmal_ciq_pl_info.port_info.iq_io_callback_func = callbackFunc;
  mmal_ciq_pl_info.port_info.iq_io_callback_data = callbackData;
  /* Increment counter (for ref purpose only) */
  mmal_ciq_pl_info.pl_stats.sio_open_counter++;
  return IQ_PORTING_OK;
#else 
  return IQ_PORTING_NOT_IMPLEMENTED;
#endif /* FEATURE_MMAL_CIQ_FUSION */
}

/*=============================================================================

  FUNCTION:  IQPorting_SIOSlaveBridgeClose

=============================================================================*/
/*!
    @brief
    This function is used to close the SDIO port

   @return		The result of close attempt.
   @retval	::IQ_PORTING_OK		Close was successful
   @retval	::IQ_PORTING_ERROR	there was an error during closing

*/
/*===========================================================================*/
iq_porting_status_t	IQPorting_SIOSlaveBridgeClose( IQ_VOID_PARAMLIST )
{
#ifdef FEATURE_MMAL_CIQ_FUSION
  mmal_ciq_pl_info.port_info.port_closed = TRUE;

  if( mmal_ciq_pl_info.port_info.stream_id != SIO_NO_STREAM_ID)
  {
    MMAL_MSG("Flushing tx data before port close");
    sio_flush_tx(mmal_ciq_pl_info.port_info.stream_id, 
                 mmal_ciq_pl_sio_flush_tx_done_cb);
  }

  mmal_ciq_pl_info.port_info.iq_io_callback_func = NULL;
  mmal_ciq_pl_info.port_info.iq_io_callback_data = NULL;

  return IQ_PORTING_OK;
#else
  return IQ_PORTING_NOT_IMPLEMENTED;
#endif /* FEATURE_MMAL_CIQ_FUSION */
}


/*=============================================================================

  FUNCTION:  IQPorting_SIOSlaveBridgeRead

=============================================================================*/
/*!
    @brief
    This function is used to read from SDIO port

   @return		The result of read attempt.
   @retval	::IQ_PORTING_OK		Read was successful
   @retval	::IQ_PORTING_ERROR	there was an error during reading
   @retval	::IQ_PORTING_IO_WOULDBLOCK	there is not data to read yet.
*/
/*===========================================================================*/
iq_porting_status_t IQPorting_SIOSlaveBridgeRead
( 
  void *ioBuffer, 
  unsigned long *ioLen 
)
{
#ifdef FEATURE_MMAL_CIQ_FUSION
  uint32 dsm_packet_len;

  /* Validate params */
  if( ioBuffer == NULL )
  {
    MMAL_MSG_ERROR("Read buffer provided by IQ Agent is NULL.");
    return IQ_PORTING_INVALID_PARAM;
  }

  if(*ioLen == 0)
  {
    MMAL_MSG("No. of bytes to read provided by IQ Agent is 0. Skip read");
    return IQ_PORTING_INVALID_PARAM;
  }

  /* Check if we have data to read */
  if( mmal_ciq_pl_info.port_info.rx_item_ptr == NULL)
  {
    MMAL_MSG("There is no data in SIO rx to read");
    mmal_ciq_pl_info.port_info.iq_read_pending = TRUE;
    return IQ_PORTING_IO_WOULDBLOCK;
  }

  dsm_packet_len = dsm_length_packet(
          mmal_ciq_pl_info.port_info.rx_item_ptr);

  /* If there could be a possible buffer overflow during read, capture
  that in our stats */
  if( *ioLen <  (uint64)dsm_packet_len)
  {
    mmal_ciq_pl_info.pl_stats.io_read_buf_overflow_cnt++;
    *ioLen = dsm_pullup_long(
      &mmal_ciq_pl_info.port_info.rx_item_ptr, 
      ioBuffer, 
      *ioLen);
  }
  else
  {
    *ioLen = dsm_pullup_long(
      &mmal_ciq_pl_info.port_info.rx_item_ptr, 
      ioBuffer, 
      dsm_packet_len);
    /* dsm item is expected to be freed at this point since we pulled out
       all the data bytes */
    MMAL_ASSERT(mmal_ciq_pl_info.port_info.rx_item_ptr == NULL);
  }

  /* Record stats */
  mmal_ciq_pl_info.pl_stats.io_sio_read_cnt++;
  mmal_ciq_pl_info.pl_stats.io_sio_read_bytes_cnt += *ioLen;

  return IQ_PORTING_OK;
#else
  return IQ_PORTING_NOT_IMPLEMENTED;
#endif /* FEATURE_MMAL_CIQ_FUSION */
}

/*=============================================================================

  FUNCTION:  IQPorting_SIOSlaveBridgeWrite

=============================================================================*/
/*!
    @brief
    This function is used to write to a SDIO port

   @return		The result of write attempt.
   @retval	::IQ_PORTING_OK		Write was successful
   @retval	::IQ_PORTING_ERROR	there was an error during writing
   @retval	::IQ_PORTING_IO_WOULDBLOCK	port not ready to accespt write yet.
*/
/*===========================================================================*/
iq_porting_status_t IQPorting_SIOSlaveBridgeWrite
(
  IQ_CONST_PARAM void *ioBuffer, 
  unsigned long *ioLen
)
{
#ifdef FEATURE_MMAL_CIQ_FUSION
  dsm_item_type*   tx_item_ptr=NULL;
  unsigned long iq_ioLen;
  unsigned long loop_write_length, pushdown_write_length,total_pushdown_bytes;

  total_pushdown_bytes=0;
  pushdown_write_length=0;
  loop_write_length=0;

  if (*ioLen > mmal_ciq_pl_info.pl_stats.sio_max_one_shot_write_byte_cnt) {
    mmal_ciq_pl_info.pl_stats.sio_max_one_shot_write_byte_cnt = *ioLen;
  }
  /* Validate incoming parameters */
  if(ioBuffer == NULL || *ioLen == 0)
  {
    MMAL_MSG_ERROR("Invalid Write parameters from IQ Agent");
    return IQ_PORTING_INVALID_PARAM;
  }

  /* Increment number of writes stat */
  mmal_ciq_pl_info.pl_stats.sio_write_cnt++;

  /* Cache ioLen to check if we may run out of DSM items */
  iq_ioLen = *ioLen;

  /* We write a max of MMAL_CIQ_BRIDGE_MAX_WRITE_BYTES in one attempt
  and loop till we have consumed all bytes from CIQ */
  while (iq_ioLen != 0) 
  {
    if (iq_ioLen > MMAL_CIQ_BRIDGE_MAX_WRITE_BYTES) 
    {
      loop_write_length = MMAL_CIQ_BRIDGE_MAX_WRITE_BYTES;
      iq_ioLen -= MMAL_CIQ_BRIDGE_MAX_WRITE_BYTES;
    }
    else
    {
      loop_write_length = iq_ioLen;
      iq_ioLen = 0;
    }
  /* Load DSM item with the data to be written*/
    pushdown_write_length = dsm_pushdown_tail(&tx_item_ptr,
                    ioBuffer,
                        (uint16)loop_write_length,
                    MMAL_DSM_ITEM_POOL
                    );

    total_pushdown_bytes+=pushdown_write_length;

    if(pushdown_write_length != loop_write_length)
  {
    MMAL_MSG_ERROR("Ran out of DSM items while writing.");
    MMAL_ASSERT(0);
  }

    ioBuffer = (char *)ioBuffer + loop_write_length;
//todo:reg hi watermark cb and inc count.

  /*---------------------------------------------------------------------------
    Send the input data to the underlying SIO device.  If we haven't been 
    assigned a valid stream ID by SIO, trash the input data.
  ---------------------------------------------------------------------------*/
  if( mmal_ciq_pl_info.port_info.stream_id != SIO_NO_STREAM_ID )
  {
    sio_control_transmit( mmal_ciq_pl_info.port_info.stream_id, 
                  tx_item_ptr );
    /* Add to total number of bytes */ 
      mmal_ciq_pl_info.pl_stats.sio_write_bytes_cnt += loop_write_length;
  }
  else
  {
    dsm_free_packet ( &tx_item_ptr );
      *ioLen=0;
      mmal_ciq_pl_info.pl_stats.error_cnt++;
    MMAL_MSG_ERROR ("Transmitting data with bad stream id. Freeing packet");
    return IQ_PORTING_ERROR;
  }
    tx_item_ptr = NULL;
  }
  MMAL_ASSERT(total_pushdown_bytes == *ioLen);
  return IQ_PORTING_OK;

#else
  return IQ_PORTING_NOT_IMPLEMENTED;
#endif /* FEATURE_MMAL_CIQ_FUSION */
}

/*=============================================================================

  FUNCTION:  IQPorting_SMDSlaveBridgeInitialize

=============================================================================*/
/*!
    @brief
    This function is used to initialize any state and allocate any resources

   @return		The result of the attempt to initialize the System porting 
                module implementation.
   @retval	::IQ_PORTING_OK		initialization was successful
   @retval	::IQ_PORTING_ERROR	there was an error during initialization

*/
/*===========================================================================*/
iq_porting_status_t IQPorting_SlaveBridgeInitialize( IQ_VOID_PARAMLIST )
{
#ifdef FEATURE_MMAL_CIQ_FUSION
  /* Initialize port_info_smd with some default values */
  mmal_ciq_pl_info.port_info_smd.smd_port_id = NULL;
  mmal_ciq_pl_info.port_info_smd.rx_item_ptr = NULL;
  mmal_ciq_pl_info.port_info_smd.iq_io_callback_func = NULL;
  mmal_ciq_pl_info.port_info_smd.iq_io_callback_data = NULL;
  mmal_ciq_pl_info.port_info_smd.port_init = TRUE;

  return IQ_PORTING_OK;
#else 
  return IQ_PORTING_NOT_IMPLEMENTED;
#endif /* FEATURE_MMAL_CIQ_FUSION */
}

/*=============================================================================

  FUNCTION:  IQPorting_SMDSlaveBridgeShutdown

=============================================================================*/
/*!
    @brief
    This function is used to shut down the SMD implementation, freeing
    any resources allocated during IQPorting_SMDSlaveBridgeInitialize()

   @return		The result of the attempt to shutdown the System porting 
                module implementation.
   @retval	::IQ_PORTING_OK		initialization was successful
   @retval	::IQ_PORTING_ERROR	there was an error during initialization

*/
/*===========================================================================*/
iq_porting_status_t	IQPorting_SlaveBridgeShutdown( IQ_VOID_PARAMLIST )
{
#ifdef FEATURE_MMAL_CIQ_FUSION
  return IQ_PORTING_OK;
#else 
  return IQ_PORTING_NOT_IMPLEMENTED;
#endif /* FEATURE_MMAL_CIQ_FUSION */
}


/*=============================================================================

  FUNCTION:  IQPorting_SMDSlaveBridgeOpen

=============================================================================*/
/*!
    @brief
    This function is used to open a SMD port to interface with APPS proc

   @return		The result of open attempt.
   @retval	::IQ_PORTING_OK		initialization was successful
   @retval	::IQ_PORTING_ERROR	there was an error during initialization

*/
/*===========================================================================*/
iq_porting_status_t IQPorting_SlaveBridgeOpen
(
 iq_io_callback_t callbackFunc, 
 void *callbackData
)
{
#ifdef FEATURE_MMAL_CIQ_FUSION
  
  smdl_handle_type        open_handle;
  memset(&open_handle, 0, sizeof(open_handle));

  if( callbackFunc == NULL)
  {
    MMAL_MSG_ERROR("IQ Agent did not specify an I/O Callback func. Assert");
    MMAL_ASSERT(0);
  }
  if( mmal_ciq_pl_info.port_info_smd.port_init == FALSE )
  {
    MMAL_MSG("Port open attempt without init. Assert");
    MMAL_ASSERT(0);
  }
      
  if( mmal_ciq_pl_info.port_info_smd.port_opened == TRUE )
  {
    MMAL_MSG("SlaveBridge port already openend.Skip");
    /* Cache the callback func and data to be used later */
    mmal_ciq_pl_info.port_info_smd.iq_io_callback_func = callbackFunc;
    mmal_ciq_pl_info.port_info_smd.iq_io_callback_data = callbackData;
    /* Increment counter (for ref purpose only) */
    mmal_ciq_pl_info.pl_stats_smd.smd_open_counter++;
    mmal_ciq_pl_info.port_info_smd.port_closed = FALSE;
    return IQ_PORTING_OK;
  }

  /*---------------------------------------------------------------------------
    Open the SMD Port
  ---------------------------------------------------------------------------*/

  open_handle = smdl_open( "MBALBRIDGE", SMD_APPS_MODEM , SMDL_OPEN_FLAGS_MODE_STREAMING,
                           MMAL_CIQ_SMD_FIFO_SIZE,
                           mmal_ciq_pl_smd_rx_enqueue_cb, NULL );

  if( open_handle == NULL )
  {
    MMAL_MSG_ERROR("Unable to open SMD Port");
    mmal_ciq_pl_info.pl_stats_smd.error_cnt++;
    return IQ_PORTING_ERROR;
  }

  mmal_ciq_pl_info.port_info_smd.smd_port_id = open_handle;

  /* Set port_opened and closed flag */
  mmal_ciq_pl_info.port_info_smd.port_opened = TRUE;
  mmal_ciq_pl_info.port_info_smd.port_closed = FALSE;

  /* Cache the callback func and data to be used later */
  mmal_ciq_pl_info.port_info_smd.iq_io_callback_func = callbackFunc;
  mmal_ciq_pl_info.port_info_smd.iq_io_callback_data = callbackData;

  /* Increment counter (for ref purpose only) */
  mmal_ciq_pl_info.pl_stats_smd.smd_open_counter++;
  return IQ_PORTING_OK;

#else 
  return IQ_PORTING_NOT_IMPLEMENTED;
#endif /* FEATURE_MMAL_CIQ_FUSION */
}

/*=============================================================================

  FUNCTION:  IQPorting_SMDSlaveBridgeClose

=============================================================================*/
/*!
    @brief
    This function is used to close the SMD port

   @return		The result of close attempt.
   @retval	::IQ_PORTING_OK		Close was successful
   @retval	::IQ_PORTING_ERROR	there was an error during closing

*/
/*===========================================================================*/
iq_porting_status_t	IQPorting_SlaveBridgeClose( IQ_VOID_PARAMLIST )
{
#ifdef FEATURE_MMAL_CIQ_FUSION
  int32 status;

  if(mmal_ciq_pl_info.port_info_smd.port_opened == TRUE )
  {
    status = smdl_close( mmal_ciq_pl_info.port_info_smd.smd_port_id );
    ASSERT( 0 == status );
  }

  mmal_ciq_pl_info.port_info_smd.iq_io_callback_func = NULL;
  mmal_ciq_pl_info.port_info_smd.iq_io_callback_data = NULL;
  mmal_ciq_pl_info.port_info_smd.port_closed = TRUE;

  return IQ_PORTING_OK;
#else
  return IQ_PORTING_NOT_IMPLEMENTED;
#endif /* FEATURE_MMAL_CIQ_FUSION */
}


/*=============================================================================

  FUNCTION:  IQPorting_SMDSlaveBridgeRead

=============================================================================*/
/*!
    @brief
    This function is used to read from SMD port

   @return		The result of read attempt.
   @retval	::IQ_PORTING_OK		Read was successful
   @retval	::IQ_PORTING_ERROR	there was an error during reading
   @retval	::IQ_PORTING_IO_WOULDBLOCK	there is not data to read yet.
*/
/*===========================================================================*/
iq_porting_status_t IQPorting_SlaveBridgeRead
( 
  void *ioBuffer, 
  unsigned long *ioLen 
)
{
#ifdef FEATURE_MMAL_CIQ_FUSION

  uint32 dsm_packet_len;

  /* Validate params */
  if( ioBuffer == NULL )
  {
    MMAL_MSG_ERROR("Read buffer provided by IQ Agent is NULL.");
    return IQ_PORTING_INVALID_PARAM;
  }

  if(*ioLen == 0)
  {
    MMAL_MSG("No. of bytes to read provided by IQ Agent is 0. Skip read");
    return IQ_PORTING_INVALID_PARAM;
  }

  /* Check if we have data to read */
  if( mmal_ciq_pl_info.port_info_smd.rx_item_ptr == NULL)
  {
    MMAL_MSG("There is no data in SMD Bridge to read");
    mmal_ciq_pl_info.port_info_smd.iq_read_pending = TRUE;
    return IQ_PORTING_IO_WOULDBLOCK;
  }

  dsm_packet_len = dsm_length_packet(
          mmal_ciq_pl_info.port_info_smd.rx_item_ptr);

  /* If there could be a possible buffer overflow during read, capture
  that in our stats */
  if( *ioLen <  (uint64)dsm_packet_len)
  {
    mmal_ciq_pl_info.pl_stats_smd.io_read_buf_overflow_cnt++;
    *ioLen = dsm_pullup_long(
      &mmal_ciq_pl_info.port_info_smd.rx_item_ptr, 
      ioBuffer, 
      *ioLen);
  }
  else
  {
    *ioLen = dsm_pullup_long(
      &mmal_ciq_pl_info.port_info_smd.rx_item_ptr, 
      ioBuffer, 
      dsm_packet_len);
    /* dsm item is expected to be freed at this point since we pulled out
       all the data bytes */
    MMAL_ASSERT(mmal_ciq_pl_info.port_info_smd.rx_item_ptr == NULL);
  }

  /* Record stats */
  mmal_ciq_pl_info.pl_stats_smd.io_smd_read_bytes_cnt++;
  mmal_ciq_pl_info.pl_stats_smd.io_smd_read_cnt += *ioLen;

  return IQ_PORTING_OK;
#else
  return IQ_PORTING_NOT_IMPLEMENTED;
#endif /* FEATURE_MMAL_CIQ_FUSION */
}

/*=============================================================================

  FUNCTION:  IQPorting_SMDSlaveBridgeWrite

=============================================================================*/
/*!
    @brief
    This function is used to write to a SMD port

   @return		The result of write attempt.
   @retval	::IQ_PORTING_OK		Write was successful
   @retval	::IQ_PORTING_ERROR	there was an error during writing
   @retval	::IQ_PORTING_IO_WOULDBLOCK	port not ready to accept write yet.
*/
/*===========================================================================*/
iq_porting_status_t IQPorting_SlaveBridgeWrite
(
  IQ_CONST_PARAM void *ioBuffer, 
  unsigned long *ioLen
)
{
#ifdef FEATURE_MMAL_CIQ_FUSION

  unsigned long iq_ioLen;
  uint32        smd_write_length, data_remaining;
  uint32        bytes_pushed, bytes_pulled, dsm_len_packet;
  void*         tx_buf;
  void*         save_buf;


  /* Validate incoming parameters */
  if(ioBuffer == NULL || *ioLen == 0)
  {
    MMAL_MSG_ERROR("Invalid Write parameters from IQ Agent");
    return IQ_PORTING_INVALID_PARAM;
  }

  /* Increment number of writes stat */
  mmal_ciq_pl_info.pl_stats_smd.smd_write_cnt++;

  /* Cache ioLen to check if we may run out of DSM items */
  iq_ioLen = *ioLen;


  /*---------------------------------------------------------------------------
    Send the input data to the underlying SMD device.  If we haven't been 
    assigned a valid port by SMD, trash the input data.
  ---------------------------------------------------------------------------*/
  if( mmal_ciq_pl_info.port_info_smd.smd_port_id != NULL )
  {
    bytes_pushed = dsm_pushdown_tail(&mmal_ciq_txData,
                                      ioBuffer,
                                      (uint16)iq_ioLen,
                                      MMAL_DSM_ITEM_POOL); 

    dsm_len_packet = dsm_length_packet(mmal_ciq_txData);
    
    if (dsm_len_packet != 0)
    {
      tx_buf = modem_mem_alloc( dsm_len_packet,
                                  MODEM_MEM_CLIENT_MCS_CRIT );

      if (tx_buf == NULL) 
      {
        ERR_FATAL("Block could not be allocated of size %d",dsm_len_packet,0,0); 
      }

      save_buf = tx_buf;

      bytes_pulled = dsm_pullup_long(
                      &mmal_ciq_txData, 
                      tx_buf, 
                      dsm_len_packet);

      smd_write_length = smdl_write( mmal_ciq_pl_info.port_info_smd.smd_port_id, bytes_pulled, tx_buf, 0 );

      if(smd_write_length < dsm_len_packet) 
      {
        data_remaining = dsm_len_packet - smd_write_length;
        tx_buf = (uint8*)tx_buf + smd_write_length;
        bytes_pushed = dsm_pushdown_tail(&mmal_ciq_txData,
                                           tx_buf,
                                           (uint16)data_remaining,
                                           MMAL_DSM_ITEM_POOL); 

        ASSERT( bytes_pushed == data_remaining );
      }
      modem_mem_free( save_buf, MODEM_MEM_CLIENT_MCS_CRIT );
    }
  }
  else
  {
    *ioLen=0;
    mmal_ciq_pl_info.pl_stats_smd.error_cnt++;
    MMAL_MSG_ERROR ("Transmitting data with bad stream id. Freeing packet");
    return IQ_PORTING_ERROR;
  }

  return IQ_PORTING_OK;
#else
  return IQ_PORTING_NOT_IMPLEMENTED;
#endif /* FEATURE_MMAL_CIQ_FUSION */
}

/*=============================================================================

  FUNCTION:  mmal_ciq_pl_get_iq_agent_init_flag

=============================================================================*/
/*!
    @brief
    This function is used to get the iq agent init flag

   @return		True: If Init was successful
                False: Otherwise.
*/
/*===========================================================================*/
boolean mmal_ciq_pl_get_iq_agent_init_flag ( void )
{
  return mmal_ciq_pl_info.iq_agent_init_success;
}
#endif /* FEATURE_MMAL_CIQ */
