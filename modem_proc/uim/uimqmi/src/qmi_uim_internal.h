#ifndef QMI_UIM_INTERNAL_H
#define QMI_UIM_INTERNAL_H
/*===========================================================================

                         Q M I _ U I M _ I N T E R N A L. H

DESCRIPTION

 The QMI User Identity Module internal header file.

                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimqmi/src/qmi_uim_internal.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/18/16    vdc    Added support for simlock single lock
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmgsdilib_common.h"
#include "qmi_uim_parsing.h"

/*===========================================================================

                                DATA TYPES

===========================================================================*/

/*---------------------------------------------------------------------------
  Values used in simlock dual mode EFS item
---------------------------------------------------------------------------*/
typedef enum
{
  QMI_UIM_SIMLOCK_STATUS_DEFAULT                                 = 0,
  QMI_UIM_SIMLOCK_DISPLAY_FOR_VALID_CARD_ON_ANY_SLOT_POLICY      = 1
} qmi_uim_simlock_status_waiting_for_valid_card_type;


/*===========================================================================
  FUNCTION QMI_UIM_UTIL_IS_SIMLOCK_STATUS_FOR_VALID_CARD_ENABLED()

  DESCRIPTION
    This function checks the simlock NV and slot policy to determine if
    the criteria is satisifed for simlock status update for valid card.

  PARAMETERS
    None

  RETURN VALUE
    Boolean indicating if criteria is staisifed or not.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_util_is_simlock_status_for_valid_card_enabled
(
  void
);

#endif /* QMI_UIM_INTERNAL_H */
