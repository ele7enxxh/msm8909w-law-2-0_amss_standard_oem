/*****************************************************************************
***
*** TITLE
***
***  GPRS LL SAP INTERFACE
***
***
*** DESCRIPTION
***
***  Interface definitions for use by Layer 3 and GPRS LLC modules. Defines the
***  structure types for implementation of the LL Service Primitives.
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gllc/inc/gllsap.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 10-04-01   tjw     Add customer.h and comdef.h to all SU API files
*** mm/dd/yy   xyz     changes
***
*****************************************************************************/

#ifndef INC_GLLSAP_H
#define INC_GLLSAP_H


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif

#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "dsm.h"
#include "queue.h"
#include "time_svc.h"
#include "geran_multi_sim.h"
#include "gprsdef.h"

/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/

/* ========================================================================
   LL Uplink Message Identifier Definition
   ======================================================================== */

typedef enum
{
  LL_ESTABLISH_REQ      = 0x00,
  LL_ESTABLISH_RES      = 0x01,

  LL_RELEASE_REQ        = 0x02,

  LL_XID_REQ            = 0x03,
  LL_XID_RES            = 0x04,

  LL_DATA_REQ           = 0x05,
  LL_UNITDATA_REQ       = 0x06,

  LL_INACT_SAPI_RES     = 0x07,

  LL_ACTIVATE_IND       = 0x08,
  LL_DEACTIVATE_IND     = 0x09,

  LL_RESET_RES          = 0x0A

} ll_ul_prim_t;


/* ------------------------------------------------------------------------
   LL_ESTABLISH_REQ Service Primitive structure def for use in the uplink.
   ------------------------------------------------------------------------ */

typedef struct
{
  ll_ul_prim_t          prim;     /* LL_ESTABLISH_REQ */

  uint32                tlli;
  dsm_item_type         *l3_xid_req;
  uint8                 radio_priority;
  uint8                 peak_throughput;

  uint8                 pfi;

  gas_id_t              gas_id;
   
} ll_establish_req_t;


/* ------------------------------------------------------------------------
   LL_ESTABLISH_RES Service Primitive structure def for use in the uplink.
   ------------------------------------------------------------------------ */

typedef struct
{
  ll_ul_prim_t          prim;     /* LL_ESTABLISH_RES */

  uint32                tlli;
  dsm_item_type         *l3_xid_neg;
  uint8                 radio_priority;
  uint8                 peak_throughput;

  uint8                 pfi;

  gas_id_t              gas_id;

} ll_establish_res_t;


/* ------------------------------------------------------------------------
   LL_RELEASE_REQ Service Primitive structure def for use in the uplink.
   ------------------------------------------------------------------------ */

typedef struct
{
  ll_ul_prim_t          prim;     /* LL_RELEASE_REQ */

  uint32                tlli;
  boolean               local;

  gas_id_t              gas_id;

} ll_release_req_t;


/* ------------------------------------------------------------------------
   LL_XID_REQ Service Primitive structure def for use in the uplink.
   ------------------------------------------------------------------------ */

typedef struct
{
  ll_ul_prim_t          prim;     /* LL_XID_REQ */

  uint32                tlli;
  dsm_item_type         *l3_xid_req;
  uint8                 radio_priority;
  uint8                 peak_throughput;

  uint8                 pfi;

  gas_id_t              gas_id;

} ll_xid_req_t;


/* ------------------------------------------------------------------------
   LL_XID_RES Service Primitive structure def for use in the uplink.
   ------------------------------------------------------------------------ */

typedef struct
{
  ll_ul_prim_t          prim;     /* LL_XID_RES */

  uint32                tlli;
  dsm_item_type         *l3_xid_neg;
  uint8                 radio_priority;
  uint8                 peak_throughput;

  uint8                 pfi;

  gas_id_t              gas_id;

} ll_xid_res_t;


/* ------------------------------------------------------------------------
   LL_INACT_SAPI_RES Service Primitive structure def for use in the uplink.
   ------------------------------------------------------------------------ */

typedef struct
{
  ll_ul_prim_t          prim;     /* LL_INACT_SAPI_RES */

  gas_id_t              gas_id;

} ll_inact_sapi_res_t;


/* ------------------------------------------------------------------------
   LL_DATA_REQ Service Primitive structure def for use in the uplink.
   ------------------------------------------------------------------------ */

/* L3 PDU reference used for acknowledged mode data requests */

typedef struct
{
  uint8                 nsapi;     /* NSAPI number               */
  uint16                npdu_num;  /* N-PDU reference number     */
  uint8                 seg_num;   /* Segment number of an N-PDU */

} ll_sndcp_ref_t;


typedef struct
{
  ll_ul_prim_t          prim;     /* LL_DATA_REQ */

  uint32                tlli;
  uint8                 radio_priority;
  uint8                 peak_throughput;
  ll_sndcp_ref_t        sndcp_ref;
  boolean               final_seqment;

  uint8                 pfi;

  dsm_item_type         *l3_pdu;

  gas_id_t              gas_id;

  geran_pdu_priority_t  pdu_priority;

} ll_data_req_t;


/* ------------------------------------------------------------------------
   LL_UNITDATA_REQ Service Primitive structure def for use in the uplink.
   ------------------------------------------------------------------------ */

typedef struct
{
  ll_ul_prim_t          prim;     /* LL_UNITDATA_REQ */

  uint32                tlli;
  uint8                 radio_priority;
  uint8                 peak_throughput;
  uint8                 reliability_class;
  boolean               ciphered;
  uint8                 protocol_disc;    /* Protocol Discriminator of UL NAS
                                             signalling PDU */
  uint8                 transaction_id;   /* Transaction Id of UL NAS
                                             signalling PDU */

  uint8                 pfi;
 
  ll_sndcp_ref_t        sndcp_ref;   
  dsm_item_type         *l3_pdu;
  gas_id_t              gas_id;
  geran_pdu_priority_t  pdu_priority;

} ll_unitdata_req_t;


/* ------------------------------------------------------------------------
   LL_ACTIVATE_IND Service Primitive structure def for use in the uplink.
   ------------------------------------------------------------------------ */

typedef struct
{
  ll_ul_prim_t          prim;     /* LL_ACTIVATE_IND */

  uint32                tlli;
  uint8                 radio_priority;
  uint8                 peak_throughput;

  uint8                 pfi;

  gas_id_t              gas_id;

} ll_activate_ind_t;


/* ------------------------------------------------------------------------
   LL_DEACTIVATE_IND Service Primitive structure def for use in the uplink.
   ------------------------------------------------------------------------ */

typedef struct
{
  ll_ul_prim_t          prim;     /* LL_DEACTIVATE_IND */

  uint32                tlli;

  gas_id_t              gas_id;

} ll_deactivate_ind_t;


/* ------------------------------------------------------------------------
   LL_RESET_RES Service Primitive structure def for use in the uplink.
   ------------------------------------------------------------------------ */

typedef struct
{
  ll_ul_prim_t          prim;     /* LL_RESET_RES */

  uint32                tlli;

  gas_id_t              gas_id;

} ll_reset_res_t;


/* ------------------------------------------------------------------------
   Union of all GPRS LL uplink signalling messages.
   ------------------------------------------------------------------------ */

typedef union
{
  ll_ul_prim_t          prim;

  ll_establish_req_t    est_req_mes;
  ll_establish_res_t    est_res_mes;
  ll_release_req_t      rel_req_mes;
  ll_xid_req_t          xid_req_mes;
  ll_xid_res_t          xid_res_mes;
  ll_inact_sapi_res_t   inact_sapi_res_mes;
  ll_activate_ind_t     act_ind_mes;
  ll_deactivate_ind_t   deact_ind_mes;
  ll_reset_res_t        reset_res_mes; 

} gprs_ll_ul_sig_mes_u;


/* ------------------------------------------------------------------------
   Union of all GPRS LL uplink data messages.
   ------------------------------------------------------------------------ */

typedef union
{
  ll_ul_prim_t          prim;

  ll_data_req_t         data_req_mes;
  ll_unitdata_req_t     unitdata_req_mes;

} gprs_ll_ul_dat_mes_u;


/* ------------------------------------------------------------------------
   GPRS LL uplink signalling message type.
   ------------------------------------------------------------------------ */

typedef struct
{
  q_link_type           link;
  uint8                 sapi;
  gprs_ll_ul_sig_mes_u  mes;

} gprs_ll_ul_sig_mes_t;


/* ------------------------------------------------------------------------
   GPRS LL uplink data message type.
   ------------------------------------------------------------------------ */

typedef struct
{
  q_link_type           link;
  uint8                 sapi;
  gprs_ll_ul_dat_mes_u  mes;

} gprs_ll_ul_dat_mes_t;



/* ========================================================================
   LL Downlink Message Identifier Definition
   ======================================================================== */

typedef enum
{
  LL_RESET_IND          = 0x80,

  LL_ESTABLISH_IND      = 0x81,
  LL_ESTABLISH_CNF      = 0x82,

  LL_RELEASE_IND        = 0x83,
  LL_RELEASE_CNF        = 0x84,

  LL_XID_IND            = 0x85,
  LL_XID_CNF            = 0x86,

  LL_DATA_IND           = 0x87,
  LL_DATA_CNF           = 0x88,

  LL_UNITDATA_IND       = 0x89,
  LL_UNITDATA_CNF       = 0x8A,

  LL_STATUS_IND         = 0x8B

} ll_dl_prim_t;


/* ------------------------------------------------------------------------
   LL_RESET_IND Service Primitive structure def for use in the downlink.
   ------------------------------------------------------------------------ */

typedef struct
{
  ll_dl_prim_t          prim;     /* LL_RESET_IND */

  uint32                tlli;
#ifdef FEATURE_GPRS_PS_HANDOVER
  boolean               use_old_xid_ind;
#endif /* FEATURE_GPRS_PS_HANDOVER */

  gas_id_t              gas_id;
} ll_reset_ind_t;


/* ------------------------------------------------------------------------
   LL_ESTABLISH_IND Service Primitive structure def for use in the downlink.
   ------------------------------------------------------------------------ */

typedef struct
{
  ll_dl_prim_t          prim;     /* LL_ESTABLISH_IND */

  uint32                tlli;
  dsm_item_type         *l3_xid_req;
  uint16                n201_u;
  uint16                n201_i;
  gas_id_t              gas_id;

} ll_establish_ind_t;


/* ------------------------------------------------------------------------
   LL_ESTABLISH_CNF Service Primitive structure def for use in the downlink.
   ------------------------------------------------------------------------ */

typedef struct
{
  ll_dl_prim_t          prim;     /* LL_ESTABLISH_CNF */

  uint32                tlli;
  dsm_item_type         *l3_xid_neg;
  uint16                n201_u;
  uint16                n201_i;
  gas_id_t              gas_id;

} ll_establish_cnf_t;


/* ------------------------------------------------------------------------
   LL_RELEASE_IND Service Primitive structure def for use in the downlink.
   ------------------------------------------------------------------------ */

typedef enum
{
  GPRS_LL_NORMAL_RELEASE    = 0x0,
  GPRS_LL_NO_PEER_RESPONSE  = 0x1,
  GPRS_LL_DM_RECEIVED       = 0x2,
  GPRS_LL_INVALID_XID_RESP  = 0x3

} ll_cause_t;

typedef struct
{
  ll_dl_prim_t          prim;     /* LL_RELEASE_IND */

  uint32                tlli;
  ll_cause_t            cause;
  gas_id_t              gas_id;

} ll_release_ind_t;


/* ------------------------------------------------------------------------
   LL_RELEASE_CNF Service Primitive structure def for use in the downlink.
   ------------------------------------------------------------------------ */

typedef struct
{
  ll_dl_prim_t          prim;     /* LL_RELEASE_CNF */

  uint32                tlli;
  gas_id_t              gas_id;

} ll_release_cnf_t;


/* ------------------------------------------------------------------------
   LL_XID_IND Service Primitive structure def for use in the downlink.
   ------------------------------------------------------------------------ */

typedef struct
{
  ll_dl_prim_t          prim;     /* LL_XID_IND */

  uint32                tlli;
  dsm_item_type         *l3_xid_req;
  uint16                n201_u;
  uint16                n201_i;
  gas_id_t              gas_id;

} ll_xid_ind_t;


/* ------------------------------------------------------------------------
   LL_XID_CNF Service Primitive structure def for use in the downlink.
   ------------------------------------------------------------------------ */

typedef struct
{
  ll_dl_prim_t          prim;     /* LL_XID_CNF */

  uint32                tlli;
  dsm_item_type         *l3_xid_neg;
  uint16                n201_u;
  uint16                n201_i;
  gas_id_t              gas_id;

} ll_xid_cnf_t;


/* ------------------------------------------------------------------------
   LL_DATA_IND Service Primitive structure def for use in the downlink.
   ------------------------------------------------------------------------ */

typedef struct
{
  ll_dl_prim_t          prim;     /* LL_DATA_IND */

  uint32                tlli;
  dsm_item_type         *l3_pdu;
  gas_id_t              gas_id;

} ll_data_ind_t;


/* ------------------------------------------------------------------------
   LL_DATA_CNF Service Primitive structure def for use in the downlink.
   ------------------------------------------------------------------------ */

typedef struct
{
  ll_dl_prim_t          prim;     /* LL_DATA_CNF */

  uint32                tlli;
  ll_sndcp_ref_t        sndcp_ref;
  gas_id_t              gas_id;

} ll_data_cnf_t;


/* ------------------------------------------------------------------------
   LL_UNITDATA_IND Service Primitive structure def for use in the downlink.
   ------------------------------------------------------------------------ */

typedef struct
{
  ll_dl_prim_t          prim;     /* LL_UNITDATA_IND */

  uint32                tlli;
  boolean               ciphered;
  dsm_item_type         *l3_pdu;
  gas_id_t              gas_id;

} ll_unitdata_ind_t;


/* ------------------------------------------------------------------------
   LL_UNITDATA_CNF Service Primitive structure def for use in the downlink.  
   ------------------------------------------------------------------------ */

typedef struct
{
  ll_dl_prim_t          prim;     /* LL_UNITDATA_CNF */

  uint32                tlli;
  ll_sndcp_ref_t        sndcp_ref;
  gas_id_t              gas_id;

} ll_unitdata_cnf_t;


/* ------------------------------------------------------------------------ 
   LL_STATUS_IND Service Primitive structure def for use in the downlink.
   ------------------------------------------------------------------------ */

typedef struct
{
  ll_dl_prim_t          prim;     /* LL_STATUS_IND */

  uint32                tlli;
  ll_cause_t            cause;
  gas_id_t              gas_id;

} ll_status_ind_t;


/* ------------------------------------------------------------------------
   Union of all GPRS LL downlink signalling messages.
   ------------------------------------------------------------------------ */

typedef union
{
  ll_dl_prim_t          prim;

  ll_reset_ind_t        rst_ind_mes;
  ll_establish_ind_t    est_ind_mes;
  ll_establish_cnf_t    est_cnf_mes;
  ll_release_ind_t      rel_ind_mes;
  ll_release_cnf_t      rel_cnf_mes;
  ll_xid_ind_t          xid_ind_mes;
  ll_xid_cnf_t          xid_cnf_mes;
  ll_data_cnf_t         data_cnf_mes;
  ll_status_ind_t       status_ind_mes;
  ll_unitdata_cnf_t     udata_cnf_mes;

} gprs_ll_dl_sig_mes_u;


/* ------------------------------------------------------------------------
   Union of all GPRS LL downlink signalling messages.
   ------------------------------------------------------------------------ */

typedef union
{
  ll_dl_prim_t          prim;

  ll_data_ind_t         data_ind_mes;
  ll_unitdata_ind_t     unitdata_ind_mes;

} gprs_ll_dl_dat_mes_u;


/* ------------------------------------------------------------------------
   GPRS LL downlink signalling message type.
   ------------------------------------------------------------------------ */

typedef struct
{
  q_link_type           link;
  uint8                 sapi;
  gprs_ll_dl_sig_mes_u  mes;

} gprs_ll_dl_sig_mes_t;


/* ------------------------------------------------------------------------
   GPRS LL downlink data message type.
   ------------------------------------------------------------------------ */

typedef struct
{
  q_link_type           link;

#ifndef DEBUG_GSM_GPRS_DS_UNIT_TEST
  time_type             timestamp;
#endif

  uint8                 sapi;
  gprs_ll_dl_dat_mes_u  mes;

} gprs_ll_dl_dat_mes_t;


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


#endif /* INC_GLLSAP_H */

/*** EOF: don't remove! ***/
