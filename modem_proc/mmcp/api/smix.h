#ifndef SM_IX_H
#define SM_IX_H

/*===========================================================================
                        S M    I X  HEADER FILE

DESCRIPTION

   Definition for packing(encode)/unpacking(decode) Session Management 
   Layer 3 messages between the SM-SM peer entities of the Mobile 
   station and the Network.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000, 2001 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/
 
/*===========================================================================

            EDIT HISTORY FOR FILE

$PVCSPath:  L:/src/asw/MSM5200/sm/vcs/smix.h_v   1.3   24 May 2001 13:48:44   kishores  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/smix.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $
            
when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/18/06   rk       Added Extended TI Feature Code
07/24/06  rk        Added prototype declaration for unpack_pdp_activate_mesg, 
                           unpack_sec_pdp_activate_mesg and unpack_tft functions.
07/03/06  ss        Added GPRS call control fixes
03/24/06   ss       Dynamic PFI validation added
04/15/05   ss       Added enum for decode parse results.
10/18/04   kvk     MT PDP support added.
09/02/04   kvk     New macros LINKED_TI_LENGTH, SM_SEC_PDP_CONTEXT_ACC_LEN and
                   SM_SEC_PDP_CONTEXT_ACT_REJ_LEN defined for sec PDP
08/13/03   mks     Added definition for INVALID_EXTENDED_TID.
12/02/02   ks      Removed the FEATURE_GSM_GPRS_SM_NETWORK_REVISION_SWITCH as
                   it is no longer needed. 
09/27/02   ks      define FEATURE_GSM_GPRS_SM_NETWORK_REVISION_SWITCH so that
                   UE can pack the QOS information according the MS rev number.  
05/23/01   ks      Definitions to check the Valid range of the Message fields.
05/17/01   ks      Added definitions for SM-SM peer message lengths and
                   minimum lenghts for the various IEs in SM.
02/16/01   ks      Created file.
            
===========================================================================*/
#include "sm.h"

/* Defintions of fields used during Encode/decode operations */

// PDP type number

#define PPP_TYPE  0x1
#define IPV4_TYPE 0x21
#define IPV6_TYPE 0x57 

#define IPV4V6_TYPE 0x8D 






/*===========================================================================
FUNCTION PACK_LAYER3_MESG 

DESCRIPTION
  This function (encodes) packs a Layer3 message to be sent to GMM. 
  The Layer 3 messages are defined in Section 9.5 of TS 24.008 standard.
  
DEPENDENCIES

  None.
  
RETURN VALUE:
  returns the xlate_status_T value.

SIDE EFFECTS

===========================================================================*/
xlate_status_T pack_layer3_mesg
(
  word* length,           /* Length in bits of the Packed Layer3 message */
  byte* ext_buf,          /* Buffer contains the packed message */
  pdp_context_info* pdp,  /* PDP context information, used to pack the msg */
  sm_layer_msg_T msg_type /* Type of L3 message being packed */
);
/*===========================================================================
FUNCTION UNPACK_PDP_ACTIVATE_MESG 

DESCRIPTION
  This function (decodes) unpacks ACTIVATE_PDP_Request sent to Network.
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern xlate_status_T unpack_pdp_activate_mesg
(
  uint16 msg_length, 
  byte* ext_buf, 
  pdp_context_info *pdp
);

/*===========================================================================
FUNCTION UNPACK_SEC_PDP_ACTIVATE_MESG 

DESCRIPTION
  This function (decodes) unpacks SECONDARY ACTIVATE_PDP_Request sent to Network.
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern xlate_status_T unpack_sec_pdp_activate_mesg
(
  word msg_length, 
  byte* ext_buf, 
  pdp_context_info *pdp
);


#endif
