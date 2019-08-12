/*****************************************************************************
***
*** TITLE
***
***  GPRS GMM -> LLC MESSAGE INTERFACE
***
***
*** DESCRIPTION
***
***  Defines the structure types for implementation of the LLGMM Service
***  Primitives for transmission to LLC by GMM. Also defines the structure
***  types for implementation of the applicable subset of LL Service
***  Primitives which are permissable on LL SAPI 1 (GMM).
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/gmmllcif.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 10-04-01   tjw     Add customer.h and comdef.h to all SU API files
*** mm/dd/yy   xyz     changes
***
*****************************************************************************/

#ifndef INC_GMMLLCIF_H
#define INC_GMMLLCIF_H


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

#include "geran_dual_sim.h"

/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/

#define GLLC_IOV_UI_SIZE 4

/* ======================================================================== */
/* GMM -> LLC LLGMM Service Primitive Message Definitions                   */
/* ======================================================================== */

/* GMM -> LLC LLGMM service primitive message identifier definition. */

typedef enum
{
  GMMLLC_LLGMM_ASSIGN_REQ       = 0x00,
  GMMLLC_LLGMM_TRIGGER_REQ      = 0x01,
  GMMLLC_LLGMM_SUSPEND_REQ      = 0x02,
  GMMLLC_LLGMM_RESUME_REQ       = 0x03,
  GMMLLC_LLGMM_STRT_RDY_TMR_REQ = 0x04,
  GMMLLC_LLGMM_STOP_RDY_TMR_REQ = 0x05,
  GMMLLC_LLGMM_TEST_MODE_REQ    = 0x06
  ,GMMLLC_LLGMM_PSHO_REQ        = 0x07
} gmmllc_llgmm_prim_e_type;

/* ======================================================================== */

/* GPRS Encryption Algorithm (GEA) identifier definition. */

typedef enum
{
  GMMLLC_GEA_NONE               = 0x00,
  GMMLLC_GEA_1                  = 0x01,
  GMMLLC_GEA_2                  = 0x02,
  GMMLLC_GEA_3                  = 0x03,
  GMMLLC_GEA_4                  = 0x04,
  GMMLLC_GEA_5                  = 0x05,
  GMMLLC_GEA_6                  = 0x06,
  GMMLLC_GEA_7                  = 0x07,

  GMMLLC_GEA_UNDEFINED          = 0xFF

} gmmllc_gea_e_type;

/* ------------------------------------------------------------------------ */

/* LLGMM-ASSIGN-REQ message type definition. TLLI assignment, TLLI change
   and TLLI unassignment request. */

typedef struct
{
  gmmllc_llgmm_prim_e_type    prim;     /* GMMLLC_LLGMM_ASSIGN_REQ          */

  uint32                      tlli_old; /* Old TLLI value                   */
  uint32                      tlli_new; /* New TLLI value                   */
  uint8                       kc[8];    /* 64 bit encrypt key,low 8 in [0]  */
  uint8                       kc_128bit[16];/* 128 bit encrypt key, low 8 in [0]*/
  gmmllc_gea_e_type           gea;      /* GPRS Encryption Algorithm        */

//ifdef F EATURE_DUAL_SIM
  sys_modem_as_id_e_type      as_id;    /* Access stratum identity    */
//endif /* F EATURE_DUAL_SIM */

} gmmllc_llgmm_assign_req_type;

/* ======================================================================== */

/* LLGMM-TRIGGER-REQ message 'cause' parameter type definition. */

typedef enum
{
  GMMLLC_PAGE_RESPONSE        = 0x0,
  GMMLLC_CELL_UPDATE          = 0x1,
  GMMLLC_CELL_NOTIFY          = 0x2

} gmmllc_trigger_cause_e_type;


/* ------------------------------------------------------------------------ */

/* LLGMM-TRIGGER-REQ message type definition. Trigger a frame transmission
   and attach a cause value. */

typedef struct
{
  gmmllc_llgmm_prim_e_type    prim;     /* GMMLLC_LLGMM_TRIGGER_REQ         */

  uint32                      tlli;     /* Temporary Logical Link Id        */
  gmmllc_trigger_cause_e_type cause;    /* Cause for trigger request        */

//ifdef F EATURE_DUAL_SIM
  sys_modem_as_id_e_type      as_id;    /* GERAN access stratum identity    */
//endif /* F EATURE_DUAL_SIM */

} gmmllc_llgmm_trigger_req_type;


/* ======================================================================== */

/* LLGMM-SUSPEND-REQ message type definition. Suspend LLC operation. */

typedef struct
{
  gmmllc_llgmm_prim_e_type    prim;     /* GMMLLC_LLGMM_SUSPEND_REQ         */

  uint32                      tlli;     /* Temporary Logical Link Id        */

//ifdef F EATURE_DUAL_SIM
  sys_modem_as_id_e_type      as_id;    /* GERAN access stratum identity    */
//endif /* F EATURE_DUAL_SIM */

} gmmllc_llgmm_suspend_req_type;


/* ======================================================================== */

/* LLGMM-RESUME-REQ message type definition. Resume LLC operation. */

typedef struct
{
  gmmllc_llgmm_prim_e_type    prim;     /* GMMLLC_LLGMM_RESUME_REQ          */

  uint32                      tlli;     /* Temporary Logical Link Id        */

//ifdef F EATURE_DUAL_SIM
  sys_modem_as_id_e_type      as_id;    /* GERAN access stratum identity    */
//endif /* F EATURE_DUAL_SIM */

} gmmllc_llgmm_resume_req_type;

/* ======================================================================== */

/* GMMLLC-LLGMM_STRT-RDY-TMR-REQ message type definition. Start ready timer */

typedef struct
{
  gmmllc_llgmm_prim_e_type    prim;       /* GMMLLC_LLGMM_STRT_RDY_TMR_REQ  */

  uint32                      tlli;       /* Temporary Logical Link Id      */
  uint32                      timeout_ms; /* Timeout in milliseconds        */

//ifdef F EATURE_DUAL_SIM
  sys_modem_as_id_e_type      as_id;      /* GERAN access stratum identity  */
//endif /* F EATURE_DUAL_SIM */

} gmmllc_llgmm_strt_rdy_tmr_req_type;

/* ======================================================================== */

/* GMMLLC-LLGMM_STOP-RDY-TMR-REQ message type definition. Stop ready timer. */

typedef struct
{
  gmmllc_llgmm_prim_e_type    prim;       /* GMMLLC_LLGMM_STOP_RDY_TMR_REQ  */

  uint32                      tlli;       /* Temporary Logical Link Id      */

//ifdef F EATURE_DUAL_SIM
  sys_modem_as_id_e_type      as_id;      /* GERAN access stratum identity  */
//endif /* F EATURE_DUAL_SIM */

} gmmllc_llgmm_stop_rdy_tmr_req_type;

/* ======================================================================== */

/* GMMLLC-LLGMM-TEST-MODE-REQ message type definition. Enter ETSI Test
   Mode A/B or SRB */

typedef struct
{
  gmmllc_llgmm_prim_e_type    prim;         /* GMMLLC_LLGMM_TEST_MODE_REQ */

  uint8                       test_mode;    /* Test Mode A/B or SRB         */
  uint8                       loopback_mode;/* Loopback mode */
  uint16                      llc_pdu_num;  /* Number of Test PDUs to Tx    */
  uint16                      llc_pdu_size; /* Octet size of PDUs to Tx     */
  uint8                       ts_offset;    /* Downlink Timeslot offset     */

//ifdef F EATURE_DUAL_SIM
  sys_modem_as_id_e_type      as_id;        /* GERAN access stratum         
                                               identity */
//endif /* F EATURE_DUAL_SIM */

} gmmllc_llgmm_test_mode_req_type;

/* ======================================================================== */

/* GMMLLC-LLGMM-PSHO-REQ message type definition. SNDCP and LLC XID Params,
   Ciphering Algorithm, IOV_UI value to be used after a PS handover */

//ifdef F EATURE_GPRS_PS_HANDOVER

typedef struct
{
  boolean           use_old_xid_ind;          /* TRUE if LLC & SNDCP should be
                                                 reset with old XID params */
  gmmllc_gea_e_type gea;                      /* GPRS ciphering Algorithm */
  uint8             iov_ui[GLLC_IOV_UI_SIZE]; /* IOV - UI value */

} gmmllc_nas_container_psho_type;


typedef struct
{
  gmmllc_llgmm_prim_e_type        prim;          /* GMMLLC_LLGMM_PSHO_REQ */

  gmmllc_nas_container_psho_type nas_container; /* Nas container */

//ifdef F EATURE_DUAL_SIM
  sys_modem_as_id_e_type          as_id;         /* GERAN access stratum 
                                                    identity */
//endif /* F EATURE_DUAL_SIM */

} gmmllc_llgmm_psho_req_type;

//endif /* F EATURE_GPRS_PS_HANDOVER */


/* ======================================================================== */
/* GMM -> LLC LL Service Primitive Message Definitions                      */
/* ======================================================================== */

/* GMM -> LLC LL service primitive message identifier definition. */

typedef enum
{
  GMMLLC_LL_UNITDATA_REQ      = 0x20,
  GMMLLC_LL_XID_REQ           = 0x21,
  GMMLLC_LL_XID_RES           = 0x22

} gmmllc_ll_prim_e_type;

/* ======================================================================== */

/* LL-UNITDATA-REQ message type definition. Unacknowledged data transmission
   request.

   NOTE: It is the sending entity's responsibility to de-allocate any heap
         memory that is linked to the l3_pdu ptr. */

typedef struct
{
  gmmllc_ll_prim_e_type   prim;           /* GMMLLC_LL_UNITDATA_REQ         */

  uint32                  tlli;           /* Temporary Logical Link Id      */
  boolean                 ciphered;       /* TRUE if data is to be ciphered */
  uint8                   *l3_pdu;        /* Ptr to PDU in heap alloc mem   */
  uint16                  l3_pdu_len;     /* Len of PDU in heap alloc mem   */
//ifdef F EATURE_GPRS_LAYER3_TX_CONFIRM
  uint8                   protocol_disc;  /* Protocol Discriminator of UL NAS
                                             signalling PDU */
  uint8                   transaction_id; /* Transaction Identifier of UL NAS
                                             signalling PDU */
//endif /* F EATURE_GPRS_LAYER3_TX_CONFIRM */
//ifdef F EATURE_DUAL_SIM
  sys_modem_as_id_e_type  as_id;          /* GERAN access stratum identity  */
//endif /* F EATURE_DUAL_SIM */
} gmmllc_ll_unitdata_req_type;

/* ======================================================================== */

/* LL-XID-REQ message type definition. eXchange of IDentification
   information request message conveying a request for XID negotiation
   exchange packet to the peer GMM entity.

   NOTES: Not expected to be used, but here for completeness.
          It is the sending entity's responsibility to de-allocate any heap
          memory that is linked to the l3_xid_req ptr. */

typedef struct
{
  gmmllc_ll_prim_e_type   prim;           /* GMMLLC_LL_XID_REQ              */

  uint32                  tlli;           /* Temporary Logical Link Id      */
  uint8                   *l3_xid_req;    /* Ptr to req in heap alloc mem   */
  uint16                  l3_xid_req_len; /* Len of req in heap alloc mem   */

//ifdef F EATURE_DUAL_SIM
  sys_modem_as_id_e_type  as_id;          /* GERAN access stratum identity  */
//endif /* F EATURE_DUAL_SIM */

} gmmllc_ll_xid_req_type;

/* ======================================================================== */

/* LL-XID-RES message type definition. eXchange of IDentification
   information response message conveying a negotiated XID response packet
   to the peer GMM entity.

   NOTES: Not expected to be used, but here for completeness.
          It is the sending entity's responsibility to de-allocate any heap
          memory that is linked to the l3_xid_neg ptr. */

typedef struct
{
  gmmllc_ll_prim_e_type   prim;           /* GMMLLC_LL_XID_RES              */

  uint32                  tlli;           /* Temporary Logical Link Id      */
  uint8                   *l3_xid_neg;    /* Ptr to neg in heap alloc mem   */
  uint16                  l3_xid_neg_len; /* Len of neg in heap alloc mem   */

//ifdef F EATURE_DUAL_SIM
  sys_modem_as_id_e_type  as_id;          /* GERAN access stratum identity  */
//endif /* F EATURE_DUAL_SIM */

} gmmllc_ll_xid_res_type;

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


#endif /* INC_GMMLLCIF_H */

/*** EOF: don't remove! ***/
