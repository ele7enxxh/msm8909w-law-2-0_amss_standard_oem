#ifndef HDRAUTH_H
#define HDRAUTH_H
/*===========================================================================

        S H A - 1   A U T H E N T I C A T I O N   P R O T O C O L   
                           D E F I N I T I O N S

DESCRIPTION
  This module contains the public declarations for the SHA-1 
  Authentication Protocol as defined in the security layer of IS-856.

Copyright (c) 2001-2005 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrauth.h_v   1.0   29 Nov 2001 17:33:16   omichael  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/inc/hdrauth.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/27/07   wsh     Added support for new prior session handling
12/13/05   pba     Changes to support multiple personality negotiation
10/04/05   pba     Added support to read/write config parameters from SCMDB/EFS
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

#define HDRAUTH_ATTRIBUTE_ID_LENGTH  8

/* Length in octets of the ACPAC header */
#define HDRAUTH_ACPAC_LEN            8

/* <EJECT> */
/*===========================================================================

                 DEFINITIONS  FOR MESSAGE PARSING
             
===========================================================================*/
#define HDRAUTH_CONFIG_REQ_MSG       0x50
#define HDRAUTH_CONFIG_RESP_MSG      0x51

/* <EJECT> */
/*===========================================================================

                        DATA  DECLARATIONS

===========================================================================*/
typedef struct
{
  /* Length, in bits, of the key signature for access probes */
  uint16 ac_auth_key_len;
} hdrauth_config_params_type;

/* <EJECT> */
/*===========================================================================

                      FUNCTION  DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION HDRAUTH_POWERUP_INIT

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
extern void hdrauth_powerup_init( void );

/*===========================================================================

FUNCTION HDRAUTH_CONFIG_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to AUTH in-config instance.

DEPENDENCIES
  None
  
PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern void hdrauth_config_msg_cb 
(
  dsm_item_type*  msg_ptr
);

/*===========================================================================

FUNCTION HDRAUTH_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to AUTH.

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
extern void hdrauth_ind_cb
(
  hdrind_ind_name_enum_type  ind_name,
  void*                      ind_data
);

/*===========================================================================

FUNCTION HDRAUTH_GET_PREVIOUS_SESSION_DATA

DESCRIPTION
  This function performs the same SHA-1 hash used for the access channel
  but uses the Nonce value passed in as the payload instead of the access 
  channel packet.

          ---------- -------------------- ----------------
         |Cryptosync|        ACPAC       |    Security    |
         |   Short  |                    |      Nonce     |
          ---------- -------------------- ----------------
              16              64                 32           
       
DEPENDENCIES
  None
 
PARAMETERS
  security_nonce        - The random number to considered payload of an 
                          access channel message.
  sec_packet_len_ptr    - The length in octets of the security layer packet
  sec_packet_ptr        - The security layer packet consisting of the given
                          payload with the ACPAC and CryptosyncShort headers
  kep_protocol_subtype  - Subtype of key exchange active in prev. session
  sec_protocol_subtype  - Subtype of security prot. active in prev. session
  auth_protocol_subtype - Subtype of auth prot. active in prev. session
  send_on_tc            - The created data is to be sent on traffic channel
  
  Note that the protocol subtypes are kept as part of the session 
  configuration so they need to be given with this command in order 
  for the security layer to know what subtypes to use in the computation.
                      
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void hdrauth_get_previous_session_data 
(
  uint32  security_nonce,
  uint8*  sec_packet_len_ptr, 
  uint8*  sec_packet_ptr,
  uint16  sec_protocol_subtype,
  uint16  auth_protocol_subtype,
  boolean send_on_tc
);

/*===========================================================================

FUNCTION HDRAUTH_SET_CONFIG_TO_DEFAULT

DESCRIPTION
  This function sets authentication protocol configuration parameters to 
  default. 
  
DEPENDENCIES
  None

PARAMETERS
  current_config - Pointer to authentication protocol's current configuration 
                   structure in SCMDB.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrauth_set_config_to_default
( 
  hdrauth_config_params_type *current_config 
);

/*===========================================================================

FUNCTION HDRAUTH_GET_INCONFIG_HANDLE

DESCRIPTION
  This function returns a pointer to inconfig instance of Authentication 
  Protocol

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  Pointer to inconfig instance of Authentication Protocol

SIDE EFFECTS
  None

===========================================================================*/
extern void* hdrauth_get_inconfig_handle( void );

#endif /* HDRAUTH_H */
