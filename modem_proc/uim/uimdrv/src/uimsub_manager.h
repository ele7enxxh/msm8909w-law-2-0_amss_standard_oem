#ifndef UIMSUB_MANAGER_H
#define UIMSUB_MANAGER_H
/*===========================================================================

        E X T E R N A L   U I M  C A R D  M A N A G E R
                            D E F I N I T I O N S

DESCRIPTION
  This contains all the declarations for the UIM Subscription Manager.
  The Subscription Manager allows Dual Slots and two cards simultaneously
  while making the presence of only one known to outside clients.  This
  way subscription information from only one card is published at a time and
  only one card is used by the rest of the modem.

Copyright (c)2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/uimsub_manager.h#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/07/16   na      Remove memory free in subscription manager report callback
08/14/14   sam     Adding header file of messsage macros
08/13/31   js      UIMDRV Memory De-Couple changes
08/08/13   ks      Advanced Subscription Manager changes
05/21/12   nmb     UIM Dual SIM Subscription Manager Initial Revision
===========================================================================*/
/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uimi.h"
#include "uim_v.h"
#include "uimdrv_msg.h"
#ifdef FEATURE_GSTK
#include "gstk_exp.h"
#include "gstk_p.h"
#endif

/*===========================================================================
                            DATA TYPES
===========================================================================*/
#ifdef FEATURE_UIM_DS_SUBSCRIPTION_MANAGER

typedef struct {
   void (*rpt_function)( uim_rpt_type * );
   uint32 userdata;
} uim_sm_cmd_saved_data_type;

extern uim_active_slot_cfg_type  uim_active_slot_configuration;
extern uim_sm_data_type          uim_sm_data;

#endif /* FEATURE_UIM_DS_SUBSCRIPTION_MANAGER */

/*===========================================================================
                       FUNCTION PROTOTYPES
===========================================================================*/

mmgsdi_return_enum_type uim_sm_mmgsdi_get_recovery_data_sync(
  uim_recovery_data_type *recovery_data_ptr,
  uim_slot_type           uim_slot_id
);

void uim_sm_rpt_function (
  uim_rpt_type *report_ptr
);

void uim_sm_mmgsdi_notify_card_error(
  uim_status_type          error_condition,
  uim_instance_global_type *uim_ptr
);

void uim_sm_gstk_uim_set_recovery_sig(
  gstk_slot_id_enum_type slot_id
);

void uim_sm_link_established(
  uim_instance_global_type *uim_ptr
);

boolean uim_sm_gstk_uim_fetch_allow(
  gstk_slot_id_enum_type slot
);

void uim_sm_translate_cmd_slot(
  uim_cmd_type *cmd_ptr
);

void uim_sm_modify_link_est(
  uim_slot_link_established_data_type link_est_data,
  uim_instance_global_type            *uim_ptr
);

#ifdef FEATURE_GSTK
void uim_sm_send_proactive_cmd(
  uim_slot_type slot,
  uint32        data_length,
  const uint8  *data
);
#endif /* FEATURE_GSTK */


uim_rpt_status uim_sm_switch_slot (
  uim_slot_type logical_slot,
  uim_slot_type physical_slot
);

uim_rpt_status uim_sm_card_slot_status_evt_reg_sync(
  uim_evt_callback_type *evt_cb_ptr
);

uint8 uim_sm_get_num_physical_slots_sync (
  void
);

uim_rpt_status uim_sm_get_logical_slot_status_sync (
  uint8                      *num_phy_slots_ptr,
  uim_phy_slot_status_type   *curr_slot_status
);

void uim_sm_init(
  uim_instance_global_type *uim_ptr
);

uim_rpt_status uim_sm_notify_qmi_slot_status(
  uim_instance_global_type *uim_ptr
);

void uim_sm_command_callback (
  uim_rpt_type *report_ptr
);

boolean uim_sm_update_active_slot_data(
  uim_instance_global_type *uim_ptr
);

uim_rpt_status uim_sm_build_command(
  uim_cmd_name_type        cmd_name,
  uim_instance_global_type *uim_ptr,
  uim_cmd_req_type         req_type
);

#endif /* UIMSUB_MANAGER_H */
