/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

     C D M A   C S F B   G C S N A    F U N C T I O N S

GENERAL DESCRIPTION
  This module contains GCSNA protocol implementation.
  The module essentially provides the functionality of GCSNA Receiver and
  GCSNA Transmitter.
  Any message sent from LTE to 1X-CP shall be processed by GCSNA Receiver
  and any message sent from 1x-CP to LTE shall be sent by GCSNA Transmitter.

EXTERNALIZED FUNCTIONS
  TO DO

INITIALIZATION AND SEQUENCING REQUIREMENTS
  NONE

  Copyright (c) 2010 - 2015 Qualcomm Technologies, Inc.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/mccgcsna.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/07/15   sjo     Fixed Klocwork issues.
01/13/15   ppr     Feature clean up changes
10/29/14   ppr     Fix not to update gcsna_option_inuse to GCSNA_OPTION_8 in 
                   CSFB mode.
09/29/14   agh     Fix the GCSNA Option used for SRVCC 
08/21/14   jh      Added amssassert.h to remove dependency on qube.h 
07/03/14   ppr     Fix to update DB_BS_P_REV item along with updating 
                   p_rev_in_use while processing GCSNA Rej message
05/06/14   gga     1X_CP F3 redistribution
03/19/14   ppr     SRVCC Feature Changes
01/27/14   gga     Fix to make sure that MS comes out of access if the max
                   retries have been exhausted.
10/09/13   pap     Removing warnings for LLVM.
09/19/13   ppr     Changes to send the call_end_cnf rpt to CM in case of
                   Retransmission TMO while sending release order to n/w
05/10/13   ppr     Added API to validate GCSNA msg 1x prev rxed
05/06/13   cjb     Resolve compiler warnings
01/31/13   gga     Additional changes to eliminate potental overflow
01/17/13   ppr     Set MCC_GCSNA_RETRY_BACKOFF_TIMER for the first gcsna msg
                   transmission attempt
01/03/13   nee     Replacement of memcpy() to memscpy()
12/28/12   gga     KLlocwork Reviews
11/19/12   ssh     Added SMS over CSFB support.
11/20/12   ppr     Changes for not to flood NAS with messages if we havent heard
                   any good state/ bad state confirmation.
10/19/12   ppr     Added defensive checks
09/24/12   ppr     Added defencive checks for NULL ptr access
09/15/12   ppr     FR#2370 - 1x memory optimization changes
07/11/12   jj      CSFB: Changes to send tunneled Release Order when there is
                   call end before Hanodover is complete.
06/14/12   ssh     CSFB: added support for duplicate detection.
05/31/12   ssh     Removed the unwanted loggings.
05/24/12   jj      CW fix.
04/26/12   ssh     CSFB: retry mech optimization.
04/06/12   ssh     CSFB: corrected the handling of N_max_reg requirement.
03/13/12   jj      CSFB: CSFB NV Items support.
03/13/12   jj      Added new CSFB events to indicate Time Trasfer ,
                   Handover comletion and Receipt of GCSNA UL/DL messages.
03/13/12   jj      CSFB: Fix to not map retry failure to SOFT failure count.
02/16/12   srk     Replaced FEATURE_LTE_TO_1X by FEATURE_MODEM_1X_IRAT_LTO1X.
01/17/12   ppr     Feature Cleanup: Mainlined Always ON features
09/13/11   jj      CSFB: Support for Hard/Soft failures for Reg.
09/16/11   jj      CSFB: KW fix.
08/26/11   jj      CSFB: fixed CW.
08/26/11   jj      CSFB: Fixed issue with incorrect parsing of AWIM record.
08/26/11   jj      CSFB: Support for GCSNA negotiation.
08/26/11   jj      CSFB: Fix for incorrect parsing of AWIM record.
07/21/11   jj      CSFB: Fixed issue with incorrect parsing of AWIM record.
07/18/11   jj      CSFB: Added support for Registration Reject Order.
07/15/11   jj      CSFB: Fixed issue with MEID UHDM Parsing.
07/07/11   jj      CSFB: Fixed issue with wrongly setting msg type when MEID
                   UHDM is received.
07/11/11   jj      CSFB: KW fixes.
06/01/11   jj      CSFB: Bundled messages (UHDM+AWIM) support.
05/27/11   ab      KW Fixes
05/20/11   jj      CSFB: Externalized GCSNA variable, which will be used GCSNA
                   seq number calculation.
05/16/11   jj      CSFB: Added support to drop rxed msgs incase previous
                   access is in progress.
05/10/11   jj      CSFB: Fine tuned the value to wait for NAS's CNF for UL
                   message transmission.
05/05/11   jj      CSFB: Fix to correctly set GCSNA retransmission count.
05/03/11   jj      CSFB: Support for handling ECAM or Redir failure.
05/02/11   jj      CSFB: Added support for handling GCSNA ack for UL messages.
04/21/11   jj      CSFB: Added chanes to support SSD operation, Unique Challenge
                   in CSFB mode.
04/05/11   jj      CSFB: Update to report Reg failure to CM incase GCSNA
                   ack doesnt come on time.
03/31/11   jj      CSFB: Changes to support GCSNA seq number incrementing.
03/29/11   jj      CSFB: Changes to support GCSNA prev check.
03/17/11   jj      CSFB: Updates for ECAM AM=1 implementation.
03/03/11   jj      CSFB: Added support for Reselection with time transfer
                   and ECAM paging channel assignment.
12/22/10   ssh     Merged MSGR queue signal with CMD_Q_SIG. And reduced the
                   GCSNA txn buffer to be one.
02/11/11   jj      CSFB: Added feature check for MEID check.
01/12/11   jj      CSFB: Changes done to remove unused GCSNA sig.
01/12/11   jj      CSFB: Added changes to support sending of ORM in
                   ULHandoverPreparationTransfer.
12/15/10   jj      CSFB: Fix for parsing Reg Reqst Order.
12/15/10   jj      CSFB: Added support for Reg Reqst Order.
12/05/10   jj      CSFB: Merged to main/latest, did header file changes to
                   match CMI-P4.
12/01/10   ssh     CSFB: Added MT calls support.
12/03/10   ssh     CSFB: Dumping the UL byte stream
11/29/10   jj      CSFB: Lint cleaned.
11/28/10   jj      CSFB: Added changes to support jump to CDMA Traffic state.
11/16/10   jj      CSFB:Added more debugs
11/11/10   jj      CSFB redesign
11/04/10   jj      CSFB: Reverted processing of 1x crkt srv msg.
10/22/10   jj      CSSB: Initialized the Retransmission timer to 1 sec, for
                   messages not needing ack.
10/01/10   ssh     CSFB: added circuit service message processing.
09/24/10   ssh     CSFB: Added retry and retransmission support.
09/23/10   jj      CSFB: Added support for Updating Reg Context.
09/02/10   jj      CSFB: Update to logging function.
09/01/10   jj      CSFB: Update to support tunneled RGM.
08/25/10   jj      CSFB: Updated code to make call to 1X Params Parsing
                   and storing function.
08/19/10   jj      CSFB: Moved some of the data declarations to mccgcsna.c
08/18/10   jj      CSFB: Added state level implementation.
06/21/10   jj      Created module to have support functions for LTE-1X CSFB
                   operation.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "1x_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"
#include "modem_1x_defs.h"
#include "msg.h"
#include "m1x_diag.h"
#include "auth.h"
#include "err.h"
#include "rex.h"
#include "task.h"
#include "dog.h"
#include "queue.h"
#include "emm_irat_if_msg.h"

#include "mc_i.h"
#include "mccgcsna.h"
#include "mcccsfbsup.h"
#include "mcccsfb.h"
#include "mccreg_v.h"
#include "mccreg_i.h"

#include "caix.h"
#include "caix_i.h"

#include "mccap_v.h"
#include "mclog_v.h"

#include "m1x_time_i.h"
#include "amssassert.h"

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X


/*==========================================================================
                         GCSNA DATA & FUNCTIONS
===========================================================================*/

/* Buffer to store GCSNA PDU */
byte mcc_gcsna_pdu[1024];

/* Buffer to store TLAC+L3 PDU */
byte mcc_gcsna_tlac_pdu[512];

/* variable to store TLAC+L3 PDU Len */
word mcc_gcsna_tlac_pdu_len;

/* variable to track if ack is received for a transmitted message */
boolean mcc_gcsna_reg_ack_found = FALSE;

/* Variable to keep track of uplink transaction ids */
LOCAL dword mcc_gcsna_ul_transaction_id = 0;

/* Internal format of GCSNA messages which are transmitted on Tunnel */
gcsna_ul_msg_type mcc_gcsna_int_msg;

#ifdef FEATURE_1X_CP_MEM_OPT
/* Pointer holds downlink GCSNA messages format which are rxed on Tunnel */
gcsna_dl_msg_type *mcc_gcsna_ext_msg_ptr = NULL;
#else /* !FEATURE_1X_CP_MEM_OPT */
/* Downlink GCSNA messages format which are rxed on Tunnel */
gcsna_dl_msg_type mcc_gcsna_ext_msg;
/* Pointer holds downlink GCSNA messages format which are rxed on Tunnel */
gcsna_dl_msg_type *mcc_gcsna_ext_msg_ptr = &mcc_gcsna_ext_msg;
#endif /* FEATURE_1X_CP_MEM_OPT */


/* Variable to track GCSNA message seq for 1X CRKT Service Message */
byte gcsna_msg_seq = 0;

/* Variable to track GCSNA Option. This will be changed if GCSNA negotiation
 * suggests a change. To start with, we will support GCSNA option 1.
 */
byte gcsna_option_inuse = GCSNA_OPTION_1;

/** Macro to unpack a byte and advance the page position
 */
#define UNPACKB_PG(dl_msg,type, field)                                        \
int_ptr->dl_msg.field = b_unpackb(buf_ptr, ext_msg_pos, FSIZ(type, field));   \
ext_msg_pos += FSIZ(type, field);                                             \
M1X_MSG( DCP, LEGACY_MED,                                                    \
  "DEBUG: DL CRKT SRV msg: "#field" = %d",                                     \
  int_ptr->dl_msg.field)

/** Macro to unpack a word and advance the page position
 */
#define UNPACKW_PG(dl_msg,type, field)                                        \
int_ptr->dl_msg.field = b_unpackw(buf_ptr, ext_msg_pos, FSIZ(type, field));   \
ext_msg_pos += FSIZ(type, field);                                             \
M1X_MSG( DCP, LEGACY_MED,                                                    \
  "DEBUG: DL CRKT SRV msg: "#field" = %d",                                     \
  int_ptr->dl_msg.field)

/* Macro to unpack a byte and advance the
   page position */
#define UNPACKB_G_PG(ext, type, field)                                        \
  int_ptr->ext = b_unpackb(buf_ptr, ext_msg_pos, FSIZ(type, field));          \
 ext_msg_pos += FSIZ(type, field)

/* Macro to unpack a word and advance the
   page position */
#define UNPACKW_G_PG(ext, type, field)                                        \
  int_ptr->ext = b_unpackw(buf_ptr, ext_msg_pos, FSIZ(type, field));          \
  ext_msg_pos += FSIZ(type, field)

#define UNPACKB_PG_BUNDLED(ext, type, field_type,field)                        \
  int_ptr->ext.field = b_unpackb(buf_ptr, ext_msg_pos, FSIZ(type, field_type));\
  ext_msg_pos += FSIZ(type, field_type)


#define UNPACKW_PG_BUNDLED(ext, type, field_type,field)                        \
  int_ptr->ext.field = b_unpackw(buf_ptr, ext_msg_pos, FSIZ(type, field_type));\
  ext_msg_pos += FSIZ(type, field_type)

/* Macro to compute the offset, in bytes, of the end of a specified field
   of a specified structure type */
#define FENDPOS( type, field )                                                \
  (FPOS( type, field ) + FSIZ( type, field ))

/* Structure used to store the sent msgs. This is required for ack_seq
   matching */
gcsna_msg_sent_type gcsna_msg_sent;

/* To Unpack PD from msg_type */
#define UNPACK_PD(dest, msg_type)                                             \
  dest = ( (msg_type & 0xC0) >> 6 ) & 0x03;

/* Maximum number of msg_seq, IWS_ID pairs stored for Duplicate Detection. */
#define MAX_CKT_SRV_MSGS_STORED_FOR_DUP_DETECTION 64

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                      Function Forward Declarations                   */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

void mcc_csfb_log_1x_ul_msg(byte* ul_gcsna_msg, word gcsna_pdu_len);

boolean mcc_gcsna_do_gcsna_option_chk(byte msg_type);

/* Stored list of msg_seq, IWS_ID pairs */

typedef struct{
  byte  iws_id_incl;
  word  iws_id;
  byte  msg_seq;
  qword rxed_timestamp;
}mcc_gcsna_rxed_msg_entry_type;

typedef struct {
  byte num_entries;
  mcc_gcsna_rxed_msg_entry_type entry[MAX_CKT_SRV_MSGS_STORED_FOR_DUP_DETECTION];
} mcc_gcsna_msg_list_type;

mcc_gcsna_msg_list_type mcc_gcsna_msg_list;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                            FUNCTIONS                                */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*===========================================================================

FUNCTION MCC_GCSNA_GET_TRANSACTION_ID

DESCRIPTION
  TBD

 DEPENDENCIES
  None.

RETURN VALUE
  TO BE FILLED

SIDE EFFECTS
  None.
===========================================================================*/
void mcc_gcsna_get_transaction_id(dword* transaction_id)
{
  /* Populate the TRANSACTION_ID */
  *(transaction_id) = ((mcc_gcsna_ul_transaction_id & 0xFFFF) + 1)%64;
  /* Populate the TECHNOLOGY */
  *(transaction_id) |= (((unsigned int)NAS_1XCP << 24));
  /* Populate the MODULE_ID */
  *(transaction_id) |= ((unsigned int)0 << 16);

  mcc_gcsna_ul_transaction_id = *(transaction_id);
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG:GCSNA trans_id for UL msg = %d",
    mcc_gcsna_ul_transaction_id);
}

/*===========================================================================

FUNCTION MCC_GCSNA_SEND_UL_MSG

DESCRIPTION
The mobile station sends a 1X Param Request message to request for 1X Params.

DEPENDENCIES
  None.

RETURN VALUE
  TBD

SIDE EFFECTS
  None.
==========================================================================*/
boolean mcc_gcsna_send_ul_msg(mcc_gcsna_msg_type msg_id,
                              word gcsna_pdu_len,
                              gcsna_ul_tx_type ul_txssion_type)
{
  emm_irat_ul_msg_req_type ul_msg_req;
  boolean ack_req = FALSE;
  byte num_gcsna_retxns = 0; /* Count of retransmissions */
  size_t copy_size;
  /* Return value of memscpy function */


  memset(&ul_msg_req, 0, sizeof(ul_msg_req));

  /* Check if the calculated GCSNA message len is greater than the PDU
   * which needs to be transmitted.
   */
  if( gcsna_pdu_len > sizeof(mcc_gcsna_pdu) )
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "DEBUG: Wrong GCSNA PDU len:%d, returning",
      gcsna_pdu_len);
    return FALSE;
  }

  /* Send event for UL message */
  if(msg_id == GCSNA_1X_CRKT_SRV_MSG)
  {
    mclog_report_event_gcsna_ul_msg(msg_id,
                                    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.gen_ac.msg_type);
  }
  else
  {
    mclog_report_event_gcsna_ul_msg(msg_id,
                                    0xFF);
  }

/* Get the original Transaction ID */
  mcc_gcsna_get_transaction_id(&(ul_msg_req.trans_id));

  /* Check if the empty space is available to store the msg we need to send */
  if(gcsna_msg_sent.msg_in_use == TRUE)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Buffer not avail for storing the transmitted msg. ");
    return FALSE;
  }

  switch(msg_id)
  {
    case GCSNA_1X_PARAM_REQ:
    {
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: Sending GCSNA_1X_PARAM_REQ");
      ul_msg_req.msg.ul_msg_type = LTE_IRAT_TUNNEL_UL_MSG_CSFB_PARAMS_REQ;
      break;
    } /* GCSNA_1X_PARAM_REQ */
    case GCSNA_1X_CRKT_SRV_MSG:
    case GCSNA_L2_ACK:
    case GCSNA_SRV_REJ_MSG:
    {
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: Sending UL msg =%d",
        msg_id);
      /* Fill the UL message type to carry this message as per the tunneled
       * message contained in it.
       */
      if(ul_txssion_type == GCSNA_USE_UL_INFO_TX_MSG_TYPE)
      {
        /* Fill the  lte_irat_tunnel_ul_msg_ul_info_tx_req_s  ul_info_tx
   * part.
   */
        ul_msg_req.msg.ul_msg_type = LTE_IRAT_TUNNEL_UL_MSG_INFO_TRANSFER_REQ;
        ul_msg_req.msg.ul_msg_u.ul_info_tx.type = LTE_IRAT_RAT_TYPE_1XRTT;
        ul_msg_req.msg.ul_msg_u.ul_info_tx.msg_container.msg_size = gcsna_pdu_len;
        copy_size = memscpy(ul_msg_req.msg.ul_msg_u.ul_info_tx.msg_container.msg_payload,
                            LTE_IRAT_TUNNEL_MAX_MSG_CONTAINER_SIZE*sizeof(uint8),
                            mcc_gcsna_pdu,gcsna_pdu_len
                           );
        if( copy_size != gcsna_pdu_len )
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
            copy_size,
            gcsna_pdu_len,
            LTE_IRAT_TUNNEL_MAX_MSG_CONTAINER_SIZE*sizeof(uint8));
        }
        if(msg_id == GCSNA_1X_CRKT_SRV_MSG)
        {
          /* This is for transmitting UL message */

          /* Get the msg_seq*/
          gcsna_msg_sent.msg_seq = b_unpackb(ul_msg_req.msg.ul_msg_u.ul_info_tx.msg_container.msg_payload,
                                             FPOS(gcsna_pdu, msg_seq), FSIZ(gcsna_pdu, msg_seq));

          /* Get the ack_req boolean. */
          ack_req = b_unpackb(ul_msg_req.msg.ul_msg_u.ul_info_tx.msg_container.msg_payload,
                              FPOS(gcsna_pdu, ack_reqd), FSIZ(gcsna_pdu, ack_reqd));
        }
        else if(msg_id == GCSNA_L2_ACK)
        {
          /* This is for txtting ack for DL message, hence set ack to FALSE */
          ack_req = FALSE;
          gcsna_msg_sent.msg_seq = b_unpackb(ul_msg_req.msg.ul_msg_u.ul_info_tx.msg_container.msg_payload,
                                             FPOS(gcsna_ack_pdu_type, ack_seq),
                                             FSIZ(gcsna_ack_pdu_type, ack_seq));

        }
        else if(msg_id == GCSNA_SRV_REJ_MSG)
        {
          /* This is for txtting ack for DL message, hence set ack to FALSE */
          ack_req = FALSE;
        }
      } /* ul_txssion_msg == GCSNA_USE_UL_INFO_MSG_TYPE */

      else if(ul_txssion_type == GCSNA_USE_UL_HO_PREP_MSG_TYPE)
      {
        /* Fill the lte_irat_tunnel_ul_msg_ul_ho_prep_tx_req_s ul_ho_prep_tx_req
          * part.
          */
        ul_msg_req.msg.ul_msg_type = LTE_IRAT_TUNNEL_UL_MSG_UL_HO_PREP_TX_REQ;


        if(mccap_is_meid_supported() == TRUE)
        {
          /* Fill the MEID related fields */
          ul_msg_req.msg.ul_msg_u.ul_ho_prep_tx_req.is_meid_present = TRUE;
          ul_msg_req.msg.ul_msg_u.ul_ho_prep_tx_req.meid.numbits = LTE_IRAT_MEID_MAX_SIZE*8;
          /* MEID is 56 bits = 7 bytes in length */
          copy_size = memscpy
                        (ul_msg_req.msg.ul_msg_u.ul_ho_prep_tx_req.meid.data,
                         LTE_IRAT_MEID_MAX_SIZE*sizeof(uint8),
                 cdma.meid,
                         LTE_IRAT_MEID_MAX_SIZE
                         );

          if( copy_size != LTE_IRAT_MEID_MAX_SIZE )
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
              copy_size,
              LTE_IRAT_MEID_MAX_SIZE,
              LTE_IRAT_MEID_MAX_SIZE*sizeof(uint8));
          }
        }
        else

        {
          ul_msg_req.msg.ul_msg_u.ul_ho_prep_tx_req.meid.numbits = 0;
          ul_msg_req.msg.ul_msg_u.ul_ho_prep_tx_req.is_meid_present = FALSE;
        }
        ul_msg_req.msg.ul_msg_u.ul_ho_prep_tx_req.type = LTE_IRAT_RAT_TYPE_1XRTT;
        ul_msg_req.msg.ul_msg_u.ul_ho_prep_tx_req.msg_container.msg_size = gcsna_pdu_len;
        copy_size = memscpy
                      (ul_msg_req.msg.ul_msg_u.ul_ho_prep_tx_req.msg_container.msg_payload,
                       LTE_IRAT_TUNNEL_MAX_MSG_CONTAINER_SIZE*sizeof(uint8),
               mcc_gcsna_pdu,
                       gcsna_pdu_len
                      );
        if( copy_size != gcsna_pdu_len )
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
            copy_size,
            gcsna_pdu_len,
            LTE_IRAT_TUNNEL_MAX_MSG_CONTAINER_SIZE*sizeof(uint8));
        }
        /* Get the msg_seq*/
        gcsna_msg_sent.msg_seq = b_unpackb(ul_msg_req.msg.ul_msg_u.ul_ho_prep_tx_req.msg_container.msg_payload,
                                              FPOS(gcsna_pdu, msg_seq), FSIZ(gcsna_pdu, msg_seq));

        /* Get the ack_req boolean. */
        ack_req = b_unpackb(ul_msg_req.msg.ul_msg_u.ul_ho_prep_tx_req.msg_container.msg_payload,
                            FPOS(gcsna_pdu, ack_reqd), FSIZ(gcsna_pdu, ack_reqd));
      } /* ul_txssion_type == GCSNA_USE_UL_HO_PREP_MSG_TYPE */
      else
      {
        ERR_FATAL("Unknow UL TXSSION MSG",0,0,0);
      }

      /* CSFB LOGGING */
      //mcc_csfb_log_1x_ul_msg(mcc_gcsna_pdu, gcsna_pdu_len);
      break;
    } /* GCSNA_1X_CRKT_SRV_MSG */

    default:
      break;
  } /* switch(msg_id) */

  /* Store the message till the ack is received. */
  gcsna_msg_sent.msg_tx = ul_msg_req;

  /* Initialize all the gcsna_msg_sent fields specific to this msg. */
  gcsna_msg_sent.msg_in_use = TRUE;
  gcsna_msg_sent.ack_req = ack_req;

  /* Fetch the number of times the transmission of this msg is required
     as part of ARQ mech. */
  if(ack_req == FALSE)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "DEBUG: Filling retry/retx cnt for msg not needing ack");
    gcsna_msg_sent.num_remaining_txns = 1;
    /* retxns_timer is set to 2 sec - it can be fine tuned based on NAS delays */
    /* As per RRC's last confirmation, 2000ms is maximum time to setup
     * connection. Hence fine tuned the value to 2000ms
     */
    gcsna_msg_sent.retxns_timer_value = 2000;
    /* We are transmitting message which doesnt need ack,set corresponding flag */
    mcc_csfb_is_dl_gcsna_ack_pending = FALSE;
  }
  else
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "DEBUG: Filling retry/retx cnt for msg needing ack");
    if(c2k_parms_msg.gcsna_l2_ack_timer_inc &&
       c2k_parms_msg.gcsna_seq_context_timer_inc)
    {
      num_gcsna_retxns =
        (c2k_parms_msg.gcsna_seq_context_timer*100)/(c2k_parms_msg.gcsna_l2_ack_timer*80);

      /* The number of retransmissions is one more than the number of
       * retransmissions . Retransmissions should start after the first transmission
       * e.g. if retransmission count  is derived as 1, it means from UE side
       * the total transmissions would be 2, one for first transmission and one for
       * retransmission.
       */
      gcsna_msg_sent.num_remaining_txns = (num_gcsna_retxns+1);

      /* retxns_timer is the time to wait for an ack in ms.*/
      gcsna_msg_sent.retxns_timer_value = c2k_parms_msg.gcsna_l2_ack_timer * 80;
    }
    else
    {
      gcsna_msg_sent.num_remaining_txns = 1;
      /* retxns_timer is set to 2 sec - it can be fine tuned based on NAS delays */
      /* As per RRC's last confirmation, 2000ms is maximum time to setup
       * connection. Hence fine tuned the value to 2000ms
       */
      gcsna_msg_sent.retxns_timer_value = 2000;
    }
    /* We are transmitting message which needs ack,set corresponding flag */
    mcc_csfb_is_dl_gcsna_ack_pending = TRUE;
  }

  /* Initialize the retry related parameters for the retransmission. */
  gcsna_msg_sent.num_remaining_retries = MAX_RETRY_COUNT;

  /* This is the first txn of the message. */
  gcsna_msg_sent.is_this_first_txn = TRUE;

  /* Initialize is_msg_tx_rsp_rxed = TRUE to make sure gcsna msg sent to NAS
   * for the first retry attempt
   */
  gcsna_msg_sent.is_msg_tx_rsp_rxed = TRUE;

  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: --- printing GCSNA txssion element START ---");

  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: msg_in_use = %d, ack_req = %d, msg_seq = %d",
    gcsna_msg_sent.msg_in_use, 
    gcsna_msg_sent.ack_req, 
    gcsna_msg_sent.msg_seq);

  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: num_remaining_txns = %d, num_remaining_retries = %d",
    gcsna_msg_sent.num_remaining_txns,  
    gcsna_msg_sent.num_remaining_retries);

  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG:retxns_timer_value:%d, is_this_first_txn:%d, is_msg_tx_rsp_rxed:%d",
    gcsna_msg_sent.retxns_timer_value, 
    gcsna_msg_sent.is_this_first_txn,
    gcsna_msg_sent.is_msg_tx_rsp_rxed);

  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: --- printing GCSNA txssion element END ---");

  /* Call the retransmission mech. */
  mcc_gcsna_transmit();
  return TRUE;
} /* mcc_gcsna_send_ul_msg */

/*===========================================================================

FUNCTION MCC_GCSNA_TRANSMIT

DESCRIPTION
  This function transmits/retransmits the GCSNA msg with passed index.
  Retransmissions are attempted if the L2 ack timer has expired and
  the L2 ack is not yet received after sending the MSG to NW

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_gcsna_transmit(void)
{

  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG:Message with num_remaining_txns = %d",
    gcsna_msg_sent.num_remaining_txns);

  /* Attempt the retry now if all the retxn attempts are not yet exhausted */
  if(gcsna_msg_sent.num_remaining_txns > 0)
  {
    M1X_MSG( DCP, LEGACY_MED,
      "DEBUG:Attempting Retry for a Retransmission attempt");
    mcc_gcsna_retry();
    gcsna_msg_sent.num_remaining_txns--;
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "DEBUG: Retransmission TMO/ Retransmission Exhausted");
    /* Check if this is for REG Attempt, if yes, check for SOFT/HARD failures
     * and report failure to CM
     */
    if( (cdma.curr_state == CDMA_CSFB_ACCESS) &&
        (mcc_csfb_access_reason == MCC_CSFB_ACCESS_REG)
      )
    {

      /* Increment the SOFT failure cnt */
      csfb_cdma.reg_retry_cnt++;
      M1X_MSG( DCP, LEGACY_MED,
        "Retransmittions are exhausted, incremented the reg_retry_cnt to %d",
        csfb_cdma.reg_retry_cnt);

      /* check if we have hit soft failures, if we still have chance, continue
       *  another round of transmission.
       */
      if( mcc_csfb_is_hard_failure(MCC_CSFB_MAX_GCSNA_ATTEMPT, NULL) )
      {
      /* We failed to receive GCSNA ack for the REG attempt, report failure */
      cm_mc_rpt_type *cm_ptr;
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG: Failed to receive GCSNA ack, returning..");

      cm_ptr = cm_mc_rpt_get_buf_else_err_fatal();
      cm_ptr->cdma_csfb_status.hdr.cmd = CM_1XCSFB_STATUS_F;
      cm_ptr->cdma_csfb_status.lte_1xcsfb_result = CM_1XCSFB_PREREG_FAIL;
      mcc_cm_mc_rpt( cm_ptr );
      /* Mark so that no registration check is performed any further */
      csfb_cdma.reg_status = MCC_CSFB_REG_STOP;
    }
      else
      {
        /* Initialize retransmission count */
        if(c2k_parms_msg.gcsna_l2_ack_timer_inc &&
           c2k_parms_msg.gcsna_seq_context_timer_inc &&
           gcsna_msg_sent.ack_req)
        {
          gcsna_msg_sent.num_remaining_txns =
            ( 1+ ( (c2k_parms_msg.gcsna_seq_context_timer*100)/(c2k_parms_msg.gcsna_l2_ack_timer*80) ) );

          /* Initialize retransmission timer */
          gcsna_msg_sent.retxns_timer_value = c2k_parms_msg.gcsna_l2_ack_timer * 80;
        }
        else
        {
          gcsna_msg_sent.num_remaining_txns = 1;
          gcsna_msg_sent.retxns_timer_value = 2000;
        }

        /* Initialize retry count */
        gcsna_msg_sent.num_remaining_retries = MAX_RETRY_COUNT;

        /* This is the first txn of the message. */
        gcsna_msg_sent.is_this_first_txn = TRUE;

        /* Initialize is_msg_tx_rsp_rxed = TRUE to make sure gcsna msg sent to NAS
         * for the first retry attempt
         */
        gcsna_msg_sent.is_msg_tx_rsp_rxed = TRUE;

        M1X_MSG( DCP, LEGACY_HIGH,
          "DEBUG:first_txn:%d,msg_tx_rsp_rxed:%d,num_remaining_retries:%d",
          gcsna_msg_sent.is_this_first_txn,
          gcsna_msg_sent.is_msg_tx_rsp_rxed,
          gcsna_msg_sent.num_remaining_retries);

        M1X_MSG( DCP, LEGACY_MED,
          "DEBUG:Attempting Retry for a Retransmission attempt");

        mcc_gcsna_retry();
        gcsna_msg_sent.num_remaining_txns--;
        return;
      }
    }
    else if( (mcc_csfb_access_reason == MCC_CSFB_ACCESS_ORD_RES) &&
             (csfb_cdma.sa_resp_ord.ordq.order == CAI_RELEASE_ORD)
            )
    {
      /* Inform CM of the call end  */
      cm_mc_rpt_type *cm_ptr;
      cm_ptr = cm_mc_rpt_get_buf_else_err_fatal();
      cm_ptr->csfb_call_end_cnf.hdr.cmd = CM_1XCSFB_CALL_END_CNF_F;
      cm_ptr->csfb_call_end_cnf.call_id = cdma.call_id;
      /* Send Call End Conf to CM. */
      mcc_cm_mc_rpt( cm_ptr );
    }
#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
    else if(mcc_csfb_access_reason == MCC_CSFB_ACCESS_MSG)
    {
      /* Update MMODE about the SMS abort. */
      M1X_MSG( DCP, LEGACY_MED,
        "SMS transmission failure");
      mcc_check_mo_dbm_status(CM_DBM_MO_L2_ACK_FAILURE);

      /* Exit from the current state and eventually to Idle State */
      mcc_csfb_gcsna_stop_txn();
      mcc_csfb_exit.exit = MCC_CSFB_EXIT_ONEX_INTRNL_FAILURE;
      mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_GCSNA_MSG_TX_TIMEOUT;
      cdma.curr_state = CDMA_CSFB_EXIT;
      return;
    }
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */

    /* Exit from the current state to Idle State, stopping of all transmissions */
    mcc_csfb_gcsna_stop_txn();
    mcc_csfb_exit.exit = MCC_CSFB_EXIT_ONEX_INTRNL_FAILURE;
    mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_GCSNA_ACK_FAIL;
    cdma.curr_state = CDMA_CSFB_EXIT;
  }
} /* mcc_gcsna_transmit */
/*===========================================================================

FUNCTION MCC_GCSNA_RETRY

DESCRIPTION
  This function retries the txn of the GCSNA msg with passed index.
  Retry is attempted when the msg was not sent by NAS to NW.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_gcsna_retry(void)
{
  errno_enum_type  estatus;

  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG:Retrying the msg, num_remaining_retries =%d",
    gcsna_msg_sent.num_remaining_retries);

  if(gcsna_msg_sent.num_remaining_retries > 0)
  {
    if( gcsna_msg_sent.is_msg_tx_rsp_rxed )
    {
      /* Initialize the message header */
      msgr_init_hdr(&(gcsna_msg_sent.msg_tx.msg_hdr), MSGR_ONEX_CP, NAS_EMM_IRAT_UL_MSG_REQ);

      estatus = msgr_send( &(gcsna_msg_sent.msg_tx.msg_hdr),sizeof(emm_irat_ul_msg_req_type) );
      ASSERT( estatus == E_SUCCESS );
      gcsna_msg_sent.is_msg_tx_rsp_rxed = FALSE;
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG: sent msg to NAS");
    }
    else
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG: skip sending msg to NAS: is_this_first_txn = %d, is_msg_tx_rsp_rxed:%d",
        gcsna_msg_sent.is_this_first_txn,
        gcsna_msg_sent.is_msg_tx_rsp_rxed);
    }

    if(gcsna_msg_sent.is_this_first_txn == TRUE)
    {
      (void)rex_set_timer(&gcsna_msg_sent.txn_timer,
                          MAX_RETRY_BACKOFF_TIMER_VALUE);
      gcsna_msg_sent.timer_type = MCC_GCSNA_RETRY_BACKOFF_TIMER;

      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG:Starting Retry Backoff timer = %d ms",
        MAX_RETRY_BACKOFF_TIMER_VALUE);
    }
    else
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG:Starting Retxn timer = %d ms",
        gcsna_msg_sent.retxns_timer_value);

      (void)rex_set_timer(&gcsna_msg_sent.txn_timer,
                          gcsna_msg_sent.retxns_timer_value);
      gcsna_msg_sent.timer_type = MCC_GCSNA_RETXN_TIMER;
    }

    gcsna_msg_sent.num_remaining_retries--;
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "NAS couldn't transmit the msg, num_remaining_txns: %d",
      gcsna_msg_sent.num_remaining_txns);

    if(gcsna_msg_sent.num_remaining_txns > 0)
    {
      /* Increment the SOFT failure cnt */
      csfb_cdma.reg_retry_cnt++;
      M1X_MSG( DCP, LEGACY_MED,
        "mcc_gcsna_retry is called with 0 num_remaining_retries, reg_retry_cnt = %d",
        csfb_cdma.reg_retry_cnt);
      mcc_csfb_gcsna_stop_txn();
    }/* if(gcsna_msg_sent.num_remaining_txns > 0) */
    else
    {
      /* Call the retransmission mechanism. We reach here when max retry count
       * and max transmission count have been exhaused. In this case, we shall
       * call mcc_gcsna_transmit() to make sure that we are properly handling
       * the error scenarios. */
      mcc_gcsna_transmit();
    }
  }

  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Done with retry attempt");
}/* mcc_gcsna_retry() */

/*===========================================================================

FUNCTION MCC_CSFB_GCSNA_STOP_TXN

DESCRIPTION
  This function aborts the transmission of the GCSNA msg with passed index.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_csfb_gcsna_stop_txn(void)
{
  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: Stopping GCSNA transmissions");

  /* Clean all the retry and retransmission related parameters. */
  gcsna_msg_sent.msg_in_use = FALSE;
  gcsna_msg_sent.num_remaining_retries = 0;
  gcsna_msg_sent.num_remaining_txns = 0;
  gcsna_msg_sent.timer_type = MCC_GCSNA_NO_TIMER;
  gcsna_msg_sent.retxns_timer_value = 0;
  gcsna_msg_sent.is_msg_tx_rsp_rxed = FALSE;

  /* Clear the timer */
  (void) rex_clr_timer( &gcsna_msg_sent.txn_timer );

  /* Reset the variable used to trace DL GCSNA ack receipt */
  mcc_csfb_is_dl_gcsna_ack_pending = FALSE;

  /* Clear the signal as well*/
  (void) rex_clr_sigs( MC_TCB_PTR, MCC_CSFB_TXN_TIMER_SIG);

  /* Set the Soft Failure Count to Zero */
  csfb_cdma.reg_retry_cnt = 0;
} /* mcc_csfb_gcsna_stop_txn */

/*===========================================================================

FUNCTION mcc_gcsna_build_gcsna_pdu

DESCRIPTION
The mobile station sends a 1X Param Request message to request for 1X Params.

DEPENDENCIES
  None.

RETURN VALUE
  TBD

SIDE EFFECTS
  None.
==========================================================================*/
word mcc_gcsna_build_gcsna_pdu(mcc_gcsna_msg_type msg_type,boolean ack_reqd,boolean stop_dup_detect)
{
  /* Build GCSNA message */
  size_t copy_size;
  /* Return value of memscpy function */
  /* Position to pack bits in message */
  word buffer_pos = 0;

  if(msg_type == GCSNA_1X_CRKT_SRV_MSG)
  {

    b_packb( msg_type, mcc_gcsna_pdu, buffer_pos,FSIZ( gcsna_pdu, msg_id ));
    buffer_pos += FSIZ(  gcsna_pdu, msg_id );

    b_packb(gcsna_option_inuse, mcc_gcsna_pdu, buffer_pos,FSIZ( gcsna_pdu, gcsna_opt ));
    buffer_pos += FSIZ( gcsna_pdu, gcsna_opt );

    /* As per GCSNA spec, AlternativeGCSNAOption_INCL shall be set to 0 by MS */
    b_packb(0 , mcc_gcsna_pdu, buffer_pos,FSIZ( gcsna_pdu, alt_gcsna_opt_incl ));
    buffer_pos += FSIZ( gcsna_pdu, alt_gcsna_opt_incl );

    b_packb(0 , mcc_gcsna_pdu, buffer_pos,FSIZ( gcsna_pdu, iws_id_incl ));
    buffer_pos += FSIZ( gcsna_pdu, iws_id_incl  );

    b_packb(ack_reqd , mcc_gcsna_pdu, buffer_pos,FSIZ( gcsna_pdu, ack_reqd ));
    buffer_pos += FSIZ( gcsna_pdu, ack_reqd  );

    b_packb(stop_dup_detect , mcc_gcsna_pdu, buffer_pos,FSIZ( gcsna_pdu, stop_dup_detect ));
    buffer_pos += FSIZ( gcsna_pdu, stop_dup_detect  );

    b_packb(gcsna_msg_seq , mcc_gcsna_pdu, buffer_pos,FSIZ( gcsna_pdu, msg_seq ));
    buffer_pos += FSIZ( gcsna_pdu, msg_seq  );

    b_packb(0 , mcc_gcsna_pdu, buffer_pos,FSIZ( gcsna_pdu, num_bundled_tlac_l3_rec ));
    buffer_pos += FSIZ( gcsna_pdu, num_bundled_tlac_l3_rec  );

    b_packb(0 , mcc_gcsna_pdu, buffer_pos,FSIZ( gcsna_pdu, gcsna_reserved ));
    buffer_pos += FSIZ(gcsna_pdu, gcsna_reserved);

    if( mcc_gcsna_tlac_pdu_len <= sizeof(mcc_gcsna_tlac_pdu) )
    {
    copy_size = memscpy((mcc_gcsna_pdu+4),(sizeof(mcc_gcsna_pdu)-4),
                         mcc_gcsna_tlac_pdu, mcc_gcsna_tlac_pdu_len );

    if( copy_size != mcc_gcsna_tlac_pdu_len )
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
        copy_size,
        mcc_gcsna_tlac_pdu_len,
        (sizeof(mcc_gcsna_pdu)-4));
    }
      if(mcc_csfb_access_reason == MCC_CSFB_ACCESS_REG)
      {
        mcc_gcsna_last_reg_msg_seq = gcsna_msg_seq;
      }
    else if( (mcc_csfb_access_reason == MCC_CSFB_ACCESS_ORD_RES)
#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
             || (mcc_csfb_access_reason == MCC_CSFB_ACCESS_MSG)
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */
           )
      {
        mcc_gcsna_last_ul_msg_seq = gcsna_msg_seq;
      }
      /* increment the gcsna msg seq number */
      gcsna_msg_seq = (gcsna_msg_seq +1) % 64;

      /* Reset using_rand_msg_seq */
      csfb_cdma.using_rand_msg_seq = FALSE;

      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: Done packing GCSNA header");
      return ((buffer_pos)/8 + mcc_gcsna_tlac_pdu_len);
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "DEBUG: Wrong GCSNA TLAC PDU len:%d, returning 0",
        mcc_gcsna_tlac_pdu_len);
      return 0;
    }

  } /* if(msg_type == GCSNA_1X_CRKT_SRV_MSG) */

  else if(msg_type == GCSNA_L2_ACK)
  {
    b_packb( msg_type, mcc_gcsna_pdu, buffer_pos,FSIZ( gcsna_ack_pdu_type, msg_id ));
    buffer_pos += FSIZ(  gcsna_ack_pdu_type, msg_id );

    b_packb(mcc_gcsna_int_msg.gcsna_ack.ack_seq,
            mcc_gcsna_pdu,
            buffer_pos,
            FSIZ( gcsna_ack_pdu_type, ack_seq ));
    buffer_pos += FSIZ(gcsna_ack_pdu_type, ack_seq);

    b_packb(0, mcc_gcsna_pdu, buffer_pos,FSIZ(gcsna_ack_pdu_type, gcsna_reserved));
    buffer_pos += FSIZ(gcsna_ack_pdu_type, gcsna_reserved);

    M1X_MSG( DCP, LEGACY_MED,
      "DEBUG: Done packing GCSNA header");
    return ((buffer_pos)/8);
  } /* if(msg_type == GCSNA_L2_ACK) */

  else if(msg_type == GCSNA_SRV_REJ_MSG)
  {
    b_packb(msg_type, mcc_gcsna_pdu, buffer_pos,FSIZ(gcsna_rej_pdu_type, msg_id));
    buffer_pos += FSIZ(  gcsna_rej_pdu_type, msg_id);

    b_packb(mcc_gcsna_int_msg.gcsna_rej.rej_seq, mcc_gcsna_pdu, buffer_pos,FSIZ(gcsna_rej_pdu_type, rej_seq));
    buffer_pos += FSIZ(gcsna_rej_pdu_type, rej_seq);

    b_packb(mcc_gcsna_int_msg.gcsna_rej.cause, mcc_gcsna_pdu, buffer_pos,FSIZ(gcsna_rej_pdu_type, cause));
    buffer_pos += FSIZ(gcsna_rej_pdu_type, cause);

    if( (mcc_gcsna_int_msg.gcsna_rej.cause == GCSNA_OPT_MISMATCH) ||
        (mcc_gcsna_int_msg.gcsna_rej.cause == GCSNA_OPT_PREV_MISMATCH)
      )
    {
      b_packb(mcc_gcsna_int_msg.gcsna_rej.num_fields, mcc_gcsna_pdu, buffer_pos,FSIZ(gcsna_rej_pdu_type, cause));
      buffer_pos += FSIZ(gcsna_rej_pdu_type, num_fields);

      b_packb(mcc_gcsna_int_msg.gcsna_rej.gcsna_opt, mcc_gcsna_pdu, buffer_pos,FSIZ(gcsna_rej_pdu_type, gcsna_opt));
      buffer_pos += FSIZ(gcsna_rej_pdu_type, gcsna_opt);
    }

    if( (mcc_gcsna_int_msg.gcsna_rej.cause == GCSNA_PREV_MISMATCH )||
        (mcc_gcsna_int_msg.gcsna_rej.cause == GCSNA_OPT_PREV_MISMATCH)
      )
    {
      b_packb(mcc_gcsna_int_msg.gcsna_rej.prev_1x, mcc_gcsna_pdu, buffer_pos,FSIZ(gcsna_rej_pdu_type, prev_1x));
      buffer_pos += FSIZ(gcsna_rej_pdu_type, prev_1x);
    }

    if(mcc_gcsna_int_msg.gcsna_rej.cause == GCSNA_INV_MSG_ID)
    {
      b_packb(mcc_gcsna_int_msg.gcsna_rej.msg_id_inv, mcc_gcsna_pdu, buffer_pos,FSIZ(gcsna_rej_pdu_type, msg_id_inv));
      buffer_pos += FSIZ(gcsna_rej_pdu_type, msg_id_inv);
    }

    b_packb(0, mcc_gcsna_pdu, buffer_pos,FSIZ(gcsna_rej_pdu_type, gcsna_reserved));
    buffer_pos += FSIZ(gcsna_rej_pdu_type, gcsna_reserved);

    return ((buffer_pos)/8);

  } /* if(msg_type == GCSNA_SRV_REJ_MSG) */

  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: Invalid GCSNA msg");
  return 0;

} /* mcc_gcsna_build_gcsna_pdu */

/*===========================================================================

FUNCTION MCC_GCSNA_SEND_1X_PARAM_RQST

DESCRIPTION
The mobile station sends a 1X Param Request message to request for 1X Params.

DEPENDENCIES
  None.

RETURN VALUE
  TBD

SIDE EFFECTS
  None.
==========================================================================*/
boolean mcc_gcsna_send_1x_param_rqst(void)
{
  /* send the message over MSGR to LTE_NAS */
  return( mcc_gcsna_send_ul_msg(GCSNA_1X_PARAM_REQ,0,GCSNA_USE_NONE_TYPE) );
}

/*===========================================================================

FUNCTION mcc_gcsna_send_1x_crkt_srv_msg

DESCRIPTION
The mobile station sends a 1X Param Request message to request for 1X Params.

DEPENDENCIES
  None.

RETURN VALUE
  TBD

SIDE EFFECTS
  None.
==========================================================================*/
boolean mcc_gcsna_send_1x_crkt_srv_msg(boolean ack_required,
                                       gcsna_ul_tx_type ul_txssion_type)
{
  word gcsna_pdu_len;
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG:TXing 1x crkt Srv msg");

  /* Build GCSNA message */
  gcsna_pdu_len = mcc_gcsna_build_gcsna_pdu( GCSNA_1X_CRKT_SRV_MSG,ack_required,
    ((gcsna_msg_seq == 0) || (csfb_cdma.using_rand_msg_seq == TRUE)) );

  /* Check if the calculated GCSNA message len is greater than the PDU
   * which needs to be transmitted.
   */
  if( (gcsna_pdu_len > sizeof(mcc_gcsna_pdu)) || (gcsna_pdu_len == 0) )
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "DEBUG: Wrong GCSNA PDU len:%d, returning",
      gcsna_pdu_len);
    return FALSE;
  }
  /* send the message over MSGR to LTE_NAS */
  return( mcc_gcsna_send_ul_msg(GCSNA_1X_CRKT_SRV_MSG,gcsna_pdu_len,ul_txssion_type) );

} /* mcc_gcsna_send_1x_crkt_srv_msg */

/*===========================================================================

FUNCTION MCC_GCSNA_SEND_GCSNA_L2_ACK

DESCRIPTION
  This function sends GCSNA L2 ack

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean mcc_gcsna_send_gcsna_l2_ack(boolean ack_required)
{
  word gcsna_pdu_len;
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Txing GCSNA ACk");
  /* Build GCSNA message */
  gcsna_pdu_len = mcc_gcsna_build_gcsna_pdu(GCSNA_L2_ACK,ack_required,FALSE);

  /* Check if the calculated GCSNA message len is greater than the PDU
   * which needs to be transmitted.
   */
  if( (gcsna_pdu_len > sizeof(mcc_gcsna_pdu)) || (gcsna_pdu_len == 0) )
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "DEBUG: Wrong GCSNA PDU len:%d, returning",
      gcsna_pdu_len);
    return FALSE;
  }
  /* send the message over MSGR to LTE_NAS */
  return(mcc_gcsna_send_ul_msg(GCSNA_L2_ACK,gcsna_pdu_len,GCSNA_USE_UL_INFO_TX_MSG_TYPE));

} /* mcc_gcsna_send_gcsna_l2_ack */

/*===========================================================================
FUNCTION MCC_GCSNA_SEND_GCSNA_REJ

DESCRIPTION
  This function sends GCSNA Reject

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean mcc_gcsna_send_gcsna_rej(boolean ack_required)
{
  word gcsna_pdu_len;
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Txing GCSNA Rej");
  /* Build GCSNA message */
  gcsna_pdu_len = mcc_gcsna_build_gcsna_pdu(GCSNA_SRV_REJ_MSG,ack_required,FALSE);

  /* Check if the calculated GCSNA message len is greater than the PDU
   * which needs to be transmitted.
   */
  if( (gcsna_pdu_len > sizeof(mcc_gcsna_pdu)) || (gcsna_pdu_len == 0) )
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "DEBUG: Wrong GCSNA PDU len:%d, returning",
      gcsna_pdu_len);
    return FALSE;
  }
  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: GCSNA Rej len = %d ",
    gcsna_pdu_len);

  /* send the message over MSGR to LTE_NAS */
  return(mcc_gcsna_send_ul_msg(GCSNA_SRV_REJ_MSG,gcsna_pdu_len,GCSNA_USE_UL_INFO_TX_MSG_TYPE));

} /* mcc_gcsna_send_gcsna_rej */

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
void mcc_gcsna_msg_tx(mcc_gcsna_msg_type gcsna_msg_id,
                      boolean ack_req,
                      gcsna_ul_tx_type ul_txssion_type)
{

  /* -------------------
  ** Message type Check
  ** ------------------- */
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: TX GCSNA msg = %d, ack_req = %d",
    gcsna_msg_id,
    ack_req);

  /* Stop any previous GCSNA transmission attempts */
  mcc_csfb_gcsna_stop_txn();

  switch(gcsna_msg_id)
  {
    case GCSNA_L2_ACK:
    {
      (void)mcc_gcsna_send_gcsna_l2_ack(ack_req);
      break;
    }
    case GCSNA_1X_PARAM_REQ:
    {
      (void)mcc_gcsna_send_1x_param_rqst();
      break;
    }
    case GCSNA_1X_CRKT_SRV_MSG:
    {
      (void)mcc_gcsna_send_1x_crkt_srv_msg(ack_req,ul_txssion_type);
      break;
    }
    case GCSNA_SRV_REJ_MSG:
    {
     (void)mcc_gcsna_send_gcsna_rej(ack_req);
      break;
    }
    default:
    {
      break;
    }
  }
} /* mcc_gcsna_msg_tx */

/*===========================================================================

FUNCTION mcc_gcsna_parse_dl_info_msg

DESCRIPTION
  This function parses the received dl info transfer message.
  The DL info messages which could be received over Tunnel
  are GCSNA_L2_ACK/GCSNA_1X_CRKT_SRV_MSG/GCSNA_SRV_REJ.

  GCSNA_L2_ACK : It carries the L2 ack sent by BS (it serves like BS Order for the
                 UL messages needing ack.
  GCSNA_1X_CRKT_SRV_MSG : It carries the CAI L3 PDU encapsulated in TLAC+GCSNA
                          headers.
  GCSNA_SRV_REJ: Carries the GCSNA reject message , in which the IWS proposes new
                 GCSNA Options.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
void mcc_gcsna_parse_dl_info_msg(
  uint8 *buf_ptr,
    /* Pointer to the received message */
  uint32 len
    /* Pointer to the number of bytes received in buf_ptr */
)
{
  /* Position in message relative to start of received buf_ptr */
  word ext_msg_pos = 0;

  byte i = 0;

  /* pointer to the buffer where message rxed needs to be stored internally.*/
  gcsna_dl_msg_type *int_ptr = mcc_gcsna_ext_msg_ptr;

  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG:Parsing DL Info Txnsfer msg");

  /* set the GCSNA valid message indication to TRUE */
  mcc_gcsna_ext_msg_ptr->gen_msg_id.is_l3_msg = MCC_CSFB_L3_MSG_RXED;

   /* Unpack the Gen GCSNA message type*/
  UNPACKB_PG(gen_msg_id,gcsna_ack_pdu_type, msg_id);

  /* Unpack rest of the message based on message id and store it into internal buffer
  * mcc_gcsna_int_msg is the internal buffer where the rxed DL message will be stored.
  */

  switch(mcc_gcsna_ext_msg_ptr->gen_msg_id.msg_id)
  {
    case GCSNA_L2_ACK:
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG: Rxed GCSNA_L2_ACK");
      mcc_gcsna_ext_msg_ptr->gen_msg_id.cdma_chn = GCSNA_CHN_PC;
      UNPACKB_PG(gcsna_ack,gcsna_ack_pdu_type, ack_seq);
      UNPACKB_PG(gcsna_ack,gcsna_ack_pdu_type, gcsna_reserved);
      break;
    }
    case GCSNA_1X_CRKT_SRV_MSG:
    {
      mcc_gcsna_parse_1x_crkt_srv_msg((buf_ptr+(ext_msg_pos/8)), len);
      break;
    }
    case GCSNA_SRV_REJ_MSG:
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG: Rxed GCSNA_SRV_REJ_MSG");
      mcc_gcsna_ext_msg_ptr->gen_msg_id.cdma_chn = GCSNA_CHN_PC;
      /* Extract GCSNA Reject fileds here */
      UNPACKB_PG(gcsna_rej,gcsna_rej_pdu_type,rej_seq);
      UNPACKB_PG(gcsna_rej,gcsna_rej_pdu_type,cause);

      if( (mcc_gcsna_ext_msg_ptr->gcsna_rej.cause == GCSNA_OPT_MISMATCH) ||
          (mcc_gcsna_ext_msg_ptr->gcsna_rej.cause == GCSNA_OPT_PREV_MISMATCH)
        )
      {
        UNPACKB_PG(gcsna_rej,gcsna_rej_pdu_type,num_fields);
         for(i=0; i<mcc_gcsna_ext_msg_ptr->gcsna_rej.num_fields; i++)
        {
          UNPACKB_PG(gcsna_rej,gcsna_rej_pdu_type,gcsna_opt);
        }
      }
      if( (mcc_gcsna_ext_msg_ptr->gcsna_rej.cause == GCSNA_PREV_MISMATCH) ||
          (mcc_gcsna_ext_msg_ptr->gcsna_rej.cause == GCSNA_OPT_PREV_MISMATCH)
        )
      {
        UNPACKB_PG(gcsna_rej,gcsna_rej_pdu_type,prev_1x);
      }
      if(mcc_gcsna_ext_msg_ptr->gcsna_rej.cause == GCSNA_INV_MSG_ID)
      {
        UNPACKB_PG(gcsna_rej,gcsna_rej_pdu_type,msg_id_inv);
      }
      break;
    }
    default:
      break;
  }
} /* mcc_gcsna_parse_dl_info_msg */

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
void mcc_gcsna_parse_1x_crkt_srv_msg(uint8 *buf_ptr, uint32 len)
{
  size_t copy_size;
  /* Return value of memscpy function */
  byte i;
  /* Position in message relative to start of received buf_ptr */
  uint32 ext_msg_pos = 0;

  /* pointer to the buffer where message rxed needs to be stored internally.*/
  gcsna_dl_msg_type *int_ptr = mcc_gcsna_ext_msg_ptr;

  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: received GCSNA_1X_CRKT_SRV_MSG");

  /* GCSNA_1X_CRKT_SRV_MSG contains GCSNA Header -> TLAC header -> CAI L3 PDU
   * To extract the L3 PDU, first we need to unpack the GCSNA  then TLAC headers
   */

  /* ------------------------------------------------------------------------*/
  /* Extracting GCSNA header for GCSNA_1X_CRKT_SRV_MSG */
  /* ------------------------------------------------------------------------*/

  UNPACKB_PG(ckt_srv_msg, gcsna_1x_crkt_srv_msg_type, gcsna_opt);
  UNPACKB_PG(ckt_srv_msg, gcsna_1x_crkt_srv_msg_type, alt_gcsna_opt_incl);

  /* Check if ALT GCSNA options are included or not. */
  if(int_ptr->ckt_srv_msg.alt_gcsna_opt_incl == TRUE)
  {
    UNPACKB_PG(ckt_srv_msg, gcsna_1x_crkt_srv_msg_type, num_alt_gcsna_opt);
    /* Loop through all the ALT GCSNA options that are included. */
    for(i=0; i < int_ptr->ckt_srv_msg.num_alt_gcsna_opt; i++)
    {
      int_ptr->ckt_srv_msg.alt_gcsna_options[i].alt_gcsna_opt =
        b_unpackb(buf_ptr, ext_msg_pos, FSIZ(alt_gcsna_opt_field_type, alt_gcsna_opt));
      ext_msg_pos += FSIZ(alt_gcsna_opt_field_type, alt_gcsna_opt);
    }
  }

  /* Check if we are compatible with the GCSNA option , else start GCSNA
   * service negotioation by sending GCSNA Reject.
   */
  if(mcc_gcsna_do_gcsna_option_chk(GCSNA_1X_CRKT_SRV_MSG) == TRUE)
  {
    /* GCSNA check passed, proceed for next fileds parsing */
    UNPACKB_PG(ckt_srv_msg, gcsna_1x_crkt_srv_msg_type, iws_id_incl);

    /* Check if IWS id is included or not. */
    if(int_ptr->ckt_srv_msg.iws_id_incl == TRUE)
    {
      UNPACKW_PG(ckt_srv_msg, gcsna_1x_crkt_srv_msg_type, iws_id);
    }

    UNPACKB_PG(ckt_srv_msg, gcsna_1x_crkt_srv_msg_type, ack_reqd);
    UNPACKB_PG(ckt_srv_msg, gcsna_1x_crkt_srv_msg_type, stop_dup_detect);
    UNPACKB_PG(ckt_srv_msg, gcsna_1x_crkt_srv_msg_type, msg_seq);
    UNPACKB_PG(ckt_srv_msg, gcsna_1x_crkt_srv_msg_type, num_bundled_tlac_l3_rec);
    UNPACKB_PG(ckt_srv_msg, gcsna_1x_crkt_srv_msg_type, gcsna_reserved);

    /* ------------------------------------------------------------------------*/
    /* Extract the TLAC header */
    /* ------------------------------------------------------------------------*/

    UNPACKB_PG(ckt_srv_msg, gcsna_1x_crkt_srv_msg_type, logical_chn);
    UNPACKB_PG(ckt_srv_msg, gcsna_1x_crkt_srv_msg_type, prev_1x);
    UNPACKB_PG(ckt_srv_msg, gcsna_1x_crkt_srv_msg_type, msg_type);
    UNPACKB_PG(ckt_srv_msg, gcsna_1x_crkt_srv_msg_type, num_tlac_hdr_rec);

    /* Loop through all the TLAC header records. */
    for(i=0;
       (i < int_ptr->ckt_srv_msg.num_tlac_hdr_rec) && (i < MAX_NUM_TLAC_HDR_REC);
        i++)
    {
      int_ptr->ckt_srv_msg.tlac_rec[i].tlac_hdr_rec_type =
        b_unpackb(buf_ptr, ext_msg_pos, FSIZ(tlac_rec_field_type, tlac_hdr_rec_type));
      ext_msg_pos += FSIZ(tlac_rec_field_type, tlac_hdr_rec_type);

      int_ptr->ckt_srv_msg.tlac_rec[i].tlac_hdr_rec_len =
        b_unpackb(buf_ptr, ext_msg_pos, FSIZ(tlac_rec_field_type, tlac_hdr_rec_len));
      ext_msg_pos += FSIZ(tlac_rec_field_type, tlac_hdr_rec_len);

      if( 8 * int_ptr->ckt_srv_msg.tlac_rec[i].tlac_hdr_rec_len <= MAX_TLAC_HDR_REC_LEN )
      {
        copy_size = memscpy(
                      (int_ptr->ckt_srv_msg.tlac_rec[i].tlac_hdr_rec),
                       MAX_TLAC_HDR_REC_LEN*sizeof(byte),
                      (buf_ptr + ext_msg_pos),
                       int_ptr->ckt_srv_msg.tlac_rec[i].tlac_hdr_rec_len
                     );
        if( copy_size != int_ptr->ckt_srv_msg.tlac_rec[i].tlac_hdr_rec_len )
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
            copy_size,
            int_ptr->ckt_srv_msg.tlac_rec[i].tlac_hdr_rec_len,
            MAX_TLAC_HDR_REC_LEN*sizeof(byte));
        }
       ext_msg_pos += (8 * int_ptr->ckt_srv_msg.tlac_rec[i].tlac_hdr_rec_len);
      }
      else
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "DEBUG: Unexpected 1X crkt service MSG with TLAC HDR REC LEN:%d > MAX_TLAC_HDR_REC_LEN",
          int_ptr->ckt_srv_msg.tlac_rec[i].tlac_hdr_rec_len);

        /* Fill the L3 message received to not received */
        mcc_gcsna_ext_msg_ptr->gen_msg_id.is_l3_msg = MCC_CSFB_L3_MSG_RXED_INCORRECT;
        return;
      }
    }

    /* If num_tlac_hdr_rec is odd than reserved bits are 7 otherwise, its 3.
     * This functionality ios for removing padding in TLCA header
     */
    if(int_ptr->ckt_srv_msg.num_tlac_hdr_rec % 2 == 1)
    {
      ext_msg_pos += 7;
    }
    else
    {
      ext_msg_pos += 3;
    }

    /* Before unpacking the L3 PDU, check if we have received prev correctly in GCSNA
     * header. If we havent received a good header declare failure and transmit
     * GCSNA reject.
     * The prev received in GCSNA will be processed as per following rules:
     * For DL Messages (sent by IWS)
     *       * GCSNA 1xProtocolRevision will be set by the IWS to P_REV_IN_USE
     *         or MIN_P_REV (same as C2K Parameters) if P_REV_IN_USE is not known.
     *       * Mobile shall accept any message <= MOB_PREV. A GSCSNA Reject
     *         shall be sent if a message includes 1xProtocolRevision > MOB_PREV.
     *         (This includes the Mobility from EUTRA case. )
     * For UL messages (rxed by IWS)
     *       * Mobile will set GCSNA 1xProtocolRevision  to P_REV_IN _USE
     *       * IWS will accept a message greater than MIN_P_REV and less than
     *         or equal to P_REV.
     */
    if( !( mcc_gcsna_do_gcsna_1x_prev_chk(GCSNA_1X_CRKT_SRV_MSG) ) )
    {
      /* We have received a message whose 1xprev is > maximum mobile capability,
       * so reject the message
       * Fill the L3 message received to not received */
      mcc_gcsna_ext_msg_ptr->gen_msg_id.is_l3_msg = MCC_CSFB_L3_MSG_GCSNA_PREV_REJ;
      return;
    }

    /* ------------------------------------------------------------------------*/
    /* Unpack the L3 PDU. */
    /* ------------------------------------------------------------------------*/

    /* Fill the CDMA channel on which we rxed the message */
    mcc_gcsna_ext_msg_ptr->gen_msg_id.cdma_chn = (gcsna_chan_type)int_ptr->ckt_srv_msg.logical_chn;

    /* Unpack the L3 PDU. */
    UNPACKW_PG(ckt_srv_msg, gcsna_1x_crkt_srv_msg_type, l3_pdu_len);

    /* Printing the L3 PDU length*/
    M1X_MSG( DCP, LEGACY_MED,
      "DEBUG: L3 PDU len = %d ",
      mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_pdu_len);

    mcc_gcsna_xlate_l3_msg((buf_ptr+(ext_msg_pos/8)),
                         (8 * mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_pdu_len));
    
    if( (8 * mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_pdu_len) <= MAX_TLAC_L3_PDU_LEN )
    {
      ext_msg_pos += (8 * int_ptr->ckt_srv_msg.l3_pdu_len);
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
      "DEBUG: Received 1X crkt service MSG with L3 PDU LEN: %d > MAX_TLAC_L3_PDU_LEN",
      (8 * mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_pdu_len));
      mcc_gcsna_ext_msg_ptr->gen_msg_id.is_l3_msg = MCC_CSFB_L3_MSG_RXED_INCORRECT;
      return;
    }
    
    /* Check if there is another bundled record */
    if(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.num_bundled_tlac_l3_rec > 0)
    {
      /* Unpack the message and place it in the unprocessed buffer */
      /* Ideally the TLAC header for this second message should be same as
       * that of previous messgae.
       */
      UNPACKB_PG_BUNDLED(ckt_srv_msg,gcsna_1x_crkt_srv_msg_type,logical_chn,bundled_logical_chn);
      UNPACKB_PG_BUNDLED(ckt_srv_msg,gcsna_1x_crkt_srv_msg_type,prev_1x,bundled_prev_1x);
      UNPACKB_PG_BUNDLED(ckt_srv_msg,gcsna_1x_crkt_srv_msg_type,msg_type,bundled_msg_type);
      UNPACKB_PG_BUNDLED(ckt_srv_msg,gcsna_1x_crkt_srv_msg_type,num_tlac_hdr_rec,bundled_num_tlac_hdr_rec);
      /* Loop through all the TLAC header records. */
      for(i=0;
          ( (i < int_ptr->ckt_srv_msg.bundled_num_tlac_hdr_rec) &&  (i < MAX_NUM_TLAC_HDR_REC) ) ;
          i++
         )
      {
        int_ptr->ckt_srv_msg.bundled_tlac_rec[i].tlac_hdr_rec_type =
          b_unpackb(buf_ptr, ext_msg_pos, FSIZ(tlac_rec_field_type, tlac_hdr_rec_type));
        ext_msg_pos += FSIZ(tlac_rec_field_type, tlac_hdr_rec_type);

        int_ptr->ckt_srv_msg.bundled_tlac_rec[i].tlac_hdr_rec_len =
          b_unpackb(buf_ptr, ext_msg_pos, FSIZ(tlac_rec_field_type, tlac_hdr_rec_len));
        ext_msg_pos += FSIZ(tlac_rec_field_type, tlac_hdr_rec_len);
        if( 8 * int_ptr->ckt_srv_msg.tlac_rec[i].tlac_hdr_rec_len <= MAX_TLAC_HDR_REC_LEN )
        {
        copy_size = memscpy(
                      (int_ptr->ckt_srv_msg.bundled_tlac_rec[i].tlac_hdr_rec),
                      MAX_TLAC_HDR_REC_LEN*sizeof(byte),
                      (buf_ptr + ext_msg_pos),
                      int_ptr->ckt_srv_msg.tlac_rec[i].tlac_hdr_rec_len);

        if( copy_size != int_ptr->ckt_srv_msg.tlac_rec[i].tlac_hdr_rec_len )
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
            copy_size,
            int_ptr->ckt_srv_msg.tlac_rec[i].tlac_hdr_rec_len,
            MAX_TLAC_HDR_REC_LEN*sizeof(byte));
        }

          ext_msg_pos += (8 * int_ptr->ckt_srv_msg.tlac_rec[i].tlac_hdr_rec_len);
        }
        else
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "DEBUG: Received 1X crkt service MSG with TLAC HDR REC LEN:%d > MAX_TLAC_HDR_REC_LEN",
            int_ptr->ckt_srv_msg.tlac_rec[i].tlac_hdr_rec_len);

          /* Fill the L3 message received to not received */
          mcc_gcsna_ext_msg_ptr->gen_msg_id.is_l3_msg = MCC_CSFB_L3_MSG_RXED_INCORRECT;
          return;
        }
      }
      /* If num_tlac_hdr_rec is odd than reserved bits are 7 otherwise, its 3.
       * This functionality ios for removing padding in TLCA header
       */
      if(int_ptr->ckt_srv_msg.bundled_num_tlac_hdr_rec % 2 == 1)
      {
        ext_msg_pos += 7;
      }
      else
      {
        ext_msg_pos += 3;
      }
      /* Unpack the L3 PDU. */
      UNPACKW_PG_BUNDLED(ckt_srv_msg, gcsna_1x_crkt_srv_msg_type, l3_pdu_len,bundled_l3_pdu_len);

      if( (8 * mcc_gcsna_ext_msg_ptr->ckt_srv_msg.bundled_l3_pdu_len) <= MAX_TLAC_L3_PDU_LEN )
      {
        /* Copy unprocessed L3 msg and process it later */
      copy_size = memscpy
                    (int_ptr->ckt_srv_msg.bundled_l3_msg, MAX_TLAC_L3_PDU_LEN*sizeof(byte),
                     (buf_ptr+(ext_msg_pos/8)), (mcc_gcsna_ext_msg_ptr->ckt_srv_msg.bundled_l3_pdu_len)
                    );
      if( copy_size != (8 * mcc_gcsna_ext_msg_ptr->ckt_srv_msg.bundled_l3_pdu_len) )
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
          copy_size,
          (8*mcc_gcsna_ext_msg_ptr->ckt_srv_msg.bundled_l3_pdu_len),
          MAX_TLAC_L3_PDU_LEN*sizeof(byte));
      }

        ext_msg_pos += (8 * int_ptr->ckt_srv_msg.bundled_l3_pdu_len);
      }
      else
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "DEBUG: Received 1X crkt service MSG with BUNDLED L3 PDU LEN:%d > MAX_TLAC_L3_PDU_LEN",
          (8 * mcc_gcsna_ext_msg_ptr->ckt_srv_msg.bundled_l3_pdu_len));

        /* Fill the L3 message received to not received */
        mcc_gcsna_ext_msg_ptr->gen_msg_id.is_l3_msg = MCC_CSFB_L3_MSG_RXED_INCORRECT;
        return;
      }
    } /* if(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.num_bundled_tlac_l3_rec > 0) */
  } /* end of if(mcc_gcsna_do_gcsna_option_chk() == TRUE) */
  else
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "DEBUG: Unsupported GCSNA option, sending GCSNA Reject");
    /* Fill the L3 message received to not received */
    mcc_gcsna_ext_msg_ptr->gen_msg_id.is_l3_msg = MCC_CSFB_L3_MSG_GCSNA_OPT_REJ;
  }
} /* mcc_gcsna_parse_1x_crkt_srv_msg */


/*===========================================================================
FUNCTION  MCC_GCSNA_PROCESS_DL_INFO

DESCRIPTION
  This function checks the duplicate and starts a context timer for this msg.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  : if this msg is a duplicated,
  FALSE : otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
boolean mcc_gcsna_detect_duplicate(byte  iws_id_incl,
                                word  iws_id,
                                byte  stop_dup_detect,
                                byte  msg_seq)
{
  byte i,j;
  boolean result = FALSE;
  qword cur_time, time_diff, time_thresh;

  /* initialize the current time to zero */
  qw_set(cur_time, 0, 0);

  m1x_time_get_20ms_frame_time(cur_time);

  if(stop_dup_detect)
  {
    /* Reset the List */
    mcc_gcsna_msg_list.num_entries = 0;
  }

  /* Find out the index of the first active entry. */
  for(i = 0; (i < mcc_gcsna_msg_list.num_entries) &&
             (i < MAX_CKT_SRV_MSGS_STORED_FOR_DUP_DETECTION); i++)
  {
    qw_sub(time_diff,cur_time,mcc_gcsna_msg_list.entry[i].rxed_timestamp);
    qw_set(time_thresh, 0, (c2k_parms_msg.gcsna_seq_context_timer*5));
    if(qw_cmp(time_diff, time_thresh) < 0)
    {
      break;
    }
  }

  /* Remove Older values from the list and shift the newer values from start. */
  for(j = i; (j != 0)&&
             (j < mcc_gcsna_msg_list.num_entries) &&
             (j < MAX_CKT_SRV_MSGS_STORED_FOR_DUP_DETECTION); j++)
  {
    mcc_gcsna_msg_list.entry[j-i].iws_id_incl = mcc_gcsna_msg_list.entry[j].iws_id_incl;
    mcc_gcsna_msg_list.entry[j-i].iws_id = mcc_gcsna_msg_list.entry[j].iws_id;
    mcc_gcsna_msg_list.entry[j-i].msg_seq = mcc_gcsna_msg_list.entry[j].msg_seq;
    qw_equ(mcc_gcsna_msg_list.entry[j-i].rxed_timestamp, mcc_gcsna_msg_list.entry[j].rxed_timestamp);
  }

  mcc_gcsna_msg_list.num_entries = mcc_gcsna_msg_list.num_entries-i;

  /* Find out if the msg is duplicate.*/
  for(i = 0; (i < mcc_gcsna_msg_list.num_entries) &&
             (i < MAX_CKT_SRV_MSGS_STORED_FOR_DUP_DETECTION); i++)
  {
    /* | iws_id_incl | entry[i].iws_id_incl | IWS ID matches | ==> Result    |
     * |-------------|----------------------|----------------|---------------|
     * |      0      |         0            |       0        | ==> Duplicate |
     * |      0      |         0            |       1        | ==> Duplicate |
     * |      0      |         1            |       0        | ==> New       |
     * |      0      |         1            |       1        | ==> New       |
     * |      1      |         0            |       0        | ==> New       |
     * |      1      |         0            |       1        | ==> New       |
     * |      1      |         1            |       0        | ==> New       |
     * |      1      |         1            |       1        | ==> Duplicate |
     * |-------------|----------------------|----------------|---------------|
     */

    if(msg_seq == mcc_gcsna_msg_list.entry[i].msg_seq)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "msg_seq %d matched with index = %d",
        msg_seq,
        i);
      if( iws_id_incl &&
          ((!mcc_gcsna_msg_list.entry[i].iws_id_incl) ||
          (iws_id != mcc_gcsna_msg_list.entry[i].iws_id))
         )
      {
        M1X_MSG( DCP, LEGACY_MED,
          "IWS_IDs didn't match!!");
        result = FALSE;
      }
      else if((!iws_id_incl) &&  (mcc_gcsna_msg_list.entry[i].iws_id_incl))
      {
        M1X_MSG( DCP, LEGACY_MED,
          "IWS id is not included in this message. Hence, no match.");
        result = FALSE;
      }
      else
      {
        result = TRUE;
        break;
      }
    }
  }

  /* if this is a new message, add it to the list. */
  if((result == FALSE) && (i < MAX_CKT_SRV_MSGS_STORED_FOR_DUP_DETECTION))
  {
    M1X_MSG( DCP, LEGACY_MED,
      "Adding the New message to mcc_gcsna_msg_list with index =%d",
      i);
    mcc_gcsna_msg_list.entry[i].iws_id_incl = iws_id_incl;
    mcc_gcsna_msg_list.entry[i].iws_id = iws_id;
    mcc_gcsna_msg_list.entry[i].msg_seq = msg_seq;
    qw_equ(mcc_gcsna_msg_list.entry[i].rxed_timestamp, cur_time);
    mcc_gcsna_msg_list.num_entries++;
  }

  return result;
}
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

void mcc_gcsna_process_dl_info(lte_irat_tunnel_msg_container_s *buf_ptr)
{

  /* Unpack and parse the entire message and store it in local buffer */
  mcc_gcsna_parse_dl_info_msg(buf_ptr->msg_payload,buf_ptr->msg_size);

  if(mcc_gcsna_ext_msg_ptr->gen_msg_id.msg_id == GCSNA_1X_CRKT_SRV_MSG)
  {
    /* Check if the message is a duplicate of the previously sent messages. */
    if (mcc_gcsna_detect_duplicate(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.iws_id_incl,
                                   mcc_gcsna_ext_msg_ptr->ckt_srv_msg.iws_id,
                                   mcc_gcsna_ext_msg_ptr->ckt_srv_msg.stop_dup_detect,
                                   mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_seq))
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Duplicate message Rxed");
      mcc_gcsna_ext_msg_ptr->gen_msg_id.is_l3_msg = MCC_CSFB_DUP_L3_MSG_RXED;
    }
  }

  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG:rxed DL Info Tx msg = %d, is_l3_msg rxed = %d",
    mcc_gcsna_ext_msg_ptr->gen_msg_id.msg_id,
    mcc_gcsna_ext_msg_ptr->gen_msg_id.is_l3_msg);

  switch(mcc_gcsna_ext_msg_ptr->gen_msg_id.msg_id)
  {
    case GCSNA_L2_ACK:
    {
      if(gcsna_msg_sent.ack_req == TRUE)
      {
        if(gcsna_msg_sent.msg_seq == mcc_gcsna_ext_msg_ptr->gcsna_ack.ack_seq)
        {
          M1X_MSG( DCP, LEGACY_HIGH,
            "DEBUG: GCSNA Ack received for msg needing ack");
          mcc_gcsna_ext_msg_ptr->gen_msg_id.is_l3_msg = MCC_CSFB_L3_MSG_RXED;
        }
      }
      break;
    }
    case GCSNA_SRV_REJ_MSG:
    {
      if(gcsna_msg_sent.msg_seq == mcc_gcsna_ext_msg_ptr->gcsna_rej.rej_seq)
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "DEBUG: GCSNA Rej received for 1x CRKT SRV msg sent");
        mcc_gcsna_ext_msg_ptr->gen_msg_id.is_l3_msg = MCC_CSFB_L3_MSG_RXED;
      }
      break;
    }
    default:
      break;
  } /* switch(mcc_gcsna_int_msg.gen_msg_id.msg_id)*/
} /* mcc_gcsna_process_dl_info */

/*===========================================================================

FUNCTION MCC_GCSNA_MSG_RX

DESCRIPTION
  This function processes GCSNA messages received from LTE over MSGR while in the
  CSFB Idle State or CSFB Access Substate.

  The messages which could be received in this state are:
  GCSNA 1X Circuit Service Message
  GCSNA L2 ack
  GCSNA Service Reject

DEPENDENCIES
  None.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/
void mcc_csfb_log_1x_ul_msg(byte* ul_gcsna_msg, word gcsna_pdu_len)
{
  word word_field;
  byte byte_field;
  byte loop_var;
  boolean bool_field;
  word pos = 0 ;
  byte i;

  for(i=0;i<gcsna_pdu_len;i++)
  {
    M1X_MSG( DCP, LEGACY_MED,
      "ul_gcsna_msg[%d] = %02x",
      i,
      ul_gcsna_msg[i]);
  }

  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG ------  1x CRKT SRV MSG ------------");
  M1X_MSG( DCP, LEGACY_HIGH,
    " ------ GCSNA Header -------");
  byte_field = b_unpackb(mcc_gcsna_pdu,pos,8);
  M1X_MSG( DCP, LEGACY_MED,
    "MessageID = %d",
    byte_field);
  pos+=8;
  byte_field = b_unpackb(mcc_gcsna_pdu,pos,8);
  M1X_MSG( DCP, LEGACY_MED,
    "GCSNAOption = %d",
    byte_field);
  pos+=8;
  bool_field = b_unpackb(mcc_gcsna_pdu,pos,1);
  M1X_MSG( DCP, LEGACY_MED,
    "AlternativeGCSNAOption_INCL = %d",
    bool_field);
  pos+=1;
  bool_field = b_unpackb(mcc_gcsna_pdu,pos,1);
  M1X_MSG( DCP, LEGACY_MED,
    "IWSIDIncl = %d",
    bool_field);
  pos+=1;
  bool_field = b_unpackb(mcc_gcsna_pdu,pos,1);
  M1X_MSG( DCP, LEGACY_MED,
    "AckRequired = %d",
    bool_field);
  pos+=1;
  bool_field = b_unpackb(mcc_gcsna_pdu,pos,1);
  M1X_MSG( DCP, LEGACY_MED,
    "StopDupDetect = %d",
    bool_field);
  pos+=1;
  byte_field = b_unpackb(mcc_gcsna_pdu,pos,6);
  M1X_MSG( DCP, LEGACY_MED,
    "MessageSequence = %d",
    byte_field);
  pos+=6;
  byte_field = b_unpackb(mcc_gcsna_pdu,pos,6);
  M1X_MSG( DCP, LEGACY_MED,
    "Reserved = %d",
    byte_field);
  pos+=6;
  M1X_MSG( DCP, LEGACY_MED,
    " ------ TLAC Header -------");

  bool_field = b_unpackb(mcc_gcsna_pdu,pos,1);
  M1X_MSG( DCP, LEGACY_MED,
    "1xLogicalChannel = %d",
    bool_field);
  pos+=1;
  byte_field = b_unpackb(mcc_gcsna_pdu,pos,8);
  M1X_MSG( DCP, LEGACY_MED,
    "1xProtocolRevision = %d",
    byte_field);
  pos+=8;
  byte_field = b_unpackb(mcc_gcsna_pdu,pos,8);
  M1X_MSG( DCP, LEGACY_MED,
    "MsgType = %d",
    byte_field);
  pos+=8;
  byte_field = b_unpackb(mcc_gcsna_pdu,pos,4);
  M1X_MSG( DCP, LEGACY_MED,
    "NumTLACHeaderRecords = %d",
    byte_field);
  pos+=4;
  byte_field = b_unpackb(mcc_gcsna_pdu,pos,4);
  M1X_MSG( DCP, LEGACY_MED,
    "TLACHeaderRecordType = %d",
    byte_field);
  pos+=4;
  byte_field = b_unpackb(mcc_gcsna_pdu,pos,8);
  M1X_MSG( DCP, LEGACY_MED,
    "TLACHeaderRecordLength = %d",
    byte_field);
  pos+=8;
  loop_var = byte_field;
  while(loop_var-- > 0)
  {
    byte_field = b_unpackb(mcc_gcsna_pdu,pos,8);
    M1X_MSG( DCP, LEGACY_MED,
      "TLACHeaderRecord = %d",
      byte_field);
    pos+=8;
  }
  byte_field = b_unpackb(mcc_gcsna_pdu,pos,7);
  M1X_MSG( DCP, LEGACY_MED,
    "Reserved = %d",
    byte_field);
  pos+=7;
  M1X_MSG( DCP, LEGACY_HIGH,
    " ------ L3 Header -------");

  word_field = b_unpackw(mcc_gcsna_pdu,pos,16);
  M1X_MSG( DCP, LEGACY_MED,
    "1xL3PDULength = %d",
    word_field);
  pos+=16;

  loop_var = word_field;
  byte_field = b_unpackb(mcc_gcsna_pdu,pos,4);
  M1X_MSG( DCP, LEGACY_MED,
    "REG_TYPE = %d",
    byte_field);
  pos+=4;
  byte_field = b_unpackb(mcc_gcsna_pdu,pos,3);
  M1X_MSG( DCP, LEGACY_MED,
    "SLOT_CYCLE_INDEX = %d",
    byte_field);
  pos+=3;
  byte_field = b_unpackb(mcc_gcsna_pdu,pos,8);
  M1X_MSG( DCP, LEGACY_MED,
    "MOB_P_REV = %d",
    byte_field);
  pos+=8;
  byte_field = b_unpackb(mcc_gcsna_pdu,pos,8);
  M1X_MSG( DCP, LEGACY_MED,
    "SCM = %d",
    byte_field);
  pos+=8;
  bool_field = b_unpackb(mcc_gcsna_pdu,pos,1);
  M1X_MSG( DCP, LEGACY_MED,
    "MOB_TERM = %d",
    bool_field);
  pos+=1;
  byte_field = b_unpackb(mcc_gcsna_pdu,pos,4);
  M1X_MSG( DCP, LEGACY_MED,
    "RETURN_CAUSE = %d",
    byte_field);
  pos+=4;
  bool_field = b_unpackb(mcc_gcsna_pdu,pos,1);
  M1X_MSG( DCP, LEGACY_MED,
    "QPCH_SUPPORTED = %d",
    bool_field);
  pos+=1;
  bool_field = b_unpackb(mcc_gcsna_pdu,pos,1);
  M1X_MSG( DCP, LEGACY_MED,
    "ENHANCED_RC = %d",
    bool_field);
  pos+=1;
  bool_field = b_unpackb(mcc_gcsna_pdu,pos,1);
  M1X_MSG( DCP, LEGACY_MED,
    "UZID_INCL = %d",
    bool_field);
  pos+=1;
  bool_field = b_unpackb(mcc_gcsna_pdu,pos,1);
  M1X_MSG( DCP, LEGACY_MED,
    "GEO_LOC_INCL = %d",
    bool_field);
  pos+=1;
  if( bool_field == TRUE)
  {
    byte_field = b_unpackb(mcc_gcsna_pdu,pos,3);
    M1X_MSG( DCP, LEGACY_MED,
      "GEO_LOC_TYPE = %d",
      byte_field);
    pos+=3;
  }
  bool_field = b_unpackb(mcc_gcsna_pdu,pos,1);
  M1X_MSG( DCP, LEGACY_MED,
    "OTD_SUPPORTED = %d",
    bool_field);
  pos+=1;
  bool_field = b_unpackb(mcc_gcsna_pdu,pos,1);
  M1X_MSG( DCP, LEGACY_MED,
    "STS_SUPPORTED = %d",
    bool_field);
  pos+=1;
  bool_field = b_unpackb(mcc_gcsna_pdu,pos,1);
  M1X_MSG( DCP, LEGACY_MED,
    "3X_CCH_SUPPORTED = %d",
    bool_field);
  pos+=1;
  bool_field = b_unpackb(mcc_gcsna_pdu,pos,1);
  M1X_MSG( DCP, LEGACY_MED,
    "WLL_INCL = %d",
    bool_field);
  pos+=1;
  bool_field = b_unpackb(mcc_gcsna_pdu,pos,1);
  M1X_MSG( DCP, LEGACY_MED,
    "ENC_INFO_INCL = %d",
    bool_field);
  pos+=1;
  bool_field = b_unpackb(mcc_gcsna_pdu,pos,1);
  M1X_MSG( DCP, LEGACY_MED,
    "MSG_INT_INFO_INCL = %d",
    bool_field);
  pos+=1;

}

/*===========================================================================
FUNCTION  GCSNA_XLATE_PC_ORDM

DESCRIPTION
  This function translates a Paging Channel Order Message from
  external to internal format. This function can be compared with
  xlate_ext_pc_ord.

DEPENDENCIES
  Assumes that the internal message type received as an input to the
  procedure already includes the Order message type.

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/

word gcsna_xlate_pc_ordm
(
  word len,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to received message to be translated */
  caii_rx_msg_type *int_ptr
    /* Pointer to place where translated message should be placed */
)
{
  word num_recs;
    /* Number of orders contained in received Paging Channel Order Msg */
  word msg_pos;
    /* Position in message relative to start of buf_ptr */
  byte add_record_len;
    /* Temporary storage for the add_record_len field */
  word order_pos;
    /* Position of order relative to start of buf_ptr */


  num_recs = 0;
  msg_pos = 0;
  order_pos = 0;

  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: Decoding an PC ORDER");

  int_ptr->pc_ord.ords[num_recs].gen.order =
      b_unpackb(buf_ptr, order_pos, FSIZ(cai_pc_gen_ord_type, order));
  order_pos += FSIZ( cai_pc_gen_ord_type, order );

  add_record_len = b_unpackb(buf_ptr, order_pos,
    FSIZ(cai_pc_gen_ord_type, add_record_len));
  order_pos += FSIZ(cai_pc_gen_ord_type, add_record_len);


  /* ------------------------------
  ** Fill in order dependent fields
  ** ------------------------------ */
  switch (int_ptr->pc_ord.ords[num_recs].gen.order)
  {
    case CAI_REG_ORD:
    {
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: Rxed CAI_REG_ORD");
      if (add_record_len >= 1)
      {
        /* ---------------------
        ** Parse order qualifier, this can be CAI_REG_REJ/CAI_REG_REQ/CAI_REG_ACK
        ** --------------------- */
        msg_pos = order_pos;
        int_ptr->pc_ord.ords[num_recs].ordq.ordq =
              b_unpackb(buf_ptr, msg_pos, FSIZ( cai_pc_ordq_type, ordq));
        msg_pos += FSIZ(cai_pc_ordq_type, ordq);

        /* --------------------------------------------
         ** Registration Accepted Order, ROAM_INDI included
         ** --------------------------------------------
         */
        if (int_ptr->pc_ord.ords[num_recs].ordq.ordq == CAI_REG_ACC)
        {
          int_ptr->pc_ord.ords[num_recs].regacc.roam_indi =
            b_unpackb(buf_ptr, msg_pos, FSIZ(cai_pc_reg_acc_type, roam_indi));
          msg_pos += FSIZ(cai_pc_reg_acc_type, roam_indi);
        }
      }
      else /* (add_record_len == 0) */
      {
        /* --------------------------------------------
         ** No order qualifier implicitly means ordq = 0
         ** (see PN-3118 Section 7.7.4)
         ** -------------------------------------------- */
        int_ptr->pc_ord.ords[num_recs].ordq.ordq = 0;
      }
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: Rxed PC ORDER ORDRQ = %d",
        int_ptr->pc_ord.ords[num_recs].ordq.ordq);
      /* -----------------------------------------
       ** Account for possible additional fields in
       ** future revs of the spec
       ** ----------------------------------------- */
      order_pos += (add_record_len * 8);
      break;
    } /* CAI_REG_ORD */

    case CAI_RELEASE_ORD:
    {
      if (add_record_len >= 1)
      {
        /* ---------------------
         ** Parse order qualifier
         ** --------------------- */
        int_ptr->pc_ord.ords[num_recs].ordq.ordq =
            b_unpackb(buf_ptr, order_pos, FSIZ(cai_pc_ordq_type, ordq));
      }
      else
      {
        /* --------------------------------------------
         ** No order qualifier implicitly means ordq = 0
         ** (see PN-3118 Section 7.7.4)
         ** -------------------------------------------- */
        int_ptr->pc_ord.ords[num_recs].ordq.ordq = 0;
      }
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: Rxed PC ORDER ORDR = %d",
        int_ptr->pc_ord.ords[num_recs].gen.order);
      /* -----------------------------------------
       ** Account for possible additional fields in
       ** future revs of the spec
       ** ----------------------------------------- */
      order_pos += (add_record_len * 8);
      break;
    } /* CAI_RELEASE_ORD */

    case CAI_REORDER_ORD:
    {
      /* --------------------------------------------
       ** No order qualifier implicitly means ordq = 0
       ** (see PN-3118 Section 7.7.4)
       ** -------------------------------------------- */
      int_ptr->pc_ord.ords[num_recs].ordq.ordq = 0;
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: Rxed PC ORDER ORDR = %d",
        int_ptr->pc_ord.ords[num_recs].gen.order);
      /* -----------------------------------------
       ** Account for possible additional fields in
       ** future revs of the spec
       ** ----------------------------------------- */
      order_pos += (add_record_len * 8);
      break;
    } /* CAI_REORDER_ORD */

    case CAI_BS_CHAL_CONF_ORD:
    {
      /* -----------------------------------------
       ** Base Station Challenge Confirmation Order
       ** ----------------------------------------- */
      if (add_record_len >= 4)
      {
        int_ptr->pc_ord.ords[num_recs].chal.ordq = b_unpackb(buf_ptr,
                                                             order_pos,
                                                             FSIZ(cai_pc_bs_chal_type, ordq));
        order_pos += FSIZ(cai_pc_bs_chal_type, ordq);

        int_ptr->pc_ord.ords[num_recs].chal.authbs = b_unpackd(buf_ptr,
                                                               order_pos,
                                                               FSIZ(cai_pc_bs_chal_type, authbs));
        order_pos += FSIZ(cai_pc_bs_chal_type, authbs) +
            FSIZ(cai_pc_bs_chal_type, reserved);

        if (add_record_len > 4)
        {
          /* -----------------------------------------
          ** Account for possible additional fields in
          ** future revs of the spec
          ** ----------------------------------------- */
          order_pos += ((add_record_len - 4) * 8);
        }
        num_recs++;
      }
      else
      {
        /* ----------------------------------------------------
        ** Invalid order length - something is very wrong here.
        ** Pretend like we didn't see it and let BS retransmit.
        ** ---------------------------------------------------- */
        order_pos += (add_record_len * 8);
      }
      break;
    } /* CAI_BS_CHAL_CONF_ORD */

    default:
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG: Rxed unsupported order = %d",
        int_ptr->pc_ord.ords[num_recs].gen.order);
      break;
    }
  } /* switch (int_ptr->pc_ord.ords[num_recs].gen.order) */

  return (CAIX_DONE_S);
} /* word gcsna_xlate_pc_ordm(..) */

/*===========================================================================

FUNCTION GCANA_XLATE_EXT_EXT_CHN_ASN

DESCRIPTION
  This function translates an ECAM  from external to internal
  format.

DEPENDENCIES
  Assumes that the internal message type received as an input to the
  procedure already includes the Extended Channel Assignment message type.

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/

word gcsna_xlate_ext_ext_chn_asn
(
  word len,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to received message to be translated */
  caii_rx_msg_type *int_ptr,
    /* Pointer to place where translated message should be placed */
  word chn,
    /* Physical Channel this message is received on */
  byte pd
    /* Protocol discriminator */
)
{
  word ext_msg_pos = 0;
  /* Position of channel assignment order relative to start of buf_ptr */
  word num_recs = 0;
  /* Number of variable parts of message */
  word i = 0;
  /* for loop iterator */
  byte *int_buf_ptr;
  /* Keep track of the starting point of the next packed order */

  /* ---------------------------------------------------------
   ** Initialize first record pointer to buf
   ** --------------------------------------------------------- */
  int_buf_ptr = int_ptr->ext_chnasn.buf;
  int_buf_ptr = (byte *)ALIGN_DWORD((dword)int_buf_ptr);
  int_ptr->ext_chnasn.chns[0] = (caii_ext_ca_ord_type*)int_buf_ptr;

  UNPACKB_G_PG(ext_chnasn.chns[num_recs]->gen.assign_mode,
               cai_ext_chnasn_base_type, assign_mode);

  /* Extract the reserved bits */
  ext_msg_pos += FSIZ(cai_ext_chnasn_base_type, reserved1);
  ext_msg_pos += FSIZ(cai_ext_chnasn_base_type, reserved2);

  if(int_ptr->ext_chnasn.chns[num_recs]->gen.assign_mode == CAI_PAGING_CHAN_ASSIGN)
  {
    #define MTYPE cai_ext_chnasn_am1_type
    UNPACKB_G_PG(ext_chnasn.chns[num_recs]->am1.respond, MTYPE, respond);
    UNPACKB_G_PG(ext_chnasn.chns[num_recs]->am1.freq_incl, MTYPE, freq_incl);

    if (int_ptr->ext_chnasn.chns[num_recs]->am1.freq_incl)
    {
      UNPACKB_G_PG(ext_chnasn.chns[num_recs]->am1.band_class, MTYPE, band_class);
      UNPACKW_G_PG(ext_chnasn.chns[num_recs]->am1.cdma_freq, MTYPE, cdma_freq);
    }

    UNPACKB_G_PG(ext_chnasn.chns[num_recs]->am1.num_pilots, MTYPE, num_pilots);

    for (i=0;
        (i < int_ptr->ext_chnasn.chns[num_recs]->am1.num_pilots+1) &&(i < CAI_EXT_CHNASN_AM1_MAX);
         i++)
    {
      UNPACKW_G_PG(ext_chnasn.chns[num_recs]->am1.pilot_pn[i], MTYPE, pilot_pn);
    }
    #undef MTYPE
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "DEBUG: Rxed invalid ECAM, rejecting");
    return CAIX_INV_MSG_S;
  }
  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: ECAM AM 1 Successfully parsed");
  return CAIX_DONE_S;
} /* gcsna_xlate_ext_ext_chn_asn */

/*===========================================================================
FUNCTION GCANA_XLATE_EXT_INFO

DESCRIPTION
  This function translates a Traffic Channel Alert With Information
  or Flash With Information Message from external to internal format.

DEPENDENCIES
  Assumes that the internal message type received as an input to the
  procedure already includes the Alert With Information or Flash With
  Information message type.

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None
===========================================================================*/
word gcsna_xlate_ext_info
(
  word len,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to received message to be translated */
  caii_rx_msg_type *int_ptr
    /* Pointer to place where translated message should be placed */
)
{
  word status = CAIX_DONE_S;
    /* Status returned to calling procedure */
  byte i = 0;
    /* Index */
  word msg_pos = 0;
    /* Position of record in message */
  byte *msg_ptr;
    /* Pointer to position inside internal message type */
  byte rec_type = 0 ;
    /* Temporary storage for record type */
  word rec_len = 0;
    /* Temporary storage for record length */
  word min_len = 0;
    /* Temp storage for message position */
  word start_msg_pos = 0;

  /* Initialize number of records to 0 */
  int_ptr->alert.num_recs = 0;

  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Xlating AWIM, Bundled AWIM len = %d",
    len);

  msg_ptr = (byte*)ALIGN_DWORD((dword)(&int_ptr->alert.recs[0]));
  min_len = FSIZ(cai_rec_hdr_type, record_type) +
            FSIZ(cai_rec_hdr_type, record_len);

  while ((len > msg_pos) && ((len - msg_pos) >= min_len) &&
         (status == CAIX_DONE_S))
  {
    /* ------------------------------
     ** Unpack message type and length
     ** ------------------------------ */

    ((caii_rec_hdr_type *) msg_ptr)->record_type = rec_type =
      b_unpackb( buf_ptr, msg_pos, FSIZ(cai_rec_hdr_type, record_type));
    msg_pos += FSIZ(cai_rec_hdr_type, record_type);

    /* -------------------------
     ** Get record length in bits
     ** ------------------------- */
    rec_len = b_unpackb(buf_ptr, msg_pos,
                        FSIZ(cai_rec_hdr_type, record_len)) * 8;
    msg_pos += FSIZ(cai_rec_hdr_type, record_len);

    start_msg_pos = msg_pos;

    if ((rec_len + msg_pos) > len)
    {
      /* ---------------------
       ** Record length invalid
       ** --------------------- */
      M1X_MSG( DCP, LEGACY_ERROR,
        "DEBUG: Invalid AWIM len, calculated len = %d, OTA len =%d ",
        (rec_len+msg_pos),
        len);
      status = CAIX_INV_LEN_S;
      break;
    }

    else
    {
      /* --------------------------------------
       ** Attempt to parse information record(s)
       ** -------------------------------------- */
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: decoding rec = %d",
        rec_type);
      switch (rec_type)
      {
        case CAI_DISPLAY_REC:
        {
          /* --------------------------
           * Display Information Record
           * -------------------------- */
          ((caii_display_rec_type *) msg_ptr)->fix.num_char =
            (byte) (rec_len / sizeof(cai_display_rec_var_type));

          for(i=0; i < ((caii_display_rec_type *) msg_ptr)->fix.num_char;i++)
          {
            ((caii_display_rec_type *) msg_ptr)->var[i] =
              b_unpackb(buf_ptr, start_msg_pos,
                        FSIZ(cai_display_rec_var_type, chari));
            start_msg_pos += FSIZ(cai_display_rec_var_type, chari);
          }

          /* -------------------------------------------------
           ** Increment internal message pointer to next record
           ** ------------------------------------------------- */
          msg_ptr += FENDPOS(caii_display_rec_type,
                             var[((caii_display_rec_type *) msg_ptr)->fix.num_char - 1]);

          msg_pos += rec_len;
          int_ptr->alert.num_recs++;
          break;
        } /* CAI_DISPLAY_REC */

        case CAI_CALLED_REC:
        {
          /* --------------------------------------
           * Called Party Number Information Record
           * -------------------------------------- */
          ((caii_called_rec_type *) msg_ptr)->fix.number_type =
             b_unpackb(buf_ptr, start_msg_pos,
                       FSIZ(cai_called_fix_type, number_type));
          start_msg_pos += FSIZ(cai_called_fix_type, number_type);

          ((caii_called_rec_type *) msg_ptr)->fix.number_plan =
             b_unpackb(buf_ptr, start_msg_pos,
                       FSIZ(cai_called_fix_type, number_plan));
          start_msg_pos += FSIZ(cai_called_fix_type, number_plan);

          ((caii_called_rec_type *) msg_ptr)->fix.num_char =
            (byte) ((rec_len - sizeof(cai_called_fix_type)) /
            sizeof(cai_called_var_type));

          for (i=0;i < ((caii_called_rec_type *) msg_ptr)->fix.num_char;i++)
          {
            ((caii_called_rec_type *) msg_ptr)->var[i] =
               b_unpackb(buf_ptr, start_msg_pos,
                         FSIZ(cai_called_var_type, chari));
            start_msg_pos += FSIZ(cai_called_var_type, chari);
          }

          /* ----------------------------------------------------------
           * Increment internal message pointer to point to next record
           * ---------------------------------------------------------- */
          msg_ptr += FENDPOS(caii_called_rec_type,
                             var[((caii_called_rec_type *) msg_ptr)->fix.num_char - 1] );

          msg_pos += rec_len;
          int_ptr->alert.num_recs++;
          break;
        } /* CAI_CALLED_REC */

        case CAI_CALLING_REC:
        case CAI_CONNECT_REC:
        {
          /* -----------------------------------------------------------
           * Calling Party Number or Connected Number Information Record
           * ----------------------------------------------------------- */
          ((caii_calling_rec_type *) msg_ptr)->fix.number_type =
            b_unpackb(buf_ptr, start_msg_pos,
                      FSIZ(cai_calling_fix_type, number_type));
          start_msg_pos += FSIZ(cai_calling_fix_type, number_type);

          ((caii_calling_rec_type *) msg_ptr)->fix.number_plan =
            b_unpackb(buf_ptr, start_msg_pos,
                      FSIZ(cai_calling_fix_type, number_plan));
          start_msg_pos += FSIZ(cai_calling_fix_type, number_plan);

          ((caii_calling_rec_type *) msg_ptr)->fix.pi =
            b_unpackb(buf_ptr, start_msg_pos, FSIZ(cai_calling_fix_type, pi));
          start_msg_pos += FSIZ(cai_calling_fix_type, pi);

          ((caii_calling_rec_type *) msg_ptr)->fix.si =
            b_unpackb(buf_ptr, start_msg_pos, FSIZ(cai_calling_fix_type, si));
          start_msg_pos += FSIZ(cai_calling_fix_type, si);

          ((caii_calling_rec_type *) msg_ptr)->fix.num_char =
            (byte) ((rec_len - sizeof(cai_calling_fix_type)) /
          sizeof(cai_calling_var_type));

          for (i=0; i < ((caii_calling_rec_type *) msg_ptr)->fix.num_char; i++)
          {
            ((caii_calling_rec_type *) msg_ptr)->var[i] =
               b_unpackb(buf_ptr, start_msg_pos,
                         FSIZ(cai_calling_var_type, chari));
            start_msg_pos += FSIZ(cai_calling_var_type, chari);
          }

          /* -------------------------------
           * Update internal message pointer
           * ------------------------------- */
          msg_ptr += FENDPOS(caii_calling_rec_type,
            var[((caii_calling_rec_type *) msg_ptr)->fix.num_char - 1]);

          msg_pos += rec_len;
          int_ptr->alert.num_recs++;
          break;
        } /* CAI_CALLING_REC || CAI_CONNECT_REC */

        case CAI_SIGNAL_REC:
        {
          /* -------------------------
           * Signal Information Record
           * ------------------------- */
          ((caii_signal_rec_type *) msg_ptr)->signal_type =
             b_unpackb(buf_ptr, start_msg_pos,
                       FSIZ(cai_signal_rec_type, signal_type));
          start_msg_pos += FSIZ(cai_signal_rec_type, signal_type);

          ((caii_signal_rec_type *) msg_ptr)->alert_pitch =
            b_unpackb(buf_ptr, start_msg_pos,
          FSIZ(cai_signal_rec_type, alert_pitch));
          start_msg_pos += FSIZ(cai_signal_rec_type, alert_pitch);

          ((caii_signal_rec_type *) msg_ptr)->signal =
            b_unpackb(buf_ptr, start_msg_pos,
                      FSIZ(cai_signal_rec_type, signal));
          start_msg_pos += FSIZ(cai_signal_rec_type, signal) +
                           FSIZ(cai_signal_rec_type, reserved);

          /* -------------------------------
           * Update internal message pointer
           * ------------------------------- */
          msg_ptr += sizeof(caii_signal_rec_type);
          msg_pos += rec_len;
          int_ptr->alert.num_recs++;
          break;
        } /* CAI_SIGNAL_REC */

        case CAI_MSG_WAITING_REC:
        {
          /* ----------------------------------
           * Message Waiting Information Record
           * ---------------------------------- */
          ((caii_msg_waiting_rec_type *) msg_ptr)->msg_count =
            b_unpackb(buf_ptr, start_msg_pos,
          FSIZ(cai_msg_waiting_rec_type, msg_count));
          start_msg_pos += FSIZ(cai_msg_waiting_rec_type, msg_count);

          /* -------------------------------
           * Update internal message pointer
           * ------------------------------- */
          msg_ptr += sizeof(caii_msg_waiting_rec_type);
          msg_pos += rec_len;
          int_ptr->alert.num_recs++;
          break;
        } /* CAI_MSG_WAITING_REC */

        case CAI_CALLED_SUB_REC:
        case CAI_CALLING_SUB_REC:
        case CAI_REDIR_SUB_REC:
        case CAI_CONNECT_SUB_REC:
        {
          /* --------------------------------------------------------
           * Calling Party Subaddress or Called Party Subaddress
           * or Redirecting Subaddress or Connected Subaddress info
           * record
           */
          /* Unpack Information Record */
          status = xlate_ext_sub_rec(rec_len, buf_ptr, msg_ptr,
                                     &start_msg_pos);

          /* -------------------------------------------------
           * Increment internal message pointer to next record
           * ------------------------------------------------- */
          msg_ptr += FENDPOS(caii_subaddr_rec_type,
                     var[((caii_subaddr_rec_type *) msg_ptr)->fix.num_char - 1]);

          msg_pos += rec_len;
          int_ptr->alert.num_recs++;
          break;
        } /* CAI_CALLED_SUB_REC */

        case CAI_REDIR_NUM_REC:
        {
          /* --------------------------------------------------------
           * Redirecting Number Information Record
           * -------------------------------------------------------- */
          /* Unpack Information Record */
          status = xlate_ext_redir_num_rec(rec_len, buf_ptr, msg_ptr,
                                           &start_msg_pos);

          /* -------------------------------------------------
           * Increment internal message pointer to next record
           * ------------------------------------------------- */
          msg_ptr += FENDPOS(caii_redir_rec_type,
            var[((caii_redir_rec_type *) msg_ptr)->fix.num_char - 1]);
          msg_pos += rec_len;
          int_ptr->alert.num_recs++;
          break;
        } /* CAI_REDIR_NUM_REC */

        case CAI_EXT_DISPLAY_REC:
        {
          /* ------------------------------
           * Extended Display Info Record
           * ------------------------------ */
          status = xlate_ext_ext_disp_rec(rec_len, buf_ptr, &msg_ptr,
                                          &start_msg_pos);

          /* -------------------------------
           * xlate function already updated
           * internal message pointer
           * ------------------------------- */

          msg_pos += rec_len;
          int_ptr->alert.num_recs++;
          break;
        } /* CAI_EXT_DISPLAY_REC */

        case CAI_METER_PULSES_REC:
        {
          /* --------------------------------
           * Meter Pulses Information Record
           * -------------------------------- */
          ((caii_meter_pulses_rec_type *) msg_ptr)->pulse_freq =
          b_unpackw(buf_ptr, start_msg_pos,
                    FSIZ(cai_meter_pulses_rec_type, pulse_freq));
          start_msg_pos += FSIZ(cai_meter_pulses_rec_type, pulse_freq);

          ((caii_meter_pulses_rec_type *) msg_ptr)->on_time =
          b_unpackb(buf_ptr, start_msg_pos,
                    FSIZ(cai_meter_pulses_rec_type, on_time));
          start_msg_pos += FSIZ(cai_meter_pulses_rec_type, on_time);

          ((caii_meter_pulses_rec_type *) msg_ptr)->off_time =
          b_unpackb(buf_ptr, start_msg_pos,
                    FSIZ(cai_meter_pulses_rec_type, off_time));
          start_msg_pos += FSIZ(cai_meter_pulses_rec_type, off_time);

          ((caii_meter_pulses_rec_type *) msg_ptr)->pulse_cnt =
          b_unpackb(buf_ptr, start_msg_pos,
                    FSIZ(cai_meter_pulses_rec_type, pulse_cnt));
          start_msg_pos += FSIZ(cai_meter_pulses_rec_type, pulse_cnt);

          /* Increment past reserved bit at end */
          start_msg_pos += FSIZ(cai_meter_pulses_rec_type, reserved);

          /* -------------------------------
           ** Update internal message pointer
           ** ------------------------------- */
          msg_ptr += sizeof(caii_meter_pulses_rec_type); /*lint !e662 */

          msg_pos += rec_len;
          int_ptr->alert.num_recs++;
          break;
        } /* CAI_METER_PULSES_REC */

        case CAI_PARA_ALERT_REC:
  {
          /* --------------------------------------
           ** Parametric Alerting Information Record
           ** -------------------------------------- */
          status = xlate_ext_para_alert_rec(rec_len, buf_ptr, msg_ptr,
                                            &start_msg_pos);

          /* -------------------------------
           ** Update internal message pointer
           ** ------------------------------- */
          if (((caii_para_alert_rec_type *) msg_ptr)->fix.num_groups == 0)
          {
            msg_ptr += FENDPOS(caii_para_alert_rec_type,fix);
          }
          else
          {
            msg_ptr += FENDPOS( caii_para_alert_rec_type,
            var[((caii_para_alert_rec_type *) msg_ptr)->fix.num_groups - 1] );
          }

          msg_pos += rec_len;
          int_ptr->alert.num_recs++;
          break;
        } /* CAI_PARA_ALERT_REC */

        case CAI_LINE_CTRL_REC:
        {
          /* -------------------------------------
           ** Line Control Information Record
           ** ------------------------------------- */
          ((caii_line_ctrl_rec_type *) msg_ptr)->pol_inc =
            b_unpackb(buf_ptr, start_msg_pos,
          FSIZ(cai_line_ctrl_rec_type, pol_inc));
          start_msg_pos += FSIZ(cai_line_ctrl_rec_type, pol_inc);

          if (((caii_line_ctrl_rec_type *) msg_ptr)->pol_inc)
          {
            ((caii_line_ctrl_rec_type *) msg_ptr)->tgl_mod =
              b_unpackb(buf_ptr, start_msg_pos,
                         FSIZ(cai_line_ctrl_rec_type, tgl_mod));
            start_msg_pos += FSIZ(cai_line_ctrl_rec_type, tgl_mod);

            ((caii_line_ctrl_rec_type *) msg_ptr)->rev_pol =
              b_unpackb(buf_ptr, start_msg_pos,
            FSIZ(cai_line_ctrl_rec_type, rev_pol));
            start_msg_pos += FSIZ(cai_line_ctrl_rec_type, rev_pol);

            ((caii_line_ctrl_rec_type *) msg_ptr)->pow_dtm =
              b_unpackb(buf_ptr, start_msg_pos,
                        FSIZ(cai_line_ctrl_rec_type, pow_dtm));
            start_msg_pos += FSIZ( cai_line_ctrl_rec_type, pow_dtm);
          }
          else
          {
            ((caii_line_ctrl_rec_type *) msg_ptr)->pow_dtm =
              b_unpackb(buf_ptr, start_msg_pos,
            FSIZ(cai_line_ctrl_rec_type, pow_dtm));
            start_msg_pos += FSIZ(cai_line_ctrl_rec_type, pow_dtm);
          }

          /* -------------------------------
           ** Update internal message pointer
           ** ------------------------------- */
          msg_ptr += sizeof(caii_line_ctrl_rec_type); /*lint !e662 */

          msg_pos += rec_len;
          int_ptr->alert.num_recs++;
          break;
        } /* CAI_LINE_CTRL_REC */

        case CAI_USER_ZONE_UPDATE_REC:
        {
          ((caii_user_zone_update_type *) msg_ptr)->uzid =
            b_unpackw(buf_ptr, start_msg_pos,
          FSIZ(cai_user_zone_update_type, uzid));
          start_msg_pos += FSIZ(cai_user_zone_update_type, uzid);

          msg_ptr += FENDPOS( caii_user_zone_update_type,uzid );
          msg_pos += rec_len;
          int_ptr->alert.num_recs++;
          break;
        } /* CAI_USER_ZONE_UPDATE_REC */

        case CAI_USER_ZONE_REJECT_REC:
        {
          ((caii_user_zone_reject_type *) msg_ptr)->reject_uzid =
            b_unpackw(buf_ptr, start_msg_pos,
          FSIZ(cai_user_zone_reject_type, reject_uzid));
          start_msg_pos += FSIZ(cai_user_zone_reject_type, reject_uzid);

          ((caii_user_zone_reject_type *) msg_ptr)->reject_action_indi =
            b_unpackb(buf_ptr, start_msg_pos,
          FSIZ(cai_user_zone_reject_type, reject_action_indi));
          start_msg_pos += FSIZ(cai_user_zone_reject_type, reject_action_indi);

          ((caii_user_zone_reject_type *) msg_ptr)->uzid_assign_incl =
            b_unpackb(buf_ptr, start_msg_pos,
          FSIZ(cai_user_zone_reject_type, uzid_assign_incl));
            start_msg_pos += FSIZ(cai_user_zone_reject_type, uzid_assign_incl);

          if (((caii_user_zone_reject_type *) msg_ptr)->uzid_assign_incl)
          {
            ((caii_user_zone_reject_type *) msg_ptr)->assign_uzid =
            b_unpackw(buf_ptr, start_msg_pos,
                      FSIZ(cai_user_zone_reject_type, assign_uzid));
            start_msg_pos += FSIZ(cai_user_zone_reject_type, assign_uzid);
            msg_ptr += FENDPOS( caii_user_zone_reject_type, assign_uzid );
          }
          else
          {
            msg_ptr += FENDPOS(caii_user_zone_reject_type,
                               uzid_assign_incl );
          }

          msg_pos += rec_len;
          int_ptr->alert.num_recs++;
          break;
        } /* CAI_USER_ZONE_REJECT_REC */
#ifdef FEATURE_IS2000_REL_A
        case CAI_MC_EXT_DISP_REC:
        {
          /* ---------------------------------------------
           ** Multi-character Extended Display Info Record
           ** --------------------------------------------- */
          status = xlate_ext_mc_ext_disp_rec(rec_len, buf_ptr, &msg_ptr,
                                             &start_msg_pos);

          /* -------------------------------
           ** xlate function already updated
           ** internal message pointer
           ** ------------------------------- */
          msg_pos += rec_len;
          int_ptr->alert.num_recs++;
          break;
        } /* CAI_MC_EXT_DISP_REC */

        case CAI_CALL_WAIT_IND_REC:
        {
          /* ----------------------------------
           ** Call Waiting Indicator Record
           ** ---------------------------------- */
          ((caii_call_waiting_ind_type *) msg_ptr)->call_waiting_ind =
            b_unpackb(buf_ptr, start_msg_pos,
                      FSIZ(cai_call_wait_type, call_waiting_ind));
          start_msg_pos += (FSIZ(cai_call_wait_type, call_waiting_ind) +
                            FSIZ(cai_call_wait_type, reserved));

          /* -------------------------------
           ** Update internal message pointer
           ** ------------------------------- */
          msg_ptr += sizeof(caii_call_waiting_ind_type);
          msg_pos += rec_len;
          int_ptr->alert.num_recs++;
          break;
        } /* CAI_CALL_WAIT_IND_REC */
#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_REL_C
        case CAI_ENH_MC_EXT_DISP_REC:
        {
          /* ---------------------------------------------
           ** Enhanced Multi-character Extended Display Info Record
           ** --------------------------------------------- */
          status = xlate_ext_enh_mc_ext_disp_rec(rec_len, buf_ptr, &msg_ptr,
                                                 &start_msg_pos);

          /* -------------------------------
           ** xlate function already updated
           ** internal message pointer
           ** ------------------------------- */
          msg_pos += rec_len;
          int_ptr->alert.num_recs++;
          break;
        } /* CAI_ENH_MC_EXT_DISP_REC */
#endif /* FEATURE_IS2000_REL_C */

        /* This is an intentional fall through. If feature is not defined
           or if the mode is not JCDMA the code will fall through into
           default */
        /*lint -fallthrough */
        default:
        {
          /* -------------------
           * Unknown record type
           * ------------------- */
          msg_ptr += sizeof( caii_rec_hdr_type );
          msg_pos += rec_len;
          int_ptr->alert.num_recs++;
          break;
        }
      } /* end switch (rec_type) */
      msg_ptr = (byte*)ALIGN_DWORD(((dword) msg_ptr));
    } /* else if ((rec_len + msg_pos) > len) */
  } /* end of while() */

  return (status);
} /* word gcsna_xlate_ext_info */

/*===========================================================================
FUNCTION  MCC_GCSNA_XLATE_L3_MSG

DESCRIPTION
  This function xlates the byte stream received in DL info transfer message or MO from EUTRA
  The byte stream received has l3 message embedded in it and we parse the l3 message as per
  C.S0005. buf_ptr is the pointer to the byte stream that needs to be parsed.

DEPENDENCIES
  None.

RETURN VALUE
  mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg is the parsed structure from buf_ptr.

SIDE EFFECTS
  None.
===========================================================================*/

void mcc_gcsna_xlate_l3_msg(byte *buf_ptr, word msg_size)
{
  word status = CAIX_DONE_S;
  byte pd; /* Protocol discriminator */

  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: Xlating tunneled l3 msg, rxed on virtual CAI Chn = %d",
    mcc_gcsna_ext_msg_ptr->ckt_srv_msg.cdma_chn);

  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Rxed msg = %d on tunnel",
    mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_type);

  /* xlate the l3 msg */
  switch( ( gcsna_chan_type )mcc_gcsna_ext_msg_ptr->ckt_srv_msg.cdma_chn)
  {
    case GCSNA_CHN_PC:
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG: rxed msg on PCH");
      if(msg_size > CAI_PC_BODY_SIZE)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "DEBUG: Rxed msg size is greater than CAI_PC_BODY_SIZE, abort");
        status = CAIX_INV_LEN_S;
      }
      else
      {
        /* Message len is fine, switch message rxed over Tunnel */
        switch(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_type)
        {
          case CAI_GENERAL_PAGE_MSG:
          {
            M1X_MSG( DCP, LEGACY_MED,
              "DEBUG:Rxed CAI_GENERAL_PAGE_MSG");
            status = xlate_ext_general_page(msg_size, buf_ptr, &(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg));
            break;
          } /* CAI_GENERAL_PAGE_MSG */

          case CAI_PC_ORD_MSG:
          {
            M1X_MSG( DCP, LEGACY_MED,
              "DEBUG:Rxed CAI_PC_ORD_MSG");
            status = gcsna_xlate_pc_ordm(msg_size, buf_ptr, &(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg));
            break;
          } /* CAI_PC_ORD_MSG */

           case CAI_EXT_CHN_ASN_MSG:
          {
            M1X_MSG( DCP, LEGACY_MED,
              "DEBUG:Rxed CAI_EXT_CHN_ASN_MSG");
             /* Unpack PD from msg_type */
            UNPACK_PD(pd,(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_type));
            status = gcsna_xlate_ext_ext_chn_asn(msg_size, buf_ptr, &(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg),CAIX_PC,pd);
            break;
          } /* CAI_EXT_CHN_ASN_MSG */

          case CAI_PC_SSD_UP_MSG:
          {
            /* ------------------
            ** SSD Update Message
            ** ------------------ */
            UNPACK_PD(pd,(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_type));
            status = xlate_ext_pc_ssd(msg_size, buf_ptr, &(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg), CAIX_PC, pd);
            /* Fill in the msg type */
            mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.pc_ssd.msg_type = mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_type;
            M1X_MSG( DCP, LEGACY_MED,
              "DEBUG: Parse SSD Update Message");
            break;
          } /* CAI_PC_SSD_UP_MSG */

          case CAI_PC_AUTH_MSG:
          {
            /* ----------------------
             ** Auth Challenge Message
             ** ---------------------- */
            M1X_MSG( DCP, LEGACY_MED,
              " DEBUG: Rxed Auth Challenge Message");
            UNPACK_PD(pd,(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_type));
            status = xlate_ext_pc_auth(msg_size, buf_ptr, &(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg), CAIX_PC, pd);
            M1X_MSG( DCP, LEGACY_MED,
              "DEBUG: Parse Auth Challenge Message");
            break;
          } /* CAI_PC_AUTH_MSG */

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
          case CAI_PC_BURST_MSG:
          {
            /* ----------------------
             ** Data Burst Message
             ** ---------------------- */
            M1X_MSG( DCP, LEGACY_MED,
              " DEBUG: Rxed Data Burst Message");
            UNPACK_PD(pd,(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_type));

            status = xlate_ext_pc_burst(msg_size, buf_ptr, &(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg), CAIX_PC, pd);
            M1X_MSG( DCP, LEGACY_MED,
              "DEBUG: Parse Data Burst Message");
            break;
          }
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */
          default:
          {
            M1X_MSG( DCP, LEGACY_HIGH,
              "DEBUG:Rxed illigal msg over Tunnel msg_id = %d ",
              mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_type);
            status = CAIX_INV_CHN_S;
            break;
          }
        } /* mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_type */
      }
      break;
    } /* GCSNA_CHN_PC */

    case GCSNA_CHN_TC:
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG: rxed msg on TCH");
      if(msg_size > CAI_FWD_TC_MSG_BODY_SIZE)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "DEBUG: Rxed msg size is greater than CAI_FWD_TC_MSG_BODY_SIZE, abort");
        status = CAIX_INV_LEN_S;
      }
      else
      {
        /* Message len is fine, switch message rxed over Tunnel */

        /* Over Tunnel, UHDM/AWIM is the only msg which could be rxed over TCH.
        * For any other TC message, ignore it.
        */
        switch(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_type)
        {
          case CAI_EXT_HO_DIR_MSG:
          case CAI_GEN_HO_DIR_MSG:
          case CAI_UNIV_HO_DIR_MSG:

          case CAI_MEID_UNIV_HO_DIR_MSG:

          {
            M1X_MSG( DCP, LEGACY_MED,
              "DEBUG:Rxed HO_DIR_MSG");
            /* Convert the external format UHDM to internal format. */
            mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.msg_type = mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_type;

            status = xlate_ext_ho_msg( msg_size, buf_ptr, &(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg) );
            M1X_MSG( DCP, LEGACY_MED,
              "RX HO type %d, hdm_seq %d #pilots %d",
              mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.gen_tc.msg_type,
              mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.hdm_seq,
              mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.num_pilots );
            break;
          } /* HO_DIR_MSG */

           case CAI_ALERT_MSG:
          {
            M1X_MSG( DCP, LEGACY_MED,
              "DEBUG:Rxed CAI_ALERT_MSG");
            /* Convert the external format AWIM to internal format. */
            mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.alert.msg_type = CAI_ALERT_MSG;
            status = gcsna_xlate_ext_info( msg_size, buf_ptr, &(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg) );
            M1X_MSG( DCP, LEGACY_HIGH,
              "DEBUG:Status after parsing = %d",
              status);
            break;
          } /* CAI_ALERT_MSG */

          default:
          {
            M1X_MSG( DCP, LEGACY_HIGH,
              "DEBUG:Rxed illigal msg over Tunnel msg_id = %d ",
              mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_type);
            status = CAIX_INV_CHN_S;
            break;
          }
        } /* switch(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_type) */
      }
      break;
    } /* GCSNA_CHN_TC */
    default:
      M1X_MSG( DCP, LEGACY_ERROR,
        "DEBUG: Illigal channel");
      status = CAIX_INV_CHN_S;
      break;
  } /* switch(mcc_gcsna_ext_msg_ptr->gen_msg_id.cdma_chn) */

  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG:status after xlating = %d",
    status);

  /* based on the status return the l3_msg flag */
  switch(status)
  {
    case CAIX_DONE_S:
    {
      mcc_gcsna_ext_msg_ptr->ckt_srv_msg.is_l3_msg = MCC_CSFB_L3_MSG_RXED;
      break;
    } /* CAIX_DONE_S */

    case CAIX_INV_LEN_S:
    case CAIX_INV_MSG_S:
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG:Rxed Malformed msg=%d",
        mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_type);
      mcc_gcsna_ext_msg_ptr->ckt_srv_msg.is_l3_msg = MCC_CSFB_L3_MSG_RXED_INCORRECT;

      /* Indicate Upper layers (CM) of error through RRC and wait for Call Release,
       * if the current access is for ORIG/PRM. It could be becaue of receiving
       *  malformed UHDM.
       */
      if( (cdma.curr_state == CDMA_CSFB_ACCESS) &&
          ( (mcc_csfb_access_reason == MCC_CSFB_ACCESS_ORIG) ||
            (mcc_csfb_access_reason == MCC_CSFB_ACCESS_PRM)
          ) &&
          ( (mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_type == CAI_UNIV_HO_DIR_MSG)

            ||(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_type == CAI_MEID_UNIV_HO_DIR_MSG)

          )
        )
     {
       /* Send report only when there is malformed UHDM. Should there be any
        * other malformed message, dont send a report.
        */
       M1X_MSG( DCP, LEGACY_HIGH,
         "DEBUG: Deactivating the lower layers");
       mcc_csfb_deactivate_lower_layers();
       
#ifdef FEATURE_MODEM_1X_SRVCC
       /* SRVCC Mode */
       if(mcc_csfb_get_srvcc_mode() == TRUE)
       {
         /* Send SRVCC HO_FAIL indication to LTE-RRC */
         if(csfb_cdma.call_status == CSFB_CALL_HO_RXED)
         {
           mcc_send_lterrc_srvcc_ho_fail_rsp(
                       LTE_IRAT_1XSRVCC_HO_FAILURE_MSG_INVAILD);
         }
         
         /* Send SRVCC HO_FAIL Indication to CM and 
          * Wait for "CALL_CLEAR_F" cmd from CM */
         mcc_send_cm_srvcc_ho_fail_ind();
       } /* SRVCC Mode */
       else
#endif /* FEATURE_MODEM_1X_SRVCC */
       { /* CSFB Mode */
         onex_cp_lte_1xcsfb_ho_fail_rsp_t ho_fail_rsp;

         /* Now send the failure to upper layers */
         /* Fill in the cause as per the ORDQ received */
         ho_fail_rsp.ho_fail_cause = LTE_IRAT_1XCSFB_HO_FAILURE_MSG_INVAILD;
         /* Send the message over MSGR to LTE_RRC */
         mc_msgr_send_msg(&ho_fail_rsp.hdr,
                          ONEX_CP_LTE_1xCSFB_HO_FAILED_RSP,
                          sizeof(ho_fail_rsp));
       } /* CSFB Mode */
     }

      break;
    } /* CAIX_INV_LEN_S or CAIX_INV_MSG_S */

    default:
    {
      mcc_gcsna_ext_msg_ptr->ckt_srv_msg.is_l3_msg = MCC_CSFB_L3_MSG_NOT_RXED;
      break;
    } /* default */

  } /*  switch(status) */

  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: Done Xlating l3 msg, l3 msg status = %d",
    mcc_gcsna_ext_msg_ptr->ckt_srv_msg.is_l3_msg);

} /* mcc_gcsna_xlate_l3_msg */

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
boolean mcc_gcsna_do_gcsna_option_chk(byte msg_type)
{
  /* Variable to track num of alt so included */
  int num_alt_so = 0;

  /* Variable to return status */
  boolean status = FALSE;

  /* Variable to remember GCSNA Option */
  byte gcsna_opt_rxed;

  /* Boolean to store SRVCC Mode */
  boolean srvcc_mode = mcc_csfb_get_srvcc_mode();

  M1X_MSG( DCP, LEGACY_MED,
        "DEBUG:GCSNA Opt Chk for msg:%d, Srvcc Mode:%d, gcsna_opt_inuse:%d",
           msg_type, 
           srvcc_mode,
           gcsna_option_inuse );

  /* Perform GCSNA Option check, act as following:
   *  o GCSNA_OPTION_0: BS supports only Rel 8, where as we support Rel9+Rel9
   *    Since we are Rel8 compatible, change gcsna_option_inuse to Rel8 and
   *    hence forth use it in all 1X messages.
   *  o GCSNA_OPTION_1: BS supports Rel9, which we also support. set
   *    gcsna_option_inuse to Rel9.
   *  o GCSNA_OPTION_2: BS supports SR-VCC, see if we can support alt_gcsna_options
   *    else send GCSNA Rejet.
   *
   *  o SRVCC Mode: We need to send GCSNA Rej msg
   *    if we rcvd GCSNA Option != GCSNA_OPTION_2
   */

  if(msg_type == GCSNA_1X_CRKT_SRV_MSG)
  {
    gcsna_opt_rxed = mcc_gcsna_ext_msg_ptr->ckt_srv_msg.gcsna_opt;
  }
  else
  {
    gcsna_opt_rxed = mcc_gcsna_ext_msg_ptr->gcsna_rej.gcsna_opt;
  }

  switch(gcsna_opt_rxed)
  {
    case GCSNA_OPTION_0:
    {
#ifdef FEATURE_MODEM_1X_SRVCC
      /* Update GCSNA option for CSFB Mode only */
      if(srvcc_mode == FALSE)
#endif /* FEATURE_MODEM_1X_SRVCC */
      {
        gcsna_option_inuse = GCSNA_OPTION_0;
        status = TRUE;
      } /* CSFB Mode */
      break;
    } /* GCSNA_OPTION_0 */

    case GCSNA_OPTION_1:
    {
#ifdef FEATURE_MODEM_1X_SRVCC
      /* Update GCSNA option for CSFB Mode only */
      if(srvcc_mode == FALSE)
#endif /* FEATURE_MODEM_1X_SRVCC */
      {
        gcsna_option_inuse = GCSNA_OPTION_1;
        status = TRUE;
      } /* CSFB Mode */
      break;
    } /* GCSNA_OPTION_1 */

    case GCSNA_OPTION_2:
    {
#ifdef FEATURE_MODEM_1X_SRVCC
      /* Update GCSNA option = 2 only for non-SRVCC Mode */
      if(srvcc_mode == FALSE)
#endif /* FEATURE_MODEM_1X_SRVCC */
      { /* CSFB Mode */
        /* check if there are any alt_srv_options */
        if( (msg_type == GCSNA_SRV_REJ_MSG) ||
            (!mcc_gcsna_ext_msg_ptr->ckt_srv_msg.alt_gcsna_opt_incl)
          )
        {
          /* Send GCSNA Reject, reset gcsna_option_inuse to what we support */
          gcsna_option_inuse = GCSNA_OPTION_1;
          status = FALSE;
        }
        else
        {
          status = FALSE;
          /* There is GCSNA Alternate Srv Option included , check if we match
           * any of those.
           */
          for(num_alt_so = 0;
              num_alt_so < mcc_gcsna_ext_msg_ptr->ckt_srv_msg.num_alt_gcsna_opt;
              num_alt_so++)
          {
            if( (mcc_gcsna_ext_msg_ptr->ckt_srv_msg.alt_gcsna_options[num_alt_so].alt_gcsna_opt == GCSNA_OPTION_0) ||
                (mcc_gcsna_ext_msg_ptr->ckt_srv_msg.alt_gcsna_options[num_alt_so].alt_gcsna_opt == GCSNA_OPTION_1)
              )
            {
              /* We found the GCSNA option, set gcsna_option_inuse accordingly */
              gcsna_option_inuse = mcc_gcsna_ext_msg_ptr->ckt_srv_msg.alt_gcsna_options[num_alt_so].alt_gcsna_opt;
              status = TRUE;
              break;
            }
          } /* for(num_alt_so = 0 */
        } /* !if( (msg_type == GCSNA_SRV_REJ_MSG) */
      } /* CSFB Mode */
      break;
    } /* GCSNA_OPTION_2 */

    case GCSNA_OPTION_8:
    {
#ifdef FEATURE_MODEM_1X_SRVCC
      /* Update GCSNA_OPTION_8 for SRVCC Mode only */
      if(srvcc_mode == TRUE)
      {
        gcsna_option_inuse = GCSNA_OPTION_8;
        status = TRUE;
      } /* SRVCC Mode */
      else
#endif /* FEATURE_MODEM_1X_SRVCC */
      {
        M1X_MSG( DCP, LEGACY_ERROR,
                 "Illegal GCSNA option 8 in CSFB mode" );
        status = FALSE;
      }
      break;
    } /* GCSNA_OPTION_8 */
    
    default:
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Illegal GCSNA option");
      status = FALSE;
      break;
    } /* default */

  } /* end of switch(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.gcsna_opt) */

  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: status = %d , gcsna_opt_rxed = %d, gcsna_opt_inuse = %d",
    status,
    gcsna_opt_rxed,
    gcsna_option_inuse);

  return status;

} /* mcc_gcsna_do_gcsna_option_chk */

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
boolean mcc_gcsna_is_valid_option_inuse(void)
{
  /* Boolean to store SRVCC Mode */
  boolean srvcc_mode = mcc_csfb_get_srvcc_mode();
              
  M1X_MSG( DCP, LEGACY_MED,
           "GCSNA option in use:%d, SRVCC Mode:%d",
           gcsna_option_inuse, srvcc_mode );
  /* Return TRUE for below cases
   * 1. CSFB Mode and GCSNA Option Inuse = GCSNA_OPTION_1
   * 2. SRVCC Mode and GCSNA Option Inuse = GCSNA_OPTION_8
   */
  if( ( (!srvcc_mode) && (gcsna_option_inuse == GCSNA_OPTION_1) ) 
      ||
      ( (srvcc_mode) && (gcsna_option_inuse == GCSNA_OPTION_8) )
    )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }  
} /* mcc_gcsna_is_valid_option_inuse() */

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
boolean mcc_gcsna_do_gcsna_1x_prev_chk(byte msg_type)
{
  /* Variable to remember GCSNA Reject msg 1xProtocolRevision */
  byte gcsna_1x_prev_rxed;

  /* Variable to return status */
  boolean status = TRUE;

  if(msg_type == GCSNA_1X_CRKT_SRV_MSG)
  {
    gcsna_1x_prev_rxed = mcc_gcsna_ext_msg_ptr->ckt_srv_msg.prev_1x;
  }
  else
  {
    gcsna_1x_prev_rxed = mcc_gcsna_ext_msg_ptr->gcsna_rej.prev_1x;
  }

  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG:GCSNA 1XPrev rxed:%d,MOB CAI REV:%d,cdma prev in use:%d",
    gcsna_1x_prev_rxed,
    cdma.mob_cai_rev,
    cdma.p_rev_in_use);

  /* If gcsna rej msg 1x protocol rev > mob_cai_rev
   * then return FALSE
     * The prev received in GCSNA will be processed as per following rules:
     * For DL Messages (sent by IWS)
     * * GCSNA 1xProtocolRevision will be set by the IWS to P_REV_IN_USE
     *   or MIN_P_REV (same as C2K Parameters) if P_REV_IN_USE is not known.
     * * Mobile shall accept any message <= MOB_PREV. A GSCSNA Reject
     *   shall be sent if a message includes 1xProtocolRevision > MOB_PREV.
     *   (This includes the Mobility from EUTRA case. )
     * For UL messages (rxed by IWS)
     * * Mobile will set GCSNA 1xProtocolRevision  to P_REV_IN _USE
     * * IWS will accept a message greater than MIN_P_REV and less than
     *   or equal to P_REV.
   */
  if( gcsna_1x_prev_rxed > cdma.mob_cai_rev )
  {
    status = FALSE;
    M1X_MSG( DCP, LEGACY_ERROR,
      "DEBUG: status:%d, Invalid GCSNA 1XPrev rxed:%d, MOB CAI REV:%d",
      status,
      gcsna_1x_prev_rxed,
      cdma.mob_cai_rev);
    return status;
  }

  /* Update cdma prev inuse with gcsna rej prev rxed */
  if( (msg_type == GCSNA_SRV_REJ_MSG) &&
      (cdma.p_rev_in_use != gcsna_1x_prev_rxed)
    )
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "CSFB: status:%d, updating prev_in_use:%d to GCSNA 1XPrev rxed:%d",
      status,
      cdma.p_rev_in_use,
      gcsna_1x_prev_rxed);
    /* Update our p-rev-in-use. The p-rev-in-use will be used later in processing
     * of  messages like tunneled UHDM. Note that while operating in tunnel mode,
     * p-rev-in-use is expected to be <=9 (REL C).
     * mcc_update_bs_p_rev() will make sure to update DB_BS_P_REV so that next
     * time if PREV changes in C2K Params then p_rev_in_use updated properly
     */
    mcc_update_bs_p_rev(gcsna_1x_prev_rxed);
  }

  return status;
} /* mcc_gcsna_do_gcsna_1x_prev_chk() */

#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */


