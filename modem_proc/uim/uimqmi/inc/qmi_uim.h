#ifndef QMI_UIM_H
#define QMI_UIM_H
/*===========================================================================

                         Q M I _ U I M . H

DESCRIPTION

 The Data Services QMI User Identity Module Service header file.

                        COPYRIGHT INFORMATION

Copyright (c) 2010, 2012 - 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimqmi/inc/qmi_uim.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/25/15    sp     Updated copyright
09/07/15    bcho   Migrate to mcfg EFS wrappers
08/27/15    bfv    Do not retry service registration if it fails once
05/01/15    vdc    Added support for QMI UIM GBA
03/25/15    stv    API to block card status events
09/30/14    kk     Enhancements to NV handling
08/29/14    tl     Add support for supply voltage command and indication
05/22/14    tl     Added support for recovery complete indications
12/24/13    am     Add NV protection to Recovery API
02/19/13    av     Merge mmgsi,gstk,qmiuim/cat conf files to mmgsdi.conf
02/14/13    tl     Move QMI UIM to MMGSDI context from QMI modem context
02/23/12    nmb    QMI QMUX Changes
08/05/10    tkl    QMI UIM & QMI CAT split from data package
01/14/10    mib    Initial check-in of QMI_UIM
===========================================================================*/

/*===========================================================================

                            CONSTANT DEFINITIONS

===========================================================================*/
#define QMI_UIM_CONFIG_GBA_FQDN_WHITE_LIST  "/nv/item_files/modem/qmi/uim/gba_fqdn_white_list"
#define QMI_UIM_CONFIG_GBA_FQDN_BLACK_LIST  "/nv/item_files/modem/qmi/uim/gba_fqdn_black_list"

/*===========================================================================
  FUNCTION QMI_UIM_INIT_PRE_STARTUP()

  DESCRIPTION
    Initializes the QMI UIM global variables and register with MMGSDI

  PARAMETERS
    void

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_init_pre_startup
(
  void
);

/*===========================================================================
  FUNCTION QMI_UIM_INIT_POST_STARTUP()

  DESCRIPTION
    Registers the QMI UIM service to the QMI framework to initalize
    the service.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_init_post_startup
(
  void
);

/*===========================================================================
QMI_UIM_SIG_HANDLER

DESCRIPTION
  QMI UIM signal handler for MMGSDI task signals

PARAMETERS
  None.

  Returns TRUE if signal should be cleared from the set signal mask,
  FALSE if further signal processing is needed and hence signal
  should not be cleared.
===========================================================================*/
extern boolean qmi_uim_sig_handler
(
  void
);

/*===========================================================================
  FUNCTION QMI_UIM_RECOVERY_COMPLETE_IND()

  DESCRIPTION
    This function is invoked by the UIM drivers when a recovery indication
    has completed successfully to inform clients of QMI UIM.

  PARAMETERS
    slot: Indicates the slot in which the recovery occured

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_recovery_complete_ind
(
  const uim_slot_type slot
);

/*===========================================================================
  FUNCTION QMI_UIM_SUPPLY_VOLTAGE_IND()

  DESCRIPTION
    This function is invoked by the UIM drivers when the Vcc needs to
    be deactivated or when Vcc has been activated.

  PARAMETERS
    slot                                      : UIM slot id
    uimdrv_qmi_indications_type               : LDO state
    uimdrv_qmi_power_management_callback_type : callback to ack drivers

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_supply_voltage_ind
(
  uim_slot_type                                     slot,
  uimdrv_qmi_indications_type                       ldo_state,
  uimdrv_qmi_power_management_callback_type         uim_callback_ptr
);

/*===========================================================================
  FUNCTION QMI_UIM_BLOCK_CARD_STATUS_INDICATIONS

  DESCRIPTION
    API to block card status indications from being sent.

  PARAMETERS
    None.

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_block_card_status_indications
(
  void
);

#endif /* QMI_UIM_H */
