/*!
  @file
  ds_3gpp_srv_req_throt_config.h

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/inc/ds_3gpp_srv_req_throt_config.h#1 $

when         who     what, where, why
--------     ---     --------------------------------------------------------
04/16/15     vb      Handle Service request throttling configuration
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
FUNCTION       DS_3GPP_SRV_REQ_THROT_FILL_CONFIG_WITH_TOKEN_CONTENTS

DESCRIPTION   This function gets the data for each token and populates the 
              structure with the appropriate data that corresponds to the 
              token number.   

PARAMETERS    token_id -  describes the current token which is 
                          being populated into the EFS structure

              srv_req_timer_index -- Indicates if the timer value read is
                                     srv_req_throttle_timer or min/max random
                                     timer values
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_3gpp_srv_req_throt_fill_config_with_token_contents
(
  uint8                        token_id,
  sys_modem_as_id_e_type       subs_id,
  uint8                        srv_req_timer_index,
  uint32                       failure_value
);

/*===========================================================================
FUNCTION      DS_3GPP_SRV_REQ_THROT_RESET_CONFIG_FROM_EFS_PER_SUBS_ID

DESCRIPTION   This function will reset the srv req throttle efs structure 
              per subscription 
 
PARAMATERS   subs_id subscription id  

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_3gpp_srv_req_throt_reset_config_from_efs_per_subs_id
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS_3GPP_SRV_REQ_THROT_GET_FAILURE_TIMER

DESCRIPTION   This function provides the srv req throttling failure timer value

PARAMETERS    counter - Throttling counter used as an index 
              subs_id - Subs_id

RETURN VALUE  Throttle Failure Timer value in msec

SIDE EFFECTS  None
===========================================================================*/
uint32 ds_3gpp_srv_req_throt_get_failure_timer 
(
   uint8                  counter,
   sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION      DS_3GPP_SRV_REQ_THROT_GET_RANDOM_TIMERS 

DESCRIPTION   This function provides the srv req throttle random timers

PARAMETERS 
              counter - Throttling counter used as an index
              uint32    *min_random_timer - min Random timer value will
                                            be populated   
              uint32    *max_random_timer - max Random timer value will
                                            be populated

RETURN VALUE  void

SIDE EFFECTS  None
===========================================================================*/
void ds_3gpp_srv_req_throt_get_random_timers
(
   uint8                  counter,
   sys_modem_as_id_e_type subs_id,
   uint32                *min_random_timer_p,    
   uint32                *max_random_timer_p
);


/*===========================================================================
FUNCTION      DS_3GPP_SRV_REQ_THROT_SET_FAILURE_TIMER 

DESCRIPTION   This function is to assign srv req throttling failure timer value
              manually

PARAMETERS    counter - Throttling counter used as an index

RETURN VALUE  none

SIDE EFFECTS  None
===========================================================================*/
void ds_3gpp_srv_req_throt_set_failure_timer 
(
   uint8 counter,
   uint32 timerval,
   sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION      DS_3GPP_SRV_REQ_THROT_FILL_PEND_FAILURE_TIMER 

DESCRIPTION   This function is to assign pdn throttling failure timer value
              manually for 6th -10th failure timers when global throttling
              is not enabled

PARAMETERS    subs_id

RETURN VALUE  none

SIDE EFFECTS  None
===========================================================================*/
void ds_3gpp_srv_req_throt_fill_pend_failure_timer 
(
   sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION      DS_3GPP_SRV_REQ_READ_THROTTLE_CONFIG_FROM_EFS_PER_SUBS_ID

DESCRIPTION   This function will read from the efs file all the necessary 
              data & fill the srv req throttle config structure per subscription.
              Only valid data is populated.

              EFS File Format - Param_Name:Failure_Value:Min_Random_Timer:MAx_Random_Timer;
              For example     - FAILURE_TIMER_1:0:2:3;
 
PARAMATERS   subs_id subscription id  

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_3gpp_srv_req_read_throttle_config_from_efs_per_subs_id
(
  sys_modem_as_id_e_type  subs_id
);

