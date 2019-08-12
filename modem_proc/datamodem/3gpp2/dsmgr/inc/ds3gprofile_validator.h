
#ifndef DS3PROFILE_VALIDATOR_H
#define DS3PROFILE_VALIDATOR_H
/*===========================================================================

               3 G   D A T A   S E R V I C E S   M A N A G E R

           P R O F I L E   V A L I D A T O R   F U N C T I O N S

                            H E A D E R    F I L E

DESCRIPTION 
  This file contains functions for validating the parameters of the attach
  APN.This header file is intended to be used internal to the 3GPP module.

  Copyright (c) 2014-2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/inc/ds3gprofile_validator.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/05/2014 ms      introduce this module to validate apn name for 3gpp spec

===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "ds_profile_db.h"
#include "ds_3gpp_pdn_context.h"


/*===========================================================================

                     MACRO

===========================================================================*/
/*---------------------------------------------------------------------------
  length of string "mncXXX.mccYYY.gprs" or 
  "6mncXXX6mccYYY113gppnetwork4org" is 19 bytes
---------------------------------------------------------------------------*/
#define DS3G_PROFILE_OPERATOR_IDENTIFIER_TOTAL_LENGTH 19
#define DS3G_PROFILE_NETWORK_IDENTIFIER_MAX_LENGTH 62
#define DS3G_PROFILE_MCC_MNC_STR_LENGTH_WITH_DELIM 4
#define DS3G_PROFILE_GPRS_STR_LENGTH_WITH_DELIM 5
#define DS3G_PROFILE_MCC_MNC_STR_LENGTH 3
#define DS3G_PROFILE_GPRS_STR_LENGTH 4
#define DS3G_PROFILE_MCC_MNC_LABEL_LENGTH 6
#define DS3G_PROFILE_GPRS_LABEL_LENGTH 4
#define DS3G_PROFILE_RAC_LAC_RNC_STR_LENGTH 3
#define DS3G_PROFILE_SGSN_STR_LENGTH 4
#define DS3G_PROFILE_MCC_MNC_LENGTH_WITH_DELIM 7

/*---------------------------------------------------------------------------
  length of string "mncXXX.mccYYY.3gppnetwork.org" or 
  "6mncXXX6mccYYY113gppnetwork4org" is 30 bytes
---------------------------------------------------------------------------*/
#define DS3G_PROFILE_FQDN_OPERATOR_IDENTIFIER_TOTAL_LENGTH 30

#define DS3G_PROFILE_APN_STR_LENGTH 3
#define DS3G_PROFILE_APN_STR_LENGTH_WITH_DELIM 4
#define DS3G_PROFILE_EPC_STR_LENGTH 3
#define DS3G_PROFILE_EPC_STR_LENGTH_WITH_DELIM 4

/*---------------------------------------------------------------------------
  length of string ".apn.epc" 8 bytes
---------------------------------------------------------------------------*/
#define DS3G_PROFILE_APN_EPC_STR_LENGTH_WITH_DELIM 8

#define DS3G_PROFILE_3GPPNETWORK_STR_LENGTH 11
#define DS3G_PROFILE_3GPPNETWORK_STR_LENGTH_WITH_DELIM 12
#define DS3G_PROFILE_ORG_STR_LENGTH 3
#define DS3G_PROFILE_ORG_STR_LENGTH_WITH_DELIM 4

/*===========================================================================

                         INTERNAL DEFINATION & TYPES

===========================================================================*/
/*---------------------------------------------------------------------------
  Structure block that contains forbidden APN name info as read from EFS

  /data/forbidden_apn_list.txt
---------------------------------------------------------------------------*/
typedef struct
{
  q_link_type        link;
  char               forbidden_apn[DS_SYS_MAX_APN_LEN];
}ds3g_profile_validator_forbidden_apn_info_type;

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
  boolean - TRUE if extraction is successful
            FALSE if there is any error
   
SIDE EFFECTS
  None
===========================================================================*/
uint8 ds3g_get_network_identifer_from_apn_name
(
  char*  input_apn,
  byte   input_apn_length,
  char*  network_identifier
);


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
   byte*  apn_ni,
   uint8 apn_ni_len
);

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
  char *apn_name,
  uint8 apn_name_len
);


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
  byte*  encoded_apn,
  uint32 encoded_apn_length,
  byte*  decoded_apn
);

/*===========================================================================
FUNCTION DS3G_DECODE_APN_NAME_NON_STRICT

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
boolean ds3g_decode_apn_name_non_strict
(
  byte*  encoded_apn,
  uint32 encoded_apn_length,
  byte*  decoded_apn
);

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
);

/*===========================================================================
FUNCTION  DS_3GPP_PDN_CONTEXT_CLEAR_PER_SUBS_ID

DESCRIPTION
  This function cleans the context for a given subs id when NV refresh occurs

PARAMETERS
  subs_id          : Subscription Id 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds3g_profile_validator_clear_state_per_subs_id
(
  sys_modem_as_id_e_type        subs_id
);

/*===========================================================================
FUNCTION  DS3G_PROFILE_VALIDATOR_PER_SUBS_INFO_INIT

DESCRIPTION
  This function initializes and reads the forbidden APN list at boot-up
  for validation with profile contents

PARAMETERS
  subs_id          : Subscription Id 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds3g_profile_validator_per_subs_info_init
(
  sys_modem_as_id_e_type        subs_id
);

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
void ds3g_profile_validator_powerup_init();

#endif /* DS3PROFILE_VALIDATOR_H */
