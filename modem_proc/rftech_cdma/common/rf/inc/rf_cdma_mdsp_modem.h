#ifndef RF_CDMA_MDSP_MODEM_H
#define RF_CDMA_MDSP_MODEM_H

/*! 
  @file
  rf_cdma_mdsp_modem.h
 
  @brief
  This module defines the function prototypes and defintions used to interact 
  with the RFLM interface, the APIs here are modem specific.
*/

/*==============================================================================

  Copyright (c) 2013 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rf_cdma_mdsp_modem.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
08/22/14   spa     Add handle as argument for 'rx_allocate resources' API
07/29/14   spa     Add API rf_cdma_mdsp_modem_get_event_size to query for event
                   size
03/31/14   zhw     Tuner AOL Abort support
03/12/14   APU     Split modem enter and exit into seperate rx and tx modem 
                   enter and exit.
02/10/14   spa     Add API to create issue seq style events 
12/16/13   zhw     Tuner AOL support
11/27/13   APU     IRAT Changes FOR bolt.
10/23/13   spa     Changed rf_cdma_dm_buff typedef to uint32 to match interface
09/25/13   JJ      Added RFLM_CDMA_MDSP_CCS_EVENT_RX_ON/OFF_ID evetns 
09/19/13   APU     MC changes to support DM.
09/16/13   JJ      Added rflm cdma /init/enter/exit
08/01/13   spa     Initial rev
==============================================================================*/

#include "comdef.h"
#include "msgr_umid.h"
#include "lm_types.h"
#include "rfcommon_locks.h"
#include "cdmafw_msg.h"
#include "rfm_mode_types.h"
#include "rfm_device_types.h"
#include "rfcommon_msg.h"
#include "rf_cdma_mdsp_ccs_events.h"
#include "rf_cdma_mdsp_types.h"
#include "rflm_api_c2k.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*----------------------------------------------------------------------------*/
/*! Typedef to refer to pointers in DM locations */
typedef uint32 rf_cdma_dm_buff;
/*----------------------------------------------------------------------------*/

boolean
rf_cdma_mdsp_modem_allocate_associate_script
(
  lm_handle_type                  lm_handle   ,
  rflm_cdma_mdsp_script_id_t      rflm_ev_id 
) ;
/*----------------------------------------------------------------------------*/
void
rf_cdma_mdsp_modem_rx_allocate_resources
(
   rfm_device_enum_type  device,
   const lm_handle_type  rxlm_handle,
   rfm_mode_enum_type    rf_mode,
   boolean               is_irat
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_mdsp_modem_rx_deallocate_resources
(
  rfm_device_enum_type  device  ,
  rfm_mode_enum_type    rf_mode  ,
  const lm_handle_type  lm_handle
) ;

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mdsp_modem_tx_allocate_resources
(
  lm_handle_type        txlm_handle
) ;

/*----------------------------------------------------------------------------*/
void
rf_cdma_mdsp_rflm_init 
(
  void
);
/*----------------------------------------------------------------------------*/

rflm_cdma_mdsp_script_id_t
rf_cdma_mdsp_modem_get_rflm_event_ids 
( 
   rf_cdma_mdsp_event_id_type e_id 
) ;


/*----------------------------------------------------------------------------*/
void*
rf_cdma_mdsp_modem_get_ccs_buffer 
(
   rfm_device_enum_type device ,
   lm_handle_type lm_handle ,
   uint32 event_id  ,
   uint32 buffer_index ,
   rf_cdma_mdsp_chain_enum_type mdsp_chain ,
   rfm_mode_enum_type rf_mode ,
   rfcommon_mdsp_transaction_settings_type* rf_cdma_mdsp_ccs_event_transaction_params, 
   boolean *is_event_issue_seq
) ;

/*----------------------------------------------------------------------------*/
uint32
rf_cdma_mdsp_modem_get_script_index 
(
   uint32 script_id ,
   uint32 event_id
) ;

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mdsp_modem_prepare_issue_seq_for_event
(
  rfm_mode_enum_type rf_mode,
  lm_handle_type lm_handle ,
  uint32 event_id,
  rfmeas_event_infor_type *temp_script_holder,
  void* ptr_to_event_mem
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_mdsp_modem_tx_deallocate_resources
(
  rfm_mode_enum_type rf_mode,
  lm_handle_type txlm_handle
);

/*----------------------------------------------------------------------------*/
uint32
rf_cdma_mdsp_modem_get_event_size
(
   rflm_cdma_mdsp_script_id_t      rflm_ev_id 
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RF_CDMA_MDSP_MODEM_H */

