/**
   @file  ps_wmk_logging.h
   @brief This file defines the API used for generating DIAG events with 
          watermark information 
*/

#ifndef PS_WMK_LOGGING_H
#define PS_WMK_LOGGING_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                       P S _ W M K _ L O G G I N G . H


GENERAL DESCRIPTION
  This file defines the API used for generating DIAG events with 
watermark information 

Copyright (c) 2013 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/ 
 
/*=============================================================================

                            EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_wmk_logging.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $
 
when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/09/13    svj    Initial version
===========================================================================*/

/*=============================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "event_defs.h"
#include "ps_phys_link.h"
#include "ps_iface.h"
#include "ds_sys.h"

#ifdef __cplusplus
extern "C" {
#endif
/*===========================================================================

                      EXTERNAL DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Data channel dormancy enum type
---------------------------------------------------------------------------*/
typedef enum 
{
  PS_WMK_LOGGING_DORMANT               = 1,
  PS_WMK_LOGGING_REORIG_FROM_DORMANCY  = 2,
  PS_WMK_LOGGING_ACTIVE                = PS_WMK_LOGGING_REORIG_FROM_DORMANCY
}ps_wmk_logging_dorm_state_type;

/*---------------------------------------------------------------------------
  Packet direction enum type
---------------------------------------------------------------------------*/
typedef enum
{
  PS_WMK_UPLINK_PKT                    = 0,
  PS_WMK_DOWNLINK_PKT                  = 1
}ps_wmk_logging_pkt_direction_type;

/*---------------------------------------------------------------------------
  Maximum no of data bytes logged in the log packet
---------------------------------------------------------------------------*/
#define PS_WMK_LOGGING_REORIG_PKT_LOG_SIZE  100


/*=============================================================================

                        PUBLIC FUNCTION DECLARATIONS

=============================================================================*/

/** 
  @brief  This function logs Watermark counts as DIAG events.

  @param[in] um_phys_link_ptr   ptr to interface control block on which to
                                operate on.
  @param[in] event_id           Event id generated over DIAG 
  @param[in] dormancy_status    Callback for event  
  @param[in] rx_wm_curr_cnt     Receive wmk current count
  @param[in] tx_wm_curr_cnt     Transit wmk current count 

  @retval 0                     Event Logging success
  @retval -1                    Event Logging failed
 
*/

int ps_wmk_logging_event_report
(
  ps_phys_link_type               *um_phys_link_ptr,
  event_id_enum_type               event_id,
  ps_wmk_logging_dorm_state_type   dormancy_status,
  uint32                           rx_wm_curr_cnt,
  uint32                           tx_wm_curr_cnt
);

/**
  @brief  This function logs Watermark counts as DIAG events.
	 
  @param[in] um_phys_link_ptr   ptr to physlink on which to
 	                        operate on.
  @param[in] event_id           Event id generated over DIAG
  @param[in] dormancy_status    Dormancy status
  @param[in] rx_wm_curr_cnt     Receive wmk current count
  @param[in] tx_wm_curr_cnt     Transit wmk current count
  @param[in] iface_ptr          ptr to interface control block
                                on which to operate on.
  
   @retval 0                    Event Logging success
   @retval -1                   Event Logging failed
  	 
  */
  	 
int ps_wmk_logging_event_report_ex
(
  ps_phys_link_type               *um_phys_link_ptr,
  event_id_enum_type               event_id,
  ps_wmk_logging_dorm_state_type   dormancy_status,
  uint32                           rx_wm_curr_cnt,
  uint32                           tx_wm_curr_cnt,
  ps_iface_type                   *iface_ptr
);
  	 
 /**
  @brief  This function logs wm counts for modem embedded calls.
  	 
  @param[in] um_phys_link_ptr   ptr to physlink on which to
   	                        operate on.
  @param[in] dormancy_status    Dormancy status
  	 
  @retval  0                    Event Logging success
  @retval -1                    Event Logging failed
   	 
 */
  	 
int ps_wmk_logging_modem_embedded_calls_info
(
  ps_phys_link_type               *um_phys_link_ptr,
  ps_wmk_logging_dorm_state_type   dormancy_status
);

 /**
  @brief  This function logs the first IP packet which causes 
          reorigination from dormancy. Only the first 100 bytes of 
          the packet is logged.
  	 
  @param[in] tech_type      technology type (3gpp/3gpp2)
  @param[in] pkt_direction  uplink/downlink packet
  @param[in] item_ptr       dsm item of the first IP packet which 
                            comes out of dormancy
  	 
  @retval  0                Packet Logging success
  @retval -1                Packet Logging failed
   	 
 */
  	 
int ps_wmk_logging_reorig_pkt
(
  ds_sys_tech_enum_type                  tech_type,
  ps_wmk_logging_pkt_direction_type      pkt_direction,
  dsm_item_type                        * item_ptr 
);

 /**
  @brief  This function logs the first IP packet.
  	 
  @param[in] tech_type      technology type (3gpp/3gpp2)
  @param[in] pkt_direction  uplink/downlink packet
  @param[in] ip_pkt_ptr     bytes of the first IP packet which 
                            comes out of dormancy
  	 
  @retval  0                Packet Logging success
  @retval -1                Packet Logging failed
   	 
 */
  	 
int ps_wmk_logging_first_reorig_ip_pkt_hdr
(
  ds_sys_tech_enum_type                  tech_type,
  ps_wmk_logging_pkt_direction_type      pkt_direction,
  byte                                 * ip_pkt_ptr
);

#ifdef __cplusplus
}
#endif

#endif /* PS_WMK_LOGGING_H */

