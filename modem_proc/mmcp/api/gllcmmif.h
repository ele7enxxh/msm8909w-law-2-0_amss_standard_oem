/*****************************************************************************
***
*** TITLE
***
***  GPRS LLC -> GMM MESSAGE INTERFACE
***
***
*** DESCRIPTION
***
***  Defines the structure types for implementation of the LLGMM Service
***  Primitives for transmission to GMM by LLC. Also defines the structure
***  types for implementation of the applicable subset of LL Service
***  Primitives which are permissable on LL SAPI 1 (GMM).  
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/gllcmmif.h#1 $   
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** mm/dd/yy   xyz       changes
***
*****************************************************************************/

#ifndef INC_GLLCMMIF_H
#define INC_GLLCMMIF_H


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

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
  GLLCMM_LLGMM_STATUS_IND    = 0x80,       /* gllcmm_llgmm_status_ind_type  */
  GLLCMM_LLGMM_TRIGGER_IND   = 0x81,       /* gllcmm_llgmm_trigger_ind_type */
  GLLCMM_LLGMM_RDY_TMR_IND   = 0x82,       /* gllcmm_llgmm_rdy_tmr_ind_type */
  GLLCMM_LL_UNITDATA_IND     = 0x83,       /* gllcmm_ll_unitdata_ind_type   */
  GLLCMM_LL_XID_IND          = 0x84,       /* gllcmm_ll_xid_ind_type        */
  GLLCMM_LL_XID_CNF          = 0x85,       /* gllcmm_ll_xid_cnf_type        */
  GLLCMM_LL_RESET_IND        = 0x86,       /* gllcmm_ll_reset_ind_type      */
  GLLCMM_LL_STATUS_IND       = 0x87,       /* gllcmm_ll_status_ind_type     */
  GLLCMM_LLGMM_SUSPEND_CNF   = 0x88,       /* gllcmm_llgmm_suspend_cnf_type */
  GLLCMM_LLGMM_NEW_TLLI_IND  = 0x89        /* gllcmm_llgmm_new_tlli_ind_type*/
  
  ,GLLCMM_LLGMM_PSHO_CNF  = 0x8A          /* gllcmm_llgmm_psho_cnf_type*/
  

} gllcmm_prim_e_type;

/* ======================================================================== */

/* LLGMM-STATUS-IND message 'cause' parameter type definition. */

typedef enum
{
  GLLCMM_NO_PEER_RESPONSE    = 0x0, /* No response from peer LLE.           */ 
  GLLCMM_INVALID_XID_RESP    = 0x1, /* XID negotiation response invalid.    */
  GLLCMM_MULTIPLE_TLLI_USERS = 0x2, /* Unsolicited response rxd from peer   */
  GLLCMM_FRAME_REJECT_COND   = 0x3, /* Frame reject condition has occurred  */
  GLLCMM_FRAME_REJECT_RESP   = 0x4, /* Frame reject response rxd from peer  */
  GLLCMM_L2_REESTABLISHMENT  = 0x5, /* Layer 2 initiated re-establishment   */
  GLLCMM_L2_TX_SUCCESS       = 0x6, /* NAS Pdu successfully transmitted     */
  GLLCMM_L2_TX_FAILURE       = 0x7  /* NAS Pdu transmission failed          */
} gllcmm_status_cause_e_type;

/* ------------------------------------------------------------------------ */

/* LLGMM-STATUS-IND message type definition - LLC status condition
   indication. */

typedef struct
{
  IMH_T                       hdr;          /* Inter-task message header    */

  uint32                      tlli;         /* Temporary Logical Link Id    */
  gllcmm_status_cause_e_type  cause;        /* LLC status ind cause id      */

  sys_modem_as_id_e_type      as_id;

 
  uint8                       protocol_disc;  /* Protocol Discriminator of UL NAS signalling PDU */  
  uint8                       transaction_id; /* Transaction Identifier of UL NAS signalling PDU */  


} gllcmm_llgmm_status_ind_type;

/* ------------------------------------------------------------------------ */

/* LLGMM-TRIGGER-IND message type definition - LLC PDU has been transmitted
   with the GMM Ready Timer inactive. */

typedef struct
{
  IMH_T                       hdr;          /* Inter-task message header    */

  uint32                      tlli;         /* Temporary Logical Link Id    */

  sys_modem_as_id_e_type      as_id;

} gllcmm_llgmm_trigger_ind_type;

/* ------------------------------------------------------------------------ */

/* LLGMM-RDY-TMR-IND message type definition - GMM Ready Timer has expired. */

typedef struct
{
  IMH_T                       hdr;          /* Inter-task message header    */

  uint32                      tlli;         /* Temporary Logical Link Id    */

  sys_modem_as_id_e_type      as_id;

} gllcmm_llgmm_rdy_tmr_ind_type;


/* ------------------------------------------------------------------------ */

/* LLGMM-SUSPEND-CNF message type definition - User Data Path is Suspended. */

typedef struct
{
  IMH_T                       hdr;          /* Inter-task message header    */

  uint32                      tlli;         /* Temporary Logical Link Id    */

  sys_modem_as_id_e_type      as_id;

} gllcmm_llgmm_suspend_cnf_type;


/* ------------------------------------------------------------------------ */

/* LLGMM-NEW-TLLI-IND message type definition - LLC has received a frame
   as addressed by a recently assigned new TLLI. */

typedef struct
{
  IMH_T                       hdr;          /* Inter-task message header    */

  uint32                      new_tlli;     /* Temporary Logical Link Id    */

  sys_modem_as_id_e_type      as_id;

} gllcmm_llgmm_new_tlli_ind_type;


/* ======================================================================== */

/* LL-UNITDATA-IND message type definition. Unacknowledged data receipt
   indication.

   NOTE: It is the receiving entity's responsibility to de-allocate the heap
         memory that is linked to the l3_pdu ptr. */

typedef struct
{
  IMH_T                    hdr;               /* Inter-task message header             */

  uint32                   tlli;              /* Temporary Logical Link Identifier     */
  boolean                  ciphered;          /* TRUE if PDU was received ciphered     */
  uint8                    *l3_pdu;           /* Pointer to PDU in heap allocated mem  */ 
  uint16                   l3_pdu_len;        /* Length of PDU in heap allocated mem   */

  sys_modem_as_id_e_type   as_id;

} gllcmm_ll_unitdata_ind_type;

/* ======================================================================== */

/* LL-XID-IND message type definition. eXchange of Identification
   information indication message conveying a request for XID negotiation
   exchange packet from the peer GMM entity. Request packet pointer is NULL
   if message is used to indicate a change in the maximum unacknowledged
   PDU size only.
   
   NOTES: Not expected to be used, but here for completeness.
          It is the receiving entity's responsibility to de-allocate the heap
          memory that is linked to the l3_xid_req ptr. */

typedef struct
{
  IMH_T                    hdr;                /* Inter-task message header             */

  uint32                   tlli;               /* Temporary Logical Link Identifier     */
  uint8                    *l3_xid_req;        /* Ptr to XID Req Pkt in heap alloc mem  */
  uint16                   l3_xid_req_len;     /* Length of XID Req Pkt in heap mem     */
  uint16                   n201_u;             /* Current maximum unack PDU size        */

  sys_modem_as_id_e_type   as_id;

} gllcmm_ll_xid_ind_type;

/* ======================================================================== */

/* LL-XID-CNF message type definition. eXchange of IDentification
   information confirmation message conveying an XID negotiated response
   packet from the peer GMM entity.

   NOTES: Not expected to be used, but here for completeness.
          It is the receiving entity's responsibility to de-allocate the heap
          memory that is linked to the l3_xid_neg ptr. */

typedef struct
{
  IMH_T                    hdr;                /* Inter-task message header             */

  uint32                   tlli;               /* Temporary Logical Link Identifier     */
  uint8                    *l3_xid_neg;        /* Ptr to XID Neg Pkt in heap alloc mem  */
  uint16                   l3_xid_neg_len;     /* Length of XID Req Pkt in heap mem     */
  uint16                   n201_u;             /* Current maximum unack PDU size        */

  sys_modem_as_id_e_type   as_id;

} gllcmm_ll_xid_cnf_type;

/* ======================================================================== */

/* LL-RESET-IND message type definition. Reset order from SGSN. */

typedef struct
{
  IMH_T                      hdr;            /* Inter-task message header             */

  uint32                     tlli;           /* Temporary Logical Link Identifier     */

  sys_modem_as_id_e_type     as_id;

} gllcmm_ll_reset_ind_type;


/* ======================================================================== */

/* LL-STATUS-IND message 'cause' parameter type definition. */

typedef enum
{
  GLLCMM_LL_NO_PEER_RESPONSE = 0x0,/* LLC peer LLE is not responding.       */
  GLLCMM_LL_INVALID_XID_RESP = 0x1 /* XID negotiation response invalid.     */

} gllcmm_ll_status_cause_e_type;

/* ------------------------------------------------------------------------ */

/* LL-STATUS-IND message type definition - LLC status condition indication
   relating to LL SAP 1 (GMM's logical channel). */

typedef struct
{
  IMH_T                           hdr;   /* Inter-task message header      */

  uint32                          tlli;  /* Temporary Logical Link Id      */
  gllcmm_ll_status_cause_e_type   cause; /* LLC status indication cause id */

  sys_modem_as_id_e_type          as_id;

} gllcmm_ll_status_ind_type;

/* ======================================================================== */
/* LL-PSHO-CNF message type definition.  */


typedef struct
{
  IMH_T                    hdr;           /* Inter-task message header             */

  sys_modem_as_id_e_type   as_id;

} gllcmm_ll_psho_cnf_type;

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


#endif /* INC_GLLCMMIF_H */

/*** EOF: don't remove! ***/
