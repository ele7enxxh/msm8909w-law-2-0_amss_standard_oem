#ifndef MCCGCSNA_H
#define MCCGCSNA_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   C D M A    C S F B  G C S N A   S T A T E

GENERAL DESCRIPTION
  This module contains the functions that support LTE-1X CSFB Tunnel Operation.
  This file contains the lowest level state machine for the CDMA-CSFB operation.
  While being in CSFB state, Mobile shall operate in CSFB-IDLE,CSFB_ACCESS states.
  In CSFB-ACCESS state, CP shall tunnel GCSNA messages as appropriate.

EXTERNALIZED FUNCTIONS
  TO DO

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The Main Control task calls the mcc_subtask procedure,with entry reason of CSFB,
  after it has been CSFB activated by MMOC .

  Copyright (c) 2010 - 2014 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/mccgcsna.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/29/14   agh     Fix the GCSNA Option used for SRVCC
03/19/14   ppr     SRVCC Feature Changes
05/10/13   ppr     API to update prev in use to GCSNA Rej msg prev in use
01/31/13   gga     Additional changes to eliminate potential overflow
11/19/12   ssh     Added SMS over CSFB support.
11/20/12   ppr     Changes for not to flood NAS with messages if we havent heard
                   any good state/ bad state confirmation.
09/15/12   ppr     FR#2370 - 1x memory optimization changes
07/03/12   ssh     1xCSFB: Added support for doing a hard failure for GCSNA
                   rejects with cause 3,4,5.
06/14/12   ssh     CSFB: added support for duplicate detection.
04/26/12   ssh     CSFB: retry mech optimization.
03/13/12   jj      CSFB: Fix to not map retry failure to SOFT failure count.
02/16/12   srk     Replaced FEATURE_LTE_TO_1X by FEATURE_MODEM_1X_IRAT_LTO1X.
08/26/11   jj      CSFB: Support for GCSNA negotiation.
06/01/11   jj      CSFB: Bundled messages (UHDM+AWIM) support.
05/20/11   jj      CSFB: Externalized GCSNA variable, which will be used GCSNA
                   seq number calculation.
05/16/11   jj      CSFB: Added changes to tunnel Reject Order if there are any
                   unsupported messages as per GCSNA Option.
04/21/11   jj      CSFB: Added chanes to support SSD operation, Unique Challenge
                   in CSFB mode.
04/20/11   jj      CSFB: Changes to support Auth record in TLAC header.
12/22/10   ssh     Merged MSGR queue signal with CMD_Q_SIG. And reduced the
                   GCSNA txn buffer to be one.
01/12/11   jj      CSFB: Update GCSNA window depth.
01/12/11   jj      CSFB: Added changes to support sending of ORM in
                   ULHandoverPreparationTransfer.
12/15/10   ssh     CSFB: Added MT support contd. (GPM processing and
                   sending PRM support.)
12/14/10   jj      CSFB: Increased size of retxns_timer_value to correctly
                   store the value of retransmissions.
12/01/10   ssh     CSFB: Added support for MT calls.
11/28/10   jj      CSFB: Added support for CSFB -> CDMA handover.
11/18/10   jj      CSFB: Added support for MO from EUTRA.
11/16/10   jj      Changed gen_msg_type header which is used in GCSNA dl info
                   transfer messages.
11/15/10   ssh     CSFB: added MO call support.
11/15/10   jj      CSFB: Fixed compiler warnings.
11/11/10   jj      CSFB redesign
09/30/10   ssh     CSFB: added support for GCSNA option Negotiation.
09/24/10   ssh     CSFB: Added retry and retransmission support.
09/23/10   jj      Support for Reg context update.
09/01/10   jj      Updated code to support tunneled RGM.
08/19/10   jj      CSFB: Moved some of the data declarations to mccgcsna.c
08/18/10   jj      CSFB: Added state level implementation.
06/21/10   jj      Created module to support LTE-1X CSFB operation.
06/21/10   jj      Update state machine for CSFB operation.


===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "1x_variation.h"
#include "modem_1x_defs.h"
#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
#include "mcccsfb.h"
#include "bit.h"
#include "caii_v.h"
#include "comdef.h"
#include "queue.h"
#include "customer.h"
#include "caix.h"
#include "caix_i.h"

/*===========================================================================
                        DATA DECLARATIONS
===========================================================================*/

/* Max size of GCSNA PDU - in bytes */
#define MCC_GCSNA_MAX_MSG 2048

/* GCSNA Options - according to these options message will be parsed
 * The GCSNA Option is itself divided into two subfields, where the upper 5 
 * bits represent the GCSNA Class, and the lower 3 bits represent 
 * GCSNA Class Revision.
 *
 *       From C.R1001_I_v2.0 specs
 *     Table 14-1. GCSNAClass Definitions
 *  GCSNA Class     GCSNA ClassRevision       1x Service
 *   0                  0               Release 8 1xCSFB from E-UTRAN
 *   0                  1               Release 9 e1xCSFB from E-UTRAN
 *   0                  2               C.S0097-A supported eCSFB
 *   1                  0               SRVCC from E-UTRAN
 */
#define GCSNA_OPTION_0 0   /* GCSNA Class - 0, GCSNA class revision - 0 */
#define GCSNA_OPTION_1 1   /* GCSNA Class - 0, GCSNA class revision - 1 */
#define GCSNA_OPTION_2 2   /* GCSNA Class - 0, GCSNA class revision - 2 */
#define GCSNA_OPTION_8 8   /* GCSNA Class - 1, GCSNA class revision - 0 */

/* GCSNA Option to be used - hard coded to 1 for now */
#define GCSNA_OPTION 1

/* GCSNA Alt Option to be used in gcsna negotiation .
 * Hard coded to 255 for now
*/
#define MAX_NUM_ALT_GCSNA_OPT 255

/* Max number of TLCA header records */
#define MAX_NUM_TLAC_HDR_REC  15

/* TLAC header max length - 8*255 Bits */
#define MAX_TLAC_HDR_REC_LEN  8*255

/* L3 PDU header max length - 8*512 Bits */
#define MAX_TLAC_L3_PDU_LEN   8*512

/* Backoff timer for retries. the value is in ms. */
#define MAX_RETRY_BACKOFF_TIMER_VALUE 500

/* Maximum time to attempt retries for the first transmission. */
#define MAX_RETRY_DURATION 15000

/* Maximum number of retries of a message. */
#define MAX_RETRY_COUNT MAX_RETRY_DURATION/MAX_RETRY_BACKOFF_TIMER_VALUE

/* Adderess record in the TLAC header */
#define TLAC_ADDR_REC 0

/* Auth record in the TLAC header */
#define TLAC_AUTH_REC 1

/* GCSNA Reject reasons */
#define GCSNA_OPT_MISMATCH 0  /* GCSNA Reject because of GCSNA OPT MISMATCH */
#define GCSNA_PREV_MISMATCH 1  /* GCSNA Rejct because of GCSNA PREV MISMATCH */
#define GCSNA_OPT_PREV_MISMATCH 2  /*  GCSNA Reject because of BOTH GCSNA OPT and PREV */
#define GCSNA_INV_MSG_ID 3 /* GCSNA Reject because of invalid message ID */
#define GCSNA_C2K_PARAMS_PROV_UNSUP 4 /* GCSNA Reject because of unsupported
                                         1xParameters provisioning */
#define GCSNA_UNSUP_REC_IN_C2K_PARAMS 5 /* GCSNA Reject because of unsupported
                                           RecordType in 1xParameters message*/

/* -------------------------------------------------------------------------
** The following data declarations are representations of the GCSNA messages
** found in C.S0097 v 1.0 (see sections 2.4 onwards ).  Each bit in the message is
** represented by a byte in the data structure.
** ------------------------------------------------------------------------ */
typedef enum
{
  /* Circuit Service message */
  GCSNA_1X_CRKT_SRV_MSG = 0x01, /* MS and BS can both send it */
  /* L2 ack  */
  GCSNA_L2_ACK          = 0x02, /* MS and BS can both send it */
  /* Service Reject message */
  GCSNA_SRV_REJ_MSG     = 0x03, /* MS and BS can both send it */
  /* 1X Parameters Request message */
  GCSNA_1X_PARAM_REQ    = 0x04, /* Only MS can send it */
  /* GCSNA Invalid message Id */
  GCSNA_INVALID_MSG
}mcc_gcsna_msg_type;

/* For the 1X Circuit Service Messages, we need to know what UL message
 * we should be using to send it across to network.
 * o  For the uplink messages like ORM/PRM we should be using
 *    ULHandoverPreparationTransfer.
 * o  For the rest of the uplink messages lile RGM we should be
 *    using ULInfoTrasfer.
 */
typedef enum
{
  /* use UL Info Transfer message */
  GCSNA_USE_UL_INFO_TX_MSG_TYPE,
  /* use UL Handover Preparation message */
  GCSNA_USE_UL_HO_PREP_MSG_TYPE,
  /* None - this will be used for 1x parm rqst msg */
  GCSNA_USE_NONE_TYPE,
}gcsna_ul_tx_type;

/* 1X Circuit Service message - TLAC header which could be
 * received or sent on Tunnel
 */

/* TLAC  Header */
typedef struct
{
  byte  logical_chn ;   /* Message type */
  byte  prev_1x;        /* Message Sequence */
  byte  msg_type;       /* Reserved Field */
  byte  tlac_hdr_rec_num;
  byte  tlac_hdr_rec_type;
  byte  tlac_hdr_rec_len;
  /* TLACHeaderRecord Start */
  byte msid_type;       /* Mobile station identifier field type */
  byte msid_len;        /* Mobile station identifier field length */
  caii_msid_type msid;  /* Mobile station identifier */
  /* Authentication fileds in TLAC header */
  boolean auth_incl;    /* Flag indicating if auth is supported */
  dword authr;          /* Authr */
  byte randc;           /* Rand value */
  byte count;           /* Auth count */
  /* MI fileds in TLAC header */
  boolean maci_incl;    /* These fileds are not valid if MI is not supported */
  byte sdu_key_id;      /* Not a valid field if MI is not supported */
  byte sdu_integrity_algo;  /* Not a valid field if MI is not supported */
  boolean sdu_sseq_or_sseqh;  /* Not a valkid filed if MI is not supported */
  byte sdu_sseq;  /* Not a valid filed if MI is not supported */
  byte sdu_sseq_h;  /* Not a valid filed if MI is not supported */
}tlac_hdr_type;

/* Uplink Tunnel message header type */
typedef struct
{
  byte  msg_id;
}gcsna_ul_gen_msg_type;

/* Uplink 1x circuit service message , which can be sent over
 * Tunnel
 * TLAC  Header + CAI L3 part
*/
typedef union
{
   caii_gen_ac_type  gen_ac;
   caii_reg_type  reg;
   caii_orig_type orig;
   caii_page_resp_type  page_resp;
   caii_ac_ord_type ac_ord;
   caii_auth_ch_resp_type auth_ch;
   caii_ac_burst_type ac_burst;
 } l3_msg_type;

typedef struct
{
  tlac_hdr_type  tlac_hdr;
  l3_msg_type l3_msg;
}gcsna_ul_1x_crkt_srv_msg_type;

 /* Uplink GCSNA ack , which can be sent over tunnel */
 typedef struct
{
  byte  msg_id;
  byte  ack_seq;
  byte  gcsna_reserved;
}gcsna_ul_ack_pdu_type;

/* Uplink GCSNA ack , which can be sent over tunnel */
typedef struct
{
 byte msg_id;
 byte rej_seq;
 byte cause;
 byte num_fields;
 byte gcsna_opt;
 byte prev_1x;
 byte msg_id_inv;
}gcsna_ul_rej_pdu_type;

/*Uplink GCSNA message Union of GCSNA Internal messages  */
typedef union
{
  gcsna_ul_gen_msg_type  gen_msg_id;
  gcsna_ul_1x_crkt_srv_msg_type  crkt_srv_msg;
  gcsna_ul_ack_pdu_type           gcsna_ack;
  gcsna_ul_rej_pdu_type  gcsna_rej;
}gcsna_ul_msg_type;

/* Status to indicate if L3 msg has been rxed or not - this is used in DL
   messages.
 */
typedef enum
{
  MCC_CSFB_L3_MSG_NOT_RXED = 0,        /* DL Message has not been received */
  MCC_CSFB_L3_MSG_RXED_INCORRECT,      /* Wrong L3 msg has been rxed */
  MCC_CSFB_L3_MSG_RXED,                /* DL Message has been received */
  MCC_CSFB_L3_MSG_GCSNA_OPT_REJ,      /* DL Message is rejected because
                                         of GCSNA OPT mismatch */
  MCC_CSFB_L3_MSG_GCSNA_PREV_REJ,      /* DL Message is rejected because
                                          of PREV mismatch */
  MCC_CSFB_DUP_L3_MSG_RXED             /* DL msg is a duplicate msg from NW. */
}dl_l3_msg_status_type;

/* Virtual channels on which a GCSNA message is received  */
typedef enum
{
  GCSNA_CHN_PC,   /* Virtual Paging Channel, it can be mapped to CAIX_PC */
  GCSNA_CHN_TC    /* Virtual Traffic Channel, it can be mapped to CAIX_TC */
}gcsna_chan_type;

/* DL 1X Ciruit Service Message header type  */
typedef struct
{
  byte  msg_id;  /* GCSNA message type */
  dl_l3_msg_status_type is_l3_msg; /* Indicates if 1X RTT msg is received */
  gcsna_chan_type  cdma_chn;  /* Indicates which channel is the message received */
}gcsna_dl_gen_msg_type;

/* DL 1X Circuit Service Message Alternate GCANA option */
typedef struct
{
  byte  alt_gcsna_opt;      /* Alternate GCSNA option */
} alt_gcsna_opt_type;


/* DL 1X Circuit Service Messgae TLAC Header type */
typedef struct
{
  byte  tlac_hdr_rec_type; /* TLAC header record type */
  byte  tlac_hdr_rec_len;  /*TLAC header record length */
  byte  tlac_hdr_rec[MAX_TLAC_HDR_REC_LEN];  /*TLAC header rec */
} tlac_rec_type;


/* DL 1x circuit service msg */
typedef struct
{
  /* Generic Message header */
  byte  msg_id;                    /* GCSNA Message ID */
  dl_l3_msg_status_type is_l3_msg; /* Flag indicating if l3 message is rxed */
  caix_chan_type  cdma_chn; /* Indicates which channel is the message received */

  /* GCSNA header */
  byte  gcsna_opt;   /* GCSNA option*/
  byte  alt_gcsna_opt_incl; /* Alternate GCSNA option included */
  byte  num_alt_gcsna_opt;  /* Number of alternate GCSNA option included*/
  alt_gcsna_opt_type alt_gcsna_options[MAX_NUM_ALT_GCSNA_OPT];
  byte  iws_id_incl;     /* IWS id included */
  word  iws_id;         /* IWS id */
  byte  ack_reqd;        /* Ack required */
  byte  stop_dup_detect; /* Stop Duplicate detection */
  byte  msg_seq;   /* Message sequence */
  byte  num_bundled_tlac_l3_rec;  /* Num of bundled records */
  byte  gcsna_reserved;  /*GCSNA reserved bits */

  /* TLAC Header */
  byte  logical_chn;    /* Logical channel */
  byte  prev_1x;        /* 1x protocol revision */
  byte  msg_type;       /* Message type */
  byte  num_tlac_hdr_rec; /* Number of TLAC header records */
  tlac_rec_type tlac_rec[MAX_NUM_TLAC_HDR_REC]; /* Tlac record */
  byte  tlac_reserved;  /* Reserved bits */

  /* L3 pdu */
  word l3_pdu_len;
  caii_rx_msg_type l3_msg;    /* translated message */

  /* Bundled Record */
  /* TLAC Header */
  byte  bundled_logical_chn;    /* Logical channel */
  byte  bundled_prev_1x;        /* 1x protocol revision */
  byte  bundled_msg_type;       /* Message type */
  byte  bundled_num_tlac_hdr_rec; /* Number of TLAC header records */
  tlac_rec_type bundled_tlac_rec[MAX_NUM_TLAC_HDR_REC]; /* Tlac record */
  byte  bundled_tlac_reserved;  /* Reserved bits */

  /* Unprocessed L3 pdu */
  word bundled_l3_pdu_len;
  byte bundled_l3_msg[MAX_TLAC_L3_PDU_LEN];  /* Unprocessed message */
}gcsna_dl_1x_crkt_srv_type;

 /* Downlink GCSNA ack , which can be sent over tunnel */
 typedef struct
{
  byte  msg_id;  /* GCSNA message type */
  dl_l3_msg_status_type is_l3_msg; /* Indicates if 1X RTT msg is received */
  caix_chan_type  cdma_chn; /* Indicates which channel is the message received */
  byte  ack_seq;
  byte  gcsna_reserved;
}gcsna_dl_ack_pdu_type;

/* Downlink GCSNA Rej , which can be sent over tunnel */
typedef struct
{
 byte msg_id;
 byte rej_seq;
 byte cause;
 byte num_fields;
 byte gcsna_opt;
 byte prev_1x;
 byte msg_id_inv;
}gcsna_dl_rej_pdu_type;

/* Union of DL GCSNA rxed messages  */
typedef union
{
  gcsna_dl_gen_msg_type      gen_msg_id;
  gcsna_dl_1x_crkt_srv_type  ckt_srv_msg;
  gcsna_dl_ack_pdu_type      gcsna_ack;
  gcsna_dl_rej_pdu_type      gcsna_rej;
}gcsna_dl_msg_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*    Messages -  For Packing/Unpacking External to MC format                                       */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* 1X Circuit service message */

/* GCSNA header   */
typedef struct
{
  byte  msg_id[8];
  byte  gcsna_opt[8];
  byte  alt_gcsna_opt_incl[1];
  byte  iws_id_incl[1];
  byte  ack_reqd[1];
  byte  stop_dup_detect[1];
  byte  msg_seq[6];
  byte  num_bundled_tlac_l3_rec[2];
  byte  gcsna_reserved[4];
}gcsna_pdu;

/* TLAC  header */
typedef struct
{
  byte  logical_chn[1];                /* Message type */
  byte  prev_1x[8];               /* Message Sequence */
  byte  msg_type[8];              /* Reserved Field */
  byte  tlac_hdr_rec_num [4];
  byte  tlac_hdr_rec_type[4];
  byte  tlac_hdr_rec_len[8];
  byte  tlac_hdr_rec[8];
  byte  tlac_l3_pdu_len[16];
  byte  tlac_reserved_7[7];
  byte  tlac_reserved_3[3];
 }tlac_pdu;

typedef PACK(struct)
{
  byte  alt_gcsna_opt[8];      /* Alternate GCSNA option */
} alt_gcsna_opt_field_type;

typedef PACK(struct)
{
  byte  tlac_hdr_rec_type[4]; /* TLAC header record type */
  byte  tlac_hdr_rec_len[8];  /*TLAC header record length */
  byte  tlac_hdr_rec[MAX_TLAC_HDR_REC_LEN];  /*TLAC header rec */
} tlac_rec_field_type;


/* 1x circuit service msg */
typedef PACK(struct)
{
  /* GCSNA 1x circuit service header */
  byte  msg_id[8];      /*Message Id*/
  byte  gcsna_opt[8];   /* GCSNA option*/
  byte  alt_gcsna_opt_incl[1]; /* Alternate GCSNA option included */
  byte  num_alt_gcsna_opt[8];  /* Number of alternate GCSNA option included*/
  alt_gcsna_opt_type alt_gcsna_options[MAX_NUM_ALT_GCSNA_OPT];
  byte  iws_id_incl[1];     /* IWS id included */
  byte  iws_id[16];         /* IWS id */
  byte  ack_reqd[1];        /* Ack required */
  byte  stop_dup_detect[1]; /* Stop Duplicate detection */
  byte  msg_seq[6];   /* Message sequence */
  byte  num_bundled_tlac_l3_rec[2];   /* Number of bundled messages */
  byte  gcsna_reserved[4];  /*GCSNA reserved bits */

  /* TLAC Header */
  byte  logical_chn[1];    /* Logical channel */
  byte  prev_1x[8];        /* 1x protocol revision */
  byte  msg_type[8];       /* Message type */
  byte  num_tlac_hdr_rec[4]; /* Number of TLAC header records */
  tlac_rec_type tlac_rec[MAX_NUM_TLAC_HDR_REC]; /* Tlac record */
  byte  tlac_reserved[7];  /* Reserved bits */

  /* L3 pdu */
  byte l3_pdu_len[16];   /* TLAC L3 PDU length */
  byte l3_pdu[MAX_TLAC_L3_PDU_LEN]; /* TLAC L3 PDU */
}gcsna_1x_crkt_srv_msg_type;

/* GCSNA ACK  */
typedef struct
{
  byte  msg_id[8];
  byte  ack_seq[6];
  byte  gcsna_reserved[2];
}gcsna_ack_pdu_type;

/* GCSNA REJECT  */
typedef struct
{
  byte  msg_id[8];
  byte  rej_seq[6];
  byte  cause[8];
  byte  num_fields[8];
  byte  gcsna_opt[8];
  byte  rec_type[8];
  byte  prev_1x[8];
  byte  msg_id_inv[8];
  byte  gcsna_reserved[2];
}gcsna_rej_pdu_type;

/* GCSNA retry times */

typedef enum{
  MCC_GCSNA_NO_TIMER,
  MCC_GCSNA_RETRY_BACKOFF_TIMER,
  MCC_GCSNA_RETXN_TIMER
}mcc_gcsna_running_timer_type_e;

/* Basic element which will get retransmitted */
typedef struct{
  /* Message that needs to be transmitted*/
    emm_irat_ul_msg_req_type msg_tx;
  /* To know if the msg is in use or not. */
    boolean msg_in_use;
  /* Boolean to store if the Ack is required. */
    boolean ack_req;
  /* Msg sequence number that needs to be matched with the rxed ack_seq */
    byte msg_seq;
  /* Number of retry attempts made for this message */
    byte num_remaining_txns;
  /* Number of retransmission attempts made for this message */
    byte num_remaining_retries;
  /* retransmission timer count value for this message */
    word retxns_timer_value;
  /* Transmission Timer shared between retry back-off and retransmission. */
    rex_timer_type txn_timer;
  /* Enum to differentiate between retry back-off and retransmission timers. */
    mcc_gcsna_running_timer_type_e timer_type;
  /* Retry Mechanism is needed only for the first message transmission. Once we txt the msg,
   * we should follow the retransmission block from the specs. This is a boolean to track
   * if this is the first time the message is txted over the air. */
    boolean is_this_first_txn;
  /* Flag to indicate if NAS has received/ acknowledged the message transmission.
   * without this flag being set, we will not further flood NAS with messages,
   * because we have a tight binding with NAS reporting us a failure/success
   * for each message transmission.
   */
    boolean is_msg_tx_rsp_rxed;
}gcsna_msg_sent_type;

/* Structure used to store the sent msgs. This is required for ack_seq
   matching */
extern gcsna_msg_sent_type gcsna_msg_sent;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Return status for GCSNA RX Processing */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Internal format of uplink GCSNA messages which are transmitted on Tunnel */
extern gcsna_ul_msg_type mcc_gcsna_int_msg;

/* Pointer holds downlink GCSNA messages format which are rxed on Tunnel */
extern gcsna_dl_msg_type *mcc_gcsna_ext_msg_ptr;

/* Variable to track GCSNA Option. This will be defaulted to GCSNA_OPTION_1
 * everytime we start CSFB mode of operation and will be negotiated to suite
 * the need.
 */
extern byte gcsna_option_inuse;

/* Variable to track GCSNA message seq for 1X CRKT Service Message */
extern byte gcsna_msg_seq;
/*===========================================================================
                                 FUNCTIONS
===========================================================================*/

/*===========================================================================

FUNCTION MCC_GCSNA_MSG_TX

DESCRIPTION
  This function is a common function which gets called for any GCSNA transmission.
  Based on the msg_id to be transmitted , packing for the message is done and then
  the message is sent over LTE tunnel.

 DEPENDENCIES
  None.

RETURN VALUE
  TO BE FILLED

SIDE EFFECTS
  None.

FOOT NOTE:
  mcc_gcsna_msg_type :
   It tells what message has to be transmitted.

===========================================================================*/
extern void mcc_gcsna_msg_tx(mcc_gcsna_msg_type gcsna_msg_id,
                             boolean ack_req,
                             gcsna_ul_tx_type ul_txssion_type);

/*===========================================================================

FUNCTION MCC_GCSNA_TRANSMIT

DESCRIPTION
  This function transmits/retransmits the GCSNA msg.Retransmissions are
  attempted if the L2 ack timer has expired and the L2 ack is not yet
  received after sending the MSG to NW

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mcc_gcsna_transmit(void);

/*===========================================================================

FUNCTION MCC_GCSNA_RETRY

DESCRIPTION
  This function retries the txn of the GCSNA msg. Retry is attempted when the
  msg was not sent by NAS to NW.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mcc_gcsna_retry(void);

/*===========================================================================

FUNCTION MCC_CSFB_GCSNA_STOP_TXN

DESCRIPTION
  This function aborts the transmission of the GCSNA msg.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mcc_csfb_gcsna_stop_txn(void);

/*===========================================================================
FUNCTION  MCC_GCSNA_PROCESS_DL_INFO

DESCRIPTION
  This function processes DL info transfer message.These messages encapsulate
  1x RTT messages.

DEPENDENCIES
  None.

RETURN VALUE
  If the message processing has been successful.

SIDE EFFECTS
  None.
===========================================================================*/
extern void mcc_gcsna_process_dl_info(lte_irat_tunnel_msg_container_s *buf_ptr);


/*===========================================================================
FUNCTION  MCC_GCSNA_XLATE_L3_MSG

DESCRIPTION
  This function xlates the byte stream received in DL info transfer message/ MO
  from EUTRA to CDMA internal structures.The byte stream received has l3
  message embedded in it.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void mcc_gcsna_xlate_l3_msg(byte *buf_ptr, word msg_size);

/*===========================================================================
FUNCTION  MCC_GCSNA_PARSE_1X_CRKT_SRV_MSG

DESCRIPTION
  This function parses the 1X_CRKT_SRV_MSG.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void mcc_gcsna_parse_1x_crkt_srv_msg(uint8 *buf_ptr, uint32 len);

/*===========================================================================
FUNCTION  MCC_GCSNA_DO_GCSNA_OPTION_CHK

DESCRIPTION
  This function processes 1X CRKT SRV msg/ GCSNA Reject and checks if there is a
  compatible GCSNA Option that can be used/negotiated between BS/MS.

DEPENDENCIES
  None.

RETURN VALUE
  If suitable GCSNA Option has been found.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean mcc_gcsna_do_gcsna_option_chk(byte msg_type);

/*===========================================================================
FUNCTION  MCC_GCSNA_IS_VALID_OPTION_INUSE

DESCRIPTION
  This function checks if GCSNA Option Inuse is valid or not

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If gcsna_option_inuse is valid
  FALSE - If gcsna_option_inuse us invalid

SIDE EFFECTS
  None.
===========================================================================*/
boolean mcc_gcsna_is_valid_option_inuse(void);

/*===========================================================================
FUNCTION  MCC_GCSNA_DO_GCSNA_1X_PREV_CHK

DESCRIPTION
  This function processes 1X CRKT SRV msg/ GCSNA Reject and updates
  gcsna_prev_inuse global var for next GCSNA msg transmission.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If valid GCSNA 1XPrev rxed
  FALSE - If invalid GCSNA 1XPrev rxed

SIDE EFFECTS
  None.
===========================================================================*/
boolean mcc_gcsna_do_gcsna_1x_prev_chk(byte msg_type);

#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

#endif /* MCCGCSNA_H */
