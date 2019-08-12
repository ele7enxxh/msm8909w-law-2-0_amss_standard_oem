/*!
  @file
  ds_3gpp_pdn_throt_config.c

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/src/ds_3gpp_pdn_throt_config.c#1 $

when         who     what, where, why
--------     ---     --------------------------------------------------------
04/16/15     vb      Handle PDN throttling configuration
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include "dsutil.h"
#include "ds_3gpp_pdn_throt_config.h"
#include "ds_3gpp_nv_manager.h"

/*-----------------------------------------------------------------------------
  Structure used to store the service req throttle config values from EFS
-----------------------------------------------------------------------------*/
typedef struct
{
  uint32 failure_timer[DS_3GPP_PDN_THROTTLE_MAX_FAILURE_TIMERS];   /* Failure Timer  */
}ds_3gpp_pdn_throt_config_type;

/*---------------------------------------------------------------------------
  Structure to store throttle config values
---------------------------------------------------------------------------*/
ds_3gpp_pdn_throt_config_type  
             *ds_3gpp_pdn_throt_config_p[DS3GSUBSMGR_SUBS_ID_MAX] = {NULL};


/*===========================================================================
FUNCTION       DS_3GPP_PDN_THROT_FILL_CONFIG_WITH_TOKEN_CONTENTS

DESCRIPTION   This function gets the data for each token and populates the 
              structure with the appropriate data that corresponds to the 
              token number.   

PARAMETERS    token_id -  describes the current token which is 
                          being populated into the EFS structure
 
              Failure_value - Failure timer value being written to
 
              subs_id 

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_3gpp_pdn_throt_fill_config_with_token_contents
(
  uint8                        token_id,
  uint32                       failure_value,
  sys_modem_as_id_e_type       subs_id
)
{
  /*-----------------------------------------------------------------------*/
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }  

  if(token_id >= DS_3GPP_PDN_THROTTLE_MAX_FAILURE_TIMERS)
  {
    DS_3GPP_MSG1_ERROR("Token ID:%d is invalid",token_id);
    return;
  }
  
  /*-------------------------------------------------------------------------
    Init to default values
  -------------------------------------------------------------------------*/

  if (ds_3gpp_pdn_throt_config_p[subs_id] == NULL)
  {

    ds_3gpp_pdn_throt_config_p[subs_id] =
      (ds_3gpp_pdn_throt_config_type *)
        modem_mem_alloc(sizeof(ds_3gpp_pdn_throt_config_type),
                        MODEM_MEM_CLIENT_DATA);

    if(ds_3gpp_pdn_throt_config_p[subs_id] != NULL)
    {
      memset((void*)ds_3gpp_pdn_throt_config_p[subs_id],0,
             (sizeof(ds_3gpp_pdn_throt_config_type)));
    }
    else
    {
      DS_3GPP_MSG0_ERROR("Heap allocation failed");
      return;
    }
  }

   ds_3gpp_pdn_throt_config_p[subs_id]->failure_timer[token_id] = 
        (uint32)failure_value;

  return;
} /*  ds_3gpp_pdn_throt_fill_config_with_token_contents */

/*===========================================================================
FUNCTION      DS_3GPP_PDN_THROT_RESET_CONFIG_FROM_EFS_PER_SUBS_ID

DESCRIPTION   This function will reset the throttle efs structure 
              per subscription 
 
PARAMATERS   subs_id subscription id  

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_3gpp_pdn_throt_reset_config_from_efs_per_subs_id
(
  sys_modem_as_id_e_type  subs_id
)
{
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }  

  if (ds_3gpp_pdn_throt_config_p[subs_id]!= NULL)
  {
    modem_mem_free(ds_3gpp_pdn_throt_config_p[subs_id],MODEM_MEM_CLIENT_DATA);
    DS_3GPP_MSG1_HIGH("Clearing throttling cfg for subs_id",subs_id);
    ds_3gpp_pdn_throt_config_p[subs_id] = NULL;
  }

}/*ds_3gpp_pdn_throt_reset_config_from_efs_per_subs_id*/

/*===========================================================================
FUNCTION      DS_3GPP_PDN_THROT_GET_FAILURE_TIMER

DESCRIPTION   This function provides the pdn throttling failure timer value

PARAMETERS    counter - Throttling counter used as an index 
              subs_id - Subs_id

RETURN VALUE  Throttle Failure Timer value in msec

SIDE EFFECTS  None
===========================================================================*/
uint32 ds_3gpp_pdn_throt_get_failure_timer 
(
   uint8                  counter,
   sys_modem_as_id_e_type subs_id
)
{
  uint32 return_timer_value =0;
  /*-----------------------------------------------------------------------*/
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return return_timer_value;
  }

  if((counter > DS_3GPP_PDN_THROTTLE_MAX_FAILURE_TIMERS) ||
     (counter < DS_3GPP_PDN_THROTTLE_MIN_FAILURE_TIMERS))
  {
    DS_3GPP_MSG1_ERROR("Invalid throttle counter value: %d", 
                    counter);
    return return_timer_value;
  }

  /*-------------------------------------------------------------------------
    Decrement the counter by 1 so that it can be used as an index into timer
    array also check for its validity
  -------------------------------------------------------------------------*/
  counter -= 1;

  if(ds_3gpp_pdn_throt_config_p[subs_id]!= NULL)
  {

    DS_3GPP_MSG2_HIGH("returning timer value :%d for failure_count %d",
                       ds_3gpp_pdn_throt_config_p[subs_id]->failure_timer[counter],
                       counter);

    return_timer_value = SEC_TO_MSEC
              (ds_3gpp_pdn_throt_config_p[subs_id]->failure_timer[counter]);
  }
  
  return return_timer_value;
} /* ds_3gpp_pdn_throt_get_failure_timer */

/*===========================================================================
FUNCTION      DS_3GPP_PDN_THROT_FILL_PEND_FAILURE_TIMER 

DESCRIPTION   This function is to assign pdn throttling failure timer value
              manually for 6th -10th failure timers when global throttling
              is not enabled

PARAMETERS    subs_id

RETURN VALUE  none

SIDE EFFECTS  None
===========================================================================*/
void ds_3gpp_pdn_throt_fill_pend_failure_timer 
(
   sys_modem_as_id_e_type subs_id
)
{
  uint8 idx = 0;
 /*-----------------------------------------------------------------------*/
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

    /*-------------------------------------------------------------------------
   If global throttling not enabled populate 7-10 indexes with 6th failure 
   timer value. This is required, because the failure_cnt will be incremented
   till DS_PDN_THROTTLE_MAX_FAILURE_TIMERS even if global throttling is not 
   enabled.
  -------------------------------------------------------------------------*/
  if( (!ds_3gpp_nv_manager_get_global_throttling(subs_id)) )
  {
    for(idx = DS_3GPP_PDN_THROTTLE_REF_FAILURE_TIMERS; 
        idx < DS_3GPP_PDN_THROTTLE_MAX_FAILURE_TIMERS; idx++)
    {
      ds_3gpp_pdn_throt_config_p[subs_id]->failure_timer[idx] = 
                     ds_3gpp_pdn_throt_config_p[subs_id]->failure_timer[idx-1];
    }
  }
 
  return;


}/*ds_3gpp_pdn_throt_fill_pend_failure_timer*/

/*===========================================================================
FUNCTION      DS_3GPP_PDN_THROT_READ_CONFIG_FROM_EFS_PER_SUBS_ID

DESCRIPTION   This function will read from the efs file all the necessary 
              data & fill the PDN throttle config structure per subscription.
              Only valid data is populated.

              EFS File Format - Param_Name:Param_Val;
              For example     - FAILURE_TIMER_1:0;
 
PARAMATERS   subs_id subscription id  

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_3gpp_pdn_throt_read_config_from_efs_per_subs_id
(
  sys_modem_as_id_e_type  subs_id
)
{
  ds3gcfgmgr_efs_token_type  efs_db; /* structure used for tokenizing the file*/
  char *from, *to; /* ptrs to start and end of the token */
  ds3gcfgmgr_efs_token_parse_status_type  ret_val 
                                              = DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS;
  boolean param_name = TRUE; /* flag to keep track of the parser state 
                                TRUE - parser expects Token Name in the EFS
                                FALSE - parser expects Token Val in the EFS*/
  uint8 token_id = 0;
  char file_name[] 
                = "/nv/item_files/modem/data/epc/pdn_throttling_config.txt";
  int atoi_result = 0;
/*---------------------------------------------------------------------------*/
   if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }


  if( ds3gcfgmgr_efs_file_init(file_name, &efs_db) == -1)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Failure reading PDN throttling cnfg");
    return;
  }


  /* Set the seperator as : */
  efs_db.seperator = ':';
/*lint -save -esym(644,token_id) param_name boolean flag takes care of this */
  while (DS3GCFGMGR_EFS_TOKEN_PARSE_EOF 
          != (ret_val = ds3gcfgmgr_efs_tokenizer(&efs_db, &from, &to )))
  {
    /*------------------------------------------------------------------------
      Token being read. 'from' points to the beginning of the token and 
      'to' point to the end of the token.

      The tokenizer automatically skips blank lines and comments (lines 
      beginning with #, so no need to check for them here).
    ------------------------------------------------------------------------*/
    if((from == to) || (DS3GCFGMGR_EFS_TOKEN_PARSE_EOL == ret_val))
    {
      /*----------------------------------------------------------------------
        Skip empty tokens.
      ----------------------------------------------------------------------*/
      continue;
    }     
    else if(DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS == ret_val)
    {
      /*--------------------------------------------------------------------- 
      check if we are looking for param name or param value 
      ---------------------------------------------------------------------*/
      if (param_name == TRUE)
      {
        /*------------------------------------------------------------------
        get the token identifier for this param name 
        ------------------------------------------------------------------*/
        if (ds_util_get_throttle_token_id(from,to,&token_id) < 0)
        {
          /* This is an error scenario, Skip till the end of the line? */
          DATA_3GMGR_MSG0( MSG_LEGACY_ERROR, "Incorrect Param Name" );
        }
        else
        {
          DATA_3GMGR_MSG1( MSG_LEGACY_HIGH, "Token Id: %d", token_id );
          /*-----------------------------------------------------------------
          set param_name as FALSE This means the next token is a 
          param value
          -----------------------------------------------------------------*/
          param_name = FALSE;
          /* set the seperator as ; */
          efs_db.seperator = ';';
        }
        
      }
      /*---------------------------------------------------------------------
      This means that the token is a param value
      ---------------------------------------------------------------------*/
      else 
      {
        /*-------------------------------------------------------------------
         pass in the identifier and param value to fill the PDN throttle 
         config structure
         ------------------------------------------------------------------*/
        if(DS_3GPP_PDN_THROTTLE_MAX_FAILURE_TIMERS >= token_id)
        {
          DSUTIL_ATOI(from,to,atoi_result);
          ds_3gpp_pdn_throt_fill_config_with_token_contents(token_id,
                                                            (uint32)atoi_result,
                                                            subs_id);
        }
        /*-------------------------------------------------------------------
         set param_name as TRUE This means that next token is a param name
        -------------------------------------------------------------------*/
        param_name = TRUE;
        /* Set the seperator as : */
        efs_db.seperator = ':';
      }
    }
  } /* end of while loop */

  /*----------------------------------------------------------------------- 
    Fill failure timers 6-10 if necessary
    -----------------------------------------------------------------------*/
  ds_3gpp_pdn_throt_fill_pend_failure_timer(subs_id);

/*lint -restore (644,token_id)*/
  ds3gcfgmgr_efs_file_close(&efs_db);
  return;
} /* ds_read_pdn_throttle_config_from_efs() */

/*===========================================================================
FUNCTION      DS_3GPP_PDN_THROT_SET_FAILURE_TIMER 

DESCRIPTION   This function is to assign pdn throttling failure timer value
              manually

PARAMETERS    counter - Throttling counter used as an index

RETURN VALUE  none

SIDE EFFECTS  None
===========================================================================*/
void ds_3gpp_pdn_throt_set_failure_timer 
(
   uint8 counter,
   uint32 timerval,
   sys_modem_as_id_e_type subs_id
)
{
  /*-------------------------------------------------------------------------
    Decrement the counter by 1 so that it can be used as an index into timer
    array also check for its validity
  -------------------------------------------------------------------------*/
  counter -= 1;

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  if(counter >= DS_3GPP_PDN_THROTTLE_MAX_FAILURE_TIMERS)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR, "Invalid throttle counter value: %d", 
                    counter);
    return;
  }

  if (ds_3gpp_pdn_throt_config_p[subs_id] == NULL)
  {

    ds_3gpp_pdn_throt_config_p[subs_id] =
      (ds_3gpp_pdn_throt_config_type *)
        modem_mem_alloc(sizeof(ds_3gpp_pdn_throt_config_type),
                        MODEM_MEM_CLIENT_DATA);

    if(ds_3gpp_pdn_throt_config_p[subs_id] != NULL)
    {
      memset((void*)ds_3gpp_pdn_throt_config_p[subs_id],0,
             (sizeof(ds_3gpp_pdn_throt_config_type)));
    }
    else
    {
      DS_3GPP_MSG0_ERROR("Heap allocation failed");
      return;
    }
  }

  ds_3gpp_pdn_throt_config_p[subs_id]->failure_timer[counter] = timerval;
  DS_3GPP_MSG2_HIGH("assigned timer value :%d for failure_count %d manually",
                     ds_3gpp_pdn_throt_config_p[subs_id]->failure_timer[counter],
                     counter);
  return;

} /*ds_3gpp_pdn_throt_set_failure_timer*/


