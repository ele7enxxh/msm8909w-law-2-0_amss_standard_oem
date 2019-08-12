#ifndef TC_PARSE_H
#define TC_PARSE_H

/*===========================================================================
                        T C  P A R S E  HEADER FILE

DESCRIPTION

   Definition for packing(encode)/unpacking(decode) Test Control Management 
   Layer 3 messages between the TC-TC peer entities of the Mobile 
   station and the Network.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000, 2001 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/
 
/*===========================================================================

            EDIT HISTORY FOR FILE

$PVCSPath:  L:/src/asw/MSM5200/ctest/vcs/tcparse.h_v   1.0   06 Feb 2002 17:59:50   madiraju  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/tc/src/tcparse.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $
            
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/24/01   ks      Created file.
            
===========================================================================*/
#include "tc.h"

/* Defintions of fields used during Encode/decode operations */

#define TC_PD           0x0F

/* ---------------------------------------------------------------------------
* Minumum length of the received TC layer3 message to check Mandatory fields.
* --------------------------------------------------------------------------*/
#define TC_CLOSE_UE_TEST_LOOP_LEN      3
#define TC_OPEN_UE_TEST_LOOP_LEN       2
#define TC_ACTIVATE_RB_TEST_MODE_LEN   2
#define TC_DEACTIVATE_RB_TEST_MODE_LEN 2

/* ---------------------------------------------------------------------------
* Enumeration for encode/decode L3 message return status.
* --------------------------------------------------------------------------*/
typedef enum
{
  L3_PARSE_SUCCESS,
  L3_PARSE_INVALID_PD_TI,
  L3_INVALID_MANDATORY_INFO,
  L3_UNSUPPORTED_MSG
} xlate_status_T;

/*===========================================================================
FUNCTION VALIDATE_LAYER3_MSG 

DESCRIPTION
  This function validates the Layer3 Message received from the Network. 
  It validates the Protocol Identifier and Mandatory IEs in the received
  TC-TC peer message.
  
DEPENDENCIES

  None.
  
RETURN VALUE
  returns the xlate_status_T value.

SIDE EFFECTS

===========================================================================*/
xlate_status_T validate_layer3_msg
(
  byte* ext_buf        /* Buffer contains the received message */
);

/*===========================================================================
FUNCTION PACK_LAYER3_MESG 

DESCRIPTION
  This function encodes (packs) a Layer3 TC message to be sent to GMM. 
  
DEPENDENCIES
  
RETURN VALUE
  returns the xlate_status_T value.

SIDE EFFECTS

===========================================================================*/
xlate_status_T unpack_layer3_mesg
(
  byte * ext_buf,           /* Buffer contains the packed TC message */
  tc_layer_msg_T* recd_msg  /* Received Message type */
);

#endif
