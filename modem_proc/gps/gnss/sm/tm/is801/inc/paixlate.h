#ifndef PAIXLATE_H
#define PAIXLATE_H

/*===========================================================================

            I S  8 0 1   T R A N S L A T I O N    S E R V I C E S    
                    D A T A    D E C L A R A T I O N S 

DESCRIPTION
  This file contains the data structures needed for use in the
  IS 801 translation services.
  

  Copyright (c) 2000 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/is801/inc/paixlate.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/15/06   ank     Changed prototype of paix_xlate_int_msid_hdr, paix_xlate_int_prdm_bsp_resp.
10/11/06   br      Removed feature GPSONE and added proper featurization.
05/24/05   cl      Make CLM a NV item
04/18/05   sk	   Moved macro from paix.c to paix.h
08/14/03   anr     Added feature to send Proprietary IS801 Base Lat/Long Msg.
06/04/03   ar      Added translation for SCU messages
07/15/02   ak      Added function to translate a AA response 
                   record from internal to external format.
12/19/02    sk     Mainlined lr translation function.
12/12/01    SK     Mod's for MSTO.
11/29/01   aar     Added a function to xlate MSTO resp msg from int to ext.
06/27/01   dd      Added structures for translating GPS Almanac/Ephemeris 
                   requests and responses.
06/07/01   mai 	   Moved a couple of macros under FEATURE_GPS_MODE_IS801.
02/26/01   mai     Added FEATURE_GPS_MODE_IS801 wrapper around GPS parts.
09/14/00   mai     Added #defines for values used in custom header encoding.
06/01/00   mai     Updated macros for correct handling of bit to byte and 
                   byte to bit conversion. 
05/19/00   mai     Made macros for converting from bits to bytes and vice 
                   versa.
05/11/00   mai     Cleaned up code.
05/03/00   mai     Created module.

===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"
#include "comdef.h"               /* Basic definitions */
#include "customer.h"

#include "tm_is801.h"
#include "tm_is801_i.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#define MIN_PDDM_LENGTH     3 /* size of PDDM HDR */

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                  PAI MESSAGE TYPES                                      */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* Reserved Byte Value */
#define PAI_RESERVED_VAL 0x00

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                  IS-801 PDE REQUEST TYPES                               */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
typedef enum 
{
  PDE_RESERVED_REQ_CODE         = 0x00,
  LOC_RESP_PDE_REQ_CODE         = 0x01,
  MS_INFO_REQ_CODE              = 0x02,
  AUTO_MEAS_WGHT_FACTR_REQ_CODE = 0x03, 
  PR_MEAS_REQ_CODE              = 0x04,
  PPM_MEAS_REQ_CODE             = 0x05,
  TIME_OFFSET_MEAS_REQ_CODE     = 0x06,
  CANCEL_REQ_CODE                = 0x07,  
  MAX_PDE_TO_MS_REQ_CODE = CANCEL_REQ_CODE
} pde_to_ms_req_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                  IS-801 MS REQUEST TYPES                                */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
typedef enum 
{
  MS_RESERVED_REQ_CODE   = 0x00,
  LOC_RESP_REQ_CODE      = 0x01,
  BS_CAP_REQ_CODE        = 0x02,
  BS_ALM_REQ_CODE        = 0x03,
  GPS_AA_REQ_CODE        = 0x04,
  GPS_SA_REQ_CODE        = 0x05,
  GPS_LA_S_REQ_CODE      = 0x06,
  MS_RESERVED_REQ_CODE_2 = 0x07,
  GPS_ALM_REQ_CODE       = 0x08,
  GPS_EPH_REQ_CODE       = 0x09,
  GPS_NAV_MSG_REQ_CODE   = 0x0a,
  GPS_ALM_COR_REQ_CODE   = 0x0b,
  GPS_SAT_HLTH_REQ_CODE  = 0x0c,
  MAX_MS_TO_PDE_REQ_CODE = GPS_SAT_HLTH_REQ_CODE
} ms_to_pde_req_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                  IS-801 MS REQUEST LENGTHS  (in bytes)                  */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
#define AA_REQ_LEN    1
#define SA_REQ_LEN    0
#define LR_REQ_LEN    1                                         
#define ALM_REQ_LEN   0
#define EPH_REQ_LEN   1

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                       CONSTANTS                                         */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
#define ADD_PILOTS_INCL     0
#define MSID_TYPE_IMSI      2

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                       MACROS                                            */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/* Macro to compute the number of bits that a given data structure is */
#define NUM_BITS_IN_BYTE(x) (8*sizeof(x))

/* Macro to pad the data structure so that one can pre-determine how many 
   bytes the data structure will take along with the RESERVED field */
#define BYTE_PAD    (NUM_BITS_IN_BYTE(byte) - 1)

/* Macro to compute the number of bytes that a given number of bits will 
   occupy */
#define NUM_BYTES_IN_BIT(x) (((x)+BYTE_PAD)/8)

#define RES_SIZE(x) ((x % NUM_BITS_IN_BYTE(byte)) != 0 ? \
        (NUM_BITS_IN_BYTE(byte) - (x % NUM_BITS_IN_BYTE(byte))) : 0)

  #define DR_SIZE_MULTIPLIER  2/* Unit size DR_SIZE corresponds to (in bits) */

  /* Macro to compute the Data Record size (in bytes) in a Sensitivity 
     Assistance message */
  #define DR_SIZE_IN_BYTES(x)  ((((x) * DR_SIZE_MULTIPLIER)+BYTE_PAD)/NUM_BITS_IN_BYTE(byte))

extern boolean paix_incl_msto;

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/                       

/*===========================================================================

FUNCTION XLATE_EXT_PDDM_HDR

DESCRIPTION
  This function translates a PDDM message header from external to internal 
  format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/

extern boolean paix_xlate_ext_pddm_hdr
(
  byte *buf_ptr,
    /* Pointer to received message to be translated */
  is801_pddm_hdr_type *int_ptr
    /* Pointer to place where translated message should be placed */
);


/*===========================================================================

FUNCTION XLATE_EXT_REQ_HEADER

DESCRIPTION
  This function translates a PDDM request element header from external to 
  internal format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/

extern boolean paix_xlate_ext_req_header
(
  byte *buf_ptr,
  paii_gen_req_hdr *int_ptr,
  uint16 length /* length of the buf_ptr */
);

/*===========================================================================

FUNCTION XLATE_EXT_RESP_HEADER

DESCRIPTION
  This function translates a PDDM response element header from external to 
  internal format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean paix_xlate_ext_resp_header
(
  byte *buf_ptr,
  paii_gen_resp_hdr *int_ptr,
  uint16 length
);

/*===========================================================================

FUNCTION PAIX_XLATE_EXT_REQ_PAYLOAD

DESCRIPTION
  This function translates external PDDM request payload to internal format

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean paix_xlate_ext_req_payload
(
  byte                      *buf_ptr,
  is801_fwd_link_req_s_type *req_msg_ptr,
  uint16                     length /* length of bytes in buf_ptr */
);

/*===========================================================================

FUNCTION PAIX_XLATE_EXT_RESP_PAYLOAD

DESCRIPTION
  This function translates external PDDM response payload to internal format

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean paix_xlate_ext_resp_payload
(
  byte                             *start,
  is801_fwd_link_resp_part_s_type  *resp_par_ptr,  
  byte                              resp_par_length,
  uint16                            length /* length of bytes in buf_ptr */
);

/*===========================================================================
  
FUNCTION XLATE_INT_AA_REQ
  
DESCRIPTION
  This function translates a Acquisition Assistance request record from 
  internal to external format.
  
DEPENDENCIES
  None
  
RETURN VALUE
  Errors that were encountered while translating the message.
  
SIDE EFFECTS
  None
  
===========================================================================*/  
extern boolean paix_xlate_int_aa_req
(
  byte              *buf_ptr,
  is801_aa_req_type *int_ptr,
  word              *msg_length_ptr /* in bits */         
);

/*===========================================================================
  
FUNCTION XLATE_INT_SA_REQ
  
DESCRIPTION
  This function translates a Sensitivity Assistance request record from 
  internal to external format.
  
DEPENDENCIES
  None
 
RETURN VALUE
  Errors that were encountered while translating the message.
  
SIDE EFFECTS
  None
  
===========================================================================*/  
extern boolean paix_xlate_int_sa_req
(
  byte              *buf_ptr,
  word              *msg_length_ptr /* in bits */         
);

/*===========================================================================

FUNCTION XLATE_INT_LR_REQ

DESCRIPTION
  This function translates a Location Response request record from internal 
  to external format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean paix_xlate_int_lr_req
(
  byte              *buf_ptr,
  is801_lr_req_type *int_ptr,
  word              *msg_length_ptr /* in bits */         
);

/*===========================================================================
  
FUNCTION XLATE_INT_ALM_REQ
  
DESCRIPTION
  This function translates a GPS Almanac request record from internal to
  external format.
  
DEPENDENCIES
  None
  
RETURN VALUE
  Errors that were encountered while translating the message.
  
SIDE EFFECTS
  None
  
===========================================================================*/
extern boolean paix_xlate_int_alm_req
(
  byte              *buf_ptr,
  word              *msg_length_ptr /* in bits */         
);

/*===========================================================================
  
FUNCTION XLATE_INT_EPH_REQ
  
DESCRIPTION
  This function translates a GPS Ephemerise request record from internal to 
  external format.
  
DEPENDENCIES
  None
  
RETURN VALUE
  Errors that were encountered while translating the message.
  
SIDE EFFECTS
  None
  
===========================================================================*/
extern boolean paix_xlate_int_eph_req
(
  byte               *buf_ptr,
  is801_eph_req_type *int_ptr,
  word               *msg_length_ptr /* in bits */         
);

/*===========================================================================

FUNCTION XLATE_INT_PDDM_HDR

DESCRIPTION
  This function translates a Position Determination Data Message header from 
  internal to external format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean paix_xlate_int_pddm_hdr
(
  byte                *buf_ptr,
  is801_pddm_hdr_type *int_ptr
);

/*===========================================================================

FUNCTION XLATE_INT_REJ_RESP

DESCRIPTION
  This function translates a Reject response record from internal to external 
  format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean paix_xlate_int_rej_resp
(
  byte                       *buf_ptr,
  is801_rev_link_resp_s_type *int_ptr,
  word                       *msg_length_ptr /* in bits */  
);

/*===========================================================================

FUNCTION XLATE_INT_CACK_RESP

DESCRIPTION
  This function translates a Provide Cancellation Acknowledgement response 
  record from internal to external format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean paix_xlate_int_cack_resp
(
  byte                       *buf_ptr,
  is801_rev_link_resp_s_type *int_ptr,
  word                       *msg_length_ptr /* in bits */    
);

/*===========================================================================

FUNCTION XLATE_INT_MSI_RESP

DESCRIPTION
  This function translates a Provide MS Information response record from 
  internal to external format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean paix_xlate_int_msi_resp
(
  byte                       *buf_ptr,
  is801_rev_link_resp_s_type *int_ptr,
  word                       *msg_length_ptr /* in bits */    
);

/*===========================================================================

FUNCTION XLATE_INT_TOM_RESP

DESCRIPTION
  This function translates a Provide Time Offset Measurement response 
  record from internal to external format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean paix_xlate_int_tom_resp
(
  byte                       *buf_ptr,
  is801_rev_link_resp_s_type *int_ptr,
  word                       *msg_length_ptr /* in bits */    
);

/*===========================================================================

FUNCTION XLATE_INT_LR_RESP

DESCRIPTION
  This function translates a Location response 
  record from internal to external format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean paix_xlate_int_lr_resp
(
  byte                       *buf_ptr,
  is801_rev_link_resp_s_type *int_ptr,
  word                       *msg_length_ptr /* in bits */    
);

/*===========================================================================

FUNCTION XLATE_INT_PPM_PART

DESCRIPTION
  This function translates a Provide Pilot Phase Measurements response record 
  from internal to external format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean paix_xlate_int_ppm_part
(
  byte                            *buf_ptr,
  is801_rev_link_resp_s_type      *int_ptr,
  is801_ppm_fragment_info_s_type  *ppm_fragment_info_ptr,  
  word                            *msg_length_ptr /* in bits */    
);

/*===========================================================================
  
FUNCTION XLATE_INT_PRM_PART
  
DESCRIPTION
  This function translates a Provide GPS Pseudorange response record from 
  internal to external format.
  
DEPENDENCIES
  None
  
RETURN VALUE
  Errors that were encountered while translating the message.
  
SIDE EFFECTS
  None
  
===========================================================================*/  
extern boolean paix_xlate_int_prm_part
(
  byte                            *buf_ptr,
  is801_rev_link_resp_s_type      *int_ptr,
  is801_prm_fragment_info_s_type  *prm_fragment_info_ptr,    
  word                            *msg_length_ptr /* in bits */    
);
                           
/*===========================================================================

FUNCTION XLATE_INT_MSID_HDR

DESCRIPTION
  This function translates a MSID header for Position Determination from 
  internal to external format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean paix_xlate_int_msid_hdr
(
  byte               *buf_ptr,
  paii_msid_hdr_type *int_ptr,
  word               *msg_length_ptr /* in bits */  
);


/*===========================================================================

FUNCTION PAIX_XLATE_INT_PRDM_BSP_RESP

DESCRIPTION
  This function translates a proprietary BS System Parameter response 
  record from internal to external format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean paix_xlate_int_prdm_bsp_resp
(
  byte                       *buf_ptr,
  is801_rev_link_resp_s_type *int_ptr,
  word                       *msg_length_ptr /* in bits */    
);

#endif /* PAIXLATE_H */
