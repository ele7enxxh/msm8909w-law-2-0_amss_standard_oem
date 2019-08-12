#ifndef UIM_V_H
#define UIM_V_H
/*===========================================================================

                  U I M  T A S K   D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for the UIM Task.

Copyright (c) 2008 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/api/uim_v.h#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/05/15   ks      Rel12 polling enhancement
11/17/14   nmb     Remove dead directory maintenance code
08/06/14   js      Handle SW 6280, 63C1 for non-telecom ADFs for activate ADF
08/05/14   lxu     add DF field in uim_terminal_profile_cmd_type
07/03/14   xj      Added UIM_MAX_CLA_TOGGLE_ATTEMPTS to toggle CLA for 2 times
06/16/14   ll      Switch to new string based TLMM APIs
04/29/14   nmb     Re-enable AT+CRSM after directory maintenance changes
03/20/14   nmb     Directory maintenance for streaming APDUs on default channel
02/25/14   na      Support of select next api for non-prov session
02/20/14   ak      Fix compile errors due to strict compiler on 9x35
02/02/14   nmb     SFI initial engg revision
01/16/14   akv     Fix for manage channel command failures due to enum mismatch
01/14/14   nmb     Remove ACCESS_F and related unused functionality
12/21/13   am      Silent Recover Feature Implementation
12/16/13   yt      Support for ISIM AUTH in HTTP digest security context
10/03/13   ks      Removed UIM_FLUSH_FOR_SLOT option for uim_flush_type
09/20/13   js      uim_cmd() should return error status
09/17/13   ssr     Added support for new recovery notification for GSTK
09/11/13   sam     API to enable disabling of uim recovery during run time
08/13/31   js      UIMDRV Memory De-Couple changes
08/08/13   ks      Advanced Subscription Manager changes
08/07/13   tl      Add CDMA AKA Authenticate functionality
07/15/13   ssr     Fixed 62XX status byte for APP selection through
                   QMI_UIM_LOGICAL_CHANNE
07/15/13   na      Introduction of new NULL_BYTES card error type
07/12/13   nmb     Global Reorganization
06/24/13   js      TSTS changes
05/30/13   js      NFC:Send response data and SWs if SWs are not recognizaed
04/15/13   tl      Removed MFLO functional components
03/31/13   tl      Set block length and data to match spec conformance
12/05/12   js      UIM Parallel Processing changes
10/09/12   tl      Change static data length copy to dynamic data length
07/19/12   tl      Increase number of supported logical channels to 20
05/22/12   yb      Robust fix to handle 61 XX chained response
03/20/12   rm      Changes for hotswap support
02/21/12   rm      Setting card detection polarity based on NV item
02/07/12   js      Fixed uim compilation warnings
01/17/12   js      Fixed length type mismatch for uim cmd/rsp data buffers
01/11/12   rm      Fix to handle 6F C2 as a successful response
12/20/11   yb      Adding new error code for technical problem
09/09/11   ssr     Fix to allow the reset command when switch is un-pressed where
                   client want to disable the hotswap interrupt at runtime,
                   Added new API to return the real time card status,
                   Added new API to disable the hotswap interrupt
09/01/11   ak      GPIO hotswap support for 9x15
08/11/11   nmb     Extended Logical Channels
08/01/11   vs      Added support for session open with select response
07/08/11   rm      Catch-up with latest on Hot-swap
07/06/11   rm      Remove the definition of uim_free_q
06/08/11   nmb     Corrected ISIM reference control number
05/13/11   nmb     Added support for ISIM GBA Authentication
04/27/11   yt      Moved definitions to private UIM header
04/25/11   yb      Adding support for hotswap
04/20/11   yt      Exposed APIs for calculating file path
04/12/11   yb      Added support for complete recovery solution
11/30/10   yt      Moving data types and APIs used only within UIM to private
                   header
09/27/10   yb      Added support to send aid TLV length as p3 in status
                   command to card
09/10/10   ps      Merge UIM Driver DSDS changes
09/08/10   nb      Added API to Start and Stop polling
07/07/10   yb      Removing dependency on rex from apps side
07/01/10   nmb     Updates for JCDMA CSIM support
06/06/10   kk      Fixed uim get enum from path util function
05/25/10   nmb     Moved slot to the end of uim_rpt_type
05/06/10   dd      Send larger terminal profile
04/14/10   yb      Revert back changes for Secure SIM crash
04/06/10   yb      Fix for crash when locked Secure SIM is moved to 2nd FFA
03/02/10   jk      Added MMSS files
02/18/10   vs      Merge for CSIM functionality
02/08/10   nb      Updated for GSTK Dual Slot Changes
12/08/09   js      Added support for UIM_CSIM_ARR file
10/28/09   mib     Added support for additional files
10/28/09   nb      Release 7 Enhanced Network Selection
10/23/09   kp      Moved shared enums from uim_v.h to uim.h
09/26/09   kp      ZI memory reduction changes
08/20/09   mib     Added support for additional files in DF-HNB and DF-WLAN
08/13/09   nb      Moved declarations to uim.h
07/20/09   mib     Added support for additional LTE files and EF-UST extension
07/17/09   yb      Added support for BCSMS service in cdma service table
06/10/09   yb      Fix to send status word as received in get response to AT COP
                   even if card returns error for envelope sent.
04/29/09   kk      Updated EF file path info for non-spec defined telecom EFs
03/18/09   ssr     Fixed limited Length TERMINAL PROFILE for non-spec
                   compliant cards
03/02/09   sun     Removed inclusion of private header file
02/17/09   nb      Fixed Compilation Warning
12/17/08   nb      Initial Version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "queue.h"
#include "cmd.h"
#include "qw.h"
#include "sys.h"
#include "uim.h"
#include "rex.h"
#include "DDITlmm.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#define UIM_MAX_INSTANCES        3
#define UIM_CMD_RSP_TIME         2000
#define UIM_COMMIT_RSP_TIME      30000

#define UIM_MAX_CLA_TOGGLE_ATTEMPTS 2

#define UIM_MAX_NUM_ATTEMPTS        3

#define UIM_MAX_PATH_ELEMENTS       4
#define UIM_DIRMAIN_MAX_PATH_ELEMENTS       5

#define UIM_AID_MAX_NAME_LENGTH  16

#define UIM_MAX_ATR_CHARS                             33
#define UIM_MAX_CHARS                                 512
#define UIM_MAX_CHV_DIGITS                            8    /* This is the maximum number of digits */
#define UIM_MAX_PSO_DATA                              256
#define UIM_MAX_BYTE_SIZE                             256

#define UIM_MAX_STREAM_ISO7816_APDU_TRANSACTION_SIZE  261


/* This is the size of the buffer created to hold the response from a compute
IP Authentication */
#define UIM_COMPUTE_IP_AUTH_RESPONSE_LENGTH 16


/* Maximum Challenge length for Simple IP */
#define UIM_MAX_3GPD_CHAP_CHALLENGE_LENGTH  32

/* Maximum Length for MN-HA Registration Data
This is changeable depending on how large the registration data is expected to
be and how many blocks of data the card can accept */
#define UIM_MAX_3GPD_MN_HA_REG_DATA_LENGTH  800

/* Maximum Length for MIP-RRQ Registration Data
This is changeable depending on how large the registration data is expected to
be and how many blocks of data the card can accept */
#define UIM_MAX_3GPD_HASH_RRQ_DATA_LENGTH   800

/* Length of the MN-AAA extention header, used in the MIP-RRQ-HASH command */
#define UIM_3GPD_MN_AAA_EXT_HEADER_LENGTH   8

/* Length of the challenge data for Mobile IP */
#define UIM_3GPD_MIP_CHALLENGE_LENGTH       238

#define UIM_MAX_HRPD_CHAP_CHALLENGE_LENGTH 254

/* OTASP/OTAPA NAM LOCK definitions. */
#define UIM_NAM_LOCK_STATE_MASK  0x1
#define UIM_NAM_LOCK_OTA_MASK    0x4
#define UIM_NL_STATE_LOCK        0x1
#define UIM_NL_STATE_UNLOCK      0x0
#define UIM_OTA_OTAPA            0x4
#define UIM_OTA_OTASP            0x0
#define UIM_OTA_NAM_LOCK         0x2
#define UIM_NAM_LOCK_BITS        0x7
#define UIM_SERV_KEY_GEN_ID_SIZE 0x2     /* Size in bytes */

/* CDMA Authentication definitions */
#define UIM_RUN_CAVE_DIGITS     3
#define UIM_GLOBAL_RAND_CHAL     0x00
#define UIM_UNIQUE_RAND_CHAL     0x01
#define UIM_AKA_AUTH_KEY_LEN     0x10    /* Size in bytes */
#define UIM_AKA_AUTH_AUTS_LEN    0x0E    /* Size in bytes */
#define UIM_AKA_AUTH_RANDA_LEN   0x10    /* Size in bytes */

#define UIM_WIM_MSE_P1_COMPUTE 0x41
#define UIM_WIM_MSE_P1_ENCIPHERING 0x81

#define UIM_WIM_MSE_P2_DIGITAL_SIGNATURE 0xB6
#define UIM_WIM_MSE_P2_CONFIDENTIALITY 0xB8
#define UIM_WIM_MSE_P2_CRYPTO_CHECKSUM 0xB4

/* Seek and Search parameters */
#define UIM_P2_SEEK_TYPE_1       0x00
#define UIM_P2_SEEK_TYPE_2       0x10

#define UIM_P2_SEEK_BEGINNING_FORWARDS   0
#define UIM_P2_SEEK_END_BACKWARDS        1
#define UIM_P2_SEEK_NEXT_LOCN_FORWARDS   2
#define UIM_P2_SEEK_PREV_LOCN_BACKWARDS  3

/* b4-b8 of the first search indicaton byte */
#define UIM_SRCH_IND_ABS            0x00
#define UIM_SRCH_IND_CHAR           0x08

/* b1-b3 of the first search indicaton byte */
#define UIM_SRCH_IND_P1_FORWARD     0x04
#define UIM_SRCH_IND_P1_BACKWARD    0x05
#define UIM_SRCH_IND_NEXT           0x06
#define UIM_SRCH_IND_PREV           0x07

/* Bit 2 of first data byte indicates where to start search */
#define UIM_SRCH_MODE_MASK          0x02

/* The length of the search indication bytes */
#define UIM_SRCH_IND_BYTES          2

#define UIM_MAX_SRCH_PATTERN_LEN    255

/* Status parameters */
#define UIM_STATUS_P1_NO_INDICATION     0x00
#define UIM_STATUS_P1_CURR_APP_INIT     0x01
#define UIM_STATUS_P1_TERMN_OF_CURR_APP 0x02


#define UIM_STATUS_P2_RSP_SAME_AS_SELECT  0x00
#define UIM_STATUS_P2_DF_NAME_OF_CURR_APP 0x01
#define UIM_STATUS_P2_RETURN_NONE         0x0C

#define UIM_SPC_STATUS_MASK      0x01

#define UIM_TERMINAL_PROFILE_DATA_LENGTH    35

/* Mediaflo length #defines */
#define UIM_MFLO_SUBSCRIBER_ID_LEN       0x08
#define UIM_MFLO_PUBLIC_KEY_LEN          0x83
#define UIM_MFLO_INIT_ACT_PARAMS_LEN     0x88


#define UIM_PARAM_P            64     /* 64 bytes of param p in key exchange */
#define UIM_PARAM_G            20     /* 20 bytes of param g in key exchange */
#define UIM_RAND_SEED          20     /* 20 bytes of seed used in key exchange*/

/* ISIM AUTHENTICATION P2 values */
#define UIM_AUTN_ISIM_AKA            0x01
#define UIM_AUTN_ISIM_HTTP_DIGEST    0x02
#define UIM_AUTN_ISIM_GBA_DATA       0x04

/* MAX bytes supported in Envelope ADPU */
#define UIM_MAX_ENVELOPE_BYTES 255

/* MAX bytes supported in TR APDU */
#define UIM_MAX_TR_BYTES       255

/* MAX Authentication Data Length */
#define UIM_MAX_AUTH_BYTES     255

/* MAX AKA Auth data length */
#define UIM_MAX_AKA_AUTH_BYTES 255

/* MAX download request bytes */
#define UIM_MAX_DLOAD_REQ_BYTES 255

/* Max payload in validation request */
#define UIM_MAX_VALIDATION_REQ_BYTES 255

/* Value to be used for invalid or absent SFI */
#define UIM_INVALID_SFI 0x00

/* Value to be used for invalid or unknown CALL TYPE */
#define UIM_CALL_TYPE_NOT_APPLICABLE 0x0
/*--------------------------------------------------------------------------
                         UIM REPORT

--------------------------------------------------------------------------*/

/* -------------------------------------------------------------------
** The UIM Task optionally sends a report when it completes
** a command which produces a result which would be useful to the
** task which issued the command.
** ------------------------------------------------------------------- */
typedef enum
{
  UIM_NO_SUCH_REPORT_R,   /* No such report */
  UIM_ACCESS_R,           /* mmgsdi uses this rsp type as generic r/w rsp */
  UIM_SELECT_R,           /* Select a file/directory from the UIM */
  UIM_SEEK_R,             /* Seek from the UIM */
  UIM_INVALIDATE_R,       /* Invalidate */
  UIM_REHABILITATE_R,     /* Rehabilitate an EF */
  UIM_VERIFY_CHV_R,       /* Verify CHV */
  UIM_CHANGE_CHV_R,       /* Change CHV */
  UIM_DISABLE_CHV_R,      /* Disable CHV */
  UIM_ENABLE_CHV_R,       /* Enable CHV */
  UIM_UNBLOCK_CHV_R,      /* Unblock CHV */
  UIM_STATUS_R,           /* Status of the card */
  UIM_TERMINAL_PROFILE_R, /* Terminal Profile */
  UIM_ENVELOPE_R,         /* UIM acknowledgement/result */
  UIM_TERMINAL_RESPONSE_R,/* Terminal Response */
  UIM_POLLING_INTERVAL_R, /* Polling interval and polling off */
  UIM_RESET_R,            /* Warm Reset */
  UIM_RUN_GSM_ALGO_R,     /* RUN GSM algorithm */
  UIM_STORE_ESN_ME_R,     /* Store ESN_ME command */
  UIM_SSD_UPDATE_R,       /* Begin process to update SSD */
  UIM_CONFIRM_SSD_R,      /* Confirm SSD Update */
  UIM_RUN_CAVE_R,         /* Perform auth signature */
  UIM_AKA_AUTH_R,         /* Perform AKA Authentication */
  UIM_GENERATE_KEYS_R,    /* Generate CMEA key and VPM */
  UIM_BS_CHAL_R,          /* Do a Base station Challenge */
  UIM_COMMIT_R,           /* Commit Request */
  UIM_MS_KEY_REQ_R,       /* MS Key Request */
  UIM_KEY_GEN_REQ_R,      /* Key Generation Request */
  UIM_SERV_KEY_GEN_REQ_R, /* Service Key Generation Request */
  UIM_VALIDATE_R,         /* Validation Request */
  UIM_CONFIG_REQ_R,       /* Configuration Request */
  UIM_DOWNLOAD_REQ_R,     /* Download Request */
  UIM_SSPR_CONFIG_REQ_R,  /* SSPR Configuration Request */
  UIM_SSPR_DOWNLOAD_REQ_R,/* SSPR Download Reqeust */
  UIM_OTAPA_REQ_R,        /* Otapa Request */
  UIM_AUTHENTICATE_R,     /* Authenticate for USIM */
  UIM_AUTHENTICATE_ODD_INS_R, /*Authenticate for USIM with ODD Ins Class*/
  UIM_MANAGE_CHANNEL_R,   /* Used to Open/Close Logical Channels */
  UIM_VERIFY_IN_CHAN_R,   /* Verifies data sent from ME */
  UIM_ENABLE_VER_REQ_IN_CHAN_R,     /* Enables Verification Requirement */
  UIM_DISABLE_VER_REQ_IN_CHAN_R,    /* Disables Verification Requirement */
  UIM_CHANGE_REF_DATA_IN_CHAN_R,    /* Compares, Conditionally replaces data */
  UIM_RESET_RETRY_COUNTER_IN_CHAN_R,/* Change ref data after reset of counter */
  UIM_MANAGE_SECURITY_ENV_R, /* Performs Restore, Set, Derive Key */
  UIM_PERFORM_SECURITY_OP_R, /* Key Transport, agreement, decipher, compute
                               and verify digital signature, cmpte checksum */
  UIM_ASK_RANDOM_R,       /* Gets Random Number */
  UIM_SELECT_IN_CHAN_R,   /* Select command in Logical Channel */
  UIM_STREAM_APDU_R,      /*  Stream an APDU to the card */
  UIM_STREAM_ISO7816_APDU_R, /* Stream an ISO 7816 APDU to the card */
  UIM_RESET_WITH_TP_R,    /* Warm Reset with optional Terminal Profile */
  UIM_RESET_SWITCH_UIM_PASSIVE_R, /* Warm Reset to make UIM Passive */
  UIM_POWER_UP_UIM_PASSIVE_R,     /* Power up UIM and bring to passive mode */
  UIM_POWER_DOWN_R,               /* Power down the UIM interface */
  UIM_PREF_SLOT_CHANGE_R, /* Preferred Slot change */
  UIM_COMPUTE_IP_AUTH_R,  /* Compute IP Authentication report */
  UIM_ISIM_AUTHENTICATE_R,
  UIM_READ_BIN_CMD_R,     /* Read binary ef from UIM */
  UIM_READ_REC_CMD_R,     /* Read non binary ef from UIM */
  UIM_WRITE_BIN_CMD_R,        /* Write to UIM  */
  UIM_WRITE_REC_CMD_R,        /* Write to UIM  */
  UIM_INC_CMD_R,          /* Increment record*/
  UIM_CLEAR_CACHE_R,     /* clear cache command response */
  UIM_RECOVER_COMPLETE_R, /*Recovery complete command response */
  UIM_STORE_ESN_MEID_ME_R,             /* Store ESN_MEID_ME command */
  UIM_BCAST_R,                         /* Bcast Message*/
  UIM_MAX_R
} uim_report_code_type;

typedef enum {
  UIM_FAIL = 0,                /* FAIL */
  UIM_PASS = 1                 /* PASS */
} uim_rpt_status;

/* Return type that could be used to indicate success/failure */
typedef enum {
  UIM_SUCCESS,
  UIM_INVALID_PARAMS,
  UIM_MEM_PROBLEM,
  UIM_GENERIC_ERROR
}uim_return_type;

/* We plan to support a maximum of  2 slots in the future */
typedef enum {
  UIM_SLOT_NONE = 0,
  UIM_SLOT_1  = 1,
  UIM_SLOT_2  = 2,
  UIM_SLOT_3  = 3,
  UIM_SLOT_AUTOMATIC = 4
} uim_slot_type;

typedef enum {
  UIM_INSTANCE_1,
  UIM_INSTANCE_2,
  UIM_INSTANCE_3,
  UIM_INSTANCE_INVALID
}uim_instance_enum_type;

/* This enum defines the first status word                                   */
typedef enum {
  SW1_NORMAL_END = 0x90,  /* Normal ending of the command                    */
  SW1_END_FETCH  = 0x91,  /* Normal ending of the command with extra command */
  SW1_DLOAD_ERR  = 0x9E,  /* SIM data download error */
  SW1_END_RESP   = 0x9F,  /* Normal ending of the command with a response    */
  SW1_BUSY       = 0x93,  /* SIM App toolkit is busy                         */
  SW1_END_RETRY  = 0x92,  /* Command successful with update retries or memory
                             error                                           */
  SW1_REFERENCE  = 0x94,  /* A reference management problem                  */
  SW1_SECURITY   = 0x98,  /* A security problem                              */
  SW1_NULL       = 0x60,  /* NULL procedure byte                             */
  SW1_P3_BAD     = 0x67,  /* Incorrect parameter P3                          */
  SW1_P1_P2_BAD  = 0x6B,  /* Incorrect parameter P1 or P2                    */
  SW1_INS_BAD    = 0x6D,  /* Unknown instruction code                        */
  SW1_CLA_BAD    = 0x6E,  /* Wrong instruction class given in command        */
  SW1_PROBLEM    = 0x6F,  /* Technical problem; no diagnostic given          */
  SW1_SIG_ERROR1 = 0xFF,  /* This is an indication of an error signal from the
                             transmitted byte                                */
  SW1_SIG_ERROR2 = 0xFE,  /* This is an indication of an error signal from the
                             transmitted byte                                */
  SW1_NO_STATUS  = 0x00,   /* This status word is used when a status word cannot
                             be obtained from the card.  It offers to status
                             information */
  SW1_WARNINGS1 = 0x62,     /* Warnings                                      */
  SW1_WARNINGS2 = 0x63,     /* Warnings                                      */
  SW1_EXECUTION_ERR2    = 0x65, /* No info given, state of NV memory changed */
  SW1_CMD_NOT_ALLOWED   = 0x69, /* Command not allowed                       */
  SW1_WRONG_PARAMS      = 0x6A, /* Wrong parameters                          */

  SW1_USIM_RESEND_APDU  = 0x6C, /* Re-send the APDU header                   */
  SW1_EXECUTION_ERR1    = 0x64, /* No info given, NV memory unchanged        */
  SW1_CLA_NOT_SUPPORTED = 0x68, /* functions in CLA not supported            */
  SW1_USIM_END_RESP     = 0x61, /* Normal ending of a command                */

  SW1_UTIL_END_RESP     = 0x61, /* Normal ending of a command                */
  SW1_LENGTH_ERROR      = 0x6C, /* Wrong length Le                           */
  SW1_SE_FAIL           = 0x66, /* Security Environment error/not set         */
  SW1_95_NFC            = 0x95, /* SW1 = 0x95 used for NFC                    */
  SW1_96_NFC            = 0x96, /* SW1 = 0x96 used for NFC                    */
  SW1_97_NFC            = 0x97, /* SW1 = 0x97 used for NFC                    */
  SW1_99_NFC            = 0x99, /* SW1 = 0x99 used for NFC                    */
  SW1_9A_NFC            = 0x9A, /* SW1 = 0x9A used for NFC                    */
  SW1_9B_NFC            = 0x9B, /* SW1 = 0x9B used for NFC                    */
  SW1_9C_NFC            = 0x9C, /* SW1 = 0x9C used for NFC                    */
  SW1_9D_NFC            = 0x9D  /* SW1 = 0x9D used for NFC                    */
} uim_sw1_type;


/* This enum defines the second status word                                  */
typedef enum {
  SW2_NORMAL_END    = 0x00,  /* Normal ending of the command                 */
  SW2_MEM_PROBLEM   = 0x40,  /* Memory problem                               */
  SW2_NO_EF         = 0x00,  /* No EF selected                               */
  SW2_OUT_OF_RANGE  = 0x02,  /* Out of range (invalid address)               */
  SW2_NOT_FOUND     = 0x04,  /* File ID/pattern not found                    */
  SW2_INCONSISTENT  = 0x08,  /* File inconsistent with command               */
  SW2_NO_CHV        = 0x02,  /* No CHV initialized                           */
  SW2_NOT_FULFILLED = 0x04,  /* Access condition not fulfilled               */
  SW2_CONTRADICTION = 0x08,  /* In contradiction with CHV status             */
  SW2_INVALIDATION  = 0x10,  /* In contradiction with invalidation status    */
  SW2_SEQ_PROBLEM   = 0x34,  /* Problem in the sequence of the command       */
  SW2_BLOCKED       = 0x40,  /* Access blocked                               */
  SW2_MAX_REACHED   = 0x50,  /* Increase cannot be performed; Max value
                                reached                                      */
  SW2_ALGORITHM_NOT_SUPPORTED   = 0x84,
  SW2_INVALID_KEY_CHECK_VALUE   = 0x85,
  SW2_PIN_BLOCKED               = 0x83,
  SW2_NO_EF_SELECTED            = 0x86,
  SW2_FILE_NOT_FOUND            = 0x82,
  SW2_REF_DATA_NOT_FOUND        = 0x88,
  SW2_NV_STATE_UNCHANGED        = 0x00,
  SW2_CMD_SUCCESS_INT_X_RETRIES = 0xC0,
  SW2_MEMORY_PROBLEM            = 0x81,
  SW2_SECURITY_NOT_SATISFIED    = 0x82,
  SW2_NO_INFO_GIVEN             = 0x00,
  SW2_LOGICAL_CHAN_NOT_SUPPORTED= 0x81,
  SW2_SECURE_MSG_NOT_SUPPORTED  = 0x82,
  SW2_FILE_STRUCT_INCOMPATIBLE  = 0x81,
  SW2_REF_DATA_INVALIDATED      = 0x84,
  SW2_CONDITIONS_NOT_SATISFIED  = 0x85,
  SW2_BAD_PARAMS_IN_DATA_FIELD  = 0x80,
  SW2_FUNCTION_NOT_SUPPORTED    = 0x81,
  SW2_RECORD_NOT_FOUND          = 0x83,
  SW2_INCONSISTENT_LC           = 0x85,
  SW2_BAD_PARAMS_P1_P2          = 0x86,
  SW2_LC_INCONSISTENT_WITH_P1_P2= 0x87,
  SW2_NOT_ENOUGH_MEMORY_SPACE   = 0x84,
  SW2_NV_STATE_CHANGED          = 0x00,
  SW2_MORE_DATA_AVAILABLE       = 0x10,

  SW2_RET_DATA_MAY_BE_CORRUPTED = 0x81,
  SW2_EOF_REACHED               = 0x82,
  SW2_SEL_EF_INVALIDATED        = 0x83,
  SW2_INCORRECT_FCI_FORMAT      = 0x84,

  SW2_FILE_FILLED_BY_LAST_WRITE = 0x81,

  SW2_APPLN_SPECIFIC_AUTH_ERR   = 0x62,

  SW2_PIN_STATUS_ERR     = 0x85, /* In contradiction with PIN status */
  SW2_PREMSTR_NOT_RDY    = 0x85, /* Pre-master secret not ready */
  SW2_INTRNL_NOT_RDY     = 0x85, /* Internal data not ready */
  SW2_INCORRECT_TAG      = 0x80, /* Incorrect data or tag */
  SW2_UNDEF_99           = 0x99,
  SW2_MORE_DATA          = 0xF1,
  SW2_AUTH_RSP_DATA      = 0xF3, /* Authentication Response Data Available*/

  SW2_UCAST_AUTH_KEY_NOT_INIT                = 0x01,   /* sw1 = 0x69 */
  SW2_UCAST_DATA_BLOCK_NOT_INIT              = 0x02,   /* sw1 = 0x69 */
  SW2_VERIFY_UCAST_FAILED                    = 0x04,   /* sw1 = 0x69 */
  SW2_LOCKED_INIT_PARAM                      = 0x05,   /* sw1 = 0x69 */
  SW2_APPLET_NOT_INIT                        = 0x06,   /* sw1 = 0x69 */
  SW2_PUBLIC_KEY_OR_CERT_NOT_INIT            = 0x07,   /* sw1 = 0x69 */
  SW2_WARNING_NFC_C1                         = 0xC1,   /* sw1 = 0x63 */
  SW2_SW1_PROBLEM                            = 0x00    /* sw1 = 0x6F */

} uim_sw2_type;

typedef uint8 uim_channel_type;

/* IMPORTANT: The enumeration and typedef for UIM channels should not be combined
   as this introduces Klocwork errors*/
/* There are a maximum of 20 channels (numbered 0-19) per card
   per spec 102 221 - section 8.7*/
 enum {
    UIM_CHANNEL0 = 0,
    UIM_MAX_CHANNELS = 20
  };

/* Same report for both read_binary and read_record */
typedef struct {
  word  num_bytes_read; /* Number of bytes read from UIM */
  byte *data;           /* Pointer to the data */
}uim_read_rpt_type;

/* Same report for both write_binary and write_record */
typedef struct {
  word  num_bytes_written;
}uim_write_rpt_type;

typedef struct
{
  word  inc_rsp_len;
  byte *data;
}uim_inc_rpt_type;

typedef struct {
  word num_data_bytes;         /* Number of data bytes in the response */
  byte data[UIM_MAX_CHARS];    /* Data */
  uim_sw1_type   get_resp_sw1; /* Status Word 1 */
  uim_sw2_type   get_resp_sw2; /* Status word 2 */
} uim_select_rpt_type;

typedef struct {
  word num_records;            /* USIM specific- number of records that match */
  byte  data[UIM_MAX_CHARS];   /* Record Numbers that match */
} uim_seek_rpt_type;

typedef struct {
  word num_bytes;              /* Number of bytes of data */
  byte data[UIM_MAX_CHARS];    /* Data */
} uim_status_rpt_type;


typedef struct {               /* UIM_ENVELOPE_R */
  word           data_length;           /* Length of data returned */
  byte           data[UIM_MAX_CHARS];   /* Data returned */
  uim_sw1_type   get_resp_sw1;          /* Status Word 1 */
  uim_sw2_type   get_resp_sw2;          /* Status word 2 */
} uim_envelope_rpt_type;

typedef struct {               /* UIM_TERMINAL_RESPONSE_R */
  boolean  proactive_cmd_pending;
} uim_terminal_response_rpt_type;

typedef struct {                      /* UIM_POLLING_INTERVAL_R */
  dword actual_uim_polling_interval; /* The one uim is using */
} uim_polling_interval_rpt_type;



typedef struct {
  word data_length;            /* Length of data returned */
  byte data[UIM_MAX_CHARS];    /* Data returned */
} uim_run_gsm_algo_rpt_type;

typedef struct {               /* UIM_STORE_ESN_ME_R & UIM_STORE_ESN_MEID_ME_R*/
  byte usage_ind_conf;
} uim_store_esn_rpt_type;

typedef struct {               /* UIM_RUN_CAVE_R */
  /* The response from the UIM is actually 3 bytes in size.  dword is used here
     to better facilitate data usage in the ARM (easier to process dword than
     3 byte field). */
  dword auth;
} uim_run_cave_rpt_type;

typedef struct {
  byte cipher_key[UIM_AKA_AUTH_KEY_LEN];
  byte integrity_key[UIM_AKA_AUTH_KEY_LEN];
  byte res_length;
  byte res_data[UIM_MAX_BYTE_SIZE];
} uim_aka_auth_keys_data_type;

typedef struct {               /* UIM_AKA_AUTH_R */
  byte sync_tag;
  union
  {
    uim_aka_auth_keys_data_type     aka_keys;
    byte                            auts[UIM_AKA_AUTH_AUTS_LEN];
  }aka_data;
} uim_aka_auth_rpt_type;

typedef struct {               /* UIM_GENERATE_KEYS_R */
  byte data[UIM_MAX_CHARS];
} uim_generate_keys_rpt_type;

typedef struct {               /* UIM_BS_CHAL_R */
  dword randbs;
} uim_bs_chal_rpt_type;

typedef struct {               /* UIM_COMMIT_R */
  byte result;
} uim_commit_rpt_type;

typedef struct {               /* UIM_MS_KEY_REQ_R */
  byte result;
} uim_ms_key_rpt_type;

typedef struct {               /* UIM_KEY_GEN_REQ_R */
  byte result;
  byte ms_result_len;
  byte ms_result[UIM_PARAM_P];
} uim_key_gen_rpt_type;

typedef struct {               /* UIM_SERV_KEY_GEN_REQ_R */
  byte result;
} uim_serv_key_gen_rpt_type;

typedef struct {               /* UIM_VALIDATE_R */
  byte block_id;
  byte result;
} uim_validate_rpt_type;

typedef struct {               /* UIM_CONFIG_REQ_R */
  byte block_id;
  byte block_length;
  byte result;
  byte data[UIM_MAX_BYTE_SIZE - 3];
} uim_config_rpt_type;

typedef struct {               /* UIM_DOWNLOAD_REQ_R */
  byte block_id;
  byte result;
} uim_download_rpt_type;

typedef struct {               /* UIM_SSPR_CONFIG_REQ_R */
  byte block_id;
  byte result;
  byte block_length;
  byte data[UIM_MAX_BYTE_SIZE - 3];
} uim_sspr_config_rpt_type;

typedef struct {               /* UIM_SSPR_DOWNLOAD_REQ_R */
  byte block_id;
  byte result;
  word segment_offset;
  byte segment_size;
} uim_sspr_download_rpt_type;

typedef struct {               /* UIM_OTAPA_REQ_R */
  byte result;
  byte nam_lock_ind;
  dword rand_otapa;
} uim_otapa_rpt_type;

typedef struct {               /* UIM_COMPUTE_IP_AUTH_R */
  word data_len;
  byte data[UIM_COMPUTE_IP_AUTH_RESPONSE_LENGTH];
} uim_compute_ip_auth_rpt_type;


typedef struct {
  word data_length;            /* Length of data returned */
  byte data[UIM_MAX_CHARS];    /* Data returned */
} uim_authenticate_rpt_type;

typedef struct{
  word  len;
  byte *data;
}uim_authenticate_odd_ins_rpt_type;

typedef struct{
  word  len;
  byte *data;
}uim_bcast_rpt_type;


typedef struct {
  word data_length;            /* Length of data returned */
  byte data[UIM_MAX_CHARS];    /* Data returned */
} uim_isim_authenticate_rpt_type;

typedef struct {
  byte          channel_num;    /* Channel Number */
} uim_manage_ch_rpt_type;

typedef struct {
  word num_data_bytes;         /* Number of data bytes in the response */
  byte data[UIM_MAX_CHARS];    /* Data */
} uim_select_in_chan_rpt_type;

typedef struct {
  word          data_length;    /* Length of Data Returned */
  byte          data[UIM_MAX_CHARS]; /* Data */
} uim_perform_sec_op_rpt_type;

typedef struct {
  word          data_length;    /* Length of data returned */
  byte          data[UIM_MAX_CHARS]; /* Data */
} uim_ask_random_rpt_type;

typedef struct {
  uim_slot_type reset_slot;    /* Slot that was reset */
} uim_reset_rpt_type;

typedef struct {
  word           num_data_bytes;      /* Number of data bytes in the rsp */
  byte           *data_ptr;           /* Data */
  uim_sw1_type   get_resp_sw1;        /* Status Word 1 */
  uim_sw2_type   get_resp_sw2;        /* Status word 2 */
} uim_stream_iso7816_apdu_rpt_type;

typedef struct {
  byte num_bytes;              /* number of bytes in ATR */
  byte data[UIM_MAX_ATR_CHARS];/* buffer to hold the ATR */
} uim_atr_rpt_type;

/* ---------------------------------------------------------------
   STRUCTURE:      uim_mflo_get_subscriber_id_rpt_type

   DESCRIPTION:
     If the operation was successful, this structure contains the
     return data for the
     UIM_MFLO_GET_SUBSCRIBER_ID_F command.
----------------------------------------------------------------*/
typedef struct {
  uint8   subscriber_id [UIM_MFLO_SUBSCRIBER_ID_LEN];
}uim_mflo_get_subscriber_id_rpt_type;

/* ---------------------------------------------------------------
   STRUCTURE:      uim_mflo_get_public_key_rpt_type

   DESCRIPTION:
     If the operation was successful, this structure contains the
     return data for the
     UIM_MFLO_GET_PUBLIC_KEY_F command.
----------------------------------------------------------------*/
typedef struct {
  uint32  len;
  uint8   public_key [UIM_MFLO_PUBLIC_KEY_LEN];
}uim_mflo_get_public_key_rpt_type;

/* ---------------------------------------------------------------
   STRUCTURE:      uim_mflo_sign_ucast_rpt_type

   DESCRIPTION:
     If the operation was successful, this structure contains the
     return data for the
     UIM_MFLO_SIGN_UCAST_F command.
----------------------------------------------------------------*/
typedef struct {
  uint32  ucast_msg_len;
  uint8  *signed_ucast_msg_ptr;
}uim_mflo_sign_ucast_rpt_type;

typedef struct
{
  cmd_hdr_type      rpt_hdr;
    /* Includes the queue links,  pointer to requesting task TCB,
       signal to set on completion,  and done queue. */
  uim_report_code_type rpt_type;     /* What type of report this is */
  uim_rpt_status    rpt_status;      /* whether the access was OK */
  uim_sw1_type      sw1;             /* Status Word 1 */
  uim_sw2_type      sw2;             /* Status word 2 */
  boolean           cmd_transacted;  /* Whether cmd was attempted to the card */
  uint32            user_data;  /* User defined data copied from cmd header */
  union
  {
     uim_read_rpt_type          read; /*Report for read_bin and read_rec */
     uim_write_rpt_type         write; /*Report for write_bin and write_rec */
     uim_inc_rpt_type           inc; /*Report for inc command */
     uim_select_rpt_type        select;
     uim_seek_rpt_type          seek;
     uim_status_rpt_type        status;

     uim_envelope_rpt_type      envelope;
     uim_terminal_response_rpt_type terminal_response;
     uim_polling_interval_rpt_type  polling_interval;

     uim_store_esn_rpt_type     store_esn;
     boolean                    ssd_update_ok;
     uim_run_cave_rpt_type      run_cave;
     uim_aka_auth_rpt_type      aka_auth;
     uim_generate_keys_rpt_type generate_keys;
     uim_bs_chal_rpt_type       bs_chal;
     uim_commit_rpt_type        commit;
     uim_ms_key_rpt_type        ms_key;
     uim_key_gen_rpt_type       key_gen;
     uim_serv_key_gen_rpt_type  serv_key_gen;
     uim_validate_rpt_type      validate;
     uim_config_rpt_type        configuration;
     uim_download_rpt_type      download;
     uim_sspr_config_rpt_type   sspr_config;
     uim_sspr_download_rpt_type sspr_download;
     uim_otapa_rpt_type         otapa;
     uim_compute_ip_auth_rpt_type compute_ip_auth;

     uim_run_gsm_algo_rpt_type  run_gsm;

     uim_authenticate_rpt_type          autn;
     uim_authenticate_odd_ins_rpt_type  autn_odd_ins;
     uim_bcast_rpt_type                 bcast;

     uim_isim_authenticate_rpt_type isim_autn;

    uim_select_in_chan_rpt_type select_in_chan;
    uim_manage_ch_rpt_type      channel;
    uim_perform_sec_op_rpt_type sec_op;
    uim_ask_random_rpt_type     ask_rand;

    uim_reset_rpt_type          reset;

    uim_atr_rpt_type            rst_uim_passive;
    uim_stream_iso7816_apdu_rpt_type stream_iso7816_apdu;

    uim_mflo_get_subscriber_id_rpt_type  mflo_get_subscriber_id;
    uim_mflo_get_public_key_rpt_type     mflo_get_public_key;
    uim_mflo_sign_ucast_rpt_type         mflo_sign_ucast;

  } rpt;
  uim_slot_type     slot;
} uim_rpt_type;

/*--------------------------------------------------------------------------
                               UIM COMMANDS

--------------------------------------------------------------------------*/
/* The commands are grouped as Generic, GSM, CDMA and USIM. They are
   identified  */

/* Command types */
typedef enum {
  UIM_NO_SUCH_COMMAND_F = 0,     /* No such command */
  UIM_INTERNAL_ME_PUP_F = 0x0100,/* POWER UP UIM due to task start up */
  UIM_INTERNAL_WAKE_UP_F,        /* Power up due to Power management */
  UIM_INTERNAL_POLL_F,           /* Internal command to Poll */
  UIM_ACCESS_F,                  /* Read or Write from UIM */
  UIM_SELECT_F,                  /* Select a file/directory from the UIM */
  UIM_MANAGE_CHANNEL_F,          /* Used to Open/Close Logical Channel */
  UIM_SEEK_F,                    /* Seek from the UIM */
  UIM_INVALIDATE_F,              /* Invalidate an EF */
  UIM_REHABILITATE_F,            /* Rehabilitate an EF */
  UIM_VERIFY_CHV_F,              /* Verify CHV */
  UIM_CHANGE_CHV_F,              /* Change CHV */
  UIM_DISABLE_CHV_F,             /* Disable CHV */
  UIM_ENABLE_CHV_F,              /* Enable CHV */
  UIM_UNBLOCK_CHV_F,             /* Unblock CHV */
  UIM_STATUS_F,                  /* Status of the Card */
  UIM_TERMINAL_PROFILE_F,        /* Send terminal profile to UIM */
  UIM_ENVELOPE_F,                /* Send a toolkit command to the UIM */
  UIM_INTERNAL_FETCH_F,          /* Retrieve a pro-active cmd from the UIM */
  UIM_TERMINAL_RESPONSE_F,       /* Send a response to a pro-active command */
  UIM_POLLING_INTERVAL_F,        /* Send a polling command to UIM */
  UIM_RESET_F,                   /* Perform a warm reset and perform power up */
  UIM_STREAM_APDU_F,             /* Used to stream an APDU to the card */
  UIM_STREAM_ISO7816_APDU_F,     /* Used to stream an ISO 7816 APDU */
  UIM_RESET_WITH_TP_F,           /* Warm Reset with optional Terminal Profile */
  UIM_PREF_SLOT_CHANGE_F,        /* Change the pref slot for app specified */
  UIM_RESET_SWITCH_UIM_PASSIVE_F,  /* Warm reset the uim and make UIM passive
                                    from this point on, make UIM passive
                                    allowing only resets and 7816 APDUs only */
  UIM_POWER_UP_UIM_PASSIVE_F,
  UIM_POWER_DOWN_F,
  UIM_READ_BIN_CMD_F,            /* Read from UIM */
  UIM_READ_REC_CMD_F,            /* Read from UIM */
  UIM_WRITE_BIN_CMD_F,           /* Write to UIM */
  UIM_WRITE_REC_CMD_F,           /* Write to UIM */
  UIM_INC_CMD_F,                 /* Increment record */
  UIM_CLEAR_CACHE_F,             /* Command to clear ALL/Selective
                                  * get response cache being maintained by UIM
                                  */
  UIM_RECOVERY_COMPLETE_F,        /* Command to indicate that GSDI has completed
                                  * recovery procedure and to continue
                                  * regular commands
                                  */
  UIM_SILENT_RECOVER_UICC_F,      /* This command is used to recover UICC
                                     from eroneous state for NFC */
  UIM_HOTSWAP_CARD_INS_F,         /* Hotswap card inserted command */
  UIM_HOTSWAP_CARD_REM_F,         /* Hotswap card removed  command */

  UIM_RUN_GSM_ALGO_F    = 0x0200,/* RUN GSM algorithm */

  UIM_STORE_ESN_ME_F    = 0x0300,/* Store ESN_ME */
  UIM_SSD_UPDATE_F,              /* SSD update */
  UIM_CONFIRM_SSD_F,             /* Confirm SSD */
  UIM_RUN_CAVE_F,                /* RUN Cave */
  UIM_AKA_AUTH_F,                /* AKA Authentication */
  UIM_GENERATE_KEYS_F,           /* Generate Keys */
  UIM_BS_CHAL_F,                 /* Base station challenge */
  UIM_MS_KEY_REQ_F,              /* MS Key Request */
  UIM_KEY_GEN_REQ_F,             /* Key Generation Request */
  UIM_SERV_KEY_GEN_REQ_F,        /* Service Key Generation Request */
  UIM_COMMIT_F,                  /* OTASP Commit request*/
  UIM_VALIDATE_F,                /* Validation request */
  UIM_CONFIG_REQ_F,              /* Configuration Request */
  UIM_DOWNLOAD_REQ_F,            /* Download Request */
  UIM_SSPR_CONFIG_REQ_F,         /* SSPR Configuration Request */
  UIM_SSPR_DOWNLOAD_REQ_F,       /* SSPR Download Request */
  UIM_OTAPA_REQ_F,               /* OTAPA request */
  UIM_COMPUTE_IP_AUTH_F,         /* Peform 3GPD Hash or Authentication */
  UIM_STORE_ESN_MEID_ME_F,       /* Store ESN_MEID_ME */

  UIM_AUTHENTICATE_F    = 0x0400,/* Authenticate for USIM */
  UIM_AUTHENTICATE_ODD_INS_F,
  UIM_BCAST_F,

  UIM_ISIM_AUTHENTICATE_F = 0x0500,/* Authenticate for ISIM */

  UIM_MFLO_INIT_ACT_PARAMS_F = 0x0700, /* Initialize the Activation Parameters */
  UIM_MFLO_GET_SUBSCRIBER_ID_F,        /* Retrieve the Subscriber ID */
  UIM_MFLO_GET_PUBLIC_KEY_F,           /* Retrieve the Public Key */
  UIM_MFLO_SIGN_UCAST_F,               /* Sign the Unicast Message */
  UIM_MFLO_VERIFY_UCAST_F,             /* Verify the Unicast Message or Signature */

  UIM_MAX_F
} uim_cmd_name_type;

typedef enum {
  UIM_CHV_NONE  = 0,
  UIM_CHV1      = 1,             /* CHV1 */
  UIM_CHV2      = 2,             /* CHV2 */
  UIM_PIN1_APP1 = 3,             /* Pin1 for APPLICATON 1 */
  UIM_PIN1_APP2 = 4,             /* Pin1 for APPLICATON 2 */
  UIM_PIN1_APP3 = 5,             /* Pin1 for APPLICATON 3 */
  UIM_PIN1_APP4 = 6,             /* Pin1 for APPLICATON 4 */
  UIM_PIN1_APP5 = 7,             /* Pin1 for APPLICATON 5 */
  UIM_PIN1_APP6 = 8,             /* Pin1 for APPLICATON 6 */
  UIM_PIN1_APP7 = 9,             /* Pin1 for APPLICATON 7 */
  UIM_PIN1_APP8 = 10,            /* Pin1 for APPLICATON 8 */

  UIM_PIN2_APP1 = 11,            /* Pin2 for APPLICATON 1 */
  UIM_PIN2_APP2 = 12,            /* Pin2 for APPLICATON 2 */
  UIM_PIN2_APP3 = 13,            /* Pin2 for APPLICATON 3 */
  UIM_PIN2_APP4 = 14,            /* Pin2 for APPLICATON 4 */
  UIM_PIN2_APP5 = 15,            /* Pin2 for APPLICATON 5 */
  UIM_PIN2_APP6 = 16,            /* Pin2 for APPLICATON 6 */
  UIM_PIN2_APP7 = 17,            /* Pin2 for APPLICATON 7 */
  UIM_PIN2_APP8 = 18,            /* Pin2 for APPLICATON 8 */
  UIM_UNIVERSAL_PIN = 19,        /* Universal Pin */
  UIM_CHV_MAX   = 20
} uim_chv_type;

typedef enum {
  UIM_MFLO_UCAST_MSG_FIRST_BLOCK  = 0x00,
  UIM_MFLO_UCAST_MSG_NEXT_BLOCK   = 0x01,
  UIM_MFLO_UCAST_MSG_LAST_BLOCK   = 0x10,
  UIM_MFLO_UCAST_MSG_SINGLE_BLOCK = 0x11
  // Any other value is invalid
}uim_mflo_ucast_msg_enum_type;

typedef enum {
  UIM_NO_SUCH_STATUS_S,          /* No such status */
  UIM_INVALID_S,                 /* Recd the command in an invalid State */
  UIM_RECEIVED_S,                /* Recd the command */
  UIM_UNINITIALIZED_S,           /* UIM is not initialized yet */
  UIM_POWERED_DOWN_S,            /* UIM is powered down */
  UIM_CLK_OFF_S,                 /* UIM does not have a clock */
  UIM_INITIALIZED_S,             /* UIM is initialized */
  UIM_ERR_S,                     /* UIM is faulty */
  UIM_PROTOCOL_ERR_S,            /* Protocol in the command is invalid */
  UIM_POLL_ERR_S,               /* Poll error */
  UIM_RESET_INIT_S,              /* Internal reset is about to start */
  UIM_RESET_COMPLETE_S,          /* Internal reset is finished */
  UIM_NO_ATR_RECEIVED_WITH_MAX_VOLTAGE_S,
  UIM_NO_ATR_RECEIVED_AFTER_INT_RESET_S,
  UIM_CORRUPT_ATR_RCVD_MAX_TIMES_S,
  UIM_PPS_TIMED_OUT_MAX_TIMES_S,
  UIM_VOLTAGE_MISMATCH_S,
  UIM_INTERNAL_CMD_TIMED_OUT_AFTER_PPS_S,
  UIM_INT_CMD_ERR_EXCEED_MAX_ATTEMPTS_S,
  UIM_MAXED_PARITY_ERROR_S,
  UIM_MAXED_RX_BREAK_ERROR_S,
  UIM_MAXED_OVERRUN_ERROR_S,
  UIM_TRANSACTION_TIMER_EXPIRED_S,
  UIM_POWER_DOWN_CMD_NOTIFICATION_S,
  UIM_INT_CMD_ERR_IN_PASSIVE_MODE_S,
  UIM_CMD_TIMED_OUT_IN_PASSIVE_MODE_S,
  UIM_MAX_PARITY_IN_PASSIVE_S,
  UIM_MAX_RXBRK_IN_PASSIVE_S,
  UIM_MAX_OVERRUN_IN_PASSIVE_S,
  UIM_CARD_REMOVED_S,
  UIM_TECHNICAL_PROBLEM_S,
  UIM_NULL_BYTE_ERR_S,
  UIM_MAXIMUM_S
} uim_status_type;

typedef enum {
  UIM_FLUSH_ONLY,        /* Flush only this command from the Q */
  UIM_FLUSH_ALL          /* Flush all the commands in the q */
} uim_flush_type;

/* Aggregation of Traffic channel voting bits */
#define UIM_CDMA_TRAF_CHAN (UIM_CDMA_TC)
#define UIM_GW_TRAF_CHAN ( (int)UIM_TC | (int)UIM_GW_TC)
#define UIM_TRAF_CHAN ( (int)UIM_CDMA_TRAF_CHAN | UIM_GW_TRAF_CHAN )

/* Type for holding data about proactive UIM */
typedef struct {
  boolean proactive_uim;
  boolean init;
} uim_proactive_uim_data_type;

/* Indicate command handling options */
typedef enum {
  UIM_OPTION_NONE           = 0x0,        /* No options used */
  UIM_OPTION_ALWAYS_RPT     = 0x1,        /* Always report   */
  UIM_OPTION_RECOVERY_CMD   = 0x2        /* Recovery command */
} uim_option_type;

/* Indication for presence of CDMA DF and/or GSM DF. */
typedef struct {
  uim_protocol_type protocol;       /* UICC or GSM protocol */
  boolean cdma_df_present;          /* Presence of CDMA DF */
  boolean gsm_df_present;           /* Presence of GSM  DF */
} uim_dfs_present_type;

typedef enum{
  UIM_NO_ACTION,                    /* Nothing to be done */
  UIM_SWITCH_SLOT_REQ,              /* Flag to control that we need to switch to inactive
                                       slot after powering down the active slot */
  UIM_NO_NOTIFICATION_TO_QMI_REQ,   /* This flag indicates that we don't have to notify qmi
                                       as we don't have latest slot status data */
  UIM_NO_MASK_HOTSWAP_INT_REQ,      /* This flag use when we don't have to mask hotswap interrupts
                                       while powering down active slot */
}uim_cmd_req_type;

/* The common Header for all the commands */
typedef struct {
  cmd_hdr_type          cmd_hdr;       /* command header */
  uim_cmd_name_type     command;       /* The command */
  uim_status_type       status;        /* Status of the R-UIM */
  uim_option_type       options;       /* Indicate command handling options */
  uim_protocol_type     protocol;      /* Protocol of the command */
  uim_slot_type         slot;          /* Slot to execute the command */
  void                  (*rpt_function)( uim_rpt_type * );
                                  /* Pointer to Function to send report */
  byte                  cmd_count;
  byte                  cmd_recovery_count; /* To keep count of recovery for this command */
  uint32                user_data;     /* User defined data */
  uim_channel_type      channel;
  uim_cmd_req_type      req_type;
} uim_hdr_type;

/* ACCESS_F command
   external dependency in 1x-auth_v.h */

typedef enum {
  UIM_READ_F     = 0,    /* Read item from UIM */
  UIM_WRITE_F    = 1,    /* Write item to UIM */
  UIM_INC_F      = 2     /* Increment item in UIM */
} uim_access_type;

typedef enum {
  UIM_CURRENT    = 0, /* Current mode */
  UIM_NEXT       = 1, /* access the next record in a record structure */
  UIM_PREVIOUS   = 2, /* access the previous record in a record structure */
  UIM_ABSOLUTE   = 3  /* Absolute mode of record access */
} uim_record_mode_type;

typedef enum {
  UIM_ACTIVATION_OR_RESET            = 0, /* Activation/reset of UICC App with FCP data returned */
  UIM_TERMINATION                    = 1, /* Termination of UICC App */
  UIM_ACTIVATION_WITH_NO_DATA        = 2, /* Activation with no FCI data returned */
  UIM_ACTIVATION_WITH_FMD            = 3, /* Activation with FMD data returned */
  UIM_ACTIVATION_WITH_FCI            = 4, /* Activation with FCI data returned */
  UIM_ACTIVATION_WITH_FCI_INTERFACES = 5  /* Activation with FCI interfaces data returned */
} uim_session_control_type;

typedef enum {
  UIM_FIRST_OR_ONLY_OCCURRENCE       = 0x00,
  UIM_LAST_OCCURRENCE                = 0x01,
  UIM_NEXT_OCCURRENCE                = 0x02,
  UIM_PREVIOUS_OCCURRENCE            = 0x03
}uim_adf_select_mode_type;

typedef struct {
  byte data[UIM_AID_MAX_NAME_LENGTH];
  byte aid_length;
} uim_aid_type;

typedef struct {
  uint8 len;
  word  path[UIM_DIRMAIN_MAX_PATH_ELEMENTS];
}uim_path_type;

typedef struct {
   boolean       is_needed;
   uim_path_type path;
   uim_aid_type  aid;
}uim_intermediate_select_type;

typedef struct
{
  uim_hdr_type        hdr;    /* Command header */
  uim_items_enum_type item;   /* Item to be accessed */
  uim_path_type       path;   /* Path for accessing an item */
  word                offset; /* Offset to write from */
  uim_aid_type        aid;    /* AID for UICC */
  word                len;    /* Length of data to be written */
  byte               *data;   /* Pointer to data to be written */
  uint8               sfi;    /* Short File Identifier if available */
}uim_write_bin_cmd_type;

typedef struct
{
  uim_hdr_type          hdr;     /* Command header */
  uim_items_enum_type   item;    /* Item to be accessed */
  uim_path_type         path;    /* Path for accessing an item */
  uim_record_mode_type  recmode; /* mode for record access */
  byte                  rec_num; /* Record number to write to */
  uim_aid_type          aid;     /* AID for UICC */
  word                  len;     /* length of data to be written */
  byte                 *data;    /* Datat to write */
  uint8                 sfi;     /* Short File Identifier if available */
}uim_write_rec_cmd_type;

typedef struct
{
  uim_hdr_type        hdr;    /* Command header */
  uim_items_enum_type item;   /* Item to be accessed */
  uim_path_type       path;   /* Path for accessing an item */
  word                offset; /* Offset to read from */
  word                len;    /* Length of data to be read */
  uim_aid_type        aid;    /* AID for UICC */
  uint8               sfi;    /* Short File Identifier if available */
}uim_read_bin_cmd_type;

typedef struct
{
  uim_hdr_type         hdr;     /* Command header */
  uim_items_enum_type  item;    /* Item to be accessed */
  uim_path_type        path;    /* Path for accessing an item */
  uim_record_mode_type recmode; /* mode for record access */
  byte                 rec_num; /* Record number to read from */
  word                 rec_len; /* Record length to read */
  uim_aid_type         aid;     /* AID for UICC */
  uint8                sfi;     /* Short File Identifier if available */
}uim_read_rec_cmd_type;


typedef struct
{
  uim_hdr_type        hdr;     /* Command header */
  uim_items_enum_type item;    /* Item to be accessed */
  uim_path_type       path;    /* Path for accessing an item */
  uim_aid_type        aid;     /* AID for UICC */
  word                len;     /* Length of data to be written */
  byte               *data;    /* Pointer to data to be written */
  uint8               sfi;     /* Short File Identifier if available */
}uim_inc_cmd_type;

typedef  unsigned short     uim_dir_type;      /* Unsigned 16 bit value */

typedef struct {
  uim_sw1_type         sw1;
  uim_sw2_type         sw2;
  int                  cmd_status;
  boolean              is_failed_select_reselect;
}uim_failed_select_reselect_type;

typedef struct {
  uim_hdr_type             hdr;       /* Command header */
  uim_file_type            file_type; /* File type to be selected */
  uim_dir_type             dir;       /* MF,DF to be accessed if file type is MF ,DF */
  uim_items_enum_type      item;      /* EF to be accessed if file type is EF*/
  uim_path_type            path;      /* Path for selecting an item */
  uim_aid_type             aid;       /* AID for UICC */
  uim_session_control_type session;    /* session control for UICC */
  boolean                  is_intermediate_sel;
  uint8                    path_position; /* index in the path for ICC selects */
  uim_failed_select_reselect_type reselect;
  uim_adf_select_mode_type adf_selection; /* Governs the ADF selection mode */
} uim_select_cmd_type;

/* SEEK_F command */
typedef enum {
  UIM_SEEK_TYPE_1 = UIM_P2_SEEK_TYPE_1,  /* Seek Type 1 */
  UIM_SEEK_TYPE_2 = UIM_P2_SEEK_TYPE_2   /* Seek Type 2 */
} uim_seek_type;

typedef enum {
  UIM_SEEK_BEGINNING_FORWARDS = UIM_P2_SEEK_BEGINNING_FORWARDS,
  UIM_SEEK_END_BACKWARDS      = UIM_P2_SEEK_END_BACKWARDS,
  UIM_SEEK_NEXT_LOCN_FORWARDS = UIM_P2_SEEK_NEXT_LOCN_FORWARDS,
  UIM_SEEK_PREV_LOCN_BACKWARDS= UIM_P2_SEEK_PREV_LOCN_BACKWARDS
} uim_seek_mode_type;

typedef enum {
   UIM_SRCH_P1_FORWARD  = UIM_SRCH_IND_P1_FORWARD,
   UIM_SRCH_P1_BACKWARD = UIM_SRCH_IND_P1_BACKWARD,
   UIM_SRCH_NEXT        = UIM_SRCH_IND_NEXT,
   UIM_SRCH_PREVIOUS    = UIM_SRCH_IND_PREV
} uim_srch_record_mode_type;

typedef enum {
   UIM_SRCH_ABSOLUTE  = UIM_SRCH_IND_ABS,
   UIM_SRCH_CHARACTER = UIM_SRCH_IND_CHAR
} uim_srch_offset_type;

typedef struct {
 /* Generic search/seek fields */
  uim_hdr_type        hdr;                         /* Command header */
  uim_items_enum_type item;                        /* EF to be searched */
  uim_path_type       path;           /* Path for accessing an item */
  byte                srch_pattern[UIM_MAX_SRCH_PATTERN_LEN];/* Search string */
  byte                num_bytes;                   /* Number of bytes in search pattern */
                                  /* Max 16 for 11.11 */

  /* For remotability purposes, the seek and search specific fields are not
     featurized */

  /* 11.11 SEEK specific fields */
  uim_seek_type         seek_type;    /* Type 1 or Type 2 Seek */
  uim_seek_mode_type    mode;         /*  Mode used to SEEK */

  /* USIM specific SRCH fields */
  uim_aid_type          aid;          /* AID for USIM */
  uim_srch_mode         srch_mode;    /* Simple/enhanced Search */
  byte                  rec_number;   /* Record Number to search from */
  uim_srch_dir_type     srch_dir;     /* Search types */

  /* Enhanced Mode specific */
  uim_srch_offset_type  offset_type;
                          /* offset as absolute position or character */
  byte                  offset;
                          /* Offset within the record to begin search from */
  uim_srch_record_mode_type  srch_rec_mode_type;  /* Record Mode to search */
  uint8                      sfi;     /* Short File Identifier if available */
} uim_seek_cmd_type;

/* UIM_INVALIDATE_F command */

typedef struct {
  uim_hdr_type   hdr;              /* Command header */
  uim_items_enum_type item;        /* EF to invalidate */
  uim_path_type   path;           /* Path for accessing an item */
  uim_aid_type   aid;              /* AID for UICC */
} uim_invalidate_cmd_type;

/* UIM_REHABILITATE_F command */

typedef struct {
  uim_hdr_type   hdr;              /* Command header */
  uim_items_enum_type item;        /* EF to invalidate */
  uim_path_type   path;           /* Path for accessing an item */
  uim_aid_type   aid;              /* AID for UICC */
} uim_rehabilitate_cmd_type;

/* UIM_VERIFY_CHV_F command */

typedef struct {
  uim_hdr_type   hdr;              /* Command header */
  uim_chv_type   chv;              /* CHV1 or CHV2 */
  byte           chv_digits[UIM_MAX_CHV_DIGITS];      /* CHV digits */
  uim_aid_type   aid;              /* AID for UICC */
} uim_verify_chv_cmd_type;

/* UIM_CHANGE_CHV_F command */

typedef struct {
 uim_hdr_type   hdr;               /* Command header */
 uim_chv_type   chv;               /* CHV1 or CHV2 */
 byte           chv_digits[UIM_MAX_CHV_DIGITS];      /* CHV digits */
 byte           new_chv_digits[UIM_MAX_CHV_DIGITS];  /* New CHV digits */
 byte           param_p2;          /* P2 for a USIM Verify */
 uim_aid_type   aid;               /* AID for UICC */
} uim_change_chv_cmd_type;

/* UIM_DISABLE_CHV_F command */

typedef struct {
 uim_hdr_type   hdr;                    /* Command header */
 uim_chv_type   chv;                    /* CHV1 or CHV2 */
 byte           chv_digits[UIM_MAX_CHV_DIGITS];  /* CHV digits */
 byte           param_p2;               /* P2 for a USIM Verify */
 uim_aid_type   aid;                    /* AID for UICC */
} uim_disable_chv_cmd_type;

/* UIM_ENABLE_CHV_F command */

typedef struct {
 uim_hdr_type   hdr;                    /* Command header */
 uim_chv_type   chv;                    /* CHV1 or CHV2 */
 byte           chv_digits[UIM_MAX_CHV_DIGITS];  /* CHV digits */
 byte           param_p2;               /* P2 for a USIM Verify */
 uim_aid_type   aid;                    /* AID for UICC */
} uim_enable_chv_cmd_type;

/* UIM_UNBLOCK_CHV_F command */

typedef struct {
  uim_hdr_type   hdr;                   /* Command header */
  uim_chv_type   chv;                   /* CHV1 or CHV2 */
  byte           chv_digits[UIM_MAX_CHV_DIGITS];      /* CHV digits */
  byte           new_chv_digits[UIM_MAX_CHV_DIGITS];  /* New CHV digits */
  byte           param_p2;              /* P2 for a USIM Verify */
  uim_aid_type   aid;                    /* AID for UICC */
} uim_unblock_chv_cmd_type;

/* Internal command for power up */
typedef struct {
  uim_hdr_type   hdr;        /* Command header */
} uim_int_type;

typedef enum {
  UIM_NO_INDICATION = UIM_STATUS_P1_NO_INDICATION,
  UIM_CURR_APP_INITIALIZED = UIM_STATUS_P1_CURR_APP_INIT,
  UIM_TERMINATION_OF_CURR_APP = UIM_STATUS_P1_TERMN_OF_CURR_APP
} uim_status_indication_type;

typedef enum {
  UIM_RETURN_RSP_IDENTICAL_TO_SELECT = UIM_STATUS_P2_RSP_SAME_AS_SELECT,
  UIM_RETURN_DF_NAME_OF_CURR_APP = UIM_STATUS_P2_DF_NAME_OF_CURR_APP,
  UIM_RETURN_NONE = UIM_STATUS_P2_RETURN_NONE
} uim_status_return_data_type;

/* UIM_STATUS_F command */
typedef struct {
  uim_hdr_type   hdr;        /* Command header */
  uim_status_indication_type status;
  uim_status_return_data_type return_data;
  uim_aid_type     aid;        /* AID for UICC */
} uim_status_cmd_type;


/* DF type */
typedef enum {
  UIM_DF_CDMA_TYPE = 0x7F25,
  UIM_DF_GSM_TYPE  = 0x7F20
} uim_df_type;

/* UIM_TERMINAL_PROFILE_F command */

typedef struct {
  uim_hdr_type   hdr;               /* Command header */
  byte           num_bytes;         /* Number of bytes in profile */
  uim_df_type    df;                /* DF Identifier */
  byte data[UIM_TERMINAL_PROFILE_DATA_LENGTH]; /* Terminal Profile */
} uim_terminal_profile_cmd_type;

/* UIM_ENVELOPE_F command */

typedef struct {
  uim_hdr_type hdr;                 /* Command header */
  byte         num_bytes;                    /* Number of bytes in envelope cmd*/
  byte         data[UIM_MAX_ENVELOPE_BYTES]; /* Envelope command */
  byte         offset;              /* Offset of envelope command in buffer */
} uim_envelope_cmd_type;

/* UIM_INTERNAL_FETCH_F command */

typedef struct {
  uim_hdr_type hdr;                 /* Command header */
  byte         command_size;        /* Proactive command size */
} uim_fetch_cmd_type;

/* UIM_TERMINAL_RESPONSE_F comamnd */

typedef struct {
  uim_hdr_type hdr;                   /* Command header */
  byte         num_bytes;               /* Number of bytes in Term response*/
  byte         data [UIM_MAX_TR_BYTES]; /* Terminal_response */
} uim_terminal_response_cmd_type;

typedef enum {               /* POLLING INTERVAL OR POLLING OFF */
  POLLING_INTERVAL = 0,
  POLLING_OFF =1
} uim_polling_cmd_enum_type;

typedef struct {
  uim_hdr_type hdr;                       /* Command header */
  uim_polling_cmd_enum_type command_type; /* Polling interval or polling off */
  uint32  polling_interval_in_ms;
} uim_polling_interval_cmd_type;

typedef struct {
  uim_hdr_type          hdr;             /* Command header */
  boolean               terminal_profile;/* Optional terminal prof dload */
  boolean               skip_gsm_df;     /* Prohibit GSM accesses on reset */
} uim_reset_with_tp_cmd_type;

/* Store ESN of ME */
typedef struct {                     /* UIM_STORE_ESN_ME_F */
  uim_hdr_type    hdr;               /* Command header */
  byte            len_and_usage;     /* Length and usage of ESN */
  dword           esn;               /* ESN */
} uim_store_esn_cmd_type;

typedef struct {                     /* UIM_STORE_ESN_ME_F */
  uim_hdr_type    hdr;               /* Command header */
  byte            len_and_usage;     /* Length and usage of MEID */
  qword           meid;              /* MEID */
  boolean         meid_available;    /* Can MEID be read from NV */
} uim_store_meid_cmd_type;

/* Update SSD command */
typedef struct {                     /* UIM_SSD_UPDATE_F */
  uim_hdr_type    hdr;               /* Command header */
  qword           randssd;           /* 56 bits from BS for SSD Generation */
  byte            process_control;   /* process control bits */
  dword           esn;               /* ESN ( 7 bytes per standard ) */
} uim_ssd_update_cmd_type;

/* Confirm SSD update */
typedef struct {                     /* UIM_CONFIRM_SSD_F */
  uim_hdr_type    hdr;               /* Command header */
  dword           authbs;            /* Auth Signature from Base Station */
} uim_confirm_ssd_cmd_type;

/* Run CAVE command */
typedef struct {                     /* UIM_RUN_CAVE_F */
  uim_hdr_type    hdr;               /* Command header */
  byte            rand_type;         /* Rand/Randu     */
  dword           rand_chal;         /* 32 bits from BS for Auth Signature */
  byte            dig_len;           /* digit length expressed in bits */
  byte            digits[UIM_RUN_CAVE_DIGITS];
                                     /* maximum of 6 BCD coded digits */
  byte            process_control;   /* process control bits */
  qword           esn;               /* ESN ( 7 bytes per standard ) */
} uim_run_cave_cmd_type;

typedef enum
{
  UIM_AKA_AUTH_NO_OP_TYPE,
  UIM_AKA_AUTH_3G,
  UIM_AKA_AUTH_WLAN,
  UIM_AKA_AUTH_MAX_ENUM
} uim_aka_auth_op_enum_type;

/* AKA Auth command */
typedef struct {                          /* UIM_AKA_AUTH_F */
  uim_hdr_type                hdr;        /* Command header */
  uim_aka_auth_op_enum_type   op_type;    /* Operation Type */
  byte                        randa[UIM_AKA_AUTH_RANDA_LEN]; /* RANDA */
  byte                        autn_len;   /* AUTN length */
  byte                        autn[UIM_MAX_AKA_AUTH_BYTES];  /* AUTN */
} uim_aka_auth_cmd_type;

/* Generate CMEA key and VPM */
typedef struct {                     /* UIM_GENERATE_KEYS_F */
  uim_hdr_type    hdr;               /* Command header */
  byte            vpm_first_octet;   /* First octet of VPM to be output */
  byte            vpm_last_octet;    /* Last  octet of VPM to be output */
} uim_generate_keys_cmd_type;

/* Base Station Challenge command */
typedef struct {                     /* UIM_BS_CHAL_F */
  uim_hdr_type    hdr;               /* Command header */
  dword           randseed;          /* Random number generator seed */
} uim_bs_chal_cmd_type;

/* Commit command */
typedef struct {                     /* UIM_COMMIT_F */
  uim_hdr_type    hdr;               /* Command header */
} uim_commit_cmd_type;

/* Generate public key command */
typedef struct {                     /* UIM_MS_KEY_REQ_F */
  uim_hdr_type    hdr;               /* Command header */
  byte      randseed[UIM_RAND_SEED]; /* Seed used to generate TRUE random no */
  byte      a_key_p_rev;             /* A-KEY-P_REV supported by BS */
  byte      param_p_len;             /* Length of Parameter P */
  byte      param_g_len;             /* Length of Parameter G */
  byte      param_p[UIM_PARAM_P];    /* Param P */
  byte      param_g[UIM_PARAM_G];    /* Param G */
} uim_ms_key_cmd_type;

/* Key generation request */
typedef struct {                     /* UIM_KEY_GEN_REQ_F */
  uim_hdr_type    hdr;               /* Command header */
  byte      bs_result_len;           /* BS Result length */
  byte      bs_result[UIM_PARAM_P];  /* BS Result */
} uim_key_gen_cmd_type;

/* Service Key generation request */
typedef struct {                           /* UIM_SERV_KEY_GEN_REQ_F */
  uim_hdr_type    hdr;                     /* Command header */
  byte            key_id[UIM_SERV_KEY_GEN_ID_SIZE]; /* Key ID Bitmap */
} uim_serv_key_gen_cmd_type;

/* Validate command */
typedef struct {                     /* UIM_VALIDATE_F */
  uim_hdr_type    hdr;               /* Command header */
  byte      block_id;                /* Block ID of the block */
  byte      block_length;            /* Length of the block */
  byte      data[UIM_MAX_VALIDATION_REQ_BYTES];  /* data in the block */
} uim_validation_cmd_type;

/* Configuration request */
typedef struct {                     /* UIM_CONFIG_REQ_F */
  uim_hdr_type    hdr;               /* Command header */
  byte      block_id;                /* Block ID of the block */
} uim_config_cmd_type;

/* Download request */
typedef struct {                     /* UIM_DOWNLOAD_REQ_F */
  uim_hdr_type    hdr;               /* Command header */
  byte      block_id;                /* Block ID of the block */
  word      block_len;               /* Length of the block */
  byte      data[UIM_MAX_CHARS];     /* data in the block */
} uim_download_cmd_type;

/* SSPR configuration request */
typedef struct {                     /* UIM_SSPR_CONFIG_REQ_F */
  uim_hdr_type    hdr;               /* Command header */
  byte      block_id;                /* Block ID of the block */
  word      req_offset;              /* Requested offset in the PRL */
  byte      req_max_size;            /* Requested Max size in the response */
} uim_sspr_config_cmd_type;

/* SSPR Download request */
typedef struct {                     /* UIM_SSPR_DOWNLOAD_REQ_F */
  uim_hdr_type    hdr;               /* Command header */
  byte      block_id;                /* Block ID of the block */
  byte      block_len;               /* Length of the block */
  byte      data[UIM_MAX_DLOAD_REQ_BYTES]; /* data in the block */
} uim_sspr_download_cmd_type;

/* OTAPA command */
typedef struct {                     /* UIM_OTAPA_REQ_F */
  uim_hdr_type    hdr;               /* Command header */
  byte            start_stop;        /* OTAPA Start Stop */
  dword           randseed;          /* Random seed */
  boolean         meid_available;    /* Available allocated and activated */
  dword           esn;               /* ESN or Pseudo ESN */
} uim_otapa_cmd_type;

typedef enum {
  UIM_CMPT_IP_CHAP       = 0,       /* Perform CHAP operation */
  UIM_CMPT_IP_MN_HA_AUTH = 1,      /* Calculate MN-HA Authenticator */
  UIM_CMPT_IP_MIP_RRQ    = 2,       /* Calculate MIP-RRQ Hash */
  UIM_CMPT_IP_MN_AAA_AUTH = 3,       /* Calculate MN-AAA Authenticator */
  UIM_CMPT_IP_HRPD_AN_CHAP = 4
} uim_compute_ip_operation_enum_type;

typedef struct {
  byte  chap_id;                    /* CHAP ID */
  byte  nai_entry_index;            /* NAI Entry Index */
  byte  chap_challenge_length;      /* Challenge Length */
  byte  chap_challenge[UIM_MAX_3GPD_CHAP_CHALLENGE_LENGTH]; /* Chap Challenge */
} uim_compute_ip_data_chap_type;

typedef struct {
  byte  nai_entry_index;            /* NAI Entry Index */
  word  registration_data_length;   /* Registration Length */
  byte  registration_data[UIM_MAX_3GPD_MN_HA_REG_DATA_LENGTH];
                                    /* Registration Data */
} uim_compute_ip_data_mn_ha_type;

typedef struct {
  word  rrq_data_length;                             /* Preceding RRQ Length */
  byte  rrq_data[UIM_MAX_3GPD_HASH_RRQ_DATA_LENGTH]; /* Prec MIP-RRQ Data */
} uim_compute_ip_data_mip_rrq_hash_type;

typedef struct {
  byte  nai_entry_index;            /* NAI Entry Index */
  byte  challenge_length;           /* Challenge Length */
  byte  challenge[UIM_3GPD_MIP_CHALLENGE_LENGTH];   /* Challenge */
} uim_compute_ip_data_mn_aaa_type;

typedef struct {
  byte  chap_id;                    /* CHAP ID */
  byte  chap_challenge_length;      /* Challenge Length */
  byte  chap_challenge[UIM_MAX_HRPD_CHAP_CHALLENGE_LENGTH]; /* Chap Challenge */
} uim_compute_ip_data_hrpd_type;

typedef union {
  uim_compute_ip_data_chap_type         chap_data;     /* CHAP Operation */
  uim_compute_ip_data_mn_ha_type        mn_ha_data;    /* MN-HA Operation */
  uim_compute_ip_data_mip_rrq_hash_type mip_rrq_data;  /* RRQ Hash Operation */
  uim_compute_ip_data_mn_aaa_type       mn_aaa_data;   /* MN-AAA Operation */
  uim_compute_ip_data_hrpd_type         hrpd_data;     /* HRPD Operation */
} uim_compute_ip_data_type;

typedef struct {
  uim_hdr_type                       hdr;               /* Command Header */
  uim_compute_ip_operation_enum_type cmpt_ip_operation; /* Operaion Type */
  uim_compute_ip_data_type           cmpt_ip_data;      /* Data for Command */
} uim_compute_ip_auth_cmd_type;

/* Types of return values for 3GPD control check from Data Services */
typedef enum {
  UIM_3GPD_MIP_RUIM_SIP_RUIM=0,    /* ME should use R-UIM for 3GPD */
  UIM_3GPD_MIP_NV_SIP_NV=1,      /* ME should use NV/SW for 3GPD  */
  UIM_3GPD_MIP_RUIM_SIP_NV=2,     /* ME only supports MIP 3GPD */
  UIM_3GPD_MIP_NV_SIP_RUIM=3      /* ME only supports SIP 3GPD */
} uim_3gpd_support_status;

/* Types of return values for HRPD control check from Data Services */
typedef enum {
  UIM_AN_HRPD_SUPPORT=0,         /* ME should use R-UIM for HRPD   */
  UIM_AN_HRPD_NO_SUPPORT=1,      /* ME should use NV/SW for HRPD  */
  UIM_AN_HRPD_USE_CAVE=2         /* ME should use Cave Algo for HRPD*/
} uim_an_hrpd_support_status;

/* RUN_GSM_ALGORITHM_F command */

typedef struct {
  uim_hdr_type   hdr;        /* Command header */
  byte rand[16];             /* Input Rand */
} uim_run_gsm_algo_type;

typedef enum {
 UIM_AUTN_NO_INFO_GIVEN,
 UIM_AUTN_GLOBAL_REF_DATA,
 UIM_AUTN_SPECIFIC_REF_DATA
}uim_autn_ref_data_type;

typedef struct {
  uim_hdr_type            hdr;             /* Command header */
  uim_aid_type            aid;             /* AID for USIM */
  byte                    ref_data_number; /* Reference data number */
  uim_autn_ref_data_type  ref_data_type;   /* Data type of reference */
  byte                    num_bytes;       /* Number of data bytes */
  byte                    data[UIM_MAX_AUTH_BYTES]; /* Application specific Input */
} uim_authenticate_type;

typedef struct {
  uim_hdr_type   hdr;             /* Command header */
  uim_aid_type   aid;             /* AID for USIM */
  byte           ref_data_number; /* Reference data number */
  uim_autn_ref_data_type ref_data_type; /* Data type of reference */
  word           len;             /* length of data  */
  byte           *data;           /* Auth Data */
} uim_authenticate_odd_ins_type;


typedef struct {
  uim_hdr_type   hdr;             /* Command header */
  uim_aid_type   aid;             /* AID for USIM */
  byte           ref_data_number; /* Reference data number */
  word           len;             /* length of data  */
  byte           *data;           /* Auth Data */
} uim_bcast_type;


typedef enum {
  UIM_MC_OPEN   = 0,    /* Manage Channel Open */
  UIM_MC_CLOSE  = 1 /* Manage Channel Close */
}uim_mc_action_type;

/* Manage Channel Command */
typedef struct {
  uim_hdr_type  hdr;            /* Command Header */
  byte          channel;         /* Channel to perform the operation in */
  byte          op_close_channel;/* Channel to open or close */
  uim_mc_action_type mc_action;  /* Manage Channel Open or Close */
} uim_manage_channel_cmd_type;

/* Select in Channel Command */
typedef struct {
  uim_hdr_type     hdr;        /* Command header */
  byte             channel;    /* Logical channel to use for the command */
  uim_file_type    file_type;  /* File type to be selected */
  uim_dir_type     dir;        /* MF,DF to be accessed if file type is MF ,DF */
  uim_items_enum_type item;    /* EF to be accessed if file type is EF*/
  uim_aid_type     aid;        /* AID to be selected if type is App */
  uim_path_type    path;       /* Path for accessing an item */
} uim_select_in_chan_cmd_type;

/* Verify command */
typedef struct {
  uim_hdr_type      hdr;        /* Command Header */
  byte              channel;    /* Logical channel to use for the command */
  byte              ref_qual;   /* Qualifier of Reference Data */
  byte              ver_data[UIM_MAX_CHV_DIGITS];   /* Verification Data */
} uim_verify_in_chan_cmd_type;

/* Enable Verification Requirement Command */

typedef struct {
  uim_hdr_type      hdr;        /* Command Header */
  byte              channel;    /* Logical channel to use for the command */
  byte              ref_qual;   /* Qualifier of Reference Data */
  byte              ver_data[UIM_MAX_CHV_DIGITS];
                                /* Verification Data */
} uim_enable_ver_req_in_chan_cmd_type;

/* Disable Verification Requirement Command */

typedef struct {
  uim_hdr_type      hdr;        /* Command Header */
  byte              channel;    /* Logical channel to use for the command */
  byte              ref_qual;   /* Qualifier of Reference Data */
  byte              ver_data[UIM_MAX_CHV_DIGITS];
                                /* Verification Data */
} uim_disable_ver_req_in_chan_cmd_type;

/* Change reference data Command */

typedef struct {
  uim_hdr_type      hdr;        /* Command Header */
  byte              channel;    /* Logical channel to use for the command */
  byte              ref_qual;   /* Qualifier of Reference Data */
  byte              ver_data[UIM_MAX_CHV_DIGITS];
                                /* Verification Data */
  byte              new_ref_data[UIM_MAX_CHV_DIGITS];
                                /* New Reference Data */
} uim_change_ref_data_in_chan_cmd_type;

/* Reset Retry Command */

typedef struct {
  uim_hdr_type      hdr;        /* Command header */
  byte              channel;    /* Logical channel to use for the command */
  byte              ref_qual;   /* Qualifier of Reference Data */
  byte              unblk_data[UIM_MAX_CHV_DIGITS];
                                /* Unblock Data */
  byte              new_ref_data[UIM_MAX_CHV_DIGITS];
                                /* New Reference Data */
} uim_reset_retry_in_chan_cmd_type;

/* Manage Security Environment Command */

typedef enum {
  UIM_MSE_RESTORE = 0,          /* Restore security environment */
  UIM_MSE_SET     = 1,          /* Set a component of a security environment */
  UIM_MSE_DERIVE_KEY = 2       /* Derive a key */
} uim_mse_operation_type;


typedef enum {
  UIM_MSE_COMPUTATION       = UIM_WIM_MSE_P1_COMPUTE,   /* Computation and deciphering */
  UIM_MSE_ENCIPHER          = UIM_WIM_MSE_P1_ENCIPHERING /* Enciphering/verication */
} uim_mse_set_p1_type;

typedef enum {
  UIM_DIGITAL_SIGNATURE = UIM_WIM_MSE_P2_DIGITAL_SIGNATURE, /* Digital Signature Template */
  UIM_CONFIDENTIALITY   = UIM_WIM_MSE_P2_CONFIDENTIALITY,   /* Confidentiality Template */
  UIM_CRYPTO_CHECKSUM   = UIM_WIM_MSE_P2_CRYPTO_CHECKSUM    /* Crypto Checksum Template */
} uim_crt_type;

typedef struct {
  byte      se_number;      /* Security Environment Number */
} uim_mse_data_restore_type;

typedef struct {
  uim_mse_set_p1_type   set_p1;     /* P1 for MSE set type */
  uim_crt_type          crt;        /* Related CRT tag */
  byte                  data_length;    /* Length of CRDO data field */
  byte                  data[UIM_MAX_CHARS];    /* CRDOs */
} uim_mse_data_set_type;

typedef struct {
  byte                  secret_key_ref;        /* Secret Key Reference */
  word                  seed_length;           /* Length of CRDO data field */
  byte                  seed[UIM_MAX_CHARS-5]; /* CRDOs and data */
} uim_mse_data_derive_type;

typedef union {
  uim_mse_data_restore_type restore_data; /* Store data for a restore mse */
  uim_mse_data_set_type     set_data;     /* Store data for a set mse */
  uim_mse_data_derive_type  derive_data;  /* Store data for a derive mse */
} uim_mse_data_type;

typedef struct {
  uim_hdr_type            hdr;               /* Command header */
  byte                    channel;    /* Logical channel to use for the cmd */
  uim_mse_operation_type  operation;        /* Set, Restore, or derive key */
  uim_mse_data_type       mse_data;         /* Operation specific data */
} uim_manage_sec_env_cmd_type;

/* Perform Security Operation Command Type */


typedef enum {
  UIM_PSO_ENCIPHER      = 0,    /* Encipher */
  UIM_PSO_DECIPHER      = 1,    /* Decipher, Application Level */
  UIM_PSO_COMPUTE_DIG_SIG=2,    /* Compute Digital Signature */
  UIM_PSO_VERIFY        = 3,    /* Verify Digital Signature */
  UIM_PSO_COMPUTE_CHECKSUM=4   /* Compute Crypto Checksum */
} uim_pso_operation_type;

typedef struct {
  uim_hdr_type      hdr;        /* Command header */
  byte              channel;    /* Logical channel to use for the cmd */
  byte              data_length; /* Length of Data Field */
  uim_pso_operation_type operation; /* Type of PSO operation */
  byte              data[UIM_MAX_PSO_DATA]; /* Data Field */
} uim_perform_sec_op_cmd_type;


/* Ask Random Command Type */

typedef struct {
  uim_hdr_type      hdr;        /* Command Header */
  byte              channel;    /* Logical channel to use for the cmd */
  byte              data_length;/* Length of Data Field */
} uim_ask_random_cmd_type;


typedef enum {
  UIM_GSM_APP,
  UIM_CDMA_APP,
  UIM_TELECOM_APP,
  UIM_USIM_APP,
  UIM_CSIM_APP,
  UIM_NONE_APP
} uim_app_type;

typedef struct {
  uim_hdr_type              hdr;  /* Command header */
  uim_app_type              app;  /* App  */
  uim_slot_type             slot; /* Slot for the DF */
} uim_slot_change_cmd_type;

/* APDU Stream command type */
typedef struct {
  uim_hdr_type      hdr;
  byte              uim_class;  /* UIM class of instruction */
  byte              instrn;     /* Instruction type         */
  byte              p1;         /* input parameter 1        */
  byte              p2;         /* input parameter 2        */
  byte              p3;         /* input parameter 3        */
  byte              data[UIM_MAX_CHARS];
} uim_stream_apdu_type;

/* APDU Stream command type - extended interface*/
typedef struct {
  uim_hdr_type      hdr;
  boolean           is_select_default_ch; /* is it a select on the default channel */
  word              num_cmd_bytes;        /* Number of command bytes */
  byte              cmd_data[UIM_MAX_STREAM_ISO7816_APDU_TRANSACTION_SIZE];/* Source data */
} uim_stream_iso7816_apdu_type;

/* options as to whether or not to notify GSDI upon power down */
typedef enum {
  UIM_OPTION_NO_NOTIFY_PDOWN, /* Not Notify GSDI about the POWER down */
  UIM_OPTION_NOTIFY_PDOWN /* Notify GSDI about the POWER down */
} uim_pdown_option_type;

/* Power down command type*/
typedef struct {
  uim_hdr_type          hdr;
  uim_pdown_option_type pdown_option;
} uim_power_down_cmd_type;


/* options as to whether the entire cache shall be erased or the cache
 * shall be selectively erased for the items listed
 */
typedef enum {
  UIM_OPTION_CLEAR_CACHE_ALL,
  UIM_OPTION_CLEAR_CACHE_SELECTIVE
} uim_clear_cache_options_type;

/* Clear cache command type */
typedef struct {
  uim_hdr_type                  hdr;
  uim_clear_cache_options_type  clr_cache_option;
  word                          num_items;
  uim_items_enum_type          *item_list;
  uim_path_type   path;           /* Path for accessing an item */
} uim_clear_cache_cmd_type;

/* ---------------------------------------------------------------
   STRUCTURE:      uim_mflo_init_act_params_cmd_type

   DESCRIPTION:
     Encrypted authentication key to be used in the initialize
     Activation parameters procedure.
----------------------------------------------------------------*/
typedef struct {
  uim_hdr_type  hdr;
  int32         act_params_len;
  uint8         act_params[UIM_MFLO_INIT_ACT_PARAMS_LEN];
}uim_mflo_init_act_params_cmd_type;

/* ---------------------------------------------------------------
   STRUCTURE:      uim_mflo_ucast_cmd_type

   DESCRIPTION:
     Input parameters for the Signing or Verification of the Unicast
     Message procedure.
   ----------------------------------------------------------------*/
typedef struct {
  uim_hdr_type                            hdr;
  uim_mflo_ucast_msg_enum_type            msg_type;
  uint32                                  ucast_msg_len;
  uint8                                  *ucast_msg_ptr;
}uim_mflo_ucast_cmd_type;

/* ---------------------------------------------------------------
   STRUCTURE:      uim_hotswap_slot_capability

   DESCRIPTION:
     HotSwap capability of a slot.
   ----------------------------------------------------------------*/
typedef struct
{
  boolean is_pmic;  /* will be set to true when hotswap is supported by PMIC */
  boolean is_uim_controller; /* will be set to true when hotswap is supported by UIM controller */
  DALGpioValueType pmic_card_det_polarity; /* FALSE - ACTIVE_LOW, TRUE - ACTIVE_HIGH */
  boolean hos_enabled;
  DALGpioValueType nGPIOInsertedLevel;  /* GPIO signal level when card is inserted */
  uint8 nSlot;              /* Slot number that this struct refers to */
}uim_hotswap_slot_capability;

typedef union {
  uim_hdr_type                hdr;        /* Generic header */
  uim_write_bin_cmd_type      write_bin;  /* write to uim bin file response in rpt */
  uim_write_rec_cmd_type      write_rec;  /* write to uim rec file response in rpt */
  uim_read_bin_cmd_type       read_bin;   /* Read bin file with result data in rpt */
  uim_read_rec_cmd_type       read_rec;   /* Read rec file with result data in rpt */
  uim_inc_cmd_type            inc;        /* Increment,result data in rpt */
  uim_int_type                uim_int ;   /* Init UIM */
  uim_select_cmd_type         select;     /* Select a file/directory  */
  uim_seek_cmd_type           seek;       /* SEEK access */
  uim_invalidate_cmd_type     invalidate; /* Invalidate an EF */
  uim_rehabilitate_cmd_type   rehab;      /* Rehabilitate an EF */
  uim_verify_chv_cmd_type     verify_chv; /* Verify CHV */
  uim_change_chv_cmd_type     change_chv; /* Change CHV */
  uim_disable_chv_cmd_type    disable_chv;/* Disable CHV */
  uim_enable_chv_cmd_type     enable_chv; /* Enable CHV */
  uim_unblock_chv_cmd_type    unblk_chv;  /* Unblock CHV */
  uim_status_cmd_type         status;     /* Status of the Card */
  uim_terminal_profile_cmd_type terminal_profile; /* Send a terminal profile */
  uim_envelope_cmd_type       envelope;   /* Appl toolkit command */
  uim_fetch_cmd_type          fetch;      /* Fetch a proactive command */
  uim_terminal_response_cmd_type terminal_response; /* Proactive command rsp */
  uim_polling_interval_cmd_type  polling; /* Polling interval or Polling off */
  uim_stream_apdu_type        stream_apdu;    /* Stream APDU */
  uim_stream_iso7816_apdu_type stream_iso7816_apdu;/* Stream ISO 7816 APDU */

  uim_store_esn_cmd_type      store_esn;      /* Store ESN */
  uim_store_meid_cmd_type     store_meid;     /* Store MEID */
  uim_ssd_update_cmd_type     ssd_update;     /* Update SSD */
  uim_confirm_ssd_cmd_type    confirm_ssd;    /* Confirm SSD */
  uim_run_cave_cmd_type       run_cave;       /* Run CAVE */
  uim_aka_auth_cmd_type       aka_auth;       /* Perform AKA Auth */
  uim_generate_keys_cmd_type  gen_keys;       /* Generate CMEA key and VPM */
  uim_bs_chal_cmd_type        bs_chal;        /* Base station challenge */
  uim_commit_cmd_type         commit;         /* Commit Request */
  uim_ms_key_cmd_type         ms_key_req;     /* MS Key Request */
  uim_key_gen_cmd_type        key_gen;        /* Key Generation Request */
  uim_serv_key_gen_cmd_type   serv_key_gen;   /* Service Key Generation Request */
  uim_validation_cmd_type     validate;       /* Validation Request */
  uim_config_cmd_type         configuration;  /* Configuration Request */
  uim_download_cmd_type       download;       /* Download Request */
  uim_sspr_config_cmd_type    sspr_config;    /* SSPR Configuration Request */
  uim_sspr_download_cmd_type  sspr_download;  /* SSPR Download Request */
  uim_otapa_cmd_type          otapa_req;      /* OTAPA Request */
  uim_compute_ip_auth_cmd_type compute_ip_auth; /* Compute IP Authentication */

  uim_run_gsm_algo_type       run_gsm;    /* RUN GSM ALGORITHM */
  uim_authenticate_type          autn;         /* Authenticate for USIM */

  uim_authenticate_odd_ins_type  autn_odd_ins; /* Authenticate for USIM with ODD Ins Classs*/
  uim_bcast_type                 bcast;        /* Bcast Command*/

  uim_authenticate_type       isim_autn;  /* Authenticate for ISIM */

  uim_verify_in_chan_cmd_type           verification_in_chan; /* Verification */
  uim_enable_ver_req_in_chan_cmd_type   enable_ver_in_chan;   /* Enable verification */
  uim_disable_ver_req_in_chan_cmd_type  disable_ver_in_chan;  /* Disable verification */
  uim_change_ref_data_in_chan_cmd_type  change_ref_in_chan;   /* Change Reference Data */
  uim_reset_retry_in_chan_cmd_type      reset_retry_in_chan;  /* Reset Retry counter */
  uim_select_in_chan_cmd_type           select_in_chan;       /* Select cmd in L Ch*/
  uim_manage_channel_cmd_type           manage_channel;
  uim_manage_sec_env_cmd_type  mse;           /* MSE */
  uim_perform_sec_op_cmd_type  pso;           /* Perform Sec Operation */
  uim_ask_random_cmd_type      ask_random;    /* Ask Random */
  uim_reset_with_tp_cmd_type   reset_with_tp; /* Reset card and optionally send TP */
  uim_slot_change_cmd_type     slot_chg;      /* Slot change request */
  uim_power_down_cmd_type      pdown;
  uim_clear_cache_cmd_type     clear_cache;
  uim_mflo_init_act_params_cmd_type      mflo_init_act_params;
  uim_mflo_ucast_cmd_type                mflo_sign_ucast;
  uim_mflo_ucast_cmd_type                mflo_verify_ucast;
} uim_cmd_type;

/* Masks for bits in CDMA service table */
#define  UIM_CDMA_SVC_CHV_DISABLE_ALLOCATED_MASK        0x01
#define  UIM_CDMA_SVC_ADN_ALLOCATED_MASK                0x04
#define  UIM_CDMA_SVC_FDN_ALLOCATED_MASK                0x10
#define  UIM_CDMA_SVC_SMS_ALLOCATED_MASK                0x40
#define  UIM_CDMA_SVC_EXT1_ALLOCATED_MASK               0x04
#define  UIM_CDMA_SVC_EXT2_ALLOCATED_MASK               0x10
#define  UIM_CDMA_SVC_SMS_PARAMS_ALLOCATED_MASK         0x40
#define  UIM_CDMA_SVC_LND_ALLOCATED_MASK                0x01
#define  UIM_CDMA_SVC_HOME_SVC_PVDR_NAME_ALLOCATED_MASK 0x01
#define  UIM_CDMA_SVC_SDN_ALLOCATED_MASK                0x04
#define  UIM_CDMA_SVC_EXT3_ALLOCATED_MASK               0x10
#define  UIM_CDMA_SVC_SMS_PP_DOWNLOAD_ALLOCATED_MASK    0x04
#define  UIM_CDMA_SVC_MENU_SEL_ALLOCATED_MASK           0x10
#define  UIM_CDMA_SVC_PROACTIVE_UIM_ALLOCATED_MASK      0x01
#define  UIM_CDMA_SVC_3GPD_MSG_ALLOCATED_MASK           0x10
#define  UIM_CDMA_SVC_3GPD_SIP_ALLOCATED_MASK           0x40
#define  UIM_CDMA_SVC_3GPD_MIP_ALLOCATED_MASK           0x04
#define  UIM_CDMA_SVC_HRPD_ALLOCATED_MASK                0x01
#define  UIM_CDMA_SVC_BCSMS_ALLOCATED_MASK              0x04
#define  UIM_CDMA_SVC_RC_ALLOCATED_MASK                  0x40
#define  UIM_CDMA_SVC_BROWSER_ALLOCATED_MASK             0x01
#define  UIM_CDMA_SVC_JAVA_ALLOCATED_MASK                0x04
#define  UIM_CDMA_SVC_BREW_ALLOCATED_MASK                0x10
#define  UIM_CDMA_SVC_LBS_ALLOCATED_MASK                 0x40
#define  UIM_CDMA_IPV6_ALLOCATED_MASK                    0x10

/* Masks for bits in CDMA service table */
#define  UIM_CDMA_SVC_CHV_DISABLE_ACTIVATED_MASK        0x02
#define  UIM_CDMA_SVC_ADN_ACTIVATED_MASK                0x08
#define  UIM_CDMA_SVC_FDN_ACTIVATED_MASK                0x20
#define  UIM_CDMA_SVC_SMS_ACTIVATED_MASK                0x80
#define  UIM_CDMA_SVC_EXT1_ACTIVATED_MASK               0x08
#define  UIM_CDMA_SVC_EXT2_ACTIVATED_MASK               0x20
#define  UIM_CDMA_SVC_SMS_PARAMS_ACTIVATED_MASK         0x80
#define  UIM_CDMA_SVC_LND_ACTIVATED_MASK                0x02
#define  UIM_CDMA_SVC_HOME_SVC_PVDR_NAME_ACTIVATED_MASK 0x02
#define  UIM_CDMA_SVC_SDN_ACTIVATED_MASK                0x08
#define  UIM_CDMA_SVC_EXT3_ACTIVATED_MASK               0x20
#define  UIM_CDMA_SVC_SMS_PP_DOWNLOAD_ACTIVATED_MASK    0x08
#define  UIM_CDMA_SVC_MENU_SEL_ACTIVATED_MASK           0x20
#define  UIM_CDMA_SVC_PROACTIVE_UIM_ACTIVATED_MASK      0x02
#define  UIM_CDMA_SVC_3GPD_MSG_ACTIVATED_MASK           0x20
#define  UIM_CDMA_SVC_3GPD_SIP_ACTIVATED_MASK           0x80
#define  UIM_CDMA_SVC_3GPD_MIP_ACTIVATED_MASK           0x08
#define  UIM_CDMA_SVC_HRPD_ACTIVATED_MASK               0x02
#define  UIM_CDMA_SVC_BCSMS_ACTIVATED_MASK              0x08
#define  UIM_CDMA_SVC_RC_ACTIVATED_MASK                 0x80
#define  UIM_CDMA_SVC_BROWSER_ACTIVATED_MASK            0x02
#define  UIM_CDMA_SVC_JAVA_ACTIVATED_MASK               0x08
#define  UIM_CDMA_SVC_BREW_ACTIVATED_MASK               0x20
#define  UIM_CDMA_SVC_LBS_ACTIVATED_MASK                0x80
#define  UIM_CDMA_IPV6_ACTIVATED_MASK                   0x20

/* ---------------------------------------------------------------
   ENUM:      uim_rt_status_enum

   DESCRIPTION:
     Real time status of a slot.
   ----------------------------------------------------------------*/
typedef enum {
  UIM_RT_STATUS_NOT_SUPPORTED   = 0x00,
  UIM_RT_STATUS_CARD_PRESENT,
  UIM_RT_STATUS_CARD_ABSENT,
  UIM_RT_STATUS_UNKNOWN
} uim_rt_status_enum;

/* ---------------------------------------------------------------
   ENUM:      uimdrv_recovery_stage_enum_type

   DESCRIPTION:
     Recovery statge enum.
   ----------------------------------------------------------------*/
typedef enum {
  UIMDRV_RECOVERY_STAGE_START    = 0x00000000,
  UIMDRV_RECOVERY_STAGE_END,
  UIMDRV_RECOVERY_STAGE_ENUM_MAX = 0x7FFFFFFF
} uimdrv_recovery_stage_enum_type;

/* ---------------------------------------------------------------
   Function Type:      uim_silent_recovery_cb_type

   DESCRIPTION:
     This is the function type exposed to QMI_UIM client to
     provide call back for SILENT RECOVE UICC/ICC and this call
     to be invoked to deliver the RECOVERY STATE (START/END).
   ----------------------------------------------------------------*/
typedef void (*uim_silent_recovery_cb_type) (uim_slot_type   slot,
                                             const void      *user_data,
                                             uimdrv_recovery_stage_enum_type recovery_stage
);

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*=====================================================================
FUNCTION UIM_SET_PROACTIVE_UIM_SLOT

DESCRIPTION
  This procedure is called by GSDI\GSTK to indicate to UIM if the underlying UIM is pro-active or not.

PARAMETERS
  boolean:    To Indicate the UIM is proactive or not.
  slot:       Slot for which the info is provided.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  Internally sets the UIM buffer.
======================================================================*/
void uim_set_proactive_uim_slot(
  boolean        proactive_uim,
  uim_slot_type  slot
);

/*===========================================================================

FUNCTION UIM_SET_PROACTIVE_UIM

DESCRIPTION
  This procedure is called by UI to indicate to UIM if the underlying UIM
  is pro-active or not.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  Internally sets the UIM buffer.
===========================================================================*/
void uim_set_proactive_uim
(
  boolean                proactive_uim
);

/*===========================================================================

FUNCTION UIM_SET_PROACTIVE_UIM_2

DESCRIPTION
  This procedure is called by UI/any entity that reads the CDMA servive table
  to indicate to UIM task if the underlying UIM is pro-active or not.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  Internally sets the UIM buffer.
===========================================================================*/
void uim_set_proactive_uim_2 (
  boolean           proactive_uim
);

/*======================================================================

FUNCTION UIM_GET_UIM_STATUS

DESCRIPTION
  This procedure retrieves the current state of UIM.

PARAMETERS
  slot:       Slot for which the info is provided.

DEPENDENCIES
  None

RETURN VALUE
  Returns uim_status.

SIDE EFFECTS
  None

======================================================================*/
uim_status_type uim_get_uim_slot_status(
  uim_slot_type  slot
);

/*===========================================================================

FUNCTION UIM_3_TASK

DESCRIPTION
  This procedure is the entrance procedure for the UIM 2 task.

DEPENDENCIES
  None

RETURN VALUE
  Does not return.

SIDE EFFECTS
  None

===========================================================================*/
void uim_3_task
(
  dword  dummy
);

/*===========================================================================

FUNCTION UIM_2_TASK

DESCRIPTION
  This procedure is the entrance procedure for the UIM 2 task.

DEPENDENCIES
  None

RETURN VALUE
  Does not return.

SIDE EFFECTS
  None

===========================================================================*/
void uim_2_task
(
  dword  dummy
);

/*===========================================================================

FUNCTION UIM_TASK

DESCRIPTION
  This procedure is the entrance procedure for the UIM task.

DEPENDENCIES
  None

RETURN VALUE
  Does not return.

SIDE EFFECTS
  None

===========================================================================*/
void uim_task (
  dword             dummy
);


/*===========================================================================

FUNCTION UIM_CMD

DESCRIPTION
  The UIM_cmd pointed to by the parameter passed in is queued up for UIM.

DEPENDENCIES
  None.

RETURN VALUE
  UIM_SUCCESS        :- Command queued to uim_cmd_q successfully.
  UIM_INVALID_PARAMS :- Invalid params sent by the client.
  UIM_MEM_PROBLEM    :- Error during malloc/memcpy operations.
  UIM_GENERIC_ERROR  :- Generic error

SIDE EFFECTS
  Sets the UIM_CMD_Q_SIG.
===========================================================================*/
uim_return_type uim_cmd
(
  uim_cmd_type      * cmd_ptr      /* the command to queue up */
);

/*===========================================================================

  FUNCTION UIM_GET_RT_STATUS

  DESCRIPTION
   This procedure retrieves the real time card status of given slot.

  DEPENDENCIES
   None

  RETURN VALUE
     Returns  uim_rt_status_enum.

  SIDE EFFECTS
    None
 ===========================================================================*/
uim_rt_status_enum uim_get_rt_status
(
  uim_slot_type  uim_slot
);

/*===========================================================================

  FUNCTION UIM_DISABLE_HOTSWAP_INTERRUPT

  DESCRIPTION
   This procedure will disable the hotswap interrupt for given slot

   DEPENDENCIES
   None

  RETURN VALUE
    None

  SIDE EFFECTS
    Hot swap interrupt will be disabled and ME will not able to detect
    any card removal and insertion for entire power session.
 ===========================================================================*/
void uim_disable_hotswap_interrupt
(
  uim_slot_type  uim_slot
);

/*===========================================================================

FUNCTION UIM_DISABLE_RECOVERY

DESCRIPTION
  This procedure disables recovery in uim.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_disable_recovery
(
  uim_slot_type  uim_slot,
  boolean  disable_recovery
);

/*===========================================================================

FUNCTION UIM_TRIGGER_SILENT_RECOVERY

DESCRIPTION
  This procedure is used to Initiate silent recovery
  to recover SWP interface which is meat for NFCC (NFC Controller).

DEPENDENCIES
  None

RETURN VALUE
  uim_return_type

SIDE EFFECTS
  None

===========================================================================*/
uim_return_type uim_trigger_silent_recovery
(
  uim_slot_type                 slot,
  const void                   *user_data,
  uim_silent_recovery_cb_type   user_callback
);

#endif /* UIM_V */

