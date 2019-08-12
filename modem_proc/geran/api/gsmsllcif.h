/*****************************************************************************
***
*** TITLE
***
***  GPRS GSMS -> LLC MESSAGE INTERFACE
***
***
*** DESCRIPTION
***
***  Defines the structure types for implementation of the applicable subset
***  of LL Service Primitives which are permissable on LL SAPI 7 (GSMS).
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/gsmsllcif.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 10-04-01   tjw     Add customer.h and comdef.h to all SU API files
*** mm/dd/yy   xyz     changes
***
*****************************************************************************/

#ifndef INC_GSMSLLCIF_H
#define INC_GSMSLLCIF_H


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#ifndef CUSTOMER_H
  #include "customer.h"
#endif

#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "sys.h"

/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/

/* GSMS -> LLC LL service primitive message identifier definition. */

typedef enum
{
  GSMSLLC_LL_UNITDATA_REQ = 0x40,
  GSMSLLC_LL_XID_REQ      = 0x41,
  GSMSLLC_LL_XID_RES      = 0x42

} gsmsllc_ll_prim_e_type;

/* ======================================================================== */

/* LL-UNITDATA-REQ message type definition. Unacknowledged data transmission
   request. */

typedef struct
{
  gsmsllc_ll_prim_e_type  prim;           /* GSMSLLC_LL_UNITDATA_REQ        */

  uint32                  tlli;           /* Temporary Logical Link Id      */
  boolean                 ciphered;       /* TRUE if data is to be ciphered */
  uint8                   radio_priority; /* SMS Radio Priority Level       */
  uint8                   *l3_pdu;        /* Ptr to PDU in heap alloc mem   */
  uint16                  l3_pdu_len;     /* Len of PDU in heap alloc mem   */

//ifdef FEATURE_DUAL_SIM
  sys_modem_as_id_e_type  as_id;          /* GERAN access stratum identity  */
//endif /* FEATURE_DUAL_SIM */

} gsmsllc_ll_unitdata_req_type;

/* ======================================================================== */

/* LL-XID-REQ message type definition. eXchange of IDentification
   information request message conveying a request for XID negotiation
   exchange packet to the peer GSMS entity.

   NOTE: Not expected to be used, but here for completeness. */

typedef struct
{
  gsmsllc_ll_prim_e_type  prim;           /* GSMSLLC_LL_XID_REQ             */

  uint32                  tlli;           /* Temporary Logical Link Id      */
  uint8                   *l3_xid_req;    /* Ptr to req in heap alloc mem   */
  uint16                  l3_xid_req_len; /* Len of req in heap alloc mem   */

//ifdef FEATURE_DUAL_SIM
  sys_modem_as_id_e_type  as_id;          /* GERAN access stratum identity  */
//endif /* FEATURE_DUAL_SIM */

} gsmsllc_ll_xid_req_type;

/* ======================================================================== */

/* LL-XID-RES message type definition. eXchange of IDentification
   information response message conveying a negotiated XID response packet
   to the peer GSMS entity.

   NOTE: Not expected to be used, but here for completeness. */


typedef struct
{
  gsmsllc_ll_prim_e_type  prim;           /* GSMSLLC_LL_XID_RES             */

  uint32                  tlli;           /* Temporary Logical Link Id      */
  uint8                   *l3_xid_neg;    /* Ptr to neg in heap alloc mem   */
  uint16                  l3_xid_neg_len; /* Len of neg in heap alloc mem   */

//ifdef FEATURE_DUAL_SIM
  sys_modem_as_id_e_type  as_id;          /* GERAN access stratum identity  */
//endif /* FEATURE_DUAL_SIM */

} gsmsllc_ll_xid_res_type;

/* ======================================================================== */


/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/



/*****************************************************************************
***
***     Public Data References
***
*****************************************************************************/



/*****************************************************************************
***
***     Public Macros
***
*****************************************************************************/



/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/


#endif /* INC_GSMSLLCIF_H  */

/*** EOF: don't remove! ***/
