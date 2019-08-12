#ifndef AUTH_I_H
#define AUTH_I_H
/*===========================================================================

               E X T E R N A L   A U T H E N T I C A T I O N
                      T A S K   D E F I N I T I O N S

DESCRIPTION
  This contains all the declarations for the Authentication Task.

  Copyright (c) 1995 - 2013 Qualcomm Technologies, Inc. 
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

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/auth.h_v   1.8   30 Aug 2002 13:34:04   azafer  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/auth_i.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/05/13   srk     Replace existing watchdog implementation with the new
                   Dog Heartbeat Mechanism.
11/07/12   cjb     Mainlining FEATURE_MODEM_1X_SUPPORTS_RCINIT feature.
02/04/12   ppr     Feature Cleanup
01/06/12   ppr     1X CP - Feature clean up: Mainline Always On features,
                   Remove Always Off Features
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
11/15/10   jtm     Initial SU API split for CP.
09/17/10   ag      Added support for dynamic DOG.
06/24/10   ssh     Added support for EFS sync.
01/10/10   jj      CSIM-P2 , migration to MMGSDI APIs.
08/30/09   jj      Added include of uim_v.h as part of CMI.
03/26/09   adw     Removed unnecessary ulpn.h include.
11/30/05   va      Added new function auth_send_update_a_key_cmd  which is
                   remotable for calling from SVCPRG module.
11/30/05   an      Mainlined FEATURE_IS2000
05/03/05   fc      Mainlined FEATURE_IS2000.
03/19/04   sb      Removed auth_switch_to_private_lcm.
08/30/02   ts      Added support for Run Time R-UIM Enable
08/06/02   lh      Remove SVD ifdefs.
04/12/02   lh      Added support for subaddress in Orig_C.
03/12/02   lh      Added CMEA support for release A messages.
08/09/01   sb      Merged the following from common archive:
                   06/19/01   ts      Moved the VPM size constant to cave.c.  Took out the
                                      redundant #ifdef FEATURE_AUTH.
                   06/13/01   ts      Changed the Generate Keys octets from 1-65 to 0-64.
05/25/01   ts      Removed RUIM server code from auth and added interface to
                   new UIM server.
02/07/01   wli     Split RUIM_UPDATE_RECORD_ST to two states:
                   RUIM_UPDATE_LINEAR_FIXED_RECORD_ST and
                   RUIM_UPDATE_CYCLIC_RECORD_ST.
1/27/01    wli     Added new RUIM commands, INVALIDATE and REHABILITATE.
12/18/00   ck      Changed the request offset in the SSPR Configuration Request
                   Interface to a word from byte.
11/28/00   ck      Added definitions to support IS-683A with the R-UIM.
11/14/00   ts      Added access to the MSISDN in the CDMA directory.
10/11/00   ts      Moved the RUIM status outside of the rpt union field into its
                   own field.
07/16/00   ck      Added CDMA DF definition.
06/12/00   ck      Added definitions for checks related to PPS and access of
                   records from RUIM.
05/30/00   ck      Changed RUIM_UI_CHV_SIG to RUIM_CHV_SIG and also removed
                   RUIM_MAX_CHV_DIGITS as it was defined in ruim.h
05/26/00   ck      Added a comma that caused the compile to break
05/01/00   ck      Added support for RUIM software. Included the new interfaces
                   for RUIM software and featurised it under FEATURE_RUIM.
                   AUTH only interface is featurised under FEATURE_AUTH to give
                   the flexibility of defining only FEATURE_AUTH or
                   FEATURE_RUIM.
04/06/00   va      Merged the following from MSM3100_CP.03.00.17
           ry      Added OTAPA support
05/06/99   kmp     Changed T_ACP to FEATURE_ACP
03/18/99   ck      Merged the support for A-Key exchange and Re-Authentication
09/04/98   pms     Neared the variables to save ROM space
07/28/98   dna     Retain message encryption on CDMA->AMPS handoff by
                   separating auth_reset from auth_resync.
11/17/95   dna     Report back when keys are generated.
08/03/95   dna     Added support for message encryption
06/07/95   dna     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "1x_variation.h"
#include "comdef.h"
#include "modem_1x_defs.h"

#include "queue.h"
#include "qw.h"
#include "cmd.h"
#include "cai.h"
#include "caii.h"
#include "caii_v.h"
#include "caii_i.h"
#include "nv.h"

#ifdef FEATURE_CSIM
#include "mmgsdilib.h"
#include "mmgsdisessionlib.h"
#endif
#ifdef FEATURE_UIM_RUIM
#include "uim.h"
#include "uim_v.h"
#endif /* FEATURE_UIM_RUIM */
#include "auth.h"
#include "auth_v.h"
#include "dog.h"
#include "rex.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* TCB for AUTH task */
extern rex_tcb_type* auth_tcb_ptr;

/* RCInit version */
#define AUTH_TCB_PTR ( auth_tcb_ptr )


#ifdef FEATURE_UIM_RUIM

#ifdef  FEATURE_UIM_RUIM_VPM_OFFSET_ONE
#define RUIM_FIRST_OCTET_OF_VPM   1
#define RUIM_LAST_OCTET_OF_VPM    65    /* 520 bits of VPM */
#else
#define RUIM_FIRST_OCTET_OF_VPM   0
#define RUIM_LAST_OCTET_OF_VPM    64    /* 520 bits of VPM */
#endif /* FEATURE_UIM_RUIM_VPM_OFFSET_ONE */

#define RUIM_CC_MASK              0x3F
#define RUIM_CC_SHIFT             8
#endif /* FEATURE_UIM_RUIM */

/*--------------------------------------------------------------------------
                             Signal Masks
--------------------------------------------------------------------------*/
#define  AUTH_DOG_RPT_SIG        0x0001
/* This signal is for DOG Heartbeat Mechanism for AUTH task */

#define  AUTH_CMD_Q_SIG          0x0002
  /* This signal is set when an item is put on the auth_cmd_q. */

#define  AUTH_SSD_TIMER_SIG      0x0004
  /* This signal is set when the SSD timer expires */


#define  AUTH_NV_CMD_SIG         0x0008
  /* This signal is set when NV completes a command */

#ifdef FEATURE_UIM_RUIM
#define  RUIM_CMD_RSP_SIG        0x0010
  /* This signal is set when RUIM is done processing a command */

#define  AUTH_UIM_CMD_RSP_SIG    0x0020
  /* This signal is set when UIM is done processing a command */

#endif /* FEATURE_UIM_RUIM */

#ifdef FEATURE_UIM_RUIM
#define RUIM_FAIL UIM_FAIL
#define RUIM_PASS UIM_PASS
typedef uim_rpt_status ruim_rpt_status;
#endif /* FEATURE_UIM_RUIM */

#ifdef FEATURE_UIM_RUIM
#define RUIM_READ_F UIM_READ_F         /* Read item from RUIM */
#define RUIM_WRITE_F UIM_WRITE_F       /* Write item to RUIM */
#define RUIM_INC_F UIM_INC_F           /* Increment item in RUIM */

/* Mobile Directory Number */
#define  RUIM_MSISDN UIM_TELECOM_MSISDN
/* Call Count */
#define  RUIM_CC UIM_CDMA_CC
/* IMSI_M */
#define  RUIM_IMSI_M UIM_CDMA_IMSI_M
/* IMSI_T */
#define  RUIM_IMSI_T UIM_CDMA_IMSI_T
/* TMSI */
#define  RUIM_TMSI UIM_CDMA_TMSI
/* Analog Home SID */
#define  RUIM_ANALOG_HOME_SID UIM_CDMA_ANALOG_HOME_SID
/* Analog Operational Parameters */
#define  RUIM_ANALOG_OP_PARAMS UIM_CDMA_ANALOG_OP_PARAMS
/* Analog Locn and Regn Indicators */
#define  RUIM_ANALOG_LOCN_AND_REGN_IND UIM_CDMA_ANALOG_LOCN_AND_REGN_IND
/* CDMA Home SID and NID */
#define  RUIM_CDMA_HOME_SID_NID UIM_CDMA_HOME_SID_NID
/* CDMA Zone Based Regn Indicators */
#define  RUIM_CDMA_ZONE_BASED_REGN_IND UIM_CDMA_ZONE_BASED_REGN_IND
/* CDMA System/Network Regn Indicators */
#define  RUIM_CDMA_SYS_REGN_IND UIM_CDMA_SYS_REGN_IND
/* CDMA Distance Based Regn Indicators */
#define  RUIM_CDMA_DIST_BASED_REGN_IND UIM_CDMA_DIST_BASED_REGN_IND
/* Access Overload Class */
#define  RUIM_ACCOLC UIM_CDMA_ACCOLC
/* Call Termination Mode Preferences */
#define  RUIM_CALL_TERM_MODE_PREF UIM_CDMA_CALL_TERM_MODE_PREF
/* Suggested Slot Cycle Index */
#define  RUIM_SCI UIM_CDMA_SCI
/* Analog Channel Preferences */
#define  RUIM_ANALOG_CHAN_PREF UIM_CDMA_ANALOG_CHAN_PREF
/* Preferred Roaming List */
#define  RUIM_PRL UIM_CDMA_PRL
/* Removable UIM ID */
#define  RUIM_RUIM_ID UIM_CDMA_RUIM_ID
/* CDMA Service Table */
#define  RUIM_CDMA_SVC_TABLE UIM_CDMA_CDMA_SVC_TABLE
/* Service Programming Code */
#define  RUIM_SPC UIM_CDMA_SPC
/* OTAPA/SPC-Enable */
#define  RUIM_OTAPA_SPC_ENABLE UIM_CDMA_OTAPA_SPC_ENABLE
/* NAM-Lock */
#define  RUIM_NAM_LOCK UIM_CDMA_NAM_LOCK
/* OTASP-OTAPA Parameters */
#define  RUIM_OTASP_OTAPA_FEATURES UIM_CDMA_OTASP_OTAPA_FEATURES
/* Service Preferences */
#define  RUIM_SERVICE_PREF UIM_CDMA_SERVICE_PREF
/* ESN_ME */
#define  RUIM_ESN_ME UIM_CDMA_ESN_ME
/* R-UIM Phase */
#define  RUIM_RUIM_PHASE UIM_CDMA_RUIM_PHASE
/* Preferred Languages */
#define  RUIM_PREF_LANG UIM_CDMA_PREF_LANG
/* Not assigned yet */
#define  RUIM_UNASSIGNED_1 UIM_CDMA_UNASSIGNED_1
/* Short Messages */
#define  RUIM_SMS UIM_CDMA_SMS
/* Short Message Service Parameters */
#define  RUIM_SMS_PARAMS UIM_CDMA_SMS_PARAMS
/* SMS Status */
#define  RUIM_SMS_STATUS UIM_CDMA_SMS_STATUS
/* Sup Svcs Feature Code Table */
#define  RUIM_SUP_SVCS_FETURE_CODE_TABLE UIM_CDMA_SUP_SVCS_FETURE_CODE_TABLE
/* Not assigned yet */
#define  RUIM_UNASSIGNED_2 UIM_CDMA_UNASSIGNED_2
/* CDMA Home Service Provider Name */
#define  RUIM_CDMA_HOME_SVC_PVDR_NAME UIM_CDMA_HOME_SVC_PVDR_NAME
/* R-UIM ID usage indicator */
#define  RUIM_UIM_ID_USAGE_IND UIM_CDMA_UIM_ID_USAGE_IND
/* Not assigned yet */
#define  RUIM_UNASSIGNED_3 UIM_CDMA_UNASSIGNED_3
/* Mobile Directory Number */
#define  RUIM_MDN UIM_CDMA_MSISDN
/* Phonebook from Telecom directory */
#define  RUIM_ADN UIM_TELECOM_ADN
/* Administrative Data */
#define  RUIM_ADM_DATA UIM_CDMA_ADM_DATA

#define  RUIM_INVALID_S AUTH_INVALID_S   /* Recd the command in an invalid State */
#define  RUIM_UNINITIALIZED_S AUTH_UNINITIALIZED_S /* RUIM is not initialized yet */
#define  RUIM_POWERED_DOWN_S AUTH_ERR_S  /* RUIM is powered down */
#define  RUIM_INITIALIZED_S AUTH_ERR_S   /* RUIM is initialized */
#define  RUIM_ERR_S AUTH_ERR_S           /* RUIM is faulty */
#define  RUIM_MAXIMUM_S

#endif /* FEATURE_UIM_RUIM */

#ifdef FEATURE_UIM_RUIM

typedef struct {
  auth_hdr_type   hdr;            /* Command header */
  byte            len_and_usage;  /* Length and usage of ESN */
  dword           esn;            /* ESN */
  void            (*rpt_function)( auth_rpt_type * );
                                  /* Pointer to Function to send report */
} auth_store_esn_type;

#endif /* FEATURE_UIM_RUIM */

/* Queue for other tasks to get auth command buffers from */
extern q_type auth_free_q;

/* Storage for A_KEY_TEMP */
extern qword a_key_temp;

/* to indicate if new A-Key has been generated */
extern boolean a_key_temp_valid;

/* to inidcate if the new ssd is valid */
extern boolean new_ssd_valid;

/* DOG report ID for AUTH task */
extern dog_report_type auth_dog_rpt_id;

/*===========================================================================

FUNCTION AUTH_DOG_REPORT

DESCRIPTION
  This function sends report to Dog heartbeat mechanism and clears the
  dog report signal.

DEPENDENCIES
  Depends on previously initialized DOG report ID.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void auth_dog_report( void );

/*===========================================================================

FUNCTION AUTH_PRIVATE_LCM_AVAILABLE

DESCRIPTION
  This procedure checks the private lcm to make sure it can be activated.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the new lcm is available, FALSE othewise.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean auth_private_lcm_available( void );

/*===========================================================================

FUNCTION AUTH_GET_COUNT

DESCRIPTION
  This procedure retrieves the current value of COUNTsp.

DEPENDENCIES
  None

RETURN VALUE
  Returns COUNTsp.

SIDE EFFECTS
  None

===========================================================================*/
extern byte auth_get_count( void );


#ifdef FEATURE_UIM_RUIM

/*===========================================================================

FUNCTION AUTH_CALCULATE_DIGITS

DESCRIPTION
  This function packs the dialed digits that need to be sent to the RUIM. If
  the number of digits is less than 6, the leading digits are zero padded.
  A max of 6 digits are packed in a 3 byte array.

DEPENDENCIES
  The digits passed in must be in a byte array and be encoded in BCD.

RETURN VALUE
  The computed digits

SIDE EFFECTS
  None

===========================================================================*/
void auth_calculate_digits
(
  byte digits[],
    /* Dialed digits or destination address digits */
  byte array_len,
    /* The number of digits in the above array */
  boolean digit_mode,
    /* TRUE if digits are ASCII, FALSE if digits are DTMF */
  byte output_digits[]
    /* output array */
);

#endif /* FEATURE_UIM_RUIM */


/*===========================================================================

FUNCTION AUTH_GET_PRIVATE_LCM

DESCRIPTION
  This procedure returns the private long code mask.

DEPENDENCIES
  The function auth_check_private_lcm needs to be called first to ensure
  a valid long code mask is available.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void auth_get_private_lcm ( qword qplcm );

/*===========================================================================

FUNCTION AUTH_ENCRYPTION_POSSIBLE

DESCRIPTION
  This procedure determines if message encryption is possible.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if auth could successfully encrypt messages.
  FALSE if auth is not initialized properly to do encryption.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean auth_encryption_possible( void );

/*===========================================================================

FUNCTION AUTH_ENCRYPT_CDMA

DESCRIPTION
  This procedure encrypts CDMA reverse traffic channel messages.
  If any fields require encryption they are encrypted.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The num_digits, num_fields, record_len, etc. fields may not accurately
  represent the size of the field(s) if bits are added for encryption.

===========================================================================*/
extern void auth_encrypt_cdma
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to body of already packed traffic channel message */
  caii_tx_msg_type *int_ptr
    /* Pointer to internal message to be transmitted */
);

/*===========================================================================

FUNCTION AUTH_DECRYPT_CDMA

DESCRIPTION
  This procedure decrypts CDMA reverse traffic channel messages.
  If any fields require decryption they are decrypted.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if there are no problems encountered.
  FALSE if fields of the given message that should be decrypted are not.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean auth_decrypt_cdma
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr
    /* Pointer to received message to be translated */
);

/*===========================================================================

FUNCTION AUTH_ENCRYPT_INFO_RECS

DESCRIPTION
  This procedure encrypts type specific fields in reverse traffic channel
  message.  To accomplish this I first make a temporary
  copy of the packed message body.  Then I unpack each set of type
  specific fields from the copy, encrypt them, and copy them back
  into the original message to be transmitted.  If a record_len is
  only one byte, another byte is added.  Once the message to be
  transmitted is lengthened it is necessary to copy all remaining
  fields (still encrypting the type specific fields, of course).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The length of Traffic Channel message to transmit may be increased.

===========================================================================*/
void auth_encrypt_info_recs
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to body of packed external-format message */
  caii_tx_msg_type *int_ptr,
    /* Pointer to internal message to be transmitted */
  word rec_start_pos,
    /* Start position of the info record after num_rec field */
  byte num_recs
    /* Number of info records to be processed */
);

/*===========================================================================

FUNCTION AUTH_ENCRYPT_ORIG_C

DESCRIPTION
  This procedure encrypts CDMA reverse traffic channel origination
  continuation message.  I copy the packed digit data into a temporary
  buffer, encrypt it, and then put it back.

DEPENDENCIES
  The Origination Continuation Message must have at least one digit in it.

RETURN VALUE
  None

SIDE EFFECTS
  The length of Traffic Channel message to transmit may be increased.

===========================================================================*/
void auth_encrypt_orig_c
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to body of packed external-format message */
  caii_tx_msg_type *int_ptr
    /* Pointer to internal message to be transmitted */
);

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================

FUNCTION AUTH_ENCRYPT_EFWIM

DESCRIPTION
  This procedure encrypts CDMA reverse traffic channel Extended Flash with
  Information message.  To accomplish this I first make a temporary
  copy of the packed message body.  Then I unpack each set of type
  specific fields from the copy, encrypt them, and copy them back
  into the original message to be transmitted.  If a record_len is
  only one byte, another byte is added.  Once the message to be
  transmitted is lengthened it is necessary to copy all remaining
  fields (still encrypting the type specific fields, of course).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The length of Traffic Channel message to transmit may be increased.

===========================================================================*/
void auth_encrypt_efwim
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to body of packed external-format message */
  caii_tx_msg_type *int_ptr
    /* Pointer to internal message to be transmitted */
);

/*===========================================================================

FUNCTION AUTH_ENCRYPT_EOM_DIGITS

DESCRIPTION
  This procedure encrypts CDMA reverse traffic channel enhanced origination
  message.  I copy the packed digit data into a temporary
  buffer, encrypt it, and then put it back.

DEPENDENCIES
  The Enhanced Origination Message must have at least one digit in it.

RETURN VALUE
  None

SIDE EFFECTS
  The length of Traffic Channel message to transmit may be increased.

===========================================================================*/
void auth_encrypt_eom_digits
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to body of packed external-format message */
  caii_tx_msg_type *int_ptr,
    /* Pointer to internal message to be transmitted */
  word digit_start_pos
    /* Start location of digits */
);

/*===========================================================================

FUNCTION AUTH_DECRYPT_EXTENDED_INFO

DESCRIPTION
  This procedure decrypts CDMA forward traffic channel Extended Flash with
  Information and Extended Alert with information messages.  To accomplish
  this I unpack each set of type specific fields, encrypt them,
  and copy them back into the original received packed message.
  If a record_len is only one byte, the extra byte added for
  encryption is removed and the overall length is decreased.
  Once the received packed message is shortened it is necessary
  to copy all remaining fields (still decrypting the type specific
  fields, of course).

DEPENDENCIES
  None

RETURN VALUE
  TURE if successful. Otherwise return FALSE.

SIDE EFFECTS
  The length of Traffic Channel message may be increased.

===========================================================================*/
boolean auth_decrypt_extended_info
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to body of packed external-format message */
  word start_pos,
    /* Start point of Information Record */
  byte num_recs
    /* Numer of records in this message */
);
#endif /* FEATURE_IS2000_REL_A */




#endif /*AUTH_I_H*/
