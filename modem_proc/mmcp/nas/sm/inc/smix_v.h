#ifndef SM_IX_V_H
#define SM_IX_V_H

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
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/sm/inc/smix_v.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $
            
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/26/12   am      Back off timer code
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
#include "mmcp_variation.h"
#include "smix.h"
#include "sm_v.h"
/* Defintions of fields used during Encode/decode operations */

#define SM_PD           0x0A

#define PD_LEN          0x4
#define TI_IND          0x4
#define TI_VALUE        0x3
#define MSG_TYPE        0x8
#define LLC_SAPI        0x8
#define RADIO_PRI       0x8
#define SPARE_LEN       0x4
#define PDP_TYPE_ORG    0x4
#define TIE_VALUE        0x7

#define BYTE_LEN             0x1
#define BYTE_LEN_INBITS      0x8
#define QOS_LENGTH           0x0C
#define INVALID_EXTENDED_TID 0x07
#define LINKED_TI_LENGTH     0x01

/* ---------------------------------------------------------------------------
* IEI definitions for Session Management Messages
* --------------------------------------------------------------------------*/

/* Note:  No TYPE 1 or 2 Information Element Indicators are present */

/* ---------------------------------------------------------------------------
* TYPE 3 and TYPE 4 INFORMATION ELEMENTS.
* --------------------------------------------------------------------------*/
#define IEI_REQUEST_LLC_SAPI            0x32
#define IEI_NEW_RADIO_PRIORITY          0x08
#define IEI_TI_INDICATOR                0x90
#define IEI_ACC_POINT_NAME              0x28
#define IEI_PROTOCOL_CFG_OPTIONS        0x27
#define IEI_PDP_ADDRESS                 0x2B
#define IEI_PACKET_FLOW_IDENT           0x34
#define IEI_TRAFFIC_FLOW_TEMPLATE       0x36
#define IEI_NEW_QOS                     0x30
#define IEI_NEW_TFT                     0x31
#define IEI_SM_CAUSE                    0x39
#define IE_NOT_INCLUDED                 0x00
#define IEI_GPRS_TIMER_3_IE             0x37
#define IEI_PDP_REQUEST_TYPE            0xA0
#ifdef FEATURE_MBMS
#error code not present
#endif
/* ---------------------------------------------------------------------------
* Minumum length of the sm-sm peer messages to check Mandatory fields.
* --------------------------------------------------------------------------*/
#define SM_ACTIVATE_PDP_CONTEXT_ACCEPT_LEN_REL99  16
#define SM_ACTIVATE_PDP_CONTEXT_ACCEPT_LEN_REL97  8

#define SM_ACTIVATE_PDP_CONTEXT_REJECT_LEN        3
#define SM_REQUEST_PDP_CONTEXT_ACTIVATION_LEN     5
#define SM_REQUEST_SEC_PDP_CONTEXT_ACTIVATION_LEN     17

#define SM_SEC_PDP_CONTEXT_ACC_LEN                16
#define SM_SEC_PDP_CONTEXT_ACT_REJ_LEN             3

#define SM_MODIFY_PDP_CONTEXT_REQUEST_LEN_REL99   16
#define SM_MODIFY_PDP_CONTEXT_REQUEST_LEN_REL97   8

#define SM_MODIFY_PDP_CONTEXT_ACCEPT_LEN          2
#define SM_MODIFY_PDP_CONTEXT_REJECT_LEN          3
#define SM_DEACTIVATE_PDP_CONTEXT_REQUEST_LEN     3
#define SM_DEACTIVATE_PDP_CONTEXT_ACCEPT_LEN      2

#ifdef FEATURE_MBMS
#error code not present
#endif

/* ---------------------------------------------------------------------------
* Maximum length of the individual Non-imperative IEs.
* --------------------------------------------------------------------------*/
#define IEI_PDP_ADDRESS_LEN             20
#define IEI_PROTOCOL_CFG_OPTIONS_LEN    253
#define IEI_NEW_RADIO_PRIORITY_LEN      1
#define IEI_TI_INDICATOR_LEN            1
#define IEI_ACC_POINT_NAME_LEN          102
#define IEI_PACKET_FLOW_IDENT_LEN       4
#define IEI_NEW_QOS_LEN                 13
#define IEI_NEW_TFT_LEN                 257

/* ---------------------------------------------------------------------------
* ADD_BYTE_LEN macro adds a byte length (8 bits) to the current position.
* --------------------------------------------------------------------------*/
#define ADD_BYTE_LEN(cur_pos) \
        (cur_pos += BYTE_LEN_INBITS)
/* ---------------------------------------------------------------------------
* Dynamically assigned PFI values should be 0x8 <= PFI <= 0x7F as per 
* Table 10.5.161/3GPP TS 24.008.
* If an invalid PFI has been allocated, use the Best Effort value "0"
* --------------------------------------------------------------------------*/
#define SM_VALIDATE_PFI( pfi )                  \
        {                                       \
          if( ((pfi) < 0x8) || ((pfi) > 0x7F) ) \
            (pfi) = 0;                          \
        }
/* ---------------------------------------------------------------------------
* Definitions to check the Valid range of the Message fields.
* --------------------------------------------------------------------------*/

/* ---------------------------------------------------------------------------
* Radio Priority.
* --------------------------------------------------------------------------*/
#define RADIO_PRI_LEVEL1 1
#define RADIO_PRI_LEVEL2 2
#define RADIO_PRI_LEVEL3 3
#define RADIO_PRI_LEVEL4 4

/* ---------------------------------------------------------------------------
* PDP ADDRESS ID fields.
* --------------------------------------------------------------------------*/

// PDP type organization

#define PDP_ETSI_TYPE  0x0 
#define PDP_IETF_TYPE  0x1




// Packet Flow ID

#define BEST_EFFORT 0
#define SIGNALING   1


/*===========================================================================
FUNCTION VALIDATE_LAYER3_MSG 

DESCRIPTION
  This function validates the Layer3 Message received from the Network. 
  It validates the Trasaction Id and the Protocol Identifier in the received
  SM-SM peer message.
  
DEPENDENCIES

  None.
  
RETURN VALUE
  returns the xlate_status_T value.

SIDE EFFECTS

===========================================================================*/
xlate_status_T validate_layer3_msg
(
  byte* ext_buf,        /* Buffer contains the received message */
  word* trans_id        /* Transaction Id returned after validation */
);

/*===========================================================================
FUNCTION UNPACK_LAYER3_MESG 

DESCRIPTION
  This function (decodes) unpacks a Layer3 message to be sent to GMM. 
  
DEPENDENCIES
  The validate_layer3_msg() function should be called before calling this.
  
RETURN VALUE
  returns the xlate_status_T value.

SIDE EFFECTS

===========================================================================*/
xlate_status_T unpack_layer3_mesg
(
  uint16  length,           /* Length of the received Layer3 message */
  byte* ext_buf,          /* Buffer contains the received message */
  pdp_context_info* pdp,  /* unpacked into the pdp context */
  sm_layer_msg_T* recd_msg  /* Received Message type */
);

/*===========================================================================
FUNCTION PACK_SM_STATUS_INFO

DESCRIPTION
  This function (encodes) packs a SM_STATUS message with the supplied
  transaction ID and cause.

DEPENDENCIES
  The validate_layer3_msg() function should be called before calling this.

RETURN VALUE
  returns the xlate_status_T value.

SIDE EFFECTS

===========================================================================*/
xlate_status_T pack_sm_status_info
(
  word* length,
  byte* ext_buf,
  word trans_id,
  word cause
);

#ifdef FEATURE_GPRS_CALLCONTROL

/* -----------------------------------------------------------------------
** FUNCTION     : PACK_PDP_ACTIVATE_MESG()

** DESCRIPTION  : Packs the ACTIVATE_PDP_REQUEST SM-SM peer message.
** ----------------------------------------------------------------------- */
extern xlate_status_T pack_pdp_activate_mesg
(
word* length,
byte* ext_buf,
pdp_context_info* pdp
);


/*===========================================================================
FUNCTION UNPACK_TFT 

DESCRIPTION
  This function (decodes) unpacks TFT.
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern xlate_status_T unpack_tft
(
  byte* ext_buf,
  uint16 *index,
  pdp_context_info* pdp
);
#endif

#endif
