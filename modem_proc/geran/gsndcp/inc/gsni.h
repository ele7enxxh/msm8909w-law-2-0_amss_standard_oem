/*****************************************************************************
***
*** TITLE
***
***  GPRS SNDCP LAYER INTERNAL DEFINITIONS
***
***
*** DESCRIPTION
***
***  Provides SNDCP definitions for the exclusive use of the SNDCP
***  module files only.
***
***
*** Copyright (c) 2009-2013 Qualcomm Technologies, Inc.
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gsndcp/inc/gsni.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** mm/dd/yy   xyz     changes
***
*****************************************************************************/

#ifndef INC_GSNI_H
#define INC_GSNI_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */

#include "queue.h"
#include "rex.h"
#include "gprs_pdu.h"
#include "smtask.h"
#include "gsndef.h"
#include "gsndcp.h"
#include "comdef.h"
#include "task.h"
#include "ps_slhc.h" /* RFC 1144 compression entity structure definition */

#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP
#include "gsnv42.h"
#endif

#include "comptask.h"
#include "comptask_api.h"

#include "geran_multi_sim.h"


/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/

/* NSAPI numbers used for array indexing in SNDCP.
   To satisfy Klocwork, replaced GSN_NSAPI_MAX with GSN_NSAPI_LAST=
   GSN_NSAPI_15. And #defined GSN_NSAPI_MAX outside the enum.
 */
typedef enum
{
  GSN_NSAPI_5 = 0,     // Must be 0.
  GSN_NSAPI_6,
  GSN_NSAPI_7,
  GSN_NSAPI_8,
  GSN_NSAPI_9,
  GSN_NSAPI_10,
  GSN_NSAPI_11,
  GSN_NSAPI_12,
  GSN_NSAPI_13,
  GSN_NSAPI_14,
  GSN_NSAPI_15,

  GSN_NSAPI_LAST=GSN_NSAPI_15       // Must be last.

} gsn_nsapi_t;
#define GSN_NSAPI_MAX       (((int32)GSN_NSAPI_LAST)+1)

/* LL SAPIs used by the SNDCP entity.
   To satisfy Klocwork, replaced GSN_SAPI_MAX with GSN_SAPI_LAST=
   GSN_SAPI_11. And #defined GSN_SAPI_MAX outside the enum.
*/
typedef enum
{
  GSN_SAPI_3  = 0,  /* Must be zero! */
  GSN_SAPI_5,
  GSN_SAPI_9,
  GSN_SAPI_11,

  GSN_SAPI_LAST=GSN_SAPI_11       // Must be last.

} gsn_sapi_t;
#define GSN_SAPI_MAX        (((int32)GSN_SAPI_LAST)+1)

/* SNDCP NSAPI operational mode */
typedef enum
{
  UNDEF,  /* Undefined */
  UNACK,  /* Unacknowledged */
  ACK     /* Acknowledged */

} gsn_nsapi_mode_t;

/* SNDCP Down-Link Entity states */
typedef enum
{
  DL_NORMAL,
  DL_RECOVERY

} gsn_dle_state_t;

/* SNDCP Down-Link Entity reassembly states */
typedef enum
{
  DL_RCV_1ST_SEQ,
  DL_RCV_SUBS_SEQ,
  DL_DISCARD

} gsn_reas_state_t;

/* SAPI entity states */
typedef enum
{
  IDLE,
  EST_PEND,
  ESTABLISHED,
  DISC_PEND

} gsn_sapi_state_t;

/*  To store the DCOMP, PCOMP and N-PDU number of the first segment
**  of an N-PDU during reassembly in order to compare with subsequent segments
*/
typedef struct
{
  uint8  dcomp;
  uint8  pcomp;
  uint8  ack_npdu_num;
  uint16 unack_npdu_num; /* Needs to be word for UNACK mode */

} gsn_first_seg_info_t;

/* Enumerates states of XID request/negotiation process */
typedef enum
{
  XID_IDLE,         /* No XID params have been exchanged between peer entities */
  XID_EST_CNF_PEND, /* LL-EST.CNF pending with XID negotiated */
  XID_CNF_PEND,     /* LL-XID.CNF pending with XID negotiated */
  XID_NEGOTIATED,   /* XID negotiation (proposed by peer) finished succesfully */
  XID_REQUESTED     /* XID requested by SNDCP entity (or SNDCP User) accepted by peer */

} gsn_xid_state_t;

/*-------------------------------------------------
    SNDCP internal commands type definitions
--------------------------------------------------*/

/* Enumerates internal SNDCP commands IDs */
typedef enum
{
  RE_EST_CMD,     /* Re-establish ACK LLC link */
  RE_TX_CMD,      /* Re-transmit unacknowledged N-PDUs */
  TIMER_CMD,      /* SNDCP Timer command */
  SEND_XID_REQ,   /* Initiate the XID request procedure */
  PDP_REG_REQ     /* Perform PDP Interface Registration */

  /* More internal cmd IDs */

} gsn_int_cmd_id_t;

/* Re-establish ACK LLC link command
*/
typedef struct
{
  gsn_sapi_t   sapi;
  gas_id_t     gas_id;

} gsn_re_est_cmd_t;

/* Re-transmit unacknowledged N-PDUs command
*/
typedef struct
{
  gsn_nsapi_t   nsapi;
  gas_id_t      gas_id;

} gsn_re_tx_cmd_t;

/* SNDCP Timer command
*/

/* Enumerates SNDCP timer ids */
typedef enum
{
  RE_EST,                       /* Re-establish ACK LLC link timer   */
  SGSN_INITIATED_LINK_EST_FAIL  /* SGSN initiated link establishment failure */  

  /* More internal timer IDs */

} gsn_timer_id_t;


/* Timer cmd structure */
typedef struct
{
  gsn_timer_id_t  tmr_id;
  gsn_sapi_t      sapi;
  gsn_nsapi_t     nsapi;
  gas_id_t        gas_id;

} gsn_timer_cmd_t;


/* Command that initiates the XID request procedure.
*/
typedef struct
{
  gsn_sapi_t      sapi;
  dsm_item_type   *xid_pdu;
  gas_id_t        gas_id;

} gsn_xid_req_cmd_t;


/* Command that initiates the PDP Interface Registration Request procedure.
*/

typedef struct
{
  uint8           nsapi;                  /* NSAP Identifier Address     */
  void            *ds_context;            /* DS Client Context Ident     */
  boolean         cipher;                 /* Ciphering Option Required   */

  /* Client Callback for Downlink NPDU delivery                          */
  void            ( *pdp_dl_fnc_ptr )( void* ds_context, dsm_item_type **npdu );

  /* Client Callbacks for Flow Control Suspension and Resumption         */

  wm_cb_type      pdp_ul_suspend_fnc_ptr;
  wm_cb_type      pdp_ul_resume_fnc_ptr;

  /* Client Callback for PDP Interface Registration Confirmation         */
  void            ( *pdp_reg_cnf_fnc_ptr )( void* ds_context, boolean success );

  gas_id_t        gas_id;

} gsn_pdp_reg_req_cmd_t;


typedef struct
{
  gsn_int_cmd_id_t  cmd_id;

  union
  {
    gsn_re_est_cmd_t      re_est;
    gsn_re_tx_cmd_t       re_tx;
    gsn_timer_cmd_t       timer;
    gsn_xid_req_cmd_t     xid_req;
    gsn_pdp_reg_req_cmd_t pdp_reg_req;

    /* More internal cmds ..  */
  } gsn_cmd;

} gsn_internal_msg_t;

/*===============================================
**        SNDCP FRAME FORMATS
*===============================================*/

/*--------------------------------------------
  SN-DATA PDU Format. See TS 04.65 par 7.2
---------------------------------------------*/

typedef struct
{
  uint8 x;
#define gsn_data_pdu_hdr_t_x_len      1 /* length of X field */

  uint8 f;
#define gsn_data_pdu_hdr_t_f_len      1

  uint8 t;
#define gsn_data_pdu_hdr_t_t_len      1

  uint8 m;
#define gsn_data_pdu_hdr_t_m_len      1

  uint8 nsapi;
#define gsn_data_pdu_hdr_t_nsapi_len  4 /* length of NSAPI field */

  uint8 dcomp;
#define gsn_data_pdu_hdr_t_dcomp_len  4

  uint8 pcomp;
#define gsn_data_pdu_hdr_t_pcomp_len  4

  uint8 num;
#define gsn_data_pdu_hdr_t_num_len    8 /* length of N-PDU number field in ack mode */

} gsn_data_pdu_hdr_t;


/*------------------------------------------------
   SN-UNITDATA PDU Format. See TS 04.65 par 7.2
-------------------------------------------------*/

typedef struct
{
  uint8 x;
#define gsn_unitdata_pdu_hdr_t_x_len      1 /* length of X field */

  uint8 f;
#define gsn_unitdata_pdu_hdr_t_f_len      1

  uint8 t;
#define gsn_unitdata_pdu_hdr_t_t_len      1

  uint8 m;
#define gsn_unitdata_pdu_hdr_t_m_len      1

  uint8 nsapi;
#define gsn_unitdata_pdu_hdr_t_nsapi_len  4 /* length of NSAPI field */

  uint8 dcomp;
#define gsn_unitdata_pdu_hdr_t_dcomp_len  4

  uint8 pcomp;
#define gsn_unitdata_pdu_hdr_t_pcomp_len  4

  uint8 seg;
#define gsn_unitdata_pdu_hdr_t_seg_len    4 /* segment number */

  uint16 num;
#define gsn_unitdata_pdu_hdr_t_num_len    12 /* length of N-PDU number field in unack mode */

} gsn_unitdata_pdu_hdr_t;


/*----------------------------------------------
  Header of SNDCP XID compression fild format
-------------------------------------------------*/

typedef struct
{
  uint8 p;
#define gsn_xid_comp_hdr_t_p_len         1

  uint8 x1;
#define gsn_xid_comp_hdr_t_x1_len        2 /* Two X filds (spere bits) */

  uint8 ent_num;
#define gsn_xid_comp_hdr_t_ent_num_len   5

  uint8 x2;
#define gsn_xid_comp_hdr_t_x2_len        3 /* Three X filds (spere bits) */

  uint8 alg_type;
#define gsn_xid_comp_hdr_t_alg_type_len  5

  uint8 length;
#define gsn_xid_comp_hdr_t_length_len    8

} gsn_xid_comp_hdr_t;



/*-----------------------------------------------
  SNDCP XID Parameter type 0. Version number
------------------------------------------------*/
typedef struct
{
  uint8 ver;
#define gsn_xid_t0_t_ver_len     8

} gsn_xid_t0_t;


/*-------------------------------------------------
  SNDCP XID Parameter type 1. Data Compression
---------------------------------------------------*/

/* V42bis data compression parameters */
typedef struct
{

  uint8  dcomp1;
#define  gsn_xid_dcomp_v42bis_t_dcomp1_len    4 /* Only DCOMP1 used in V42 bis */

  uint8  dcomp2;
#define  gsn_xid_dcomp_v42bis_t_dcomp2_len    4 /* DCOMP2 should be set to zero */

  uint16 nsapis;
#define  gsn_xid_dcomp_v42bis_t_nsapis_len    16/* Applicable NSAPIs */

  uint8  p0;
#define  gsn_xid_dcomp_v42bis_t_p0_len        8

  uint16 p1;
#define  gsn_xid_dcomp_v42bis_t_p1_len        16

  uint8  p2;
#define  gsn_xid_dcomp_v42bis_t_p2_len        8


} gsn_xid_dcomp_v42bis_t;

/* Data compression Algorithm types */
typedef union
{

  /* V.42 bis  algorithm data type */
  gsn_xid_dcomp_v42bis_t  xid_v42bis;

  /* Other Data compression algorithm types should be added here */

} gsn_xid_t1_alg_t;

/* SNDCP XID Parameter type 1 field format */
typedef struct
{
  /* header of data compression field format */
  gsn_xid_comp_hdr_t      hdr;

  gsn_xid_t1_alg_t        alg;

} gsn_xid_t1_t;

/*---------------------------------------------------------------------
  SNDCP XID Parameter type 2. Protocol Control Information Compression
----------------------------------------------------------------------*/

/* RFC 1144 TCP/IP header compression parameters */
typedef struct
{

  uint8  pcomp1;
#define  gsn_xid_pcomp_1144_t_pcomp1_len    4 /* Uncompressed TCP */

  uint8  pcomp2;
#define  gsn_xid_pcomp_1144_t_pcomp2_len    4 /* Compressed TCP */

  uint16 nsapis;
#define  gsn_xid_pcomp_1144_t_nsapis_len    16/* Applicable NSAPIs */

  uint8  s0_1;
#define  gsn_xid_pcomp_1144_t_s0_1_len      8


} gsn_xid_pcomp_1144_t;

/*  RFC 2507 TCP/IP and UDP/IP header compression parameters  */
typedef struct
{

  uint8  pcomp1;
#define  gsn_xid_pcomp_2507_t_pcomp1_len    4 /* Full header */

  uint8  pcomp2;
#define  gsn_xid_pcomp_2507_t_pcomp2_len    4 /* Compressed TCP */

  uint8  pcomp3;
#define  gsn_xid_pcomp_2507_t_pcomp3_len    4 /* Compressed TCP non-delta */

  uint8  pcomp4;
#define  gsn_xid_pcomp_2507_t_pcomp4_len    4 /* Compressed non-TCP */

  uint8  pcomp5;
#define  gsn_xid_pcomp_2507_t_pcomp5_len    4 /* Context state */

  uint8  pcomp6;
#define  gsn_xid_pcomp_2507_t_pcomp6_len    4 /* Dumy. Number of PCOMP values must be
                                                even in the compression field */

  uint16 nsapis;
#define  gsn_xid_pcomp_2507_t_nsapis_len    16 /* Applicable NSAPIs */

  uint16 max_per;
#define  gsn_xid_pcomp_2507_t_max_per_len   16 /* F_MAX_PERIOD */

  uint8  max_time;
#define  gsn_xid_pcomp_2507_t_max_time_len   8  /* F_MAX_TIME */

  uint8  max_hdr;
#define  gsn_xid_pcomp_2507_t_max_hdr_len   8  /* MAX_HEDER */

  uint8  tcp_sp;
#define  gsn_xid_pcomp_2507_t_tcp_sp_len    8  /* TCP_SPACE */

  uint16 ntcp_sp;
#define  gsn_xid_pcomp_2507_t_ntcp_sp_len   16 /* NON_TCP_SPACE */

} gsn_xid_pcomp_2507_t;



/* RFC 3095 RTP/UDP/IP, UDP/IP, ESP/IP header compression parameters (RoHC) */
typedef struct
{

  uint8  pcomp1;
#define  gsn_xid_pcomp_3095_t_pcomp1_len    4    /* RoHC small CIDs */

  uint8  pcomp2;
#define  gsn_xid_pcomp_3095_t_pcomp2_len    4    /* RoHC large-CIDs */

  uint16 nsapis;
#define  gsn_xid_pcomp_3095_t_nsapis_len    16   /* Applicable NSAPIs */

  uint16 max_cid;
#define  gsn_xid_pcomp_3095_t_max_cid_len   16   /* MAX_CID */

  uint16 max_hdr;
#define  gsn_xid_pcomp_3095_t_max_hdr_len   16   /* MAX_HEADER */

  uint16 profile1;
#define  gsn_xid_pcomp_3095_t_profile1_len  16   /* PROFILE 1 */

  uint16 profile2;
#define  gsn_xid_pcomp_3095_t_profile2_len  16   /* PROFILE 2 */

  uint16 profile3;
#define  gsn_xid_pcomp_3095_t_profile3_len  16   /* PROFILE 3 */

  uint16 profile4;
#define  gsn_xid_pcomp_3095_t_profile4_len  16   /* PROFILE 4 */

#if 0
  uint16 profile5;
#define  gsn_xid_pcomp_3095_t_profile5_len  16   /* PROFILE 5 */

  uint16 profile6;
#define  gsn_xid_pcomp_3095_t_profile6_len  16   /* PROFILE 6 */

  uint16 profile7;
#define  gsn_xid_pcomp_3095_t_profile7_len  16   /* PROFILE 7 */

  uint16 profile8;
#define  gsn_xid_pcomp_3095_t_profile8_len  16   /* PROFILE 8 */

  uint16 profile9;
#define  gsn_xid_pcomp_3095_t_profile9_len  16   /* PROFILE 9 */

  uint16 profile10;
#define  gsn_xid_pcomp_3095_t_profile10_len  16  /* PROFILE 10 */

  uint16 profile11;
#define  gsn_xid_pcomp_3095_t_profile11_len  16  /* PROFILE 11 */

  uint16 profile12;
#define  gsn_xid_pcomp_3095_t_profile12_len  16  /* PROFILE 12 */

  uint16 profile13;
#define  gsn_xid_pcomp_3095_t_profile13_len  16  /* PROFILE 13 */

  uint16 profile14;
#define  gsn_xid_pcomp_3095_t_profile14_len  16  /* PROFILE 14 */

  uint16 profile15;
#define  gsn_xid_pcomp_3095_t_profile15_len  16  /* PROFILE 15 */

  uint16 profile16;
#define  gsn_xid_pcomp_3095_t_profile16_len  16  /* PROFILE 16 */
#endif /* #if 0 */

} gsn_xid_pcomp_3095_t;



/* Protocol Control Information (PCI) compression Algorithm types */
typedef union
{
  /* RFC 1144  algorithm data type */
  gsn_xid_pcomp_1144_t    xid_rfc1144;

  /* RFC 2507  algorithm data type */
  gsn_xid_pcomp_2507_t    xid_rfc2507;

  /* RFC 3095  algorithm data type */
  gsn_xid_pcomp_3095_t    xid_rfc3095;

} gsn_xid_t2_alg_t;


/* SNDCP XID Parameter type 2 field format */
typedef struct
{
  /* header of PCI compression fild format */
  gsn_xid_comp_hdr_t      hdr;

  gsn_xid_t2_alg_t        alg;

} gsn_xid_t2_t;


/*-------------------------------------------
  Union that holds all the SNDCP XID types
--------------------------------------------*/
typedef union
{
  gsn_xid_t0_t      xid_t0;
  gsn_xid_t1_t      xid_t1;
  gsn_xid_t2_t      xid_t2;

  /* Other XID types should be added here */

} gsn_xid_type_t;



/*-------------------------------------------
  XID block format. See TS 04.65 par 6.8
--------------------------------------------*/

typedef struct
{
  uint8             type;
#define gsn_xid_block_t_type_len    8


  uint8             length;
#define gsn_xid_block_t_length_len  8

  gsn_xid_type_t    type_field;

} gsn_xid_block_t;

/*===============================================
**  SNDCP XID compression related data types
================================================*/

/* States of compression entity number or PCOMP/DCOMP value */
typedef enum
{
  UNASSIGNED = 0,
  SELECTED,
  ASSIGNED

}num_state_t;

#ifdef FEATURE_GSM_GPRS_SNDCP_PCOMP

/* Protocol Control Information (PCI) compression entity */
typedef struct
{
  num_state_t       state;      /* State of compression entity number */
  void              *comp_e;    /* Holds a pointer to a data structure which
                                   stores data for an instance of 1144,2507, or
                                   3095 compression/decompression engine */
  uint8             ent_num;    /* Number (0:31) assigned to the entity */
  uint8             t2_alg;     /* T2 comp algorithm used by the entity  */

  /* Ack or UnAck mode of operation of NSAPIs using the entity
  ** Note that only NSAPIs of the same mode can share the same entity
  */
  gsn_nsapi_mode_t  op_mode;

  gsn_xid_t2_alg_t  alg;        /* Type 2 (or PCI) comp algprothm parameters */

}pcomp_e_t;

typedef struct
{
  num_state_t       state;      /* State of PCOMP value */
  uint8             val_num;    /* Number (0:15) assigned to the PCOMP value */

  /* Type 2 algorithm that the PCOMP value is selected or assigned to.*/
  uint8             alg;

}pcomp_t;


/* Type used for linking (in SAPI's t2_alg_q) assigned or selected PCI
** comp entities that use the same compression alcorithm
*/
typedef struct
{
  q_link_type       link;
  pcomp_e_t         *pcomp_e_ptr;

}t2_alg_q_t;

#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP */


#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP

/* Data compression entity */
typedef struct
{
  num_state_t       state;      /* State of compression entity number */
  comp_ent_V42bis_t comp_e;     /* Holds compression state info of the entity */
  uint8             ent_num;    /* Number (0:31) assigned to the entity */
  uint8             t1_alg;     /* T1 comp algorithm used by the entity  */

  /* Ack or UnAck mode of operation of NSAPIs using the entity
  ** Note that only NSAPIs of the same mode can share the same entity
  */
  gsn_nsapi_mode_t  op_mode;

  gsn_xid_t1_alg_t  alg;        /* Type 1 ( or data comp ) algprothm parameters */

}dcomp_e_t;

typedef struct
{
  num_state_t       state;      /* State of DCOMP value */
  uint8             val_num;    /* Number (0:15) assigned to the DCOMP value */

  /* Type 2 algorithm that the DCOMP value is selected or assigned to.*/
  uint8             alg;

}dcomp_t;

/* Type used for linking (in SAPI's t1_alg_q) assigned or selected Data
** comp entities that use the same data compression algorithm.
*/
typedef struct
{
  q_link_type       link;
  dcomp_e_t         *dcomp_e_ptr;

}t1_alg_q_t;

#endif /* FEATURE_GSM_GPRS_SNDCP_DCOMP */


#if( (defined(FEATURE_GSM_GPRS_SNDCP_PCOMP)) || (defined(FEATURE_GSM_GPRS_SNDCP_DCOMP)) )

/* Type used for XID parameters requested by PDP via SN-XID.REQ
** primitive. At the moment PDP does not uses SN-XID.REQ primitive
** to request XID parameters. So these parameters are initialised
** to default/desirable values on NSAPI initialisation.
*/
typedef struct
{
  /*
  ** T0 XID parameter - SNDCP version number
  *--------------------------------------*/
  uint8   ver_num;

  /*
  ** T1 XID parameters - Data compression
  *--------------------------------------*/
  uint8   t1_alg;

  /* V42bis parameters */
  uint8   p0;
  uint16  p1;
  uint8   p2;

  /*
  ** T2 XID parameters - PCI compression
  ** ------------------------------------*/
  uint8  t2_alg;

  /* RFC 1144 parameters */
  uint8   s0_1;

  /* RFC 2507 parameters */
  uint16  max_per;
  uint8   max_time;
  uint8   max_hdr;
  uint8   tcp_sp;
  uint16  ntcp_sp;

  /* RFC 3095 parameters */
  uint16  max_cid;
  uint16  max_header;
  uint16  profile1;
  uint16  profile2;
  uint16  profile3;
  uint16  profile4;
}gsn_sn_xid_req_t;

#endif /* defined(FEATURE_GSM_GPRS_SNDCP_PCOMP)) || ... */


/*=============================
  SNDCP Up-Link entity
==============================*/
typedef struct
{
  /*-----------------------------------------------------------------------
  Suspend flag is set when UL data flow is suspended due to:
  GSN_UL_SUSP_CTS_OFF   : LLC has requested (activated) flow control
  GSN_UL_SUSP_XID_PEND  : data flow will resume after LL_XID_CNF
                          or collision resolution
  GSN_UL_SUSP_EST_PEND  : data flow will resume after LL_ESTABLISH_CNF
                          or LL_ESTABLISH_IND
  GSN_UL_SUSP_SEQ_PEND  : Due to LL_RESET_IND data flow will resume
                          after SNSM_SEQUENCE_IND
  GSN_UL_SUSP_MAX_NPDU  : Number of N-PDUs buffered by SNDCP
                          has reached GPRS_MAX_RETR_NPDUS
  GSN_UL_SUSP_XID_RESET : LLC will trigger suspension on receiving XID RESET 
                          from NW. LLC will resume on receiving LL_RESET_RESP.
  -----------------------------------------------------------------------*/
  uint8           susp_flag;

  /* In Ack mode all NPDUs are stored untill acked */
  q_type          npdu_buff;

  /* Flag indicating validity of npdu_buff queue for a given NSAPI. */
  boolean         npdu_buff_valid;

  /* TRUE when NSAPI has buffered NPDUs ready for retransmission */
  boolean         retr;

  /* Send NPDU count for Ack operation. Modulo 256 */
  uint8           ack_send_npdu;

  /* Send NPDU count for UnAck operation. Modulo 4096 */
  uint16          unack_send_npdu;

} gsn_ule_t;

/*============================
   SNDCP Down-Link entity
=============================*/
typedef struct
{
  gsn_dle_state_t       state;          /* state of Down-link entity */
  gsn_reas_state_t      reas_state;     /* reassembly state */
  dsm_item_type         *reas_buff;     /* pointer to Reassembly buffer */

  /* Holds the previous seg number during reassembly of SN-UNITDATA PDUs. Mod 16 */
  uint8                 prv_seg;

  /* Holds DCOMP, PCOMP and N-PDU number of 1st Seg */
  gsn_first_seg_info_t  first_seg_info;

  /* Receive NPDU count for Ack operation. Modulo 256 */
  uint8                 ack_rcv_npdu;

  /* Receive NPDU count for UnAck operation. Modulo 4096.
  ** Note, that this counter is needed only for detecting
  ** lost DL SN-PDUs in order to inform the decompessor
  ** (and the PDP, if needed) about the lost frame(s).
  */
  uint16                unack_rcv_npdu;


  /* Queue for storing receiving NPDU segments. Once all segments are received
  ** and re-ordering of an NPDU is completed, it is sent to reassembly module.
  **
  ** In this segment re-ordering buffer, NPDU segment reference items are
  ** stored. Each NPDU reference item contains NPDU number and a queue of
  ** segments belonging to that NPDU.
  **  
  ** When a PDP context is activated, segment re-ordering queue is created for
  ** that NSAPI. When PDP context is deactivated, any remaining NPDU reference
  ** items and attached segments are discarded and memory is freed. And segment
  ** queue attached to each NPDU reference item and re-ordering queue are
  ** destroyed.
  */
  q_type                npdu_ref_buff;

  /* Indicates whether npdu_ref_buff is valid */
  boolean               npdu_ref_buff_valid;


} gsn_dle_t;


/*============================
   SNDCP NPDU reference item
=============================*/
typedef struct
{
  q_link_type link;

  /* N-PDU number */
  uint16      npdu_num;     

  /* Segment queue */ 
  q_type      seg_reorder_buff;

  /* Indicates whether seg_reorder_buff is valid */
  boolean     seg_reorder_buff_valid;

} gsn_npdu_ref_item_t;


/*============================
   SNDCP NPDU segment item
=============================*/
typedef struct
{
  q_link_type            link;

  /* Pointer NPDU segment header. */
  gsn_unitdata_pdu_hdr_t sn_hdr;

  /* Pointer to NPDU segment */ 
  dsm_item_type          *seg_ptr;

} gsn_seg_item_t;


/*================================
   SNDCP SAPI entity
================================*/
typedef struct
{
  gsn_sapi_t        sapi;         /* SAPI number */
  uint32            tlli;         /* assigned TLLI */
  gsn_sapi_state_t  state;        /* Sate of SAPI entity */
  q_type            nsapi_ptr_q;  /* Queue of pointers of NSAPI entities mapped on SAPI  */

  /* Flag indicating validity of NSAPI pointer queue attached to this SAPI. */
  boolean           is_nsapi_ptr_q_valid;

  gsn_xid_state_t   xid_state;    /* state of XID request/negotiation process */
  uint8             xid_req_cnt;  /* Counter for XID request renegotiation */

  /* Set TRUE in case of re-establishment  */
  boolean           re_establish;

  /* Set TRUE if MS SNDCP is the originator of the establishment
  ** (or re-establishment) procedure.
  */
  boolean           link_estab_originator;

#if( (defined(FEATURE_GSM_GPRS_SNDCP_PCOMP)) || (defined(FEATURE_GSM_GPRS_SNDCP_DCOMP)) )

  /* Points to the latest requested XID block.
  ** To be used in the case of retransmission
  */
  dsm_item_type     *xid_req_dup_ptr;

#endif

#ifdef FEATURE_GSM_GPRS_SNDCP_PCOMP


  /* Holds pointers to PCI comp entities. Pointers are NULL on start up.
  ** A PCI comp entity is dynamicaly created when an entity is selected or assigned.
  */
  pcomp_e_t         *pcomp_e_pool[ENT_NUM_MAX +1];

  /* Holds pointers to PCOMP values. Pointers are NULL on start up. A PCOMP value is
  ** dynamicaly created when a value is selected or assigned to an algorithm.
  */
  pcomp_t           *pcomp_pool[PCOMP_VAL_MAX +1];

  /* Selected or assigned PCI comp entities that use the same algorithm are
  ** linked together in the algorithm's queue. The queues hold t2_alg_q_t items.
  */
  q_type            t2_alg_q[T2_ALG_MAX + 1];


#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP */

#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP


  /* Holds pointers to Data comp entities. Pointers are NULL on start up.
  ** A Data comp entity is dynamicaly created when an entity is selected or assigned.
  */
  dcomp_e_t         *dcomp_e_pool[ENT_NUM_MAX +1];

  /* Holds pointers to DCOMP values. Pointers are NULL on start up. A DCOMP value is
  ** dynamically created when a value is selected or assigned to an algorithm.
  */
  dcomp_t           *dcomp_pool[DCOMP_VAL_MAX +1];

  /* Selected or assigned data comp entities that use the same algorithm
  ** ( only V42 bis is supported ) are linked together in the algorithm's queue.
  ** The queue holds t1_alg_q_t items.
  */
  q_type            t1_alg_q[T1_ALG_MAX + 1];


#endif /* FEATURE_GSM_GPRS_SNDCP_DCOMP */
 
  gas_id_t          gas_id;


} gsn_sapi_e_t;


/* Type definition for PDP DL data function  */
typedef void(*pdp_dl_fnc_t)( void* ds_context, dsm_item_type **npdu );

/* Type definition for PDP Registration Confirmation Callback */
typedef void(*pdp_reg_cnf_fnc_t)( void* ds_context, boolean success );


/*=================================================
  SNDCP Entity. Attached to an NSAPI in SN-SAP
==================================================*/
typedef struct gsn_nsapi_e
{
  gsn_nsapi_t       nsapi;            /* NSAPI number */
  uint8             radio_pr;         /* Radio Priority */
  uint8             pfi;              /* Packet Flow Id */
  qos_T             qos;              /* Quality of service */
  gsn_nsapi_mode_t  op_mode;          /* Ack or UnAck mode of operation */
  gsn_ule_t         ule;              /* Up-link entity of the NSAPI */
  gsn_dle_t         dle;              /* Down-link entity of the NSAPI */
  gsn_sapi_e_t      *sapi_ptr;        /* Pointer to the SAPI that NSAPI is mapped */
  boolean           first_dl_npdu;    /* TRUE untill the first DL N-PDU is received */

  /* PDP data characteristics. Set upon PDP registration */
  sys_pdp_data_comp_e_type    dcomp_alg; /* dcomp algorithm */
  sys_pdp_header_comp_e_type  pcomp_alg; /* pcomp algorithm */
  gsnsm_pdp_type    pdp_type;         /* packet data protocol type */
  boolean           data_comp;        /* TRUE if data compression is requested by PDP */
  boolean           pci_comp;         /* TRUE if PCI compression is requested by PDP */
  boolean           cipher;           /* TRUE if ciphering is requested by PDP */

  /* PDP data flow control function pointers. Set upon PDP registration */
  wm_cb_type        pdp_ul_suspend_fnc;
  wm_cb_type        pdp_ul_resume_fnc;

  /* PDP function pointer for forwarding DL data. Set upon PDP registration */
  pdp_dl_fnc_t      pdp_frw_dl_data_fnc;

  /* PDP Registration Confirmation Pending Callback. Set upon PDP registration. */
  pdp_reg_cnf_fnc_t pdp_reg_cnf_fnc;

  /* PS context for DL data.  Set upon PDP registration */
  void              *ds_context;

  /* Set TRUE if NSAPI have to be initialised after LL_RELEASE_CNF */
  boolean           init_nsapi;

  /* Set TRUE if NSAPI have to be delinked from SAPI after LL_RELEASE_CNF */
  boolean           delink_nsapi;

  /* Set TRUE when  SNSM-ACTIVATE.IND primitive is recieved  */
  boolean           actv_resp_pend;

  /* Set TRUE when SNSM-DEACTIVATE.IND primitive is received*/
  boolean           deactv_resp_pend;

  /* Set TRUE when SNSM-MODIFY.IND primitive is received */
  boolean           modif_resp_pend;

  /* Set TRUE when LL_ESTABLISH.REQ primitive is send due to SNSM-MODIFY.IND */
  boolean           modif_rel_resp_pend;

  /* Set TRUE when LL_RELEASE.REQ primitive is send due to SNSM-MODIFY.IND */
  boolean           modif_est_resp_pend;

  /* Set TRUE when SN-XID.REQ primitive is received from PDP */
  boolean           sn_xid_resp_pend;

#if( (defined(FEATURE_GSM_GPRS_SNDCP_PCOMP)) || (defined(FEATURE_GSM_GPRS_SNDCP_DCOMP)) )

  gsn_sn_xid_req_t  sn_xid_req;       /* XID parameters requsted by PDP */

#endif


#ifdef FEATURE_GSM_GPRS_SNDCP_PCOMP

  /* Holds the numbers of the PCI entities that can be used in the NSAPI.
  ** Note, that there is no way, in the down link, to distinguish between two
  ** PCI compression entities which are mapped to the same NSAPI and use the
  ** same compression algorithm from the PCOMP value on the SN_DATA_IND or SN_UNITDATA_IND header.
  ** This implies one NSAPI can only have one compression entity that uses a specific algorithm.
  ** This is why this array holds T2_ALG_MAX + 1 (type 2 algorithms are PCI algorithms)
  ** PCI compression entities.
  */
  uint8             pcomp_e_num[T2_ALG_MAX +1];

#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP */

#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP

  /* Holds the numbers of Data compression entities that can be used in the NSAPI.
  ** The comment above, applies for Data comp entities too.
  */
  uint8             dcomp_e_num[T1_ALG_MAX +1];

#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP */

  /* For more information regarding SNDCP diagnostic monitoring
  ** refer to gsndcp.h.
  */
  gsndcp_diag_pdu_stat_rsp_t    diag_pdu_stat_rsp;

  /* The following parameters are used when the interface between the mode
  ** Handler and SNDCP is watermark based.
  */
  dsm_watermark_type    *to_gsndcp_wm;  /* Tx WM Ptr registered with SNDCP */
  dsm_watermark_type    *from_gsndcp_wm;/* Rx WM Ptr registered with SNDCP */
  void                  (*from_gsndcp_post_rx_func_ptr) (byte,byte,void *);
                                             /* SNDCP Rx Callback Function */
  void                  *from_gsndcp_rx_func_parm;
                                       /* SNDCP Rx Callback Function Param */
  comp_sig_enum_type    to_gsndcp_sig;             /* SNDCP Tx Data Signal */

#ifdef FEATURE_DUAL_SIM
  gas_id_t              gas_id;        /* GAS_ID  */
#endif /* FEATURE_DUAL_SIM */
  
  /* If SNSM_MODIFY_IND indicates reliability class 1 or 2, then SNDCP starts
  ** this timer. When SGSN initiated LLC link is established this timer is
  ** stopped. If SGSN fails to establish LLC link and this timer time out,
  ** then SNDCP informs of this failure to SM via SNSM_STATUS_REQ with
  ** cause SNSM_MODIFY_FAILURE.
  */
   
  rex_timer_type        sgsn_initiated_link_est_fail_timer;
 
} gsn_nsapi_e_t;


/* Type used for active NSAPI queueing */
typedef struct
{
  q_link_type         link;
  gsn_nsapi_e_t       *nsapi_ptr;

}gsn_nsapi_q_t;


/* Type used for storing N-PDUs in npdu_buff while waiting to be acked
** by LLC (Rel Class 1, 2) or RLC (Rel Class 3,4,5).
*/
typedef struct
{

  q_link_type     link;

  /*Send NPDU count for Ack operation. Modulo 256 */
  uint8           ack_send_npdu;

  /*Send NPDU count for Unack operation. Modulo 4096 */
  uint16          unack_send_npdu;

  /* TRUE if N-PDU has been retransmitted */
  boolean         re_tr_flag;

  /* TRUE if all segments of the NPDU have been confirmed */
  boolean         npdu_acked;

  /* Segmentation buffers. Hold segments' reference
  ** numbers to be acked for each N-PDU
  */
  q_type          seg_ref_buff;

  /* Flag indicating validity of seg_ref_buff for this NPDU */
  boolean         seg_ref_buff_valid;

  dsm_item_type   *npdu_ptr;      /* N-PDU pointer */

  /* Indicates whether PDU in retransmit queue is a feedback PDU or not.
  ** Feedback PDUs are removed upon RC2 link re-establishment.
  */
  boolean         fback_pdu_flag;

} gsn_npdu_item_t;


/* Type used for storing N-PDU segment references in seg_ref_buff while
** waiting to be acked by LLC (Rel Class 1, 2) or RLC (Rel Class 3,4,5).
*/
typedef struct
{
  q_link_type link;

  uint16      npdu_num; /* N-PDU number */
  uint8       seg_num;  /* segment number of the N-PDU */

} gsn_seg_ref_item_t;


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

/* Hold LLC SAPIs' N201_I and N201_U  */
extern uint16             gsn_n201_i[NUM_GERAN_DATA_SPACES][4];
extern uint16             gsn_n201_u[NUM_GERAN_DATA_SPACES][4];

/* If TRUE, Indicates that the SNDCP layer has been initialised  */
extern boolean sndcp_init;

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

/*----------------------------------------------------------
**
** Function prototypes for SNDCP internal command handling.
**
**---------------------------------------------------------*/

/*===========================================================================
===
===  FUNCTION      GSN_INT_GET_MES_BUF()
===
===  DESCRIPTION
===
===    Dynamically allocate sufficient memory from heap to accommodate a
===    message of type gsn_internal_msg_t.
===
===  DEPENDENCIES
===
===    None
===
===  RETURN VALUE
===
===    Pointer to allocated gsnsm_mes_type buffer.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

gsn_internal_msg_t *gsn_int_get_mes_buf( void );

/*===========================================================================
===
===  FUNCTION      GSN_INT_PUT_MES_BUF()
===
===  DESCRIPTION
===
===    Post the SNDCP internal message as given by the message pointer
===    to the SNDCP command queue.
===
===  DEPENDENCIES
===
===    SNDCP must be initialised.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

void gsn_int_put_mes( gsn_internal_msg_t *mes_ptr );


/*===========================================================================
===
===  FUNCTION      GSN_PROCESS_HIWATER()
===
===  DESCRIPTION
===
===    To be called when a high watermark has been reached in the SNDCP to LLC
===    watermark interface
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===   PDP UL data flow on the specified NSAPI might be resumed
===
===========================================================================*/

extern void gsn_process_hiwater( gas_id_t gas_id, gsn_sapi_t sapi, gsn_nsapi_mode_t  op_mode, uint8 susp_reason );


/*===========================================================================
===
===  FUNCTION      GSN_PROCESS_LOWATER()
===
===  DESCRIPTION
===
===    To be called when a low watermark has been reached in the SNDCP to LLC
===    watermark interface
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===   PDP UL data flow on the specified NSAPI might be resumed
===
===========================================================================*/

extern void gsn_process_lowater( gas_id_t gas_id, gsn_sapi_t sapi, gsn_nsapi_mode_t op_mode, uint8 susp_reason );


/*===========================================================================
===
===  FUNCTION      GSN_TIMER_CALLBACK()
===
===  DESCRIPTION
===
===    Sends an internal message to SNDCP's command queue to indicate the
===    expiry of sgsn initiated rel class 2 link establishment timer.
=== 
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

void gsn_timer_callback( uint32 gasid_nsapi_and_timer_id );



#endif /* INC_GSNI_H   */

/*** EOF: don't remove! ***/
