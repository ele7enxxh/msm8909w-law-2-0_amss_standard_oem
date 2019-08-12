#ifndef RFMEAS_MDSP_H
#define RFMEAS_MDSP_H

/*! 
  @file
  rfmeas_mdsp.h
 
  @brief
  Thie module defines the MDSP interfaces that are common across all techologies. 
*/

/*==============================================================================

  Copyright (c) 2010-2016 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/mdsp/qdsp6_meas/inc/rfmeas_mdsp.h#1 $ 

when       who     what, where, why
--------   ---     -------------------------------------------------------------
08/18/16   cv      Fix for IRAT_EVENT ID construction overflow for 32 bit channels
08/18/16   cv      Update IQMC APIs to use 32-bit ARFCNs
03/21/14   ra      Support FW cleanup of deprecated files
02/06/13   gvn     Changes to enable Script Abort
01/21/14   cj      Definition for RFMEAS_MDSP_GET_EVT_SEQ_ID added
01/02/14   gvn     Interface change to support enqueue policy check
12/04/13   whz     Added status_update_script and status_check_script APIs
11/19/13   cj      Added rfmeas_mdsp_prepare_ccs_irat_script API
10/15/13   pl      Added preload events
10/15/13   pl      add new interface to support DM handle
09/09/13   php     Split SSBI scripts based on bus into 2 Task Queues
08/16/13   ndb     Clean-up the SPI based antenna tuner support
05/09/13   spa     Removed unused header files 
04/09/13   pl      remove unused functions
03/26/13   jyu     Added the change to allow override trigger type
11/27/12   aki     Added rfmeas_mdsp_deinit
10/16/12   pl      remove obsolete function
10/14/12   pl      Add IRAT Event ids
10/11/12   pl      Bug fixes from off-target test
10/04/12   sr      Added IRAT Init and rf_buffer get APIs. 
10/02/12   pl      Remove obsolete API
10/02/12   pl      Adding CCS support for RF-FW IRAT script
03/26/12   av      IRAT code cleanup for W and G. Remove rfdevice_meas. 
10/13/11   gh      Add separate ant tuner SPI write script API
08/25/11   sar      Removed obsolete file bbrx.h.
08/10/11   tnt     Add Ant Tuner support 
07/29/11   av      Create new API to write only SBI script to FW. (will convert to one api 
                   with other techs later) 
07/24/11   av      XtoG, GtoX changes for new GRFC and RXLM interface  
07/19/11   whc     Remove unused IRAT GRFC mask
06/13/11   tnt     Add new function to execute the GRFC mask 
04/14/11   dw      Remove BBRx scripts in rfmeas_mdsp_meas_write_irat_script() 
08/24/10   pl      Add support for IRAT script execution using FW
07/28/10   pl      Add support for IRAT script execution 
07/22/10   pl      Initial Revision.

==============================================================================*/
/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "rfa_variation.h"
#include "comdef.h"
#include "rfdevice_meas_type_defs.h"
#include "rfmeas_types.h"
#include "rf_buffer_intf.h"
#include "rflm_ccs_rf_event_intf.h"


/*============================================================================== 
 
                               MACROS AND TYPEDEFS 
 
==============================================================================*/
/*! @brief define event IDs for the IRAT events,
           2 event grouping are defined: START (tune-away) and STOP (tune-back)
           Each event grouping contains 3 events:
                EVENT_A (transceiver related),
                EVENT_B (non-transceiver related),
                PRELOAD (transceiver preload) */
#define RFMEAS_MDSP_CCS_EVENT_START_A_ID (RFCOMMON_MDSP_EVENT_ID_BASE_IRAT | 0x00000001)
#define RFMEAS_MDSP_CCS_EVENT_START_B_ID (RFCOMMON_MDSP_EVENT_ID_BASE_IRAT | 0x00000002)
#define RFMEAS_MDSP_CCS_EVENT_STOP_A_ID  (RFCOMMON_MDSP_EVENT_ID_BASE_IRAT | 0x00000003)
#define RFMEAS_MDSP_CCS_EVENT_STOP_B_ID  (RFCOMMON_MDSP_EVENT_ID_BASE_IRAT | 0x00000004)
#define RFMEAS_MDSP_CCS_EVENT_START_PRELOAD_ID  (RFCOMMON_MDSP_EVENT_ID_BASE_IRAT | 0x00000005)
#define RFMEAS_MDSP_CCS_EVENT_STOP_PRELOAD_ID   (RFCOMMON_MDSP_EVENT_ID_BASE_IRAT | 0x00000006)

#define RFMEAS_MDSP_GET_EVT_SEQ_ID(owner_tech, rf_band, channel, event_a_id) (((((event_a_id & 0xF)<<28)|((channel & 0xFFFF) <<12))|rf_band<<4)|(owner_tech & 0xF))

/* STRUCTURE DEFINITION */

typedef struct 
{
  uint32                       rf_band;
  uint32                       channel;
  uint16                       buff_mask;
}rfmeas_mdsp_policy_check_params_s;
/*============================================================================== 
 
                             FUNCTION DECLARATIONS
 
==============================================================================*/

/*----------------------------------------------------------------------------*/

void rfmeas_mdsp_init(void);

/*----------------------------------------------------------------------------*/

void rfmeas_mdsp_deinit(void);

/*----------------------------------------------------------------------------*/

rf_buffer_intf *rfmeas_mdsp_buffer_get(rfm_meas_program_script_type script_type);

/*----------------------------------------------------------------------------*/
/* REMOVE WHEN ALL TECH MOVE TO NEW V2 interface */
boolean rfmeas_mdsp_meas_write_ccs_irat_script
(
  uint32                       buffer_index,
  rfm_meas_program_script_type script_type,
  rf_buffer_intf               *script_buffer_ptr,
  boolean                      trigger_type_flag
);

/*----------------------------------------------------------------------------*/
boolean rfmeas_mdsp_meas_abort_script
(
  uint32                       handle_id,
  uint32                       script_id
);
/*----------------------------------------------------------------------------*/
boolean rfmeas_mdsp_meas_write_ccs_irat_script_optim
(
  uint32                       buffer_index,
  rfm_meas_program_script_type script_type,
  rf_buffer_intf               *script_buffer_ptr,
  boolean                      trigger_type_flag
);
/*----------------------------------------------------------------------------*/
boolean rfmeas_mdsp_meas_write_ccs_irat_script_v2
(
  uint32                       handle_id,
  rfm_meas_program_script_type script_type,
  rf_buffer_intf               *preload_buffer_ptr,
  uint32                       preload_script_id,
  rf_buffer_intf               *trigger_buffer_ptr,
  uint32                       trigger_script_id,
  rf_buffer_intf               *non_trx_buffer_ptr,
  uint32                       non_trx_script_id,
  boolean                      combine_trigger_non_trx,
  rfmeas_mdsp_policy_check_params_s *policy_check_ptr
);

/*----------------------------------------------------------------------------*/

boolean rfmeas_mdsp_prepare_ccs_irat_script
(
  uint32                       handle_id,
  rfm_meas_program_script_type script_type,
  rf_buffer_intf               *buffer_ptr,
  uint32                       script_id );

/*----------------------------------------------------------------------------*/
boolean rfmeas_mdsp_meas_status_update_script
(
  uint32                       buffer_index,
  rfm_meas_program_script_type script_type
);

/*----------------------------------------------------------------------------*/
boolean rfmeas_mdsp_meas_status_check_script(
  uint32                       buffer_index,
  rfm_meas_program_script_type script_type,
  rflm_ccs_rf_event_status_type     *task_a_status,
  rflm_ccs_rf_event_status_type     *task_b_status
);

#endif /* RFMEAS_MDSP_H */

