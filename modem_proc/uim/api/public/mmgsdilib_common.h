#ifndef MMGSDILIB_COMMON_H
#define MMGSDILIB_COMMON_H
/*===========================================================================

            M M G S D I   L I B R A R Y   C O M M O N   D E F I N I T I O N S

===========================================================================*/
/**
@file mmgsdilib_common.h
@brief This library contains declarations and definitions common to MMGSDI
library functions.
This library contains declarations and definitions used by both the MMGSDI
Session Library and MMGSDI Library.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The mmgsdi_mainpage.dox file contains all file/group descriptions that are
      in the output PDF generated using Doxygen and Latex. To edit or update
      any of the file/group text in the PDF, edit the mmgsdi_mainpage.dox file or
      contact Tech Pubs.

      The above description for this file is part of the "common_library" group
      description in the mmgsdi_mainpage.dox file.
===========================================================================*/

/*=============================================================================
Copyright (c) 2010-2016 by QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary.
=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/api/public/mmgsdilib_common.h#1 $$ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
11/08/16   bcho    Added support for ICCID simlock
10/25/16   dd      Added support for auto reg SMS when ESN is changed
06/17/16   vdc     Added support for SPN based SIMLOCK feature
06/09/16   ar      Send FDN event source along with FDN event
04/11/16   av      Introduce LPA support
01/30/16   vdc     Halt initialization if IMSI is not provisioned in card
12/25/15   bcho    Send Card and session events based on SFS data in PSM mode
11/23/15   vdc     Send profile switching evt when profile has switched in card
11/06/15   stv     Always send no refresh enforcement to clients
09/25/15   vr      Added support for ERA-GLONASS
09/09/15   stv     Pack imsi_m activation status into subscription_ready_evt
08/10/15   stv     To send temporary unlock status indication
05/29/15   hh      Convert MANAGE CHANNEL APDU to MMGSDI requests
04/30/15   lxu     Support refresh on EF GBABP
03/30/15   ar      Support for cross-mapping in multisim_auto_provisioning NV
03/11/15   vv      Added support for new perso status
11/10/14   tl      Remove SAP pin
11/05/14   ar      Remove mmgsdi_get_card_reader_status () API
07/24/14   yt      Support for restricted SIM access via MMGSDI session APIs
07/15/14   kk      NV Refresh: MCFG - MMGSDI interactions
07/10/14   tl      Remove ALWAYS OFF feature FEATURE_MMGSDI_PKCS15
06/12/14   av      Cache mandatory EFs used by NVRUIM for critical 1x NV reads
06/10/14   tl      Introduce EF 3GPDUPPExt
04/17/14   av      Enhance MMGSDI attributes cache
04/08/14   ar      Skip sending event to clients if service status is same
03/26/14   tl      SIM Lock secondary revisions
03/24/14   ar      Remove MMGSDI_SESSION_ENABLE/DISABLE_FDN_REQ
03/18/14   tl      Introduce new SIM Lock feature
03/13/14   ar      Update correct PRL_VERSION and PRL_REVISION
03/03/14   hh      Purge XXLOCI files by marking certain bytes invalid
02/25/14   yt      Support for selection mode in SELECT AID request
02/25/14   hh      Purge temporary mobile identities during UICC Reset Refresh
02/25/14   bcho    Added support to store OPLMN List in EFS and cache it
02/20/14   ak      Fix compile errors due to strict compiler on 9x35
12/02/13   tkl     Support for SFI
01/29/14   vdc     Added support for PLMN name source
12/11/13   vv      Added support for the EF-PSISMSC in DF TELECOM
11/22/13   bcho    Use roaming status received in CM SS event in EONS algorithm
11/08/13   yt      Reduce size of MMGSDI tables
11/05/13   tkl     Add IMSI check for TDS test SIM
10/27/13   df      Add MMGSDI_CARD_ERR_SAP_CONNECTED error condition
10/01/13   av      Reject power-down/up with new error code in ONCHIP mode
10/01/13   vv      Added support for the EF-PWS
09/23/13   df      Support for SIM busy
08/30/13   av      Support for Chinese language PLMN names
08/07/13   tl      Add CDMA AKA Authenticate functionality
07/26/13   av      Move MMGSDI_MAX_APP_ID_LEN to publich header for uimqmi use
07/15/13   vs      EONS updates to support single baseband SGLTE
07/15/13   av      Introduction of new NULL_BYTES card error type
07/01/13   at      Support for additional info in EONS APIs
06/28/13   bcho    APIs added to return plmn info from SE13 table.
06/27/13   spo     Added support for files under DF TELECOM
05/21/13   vv      Added support for session closed cause
05/17/13   hn      Support for Alpha for Refresh
04/30/13   bcho    Added support to provide rplmn-spn display bit and network
                   info in get operator name API cnf and in EONS event
04/30/13   av      Added ICCID in the MMGSDI_CARD_INSERTED_EVT
04/15/13   tl      Remove SAP init, dereg and pin verify functionality
04/11/13   vv      Re-design handling of VPE response from the card
03/04/13   av      Updates for T3245 timer support to mark APP as legal/valid
02/22/13   yt      Support for slot 3
01/31/13   tl      Added new memory management error code
01/20/13   vv      Introduced new UCS2_PACKED encoding scheme
01/20/13   vv      Added support for updating EF-ACSGL record
01/20/13   vv      Added EONS support for  the CSG
12/05/12   vv      Added support for parallel processing in MMGSDI
10/21/12   av      Added support for new ECC event
09/20/12   bcho    Added support for Get SE13 Plmn Names API
09/11/12   bcho    Ignore display condition enum added
09/05/12   av      Handling of VPE response from the card
09/03/12   bcho    Added support for EONS cache ready event
08/29/12   at      Support for new API mmgsdi_sap_connect_ext
08/16/12   at      Support for new API mmgsdi_session_register_all_for_refresh
08/13/12   bcho    Radio Access Technology enum added
08/10/12   av      Removed RPC support along with RPC style comments
07/30/12   yt      New error code for when app is not shareable across channels
07/13/12   vv      Added support for the CSG EFs
05/23/12   vv      Added support for file update notification registration
05/17/12   tl      Added support for enabling Virtual SIM
03/09/12   bcho    Perso status FAILURE and WAIT FOR EXTERNAL PERSO added,
                   MMGSDI_APP_STATE_READY_NO_SUB_EVT replaced with
                   MMGSDI_APP_STATE_EXT_PERSO_FAILED.
03/01/12   kk      Added support for app state extraction in get_session_info
02/27/12   shr     Added support for Send APDU extension API
02/23/12   shr     Added support for Session Open with MF
02/22/12   vv      Added new status type in mmgsdi_pin_status_enum_type
12/27/11   kk      Fixed EF ENUM numbering
12/14/11   bcho    New error type added in mmgsdi_crad_err_info_enum_type
11/23/11   shr     Removed legacy GSDI interfaces
11/23/11   bcho    Get Operator Name updates
11/02/11   bcho    Added support for Get Operator Name API
10/21/11   nmb     Additional RPM Files for SIM EF enumerations
10/12/11   nb      Added support for new Refresh Stage
08/24/11   nmb     Added RPM Files for USIM EF path lookups
07/05/11   vs      Support key ref based PIN op, session open with select rsp
06/15/11   shr     Added support for Session Open extension API
05/17/11   yt      Support for INCREASE command
05/05/11   shr     Handle REFRESH timer expiry in MMGSDI task context
04/22/11   vs      Support for subscription ok confirmation
04/21/11   nmb     Added RPM Files for USIM EF path lookups
04/07/11   nmb     Support for ISIM GBA authentication
03/28/11   vs      Added return types for manage channel or select aid failure
01/06/11   kk      Added OTA COMMIT RSP event - triggered after OTA commit rsp
01/03/11   kk      Revert mmgsdi_session_read_prl_cnf_type metacomment to a VARRAY
10/08/10   kk      Added support for sending FDN event when EF FDN is updated
09/23/10   sw      (Tech Pubs) Edited/added Doxygen comments and markup.
07/22/10   adp     Moving the enums in mmgsdi_file_enum_type to better align
                   with different package
06/28/10   nmb     Updates for JCDMA CSIM support
06/16/10   shr     Added support for Hidden key event
06/15/10   js      Fixed meta comments for read_prl_cnf_type
05/13/10   ssr     Fixed UIM 3GPD NV support
05/03/10   adp     Adding Perso related mapping from gsdi perso status to
                   mmgsdi perso status.
04/27/10   shr     Added Hiddenkey support
03/19/10   shr     Switch/deactivate provisioning clean up and redesign
03/09/10   shr     Clean up REFRESH FCN, INIT, INIT_FCN and APP_RESET
03/02/10   jk      Added MMSS Files
02/19/10   shr     Session get info API implementation
02/17/10   shr     Fixed Session activate/deactivate handling
02/01/10   jk      EPRL Support
01/25/10   vs      Initial Revision

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "comdef.h"
#include "qw.h"


/** @addtogroup common_library
@{ */

/*=============================================================================

                       DEFINES DECLARATIONS

=============================================================================*/
/** File ID size (in bytes) as indicated in Specification 31.102 11.11/51.011.
    For example, 0x7F20 is typically split into two bytes, 0x7F and 0x20. */
#define MMGSDI_FILE_ID_SIZE                          2

/** Minimum PIN length for PIN operations. The minimum length allowed is four
    bytes. */
#define MMGSDI_PIN_MIN_LEN                           4

/** Maximum PIN length for PIN operations. The maximum length allowed is eight
    bytes. */
#define MMGSDI_PIN_MAX_LEN                           8

/** Maximum number of applications on a UICC. */
#define MMGSDI_MAX_APP_INFO                          9

/** Maximum Application Identifier/label length for an application. */
#define MMGSDI_MAX_AID_LEN                           32

/** Maximum path length for access. */
#define MMGSDI_MAX_PATH_LEN                          5

/** Service category for CDMA. */
#define MMGSDI_CDMA_SRV                              0x100

/** Service category for GSM. */
#define MMGSDI_GSM_SRV                               0x200

/** Service category for a USIM. */
#define MMGSDI_USIM_SRV                              0x400

/** Service category for a CSIM. */
#define MMGSDI_CSIM_SRV                              0x800

/** Length of the key size returned by the Generate Key/VPM command on a RUIM. */
#define MMGSDI_KEY_SIZE                              8

/** Maximum HRPD CHAP challenge length. */
#define MMGSDI_MAX_HRPD_CHAP_CHALLENGE_LENGTH        254

/** Maximum 3G Packed Data MIP challenge length. */
#define MMGSDI_3GPD_MAX_MIP_CHALLENGE_LENGTH         238

/** Maximum 3GPD MIP hash Registration Request data length. */
#define MMGSDI_MAX_3GPD_HASH_RRQ_DATA_LENGTH         800

/** Maximum 3GPD MIP hash Registration Request data length. */
#define MMGSDI_MAX_3GPD_HASH_RRQ_DATA_LENGTH         800

/** Maximum 3GPD MIP Mobile Network Home Agent data length. */
#define MMGSDI_MAX_3GPD_MN_HA_REG_DATA_LENGTH        800

/** Maximum 3GPD Simple IP CHAP challenge length. */
#define MMGSDI_MAX_3GPD_CHAP_CHALLENGE_LENGTH        32

/** Maximum entries in each Public Key Cryptography System table.

    This is not a specification-defined value, but a slightly higher value to
    hold all possible PKCS15 elementary files. */
#define MMGSDI_MAX_PKCS15_TABLE_ENTRIES              10

/** Length of a signed response in a GSM algorithm. */
#define MMGSDI_GSM_ALGO_SRES_LEN                     4

/** Length of the Key cipher in a GSM algorithm. */
#define MMGSDI_GSM_ALGO_KC_LEN                       8

/** Length of the EF Location Information (LOCI) file. */
#define MMGSDI_LOCI_LEN                              11

/** Length of the EF Packet Switched Location Information (PSLOCI) file. */
#define MMGSDI_PSLOCI_LEN                            14

/** Length of the EF EPS Location Information (EPSLOCI) file. */
#define MMGSDI_EPSLOCI_LEN                           18

/** Location Update Service offset of the EF LOCI file. */
#define MMGSDI_LUS_OFFSET                            10

/** Location Area Identity (LAI) offset of the EF LOCI file. */
#define MMGSDI_LAI_OFFSET                            4

/** Routing Area Update Status offset of the EF PSLOCI file. */
#define MMGSDI_PSLOCI_RAUS_OFFSET                    13

/** EPS Update Status offset of the EF EPSLOCI file. */
#define MMGSDI_EPSLOCI_EPSUS_OFFSET                  17

/** Length of the EF International Mobile Subscriber Identity (IMSI) file. */
#define MMGSDI_IMSI_LEN                              9

/** IMSI length (byte 1 of IMSI contents) if IMSI is not provisioned */
#define MMGSDI_IMSI_NOT_PROVISIONED                  0xFF

/** Minimum length of a valid EF-IMSI file. */
#define MMGSDI_MIN_VALID_IMSI_LEN                    3

/** Maximum length of a valid EF-IMSI file. */
#define MMGSDI_MAX_VALID_IMSI_LEN                    8

/** Length of the CDMA EF IMSI file. */
#define MMGSDI_CDMA_IMSI_LEN                         10

/** Length used for an unknown length of a read response. */
#define MMGSDI_STANDARD_READ_LEN                     0

/** Maximum data block length. */
#define MMGSDI_MAX_DATA_BLOCK_LEN                    256

/** Length of the EF Access Control Class file. */
#define MMGSDI_ACC_LEN                               2

/** Length of the EF Broadcast Control Channel file. */
#define MMGSDI_BCCH_LEN                              16

/** Length of the EF Acting Home Public Land Mobile Network (PLMN) file. */
#define MMGSDI_CINGULAR_ACT_HPLMN_LEN                3

/** Length of the EF System Tester file. */
#define MMGSDI_CINGULAR_TST_LEN                      8

/** Length of the EF Home PLMN file. */
#define MMGSDI_HPLMN_SP_LEN                          1

/** Length of the EF Key Cipher General Packet Radio Services (GPRS) file. */
#define MMGSDI_KCGPRS_LEN                            8

/** Length of the Key Cipher. */
#define MMGSDI_KC_LEN                                8

/** Length of the EF Location Information GPRS file. */
#define MMGSDI_LOCIGPRS_LEN                          14

/** Length of the Bluetooth<sup>&reg;</sup> Device address. */
#define MMGSDIBT_BD_ADDR_LEN                         6

/** Maximum file path. */
#define MMGSDI_MAX_FILE_PATHS                        0x02

/** Size of the CAVE authentication digits. */
#define MMGSDI_CAVE_DIGITS_SIZE                      3

/** Length of the Short Message Service. */
#define MMGSDI_SMS_LEN                               176

/** Length of the Short Message Service Report. */
#define MMGSDI_SMSR_LEN                              30

/** Length of the Accumulated Call Meter. */
#define MMGSDI_ACM_LEN                               3

/** Length of the Accumulated Call Meter Max. */
#define MMGSDI_ACM_MAX_LEN                           3

/** Length of the Service Provider Name. */
#define MMGSDI_SPN_LEN                               17

/** Length of the Price per Unit and Currency Table. */
#define MMGSDI_PUCT_LEN                              5

/** Length of the Phase. */
#define MMGSDI_PHASE_LEN                             1

/** Bit masks to be used to indicate Provisioning Sessions and/or
    Card Sessions to be opened */
#define   MMGSDI_GW_PROV_PRI_SESSION_MASK             0x00000001
#define   MMGSDI_GW_PROV_SEC_SESSION_MASK             0x00000002
#define   MMGSDI_1X_PROV_PRI_SESSION_MASK             0x00000004
#define   MMGSDI_1X_PROV_SEC_SESSION_MASK             0x00000008
#define   MMGSDI_CARD_SESSION_SLOT_1_MASK             0x00000010
#define   MMGSDI_CARD_SESSION_SLOT_2_MASK             0x00000020
#define   MMGSDI_GW_PROV_TER_SESSION_MASK             0x00000040
#define   MMGSDI_1X_PROV_TER_SESSION_MASK             0x00000080
#define   MMGSDI_CARD_SESSION_SLOT_3_MASK             0x00000100

/** Bit masks to be used to register for Set Notify (corresponding to
    provisioning sessions) */
#define   MMGSDI_GW_PROV_PRI_SESSION_SET_NOTIFY_MASK  0x00000001
#define   MMGSDI_GW_PROV_SEC_SESSION_SET_NOTIFY_MASK  0x00000002
#define   MMGSDI_1X_PROV_PRI_SESSION_SET_NOTIFY_MASK  0x00000004
#define   MMGSDI_1X_PROV_SEC_SESSION_SET_NOTIFY_MASK  0x00000008
#define   MMGSDI_GW_PROV_TER_SESSION_SET_NOTIFY_MASK  0x00000010
#define   MMGSDI_1X_PROV_TER_SESSION_SET_NOTIFY_MASK  0x00000020

/** ICCID LEN */
#define MMGSDI_ICCID_LEN                              10

/** Maximum length for Personalization control key */
#define MMGSDI_PERSO_MAX_KEY                          8

/** Size of plmn identity. */
#define   MMGSDI_PLMN_ID_SIZE                         3

/** Maximum length for PLMN short name retrieved from SE13 table */
#define MMGSDI_SHORT_NAME_MAX_LEN                     25

/** Maximum length for PLMN long name retrieved from SE13 table */
#define MMGSDI_LONG_NAME_MAX_LEN                      50

/** Bit masks to be used to indicate network type associated with MNC/MCC */
#define MMGSDI_PLMN_NETWK_TYPE_GSM_900_MASK           0x00000001
#define MMGSDI_PLMN_NETWK_TYPE_DCS_1800_MASK          0x00000002
#define MMGSDI_PLMN_NETWK_TYPE_PCS_1900_MASK          0x00000004
#define MMGSDI_PLMN_NETWK_TYPE_GSM_SAT_MASK           0x00000008
#define MMGSDI_PLMN_NETWK_TYPE_UMTS_MASK              0x00000010
#define MMGSDI_PLMN_NETWK_TYPE_LTE_MASK               0x00000020

/** Max number of 7-bit encoded alphabets required to identify a
    language according to ISO 639 and ISO 15924 */
#define MMGSDI_LANG_ID_LEN_MAX                        8

/** Max number of additional languages (other than the
    default english language) supported for PLMN names by
    MMGSDI */
#define MMGSDI_ADDITIONAL_NAMES_MAX                   4

/* Application identifier max length */
#define MMGSDI_MAX_APP_ID_LEN                         0x10

/** Length of the CDMA random anonymity key */
#define   MMGSDI_CDMA_AKA_RANDA_LEN                   16

/** Fixed length of CDMA AKA Auth response data */
#define   MMGSDI_CDMA_AKA_CK_LEN                      16
#define   MMGSDI_CDMA_AKA_IK_LEN                      16
#define   MMGSDI_CDMA_AKA_AUTS_LEN                    14

/** bitmask with test card type */
#define   MMGSDI_TEST_CARD_GCF_MASK                   0x01
#define   MMGSDI_TEST_CARD_TDS_MASK                   0x02

/** MMGSDI enum range   */
#define MMGSDI_EF_UNDER_MF_START                      0x00
#define MMGSDI_RUIM_EF_START                          0x10
#define MMGSDI_GSM_EF_START                           0x20
#define MMGSDI_GSM_SOLSA_DF_START                     0x21
#define MMGSDI_GSM_MEXE_DF_START                      0x22
#define MMGSDI_TELECOM_EF_START                       0x30
#define MMGSDI_TELECOM_GRAPHICS_DF_START              0x31
#define MMGSDI_TELECOM_PHONEBOOK_DF_START             0x32
#define MMGSDI_TELECOM_MM_DF_START                    0x33
#define MMGSDI_TELECOM_MMSS_DF_START                  0x34
#define MMGSDI_USIM_EF_START                          0x40
#define MMGSDI_USIM_PHONEBOOK_DF_START                0x41
#define MMGSDI_USIM_GSM_ACCESS_DF_START               0x42
#define MMGSDI_USIM_MEXE_DF_START                     0x43
#define MMGSDI_USIM_WLAN_DF_START                     0x44
#define MMGSDI_USIM_HNB_DF_START                      0x45
#define MMGSDI_DCS1800_EF_START                       0x50
#define MMGSDI_ISIM_EF_START                          0x60
#define MMGSDI_ORANGE_SIM_EF_START                    0x70
#define MMGSDI_ORANGE_USIM_EF_START                   0x71
#define MMGSDI_CINGULAR_SIM_EF_START                  0x80
#define MMGSDI_CINGULAR_USIM_EF_START                 0x81
#define MMGSDI_CINGULAR_SIM_NETWORK_DF_START          0x82
#define MMGSDI_CINGULAR_SIM_RPM_DF_START              0x83
#define MMGSDI_CINGULAR_USIM_NETWORK_DF_START         0x84
#define MMGSDI_CINGULAR_USIM_RPM_DF_START             0x85
#define MMGSDI_HZI_EF_START                           0x90
#define MMGSDI_CSIM_EF_START                          0xA0
#define MMGSDI_CSIM_PHONEBOOK_DF_START                0xA1
#define MMGSDI_CSIM_JCDMA_DF_START                    0xA2
#define MMGSDI_EF_PATH_START                          0xB0
#define MMGSDI_NO_FILE_ENUM_START                     0xC0

/* Largest valid SFI value */
#define MMGSDI_VALID_MAX_SFI                          30
/* Invalid SFI value */
#define MMGSDI_INVALID_SFI                            0

/** Maximum number of profiles */
#define MMGSDI_MAX_PROFILE                            2

/** @} */ /* end_addtogroup common_library */
/** @addtogroup common_library
@{ */

/*=============================================================================

                   ENUMERATED DATA DECLARATIONS

=============================================================================*/

/* Accesses the SIM/USIM/RUIM Elementary Files.
MMGSDI_MAX_FILE_ENUM forces the enumerator to be an int32 type.
*/

/*===========================================================================
   ENUM:      MMGSDI_CNF_ENUM_TYPE
=============================================================================*/
/** Command confirmation enumerator that is returned in the user callback.
*/
typedef enum {
  /* 0 */
  MMGSDI_READ_CNF,             /**< Confirms the MMGSDI_READ command. */
  MMGSDI_WRITE_CNF,            /**< Confirms the MMGSDI_WRITE command. */
  MMGSDI_GET_FILE_ATTR_CNF,    /**< Confirms the MMGSDI_GET_FILE_ATTR command. */
  MMGSDI_SESSION_OPEN_CNF,     /**< Confirms the MMGSDI_SESSION_OPEN command. */
  MMGSDI_SESSION_CLOSE_CNF,    /**< Confirms the MMGSDI_SESSION_CLOSE command. */
  MMGSDI_ISIM_AUTH_CNF,        /**< Confirms the MMGSDI_ISIM_AUTH command. */
  MMGSDI_SAP_CONNECT_CNF,      /**< Confirms the MMGSDI_SAP_CONNECT command. */
  MMGSDI_SAP_DISCONNECT_CNF,   /**< Confirms the MMGSDI_SAP_DISCONNECT command. */
  MMGSDI_SAP_GET_ATR_CNF,      /**< Confirms the MMGSDI_SAP_GET_ATR command. */
  MMGSDI_SAP_SEND_APDU_CNF,    /**< Confirms the MMGSDI_SAP_SEND_APDU command. */

  /* 10 */
  MMGSDI_SEND_APDU_CNF,        /**< Confirms the MMGSDI_SEND_APDU command. */
  MMGSDI_SAP_POWER_OFF_CNF,    /**< Confirms the MMGSDI_SAP_POWER_OFF command. */
  MMGSDI_SAP_CARD_READER_STATUS_CNF, /**< Confirms the
                                        MMGSDI_SAP_CARD_READER_STATUS command. */
  MMGSDI_SAP_POWER_ON_CNF,     /**< Confirms the MMGSDI_SAP_POWER_ON command. */
  MMGSDI_SAP_RESET_CNF,        /**< Confirms the MMGSDI_SAP_RESET command. */
  MMGSDI_CARD_PDOWN_CNF,       /**< Confirms the MMGSDI_CARD_PDOWN command. */
  MMGSDI_CARD_PUP_CNF,         /**< Confirms the MMGSDI_CARD_PUP command. */
  MMGSDI_CARD_STATUS_CNF,      /**< Confirms the MMGSDI_CARD_STATUS command. */
  MMGSDI_REFRESH_CNF,          /**< Confirms the MMGSDI_REFRESH command. */
  MMGSDI_ACTIVATE_ONCHIP_SIM_CNF,  /**< Confirms the MMGSDI_ACTIVATE_ONCHIP_SIM
                                        command. */

  /* 20 */
  MMGSDI_REHABILITATE_CNF,     /**< Confirms the MMGSDI_REHABILITATE command. */
  MMGSDI_INVALIDATE_CNF,       /**< Confirms the MMGSDI_INVALIDATE command. */
  MMGSDI_PIN_OPERATION_CNF,    /**< Confirms the MMGSDI_PIN_OPERATION command. */
  MMGSDI_GET_ALL_PIN_STATUS_CNF, /**< Confirms the MMGSDI_GET_ALL_PIN_STATUS
                                      command. */
  MMGSDI_SESSION_DEACTIVATE_CNF, /**< Confirms the MMGSDI_SESSION_DEACTIVATE
                                      command. */
  MMGSDI_SESSION_SELECT_AID_CNF, /**< Confirms the MMGSDI_SESSION_SELECT_AID
                                      command. */
  MMGSDI_SRV_AVAILABLE_CNF,      /**< Confirms the MMGSDI_SRV_AVAILABLE command. */
  MMGSDI_USIM_AUTH_CNF,          /**< Confirms the MMGSDI_USIM_AUTH command. */
  MMGSDI_COMPUTE_IP_AUTH_CNF,    /**< Confirms the MMGSDI_COMPUTE_IP_AUTH
                                      command. */
  MMGSDI_RUN_CAVE_CNF,           /**< Confirms the MMMGSDI_RUN_CAVE command. */

  /* 30 */
  MMGSDI_GENERATE_KEY_VPM_CNF,   /**< Confirms the MMGSDI_GENERATE_KEY_VPM
                                      command. */
  MMGSDI_JCDMA_GET_CARD_INFO_CNF,/**< Confirms the MMGSDI_JCDMA_GET_CARD_INFO
                                      command. */
  MMGSDI_CARD_RESET_CNF,         /**< Confirms the MMGSDI_CARD_RESET command. */
  MMGSDI_GET_ATR_CNF,            /**< Confirms the MMGSDI_GET_ATR command. */
  MMGSDI_BCAST_CNF,             /**< Confirms the MMGSDI_BCAST command. */
  MMGSDI_SEARCH_CNF,            /**< Confirms the MMGSDI_SEARCH command. */
  MMGSDI_GET_ALL_AVAILABLE_APPS_CNF, /**< Confirms the
                                        MMGSDI_GET_ALL_AVAILABLE_APPS command. */
  MMGSDI_SESSION_ACTIVATE_OR_SWITCH_PROVISIONING_CNF, /**< Confirms the
                                  MMGSDI_SESSION_ACTIVATE_OR_SWITCH_PROVISIONING
                                  command. */
  MMGSDI_SESSION_GET_INFO_CNF,      /**< Confirms the MMGSDI_SESSION_GET_INFO
                                         command. */
  MMGSDI_SESSION_RUN_GSM_ALGO_CNF,  /**< Confirms the
                                         MMGSDI_SESSION_RUN_GSM_ALGO command. */

  /* 40 */
  MMGSDI_SESSION_ENABLE_SERVICE_CNF,/**< Confirms the
                                         MMGSDI_SESSION_ENABLE_SERVICE command. */
  MMGSDI_SESSION_DISABLE_SERVICE_CNF, /**< Confirms the
                                       MMGSDI_SESSION_DISABLE_SERVICE command. */
  MMGSDI_SESSION_PERSO_CNF,       /**< Confirms the MMGSDI_SESSION_PERSO
                                       command. */
  MMGSDI_CLIENT_ID_AND_EVT_REG_CNF,   /**< Confirms the
                                       MMGSDI_CLIENT_ID_AND_EVT_REG command. */
  MMGSDI_CLIENT_ID_AND_EVT_DEREG_CNF, /**< Confirms the
                                       MMGSDI_CLIENT_ID_AND_EVT_DEREG command. */
  MMGSDI_SESSION_STORE_ESN_CNF,       /**< Confirms the MMGSDI_SESSION_STORE_ESN
                                           command. */
  MMGSDI_SESSION_PROV_APP_INIT_COMPLETE_CNF, /**< Confirms the
                                          MMGSDI_SESSION_PROV_APP_INIT_COMPLETE
                                          command. */
  MMGSDI_SESSION_GET_APP_CAPABILITIES_CNF, /**< Confirms the
                                          MMGSDI_SESSION_GET_APP_CAPABILITIES
                                          command. */
  MMGSDI_SESSION_GET_CPHS_INFO_CNF, /**< Confirms the
                                         MMGSDI_SESSION_GET_CPHS_INFO command. */
  MMGSDI_SESSION_SSD_UPDATE_CNF,    /**< Confirms the MMGSDI_SESSION_SSD_UPDATE
                                         command. */

  /* 50 */
  MMGSDI_SESSION_SSD_CONFIRM_CNF,   /**< Confirms the MMGSDI_SESSION_SSD_CONFIRM
                                         command. */
  MMGSDI_SESSION_BS_CHAL_CNF,       /**< Confirms the MMGSDI_SESSION_BS_CHAL
                                         command. */
  MMGSDI_SESSION_OTASP_MS_KEY_CNF,  /**< Confirms the MMGSDI_SESSION_OTASP_MS_KEY
                                         command. */
  MMGSDI_SESSION_OTASP_KEY_GEN_CNF, /**< Confirms the
                                         MMGSDI_SESSION_OTASP_KEY_GEN command. */
  MMGSDI_SESSION_OTASP_SSPR_CONFIG_CNF, /**< Confirms the
                                        MMGSDI_SESSION_OTASP_SSPR_CONFIG command. */
  MMGSDI_SESSION_OTASP_COMMIT_CNF, /**< Confirms the MMGSDI_SESSION_OTASP_COMMIT
                                        command. */
  MMGSDI_SESSION_OTASP_OTAPA_CNF,  /**< Confirms the MMGSDI_SESSION_OTASP_OTAPA
                                        command. */
  MMGSDI_SESSION_OTASP_BLOCK_CNF,  /**< Confirms the MMGSDI_SESSION_OTASP_BLOCK
                                        command. */
  MMGSDI_SESSION_READ_PRL_CNF,     /**< Confirms the MMGSDI_SESSION_READ_PRL
                                        command. */
  MMGSDI_SESSION_APP_RESET_DEACTIVATE_CNF, /**< Confirms the
                                           MMGSDI_SESSION_APP_RESET_DEACTIVATE
                                           command. */

  /* 60 */
  MMGSDI_SESSION_APP_RESET_ACTIVATE_CNF, /**< Confirms the
                                              MMGSDI_SESSION_APP_RESET_ACTIVATE
                                              command. */
  MMGSDI_SESSION_SUBSCRIPTION_OK_CNF, /**< Confirms the
                                           MMGSDI_SESSION_SUBSCRIPTION_OK
                                           command */
  MMGSDI_INCREASE_CNF,                /**< Confirms the MMGSDI_INCREASE command. */
  MMGSDI_SESSION_OPEN_EXT_CNF,        /**< Confirms the MMGSDI_SESSION_OPEN_EXT
                                           command. */
  MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_CNF,  /**< Confirms the
                                                 SESSION_OPEN_WITH_SELECT_RSP
                                                 command */
  MMGSDI_SESSION_GET_OPERATOR_NAME_CNF,  /**< confirms the SESSION_GET_OPERATOR_NAME
                                              command*/
  MMGSDI_SEND_APDU_EXT_CNF,         /**< Confirms the MMGSDI_SEND_APDU_EXT command. */
  MMGSDI_SESSION_OPEN_WITH_MF_CNF,  /**< Confirms the
                                         SESSION_OPEN_WITH_MF
                                         command */
  MMGSDI_SESSION_REGISTER_FILE_UPDATE_CNF,  /**< Confirms the
                                                 MMGSDI_SESSION_REGISTER_FILE_UPDATE command. */
  MMGSDI_GSTK_LOC_ENV_RSP_RECEIVED_CNF, /**< Confirms the
                                             MMGSDI_GSTK_LOC_ENV_RSP_RECEIVED_REQ
                                             command */

  /* 70 */
  MMGSDI_GET_SE13_PLMN_NAMES_CNF,  /**< Confirms MMGSDI_GET_SE13_PLMN_NAMES
                                        command */
  MMGSDI_SESSION_UPDATE_ACSGL_RECORD_CNF,  /**< Confirms the
                                                MMGSDI_SESSION_UPDATE_ACSGL_RECORD command. */
  MMGSDI_GET_SE13_PLMN_INFO_BY_NAME_CNF,  /**< Confirms MMGSDI_GET_SE13_PLMN_INFO_BY_NAME
                                               command */
  MMGSDI_CDMA_AKA_CNF,            /**< Confirms the
                                  MMGSDI_SESSION_CDMA_AKA_AUTHENTICATE command. */
  MMGSDI_SET_BUILTIN_PLMN_LIST_CNF,  /**< Confirms MMGSDI_SET_BUILTIN_PLMN_LIST
                                          command */
  MMGSDI_SESSION_ENABLE_FDN_CNF,  /**< Confirms the MMGSDI_SESSION_ENABLE_FDN
                                       command. */
  MMGSDI_SESSION_DISABLE_FDN_CNF, /**< Confirms the MMGSDI_SESSION_DISABLE_FDN
                                       command. */
  MMGSDI_CONTINUE_AFTER_NV_REFRESH_CNF, /**< Confirms the MMGSDI_CONTINUE_AFTER_NV_REFRESH
                                       command. */
  MMGSDI_HANDLE_NV_REFRESH_CNF, /**< Confirms the MMGSDI_HANDLE_NV_REFRESH
                                       command. */
  MMGSDI_RESTRICTED_SIM_ACCESS_CNF, /**< Confirms the MMGSDI_RESTRICTED_SIM_ACCESS
                                       command. */
  MMGSDI_MANAGE_CHANNEL_CNF,         /**< Confirms the MMGSDI_MANAGE_CHANNEL_REQ command */

  /* 80 */
  MMGSDI_SET_SIM_PROFILE_CNF,        /**< Confirms the MMGSDI_SET_SIM_PROFILE
                                          command. */
  MMGSDI_GET_SIM_PROFILE_CNF,        /**< Confirms the MMGSDI_GET_SIM_PROFILE
                                          command. */
  MMGSDI_MAX_CNF_ENUM = 0x7FFFFFFF   /**< Forces an initialize size enumeration. */
}mmgsdi_cnf_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_RETURN_ENUM_TYPE
=============================================================================*/
/** Return status of MMGSDI processes.
*/
typedef enum {
  MMGSDI_SUCCESS              = 0x00000000,   /**< Returned by all functions. */
  MMGSDI_INCORRECT_PARAMS,     /**< Incorrect input parameters. Returned by
                                    all functions. */
  MMGSDI_CMD_QUEUE_FULL,       /**< Maximum number of commands that MMGSDI can
                                    handle has been reached. Returned by all
                                    functions. */
  MMGSDI_ERROR,                /**< General error. Returned by all functions. */
  MMGSDI_ACCESS_DENIED,        /**< Access is denied by the card. Returned by
                                    all functions. */
  MMGSDI_NOT_FOUND,            /**< Files are not found. Returned by all
                                    functions. */
  MMGSDI_INCOMPAT_PIN_STATUS,  /**< PIN or PIN Unblocking key is incompatible.
                                    Returned by PIN functions. */
  MMGSDI_INCORRECT_CODE,       /**< PIN or PIN Unblocking key verification
                                    failed. Returned by PIN functions. */
  MMGSDI_CODE_BLOCKED,         /**< PIN is blocked. Returned by PIN functions. */
  MMGSDI_INCREASE_IMPOSSIBLE,  /**< Unable to perform the increase operation.
                                    Returned by increase functions. */

  MMGSDI_NOT_SUPPORTED,        /**< SIM card does not support the requested file
                                    or function. Returned by all functions. */
  MMGSDI_NOT_INIT,             /**< MMGSDI is not ready. Returned by all
                                    functions. */
  MMGSDI_SUCCESS_BUT_ILLEGAL_SIM, /**< Request was performed successfully, but
                                       the network has marked this SIM/handset
                                       as illegal. Returned by all functions. */
  MMGSDI_AUTH_ERROR_INCORRECT_MAC, /**< USIM error returned during authentication
                                        processing. Returned by the UMTS
                                        Authentication Request message. */
  MMGSDI_AUTH_ERROR_GSM_CNTXT_NOT_SUP, /**< USIM error returned during
                                       authentication processing. Returned by
                                       the UMTS Authentication Request message. */
  MMGSDI_SIM_TECHNICAL_PROBLEMS, /**< USIM OS has encountered an unexpected
                                      error. Returned by all functions. */
  MMGSDI_NO_EF_SELECTED,         /**< SIM/RUIM/USIM has determined that an
                                      access was performed on an EF that has not
                                      been selected. Returned by all functions. */
  MMGSDI_EF_INCONSISTENT,        /**< Action being performed on a file is
                                      inconsistent with the properties of that
                                      file. Returned by all functions. */
  MMGSDI_ERROR_NO_EVENT_NEEDED,  /**< Unhandled error has occurred; MMGSDI has
                                      determined it is not to generate an event
                                      to make the mobile generate limited
                                      service. Returned by all functions. */
  MMGSDI_PIN_NOT_INITIALIZED,    /**< Action being performed on a SIM that is
                                      not initialized. Returned by all PIN
                                      functions. */

  MMGSDI_UNKNOWN_INST_CLASS,     /**< Instruction with an unknown instruction
                                      class was generated and sent to the SIM.
                                      Returned by all functions. */
  MMGSDI_WARNING_NO_INFO_GIVEN,  /**< Internal error in the SIM/RUIM/USIM has
                                      occurred; the card has determined it is
                                      a warning. Returned by all functions. */
  MMGSDI_WARNING_POSSIBLE_CORRUPTION, /**< Data in the SIM/RUIM/USIM may have
                                           been corrupted for the action or
                                           access. Returned by all functions. */
  MMGSDI_INCORRECT_LENGTH,    /**< Parameter that requires a length does not
                                   match that of the SIM/RUIM or USIM. Returned
                                   by all functions. */
  MMGSDI_UIM_CMD_TIMEOUT,     /**< UIM interface to the card has timed out.
                                   Returned by all functions. */
  MMGSDI_CODE_PERM_BLOCKED,   /**< PIN is permanently blocked. Returned by
                                   the PIN functions. */
  MMGSDI_REFRESH_SUCCESS,     /**< Sent to the GSTK task as a result of
                                   completing the REFRESH SIM Application
                                   Toolkit command; valid only with
                                   FEATURE_GSTK. Set in gsdmimain. */
  MMGSDI_REFRESH_IN_PROGRESS, /**< Generic SIM Driver Interface is performing a
                                   REFRESH. Returned by the refresh function. */
  MMGSDI_CAN_NOT_REFRESH,     /**< Set as a result of querying the UI as to
                                   whether the Generic SIM Driver interface can
                                   perform a REFRESH. UI criteria is to ensure
                                   that the ME is not in a call. If the ME is
                                   in a call, REFRESH is not allowed. Returned
                                   by the refresh function. */
  MMGSDI_REFRESH_LATER,       /**< Generic SIM Driver interface will refresh
                                   later. Returned by the Refresh function. */

  MMGSDI_PATHS_DECODE_ERROR,  /**< Set by the UIM convert function when a given
                                   path does not map to uim_item_enum_type.
                                   Returned by the file path conversion
                                   functions. */
  MMGSDI_SUCCESS_AFTER_CARD_INTERNAL_UPDATE,
                               /**< Set when the card returns a 0x92F0 status
                                    word, except for STATUS, SEEK, RUN GSM
                                    ALGORITHM, and SELECT. */
  MMGSDI_SIM_BUSY,             /**< Set when the card returns 0x9300. Returned
                                    by the Envelope command. */
  MMGSDI_INVALIDATION_CONTRADICTION_STATUS,
                               /**< Set when the card returns a 0x9810 status
                                    word for ACCESS, SEEK, INVALIDATE,
                                    and REHABILITATE commands. */
  MMGSDI_INCREASE_MAX_REACHED, /**< Set when the card returns 0x9850 for the
                                    increase request. Returned by the write
                                    operation for the cyclic file for the
                                    increase operation. */
  MMGSDI_AUTH_FAIL,            /**< Set when the card fails the GSM algorithm
                                    process. Set in the run GSM algorithm. */
  MMGSDI_AUTS_FAIL,            /**< Set when USIM/3G/ISIM authentication fails
                                    because of an Authentication Sequence
                                    Counter failure. Set in USIM/3G/ISIM
                                    authentication. */
  MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED, /**< Set when the MMGSDI is unable to get
                                         memory allocation from the heap.
                                         Returned by functions that require
                                         dynamic allocation of memory. */
  MMGSDI_UIM_INTERNALLY_RESET,      /**< UIM interface to the card is
                                         internally reset. */
  MMGSDI_AUTH_ERROR_KEY_FRESHNESS_FAIL, /**< Authentication error: Key Freshness
                                         failure. The last successfully used
                                         MBMS User Key (MUK) does not correspond
                                         to the last generated MUK. */

  MMGSDI_AUTH_ERROR_NO_MEM_SPACE,  /**< Authentication error in MBMS security
                                        context (MSK Update mode) caused by no
                                        no available memory space in the EF
                                        MBMS User Key file. */
  MMGSDI_REF_DATA_NOT_FOUND,       /**< Reference data is not found. */
  MMGSDI_LOCKED_INIT_PARAM,        /**< MediaFLO&tm;&nbsp;parameters are already
                                        initialized. */
  MMGSDI_APP_NOT_INIT,             /**< MediaFLO parameters are not initialized. */
  MMGSDI_PUBLIC_KEY_CERT_NOT_INIT, /**< MediaFLO public key is not initialized. */
  MMGSDI_UCAST_AUTH_KEY_NOT_INIT,  /**< MediaFLO Unicast authentication key
                                        is not initialized. */
  MMGSDI_UCAST_DATA_BLOCK_NOT_INIT, /**< MediaFLO data blocks for a Unicast
                                          message were not received in order. */
  MMGSDI_VERIFY_UCAST_FAILED,       /**< Verification of the MediaFLO Unicast
                                         message and signature failed because
                                         the signature did not match. */
  MMGSDI_REFRESH_SUCCESS_NO_AID_ACTIVE, /**< Application termination succeeded;
                                             no Application Identifier is active. */
  MMGSDI_REFRESH_ADDITIONAL_FILES_READ, /**< ME has read EFs in addition to
                                             those notified by the SIM. */

  MMGSDI_AUTH_ERROR_CAVE_NOT_RUN,  /**< UIM error during authentication
                                        processing. Cellular Authentication and
                                        Voice Encryption command did not run. */
  MMGSDI_REFRESH_FAIL_INVALID_AID, /**< Application reset failed due to an
                                        invalid Application Identifier. */
  MMGSDI_RPC_ERROR,                /**< Remote procedure call error returned for
                                        a failed function call. Returned by all
                                        functions. */
  MMGSDI_WRONG_CLASS,              /**< Instruction with incorrect instruction
                                        class was generated and sent to the SIM.
                                        Returned by all functions. */
  MMGSDI_PERSO_CHECK_FAILED,       /**< Personalization check failed. */
  MMGSDI_PERSO_INVALID_CK,         /**< Control key verification failed. */
  MMGSDI_PERSO_CK_BLOCKED,         /**< Wrong control key verification exceeded
                                        the maximum. */
  MMGSDI_PERSO_INVALID_DATA,       /**< Personalization data is not to
                                        specification. */
  MMGSDI_PERSO_PERM_DISABLED,      /**< Personalization feature is permanently
                                        disabled. */
  MMGSDI_MANAGE_CHANNEL_FAILED,    /**< Attempt to open logical channel
                                        failed */

  MMGSDI_SELECT_AID_FAILED,        /**< Attempt to select application failed */
  MMGSDI_UIM_INSTN_CHG_SUCCESS,    /**< A command to reset card with new
                                        instruction class is successful and
                                        is in progress */
  MMGSDI_APP_STATE_DENIED,         /**< Command is not allowed in APP state,
                                        specifically use during provisioning
                                        application initalization */
  MMGSDI_PERSO_CARD_DATA_ERROR,    /**< Personalization error due to invalid
                                        Card data */
  MMGSDI_APP_NOT_SHAREABLE,        /**< Application cannot be shared among
                                        logical channels */
  MMGSDI_CMD_QUEUED_TO_SLOT_TASK,  /**< Command is queued to slot specific task */
  MMGSDI_CSG_UPDATE_FAILED_FILE_FULL,  /**< Update EF-ACSGL record error due to
                                            no empty records or records with
                                            sufficient space for new csg id tlv */
  MMGSDI_CSG_NOT_FOUND,                /**< CSG Id to delete is not found in
                                            EF-ACSGL records */
  MMGSDI_MEMORY_ERROR,             /**< Card returned error with memory management
                                        during execution */
  MMGSDI_RESTRICTED_IN_ONCHIP_MODE,/**< Command is retricted/blocked when ONCHIP
                                        is active. */
  MMGSDI_MULTI_PROFILE_NO_EFFECT,  /**< No change of profile in the card after
                                        switch profile command*/
  MMGSDI_MULTI_PROFILE_NOT_SUPPORTED, /** Multiple profiles are not suppported */
  MMGSDI_HALT_INIT_ERROR,             /** Halt initialization */
  MMGSDI_MAX_RETURN_ENUM       = 0x7FFFFFFF /**< Forces an initialize size
                                                 enumeration. */
}mmgsdi_return_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_EVENTS_ENUM_TYPE
=============================================================================*/
/**  Return events from MMGSDI processes.
*/
typedef enum {
  /* CARD RELATED EVENTS – RECEIVED WITH CLIENT ID Event REG */
  MMGSDI_CARD_INSERTED_EVT = 0x00000000,       /**< Card inserted. */
  MMGSDI_CARD_ERROR_EVT,                       /**< Card error. */
  MMGSDI_CARD_REMOVED_EVT,                     /**< Card removed. */
  MMGSDI_SIM_BUSY_EVT,                         /**< SIM busy. */
  MMGSDI_TEST_SIM_EVT,                         /**< Updating test SIM status and if
                                                    status is changed from previous
                                                    card */
  MMGSDI_PROFILE_DATA_EVT,                     /**< Profile data */
  MMGSDI_SAP_CONNECT_EVT = 0x00000006,         /**< SAP is connected as a server
                                                    or client. */
  MMGSDI_SAP_DISCONNECT_EVT,                   /**< SAP server or client was
                                                    disconnected. */
  MMGSDI_SESSION_CLOSE_EVT,                    /**< Session was closed. */
  MMGSDI_PERSO_TEMPORARY_UNLOCK_EVT,                      /**< Perso Temporary unlock information event*/
  MMGSDI_TERMINAL_PROFILE_DL_EVT = 0x0000000B, /**< Terminal Profile DL event */

  /* APP RELATED EVENTS – RECEIVED WITH Session ID Reg */
  MMGSDI_SUBSCRIPTION_READY_EVT, /**< Subscription ready event. After PIN1_EVT
                                      and the session ID are assigned to a
                                      channel, the client can start making
                                      requests of the application. */
  MMGSDI_SESSION_CHANGED_EVT, /**< Session changed event. The provisioning
                                   application changed, so the client's session
                                   is remapped to the new provisioning
                                   application automatically. Opening a session
                                   for a nonprovisioning application by using an
                                   AID can also produce this event. */
  MMGSDI_PIN1_EVT,            /**< PIN1 authentication event. */
  MMGSDI_PIN2_EVT,            /**< PIN2 authentication event. */
  MMGSDI_UNIVERSAL_PIN_EVT,   /**< Universal PIN authentication event. */
  MMGSDI_REFRESH_EVT,         /**< Application or card refresh event. */
  MMGSDI_FDN_EVT,             /**< Enable or disable fixed dialing number. */
  MMGSDI_PERSO_EVT,           /**< Personalization event. */
  MMGSDI_SESSION_ILLEGAL_SUBSCRIPTION_EVT,
                              /**< Attempt was made to use an illegal
                                   subscription profile for UMTS or CDMA
                                   acquisition. */
  MMGSDI_HIDDENKEY_EVT,       /**< Hidden Key event. */
  MMGSDI_OTA_COMMIT_RSP_EVT,  /**< OTA commit response indication event. */
  MMGSDI_EONS_EVT,            /**< EONS Event. */
  MMGSDI_FILE_UPDATE_EVT,     /**< File update event. */
  MMGSDI_EONS_OPL_PNN_CACHE_READY_EVT, /**< EONS opl pnn cache ready event*/
  MMGSDI_ECC_EVT,             /**< EF-ECC data read event. */
  MMGSDI_SESSION_LEGAL_SUBSCRIPTION_EVT,
                              /**< Updating an illegal subscription
                                   back as ready */
  MMGSDI_PROFILE_SWITCHING_EVT, /** < Switching of profile in the card is
                                      in progress */
  MMGSDI_MAX_EVT_ENUM      = 0x7FFFFFFF /**< Forces an initialize size
                                             enumeration. */
}mmgsdi_events_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_SAP_STATE_ENUM_TYPE
=============================================================================*/
/** Indicates the state during Service Access Point connect/disconnect.
*/
typedef enum {
  MMGSDI_SAP_CONNECTING       = 0x00000000, /**< &nbsp; */
  MMGSDI_SAP_CONNECTED,                     /**< &nbsp; */
  MMGSDI_SAP_CONNECT_FAIL,                  /**< &nbsp; */
  MMGSDI_SAP_DISCONNECTING,                 /**< &nbsp; */
  MMGSDI_SAP_DISCONNECTED,                  /**< &nbsp; */
  MMGSDI_SAP_DISCONNECT_FAIL,               /**< &nbsp; */
  MMGSDI_MAX_SAP_STATE_ENUM   = 0x7FFFFFFF /**< Forces an initialize size
                                                enumeration. */
}mmgsdi_sap_state_enum_type;

/*=============================================================================
ENUM:    MMGSDI_FILE_ENUM_TYPE
=============================================================================*/

/** Accesses the SIM/USIM/RUIM Elementary Files.

    MMGSDI_MAX_FILE_ENUM forces the enumerator to be an int32 type.
*/
typedef enum {

  /* EFs at the MF level */
  /* (0x0)00 => 0 */
  MMGSDI_ICCID          = (MMGSDI_EF_UNDER_MF_START << 8),
  MMGSDI_ELP,                      /**< Extended Language Preference. */
  MMGSDI_DIR,                      /**< Electronic File Directory for a USIM. */
  MMGSDI_ARR,                      /**< Access Rule Reference. */

  /* EFs at CDMA level */
  /* (0x1)00 => 0 */
  MMGSDI_CDMA_CC        = (MMGSDI_RUIM_EF_START << 8),

  MMGSDI_CDMA_IMSI_M,              /**< MIN-based International Mobile
                                        Subscriber Identity. */
  MMGSDI_CDMA_IMSI_T,              /**< True International Mobile Subscriber
                                        Identity. */
  MMGSDI_CDMA_TMSI,                /**< Temporary Mobile Subscriber Identity. */
  MMGSDI_CDMA_ANALOG_HOME_SID,     /**< Analog home Service ID. */
  MMGSDI_CDMA_ANALOG_OP_PARAMS,    /**< Analog operational parameters. */

  MMGSDI_CDMA_ANALOG_LOCN_AND_REGN_IND,/**< Analog location and region
                                            indicators. */
  MMGSDI_CDMA_HOME_SID_NID,        /**< CDMA home Service ID and Network ID. */
  MMGSDI_CDMA_ZONE_BASED_REGN_IND, /**< CDMA zone-based region indicators. */
  MMGSDI_CDMA_SYS_REGN_IND,        /**< CDMA system/network region indicators. */

  /* (0x1)0A => 10 */
  MMGSDI_CDMA_DIST_BASED_REGN_IND, /**< CDMA distance-based region indicators. */
  MMGSDI_CDMA_ACCOLC,              /**< Access Overload Class. */
  MMGSDI_CDMA_CALL_TERM_MODE_PREF, /**< Call termination mode preferences. */
  MMGSDI_CDMA_SCI,                 /**< Suggested Slot Cycle Index. */
  MMGSDI_CDMA_ANALOG_CHAN_PREF,    /**< Analog channel preferences. */
  MMGSDI_CDMA_PRL,                 /**< Preferred Roaming List. */
  MMGSDI_CDMA_RUIM_ID,             /**< Removable UIM ID. */
  MMGSDI_CDMA_SVC_TABLE,           /**< CDMA service table. */
  MMGSDI_CDMA_SPC,                 /**< Service Programming Code. */
  MMGSDI_CDMA_OTAPA_SPC_ENABLE,    /**< Over-The-Air Parameter Administration
                                         Service Programming Code enable. */

  /* (0x1)14 => 20*/
  MMGSDI_CDMA_NAM_LOCK,            /**< Number Assignment Module lock. */
  MMGSDI_CDMA_OTASP_OTAPA_FEATURES, /**< Over-The-Air Service Provisioning/
                                         Over-The-Air Service Provisioning
                                         Over-The-Air Parameter Administration
                                         parameters. */
  MMGSDI_CDMA_SERVICE_PREF,     /**< Service preferences. */
  MMGSDI_CDMA_ESN_ME,           /**< ESN for the ME. */
  MMGSDI_CDMA_RUIM_PHASE,       /**< Removable UIM phase. */
  MMGSDI_CDMA_PREF_LANG,        /**< Preferred languages. */
  MMGSDI_CDMA_UNASSIGNED_1,     /**< Not yet assigned. */
  MMGSDI_CDMA_SMS,              /**< Short Message Service. */
  MMGSDI_CDMA_SMS_PARAMS,       /**< Short Message Service Parameters. */
  MMGSDI_CDMA_SMS_STATUS,       /**< Short Message Service Status. */

  /* (0x1)1E => 30 */
  MMGSDI_CDMA_SUP_SVCS_FEATURE_CODE_TABLE, /**< Supplemental Services feature
                                                code table. */
  MMGSDI_CDMA_UNASSIGNED_2,       /**< Not yet assigned. */
  MMGSDI_CDMA_HOME_SVC_PVDR_NAME, /**< CDMA home service provider name. */
  MMGSDI_CDMA_UIM_ID_USAGE_IND,   /**< Removable UIM ID usage indicator. */
  MMGSDI_CDMA_ADM_DATA,           /**< CDMA administrative data. */
  MMGSDI_CDMA_MSISDN,             /**< Mobile Station International Subsciber
                                       Directory Number. */
  MMGSDI_CDMA_MAXIMUM_PRL,      /**< Maximum size of the Preferred Roaming List. */
  MMGSDI_CDMA_SPC_STATUS,       /**< Service Programming Code status. */
  MMGSDI_CDMA_ECC,              /**< Emergency Call Codes. */
  MMGSDI_CDMA_3GPD_ME3GPDOPC,   /**< Operational Capabilities. */

  /* (0x1)28 => 40 */
  MMGSDI_CDMA_3GPD_3GPDOPM,     /**< Operational Mode. */
  MMGSDI_CDMA_3GPD_SIPCAP,      /**< Simple IP Capabilities. */
  MMGSDI_CDMA_3GPD_MIPCAP,      /**< Mobile IP Capabilities. */
  MMGSDI_CDMA_3GPD_SIPUPP,      /**< Simple IP User Profile Parameters. */
  MMGSDI_CDMA_3GPD_MIPUPP,      /**< Mobile IP User Profile Parameters. */
  MMGSDI_CDMA_3GPD_SIPSP,       /**< Simple IP Status Parameters. */
  MMGSDI_CDMA_3GPD_MIPSP,       /**< Mobile IP Status Parameters. */
  MMGSDI_CDMA_3GPD_SIPPAPSS,    /**< Simple IP Password Authentication Protocol
                                     Shared Secret parameters. */
  MMGSDI_CDMA_UNASSIGNED_3,     /**< Reserved. */
  MMGSDI_CDMA_UNASSIGNED_4,     /**< Reserved. */

  /* (0x1)32 => 50 */
  MMGSDI_CDMA_PUZL,             /**< Preferred User Zone List. */
  MMGSDI_CDMA_MAXPUZL,          /**< Maximum Preferred User Zone List. */
  MMGSDI_CDMA_MECRP,            /**< ME-specific Configuration Request
                                     Parameters. */
  MMGSDI_CDMA_HRPDCAP,          /**< HRPD access authorization Capability
                                     Parameters. */
  MMGSDI_CDMA_HRPDUPP,          /**< HRPD access authorization User Profile
                                     Parameters. */
  MMGSDI_CDMA_CSSPR,            /**< Current System Selection Preferred Roaming
                                     protocol revision. */
  MMGSDI_CDMA_ATC,              /**< Access Terminal Class. */
  MMGSDI_CDMA_EPRL,             /**< Extended Preferred Roaming List. */
  MMGSDI_CDMA_BCSMS_CONFIG,     /**< Broadcast SMS configuration. */
  MMGSDI_CDMA_BCSMS_PREF,       /**< Broadcast SMS preferences. */

  /* (0x1)3C => 60 */
  MMGSDI_CDMA_BCSMS_TABLE,      /**< Broadcast SMS table. */
  MMGSDI_CDMA_BCSMS_PARAMS,     /**< Broadcast SMS parameters. */
  MMGSDI_CDMA_MMS_NOTIF,        /**< Multimedia Messaging Service notification. */
  MMGSDI_CDMA_MMS_EXT8,         /**< Multimedia Messaging Service extension 8. */
  MMGSDI_CDMA_MMS_ICP,          /**< Multimedia Messaging Service Issuer
                                     Connectivity Parameters. */
  MMGSDI_CDMA_MMS_UP,           /**< Multimedia Messaging Service User
                                     Preferences. */
  MMGSDI_CDMA_SMS_CAP,          /**< Short Message Service Capabilities. */
  MMGSDI_CDMA_3GPD_IPV6CAP,     /**< IPv6 capabilities. */
  MMGSDI_CDMA_3GPD_MIPFLAGS,    /**< Mobile IP flags. */
  MMGSDI_CDMA_3GPD_TCPCONFIG,   /**< TCP configuration. */

  /* (0x1)46 => 70 */
  MMGSDI_CDMA_3GPD_DGC,         /**< Data Generic Configuration. */
  MMGSDI_CDMA_BROWSER_CP,       /**< Browser Connectivity Parameters. */
  MMGSDI_CDMA_BROWSER_BM,       /**< Browser Bookmarks. */
  MMGSDI_CDMA_3GPD_SIPUPPEXT,   /**< Simple IP User Profile Parameters
                                     extension. */
  MMGSDI_CDMA_MMS_CONFIG,       /**< Multimedia Messaging Service configuration. */
  MMGSDI_CDMA_JAVA_DURL,        /**< Java Download URL. */
  MMGSDI_CDMA_3GPD_MIPUPPEXT,   /**< Mobile IP User Profile Parameters extension. */
  MMGSDI_CDMA_BREW_DLOAD,       /**< BREW<sup>&amp;reg;</sub> download. */
  MMGSDI_CDMA_BREW_TSID,        /**< BREW Teleservice ID. */
  MMGSDI_CDMA_BREW_SID,         /**< BREW Subscriber ID. */

  /* (0x1)50 => 80 */
  MMGSDI_CDMA_LBS_XCONFIG,      /**< Location-Based Services XTRA configuration. */
  MMGSDI_CDMA_LBS_XSURL,        /**< Location-Based Services XTRA Server URLs. */
  MMGSDI_CDMA_LBS_V2CONFIG,     /**< Location-Based Services V2 configuration. */
  MMGSDI_CDMA_LBS_V2PDEADDR,    /**< Location-Based Services V2 Position
                                   Determination Entity Address. */
  MMGSDI_CDMA_LBS_V2MPCADDR,    /**< Location-Based Services V2 Mobile
                                     Positioning Center Address. */
  MMGSDI_CDMA_BREW_AEP,         /**< BREW Application Execution Policy. */
  MMGSDI_CDMA_MODEL,                /**< Model information. */
  MMGSDI_CDMA_RC,                   /**< Root Certificate. */
  MMGSDI_CDMA_APP_LABELS,           /**< Applications labels. */
  MMGSDI_CDMA_USER_AGENT_STRING,    /**< Japanese CDMA agent string. */

  /* (0x1)5A => 90 */
  MMGSDI_CDMA_GID_REFERENCES,       /**< JCDMA Group Identifier references. */
  MMGSDI_CDMA_ME_DOWNLOADABLE_DATA, /**< JCDMA ME downloadable data. */
  MMGSDI_CDMA_ME_SETTING_DATA,      /**< JCDMA ME setting data. */
  MMGSDI_CDMA_ME_USER_DATA,         /**< JCDMA user data. */
  MMGSDI_CDMA_RESERVED2,            /**< JCDMA reserved. */
  MMGSDI_CDMA_RESERVED1,            /**< JCDMA reserved. */
  MMGSDI_CDMA_UIM_SVC_TABLE,  /**< JCDMA UIM service table. */
  MMGSDI_CDMA_UIM_FEATURES,   /**< JCDMA UIM features. */
  MMGSDI_CDMA_HRPD_HRPDUPP,   /**< HRPD User Profile Parameters. */
  MMGSDI_CDMA_SF_EUIM_ID,     /**< Short Form Expanded UIM ID. */

  /* (0x1)64 => 100 */
  /* JCDMA EFs. Certain JCDMA EFs are listed as JCDMA EFs. */
  MMGSDI_CDMA_UIM_VERSION,    /**< JCDMA signature and version. */
  MMGSDI_CDMA_IMSI_STATUS,    /**< JCDMA International Mobile Subscriber
                                   Identity status. */
  MMGSDI_CDMA_GID1,           /**< JCDMA service provider group ID. */
  MMGSDI_CDMA_GID2,           /**< JCDMA user group ID. */

  /* 3GPD */
  MMGSDI_CDMA_3GPD_OP_CAP,    /**< 3G packed data operational capacity. */
  MMGSDI_CDMA_3GPD_UPPEXT,    /**< User Profile Parameters extension. */

  /* EFs in DF GSM */
  /* (0x2)00 => 0 */
  MMGSDI_GSM_LP         = (MMGSDI_GSM_EF_START << 8),

  MMGSDI_GSM_IMSI,            /**< International Mobile Subscriber Identity. */
  MMGSDI_GSM_KC,              /**< Key Cipher. */
  MMGSDI_GSM_PLMN,            /**< Public Land Mobile Network selector. */
  MMGSDI_GSM_HPLMN,           /**< Home Public Land Mobile Networks search
                                   period. */
  MMGSDI_GSM_ACM_MAX,         /**< Accumulated Call Meter maximum value. */
  MMGSDI_GSM_SST,             /**< SIM Service Table. */
  MMGSDI_GSM_ACM,             /**< Accumulated Call Meter. */
  MMGSDI_GSM_GID1,            /**< Group Identifier Level 1. */
  MMGSDI_GSM_GID2,            /**< Group Identifier Level 2. */

  /* (0x2)0A => 10 */
  MMGSDI_GSM_SPN,             /**< Service Provider Name. */
  MMGSDI_GSM_PUCT,            /**< Price per Unit and Currency Table. */
  MMGSDI_GSM_CBMI,            /**< Cell Broadcast Message Identifier selection. */
  MMGSDI_GSM_BCCH,            /**< Broadcast Control Channels. */
  MMGSDI_GSM_ACC,             /**< Access Control Class. */
  MMGSDI_GSM_FPLMN,           /**< Forbidden Public Land Mobile Networks. */
  MMGSDI_GSM_LOCI,            /**< Location Information. */
  MMGSDI_GSM_AD,              /**< Administrative Data. */
  MMGSDI_GSM_PHASE,           /**< Phase identification. */
  MMGSDI_GSM_VGCS,            /**< Voice Group Call Service. */

  /* (0x2)14 => 20 */
  MMGSDI_GSM_VGCSS,           /**< Voice Group Call Service Status. */
  MMGSDI_GSM_VBS,             /**< Voice Broadcast Service. */
  MMGSDI_GSM_VBSS,            /**< Voice Broadcast Service Status. */
  MMGSDI_GSM_EMLPP,           /**< Enhanced Multi-Level Preemption and Priority. */
  MMGSDI_GSM_AAEM,            /**< Automatic Answer for the Enhanced
                                   Multi-Level Preemption and Priority service. */
  MMGSDI_GSM_CBMID,           /**< Cell Broadcast Message ID for data download. */
  MMGSDI_GSM_ECC,             /**< Emergency Call Codes. */
  MMGSDI_GSM_CBMIR,           /**< Cell Broadcast Message ID Range selection. */
  MMGSDI_GSM_DCK,             /**< Depersonalization Control Key. */
  MMGSDI_GSM_CNL,             /**< Cooperative Network List. */

  /* (0x2)1E => 30 */
  MMGSDI_GSM_NIA,             /**< Network's Indication of Alerting. */
  MMGSDI_GSM_KCGPRS,          /**< Key cipher for General Packet Radio Services. */
  MMGSDI_GSM_LOCIGPRS,        /**< Location Information for GPRS. */
  MMGSDI_GSM_SUME,            /**< Setup Menu Elements. */
  MMGSDI_GSM_PLMNWACT,        /**< Public Land Mobile Networks selector With
                                   Access Technology. */
  MMGSDI_GSM_OPLMNWACT,       /**< Operator-controlled Public Land Mobile
                                   Networks selector With Access Technology. */
  MMGSDI_GSM_HPLMNACT,        /**< Home PLMN Access Technology. */
  MMGSDI_GSM_CPBCCH,          /**< Compact Packet Broadcast Control Channel
                                   information. */
  MMGSDI_GSM_INVSCAN,         /**< Investigation PLMN Scan. */
  MMGSDI_GSM_RPLMNAT,         /**< Registered PLMN last used Access Technology. */

  /* (0x2)28 => 40 */
  MMGSDI_GSM_PNN,             /**< PLMN Network Name. */
  MMGSDI_GSM_OPL,             /**< Operator PLMN List. */
  MMGSDI_GSM_MBDN,            /**< Mail Box Dialing Number. */
  MMGSDI_GSM_EXT6,            /**< Extension 6. */
  MMGSDI_GSM_MBI,             /**< Mail Box Identifier. */
  MMGSDI_GSM_MWIS,            /**< Message Waiting Indication Status. */
  MMGSDI_GSM_EXT1,            /**< Extension 1. */
  MMGSDI_GSM_SPDI,            /**< Service Provider Display Information. */

  /* EFs for CPHS */
  MMGSDI_GSM_CFIS,            /**< CPHS Call Forwarding Indication Status. */
  MMGSDI_GSM_VMWI,            /**< CPHS Voice Mail Waiting Indicator. */

  /* (0x2)32 => 50 */
  MMGSDI_GSM_SVC_STR_TBL,     /**< CPHS service string table. */
  MMGSDI_GSM_CFF,             /**< CPHS Call Forwarding Flag. */
  MMGSDI_GSM_ONS,             /**< CPHS Operator Name String. */
  MMGSDI_GSM_CSP,             /**< CPHS Customer Service Profile. */
  MMGSDI_GSM_CPHSI,           /**< CPHS Information. */
  MMGSDI_GSM_MN,              /**< CPHS Mailbox Number. */

  /* image file */
  /* need to move to telecom, also need to find out the path*/
  MMGSDI_GSM_IMG,             /**< Single image file. */
  MMGSDI_GSM_IMG1INST1,       /**< Image 1, instance 1. */
  MMGSDI_GSM_IMG1INST2,       /**< Image 1, instance 2. */
  MMGSDI_GSM_IMG1INST3,       /**< Image 1, instance 3. */
  MMGSDI_GSM_IMG2INST1,       /**< Image 2, instance 1. */
  MMGSDI_GSM_IMG2INST2,       /**< Image 2, instance 2. */
  MMGSDI_GSM_IMG2INST3,       /**< Image 2, instance 3. */
  MMGSDI_GSM_IMG3INST1,       /**< Image 3, instance 1. */
  MMGSDI_GSM_IMG3INST2,       /**< Inage 3, instance 2. */
  MMGSDI_GSM_IMG3INST3,       /**< Image 3, instance 3.*/
  MMGSDI_GSM_ONS_SHORT,     /**< CPHS Operator Name Short String. */

    /* EFs at SoLSA */
  /* (0x21)00 => 0 */
  MMGSDI_GSM_SAI          = (MMGSDI_GSM_SOLSA_DF_START << 8),
                              /**< SoLSA Access Indicator. */
  MMGSDI_GSM_SLL,             /**< SoLSA LSA List. */

  /* EFs at MExE Level */
  /* (0x22)00 => 0 */
  MMGSDI_GSM_MEXE_ST      = (MMGSDI_GSM_MEXE_DF_START << 8),
                              /**< MExE Service Table. */
  MMGSDI_GSM_ORPK,            /**< MExE Operator Root Public Key. */
  MMGSDI_GSM_ARPK,            /**< MExE Administrator Root Public Key. */
  MMGSDI_GSM_TPRPK,           /**< Third-Party Root Public Key. */

  /* EFs at the telecom DF in GSM and CDMA */
  /* (0x3)00 => 0 */
  MMGSDI_TELECOM_ADN      = (MMGSDI_TELECOM_EF_START << 8),
                              /**< Abbreviated Dialing Numbers. */
  MMGSDI_TELECOM_FDN,         /**< Fixed Dialing Numbers. */
  MMGSDI_TELECOM_SMS,         /**< Short Message Service. */
  MMGSDI_TELECOM_CCP,         /**< Capability Configuration Parameters. */
  MMGSDI_TELECOM_ECCP,        /**< Extended CCP. */
  MMGSDI_TELECOM_MSISDN,      /**< Mobile Station International Subscriber
                                   Directory Number. */
  MMGSDI_TELECOM_SMSP,        /**< Short Message Service Parameters. */
  MMGSDI_TELECOM_SMSS,        /**< Short Message Service Status. */
  MMGSDI_TELECOM_LND,         /**< Last Number Dialed. */
  MMGSDI_TELECOM_SDN,         /**< Service Dialing Numbers. */

  /* (0x3)0A => 10 */
  MMGSDI_TELECOM_EXT1,        /**< Extension 1. */
  MMGSDI_TELECOM_EXT2,        /**< Extension 2. */
  MMGSDI_TELECOM_EXT3,        /**< Extension 3. */
  MMGSDI_TELECOM_BDN,         /**< Barred Dialing Numbers. */
  MMGSDI_TELECOM_EXT4,        /**< Extension 4. */
  MMGSDI_TELECOM_SMSR,        /**< Short Message Service Reports. */
  MMGSDI_TELECOM_CMI,         /**< Comparison Method Information. */
  MMGSDI_TELECOM_SUME,        /**< Setup Menu Elements. */
  MMGSDI_TELECOM_ARR,         /**< Access Rule Reference. */
  MMGSDI_TELECOM_ICE_DN,      /**< In Case of Emergency – Dialling Number. */

  /* (0x3)14 => 20 */
  MMGSDI_TELECOM_ICE_FF,      /**< In Case of Emergency – Free Format. */
  MMGSDI_TELECOM_PSISMSC,     /**< Public Service Identity of the SM-SC. */

  /* EFs at DF GRAPHICS under DF Telecom */
  /* (0x31)00 => 0 */
  MMGSDI_IMAGE          = (MMGSDI_TELECOM_GRAPHICS_DF_START << 8),
                               /**< Single image file. */
  MMGSDI_IMAGE_FILE,           /**< Image instance data file. */
  MMGSDI_TELECOM_ICE_GRAPHICS, /**< In Case of Emergency – Graphics. */

  /* EFs at the DF PHONEBOOK under  DF Telecom */
  /* (0x32)00 => 0 */
  MMGSDI_TELECOM_PBR    = (MMGSDI_TELECOM_PHONEBOOK_DF_START << 8),
                              /**< Phone Book Reference file. */
  MMGSDI_TELECOM_PSC,         /**< Phone book Synchronization Center. */
  MMGSDI_TELECOM_CC,          /**< Change Counter. */
  MMGSDI_TELECOM_PUID,        /**< Previous Unique Identifier. */

  /* EFs at the DF Multimedia under DF Telecom */
  /* (0x33)00 => 0 */
  MMGSDI_TELECOM_MML    = (MMGSDI_TELECOM_MM_DF_START << 8),
                              /**< Multimedia Messages List. */
  MMGSDI_TELECOM_MMDF,        /**< Multimedia Messages Data File. */

  /* EFs at the DF MultiMode System Selection under DF Telecom */
  /* (0x34)00 => 0 */
  MMGSDI_TELECOM_MLPL   = (MMGSDI_TELECOM_MMSS_DF_START << 8),
                              /**< MMSS Location Assisted Priority List. */
  MMGSDI_TELECOM_MSPL,        /**< MMSS System Priority List. */
  MMGSDI_TELECOM_MMSSMODE,    /**< MultiMode System Selection Mode settings. */

  /* EFs of USIM ADF */
  /* (0x4)00 => 0 */
  MMGSDI_USIM_LI        = ( MMGSDI_USIM_EF_START << 8),
                              /**< Language Indication. */
  MMGSDI_USIM_IMSI,           /**< International Mobile Subscriber Identity. */
  MMGSDI_USIM_KEYS,           /**< Ciphering and integrity keys. */
  MMGSDI_USIM_KEYSPS,         /**< Ciphering and integrity keys for the Packet
                                   Switched domain. */
  MMGSDI_USIM_PLMNWACT,       /**< User-controlled Public Land Mobile Networks
                                   selector With Access Technology. */
  MMGSDI_USIM_UPLMNSEL,       /**< User-controlled Public Land Mobile Networks
                                   Selector. */
  MMGSDI_USIM_HPLMN,          /**< Home PLMN search period. */
  MMGSDI_USIM_ACM_MAX,        /**< Accumulated Call Meter maximum value. */
  MMGSDI_USIM_UST,            /**< USIM Service Table. */
  MMGSDI_USIM_ACM,            /**< Accumulated Call Meter. */

  /* (0x4)0A => 10 */
  MMGSDI_USIM_GID1,           /**< Group Identifier level 1. */
  MMGSDI_USIM_GID2,           /**< Group Identifier level 2. */
  MMGSDI_USIM_SPN,            /**< Service Provider Name. */
  MMGSDI_USIM_PUCT,           /**< Price per Unit and Currency Table. */
  MMGSDI_USIM_CBMI,           /**< Cell Broadcast Message Identifier selection. */
  MMGSDI_USIM_ACC,            /**< Access Control Class. */
  MMGSDI_USIM_FPLMN,          /**< Forbidden PLMNs. */
  MMGSDI_USIM_LOCI,           /**< Location Information. */
  MMGSDI_USIM_AD,             /**< Administrative Data. */
  MMGSDI_USIM_CBMID,          /**< Cell Broadcast Message Identifier for data
                                   Download. */

  /* (0x4)14 => 20 */
  MMGSDI_USIM_ECC,            /**< Emergency Call Codes. */
  MMGSDI_USIM_CBMIR,          /**< Cell Broadcast Message Identifier Range
                                   selection. */
  MMGSDI_USIM_PSLOCI,         /**< Packet Switched Location Information. */
  MMGSDI_USIM_FDN,            /**< Fixed Dialing Numbers. */
  MMGSDI_USIM_SMS,            /**< Short Message Service. */
  MMGSDI_USIM_MSISDN,         /**< Mobile Station International Subscriber
                                   Directory Number. */
  MMGSDI_USIM_SMSP,           /**< Short Message Service Parameters. */
  MMGSDI_USIM_SMSS,           /**< Short Message Service Status. */
  MMGSDI_USIM_SDN,            /**< Service Dialing Numbers. */
  MMGSDI_USIM_EXT2,           /**< Extension 2. */

  /* (0x4)1E => 30 */
  MMGSDI_USIM_EXT3,           /**< Extension 3. */
  MMGSDI_USIM_SMSR,           /**< Short Message Service Reports. */
  MMGSDI_USIM_ICI,            /**< Incoming Call Information. */
  MMGSDI_USIM_OCI,            /**< Outgoing Call Information. */
  MMGSDI_USIM_ICT,            /**< Incoming Call Timer. */
  MMGSDI_USIM_OCT,            /**< Outgoing Call Timer. */
  MMGSDI_USIM_EXT5,           /**< Extension 5. */
  MMGSDI_USIM_CCP2,           /**< Capability Configuration Parameters 2. */
  MMGSDI_USIM_EMLPP,          /**< Enhanced Multi-Level Precedence and
                                   Preemption. */
  MMGSDI_USIM_AAEM,           /**< Automatic Answer for EMLPP service. */

  /* (0x4)28 => 40 */
  MMGSDI_USIM_GMSI,           /**< Group identity. */
  MMGSDI_USIM_HIDDENKEY,      /**< Key for hidden phone book entries. */
  MMGSDI_USIM_BDN,            /**< Barred Dialing Numbers. */
  MMGSDI_USIM_EXT4,           /**< Extension 4. */
  MMGSDI_USIM_CMI,            /**< Comparison Method Information. */
  MMGSDI_USIM_EST,            /**< Enabled Services Table. */
  MMGSDI_USIM_ACL,            /**< Access Control List. */
  MMGSDI_USIM_DCK,            /**< Depersonalization Control Key. */
  MMGSDI_USIM_CNL,            /**< Cooperative Network List. */
  MMGSDI_USIM_START_HFN,      /**< Initialization values for Hyper Frame Number. */

  /* (0x4)32 => 50 */
  MMGSDI_USIM_THRESHOLD,      /**< Maximum value of START. */
  MMGSDI_USIM_OPLMNWACT,      /**< Operator-controlled Public Land Mobile
                                   Networks selector With Access Technology. */
  MMGSDI_USIM_OPLMNSEL,       /**< Operator-controlled Public Land Mobile
                                   Networks selector. */
  MMGSDI_USIM_HPLMNWACT,      /**< Home PLMN selector With Access Technology. */
  MMGSDI_USIM_ARR,            /**< Access Rule Reference. */
  MMGSDI_USIM_RPLMNACT,       /**< Registered PLMN last used Access Technology. */
  MMGSDI_USIM_NETPAR,         /**< Network parameters. */
  MMGSDI_USIM_PNN,            /**< PLMN Network Name. */
  MMGSDI_USIM_OPL,            /**< Operator PLMN List. */
  MMGSDI_USIM_MBDN,           /**< Mailbox Dialing Number. */

  /* (0x4)3C => 60 */
  MMGSDI_USIM_EXT6,           /**< Extension 6. */
  MMGSDI_USIM_MBI,            /**< Mailbox Identifier. */
  MMGSDI_USIM_MWIS,           /**< Message Waiting Indication Status. */
  MMGSDI_USIM_SPDI,           /**< Service Provider Display Information. */
  MMGSDI_USIM_EHPLMN,         /**< Equivalent Home PLMN. */
  MMGSDI_USIM_CFIS,           /**< Call Forwarding Indicator Status. */
  MMGSDI_USIM_VGCSCA,         /**< Voice Group Call Service Ciphering Algorithm. */
  MMGSDI_USIM_VBSCA,          /**< Voice Broadcast Service Ciphering Algorithm. */
  MMGSDI_USIM_GBABP,          /**< MBMS Generic Bootstrapping Architecture
                                   Bootstrapping Parameters. */
  MMGSDI_USIM_GBANL,          /**< MBMS GBA Network Application Function
                                   derivation List associated with the Network
                                   Application Function derivation process. */

  /* (0x4)46 => 70 */
  MMGSDI_USIM_MSK,            /**< MBMS Service Key. */
  MMGSDI_USIM_MUK,            /**< MBMS User Key. */

  /* CPHS */
  MMGSDI_USIM_VMWI,           /**< CPHS Voice Mail Waiting Indicator. */
  MMGSDI_USIM_SVC_STR_TBL,    /**< CHPS Service string table. */
  MMGSDI_USIM_CFF,            /**< CHPS Call Forwarding Flag. */
  MMGSDI_USIM_ONS,            /**< CHPS Operator Name String. */
  MMGSDI_USIM_CSP,            /**< CHPS Customer Service Profile. */
  MMGSDI_USIM_CPHSI,          /**< CHPS Information. */
  MMGSDI_USIM_MN,             /**< CHPS Mailbox Number. */
  MMGSDI_USIM_MMSN,           /**< Multimedia Messaging Service */

/* EFs for MMS Notification */
  /* (0x4)50 => 80 */
  MMGSDI_USIM_MMSICP,             /**< Multimedia Messaging Service Issuer
                                       Connectivity Parameters. */

  MMGSDI_USIM_MMSUP,              /**< Multimedia Messaging Service User
                                       Preferences. */
  MMGSDI_USIM_MMSUCP,             /**< Multimedia Messaging Service User
                                       Connectivity Parameters. */
  MMGSDI_USIM_EXT8,               /**< Extension 8. */
  MMGSDI_USIM_EPSLOCI,            /**< EPS Location Information. */
  MMGSDI_USIM_EPSNSC,             /**< EPS NAS Security Context. */
  MMGSDI_USIM_SPNI,               /**< Service Provider Name Icon. */
  MMGSDI_USIM_PNNI,               /**< PLMN Network Name Icon. */
  MMGSDI_USIM_NCP_IP,             /**< Connectivity parameters for USIM IP
                                       connections. */

  MMGSDI_USIM_NASCONFIG,          /**< NAS configuration parameter information */

  /* (0x4)5A => 90 */
  MMGSDI_USIM_EHPLMNPI,           /**< Equivalent HPLMN Presentation Indicator */
  MMGSDI_USIM_LRPLMNSI,           /**< Last RPLMN Selection Indication */
  MMGSDI_USIM_ONS_SHORT,          /**< CHPS Operator Name Short String. */
  MMGSDI_USIM_PWS,                /**< Public Warning System Configuration */

  /* EFs for PKCS #15 support under USIM ADF */
  MMGSDI_PKCS15_ODF,              /**< Object Directory File. */
  MMGSDI_PKCS15_TI,               /**< Token Information file. */
  MMGSDI_PKCS15_US,               /**< Unused Space information file. */

  /*EFs at DF PHONEBOOK under USIM ADF */
  /* (0x41)00 => 0 */
  MMGSDI_USIM_PBR       = (MMGSDI_USIM_PHONEBOOK_DF_START << 8),
                              /**< Phone Book Reference file. */
  MMGSDI_USIM_PSC,            /**< Phone book Synchronization Center. */
  MMGSDI_USIM_CC,             /**< Change Counter. */
  MMGSDI_USIM_PUID,           /**< Previous Unique Identifier. */

  /*EFs at DF GSM under USIM ADF */
  /* (0x42)00 => 0 */
  MMGSDI_USIM_KC        = (MMGSDI_USIM_GSM_ACCESS_DF_START << 8),
                              /**< GSM Key Cipher. */
  MMGSDI_USIM_KCGPRS,         /**< Key Cipher for GPRS. */
  MMGSDI_USIM_CPBCCH,         /**< Compact Packet Broadcast Control Channel
                                   information. */
  MMGSDI_USIM_INVSCAN,        /**< Investigation scan. */

  /*EFs at DF MEXe under USIM ADF */
  /* (0x43)00 => 0 */
  MMGSDI_USIM_MEXE_ST   = (MMGSDI_USIM_MEXE_DF_START << 8),
                              /**< Mobile Equipment Execution Environment Service
                                   Table. */
  MMGSDI_USIM_ORPK,           /**< Operator Root Public Key. */
  MMGSDI_USIM_ARPK,           /**< Administrator Root Public Key. */
  MMGSDI_USIM_TPRPK,          /**< Third Party Root Public Key. */

  /* I-WLAN EFs*/
  /* (0x44)00 => 0 */
  MMGSDI_USIM_PSEUDO    = (MMGSDI_USIM_WLAN_DF_START << 8),
  MMGSDI_USIM_UPLMNWLAN,      /**< User-controlled PLMN selector for I-WLAN
                                   access. */
  MMGSDI_USIM_0PLMNWLAN,      /**< Operator-controlled PLMN selector for I-WLAN
                                   access. */
  MMGSDI_USIM_UWSIDL,         /**< User-controlled WLAN-Specific Identifier
                                   List. */
  MMGSDI_USIM_OWSIDL,         /**< Operator-controlled WLAN-Specific Identifier
                                   List. */
  MMGSDI_USIM_WRI,            /**< WLAN Reauthentication Identity. */
  MMGSDI_USIM_HWSIDL,         /**< Home I-WLAN-Specific Identifier List. */
  MMGSDI_USIM_WEHPLMNPI,      /**< I-WLAN Equivalent Home PLMN Presentation */
  MMGSDI_USIM_WHPI,           /**< I-WLAN Home PLMN Priority Indication. */
  MMGSDI_USIM_WLRPLMN,        /**< I-WLAN Last Registered PLMN. */
  MMGSDI_USIM_HPLMNDAI,       /**< Home PLMN Direct Access Indicator. */

  /* CSG EFs */
  /* (0x45)00 => 0 */
  MMGSDI_USIM_ACSGL     = (MMGSDI_USIM_HNB_DF_START << 8),
                              /**< Allowed Closed Subscriber Group Lists and
                                   corresponding indications. */
  MMGSDI_USIM_CSGT,           /**< Closed Subscriber Group Indication. */
  MMGSDI_USIM_HNBN,           /**< Home (e)NodeB Name. */
  MMGSDI_USIM_OCSGL,          /**< Operator CSG Lists and corresponding indications */
  MMGSDI_USIM_OCSGT,          /**< Operator CSG Type */
  MMGSDI_USIM_OHNBN,          /**< Operator Home (e)NodeB Name */

  /* EFs of DCS1800 DF */
  /* (0x5)00 => 0 */
  MMGSDI_DCS1800_IMSI   = (MMGSDI_DCS1800_EF_START << 8),
                              /**< International Mobile Subscriber Identity. */
  MMGSDI_DCS1800_KC,          /**< Key Cipher. */
  MMGSDI_DCS1800_PLMN,        /**< Public Land Mobile Network selector. */
  MMGSDI_DCS1800_SST,         /**< SIM Service Table. */
  MMGSDI_DCS1800_BCCH,        /**< Broadcast Control Channels. */
  MMGSDI_DCS1800_ACC,         /**< Access Control Class. */
  MMGSDI_DCS1800_FPLMN,       /**< Forbidden PLMNs. */
  MMGSDI_DCS1800_LOCI,        /**< Location Information. */
  MMGSDI_DCS1800_AD,          /**< Administrative Data. */

  /* EFs of ISIM Application */
  /* (0x6)00 => 0 */
  MMGSDI_ISIM_IMPI           = (MMGSDI_ISIM_EF_START << 8),
                              /**< IP Multimedia Private user Identity. */
  MMGSDI_ISIM_DOMAIN,         /**< Home network Domain name. */
  MMGSDI_ISIM_IMPU,           /**< IP Multimedia Public User identity. */
  MMGSDI_ISIM_ARR,            /**< Access Rule Reference. */
  MMGSDI_ISIM_KEYS,           /**< Ciphering and integrity keys for IMS. */
  MMGSDI_ISIM_AD,             /**< Administrative Data. */
  MMGSDI_ISIM_GBABP,          /**< GBA Bootstrapping parameters. */

  /* EFs for Orange */
  /* (0x7)00 => 0 */
  MMGSDI_SIM_7F40_PROP1_DFS  = (MMGSDI_ORANGE_SIM_EF_START << 8),
                                /**< Orange&tm;&nbsp;proprietary: Dynamic Flags
                                     Status. */
  MMGSDI_SIM_7F40_PROP1_D2FS,   /**< Orange proprietary: Dynamic2 Flag Setting. */
  MMGSDI_SIM_7F40_PROP1_CSP2,   /**< Orange proprietary: Customer Service
                                     Profile line 2. */
  MMGSDI_SIM_7F40_PROP1_PARAMS, /**< Orange proprietary: EF parameters, Welcome
                                     EFs of the ORANGE DF under the USIM ADF. */

  /* (0x71)00 => 0 */
  MMGSDI_USIM_7F40_PROP1_DFS = (MMGSDI_ORANGE_USIM_EF_START << 8),
                                /**< Orange proprietary: Dynamic Flags Status. */
  MMGSDI_USIM_7F40_PROP1_D2FS,  /**< Orange proprietary: Dynamic2 Flag Setting. */
  MMGSDI_USIM_7F40_PROP1_CSP2,  /**< Orange proprietary: Customer Service
                                    Profile line 2. */
  MMGSDI_USIM_7F40_PROP1_PARAMS,    /**< Orange proprietary: EF parameters,
                                         Welcome message. */


  /* EF under Cingular */
  MMGSDI_SIM_7F66_PROP1_SPT_TABLE  = (MMGSDI_CINGULAR_SIM_EF_START << 8),
                                    /**< Cingular proprietary: Support table. */

  /* (0x81)00 => 0 */
  MMGSDI_USIM_7F66_PROP1_SPT_TABLE = (MMGSDI_CINGULAR_USIM_EF_START << 8),
                                  /**< Cingular proprietary: Cingular TST. */

  /* (0x82)00 => 0 */
  MMGSDI_SIM_7F66_PROP1_ACT_HPLMN  = (MMGSDI_CINGULAR_SIM_NETWORK_DF_START << 8),
                                  /**< Cingular proprietary: Acting
                                         Home PLMN. */

  /* (0x83)00 => 0 */
  MMGSDI_SIM_7F66_PROP1_RPM_EF     = (MMGSDI_CINGULAR_SIM_RPM_DF_START << 8),
                                  /**< Proprietary: RPM Enabled Flag */
  MMGSDI_SIM_7F66_PROP1_RPM_PARAM,/**< Proprietary: RPM Parameters */
  MMGSDI_SIM_7F66_PROP1_RPM_CNTR_LR,
                                  /**< Proprietary: RPM Leak Rate Counter */
  MMGSDI_SIM_7F66_PROP1_RPM_CNTRS,/**< Proprietary: RPM Counters */
  MMGSDI_SIM_7F66_PROP1_RPM_VI,   /**< Proprietary: RPM Version Information */

  /* (0x84)00 => 0 */
  MMGSDI_USIM_7F66_PROP1_ACT_HPLMN  = (MMGSDI_CINGULAR_USIM_NETWORK_DF_START << 8),
                                  /**< Cingular proprietary: Acting Home PLMN. */
  MMGSDI_USIM_7F66_PROP1_RAT,       /**< Cingular proprietary: Radio Access
                                         Technology. */

  /* (0x85)00 => 0 */
  MMGSDI_USIM_7F66_PROP1_RPM_EF     = (MMGSDI_CINGULAR_USIM_RPM_DF_START << 8),
                                  /**< Proprietary: RPM Enabled Flag */
  MMGSDI_USIM_7F66_PROP1_RPM_PARAM,
                                  /**< Proprietary: RPM Parameters */
  MMGSDI_USIM_7F66_PROP1_RPM_CNTR_LR,
                                  /**< Proprietary: RPM Leak Rate Counter */
  MMGSDI_USIM_7F66_PROP1_RPM_CNTRS,
                                  /**< Proprietary: RPM Counters */
  MMGSDI_USIM_7F66_PROP1_RPM_VI,  /**< Proprietary: RPM Version Information */

  /* EFs of Home Zone */
  /* (0x9)00 => 0 */
  MMGSDI_SIM_7F43_PROP1_HZ        = (MMGSDI_HZI_EF_START << 8),
                                    /**< Home Zone. */
  MMGSDI_SIM_7F43_PROP1_CACHE1,     /**< USIM Home Zone Information (UHZI)
                                         proprietary: Cache 1. */
  MMGSDI_SIM_7F43_PROP1_CACHE2,     /**< UHZI proprietary: Cache 2. */
  MMGSDI_SIM_7F43_PROP1_CACHE3,     /**< UHZI proprietary: Cache 3. */
  MMGSDI_SIM_7F43_PROP1_CACHE4,              /**< UHZI proprietary: Cache 4. */
  MMGSDI_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_1, /**< UHZI proprietary: LAC_CI_1. */
  MMGSDI_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_2, /**< UHZI proprietary: LAC_CI_2. */
  MMGSDI_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_3, /**< UHZI proprietary: LAC_CI_3. */
  MMGSDI_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_4, /**< UHZI proprietary: LAC_CI_4. */
  MMGSDI_SIM_7F43_PROP1_TAGS,                /**< UHZI proprietary: Tags. */
  MMGSDI_SIM_7F43_PROP1_SETTINGS,            /**< UHZI proprietary: Settings. */

  /* EFs in CSIM ADF*/
  /* (0xA)00 => 0 */
  MMGSDI_CSIM_ARR                = (MMGSDI_CSIM_EF_START << 8),
  MMGSDI_CSIM_CALL_COUNT,     /**< Call count. */
  MMGSDI_CSIM_IMSI_M,         /**< MIN-based International Mobile Subscriber
                                   Identity. */
  MMGSDI_CSIM_IMSI_T,         /**< True International Mobile Subscriber
                                   Identity.*/
  MMGSDI_CSIM_TMSI,           /**< Temporary Mobile Subscriber Identity. */
  MMGSDI_CSIM_ANALOG_HOME_SID,          /**< Analog home SID. */
  MMGSDI_CSIM_ANALOG_OP_PARAMS,         /**< Analog Operational parameters. */
  MMGSDI_CSIM_ANALOG_LOCN_AND_REGN_IND, /**< Analog location and region
                                             indicators. */
  MMGSDI_CSIM_CDMA_HOME_SID_NID,        /**< CDMA home SID and NID. */
  MMGSDI_CSIM_CDMA_ZONE_BASED_REGN_IND, /**< CDMA zone-based region indicators. */

  /* (0xA)0A => 10 */
  MMGSDI_CSIM_CDMA_SYS_REGN_IND,        /**< CDMA system/network region
                                             indicators. */
  MMGSDI_CSIM_CDMA_DIST_BASED_REGN_IND, /**< CDMA distance-based region
                                             indicators. */
  MMGSDI_CSIM_ACCOLC,                   /**< Access Overload Class. */

  MMGSDI_CSIM_CALL_TERM_MODE_PREF,      /**< Call termination mode preferences. */
  MMGSDI_CSIM_SSCI,                     /**< Suggested Slot Cycle Index. */
  MMGSDI_CSIM_ANALOG_CHAN_PREF,     /**< Analog channel preferences. */
  MMGSDI_CSIM_PRL,                  /**< Preferred Roaming List */
  MMGSDI_CSIM_RUIM_ID,              /**< Removable UIM ID. */
  MMGSDI_CSIM_CST,                  /**< CSIM Service Table. */
  MMGSDI_CSIM_SPC,                  /**< Service Programming Code. */

  /* (0xA)14 => 20 */
  MMGSDI_CSIM_OTAPA_SPC_ENABLE,     /**< Over-The-Air Parameter Administration
                                         Service Programming Code enable. */
  MMGSDI_CSIM_NAM_LOCK,             /**< Number Assignment Module lock. */
  MMGSDI_CSIM_OTASP_OTAPA_FEATURES, /**< Over-The-Air Service Provisioning/
                                         Over-The-Air Parameter Administration
                                         features. */
  MMGSDI_CSIM_SERVICE_PREF,    /**< Service preferences. */
  MMGSDI_CSIM_ESN_ME,          /**< ESN for the ME. */
  MMGSDI_CSIM_UNASSIGNED_1,    /**< Reserved 1. */
  MMGSDI_CSIM_LANG_IND,        /**< Language indication. */
  MMGSDI_CSIM_FDN,             /**< Fixed Dialing Number. */
  MMGSDI_CSIM_SMS,             /**< Short Message Service. */
  MMGSDI_CSIM_SMS_PARAMS,      /**< Short Message Service Parameters. */

  /* (0xA)1E => 30 */
  MMGSDI_CSIM_SMS_STATUS,      /**< Short Message Service Status. */
  MMGSDI_CSIM_SSFC,            /**< Supplementary Services Feature Code table. */
  MMGSDI_CSIM_SPN,             /**< CDMA home Service Provider Name. */
  MMGSDI_CSIM_USGIND,          /**< Usage Indicator for the UIM ID/Short Form
                                    EUIMID. */
  MMGSDI_CSIM_AD,              /**< CDMA Administrative Data. */
  MMGSDI_CSIM_MSISDN,          /**< Mobile Station International Subscriber
                                    Directory Number. */
  MMGSDI_CSIM_MAXIMUM_PRL,     /**< Maximum size of the PRL. */
  MMGSDI_CSIM_SPC_STATUS,      /**< Service Programming Code status. */
  MMGSDI_CSIM_ECC,             /**< Emergency Call Codes. */
  MMGSDI_CSIM_3GPD_ME3GPDOPC,  /**< 3GPD Operational Capabilities. */

  /* (0xA)28 => 40 */
  MMGSDI_CSIM_3GPD_3GPDOPM,    /**< 3GPD Operational Mode. */
  MMGSDI_CSIM_3GPD_SIPCAP,     /**< Simple IP Capabilities. */
  MMGSDI_CSIM_3GPD_MIPCAP,     /**< Mobile IP Capabilities. */
  MMGSDI_CSIM_3GPD_SIPUPP,     /**< Simple IP User Profile Parameters. */
  MMGSDI_CSIM_3GPD_MIPUPP,     /**< Mobile IP User Profile Parameters. */
  MMGSDI_CSIM_3GPD_SIPSP,      /**< Simple IP Status Parameters. */
  MMGSDI_CSIM_3GPD_MIPSP,      /**< Mobile IP Status Parameters. */
  MMGSDI_CSIM_3GPD_SIPPAPSS,   /**< Simple IP Password Authentication Protocol
                                    Shared Secret parameters. */
  MMGSDI_CSIM_UNASSIGNED_2,    /**< Reserved 2. */
  MMGSDI_CSIM_UNASSIGNED_3,    /**< Reserved 3. */

  /* (0xA)32 => 50 */
  MMGSDI_CSIM_PUZL,            /**< Preferred User Zone List. */
  MMGSDI_CSIM_MAXPUZL,         /**< Maximum Preferred User Zone List. */
  MMGSDI_CSIM_MECRP,           /**< ME-specific Configuration Request Parameters. */
  MMGSDI_CSIM_HRPDCAP,         /**< HRPD access authorization Capability
                                    Parameters. */
  MMGSDI_CSIM_HRPDUPP,         /**< HRPD access authorization User Profile
                                    Parameters. */
  MMGSDI_CSIM_CSSPR,           /**< Current System Selection Preferred Roaming
                                    protocol revision. */
  MMGSDI_CSIM_ATC,             /**< Access Terminal Class. */
  MMGSDI_CSIM_EPRL,            /**< Extended Preferred Roaming List. */
  MMGSDI_CSIM_BCSMS_CONFIG,    /**< Broadcast SMS configuration. */
  MMGSDI_CSIM_BCSMS_PREF,      /**< Broadcast SMS preferences. */

  /* (0xA)3C => 60 */
  MMGSDI_CSIM_BCSMS_TABLE,     /**< Broadcast SMS table. */
  MMGSDI_CSIM_BCSMS_PARAMS,    /**< Broadcast SMS parameters. */
  MMGSDI_CSIM_BAKPARA,         /**< Currently used BAK parameters. */
  MMGSDI_CSIM_UPBAKPARA,       /**< Updated BAK parameters. */
  MMGSDI_CSIM_MMSN,            /**< Multimedia Messaging Service Notification. */
  MMGSDI_CSIM_MMS_EXT8,        /**< Multimedia Messaging Service Extension 8. */
  MMGSDI_CSIM_MMS_ICP,         /**< Multimedia Messaging Service Issuer
                                    Connectivity Parameters. */
  MMGSDI_CSIM_MMS_UP,          /**< Multimedia Messaging Service User
                                    Preferences. */
  MMGSDI_CSIM_MMS_UCP,         /**< Multimedia Messaging Service User
                                    Connectivity Parameters. */
  MMGSDI_CSIM_AUTH_CAP,        /**< Authentication capability. */

  /* (0xA)46 => 70 */
  MMGSDI_CSIM_3GCIK,           /**< 3G Cipher and Integrity Keys. */
  MMGSDI_CSIM_DCK,             /**< Depersonalization Control Key. */
  MMGSDI_CSIM_GID1,            /**< Japanese CDMA Group Identifier 1. */
  MMGSDI_CSIM_GID2,            /**< JCDMA Group Identifier 2. */
  MMGSDI_CSIM_CNL,             /**< CDMA Cooperative Network List. */
  MMGSDI_CSIM_HOME_TAG,        /**< Home system tag. */
  MMGSDI_CSIM_GROUP_TAG,       /**< Group tag list. */
  MMGSDI_CSIM_SPECIFIC_TAG,    /**< Specific tag list. */
  MMGSDI_CSIM_CALL_PROMPT,     /**< Call prompt list. */
  MMGSDI_CSIM_SF_EUIM_ID,      /**< Short Form EUIMID. */

  /* (0xA)50 => 80 */
  MMGSDI_CSIM_EST,             /**< Enabled Service Table. */
  MMGSDI_CSIM_HIDDEN_KEY,      /**< Key for hidden phone book entry. */
  MMGSDI_CSIM_LCS_VERSION,     /**< Location Services protocol Version. */
  MMGSDI_CSIM_LCS_CP,          /**< Location Services Connectivity Parameter. */
  MMGSDI_CSIM_SDN,             /**< Service Dialing Numbers. */
  MMGSDI_CSIM_EXT2,            /**< Extension 2. */
  MMGSDI_CSIM_EXT3,            /**< Extension 3. */
  MMGSDI_CSIM_ICI,             /**< Incoming Call Information. */
  MMGSDI_CSIM_OCI,             /**< Outgoing Call Information. */
  MMGSDI_CSIM_EXT5,            /**< Extension 5. */

  /* (0xA)5A => 90 */
  MMGSDI_CSIM_CCP2,            /**< Capability Configuration Parameters 2. */
  MMGSDI_CSIM_APP_LABELS,      /**< Application labels. */
  MMGSDI_CSIM_DEV_MODEL,       /**< Device model information. */
  MMGSDI_CSIM_ROOT_CERT,       /**< Root certificate. */
  MMGSDI_CSIM_SMS_CAP,         /**< Short Message Service Capabilities. */
  MMGSDI_CSIM_3GPD_MIPFLAGS,   /**< 3GPD Mobile IP flags. */
  MMGSDI_CSIM_3GPD_SIPUPPEXT,  /**< 3GPDSimple IP User Profile Parameters extension. */
  MMGSDI_CSIM_3GPD_MIPUPPEXT,  /**< 3GPDMobile IP User Profile Parameters extension. */
  MMGSDI_CSIM_3GPD_IPV6CAP,    /**< 3GPD IPv6 capabilities. */
  MMGSDI_CSIM_3GPD_TCPCONFIG,  /**< 3GPD TCP configuration. */

  /* (0xA)64 => 100 */
  MMGSDI_CSIM_3GPD_DGC,        /**< 3GPD Data Generic Configuration. */
  MMGSDI_CSIM_BROWSER_CP,      /**< Browser Connectivity Parameters. */
  MMGSDI_CSIM_BROWSER_BM,      /**< Browser Boookmarks. */
  MMGSDI_CSIM_MMS_CONFIG,      /**< Multimedia Messaging Service configuration. */
  MMGSDI_CSIM_JDL,             /**< Java<sup>&amp;reg;</sub> Download URL. */
  MMGSDI_CSIM_LBS_XCONFIG,     /**< Location Based Services XTRA configuration. */
  MMGSDI_CSIM_LBS_XSURL,       /**< Location Based Services XTRA Server URLs. */
  MMGSDI_CSIM_LBS_V2CONFIG,    /**< Location Based Services V2 configuration. */
  MMGSDI_CSIM_LBS_V2PDEADDR,   /**< Location Based Services V2 Position
                                    Determination Entity address. */
  MMGSDI_CSIM_LBS_V2MPCADDR,   /**< Location Based Services V2 Mobile
                                    Positioning Center address. */

  /* (0xA)6e => 110 */
  MMGSDI_CSIM_BREW_DLOAD,      /**< BREW Download. */
  MMGSDI_CSIM_BREW_TSID,       /**< BREW Teleservice ID. */
  MMGSDI_CSIM_BREW_SID,        /**< BREW Subscriber ID. */
  MMGSDI_CSIM_BREW_AEP,        /**< BREW Application Execution Policy. */
  MMGSDI_CSIM_3GPD_UPPEXT,     /**< 3GPD User Profile Parameters extension. */

  /*EFs at DF PHONEBOOK under CSIM ADF */
  /* (0xA1)00 => 0 */
  MMGSDI_CSIM_PBR      = (MMGSDI_CSIM_PHONEBOOK_DF_START << 8),
                                  /**< Phone Book Reference file. */
  MMGSDI_CSIM_PSC,                /**< Phone book Synchronization Center. */
  MMGSDI_CSIM_CHANGE_COUNTER,     /**< Change Counter. */
  MMGSDI_CSIM_PUID,               /**< Previous Unique Identifier. */
  MMGSDI_CSIM_EXT_ME_SETTING_DATA,  /**< Japanese CDMA extended ME setting data. */
  MMGSDI_CSIM_USER_AGENT_STRING,    /**< JCDMA user agent string. */

  /*EFs at DF JCDMA under CSIM ADF */
  /* (0xA2)00 => 0 */
  MMGSDI_CSIM_IMSI_STATUS= (MMGSDI_CSIM_JCDMA_DF_START << 8),
                                    /**< JCDMA International Mobile Subscriber
                                         Identity status. */
  MMGSDI_CSIM_ME_DOWNLOADABLE_DATA, /**< JCDMA ME downloadable data. */
  MMGSDI_CSIM_ME_SETTING_DATA,      /**< JCDMA ME setting data. */
  MMGSDI_CSIM_ME_USER_DATA,         /**< JCDMA ME user data. */
  MMGSDI_CSIM_UIM_SVC_TABLE,        /**< JCDMA UIM service table. */
  MMGSDI_CSIM_UIM_FEATURES,         /**< JCDMA UIM features. */
  MMGSDI_CSIM_UIM_VERSION,          /**< JCDMA signature and version. */

  MMGSDI_BY_PATH_ENUM    = (MMGSDI_EF_PATH_START << 8),
    /* EFs with no matching path, will remove later, keeping for compilation*/
  /* EFs for PKCS #15 support */
  MMGSDI_PKCS15_PRKDF,
                              /**< Private Key Directory File. */
  MMGSDI_PKCS15_PUKDF,        /**< Public Key Directory File. */
  MMGSDI_PKCS15_SKDF,         /**< Secret Key Directory File. */
  MMGSDI_PKCS15_CDF,          /**< Certificate Directory File. */
  MMGSDI_PKCS15_DODF,         /**< Data Object Directory File. */
  MMGSDI_PKCS15_AODF,         /**< Authentication Object Directory File. */

  MMGSDI_NO_FILE_ENUM    = (MMGSDI_NO_FILE_ENUM_START << 8),
                                  /**< Last element of the file. Also used to indicate if
                             there is no file enumeration.\n
                             @note1 This must be placed just before MAX_FILE_ENUM. */

  MMGSDI_MAX_FILE_ENUM   = 0xFFFFFFFF /**< Forces an initialize size enumeration. */
} mmgsdi_file_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_DF_ENUM_TYPE
============================================================================*/
/** Used to access the SIM/USIM/RUIM directories.
*/
typedef enum {
  MMGSDI_MF_DIR           = 0x3F00,     /**< Master File directory. */
  MMGSDI_GSM_DIR          = 0x7F20,     /**< GSM directory. */
  MMGSDI_TELECOM_DIR      = 0x7F10,     /**< Telecom directory. */
  MMGSDI_LAST_SEL_ADF_DIR = 0x7FFF,     /**< Last selected Application Dedicated
                                             File directory. */
  MMGSDI_DCS1800_DIR      = 0x7F21,     /**< Digital Cellular System 1800 MHz
                                             directory. */
  MMGSDI_RUIM_DIR         = 0x7F25,     /**< RUIM directory. */
  MMGSDI_MAX_DF_ENUM      = 0x7FFFFFFF  /**< Forces the enumeration to be an
                                             int32 type. */
}mmgsdi_df_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_ACCESS_METHOD_ENUM_TYPE
=============================================================================*/
/** Method of accessing a card's file system.
*/
typedef enum {
  MMGSDI_EF_ENUM_ACCESS,    /**< Uses the EF enumeration provided in the
                                 the MMGSDI_FILE_ENUM_TYPE request. */
  MMGSDI_DF_ENUM_ACCESS,    /**< Uses the DF enumeration provided in the
                                 the MMGSDI_DIR_FILE_ENUM_TYPE request. */
  MMGSDI_BY_PATH_ACCESS,    /**< Uses the path provided in the request. */
  MMGSDI_BY_APP_ID_ACCESS,  /**< Uses the application ID. */
  MMGSDI_MAX_ACCESS_METHOD_ENUM = 0x7FFFFFFF /**< Forces an initialize size
                                                  enumeration. */
}mmgsdi_access_method_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_PIN_ENUM_TYPE
=============================================================================*/
/** Card holder verification values or PINs for a SIM/USIM/RUIM. Includes
  key references for UICC applications
*/
typedef enum {
  MMGSDI_PIN1         = 0x00, /**< Either GSM/RUIM PIN1 or USIM APP1 PIN1. */
  MMGSDI_PIN2,           /**< Either GSM/RUIM PIN2 or USIM APP1 PIN2. */
  MMGSDI_UNIVERSAL_PIN,  /**< Universal PIN used by a UICC card. This type of
                              PIN is used in a multi-application environment to
                              allow several applications to share one common PIN. */
  MMGSDI_ADM1,           /**< Administrative PIN1. */
  MMGSDI_ADM2,           /**< Administrative PIN2. */
  MMGSDI_ADM3,           /**< Administrative PIN3. */
  MMGSDI_ADM4,           /**< Administrative PIN4. */
  MMGSDI_ADM5,           /**< Administrative PIN5. */
  MMGSDI_ADM6,           /**< Administrative PIN6. */
  MMGSDI_ADM7,           /**< Administrative PIN7. */
  MMGSDI_ADM8,           /**< Administrative PIN8. */
  MMGSDI_ADM9,           /**< Administrative PIN9. */
  MMGSDI_ADM10,          /**< Administrative PIN10. */
  MMGSDI_ADM11,          /**< Administrative PIN11. */
  MMGSDI_ADM12,          /**< Administrative PIN12. */
  MMGSDI_ADM13,          /**< Administrative PIN13. */
  MMGSDI_ADM14,          /**< Administrative PIN14. */
  MMGSDI_ADM15,          /**< Administrative PIN15. */
  MMGSDI_HIDDENKEY,      /**< EF Hidden Key used for hiding UICC phone book
                              entries. */
  MMGSDI_KEY_REF_PIN_APPL_1,     /**< Key Reference for PIN Appliction 1 */
  MMGSDI_KEY_REF_PIN_APPL_2,     /**< Key Reference for PIN Appliction 2 */
  MMGSDI_KEY_REF_PIN_APPL_3,     /**< Key Reference for PIN Appliction 3 */
  MMGSDI_KEY_REF_PIN_APPL_4,     /**< Key Reference for PIN Appliction 4 */
  MMGSDI_KEY_REF_PIN_APPL_5,     /**< Key Reference for PIN Appliction 5 */
  MMGSDI_KEY_REF_PIN_APPL_6,     /**< Key Reference for PIN Appliction 6 */
  MMGSDI_KEY_REF_PIN_APPL_7,     /**< Key Reference for PIN Appliction 7 */
  MMGSDI_KEY_REF_PIN_APPL_8,     /**< Key Reference for PIN Appliction 8 */
  MMGSDI_KEY_REF_SEC_PIN_APPL_1, /**< Key Reference for Second PIN Appliction 1 */
  MMGSDI_KEY_REF_SEC_PIN_APPL_2, /**< Key Reference for Second PIN Appliction 2 */
  MMGSDI_KEY_REF_SEC_PIN_APPL_3, /**< Key Reference for Second PIN Appliction 3 */
  MMGSDI_KEY_REF_SEC_PIN_APPL_4, /**< Key Reference for Second PIN Appliction 4 */
  MMGSDI_KEY_REF_SEC_PIN_APPL_5, /**< Key Reference for Second PIN Appliction 5 */
  MMGSDI_KEY_REF_SEC_PIN_APPL_6, /**< Key Reference for Second PIN Appliction 6 */
  MMGSDI_KEY_REF_SEC_PIN_APPL_7, /**< Key Reference for Second PIN Appliction 7 */
  MMGSDI_KEY_REF_SEC_PIN_APPL_8, /**< Key Reference for Second PIN Appliction 8 */
  MMGSDI_MAX_PIN_ENUM = 0x7F /**< Forces an initialize size enumeration. */
}mmgsdi_pin_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_SLOT_ID_ENUM_TYPE
=============================================================================*/
/** Slot ID, which tells the MMGSDI task and drivers with which slot to
    communicate.
*/
typedef enum {
  MMGSDI_SLOT_1          = 0x00000001, /**< Directs the request to the
                                            SIM/USIM/RUIM inserted in Slot 1. */

  MMGSDI_SLOT_2,                       /**< Directs the request to the
                                            SIM/USIM/RUIM inserted in Slot 2. */

  MMGSDI_SLOT_3,                       /**< Directs the request to the
                                            SIM/USIM/RUIM inserted in Slot 3. */

  MMGSDI_SLOT_AUTOMATIC,
    /**< Directs the request to the slot from which the relevant subscription
         is being obtained. For example, if the ME is acquiring GSM with the
         information from Slot 1 and there are two GSM SIMs in the ME, the
         request is directed to Slot 1. */

  MMGSDI_MAX_SLOT_ID_ENUM = 0x7FFFFFFF /**< Forces an initialize size
                                            enumeration. */
}mmgsdi_slot_id_enum_type;

/** @} */ /* end_addtogroup common_library */
/** @addtogroup common_library
@{ */

/*===========================================================================
   STRUCTURE:      MMGSDI_APP_ENUM_TYPE
=============================================================================*/
/** Type of card containing the application to which the client is talking.
*/
typedef enum {
 MMGSDI_APP_NONE,      /**< &nbsp; */
 MMGSDI_APP_SIM,       /**< &nbsp; */
 MMGSDI_APP_RUIM,      /**< &nbsp; */
 MMGSDI_APP_USIM,      /**< &nbsp; */
 MMGSDI_APP_CSIM,      /**< &nbsp; */
 MMGSDI_APP_UNKNOWN    /**< &nbsp; */
}mmgsdi_app_enum_type;

/** @} */ /* end_addtogroup common_library */
/** @addtogroup common_library
@{ */

/*=============================================================================
   ENUM:      MMGSDI_FILE_STRUCTURE_ENUM_TYPE
=============================================================================*/
/** Type of file.
*/
typedef enum {
  MMGSDI_LINEAR_FIXED_FILE       = 0x00000000, /**< Indexed using record
                                                    numbers; an offset is not
                                                    allowed. */
  MMGSDI_CYCLIC_FILE,        /**< Indexed using record numbers; an offset is
                                  not allowed. Some files allow for an increase. */
  MMGSDI_TRANSPARENT_FILE,   /**< A flat file where an offset is allowed.
                                  Indexing by records is not allowed. */
  MMGSDI_MASTER_FILE,        /**< Indication of a master Dedicated File,
                                  which allows for a functional grouping of
                                  files. It can be the parent of DFs or EFs. */
  MMGSDI_DEDICATED_FILE,     /**< DFs are referenced by file identifiers. */
  MMGSDI_MAX_FILE_STRUCTURE_ENUM = 0x7FFFFFFF /**< Forces an initialize size
                                                   enumeration. */
}mmgsdi_file_structure_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_FILE_SECURITY_ENUM_TYPE
=============================================================================*/
/** File security access method.
*/
typedef enum {
  MMGSDI_ALWAYS_ALLOWED         = 0x00, /**< Access is always allowed. */
  MMGSDI_NEVER_ALLOWED,                       /**< Access is never allowed. */
  MMGSDI_AND_ALLOWED,                         /**< Access is allowed when all
                                                   the PINs have been verified.*/
  MMGSDI_OR_ALLOWED,                          /**< Access is allowed when one of
                                                   the PINs has been verified.*/
  //NEED TO HANDLE THIS CASE IN mmgsdi_uicc_rsp_get_file_attr
  MMGSDI_SINGLE_ALLOWED,                      /**< A single PIN is required to
                                                   be verified for access. */
  MMGSDI_MAX_FILE_SECURITY_ENUM = 0x7F  /**< Forces an initialize size
                                                   enumeration. */
}mmgsdi_file_security_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_REFRESH_MODE_ENUM_TYPE
=============================================================================*/
/** Available refresh modes. See ETSI TS 101 267 (3GPP TS 11.14) and
    ETSI TS 131 111 (3GPP TS 31.111).
*/
typedef enum {
  MMGSDI_REFRESH_RESET,             /**< Resets the card. */
  MMGSDI_REFRESH_NAA_INIT,          /**< Initializes the USIM. */
  MMGSDI_REFRESH_NAA_INIT_FCN,      /**< Initializes the USIM and notifies the
                                         ME of EF file changes in the USIM. */
  MMGSDI_REFRESH_NAA_FCN,           /**< Notifies the ME of EF file changes in
                                         the USIM. */
  MMGSDI_REFRESH_NAA_INIT_FULL_FCN, /**< Initializes the USIM and performs full
                                         File Change Notification. */
  MMGSDI_REFRESH_NAA_APP_RESET,     /**< Resets the USIM application. */
  MMGSDI_REFRESH_3G_SESSION_RESET,  /**< Resets the 3G session. */
  MMGSDI_REFRESH_STEERING_OF_ROAMING, /**< Initiates the Steering of Roaming
                                           procedure, which forces the user
                                           equipment to camp on the
                                           operator-preferred PLMN. */
  MMGSDI_REFRESH_RESET_AUTO,        /**< Resets automatically. */
  MMGSDI_REFRESH_APP_RESET_AUTO,    /**< Resets automatically when an
                                         application requires complete
                                         initialization procedures to be
                                         performed. */
  MMGSDI_REFRESH_MODE_ENUM_MAX = 0x7FFFFFFF /**< Forces an initialize size
                                                 enumeration. */
}mmgsdi_refresh_mode_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_REFRESH_ORIG_ENUM_TYPE
=============================================================================*/
/** Original refresh request type.
*/
typedef enum {
  MMGSDI_REFRESH_ORIG_REFRESH_REQ,      /**< Refresh command from the card via
                                             GSTK. */
  MMGSDI_REFRESH_ORIG_PUP_REQ,          /**< Card Power Up request. */
  MMGSDI_REFRESH_ORIG_REG_REQ,          /**< Registration for refresh. */
  MMGSDI_REFRESH_ORIG_DEREG_REQ,        /**< Deregistration from refresh. */
  MMGSDI_REFRESH_ORIG_OK_TO_REFRESH_REQ,/**< Request to initialize or perform a
                                             file change notification. */
  MMGSDI_REFRESH_ORIG_COMPLETE_REQ,     /**< Refresh complete request. */
  MMGSDI_REFRESH_ORIG_AUTO_REFRESH_REQ, /**< Refresh command triggered by the
                                             MMGSDI internally. */
  MMGSDI_REFRESH_ORIG_TIMER_EXPIRY_REQ, /**< Refresh timer expiry request. */
  MMGSDI_REFRESH_ORIG_PROCEED_WITH_REFRESH_REQ,/**< Request to proceed with refresh. */
  MMGSDI_REFRESH_ORIG_REG_ALL_REQ,      /**< Registration for refresh all files. */
  MMGSDI_REFRESH_ORIG_DEREG_ALL_REQ,    /**< Deregistration from refresh all files. */
  MMGSDI_REFRESH_ORIG_ENUM_MAX = 0x7FFFFFFF /**< Forces an initialize size
                                                 enumeration. */
}mmgsdi_refresh_orig_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_REFRESH_STAGE_ENUM_TYPE
=============================================================================*/
/** Possible stage for each refresh event.
*/
typedef enum { MMGSDI_REFRESH_STAGE_START    = 0x00000000, /**< Starting refresh. */
  MMGSDI_REFRESH_STAGE_IN_PROGRESS_APP_DEACTIVATED, /**< Application is being
                                                         deactivated. */
  MMGSDI_REFRESH_STAGE_IN_PROGRESS_APP_ACTIVATED,   /**< Application is being
                                                         activated. */
  MMGSDI_REFRESH_STAGE_END_SUCCESS,         /**< Refresh completed successfully. */
  MMGSDI_REFRESH_STAGE_END_FAILED,          /**< Refresh failed. */
  MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT, /**< Waiting for an acceptable time
                                                 to initialize the card. */
  MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN,  /**< Waiting for an acceptable time
                                                 to receive the File Change
                                                 Notification. */
  MMGSDI_REFRESH_STAGE_WAIT_FOR_PROCEED_WITH_REFRESH,  /**< Waiting for approval from client to
                                                  proceed with refresh */
  MMGSDI_REFRESH_STAGE_ENUM_MAX = 0x7FFFFFFF /**< Forces an initialize size
                                                  enumeration. */
}mmgsdi_refresh_stage_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_REFRESH_START_APP_ENUM_TYPE
=============================================================================*/
/** Indicates if the REFRESH_START event is for provisioning applications or
    non-provisioning applications.
*/
typedef enum {
  MMGSDI_REFRESH_START_PROV_APP,         /**< Provisioning applications. */
  MMGSDI_REFRESH_START_NON_PROV_APP,     /**< Non-provisioning applications. */
  MMGSDI_REFRESH_START_ENUM_MAX = 0x7FFFFFFF /**< Forces an initialize size
                                                  enumeration. */
}mmgsdi_refresh_start_app_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_REFRESH_SOURCE_ENUM_TYPE
=============================================================================*/
/** Indicates source of the refresh.
*/
typedef enum {
  MMGSDI_REFRESH_SOURCE_SIM,            /**< Refresh command initiated by SIM
                                            card */
  MMGSDI_REFRESH_SOURCE_USER,           /**< Refresh initiated due to action by
                                             user */
  MMGSDI_REFRESH_SOURCE_MMGSDI,         /**< Internal fake refresh originated
                                             by MMGSDI */
  MMGSDI_REFRESH_SOURCE_BUILTIN_PLMN,   /**< Refresh initiated after storing/
                                             deleting builtin PLMN list in EFS*/
  MMGSDI_REFRESH_SOURCE_ECALL           /**< Refresh command initiated by SIM
                                             card after the profile is switched
                                             to EMERGENCY*/
}mmgsdi_refresh_source_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_FDN_EVT_SOURCE_ENUM_TYPE
=============================================================================*/
/** Indicates source of the FDN event.
*/
typedef enum {
  MMGSDI_FDN_SOURCE_CARD_INIT, /**< In case of card initialization */
  MMGSDI_FDN_SOURCE_REFRESH,   /**< In case of refresh */
  MMGSDI_FDN_SOURCE_USER,      /**< In case of write_req or enable/disable
                                    service the source is user */
}mmgsdi_fdn_evt_source_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_PIN_STATUS_ENUM_TYPE
=============================================================================*/
/** PIN status.
*/
typedef enum {
  MMGSDI_PIN_STATUS_NOT_INITIALIZED, /**< Status of PIN checking is not
                                          initialized. */
  MMGSDI_PIN_ENABLED_NOT_VERIFIED,   /**< PIN checking is enabled but the
                                          PIN has not been verified. */
  MMGSDI_PIN_ENABLED_VERIFIED,       /**< PIN checking is enabled and the PIN
                                          has been verified. */
  MMGSDI_PIN_DISABLED,               /**< PIN checking has been disabled. */
  MMGSDI_PIN_BLOCKED,                /**< After three consecutive false PIN
                                          presentations, the PIN has been
                                          blocked. */
  MMGSDI_PIN_PERM_BLOCKED,           /**< PIN has been permanently blocked. */
  MMGSDI_PIN_UNBLOCKED,              /**< PIN is enabled again after having been
                                          blocked. */
  MMGSDI_PIN_CHANGED,                /**< PIN has been changed. */
  MMGSDI_PIN_NOT_FOUND,              /**< PIN is not found. */
  MMGSDI_PIN_RETRY_COUNT_DECREASED   /**< PIN retry count decreased. */
}mmgsdi_pin_status_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_SAP_MODE_ENUM_TYPE
=============================================================================*/
/** Service Access Point mode.
*/
typedef enum {
  MMGSDIBT_SERVER,                        /**< &nbsp; */
  MMGSDIBT_CLIENT,                        /**< &nbsp; */
  MMGSDIBT_MAX_SAP_MODE_ENUM = 0x7FFFFFFF /**< Forces an initialize size
                                               enumeration. */
}mmgsdi_sap_mode_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_DISCONNECT_MODE_ENUM_TYPE
=============================================================================*/
/** SAP Disconnect mode.
*/
typedef enum {
  MMGSDIBT_DISCONNECT_IMMED,                     /**< Immediate disconnect. */
  MMGSDIBT_DISCONNECT_GRACE,                     /**< Graceful shutdown of
                                                      the connection. */
  MMGSDIBT_MAX_DISCONNECT_MODE_ENUM = 0x7FFFFFFF /**< Forces an initialize
                                                      size enumeration. */
}mmgsdi_disconnect_mode_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_STATUS_ME_APP_ENUM_TYPE
=============================================================================*/
/** Indicates to the card what the mobile device is going to do with regard to
    the status command sent for the indicated application ID.

    The response is applicable only to a UICC. For a 2G card (ICC), the status
    command is ignored.
*/
typedef enum {
  MMGSDI_STATUS_APP_NONE          = 0x00000000, /**< For GSM SIM cards (ICC).
                                                     No application. */
  MMGSDI_STATUS_APP_NO_INDICATION, /**< For UICC cards. Reports that the mobile
                                        device has not initialized or terminated
                                        the application. */
  MMGSDI_STATUS_APP_INITIALIZED,   /**< For UICC cards. Reports that the mobile
                                        device has initialized the application
                                        in the phone's image. */
  MMGSDI_STATUS_APP_TERMINATED,    /**< For UICC cards. Reports that the mobile
                                        device has terminated the application in
                                        the phone's image. */
  MMGSDI_STATUS_APP_MAX           = 0x7FFFFFFF  /**< Forces an initialize size
                                                     enumeration. */
}mmgsdi_status_me_app_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_STATUS_RET_DATA_ENUM_TYPE
=============================================================================*/
/** Indicates to the card the structure of the data the mobile equipment is
    expecting.

    This enumeration is applicable to a UICC only. It is ignored if it is
    provided for a 2G card (ICC).
*/
typedef enum {
  MMGSDI_STATUS_NONE        = 0x00000000,  /**< For GSM ICC cards only. No
                                                status. */
  MMGSDI_STATUS_DATA_SELECT,   /**< For UICC cards. Response parameters and
                                    data are identical to the response parameters
                                    and data of the SELECT command. */
  MMGSDI_STATUS_TLV_DATA,      /**< For UICC cards. The DFNAME Tag Length Value
                                    object of the currently selected application
                                    is returned. */
  MMGSDI_STATUS_NO_DATA,       /**< No data is returned. */
  MMGSDI_STATUS_MAX         = 0x7FFFFFFF   /**< Forces an initialize size
                                                enumeration. */
}mmgsdi_status_ret_data_enum_type;

/*=============================================================================
   ENUM:      MMGSDI_PERSO_ACTION_TYPE
=============================================================================*/
/** Specifies the personalization action.
*/
typedef enum {
  MMGSDI_PERSO_ACTIVATE               = 0x00000000,
                                     /**< Activates personalization. */
  MMGSDI_PERSO_DEACTIVATE,           /**< Deactivates personalization. */
  MMGSDI_PERSO_UNBLOCK,              /**< Unblocks personalization. */
  MMGSDI_PERSO_DISABLE,              /**< Disables personalization. */
  MMGSDI_PERSO_OTA_DEPERSO,          /**< Performs over-the-air
                                          depersonalization. */
  MMGSDI_PERSO_GET_FEATURE_IND,      /**< Gets personalization feature
                                          indicators. */
  MMGSDI_PERSO_GET_FEATURE_DATA,     /**< Gets personalization feature data. */
  MMGSDI_PERSO_SET_FEATURE_DATA,     /**< Sets personalization feature data. */
  MMGSDI_PERSO_GET_PERM_DISABLE_IND, /**< Gets permanently disabled feature
                                          indicators. */
  MMGSDI_PERSO_GET_DCK_RETRIES,      /**< Gets Depersonalization Control Key
                                          retries. */
  MMGSDI_PERSO_GET_UNBLOCK_DCK_RETRIES, /**< Gets unblocked DCK retries. */
  MMGSDI_PERSO_GET_FEATURE_KEY,      /**< Gets the personalization feature key. */
  MMGSDI_PERSO_LOCK_DOWN,            /**< Locks down personalization. */
  MMGSDI_MAX_PERSO_ACTION_ENUM        = 0x7FFFFFFF /**< Forces an initialize
                                                        size enumeration. */
}mmgsdi_perso_action_enum_type;

/*===========================================================================
  ENUM:      MMGSDI_PERSO_FEATURE_ENUM_TYPE
===========================================================================*/
/** Identifiers used for each personalization feature.
*/
typedef enum {
  MMGSDI_PERSO_NW    = 0,  /**< Network. */
  MMGSDI_PERSO_NS,         /**< Network Subset. */
  MMGSDI_PERSO_SP,         /**< Service Provider. */
  MMGSDI_PERSO_CP,         /**< Corporate. */
  MMGSDI_PERSO_SPN,        /**< SPN. */
  MMGSDI_PERSO_SP_EHPLMN,  /**< SP + EHPLMN */
  MMGSDI_PERSO_ICCID,      /**< ICCID */
  MMGSDI_PERSO_SIM,        /**< SIM/USIM. */

  MMGSDI_PERSO_RUIM_NW1,   /**< Network 1 for RUIM. */
  MMGSDI_PERSO_RUIM_NW2,   /**< Network 2 for RUIM. */
  MMGSDI_PERSO_RUIM_HRPD,  /**< HRPD for RUIM. */
  MMGSDI_PERSO_RUIM_SP,    /**< Service Provider for RUIM. */
  MMGSDI_PERSO_RUIM_CP,    /**< Corporate for RUIM. */
  MMGSDI_PERSO_RUIM_RUIM,  /**< RUIM. */

  MMGSDI_MAX_PERSO_FEATURE_ENUM  = 0x7FFFFFFF /**< Forces an initialize size
                                                   enumeration. */
} mmgsdi_perso_feature_enum_type;

/*===========================================================================
  ENUM:      MMGSDI_PERSO_LOCK_DOWN_ENUM_TYPE
===========================================================================*/
/** Identifiers used for each personalization feature.
*/
typedef enum {
  MMGSDI_PERSO_LOCK_DOWN_GET_FEATURE_KEY = 0x00000000, /**< Personalization
                                                            feature key. */
  MMGSDI_PERSO_LOCK_DOWN_MAX             = 0x7FFFFFFF  /**< Forces an initialize
                                                            size enumeration. */
}mmgsdi_perso_lock_down_enum_type;

/*===========================================================================
  ENUM:      MMGSDI_PERSO_GET_KEY_ENUM_TYPE
===========================================================================*/
/** Identifiers used to get information for all keys or for unblock keys only.
*/
typedef enum {
  MMGSDI_PERSO_GET_KEY            = 0x00000000, /**< Information for all keys. */
  MMGSDI_PERSO_GET_UNBLOCK_KEY,                 /**< Information for unblock
                                                     keys only. */
  MMGSDI_MAX_PERSO_GET_KEY_ENUM   = 0x7FFFFFFF  /**< Forces an initialize size
                                                     enumeration. */
} mmgsdi_perso_get_key_enum_type;

/*===========================================================================
  ENUM:      MMGSDI_PERSO_FEATURE_IND_ENUM_TYPE
===========================================================================*/
/** Identifiers used to get information for permanently disabled
    personalization indicators or for all personalization indicators.
*/
typedef enum {
  MMGSDI_PERSO_ALL_FEATURE_IND      = 0x00000000, /**< All personalization
                                                       indicators. */
  MMGSDI_PERSO_DISABLE_FEATURE_IND,               /**< Permanently disabled
                                                       personalization
                                                       indicators. */
  MMGSDI_MAX_PERSO_FEATURE_IND_ENUM = 0x7FFFFFFF  /**< Forces an initialize
                                                       size enumeration. */
} mmgsdi_perso_feature_ind_enum_type;

/*===========================================================================
  ENUM:      MMGSDI_PERSO_DCK_RETRIES_ENUM_TYPE
===========================================================================*/
/** Identifiers used to get or unblock Depersonalization Control Key retries.
*/
typedef enum {
  MMGSDI_PERSO_DCK_RETRIES            = 0x00000000, /**< Get DCK retries. */
  MMGSDI_PERSO_UNBLOCK_DCK_RETRIES,                 /**< Unblock DCK retries. */
  MMGSDI_MAX_PERSO_DCK_RETRIES_ENUM   = 0x7FFFFFFF  /**< Forces an initialize
                                                         size enumeration. */
} mmgsdi_perso_dck_retries_enum_type;

/*===========================================================================
  ENUM:      MMGSDI_PERSO_STATUS_ENUM_TYPE
===========================================================================*/
/** Identifiers used for the status of personalization features.
*/
typedef enum {
  MMGSDI_PERSO_STATUS_NONE,          /**< Currently not personalized. */
  MMGSDI_PERSO_STATUS_DONE,          /**< Personalized. */
  MMGSDI_PERSO_STATUS_LOCKED,        /**< ME is locked. */
  MMGSDI_PERSO_STATUS_UNLOCKED,      /**< ME is unlocked to allow
                                          personalization. */
  MMGSDI_PERSO_STATUS_BLOCKED,       /**< ME has been blocked as a result of
                                          excessive failed attempts at
                                          depersonalization. */
  MMGSDI_PERSO_STATUS_UNBLOCKED,     /**< ME has been unblocked to allow
                                          depersonalization. */
  MMGSDI_PERSO_STATUS_SANITY_ERROR,  /**< Entered and stored control key values
                                          differ. */
  MMGSDI_PERSO_STATUS_WAIT_FOR_EXT_PERSO,    /**< Wait for external personalization
                                                  request.*/
  MMGSDI_PERSO_STATUS_GENERIC_FAILURE,       /**< Personalization is failed. */
  MMGSDI_PERSO_STATUS_IN_PROGRESS,           /**< Personalization check is in
                                                  progress due to sub or slot dependency */
  MMGSDI_MAX_PERSO_STATUS_ENUM  = 0x7FFFFFFF /**< Forces an initialize size
                                                  enumeration. */
}mmgsdi_perso_status_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_CARD_ERR_INFO_ENUM_TYPE
=============================================================================*/
/** Specific card error condition encountered by the client.
*/
typedef enum {
  MMGSDI_CARD_ERR_UNKNOWN_ERROR                = 0x00000000, /**< &nbsp; */
  MMGSDI_CARD_ERR_POLL_ERROR,                  /**< &nbsp; */
  MMGSDI_CARD_ERR_NO_ATR_RCVD_AT_MAX_VOLT,     /**< ATR was not received at the
                                                    maximum voltage. */
  MMGSDI_CARD_ERR_NO_ATR_RCVD_AFTER_RESET,     /**< ATR was not received after
                                                    a reset. */
  MMGSDI_CARD_ERR_CORRUPT_ATR_RCVD_MAX_TIMES,  /**< Corrupted ATR was received
                                                    after the maximum times. */
  MMGSDI_CARD_ERR_PPS_TIMED_OUT_MAX_TIMES,     /**< Protocol and Parameter
                                                    Select timed out after the
                                                    maximum times. */
  MMGSDI_CARD_ERR_VOLT_MISMATCH,               /**< Voltage mismatch. */
  MMGSDI_CARD_ERR_INT_CMD_TIMED_OUT_AFTER_PPS, /**< Internal command timed out
                                                    after a Protocol and
                                                    Parameter Select. */
  MMGSDI_CARD_ERR_INT_CMD_ERR_EXCEED_MAX_NUMS, /**< Maximum number of internal
                                                    command errors was reached. */
  MMGSDI_CARD_ERR_MAXED_PARITY_ERROR,          /**< Maximized due to a
                                                    parity error. */
  MMGSDI_CARD_ERR_MAXED_RX_BREAK_ERR,          /**< Maximized due to a break
                                                    in a reception error. */
  MMGSDI_CARD_ERR_MAXED_OVERRUN_ERR,           /**< Maximized due to an
                                                    overrun error. */
  MMGSDI_CARD_ERR_TRANS_TIMER_EXP,             /**< Transmission timer expired. */
  MMGSDI_CARD_ERR_PWR_DN_CMD_NOTIFY,           /**< Power-down command
                                                    notification error. */
  MMGSDI_CARD_ERR_INT_ERR_IN_PASSIVE_MODE,     /**< Internal error in Passive
                                                    mode. */
  MMGSDI_CARD_ERR_TIMED_OUT_IN_PASSIVE_MODE,   /**< Timed out in Passive mode. */
  MMGSDI_CARD_ERR_MAX_PARITY_IN_PASSIVE,       /**< Maximum parity error in
                                                    Passive mode. */
  MMGSDI_CARD_ERR_MAX_RXBRK_IN_PASSIVE,        /**< Maximum reception break in
                                                    Passive mode. */
  MMGSDI_CARD_ERR_MAX_OVERRUN_IN_PASSIVE,      /**< Maximum overrun errors in
                                                    Passive mode. */
  MMGSDI_CARD_ERR_NO_PROTOCOL_AT_CARD_PUP,     /**< No protocol at card power
                                                    up. */
  MMGSDI_CARD_ERR_CARD_REMOVED,                /**< Card Removed */
  MMGSDI_CARD_ERR_TECHNICAL_PROBLEM,           /**< Technical problem */
  MMGSDI_CARD_ERR_PWR_DN_PERSO_ERR,            /**< Power-down due to perso
                                                    error */
  MMGSDI_CARD_ERR_NULL_BYTES,                  /**< Card responded with NULL
                                                    bytes */
  MMGSDI_CARD_ERR_SAP_CONNECTED,               /**< Card unavailable as SAP is
                                                    connected */
  MMGSDI_CARD_ERR_MAX                               = 0x7FFFFFFF
                                               /**< Forces an initialize size
                                                    enumeration. */
}mmgsdi_card_err_info_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_CARD_PDOWN_OPTIONS_TYPE
=============================================================================*/
/** Type of power down required.
*/
typedef enum {
  MMGSDI_CARD_POWER_DOWN_ONLY,            /**< Performs a power down only. */
  MMGSDI_CARD_POWER_DOWN_NOTIFY_GSDI,     /**< Performs a power down and notifies
                                               the Generic SIM Driver Interface. */
  MMGSDI_CARD_PDOWN_MAX_ENUM = 0x7FFFFFFF /**< Forces an initialize size
                                               enumeration. */
}mmgsdi_card_pdown_options_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_CARD_PUP_OPTIONS_ENUM_TYPE
=============================================================================*/
/** Type of power up required.
*/
typedef enum {
  MMGSDI_CARD_POWER_UP_NONE,                /**< No power up is required. */
  MMGSDI_CARD_POWER_UP_SWITCH_UIM_PASSIVE,  /**< Performs a power up and switches
                                                 the UIM to Passive mode. */
  MMGSDI_CARD_POWER_UP_INITIAL_PUP,         /**< Performs an initial power up. */
  MMGSDI_CARD_PUP_MAX_ENUM = 0x7FFFFFFF     /**< Forces an initialize size
                                                 enumeration. */
}mmgsdi_card_pup_options_enum_type;

/*===========================================================================
   ENUM:  MMGSDI_PIN_OPERATION_ENUM_TYPE
=============================================================================*/
/** Used to differentiate between PIN operations.

@note1hang There is no MAX enumeration because this enumeration must be one
           byte only.
*/
typedef enum {
  MMGSDI_PIN_OP_NONE    = 0x00,      /**< &nbsp; */
  MMGSDI_PIN_OP_VERIFY,              /**< &nbsp; */
  MMGSDI_PIN_OP_ENABLE,              /**< &nbsp; */
  MMGSDI_PIN_OP_DISABLE,             /**< &nbsp; */
  MMGSDI_PIN_OP_CHANGE,              /**< &nbsp; */
  MMGSDI_PIN_OP_UNBLOCK,             /**< &nbsp; */
  MMGSDI_PIN_OP_DISABLE_AND_REPLACE  /**< &nbsp; */
}mmgsdi_pin_operation_enum_type;

/*===========================================================================
   ENUM: MMGSDI_PIN_REPLACE_ENUM_TYPE
=============================================================================*/
/** Indicates whether a PIN is replaced by a Universal PIN.

    If the PIN is replaced by a Universal PIN, the client must send a
    verification command for the Universal PIN to obtain access to the EFs that
    are protected by that PIN.
*/
typedef enum {
  MMGSDI_PIN_NOT_REPLACED_BY_UNIVERSAL, /**< &nbsp; */
  MMGSDI_PIN_REPLACED_BY_UNIVERSAL      /**< &nbsp; */
}mmgsdi_pin_replace_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_SEND_APDU_OPTIONS_TYPE
=============================================================================*/
/** Type of Send APDU response required.
*/
typedef enum {
  MMGSDI_SEND_APDU_RETURN_IMPLICIT_STATUS_WORDS, /**< When implicit status words
                                                      (SW1/SW2 of 0x61/0xXX or
                                                       0x9F/0xXX) is received
                                                       from card, return the
                                                       same */
  MMGSDI_SEND_APDU_NO_IMPLICIT_STATUS_WORDS,     /**< Do not return implicit
                                                      status words, only final
                                                      data */
  MMGSDI_SEND_APDU_MAX_ENUM = 0x7FFFFFFF         /**< Forces an initialize size
                                                      enumeration. */
}mmgsdi_send_apdu_options_enum_type;

/*===========================================================================
   ENUM: MMGSDI_SERVICE_ENUM_TYPE
=============================================================================*/
/** Types of services available in CST/UST/SST.
*/
typedef enum {
  MMGSDI_NONE              = 0x00000000, /**< No service. */
  MMGSDI_GET_SRV_FROM_APP_TYPE,          /**< Get service name from app type */

  /* 100 */
  MMGSDI_CDMA_SRV_CHV1_DISABLE     = MMGSDI_CDMA_SRV, /**< CHV1 disabled
                                                           function. */
  MMGSDI_CDMA_SRV_LOCAL_PHONEBOOK,    /**< Local phone book. */
  MMGSDI_CDMA_SRV_FDN,                /**< Forbidden Dialing Number. */
  MMGSDI_CDMA_SRV_SMS,                /**< Short Message Service. */
  MMGSDI_CDMA_SRV_HRPD,               /**< High Rate Packet Data. */
  MMGSDI_CDMA_SRV_ENHANCED_PHONEBOOK, /**< Enhanced phone book. */
  MMGSDI_CDMA_SRV_MMD,                /**< MultiMedia Domain. */
  MMGSDI_CDMA_SRV_SF_EUIMID,          /**< Short Form Expanded User Identity
                                           Module Identifier. */
  MMGSDI_CDMA_SRV_MEID,               /**< Mobile Equipment Identifier. */
  MMGSDI_CDMA_SRV_EXT1,               /**< Extension 1. */

  /* 110 */
  MMGSDI_CDMA_SRV_EXT2,               /**< Extension 2. */
  MMGSDI_CDMA_SRV_SMSP,               /**< Short Message Service Parameter. */
  MMGSDI_CDMA_SRV_LND,                /**< Last Number Dialed. */
  MMGSDI_CDMA_SRV_SVR_CAT_PROG_BCSMS, /**< Service category program for
                                           Broadcast SMS. */
  MMGSDI_CDMA_SRV_3GPD_EXT,           /**< 3GPD and messaging extensions. */
  MMGSDI_CDMA_SRV_RFU8,               /**< Reserved for Future Use. */
  MMGSDI_CDMA_SRV_SPN,                /**< Service Provider Name. */
  MMGSDI_CDMA_SRV_SDN,                /**< Service Dialing Numbers. */
  MMGSDI_CDMA_SRV_EXT3,               /**< Extension 3. */
  MMGSDI_CDMA_SRV_3GPD_SIP,           /**< 3GPD Simple IP. */

  /* 120 */
  MMGSDI_CDMA_SRV_BROWSER,            /**< Browser configuration parameters. */
  MMGSDI_CDMA_SRV_JAVA,               /**< JAVA configuration parameters. */
  MMGSDI_CDMA_SRV_BREW,               /**< BREW configuration parameters. */
  MMGSDI_CDMA_SRV_LBS,                /**< Location-Based Services. */
  MMGSDI_CDMA_SRV_DATA_DL_SMSCB,      /**< Data Download Short Message Service
                                           Cell Broadcast. */
  MMGSDI_CDMA_SRV_DATA_DL_SMSPP,      /**< Data Download Short Message Service
                                           Point-to-Point. */
  MMGSDI_CDMA_SRV_MENU_SEL,           /**< Menu selection. */
  MMGSDI_CDMA_SRV_CALL_CONTROL,       /**< Call Control. */
  MMGSDI_CDMA_SRV_PROACTIVE,          /**< Proactive RUIM. */
  MMGSDI_CDMA_SRV_AKA,                /**< 3G Authorization and Key Agreement. */

  /* 130 */
  MMGSDI_CDMA_SRV_IPV6,               /**< Internet Protocol V6. */
  MMGSDI_CDMA_SRV_RFU15,              /**< Reserved for Future Use. */
  MMGSDI_CDMA_SRV_RFU16,              /**< Reserved for Future Use. */
  MMGSDI_CDMA_SRV_RFU17,              /**< Reserved for Future Use. */
  MMGSDI_CDMA_SRV_RFU18,              /**< Reserved for Future Use. */
  MMGSDI_CDMA_SRV_RFU19,              /**< Reserved for Future Use. */
  MMGSDI_CDMA_SRV_RFU20,              /**< Reserved for Future Use. */
  MMGSDI_CDMA_SRV_3GPD_MIP,           /**< 3GPD Mobile IP. */
  MMGSDI_CDMA_SRV_BCMCS,              /**< BroadCast MultiCast Service. */
  MMGSDI_CDMA_SRV_MMS,                /**< Multimedia Messaging Service. */

  /* 140 */
  MMGSDI_CDMA_SRV_EXT8,               /**< Extension 8. */
  MMGSDI_CDMA_SRV_MMS_USER_PARAMS,    /**< Multimedia Messaging Service user
                                           connectivity parameters. */
  MMGSDI_CDMA_SRV_APPL_AUTH,          /**< Application authentication. */
  MMGSDI_CDMA_SRV_GID1,               /**< Group Identifier 1. */
  MMGSDI_CDMA_SRV_GID2,               /**< Group Identifier 2. */
  MMGSDI_CDMA_SRV_DEPERSO_CNTL_KEYS,  /**< Depersonalization control keys. */
  MMGSDI_CDMA_SRV_COOP_NET_LIST,      /**< Cooperative network list. */

  /* 200 */
  MMGSDI_GSM_SRV_CHV1_DISABLE     = MMGSDI_GSM_SRV, /**< CHV1 disabled function. */
  MMGSDI_GSM_SRV_ADN,                 /**< GSM Abbreviated Dialing Numbers. */
  MMGSDI_GSM_SRV_FDN,                 /**< Forbidden Dialing Numbers. */
  MMGSDI_GSM_SRV_SMS,                 /**< Short Message Service. */
  MMGSDI_GSM_SRV_AOC,                 /**< Advice of Charge. */
  MMGSDI_GSM_SRV_CCP,                 /**< Configuration Capability Parameters. */
  MMGSDI_GSM_SRV_PLMN,                /**< Public Land Mobile Network selector. */
  MMGSDI_GSM_SRV_RFU1,                /**< Reserved for Future Use. */
  MMGSDI_GSM_SRV_MSISDN,              /**< Mobile Station International
                                           Subscriber Directory Number. */
  MMGSDI_GSM_SRV_EXT1,                /**< Extension 1. */

  /* 210 */
  MMGSDI_GSM_SRV_EXT2,                /**< Extension 2. */
  MMGSDI_GSM_SRV_SMSP,                /**< Short Message Service Parameters. */
  MMGSDI_GSM_SRV_LND,                 /**< Last Number Dialed. */
  MMGSDI_GSM_SRV_CBMI,                /**< Cell Broadcast Message Identifier. */
  MMGSDI_GSM_SRV_GID1,                /**< Group Identifier 1. */
  MMGSDI_GSM_SRV_GID2,                /**< Group Identifier 2. */
  MMGSDI_GSM_SRV_SPN,                 /**< Service Provider Name. */
  MMGSDI_GSM_SRV_SDN,                 /**< Service Dialing Numbers. */
  MMGSDI_GSM_SRV_EXT3,                /**< Extension 3. */
  MMGSDI_GSM_SRV_RFU2,                /**< Reserved for Future Use. */

  /* 220 */
  MMGSDI_GSM_SRV_VGCS,                /**< Voice Group Call Service group
                                           identifier list. */
  MMGSDI_GSM_SRV_VBS,                 /**< Voice Broadcast Service group
                                           identifier list. */
  MMGSDI_GSM_SRV_ENH_ML_SVC,          /**< Enhanced Multi Level Precedence and
                                           Preemption service. */
  MMGSDI_GSM_SRV_AA_EMLPP,            /**< Automatic Answer for Enhanced
                                           Multi-Level Preemption and Priority. */
MMGSDI_GSM_SRV_DATA_DL_SMSCB,         /**< Data Download via Short Message
                                           Service Cell Broadcast. */
  MMGSDI_GSM_SRV_DATA_DL_SMSPP,       /**< Data Download via Short Message
                                           Service Point-to-Point. */
  MMGSDI_GSM_SRV_MENU_SEL,            /**< Menu selector. */
  MMGSDI_GSM_SRV_CALL_CONTROL,        /**< Call control. */
  MMGSDI_GSM_SRV_PROACTIVE,           /**< Proactive SIM. */
  MMGSDI_GSM_SRV_CBMIR,               /**< Cell Broadcast Message Ranges. */

  /* 230 */
  MMGSDI_GSM_SRV_BDN,                 /**< Barred Dialing Numbers. */
  MMGSDI_GSM_SRV_EXT4,                /**< Extension 4. */
  MMGSDI_GSM_SRV_DCK,                 /**< Depersonalization Control Key. */
  MMGSDI_GSM_SRV_CNL,                 /**< Cooperative Network List. */
  MMGSDI_GSM_SRV_SMSR,                /**< Short Message Service Status Reports. */
  MMGSDI_GSM_SRV_NIA,                 /**< Network's Indication of Alerting in
                                           the MS. */
  MMGSDI_GSM_SRV_MOSMS_CONTROL,       /**< Mobile Originated SMS control. */
  MMGSDI_GSM_SRV_GPRS,                /**< General Packet Radio Services. */
  MMGSDI_GSM_SRV_IMAGE,               /**< Image storage. */
  MMGSDI_GSM_SRV_SOLSA,               /**< Support Of Local Service Area. */

  /* 240 */
  MMGSDI_GSM_SRV_USSD,                /**< Unstructured Supplementary Service
                                           Data string data object supported in
                                           call control. */
  MMGSDI_GSM_SRV_RUN_AT_CMD,          /**< Run at Command command. */
  MMGSDI_GSM_SRV_UPLMN_SEL_WACT,      /**< User-controlled Public Land Mobile
                                           Networks selector With Access
                                           Technology. */
  MMGSDI_GSM_SRV_OPLMN_SEL_WACT,      /**< Operator-controlled PLMN selector
                                           With Access Technology. */
  MMGSDI_GSM_SRV_HPLMN_WACT,          /**< Home PLMN selector With Access
                                           Technology. */
  MMGSDI_GSM_SRV_CPBCCH,              /**< Compact Packet Broadcast Control
                                           Channel. */
  MMGSDI_GSM_SRV_INV_SCAN,            /**< Investigation Scan. */
  MMGSDI_GSM_SRV_ECCP,                /**< Extended Capability Configuration
                                           Parameter (in GSM). */
  MMGSDI_GSM_SRV_MEXE,                /**< Mobile Equipment Execution Environment
                                           information. */
  MMGSDI_GSM_SRV_RPLMN_LACT,          /**< Registered PLMN Last used Access
                                           Technology. */

  /* 250 */
  MMGSDI_GSM_SRV_PLMN_NTWRK_NAME,     /**< PLMN network name. */
  MMGSDI_GSM_SRV_OPLMN_LIST,          /**< Operator PLMN list. */
  MMGSDI_GSM_SRV_MDN,                 /**< Mailbox Dialing Numbers. */
  MMGSDI_GSM_SRV_MWI,                 /**< Message Wait Indication. */
  MMGSDI_GSM_SRV_CFI,                 /**< Call Forward Indication status. */
  MMGSDI_GSM_SRV_SPDI,                /**< Service Provider Display Information. */
  MMGSDI_GSM_SRV_MMS,                 /**< Multimedia Messaging Service. */
  MMGSDI_GSM_SRV_EXT8,                /**< Extension 8. */
  MMGSDI_GSM_SRV_MMS_USR_P,           /**< Multimedia Messaging Service User
                                           connectivity Parameters. */

  /* 400 */
  MMGSDI_USIM_SRV_LOCAL_PHONEBOOK  = MMGSDI_USIM_SRV, /**< Local phone book
                                                     (under ADF/phone book/ADN). */
  MMGSDI_USIM_SRV_FDN,                /**< Forbidden Dialing Numbers. */
  MMGSDI_USIM_SRV_EXT2,               /**< Extension 2. */
  MMGSDI_USIM_SRV_SDN,                /**< Service Dialing Numbers. */
  MMGSDI_USIM_SRV_EXT3,               /**< Extension 3. */
  MMGSDI_USIM_SRV_BDN,                /**< Barred Dialing Numbers. */
  MMGSDI_USIM_SRV_EXT4,               /**< Extension 4. */
  MMGSDI_USIM_SRV_OCI_OCT,            /**< Outgoing Call Information/Timer. */
  MMGSDI_USIM_SRV_ICI_ICT,            /**< Incoming Call Information/Timer. */
  MMGSDI_USIM_SRV_SMS,                /**< Short Message Service. */

  /* 40A OR 0x400 + 10 */
  MMGSDI_USIM_SRV_SMSR,               /**< Short Message Service Status Reports. */
  MMGSDI_USIM_SRV_SMSP,               /**< Short Message Service Parameters. */
  MMGSDI_USIM_SRV_AOC,                /**< Advice of Charge. */
  MMGSDI_USIM_SRV_CCP,                /**< Capability Configuration Parameters. */
  MMGSDI_USIM_SRV_CBMI,               /**< Cell Broadcast Message Identifier. */
  MMGSDI_USIM_SRV_CBMIR,              /**< Cell Broadcast Message Identifier
                                           Ranges. */
  MMGSDI_USIM_SRV_GID1,               /**< Group Identifier 1. */
  MMGSDI_USIM_SRV_GID2,               /**< Group Identifier 2. */
  MMGSDI_USIM_SRV_SPN,                /**< Service Provider Name. */
  MMGSDI_USIM_SRV_UPLMN_SEL_WACT,     /**< User-controlled Public Land Mobile
                                           Networks selector With Access
                                           Technology. */

  /* 0x414 OR 0x400+20 */
  MMGSDI_USIM_SRV_MSISDN,             /**< Mobile Station International
                                           Subscriber Directory Number. */
  MMGSDI_USIM_SRV_IMAGE,              /**< Image service. */
  MMGSDI_USIM_SRV_SOLSA,              /**< Support Of Local Service Area. */
  MMGSDI_USIM_SRV_ENH_ML_SVC,         /**< Enhanced Multi-Level precedence and
                                           preemption service. */
  MMGSDI_USIM_SRV_AA_EMLPP,           /**< Automatic Answer for Enhanced
                                           Multi-Level Preemption and Priority. */
  MMGSDI_USIM_SRV_RFU1,               /**< Reserved for Future Use. */
  MMGSDI_USIM_SRV_GSM_ACCESS,         /**< USIM ADF access to USIM files for
                                           GSM roaming. */
  MMGSDI_USIM_SRV_DATA_DL_SMSPP,      /**< Data download via Short Message
                                           Service Point-to-Point. */
  MMGSDI_USIM_SRV_DATA_DL_SMSCB,      /**< Data download via Short Message
                                           Service Cell Broadcast. */
  MMGSDI_USIM_SRV_CALL_CONTROL,       /**< Call control. */

  /* 0x41E OR 0x400+30 */
  MMGSDI_USIM_SRV_MOSMS_CONTROL,      /**< Mobile Originated SMS control. */
  MMGSDI_USIM_SRV_RUN_AT_CMD,         /**< Run AT command. */
  MMGSDI_USIM_SRV_SET_TO_1,           /**< Service shall be set to 1. */
  MMGSDI_USIM_SRV_EST,                /**< Enable Service Table in USIM. */
  MMGSDI_USIM_SRV_ACL,                /**< Access Control List. */
  MMGSDI_USIM_SRV_DCK,                /**< Depersonalization Control Key. */
  MMGSDI_USIM_SRV_CNL,                /**< Cooperative Network List. */
  MMGSDI_USIM_SRV_GSM_SECURITY_CONTEXT, /**< GSM security context. */
  MMGSDI_USIM_SRV_CPBCCH,             /**< Compact Packet Broadcast Control
                                           Channel. */
  MMGSDI_USIM_SRV_INV_SCAN,           /**< Investigation scan. */

  /* 0x428 OR 0x400+40 */
  MMGSDI_USIM_SRV_MEXE,               /**< Mobile Equipment Execution
                                           Environment information. */
  MMGSDI_USIM_SRV_OPLMN_SEL_WACT,     /**< Operator-controlled Public Land
                                           Mobile Networks selector With Access
                                           Technology. */
  MMGSDI_USIM_SRV_HPLMN_WACT,         /**< Home PLMN selector With Access
                                           Technology. */
  MMGSDI_USIM_SRV_EXT5,               /**< Extension 5. */
  MMGSDI_USIM_SRV_PLMN_NTWRK_NAME,    /**< PLMN network name. */
  MMGSDI_USIM_SRV_OPLMN_LIST,         /**< Operator PLMN list. */
  MMGSDI_USIM_SRV_MDN,                /**< Mailbox Dialing Numbers. */
  MMGSDI_USIM_SRV_MWI,                /**< Message Wait Indication. */
  MMGSDI_USIM_SRV_CFI,                /**< Call Forward Indication status. */
  MMGSDI_USIM_SRV_RFU2,               /**< Reserved for Future Use. */
  MMGSDI_USIM_SRV_RPLMN_LACT = MMGSDI_USIM_SRV_RFU2, /**< Registered PLMN LACT
                                      service 50 is replaced with Reserved for
                                      Future Use. */

  /* 0x432 OR 0x400+50 */
  MMGSDI_USIM_SRV_SPDI,               /**< Service Provider Display Information. */
  MMGSDI_USIM_SRV_MMS,                /**< Multimedia Messaging Service. */
  MMGSDI_USIM_SRV_EXT8,               /**< Extension 8.*/
  MMGSDI_USIM_SRV_GPRS_CC,            /**< Call Control on GPRS by USIM. */
  MMGSDI_USIM_SRV_MMS_USR_P,          /**< Multimedia Messaging Service User
                                           connectivity parameters. */
  MMGSDI_USIM_SRV_NIA,                /**< Network's Indication of Alerting. */
  MMGSDI_USIM_SRV_VGCS,               /**< Voice Group Call Service group
                                           identifier list. */
  MMGSDI_USIM_SRV_VBS,                /**< Voice Broadcast Service group
                                           identifier list. */
  MMGSDI_USIM_SRV_PSEUDONYM,          /**< Pseudonym service. */
  MMGSDI_USIM_SRV_UPLMN_WLAN_ACC,     /**< User-controlled PLMN selector for
                                           WLAN access. */

  /* 0x43C OR 0x400+60 */
  MMGSDI_USIM_SRV_OPLMN_WLAN_ACC,     /**< Operator-controlled PLMN selector for
                                           WLAN access. */
  MMGSDI_USIM_SRV_USER_CTRL_WSID,     /**< User-controlled WLAN Specific
                                           Identifier list. */
  MMGSDI_USIM_SRV_OPER_CTRL_WSID,     /**< Operator-controlled WLAN Specific
                                           Identifier list. */
  MMGSDI_USIM_SRV_VGCS_SECURITY,      /**< Voice Group Call Service security. */
  MMGSDI_USIM_SRV_VBS_SECURITY,       /**< Voice Broadcast Service security. */
  MMGSDI_USIM_SRV_WLAN_REAUTH_ID,     /**< WLAN Reauthentication Identity. */
  MMGSDI_USIM_SRV_MM_STORAGE,         /**< Multimedia Messaging Storage. */
  MMGSDI_USIM_SRV_GBA,                /**< Generic Bootstrapping Architecture */
  MMGSDI_USIM_SRV_MBMS_SECURITY,      /**< Multimedia Broadcast/Multicast
                                           Service security. */
  MMGSDI_USIM_SRV_DATA_DL_USSD,       /**< Data Download via Unstructured
                                           Supplementary Service Data and USSD
                                           application mode. */

  /* 0x446 OR 0x400+70 */
  MMGSDI_USIM_SRV_EHPLMN,             /**< Equivalent Home PLMN. */
  MMGSDI_USIM_SRV_TP_AFTER_UICC_ACT,  /**< Additional Terminal Profile after
                                           UICC activation. */
  MMGSDI_USIM_SRV_EHPLMNI,            /**< Equivalent Home PLMN presentation
                                           Indicator. */
  MMGSDI_USIM_LAST_RPMN_SEL_IND,      /**< Last Registered PLMN selection
                                           indication. */
  MMGSDI_USIM_SRV_OMA_BCAST,          /**< Open Mobile Aliance broadcast service
                                           profile. */
  MMGSDI_USIM_SRV_GBA_KEY_EST,        /**< GBA-based local key establishment
                                           mechanism. */
  MMGSDI_TERMINAL_APPS,               /**< Terminal applications. */
  MMGSDI_USIM_SRV_TERMINAL_APPS = MMGSDI_TERMINAL_APPS, /**< Renaming terminal
                                                             applications. */

  MMGSDI_USIM_SRV_SPNI,            /**< Service Provider Name Icon. */
  MMGSDI_USIM_SRV_PNNI,            /**< PLMN Network Name Icon. */

  /* 0x450 OR 0x400+80 */
  MMGSDI_USIM_SRV_NCP_IP,          /**< Connectivity Parameters for USIM IP
                                        connections. */
  MMGSDI_USIM_SRV_HWSIDL,          /**< Home I-WLAN Specific Identifier List. */
  MMGSDI_USIM_SRV_WEHPLMNPI,       /**< I-WLAN Equivalent HPLMN Presentation
                                        Indication. */
  MMGSDI_USIM_SRV_WHPI,            /**< I-WLAN HPLMN Priority Indication. */
  MMGSDI_USIM_SRV_WLRPLMN,         /**< I-WLAN Last Registered PLMN. */
  MMGSDI_USIM_SRV_EPS_MMI,         /**< EPS Mobility Management Information. */
  MMGSDI_USIM_SRV_ACSGL,           /**< Allowed CSG Lists and corresponding
                                        indications. */
  MMGSDI_USIM_SRV_EPS_PDN_CC,      /**< Call control on EPS PDN connection by
                                        USIM. */
  MMGSDI_USIM_SRV_HPLMN_DA,        /**< HPLMN Direct Access. */
  MMGSDI_USIM_SRV_ECALL,           /**< eCall Data */

  /* 0x45A OR 0x400+90 */
  MMGSDI_USIM_SRV_OCSGL,           /**< Operator CSG Lists and corresponding
                                        indications. */
  MMGSDI_USIM_SRV_SMIP,            /**< Support for SM-over-IP. */
  MMGSDI_USIM_SRV_CSG_DC,          /**< Support of CSG Display Control. */
  MMGSDI_USIM_SRV_IMS_CCTRL,       /**< Communication Control for IMS by USIM. */
  MMGSDI_USIM_SRV_ETA,             /**< Extended Terminal Applications. */
  MMGSDI_USIM_SRV_UICC_ACCESS_TO_IMS, /**< Support of UICC access to IMS. */
  MMGSDI_USIM_SRV_NAS_CONFIG,      /**< Non-Access Stratum configuration by USIM. */
  MMGSDI_USIM_SRV_PWS,             /**< Public Warning System configuration by USIM. */

  /* 0x800 OR 0x800+0 */
  MMGSDI_CSIM_SRV_LOCAL_PHONEBOOK  = MMGSDI_CSIM_SRV, /**< Local phone book:
                                                      Under ADF/Phone Book/ADN. */
  MMGSDI_CSIM_SRV_FDN,                /**< Fixed Dialing Number. */
  MMGSDI_CSIM_SRV_EXT2,               /**< Extension 2. */
  MMGSDI_CSIM_SRV_SDN,                /**< Service Dialing Number. */
  MMGSDI_CSIM_SRV_EXT3,               /**< Extension 3. */
  MMGSDI_CSIM_SRV_SMS,                /**< Short Message Service. */
  MMGSDI_CSIM_SRV_SMSP,               /**< Short Message Service Parameters. */
  MMGSDI_CSIM_SRV_HRPD,               /**< High Rate Packet Data. */
  MMGSDI_CSIM_SRV_BSVR_CAT_PROG_BCSMS, /**< Service category program for
                                            Broadcast SMS. */
  MMGSDI_CSIM_SRV_SPN,                /**< CDMA home Service Provider Name. */

  /* 0x80A OR 0x800+10 */
  MMGSDI_CSIM_SRV_DATA_DL_SMSCB,      /**< Data Download via Short Message
                                           Service Cell Broadcast. */
  MMGSDI_CSIM_SRV_DATA_DL_SMSPP,      /**< Data Download via Short Message
                                           Service Point-to-Point. */
  MMGSDI_CSIM_SRV_CALL_CONTROL,       /**< Call Control. */
  MMGSDI_CSIM_SRV_3GPD_SIP,           /**< 3GPD Simple IP. */
  MMGSDI_CSIM_SRV_3GPD_MIP,           /**< 3GPD Mobile IP. */
  MMGSDI_CSIM_SRV_AKA,                /**< Authentication and Key Agreement. */
  MMGSDI_CSIM_SRV_LCS_IP,             /**< IP-based Location Services. */
  MMGSDI_CSIM_SRV_BCMCS,              /**< BroadCast MultiCast Service. */
  MMGSDI_CSIM_SRV_MMS,                /**< Multimedia Messaging Service. */
  MMGSDI_CSIM_SRV_EXT8,               /**< Extension 8. */

  /* 0x814 OR 0x800+20 */
  MMGSDI_CSIM_SRV_MMS_USR_P,          /**< Multimedia Messaging Service user
                                           connectivity parameters. */
  MMGSDI_CSIM_SRV_APP_AUTH,           /**< Application authentication. */
  MMGSDI_CSIM_SRV_GID1,               /**< Group Identifier Level 1. */
  MMGSDI_CSIM_SRV_GID2,               /**< Group Identifier Level 2. */
  MMGSDI_CSIM_SRV_DCK,                /**< Depersonalization Control Key. */
  MMGSDI_CSIM_SRV_CNL,                /**< Cooperative Network List. */
  MMGSDI_CSIM_SRV_OCI,                /**< Outgoing Call Information. */
  MMGSDI_CSIM_SRV_ICI,                /**< Incoming Call Information. */
  MMGSDI_CSIM_SRV_EXT5,               /**< Extension 5. */
  MMGSDI_CSIM_SRV_MM_STORAGE,         /**< Multimedia storage. */

  /* 0x81E OR 0x800+30 */
  MMGSDI_CSIM_SRV_IMAGE,              /**< EF image. */
  MMGSDI_CSIM_SRV_EST,                /**< Enabled Services Table. */
  MMGSDI_CSIM_SRV_CCP,                /**< Capability Configuration Parameters. */
  MMGSDI_CSIM_SRV_SF_EUIMID,          /**< Short Form Expanded User Identity
                                           Module Identifier. */
  MMGSDI_CSIM_SRV_3GPD_EXT,           /**< Messaging and 3GPD extensions. */
  MMGSDI_CSIM_SRV_ROOT_CERT,          /**< Root certificates. */
  MMGSDI_CSIM_SRV_BROWSER,            /**< Browser. */
  MMGSDI_CSIM_SRV_JAVA,               /**< Java. */
  MMGSDI_CSIM_SRV_LBS,                /**< Location Based Services. */
  MMGSDI_CSIM_SRV_BREW,               /**< BREW. */

  /* 0x828 OR 0x800+40 */
  MMGSDI_CSIM_SRV_IPV6,                 /**< IPv6. */
  MMGSDI_SRV_MAX           = 0x7FFFFFFF /**< Forces an initialize size
                                             enumeration. */
}mmgsdi_service_enum_type;

/*===========================================================================
   ENUM: MMGSDI_AUTN_CONTEXT_ENUM_TYPE
=============================================================================*/
/** Indicates to the UIM which authentication context is requested by the
    client.

    The enumerators map directly to values in Specification 31.102.
*/
typedef enum {
  MMGSDI_AUTN_USIM_GSM_CONTEXT                = 0x00,  /**< &nbsp; */
  MMGSDI_AUTN_USIM_3G_CONTEXT                 = 0x10,  /**< &nbsp; */
  MMGSDI_AUTN_USIM_VGCS_VBS_CONTEXT           = 0x20,  /**< Voice Group Call
                                                       Service Voice Broadcast
                                                       Service context. */
  MMGSDI_AUTN_USIM_GBA_BOOTSTRAPPING_CONTEXT  = 0x41,  /**< Generic Bootstrapping
                                                       Architecture context,
                                                       Bootstrapping mode. */
  MMGSDI_AUTN_USIM_GBA_NAF_DERIVATION_CONTEXT = 0x42,  /**< Generic Bootstrapping
                                                       Architecture context,
                                                       Network Application
                                                       Function Derivation mode. */
  MMGSDI_AUTN_USIM_MBMS_MSK_UPDATE_CONTEXT    = 0x51,  /**< Multimedia Broadcast/
                                                       Multicast Service context,
                                                       MBMS Service Key Update
                                                       mode. */
  MMGSDI_AUTN_USIM_MBMS_SPE_DEL_CONTEXT       = 0x511, /**< MBMS context,
                                                       Service Protection
                                                       Encryption Audit mode. */
  MMGSDI_AUTN_USIM_MBMS_REC_DEL_CONTEXT       = 0x512, /**< USIM MBMS context,
                                                       Recording Audit mode. */
  MMGSDI_AUTN_USIM_MBMS_MTK_GEN_CONTEXT       = 0x52,  /**< USIM MBMS context,
                                                       MBMS Traffic Key
                                                       Generation mode. */
  MMGSDI_AUTN_USIM_MBMS_MSK_DEL_CONTEXT       = 0x53,  /**< USIM MBMS context,
                                                       MSK Deletion mode. */
  MMGSDI_AUTN_USIM_MBMS_MUK_DEL_CONTEXT       = 0x54,  /**< USIM MBMS context,
                                                       MBMS User Key Deletion
                                                       mode. */
  MMGSDI_AUTN_ISIM_CONTEXT                    = 0x60,  /**< ISIM AKA
                                                       security context.*/
  MMGSDI_AUTN_ISIM_HTTP_DIGEST_CONTEXT        = 0x62,  /**< ISIM HTTP DIGEST
                                                       mode security context */
  MMGSDI_AUTN_ISIM_GBA_BOOTSTRAPPING_CONTEXT  = 0x641, /**< ISIM Generic Bootstrapping
                                                       Architecture context,
                                                       Bootstrapping mode. */
  MMGSDI_AUTN_ISIM_GBA_NAF_DERIVATION_CONTEXT = 0x642, /**< ISIM Generic Bootstrapping
                                                       Architecture context,
                                                       Network Application
                                                       Function Derivation mode. */

  MMGSDI_USIM_AUTN_MAX                        = 0x7FFFFFFF /**< Forces an
                                                           initialize size
                                                           enumeration. */
}mmgsdi_autn_context_enum_type;

/*===========================================================================
   ENUM:      MMGSDI_JCDMA_CARD_INFO_ENUM_TYPE
=============================================================================*/
/** UIM card information on power up (for Japanese CDMA).
*/
typedef enum {
  MMGSDI_JCDMA_NON_JCDMA_CARD,          /**< Non-JCDMA card. */
  MMGSDI_JCDMA_WHITE_CARD,              /**< White card; non-operational. */
  MMGSDI_JCDMA_GRAY_CARD,               /**< Gray card; to be activated,
                                             requires proprietary procedures. */
  MMGSDI_JCDMA_GRAY_CARD_LINE_LOCKED,   /**< Gray card; line locked. */
  MMGSDI_JCDMA_BLACK_CARD,              /**< Black card; operational. */
  MMGSDI_JCDMA_BLACK_CARD_NEW_CARD,     /**< New black card; to be activated,
                                             requires proprietary procedures. */
  MMGSDI_JCDMA_BLACK_CARD_LINE_LOCKED,  /**< Black card; line locked. */
  MMGSDI_JCDMA_BLACK_CARD_NW_LOCKED,    /**< Black card; network locked. */
  MMGSDI_JCDMA_BLACK_CARD_SP_LOCKED,    /**< Black card; service provider
                                             locked. */
  MMGSDI_JCDMA_BLACK_CARD_UG_LOCKED,    /**< Black card; user group locked. */
  MMGSDI_JCDMA_UNKNOWN_CARD,            /**< Invalid value. */
  MMGSDI_JCDMA_BLACK_WHITE_CARD,   /**< Black and white card; a new combination
                                        for CSIM color and lock implementation. */
  MMGSDI_JCDMA_WHITE_BLACK_CARD,   /**< White and black card; a new combination
                                        for CSIM color and lock implementation. */
  MMGSDI_JCDMA_GRAY_BLACK_CARD,    /**< Gray and black card; a new combination
                                        for CSIM color and lock implementation. */
  MMGSDI_JCDMA_GRAY_BLACK_CARD_LINE_LOCKED, /**< Gray and black card; line
                                            locked; a new combination for CSIM
                                            color and lock implementation. */
  MMGSDI_JCDMA_GRAY_WHITE_CARD     /**< Gray and white card; a new combination
                                        for CSIM color and lock implementation. */
}mmgsdi_jcdma_card_info_enum_type;

/*===========================================================================
   ENUM: MMGSDI_ESN_ME_CHANGE_ENUM_TYPE
===========================================================================*/
/** Indicates whether ESN ME has changed, as defined in specification
    3GPP2 C.S0023-B.
*/
typedef enum {
  MMGSDI_ESN_ME_UNKNOWN              = 0,  /** ESN status is unknown */
  MMGSDI_ESN_ME_NO_CHANGE            = 1,  /** No change in ESN */
  MMGSDI_ESN_ME_CHANGE               = 2,  /** ESN has changed */
  MMGSDI_ESN_ME_UPDATE_PENDING       = 3,  /** ESN has changed but needs update in card */
  MMGSDI_ESN_ME_CHANGE_NOT_SUPPORTED = 4   /** ESN is not supported */
}mmgsdi_esn_me_change_enum_type;

/*===========================================================================
   ENUM: MMGSDI_MFLO_UCAST_MSG_ENUM_TYPE
===========================================================================*/
/** Type of MediaFlo&tm;&nbsp;Unicast message block types: a single block or
    a sequence of blocks. Any other value is invalid.
*/
typedef enum {
  MMGSDI_MFLO_UCAST_MSG_FIRST_BLOCK  = 0x00, /**< First block in a sequence. */
  MMGSDI_MFLO_UCAST_MSG_NEXT_BLOCK   = 0x01, /**< Next block in a sequence. */
  MMGSDI_MFLO_UCAST_MSG_LAST_BLOCK   = 0x10, /**< Last block in a sequence. */
  MMGSDI_MFLO_UCAST_MSG_SINGLE_BLOCK = 0x11  /**< Single block for a message. */
  // Any other value is invalid
}mmgsdi_mflo_ucast_msg_enum_type;

/*===========================================================================
   ENUM: MMGSDI_BCAST_MODE_ENUM_TYPE
===========================================================================*/
/** Type of Open Mobile Alliance broadcast mode.

    See <i>Service and Content Protection for Mobile Broadcast Services</i>,
    Approved Version 1.0 - 12 Feb 2009.
*/
typedef enum {
  MMGSDI_BCAST_SPE_AUDIT_MODE,        /**< Service Protection Encryption
                                           Audit mode. */
  MMGSDI_BCAST_SPE_SIG_MODE,          /**< SPE Record Signaling mode. */
  MMGSDI_BCAST_REC_AUDIT_MODE,        /**< Recording Audit mode. */
  MMGSDI_BCAST_MODE_MAX = 0x7FFFFFFF  /**< Forces an initialize size enumeration. */
}mmgsdi_bcast_mode_enum_type;

/*===========================================================================
   ENUM: MMGSDI_SEARCH_ENUM_TYPE
===========================================================================*/
/** Type of search on an ICC or UICC card.
*/
typedef enum {
  MMGSDI_SEARCH_NONE           = 0x00000000, /**< No search type. */
  MMGSDI_ICC_SEARCH,                         /**< Search commands on an ICC card. */
  MMGSDI_UICC_SIMPLE_SEARCH,                 /**< Simple search record on a UICC
                                                  card. */
  MMGSDI_UICC_ENHANCED_SEARCH,               /**< Enhanced search record on a
                                                  UICC card. */
  MMGSDI_MAX_SEARCH_ENUM       = 0x7FFFFFFF  /**< Forces an initialize size
                                                  enumeration. */
  // Any other value is invalid
}mmgsdi_search_enum_type;

/*===========================================================================
  ENUM:      MMGSDI_SEEK_DIRECTION_ENUM_TYPE
===========================================================================*/
/** Direction of the search in an EF on an ICC card.
*/
typedef enum {
  MMGSDI_SEEK_DIRECTION_NONE       = 0x00000000, /**< No search direction. */
  MMGSDI_SEEK_BEGINNING_FORWARDS,                /**< Start from first record
                                                      and move forward. */
  MMGSDI_SEEK_END_BACKWARDS,                     /**< Start from last record
                                                      and move backward. */
  MMGSDI_MAX_SEEK_DIRECTION_ENUM   = 0x7FFFFFFF  /**< Forces an initialize size
                                                      enumeration. */
}mmgsdi_seek_direction_enum_type;

/*===========================================================================
  ENUM:      MMGSDI_SEARCH_DIRECTION_ENUM_TYPE
===========================================================================*/
/** Direction of the search in an EF on a UICC card.
*/
typedef enum {
  MMGSDI_SEARCH_DIRECTION_NONE         = 0x00000000, /**< No search direction. */
  MMGSDI_SEARCH_FORWARD_FROM_REC_NUM,      /**< Start forward from a specified
                                                record number. */
  MMGSDI_SEARCH_BACKWARD_FROM_REC_NUM,     /**< Start backward from a specified
                                                record number. */
  MMGSDI_MAX_SEARCH_DIRECTION_ENUM     = 0x7FFFFFFF  /**< Forces an initialize
                                                          size enumeration. */
}mmgsdi_search_direction_enum_type;

/*===========================================================================
  ENUM:      MMGSDI_SEARCH_OPTION_ENUM_TYPE
===========================================================================*/
/** Type of search in a record on a UICC card.
*/
typedef enum {
   MMGSDI_SEARCH_OPTION_NONE     = 0x00000000, /**< No search direction. */
   MMGSDI_SEARCH_BY_OFFSET,                    /**< Searches in a pattern that
                                               starts from a specified offset. */
   MMGSDI_SEARCH_BY_CHAR,                      /**< Searches in a pattern that
                                               starts with a character value (one
                                               byte of information). */
   MMGSDI_MAX_SEARCH_OPTION_ENUM = 0x7FFFFFFF  /**< Forces an initialize size
                                                    enumeration. */
}mmgsdi_search_option_enum_type;

/*=============================================================================
  ENUM:         MMGSDI_SESSION_TYPE_ENUM_TYPE
===============================================================================*/

/** Types of sessions.
*/
typedef enum {
  MMGSDI_GW_PROV_PRI_SESSION = 0x000000000,
     /**< Session associated with the default (primary) GSM/WCDMA provisioning
          application (for USIM applications or SIM requests). */
  MMGSDI_1X_PROV_PRI_SESSION,
     /**< Session associated with the default (primary) CDMA 1X provisioning
          application (for CSIM applications or RUIM requests). */
  MMGSDI_GW_PROV_SEC_SESSION,
     /**< Session associated with a secondary GW application. */
  MMGSDI_1X_PROV_SEC_SESSION,
     /**< Session associated with a secondary 1X application. */
  MMGSDI_NON_PROV_SESSION_SLOT_1,
     /**< Session associated with a nonprovisioning application for a card in
          slot 1. */
  MMGSDI_NON_PROV_SESSION_SLOT_2,
     /**< Session associated with a nonprovisioning application for a card in
          slot 2. */
  MMGSDI_CARD_SESSION_SLOT_1,
     /**< Session associated with a card in slot 1 that is being used for
          card-related services. */
  MMGSDI_CARD_SESSION_SLOT_2,
     /**< Session associated with a card in slot 2 that is being used for
          card-related services. */
  MMGSDI_GW_PROV_TER_SESSION,
     /**< Session associated with a tertiary GW provisioning application. */
  MMGSDI_1X_PROV_TER_SESSION,
     /**< Session associated with a tertiary 1X provisioning application. */
  MMGSDI_NON_PROV_SESSION_SLOT_3,
     /**< Session associated with a nonprovisioning application for a card in
          slot 3. */
  MMGSDI_CARD_SESSION_SLOT_3,
     /**< Session associated with a card in slot 3 that is being used for
          card-related services. */
  MMGSDI_MAX_SESSION_TYPE_ENUM = 0x7FFFFFFF
     /**< Forces an initialize size enumeration. */
} mmgsdi_session_type_enum_type;

/*===========================================================================
   Enum:    MMGSDI_ONCHIP_SIM_STATE_ENUM_TYPE
=============================================================================*/
/** Defines the values that depend on the on-chip SIM state.
*/
typedef enum {
  MMGSDI_ONCHIP_SIM_NOT_VALID      = 0x0000, /**< SIM is not initialized or is
                                                  not running. */
  MMGSDI_ONCHIP_SIM_INIT,                    /**< SIM is initialized. */
  MMGSDI_ONCHIP_SIM_STATE_MAX_ENUM = 0x7FFF  /**< SIM is initialized, which means
                                             the initialization and populating
                                             of the SIM cache has completed. */
}mmgsdi_onchip_sim_state_enum_type;

/*=============================================================================
  ENUM:           MMGSDI_PROTOCOL_ENUM_TYPE
===============================================================================*/
/** Card protocol type.
*/
typedef enum {
  MMGSDI_NO_PROTOCOL = 0x000000000,     /**< No protocol type is associated with
                                             the card. */
  MMGSDI_ICC,                           /**< Integrated Circuit Card. */
  MMGSDI_UICC,                          /**< Universal Integrated Circuit Card. */
  MMGSDI_MAX_PROTOCOL_ENUM = 0x7FFFFFFF /**< Forces an initialize size
                                             enumeration. */
} mmgsdi_protocol_enum_type;

/*=============================================================================
   ENUM:      MMGSDI_COMPUTE_IP_OPERATION_ENUM_TYPE
===========================================================================*/
/** Type of compute IP authentication that is being requested.
*/
typedef enum {
  MMGSDI_CMPT_IP_SIP_CHAP       = 0, /**< Performs CHAP operation for the SIP. */
  MMGSDI_CMPT_IP_MIP_MN_HA      = 1, /**< Calculates the Mobile Network Home
                                          Agent authenticator. */
  MMGSDI_CMPT_IP_MIP_RRQ        = 2, /**< Calculates the Mobile IP Registration
                                          Request hash. */
  MMGSDI_CMPT_IP_MIP_MN_AAA     = 3, /**< Calculates the Mobile Network
                                          Authentication, Authorization, and
                                          Accounting authenticator. */
  MMGSDI_CMPT_IP_HRPD_CHAP      = 4, /**< Performs a CHAP operation for HRPD. */
  MMGSDI_CMPT_IP_MAX_VALUE      = 5  /**< Maximum number of enumerators. */
} mmgsdi_compute_ip_operation_enum_type;

/*=============================================================================
   ENUM:      MMGSDI_OTASP_BLOCK_OP_ENUM_TYPE
=============================================================================*/
/** Type of OTASP block operation that is being requested.
*/
typedef enum {
  MMGSDI_OTASP_CONFIG           = 0,         /**< Performs a configuration. */
  MMGSDI_OTASP_DOWNLOAD         = 1,         /**< Performs a download. */
  MMGSDI_OTASP_SSPR_DOWNLOAD    = 2,         /**< Performs a System Selection
                                                  with  Preferred Roaming
                                                  download. */
  MMGSDI_OTASP_VALIDATE         = 3,         /**< Performs a validation. */
  MMGSDI_OTASP_MAX_VALUE        = 0x7FFFFFFF /**< Forces an initialize size
                                                  enumeration. */
} mmgsdi_otasp_block_op_enum_type;

/*===========================================================================
  ENUM:   MMGSDI_CURRENT_SERVICE_ENUM_TYPE
===========================================================================*/
/** Status of the current service.

    This enumeration is used to maintain the Service Table information as it
    pertains to the EF-CPHSINFO file. This file contains information for the
    Common PCN Handset Specification for GSM handsets.
*/
typedef enum {
  MMGSDI_NOT_ACTIVATED_AND_NOT_ALLOCATED = 0x00,
                   /**< Current service is not activated and not allocated. */
  MMGSDI_NOT_ACTIVATED_AND_ALLOCATED     = 0x01,
                   /**< Current service is allocated but not activated. */
  MMGSDI_ACTIVATED_AND_NOT_ALLOCATED     = 0x02,
                   /**< Current service is activated but not allocated. */
  MMGSDI_ACTIVATED_AND_ALLOCATED         = 0x03
                   /**< Current service is both activated and allocated. */
}mmgsdi_current_service_enum_type;

/*===========================================================================
  ENUM:   MMGSDI_CSP_ENUM_TYPE
===========================================================================*/
/** Indicates if the Customer Service Profile information is on or off.
*/
typedef enum {
  MMGSDI_CSP_IND_OFF = 0x00,   /**< &nbsp; */
  MMGSDI_CSP_IND_ON  = 0x01    /**< &nbsp; */
}mmgsdi_csp_enum_type;

/*===========================================================================
  ENUM:   MMGSDI_IN_TC_MODE_ENUM_TYPE
===========================================================================*/
/** Enums for In Traffic Control mode used during set/unset UIM power control
*/
typedef enum {
  MMGSDI_NOT_IN_TC    = 0x00000000,
  MMGSDI_IN_TC_GW,
  MMGSDI_IN_TC_1X
}mmgsdi_in_tc_mode_enum_type;

/*=============================================================================
  ENUM:   MMGSDI_SELECT_RSP_ENUM_TYPE
===========================================================================*/
/** Type of response requested as part of a SELECT
*/
typedef enum {
  MMGSDI_SELECT_RSP_NO_DATA             = 0x00,  /**< No data requested on app
                                                      selection */
  MMGSDI_SELECT_RSP_FCP                 = 0x01,  /**< File Control Parameter
                                                      requested on selection */
  MMGSDI_SELECT_RSP_FMD                 = 0x02,  /**< File Management Data
                                                      requested on selection */
  MMGSDI_SELECT_RSP_FCI                 = 0x03,  /**< File Control Information
                                                      requested on selection */
  MMGSDI_SELECT_RSP_FCI_WITH_INTERFACES = 0x04   /**< File Control Information
                                                      with interfaces requested
                                                      on selection */
} mmgsdi_select_rsp_enum_type;

/*=============================================================================
  ENUM:   MMGSDI_EONS_ENCODING_ENUM_TYPE
=======================================================================*/
/** Encoding type enumerator for operator names. Enum members are derived by
   referring 3G TS 23.038, 3GPP TS 24.008 and  3GPP2 C.S0001 standard documents.
*/
typedef enum {
  MMGSDI_EONS_ENC_INVALID                     = 0x00, /**< Invalid encoding. */
  MMGSDI_EONS_ENC_GSM_7BIT_DEF_ALPHA_PACKED,          /**< GSM 7-bit default
                                          alphabet encoding, 3GPP TS 24.008. */
  MMGSDI_EONS_ENC_GSM_7BIT_DEF_ALPHA_UNPACKED,        /**< Default 7 bit coded
                                          alphabet(with bit 8 set to 0)
                                          encoding, 3G TS 23.038. */
  MMGSDI_EONS_ENC_UCS2,                                /**< UCS2 normal encoding,
                                          that is, USC2 encoding where data is
                                          in unpacked 16-bit format, 3GPP TS 24.008. */
  MMGSDI_EONS_ENC_UNSPECIFIED,                        /**< Cdma spn specific,
                                          3GPP2 C.S0001-D v2.0. */
  MMGSDI_EONS_ENC_EXT_PROTOCOL_MSG,                   /**< Cdma spn specific,
                                          3GPP2 C.S0001-D v2.0. */
  MMGSDI_EONS_ENC_7BIT_ASCII,                         /**< Cdma spn specific,
                                          3GPP2 C.S0001-D v2.0. */
  MMGSDI_EONS_ENC_IA5,                                /**< Cdma spn specific,
                                          3GPP2 C.S0001-D v2.0. */
  MMGSDI_EONS_ENC_SHIFT_JIS,                          /**< Cdma spn specific,
                                          3GPP2 C.S0001-D v2.0. */
  MMGSDI_EONS_ENC_KOREAN,                             /**< Cdma spn specific,
                                          3GPP2 C.S0001-D v2.0. */
  MMGSDI_EONS_ENC_LATIN_HEBREW,                       /**< Cdma spn specific,
                                          3GPP2 C.S0001-D v2.0. */
  MMGSDI_EONS_ENC_LATIN,                              /**< Cdma spn specific,
                                          3GPP2 C.S0001-D v2.0. */
  MMGSDI_EONS_ENC_GSM_DCS,                            /**< Cdma spn specific,
                                          3GPP2 C.S0001-D v2.0. */
  MMGSDI_EONS_ENC_UCS2_PACKED                         /**< UCS2 packed encoding,
                                          that is, USC2 encoding where first
                                          byte of 16-bit format data is 0x80,
                                          0x81 or 0x82, ETSI TS 102 221, Annex A */
 } mmgsdi_eons_encoding_enum_type;

/*=============================================================================
  ENUM:   MMGSDI_ALPHA_ENCODING_ENUM_TYPE
=======================================================================*/
/** ENUM for values of Data Encoding Scheme (DCS) byte in Alpha
*/
typedef enum {
  MMGSDI_ALPHA_ENC_8_BIT = 0x00, /**< 8-bit coding scheme. */
  MMGSDI_ALPHA_ENC_UCS2_8_BIT,   /**< UCS-2 8-bit coding scheme. */
  MMGSDI_ALPHA_ENC_UCS2_7_BIT,   /**< UCS-2 7-bit coding scheme. */
  MMGSDI_ALPHA_ENC_7_BIT,        /**< 7-bit coding scheme. */
  MMGSDI_ALPHA_ENC_NONE,         /**< Coding scheme is not specified. */
  MMGSDI_ALPHA_ENC_MAX = 0xFFFF  /**< Maximum value of the coding scheme type. */
 } mmgsdi_alpha_encoding_enum_type;

/*=============================================================================
  ENUM:   MMGSDI_EONS_SPN_DISPLAY_BIT_ENUM_TYPE
=======================================================================*/
/** ENUM for values of Bit2 in EF-SPN Display byte or values of Bit1 in
    display byte of EF-CDMA SPN.
*/
typedef enum {
  MMGSDI_EONS_SPN_DISPLAY_BIT_NOT_AVAILABLE = 0x00,
  MMGSDI_EONS_SPN_DISPLAY_BIT_VAL_0, /*Bit value is 0 in display byte*/
  MMGSDI_EONS_SPN_DISPLAY_BIT_VAL_1 /*Bit value is 1 in display byte*/
} mmgsdi_eons_spn_display_bit_enum_type;

/*=============================================================================
  ENUM:   MMGSDI_EONS_RPLMN_DISPLAY_BIT_ENUM_TYPE
=======================================================================*/
/** ENUM for values of Bit1 in EF-SPN Display byte
*/
typedef enum {
  MMGSDI_EONS_RPLMN_DISPLAY_BIT_NOT_AVAILABLE = 0x00,
  MMGSDI_EONS_RPLMN_DISPLAY_BIT_VAL_0, /*Bit1 is 0 in EF-SPN display byte*/
  MMGSDI_EONS_RPLMN_DISPLAY_BIT_VAL_1 /*Bit1 is 1 in EF-SPN display byte*/
} mmgsdi_eons_rplmn_display_bit_enum_type;

/*=============================================================================
  ENUM:   MMGSDI_EONS_ROAMING_STATUS_ENUM_TYPE
=======================================================================*/
/** ENUM for network's home and roaming status
*/
typedef enum {
  MMGSDI_EONS_NW_INFO_NOT_AVAILABLE = 0x00,
  MMGSDI_EONS_IN_HOME_NW,
  MMGSDI_EONS_IN_ROAMING_NW
} mmgsdi_eons_roaming_status_enum_type;

/*=============================================================================
  ENUM:   MMGSDI_APP_STATE_ENUM_TYPE
=======================================================================*/
/** Provisining and non-provisining Application states
*/
typedef enum {
  MMGSDI_APP_STATE_UNKNOWN,      /**< Before Card is detected or after Card is removed.  In the
                                     case of UICC, before the EF-DIR is read. */
  MMGSDI_APP_STATE_DETECTED,     /**< After the Card has been detected and in case of UICC, after
                                      the EF-DIR is read */
  MMGSDI_APP_STATE_DEACTIVATED,        /**< This indicates that the application has been deactivated.
                                            This is a transcient state */
  MMGSDI_APP_STATE_ACTIVATED,     /**< After the provisioning application has been activate/switch.
                                       Applicable to Prov Application(s) */
  MMGSDI_APP_STATE_PIN_EVT_SENT,      /**< After PIN event is sent.  Applicable to Prov Application(s)*/
  MMGSDI_APP_STATE_READY_FOR_PERSO,       /**< Indicates Applocation is ready for Personalization
                                             Applicable to Prov Application(s) */
  MMGSDI_APP_STATE_PERSO_VERIFIED,      /**< Indicates Personalization has been completed.
                                             Applicable to Prov Application(s) */
  MMGSDI_APP_STATE_READY,              /**< For Provisioning application(s), this indicates SUBSCRIPTION
                                            READY event has been sent.
                                            For Non Provisioning application, this indicates after a
                                            successful session_open and selection of the application on
                                            the card.
                                            For application that is shared between a PROV and non PROV
                                            client, the state follows the Provisioning application */
  MMGSDI_APP_STATE_ILLEGAL,            /**< When application has been set as illegal by NAS */
  MMGSDI_APP_STATE_WAIT_FOR_SUB_OK,       /**< When application is waiting for
                                               external perso */
  MMGSDI_APP_STATE_EXT_PERSO_FAILED,   /**< When external perso fails for
                                              the application */
  MMGSDI_APP_STATE_ENUM_MAX = 0x7FFFFFFF
} mmgsdi_app_state_enum_type;

/*=============================================================================
  ENUM:   MMGSDI_RAT_ENUM_TYPE
=======================================================================*/
/** Radio access technology associated with plmn id
*/
typedef enum
{
  MMGSDI_RAT_NONE, /**< RAT type none*/
  MMGSDI_RAT_GSM,  /**< GSM radio access technology. */
  MMGSDI_RAT_UMTS, /**< UMTS radio access technology. */
  MMGSDI_RAT_LTE,  /**< LTE radio access technology. */
  MMGSDI_RAT_TDS,  /**< TD-SCDMA radio access technology */
  MMGSDI_RAT_MAX = 0x7FFFFFFF  /**< Forces an initialize size enumeration. */
}mmgsdi_rat_enum_type;

/*=============================================================================
  ENUM:   MMGSDI_SAP_CONN_CONDTION_ENUM_TYPE
=============================================================================*/
/** SAP connection condition type
*/
typedef enum
{
  MMGSDI_SAP_CONN_COND_BLOCK_VOICE_OR_DATA = 0x00, /**< Block SAP when data or
                                                        //voice call is active */
  MMGSDI_SAP_CONN_COND_BLOCK_DATA          = 0x01, /**< Block SAP when only
                                                        //data call is active */
  MMGSDI_SAP_CONN_COND_BLOCK_VOICE         = 0x02, /**< Block SAP when only
                                                        //voice call is active */
  MMGSDI_SAP_CONN_COND_BLOCK_NONE          = 0x03  /**< Allow SAP in all cases */
}mmgsdi_sap_conn_condition_enum_type;

/*=============================================================================
  ENUM:   MMGSDI_EONS_IGNORE_DISP_COND_ENUM_TYPE
=======================================================================*/
/** To ignore display condition provided in EF-SPN for SPN and RPLMN name
*/
typedef enum
{
 MMGSDI_EONS_IGNORE_DISPLAY_COND_NONE, /**< Don't ignore any display
                                            condition */
 MMGSDI_EONS_IGNORE_SPN_DISPLAY_COND,  /**< Ignore SPN display condition */
 MMGSDI_EONS_IGNORE_RPLMN_DISPLAY_COND, /**< Ignore RPLMN display condition */
 MMGSDI_EONS_IGNORE_SPN_RPLMN_DISPLAY_COND, /**< Ignore SPN and RPLMN display
                                                 condition (This enum value is
                                                 not valid for 1X Provisioning
                                                 Sessions) */
 MMGSDI_EONS_IGNORE_DISPLAY_COND_MAX = 0x7FFFFFFF  /**< Forces an initialize
                                                        size enumeration. */
} mmgsdi_eons_ignore_disp_cond_enum_type;

/*=============================================================================
  ENUM:   MMGSDI_SYNC_FAIL_ENUM_TYPE
=============================================================================*/
/** Identify if resynchronization is required for CDMA AKA authentication
*/
typedef enum
{
  MMGSDI_CDMA_AKA_VALID_SEQ_NUM       = 0x00, /**< Sequence numbers valid
                                                   use include CK, IK and RES */
  MMGSDI_CDMA_AKA_INVALID_SEQ_NUM     = 0x01  /**< Sequence numbers invalid
                                                   use include AUTS */
} mmgsdi_sync_fail_enum_type;

/*=============================================================================
  ENUM:   MMGSDI_SESSION_CLOSED_CAUSE_ENUM_TYPE
=============================================================================*/
/** Session closed cause type
*/
typedef enum
{
  MMGSDI_SESSION_CLOSED_CAUSE_UNKNOWN,
  MMGSDI_SESSION_CLOSED_CAUSE_CLIENT_REQUEST,
  MMGSDI_SESSION_CLOSED_CAUSE_CARD_ERROR,
  MMGSDI_SESSION_CLOSED_CAUSE_CARD_POWER_DOWN,
  MMGSDI_SESSION_CLOSED_CAUSE_CARD_REMOVED,
  MMGSDI_SESSION_CLOSED_CAUSE_REFRESH,
  MMGSDI_SESSION_CLOSED_CAUSE_PIN_STATUS_FAIL,
  MMGSDI_SESSION_CLOSED_CAUSE_RECOVERY,
  MMGSDI_SESSION_CLOSED_CAUSE_FDN_SRV_ENABLED_WITH_NO_FDN_SUPPORT,
  MMGSDI_SESSION_CLOSED_CAUSE_PERSO_OPERATION_FAIL,
  MMGSDI_SESSION_CLOSED_CAUSE_MISMATCH_FOUND,

  /* File Content Invalid */
  MMGSDI_SESSION_CLOSED_CAUSE_FILE_CONTENT_INVALID_IMSI,
  MMGSDI_SESSION_CLOSED_CAUSE_FILE_CONTENT_INVALID_ACC,
  MMGSDI_SESSION_CLOSED_CAUSE_FILE_CONTENT_INVALID_PRL,

  /* Mandatory Files Missing */
  MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_AD,
  MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_IMSI,
  MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_ACC,
  MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_HPPLMN,
  MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_LOCI,
  MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_PSLOCI,
  MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_KEYS,
  MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_FPLMN,
  MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_START_HFN,
  MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_THRESHOLD,
  MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_KC,
  MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_PHASE,
  MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_CST,
  MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_RUIM_ID,
  MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_SERVICE_PREF,
  MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_IMSI_M,
  MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_IMSI_T,
  MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_ACCOLC,
  MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_DIST_BASED_REGN_IND,
  MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_SYS_REGN_IND,
  MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_OTAPA_SPC_ENABLE,
  MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_NAM_LOCK,
  MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_HOME_SID_NID,
  MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_ZONE_BASED_REGN_IND
}mmgsdi_session_closed_cause_enum_type;

/*=============================================================================
  ENUM:   MMGSDI_EONS_NAME_SOURCE_ENUM_TYPE
=============================================================================*/
/** The source of EONS name
*/
typedef enum
{
  MMGSDI_EONS_NAME_SOURCE_UNKNOWN = 0x00,
  MMGSDI_EONS_NAME_SOURCE_OPL_PNN,
  MMGSDI_EONS_NAME_SOURCE_CPHS_ONS,
  MMGSDI_EONS_NAME_SOURCE_NITZ,
  MMGSDI_EONS_NAME_SOURCE_SE13,
  MMGSDI_EONS_NAME_SOURCE_MCC_MNC
}mmgsdi_eons_name_source_enum_type;

/*=============================================================================
  ENUM:   MMGSDI_SELECT_MODE_ENUM_TYPE
=============================================================================*/
/** The mode for SELECT command
*/
typedef enum
{
  MMGSDI_SELECT_MODE_UNKNOWN = 0x00,
  MMGSDI_SELECT_MODE_FIRST_OR_ONLY,
  MMGSDI_SELECT_MODE_LAST,
  MMGSDI_SELECT_MODE_NEXT,
  MMGSDI_SELECT_MODE_PREVIOUS
}mmgsdi_select_mode_enum_type;

/*===========================================================================
  ENUM:   MMGSDI_APP_ACT_STATUS_ENUM_TYPE
===========================================================================*/
/** Enums for determining if an app (such as CSIM) is activated (that is,
    IMSI_M for CSIM is programmed) or not
*/
typedef enum {
  MMGSDI_ACT_STATUS_UNKNOWN,
  MMGSDI_ACT_STATUS_ACTIVATED,
  MMGSDI_ACT_STATUS_NOT_ACTIVATED,
  MMGSDI_ACT_STATUS_NOT_APPLICABLE = 0x7F
}mmgsdi_app_act_status_enum_type;

/*===========================================================================
  ENUM:   MMGSDI_PROFILE_ID_ENUM_TYPE
===========================================================================*/
/** Enums for profile IDs used in profile data
*/
typedef enum {
  MMGSDI_PROFILE_1              = 0x01,
  MMGSDI_PROFILE_2              = 0x02,
  MMGSDI_PROFILE_3              = 0x03,
  MMGSDI_PROFILE_4              = 0x04,
  MMGSDI_PROFILE_5              = 0x05,
  MMGSDI_PROFILE_6              = 0x06,
  MMGSDI_PROFILE_7              = 0x07,
  MMGSDI_PROFILE_8              = 0x08,
  MMGSDI_PROFILE_NOT_APPLICABLE = 0x7F
}mmgsdi_profile_id_enum_type;

/*===========================================================================
  ENUM:   MMGSDI_PROFILE_TYPE_ENUM_TYPE
===========================================================================*/
/** Enums for profile type used in profile data
*/
typedef enum {
  MMGSDI_PROFILE_TYPE_REGULAR        = 0x00,
  MMGSDI_PROFILE_TYPE_EMERGENCY      = 0x01,
  MMGSDI_PROFILE_TYPE_NOT_APPLICABLE = 0x7F
}mmgsdi_profile_type_enum_type;

/** @} */ /* end_addtogroup common_library */
/** @addtogroup common_library
@{ */

/*=============================================================================

                       ABSTRACT DATA TYPE DECLARATIONS

=============================================================================*/

/*-----------------------------------------------------------------------------
   DATA TYPE:      MMGSDI_CLIENT_ID_TYPE
-----------------------------------------------------------------------------*/
/** Client ID that a client of a new MMGSDI API must obtain from the MMGSDI
    task. This ID is obtained by using the be  mmgsdi_client_id_reg() function.
*/
typedef uint64 mmgsdi_client_id_type;

/* ----------------------------------------------------------------------------
   DATA TYPE:      MMGSDI_SESSION_ID_TYPE
-----------------------------------------------------------------------------*/
/** Session ID that clients of new session-related MMGSDI APIs must obtain from
    the MMGSDI task. This ID is obtained by calling the mmgsdi_session_open()
    function.
*/
typedef uint64 mmgsdi_session_id_type;

/* ----------------------------------------------------------------------------
   DATA TYPE:      MMGSDI_LEN_TYPE
-----------------------------------------------------------------------------*/
/** General length type definition.
*/
typedef int32 mmgsdi_len_type;

/* ----------------------------------------------------------------------------
   DATA TYPE:      MMGSDI_CLIENT_DATA_TYPE
-----------------------------------------------------------------------------*/
/** User data for the client.

    This pointer is accepted as part of the API and is returned to the client.
    A NULL is valid. The pointer is received as a void * to allow the client to
    point to any type of data structure.
*/
typedef uint32 mmgsdi_client_data_type;

/* ----------------------------------------------------------------------------
   DATA TYPE:      MMGSDI_REC_NUM_TYPE
-----------------------------------------------------------------------------*/
/** Record number for linear fixed and cyclic files.
*/
typedef int32 mmgsdi_rec_num_type;

/* ----------------------------------------------------------------------------
   DATA TYPE:      MMGSDI_CSG_ID_TYPE
-----------------------------------------------------------------------------*/
/** Length of csg_id.
*/
typedef uint32 mmgsdi_csg_id_type;

/** @} */ /* end_addtogroup common_library */
/** @addtogroup common_library
@{ */

/* ----------------------------------------------------------------------------
   STRUCTURE:    MMGSDI_DATA_TYPE
-----------------------------------------------------------------------------*/
/** @brief Generic data structure.
*/
typedef struct {
  mmgsdi_len_type    data_len;    /**< Length of the data. */
  uint8            * data_ptr;    /**< Pointer to the data. */
} mmgsdi_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:    MMGSDI_PLMN_ID_TYPE
-----------------------------------------------------------------------------*/
/** @brief structure contains uint8 array of length MMGSDI_PLMN_ID_SIZE to
     accommodate plmn identity, associated radio access technology and
     csg identity
*/
typedef struct{
  uint8                 plmn_id_val[MMGSDI_PLMN_ID_SIZE];  /**< value of plmn
                                                                identity. */
  mmgsdi_rat_enum_type  rat;                               /**< Radio access
                                                                technology */
  mmgsdi_csg_id_type    csg_id;                            /**< value of csg
                                                                identity. */
} mmgsdi_plmn_id_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:    MMGSDI_BUILTIN_PLMN_ID_TYPE
-----------------------------------------------------------------------------*/
/** @brief structure contains uint8 array of length MMGSDI_PLMN_ID_SIZE to
     accommodate plmn identity and associated access technology. This is based
     on format of records in EF-OPLMNwACT.
*/
typedef struct{
  uint8                 plmn_id_val[MMGSDI_PLMN_ID_SIZE];  /**< value of plmn
                                                                identity. */
  uint16                access_tech;                       /**<  access
                                                                technology */
} mmgsdi_builtin_plmn_id_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:    MMGSDI_PLMN_ID_LIST_TYPE
-----------------------------------------------------------------------------*/
/** @brief structure contains number of plmn ids and pointer to
    mmgsdi_plmn_id_type list
*/
typedef struct{
  uint32                 num_of_plmn_ids;  /**< Number of plmn ids. */
  mmgsdi_plmn_id_type  * plmn_list_ptr;    /**< Pointer to plmn id list. */
} mmgsdi_plmn_id_list_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:    MMGSDI_BUILTIN_PLMN_LIST_TYPE
-----------------------------------------------------------------------------*/
/** @brief structure contains number of builtin plmn ids and pointer to
    mmgsdi_builtin_plmn_id_type list
*/
typedef struct{
  uint16                         num_of_plmn_ids;  /**< Number of plmn ids. */
  mmgsdi_builtin_plmn_id_type  * plmn_list_ptr;    /**< Pointer to plmn id list. */
} mmgsdi_builtin_plmn_list_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:    MMGSDI_EONS_NAME_TYPE
-----------------------------------------------------------------------------*/
/** @brief structure consists of encoding type, data length and data
    pointer for operator name
*/
typedef struct{
  mmgsdi_eons_encoding_enum_type   eons_encoding;  /**< Encoding type. */
  mmgsdi_data_type                 eons_data;      /**< Eons data */
} mmgsdi_eons_name_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:    MMGSDI_PLMN_NAME_TYPE
-----------------------------------------------------------------------------*/
/** @brief structure consists of country initial info, spare bit info and
    operator name info
*/
typedef struct{
  mmgsdi_eons_name_type      plmn_name;             /**< Plmn name */
  boolean                    plmn_name_ci;          /**< Flag that indicates
                                                         whether the country
                                                         initials should be
                                                         added to plmn name. */
  uint8                      plmn_name_spare_bits;  /**< Number of spare bits
                                                         in the end octet of the
                                                         plmn name */
} mmgsdi_plmn_name_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:    MMGSDI_PLMN_LANG_TYPE
-----------------------------------------------------------------------------*/
/** @brief structure consists of language ID or codes as defined in ISO639 and
    ISO15924 encoded in SMS 7-bit coded alphabet
*/
typedef struct{
  uint8                   lang_len;             /**< Language length */
  char                    lang_id[MMGSDI_LANG_ID_LEN_MAX]; /**< Language ID such as:
                                                                "zh_hant" for Chinese trad
                                                                "zh_hans" for Chinese simp
                                                                */
} mmgsdi_plmn_lang_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:    MMGSDI_PLMN_ADDITIONAL_NAMES_TYPE
-----------------------------------------------------------------------------*/
/** @brief structure consists of PLMN's short name and long name in languages
    other than the default language (that is, english). It also consists of
    the language ID.
*/
typedef struct{
  mmgsdi_plmn_lang_type       plmn_lang;
                                                     /**< Language ID or
                                                          codes as defined in
                                                          ISO639 and ISO15924
                                                          encoded in SMS 7-bit
                                                          coded alphabet*/
  mmgsdi_plmn_name_type       plmn_long_name;        /**< Plmn long name. */
  mmgsdi_plmn_name_type       plmn_short_name;       /**< Plmn short name. */
} mmgsdi_plmn_additional_names_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:    MMGSDI_PLMN_ADDITIONAL_INFO_TYPE
-----------------------------------------------------------------------------*/
/** @brief structure consists of PLMN's additional info
*/
typedef struct{
  mmgsdi_eons_encoding_enum_type   info_encoding;  /**< Encoding type. */
  mmgsdi_data_type                 info_data;      /**< Additional info data */
} mmgsdi_plmn_additional_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:    MMGSDI_PLMN_INFO_TYPE
-----------------------------------------------------------------------------*/
/** @brief structure consists of PLMN's short name, long name and Id.
*/
typedef struct{
  mmgsdi_plmn_name_type             plmn_long_name;   /**< PLMN long name.  */
  mmgsdi_plmn_name_type             plmn_short_name;  /**< PLMN short name. */
  mmgsdi_plmn_id_type               plmn_id;          /**< PLMN identity.   */
  uint16                            lac;   /**< 0xFFFF will denote invalid
                                                value. LAC will be provided
                                                only with registered PLMN.  */
  mmgsdi_plmn_additional_info_type  plmn_additional_info;
                                           /**< PLMN additional information */
  uint8                             num_plmn_additional_names;
                                                     /**< Number of additional
                                                          languages supported
                                                          for the PLMN */
  mmgsdi_plmn_additional_names_type plmn_additional_names[MMGSDI_ADDITIONAL_NAMES_MAX];
                                                     /**< Plmn long and short names
                                                          in additional languages
                                                          such as Chinese. */
  mmgsdi_eons_name_source_enum_type  plmn_name_source;  /**< EONS name source */
} mmgsdi_plmn_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:    MMGSDI_LIST_PLMN_NAME_TYPE
-----------------------------------------------------------------------------*/
/** @brief structure contains number of PLMN name requested and respective list
    of plmn names.
*/
typedef struct{
  uint32                  num_of_plmn_ids;  /**< Number of plmn ids. */
  mmgsdi_plmn_info_type * plmn_info_ptr;    /**< Pointer to plmn info. */
} mmgsdi_list_plmn_name_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_RECORD_EF_INFO_TYPE
------------------------------------------------------------------------------*/
/** @brief structure contains info about total number of records, number of
    valid records, record len and list of valid record numbers in a record
    based EF file.
*/
typedef struct {
  mmgsdi_rec_num_type         num_of_rec;        /**< Number of records in the
                                                      file. */
  mmgsdi_len_type             rec_len;           /**< Length of each record. */
  mmgsdi_data_type            valid_record_list; /**< Number of valid
                                                      records and record
                                                      number list. */
} mmgsdi_record_ef_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_PIN_INFO_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when the status of all PINs is returned.
*/
typedef struct {
  mmgsdi_pin_enum_type         pin_id;          /**< Type of PIN: PIN1,
                                                     PIN2, UPIN, etc. */
  mmgsdi_pin_status_enum_type  status;          /**< Status of PIN checking. */
  mmgsdi_pin_replace_enum_type pin_replacement; /**< PIN, or replaced by UPIN. */
  uint32                       num_retries;     /**< Number of retries before
                                                     PIN is blocked. */
  uint32                       num_unblock_retries; /**< Number of consecutive
                                           false UNBLOCK PIN presentations
                                           allowed before unblocking is blocked. */
  boolean                      valid_num_retries; /**< Remaining retries
                                                       counter. */
  boolean                      valid_num_unblock_retries; /**< Remaining unblock
                                                               retries counter. */
} mmgsdi_pin_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_STATIC_DATA_TYPE
-----------------------------------------------------------------------------*/
/** @brief Generic data structure to hold static data.
*/
typedef struct {
  mmgsdi_len_type    data_len;                      /**< Length of the data. */
  uint8              data_ptr[MMGSDI_MAX_AID_LEN];  /**< Pointer to the data. */
}mmgsdi_static_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_PERSO_FEATURE_TYPE
-----------------------------------------------------------------------------*/
/** @brief Confirmation/response for the MMGSDI_PERSO_ACTIVATE, DEACTIVE,
    UNBLOCK, and DISABLE commands. This structure indicates whether the
    personalization feature was properly activated, deactivated, unblocked, or
    disabled.
*/
typedef struct {
  mmgsdi_perso_feature_enum_type   perso_feature; /**< Feature operated on. */
  uint32                           num_retries;   /**< Number of retries. */
}mmgsdi_perso_feature_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_PERSO_FEATURE_IND_TYPE
-----------------------------------------------------------------------------*/
/** @brief Confirmation/response for the MMGSDI_PERSO_GET_FEATURE_IND
    request/command.

    This structure returns the current status of all personalization features:
     - TRUE -- Active
     - FALSE -- Not active
*/
typedef struct {
    boolean                     nw_ind_status; /**< Network indicator status. */
    boolean                     ns_ind_status; /**< Network Subset indicator
                                                    status. */
    boolean                     sp_ind_status; /**< Service Provider indicator
                                                    status. */
    boolean                     cp_ind_status; /**< Corporate indicator status. */
    boolean                     sim_ind_status; /**< SIM indicator status. */

    boolean                     ruim_nw1_ind_status; /**< Network indicator
                                                          status. */
    boolean                     ruim_nw2_ind_status; /**< Network subset
                                                          indicator status. */
    boolean                     ruim_hrpd_ind_status; /**< HRPD indicator status. */
    boolean                     ruim_sp_ind_status; /**< Service Provider
                                                         indicator status. */
    boolean                     ruim_cp_ind_status; /**< Corporate indicator
                                                         status. */
    boolean                     ruim_ruim_ind_status; /**< SIM indicator status. */
}mmgsdi_perso_feature_ind_type;

/** @} */ /* end_addtogroup common_library */
/** @addtogroup common_library
@{ */

/* ----------------------------------------------------------------------------
   DATA TYPE:      MMGSDI_PERSO_OTA_DEPERSO_TYPE
-----------------------------------------------------------------------------*/
/** Confirmation/response for the MMGSDI_PERSO_OTA_DEPERSO command.
*/
typedef mmgsdi_data_type         mmgsdi_perso_ota_deperso_type;

/* ----------------------------------------------------------------------------
   DATA TYPE:      MMGSDI_PERSO_GET_FEATURE_DATA_TYPE
-----------------------------------------------------------------------------*/
/** Confirmation/response for the MMGSDI_PERSO_GET_FEATURE_DATA command. This
    typedef returns whether personalization data was successfully retrieved.
*/
typedef mmgsdi_data_type         mmgsdi_perso_get_feature_data_type;

/** @} */ /* end_addtogroup common_library */
/** @addtogroup common_library
@{ */

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_PERSO_DCK_RETRIES_TYPE
-----------------------------------------------------------------------------*/
/** @brief Confirmation/response for the MMGSDI_PERSO_GET_DCK_RETRIES. This
    structure gets the number of Depersonalization Control Key (DCK) retries
    still remaining.
*/
typedef struct {
  boolean                     unblock_retries;  /**< Number of unblock retries. */

  int32                       nw_num_retries;   /**< Number of network retries. */
  int32                       ns_num_retries;   /**< Number of network subset
                                                     retries. */
  int32                       sp_num_retries;   /**< Number of service provider
                                                     retries. */
  int32                       cp_num_retries;   /**< Number of corporate retries. */
  int32                       sim_num_retries;  /**< Number of SIM user retries. */

  int32                       ruim_nw1_num_retries;  /**< Number of RUIM network
                                                          1 retries. */
  int32                       ruim_nw2_num_retries;  /**< Number of RUIM network
                                                          2 retries. */
  int32                       ruim_hrpd_num_retries; /**< Number of RUIM HRPD
                                                          retries. */
  int32                       ruim_sp_num_retries;   /**< Number of RUIM service
                                                          provider retries. */
  int32                       ruim_cp_num_retries;   /**< Number of RUIM
                                                          corporate retries. */
  int32                       ruim_ruim_num_retries; /**< Number of RUIM RUIM
                                                          user retries. */
}mmgsdi_perso_dck_retries_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_PERSO_GET_FEATURE_KEY_TYPE
-----------------------------------------------------------------------------*/
/** @brief Confirmation/response for the MMGSDI_PERSO_GET_FEATURE_KEY
    request/command. This structure returns the control key for the feature.
*/
typedef struct {
  mmgsdi_perso_feature_enum_type feature; /**< Type of personalization. */
  mmgsdi_data_type               key;     /**< Control keys for personalization. */
}mmgsdi_perso_get_feature_key_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_AID_TYPE
-----------------------------------------------------------------------------*/
/** @brief Information about the Application Identifier (AID).
*/
typedef struct {
  mmgsdi_app_enum_type       app_type; /**< Type of card containing the
                                            application. */
  mmgsdi_static_data_type    aid;      /**< Application Identifier. */
  mmgsdi_static_data_type    label;    /**< Label for the AID. */
} mmgsdi_aid_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_APP_INFO_TYPE
-----------------------------------------------------------------------------*/
/** @brief All information for an application.
*/
typedef struct {
  boolean                   cached;
    /**< If set to TRUE, indicates that the application has been selected and
         that activation may be in progress (applicable only for provisioning
         applications). */
  mmgsdi_slot_id_enum_type  slot;     /**< Slot in which the card is inserted. */
  mmgsdi_pin_info_type      pin1;     /**< PIN1 information. */
  mmgsdi_pin_info_type      pin2;     /**< PIN2 information. */
  mmgsdi_pin_info_type      universal_pin; /**< Universal PIN information. */
/** @cond */
  boolean                   prov_app; /* Deprecated */
/** @endcond */
  mmgsdi_aid_type           app_data; /**< Information about the associated
                                           application. */
  mmgsdi_pin_info_type      hiddenkey; /**< EF Hidden Key used for hiding UICC
                                            phone book entries. */
  uint8                     dir_index; /**< Record number in the EF-DIR
                                            corresponding to the selected
                                            application. */
}mmgsdi_app_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_PATH_TYPE
-----------------------------------------------------------------------------*/
/** @brief Provides the entire path for accessing a file, starting from
    MF\\DF\\ADF.

    Path examples:
    - MF                   -- [3F00]
    - Current ADF          -- [3F00][7FFF]
    - GSM DF               -- [3F00][7F20] EF under
    - MF                   -- [3F00][EF] EF under current
    - ADF                  -- [3F00[7FFF][EF]
    - EF under Level 1 DF  -- [3F00][DF1][EF]
    - EF under Level 2 DF  -- [3F00][DF1][DF2][EF]

@note1hang DFs or EFs cannot be accessed under a non-current ADF without the
           ADF being activated before sending it down the path.
*/
typedef struct {
  mmgsdi_len_type       path_len;                       /**< Path length. */
  uint16                path_buf[MMGSDI_MAX_PATH_LEN];  /**< Path buffer. */
} mmgsdi_path_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_ACCESS_TYPE
-----------------------------------------------------------------------------*/
/** @brief Indicates which EF/DF/MF/ADF or path the client is to access.
*/
typedef struct {
  mmgsdi_access_method_enum_type   access_method; /**< Indicates how the file
                                                  union is to be interpreted. */

  /** Used by the mmgsdi_access_type structure to provide the specific file
      information. */
  union {
    mmgsdi_file_enum_type       file_enum; /**< Uses the file enumeration:
                                        access_method == MMGSDI_EF_ENUM_ACCESS. */
    mmgsdi_df_enum_type         df_enum;   /**< Uses the DF enumeration:
                                        access_method == MMGSDI_DF_ENUM_ACCESS. */
    mmgsdi_path_type            path_type; /**< Uses the path provided in the
                               request: access_method == MMGSDI_BY_PATH_ACCESS. */
    mmgsdi_static_data_type     app_id;    /**< Uses the application ID:
                                      access_method == MMGSDI_BY_APP_ID_ACCESS. */
  } file;
} mmgsdi_access_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_PERSO_LOCK_DOWN_TYPE
-----------------------------------------------------------------------------*/
/** @brief Confirmation/response for the MMGSDI_PERSO_LOCK_DOWN
    request/command. This structure returns the personalization feature key.
*/
typedef struct {
  mmgsdi_perso_lock_down_enum_type locked_command; /**< Personalization feature
                                                        key. */
}mmgsdi_perso_lock_down_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_REFRESH_FILE_LIST_TYPE
-----------------------------------------------------------------------------*/
/** @brief Information either for files that have been refreshed or for a list
    of files from which the client is to receive notification.

    For file_list_ptr and file_path_ptr, only one of the pointers shall point
    to a valid memory location. The other pointer shall be NULL. Enumerations
    in file_list_ptr and the variable file_path_len indicate the length in
    bytes for file_path_ptr.
*/
typedef struct {
   uint32                   num_files;      /**< Number of files. */
   mmgsdi_file_enum_type   *file_list_ptr;  /**< Pointer to the file list. */
   uint8                   *file_path_ptr;  /**< Pointer to the file path. */
   uint32                   file_path_len;  /**< Length of the file path. */
}mmgsdi_refresh_file_list_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SW_TYPE
-----------------------------------------------------------------------------*/
/** @brief Contains the status word returned from the card.

    Only certain commands have the status word set explicitly. For other
    confirmation types, present == FALSE. For any error generated internally by
    the MMGSDI or UIM during the command processing, the present field is set to
    FALSE.

    The status words are coded as per the TS 51.011 Specification of the SIM-ME
    interface, TS 102.221 UICC Terminal Interface, and 3GPP2 R-UIM
    specifications.
*/
typedef struct {
  boolean                  present; /**< Indicates whether Status Word 1 and
                                         Status Word 2 members are to be checked. */
  uint8                    sw1;     /**< Status Word 1. */
  uint8                    sw2;     /**< Status Word 2. */
} mmgsdi_sw_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SE13_NETWORK_INFO_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used to provide network info from GSMA SE13 table.
*/
typedef struct {
  uint16   mcc; /**< Mobile Network Code */
  uint16   mnc; /**< Mobile Country Code */
  uint32   network_type_mask; /**< Network type mask */
  char     short_name[MMGSDI_SHORT_NAME_MAX_LEN];
           /**< Null terminated string containing the network's short name.*/
  char     long_name[MMGSDI_LONG_NAME_MAX_LEN];
           /**< Null terminated string containing the network's full name. */
  boolean  gsma_entry; /**< Entry is available in GSMA SE13 table */
} mmgsdi_se13_network_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_RESPONSE_HEADER_TYPE
-----------------------------------------------------------------------------*/
/** @brief Information about the response header used to notify the client.
*/
typedef struct {
  mmgsdi_client_id_type     client_id;     /**< ID of the client to be notified. */
  mmgsdi_session_id_type    session_id;    /**< Client session ID. */
  mmgsdi_cnf_enum_type      response_type; /**< Response command type. */
  mmgsdi_len_type           response_len;  /**< Length of message plus header. */
  mmgsdi_len_type           payload_len;
       /**< Length of the message payload. This includes the size of the
            structure without the header, as well as the size of any pointer
            data being allocated.\n
            @note1 The additional data pointer being allocated is not a
            continuous memory space. */
  mmgsdi_slot_id_enum_type  slot_id;       /**< Slot on which the request is to
                                                be performed. */
  mmgsdi_client_data_type   client_data;   /**< Client data that was passed in
                                                earlier in the request. */
  mmgsdi_return_enum_type   mmgsdi_status; /**< Return status of the request. */
  mmgsdi_sw_type            status_word;   /**< Status word. */
} mmgsdi_response_header_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_TEMP_UNLOCK_INFO_TYPE
-----------------------------------------------------------------------------*/
/** @brief Information about the Temporary Unlock
           event information Type.
*/
typedef struct {
  mmgsdi_slot_id_enum_type        slot_id; /**< Slot ID */
  mmgsdi_perso_feature_enum_type feature; /**< Personalization feature */
} mmgsdi_perso_temp_unlock_evt_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_PROFILE_INFO_TYPE
-----------------------------------------------------------------------------*/
/** @brief Information about the Profile Type.
*/
typedef struct {
  mmgsdi_profile_id_enum_type   profile_id;   /**< Profile ID used */
  mmgsdi_profile_type_enum_type profile_type; /**< Type of profile */
} mmgsdi_profile_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CARD_ERR_EVT_INFO_TYPE
-----------------------------------------------------------------------------*/
/** @brief Information about a Card Error event.
*/
typedef struct {
  mmgsdi_slot_id_enum_type       slot; /**< Slot in which the card is inserted. */
  mmgsdi_card_err_info_enum_type info; /**< Specific card error condition. */
} mmgsdi_card_err_evt_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CARD_REMOVED_EVT_INFO_TYPE
-----------------------------------------------------------------------------*/
/** @brief Information about a Card Removed event.
*/
typedef struct {
  mmgsdi_slot_id_enum_type slot; /**< Slot in which the card is inserted. */
} mmgsdi_card_removed_evt_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CARD_INSERTED_EVT_INFO_TYPE
-----------------------------------------------------------------------------*/
/** @brief Information about a Card Inserted event.
*/
typedef struct {
  mmgsdi_slot_id_enum_type      slot;           /**< Slot in which the card is
                                                     inserted. */
  uint32                        num_aids_avail; /**< Applicable only if
                                                avail_apps has usim_app in it. */
  mmgsdi_aid_type               aid_info[MMGSDI_MAX_APP_INFO];
                                                /**< Applicable only if
                                                avail_apps has usim_app in it. */
  uint8                         iccid_len;
                                                /**< Length of ICCID of
                                                the card*/
  uint8                         iccid[MMGSDI_ICCID_LEN];
                                                /**< ICCID of the card */
  boolean                       is_euicc;
} mmgsdi_card_inserted_evt_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SIM_BUSY_EVT_INFO_TYPE
-----------------------------------------------------------------------------*/
/** @brief Information about a SIM busy event.
*/
typedef struct {
  mmgsdi_slot_id_enum_type  slot;      /**< Slot event refers to */
  boolean                   sim_busy;  /**< True  = SIM is busy,
                                            False = Normal operation resumed */
} mmgsdi_sim_busy_evt_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_ILLEGAL_SUBSCRIPTION_EVT_INFO_TYPE
-----------------------------------------------------------------------------*/
/** @brief Information about an Illegal Subscription event.
*/
typedef struct {
  mmgsdi_session_id_type   session_id;  /**< Session ID returned to indicate the
                                             application that was made illegal. */
} mmgsdi_session_illegal_subscription_evt_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_PROFILE_DATA_EVT_INFO_TYPE
-----------------------------------------------------------------------------*/
/** @brief Information about profile data event.
*/
typedef struct {
  mmgsdi_slot_id_enum_type   slot;    /**< Slot in which the card is inserted */
  mmgsdi_profile_info_type   profile; /**< Profile data */
} mmgsdi_profile_data_evt_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_PROFILE_SWITCHING_EVT_INFO_TYPE
-----------------------------------------------------------------------------*/
/** @brief Information about profile switching event.
*/
typedef struct {
  mmgsdi_slot_id_enum_type   slot;    /**< Slot in which the profile switch is
                                           in progress */
  mmgsdi_profile_info_type   profile; /**< Profile info after the profile switch */
} mmgsdi_profile_switch_evt_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_CLOSE_EVT_INFO_TYPE
-----------------------------------------------------------------------------*/
/** @brief Information about a Session Close event.
*/
typedef struct {
  mmgsdi_session_id_type                 session_id; /**< Session ID of the
                                                          closed request. */
  mmgsdi_slot_id_enum_type               slot;       /**< Slot in which the
                                                          card is inserted. */
  mmgsdi_data_type                       app_id;     /**< Application details. */
  mmgsdi_session_closed_cause_enum_type  cause;      /**< Cause for session
                                                          closure. */
} mmgsdi_session_close_evt_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_FDN_EVT_INFO_TYPE
-----------------------------------------------------------------------------*/
/** @brief Information about a Fixed Dialing Number (FDN) event.
*/
typedef struct {
  mmgsdi_slot_id_enum_type         slot;       /**< Slot in which the card
                                                    is inserted. */
  boolean                          enabled;    /**< Indicates whether the FDN
                                                    feature is enabled. */
  mmgsdi_rec_num_type              rec_num;    /**< FDN record number which is
                                                    updated  */
  mmgsdi_data_type                 rec_data;   /**< FDN record data which is
                                                    updated in the card */
  mmgsdi_fdn_evt_source_enum_type  evt_source; /**< Source of FDN so that
                                                    clients can decide whether
                                                    they need to handle the FDN
                                                    event or not */
} mmgsdi_fdn_evt_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_REFRESH_ENFORCEMENT_POLICY_TYPE

   DESCRIPTION:
     Refresh Enforcement Policy type
     is_enforcement_present               :  whether card has specified a refresh enforcement
     data                                 :  Contains the actual enforcement information as below
     force_refresh_navigating_menu        :  force immediate refresh even user is navigating through menu
     force_refresh_data_call              :  force immediate refresh even a data call is in progress
     force_refresh_voice_call             :  force immediate refresh even a voice call is in progress

-------------------------------------------------------------------------------*/
typedef struct {
  boolean      is_enforcement_present;
  struct
  {
    boolean    force_refresh_navigating_menu;
    boolean    force_refresh_data_call;
    boolean    force_refresh_voice_call;
  }data;
}mmgsdi_refresh_enforcement_policy_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_REFRESH_EVT_INFO_TYPE
-----------------------------------------------------------------------------*/
/** @brief Information about a Refresh event.
*/
typedef struct {
  mmgsdi_slot_id_enum_type                  slot;               /**< Slot in which the card is
                                                                     inserted. */
  mmgsdi_refresh_source_enum_type           source;             /**< Source of the refresh. */
  mmgsdi_refresh_stage_enum_type            stage;              /**< Stage of the refresh event. */
  mmgsdi_refresh_mode_enum_type             mode;               /**< Current refresh mode. */
  mmgsdi_aid_type                           aid;                /**< Application Identifier. */
  mmgsdi_app_info_type                      app_info;           /**< Application information. */
  mmgsdi_refresh_file_list_type             refresh_files;      /**< List of changed EF files
                                                      for the ME to read. */
  mmgsdi_refresh_mode_enum_type             orig_mode;          /**< Previous refresh mode. */
  mmgsdi_data_type                          plmn_list;          /**< List of Public Land Mobile
                                                                     Networks. */
  mmgsdi_refresh_enforcement_policy_type    enforcement_policy; /**< Refresh enforcement policy. */
} mmgsdi_refresh_evt_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_TP_DL_EVT_INFO_TYPE
-----------------------------------------------------------------------------*/
/** @brief Information about a Terminal Profile Download event.
*/
typedef struct {
  mmgsdi_slot_id_enum_type      slot;     /**< Slot in which the card is
                                               inserted. */
  mmgsdi_protocol_enum_type     protocol; /**< Type of card protocol. */
  boolean                       ok_dl;    /**< Status of download event. */
} mmgsdi_tp_dl_evt_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_PIN_EVT_INFO_TYPE
-----------------------------------------------------------------------------*/
/** @brief Information about a PIN event.
*/
typedef struct {
  mmgsdi_slot_id_enum_type      slot;      /**< Slot in which the card is
                                               inserted. */
  uint32                        num_aids;  /**< Number of Application Identifiers
                                                on the card. */
  mmgsdi_pin_info_type          pin_info;  /**< Status of the PINs returned. */
  mmgsdi_aid_type               aid_type[MMGSDI_MAX_APP_INFO];
                                           /**< Application information. */
  uint8                         dir_index; /**< Record number in the EF-DIR
                                            corresponding to the selected
                                            application. */
} mmgsdi_pin_evt_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_CHANGED_EVT_INFO_TYPE
-----------------------------------------------------------------------------*/
/** @brief Information about a Session Changed event.
*/
typedef struct {
  boolean                                 activated;  /**< Indicates if the Session ID is
                                                           associated with an application on the
                                                           card. If FALSE, access to session-
                                                           related data will probably fail. */
  mmgsdi_app_info_type                    app_info;   /**< Application information. */
  mmgsdi_session_closed_cause_enum_type   cause;      /**< Cause for session closure.
                                                           Valid when activated
                                                           member is set to FALSE */
} mmgsdi_session_changed_evt_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SUBSCRIPTION_READY_EVT_INFO_TYPE
-----------------------------------------------------------------------------*/
/** @brief Information about an Application Initialization Completed event.
*/
typedef struct {
  mmgsdi_app_info_type            app_info;          /**< Application information. */
  mmgsdi_app_act_status_enum_type activation_status; /**< Application activation status */
} mmgsdi_subscription_ready_evt_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SAP_CONNECT_EVT_INFO_TYPE
-----------------------------------------------------------------------------*/
/** @brief Information about a SAP Connect event.
*/
typedef struct {
  mmgsdi_slot_id_enum_type      slot;           /**< Slot in which the card is
                                                     inserted. */
  mmgsdi_sap_state_enum_type    connect_state;  /**< State during SAP connect. */
  mmgsdi_sap_mode_enum_type     sap_mode;       /**< SAP mode (server or client). */
  mmgsdi_sap_mode_enum_type     sap_connect_initiate_by;
                                                /**< Initiated connection:
                                                     server or client. */
} mmgsdi_sap_connect_evt_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SAP_DISCONNECT_EVT_INFO_TYPE
-----------------------------------------------------------------------------*/
/** @brief Information about a SAP Disconnect event.
*/
typedef struct {
  mmgsdi_slot_id_enum_type         slot;             /**< Slot in which the card
                                                          is inserted. */
  mmgsdi_sap_state_enum_type       disconnect_state; /**< State during SAP
                                                          connect. */
  mmgsdi_sap_mode_enum_type        sap_mode;         /**< SAP mode (server or
                                                          client).*/
  mmgsdi_sap_mode_enum_type        sap_disconnect_initiate_by;
                                                     /**< What initiated the
                                                          connection: server or
                                                          client. */
  mmgsdi_disconnect_mode_enum_type sap_disconnect_type;
                                                     /**< Type of disconnect
                                                          (immediate or graceful
                                                          shutdown). */
} mmgsdi_sap_disconnect_evt_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_EONS_PER_STACK_EVT_INFO_TYPE
------------------------------------------------------------------------------*/
/** @brief EONS event data payload containing call stack specific information
*/
typedef struct {
  boolean                                 is_valid;
         /**< Indicates data validity. */
  mmgsdi_eons_roaming_status_enum_type    roaming_status;
         /**< Roaming status: set to home if device is registered in home N/W
              or RPLMN matches with SPDI list */
  boolean                                 display_operator_info;
         /**< Flag indicating whether operator info should be displayed (3GPP) */
  boolean                                 display_spn_info;
         /**< Flag indicating whether spn should be displayed */
  mmgsdi_plmn_info_type                   operator_info;
         /**< Operator info pertaining to the stack (3GPP) */
} mmgsdi_eons_per_stack_evt_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_EONS_EVT_INFO_TYPE
------------------------------------------------------------------------------*/
/** @brief Event data structure which contains SPN and requested PLMN names.
*/
typedef struct {
  mmgsdi_eons_name_type                   spn;
         /**< Service provider name. */
  boolean                                 display_spn;
         /**< Flag indicating whether spn should be displayed, determined using
              first stack roaming status. TO BE DEPRECATED */
  mmgsdi_eons_spn_display_bit_enum_type   spn_display_bit;
         /**< Display bit value for spn */
  uint8                                   spn_lang_ind;
         /**< Language indicator of SPN (3GPP2) */
  mmgsdi_eons_rplmn_display_bit_enum_type rplmn_display_bit;
         /**< Display bit value for rplmn (3GPP) */
  mmgsdi_eons_per_stack_evt_info_type     first_stack_info;
         /**< Event info pertaining to the first call stack */
  mmgsdi_eons_per_stack_evt_info_type     sec_stack_info;
         /**< Event info pertaining to the second call stack */
} mmgsdi_eons_evt_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_EONS_OPL_PNN_CACHE_READY_EVT_INFO_TYPE
------------------------------------------------------------------------------*/
/** @brief Event data structure which contains OPL, PNN file info.

    MMGSDI sends MMGSDI_EONS_OPL_PNN_CACHE_READY_EVT after completion of
    EF-OPL, EF-PNN caching

    Event provides info about total number of records, number of valid records
    and record size in EF-OPL, EF-PNN. It also provides list of valid OPL, PNN
    record numbers.
*/
typedef struct {
  mmgsdi_record_ef_info_type            opl_info; /**< OPL Info. */
  mmgsdi_record_ef_info_type            pnn_info; /**< PNN Info. */
} mmgsdi_eons_opl_pnn_cache_ready_evt_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_ECC_DATA_TYPE
-----------------------------------------------------------------------------*/
/** @brief ECC data type for MMGSDI_ECC_EVT
*/
typedef struct {
  mmgsdi_rec_num_type         rec_num;          /**< Record number (valid for
                                                     linear fixed file only) */
  mmgsdi_data_type            data;             /**< ECC record/file data */
} mmgsdi_ecc_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_ECC_EVT_INFO_TYPE
-----------------------------------------------------------------------------*/
/** @brief Payload of the MMGSDI_ECC_EVT
*/
typedef struct {
  mmgsdi_slot_id_enum_type          slot;       /**< Slot in which the card is
                                                     inserted. */
  mmgsdi_file_enum_type             file_enum;  /**< ECC file name */
  uint8                             ecc_count;  /**< Number of ECC fields. */
  mmgsdi_ecc_data_type             *ecc_data;   /**< Valid data in EF-ECC. */
} mmgsdi_ecc_evt_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_PERSO_EVT_INFO_TYPE
-----------------------------------------------------------------------------*/
/** @brief Information about a Personalization event.
*/
typedef struct {
  mmgsdi_perso_feature_enum_type  feature;  /**< One of five ways to personalize
                                                 an ME for GSM/UMTS or one of
                                                 six ways for CDMA. */
  mmgsdi_perso_status_enum_type   status;   /**< Status of the personalization
                                                 event. */
  uint32                          num_retries; /**< Number of personalization
                                                    retries. */
  uint32                          num_unblock_retries;
                                               /**< Number of unblock retries
                                                    allowed. */
} mmgsdi_perso_evt_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_FILE_UPDATE_EVT_INFO_TYPE
-----------------------------------------------------------------------------*/
/** @brief Payload of the MMGSDI_FILE_UPDATE_EVT
*/
typedef struct {
  mmgsdi_access_type                file;        /**< File written to. */
  mmgsdi_file_structure_enum_type   file_type;   /**< Indicates which file_index type
                                                   to use. */

  /** Used by the mmgsdi_file_update_evt_info_type structure to
      provide additional file update info. */
  union {
    uint8                           rec_num;     /**< Record number accessed for
                                                     linear fixed or cyclic
                                                     file. */
    uint16                          offset;      /**< Offset accessed for
                                                     transparent file. */
  } file_index;
  mmgsdi_data_type                  file_data;   /**< Data written to the card. */
} mmgsdi_file_update_evt_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_LEGAL_SUBSCRIPTION_EVT_INFO_TYPE
-----------------------------------------------------------------------------*/
/** @brief Information about an subscription applicaiton marked as legal
*/
typedef struct {
  mmgsdi_app_info_type            app_info;      /**< Application information. */
} mmgsdi_session_legal_subscription_evt_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_TEST_SIM_EVT_INFO_TYPE
-----------------------------------------------------------------------------*/
/** @brief Information about a Test SIM event.
*/
typedef struct {
  mmgsdi_slot_id_enum_type      slot;                 /**< Slot in which the card is
                                                           inserted. */
  uint8                         test_sim;             /**< Test SIM bitmask to indicate
                                                           if it is a test SIM */
} mmgsdi_test_sim_evt_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_EVENT_DATA_TYPE
-----------------------------------------------------------------------------*/
/** @brief Contains the event to be notified.
*/
typedef struct {
  mmgsdi_events_enum_type  evt;        /**< Name of the event. */
  mmgsdi_client_id_type    client_id;  /**< ID of the client receiving the event. */
  mmgsdi_session_id_type   session_id; /**< Session ID of the event. */

  /** Used by the mmgsdi_event_data_type structure to provide the corresponding
      information for the event. */
  union {
    mmgsdi_pin_evt_info_type                            pin;
           /**< PIN event. */
    mmgsdi_tp_dl_evt_info_type                          terminal_profile;
           /**< Terminal profile delete event. */
    mmgsdi_refresh_evt_info_type                        refresh;
           /**< Refresh event. */
    mmgsdi_fdn_evt_info_type                            fdn;
           /**< Random event. */
    mmgsdi_card_inserted_evt_info_type                  card_inserted;
           /**< Card inserted event. */
    mmgsdi_session_changed_evt_info_type                session_changed;
           /**< Session changed event. */
    mmgsdi_subscription_ready_evt_info_type             subscription_ready;
           /**< Subscription ready event. */
    mmgsdi_card_err_evt_info_type                       card_error;
           /**< Card error event. */
    mmgsdi_sim_busy_evt_info_type                       sim_busy;
           /**< Sim busy event. */
    mmgsdi_card_removed_evt_info_type                   card_removed;
           /**< Card removed event. */
    mmgsdi_sap_connect_evt_info_type                    sap_connect;
           /**< SAP connect event. */
    mmgsdi_sap_disconnect_evt_info_type                 sap_disconnect;
           /**< SAP disconnect event. */
    mmgsdi_session_close_evt_info_type                  session_close;
          /**< Session close event. */
    mmgsdi_perso_evt_info_type                          perso;
          /**< Personification event. */
    mmgsdi_session_illegal_subscription_evt_info_type   illegal_session;
          /**< Illegal session event. */
    mmgsdi_eons_evt_info_type                           eons;
          /**< EONS event. */
    mmgsdi_file_update_evt_info_type                    file_update;
          /**< File Update event. */
    mmgsdi_eons_opl_pnn_cache_ready_evt_info_type       eons_opl_pnn_cache_ready;
          /**< EONS opl pnn cache ready event. */
    mmgsdi_ecc_evt_info_type                            ecc;
          /**< ECC data read event. */
    mmgsdi_session_legal_subscription_evt_info_type     legal_session;
          /**< Subscription marked as legal event. */
    mmgsdi_test_sim_evt_info_type                       test_sim;
          /**< Test SIM status. */
    mmgsdi_perso_temp_unlock_evt_info_type              perso_temp_unlock;
          /**< Perso temporary unlock event. */
    mmgsdi_profile_data_evt_info_type                   profile_data;
          /**< Profile data event. */
    mmgsdi_profile_switch_evt_info_type                 profile_switch;
          /**< Profile switching successful event */
  } data;
} mmgsdi_event_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:    MMGSDI_FILE_UPDATE_LIST_TYPE
-----------------------------------------------------------------------------*/
/** A list of files for which the client is to receive notification on update.
Files can be specified by enumeration or by path or using both the methods.
num_files represent the total number of files registered, irrespective of the
method specified to register.
*/
typedef struct {
   uint8                   num_files;       /**< Number of files */
   mmgsdi_access_type     *file_list_ptr;   /**< File list */
}mmgsdi_file_update_list_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:    MMGSDI_UPDATE_ACSGL_ENUM_TYPE
-----------------------------------------------------------------------------*/
/** Identifiers used for ACSGL update type
*/
typedef enum{
   MMGSDI_UPDATE_DELETE_CSG_ID,
   MMGSDI_UPDATE_ADD_CSG_ID
}mmgsdi_update_acsgl_enum_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:    MMGSDI_PLMN_CSG_ID_TYPE
-----------------------------------------------------------------------------*/
/** Structure contains uint8 array of length MMGSDI_PLMN_ID_SIZE to
    accommodate plmn identity and csg identity
*/
typedef struct{
  uint8               plmn_id_val[MMGSDI_PLMN_ID_SIZE];  /**< value of plmn identity. */
  mmgsdi_csg_id_type  csg_id;                            /**< value of CSG identity. */
} mmgsdi_plmn_csg_id_type;

/** @} */ /* end_addtogroup common_library */
/** @addtogroup common_library
@{ */

/* ----------------------------------------------------------------------------
   DATA TYPE:      MMGSDI_OFFSET_TYPE
-----------------------------------------------------------------------------*/
/** Offset that allows a client to receive data from any part of the data up to
    the end of the data.

    This offset is applicable to transparent files only.
*/
typedef int32 mmgsdi_offset_type;

/* ----------------------------------------------------------------------------
   DATA TYPE:      MMGSDI_FILE_SIZE_TYPE
-----------------------------------------------------------------------------*/
/** Indication of the file size.
*/
typedef int32 mmgsdi_file_size_type;

/* ----------------------------------------------------------------------------
   DATA TYPE:      MMGSDI_FILE_ID_TYPE
-----------------------------------------------------------------------------*/
/** Two-byte file ID value from the card.
*/
typedef uint8  mmgsdi_file_id_type[MMGSDI_FILE_ID_SIZE];

/* ----------------------------------------------------------------------------
   DATA TYPE:      MMGSDI_CAVE_RAND_TYPE_TYPE
-----------------------------------------------------------------------------*/
/** Random challenge type for a Run CAVE command.
*/
typedef uint8 mmgsdi_cave_rand_type_type;

/* ----------------------------------------------------------------------------
   DATA TYPE:      MMGSDI_CAVE_RAND_CHAL_TYPE
-----------------------------------------------------------------------------*/
/** Random challenge for a Run CAVE command.
*/
typedef dword mmgsdi_cave_rand_chal_type;

/* ----------------------------------------------------------------------------
   DATA TYPE:      MMGSDI_CAVE_DIG_LEN_TYPE
-----------------------------------------------------------------------------*/
/** Length of digits for a Run CAVE command.
*/
typedef uint8 mmgsdi_cave_dig_len_type;

/* ----------------------------------------------------------------------------
   DATA TYPE:      MMGSDI_CAVE_DIGITS_TYPE
-----------------------------------------------------------------------------*/
/** Actual digits provided for a Run CAVE command.
*/
typedef uint8 mmgsdi_cave_digits_type[MMGSDI_CAVE_DIGITS_SIZE];

/* ----------------------------------------------------------------------------
   DATA TYPE:      MMGSDI_CAVE_PROCSS_CONTROL_TYPE
-----------------------------------------------------------------------------*/
/** Process control for a Run CAVE command.
*/
typedef uint8 mmgsdi_cave_process_control_type;

/* ----------------------------------------------------------------------------
   DATA TYPE:      MMGSDI_CAVE_ESN_TYPE
-----------------------------------------------------------------------------*/
/** Electronic Serial Number for a Run CAVE command.

    The type QWORD is a two-element array, and each element is four bytes.
*/
typedef qword mmgsdi_cave_esn_type;

/* ----------------------------------------------------------------------------
   DATA TYPE:      MMGSDI_CAVE_AUTH_RESP_TYPE
-----------------------------------------------------------------------------*/
/** Authorization response for a Run CAVE command.
*/
typedef dword mmgsdi_cave_auth_resp_type;

/* ----------------------------------------------------------------------------
   DATA TYPE:      MMGSDI_VPM_OCTET_TYPE
-----------------------------------------------------------------------------*/
/** Octet of Voice Privacy Mask data for a Generate Key/VPM request.
*/
typedef uint8 mmgsdi_single_vpm_octet_type;

/* ----------------------------------------------------------------------------
   DATA TYPE:      MMGSDI_KEY_TYPE
-----------------------------------------------------------------------------*/
/** Key response for a Generate Key/VPM command.
*/
typedef uint8 mmgsdi_key_type[MMGSDI_KEY_SIZE];

/* ----------------------------------------------------------------------------
   DATA TYPE:      MMGSDI_DATA_TYPEs
-----------------------------------------------------------------------------*/
/** @name MMGSDI Data Types
    Data to be read from or written to the card.
@{ */
typedef mmgsdi_data_type mmgsdi_write_data_type; /**< Data written to the card. */
typedef mmgsdi_data_type mmgsdi_read_data_type;  /**< Data read from the card. */
typedef mmgsdi_data_type mmgsdi_increase_data_type;  /**< Incremented data from
                                                     the card. */
typedef mmgsdi_data_type mmgsdi_send_apdu_data_type; /**< Application Protocol
                                                      data unit data sent to the
                                                      card. */
typedef mmgsdi_data_type mmgsdi_sap_get_atr_data_type; /**< Answer-To-Reset
                                                      information in the SAP
                                                      context for the card. */
typedef mmgsdi_data_type mmgsdi_sap_send_apdu_data_type; /**< Application
                                                      Protocol data unit to send
                                                      to the card. */
typedef mmgsdi_data_type mmgsdi_sap_card_reader_status_data_type; /**< Card
                                                      reader status data in the
                                                      SAP context. */
typedef mmgsdi_data_type mmgsdi_status_data_type;    /**< Card status data. */
typedef mmgsdi_data_type mmgsdi_onchip_sim_config_data_type; /**< MMGSDI cache
                                                      data for on-chip simulation
                                                      in TLV format. */
typedef mmgsdi_data_type mmgsdi_cmpt_ip_response_data_type; /**< Data required
                                                      for computing the IP
                                                      authentication. */
typedef mmgsdi_data_type mmgsdi_atr_data_type;   /**< Answer-To-Reset information
                                                      for the card. */
typedef mmgsdi_data_type mmgsdi_vpm_octets_type; /**< Data octets resulting from
                                                      a Generate Key/VPM type
                                                      command. */
typedef mmgsdi_data_type mmgsdi_seek_data_type;  /**< Record data returned from
                                                      a GSM Seek command on a
                                                      UICC card. */
typedef mmgsdi_data_type mmgsdi_search_data_type; /**< Record data returned from
                                                      a Search command on a UICC
                                                      card. */
typedef mmgsdi_data_type mmgsdi_perso_client_data_type; /**< A structure on the
                                                      card with control key
                                                      information. */
typedef mmgsdi_data_type mmgsdi_get_file_attr_data_type; /**< File attribute
                                                      data read from a card. */
/** @} */ /* end name_group Data Types */

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CDMA_AKA_AUTH_KEYS_DATA_TYPE
-----------------------------------------------------------------------------*/
/** @brief Holds CDMA AKA Auth key data information when sequential data is valid.
*/
typedef struct {
  uint8                       ck[MMGSDI_CDMA_AKA_CK_LEN]; /**< Computed cipher key. */
  uint8                       ik[MMGSDI_CDMA_AKA_IK_LEN]; /**< Computed integrity key. */
  mmgsdi_data_type            res;                        /**< Computed user response indicating
                                                            successful 3G/WLAN authentication in the
                                                            AKA security context. */
} mmgsdi_cdma_aka_auth_keys_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_FILE_SECURITY_TYPE
-----------------------------------------------------------------------------*/
/** @brief File security access information.
*/
typedef struct {
  mmgsdi_file_security_enum_type   protection_method;
    /**< Protection method:
     - MMGSDI_ALWAYS_ALLOWED -- num_protection_pin = 0 and protection_pin = NULL
     - MMGSDI_NEVER_ALLOWED -- num_protection_pin = 0 and protection_pin = NULL
     - MMGSDI_AND_ALLOWED -- More than one PIN is required
     - MMGSDI_OR_ALLOWED -- One of multiple PINs is required
     - MMGSDI_SINGLE_ALLOWED -- Only one PIN is required
     */
  int32                            num_protection_pin; /**< Number of protection
                                                            PINs required. */
  mmgsdi_pin_enum_type           * protection_pin_ptr; /**< Pointer to the PIN. */
} mmgsdi_file_security_type;

/* --------------------------------------------------------------------------
   STRUCTURE:    MMGSDI_SECURITY_ACCESS_TYPE
-----------------------------------------------------------------------------*/
/** @brief File security access information for various operations.
*/
typedef struct {
  mmgsdi_file_security_type        read;     /**< Read security requirements. */
  mmgsdi_file_security_type        write;    /**< Write security requirements. */
  mmgsdi_file_security_type        increase; /**< Increase security requirements. */
  mmgsdi_file_security_type invalidate_deactivate; /**< Invalidate or deactivate
                                                         security requirements. */
  mmgsdi_file_security_type rehabilitate_activate; /**< Rehabilitate or activate
                                                        security requirements. */
} mmgsdi_file_security_access_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_FILE_STATUS_TYPE
-----------------------------------------------------------------------------*/
/** @brief File status information.
*/
typedef struct {
  boolean                  file_invalidated;  /**< Indicates whether the file
                                                   is invalidated. */
  boolean   read_write_when_invalidated;      /**< Indicates if a read/write
                                                   operation is allowed when the
                                                   file is invalidated. */
} mmgsdi_file_status_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_LINEAR_FIXED_FILE_INFO_TYPE
-----------------------------------------------------------------------------*/
/** @brief File attributes data for a linear fixed file.
*/
typedef struct {
  mmgsdi_file_security_access_type  file_security; /**< Security access condition
                                                        for the file. */
  mmgsdi_rec_num_type               num_of_rec;    /**< Number of records in the
                                                        file. */
  mmgsdi_len_type                   rec_len;       /**< Length of each record. */
  mmgsdi_file_status_type           file_status;   /**< Status of the file. */
} mmgsdi_linear_fixed_file_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CYCLIC_FILE_INFO_TYPE
-----------------------------------------------------------------------------*/
/** @brief File attributes data for a cyclic file.
*/
typedef struct {
  mmgsdi_file_security_access_type  file_security; /**< Security access
                                                        condition for the file. */
  mmgsdi_rec_num_type               num_of_rec;    /**< Number of records in
                                                        the file. */
  mmgsdi_len_type                   rec_len;       /**< Length of each record. */
  mmgsdi_file_status_type           file_status;   /**< Status of the file. */
  boolean                           increase_allowed; /**< Increase operation is or
                                                           is not allowed. */
} mmgsdi_cyclic_file_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_TRANSPARENT_FILE_INFO_TYPE
-----------------------------------------------------------------------------*/
/** @brief File attributes data for a transparent file.
*/
typedef struct {
  mmgsdi_file_security_access_type  file_security;  /**< Security access condition
                                                         for the file. */
  mmgsdi_file_status_type           file_status;    /**< Status of the file. */
} mmgsdi_transparent_file_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_FILE_ATTRIBUTES_TYPE
-----------------------------------------------------------------------------*/
/** @brief File attributes data resulting from a Get File Attributes request.
*/
typedef struct {
  mmgsdi_file_size_type           file_size;  /**< Total size of the file (MF,
                                                   DF, or EF). */
  mmgsdi_file_id_type             file_id;    /**< ID used in the SIM/USIM/RUIM. */
  mmgsdi_file_structure_enum_type file_type;  /**< Indicates which file_info type
                                                   to use. */
  uint8                           sfi;        /**< Short File Indicator for the
                                                    file, if supported for the file.
                                                    If the file does not support SFI,
                                                    this will be set to INVALID_SFI */

  /** Used by the mmgsdi_file_attributes_type structure to provide additional
      file attributes. */
  union {
    mmgsdi_transparent_file_info_type  transparent_file;  /**< Transparent file
                                                               attributes. */
    mmgsdi_linear_fixed_file_info_type linear_fixed_file; /**< Linear fixed file
                                                               attributes. */
    mmgsdi_cyclic_file_info_type       cyclic_file;       /**< Cyclic file
                                                               attributes. */
  }file_info;
} mmgsdi_file_attributes_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CMPT_IP_SIP_CHAP_DATA
-----------------------------------------------------------------------------*/
/** @brief Data required for a Simple IP Challenge Hash Authentication Protocol
    (CHAP) calculation.
*/
typedef struct {
  uint8                 chap_id;                  /**< CHAP ID. */
  uint8                 nai_entry_index;          /**< Network Access Identifier
                                                       entry index. */
  mmgsdi_len_type       chap_challenge_length;    /**< Challenge length. */
  uint8                 chap_challenge[MMGSDI_MAX_3GPD_CHAP_CHALLENGE_LENGTH];
                                                  /**< CHAP challenge data. */
} mmgsdi_cmpt_ip_sip_chap_data;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CMPT_IP_MIP_MN_HA_DATA
-----------------------------------------------------------------------------*/
/** @brief Data required for a Mobile IP Mobile Network Home Agent calculation.
*/
typedef struct {
  uint8            nai_entry_index;           /**< Network Access Identifier
                                                   entry index. */
  mmgsdi_len_type  registration_data_length;  /**< Registration data length.
                                              This shall not be larger than
                                       MMGSDI_MAX_3GPD_MN_HA_REG_DATA_LENGTH.*/
  uint8            *registration_data_ptr;    /**< Registration data pointer. */
} mmgsdi_cmpt_ip_mip_mn_ha_data;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CMPT_IP_MIP_RRQ_DATA
-----------------------------------------------------------------------------*/
/** @brief Contains data required for a Mobile IP Registration Request
    calculation.
*/
typedef struct {
  mmgsdi_len_type  rrq_data_length; /**< Preceding RRQ length. This shall not be
                                        larger than
                                        MMGSDI_MAX_3GPD_HASH_RRQ_DATA_LENGTH. */
  uint8            *rrq_data_ptr;   /**< Preceding MIP-RRQ data pointer. */
} mmgsdi_cmpt_ip_mip_mip_rrq_data;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CMPT_IP_MIP_MN_AAA_DATA
-----------------------------------------------------------------------------*/
/** @brief Contains data required for a Mobile IP Mobile Network Authentication,
    Authorization, and Accounting calculation.
*/
typedef struct {
  uint8            nai_entry_index;  /**< Network Access Index entry index. */
  mmgsdi_len_type  challenge_length; /**< Challenge length. This shall not be
                                         larger than
                                         MMGSDI_3GPD_MAX_MIP_CHALLENGE_LENGTH.*/
  uint8            *challenge_ptr;   /**< Challenge pointer. */
} mmgsdi_cmpt_ip_mip_mn_aaa_data;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CMPT_IP_HRPD_CHAP_DATA
-----------------------------------------------------------------------------*/
/** @brief Contains data required for a High Rate Packet Data (HRPD) CHAP
    calculation.
*/
typedef struct {
  uint8            chap_id;                /**< CHAP ID. */
  mmgsdi_len_type  chap_challenge_length;  /**< CHAP challenge length. This
                                           shall not exceed
                                           MMGSDI_MAX_HRPD_CHAP_CHALLENGE_LENGTH.*/
  uint8            *chap_challenge_ptr;    /**< CHAP challenge pointer. */
} mmgsdi_cmpt_ip_hrpd_chap_data;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_COMPUTE_IP_DATA_TYPE
-----------------------------------------------------------------------------*/
/** @brief Indicates which type of compute IP authentication the user is
    requesting.
*/
typedef struct {
  mmgsdi_compute_ip_operation_enum_type operation_type; /**< Type of
                                                      authentication operation. */

  /** Used by the mmgsdi_compute_ip_data_type structure.*/
  union {
    mmgsdi_cmpt_ip_sip_chap_data      sip_chap_data;  /**< CHAP operation for
                                                           the SIP. */
    mmgsdi_cmpt_ip_mip_mn_ha_data     mn_ha_data;  /**< Mobile Network Home
                                                        Agent authenticator. */
    mmgsdi_cmpt_ip_mip_mip_rrq_data   rrq_data;    /**< Mobile IP Registration
                                                        Request hash. */
    mmgsdi_cmpt_ip_mip_mn_aaa_data    mn_aaa_data; /**< Mobile Network
                                                 Authentication, Authorization,
                                                 and Accounting authenticator. */
    mmgsdi_cmpt_ip_hrpd_chap_data     hrpd_chap_data; /**< CHAP operation for
                                                           HRPD. */
  } data;
} mmgsdi_compute_ip_data_type;

/* ----------------------------------------------------------------------------
  STRUCTURE:      MMGSDI_SEARCH_OFFSET_DATA_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used for offset parameters in a UICC enhanced search.
*/
typedef struct {
  mmgsdi_search_option_enum_type  offset_type;
   /**< Indicates a search option in a record. Contains either the absolute
        record offset position or a one-byte character value. */
  mmgsdi_len_type                 offset_data;
   /**< One byte of information. For a record offset, the range is from 0x00
        to 0xFE. For a character value, the range is from 0x00 to 0xFF. */
} mmgsdi_search_offset_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE: MMGSDI_PKCS15_LOOKUP_TABLE_ENTRIES
-----------------------------------------------------------------------------*/
/** @brief EF enumeration and file path information for Public Key Cryptography
    Standards #15 (PKCS15) lookup table entries.
*/
typedef struct {
  mmgsdi_file_enum_type      pkcs15_file_enum; /**< List of PKCS15 EFs whose
                                                    paths are extracted from the
                                                    EF Object Directory File.*/
  mmgsdi_path_type           pkcs15_file_path; /**< File path information of
                                                    the PKCS15 EF. */
} mmgsdi_pkcs15_lookup_table_entries;

/* ----------------------------------------------------------------------------
   STRUCTURE: MMGSDI_PKCS15_LOOKUP_TABLE
-----------------------------------------------------------------------------*/
/** @brief EF enumeration and file path information for the PKCS15 lookup table.

    The categories are defined as follows:
    - EF enumeration -- List of PKCS15 EFs whose paths are extracted from the EF
      Object Directory File (EF-ODF).
    - File path information -- File path information of the PKCS15 EF.
*/
typedef struct {
  uint8                               no_of_pkcs15_lookup_table_entries;
       /**< Number of entries in the table. */
  mmgsdi_pkcs15_lookup_table_entries  pkcs15_lookup_table [MMGSDI_MAX_PKCS15_TABLE_ENTRIES];
       /**< Enumeration and file path information for PKCS15 entries. */
} mmgsdi_pkcs15_lookup_table;

/* ----------------------------------------------------------------------------
   STRUCTURE:    MMGSDI_CPHS_SERVICE_TABLE_TYPE
-----------------------------------------------------------------------------*/
/** @brief Common PCN Handset Specification (CPHS) service table information as
    it pertains to the EF-CPHSINFO file.
*/
typedef struct {
  uint8                             phase; /**< Common PCN Handset Specification
                                                phase. */
  mmgsdi_current_service_enum_type  csp;   /**< Customer Service Profile. */
  mmgsdi_current_service_enum_type  sst;   /**< Phase 1 String Service Table. */
  mmgsdi_current_service_enum_type  mbn;   /**< Mailbox Numbers. */
  mmgsdi_current_service_enum_type  ops;   /**< Operator name short form. */
  mmgsdi_current_service_enum_type  ifn;   /**< Information Numbers. */
}mmgsdi_cphs_service_table_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:    MMGSDI_CPHS_CUST_SERVICE_PROFILE_TYPE
-----------------------------------------------------------------------------*/
/** @brief CPHS customer service profile information. For more information, see
    the Common PCN Handset Specification (Version 4.2), Tdoc T3-00 0450.
*/
typedef struct {
  mmgsdi_csp_enum_type call_offering_CFU;   /**< Call Forward Unconditional. */
  mmgsdi_csp_enum_type call_offering_CFB;   /**< Call Forward Busy (call on
                                                 subscriber). */
  mmgsdi_csp_enum_type call_offering_CFNRy; /**< Connection Frame Number. */
  mmgsdi_csp_enum_type call_offering_CFNRc; /**< Connection Frame Number. */
  mmgsdi_csp_enum_type call_offering_CT;    /**< Call Transfer. */
  mmgsdi_csp_enum_type call_restrict_BOAC;      /**< Barring of All Outgoing
                                                     Calls. */
  mmgsdi_csp_enum_type call_restrict_BOIC;      /**< Barring of Outgoing
                                                     International Calls. */
  mmgsdi_csp_enum_type call_restrict_BOIC_exHC; /**< Barring of Outgoing
                                                     International Calls except
                                                     those directed to the Home
                                                     Public Land Mobile Networks
                                                     country. */
  mmgsdi_csp_enum_type call_restrict_BAIC;      /**< Barring of All Incoming
                                                     Calls when Roaming Outside
                                                     the Home PLMN country. */
  mmgsdi_csp_enum_type call_restrict_BIC_Roam;  /**< Blind Interference
                                                     Cancellation. */
  mmgsdi_csp_enum_type other_supps_MPTY;        /**< Multi-Party Service. */
  mmgsdi_csp_enum_type other_supps_CUG;         /**< Closed User Group. */
  mmgsdi_csp_enum_type other_supps_AoC;         /**< Advice of Charge. */
  mmgsdi_csp_enum_type other_supps_Pref_CUG;    /**< Preferential CUG. */
  mmgsdi_csp_enum_type other_supps_CUG_OA;      /**< CUG Outgoing Access. */
  mmgsdi_csp_enum_type call_completion_HOLD;    /**< Call Hold. */
  mmgsdi_csp_enum_type call_completion_CW;      /**< Call Waiting. */
  mmgsdi_csp_enum_type call_completion_CCBS;    /**< Completion of Call to Busy
                                                     Subscriber. */
  mmgsdi_csp_enum_type call_completion_User_Signalling; /**< Restriction of menus
                                                        allowing use of user-to-user
                                                        signalling. */
  mmgsdi_csp_enum_type teleservices_SM_MT;      /**< Short Message-Mobile
                                                     Terminated. */
  mmgsdi_csp_enum_type teleservices_SM_MO;      /**< Short Message-Mobile
                                                     Originated. */
  mmgsdi_csp_enum_type teleservices_SM_CB;      /**< Short Message-Cell
                                                     Broadcast. */
  mmgsdi_csp_enum_type teleservices_Reply_Path; /**< Restricts menu options for
                                                     the ability to set reply path
                                                     active on outgoing Short
                                                     Messages. */
  mmgsdi_csp_enum_type teleservices_Del_Conf;   /**< SMS Delivery Confirmation. */
  mmgsdi_csp_enum_type teleservices_Protocol_ID; /**< Restriction of menus for
                                                      SMS Protocol ID options. */
  mmgsdi_csp_enum_type teleservices_Validity_Period; /**< Restriction of menus
                                                     for SMS Validity period
                                                     options. */
  mmgsdi_csp_enum_type cphs_teleservices_als; /**< Alternate Line Service. */
  mmgsdi_csp_enum_type cphs_features_SST;     /**< CPHS features (SIM Service
                                                   Table in Phase 1). */
  mmgsdi_csp_enum_type number_identify_CLIP;  /**< Calling Line Identification
                                                   Presentation. */
  mmgsdi_csp_enum_type number_identify_CoLR;  /**< Connected Line Identification
                                                   Restriction. */
  mmgsdi_csp_enum_type number_identify_CoLP;  /**< Connected Line Identification
                                                   Presentation. */
  mmgsdi_csp_enum_type number_identify_MCI;   /**< Malicious Call Indicator. */
  mmgsdi_csp_enum_type number_identify_CLI_send; /**< CLI per call mode (default
                                                  block CLI) menu to send CLI. */
  mmgsdi_csp_enum_type number_identify_CLI_block; /**< CLI per call mode (default
                                                  send CLI) menu to block CLI. */
  mmgsdi_csp_enum_type p2plus_services_GPRS;    /**< Menus concerned with GPRS
                                                     functionality. */
  mmgsdi_csp_enum_type p2plus_services_HSCSD; /**< Menus concerned with High
                                              Speed Circuit Switched Data
                                              functionality. */
  mmgsdi_csp_enum_type p2plus_services_Voice_Group_call;
                                         /**< ASCI Voice Group Call menus. */
  mmgsdi_csp_enum_type p2plus_services_Voice_Broadcast_Svc;
                                         /**< ASCI Voice Broadcast Service menus. */
  mmgsdi_csp_enum_type p2plus_services_Muli_Sub_Profile;
                                         /**< Phase 2+ multiple subscriber
                                              profile menus. */
  mmgsdi_csp_enum_type p2plus_services_Multiband;
                                         /**< Restrict menus that allow a user
                                              to select a particular GSM
                                              900/1800 or 1900 band. */
  mmgsdi_csp_enum_type value_added_PLMN_MODE; /**< Restrict menu options that
                                              allow for manual Public Land Mobile
                                              Network selection. */
  mmgsdi_csp_enum_type value_added_VPS;  /**< Restrict menu options that allow
                                              for Voice Mail or other similar
                                              menus. */
  mmgsdi_csp_enum_type value_added_SM_MO_PAGING; /**< Restrict menu options that
                                                 allow the ability to send short
                                                 messages with type Paging. */
  mmgsdi_csp_enum_type value_added_SM_MO_EMAIL; /**< Restrict menu options that
                                                allow the ability to send short
                                                messages with type Email. */
  mmgsdi_csp_enum_type value_added_FAX;       /**< Restrict menu options that
                                                   allow fax calls. */
  mmgsdi_csp_enum_type value_added_DATA;      /**< Restrict menu options that
                                                   allow data calls. */
  mmgsdi_csp_enum_type valued_added_Language; /**< Restrict menus that allow
                                                   the user to change language. */
  mmgsdi_csp_enum_type information_numbers;   /**< ME shall only present
                                              information numbers to the user
                                              if this field has 0xFF data. */
}mmgsdi_cphs_cust_service_profile_type;

/* ----------------------------------------------------------------------------
   STRUCTURE: MMGSDI_CPHS_INFO_TYPE
-------------------------------------------------------------------------------*/
/** @brief CPHS capability information, CPHS services status, and CPHS customer
    service profile status.
*/
typedef struct {
  boolean                               cphs_capable; /**< CPHS capability info. */
  mmgsdi_cphs_service_table_type        cphs_service; /**< CPHS service status. */
  mmgsdi_cphs_cust_service_profile_type cphs_csp;     /**< Customer Service
                                                           Profile status. */
} mmgsdi_cphs_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_PIN_OPERATION_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when the PIN Operation status for verify, disable, enable,
    change, and unblock operations is returned.
*/
typedef struct {
  mmgsdi_response_header_type    response_header; /**< Response data and status
                                                  of the corresponding PIN
                                                  request. */
  mmgsdi_pin_info_type           pin_info;   /**< ID of the PIN on which the
                                                  operation is being performed. */
  mmgsdi_pin_operation_enum_type pin_op;     /**< State of the PIN. */
} mmgsdi_pin_operation_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_GET_ALL_PIN_STATUS_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when the status of all PINs is returned.
*/
typedef struct {
  mmgsdi_response_header_type  response_header; /**< Response data and status of
                                                     the request. */
  mmgsdi_pin_info_type         pin1;          /**< Status of PIN1. */
  mmgsdi_pin_info_type         pin2;          /**< Status of PIN2. */
  mmgsdi_pin_info_type         universal_pin; /**< Status of the Universal PIN. */
  mmgsdi_pin_info_type         hiddenkey;     /**< Status of the Hidden Key. */
  uint8                        dir_index;     /**< Record number in the EFDIR
                                                   corresponding to the selected
                                                   application. */
} mmgsdi_get_all_pin_status_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_STATUS_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when the Card Status Confirmation type is returned.
*/
typedef struct {
  mmgsdi_response_header_type response_header; /**< Reads the response header
                                                    information. */
  mmgsdi_status_data_type     status_data;     /**< Status data. */
} mmgsdi_status_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_READ_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when a Read response is returned from the MMGSDI task.
*/
typedef struct {
  mmgsdi_response_header_type response_header;  /**< Reads the response header
                                                     information. */
  mmgsdi_access_type          access;           /**< File that is read from. */
  mmgsdi_rec_num_type         accessed_rec_num; /**< Record number accessed for
                                                     linear fixed or cyclic
                                                     files. */
  mmgsdi_offset_type          accessed_offset;  /**< Offset accessed for
                                                     transparent files. */
  mmgsdi_read_data_type       read_data;        /**< Data read from the card. */
} mmgsdi_read_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_WRITE_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when a Write response is returned from the MMGSDI task.
*/
typedef struct {
  mmgsdi_response_header_type response_header;  /**< Write response header
                                                     information. */
  mmgsdi_access_type          access;           /**< File written to. */
  mmgsdi_rec_num_type         accessed_rec_num; /**< Record number accessed for
                                                     linear fixed or cyclic
                                                     files. */
  mmgsdi_offset_type          accessed_offset;  /**< Offset accessed for
                                                     transparent files. */
  mmgsdi_len_type             written_data_len; /**< Length of the data written
                                                     to the card. */
} mmgsdi_write_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_INCREASE_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when an Increase response is returned from the MMGSDI task.
*/
typedef struct {
  mmgsdi_response_header_type response_header;  /**< Reads the response header
                                                     information. */
  mmgsdi_access_type          access;           /**< File that is
                                                     incremented. */
  mmgsdi_increase_data_type   increased_data;   /**< Value of the incremented
                                                     record from the card. */
} mmgsdi_increase_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_GET_FILE_ATTR_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when a Get File Attributes response is returned from the MMGSDI
    task.
*/
typedef struct {
  mmgsdi_response_header_type     response_header;    /**< Get File Attributes
                                                           response header
                                                           information. */
  mmgsdi_access_type              access;             /**< File for which the
                                                           request was made. */
  mmgsdi_file_attributes_type     file_attrib;        /**< Header information. */
  uint8                           no_of_file_path;    /**< Length of the file
                                                           hierarchy in the file
                                                           path (from the root
                                                           file, MF). */
  mmgsdi_path_type                file_path[MMGSDI_MAX_FILE_PATHS];
                                                      /**< Actual file path. */
  mmgsdi_get_file_attr_data_type  raw_file_attr_data; /**< Raw file attribute
                                                      data returned by the card. */
} mmgsdi_get_file_attr_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_JCDMA_GET_CARD_INFO_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when a Get Japanese CDMA (JCDMA) Card Information response is
    returned from the MMGSDI task.
*/
typedef struct {
  mmgsdi_response_header_type       response_header; /**< Get JCDMA Card
                                                          Information response
                                                          header information. */
  mmgsdi_jcdma_card_info_enum_type  card_info;      /**< Power-up UIM card
                                                         information. */
  mmgsdi_esn_me_change_enum_type    me_esn_change;  /**< Indicates if the ESN ME
                                                         has changed. */
} mmgsdi_jcdma_get_card_info_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CLIENT_ID_AND_EVT_REG_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when Client ID and Event Registration responses are
    returned from the MMGSDI task.
*/
typedef struct {
  mmgsdi_response_header_type response_header; /**< Client ID and event
                                                    registration response header
                                                    information. */
} mmgsdi_client_id_and_evt_reg_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CLIENT_ID_AND_EVT_DEREG_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used for notification if the client ID and event deregistration
    succeeded or failed.
*/
typedef struct {
  mmgsdi_response_header_type response_header; /**< Client deregistration ID
                                                    response header information. */
} mmgsdi_client_id_and_evt_dereg_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_REFRESH_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used for notification if a Refresh process is successful or not.
*/
typedef struct {
  mmgsdi_response_header_type    response_header;  /**< Header information for
                                                   the response, including the
                                                   success or failure of the
                                                   process.*/

  mmgsdi_refresh_orig_enum_type  orig_refresh_req; /**< Origination of the
                                                        refresh request. */
} mmgsdi_refresh_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_REHABILITATE_CNF_TYPE
-------------------------------------------------------------------------------*/
/** @brief Used for notification if a Rehabilitation process succeeded or failed.
*/
typedef struct {
  mmgsdi_response_header_type response_header; /**< Header information for the
                                                response, including the success
                                                or failure of the process. */
  mmgsdi_access_type          access;          /**< File requested for
                                                rehabilitation. */
} mmgsdi_rehabilitate_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_INVALIDATE_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used for notification if an Invalidation process succeeded or failed.
*/
typedef struct {
  mmgsdi_response_header_type response_header; /**< Header information for the
                                                response, including the success
                                                or failure of the process. */
  mmgsdi_access_type          access;          /**< File requested for
                                                invalidation. */
} mmgsdi_invalidate_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SAP_CONNECT_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when a SAP Connect response is returned from the MMGSDI task.
*/
typedef struct {
  mmgsdi_response_header_type response_header; /**< Header information for the
                                                response, including the success
                                                or failure of the process. */
} mmgsdi_sap_connect_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SAP_DISCONNECT_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when a SAP Disconnect response is returned from the MMGSDI task.
*/
typedef struct {
  mmgsdi_response_header_type         response_header;
       /**< Header information for the response, including the success or
            failure of the process.*/
  mmgsdi_disconnect_mode_enum_type    disconnect_mode;
       /**< Type of disconnect (immediate or graceful shutdown). */
} mmgsdi_sap_disconnect_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_GET_ATR_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when a Get ATR response is returned from the MMGSDI task.
*/
typedef struct {
  mmgsdi_response_header_type  response_header; /**< Header information for the
                                                 response, including the success
                                                 or failure of the process.*/
  mmgsdi_atr_data_type         atr_data;        /**< Answer-To-Reset data. */
} mmgsdi_get_atr_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SAP_GET_ATR_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when a SAP Get ATR response is returned from the MMGSDI task.
*/
typedef struct {
  mmgsdi_response_header_type  response_header; /**< Header information for the
                                                 response, including the success
                                                 or failure of the process.*/
  mmgsdi_sap_get_atr_data_type atr_data;        /**< Answer-To-Reset data. */
} mmgsdi_sap_get_atr_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SEND_APDU_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when an APDU response is returned from the MMGSDI task.
*/
typedef struct {
  mmgsdi_response_header_type response_header; /**< Header information for the
                                               response, including the success
                                               or failure of the process.*/
  uint32                      max_size;             /**< Maximum size of the
                                                         data. */
  int32                       channel_id;           /**< Logical channel ID. */
  uint16                      implicit_get_rsp_sw1; /**< Implicit Get Response
                                                         Status Word 1. */
  uint16                      implicit_get_rsp_sw2; /**< Implicit Get Response
                                                         Status Word 2. */
  mmgsdi_send_apdu_data_type  apdu_data;            /**< APDU data sent to the
                                                         card. */
} mmgsdi_send_apdu_cnf_type;

/** @} */ /* end_addtogroup common_library */
/** @addtogroup common_library
@{ */

/* ----------------------------------------------------------------------------
   DATA TYPE:      MMGSDI_SAP_SEND_APDU_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Structure used when a SAP APDU response is returned from the MMGSDI
    task.
*/
typedef mmgsdi_send_apdu_cnf_type mmgsdi_sap_send_apdu_cnf_type;

/** @} */ /* end_addtogroup common_library */
/** @addtogroup common_library
@{ */

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SAP_POWER_OFF_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when a SAP Power Off response is returned from the MMGSDI task.
*/
typedef struct {
  mmgsdi_response_header_type response_header;
       /**< Header information for the response, including the success or
            failure of the process. */
} mmgsdi_sap_power_off_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SAP_CARD_READER_STATUS_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when a SAP Card Reader Status response is returned from the
    MMGSDI task.
*/
typedef struct {
  mmgsdi_response_header_type             response_header;
       /**< Header information for the response, including the success or
            failure of the process.*/
  mmgsdi_sap_card_reader_status_data_type card_reader_status_data;
       /**< Card reader status data. */
} mmgsdi_sap_card_reader_status_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SAP_RESET_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when a SAP Reset response is returned from the MMGSDI task.
*/
typedef struct {
  mmgsdi_response_header_type response_header; /**< Header information for the
                                               response, including the success
                                               or failure of the process.*/
} mmgsdi_sap_reset_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SAP_POWER_ON_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when a SAP Power On response is returned from the MMGSDI task.
*/
typedef struct {
  mmgsdi_response_header_type    response_header; /**< Header information for
                                                  the response, including the
                                                  success or failure of the
                                                  process.*/
} mmgsdi_sap_power_on_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_OPEN_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when an Open Session response is returned from the MMGSDI task.
*/
typedef struct {
  mmgsdi_response_header_type response_header; /**< Client register ID response
                                                    header information. */
  mmgsdi_session_id_type      session_id;  /**< Session ID returned to indicate
                                                which session has been selected. */
  int32                       channel_id;  /**< Channel ID. */
  mmgsdi_slot_id_enum_type    slot_id;     /**< Slot ID corresponding to the
                                                session (valid only if an
                                                application is linked to the
                                                session). */
  mmgsdi_aid_type             app_info;    /**< Application details (application
                                                type, ID, and label); valid only
                                                if an application is linked to
                                                the session. */
} mmgsdi_session_open_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_OPEN_WITH_SEL_RSP_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when an Open Session with a response containing requested
    select response is returned from the MMGSDI task.
*/
typedef struct {
  mmgsdi_response_header_type response_header; /**< Client register ID response
                                                    header information. */
  mmgsdi_session_id_type      session_id;  /**< Session ID returned to indicate
                                                which session has been selected. */
  int32                       channel_id;  /**< Channel ID. */
  mmgsdi_slot_id_enum_type    slot_id;     /**< Slot ID corresponding to the
                                                session (valid only if an
                                                application is linked to the
                                                session). */
  mmgsdi_aid_type             app_info;    /**< Application details (application
                                                type, ID, and label); valid only
                                                if an application is linked to
                                                the session. */
  mmgsdi_select_rsp_enum_type select_rsp_type; /**< Select response requested */
  mmgsdi_data_type            select_rsp;      /**< Select response data */
} mmgsdi_session_open_with_sel_rsp_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_OPEN_WITH_MF_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when a Session Open with MF response is returned from the
    MMGSDI task.
*/
typedef struct {
  mmgsdi_response_header_type response_header; /**< Client register ID response
                                                    header information. */
  mmgsdi_session_id_type      session_id;  /**< Session ID returned to indicate
                                                which session has been selected. */
  int32                       channel_id;  /**< Channel ID. */
  mmgsdi_slot_id_enum_type    slot_id;     /**< Slot ID corresponding to the
                                                session */
} mmgsdi_session_open_with_mf_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_CLOSE_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when a Close Session response is returned from the MMGSDI task.
*/
typedef struct {
  mmgsdi_response_header_type response_header; /**< Client register ID response
                                                    header information. */
  mmgsdi_session_id_type      session_id;      /**< Session ID returned to
                                                    indicate which session has
                                                    been closed. */
  mmgsdi_aid_type             app_info;        /**< Application details
                                                    (application type, ID, and
                                                    label). */
} mmgsdi_session_close_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_DEACTIVATE_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when a Deactivate Session response is returned from the MMGSDI
    task.
*/
typedef struct {
  mmgsdi_response_header_type response_header; /**< Client register ID response
                                                    header information. */
  mmgsdi_client_id_type       session_id;  /**< Session ID returned to indicate
                                                which session has been
                                                deactivated. */
  mmgsdi_aid_type             app_id;      /**< Application details (application
                                                type, ID, and label). */
} mmgsdi_session_deactivate_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_APP_RESET_DEACTIVATE_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when an application reset-related Application Deactivation
    response is returned from the MMGSDI task.
*/
typedef struct {
  mmgsdi_response_header_type response_header; /**< Client register ID response
                                                    header information. */
  mmgsdi_client_id_type       session_id;  /**< Session ID returned to indicate
                                                which session has been
                                                deactivated. */
  mmgsdi_aid_type             app_id;      /**< Application details (application
                                                type, ID, and label). */
} mmgsdi_session_app_reset_deactivate_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_APP_RESET_ACTIVATE_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when an application reset-related Application Activation response
    is returned from the MMGSDI task.
*/
typedef struct {
  mmgsdi_response_header_type response_header; /**< Client register ID response
                                                    header information. */
} mmgsdi_session_app_reset_activate_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_SELECT_AID_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when a Session AID Selection response is returned from the
    MMGSDI task.
*/
typedef struct {
  mmgsdi_response_header_type response_header; /**< Session AID Selection
                                               response header information. */
  mmgsdi_client_id_type       session_id;  /**< Session ID returned to indicate
                                                which session has been selected. */
  mmgsdi_aid_type             app_id;      /**< Application details (application
                                                type, ID, and label). */
  mmgsdi_select_rsp_enum_type select_rsp_type; /**< Select response requested */
  mmgsdi_data_type            select_rsp;      /**< Select response data */
} mmgsdi_session_select_aid_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_ISIM_AUTH_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when an ISIM Authentication response is returned from the
    MMGSDI task.
*/
typedef struct {
  mmgsdi_response_header_type response_header; /**< Reads the response header
                                                    information. */
  mmgsdi_data_type            res;   /**< Computed user response indicating
                                          successful 3G authentication in the
                                          IMS AKA security context, or successful
                                          GBA operation in the GBA Bootstrapping
                                          mode security context. */
  mmgsdi_data_type            ck;   /**< Computed cipher key. */
  mmgsdi_data_type            ik;   /**< Computed integrity key. */
  mmgsdi_data_type            auts; /**< Computed Authentication Sequence Counter
                                         response indicating synchronization
                                         failure in the IMS AKA security context
                                         or in the GBA Bootstrapping mode
                                         security context. */
} mmgsdi_isim_auth_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CARD_PUP_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when a card Power Up response is returned from the MMGSDI task.
*/
typedef struct {
  mmgsdi_response_header_type       response_header; /**< Reads the response
                                                          header information. */
  mmgsdi_card_pup_options_enum_type option;    /**< Indicates how the power
                                                    up was performed. */
}mmgsdi_card_pup_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CARD_PDOWN_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when a card Power Down response is returned from the MMGSDI
    task.
*/
typedef struct {
  mmgsdi_response_header_type         response_header; /**< Reads the response
                                                            header information. */
  mmgsdi_card_pdown_options_enum_type option;     /**< Indicates how the power
                                                       down was performed. */
}mmgsdi_card_pdown_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SRV_AVAILABLE_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when the Service Available status is returned.
*/
typedef struct {
  mmgsdi_response_header_type    response_header; /**< Response data and status
                                                       of the corresponding
                                                       service. */
  boolean                        srv_available;   /**< Status of the service. */
} mmgsdi_srv_available_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_USIM_AUTH_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when a USIM Authorization response is returned.
*/
typedef struct {
  mmgsdi_response_header_type response_header; /**< Response data and status of
                                                    the corresponding service. */
  mmgsdi_data_type            auth_response;   /**< Raw data from the card. */
} mmgsdi_usim_auth_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_COMPUTE_IP_AUTH_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when data resulting from a Compute IP Authorization command is
    returned.
*/
typedef struct {
  mmgsdi_response_header_type            response_header;
         /**< Header information for the response. */
  mmgsdi_cmpt_ip_response_data_type      cmpt_ip_response_data;
         /**< Compute IP authorization response data. */
} mmgsdi_compute_ip_auth_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_RUN_CAVE_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when data resulting from a Run CAVE command is returned.
*/
typedef struct {
  mmgsdi_response_header_type            response_header;
         /**< Header information for the response. */
  mmgsdi_cave_auth_resp_type             run_cave_response_data;
         /**< Run CAVE response data. */
} mmgsdi_run_cave_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CDMA_AKA_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when data resulting from a AKA command is returned.
*/
typedef struct {
  mmgsdi_response_header_type            response_header;
         /**< Header information for the response. */
  mmgsdi_sync_fail_enum_type             sync_fail_tag;
         /**< Determines if the R-UIM detected a valid sequence */
  union{
    mmgsdi_cdma_aka_auth_keys_data_type  aka_keys;
         /**< Holds CDMA AKA Auth key data information when sequential data is valid */
    uint8                                auts[MMGSDI_CDMA_AKA_AUTS_LEN];
                                            /**< Computed Authentication Sequence Counter
                                                 response indicating synchronization
                                                 failure in the AKA security context. */
  } aka_data;
} mmgsdi_cdma_aka_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_GENERATE_KEY_VPM_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when data resulting from a Generate Key/VPM type command is
    returned.
*/
typedef struct {
  mmgsdi_response_header_type            response_header;
         /**< Header information for the response. */
  mmgsdi_key_type                        key_data;
         /**< Key response for a Generate Key/VPM command. */
  mmgsdi_vpm_octets_type                 octet_data;
         /**< Octet data for VPM. */
} mmgsdi_generate_key_vpm_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CARD_RESET_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when card reset data is returned from the MMGSDI task to the
    client.
*/
typedef struct {
  mmgsdi_response_header_type         response_header; /**< Header information
                                                            for the response. */
} mmgsdi_card_reset_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_BCAST_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when a broadcast confirmation is returned.
*/
typedef struct {
  mmgsdi_response_header_type response_header;
         /**< Response data and status of the corresponding BCAST_REQ. */
  mmgsdi_data_type            bcast_response;
         /**< Raw data from the card. */
} mmgsdi_bcast_cnf_type;

/* ----------------------------------------------------------------------------
  STRUCTURE:      MMGSDI_SEARCH_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when a Search/Seek response is returned from the MMGSDI task.
*/
typedef struct {
  mmgsdi_response_header_type response_header;      /**< Header information for
                                                         the response. */
  mmgsdi_access_type          access;               /**< File that is being
                                                         searched. */
  mmgsdi_search_data_type     searched_record_nums;
    /**< List of record numbers (0x01, 0x02, etc.) returned from the Search and
         Seek command. */
} mmgsdi_search_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_GET_ALL_AVAILABLE_APPS_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when a response is returned from an MMGSDI task on a request to
    get the information about all available applications on the cards.
*/
typedef struct {
  mmgsdi_response_header_type  response_header; /**< Header information for
                                                     the response. */
  int32                        card1_num_app;   /**< Number of available
                                                     applications on card 1. */
  int32                        card2_num_app;   /**< Number of available
                                                     applications on card 2. */
  int32                        card3_num_app;   /**< Number of available
                                                     applications on card 3. */
  mmgsdi_app_info_type         card1_app_info[MMGSDI_MAX_APP_INFO];
    /**< Array containing the information of all available applications on
         card 1. */
  mmgsdi_app_info_type         card2_app_info[MMGSDI_MAX_APP_INFO];
    /**< Array containing the information of all available applications on
         card 2. */
  mmgsdi_app_info_type         card3_app_info[MMGSDI_MAX_APP_INFO];
    /**< Array containing the information of all available applications on
         card 3. */
} mmgsdi_get_all_available_apps_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_ACT_OR_SWITCH_PROV_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when a response is returned from the MMGSDI task on a request
    to activate or change a provisioning application.
*/
typedef struct {
  mmgsdi_response_header_type  response_header;  /**< Header information for
                                                      the response. */
} mmgsdi_session_act_or_switch_prov_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_GET_INFO_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when a Get Session Information Request response is returned
    from the MMGSDI task.
*/
typedef struct {
  mmgsdi_response_header_type    response_header;  /**< Header information for
                                                        the response. */
  mmgsdi_session_type_enum_type  session_type;     /**< Session type. */
  mmgsdi_slot_id_enum_type       slot_id;          /**< Slot ID. */
  int32                          channel_id;       /**< Logical channel ID that
                                                        is assigned to the
                                                        application. */
  mmgsdi_aid_type                app_data;         /**< Information about the
                                                        associated application. */
  mmgsdi_app_state_enum_type     app_state;        /**< Application status */
} mmgsdi_session_get_info_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_RUN_GSM_ALGO_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when data resulting from a Run GSM Algorithm command is returned.
*/
typedef struct {
  mmgsdi_response_header_type  response_header; /**< Header information for the
                                                     response. */
  uint8                        sres_resp[MMGSDI_GSM_ALGO_SRES_LEN];
       /**< Signed Response calculated by a USIM. */
  uint8                        kc_resp[MMGSDI_GSM_ALGO_KC_LEN];
       /**< GSM Key Cipher. */
} mmgsdi_session_run_gsm_algo_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_ENABLE_SERVICE_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when a response is returned from the MMGSDI task on a request
    to enable the BDN/ACL service.
*/
typedef struct {
  mmgsdi_response_header_type  response_header; /**< Header information for the
                                                     response. */
  mmgsdi_service_enum_type     service_type;    /**< Type of service to be
                                                     enabled or disabled. */
  boolean                      is_status_same;  /**< Any change in service
                                                     status.  */
} mmgsdi_session_enable_service_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_DISABLE_SERVICE_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when a response is returned from the MMGSDI task on a request
    to disable the BDN/ACL service.
*/
typedef struct {
  mmgsdi_response_header_type  response_header; /**< Header information for the
                                                     response. */
  mmgsdi_service_enum_type     service_type;    /**< Type of service to be
                                                     enabled or disabled. */
  boolean                      is_status_same;  /**< Any change in service
                                                     status.  */
} mmgsdi_session_disable_service_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_ENABLE_FDN_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when a response is returned from the MMGSDI task on a request
    to enable the FDN service.
*/
typedef struct {
  mmgsdi_response_header_type  response_header; /**< Header information for the
                                                     response. */
} mmgsdi_session_enable_fdn_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_DISABLE_FDN_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when a response is returned from the MMGSDI task on a request
    to disable the FDN service.
*/
typedef struct {
  mmgsdi_response_header_type  response_header; /**< Header information for the
                                                     response. */
} mmgsdi_session_disable_fdn_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_STORE_ESN_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when a response is returned from the MMGSDI task on a request
    to store ESN information.
*/
typedef struct {
  mmgsdi_response_header_type    response_header; /**< Header information for
                                                       the response. */
} mmgsdi_session_store_esn_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_PROV_APP_INIT_COMPLETE_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when a Provisioning Application Initialization Complete Request
    response is returned from the MMGSDI task.
*/
typedef struct {
  mmgsdi_response_header_type  response_header; /**< Header information for the
                                                     response. */
} mmgsdi_session_prov_app_init_complete_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:    MMGSDI_SESSION_PERSO_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when a Personalization Confirmation response is returned from
    the MMGSDI task.
*/
typedef struct {
  mmgsdi_response_header_type    response_header; /**< Header information for
                                                       the response. */
  mmgsdi_perso_action_enum_type  perso_action;    /**< Personalization command
                                                       that is being processed. */

  /** Used by the mmgsdi_session_perso_cnf_type structure to provide
      Personalization Confirmation details. */
  union {
    mmgsdi_perso_feature_type          feature_data_cnf;
           /**< Response data for an Activate/Deactivate/Unblock/Disable
                command. */
    mmgsdi_perso_feature_ind_type      get_feature_ind_cnf;
           /**< Response data for a Get Feature Indicator command. */
    mmgsdi_perso_ota_deperso_type      ota_deperso_cnf;
           /**< Response data for an OTA Depersonalization command. */
    mmgsdi_perso_get_feature_data_type get_data_cnf;
           /**< Response data for a Get Data command. */
    mmgsdi_perso_dck_retries_type      dck_num_retries_cnf;
           /**< Response data for a DCK Retries command. */
    mmgsdi_perso_get_feature_key_type  get_key_cnf;
           /**< Response data for a Get Feature Key command. */
    mmgsdi_perso_lock_down_type        lock_down_cnf;
           /**< Response data for a lock down command. */
  } perso_cnf_data;
} mmgsdi_session_perso_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_APP_CAPABILITIES_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Information about an application's capabilities.
*/
typedef struct {
  mmgsdi_response_header_type    response_header; /**< Header information for
                                                       the response. */
  boolean                        fdn_enabled;     /**< Fixed Dialing Number is
                                                       enabled for this
                                                       application. */
  boolean                        bdn_enabled;     /**< Barred Dialing Number is
                                                       enabled for this
                                                       application. */
  boolean                        acl_enabled;     /**< Access Control List is
                                                       enabled for this
                                                       application. */
  boolean                        imsi_invalidated; /**< International Mobile
                                                        Subscriber Identity is
                                                        invalidated. */
}mmgsdi_session_app_capabilities_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_GET_CPHS_INFO_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when a Get CPHS information Request response is returned from
    the MMGSDI task.
*/
typedef struct {
  mmgsdi_response_header_type  response_header; /**< Header information for the
                                                     response. */
  mmgsdi_cphs_info_type        cphs_info;       /**< CPHS information. */
} mmgsdi_session_get_cphs_info_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_SSD_UPDATE_CNF
-----------------------------------------------------------------------------*/
/** @brief Used when a Shared Secret Data Update Request response is returned
    from the MMGSDI task.
*/
typedef struct {
  mmgsdi_response_header_type      response_header; /**< Header information for
                                                         the response. */
} mmgsdi_session_ssd_update_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_SSD_CONFIRM_CNF
-----------------------------------------------------------------------------*/
/** @brief Used when a Shared Secret Data Confirm request response is returned
    from the MMGSDI task.
*/
typedef struct {
  mmgsdi_response_header_type      response_header; /**< Header information for
                                                         the response. */
  boolean                          ssd_update_ok;   /**< Confirm the Boolean
                                                         status. */
} mmgsdi_session_ssd_confirm_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_BS_CHAL_CNF
-----------------------------------------------------------------------------*/
/** @brief Used when a Base Station Challenge Request response is returned from
    the MMGSDI task.
*/
typedef struct {
  mmgsdi_response_header_type      response_header; /**< Header information for
                                                         the response. */
  dword                            randbs;          /**< Thirty-six-bit random
                                                         number. */
} mmgsdi_session_bs_chal_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_OTASP_MS_KEY_CNF
-----------------------------------------------------------------------------*/
/** @brief Used when an OTASP MS Key Request response is returned from the MMGSDI
    task.
*/
typedef struct {
  mmgsdi_response_header_type      response_header; /**< Header information for
                                                         the response. */
  uint8                            result;          /**< Result code. */
} mmgsdi_session_otasp_ms_key_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:     MMGSDI_SESSION_OTASP_KEY_GEN_CNF
-----------------------------------------------------------------------------*/
/** @brief Used when an OTASP Key Generation Request response is returned from
    the MMGSDI task.
*/
typedef struct {
  mmgsdi_response_header_type      response_header; /**< Header information for
                                                         the response. */
  uint8                            result;          /**< Result code. */
  mmgsdi_data_type                 ms_result;       /**< Mobile station result. */
} mmgsdi_session_otasp_key_gen_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:     MMGSDI_SESSION_OTASP_SSPR_CONFIG_CNF
-----------------------------------------------------------------------------*/
/** @brief Used when an OTASP System Selection with Preferred Roaming
    Configuration Request response is returned from the MMGSDI task.
*/
typedef struct {
  mmgsdi_response_header_type      response_header; /**< Header information for
                                                         the response. */
  uint8                            block_id;        /**< Block ID. */
  uint8                            result;          /**< Result code. */
  mmgsdi_data_type                 block_data;      /**< Block data. */
} mmgsdi_session_otasp_sspr_config_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:     MMGSDI_SESSION_OTASP_COMMIT_CNF
-----------------------------------------------------------------------------*/
/** @brief Used when an OTASP Commit Request response is returned from the
    MMGSDI task.
*/
typedef struct {
  mmgsdi_response_header_type      response_header; /**< Header information for
                                                         the response. */
} mmgsdi_session_otasp_commit_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_OTASP_OTAPA_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when an OTASP/OTAPA Request response is returned from the
    MMGSDI task.
*/
typedef struct {
  mmgsdi_response_header_type response_header; /**< Header information for the
                                                    response. */
  uint8                       result;          /**< Result code. */
  uint8                       nam_lock_ind;    /**< Number Assignment Module
                                                    lock indicator. */
  dword                       rand_otapa;      /**< Thirty-two-bit random number. */
} mmgsdi_session_otasp_otapa_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_OTASP_BLOCK_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when an OTASP Block Request response is returned from the MMGSDI
    task.
*/
typedef struct {
  mmgsdi_response_header_type      response_header; /**< Header information for
                                                         the response. */
  mmgsdi_otasp_block_op_enum_type  block_op_type;   /**< Block operation type. */
  uint8                            block_id;        /**< Block ID. */
  uint8                            result;          /**< Result code. */
  word                             segment_offset;  /**< Segment offset. */
  uint8                            segment_size;    /**< Segment size. */
  mmgsdi_data_type                 block_data;      /**< Block information. */
} mmgsdi_session_otasp_block_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_READ_PRL_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when a Read Preferred Roaming List (PRL) response is returned
    from the MMGSDI task.
*/
typedef struct {
  mmgsdi_response_header_type      response_header; /**< Header information for
                                                         the response. */
  word                             prl_version;   /**< IS-683-A or IS-683-C PRL. */
  dword                            size;          /**< Size of the PRL. */
  boolean                          valid;         /**< PRL is valid or invalid. */
  byte                             *roaming_list_ptr; /**< Pointer to the PRL. */
  word                             sspr_p_rev;      /**< Revision 3 or Revision 1*/
} mmgsdi_session_read_prl_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_ONCHIP_SIM_STATE
-----------------------------------------------------------------------------*/
/** @brief Indicates the current state of the on-chip SIM.
*/
typedef struct {
  mmgsdi_onchip_sim_state_enum_type     state; /**< State of the on-chip SIM.\n
                                        - 0 = MMGSDI_ONCHIP_SIM_NOT_VALID
                                        - 1 = MMGSDI_ONCHIP_SIM_INIT_COMPLETED */
}mmgsdi_onchip_sim_state;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_ACTIVATE_ONCHIP_SIM_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when on-chip SIM data is returned.
*/
typedef struct {
  mmgsdi_response_header_type  response_header;  /**< Response data and status
                                                    of ACTIVATE ONCHIP SIM REQ. */
  mmgsdi_onchip_sim_state      onchip_sim_state; /**< State of the on-chip SIM. */
} mmgsdi_act_onchip_sim_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_GSTK_LOC_ENV_RSP_RECEIVED_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when a vpe resp is received from the card.
*/
typedef struct {
  mmgsdi_response_header_type       response_header; /**< Reads the response
                                                          header information. */
  boolean                           imsi_switched;   /**< Indicates if IMSI
                                                          switched */
}mmgsdi_gstk_loc_env_rsp_received_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_SUBSCRIPTION_OK_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when the subscription publication request is acknowledged by
    the MMGSDI task.
*/
typedef struct {
  mmgsdi_response_header_type response_header; /**< Subscription OK response
                                                    header information. */
} mmgsdi_session_subscription_ok_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_OPEN_INFO_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when an open Session response is returned from the
    MMGSDI task.
*/
typedef struct {
  mmgsdi_session_id_type         session_id;   /**< Session ID returned to
                                                 indicate which session has
                                                 been selected. */
  mmgsdi_session_type_enum_type  session_type; /**< Session type returned to
                                                 indicate which type of
                                                 session has been selected */
  int32                          channel_id;   /**< If application is active on
                                                 Session, Channel ID
                                                 corresponding to the same */
  mmgsdi_slot_id_enum_type       slot_id;      /**< Slot ID corresponding to the
                                                 session (valid only if an
                                                 application is linked to the
                                                 session). */
  mmgsdi_aid_type                app_info;     /**< Application detail (app type,
                                                 AID and label) of the
                                                 application active on the
                                                 Session */
} mmgsdi_session_open_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_OPEN_EXT_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when an open Session extension response is
    returned from the MMGSDI task.
*/
typedef struct {
  mmgsdi_response_header_type    response_header;  /**< Session Open response
                                                     Header information */
  uint8                          num_sessions;     /**< Number of sessions that
                                                     have been opened */
  mmgsdi_session_open_info_type *session_info_ptr; /**< Info on the Sessions
                                                     opened (indexed from:
                                                     0..num_sessions-1) */
} mmgsdi_session_open_ext_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:    MMGSDI_SESSION_GET_OPERATOR_NAME_CNF_TYPE
 ----------------------------------------------------------------------------*/
/** @brief Confirmation data structure which contains SPN and requested PLMN names.
*/
typedef struct{
  mmgsdi_response_header_type             response_header;
         /**< Header information for response. */
  mmgsdi_eons_name_type                   spn;
         /**< Service provider name */
  uint8                                   spn_lang_ind;
         /**< language indicator of SPN (3GPP2) */
  mmgsdi_list_plmn_name_type              plmn_list;
         /**< plmn name list.*/
  mmgsdi_eons_spn_display_bit_enum_type   spn_display_bit;
         /**< Display bit value for spn */
  mmgsdi_eons_rplmn_display_bit_enum_type rplmn_display_bit;
         /**< Display bit value for rplmn */
  mmgsdi_eons_roaming_status_enum_type    roaming_status;
         /**< Roaming status: set to home if device is registered in home N/W
              or RPLMN matches with SPDI list */
} mmgsdi_session_get_operator_name_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SEND_APDU_EXT_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when an APDU extension response is returned from the
    MMGSDI task.
*/
typedef struct {
  mmgsdi_response_header_type response_header;      /**< Header information for
                                                         the response. */
  mmgsdi_send_apdu_options_enum_type
                              apdu_option;          /**< Type of APDU response
                                                         requested */
  mmgsdi_send_apdu_data_type  apdu_data;            /**< APDU data sent to the
                                                         card. */
} mmgsdi_send_apdu_ext_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:    MMGSDI_GET_SE13_PLMN_NAMES_CNF_TYPE
 ----------------------------------------------------------------------------*/
/** @brief Confirmation data structure which contains PLMN names from SE13 table.
*/
typedef struct{
  mmgsdi_response_header_type  response_header; /**< Header information for
                                                     response. */
  mmgsdi_list_plmn_name_type   plmn_list;       /**< plmn name list.*/
} mmgsdi_get_se13_plmn_names_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_FILE_UPDATE_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used for notification if a file update registration process is
    successful or not.
*/
typedef struct {
  mmgsdi_response_header_type    response_header;  /**< Header information for
                                                   the response, including the
                                                   success or failure of the
                                                   process.*/
} mmgsdi_session_reg_file_update_cnf_type;

typedef struct {
  mmgsdi_response_header_type    response_header;  /**< Header information for
                                                   the response, including the
                                                   success or failure of the
                                                   process.*/
  mmgsdi_update_acsgl_enum_type  update_req;      /**< Type of update
                                                  requested by the client. */
  mmgsdi_plmn_csg_id_type        plmn_csg_id;     /**< CSG ID corresponding to
                                                  a PLMN ID to be updated. */
}mmgsdi_session_update_acsgl_record_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:    MMGSDI_SE13_PLMN_INFO_TYPE
-----------------------------------------------------------------------------*/
/** @brief structure consists of PLMN's short name, long name, MCC, MNC and RAT.
*/
typedef struct{
  uint16               mcc;                  /**< MCC value.*/
  uint16               mnc;                  /**< MNC value.*/
  uint32               network_mask;         /**< RAT type.*/
  mmgsdi_data_type     plmn_long_name;       /**< plmn long name.*/
  mmgsdi_data_type     plmn_short_name;      /**< plmn short name.*/
} mmgsdi_se13_plmn_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:    MMGSDI_GET_SE13_PLMN_INFO_BY_NAME_CNF_TYPE
----------------------------------------------------------------------------*/
/** @brief Confirmation data structure which contains PLMN info from SE13 table.
*/
typedef struct{
  mmgsdi_response_header_type  response_header; /**< Header information for
                                                     response. */
  uint16                       num_of_plmn_ids; /**< Number of plmn ids. */
  mmgsdi_se13_plmn_info_type * se13_plmn_info_ptr; /**< Pointer to plmn info. */
} mmgsdi_get_se13_plmn_info_by_name_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:    MMGSDI_SET_BUILTIN_PLMN_LIST_CNF_TYPE
----------------------------------------------------------------------------*/
/** @brief Used when a response is returned from the MMGSDI task on a request
    to set operator PLMN list.
*/
typedef struct{
  mmgsdi_response_header_type  response_header; /**< Header information for
                                                     response. */
} mmgsdi_set_builtin_plmn_list_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_RESTRICTED_SIM_ACCESS_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used when response for restricted SIM access is returned.
*/
typedef struct {
  mmgsdi_response_header_type response_header; /**< Header information for the
                                               response, including the success
                                               or failure of the process.*/
  mmgsdi_data_type            response_data;   /**< Response data returned by
                                                    the card.  */
} mmgsdi_restricted_sim_access_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_GET_SIM_PROFILE_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used to get the list of supported profiles.
*/
typedef struct{
  mmgsdi_response_header_type  response_header; /**< Header information 
                                                  for response */
  uint8                        num_profile;     /**< Number of profiles
                                                  in profile list */
  mmgsdi_profile_info_type     profile_list[MMGSDI_MAX_PROFILE];
                                                /**< List of SIM profiles */
  mmgsdi_profile_info_type     active_profile;
                                                /** < Current active profile */
} mmgsdi_get_sim_profile_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SET_SIM_PROFILE_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Used to set/switch to a particular profile.
*/
typedef struct{
  mmgsdi_response_header_type  response_header; /**< Header information
                                                  for response */
} mmgsdi_set_sim_profile_cnf_type;

/** @} */ /* end_addtogroup common_library */
/** @addtogroup common_library
@{ */

typedef struct {
  mmgsdi_response_header_type  response_header; /**< Manage Channel response
                                                     header information. */
  uint8                        channel_id;      /**< channel_id that is opened,
                                                     0xFF for close channel op. */
}mmgsdi_manage_channel_cnf_type;

/* ----------------------------------------------------------------------------
   UNION:    MMGSDI_CNF_TYPE
-----------------------------------------------------------------------------*/
/** @brief Confirmation data for various requests.
*/
typedef union {
  mmgsdi_response_header_type                response_header;
         /**< Response header. */
  mmgsdi_client_id_and_evt_reg_cnf_type      client_id_and_evt_reg_cnf;
         /**< Both Client ID and Event registration. */
  mmgsdi_client_id_and_evt_dereg_cnf_type    client_id_and_evt_dereg_cnf;
         /**< Both the Client ID and event deregistration. */
  mmgsdi_read_cnf_type                       read_cnf;
         /**< Read request. */
  mmgsdi_write_cnf_type                      write_cnf;
         /**< Write request. */
  mmgsdi_increase_cnf_type                   increase_cnf;
         /**< Increase request. */
  mmgsdi_get_file_attr_cnf_type              get_file_attr_cnf;
         /**< Get file attributes request. */
  mmgsdi_session_open_cnf_type               session_open_cnf;
         /**< Open session request. */
  mmgsdi_session_open_ext_cnf_type           session_open_ext_cnf;
         /**< Open Session extension request. */
  mmgsdi_session_close_cnf_type              session_close_cnf;
         /**< Close session request. */
  mmgsdi_session_deactivate_cnf_type         session_deactivate_cnf;
         /**< Deactivate session request. */
  mmgsdi_session_app_reset_deactivate_cnf_type session_app_reset_deactivate_cnf;
         /**< Application Reset-related application deactivation. */
  mmgsdi_session_app_reset_activate_cnf_type session_app_reset_activate_cnf;
         /**< Application Reset-related application activation. */
  mmgsdi_session_select_aid_cnf_type         session_select_aid_cnf;
         /**< Session Application Identifier selection. */
  mmgsdi_isim_auth_cnf_type                  isim_auth_cnf;
         /**< ISIM authentication request. */
  mmgsdi_send_apdu_cnf_type                  send_apdu_cnf;
         /**< Send APDU request. */
  mmgsdi_get_atr_cnf_type                    get_atr_cnf;
         /**< Get ATR request. */
  mmgsdi_sap_connect_cnf_type                sap_connect_cnf;
         /**< SAP connect request. */
  mmgsdi_sap_disconnect_cnf_type             sap_disconnect_cnf;
         /**< SAP disconnect request. */
  mmgsdi_sap_get_atr_cnf_type                sap_get_atr_cnf;
         /**< Get ATR in SAP context request. */
  mmgsdi_sap_send_apdu_cnf_type              sap_send_apdu_cnf;
         /**< Send APDU in SAP context request. */
  mmgsdi_sap_power_on_cnf_type               sap_power_on_cnf;
         /**< SAP power on request. */
  mmgsdi_sap_power_off_cnf_type              sap_power_off_cnf;
         /**< SAP power off request. */
  mmgsdi_sap_reset_cnf_type                  sap_reset_cnf;
         /**< SAP reset request. */
  mmgsdi_sap_card_reader_status_cnf_type     sap_card_reader_status_cnf;
         /**< SAP card reader status request. */
  mmgsdi_card_pup_cnf_type                   card_pup_cnf;
         /**< Card power up request. */
  mmgsdi_card_pdown_cnf_type                 card_pdown_cnf;
         /**< Card power down request. */
  mmgsdi_status_cnf_type                     status_cnf;
         /**< Care status request. */
  mmgsdi_refresh_cnf_type                    refresh_cnf;
         /**< SIM initialization or application reset request. */
  mmgsdi_rehabilitate_cnf_type               rehab_cnf;
         /**< Rehabilitate file security requirements. */
  mmgsdi_invalidate_cnf_type                 invalidate_cnf;
         /**< Invalidate file security requirements. */
  mmgsdi_act_onchip_sim_cnf_type             act_onchip_cnf;
         /**< Act on on-chip SIM data. */
  mmgsdi_pin_operation_cnf_type              pin_operation_cnf;
         /**< PIN operation. */
  mmgsdi_get_all_pin_status_cnf_type         get_all_pin_status_cnf;
         /**< Get the status of all PINs. */
  mmgsdi_srv_available_cnf_type              srv_available_cnf;
         /**< Server available request. */
  mmgsdi_usim_auth_cnf_type                  usim_auth_cnf;
         /**< Authenicate USIM. */
  mmgsdi_jcdma_get_card_info_cnf_type        get_jcdma_card_info_cnf;
         /**< Get Japanese CDMA card information request. */
  mmgsdi_compute_ip_auth_cnf_type            compute_ip_cnf;
         /**< Compute IP authentication request. */
  mmgsdi_run_cave_cnf_type                   run_cave_cnf;
         /**< Run CAVE algorithm request. */
  mmgsdi_cdma_aka_cnf_type                   cdma_aka_cnf;
         /**< CDMA AKA authentication request. */
  mmgsdi_generate_key_vpm_cnf_type           generate_key_vpm_cnf;
         /**< Generate Key/Voice Privacy Mask request. */
  mmgsdi_card_reset_cnf_type                 card_reset_cnf;
         /**< Card is reset. */
  mmgsdi_bcast_cnf_type                      bcast_cnf;
         /**< Broadcast is sent. */
  mmgsdi_search_cnf_type                     search_cnf;
         /**< File search is requested. */
  mmgsdi_get_all_available_apps_cnf_type     get_all_available_apps_cnf;
         /**< Get information about all available applications on a card. */
  mmgsdi_session_act_or_switch_prov_cnf_type session_activate_or_switch_provisioning_cnf;
         /**< Activate or change a provisioning application. */
  mmgsdi_session_get_info_cnf_type           session_get_info_cnf;
         /**< Get the session information. */
  mmgsdi_session_run_gsm_algo_cnf_type       session_run_gsm_algo_cnf;
         /**< Run the GSM algorithm. */
  mmgsdi_session_enable_service_cnf_type     session_enable_service_cnf;
         /**< Enable the BDN or ACL service. */
  mmgsdi_session_disable_service_cnf_type    session_disable_service_cnf;
         /**< Disable the BDN or ACL service. */
  mmgsdi_session_perso_cnf_type              session_perso_cnf;
         /**< Personalization request. */
  mmgsdi_session_store_esn_cnf_type          session_store_esn_cnf;
         /**< Store ESN information. */
  mmgsdi_session_prov_app_init_complete_cnf_type
                                             session_prov_app_init_complete_cnf;
         /**< Provisioning application initialization complete notification. */
  mmgsdi_session_app_capabilities_cnf_type   session_app_capabilities_cnf;
         /**< Request for information about an application's capabilities. */
  mmgsdi_session_get_cphs_info_cnf_type      session_get_cphs_info_cnf;
         /**< Get CPHS information. */
  mmgsdi_session_ssd_update_cnf_type         session_ssd_update_cnf;
         /**< Update the Shared Secret Data (SSD). */
  mmgsdi_session_ssd_confirm_cnf_type        session_ssd_confirm_cnf;
         /**< Confirm the SSD. */
  mmgsdi_session_bs_chal_cnf_type            session_bs_chal_cnf;
         /**< Perform a Base Station Challenge. */
  mmgsdi_session_otasp_ms_key_cnf_type       session_otasp_ms_key_cnf;
         /**< Request an OTASP MS key. */
  mmgsdi_session_otasp_key_gen_cnf_type      session_otasp_key_gen_cnf;
         /**< Generate an OTASP MS key. */
  mmgsdi_session_otasp_sspr_config_cnf_type  session_otasp_sspr_config_cnf;
         /**< Request for System Selection with Preferred Roaming. */
  mmgsdi_session_otasp_commit_cnf_type       session_otasp_commit_cnf;
         /**< OTASP commit request. */
  mmgsdi_session_otasp_otapa_cnf_type        session_otasp_otapa_cnf;
         /**< OTASP/OTAPA request. */
  mmgsdi_session_otasp_block_cnf_type        session_otasp_block_cnf;
         /**< OTASP block request. */
  mmgsdi_session_read_prl_cnf_type           session_read_prl_cnf;
         /**< Read Preferred Roaming List request. */
  mmgsdi_session_subscription_ok_cnf_type    session_subscription_ok_cnf;
         /**< Subscription OK request  */
  mmgsdi_session_open_with_sel_rsp_cnf_type  session_open_with_sel_rsp_cnf;
         /**< Session open with select response */
  mmgsdi_session_get_operator_name_cnf_type session_get_operator_name_cnf;
         /**< get operator name request */
  mmgsdi_send_apdu_ext_cnf_type              send_apdu_ext_cnf;
         /**< Send APDU extension request. */
  mmgsdi_session_open_with_mf_cnf_type       session_open_with_mf_cnf;
         /**< Session open with mf */
  mmgsdi_session_reg_file_update_cnf_type    session_reg_file_update_cnf;
         /**< File update registration request */
  mmgsdi_gstk_loc_env_rsp_received_cnf_type  gstk_loc_env_rsp_received_cnf;
         /**< VPE rsp received request */
  mmgsdi_get_se13_plmn_names_cnf_type        get_se13_plmn_names_cnf;
         /**< Get SE13 plmn names */
  mmgsdi_session_update_acsgl_record_cnf_type session_update_acsgl_record_cnf;
         /**< Update EF-ACSGL record request */
  mmgsdi_get_se13_plmn_info_by_name_cnf_type  get_se13_plmn_info_by_name_cnf;
         /**< Get SE13 plmn names */
  mmgsdi_set_builtin_plmn_list_cnf_type       set_builtin_plmn_list_cnf;
         /**< Set builtin operator plmn list */
  mmgsdi_restricted_sim_access_cnf_type       restricted_sim_access_cnf;
         /**< Restricted SIM access */
  mmgsdi_manage_channel_cnf_type              manage_channel_cnf;
         /**< Manage Channel request */
  mmgsdi_get_sim_profile_cnf_type             get_sim_profile_cnf;
         /**< Get SIM profile  */
  mmgsdi_set_sim_profile_cnf_type             set_sim_profile_cnf;
         /**< Set SIM profile  */
} mmgsdi_cnf_type;

/** @} */ /* end_addtogroup common_library */
/** @addtogroup common_library
@{ */

/* ----------------------------------------------------------------------------
   DATA TYPE:      MMGSDI_CALLBACK_TYPE
-----------------------------------------------------------------------------*/
/** Client callback function.

    The parameter to the callback function contains the results of the
    procedures performed at the client's request.
*/
typedef void (*mmgsdi_callback_type) (mmgsdi_return_enum_type status,
                                      mmgsdi_cnf_enum_type    cnf,
                                      const mmgsdi_cnf_type  *cnf_ptr);

/* ----------------------------------------------------------------------------
   DATA TYPE:      MMGSDI_EVT_CALLBACK_TYPE
-------------------------------------------------------------------------------*/
/** Event callback function registered by a client to receive MMGSDI events.
*/
typedef void mmgsdi_evt_callback_type (const mmgsdi_event_data_type *event);

/** @} */ /* end_addtogroup common_library */

#endif /* MMGSDILIB_COMMON_H */
