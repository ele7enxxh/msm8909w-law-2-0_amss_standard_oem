/******************************************************************************
  @file    ps_wmk_logging.c
  @brief   

  DESCRIPTION
  This file defines the API used for generating DIAG events with 
  watermark information 
 
  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (C) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/src/ps_wmk_logging.c#1 $
  $Author: mplcsds1 $
  $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/11/13    svj     Initial version.

===========================================================================*/
/*===========================================================================

                             INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "customer.h"


#include "msg.h"
#include "ds_Utils_DebugMsg.h"

#include "ps_wmk_logging.h"
#include "event_defs.h"
#include "event.h"           /* For logging DIAG event and event codes      */
#include "ps_sys.h"
#include "ps_ifacei_utils.h"
#include "log.h"             /* to log the packet */
#include "log_codes.h"
#include "ds_sys.h"
#include "ps_iface_errors_def.h"

/*===========================================================================

                           LOCAL DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Data type used to log watermark levels
---------------------------------------------------------------------------*/
typedef struct
{ 
  ps_phys_link_type              *um_phys_link_ptr;
  uint32                          rx_wmk_curr_cnt;	
  uint32                          tx_wmk_curr_cnt;	
  ps_wmk_logging_dorm_state_type  dormancy_status;
}ps_wmk_logging_events_type;

typedef struct
{ 
  ps_phys_link_type              *um_phys_link_ptr;
  uint32                          rx_wmk_curr_cnt;	
  uint32                          tx_wmk_curr_cnt;	
  ps_wmk_logging_dorm_state_type  dormancy_status;
  uint8                           tech_type;
  uint32                          profile_id;
  uint32                          addr_family;	
}ps_wmk_logging_events_ex_type;

/*---------------------------------------------------------------------------
  Data type used to log the IP packet which causes reorigination from
  dormancy
---------------------------------------------------------------------------*/
typedef struct
{ 
  uint8                              version;
  uint8                              reserved;	
  ps_sys_tech_enum_type              tech_type;
  ps_wmk_logging_pkt_direction_type  direction;  	
  byte                               packet[PS_WMK_LOGGING_REORIG_PKT_LOG_SIZE];
}ps_wmk_logging_reorig_log_pkt_type;

/*===========================================================================

                      PUBLIC FUNCTION DEFINITIONS

===========================================================================*/

int ps_wmk_logging_event_report
(
  ps_phys_link_type               *um_phys_link_ptr,
  event_id_enum_type               event_id,
  ps_wmk_logging_dorm_state_type   dormancy_status,
  uint32                           rx_wm_curr_cnt,
  uint32                           tx_wm_curr_cnt
)
{
   ps_wmk_logging_events_type ps_wmk_logging_events_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   memset(&ps_wmk_logging_events_info, 0, sizeof(ps_wmk_logging_events_info));

   if(um_phys_link_ptr == NULL)
   {
      LOG_MSG_ERROR_0("ps_wmk_logging_event_report: Null physlink ptr");
      return -1;
   }

   ps_wmk_logging_events_info.um_phys_link_ptr = um_phys_link_ptr;
   ps_wmk_logging_events_info.rx_wmk_curr_cnt  = rx_wm_curr_cnt;
   ps_wmk_logging_events_info.tx_wmk_curr_cnt  = tx_wm_curr_cnt;
   ps_wmk_logging_events_info.dormancy_status  = dormancy_status;
   
   event_report_payload (event_id,
                         sizeof(ps_wmk_logging_events_type),
                         (void*) &ps_wmk_logging_events_info); 
   return 0;

} /* ps_wmk_logging_event_report */

int ps_wmk_logging_event_report_ex
(
  ps_phys_link_type               *um_phys_link_ptr,
  event_id_enum_type               event_id,
  ps_wmk_logging_dorm_state_type   dormancy_status,
  uint32                           rx_wm_curr_cnt,
  uint32                           tx_wm_curr_cnt,
  ps_iface_type                   *iface_ptr
)
{
    ps_wmk_logging_events_ex_type ps_wmk_logging_events_info_ex;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    memset(&ps_wmk_logging_events_info_ex, 0, sizeof(ps_wmk_logging_events_ex_type));

    if(um_phys_link_ptr == NULL)
    {
       LOG_MSG_ERROR_0("ps_wmk_logging_event_report_ex: Null physlink ptr");
       return -1;
    }

    if(iface_ptr == NULL)
    {
       LOG_MSG_ERROR_0("ps_wmk_logging_event_report_ex: Null iface ptr");
       return -1;
    }

    ps_wmk_logging_events_info_ex.um_phys_link_ptr = um_phys_link_ptr;
    ps_wmk_logging_events_info_ex.rx_wmk_curr_cnt  = rx_wm_curr_cnt;
    ps_wmk_logging_events_info_ex.tx_wmk_curr_cnt  = tx_wm_curr_cnt;
    ps_wmk_logging_events_info_ex.dormancy_status  = dormancy_status;

    if(iface_ptr->name == UMTS_IFACE)
    {
      /* 3gpp event */
      ps_wmk_logging_events_info_ex.profile_id = 
        PS_IFACEI_GET_3GPP_PROFILE_ID(iface_ptr);
      ps_wmk_logging_events_info_ex.tech_type  = PS_SYS_TECH_3GPP;
      ps_wmk_logging_events_info_ex.addr_family = 
        (uint32) ps_iface_get_addr_family(iface_ptr);
        LOG_MSG_ERROR_2("ps_wmk_logging_event_report_ex() profile id = %d,"
                        "addr family = %d",
                         ps_wmk_logging_events_info_ex.profile_id,
                         ps_wmk_logging_events_info_ex.addr_family);

    }
    else if(iface_ptr->name == CDMA_SN_IFACE)
    {
      /* 3gpp2 event */
      ps_wmk_logging_events_info_ex.tech_type  = PS_SYS_TECH_3GPP2;

      if(dormancy_status == PS_WMK_LOGGING_DORMANT)
      {
        //Ignoring profile id/ip type for 3gpp2 dormant event
        ps_wmk_logging_events_info_ex.addr_family  = 0xFF;
        ps_wmk_logging_events_info_ex.profile_id   = 0xFF;  
        LOG_MSG_ERROR_2("ps_wmk_logging_event_report_ex() profile id = %d,"
                        "addr family = %d",
                         ps_wmk_logging_events_info_ex.addr_family,
                         ps_wmk_logging_events_info_ex.profile_id);

      }
      else
      {
        ps_wmk_logging_events_info_ex.profile_id = 
          PS_IFACEI_GET_3GPP2_PROFILE_ID(iface_ptr);
        ps_wmk_logging_events_info_ex.addr_family = 
          (uint32) ps_iface_get_addr_family(iface_ptr);
        LOG_MSG_ERROR_2("ps_wmk_logging_event_report_ex() profile id = %d,"
                        "addr family = %d",
                         ps_wmk_logging_events_info_ex.profile_id,
                         ps_wmk_logging_events_info_ex.addr_family);
      }
    }
    else
    {
      LOG_MSG_ERROR_0("ps_wmk_logging_event_report_ex() : "
                      "Invalid tech type: Not logging event!");
      return -1;
    }
      
    event_report_payload (event_id,
                          sizeof(ps_wmk_logging_events_ex_type),
                          (void*) &ps_wmk_logging_events_info_ex); 
    return 0;

} /* ps_wmk_logging_event_report_ex */

int ps_wmk_logging_modem_embedded_calls_info
(
  ps_phys_link_type               *um_phys_link_ptr,
  ps_wmk_logging_dorm_state_type   dormancy_status
)
{
  ps_iface_type                   *iface_ptr = NULL;
  uint32                           rx_wm_curr_cnt;
  uint32                           tx_wm_curr_cnt;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_0("ps_wmk_logging_modem_embedded_calls_info()");

  if(um_phys_link_ptr == NULL)
  {
     LOG_MSG_ERROR_0("ps_wmk_logging_modem_embedded_calls_info: Null physlink ptr");
     return -1;
  }

  /* Retrieve iface ptr */
  if(um_phys_link_ptr->higher_layer_protocol == 
       PS_PHYS_LINK_HIGHER_LAYER_PROTOCOL_IPV4)
  {
    iface_ptr = um_phys_link_ptr->rx_cmd_info.v4_iface_ptr;
  }
  else if(um_phys_link_ptr->higher_layer_protocol == 
            PS_PHYS_LINK_HIGHER_LAYER_PROTOCOL_IPV6)
  {
    iface_ptr = um_phys_link_ptr->rx_cmd_info.v6_iface_ptr;
  }
  else
  {
    LOG_MSG_ERROR_0("ps_wmk_logging_modem_embedded_calls_info() "
                    "Invalid  higher layer protocol: Not logging event!");
    return -1;
  }

  if(iface_ptr == NULL)
  {
     LOG_MSG_ERROR_0("ps_wmk_logging_modem_embedded_calls_info() Null iface ptr");
     return -1;
  }

  /* Retrieve wm counts */
  rx_wm_curr_cnt = iface_ptr->iface_i_stats.pkts_rx;
  tx_wm_curr_cnt = iface_ptr->iface_i_stats.pkts_tx;

  LOG_MSG_ERROR_2("ps_wmk_logging_modem_embedded_calls_info() rx_wm_curr_cnt = %d,tx_wm_curr_cnt = %d",
                   rx_wm_curr_cnt,
                   tx_wm_curr_cnt);


  ps_wmk_logging_event_report_ex(um_phys_link_ptr, 
                                 EVENT_DS_UM_QUEUE_STATS_EX,
                                 dormancy_status,
                                 rx_wm_curr_cnt,
                                 tx_wm_curr_cnt,
                                 iface_ptr);
  return 0;

} /* ps_wmk_logging_modem_embedded_calls_info */

/*===========================================================================
FUNCTION PS_WMK_LOGGING_FIRST_REORIG_IP_PKT_HDR

DESCRIPTION
  This function logs the first IP packet which causes reorigination
  from dormancy. Only the first 100 bytes of the packet is logged.

PARAMETERS
  tech_type     : technology type (3gpp/3gpp2)
  pkt_direction : uplink / downlink packet
  ip_pkt_ptr    : the data byes of the packet

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_wmk_logging_first_reorig_ip_pkt_hdr
(
  ds_sys_tech_enum_type                  tech_type,
  ps_wmk_logging_pkt_direction_type      pkt_direction,
  byte                                 * ip_pkt_ptr
)
{
  byte                                 * log_ptr;     /* pointer to log buffer       */
  uint16                                 len = 0;     /* length of log frame payload */
  ps_wmk_logging_reorig_log_pkt_type   * log_pkt_ptr; /* pointer to payload in the log pkt */
  byte                                 * log_payload_ptr; /* pointer to payload in the log pkt */
  log_code_type                          log_code = LOG_DS_FIRST_REORIG_IP_PACKET_HEADER_C;
  uint32                                 ret_val = -1;
  uint32                                 err_no  = PS_IFACE_ERR_NONE;    
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  do
  {
    if (NULL == ip_pkt_ptr)
    {
      err_no = PS_IFACE_ERR_NULL_ARG; 
      break;
    }
    
    /* Size of the payload */
    len = sizeof(ps_wmk_logging_reorig_log_pkt_type);
  
    /* allocate memory for the log packet */
    log_ptr = (byte *)log_alloc(log_code, len + sizeof(log_hdr_type));
  
    if (NULL == log_ptr)
    {
      err_no = PS_IFACE_ERR_OUT_OF_DIAG_BUF; 
      break;
    }
  
    /* Populate the payload information */
    log_payload_ptr        = log_ptr + sizeof(log_hdr_type);
    log_pkt_ptr            = (ps_wmk_logging_reorig_log_pkt_type*)log_payload_ptr;
    log_pkt_ptr->version   = 1;
    log_pkt_ptr->reserved  = 0;
    log_pkt_ptr->tech_type = (ps_sys_tech_enum_type)tech_type;
    log_pkt_ptr->direction = pkt_direction;
    memset(log_pkt_ptr->packet, 0, PS_WMK_LOGGING_REORIG_PKT_LOG_SIZE);
    memscpy(log_pkt_ptr->packet, PS_WMK_LOGGING_REORIG_PKT_LOG_SIZE,
            ip_pkt_ptr, PS_WMK_LOGGING_REORIG_PKT_LOG_SIZE);
    
    /*-------------------------------------------------------------------------
      Dispatch log buffer to diag for delivery to diagnostic monitor
    -------------------------------------------------------------------------*/
    log_commit(log_ptr);

    ret_val = 0;

  } while(0);

  LOG_MSG_ERROR_1("ps_wmk_logging_first_reorig_ip_pkt_hdr(): "
                  "Error code %d", err_no);

  return ret_val;

}/* ps_wmk_logging_first_reorig_ip_pkt_hdr */

/*===========================================================================
FUNCTION PS_WMK_LOGGING_REORIG_PKT

DESCRIPTION
  This function is used by the mode handler to log the first packet
  which causes reorigination from dormancy. The dsm item pointer is
  passed from which the bytes to be logged is extracted.

PARAMETERS
  tech_type     : technology type (3gpp/3gpp2)
  pkt_direction : uplink / downlink packet
  item_ptr      : dsm item pointer

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

int ps_wmk_logging_reorig_pkt
(
  ds_sys_tech_enum_type                 tech_type,
  ps_wmk_logging_pkt_direction_type     pkt_direction,
  dsm_item_type                       * item_ptr 
)
{
  uint16                                 offset = 0;  /* contains the offset value   */
  byte                                   packet[PS_WMK_LOGGING_REORIG_PKT_LOG_SIZE]= {0};
  uint16                                 pkt_len = PS_WMK_LOGGING_REORIG_PKT_LOG_SIZE;
  uint32                                 ret_val = -1;
  uint32                                 err_no  = PS_IFACE_ERR_NONE;    
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  do
  {
    if (NULL == item_ptr)
    {
      err_no  = PS_IFACE_ERR_NULL_ARG; 
      break;
    }
  
    /*-----------------------------------------------------------------------
      Extract data from item_ptr into log buffer(log_ptr).
    -----------------------------------------------------------------------*/
    if (dsm_extract(item_ptr, offset, packet, pkt_len) == FALSE)
    {
      err_no  = PS_IFACE_ERR_DSM_EXTRACT_FAILURE; 
      break;
    }
  
    ret_val = ps_wmk_logging_first_reorig_ip_pkt_hdr(tech_type, 
                                                     pkt_direction, 
                                                     packet);

  } while(0);

  LOG_MSG_ERROR_1("ps_wmk_logging_reorig_pkt(): "
                  "Error code %d", err_no);

  return ret_val;

} /* ps_wmk_logging_reorig_pkt */

