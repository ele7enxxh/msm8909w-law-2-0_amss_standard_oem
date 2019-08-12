#ifndef CAIX_I_H
#define CAIX_I_H
/*===========================================================================

            C D M A   T R A N S L A T I O N    S E R V I C E S
                    D A T A    D E C L A R A T I O N S

DESCRIPTION
  This file contains the data structures needed for use in the
  CDMA translation services.


  Copyright (c) 1990 - 2015 Qualcomm Technologies, Inc. 
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

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/caix.h_v   1.8   01 Oct 2002 15:01:32   azafer  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/caix_i.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/13/15   ppr     Feature clean up changes
11/19/12   ssh     Added SMS over CSFB support.
09/15/12   ppr     FR#2370 - 1x memory optimization changes
06/29/12   ppr     FEATURE_IS2000_REL_A_CC cleanup changes
05/09/12   ppr     Feature Cleanup
02/04/12   ppr     Feature Cleanup
12/29/11   ssh     Added ATIM cycle support.
08/29/11   cjb     Renamed FEATURE_MODEM_1X_IRAT_1XTOL to FEATURE_1X_TO_LTE.
06/14/11   ssh     Added ATIM support to perform the 1X to LTE reselection.
06/01/11   jj      CSFB: Bundled messages (UHDM+AWIM) support.
05/16/11   jj      CSFB: Externalized function which is called in CSFB mode.
05/02/11   jj      CSFB: Added support for handling GCSNA ack for UL messages.
04/29/11   ag      CSFB: Merged chanes to support SSD operation, Unique Challenge
                   in CSFB mode.
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
04/19/11   jj      CSFB: Code merge from rel/1H11 branch
04/18/11   ssh     Merged MSGR queue signal with CMD_Q_SIG. And reduced the
                   GCSNA txn buffer to be one.
02/15/11   jj      Fixed compile and lint warnings.
12/10/10   jj      Added changes to support CSFB.
11/15/10   jtm     Initial SU API split for CP.
09/28/09   ag      1X Advanced Signaling Support - Phase 2
08/02/09   jtm     Lint fixes.
03/19/07   pg      Lint Cleanup.
01/30/07   sb      Changed function prototype of caix_get_message_type.
01/18/07   sb      Fixed function prototype.
01/15/07   sb      VOIP-1X handoff updates.
11/21/06   sb      Added support for VOIP-1X handoffs.
09/29/06   fh      Added a new parameter to caix_int_to_ext to indicate whether
                   the function call is for calculating access channel message
                   size or for parsing before transmitting the message.
06/12/06   fc      Changes for UMTS.
06/01/06   fc      Cleanup umts interface.
05/17/06   fh      Merged in new development of AKA/AES features
05/16/06   fh      Changes under AES feature: the parser function caix_int_to_ext
                   carries an extra argument to indicate whether to just
                   get the buffer length for access channel messages, or parsing
                   and AES encrypting the whole message.
04/26/06   ht      Added L2 Ack support when parsing failed.
01/20/06   fc      Merged the following :
           fh      Added support for AKA and MI.
07/12/05   ht      Added MEID Addressing support
07/08/05   sb      Merged the following from MSMSHARED:
07/05/05   sb       Added MEID support.
06/03/04   fc      P_REV changes.
04/13/04   bkm     Added IS2000 Release D support.
09/19/03   fc      Added enum for ECAM buffer size sanity check.
07/21/04   sj      Added support for umts to 1x handover.
06/30/03   fc      Added support for IS2000 Release C messages.
02/05/03   fc      Added IS2000 Release B f-csh changes.
02/05/03   fc      Cleanup for f-csh forward compatability.
11/07/02   az      Mainlined FEATURE_IS95B
10/01/02   az      Mainlined FEATURE_IS2000
06/14/02   sb      Do not define xlate_ext_pc_hdr in SILK, causes compile error.
06/12/02   yll     Added xlate_ext_pc_hdr() prototype.
05/22/02   fc      Changes for message pack unit test.
05/20/02   fc      Added support for Universal Page Message.
01/07/02   sb      Only include cai.h and caii.h if we are not in SILK.
11/19/01   fc      Included cai.h and caii.h.
12/21/00   kk/lh   Added IS-2000 release A support.
06/19/00   yll     Added packing the MSID to be used by PDSM IS801 services.
12/08/99   jw      Basic IS2000 message support.
02/12/99   lcc     Added some IS95B support related to pilot reporting.
11/23/98   ram     merged in IS95B P_REV support from
                   MDR archive. Updated Copyright notice.
01/23/91   jai     Changed name of file to caix.h
10/30/90   jai     Created file.

===========================================================================*/

#include "1x_variation.h"
#include "cai.h"
#include "cai_v.h"
#include "cai_i.h"
#include "caii.h"
#include "caii_v.h"
#include "caii_i.h"
#include "caix.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* Variable to indicate the current p_rev in use */

extern byte caix_p_rev_in_use;
/* Return status */
enum
{
  CAIX_DONE_S = 0,            /* Message has been translated ok */
  CAIX_INV_LEN_S,             /* Message length was invalid */
  CAIX_INV_MSG_S,             /* Received message type was invalid */
  CAIX_INV_CHN_S,             /* Received Channel was invalid */
  CAIX_INV_ENC_MODE_NOT_SUP_S /* Encryption Mode not supported */
#ifdef FEATURE_IS2000_REL_A
  , CAIX_AES_FAIL_DO_NOTHING
  , CAIX_AES_FAIL_DO_RESYNC
  , CAIX_AES_FAIL_DO_SYS_DET
#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_REL_C
  , CAIX_MI_FAIL_DO_NOTHING   /* MI failure.. Do nothing */
  , CAIX_MI_FAIL_DO_KEY_RESET /* MI failure.. establish new keys */
  , CAIX_MI_FAIL_DO_RESYNC    /* MI failure.. establish new cryto sync */
  , CAIX_MI_FAIL_DO_SYS_DET   /* MI failure.. go to system determination */
#endif /* FEATURE_IS2000_REL_C */
};

/* Layer2 fields packing */
typedef enum
{
  CAIX_L2_FIELDS_PACK     = 0, /* Layer 2 fields are packed. */
  CAIX_L2_FIELDS_NOT_PACK = 1  /* Lsyer 2 fields are not packed. */
} caix_l2_fields_pack_type;

enum
{
  /* the following constant is used to indicate that the function call
   * of caix_int_to_ext is to really parsing the message before
   * transmission. This means the binary stream will go through the full
   * AES procedure
   */
  CAIX_PARSE_MSG = 0,

  /* the following constant is used to indicate that the function call
   * of caix_int_to_ext is to get the message length only
   */
  CAIX_CALC_MSG_LENGTH
};

#define CAIX_MIN_LEN_FOR_L2_ACK     3  /* bytes */
        /* msgtype (1 byte),
         * msg_seq (3 bits),
         * ack_seq (3 bits),
         * ack_req (1 bit),
         * encryption (2 bits)
         * total 2+ bytes (rounded up to 3 bytes)
         */

  /* The structure to hold the Message Rec field of the GEM, i.e. the message
   * extended by GEM. Since any message can be extended by GEM, so it is not
   * possible to include the message body in the GEM structure. Rather, a
   * pointer is available in GEM structure which will point to the above
   * defined structure.
   */
  extern caii_rx_msg_type *caii_gem_message_rec_ptr;

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/
/* Procedure which translates paging channel messages from external to internal format */
extern word xlate_ext_pch_msg
(
  word len,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to received message to be translated */
  caii_rx_msg_type *int_ptr
    /* Pointer to place where translated message should be placed */
);

/* Procedure which translates traffic channel messages from external to internal format */
extern word xlate_ext_tc_msg
(
  word len,
    /* Length of message in bits */
  byte *tc_ptr,
    /* Pointer to received message to be translated */
  caii_rx_msg_type *int_ptr
    /* Pointer to place where translated message should be placed */
);

/* Procedure which translates messgaes from external to internal format */
extern word caix_ext_to_int
       (
       word len,
         /* message length in bits */
       caix_chan_type chn,
         /* channel message was received on */
       cai_rx_msg_type *rx_msg_ptr,
         /* pointer to received message to be translated */
       caii_rx_msg_type *int_msg_ptr
         /* pointer to place where translated message should be placed */
       );

/* Procedure which translates messgaes from internal to external format */
extern word caix_int_to_ext
       (
       word *len_ptr,
         /* pointer to where message length, in bits should be placed */
       word chn,
         /* channel message is to be transmitted on */
       caii_tx_msg_type *int_msg_ptr,
         /* pointer to internal message to be tanslated */
       cai_tx_msg_type *tx_msg_ptr,
         /* pointer to place where translated message should be placed */
       uint8 parsing_reason
       /* This flag indicates the true reason of why this function is
        * getting called. Currently there are two reasons defined:
        *
        * CAIX_CAL_AC_MSG_LENGTH:
        * this reason is used to indicate that the function call
        * of caix_int_to_ext is to get the message length only
        *
        * CAIX_PARSE_AC_MSG:
        * this reason is used to indicate that the function call
        * of caix_int_to_ext is to really parsing the message before
        * transmission. This means the binary stream will go through the full
        * AES procedure
        */
       );

#ifdef FEATURE_GPSONE
/* Procedure which translates an MSID from internal to external format. The
 *  external format is as follows:
 *
 * Field         Length(bits)
 * RESERVED      1
 * MSID_TYPE     3
 * MSID_LEN      4
 * MSID          8xMSID_LEN
 */
extern word caix_msid_int_to_ext
       (
       word *len_ptr,
         /* Pointer to length of MSID header and fields in bits */
       byte *buf_ptr,
         /* Pointer to where translated data should be placed */
       caii_ac_hdr_type *int_ptr
         /* Pointer to internal data to be xlated */
       );
#endif /* FEATURE_GPSONE */

extern void xlate_ext_pc_hdr
(
  word *buf_pos,
    /* Position of header fields relative to start of buf_ptr */
  byte *buf_ptr,
    /* Pointer to received message to be translated */
  caii_pc_hdr_type *int_ptr
    /* Pointer to place where translated message should be placed */
);

extern void xlate_ext_tc_hdr
(
  byte *buf_ptr,
    /* Pointer to received message to be translated */
  caii_tc_hdr_type *int_ptr
    /* Pointer to place where translated message should be placed */
);

#define CAIX_P_REV_UNKNOWN  0
  /* Used to tell layer 3 that a message is in old format ( Pre 95B ) */

#define CAIX_P_REV_IS95B    4
  /* Minimum P_REV for IS95B phones */

#define CAIX_P_REV_IS2000   6
  /* Minimum P_REV for IS2000 phones */

#define CAIX_P_REV_IS2000_REL_A 7
  /* Minimum P_REV for IS2000 Release A capable phones */

#define CAIX_P_REV_IS2000_REL_B 8
  /* Minimum P_REV for IS2000 Release B capable phones */

#define CAIX_P_REV_IS2000_REL_C 9
  /* Minimum P_REV for IS2000 Release C capable phones */

#define CAIX_P_REV_IS2000_REL_C_MI 10
  /* Minimum P_REV for IS2000 Release C message integrity capable phones */

#define CAIX_P_REV_IS2000_REL_D 11
  /* Minimum P_REV for IS2000 Release D capable phones */

void caix_set_p_rev_in_use(byte p_rev_in_use);

/*===========================================================================

FUNCTION XLATE_INT_AC_BURST

DESCRIPTION
  This function translates an Access Channel Data Burst Message from
  internal to external format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/

word xlate_int_ac_burst
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to where translated message should be placed */
  caii_tx_msg_type *int_ptr,
    /* Pointer to internal message to be transmitted */
  word chn,
    /* Channel message is to be transmitted on */

  uint8 parsing_reason
  /* This flag indicates the true reason of why this function is
   * getting called. Currently there are two reasons defined:
   *
   * CAIX_CALC_MSG_LENGTH:
   * this reason is used to indicate that the function call
   * of caix_int_to_ext is to get the message length only
   *
   * CAIX_PARSE_MSG:
   * this reason is used to indicate that the function call
   * of caix_int_to_ext is to really parsing the message before
   * transmission. This means the binary stream will go through the full
   * AES procedure
   */
);

/*===========================================================================

FUNCTION XLATE_EXT_PC_BURST

DESCRIPTION
  This function translates a Paging Channel Data Burst Message from
  external to internal format.

DEPENDENCIES
  Assumes that the internal message type received as an input to the
  procedure already includes the Data Burst message type.

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/

word xlate_ext_pc_burst
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
);

/*===========================================================================

FUNCTION XLATE_INT_ORIG

DESCRIPTION
  This function translates an Origination Message from internal to
  external format.

DEPENDENCIES
  p_rev_in_use must be set correctly in the internal format.

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/

word xlate_int_orig
(
  word *len_ptr,
  /* Length of message in bits */
  byte *buf_ptr,
  /* Pointer to where translated message should be placed */
  caii_tx_msg_type *int_ptr,
  /* Pointer to internal message to be transmitted */
  word chn,
  /* Channel message is to be transmitted on */

  uint8 parsing_reason,
  /* This flag indicates the true reason of why this function is
   * getting called. Currently there are two reasons defined:
   *
   * CAIX_CALC_MSG_LENGTH:
   * this reason is used to indicate that the function call
   * of caix_int_to_ext is to get the message length only
   *
   * CAIX_PARSE_MSG:
   * this reason is used to indicate that the function call
   * of caix_int_to_ext is to really parsing the message before
   * transmission. This means the binary stream will go through the full
   * AES procedure
   */

  boolean is_voip_1x_handoff
  /* Indicates if this is a VOIP-1X handoff */
);

/*===========================================================================

FUNCTION XLATE_EXT_TC_MSG

DESCRIPTION
  This function translates a received Traffic Channel Message from
  external to internal format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/

word xlate_ext_tc_msg
(
  word len,
    /* Length of message in bits */
  byte *tc_ptr,
    /* Pointer to received message to be translated */
  caii_rx_msg_type *int_ptr
    /* Pointer to place where translated message should be placed */
);

/*===========================================================================

FUNCTION CAIX_GET_MESSAGE_TYPE

DESCRIPTION
  This function peeks at the first byte of a bitstream to determine the
  message type.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  Msg_type will hold the value of the message type.

===========================================================================*/
word caix_get_message_type
(
  byte * msg_type,
  /* This will hold the message type of the message */
  word chn,
  /* Channel message was received on */
  byte *buf_ptr
  /* Pointer to received message to be translated */
);

/*===========================================================================

FUNCTION XLATE_INT_PAGE_RESP

DESCRIPTION
  This function translates a Page Response Message from internal to
  external format.

DEPENDENCIES
  p_rev_in_use in the internal format is required.

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/

extern word xlate_int_page_resp
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to where translated message should be placed */
  caii_tx_msg_type *int_ptr,
    /* Pointer to internal message to be transmitted */
  word chn,
    /* Channel message is to be transmitted on */

  uint8 parsing_reason
  /* This flag indicates the true reason of why this function is
   * getting called. Currently there are two reasons defined:
   *
   * CAIX_CALC_MSG_LENGTH:
   * this reason is used to indicate that the function call
   * of caix_int_to_ext is to get the message length only
   *
   * CAIX_PARSE_MSG:
   * this reason is used to indicate that the function call
   * of caix_int_to_ext is to really parsing the message before
   * transmission. This means the binary stream will go through the full
   * AES procedure
   */
);

/*===========================================================================

FUNCTION XLATE_EXT_GENERAL_PAGE

DESCRIPTION
  This function translates a Paging Channel General Page Message from
  external to internal format.

DEPENDENCIES
  Assumes that the internal message type received as an input to the
  procedure already includes the General Page message type.

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/

extern word xlate_ext_general_page
(
  word len,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to received message to be translated */
  caii_rx_msg_type *int_ptr
    /* Pointer to place where translated message should be placed */
);

/*===========================================================================

FUNCTION XLATE_EXT_HO_MSG

DESCRIPTION
  This function translates a Generic Traffic Channel Handoff Direction
  Message (HDM, EHDM, GHDM, UHDM, MEID UHDM) from external to internal format.

DEPENDENCIES
  Assumes that the internal message type field received as an input to the
  function already includes the Handoff Direction Message type.  Also
  assumes that the message buffer is pointing to the field after the
  message type field.  Note:  Since this is a sort of like a union of all of
  the handoff direction messages, generally one CANNOT use sizeof on the
  cai.h structures, as they may contain extra fields.

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  Puts parsed message in location pointed to by int_ptr.

===========================================================================*/

word xlate_ext_ho_msg
(
  word len,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to received message to be translated */
  caii_rx_msg_type *int_ptr
    /* Pointer to place where translated message should be placed */
);

/*===========================================================================
FUNCTION XLATE_EXT_PC_SSD

DESCRIPTION
  This function translates a Paging Channel SSD Update Message
  from external to internal format.

DEPENDENCIES
  Assumes that the internal message type received as an input to the
  procedure already includes the SSD Update message type.

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None.
===========================================================================*/
extern word xlate_ext_pc_ssd
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
);


/*===========================================================================
FUNCTION XLATE_EXT_PC_AUTH

DESCRIPTION
  This function translates a Paging Channel Authentication Challenge Message
  from external to internal format.

DEPENDENCIES
  Assumes that the internal message type received as an input to the
  procedure already includes the Authentication Challenge message type.

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None
===========================================================================*/
extern word xlate_ext_pc_auth
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
);

/*===========================================================================

FUNCTION XLATE_INT_AUTH_CH

DESCRIPTION
  This function translates an Authentication Challenge Response Message
  from internal to external format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/

word xlate_int_auth_ch
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to where translated message should be placed */
  caii_tx_msg_type *int_ptr,
    /* Pointer to internal message to be transmitted */
  word chn,
    /* Channel message is to be transmitted on */

  uint8 parsing_reason
  /* This flag indicates the true reason of why this function is
   * getting called. Currently there are two reasons defined:
   *
   * CAIX_CALC_MSG_LENGTH:
   * this reason is used to indicate that the function call
   * of caix_int_to_ext is to get the message length only
   *
   * CAIX_PARSE_MSG:
   * this reason is used to indicate that the function call
   * of caix_int_to_ext is to really parsing the message before
   * transmission. This means the binary stream will go through the full
   * AES procedure
   */
);

/*===========================================================================

FUNCTION XLATE_INT_AC_ORD

DESCRIPTION
  This function translates an Access Channel Order Message from internal
  to external format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/

extern word xlate_int_ac_ord
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to where translated message should be placed */
  caii_tx_msg_type *int_ptr,
    /* Pointer to internal message to be transmitted */
  word chn,
    /* Channel message is to be transmitted on */

  uint8 parsing_reason
  /* This flag indicates the true reason of why this function is
   * getting called. Currently there are two reasons defined:
   *
   * CAIX_CALC_MSG_LENGTH:
   * this reason is used to indicate that the function call
   * of caix_int_to_ext is to get the message length only
   *
   * CAIX_PARSE_MSG:
   * this reason is used to indicate that the function call
   * of caix_int_to_ext is to really parsing the message before
   * transmission. This means the binary stream will go through the full
   * AES procedure
   */
);

/*===========================================================================

FUNCTION XLATE_INT_TC_ORD

DESCRIPTION
  This function translates a Traffic Channel Order Message from internal
  to external format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/

extern word xlate_int_tc_ord
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to where translated message should be placed */
  caii_tx_msg_type *int_ptr
    /* Pointer to internal message to be transmitted */
);

/*===========================================================================
FUNCTION XLATE_EXT_SUB_REC

DESCRIPTION
  This function translates the Called Party Subaddress/Calling Party
  Subaddress/Redirect Subaddress/Connected Subaddress information records
  appended to the Feature Notification Message on the Paging Channel, or
  the Alert with Information or Flash with Information Messages on the
  Forward Traffic Channels.

DEPENDENCIES
  Assumes that the buffer pointer received as an input to the
  procedure points to the first bit of the subaddress record and the
  internal message pointer points to the record.

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None
===========================================================================*/
word xlate_ext_sub_rec
(
  word rec_len,
    /* Length in bits of the information record to be parsed */
  byte *buf_ptr,
    /* Pointer to received message to be translated */
  byte *msg_ptr,
    /* Pointer to position inside the internal message type */
  word *msg_pos_ptr
    /* Pointer to position of record inside message */
);

/*===========================================================================

FUNCTION XLATE_EXT_REDIR_NUM_REC

DESCRIPTION
  This function translates the Redirecting Number information record
  appended to the Feature Notification Message on the Paging Channel, or
  the Alert with Information or Flash with Information Messages on the
  Forward Traffic Channels from the external to internal format.

DEPENDENCIES
  Assumes that the buffer pointer received as an input to the
  procedure points to the first bit of the subaddress record and the
  internal message pointer points to the record.

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
word xlate_ext_redir_num_rec
(
  word rec_len,
    /* Length in bits of the information record to be parsed */
  byte *buf_ptr,
    /* Pointer to received message to be translated */
  byte *msg_ptr,
    /* Pointer to position inside the internal message type */
  word *msg_pos_ptr
    /* Pointer to position of record inside message */
);

/*===========================================================================

FUNCTION XLATE_EXT_EXT_DISP_REC

DESCRIPTION
  This function translates the Extended Display information record
  appended to the Feature Notification Message on the Paging Channel, or
  the Alert with Information or Flash with Information Messages on the
  Forward Traffic Channels from the external to internal format.

DEPENDENCIES
  Assumes that the buffer pointer received as an input to the
  procedure points to the first bit of the subaddress record and the
  internal message pointer points to the record.

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  Updates the msg_ptr to point to the first byte past the translated
  message.

===========================================================================*/
word xlate_ext_ext_disp_rec
(
  word rec_len,
    /* Length in bits of the information record to be parsed */
  byte *buf_ptr,
    /* Pointer to received message to be translated */
  byte **message_ptr,
    /* Pointer to pointer to position inside the internal message type */
  word *msg_pos_ptr
    /* Pointer to position of record inside message */
);

/*===========================================================================

FUNCTION XLATE_EXT_PARA_ALERT_REC

DESCRIPTION
  This function translates the Parametric Alerting information record
  appended to the Feature Notification Message on the Paging Channel, or
  the Alert with Information or Flash with Information Messages on the
  Forward Traffic Channels from the external to internal format.

DEPENDENCIES
  Assumes that the buffer pointer received as an input to the
  procedure points to the first bit of the subaddress record and the
  internal message pointer points to the record.

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
word xlate_ext_para_alert_rec
(
  word rec_len,
    /* Length in bits of the information record to be parsed */
  byte *buf_ptr,
    /* Pointer to received message to be translated */
  byte *msg_ptr,
    /* Pointer to position inside the internal message type */
  word *msg_pos_ptr
    /* Pointer to position of record inside message */
);

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================

FUNCTION XLATE_EXT_MC_EXT_DISP_REC

DESCRIPTION
  This function translates the Multi-character Extended Display information
  record appended to the Feature Notification Message on the Paging Channel, or
  the Alert with Information or Flash with Information Messages on the
  Forward Traffic Channels from the external to internal format.

DEPENDENCIES
  Assumes that the buffer pointer received as an input to the
  procedure points to the first bit of the subaddress record and the
  internal message pointer points to the record.

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  Updates the msg_ptr to point to the first byte past the translated
  message.

===========================================================================*/
word xlate_ext_mc_ext_disp_rec
(
  word rec_len,
    /* Length in bits of the information record to be parsed */
  byte *buf_ptr,
    /* Pointer to received message to be translated */
  byte **message_ptr,
    /* Pointer to pointer to position inside the internal message type */
  word *msg_pos_ptr
    /* Pointer to position of record inside message */
);
#endif

#ifdef FEATURE_IS2000_REL_C
/*===========================================================================

FUNCTION XLATE_EXT_ENH_MC_EXT_DISP_REC

DESCRIPTION
  This function translates the Enhanced Multi-character Extended Display information
  record appended to the Feature Notification Message on the Paging Channel, or
  the Alert with Information or Flash with Information Messages on the
  Forward Traffic Channels from the external to internal format.

DEPENDENCIES
  Assumes that the buffer pointer received as an input to the
  procedure points to the first bit of the subaddress record and the
  internal message pointer points to the record.

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  Updates the msg_ptr to point to the first byte past the translated
  message.

===========================================================================*/
word xlate_ext_enh_mc_ext_disp_rec
(
  word rec_len,
    /* Length in bits of the information record to be parsed */
  byte *buf_ptr,
    /* Pointer to received message to be translated */
  byte **message_ptr,
    /* Pointer to pointer to position inside the internal message type */
  word *msg_pos_ptr
    /* Pointer to position of record inside message */
);
#endif /* FEATURE_IS2000_REL_C */

#ifdef FEATURE_1X_TO_LTE

/*===========================================================================

FUNCTION XLATE_EXT_ALT_TECH_INFO

DESCRIPTION
  This function translates an Alternative Technologies Information Message
  from external to internal format.

DEPENDENCIES
  Assumes that the internal message type received as an input to the
  procedure already includes the ANSI-41 Sys Parm type

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
word xlate_ext_alt_tech_info
(
  word len,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to received message to be translated */
  caii_rx_msg_type *int_ptr,
    /* Pointer to place where translated message should be placed */
 word chn
    /* Physical Channel this message is received on */
);

/*===========================================================================

FUNCTION XLATE_EXT_TC_ALT_TECH_INFO

DESCRIPTION
  This function translates an Alternative Technologies Information Message
  from external to internal format.

DEPENDENCIES
  Assumes that the internal message type received as an input to the
  procedure already includes the ANSI-41 Sys Parm type

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
word xlate_ext_tc_alt_tech_info(
  word len,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to received message to be translated */
  caii_rx_msg_type *int_ptr,
    /* Pointer to place where translated message should be placed */
 word chn
    /* Physical Channel this message is received on */
);
#endif /* FEATURE_1X_TO_LTE */

#endif /* CAIX_I_H */

