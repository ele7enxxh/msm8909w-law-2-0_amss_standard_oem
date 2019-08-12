#ifndef HDRKEP_H
#define HDRKEP_H
/*===========================================================================

             D H   K E Y   E X C H A N G E   P R O T O C O L   
                           D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for the DH Key Exchange Protocol

Copyright (c) 2001-2005 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/COMMON/vcs/hdrkep.h_v   1.2   14 Mar 2003 11:10:32   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/inc/hdrkep.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/13/05   pba     Changes to support multiple personality negotiation
10/04/05   pba     Added support to read/write config parameters from SCMDB/EFS
03/12/03   mpa     Fixed attribute_id_length
03/19/02   om      Fixed get_previous_rpc_auth_key() to return key length
11/29/01   om      First integrated version
10/19/01   dna     Created module

===========================================================================*/

/*===========================================================================

                    INCLUDE  FILES  FOR  MODULE

===========================================================================*/
#include "dsm.h"
#include "hdrhai.h"
#include "hdrind.h"

/* <EJECT> */
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/
/* Attribute ID length in bytes */
#define HDRKEP_ATTRIBUTE_ID_LENGTH 1

/* <EJECT> */
/*===========================================================================

                 DEFINITIONS  FOR MESSAGE PARSING
             
===========================================================================*/
#define HDRKEP_KEY_REQ_MSG       0x00
#define HDRKEP_KEY_RESP_MSG      0x01
#define HDRKEP_AN_KEY_COMP_MSG   0x02
#define HDRKEP_AT_KEY_COMP_MSG   0x03
#define HDRKEP_CONFIG_REQ_MSG    0x50
#define HDRKEP_CONFIG_RESP_MSG   0x51

/* Length of keys (message digests) that are available as public data */
#define HDRKEP_KEY_SIG_LEN       20

typedef struct
{
  /* Key length in octets - either 96 or 128 */
  uint16   key_len;

  /* If the keys (rpc_auth_key) are valid */
  boolean  keys_are_valid;

  /* Storage for the derived keys */
  uint8    rpc_auth_key[HDRKEP_KEY_SIG_LEN];
  /* The other 7 keys are not stored because they are not used */

} hdrkep_config_params_type;

/* <EJECT> */
/*===========================================================================

                        DATA  DECLARATIONS

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      FUNCTION  DECLARATIONS

===========================================================================*/

/* <EJECT> */
/*===========================================================================

FUNCTION HDRKEP_POWERUP_INIT

DESCRIPTION
  This function performs powerup initialization for Location Update protocol.

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

extern void hdrkep_powerup_init( void );

/*===========================================================================

FUNCTION HDRKEP_CONFIG_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to KEP in-config instance.

DEPENDENCIES
  None
  
PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

extern void hdrkep_config_msg_cb 
(
  dsm_item_type*  msg_ptr
);

/*===========================================================================

FUNCTION HDRKEP_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to KEP.

DEPENDENCIES
  None
 
PARAMETERS
  ind_name - Indication to process, is a global enum for all
                    possible indications in HDR signaling
  ind_data - Optional data pointer for indication specific data
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

extern void hdrkep_ind_cb 
(
  hdrind_ind_name_enum_type  ind_name,
  void*                      ind_data
);

/*===========================================================================

FUNCTION HDRKEP_GET_RPC_AUTH_KEY

DESCRIPTION
  This function returns the RPC Auth Key, if it has been negotiated.

DEPENDENCIES
  None
 
PARAMETERS
  rpc_auth_key - Pointer to an array of HDRKEP_KEY_SIG_LEN octets in which
                 the current value of rpc_auth_key is returned, if available.
 
RETURN VALUE
  Size of the key in bytes, 0 if non-valid.
  
SIDE EFFECTS
  None

===========================================================================*/
uint16 hdrkep_get_rpc_auth_key
(
  uint8* rpc_auth_key
);

/*===========================================================================

FUNCTION HDRKEP_GET_PREV_RPC_AUTH_KEY

DESCRIPTION
  This function returns the RPC Auth Key of the previous session, 
  if it has been negotiated.

DEPENDENCIES
  None
 
PARAMETERS
  rpc_auth_key - Pointer to an array of HDRKEP_KEY_SIG_LEN octets in which
                 the current value of rpc_auth_key is returned, if available.
 
RETURN VALUE
  Size of the key in bytes, 0 if non-valid.
  
SIDE EFFECTS
  None

===========================================================================*/
uint16 hdrkep_get_previous_rpc_auth_key
(
  uint8 *rpc_auth_key
);

/*===========================================================================

FUNCTION HDRKEP_SET_CONFIG_TO_DEFAULT

DESCRIPTION
  This function initializes the Key Exchange Protocol configuration parameters
  to default values.

DEPENDENCIES
  None

PARAMETERS
  current_config - Pointer to Key Exchange Protocol's current configuration 
                   structure in SCMDB.
                   
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrkep_set_config_to_default
( 
  hdrkep_config_params_type *current_config 
);

/*===========================================================================

FUNCTION HDRKEP_GET_INCONFIG_HANDLE

DESCRIPTION
  This function returns a pointer to inconfig instance of Key Exchange 
  Protocol 

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  Pointer to inconfig instance of KEP

SIDE EFFECTS
  None

===========================================================================*/
extern void* hdrkep_get_inconfig_handle( void );

#endif /* HDRKEP_H */











