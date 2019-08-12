/*!
  @file
  ds_3gpp_cc_sp_throt_config.h

  @brief
  REQUIRED brief one-sentence description of this C module.

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2015 QUALCOMM Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/inc/ds_3gpp_cc_sp_throt_config.h#1 $

when         who     what, where, why
--------     ---     --------------------------------------------------------
04/16/15     vb      Handle cc sp request throttling configuration
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/


#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include "ds_3gpp_pdn_context.h"
#include "dsutil.h"

/*===========================================================================
FUNCTION      DS_3GPP_CC_SP_THROT_RESET_PER_SUBS_ID

DESCRIPTION   This function will reset the cc_sp_throttle structure per 
              subscription 

PARAMATERS   subs_id subscription id 
 
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/

void ds_3gpp_cc_sp_throt_reset_per_subs_id
(
  sys_modem_as_id_e_type        subs_id
);

/*===========================================================================
FUNCTION      DS_3GPP_CC_SP_THROT_READ_CONFIG_FROM_EFS_PER_SUBS_ID

DESCRIPTION   This function will read from the efs file all the necessary 
              data & fill the PDN throttle config structure. Only valid data 
              is populated.

              EFS File Format - Param_Name:Param_Val;
              For example     - FAILURE_TIMER_1:0;   
 
PARAMATERS   subs_id subscription id 
 
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_3gpp_cc_sp_throt_read_config_from_efs_per_subs_id
(
  sys_modem_as_id_e_type        subs_id
);

/*===========================================================================
FUNCTION      DS_3GPP_CC_SP_THROT_GET_TIMER 

DESCRIPTION   This function provides the PDN throttling failure timer value

PARAMETERS    counter - Throttling counter used as an index

RETURN VALUE  PDN throttle Failure Timer value in msec

SIDE EFFECTS  None
===========================================================================*/
uint32 ds_3gpp_cc_sp_throt_get_timer 
(
  uint16 cause_code, 
  uint8 counter,
  sys_modem_as_id_e_type  subs_id
);


/*===========================================================================
FUNCTION      DS_3GPP_CC_SP_THROT_GET_TOKEN_ID

DESCRIPTION   This function returns the token id associated with each pdn
              throttle config parameter. 

PARAMETERS    char *from - start of the token (param name)
              char *to   - end of the token (param name)
              *token_id  - Pointer in which Token ID is returned

DEPENDENCIES  None

RETURN VALUE  0  - success
              -1 - failure

SIDE EFFECTS  None
===========================================================================*/
int16 ds_3gpp_cc_sp_throt_get_token_id
(  
  char *from,
  char *to,
  uint8 *token_id,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS_3GPP_CC_SP_THROT_IS_CAUSE_CODE_CONFIGURED

DESCRIPTION   This function return true if given cause code is configured in 
              cc_sp_throttle_timer.txt file

PARAMETERS    cause code - PDP activation reject cause

RETURN VALUE  true- if cause code configured, false otherwise

SIDE EFFECTS  None
SIDE EFFECTS  None
===========================================================================*/
boolean ds_3gpp_cc_sp_throt_is_cause_code_configured
(
  uint16 cause_code,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS_3GPP_CC_SP_THROT_SET_DEFAULT_CONFIG

DESCRIPTION   This function used to set the PDN throttling failure timer value

PARAMETERS    counter - Throttling counter used as an index

RETURN VALUE  PDN throttle Failure Timer value in msec

SIDE EFFECTS  None
===========================================================================*/
void ds_3gpp_cc_sp_throt_set_default_config
(
   sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS_3GPP_CC_SP_THROT_GET_PLMN_UNBLOCK_TIMER_VAL

DESCRIPTION   This function provides the plmn unblock timer value
              configured in EFS

PARAMETERS    

RETURN VALUE  plmn unblock timer value in msecs

SIDE EFFECTS  None
===========================================================================*/
uint32 ds_3gpp_cc_sp_throt_get_plmn_unblock_timer_val
(
   void
);

/*===========================================================================
FUNCTION      DS_3GPP_CC_SP_SET_PLMN_UNBLOCK_TIMER_VAL

DESCRIPTION   This function used to plmn unblock timer configured in EFS.

DEPENDENCIES  None.

RETURN VALUE  Integer value
             
SIDE EFFECTS  None.
===========================================================================*/
void ds_3gpp_cc_sp_set_plmn_unblock_timer_val
(
   uint32 timer_val
);
