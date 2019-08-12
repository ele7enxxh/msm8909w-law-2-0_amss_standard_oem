#ifndef QMI_CAT_H
#define QMI_CAT_H
/*===========================================================================

                         Q M I _ C A T . H

DESCRIPTION

 The Data Services QMI Card Application Toolkit Service header file.

                      COPYRIGHT INFORMATION

Copyright (c) 2009 - 2010, 2012 - 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimqmi/inc/qmi_cat.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/25/15    sp     Updated copyright
09/07/15    bcho   Migrate to mcfg EFS wrappers
08/27/15    vdc    Do not retry service registration if it fails once
02/13/15    hh     Split qmi_cat_init into pre and post startup
10/30/14    gm     Move enable/disable of raw unsupported features to NV
09/24/14    gm     Remove GOBI mode code
09/09/14    bd     Moved an enum to header file
08/05/14    hh     Added support to retrieve supported proactive commands
01/09/14    kb     Add de-init function when gstk task is stopped
04/11/13    tkl    Add NV suport for SETUP Call display alpha value
04/02/13    av     Move UIM tasks to group4 of RCINIT
02/19/13    av     Merge mmgsdi,gstk,qmiuim/cat conf files to mmgsdi.conf
02/14/13    tl     Move QMI CAT to GSTK context from QMI modem context
02/23/12    nmb    QMI QMUX Transition
08/05/10    tkl    QMI UIM & QMI CAT split from data package
03/25/10    dd     Adding decoded tlvs
11/01/09    mj     Initial check-in of QMI_CAT
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "gstk_exp.h"

/*=============================================================================

                   ENUMERATED DATA

=============================================================================*/

/*---------------------------------------------------------------------------
  QMI CAT configuration type
---------------------------------------------------------------------------*/
typedef enum
{
  CATI_CONFIG_MODE_DISABLED                    = 0x00,
  /* GOBI mode no longer supported */
  CATI_CONFIG_MODE_ANDROID                     = 0x02,
  CATI_CONFIG_MODE_DECODED                     = 0x03,
  CATI_CONFIG_MODE_DECODED_PULLONLY            = 0x04,
  CATI_CONFIG_MODE_CUSTOM_RAW                  = 0x05,
  CATI_CONFIG_MODE_CUSTOM_DECODED              = 0x06
} qmi_cati_config_mode_e_type;

/*=============================================================================

                   FUNCTIONS

=============================================================================*/

/*===========================================================================
  FUNCTION QMI_CAT_INIT_PRE_STARTUP()

  DESCRIPTION
    Initializes the QMI CAT global data and message queues

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_cat_init_pre_startup
(
  void
);


/*===========================================================================
  FUNCTION QMI_CAT_INIT_POST_STARTUP()

  DESCRIPTION
    Registers the QMI CAT service to the QMI framework to initalize the service
    and registers with MMGSDI, GSTK

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_cat_init_post_startup
(
  void
);

/*===========================================================================
QMI_CAT_SIG_HANDLER

DESCRIPTION
  QMI CAT signal handler for GSTK signals

PARAMETERS
  None.

  Returns TRUE if signal should be cleared from the set signal mask,
  FALSE if further signal processing is needed and hence signal
  should not be cleared.
===========================================================================*/
extern boolean qmi_cat_sig_handler
(
  void
);

/*===========================================================================
  FUNCTION QMI_CAT_GET_TERMINAL_PROFILE_VALUE_LIST()

  DESCRIPTION
    Get terminal profile value list according to the QMI_CAT config mode.

  PARAMETERS
    none

  RETURN VALUE
    gstk_profile_dl_support_ext_type* : array of terminal profile

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern gstk_profile_dl_support_ext_type* qmi_cat_get_terminal_profile_value_list
(
  uint8                            * tp_count_ptr
);

/*===========================================================================
  FUNCTION QMI_CAT_RECOVERY_COMPLETE()

  DESCRIPTION
    This function is called by the QMI UIM when a recovery indication
    has completed successfully

  PARAMETERS
    slot: Indicates the slot in which the recovery occured

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_cat_recovery_complete
(
  mmgsdi_slot_id_enum_type  slot
);

/*===========================================================================
FUNCTION: qmi_cat_deinit

DESCRIPTION:
  This function is used to free the qmicat private resources.
  This is called when GSTK is powering down.

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None
===========================================================================*/
extern void qmi_cat_deinit
(
  void
);

#endif /* QMI_CAT_H */
