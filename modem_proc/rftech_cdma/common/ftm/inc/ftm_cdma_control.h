#ifndef FTM_CDMA_CONTROL_H
#define FTM_CDMA_CONTROL_H

/*!
  @file
  ftm_cdma_control.h

  @brief
  This module contains defintions and prototypes to be used by CDMA FTM.
*/

/*==============================================================================

  Copyright (c) 2011 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

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

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/ftm/inc/ftm_cdma_control.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
11/19/14   spa     Added FTM concurrecny manager check,done in single enable API
06/18/13   JJ      Added two enum type for Register/De-register FW msg
05/14/13   cc      Defined an enum to handle NEW RSP packet to Diag
04/12/13   aro     Debug enable interfaces to enable calv3 debug mode
08/09/11   aro     Moved send FTM command from CDMA to FTM common
07/27/11   aro     Added generic FTM send command function for CDMA
03/28/11   aro     Renamed function to ftm_cdma_send_command
03/28/11   aro     [1] Changed ftm_cdma_create_ftm_q_msg() signature to return
                   flag indicating success and failure
                   [2] Added functionality to send FTM message from this func
03/28/11   aro     Added prototype for ftm_cdma_create_ftm_q_msg()
02/02/11   aro     Added FTM CDMA Init
02/01/11   aro     Moved malloc function to FTM Common
01/15/11   aro     Initial Release

==============================================================================*/

#include "comdef.h"

#include "ftmdiag.h"
#include "ftm.h"
#include "rfm_device_types.h"
#include "lm_types.h" /* For LM handle type */

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Lists the FTM 1xHDR Command Reponse status.
*/
typedef enum
{

  FTM_CDMA_BAD_RSP  = 0,
  /*!< Bad Response Packet to diag */

  FTM_CDMA_GOOD_RSP = 1,
  /*!< Good Response Packet to diag */ 

  FTM_CDMA_NO_RSP   = 2,
  /*!< No response packet to diag Response Packet */

  FTM_CDMA_NEW_RSP_PKT   = 3
  /*!< Send a new response packet to Diag. */  

} ftm_cdma_rsp_type;

/*----------------------------------------------------------------------------*/
/*! Holds information for current ftm cmd */
extern ftm_pkt_data_type ftm_pkt_data;

/*----------------------------------------------------------------------------*/
boolean
ftm_cdma_init
(
  void
);

/*----------------------------------------------------------------------------*/
boolean
ftm_cdma_send_command
(
  rfm_device_enum_type device,
  ftm_rf_factory_parms *ftm_cmd_param
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_cdma_create_rsp_pkt
(
  ftm_cdma_rsp_type status
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_cdma_malloc_rsp_pkt
(
  void * ftm_req_data,
  uint16 rsp_pkt_size
);

/*----------------------------------------------------------------------------*/
boolean
ftm_cdma_configure_debug_mode
(
  rfm_device_enum_type device,
  uint32 debug_mask,
  boolean debug_en
);

/*----------------------------------------------------------------------------*/
boolean 
ftm_cdma_get_pa_switchpoints
( 
  rfm_device_enum_type        tx_device , 
  rfm_cdma_band_class_type    band , 
  int16                       pa_rise_swpts[] , 
  int16                       pa_fall_swpts[] , 
  int16*                      active_pa_states  
) ;

/*----------------------------------------------------------------------------*/
boolean
ftm_cdma_control_enable_tech
(
  rfm_mode_enum_type tech,
  rfm_device_enum_type rf_dev,
  sys_channel_type band_chan,
  lm_handle_type rxlm_handle,
  const rfm_cb_handler_type callback,
  void* const userdata
);

#ifdef __cplusplus
}
#endif

#endif /* FTM_CDMA_CONTROL_H */

