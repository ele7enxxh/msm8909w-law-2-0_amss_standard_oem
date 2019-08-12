/*================================================================================

DESCRIPTION


Copyright (c) 2009-2016 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimqmi/src/qmi_uim_parsing.c#1 $$ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
10/14/16   bcho    Added support for ICCID simlock
10/25/16   dd      Added support for auto reg SMS when ESN is changed
09/06/16   ar      Addition of QMI_UIM_SEARCH_RECORD interface
08/18/16   vdc     Added support for simlock single lock
06/17/16   vdc     Added support for SPN based SIMLOCK feature
05/20/16   ar      Reducing F3s for the parsing error
05/06/16   sp      Remove Excessive F3's
05/05/16   vdc     Send control key presence in get config presence
05/02/16   ar      Check service availability before allowing EF GSM MBI read
04/11/16   ar      Changes for LPA support
11/18/15   vdc     Fix parsing logic for CP & SP personalization
11/16/15   vdc     Decrease the check for minimum perso code len to 15
12/10/14   vdc     Added support for ERA-GLONASS
08/20/15   at      Replacement of Secapis to use a derived key & HMAC
08/10/15   stv     To send temporary unlock status indication
07/01/15   bcho    Fix enabling a lock with more than 20 codes of lock data
07/08/15   bcho    Redundant checks for SIMLOCK
05/15/15   vv      Support for RSU
05/14/15   ar      Support for terminate_app TLV in qmi_uim_close_session
05/04/15   vv      Support for more than 1 bytes of GID1/GID2
05/01/15   vdc     Added support for APN for bootstrapping
05/01/15   vdc     Added support for QMI UIM GBA
03/30/15   ar      Support for cross-mapping in multisim_auto_provisioning NV
03/19/15   at      Introducing new relock feature in SIMLock
02/24/15   vdc     Add USIM MBI EF to USIM path/enum conversion table
02/16/15   ar      Pass large buffer to SECAPI for decryption
02/03/15   xj      Add USB UICC and SAP CLIENT supported interface
12/16/14   av      Apply auth restr via NV for IIC only if USIM is selected
11/14/14   hh      Fix compilation warnings
10/14/14   tl      Add featurization to strip encryption and decryption
10/14/14   vv      Fix decrypting the simlock config operation
10/13/14   tl      Change error returned when CK length is zero
10/10/14   vv      Fix a merge error
10/10/14   vv      Add support for remote simlock configuration
10/08/14   vdc     Added support for UICC activation status indication
09/30/14   vv      Add support for emergency only command
09/25/14   tl      Add support for secure read
09/22/14   tl      Assign the decrypted PIN limit before decryption
09/17/14   tl      Add support for perso and deperso secure
08/29/14   tl      Add support for supply voltage command and indication
08/11/14   vv      Fixed compiler warnings
07/25/14   vv      Added support for perso command
07/22/14   at      Fix error in ISIM Auth response for different contexts
06/30/14   vv      Remove perso code
06/12/14   av      Enhance GET_SERVICE_STATUS to accomodate all service numbers
06/12/14   av      Cache mandatory EFs used by NVRUIM for critical 1x NV reads
05/22/14   tl      Added support for recovery complete indications
05/13/14   yt      Add CDMA PRL and EPRL files to RUIM enum-to-path table
05/13/14   vdc     Support for reduced card status indications
05/12/14   df      Add QMI support for status cmd
04/03/14   yt      Calculate card status TLV length based on number of apps
03/26/14   tl      SIM Lock secondary revisions
03/18/14   tl      Introduce new SIM Lock feature
02/25/14   yt      Support for QMI_UIM_RESELCT_REQ
02/25/14   bcho    Added support to store OPLMN List in EFS and cache it
02/24/14   at      Support for read transparent long indication
01/10/14   df      Use v2.0 diag macros
12/16/13   yt      Support for ISIM AUTH in HTTP digest security context
12/09/13   spo     Fix for handling auth request for USIM and ISIM
11/25/13   ar      Added Conversion from Path to Enum in CSIM table
11/20/13   tkl     Added API for Increase
10/09/13   spo     Fixed conversion of file access method from enum to path
10/09/13   spo     Fixed conversion from path to enum for third slot
10/04/13   vv      Added API to return se13 table release date and source
09/23/13   df      Support for SIM busy
09/16/13   yt      Remove redundant code
09/11/13   av      Handle new hotswap status CARD_UNKNOWN
09/02/13   df      Correct comments
08/08/13   ks      Support for subscription manager
07/26/13   av      Fix qmi_uim_tlv_aid to validate against the proper max AID
07/10/13   vdc     Added support for missing triple sim instances
07/02/13   yt      Dynamically allocate slot and app data
07/02/13   kb      Fix compilation error
06/28/13   kb      Added QMI malloc and free wrapper functions
06/27/13   spo     Added support for files under DF TELECOM
06/25/13   av      Add CSIM/CDMA SF_EUIMID to path conversion tables for caching
06/25/13   av      Support path to enum conversion for 7F66_PROP1_ACT_HPLMN EFs
06/21/13   kk      Updated support for triple sim
06/19/13   vv      Do not udpate PIN2 state based on app state
05/21/13   vv      Extend session closed indication for all sessions
04/30/13   spo     Replaced instances of memcpy with memscpy
02/22/13   yt      Support for slot 3
02/12/13   at      Added support for file status TLV
02/07/13   yt      Support for activation/deactivation of EFs
12/07/12   tl      Add SST/UST/CST EFs to path/enum conversion tables
11/09/12   yt      Move QMI UIM to UIM heap
11/02/12   yt      Add support for ACL in SET and GET SERVICE STATUS commands
09/10/12   at      Fixed compiler warnings
09/07/12   at      Removal of private DS functions and headers
09/07/12   at      Fixed parsing of SAP Connection Condition TLV
09/07/12   tl      Added support for SGLTE dual slots
08/29/12   at      Optional TlV in SAP connection req to check for active calls
08/28/12   spo     Added support for Session Closed Indication
08/16/12   at      New command handling for QMI_UIM_REFRESH_REGISTER_ALL
08/16/12   tl      Add session close without app termination functionality
05/23/12   hn      Enable 1-byte alignment packing on Windows environment
05/18/12   tl      Fixed Run Cave Algo response package
05/14/12   at      Add new TLV to QMI_UIM_GET_CARD_STATUS command
04/27/12   av      Removed FEATURE_UIM_SUPPORT_HOTSWAP feature flag
04/11/12   at      Indication support for sending long APDUs
03/02/12   vv      Added optional TLV in send APDU request
03/02/12   vv      Added support for querying ESN status
02/27/12   yt      Add function to construct file_length TLV
02/23/12   nmb     QMI QMUX Transition
01/03/12   nb      Added optional TLV for QMI configuration
12/21/11   nmb     Secure channel perso for apps
12/21/11   shr     Fixed compilation errors - GSDI Removal
12/20/11   hn      Add support for off-target testing
10/12/11   at      Handling intermediate get response in SAP commands
10/05/11   yt      Added support for refresh of non provisioning sessions
                   Removed flag to indicate if TLV is mandatory or optional
09/19/11   yt      Fixed compilation error
09/16/11   yt      Added optional TLV for status of hot swap switch
08/16/11   at      Indication support for various commands
08/01/11   at      Support for keyref_id TLV in PIN commands
08/01/11   at      Support for using session id on an open channel id
08/01/11   vs      Updating generic response copy to avoid write overflow
06/27/11   at      Renamed qmi_uim_response_sap_generic()
05/13/11   nmb     Added support for ISIM GBA Authentication
04/28/11   mib     Fixed crash due to invalid free reading multiple records
04/26/11   vs      Support for subscription ok request
04/25/11   mib     Removed mapping of some TELECOM files into MMGSDI enum
04/07/11   mib     Added support to read multiple records at the same time
04/01/11   mib     Changed TLV for the encrypted PIN passed for verify
03/29/11   mib     Fixed possible memory leak in refresh registration
03/15/11   mib     Added support for ISIM app type
03/09/11   at      Added support for PIN1 encryption/decryption
03/03/11   mib     Added support to manage logical channels to send APDUs
03/01/11   at      Added support for SAP request & indication
01/12/11   mib     Added support for CSIM file paths
01/04/11   mib     Added support for Compute IP Authentication
12/21/10   at      Added support for SAP connection request
12/17/10   mib     Split APDU request into two TLVs
12/09/10   at      Added support for send APDU request
11/08/10   mib     Fixed Klokwork warning in refresh indication
11/02/10   mib     Added additional check on path length
10/22/10   mib     Added support for get configuration request
10/12/10   mib     Support for refresh by path
09/20/10   mib     Fixed authenticate response
09/09/10   mib     Changed PIN status in TLV when PIN is not initialized
08/25/10   yt      Updated file_type for MF and DF in get file attr response
08/05/10   at      Updated GSM & USIM EF mapping tables with new EFs
08/05/10   tkl     QMI UIM & QMI CAT split from data package
08/04/10   at      Fixed current pointer updation for register refresh
07/26/10   mib     Return QMI_ERR_INVALID_ARG for wrong path length
07/08/10   mib     Added check on slot 2 for single slot devices
07/08/10   mib     Added support for QMI_ERR_INVALID_ARG
07/07/10   yt      Updated TLV parsing for session change
07/07/10   mib     Better error handling for QMI_ERR_ARG_TOO_LONG
06/18/10   tkl     Added enums support for refresh and handle telecom files
                   conversion
06/18/10   mib     Fixed Klokwork warnings: uninitialized security mask
                   Fixed AID value in case of multiple applications
06/16/10   mib     Added support for hidden key
06/10/10   mib     Fixed number of retries for PIN operations
06/07/10   mib     Added support for GET LABEL and SESSION CHANGE requests
06/03/10   mib     Fixed compilation warnings
05/15/10   mib     Improved mechanism to pass TLV id for responses
05/13/10   mib     Fixed SW1/SW2 in the response
04/15/10   mib     Fixed session type for 1X provisioning.
04/06/10   mib     Added support for authenticate and close session.
                   Changed TLV ids for consistency with other QMI services
04/05/10   at      Fixed handling for TLV length in refresh indications
01/13/10   mib     Initial version
=============================================================================*/


#include "uim_variation.h"
#include "customer.h"
#include "uim_msg.h"

#include "comdef.h"
#include "dsm.h"
#include "amssassert.h"
#include "modem_mem.h"
#include <stringl/stringl.h>

#include "qmi_svc_defs.h"
#include "qmi_svc_utils.h"
#include "qmi_uim_parsing.h"
#include "qmi_uim_internal.h"
#include "qmi_uim_cat_common.h"
#include "qmi_uim_encryption.h"
#include "uimi.h"
#include "user_identity_module_v01.h"

/* These includes need to always be at bottom - offtarget use only */
#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/*---------------------------------------------------------------------------
  Minimum length of Request TLVs with variable size
---------------------------------------------------------------------------*/
#define QMI_UIM_TLV_MIN_LEN_SESSION_INFORMATION       2
#define QMI_UIM_TLV_MIN_LEN_FILE_ID                   3
#define QMI_UIM_TLV_MIN_LEN_SET_PIN_PROTECTION        3
#define QMI_UIM_TLV_MIN_LEN_VERIFY_PIN                2
#define QMI_UIM_TLV_MIN_LEN_UNBLOCK_PIN               3
#define QMI_UIM_TLV_MIN_LEN_CHANGE_PIN                3
#define QMI_UIM_TLV_MIN_LEN_DEPERSONALIZATION         3
#define QMI_UIM_TLV_MIN_LEN_APPLICATION_INFORMATION   2
#define QMI_UIM_TLV_MIN_LEN_AID                       1
#define QMI_UIM_TLV_MIN_LEN_ENCRYPTED_PIN             1
#define QMI_UIM_TLV_MAX_ENCRYPTED_IMSI_LEN            255


/*---------------------------------------------------------------------------
  Get Config Mask Values
---------------------------------------------------------------------------*/
#define UIM_GET_CONFIGURATION_AUTOMATIC_SELECTION    (0x01)
#define UIM_GET_CONFIGURATION_PERSONALIZATION_STATUS (0x02)
#define UIM_GET_CONFIGURATION_HALT_SUBSCRIPTION      (0x04)
#define UIM_GET_CONFIGURATION_USB_UICC_SUPPORTED     (0x08)
#define UIM_GET_CONFIGURATION_SAP_CLIENT_SUPPORTED   (0x10)

/*---------------------------------------------------------------------------
  Length of ICCID
---------------------------------------------------------------------------*/
#define QMI_UIM_MAX_ICCID_SIZE                        10

/*---------------------------------------------------------------------------
  Maximum size of lock codes
---------------------------------------------------------------------------*/
#define QMI_UIM_TLV_PERSO_NUM_NW_MAX             (85)
#define QMI_UIM_TLV_PERSO_NUM_NS_MAX             (64)
#define QMI_UIM_TLV_PERSO_NUM_SP_MAX             (64)
#define QMI_UIM_TLV_PERSO_NUM_CP_MAX             (51)
#define QMI_UIM_TLV_PERSO_NUM_SIM_MAX            (32)
#define QMI_UIM_TLV_PERSO_NUM_SPN_MAX            (32)
#define QMI_UIM_TLV_PERSO_NUM_ICCID_MAX          (32)
#define QMI_UIM_TLV_PERSO_NUM_NW_TYPE2_MAX       (128)

/*---------------------------------------------------------------------------
  Minimum size of codes
---------------------------------------------------------------------------*/
/* The minimum len is derived with MNC_LEN as 2.
   For NW_CODE: 3 (MCC Bytes) + 1 (MNC_LEN Byte) + 2 (MNC Bytes) */
#define QMI_UIM_TLV_NW_CODE_MIN_LEN            (6)
#define QMI_UIM_TLV_NS_CODE_MIN_LEN            (8)
#define QMI_UIM_TLV_SP_CODE_MIN_LEN            (7)
#define QMI_UIM_TLV_CP_CODE_MIN_LEN            (8)
#define QMI_UIM_TLV_SIM_CODE_MIN_LEN           (6)
#define QMI_UIM_TLV_NW_TYPE2_CODE_MIN_LEN      (4)
#define QMI_UIM_TLV_SPN_CODE_MIN_LEN           (22)
#define QMI_UIM_TLV_ICCID_CODE_MIN_LEN         (8)

/*---------------------------------------------------------------------------
  Minimum and Maximum MNC lengths
---------------------------------------------------------------------------*/
#define QMI_UIM_SIMLOCK_MNC_MIN                (2)
#define QMI_UIM_SIMLOCK_MNC_MAX                (3)

/*---------------------------------------------------------------------------
  Encypted/Decrypted magnitudes
---------------------------------------------------------------------------*/
#define QMI_UIM_DECRYPTED_PERSO_SECURE_REQ_MIN_LEN             (15)
#define QMI_UIM_DECRYPT_PERSO_SECURE_SLOT_OFFSET               (8)
#define QMI_UIM_DECRYPT_PERSO_SECURE_CATEGORY_OFFSET           (9)
#define QMI_UIM_DECRYPT_PERSO_SECURE_NUM_CODES_OFFSET          (10)
#define QMI_UIM_DECRYPT_PERSO_CODE_DATA_OFFSET                 (11)
#define QMI_UIM_DECRYPTED_DEPERSO_SECURE_REQ_LEN               (15)
#define QMI_UIM_DECRYPT_DEPERSO_SECURE_SLOT_OFFSET             (8)
#define QMI_UIM_DECRYPT_DEPERSO_SECURE_CATEGORY_OFFSET         (9)
#define QMI_UIM_DECRYPT_DEPERSO_SECURE_OPERATION_OFFSET        (10)
#define QMI_UIM_DECRYPT_DEPERSO_SECURE_DURATION_OFFSET         (11)
#define QMI_UIM_DECRYPTED_RESPONSE_LEN                         (12)
#define QMI_UIM_ENCRYPTED_RESP_MAX                             (1024)
#define QMI_UIM_ENCRYPTED_RESULT_SUCCESS                       (0)
#define QMI_UIM_ENCRYPTED_RESULT_FAILURE                       (1)
#define QMI_UIM_ENCRYPTED_MSG_RFU_SIZE                         (4)
#define QMI_UIM_DECRYPTED_EMERGENCY_ONLY_REQ_LEN               (9)
#define QMI_UIM_DECRYPT_EMERGENCY_ONLY_MODE_OFFSET             (8)
#define QMI_UIM_DECRYPTED_SIMLOCK_CONFIG_MIN_LEN               (9)
#define QMI_UIM_DECRYPT_SIMLOCK_CONFIG_OPERATION_OFFSET        (8)
#define QMI_UIM_DECRYPT_SIMLOCK_CONFIG_RESULT_OFFSET           (9)
#define QMI_UIM_DECRYPT_SIMLOCK_CONFIG_FILE_ID_OFFSET          (10)
#define QMI_UIM_DECRYPT_SIMLOCK_CONFIG_READ_LEN_OFFSET         (11)
#define QMI_UIM_DECRYPT_SIMLOCK_CONFIG_READ_DATA_OFFSET        (13)
#define QMI_UIM_DECRYPTED_SIMLOCK_CONFIG_LEN                   (11)
#define QMI_UIM_DECRYPTED_SIMLOCK_CONFIG_READ_LEN              (13)

/*---------------------------------------------------------------------------
  Maxmimum size represented by a unsigned 16 bit value
---------------------------------------------------------------------------*/
#define QMI_UIM_MAX_UINT16                                     (65535)

/*---------------------------------------------------------------------------
  The number of lists needed to accomodate the lock codes.
  Each list can fit SIMLOCK_NUM_CODES_MAX (20) codes.
---------------------------------------------------------------------------*/
#define QMI_UIM_PERSO_GET_LIST_COUNT(x)  ((x > 0) ? ((x - 1) / SIMLOCK_NUM_CODES_MAX + 1) : 0)

/*---------------------------------------------------------------------------
  Structure with map between file path and file enum
---------------------------------------------------------------------------*/
typedef struct
{
  uint16                 file_id;
  mmgsdi_file_enum_type  mmgsdi_enum;
} qmi_uim_ef_map_type;

/*---------------------------------------------------------------------------
  Structure with map between lock category and min/max codes
---------------------------------------------------------------------------*/
typedef struct
{
  simlock_category_enum_type     category;
  uint8                          num_codes_max;
  uint8                          code_data_min;
} qmi_uim_category_code_size_map_type;

/*---------------------------------------------------------------------------
  Structure used for TLV composition of card status
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint8  card_state;
  uint8  upin_state;
  uint8  upin_retries;
  uint8  upuk_retries;
  uint8  error_code;
  uint8  num_app;
} tlv_data_card_state_type;

typedef PACKED struct PACKED_POST
{
  uint8  app_type;
  uint8  app_state;
  uint8  perso_state;
  uint8  perso_feature;
  uint8  perso_retries;
  uint8  perso_unblock_retries;
  uint8  aid_len;
} tlv_data_app_state_top_type;

typedef PACKED struct PACKED_POST
{
  uint8  univ_pin;
  uint8  pin1_state;
  uint8  pin1_retries;
  uint8  puk1_retries;
  uint8  pin2_state;
  uint8  pin2_retries;
  uint8  puk2_retries;
} tlv_data_app_state_bottom_type;

/*---------------------------------------------------------------------------
  Tables with map between file path and file enum
---------------------------------------------------------------------------*/
#define QMI_UIM_GSM_TABLE(table, path)          \
         const uint16 path[] =                  \
         { 0x3F00, 0x7F20 };                    \
         const qmi_uim_ef_map_type table[] =    \
         {                                      \
           { 0x6F05 , MMGSDI_GSM_LP          }, \
           { 0x6F07 , MMGSDI_GSM_IMSI        }, \
           { 0x6F13 , MMGSDI_GSM_CFF         }, \
           { 0x6F14 , MMGSDI_GSM_ONS         }, \
           { 0x6F15 , MMGSDI_GSM_CSP         }, \
           { 0x6F20 , MMGSDI_GSM_KC          }, \
           { 0x6F30 , MMGSDI_GSM_PLMN        }, \
           { 0x6F31 , MMGSDI_GSM_HPLMN       }, \
           { 0x6F37 , MMGSDI_GSM_ACM_MAX     }, \
           { 0x6F38 , MMGSDI_GSM_SST         }, \
           { 0x6F39 , MMGSDI_GSM_ACM         }, \
           { 0x6F3E , MMGSDI_GSM_GID1        }, \
           { 0x6F3F , MMGSDI_GSM_GID2        }, \
           { 0x6F46 , MMGSDI_GSM_SPN         }, \
           { 0x6F41 , MMGSDI_GSM_PUCT        }, \
           { 0x6F45 , MMGSDI_GSM_CBMI        }, \
           { 0x6F74 , MMGSDI_GSM_BCCH        }, \
           { 0x6F78 , MMGSDI_GSM_ACC         }, \
           { 0x6F7B , MMGSDI_GSM_FPLMN       }, \
           { 0x6F7E , MMGSDI_GSM_LOCI        }, \
           { 0x6FAD , MMGSDI_GSM_AD          }, \
           { 0x6FAE , MMGSDI_GSM_PHASE       }, \
           { 0x6FB1 , MMGSDI_GSM_VGCS        }, \
           { 0x6FC5 , MMGSDI_GSM_PNN         }, \
           { 0x6FC6 , MMGSDI_GSM_OPL         }, \
           { 0x6FC7 , MMGSDI_GSM_MBDN        }, \
           { 0x6FC8 , MMGSDI_GSM_EXT6        }, \
           { 0x6FC9 , MMGSDI_GSM_MBI         }, \
           { 0x6FB2 , MMGSDI_GSM_VGCSS       }, \
           { 0x6FB3 , MMGSDI_GSM_VBS         }, \
           { 0x6FB4 , MMGSDI_GSM_VBSS        }, \
           { 0x6FB5 , MMGSDI_GSM_EMLPP       }, \
           { 0x6FB6 , MMGSDI_GSM_AAEM        }, \
           { 0x6F50 , MMGSDI_GSM_CBMIR       }, \
           { 0x6F32 , MMGSDI_GSM_CNL         }, \
           { 0x6F51 , MMGSDI_GSM_NIA         }, \
           { 0x6F63 , MMGSDI_GSM_CPBCCH      }, \
           { 0x6FCA , MMGSDI_GSM_MWIS        }, \
           { 0x6FCD , MMGSDI_GSM_SPDI        }, \
           { 0x6FCB , MMGSDI_GSM_CFIS        }, \
           { 0x6F61 , MMGSDI_GSM_OPLMNWACT   }  \
         };

#define QMI_UIM_RUIM_TABLE(table, path)                     \
         const uint16 path[] =                              \
         { 0x3F00, 0x7F25 };                                \
         const qmi_uim_ef_map_type table[] =                \
         {                                                  \
           { 0x6F22 , MMGSDI_CDMA_IMSI_M                 }, \
           { 0x6F23 , MMGSDI_CDMA_IMSI_T                 }, \
           { 0x6F41 , MMGSDI_CDMA_HOME_SVC_PVDR_NAME     }, \
           { 0x6F74 , MMGSDI_CDMA_SF_EUIM_ID             }, \
           { 0x6F30 , MMGSDI_CDMA_PRL                    }, \
           { 0x6F5A , MMGSDI_CDMA_EPRL                   }  \
         };

#define QMI_UIM_USIM_TABLE(table, path)         \
         const uint16 path[] =                  \
         { 0x3F00, 0x7FFF };                    \
         const qmi_uim_ef_map_type table[] =    \
         {                                      \
           { 0x6F05 , MMGSDI_USIM_LI         }, \
           { 0x6F07 , MMGSDI_USIM_IMSI       }, \
           { 0x6F13 , MMGSDI_USIM_CFF        }, \
           { 0x6F14 , MMGSDI_USIM_ONS        }, \
           { 0x6F15 , MMGSDI_USIM_CSP        }, \
           { 0x6F20 , MMGSDI_USIM_KEYS       }, \
           { 0x6F08 , MMGSDI_USIM_KEYS       }, \
           { 0x6F09 , MMGSDI_USIM_KEYSPS     }, \
           { 0x6F60 , MMGSDI_USIM_PLMNWACT   }, \
           { 0x6F30 , MMGSDI_USIM_PLMNWACT   }, \
           { 0x6F31 , MMGSDI_USIM_HPLMN      }, \
           { 0x6F37 , MMGSDI_USIM_ACM_MAX    }, \
           { 0x6F38 , MMGSDI_USIM_UST        }, \
           { 0x6F39 , MMGSDI_USIM_ACM        }, \
           { 0x6F3E , MMGSDI_USIM_GID1       }, \
           { 0x6F3F , MMGSDI_USIM_GID2       }, \
           { 0x6F46 , MMGSDI_USIM_SPN        }, \
           { 0x6F41 , MMGSDI_USIM_PUCT       }, \
           { 0x6F45 , MMGSDI_USIM_CBMI       }, \
           { 0x6F78 , MMGSDI_USIM_ACC        }, \
           { 0x6F7B , MMGSDI_USIM_FPLMN      }, \
           { 0x6F7E , MMGSDI_USIM_LOCI       }, \
           { 0x6FAD , MMGSDI_USIM_AD         }, \
           { 0x6F48 , MMGSDI_USIM_CBMID      }, \
           { 0x6FB7 , MMGSDI_USIM_ECC        }, \
           { 0x6F50 , MMGSDI_USIM_CBMIR      }, \
           { 0x6F73 , MMGSDI_USIM_PSLOCI     }, \
           { 0x6F3B , MMGSDI_USIM_FDN        }, \
           { 0x6F3C , MMGSDI_USIM_SMS        }, \
           { 0x6F40 , MMGSDI_USIM_MSISDN     }, \
           { 0x6F42 , MMGSDI_USIM_SMSP       }, \
           { 0x6FC5 , MMGSDI_USIM_PNN        }, \
           { 0x6FC6 , MMGSDI_USIM_OPL        }, \
           { 0x6FC7 , MMGSDI_USIM_MBDN       }, \
           { 0x6FC8 , MMGSDI_USIM_EXT6       }, \
           { 0x6FCB , MMGSDI_USIM_CFIS       }, \
           { 0x6FCA , MMGSDI_USIM_MWIS       }, \
           { 0x6F4E , MMGSDI_USIM_EXT5       }, \
           { 0x6F49 , MMGSDI_USIM_SDN        }, \
           { 0x6F4B , MMGSDI_USIM_EXT2       }, \
           { 0x6F4C , MMGSDI_USIM_EXT3       }, \
           { 0x6F4D , MMGSDI_USIM_BDN        }, \
           { 0x6F55 , MMGSDI_USIM_EXT4       }, \
           { 0x6F81 , MMGSDI_USIM_OCI        }, \
           { 0x6F80 , MMGSDI_USIM_ICI        }, \
           { 0x6F47 , MMGSDI_USIM_SMSR       }, \
           { 0x6FB5 , MMGSDI_USIM_EMLPP      }, \
           { 0x6FB6 , MMGSDI_USIM_AAEM       }, \
           { 0x6F56 , MMGSDI_USIM_EST        }, \
           { 0x6F57 , MMGSDI_USIM_ACL        }, \
           { 0x6F32 , MMGSDI_USIM_CNL        }, \
           { 0x6FCD , MMGSDI_USIM_SPDI       }, \
           { 0x6FCF , MMGSDI_USIM_EXT8       }, \
           { 0x6FD6 , MMGSDI_USIM_GBABP      }, \
           { 0x6FDB , MMGSDI_USIM_EHPLMNPI   }, \
           { 0x6FDC , MMGSDI_USIM_LRPLMNSI   }, \
           { 0x6FDE , MMGSDI_USIM_SPNI       }, \
           { 0x6FDF , MMGSDI_USIM_PNNI       }, \
           { 0x6FE2 , MMGSDI_USIM_NCP_IP     }, \
           { 0x6F62 , MMGSDI_USIM_HPLMNWACT  }, \
           { 0x6F4F , MMGSDI_USIM_CCP2       }, \
           { 0x6FC3 , MMGSDI_USIM_HIDDENKEY  }, \
           { 0x6F61 , MMGSDI_USIM_OPLMNWACT  }, \
           { 0x6FC9 , MMGSDI_USIM_MBI        }  \
         };

#define QMI_UIM_CSIM_TABLE(table, path)          \
         const uint16 path[] =                   \
         { 0x3F00, 0x7FFF };                     \
         const qmi_uim_ef_map_type table[] =     \
         {                                       \
           { 0x6F22 , MMGSDI_CSIM_IMSI_M      }, \
           { 0x6F23 , MMGSDI_CSIM_IMSI_T      }, \
           { 0x6F31 , MMGSDI_CSIM_RUIM_ID     }, \
           { 0x6F32 , MMGSDI_CSIM_CST         }, \
           { 0x6F75 , MMGSDI_CSIM_EST         }, \
           { 0x6F3B , MMGSDI_CSIM_FDN         }, \
           { 0x6F44 , MMGSDI_CSIM_MSISDN      }, \
           { 0x6F30 , MMGSDI_CSIM_PRL         }, \
           { 0x6F5A , MMGSDI_CSIM_EPRL        }, \
           { 0x6F45 , MMGSDI_CSIM_MAXIMUM_PRL }, \
           { 0x6F7A , MMGSDI_CSIM_EXT2        }, \
           { 0x6F7B , MMGSDI_CSIM_EXT3        }, \
           { 0x6F69 , MMGSDI_CSIM_MMS_UCP     }, \
           { 0x6F6D , MMGSDI_CSIM_GID1        }, \
           { 0x6F6E , MMGSDI_CSIM_GID2        }, \
           { 0x6F6F , MMGSDI_CSIM_CNL         }, \
           { 0x6F7D , MMGSDI_CSIM_OCI         }, \
           { 0x6F7C , MMGSDI_CSIM_ICI         }, \
           { 0x6F7E , MMGSDI_CSIM_EXT5        }, \
           { 0x6F8D , MMGSDI_CSIM_JDL         }, \
           { 0x6F41 , MMGSDI_CSIM_SPN         }, \
           { 0x6F74 , MMGSDI_CSIM_SF_EUIM_ID  }, \
           { 0x6F47 , MMGSDI_CSIM_ECC         }, \
           { 0x6F3A , MMGSDI_CSIM_LANG_IND    }, \
           { 0x6F76 , MMGSDI_CSIM_HIDDEN_KEY  }  \
         };

#define QMI_UIM_MF_TABLE(table, path)          \
         const uint16 path[]  =                \
         { 0x3F00 };                           \
         const qmi_uim_ef_map_type table[] =   \
         {                                     \
           { 0x2FE2, MMGSDI_ICCID     },       \
           { 0x2F05, MMGSDI_ELP       },       \
           { 0x2F00, MMGSDI_DIR       },       \
           { 0x2F06, MMGSDI_ARR       }        \
         };

#define QMI_UIM_TELECOM_TABLE(table, path)      \
         const uint16 path[] =                  \
         { 0x3F00, 0x7F10 };                    \
         const qmi_uim_ef_map_type table[] =    \
         {                                      \
           { 0x6F3B , MMGSDI_TELECOM_FDN     }, \
           { 0x6F3C , MMGSDI_TELECOM_SMS     }, \
           { 0x6F4F , MMGSDI_TELECOM_ECCP    }, \
           { 0x6F40 , MMGSDI_TELECOM_MSISDN  }, \
           { 0x6F42 , MMGSDI_TELECOM_SMSP    }, \
           { 0x6F43 , MMGSDI_TELECOM_SMSS    }, \
           { 0x6F44 , MMGSDI_TELECOM_LND     }, \
           { 0x6F49 , MMGSDI_TELECOM_SDN     }, \
           { 0x6F4C , MMGSDI_TELECOM_EXT3    }, \
           { 0x6F4D , MMGSDI_TELECOM_BDN     }, \
           { 0x6F4E , MMGSDI_TELECOM_EXT4    }, \
           { 0x6F47 , MMGSDI_TELECOM_SMSR    }, \
           { 0x6FE0 , MMGSDI_TELECOM_ICE_DN  }, \
           { 0x6FE1 , MMGSDI_TELECOM_ICE_FF  }  \
         };

#define QMI_UIM_TELECOM_GRAPHICS_TABLE(table, path)     \
         const uint16 path[] =                          \
         { 0x3F00, 0x7F10, 0x5F50 };                    \
         const qmi_uim_ef_map_type table[] =            \
         {                                              \
           { 0x4F21, MMGSDI_TELECOM_ICE_GRAPHICS }      \
         };

#define QMI_UIM_PROP_7F66_2G_TABLE(table, path)          \
         const uint16 path[] =                           \
         { 0x3F00, 0x7F66, 0x5F30 };                     \
         const qmi_uim_ef_map_type table[] =             \
         {                                               \
           { 0x4F34, MMGSDI_SIM_7F66_PROP1_ACT_HPLMN  }  \
         };

#define QMI_UIM_PROP_7F66_3G_TABLE(table, path)         \
         const uint16 path[] =                          \
         { 0x3F00, 0x7FFF, 0x7F66, 0x5F30 };            \
         const qmi_uim_ef_map_type table[] =            \
         {                                              \
           { 0x4F34, MMGSDI_USIM_7F66_PROP1_ACT_HPLMN } \
         };

/*---------------------------------------------------------------------------
  Table with map between lock category and min/max code sizes
---------------------------------------------------------------------------*/
static const qmi_uim_category_code_size_map_type qmi_uim_simlock_category_table[] =
{
  {SIMLOCK_CATEGORY_3GPP_NW,         QMI_UIM_TLV_PERSO_NUM_NW_MAX,        QMI_UIM_TLV_NW_CODE_MIN_LEN},
  {SIMLOCK_CATEGORY_3GPP_NS,         QMI_UIM_TLV_PERSO_NUM_NS_MAX,        QMI_UIM_TLV_NS_CODE_MIN_LEN},
  {SIMLOCK_CATEGORY_3GPP_SP,         QMI_UIM_TLV_PERSO_NUM_SP_MAX,        QMI_UIM_TLV_SP_CODE_MIN_LEN},
  {SIMLOCK_CATEGORY_3GPP_CP,         QMI_UIM_TLV_PERSO_NUM_CP_MAX,        QMI_UIM_TLV_CP_CODE_MIN_LEN},
  {SIMLOCK_CATEGORY_3GPP_SIM,        QMI_UIM_TLV_PERSO_NUM_SIM_MAX,       QMI_UIM_TLV_SIM_CODE_MIN_LEN},
  {SIMLOCK_CATEGORY_3GPP2_NW_TYPE1,  QMI_UIM_TLV_PERSO_NUM_NW_MAX,        QMI_UIM_TLV_NW_CODE_MIN_LEN},
  {SIMLOCK_CATEGORY_3GPP2_NW_TYPE2,  QMI_UIM_TLV_PERSO_NUM_NW_TYPE2_MAX,  QMI_UIM_TLV_NW_TYPE2_CODE_MIN_LEN},
  {SIMLOCK_CATEGORY_3GPP2_RUIM,      QMI_UIM_TLV_PERSO_NUM_SIM_MAX,       QMI_UIM_TLV_SIM_CODE_MIN_LEN},
  {SIMLOCK_CATEGORY_3GPP_SPN,        QMI_UIM_TLV_PERSO_NUM_SPN_MAX,       QMI_UIM_TLV_SPN_CODE_MIN_LEN},
  {SIMLOCK_CATEGORY_3GPP_ICCID,      QMI_UIM_TLV_PERSO_NUM_ICCID_MAX,     QMI_UIM_TLV_ICCID_CODE_MIN_LEN}
};


/*===========================================================================
  FUNCTION QMI_UIM_CONVERT_FILE_ACCESS_METHOD_TABLE()

  DESCRIPTION
    Converts file access method between path and MMGSDI enum
    using a specified input table

  PARAMETERS
    file_access_ptr : input/output with file value
    access_method   : desired output access method
    table_ptr       : table to be used for the conversion
    table_size      : table size

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_uim_convert_file_access_method_table
(
  mmgsdi_access_type             * file_access_ptr,
  mmgsdi_access_method_enum_type   access_method,
  const qmi_uim_ef_map_type      * table_ptr,
  uint16                           table_size,
  const uint16                   * table_path_ptr,
  uint8                            table_path_len
)
{
  uint32 index    = 0;

  ASSERT(file_access_ptr);
  ASSERT(table_ptr);
  ASSERT(table_size > 0);
  ASSERT(table_path_ptr);
  ASSERT(table_path_len < MMGSDI_MAX_PATH_LEN);


  /* Check file access method */
  if (file_access_ptr->access_method == access_method)
  {
    /* Input format matches output format, so we can
       immediately return success */
    UIM_MSG_HIGH_0("No file access conversion required");
    return TRUE;
  }

  if (file_access_ptr->access_method == MMGSDI_BY_PATH_ACCESS &&
      access_method == MMGSDI_EF_ENUM_ACCESS)
  {
    /* Conversion from PATH to ENUM */

    /* Check length */
    if ((file_access_ptr->file.path_type.path_len != table_path_len + 1) ||
        memcmp(file_access_ptr->file.path_type.path_buf, table_path_ptr, table_path_len * sizeof(uint16)) != 0)
    {
      return FALSE;
    }

    /* loop on all items in the table */
    for (index = 0; index < table_size; index++)
    {
      if (table_ptr[index].file_id == file_access_ptr->file.path_type.path_buf[table_path_len])
      {
        file_access_ptr->access_method = MMGSDI_EF_ENUM_ACCESS;
        file_access_ptr->file.file_enum = table_ptr[index].mmgsdi_enum;

        return TRUE;
      }
    }
    UIM_MSG_HIGH_0("QMI_UIM conversion from path to enum failed");
  }
  else if (file_access_ptr->access_method == MMGSDI_EF_ENUM_ACCESS &&
           access_method == MMGSDI_BY_PATH_ACCESS)
  {
    /* Conversion from ENUM to PATH */

    /* loop on all items in the table */
    for (index = 0; index < table_size; index++)
    {
      if (table_ptr[index].mmgsdi_enum == file_access_ptr->file.file_enum)
      {
        file_access_ptr->access_method = MMGSDI_BY_PATH_ACCESS;
        file_access_ptr->file.path_type.path_len = table_path_len + 1;
        (void)memscpy((void*)file_access_ptr->file.path_type.path_buf,
                      sizeof(file_access_ptr->file.path_type.path_buf),
                      (void*)table_path_ptr,
                      table_path_len * sizeof(uint16));
        file_access_ptr->file.path_type.path_buf[table_path_len] = table_ptr[index].file_id;

        return TRUE;
      }
    }
    UIM_MSG_HIGH_0("QMI_UIM conversion from enum to path failed");
  }
  else
  {
    /* Invalid conversion */
    UIM_MSG_HIGH_2("QMI_UIM conversion from 0x%x to 0x%x is impossible",
                   file_access_ptr->access_method, access_method);
  }

  return FALSE;
} /* qmi_uim_convert_file_access_method_table */


/*===========================================================================
  FUNCTION QMI_UIM_CONVERT_FILE_ACCESS_METHOD_TELECOM()

  DESCRIPTION
    Creates the table with TELECOM files and invokes the
    function to execute the conversion based on that table

  PARAMETERS
    file_access_ptr : input/output with file value
    access_method   : desired output access method

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_uim_convert_file_access_method_telecom
(
  mmgsdi_access_type             * file_access_ptr,
  mmgsdi_access_method_enum_type   access_method
)
{
  boolean result = FALSE;
  QMI_UIM_TELECOM_TABLE(telecom_table, telecom_path);

  ASSERT(file_access_ptr);

  result = qmi_uim_convert_file_access_method_table(
                file_access_ptr,
                access_method,
                telecom_table,
                sizeof(telecom_table) / sizeof(qmi_uim_ef_map_type),
                telecom_path,
                sizeof(telecom_path) / sizeof(uint16));

  return result;
} /* qmi_uim_convert_file_access_method_telecom */


/*===========================================================================
  FUNCTION QMI_UIM_CONVERT_FILE_ACCESS_METHOD_GSM()

  DESCRIPTION
    Creates the table with GSM files and invokes the
    function to execute the conversion based on that table

  PARAMETERS
    file_access_ptr : input/output with file value
    access_method   : desired output access method

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_uim_convert_file_access_method_gsm
(
  mmgsdi_access_type             * file_access_ptr,
  mmgsdi_access_method_enum_type   access_method
)
{
  boolean result = FALSE;
  QMI_UIM_GSM_TABLE(gsm_table, gsm_path);

  ASSERT(file_access_ptr);

  result = qmi_uim_convert_file_access_method_table(
                file_access_ptr,
                access_method,
                gsm_table,
                sizeof(gsm_table) / sizeof(qmi_uim_ef_map_type),
                gsm_path,
                sizeof(gsm_path) / sizeof(uint16));

  return result;
} /* qmi_uim_convert_file_access_method_gsm */


/*===========================================================================
  FUNCTION QMI_UIM_CONVERT_FILE_ACCESS_METHOD_RUIM()

  DESCRIPTION
    Creates the table with RUIM files and invokes the
    function to execute the conversion based on that table

  PARAMETERS
    file_access_ptr : input/output with file value
    access_method   : desired output access method

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_uim_convert_file_access_method_ruim
(
  mmgsdi_access_type             * file_access_ptr,
  mmgsdi_access_method_enum_type   access_method
)
{
  boolean result = FALSE;
  QMI_UIM_RUIM_TABLE(ruim_table, ruim_path);

  ASSERT(file_access_ptr);

  result = qmi_uim_convert_file_access_method_table(
                file_access_ptr,
                access_method,
                ruim_table,
                sizeof(ruim_table) / sizeof(qmi_uim_ef_map_type),
                ruim_path,
                sizeof(ruim_path) / sizeof(uint16));

  return result;
} /* qmi_uim_convert_file_access_method_ruim */


/*===========================================================================
  FUNCTION QMI_UIM_CONVERT_FILE_ACCESS_METHOD_USIM()

  DESCRIPTION
    Creates the table with USIM files and invokes the
    function to execute the conversion based on that table

  PARAMETERS
    file_access_ptr : input/output with file value
    access_method   : desired output access method

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_uim_convert_file_access_method_usim
(
  mmgsdi_access_type             * file_access_ptr,
  mmgsdi_access_method_enum_type   access_method
)
{
  boolean result = FALSE;
  QMI_UIM_USIM_TABLE(usim_table, usim_path);

  ASSERT(file_access_ptr);

  result = qmi_uim_convert_file_access_method_table(
                file_access_ptr,
                access_method,
                usim_table,
                sizeof(usim_table) / sizeof(qmi_uim_ef_map_type),
                usim_path,
                sizeof(usim_path) / sizeof(uint16));

  return result;
} /* qmi_uim_convert_file_access_method_usim */


/*===========================================================================
  FUNCTION QMI_UIM_CONVERT_FILE_ACCESS_METHOD_CSIM()

  DESCRIPTION
    Creates the table with CSIM files and invokes the
    function to execute the conversion based on that table

  PARAMETERS
    file_access_ptr : input/output with file value
    access_method   : desired output access method

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_uim_convert_file_access_method_csim
(
  mmgsdi_access_type             * file_access_ptr,
  mmgsdi_access_method_enum_type   access_method
)
{
  boolean result = FALSE;
  QMI_UIM_CSIM_TABLE(csim_table, csim_path);

  ASSERT(file_access_ptr);

  result = qmi_uim_convert_file_access_method_table(
                file_access_ptr,
                access_method,
                csim_table,
                sizeof(csim_table) / sizeof(qmi_uim_ef_map_type),
                csim_path,
                sizeof(csim_path) / sizeof(uint16));

  return result;
} /* qmi_uim_convert_file_access_method_csim */


/*===========================================================================
  FUNCTION QMI_UIM_CONVERT_FILE_ACCESS_METHOD_MF()

  DESCRIPTION
    Creates the table with miscellaneous files (files not under
    GSM, USIM or CDMA) and invokes the function to execute the
    conversion based on that table

  PARAMETERS
    file_access_ptr : input/output with file value
    access_method   : desired output access method

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_uim_convert_file_access_method_mf
(
  mmgsdi_access_type             * file_access_ptr,
  mmgsdi_access_method_enum_type   access_method
)
{
  boolean result = FALSE;
  QMI_UIM_MF_TABLE(mf_table, mf_path);

  ASSERT(file_access_ptr);

  result = qmi_uim_convert_file_access_method_table(
                file_access_ptr,
                access_method,
                mf_table,
                sizeof(mf_table) / sizeof(qmi_uim_ef_map_type),
                mf_path,
                sizeof(mf_path) / sizeof(uint16));

  return result;
} /* qmi_uim_convert_file_access_method_mf */


/*===========================================================================
  FUNCTION QMI_UIM_CONVERT_FILE_ACCESS_METHOD_PROP_7F66_2G()

  DESCRIPTION
    Creates the table with propprietary 2G files and invokes the
    function to execute the conversion based on that table

  PARAMETERS
    file_access_ptr : input/output with file value
    access_method   : desired output access method

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_uim_convert_file_access_method_prop_7f66_2g
(
  mmgsdi_access_type             * file_access_ptr,
  mmgsdi_access_method_enum_type   access_method
)
{
  boolean result = FALSE;
  QMI_UIM_PROP_7F66_2G_TABLE(prop_7f66_2g_table, prop_7f66_2g_path);

  ASSERT(file_access_ptr);

  result = qmi_uim_convert_file_access_method_table(
                file_access_ptr,
                access_method,
                prop_7f66_2g_table,
                sizeof(prop_7f66_2g_table) / sizeof(qmi_uim_ef_map_type),
                prop_7f66_2g_path,
                sizeof(prop_7f66_2g_path) / sizeof(uint16));

  return result;
} /* qmi_uim_convert_file_access_method_prop_7f66_2g */


/*===========================================================================
  FUNCTION QMI_UIM_CONVERT_FILE_ACCESS_METHOD_PROP_7F66_3G()

  DESCRIPTION
    Creates the table with proprietary 3G files and invokes the
    function to execute the conversion based on that table

  PARAMETERS
    file_access_ptr : input/output with file value
    access_method   : desired output access method

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_uim_convert_file_access_method_prop_7f66_3g
(
  mmgsdi_access_type             * file_access_ptr,
  mmgsdi_access_method_enum_type   access_method
)
{
  boolean result = FALSE;
  QMI_UIM_PROP_7F66_3G_TABLE(prop_7f66_3g_table, prop_7f66_3g_path);

  ASSERT(file_access_ptr);

  result = qmi_uim_convert_file_access_method_table(
                file_access_ptr,
                access_method,
                prop_7f66_3g_table,
                sizeof(prop_7f66_3g_table) / sizeof(qmi_uim_ef_map_type),
                prop_7f66_3g_path,
                sizeof(prop_7f66_3g_path) / sizeof(uint16));

  return result;
} /* qmi_uim_convert_file_access_method_prop_7f66_3g */


/*===========================================================================
  FUNCTION QMI_UIM_CONVERT_FILE_ACCESS_METHOD_TELECOM_GRAPHICS()

  DESCRIPTION
    Creates the table with Telecom Graphics files and invokes the
    function to execute the conversion based on that table

  PARAMETERS
    file_access_ptr : input/output with file value
    access_method   : desired output access method

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_uim_convert_file_access_method_telecom_graphics
(
  mmgsdi_access_type             * file_access_ptr,
  mmgsdi_access_method_enum_type   access_method
)
{
  boolean result = FALSE;
  QMI_UIM_TELECOM_GRAPHICS_TABLE(telecom_graphics_table, telecom_graphics_path);

  ASSERT(file_access_ptr);

  result = qmi_uim_convert_file_access_method_table(
                file_access_ptr,
                access_method,
                telecom_graphics_table,
                sizeof(telecom_graphics_table) / sizeof(qmi_uim_ef_map_type),
                telecom_graphics_path,
                sizeof(telecom_graphics_path) / sizeof(uint16));

  return result;
} /* qmi_uim_convert_file_access_method_telecom_graphics */


/*===========================================================================
  FUNCTION QMI_UIM_CONVERT_FILE_ACCESS_METHOD()

  DESCRIPTION
    Converts file access method between path and MMGSDI enum

  PARAMETERS
    file_access_ptr : input/output with file value
    access_method   : desired output access method
    session_type    : session type

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_convert_file_access_method
(
  mmgsdi_access_type             * file_access_ptr,
  mmgsdi_access_method_enum_type   access_method,
  mmgsdi_session_type_enum_type    session_type
)
{
  ASSERT(file_access_ptr);

  /* Check file access method */
  if (file_access_ptr->access_method == access_method)
  {
    /* Input format matches output format, so we can
       immediately return success */
    UIM_MSG_HIGH_0("No file access conversion required");
    return TRUE;
  }

  switch(file_access_ptr->access_method)
  {
    case MMGSDI_BY_PATH_ACCESS:
      if (file_access_ptr->file.path_type.path_len == 2 &&
          file_access_ptr->file.path_type.path_buf[0] == 0x3F00)
      {
        return qmi_uim_convert_file_access_method_mf(file_access_ptr,
                                                     access_method);
      }
      if (file_access_ptr->file.path_type.path_len == 3 &&
          file_access_ptr->file.path_type.path_buf[0] == 0x3F00)
      {
        /* Look at the second item in the path to understand which
           function is required */
        if (file_access_ptr->file.path_type.path_buf[1] == 0x7F10)
        {
          return qmi_uim_convert_file_access_method_telecom(file_access_ptr,
                                                            access_method);
        }
        if (file_access_ptr->file.path_type.path_buf[1] == 0x7F20)
        {
          return qmi_uim_convert_file_access_method_gsm(file_access_ptr,
                                                        access_method);
        }
        if (file_access_ptr->file.path_type.path_buf[1] == 0x7F25)
        {
          return qmi_uim_convert_file_access_method_ruim(file_access_ptr,
                                                         access_method);
        }
        if (file_access_ptr->file.path_type.path_buf[1] == 0x7FFF)
        {
          if (session_type == MMGSDI_GW_PROV_PRI_SESSION ||
              session_type == MMGSDI_GW_PROV_SEC_SESSION ||
              session_type == MMGSDI_GW_PROV_TER_SESSION)
          {
            return qmi_uim_convert_file_access_method_usim(file_access_ptr,
                                                           access_method);
          }
          else if (session_type == MMGSDI_1X_PROV_PRI_SESSION ||
                   session_type == MMGSDI_1X_PROV_SEC_SESSION ||
                   session_type == MMGSDI_1X_PROV_TER_SESSION)
          {
            return qmi_uim_convert_file_access_method_csim(file_access_ptr,
                                                           access_method);
          }
          else
          {
            /* For non-provisioning sessions, don't convert the path
               into enum: nothing to do! */
          }
        }
      }
      if (file_access_ptr->file.path_type.path_len == 4 &&
          file_access_ptr->file.path_type.path_buf[0] == 0x3F00 &&
          file_access_ptr->file.path_type.path_buf[1] == 0x7F66 &&
          file_access_ptr->file.path_type.path_buf[2] == 0x5F30)
      {
        return qmi_uim_convert_file_access_method_prop_7f66_2g(file_access_ptr,
                                                               access_method);
      }
      if (file_access_ptr->file.path_type.path_len == 5 &&
          file_access_ptr->file.path_type.path_buf[0] == 0x3F00 &&
          file_access_ptr->file.path_type.path_buf[1] == 0x7FFF &&
          file_access_ptr->file.path_type.path_buf[2] == 0x7F66 &&
          file_access_ptr->file.path_type.path_buf[3] == 0x5F30)
      {
        if (session_type == MMGSDI_GW_PROV_PRI_SESSION ||
            session_type == MMGSDI_GW_PROV_SEC_SESSION ||
            session_type == MMGSDI_GW_PROV_TER_SESSION)
        {
          return qmi_uim_convert_file_access_method_prop_7f66_3g(file_access_ptr,
                                                                 access_method);
        }
      }
      if (file_access_ptr->file.path_type.path_len == 4 &&
          file_access_ptr->file.path_type.path_buf[0] == 0x3F00 &&
          file_access_ptr->file.path_type.path_buf[1] == 0x7F10 &&
          file_access_ptr->file.path_type.path_buf[2] == 0x5F50)
      {
          return qmi_uim_convert_file_access_method_telecom_graphics(file_access_ptr,
                                                                     access_method);
      }
      break;

    case MMGSDI_EF_ENUM_ACCESS:
      /* Look at the range of the enum to understand which
         function is required */
      if (file_access_ptr->file.file_enum >= MMGSDI_ICCID &&
          file_access_ptr->file.file_enum <= MMGSDI_ARR)
      {
        return qmi_uim_convert_file_access_method_mf(file_access_ptr,
                                                     access_method);
      }
      if (file_access_ptr->file.file_enum >= MMGSDI_IMAGE_FILE &&
          file_access_ptr->file.file_enum <= MMGSDI_TELECOM_ICE_GRAPHICS)
      {
        return qmi_uim_convert_file_access_method_telecom_graphics(file_access_ptr,
                                                                   access_method);
      }
      if (file_access_ptr->file.file_enum >= MMGSDI_TELECOM_ADN &&
          file_access_ptr->file.file_enum <= MMGSDI_TELECOM_MMSSMODE)
      {
        return qmi_uim_convert_file_access_method_telecom(file_access_ptr,
                                                          access_method);
      }
      if (file_access_ptr->file.file_enum >= MMGSDI_GSM_LP &&
          file_access_ptr->file.file_enum <= MMGSDI_GSM_IMG3INST3)
      {
        return qmi_uim_convert_file_access_method_gsm(file_access_ptr,
                                                      access_method);
      }
      if (file_access_ptr->file.file_enum >= MMGSDI_CDMA_CC &&
          file_access_ptr->file.file_enum <= MMGSDI_CDMA_3GPD_OP_CAP)
      {
        return qmi_uim_convert_file_access_method_ruim(file_access_ptr,
                                                       access_method);
      }
      if (file_access_ptr->file.file_enum >= MMGSDI_USIM_LI &&
          file_access_ptr->file.file_enum <= MMGSDI_USIM_LRPLMNSI)
      {
        return qmi_uim_convert_file_access_method_usim(file_access_ptr,
                                                       access_method);
      }
      if (file_access_ptr->file.file_enum >= MMGSDI_CSIM_ARR &&
          file_access_ptr->file.file_enum <= MMGSDI_CSIM_UIM_VERSION)
      {
        return qmi_uim_convert_file_access_method_csim(file_access_ptr,
                                                       access_method);
      }
      if (file_access_ptr->file.file_enum >= MMGSDI_SIM_7F66_PROP1_ACT_HPLMN &&
          file_access_ptr->file.file_enum <= MMGSDI_SIM_7F66_PROP1_SPT_TABLE)
      {
        return qmi_uim_convert_file_access_method_prop_7f66_2g(file_access_ptr,
                                                               access_method);
      }
      if (file_access_ptr->file.file_enum >= MMGSDI_USIM_7F66_PROP1_ACT_HPLMN &&
          file_access_ptr->file.file_enum <= MMGSDI_USIM_7F66_PROP1_RAT)
      {
        return qmi_uim_convert_file_access_method_prop_7f66_3g(file_access_ptr,
                                                               access_method);
      }
      break;

    default:
      /* Not supported */
      break;
  }

  return FALSE;
} /* qmi_uim_convert_file_access_method */


/*===========================================================================
  FUNCTION QMI_UIM_CONVERT_SLOT_TO_INDEX()

  DESCRIPTION
    Converts MMGSDI slot information into index, starting from 0.

  PARAMETERS
    slot_id : MMGSDI slot information
    index   : output parameter with index value

  RETURN VALUE
    boolean to indicate success or failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_uim_convert_slot_to_index
(
  mmgsdi_slot_id_enum_type  slot_id,
  uint8 *                   index_ptr
)
{
  ASSERT(index_ptr);

  switch(slot_id)
  {
    case MMGSDI_SLOT_1:
      *index_ptr = 0;
      break;

    case MMGSDI_SLOT_2:
      *index_ptr = 1;
      break;

    case MMGSDI_SLOT_3:
      *index_ptr = 2;
      break;

    default:
      return FALSE;
  }

  if (*index_ptr >= QMI_UIM_MAX_CARD_COUNT)
  {
    return FALSE;
  }

  return TRUE;
} /* qmi_uim_convert_slot_to_index */


/*===========================================================================
  FUNCTION QMI_UIM_SIMLOCK_SLOT_INDEX_TO_UIM_SLOT()

  DESCRIPTION
    Determines the UIM slot id from the simlock slot index

  PARAMETERS
    slot_index          : slot index
    uim_slot_ptr        : output parameter containing uim slot

  RETURN VALUE
    QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_simlock_slot_index_to_uim_slot
(
  uint8                              slot_index,
  uim_slot_enum_v01                 *uim_slot_ptr
)
{
  ASSERT(uim_slot_ptr);

  switch(slot_index)
  {
    case 0:
      *uim_slot_ptr = UIM_SLOT_1_V01;
      break;

    case 1:
      *uim_slot_ptr = UIM_SLOT_2_V01;
      break;

    case 2:
      *uim_slot_ptr = UIM_SLOT_3_V01;
      break;

    default:
      return QMI_ERR_INTERNAL;
  }

  return QMI_ERR_NONE;
}/* qmi_uim_simlock_slot_index_to_uim_slot */


/*===========================================================================
  FUNCTION QMI_UIM_CHECK_PIN_DIGITS()

  DESCRIPTION
    Checks the digits of PIN value. Allowed digits are only from
    '0' to '9'. This function does not check the length of the PIN.

  PARAMETERS
    pin_value : value of the pin

  RETURN VALUE
    boolean to indicate success or failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_check_pin_digits
(
  mmgsdi_data_type  pin_value
)
{
  mmgsdi_len_type i = 0;

  if (pin_value.data_ptr == NULL)
  {
    return FALSE;
  }

  for (i = 0; i < pin_value.data_len; i++)
  {
    if (pin_value.data_ptr[i] < '0' ||
        pin_value.data_ptr[i] > '9')
    {
      UIM_MSG_ERR_1("QMI_UIM invalid PIN digit: 0x%x", pin_value.data_ptr[i]);
      return FALSE;
    }
  }

  return TRUE;
} /* qmi_uim_check_pin_digits */


/*===========================================================================
  FUNCTION QMI_UIM_GET_PROFILE_ID_FROM_INDEX()

  DESCRIPTION
    Converts index into QMI UIM Profile ID

  PARAMETERS
    profile_index     : Index of the profile

  RETURN VALUE
    profile_id

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
mmgsdi_profile_id_enum_type qmi_uim_get_profile_id_from_index
(
  uint8       profile_index
)
{
  switch (profile_index)
  {
    case 0:
      return MMGSDI_PROFILE_1;
    case 1:
      return MMGSDI_PROFILE_2;
    case 2:
      return MMGSDI_PROFILE_3;
    case 3:
      return MMGSDI_PROFILE_4;
    case 4:
      return MMGSDI_PROFILE_5;
    case 5:
      return MMGSDI_PROFILE_6;
    case 6:
      return MMGSDI_PROFILE_7;
    case 7:
      return MMGSDI_PROFILE_8;
    default:
      break;
  }

  return MMGSDI_PROFILE_NOT_APPLICABLE;
} /*  qmi_uim_get_profile_id_from_index */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_BOOLEAN()

  DESCRIPTION
    Parses a boolean TLV

  PARAMETERS
    sdu_in            : input sdu
    len               : length of the TLV item
    boolean_ptr       : value of the boolean

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_boolean
(
  dsm_item_type **                 sdu_in,
  uint16                           len,
  qmi_uim_tlv_boolean_type       * boolean_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint8  value;
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(boolean_ptr);

  /* Check length */
  if (len != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, sizeof(tlv_data));
  if (temp != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Ok to refresh */
  switch (tlv_data.value)
  {
    case 0:
      boolean_ptr->value = FALSE;
      break;
    case 1:
      boolean_ptr->value = TRUE;
      break;
    default:
      return QMI_ERR_INVALID_ARG;
  }

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_boolean */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_SESSION_INFORMATION()

  DESCRIPTION
    Parses the TLV item of session information

  PARAMETERS
    sdu_in                  : input sdu
    len                     : length of the TLV item
    session_information_ptr : output with parsed values

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_session_information
(
  dsm_item_type **                       sdu_in,
  uint16                                 len,
  qmi_uim_tlv_session_information_type * session_information_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint8  session_type;
    uint8  aid_len;
    uint8  aid_value[MMGSDI_MAX_AID_LEN];
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(session_information_ptr);

  /* Check length */
  if (len < QMI_UIM_TLV_MIN_LEN_SESSION_INFORMATION)
  {
    return QMI_ERR_MALFORMED_MSG;
  }
  if (len > sizeof(tlv_data))
  {
    return QMI_ERR_ARG_TOO_LONG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, len);
  if (temp != len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Session type */
  switch (tlv_data.session_type)
  {
    case 0:
      session_information_ptr->session_type = MMGSDI_GW_PROV_PRI_SESSION;
      break;
    case 1:
      session_information_ptr->session_type = MMGSDI_1X_PROV_PRI_SESSION;
      break;
    case 2:
      session_information_ptr->session_type = MMGSDI_GW_PROV_SEC_SESSION;
      break;
    case 3:
      session_information_ptr->session_type = MMGSDI_1X_PROV_SEC_SESSION;
      break;
    case 4:
      session_information_ptr->session_type = MMGSDI_NON_PROV_SESSION_SLOT_1;
      break;
    case 5:
      session_information_ptr->session_type = MMGSDI_NON_PROV_SESSION_SLOT_2;
      break;
    case 6:
      session_information_ptr->session_type = MMGSDI_CARD_SESSION_SLOT_1;
      break;
    case 7:
      session_information_ptr->session_type = MMGSDI_CARD_SESSION_SLOT_2;
      break;
    case 8:
      session_information_ptr->session_type = MMGSDI_NON_PROV_SESSION_SLOT_1;
      break;
    case 9:
      session_information_ptr->session_type = MMGSDI_NON_PROV_SESSION_SLOT_2;
      break;
    case 10:
      session_information_ptr->session_type = MMGSDI_GW_PROV_TER_SESSION;
      break;
    case 11:
      session_information_ptr->session_type = MMGSDI_1X_PROV_TER_SESSION;
      break;
    case 12:
    case 13:
    case 14:
    case 15:
      return QMI_ERR_NOT_SUPPORTED;
    case 16:
      session_information_ptr->session_type = MMGSDI_NON_PROV_SESSION_SLOT_3;
      break;
    case 17:
    case 18:
      return QMI_ERR_NOT_SUPPORTED;
    case 19:
      session_information_ptr->session_type = MMGSDI_CARD_SESSION_SLOT_3;
      break;
    case 20:
    case 21:
      return QMI_ERR_NOT_SUPPORTED;
    case 22:
      session_information_ptr->session_type = MMGSDI_NON_PROV_SESSION_SLOT_3;
      break;
    case 23:
    case 24:
      return QMI_ERR_NOT_SUPPORTED;
    default:
      return QMI_ERR_INVALID_ARG;
  }

  /* AID length */
  session_information_ptr->aid.data_len = (mmgsdi_len_type)tlv_data.aid_len;
  if (session_information_ptr->aid.data_len + QMI_UIM_TLV_MIN_LEN_SESSION_INFORMATION != len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }
  if (session_information_ptr->aid.data_len > MMGSDI_MAX_AID_LEN)
  {
    return QMI_ERR_ARG_TOO_LONG;
  }
  /* Validate Channel ID length, reject length other than 1 byte */
  if ((tlv_data.session_type == 8) ||
      (tlv_data.session_type == 9) ||
      (tlv_data.session_type == 22))
  {
    if (session_information_ptr->aid.data_len != QMI_UIM_TLV_CHANNEL_ID_LEN)
    {
      return QMI_ERR_INVALID_ARG;
    }
  }

  /* AID value */
  if (session_information_ptr->aid.data_len > 0)
  {
    (void)memscpy((void*)session_information_ptr->aid.data_ptr,
                  sizeof(session_information_ptr->aid.data_ptr),
           (void*)tlv_data.aid_value,
           (uint32)session_information_ptr->aid.data_len);
  }

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_session_information */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_FILE_ID()

  DESCRIPTION
    Parses the TLV item of file id

  PARAMETERS
    sdu_in      : input sdu
    len         : length of the TLV item
    file_id_ptr : output with file id value

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_file_id
(
  dsm_item_type **           sdu_in,
  uint16                     len,
  qmi_uim_tlv_file_id_type * file_id_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint16 file_id;
    uint8  path_len;
    uint8  path_value[(MMGSDI_MAX_PATH_LEN - 1) * 2];
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(file_id_ptr);

  /* Check length */
  if (len < QMI_UIM_TLV_MIN_LEN_FILE_ID)
  {
    return QMI_ERR_MALFORMED_MSG;
  }
  if (len > sizeof(tlv_data))
  {
    return QMI_ERR_ARG_TOO_LONG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, len);
  if (temp != len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Verify path length value */
  if ( tlv_data.path_len == 0 ||
       (tlv_data.path_len & 0x01) != 0 ||
       tlv_data.path_len + QMI_UIM_TLV_MIN_LEN_FILE_ID != len)
  {
    return QMI_ERR_INVALID_ARG;
  }
  if ( (tlv_data.path_len > (MMGSDI_MAX_PATH_LEN - 1) * sizeof(uint16)) )
  {
    return QMI_ERR_ARG_TOO_LONG;
  }

  /* Set access by path */
  file_id_ptr->file_access.access_method = MMGSDI_BY_PATH_ACCESS;

  /* Path length (in numbers of uint16) and value */
  file_id_ptr->file_access.file.path_type.path_len =
    ((mmgsdi_len_type)tlv_data.path_len) / sizeof(uint16);
  (void)memscpy((void*)file_id_ptr->file_access.file.path_type.path_buf,
                sizeof(file_id_ptr->file_access.file.path_type.path_buf),
          (void*)tlv_data.path_value,
          tlv_data.path_len );

  /* Add file id to the path */
  file_id_ptr->file_access.file.path_type.path_buf[file_id_ptr->file_access.file.path_type.path_len] = tlv_data.file_id;
  file_id_ptr->file_access.file.path_type.path_len += 1;

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_file_id */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_READ_TRANSPARENT()

  DESCRIPTION
    Parses the TLV item of read transparent

  PARAMETERS
    sdu_in               : input sdu
    len                  : length of the TLV item
    read_transparent_ptr : output with read tranparent value

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_read_transparent
(
  dsm_item_type **                    sdu_in,
  uint16                              len,
  qmi_uim_tlv_read_transparent_type * read_transparent_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint16 offset;
    uint16 length;
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(read_transparent_ptr);

  /* Check length */
  if (len != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, len);
  if (temp != len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Offset */
  read_transparent_ptr->offset = (mmgsdi_offset_type)tlv_data.offset;

  /* Length */
  read_transparent_ptr->length = (mmgsdi_len_type)tlv_data.length;

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_read_transparent */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_READ_RECORD()

  DESCRIPTION
    Parses the TLV item of read record

  PARAMETERS
    sdu_in          : input sdu
    len             : length of the TLV item
    read_record_ptr : output with read record value

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_read_record
(
  dsm_item_type **               sdu_in,
  uint16                         len,
  qmi_uim_tlv_read_record_type * read_record_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint16 record;
    uint16 length;
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(read_record_ptr);

  /* Check length */
  if (len != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, len);
  if (temp != len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Offset */
  read_record_ptr->record = (mmgsdi_rec_num_type)tlv_data.record;

  /* Record */
  read_record_ptr->length = (mmgsdi_len_type)tlv_data.length;

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_read_record */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_READ_RECORD_LAST()

  DESCRIPTION
    Parses the TLV item of read record last

  PARAMETERS
    sdu_in               : input sdu
    len                  : length of the TLV item
    read_record_last_ptr : output with read record last value

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_read_record_last
(
  dsm_item_type **                    sdu_in,
  uint16                              len,
  qmi_uim_tlv_read_record_last_type * read_record_last_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint16 last_record;
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(read_record_last_ptr);

  /* Check length */
  if (len != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, len);
  if (temp != len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Record */
  read_record_last_ptr->last_record = (mmgsdi_rec_num_type)tlv_data.last_record;

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_read_record_last */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_WRITE_TRANSPARENT()

  DESCRIPTION
    Parses the TLV item of write transparent

  PARAMETERS
    sdu_in                : input sdu
    len                   : length of the TLV item
    write_transparent_ptr : output with write tranparent value

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    Allocates memory that needs to be freed by the caller

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_write_transparent
(
  dsm_item_type **                     sdu_in,
  uint16                               len,
  qmi_uim_tlv_write_transparent_type * write_transparent_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint16 offset;
    uint16 length;
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(write_transparent_ptr);

  /* Check length */
  if (len < sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, sizeof(tlv_data));
  if (temp != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Offset */
  write_transparent_ptr->offset = (mmgsdi_offset_type)tlv_data.offset;

  /* Length */
  write_transparent_ptr->data.data_len = (mmgsdi_len_type)tlv_data.length;

  /* Check data length */
  if (len != sizeof(tlv_data) + tlv_data.length)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract body to write. The memory allocated here needs to be freed by
     the caller, after using the data */
  if (tlv_data.length > 0)
  {
    write_transparent_ptr->data.data_ptr =
          (uint8*)uimqmi_malloc(tlv_data.length);
    if (write_transparent_ptr->data.data_ptr == NULL)
    {
      return QMI_ERR_NO_MEMORY;
    }

    temp = dsm_pullup(sdu_in,
                      (void*)write_transparent_ptr->data.data_ptr,
                      tlv_data.length);
    if (temp != tlv_data.length)
    {
      return QMI_ERR_MALFORMED_MSG;
    }
  }

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_write_transparent */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_WRITE_RECORD()

  DESCRIPTION
    Parses the TLV item of write record

  PARAMETERS
    sdu_in           : input sdu
    len              : length of the TLV item
    write_record_ptr : output with write record value

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    Allocates memory that needs to be freed by the caller

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_write_record
(
  dsm_item_type **                sdu_in,
  uint16                          len,
  qmi_uim_tlv_write_record_type * write_record_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint16 record;
    uint16 length;
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(write_record_ptr);

  /* Check length */
  if (len < sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, sizeof(tlv_data));
  if (temp != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Record */
  write_record_ptr->record = (mmgsdi_rec_num_type)tlv_data.record;

  /* Length */
  write_record_ptr->data.data_len = (mmgsdi_len_type)tlv_data.length;

  /* Check data length */
  if (len != sizeof(tlv_data) + tlv_data.length)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract body to write. The memory allocated here needs to be freed by
     the caller, after using the data */
  if (tlv_data.length > 0)
  {
    write_record_ptr->data.data_ptr =
      (uint8*)uimqmi_malloc(tlv_data.length);
    if (write_record_ptr->data.data_ptr == NULL)
    {
      return QMI_ERR_NO_MEMORY;
    }

    temp = dsm_pullup(sdu_in,
                      (void*)write_record_ptr->data.data_ptr,
                      tlv_data.length);
    if (temp != tlv_data.length)
    {
      return QMI_ERR_MALFORMED_MSG;
    }
  }

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_write_record */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_SET_PIN_PROTECTION()

  DESCRIPTION
    Parses the TLV item of set pin protection

  PARAMETERS
    sdu_in                 : input sdu
    len                    : length of the TLV item
    set_pin_protection_ptr : output with set pin protection value

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_set_pin_protection
(
  dsm_item_type **                      sdu_in,
  uint16                                len,
  qmi_uim_tlv_set_pin_protection_type * set_pin_protection_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint8  pin_id;
    uint8  pin_operation;
    uint8  pin_len;
    uint8  pin_value[QMI_UIM_TLV_MAX_PIN_LEN];
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(set_pin_protection_ptr);

  /* Check length */
  if (len < QMI_UIM_TLV_MIN_LEN_SET_PIN_PROTECTION)
  {
    return QMI_ERR_MALFORMED_MSG;
  }
  if (len > sizeof(tlv_data))
  {
    return QMI_ERR_ARG_TOO_LONG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, len);
  if (temp != len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Pin id */
  switch(tlv_data.pin_id)
  {
    case 1:
      set_pin_protection_ptr->pin_id = MMGSDI_PIN1;
      break;
    case 2:
      set_pin_protection_ptr->pin_id = MMGSDI_PIN2;
      break;
    case 3:
      set_pin_protection_ptr->pin_id = MMGSDI_UNIVERSAL_PIN;
      break;
    case 4:
      set_pin_protection_ptr->pin_id = MMGSDI_HIDDENKEY;
      break;
    default:
      return QMI_ERR_INVALID_PINID;
  }

  /* Pin operation */
  switch (tlv_data.pin_operation)
  {
    case 0:
      set_pin_protection_ptr->pin_enable = FALSE;
      break;
    case 1:
      set_pin_protection_ptr->pin_enable = TRUE;
      break;
    default:
      return QMI_ERR_INVALID_ARG;
  }

  /* Pin length */
  set_pin_protection_ptr->pin_data.data_len = (mmgsdi_len_type)tlv_data.pin_len;

  /* Check data length */
  if (len != QMI_UIM_TLV_MIN_LEN_SET_PIN_PROTECTION +
             set_pin_protection_ptr->pin_data.data_len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }
  if (set_pin_protection_ptr->pin_data.data_len == 0 ||
      set_pin_protection_ptr->pin_data.data_len > QMI_UIM_TLV_MAX_PIN_LEN)
  {
    return QMI_ERR_ARG_TOO_LONG;
  }

  /* Pin value */
  (void)memscpy((void*)set_pin_protection_ptr->pin_value,
                sizeof(set_pin_protection_ptr->pin_value),
         (void*)tlv_data.pin_value,
         (uint32)set_pin_protection_ptr->pin_data.data_len);
  set_pin_protection_ptr->pin_data.data_ptr = set_pin_protection_ptr->pin_value;

  /* Check PIN value */
  if (!qmi_uim_check_pin_digits(set_pin_protection_ptr->pin_data))
  {
    return QMI_ERR_INCORRECT_PIN;
  }

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_set_pin_protection */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_VERIFY_PIN()

  DESCRIPTION
    Parses the TLV item of verify pin

  PARAMETERS
    sdu_in         : input sdu
    len            : length of the TLV item
    verify_pin_ptr : output with verify pin value

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_verify_pin
(
  dsm_item_type **              sdu_in,
  uint16                        len,
  qmi_uim_tlv_verify_pin_type * verify_pin_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint8  pin_id;
    uint8  pin_len;
    uint8  pin_data[QMI_UIM_TLV_MAX_PIN_LEN];
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(verify_pin_ptr);

  /* Check length */
  if (len < QMI_UIM_TLV_MIN_LEN_VERIFY_PIN)
  {
    return QMI_ERR_MALFORMED_MSG;
  }
  if (len > sizeof(tlv_data))
  {
    return QMI_ERR_ARG_TOO_LONG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, len);
  if (temp != len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Pin id */
  switch(tlv_data.pin_id)
  {
    case 1:
      verify_pin_ptr->pin_id = MMGSDI_PIN1;
      break;
    case 2:
      verify_pin_ptr->pin_id = MMGSDI_PIN2;
      break;
    case 3:
      verify_pin_ptr->pin_id = MMGSDI_UNIVERSAL_PIN;
      break;
    case 4:
      verify_pin_ptr->pin_id = MMGSDI_HIDDENKEY;
      break;
    default:
      return QMI_ERR_INVALID_PINID;
  }

  /* Pin length */
  verify_pin_ptr->pin_data.data_len = (mmgsdi_len_type)tlv_data.pin_len;

  /* Check data length */
  if (len != QMI_UIM_TLV_MIN_LEN_VERIFY_PIN +
             verify_pin_ptr->pin_data.data_len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Accept PIN length as 0 only in case of PIN1 because the request
     might contain also an encrypted PIN value */
  if (verify_pin_ptr->pin_data.data_len == 0 &&
      verify_pin_ptr->pin_id != MMGSDI_PIN1)
  {
    return QMI_ERR_INVALID_ARG;
  }

  if (verify_pin_ptr->pin_data.data_len > QMI_UIM_TLV_MAX_PIN_LEN)
  {
    return QMI_ERR_ARG_TOO_LONG;
  }

  /* Pin value */
  (void)memscpy((void*)verify_pin_ptr->pin_value,
                sizeof(verify_pin_ptr->pin_value),
         (void*)tlv_data.pin_data,
         (uint32)verify_pin_ptr->pin_data.data_len);
  verify_pin_ptr->pin_data.data_ptr = verify_pin_ptr->pin_value;

  /* Check PIN value */
  if (!qmi_uim_check_pin_digits(verify_pin_ptr->pin_data))
  {
    return QMI_ERR_INCORRECT_PIN;
  }

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_verify_pin */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_UNBLOCK_PIN()

  DESCRIPTION
    Parses the TLV item of unblock pin

  PARAMETERS
    sdu_in          : input sdu
    len             : length of the TLV item
    unblock_pin_ptr : output with unblock pin value

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_unblock_pin
(
  dsm_item_type **               sdu_in,
  uint16                         len,
  qmi_uim_tlv_unblock_pin_type * unblock_pin_ptr
)
{
  int32             temp;
  uint8             temp_uint8;

  /* The payload has two variable items and one of them is located in the middle
     so we cannot easily use a packed structure and use a simple buffer */
  uint8  tlv_data[QMI_UIM_TLV_MIN_LEN_UNBLOCK_PIN + (2 * QMI_UIM_TLV_MAX_PIN_LEN)];
  uint8* current = tlv_data;

  ASSERT(sdu_in);
  ASSERT(unblock_pin_ptr);

  /* Check length */
  if (len < QMI_UIM_TLV_MIN_LEN_UNBLOCK_PIN)
  {
    return QMI_ERR_MALFORMED_MSG;
  }
  if (len > sizeof(tlv_data))
  {
    return QMI_ERR_ARG_TOO_LONG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)tlv_data, len);
  if (temp != len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Pin id */
  (void)memscpy((void*)&temp_uint8, sizeof(uint8), current, sizeof(uint8));
  current += sizeof(uint8);
  switch(temp_uint8)
  {
    case 1:
      unblock_pin_ptr->pin_id = MMGSDI_PIN1;
      break;
    case 2:
      unblock_pin_ptr->pin_id = MMGSDI_PIN2;
      break;
    case 3:
      unblock_pin_ptr->pin_id = MMGSDI_UNIVERSAL_PIN;
      break;
    default:
      return QMI_ERR_INVALID_PINID;
  }

  /* Puk length */
  (void)memscpy((void*)&temp_uint8, sizeof(uint8), current, sizeof(uint8));
  current += sizeof(uint8);
  unblock_pin_ptr->puk_data.data_len = (mmgsdi_len_type)temp_uint8;

  /* Check data length */
  if (len < QMI_UIM_TLV_MIN_LEN_UNBLOCK_PIN +
            unblock_pin_ptr->puk_data.data_len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }
  if (unblock_pin_ptr->puk_data.data_len == 0 ||
      unblock_pin_ptr->puk_data.data_len > QMI_UIM_TLV_MAX_PIN_LEN)
  {
    return QMI_ERR_ARG_TOO_LONG;
  }

  /* Puk value */
  (void)memscpy((void*)unblock_pin_ptr->puk_value,
                sizeof(unblock_pin_ptr->puk_value),
         (void*)current,
         (uint32)unblock_pin_ptr->puk_data.data_len);
  current += unblock_pin_ptr->puk_data.data_len;
  unblock_pin_ptr->puk_data.data_ptr = unblock_pin_ptr->puk_value;

  /* Check PUK value */
  if (!qmi_uim_check_pin_digits(unblock_pin_ptr->puk_data))
  {
    return QMI_ERR_INCORRECT_PIN;
  }

  /* Pin length */
  (void)memscpy((void*)&temp_uint8, sizeof(uint8), current, sizeof(uint8));
  current += sizeof(uint8);
  unblock_pin_ptr->pin_data.data_len = (mmgsdi_len_type)temp_uint8;

  /* Check pin length */
  if (len != QMI_UIM_TLV_MIN_LEN_UNBLOCK_PIN +
             unblock_pin_ptr->puk_data.data_len +
             unblock_pin_ptr->pin_data.data_len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }
  if (unblock_pin_ptr->pin_data.data_len == 0 ||
      unblock_pin_ptr->pin_data.data_len > QMI_UIM_TLV_MAX_PIN_LEN)
  {
    return QMI_ERR_ARG_TOO_LONG;
  }

  /* Pin value */
  (void)memscpy((void*)unblock_pin_ptr->pin_value,
                sizeof(unblock_pin_ptr->pin_value),
         (void*)current,
         (uint32)unblock_pin_ptr->pin_data.data_len);
  current += unblock_pin_ptr->pin_data.data_len;
  unblock_pin_ptr->pin_data.data_ptr = unblock_pin_ptr->pin_value;

  /* Check PIN value */
  if (!qmi_uim_check_pin_digits(unblock_pin_ptr->pin_data))
  {
    return QMI_ERR_INCORRECT_PIN;
  }

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_unblock_pin */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_CHANGE_PIN()

  DESCRIPTION
    Parses the TLV item of change pin

  PARAMETERS
    sdu_in         : input sdu
    len            : length of the TLV item
    change_pin_ptr : output with change pin value

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_change_pin
(
  dsm_item_type **              sdu_in,
  uint16                        len,
  qmi_uim_tlv_change_pin_type * change_pin_ptr
)
{
  int32             temp;
  uint8             temp_uint8;

  /* The payload has two variable items and one of them is located in the middle
     so we cannot easily use a packed structure and use a simple buffer */
  uint8  tlv_data[QMI_UIM_TLV_MIN_LEN_CHANGE_PIN + (2 * QMI_UIM_TLV_MAX_PIN_LEN)];
  uint8* current = tlv_data;

  ASSERT(sdu_in);
  ASSERT(change_pin_ptr);

  /* Check length */
  if (len < QMI_UIM_TLV_MIN_LEN_CHANGE_PIN)
  {
    return QMI_ERR_MALFORMED_MSG;
  }
  if (len > sizeof(tlv_data))
  {
    return QMI_ERR_ARG_TOO_LONG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)tlv_data, len);
  if (temp != len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Pin id */
  (void)memscpy((void*)&temp_uint8, sizeof(uint8), current, sizeof(uint8));
  current += sizeof(uint8);
  switch(temp_uint8)
  {
    case 1:
      change_pin_ptr->pin_id = MMGSDI_PIN1;
      break;
    case 2:
      change_pin_ptr->pin_id = MMGSDI_PIN2;
      break;
    case 3:
      change_pin_ptr->pin_id = MMGSDI_UNIVERSAL_PIN;
      break;
    case 4:
      change_pin_ptr->pin_id = MMGSDI_HIDDENKEY;
      break;
    default:
      return QMI_ERR_INVALID_PINID;
  }

  /* Old pin length */
  (void)memscpy((void*)&temp_uint8, sizeof(uint8), current, sizeof(uint8));
  current += sizeof(uint8);
  change_pin_ptr->old_pin_data.data_len = (mmgsdi_len_type)temp_uint8;

  /* Check data length */
  if (len < QMI_UIM_TLV_MIN_LEN_CHANGE_PIN +
            change_pin_ptr->old_pin_data.data_len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }
  if (change_pin_ptr->old_pin_data.data_len == 0 ||
      change_pin_ptr->old_pin_data.data_len > QMI_UIM_TLV_MAX_PIN_LEN)
  {
    return QMI_ERR_ARG_TOO_LONG;
  }

  /* Old pin value */
  (void)memscpy((void*)change_pin_ptr->old_pin_value,
                sizeof(change_pin_ptr->old_pin_value),
         (void*)current,
         (uint32)change_pin_ptr->old_pin_data.data_len);
  current += change_pin_ptr->old_pin_data.data_len;
  change_pin_ptr->old_pin_data.data_ptr = change_pin_ptr->old_pin_value;

  /* Check old PIN value */
  if (!qmi_uim_check_pin_digits(change_pin_ptr->old_pin_data))
  {
    return QMI_ERR_INCORRECT_PIN;
  }

  /* New pin length */
  (void)memscpy((void*)&temp_uint8, sizeof(uint8), current, sizeof(uint8));
  current += sizeof(uint8);
  change_pin_ptr->new_pin_data.data_len = (mmgsdi_len_type)temp_uint8;

  /* Check new pin length */
  if (len != QMI_UIM_TLV_MIN_LEN_CHANGE_PIN +
             change_pin_ptr->old_pin_data.data_len +
             change_pin_ptr->new_pin_data.data_len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }
  if (change_pin_ptr->new_pin_data.data_len == 0 ||
      change_pin_ptr->new_pin_data.data_len > QMI_UIM_TLV_MAX_PIN_LEN)
  {
    return QMI_ERR_ARG_TOO_LONG;
  }

  /* New pin value */
  (void)memscpy((void*)change_pin_ptr->new_pin_value,
                sizeof(change_pin_ptr->new_pin_value),
         (void*)current,
         (uint32)change_pin_ptr->new_pin_data.data_len);
  current += change_pin_ptr->new_pin_data.data_len;
  change_pin_ptr->new_pin_data.data_ptr = change_pin_ptr->new_pin_value;

  /* Check new PIN value */
  if (!qmi_uim_check_pin_digits(change_pin_ptr->new_pin_data))
  {
    return QMI_ERR_INCORRECT_PIN;
  }

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_change_pin */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_DEPERSONALIZATION()

  DESCRIPTION
    Parses the TLV item of deactivate perso

  PARAMETERS
    sdu_in                : input sdu
    len                   : length of the TLV item
    depersonalization_ptr : output with deactivate perso value

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_depersonalization
(
  dsm_item_type **                     sdu_in,
  uint16                               len,
  qmi_uim_tlv_depersonalization_type * depersonalization_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint8  feature;
    uint8  operation;
    uint8  ck_len;
    uint8  ck_value[QMI_UIM_TLV_MAX_CK_LEN];
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(depersonalization_ptr);

  /* Check length */
  if (len < QMI_UIM_TLV_MIN_LEN_DEPERSONALIZATION)
  {
    return QMI_ERR_MALFORMED_MSG;
  }
  if (len > sizeof(tlv_data))
  {
    return QMI_ERR_ARG_TOO_LONG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, len);
  if (temp != len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Feature */
  switch(tlv_data.feature)
  {
    case 0:
      depersonalization_ptr->category = SIMLOCK_CATEGORY_3GPP_NW;
      break;
    case 1:
      depersonalization_ptr->category = SIMLOCK_CATEGORY_3GPP_NS;
      break;
    case 2:
      depersonalization_ptr->category = SIMLOCK_CATEGORY_3GPP_SP;
      break;
    case 3:
      depersonalization_ptr->category = SIMLOCK_CATEGORY_3GPP_CP;
      break;
    case 4:
      depersonalization_ptr->category = SIMLOCK_CATEGORY_3GPP_SIM;
      break;
    case 5:
      depersonalization_ptr->category = SIMLOCK_CATEGORY_3GPP2_NW_TYPE1;
      break;
    case 6:
      depersonalization_ptr->category = SIMLOCK_CATEGORY_3GPP2_NW_TYPE2;
      break;
    case 7:
      depersonalization_ptr->category = SIMLOCK_CATEGORY_3GPP2_HRPD;
      break;
    case 8:
      depersonalization_ptr->category = SIMLOCK_CATEGORY_3GPP2_CP;
      break;
    case 9:
      depersonalization_ptr->category = SIMLOCK_CATEGORY_3GPP2_CP;
      break;
    case 10:
      depersonalization_ptr->category = SIMLOCK_CATEGORY_3GPP2_RUIM;
      break;
    case 11:
      depersonalization_ptr->category = SIMLOCK_CATEGORY_3GPP_SPN;
      break;
    case UIM_PERSO_FEATURE_GW_ICCID_V01:
      depersonalization_ptr->category = SIMLOCK_CATEGORY_3GPP_ICCID;
      break;
    default:
      return QMI_ERR_INVALID_ARG;
  }

  /* Operation */
  switch (tlv_data.operation)
  {
    case 0:
      /* Indicates the operation is to unlock the SIM */
      break;
    case 1:
      return QMI_ERR_NOT_SUPPORTED;
    default:
      return QMI_ERR_INVALID_ARG;
  }

  if (tlv_data.ck_len == 0 ||
      tlv_data.ck_len > QMI_UIM_TLV_MAX_CK_LEN)
  {
    return QMI_ERR_INVALID_ARG;
  }

  /* Check data length */
  if (len != QMI_UIM_TLV_MIN_LEN_DEPERSONALIZATION +
             tlv_data.ck_len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Control key length */
  depersonalization_ptr->ck.simlock_ck_length =
    tlv_data.ck_len;

  /* Control key value */
  (void)memscpy((void*)depersonalization_ptr->ck.simlock_ck_data,
                sizeof(depersonalization_ptr->ck.simlock_ck_data),
                (void*)tlv_data.ck_value,
                (uint32)depersonalization_ptr->ck.simlock_ck_length);

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_depersonalization */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_SLOT()

  DESCRIPTION
    Parses the TLV item of slot

  PARAMETERS
    sdu_in   : input sdu
    len      : length of the TLV item
    slot_ptr : output with slot value

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_slot
(
  dsm_item_type **        sdu_in,
  uint16                  len,
  qmi_uim_tlv_slot_type * slot_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint8  slot;
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(slot_ptr);

  /* Check length */
  if (len != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, len);
  if (temp != len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Slot */
  switch(tlv_data.slot)
  {
    case 1:
      slot_ptr->slot = MMGSDI_SLOT_1;
      break;
    case 2:
      slot_ptr->slot = MMGSDI_SLOT_2;
      break;
    case 3:
      slot_ptr->slot = MMGSDI_SLOT_3;
      break;
    case 4:
    case 5:
      return QMI_ERR_NOT_SUPPORTED;
    default:
      return QMI_ERR_INVALID_ARG;
  }

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_slot */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_PHYSICAL_SLOT()

  DESCRIPTION
    Parses the TLV item of physical slot

  PARAMETERS
    sdu_in   : input sdu
    len      : length of the TLV item
    slot_ptr : output with physical slot value

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_physical_slot
(
  dsm_item_type **                 sdu_in,
  uint16                           len,
  qmi_uim_tlv_physical_slot_type * slot_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint32 physical_slot;
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(slot_ptr);

  /* Check length */
  if (len != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, len);
  if (temp != len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Physical Slot */
  switch(tlv_data.physical_slot)
  {
    case 1:
      slot_ptr->slot = UIM_SLOT_1;
      break;
    case 2:
      slot_ptr->slot = UIM_SLOT_2;
      break;
    case 3:
    case 4:
    case 5:
      return QMI_ERR_NOT_SUPPORTED;
    default:
      return QMI_ERR_INVALID_ARG;
  }

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_physical_slot */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_EVENT_REG()

  DESCRIPTION
    Parses the TLV item of event reg

  PARAMETERS
    sdu_in        : input sdu
    len           : length of the TLV item
    event_reg_ptr : output with event reg value

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_event_reg
(
  dsm_item_type **             sdu_in,
  uint16                       len,
  qmi_uim_tlv_event_reg_type * event_reg_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint32  bitmask;
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(event_reg_ptr);

  /* Check length */
  if (len != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, len);
  if (temp != len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Set all flags to false by default */
  memset(event_reg_ptr, 0, sizeof(qmi_uim_tlv_event_reg_type));

  /* Legacy card status */
  if ((tlv_data.bitmask & 0x00000001) != 0)
  {
    event_reg_ptr->card_status = TRUE;
  }

  /* Modem events */
  if ((tlv_data.bitmask & 0x00000002) != 0)
  {
    event_reg_ptr->sap_events = TRUE;
  }

  /* Extended card status */
  if ((tlv_data.bitmask & 0x00000004) != 0)
  {
    event_reg_ptr->extended_card_status = TRUE;
  }

  /* Provisioning session close indication */
  if((tlv_data.bitmask & 0x00000008) != 0)
  {
    event_reg_ptr->provisioning_session_close_event = TRUE;
  }

  /* Physical slots status */
  if ((tlv_data.bitmask & 0x00000010) != 0)
  {
    event_reg_ptr->physical_slots_status = TRUE;
  }

  /* Sim Busy events */
  if((tlv_data.bitmask & 0x00000020) != 0)
  {
    event_reg_ptr->sim_busy_events = TRUE;
  }

  /* Reduced card status */
  if((tlv_data.bitmask & 0x00000040) != 0)
  {
    event_reg_ptr->reduced_card_status_events = TRUE;
  }

  /* Recovery Complete indication */
  if((tlv_data.bitmask & 0x00000080) != 0)
  {
    event_reg_ptr->recovery_complete_event = TRUE;
  }

  /* Supply voltage events */
  if((tlv_data.bitmask & 0x00000100) != 0)
  {
    event_reg_ptr->supply_voltage_events = TRUE;
  }

  /* Card activation status indication */
  if((tlv_data.bitmask & 0x00000200) != 0)
  {
    event_reg_ptr->activation_status_event = TRUE;
  }

  /* Simlock remote SFS events */
  if((tlv_data.bitmask & 0x00000400) != 0)
  {
    event_reg_ptr->simlock_remote_sfs_events = TRUE;
  }

  /* Simlock remote SFS events */
  if((tlv_data.bitmask & 0x00000800) != 0)
  {
    event_reg_ptr->simlock_temp_unlock_status = TRUE;
  }

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_event_reg */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_AUTHENTICATE()

  DESCRIPTION
    Parses the TLV item of event reg

  PARAMETERS
    sdu_in        : input sdu
    len           : length of the TLV item
    event_reg_ptr : output with event reg value

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_authenticate
(
  dsm_item_type **                sdu_in,
  uint16                          len,
  qmi_uim_tlv_authenticate_type * authenticate_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint8  context;
    uint16 length;
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(authenticate_ptr);

  /* Check length */
  if (len < sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, sizeof(tlv_data));
  if (temp != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Context */
  switch(tlv_data.context)
  {
    case 0:
      authenticate_ptr->auth_context =
        QMI_UIM_AUTH_CONTEXT_RUN_GSM_ALGO;
      break;
    case 1:
      authenticate_ptr->auth_context =
        QMI_UIM_AUTH_CONTEXT_RUN_CAVE_ALGO;
      break;
    case 2:
      authenticate_ptr->auth_context =
        QMI_UIM_AUTH_CONTEXT_GSM_SECURITY;
      break;
    case 3:
      authenticate_ptr->auth_context =
        QMI_UIM_AUTH_CONTEXT_3G_SECURITY;
      break;
    case 4:
      authenticate_ptr->auth_context =
        QMI_UIM_AUTH_CONTEXT_VGCS_VBS_SECURITY;
      break;
    case 5:
      authenticate_ptr->auth_context =
        QMI_UIM_AUTH_CONTEXT_GBA_BOOTSTRAPPING_SECURITY;
      break;
    case 6:
      authenticate_ptr->auth_context =
        QMI_UIM_AUTH_CONTEXT_GBA_NAF_DERIVATION_SECURITY;
      break;
    case 7:
      authenticate_ptr->auth_context =
        QMI_UIM_AUTH_CONTEXT_MBMS_MSK_UPDATE;
      break;
    case 8:
      authenticate_ptr->auth_context =
        QMI_UIM_AUTH_CONTEXT_MBMS_MTK_GEN;
      break;
    case 9:
      authenticate_ptr->auth_context =
        QMI_UIM_AUTH_CONTEXT_MBMS_MSK_DEL;
      break;
    case 10:
      authenticate_ptr->auth_context =
        QMI_UIM_AUTH_CONTEXT_MBMS_MUK_DEL;
      break;
    case 11:
      authenticate_ptr->auth_context =
        QMI_UIM_AUTH_CONTEXT_IMS_AKA_SECURITY;
      break;
    case 12:
      authenticate_ptr->auth_context =
        QMI_UIM_AUTH_CONTEXT_HTTP_DIGEST_SECURITY;
      break;
    case 13:
      authenticate_ptr->auth_context =
        QMI_UIM_AUTH_CONTEXT_COMPUTE_IP_CHAP;
      break;
    case 14:
      authenticate_ptr->auth_context =
        QMI_UIM_AUTH_CONTEXT_COMPUTE_IP_MN_HA;
      break;
    case 15:
      authenticate_ptr->auth_context =
        QMI_UIM_AUTH_CONTEXT_COMPUTE_IP_MIP_RRQ;
      break;
    case 16:
      authenticate_ptr->auth_context =
        QMI_UIM_AUTH_CONTEXT_COMPUTE_IP_MN_AAA;
      break;
    case 17:
      authenticate_ptr->auth_context =
        QMI_UIM_AUTH_CONTEXT_COMPUTE_IP_HRPD_ACCESS;
      break;
    default:
      return QMI_ERR_INVALID_ARG;
  }

  /* Length */
  authenticate_ptr->auth_data.data_len = (mmgsdi_len_type)tlv_data.length;

  /* Check data length */
  if (len != sizeof(tlv_data) + tlv_data.length)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract body for authenticate. The memory allocated here needs to be freed by
     the caller, after using the data */
  if (tlv_data.length > 0)
  {
    authenticate_ptr->auth_data.data_ptr =
      (uint8*)uimqmi_malloc(tlv_data.length);
    if (authenticate_ptr->auth_data.data_ptr == NULL)
    {
      return QMI_ERR_NO_MEMORY;
    }

    temp = dsm_pullup(sdu_in,
                      (void*)authenticate_ptr->auth_data.data_ptr,
                      tlv_data.length);
    if (temp != tlv_data.length)
    {
      return QMI_ERR_MALFORMED_MSG;
    }
  }

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_authenticate */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_CAPABILITIES_MASK()

  DESCRIPTION
    Parses the TLV item of capabilities mask

  PARAMETERS
    sdu_in        : input sdu
    len           : length of the TLV item
    event_reg_ptr : output with capabilities mask value

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_capabilities_mask
(
  dsm_item_type **                     sdu_in,
  uint16                               len,
  qmi_uim_tlv_capabilities_mask_type * capabilities_mask_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint32  bitmask;
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(capabilities_mask_ptr);

  /* Check length */
  if (len != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, len);
  if (temp != len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Set all flags to false by default */
  memset(capabilities_mask_ptr, 0, sizeof(qmi_uim_tlv_capabilities_mask_type));

  /* Card status */
  if ((tlv_data.bitmask & 0x00000001) != 0)
  {
    capabilities_mask_ptr->fdn_status = TRUE;
  }
  if ((tlv_data.bitmask & 0x00000002) != 0)
  {
    capabilities_mask_ptr->hidden_key_status = TRUE;
  }
  if ((tlv_data.bitmask & 0x00000004) != 0)
  {
    capabilities_mask_ptr->index_in_dir = TRUE;
  }
  if ((tlv_data.bitmask & 0x00000008) != 0)
  {
    capabilities_mask_ptr->esn_me_status = TRUE;
  }
  if ((tlv_data.bitmask & 0x00000010) != 0)
  {
    capabilities_mask_ptr->acl_status = TRUE;
  }
  if ((tlv_data.bitmask & 0x00000020) != 0)
  {
    capabilities_mask_ptr->service_status = TRUE;
  }

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_capabilities_mask */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_APPLICATION_INFORMATION()

  DESCRIPTION
    Parses the TLV item of application information

  PARAMETERS
    sdu_in        : input sdu
    len           : length of the TLV item
    app_info_ptr  : output with application information value

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_application_information
(
  dsm_item_type **                   sdu_in,
  uint16                             len,
  qmi_uim_tlv_app_information_type * app_info_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint8  slot;
    uint8  aid_len;
    uint8  aid_value[MMGSDI_MAX_AID_LEN];
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(app_info_ptr);

  /* Check length */
  if (len < QMI_UIM_TLV_MIN_LEN_APPLICATION_INFORMATION)
  {
    return QMI_ERR_MALFORMED_MSG;
  }
  if (len > sizeof(tlv_data))
  {
    return QMI_ERR_ARG_TOO_LONG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, len);
  if (temp != len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Slot id */
  switch (tlv_data.slot)
  {
    case 1:
      app_info_ptr->slot = MMGSDI_SLOT_1;
      break;
    case 2:
      app_info_ptr->slot = MMGSDI_SLOT_2;
      break;
    case 3:
      app_info_ptr->slot = MMGSDI_SLOT_3;
      break;
    case 4:
    case 5:
      return QMI_ERR_NOT_SUPPORTED;
    default:
      return QMI_ERR_INVALID_ARG;
  }

  /* AID length */
  app_info_ptr->aid.data_len = (mmgsdi_len_type)tlv_data.aid_len;
  if (app_info_ptr->aid.data_len + QMI_UIM_TLV_MIN_LEN_APPLICATION_INFORMATION != len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }
  if (app_info_ptr->aid.data_len > MMGSDI_MAX_AID_LEN)
  {
    return QMI_ERR_ARG_TOO_LONG;
  }

  /* AID value */
  if (app_info_ptr->aid.data_len > 0)
  {
    (void)memscpy((void*)app_info_ptr->aid.data_ptr,
                  sizeof(app_info_ptr->aid.data_ptr),
           (void*)tlv_data.aid_value,
           (uint32)app_info_ptr->aid.data_len);
  }

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_application_information */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_SESSION_CHANGE()

  DESCRIPTION
    Parses the TLV item of session change

  PARAMETERS
    sdu_in             : input sdu
    len                : length of the TLV item
    session_change_ptr : output with session change value

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_session_change
(
  dsm_item_type **                   sdu_in,
  uint16                             len,
  qmi_uim_tlv_session_change_type  * session_change_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint8  session_type;
    uint8  activate;
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(session_change_ptr);

  /* Check length */
  if (len != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, len);
  if (temp != len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Session type */
  switch (tlv_data.session_type)
  {
    case 0:
      session_change_ptr->session_type = MMGSDI_GW_PROV_PRI_SESSION;
      break;
    case 1:
      session_change_ptr->session_type = MMGSDI_1X_PROV_PRI_SESSION;
      break;
    case 2:
      session_change_ptr->session_type = MMGSDI_GW_PROV_SEC_SESSION;
      break;
    case 3:
      session_change_ptr->session_type = MMGSDI_1X_PROV_SEC_SESSION;
      break;
    case 10:
      session_change_ptr->session_type = MMGSDI_GW_PROV_TER_SESSION;
      break;
    case 11:
      session_change_ptr->session_type = MMGSDI_1X_PROV_TER_SESSION;
      break;
    case 12:
    case 13:
    case 14:
    case 15:
      return QMI_ERR_NOT_SUPPORTED;
    default:
      /* Only provisioning sessions are valid for session change */
      return QMI_ERR_INVALID_ARG;
  }

  /* Activate */
  switch(tlv_data.activate)
  {
    case 0:
      session_change_ptr->activate = FALSE;
      break;
    case 1:
      session_change_ptr->activate = TRUE;
      break;
    default:
      /* Only provisioning sessions are valid for session change */
      return QMI_ERR_INVALID_ARG;
  }

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_session_change */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_APDU_REQUEST()

  DESCRIPTION
    Parses the TLV item of APDU request

  PARAMETERS
    sdu_in        : input sdu
    len           : length of the TLV item
    app_info_ptr  : output with APDU request value

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_apdu_request
(
  dsm_item_type **                   sdu_in,
  uint16                             len,
  qmi_uim_tlv_apdu_type            * apdu_info_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint16  apdu_len;
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(apdu_info_ptr);

  /* Check length */
  if (len < sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, sizeof(tlv_data));
  if (temp != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* APDU data length */
  apdu_info_ptr->apdu_data.data_len = (mmgsdi_len_type)tlv_data.apdu_len;

  /* Check data length */
  if (len != sizeof(tlv_data) + tlv_data.apdu_len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract APDU data to write. The memory allocated here needs to be freed by
     the caller, after using the data */
  if (tlv_data.apdu_len > 0)
  {
    apdu_info_ptr->apdu_data.data_ptr =
      (uint8*)uimqmi_malloc(tlv_data.apdu_len);

    if (apdu_info_ptr->apdu_data.data_ptr == NULL)
    {
      return QMI_ERR_NO_MEMORY;
    }

    temp = dsm_pullup(sdu_in,
                      (void*)apdu_info_ptr->apdu_data.data_ptr,
                      tlv_data.apdu_len);
    if (temp != tlv_data.apdu_len)
    {
      return QMI_ERR_MALFORMED_MSG;
    }
  }

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_apdu_request */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_SAP_CONNECT()

  DESCRIPTION
    Parses the TLV item of SAP connect

  PARAMETERS
    sdu_in                   : input sdu
    len                      : length of the TLV item
    sap_connection_info_ptr  : output with SAP connection request

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_sap_connect
(
  dsm_item_type **                   sdu_in,
  uint16                             len,
  qmi_uim_tlv_sap_connect_type     * sap_connection_info_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint8   connect;
    uint8   slot;
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(sap_connection_info_ptr);

  /* Check length */
  if (len != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, sizeof(tlv_data));
  if (temp != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Connection operation */
  switch (tlv_data.connect)
  {
    case 0:
      sap_connection_info_ptr->connect = QMI_UIM_SAP_CONNECTION_OP_DISCONNECT;
      break;
    case 1:
      sap_connection_info_ptr->connect = QMI_UIM_SAP_CONNECTION_OP_CONNECT;
      break;
    case 2:
      sap_connection_info_ptr->connect = QMI_UIM_SAP_CONNECTION_OP_CHECK_STATUS;
      break;
    default:
      return QMI_ERR_INVALID_ARG;
  }

  /* Slot id */
  switch (tlv_data.slot)
  {
    case 1:
      sap_connection_info_ptr->slot = MMGSDI_SLOT_1;
      break;
    case 2:
      sap_connection_info_ptr->slot = MMGSDI_SLOT_2;
      break;
    case 3:
      sap_connection_info_ptr->slot = MMGSDI_SLOT_3;
      break;
    case 4:
    case 5:
      return QMI_ERR_NOT_SUPPORTED;
    default:
      return QMI_ERR_INVALID_ARG;
  }

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_sap_connect */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_SAP_DISCONNECT_MODE()

  DESCRIPTION
    Parses the TLV item of SAP disconnect mode

  PARAMETERS
    sdu_in                      : input sdu
    len                         : length of the TLV item
    sap_disconnection_info_ptr  : output with SAP disconnection mode

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_sap_disconnect_mode
(
  dsm_item_type **                        sdu_in,
  uint16                                  len,
  qmi_uim_tlv_sap_disconnect_mode_type  * sap_disconnection_info_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint8   mode;
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(sap_disconnection_info_ptr);

  /* Check length */
  if (len != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, sizeof(tlv_data));
  if (temp != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Connection operation */
  switch (tlv_data.mode)
  {
    case 0:
      sap_disconnection_info_ptr->mode = MMGSDIBT_DISCONNECT_IMMED;
      break;
    case 1:
      sap_disconnection_info_ptr->mode = MMGSDIBT_DISCONNECT_GRACE;
      break;
    default:
      return QMI_ERR_INVALID_ARG;
  }

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_sap_disconnect_mode */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_SAP_REQUEST()

  DESCRIPTION
    Parses the TLV item of SAP request

  PARAMETERS
    sdu_in                      : input sdu
    len                         : length of the TLV item
    sap_request_info_ptr        : output with SAP request info

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_sap_request
(
  dsm_item_type **                        sdu_in,
  uint16                                  len,
  qmi_uim_tlv_sap_request_type          * sap_request_info_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint8   request;
    uint8   slot;
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(sap_request_info_ptr);

  /* Check length */
  if (len != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, sizeof(tlv_data));
  if (temp != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Connection operation */
  switch (tlv_data.request)
  {
    case 0:
      sap_request_info_ptr->request = QMI_UIM_SAP_REQUEST_GET_ATR;
      break;
    case 1:
      sap_request_info_ptr->request = QMI_UIM_SAP_REQUEST_SEND_APDU;
      break;
    case 2:
      sap_request_info_ptr->request = QMI_UIM_SAP_REQUEST_SIM_POWER_OFF;
      break;
    case 3:
      sap_request_info_ptr->request = QMI_UIM_SAP_REQUEST_SIM_POWER_ON;
      break;
    case 4:
      sap_request_info_ptr->request = QMI_UIM_SAP_REQUEST_SIM_RESET;
      break;
    case 5:
      sap_request_info_ptr->request = QMI_UIM_SAP_REQUEST_CARD_READER_STATUS;
      break;
    default:
      return QMI_ERR_INVALID_ARG;
  }

  /* Slot id */
  switch (tlv_data.slot)
  {
    case 1:
      sap_request_info_ptr->slot = MMGSDI_SLOT_1;
      break;
    case 2:
      sap_request_info_ptr->slot = MMGSDI_SLOT_2;
      break;
    case 3:
      sap_request_info_ptr->slot = MMGSDI_SLOT_3;
      break;
    case 4:
    case 5:
      return QMI_ERR_NOT_SUPPORTED;
    default:
      return QMI_ERR_INVALID_ARG;
  }

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_sap_request */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_CHANNEL_ID()

  DESCRIPTION
    Parses the TLV item of channel id

  PARAMETERS
    sdu_in               : input sdu
    len                  : length of the TLV item
    apdu_channel_id_ptr  : output with APDU channel id value

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_channel_id
(
  dsm_item_type **                   sdu_in,
  uint16                             len,
  qmi_uim_tlv_logical_channel_type * logical_channel_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint8  logical_channel;
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(logical_channel_ptr);

  /* Check length */
  if (len != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, len);
  if (temp != len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  logical_channel_ptr->logical_channel = tlv_data.logical_channel;

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_channel_id */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_AID()

  DESCRIPTION
    Parses the TLV item of channel id for a close request

  PARAMETERS
    sdu_in               : input sdu
    len                  : length of the TLV item
    aid_ptr              : output with AID value

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_aid
(
  dsm_item_type **       sdu_in,
  uint16                 len,
  qmi_uim_tlv_aid_type * aid_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint8  aid_len;
    uint8  aid_value[MMGSDI_MAX_APP_ID_LEN];
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(aid_ptr);

  /* Check length */
  if (len < QMI_UIM_TLV_MIN_LEN_AID)
  {
    return QMI_ERR_MALFORMED_MSG;
  }
  if (len > sizeof(tlv_data))
  {
    return QMI_ERR_ARG_TOO_LONG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, len);
  if (temp != len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* AID length */
  aid_ptr->aid.data_len = (mmgsdi_len_type)tlv_data.aid_len;
  if (aid_ptr->aid.data_len + QMI_UIM_TLV_MIN_LEN_AID != len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }
  if (aid_ptr->aid.data_len > MMGSDI_MAX_APP_ID_LEN)
  {
    return QMI_ERR_ARG_TOO_LONG;
  }

  /* AID value */
  if (aid_ptr->aid.data_len > 0)
  {
    (void)memscpy((void*)aid_ptr->aid.data_ptr,
                  sizeof(aid_ptr->aid.data_ptr),
                  (void*)tlv_data.aid_value,
                  (uint32)aid_ptr->aid.data_len);
  }

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_aid */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_DECRYPTED_PIN()

  DESCRIPTION
    Parses the TLV item of encrypted PIN and decodes it.

  PARAMETERS
    sdu_in                      : input sdu
    len                         : length of the TLV item
    decrypted_pin_ptr           : output with encrypted PIN

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_decrypted_pin
(
  dsm_item_type **                        sdu_in,
  uint16                                  len,
  qmi_uim_tlv_decrypted_pin_type        * decrypted_pin_ptr
)
{
  int32             temp          = 0;
  uint8             decrypted_len = 0;
  uint8           * pin_value_ptr = NULL;
  qmi_error_e_type  qmi_err       = QMI_ERR_NONE;

  PACKED struct PACKED_POST
  {
    uint8  pin_len;
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(decrypted_pin_ptr);

  /* Check length */
  if (len < QMI_UIM_TLV_MIN_LEN_ENCRYPTED_PIN)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract length */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, sizeof(tlv_data));
  if (temp != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Check data length */
  if (len != sizeof(tlv_data) + tlv_data.pin_len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  if (tlv_data.pin_len == 0)
  {
    return QMI_ERR_ARG_TOO_LONG;
  }

  /* Extract PIN data */
  pin_value_ptr = (uint8*)uimqmi_malloc(tlv_data.pin_len);
  if (pin_value_ptr == NULL)
  {
    return QMI_ERR_NO_MEMORY;
  }

  temp = dsm_pullup(sdu_in, (void*)pin_value_ptr, tlv_data.pin_len);
  if (temp != tlv_data.pin_len)
  {
    qmi_err = QMI_ERR_MALFORMED_MSG;
    goto return_decrypt_pin;
  }

  memset(decrypted_pin_ptr, 0x00, sizeof(qmi_uim_tlv_decrypted_pin_type));

  decrypted_len = sizeof(decrypted_pin_ptr->decrypted_pin_value);

  /* Decrypt the PIN and update the TLV */
  if (!qmi_uim_decrypt_pin(tlv_data.pin_len,
                           pin_value_ptr,
                           (uint8*)&decrypted_len,
                           decrypted_pin_ptr->decrypted_pin_value))
  {
    qmi_err = QMI_ERR_INCORRECT_PIN;
    goto return_decrypt_pin;
  }

  decrypted_pin_ptr->decrypted_pin_data.data_len = (mmgsdi_len_type)decrypted_len;

  /* Check decrypted pin length */
  if (decrypted_pin_ptr->decrypted_pin_data.data_len == 0 ||
      decrypted_pin_ptr->decrypted_pin_data.data_len > QMI_UIM_TLV_MAX_PIN_LEN)
  {
    UIM_MSG_ERR_1("Invalid decrypted PIN length: 0x%x",
                  decrypted_pin_ptr->decrypted_pin_data.data_len);
    qmi_err = QMI_ERR_INCORRECT_PIN;
    goto return_decrypt_pin;
  }

  decrypted_pin_ptr->decrypted_pin_data.data_ptr = decrypted_pin_ptr->decrypted_pin_value;
  qmi_err = QMI_ERR_NONE;

return_decrypt_pin:
  uimqmi_free(pin_value_ptr);
  return qmi_err;
} /* qmi_uim_tlv_decrypted_pin */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_PIN_KEYREF_ID()

  DESCRIPTION
    Parses the TLV item of key reference id

  PARAMETERS
    sdu_in                 : input sdu
    len                    : length of the TLV item
    set_pin_protection_ptr : output with key reference id value

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_pin_keyref_id
(
  dsm_item_type **                      sdu_in,
  uint16                                len,
  qmi_uim_tlv_pin_keyref_id_type      * pin_keyref_id_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint8  keyref_id;
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(pin_keyref_id_ptr);

  /* Check length */
  if (len != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, len);
  if (temp != len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Key reference id */
  switch(tlv_data.keyref_id)
  {
    case 0x01:
      pin_keyref_id_ptr->keyref_id = QMI_UIM_PIN_KEYREF_ID_APPL_1;
      break;
    case 0x02:
      pin_keyref_id_ptr->keyref_id = QMI_UIM_PIN_KEYREF_ID_APPL_2;
      break;
    case 0x03:
      pin_keyref_id_ptr->keyref_id = QMI_UIM_PIN_KEYREF_ID_APPL_3;
      break;
    case 0x04:
      pin_keyref_id_ptr->keyref_id = QMI_UIM_PIN_KEYREF_ID_APPL_4;
      break;
    case 0x05:
      pin_keyref_id_ptr->keyref_id = QMI_UIM_PIN_KEYREF_ID_APPL_5;
      break;
    case 0x06:
      pin_keyref_id_ptr->keyref_id = QMI_UIM_PIN_KEYREF_ID_APPL_6;
      break;
    case 0x07:
      pin_keyref_id_ptr->keyref_id = QMI_UIM_PIN_KEYREF_ID_APPL_7;
      break;
    case 0x08:
      pin_keyref_id_ptr->keyref_id = QMI_UIM_PIN_KEYREF_ID_APPL_8;
      break;
    default:
      return QMI_ERR_INVALID_ARG;
  }

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_pin_keyref_id */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_UINT32()

  DESCRIPTION
    Parses the uint32 TLV

  PARAMETERS
    sdu_in                 : input sdu
    len                    : length of the TLV item
    uint32_value_ptr       : value of uint32

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_uint32
(
  dsm_item_type **                          sdu_in,
  uint16                                    len,
  qmi_uim_tlv_uint32_type                 * uint32_value_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint32  value;
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(uint32_value_ptr);

  /* Check length */
  if (len != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, len);
  if (temp != len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Update token */
  uint32_value_ptr->value    = tlv_data.value;

  UIM_MSG_HIGH_1("qmi_uim_tlv_uint32: parsed value: 0x%x",
                 uint32_value_ptr->value);

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_uint32 */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_REGISTER_REFRESH()

  DESCRIPTION
    Parses the TLV item of register for refresh

  PARAMETERS
    sdu_in               : input sdu
    len                  : length of the TLV item
    register_refresh_ptr : output with register for refresh value

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_register_refresh
(
  dsm_item_type **                    sdu_in,
  uint16                              len,
  qmi_uim_tlv_register_refresh_type * register_refresh_ptr
)
{
  int32                     temp               = 0;
  uint16                    i                  = 0;
  uint8*                    file_list_data     = NULL;
  uint8*                    current            = NULL;
  uint16                    file_list_size     = 0;
  uint16                    file_count         = 0;
  qmi_error_e_type          ret_value          = QMI_ERR_NONE;
  qmi_uim_files_list_type * last_file_node_ptr = NULL;
  qmi_uim_files_list_type * new_file_node_ptr  = NULL;

  PACKED struct PACKED_POST
  {
    uint8  register_op;
    uint8  vote_for_init;
    uint16 num_files;
  } tlv_data;

  PACKED struct PACKED_POST
  {
    uint16 file_id;
    uint8  path_len;
  } tlv_data_file_id;

  ASSERT(sdu_in);
  ASSERT(register_refresh_ptr);

  /* Check length */
  if (len < sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, sizeof(tlv_data));
  if (temp != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Register or deregister */
  switch (tlv_data.register_op)
  {
    case 0:
      register_refresh_ptr->register_op = FALSE;
      break;
    case 1:
      register_refresh_ptr->register_op = TRUE;
      break;
    default:
      return QMI_ERR_INVALID_ARG;
  }

  /* Vote for init */
  switch (tlv_data.vote_for_init)
  {
    case 0:
      register_refresh_ptr->vote_for_init = FALSE;
      break;
    case 1:
      register_refresh_ptr->vote_for_init = TRUE;
      break;
    default:
      return QMI_ERR_INVALID_ARG;
  }

  /* Initialize the refresh list. */
  register_refresh_ptr->file_list_ptr = NULL;

  /* Basic check on size... we cannot check the size right now,
     as each file might have a different path length. Continue with
     each file is at least sizeof(uint16) */
  if ( len < sizeof(tlv_data) + (tlv_data.num_files * sizeof(uint16)) )
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* The case with zero files in the refresh list is
     perfectly valid: we can immediately return */
  if (tlv_data.num_files == 0)
  {
    return QMI_ERR_NONE;
  }

  /* Allocate memory */
  file_list_size = len - sizeof(tlv_data);
  file_list_data =
    (uint8*)uimqmi_malloc(file_list_size);
  if (file_list_data == NULL)
  {
    return QMI_ERR_NO_MEMORY;
  }

  /* Extract file list from payload */
  temp = dsm_pullup(sdu_in, (void*)file_list_data, len - sizeof(tlv_data));
  if (temp != len - sizeof(tlv_data))
  {
    uimqmi_free(file_list_data);
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Parse each file */
  current = file_list_data;
  for (i = 0; i < tlv_data.num_files; i++)
  {
    mmgsdi_access_type  file_access;

    memset(&file_access, 0, sizeof(mmgsdi_access_type));
    file_access.access_method = MMGSDI_BY_PATH_ACCESS;

    /* Check size */
    if (current + sizeof(tlv_data_file_id) > file_list_data + file_list_size)
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    /* Extract file id and size */
    (void)memscpy(&tlv_data_file_id, sizeof(tlv_data_file_id),
                  current, sizeof(tlv_data_file_id));
    current += sizeof(tlv_data_file_id);

    /* Check size for path */
    if ( (current + tlv_data_file_id.path_len > file_list_data + file_list_size) ||
         ((tlv_data_file_id.path_len & 0x01) != 0) )
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }
    if ( tlv_data_file_id.path_len > (MMGSDI_MAX_PATH_LEN - 1) * sizeof(uint16) )
    {
      ret_value = QMI_ERR_ARG_TOO_LONG;
      break;
    }

    /* Path length (in numbers of uint16) and value */
    file_access.file.path_type.path_len = tlv_data_file_id.path_len / sizeof(uint16);
    (void)memscpy(file_access.file.path_type.path_buf, sizeof(file_access.file.path_type.path_buf),
                  current, tlv_data_file_id.path_len);
    current += tlv_data_file_id.path_len;

    /* Add file id to the path */
    file_access.file.path_type.path_buf[file_access.file.path_type.path_len] =
      tlv_data_file_id.file_id;
    file_access.file.path_type.path_len += 1;

    /* Allocate node in the linked list */
    new_file_node_ptr =
      (qmi_uim_files_list_type*)uimqmi_malloc(sizeof(qmi_uim_files_list_type));
    if (new_file_node_ptr == NULL)
    {
      continue;
    }

    /* Copy file details into the new note */
    new_file_node_ptr->next_ptr = NULL;
    (void)memscpy((void*)&new_file_node_ptr->file,
                  sizeof(mmgsdi_access_type),
            (void*)&file_access,
            sizeof(mmgsdi_access_type) );

    /* Append new node to the end of the list */
    if (last_file_node_ptr == NULL)
    {
      register_refresh_ptr->file_list_ptr = new_file_node_ptr;
    }
    else
    {
      last_file_node_ptr->next_ptr = new_file_node_ptr;
    }
    last_file_node_ptr = new_file_node_ptr;

    file_count++;
  }

  UIM_MSG_HIGH_1("QMI_UIM files in the refresh list: 0x%x", file_count);

  /* Free temporary memory */
  uimqmi_free(file_list_data);

  return ret_value;
} /* qmi_uim_tlv_register_refresh */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_FILE_CONTROL_INFO()

  DESCRIPTION
    Parses the TLV item of file control information request type

  PARAMETERS
    sdu_in      : input sdu
    len         : length of the TLV item
    sub_ok_ptr  : output with ok file control information request type value

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_file_control_info
(
  dsm_item_type **                      sdu_in,
  uint16                                len,
  qmi_uim_tlv_file_control_info_type  * fci_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint8 fci;
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(fci_ptr);

  /* Check length */
  if (len != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, sizeof(tlv_data));
  if (temp != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract file control information requested */
  switch (tlv_data.fci)
  {
    case 0x00:
      fci_ptr->fci_type = MMGSDI_SELECT_RSP_NO_DATA;
      break;
    case 0x01:
      fci_ptr->fci_type = MMGSDI_SELECT_RSP_FCP;
      break;
    case 0x02:
      fci_ptr->fci_type = MMGSDI_SELECT_RSP_FCI;
      break;
    case 0x03:
      fci_ptr->fci_type = MMGSDI_SELECT_RSP_FCI_WITH_INTERFACES;
      break;
    case 0x04:
      fci_ptr->fci_type = MMGSDI_SELECT_RSP_FMD;
      break;
    default:
      return QMI_ERR_INVALID_ARG;
  }
  return QMI_ERR_NONE;
}/* qmi_uim_tlv_file_control_info */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_DECRYPTED_IMSI()

  DESCRIPTION
    Parses the TLV item for encrypted IMSI and decodes the IMSI

  PARAMETERS
    sdu_in                    : input sdu
    len                       : length of the TLV item
    decrypted_imsi_ptr        : output decoded IMSI

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_decrypted_imsi
(
  dsm_item_type **                         sdu_in,
  uint16                                   len,
  qmi_uim_tlv_decrypted_imsi_type *        decrypted_imsi_ptr
)
{
  int32             temp                  = 0;
  uint32            decrypted_imsi_len    = 0;
  uint8            *decrypt_buf_data_ptr  = NULL;

  PACKED struct PACKED_POST
  {
    uint8  length;
    uint8  encrypted_imsi_data[QMI_UIM_TLV_MAX_ENCRYPTED_IMSI_LEN];
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(decrypted_imsi_ptr);

  /* Check length */
  if (len < sizeof(tlv_data.length) || len > sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data and ensure it is formed properly */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, len);
  if (temp != len ||
      (len != (sizeof(tlv_data.length) + tlv_data.length)) ||
      tlv_data.length == 0)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  decrypted_imsi_len = (uint32)tlv_data.length;

  decrypt_buf_data_ptr = (uint8*)uimqmi_malloc(decrypted_imsi_len);
  if (decrypt_buf_data_ptr == NULL)
  {
    return QMI_ERR_NO_MEMORY;
  }

  /* check error result */
  if (!qmi_uim_decrypt_payload(tlv_data.encrypted_imsi_data,
                               tlv_data.length,
                               decrypt_buf_data_ptr,
                               &decrypted_imsi_len))
  {
    uimqmi_free(decrypt_buf_data_ptr);
    return QMI_ERR_INTERNAL;
  }

  if (decrypted_imsi_len > QMI_UIM_MAX_IMSI_LEN)
  {
    UIM_MSG_ERR_1("Invalid IMSI data length: 0x%x", decrypted_imsi_len);
    uimqmi_free(decrypt_buf_data_ptr);
    return QMI_ERR_INTERNAL;
  }

  decrypted_imsi_ptr->imsi_data.len = (uint8)decrypted_imsi_len;

  (void)memscpy((void*)decrypted_imsi_ptr->imsi_data.imsi,
                decrypted_imsi_ptr->imsi_data.len,
                (void*)decrypt_buf_data_ptr,
                decrypted_imsi_len);

  uimqmi_free(decrypt_buf_data_ptr);

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_decrypted_imsi */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_GET_CONFIG_MASK()

  DESCRIPTION
    Parses the TLV item get config mask

  PARAMETERS
    sdu_in                    : input sdu
    len                       : length of the TLV item
    get_config_mask_ptr       : output config mask

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_get_config_mask
(
  dsm_item_type                          ** sdu_in,
  uint16                                    len,
  qmi_uim_tlv_get_configuration_mask_type * get_config_mask_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint32  config_mask;
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(get_config_mask_ptr);

  /* Check length */
  if (len != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, len);
  if (temp != len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  get_config_mask_ptr->automatic_selection      =
           (tlv_data.config_mask & UIM_GET_CONFIGURATION_AUTOMATIC_SELECTION) ? TRUE : FALSE;
  get_config_mask_ptr->personalization_status   =
           (tlv_data.config_mask & UIM_GET_CONFIGURATION_PERSONALIZATION_STATUS) ? TRUE : FALSE;
  get_config_mask_ptr->halt_subscription_status =
           (tlv_data.config_mask & UIM_GET_CONFIGURATION_HALT_SUBSCRIPTION) ? TRUE: FALSE;
  get_config_mask_ptr->usb_uicc_supported_status =
           (tlv_data.config_mask & UIM_GET_CONFIGURATION_USB_UICC_SUPPORTED) ? TRUE: FALSE;
  get_config_mask_ptr->sap_client_supported_status =
           (tlv_data.config_mask & UIM_GET_CONFIGURATION_SAP_CLIENT_SUPPORTED) ? TRUE: FALSE;

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_qmi_get_config_mask */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_SAP_CONNECTION_CONDITION()

  DESCRIPTION
    Parses the TLV item of SAP connection condiion

  PARAMETERS
    sdu_in                   : input sdu
    len                      : length of the TLV item
    sap_conn_condition_ptr   : output with value of SAP connection condition

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_sap_connection_condition
(
  dsm_item_type **                            sdu_in,
  uint16                                      len,
  qmi_uim_tlv_sap_conn_condition_type       * sap_conn_condition_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint32  conn_condition;
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(sap_conn_condition_ptr);

  /* Check length */
  if (len != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, sizeof(tlv_data));
  if (temp != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Read data */
  switch (tlv_data.conn_condition)
  {
    case 0x00:
      sap_conn_condition_ptr->condition = MMGSDI_SAP_CONN_COND_BLOCK_VOICE_OR_DATA;
      break;
    case 0x01:
      sap_conn_condition_ptr->condition = MMGSDI_SAP_CONN_COND_BLOCK_DATA;
      break;
    case 0x02:
      sap_conn_condition_ptr->condition = MMGSDI_SAP_CONN_COND_BLOCK_VOICE;
      break;
    case 0x03:
      sap_conn_condition_ptr->condition = MMGSDI_SAP_CONN_COND_BLOCK_NONE;
      break;
    default:
      return QMI_ERR_INVALID_ARG;
  }

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_sap_connection_condition */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_FILE_STATUS()

  DESCRIPTION
    Parses the TLV item of set file status

  PARAMETERS
    sdu_in          : input sdu
    len             : length of the TLV item
    file_status_ptr : output with file status value

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_file_status
(
  dsm_item_type **               sdu_in,
  uint16                         len,
  qmi_uim_tlv_file_status_type * file_status_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint32  file_status;
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(file_status_ptr);

  /* Check length */
  if (len != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, len);
  if (temp != len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Card status */
  switch(tlv_data.file_status)
  {
    case 0:
      file_status_ptr->file_status = QMI_UIM_FILE_STATUS_DEACTIVATED;
      break;
    case 1:
      file_status_ptr->file_status = QMI_UIM_FILE_STATUS_ACTIVATED;
      break;
    default:
      return QMI_ERR_INVALID_ARG;
  }

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_file_status */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_INCREASE()

  DESCRIPTION
    Parses the TLV item of increase

  PARAMETERS
    sdu_in                : input sdu
    len                   : length of the TLV item
    increase_ptr          : output with increase value

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    Allocates memory that needs to be freed by the caller

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_increase
(
  dsm_item_type **                     sdu_in,
  uint16                               len,
  qmi_uim_tlv_increase_type          * increase_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint8 length;
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(increase_ptr);

  /* Check length */
  if (len < sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, sizeof(tlv_data));
  if (temp != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Check data length */
  if (len == 0 ||
      len != sizeof(tlv_data) + tlv_data.length)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Length */
  increase_ptr->data.data_len = (mmgsdi_len_type)tlv_data.length;

  /* Extract body to write. The memory allocated here needs to be freed by
     the caller, after using the data */
  increase_ptr->data.data_ptr =
        (uint8*)uimqmi_malloc(tlv_data.length);
  if (increase_ptr->data.data_ptr == NULL)
  {
    return QMI_ERR_NO_MEMORY;
  }

  temp = dsm_pullup(sdu_in,
                    (void*)increase_ptr->data.data_ptr,
                    tlv_data.length);
  if (temp != tlv_data.length)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_increase */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_SELECT_MODE()

  DESCRIPTION
    Parses the TLV item for mode of selection

  PARAMETERS
    sdu_in          : input sdu
    len             : length of the TLV item
    select_mode_ptr : output with selection mode value

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_select_mode
(
  dsm_item_type **               sdu_in,
  uint16                         len,
  qmi_uim_tlv_select_mode_type * select_mode_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint32  select_mode;
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(select_mode_ptr);

  /* Check length */
  if (len != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, len);
  if (temp != len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Card status */
  switch(tlv_data.select_mode)
  {
    case 0:
      select_mode_ptr->mode = MMGSDI_SELECT_MODE_FIRST_OR_ONLY;
      break;
    case 1:
      select_mode_ptr->mode = MMGSDI_SELECT_MODE_LAST;
      break;
    case 2:
      select_mode_ptr->mode = MMGSDI_SELECT_MODE_NEXT;
      break;
    case 3:
      select_mode_ptr->mode = MMGSDI_SELECT_MODE_PREVIOUS;
      break;
    default:
      return QMI_ERR_INVALID_ARG;
  }

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_select_mode */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_STATUS_CMD_MODE()

  DESCRIPTION
    Parses the TLV item for status cmd mode

  PARAMETERS
    sdu_in          : input sdu
    len             : length of the TLV item
    select_mode_ptr : output with selection mode value

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_status_cmd_mode
(
  dsm_item_type **                   sdu_in,
  uint16                             len,
  qmi_uim_tlv_status_cmd_mode_type * status_cmd_mode_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint32 status_cmd_mode;
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(status_cmd_mode_ptr);

  /* Check length */
  if (len != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, len);
  if (temp != len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Card status */
  switch(tlv_data.status_cmd_mode)
  {
    case 0:
      status_cmd_mode_ptr->mode = MMGSDI_STATUS_APP_NO_INDICATION;
      break;
    case 1:
      status_cmd_mode_ptr->mode = MMGSDI_STATUS_APP_INITIALIZED;
      break;
    case 2:
      status_cmd_mode_ptr->mode = MMGSDI_STATUS_APP_TERMINATED;
      break;
    default:
      return QMI_ERR_INVALID_ARG;
  }

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_status_cmd_mode */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_STATUS_CMD_RESP()

  DESCRIPTION
    Parses the TLV item for status cmd responce

  PARAMETERS
    sdu_in          : input sdu
    len             : length of the TLV item
    select_mode_ptr : output with selection mode value

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_status_cmd_resp
(
  dsm_item_type **                   sdu_in,
  uint16                             len,
  qmi_uim_tlv_status_cmd_resp_type * status_cmd_resp_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint32 status_cmd_resp;
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(status_cmd_resp_ptr);

  /* Check length */
  if (len != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, len);
  if (temp != len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Card status */
  switch(tlv_data.status_cmd_resp)
  {
    case 0:
      status_cmd_resp_ptr->resp = MMGSDI_STATUS_DATA_SELECT;
      break;
    case 1:
      status_cmd_resp_ptr->resp = MMGSDI_STATUS_TLV_DATA;
      break;
    case 2:
      status_cmd_resp_ptr->resp = MMGSDI_STATUS_NO_DATA;
      break;
    default:
      return QMI_ERR_INVALID_ARG;
  }

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_status_cmd_resp */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_SERVICE_NUMBER()

  DESCRIPTION
    Parses the TLV item of service number

  PARAMETERS
    sdu_in              : input sdu
    len                 : length of the TLV item
    service_number_ptr  : output with service number value

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_service_number
(
  dsm_item_type                      ** sdu_in,
  uint16                                len,
  qmi_uim_tlv_service_number_type     * service_number_ptr
)
{
  int32 temp;

  PACKED struct PACKED_POST
  {
    uint8 service_number;
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(service_number_ptr);

  /* Check length */
  if (len != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, len);
  if (temp != len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Copy over service number */
  service_number_ptr->service_number = tlv_data.service_number;

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_service_number */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_PERSONALIZATION_PARSE_SPN_CODE()

  DESCRIPTION
    Parses the SPN codes

  PARAMETERS
    num_codes           : number of SPN codes
    code_data_buf_len   : length of raw code data
    code_data_buf_ptr   : pointer to raw code data
    num_lists           : number of lists needed for SPN codes
    spn_code_data_ptr   : output with SPN codes

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_personalization_parse_spn_code
(
  uint8                              num_codes,
  uint16                             code_data_buf_len,
  const uint8                    *   code_data_buf_ptr,
  uint8                              num_lists,
  simlock_category_data_type     *   spn_code_data_ptr
)
{
  qmi_error_e_type   ret_value         = QMI_ERR_NONE;
  uint8              code_cnt          = 0;
  uint8              list_cnt          = 0;
  uint8              list_code_cnt     = 0;
  uint8              mnc_len           = 0;
  const uint8      * current_code_ptr  = NULL;

  if ((code_data_buf_len == 0) ||
      (code_data_buf_ptr == NULL) ||
      (spn_code_data_ptr == NULL))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  current_code_ptr = code_data_buf_ptr;

  for(code_cnt = 0; code_cnt < num_codes; code_cnt++)
  {
    /* Check the data size */
    if(current_code_ptr + SIMLOCK_MCC_LEN + sizeof(mnc_len) >
        code_data_buf_ptr + code_data_buf_len)
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    /* Each list will fit, SIMLOCK_NUM_CODES_MAX. So if
       list_code_cnt is SIMLOCK_NUM_CODES_MAX, then fill
       the next list */
    if(list_code_cnt == SIMLOCK_NUM_CODES_MAX)
    {
      if(list_cnt < num_lists)
      {
        list_code_cnt = 0;
        list_cnt++;
      }
      else
      {
        ret_value = QMI_ERR_MALFORMED_MSG;
        break;
      }
    }

    if((list_cnt >= num_lists) ||
       (list_code_cnt >= SIMLOCK_NUM_CODES_MAX))
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    (void)memscpy((void*)spn_code_data_ptr[list_cnt].code_data.spn_3gpp_code_list.spn_code_data[list_code_cnt].nw_code.mcc,
                  SIMLOCK_MCC_LEN,
                  current_code_ptr,
                  SIMLOCK_MCC_LEN);
    current_code_ptr += SIMLOCK_MCC_LEN;

    mnc_len = *current_code_ptr;

    /* validate the mnc_len */
    if((mnc_len != QMI_UIM_SIMLOCK_MNC_MIN) &&
       (mnc_len != QMI_UIM_MNC_MAX_V01))
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    spn_code_data_ptr[list_cnt].code_data.spn_3gpp_code_list.spn_code_data[list_code_cnt].nw_code.mnc.mnc_len = mnc_len;
    current_code_ptr += sizeof(mnc_len);

    if(current_code_ptr + mnc_len > code_data_buf_ptr + code_data_buf_len)
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    (void)memscpy((void*)spn_code_data_ptr[list_cnt].code_data.spn_3gpp_code_list.spn_code_data[list_code_cnt].nw_code.mnc.mnc_data,
                  SIMLOCK_MNC_MAX,
                  current_code_ptr,
                  spn_code_data_ptr[list_cnt].code_data.spn_3gpp_code_list.spn_code_data[list_code_cnt].nw_code.mnc.mnc_len);
    current_code_ptr += mnc_len;

    /* Check the data size */
    if(current_code_ptr + SIMLOCK_SPN_LEN > code_data_buf_ptr + code_data_buf_len)
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    (void)memscpy(spn_code_data_ptr[list_cnt].code_data.spn_3gpp_code_list.spn_code_data[list_code_cnt].spn.spn_data,
                  SIMLOCK_SPN_LEN,
                  current_code_ptr,
                  SIMLOCK_SPN_LEN);

    spn_code_data_ptr[list_cnt].code_data.spn_3gpp_code_list.spn_code_data[list_code_cnt].spn.spn_valid = TRUE;

    current_code_ptr += SIMLOCK_SPN_LEN;

    spn_code_data_ptr[list_cnt].code_data.spn_3gpp_code_list.num_spn_codes++;
    list_code_cnt++;
  }

  if (ret_value == QMI_ERR_NONE &&
      current_code_ptr != code_data_buf_ptr + code_data_buf_len)
  {
    ret_value = QMI_ERR_MALFORMED_MSG;
  }

  return ret_value;
} /* qmi_uim_tlv_personalization_parse_spn_code*/


/*===========================================================================
  FUNCTION QMI_UIM_TLV_PERSONALIZATION_PARSE_ICCID_CODE()

  DESCRIPTION
    Parses the ICCID codes

  PARAMETERS
    num_codes           : number of ICCID codes
    code_data_buf_len   : length of raw code data
    code_data_buf_ptr   : pointer to raw code data
    num_lists           : number of lists needed for ICCID codes
    extended_category   : type of category. normal, ext or ext2
    iccid_code_data_ptr : output with ICCID codes

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_personalization_parse_iccid_code
(
  uint8                                    num_codes,
  uint16                                   code_data_buf_len,
  const uint8                            * code_data_buf_ptr,
  uint8                                    num_lists,
  simlock_category_data_type             * iccid_code_data_ptr
)
{
  qmi_error_e_type   ret_value         = QMI_ERR_NONE;
  uint8              code_cnt          = 0;
  uint8              list_cnt          = 0;
  uint8              list_code_cnt     = 0;
  uint8              mnc_len           = 0;
  const uint8      * current_code_ptr  = NULL;
  uint8              iccid_len         = 0;

  if ((code_data_buf_len == 0) ||
      (code_data_buf_ptr == NULL) ||
      (iccid_code_data_ptr == NULL))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  current_code_ptr = code_data_buf_ptr;

  for(code_cnt = 0; code_cnt < num_codes; code_cnt++)
  {
    /* Check the data size */
    if(current_code_ptr + SIMLOCK_MCC_LEN + sizeof(mnc_len) >
         code_data_buf_ptr + code_data_buf_len)
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    /* Each list will fit, SIMLOCK_NUM_CODES_MAX. So if
       list_code_cnt is SIMLOCK_NUM_CODES_MAX, then fill
       the next list */
    if(list_code_cnt == SIMLOCK_NUM_CODES_MAX)
    {
      if(list_cnt < num_lists)
      {
        list_code_cnt = 0;
        list_cnt++;
      }
      else
      {
        ret_value = QMI_ERR_MALFORMED_MSG;
        break;
      }
    }

    if((list_cnt >= num_lists) ||
       (list_code_cnt >= SIMLOCK_NUM_CODES_MAX))
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    (void)memscpy((void*)iccid_code_data_ptr[list_cnt].code_data.iccid_3gpp_code_list.iccid_code_data[list_code_cnt].nw_code.mcc,
                    SIMLOCK_MCC_LEN,
                    current_code_ptr,
                    SIMLOCK_MCC_LEN);
    current_code_ptr += SIMLOCK_MCC_LEN;

    mnc_len = *current_code_ptr;

    /* validate the mnc_len */
    if((mnc_len != QMI_UIM_SIMLOCK_MNC_MIN) &&
       (mnc_len != QMI_UIM_MNC_MAX_V01))
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    iccid_code_data_ptr[list_cnt].code_data.iccid_3gpp_code_list.iccid_code_data[list_code_cnt].nw_code.mnc.mnc_len = mnc_len;
    current_code_ptr += sizeof(mnc_len);

    if(current_code_ptr + mnc_len > code_data_buf_ptr + code_data_buf_len)
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    (void)memscpy((void*)iccid_code_data_ptr[list_cnt].code_data.iccid_3gpp_code_list.iccid_code_data[list_code_cnt].nw_code.mnc.mnc_data,
            SIMLOCK_MNC_MAX,
            current_code_ptr,
            iccid_code_data_ptr[list_cnt].code_data.iccid_3gpp_code_list.iccid_code_data[list_code_cnt].nw_code.mnc.mnc_len);
    current_code_ptr += mnc_len;

    /* Check the data size */
    if(current_code_ptr + sizeof(uint8) > code_data_buf_ptr + code_data_buf_len)
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    iccid_len = *current_code_ptr;

    /* validate the iccid_len */
    if(iccid_len == 0)
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    if(iccid_len > QMI_UIM_ICCID_DIGIT_MAX_V01)
    {
      ret_value = QMI_ERR_ARG_TOO_LONG;
      break;
    }

    iccid_code_data_ptr[list_cnt].code_data.iccid_3gpp_code_list.iccid_code_data[list_code_cnt].iccid.iccid_len = iccid_len;
    current_code_ptr += sizeof(uint8);

    /* Check the data size */
    if(current_code_ptr + iccid_len > code_data_buf_ptr + code_data_buf_len)
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    (void)memscpy((void*)iccid_code_data_ptr[list_cnt].code_data.iccid_3gpp_code_list.iccid_code_data[list_code_cnt].iccid.iccid_data,
            sizeof(iccid_code_data_ptr[list_cnt].code_data.iccid_3gpp_code_list.iccid_code_data[list_code_cnt].iccid.iccid_data),
            current_code_ptr,
            iccid_len);
    current_code_ptr += iccid_len;

    iccid_code_data_ptr[list_cnt].code_data.iccid_3gpp_code_list.num_iccid_codes++;
    list_code_cnt++;
  }

  if (ret_value == QMI_ERR_NONE &&
      current_code_ptr != code_data_buf_ptr + code_data_buf_len)
  {
    ret_value = QMI_ERR_MALFORMED_MSG;
  }

  return ret_value;
} /* qmi_uim_tlv_personalization_parse_iccid_code*/


/*===========================================================================
  FUNCTION QMI_UIM_TLV_PERSONALIZATION_PARSE_RUIM_CODE()

  DESCRIPTION
    Parses the RUIM codes

  PARAMETERS
    num_codes          : number of RUIM codes
    code_data_buf_len  : length of raw code data
    code_data_buf_ptr  : pointer to raw code data
    num_lists          : number of lists needed for RUIM codes
    ruim_code_data_ptr : output with RUIM codes

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_personalization_parse_ruim_code
(
  uint8                              num_codes,
  uint16                             code_data_buf_len,
  const uint8                    *   code_data_buf_ptr,
  uint8                              num_lists,
  simlock_category_data_type     *   ruim_code_data_ptr
)
{
  qmi_error_e_type   ret_value         = QMI_ERR_NONE;
  uint8              code_cnt          = 0;
  uint8              list_cnt          = 0;
  uint8              list_code_cnt     = 0;
  uint8              mnc_len           = 0;
  uint8              msin_len          = 0;
  const uint8      * current_code_ptr  = NULL;

  if ((code_data_buf_len == 0) ||
      (code_data_buf_ptr == NULL) ||
      (ruim_code_data_ptr == NULL))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  current_code_ptr = code_data_buf_ptr;

  for(code_cnt = 0; code_cnt < num_codes; code_cnt++)
  {
    /* Check the data size */
    if(current_code_ptr + SIMLOCK_MCC_LEN + sizeof(mnc_len) >
        code_data_buf_ptr + code_data_buf_len)
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    /* Each list will fit, SIMLOCK_NUM_CODES_MAX. So if
       list_code_cnt is SIMLOCK_NUM_CODES_MAX, then fill
       the next list */
    if(list_code_cnt == SIMLOCK_NUM_CODES_MAX)
    {
      if(list_cnt < num_lists)
      {
        list_code_cnt = 0;
        list_cnt++;
      }
      else
      {
        ret_value = QMI_ERR_MALFORMED_MSG;
        break;
      }
    }

    if((list_cnt >= num_lists) ||
       (list_code_cnt >= SIMLOCK_NUM_CODES_MAX))
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    (void)memscpy((void*)ruim_code_data_ptr[list_cnt].code_data.ruim_3gpp2_code_list.sim_code_data[list_code_cnt].nw_code.mcc,
                  SIMLOCK_MCC_LEN,
                  current_code_ptr,
                  SIMLOCK_MCC_LEN);
    current_code_ptr += SIMLOCK_MCC_LEN;

    mnc_len = *current_code_ptr;

    /* validate the mnc_len */
    if((mnc_len != QMI_UIM_SIMLOCK_MNC_MIN) &&
       (mnc_len != QMI_UIM_SIMLOCK_MNC_MAX))
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    ruim_code_data_ptr[list_cnt].code_data.ruim_3gpp2_code_list.sim_code_data[list_code_cnt].nw_code.mnc.mnc_len = mnc_len;
    current_code_ptr += sizeof(mnc_len);

    /* Check the data size */
    if(current_code_ptr + mnc_len + sizeof(msin_len) > code_data_buf_ptr + code_data_buf_len)
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    (void)memscpy((void*)ruim_code_data_ptr[list_cnt].code_data.ruim_3gpp2_code_list.sim_code_data[list_code_cnt].nw_code.mnc.mnc_data,
                  SIMLOCK_MNC_MAX,
                  current_code_ptr,
                  ruim_code_data_ptr[list_cnt].code_data.ruim_3gpp2_code_list.sim_code_data[list_code_cnt].nw_code.mnc.mnc_len);
    current_code_ptr += mnc_len;

    msin_len = *current_code_ptr;

    /* validate the msin_len */
    if(msin_len > SIMLOCK_MSIN_MAX)
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    ruim_code_data_ptr[list_cnt].code_data.ruim_3gpp2_code_list.sim_code_data[list_code_cnt].imsi_msin.msin_len = msin_len;
    current_code_ptr += sizeof(msin_len);

    /* Check the data size */
    if(current_code_ptr + msin_len > code_data_buf_ptr + code_data_buf_len)
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    (void)memscpy((void*)ruim_code_data_ptr[list_cnt].code_data.ruim_3gpp2_code_list.sim_code_data[list_code_cnt].imsi_msin.msin_data,
                  SIMLOCK_MSIN_MAX,
                  current_code_ptr,
                  ruim_code_data_ptr[list_cnt].code_data.ruim_3gpp2_code_list.sim_code_data[list_code_cnt].imsi_msin.msin_len);
    current_code_ptr += msin_len;

    ruim_code_data_ptr[list_cnt].code_data.ruim_3gpp2_code_list.num_sim_codes++;
    list_code_cnt++;
  }

  if (current_code_ptr != code_data_buf_ptr + code_data_buf_len)
  {
    ret_value = QMI_ERR_MALFORMED_MSG;
  }

  return ret_value;
} /* qmi_uim_tlv_personalization_parse_ruim_code */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_PERSONALIZATION_PARSE_NW_TYPE2_CODE()

  DESCRIPTION
    Parses the NW type2 codes

  PARAMETERS
    num_codes              : number of NW Type2 codes
    code_data_buf_len      : length of raw code data
    code_data_buf_ptr      : pointer to raw code data
    num_lists              : number of lists needed for nw_type2 codes
    nw_type2_code_data_ptr : output with NW Type2 codes

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_personalization_parse_nw_type2_code
(
  uint8                              num_codes,
  uint16                             code_data_buf_len,
  const uint8                    *   code_data_buf_ptr,
  uint8                              num_lists,
  simlock_category_data_type     *   nw_type2_code_data_ptr
)
{
  qmi_error_e_type   ret_value         = QMI_ERR_NONE;
  uint8              code_cnt          = 0;
  uint8              list_cnt          = 0;
  uint8              list_code_cnt     = 0;
  const uint8      * current_code_ptr  = NULL;

  if ((code_data_buf_len == 0) ||
      (code_data_buf_ptr == NULL) ||
      (nw_type2_code_data_ptr == NULL))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  current_code_ptr = code_data_buf_ptr;

  for(code_cnt = 0; code_cnt < num_codes; code_cnt++)
  {
    /* Check the data size */
    if(code_data_buf_ptr + SIMLOCK_IRM_LEN > code_data_buf_ptr + code_data_buf_len)
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    /* Each list will fit, SIMLOCK_NUM_CODES_MAX. So if
       list_code_cnt is SIMLOCK_NUM_CODES_MAX, then fill
       the next list */
    if(list_code_cnt == SIMLOCK_NUM_CODES_MAX)
    {
      if(list_cnt < num_lists)
      {
        list_code_cnt = 0;
        list_cnt++;
      }
      else
      {
        ret_value = QMI_ERR_MALFORMED_MSG;
        break;
      }
    }

    if((list_cnt >= num_lists) ||
       (list_code_cnt >= SIMLOCK_NUM_CODES_MAX))
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    nw_type2_code_data_ptr[list_cnt].category_type = SIMLOCK_CATEGORY_3GPP2_NW_TYPE2;

    (void)memscpy((void*)nw_type2_code_data_ptr[list_cnt].code_data.nw_type2_3gpp2_code_list.nw_type2_code_data[list_code_cnt].irm,
                  SIMLOCK_IRM_LEN,
                  current_code_ptr,
                  SIMLOCK_IRM_LEN);
    current_code_ptr += SIMLOCK_IRM_LEN;

    nw_type2_code_data_ptr[list_cnt].code_data.nw_type2_3gpp2_code_list.num_nw_type2_codes++;
    list_code_cnt++;
  }

  if (current_code_ptr != code_data_buf_ptr + code_data_buf_len)
  {
    ret_value = QMI_ERR_MALFORMED_MSG;
  }

  return ret_value;
} /* qmi_uim_tlv_personalization_parse_nw_type2_code */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_PERSONALIZATION_PARSE_NW_TYPE1_CODE()

  DESCRIPTION
    Parses the NW type1 codes

  PARAMETERS
    num_codes              : number of NW Type1 codes
    code_data_buf_len      : length of raw code data
    code_data_buf_ptr      : pointer to raw code data
    num_lists              : number of lists needed for nw_type1 codes
    nw_type1_code_data_ptr : output with NW Type1 codes

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_personalization_parse_nw_type1_code
(
  uint8                              num_codes,
  uint16                             code_data_buf_len,
  const uint8                    *   code_data_buf_ptr,
  uint8                              num_lists,
  simlock_category_data_type     *   nw_type1_code_data_ptr
)
{
  qmi_error_e_type   ret_value         = QMI_ERR_NONE;
  uint8              code_cnt          = 0;
  uint8              list_cnt          = 0;
  uint8              list_code_cnt     = 0;
  uint8              mnc_len           = 0;
  const uint8      * current_code_ptr  = NULL;

  if ((code_data_buf_len == 0) ||
      (code_data_buf_ptr == NULL) ||
      (nw_type1_code_data_ptr == NULL))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  current_code_ptr = code_data_buf_ptr;

  for(code_cnt = 0; code_cnt < num_codes; code_cnt++)
  {
    /* Check the data size */
    if(current_code_ptr + SIMLOCK_MCC_LEN + sizeof(mnc_len) >
        code_data_buf_ptr + code_data_buf_len)
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    /* Each list will fit, SIMLOCK_NUM_CODES_MAX. So if
       list_code_cnt is SIMLOCK_NUM_CODES_MAX, then fill
       the next list */
    if(list_code_cnt == SIMLOCK_NUM_CODES_MAX)
    {
      if(list_cnt < num_lists)
      {
        list_code_cnt = 0;
        list_cnt++;
      }
      else
      {
        ret_value = QMI_ERR_MALFORMED_MSG;
        break;
      }
    }

    if((list_cnt >= num_lists) ||
       (list_code_cnt >= SIMLOCK_NUM_CODES_MAX))
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    (void)memscpy((void*)nw_type1_code_data_ptr[list_cnt].code_data.nw_type1_3gpp2_code_list.nw_code_data[list_code_cnt].mcc,
                  SIMLOCK_MCC_LEN,
                  current_code_ptr,
                  SIMLOCK_MCC_LEN);
    current_code_ptr += SIMLOCK_MCC_LEN;

    mnc_len = *current_code_ptr;

    /* validate the mnc_len */
    if((mnc_len != QMI_UIM_SIMLOCK_MNC_MIN) &&
       (mnc_len != QMI_UIM_SIMLOCK_MNC_MAX))
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    nw_type1_code_data_ptr[list_cnt].code_data.nw_type1_3gpp2_code_list.nw_code_data[list_code_cnt].mnc.mnc_len = mnc_len;
    current_code_ptr += sizeof(mnc_len);

    /* Check the data size */
    if(current_code_ptr + mnc_len > code_data_buf_ptr + code_data_buf_len)
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    (void)memscpy((void*)nw_type1_code_data_ptr[list_cnt].code_data.nw_type1_3gpp2_code_list.nw_code_data[list_code_cnt].mnc.mnc_data,
                  SIMLOCK_MNC_MAX,
                  current_code_ptr,
                  nw_type1_code_data_ptr[list_cnt].code_data.nw_type1_3gpp2_code_list.nw_code_data[list_code_cnt].mnc.mnc_len);
    current_code_ptr += mnc_len;

    nw_type1_code_data_ptr[list_cnt].code_data.nw_type1_3gpp2_code_list.num_nw_codes++;
    list_code_cnt++;
  }

  if (current_code_ptr != code_data_buf_ptr + code_data_buf_len)
  {
    ret_value = QMI_ERR_MALFORMED_MSG;
  }

  return ret_value;
} /* qmi_uim_tlv_personalization_parse_nw_type1_code */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_PERSONALIZATION_PARSE_SIM_CODE()

  DESCRIPTION
    Parses the SIM codes

  PARAMETERS
    num_codes           : number of SIM codes
    code_data_buf_len   : length of raw code data
    code_data_buf_ptr   : pointer to raw code data
    num_lists           : number of lists needed for SIM codes
    sim_code_data_ptr   : output with SIM codes

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_personalization_parse_sim_code
(
  uint8                              num_codes,
  uint16                             code_data_buf_len,
  const uint8                    *   code_data_buf_ptr,
  uint8                              num_lists,
  simlock_category_data_type     *   sim_code_data_ptr
)
{
  qmi_error_e_type   ret_value         = QMI_ERR_NONE;
  uint8              code_cnt          = 0;
  uint8              list_cnt          = 0;
  uint8              list_code_cnt     = 0;
  uint8              mnc_len           = 0;
  uint8              msin_len          = 0;
  const uint8      * current_code_ptr  = NULL;

  if ((code_data_buf_len == 0) ||
      (code_data_buf_ptr == NULL) ||
      (sim_code_data_ptr == NULL))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  current_code_ptr = code_data_buf_ptr;

  for(code_cnt = 0; code_cnt < num_codes; code_cnt++)
  {
    /* Check the data size */
    if(current_code_ptr + SIMLOCK_MCC_LEN + sizeof(mnc_len) >
        code_data_buf_ptr + code_data_buf_len)
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    /* Each list will fit, SIMLOCK_NUM_CODES_MAX. So if
       list_code_cnt is SIMLOCK_NUM_CODES_MAX, then fill
       the next list */
    if(list_code_cnt == SIMLOCK_NUM_CODES_MAX)
    {
      if(list_cnt < num_lists)
      {
        list_code_cnt = 0;
        list_cnt++;
      }
      else
      {
        ret_value = QMI_ERR_MALFORMED_MSG;
        break;
      }
    }

    if((list_cnt >= num_lists) ||
       (list_code_cnt >= SIMLOCK_NUM_CODES_MAX))
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    (void)memscpy((void*)sim_code_data_ptr[list_cnt].code_data.sim_3gpp_code_list.sim_code_data[list_code_cnt].nw_code.mcc,
                  SIMLOCK_MCC_LEN,
                  current_code_ptr,
                  SIMLOCK_MCC_LEN);
    current_code_ptr += SIMLOCK_MCC_LEN;

    mnc_len = *current_code_ptr;

    /* validate the mnc_len */
    if((mnc_len != QMI_UIM_SIMLOCK_MNC_MIN) &&
       (mnc_len != QMI_UIM_SIMLOCK_MNC_MAX))
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    sim_code_data_ptr[list_cnt].code_data.sim_3gpp_code_list.sim_code_data[list_code_cnt].nw_code.mnc.mnc_len = mnc_len;
    current_code_ptr += sizeof(mnc_len);

    /* Check the data size */
    if(current_code_ptr + mnc_len + sizeof(msin_len) > code_data_buf_ptr + code_data_buf_len)
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    (void)memscpy((void*)sim_code_data_ptr[list_cnt].code_data.sim_3gpp_code_list.sim_code_data[list_code_cnt].nw_code.mnc.mnc_data,
            SIMLOCK_MNC_MAX,
            current_code_ptr,
            sim_code_data_ptr[list_cnt].code_data.sim_3gpp_code_list.sim_code_data[list_code_cnt].nw_code.mnc.mnc_len);
    current_code_ptr += mnc_len;

    msin_len = *current_code_ptr;

    /* validate the msin_len */
    if(msin_len > SIMLOCK_MSIN_MAX)
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    sim_code_data_ptr[list_cnt].code_data.sim_3gpp_code_list.sim_code_data[list_code_cnt].imsi_msin.msin_len = msin_len;
    current_code_ptr += sizeof(msin_len);

    /* Check the data size */
    if(current_code_ptr + msin_len > code_data_buf_ptr + code_data_buf_len)
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    (void)memscpy((void*)sim_code_data_ptr[list_cnt].code_data.sim_3gpp_code_list.sim_code_data[list_code_cnt].imsi_msin.msin_data,
            SIMLOCK_MSIN_MAX,
            current_code_ptr,
            sim_code_data_ptr[list_cnt].code_data.sim_3gpp_code_list.sim_code_data[list_code_cnt].imsi_msin.msin_len);
    current_code_ptr += msin_len;

    sim_code_data_ptr[list_cnt].code_data.sim_3gpp_code_list.num_sim_codes++;
    list_code_cnt++;
  }

  if (current_code_ptr != code_data_buf_ptr + code_data_buf_len)
  {
    ret_value = QMI_ERR_MALFORMED_MSG;
  }

  return ret_value;
} /* qmi_uim_tlv_personalization_parse_sim_code */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_PERSONALIZATION_PARSE_CP_CODE()

  DESCRIPTION
    Parses the CP codes

  PARAMETERS
    num_codes           : number of CP codes
    code_data_buf_len   : length of raw code data
    code_data_buf_ptr   : pointer to raw code data
    num_lists           : number of lists needed for CP codes
    cp_code_data_ptr    : output with CP codes

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_personalization_parse_cp_code
(
  uint8                              num_codes,
  uint16                             code_data_buf_len,
  const uint8                    *   code_data_buf_ptr,
  uint8                              num_lists,
  simlock_category_data_type     *   cp_code_data_ptr
)
{
  qmi_error_e_type   ret_value         = QMI_ERR_NONE;
  uint8              code_cnt          = 0;
  uint8              list_cnt          = 0;
  uint8              list_code_cnt     = 0;
  uint8              mnc_len           = 0;
  const uint8      * current_code_ptr  = NULL;

  if ((code_data_buf_len == 0) ||
      (code_data_buf_ptr == NULL) ||
      (cp_code_data_ptr == NULL))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  current_code_ptr = code_data_buf_ptr;

  for(code_cnt = 0; code_cnt < num_codes; code_cnt++)
  {
    /* Check the data size */
    if(current_code_ptr + SIMLOCK_MCC_LEN + sizeof(mnc_len) >
        code_data_buf_ptr + code_data_buf_len)
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    /* Each list will fit, SIMLOCK_NUM_CODES_MAX. So if
       list_code_cnt is SIMLOCK_NUM_CODES_MAX, then fill
       the next list */
    if(list_code_cnt == SIMLOCK_NUM_CODES_MAX)
    {
      if(list_cnt < num_lists)
      {
        list_code_cnt = 0;
        list_cnt++;
      }
      else
      {
        ret_value = QMI_ERR_MALFORMED_MSG;
        break;
      }
    }

    if((list_cnt >= num_lists) ||
       (list_code_cnt >= SIMLOCK_NUM_CODES_MAX))
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    (void)memscpy((void*)cp_code_data_ptr[list_cnt].code_data.cp_3gpp_code_list.cp_code_data[list_code_cnt].nw_code.mcc,
                  SIMLOCK_MCC_LEN,
                  current_code_ptr,
                  SIMLOCK_MCC_LEN);
    current_code_ptr += SIMLOCK_MCC_LEN;

    mnc_len = *current_code_ptr;

    /* validate the mnc_len */
    if((mnc_len != QMI_UIM_SIMLOCK_MNC_MIN) &&
       (mnc_len != QMI_UIM_SIMLOCK_MNC_MAX))
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    cp_code_data_ptr[list_cnt].code_data.cp_3gpp_code_list.cp_code_data[list_code_cnt].nw_code.mnc.mnc_len = mnc_len;
    current_code_ptr += sizeof(mnc_len);

    /* Check the data size */
    if(current_code_ptr + mnc_len > code_data_buf_ptr + code_data_buf_len)
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    (void)memscpy((void*)cp_code_data_ptr[list_cnt].code_data.cp_3gpp_code_list.cp_code_data[list_code_cnt].nw_code.mnc.mnc_data,
                  SIMLOCK_MNC_MAX,
                  current_code_ptr,
                  cp_code_data_ptr[list_cnt].code_data.cp_3gpp_code_list.cp_code_data[list_code_cnt].nw_code.mnc.mnc_len);
    current_code_ptr += mnc_len;

    /* Check the data size */
    if(current_code_ptr + sizeof(uint8) + sizeof(uint8) >
        code_data_buf_ptr + code_data_buf_len)
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    cp_code_data_ptr[list_cnt].code_data.cp_3gpp_code_list.cp_code_data[list_code_cnt].gid1.gid_data[0] = *current_code_ptr;
    cp_code_data_ptr[list_cnt].code_data.cp_3gpp_code_list.cp_code_data[list_code_cnt].gid1.gid_len = 1;
    current_code_ptr += sizeof(uint8);

    cp_code_data_ptr[list_cnt].code_data.cp_3gpp_code_list.cp_code_data[list_code_cnt].gid2.gid_data[0] = *current_code_ptr;
    cp_code_data_ptr[list_cnt].code_data.cp_3gpp_code_list.cp_code_data[list_code_cnt].gid2.gid_len = 1;
    current_code_ptr += sizeof(uint8);

    cp_code_data_ptr[list_cnt].code_data.cp_3gpp_code_list.num_cp_codes++;
    list_code_cnt++;
  }

  if (current_code_ptr != code_data_buf_ptr + code_data_buf_len)
  {
    ret_value = QMI_ERR_MALFORMED_MSG;
  }

  return ret_value;
} /* qmi_uim_tlv_personalization_parse_cp_code */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_PERSONALIZATION_PARSE_SP_CODE()

  DESCRIPTION
    Parses the SP codes

  PARAMETERS
    num_codes           : number of SP codes
    code_data_buf_len   : length of raw code data
    code_data_buf_ptr   : pointer to raw code data
    num_lists           : number of lists needed for SP codes
    sp_code_data_ptr    : output with SP codes

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_personalization_parse_sp_code
(
  uint8                              num_codes,
  uint16                             code_data_buf_len,
  const uint8                    *   code_data_buf_ptr,
  uint8                              num_lists,
  simlock_category_data_type     *   sp_code_data_ptr
)
{
  qmi_error_e_type   ret_value         = QMI_ERR_NONE;
  uint8              code_cnt          = 0;
  uint8              list_cnt          = 0;
  uint8              list_code_cnt     = 0;
  uint8              mnc_len           = 0;
  const uint8      * current_code_ptr  = NULL;

  if ((code_data_buf_len == 0) ||
      (code_data_buf_ptr == NULL) ||
      (sp_code_data_ptr == NULL))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  current_code_ptr = code_data_buf_ptr;

  for(code_cnt = 0; code_cnt < num_codes; code_cnt++)
  {
    /* Check the data size */
    if(current_code_ptr + SIMLOCK_MCC_LEN + sizeof(mnc_len) >
        code_data_buf_ptr + code_data_buf_len)
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    /* Each list will fit, SIMLOCK_NUM_CODES_MAX. So if
       list_code_cnt is SIMLOCK_NUM_CODES_MAX, then fill
       the next list */
    if(list_code_cnt == SIMLOCK_NUM_CODES_MAX)
    {
      if(list_cnt < num_lists)
      {
        list_code_cnt = 0;
        list_cnt++;
      }
      else
      {
        ret_value = QMI_ERR_MALFORMED_MSG;
        break;
      }
    }

    if((list_cnt >= num_lists) ||
       (list_code_cnt >= SIMLOCK_NUM_CODES_MAX))
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    (void)memscpy((void*)sp_code_data_ptr[list_cnt].code_data.sp_3gpp_code_list.sp_code_data[list_code_cnt].nw_code.mcc,
                  SIMLOCK_MCC_LEN,
                  current_code_ptr,
                  SIMLOCK_MCC_LEN);
    current_code_ptr += SIMLOCK_MCC_LEN;

    mnc_len = *current_code_ptr;

    /* validate the mnc_len */
    if((mnc_len != QMI_UIM_SIMLOCK_MNC_MIN) &&
       (mnc_len != QMI_UIM_SIMLOCK_MNC_MAX))
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    sp_code_data_ptr[list_cnt].code_data.sp_3gpp_code_list.sp_code_data[list_code_cnt].nw_code.mnc.mnc_len = mnc_len;
    current_code_ptr += sizeof(mnc_len);

    if(current_code_ptr + mnc_len > code_data_buf_ptr + code_data_buf_len)
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    (void)memscpy((void*)sp_code_data_ptr[list_cnt].code_data.sp_3gpp_code_list.sp_code_data[list_code_cnt].nw_code.mnc.mnc_data,
            SIMLOCK_MNC_MAX,
            current_code_ptr,
            sp_code_data_ptr[list_cnt].code_data.sp_3gpp_code_list.sp_code_data[list_code_cnt].nw_code.mnc.mnc_len);
    current_code_ptr += mnc_len;

    /* Check the data size */
    if(current_code_ptr + sizeof(uint8) > code_data_buf_ptr + code_data_buf_len)
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    sp_code_data_ptr[list_cnt].code_data.sp_3gpp_code_list.sp_code_data[list_code_cnt].gid1.gid_data[0] = *current_code_ptr;
    sp_code_data_ptr[list_cnt].code_data.sp_3gpp_code_list.sp_code_data[list_code_cnt].gid1.gid_len = 1;
    current_code_ptr += sizeof(uint8);

    sp_code_data_ptr[list_cnt].code_data.sp_3gpp_code_list.num_sp_codes++;
    list_code_cnt++;
  }

  if (current_code_ptr != code_data_buf_ptr + code_data_buf_len)
  {
    ret_value = QMI_ERR_MALFORMED_MSG;
  }

  return ret_value;
} /* qmi_uim_tlv_personalization_parse_sp_code*/


/*===========================================================================
  FUNCTION QMI_UIM_TLV_PERSONALIZATION_PARSE_NS_CODE()

  DESCRIPTION
    Parses the NS codes

  PARAMETERS
    num_codes           : number of NS codes
    code_data_buf_len   : length of raw code data
    code_data_buf_ptr   : pointer to raw code data
    num_lists           : number of lists needed for NS codes
    nw_code_data_ptr    : output with NS codes

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_personalization_parse_ns_code
(
  uint8                              num_codes,
  uint16                             code_data_buf_len,
  const uint8                    *   code_data_buf_ptr,
  uint8                              num_lists,
  simlock_category_data_type     *   ns_code_data_ptr
)
{
  qmi_error_e_type   ret_value         = QMI_ERR_NONE;
  uint8              code_cnt          = 0;
  uint8              list_cnt          = 0;
  uint8              list_code_cnt     = 0;
  uint8              mnc_len           = 0;
  const uint8      * current_code_ptr  = NULL;

  if ((code_data_buf_len == 0) ||
      (code_data_buf_ptr == NULL) ||
      (ns_code_data_ptr == NULL))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  current_code_ptr = code_data_buf_ptr;

  for(code_cnt = 0; code_cnt < num_codes; code_cnt++)
  {
    /* Check the data size */
    if(current_code_ptr + SIMLOCK_MCC_LEN + sizeof(mnc_len) >
        code_data_buf_ptr + code_data_buf_len)
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    /* Each list will fit, SIMLOCK_NUM_CODES_MAX. So if
       list_code_cnt is SIMLOCK_NUM_CODES_MAX, then fill
       the next list */
    if(list_code_cnt == SIMLOCK_NUM_CODES_MAX)
    {
      if(list_cnt < num_lists)
      {
        list_code_cnt = 0;
        list_cnt++;
      }
      else
      {
        ret_value = QMI_ERR_MALFORMED_MSG;
        break;
      }
    }

    if((list_cnt >= num_lists) ||
       (list_code_cnt >= SIMLOCK_NUM_CODES_MAX))
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    (void)memscpy((void*)ns_code_data_ptr[list_cnt].code_data.ns_3gpp_code_list.ns_code_data[list_code_cnt].nw_code.mcc,
            SIMLOCK_MCC_LEN,
            current_code_ptr,
            SIMLOCK_MCC_LEN);
    current_code_ptr += SIMLOCK_MCC_LEN;

    mnc_len = *current_code_ptr;

    /* validate the mnc_len */
    if((mnc_len != QMI_UIM_SIMLOCK_MNC_MIN) &&
       (mnc_len != QMI_UIM_SIMLOCK_MNC_MAX))
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    ns_code_data_ptr[list_cnt].code_data.ns_3gpp_code_list.ns_code_data[list_code_cnt].nw_code.mnc.mnc_len = mnc_len;
    current_code_ptr += sizeof(mnc_len);

    if(current_code_ptr + mnc_len > code_data_buf_ptr + code_data_buf_len)
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    (void)memscpy((void*)ns_code_data_ptr[list_cnt].code_data.ns_3gpp_code_list.ns_code_data[list_code_cnt].nw_code.mnc.mnc_data,
            SIMLOCK_MNC_MAX,
            current_code_ptr,
            ns_code_data_ptr[list_cnt].code_data.ns_3gpp_code_list.ns_code_data[list_code_cnt].nw_code.mnc.mnc_len);
    current_code_ptr += mnc_len;

    if(current_code_ptr + sizeof(uint8) + sizeof(uint8) >
         code_data_buf_ptr + code_data_buf_len)
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    ns_code_data_ptr[list_cnt].code_data.ns_3gpp_code_list.ns_code_data[list_code_cnt].imsi_digit6 = *current_code_ptr;
    current_code_ptr += sizeof(uint8);

    ns_code_data_ptr[list_cnt].code_data.ns_3gpp_code_list.ns_code_data[list_code_cnt].imsi_digit7 = *current_code_ptr;
    current_code_ptr += sizeof(uint8);

    ns_code_data_ptr[list_cnt].code_data.ns_3gpp_code_list.num_ns_codes++;
    list_code_cnt++;
  }

  if (current_code_ptr != code_data_buf_ptr + code_data_buf_len)
  {
    ret_value = QMI_ERR_MALFORMED_MSG;
  }

  return ret_value;
} /* qmi_uim_tlv_personalization_parse_ns_code */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_PERSONALIZATION_PARSE_NW_CODE()

  DESCRIPTION
    Parses the NW codes

  PARAMETERS
    num_codes           : number of NW codes
    code_data_buf_len   : length of raw code data
    code_data_buf_ptr   : pointer to raw code data
    num_lists           : number of lists needed for NW codes
    nw_code_data_ptr    : output with NW codes

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_personalization_parse_nw_code
(
  uint8                              num_codes,
  uint16                             code_data_buf_len,
  const uint8                    *   code_data_buf_ptr,
  uint8                              num_lists,
  simlock_category_data_type     *   nw_code_data_ptr
)
{
  qmi_error_e_type   ret_value         = QMI_ERR_NONE;
  uint8              code_cnt          = 0;
  uint8              list_cnt          = 0;
  uint8              list_code_cnt     = 0;
  uint8              mnc_len           = 0;
  const uint8      * current_code_ptr  = NULL;

  if ((code_data_buf_len == 0) ||
      (code_data_buf_ptr == NULL) ||
      (nw_code_data_ptr == NULL))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  current_code_ptr = code_data_buf_ptr;

  for(code_cnt = 0; code_cnt < num_codes; code_cnt++)
  {
    /* Check the data size */
    if(current_code_ptr + SIMLOCK_MCC_LEN + sizeof(mnc_len) >
        code_data_buf_ptr + code_data_buf_len)
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    /* Each list will fit, SIMLOCK_NUM_CODES_MAX. So if
       list_code_cnt is SIMLOCK_NUM_CODES_MAX, then fill
       the next list */
    if(list_code_cnt == SIMLOCK_NUM_CODES_MAX)
    {
      if(list_cnt < num_lists)
      {
        list_code_cnt = 0;
        list_cnt++;
      }
      else
      {
        ret_value = QMI_ERR_MALFORMED_MSG;
        break;
      }
    }

    if((list_cnt >= num_lists) ||
       (list_code_cnt >= SIMLOCK_NUM_CODES_MAX))
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    (void)memscpy((void*)nw_code_data_ptr[list_cnt].code_data.nw_3gpp_code_list.nw_code_data[list_code_cnt].mcc,
                  SIMLOCK_MCC_LEN,
                  current_code_ptr,
                  SIMLOCK_MCC_LEN);
    current_code_ptr += SIMLOCK_MCC_LEN;

    mnc_len = *current_code_ptr;

    /* validate the mnc_len */
    if((mnc_len != QMI_UIM_SIMLOCK_MNC_MIN) &&
       (mnc_len != QMI_UIM_SIMLOCK_MNC_MAX))
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    nw_code_data_ptr[list_cnt].code_data.nw_3gpp_code_list.nw_code_data[list_code_cnt].mnc.mnc_len = mnc_len;
    current_code_ptr += sizeof(mnc_len);

    /* Check the data size */
    if(current_code_ptr + mnc_len > code_data_buf_ptr + code_data_buf_len)
    {
      ret_value = QMI_ERR_MALFORMED_MSG;
      break;
    }

    (void)memscpy((void*)nw_code_data_ptr[list_cnt].code_data.nw_3gpp_code_list.nw_code_data[list_code_cnt].mnc.mnc_data,
            SIMLOCK_MNC_MAX,
            current_code_ptr,
            nw_code_data_ptr[list_cnt].code_data.nw_3gpp_code_list.nw_code_data[list_code_cnt].mnc.mnc_len);
    current_code_ptr += mnc_len;

    nw_code_data_ptr[list_cnt].code_data.nw_3gpp_code_list.num_nw_codes++;
    list_code_cnt++;
  }

  if (current_code_ptr != code_data_buf_ptr + code_data_buf_len)
  {
    ret_value = QMI_ERR_MALFORMED_MSG;
  }

  return ret_value;
} /* qmi_uim_tlv_personalization_parse_nw_code */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_PERSONALIZATION_CK()

  DESCRIPTION
    Parses the CK TLV item of activate perso

  PARAMETERS
    sdu_in                : input sdu
    len                   : length of the TLV item
    personalization_ptr   : output with activate perso value

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_personalization_ck
(
  dsm_item_type **                        sdu_in,
  uint16                                  len,
  qmi_uim_tlv_personalization_ck_type   * personalization_ck_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint8  ck_len;
    uint8  ck_value[QMI_UIM_TLV_MAX_CK_LEN];
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(personalization_ck_ptr);

  /* Check length */
  if (len < sizeof(tlv_data.ck_len) || len > sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, len);
  if (temp != len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  if (tlv_data.ck_len == 0 ||
      tlv_data.ck_len > QMI_UIM_TLV_MAX_CK_LEN)
  {
    return QMI_ERR_ARG_TOO_LONG;
  }

  /* Check data length */
  if (len != sizeof(tlv_data.ck_len) + tlv_data.ck_len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Control key length */
  personalization_ck_ptr->ck.simlock_ck_length =
    tlv_data.ck_len;

  /* Control key value */
  (void)memscpy((void*)personalization_ck_ptr->ck.simlock_ck_data,
                sizeof(personalization_ck_ptr->ck.simlock_ck_data),
                (void*)tlv_data.ck_value,
                (uint32)personalization_ck_ptr->ck.simlock_ck_length);

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_personalization_ck */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_PERSONALIZATION_PARSE_CODES()

  DESCRIPTION
    Parses the personalization code

  PARAMETERS
    num_codes           : number of codes
    code_data_buf_len   : length of raw code data
    code_data_buf_ptr   : pointer to raw code data
    list_cnt            : number of lists needed for the codes
    code_data_ptr       : output with parsed codes

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_personalization_parse_codes
(
  uint8                              num_codes,
  uint16                             code_data_buf_len,
  const uint8                      * code_data_buf_ptr,
  uint8                              num_lists,
  simlock_category_data_type       * code_data_ptr
)
{
  qmi_error_e_type  ret_value         = QMI_ERR_NONE;

  if ((num_codes == 0) ||
      (code_data_buf_len == 0) ||
      (code_data_buf_ptr == NULL) ||
      (num_lists == 0) ||
      (code_data_ptr == NULL))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  switch(code_data_ptr->category_type)
  {
    case SIMLOCK_CATEGORY_3GPP_NW:
      ret_value = qmi_uim_tlv_personalization_parse_nw_code(num_codes,
                                                            code_data_buf_len,
                                                            code_data_buf_ptr,
                                                            num_lists,
                                                            code_data_ptr);
      break;

    case SIMLOCK_CATEGORY_3GPP_NS:
      ret_value = qmi_uim_tlv_personalization_parse_ns_code(num_codes,
                                                            code_data_buf_len,
                                                            code_data_buf_ptr,
                                                            num_lists,
                                                            code_data_ptr);
      break;

    case SIMLOCK_CATEGORY_3GPP_SP:
      ret_value = qmi_uim_tlv_personalization_parse_sp_code(num_codes,
                                                            code_data_buf_len,
                                                            code_data_buf_ptr,
                                                            num_lists,
                                                            code_data_ptr);
      break;

    case SIMLOCK_CATEGORY_3GPP_CP:
      ret_value = qmi_uim_tlv_personalization_parse_cp_code(num_codes,
                                                            code_data_buf_len,
                                                            code_data_buf_ptr,
                                                            num_lists,
                                                            code_data_ptr);
      break;

    case SIMLOCK_CATEGORY_3GPP_SIM:
      ret_value = qmi_uim_tlv_personalization_parse_sim_code(num_codes,
                                                             code_data_buf_len,
                                                             code_data_buf_ptr,
                                                             num_lists,
                                                             code_data_ptr);
      break;

    case SIMLOCK_CATEGORY_3GPP_SPN:
      ret_value = qmi_uim_tlv_personalization_parse_spn_code(num_codes,
                                                             code_data_buf_len,
                                                             code_data_buf_ptr,
                                                             num_lists,
                                                             code_data_ptr);
      break;

    case SIMLOCK_CATEGORY_3GPP_ICCID:
      ret_value = qmi_uim_tlv_personalization_parse_iccid_code(num_codes,
                                                               code_data_buf_len,
                                                               code_data_buf_ptr,
                                                               num_lists,
                                                               code_data_ptr);
      break;

    case SIMLOCK_CATEGORY_3GPP2_NW_TYPE1:
      ret_value = qmi_uim_tlv_personalization_parse_nw_type1_code(num_codes,
                                                                  code_data_buf_len,
                                                                  code_data_buf_ptr,
                                                                  num_lists,
                                                                  code_data_ptr);
      break;

    case SIMLOCK_CATEGORY_3GPP2_NW_TYPE2:
      ret_value = qmi_uim_tlv_personalization_parse_nw_type2_code(num_codes,
                                                                  code_data_buf_len,
                                                                  code_data_buf_ptr,
                                                                  num_lists,
                                                                  code_data_ptr);
      break;

    case SIMLOCK_CATEGORY_3GPP2_RUIM:
      ret_value = qmi_uim_tlv_personalization_parse_ruim_code(num_codes,
                                                              code_data_buf_len,
                                                              code_data_buf_ptr,
                                                              num_lists,
                                                              code_data_ptr);
      break;

    case SIMLOCK_CATEGORY_3GPP2_HRPD:
    case SIMLOCK_CATEGORY_3GPP2_SP:
    case SIMLOCK_CATEGORY_3GPP2_CP:
      ret_value = QMI_ERR_NOT_SUPPORTED;
      break;

    default:
      ret_value = QMI_ERR_INVALID_ARG;
      break;
  }

  return ret_value;
} /* qmi_uim_tlv_personalization_parse_codes */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_PERSONALIZATION_CODE()

  DESCRIPTION
    Parses the lock code TLV item of activate perso

  PARAMETERS
    sdu_in                : input sdu
    len                   : length of the TLV item
    personalization_ptr   : output with activate perso value

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_personalization_code
(
  dsm_item_type **                               sdu_in,
  uint16                                         len,
  simlock_category_enum_type                     category,
  qmi_uim_tlv_personalization_code_type        * personalization_code_ptr
)
{
  qmi_error_e_type  ret_value         = QMI_ERR_NONE;
  int32             temp              = 0;
  uint16            code_data_size    = 0;
  uint8             table_size        = 0;
  uint8             table_index       = 0;
  uint8             list_cnt          = 0;
  uint8           * code_data_ptr     = NULL;

  PACKED struct PACKED_POST
  {
    uint8  num_of_codes;
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(personalization_code_ptr);

  /* Check length */
  if (len < sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, sizeof(tlv_data));
  if (temp != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  table_size = sizeof(qmi_uim_simlock_category_table)/sizeof(qmi_uim_category_code_size_map_type);

  for (table_index = 0; table_index < table_size; table_index++)
  {
    if (category != qmi_uim_simlock_category_table[table_index].category)
    {
      continue;
    }

    /* Check the number of codes */
    if (tlv_data.num_of_codes == 0 ||
        tlv_data.num_of_codes > qmi_uim_simlock_category_table[table_index].num_codes_max)
    {
      UIM_MSG_ERR_1("QMI_UIM invalid number of codes: 0x%x", tlv_data.num_of_codes);
      return QMI_ERR_ARG_TOO_LONG;
    }

    /* Check data length */
    if( len < (sizeof(tlv_data) +
               (tlv_data.num_of_codes * qmi_uim_simlock_category_table[table_index].code_data_min)) )
    {
      return QMI_ERR_MALFORMED_MSG;
    }
    break;
  }

  if (table_index == table_size)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  code_data_size = len - sizeof(tlv_data);

  code_data_ptr = (uint8 *)uimqmi_malloc(code_data_size);
  if(code_data_ptr == NULL)
  {
    return QMI_ERR_NO_MEMORY;
  }

  /* Extract the code data */
  temp = dsm_pullup(sdu_in, (void*)code_data_ptr, code_data_size);
  if (temp != code_data_size)
  {
    uimqmi_free(code_data_ptr);
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Get the number of lists needed to fit all the codes */
  personalization_code_ptr->num_of_lists = QMI_UIM_PERSO_GET_LIST_COUNT(tlv_data.num_of_codes);
  if(personalization_code_ptr->num_of_lists == 0)
  {
    uimqmi_free(code_data_ptr);
    return QMI_ERR_MALFORMED_MSG;
  }

  personalization_code_ptr->code_ptr =
                (simlock_category_data_type *)uimqmi_malloc(personalization_code_ptr->num_of_lists *
                                                            sizeof(simlock_category_data_type));
  if(personalization_code_ptr->code_ptr == NULL)
  {
    uimqmi_free(code_data_ptr);
    return QMI_ERR_NO_MEMORY;
  }

  for(list_cnt = 0; list_cnt < personalization_code_ptr->num_of_lists;
      list_cnt++)
  {
    personalization_code_ptr->code_ptr[list_cnt].category_type = category;
  }

  ret_value = qmi_uim_tlv_personalization_parse_codes(tlv_data.num_of_codes,
                                                      code_data_size,
                                                      code_data_ptr,
                                                      personalization_code_ptr->num_of_lists,
                                                      personalization_code_ptr->code_ptr);

  uimqmi_free(code_data_ptr);
  return ret_value;
} /* qmi_uim_tlv_personalization_code */


/*===========================================================================
  FUNCTION QMI_UIM_DECRYPT_SLOT_TO_SIMLOCK_SLOT_ID()

  DESCRIPTION
    Converts decrypted slot to SimLock slot id.

  PARAMETERS
    decrypt_slot       : decrypt slot value
    simlock_slot_ptr   : output parameter containing SimLock slot id

  RETURN VALUE
    QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_decrypt_slot_to_simlock_slot_id
(
  uint8                       decrypt_slot,
  simlock_slot_enum_type    * simlock_slot_ptr
)
{
  ASSERT(simlock_slot_ptr);

  switch (decrypt_slot)
  {
    case 1:
      *simlock_slot_ptr = SIMLOCK_SLOT_1;
      break;
    case 2:
      *simlock_slot_ptr = SIMLOCK_SLOT_2;
      break;
    case 3:
      *simlock_slot_ptr = SIMLOCK_SLOT_3;
      break;
    default:
      *simlock_slot_ptr = SIMLOCK_SLOT_NUM;
      return QMI_ERR_INVALID_ARG;
  }

  return QMI_ERR_NONE;
} /* qmi_uim_decrypt_slot_to_simlock_slot_id */


/*===========================================================================
  FUNCTION QMI_UIM_PARSE_SECURE_MESSAGE_HEADER_DATA()

  DESCRIPTION
    This function parses the encrypted header information

  PARAMETERS
    data_len         : raw header length
    data_ptr         : raw header data
    header_data_ptr  : parsed header data

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_parse_secure_message_header_data
(
  uint32                                  data_len,
  const uint8                         *   data_ptr,
  qmi_uim_secure_header_type          *   header_data_ptr
)
{
  PACKED struct PACKED_POST
  {
    uint16 cmd_id;
    uint16 request_id;
    uint8  rfu[QMI_UIM_ENCRYPTED_MSG_RFU_SIZE];
  } raw_data;

  if((data_ptr == NULL) ||
     (header_data_ptr == NULL))
  {
     return QMI_ERR_INTERNAL;
  }

  if(data_len < sizeof(raw_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* copy the request header data to packed struct to be parsed */
  (void)memscpy(&raw_data,
                sizeof(raw_data),
                data_ptr,
                sizeof(raw_data));

  header_data_ptr->cmd_id = raw_data.cmd_id;
  header_data_ptr->request_id = raw_data.request_id;

  return QMI_ERR_NONE;
} /* qmi_uim_parse_secure_message_header_data */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_DECRYPT_REQUEST()

  DESCRIPTION
    Generic function that decrypts the encrypted request data

  PARAMETERS
    sdu_in                    : input sdu
    len                       : length of the TLV item
    decoded_data_len_ptr      : output decoded data len
    decoded_data_pptr         : output decoded data

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_decrypt_request
(
  dsm_item_type **         sdu_in,
  uint16                   len,
  uint32        *          decoded_data_len_ptr,
  uint8         **         decoded_data_pptr
)
{
  uint8                  * encrypted_data_ptr   = NULL;
  qmi_error_e_type         errval               = QMI_ERR_NONE;
  int32                    temp                 = 0;

  PACKED struct PACKED_POST
  {
    uint16  length;
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(decoded_data_pptr);
  ASSERT(decoded_data_len_ptr);

  /* Check length */
  if (len < sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data and ensure it is formed properly */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, sizeof(tlv_data));
  if (temp != sizeof(tlv_data.length))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  if (tlv_data.length == 0 ||
      len != (sizeof(tlv_data.length) + tlv_data.length))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  encrypted_data_ptr = (uint8 *)uimqmi_malloc(tlv_data.length);
  if(encrypted_data_ptr == NULL)
  {
    return QMI_ERR_NO_MEMORY;
  }

  /* Extract the code data */
  temp = dsm_pullup(sdu_in, (void*)encrypted_data_ptr, tlv_data.length);
  if (temp != tlv_data.length)
  {
    uimqmi_free(encrypted_data_ptr);
    return QMI_ERR_MALFORMED_MSG;
  }

  *decoded_data_len_ptr = tlv_data.length;

  /* caller is responsible to free this memory */
  *decoded_data_pptr = (uint8 *)uimqmi_malloc(tlv_data.length);
  if(*decoded_data_pptr == NULL)
  {
    uimqmi_free(encrypted_data_ptr);
    return QMI_ERR_NO_MEMORY;
  }

#ifndef FEATURE_QMI_UIM_SECURE_CHANNEL_TEST
  if (!qmi_uim_decrypt_payload(encrypted_data_ptr,
                               tlv_data.length,
                               *decoded_data_pptr,
                               decoded_data_len_ptr))
  {
    uimqmi_free(*decoded_data_pptr);
    *decoded_data_pptr = NULL;
    errval = QMI_ERR_AUTHENTICATION_FAILED;
  }
#else
  #error code not present
#endif /* FEATURE_QMI_UIM_SECURE_CHANNEL_TEST */

  uimqmi_free(encrypted_data_ptr);
  return errval;
} /* qmi_uim_tlv_decrypt_request */


/*===========================================================================
  FUNCTION QMI_UIM_CONVERT_CATEGORY_TO_SIMLOCK_CATEGORY()

  DESCRIPTION
    Converts the incoming category to SimLock category.

  PARAMETERS
    convert_category       : category value to be converted
    simlock_category_ptr   : output parameter containing SimLock category

  RETURN VALUE
    QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_convert_category_to_simlock_category
(
  uint8                         convert_category,
  simlock_category_enum_type  * simlock_category_ptr
)
{
  ASSERT(simlock_category_ptr);

  switch (convert_category)
  {
    case 0:
      *simlock_category_ptr = SIMLOCK_CATEGORY_3GPP_NW;
      break;
    case 1:
      *simlock_category_ptr = SIMLOCK_CATEGORY_3GPP_NS;
      break;
    case 2:
      *simlock_category_ptr = SIMLOCK_CATEGORY_3GPP_SP;
      break;
    case 3:
      *simlock_category_ptr = SIMLOCK_CATEGORY_3GPP_CP;
      break;
    case 4:
      *simlock_category_ptr = SIMLOCK_CATEGORY_3GPP_SIM;
      break;
    case 5:
      *simlock_category_ptr = SIMLOCK_CATEGORY_3GPP2_NW_TYPE1;
      break;
    case 6:
      *simlock_category_ptr = SIMLOCK_CATEGORY_3GPP2_NW_TYPE2;
      break;
    case 7:
    case 8:
    case 9:
      return QMI_ERR_NOT_SUPPORTED;
    case 10:
      *simlock_category_ptr = SIMLOCK_CATEGORY_3GPP2_RUIM;
      break;
    case 11:
      *simlock_category_ptr = SIMLOCK_CATEGORY_3GPP_SPN;
      break;
    case 13:
      *simlock_category_ptr = SIMLOCK_CATEGORY_3GPP_ICCID;
      break;
    default:
      return QMI_ERR_INVALID_ARG;
  }

  return QMI_ERR_NONE;
} /* qmi_uim_convert_category_to_simlock_category */


/*===========================================================================
  FUNCTION QMI_UIM_DECRYPT_OPERATION_TO_SIMLOCK_OPERATION()

  DESCRIPTION
    Converts decrypted operation to SimLock operation.

  PARAMETERS
    decrypt_operation       : decrypt operation value
    simlock_operation_ptr   : output parameter containing SimLock operation

  RETURN VALUE
    QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_decrypt_operation_to_simlock_operation
(
  uint8                                    decrypt_operation,
  qmi_uim_deperso_secure_operation_type  * simlock_operation_ptr
)
{
  ASSERT(simlock_operation_ptr);

  switch (decrypt_operation)
  {
    case 0:
      *simlock_operation_ptr = QMI_UIM_DEPERSO_UNLOCK_WITHOUT_CK;
      break;
    case 1:
      *simlock_operation_ptr = QMI_UIM_DEPERSO_TEMP_UNLOCK_ENABLE;
      break;
    case 2:
      *simlock_operation_ptr = QMI_UIM_DEPERSO_TEMP_UNLOCK_DISABLE;
      break;
    default:
      return QMI_ERR_INVALID_ARG;
  }

  return QMI_ERR_NONE;
} /* qmi_uim_decrypt_operation_to_simlock_operation */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_DECRYPTED_PERSO_SECURE()

  DESCRIPTION
    Parses the TLV item for encrypted perso request

  PARAMETERS
    sdu_in                    : input sdu
    len                       : length of the TLV item
    decoded_perso_ptr         : output decoded perso secure data

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_decrypted_perso_secure
(
  dsm_item_type **                           sdu_in,
  uint16                                     len,
  qmi_uim_tlv_decrypted_perso_secure_type *  decoded_perso_ptr
)
{
  uint8                               * decrypted_perso_data_ptr   = NULL;
  uint16                                decrypted_perso_data_len   = 0;
  qmi_error_e_type                      errval                     = QMI_ERR_NONE;
  uint32                                temp_perso_len             = 0;
  uint8                                 list_cnt                   = 0;
  simlock_category_enum_type            category                   = SIMLOCK_CATEGORY_3GPP_NW;

  ASSERT(sdu_in);
  ASSERT(decoded_perso_ptr);

  memset(decoded_perso_ptr, 0x00, sizeof(qmi_uim_tlv_decrypted_perso_secure_type));

  /* decrypt the request data */
  errval = qmi_uim_tlv_decrypt_request(sdu_in, len,
                                       &temp_perso_len,
                                       &decrypted_perso_data_ptr);
  if((errval != QMI_ERR_NONE) ||
     (decrypted_perso_data_ptr == NULL))
  {
    UIM_MSG_ERR_0("Failed to decrypt request data");
    return errval;
  }

  do
  {
    if (temp_perso_len > QMI_UIM_MAX_UINT16)
    {
      UIM_MSG_ERR_1("Invalid perso data length: 0x%x", temp_perso_len);
      errval = QMI_ERR_INTERNAL;
      break;
    }

    decrypted_perso_data_len = (uint16)temp_perso_len;

    /* Check the minimum perso length with 15 to incorporate 1x nw type2 perso */
    if(decrypted_perso_data_len < QMI_UIM_DECRYPTED_PERSO_SECURE_REQ_MIN_LEN)
    {
      UIM_MSG_ERR_1("QMI-UIM: decrypted data len: 0x%x, less than expected",
                    decrypted_perso_data_len);
      errval = QMI_ERR_MALFORMED_MSG;
      break;
    }

    /* parse the header data */
    errval = qmi_uim_parse_secure_message_header_data(decrypted_perso_data_len,
                                                      decrypted_perso_data_ptr,
                                                      &decoded_perso_ptr->header);
    if(errval != QMI_ERR_NONE)
    {
      break;
    }

    errval = qmi_uim_decrypt_slot_to_simlock_slot_id(
                  decrypted_perso_data_ptr[QMI_UIM_DECRYPT_PERSO_SECURE_SLOT_OFFSET],
                  &decoded_perso_ptr->slot);
    if(errval != QMI_ERR_NONE)
    {
      break;
    }

    /* Get the number of lists needed to fit all the codes */
    decoded_perso_ptr->num_of_lists =
                  QMI_UIM_PERSO_GET_LIST_COUNT(decrypted_perso_data_ptr[QMI_UIM_DECRYPT_PERSO_SECURE_NUM_CODES_OFFSET]);
    if(decoded_perso_ptr->num_of_lists == 0)
    {
      errval = QMI_ERR_MALFORMED_MSG;
      break;
    }

    decoded_perso_ptr->code_ptr =
      (simlock_category_data_type*)uimqmi_malloc(decoded_perso_ptr->num_of_lists * sizeof(simlock_category_data_type));
    if(decoded_perso_ptr->code_ptr == NULL)
    {
      errval = QMI_ERR_NO_MEMORY;
      break;
    }

    errval = qmi_uim_convert_category_to_simlock_category(
                  decrypted_perso_data_ptr[QMI_UIM_DECRYPT_PERSO_SECURE_CATEGORY_OFFSET],
                  &category);
    if(errval != QMI_ERR_NONE)
    {
      break;
    }

    for(list_cnt = 0; list_cnt < decoded_perso_ptr->num_of_lists;
        list_cnt++)
    {
      decoded_perso_ptr->code_ptr[list_cnt].category_type = category;
    }

    errval = qmi_uim_tlv_personalization_parse_codes(decrypted_perso_data_ptr[QMI_UIM_DECRYPT_PERSO_SECURE_NUM_CODES_OFFSET],
                                                     decrypted_perso_data_len - QMI_UIM_DECRYPT_PERSO_CODE_DATA_OFFSET,
                                                     &decrypted_perso_data_ptr[QMI_UIM_DECRYPT_PERSO_CODE_DATA_OFFSET],
                                                     decoded_perso_ptr->num_of_lists,
                                                     decoded_perso_ptr->code_ptr);
  }
  while (0);

  uimqmi_free(decrypted_perso_data_ptr);

  return errval;
} /* qmi_uim_tlv_decrypted_perso_secure */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_DECRYPTED_DEPERSO_SECURE()

  DESCRIPTION
    Parses the TLV item for encrypted deperso request

  PARAMETERS
    sdu_in                    : input sdu
    len                       : length of the TLV item
    decoded_perso_ptr         : output decoded perso secure data

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_decrypted_deperso_secure
(
  dsm_item_type **                             sdu_in,
  uint16                                       len,
  qmi_uim_tlv_decrypted_deperso_secure_type *  decoded_deperso_ptr
)
{
  uint8                               * decrypted_deperso_data_ptr   = NULL;
  uint32                                decrypted_deperso_data_len   = 0;
  qmi_error_e_type                      errval                       = QMI_ERR_NONE;

  ASSERT(sdu_in);
  ASSERT(decoded_deperso_ptr);

  memset(decoded_deperso_ptr, 0x00, sizeof(qmi_uim_tlv_decrypted_deperso_secure_type));

  /* decrypt the request data */
  errval = qmi_uim_tlv_decrypt_request(sdu_in, len,
                                       &decrypted_deperso_data_len,
                                       &decrypted_deperso_data_ptr);
  if((errval != QMI_ERR_NONE) ||
     (decrypted_deperso_data_ptr == NULL))
  {
    UIM_MSG_ERR_0("Failed to decrypt request data");
    return errval;
  }

  do
  {
    /* check the length */
    if(decrypted_deperso_data_len < QMI_UIM_DECRYPTED_DEPERSO_SECURE_REQ_LEN)
    {
      UIM_MSG_ERR_1("QMI-UIM: decrypted data len: 0x%x, less than expected",
                    decrypted_deperso_data_len);
      errval = QMI_ERR_MALFORMED_MSG;
      break;
    }

    /* parse the header data */
    errval = qmi_uim_parse_secure_message_header_data(decrypted_deperso_data_len,
                                                      decrypted_deperso_data_ptr,
                                                      &decoded_deperso_ptr->header);
    if(errval != QMI_ERR_NONE)
    {
      break;
    }

    errval = qmi_uim_decrypt_slot_to_simlock_slot_id(
                  decrypted_deperso_data_ptr[QMI_UIM_DECRYPT_DEPERSO_SECURE_SLOT_OFFSET],
                  &decoded_deperso_ptr->slot);
    if(errval != QMI_ERR_NONE)
    {
      break;
    }

    errval = qmi_uim_convert_category_to_simlock_category(
                  decrypted_deperso_data_ptr[QMI_UIM_DECRYPT_DEPERSO_SECURE_CATEGORY_OFFSET],
                  &decoded_deperso_ptr->category_type);
    if(errval != QMI_ERR_NONE)
    {
      break;
    }

    errval = qmi_uim_decrypt_operation_to_simlock_operation(
                  decrypted_deperso_data_ptr[QMI_UIM_DECRYPT_DEPERSO_SECURE_OPERATION_OFFSET],
                  &decoded_deperso_ptr->operation);
    if(errval != QMI_ERR_NONE)
    {
      break;
    }

    if (decoded_deperso_ptr->operation == QMI_UIM_DEPERSO_TEMP_UNLOCK_ENABLE)
    {
      (void)memscpy((void*)&decoded_deperso_ptr->duration,
                    sizeof(uint32),
                    (void*)&decrypted_deperso_data_ptr[QMI_UIM_DECRYPT_DEPERSO_SECURE_DURATION_OFFSET],
                    sizeof(uint32));
    }
  }
  while (0);

  uimqmi_free(decrypted_deperso_data_ptr);

  return errval;
} /* qmi_uim_tlv_decrypted_deperso_secure */


/*===========================================================================
  FUNCTION QMI_UIM_DECRYPT_DEVICE_MODE()

  DESCRIPTION
    Converts decrypted device mode to the SIMLock device mode enum type

  PARAMETERS
    decrypt_device_mode : decrypt slot value
    simlock_slot_ptr    : output parameter containing SimLock slot id

  RETURN VALUE
    QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_decrypt_device_mode
(
  uint8                              decrypt_device_mode,
  simlock_device_mode_enum_type    * device_mode_ptr
)
{
  ASSERT(device_mode_ptr);

  switch (decrypt_device_mode)
  {
    case 0:
      *device_mode_ptr = SIMLOCK_DEVICE_NORMAL_OPERATION_MODE;
      break;
    case 1:
      *device_mode_ptr = SIMLOCK_DEVICE_EMERGENCY_ONLY_MODE;
      break;
    default:
      return QMI_ERR_INVALID_ARG;
  }

  return QMI_ERR_NONE;
} /* qmi_uim_decrypt_device_mode */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_DECRYPTED_EMERGENCY_ONLY()

  DESCRIPTION
    Parses the TLV item for encrypted emergency only request

  PARAMETERS
    sdu_in                    : input sdu
    len                       : length of the TLV item
    decoded_perso_ptr         : output decoded perso secure data

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_decrypted_emergency_only
(
  dsm_item_type **                             sdu_in,
  uint16                                       len,
  qmi_uim_tlv_decrypted_emergency_only_type *  decoded_emergency_only_ptr
)
{
  uint8                  * decrypted_emergency_only_data_ptr   = NULL;
  uint32                   decrypted_emergency_only_data_len   = 0;
  qmi_error_e_type         errval                              = QMI_ERR_NONE;

  ASSERT(sdu_in);
  ASSERT(decoded_emergency_only_ptr);

  memset(decoded_emergency_only_ptr, 0x00, sizeof(qmi_uim_tlv_decrypted_emergency_only_type));

  /* decrypt the request data */
  errval = qmi_uim_tlv_decrypt_request(sdu_in, len,
                                       &decrypted_emergency_only_data_len,
                                       &decrypted_emergency_only_data_ptr);
  if((errval != QMI_ERR_NONE) ||
     (decrypted_emergency_only_data_ptr == NULL))
  {
    UIM_MSG_ERR_0("Failed to decrypt request data");
    return errval;
  }

  do
  {
    /* check the length */
    if(decrypted_emergency_only_data_len < QMI_UIM_DECRYPTED_EMERGENCY_ONLY_REQ_LEN)
    {
      UIM_MSG_ERR_1("QMI-UIM: decrypted data len: 0x%x, less than expected",
                    decrypted_emergency_only_data_len);
      errval = QMI_ERR_MALFORMED_MSG;
      break;
    }

    /* parse the header data */
    errval = qmi_uim_parse_secure_message_header_data(decrypted_emergency_only_data_len,
                                                      decrypted_emergency_only_data_ptr,
                                                      &decoded_emergency_only_ptr->header);
    if(errval != QMI_ERR_NONE)
    {
      break;
    }

    errval = qmi_uim_decrypt_device_mode(
                  decrypted_emergency_only_data_ptr[QMI_UIM_DECRYPT_EMERGENCY_ONLY_MODE_OFFSET],
                  &decoded_emergency_only_ptr->device_mode);
    if(errval != QMI_ERR_NONE)
    {
      break;
    }
  }
  while (0);

  uimqmi_free(decrypted_emergency_only_data_ptr);

  return errval;
} /* qmi_uim_tlv_decrypted_emergency_only */


/*===========================================================================
  FUNCTION QMI_UIM_CONVERT_SIMLOCK_FILE_TYPE_TO_FILE_ID

  DESCRIPTION
    Converts the SIMLock file type to a file id

  PARAMETERS
    file        : simlock file type
    file_id_ptr : converted file id

  RETURN VALUE
    errval          : QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_convert_simlock_file_type_to_file_id
(
  simlock_file_type       file,
  uint8                *  file_id_ptr
)
{
  ASSERT(file_id_ptr);

  if(file.file_type == SIMLOCK_FILE_TYPE_CONFIG)
  {
    *file_id_ptr = 0;
    return QMI_ERR_NONE;
  }

  if(file.file_type == SIMLOCK_FILE_TYPE_CATEGORY)
  {
    uint8          file_id_slot      = 1;

    /* simlock slot enums start with 0 (SIMLOCK_SLOT_1) but in
       file_id the slot starts with 1 */
    switch (file.data.category_data.slot)
    {
       case SIMLOCK_SLOT_1:
         file_id_slot = 1;
         break;

       case SIMLOCK_SLOT_2:
         file_id_slot = 2;
         break;

       case SIMLOCK_SLOT_3:
         file_id_slot = 3;
         break;

       case SIMLOCK_SLOT_4:
         file_id_slot = 4;
         break;

       default:
         return QMI_ERR_INTERNAL;
    }

    *file_id_ptr |= file_id_slot;
    *file_id_ptr <<= 4;
    *file_id_ptr |= (uint8)(file.data.category_data.category);
    return QMI_ERR_NONE;
  }

  return QMI_ERR_INTERNAL;
} /* qmi_uim_convert_simlock_file_type_to_file_id */


/*===========================================================================
  FUNCTION QMI_UIM_DECRYPT_SIMLOCK_CONFIG_OPERATION

  DESCRIPTION
    Converts decrypted operation to operation.

  PARAMETERS
    decrypt_operation       : decrypt operation value
    simlock_operation_ptr   : output parameter containing operation

  RETURN VALUE
    QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_decrypt_simlock_config_operation
(
  uint8                                    decrypt_operation,
  qmi_uim_simlock_config_operation_type  * simlock_operation_ptr
)
{
  ASSERT(simlock_operation_ptr);

  switch (decrypt_operation)
  {
    case 0:
      *simlock_operation_ptr = QMI_UIM_SIMLOCK_CONFIG_OPERATION_READ;
      break;
    case 1:
      *simlock_operation_ptr = QMI_UIM_SIMLOCK_CONFIG_OPERATION_WRITE;
      break;
    case 2:
      *simlock_operation_ptr = QMI_UIM_SIMLOCK_CONFIG_OPERATION_ENABLE_REMOTE_SFS;
      break;
    case 3:
      *simlock_operation_ptr = QMI_UIM_SIMLOCK_CONFIG_OPERATION_DISABLE_REMOTE_SFS;
      break;
    default:
      return QMI_ERR_INVALID_ARG;
  }

  return QMI_ERR_NONE;
} /* qmi_uim_decrypt_simlock_config_operation */


/*===========================================================================
  FUNCTION QMI_UIM_DECRYPT_SIMLOCK_CONFIG_RESULT

  DESCRIPTION
    Converts decrypted result to result.

  PARAMETERS
    decrypt_result       : decrypt result value
    simlock_result_ptr   : output parameter containing result

  RETURN VALUE
    QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_decrypt_simlock_config_result
(
  uint8                                    decrypt_result,
  simlock_result_enum_type               * simlock_result_ptr
)
{
  ASSERT(simlock_result_ptr);

  switch (decrypt_result)
  {
    case 0:
      *simlock_result_ptr = SIMLOCK_SUCCESS;
      break;
    case 1:
    case 2:
      *simlock_result_ptr = SIMLOCK_GENERIC_ERROR;
      break;
    default:
      return QMI_ERR_INVALID_ARG;
  }

  return QMI_ERR_NONE;
} /* qmi_uim_decrypt_simlock_config_result */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_DECRYPTED_SIMLOCK_CONFIG()

  DESCRIPTION
    Parses the TLV item for encrypted simlock config request

  PARAMETERS
    sdu_in                     : input sdu
    len                        : length of the TLV item
    decoded_simlock_config_ptr : output decoded simlock config data

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_decrypted_simlock_config
(
  dsm_item_type **                             sdu_in,
  uint16                                       len,
  qmi_uim_tlv_decrypted_simlock_config_type *  decoded_simlock_config_ptr
)
{
  uint8                               * decrypted_simlock_config_data_ptr  = NULL;
  uint32                                decrypted_simlock_config_data_len  = 0;
  qmi_error_e_type                      errval                             = QMI_ERR_NONE;

  ASSERT(sdu_in);
  ASSERT(decoded_simlock_config_ptr);

  memset(decoded_simlock_config_ptr, 0x00, sizeof(qmi_uim_tlv_decrypted_simlock_config_type));

  /* decrypt the request data */
  errval = qmi_uim_tlv_decrypt_request(sdu_in, len,
                                       &decrypted_simlock_config_data_len,
                                       &decrypted_simlock_config_data_ptr);
  if((errval != QMI_ERR_NONE) ||
     (decrypted_simlock_config_data_ptr == NULL))
  {
    UIM_MSG_ERR_0("Failed to decrypt request data");
    return errval;
  }

  do
  {
    /* check the length */
    if(decrypted_simlock_config_data_len < QMI_UIM_DECRYPTED_SIMLOCK_CONFIG_MIN_LEN)
    {
      errval = QMI_ERR_MALFORMED_MSG;
      break;
    }

    /* parse the header data */
    errval = qmi_uim_parse_secure_message_header_data(decrypted_simlock_config_data_len,
                                                      decrypted_simlock_config_data_ptr,
                                                      &decoded_simlock_config_ptr->header);
    if(errval != QMI_ERR_NONE)
    {
      break;
    }

    errval = qmi_uim_decrypt_simlock_config_operation(
                  decrypted_simlock_config_data_ptr[QMI_UIM_DECRYPT_SIMLOCK_CONFIG_OPERATION_OFFSET],
                  &decoded_simlock_config_ptr->operation);
    if(errval != QMI_ERR_NONE)
    {
      break;
    }

    if(decoded_simlock_config_ptr->operation == QMI_UIM_SIMLOCK_CONFIG_OPERATION_ENABLE_REMOTE_SFS)
    {
      decoded_simlock_config_ptr->operation_type.remote_sfs_operation =
               SIMLOCK_REMOTE_SFS_ENABLE;
    }

    if(decoded_simlock_config_ptr->operation == QMI_UIM_SIMLOCK_CONFIG_OPERATION_DISABLE_REMOTE_SFS)
    {
      decoded_simlock_config_ptr->operation_type.remote_sfs_operation =
               SIMLOCK_REMOTE_SFS_DISABLE;
    }

    if((decoded_simlock_config_ptr->operation == QMI_UIM_SIMLOCK_CONFIG_OPERATION_READ) ||
       (decoded_simlock_config_ptr->operation == QMI_UIM_SIMLOCK_CONFIG_OPERATION_WRITE))
    {
      /* check the length */
      if(decrypted_simlock_config_data_len < QMI_UIM_DECRYPTED_SIMLOCK_CONFIG_LEN)
      {
        errval = QMI_ERR_MALFORMED_MSG;
        break;
      }
    }

    if(decoded_simlock_config_ptr->operation == QMI_UIM_SIMLOCK_CONFIG_OPERATION_WRITE)
    {
      errval = qmi_uim_decrypt_simlock_config_result(
                  decrypted_simlock_config_data_ptr[QMI_UIM_DECRYPT_SIMLOCK_CONFIG_RESULT_OFFSET],
                  &decoded_simlock_config_ptr->operation_type.sfs_write_operation.result);
      if(errval != QMI_ERR_NONE)
      {
        break;
      }

      decoded_simlock_config_ptr->operation_type.sfs_write_operation.file_id =
              decrypted_simlock_config_data_ptr[QMI_UIM_DECRYPT_SIMLOCK_CONFIG_FILE_ID_OFFSET];
    }

    if (decoded_simlock_config_ptr->operation == QMI_UIM_SIMLOCK_CONFIG_OPERATION_READ)
    {
      uint16    temp_data_len         = 0;

      errval = qmi_uim_decrypt_simlock_config_result(
                  decrypted_simlock_config_data_ptr[QMI_UIM_DECRYPT_SIMLOCK_CONFIG_RESULT_OFFSET],
                  &decoded_simlock_config_ptr->operation_type.sfs_read_operation.result);
      if(errval != QMI_ERR_NONE)
      {
        break;
      }

      decoded_simlock_config_ptr->operation_type.sfs_read_operation.file_id =
              decrypted_simlock_config_data_ptr[QMI_UIM_DECRYPT_SIMLOCK_CONFIG_FILE_ID_OFFSET];

      memset(&decoded_simlock_config_ptr->operation_type.sfs_read_operation.data, 0x00, sizeof(simlock_data_type));

      /* in case of a failure, length and data might not be present */
      if(decoded_simlock_config_ptr->operation_type.sfs_read_operation.result != SIMLOCK_SUCCESS)
      {
        break;
      }

      /* check the length */
      if(decrypted_simlock_config_data_len < QMI_UIM_DECRYPTED_SIMLOCK_CONFIG_READ_LEN)
      {
        errval = QMI_ERR_MALFORMED_MSG;
        break;
      }

      /* copy the length */
      (void)memscpy((void*)&temp_data_len,
                    sizeof(uint16),
                    (void*)&decrypted_simlock_config_data_ptr[QMI_UIM_DECRYPT_SIMLOCK_CONFIG_READ_LEN_OFFSET],
                    sizeof(uint16));

      decoded_simlock_config_ptr->operation_type.sfs_read_operation.data.data_len = (uint32)temp_data_len;

      /* check if the data length is correct */
      if(decoded_simlock_config_ptr->operation_type.sfs_read_operation.data.data_len !=
                            (decrypted_simlock_config_data_len - QMI_UIM_DECRYPTED_SIMLOCK_CONFIG_READ_LEN))
      {
        UIM_MSG_ERR_1("QMI-UIM: invalid decrypted read data len: 0x%x",
                      decoded_simlock_config_ptr->operation_type.sfs_read_operation.data.data_len);
        errval = QMI_ERR_MALFORMED_MSG;
        break;
      }

      decoded_simlock_config_ptr->operation_type.sfs_read_operation.data.data_ptr =
            (uint8*)uimqmi_malloc(decoded_simlock_config_ptr->operation_type.sfs_read_operation.data.data_len);
      if(decoded_simlock_config_ptr->operation_type.sfs_read_operation.data.data_ptr == NULL)
      {
        errval = QMI_ERR_NO_MEMORY;
        break;
      }

      (void)memscpy((void*)decoded_simlock_config_ptr->operation_type.sfs_read_operation.data.data_ptr,
                    decoded_simlock_config_ptr->operation_type.sfs_read_operation.data.data_len,
                    (void*)&decrypted_simlock_config_data_ptr[QMI_UIM_DECRYPT_SIMLOCK_CONFIG_READ_DATA_OFFSET],
                    decoded_simlock_config_ptr->operation_type.sfs_read_operation.data.data_len);
    }
  }
  while (0);

  if (errval != QMI_ERR_NONE)
  {
    UIM_MSG_ERR_1("QMI-UIM: decrypted data len: 0x%x, less than expected",
                  decrypted_simlock_config_data_len);
  }

  uimqmi_free(decrypted_simlock_config_data_ptr);

  return errval;
} /* qmi_uim_tlv_decrypted_simlock_config */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_PERSONALIZATION_RELOCK_FEATURE()

  DESCRIPTION
    Parses the TLV item of reactivate perso feature

  PARAMETERS
    sdu_in            : input sdu
    len               : length of the TLV item
    reactivate_ptr    : output with perso feature to reactivate

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_personalization_relock_feature
(
  dsm_item_type **                              sdu_in,
  uint16                                        len,
  qmi_uim_tlv_personalization_reactivate_type * reactivate_ptr
)
{
  int32             temp   = 0;
  qmi_error_e_type  errval = QMI_ERR_NONE;

  PACKED struct PACKED_POST
  {
    uint8  feature;
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(reactivate_ptr);

  /* Check length */
  if (len != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, len);
  if (temp != len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Update category */
  errval = qmi_uim_convert_category_to_simlock_category(
                  tlv_data.feature,
                  &reactivate_ptr->category);
  if(errval != QMI_ERR_NONE)
  {
    return errval;
  }

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_personalization_relock_feature */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_NAF_ID()

  DESCRIPTION
    Parses the TLV item for NAF ID request

  PARAMETERS
    sdu_in                     : input sdu
    len                        : length of the TLV item
    gba_naf_id_ptr             : NAF ID data pointer

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_naf_id
(
  dsm_item_type **            sdu_in,
  uint16                      len,
  qmi_uim_tlv_naf_id_type *   gba_naf_id_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint8  fqdn_len;
  } tlv_fqdn_len_data;

  PACKED struct PACKED_POST
  {
    uint8  security_protocol_id[QMI_UIM_SECURITY_PROTOCOL_LEN];
  } tlv_sec_protocol_data;

  ASSERT(sdu_in);
  ASSERT(gba_naf_id_ptr);

  /* Check length for fqdn length */
  if (len < sizeof(tlv_fqdn_len_data) + sizeof(tlv_sec_protocol_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract fqdn length data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_fqdn_len_data, sizeof(tlv_fqdn_len_data));
  if (temp != sizeof(tlv_fqdn_len_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Check the total length. Check for fqdn_len is not required as the maximum
     value it can have is QMI_UIM_FQDN_MAX_V01(255) */
  if(len != sizeof(tlv_fqdn_len_data) +
            tlv_fqdn_len_data.fqdn_len +
            sizeof(tlv_sec_protocol_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  gba_naf_id_ptr->naf_id_ptr = (gba_naf_id_type *)uimqmi_malloc(sizeof(gba_naf_id_type));
  if (gba_naf_id_ptr->naf_id_ptr == NULL)
  {
    return QMI_ERR_NO_MEMORY;
  }

  /* fqdn length */
  gba_naf_id_ptr->naf_id_ptr->fqdn.data_len = tlv_fqdn_len_data.fqdn_len;

  if(tlv_fqdn_len_data.fqdn_len > 0)
  {
    /* Extract fqdn data */
    temp = dsm_pullup(sdu_in,
                      (void*)gba_naf_id_ptr->naf_id_ptr->fqdn.data,
                      tlv_fqdn_len_data.fqdn_len);
    if (temp != tlv_fqdn_len_data.fqdn_len)
    {
      return QMI_ERR_MALFORMED_MSG;
    }
  }

  /* Extract security protocol id */
  temp = dsm_pullup(sdu_in,
                    (void*)gba_naf_id_ptr->naf_id_ptr->security_protocol,
                    sizeof(tlv_sec_protocol_data));
  if (temp != sizeof(tlv_sec_protocol_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_naf_id */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_APN()

  DESCRIPTION
    Parses the TLV item for Access Point Name request

  PARAMETERS
    sdu_in                     : input sdu
    len                        : length of the TLV item
    apn_ptr                    : APN data pointer

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_apn
(
  dsm_item_type **        sdu_in,
  uint16                  len,
  qmi_uim_tlv_apn_type  * apn_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint8  apn_len;
  } tlv_apn_len_data;

  ASSERT(sdu_in);
  ASSERT(apn_ptr);

  /* Check length for APN length */
  if (len < sizeof(tlv_apn_len_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract APN length data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_apn_len_data, sizeof(tlv_apn_len_data));
  if (temp != sizeof(tlv_apn_len_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Check the total length */
  if(tlv_apn_len_data.apn_len > QMI_UIM_APN_NAME_MAX ||
     len != sizeof(tlv_apn_len_data) + tlv_apn_len_data.apn_len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  if(tlv_apn_len_data.apn_len > 0)
  {
    apn_ptr->apn_data_ptr = (char *)uimqmi_malloc(tlv_apn_len_data.apn_len + 1);
    if (apn_ptr->apn_data_ptr == NULL)
    {
      return QMI_ERR_NO_MEMORY;
    }

    /* Extract APN data */
    temp = dsm_pullup(sdu_in,
                      (void*)apn_ptr->apn_data_ptr,
                      tlv_apn_len_data.apn_len);
    if (temp != tlv_apn_len_data.apn_len)
    {
      return QMI_ERR_MALFORMED_MSG;
    }

    /* Terminate the APN with NULL */
    apn_ptr->apn_data_ptr[tlv_apn_len_data.apn_len] = '\0';
  }

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_apn */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_REMOTE_UNLOCK_CONFIG()

  DESCRIPTION
    Parses the TLV item of remote unlock request

  PARAMETERS
    sdu_in            : input sdu
    len               : length of the TLV item
    config_data_ptr   : output with simlock config data to set

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_remote_unlock_config
(
  dsm_item_type **                              sdu_in,
  uint16                                        len,
  qmi_uim_tlv_remote_unlock_config_type      *  config_data_ptr
)
{
  int32     temp      = 0;

  PACKED struct PACKED_POST
  {
    uint16  length;
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(config_data_ptr);

  /* Check length */
  if (len < sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data and ensure it is formed properly */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, sizeof(tlv_data));
  if (temp != sizeof(tlv_data.length))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  if (tlv_data.length == 0 ||
      len != (sizeof(tlv_data.length) + tlv_data.length))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  config_data_ptr->config_data.data_ptr = (uint8 *)uimqmi_malloc(tlv_data.length);
  if(config_data_ptr->config_data.data_ptr == NULL)
  {
    return QMI_ERR_NO_MEMORY;
  }

  /* Extract the config data */
  temp = dsm_pullup(sdu_in, (void*)config_data_ptr->config_data.data_ptr, tlv_data.length);
  if (temp != tlv_data.length)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  config_data_ptr->config_data.data_len = tlv_data.length;

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_remote_unlock_config */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_IMSI()

  DESCRIPTION
    Parses the TLV item of verify IMSI request

  PARAMETERS
    sdu_in            : input sdu
    len               : length of the TLV item
    imsi_data_ptr     : output with imsi data

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_imsi
(
  dsm_item_type **                              sdu_in,
  uint16                                        len,
  qmi_uim_tlv_imsi_type                      *  imsi_data_ptr
)
{
  int32     temp      = 0;

  PACKED struct PACKED_POST
  {
    uint8  length;
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(imsi_data_ptr);

  /* Check length */
  if (len < sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data and ensure it is formed properly */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, sizeof(tlv_data));
  if (temp != sizeof(tlv_data.length))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  if (tlv_data.length == 0 ||
      len != (sizeof(tlv_data.length) + tlv_data.length) ||
      tlv_data.length > sizeof(imsi_data_ptr->imsi_data.imsi))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract the config data */
  temp = dsm_pullup(sdu_in, (void*)imsi_data_ptr->imsi_data.imsi, tlv_data.length);
  if (temp != tlv_data.length)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  imsi_data_ptr->imsi_data.imsi_len = tlv_data.length;

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_imsi */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_PROFILE_ID()

  DESCRIPTION
    Parses the TLV item of set_sim_profile

  PARAMETERS
    sdu_in                   : input sdu
    len                      : length of the TLV item
    extended_card_status_ptr : output indicating if extended card status
                               is requested
  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_profile_id
(
  dsm_item_type **                   sdu_in,
  uint16                             len,
  qmi_uim_tlv_set_sim_profile_type  *set_sim_profile_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint32  profile_id;
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(set_sim_profile_ptr);

  /* Check length */
  if (len != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, len);
  if (temp != len)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extended card status flag */
  switch (tlv_data.profile_id)
  {
    case 0x01:
      set_sim_profile_ptr->profile_id = MMGSDI_PROFILE_1;
      break;
    case 0x02:
      set_sim_profile_ptr->profile_id = MMGSDI_PROFILE_2;
      break;
    case UIM_PROFILE_ID_3_V01:
      set_sim_profile_ptr->profile_id = MMGSDI_PROFILE_3;
      break;
    case UIM_PROFILE_ID_4_V01:
      set_sim_profile_ptr->profile_id = MMGSDI_PROFILE_4;
      break;
    case UIM_PROFILE_ID_5_V01:
      set_sim_profile_ptr->profile_id = MMGSDI_PROFILE_5;
      break;
    case UIM_PROFILE_ID_6_V01:
      set_sim_profile_ptr->profile_id = MMGSDI_PROFILE_6;
      break;
    case UIM_PROFILE_ID_7_V01:
      set_sim_profile_ptr->profile_id = MMGSDI_PROFILE_7;
      break;
    case UIM_PROFILE_ID_8_V01:
      set_sim_profile_ptr->profile_id = MMGSDI_PROFILE_8;
      break;
    default:
      return QMI_ERR_INVALID_ARG;
  }

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_profile_id */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_SEARCH_PATTERN()

  DESCRIPTION
    Parses the pattern TLV item of search_pattern

  PARAMETERS
    sdu_in                   : input sdu
    len                      : length of the TLV item
    search_pattern_ptr       : output with search pattern value

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_search_pattern
(
  dsm_item_type **                   sdu_in,
  uint16                             len,
  qmi_uim_tlv_search_pattern_type   *search_pattern_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint8  pattern_length;
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(search_pattern_ptr);

  /* Check length */
  if (len < sizeof(tlv_data))
  {
    UIM_MSG_ERR_1("QMI_UIM invalid len: 0x%x", len);
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, sizeof(tlv_data));
  if (temp != sizeof(tlv_data))
  {
    UIM_MSG_ERR_0("QMI_UIM DSM pullout error");
    return QMI_ERR_MALFORMED_MSG;
  }
  
  /* Check data length */
  if (len != sizeof(tlv_data) + tlv_data.pattern_length)
  {
    UIM_MSG_ERR_1("QMI_UIM invalid len: 0x%x", len);
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Length */
  search_pattern_ptr->pattern.data_len = (mmgsdi_len_type)tlv_data.pattern_length;

  /* Extract body to write. The memory allocated here needs to be freed by
     the caller, after using the data */
  search_pattern_ptr->pattern.data_ptr =
        (uint8*)uimqmi_malloc(tlv_data.pattern_length);
  if (search_pattern_ptr->pattern.data_ptr == NULL)
  {
    UIM_MSG_ERR_0("QMI_UIM memory alloc error");
    return QMI_ERR_NO_MEMORY;
  }

  temp = dsm_pullup(sdu_in,
                    (void*)search_pattern_ptr->pattern.data_ptr,
                    tlv_data.pattern_length);
  if (temp != tlv_data.pattern_length)
  {
    UIM_MSG_ERR_0("QMI_UIM DSM pullout error");
    uimqmi_free(search_pattern_ptr->pattern.data_ptr);
    return QMI_ERR_MALFORMED_MSG;
  }

  return QMI_ERR_NONE;
}/* qmi_uim_tlv_search_pattern */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_SEARCH_DATA()

  DESCRIPTION
    Parses the Search Data TLV item

  PARAMETERS
    sdu_in                   : input sdu
    len                      : length of the TLV item
    search_data_ptr          : output with direction and first record value

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_search_data
(
  dsm_item_type **                   sdu_in,
  uint16                             len,
  qmi_uim_tlv_search_data_type      *search_data_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint32 search_type;
    uint8  first_record;
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(search_data_ptr);

  /* Check length */
  if (len != sizeof(tlv_data))
  {
    UIM_MSG_ERR_1("QMI_UIM invalid len: 0x%x", len);
    return QMI_ERR_MALFORMED_MSG;
  }
  
  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, len);
  if (temp != len)
  {
    UIM_MSG_ERR_0("QMI_UIM DSM pullout error");
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Record */
  search_data_ptr->first_record_num = (mmgsdi_rec_num_type)tlv_data.first_record;

  /* Search Type */
  switch (tlv_data.search_type)
  {
    case 0:
      search_data_ptr->direction = MMGSDI_SEARCH_FORWARD_FROM_REC_NUM;
      break;
    case 1:
      search_data_ptr->direction = MMGSDI_SEARCH_BACKWARD_FROM_REC_NUM;
      break;
    default:
      UIM_MSG_ERR_1("QMI_UIM invalid search type: 0x%x", tlv_data.search_type);
      return QMI_ERR_INVALID_ARG;
  }

  return QMI_ERR_NONE;
}/* qmi_uim_tlv_search_data */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_EUICC_PROFILE_NAME()

  DESCRIPTION
    Parses the TLV item of profile name or nickname

  PARAMETERS
    sdu_in                      : input sdu
    len                         : length of the TLV item
    euicc_profile_name_data_ptr : output with profile name

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_euicc_profile_name
(
  dsm_item_type **                           sdu_in,
  uint16                                     len,
  qmi_uim_tlv_euicc_name_type              * euicc_profile_name_data_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint8 length;
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(euicc_profile_name_data_ptr);

  /* Check length */
  if (len < sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, sizeof(tlv_data));
  if (temp != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Length */
  euicc_profile_name_data_ptr->euicc_name_data.name_len = tlv_data.length;

  /* Check data length */
  if (len != sizeof(tlv_data) + tlv_data.length)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  if (tlv_data.length > LPA_NAME_LEN_MAX)
  {
    return QMI_ERR_ARG_TOO_LONG;
  }

  /* Extract body to write. The memory allocated here needs to be freed by
     the caller, after using the data */
  if (tlv_data.length > 0)
  {
    temp = dsm_pullup(sdu_in,
                      (void*)euicc_profile_name_data_ptr->euicc_name_data.name,
                      tlv_data.length);
    if (temp != tlv_data.length)
    {
      return QMI_ERR_MALFORMED_MSG;
    }
  }

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_euicc_profile_name */


/*===========================================================================
  FUNCTION QMI_UIM_TLV_EUICC_DOWNLOAD_CODE()

  DESCRIPTION
    Parses the TLV item of add profile activation or confirmation code

  PARAMETERS
    sdu_in                      : input sdu
    len                         : length of the TLV item
    temporary_identity_mask_ptr : output with temporary identity mask value

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_tlv_euicc_download_code
(
  dsm_item_type **                           sdu_in,
  uint16                                     len,
  qmi_uim_tlv_download_code_type           * euicc_download_code_ptr
)
{
  int32             temp;

  PACKED struct PACKED_POST
  {
    uint16 length;
  } tlv_data;

  ASSERT(sdu_in);
  ASSERT(euicc_download_code_ptr);

  /* Check length */
  if (len < sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Extract data */
  temp = dsm_pullup(sdu_in, (void*)&tlv_data, sizeof(tlv_data));
  if (temp != sizeof(tlv_data))
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Length */
  euicc_download_code_ptr->code.data_len = tlv_data.length;

  /* Check data length */
  if (len != sizeof(tlv_data) + tlv_data.length)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  if (tlv_data.length > QMI_UIM_EUICC_DOWNLOAD_CODE_MAX_V01)
  {
    return QMI_ERR_ARG_TOO_LONG;
  }

  /* Extract body to write. The memory allocated here needs to be freed by
     the caller, after using the data */
  if (tlv_data.length > 0)
  {
    euicc_download_code_ptr->code.data_ptr =
          (uint8*)uimqmi_malloc(tlv_data.length);
    if (euicc_download_code_ptr->code.data_ptr == NULL)
    {
      return QMI_ERR_NO_MEMORY;
    }

    temp = dsm_pullup(sdu_in,
                      (void*)euicc_download_code_ptr->code.data_ptr,
                      tlv_data.length);
    if (temp != tlv_data.length)
    {
      return QMI_ERR_MALFORMED_MSG;
    }
  }

  return QMI_ERR_NONE;
} /* qmi_uim_tlv_euicc_download_code */


/*===========================================================================
  FUNCTION QMI_UIM_PARSE_REQUEST()

  DESCRIPTION
    Parses the TLV content of QMI_UIM message

  PARAMETERS
    sdu_in          : input sdu
    parsed_data_ptr : output with parsed values
    count           : number of values to be parsed

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_uim_parse_request
(
  dsm_item_type **        sdu_in,
  qmi_uim_tlv_item_type * parsed_data_ptr,
  uint16                  count
)
{
  uint16            index  = 0;
  qmi_error_e_type  result = QMI_ERR_NONE;

  ASSERT(sdu_in);
  ASSERT(parsed_data_ptr);
  ASSERT(count > 0);

  while (*sdu_in)
  {
    uint8            type             = 0;
    uint16           len              = 0;
    boolean          parsing_required = FALSE;

    if( !qmi_svc_get_tl( sdu_in, &type, &len ) )
    {
      continue;
    }

    /* Check if tag is required (mandatory or optional) */
    for (index = 0; index < count; index++)
    {
      if (parsed_data_ptr[index].tag == type &&
          parsed_data_ptr[index].present == FALSE)
      {
        parsing_required = TRUE;
        break;
      }
    }

    if (!parsing_required)
    {
      /* We need to pullout this item */
      if (len != dsm_pullup(sdu_in, NULL, len))
      {
        result = QMI_ERR_MALFORMED_MSG;
        break;
      }
      continue;
    }

    switch(parsed_data_ptr[index].type)
    {
      case UIMI_TLV_TYPE_SESSION_INFORMATION:
        result = qmi_uim_tlv_session_information(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.session_information));
        break;

      case UIMI_TLV_TYPE_FILE_ID:
        result = qmi_uim_tlv_file_id(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.file_id));
        break;

      case UIMI_TLV_TYPE_READ_TRANSPARENT:
        result = qmi_uim_tlv_read_transparent(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.read_transparent));
        break;

      case UIMI_TLV_TYPE_READ_RECORD:
        result = qmi_uim_tlv_read_record(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.read_record));
        break;

      case UIMI_TLV_TYPE_WRITE_TRANSPARENT:
        result = qmi_uim_tlv_write_transparent(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.write_transparent));
        break;

      case UIMI_TLV_TYPE_WRITE_RECORD:
        result = qmi_uim_tlv_write_record(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.write_record));
        break;

      case UIMI_TLV_TYPE_REGISTER_REFRESH:
        result = qmi_uim_tlv_register_refresh(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.register_refresh));
        break;

      case UIMI_TLV_TYPE_OK_FOR_REFRESH:
        result = qmi_uim_tlv_boolean(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.ok_to_refresh));
        break;

      case UIMI_TLV_TYPE_REFRESH_COMPLETE:
        result = qmi_uim_tlv_boolean(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.refresh_complete));
        break;

      case UIMI_TLV_TYPE_SET_PIN_PROTECTION:
        result = qmi_uim_tlv_set_pin_protection(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.set_pin_protection));
        break;

      case UIMI_TLV_TYPE_VERIFY_PIN:
        result = qmi_uim_tlv_verify_pin(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.verify_pin));
        break;

      case UIMI_TLV_TYPE_UNBLOCK_PIN:
        result = qmi_uim_tlv_unblock_pin(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.unblock_pin));
        break;

      case UIMI_TLV_TYPE_CHANGE_PIN:
        result = qmi_uim_tlv_change_pin(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.change_pin));
        break;

      case UIMI_TLV_TYPE_DEPERSONALIZATION:
        result = qmi_uim_tlv_depersonalization(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.depersonalization));
        break;

      case UIMI_TLV_TYPE_SLOT:
        result = qmi_uim_tlv_slot(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.slot));
        break;

      case UIMI_TLV_TYPE_EVENT_REG:
        result = qmi_uim_tlv_event_reg(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.event_reg));
        break;

      case UIMI_TLV_TYPE_AUTHENTICATE:
        result = qmi_uim_tlv_authenticate(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.authenticate));
        break;

      case UIMI_TLV_TYPE_CAPABILITIES_MASK:
        result = qmi_uim_tlv_capabilities_mask(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.capabilities_mask));
        break;

      case UIMI_TLV_TYPE_SET_FDN_STATUS:
        result = qmi_uim_tlv_boolean(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.set_fdn_status));
        break;

      case UIMI_TLV_TYPE_APPLICATION_INFORMATION:
        result = qmi_uim_tlv_application_information(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.application_information));
        break;

      case UIMI_TLV_TYPE_SESSION_CHANGE:
        result = qmi_uim_tlv_session_change(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.session_change));
        break;

      case UIMI_TLV_TYPE_APDU_REQUEST:
        result = qmi_uim_tlv_apdu_request(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.send_apdu));
        break;

      case UIMI_TLV_TYPE_SAP_CONNECT:
        result = qmi_uim_tlv_sap_connect(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.sap_connection));
        break;

      case UIMI_TLV_TYPE_SAP_DISCONNECT_MODE:
        result = qmi_uim_tlv_sap_disconnect_mode(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.sap_disconnect_mode));
        break;

      case UIMI_TLV_TYPE_SAP_REQUEST:
        result = qmi_uim_tlv_sap_request(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.sap_request));
        break;

      case UIMI_TLV_TYPE_LOGICAL_CHANNEL:
        result = qmi_uim_tlv_channel_id(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.logical_channel));
        break;

      case UIMI_TLV_TYPE_AID:
        result = qmi_uim_tlv_aid(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.aid));
        break;

      case UIMI_TLV_TYPE_DECRYPTED_PIN:
        result = qmi_uim_tlv_decrypted_pin(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.decrypted_pin));
        break;

      case UIMI_TLV_TYPE_READ_RECORD_LAST:
        result = qmi_uim_tlv_read_record_last(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.read_record_last));
        break;

      case UIMI_TLV_TYPE_SUBSCRIPTION_OK:
        result = qmi_uim_tlv_boolean(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.ok_for_subscription));
        break;

      case UIMI_TLV_TYPE_FILE_CONTROL_INFO:
        result = qmi_uim_tlv_file_control_info(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.file_control_info));
        break;

      case UIMI_TLV_TYPE_PIN_KEYREF_ID:
        result = qmi_uim_tlv_pin_keyref_id(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.pin_keyref_id));
        break;

      case UIMI_TLV_TYPE_IND_TOKEN:
        result = qmi_uim_tlv_uint32(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.ind_token));
        break;

      case UIMI_TLV_TYPE_IGNORE_HOTSWAP_SWITCH:
        result = qmi_uim_tlv_boolean(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.ignore_hotswap_switch));
        break;

      case UIMI_TLV_TYPE_SAP_INTERMEDIATE_GET_RESP:
        result = qmi_uim_tlv_boolean(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.sap_intermediate_get_resp));
        break;

      case UIMI_TLV_TYPE_ENCRYPTED:
        result = qmi_uim_tlv_boolean(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.encrypted));
        break;

      case UIMI_TLV_TYPE_DECRYPTED_IMSI:
        result = qmi_uim_tlv_decrypted_imsi(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.decrypted_imsi));
        break;

      case UIMI_TLV_TYPE_CONFIGURATION_MASK:
        result = qmi_uim_tlv_get_config_mask(
                   sdu_in,
                   len,
                   &(parsed_data_ptr[index].data.configuration_mask));
        break;

      case UIMI_TLV_TYPE_SKIP_PROCEDURE_BYTES:
        result = qmi_uim_tlv_boolean(
                   sdu_in,
                   len,
                   &(parsed_data_ptr[index].data.skip_procedure_bytes));
        break;

      case UIMI_TLV_TYPE_TERMINATE_APP:
        result = qmi_uim_tlv_boolean(
                   sdu_in,
                   len,
                   &(parsed_data_ptr[index].data.terminate_app));
        break;

      case UIMI_TLV_TYPE_REGISTER_FOR_REFRESH:
        result = qmi_uim_tlv_boolean(
                   sdu_in,
                   len,
                   &(parsed_data_ptr[index].data.refresh_registration));
        break;

      case UIMI_TLV_TYPE_REFRESH_VOTE_FOR_INIT:
        result = qmi_uim_tlv_boolean(
                   sdu_in,
                   len,
                   &(parsed_data_ptr[index].data.refresh_vote_for_init));
        break;

      case UIMI_TLV_TYPE_SAP_CONNECTION_CONDITION:
        result = qmi_uim_tlv_sap_connection_condition(
                   sdu_in,
                   len,
                   &(parsed_data_ptr[index].data.sap_connection_condtion));
        break;

      case UIMI_TLV_TYPE_SET_ACL_STATUS:
        result = qmi_uim_tlv_boolean(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.set_acl_status));
        break;

      case UIMI_TLV_TYPE_FILE_STATUS:
        result = qmi_uim_tlv_file_status(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.file_status));
        break;

      case UIMI_TLV_TYPE_EXTENDED_CARD_STATUS:
        result = qmi_uim_tlv_boolean(
                   sdu_in,
                   len,
                   &(parsed_data_ptr[index].data.extended_card_status));
        break;

      case UIMI_TLV_TYPE_PHYSICAL_SLOT:
        result = qmi_uim_tlv_physical_slot(
                   sdu_in,
                   len,
                   &(parsed_data_ptr[index].data.physical_slot));
        break;

      case UIMI_TLV_TYPE_INCREASE:
        result = qmi_uim_tlv_increase(
                   sdu_in,
                   len,
                   &(parsed_data_ptr[index].data.increase_data));
        break;

      case UIMI_TLV_TYPE_SELECT_MODE:
        result = qmi_uim_tlv_select_mode(
                   sdu_in,
                   len,
                   &(parsed_data_ptr[index].data.select_mode));
        break;

      case UIMI_TLV_TYPE_STATUS_CMD_MODE:
        result = qmi_uim_tlv_status_cmd_mode(
                   sdu_in,
                   len,
                   &(parsed_data_ptr[index].data.status_cmd_mode));
        break;

      case UIMI_TLV_TYPE_STATUS_CMD_RESP:
        result = qmi_uim_tlv_status_cmd_resp(
                   sdu_in,
                   len,
                   &(parsed_data_ptr[index].data.status_cmd_resp));
        break;

      case UIMI_TLV_TYPE_SERVICE_NUMBER:
        result = qmi_uim_tlv_service_number(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.service_number));
        break;

      case UIMI_TLV_TYPE_PERSONALIZATION:
        result = qmi_uim_tlv_personalization_ck(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.personalization_ck));
        break;

      case UIMI_TLV_TYPE_GW_NW_PERSO_CODE:
        result = qmi_uim_tlv_personalization_code(
                    sdu_in,
                    len,
                    SIMLOCK_CATEGORY_3GPP_NW,
                    &(parsed_data_ptr[index].data.personalization_code));
        break;

      case UIMI_TLV_TYPE_GW_NS_PERSO_CODE:
        result = qmi_uim_tlv_personalization_code(
                    sdu_in,
                    len,
                    SIMLOCK_CATEGORY_3GPP_NS,
                    &(parsed_data_ptr[index].data.personalization_code));
        break;

      case UIMI_TLV_TYPE_GW_SP_PERSO_CODE:
        result = qmi_uim_tlv_personalization_code(
                    sdu_in,
                    len,
                    SIMLOCK_CATEGORY_3GPP_SP,
                    &(parsed_data_ptr[index].data.personalization_code));
        break;

      case UIMI_TLV_TYPE_GW_CP_PERSO_CODE:
        result = qmi_uim_tlv_personalization_code(
                    sdu_in,
                    len,
                    SIMLOCK_CATEGORY_3GPP_CP,
                    &(parsed_data_ptr[index].data.personalization_code));
        break;

      case UIMI_TLV_TYPE_GW_SIM_PERSO_CODE:
        result = qmi_uim_tlv_personalization_code(
                    sdu_in,
                    len,
                    SIMLOCK_CATEGORY_3GPP_SIM,
                    &(parsed_data_ptr[index].data.personalization_code));
        break;

      case UIMI_TLV_TYPE_GW_SPN_PERSO_CODE:
        result = qmi_uim_tlv_personalization_code(
                    sdu_in,
                    len,
                    SIMLOCK_CATEGORY_3GPP_SPN,
                    &(parsed_data_ptr[index].data.personalization_code));
        break;

      case UIMI_TLV_TYPE_GW_ICCID_PERSO_CODE:
        result = qmi_uim_tlv_personalization_code(
                    sdu_in,
                    len,
                    SIMLOCK_CATEGORY_3GPP_ICCID,
                    &(parsed_data_ptr[index].data.personalization_code));
        break;

      case UIMI_TLV_TYPE_1X_NW_TYPE1_PERSO_CODE:
        result = qmi_uim_tlv_personalization_code(
                    sdu_in,
                    len,
                    SIMLOCK_CATEGORY_3GPP2_NW_TYPE1,
                    &(parsed_data_ptr[index].data.personalization_code));
        break;

      case UIMI_TLV_TYPE_1X_NW_TYPE2_PERSO_CODE:
        result = qmi_uim_tlv_personalization_code(
                    sdu_in,
                    len,
                    SIMLOCK_CATEGORY_3GPP2_NW_TYPE2,
                    &(parsed_data_ptr[index].data.personalization_code));
        break;

      case UIMI_TLV_TYPE_1X_RUIM_PERSO_CODE:
        result = qmi_uim_tlv_personalization_code(
                    sdu_in,
                    len,
                    SIMLOCK_CATEGORY_3GPP2_RUIM,
                    &(parsed_data_ptr[index].data.personalization_code));
        break;

      case UIMI_TLV_TYPE_PERSO_ITERATION_CNT:
        result = qmi_uim_tlv_uint32(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.personalization_iteration_cnt));
        break;

      case UIMI_TLV_TYPE_PERSO_UNLOCK_RETRIES_CNT:
        result = qmi_uim_tlv_uint32(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.personalization_retries_cnt));
        break;

      case UIMI_TLV_TYPE_DECRYPTED_PERSO_SECURE:
        result = qmi_uim_tlv_decrypted_perso_secure(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.decrypted_perso_secure));
        break;

      case UIMI_TLV_TYPE_DECRYPTED_DEPERSO_SECURE:
        result = qmi_uim_tlv_decrypted_deperso_secure(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.decrypted_deperso_secure));
        break;

      case UIMI_TLV_TYPE_ENCRYPTED_EXT:
        result = qmi_uim_tlv_boolean(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.encrypted_ext));
        break;

      case UIMI_TLV_TYPE_DECRYPTED_EMERGENCY_ONLY:
        result = qmi_uim_tlv_decrypted_emergency_only(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.decrypted_emergency_only));
        break;

      case UIMI_TLV_TYPE_DECRYPTED_SIMLOCK_CONFIG:
         result = qmi_uim_tlv_decrypted_simlock_config(
                     sdu_in,
                     len,
                     &(parsed_data_ptr[index].data.decrypted_simlock_config));
         break;

      case UIMI_TLV_TYPE_PERSO_REACTIVATE_FEATURE:
        result = qmi_uim_tlv_personalization_relock_feature(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.personalization_reactivate));
        break;

      case UIMI_TLV_TYPE_NAF_ID:
        result = qmi_uim_tlv_naf_id(
                   sdu_in,
                   len,
                   &(parsed_data_ptr[index].data.gba_naf_id));
        break;

      case UIMI_TLV_TYPE_FORCE_BOOTSTRAPPING:
        result = qmi_uim_tlv_boolean(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.gba_force_bootstrapping));
        break;

      case UIMI_TLV_TYPE_ACCESS_POINT_NAME:
        result = qmi_uim_tlv_apn(
                   sdu_in,
                   len,
                   &(parsed_data_ptr[index].data.apn));
        break;

      case UIMI_TLV_TYPE_REMOTE_UNLOCK_CONFIG:
        result = qmi_uim_tlv_remote_unlock_config(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.remote_unlock_config));
         break;

      case UIMI_TLV_TYPE_IMSI:
         result = qmi_uim_tlv_imsi(
                    sdu_in,
                    len,
                    &(parsed_data_ptr[index].data.imsi));
         break;

      case UIMI_TLV_TYPE_PROFILE_ID:
        result = qmi_uim_tlv_profile_id(
                   sdu_in,
                   len,
                   &(parsed_data_ptr[index].data.sim_profile));
        break;

      case UIMI_TLV_TYPE_SEARCH_PATTERN:
        result = qmi_uim_tlv_search_pattern(
                   sdu_in,
                   len,
                   &(parsed_data_ptr[index].data.search_pattern));
        break;

      case UIMI_TLV_TYPE_SEARCH_DATA:
        result = qmi_uim_tlv_search_data(
                   sdu_in,
                   len,
                   &(parsed_data_ptr[index].data.search_data));
        break;

      case UIMI_TLV_TYPE_EUICC_PROFILE_NAME:
        result = qmi_uim_tlv_euicc_profile_name(
                   sdu_in,
                   len,
                   &(parsed_data_ptr[index].data.euicc_name));
        break;

      case UIMI_TLV_TYPE_SET_UNSET_PROFILE:
        result = qmi_uim_tlv_boolean(
                   sdu_in,
                   len,
                   &(parsed_data_ptr[index].data.set_unset_profile));
        break;

      case UIMI_TLV_TYPE_EUICC_DOWNLOAD_CODE:
      case UIMI_TLV_TYPE_EUICC_CONFIRMATION_CODE:
        result = qmi_uim_tlv_euicc_download_code(
                   sdu_in,
                   len,
                   &(parsed_data_ptr[index].data.euicc_download_code));
        break;

      default:
        /* This should not happen because unwanted TLV types were already
           detected above, when checked if TLV was mandatory or optional */
        result = QMI_ERR_MALFORMED_MSG;
        break;
    }

    if (result != QMI_ERR_NONE)
    {
      UIM_MSG_ERR_2("Error 0x%x parsing TLV type 0x%x", result, type);
      break;
    }

    parsed_data_ptr[index].present = TRUE;
  }

  /* In case of success, check that all mandatory TLV were received */
  if (result == QMI_ERR_NONE)
  {
    for (index = 0; index < count; index++)
    {
      boolean mandatory = FALSE;

      /* Check if each TLV is mandatory from the tag */
      if (parsed_data_ptr[index].tag >= UIMI_TLV_TAG_MANDATORY_1 &&
          parsed_data_ptr[index].tag < UIMI_TLV_TAG_OPTIONAL_1)
      {
        mandatory = TRUE;
      }

      if (mandatory &&
          !parsed_data_ptr[index].present)
      {
        result = QMI_ERR_MISSING_ARG;
        break;
      }
    }
  }

  /* In case of error, clean the TLV data, as it might contain
     sensitive information, such as PIN values, or allocated
     memory, such as write content */
  if (result != QMI_ERR_NONE)
  {
    qmi_uim_free_tlv_content(parsed_data_ptr, count);
  }

  return result;
} /* qmi_uim_parse_request */


/*===========================================================================
  FUNCTION QMI_UIM_FREE_TLV_CONTENT()

  DESCRIPTION
    Free the data inside a TLV

  PARAMETERS
    tlv_data_ptr : pointer to TLV data
    count        : number of TLV items

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_free_tlv_content
(
  qmi_uim_tlv_item_type * tlv_data_ptr,
  uint16                  count
)
{
  uint16  i = 0;

  ASSERT(tlv_data_ptr);

  /* Free data in the TLV: only for write transparent and write
     record, we need to make a free */
  for (i = 0; i < count; i++)
  {
    if (!tlv_data_ptr[i].present)
    {
      /* TLV is not present... nothing to free */
      continue;
    }

    switch(tlv_data_ptr[i].type)
    {
      case UIMI_TLV_TYPE_WRITE_TRANSPARENT:
        if (tlv_data_ptr[i].data.write_transparent.data.data_ptr)
        {
          uimqmi_free(tlv_data_ptr[i].data.write_transparent.data.data_ptr);
          tlv_data_ptr[i].data.write_transparent.data.data_ptr = NULL;
        }
        break;
      case UIMI_TLV_TYPE_WRITE_RECORD:
        if (tlv_data_ptr[i].data.write_record.data.data_ptr)
        {
          uimqmi_free(tlv_data_ptr[i].data.write_record.data.data_ptr);
          tlv_data_ptr[i].data.write_record.data.data_ptr = NULL;
        }
        break;
      case UIMI_TLV_TYPE_REGISTER_REFRESH:
        while (tlv_data_ptr[i].data.register_refresh.file_list_ptr != NULL)
        {
          qmi_uim_files_list_type * current_ptr = tlv_data_ptr[i].data.register_refresh.file_list_ptr;
          tlv_data_ptr[i].data.register_refresh.file_list_ptr = current_ptr->next_ptr;
          uimqmi_free(current_ptr);
        }
        break;
      case UIMI_TLV_TYPE_AUTHENTICATE:
        if (tlv_data_ptr[i].data.authenticate.auth_data.data_ptr)
        {
          uimqmi_free(tlv_data_ptr[i].data.authenticate.auth_data.data_ptr);
          tlv_data_ptr[i].data.authenticate.auth_data.data_ptr = NULL;
        }
        break;
      case UIMI_TLV_TYPE_APDU_REQUEST:
        if (tlv_data_ptr[i].data.send_apdu.apdu_data.data_ptr)
        {
          uimqmi_free(tlv_data_ptr[i].data.send_apdu.apdu_data.data_ptr);
          tlv_data_ptr[i].data.send_apdu.apdu_data.data_ptr = NULL;
        }
        break;
      case UIMI_TLV_TYPE_INCREASE:
        if (tlv_data_ptr[i].data.increase_data.data.data_ptr)
        {
          uimqmi_free(tlv_data_ptr[i].data.increase_data.data.data_ptr);
          tlv_data_ptr[i].data.increase_data.data.data_ptr = NULL;
        }
        break;
      case UIMI_TLV_TYPE_GW_NW_PERSO_CODE:
      case UIMI_TLV_TYPE_GW_NS_PERSO_CODE:
      case UIMI_TLV_TYPE_GW_SP_PERSO_CODE:
      case UIMI_TLV_TYPE_GW_CP_PERSO_CODE:
      case UIMI_TLV_TYPE_GW_SIM_PERSO_CODE:
      case UIMI_TLV_TYPE_1X_NW_TYPE1_PERSO_CODE:
      case UIMI_TLV_TYPE_1X_NW_TYPE2_PERSO_CODE:
      case UIMI_TLV_TYPE_1X_RUIM_PERSO_CODE:
        if(tlv_data_ptr[i].data.personalization_code.code_ptr)
        {
          uimqmi_free(tlv_data_ptr[i].data.personalization_code.code_ptr);
          tlv_data_ptr[i].data.personalization_code.code_ptr = NULL;
        }
        break;
      case UIMI_TLV_TYPE_DECRYPTED_PERSO_SECURE:
        if (tlv_data_ptr[i].data.decrypted_perso_secure.code_ptr)
        {
          uimqmi_free(tlv_data_ptr[i].data.decrypted_perso_secure.code_ptr);
          tlv_data_ptr[i].data.decrypted_perso_secure.code_ptr = NULL;
        }
        break;
      case UIMI_TLV_TYPE_DECRYPTED_SIMLOCK_CONFIG:
        if (tlv_data_ptr[i].data.decrypted_simlock_config.operation_type.sfs_read_operation.data.data_ptr)
        {
          uimqmi_free(tlv_data_ptr[i].data.decrypted_simlock_config.operation_type.sfs_read_operation.data.data_ptr);
          tlv_data_ptr[i].data.decrypted_simlock_config.operation_type.sfs_read_operation.data.data_ptr = NULL;
        }
        break;
      case UIMI_TLV_TYPE_NAF_ID:
        if(tlv_data_ptr[i].data.gba_naf_id.naf_id_ptr)
        {
          uimqmi_free(tlv_data_ptr[i].data.gba_naf_id.naf_id_ptr);
          tlv_data_ptr[i].data.gba_naf_id.naf_id_ptr = NULL;
        }
        break;
      case UIMI_TLV_TYPE_REMOTE_UNLOCK_CONFIG:
        if(tlv_data_ptr[i].data.remote_unlock_config.config_data.data_ptr)
        {
          uimqmi_free(tlv_data_ptr[i].data.remote_unlock_config.config_data.data_ptr);
          tlv_data_ptr[i].data.remote_unlock_config.config_data.data_ptr = NULL;
        }
        break;
      case UIMI_TLV_TYPE_EUICC_DOWNLOAD_CODE:
      case UIMI_TLV_TYPE_EUICC_CONFIRMATION_CODE:
        if (tlv_data_ptr[i].data.euicc_download_code.code.data_ptr)
        {
          uimqmi_free(tlv_data_ptr[i].data.euicc_download_code.code.data_ptr);
          tlv_data_ptr[i].data.euicc_download_code.code.data_ptr = NULL;
        }
        break;
      case UIMI_TLV_TYPE_SEARCH_PATTERN:
        if (tlv_data_ptr[i].data.search_pattern.pattern.data_ptr)
        {
          uimqmi_free(tlv_data_ptr[i].data.search_pattern.pattern.data_ptr);
          tlv_data_ptr[i].data.search_pattern.pattern.data_ptr = NULL;
        }
        break;
      default:
        /* Nothing to free for all other TLVs */
        break;
    }
  }

  /* Zero the memory for security reasons: the TLV might contain
     PIN codes or other sensitive information */
  if (count > 0)
  {
    memset(tlv_data_ptr, 0, count * sizeof(qmi_uim_tlv_item_type));
  }
} /* qmi_uim_free_tlv_content */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_CARD_RESULT()

  DESCRIPTION
    Composes the TLV content of card status

  PARAMETERS
    tlv_id        : id to be used for the TLV
    response_pptr : output sdu
    status_word   : status word, as received from the card

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_card_result
(
  byte            tlv_id,
  dsm_item_type** response_pptr,
  mmgsdi_sw_type  status_word
)
{
  PACKED struct PACKED_POST
  {
    uint8 sw1;
    uint8 sw2;
  } tlv_data;

  ASSERT(response_pptr);

  /* SW code is not present... simply return TRUE.
     This TLV is optional and will be skipped */
  if (status_word.present == FALSE)
  {
    return TRUE;
  }

  tlv_data.sw1 = status_word.sw1;
  tlv_data.sw2 = status_word.sw2;

  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               sizeof(tlv_data),
                               (void *)&tlv_data);
} /* qmi_uim_response_card_result */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_READ_ADDITIONAL_RECORDS_RESULT()

  DESCRIPTION
    Composes the TLV content of additional records result

  PARAMETERS
    tlv_id         : id to be used for the TLV
    response       : output sdu
    num_records    : number of records in the array
    records_ptr    : array of records from the card

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_read_additional_records_result
(
  byte                              tlv_id,
  dsm_item_type **                  response_pptr,
  uint16                            num_records,
  qmi_uim_additional_records_type * records_ptr
)
{
  uint16  i            = 0;
  uint16  expected_len = 0;
  boolean ret_value    = TRUE;
  uint16  tlv_len      = 0;
  uint8*  tlv_data_ptr = NULL;
  uint8*  curr_ptr     = NULL;
  uint16  temp_uint16  = 0;

  ASSERT(response_pptr);

  /* At least 2 records should be present. We can still return TRUE
     as this is an optional TLV */
  if (num_records <= 1 ||
      records_ptr == NULL)
  {
    return TRUE;
  }

  /* All records should have the size of the first record. We can still
     return TRUE as this is an optional TLV */
  expected_len = (uint16)records_ptr[0].read_data.data_len;
  if (expected_len == 0)
  {
    return TRUE;
  }

  /* Calculate amount of memory: all records must have the same length as
     the first one. The first record should not be copied. */
  tlv_len = sizeof(uint16);
  for (i = 1; i < num_records; i++)
  {
    if (records_ptr[i].mmgsdi_status == MMGSDI_SUCCESS &&
        records_ptr[i].read_data.data_len == expected_len &&
        records_ptr[i].read_data.data_ptr != NULL)
    {
      tlv_len += expected_len;
    }
    else
    {
      break;
    }
  }

  /* Allocate memory */
  tlv_data_ptr = (uint8*)uimqmi_malloc(tlv_len);
  if (tlv_data_ptr == NULL)
  {
    return FALSE;
  }
  curr_ptr = tlv_data_ptr;

  /* Copy length */
  temp_uint16 = tlv_len - sizeof(uint16);
  (void)memscpy((void*)tlv_data_ptr, tlv_len, &temp_uint16, sizeof(uint16));
  curr_ptr += sizeof(uint16);

  /* Copy data (except first record) */
  for (i = 1; i < num_records; i++)
  {
    if (records_ptr[i].mmgsdi_status == MMGSDI_SUCCESS &&
        records_ptr[i].read_data.data_len == expected_len &&
        records_ptr[i].read_data.data_ptr != NULL)
    {
      ASSERT(curr_ptr + expected_len <= tlv_data_ptr + tlv_len);
      (void)memscpy(curr_ptr,
                    tlv_len - (curr_ptr - tlv_data_ptr),
             records_ptr[i].read_data.data_ptr,
             records_ptr[i].read_data.data_len);
      curr_ptr += records_ptr[i].read_data.data_len;
    }
    else
    {
      break;
    }
  }

  ret_value = qmi_svc_put_param_tlv(response_pptr,
                                    tlv_id,
                                    tlv_len,
                                    (void *)tlv_data_ptr);

  uimqmi_free(tlv_data_ptr);
  return ret_value;
} /* qmi_uim_response_read_additional_records_result */


/*===========================================================================
  FUNCTION QMI_UIM_CONVERT_SECURITY_ATTRIBUTES()

  DESCRIPTION
security values to be used in the TLV response
  for get file attributes

  PARAMETERS
    file_security       : MMGSDI structure with file security info
    security_method_ptr : output parameter with security method
    security_mask_ptr   : output parameter with security mask

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_uim_convert_security_attributes
(
  mmgsdi_file_security_type file_security,
  uint8  *                  security_method_ptr,
  uint16 *                  security_mask_ptr
)
{
  ASSERT(security_method_ptr);
  ASSERT(security_mask_ptr);

  /* Compose the security method */
  switch(file_security.protection_method)
  {
    case MMGSDI_ALWAYS_ALLOWED:
      *security_method_ptr = 0;
      break;

    case MMGSDI_NEVER_ALLOWED:
      *security_method_ptr = 1;
      break;

    case MMGSDI_AND_ALLOWED:
      *security_method_ptr = 2;
      break;

    case MMGSDI_OR_ALLOWED:
      *security_method_ptr = 3;
      break;

    case MMGSDI_SINGLE_ALLOWED:
      *security_method_ptr = 4;
      break;

    default:
      *security_method_ptr = 0;
      break;
  }

  /* Compose the security mask */
  *security_mask_ptr = 0;
  if (file_security.protection_method == MMGSDI_AND_ALLOWED ||
      file_security.protection_method == MMGSDI_OR_ALLOWED ||
      file_security.protection_method == MMGSDI_SINGLE_ALLOWED)
  {
    if (file_security.num_protection_pin != 0)
    {
      uint16 i = 0;
      for (i = 0; i < file_security.num_protection_pin; i++)
      {
        switch(file_security.protection_pin_ptr[i])
        {
          case MMGSDI_PIN1:
            *security_mask_ptr |= 0x1;
            break;

          case MMGSDI_PIN2:
            *security_mask_ptr |= 0x2;
            break;

          case MMGSDI_UNIVERSAL_PIN:
            *security_mask_ptr |= 0x4;
            break;

          default:
            /* Handle all ADM values in the same way */
            *security_mask_ptr |= 0x8;
            break;
        }
      }
    }
  }
} /* qmi_uim_convert_security_attributes */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_GET_FILE_ATTRIBUTES()

  DESCRIPTION
    Composes the TLV content of read result

  PARAMETERS
    tlv_id         : id to be used for the TLV
    response_pptr  : output sdu
    attribute_data : file attributes from the card
    raw_value      : attributes raw value

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_get_file_attributes
(
  byte                           tlv_id,
  dsm_item_type **               response_pptr,
  mmgsdi_file_attributes_type    attribute_data,
  mmgsdi_get_file_attr_data_type raw_value
)
{
  boolean                            ret_value         = TRUE;
  uint16                             tlv_len           = 0;
  uint8*                             tlv_data_complete = NULL;
  mmgsdi_file_security_access_type * security_access   = NULL;

  PACKED struct PACKED_POST
  {
    uint16 file_size;
    uint16 file_id;
    uint8  file_type;
    uint16 rec_size;
    uint16 rec_count;
    uint8  security_read_method;
    uint16 security_read_mask;
    uint8  security_write_method;
    uint16 security_write_mask;
    uint8  security_increase_method;
    uint16 security_increase_mask;
    uint8  security_deactivate_method;
    uint16 security_deactivate_mask;
    uint8  security_activate_method;
    uint16 security_activate_mask;
    uint16 raw_len;
  } tlv_data;

  ASSERT(response_pptr);

  tlv_len = sizeof(tlv_data);
  if (raw_value.data_len > 0 && raw_value.data_ptr != NULL)
  {
     tlv_len += (uint16)raw_value.data_len;
  }

  tlv_data_complete = (uint8*)uimqmi_malloc(tlv_len);
  if (tlv_data_complete == NULL)
  {
    return FALSE;
  }

  tlv_data.file_size = (uint16)attribute_data.file_size;
  tlv_data.file_id   = (attribute_data.file_id[0] << 8) + attribute_data.file_id[1];
  switch (attribute_data.file_type)
  {
    case MMGSDI_LINEAR_FIXED_FILE:
      tlv_data.file_type = 2;
      tlv_data.rec_size  = (uint16)attribute_data.file_info.linear_fixed_file.rec_len;
      tlv_data.rec_count = (uint16)attribute_data.file_info.linear_fixed_file.num_of_rec;
      security_access    = &attribute_data.file_info.linear_fixed_file.file_security;
      break;

    case MMGSDI_CYCLIC_FILE:
      tlv_data.file_type = 1;
      tlv_data.rec_size  = (uint16)attribute_data.file_info.cyclic_file.rec_len;
      tlv_data.rec_count = (uint16)attribute_data.file_info.cyclic_file.num_of_rec;
      security_access    = &attribute_data.file_info.cyclic_file.file_security;
      break;

    case MMGSDI_TRANSPARENT_FILE:
      tlv_data.file_type = 0;
      tlv_data.rec_size  = 0;
      tlv_data.rec_count = 0;
      security_access    = &attribute_data.file_info.transparent_file.file_security;
      break;

    case MMGSDI_MASTER_FILE:
      tlv_data.file_type = 4;
      tlv_data.rec_size  = 0;
      tlv_data.rec_count = 0;
      break;

    case MMGSDI_DEDICATED_FILE:
      tlv_data.file_type = 3;
      tlv_data.rec_size  = 0;
      tlv_data.rec_count = 0;
      break;

    default:
      uimqmi_free(tlv_data_complete);
      return FALSE;
  }

  if (security_access != NULL)
  {
    uint8  temp_security_method = 0;
    uint16 temp_security_mask   = 0;

    /* Read */
    qmi_uim_convert_security_attributes(security_access->read,
                                        &temp_security_method,
                                        &temp_security_mask);
    tlv_data.security_read_method = temp_security_method;
    tlv_data.security_read_mask   = temp_security_mask;

    /* Write */
    qmi_uim_convert_security_attributes(security_access->write,
                                        &temp_security_method,
                                        &temp_security_mask);
    tlv_data.security_write_method = temp_security_method;
    tlv_data.security_write_mask   = temp_security_mask;

    /* Increase */
    qmi_uim_convert_security_attributes(security_access->increase,
                                        &temp_security_method,
                                        &temp_security_mask);
    tlv_data.security_increase_method = temp_security_method;
    tlv_data.security_increase_mask   = temp_security_mask;

    /* Deactivate */
    qmi_uim_convert_security_attributes(security_access->invalidate_deactivate,
                                        &temp_security_method,
                                        &temp_security_mask);
    tlv_data.security_deactivate_method = temp_security_method;
    tlv_data.security_deactivate_mask   = temp_security_mask;

    /* Activate */
    qmi_uim_convert_security_attributes(security_access->rehabilitate_activate,
                                        &temp_security_method,
                                        &temp_security_mask);
    tlv_data.security_activate_method = temp_security_method;
    tlv_data.security_activate_mask   = temp_security_mask;
  }
  else
  {
    /* Set secutrity as always for default */
    tlv_data.security_read_method       = 0;
    tlv_data.security_read_mask         = 0;
    tlv_data.security_write_method      = 0;
    tlv_data.security_write_mask        = 0;
    tlv_data.security_increase_method   = 0;
    tlv_data.security_increase_mask     = 0;
    tlv_data.security_deactivate_method = 0;
    tlv_data.security_deactivate_mask   = 0;
    tlv_data.security_activate_method   = 0;
    tlv_data.security_activate_mask     = 0;
  }

  tlv_data.raw_len = (uint16)raw_value.data_len;

  /* Copy packed structure in buffer to be sent */
  (void)memscpy((void*)tlv_data_complete, tlv_len, &tlv_data, sizeof(tlv_data));
  if (raw_value.data_len > 0 && raw_value.data_ptr != NULL)
  {
    (void)memscpy((void*)(tlv_data_complete + sizeof(tlv_data)),
                  tlv_len - sizeof(tlv_data),
           (void*)raw_value.data_ptr,
           (uint32)raw_value.data_len);
  }

  ret_value = qmi_svc_put_param_tlv(response_pptr,
                                    tlv_id,
                                    tlv_len,
                                    (void *)tlv_data_complete);

  uimqmi_free(tlv_data_complete);
  return ret_value;
} /* qmi_uim_response_get_file_attributes */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_RETRIES_LEFT()

  DESCRIPTION
    Composes the TLV content of PIN retries left

  PARAMETERS
    tlv_id              : id to be used for the TLV
    response_pptr       : output sdu
    num_retries         : number of PIN retries
    num_unblock_retries : number of PUK retries

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_retries_left
(
  byte             tlv_id,
  dsm_item_type ** response_pptr,
  uint32           num_retries,
  uint32           num_unblock_retries
)
{
  PACKED struct PACKED_POST
  {
    uint8 num_retries;
    uint8 num_unblock_retries;
  } tlv_data;

  ASSERT(response_pptr);

  tlv_data.num_retries = (uint8)num_retries;
  tlv_data.num_unblock_retries = (uint8)num_unblock_retries;

  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               sizeof(tlv_data),
                               (void *)&tlv_data);
} /* qmi_uim_response_retries_left */


/*===========================================================================
  FUNCTION QMI_UIM_AID_MATCH()

  DESCRIPTION
    Matches the two AIDs and return result

  PARAMETERS
    app_aid_ptr         : Value of USIM/CSIM/ISIM AID
    app_aid_len         : Length of USIM/CSIM/ISIM AID
    aid_ptr             : Value of the request AID
    partial_aid_match_ok: Dictates whether caller is ok with going ahead
                          with checking for a partial AID match incase
                          complete AID did not match with USIM/CSIM/ISIM

  RETURN VALUE
    TRUE if matches. FALSE if not

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_uim_aid_match
(
  const uint8*                    app_aid_ptr,
  uint8                           app_aid_len,
  const mmgsdi_static_data_type * aid_ptr,
  boolean                         partial_aid_match_ok
)
{
  boolean     match_found      = FALSE;

  ASSERT(aid_ptr != NULL);
  ASSERT(app_aid_ptr != NULL);

  if (aid_ptr->data_len < 0 ||
      aid_ptr->data_len > MMGSDI_MAX_AID_LEN)
  {
    return FALSE;
  }

  if (aid_ptr->data_len >= app_aid_len)
  {
    if (memcmp(aid_ptr->data_ptr, app_aid_ptr, app_aid_len) == 0)
    {
      /* AID matched completely with a USIM AID (first 7 bytes)*/
      match_found = TRUE;
    }
    return match_found;
  }

  /* Input AID seems to be a partial AID (shorter than 7 bytes). If the
     caller is ok with checking for a partial-AID match, lets see if the
     input AID matches partially with the USIM/CSIM/ISIM aid and return
     TRUE in that case. */
  if (partial_aid_match_ok)
  {
    /* If zero-length AID is passed in as input, lets mark it as a (partial)
       match */
    if (aid_ptr->data_len == 0 ||
        memcmp(aid_ptr->data_ptr, app_aid_ptr, aid_ptr->data_len) == 0)
    {
      match_found = TRUE;
    }
  }
  return match_found;
} /* qmi_uim_aid_match */


/*===========================================================================
  FUNCTION QMI_UIM_IS_ISIM_AID()

  DESCRIPTION
    Checks if the current AID belongs to a ISIM or not

  PARAMETERS
    aid_ptr       : value of the AID

  RETURN VALUE
    TRUE for ISIM. FALSE in all other cases

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_is_isim_aid
(
  const mmgsdi_static_data_type * aid_ptr
)
{
  const uint8 isim_aid_value[] = { 0xA0, 0x00, 0x00, 0x00, 0x87,
                                   0x10, 0x04 };

  return qmi_uim_aid_match(isim_aid_value,
                           sizeof(isim_aid_value),
                           aid_ptr,
                           FALSE);
} /* qmi_uim_is_isim_aid */


/*===========================================================================
  FUNCTION QMI_UIM_IS_USIM_AID()

  DESCRIPTION
    Checks if the current AID belongs to a USIM or not.

  PARAMETERS
    aid_ptr             : value of the AID
    partial_aid_match_ok: dictates whether caller is ok with going ahead
                          with checking for a partial AID match incase
                          complete AID did not match with USIM

  RETURN VALUE
    boolean
      TRUE        : if the AID matched (completely or partially depending on
                    input parameter).
      FALSE       : in all other cases

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_is_usim_aid
(
  const   mmgsdi_static_data_type * aid_ptr,
  boolean                           partial_aid_match_ok
)
{
  const uint8 usim_aid_value[] = { 0xA0, 0x00, 0x00, 0x00, 0x87,
                                   0x10, 0x02 };

  return qmi_uim_aid_match(usim_aid_value,
                           sizeof(usim_aid_value),
                           aid_ptr,
                           partial_aid_match_ok);
} /* qmi_uim_is_usim_aid */


/*===========================================================================
  FUNCTION QMI_UIM_IS_CSIM_AID()

  DESCRIPTION
    Checks if the current AID belongs to a CSIM or not.

  PARAMETERS
    aid_ptr             : value of the AID
    partial_aid_match_ok: dictates whether caller is ok with going ahead
                          with checking for a partial AID match incase
                          complete AID did not match with USIM

  RETURN VALUE
    boolean
      TRUE        : if the AID matched (completely or partially depending on
                    input parameter).
      FALSE       : in all other cases

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_is_csim_aid
(
  const   mmgsdi_static_data_type * aid_ptr,
  boolean                           partial_aid_match_ok
)
{
  const uint8 csim_aid_value[] = { 0xA0, 0x00, 0x00, 0x03, 0x43,
                                   0x10, 0x02 };

  return qmi_uim_aid_match(csim_aid_value,
                           sizeof(csim_aid_value),
                           aid_ptr,
                           partial_aid_match_ok);
} /* qmi_uim_is_csim_aid */


#ifdef FEATURE_SIMLOCK
/*===========================================================================
  FUNCTION QMI_UIM_POPULATE_SIMLOCK_STATUS_FOR_VALID_CARD_ANY_SLOT_POLICY()

  DESCRIPTION
    This function updates the perso state of current slot application if at
    least one other slot has card inserted and any applications is in PIN
    state or in PERSO check state with perso state <= in progress.

  PARAMETERS
    card_state_ptr         : pointer to card state
    current_slot_index     : current slot index in card state
    current_slot_app_index : current slot app index in card state
    max_card_count         : maximum card count for this target

  RETURN VALUE
    qmi_uim_perso_state_type : perso state to be set for current
                               slot appplication

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_uim_perso_state_type qmi_uim_populate_simlock_status_for_valid_card_any_slot_policy
(
  const                    qmi_uim_card_state * card_state_ptr,
  uint8                    current_slot_index,
  uint8                    current_slot_app_index,
  uint8                    max_card_count
 )
{
  uint8   other_slot_index     = 0;
  uint8   other_slot_app_index = 0;
  boolean update_perso_state   = FALSE;
  uint8   app_slot_index           = 0;

  if (card_state_ptr == NULL)
  {
    return QMI_UIM_PERSO_STATE_UNKNOWN;
  }

  if(card_state_ptr->application[current_slot_app_index]->perso_state !=
        QMI_UIM_PERSO_STATE_CODE_REQUIRED ||
      max_card_count <= 1)
  {
    return card_state_ptr->application[current_slot_app_index]->perso_state;
  }

  /* check other slots and all app states */
  for (other_slot_index = 0;
       update_perso_state == FALSE && other_slot_index < max_card_count;
       other_slot_index++)
  {
    /* Update perso state when at least one other slot has card inserted and
       any applications stuck in QMI_UIM_APP_STATE_PIN_REQUIRED or
       in QMI_UIM_APP_STATE_PERSO with perso_state <= QMI_UIM_PERSO_STATE_IN_PROGRESS*/
    if (card_state_ptr->card[other_slot_index] == NULL ||
        (other_slot_index == current_slot_index) ||
        (other_slot_index != current_slot_index &&
         card_state_ptr->card[other_slot_index]->card_state != QMI_UIM_CARD_STATE_PRESENT))
    {
      continue;
    }

    /* check for apps from other_slot_index */
    for (other_slot_app_index = 0;
         (other_slot_app_index < card_state_ptr->num_apps &&
          other_slot_app_index < QMI_UIM_MAX_APP_COUNT);
         other_slot_app_index++)
    {
      if (card_state_ptr->application[other_slot_app_index] == NULL)
      {
        continue;
      }
	  
      /* Pick only applications with other_slot_index*/
      if (!qmi_uim_convert_slot_to_index(card_state_ptr->application[other_slot_app_index]->slot,
                                         &app_slot_index)               ||
          app_slot_index != other_slot_index)
      {
        continue;
      }

      if ((card_state_ptr->application[other_slot_app_index]->app_state == QMI_UIM_APP_STATE_PIN_REQUIRED) ||
          (card_state_ptr->application[other_slot_app_index]->app_state == QMI_UIM_APP_STATE_PERSO &&
           card_state_ptr->application[other_slot_app_index]->perso_state <= QMI_UIM_PERSO_STATE_IN_PROGRESS))
      {

        UIM_MSG_HIGH_4("update perso state from 0x%x to 1, app slot 0x%x, app state 0x%x, perso 0x%x",
                       card_state_ptr->application[current_slot_app_index]->perso_state,
                       other_slot_app_index,
                       card_state_ptr->application[other_slot_app_index]->app_state,
                       card_state_ptr->application[other_slot_app_index]->perso_state);
        update_perso_state = TRUE;
        break;
      }
    }
  }

  if (update_perso_state)
  {
    return QMI_UIM_PERSO_STATE_IN_PROGRESS;
  }

  return card_state_ptr->application[current_slot_app_index]->perso_state;
} /* qmi_uim_populate_simlock_status_for_valid_card_any_slot_policy */
#endif /* FEATURE_SIMLOCK */


/*===========================================================================
  FUNCTION QMI_UIM_POPULATE_CARD_AND_APP_STATE()

  DESCRIPTION
    Composes the card and app state info for card status indication or
    get card status request

  PARAMETERS
    tlv_len                 : length of tlv_data_ptr
    tlv_data_ptr            : memory to fill in TLV data
    card_state_ptr          : card status
    app_count_per_slot_len  : length of apps present per slot array
    app_count_per_slot_ptr  : apps present per slot
    extended_card_status    : if the TLV is for extended card status
    validit_len             : length of card status validity array
    validity_ptr            : card status validity array

  RETURN VALUE
    boolean

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_uim_populate_card_and_app_state
(
  uint16                     tlv_len,
  uint8                    * tlv_data_ptr,
  const qmi_uim_card_state * card_state_ptr,
  uint8                      app_count_per_slot_len,
  uint8                    * app_count_per_slot_ptr,
  boolean                    extended_card_status,
  uint8                      validity_len,
  const boolean            * validity_ptr
)
{
  uint8                           i                          = 0;
  uint8                           j                          = 0;
  uint8                           slot_index                 = 0;
  uint8                         * curr_ptr                   = NULL;
  uint8                           max_card_count             = 0;
  uint8                           max_app_count              = 0;
  uint8                           app_count                  = 0;
  tlv_data_card_state_type        tlv_data_card_state;
  tlv_data_app_state_top_type     tlv_data_app_state_top;
  tlv_data_app_state_bottom_type  tlv_data_app_state_bottom;

  ASSERT(card_state_ptr);
  ASSERT(tlv_data_ptr);
  ASSERT(app_count_per_slot_ptr);

  curr_ptr = tlv_data_ptr;

  if (extended_card_status)
  {
    max_card_count = (card_state_ptr->num_slots < QMI_UIM_MAX_CARD_COUNT) ?
                       card_state_ptr->num_slots : QMI_UIM_MAX_CARD_COUNT;
    max_app_count = QMI_UIM_MAX_APPS_PER_CARD;
  }
  else
  {
    max_card_count = (card_state_ptr->num_slots < QMI_UIM_LEGACY_MAX_CARD_COUNT) ?
                       card_state_ptr->num_slots : QMI_UIM_LEGACY_MAX_CARD_COUNT;
    max_app_count = QMI_UIM_LEGACY_MAX_APPS_PER_CARD;
  }

  if(app_count_per_slot_len < max_card_count)
  {
    return FALSE;
  }

  /* Loop thru all the cards */
  for (i = 0; i < max_card_count; i++)
  {
    if (card_state_ptr->card[i] == NULL)
    {
      return FALSE;
    }

    /* Card state */
    if (extended_card_status && (validity_ptr != NULL) && (i < validity_len))
    {
      if (validity_ptr[i])
      {
        tlv_data_card_state.card_state = (uint8)card_state_ptr->card[i]->card_state;
      }
      else
      {
        tlv_data_card_state.card_state = 3;
      }
    }
    else
    {
      tlv_data_card_state.card_state = (uint8)card_state_ptr->card[i]->card_state;
    }

    switch(card_state_ptr->card[i]->upin_state)
    {
      case MMGSDI_PIN_ENABLED_NOT_VERIFIED:
        tlv_data_card_state.upin_state = 1;
        break;
      case MMGSDI_PIN_ENABLED_VERIFIED:
        tlv_data_card_state.upin_state = 2;
        break;
      case MMGSDI_PIN_DISABLED:
        tlv_data_card_state.upin_state = 3;
        break;
      case MMGSDI_PIN_BLOCKED:
        tlv_data_card_state.upin_state = 4;
        break;
      case MMGSDI_PIN_PERM_BLOCKED:
        tlv_data_card_state.upin_state = 5;
        break;
      case MMGSDI_PIN_STATUS_NOT_INITIALIZED:
      default:
        tlv_data_card_state.upin_state = 0;
        break;
    }
    tlv_data_card_state.upin_retries = card_state_ptr->card[i]->upin_num_retries;
    tlv_data_card_state.upuk_retries = card_state_ptr->card[i]->upuk_num_retries;
    tlv_data_card_state.error_code   = (uint8)card_state_ptr->card[i]->card_error;
    tlv_data_card_state.num_app      = app_count_per_slot_ptr[i];

    /* Copy card state in the TLV */
    ASSERT(curr_ptr + sizeof(tlv_data_card_state) <= tlv_data_ptr + tlv_len);
    (void)memscpy(curr_ptr, tlv_len - (curr_ptr - tlv_data_ptr),
                  (void*)&tlv_data_card_state, sizeof(tlv_data_card_state));
    curr_ptr += sizeof(tlv_data_card_state);

    /* Loop thru all the applications */
    app_count = 0;
    for (j = 0; j < card_state_ptr->num_apps && j < QMI_UIM_MAX_APP_COUNT; j++)
    {
      if (app_count >= max_app_count)
      {
        break;
      }

      if (card_state_ptr->application[j] == NULL)
      {
        return FALSE;
      }

      if (extended_card_status == FALSE &&
          (card_state_ptr->application[j]->slot != MMGSDI_SLOT_1 &&
           card_state_ptr->application[j]->slot != MMGSDI_SLOT_2))
      {
        continue;
      }

      /* Pick only applications for this card */
      if (!qmi_uim_convert_slot_to_index(card_state_ptr->application[j]->slot,
                                         &slot_index))
      {
        continue;
      }

      if (slot_index != i)
      {
        continue;
      }

      /* App type */
      switch(card_state_ptr->application[j]->app_type)
      {
        case MMGSDI_APP_SIM:
          tlv_data_app_state_top.app_type = 1;
          break;
        case MMGSDI_APP_RUIM:
          tlv_data_app_state_top.app_type = 3;
          break;
        case MMGSDI_APP_USIM:
          tlv_data_app_state_top.app_type = 2;
          break;
        case MMGSDI_APP_CSIM:
          tlv_data_app_state_top.app_type = 4;
          break;
        case MMGSDI_APP_UNKNOWN:
          if (qmi_uim_is_isim_aid(&card_state_ptr->application[j]->aid))
          {
            tlv_data_app_state_top.app_type = 5;
          }
          else
          {
            tlv_data_app_state_top.app_type = 0;
          }
          break;
        default:
          tlv_data_app_state_top.app_type = 0;
          break;
      }

      /* App state */
      tlv_data_app_state_top.app_state =
        (uint8)card_state_ptr->application[j]->app_state;

      /* Perso state */
      tlv_data_app_state_top.perso_state =
        (uint8)card_state_ptr->application[j]->perso_state;

#ifdef FEATURE_SIMLOCK
      /* Check if simlock status update for display is needed */
      if (qmi_uim_util_is_simlock_status_for_valid_card_enabled())
      {
        tlv_data_app_state_top.perso_state =
          (uint8)qmi_uim_populate_simlock_status_for_valid_card_any_slot_policy(card_state_ptr,
                                                                                i,
                                                                                j,
                                                                                max_card_count);
      }
#endif /* FEATURE_SIMLOCK */

      /* Perso feature */
      switch(card_state_ptr->application[j]->perso_feature)
      {
        case MMGSDI_PERSO_NW:
          tlv_data_app_state_top.perso_feature = 0;
          break;

        case MMGSDI_PERSO_NS:
          tlv_data_app_state_top.perso_feature = 1;
          break;

        case MMGSDI_PERSO_SP:
          tlv_data_app_state_top.perso_feature = 2;
          break;

        case MMGSDI_PERSO_CP:
          tlv_data_app_state_top.perso_feature = 3;
          break;

        case MMGSDI_PERSO_SIM:
          tlv_data_app_state_top.perso_feature = 4;
          break;

        case MMGSDI_PERSO_SPN:
          tlv_data_app_state_top.perso_feature = 12;
          break;

        case MMGSDI_PERSO_ICCID:
          tlv_data_app_state_top.perso_feature = UIM_PERSO_FEATURE_STATUS_GW_ICCID_V01;
          break;

        case MMGSDI_PERSO_RUIM_NW1:
          tlv_data_app_state_top.perso_feature = 5;
          break;

        case MMGSDI_PERSO_RUIM_NW2:
          tlv_data_app_state_top.perso_feature = 6;
          break;

        case MMGSDI_PERSO_RUIM_HRPD:
          tlv_data_app_state_top.perso_feature = 7;
          break;

        case MMGSDI_PERSO_RUIM_SP:
          tlv_data_app_state_top.perso_feature = 8;
          break;

        case MMGSDI_PERSO_RUIM_CP:
          tlv_data_app_state_top.perso_feature = 9;
          break;

        case MMGSDI_PERSO_RUIM_RUIM:
          tlv_data_app_state_top.perso_feature = 10;
          break;

        default:
          tlv_data_app_state_top.perso_feature = 11;
          break;
      }

      /* Perso retries */
      tlv_data_app_state_top.perso_retries =
        card_state_ptr->application[j]->perso_retries;

      /* Perso unblock retries */
      tlv_data_app_state_top.perso_unblock_retries =
        card_state_ptr->application[j]->perso_unblock_retries;

      /* AID length */
      tlv_data_app_state_top.aid_len =
        (uint8)card_state_ptr->application[j]->aid.data_len;

      /* UPIN */
      tlv_data_app_state_bottom.univ_pin =
        (card_state_ptr->application[j]->upin ? 1 : 0);

      /* PIN 1 state */
      switch(card_state_ptr->application[j]->pin1_state)
      {
        case MMGSDI_PIN_ENABLED_NOT_VERIFIED:
          tlv_data_app_state_bottom.pin1_state = 1;
          break;
        case MMGSDI_PIN_ENABLED_VERIFIED:
          tlv_data_app_state_bottom.pin1_state = 2;
          break;
        case MMGSDI_PIN_DISABLED:
          tlv_data_app_state_bottom.pin1_state = 3;
          break;
        case MMGSDI_PIN_BLOCKED:
          tlv_data_app_state_bottom.pin1_state = 4;
          break;
        case MMGSDI_PIN_PERM_BLOCKED:
          tlv_data_app_state_bottom.pin1_state = 5;
          break;
        case MMGSDI_PIN_STATUS_NOT_INITIALIZED:
          if (card_state_ptr->application[j]->app_state == QMI_UIM_APP_STATE_READY ||
              card_state_ptr->application[j]->app_state == QMI_UIM_APP_STATE_PERSO ||
              card_state_ptr->application[j]->app_state == QMI_UIM_APP_STATE_ILLEGAL)
          {
            tlv_data_app_state_bottom.pin1_state = 3;
          }
          else
          {
            tlv_data_app_state_bottom.pin1_state = 0;
          }
          break;
        default:
          tlv_data_app_state_bottom.pin1_state = 0;
          break;
      }

      /* PIN1 retries */
      tlv_data_app_state_bottom.pin1_retries =
        card_state_ptr->application[j]->pin1_num_retries;
      tlv_data_app_state_bottom.puk1_retries =
        card_state_ptr->application[j]->puk1_num_retries;

      /* PIN 2 state */
      switch(card_state_ptr->application[j]->pin2_state)
      {
        case MMGSDI_PIN_ENABLED_NOT_VERIFIED:
          tlv_data_app_state_bottom.pin2_state = 1;
          break;
        case MMGSDI_PIN_ENABLED_VERIFIED:
          tlv_data_app_state_bottom.pin2_state = 2;
          break;
        case MMGSDI_PIN_DISABLED:
          tlv_data_app_state_bottom.pin2_state = 3;
          break;
        case MMGSDI_PIN_BLOCKED:
          tlv_data_app_state_bottom.pin2_state = 4;
          break;
        case MMGSDI_PIN_PERM_BLOCKED:
          tlv_data_app_state_bottom.pin2_state = 5;
          break;
        case MMGSDI_PIN_STATUS_NOT_INITIALIZED:
        default:
          /* If PIN2 state is not initialized or unknown,
             do not update based on app state */
          tlv_data_app_state_bottom.pin2_state = 0;
          break;
      }

      /* PIN2 retries */
      tlv_data_app_state_bottom.pin2_retries =
        card_state_ptr->application[j]->pin2_num_retries;
      tlv_data_app_state_bottom.puk2_retries =
        card_state_ptr->application[j]->puk2_num_retries;

      /* Copy top part to TLV */
      ASSERT(curr_ptr + sizeof(tlv_data_app_state_top) <= tlv_data_ptr + tlv_len);
      (void)memscpy((void*)curr_ptr,
                    tlv_len - (curr_ptr - tlv_data_ptr),
             (void*)&tlv_data_app_state_top,
             sizeof(tlv_data_app_state_top));
      curr_ptr += sizeof(tlv_data_app_state_top);

      /* AID value */
      if (card_state_ptr->application[j]->aid.data_len > 0)
      {
        ASSERT(curr_ptr + card_state_ptr->application[j]->aid.data_len <= tlv_data_ptr + tlv_len);
        (void)memscpy((void*)curr_ptr,
                      tlv_len - (curr_ptr - tlv_data_ptr),
               (void*)card_state_ptr->application[j]->aid.data_ptr,
               (uint32)card_state_ptr->application[j]->aid.data_len);
        curr_ptr += card_state_ptr->application[j]->aid.data_len;
      }

      /* Copy bottom part to TLV */
      ASSERT(curr_ptr + sizeof(tlv_data_app_state_bottom) <= tlv_data_ptr + tlv_len);
      (void)memscpy((void*)curr_ptr,
                    tlv_len - (curr_ptr - tlv_data_ptr),
             (void*)&tlv_data_app_state_bottom,
             (uint32)sizeof(tlv_data_app_state_bottom));
      curr_ptr += sizeof(tlv_data_app_state_bottom);

      app_count++;
    } /* for (j = 0; j < card_state_ptr->num_apps && j < QMI_UIM_MAX_APP_COUNT; j++) */
  } /* for (i = 0; i < num_slots; i++) */

  return TRUE;
} /* qmi_uim_populate_card_and_app_state */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_CARD_STATUS()

  DESCRIPTION
    Composes the TLV content for card status indication or
    get card status request

  PARAMETERS
    tlv_id        : id to be used for the TLV
    response_pptr : output sdu
    card_state    : card status

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_card_status
(
  byte                       tlv_id,
  dsm_item_type **           response_pptr,
  const qmi_uim_card_state * card_state_ptr
)
{
  uint8   i            = 0;
  boolean ret_value    = TRUE;
  uint16  tlv_len      = 0;
  uint8*  tlv_data_ptr = NULL;
  uint8*  curr_ptr     = NULL;
  uint8   slot_index   = 0;
  uint8   num_slots    = 0;
  uint8   app_count_per_slot[QMI_UIM_LEGACY_MAX_CARD_COUNT];

  PACKED struct PACKED_POST
  {
    uint16 index_gw_pri;
    uint16 index_1x_pri;
    uint16 index_gw_sec;
    uint16 index_1x_sec;
    uint8  num_slot;
  } tlv_data_intro;

  ASSERT(response_pptr);
  ASSERT(card_state_ptr);
  ASSERT(card_state_ptr->num_apps <= QMI_UIM_MAX_APP_COUNT);
  ASSERT(card_state_ptr->num_slots > 0);

  memset(app_count_per_slot, 0, sizeof(app_count_per_slot));
  num_slots = (QMI_UIM_LEGACY_MAX_CARD_COUNT < card_state_ptr->num_slots) ?
                QMI_UIM_LEGACY_MAX_CARD_COUNT : card_state_ptr->num_slots;

  /* Initialize TLV length. Total length will be calculated after taking into
     account the total number of apps. */
  tlv_len = sizeof(tlv_data_intro);
  tlv_len += (sizeof(tlv_data_card_state_type) * num_slots);

  /* Intro: set default values for indexes and number of slots */
  tlv_data_intro.index_gw_pri = 0xFFFF;
  tlv_data_intro.index_1x_pri = 0xFFFF;
  tlv_data_intro.index_gw_sec = 0xFFFF;
  tlv_data_intro.index_1x_sec = 0xFFFF;
  tlv_data_intro.num_slot     = num_slots;

  /* Count the number of applications for each slot and
     populate the indexes to be used in the TLV */
  for (i = 0; i < card_state_ptr->num_apps && i < QMI_UIM_MAX_APP_COUNT; i++)
  {
    if (card_state_ptr->application[i] == NULL)
    {
      continue;
    }

    if (card_state_ptr->application[i]->slot != MMGSDI_SLOT_1 &&
        card_state_ptr->application[i]->slot != MMGSDI_SLOT_2)
    {
      continue;
    }

    if (!qmi_uim_convert_slot_to_index(card_state_ptr->application[i]->slot,
                                       &slot_index))
    {
      continue;
    }

    if (app_count_per_slot[slot_index] >= QMI_UIM_LEGACY_MAX_APPS_PER_CARD)
    {
      continue;
    }

    if (i == card_state_ptr->index_gw[0])
    {
      tlv_data_intro.index_gw_pri = (uint16)((uint16)slot_index << 8) +
                                    (uint16)app_count_per_slot[slot_index];
    }
    if (i == card_state_ptr->index_1x[0])
    {
      tlv_data_intro.index_1x_pri = (uint16)((uint16)slot_index << 8) +
                                    (uint16)app_count_per_slot[slot_index];
    }
    if (i == card_state_ptr->index_gw[1])
    {
      tlv_data_intro.index_gw_sec = (uint16)((uint16)slot_index << 8) +
                                    (uint16)app_count_per_slot[slot_index];
    }
    if (i == card_state_ptr->index_1x[1])
    {
      tlv_data_intro.index_1x_sec = (uint16)((uint16)slot_index << 8) +
                                    (uint16)app_count_per_slot[slot_index];
    }
    app_count_per_slot[slot_index]++;

    tlv_len += sizeof(tlv_data_app_state_top_type) + sizeof(tlv_data_app_state_bottom_type) +
                 (uint16)card_state_ptr->application[i]->aid.data_len;
  }

  /* Allocate memory for TLV */
  tlv_data_ptr = (uint8*)uimqmi_malloc(tlv_len);
  if (tlv_data_ptr == NULL)
  {
    return FALSE;
  }
  curr_ptr = tlv_data_ptr;

  /* Copy intro in the TLV */
  ASSERT(curr_ptr + sizeof(tlv_data_intro) <= tlv_data_ptr + tlv_len);
  (void)memscpy(curr_ptr, tlv_len - (curr_ptr - tlv_data_ptr),
                (void*)&tlv_data_intro, sizeof(tlv_data_intro));
  curr_ptr += sizeof(tlv_data_intro);

  ret_value = qmi_uim_populate_card_and_app_state(tlv_len - sizeof(tlv_data_intro),
                                                  curr_ptr,
                                                  card_state_ptr,
                                                  QMI_UIM_LEGACY_MAX_CARD_COUNT,
                                                  app_count_per_slot,
                                                  FALSE,
                                                  0,
                                                  NULL);
  if (ret_value)
  {
    ret_value = qmi_svc_put_param_tlv(response_pptr,
                                      tlv_id,
                                      tlv_len,
                                      (void *)tlv_data_ptr);
  }

  uimqmi_free(tlv_data_ptr);
  return ret_value;
} /* qmi_uim_response_card_status */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_EXTENDED_CARD_STATUS()

  DESCRIPTION
    Composes the extended card status TLV content for card status indication
    or get card status request

  PARAMETERS
    tlv_id          : id to be used for the TLV
    response_pptr   : output sdu
    card_state_ptr  : card status

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_extended_card_status
(
  byte                       tlv_id,
  dsm_item_type **           response_pptr,
  const qmi_uim_card_state * card_state_ptr,
  uint8                      validity_len,
  const boolean            * validity_ptr
)
{
  uint8   i                     = 0;
  uint8   j                     = 0;
  boolean ret_value             = TRUE;
  uint16  tlv_len               = 0;
  uint8*  tlv_data_ptr          = NULL;
  uint8*  curr_ptr              = NULL;
  uint8   slot_index            = 0;
  uint8   num_slots             = 0;
  uint8   tlv_data_gw_prov_len  = 0;
  uint8   tlv_data_1x_prov_len  = 0;
  uint8   tlv_data_intro_len    = 0;
  uint8   num_prov_sessions     = 0;
  uint8   app_count_per_slot[QMI_UIM_MAX_CARD_COUNT];

  PACKED struct PACKED_POST
  {
    uint8    index_gw_len;
    uint16   index_gw[QMI_UIM_MAX_PROV_SESSIONS];
  } tlv_data_gw_prov;

  PACKED struct PACKED_POST
  {
    uint8    index_1x_len;
    uint16   index_1x[QMI_UIM_MAX_PROV_SESSIONS];
  } tlv_data_1x_prov;

  PACKED struct PACKED_POST
  {
    uint8    card_info_len;
  } tlv_data_card_info_len;

  ASSERT(response_pptr);
  ASSERT(card_state_ptr);
  ASSERT(card_state_ptr->num_apps <= QMI_UIM_MAX_APP_COUNT);
  ASSERT(card_state_ptr->num_slots > 0);

  memset(app_count_per_slot, 0, sizeof(app_count_per_slot));
  num_slots = (QMI_UIM_MAX_CARD_COUNT < card_state_ptr->num_slots) ?
                QMI_UIM_MAX_CARD_COUNT : card_state_ptr->num_slots;
  num_prov_sessions = (QMI_UIM_MAX_PROV_SESSIONS < card_state_ptr->num_slots) ?
                         QMI_UIM_MAX_PROV_SESSIONS : card_state_ptr->num_slots;

  /* Calculate actual lengths of index data that needs to be copied to
     the final TLV */
  tlv_data_gw_prov_len = sizeof(uint8) + (sizeof(uint16) * num_prov_sessions);
  tlv_data_1x_prov_len = sizeof(uint8) + (sizeof(uint16) * num_prov_sessions);
  tlv_data_intro_len = tlv_data_gw_prov_len + tlv_data_1x_prov_len +
                         sizeof(tlv_data_card_info_len);

  /* Calculate memory requirement. Here we do not take into consideration
     the max number of apps per card and in worst case may allocate more
     memory than required. */
  tlv_len = tlv_data_intro_len;
  tlv_len += (sizeof(tlv_data_card_state_type) * num_slots);
  for (i = 0; i < card_state_ptr->num_apps && i < QMI_UIM_MAX_APP_COUNT; i++)
  {
    if (card_state_ptr->application[i] != NULL)
    {
      tlv_len += sizeof(tlv_data_app_state_top_type) + sizeof(tlv_data_app_state_bottom_type) +
                 (uint16)card_state_ptr->application[i]->aid.data_len;
    }
  }

  /* Allocate memory for TLV */
  tlv_data_ptr = (uint8*)uimqmi_malloc(tlv_len);

  if (tlv_data_ptr == NULL)
  {
    return FALSE;
  }
  curr_ptr = tlv_data_ptr;

  /* Intro: set default values for indexes and number of slots */
  tlv_data_gw_prov.index_gw_len = num_prov_sessions;
  memset(tlv_data_gw_prov.index_gw, 0xFF, sizeof(tlv_data_gw_prov.index_gw));
  tlv_data_1x_prov.index_1x_len = num_prov_sessions;
  memset(tlv_data_1x_prov.index_1x, 0xFF, sizeof(tlv_data_1x_prov.index_1x));

  tlv_data_card_info_len.card_info_len = num_slots;

  /* Count the number of applications for each slot and
     populate the indexes to be used in the TLV */
  for (i = 0; i < card_state_ptr->num_apps && i < QMI_UIM_MAX_APP_COUNT; i++)
  {
    if (card_state_ptr->application[i] == NULL)
    {
      continue;
    }

    if (!qmi_uim_convert_slot_to_index(card_state_ptr->application[i]->slot,
                                       &slot_index))
    {
      continue;
    }

    for (j = 0; j < QMI_UIM_MAX_PROV_SESSIONS && j < card_state_ptr->num_slots; j++)
    {
      if (i == card_state_ptr->index_gw[j])
      {
        tlv_data_gw_prov.index_gw[j] = (uint16)((uint16)slot_index << 8) +
                                         (uint16)app_count_per_slot[slot_index];
      }
      if (i == card_state_ptr->index_1x[j])
      {
        tlv_data_1x_prov.index_1x[j] = (uint16)((uint16)slot_index << 8) +
                                         (uint16)app_count_per_slot[slot_index];
      }
    }
    app_count_per_slot[slot_index]++;
  }

  /* Copy intro in the TLV. */
  ASSERT(curr_ptr + tlv_data_intro_len <= tlv_data_ptr + tlv_len);
  (void)memscpy(curr_ptr, tlv_len - (curr_ptr - tlv_data_ptr),
                (void*)&tlv_data_gw_prov , tlv_data_gw_prov_len);
  curr_ptr += tlv_data_gw_prov_len;
  (void)memscpy(curr_ptr, tlv_len - (curr_ptr - tlv_data_ptr),
                (void*)&tlv_data_1x_prov , tlv_data_1x_prov_len);
  curr_ptr += tlv_data_1x_prov_len;
  (void)memscpy(curr_ptr, tlv_len - (curr_ptr - tlv_data_ptr),
                (void*)&tlv_data_card_info_len , sizeof(tlv_data_card_info_len));
  curr_ptr += sizeof(tlv_data_card_info_len);

  ret_value = qmi_uim_populate_card_and_app_state(tlv_len - tlv_data_intro_len,
                                                  curr_ptr,
                                                  card_state_ptr,
                                                  QMI_UIM_MAX_CARD_COUNT,
                                                  app_count_per_slot,
                                                  TRUE,
                                                  validity_len,
                                                  validity_ptr);
  if (ret_value)
  {
    ret_value = qmi_svc_put_param_tlv(response_pptr,
                                      tlv_id,
                                      tlv_len,
                                      (void *)tlv_data_ptr);
  }

  uimqmi_free(tlv_data_ptr);
  return ret_value;
} /* qmi_uim_response_extended_card_status */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_EVENT_REG()

  DESCRIPTION
    Composes the TLV content of event reg

  PARAMETERS
    tlv_id                   : id to be used for the TLV
    response_pptr            : output sdu
    reg_event_type           : event registered type

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_event_reg
(
  byte                               tlv_id,
  dsm_item_type                   ** response_pptr,
  qmi_uim_registration_event_type    reg_event_type
)
{
  PACKED struct PACKED_POST
  {
    uint32 mask;
  } tlv_data;

  ASSERT(response_pptr);

  tlv_data.mask = 0x00;

  if (reg_event_type.card_status)
  {
    tlv_data.mask |= 0x01;
  }
  if (reg_event_type.sap_events)
  {
    tlv_data.mask |= 0x02;
  }
  if (reg_event_type.extended_card_status)
  {
    tlv_data.mask |= 0x04;
  }
  if(reg_event_type.prov_session_close_event)
  {
    tlv_data.mask |= 0x08;
  }
  if (reg_event_type.physical_slots_status)
  {
    tlv_data.mask |= 0x10;
  }
  if(reg_event_type.sim_busy_events)
  {
    tlv_data.mask |= 0x20;
  }
  if(reg_event_type.reduced_card_status_events)
  {
    tlv_data.mask |= 0x40;
  }
  if(reg_event_type.recovery_complete_event)
  {
    tlv_data.mask |= 0x80;
  }
  if(reg_event_type.supply_voltage_events)
  {
    tlv_data.mask |= 0x100;
  }
  if(reg_event_type.reg_for_activation_status_event)
  {
    tlv_data.mask |= 0x200;
  }
  if(reg_event_type.simlock_remote_sfs_events)
  {
    tlv_data.mask |= 0x400;
  }
  if(reg_event_type.simlock_temp_unlock_status)
  {
    tlv_data.mask |= 0x800;
  }
  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               sizeof(tlv_data),
                               (void *)&tlv_data);
} /* qmi_uim_response_event_reg */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_REFRESH_EVENT()

  DESCRIPTION
    Composes the TLV content of refresh event

  PARAMETERS
    tlv_id              : id to be used for the TLV
    response_pptr       : output sdu
    session_type        : session type
    aid_ptr             : AID value
    refresh_evt_ptr     : refresh event

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_refresh_event
(
  byte                                 tlv_id,
  dsm_item_type **                     response_pptr,
  mmgsdi_session_type_enum_type        session_type,
  const mmgsdi_static_data_type *      aid_ptr,
  const mmgsdi_refresh_evt_info_type * refresh_evt_ptr
)
{
  uint16   i               = 0;
  uint16   num_files       = 0;
  uint16   tlv_len         = 0;
  uint8 *  tlv_data_ptr    = NULL;
  uint8*   curr_ptr        = NULL;
  boolean  ret_value       = FALSE;

  PACKED struct PACKED_POST
  {
    uint8  stage;
    uint8  mode;
    uint8  session_type;
    uint8  aid_len;
  } tlv_data_static_top;

  PACKED struct PACKED_POST
  {
    uint16 num_of_files;
  } tlv_data_static_bottom;

  PACKED struct PACKED_POST
  {
    uint16 file_id;
    uint8  path_len;
  } tlv_data_static_file;

  ASSERT(response_pptr);
  ASSERT(aid_ptr);
  ASSERT(refresh_evt_ptr);

  /* Calculate number of files, counting the number of 3F00 */
  num_files = 0;
  if (refresh_evt_ptr->refresh_files.file_path_ptr != NULL)
  {
    for (i = 0; i < refresh_evt_ptr->refresh_files.file_path_len - 1; i += 2)
    {
      if (refresh_evt_ptr->refresh_files.file_path_ptr[i] == 0x3F &&
          refresh_evt_ptr->refresh_files.file_path_ptr[i + 1] == 0x00)
      {
        num_files++;
      }
    }
  }

  /* Calculate maximum memory requirement */
  tlv_len = sizeof(tlv_data_static_top) +
            sizeof(tlv_data_static_bottom) +
            (uint16)aid_ptr->data_len +
            (uint16)(num_files *
               (sizeof(tlv_data_static_file) + ((MMGSDI_MAX_PATH_LEN - 1) * sizeof(uint16))));

  /* Allocate memory for TLV */
  tlv_data_ptr = (uint8*)uimqmi_malloc(tlv_len);
  if (tlv_data_ptr == NULL)
  {
    return FALSE;
  }
  memset(tlv_data_ptr, 0, tlv_len);

  /* Set pointer to start of buffer */
  curr_ptr = tlv_data_ptr;


  /* Stage */
  switch(refresh_evt_ptr->stage)
  {
    case MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT:
    case MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN:
      tlv_data_static_top.stage = 0;
      break;

    case MMGSDI_REFRESH_STAGE_START:
      tlv_data_static_top.stage = 1;
      break;

    case MMGSDI_REFRESH_STAGE_END_SUCCESS:
      tlv_data_static_top.stage = 2;
      break;

    case MMGSDI_REFRESH_STAGE_END_FAILED:
      tlv_data_static_top.stage = 3;
      break;

    default:
      /* Some stages are not sent to QMI clients */
      uimqmi_free(tlv_data_ptr);
      return FALSE;
  }

  /* Mode */
  switch(refresh_evt_ptr->mode)
  {
    case MMGSDI_REFRESH_RESET:
    case MMGSDI_REFRESH_RESET_AUTO:
      tlv_data_static_top.mode = 0;
      break;

    case MMGSDI_REFRESH_NAA_INIT:
      tlv_data_static_top.mode = 1;
      break;

    case MMGSDI_REFRESH_NAA_INIT_FCN:
      tlv_data_static_top.mode = 2;
      break;

    case MMGSDI_REFRESH_NAA_FCN:
      tlv_data_static_top.mode = 3;
      break;

    case MMGSDI_REFRESH_NAA_INIT_FULL_FCN:
      tlv_data_static_top.mode = 4;
      break;

    case MMGSDI_REFRESH_NAA_APP_RESET:
      tlv_data_static_top.mode = 5;
      break;

    case MMGSDI_REFRESH_3G_SESSION_RESET:
      tlv_data_static_top.mode = 6;
      break;

    default:
      /* Some modes are not sent to QMI clients */
      uimqmi_free(tlv_data_ptr);
      return FALSE;
  }

  /* Session type */
  switch(session_type)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
      tlv_data_static_top.session_type = 0;
      break;

    case MMGSDI_1X_PROV_PRI_SESSION:
      tlv_data_static_top.session_type = 1;
      break;

    case MMGSDI_GW_PROV_SEC_SESSION:
      tlv_data_static_top.session_type = 2;
      break;

    case MMGSDI_1X_PROV_SEC_SESSION:
      tlv_data_static_top.session_type = 3;
      break;

    case MMGSDI_NON_PROV_SESSION_SLOT_1:
      tlv_data_static_top.session_type = 4;
      break;

    case MMGSDI_NON_PROV_SESSION_SLOT_2:
      tlv_data_static_top.session_type = 5;
      break;

    case MMGSDI_CARD_SESSION_SLOT_1:
      tlv_data_static_top.session_type = 6;
      break;

    case MMGSDI_CARD_SESSION_SLOT_2:
      tlv_data_static_top.session_type = 7;
      break;

    case MMGSDI_GW_PROV_TER_SESSION:
      tlv_data_static_top.session_type = 10;
      break;

    case MMGSDI_1X_PROV_TER_SESSION:
      tlv_data_static_top.session_type = 11;
      break;

    case MMGSDI_NON_PROV_SESSION_SLOT_3:
      tlv_data_static_top.session_type = 16;
      break;

    case MMGSDI_CARD_SESSION_SLOT_3:
      tlv_data_static_top.session_type = 19;
      break;

    default:
      /* Some session types are not sent to QMI clients */
      uimqmi_free(tlv_data_ptr);
      return FALSE;
  }

  /* AID length */
  tlv_data_static_top.aid_len = (uint8)aid_ptr->data_len;

  /* Set number of files to zero initially */
  tlv_data_static_bottom.num_of_files = 0;

  /* Copy top part in the TLV */
  ASSERT(curr_ptr + sizeof(tlv_data_static_top) <= tlv_data_ptr + tlv_len);
  (void)memscpy(curr_ptr, tlv_len - (curr_ptr - tlv_data_ptr),
                &tlv_data_static_top, sizeof(tlv_data_static_top));
  curr_ptr += sizeof(tlv_data_static_top);

  /* Copy AID in the TLV */
  ASSERT(curr_ptr + aid_ptr->data_len <= tlv_data_ptr + tlv_len);
  (void)memscpy(curr_ptr, tlv_len - (curr_ptr - tlv_data_ptr),
                aid_ptr->data_ptr, (uint32)aid_ptr->data_len);
  curr_ptr += aid_ptr->data_len;

  /* Copy bottom part in the TLV */
  ASSERT(curr_ptr + sizeof(tlv_data_static_bottom) <= tlv_data_ptr + tlv_len);
  (void)memscpy(curr_ptr, tlv_len - (curr_ptr - tlv_data_ptr),
                &tlv_data_static_bottom, (uint32)sizeof(tlv_data_static_bottom));
  curr_ptr += sizeof(tlv_data_static_bottom);

  /* Loop thru all the files */
  if (refresh_evt_ptr->refresh_files.file_path_ptr != NULL)
  {
    boolean            error = FALSE;
    mmgsdi_path_type   file_path;

    memset(&file_path, 0, sizeof(file_path));

    for (i = 0; i < refresh_evt_ptr->refresh_files.file_path_len - 1; i += 2)
    {
      /* Check if there is the start of a new file */
      if (refresh_evt_ptr->refresh_files.file_path_ptr[i] == 0x3F &&
          refresh_evt_ptr->refresh_files.file_path_ptr[i + 1] == 0x00)
      {
        /* Store the previous file in the TLV */
        if (file_path.path_len > 0 && file_path.path_len <= MMGSDI_MAX_PATH_LEN && !error)
        {
          /* Increase total number of files */
          tlv_data_static_bottom.num_of_files++;

          /* Calculate file id and path len for TLV */
          tlv_data_static_file.file_id = file_path.path_buf[file_path.path_len - 1];
          tlv_data_static_file.path_len = (uint8)((file_path.path_len - 1) * sizeof(uint16));;

          /* Copy file id and path len in the TLV */
          ASSERT(curr_ptr + sizeof(tlv_data_static_file) <= tlv_data_ptr + tlv_len);
          (void)memscpy(curr_ptr, tlv_len - (curr_ptr - tlv_data_ptr),
                        &tlv_data_static_file, sizeof(tlv_data_static_file));
          curr_ptr += sizeof(tlv_data_static_file);

          /* Copy path in the TLV */
          if (file_path.path_len > 1)
          {
            ASSERT(curr_ptr + tlv_data_static_file.path_len <= tlv_data_ptr + tlv_len);
            (void)memscpy(curr_ptr, tlv_len - (curr_ptr - tlv_data_ptr),
                          &file_path.path_buf[0], tlv_data_static_file.path_len);
            curr_ptr += tlv_data_static_file.path_len;
          }
        }

        /* Reset file path to start a new file */
        memset(&file_path, 0, sizeof(file_path));
        error = FALSE;
      }

      /* Insert data in the current file */
      if (file_path.path_len < MMGSDI_MAX_PATH_LEN)
      {
        file_path.path_buf[file_path.path_len]  = (uint16)(refresh_evt_ptr->refresh_files.file_path_ptr[i] & 0xFF);
        file_path.path_buf[file_path.path_len]  = file_path.path_buf[file_path.path_len] << 8;
        file_path.path_buf[file_path.path_len] += (uint16)(refresh_evt_ptr->refresh_files.file_path_ptr[i + 1] & 0xFF);

        file_path.path_len += 1;
      }
      else
      {
        UIM_MSG_ERR_0("Path too long... will be discarded");
        error = TRUE;
      }
    }

    /* Insert the last file in the TLV */
    if (file_path.path_len > 0 && file_path.path_len <= MMGSDI_MAX_PATH_LEN && !error)
    {
      /* Increase total number of files */
      tlv_data_static_bottom.num_of_files++;

      /* Calculate file id and path len for TLV */
      tlv_data_static_file.file_id = file_path.path_buf[file_path.path_len - 1];
      tlv_data_static_file.path_len = (uint8)((file_path.path_len - 1) * sizeof(uint16));;

      /* Copy file id and path len in the TLV */
      ASSERT(curr_ptr + sizeof(tlv_data_static_file) <= tlv_data_ptr + tlv_len);
      (void)memscpy(curr_ptr, tlv_len - (curr_ptr - tlv_data_ptr),
                    &tlv_data_static_file, sizeof(tlv_data_static_file));
      curr_ptr += sizeof(tlv_data_static_file);

      /* Copy path in the TLV */
      if (file_path.path_len > 1)
      {
        ASSERT(curr_ptr + tlv_data_static_file.path_len <= tlv_data_ptr + tlv_len);
        (void)memscpy(curr_ptr, tlv_len - (curr_ptr - tlv_data_ptr),
                      &file_path.path_buf[0], tlv_data_static_file.path_len);
        curr_ptr += tlv_data_static_file.path_len;
      }
    }
  }

  /* We need to copy the static part of the TLV containing the
     number of files again into the TLV because the number of files
     might have changed */
  (void)memscpy((void*)(tlv_data_ptr + sizeof(tlv_data_static_top) + aid_ptr->data_len),
                 tlv_len - (sizeof(tlv_data_static_top) + aid_ptr->data_len),
         (void*)&tlv_data_static_bottom,
         sizeof(tlv_data_static_bottom));

  /* Update actual length of TLV */
  tlv_len = (uint16)(curr_ptr - tlv_data_ptr);

  ret_value = qmi_svc_put_param_tlv(response_pptr,
                                    tlv_id,
                                    tlv_len,
                                    (void *)tlv_data_ptr);

  uimqmi_free(tlv_data_ptr);
  return ret_value;
} /* qmi_uim_response_refresh_event */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_AUTHENTICATE_CAVE()

  DESCRIPTION
    Composes the TLV content of cave authentication

  PARAMETERS
    tlv_id              : id to be used for the TLV
    response_pptr       : output sdu
    cave_result         : result of CAVE algorithm

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_authenticate_cave
(
  byte                        tlv_id,
  dsm_item_type **            response_pptr,
  mmgsdi_cave_auth_resp_type  cave_result
)
{
  PACKED struct PACKED_POST
  {
    uint16 cave_len;
    uint8  cave_result[3];
  } tlv_data;

  ASSERT(response_pptr);

  tlv_data.cave_len       = sizeof(tlv_data.cave_result);
  tlv_data.cave_result[0] = (uint8)((cave_result >> 16) & 0xFF);
  tlv_data.cave_result[1] = (uint8)((cave_result >> 8) & 0xFF);
  tlv_data.cave_result[2] = (uint8)(cave_result & 0xFF);

  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               sizeof(tlv_data),
                               (void *)&tlv_data);
} /* qmi_uim_response_authenticate_cave */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_AUTHENTICATE_GSM_ALGO()

  DESCRIPTION
    Composes the TLV content of GSM authentication

  PARAMETERS
    tlv_id              : id to be used for the TLV
    response_pptr       : output sdu
    sres_ptr            : SRES value
    kc_ptr              : Kc value

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_authenticate_gsm_algo
(
  byte                        tlv_id,
  dsm_item_type **            response_pptr,
  const uint8 *               sres_ptr,
  const uint8 *               kc_ptr
)
{
  PACKED struct PACKED_POST
  {
    uint16 content_len;
    uint8  content[MMGSDI_GSM_ALGO_SRES_LEN + MMGSDI_GSM_ALGO_KC_LEN];
  } tlv_data;

  ASSERT(response_pptr);

  ASSERT(sres_ptr);
  ASSERT(kc_ptr);

  tlv_data.content_len = MMGSDI_GSM_ALGO_SRES_LEN + MMGSDI_GSM_ALGO_KC_LEN;

  /* Reconstruct the response from card */
  (void)memscpy((void*)tlv_data.content,
                sizeof(tlv_data.content),
         (void*)sres_ptr,
         MMGSDI_GSM_ALGO_SRES_LEN);
  (void)memscpy((void*)(tlv_data.content + MMGSDI_GSM_ALGO_SRES_LEN),
                sizeof(tlv_data.content) - MMGSDI_GSM_ALGO_SRES_LEN,
         (void*)kc_ptr,
         MMGSDI_GSM_ALGO_KC_LEN);

  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               sizeof(tlv_data),
                               (void *)&tlv_data);
} /* qmi_uim_response_authenticate_gsm_algo */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_AUTHENTICATE_GENERIC()

  DESCRIPTION
    Composes the TLV content of a generic authentication where entire
    response is passed as it is.

  PARAMETERS
    tlv_id              : id to be used for the TLV
    response_pptr       : output sdu
    auth_response       : result of authenticate

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_authenticate_generic
(
  byte                        tlv_id,
  dsm_item_type **            response_pptr,
  mmgsdi_data_type            auth_response
)
{
  boolean  ret_value    = FALSE;
  uint16   tlv_len      = 0;
  uint8  * tlv_data_ptr = NULL;

  ASSERT(response_pptr);

  /* It might be ok to have empty data. In this case, the TLV is not
     added, but we still return SUCCESS */
  if (auth_response.data_len == 0 ||
      auth_response.data_ptr == NULL)
  {
    return TRUE;
  }

  /* Allocate payload */
  tlv_data_ptr = uimqmi_malloc(sizeof(uint16) + auth_response.data_len);
  if (tlv_data_ptr == NULL)
  {
    return FALSE;
  }

  tlv_len = (uint16)auth_response.data_len;
  (void)memscpy(tlv_data_ptr, sizeof(uint16) + auth_response.data_len, &tlv_len, sizeof(uint16));
  (void)memscpy(tlv_data_ptr + sizeof(uint16), auth_response.data_len,
                auth_response.data_ptr, auth_response.data_len);

  ret_value = qmi_svc_put_param_tlv(response_pptr,
                                    tlv_id,
                                    (uint16)(sizeof(uint16) + auth_response.data_len),
                                    (void *)tlv_data_ptr);

  uimqmi_free(tlv_data_ptr);
  return ret_value;
} /* qmi_uim_response_authenticate_generic */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_AUTHENTICATE_ISIM()

  DESCRIPTION
    Composes the TLV content of ISIM authentication

  PARAMETERS
    tlv_id        : id to be used for the TLV
    response_pptr : output sdu
    status        : status
    auth_context  : auth context
    res           : RES value
    ck            : CK value
    ik            : IK value
    auts          : AUTS value

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_authenticate_isim
(
  byte                        tlv_id,
  dsm_item_type **            response_pptr,
  mmgsdi_return_enum_type     status,
  qmi_uim_auth_context_type   auth_context,
  mmgsdi_data_type            res,
  mmgsdi_data_type            ck,
  mmgsdi_data_type            ik,
  mmgsdi_data_type            auts
)
{
  boolean  ret_value    = FALSE;
  uint16   index        = 0;
  uint16   tlv_len      = 0;
  uint8 *  tlv_data_ptr = NULL;
  uint16   len_value    = 0;

  ASSERT(response_pptr);

  /* Compose payload */
  if (status == MMGSDI_SUCCESS)
  {
    ASSERT(res.data_ptr);

    /* Update response length based on the contexts requested:
       - ISIM AKA Authentication returns ck and ik in addition to res
       - HTTP Digest context returns res and ck
       - GBA Auth contexts ck & ik are not needed, so skip it altogether */
    switch (auth_context)
    {
      case QMI_UIM_AUTH_CONTEXT_GBA_BOOTSTRAPPING_SECURITY:
      case QMI_UIM_AUTH_CONTEXT_GBA_NAF_DERIVATION_SECURITY:
        len_value = (uint16)(res.data_len + 2);
        break;
      case QMI_UIM_AUTH_CONTEXT_HTTP_DIGEST_SECURITY:
        len_value = (uint16)(res.data_len + ck.data_len + 3);
        break;
      case QMI_UIM_AUTH_CONTEXT_IMS_AKA_SECURITY:
        len_value = (uint16)(res.data_len + ck.data_len + ik.data_len + 4);
        break;
      default:
        return FALSE;
    }

    tlv_len = len_value + 2;
    tlv_data_ptr = uimqmi_malloc(tlv_len);

    if (tlv_data_ptr == NULL)
    {
      return FALSE;
    }

    (void)memscpy(tlv_data_ptr, tlv_len, &len_value, sizeof(uint16));
    index += 2;

    /* Reconstruct the response from card. Success
       for all ISIM AUTH operations is 0xDB */
    tlv_data_ptr[index++] = 0xDB;

    tlv_data_ptr[index++] = (uint8)res.data_len;
    (void)memscpy((void*)(tlv_data_ptr + index), tlv_len - index,
                  (void*)res.data_ptr, (uint32)res.data_len);
    index += (uint16)res.data_len;

    /* Only ISIM AKA Authentication returns ck and ik in addition to res */
    if ((auth_context == QMI_UIM_AUTH_CONTEXT_IMS_AKA_SECURITY) ||
        (auth_context == QMI_UIM_AUTH_CONTEXT_HTTP_DIGEST_SECURITY))
    {
      tlv_data_ptr[index++] = (uint8)ck.data_len;
      if((ck.data_len > 0) && (ck.data_ptr != NULL))
      {
        (void)memscpy((void*)(tlv_data_ptr + index),
                      tlv_len - index,
                      (void*)ck.data_ptr,
                      (uint32)ck.data_len);
        index += (uint16)ck.data_len;
      }
    }
    if (auth_context == QMI_UIM_AUTH_CONTEXT_IMS_AKA_SECURITY)
    {
      tlv_data_ptr[index++] = (uint8)ik.data_len;
      if ((ik.data_len > 0) && (ik.data_ptr != NULL))
      {
        (void)memscpy((void*)(tlv_data_ptr + index), tlv_len - index,
                      (void*)ik.data_ptr, (uint32)ik.data_len);
        index += (uint16)ik.data_len;
      }
    }
  }
  else if (status == MMGSDI_AUTS_FAIL)
  {
    ASSERT(auts.data_ptr);

    len_value = (uint16)(auts.data_len + 2);

    tlv_len = len_value + 2;
    tlv_data_ptr = uimqmi_malloc(tlv_len);

    if (tlv_data_ptr == NULL)
    {
      return FALSE;
    }

    (void)memscpy(tlv_data_ptr, tlv_len,
                  &len_value, sizeof(uint16));
    index += 2;

    /* Reconstruct the response from card */
    tlv_data_ptr[index++] = 0xDC;

    tlv_data_ptr[index++] = (uint8)auts.data_len;
    (void)memscpy((void*)(tlv_data_ptr + index), tlv_len - index,
                  (void*)auts.data_ptr, (uint32)auts.data_len);
    index += (uint16)auts.data_len;
  }
  else
  {
    return FALSE;
  }

  ret_value = qmi_svc_put_param_tlv(response_pptr,
                                    tlv_id,
                                    tlv_len,
                                    (void *)tlv_data_ptr);

  uimqmi_free(tlv_data_ptr);

  return ret_value;
} /* qmi_uim_response_authenticate_isim */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_SERVICE_STATUS()

  DESCRIPTION
    Composes the TLV content of FDN,ACL service status

  PARAMETERS
    tlv_id        : id to be used for the TLV
    response_pptr : output sdu
    svc_available : Service is available or not
    svc_enabled   : Service is enabled or not

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_service_status
(
  byte                        tlv_id,
  dsm_item_type **            response_pptr,
  boolean                     svc_available,
  boolean                     svc_enabled
)
{
  PACKED struct PACKED_POST
  {
    uint8 svc_status;
  } tlv_data;

  ASSERT(response_pptr);

  if (svc_available)
  {
    if (svc_enabled)
    {
      tlv_data.svc_status = 2;
    }
    else
    {
      tlv_data.svc_status = 1;
    }
  }
  else
  {
    tlv_data.svc_status = 0;
  }

  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               sizeof(tlv_data),
                               (void *)&tlv_data);
} /* qmi_uim_response_service_status */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_BOOLEAN()

  DESCRIPTION
    Composes the TLV content from the value of boolean that is passed in

  PARAMETERS
    tlv_id        : id to be used for the TLV
    response_pptr : output sdu
    is_true       : boolean

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_boolean
(
  byte                        tlv_id,
  dsm_item_type **            response_pptr,
  boolean                     is_true
)
{
  PACKED struct PACKED_POST
  {
    uint8 is_true;
  } tlv_data;

  ASSERT(response_pptr);

  if (is_true)
  {
    tlv_data.is_true = 1;
  }
  else
  {
    tlv_data.is_true = 0;
  }

  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               sizeof(tlv_data),
                               (void *)&tlv_data);
} /* qmi_uim_response_boolean */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_LABEL()

  DESCRIPTION
    Composes the TLV content of application label

  PARAMETERS
    tlv_id        : id to be used for the TLV
    response_pptr : output sdu
    label_ptr     : label

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_label
(
  byte                            tlv_id,
  dsm_item_type **                response_pptr,
  const mmgsdi_static_data_type * label_ptr
)
{
  PACKED struct PACKED_POST
  {
    uint8 label_len;
    uint8 lable_value[MMGSDI_MAX_AID_LEN];
  } tlv_data;

  ASSERT(response_pptr);
  ASSERT(label_ptr);

  /* Check length */
  if (label_ptr->data_len > MMGSDI_MAX_AID_LEN)
  {
    return FALSE;
  }

  tlv_data.label_len = (uint8)label_ptr->data_len;
  (void)memscpy(tlv_data.lable_value,
                sizeof(tlv_data.lable_value),
         label_ptr->data_ptr,
         tlv_data.label_len);

  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               tlv_data.label_len + sizeof(uint8),
                               (void *)&tlv_data);
} /* qmi_uim_response_label */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_HIDDEN_KEY_STATUS()

  DESCRIPTION
    Composes the TLV content of hidden key status

  PARAMETERS
    tlv_id             : id to be used for the TLV
    response_pptr      : output sdu
    hidden_key_status  : hidden key status

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_hidden_key_status
(
  byte                            tlv_id,
  dsm_item_type **                response_pptr,
  mmgsdi_pin_status_enum_type     hidden_key_status
)
{
  PACKED struct PACKED_POST
  {
    uint8 hidden_key_status;
  } tlv_data;

  ASSERT(response_pptr);

  switch(hidden_key_status)
  {
    case MMGSDI_PIN_NOT_FOUND:
    case MMGSDI_PIN_STATUS_NOT_INITIALIZED:
      tlv_data.hidden_key_status = 0;
      break;

    case MMGSDI_PIN_ENABLED_NOT_VERIFIED:
      tlv_data.hidden_key_status = 1;
      break;

    case MMGSDI_PIN_ENABLED_VERIFIED:
      tlv_data.hidden_key_status = 2;
      break;

    case MMGSDI_PIN_DISABLED:
      tlv_data.hidden_key_status = 3;
      break;

    case MMGSDI_PIN_BLOCKED:
    case MMGSDI_PIN_PERM_BLOCKED:
    default:
      return FALSE;
  }

  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               sizeof(tlv_data),
                               (void *)&tlv_data);
} /* qmi_uim_response_hidden_key_status */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_INDEX_IN_DIR()

  DESCRIPTION
    Composes the TLV content of index in EF-DIR

  PARAMETERS
    tlv_id             : id to be used for the TLV
    response_pptr      : output sdu
    index_in_dir       : index in EF-DIR file

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_index_in_dir
(
  byte                            tlv_id,
  dsm_item_type **                response_pptr,
  uint8                           index_in_dir
)
{
  PACKED struct PACKED_POST
  {
    uint8 index_in_dir;
  } tlv_data;

  ASSERT(response_pptr);

  tlv_data.index_in_dir = index_in_dir;

  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               sizeof(tlv_data),
                               (void *)&tlv_data);
} /* qmi_uim_response_index_in_dir */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_ESN_STATUS()

  DESCRIPTION
    Composes the TLV content of ESN status

  PARAMETERS
    tlv_id        : id to be used for the TLV
    response_pptr : output sdu
    esn_status    : ESN is changed or not

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_esn_status
(
  byte                            tlv_id,
  dsm_item_type **                response_pptr,
  mmgsdi_esn_me_change_enum_type  esn_status
)
{
  PACKED struct PACKED_POST
  {
    uint8 esn_status;
  } tlv_data;

  ASSERT(response_pptr);

  switch(esn_status)
  {
    case MMGSDI_ESN_ME_NO_CHANGE:
      tlv_data.esn_status = 0;
      break;
    case MMGSDI_ESN_ME_CHANGE:
    case MMGSDI_ESN_ME_UPDATE_PENDING:
      tlv_data.esn_status = 1;
      break;
    default:
      /* Invalid value: return without adding any TLV */
      return TRUE;
  }

  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               sizeof(tlv_data),
                               (void *)&tlv_data);
} /* qmi_uim_response_esn_status */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_GET_CONFIGURATION_PERSO()

  DESCRIPTION
    Composes the TLV content of personalization in the
    configuration

  PARAMETERS
    tlv_id              : id to be used for the TLV
    response_pptr       : output sdu
    feature_ind         : status of features
    retries             : number of retries
    unblock_retries     : number of unblock retries

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_get_configuration_perso
(
  byte                                  tlv_id,
  dsm_item_type **                      response_pptr,
  const mmgsdi_perso_feature_ind_type * feature_ind_ptr,
  const mmgsdi_perso_dck_retries_type * retries_ptr,
  const mmgsdi_perso_dck_retries_type * unblock_retries_ptr
)
{
  uint16  tlv_len = 0;

  PACKED struct PACKED_POST
  {
    uint8    num_features;
    PACKED struct PACKED_POST
    {
      uint8  feature_id;
      uint8  verify_left;
      uint8  unblock_left;
    } feature[QMI_UIM_MAX_NUMBER_PERSO_FEATURES];
  } tlv_data;

  ASSERT(response_pptr);
  ASSERT(feature_ind_ptr);
  ASSERT(retries_ptr);
  ASSERT(unblock_retries_ptr);

  memset(&tlv_data, 0, sizeof(tlv_data));

  if (feature_ind_ptr->nw_ind_status)
  {
    ASSERT(tlv_data.num_features < QMI_UIM_MAX_NUMBER_PERSO_FEATURES);

    tlv_data.feature[tlv_data.num_features].feature_id   = 0;
    tlv_data.feature[tlv_data.num_features].verify_left  = (uint8)retries_ptr->nw_num_retries;
    tlv_data.feature[tlv_data.num_features].unblock_left = (uint8)unblock_retries_ptr->nw_num_retries;
    tlv_data.num_features++;
  }
  if (feature_ind_ptr->ns_ind_status)
  {
    ASSERT(tlv_data.num_features < QMI_UIM_MAX_NUMBER_PERSO_FEATURES);

    tlv_data.feature[tlv_data.num_features].feature_id   = 1;
    tlv_data.feature[tlv_data.num_features].verify_left  = (uint8)retries_ptr->ns_num_retries;
    tlv_data.feature[tlv_data.num_features].unblock_left = (uint8)unblock_retries_ptr->ns_num_retries;
    tlv_data.num_features++;
  }
  if (feature_ind_ptr->sp_ind_status)
  {
    ASSERT(tlv_data.num_features < QMI_UIM_MAX_NUMBER_PERSO_FEATURES);

    tlv_data.feature[tlv_data.num_features].feature_id   = 2;
    tlv_data.feature[tlv_data.num_features].verify_left  = (uint8)retries_ptr->sp_num_retries;
    tlv_data.feature[tlv_data.num_features].unblock_left = (uint8)unblock_retries_ptr->sp_num_retries;
    tlv_data.num_features++;
  }
  if (feature_ind_ptr->cp_ind_status)
  {
    ASSERT(tlv_data.num_features < QMI_UIM_MAX_NUMBER_PERSO_FEATURES);

    tlv_data.feature[tlv_data.num_features].feature_id   = 3;
    tlv_data.feature[tlv_data.num_features].verify_left  = (uint8)retries_ptr->cp_num_retries;
    tlv_data.feature[tlv_data.num_features].unblock_left = (uint8)unblock_retries_ptr->cp_num_retries;
    tlv_data.num_features++;
  }
  if (feature_ind_ptr->sim_ind_status)
  {
    ASSERT(tlv_data.num_features < QMI_UIM_MAX_NUMBER_PERSO_FEATURES);

    tlv_data.feature[tlv_data.num_features].feature_id   = 4;
    tlv_data.feature[tlv_data.num_features].verify_left  = (uint8)retries_ptr->sim_num_retries;
    tlv_data.feature[tlv_data.num_features].unblock_left = (uint8)unblock_retries_ptr->sim_num_retries;
    tlv_data.num_features++;
  }
  if (feature_ind_ptr->ruim_nw1_ind_status)
  {
    ASSERT(tlv_data.num_features < QMI_UIM_MAX_NUMBER_PERSO_FEATURES);

    tlv_data.feature[tlv_data.num_features].feature_id   = 5;
    tlv_data.feature[tlv_data.num_features].verify_left  = (uint8)retries_ptr->ruim_nw1_num_retries;
    tlv_data.feature[tlv_data.num_features].unblock_left = (uint8)unblock_retries_ptr->ruim_nw1_num_retries;
    tlv_data.num_features++;
  }
  if (feature_ind_ptr->ruim_nw2_ind_status)
  {
    ASSERT(tlv_data.num_features < QMI_UIM_MAX_NUMBER_PERSO_FEATURES);

    tlv_data.feature[tlv_data.num_features].feature_id   = 6;
    tlv_data.feature[tlv_data.num_features].verify_left  = (uint8)retries_ptr->ruim_nw2_num_retries;
    tlv_data.feature[tlv_data.num_features].unblock_left = (uint8)unblock_retries_ptr->ruim_nw2_num_retries;
    tlv_data.num_features++;
  }
  if (feature_ind_ptr->ruim_hrpd_ind_status)
  {
    ASSERT(tlv_data.num_features < QMI_UIM_MAX_NUMBER_PERSO_FEATURES);

    tlv_data.feature[tlv_data.num_features].feature_id   = 7;
    tlv_data.feature[tlv_data.num_features].verify_left  = (uint8)retries_ptr->ruim_hrpd_num_retries;
    tlv_data.feature[tlv_data.num_features].unblock_left = (uint8)unblock_retries_ptr->ruim_hrpd_num_retries;
    tlv_data.num_features++;
  }
  if (feature_ind_ptr->ruim_sp_ind_status)
  {
    ASSERT(tlv_data.num_features < QMI_UIM_MAX_NUMBER_PERSO_FEATURES);

    tlv_data.feature[tlv_data.num_features].feature_id   = 8;
    tlv_data.feature[tlv_data.num_features].verify_left  = (uint8)retries_ptr->ruim_sp_num_retries;
    tlv_data.feature[tlv_data.num_features].unblock_left = (uint8)unblock_retries_ptr->ruim_sp_num_retries;
    tlv_data.num_features++;
  }
  if (feature_ind_ptr->ruim_cp_ind_status)
  {
    ASSERT(tlv_data.num_features < QMI_UIM_MAX_NUMBER_PERSO_FEATURES);

    tlv_data.feature[tlv_data.num_features].feature_id   = 9;
    tlv_data.feature[tlv_data.num_features].verify_left  = (uint8)retries_ptr->ruim_cp_num_retries;
    tlv_data.feature[tlv_data.num_features].unblock_left = (uint8)unblock_retries_ptr->ruim_cp_num_retries;
    tlv_data.num_features++;
  }
  if (feature_ind_ptr->ruim_ruim_ind_status)
  {
    ASSERT(tlv_data.num_features < QMI_UIM_MAX_NUMBER_PERSO_FEATURES);

    tlv_data.feature[tlv_data.num_features].feature_id   = 10;
    tlv_data.feature[tlv_data.num_features].verify_left  = (uint8)retries_ptr->ruim_ruim_num_retries;
    tlv_data.feature[tlv_data.num_features].unblock_left = (uint8)unblock_retries_ptr->ruim_ruim_num_retries;
    tlv_data.num_features++;
  }

  /* Calculare the TLV len */
  tlv_len = sizeof(uint8) + (tlv_data.num_features * sizeof(uint8) * 3);

  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               tlv_len,
                               (void *)&tlv_data);

} /* qmi_uim_response_get_configuration_perso */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_CONVERT_SIMLOCK_CATEGORY()

  DESCRIPTION
    This function converts the simlock category to the values to be used
    in the TLV response

  PARAMETERS
    category            : Simlock category
    feature_ptr         : output parameter with feature

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_uim_response_convert_simlock_category
(
  simlock_category_enum_type      category,
  uint8                        *  feature_ptr
)
{
  qmi_error_e_type  result = QMI_ERR_NONE;

  ASSERT(feature_ptr);

  switch (category)
  {
    case SIMLOCK_CATEGORY_3GPP_NW:
      *feature_ptr = 0;
      break;
    case SIMLOCK_CATEGORY_3GPP_NS:
      *feature_ptr = 1;
      break;
    case SIMLOCK_CATEGORY_3GPP_SP:
      *feature_ptr = 2;
      break;
    case SIMLOCK_CATEGORY_3GPP_CP:
      *feature_ptr = 3;
      break;
    case SIMLOCK_CATEGORY_3GPP_SIM:
      *feature_ptr = 4;
      break;
    case SIMLOCK_CATEGORY_3GPP2_NW_TYPE1:
      *feature_ptr = 5;
      break;
    case SIMLOCK_CATEGORY_3GPP2_NW_TYPE2:
      *feature_ptr = 6;
      break;
    case SIMLOCK_CATEGORY_3GPP2_HRPD:
      *feature_ptr = 7;
      break;
    case SIMLOCK_CATEGORY_3GPP2_SP:
      *feature_ptr = 8;
      break;
    case SIMLOCK_CATEGORY_3GPP2_CP:
      *feature_ptr = 9;
      break;
    case SIMLOCK_CATEGORY_3GPP2_RUIM:
      *feature_ptr = 10;
      break;
    case SIMLOCK_CATEGORY_3GPP_SPN:
      *feature_ptr = 11;
      break;
    case SIMLOCK_CATEGORY_3GPP_ICCID:
      *feature_ptr = UIM_PERSO_FEATURE_GW_ICCID_V01;
      break;
    default:
      result = QMI_ERR_MALFORMED_MSG;
      break;
  }

  return result;
} /* qmi_uim_response_convert_simlock_category */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_GET_CONFIGURATION_SIMLOCK()

  DESCRIPTION
    Composes the TLV content of personalization in the
    configuration

  PARAMETERS
    tlv_id              : id to be used for the TLV
    response_pptr       : output sdu
    get_status_ptr      : feature status information

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_get_configuration_simlock
(
  byte                                      tlv_id,
  dsm_item_type **                          response_pptr,
  const simlock_get_status_msg_resp_type  * get_status_ptr
)
{
  uint16  tlv_len = 0;
  uint8   i       = 0;

  PACKED struct PACKED_POST
  {
    uint8    num_features;
    PACKED struct PACKED_POST
    {
      uint8  feature_id;
      uint8  verify_left;
      uint8  unblock_left;
    } feature[QMI_UIM_MAX_NUMBER_PERSO_FEATURES];
  } tlv_data;

  ASSERT(response_pptr);
  ASSERT(get_status_ptr);

  memset(&tlv_data, 0, sizeof(tlv_data));

  for (i = 0;
       i < SIMLOCK_CATEGORY_COUNT_MAX && i < QMI_UIM_MAX_NUMBER_PERSO_FEATURES;
       i++)
  {
    ASSERT(tlv_data.num_features < QMI_UIM_MAX_NUMBER_PERSO_FEATURES);

    /* Default to slot 1 lock information */
    if (get_status_ptr->lock_info[0].category_info[i].category_status == FALSE)
    {
      continue;
    }

    if(qmi_uim_response_convert_simlock_category(get_status_ptr->lock_info[0].category_info[i].category,
                                                 &tlv_data.feature[tlv_data.num_features].feature_id) != QMI_ERR_NONE)
    {
      return FALSE;
    }

    /* When num_retries_max is zero, that indicates infinite retries */
    if (get_status_ptr->lock_info[0].category_info[i].num_retries_max == 0)
    {
      tlv_data.feature[tlv_data.num_features].verify_left = QMI_UIM_SIMLOCK_MAX_RETRIES;
    }
    else if (get_status_ptr->lock_info[0].category_info[i].num_retries_max <=
               get_status_ptr->lock_info[0].category_info[i].curr_retries)
    {
      tlv_data.feature[tlv_data.num_features].verify_left = 0;
    }
    else
    {
      tlv_data.feature[tlv_data.num_features].verify_left =
        (uint8)(get_status_ptr->lock_info[0].category_info[i].num_retries_max -
                get_status_ptr->lock_info[0].category_info[i].curr_retries);
    }

    tlv_data.feature[tlv_data.num_features].unblock_left = 0;
    tlv_data.num_features++;
  }

  /* Calculare the TLV len */
  tlv_len = sizeof(uint8) + (tlv_data.num_features * sizeof(uint8) * 3);

  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               tlv_len,
                               (void *)&tlv_data);

} /* qmi_uim_response_get_configuration_simlock */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_TEMPORARY_UNLOCK_TIME_LEFT()

  DESCRIPTION
    Composes the TLV content of temporary depersonalization time left

  PARAMETERS
    tlv_id              : id to be used for the TLV
    response_pptr       : output sdu
    get_status_ptr      : feature status information

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_temporary_unlock_time_left
(
  byte                                      tlv_id,
  dsm_item_type **                          response_pptr,
  uint8                                     num_slots,
  const simlock_get_status_msg_resp_type  * get_status_ptr
)
{
  uint16    tlv_len                                  = 0;
  uint16    offset                                   = 0;
  uint8     i                                        = 0;
  uint8     j                                        = 0;
  uint8     slot_cnt                                 = 0;
  uint8     total_features                           = 0;
  uint8  *  tlv_data_ptr                             = NULL;
  uint8     features_in_slot[QMI_UIM_MAX_CARD_COUNT] = {0};
  boolean   ret_value                                = TRUE;

  PACKED struct PACKED_POST
  {
    uint8   feature_id;
    uint32  time_left;
  } feature;

  ASSERT(response_pptr);
  ASSERT(get_status_ptr);

  slot_cnt = MIN(num_slots, QMI_UIM_MAX_CARD_COUNT);

  /* calculate the number of features active per slot */
  for (i = 0;
       i < SIMLOCK_SLOT_COUNT_MAX && i < slot_cnt;
       i++)
  {
    for(j = 0;
        j < SIMLOCK_CATEGORY_COUNT_MAX && j < QMI_UIM_MAX_NUMBER_PERSO_FEATURES;
        j++)
    {
      if ((get_status_ptr->lock_info[i].category_info[j].category_status) &&
          (get_status_ptr->remaining_time.duration[i][j] > 0))
      {
        features_in_slot[i]++;
        total_features++;
      }
    }
  }

  /* calculate the total length */
  tlv_len = sizeof(uint8) +
            (slot_cnt * sizeof(uint8)) +
            (sizeof(feature) * total_features);

  tlv_data_ptr = (uint8*)uimqmi_malloc(tlv_len);
  if (tlv_data_ptr == NULL)
  {
    return FALSE;
  }

  /* Insert the number of slots in the TLV data */
  tlv_data_ptr[offset++] = slot_cnt;

  for (i = 0;
       i < SIMLOCK_SLOT_COUNT_MAX && i < slot_cnt;
       i++)
  {
    ASSERT(offset < tlv_len);

    /* Insert the number of active features per slot in the TLV data */
    tlv_data_ptr[offset++] = features_in_slot[i];

    /* Check the features that are active and insert the
       corresponding feature id and time left in the TLV data */
    for(j = 0;
        j < SIMLOCK_CATEGORY_COUNT_MAX && j < QMI_UIM_MAX_NUMBER_PERSO_FEATURES;
        j++)
    {
      if ((get_status_ptr->lock_info[i].category_info[j].category_status) &&
          (get_status_ptr->remaining_time.duration[i][j] > 0))
      {
        if(qmi_uim_response_convert_simlock_category(get_status_ptr->lock_info[i].category_info[j].category,
                                                     &feature.feature_id) != QMI_ERR_NONE)
        {
          uimqmi_free(tlv_data_ptr);
          return FALSE;
        }

        feature.time_left  = get_status_ptr->remaining_time.duration[i][j];

        ASSERT((offset + sizeof(feature)) <= tlv_len);
        (void)memscpy((void*)(tlv_data_ptr + offset),
                      tlv_len - offset,
                      &feature,
                      sizeof(feature));
        offset += sizeof(feature);
      }
    }
  }

  ret_value = qmi_svc_put_param_tlv(response_pptr,
                                    tlv_id,
                                    tlv_len,
                                    (void *)tlv_data_ptr);

  uimqmi_free(tlv_data_ptr);
  return ret_value;
} /* qmi_uim_response_temporary_unlock_time_left */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_GET_ALL_PROFILE_INFO()

  DESCRIPTION
    Composes the TLV content of profile information

  PARAMETERS
    tlv_id              : id to be used for the TLV
    response_pptr       : output sdu
    get_sim_profile_cnf_ptr : profile confirmation pointer

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_get_all_profile_info
(
  byte                                   tlv_id,
  dsm_item_type **                       response_pptr,
  const mmgsdi_get_sim_profile_cnf_type *get_sim_profile_cnf_ptr
)
{
  uint8   profile_index = 0;
  uint16  tlv_len       = 0;
  uint8*  tlv_data_ptr  = NULL;
  uint8*  curr_ptr      = NULL;
  boolean ret_value    = TRUE;
  uint8   num_profiles  = 0;

  PACKED struct PACKED_POST
  {
    uint8 num_profiles;
  }tlv_data_profiles;

  PACKED struct PACKED_POST
  {
    uint32 profile_id;
    uint32 profile_type;
  }tlv_data_profile_data;

  ASSERT(response_pptr);
  ASSERT(get_sim_profile_cnf_ptr);

  num_profiles = MIN(get_sim_profile_cnf_ptr->num_profile, MMGSDI_MAX_PROFILE);

  /* calculate memory requirement */
  tlv_len = sizeof(tlv_data_profiles);
  tlv_len += (sizeof(tlv_data_profile_data) * num_profiles);

  /* Allocate memory for TLV */
  tlv_data_ptr = (uint8*)modem_mem_alloc(tlv_len, MODEM_MEM_CLIENT_DATACOMMON);
  if (tlv_data_ptr == NULL)
  {
    return FALSE;
  }
  curr_ptr = tlv_data_ptr;

  tlv_data_profiles.num_profiles = num_profiles;

  /* Copy num of profiles in the TLV */
  ASSERT(curr_ptr + sizeof(tlv_data_profiles) <= tlv_data_ptr + tlv_len);
  memscpy(curr_ptr,
          tlv_len - (curr_ptr - tlv_data_ptr),
          &tlv_data_profiles,
          sizeof(tlv_data_profiles));
  curr_ptr += sizeof(tlv_data_profiles);

  for(profile_index = 0;
      profile_index < num_profiles;
      profile_index++)
  {
    tlv_data_profile_data.profile_id =
      get_sim_profile_cnf_ptr->profile_list[profile_index].profile_id;
    tlv_data_profile_data.profile_type =
      get_sim_profile_cnf_ptr->profile_list[profile_index].profile_type;

    /* Copy profile data in the TLV */
    ASSERT(curr_ptr + sizeof(tlv_data_profile_data) <= tlv_data_ptr + tlv_len);
    memscpy(curr_ptr,
            tlv_len - (curr_ptr - tlv_data_ptr),
            &tlv_data_profile_data,
            sizeof(tlv_data_profile_data));
    curr_ptr += sizeof(tlv_data_profile_data);
  }

  tlv_len = (uint16)(curr_ptr - tlv_data_ptr);

  ret_value = qmi_svc_put_param_tlv(response_pptr,
                                    tlv_id,
                                    tlv_len,
                                    (void *)tlv_data_ptr);

  modem_mem_free(tlv_data_ptr, MODEM_MEM_CLIENT_DATACOMMON);

  return ret_value;
} /* qmi_uim_response_get_all_profile_info */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_GET_ACTIVE_PROFILE_INFO()

  DESCRIPTION
    Composes the TLV content of active profile information

  PARAMETERS
    tlv_id              : id to be used for the TLV
    response_pptr       : output sdu
    get_sim_profile_cnf_ptr : profile confirmation pointer

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_get_active_profile_info
(
  byte                                   tlv_id,
  dsm_item_type **                       response_pptr,
  const mmgsdi_get_sim_profile_cnf_type *get_sim_profile_cnf_ptr
)
{
  PACKED struct PACKED_POST
  {
    uint32 active_profile_id;
    uint32 active_profile_type;
  }tlv_data_active_profile;

  ASSERT(response_pptr);
  ASSERT(get_sim_profile_cnf_ptr);

  tlv_data_active_profile.active_profile_id =
    get_sim_profile_cnf_ptr->active_profile.profile_id;
  tlv_data_active_profile.active_profile_type =
    get_sim_profile_cnf_ptr->active_profile.profile_type;

  return qmi_svc_put_param_tlv(response_pptr,
                                    tlv_id,
                               sizeof(tlv_data_active_profile),
                               (void *)&tlv_data_active_profile);
} /* qmi_uim_response_get_active_profile_info */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_SEND_APDU_RESULT()

  DESCRIPTION
    Composes the TLV content of send APDU result

  PARAMETERS
    tlv_id                  : id to be used for the TLV
    response_pptr           : output sdu
    apdu_data               : response APDU data from the card
    procedure_bytes         : procedure bytes, if applicable

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_send_apdu_result
(
  byte                          tlv_id,
  dsm_item_type **              response_pptr,
  mmgsdi_send_apdu_data_type    apdu_data,
  mmgsdi_sw_type                procedure_bytes
)
{
  boolean ret_value   = TRUE;
  uint16  tlv_len     = 0;
  uint8*  tlv_data    = NULL;
  uint16  temp_uint16 = 0;

  ASSERT(response_pptr);

  /* Data is not present... simply return TRUE.
     This TLV is optional and will be skipped */
  if (apdu_data.data_len == 0 || apdu_data.data_ptr == NULL)
  {
    return TRUE;
  }

  /* Find the total length */
  temp_uint16 = (uint16)apdu_data.data_len;
  if (procedure_bytes.present)
  {
    temp_uint16 += 2 * sizeof(uint8);
  }

  tlv_len = temp_uint16 + sizeof(uint16);

  /* Allocate buffer needed */
  tlv_data = (uint8*)uimqmi_malloc(tlv_len);
  if (tlv_data == NULL)
  {
    return FALSE;
  }

  /* Copy length */
  (void)memscpy((void*)tlv_data, tlv_len, &temp_uint16, sizeof(uint16));

  /* Copy APDU response from the card */
  (void)memscpy((void*)(tlv_data + sizeof(uint16)),
                tlv_len - sizeof(uint16),
         (void*)apdu_data.data_ptr,
         (uint32)apdu_data.data_len);

  /* Append procedure bytes if needed */
  if (procedure_bytes.present)
  {
    /* We append procedure bytes sw1 & sw2 for the subsequent partial data request,
       Example - <req_data_bytes> 0x61 0x<remaining_len> */
    uint16 offset            = sizeof(uint16) + (uint16)apdu_data.data_len;
    *(tlv_data + offset)     = procedure_bytes.sw1;
    *(tlv_data + offset + 1) = procedure_bytes.sw2;
  }

  ret_value = qmi_svc_put_param_tlv(response_pptr,
                                    tlv_id,
                                    tlv_len,
                                    (void *)tlv_data);

  uimqmi_free(tlv_data);

  return ret_value;
} /* qmi_uim_response_send_apdu_result */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_PROCEDURE_BYTES()

  DESCRIPTION
    Composes the TLV content of procedure bytes

  PARAMETERS
    tlv_id                  : id to be used for the TLV
    response_pptr           : output sdu
    procedure_bytes         : procedure bytes to be sent

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_procedure_bytes
(
  byte                          tlv_id,
  dsm_item_type **              response_pptr,
  mmgsdi_sw_type                procedure_bytes
)
{
  PACKED struct PACKED_POST
  {
    uint16  apdu_len;
    uint8   apdu_data[2 * sizeof(uint8)];
  } tlv_data;

  ASSERT(response_pptr);

  /* In this case, SW1 and SW2 should be there */
  if (!procedure_bytes.present)
  {
    return FALSE;
  }

  memset(&tlv_data, 0, sizeof(tlv_data));

  /* Update the TLV */
  tlv_data.apdu_len     = 2 * sizeof(uint8);
  tlv_data.apdu_data[0] = procedure_bytes.sw1;
  tlv_data.apdu_data[1] = procedure_bytes.sw2;

  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               sizeof(tlv_data),
                               (void *)&tlv_data);
} /* qmi_uim_response_procedure_bytes */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_SAP_STATE()

  DESCRIPTION
    Composes the TLV content of SAP state

  PARAMETERS
    tlv_id        : id to be used for the TLV
    response_pptr : output sdu
    is_enabled    : flag to indicate if SAP is enabled
    sap_state     : SAP state

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_sap_state
(
  byte                            tlv_id,
  dsm_item_type **                response_pptr,
  boolean                         is_enabled,
  mmgsdi_sap_state_enum_type      sap_state
)
{
  PACKED struct PACKED_POST
  {
    uint8 sap_state;
  } tlv_data;

  ASSERT(response_pptr);

  if (!is_enabled)
  {
    tlv_data.sap_state = 0;
  }
  else
  {
    switch(sap_state)
    {
      case MMGSDI_SAP_CONNECTING:
        tlv_data.sap_state = 1;
        break;
      case MMGSDI_SAP_CONNECTED:
        tlv_data.sap_state = 2;
        break;
      case MMGSDI_SAP_CONNECT_FAIL:
      case MMGSDI_SAP_DISCONNECT_FAIL:
        tlv_data.sap_state = 3;
        break;
      case MMGSDI_SAP_DISCONNECTING:
        tlv_data.sap_state = 4;
        break;
      case MMGSDI_SAP_DISCONNECTED:
        tlv_data.sap_state = 5;
        break;
      default:
        return FALSE;
    }
  }

  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               sizeof(tlv_data),
                               (void *)&tlv_data);
} /* qmi_uim_response_sap_state */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_SAP_EVENT()

  DESCRIPTION
    Composes the TLV content of SAP indication

  PARAMETERS
    tlv_id        : id to be used for the TLV
    response_pptr : output sdu
    is_enabled    : flag to indicate if SAP is enabled
    sap_state     : SAP state
    slot_idx      : slot subscript

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_sap_event
(
  byte                            tlv_id,
  dsm_item_type **                response_pptr,
  boolean                         is_enabled,
  mmgsdi_sap_state_enum_type      sap_state,
  mmgsdi_slot_id_enum_type        slot
)
{
  PACKED struct PACKED_POST
  {
    uint8 sap_state;
    uint8 slot;
  } tlv_data;

  ASSERT(response_pptr);

  /* Update sap state */
  if (!is_enabled)
  {
    tlv_data.sap_state = 0;
  }
  else
  {
    switch(sap_state)
    {
      case MMGSDI_SAP_CONNECTING:
        tlv_data.sap_state = 1;
        break;
      case MMGSDI_SAP_CONNECTED:
        tlv_data.sap_state = 2;
        break;
      case MMGSDI_SAP_CONNECT_FAIL:
      case MMGSDI_SAP_DISCONNECT_FAIL:
        tlv_data.sap_state = 3;
        break;
      case MMGSDI_SAP_DISCONNECTING:
        tlv_data.sap_state = 4;
        break;
      case MMGSDI_SAP_DISCONNECTED:
        tlv_data.sap_state = 5;
        break;
      default:
        return FALSE;
    }
  }

  /* Update slot state */
  switch(slot)
  {
    case MMGSDI_SLOT_1:
      tlv_data.slot = 1;
      break;
    case MMGSDI_SLOT_2:
      tlv_data.slot = 2;
      break;
    case MMGSDI_SLOT_3:
      tlv_data.slot = 3;
      break;
    default:
      return FALSE;
  }

  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               sizeof(tlv_data),
                               (void *)&tlv_data);
} /* qmi_uim_response_sap_event */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_GENERIC_PAYLOAD_8BIT()

  DESCRIPTION
    Composes the TLV content of variable data passed

  PARAMETERS
    tlv_id            : id to be used for the TLV
    response_pptr     : output sdu
    response_data     : response data from MMGSDI for the request
    payload_mandatory : specifies if response_data is to be checked for
                        validity

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_generic_payload_8bit
(
  byte                          tlv_id,
  dsm_item_type **              response_pptr,
  mmgsdi_data_type              response_data,
  boolean                       payload_mandatory
)
{
  boolean ret_value   = TRUE;
  uint16  tlv_len     = 0;
  uint8*  tlv_data    = NULL;
  uint8   temp_uint8  = 0;

  ASSERT(response_pptr);

  /* Check for validity of the payload data */
  if (response_data.data_len == 0 || response_data.data_ptr == NULL)
  {
    /* If payload is mandatory for the response of the called function,
       this is an error condition.
       Otherwise, return TRUE since this TLV is optional & will be skipped */
    if (payload_mandatory)
    {
      ret_value = FALSE;
    }
    return ret_value;
  }

  /* Length value is 1 byte for both of these TLV types */
  tlv_len = (uint8)response_data.data_len + sizeof(uint8);

  tlv_data = (uint8*)uimqmi_malloc(tlv_len);
  if (tlv_data == NULL)
  {
    return FALSE;
  }

  /* Copy length */
  temp_uint8 = (uint8)response_data.data_len;
  (void)memscpy((void*)tlv_data, tlv_len, &temp_uint8, sizeof(uint8));

  /* Copy response from the card */
  (void)memscpy((void*)(tlv_data + sizeof(uint8)),
          tlv_len - sizeof(uint8),
         (void*)response_data.data_ptr,
         temp_uint8);

  ret_value = qmi_svc_put_param_tlv(response_pptr,
                                    tlv_id,
                                    tlv_len,
                                    (void *)tlv_data);

  uimqmi_free(tlv_data);

  return ret_value;
} /* qmi_uim_response_generic_payload_8bit */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_GENERIC_PAYLOAD_16BIT()

  DESCRIPTION
    Composes the TLV content of variable data passed

  PARAMETERS
    tlv_id            : id to be used for the TLV
    response_pptr     : output sdu
    response_data     : response data from MMGSDI for the request
    payload_mandatory : specifies if response_data is to be checked for
                        validity

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_generic_payload_16bit
(
  byte                  tlv_id,
  dsm_item_type **      response_pptr,
  mmgsdi_data_type      response_data,
  boolean               payload_mandatory
)
{
  boolean ret_value   = TRUE;
  uint16  tlv_len     = 0;
  uint8*  tlv_data    = NULL;
  uint16  temp_uint16 = 0;

  ASSERT(response_pptr);

  /* Check for validity of the payload data */
  if (response_data.data_len == 0 || response_data.data_ptr == NULL)
  {
    /* If payload is mandatory for the response of the called function,
       this is an error condition.
       Otherwise, return TRUE since this TLV is optional & will be skipped */
    if (payload_mandatory)
    {
      ret_value = FALSE;
    }
    return ret_value;
  }

  /* Length value is 2 bytes for both of these TLV types */
  tlv_len = (uint16)response_data.data_len + sizeof(uint16);

  tlv_data = (uint8*)uimqmi_malloc(tlv_len);
  if (tlv_data == NULL)
  {
    return FALSE;
  }

  /* Copy length */
  temp_uint16 = (uint16)response_data.data_len;
  (void)memscpy((void*)tlv_data, tlv_len, &temp_uint16, sizeof(uint16));

  /* Copy content */
  (void)memscpy((void*)(tlv_data + sizeof(uint16)),
                tlv_len - sizeof(uint16),
                (void*)response_data.data_ptr,
                temp_uint16);

  ret_value = qmi_svc_put_param_tlv(response_pptr,
                                    tlv_id,
                                    tlv_len,
                                    (void *)tlv_data);

  uimqmi_free(tlv_data);
  return ret_value;
} /* qmi_uim_response_generic_payload_16bit */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_LOGICAL_CHANNEL()

  DESCRIPTION
    Composes the TLV content of logical channel

  PARAMETERS
    tlv_id             : id to be used for the TLV
    response_pptr      : output sdu
    logical_channel    : logical channel

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_logical_channel
(
  byte                            tlv_id,
  dsm_item_type **                response_pptr,
  uint8                           logical_channel
)
{
  PACKED struct PACKED_POST
  {
    uint8 logical_channel;
  } tlv_data;

  ASSERT(response_pptr);

  tlv_data.logical_channel = logical_channel;

  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               sizeof(tlv_data),
                               (void *)&tlv_data);
} /* qmi_uim_response_logical_channel */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_IND_TOKEN()

  DESCRIPTION
    Composes the TLV content of indication token

  PARAMETERS
    tlv_id             : id to be used for the TLV
    response_pptr      : output sdu
    token              : token info to be sent

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_ind_token
(
  byte                            tlv_id,
  dsm_item_type **                response_pptr,
  uint32                          token
)
{
  PACKED struct PACKED_POST
  {
    uint32 token;
  } tlv_data;

  ASSERT(response_pptr);

  tlv_data.token  = token;

  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               sizeof(tlv_data),
                               (void *)&tlv_data);
} /* qmi_uim_response_ind_token */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_HOT_SWAP_STATUS()

  DESCRIPTION
    Composes the hot swap switch status TLV content for get card status
    request

  PARAMETERS
    tlv_id                : id to be used for the TLV
    response_pptr         : output sdu
    extended_status       : if it is extended hot swap status
    hot_swap_status_len   : length of hot_swap_status_ptr array
    hot_swap_status_ptr   : array with status of hot swap switch for each slot

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_hot_swap_status
(
  byte                            tlv_id,
  dsm_item_type **                response_pptr,
  boolean                         extended_status,
  uint8                           hot_swap_status_len,
  uim_rt_status_enum *            hot_swap_status_ptr
)
{
  uint8              slot         = 0;
  uint8              tlv_len      = 0;
  uint8              hot_swap_len = 0;

  PACKED struct PACKED_POST
  {
    uint8 num_cards;
    uint8 hot_swap[QMI_UIM_MAX_CARD_COUNT];
  } tlv_data;

  uim_rt_status_enum status  = UIM_RT_STATUS_UNKNOWN;

  ASSERT(response_pptr);
  ASSERT(hot_swap_status_ptr);

  if (extended_status)
  {
    hot_swap_len = (QMI_UIM_MAX_CARD_COUNT < hot_swap_status_len) ?
                     QMI_UIM_MAX_CARD_COUNT : hot_swap_status_len;
  }
  else
  {
    hot_swap_len = (QMI_UIM_LEGACY_MAX_CARD_COUNT < hot_swap_status_len) ?
                     QMI_UIM_LEGACY_MAX_CARD_COUNT : hot_swap_status_len;
  }

  tlv_data.num_cards = hot_swap_len;
  for(slot = 0;
      slot < QMI_UIM_MAX_CARD_COUNT && slot < hot_swap_len;
      slot++)
  {
    status = hot_swap_status_ptr[slot];
    switch(status)
    {
      case UIM_RT_STATUS_CARD_PRESENT:
        tlv_data.hot_swap[slot] = 0x02;
        break;
      case UIM_RT_STATUS_CARD_ABSENT:
        tlv_data.hot_swap[slot] = 0x03;
        break;
      case UIM_RT_STATUS_NOT_SUPPORTED:
      default:
        tlv_data.hot_swap[slot] = 0x00;
        break;
    }
  }

  tlv_len = sizeof(uint8) + (sizeof(uint8) * hot_swap_len);

  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               tlv_len,
                               (void *)&tlv_data);
}/* qmi_uim_response_hot_swap_status */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_FILE_LENGTH()

  DESCRIPTION
    Composes the file length TLV

  PARAMETERS
    tlv_id             : id to be used for the TLV
    response_pptr      : output sdu
    file_length        : file length

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_file_length
(
  byte                            tlv_id,
  dsm_item_type **                response_pptr,
  uint16                          file_length
)
{
  PACKED struct PACKED_POST
  {
    uint16 file_length;
  } tlv_data;

  ASSERT(response_pptr);

  tlv_data.file_length = file_length;

  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               sizeof(tlv_data),
                               (void *)&tlv_data);
} /* qmi_uim_response_file_length */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_APDU_LONG_RESPONSE()

  DESCRIPTION
    Composes the TLV content of send APDU long response

  PARAMETERS
    tlv_id        : id to be used for the TLV
    response_pptr : output sdu
    token         : Token used in the subsequent indications
    total_len     : Total length of the response APDU from the card

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_apdu_long_response
(
  byte                          tlv_id,
  dsm_item_type **              response_pptr,
  uint32                        token,
  uint16                        total_len
)
{
  boolean   ret_value    = TRUE;

  PACKED struct PACKED_POST
  {
    uint16 total_len;
    uint32 token;
  } tlv_data;

  ASSERT(response_pptr);

  memset(&tlv_data, 0, sizeof(tlv_data));
  tlv_data.total_len = total_len;
  tlv_data.token     = token;

  ret_value = qmi_svc_put_param_tlv(response_pptr,
                                    tlv_id,
                                    sizeof(tlv_data),
                                    (void *)&tlv_data);

  return ret_value;
} /* qmi_uim_response_apdu_long_response */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_LONG_RESPONSE_CHUNK()

  DESCRIPTION
    Composes the TLV content of a long response chunk. This is currently used
    in cases of send APDU and read transparent indications.

  PARAMETERS
    tlv_id        : id to be used for the TLV
    response_pptr : output sdu
    token         : Token used in the previous long response
    total_len     : Total length of the response data from the card
    offset        : Offset of the current chunk of response data
    long_data     : One chunk of the long response data from the card

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_long_response_chunk
(
  byte                          tlv_id,
  dsm_item_type **              response_pptr,
  uint32                        token,
  uint16                        total_len,
  uint16                        offset,
  mmgsdi_data_type              long_data
)
{
  boolean   ret_value    = TRUE;
  uint16    tlv_len      = 0;
  uint8   * tlv_data_ptr = NULL;

  PACKED struct PACKED_POST
  {
    uint32 token;
    uint16 total_len;
    uint16 offset;
    uint16 data_len;
  } tlv_data;

  ASSERT(response_pptr);

  /* If data is not present... return FALSE.
     This TLV is mandatory and will be rejected */
  if (long_data.data_len == 0 || long_data.data_ptr == NULL)
  {
    return FALSE;
  }

  /* First update the static fields */
  memset(&tlv_data, 0, sizeof(tlv_data));
  tlv_data.token     = token;
  tlv_data.total_len = total_len;
  tlv_data.offset    = offset;
  tlv_data.data_len  = (uint16)long_data.data_len;

  tlv_len      = tlv_data.data_len + sizeof(tlv_data);
  tlv_data_ptr = (uint8*)uimqmi_malloc(tlv_len);
  if (tlv_data_ptr == NULL)
  {
    return FALSE;
  }

  memset(tlv_data_ptr, 0, tlv_len);

  /* Copy static fields of TLV */
  (void)memscpy((void*)tlv_data_ptr, tlv_len, &tlv_data, sizeof(tlv_data));

  /* Copy APDU data */
  (void)memscpy((void*)(tlv_data_ptr+sizeof(tlv_data)),
                tlv_len - sizeof(tlv_data),
                (void*)long_data.data_ptr,
                (uint32)tlv_data.data_len);

  ret_value = qmi_svc_put_param_tlv(response_pptr,
                                    tlv_id,
                                    tlv_len,
                                    (void *)tlv_data_ptr);

  uimqmi_free(tlv_data_ptr);

  return ret_value;
} /* qmi_uim_response_long_response_chunk */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_CARD_STATUS_VALIDITY()

  DESCRIPTION
    Composes the card status validity TLV content for get card status
    request

  PARAMETERS
    tlv_id          : id to be used for the TLV
    response_pptr   : output sdu
    validity_len    : length of validity status array
    validity_ptr    : pointer to validity status array

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_card_status_validity
(
  byte                            tlv_id,
  dsm_item_type **                response_pptr,
  uint8                           validity_len,
  boolean                       * validity_ptr
)
{
  uint8              slot     = 0;
  uint8              tlv_len  = 0;
  boolean            skip_tlv = TRUE;

  PACKED struct PACKED_POST
  {
    uint8 num_cards;
    uint8 card_status_valid[QMI_UIM_MAX_CARD_COUNT];
  } tlv_data;

  ASSERT(response_pptr);

  /* Skip TLV on error input */
  if ((validity_ptr == NULL) ||
      (validity_len == 0) ||
      (validity_len > QMI_UIM_MAX_CARD_COUNT))
  {
    return FALSE;
  }

  /* Skip this TLV in case status in all slots are determined to be valid */
  for(slot = 0; slot < validity_len; slot++)
  {
    if (!validity_ptr[slot])
    {
      skip_tlv = FALSE;
      break;
    }
  }

  if (skip_tlv)
  {
    return TRUE;
  }

  memset(&tlv_data, 0, sizeof(tlv_data));

  tlv_data.num_cards = validity_len;
  tlv_len = sizeof(uint8) + sizeof(uint8)*validity_len;

  for(slot = 0; slot < QMI_UIM_MAX_CARD_COUNT && slot < validity_len; slot++)
  {
    tlv_data.card_status_valid[slot] = (validity_ptr[slot]) ? 1 : 0;
  }

  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               tlv_len,
                               (void *)&tlv_data);
} /* qmi_uim_response_card_status_validity */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_SIM_BUSY()

  DESCRIPTION
    Composes the sim busy TLV content for get card status request.

  PARAMETERS
    tlv_id          : id to be used for the TLV
    response_pptr   : output sdu
    card_state      : structure containing num of cards and sim busy status

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_sim_busy
(
  byte                            tlv_id,
  dsm_item_type **                response_pptr,
  const qmi_uim_card_state      * card_state
)
{
  uint8          slot         = 0;
  uint8          tlv_len      = 0;

  PACKED struct PACKED_POST
  {
    uint8    num_cards;
    uint8    busy_status[QMI_UIM_MAX_CARD_COUNT];
  } tlv_data;

  ASSERT(response_pptr);
  ASSERT(card_state);

  memset(&tlv_data, 0, sizeof(tlv_data));

  tlv_data.num_cards = card_state->num_slots;

  tlv_len = sizeof(uint8);

  for(slot = 0; slot < QMI_UIM_MAX_CARD_COUNT &&
                slot < card_state->num_slots ; slot++)
  {
    tlv_data.busy_status[slot] = ( card_state->card[slot]->sim_busy ) ? 1 : 0;
    tlv_len += sizeof(uint8);
  }

  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               tlv_len,
                               (void *)&tlv_data);
} /* qmi_uim_response_sim_busy */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_SLOT()

  DESCRIPTION
    Composes the TLV content for Slot ID

  PARAMETERS
    slot_id : MMGSDI slot information

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_slot
(
  byte                        tlv_id,
  dsm_item_type            ** response_pptr,
  mmgsdi_slot_id_enum_type    slot
)
{
  PACKED struct PACKED_POST
  {
    uint8 slot;
  } tlv_data;

  ASSERT(response_pptr);

  /* Update slot ID */
  switch(slot)
  {
    case MMGSDI_SLOT_1:
      tlv_data.slot = 1;
      break;
    case MMGSDI_SLOT_2:
      tlv_data.slot = 2;
      break;
    case MMGSDI_SLOT_3:
      tlv_data.slot = 3;
      break;
    default:
      return FALSE;
  }

  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               sizeof(tlv_data),
                               (void *)&tlv_data);
} /* qmi_uim_response_slot */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_ACTIVATION_STATUS()

  DESCRIPTION
    Composes the TLV content for card activation status

  PARAMETERS
    tlv_id            : id to be used for the TLV
    response_pptr     : output sdu
    gstk_otasp_status : contains GSTK OTASP activation status

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_activation_status
(
  byte                           tlv_id,
  dsm_item_type               ** response_pptr,
  gstk_otasp_act_status_enum_type    gstk_otasp_status
)
{
  PACKED struct PACKED_POST
  {
    uint32 otasp_status;
  } tlv_data;

  ASSERT(response_pptr);

  /* Update OTASP status */
  switch(gstk_otasp_status)
  {
    case GSTK_OTASP_ACT_STATUS_START:
      tlv_data.otasp_status = 0;
      break;
    case GSTK_OTASP_ACT_STATUS_SUCCESS:
      tlv_data.otasp_status = 1;
      break;
    case GSTK_OTASP_ACT_STATUS_PERM_FAIL:
      tlv_data.otasp_status = 2;
      break;
    default:
      return FALSE;
  }

  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               sizeof(tlv_data),
                               (void *)&tlv_data);
} /* qmi_uim_response_activation_status */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_AID()

  DESCRIPTION
    Composes the TLV content for AID

  PARAMETERS
    aid_ptr : AID information

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_aid
(
  byte                             tlv_id,
  dsm_item_type                 ** response_pptr,
  const mmgsdi_static_data_type  * aid_ptr
)
{
  PACKED struct PACKED_POST
  {
    uint8 aid_len;
    uint8 aid_value[MMGSDI_MAX_AID_LEN];
  } tlv_data;

  ASSERT(response_pptr);
  ASSERT(aid_ptr);

  /* Check for AID length validity */
  if ((aid_ptr->data_len < 0) ||
      (aid_ptr->data_len > MMGSDI_MAX_AID_LEN))
  {
    return FALSE;
  }

  tlv_data.aid_len = (uint8)aid_ptr->data_len;
  (void)memscpy(tlv_data.aid_value,
                sizeof(tlv_data.aid_value),
         aid_ptr->data_ptr,
         tlv_data.aid_len);

  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               tlv_data.aid_len + sizeof(uint8),
                               (void *)&tlv_data);
} /* qmi_uim_response_aid */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_FILE_STATUS()

  DESCRIPTION
    Composes the TLV content for file status

  PARAMETERS
    tlv_id           : TLV id to be sent
    response_pptr    : Pointer to response pointer
    file_attrib_ptr  : Pointer to file attributes response

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_file_status
(
  byte                                 tlv_id,
  dsm_item_type                     ** response_pptr,
  const mmgsdi_file_attributes_type  * file_attrib_ptr
)
{
  boolean file_invalidated = FALSE;

  PACKED struct PACKED_POST
  {
    uint32 file_status;
  } tlv_data;

  ASSERT(response_pptr);
  ASSERT(file_attrib_ptr);

  switch (file_attrib_ptr->file_type)
  {
     case MMGSDI_LINEAR_FIXED_FILE:
       file_invalidated =
         file_attrib_ptr->file_info.linear_fixed_file.file_status.file_invalidated;
       break;
     case MMGSDI_CYCLIC_FILE:
       file_invalidated =
         file_attrib_ptr->file_info.cyclic_file.file_status.file_invalidated;
       break;
     case MMGSDI_TRANSPARENT_FILE:
       file_invalidated =
         file_attrib_ptr->file_info.transparent_file.file_status.file_invalidated;
       break;
     default:
       return FALSE;
  }

  /* Update the value */
  tlv_data.file_status = file_invalidated ? 0: 1;

  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               sizeof(tlv_data),
                               (void *)&tlv_data);
} /* qmi_uim_response_file_status */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_SESSION_TYPE()

  DESCRIPTION
    Composes the TLV content for Session type

  PARAMETERS
    tlv_id           : TLV id to be sent
    response_pptr    : Pointer to response pointer
    session_type     : MMGSDI session type

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_session_type
(
  byte                            tlv_id,
  dsm_item_type                ** response_pptr,
  mmgsdi_session_type_enum_type   session_type
)
{
  PACKED struct PACKED_POST
  {
    uint8 session_type;
  } tlv_data;

  ASSERT(response_pptr);

  switch(session_type)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
      tlv_data.session_type = 0;
      break;
    case MMGSDI_1X_PROV_PRI_SESSION:
      tlv_data.session_type = 1;
      break;
    case MMGSDI_GW_PROV_SEC_SESSION:
      tlv_data.session_type = 2;
      break;
    case MMGSDI_1X_PROV_SEC_SESSION:
      tlv_data.session_type = 3;
      break;
    case MMGSDI_GW_PROV_TER_SESSION:
      tlv_data.session_type = 10;
      break;
    case MMGSDI_1X_PROV_TER_SESSION:
      tlv_data.session_type = 11;
      break;
    default:
      return FALSE;
  }

  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               sizeof(tlv_data),
                               (void *)&tlv_data);
} /* qmi_uim_response_session_type */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_SESSION_CLOSED_CAUSE()

  DESCRIPTION
    Composes the TLV content for session closure cause

  PARAMETERS
    tlv_id           : TLV id to be sent
    response_pptr    : Pointer to response pointer
    cause            : Session close cause

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_session_closed_cause
(
  byte                                    tlv_id,
  dsm_item_type                        ** response_pptr,
  mmgsdi_session_closed_cause_enum_type   cause
)
{
  PACKED struct PACKED_POST
  {
    uint32 cause;
  } tlv_data;

  ASSERT(response_pptr);

  switch(cause)
  {
    case MMGSDI_SESSION_CLOSED_CAUSE_CLIENT_REQUEST:
    case MMGSDI_SESSION_CLOSED_CAUSE_MISMATCH_FOUND:
      tlv_data.cause = 1;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_CARD_ERROR:
      tlv_data.cause = 2;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_CARD_POWER_DOWN:
      tlv_data.cause = 3;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_CARD_REMOVED:
      tlv_data.cause = 4;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_REFRESH:
      tlv_data.cause = 5;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_PIN_STATUS_FAIL:
      tlv_data.cause = 6;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_RECOVERY:
      tlv_data.cause = 7;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_FDN_SRV_ENABLED_WITH_NO_FDN_SUPPORT:
      tlv_data.cause = 8;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_PERSO_OPERATION_FAIL:
      tlv_data.cause = 9;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_FILE_CONTENT_INVALID_IMSI:
    case MMGSDI_SESSION_CLOSED_CAUSE_FILE_CONTENT_INVALID_ACC:
    case MMGSDI_SESSION_CLOSED_CAUSE_FILE_CONTENT_INVALID_PRL:
      tlv_data.cause = 10;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_AD:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_IMSI:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_ACC:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_HPPLMN:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_LOCI:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_PSLOCI:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_KEYS:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_FPLMN:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_START_HFN:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_THRESHOLD:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_KC:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_PHASE:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_CST:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_RUIM_ID:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_SERVICE_PREF:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_IMSI_M:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_IMSI_T:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_ACCOLC:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_DIST_BASED_REGN_IND:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_SYS_REGN_IND:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_OTAPA_SPC_ENABLE:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_NAM_LOCK:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_HOME_SID_NID:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_ZONE_BASED_REGN_IND:
      tlv_data.cause = 11;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_UNKNOWN:
    default:
      tlv_data.cause = 0;
      break;
  }

  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               sizeof(tlv_data),
                               (void *)&tlv_data);
} /* qmi_uim_response_session_closed_cause */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_SESSION_CLOSED_FILE_ID()

  DESCRIPTION
    Composes the TLV content for the File ID

  PARAMETERS
    tlv_id           : TLV id to be sent
    response_pptr    : Pointer to response pointer
    cause            : Session close cause

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_session_closed_file_id
(
  byte                                   tlv_id,
  dsm_item_type                       ** response_pptr,
  mmgsdi_session_closed_cause_enum_type  cause
)
{
  PACKED struct PACKED_POST
  {
    uint16 file_id;
  } tlv_data;

  ASSERT(response_pptr);

  switch (cause)
  {
     case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_AD:
       tlv_data.file_id = 0x6FAD;
       break;
     case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_IMSI:
     case MMGSDI_SESSION_CLOSED_CAUSE_FILE_CONTENT_INVALID_IMSI:
       tlv_data.file_id = 0x6F07;
       break;
     case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_ACC:
     case MMGSDI_SESSION_CLOSED_CAUSE_FILE_CONTENT_INVALID_ACC:
       tlv_data.file_id = 0x6F78;
       break;
     case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_HPPLMN:
       tlv_data.file_id = 0x6F31;
       break;
     case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_LOCI:
       tlv_data.file_id = 0x6F7E;
       break;
     case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_PSLOCI:
       tlv_data.file_id = 0x6F73;
       break;
     case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_KEYS:
       tlv_data.file_id = 0x6F08;
       break;
     case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_FPLMN:
       tlv_data.file_id = 0x6F7B;
       break;
     case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_START_HFN:
       tlv_data.file_id = 0x6F5B;
       break;
     case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_THRESHOLD:
       tlv_data.file_id = 0x6F5C;
       break;
     case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_PHASE:
       tlv_data.file_id = 0x6FAE;
       break;
     case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_CST:
       tlv_data.file_id = 0x6F32;
       break;
     case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_RUIM_ID:
       tlv_data.file_id = 0x6F31;
       break;
     case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_SERVICE_PREF:
       tlv_data.file_id = 0x6F37;
       break;
     case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_IMSI_M:
       tlv_data.file_id = 0x6F22;
       break;
     case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_IMSI_T:
       tlv_data.file_id = 0x6F23;
       break;
     case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_ACCOLC:
       tlv_data.file_id = 0x6F2C;
       break;
     case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_DIST_BASED_REGN_IND:
       tlv_data.file_id = 0x6F2B;
       break;
     case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_SYS_REGN_IND:
       tlv_data.file_id = 0x6F2A;
       break;
     case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_OTAPA_SPC_ENABLE:
       tlv_data.file_id = 0x6F34;
       break;
     case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_NAM_LOCK:
       tlv_data.file_id = 0x6F35;
       break;
     case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_HOME_SID_NID:
       tlv_data.file_id = 0x6F28;
       break;
     case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_ZONE_BASED_REGN_IND:
       tlv_data.file_id = 0x6F29;
       break;
     case MMGSDI_SESSION_CLOSED_CAUSE_FILE_CONTENT_INVALID_PRL:
       tlv_data.file_id = 0x6F30;
       break;
     default:
       return TRUE;
  }

  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               sizeof(tlv_data),
                               (void *)&tlv_data);
} /* qmi_uim_response_session_closed_file_id */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_PHYSICAL_SLOTS_STATUS()

  DESCRIPTION
    Composes the TLV content for slots status indication

  PARAMETERS
    tlv_id             : id to be used for the TLV
    response_pptr      : output sdu
    slot_status_ptr    : Status of physical slots
    num_slots          : number of physical slots

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_physical_slots_status
(
  byte                               tlv_id,
  dsm_item_type                   ** response_pptr,
  const uim_phy_slot_status_type   * slot_status_ptr,
  uint8                              num_slots
)
{
  uint8   i              = 0;
  uint16  tlv_len        = 0;
  uint8*  tlv_data_ptr   = NULL;
  uint8*  curr_ptr       = NULL;
  boolean ret_value      = FALSE;

  PACKED struct PACKED_POST
  {
    uint32 card_hotswap_status;
    uint32 card_state;
    uint8  logical_slot;
    uint8  iccid_len;
  } tlv_top_data;

  ASSERT(response_pptr);
  ASSERT(slot_status_ptr);

  /* calculate memory requirement */
  tlv_len = sizeof(num_slots);

  for (i = 0; i < num_slots && i < UIM_NUM_PHY_SLOTS; i++)
  {
    /* Fail if iccid length is greater than max iccid size, that is, 10 */
    if(slot_status_ptr[i].iccid_len > QMI_UIM_MAX_ICCID_SIZE)
    {
      return FALSE;
    }
    tlv_len += sizeof(tlv_top_data) + (uint16) slot_status_ptr[i].iccid_len;
  }

  /* Memory requirement calculation is done. Lets proceed with allocating memory */
  tlv_data_ptr = (uint8*)modem_mem_alloc(tlv_len, MODEM_MEM_CLIENT_UIM);
  if (tlv_data_ptr == NULL)
  {
    return FALSE;
  }
  curr_ptr = tlv_data_ptr;

  /* Copy number of physical slots in the TLV */
  ASSERT(curr_ptr + sizeof(num_slots) <= tlv_data_ptr + tlv_len);
  (void)memscpy(curr_ptr,
                tlv_len - (curr_ptr - tlv_data_ptr),
                (void*)&num_slots,
                sizeof(num_slots));
  curr_ptr += sizeof(num_slots);

  /* Copy physical slots status in the TLV */
  for (i = 0; i < num_slots && i < UIM_NUM_PHY_SLOTS; i++)
  {
    memset(&tlv_top_data, 0, sizeof(tlv_top_data));

    switch (slot_status_ptr[i].card_presence_status)
    {
      case UIM_CARD_REMOVED:
        tlv_top_data.card_hotswap_status = 1;
        break;
      case UIM_CARD_INSERTED:
        tlv_top_data.card_hotswap_status = 2;
        break;
      case UIM_CARD_UNCHANGED:
      case UIM_CARD_STATUS_UNKNOWN:
      default:
        UIM_MSG_ERR_1("Invalid card hotswap status 0x%x",
                      slot_status_ptr[i].card_presence_status);
        tlv_top_data.card_hotswap_status = 0;
        break;
    }

    switch (slot_status_ptr[i].activity_status)
    {
      case UIM_SLOT_STATE_INACTIVE:
        tlv_top_data.card_state = 0;
        break;
      case UIM_SLOT_STATE_ACTIVE:
        tlv_top_data.card_state = 1;
        break;
      default:
        UIM_MSG_ERR_1("Invalid card state 0x%x",
                      slot_status_ptr[i].activity_status);
        tlv_top_data.card_state = 1;
        break;
    }

    switch (slot_status_ptr[i].logical_slot)
    {
      case UIM_SLOT_1:
        tlv_top_data.logical_slot = 1;
        break;
      case UIM_SLOT_2:
        tlv_top_data.logical_slot = 2;
        break;
      case UIM_SLOT_NONE:
      default:
        UIM_MSG_ERR_1("Invalid logical slot 0x%x",
                      slot_status_ptr[i].logical_slot);
        ret_value = FALSE;
        goto return_label;
    }

    tlv_top_data.iccid_len = slot_status_ptr[i].iccid_len;

    ASSERT(curr_ptr + sizeof(tlv_top_data) <= tlv_data_ptr + tlv_len);
    (void)memscpy(curr_ptr,
                  tlv_len - (curr_ptr - tlv_data_ptr),
                  (void*)&tlv_top_data,
                  sizeof(tlv_top_data));
    curr_ptr += sizeof(tlv_top_data);

    if(tlv_top_data.iccid_len > 0)
    {
      /* Copy ICCID (if valid) of the physical slot */
      ASSERT(curr_ptr + tlv_top_data.iccid_len <= tlv_data_ptr + tlv_len);
      (void)memscpy(curr_ptr,
                    tlv_len - (curr_ptr - tlv_data_ptr),
                    (void*)slot_status_ptr[i].iccid,
                    tlv_top_data.iccid_len);
      curr_ptr += tlv_top_data.iccid_len;
    }
  }

  ret_value = qmi_svc_put_param_tlv(response_pptr,
                                    tlv_id,
                                    tlv_len,
                                    (void *)tlv_data_ptr);

return_label:
  modem_mem_free(tlv_data_ptr, MODEM_MEM_CLIENT_UIM);

  return ret_value;
} /* qmi_uim_response_physical_slots_status */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_SE13_TABLE_RELEASE_DATE()

  DESCRIPTION
    Composes the TLV content for the SE13 table release date

  PARAMETERS
    tlv_id                          : id to be used for the TLV
    response_pptr                   : output sdu
    se13_table_release_info_ptr     : contains se13 release date and source

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_se13_table_release_date
(
  byte                                     tlv_id,
  dsm_item_type                         ** response_pptr,
  const mmgsdi_se13_table_details_info   * se13_table_details_info_ptr
)
{
  PACKED struct PACKED_POST
  {
    uint8   se13_release_month;
    uint8   se13_release_day;
    uint16  se13_release_year;
  } tlv_data;

  ASSERT(response_pptr);
  ASSERT(se13_table_details_info_ptr);

  tlv_data.se13_release_month = se13_table_details_info_ptr->se13_date.mmgsdi_se13_release_month;
  tlv_data.se13_release_day = se13_table_details_info_ptr->se13_date.mmgsdi_se13_release_day;
  tlv_data.se13_release_year = se13_table_details_info_ptr->se13_date.mmgsdi_se13_release_year;

  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               sizeof(tlv_data),
                               (void *)&tlv_data);
} /* qmi_uim_response_se13_table_release_date */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_SE13_TABLE_RELEASE_SOURCE()

  DESCRIPTION
    Composes the TLV content for the SE.13 table source

  PARAMETERS
    tlv_id                          : id to be used for the TLV
    response_pptr                   : output sdu
    se13_table_release_info_ptr     : contains se13 release date and source

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_se13_table_release_source
(
  byte                                      tlv_id,
  dsm_item_type                          ** response_pptr,
  const mmgsdi_se13_table_details_info    * se13_table_details_info_ptr
)
{
  PACKED struct PACKED_POST
  {
    uint32 source;
  } tlv_data;

  ASSERT(response_pptr);
  ASSERT(se13_table_details_info_ptr);

  switch(se13_table_details_info_ptr->se13_source)
  {
    case MMGSDI_SE13_HARDCODED_SOURCE:
      tlv_data.source = 1;
      break;
    case MMGSDI_SE13_EFS_SOURCE:
      tlv_data.source = 2;
      break;
    default:
      return FALSE;
  }

  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               sizeof(tlv_data),
                               (void *)&tlv_data);
} /* qmi_uim_response_se13_table_release_source */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_INCREASE_RESULT()

  DESCRIPTION
    Composes the TLV content of read result

  PARAMETERS
    tlv_id          : id to be used for the TLV
    response_pptr   : output sdu
    increase_result : increase data from the card

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_increase_result
(
  byte                         tlv_id,
  dsm_item_type             ** response_pptr,
  mmgsdi_increase_data_type    increase_data
)
{
  boolean ret_value    = TRUE;
  uint8   tlv_len      = 0;
  uint8*  tlv_data     = NULL;
  uint8   temp_uint8   = 0;

  ASSERT(response_pptr);

  /* Data is not present... simply return TRUE.
     This TLV is optional and will be skipped */
  if (increase_data.data_len == 0 || increase_data.data_ptr == NULL)
  {
    return TRUE;
  }

  tlv_len = (uint8)increase_data.data_len + sizeof(uint8);
  tlv_data = (uint8*)uimqmi_malloc(tlv_len);
  if (tlv_data == NULL)
  {
    return FALSE;
  }

  /* Copy length */
  temp_uint8 = (uint8)increase_data.data_len;
  (void)memscpy((void*)tlv_data, tlv_len, &temp_uint8, sizeof(uint8));

  /* Copy content */
  (void)memscpy((void*)(tlv_data + sizeof(uint8)),
                tlv_len - sizeof(uint8),
         (void*)increase_data.data_ptr,
         (uint32)increase_data.data_len);

  ret_value = qmi_svc_put_param_tlv(response_pptr,
                                    tlv_id,
                                    tlv_len,
                                    (void *)tlv_data);

  uimqmi_free(tlv_data);
  return ret_value;
} /* qmi_uim_response_increase_result */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_VCC_STATE()

  DESCRIPTION
    Composes the TLV content for Vcc state

  PARAMETERS
    tlv_id          : id to be used for the TLV
    response_pptr   : output sdu
    ldo_state       : UIM LDO state information

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_vcc_state
(
  byte                              tlv_id,
  dsm_item_type                  ** response_pptr,
  uimdrv_qmi_indications_type       ldo_state
)
{
  PACKED struct PACKED_POST
  {
    uint32 ldo_state;
  } tlv_data;

  ASSERT(response_pptr);

  switch(ldo_state)
  {
    case UIMDRV_LDO_ACTIVATED_IND:
      tlv_data.ldo_state = 0;
      break;
    case UIMDRV_LDO_AWAITING_DEACTIVATION_IND:
      tlv_data.ldo_state = 1;
      break;
    default:
      return FALSE;
  }

  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               sizeof(tlv_data),
                               (void *)&tlv_data);
} /* qmi_uim_response_vcc_state */


/*===========================================================================
  FUNCTION  QMI_UIM_RESPONSE_ENCRYPTED_STATUS()

  DESCRIPTION
    Encrypts and builds the response returned to security sensitive clients

  PARAMETERS
    tlv_id             : id to be used for the TLV
    cmd_id             : Command id
    request_id         : Request id
    result             : SimLock result status
    response_pptr      : Output sdu

  RETURN VALUE
    errval             : QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_uim_response_encrypted_status
(
  byte                         tlv_id,
  uint16                       cmd_id,
  uint16                       request_id,
  qmi_error_e_type             error,
  dsm_item_type **             response_pptr
)
{
  qmi_error_e_type        errval              = QMI_ERR_NONE;
  mmgsdi_data_type        rsp_data            = { 0, NULL };
  mmgsdi_data_type        encrypted_rsp_data  = { 0, NULL };
  boolean                 encrypted           = FALSE;

  PACKED struct PACKED_POST
  {
    uint16   cmd_id;
    uint16   request_id;
    uint8    rfu[QMI_UIM_ENCRYPTED_MSG_RFU_SIZE];
    uint16   result;
    uint16   error;
  } rsp_data_buffer;

  if(response_pptr == NULL)
  {
    return QMI_ERR_INTERNAL;
  }

  memset(&rsp_data_buffer, 0x00, sizeof(rsp_data_buffer));

  rsp_data_buffer.cmd_id = cmd_id;
  rsp_data_buffer.request_id = request_id;

  if (error != QMI_ERR_NONE)
  {
    rsp_data_buffer.result = QMI_RESULT_FAILURE;
  }
  else
  {
    rsp_data_buffer.result = QMI_RESULT_SUCCESS;
  }

  rsp_data_buffer.error = error;

  rsp_data.data_len = sizeof(rsp_data_buffer);
  rsp_data.data_ptr = (uint8*)&rsp_data_buffer;

  encrypted = qmi_uim_encrypt_payload(rsp_data,
                                      &encrypted_rsp_data);

  if (encrypted &&
      encrypted_rsp_data.data_len <= QMI_UIM_ENCRYPTED_RESP_MAX &&
      encrypted_rsp_data.data_ptr != NULL)
  {
    if (FALSE == qmi_uim_response_generic_payload_16bit(
                   tlv_id,
                   response_pptr,
                   encrypted_rsp_data,
                   FALSE))
    {
      dsm_free_packet(response_pptr);
      errval = QMI_ERR_NO_MEMORY;
    }
  }
  else
  {
    UIM_MSG_ERR_2("Encrypted: 0x%x, Encrypted data length: 0x%x",
                  encrypted,
                  encrypted_rsp_data.data_len);
    errval = QMI_ERR_INTERNAL;
  }

  if(encrypted_rsp_data.data_ptr != NULL)
  {
    uimqmi_free(encrypted_rsp_data.data_ptr);
    encrypted_rsp_data.data_ptr = NULL;
  }

  return errval;
} /* qmi_uim_response_encrypted_status */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_PERSO_CONTROL_KEY_PRESENCE()

  DESCRIPTION
    Composes the TLV content of perso control key presence

  PARAMETERS
    tlv_id              : id to be used for the TLV
    response_pptr       : output sdu
    num_slots           : number of slots supported
    get_status_ptr      : feature status information

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_perso_control_key_presence
(
  byte                                      tlv_id,
  dsm_item_type **                          response_pptr,
  uint8                                     num_slots,
  const simlock_get_status_msg_resp_type  * get_status_ptr
)
{
  uint16    tlv_len                                  = 0;
  uint16    offset                                   = 0;
  uint8     i                                        = 0;
  uint8     j                                        = 0;
  uint8     slot_cnt                                 = 0;
  uint8     total_features                           = 0;
  uint8  *  tlv_data_ptr                             = NULL;
  uint8     features_in_slot[QMI_UIM_MAX_CARD_COUNT] = {0};
  boolean   ret_value                                = TRUE;

  PACKED struct PACKED_POST
  {
    uint8   feature_id;
    uint8   ck_present;
  } feature;

  ASSERT(response_pptr);
  ASSERT(get_status_ptr);

  slot_cnt = MIN(num_slots, QMI_UIM_MAX_CARD_COUNT);

  /* calculate the number of features active per slot */
  for (i = 0;
       i < SIMLOCK_SLOT_COUNT_MAX && i < slot_cnt;
       i++)
  {
    for(j = 0;
        j < SIMLOCK_CATEGORY_COUNT_MAX && j < QMI_UIM_MAX_NUMBER_PERSO_FEATURES;
        j++)
    {
      if (get_status_ptr->lock_info[i].category_info[j].category_status)
      {
        features_in_slot[i]++;
        total_features++;
      }
    }
  }

  /* No active features are available */
  if(total_features == 0)
  {
    return TRUE;
  }

  /* calculate the total length */
  tlv_len = sizeof(uint8) +
            (slot_cnt * sizeof(uint8)) +
            (sizeof(feature) * total_features);

  tlv_data_ptr = (uint8*)uimqmi_malloc(tlv_len);
  if (tlv_data_ptr == NULL)
  {
    return FALSE;
  }

  /* Insert the number of slots in the TLV data */
  tlv_data_ptr[offset++] = slot_cnt;

  for (i = 0;
       i < SIMLOCK_SLOT_COUNT_MAX && i < slot_cnt;
       i++)
  {
    ASSERT(offset < tlv_len);

    /* Insert the number of active features per slot in the TLV data */
    tlv_data_ptr[offset++] = features_in_slot[i];

    /* Check the features that are active and insert the
       corresponding feature id and time left in the TLV data */
    for(j = 0;
        j < SIMLOCK_CATEGORY_COUNT_MAX && j < QMI_UIM_MAX_NUMBER_PERSO_FEATURES;
        j++)
    {
      if (get_status_ptr->lock_info[i].category_info[j].category_status)
      {
        if(qmi_uim_response_convert_simlock_category(get_status_ptr->lock_info[i].category_info[j].category,
                                                     &feature.feature_id) != QMI_ERR_NONE)
        {
          uimqmi_free(tlv_data_ptr);
          return FALSE;
        }

        feature.ck_present  = get_status_ptr->lock_info[i].category_info[j].ck_present;

        ASSERT((offset + sizeof(feature)) <= tlv_len);
        (void)memscpy((void*)(tlv_data_ptr + offset),
                      tlv_len - offset,
                      &feature,
                      sizeof(feature));
        offset += sizeof(feature);
      }
    }
  }

  ret_value = qmi_svc_put_param_tlv(response_pptr,
                                    tlv_id,
                                    tlv_len,
                                    (void *)tlv_data_ptr);

  uimqmi_free(tlv_data_ptr);
  return ret_value;
} /* qmi_uim_response_perso_control_key_presence */


/*===========================================================================
  FUNCTION  QMI_UIM_SIMLOCK_REMOTE_SFS_ENCRYPTED_IND()

  DESCRIPTION
    Encrypts remote SFS operation request using Trust Zone application APIs

  PARAMETERS
    operation     : Operation requested (Read/Write)
    request_id    : Request ID for this request
    file          : The file to perform operation on
    write_data    : The data to be written (valid for write operation)
    resposne_pptr : Pointer to command response pointer

  RETURN VALUE
    TRUE        : If the encrypted response is build successfully
    FALSE       : If otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_simlock_remote_sfs_encrypted_ind
(
  uint16                              cmd_id,
  simlock_file_operation_enum_type    operation,
  simlock_token_id_type               request_id,
  simlock_file_type                   file,
  simlock_data_type                   write_data,
  dsm_item_type                    ** response_pptr
)
{
  boolean               encryption_used  = TRUE;
  mmgsdi_data_type      decrypted_data   = {0, NULL};
  mmgsdi_data_type      encrypted_data   = {0, NULL};
  qmi_error_e_type      errval           = QMI_ERR_NONE;

  PACKED struct PACKED_POST
  {
    uint16  cmd_id;
    uint16  request_id;
    uint8   rfu[QMI_UIM_ENCRYPTED_MSG_RFU_SIZE];
    uint8   operation;
    uint8   file_id;
  } fixed_data;

  ASSERT(response_pptr);

  memset(&fixed_data, 0x00, sizeof(fixed_data));

  fixed_data.cmd_id     = cmd_id;
  fixed_data.request_id = request_id;

  switch (operation)
  {
    case SIMLOCK_FILE_OPERATION_READ:
      fixed_data.operation  = 0;
      break;

    case SIMLOCK_FILE_OPERATION_WRITE:
      fixed_data.operation  = 1;
      break;

    default:
      return FALSE;
  }

  errval = qmi_uim_convert_simlock_file_type_to_file_id(file, &fixed_data.file_id);
  if(errval != QMI_ERR_NONE)
  {
    return FALSE;
  }

  /* calculate the length of decrypted indication */
  decrypted_data.data_len = sizeof(fixed_data);

  if(operation == SIMLOCK_FILE_OPERATION_WRITE)
  {
    if ((write_data.data_len == 0) ||
        (write_data.data_ptr == NULL))
    {
      return FALSE;
    }

    /* For this operation, the data_len (2 bytes) and data are sent
       in the indication payload */
    decrypted_data.data_len += (sizeof(uint16) + write_data.data_len);
    if (decrypted_data.data_len > QMI_UIM_MAX_UINT16)
    {
      return FALSE;
    }
  }

  decrypted_data.data_ptr = uimqmi_malloc(decrypted_data.data_len);
  if (decrypted_data.data_ptr == NULL)
  {
    return FALSE;
  }

  /* copy the fixed data */
  (void)memscpy((void*)decrypted_data.data_ptr,
                decrypted_data.data_len,
                (void*)&fixed_data,
                sizeof(fixed_data));

  if(operation == SIMLOCK_FILE_OPERATION_WRITE)
  {
    uint16  temp_data_len = 0;

    temp_data_len = (uint16)write_data.data_len;

    /* copy the length */
    (void)memscpy((void*)(decrypted_data.data_ptr + sizeof(fixed_data)),
                  (decrypted_data.data_len - sizeof(fixed_data)),
                  (uint8*)&temp_data_len,
                  sizeof(uint16));

    /* copy the data */
    (void)memscpy((void*)(decrypted_data.data_ptr + sizeof(fixed_data) + sizeof(uint16)),
                  (decrypted_data.data_len - sizeof(fixed_data) - sizeof(uint16)),
                  (void*)write_data.data_ptr,
                  write_data.data_len);
  }

  encryption_used = qmi_uim_encrypt_payload(decrypted_data,
                                            &encrypted_data);

  if (encryption_used == FALSE ||
      FALSE == qmi_uim_response_generic_payload_16bit(
                 UIMI_TLV_IND_TAG_MANDATORY_1,
                 response_pptr,
                 encrypted_data,
                 TRUE))
  {
    dsm_free_packet(response_pptr);
    errval = QMI_ERR_INTERNAL;
  }

  uimqmi_free(decrypted_data.data_ptr);
  decrypted_data.data_ptr = NULL;

  if(encrypted_data.data_ptr != NULL)
  {
    uimqmi_free(encrypted_data.data_ptr);
    encrypted_data.data_ptr = NULL;
  }

  return (errval == QMI_ERR_NONE ? TRUE : FALSE);
} /* qmi_uim_simlock_remote_sfs_encrypted_ind */


/*===========================================================================
  FUNCTION  QMI_UIM_RESPONSE_TEMPORARY_UNLOCK_STATUS()

  DESCRIPTION
    Prepares the temporary unlock indication based on the
    response of get status response

  PARAMETERS
    tlv_id                       : ID to be used for TLV 
    indication_pptr              : Pointer to Indication pointer
    temporary_unlock_status_ptr  : Pointer to the SIMLOCK get status information
    num_slots       : Number of slots supported

  RETURN VALUE
    TRUE        : If the indication can be send to client
    FALSE       : If otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_temporary_unlock_status
(
  byte                                     tlv_id,
  dsm_item_type                          **indication_pptr,
  const simlock_get_status_msg_resp_type  *temporary_unlock_status_ptr,
  uint8                                    num_slots
)
{
  uint8    i                                        = 0;
  uint8    j                                        = 0;
  uint8    total_features                           = 0;
  uint16   offset                                   = 0;
  uint16   tlv_len                                  = 0;
  uint8    features_in_slot[QMI_UIM_MAX_CARD_COUNT] = {0};
  uint8   *ind_data_ptr                             = NULL;
  boolean  ret_val                                  = FALSE;

  PACKED struct PACKED_POST
  {
    uint8   feature_id;
    uint32  time_left;
  } temp_unlock_info;

  ASSERT(indication_pptr);
  ASSERT(temporary_unlock_status_ptr);

  num_slots = MIN(num_slots, QMI_UIM_MAX_CARD_COUNT);

  for (i = 0;
       i < SIMLOCK_SLOT_COUNT_MAX && i < num_slots;
       i++)
  {
    for(j = 0;
        j < SIMLOCK_CATEGORY_COUNT_MAX && j < QMI_UIM_MAX_NUMBER_PERSO_FEATURES;
        j++)
    {
      if ((temporary_unlock_status_ptr->lock_info[i].category_info[j].category_status) &&
          (temporary_unlock_status_ptr->remaining_time.duration[i][j] > 0))
      {
        features_in_slot[i]++;
        total_features++;
      }
    }
  }

  /* calculate the total length */
  tlv_len = sizeof(uint8) +
           (num_slots * sizeof(uint8)) +
           (num_slots * sizeof(uint8)) +
           (sizeof(temp_unlock_info) * total_features);

  ind_data_ptr = (uint8*)uimqmi_malloc(tlv_len);
  if (ind_data_ptr == NULL)
  {
    return FALSE;
  }

  /* Insert the number of slots in the TLV data */
  ind_data_ptr[offset++] = num_slots;

  for (i = 0;
       i < SIMLOCK_SLOT_COUNT_MAX && i < num_slots;
       i++)
  {
    uim_slot_enum_v01   slot = UIM_SLOT_1_V01;

    ASSERT(offset + 2 <= tlv_len);

    /* Insert the number of active features per slot in the TLV data */
    if(qmi_uim_simlock_slot_index_to_uim_slot(i,&slot) != QMI_ERR_NONE)
    {
      uimqmi_free(ind_data_ptr);
      return FALSE;
    }

    /* Copy slot and the number of features active in that slot */
    ind_data_ptr[offset++] = slot;
    ind_data_ptr[offset++] = features_in_slot[i];

    /* Check the features that are active and insert the
       corresponding feature id and time left in the TLV data */
    for(j = 0;
        j < SIMLOCK_CATEGORY_COUNT_MAX && j < QMI_UIM_MAX_NUMBER_PERSO_FEATURES;
        j++)
    {
      if ((temporary_unlock_status_ptr->lock_info[i].category_info[j].category_status) &&
          (temporary_unlock_status_ptr->remaining_time.duration[i][j] > 0))
      {
        /* Pack all the existing temporary unlock features */
        if(qmi_uim_response_convert_simlock_category(temporary_unlock_status_ptr->lock_info[i].category_info[j].category,
                                                     &temp_unlock_info.feature_id) != QMI_ERR_NONE)
        {
          uimqmi_free(ind_data_ptr);
          return FALSE;
        }

        temp_unlock_info.time_left = temporary_unlock_status_ptr->remaining_time.duration[i][j];

        ASSERT((offset + sizeof(temp_unlock_info)) <= tlv_len);
        (void)memscpy((void*)(ind_data_ptr + offset),
                      tlv_len - offset,
                      &temp_unlock_info,
                      sizeof(temp_unlock_info));
        offset += sizeof(temp_unlock_info);
      }
    }
  }

  ret_val = qmi_svc_put_param_tlv(indication_pptr,
                                  tlv_id,
                                tlv_len,
                                  (void *)ind_data_ptr);

  uimqmi_free(ind_data_ptr);
  return ret_val;
} /* qmi_uim_response_temporary_unlock_status */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_ENCRYPTED_PIN1()

  DESCRIPTION
    Composes the TLV content of the encrypted PIN1 value

  PARAMETERS
    tlv_id                : id to be used for the TLV
    response_pptr         : output sdu
    encrypted_pin1_length : encrypted PIN1 data length
    encrypted_pin1_ptr    : encrypted PIN1 data pointer

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_encrypted_pin1
(
  byte                          tlv_id,
  dsm_item_type **              response_pptr,
  uint8                         encrypted_pin1_length,
  const uint8 *                 encrypted_pin1_ptr
)
{
  uint8     * encr_pin1_value_ptr = NULL;
  boolean     ret_value           = TRUE;

  ASSERT(response_pptr);

  /* Data is not present... simply return TRUE.
     This TLV is optional and will be skipped */
  if (encrypted_pin1_length == 0 || encrypted_pin1_ptr == NULL)
  {
    return TRUE;
  }

  /* Allocate buffer needed (one more byte of the length field ) */
  encr_pin1_value_ptr = (uint8*)uimqmi_malloc(encrypted_pin1_length + sizeof(uint8));
  if (encr_pin1_value_ptr == NULL)
  {
    return FALSE;
  }

  /* Copy length */
  (void)memscpy((void*)encr_pin1_value_ptr,
                encrypted_pin1_length,
                &encrypted_pin1_length,
                sizeof(uint8));

  /* Copy PIN data */
  (void)memscpy((void*)(encr_pin1_value_ptr + sizeof(uint8)),
                encrypted_pin1_length,
                (void*)encrypted_pin1_ptr,
                (uint32)encrypted_pin1_length);

  ret_value = qmi_svc_put_param_tlv(response_pptr,
                                    tlv_id,
                                    encrypted_pin1_length  + sizeof(uint8),
                                    (void *)encr_pin1_value_ptr);
  uimqmi_free(encr_pin1_value_ptr);
  return ret_value;
} /* qmi_uim_response_encrypted_pin1 */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_GET_EID_INFO()

  DESCRIPTION
    Composes the TLV for EID retrieved

  PARAMETERS
    tlv_id              : id to be used for the TLV
    response_pptr       : output sdu
    get_eid_resp_ptr : profile confirmation pointer

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_get_eid_info
(
  byte                                   tlv_id,
  dsm_item_type **                       response_pptr,
  const lpa_get_eid_resp_type           *get_eid_resp_ptr
)
{
  PACKED struct PACKED_POST
  {
    uint8 eid_len;
    uint8 eid[QMI_UIM_EID_LEN_V01];
  }tlv_data_eid;

  ASSERT(response_pptr);
  ASSERT(get_eid_resp_ptr);

  tlv_data_eid.eid_len = sizeof(tlv_data_eid.eid);
  (void)memscpy((void*)tlv_data_eid.eid,
                sizeof(tlv_data_eid.eid),
                (void*)get_eid_resp_ptr->eid,
                sizeof(get_eid_resp_ptr->eid));

  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               sizeof(tlv_data_eid),
                               (void *)&tlv_data_eid);
} /* qmi_uim_response_get_eid_info */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_GET_EUICC_PROFILE_INFO()

  DESCRIPTION
    Composes the TLV content of profile information

  PARAMETERS
    tlv_id                     : id to be used for the TLV
    response_pptr              : output sdu
    get_profiles_info_resp_ptr : profile information pointer

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_get_euicc_profile_info
(
  byte                                   tlv_id,
  dsm_item_type **                       response_pptr,
  const lpa_get_profiles_info_resp_type *get_profiles_info_resp_ptr
)
{
  uint8   profile_index = 0;
  uint16  tlv_len       = 0;

  PACKED struct PACKED_POST
  {
    uint8 num_profiles;
    PACKED struct PACKED_POST
    {
      uint32 profile_id;
      uint32 profile_type;
    }profile_data[QMI_UIM_PROFILES_MAX_V01];
  }tlv_data_profile_data;

  ASSERT(response_pptr);
  ASSERT(get_profiles_info_resp_ptr);

  tlv_data_profile_data.num_profiles = MIN(get_profiles_info_resp_ptr->num_profiles,
                                           QMI_UIM_PROFILES_MAX_V01);

  for(profile_index = 0;
      profile_index < tlv_data_profile_data.num_profiles;
      profile_index++)
  {
    tlv_data_profile_data.profile_data[profile_index].profile_id =
                          qmi_uim_get_profile_id_from_index(profile_index);
    tlv_data_profile_data.profile_data[profile_index].profile_type =
                          UIM_PROFILE_TYPE_REGULAR_V01;
  }

  tlv_len = sizeof(uint8) +
            (sizeof(tlv_data_profile_data.profile_data[0]) *
             tlv_data_profile_data.num_profiles);

  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               tlv_len,
                               (void *)&tlv_data_profile_data);
} /* qmi_uim_response_get_euicc_profile_info */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_EUICC_DOWNLOAD_STATUS_DATA()

  DESCRIPTION
    Composes the TLV content for download progress

  PARAMETERS
    tlv_id           : TLV id to be sent
    response_pptr    : Pointer to response pointer
    lpa_event_ptr     : Pointer to LPA event data
    lpa_status         : LPA download status


  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_euicc_download_status_data
(
  byte                             tlv_id,
  dsm_item_type                 ** response_pptr,
  const lpa_event_data_type      * lpa_event_ptr,
  lpa_result_enum_type             lpa_status
)
{
  PACKED struct PACKED_POST
  {
    uint32   download_status;
  } tlv_data;

  ASSERT(response_pptr);
  ASSERT(lpa_event_ptr);

  if (lpa_status == LPA_SUCCESS)
  {
    if (lpa_event_ptr->evt_data.profile_download_progress.installation_complete)
    {
      tlv_data.download_status = UIM_PROFILE_INSTALLATION_COMPLETE_V01;
    }
    else if (lpa_event_ptr->evt_data.profile_download_progress.percent_downloaded < 100)
    {
      tlv_data.download_status = UIM_PROFILE_DOWNLOAD_IN_PROGRESS_V01;
    }
    else if (lpa_event_ptr->evt_data.profile_download_progress.percent_downloaded == 100 &&
             lpa_event_ptr->evt_data.profile_download_progress.installation_complete == FALSE)
    {
      tlv_data.download_status = UIM_PROFILE_DOWNLOAD_COMPLETE_INSTALLATION_IN_PROGRESS_V01;
    }
    else
    {
      tlv_data.download_status = UIM_PROFILE_DOWNLOAD_ERROR_V01;
    }
  }
  else
  {
    tlv_data.download_status = UIM_PROFILE_DOWNLOAD_ERROR_V01;
  }

  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               sizeof(tlv_data),
                               (void *)&tlv_data);
} /* qmi_uim_response_euicc_download_status_data */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_PROFILE_PERCENTAGE()

  DESCRIPTION
    Composes the TLV content for percentage downloaded or installed during
    add profile


  PARAMETERS
    tlv_id           : TLV id to be sent
    response_pptr    : Pointer to response pointer
    lpa_event_ptr     : Pointer to LPA event data

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_profile_percentage
(
  byte                             tlv_id,
  dsm_item_type                 ** response_pptr,
  uint8                            download_percent
)
{
  PACKED struct PACKED_POST
  {
    uint8   download_percent;
  } tlv_data;

  ASSERT(response_pptr);

  tlv_data.download_percent = download_percent;

  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               sizeof(tlv_data),
                               (void *)&tlv_data);
} /* qmi_uim_response_profile_percentage */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_DOWNLOAD_ERROR_CAUSE()

  DESCRIPTION
    Composes the TLV content for error cause during add profile


  PARAMETERS
    tlv_id           : TLV id to be sent
    response_pptr    : Pointer to response pointer
    lpa_status       : LPA status for the add_profile event

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_download_error_cause
(
  byte                             tlv_id,
  dsm_item_type                 ** response_pptr,
  lpa_result_enum_type             lpa_status
)
{
  PACKED struct PACKED_POST
  {
    uint32   error_cause;
  } tlv_data;

  ASSERT(response_pptr);

  switch (lpa_status)
  {
    case LPA_ERROR_HEAP_EXHAUSTED:
      tlv_data.error_cause = UIM_DOWNLOAD_ERROR_CAUSE_MEMORY_V01;
      break;
    
    case LPA_ERROR_NETWORK:
      tlv_data.error_cause = UIM_DOWNLOAD_ERROR_CAUSE_NETWORK_V01;
      break;
    
    case LPA_ERROR_SIM_NOT_READY:
      tlv_data.error_cause = UIM_DOWNLOAD_ERROR_CAUSE_SIM_V01;
      break;

    default:
      tlv_data.error_cause = UIM_DOWNLOAD_ERROR_CAUSE_GENERIC_V01;
      break;
  }

  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               sizeof(tlv_data),
                               (void *)&tlv_data);
} /* qmi_uim_response_download_error_cause */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_PROFILE_STATE()

  DESCRIPTION
    Composes the TLV content of profile state

  PARAMETERS
    tlv_id              : id to be used for the TLV
    response_pptr       : output sdu
    slot_index          : Slot Index
    profile_index       : Profile Index

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_profile_state
(
  byte                                   tlv_id,
  dsm_item_type **                       response_pptr,
  lpa_profile_state_enum_type            profile_state
)
{
  PACKED struct PACKED_POST
  {
    uint32 profile_state;
  }tlv_data;

  ASSERT(response_pptr);

  switch (profile_state)
  {
    case LPA_PROFILE_STATE_ENABLED:
      tlv_data.profile_state = UIM_PROFILE_STATE_ACTIVE_V01;
      break;
    case LPA_PROFILE_STATE_DISABLED:
      tlv_data.profile_state = UIM_PROFILE_STATE_INACTIVE_V01;
      break;
    default:
      return FALSE;
  }

  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               sizeof(tlv_data),
                               (void *)&tlv_data);
} /* qmi_uim_response_profile_state */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_PROFILE_ICON_TYPE()

  DESCRIPTION
    Composes the TLV content of profile icon

  PARAMETERS
    tlv_id              : id to be used for the TLV
    response_pptr       : output sdu
    slot_index          : Slot Index
    profile_index       : Profile Index

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_profile_icon_type
(
  byte                                   tlv_id,
  dsm_item_type **                       response_pptr,
  lpa_icon_type_enum_type                profile_icon
)
{
  PACKED struct PACKED_POST
  {
    uint32 icon_type;
  }tlv_data;

  ASSERT(response_pptr);

  switch (profile_icon)
  {
    case LPA_ICON_TYPE_JPEG:
      tlv_data.icon_type = UIM_PROFILE_ICON_TYPE_JPEG_V01;
      break;
    case LPA_ICON_TYPE_PNG:
      tlv_data.icon_type = UIM_PROFILE_ICON_TYPE_PNG_V01;
      break;
    default:
      tlv_data.icon_type = UIM_PROFILE_ICON_TYPE_NONE_V01;
      break;
  }

  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               sizeof(tlv_data),
                               (void *)&tlv_data);
} /* qmi_uim_response_profile_icon_type */


#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
