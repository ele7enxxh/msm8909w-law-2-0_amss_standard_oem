/*!
  @file
  ds_3gpp_srv_req_throt_config.c

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/src/ds_3gpp_srv_req_throt_config.c#1 $

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
#include "ds_3gpp_srv_req_throt_config.h"
#include "ds_3gpp_nv_manager.h"



/*-------------------------------------------------------------------------- 
  These are used to indicate what is the timer read from the service request
  config txt
  0 -- Service request throttling time
  1 -- Service request min random time
  2 -- Service request max random time
  --------------------------------------------------------------------------*/
#define DS_3GPP_SRV_REQ_THROT_TIME   0
#define DS_3GPP_SRV_REQ_MIN_RAN_TIME 1
#define DS_3GPP_SRV_REQ_MAX_RAN_TIME 2


#define DS_3GPP_SRV_REQ_THROTTLE_MAX_FAILURE_TIMERS 10 /* max number of srv req failure timers */
#define DS_3GPP_SRV_REQ_THROTTLE_MIN_FAILURE_TIMERS  1 /* min number of failure timers */
#define DS_3GPP_SRV_REQ_THROTTLE_REF_FAILURE_TIMERS  6 /* Fill the srv req failure timers from 6-10*/
/*-----------------------------------------------------------------------------
  Structure used to store the service req throttle config values from EFS
-----------------------------------------------------------------------------*/
typedef struct
{
  uint32 failure_timer[DS_3GPP_SRV_REQ_THROTTLE_MAX_FAILURE_TIMERS];   /* Failure Timer  */
  uint32 rand_timer_min[DS_3GPP_SRV_REQ_THROTTLE_MAX_FAILURE_TIMERS];  /* Random min timer*/
  uint32 rand_timer_max[DS_3GPP_SRV_REQ_THROTTLE_MAX_FAILURE_TIMERS];  /* Random max timer*/
}ds_3gpp_srv_req_throt_config_type;

/*---------------------------------------------------------------------------
  Structure to store throttle config values
---------------------------------------------------------------------------*/
ds_3gpp_srv_req_throt_config_type  
             *ds_3gpp_srv_req_throt_config_p[DS3GSUBSMGR_SUBS_ID_MAX] = {NULL};

/*===========================================================================
FUNCTION       DS_3GPP_SRV_REQ_THROT_FILL_CONFIG_WITH_TOKEN_CONTENTS

DESCRIPTION   This function gets the data for each token and populates the 
              structure with the appropriate data that corresponds to the 
              token number.   

PARAMETERS    token_id -  describes the current token which is 
                          being populated into the EFS structure
 
              subs_id

              srv_req_timer_index -- Indicates if the timer value read is
                                     srv_req_throttle_timer or min/max random
                                     timer values
 
              failure_timer_value
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
)
{
 /*----------------------------------------------------------------------*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  } 

  if(token_id >= DS_3GPP_SRV_REQ_THROTTLE_MAX_FAILURE_TIMERS)
  {
    DS_3GPP_MSG1_ERROR("Token ID:%d is invalid",token_id);
    return;
  }
  
  /*-------------------------------------------------------------------------
    Init to default values
  -------------------------------------------------------------------------*/

  if (ds_3gpp_srv_req_throt_config_p[subs_id] == NULL)
  {

    ds_3gpp_srv_req_throt_config_p[subs_id] =
      (ds_3gpp_srv_req_throt_config_type *)
        modem_mem_alloc(sizeof(ds_3gpp_srv_req_throt_config_type),
                        MODEM_MEM_CLIENT_DATA);

    if(ds_3gpp_srv_req_throt_config_p[subs_id] != NULL)
    {
      memset((void*)ds_3gpp_srv_req_throt_config_p[subs_id],0,
             (sizeof(ds_3gpp_srv_req_throt_config_type)));
    }
    else
    {
      DS_3GPP_MSG0_ERROR("Heap allocation failed");
      return;
    }
  }

  /*--------------------------------------------------------------------------- 
    Use the timer index to know if the time read is for service request
    throttling or service request random time
  ---------------------------------------------------------------------------*/
  switch (srv_req_timer_index)
  {
    case DS_3GPP_SRV_REQ_THROT_TIME:
      ds_3gpp_srv_req_throt_config_p[subs_id]->failure_timer[token_id] = 
                                                   (uint32)failure_value;
      break;

    case DS_3GPP_SRV_REQ_MIN_RAN_TIME:
      ds_3gpp_srv_req_throt_config_p[subs_id]->rand_timer_min[token_id] =
                                                   (uint32)failure_value;
      break;

    case DS_3GPP_SRV_REQ_MAX_RAN_TIME:
      ds_3gpp_srv_req_throt_config_p[subs_id]->rand_timer_max[token_id] = 
                                                   (uint32)failure_value;
      break;

    default:
      DS_3GPP_MSG0_ERROR("Invalid srv_req_throt_index passed");
      break;
  }

  return;
}/*ds_3gpp_srv_req_throt_fill_config_with_token_contents*/

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
)
{
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }  

  if (ds_3gpp_srv_req_throt_config_p[subs_id]!=NULL)
  {
    modem_mem_free(ds_3gpp_srv_req_throt_config_p[subs_id],MODEM_MEM_CLIENT_DATA);
    DS_3GPP_MSG1_HIGH("Clearing srv throttling cnfg for subs_id",subs_id);
    ds_3gpp_srv_req_throt_config_p[subs_id] = NULL;
  }

}/*ds_3gpp_srv_req_throt_reset_config_from_efs_per_subs_id*/

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
)
{
  uint32 return_timer_value =0;
  /*-----------------------------------------------------------------------*/
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    DS_3GPP_MSG1_ERROR("Invalid subs ID %d",subs_id);    
    return return_timer_value;
  }

  if((counter > DS_3GPP_SRV_REQ_THROTTLE_MAX_FAILURE_TIMERS) ||
     (counter < DS_3GPP_SRV_REQ_THROTTLE_MIN_FAILURE_TIMERS))
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

  if(ds_3gpp_srv_req_throt_config_p[subs_id] != NULL)
  {
    return_timer_value = SEC_TO_MSEC
              (ds_3gpp_srv_req_throt_config_p[subs_id]->failure_timer[counter]);

    DS_3GPP_MSG2_HIGH("returning timer value :%d for failure_count %d",
                       ds_3gpp_srv_req_throt_config_p[subs_id]->failure_timer[counter],
                       counter);
			  
  }
  
  return return_timer_value;
} /* ds_3gpp_srv_req_throt_get_failure_timer */

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
)
{
  /*-----------------------------------------------------------------------*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }


  if((counter > DS_3GPP_SRV_REQ_THROTTLE_MAX_FAILURE_TIMERS) ||
     (counter < DS_3GPP_SRV_REQ_THROTTLE_MIN_FAILURE_TIMERS))
  {
    DS_3GPP_MSG1_ERROR("Invalid throttle counter value: %d", 
                        counter);
    return;
  }

  if((min_random_timer_p == NULL) ||
     (max_random_timer_p == NULL))
  {
    DS_3GPP_MSG2_ERROR("Input ptr is NULL: "
                       "max_random_timer:0x%x:min_random_timer:0x%x",
                       min_random_timer_p,max_random_timer_p);
    return;
  }

  *min_random_timer_p = 0;
  *max_random_timer_p = 0;

  /*-------------------------------------------------------------------------
    Decrement the counter by 1 so that it can be used as an index into timer
    array also check for its validity
  -------------------------------------------------------------------------*/
  counter -= 1;

  if(ds_3gpp_srv_req_throt_config_p[subs_id]!= NULL)
  {
    *min_random_timer_p = ds_3gpp_srv_req_throt_config_p[subs_id]->
                                      rand_timer_min[counter];

    *max_random_timer_p = ds_3gpp_srv_req_throt_config_p[subs_id]->
                                      rand_timer_max[counter];
  }
  
  return;
} /* ds_3gpp_srv_req_throt_get_random_timers() */

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

  if(counter >= DS_3GPP_SRV_REQ_THROTTLE_MAX_FAILURE_TIMERS)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR, "Invalid srv req throttle counter value: %d", 
                    counter);
    return;
  }
  
  if (ds_3gpp_srv_req_throt_config_p[subs_id] == NULL)
  {
    ds_3gpp_srv_req_throt_config_p[subs_id] =
      (ds_3gpp_srv_req_throt_config_type *)
        modem_mem_alloc(sizeof(ds_3gpp_srv_req_throt_config_type),
                        MODEM_MEM_CLIENT_DATA);

    if(ds_3gpp_srv_req_throt_config_p[subs_id] != NULL)
    {
      memset((void*)ds_3gpp_srv_req_throt_config_p[subs_id],0,
             (sizeof(ds_3gpp_srv_req_throt_config_type)));
    }
    else
    {
      DS_3GPP_MSG0_ERROR("Heap allocation failed");
      return;
    }
  }

  ds_3gpp_srv_req_throt_config_p[subs_id]->failure_timer[counter] = timerval;
  DS_3GPP_MSG2_HIGH("assigned timer value :%d for failure_count %d manually",
                     ds_3gpp_srv_req_throt_config_p[subs_id]->failure_timer[counter],
                     counter);
  return;

} /*ds_3gpp_srv_req_throt_set_failure_timer*/

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
)
{
  ds3gcfgmgr_efs_token_type  efs_db; /* structure used for tokenizing the file*/
  char *from, *to; /* ptrs to start and end of the token */
  ds3gcfgmgr_efs_token_parse_status_type  ret_val 
                                              = DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS;
  boolean param_name = TRUE; /* flag to keep track of the parser state 
                                TRUE - parser expects Token Name in the EFS
                                FALSE - parser expects Token Val in the EFS*/
  uint8  token_id = 0;
  uint8  srv_req_timer_index = 0; /*
                                   If 0 -- Srv req throttle value is read
                                      1 -- Min random timer value is read
                                      2 -- Max random timer value
                                  */

  char   file_name[] 
                = "/data/3gpp/throttling/srv_req_throt_config.txt";

  int    atoi_result = 0;

/*-------------------------------------------------------------------------*/
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }


  if(ds3gcfgmgr_efs_file_init(file_name,&efs_db) == -1)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Failure reading throttling cnfg");
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
          efs_db.seperator = '*';
        }
      }
      /*---------------------------------------------------------------------
      This means that the token is a param value
      ---------------------------------------------------------------------*/
      else 
      {
        /*-------------------------------------------------------------------
         pass in the identifier and param value to fill the throttle 
         config structure
         ------------------------------------------------------------------*/
         if(DS_3GPP_SRV_REQ_THROTTLE_MAX_FAILURE_TIMERS >= token_id)
         {
           DSUTIL_ATOI(from,to,atoi_result);

           ds_3gpp_srv_req_throt_fill_config_with_token_contents(token_id,
                                                                 subs_id,
                                                                 srv_req_timer_index,
                                                                 (uint32)atoi_result);
         }
        /*-------------------------------------------------------------------
         set param_name as TRUE This means that next token is a param name
        -------------------------------------------------------------------*/
        if(efs_db.seperator == ';')
        {
          param_name = TRUE;
          /* Set the seperator as : */
          efs_db.seperator = ':';
          srv_req_timer_index =  0;
        }
        else
        {
          efs_db.seperator = '*';
          srv_req_timer_index++;
        }
      }
    }
  } /* end of while loop */

 /*----------------------------------------------------------------------- 
    Fill failure timers 6-10 if necessary
 -----------------------------------------------------------------------*/
  ds_3gpp_srv_req_throt_fill_pend_failure_timer(subs_id);

/*lint -restore (644,token_id)*/
  ds3gcfgmgr_efs_file_close(&efs_db);
  return;
} /* ds_3gpp_srv_req_read_throttle_config_from_efs_per_subs_id */


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
    for(idx = DS_3GPP_SRV_REQ_THROTTLE_REF_FAILURE_TIMERS; 
        idx < DS_3GPP_SRV_REQ_THROTTLE_MAX_FAILURE_TIMERS; idx++)
    {
      ds_3gpp_srv_req_throt_config_p[subs_id]->failure_timer[idx] = 
                     ds_3gpp_srv_req_throt_config_p[subs_id]->failure_timer[idx-1];
    }
  }
 
  for(idx = 0; idx < DS_3GPP_SRV_REQ_THROTTLE_MAX_FAILURE_TIMERS; idx++)
  {
    DS_3GPP_MSG2_HIGH("srv_req_throttle.failure_timer[%d]: %d",(idx+1),
                       ds_3gpp_srv_req_throt_config_p[subs_id]->failure_timer[idx]);
  }

  return;

}/*ds_3gpp_srv_req_throt_fill_pend_failure_timer*/
