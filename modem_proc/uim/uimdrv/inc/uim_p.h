#ifndef UIM_P_H
#define UIM_P_H

/*===========================================================================

            U I M  T A S K  P R I V A T E  D E F I N I T I O N S

DESCRIPTION
  This contains declarations for the UIM Task to be shared only with other
  UIM modules.

Copyright (c) 2008 - 2016 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/inc/uim_p.h#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/10/16   gm      Handling BTSAP connection unavailable signal
04/11/16   ks      Code changes needed for LPA support
01/25/16   ssr     Delay local card power up in Modem SSR in BTSAP mode
11/26/15   sam     Add logic to update EF-TST in recovery
11/05/15   ks      Rel12 polling enhancement
04/27/15   vr      Support to check the pending proactive command on card
02/03/15   xj      Add USB UICC and SAP CLIENT supported interface
11/24/14   lm      UT framework changes for uim driver task init
11/17/14   nmb     Remove dead directory maintenance code
09/18/14   akv     UIMDRV UT enhancements
08/28/14   akv     UIMDRV-QMI interactions - Indications and acks for LDO events
08/14/14   ks      Hotswap enhancement for flaky mechanical switches
08/14/14   sam     Support for extended recovery
07/30/14   sam     Fix to send Status command when Apps are reopened during recovery
07/10/14   ll      NV refresh upon hotswap feature
06/11/14   ll      Add interruptible timed delay and return value for update params
03/31/14   lxu     Fix USB UICC recovery, hotswap and iso card issue
03/27/14   lxu     USB UICC modem implement
03/11/14   kb      Added TASK_EXIT_SIG to synchronize exiting of all tasks
01/24/14   yk      Code Clean up
12/11/13   vv      Added support for the EF-PSISMSC in DF TELECOM
10/24/13   akv     Removed unnecessary feature flags
10/22/13   ll      Add UIM BUSY Response feature
10/01/13   vv      Added support for the EF-PWS
09/30/13   akv     Removal of multiple efs reads performed by uim tasks
09/27/13   akv     Task stop and offline sig changed to ensure tmc compatibility
09/26/13   yk      Clean up signals
09/16/13   yk      Removed the code for old targets and for old features
09/04/13   ks      Subscription manager - Added CARD_STATUS_UNKNOWN
08/21/13   sam     Power down UIM on technical problems through NV control
07/20/13   ks      Advanced Subscription Manager changes
07/12/13   nmb     Global reorganization
07/09/13   js      Fix for extra modem wake up while UE is camped on CDMA.
06/27/13   spo     Added support for files under DF TELECOM
06/24/13   js      TSTS changes
02/19/13   av      Merge jcdma conf file contents to uimdrv.conf
02/11/13   akv     UIM API for querying the available/enabled UIM slots
01/24/13   akv     ICCID Cached and sent along with link est data to GSDI
12/13/12   ms      New params added in uim_recovery_app_info_type for selecting
                   correct app info type
12/05/12   js      UIM Parallel Processing changes
11/07/12   akv     Task stop and RCINIT registration changes for Dime
11/06/12   ms      Reducing the retrial command failure attempts to 2 from 3
                   for non-powering up commands
10/26/12   akv     HW Enumeration changes (without features)
08/31/12   js      Fixed compiler warnings
08/09/12   ms      New params added in uim_recovery_app_info_type
07/13/12   vv      Added support for the CSG EFs
06/12/12   tl      Added Support for UIM specific JCDMA NV item
05/01/12   av      Replaced FEATURE_UIM_SUPPORT_HOTSWAP with an nvitem
03/08/12   nmb     Notify card of logical channel support
01/20/12   ak/rm   NV item logic for hotswap polarity
02/18/12   av      Cleaned up remaining ALWAYS ON features
01/23/12   ak      Fix a MOB compile error
01/20/12   ak      Platform ID/NV item logic for hotswap polarity
01/11/12   ak      Reduce debounce sample period to 330 from 3300 sclks
12/21/11   ms      Fix for Legacy GSDI removal. Added support for uim-slot
                   link establishment w.r.t to addition of MMGSDI API interface.
12/20/11   bcho    Added support for Get Operator Name API
11/30/11   ssr     Provide a new API to check the hotswap capability of given
                   slot
11/30/11   js      Revert support for file CPHS Get Operator Name Short String
11/22/11   js      Add support for file CPHS Get Operator Name Short String
11/15/11   nmb     RPM Updates
10/04/11   yk      Change deferrable timers to non-deferrable timers
09/30/11   ssr     Removed uim_notify_sys_mode api
09/26/11   ssr     Rearranging the UIM_CONF_FILE enum
09/15/11   nb      Reverting changes to block polling
08/30/11   yb      Added support for complete recovery solution
08/24/11   ssr     Fixed  NV Item support for hotswap configuration
07/18/11   rm      Catch-up with latest on Hot-swap
06/16/11   ssr     Increase debounce timer for card insertion
06/06/11   yt      Added P4 path template
05/23/11   ssr     Fixed for QCN backup support for JCDMA
05/16/11   ssr     Added UIM thread safe support
04/27/11   yt      Moved definitions to private UIM header
04/25/11   yb      Adding support for hotswap
04/20/11   yt      Exposed APIs for calculating file path
01/31/11   ssr     Decoupled FEATURE_UIM_JCDMA_RUIM_SUPPORT feature
01/28/11   yb      Fix for compilation error
01/27/11   nb      Fix for compilation warning
11/30/10   yt      Initial revision

===========================================================================*/

/*===========================================================================

                        DEFINITION DEPENDANCIES

===========================================================================*/
#include "uim_variation.h"

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_v.h"
#include "uim.h"

/*------------------------------------------------------------------------------
  SIGNALS FOR UIM TASK
------------------------------------------------------------------------------*/
  #define UIM_ALL_ZERO_SIG                0x00000000  /* All zero sig */
  #define UIM_DOG_HB_RPT_SIG              0x00000001  /* DOG Heart-Bit         */
  #define UIM_CMD_Q_SIG                   0x00000002  /* Command Q             */
  #define UIM_CMD_RSP_SIG                 0x00000004  /* Response              */
  #define UIM_CMD_RSP_TIMEOUT_SIG         0x00000008  /* Response Timeout      */
  #define UIM_POLL_TIMER_SIG              0x00000010  /* Poll Timer            */
  #define UIM_NV_CMD_SIG                  0x00000020  /* NV Operation          */
  #define UIM_STATE_TOGGLE_SIG            0x00000040  /* To exit main rex_wait */
  #define UIM_TRANSACTION_SIG             0x00000080  /* Transaction Timeout   */
  #define UIM_HOTSWAP_CMD_CARD_INS_SIG    0x00000100  /* HOTSWAP - Insertion   */
  #define UIM_HOTSWAP_CMD_CARD_REM_SIG    0x00000200  /* HOTSWAP - Removal     */
  #define UIM_EFSLOG_PURGE_SIG            0x00000400  /* EFS Log buf at 1/2    */
  #define UIM_TIMED_SLEEP_TIMER_SIG       0x00000800  /* rex_timed_wait()      */
  #define UIM_TASK_STOP_SIG               0x00001000  /* Task Stop             */
  #define UIM_TASK_OFFLINE_SIG            0x00002000  /* Task Offline          */
#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
  #define UIM_BUSY_IND_TIMER_EXP_SIG      0x00008000  /* UIM BUSY RESPONSE Feature */
  #define UIM_TRANS_TIMER_EXP_SIG         0x00010000  /* UIM BUSY RESPONSE Feature */
  #define UIM_SIMULATE_NULL_TIMER_EXP_SIG 0x00020000  /* UIM BUSY RESPONSE Feature test*/
  #define UIM_USB_REMOTE_WAKEUP_SIG       0x00040000  /* USB UICC poll             */
  #define UIM_USB_CONNECT_AVAILABLE_SIG   0x00080000  /* USB Connection Completed  */
  #define UIM_USB_FALLBACK_TO_ISO_SIG     0x00100000  /* USB mode fallback to ISO  */
  #define UIM_MCGF_NV_REFRESH_SIG         0x00200000  /* MCFG NV refresh Feature */
  #define UIM_SUSPICIOUS_CARD_REM_SIG     0x00400000  /* Card Removed Suspicious signal */
  #define UIM_EXT_RECOVERY_TIMER_EXP_SIG  0x00800000  /*Extended recovery feature*/
  #define UIM_QMI_SANITY_TIMER_EXP_SIG    0x01000000  /* UIM QMI Sanity timer exp signal */
  #define UIM_QMI_ACK_RECEIVED_SIG        0x02000000  /* QMI ACK received signal */
#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#endif
  #define UIM_FETCH_PENDING_SIG            0x08000000   /*  Fetch pending */
  #define UIM_REMOTE_CONNECTION_UNAVAILABLE_SIG     0x20000000  /* Conn unavail signal */
  #define UIM_REMOTE_CONNECTION_AVAILABLE_SIG       0x40000000  /* Conn Avail signal */
/*----------------------------------------------------------------------------*/

#define UIM_POLL_TIME            28000
/* For CDMA 29.5 sec (max poll) - 5.12 sec CDMA drx - 2 msec modem wake up */
#define UIM_CDMA_POLL_TIME        24378
/* "Setting the default USB poll time to 5 mins" */
#define UIM_USB_POLL_TIME         300000

#define UIM_MAX_BAD_BLOCK_RETRIES   2

#define UIM_MAX_TRANSACTION_SIZE                      256
#define UIM_MAX_TRANS_SIZE_OUT                        255
#define UIM_MAX_TRANSACTION_P3                        0
#define NUM_BYTES_APDU_HDR                            5
#define UIM_MAX_NUM_CMD_ATTEMPTS(uim_ptr)    ( 3 + uim_ptr->setting.config_params.ext_cmd_additional_re_try_cnt )

/* This is the maximum number of ICCID chars
   as defined in the specifications */
#define UIM_MAX_ICCID_CHARS                           10

/* This table contains the path corresponding to the items in the items table.
   This is a three dimensional table and each record contains the DF1, DF2
   and EF corresponding to the EF. If there is no second level DF, the DF2 is
   the same. If the EF is under the MF, the DF1 and DF2 are set to zero. The
   order of the items is the same as in the uim_items_table array.

   !!!!!! PLEASE TAKE UTMOST CARE WHEN UPDATING THIS TABLE !!!!!! */

/* IMPORTANT!!!!!!!! order in uim_items_table, gsdi_mf_cache_tbl and the enum need to match */
#define UIM_MAX_EF_UNDER_MF_ITEMS       5 // READ ABOVE!

/* IMPORTANT!!!!!!! order in uim_items_table, gsdi_ruim_cache_tbl and the enum need to match */
#define UIM_MAX_RUIM_ITEMS              106 // READ ABOVE!

/* IMPORTANT!!!!!!!! order in uim_items_table, gsdi_gsm_cache_tbl and the enum need to match */
#define UIM_MAX_GSM_ITEMS               63 // READ ABOVE!

#define UIM_MAX_DCS1800_ITEMS           10

/* IMPORTANT!!!!!!!! order in uim_items_table, gsdi_telecom_cache_tbl and the enum need to match */
#define UIM_MAX_TELECOM_ITEMS           33 // READ ABOVE!

/* IMPORTANT!!!!!!!! order in uim_items_table, gsdi_usim_cache_tbl and the enum need to match */
#define UIM_MAX_USIM_ITEMS              131 // READ ABOVE!
#define UIM_MAX_ORANGE_ITEMS            4

#define UIM_MAX_ISIM_ITEMS              6

#define UIM_MAX_CINGULAR_ITEMS          7
#define UIM_MAX_WIM_ITEMS               3

#define UIM_MAX_HZI_ITEMS               11

#define UIM_MAX_MFLO_ITEMS              12

/* IMPORTANT!!!!!!!! order in uim_items_table, gsdi_csim_cache_tbl and the enum need to match */
#define UIM_MAX_CSIM_ITEMS              127 // READ ABOVE !

#define UIM_MAX_EF_BY_PATH_ITEMS        1

/* Default Simple IP profile to be used on the card
 NOTE:  Only one Simple IP profile is supported in the ME, although
 multiple profiles are supported on the RUIM.  Profiles can only be changed
 at complile-time by changing this constant. */

#define UIM_CURRENTLY_ACTIVE_SIP_PROFILE 0

#define UIM_DATA_DO_HEADER 0x4D
#define UIM_DATA_FILE_PATH 0x81
#define UIM_DATA_KEY_REF_PUB_ASYM 0x83
#define UIM_DATA_KEY_DIR_SYM 0x83
#define UIM_DATA_KEY_SESS_SY 0x84
#define UIM_DATA_KEY_REF_PRV_ASYM 0x84
#define UIM_DATA_HASH_CODE 0x90
#define UIM_DATA_RNDM_NUM 0x91
#define UIM_DATA_CHLNG_DATA_ITM 0x94
#define UIM_DATA_RESULT_LEN 0x96

#define UIM_PSO_DATA_PLAIN_VALUE 0x80
#define UIM_PSO_DATA_PADD_IND_CRYPTOGRAM 0x86
#define UIM_PSO_DATA_CRYPTOGRAPHIC_CHECKSUM 0x8E
#define UIM_PSO_DATA_DIGITAL_SIGNATURE 0x9E
#define UIM_PSO_DATA_DIGITAL_SIGNATURE_INPUT 0x9A
#define UIM_PSO_DATA_LE_VALUE 0x96
#define UIM_PSO_DATA_INPUT_TEMPLATE_DS 0xA8

#define UIM_CDMA_DF              0x7F25
#define UIM_GSM_DF               0x7F20
#define UIM_DCS_1800_DF          0x7F21
#define UIM_TELECOM_DF           0x7F10
#define UIM_ORANGE_DF            0x7F40
#define UIM_CINGULAR_DF          0x7F66
#define UIM_GRAPHICS_DF          0x5F50
#define UIM_SOLSA_DF             0x5F70
#define UIM_MEXE_DF              0x5F3C
#define UIM_HZI_DF               0x7F43
#define UIM_MFLO_RAN_DF          0x5F10
#define UIM_MFLO_MFAPP_DF        0x5F50

/* These constants define the duration conversion values */
#define MSEC_PER_DSEC 100
#define MSEC_PER_SEC  1000
#define MSECS_PER_MIN 60000

#define UIM_EF_TST_MAX_LEN                            32

/* Minimum polling duration supported by code. */
#define UIM_MIN_POLL_DURATION 5000
#define UIM_POLL_OFFSET 2000
/* Maximum difference between the regular poll timer and wake up poll timer */
#define UIM_WAKE_UP_POLL_OFFSET 1500
#define UIM_CDMA_WAKE_UP_POLL_OFFSET  5120
#define UIM_MAX_POLL_DURATION (unsigned)(0xFFFFFFFF - UIM_WAKE_UP_POLL_OFFSET)
#define UIM_MIN_TIME_UNIT UIM_TK_TENTHS_SECS
#define UIM_MIN_TIME_INTERVAL (UIM_MIN_POLL_DURATION/MSEC_PER_DSEC)

typedef struct uim_instance_global_type uim_instance_global_type;
typedef struct uim_shared_global_type   uim_shared_global_type;

#define UIM_NUM_REHAB_FILES                          0x5

typedef enum {
  UIM_NORMAL_MODE,
  UIM_PASSIVE_MODE,
  UIM_RECOVERY_MODE
} uim_mode_type;

/* Hot swap status */
/* NOTE: do not change the order of the ENUMs */
typedef enum {
  UIM_CARD_REMOVED,
  UIM_CARD_INSERTED,
  UIM_CARD_UNCHANGED,
  UIM_CARD_STATUS_UNKNOWN
}uim_card_swap_enum_type;

#define UIM_7816_APDU_CLASS_OFFSET  0
#define UIM_7816_APDU_INSTRN_OFFSET 1
#define UIM_7816_APDU_P1_OFFSET     2
#define UIM_7816_APDU_P2_OFFSET     3
#define UIM_7816_APDU_MIN_SIZE      4
#define UIM_CASE1_7816_APDU_SIZE    4
#define UIM_CASE2_7816_APDU_SIZE    5
#define UIM_CASE2_7816_LE_OFFSET    4
#define UIM_CASE3_4_7816_LC_OFFSET  4
#define UIM_ATCSIM_APDU_MIN_SIZE    4

#define SELECT                          ((uim_instrns_enum_type) 0xA4)
#define STATUS                          ((uim_instrns_enum_type) 0xF2)
#define READ_BINARY                     ((uim_instrns_enum_type) 0xB0)
#define UPDATE_BINARY                   ((uim_instrns_enum_type) 0xD6)
#define READ_RECORD                     ((uim_instrns_enum_type) 0xB2)
#define UPDATE_RECORD                   ((uim_instrns_enum_type) 0xDC)
#define SEEK                            ((uim_instrns_enum_type) 0xA2)
#define INCREASE                        ((uim_instrns_enum_type) 0x32)
#define VERIFY_CHV                      ((uim_instrns_enum_type) 0x20)
#define CHANGE_CHV                      ((uim_instrns_enum_type) 0x24)
#define DISABLE_CHV                     ((uim_instrns_enum_type) 0x26)
#define ENABLE_CHV                      ((uim_instrns_enum_type) 0x28)
#define UNBLOCK_CHV                     ((uim_instrns_enum_type) 0x2C)
#define INVALIDATE                      ((uim_instrns_enum_type) 0x04)
#define REHABILITATE                    ((uim_instrns_enum_type) 0x44)
#define RUN_GSM_ALGORITHM               ((uim_instrns_enum_type) 0x88)
#define AUTHENTICATE                    ((uim_instrns_enum_type) 0x88)
#define AUTHENTICATE_ODD_INS            ((uim_instrns_enum_type) 0x89)
#define BCAST_INS                       ((uim_instrns_enum_type) 0x1B)
#define ISIM_AUTHENTICATE               ((uim_instrns_enum_type) 0x88)
#define SLEEP                           ((uim_instrns_enum_type) 0xFA)
#define GET_RESPONSE                    ((uim_instrns_enum_type) 0xC0)
#define TERMINAL_PROFILE                ((uim_instrns_enum_type) 0x10)
#define ENVELOPE                        ((uim_instrns_enum_type) 0xC2)
#define FETCH                           ((uim_instrns_enum_type) 0x12)
#define TERMINAL_RESPONSE               ((uim_instrns_enum_type) 0x14)
#define UPDATE_SSD                      ((uim_instrns_enum_type) 0x84)
#define BS_CHALLENGE                    ((uim_instrns_enum_type) 0x8A)
#define CONFIRM_SSD                     ((uim_instrns_enum_type) 0x82)
#define RUN_CAVE                        ((uim_instrns_enum_type) 0x88)
#define UIM_CDMA_AKA_AUTH               ((uim_instrns_enum_type) 0x88)
#define GEN_KEY_VPM                     ((uim_instrns_enum_type) 0x8E)
#define UIM_MANAGE_SSD                  ((uim_instrns_enum_type) 0x82)
#define UIM_GENERIC_KEY_GEN_REQ         ((uim_instrns_enum_type) 0x50)
#define UIM_GENERIC_CONFIG_REQ          ((uim_instrns_enum_type) 0x54)
#define UIM_GENERIC_DOWNLOAD_REQ        ((uim_instrns_enum_type) 0x56)
#define MS_KEY_REQUEST                  ((uim_instrns_enum_type) 0x50)
#define KEY_GEN_REQUEST                 ((uim_instrns_enum_type) 0x52)
#define CONFIG_REQUEST                  ((uim_instrns_enum_type) 0x54)
#define DOWNLOAD_REQUEST                ((uim_instrns_enum_type) 0x56)
#define SSPR_CONFIG_REQUEST             ((uim_instrns_enum_type) 0xEA)
#define SSPR_DOWNLOAD_REQUEST           ((uim_instrns_enum_type) 0xEC)
#define OTAPA_REQUEST                   ((uim_instrns_enum_type) 0xEE)
#define COMMIT                          ((uim_instrns_enum_type) 0xCC)
#define VALIDATE                        ((uim_instrns_enum_type) 0xCE)
#define STORE_ESN_ME                    ((uim_instrns_enum_type) 0xDE)
#define COMPUTE_IP_AUTHENTICATION       ((uim_instrns_enum_type) 0x80)
#define VERIFY_IN_CHAN                  ((uim_instrns_enum_type) 0x20)
#define DISABLE_VER_IN_CHAN             ((uim_instrns_enum_type) 0x26)
#define ENABLE_VER_IN_CHAN              ((uim_instrns_enum_type) 0x28)
#define CHANGE_REF_IN_CHAN              ((uim_instrns_enum_type) 0x24)
#define RESET_RETRY_IN_CHAN             ((uim_instrns_enum_type) 0x2C)
#define MSE                             ((uim_instrns_enum_type) 0x22)
#define PSO                             ((uim_instrns_enum_type) 0x2A)
#define ASK_RANDOM                      ((uim_instrns_enum_type) 0x84)
#define MANAGE_CHANNEL                  ((uim_instrns_enum_type) 0x70)
#define UIM_GENERATE_ASY_KEY_PAIR       ((uim_instrns_enum_type) 0x46)
#define UIM_MFLO_INIT_ACT_PARAMS        ((uim_instrns_enum_type) 0x30)
#define UIM_MFLO_GET_SUBSCRIBER_ID      ((uim_instrns_enum_type) 0x32)
#define UIM_MFLO_GET_PUBLIC_KEY         ((uim_instrns_enum_type) 0x34)
#define UIM_MFLO_SIGN_UCAST             ((uim_instrns_enum_type) 0x36)
#define UIM_MFLO_VERIFY_UCAST           ((uim_instrns_enum_type) 0x38)
#define UIM_TERMINAL_CAPABILITY         ((uim_instrns_enum_type) 0xAA)

/* Maximum number of historical characters of ATR. */
#define UIM_MAX_HIST_CHARS           0x0F

/*Holds the number of slots*/

#define UIM_MAX_NUM_SLOTS            3

/* Defining the UIM_MAX_NUMBER_INSTANCES to be 4.
   It has to be in accordance with UIM_INSTANCES_CONFIGURABLE.
   UIM_INSTANCES_CONFIGURABLE is based on the number of UIM instances configurable by NV 70210.
   Hence UIM_MAX_NUMBER_INSTANCES = UIM_INSTANCES_CONFIGURABLE.
*/
#define UIM_MAX_NUMBER_INSTANCES     4

typedef byte uim_instrns_enum_type;

/* Coding for PROFILE COMMAND      */
/* Byte 1 of TERMINAL PROFILE data */
#define UIM_TK_B1_PROFILE_DOWNLOAD             0x01
#define UIM_TK_B1_CDMA_SMS_PP_DOWNLOAD         0x02
#define UIM_TK_B1_CELL_BROADCAST_DOWNLOAD      0x04
#define UIM_TK_B1_MENU_SELECTION               0x08

#ifdef FEATURE_UIM_TOOLKIT_UTK
#define UIM_TK_B1_CDMA_SMS_PP_DOWNLOAD2        0x10
#elif defined(FEATURE_UIM_TOOLKIT_STK)
#define UIM_TK_B1_SUPPORT_9EXX_ERROR_CODE      0x10
#endif

#define UIM_TK_B1_TIMER_EXPIRATION             0x20
#define UIM_TK_B1_RFU1                         0x40
#define UIM_TK_B1_CALL_CTRL_BY_NAA             0x80

/* Byte 2 of TERMINAL PROFILE data */
#define UIM_TK_B2_COMMAND_RESULT               0x01
#define UIM_TK_B2_CALL_CTRL_BY_NAA             0x02

#ifdef FEATURE_UIM_TOOLKIT_UTK
#define UIM_TK_B2_CALL_CTRL_BY_NAA2            0x04
#elif  defined(FEATURE_UIM_TOOLKIT_STK)
#define UIM_TK_CELL_ID_INCL_IN_CC_BY_NAA       0x04
#endif

#ifdef FEATURE_UIM_TOOLKIT_UTK
#define UIM_TK_B2_RFU1                         0x08
#elif defined(FEATURE_UIM_TOOLKIT_STK)
#define UIM_TK_MO_SMS_CONTROL_BY_NAA           0x08
#endif

#ifdef FEATURE_UIM_TOOLKIT_UTK
#define UIM_TK_B2_RFU2                         0x10
#elif defined(FEATURE_UIM_TOOLKIT_STK)
#define UIM_TK_ALPHA_ID_HANDLING               0x10
#endif

#define UIM_TK_B2_UNICODE_ENTRY                0x20
#define UIM_TK_B2_UNICODE_DISPLAY              0x40
#define UIM_TK_B2_DISPAY_EXTENSION_TEXT        0x80

/* Byte 3 of TERMINAL PROFILE data */
#define UIM_TK_B3_DISPLAY_TEXT                 0x01
#define UIM_TK_B3_GET_INKEY                    0x02
#define UIM_TK_B3_GET_INPUT                    0x04
#define UIM_TK_B3_MORE_TIME                    0x08
#define UIM_TK_B3_PLAY_TOME                    0x10
#define UIM_TK_B3_POLL_INTERVAL                0x20
#define UIM_TK_B3_POLLING_OFF                  0x40
#define UIM_TK_B3_REFRESH                      0x80

/* Byte 4 of TERMINAL PROFILE data */
#define UIM_TK_B4_SELECT_ITEM                  0x01
#define UIM_TK_B4_SEND_SHORT_MSG               0x02
#define UIM_TK_B4_SEND_SS                      0x04
#define UIM_TK_B4_SEND_USSD                    0x08
#define UIM_TK_B4_SETUP_CALL                   0x10
#define UIM_TK_B4_SETUP_MENU                   0x20
#define UIM_TK_B4_PROVIDE_LOCAL_INFO_MCC       0x40
#define UIM_TK_B4_PROVIDE_LOCAL_INFO_NMR       0x80

/* Byte 5 of TERMINAL PROFILE data */
#define UIM_TK_B5_SETUP_EVENT_LIST             0x01
#define UIM_TK_B5_MT_CALL                      0x02
#define UIM_TK_B5_CALL_CONNECTED               0x04
#define UIM_TK_B5_CALL_DISCONNECTED            0x08
#define UIM_TK_B5_LOCATION_STATUS              0x10
#define UIM_TK_B5_USER_ACTIVITY                0x20
#define UIM_TK_B5_IDLE_SCREEN_AVAILABLE        0x40
#define UIM_TK_B5_CARD_READER_STATUS           0x80

/* Byte 6 of TERMINAL PROFILE data */
#define UIM_TK_B6_LANGUAGE_SELECTION           0x01
#define UIM_TK_B6_BROWSER_TERMINATION          0x02
#define UIM_TK_B6_DATA_AVAILABLE               0x04
#define UIM_TK_B6_CHAN_STATUS                  0x08

#ifdef FEATURE_UIM_TOOLKIT_UTK
#define UIM_TK_B6_ACCESS_TECH_CHANGE           0x10
#define UIM_TK_B6_DISPLAY_PARAMS_CHANGE        0x20
#define UIM_TK_B6_LOCAL_CONNECTION             0x40
#define UIM_TK_B6_RFU                          0x80
#else
#define UIM_TK_B6_RFU1                         0x10
#define UIM_TK_B6_RFU2                         0x20
#define UIM_TK_B6_RFU3                         0x40
#define UIM_TK_B6_RFU4                         0x80
#endif

/* Byte 7 of TERMINAL PROFILE data */
#define UIM_TK_B7_POWER_ON_CARD                0x01
#define UIM_TK_B7_POWER_OFF_CARD               0x02
#define UIM_TK_B7_PERFORM_CARD_APDU            0x04
#define UIM_TK_B7_GET_CARD_READER_STATUS       0x08
#define UIM_TK_B7_GET_CARD_READER_ID           0x10
#define UIM_TK_B7_RFU1                         0x20
#define UIM_TK_B7_RFU2                         0x40
#define UIM_TK_B7_RFU3                         0x80

/* Byte 8 of TERMINAL PROFILE data */
#define UIM_TK_B8_TIMER_MGMT_START_STOP        0x01
#define UIM_TK_B8_TIMER_MGMT_CURR_VALUE        0x02
#define UIM_TK_B8_PROVIDE_LOCAL_INFO_DATE      0x04

#ifdef FEATURE_UIM_TOOLKIT_UTK
#define UIM_TK_B8_GET_INKEY                    0x08
#else
#define UIM_TK_B8_BINARY_CHOIVE_IN_GET_INKEY   0x08
#endif

#define UIM_TK_B8_SETUP_IDLE_MODE_TEXT         0x10
#define UIM_TK_B8_RUN_AT_COMMAND               0x20
#define UIM_TK_B8_SEC_ALPHA_ID_IN_SETUP_CALL   0x40

#ifdef FEATURE_UIM_TOOLKIT_UTK
#define UIM_TK_B8_CALL_CTRL_BY_NAA             0x80
#else
#define UIM_TK_B8_SEC_CAP_CFG_PARAM            0x80
#endif

/* Byte 9 of TERMINAL PROFILE data */
#ifdef FEATURE_UIM_TOOLKIT_UTK
#define UIM_TK_B9_DISPLAY_TEXT                 0x01
#else
#define UIM_TK_B9_SUSTAINED_DIDPLAY_TEXT       0x01
#endif

#define UIM_TK_B9_SEND_TDMF                    0x02

#ifdef FEATURE_UIM_TOOLKIT_UTK
#define UIM_TK_B9_PROVIDE_LOCAL_INFO_NMR       0x04
#else
#define UIM_TK_B9_PROVIDE_LOCAL_INFO_BCCH      0x04
#endif

#define UIM_TK_B9_PROVIDE_LOCAL_INFO_LANG      0x08
#define UIM_TK_B9_PROVIDE_LOCAL_INFO_TIMING    0x10
#define UIM_TK_B9_LANGUAGE_NOTIFICATION        0x20
#define UIM_TK_B9_LAUNCH_BROWSER               0x40

#ifdef FEATURE_UIM_TOOLKIT_UTK
#define UIM_TK_B9_PROVIDE_LOCAL_INFO_ACC_TECH  0x80
#else
#define UIM_TK_B9_RFU                          0x80
#endif

/* Byte 10 of TERMINAL PROFILE data */
#define UIM_TK_B10_SOFT_KEYS_SELECT_ITEM       0x01
#define UIM_TK_B10_SOFT_KEYS_SETUP_MENU        0x02
#define UIM_TK_B10_RFU1                        0x04
#define UIM_TK_B10_RFU2                        0x08
#define UIM_TK_B10_RFU3                        0x10
#define UIM_TK_B10_RFU4                        0x20
#define UIM_TK_B10_RFU5                        0x40
#define UIM_TK_B10_RFU6                        0x80

/* Byte 11 of TERMINAL PROFILE data */
/* Indicates the max number of soft keys */

/* Byte 12 of TERMINAL PROFILE data */
#define UIM_TK_B12_OPEN_CHAN                   0x01
#define UIM_TK_B12_CLOSE_CHAN                  0x02
#define UIM_TK_B12_RECEIVE_DATA                0x04
#define UIM_TK_B12_SEND_DATA                   0x08
#define UIM_TK_B12_GET_CHANNEL_STATUS          0x10
#ifdef FEATURE_UIM_TOOLKIT_UTK
#define UIM_TK_B12_SERVICE_SRCH                0x20
#define UIM_TK_B12_GET_SERVICE_INFO            0x40
#define UIM_TK_B12_DECLARE_SERVICE             0x80
#else
#define UIM_TK_B12_RFU1                        0x20
#define UIM_TK_B12_RFU2                        0x40
#define UIM_TK_B12_RFU3                        0x80
#endif

/* Byte 13 of TERMINAL PROFILE data */
#define UIM_TK_B13_CSD_SUP_BY_ME               0x01
#define UIM_TK_B13_GPRS_SUP_BY_ME              0x02
#ifdef FEATURE_UIM_TOOLKIT_UTK
#define UIM_TK_B13_BT_SUP_BY_ME                0x04
#define UIM_TK_B13_IRDA_SUP_BY_ME              0x08
#define UIM_TK_B13_RS232_SUP_BY_ME             0x10
#endif
/* bit6-bit8 are number of channel supported */

/* Byte 14 of TERMINAL PROFILE data */
/* bit1-bit5 are number of characters supported */
#define UIM_TK_B14_RFU1                        0x20
#define UIM_TK_B14_RFU2                        0x40
#define UIM_TK_B14_SCREEN_SIZE_PARAMS          0x80

/* Byte 15 of TERMINAL PROFILE data */
/* bit1-bit7 are number of characters supported */
#define UIM_TK_B15_VAR_SIZE_FONT               0x10

/* Byte 16 of TERMINAL PROFILE data */
#define UIM_TK_B16_DISP_RESIZE                 0x01
#define UIM_TK_B16_TEXT_WRAP                   0x02
#define UIM_TK_B16_TEXT_SCROLL                 0x04
#define UIM_TK_B16_RFU1                        0x08
#define UIM_TK_B16_RFU2                        0x10
/* bit6-bit8 are width reduction */

/* Byte 17 of TERMINAL PROFILE data */
#define UIM_TK_B17_TCP                         0x01
#define UIM_TK_B17_UDP                         0x02
#define UIM_TK_B17_RFU1                        0x04
#define UIM_TK_B17_RFU2                        0x08
#define UIM_TK_B17_RFU3                        0x10
#define UIM_TK_B17_RFU4                        0x20
#define UIM_TK_B17_RFU5                        0x40
#define UIM_TK_B17_RFU6                        0x80

#ifdef FEATURE_UIM_TOOLKIT_UTK
/* Byte 18 of TERMINAL PROFILE data */
#define UIM_TK_B18_DISP_TEXT_VAR_TIME_OUT      0x01
#define UIM_TK_B18_GET_INKEY_W_HELP            0x02
#define UIM_TK_B18_USB                         0x04
#define UIM_TK_B18_GET_INKEY_VAR_TIME_OUT      0x08
#define UIM_TK_B18_PROVIDE_LOCAL_INFO_ESN      0x10
#define UIM_TK_B18_RFU1                        0x20
#define UIM_TK_B18_RFU2                        0x40
#define UIM_TK_B18_RFU3                        0x80

#define UIM_TK_B20_CDMA_SEND_SMS               0x01
#define UIM_TK_B20_CDMA_SMS_PP_DOWNLOAD        0x02

#endif /* FEATURE_UIM_TOOLKIT_UTK */

#ifdef FEATURE_UIM_TOOLKIT_UTK
  /* Length of PROFILE command data (UTK spec) */
  #define UIM_TK_TERMINAL_PROFILE_DATA_LENGTH    20
#else /* !FEATURE_UIM_TOOLKIT_UTK */
  #define UIM_TK_TERMINAL_PROFILE_DATA_LENGTH    19
#endif /* FEATURE_UIM_TOOLKIT_UTK */

#define UIM_ICCID_SIZE                           10
/* Number of physical slots present */
#define UIM_NUM_PHY_SLOTS                        2

/*---------------------------------------------------------------------
STRUCT:
  UIM_HIST_CHAR_BUF_TYPE
DESCRIPTION:
  Buffer for Historical characters
---------------------------------------------------------------------*/
typedef struct
{
  byte data[ UIM_MAX_HIST_CHARS ];
  byte length;
} uim_hist_char_buf_type;

/*------------------------------------------------------------------------------
STRUCT:
  UIM_ATR_BUF_TYPE
DESCRIPTION:
  Buffer To Store ATR info
------------------------------------------------------------------------------*/
typedef struct {
  byte num_bytes;              /* number of bytes in ATR */
  byte data[UIM_MAX_ATR_CHARS];/* buffer to hold the ATR */
} uim_atr_buf_type;

/*------------------------------------------------------------------------------
STRUCT:
  UIM_ICCID_BUF_TYPE
DESCRIPTION:
  Buffer To Store the ICCID info
------------------------------------------------------------------------------*/
typedef struct {
  byte num_bytes;                 /* number of bytes in ICCID */
  byte data[UIM_MAX_ICCID_CHARS]; /* buffer to hold the ICCID */
} uim_iccid_buf_type;

/*------------------------------------------------------------------------------
ENUM:
  uim_card_status_type
DESCRIPTION:
  Indicates whether or Not a Slot is present on the Card
------------------------------------------------------------------------------*/
typedef enum{
  UIM_SLOT_ERROR = 0,
  UIM_SLOT_PRESENT = 1
}uim_card_status_type;

/*------------------------------------------------------------------------------
ENUM:
  uim_slot_connection_enum_type
DESCRIPTION:
  Indicates the type of UIM connection
------------------------------------------------------------------------------*/
typedef enum{
  UIM_CONNECTION_ISO   = 0,
  UIM_CONNECTION_BTSAP,
  UIM_CONNECTION_USBUICC
}uim_slot_connection_enum_type;

/*------------------------------------------------------------------------------
STRUCT:
  uim_slot_link_established_data_type;
DESCRIPTION:
  Buffer To Store GSDI-UIM Link Established info
------------------------------------------------------------------------------*/
typedef struct {
  uim_protocol_type          protocol;
  uim_card_status_type       slot_status;
  boolean                    cdma_df_present;
  boolean                    gsm_df_present;
  uim_atr_buf_type           atr_buf;
  uim_hist_char_buf_type     hist_char_buf;
  uint32                     work_waiting_time;
  uim_iccid_buf_type         iccid_buf;
  boolean                    is_euicc;
}uim_slot_link_established_data_type;

typedef struct {
  uim_slot_link_established_data_type slot_data[UIM_MAX_NUM_SLOTS];
}uim_link_established_data_type;


/* ---------------------------------------------------------------
   STRUCTURE:      uim_recovery_pin_info_type

   DESCRIPTION:
     Structure holds PIN data required during recovery.
   ----------------------------------------------------------------*/
typedef struct {
  uim_chv_type   chv;                             /* PIN key ref. */
  byte           chv_digits[UIM_MAX_CHV_DIGITS];  /* PIN digits */
}uim_recovery_pin_info_type;

/* ---------------------------------------------------------------
   STRUCTURE:      uim_recovery_ef_tst_info_type

   DESCRIPTION:
     Structure holds EF-TST related data and length required
    during recovery
   ----------------------------------------------------------------*/
typedef struct {
  uint8   length;                       /*lenght of the EF-TST data*/
  uint8   data[UIM_EF_TST_MAX_LEN];  /*pointer to the EF-tst data*/
} uim_recovery_ef_tst_info_type;


/* ---------------------------------------------------------------
   STRUCTURE:      uim_recovery_app_info_type

   DESCRIPTION:
     Structure holds Application related data required
    during recovery
   ----------------------------------------------------------------*/
typedef struct {
  boolean                       is_ch_valid;     /* Is the recovery data valid */
  boolean                       do_not_recover;  /* do not recover this channl */
  boolean                       is_aid_valid;    /* recover the channel if AID is valid*/
  uim_channel_type              channel;         /* Channel ID */
  uim_aid_type                  aid;             /* AID for UICC */
  boolean                       send_status;     /* send status req */
  uim_recovery_pin_info_type    pin1_data;       /* PIN1 data for the app */
  uim_recovery_pin_info_type    pin2_data;       /* PIN2 data for the app */
  uim_session_control_type      session_control; /*select response type*/
  uim_recovery_ef_tst_info_type ef_tst_data;     /* EF-tst related data */
}uim_recovery_app_info_type;


/* ---------------------------------------------------------------
   STRUCTURE:      uim_recovery_rehab_info_type

   DESCRIPTION:
     Structure holds File Rehabilitation related data required
    during recovery
   ----------------------------------------------------------------*/
typedef struct {
  uim_items_enum_type item;             /* File to be rehabilitated */
  uim_path_type       path;
  boolean             rehab_reqd;       /* Is rehabilitation required */
}uim_recovery_rehab_info_type;

/* ---------------------------------------------------------------
   STRUCTURE:      uim_recovery_data_type

   DESCRIPTION:
     Structure holds all card related data required during recovery
   ----------------------------------------------------------------*/
typedef struct {
  uim_slot_type                slot;                              /* Slot to be recovered */
  uim_protocol_type            protocol;                          /* Protocol of the slot */
  uint8                        num_channels;                      /* Total number of active channels on the card */
  uint8                        ch_to_be_recovered;                /* Index of channel to be recovered */
  uim_recovery_app_info_type   app_data[UIM_MAX_CHANNELS];        /* App data of all active apps on the card */
  uim_recovery_pin_info_type   upin_data;                         /* Universal PIN data for the card */
  uim_recovery_rehab_info_type rehab_data[UIM_NUM_REHAB_FILES];   /* Array of files to be rehabilitated */
  uint8                        rehab_file_index;                  /* Index of file to be rehabilitated */
}uim_recovery_data_type;

/* ---------------------------------------------------------------
   STRUCTURE:      uim_slot_status_type

   DESCRIPTION:
     1. Structure holds uimdrv slot status got by reading
        UIM_HW_CONFIG_NV_EF.
     2. slot_available indicates if the HW has the capability
        to support the slot.
     3. slot_enabled indicates if the available slot is enabled
        by the user which is determined by reading NV
        UIM_HW_CONFIG_NV_EF.
   ----------------------------------------------------------------*/
typedef struct {
  boolean  slot_available;
  boolean  slot_enabled;
}uim_slot_status_type;

/* ---------------------------------------------------------------
   STRUCTURE:      uimdrv_hw_slot_info_type

   DESCRIPTION:
     Structure holds all hardware interface info got by reading
     UIM_HW_CONFIG_NV_EF
   ----------------------------------------------------------------*/
typedef struct {
  uim_slot_status_type  slot_status[UIM_MAX_NUMBER_INSTANCES];
}uim_hw_slot_info_type;

/* ---------------------------------------------------------------
   ENUM:      uim_slot_state_enum_type

   DESCRIPTION:
      Enum to depict activity status of the physical slots
   ----------------------------------------------------------------*/
typedef enum {
  UIM_SLOT_STATE_INACTIVE      = 0x00,
  UIM_SLOT_STATE_ACTIVE        = 0x01
} uim_slot_state_enum_type;

/* ---------------------------------------------------------------
   STRUCTURE:      uim_phy_slot_status_type

   DESCRIPTION:
     Structure holds all physical card related data required during
     subscription manager data transition to QMI
   ----------------------------------------------------------------*/
typedef struct{
  uim_slot_type             logical_slot;                    /* Logical slot mapped to given physical slot*/
  uim_card_swap_enum_type   card_presence_status;            /* Hotswap card status of given physical slot */
  byte                      iccid_len;                       /* Length of iccid for the card inserted*/
  byte                      iccid[UIM_ICCID_SIZE];           /* iccid of given physical slot */
  uim_slot_state_enum_type  activity_status;
}uim_phy_slot_status_type;

/* ---------------------------------------------------------------
   ENUM:      uimdrv_qmi_indications_type

   DESCRIPTION:
      Enum to depict the UIM indications that are sent to QMI_UIM.
      These indications are used to notify AP via QMI_UIM.
   ----------------------------------------------------------------*/
typedef enum
{
  UIMDRV_QMI_INVALID_IND,
  /* LDO indications to QMI */
  UIMDRV_LDO_ACTIVATED_IND, /* Notify that the LDO is activated */
  UIMDRV_LDO_AWAITING_DEACTIVATION_IND, /* Notify that the LDO is awaiting deactivation */
  UIMDRV_LDO_DEACTIVATED_IND   /* RFU - Notify that the LDO is deactivated */
  /* New indications to be sent to QMI can be added as required */
}uimdrv_qmi_indications_type;

/* ---------------------------------------------------------------
   ENUM:      uimdrv_qmi_acknowledgements_type

   DESCRIPTION:
      Enum to depict the ack's from QMI_UIM for the indications
      that were sent earlier to QMI_UIM.
   ----------------------------------------------------------------*/
typedef enum
{
  UIMDRV_QMI_INVALID_IND_ACK,
  UIMDRV_QMI_IND_ACK_NOT_REQUIRED = UIMDRV_QMI_INVALID_IND_ACK,
  /* ACKS to Indications sent to QMI */
  UIMDRV_LDO_AWAITING_DEACTIVATION_IND_ACK, /* ACK for UIMDRV_LDO_DEACTIVATION_IND that was sent to QMI_UIM */
  /* New ACK's awaited from QMI can be added as required */
}uimdrv_qmi_acknowledgements_type;

/* Callback for QMI -  To pass the ACK from AP for the LDO indications sent by us.
   Indications are sent by us to AP prior to deactivating the LDO and after
   reactivation of the LDO. */
typedef void (*uimdrv_qmi_power_management_callback_type) (uim_slot_type  uim_slot,
                                                           uimdrv_qmi_acknowledgements_type  ack);

/* Callback to configure QMI with UIM and providing current slot information*/
typedef void uim_evt_callback_type (const uint8 num_physical_slots,
                                     const uim_phy_slot_status_type   *curr_slot_status);


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION UIM_POWER_CONTROL_DS

DESCRIPTION
  This procedure is the voting interface for modules to indicate their pref as
  to whether they would like to keep the UIM powered ON or OFF.

DEPENDENCIES
  Poll time delay to be used.

RETURN VALUE
  None.

SIDE EFFECTS
  Internally sets the UIM_TOGLLE_STATE_SIG and could change the state of UIM
  as a result.

===========================================================================*/
dword uim_power_control_ds(
  uim_slot_type  slot,
  uim_voter_type mask,
  boolean        uim_reqd,
  uint16         uim_call_mask
);


/*===========================================================================

FUNCTION UIM_IS_HOTSWAP_ENABLED_SLOT

DESCRIPTION
  This procedure returns TRUE if given slot have the hot swap capability
  else it return false.

DEPENDENCIES
  None

RETURN VALUE
  Returns boolean.

SIDE EFFECTS
  None

===========================================================================*/
boolean uim_is_hotswap_enabled_slot
(
  uim_slot_type uim_slot
);

/*===========================================================================

FUNCTION UIM_GET_HW_SLOT_INFO

DESCRIPTION
  1. Exposed api for other uim modules to get the number of active UIM
     interfaces.
  2. This function invokes uimdrv_hw_nv_configuration() which
     reads nv UIM_HW_CONFIG_NV_EF that is used to configure UIMDRV.
  3. The NV is read only once and there after only the hw slot information
     is returned that is of type uim_hw_slot_info_type.

DEPENDENCIES
  None

RETURN VALUE
  Returns uim_hw_slot_info_type.

SIDE EFFECTS
  None

===========================================================================*/
uim_hw_slot_info_type uim_get_hw_slot_info
(
   void
);

/*===========================================================================

FUNCTION UIM_IS_USB_UICC_SUPPORTED

DESCRIPTION
  whether USB UICC is supported or not.

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/
extern boolean uim_is_usb_uicc_supported
(
  void
);

/*===========================================================================

FUNCTION UIM_IS_SAP_CLIENT_SUPPORTED

DESCRIPTION
  whether SAP CLIENT is supported or not.

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/
extern boolean uim_is_sap_client_supported
(
  void
);

/*===========================================================================

FUNCTION UIM_CHECK_PENDING_PROACTIVE_COMMAND

DESCRIPTION
  This procedure return true if check pending proactive request
  is successfully sent to the UIM task.

DEPENDENCIES
  None

RETURN VALUE
  Returns boolean.

SIDE EFFECTS
  None

===========================================================================*/
boolean uim_check_pending_proactive_command
(
  uim_slot_type uim_slot
);
#endif /* UIM_P_H */
