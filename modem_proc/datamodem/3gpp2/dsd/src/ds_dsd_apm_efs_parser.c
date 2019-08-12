/*===========================================================================
                        DSD APM EFS PARSER

GENERAL DESCRIPTION
  All the declarations and definitions necessary to support parsing
  of DSD APM related EFS file.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2012-2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsd/src/ds_dsd_apm_efs_parser.c#1 $
  $DateTime: 2016/12/13 08:00:03 $
  $Author: mplcsds1 $


when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/10/13    rs     Added support for NULL APN string
08/22/12    ss     Removing efs write call as that is added to dsutil now.
05/12/12    fjia   Create APM EFS parser module
===========================================================================*/

/*===========================================================================
                          INCLUDE FILES FOR MODULE
===========================================================================*/
#include "msg.h"
#include "data_msg.h"
#include "comdef.h"
#include "dsutil.h"
#include "ds3gcfgmgr.h"
#include "ds3gsubsmgr.h"
#include "ds_dsd_apm_efs_parser.h"
#include "modem_mem.h"
#include "lte.h"
#include "mcfg_fs.h"
#include <stringl/stringl.h>
#include "ds_system_heap.h"
#ifdef FEATURE_DATA_LTE
#include "ds_3gpp_pdn_context.h"
#endif /* FEATURE_DATA_LTE */
/*===========================================================================
                              GLOBAL VARIABLES
===========================================================================*/
#ifdef FEATURE_DATA_LTE
/*---------------------------------------------------------------------------
  An array of strings to map the Parameter Name string (mentioned in EFS
  file) to a token ID. This token ID will be used to modify
  the correct entry in the internal APM rule structure.
---------------------------------------------------------------------------*/
static char dsd_apm_rule_param_name_id[DSD_APM_RULE_MAX_PARAMS_NUM]
                                     [DSD_APM_RULE_MAX_PARAM_NAME_SIZE] = 
                        { "APN_String",
                          "ATTACH_TYPE_RULES",
                          "ON_DEMAND_TYPE_RULES",
                          "P-CSCFRequired",
                          "IPAddressRequired",                          
                          "DNSAddressRequired",
                          "P-CSCF_OR_DNSAddressRequired",
                          "Cause_Code_Rules",
                          "Detach_Network_Cause",
                          "Reject_Network_Cause"
};

static uint8 ds_dsd_apm_attach_prof_status = 0;

/*===========================================================================
                      INTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DSD_ATOI

DESCRIPTION    Converts the character input to integer type

PARAMETERS    x - Begin pointer (char *)
              y - End pointer (char *)

DEPENDENCIES  None

RETURN VALUE  ouput integer

SIDE EFFECTS  None
===========================================================================*/
int dsd_apm_atoi
(
  char* x, char* y
)
{
  int z = 0;
  boolean is_valid = TRUE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  while( x < y )
  {
    if((*x) >= '0' && (*x) <= '9')
    {
      z = z *10 + (*x- '0');
      x++;
    }
    else
    {
      is_valid = FALSE;
      break;
    }
  }

  if(!is_valid)
  {
    return -1;
  }
  return z;
} /* DSD_APM_ATOI() */

/*===========================================================================
FUNCTION      dsd_apm_pdn_rule_get_token_id

DESCRIPTION   This function returns the token id associated with each
              apm rule parameter.

PARAMETERS    char *from - start of the token (param name)
              char *to   - end of the token (param name)

DEPENDENCIES  None

RETURN VALUE  0 - success
              1 - failure

SIDE EFFECTS  None
===========================================================================*/
int16 dsd_apm_pdn_rule_get_token_id
(
  char    *from,
  char    *to,
  int8    *token_id
)
{
  int16 ret_val = -1; /* return value */
  uint8 i;            /* counter for number of params */
  uint8 length;       /* length of the token (param name) */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get the length of the string that is passed */

  if(NULL == from ||
     NULL == to ||
     NULL == token_id ||
     ( from > to ))
  {
    DATA_MSG3(MSG_LEGACY_ERROR, "Invalid Input params, From pointer: 0x%x "
              "To pointer: 0x%x and token id: 0x%x", from, to, token_id);
    return ret_val;
  }

  /* Assumption that the token length will not be greater than 255 */
  length =(uint8)(to-from);

  for (i=0; i<DSD_APM_RULE_MAX_PARAMS_NUM; i++)
  {
    if (length == strlen(dsd_apm_rule_param_name_id[i]))
    {
      if (0 == strncasecmp(from,dsd_apm_rule_param_name_id[i],length))
      {
        *token_id = i;
        ret_val = 0;
        break;
     }
   }
  }
  return ret_val;

}/* dsd_apm_pdn_rule_get_token_id() */

/*===========================================================================
FUNCTION      DSD_APM_GET_APN_STRING_WITH_TOKEN_CONTENT

DESCRIPTION   This function gets apn string from EFS.

PARAMETERS    char *from, *to - start and end of the character array which
                          holds the data to populate the profile structure
              dsd_apm_pdn_name_type  *apn_name - structure to store apn string
 

DEPENDENCIES  None

RETURN VALUE  TRUE - get valid APN string 
              FALSE - APN string is not valid 

SIDE EFFECTS  None
===========================================================================*/
boolean dsd_apm_get_apn_string_with_token_content
(
  char   *from,
  char   *to,
  dsd_apm_apn_name_type   *apn_name
)
{
  uint8   length;
  boolean ret_val = TRUE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Assumption that the token length will not be greater than 255 */
  /*Note: 'to' points to (end of token+1)*/
  length = (uint8)((to-from)/sizeof(char));

  if ( (from > to)  || (length > DSD_APM_MAX_APN_STRING_LEN))
  {
    apn_name->apn_string_len = 0;
    memset(apn_name->apn_string,'\0', DSD_APM_MAX_APN_STRING_LEN);
    DATA_MSG0(MSG_LEGACY_ERROR,"Error in APN String. Setting APN length to 0");
    ret_val = FALSE;
  }
  else
  {
    (void)memscpy(apn_name->apn_string,
                  DSD_APM_MAX_APN_STRING_LEN, from, length);
     apn_name->apn_string_len = length;
  }

  return ret_val;
} /* dsd_apm_get_apn_string_with_token_content() */

/*===========================================================================
FUNCTION      DSD_APM_VALIDATE_RULE_TYPE_WITH_TOKEN_CONTENT

DESCRIPTION   This function check if attach_rule_type/on_demand_rule_type 
              is valid or not 

PARAMETERS    char *from, *to - start and end of the character array which
                          holds the data to populate the profile structure
              boolen  *valid_rule - structure to store valid_rule_type

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dsd_apm_validate_rule_type_with_token_content
(
  char       *from,
  char       *to,
  boolean    *valid_rule
)
{
  uint8   length;
  char    val_true[] = "TRUE";
  char    *start = from;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  while ( *start == ' ' && start < to )
  {
    start++;
  }

  length =(uint8)(to-start);
  if ( (length == strlen(val_true)) && 
                        ( strncasecmp(start, val_true, length) == 0 ) )
  {
    *valid_rule = TRUE;
  }
  else
  {
    *valid_rule = FALSE;
  }
  return;
}/* dsd_apm_validate_rule_type_with_token_content() */

/*===========================================================================
FUNCTION      DSD_APM_FILL_ATTACH_RULE_WITH_TOKEN_CONTENT

DESCRIPTION   This function gets the data for each token and populates apn_info
              structure with the appropriate data that corresponds to the
              token number.

PARAMETERS    token_id -  describes the current token which is
                              being populated into the EFS structure

              char *from, *to - start and end of the character array which
                          holds the data to populate the profile structure
 
              pdn_id - PDN number to fill correspond content

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dsd_apm_fill_attach_rule_with_token_content
(
  uint8  token_id,
  char   *from,
  char   *to,
  apn_info_type *apn_info
)
{
  ipRequiredType  value = 0;
  char    *start = from;
  boolean  token_valid = TRUE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( apn_info == NULL )
  {
    DATA_MSG0(MSG_LEGACY_ERROR,"Invalid apn_info ptr passed");
    return;
  }

  while ( *start == ' ' && start < to )
  {
    start++;
  }

  switch(*start)
  {
    case '1':
      value = ipv4_mandatory_and_ipv6_donotcare;
      break;
    case '2':
      value = ipv6_mandatory_and_ipv4_donotcare;
      break;
    case '3':
      value = ipv4_mandatory_and_ipv6_mandatory;
      break;
    case '4':
      value = ipv4_dontcare_and_ipv6_donotcare;
      break;
    case '5':
      value = ipv4_mandatory_or_ipv6_mandatory;
      break;
    default:
      token_valid = FALSE;
      break;
  }

  if ( token_valid == TRUE )
  {
    DATA_MSG2(MSG_LEGACY_MED,"APM attach rule: token_id : %d ,value %d", 
              token_id,value);
    switch (token_id)
    {
      case DS_DSD_APM_PCSCF_REQUIRED:
        apn_info->AttachPdn.config_rule.config_valid_flag |= CONFIG_PCSCF;
        apn_info->AttachPdn.config_rule.pcscf_address = value;
        break;

      case DS_DSD_APM_IPADDRESS_REQUIRED:
        apn_info->AttachPdn.config_rule.config_valid_flag |= CONFIG_IP_ADDRESS;
        apn_info->AttachPdn.config_rule.ip_address = value;
        break;

      case DS_DSD_APM_DNSADDRESS_REQUIRED:
        apn_info->AttachPdn.config_rule.config_valid_flag 
          |= CONFIG_DNS_ADDRESS;
        apn_info->AttachPdn.config_rule.dns_address = value;
        break;

      case DS_DSD_APM_PCSCF_OR_DNSADDRESS_REQUIRED:
        apn_info->AttachPdn.config_rule.config_valid_flag 
          |= CONFIG_PCSCF_DNS_ADDRESS;
        apn_info->AttachPdn.config_rule.pcscf_or_dns_address = value;
        break;

      default:
        break;
    }
  }
  return;
} /* dsd_apm_fill_attach_rule_with_token_content() */ 

/*===========================================================================
FUNCTION      DSD_APM_FILL_ON_DEMAND_RULE_WITH_TOKEN_CONTENT

DESCRIPTION   This function gets the data for each token and populates the
              apn_info structure with the appropriate data that corresponds
              to the token number.

PARAMETERS    token_id -  describes the current token which is
                              being populated into the EFS structure

              char *from, *to - start and end of the character array which
                          holds the data to populate the profile structure.
 
              pdn_id - PDN number to fill correspond content

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dsd_apm_fill_on_demand_rule_with_token_content
(
  uint8  token_id,
  char   *from,
  char   *to,
  apn_info_type  *apn_info
)
{
  ipRequiredType  value = 0;
  boolean         token_valid = TRUE;
  char            *start = from;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( apn_info == NULL  )
  {
    DATA_MSG0(MSG_LEGACY_ERROR,"Invalid apn info ptr passed");
    return;
  }

  while ( *start == ' ' && start < to )
  {
    start++;
  }

  switch(*start)
  {
    case '1':
      value = ipv4_mandatory_and_ipv6_donotcare;
      break;
    case '2':
      value = ipv6_mandatory_and_ipv4_donotcare;
      break;
    case '3':
      value = ipv4_mandatory_and_ipv6_mandatory;
      break;
    case '4':
      value = ipv4_dontcare_and_ipv6_donotcare;
      break;
    case '5':
      value = ipv4_mandatory_or_ipv6_mandatory;
      break;
    default:
      token_valid = FALSE;
      break;
  }

  if ( token_valid == TRUE )
  {
    DATA_MSG2(MSG_LEGACY_MED,"APM attach rule: token_id : %d ,value %d", 
              token_id,value);
    switch (token_id)
    {
      case DS_DSD_APM_PCSCF_REQUIRED:
        apn_info->OnDemandPdn.config_rule.config_valid_flag |= CONFIG_PCSCF;
        apn_info->OnDemandPdn.config_rule.pcscf_address = value;

        break;

      case DS_DSD_APM_IPADDRESS_REQUIRED:
        apn_info->OnDemandPdn.config_rule.config_valid_flag 
          |= CONFIG_IP_ADDRESS;
        apn_info->OnDemandPdn.config_rule.ip_address = value;
        break;


      case DS_DSD_APM_DNSADDRESS_REQUIRED:
        apn_info->OnDemandPdn.config_rule.config_valid_flag 
          |= CONFIG_DNS_ADDRESS;
        apn_info->OnDemandPdn.config_rule.dns_address = value;
        break;

      case DS_DSD_APM_PCSCF_OR_DNSADDRESS_REQUIRED:
        apn_info->OnDemandPdn.config_rule.config_valid_flag 
          |= CONFIG_PCSCF_DNS_ADDRESS;
        apn_info->OnDemandPdn.config_rule.pcscf_or_dns_address = value;
        break;

      default:
        break;
    }
  }
  return;
} /* dsd_apm_fill_on_demand_rule_with_token_content() */ 

/*===========================================================================
FUNCTION      DSD_APM_EXTRACT_APN_NAME_FROM_PROFILE

DESCRIPTION   This function extracts the APN name from Profile if new attach 
              profile lists have been cached 

PARAMETERS    subs_id:                         Current active PS subscription 
              attach_prof_q:                   attach profile queue
              rule_index:                      Index to match corresponding
                                               prof in attach prof queue
              apn_name:                        Structure to hold APN name

DEPENDENCIES  None

RETURN VALUE  TRUE: IF read from profile was successful 
              FALSE: Otherwise 

SIDE EFFECTS  None
===========================================================================*/
boolean dsd_apm_extract_apn_name_from_profile
(
  sys_modem_as_id_e_type              subs_id,
  q_type                             *attach_prof_q,
  uint16                              rule_index,
  dsd_apm_apn_name_type              *apn_name
)
{
  attach_profile_data_block          *profile_block = NULL;
  ds_profile_info_type                prof_apn_name = {0, 0};
  boolean                             retval = FALSE;
  uint16                              prof_index = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(apn_name == NULL || attach_prof_q == NULL)
  {
    DATA_MSG0(MSG_LEGACY_ERROR, "Invalid Input Parameters");
    return retval;
  }

  profile_block = (attach_profile_data_block *)q_check(attach_prof_q);

  while(profile_block != NULL)
  {
    if(profile_block->attach_profile.subs_id == subs_id)
    {
      prof_index++;
      if(prof_index == rule_index)
      {
        /*---------------------------------------------------------------------
          Get the apn_name from profile database
        ---------------------------------------------------------------------*/
        memset(apn_name, '\0', sizeof(dsd_apm_apn_name_type));
        prof_apn_name.buf = apn_name->apn_string;
        prof_apn_name.len = DS_DSD_APM_MAX_APN_STRING_LEN;

        if ( dsd_apm_get_profile_param_info(
               profile_block->attach_profile.profile,
               DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_APN,
               &prof_apn_name,
               subs_id) == TRUE )
        {
          /*---------------------------------------------------------------------
            update the apn_name in the rule block
          ---------------------------------------------------------------------*/
          retval = TRUE;
          apn_name->apn_string_len = (uint8)prof_apn_name.len;
        }
        else
        {
          retval = FALSE;
        }
        break;
      }
    }
    profile_block = (attach_profile_data_block *)
                                q_next(attach_prof_q, &(profile_block->link));
  }

  return retval;
}

/*===========================================================================
FUNCTION      DSD_APM_FILL_ATTACH_PROFILE_WITH_TOKEN_CONTENT

DESCRIPTION   This function gets the data for each token and populates the
              structure with the appropriate data that corresponds to the
              token number.

PARAMETERS    token_id -  describes the current token which is
                              being populated into the EFS structure
 
              char *from, *to - start and end of the character array which
                          holds the data to populate the profile structure
 
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean dsd_apm_fill_queue_block_with_token_content
(
  char   *compare_val,
  char   *from,
  char   *to,
  uint16 *queue_block_element_value
)
{
  int16   extracted_val = 0;
  boolean ret_val = FALSE;
  char    *start = from;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( compare_val == NULL ||
       from == NULL ||
       to == NULL ||
       queue_block_element_value == NULL)
  {
    DATA_MSG0(MSG_LEGACY_ERROR,"Invalid Input Params");
    return ret_val;
  }

  /*----------------------------------------------------------------------
   Skip empty character
  ----------------------------------------------------------------------*/
  while (*start == ' ' && start < to )
  {
    start++;
  }


  /*----------------------------------------------------------------------
   Extract the numeric value assigned to token ID and store in the queue
   block
  ----------------------------------------------------------------------*/
  if ((strncasecmp(compare_val, "Attach_Profile_Id", 
                   strlen("Attach_Profile_Id")) == 0)||
      (strncasecmp(compare_val, "Detach_Network_Cause", 
                   strlen("Detach_Network_Cause")) == 0)||
      (strncasecmp(compare_val, "Reject_Network_Cause", 
                   strlen("Reject_Network_Cause")) == 0))
  {
    extracted_val = dsd_apm_atoi(start,to);
    if(extracted_val >= 0)
    {
      *queue_block_element_value = extracted_val;
      ret_val = TRUE;
    }

    DATA_MSG2(MSG_LEGACY_MED,"Valid Value extracted: %d ,after parsing %d", 
               ret_val, extracted_val);
  }

  return ret_val;
}/* dsd_apm_fill_attach_profile_with_token_content() */

/*===========================================================================
FUNCTION      DSD_APM_APPEND_CAUSE_CODES_TO_RULE_STRING_BUF

DESCRIPTION   This is a utility function used to prepare the string that writes 
              cause code information to the EFS File
 
PARAMETERS    data_block_ptr: APM Rule Block 
              cause_code_copy_string: Character string containing cause codes
              cause_type: DETACH/REJECT cause code type

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dsd_apm_append_cause_codes_to_rule_string_buf
(
  char                                   *cause_code_copy_string,
  uint16                                  cause_code_copy_str_length,
  ds_dsd_apm_data_block                  *data_block_ptr,
  uint16                                  cause_type
)
{
  char                                    temp_copy_str_buffer[10];
  ds_dsd_apm_cause_code_info_type        *last_cause_code_block_per_type_p = NULL; 
  ds_dsd_apm_cause_code_info_type        *cause_code_block_ptr = NULL;
  uint16                                  temp_buf_length = 10;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(data_block_ptr != NULL &&
     cause_code_copy_string != NULL)
  {
    cause_code_block_ptr = (ds_dsd_apm_cause_code_info_type *)q_check(
       &data_block_ptr->apn_info.cause_code_info.cause_code_info_q);

    last_cause_code_block_per_type_p = ds_dsd_apm_get_last_cause_block_per_type
                                                  (data_block_ptr,cause_type);

  /*----------------------------------------------------------------------
    We extract the cause codes from Rule block, append the delimiter to it
    depending on whether it is not the last cause code block for a
    particular cause type (REJECT_NETWORK_CAUSE or DETACH_NETWORK_CAUSE)
  ----------------------------------------------------------------------*/
    memset(cause_code_copy_string, '\0', cause_code_copy_str_length);

    while(cause_code_block_ptr != NULL)
    {
      if(cause_code_block_ptr->cause_type == cause_type)
      {
     	memset(temp_copy_str_buffer, '\0', temp_buf_length);

      /*------------------------------------------------------------------
       We dont append the delimiter to the cause code if its the last block
       since Utility function copy_param_to_string takes care of appending
       them at the end
      -------------------------------------------------------------------*/
        if(last_cause_code_block_per_type_p != cause_code_block_ptr)
        {
          snprintf(temp_copy_str_buffer, 
                  temp_buf_length - 1, "%d;", 
                  cause_code_block_ptr->cause_code);
        }
        else
        {
          snprintf(temp_copy_str_buffer, 
                  temp_buf_length - 1, "%d", 
                  cause_code_block_ptr->cause_code);
        }

        strlcat(cause_code_copy_string, 
                (const char *)temp_copy_str_buffer,
                (cause_code_copy_str_length - 1));
      }

      cause_code_block_ptr = (ds_dsd_apm_cause_code_info_type *)q_next
        (&(data_block_ptr->apn_info.cause_code_info.cause_code_info_q),
         &(cause_code_block_ptr->link));
    }
  }
  else
  {
    DATA_MSG2(MSG_LEGACY_ERROR, 
              "Invalid Input Params, APM Rule Block: 0x%x & Cause code str:0x%x",
              data_block_ptr, cause_code_copy_string);
  }
}

/*===========================================================================
FUNCTION      DSD_APM_PARSE_PDN_RULE_LIST_FROM_EFS

DESCRIPTION   This function will read from the efs file all the necessary
              data that will populate the EFS structure.

              EFS File Format

              Param_Name:Param_Val;

              For example -
              APN_String:qcomIMS.com;

PARAMETERS    efs_db - structure that contains info about the EFS file

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean dsd_apm_parse_pdn_rules_from_efs
(
  sys_modem_as_id_e_type     subs_id,
  ds3gcfgmgr_efs_token_type *efs_db,
  q_type                    *attach_prof_q,
  q_type                    *rule_q
)
{
  char  *from, *to; /* ptrs to start and end of a token */
  ds3gcfgmgr_efs_token_parse_status_type  ret_val
                                          = DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS;
  boolean on_param_name = TRUE; /* flag to keep track of the parser state
                                TRUE - parser expects Token Name in the EFS
                                FALSE - parser expects Token Val in the EFS*/
  int8 token_id = -1;
  boolean attach_pdn_rule = TRUE; /*Distinguish attach/on-demand PDN rules*/
  boolean valid_apn_rule = TRUE;
  boolean valid_apn_string = FALSE; /*Tells APN String is valid or not*/
  boolean cause_code_rule = FALSE; /*Tells cause code rules are present or not*/
  boolean result = FALSE;
  ds_dsd_apm_data_block     *apn_info_block = NULL;
  ds_dsd_apm_cause_code_info_type *cause_code_block_p = NULL;
  dsd_apm_apn_name_type     curr_pdn_name;
  uint16                    rule_index = 0;
  boolean                   is_valid = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  curr_pdn_name.apn_string_len = 0;
  
  /* Set the seperator as : */
  efs_db->seperator = ':';

  while (DS3GCFGMGR_EFS_TOKEN_PARSE_EOF
          != (ret_val = ds3gcfgmgr_efs_tokenizer(efs_db, &from, &to )))
  {
     
    /*------------------------------------------------------------------------
      Token being read. from points to the beginning of the token and
      to points to the (end of the token+1).

      e.g: string= "token;" => from points to 't' and to points to ';'

      The tokenizer automatically skips blank lines and comments (lines
      beginning with #, so no need to check for them here).
     
      The additonal check of token_id ensures that NULL APN is accepted, 
      Token_Id: APN_STRING can accept Null parameter values, other tokens 
      cannot
    ------------------------------------------------------------------------*/
    if ((DS3GCFGMGR_EFS_TOKEN_PARSE_EOL == ret_val) ||
        (from == to && (token_id != DS_DSD_APM_APN_STRING)))
    {
      /*----------------------------------------------------------------------
        Skip empty tokens.
      ----------------------------------------------------------------------*/
      if (on_param_name == FALSE && 
          ret_val == DS3GCFGMGR_EFS_TOKEN_PARSE_EOL)
      {
        on_param_name = TRUE;
        efs_db->seperator = ':'; 
      }
      continue;
    }

    else if ( DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS == ret_val )
    {
      /*---------------------------------------------------------------------
      check if we are looking for param name or param value
      ---------------------------------------------------------------------*/
      if ( on_param_name == TRUE )
      {
        if( dsd_apm_pdn_rule_get_token_id(from, to, &token_id) >= 0)
        {
            is_valid = TRUE;
        /*-----------------------------------------------------------------
          set param_name as FALSE This means the next token is a
          param value
          -----------------------------------------------------------------*/
          on_param_name = FALSE;
          /* set the seperator as ; */
          efs_db->seperator = ';';
        }
         DATA_MSG2(MSG_LEGACY_MED,"Token valid: %d , token ID: %d",
                   is_valid, token_id);
      }

      /*---------------------------------------------------------------------
      This means that the token is a param value
      ---------------------------------------------------------------------*/
      else
      {
        switch( token_id )
        {
          case DS_DSD_APM_APN_STRING:
          {
            if ( apn_info_block != NULL )
            {
              q_link(apn_info_block, &(apn_info_block->link));
              q_put(rule_q, &(apn_info_block->link));
              apn_info_block = NULL;
            }

            valid_apn_string = FALSE;
            valid_apn_rule = FALSE;
            rule_index++;

            memset(curr_pdn_name.apn_string,
                   '\0', 
                   DS_DSD_APM_MAX_APN_STRING_LEN);
            if(ds_dsd_apm_attach_prof_status & ATTACH_PROF_CHANGED)
            {
              valid_apn_string = dsd_apm_extract_apn_name_from_profile(
                                                               subs_id,
                                                               attach_prof_q, 
                                                               rule_index, 
                                                               &curr_pdn_name);
            }
            else
            {
              valid_apn_string = dsd_apm_get_apn_string_with_token_content 
                                 (from, to, &curr_pdn_name);
            }
            if ( valid_apn_string == TRUE )
            {
              apn_info_block = (ds_dsd_apm_data_block *)modem_mem_alloc(
                    sizeof(ds_dsd_apm_data_block), MODEM_MEM_CLIENT_DATA);
              if ( apn_info_block == NULL )
              {
                DATA_MSG0(MSG_LEGACY_ERROR,
                          "memory alloc for APM queue block failed");
                result = FALSE;
                break;
              }
              else
              {
                memset(apn_info_block, 0, sizeof(ds_dsd_apm_data_block));
                apn_info_block->apn_info.subs_id = subs_id;
                apn_info_block->apn_info.apn_name_len = 
                  curr_pdn_name.apn_string_len;
                strlcpy(apn_info_block->apn_info.apn_name,
                        curr_pdn_name.apn_string,
                        apn_info_block->apn_info.apn_name_len + 1);
                q_init(&(apn_info_block->apn_info.cause_code_info.
                         cause_code_info_q));
                token_id = -1;
                result = TRUE;
              }
            }
            else
            {
              DATA_MSG0(MSG_LEGACY_ERROR, 
                        "Unable to extract APN String from EFS");
            }
          }
          break;

          case DS_DSD_APM_ATTACH_TYPE_RULES:
          {
            if ( valid_apn_string == TRUE )
            {
              attach_pdn_rule = TRUE;

              dsd_apm_validate_rule_type_with_token_content(from, to,
                                                            &valid_apn_rule);
              apn_info_block->apn_info.AttachPdn.attach_rule_valid = 
                                                              valid_apn_rule;
            }
          }
          break;

          case DS_DSD_APM_ON_DEMAND_TYPE_RULES:
          {
            if ( valid_apn_string == TRUE )
            {
              attach_pdn_rule = FALSE;
              dsd_apm_validate_rule_type_with_token_content(from, to,
                                                            &valid_apn_rule);
              apn_info_block->apn_info.OnDemandPdn.on_demand_rule_valid = 
                                                              valid_apn_rule;
            }
          }
          break;

          case DS_DSD_APM_CAUSE_CODE_RULES:
          {
            if(valid_apn_string == TRUE)
            {
              dsd_apm_validate_rule_type_with_token_content(from, to,
                                                            &cause_code_rule);
              apn_info_block->apn_info.cause_code_info.cause_code_rule_valid = 
                                                               cause_code_rule;
            }
          }
          break;

          case DS_DSD_APM_PCSCF_REQUIRED:
          case DS_DSD_APM_IPADDRESS_REQUIRED:
          case DS_DSD_APM_DNSADDRESS_REQUIRED:
          case DS_DSD_APM_PCSCF_OR_DNSADDRESS_REQUIRED:
          {
            if ( (valid_apn_string == TRUE) && (valid_apn_rule == TRUE) )
            {
              if ( attach_pdn_rule == TRUE )
              {
                if ( apn_info_block != NULL )
                {
                  dsd_apm_fill_attach_rule_with_token_content(
                     token_id,from,to,&(apn_info_block->apn_info));
                }
              }
              else
              {
                if(apn_info_block != NULL)
                {
                   dsd_apm_fill_on_demand_rule_with_token_content(
                     token_id, from, to,&(apn_info_block->apn_info));
                }
              }
            }
            else
            {
              DATA_MSG2(MSG_LEGACY_ERROR, 
                        "PDN String Validity: %d & PDN Rule Validity: %d",
                        valid_apn_string, valid_apn_rule);
            }
          }
          break;

           case DS_DSD_APM_DETACH_NETWORK_CAUSE:
           case DS_DSD_APM_REJECT_NETWORK_CAUSE:
           {
            /*-------------------------------------------------------------
              Only if we a valid APN string and cause code rules are present
              we add a block in cause code Queue
            --------------------------------------------------------------*/
             if((valid_apn_string == TRUE) && (cause_code_rule == TRUE))
             {
               cause_code_block_p =(ds_dsd_apm_cause_code_info_type *) 
                        modem_mem_alloc(sizeof(ds_dsd_apm_cause_code_info_type), 
                        MODEM_MEM_CLIENT_DATA );

               if (cause_code_block_p == NULL )
               {
                 DATA_MSG0(MSG_LEGACY_ERROR,"Allocate memory failed");
                 result = FALSE;
                 break;
               }
               else
               {  
                 memset(cause_code_block_p,
                        0,
                        sizeof(ds_dsd_apm_cause_code_info_type)); 

                 if ((dsd_apm_fill_queue_block_with_token_content( 
                                   dsd_apm_rule_param_name_id[token_id], 
                                   from, 
                                   to, 
                                   &(cause_code_block_p->cause_code))) 
                                   == FALSE )
                 {
                   modem_mem_free(cause_code_block_p, MODEM_MEM_CLIENT_DATA );
                   cause_code_block_p = NULL;
                 }
                 else
                 {
                  /*-----------------------------------------------------------
                    We set the mask according the type of cause code read
                  ----------------------------------------------------------*/
                   cause_code_block_p->cause_type = token_id;
                   if(token_id == DS_DSD_APM_DETACH_NETWORK_CAUSE)
                   {
                     apn_info_block->apn_info.cause_code_info.
                                      config_valid_flag |= CONFIG_DETACH_NORMAL;
                   }
                   else
                   {
                     apn_info_block->apn_info.cause_code_info.
                                      config_valid_flag |= CONFIG_ATTACH_FAIL;
                   }
                   q_link(cause_code_block_p, &(cause_code_block_p->link));
                   q_put(&(apn_info_block->apn_info.cause_code_info.
                           cause_code_info_q), &(cause_code_block_p->link));
                   cause_code_block_p = NULL;
                 }
               }
             }
             else
             {
               DATA_MSG2(MSG_LEGACY_ERROR, 
                         "PDN String Validity: %d & Cause Code Rule Validity: %d",
                         valid_apn_string, cause_code_rule);
            
             }
          }
          break;

          default:
            DATA_MSG1(MSG_LEGACY_ERROR,"Invalid APM rule param %d name", 
                      token_id);
            break;
        }
        
        if ( result == FALSE )
        {
          DATA_MSG0(MSG_LEGACY_ERROR,"Read APM rule EFS file fails");
          break;
        }
      }
    }
    else
    {
      DATA_MSG1(MSG_LEGACY_ERROR,
                "Unexpected return value from Tokenizer: %d",ret_val);
    }
  }

  if ( apn_info_block != NULL )
  {
    q_link(apn_info_block, &(apn_info_block->link));
    q_put(rule_q, &(apn_info_block->link));
  }

  return result;
} /* dsd_apm_parse_pdn_rules_from_efs() */

/*===========================================================================
FUNCTION      DSD_APM_ATTACH_PROFILE_GET_TOKEN_ID

DESCRIPTION   This function extracts the token between 2 end points, 
              start and to

PARAMETERS    char *from - start of the token (param name)
              char *to   - end of the token (param name)

DEPENDENCIES  None

RETURN VALUE  0 - failure
              1 - success

SIDE EFFECTS  None
===========================================================================*/
boolean dsd_apm_match_token_id
(
  char      *from,
  char      *to,
  char      *para_name
)
{
  boolean ret_val = FALSE; /* return value */
  uint8 length;       /* length of the token (param name) */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get the length of the string that is passed */
  if  ( from > to )
  {
    DATA_MSG0(MSG_LEGACY_ERROR,"Invalid Parameters");
    return ret_val;
  }

  /* Assumption that the token length will not be greater than 255 */
  length =(uint8)(to-from);

  if ( (length == strlen(para_name)) &&
       ( 0 == strncasecmp(from, para_name, length)) )
  {
    ret_val = TRUE;
  }

  return ret_val;
} /*dsd_apm_attach_profile_get_token_id()*/

/*===========================================================================
FUNCTION      DSD_APM_PARSE_ATTACH_PROFILES_FROM_EFS

DESCRIPTION   This function read attach profile from EFS and save to passed 
              profile queue. 

              EFS File Format

              Param_Name:Param_Val;

              For example -
              Attach_Profile_ID:1;

PARAMETERS    efs_db - structure that contains info about the EFS file

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean dsd_apm_parse_attach_profiles_from_efs
(
  sys_modem_as_id_e_type     subs_id,
  ds3gcfgmgr_efs_token_type *efs_db,
  q_type                    *profile_q
)
{
  char  *from, *to; /* ptrs to start and end of a token */
  ds3gcfgmgr_efs_token_parse_status_type  ret_val
                                          = DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS;
  boolean param_name = TRUE; /* flag to keep track of the parser state
                                TRUE - parser expects Token Name in the EFS
                                FALSE - parser expects Token Val in the EFS*/
  boolean   result = FALSE;
  attach_profile_data_block         *profile_block = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Set the seperator as : */
  efs_db->seperator = ':';
  while (DS3GCFGMGR_EFS_TOKEN_PARSE_EOF
          != (ret_val = ds3gcfgmgr_efs_tokenizer(efs_db, &from, &to )))
  {
    /*------------------------------------------------------------------------
      Token being read. from points to the beginning of the token and
      to points to the (end of the token+1).

      e.g: string= "token;" => from points to 't' and to points to ';'

      The tokenizer automatically skips blank lines and comments (lines
      beginning with #, so no need to check for them here).
    ------------------------------------------------------------------------*/
    if ((from == to) || (DS3GCFGMGR_EFS_TOKEN_PARSE_EOL == ret_val) )
    {
      /*----------------------------------------------------------------------
        Skip empty tokens.
      ----------------------------------------------------------------------*/
      if (param_name == FALSE)
      {
        param_name = TRUE;
        efs_db->seperator = ':'; 
      }
      continue;
    }

    else if ( DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS == ret_val )
    {
      /*---------------------------------------------------------------------
      check if we are looking for param name or param value
      ---------------------------------------------------------------------*/
      if ( param_name == TRUE )
      {
        if (dsd_apm_match_token_id(from, to, "Attach_Profile_Id") == FALSE)
        {
          DATA_MSG0(MSG_LEGACY_ERROR,
                    "Incorrect param name of attach profiles EFS");
        }
        else
        {
          /*-----------------------------------------------------------------
          set param_name as FALSE This means the next token is a
          param value
          -----------------------------------------------------------------*/
          param_name = FALSE;
          /* set the seperator as ; */
          efs_db->seperator = ';';
        }
      }

      /*---------------------------------------------------------------------
      This means that the token is a param value
      ---------------------------------------------------------------------*/
      else
      {
          profile_block =(attach_profile_data_block*) modem_mem_alloc
                                           ( sizeof(attach_profile_data_block), 
                                             MODEM_MEM_CLIENT_DATA );
          if ( profile_block == NULL )
          {
            DATA_MSG0(MSG_LEGACY_ERROR,"Allocate memory failed");
            break;
          }
          else
          {  
           memset(profile_block, 0, sizeof(attach_profile_data_block)); 
           profile_block->attach_profile.subs_id = subs_id;
          if ((dsd_apm_fill_queue_block_with_token_content("Attach_Profile_Id", 
                                                          from, 
                                                          to, 
                                    &(profile_block->attach_profile.profile))) 
                == FALSE )
            {
             modem_mem_free(profile_block, MODEM_MEM_CLIENT_DATA );
            }
          else
          {
            result = TRUE;
            DATA_MSG1(MSG_LEGACY_MED, 
                      "Successful Profile entry addtion to Queue : %d",
                      profile_block->attach_profile.profile);
            q_link(profile_block, &(profile_block->link));
            q_put(profile_q, &(profile_block->link));
          }
          profile_block = NULL;
        }

        /*-------------------------------------------------------------------
         set param_name as TRUE This means that next token is a param name
        -------------------------------------------------------------------*/
        param_name = TRUE;
        /* Set the seperator as : */
        efs_db->seperator = ':';
      }
    }
    else
    {
      DATA_MSG1(MSG_LEGACY_ERROR, 
                "Unexpected return value from Tokenizer: %d",
                ret_val);
    }
  }
  return result;
}/* dsd_apm_parse_attach_profiles_from_efs() */

/*===========================================================================
                      EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DSD_APM_READ_PDN_RULE_LIST_FROM_EFS

DESCRIPTION   This function will read PDN rules from EFS

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean dsd_apm_read_pdn_rule_list_from_efs
(
  sys_modem_as_id_e_type     subs_id,
  q_type                    *attach_prof_q,
  q_type                    *q
)
{
  ds3gcfgmgr_efs_token_type  efs_db;
  boolean                    result = TRUE;
  char                      *apm_rules_string = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Open EFS file */
  if ( ds3gcfgmgr_efs_file_init_ex(DSD_APM_RULES_FILE, subs_id, &efs_db) == 0 )
  {
    result = dsd_apm_parse_pdn_rules_from_efs(subs_id, &efs_db, 
                                              attach_prof_q, q);
    /* Close EFS file */
    ds3gcfgmgr_efs_file_close(&efs_db);

    /*---------------------------------------------------------------------
      Update the rules in EFS if new attach profiles were read
    ---------------------------------------------------------------------*/
    if(ds_dsd_apm_attach_prof_status & ATTACH_PROF_CHANGED)
    {
      /*---------------------------------------------------------------------
        Create a new string from APM rules queue
      ---------------------------------------------------------------------*/
      apm_rules_string = (char*)modem_mem_alloc(DSD_APM_PROFILE_MAX_BUFF_SIZE,
                                                MODEM_MEM_CLIENT_DATA);
      if(apm_rules_string == NULL)
      {
        /*------------------------------------------------------------------
        This condition can not be recovered
        -------------------------------------------------------------------*/
        DATA_MSG0(MSG_LEGACY_ERROR,"APM couldn't get data_block_ptr memory " );
        result = FALSE;
        return result;
      }

      memset(apm_rules_string,0,DSD_APM_PROFILE_MAX_BUFF_SIZE);
      result = dsd_apm_gen_apm_rule_string(q, 
                                           apm_rules_string,
                                           DSD_APM_PROFILE_MAX_BUFF_SIZE,										   
                                           subs_id);

      if(result == FALSE)
      {
        DATA_MSG0(MSG_LEGACY_ERROR,
                  "Error generating apn rule string. Not updating apn rules");
        result = FALSE;
        return result;
      }

      /*---------------------------------------------------------------------
        Overwrite the existing EFS file with new string
      ---------------------------------------------------------------------*/
      dsd_apm_update_apn_rule_efs_file(subs_id, q, apm_rules_string);
    }

  }
  else
  {
    result = FALSE;
    DATA_MSG0(MSG_LEGACY_ERROR,"Error opening PDN rule EFS file");
  }

  if(apm_rules_string != NULL)
  {
    modem_mem_free(apm_rules_string, MODEM_MEM_CLIENT_DATA);
  }

  return result;
} /* dsd_apm_read_pdn_rule_list_from_efs() */

/*===========================================================================
FUNCTION      DSD_APM_READ_ATTACH_PROFILE_LIST_FROM_EFS

DESCRIPTION   This function will read attach profiles from EFS

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean dsd_apm_read_attach_profile_list_from_efs
(
  sys_modem_as_id_e_type     subs_id,
  q_type                    *q
)
{
  ds3gcfgmgr_efs_token_type  efs_db;
  struct fs_stat             fs_buf;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ds_dsd_apm_attach_prof_status = ATTACH_PROF_READ_FAIL;

  if(q == NULL)
  {
    DATA_MSG0(MSG_LEGACY_ERROR, "Invalid Input Params");
    return ds_dsd_apm_attach_prof_status;
  }

  memset(&fs_buf, 0, sizeof(struct fs_stat));

  if( mcfg_fs_stat(DSD_APM_TMP_ATTACH_PROFILE_FILE,
                   &fs_buf,
                   MCFG_FS_TYPE_EFS,
                   (mcfg_fs_sub_id_e_type)subs_id)
        == MCFG_FS_STATUS_OK )
  {
    DATA_MSG0(MSG_LEGACY_MED, "Temp attach Profile List, exists copying ");

    if( mcfg_fs_stat(DSD_APM_ATTACH_PROFILE_FILE,
                     &fs_buf,
                     MCFG_FS_TYPE_EFS,
                     (mcfg_fs_sub_id_e_type)subs_id)
          == MCFG_FS_STATUS_OK )
    {
      if( mcfg_fs_delete(DSD_APM_ATTACH_PROFILE_FILE,
                         MCFG_FS_TYPE_EFS,
                         (mcfg_fs_sub_id_e_type)subs_id)
            != MCFG_FS_STATUS_OK )
      {
        DATA_MSG0(MSG_LEGACY_ERROR,"Delete original attach pdn list fails");
      }
    }

    if( mcfg_fs_rename(DSD_APM_TMP_ATTACH_PROFILE_FILE,
                       DSD_APM_ATTACH_PROFILE_FILE,
                       MCFG_FS_TYPE_EFS,
                       (mcfg_fs_sub_id_e_type)subs_id)
          == -1 )
    {
      DATA_MSG0(MSG_LEGACY_ERROR, 
                "Rename un-successful, stale contents will be read");
    }
    ds_dsd_apm_attach_prof_status |= ATTACH_PROF_CHANGED;
  }

  /* Open EFS file */
  if( ds3gcfgmgr_efs_file_init_ex(DSD_APM_ATTACH_PROFILE_FILE,
                                  subs_id,
                                  &efs_db)
        == 0 )
  {
    if(dsd_apm_parse_attach_profiles_from_efs(subs_id, &efs_db, q) == TRUE)
    {
      ds_dsd_apm_attach_prof_status |= ATTACH_PROF_READ_SUCCESS;
    }
    /* Close EFS file */
    ds3gcfgmgr_efs_file_close(&efs_db);
  }
  else
  {
    ds_dsd_apm_attach_prof_status = ATTACH_PROF_READ_FAIL;
    DATA_MSG0(MSG_LEGACY_ERROR,"Error opening attach profile file");
  }

  return ds_dsd_apm_attach_prof_status;
}/* dsd_apm_read_attach_profile_list_from_efs() */

/*===========================================================================
FUNCTION      DSD_APM_READ_DEFAULT_ATTACH_PROF_FILE

DESCRIPTION   This function will read attach profiles from the PDP registry

PARAMETERS    subs_id:                     Current active Data subs 
              attach_prof_q:               Queue that contains the attach
                                           profiles

DEPENDENCIES  None

RETURN VALUE  TRUE: If we were able to obtain default attach prof 
              FALSE: Otherwise 

SIDE EFFECTS  Modification of global attach profile queue of APM
===========================================================================*/
boolean dsd_apm_read_default_attach_prof 
(
  sys_modem_as_id_e_type                 subs_id,
  q_type                                *attach_prof_q
)
{
  ds_umts_pdp_profile_status_etype        profile_status = DS_UMTS_PDP_FAIL;
  byte                                    def_sock_prof = 0;
  boolean                                 ret_val = FALSE;
  attach_profile_data_block              *profile_block = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  profile_status = ds_umts_get_pdp_profile_num_for_sock_calls_per_subs(
                        dsumts_subs_mgr_get_subs_id(subs_id),
                        &def_sock_prof);

  if(profile_status == DS_UMTS_PDP_SUCCESS)
  {
    DATA_MSG1_MED ("Default Socket Profile read from PDP registry is %d",
                   def_sock_prof);
    profile_block = (attach_profile_data_block *)modem_mem_alloc (
                     sizeof(attach_profile_data_block), MODEM_MEM_CLIENT_DATA);
    if(profile_block != NULL)
    {
      memset (profile_block, 0, sizeof(attach_profile_data_block));
      profile_block->attach_profile.profile = def_sock_prof;
      profile_block->attach_profile.subs_id = subs_id;

      q_link(profile_block, &(profile_block->link));
      q_put(attach_prof_q, &(profile_block->link));

      ret_val = TRUE;
    }
    else
    {
      DATA_MSG0_ERROR ("Unable to allocate memory, cannot insert default attach "
                       "profile block in Attach Profile Queue");
    }
  }
  else
  {
    DATA_MSG1_ERROR ("PDP Registry read not successful, Return Status: %d",
                     profile_status);
  }
  
  return ret_val;
}
/*===========================================================================
FUNCTION       DSD_APM_COPY_PARAM_TO_STRING

DESCRIPTION    Copies the param name to the string and add : seperator

DEPENDENCIES   NONE

RETURN VALUE   Length of characters copied to the string

SIDE EFFECTS   NONE
===========================================================================*/
uint16 dsd_apm_copy_param_to_string
(
  char                                  * str,
  dsd_apm_rule_param_name_enum_type            param,
  char                                  * param_val,
  uint16                                  param_val_len,
  uint16                                  remaining_buffer_size
)
{
  uint16                                  length=0;
  uint16                                  tmp_len=0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  length = strlen(dsd_apm_rule_param_name_id[param])+param_val_len+4;

  DATA_MSG4(MSG_LEGACY_HIGH,
            "dsd_apm_copy_param_to_string: str:0x%xd, param:%d, "
            "param_val_len:%d , length: %d",
            str,param,param_val_len,length);

  if (length > remaining_buffer_size)
  {
    DATA_MSG0(MSG_LEGACY_ERROR,"dsd_apm_copy_param_to_string: Buffer Overflow"
                               " detected");
    return tmp_len;
  }
  /*---------------------------------------------------------------------
    if PCSCF_REQUIRED or IPADDRESS_REQUIRED, the param_val is a char.
    else, param_val is a string
  ---------------------------------------------------------------------*/
  if((param == DS_DSD_APM_PCSCF_REQUIRED) || 
     (param == DS_DSD_APM_IPADDRESS_REQUIRED)||
     (param == DS_DSD_APM_DNSADDRESS_REQUIRED) ||
     (param == DS_DSD_APM_PCSCF_OR_DNSADDRESS_REQUIRED))
  {
    DATA_MSG_SPRINTF_1(MSG_LEGACY_HIGH,"param_val:%c",*param_val);

    tmp_len=snprintf(str,
                     length,
                     "%s:%c;\n",
                     dsd_apm_rule_param_name_id[param],
                     *param_val);
  }
  else
  {
    DATA_MSG_SPRINTF_1(MSG_LEGACY_HIGH,"param_val:%s",param_val);

    tmp_len=snprintf(str,
                     length,
                     "%s:%s;\n",
                     dsd_apm_rule_param_name_id[param],
                     param_val);
  }
  
  DATA_MSG2(MSG_LEGACY_HIGH,"tmp_len: %d , str:%s",tmp_len,str);

  return tmp_len;
}/* dsd_apm_copy_param_to_string */

/*===========================================================================
FUNCTION       DSD_APM_GEN_APM_RULE_STRING

DESCRIPTION    Creates apm rule string from apm_rule_q

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
boolean dsd_apm_gen_apm_rule_string
(
  q_type                       *rule_q,
  char                         *str,
  uint16                        buffer_size,
  sys_modem_as_id_e_type        subs_id
)
{
  char                          ipaddr_char;
  uint16                        start_pos=0;
  uint16                        length=0;
  ds_dsd_apm_data_block        *apn_data_block_temp = NULL;
  uint16                        remaining_buffer_size = buffer_size;
  char                          cause_code_string[100];
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(rule_q == NULL || buffer_size == 0)
  {
    DATA_MSG0(MSG_LEGACY_ERROR,"queue is empty");
    return FALSE;
  }

  /*---------------------------------------------------------------------
    Loop through the queue and create the string
  ---------------------------------------------------------------------*/
  /* grab first item in the queue */
  apn_data_block_temp = (ds_dsd_apm_data_block*)q_check(rule_q);

  while(apn_data_block_temp != NULL)
  {
    /* process only if subs id matches */
    if (apn_data_block_temp->apn_info.subs_id == subs_id)
    {
      // 1. copy - APN_STRING
      length = dsd_apm_copy_param_to_string(
                 str+start_pos,
                 DS_DSD_APM_APN_STRING,
                 apn_data_block_temp->apn_info.apn_name,
                 apn_data_block_temp->apn_info.apn_name_len,
                 remaining_buffer_size);
     
      if(length == 0)
      {
        return FALSE;
      }

      start_pos+= length;
      remaining_buffer_size -= length;

      // 2. copy - ATTACH_TYPE_RULES
      if(apn_data_block_temp->apn_info.AttachPdn.attach_rule_valid == TRUE)
      {
        length = dsd_apm_copy_param_to_string(str+start_pos,
                                              DS_DSD_APM_ATTACH_TYPE_RULES,
                                              "TRUE",
                                              strlen("TRUE"),
                                              remaining_buffer_size);
      }
      else
      {
        length = dsd_apm_copy_param_to_string(str+start_pos,
                                              DS_DSD_APM_ATTACH_TYPE_RULES,
                                              "FALSE",
                                              strlen("FALSE"),
                                              remaining_buffer_size);
      }
      if(length == 0)
      {
        return FALSE;
      }
      start_pos+= length;
      remaining_buffer_size -= length;

      // 3. copy - PCSCF_REQUIRED
      ipaddr_char = '4';
      if((apn_data_block_temp->apn_info.AttachPdn.config_rule.config_valid_flag|
         CONFIG_PCSCF) != 0)
      {
        ipaddr_char 
          = ds_dsd_apm_translate_ipreq_to_char(
             apn_data_block_temp->apn_info.AttachPdn.config_rule.pcscf_address);
      }
      length = dsd_apm_copy_param_to_string(str+start_pos,
                                            DS_DSD_APM_PCSCF_REQUIRED,
                                            &ipaddr_char,
                                            1,
                                            remaining_buffer_size);
      if(length == 0)
      {
        return FALSE;
      }
      start_pos+= length;
      remaining_buffer_size -= length;

      // 4. copy - IPADDRESS_REQUIRED
      ipaddr_char = '4';
      if((apn_data_block_temp->apn_info.AttachPdn.config_rule.config_valid_flag|
         CONFIG_IP_ADDRESS) != 0)
      {
        ipaddr_char 
          = ds_dsd_apm_translate_ipreq_to_char(
             apn_data_block_temp->apn_info.AttachPdn.config_rule.ip_address);
      }
      length = dsd_apm_copy_param_to_string(str+start_pos,
                                            DS_DSD_APM_IPADDRESS_REQUIRED,
                                            &ipaddr_char,
                                            1, 
                                            remaining_buffer_size);
      if(length == 0)
      {
        return FALSE;
      }
      start_pos+= length;
      remaining_buffer_size -= length;

      // 5. copy - DNSADDRESS_REQUIRED
      ipaddr_char = '4';
      if((apn_data_block_temp->apn_info.AttachPdn.config_rule.config_valid_flag|
         CONFIG_DNS_ADDRESS) != 0)
      {
        ipaddr_char 
          = ds_dsd_apm_translate_ipreq_to_char(
             apn_data_block_temp->apn_info.AttachPdn.config_rule.dns_address);
      }
      length = dsd_apm_copy_param_to_string(str+start_pos,
                                            DS_DSD_APM_DNSADDRESS_REQUIRED,
                                            &ipaddr_char,
                                            1, 
                                            remaining_buffer_size);
      if(length == 0)
      {
        return FALSE;
      }
      start_pos+= length;
      remaining_buffer_size -= length;


      // 6. copy - PCSCF_OR_DNSADDRESS_REQUIRED
      ipaddr_char = '4';
      if((apn_data_block_temp->apn_info.AttachPdn.config_rule.config_valid_flag|
         CONFIG_PCSCF_DNS_ADDRESS) != 0)
      {
        ipaddr_char 
          = ds_dsd_apm_translate_ipreq_to_char(
             apn_data_block_temp->apn_info.AttachPdn.config_rule.pcscf_or_dns_address);
      }
      length = dsd_apm_copy_param_to_string(str+start_pos,
                                            DS_DSD_APM_PCSCF_OR_DNSADDRESS_REQUIRED,
                                            &ipaddr_char,
                                            1, 
                                            remaining_buffer_size);
      if(length == 0)
      {
        return FALSE;
      }
      start_pos+= length;
      remaining_buffer_size -= length;

      // 7. copy - ON_DEMAND_TYPE_RULES
      if(apn_data_block_temp->apn_info.OnDemandPdn.on_demand_rule_valid == TRUE)
      {
        length = dsd_apm_copy_param_to_string(str+start_pos,
                                              DS_DSD_APM_ON_DEMAND_TYPE_RULES,
                                              "TRUE",
                                              strlen("TRUE"),
                                              remaining_buffer_size);
      }
      else
      {
        length = dsd_apm_copy_param_to_string(str+start_pos,
                                              DS_DSD_APM_ON_DEMAND_TYPE_RULES,
                                              "FALSE",
                                              strlen("FALSE"),
                                              remaining_buffer_size);
      }
      if(length == 0)
      {
        return FALSE;
      }
      start_pos+= length;
      remaining_buffer_size -= length;

      // 8. copy  - PCSCF_REQUIRED
      ipaddr_char = '4';
      if((apn_data_block_temp->apn_info.OnDemandPdn.config_rule.config_valid_flag|
         CONFIG_PCSCF) != 0)
      {
        ipaddr_char 
          = ds_dsd_apm_translate_ipreq_to_char(
             apn_data_block_temp->apn_info.OnDemandPdn.config_rule.pcscf_address);
      }
      length = dsd_apm_copy_param_to_string(str+start_pos,
                                            DS_DSD_APM_PCSCF_REQUIRED,
                                            &ipaddr_char,
                                            1, 
                                            remaining_buffer_size);
      if(length == 0)
      {
        return FALSE;
      }
      start_pos+= length;
      remaining_buffer_size -= length;

      // 9. copy - IPADDRESS_REQUIRED
      ipaddr_char = '4';
      if((apn_data_block_temp->apn_info.OnDemandPdn.config_rule.config_valid_flag|
         CONFIG_IP_ADDRESS) != 0)
      {
        ipaddr_char 
          = ds_dsd_apm_translate_ipreq_to_char(
             apn_data_block_temp->apn_info.OnDemandPdn.config_rule.ip_address);
      }
      length = dsd_apm_copy_param_to_string(str+start_pos,
                                            DS_DSD_APM_IPADDRESS_REQUIRED,
                                            &ipaddr_char,
                                            1,
                                            remaining_buffer_size);
      if(length == 0)
      {
        return FALSE;
      }
      start_pos+= length;
      remaining_buffer_size -= length;

      // 10. copy - DNSADDRESS_REQUIRED
      ipaddr_char = '4';
      if((apn_data_block_temp->apn_info.OnDemandPdn.config_rule.config_valid_flag|
         CONFIG_DNS_ADDRESS) != 0)
      {
        ipaddr_char 
          = ds_dsd_apm_translate_ipreq_to_char(
             apn_data_block_temp->apn_info.OnDemandPdn.config_rule.dns_address);
      }
      length = dsd_apm_copy_param_to_string(str+start_pos,
                                            DS_DSD_APM_DNSADDRESS_REQUIRED,
                                            &ipaddr_char,
                                            1, 
                                            remaining_buffer_size);
      if(length == 0)
      {
        return FALSE;
      }
      start_pos+= length;
      remaining_buffer_size -= length;


      // 11. copy - PCSCF_OR_DNSADDRESS_REQUIRED
      ipaddr_char = '4';
      if((apn_data_block_temp->apn_info.OnDemandPdn.config_rule.config_valid_flag|
         CONFIG_PCSCF_DNS_ADDRESS) != 0)
      {
        ipaddr_char 
          = ds_dsd_apm_translate_ipreq_to_char(
             apn_data_block_temp->apn_info.OnDemandPdn.config_rule.pcscf_or_dns_address);
      }
      length = dsd_apm_copy_param_to_string(str+start_pos,
                                            DS_DSD_APM_PCSCF_OR_DNSADDRESS_REQUIRED,
                                            &ipaddr_char,
                                            1,
                                            remaining_buffer_size);
      if(length == 0)
      {
        return FALSE;
      }
      start_pos+= length;
      remaining_buffer_size -= length;
      // 12. copy - Cause Code Rules
      if(apn_data_block_temp->apn_info.cause_code_info.cause_code_rule_valid)
      {
        length = dsd_apm_copy_param_to_string(str+start_pos,
                                              DS_DSD_APM_CAUSE_CODE_RULES,
                                              "TRUE",
                                              strlen("TRUE"),
                                              remaining_buffer_size);

        if(length == 0)
        {
          return FALSE;
        }
        start_pos+= length;
        remaining_buffer_size -= length;

        // 13. copy - Detach Network Cause 
        if(apn_data_block_temp->apn_info.cause_code_info.config_valid_flag &
           CONFIG_DETACH_NORMAL)
        {
          memset(cause_code_string, '\0', sizeof(char)*100);
          dsd_apm_append_cause_codes_to_rule_string_buf(cause_code_string,
                                               100,
                                               apn_data_block_temp,
                                               DS_DSD_APM_DETACH_NETWORK_CAUSE);

          length = dsd_apm_copy_param_to_string(str+start_pos,
                                                DS_DSD_APM_DETACH_NETWORK_CAUSE,
                                                cause_code_string,
                                                strlen(cause_code_string),
                                                remaining_buffer_size);

          if(length == 0)
          {
            return FALSE;
          }
          start_pos+= length;
          remaining_buffer_size -= length;
        }

        // 14. copy - Reject Network Cause 
        if(apn_data_block_temp->apn_info.cause_code_info.config_valid_flag &
           CONFIG_ATTACH_FAIL)
        {
          memset(cause_code_string, '\0', sizeof(char)*100);
          dsd_apm_append_cause_codes_to_rule_string_buf(cause_code_string,
                                               100,
                                               apn_data_block_temp,
                                               DS_DSD_APM_REJECT_NETWORK_CAUSE);

          length = dsd_apm_copy_param_to_string(str+start_pos,
                                                DS_DSD_APM_REJECT_NETWORK_CAUSE,
                                                cause_code_string,
                                                strlen(cause_code_string),
                                                remaining_buffer_size);

          if(length == 0)
          {
            return FALSE;
          }
          start_pos+= length;
          remaining_buffer_size -= length;
        }
      }

      DATA_MSG1(MSG_LEGACY_HIGH,"total len = %d",start_pos);
    }

    /* get next element in the queue */
    apn_data_block_temp = q_next(rule_q,&(apn_data_block_temp->link));
  }// end while

  return TRUE;
}/* dsd_apm_gen_apm_rule_string() */

/*===========================================================================
FUNCTION      DSD_APM_UPDATE_APN_RULE_EFS_FILE

DESCRIPTION   This function will overwrite the EFS file with string passed

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean dsd_apm_update_apn_rule_efs_file
(
  sys_modem_as_id_e_type     subs_id,
  q_type                    *q, 
  char*                     writebuff
)
{
  int     fd = -1;
  int         mcfg_ret = -1;
  uint16  writebufflen = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  DATA_MSG1(MSG_LEGACY_MED,"dsd_apm_update_apn_rule_efs_file for sub %d",
            subs_id);

  //open the APM rules file and truncate the file
  fd = mcfg_fopen(DSD_APM_RULES_FILE,
                  MCFG_FS_O_WRONLY|MCFG_FS_O_TRUNC,
                  MCFG_FS_ALLPERMS,
                  MCFG_FS_TYPE_EFS,
                  (mcfg_fs_sub_id_e_type)subs_id);

  writebufflen = strlen(writebuff);
  if (writebufflen)
  {
    DATA_MSG1(MSG_LEGACY_MED,"Writing %d bytes to EFS", writebufflen);
    mcfg_ret = mcfg_fwrite(fd, writebuff, writebufflen, MCFG_FS_TYPE_EFS);
    if((-1 == mcfg_ret) || (mcfg_ret != writebufflen))
    {
      DATA_MSG1(MSG_LEGACY_ERROR,"EFS Write failed. Efs_ret:%d", mcfg_ret);
      return FALSE;
    }
  }

  if( -1 == mcfg_fclose(fd, MCFG_FS_TYPE_EFS) )
  {
    DATA_MSG0(MSG_LEGACY_ERROR,"EFS close failed");
    return FALSE;
  }
  
  return TRUE;
}/* dsd_apm_update_apn_rule_efs_file() */

/*===========================================================================
FUNCTION      DS_DSD_APM_TRANSLATE_IPREQ_TO_CHAR

DESCRIPTION   This is a utility to translate IP Required enum type to char

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
char  ds_dsd_apm_translate_ipreq_to_char
(
  ipRequiredType ip_req
)
{
  char ch;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch (ip_req)
  {
    case ipv4_mandatory_and_ipv6_donotcare:
      ch = '1';
      break;
    case ipv6_mandatory_and_ipv4_donotcare:
      ch = '2';
      break;
    case ipv4_mandatory_and_ipv6_mandatory:
      ch = '3';
      break;
    case ipv4_dontcare_and_ipv6_donotcare:
      ch = '4';
      break;
    case ipv4_mandatory_or_ipv6_mandatory:
      ch = '5';
      break;
    default:
      ch = '0';
      break;
  }
  return ch;
}/* ds_dsd_apm_translate_ipreq_to_char() */

/*===========================================================================
FUNCTION      DS_DSD_APM_EFS_DEL_ATTACH_INFO

DESCRIPTION   This function clears all the attach info files from EFS

DEPENDENCIES  None.

RETURN VALUE  void
===========================================================================*/
void ds_dsd_apm_efs_del_attach_info
(
  sys_modem_as_id_e_type  subs_id
)
{
  struct fs_stat  fs_buf;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( mcfg_fs_stat(DSD_APM_TMP_ATTACH_PROFILE_FILE,
                   &fs_buf,
                   MCFG_FS_TYPE_EFS,
                   (mcfg_fs_sub_id_e_type)subs_id) == MCFG_FS_STATUS_OK)
  {
    (void)mcfg_fs_delete(DSD_APM_TMP_ATTACH_PROFILE_FILE,
                         MCFG_FS_TYPE_EFS,
                         (mcfg_fs_sub_id_e_type)subs_id);
  }

  if( mcfg_fs_stat(DSD_APM_ATTACH_PROFILE_FILE,
                   &fs_buf,
                   MCFG_FS_TYPE_EFS,
                   (mcfg_fs_sub_id_e_type)subs_id) == MCFG_FS_STATUS_OK)
  {
    (void)mcfg_fs_delete(DSD_APM_ATTACH_PROFILE_FILE,
                         MCFG_FS_TYPE_EFS,
                         (mcfg_fs_sub_id_e_type)subs_id);
  }

  if( mcfg_fs_stat(DSD_APM_RULES_FILE,
                   &fs_buf,
                   MCFG_FS_TYPE_EFS,
                   (mcfg_fs_sub_id_e_type)subs_id) == MCFG_FS_STATUS_OK)
  {
    (void)mcfg_fs_delete(DSD_APM_RULES_FILE,
                         MCFG_FS_TYPE_EFS,
                        (mcfg_fs_sub_id_e_type)subs_id);
  }

}/* ds_dsd_apm_efs_del_attach_info() */

/*===========================================================================
FUNCTION      DS_DSD_APM_EFS_OVERWRITE_ATTACH_PDN_LIST

DESCRIPTION   This function overwrite the attach PDN list in EFS with input 
              attach PDN profile ID array 

DEPENDENCIES  None.

RETURN VALUE  TRUE: Operation succeeds 
              FALSE: Operation fails 

===========================================================================*/
boolean ds_dsd_apm_efs_overwrite_attach_pdn_list
(
  sys_modem_as_id_e_type     subs_id,
  uint16                    *profile_id_arr,
  uint8                      num_of_pdn
)
{
  int                        fd = -1;
  uint8                      i = 0;
  char                       temp_buf[100] = {0};
  char                       prefix[] = "Attach_Profile_ID";
  int         mcfg_ret = -1;
  int32                      dir_result = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( profile_id_arr == NULL                              ||
      num_of_pdn == 0                                     ||
      num_of_pdn > DS_SYS_LTE_ATTACH_PDN_PROFILE_LIST_MAX )
  {
    DATA_MSG0(MSG_LEGACY_ERROR, "Invalid Input arguments");
    return FALSE;
  }

  DATA_MSG1(MSG_LEGACY_MED, 
            "ds_dsd_apm_efs_overwrite_attach_pdn_list on sub %d", subs_id);

  dir_result = ds_path_is_directory(DSD_APM_ROOT_DIR);
  if( 0 != dir_result )
  {
    /* Directory doesn't exist yet */
    DATA_MSG1(MSG_LEGACY_HIGH,
              "data directory doesn't exist, creat one dir_result:%d",
              dir_result);
    if( 0 != mcfg_mkdir( DSD_APM_ROOT_DIR,
                         S_IREAD|S_IWRITE|S_IEXEC,
                         MCFG_FS_TYPE_EFS) )
    {
      DATA_MSG1(MSG_LEGACY_ERROR,"efs_mkdir Unable to create dir Err: %d",
                mcfg_fs_errno(MCFG_FS_TYPE_EFS));
      return FALSE;
    }
  }

  /* Create a tmp efs file*/
  fd = mcfg_fopen(DSD_APM_TMP_ATTACH_PROFILE_FILE,
                  MCFG_FS_O_CREAT|MCFG_FS_O_TRUNC|MCFG_FS_O_WRONLY,
                  MCFG_FS_DEFFILEMODE,
                  MCFG_FS_TYPE_EFS,
                  (mcfg_fs_sub_id_e_type)subs_id);
  if (-1 == fd)
  {
    DATA_MSG1(MSG_LEGACY_ERROR,"create tmp attach pdn list Err: %d",
              mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    return FALSE;
  }
  for (i=0; i<num_of_pdn; i++)
  {
    memset(temp_buf, 0, sizeof(temp_buf));
	snprintf(
             temp_buf,
             sizeof(temp_buf) - 1,
             "%s:%d;\r\n",
             prefix,profile_id_arr[i]
            );
    mcfg_ret = mcfg_fwrite(fd, temp_buf, strlen(temp_buf), MCFG_FS_TYPE_EFS);
    if (-1 == mcfg_ret || mcfg_ret != strlen(temp_buf))
    {
      DATA_MSG0(MSG_LEGACY_ERROR,"efs write attach pdn list fails");
      return FALSE;
    }
    DATA_MSG1(MSG_LEGACY_MED,"efs write attach pdn list, ID:%d", 
              profile_id_arr[i]);   
  }

  if( -1 == mcfg_fclose(fd, MCFG_FS_TYPE_EFS) )
  {
    DATA_MSG0(MSG_LEGACY_ERROR,"close attach pdn list fails");
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================
FUNCTION      DSD_APM_GET_PROFILE_PARAM_INFO

DESCRIPTION   This function gets the profile param info from profile cache.

DEPENDENCIES  None

RETURN VALUE  TRUE: If read is successful 
              FALSE: Otherwise 

SIDE EFFECTS  None
===========================================================================*/
boolean dsd_apm_get_profile_param_info
(
  uint16                            profile_id,
  ds_profile_identifier_type        profile_param_id,
  ds_profile_info_type       *profile_param_info_ptr,
  sys_modem_as_id_e_type      subs_id
)
{
  ds_profile_status_etype           result;
  ds_profile_hndl_type              profile_handle = NULL;
  boolean                           ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( profile_param_info_ptr == NULL )
  {
    DATA_MSG0(MSG_LEGACY_ERROR,"NULL profile_param_info ptr");
    return FALSE;
  }

  do
  {
    result = ds_profile_begin_transaction_per_sub(
                                           DS_PROFILE_TRN_READ,
                                           DS_PROFILE_TECH_3GPP,
                                           (ds_profile_num_type)profile_id,
                                           ds3gsubsmgr_subs_id_cm_to_dsprofile(subs_id),
                                           &profile_handle );

    if( DS_PROFILE_REG_RESULT_SUCCESS != result ||
        profile_handle == NULL )
    {
      DATA_MSG2(MSG_LEGACY_ERROR,"Begin transaction failed %d %x ",
                result,profile_handle);
      break;
    }

    result = ds_profile_get_param( profile_handle,
                                   profile_param_id,
                                   profile_param_info_ptr);

    if( DS_PROFILE_REG_RESULT_SUCCESS != result )
    {
      DATA_MSG0(MSG_LEGACY_ERROR,"Read profile failed");
      break;
    }

    ret_val = TRUE;
  }while (0);

  if( NULL != profile_handle )
  {
    result = ds_profile_end_transaction (profile_handle,
                                         DS_PROFILE_ACTION_CANCEL);
  }

  return ret_val;
}/* dsd_apm_get_profile_param_info() */

#endif /* FEATURE_DATA_LTE */
