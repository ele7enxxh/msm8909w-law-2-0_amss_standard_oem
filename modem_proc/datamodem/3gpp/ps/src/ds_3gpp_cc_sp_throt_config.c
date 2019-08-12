/*!
  @file
  ds_3gpp_cc_sp_throt_config.c

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/src/ds_3gpp_cc_sp_throt_config.c#1 $

when         who     what, where, why
--------     ---     --------------------------------------------------------
04/16/15     vb      Handle cc_sp throttling configuration
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include "ds_3gpp_cc_sp_throt_config.h"
#include "ds_3gpp_pdn_throttle_sm.h"

/*===========================================================================

                           DEFINITIONS

===========================================================================*/


#define DS_3GPP_CC_SP_MAX_NUM_THROTTLE_CAUSE_CODES  5
#define DS_3GPP_CC_SP_MAX_CAUSE_CODE_NAME_LENGTH    3

/*------------------------------------------------------------------------- 
  CC SP structure maintaining timer_val/causecode and failure_count
  -------------------------------------------------------------------------*/
typedef struct 
{
  uint16 timerval;
  uint16 fail_count;
  uint16 cause_code;
}ds_3gpp_cc_sp_throt_timer_type;

/*-------------------------------------------------------------------------- 
  CC SP enum type
  --------------------------------------------------------------------------*/
typedef enum
{
  DS_3GPP_CC_SP_THROT_CAUSE_CODE_NUMBER,
  DS_3GPP_CC_SP_THROT_MAX_FAILURE_COUNTER,
  DS_3GPP_CC_SP_THROT_TIMER_VALUE
}ds_3gpp_throt_cc_sp_cfg_item_e_type;

 ds_3gpp_cc_sp_throt_timer_type 
   *ds_3gpp_cc_sp_throt_timer_table[DS3GSUBSMGR_SUBS_ID_MAX]
                      [DS_3GPP_CC_SP_MAX_NUM_THROTTLE_CAUSE_CODES] = {{NULL}}; 


static uint32 ds_3gpp_cc_sp_plmn_unblock_timer_val = 0;

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
)
{  
  int i=0;
/*--------------------------------------------------------------------------*/
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }  

  for(i=0;i < DS_3GPP_CC_SP_MAX_NUM_THROTTLE_CAUSE_CODES;i++)
  {
    if (ds_3gpp_cc_sp_throt_timer_table[subs_id][i] != NULL)
    {
      modem_mem_free(ds_3gpp_cc_sp_throt_timer_table[subs_id][i],MODEM_MEM_CLIENT_DATA);

      DS_3GPP_MSG2_HIGH("Clearing cc_sp_throttle_timer_table  "
                        "for subs Id cause code",
                        subs_id,
                        i);
    }
    ds_3gpp_cc_sp_throt_timer_table[subs_id][i] = NULL;
  }
  ds_3gpp_throt_sm_set_cc_sp_throttling(subs_id,FALSE);
}/*ds_3gpp_cc_sp_throt_reset_per_subs_id*/

/*===========================================================================
FUNCTION      ds_3gpp_cc_sp_throt_config_with_token_contents

DESCRIPTION   This function gets the data for each token and populates the 
              structure with the appropriate data that corresponds to the 
              token number.   

PARAMETERS    token_id -  describes the current token which is 
                          being populated into the EFS structure

              *from, *to - start and end of the character array which 
                           holds the data to populate the profile structure

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_3gpp_cc_sp_throt_config_with_token_contents
(
  uint8                                token_id,
  char                                 *from,
  char                                 *to,
  ds_3gpp_throt_cc_sp_cfg_item_e_type  cfg_item,
  sys_modem_as_id_e_type               subs_id
)
{
  int                 atoi_result;
  /*-----------------------------------------------------------------------*/
  DS_3GPP_MSG1_HIGH("ds_cc_sp_throttle_config_with_token_contents: %d", cfg_item);

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;   
  }

  switch(cfg_item)
  {
    case DS_3GPP_CC_SP_THROT_CAUSE_CODE_NUMBER:
    {
      DSUTIL_ATOI(from,to,atoi_result);
      if(atoi_result == 0)
      {
        break;
      }
      if((token_id < DS_3GPP_CC_SP_MAX_NUM_THROTTLE_CAUSE_CODES) &&
         (ds_3gpp_cc_sp_throt_timer_table[subs_id][token_id] == NULL ))
      {
        ds_3gpp_cc_sp_throt_timer_table[subs_id][token_id] = 
          (ds_3gpp_cc_sp_throt_timer_type*)modem_mem_alloc(sizeof(ds_3gpp_cc_sp_throt_timer_type), 
                                                MODEM_MEM_CLIENT_DATA);

        ds_3gpp_cc_sp_throt_timer_table[subs_id][token_id]->cause_code = atoi_result;
      }
    }
    break;

    case DS_3GPP_CC_SP_THROT_MAX_FAILURE_COUNTER:
    {
      DSUTIL_ATOI(from,to,atoi_result);
      if(atoi_result == 0)
      {
        break;
      }
      if((token_id < DS_3GPP_CC_SP_MAX_NUM_THROTTLE_CAUSE_CODES) && 
         (ds_3gpp_cc_sp_throt_timer_table[subs_id][token_id] != NULL ))
      {
        ds_3gpp_cc_sp_throt_timer_table[subs_id][token_id]->fail_count = atoi_result;
      }
    }
    break;

    case DS_3GPP_CC_SP_THROT_TIMER_VALUE:
    {
      DSUTIL_ATOI(from,to,atoi_result);
      if(atoi_result == 0)
      {
        break;
      }

      if((token_id < DS_3GPP_CC_SP_MAX_NUM_THROTTLE_CAUSE_CODES) && 
         (ds_3gpp_cc_sp_throt_timer_table[subs_id][token_id] != NULL ))
      {
        ds_3gpp_cc_sp_throt_timer_table[subs_id][token_id]->timerval= atoi_result;
      }
    }
    break;

    default:
      DS_3GPP_MSG0_ERROR("invalid option");
  }
  return;
} /* ds_3gpp_cc_sp_throt_config_with_token_contents */


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
)
{
  uint16 i;  /* counter for number of params */
  boolean found = FALSE;
  uint32 timer_val = 0;
/*-------------------------------------------------------------------------*/
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return 0;   
  }

  
  for(i=0;i<DS_3GPP_CC_SP_MAX_NUM_THROTTLE_CAUSE_CODES;i++)
  {
    if((ds_3gpp_cc_sp_throt_timer_table[subs_id][i] != NULL) &&
       (ds_3gpp_cc_sp_throt_timer_table[subs_id][i]->cause_code == cause_code))
    {
      found = TRUE;
      DS_3GPP_MSG0_ERROR("entry already present ");
      break;
    }
  }
  if(found && counter <= ds_3gpp_cc_sp_throt_timer_table[subs_id][i]->fail_count )
  {
    timer_val = ds_3gpp_cc_sp_throt_timer_table[subs_id][i]->timerval;
  }
  else if(found == FALSE)
  {
   return 0;
  }
  else
  {
   return DS_3GPP_PDN_THROT_INFINITE_THROT;
  }
    DS_3GPP_MSG2_HIGH("returning timer value :%d for failure_count %d",
                      timer_val,
                       counter);
    return SEC_TO_MSEC(timer_val);
} /* ds_3gpp_cc_sp_throt_get_timer() */


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
  char                    *from,
  char                    *to,
  uint8                   *token_id,
  sys_modem_as_id_e_type  subs_id
)
{
  uint8 i;           /* counter for number of params */
  uint8 length;      /* length of the token (param name) */
  int16 atoi_result;
  int8 first_free_index = -1;
  boolean found = FALSE;
  /*-----------------------------------------------------------------------*/
  DSUTIL_ATOI(from,to,atoi_result);
  
  DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, "in ds_cc_sp_throttling_get_token_id %d", atoi_result);

  length =(uint8)(to-from);
  if(atoi_result == 0 || length > DS_3GPP_CC_SP_MAX_CAUSE_CODE_NAME_LENGTH)
  {
   return -1;
  }

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return -1;
  }

  for(i=0;i<DS_3GPP_CC_SP_MAX_NUM_THROTTLE_CAUSE_CODES;i++)
  {
    if(ds_3gpp_cc_sp_throt_timer_table[subs_id][i] == NULL && first_free_index == -1)
    {
     first_free_index = i;
    }
    else if((ds_3gpp_cc_sp_throt_timer_table[subs_id][i] != NULL) && 
          (ds_3gpp_cc_sp_throt_timer_table[subs_id][i]->cause_code == atoi_result))
    {
      found = TRUE;
      *token_id = i;
       DS_3GPP_MSG0_ERROR("entry already present ");
       break;
    }
  }
  if(found)
  {
    return -1;
  }
  *token_id = first_free_index;
  return 0;
} /*ds_3gpp_cc_sp_throt_get_token_id()*/

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
)
{
  uint16 i;  /* counter for number of params */

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return FALSE;   
  }

  DS_3GPP_MSG1_MED ("Cause code sent is: %d", cause_code);

  for(i=0;i<DS_3GPP_CC_SP_MAX_NUM_THROTTLE_CAUSE_CODES;i++)
  {
    if((ds_3gpp_cc_sp_throt_timer_table[subs_id][i] != NULL) &&
       (ds_3gpp_cc_sp_throt_timer_table[subs_id][i]->cause_code == cause_code))
    {
      DS_3GPP_MSG1_MED ("Cause code found in table: %d", 
                          ds_3gpp_cc_sp_throt_timer_table[subs_id][i]);
      return TRUE;
    }
  }
 return FALSE;
}/*ds_3gpp_cc_sp_throt_is_cause_code_configured*/


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
)
{
  DS_3GPP_MSG1_HIGH("returning plmn_unblock_timer_val :%d" ,
                    ds_3gpp_cc_sp_plmn_unblock_timer_val);

  return (ds_3gpp_cc_sp_plmn_unblock_timer_val*1000);
};/*ds_3gpp_cc_sp_throt_get_plmn_unblock_timer_val*/

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
)
{
  if(timer_val>0)
  {
    ds_3gpp_cc_sp_plmn_unblock_timer_val = timer_val;
  }
}/*ds_3gpp_cc_sp_set_plmn_unblock_timer_val()*/


/*===========================================================================
FUNCTION      DS_3GPP_CC_SP_THROT_READ_CONFIG_FROM_EFS_PER_SUBS_ID

DESCRIPTION   This function will read from the efs file all the necessary 
              data & fill the PDN throttle config structure. Only valid data 
              is populated.

              EFS File Format - Param_Name:Param_Val;
              For example     - FAILURE_TIMER_1:0;   
 
PARAMATERS    subs_id subscription id 
 
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/

void ds_3gpp_cc_sp_throt_read_config_from_efs_per_subs_id
(
  sys_modem_as_id_e_type        subs_id
)
{
  ds3gcfgmgr_efs_token_type               efs_db; /* structure used for tokenizing the file*/
  char                                   *from, *to; /* ptrs to start and end of the token */
  ds3gcfgmgr_efs_token_parse_status_type  ret_val = DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS;
  boolean                                 param_name = TRUE; /* flag to keep track of the parser state 
                                                                TRUE - parser expects Token Name in the EFS
                                                                FALSE - parser expects Token Val in the EFS*/
  uint8                                   i = 0;
  uint8                                   token_id = 0;
  uint8                                   param_num = 0;
  uint16                                  length;
  int16                                   atoi_result;
  char                                    file_name[] = "/nv/item_files/modem/data/epc/cc_sp_throttle_timer.txt";

/*-----------------------------------------------------------------------------------------*/
  DS_3GPP_MSG0_HIGH("in ds_read_cc_sp_throttle_config_from_efs");

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }  

  if (ds_3gpp_throt_sm_is_cc_sp_throttling_enabled(subs_id))
  {
    DS_3GPP_MSG1_HIGH("cc_sp_throttling FOR Subs Id is already initialized"
                      ,subs_id);
    return;
  }

  for(i=0;i<DS_3GPP_CC_SP_MAX_NUM_THROTTLE_CAUSE_CODES;i++)
  {
    if(ds_3gpp_cc_sp_throt_timer_table[subs_id][i] != NULL)
    {
      DS_3GPP_MSG1_HIGH("Something wrong here, feature flag not set but memory"
                        "valid" ,subs_id);
      modem_mem_free(ds_3gpp_cc_sp_throt_timer_table[subs_id][i], 
                     MODEM_MEM_CLIENT_DATA);
      ds_3gpp_cc_sp_throt_timer_table[subs_id][i] = NULL;     
    }
  }

  //memset((void*)&cc_sp_throttle_timer_table[subs_id], 0, sizeof(throttle_timer_type));
  if(ds3gcfgmgr_efs_file_init(file_name,&efs_db) == -1)
  {
    DS_3GPP_MSG0_ERROR("Failure reading PDN throttling cnfg");
    return ;
  }
  /*read NV value*/
  /* Set the seperator as : */
  efs_db.seperator = ':';
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
      if( param_name == TRUE)
      {
        length =(uint8)(to-from);
        if((0 != strncmp(from,"enable",length)) &&
           (0 != strncmp(from,"ENABLE",length)))
        {
          DS_3GPP_MSG0_ERROR ("config file is not configured properly, "
                              "enable flag not found");
          return;
        }
        efs_db.seperator = ';';
        param_name = FALSE;
      }
      else
      {
        length =(uint8)(to-from);
        DSUTIL_ATOI(from,to,atoi_result);
        if(atoi_result == 1)
        {
         ds_3gpp_throt_sm_set_cc_sp_throttling(subs_id,TRUE);
        }
        break;
      }
    }
  }
    /* Set the seperator as : */
  param_name = TRUE;
  efs_db.seperator = ':';
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
      if( param_name == TRUE)
      {
        length =(uint8)(to-from);
        if((0 != strncmp(from,"unblocktimer",length)) && (0 != strncmp(from,"UNBLOCKTIMER",length)))
        {
          DS_3GPP_MSG0_ERROR("config file is not configured properly, enable flag not found");
          break;
        }
        efs_db.seperator = ';';
        param_name = FALSE;
      }
      else
      {
        length =(uint8)(to-from);
        DSUTIL_ATOI(from,to,atoi_result);
        if(atoi_result >=0)
        {
          ds_3gpp_cc_sp_set_plmn_unblock_timer_val(atoi_result);
        }
        break;
      }
    }
  }
  efs_db.seperator = ':';
  /*Do not get the next token if the current token is not unblocktimer*/
  /*lint -save -esym(644,token_id) param_name boolean flag takes care of this */
  while ((param_name)||(DS3GCFGMGR_EFS_TOKEN_PARSE_EOF
           != (ret_val = ds3gcfgmgr_efs_tokenizer(&efs_db, &from, &to ))))
  {
    param_name = FALSE;
    /*-----------------------------------------------------------------------
      Token being read. 'from' points to the beginning of the token and
      'to' point to the end of the token.
      The tokenizer automatically skips blank lines and comments (lines
      beginning with #, so no need to check for them here).
    -----------------------------------------------------------------------*/
    if((from == to) || (DS3GCFGMGR_EFS_TOKEN_PARSE_EOL == ret_val))
    {
      /*---------------------------------------------------------------------
        Skip empty tokens.
      ---------------------------------------------------------------------*/
      continue;
    }
    else if(DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS == ret_val)
    {
      /*---------------------------------------------------------------------
        check if we are looking for param name or param value
      ---------------------------------------------------------------------*/
      if (param_num < 2 )
      {
        /*-------------------------------------------------------------------
          get the token identifier for this param name
        -------------------------------------------------------------------*/
        if (param_num == 0)
        {
          if( ds_3gpp_cc_sp_throt_get_token_id(from,to,&token_id,subs_id) < 0)
          {
            /* This is an error scenario, Skip till the end of the line? */
            DS_3GPP_MSG0_ERROR("Incorrect Param Name" );
          }
          else
          {
            efs_db.seperator = ':';
            param_num++;
            ds_3gpp_cc_sp_throt_config_with_token_contents(token_id, 
                                                           from, 
                                                           to,
                                                           DS_3GPP_CC_SP_THROT_CAUSE_CODE_NUMBER,
                                                           subs_id);
          }
        }
        else if(param_num == 1)
        {
          efs_db.seperator = ';';
          param_num++;
          ds_3gpp_cc_sp_throt_config_with_token_contents(token_id, 
                                                         from,
                                                         to,
                                                         DS_3GPP_CC_SP_THROT_MAX_FAILURE_COUNTER,
                                                         subs_id);
        }
        else
        {
          DS_3GPP_MSG1_ERROR("wrong input Token Id: %d",token_id );
         /*-----------------------------------------------------------------
          set param_name as FALSE This means the next token is a 
          param value
          -----------------------------------------------------------------*/
          param_num++;
          efs_db.seperator = ';';
          continue;
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
        ds_3gpp_cc_sp_throt_config_with_token_contents( token_id,
                                                     from, to,
                                                     DS_3GPP_CC_SP_THROT_TIMER_VALUE,
                                                      subs_id);
       /*-------------------------------------------------------------------
        set param_name as TRUE This means that next token is a param name
        -------------------------------------------------------------------*/
        param_num = 0;
        /* Set the seperator as : */
        efs_db.seperator = ':';
      }
    }
  } /* end of while loop */

}/*ds_3gpp_cc_sp_throt_read_config_from_efs_per_subs_id*/

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

