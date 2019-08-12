#ifndef GL1_MSG_WTR_HOP_H
#define GL1_MSG_WTR_HOP_H
/*========================================================================
                       GSM GPRS DEFINITIONS FILE
DESCRIPTION
   This file contains definitions used by the wtr hop feature

Copyright (c) 2001-2014 Qualcomm Technologies, Inc.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
========================================================================== */

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gdrivers/inc/gl1_msg_wtr_hop.h#1 $
$DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
04/11/14    nk      CR750248 Reset WTR Hop state machine at Change state
21/07/14    cs      CR697245 New GRM API support for WTR Hopping
17/03/14    pjr     CR633698 Dedicated WTR Hop Bring Up changes
16/01/14    pjr     CR600612 Initial revision
========================================================================== */
#include "geran_variation.h"

#ifdef FEATURE_GSM_WTR_HOP

#include "geran_dual_sim.h"
#include "rfm_device_types.h"
#include "geran_eng_mode_read_api.h"
#include "l1i.h"
#include "trm.h"

typedef enum
{
  WTR_HOP_INACTIVE,
  WTR_HOP_INIT,
  WTR_HOP_ALLOC_RXLM,
  WTR_HOP_ALLOC_TXLM,
  WTR_HOP_STOP_SCE,
  WTR_HOP_RF_ENTER_MODE,
  WTR_HOP_RF_SET_TX,
  WTR_HOP_CHANGE_DEVICE,
  WTR_HOP_CLEANUP
} wtr_status_type;

typedef enum
{
  WTR_HOP_START_SUCCESS,
  WTR_HOP_START_HOP_IN_PROGRESS,
  WTR_HOP_START_GAS_NOT_ACTIVE,
  WTR_HOP_START_INVALID_RF_DEVICE
} wtr_hop_start_result_t;

/*===========================================================================

FUNCTION gl1_msg_wtr_hop_state_machine

DESCRIPTION
  This state machine is called from L1_control_dedicated function after 
  the execution of the Frame Notification. The state machine performs all
  the actions to prepare L1 and RF for hopping the WTR. The hop operation
  is performed in the idle frame before the FTN and is executed by function
  gl1_msg_wtr_hop_idle_frame_pre_ftn. After the hop has been completed the
  state machine de-allocate unused resources and restart the SCE.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void gl1_msg_wtr_hop_state_machine(channel_information_T *chnl_1_data, gas_id_t gas_id);

/*===========================================================================

FUNCTION gl1_msg_wtr_hop_idle_frame_pre_ftn

DESCRIPTION
  This function is called every frame when in dedicated mode but return 
  inmediately if the conditions are not ready for hopping the RF device.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void gl1_msg_wtr_hop_idle_frame_pre_ftn(gas_id_t gas_id);

/*===========================================================================

FUNCTION gl1_msg_wtr_hop_get_status

DESCRIPTION
  This function returns the current state of the wtr hop state machine

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
wtr_status_type gl1_msg_wtr_hop_get_status(void);                                   

/*===========================================================================

FUNCTION gl1_msg_wtr_hop_get_gas_to_hop

DESCRIPTION
  This function returns the gas_id of the sub being swapped, if no sub is
  being swaped GERAN_ACCESS_STRATUM_ID_UNDEFINED is returned

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
gas_id_t gl1_msg_wtr_hop_get_gas_to_hop(void);

/*===========================================================================

FUNCTION gl1_msg_wtr_hop_abort

DESCRIPTION
  This function aborts a hop in progress

DEPENDENCIES
  None.

RETURN VALUE
  Return TRUE if abort was successful or no hop is in progress
  Return FALSE to indicate that wtr hop state machine still needs to be ticked 

SIDE EFFECTS
  None.

===========================================================================*/
boolean gl1_msg_wtr_hop_abort(boolean restart_sce, gas_id_t gas_id);

/*===========================================================================

FUNCTION gl1_msg_hop_start

DESCRIPTION
  Triggers a radio transceiver hop operation on a sub in dedicated mode

DEPENDENCIES
  None.

RETURN VALUE
  WTR_HOP_START_SUCCESS if operation successfully started
  or an error code otherwise

SIDE EFFECTS
  None.

===========================================================================*/
wtr_hop_start_result_t gl1_msg_hop_start(
                         rfm_device_enum_type rf_device, 
                         gas_id_t             gas_id );

/*===========================================================================

FUNCTION gl1_msg_wtr_hop_rude_reset

DESCRIPTION
  This function performs a rude reset of WTR Hop state machine

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void gl1_msg_wtr_hop_rude_reset(gas_id_t gas_id);

#endif /* FEATURE_GSM_WTR_HOP */
#endif
