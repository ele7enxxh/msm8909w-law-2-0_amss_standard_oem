#ifndef MCCAES_H
#define MCCAES_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    C D M A   M A I N   C O N T R O L

                       ADVANCED ENCRYPTION STANDARD

GENERAL DESCRIPTION

  This is the module for Advanced Encryption Standard.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2005 - 2013 Qualcomm Technologies, Inc. 
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


/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/mccaes.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/15/10   jtm     Initial SU API split for CP.
05/14/09   ag      CMI phase 2 changes
09/30/06   fh      Moved CRC calculation function into mccsup.c
09/15/06   fh      Added a function to set LAC fields of TC messages
09/02/06   fh      Miscellaneous code review changes
08/04/06   fh      Cleaned up the usage of AES capability functions
07/26/06   fh      Added a new feature FEATURE_IS2000_REL_A_AES_ECK to keep
                   the key schedule (expanded CK) in buffer for future use
                   until the key is invalidated or regenerated.
                   Removed the middle layer function of mccaes_privacy_key.
07/24/06   fh      Added back the special validation function to check encrypt_mode
                   for UHDM after the code review. Also added a new function to
                   check ui_encrypt_mode of connection records in UHDM.
07/07/06   fh      Cleaned up AES capability helper functions, and removed the
                   UHDM AES field validating function
07/06/06   fh      Added a function to specially check encrypt_mode for UHDM
06/07/06   fh      Added Voice Service AES encryption/decryption under the features
05/24/06   fh      AKA/AES: misclaneous fixes within AKA/AES features
05/17/06   fh      Merged in new development of AKA/AES features
01/26/06   fh      create this module

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/*lint -e766*/
#include "1x_variation.h"
#include "comdef.h"


#ifdef FEATURE_IS2000_REL_A_AES

#include "mccdma_v.h"
#include "mccdma_i.h"
#include "mccaka.h"
#include "mccsec.h"
#include "mccmi.h"
#include "mccaesalg.h"
#include "bit.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
#define MCCAES_KEY_LENGTH          16  /* 16 bytes */
#define MCCAES_BLOCK_LENGTH        16  /* 16 bytes */
#define MCCAES_FRESH_SIZE           5  /* 5  bytes */

#define MCCAES_DIRECTION_FORWARD    0  /* direction for encryption/decryption */
#define MCCAES_DIRECTION_REVERSE    1  /* direction for encryption/decryption */

#define MCCAES_KEY_ID               0  /* when AKA/MI is not enabled, AES uses key ID 0 */

#define MCCAES_AES_FAILURE_MAX               2 /* consecutive aes decryption failure treshold for RESYNC */
#define MCCAES_AES_FAILURE_MAX_FOR_SYS_DET  32 /* consecutive aes decryption failure treshold for SD */
#define MCCAES_MACI_SIZE_IN_BITS            32 /* The size of MACI field in messages is 4 bytes */

#define MCCAES_CALCULATE_BUF_LENGTH          0
#define MCCAES_PERFORM_AES                   1

#define MCCAES_INVALID_VOICE_SR_ID           0xff /* this is a special SR_ID value which indicates
                                                   * that it is an invalid value with respect to AES.
                                                   * It could be either no voice service existing or
                                                   * the voice service does not have AES turned on.
                                                   */
typedef enum
{
  MCCAES_AES_DECRYPT_SUCCESS = 0x20,         /* aes decryption failure without prior MACI check */
  /* arbitray set to a value of 0x20. Just wanted to be away from MI validation status values
   * as defined in mccmi_maci_validation_status_type in mccmi.h
   */
  MCCAES_AES_NOT_ENCRYPTED,                  /* message is supposed to have been encrypted but not */
  MCCAES_AES_DECRYPT_FAIL,                   /* aes decryption successful */
  MCCAES_AES_DECRYPT_FAIL_SSEQ_DUPLICATE,    /* aes failure due to SSEQ duplicated */
  MCCAES_AES_DECRYPT_FAIL_SSEQ_OUT_OF_RANGE, /* aes failure due to SSEQ out of range */
  MCCAES_AES_DECRYPT_FAIL_AFTER_MI           /* aes decryption failure after MACI check passed */

} mccaes_decrypt_status_type;


/*===========================================================================

FUNCTION MCCAES_CALCULATE_NEW_BUF_LENGTH

DESCRIPTION
  This function calculates new buffer length if it were AES encrypted. It
  does not really perform AES encryption.


DEPENDENCIES
  None

RETURN VALUE
  the new buffer length in bits if it were AES encrypted. This may add padding
  bits and 8 bits of AES CRC.

SIDE EFFECTS
  None

===========================================================================*/
unsigned long mccaes_calculate_new_buf_length(
                  uint8 sdu_encrypt_mode,       /* sdu_encrypt_mode of the message */
                  unsigned long bit_offset,     /* in bits */
                  unsigned long buf_bit_count   /* this is the buffer length in bits */
                );

/*===========================================================================

FUNCTION MCCAES_ENCRYPT_SIG_MSG

DESCRIPTION
  This function encrypts Layer 3 PDU of a signaling message. The same buffer
  has been changed after the encryption.


DEPENDENCIES
  None

RETURN VALUE
  the new number of bits of L3 PDU after the possible padding and adding
  an 8-bit CRC

SIDE EFFECTS
  None

===========================================================================*/
uint16 mccaes_encrypt_sig_msg(
                               boolean (*get_ck_with_id)(uint8 key_id, uint8 *ck),         /* callback function to get CK */
                               uint8 ack_mode,                  /* ACK mode, 0 or 1 */
                               uint8 direction,                 /* 0 on forward link, 1 on reverse link */
                               uint32 ext_sseq,                 /* 32 bit crypto sync number */
                               uint8 *buffer,           /* data buffer to be encrypted/decrypted */
                               uint16 l3_start,         /* in bits */
                               uint16 l3_pdu_len        /* L3 PDU length in bits */
                             );


/*===========================================================================

FUNCTION MCCAES_DECRYPT_SIG_MSG

DESCRIPTION
  This function decrypts Layer 3 PDU of a signaling message. The same buffer has
  been changed after the decryption.


DEPENDENCIES
  None

RETURN VALUE
    -- MCCAES_AES_DECRYPT_SUCCESS -- if the decryption was successful
    -- MCCAES_AES_DECRYPT_FAIL_SSEQ_DUPLICATE -- if the decryption failed

SIDE EFFECTS
  None

===========================================================================*/
uint16 mccaes_decrypt_sig_msg(
                  boolean (*get_ck_with_id)(uint8 key_id, uint8 *ck),      /* call back function to get cipher key */
                  uint8 sdu_key_id,
                  uint8 ack_mode,               /* ACK mode, 0 or 1 */
                  uint8 direction,              /* 0 on forward link, 1 on reverse link */
                  uint32 ext_sseq,              /* 32 bit crypto sync number */
                  uint8 *buffer,                /* data buffer to be encrypted/decrypted */
                  uint16 l3_start,              /* in bits */
                  uint16 buf_len                /* including both LAC and L3. in bits */
                );


/*===========================================================================

FUNCTION MCCAES_SR_AES_ON

DESCRIPTION
  This function determines if AES for the given service identified by sr_id
  is ON.


DEPENDENCIES
  None

RETURN VALUE
  TRUE if AES is ON, FALSE if it is OFF.

SIDE EFFECTS
  None

===========================================================================*/
boolean mccaes_sr_aes_on( uint8 sr_id );

/*===========================================================================

FUNCTION MCCAES_CONN_AES_ON

DESCRIPTION
  This function determines if a connection of the given connection index in
  the current connection records has AES turned ON


DEPENDENCIES
  None

RETURN VALUE
  TRUE if AES is ON, FALSE if it is OFF.

SIDE EFFECTS
  None

===========================================================================*/
boolean mccaes_conn_aes_on( uint8 conn_index );

/*===========================================================================

FUNCTION MCCAES_IS_VOICE_SO

DESCRIPTION
  This function checks if a given service option is voice service option


DEPENDENCIES
  None

RETURN VALUE
  TRUE if it is a voice service option, FALSE otherwise


SIDE EFFECTS
  None

===========================================================================*/
boolean mccaes_is_voice_so( uint16 so );


/*===========================================================================

FUNCTION MCCAES_ENCRYPT_UI_FRAME

DESCRIPTION
  This function encrypts a user information frames


DEPENDENCIES
  None

RETURN VALUE
  None. The frame buffer may have been encrypted

SIDE EFFECTS
  None

===========================================================================*/
void mccaes_encrypt_ui_frame(
               uint8  sr_id,           /* the sr_id of the frame */
               uint8  chan_id,         /* the channel id of the frame */
               uint32 sys_time,        /* system_time / 20 */
               uint8  *frame_buf,      /* the frame buffer to be encrypted */
               uint16 frame_offset,    /* the starting bit to be encrypted */
               uint16 frame_bits       /* the number of bits to be encrypted */
              );

/*===========================================================================

FUNCTION MCCAES_DECRYPT_UI_FRAME

DESCRIPTION
  This function decrypts a user information frames


DEPENDENCIES
  None

RETURN VALUE
  None. The frame buffer may have been encrypted

SIDE EFFECTS
  None

===========================================================================*/
void mccaes_decrypt_ui_frame(
               uint8  sr_id,           /* the sr_id of the frame */
               uint8  chan_id,         /* the channel id of the frame */
               uint32 sys_time,        /* system_time / 20 */
               uint8  *frame_buf,      /* the frame buffer to be encrypted */
               uint16 frame_offset,    /* the starting bit to be encrypted */
               uint16 frame_bits       /* the number of bits to be encrypted */
              );

/*===========================================================================

FUNCTION MCCAES_VALIDATE_SIG_AES_MODE

DESCRIPTION
  This function validates if a signaling AES mode value is valid according to
  the AES capability of both the mobile and BS.

  Currently the only valid values are DISABLED, CMEA, and REA when it supports AES


DEPENDENCIES
  None

RETURN VALUE
  TRUE if it is a valid value, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean mccaes_validate_sig_aes_mode(uint8 enc_mode);

/*===========================================================================

FUNCTION MCCAES_VALIDATE_UI_AES_MODE

DESCRIPTION
  This function validates if a UI AES mode value is valid according to
  the AES capability of both the mobile and BS.

  Currently the only valid values are DISABLED and REA when it supports AES


DEPENDENCIES
  None

RETURN VALUE
  TRUE if it is a valid value, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean mccaes_validate_ui_aes_mode(uint8 enc_mode);


/*===========================================================================

FUNCTION MCCAES_VALIDATE_TC_SMCM_UI_ENCRYPT_MODE

DESCRIPTION
  This function validates the fields of ui_encrypt_mode for each connection records
  in TC SMCM

  Currently the only valid values are DISABLED and REA when it supports AES


DEPENDENCIES
  None

RETURN VALUE
  TRUE if the values are valid, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean mccaes_validate_tc_smcm_ui_encrypt_mode
(
    const mccrxtx_msg_type          *msg_ptr
);

/*===========================================================================

FUNCTION MCCAES_VALIDATE_TC_SMCM_AES_FIELDS

DESCRIPTION
  This function validates the AES fields for TC SMCM

  Currently the only valid values are DISABLED and REA when it supports AES


DEPENDENCIES
  None

RETURN VALUE
  TRUE if all the AES fields are valid, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean mccaes_validate_tc_smcm_aes_fields
(
   const mccrxtx_msg_type          *msg_ptr
);

/*===========================================================================

FUNCTION MCCAES_VALIDATE_TC_ENCRYPT_MODE

DESCRIPTION
  This function validates if L3 field encrypt_mode is valid for traffic channel
  messages


DEPENDENCIES
  None

RETURN VALUE
  TRUE if it is a valid value, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean mccaes_validate_tc_encrypt_mode(uint8 encrypt_mode);


/*===========================================================================

FUNCTION MCCAES_VALIDATE_UHDM_ENCRYPT_MODE

DESCRIPTION
  This function validates if encrypt_mode of UHDM is valid
  messages


DEPENDENCIES
  None

RETURN VALUE
  TRUE if it is valid, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean mccaes_validate_uhdm_encrypt_mode(const mccrxtx_msg_type *msg_ptr);

/*===========================================================================

FUNCTION MCCAES_VALIDATE_UHDM_UI_ENCRYPT_MODE

DESCRIPTION
  This function validates if ui_encrypt_mode of UHDM is valid for each of the
  connection record.


DEPENDENCIES
  None

RETURN VALUE
  TRUE if it is valid, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean mccaes_validate_uhdm_ui_encrypt_mode(const mccrxtx_msg_type *msg_ptr);

/*===========================================================================

FUNCTION MCCAES_CHECK_ENC_SEQ

DESCRIPTION
  This function checks if a given enc_seq is a valid sequence number (8 bits).
  If yes, construct a 32-bit rx_ext_sseq into the given data structure.


DEPENDENCIES
  None

RRETURN VALUE
  return a parsing status to CAIX based on SSEQ validation status.
  also the given data structure sseq_info may be updated.

  the possible return values are:
     CAIX_DONE_S -- sseq check OK
     MCCAES_AES_DECRYPT_FAIL_SSEQ_DUPLICATE -- sseq duplicated
     MCCAES_AES_DECRYPT_FAIL_SSEQ_OUT_OF_RANGE -- sseq out of range

SIDE EFFECTS
  None

===========================================================================*/
uint16 mccaes_check_enc_seq(
                              uint8 sdu_sseq,
                              /* 8 bits of SDU_SSEQ from the L2 header of the incoming message */

                              boolean ack_req,
                              /* Whether this message requires an Ack to be sent back or not */

                              mccsec_fwd_sseq_info_type * sseq_info
                              /* gathered sseq info. This is the output variable */
                            );

/*===========================================================================

FUNCTION MCCAES_STATE_UPDATE

DESCRIPTION
  This function processes SSEQ validation status and AES decryption status.
  It updates internal counter aes_status.sig_decrypt_failures accordingly.


DEPENDENCIES
  None

RETURN VALUE
  return a parsing status to CAIX

SIDE EFFECTS
  None

===========================================================================*/
uint16 mccaes_state_update( uint16 sseq_or_aes_decrypt_status );

/*===========================================================================

FUNCTION MCCAES_RTC_ENCRYPT_MODE

DESCRIPTION
  This function determines appropriate value of LAC field encrypt_mode for
  reverse traffic channel messages


DEPENDENCIES
  None

RETURN VALUE
  the value of encrypt_mode field in reverse traffic channel messages

SIDE EFFECTS
  None

===========================================================================*/
uint8 mccaes_rtc_encrypt_mode(
                              uint8 msg_type,          /* message type */

                              /* order type when it is an order message */
                              uint8 order,

                              /* orderq when it is a MS Reject Order */
                              uint8 ordq,

                              /* current encrypt_mode set in RXTX module */
                              uint8 rxtx_encrypt_mode
                              );


/*===========================================================================

FUNCTION MCCAES_VALIDATE_FCSCH_AES_FIELDS

DESCRIPTION
  This function validates AES related fields in RES messages or Common Channel
  Security Mode Command Message


DEPENDENCIES
  None

RETURN VALUE
  a boolean value of the status. If TRUE, the fields are OK, this could be either
  valid fields, or fields not to be checked when P_REV is 10 or greater and the message
  was not MACI protected.

  If it return FALSE, it means one or more AES related fields are invalid.

SIDE EFFECTS
  None

===========================================================================*/
boolean mccaes_validate_fcsch_aes_fields
(
  const mccdma_matched_msg_type *msg_ptr
);


/*===========================================================================

FUNCTION MCCAES_PROCESS_FCSCH_AES_FIELDS

DESCRIPTION
  This function processes AES related fields in RES messages or Common Channel
  Security Mode Command Message


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mccaes_process_fcsch_aes_fields
(
  const mccdma_matched_msg_type *msg_ptr
);

/*===========================================================================

FUNCTION MCCAES_PROCESS_TC_SMCM_UI_ENCRYPT_MODE

DESCRIPTION
  This function processes ui_encrypt_mode in TC SMCM.


DEPENDENCIES
  None

RETURN VALUE
  TRUE if it is successfully processed, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean mccaes_process_tc_smcm_ui_encrypt_mode
(
  const mccrxtx_msg_type          *msg_ptr
);


/*===========================================================================

FUNCTION mccaes_supports_sig_aes

DESCRIPTION
   This function returns true if mobile AND BS support signaling AES.

DEPENDENCIES
  None.

RETURN VALUE
  It returns TRUE if mobile AND BS support signaling AES. Otherwise
  it returns FALSE.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccaes_supports_sig_aes(void);


/*===========================================================================

FUNCTION mccaes_supports_ui_aes

DESCRIPTION
   This function returns true if both mobile AND BS support UI AES.

DEPENDENCIES
  None.

RETURN VALUE
  It returns TRUE if both mobile AND BS support UI AES. Otherwise
  it returns FALSE.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccaes_supports_ui_aes(void);


/*===========================================================================

FUNCTION mccaes_supports_aes

DESCRIPTION
   This function returns true if both mobile AND BS supports AES.

DEPENDENCIES
  None.

RETURN VALUE
  It returns TRUE if both mobile AND BS support signaling AES and/or UI AES. otherwise
  it returns FALSE

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccaes_supports_aes(void);


/*===========================================================================

FUNCTION mccaes_set_tc_msg_encrypt_fields

DESCRIPTION
   This function sets the LAC fields of TC messages

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void mccaes_set_tc_msg_encrypt_fields (
                                        rxtx_tc_msg_type *msg_ptr
                                      );

/*===========================================================================

FUNCTION MCCAES_SETUP_AC_ENCRYPT_FIELDS

DESCRIPTION
  This function setups the LAC encryption fields for AC messages.
  fields.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mccaes_setup_ac_encrypt_fields
(
  caii_ac_enc_type *int_ptr,
    /* Pointer to the internal format of the encryption fields */
  const caii_ac_auth_msg_int_type *ami_ptr,
    /* Pointer to the internal format of the authentication and message
       integrity fields */
  uint8 msg_type,
  uint8 ack_req,   /* the ack_mode is needed to set enc_seq */
  uint8 order,     /* in case it is an AC order message, this is the order type */
  uint8 ordq
); /* mccaes_setup_ac_encrypt_fields */

#endif /* FEATURE_IS2000_REL_A_AES */
#endif /* MCCAES_H */
