/*****************************************************************************
***
*** TITLE
***
***  GPRS LLC -> GSMS MESSAGE INTERFACE
***
***
*** DESCRIPTION
***
***  Defines the structure types for implementation of the applicable subset
***  of LL Service Primitives which are permissable on LL SAPI 7 (GSMS).  
***
*** 
*** Copyright (c) 2002 QUALCOMM UK Limited. 
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/gllcsmsif.h#1 $   
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** mm/dd/yy   xyz       changes
***
*****************************************************************************/

#ifndef INC_GLLCSMSIF_H
#define INC_GLLCSMSIF_H


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "customer.h" /* This include must be first before #ifdef FEATURE_  */
#include "comdef.h"
#include "sys_stru.h"


/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/

/* LLC -> GMM LLGMM and LL service primitive message identifier definition. */

typedef enum
{
  GLLCSMS_LL_UNITDATA_IND = 0xC0,           /* gllcsms_ll_unitdata_ind_type */
  GLLCSMS_LL_XID_IND      = 0xC1,           /* gllcsms_ll_xid_ind_type      */
  GLLCSMS_LL_XID_CNF      = 0xC2,           /* gllcsms_ll_xid_cnf_type      */
  GLLCSMS_LL_RESET_IND    = 0xC3,           /* gllcsms_ll_reset_ind_type    */
  GLLCSMS_LL_STATUS_IND   = 0xC4            /* gllcsms_ll_status_ind_type   */

} gllcsms_prim_e_type;

/* ======================================================================== */

/* LL-UNITDATA-IND message type definition. Unacknowledged data receipt
   indication. */

typedef struct
{
  IMH_T                    hdr;               /* Inter-task message header             */

  uint32                   tlli;              /* Temporary Logical Link Identifier     */
  boolean                  ciphered;          /* TRUE if PDU was received ciphered     */
  uint8                    *l3_pdu;           /* Pointer to PDU in heap allocated mem  */ 
  uint16                   l3_pdu_len;        /* Length of PDU in heap allocated mem   */
  sys_modem_as_id_e_type   as_id;

} gllcsms_ll_unitdata_ind_type;

/* ======================================================================== */

/* LL-XID-IND message type definition. eXchange of Identification
   information indication message conveying a request for XID negotiation
   exchange packet from the peer GMM entity. Request packet pointer is NULL
   if message is used to indicate a change in the maximum unacknowledged
   PDU size only.
   
   NOTE: Not expected to be used, but here for completeness. */

typedef struct
{
  IMH_T                    hdr;               /* Inter-task message header             */

  uint32                   tlli;              /* Temporary Logical Link Identifier     */
  uint8                    *l3_xid_req;       /* Ptr to XID Req Pkt in heap alloc mem  */
  uint16                   l3_xid_req_len;    /* Length of XID Req Pkt in heap mem     */
  uint16                   n201_u;            /* Current maximum unack PDU size        */

  sys_modem_as_id_e_type   as_id;

} gllcsms_ll_xid_ind_type;

/* ======================================================================== */

/* LL-XID-CNF message type definition. eXchange of IDentification
   information confirmation message conveying an XID negotiated response
   packet from the peer GMM entity.

   NOTE: Not expected to be used, but here for completeness. */

typedef struct
{
  IMH_T                    hdr;               /* Inter-task message header             */

  uint32                   tlli;              /* Temporary Logical Link Identifier     */
  uint8                    *l3_xid_neg;       /* Ptr to XID Neg Pkt in heap alloc mem  */
  uint16                   l3_xid_neg_len;    /* Length of XID Req Pkt in heap mem     */
  uint16                   n201_u;            /* Current maximum unack PDU size        */

  sys_modem_as_id_e_type   as_id;

} gllcsms_ll_xid_cnf_type;

/* ======================================================================== */

/* LL-RESET-IND message type definition. Reset order from SGSN.

   NOTE: Not expected to be used, but here for completeness. */

typedef struct
{
  IMH_T                    hdr;          /* Inter-task message header             */

  uint32                   tlli;         /* Temporary Logical Link Identifier     */

  sys_modem_as_id_e_type   as_id;

} gllcsms_ll_reset_ind_type;


/* ======================================================================== */

/* LL-STATUS-IND message 'cause' parameter type definition. */

typedef enum
{
  GLLCSMS_LL_NO_PEER_RESPONSE = 0x0, /* LLC peer LLE is not responding.     */
  GLLCSMS_LL_INVALID_XID_RESP = 0x1  /* XID negotiation response invalid.   */

} gllcsms_status_cause_e_type;

/* ------------------------------------------------------------------------ */

/* LL-STATUS-IND message type definition - LLC status condition indication
   relating to LL SAP 1 (GMM's logical channel). */

typedef struct
{
  IMH_T                        hdr;    /* Inter-task message header         */

  uint32                       tlli;   /* Temporary Logical Link Id         */
  gllcsms_status_cause_e_type  cause;  /* LLC status indication cause id    */

  sys_modem_as_id_e_type       as_id;

} gllcsms_ll_status_ind_type;

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


#endif /* INC_GLLCSMSIF_H */

/*** EOF: don't remove! ***/
