#ifndef RRCASN1UTIL_H
#define RRCASN1UTIL_H
/*===========================================================================
                          R R C A S N 1 U T I L

DESCRIPTION
  This header file contains function prototypes for the functions in 
  rrcasn1util.h.

Copyright (c) 2001 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/OSSARM7ADS11/vcs/rrcasn1util.h_v   1.2   03 Oct 2001 19:26:24   rjeenaga  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcasn1util.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/31/13   sg      Made changes to optimize dec_ctxt_list
05/13/13   sg      Made changes to optimize dec_ctxt_list
04/05/13   sg      Made changes to revert the CR 458942 for dec_ctxt_list optimization
04/01/13   sg      Made changes to optimize dec_ctxt_list
01/03/13   sr      Made changes to remove the ASN functions used for encoding of SIBs and modified the
                   processing of CSFB system information container accordingly as part of memrory optimization
10/19/12   sg      Made changes for Rel 9 migration, removing unsupported IEs and ZI reductions. 
08/03/11   su      Added code to free SIB container’s decode context and 
                   add parameters to dec_ctxt_list[], to aid in debugging 
                   which PDU is leaking.
03/10/11   rl      Fixed the compiler warning
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
12/07/10   ss      Migrated CSFB code on Osys
11/03/10   rl      Updated with Osys migration changes
06/04/03   sk      Modified prototype for function  rrcasn1_free_pdu_with_buf
05/01/03   kc      Added prototypes for function rrcasn1_get_number_of_pad_bits 
                   which returns the number of padding bits in the last encoded 
                   message.
10/03/01   rj      Added prototypes for functions rrcasn1_free_pdu_with_buf
                   and rrcasn1_decode_pdu_with_buf. These functions are added
                   to fix memory leak.
08/30/01   rj      Moved prototype for function rrc_send_rrc_status_message
                   to RRCSEND module and deleted include files rrcdata.h and
                   rrcsend.h
06/18/01   jrs     Hid OSS' data structures.
04/18/01   rj      Updated to have the received message type and transaction Id
                   as formal parameters to the function rrc_send_rrc_status_message
                   instead of structure that contains both of them.
03/27/01   rj      Modified prototype for function rrc_send_rrc_status_message
                   to the latest rrcasn1.h and December specification
02/06/01   rj      Modified the interface to rrc_send_rrc_status_message
                   function and added RRC procedure and L2 ACK Status as
                   additional formal parameters to this function.
12/21/00   jrs     Updated the header file to match the C file.
12/13/00   jrs     Created file.

===========================================================================*/


/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "uecomdef.h"
#include "rrcasn1.h"
#include "rrcdata.h"


/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

typedef struct {
    long           length;
    unsigned char *value;
} pdu_buf_type;

/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

extern OSCTXT enc_ctxt;

typedef struct
{
  OSCTXT dec_context;
  boolean isfree;
  int pdu_num;              /* Added parameters pdu_num,state,msg_type
                                                   in order to debug which PDU is leaking and 
                                                   what state are we in*/
  rrc_state_e_type state;
  uint8 msg_type;
  void * dec_ptr_address;
}rrcasn1util_obj_ctxt_list_type;

#define RRC_MAX_NUM_ASN1_CTXT 40
extern rrcasn1util_obj_ctxt_list_type *dec_ctxt_list;


/* =======================================================================
**                          Macro Definitions
** ======================================================================= */



/* =======================================================================
**                        Function Declarations
** ======================================================================= */


/*===========================================================================

FUNCTION RRCASN1_ENCODE_PDU

DESCRIPTION
  This function ASN.1 encodes a passed signalling message. Note that space for
  the encoded message is dynamically allocated and must be freed by calling 
  rrcasn1_free_buf().

DEPENDENCIES
  The memory manager and the ASN.1 subsystem must have been previously 
  initialized.

RETURN VALUE
  This function returns a pdu_buf_type. If encoding failed, pdu_buf.value will
  be NULL and pdu_buf.length is meaningless. If encoding succeeded, 
  pdu_buf.value will be non-NULL and pdu_buf.length will contain the number 
  of bytes in the encoded PDU.

SIDE EFFECTS
  None.

===========================================================================*/

pdu_buf_type rrcasn1_encode_pdu
(
  void *unencoded_pdu_ptr,    // The pointer to the PDU to encode
  int pdu_num                 // The PDU number to encode
);
/*===========================================================================

FUNCTION rrc_free_dec_ctxt_list

DESCRIPTION
  This function frees the memory allocated for dec_ctxt_list.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None.
  
===========================================================================*/
void rrc_free_dec_ctxt_list(void);


/*===========================================================================

FUNCTION RRCASN1_DECODE_PDU

DESCRIPTION
  This function ASN.1 decodes a passed signalling message. Note that the
  decoded PDU is dynamically allocated and needs to be freed by calling 
  rrcasn1_free_pdu().

DEPENDENCIES
  The memory_manager and the ASN.1 subsystem must have been previously 
  initialized.

RETURN VALUE
  NULL if an error occurred during decoding, otherwise a pointer to the
  decoded PDU.

SIDE EFFECTS
  None.

===========================================================================*/

void *rrcasn1_decode_pdu
(  
  void *encoded_pdu_ptr,      // The pointer to the PDU to decode
  int pdu_num,                // The PDU number to decode
  uint32 encoded_pdu_size     // The size (in bytes) of the PDU
);


/*===========================================================================

FUNCTION RRCASN1_DECODE_PDU_WITH_BUF

DESCRIPTION
  This function ASN.1 decodes a passed signalling message. Note that the
  decoded PDU is not dynamically allocated and needs to be freed by calling 
  rrcasn1_free_pdu_with_buf().

DEPENDENCIES
  The memory_manager and the ASN.1 subsystem must have been previously 
  initialized.

RETURN VALUE
  NULL if an error occurred during decoding, otherwise a pointer to the
  decoded PDU.

SIDE EFFECTS
  None.
===========================================================================*/

void *rrcasn1_decode_pdu_with_buf
(  
  void *encoded_pdu_ptr,      // The pointer to the PDU to decode
  int pdu_num,                // The PDU number to decode
  uint32 encoded_pdu_size,     // The size (in bytes) of the PDU
  uint32 length
);
/*===========================================================================

FUNCTION RRCASN1_FREE_BUF

DESCRIPTION
  This function frees memory that was allocated by rrcasn1_encode_pdu().

DEPENDENCIES
  The memory_manager and the ASN.1 subsystem must have been previously 
  initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void rrcasn1_free_buf
(
  void *buf                 // Pointer to the buffer to free
);


/*===========================================================================

FUNCTION RRCASN1_FREE_PDU

DESCRIPTION
  This function frees memory that was allocated by rrcasn1_decode_pdu().

DEPENDENCIES
  The memory_manager and the ASN.1 subsystem must have been previously 
  initialized.

RETURN VALUE
  Non-zero if an error occurred; else zero.

SIDE EFFECTS
  None.

===========================================================================*/

int rrcasn1_free_pdu
(
  int pdunum,               // The PDU number to be freed
  void *buf                 // The PDU to be freed
);

 uint32  rrc_enc_UE_RadioAccessCapabilityInfo_PDU(
    void * unencoded_pdu_ptr,
    rrc_InterRATHandoverInfo_present * lbitstr);


uint8 rrc_get_free_asn1_ctxt_idx
(
void
);

void rrc_init_asn1_ctx
(
  void
);

void rrc_free_asn1_ctxt_idx
(
  uint8 index
);

uint8 rrc_get_used_ctxt_idx_for_asn1_ptr
(
  void * ptr_adress
);




#endif /* RRCASN1UTIL_H */
