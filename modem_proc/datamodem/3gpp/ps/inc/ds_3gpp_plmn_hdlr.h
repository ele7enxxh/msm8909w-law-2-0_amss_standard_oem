#ifndef DS_3GPP_PLMN_HDLR_H
#define DS_3GPP_PLMN_HDLR_H

/*!
  @file
  ds_3gpp_plmn_hdlr.h

  @brief
  PLMN related functions and routines

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2009 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/inc/ds_3gpp_plmn_hdlr.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/05/14   vm      Initial version

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"

#include "sys_v.h"
#include "reg_sim.h"
#include "sys_eplmn_list.h"

/*===========================================================================

                         EXTERNAL TYPE DECLRATIONS

===========================================================================*/

/*===========================================================================

                      EXTERNAL FUNCTION DECLRATIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS_3GPP_GET_EPLMN_LIST_PER_SUB

DESCRIPTION   This function returns the EPLMN List per subs id.
              Note: memory has to be allocated by user.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_3gpp_get_eplmn_list_per_sub
(
  sys_modem_as_id_e_type  subs_id,
  sys_plmn_list_s_type   *eplmn_list_p,
  sys_plmn_id_s_type     *rplmn_p
);

/*===========================================================================
FUNCTION      DS_3GPP_GET_EHPLMN_LIST_PER_SUB

DESCRIPTION   This function returns the EHPLMN List per subs id.
              Note: memory has to be allocated by user.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_3gpp_get_ehplmn_list_per_sub
(
  sys_modem_as_id_e_type     subs_id,
  reg_sim_plmn_list_s_type  *ehplmn_list_p
);

/*===========================================================================
FUNCTION      DS_3GPP_IS_PLMN_IN_EHPLMN_LIST

DESCRIPTION   This function checks whether the given PLMN is in EHPLMN List

DEPENDENCIES  None

RETURN VALUE  TRUE, if PLMN is in EHPLMN List
              FALSE, otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean ds_3gpp_is_plmn_in_ehplmn_list
(
  sys_plmn_id_s_type      plmn_id,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS_3GPP_GET_HPLMN_ID_PER_SUB

DESCRIPTION   This function returns the HPLMN ID per subs id.
              Note: memory has to be allocated by user.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_3gpp_get_hplmn_id_per_sub
(
  sys_modem_as_id_e_type   subs_id,
  sys_plmn_id_s_type      *hplmn_id_p
);

#endif /* DS_3GPP_PLMN_HDLR_H */
