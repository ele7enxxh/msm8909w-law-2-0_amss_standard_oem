/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               3 G   D A T A   S E R V I C E S   M A N A G E R

           P R O F I L E   V A L I D A T O R   F U N C T I O N S

GENERAL DESCRIPTION
  This file contains functions for validating the parameters of the attach
  APN.This header file is intended to be used internal to the 3GPP module.
 
EXTERNALIZED FUNCTIONS
  

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2014-2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/src/ds3gprofile_validator.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/05/2014 ms      introduce this module to validate apn name for 3gpp spec

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "ds3gprofile_validator.h"
#include "data_msg.h"
#include "queue.h"
#include "ds_3gppi_utils.h"
#include "ds_eps_pdn_context.h"
#include "modem_mem.h"
#include <stringl/stringl.h>

#define LABEL_DELIMITOR 0x2e

/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/
static q_type ds3g_profile_validator_forbidden_apn_q[DS3GSUBSMGR_SUBS_ID_COUNT] = 
                                                                     {{{NULL}}};

/*===========================================================================

                     FUNCTION DELCARATION

===========================================================================*/


/*===========================================================================
FUNCTION DS3G_GET_NETWORK_IDENTIFER_FROM_APN_NAME

DESCRIPTION
  This function is used to get network identifier part from apn name stored in the profile. This
  function leaves out the operator identifier. It picks up only the labels
  that reflect the network identifier.

PARAMETERS
  IN
  input_apn - Pointer to the APN in the profile. 
  input_apn_length - length of the apn string

  OUT
  network_identifier - Pointer to the byte array in which the network identifier of APN is returned

DEPENDENCIES
  None

RETURN VALUE
  uint8 - length of the network identifier
 
SIDE EFFECTS
  None
===========================================================================*/
uint8 ds3g_get_network_identifer_from_apn_name
(
  char  *input_apn,
  byte   input_apn_length,
  char  *network_identifier
)
{
  uint32  index = 0;
  boolean has_operator_identifier_part = FALSE;
  boolean has_fqdn_operator_identifier_part = FALSE;
  byte apn_length = input_apn_length;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "APN Name passed with length: %d",input_apn_length);

  if (input_apn == NULL) 
  {
     DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "input_apn passed is null.");
     return 0;
  }

  apn_length = strnlen(input_apn, input_apn_length) ; 
  if (apn_length < input_apn_length) 
  {
     input_apn_length = apn_length;
  }

  if (input_apn_length == 0 || input_apn[0] == 0) 
  {
     DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "apn local_apn_length is 0.");
     return 0;
  }


  if(network_identifier == NULL)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,"network identifier buffer is null.");
    return 0;
  }

  /*------------------------------------------------------------------------
    Determine if encoded_apn has operator identifier part. A valid operating
    would have the 3 labels in the following format 
    "mnc<MNC>.mcc<MCC>.gprs" 

   For example resolved apn name is (in byte format)
   {7,'a','p','n','n','a','m','e',6,'m','n','c','0','0','1',6,'m','c','c','0','1','1',4,'g','p','r','s'}
   "mnc001.mcc011.gprs" would be operator identifier (total 19 bytes)
   "apnname" would be network identifier
   
   The APN can also be in FQDN (Fully Qualified Domain Name) format. The
   network operator would be
   "apn.epc.mnc<MNC>.mcc<MCC>.3gppnetwork.org"
   
   For example resolved apn name is (in byte format)
   {3,'i','m','s',3,'a','p','n',3,'e','p','c',6,'m','n','c','0','0','1',6,'m','c','c','0','0','1',11,'3','g','p','p','n','e','t','w','o','r','k',3,'o','r','g'}
   "ims.apn.epc.mnc001.mcc001.3gppnetwork.org" would be the operator identifier (total 42 bytes)
   "ims" would be the network identifier
  
    For references check:

    Spec: 123 003 V 12.4.1 (2014-10)
    9 Definition of Access Point Name
    19.4.2.2 Access Point Name FQDN (APN-FQDN)

  ------------------------------------------------------------------------*/

  /*------------------------------------------------------------------------
    Traverse the encoded APN to determine the number of labels in the string
  ------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    check for mnc<MNC>.mcc<MCC>.gprs format
   --------------------------------------------------------------------------*/
  if(input_apn_length > DS3G_PROFILE_OPERATOR_IDENTIFIER_TOTAL_LENGTH ) 
  {
    do
    {
      has_operator_identifier_part = TRUE;
      index = input_apn_length - DS3G_PROFILE_OPERATOR_IDENTIFIER_TOTAL_LENGTH;

      if(strncasecmp(input_apn+index,".mnc",DS3G_PROFILE_MCC_MNC_STR_LENGTH_WITH_DELIM) != 0)
      {
        has_operator_identifier_part = FALSE;
        break;
      }

      index += DS3G_PROFILE_MCC_MNC_LENGTH_WITH_DELIM;

      if(strncasecmp(input_apn+index,".mcc",DS3G_PROFILE_MCC_MNC_STR_LENGTH_WITH_DELIM) != 0)
      {
        has_operator_identifier_part = FALSE;
        break;
      }

      index += DS3G_PROFILE_MCC_MNC_LENGTH_WITH_DELIM;

      if(strncasecmp(input_apn+index,".gprs",DS3G_PROFILE_GPRS_STR_LENGTH_WITH_DELIM) != 0)
      {
        has_operator_identifier_part = FALSE;
        break;
      }
    }while (0); 
  }

  /*-------------------------------------------------------------------------
    check for FQDN format apn.epc.mnc<MNC>.mcc<MCC>.3gppnetwork.org.
    Skip check if format is already valid from non-FQDN check
   --------------------------------------------------------------------------*/
  if ((input_apn_length > DS3G_PROFILE_FQDN_OPERATOR_IDENTIFIER_TOTAL_LENGTH) && 
           (has_operator_identifier_part == FALSE))
  {
    do
    {
      has_fqdn_operator_identifier_part = TRUE;
      index = input_apn_length - DS3G_PROFILE_FQDN_OPERATOR_IDENTIFIER_TOTAL_LENGTH;

      if(strncasecmp(input_apn+index,".mnc",DS3G_PROFILE_MCC_MNC_STR_LENGTH_WITH_DELIM) != 0)
      {
        has_fqdn_operator_identifier_part = FALSE;
        break;
      }

      index += DS3G_PROFILE_MCC_MNC_LENGTH_WITH_DELIM;

      if(strncasecmp(input_apn+index,".mcc",DS3G_PROFILE_MCC_MNC_STR_LENGTH_WITH_DELIM) != 0)
      {
        has_fqdn_operator_identifier_part = FALSE;
        break;
      }

      index += DS3G_PROFILE_MCC_MNC_LENGTH_WITH_DELIM;

      if(strncasecmp(input_apn+index,".3gppnetwork",DS3G_PROFILE_3GPPNETWORK_STR_LENGTH_WITH_DELIM) != 0)
      {
        has_fqdn_operator_identifier_part = FALSE;
        break;
      }

      index += DS3G_PROFILE_3GPPNETWORK_STR_LENGTH_WITH_DELIM;

      if(strncasecmp(input_apn+index,".org",DS3G_PROFILE_ORG_STR_LENGTH_WITH_DELIM) != 0)
      {
        has_fqdn_operator_identifier_part = FALSE;
        break;
      }
    }while (0);
  }

  if (has_operator_identifier_part) 
  {
    input_apn_length -= DS3G_PROFILE_OPERATOR_IDENTIFIER_TOTAL_LENGTH;
  }
  else if (has_fqdn_operator_identifier_part)
  {
    input_apn_length -= DS3G_PROFILE_FQDN_OPERATOR_IDENTIFIER_TOTAL_LENGTH;
    if((input_apn_length > DS3G_PROFILE_APN_EPC_STR_LENGTH_WITH_DELIM) &&
        (strncasecmp((const char*)(input_apn+(input_apn_length-DS3G_PROFILE_APN_EPC_STR_LENGTH_WITH_DELIM)),
              ".apn.epc",DS3G_PROFILE_APN_EPC_STR_LENGTH_WITH_DELIM) == 0))
    {
      input_apn_length -= DS3G_PROFILE_APN_EPC_STR_LENGTH_WITH_DELIM;
    }
  }

  if (input_apn_length <= DS3G_PROFILE_NETWORK_IDENTIFIER_MAX_LENGTH) 
  {
    memscpy(network_identifier, input_apn_length,
                 input_apn,
                 input_apn_length);
    network_identifier[input_apn_length] = 0x0;
  }
  
  return input_apn_length;
} /* ds3g_get_network_identifer_from_apn_name */


/*===========================================================================
FUNCTION DS3G_PROFILE_APN_IS_NETWORK_IDENTIFIER_VALID

DESCRIPTION
  This functions checks if network identifier is fully compliant with 3gpp spec.

PARAMETERS
  IN
  apn_ni - Pointer to the network identifier in the profile. 
  apn_ni_len - length of the network identifier string


DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE if network indentifier is valid
            FALSE  otherwise
   
SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_profile_apn_is_network_identifier_valid
(
  byte   *apn_ni,
  uint8   apn_ni_len
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   if (apn_ni_len == 0)
   {
      return TRUE;
   }

   if (apn_ni_len > DS3G_PROFILE_NETWORK_IDENTIFIER_MAX_LENGTH) 
   {
     DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                     "Network Indentifier length:%d > MAX_LENGTH",
                     apn_ni_len);
     return FALSE;
   }

   if (apn_ni == NULL)
   {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,"Network identifier is NULL'.gprs'");
      return FALSE;
   }

   if(apn_ni_len >= DS3G_PROFILE_GPRS_STR_LENGTH_WITH_DELIM && 
       strncasecmp((char*)apn_ni + apn_ni_len-DS3G_PROFILE_GPRS_STR_LENGTH_WITH_DELIM ,
                   ".gprs",DS3G_PROFILE_GPRS_STR_LENGTH_WITH_DELIM) == 0)
    {
      return FALSE;
    }
    else if(apn_ni_len >= DS3G_PROFILE_RAC_LAC_RNC_STR_LENGTH && 
            (strncasecmp((char*)apn_ni,"rac",DS3G_PROFILE_RAC_LAC_RNC_STR_LENGTH) == 0))
    {
      return FALSE;
    }
    else if(apn_ni_len >= DS3G_PROFILE_RAC_LAC_RNC_STR_LENGTH &&
            (strncasecmp((char*)apn_ni,"lac",DS3G_PROFILE_RAC_LAC_RNC_STR_LENGTH) == 0))
    {
      return FALSE;
    }
    else if(apn_ni_len >= DS3G_PROFILE_RAC_LAC_RNC_STR_LENGTH &&
            (strncasecmp((char*)apn_ni,"rnc",DS3G_PROFILE_RAC_LAC_RNC_STR_LENGTH) == 0))
    {
      return FALSE;
    }
    else if(apn_ni_len >= DS3G_PROFILE_SGSN_STR_LENGTH &&
            (strncasecmp((char*)apn_ni,"sgsn",DS3G_PROFILE_SGSN_STR_LENGTH) == 0))
    {
      return FALSE;
    }

    return TRUE;
} /*ds3g_profile_apn_is_network_identifier_valid*/

/*===========================================================================
FUNCTION DS3G_PROFILE_VALIDATE_APN_NAME

DESCRIPTION
  The function validates apn name according to 3gpp spec
    
PARAMETERS: 
  *apn_name  --  apn name char pointer
  *info     --  apn name lenght
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  TRUE - if apn name is valid
  FALSE  - otherwise
  
SIDE EFFECTS 
  None 
===========================================================================*/
boolean ds3g_profile_validate_apn_name
(
  char   *apn_name,
  uint8   apn_name_len
)
{
  uint8 network_ident_length = 0;
  byte apn_network_identifier[DS3G_PROFILE_NETWORK_IDENTIFIER_MAX_LENGTH+1];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(apn_name == NULL)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,"apn_name is NULL");
    return TRUE;
  }

  //Validate operator identifier and extract the network identifier here
  network_ident_length = ds3g_get_network_identifer_from_apn_name(
     apn_name, apn_name_len, (char*)apn_network_identifier);

  if (!ds3g_profile_apn_is_network_identifier_valid(apn_network_identifier,
                                                      network_ident_length )) 
  {
     DATA_3GMGR_MSG0(MSG_LEGACY_LOW,"Network Indentifier is not valid");
     return FALSE;
  }

  return TRUE;
} /*ds3g_profile_validate_apn_name*/

/*===========================================================================
FUNCTION DS3G_DECODE_APN_NAME

DESCRIPTION
  This function is used to decode the APN name returned by the network. This
  function leaves out the operator identified. It picks up only the labels
  that reflect the network identifier.

PARAMETERS
  IN
  encoded_apn - Pointer to the APN returned by the network. This is encoded
  encoded_apn_length - length of the encoded string

  OUT
  decoded_apn - Pointer to the byte array in which the decoded APN is returned

DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE if decoding is successful
            FALSE if there is any error
   
SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_decode_apn_name
(
  byte    *encoded_apn,
  uint32   encoded_apn_length,
  byte    *decoded_apn
)
{
  uint32  label_count = 0;
  uint32  label_length = 0;
  uint32  index = 0;
  boolean ret_val = TRUE;
  uint32  apn_string_offset = 0;
  boolean has_operator_identifier_part = FALSE;
  boolean has_fqdn_operator_identifier_part = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_3GMGR_MSG1(MSG_LEGACY_LOW,"Encoded APN length: %d",encoded_apn_length);
  DATA_3GMGR_MSG_SPRINTF_1(MSG_LEGACY_LOW,"Encoded APN name: %s",encoded_apn);

  /*------------------------------------------------------------------------
    Determine if encoded_apn has operator identifier part. A valid operating
    would have the 3 labels in the following format 
    "mnc<MNC>.mcc<MCC>.gprs" 

   For example resolved apn name is (in byte format)
   {7,'a','p','n','n','a','m','e',6,'m','n','c','0','0','1',6,'m','c','c','0','1','1',4,'g','p','r','s'}
   "mnc001.mcc011.gprs" would be operator identifier (total 19 bytes)
   "apnname" would be network identifier
   
   The APN can also be in FQDN (Fully Qualified Domain Name) format. The
   network operator would be
   "apn.epc.mnc<MNC>.mcc<MCC>.3gppnetwork.org"
   
   For example resolved apn name is (in byte format)
   {3,'i','m','s',3,'a','p','n',3,'e','p','c',6,'m','n','c','0','0','1',6,'m','c','c','0','0','1',11,'3','g','p','p','n','e','t','w','o','r','k',3,'o','r','g'}
   "apn.epc.mnc001.mcc001.3gppnetwork.org" would be the operator identifier (total 38 bytes)
   "ims" would be the network identifier
  
    For references check:

    Spec: 123 003 V 12.4.1 (2014-10)
    9 Definition of Access Point Name
    19.4.2.2 Access Point Name FQDN (APN-FQDN)

  ------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    check for mnc<MNC>.mcc<MCC>.gprs format
   --------------------------------------------------------------------------*/
  if(encoded_apn_length > DS3G_PROFILE_OPERATOR_IDENTIFIER_TOTAL_LENGTH) //Greater than 19 bytes
  {
    do
    {
      has_operator_identifier_part = TRUE;
      index = encoded_apn_length - DS3G_PROFILE_OPERATOR_IDENTIFIER_TOTAL_LENGTH;
      label_length = (uint32)(*(encoded_apn+index));
      /*-------------------------------------------------------------------------
        check length and make sure it has "mnc"
       --------------------------------------------------------------------------*/
      if((label_length != DS3G_PROFILE_MCC_MNC_LABEL_LENGTH) || 
         (strncasecmp((char*)encoded_apn+index+1,"mnc",DS3G_PROFILE_MCC_MNC_STR_LENGTH) != 0))
      {
        has_operator_identifier_part = FALSE;
        break;
      }

      index += label_length + 1;
      label_length = (uint32)(*(encoded_apn+index));
      /*-------------------------------------------------------------------------
       check length and make sure it has "mcc"
       --------------------------------------------------------------------------*/
      if((label_length != DS3G_PROFILE_MCC_MNC_LABEL_LENGTH) || 
        (strncasecmp((char*)encoded_apn+index+1,"mcc",DS3G_PROFILE_MCC_MNC_STR_LENGTH) != 0))
      {
        has_operator_identifier_part = FALSE;
        break;
      }

      index += label_length + 1;
      label_length = (uint32)(*(encoded_apn+index));
      /*-------------------------------------------------------------------------
       check length and make sure it has "gprs"
      --------------------------------------------------------------------------*/
      if((label_length != DS3G_PROFILE_GPRS_LABEL_LENGTH) || 
        (strncasecmp((char*)encoded_apn+index+1,"gprs",DS3G_PROFILE_GPRS_STR_LENGTH) != 0))
      {
        has_operator_identifier_part = FALSE;
        break;
      }
    }while (0); 
  }

  /*-------------------------------------------------------------------------
    check for FQDN format apn.epc.mnc<MNC>.mcc<MCC>.3gppnetwork.org.
    Skip check if format is already valid from non-FQDN check
   --------------------------------------------------------------------------*/
  if ((encoded_apn_length > DS3G_PROFILE_FQDN_OPERATOR_IDENTIFIER_TOTAL_LENGTH) && 
      (has_operator_identifier_part == FALSE))
  {
    do
    {
      has_fqdn_operator_identifier_part = TRUE;
      index = encoded_apn_length - DS3G_PROFILE_FQDN_OPERATOR_IDENTIFIER_TOTAL_LENGTH;
      label_length = (uint32)(*(encoded_apn+index));


      /*-------------------------------------------------------------------------
        check length and make sure it has "mnc"
       --------------------------------------------------------------------------*/
      if((label_length != DS3G_PROFILE_MCC_MNC_LABEL_LENGTH) || 
         (strncasecmp((char*)encoded_apn+index+1,"mnc",DS3G_PROFILE_MCC_MNC_STR_LENGTH) != 0))
      {
        has_fqdn_operator_identifier_part = FALSE;
        break;
      }

      index += label_length + 1;
      label_length = (uint32)(*(encoded_apn+index));

      /*-------------------------------------------------------------------------
       check length and make sure it has "mcc"
       --------------------------------------------------------------------------*/
      if((label_length != DS3G_PROFILE_MCC_MNC_LABEL_LENGTH) || 
        (strncasecmp((char*)encoded_apn+index+1,"mcc",DS3G_PROFILE_MCC_MNC_STR_LENGTH) != 0))
      {
        has_fqdn_operator_identifier_part = FALSE;
        break;
      }

      index += label_length + 1;
      label_length = (uint32)(*(encoded_apn+index));

      /*-------------------------------------------------------------------------
        check length and make sure it has "3gppnetwork"
       --------------------------------------------------------------------------*/
      if((label_length != DS3G_PROFILE_3GPPNETWORK_STR_LENGTH) || 
         (strncasecmp((char*)encoded_apn+index+1,"3gppnetwork",DS3G_PROFILE_3GPPNETWORK_STR_LENGTH) != 0))
      {
        has_fqdn_operator_identifier_part = FALSE;
        break;
      }

      index += label_length + 1;
      label_length = (uint32)(*(encoded_apn+index));

      /*-------------------------------------------------------------------------
        check length and make sure it has "org"
       --------------------------------------------------------------------------*/
      if((label_length != DS3G_PROFILE_ORG_STR_LENGTH) || 
         (strncasecmp((char*)encoded_apn+index+1,"org",DS3G_PROFILE_ORG_STR_LENGTH) != 0))
      {
        has_fqdn_operator_identifier_part = FALSE;
        break;
      }
    }while (0); 
  }

  if(has_operator_identifier_part == TRUE) 
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Valid Operator Identifier Found");
    encoded_apn_length -= DS3G_PROFILE_OPERATOR_IDENTIFIER_TOTAL_LENGTH;
  }
  else if (has_fqdn_operator_identifier_part == TRUE)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Valid FQDN Operator Identifier Found");
    encoded_apn_length -= DS3G_PROFILE_FQDN_OPERATOR_IDENTIFIER_TOTAL_LENGTH;
  }
  
  label_length = 0;
  index = 0;

  while(index < encoded_apn_length)
  {
    /*----------------------------------------------------------------------
      Get the length of the label
      Increment the label count
      Increase the offset into the string to point to the next label length
      indicator
    ----------------------------------------------------------------------*/
    label_length = (uint32)(*(encoded_apn+index));

    if(label_length > 0)
    {
      label_count++;
    }
    /*--------------------------------------------------------------------------
      Always increment by label_length+1. This is to ensure that we get to the
      next label_length indicator
    --------------------------------------------------------------------------*/
    index += (label_length+1);

  }// while loop to traverse the encoded APN string

  DATA_3GMGR_MSG1(MSG_LEGACY_MED,"Number of labels in APN Network identifier: %d",label_count);

  if(label_count < 1)
  {
    ret_val = FALSE;
  }
  else
  {
    /*--------------------------------------------------------------------------
      Reset the index to 1.
      Also reset the label_length to 0
    --------------------------------------------------------------------------*/
    index = 1;
    label_length = 0;

    /*------------------------------------------------------------------------
      Remember the following while copying from src to dest:

      Apn_string_offset is used to index into both buffers, but the offset
      in the src string is always one greater than dest
    ------------------------------------------------------------------------*/
    
    while(index <= label_count)
    {
      label_length = (uint32)(*(encoded_apn+apn_string_offset));

      if((apn_string_offset + label_length) < encoded_apn_length)
      {
        memscpy(decoded_apn+apn_string_offset, 
                label_length,
                encoded_apn+apn_string_offset+1,
                label_length);

        /*----------------------------------------------------------------------
          Increment the apn_string_offset by label_length
        ----------------------------------------------------------------------*/
        apn_string_offset += label_length;

        if(index == label_count)
        {
          /*--------------------------------------------------------------------
            Terminate the string with NULL
          --------------------------------------------------------------------*/
          *(decoded_apn+apn_string_offset) = 0x0;
        }
        else
        {
          /*--------------------------------------------------------------------
            Write the label delimitor in the decoded string
          --------------------------------------------------------------------*/
          *(decoded_apn+apn_string_offset) = LABEL_DELIMITOR;

          /*--------------------------------------------------------------------
            Increment the apn_string_offset by one.
          --------------------------------------------------------------------*/
          apn_string_offset++;
        }
        index++;
      }
      else
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,"Error decoding. Avoiding buf overrun on Src Str");
        ret_val = FALSE;
        break;
      }

    } //while loop to extract labels corresponding to N/w Identifier
  } // label count greater than 3

  /*-----------------------------------------------------------------------------------------
   The APN Network Identifier shall contain at least one label and shall have, after encoding as 
   defined in subclause 9.1 above, a maximum length of 63 octets. An APN Network Identifier shall
   not start with any of the strings "rac", "lac", "sgsn" or "rnc", and it shall not end in ".gprs".
   ------------------------------------------------------------------------------------------*/
  
  if((has_fqdn_operator_identifier_part == TRUE)&&
     (apn_string_offset > DS3G_PROFILE_APN_EPC_STR_LENGTH_WITH_DELIM) &&
     (strncasecmp((const char *)(decoded_apn+(apn_string_offset-DS3G_PROFILE_APN_EPC_STR_LENGTH_WITH_DELIM)),
              ".apn.epc",DS3G_PROFILE_APN_EPC_STR_LENGTH_WITH_DELIM) == 0))
  {
    apn_string_offset -= DS3G_PROFILE_APN_EPC_STR_LENGTH_WITH_DELIM;
    decoded_apn[apn_string_offset] = 0;
  }

  if (ret_val && ds3g_profile_apn_is_network_identifier_valid(decoded_apn, apn_string_offset)) 
  {
    ret_val = TRUE; 
  }
  else
  {
    ret_val = FALSE;
  }
  
  return ret_val;
} /* ds3g_decode_apn_name */

/*===========================================================================
FUNCTION DS3G_DECODE_APN_NAME_NON_STRICT

DESCRIPTION
  This function is used to decode the APN name returned by the network. This
  function leaves out the operator identifier. It picks up only the labels
  that reflect the network identifier.

PARAMETERS
  IN
  encoded_apn - Pointer to the APN returned by the network. This is encoded
  encoded_apn_length - length of the encoded string

  OUT
  decoded_apn - Pointer to the byte array in which the decoded APN is returned

DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE if decoding is successful
            FALSE if there is any error
   
SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_decode_apn_name_non_strict
(
  byte    *encoded_apn,
  uint32   encoded_apn_length,
  byte    *decoded_apn
)
{
  uint32  label_count = 0;
  uint32  label_length = 0;
  uint32  index = 0;
  boolean ret_val = TRUE;
  uint32  apn_string_offset = 0;
  uint32  temp_encoded_apn_length = encoded_apn_length;
  boolean op_id_found = FALSE;
  uint16  label_length_arr[20] = {0};
  uint16  temp_label_count = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_3GMGR_MSG1(MSG_LEGACY_LOW,"Encoded APN length: %d",encoded_apn_length);
  DATA_3GMGR_MSG_SPRINTF_1(MSG_LEGACY_LOW,"Encoded APN name: %s",encoded_apn);
  /*----------------------------------------------------------------------- 
    Store all the label lengths in an array, This is helpful to scan the
    string from the reverse and to find keywords like mcc.
    Also get the number of labels that exist
    -----------------------------------------------------------------------*/
  while(temp_encoded_apn_length > 0)
  {
    label_length = (uint16)(*(encoded_apn+index));

    if((label_count < 20) && (label_length > 0))
    {
      label_length_arr[label_count] = label_length;
      label_count++;
    }

    index += label_length + 1;

    if (temp_encoded_apn_length < (label_length + 1))
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,"Format Error");  
      return FALSE;
    }
    else
    {
      temp_encoded_apn_length -= (label_length + 1);
    }
  }

  index = 0;
  temp_encoded_apn_length = encoded_apn_length;

  /*--------------------------------------------------------------------- 
    Now go to the end of the string and try to find if key word
    mnc  exists, If MNC exists then look for the match epc.apn.mnc.
   
    MNC should not be at the first level. If mNc is at the first level
    treat the whole string as network identifier
   
    If epc.apn.mnc is found all content before epc.apn.mnc is the
    Network ID
   
    If mcc is found all content before MNC is the Network ID
    ---------------------------------------------------------------------*/

  /*--------------------------------------------------------------------- 
    Reach the last label
  ---------------------------------------------------------------------*/
  do
  {
    label_length = *(encoded_apn+index);
    temp_label_count++;

    if(temp_label_count == label_count)
    {
      break;
    }

    index += label_length+1;

  }while(1);


  /*---------------------------------------------------------------------- 
    Now start traversing from the back of the label
    and examine the contents to see if MNC is found
    If MNC is found, exit out of the while loop
    ---------------------------------------------------------------------*/
  while(temp_label_count > 0)
  {
    label_length = *(encoded_apn+index);

    if((strncasecmp((char*)encoded_apn+index+1,
                    "mnc",DS3G_PROFILE_MCC_MNC_STR_LENGTH) == 0) &&
        temp_label_count != 1)
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,"MNC match non_strict check");
      op_id_found =  TRUE;
      break;
    }
 
    if((temp_label_count-2) >= 0)
    {
      index = index - (label_length_arr[temp_label_count-2]) - 1;
    }
    temp_label_count--;
  }

  /*----------------------------------------------------------------------- 
    If no match occurs at this point then entire length is the
    Network identifier length.
   
    If a match occurs then check for epc.apn(reverse)
    -----------------------------------------------------------------------*/
  do
  {
    if(op_id_found)
    {
      temp_encoded_apn_length = index;
      /*-------------------------------------------------------------------- 
        Get the previous label and see if it is epc
      -------------------------------------------------------------------*/
      temp_label_count--;

      if((temp_label_count > 0) && ((temp_label_count-2) >= 0))
      {
        index = index - (label_length_arr[temp_label_count-2]) - 1;
      }
      else
      {
        break; /* Means not enough labels to scan*/
      }

      label_length = *(encoded_apn+index);
      /*-------------------------------------------------------------------------
         check for EPC
      --------------------------------------------------------------------------*/
      if((label_length != DS3G_PROFILE_EPC_STR_LENGTH) || 
         (strncasecmp((char*)encoded_apn+index+1,"epc",
                      DS3G_PROFILE_EPC_STR_LENGTH) != 0))
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_LOW,"epc.apn match non_strict check fail");
        break;
      }

      temp_label_count--;
      /*-------------------------------------------------------------------- 
        Get the previous label and see if it is apn
      -------------------------------------------------------------------*/
      if((temp_label_count > 0) && (temp_label_count-2) >= 0)
      {
        index = index - (label_length_arr[temp_label_count-2]) - 1;
      }
      else
      {
        break; /* Means not enough labels to scan*/
      }

      label_length = *(encoded_apn+index);
    /*-------------------------------------------------------------------------
       check for apn
     --------------------------------------------------------------------------*/
      if((label_length != DS3G_PROFILE_APN_STR_LENGTH) || 
         (strncasecmp((char*)encoded_apn+index+1,"apn",
                      DS3G_PROFILE_APN_STR_LENGTH) != 0))
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_LOW,"apn match non_strict check fail");
        break;
      }


     /*-------------------------------------------------------------------- 
       If we reach this point epc.apn match is successful
       Now set the temp_encoded_apn_length to the index value
      --------------------------------------------------------------------*/
      temp_encoded_apn_length = index;
    }
  }while(0);
  

  label_length = 0;
  index = 0;
  label_count = 0;

  while(index < temp_encoded_apn_length)
  {
    /*----------------------------------------------------------------------
      Get the length of the label
      Increment the label count
      Increase the offset into the string to point to the next label length
      indicator
    ----------------------------------------------------------------------*/
    label_length = (uint32)(*(encoded_apn+index));

    if(label_length > 0)
    {
      label_count++;
    }
    /*--------------------------------------------------------------------------
      Always increment by label_length+1. This is to ensure that we get to the
      next label_length indicator
    --------------------------------------------------------------------------*/
    index += (label_length+1);

  }// while loop to traverse the encoded APN string

  DATA_3GMGR_MSG1(MSG_LEGACY_MED,"Number of labels in APN Network identifier: %d",label_count);

  if(label_count < 1)
  {
    ret_val = FALSE;
  }
  else
  {

    /*--------------------------------------------------------------------------
      Reset the index to 1.
      Also reset the label_length to 0
    --------------------------------------------------------------------------*/
    index = 1;
    label_length = 0;

    /*------------------------------------------------------------------------
      Remember the following while copying from src to dest:

      Apn_string_offset is used to index into both buffers, but the offset
      in the src string is always one greater than dest
    ------------------------------------------------------------------------*/
    
    while(index <= label_count)
    {
      label_length = (uint32)(*(encoded_apn+apn_string_offset));

      if((apn_string_offset + label_length) < temp_encoded_apn_length)
      {
        memscpy(decoded_apn+apn_string_offset, 
                label_length,
                encoded_apn+apn_string_offset+1,
                label_length);

        /*----------------------------------------------------------------------
          Increment the apn_string_offset by label_length
        ----------------------------------------------------------------------*/
        apn_string_offset += label_length;

        if(index == label_count)
        {
          /*--------------------------------------------------------------------
            Terminate the string with NULL
          --------------------------------------------------------------------*/
          *(decoded_apn+apn_string_offset) = 0x0;
        }
        else
        {
          /*--------------------------------------------------------------------
            Write the label delimitor in the decoded string
          --------------------------------------------------------------------*/
          *(decoded_apn+apn_string_offset) = LABEL_DELIMITOR;

          /*--------------------------------------------------------------------
            Increment the apn_string_offset by one.
          --------------------------------------------------------------------*/
          apn_string_offset++;
        }
        index++;
      }
      else
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,"Error decoding. Avoiding buf overrun on Src Str");
        ret_val = FALSE;
        break;
      }
    } //while loop to extract labels corresponding to N/w Identifier
  } // label count greater than 3

  DATA_3GMGR_MSG_SPRINTF_1(MSG_LEGACY_HIGH,"Decoded APN name: %s",decoded_apn);
  return ret_val;
} /* ds3g_decode_apn_name */

/*===========================================================================
FUNCTION DS3G_PROFILE_VALIDATOR_COMPARE_FORBIDDEN_APN

DESCRIPTION
  This function compares the APN name passed with the APN name present in
  a forbidden APN info block

PARAMETERS 
  item_ptr: Element from forbidden APN list queue 
  target_val(apn_name): APN to be checked if its forbidden or not 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If the forbidden APN name matches the APN name present in the queue block
  FALSE: Otherwise

SIDE EFFECTS
  None.
===========================================================================*/
int ds3g_profile_validator_compare_forbidden_apn
(
  void  *item_ptr,
  void  *target_val
)
{
  char                                            *apn_name = NULL;
  int                                              ret_val = FALSE;
  ds3g_profile_validator_forbidden_apn_info_type  *forbidden_apn_info_blk_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( NULL == item_ptr || NULL == target_val )
  {
    return ret_val;
  }

  forbidden_apn_info_blk_ptr = 
                   (ds3g_profile_validator_forbidden_apn_info_type *)item_ptr;
  apn_name = (char *)target_val;

  if(strlen(apn_name) == 
               strlen((const char*)forbidden_apn_info_blk_ptr->forbidden_apn))
  {
    if(strncasecmp((const char *)apn_name, 
                   (const char *)forbidden_apn_info_blk_ptr->forbidden_apn,
                   DS_SYS_MAX_APN_LEN) == 0)
    {
      ret_val = TRUE;
    }
  }

  return ret_val;
} /* ds3g_profile_validator_compare_forbidden_apn */

/*===========================================================================
FUNCTION DS3G_PROFILE_VALIDATOR_IS_APN_FORBIDDEN

DESCRIPTION
  This function invokes the utility to check if the APN is a forbidden APN or
  not

PARAMETERS 
  pdn_context_ptr: PDN who APN name needs to be validated 
  subs_id: Current active PS subs

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds3g_profile_validator_is_apn_forbidden
(
  ds_pdn_context_s       *pdn_context_ptr,
  sys_modem_as_id_e_type  subs_id
)
{
  boolean              ret_val = FALSE;
  sys_sys_mode_e_type  network_mode = SYS_SYS_MODE_NONE;
  ds3g_apn_name_type   apn_name;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    if(!ds3gsubsmgr_is_subs_id_valid(subs_id) ||
       !ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_ptr))
    {
      break;
    }

    network_mode = ds_3gpp_get_apn_current_mode(pdn_context_ptr);

    DS_3GPP_MSG2_MED("Network Mode: %d and Emergency service support: %d",
                     network_mode, 
                     pdn_context_ptr->ds_pdn_context_dyn_p->pdp_profile.
                     emergency_calls_are_supported);

    if(!DS_3GPP_IS_3GPP_CURRENT_MODE(network_mode))
    {
      break;
    }

#ifdef FEATURE_DATA_LTE 
    if (((SYS_SYS_MODE_LTE == network_mode) && 
        (DS_EPS_ATTACHED != ds_eps_attach_sm_get_state(subs_id))) ||
        DS_3GPP_PDN_SUPPORTS_EMERGENCY_SERVICES(pdn_context_ptr))
    {
      break;
    }
#endif /* FEATURE_DATA_LTE  */

    memset(&apn_name,0,sizeof(ds3g_apn_name_type));

    /*------------------------------------------------------------------------ 
      If we have reached this stage, It is an additional PDN
      Check if the apn in the PDN context is NULL, If NULL apn return TRUE
     ------------------------------------------------------------------------*/
    if(ds_3gpp_get_apn_from_pdn_context(pdn_context_ptr,&apn_name))
    {
      if(NULL != q_linear_search(&ds3g_profile_validator_forbidden_apn_q[subs_id],
                                 ds3g_profile_validator_compare_forbidden_apn,
                                 (void *)apn_name.apn_string))
      {
        ret_val = TRUE;
      }
    }
  }while(0);

  return ret_val;
} /* ds3g_profile_validator_is_apn_forbidden */

/*===========================================================================
FUNCTION DS3G_PROFILE_VALIDATOR_FREE_FORBIDDEN_APN_INFO_Q

DESCRIPTION
  This function frees the existing APN info queue before saving new contents

PARAMETERS
  subs_id: Current active PS subs

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Empties Forbidden APN Info Queue
===========================================================================*/
void ds3g_profile_validator_free_forbidden_apn_info_q
(
  sys_modem_as_id_e_type  subs_id
)
{
  ds3g_profile_validator_forbidden_apn_info_type   *extracted_q_block_ptr = NULL;
  ds3g_profile_validator_forbidden_apn_info_type   *prev_q_block_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  extracted_q_block_ptr = (ds3g_profile_validator_forbidden_apn_info_type *)
          q_check(&ds3g_profile_validator_forbidden_apn_q[subs_id]);

  while(NULL != extracted_q_block_ptr)
  {
    prev_q_block_ptr = extracted_q_block_ptr;
    extracted_q_block_ptr = q_next(
                             &ds3g_profile_validator_forbidden_apn_q[subs_id],
                             &(extracted_q_block_ptr->link));

    q_delete(&ds3g_profile_validator_forbidden_apn_q[subs_id],
             &(prev_q_block_ptr->link));
    modem_mem_free(prev_q_block_ptr, MODEM_MEM_CLIENT_DATA);
    prev_q_block_ptr = NULL;
  }

  return;
} /* ds3g_profile_validator_free_forbidden_apn_info_q */

/*===========================================================================
FUNCTION DS3G_PROFILE_VALIDATOR_COPY_FORBIDDEN_APN_INFO

DESCRIPTION
  This function copies the APN info extracted from EFS file into a local
  queue

PARAMETERS
  efs_extracted_info_q_ptr: Queue containing the extracted EFS info
  subs_id: Current active PS subs

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds3g_profile_validator_copy_forbidden_apn_info
(
  q_type                  *efs_extracted_info_q_ptr,
  sys_modem_as_id_e_type   subs_id
)
{
  ds_3gppi_utils_extracted_info_type   *efs_extracted_info_block_ptr = NULL;
  ds_3gppi_utils_extracted_info_type   *prev_info_block_ptr = NULL;
  ds3g_profile_validator_forbidden_apn_info_type  
                                       *forbidden_apn_info_block_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  ds3g_profile_validator_free_forbidden_apn_info_q(subs_id);

  efs_extracted_info_block_ptr = (ds_3gppi_utils_extracted_info_type *)
                                              q_check(efs_extracted_info_q_ptr);

  while(NULL != efs_extracted_info_block_ptr)
  {
    forbidden_apn_info_block_ptr = (ds3g_profile_validator_forbidden_apn_info_type *)
           modem_mem_alloc(sizeof(ds3g_profile_validator_forbidden_apn_info_type),
                           MODEM_MEM_CLIENT_DATA);

    if(NULL != forbidden_apn_info_block_ptr)
    {
      memset(forbidden_apn_info_block_ptr, 
              0,
              sizeof(ds3g_profile_validator_forbidden_apn_info_type));

      strlcpy(forbidden_apn_info_block_ptr->forbidden_apn, 
              efs_extracted_info_block_ptr->extracted_data,
              efs_extracted_info_block_ptr->extracted_data_length+1);

      q_link(forbidden_apn_info_block_ptr , 
             &(forbidden_apn_info_block_ptr->link));
      q_put(&ds3g_profile_validator_forbidden_apn_q[subs_id], 
            &(forbidden_apn_info_block_ptr->link));
    }

    prev_info_block_ptr = efs_extracted_info_block_ptr;

    efs_extracted_info_block_ptr = (ds_3gppi_utils_extracted_info_type *)
      q_next(efs_extracted_info_q_ptr, &(efs_extracted_info_block_ptr->link));

    q_delete(efs_extracted_info_q_ptr, &(prev_info_block_ptr->link));

    modem_mem_free(prev_info_block_ptr->extracted_data, MODEM_MEM_CLIENT_DATA);
    modem_mem_free(prev_info_block_ptr, MODEM_MEM_CLIENT_DATA);
    prev_info_block_ptr = NULL;
  }

  return;
} /* ds3g_profile_validator_copy_forbidden_apn_info */

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_READ_FORBIDDEN_APN_LIST_FROM_EFS

DESCRIPTION
  This function invokes the utility to read forbidden APN list from EFS
  and then copies the info into its local structure

PARAMETERS
  subs_id: Current active PS subs

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds3g_profile_validator_read_forbidden_apn_list_from_efs
(
  sys_modem_as_id_e_type  subs_id
)
{
   q_type  efs_extracted_info_q;
   char    efs_file_name[] = "/data/forbidden_apn_list.txt";
   char    compare_tag[] = "forbidden_apn";
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   memset(&efs_extracted_info_q, 0, sizeof(q_type));
   q_init(&efs_extracted_info_q);

   if(FALSE == ds_3gppi_utils_read_apn_info_from_efs((char *)compare_tag,
                                                     (char *)efs_file_name,
                                                     &efs_extracted_info_q,
                                                     subs_id))
   {
     DATA_MSG_SPRINTF_1(MSG_LEGACY_LOW, 
                        "Unable to read contents of file: %s", efs_file_name);
     q_destroy(&efs_extracted_info_q);
     return;
   }

   ds3g_profile_validator_copy_forbidden_apn_info(&efs_extracted_info_q,subs_id);
   q_destroy(&efs_extracted_info_q);

   return;
} /* ds3g_profile_validator_read_forbidden_apn_list_from_efs */

/*===========================================================================
FUNCTION  DS3G_PROFILE_VALIDATOR_CLEAR_STATE_PER_SUBS_ID

DESCRIPTION
  This function cleans the context for a given subs id when NV refresh occurs

PARAMETERS
  subs_id: Subscription Id 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds3g_profile_validator_clear_state_per_subs_id
(
  sys_modem_as_id_e_type  subs_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  ds3g_profile_validator_free_forbidden_apn_info_q(subs_id);
  return;
} /* ds3g_profile_validator_clear_state_per_subs_id */

/*===========================================================================
FUNCTION  DS3G_PROFILE_VALIDATOR_PER_SUBS_INFO_INIT

DESCRIPTION
  This function initializes and reads the forbidden APN list at boot-up
  for validation with profile contents

PARAMETERS
  subs_id: Subscription Id 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds3g_profile_validator_per_subs_info_init
(
  sys_modem_as_id_e_type  subs_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }
  /*-----------------------------------------------------------------------
    Read forbidden APN List info from EFS: /data/forbidden_apn_list.txt      
  -----------------------------------------------------------------------*/
  ds3g_profile_validator_read_forbidden_apn_list_from_efs(subs_id);
  return;
}

/*===========================================================================
FUNCTION  DS3G_PROFILE_VALIDATOR_POWERUP_INIT

DESCRIPTION
  This function initializes  forbidden APN queue at powerup

PARAMETERS
  subs_id          : Subscription Id 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds3g_profile_validator_powerup_init()
{
  boolean i = 0;
 /* ----------------------------------------------------------------  */
 
  /* initialize the forbidden apn queue at powerup itself as NV refresh may happen 
        before queue gets initialized later */
        
  for(i=0; i<DS3GSUBSMGR_SUBS_ID_MAX; i++)
  {
    memset(&ds3g_profile_validator_forbidden_apn_q[i], 0, sizeof(q_type));
    q_init(&ds3g_profile_validator_forbidden_apn_q[i]);      
  }
  return;
}

