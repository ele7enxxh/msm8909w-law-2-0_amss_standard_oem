/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S
                        A T   C O M M A N D   

                        ( S M S  C O M M A N D S )

GENERAL DESCRIPTION
  This module contains the command tables and data definitions required
  to define the SMS related AT modem commands.

  This file contains common sms functions across modes.
  
EXTERNALIZED FUNCTIONS

  Public functions exported outside of sms module.

    dsat_flush_sms_indications
      Flushes buffer of SMS incoming message indications and sends indications
      to TE. Indications are buffered and sent based on AT parameter setting

    dsatsms_init_sms_ex
      SMS initialization function. This function does the following: 
      Initializes ATCOP as one of the clients to WMS, Registers the WMS 
      message and configuration call back functions and Sets up the message 
      routing configuration.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

   Copyright (c) 2003 - 2014 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatsms_ex.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/29/14   sc      Added support for NV Refresh.
06/27/14   tk      Initial revision (created file for Dynamic ATCoP).

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stringl/stringl.h>
#include <stdio.h>

#if defined(T_WINNT)
#error code not present
#endif /* WINNT */

#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)
#include <stdio.h>
#include <string.h>

#include "dsati.h"
#include "dsatdl.h"
#include "dsatsmsi.h"
#include "err.h"
#include "amssassert.h"
#include "queue.h"

#include "dsatsms.h"
#include "dsatctab.h"
#ifdef FEATURE_ETSI_SMS
#include "dsatetsictab.h"
#include "dsatetsime.h"
#include "dsatetsismsi.h"
#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_CDMA_SMS
#include "dsat707sms.h"
#include "dsat707smsctab.h"
#endif /* FEATURE_CDMA_SMS */

/*===========================================================================
            REGIONAL DEFINITIONS AND DECLARATIONS FOR MODULE
  This section contains reginal definitions for variables that are used
  in ATCOP SMS SUB SYSTEM
===========================================================================*/

/*--------------------------------------------------------------------------
  Debug macro for this module.
---------------------------------------------------------------------------*/
#define DEBUG( x )

/*--------------------------------------------------------------------------
  Variables common for SMS module
---------------------------------------------------------------------------*/
/* WMS client (ATCOP) ID */
wms_client_id_type dsatsmsi_client_id = {0};

/* Variable to find out what command is being processed currently */
dsat_sms_cmd_s_type dsatsmsi_pres_cmd = {0};

/* Variable used in SMS SM */
sms_cmd_sm_type dsatsmsi_cmd_sm = {0};

/* Flag for wms readiness */
boolean dsatsmsi_wms_ready = FALSE;

#ifdef FEATURE_DSAT_MDM_FUSION
/* Variable to indicate if IMS registration happened */
boolean dsat_sms_over_ims = FALSE; 
#endif /* FEATURE_DSAT_MDM_FUSION */

/* Watermarks and Queues for SMS. */
/* This WM holds received messages and implements flow control. 
   Actually it just drops MT messages if the watermark is at high 
   as indicated by sms_mt_msg_wm_full */
dsm_watermark_type  dsat_mt_msg_wm;
LOCAL q_type              dsat_mt_msg_q;

/* This one holds all other SMS events and does not perform flow control */
dsm_watermark_type  dsat_sms_wm = {0};
LOCAL q_type              dsat_sms_q;

/* Flag indicating MT message watermark is at high water */
/* Messages received when this flag is TRUE will be lost */
LOCAL boolean sms_mt_msg_wm_full = FALSE;

#ifdef FEATURE_ETSI_SMS
/* Track the memory store reporting full state (for +CIND command) */
LOCAL wms_memory_store_e_type dsatsms_full_mem_store[DSAT_SYS_MODEM_AS_ID_MAX] = {0};
#endif /*  FEATURE_ETSI_SMS */

/*Maximum number of MT messages on the queue*/
#define SMS_MT_MSG_COUNT    15 

/* SMS Mobile Terminated message queue node declaration */
/* This queue is used for STORE and NOTIFY messages     */
LOCAL sms_mt_msg_type mt_msg_buf[SMS_MT_MSG_COUNT];

LOCAL sms_mt_msg_type is707_mt_msg_buf[SMS_MT_MSG_COUNT];

/* SMS Mobile Terminated message queue */
q_type sms_mt_msg_q;         

/* Queue for storing free message buffers */ 
q_type sms_mt_msg_free_q;

/* SMS Mobile Terminated message queue */
q_type is707_sms_mt_msg_q;         

/* Queue for storing free message buffers */ 
q_type is707_sms_mt_msg_free_q;

/* Lo Water, Hi Water and Don't Exceed counts for the   */
/* Autodetect watermark used for RX data bytes from WMS */
#define DSAT_FROM_WMS_LOWATER_CNT          (WMS_MAX_LEN * 5)
#define DSAT_FROM_WMS_HIWATER_CNT          (WMS_MAX_LEN * 15)
#define DSAT_FROM_WMS_DONT_EXCEED_CNT      (WMS_MAX_LEN * 30)

/*--------------------------------------------------------------------------
  Local function prototypes
---------------------------------------------------------------------------*/

LOCAL void sms_msg_cb_func 
(
  wms_msg_event_e_type            event,       /* Event ID              */
  const wms_msg_event_info_s_type *event_ptr   /* Pointer to Event info */
);

LOCAL void sms_cfg_cb_func 
(
  wms_cfg_event_e_type            event,     /* Event ID              */
  const wms_cfg_event_info_s_type *event_ptr /* Pointer to Event info */
);

LOCAL sms_event_e_type cfg_event_lookup
(
 wms_cfg_event_e_type wms_event /* Event ID */
);

LOCAL sms_event_e_type msg_event_lookup
(
 wms_msg_event_e_type wms_event /* Event ID */
);

LOCAL dsati_operating_cmd_mode_type dsatsms_get_cfg_msg_mode
(
 sms_event_info_s_type *event_info /* Event info */
);

LOCAL void setup_sms_watermarks( void );

LOCAL void sms_mt_msg_wm_non_empty_cb (dsm_watermark_type *wm, void *v);

LOCAL void sms_wm_non_empty_cb (dsm_watermark_type *wm, void *v);

LOCAL void sms_wm_hiwater_cb (dsm_watermark_type *wm, void *v);

LOCAL void sms_wm_lowater_cb (dsm_watermark_type *wm, void *v);

LOCAL void mt_msg_queue_init( void );


/*---------------------------------------------------------------------------
 * FUNCTION DEFINITIONS                                                     
 *-------------------------------------------------------------------------*/
/*===========================================================================

FUNCTION DSATSMS_SIGNAL_HANDLER

DESCRIPTION
  This function processes both received MT message and SMS signals, 
  which indicate MT message and other SMS events, respectively, have been 
  received from WMS.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ERROR: if we find an error in procssing the event
  DSAT_OK: if processing of the event is complete and right
  DSAT_ASYNC_CMD : if we still have more events to process
  DSAT_ASYNC_EVENT : if the event is ignored

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatsms_signal_handler
(
 dsat_mode_enum_type at_mode
)
{
  dsm_item_type * sms_dsm_item_ptr;
  sms_event_info_s_type               * sms_info      = NULL;
  const msg_handler_tab_type          * handler_table = NULL;
  int                                   table_size = 0;
  dsati_operating_cmd_mode_type         cmd_mode;
  int                                   index;
  dsat_result_enum_type                 result = DSAT_OK;

  while ( (sms_dsm_item_ptr = dsm_dequeue( &dsat_mt_msg_wm )) != NULL ||
          (sms_dsm_item_ptr = dsm_dequeue( &dsat_sms_wm )) != NULL )
  {
    sms_info = dsat_alloc_memory(sizeof(sms_event_info_s_type), FALSE);

    (void)dsm_pullup 
      (&sms_dsm_item_ptr, sms_info, sizeof(sms_event_info_s_type));

    dsm_free_packet( &sms_dsm_item_ptr );

    cmd_mode = dsatsms_get_cfg_msg_mode(sms_info);

    if (!IS_VALID_SUBS_ID(sms_info->subs_id))
    {
      DS_AT_MSG1_ERROR("Invalid subs ID %d, continuing...", sms_info->subs_id);
      continue;
    }

    DS_AT_MSG4_HIGH("ATCOP:Freed %d DSM item, signal handler, SMS mode = %d, Event = %d, Command mode %d",
                      sms_dsm_item_ptr, sms_info->u.msg_status.msg_hdr.message_mode,
                      sms_info->event, cmd_mode);

#ifdef FEATURE_ETSI_SMS 
    /* Respond to SMS_CFG_EVENT_GW_READY event regardless of currently recorded
       command mode to avoid race condition where mode is changing but this 
       event preceded that notification */
    if((cmd_mode == ETSI_CMD_MODE)
        ||
        (SMS_CFG_EVENT_GW_READY == sms_info->event) ||
        (SMS_CFG_EVENT_MESSAGE_LIST == sms_info->event) ||
        (SMS_MSG_EVENT_READ_TEMPLATE == sms_info->event) )
    {
      DSAT_DL_CHECK_SYMBOL_ADDR(dsatdl_vtable.dsatetsisms_msg_handler_table_ptr);
      handler_table = dsatdl_vtable.dsatetsisms_msg_handler_table_ptr->base_ptr;
      table_size = (int) dsatdl_vtable.dsatetsisms_msg_handler_table_ptr->size;
    }
    else
#endif /* FEATURE_ETSI_SMS */
#ifdef FEATURE_CDMA_SMS 
    if(cmd_mode == CDMA_CMD_MODE)
    {
      DSAT_DL_CHECK_SYMBOL_ADDR(dsatdl_vtable.dsat707sms_msg_handler_table_ptr);
      handler_table = dsatdl_vtable.dsat707sms_msg_handler_table_ptr->base_ptr;
      table_size = (int) dsatdl_vtable.dsat707sms_msg_handler_table_ptr->size;
    }
    else
#endif /* FEATURE_CDMA_SMS */
    {
      DS_AT_MSG2_ERROR("Cannot process this event: %d in this mode: %d "
                       "Trying to process other pending messages, if any",
                 (int)sms_info->event, cmd_mode);
    }

    for (index = 0; index < table_size; index++)
    {
      if(handler_table[index].event == sms_info->event)
      {
        DS_AT_MSG1_HIGH("dsatetsisms_msg_handler_table index = %d",index);
        result = handler_table[index].handler(at_mode, sms_info);

        if ( result != DSAT_ASYNC_CMD && result != DSAT_ASYNC_EVENT )
        {
          if (dsat_sms_wm.current_cnt != 0)
          {
#ifdef FEATURE_MODEM_RCINIT_PHASE2
            (void)rex_set_sigs( rcinit_lookup_rextask("ds"), DS_AT_SMS_SIG );
#else
            (void)rex_set_sigs( &ds_tcb, DS_AT_SMS_SIG );
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */
          }

          if (dsat_mt_msg_wm.current_cnt != 0)
          {
#ifdef FEATURE_MODEM_RCINIT_PHASE2
            (void)rex_set_sigs( rcinit_lookup_rextask("ds"), DS_AT_MT_MSG_SIG );
#else
            (void)rex_set_sigs( &ds_tcb, DS_AT_MT_MSG_SIG );
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */

          }
          dsatutil_free_memory_ext((void **)&sms_info);
          return result;
        }
      }
    } /* for */
    /*case to free async cmd's else it will NULL and just return back*/
    dsatutil_free_memory_ext((void **)&sms_info);
  } /* while */

  dsatutil_free_memory_ext((void **)&sms_info);
  return DSAT_ASYNC_EVENT;
}/* dsatsms_signal_handler */


/*===========================================================================

FUNCTION DSAT_FLUSH_SMS_INDICATIONS

DESCRIPTION
  Flushes buffer of SMS incoming message indications and sends indications
  to TE. Indications are buffered and sent based on AT parameter setting.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/
void dsat_flush_sms_indications
(
 void
)
{
  DSAT_DL_CHECK_SYMBOL_ADDR(dsatdl_vtable.dsatsms_flush_sms_indications_fp);
  dsatdl_vtable.dsatsms_flush_sms_indications_fp();

  return;
}/* dsat_flush_sms_indications */


/*===========================================================================

FUNCTION    DSATSMSI_CMD_CB_FUNC

DESCRIPTION
  Process the WMS command call back status.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
void dsatsmsi_cmd_cb_func
(
  wms_cmd_id_e_type     cmd,   /* Cmd ID            */
  void                  *data, /* User data pointer */
  wms_cmd_err_e_type    status /* Command Status    */
)
{
  DEBUG(
        DS_AT_MSG2_MED("dsatsmsi_cmd_cb_func, cmd:%d, status:%d",
                cmd, status);
        DS_AT_MSG2_MED("data: %d, %p", *((int *) data), data);
        );

  if (status == WMS_CMD_ERR_NONE) {
    /* If there is no error */
    /* Continue             */
    return;
  }
  
  /* Pass the error status to the TE */

  DS_AT_MSG2_HIGH("DS SMS cmd err %d %d", cmd, status);

  DSAT_DL_LOCK();

  if (DSAT_DL_VALIDATE_SYMBOL_ADDR(dsatdl_vtable.dsatsmsi_cmd_err_cb_func_fp))
  {
    dsatdl_vtable.dsatsmsi_cmd_err_cb_func_fp(cmd, status);
  }

  DSAT_DL_UNLOCK();

  return;
}/* dsatsmsi_cmd_cb_func */


/*===========================================================================

FUNCTION    MT_MSG_QUEUE_INIT

DESCRIPTION
  Initialize the mobile terminted message queue and the free message queue, 
  and link the message items onto the free message queue.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void mt_msg_queue_init( void )
{
  uint8    i;                                                /* Loop index */

  /*  Initialize the mobile terminted message queue and the free message  */
  /*  queue, and link the message items onto the free message queue.      */  
  
  (void) q_init( &sms_mt_msg_q );
  (void) q_init( &sms_mt_msg_free_q );

  (void) q_init( &is707_sms_mt_msg_q );
  (void) q_init( &is707_sms_mt_msg_free_q );


  for( i = 0; i < SMS_MT_MSG_COUNT; i++ )
  {
    (void) q_link( &mt_msg_buf[i], &mt_msg_buf[i].link );
    q_put( &sms_mt_msg_free_q, &mt_msg_buf[i].link );
  }

  for( i = 0; i < SMS_MT_MSG_COUNT; i++ )
  {
    (void) q_link( &is707_mt_msg_buf[i], &is707_mt_msg_buf[i].link );
    q_put( &is707_sms_mt_msg_free_q, &is707_mt_msg_buf[i].link );
  }

} /* mt_msg_queue_init */



/*===========================================================================

FUNCTION  DSATSMS_INIT_SMS_EX

DESCRIPTION
  SMS initialization function. This function does the following:
  - Initializes ATCOP as one of the clients to WMS.
  - Registers the WMS message and configuration call back functions.
  - Activates ATCOP asa client
  - Sets up the message routing configuration.
  
DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
void dsatsms_init_sms_ex ( void )
{
  wms_client_err_e_type  result;
  sys_modem_as_id_e_type subs_id;

  /* Initializes necessary watermarks first */
  setup_sms_watermarks();

  dsatsmsi_pres_cmd.cmd_name = SMS_NONE;

  /* Initializes ATCOP as one of the clients to WMS               */
  /* Rest of the functionality needs the client to be initialized */
  result = wms_client_init (  
                                WMS_CLIENT_TYPE_ATCOP,
                                &dsatsmsi_client_id
                              );

  /* This should not happen, raise an error */
  if (result != WMS_CLIENT_ERR_NONE)
  {
    DS_AT_MSG0_ERROR("Could not initialize with WMS");
  }

  /* Register the configuration call back function */
  result = wms_client_reg_cfg_cb ( dsatsmsi_client_id,
                                   (wms_cfg_event_cb_type)sms_cfg_cb_func );

  if (result != WMS_CLIENT_ERR_NONE)
  {
    DS_AT_MSG0_ERROR("Could not register call back function with reg_cfg_cb");
  }
  /* Register the message call back function */
  result = wms_client_reg_msg_cb ( dsatsmsi_client_id,
                                   (wms_msg_event_cb_type)sms_msg_cb_func );
  if (result != WMS_CLIENT_ERR_NONE)
  {
    DS_AT_MSG0_ERROR("Could not register call back function with reg_msg_cb");
  }

  result = wms_client_activate ( dsatsmsi_client_id );

  if (result != WMS_CLIENT_ERR_NONE)
  {
    DS_AT_MSG0_ERROR("Could not initialize with WMS");
  }

  for (subs_id = SYS_MODEM_AS_ID_1; subs_id < DSAT_SYS_MODEM_AS_ID_MAX; subs_id++)
  {
    dsatsms_sync_nv(subs_id);
  }

  /* Initializing ATCoP SMS Queue which would be used by both 1x and UMTS */
#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)
  mt_msg_queue_init();
#endif /* FEATURE_ETSI_SMS || FEATURE_CDMA_SMS */

#ifdef FEATURE_CDMA_SMS

  DSATUTIL_SET_VAL(DSAT707_SMS_QCNMI_IDX,0,CNMI_MODE_VAL,0,1,NUM_TYPE)
  DSATUTIL_SET_VAL(DSAT707_SMS_QCNMI_IDX,0,CNMI_MT_VAL,0,1,NUM_TYPE)
  DSATUTIL_SET_VAL(DSAT707_SMS_QCNMI_IDX,0,CNMI_BM_VAL,0,0,NUM_TYPE)
  DSATUTIL_SET_VAL(DSAT707_SMS_QCNMI_IDX,0,CNMI_DS_VAL,0,2,NUM_TYPE)
  DSATUTIL_SET_VAL(DSAT707_SMS_QCNMI_IDX,0,CNMI_BFR_VAL,0,0,NUM_TYPE)


#ifdef FEATURE_DSAT_EXTENDED_SMS
  DSATUTIL_SET_VAL(DSAT707_SMS_HSMSSS_IDX,0,0,0,0,NUM_TYPE)
  DSATUTIL_SET_VAL(DSAT707_SMS_HSMSSS_IDX,0,1,0,WMS_PRIORITY_NORMAL,NUM_TYPE)
  DSATUTIL_SET_VAL(DSAT707_SMS_HSMSSS_IDX,0,2,0,1,NUM_TYPE)
  DSATUTIL_SET_VAL(DSAT707_SMS_HSMSSS_IDX,0,3,0,WMS_PRIVACY_NORMAL,NUM_TYPE)

#endif /* FEATURE_DSAT_EXTENDED_SMS */

#endif /* FEATURE_CDMA_SMS */

  return;
}/* dsatsms_init_sms_ex */

/*===========================================================================

FUNCTION  DSATSMS_SYNC_NV

DESCRIPTION
  This function syncs sms nv.
  
DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
void dsatsms_sync_nv
(
  sys_modem_as_id_e_type    subs_id
)
{
  nv_item_type item;

  memset(&item, 0x00, sizeof(nv_item_type));
  
  /* Get the MEM3 value from NV (in sync with NV) and use it for CPMS read command 
     MEM1 and MEM2 are defaulted to ME at boot up.*/
  if ( dsatutil_get_nv_item_per_subs ( NV_SMS_CFG_ROUTING_I ,
                                       &item,
                                       subs_id) == NV_DONE_S )
  {
    /* NV item gets changed only when the cpms_val[2] is modified. 
       cpms_val[0],cpms_val[1] remains same. At boot up we need 
       to get only the cpms_val[2] from NV */
#ifdef FEATURE_ETSI_SMS
    DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_CPMS_ETSI_IDX,subs_id,2,0,
                               item.sms_cfg_routing.pp_mem_stores[2],NUM_TYPE)
#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_CDMA_SMS
  if ( subs_id == SYS_MODEM_AS_ID_1 )
  {
    /* Check if value in NV is valid, If not set <mem3> to default */
    if ( item.sms_cfg_routing.pp_mem_stores[5] < 3)
    {
      DSATUTIL_SET_VAL(DSAT707_SMS_QCPMS_IDX,0,2,0,item.sms_cfg_routing.pp_mem_stores[5],NUM_TYPE)
    }
    else 
    {
      DSATUTIL_SET_VAL(DSAT707_SMS_QCPMS_IDX,0,2,0,0,NUM_TYPE)
    }
  }
#endif /* FEATURE_CDMA_SMS */
  }
}

/**********************LOCAL UTILITY FUNCTIONS******************************/
/*===========================================================================
FUNCTION  SMS_VALIDATE_CLIENT_ID

DESCRIPTION
  Validate client ID for handling WMS events.

DEPENDENCIES

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean sms_validate_client_id
(
  wms_client_type_e_type client_id
)
{
  if ((WMS_CLIENT_TYPE_INTERNAL != client_id) &&
      (WMS_CLIENT_TYPE_ATCOP != client_id))
  {
    DS_AT_MSG1_MED("Client ID mismatch %d", client_id);
    return FALSE;
  }

  return TRUE;
} /* sms_validate_client_id */

/**********************CALL BACK FUNCTIONS**********************************/

/*===========================================================================
FUNCTION  SMS_MSG_CB_FUNC

DESCRIPTION
  SMS Message event callback function

DEPENDENCIES

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer
===========================================================================*/
LOCAL void sms_msg_cb_func
(
  wms_msg_event_e_type            event,       /* Event ID              */
  const wms_msg_event_info_s_type *event_ptr   /* Pointer to Event info */
)
{
  wms_client_type_e_type client_id = WMS_CLIENT_TYPE_INTERNAL;
  sms_event_info_s_type  event_info;
  dsm_item_type         *ds_cmd_buff = NULL;
  dsm_watermark_type    *wm_to_use = &dsat_sms_wm;

  ASSERT (event_ptr != NULL);

  DS_AT_MSG1_HIGH("sms_msg_cb_func, event:%d", event);

  /* Ignore unwanted events based on client ID */
  switch (event)
  {
    case WMS_MSG_EVENT_SEND:
    case WMS_MSG_EVENT_READ:
    case WMS_MSG_EVENT_WRITE:
    case WMS_MSG_EVENT_DELETE:
    case WMS_MSG_EVENT_MODIFY_TAG:
    case WMS_MSG_EVENT_READ_TEMPLATE:
    case WMS_MSG_EVENT_WRITE_TEMPLATE:
    case WMS_MSG_EVENT_DELETE_TEMPLATE:
    case WMS_MSG_EVENT_READ_STS_REPORT:
    case WMS_MSG_EVENT_WRITE_STS_REPORT:
      client_id = event_ptr->status_info.client_id;
      break;

    default:
      break;
  }

  if (FALSE == sms_validate_client_id(client_id))
  {
    return;
  }

  memset ( &event_info, 0, sizeof( sms_event_info_s_type ) );
  
#ifdef FEATURE_DSAT_MDM_FUSION
  if( event == WMS_MSG_EVENT_TRANSPORT_REG )
  {
    if( event_ptr->transport_reg_info.transport_type == WMS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS)
    {
      dsat_sms_over_ims = event_ptr->transport_reg_info.is_registered;
    }
    else
    {
      dsat_sms_over_ims = FALSE;
    }
    DS_AT_MSG2_HIGH("SMS Transport type (%d), Registered (%d)",
               (int)event_ptr->transport_reg_info.transport_type,
               (int)event_ptr->transport_reg_info.is_registered);
    return;
  }
#endif /* FEATURE_DSAT_MDM_FUSION */

  event_info.event = msg_event_lookup(event);

  if ( event_info.event == SMS_EVENT_NONE )
  {
    DS_AT_MSG1_ERROR("unable to map, event:%d", event);
    return;
  }
  
  if ( event == WMS_MSG_EVENT_SEND ||
       event == WMS_MSG_EVENT_READ ||
       event == WMS_MSG_EVENT_WRITE            ||
       event == WMS_MSG_EVENT_DELETE           ||
       event == WMS_MSG_EVENT_READ_STS_REPORT  ||
       event == WMS_MSG_EVENT_WRITE_STS_REPORT   )
  {
    event_info.u.msg_status.msg_hdr = event_ptr->status_info.message.msg_hdr;
    event_info.subs_id = event_ptr->status_info.as_id;

    switch ( event_info.u.msg_status.msg_hdr.message_mode )
    {
      case WMS_MESSAGE_MODE_CDMA:
        event_info.u.msg_status.u.cdma_message = 
          event_ptr->status_info.message.u.cdma_message;
        break;
      case WMS_MESSAGE_MODE_GW:
        event_info.u.msg_status.u.gw_message = 
          event_ptr->status_info.message.u.gw_message;
        break;
      default:
        DS_AT_MSG1_ERROR("Bad message mode: %d", 
             event_info.u.msg_status.msg_hdr.message_mode);
        return;
    }
    event_info.u.msg_status.user_data = event_ptr->status_info.user_data;
    wm_to_use = &dsat_sms_wm;
  }
  else if ( ( event == WMS_MSG_EVENT_RECEIVED_MESSAGE ) ||
            ( event == WMS_MSG_EVENT_STATUS_REPORT ) )
  {
    /* Drop received message if watermark is full */
    if (sms_mt_msg_wm_full)
    {
      return;
    }

    event_info.u.mt_msg.msg_hdr = event_ptr->mt_message_info.message.msg_hdr;
    event_info.subs_id = event_ptr->mt_message_info.as_id;

    switch ( event_info.u.mt_msg.msg_hdr.message_mode )
    {
      case WMS_MESSAGE_MODE_CDMA:
        event_info.u.mt_msg.u.cdma_message = 
          event_ptr->mt_message_info.message.u.cdma_message;
        break;
      case WMS_MESSAGE_MODE_GW:
        event_info.u.mt_msg.u.gw_message = 
        event_ptr->mt_message_info.message.u.gw_message;
        break;
      default:
        DS_AT_MSG1_ERROR("Bad message mode: %d", 
             event_info.u.msg_status.msg_hdr.message_mode);
        return;
    }
    event_info.u.mt_msg.transaction_id = 
    event_ptr->mt_message_info.transaction_id;

    wm_to_use = &dsat_mt_msg_wm;
  }
  else if ( event == WMS_MSG_EVENT_SUBMIT_REPORT )
  {
    int i;

    /* Drop received message if watermark is full */
    if (sms_mt_msg_wm_full)
    {
      return;
    }
    event_info.u.submit_report.message_mode = event_ptr->submit_report_info.message_mode;
    event_info.u.submit_report.report_status = 
      event_ptr->submit_report_info.report_status;
    event_info.u.submit_report.tpdu_type = 
      event_ptr->submit_report_info.tpdu_type;
    event_info.u.submit_report.u.tp_cause = 
      event_ptr->submit_report_info.u.error.tp_cause;
    event_info.u.submit_report.tl_status =
      event_ptr->submit_report_info.cause_info.tl_status;
    event_info.u.submit_report.u.ack.sm_len = 
      event_ptr->submit_report_info.u.ack.user_data.sm_len;
    event_info.u.submit_report.u.ack.timestamp = 
      event_ptr->submit_report_info.u.ack.timestamp;

    if ( ( event_ptr->submit_report_info.report_status == WMS_RPT_OK ) &&
         ( event_ptr->submit_report_info.tpdu_type != WMS_TPDU_NONE ) )
    {
      for ( i=0; 
            ((i<event_info.u.submit_report.u.ack.sm_len) && (i<WMS_MAX_LEN)); 
            i++ )
      {
        event_info.u.submit_report.u.ack.sm_data[i] =
        event_ptr->submit_report_info.u.ack.user_data.sm_data[i];
      }
    }
    event_info.subs_id = event_ptr->submit_report_info.as_id;
    wm_to_use = &dsat_mt_msg_wm;
  }
  else if ( event == WMS_MSG_EVENT_MT_MESSAGE_ERROR )
  {
    event_info.u.err_report = event_ptr->mt_message_error.report_status;
    event_info.subs_id = event_ptr->mt_message_error.as_id;
    wm_to_use = &dsat_sms_wm;
  }
  else if ( event == WMS_MSG_EVENT_READ_TEMPLATE )
  {
    event_info.u.msg_status.u.gw_template = 
      event_ptr->status_info.message.u.gw_template;
    event_info.u.msg_status.msg_hdr.mem_store = 
      event_ptr->status_info.message.msg_hdr.mem_store;
    event_info.subs_id = event_ptr->status_info.as_id;

  }
  
  /* Place the information in a DSM item */
  ds_cmd_buff = dsat_dsm_create_packet(&event_info, sizeof(sms_event_info_s_type), TRUE);
  if (NULL != ds_cmd_buff)
  {
    dsm_enqueue(wm_to_use, &ds_cmd_buff);
  }

  return;
}/* sms_msg_cb_func */


/*===========================================================================
FUNCTION  SMS_CFG_CB_FUNC

DESCRIPTION
  SMS configuration event callback function

DEPENDENCIES

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer
===========================================================================*/
LOCAL void sms_cfg_cb_func 
(
  wms_cfg_event_e_type            event,     /* Event ID              */
  const wms_cfg_event_info_s_type *event_ptr /* Pointer to Event info */
)
{
  wms_client_type_e_type client_id = WMS_CLIENT_TYPE_INTERNAL;
  sms_event_info_s_type  event_info;
  dsm_item_type         *ds_cmd_buff;

  ASSERT (event_ptr != NULL);

  DS_AT_MSG1_HIGH("sms_cfg_cb_func, event:%d", event);

  /* Ignore unwanted events based on client ID */
  switch (event)
  {
    case WMS_CFG_EVENT_ROUTES:
      client_id = event_ptr->routes.client_id;
      break;

    case WMS_CFG_EVENT_MEMORY_STATUS:
      client_id = event_ptr->memory_status.client_id;
      break;

    case WMS_CFG_EVENT_MESSAGE_LIST:
      if (WMS_TAG_MO_TEMPLATE != event_ptr->message_list.tag)
      {
        client_id = event_ptr->message_list.client_id;
      }
      break;

    case WMS_CFG_EVENT_LINK_CONTROL:
      client_id = event_ptr->link_control.client_id;
      break;

    case WMS_CFG_EVENT_MS_GW_DOMAIN_PREF:
      client_id = event_ptr->ms_gw_domain_pref.client_id;
      break;

    default:
      break;
  }

  if (FALSE == sms_validate_client_id(client_id))
  {
    return;
  }

  memset ( &event_info, 0, sizeof( sms_event_info_s_type ) );

  event_info.event = cfg_event_lookup(event);

  switch ( event_info.event )
  {
    case SMS_CFG_EVENT_MESSAGE_LIST:
      {
        uint32 i;
        event_info.u.msg_list.tag = event_ptr->message_list.tag;
        event_info.u.msg_list.len = (uint16)event_ptr->message_list.len;
        event_info.u.msg_list.mem_store = event_ptr->message_list.mem_store;
        for ( i = 0; i < event_ptr->message_list.len; i++ )
        {
          event_info.u.msg_list.indices[i] = 
            (uint16) event_ptr->message_list.indices[i];
        }
        event_info.subs_id = event_ptr->message_list.as_id;
      }
      break;
  
    case SMS_CFG_EVENT_MEMORY_STATUS:
      event_info.u.mem_status = event_ptr->memory_status;
      event_info.subs_id = event_ptr->memory_status.as_id;
      break;
   
#ifdef FEATURE_ETSI_SMS
    case SMS_CFG_EVENT_GW_DOMAIN_PREF:
#ifdef FEATURE_DUAL_SIM
      event_info.u.gw_domain = event_ptr->ms_gw_domain_pref.domain_pref;
      event_info.subs_id = event_ptr->ms_gw_domain_pref.as_id;
#else
      event_info.u.gw_domain = event_ptr->gw_domain_pref;
#endif /* FEATURE_DUAL_SIM */
      break;

    case SMS_CFG_EVENT_LINK_CONTROL:
      event_info.u.link_mode = event_ptr->link_control.link_control_mode;
      break; 
#endif /* FEATURE_ETSI_SMS */

#ifndef FEATURE_DUAL_SIM
    case SMS_CFG_EVENT_GW_READY:
#ifdef FEATURE_CDMA_SMS
    case SMS_CFG_EVENT_CDMA_READY:
#endif /* FEATURE_CDMA_SMS */
#else
    case SMS_CFG_EVENT_MS_READY:
      if (WMS_CFG_SYS_GW == event_ptr->ms_ready.sys_type)
      {
        event_info.event = SMS_CFG_EVENT_GW_READY;
      }
#ifdef FEATURE_CDMA_SMS
      else if (WMS_CFG_SYS_CDMA == event_ptr->ms_ready.sys_type)
      {
        event_info.event = SMS_CFG_EVENT_CDMA_READY;
      }
#endif /* FEATURE_CDMA_SMS */
      else
      {
        DS_AT_MSG1_ERROR("Invalid system type %d in ready event",
                         event_ptr->ms_ready.sys_type);
        return;
      }

      event_info.subs_id = event_ptr->ms_ready.as_id;
#endif /* FEATURE_DUAL_SIM */
      break;

#ifdef FEATURE_DSAT_EXTENDED_SMS 
    case SMS_CFG_EVENT_MEMORY_FULL:
      event_info.u.mem_full = event_ptr->mem_store;
      break;
#endif /* FEATURE_DSAT_EXTENDED_SMS */

    case SMS_EVENT_NONE:
      DS_AT_MSG1_MED("unable to map, event:%d", event);
      return;

    default:
      break;
  }

  /* Place the information in a DSM item */
  ds_cmd_buff = dsat_dsm_create_packet(&event_info, sizeof(sms_event_info_s_type), TRUE);
  if (NULL != ds_cmd_buff)
  {
    dsm_enqueue(&dsat_sms_wm, &ds_cmd_buff);
  }

  return;
}/* sms_cfg_cb_func */


/*===========================================================================
FUNCTION  DSATSMS_GET_CFG_MSG_MODE

DESCRIPTION
  Function to retrieve the Mode amognst GW or CDMA for which the event is received 

DEPENDENCIES
  None

RETURNS
  Mode.amongst GW or CDMA for which the event is sent/handled for.

SIDE EFFECTS
   
===========================================================================*/
dsati_operating_cmd_mode_type dsatsms_get_cfg_msg_mode
(
 sms_event_info_s_type *event_info /* Event info */
)
{
  dsati_operating_cmd_mode_type cmd_mode = ETSI_CMD_MODE;


  switch(event_info->event)
  {
#ifdef FEATURE_CDMA_SMS
    case SMS_CFG_EVENT_CDMA_READY:
    #ifdef FEATURE_DSAT_EXTENDED_SMS
    case SMS_CFG_EVENT_MEMORY_FULL:
    #endif /* FEATURE_DSAT_EXTENDED_SMS */
      cmd_mode = CDMA_CMD_MODE;
      break;
#endif /* FEATURE_CDMA_SMS */

    case SMS_CFG_EVENT_ROUTES: 
    case SMS_CFG_EVENT_MEMORY_STATUS: 
    case SMS_CFG_EVENT_MESSAGE_LIST: 
    case SMS_MSG_EVENT_MODIFY_TAG: 
    case SMS_MSG_EVENT_ACK:
    case SMS_CFG_EVENT_LINK_CONTROL: 
    case SMS_MSG_EVENT_READ_TEMPLATE: 
    case SMS_MSG_EVENT_WRITE_TEMPLATE: 
    case SMS_MSG_EVENT_DELETE_TEMPLATE: 
    #ifdef FEATURE_ETSI_SMS_PS
    case SMS_CFG_EVENT_GW_DOMAIN_PREF:
    #endif /* FEATURE_ETSI_SMS_PS */
    case SMS_CFG_EVENT_GW_READY:
    #ifdef FEATURE_ETSI_SMS_CB
    case SMS_BC_MM_EVENT_ADD_SRVS: 
    case SMS_BC_MM_EVENT_DELETE_SRVS:  
    case SMS_BC_MM_EVENT_PREF: 
    case SMS_BC_MM_EVENT_DELETE_ALL_SRVS:
    case SMS_BC_MM_EVENT_SRV_IDS: 
    case SMS_MSG_EVENT_MT_MESSAGE_ERROR: 
      cmd_mode = ETSI_CMD_MODE;
      break;       
    #endif /* FEATURE_ETSI_SMS_CB */
    
    case SMS_MSG_EVENT_SEND:
    case SMS_MSG_EVENT_READ:
    case SMS_MSG_EVENT_WRITE:
    case SMS_MSG_EVENT_DELETE:
    case SMS_MSG_EVENT_READ_STS_REPORT: 
    case SMS_MSG_EVENT_WRITE_STS_REPORT: 
      if( event_info->u.msg_status.msg_hdr.message_mode == WMS_MESSAGE_MODE_CDMA )
      {
        cmd_mode = CDMA_CMD_MODE;
      }
      else
      {
        cmd_mode = ETSI_CMD_MODE;
      }
      break; 
    
    case SMS_MSG_EVENT_RECEIVED_MESSAGE: 
    case SMS_MSG_EVENT_STATUS_REPORT: 
      if( event_info->u.mt_msg.msg_hdr.message_mode == WMS_MESSAGE_MODE_CDMA )
      {
        cmd_mode = CDMA_CMD_MODE;
      }
      else
      {
        cmd_mode = ETSI_CMD_MODE;
      }
      break;

    case SMS_MSG_EVENT_SUBMIT_REPORT: 
      if( event_info->u.submit_report.message_mode == WMS_MESSAGE_MODE_CDMA )
      {
        cmd_mode = CDMA_CMD_MODE;
      }
      else
      {
        cmd_mode = ETSI_CMD_MODE;
      }
      break;
    default:
      DS_AT_MSG1_HIGH("Unknown %d event, defaulting to ETSI mode",event_info->event);
  }

   DS_AT_MSG2_HIGH("%d event - %d mode",event_info->event,cmd_mode);

  return cmd_mode;
}/* dsatsms_get_cfg_msg_mode */
/*===========================================================================

FUNCTION  DSATSMSI_GW_READY_CFG_EVENT_HANDLER

DESCRIPTION
  Handler function for gw ready configuration event from WMS

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  Possible values are:
    DSAT_ASYNC_EVENT : the event is not related to an AT command

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatsmsi_gw_ready_cfg_event_handler
(
 dsat_mode_enum_type mode, /* AT command mode    */
  sms_event_info_s_type  *event_info   /* Event info ptr    */
)
{
  /* WMS is ready now */
  dsatsmsi_wms_ready = TRUE;

  if (TRUE == dsatme_is_thin_ui())
  {
    /* Refer to 3g TS 27.005 section 3.4.1 and table 2 */
    (void) dsatsmsi_route_config ( MT_VAL_1, DS_VAL_0, event_info->subs_id );
  }

#ifdef FEATURE_ETSI_SMS_PS
  /* set the domain pref setting to be a invalid number.
     We will try to get the real number from WMS*/
  DSATUTIL_SET_VAL(DSATETSI_EXT_CGSMS_IDX,event_info->subs_id,0,0,
                                          dsat_cgsms_dflm.upper + 1,NUM_TYPE)
  /* get domain pref info */
#ifdef FEATURE_DUAL_SIM
  if ( wms_cfg_ms_get_gw_domain_pref( dsatsmsi_client_id,
                                      event_info->subs_id,
                                      dsatsmsi_cmd_cb_func,
                                      NULL) != WMS_OK_S )
  {
    DS_AT_MSG0_ERROR("fail to get domain pref info from WMS");
  }
#else
  if ( wms_cfg_get_gw_domain_pref(dsatsmsi_client_id, dsatsmsi_cmd_cb_func,
                                  NULL) != WMS_OK_S )
  {
    DS_AT_MSG0_ERROR("fail to get domain pref info from WMS");
  }
#endif /* FEATURE_DUAL_SIM */
#endif /* FEATURE_ETSI_SMS_PS */
  return DSAT_ASYNC_EVENT;
}/* dsatsmsi_gw_ready_cfg_event_handler */

#ifdef FEATURE_CDMA_SMS
/*===========================================================================

FUNCTION  DSATSMSI_CDMA_READY_CFG_EVENT_HANDLER

DESCRIPTION
  Handler function for cdma ready configuration event from WMS

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  Possible values are:
    DSAT_ASYNC_EVENT : the event is not related to an AT command

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatsmsi_cdma_ready_cfg_event_handler
(
 dsat_mode_enum_type mode, /* AT command mode    */
  sms_event_info_s_type  *event_info   /* Event info ptr    */
)
{
  /* WMS is ready now */
  dsatsmsi_wms_ready = TRUE;

  if (TRUE == dsatme_is_thin_ui())
  {
    (void) dsat707smsi_route_config ( MT_VAL_1, DS_VAL_0 );
  }

  return DSAT_ASYNC_EVENT;
}/* dsatsmsi_cdma_ready_cfg_event_handler */
#endif /* FEATURE_CDMA_SMS */


/*===========================================================================

FUNCTION  DSATSMS_MEMORY_STATUS_CFG_EVENT_HANDLER_EX

DESCRIPTION
  Handler function for memory status configuration event from WMS

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatsmsi_memory_status_cfg_event_handler_ex
(
  dsat_mode_enum_type mode, /* AT command mode    */
  sms_event_info_s_type  *event_info   /* Event info ptr    */
)
{
  dsat_result_enum_type  result = DSAT_ASYNC_EVENT;

#ifdef FEATURE_ETSI_SMS
/* +CIND URC is appliable only for GW memories */
  if (( event_info->u.mem_status.mem_store == WMS_MEMORY_STORE_RAM_GW )||
      ( event_info->u.mem_status.mem_store == WMS_MEMORY_STORE_SIM )||
      ( event_info->u.mem_status.mem_store == WMS_MEMORY_STORE_NV_GW ))
  {

  /* Check for clearing SMSFULL +CIND indicator */
    /*calculate index for CIND*/
    /* Check for clearing SMSFULL +CIND indicator */
    if( (DSAT_CIND_BINARY_SET == 
        dsatetsime_get_indicator_state( DSAT_CIND_INDEX_SMSFULL  , event_info->subs_id)) &&
        (dsatsms_full_mem_store[event_info->subs_id] == event_info->u.mem_status.mem_store) )
    {
      /* Check for empty slot */
      if (0 < event_info->u.mem_status.free_slots)
      {
        /* Update +CIND indicator */
        (void)dsatetsime_change_indicator_state( DSAT_CIND_INDEX_SMSFULL, 
                                                 DSAT_CIND_BINARY_UNSET,event_info->subs_id);
        dsatsms_full_mem_store[event_info->subs_id] = WMS_MEMORY_STORE_NONE;
      }
    }
    /* Check for setting SMSFULL +CIND indicator */
    else if (0 == event_info->u.mem_status.free_slots)
    {
      /* Update +CIND indicator */
      (void)dsatetsime_change_indicator_state( DSAT_CIND_INDEX_SMSFULL,
                                               DSAT_CIND_BINARY_SET,event_info->subs_id);
      dsatsms_full_mem_store[event_info->subs_id] = event_info->u.mem_status.mem_store;
    }
  }
#endif /* FEATURE_ETSI_SMS */

  if (DSAT_DL_VALIDATE_SYMBOL_ADDR(
        dsatdl_vtable.dsatsmsi_memory_status_cfg_event_handler_fp))
  {
    result =
      dsatdl_vtable.dsatsmsi_memory_status_cfg_event_handler_fp(mode, event_info);
  }

  return result;
} /* dsatsmsi_memory_status_cfg_event_handler_ex */

#ifdef FEATURE_ETSI_SMS
/*===========================================================================

FUNCTION  DSATSMSI_LINK_CTRL_EVENT_HANDLER

DESCRIPTION
  Handler function for link control events from WMS

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  Possible values are:
    DSAT_ASYNC_EVENT : the event is not related to an AT command

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatsmsi_link_ctrl_event_handler
(
  dsat_mode_enum_type    mode,         /* AT command mode    */
  sms_event_info_s_type  *event_info   /* Event info ptr    */
)
{
  ASSERT (event_info != NULL);

  /* Maintain link option setting locally */
  switch ( event_info->u.link_mode )
  {
    case WMS_LINK_CONTROL_ENABLED_ONE:
      DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_CMMS_ETSI_IDX,event_info->subs_id,0,0,
                                          SMS_LINK_CONTROL_ENABLED_ONE,NUM_TYPE)
      break;
    case WMS_LINK_CONTROL_ENABLE_ALL:
      DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_CMMS_ETSI_IDX,
                  event_info->subs_id,0,0,SMS_LINK_CONTROL_ENABLED_ALL,NUM_TYPE)
      break;
    default:
      DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_CMMS_ETSI_IDX,
                     event_info->subs_id,0,0,SMS_LINK_CONTROL_DISABLED,NUM_TYPE)
      break;
  }

  if ( dsatsmsi_pres_cmd.cmd_name == SMS_CMMS ) 
  {
    DSAT_SMS_STATE_CLEAN();
    return DSAT_OK;
  } 

  return DSAT_ASYNC_EVENT;
}/* dsatsmsi_link_ctrl_event_handler */
#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_ETSI_SMS_PS
/*===========================================================================

FUNCTION  DSATSMS_GW_DOMAIN_PREF_EVENT_HANDLER

DESCRIPTION
  Handler function for domain pref event from WMS

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  Possible values are:
    DSAT_OK          : if the command has been successfully executed
    DSAT_ASYNC_EVENT : if we are not running +CGSMS command

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatsmsi_gw_domain_pref_event_handler
(
  dsat_mode_enum_type mode, /* AT command mode    */
  sms_event_info_s_type  *event_info   /* Event info ptr    */
)
{
  dsat_result_enum_type         result;
  wms_gw_domain_pref_e_type     domain_pref = event_info->u.gw_domain;
  dsat_num_item_type            srv_num;
  dsat_num_item_type            cgsms_val;

  if(dsatsmsi_domain_pref_to_srv(domain_pref, &srv_num))
  {
    if(srv_num <= dsat_cgsms_dflm.upper && srv_num >= dsat_cgsms_dflm.lower)
    {
      /* Preference could have been set via UI or +CGSMS command:
         If via UI just save new setting, if via +CGSMS command values 0-3 can
         be set but only values of 2 or 3 will be passed to WMS. 
         So on reponse from WMS due to +CGSMS write command srv_num may 
         indicate an "as set" or a value mapped from 0/1 to 2/3. If value 
         returned by WMS doesn't indicate "as set" or mapped value something
         odd has happened so just save set value */
      cgsms_val =(dsat_num_item_type)dsatutil_get_val(
                         DSATETSI_EXT_CGSMS_IDX,event_info->subs_id,0,NUM_TYPE);
      if ( dsatsmsi_pres_cmd.cmd_name != SMS_CGSMS ||
            (srv_num != cgsms_val && (srv_num - 2) != cgsms_val) )
      {
        DSATUTIL_SET_VAL(DSATETSI_EXT_CGSMS_IDX,event_info->subs_id,0,0,srv_num,NUM_TYPE)
      }

      result = DSAT_OK;
    }
    else
    {
      /* srv_num is not supported */
      result = DSAT_ERROR;
    }
  }
  else
  {
    /* wrong srv_num */
    result = DSAT_ERROR;
  }
  
  if(dsatsmsi_pres_cmd.cmd_name != SMS_CGSMS)
  {
    if(result == DSAT_ERROR)
    {
      DS_AT_MSG1_ERROR("wrong domain pref",domain_pref);
    }
    result = DSAT_ASYNC_EVENT;
  }
  else
  {
    DSAT_SMS_STATE_CLEAN();
  }

  return result;

}/* dsatsmsi_gw_domain_pref_event_handler */
#endif /* FEATURE_ETSI_SMS_PS */


/*===========================================================================
FUNCTION  CFG_EVENT_LOOKUP

DESCRIPTION
  Maps the wms_cfg_event_e_type to sms_event_e_type

DEPENDENCIES
  None

RETURNS
  sms_event_e_type

SIDE EFFECTS
  None
===========================================================================*/
sms_event_e_type cfg_event_lookup
( 
 wms_cfg_event_e_type wms_event /* Event ID */
)
{
  sms_event_e_type atcop_event;

  switch ( wms_event )
  {
#ifndef FEATURE_DUAL_SIM
  case WMS_CFG_EVENT_GW_READY:
    atcop_event = SMS_CFG_EVENT_GW_READY;
    break;
#ifdef FEATURE_CDMA_SMS
  case WMS_CFG_EVENT_CDMA_READY:
    atcop_event = SMS_CFG_EVENT_CDMA_READY;
    break;
#endif /* FEATURE_CDMA_SMS */
#else
  case WMS_CFG_EVENT_MS_READY:
    atcop_event = SMS_CFG_EVENT_MS_READY;
    break;
#endif /* FEATURE_DUAL_SIM */
  case WMS_CFG_EVENT_ROUTES:
    atcop_event = SMS_CFG_EVENT_ROUTES;
    break;
  case WMS_CFG_EVENT_MEMORY_STATUS:
    atcop_event = SMS_CFG_EVENT_MEMORY_STATUS;
    break;
  case WMS_CFG_EVENT_MESSAGE_LIST:
    atcop_event = SMS_CFG_EVENT_MESSAGE_LIST;
    break;
  case WMS_CFG_EVENT_LINK_CONTROL:
    atcop_event = SMS_CFG_EVENT_LINK_CONTROL;
    break; 
#ifdef FEATURE_ETSI_SMS_PS
#ifdef FEATURE_DUAL_SIM
  case WMS_CFG_EVENT_MS_GW_DOMAIN_PREF:
#else
  case WMS_CFG_EVENT_GW_DOMAIN_PREF:
#endif /* FEATURE_DUAL_SIM */
    atcop_event = SMS_CFG_EVENT_GW_DOMAIN_PREF;
    break;
#endif /* FEATURE_ETSI_SMS_PS */
#ifdef FEATURE_DSAT_EXTENDED_SMS
  case WMS_CFG_EVENT_MEMORY_FULL:
#if FEATURE_DUAL_SIM
  case WMS_CFG_EVENT_MS_MEMORY_FULL:
#endif /* FEATURE_DUAL_SIM */
    atcop_event = SMS_CFG_EVENT_MEMORY_FULL;
    break;
#endif /* FEATURE_DSAT_EXTENDED_SMS */
  default:
    atcop_event = SMS_EVENT_NONE;
    break;
  }

  return atcop_event;
}

/*===========================================================================
FUNCTION  MSG_EVENT_LOOKUP

DESCRIPTION
  Maps the wms_msg_event_e_type to sms_event_e_type

DEPENDENCIES
  None

RETURNS
  sms_event_e_type

SIDE EFFECTS
  None
===========================================================================*/
sms_event_e_type msg_event_lookup
(
 wms_msg_event_e_type wms_event /* Event ID */
)
{
  sms_event_e_type atcop_event;

  switch ( wms_event )
  {
    case WMS_MSG_EVENT_SEND:
      atcop_event = SMS_MSG_EVENT_SEND;
      break;
    case WMS_MSG_EVENT_ACK:
      atcop_event = SMS_MSG_EVENT_ACK;
      break;
    case WMS_MSG_EVENT_READ:
      atcop_event = SMS_MSG_EVENT_READ;
      break;
    case WMS_MSG_EVENT_WRITE:
      atcop_event = SMS_MSG_EVENT_WRITE;
      break;
    case WMS_MSG_EVENT_DELETE:
      atcop_event = SMS_MSG_EVENT_DELETE;
      break;
    case WMS_MSG_EVENT_MODIFY_TAG:
      atcop_event = SMS_MSG_EVENT_MODIFY_TAG;
      break;
    case WMS_MSG_EVENT_READ_TEMPLATE:
      atcop_event = SMS_MSG_EVENT_READ_TEMPLATE;
      break;
    case WMS_MSG_EVENT_WRITE_TEMPLATE:
      atcop_event = SMS_MSG_EVENT_WRITE_TEMPLATE;
      break;
    case WMS_MSG_EVENT_DELETE_TEMPLATE:
      atcop_event = SMS_MSG_EVENT_DELETE_TEMPLATE;
      break;
    case WMS_MSG_EVENT_RECEIVED_MESSAGE:
      atcop_event = SMS_MSG_EVENT_RECEIVED_MESSAGE;
      break;
    case WMS_MSG_EVENT_SUBMIT_REPORT:
      atcop_event = SMS_MSG_EVENT_SUBMIT_REPORT;
      break;
    case WMS_MSG_EVENT_STATUS_REPORT:
      atcop_event = SMS_MSG_EVENT_STATUS_REPORT;
      break;
    case WMS_MSG_EVENT_MT_MESSAGE_ERROR:
      atcop_event = SMS_MSG_EVENT_MT_MESSAGE_ERROR;
      break;
    case WMS_MSG_EVENT_READ_STS_REPORT:
      atcop_event = SMS_MSG_EVENT_READ_STS_REPORT;
      break;
    case WMS_MSG_EVENT_WRITE_STS_REPORT:
      atcop_event = SMS_MSG_EVENT_WRITE_STS_REPORT;
      break;
    default:
      atcop_event = SMS_EVENT_NONE;
      break;
  }

  return atcop_event;
  }


/*===========================================================================
FUNCTION  SETUP_SMS_WATERMARKS

DESCRIPTION
  This function initializes the watermarks used for Receiving SMS data
  from WMS. The lowater, hiwater and don't exceed counts are configured. 
  Queue to be used by WM is also initialized

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void setup_sms_watermarks( void )
{

  /*-------------------------------------------------------------------------
    Initialize, set Don't Exceed count and register the associated queue 
    for storing MT messages.
  -------------------------------------------------------------------------*/
  dsm_queue_init(&dsat_mt_msg_wm, DSAT_FROM_WMS_DONT_EXCEED_CNT, &dsat_mt_msg_q);

  /*-------------------------------------------------------------------------
    Set the Lo Water and Hi Water counts for the 
    MT message watermark.
  -------------------------------------------------------------------------*/
  dsat_mt_msg_wm.lo_watermark    =  DSAT_FROM_WMS_LOWATER_CNT;
  dsat_mt_msg_wm.hi_watermark    =  DSAT_FROM_WMS_HIWATER_CNT;

  /*-------------------------------------------------------------------------
    Initialize the watermark internal byte counts for the MT message watermark.
  -------------------------------------------------------------------------*/
  dsat_mt_msg_wm.current_cnt = 0;
#ifdef FEATURE_DSM_MEM_CHK
  dsat_mt_msg_wm.highest_cnt = 0;
  dsat_mt_msg_wm.total_rcvd_cnt = 0;
#endif /* FEATURE_DSM_MEM_CHK */

  /*-------------------------------------------------------------------------
    Register callback functions for the MT message watermark.
  -------------------------------------------------------------------------*/
  /*lint -save -e64 suppress error 64*/
  dsat_mt_msg_wm.lowater_func_ptr      = sms_wm_lowater_cb;
  dsat_mt_msg_wm.hiwater_func_ptr      = sms_wm_hiwater_cb;
  dsat_mt_msg_wm.each_enqueue_func_ptr = NULL;
  dsat_mt_msg_wm.gone_empty_func_ptr   = NULL;
  dsat_mt_msg_wm.non_empty_func_ptr    = sms_mt_msg_wm_non_empty_cb;
  /*lint -restore suppress error 64*/

  /*-------------------------------------------------------------------------
    Initialize, set Don't Exceed count and register the associated queue 
    for storing other than MT messages.
  -------------------------------------------------------------------------*/
  dsm_queue_init(&dsat_sms_wm, DSAT_FROM_WMS_DONT_EXCEED_CNT, &dsat_sms_q);

  /*-------------------------------------------------------------------------
    Set up the watermark and queue for SMS events other than 
    MT message from WMS. This watermark will not be flow controlled.
  -------------------------------------------------------------------------*/
  dsat_sms_wm.lo_watermark    =  0;
  dsat_sms_wm.hi_watermark    =  DSAT_FROM_WMS_DONT_EXCEED_CNT;
  dsat_sms_wm.current_cnt = 0;
#ifdef FEATURE_DSM_MEM_CHK
  dsat_sms_wm.highest_cnt = 0;
  dsat_sms_wm.total_rcvd_cnt = 0;
#endif /* FEATURE_DSM_MEM_CHK */

  dsat_sms_wm.lowater_func_ptr      = NULL;
  dsat_sms_wm.hiwater_func_ptr      = NULL;
  dsat_sms_wm.each_enqueue_func_ptr = NULL;
  dsat_sms_wm.gone_empty_func_ptr   = NULL;
  /*lint -save -e64 suppress error 64*/
  dsat_sms_wm.non_empty_func_ptr    = sms_wm_non_empty_cb;
  /*lint -restore suppress error 64*/

} /* setup_sms_watermarks */

/* <EJECT> */
/*===========================================================================

FUNCTION SMS_WM_HIWATER_CB

DESCRIPTION
  This is the high watermark function registered with the actual watermark.
  It is added to deregister WMS callbacks once watermark goes full.
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
void sms_wm_hiwater_cb (dsm_watermark_type *wm, void *v)
  {
  /* Set flag indicating received messages should be dropped */
  sms_mt_msg_wm_full = TRUE;
  }

/* <EJECT> */
/*===========================================================================

FUNCTION SMS_WM_LOWATER_CB

DESCRIPTION
  This is the low watermark function registered with the actual watermark.
  It is added to re-register for WMS callbacks once watermark goes low.
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
void sms_wm_lowater_cb (dsm_watermark_type *wm, void *v)
  {
  /* Set flag indicating received messages should no longer be dropped */
  sms_mt_msg_wm_full = FALSE;
  }


/*===========================================================================

FUNCTION SMS_MT_MSG_WM_NON_EMPTY_CB

DESCRIPTION
  Callback function invoked when the received message watermark goes non-empty.

  This function simply sets the received MT message signal.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void sms_mt_msg_wm_non_empty_cb (dsm_watermark_type *wm, void *v)
{
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  (void)rex_set_sigs( rcinit_lookup_rextask("ds"), DS_AT_MT_MSG_SIG );
#else
  (void)rex_set_sigs( &ds_tcb, DS_AT_MT_MSG_SIG );
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */

} /* sms_mt_msg_wm_non_empty_cb */


/*===========================================================================

FUNCTION SMS_WM_NON_EMPTY_CB

DESCRIPTION
  Callback function invoked when the SMS event watermark item goes 
  non-empty.

  This function simply sets the other, non-"MT message" SMS event signal.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void sms_wm_non_empty_cb (dsm_watermark_type *wm, void *v)
{
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  (void)rex_set_sigs( rcinit_lookup_rextask("ds"), DS_AT_SMS_SIG );
#else
  (void)rex_set_sigs( &ds_tcb, DS_AT_SMS_SIG );
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */

} /* sms_wm_non_empty_cb */

/*===========================================================================

FUNCTION    QUEUE_MT_MSG

DESCRIPTION
  Adds unsolicited response result code to a queue.
  If the queue is full then the first element is deleted and the new
  element is replaces it.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void queue_mt_msg
(
 sms_mt_msg_e_type       msg_type,   /* Type of message      */
 wms_message_index_type  msg_index,  /* Index Number         */
 wms_memory_store_e_type mem_store,  /* Type of Memory Store */
 wms_transaction_id_type tid,         /* Transaction ID       */
 dsati_operating_cmd_mode_type cmd_mode
)
{
  sms_mt_msg_type * msg_ptr;
  q_type  *msg_q_ptr;
  q_type  *msg_free_q_ptr;
  
  if(cmd_mode == ETSI_CMD_MODE)
  {
    msg_q_ptr = &sms_mt_msg_q;
    msg_free_q_ptr= &sms_mt_msg_free_q;
  }
  else
  {
    msg_q_ptr = &is707_sms_mt_msg_q;
    msg_free_q_ptr= &is707_sms_mt_msg_free_q;
  }

  /* Get a command buffer from the free command queue */
  if( (msg_ptr = q_get( msg_free_q_ptr )) == NULL )
  {
    /* No free command buffers available                */
    /* Get the tip, start adding the new element at tip */

    /* Get the first element in the actual queue */
    msg_ptr = q_get( msg_q_ptr );
    ASSERT(msg_ptr != NULL);
    /* Add data to it */
    msg_ptr->msg_type = msg_type;
    msg_ptr->index = msg_index;
    msg_ptr->mem_store = mem_store;
    msg_ptr->tid = tid;
   
    /* Put the message on the message queue */
    q_put( msg_q_ptr, &msg_ptr->link );

  }
  else
  {
    msg_ptr->msg_type = msg_type;
    msg_ptr->index = msg_index;
    msg_ptr->mem_store = mem_store;
    msg_ptr->tid = tid;

    /* Put the message on the message queue */
    q_put( msg_q_ptr, &msg_ptr->link );
  }

  return;
} /* add_mt_msg_to_queue */

#endif /* defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS) */

/*===========================================================================

FUNCTION    DSATSMS_STATE_CLEAN

DESCRIPTION
  This function cleans up ATCoP SMS state variables.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatsms_state_clean
(
  const char *filename,
  uint32      line,
  boolean     cmd_clean
)
{
  DS_AT_MSG_SPRINTF_4_HIGH("Cleaning up ATCoP SMS state variables cmd %d HC cmd %d "
                           "(file:%s, %d)", dsatsmsi_pres_cmd.cmd_name, 
                           dsatsmsi_pres_cmd.is_hc_cmd, filename, line);

  dsatsmsi_pres_cmd.cmd_name = SMS_NONE;
  dsatsmsi_pres_cmd.is_hc_cmd = FALSE;
  dsatsmsi_pres_cmd.cmd_hdr[0] = '\0';

  if (cmd_clean)
  {
    memset ( &dsatsmsi_cmd_sm, 0, sizeof(sms_cmd_sm_type) );
  }

  return;
} /* dsatsms_state_clean */

/*===========================================================================

FUNCTION    DSATSMS_SET_STATE

DESCRIPTION
  This function sets up ATCoP SMS state variables.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatsms_set_state
(
  sms_cmd_name_e_type  tcmd_name,
  byte                *tcmd_hdr,
  boolean              hc_cmd
)
{
  DS_AT_MSG2_MED("Setting up ATCoP SMS state variables cmd %d HC cmd %d",
                 tcmd_name, hc_cmd);

  dsatsmsi_pres_cmd.cmd_name = tcmd_name;
  (void)strlcpy(dsatsmsi_pres_cmd.cmd_hdr, (const char*) tcmd_hdr,
                sizeof(dsatsmsi_pres_cmd.cmd_hdr));
  dsatsmsi_pres_cmd.is_hc_cmd = hc_cmd;

  return;
} /* dsatsms_set_state */

