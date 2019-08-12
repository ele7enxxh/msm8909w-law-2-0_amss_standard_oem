#ifndef UIMDIAG_H
#define UIMDIAG_H

/*===========================================================================

===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2009 - 2014 by QUALCOMM Technologies, Inc (QTI).
All Rights Reserved. QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdiag/inc/uimdiag.h#1 $$ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who   what, where, why
--------   ---   -----------------------------------------------------------
07/01/15   bcho  Support for whitelist and blacklist codes in same category
07/10/14   tl    Re-add CREATE_PKCS15_LOOKUP_TABLE_CMD
07/10/14   tl    Remove ALWAYS OFF feature FEATURE_MMGSDI_PKCS15
06/12/14   vv    Remove FEATURE_SIMLOCK
06/10/14   tl    Introduce EF 3GPDUPPExt
03/18/14   tl    Introduce new SIM Lock feature
06/27/13   spo   Added support for files under DF TELECOM
05/24/13   bcho  Support for perso get feature data Cnf
05/06/13   sw    Changes made for secure version of memcpy
09/25/12   tl    Adding nvruim_data_req_enum_type to sessionid_type
07/13/12   vv    Renamed CSGI to CSGL
12/23/11   bcho  files added in uimdiag_file_enum_type enum
10/18/11   bcho  Adding support for mmgsdi_session_open_ext API
10/14/11   bcho  Added support for Get Operator Name API
09/23/11   nb    Support for SAP CONNECT/SAP DISCONNECT
08/05/11   adp   Adding support to trigger refresh in uimdiag
05/27/11   nb    Support for new refresh related APIs
08/22/11   av    Added support for pdown,pup,activate onchip,send apdu commands
04/26/11   yt    Replaced references to TMC heap with Modem heap
12/29/10   ms    Adding support for BT SAP APIs
12/03/10   adp   Adding support for a number of commands
09/07/10   adp   Adding Support for dck_num_retries,lock_down_enable_ltd_access,
                 seek and search commands
08/09/10   adp   Adding support for service enable, disable, get perso key,
                 is service available, perso unblock.
07/02/10   yb    Removing dependency on rex from apps side
04/15/10   adp   Added support for PACKED structs for Q6. PACKED_POST keyword
                 is added for each packed union/structure.
03/25/10   adp   Adding support for deactivate_provisioning,
                 get_app_capabilities,session_get_cphs_info,
                 isim_authenticate,mmgsdi_session_send_card_status,
                 create_pkcreate_lookup_table
03/19/10   vs    Fixed compile error
03/19/10   rk    Added support for JCDMA get card info
02/22/10   adp   Adding support for Perso Commands
02/09/10   yt    Fixed compilation error
12/27/09   ap    Adding support for Refresh Commands
12/03/09   rk    Adding support for get file attributes, read and write commands
10/19/09   adp   Adding support for pin commands
10/18/09   adp   Moving session_open, session_close, act_or_switch, get_info
                 structures to uimdiag_mmgsdi.h
09/02/09   adp   Initial Revision -Added initial mmgsdi session based commands

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "uimdiag_mmgsdi.h"
#include "uim.h"
#include "mmgsdilib.h"
#include "mmgsdisessionlib.h"
#include "diagpkt.h"
#include "modem_mem.h"
#include "stringl/stringl.h"

#include "uimdiag_simlock.h"
#include "simlock_common.h"

/*=============================================================================

                       DEFINES DECLARATIONS

=============================================================================*/

/* Define the Command Header and Length Details */
#define UIMDIAG_CMD_HEADER_CMD_CODE             0x80
#define UIMDIAG_CMD_HEADER_CMD_CODE_LEN         0x01
#define UIMDIAG_CMD_HEADER_SUBSYS_ID            0x33
#define UIMDIAG_CMD_HEADER_SUBSYS_ID_LEN        0x01
#define UIMDIAG_CMD_HEADER_SUBSYS_CMD_CODE_LEN  0X02
#define UIMDIAG_INVALID_SESSION_ID              0x00000000FFFFFFFF

#define UIMDIAG_MAX_CLIENT_REQ_TBL_SIZE         0x14
#define UIMDIAG_MAX_VARS                        10
#define DIAG_SUBSYS_UIM                         DIAG_SUBSYS_GSDI

#define UIMDIAG_GET_FN_ERR                      (-1)

#define UIMDIAG_REQ_TABLE_INDEX_FREE            0
#define UIMDIAG_REQ_TABLE_INDEX_USED            1

#define UIMDIAG_REQ_TABLE_IS_INDEX_VALID(index)                             \
   (((index < 0) || (index >= UIMDIAG_MAX_CLIENT_REQ_TBL_SIZE))? FALSE:TRUE)

/*=============================================================================

                   ENUMERATED DATA DECLARATIONS

=============================================================================*/

/*===========================================================================
   ENUM:      uimdiag_return_enum_type

   DESCRIPTION:
     Indication of return status of uimdiag
=============================================================================*/
typedef enum
{
  UIMDIAG_SUCCESS,
  UIMDIAG_ERROR,
  UIMDIAG_INCORRECT_PARAM,
  UIMDIAG_MEMORY_ERROR_HEAP_EXHAUSTED,
  UIMDIAG_OUT_OF_BOUNDS,
  UIMDIAG_MAX_ASYNC_REQ_EXCEED_ERR    = 0x64,
  UIMDIAG_MAX_RETURN_TYPE             = 0x7FFFFFFF
}uimdiag_return_enum_type;

/*===========================================================================
   ENUM:      uimdiag_subsystem_cmd_enum_type

   DESCRIPTION:
     UIM DIAG Subsystem Subcommands
     MMGSDI - 64000 - 65000
=============================================================================*/
typedef enum
{
  UIMDIAG_NONE_CMD,

     /* SIMLOCK */
  UIMDIAG_SIMLOCK_SET_POLICY_CMD = 60000,
  UIMDIAG_SIMLOCK_HCK_LOCK_CMD,
  UIMDIAG_SIMLOCK_CK_LOCK_CMD,
  UIMDIAG_SIMLOCK_UNLOCK_CMD,
  UIMDIAG_SIMLOCK_GET_STATUS_CMD,
  UIMDIAG_SIMLOCK_GET_CATEGORY_DATA_CMD,
  UIMDIAG_SIMLOCK_ADD_LOCK_CODES_HCK_CMD,
  UIMDIAG_SIMLOCK_ADD_LOCK_CODES_CK_CMD,
  UIMDIAG_SIMLOCK_GET_CATEGORY_DATA_EXT_CMD,

     /* MMGSDI */
  UIMDIAG_MMGSDI_CLIENT_ID_AND_EVT_REG_CMD = 64000,
  UIMDIAG_MMGSDI_CLIENT_ID_AND_EVT_DEREG_CMD,
  UIMDIAG_MMGSDI_SESSION_OPEN_CMD,
  UIMDIAG_MMGSDI_SESSION_CLOSE_CMD,
  UIMDIAG_MMGSDI_SESSION_ACT_OR_SW_PROV_CMD,
  UIMDIAG_MMGSDI_SESSION_GET_INFO_CMD,
  UIMDIAG_MMGSDI_SESSION_GET_ALL_PIN_STATUS_CMD,
  UIMDIAG_MMGSDI_SESSION_VERIFY_PIN_CMD,
  UIMDIAG_MMGSDI_SESSION_DISABLE_PIN_CMD,
  UIMDIAG_MMGSDI_SESSION_ENABLE_PIN_CMD,

           /* 10 */
  UIMDIAG_MMGSDI_SESSION_CHANGE_PIN_CMD,
  UIMDIAG_MMGSDI_SESSION_UNBLOCK_PIN_CMD,
  UIMDIAG_MMGSDI_SESSION_GET_FILE_ATTR_CMD,
  UIMDIAG_MMGSDI_SESSION_READ_RECORD_CMD,
  UIMDIAG_MMGSDI_SESSION_READ_TRANSPARENT_CMD,
  UIMDIAG_MMGSDI_SESSION_WRITE_RECORD_CMD,
  UIMDIAG_MMGSDI_SESSION_WRITE_TRANSPARENT_CMD,
  UIMDIAG_MMGSDI_SESSION_REG_FOR_REFRESH_CMD,
  UIMDIAG_MMGSDI_SESSION_DEREG_FOR_REFRESH_CMD,
  UIMDIAG_MMGSDI_SESSION_OK_TO_REFRESH_CMD,

            /* 20 */
  UIMDIAG_MMGSDI_SESSION_REFRESH_COMPLETE_CMD,
  UIMDIAG_MMGSDI_SESSION_PERSO_ACTIVATE_CMD,
  UIMDIAG_MMGSDI_SESSION_PERSO_DEACTIVATE_CMD,
  UIMDIAG_MMGSDI_SESSION_PERSO_GET_INDICATORS_CMD,
  UIMDIAG_MMGSDI_SESSION_PERSO_SET_DATA_CMD,
  UIMDIAG_MMGSDI_SESSION_PERSO_GET_DATA_CMD,
  UIMDIAG_MMGSDI_SESSION_DEACT_PROV_CMD,
  UIMDIAG_MMGSDI_SESSION_JCDMA_GET_CARD_INFO_CMD,
  UIMDIAG_MMGSDI_SESSION_GET_APP_CAP_CMD,
  UIMDIAG_MMGSDI_SESSION_GET_CPHS_INFO_CMD,

          /* 30 */
  UIMDIAG_MMGSDI_SESSION_ISIM_AUTH_CMD,
  UIMDIAG_MMGSDI_SESSION_SEND_CARD_STATUS_CMD,
  UIMDIAG_MMGSDI_SESSION_CREATE_PKCS15_LOOKUP_TABLE_CMD,
  UIMDIAG_MMGSDI_SESSION_PERSO_GET_KEY_CMD,
  UIMDIAG_MMGSDI_SESSION_PERSO_UNBLOCK_CMD,
  UIMDIAG_MMGSDI_SESSION_DISABLE_SERVICE_CMD,
  UIMDIAG_MMGSDI_SESSION_ENABLE_SERVICE_CMD,
  UIMDIAG_MMGSDI_SESSION_IS_SERVICE_AVAIL_CMD,
  UIMDIAG_MMGSDI_SESSION_PERSO_GET_DCK_RETRIES_CMD,
  UIMDIAG_MMGSDI_SESSION_PERSO_LOCK_DWN_ENABLE_LTD_ACCESS_CMD,

    /* 40 */
  UIMDIAG_MMGSDI_SESSION_SEEK_CMD,
  UIMDIAG_MMGSDI_SESSION_SEARCH_CMD,
  UIMDIAG_MMGSDI_SESSION_PROV_APP_INIT_COMPLETE_CMD,
  UIMDIAG_MMGSDI_SESSION_COMPUTE_IP_AUTH_CMD,
  UIMDIAG_MMGSDI_SESSION_SSD_UPDATE_CMD,
  UIMDIAG_MMGSDI_SESSION_SSD_CONFIRM_CMD,
  UIMDIAG_MMGSDI_SESSION_BS_CHAL_CMD,
  UIMDIAG_MMGSDI_SESSION_READ_PRL_CMD,
  UIMDIAG_MMGSDI_CHECK_DIAG_SUPPORT_CMD,
  UIMDIAG_MMGSDI_SESSION_GET_INFO_SYNC_CMD,

    /* 50 */
  UIMDIAG_MMGSDI_SESSION_READ_CACHE_FILE_SIZE_CMD,
  UIMDIAG_MMGSDI_SESSION_READ_CACHE_CMD,
  UIMDIAG_MMGSDI_SESSION_WRITE_CACHE_CMD,
  UIMDIAG_MMGSDI_SESSION_ILLEGAL_SUBSCRIPTION_SYNC_CMD,
  UIMDIAG_MMGSDI_SESSION_TOGGLE_UIM_PWR_CTRL_SYNC_CMD,
  UIMDIAG_MMGSDI_SESSION_GET_APP_CAPABILITIES_SYNC_CMD,
  UIMDIAG_MMGSDI_SAP_GET_ATR_CMD,
  UIMDIAG_MMGSDI_SAP_SEND_APDU_CMD,
  UIMDIAG_MMGSDI_SAP_POWER_ON_CMD,
  UIMDIAG_MMGSDI_SAP_POWER_OFF_CMD,

    /* 60 */
  UIMDIAG_MMGSDI_SAP_CARD_READER_STATUS_CMD,
  UIMDIAG_MMGSDI_SAP_RESET_CMD,
  UIMDIAG_MMGSDI_SAP_CONNECT_CMD,
  UIMDIAG_MMGSDI_SAP_DISCONNECT_CMD,
  UIMDIAG_MMGDDI_SESSION_REGISTER_FOR_REFRESH_PROCEED_VOTING_CMD,
  UIMDIAG_MMGSDI_SESSION_OK_TO_PROCEED_WITH_REFRESH_CMD,
  UIMDIAG_MMGSDI_CARD_PUP_CMD,
  UIMDIAG_MMGSDI_CARD_PDOWN_CMD,
  UIMDIAG_MMGSDI_ACTIVATE_ONCHIP_SIM_CMD,
  UIMDIAG_MMGSDI_ACTIVATE_ONCHIP_USIM_CMD,

    /* 70 */
  UIMDIAG_MMGSDI_SEND_APDU_CMD,
  UIMDIAG_MMGSDI_SESSION_GET_NVRUIM_DATA_SYNC_CMD,
  UIMDIAG_MMGSDI_GET_ALL_AVAILABLE_APPS_CMD,
  UIMDIAG_MMGSDI_GET_ALL_AVAIL_APP_SYNC_CMD,
  UIMDIAG_MMGSDI_NAA_REFRESH_CMD,
  UIMDIAG_MMGSDI_SESSION_OPEN_EXT_CMD,
  UIMDIAG_MMGSDI_SESSION_GET_OPERATOR_NAME_CMD,
  UIMDIAG_MMGSDI_IS_SERVICE_AVAILABLE_SYNC_CMD,

  UIMDIAG_MAX_CMD = 0x7FFFFFFF
}uimdiag_subsystem_cmd_enum_type;

typedef enum
{
   /* 0 */
  /* EFs at the MF level */
  UIMDIAG_ICCID      = 0x00000000,  /**< Integrated Circuit Card ID. */
  UIMDIAG_ELP,                      /**< Extended Language Preference. */
  UIMDIAG_DIR,                      /**< Electronic File Directory for a USIM. */
  UIMDIAG_ARR,                      /**< Access Rule Reference. */

  /* EFs at CDMA level */
  UIMDIAG_CDMA_CC,                  /**< Call Count. */
  UIMDIAG_CDMA_IMSI_M,              /**< MIN-based International Mobile
                                        Subscriber Identity. */
  UIMDIAG_CDMA_IMSI_T,              /**< True International Mobile Subscriber
                                        Identity. */
  UIMDIAG_CDMA_TMSI,                /**< Temporary Mobile Subscriber Identity. */
  UIMDIAG_CDMA_ANALOG_HOME_SID,     /**< Analog home Service ID. */
  UIMDIAG_CDMA_ANALOG_OP_PARAMS,    /**< Analog operational parameters. */

  /* 10 */
  UIMDIAG_CDMA_ANALOG_LOCN_AND_REGN_IND,/**< Analog location and region
                                            indicators. */
  UIMDIAG_CDMA_HOME_SID_NID,        /**< CDMA home Service ID and Network ID. */
  UIMDIAG_CDMA_ZONE_BASED_REGN_IND, /**< CDMA zone-based region indicators. */
  UIMDIAG_CDMA_SYS_REGN_IND,        /**< CDMA system/network region indicators. */
  UIMDIAG_CDMA_DIST_BASED_REGN_IND, /**< CDMA distance-based region indicators. */
  UIMDIAG_CDMA_ACCOLC,              /**< Access Overload Class. */
  UIMDIAG_CDMA_CALL_TERM_MODE_PREF, /**< Call termination mode preferences. */
  UIMDIAG_CDMA_SCI,                 /**< Suggested Slot Cycle Index. */
  UIMDIAG_CDMA_ANALOG_CHAN_PREF,    /**< Analog channel preferences. */
  UIMDIAG_CDMA_PRL,                 /**< Preferred Roaming List. */

  /* 20 */
  UIMDIAG_CDMA_RUIM_ID,             /**< Removable UIM ID. */
  UIMDIAG_CDMA_SVC_TABLE,           /**< CDMA service table. */
  UIMDIAG_CDMA_SPC,                 /**< Service Programming Code. */
  UIMDIAG_CDMA_OTAPA_SPC_ENABLE,    /**< Over-The-Air Parameter Administration
                                         Service Programming Code enable. */
  UIMDIAG_CDMA_NAM_LOCK,            /**< Number Assignment Module lock. */
  UIMDIAG_CDMA_OTASP_OTAPA_FEATURES, /**< Over-The-Air Service Provisioning/
                                         Over-The-Air Service Provisioning
                                         Over-The-Air Parameter Administration
                                         parameters. */
  UIMDIAG_CDMA_SERVICE_PREF,     /**< Service preferences. */
  UIMDIAG_CDMA_ESN_ME,           /**< ESN for the ME. */
  UIMDIAG_CDMA_RUIM_PHASE,       /**< Removable UIM phase. */
  UIMDIAG_CDMA_PREF_LANG,        /**< Preferred languages. */

  /* 30 */
  UIMDIAG_CDMA_UNASSIGNED_1,     /**< Not yet assigned. */
  UIMDIAG_CDMA_SMS,              /**< Short Message Service. */
  UIMDIAG_CDMA_SMS_PARAMS,       /**< Short Message Service Parameters. */
  UIMDIAG_CDMA_SMS_STATUS,       /**< Short Message Service Status. */
  UIMDIAG_CDMA_SUP_SVCS_FEATURE_CODE_TABLE, /**< Supplemental Services feature
                                                code table. */
  UIMDIAG_CDMA_UNASSIGNED_2,       /**< Not yet assigned. */
  UIMDIAG_CDMA_HOME_SVC_PVDR_NAME, /**< CDMA home service provider name. */
  UIMDIAG_CDMA_UIM_ID_USAGE_IND,   /**< Removable UIM ID usage indicator. */
  UIMDIAG_CDMA_ADM_DATA,           /**< CDMA administrative data. */
  UIMDIAG_CDMA_MSISDN,             /**< Mobile Station International Subsciber
                                       Directory Number. */
  /* 40 */
  UIMDIAG_CDMA_MAXIMUM_PRL,      /**< Maximum size of the Preferred Roaming List. */
  UIMDIAG_CDMA_SPC_STATUS,       /**< Service Programming Code status. */
  UIMDIAG_CDMA_ECC,              /**< Emergency Call Codes. */
  UIMDIAG_CDMA_3GPD_ME3GPDOPC,   /**< Operational Capabilities. */
  UIMDIAG_CDMA_3GPD_3GPDOPM,     /**< Operational Mode. */
  UIMDIAG_CDMA_3GPD_SIPCAP,      /**< Simple IP Capabilities. */
  UIMDIAG_CDMA_3GPD_MIPCAP,      /**< Mobile IP Capabilities. */
  UIMDIAG_CDMA_3GPD_SIPUPP,      /**< Simple IP User Profile Parameters. */
  UIMDIAG_CDMA_3GPD_MIPUPP,      /**< Mobile IP User Profile Parameters. */
  UIMDIAG_CDMA_3GPD_SIPSP,       /**< Simple IP Status Parameters. */

  /* 50 */
  UIMDIAG_CDMA_3GPD_MIPSP,       /**< Mobile IP Status Parameters. */
  UIMDIAG_CDMA_3GPD_SIPPAPSS,    /**< Simple IP Password Authentication Protocol
                                     Shared Secret parameters. */
  UIMDIAG_CDMA_UNASSIGNED_3,     /**< Reserved. */
  UIMDIAG_CDMA_UNASSIGNED_4,     /**< Reserved. */
  UIMDIAG_CDMA_PUZL,             /**< Preferred User Zone List. */
  UIMDIAG_CDMA_MAXPUZL,          /**< Maximum Preferred User Zone List. */
  UIMDIAG_CDMA_MECRP,            /**< ME-specific Configuration Request
                                     Parameters. */
  UIMDIAG_CDMA_HRPDCAP,          /**< HRPD access authorization Capability
                                     Parameters. */
  UIMDIAG_CDMA_HRPDUPP,          /**< HRPD access authorization User Profile
                                     Parameters. */
  UIMDIAG_CDMA_CSSPR,            /**< Current System Selection Preferred Roaming
                                     protocol revision. */

  /* 60 */
  UIMDIAG_CDMA_ATC,              /**< Access Terminal Class. */
  UIMDIAG_CDMA_EPRL,             /**< Extended Preferred Roaming List. */
  UIMDIAG_CDMA_BCSMS_CONFIG,     /**< Broadcast SMS configuration. */
  UIMDIAG_CDMA_BCSMS_PREF,       /**< Broadcast SMS preferences. */
  UIMDIAG_CDMA_BCSMS_TABLE,      /**< Broadcast SMS table. */
  UIMDIAG_CDMA_BCSMS_PARAMS,     /**< Broadcast SMS parameters. */
  UIMDIAG_CDMA_MMS_NOTIF,        /**< Multimedia Messaging Service notification. */
  UIMDIAG_CDMA_MMS_EXT8,         /**< Multimedia Messaging Service extension 8. */
  UIMDIAG_CDMA_MMS_ICP,          /**< Multimedia Messaging Service Issuer
                                     Connectivity Parameters. */
  UIMDIAG_CDMA_MMS_UP,           /**< Multimedia Messaging Service User
                                     Preferences. */

  /* 70 */
  UIMDIAG_CDMA_SMS_CAP,          /**< Short Message Service Capabilities. */
  UIMDIAG_CDMA_3GPD_IPV6CAP,     /**< IPv6 capabilities. */
  UIMDIAG_CDMA_3GPD_MIPFLAGS,    /**< Mobile IP flags. */
  UIMDIAG_CDMA_3GPD_TCPCONFIG,   /**< TCP configuration. */
  UIMDIAG_CDMA_3GPD_DGC,         /**< Data Generic Configuration. */
  UIMDIAG_CDMA_BROWSER_CP,       /**< Browser Connectivity Parameters. */
  UIMDIAG_CDMA_BROWSER_BM,       /**< Browser Bookmarks. */
  UIMDIAG_CDMA_3GPD_SIPUPPEXT,   /**< Simple IP User Profile Parameters
                                     extension. */
  UIMDIAG_CDMA_MMS_CONFIG,       /**< Multimedia Messaging Service configuration. */
  UIMDIAG_CDMA_JAVA_DURL,        /**< Java Download URL. */

  /* 80 */
  UIMDIAG_CDMA_3GPD_MIPUPPEXT,   /**< Mobile IP User Profile Parameters extension. */
  UIMDIAG_CDMA_BREW_DLOAD,       /**< BREW<sup>&amp;reg;</sub> download. */
  UIMDIAG_CDMA_BREW_TSID,        /**< BREW Teleservice ID. */
  UIMDIAG_CDMA_BREW_SID,         /**< BREW Subscriber ID. */
  UIMDIAG_CDMA_LBS_XCONFIG,      /**< Location-Based Services XTRA configuration. */
  UIMDIAG_CDMA_LBS_XSURL,        /**< Location-Based Services XTRA Server URLs. */
  UIMDIAG_CDMA_LBS_V2CONFIG,     /**< Location-Based Services V2 configuration. */
  UIMDIAG_CDMA_LBS_V2PDEADDR,    /**< Location-Based Services V2 Position
                                   Determination Entity Address. */
  UIMDIAG_CDMA_LBS_V2MPCADDR,    /**< Location-Based Services V2 Mobile
                                     Positioning Center Address. */
  UIMDIAG_CDMA_BREW_AEP,         /**< BREW Application Execution Policy. */

  /* 90 */
  UIMDIAG_CDMA_MODEL,                /**< Model information. */
  UIMDIAG_CDMA_RC,                   /**< Root Certificate. */
  UIMDIAG_CDMA_APP_LABELS,           /**< Applications labels. */
  UIMDIAG_CDMA_USER_AGENT_STRING,    /**< Japanese CDMA agent string. */
  UIMDIAG_CDMA_GID_REFERENCES,       /**< JCDMA Group Identifier references. */
  UIMDIAG_CDMA_ME_DOWNLOADABLE_DATA, /**< JCDMA ME downloadable data. */
  UIMDIAG_CDMA_ME_SETTING_DATA,      /**< JCDMA ME setting data. */
  UIMDIAG_CDMA_ME_USER_DATA,         /**< JCDMA user data. */
  UIMDIAG_CDMA_RESERVED2,            /**< JCDMA reserved. */
  UIMDIAG_CDMA_RESERVED1,            /**< JCDMA reserved. */

  /* 100 */
  UIMDIAG_CDMA_UIM_SVC_TABLE,  /**< JCDMA UIM service table. */
  UIMDIAG_CDMA_UIM_FEATURES,   /**< JCDMA UIM features. */
  UIMDIAG_CDMA_HRPD_HRPDUPP,   /**< HRPD User Profile Parameters. */
  UIMDIAG_CDMA_SF_EUIM_ID,     /**< Short Form Expanded UIM ID. */

  /* JCDMA EFs. Certain JCDMA EFs are listed as JCDMA EFs. */
  UIMDIAG_CDMA_UIM_VERSION,    /**< JCDMA signature and version. */
  UIMDIAG_CDMA_IMSI_STATUS,    /**< JCDMA International Mobile Subscriber
                                   Identity status. */
  UIMDIAG_CDMA_GID1,           /**< JCDMA service provider group ID. */
  UIMDIAG_CDMA_GID2,           /**< JCDMA user group ID. */

  /* 3GPD */
  UIMDIAG_CDMA_3GPD_OP_CAP,    /**< 3G packed data operational capacity. */

  /* EFs in DF GSM */
  UIMDIAG_GSM_LP,              /**< Language preference. */

  /* 110 */
  UIMDIAG_GSM_IMSI,            /**< International Mobile Subscriber Identity. */
  UIMDIAG_GSM_KC,              /**< Key Cipher. */
  UIMDIAG_GSM_PLMN,            /**< Public Land Mobile Network selector. */
  UIMDIAG_GSM_HPLMN,           /**< Home Public Land Mobile Networks search
                                   period. */
  UIMDIAG_GSM_ACM_MAX,         /**< Accumulated Call Meter maximum value. */
  UIMDIAG_GSM_SST,             /**< SIM Service Table. */
  UIMDIAG_GSM_ACM,             /**< Accumulated Call Meter. */
  UIMDIAG_GSM_GID1,            /**< Group Identifier Level 1. */
  UIMDIAG_GSM_GID2,            /**< Group Identifier Level 2. */
  UIMDIAG_GSM_SPN,             /**< Service Provider Name. */

  /* 120 */
  UIMDIAG_GSM_PUCT,            /**< Price per Unit and Currency Table. */
  UIMDIAG_GSM_CBMI,            /**< Cell Broadcast Message Identifier selection. */
  UIMDIAG_GSM_BCCH,            /**< Broadcast Control Channels. */
  UIMDIAG_GSM_ACC,             /**< Access Control Class. */
  UIMDIAG_GSM_FPLMN,           /**< Forbidden Public Land Mobile Networks. */
  UIMDIAG_GSM_LOCI,            /**< Location Information. */
  UIMDIAG_GSM_AD,              /**< Administrative Data. */
  UIMDIAG_GSM_PHASE,           /**< Phase identification. */
  UIMDIAG_GSM_VGCS,            /**< Voice Group Call Service. */
  UIMDIAG_GSM_VGCSS,           /**< Voice Group Call Service Status. */

  /* 130 */
  UIMDIAG_GSM_VBS,             /**< Voice Broadcast Service. */
  UIMDIAG_GSM_VBSS,            /**< Voice Broadcast Service Status. */
  UIMDIAG_GSM_EMLPP,           /**< Enhanced Multi-Level Preemption and Priority. */
  UIMDIAG_GSM_AAEM,            /**< Automatic Answer for the Enhanced
                                   Multi-Level Preemption and Priority service. */
  UIMDIAG_GSM_CBMID,           /**< Cell Broadcast Message ID for data download. */
  UIMDIAG_GSM_ECC,             /**< Emergency Call Codes. */
  UIMDIAG_GSM_CBMIR,           /**< Cell Broadcast Message ID Range selection. */
  UIMDIAG_GSM_DCK,             /**< Depersonalization Control Key. */
  UIMDIAG_GSM_CNL,             /**< Cooperative Network List. */
  UIMDIAG_GSM_NIA,             /**< Network's Indication of Alerting. */

  /* 140 */
  UIMDIAG_GSM_KCGPRS,          /**< Key cipher for General Packet Radio Services. */
  UIMDIAG_GSM_LOCIGPRS,        /**< Location Information for GPRS. */
  UIMDIAG_GSM_SUME,            /**< Setup Menu Elements. */
  UIMDIAG_GSM_PLMNWACT,        /**< Public Land Mobile Networks selector With
                                   Access Technology. */
  UIMDIAG_GSM_OPLMNWACT,       /**< Operator-controlled Public Land Mobile
                                   Networks selector With Access Technology. */
  UIMDIAG_GSM_HPLMNACT,        /**< Home PLMN Access Technology. */
  UIMDIAG_GSM_CPBCCH,          /**< Compact Packet Broadcast Control Channel
                                   information. */
  UIMDIAG_GSM_INVSCAN,         /**< Investigation PLMN Scan. */
  UIMDIAG_GSM_RPLMNAT,         /**< Registered PLMN last used Access Technology. */
  UIMDIAG_GSM_PNN,             /**< PLMN Network Name. */

  /* 150 */
  UIMDIAG_GSM_OPL,             /**< Operator PLMN List. */
  UIMDIAG_GSM_MBDN,            /**< Mail Box Dialing Number. */
  UIMDIAG_GSM_EXT6,            /**< Extension 6. */
  UIMDIAG_GSM_MBI,             /**< Mail Box Identifier. */
  UIMDIAG_GSM_MWIS,            /**< Message Waiting Indication Status. */
  UIMDIAG_GSM_EXT1,            /**< Extension 1. */
  UIMDIAG_GSM_SPDI,            /**< Service Provider Display Information. */

  /* EFs for CPHS */
  UIMDIAG_GSM_CFIS,            /**< CPHS Call Forwarding Indication Status. */
  UIMDIAG_GSM_VMWI,            /**< CPHS Voice Mail Waiting Indicator. */
  UIMDIAG_GSM_SVC_STR_TBL,     /**< CPHS service string table. */

  /* 160 */
  UIMDIAG_GSM_CFF,             /**< CPHS Call Forwarding Flag. */
  UIMDIAG_GSM_ONS,             /**< CPHS Operator Name String. */
  UIMDIAG_GSM_CSP,             /**< CPHS Customer Service Profile. */
  UIMDIAG_GSM_CPHSI,           /**< CPHS Information. */
  UIMDIAG_GSM_MN,              /**< CPHS Mailbox Number. */

  /* EFs at SoLSA */
  UIMDIAG_GSM_SAI,             /**< SoLSA Access Indicator. */
  UIMDIAG_GSM_SLL,             /**< SoLSA LSA List. */

  /* EFs at MExE Level */
  UIMDIAG_GSM_MEXE_ST,         /**< MExE Service Table. */
  UIMDIAG_GSM_ORPK,            /**< MExE Operator Root Public Key. */
  UIMDIAG_GSM_ARPK,            /**< MExE Administrator Root Public Key. */

  /* 170 */
  UIMDIAG_GSM_TPRPK,           /**< Third-Party Root Public Key. */

  /* image file */
  UIMDIAG_GSM_IMG,             /**< Single image file. */
  UIMDIAG_GSM_IMG1INST1,       /**< Image 1, instance 1. */
  UIMDIAG_GSM_IMG1INST2,       /**< Image 1, instance 2. */
  UIMDIAG_GSM_IMG1INST3,       /**< Image 1, instance 3. */
  UIMDIAG_GSM_IMG2INST1,       /**< Image 2, instance 1. */
  UIMDIAG_GSM_IMG2INST2,       /**< Image 2, instance 2. */
  UIMDIAG_GSM_IMG2INST3,       /**< Image 2, instance 3. */
  UIMDIAG_GSM_IMG3INST1,       /**< Image 3, instance 1. */
  UIMDIAG_GSM_IMG3INST2,       /**< Inage 3, instance 2. */

  /* 180 */
  UIMDIAG_GSM_IMG3INST3,       /**< Image 3, instance 3.*/
  UIMDIAG_IMAGE,               /**< Single image file. */

  /* EFs at the telecom DF in GSM and CDMA */
  UIMDIAG_TELECOM_ADN,         /**< Abbreviated Dialing Numbers. */
  UIMDIAG_TELECOM_FDN,         /**< Fixed Dialing Numbers. */
  UIMDIAG_TELECOM_SMS,         /**< Short Message Service. */
  UIMDIAG_TELECOM_CCP,         /**< Capability Configuration Parameters. */
  UIMDIAG_TELECOM_ECCP,        /**< Extended CCP. */
  UIMDIAG_TELECOM_MSISDN,      /**< Mobile Station International Subscriber
                                   Directory Number. */
  UIMDIAG_TELECOM_SMSP,        /**< Short Message Service Parameters. */
  UIMDIAG_TELECOM_SMSS,        /**< Short Message Service Status. */

  /* 190 */
  UIMDIAG_TELECOM_LND,         /**< Last Number Dialed. */
  UIMDIAG_TELECOM_SDN,         /**< Service Dialing Numbers. */
  UIMDIAG_TELECOM_EXT1,        /**< Extension 1. */
  UIMDIAG_TELECOM_EXT2,        /**< Extension 2. */
  UIMDIAG_TELECOM_EXT3,        /**< Extension 3. */
  UIMDIAG_TELECOM_BDN,         /**< Barred Dialing Numbers. */
  UIMDIAG_TELECOM_EXT4,        /**< Extension 4. */
  UIMDIAG_TELECOM_SMSR,        /**< Short Message Service Reports. */
  UIMDIAG_TELECOM_CMI,         /**< Comparison Method Information. */
  UIMDIAG_TELECOM_SUME,        /**< Setup Menu Elements. */

  /* 200 */
  UIMDIAG_TELECOM_ARR,         /**< Access Rule Reference. */

  /* EFs at DF GRAPHICS under DF Telecom */
  UIMDIAG_IMAGE_FILE,          /**< Image instance data file. */

  /* EFs at the DF PHONEBOOK under  DF Telecom */
  UIMDIAG_TELECOM_PBR,         /**< Phone Book Reference file. */

   /* 205 */
  UIMDIAG_TELECOM_PSC =  0x000000CD,         /**< Phone book Synchronization Center. */
  UIMDIAG_TELECOM_CC,          /**< Change Counter. */
  UIMDIAG_TELECOM_PUID,        /**< Previous Unique Identifier. */
  UIMDIAG_TELECOM_MML,         /**< Multimedia Messages List. */
  UIMDIAG_TELECOM_MMDF,        /**< Multimedia Messages Data File. */

  /* 210 */
  UIMDIAG_TELECOM_MLPL,        /**< MMSS Location Assisted Priority List. */
  UIMDIAG_TELECOM_MSPL,        /**< MMSS System Priority List. */
  UIMDIAG_TELECOM_MMSSMODE,    /**< MultiMode System Selection Mode settings. */
  /* EFs of USIM ADF */
   /* 257 */
  UIMDIAG_USIM_LI = 0x00000101,             /**< Language Indication. */
  UIMDIAG_USIM_IMSI,           /**< International Mobile Subscriber Identity. */
  UIMDIAG_USIM_KEYS,           /**< Ciphering and integrity keys. */

  /* 260 */
  UIMDIAG_USIM_KEYSPS,         /**< Ciphering and integrity keys for the Packet
                                   Switched domain. */
  UIMDIAG_USIM_PLMNWACT,       /**< User-controlled Public Land Mobile Networks
                                   selector With Access Technology. */
  UIMDIAG_USIM_UPLMNSEL,       /**< User-controlled Public Land Mobile Networks
                                   Selector. */
  UIMDIAG_USIM_HPLMN,          /**< Home PLMN search period. */
  UIMDIAG_USIM_ACM_MAX,        /**< Accumulated Call Meter maximum value. */
  UIMDIAG_USIM_UST,            /**< USIM Service Table. */
  UIMDIAG_USIM_ACM,            /**< Accumulated Call Meter. */
  UIMDIAG_USIM_GID1,           /**< Group Identifier level 1. */
  UIMDIAG_USIM_GID2,           /**< Group Identifier level 2. */
  UIMDIAG_USIM_SPN,            /**< Service Provider Name. */

  /* 270 */
  UIMDIAG_USIM_PUCT,           /**< Price per Unit and Currency Table. */
  UIMDIAG_USIM_CBMI,           /**< Cell Broadcast Message Identifier selection. */
  UIMDIAG_USIM_ACC,            /**< Access Control Class. */
  UIMDIAG_USIM_FPLMN,          /**< Forbidden PLMNs. */
  UIMDIAG_USIM_LOCI,           /**< Location Information. */
  UIMDIAG_USIM_AD,             /**< Administrative Data. */
  UIMDIAG_USIM_CBMID,          /**< Cell Broadcast Message Identifier for data
                                   Download. */
  UIMDIAG_USIM_ECC,            /**< Emergency Call Codes. */
  UIMDIAG_USIM_CBMIR,          /**< Cell Broadcast Message Identifier Range
                                   selection. */
  UIMDIAG_USIM_PSLOCI,         /**< Packet Switched Location Information. */

  /* 280 */
  UIMDIAG_USIM_FDN,            /**< Fixed Dialing Numbers. */
  UIMDIAG_USIM_SMS,            /**< Short Message Service. */
  UIMDIAG_USIM_MSISDN,         /**< Mobile Station International Subscriber
                                   Directory Number. */
  UIMDIAG_USIM_SMSP,           /**< Short Message Service Parameters. */
  UIMDIAG_USIM_SMSS,           /**< Short Message Service Status. */
  UIMDIAG_USIM_SDN,            /**< Service Dialing Numbers. */
  UIMDIAG_USIM_EXT2,           /**< Extension 2. */
  UIMDIAG_USIM_EXT3,           /**< Extension 3. */
  UIMDIAG_USIM_SMSR,           /**< Short Message Service Reports. */
  UIMDIAG_USIM_ICI,            /**< Incoming Call Information. */

  /* 290 */
  UIMDIAG_USIM_OCI,            /**< Outgoing Call Information. */
  UIMDIAG_USIM_ICT,            /**< Incoming Call Timer. */
  UIMDIAG_USIM_OCT,            /**< Outgoing Call Timer. */
  UIMDIAG_USIM_EXT5,           /**< Extension 5. */
  UIMDIAG_USIM_CCP2,           /**< Capability Configuration Parameters 2. */
  UIMDIAG_USIM_EMLPP,          /**< Enhanced Multi-Level Precedence and
                                   Preemption. */
  UIMDIAG_USIM_AAEM,           /**< Automatic Answer for EMLPP service. */
  UIMDIAG_USIM_GMSI,           /**< Group identity. */
  UIMDIAG_USIM_HIDDENKEY,      /**< Key for hidden phone book entries. */
  UIMDIAG_USIM_BDN,            /**< Barred Dialing Numbers. */

  /* 300 */
  UIMDIAG_USIM_EXT4,           /**< Extension 4. */
  UIMDIAG_USIM_CMI,            /**< Comparison Method Information. */
  UIMDIAG_USIM_EST,            /**< Enabled Services Table. */
  UIMDIAG_USIM_ACL,            /**< Access Control List. */
  UIMDIAG_USIM_DCK,            /**< Depersonalization Control Key. */
  UIMDIAG_USIM_CNL,            /**< Cooperative Network List. */
  UIMDIAG_USIM_START_HFN,      /**< Initialization values for Hyper Frame Number. */
  UIMDIAG_USIM_THRESHOLD,      /**< Maximum value of START. */
  UIMDIAG_USIM_OPLMNWACT,      /**< Operator-controlled Public Land Mobile
                                   Networks selector With Access Technology. */
  UIMDIAG_USIM_OPLMNSEL,       /**< Operator-controlled Public Land Mobile
                                   Networks selector. */

  /* 310 */
  UIMDIAG_USIM_HPLMNWACT,      /**< Home PLMN selector With Access Technology. */
  UIMDIAG_USIM_ARR,            /**< Access Rule Reference. */
  UIMDIAG_USIM_RPLMNACT,       /**< Registered PLMN last used Access Technology. */
  UIMDIAG_USIM_NETPAR,         /**< Network parameters. */
  UIMDIAG_USIM_PNN,            /**< PLMN Network Name. */
  UIMDIAG_USIM_OPL,            /**< Operator PLMN List. */
  UIMDIAG_USIM_MBDN,           /**< Mailbox Dialing Number. */
  UIMDIAG_USIM_EXT6,           /**< Extension 6. */
  UIMDIAG_USIM_MBI,            /**< Mailbox Identifier. */
  UIMDIAG_USIM_MWIS,           /**< Message Waiting Indication Status. */

  /* 320 */
  UIMDIAG_USIM_SPDI,           /**< Service Provider Display Information. */
  UIMDIAG_USIM_EHPLMN,         /**< Equivalent Home PLMN. */
  UIMDIAG_USIM_CFIS,           /**< Call Forwarding Indicator Status. */
  UIMDIAG_USIM_VGCSCA,         /**< Voice Group Call Service Ciphering Algorithm. */
  UIMDIAG_USIM_VBSCA,          /**< Voice Broadcast Service Ciphering Algorithm. */
  /* MBMS*/
  UIMDIAG_USIM_GBABP,          /**< MBMS Generic Bootstrapping Architecture
                                   Bootstrapping Parameters. */
  UIMDIAG_USIM_GBANL,          /**< MBMS GBA Network Application Function
                                   derivation List associated with the Network
                                   Application Function derivation process. */
  UIMDIAG_USIM_MSK,            /**< MBMS Service Key. */
  UIMDIAG_USIM_MUK,            /**< MBMS User Key. */
  /* CPHS */
  UIMDIAG_USIM_VMWI,           /**< CPHS Voice Mail Waiting Indicator. */

  /* 330 */
  UIMDIAG_USIM_SVC_STR_TBL,    /**< CHPS Service string table. */
  UIMDIAG_USIM_CFF,            /**< CHPS Call Forwarding Flag. */
  UIMDIAG_USIM_ONS,            /**< CHPS Operator Name String. */
  UIMDIAG_USIM_CSP,            /**< CHPS Customer Service Profile. */
  UIMDIAG_USIM_CPHSI,          /**< CHPS Information. */
  UIMDIAG_USIM_MN,             /**< CHPS Mailbox Number. */
  /*EFs at DF PHONEBOOK under USIM ADF */

   /* 386 */
  UIMDIAG_USIM_PBR = 0x00000182,            /**< Phone Book Reference file. */
  UIMDIAG_USIM_PSC,            /**< Phone book Synchronization Center. */
  UIMDIAG_USIM_CC,             /**< Change Counter. */
  UIMDIAG_USIM_PUID,           /**< Previous Unique Identifier. */

  /* 390 */
  /*EFs at DF GSM under USIM ADF */
  UIMDIAG_USIM_KC,             /**< GSM Key Cipher. */
  UIMDIAG_USIM_KCGPRS,         /**< Key Cipher for GPRS. */
  UIMDIAG_USIM_CPBCCH,         /**< Compact Packet Broadcast Control Channel
                                   information. */
  UIMDIAG_USIM_INVSCAN,        /**< Investigation scan. */
  /*EFs at DF MEXe under USIM ADF */
  UIMDIAG_USIM_MEXE_ST,        /**< Mobile Equipment Execution Environment Service
                                   Table. */
  UIMDIAG_USIM_ORPK,           /**< Operator Root Public Key. */
  UIMDIAG_USIM_ARPK,           /**< Administrator Root Public Key. */
  UIMDIAG_USIM_TPRPK,          /**< Third Party Root Public Key. */
  /* EFs for MMS Notification */
  UIMDIAG_USIM_MMSN,           /**< Multimedia Messaging Service Notification. */
  UIMDIAG_USIM_MMSICP,         /**< Multimedia Messaging Service Issuer
                                   Connectivity Parameters. */

  /* 400 */
  UIMDIAG_USIM_MMSUP,          /**< Multimedia Messaging Service User
                                   Preferences. */
  UIMDIAG_USIM_MMSUCP,         /**< Multimedia Messaging Service User
                                   Connectivity Parameters. */
  UIMDIAG_USIM_EXT8,           /**< Extension 8. */
  UIMDIAG_USIM_EPSLOCI,        /**< EPS Location Information. */
  UIMDIAG_USIM_EPSNSC,         /**< EPS NAS Security Context. */
  UIMDIAG_USIM_SPNI,           /**< Service Provider Name Icon. */
  UIMDIAG_USIM_PNNI,           /**< PLMN Network Name Icon. */
  UIMDIAG_USIM_NCP_IP,         /**< Connectivity parameters for USIM IP
                                   connections. */
  UIMDIAG_USIM_PSEUDO,         /**< Pseudonym. */
  UIMDIAG_USIM_UPLMNWLAN,      /**< User-controlled PLMN selector for I-WLAN
                                   access. */
  /* 410 */
  UIMDIAG_USIM_0PLMNWLAN,      /**< Operator-controlled PLMN selector for I-WLAN
                                   access. */
  UIMDIAG_USIM_UWSIDL,         /**< User-controlled WLAN-Specific Identifier
                                   List. */
  UIMDIAG_USIM_OWSIDL,         /**< Operator-controlled WLAN-Specific Identifier
                                   List. */
  UIMDIAG_USIM_WRI,            /**< WLAN Reauthentication Identity. */

  UIMDIAG_USIM_HWSIDL,         /**< Home I-WLAN-Specific Identifier List. */
  UIMDIAG_USIM_WEHPLMNPI,      /**< I-WLAN Equivalent Home PLMN Presentation */
  UIMDIAG_USIM_WHPI,           /**< I-WLAN Home PLMN Priority Indication. */
  UIMDIAG_USIM_WLRPLMN,        /**< I-WLAN Last Registered PLMN. */
  UIMDIAG_USIM_HPLMNDAI,       /**< Home PLMN Direct Access Indicator. */
  UIMDIAG_USIM_ACSGL,          /**< Allowed Closed Subscriber Group Lists and
                                   corresponding indications. */

  /* 420 */
  UIMDIAG_USIM_CSGT,           /**< Closed Subscriber Group Indication. */

  UIMDIAG_USIM_HNBN,           /**< Home (e)NodeB Name. */
  UIMDIAG_USIM_EHPLMNPI,       /**< Equivalent HPLMN Presentation Indicator */
  UIMDIAG_USIM_LRPLMNSI,       /**< Last RPLMN Selection Indication */
  /* Efs for PKCS #15 support */
  UIMDIAG_PKCS15_ODF,          /**< Object Directory File. */
  UIMDIAG_PKCS15_PRKDF,        /**< Private Key Directory File. */
  UIMDIAG_PKCS15_PUKDF,        /**< Public Key Directory File. */
  UIMDIAG_PKCS15_SKDF,         /**< Secret Key Directory File. */
  UIMDIAG_PKCS15_CDF,          /**< Certificate Directory File. */
  UIMDIAG_PKCS15_DODF,         /**< Data Object Directory File. */

  /* 430 */
  UIMDIAG_PKCS15_AODF,         /**< Authentication Object Directory File. */
  UIMDIAG_PKCS15_TI,           /**< Token Information file. */
  UIMDIAG_PKCS15_US,           /**< Unused Space information file. */
  /* EFs of DCS1800 DF */
  UIMDIAG_DCS1800_IMSI,        /**< International Mobile Subscriber Identity. */
  UIMDIAG_DCS1800_KC,          /**< Key Cipher. */
  UIMDIAG_DCS1800_PLMN,        /**< Public Land Mobile Network selector. */
  UIMDIAG_DCS1800_SST,         /**< SIM Service Table. */
  UIMDIAG_DCS1800_BCCH,        /**< Broadcast Control Channels. */
  UIMDIAG_DCS1800_ACC,         /**< Access Control Class. */
  UIMDIAG_DCS1800_FPLMN,       /**< Forbidden PLMNs. */

  /* 440 */
  UIMDIAG_DCS1800_LOCI,        /**< Location Information. */
  UIMDIAG_DCS1800_AD,          /**< Administrative Data. */
  /* EFs of WIM DF */
  UIMDIAG_WIM_ODF,             /**< Wireless ID Module Object Directory File. */
  UIMDIAG_WIM_TOKEN_INFO,      /**< Generic token information and capabilities. */
  UIMDIAG_WIM_UNUSED_SPACE,    /**< Unused space. */
  /* EFs of ISIM Application */
  UIMDIAG_ISIM_IMPI,           /**< IP Multimedia Private user Identity. */
  UIMDIAG_ISIM_DOMAIN,         /**< Home network Domain name. */
  UIMDIAG_ISIM_IMPU,           /**< IP Multimedia Public User identity. */
  UIMDIAG_ISIM_ARR,            /**< Access Rule Reference. */
  UIMDIAG_ISIM_KEYS,           /**< Ciphering and integrity keys for IMS. */

  /* 450 */
  UIMDIAG_ISIM_AD,             /**< Administrative Data. */
  /* EFs for Orange */
  UIMDIAG_SIM_7F40_PROP1_DFS,    /**< Orange&tm;&nbsp;proprietary: Dynamic Flags
                                     Status. */
  UIMDIAG_SIM_7F40_PROP1_D2FS,   /**< Orange proprietary: Dynamic2 Flag Setting. */
  UIMDIAG_SIM_7F40_PROP1_CSP2,   /**< Orange proprietary: Customer Service
                                     Profile line 2. */
  UIMDIAG_SIM_7F40_PROP1_PARAMS, /**< Orange proprietary: EF parameters, Welcome
                                     EFs of the ORANGE DF under the USIM ADF. */
  UIMDIAG_USIM_7F40_PROP1_DFS,   /**< Orange proprietary: Dynamic Flags Status. */
  UIMDIAG_USIM_7F40_PROP1_D2FS,  /**< Orange proprietary: Dynamic2 Flag Setting. */
  UIMDIAG_USIM_7F40_PROP1_CSP2,  /**< Orange proprietary: Customer Service
                                    Profile line 2. */
  UIMDIAG_USIM_7F40_PROP1_PARAMS,    /**< Orange proprietary: EF parameters,
                                         Welcome message. */
  /* EFs under Cingular */
  UIMDIAG_SIM_7F66_PROP1_ACT_HPLMN,  /**< Cingular&reg;&nbsp; proprietary: Acting
                                         Home PLMN. */

  /* 460 */
  UIMDIAG_SIM_7F66_PROP1_SPT_TABLE,  /**< Cingular proprietary: Support table. */
  /* EFs of Cingular */
  UIMDIAG_USIM_7F66_PROP1_ACT_HPLMN, /**< Cingular proprietary: Acting Home PLMN. */
  UIMDIAG_USIM_7F66_PROP1_SPT_TABLE, /**< Cingular proprietary: Cingular TST. */
  UIMDIAG_USIM_7F66_PROP1_RAT,       /**< Cingular proprietary: Radio Access
                                         Technology. */
  /* EFs of Home Zone */
  UIMDIAG_SIM_7F43_PROP1_HZ,         /**< Home Zone. */
  UIMDIAG_SIM_7F43_PROP1_CACHE1,     /**< USIM Home Zone Information (UHZI)
                                         proprietary: Cache 1. */
  UIMDIAG_SIM_7F43_PROP1_CACHE2,     /**< UHZI proprietary: Cache 2. */
  UIMDIAG_SIM_7F43_PROP1_CACHE3,     /**< UHZI proprietary: Cache 3. */
  UIMDIAG_SIM_7F43_PROP1_CACHE4,              /**< UHZI proprietary: Cache 4. */
  UIMDIAG_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_1, /**< UHZI proprietary: LAC_CI_1. */

  /* 470 */
  UIMDIAG_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_2, /**< UHZI proprietary: LAC_CI_2. */
  UIMDIAG_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_3, /**< UHZI proprietary: LAC_CI_3. */
  UIMDIAG_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_4, /**< UHZI proprietary: LAC_CI_4. */
  UIMDIAG_SIM_7F43_PROP1_TAGS,                /**< UHZI proprietary: Tags. */
  UIMDIAG_SIM_7F43_PROP1_SETTINGS,            /**< UHZI proprietary: Settings. */
  /* EFs under MediaFLO/RAN DF */
  UIMDIAG_MFLO_RAN_RFR,          /**< Radio Access Network File Revision. */
  UIMDIAG_MFLO_RAN_RF,           /**< Radio Access Network Radio Frequency. */
  /* EFs under MediaFLO/MFAPP DF */
  UIMDIAG_MFLO_MFAPP_MFR,        /**< MediaFLO&tm;&nbsp;application Revision. */
  UIMDIAG_MFLO_MFAPP_BCSID,      /**< Billing and Customer Service provider ID. */
  UIMDIAG_MFLO_MFAPP_MFT,        /**< MediaFLO Feature Table. */

  /* 480 */
  UIMDIAG_MFLO_MFAPP_SUBTYPE,    /**< Subscriber Type. */
  UIMDIAG_MFLO_MFAPP_SUBPROF,    /**< Subscriber Profile. */
  UIMDIAG_MFLO_MFAPP_UTSD,       /**< Usage Tracking Service Data. */
  UIMDIAG_MFLO_MFAPP_UIP,        /**< UI Preference. */
  UIMDIAG_MFLO_MFAPP_PCSET,      /**< Parental Control Setting. */
  UIMDIAG_MFLO_MFAPP_MCHSET,     /**< Multi-presentation-view Channel Setting. */
  UIMDIAG_MFLO_MFAPP_RFUD,       /**< Reserved for Future User Data. */
  /* EFs in CDMA DF*/
  UIMDIAG_CSIM_ARR,            /**< Access Rule Reference. */
  UIMDIAG_CSIM_CALL_COUNT,     /**< Call count. */
  UIMDIAG_CSIM_IMSI_M,         /**< MIN-based International Mobile Subscriber
                                   Identity. */

  /* 490 */
  UIMDIAG_CSIM_IMSI_T,         /**< True International Mobile Subscriber
                                   Identity.*/
  UIMDIAG_CSIM_TMSI,           /**< Temporary Mobile Subscriber Identity. */
  UIMDIAG_CSIM_ANALOG_HOME_SID,          /**< Analog home SID. */
  UIMDIAG_CSIM_ANALOG_OP_PARAMS,         /**< Analog Operational parameters. */
  UIMDIAG_CSIM_ANALOG_LOCN_AND_REGN_IND, /**< Analog location and region
                                             indicators. */
  UIMDIAG_CSIM_CDMA_HOME_SID_NID,        /**< CDMA home SID and NID. */
  UIMDIAG_CSIM_CDMA_ZONE_BASED_REGN_IND, /**< CDMA zone-based region indicators. */
  UIMDIAG_CSIM_CDMA_SYS_REGN_IND,        /**< CDMA system/network region
                                             indicators. */
  UIMDIAG_CSIM_CDMA_DIST_BASED_REGN_IND, /**< CDMA distance-based region
                                             indicators. */
  UIMDIAG_CSIM_ACCOLC,                   /**< Access Overload Class. */

  /* 500*/
  UIMDIAG_CSIM_CALL_TERM_MODE_PREF,      /**< Call termination mode preferences. */
  UIMDIAG_CSIM_SSCI,                     /**< Suggested Slot Cycle Index. */
  UIMDIAG_CSIM_ANALOG_CHAN_PREF,     /**< Analog channel preferences. */
  UIMDIAG_CSIM_PRL,                  /**< Preferred Roaming List */
  UIMDIAG_CSIM_RUIM_ID,              /**< Removable UIM ID. */
  UIMDIAG_CSIM_CST,                  /**< CSIM Service Table. */
  UIMDIAG_CSIM_SPC,                  /**< Service Programming Code. */
  UIMDIAG_CSIM_OTAPA_SPC_ENABLE,     /**< Over-The-Air Parameter Administration
                                         Service Programming Code enable. */
  UIMDIAG_CSIM_NAM_LOCK,             /**< Number Assignment Module lock. */
  UIMDIAG_CSIM_OTASP_OTAPA_FEATURES, /**< Over-The-Air Service Provisioning/
                                         Over-The-Air Parameter Administration
                                         features. */

  /* 510 */
  UIMDIAG_CSIM_SERVICE_PREF,    /**< Service preferences. */
  UIMDIAG_CSIM_ESN_ME,          /**< ESN for the ME. */
  UIMDIAG_CSIM_UNASSIGNED_1,    /**< Reserved 1. */
  UIMDIAG_CSIM_LANG_IND,        /**< Language indication. */
  UIMDIAG_CSIM_FDN,             /**< Fixed Dialing Number. */
  UIMDIAG_CSIM_SMS,             /**< Short Message Service. */
  UIMDIAG_CSIM_SMS_PARAMS,      /**< Short Message Service Parameters. */
  UIMDIAG_CSIM_SMS_STATUS,      /**< Short Message Service Status. */
  UIMDIAG_CSIM_SSFC,            /**< Supplementary Services Feature Code table. */
  UIMDIAG_CSIM_SPN,             /**< CDMA home Service Provider Name. */

  /* 520 */
  UIMDIAG_CSIM_USGIND,          /**< Usage Indicator for the UIM ID/Short Form
                                    EUIMID. */
  UIMDIAG_CSIM_AD,              /**< CDMA Administrative Data. */
  UIMDIAG_CSIM_MSISDN,          /**< Mobile Station International Subscriber
                                    Directory Number. */
  UIMDIAG_CSIM_MAXIMUM_PRL,     /**< Maximum size of the PRL. */
  UIMDIAG_CSIM_SPC_STATUS,      /**< Service Programming Code status. */
  UIMDIAG_CSIM_ECC,             /**< Emergency Call Codes. */
  UIMDIAG_CSIM_3GPD_ME3GPDOPC,  /**< 3GPD Operational Capabilities. */
  UIMDIAG_CSIM_3GPD_3GPDOPM,    /**< 3GPD Operational Mode. */
  UIMDIAG_CSIM_3GPD_SIPCAP,     /**< Simple IP Capabilities. */
  UIMDIAG_CSIM_3GPD_MIPCAP,     /**< Mobile IP Capabilities. */

  /* 530 */
  UIMDIAG_CSIM_3GPD_SIPUPP,     /**< Simple IP User Profile Parameters. */
  UIMDIAG_CSIM_3GPD_MIPUPP,     /**< Mobile IP User Profile Parameters. */
  UIMDIAG_CSIM_3GPD_SIPSP,      /**< Simple IP Status Parameters. */
  UIMDIAG_CSIM_3GPD_MIPSP,      /**< Mobile IP Status Parameters. */
  UIMDIAG_CSIM_3GPD_SIPPAPSS,   /**< Simple IP Password Authentication Protocol
                                    Shared Secret parameters. */
  UIMDIAG_CSIM_UNASSIGNED_2,    /**< Reserved 2. */
  UIMDIAG_CSIM_UNASSIGNED_3,    /**< Reserved 3. */
  UIMDIAG_CSIM_PUZL,            /**< Preferred User Zone List. */
  UIMDIAG_CSIM_MAXPUZL,         /**< Maximum Preferred User Zone List. */
  UIMDIAG_CSIM_MECRP,           /**< ME-specific Configuration Request Parameters. */

  /* 540 */
  UIMDIAG_CSIM_HRPDCAP,         /**< HRPD access authorization Capability
                                    Parameters. */
  UIMDIAG_CSIM_HRPDUPP,         /**< HRPD access authorization User Profile
                                    Parameters. */
  UIMDIAG_CSIM_CSSPR,           /**< Current System Selection Preferred Roaming
                                    protocol revision. */
  UIMDIAG_CSIM_ATC,             /**< Access Terminal Class. */
  UIMDIAG_CSIM_EPRL,            /**< Extended Preferred Roaming List. */

  UIMDIAG_CSIM_BCSMS_CONFIG,    /**< Broadcast SMS configuration. */
  UIMDIAG_CSIM_BCSMS_PREF,      /**< Broadcast SMS preferences. */
  UIMDIAG_CSIM_BCSMS_TABLE,     /**< Broadcast SMS table. */
  UIMDIAG_CSIM_BCSMS_PARAMS,    /**< Broadcast SMS parameters. */
  UIMDIAG_CSIM_BAKPARA,         /**< Currently used BAK parameters. */

  /* 550 */
  UIMDIAG_CSIM_UPBAKPARA,       /**< Updated BAK parameters. */
  UIMDIAG_CSIM_MMSN,            /**< Multimedia Messaging Service Notification. */
  UIMDIAG_CSIM_MMS_EXT8,        /**< Multimedia Messaging Service Extension 8. */
  UIMDIAG_CSIM_MMS_ICP,         /**< Multimedia Messaging Service Issuer
                                    Connectivity Parameters. */
  UIMDIAG_CSIM_MMS_UP,          /**< Multimedia Messaging Service User
                                    Preferences. */
  UIMDIAG_CSIM_MMS_UCP,         /**< Multimedia Messaging Service User
                                    Connectivity Parameters. */
  UIMDIAG_CSIM_AUTH_CAP,        /**< Authentication capability. */
  UIMDIAG_CSIM_3GCIK,           /**< 3G Cipher and Integrity Keys. */
  UIMDIAG_CSIM_DCK,             /**< Depersonalization Control Key. */
  UIMDIAG_CSIM_GID1,            /**< Japanese CDMA Group Identifier 1. */

  /* 560 */
  UIMDIAG_CSIM_GID2,            /**< JCDMA Group Identifier 2. */
  UIMDIAG_CSIM_CNL,             /**< CDMA Cooperative Network List. */
  UIMDIAG_CSIM_HOME_TAG,        /**< Home system tag. */
  UIMDIAG_CSIM_GROUP_TAG,       /**< Group tag list. */
  UIMDIAG_CSIM_SPECIFIC_TAG,    /**< Specific tag list. */
  UIMDIAG_CSIM_CALL_PROMPT,     /**< Call prompt list. */
  UIMDIAG_CSIM_SF_EUIM_ID,      /**< Short Form EUIMID. */
  UIMDIAG_CSIM_EST,             /**< Enabled Service Table. */
  UIMDIAG_CSIM_HIDDEN_KEY,      /**< Key for hidden phone book entry. */
  UIMDIAG_CSIM_LCS_VERSION,     /**< Location Services protocol Version. */

   /* 570 */
  UIMDIAG_CSIM_LCS_CP,          /**< Location Services Connectivity Parameter. */
  UIMDIAG_CSIM_SDN,             /**< Service Dialing Numbers. */
  UIMDIAG_CSIM_EXT2,            /**< Extension 2. */
  UIMDIAG_CSIM_EXT3,            /**< Extension 3. */
  UIMDIAG_CSIM_ICI,             /**< Incoming Call Information. */
  UIMDIAG_CSIM_OCI,             /**< Outgoing Call Information. */
  UIMDIAG_CSIM_EXT5,            /**< Extension 5. */
  UIMDIAG_CSIM_CCP2,            /**< Capability Configuration Parameters 2. */
  UIMDIAG_CSIM_APP_LABELS,      /**< Application labels. */
  UIMDIAG_CSIM_DEV_MODEL,       /**< Device model information. */

    /* 580 */
  UIMDIAG_CSIM_ROOT_CERT,       /**< Root certificate. */
  UIMDIAG_CSIM_SMS_CAP,         /**< Short Message Service Capabilities. */
  UIMDIAG_CSIM_3GPD_MIPFLAGS,   /**< 3GPD Mobile IP flags. */
  UIMDIAG_CSIM_3GPD_SIPUPPEXT,  /**< 3GPDSimple IP User Profile Parameters extension. */
  UIMDIAG_CSIM_3GPD_MIPUPPEXT,  /**< 3GPDMobile IP User Profile Parameters extension. */
  UIMDIAG_CSIM_3GPD_IPV6CAP,    /**< 3GPD IPv6 capabilities. */
  UIMDIAG_CSIM_3GPD_TCPCONFIG,  /**< 3GPD TCP configuration. */
  UIMDIAG_CSIM_3GPD_DGC,        /**< 3GPD Data Generic Configuration. */
  UIMDIAG_CSIM_BROWSER_CP,      /**< Browser Connectivity Parameters. */
  UIMDIAG_CSIM_BROWSER_BM,      /**< Browser Boookmarks. */

    /* 590 */
  UIMDIAG_CSIM_MMS_CONFIG,      /**< Multimedia Messaging Service configuration. */
  UIMDIAG_CSIM_JDL,             /**< Java<sup>&amp;reg;</sub> Download URL. */
  UIMDIAG_CSIM_PBR,             /**< Phone Book Reference file. */
  UIMDIAG_CSIM_PSC,             /**< Phone book Synchronization Center. */
  UIMDIAG_CSIM_CHANGE_COUNTER,  /**< Change Counter. */
  UIMDIAG_CSIM_PUID,            /**< Previous Unique Identifier. */
  UIMDIAG_CSIM_LBS_XCONFIG,     /**< Location Based Services XTRA configuration. */
  UIMDIAG_CSIM_LBS_XSURL,       /**< Location Based Services XTRA Server URLs. */
  UIMDIAG_CSIM_LBS_V2CONFIG,    /**< Location Based Services V2 configuration. */
  UIMDIAG_CSIM_LBS_V2PDEADDR,   /**< Location Based Services V2 Position
                                    Determination Entity address. */

    /* 600 */
  UIMDIAG_CSIM_LBS_V2MPCADDR,   /**< Location Based Services V2 Mobile
                                    Positioning Center address. */
  UIMDIAG_CSIM_BREW_DLOAD,      /**< BREW Download. */
  UIMDIAG_CSIM_BREW_TSID,       /**< BREW Teleservice ID. */
  UIMDIAG_CSIM_BREW_SID,        /**< BREW Subscriber ID. */
  UIMDIAG_CSIM_BREW_AEP,        /**< BREW Application Execution Policy. */

   /* 655 */
  UIMDIAG_CSIM_EXT_ME_SETTING_DATA = 0x0000028F,  /**< Japanese CDMA extended ME setting data. */
  UIMDIAG_CSIM_USER_AGENT_STRING,    /**< JCDMA user agent string. */
  UIMDIAG_CSIM_IMSI_STATUS,          /**< JCDMA International Mobile Subscriber
                                         Identity status. */
  UIMDIAG_CSIM_ME_DOWNLOADABLE_DATA, /**< JCDMA ME downloadable data. */
  UIMDIAG_CSIM_ME_SETTING_DATA,      /**< JCDMA ME setting data. */

  /* 660 */
  UIMDIAG_CSIM_ME_USER_DATA,         /**< JCDMA ME user data. */
  UIMDIAG_CSIM_UIM_SVC_TABLE,        /**< JCDMA UIM service table. */
  UIMDIAG_CSIM_UIM_FEATURES,         /**< JCDMA UIM features. */
  UIMDIAG_CSIM_UIM_VERSION,          /**< JCDMA signature and version. */
  UIMDIAG_USIM_7F66_PROP1_RPM_EF,    /**< Cingular proprietary: RPM Enabled Flag */
  UIMDIAG_USIM_7F66_PROP1_RPM_PARAM, /**< Cingular proprietary: RPM Parameters */
  UIMDIAG_GSM_ONS_SHORT,             /**< CPHS: Operator Name Short String */
  UIMDIAG_USIM_ONS_SHORT,            /**< CPHS: Operator Name Short String */
  UIMDIAG_TELECOM_ICE_DN,            /**< In Case of Emergency – Dialling Number. */
  UIMDIAG_TELECOM_ICE_FF,            /**< In Case of Emergency – Free Format. */

  /* 670 */
  UIMDIAG_TELECOM_ICE_GRAPHICS,      /**< In Case of Emergency – Graphics. */
  UIMDIAG_CDMA_3GPD_UPPEXT,          /**< User Profile Parameters extension. */
  UIMDIAG_CSIM_3GPD_UPPEXT,          /**< User Profile Parameters extension. */
  UIMDIAG_NO_FILE_ENUM,  /**< Last element of the file. Also used to indicate if
                             there is no file enumeration.\n
                             @note1 This must be placed just before MAX_FILE_ENUM. */

  UIMDIAG_MAX_FILE_ENUM   = 0x7FFFFFFF /**< Forces an initialize size enumeration. */
  //reserved
}uimdiag_file_enum_type;

/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/

/* ----------------------------------------------------------------------------
   STRUCTURE:      uimdiag_req_table_elem_type

   DESCRIPTION:   The request table to store the response_id and subcmd details
                  Used in the callback
      use_flag:   Indicate whether the the current element is available or not
      rsp_id:     Response ID
      subcmd:     UIMDIAG subcommand
-------------------------------------------------------------------------------*/
typedef struct
{
  boolean                         use_flag;
  uint32                          rsp_id;
  uimdiag_subsystem_cmd_enum_type subcmd;
}uimdiag_req_table_elem_type;

extern uimdiag_req_table_elem_type *uimdiag_req_table_ptr;

/* ----------------------------------------------------------------------------
   STRUCTURE:     uimdiag_cmd_header_type

   DESCRIPTION:      Basic command header info
   command code:     Command code
   subsys_id:        ID type for the DIAG subsystem packet
   subsys_cmd_code:  Command Code for DIAG subsystem packet
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint8                        command_code;
  diagpkt_subsys_id_type       subsys_id;
  diagpkt_subsys_cmd_code_type subsys_cmd_code;
} uimdiag_cmd_header_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      uimdiag_cmd_rsp_hdr_type

   DESCRIPTION:    Generic response herader type for outgoing diag
                   packet diagpkt_subsys_header_v2_type
   rsp_header:     Response header
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  diagpkt_subsys_header_v2_type rsp_header;
}uimdiag_cmd_rsp_hdr_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      uimdiag_generic_req_type

   DESCRIPTION:             Generic Request type for incoming diag packet
   cmd_header:              Common Command header
   client_id_and_evt_dereg: Command Req for Client Id and Evt Dereg
   session_open:            Command Req for Session Open
   session_close:           Command Req for Session Close
   session_act_or_sw_prov:  Command Req for Activate or Switch Prov
   session_get_info:        Command Req for Session Get Info
   session_get_all_pin_stat:Command Req for Session Get All Pin Status
   session_verify_pin:      Command Req for Session Verify Pin
   session_disable_pin:     Command Req for Session Disable Pin
   session_enable_pin:      Command Req for Session Enable Pin
   session_change_pin:      Command Req for Session change Pin
   session_unblock_pin:     Command Req for Session Unblock Pin
   session_get_file_attr:   Command Req for Session Get File Attr
   session_read_record:     Command Req for Session Read Record
   session_read_transparent:Command Req for Session Read Transparent
   session_write_record:    Command Req for Session Write Record
   session_write_transparent:Command Req for Session Write Transparent        .
   session_reg_for_refresh: Command Req for Register for Refresh
   session_ok_to_refresh:   Command Req for Simulating OK to Refresh to MMGSDI
   session_refresh_complete:Command Req to indicate to MMGSDI that the client
                            has finished its refresh process.
   session_dereg_for_refresh:Commmand Req to deregister refresh files by
                             providing a list of file enums or paths. The client
                             can also change vote for init preferences
   session_perso:           Command Req to activate / deactivate a
                            personalization feature by an external source
   session_perso_get_ind:   Command Req to retrieve the information about
                            personalization Indicators
   session_perso_get_key:   Command Req to get feature Indicator key
   session_perso_get_dck_retries:
                            Command Req to get the DCK number of retries.
   session_perso_lock_dn_enable_ltd_access
                            Command Req used to lock down some of the
                            perso commands
   session_deact_prov:      Command Req to allow the client to deactivate
                            an application SIM/USIM/RUIM/CSIM from being the
                            provisioning application.
   session_jcdma_get_card_info: Command Req to retrive JCDMA card color/lock
   session_get_app_cap:     Command Req to extract all the provisioning application
                            capabilities like fdn, bdn, acl, imsi status and
                            phone book related
   session_get_cphs_info:   Command req used to get the CPHS related information.
   session_isim_auth:       Command Req used to perform a "Network" Initiated
                            Authentication with the ISIM Application.
   session_send_crd_status: Command Req used to get info concerning the
                            current directory
   session_disable_srv:     Command Req used to disable only an ACL, BDN, or
                            FDN service
   session_enable_srv:      Command Req used to enable only an ACL, BDN, or
                            FDN service
   session_is_srv_avail:    Command Req used to determine if a service is
                            available, given the service type.
   session_seek_req:        Command Req that performs GSM Seek command on card.
   session_search:          Command Req that performs Search command on UICC card.
   session_trigger_refresh: Command Request that triggers Refresh
   is_service_available_sync:
                            Command Request to check is Service is available (sync API)
   session_get_nvruim_data_sync:
                            Command Request to performs NVRUIM Sync
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uimdiag_cmd_header_type    cmd_header;
  PACKED union PACKED_POST
  {
    uimdiag_mmgsdi_client_id_and_evt_dereg_cmd_type    client_id_and_evt_dereg;
    uimdiag_mmgsdi_session_open_cmd_type               session_open;
    uimdiag_mmgsdi_sessionid_type                      session_close;
    uimdiag_mmgsdi_session_act_or_sw_prov_cmd_type
                                                       session_act_or_sw_prov;
    uimdiag_mmgsdi_session_get_info_cmd_type           session_get_info;
    uimdiag_mmgsdi_sessionid_type                      session_get_all_pin_stat;
    uimdiag_mmgsdi_session_verify_pin_cmd_type         session_verify_pin;
    uimdiag_mmgsdi_session_disable_pin_cmd_type        session_disable_pin;
    uimdiag_mmgsdi_session_enable_pin_cmd_type         session_enable_pin;
    uimdiag_mmgsdi_session_change_pin_cmd_type         session_change_pin;
    uimdiag_mmgsdi_session_unblock_pin_cmd_type        session_unblock_pin;
    uimdiag_mmgsdi_session_get_file_attr_cmd_type      session_get_file_attr;
    uimdiag_mmgsdi_session_read_record_cmd_type        session_read_record;
    uimdiag_mmgsdi_session_read_transparent_cmd_type   session_read_transparent;
    uimdiag_mmgsdi_session_write_record_cmd_type       session_write_record;
    uimdiag_mmgsdi_session_write_transparent_cmd_type  session_write_transparent;
    uimdiag_mmgsdi_session_refresh_cmd_type            session_refresh;
    uimdiag_mmgsdi_session_ok_to_refresh_cmd_type      session_ok_to_refresh;
    uimdiag_mmgsdi_session_refresh_complete_cmd_type   session_refresh_complete;
    uimdiag_mmgsdi_session_perso_cmd_type              session_perso;
    uimdiag_mmgsdi_session_perso_get_ind_cmd_type      session_perso_get_ind;
    uimdiag_mmgsdi_session_perso_get_key_cmd_type      session_perso_get_key;
    uimdiag_mmgsdi_session_perso_get_dck_retries_cmd_type
                                                       session_perso_get_dck_retries;
    uimdiag_mmgsdi_session_perso_lock_dwn_enable_ltd_access_cmd_type
                                                       session_perso_lock_dwn_enable_ltd_access;
    uimdiag_mmgsdi_sessionid_type                      session_deact_prov;
    uimdiag_mmgsdi_sessionid_type                      session_jcdma_get_card_info;
    uimdiag_mmgsdi_sessionid_type                      session_get_app_cap;
    uimdiag_mmgsdi_sessionid_type                      session_get_cphs_info;
    uimdiag_mmgsdi_session_isim_auth_cmd_type          session_isim_auth;
    uimdiag_mmgsdi_session_send_crd_status_cmd_type    session_send_crd_status;
    uimdiag_mmgsdi_session_service_cmd_type            session_disable_srv;
    uimdiag_mmgsdi_session_service_cmd_type            session_enable_srv;
    uimdiag_mmgsdi_session_service_cmd_type            session_is_srv_avail;
    uimdiag_mmgsdi_session_seek_cmd_type               session_seek_req;
    uimdiag_mmgsdi_session_search_cmd_type             session_search_req;
    uimdiag_mmgsdi_session_prov_app_init_compl_cmd_type
                                                       session_prov_app_init_complete;
    uimdiag_mmgsdi_session_compute_ip_auth_cmd_type    session_compute_ip_auth;

    uimdiag_mmgsdi_session_ssd_update_cmd_type         session_ssd_update;
    uimdiag_mmgsdi_session_confirm_ssd_cmd_type        session_cnf_ssd;
    uimdiag_mmgsdi_session_bs_chal_cmd_type            session_bs_chal;
    uimdiag_mmgsdi_sessionid_type                      session_read_prl;
    uimdiag_mmgsdi_session_get_info_sync_cmd_type      session_get_info_sync;
    uimdiag_mmgsdi_session_read_cache_file_size_cmd_type
                                                       session_cache_file_size_type;
    uimdiag_mmgsdi_session_read_cache_cmd_type         session_read_cache;
    uimdiag_mmgsdi_session_write_cache_cmd_type        session_write_cache;
    uimdiag_mmgsdi_sessionid_type                      session_illegal_suscr_sync;
    uimdiag_mmgsdi_session_toggle_uim_pwr_ctrl_cmd_type
                                                       session_toggle_uim_pwr_ctrl;
    uimdiag_mmgsdi_sessionid_type                      session_get_app_cap_sync;
    uimdiag_mmgsdi_sap_get_atr_cmd_type                sap_get_atr;
    uimdiag_mmgsdi_sap_send_apdu_cmd_type              sap_send_apdu;
    uimdiag_mmgsdi_sap_generic_cmd_type                sap_power_on;
    uimdiag_mmgsdi_sap_generic_cmd_type                sap_power_off;
    uimdiag_mmgsdi_sap_generic_cmd_type                sap_card_reader_status;
    uimdiag_mmgsdi_sap_generic_cmd_type                sap_reset;
    uimdiag_mmgsdi_session_refresh_proceed_voting_cmd_type    session_refresh_proceed_voting;
    uimdiag_mmgsdi_session_proceed_refresh_cmd_type       session_refresh_ok_to_proceed;
    uimdiag_mmgsdi_naa_refresh_cmd_type                session_trigger_refresh;
    uimdiag_mmgsdi_card_pdown_cmd_type                 card_pdown;
    uimdiag_mmgsdi_card_pup_cmd_type                   card_pup;
    uimdiag_mmgsdi_activate_onchip_sim_cmd_type        activate_onchip_sim;
    uimdiag_mmgsdi_send_apdu_cmd_type                  send_apdu;
    uimdiag_mmgsdi_get_all_available_apps_cmd_type get_all_available_apps;
    uimdiag_mmgsdi_session_service_cmd_type            is_service_available_sync;
    uimdiag_mmgsdi_sap_generic_cmd_type                sap_connect;
    uimdiag_mmgsdi_sap_disconnect_cmd_type             sap_disconnect;
    uimdiag_mmgsdi_session_get_operator_name_cmd_type       session_get_operator_name;
    uimdiag_mmgsdi_session_open_ext_cmd_type           session_open_ext;
    uimdiag_mmgsdi_nvruim_data_req_type                session_get_nvruim_data_sync;
  }request;
}uimdiag_generic_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      uimdiag_simlock_req_type

   DESCRIPTION:
-------------------------------------------------------------------------------*/
typedef struct
{
  uimdiag_cmd_header_type    cmd_header;
  union
  {
    /* SIM Lock */
    simlock_set_config_policy_msg_req_type             config_policy;
    simlock_set_lock_hck_msg_req_type                  lock_hck;
    simlock_set_lock_ck_msg_req_type                   lock_ck;
    simlock_unlock_device_msg_req_type                 unlock;
    simlock_get_category_data_msg_req_type             get_category_data;
    simlock_add_lock_codes_hck_msg_req_type            add_lock_codes_hck;
    simlock_add_lock_codes_ck_msg_req_type             add_lock_codes_ck;
  } data;
} uimdiag_simlock_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      uimdiag_cmd_rsp_pyld_type

   DESCRIPTION:  Command response type
   client_id_and_evt_reg:     Client ID registration
   client_id_and_evt_dereg:   Client ID deregistration
   session_open:              Payload for Session Open
   session_close:             Payload for Session Close
   session_act_or_sw_prov:    Payload for Activate or Switch Prov
   session_get_info:          Payload for Session Get Info
   session_get_all_pin_status:Payload for Session Get Pin Status
   session_pin_operation:     Payload for Session Pin Operation
   session_get_file_attr:     Payload for Session Get file attr
   session_read_record:       Payload for Session read record/read transparent
   session_write_record:      Payload for Session write record/write transparent
   session_refresh:           Payload for Refresh
   session_perso:             Payload for Perso Activation
   session_perso_get_ind:     Payload for retrieving the information about
                              personalization Indicators
   session_perso_data:        Payload to set / get  the personalization data
                              for a particular personalization feature
   session_perso_get_key:     Payload for Perso feature Indicator key
   session_perso_get_dck_retries:
                              Payload for Perso Get DCK Retries.
   session_perso_lock_dwn_enable_ltd_access
                              Confirmation/response for the MMGSDI_PERSO_LOCK_DOWN_REQ
                              request/command. It returns the locked down command
   session_deact_prov:        Payload for deactivate provisioning of an application
   session_jcdma_get_card_info Payload to get JCDMA card info
   session_get_app_cap:       Payload to extract all the provisioning application
                              capabilities like fdn, bdn, acl, imsi status and
                              phone book related
   session_get_cphs_info:     Payload to get the CPHS related information.
   session_isim_auth:         Payload to perform a "Network" Initiated
                              Authentication with the ISIM Application.
   session_send_crd_status    Payload to get information about current directory
   session_disable_srv:       Payload to disable an ACL, BDN, or FDN service
   session_enable_srv:        Payload to enable  an ACL, BDN, or FDN service
   session_is_srv_avail:      Payload to determine if a service is available,
                              given the service type.
   session_search_resp:       Response Payload that performs Search command on
                              UICC card.
   session_get_nvruim_data_sync:
                              Response Payload for NVRUIM Sync
-----------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  mmgsdi_return_enum_type mmgsdi_status;
  PACKED union PACKED_POST
  {
    /* MMGSDI */
    uimdiag_mmgsdi_rsp_client_id_and_evt_reg_pyld_type
                                                    client_id_and_evt_reg;
    uimdiag_mmgsdi_rsp_client_id_and_evt_dereg_pyld_type
                                                    client_id_and_evt_dereg;
    uimdiag_mmgsdi_rsp_session_open_pyld_type       session_open;
    uimdiag_mmgsdi_rsp_session_close_pyld_type      session_close;
    uimdiag_mmgsdi_sessionid_type                   session_act_or_sw_prov;
    uimdiag_mmgsdi_rsp_session_get_info_pyld_type   session_get_info;
    uimdiag_mmgsdi_rsp_session_get_all_pin_stat_pyld_type
                                                    session_get_all_pin_status;
    uimdiag_mmgsdi_rsp_session_pin_operation_pyld_type
                                                    session_pin_operation;
    uimdiag_mmgsdi_rsp_session_get_file_attr_pyld_type
                                                    session_get_file_attr;
    uimdiag_mmgsdi_rsp_session_read_record_pyld_type
                                                    session_read_record;
    uimdiag_mmgsdi_rsp_session_write_record_pyld_type
                                                    session_write_record;
    uimdiag_mmgsdi_sessionid_type                   session_refresh;
    uimdiag_mmgsdi_rsp_session_perso_pyld_type      session_perso;
    uimdiag_mmgsdi_rsp_session_perso_get_data_pyld_type
                                                    session_perso_get_data;
    uimdiag_mmgsdi_rsp_session_perso_get_ind_pyld_type
                                                    session_perso_get_ind;
    uimdiag_mmgsdi_sessionid_type                   session_perso_data;
    uimdiag_mmgsdi_rsp_session_perso_get_key_pyld_type
                                                    session_perso_get_key;
    uimdiag_mmgsdi_rsp_session_perso_get_dck_retries_pyld_type
                                                    session_perso_get_dck_retries;
    uimdiag_mmgsdi_rsp_session_perso_lock_dwn_enable_ltd_access_pyld_type
                                                    session_perso_lock_dwn_enable_ltd_access;
    uimdiag_mmgsdi_rsp_session_deact_prov_pyld_type session_deact_prov;
    uimdiag_mmgsdi_rsp_session_jcdma_get_card_info_pyld_type
                                                    session_jcdma_get_card_info;
    uimdiag_mmgsdi_rsp_session_get_app_cap_pyld_type
                                                    session_get_app_cap;
    uimdiag_mmgsdi_rsp_session_get_app_cap_pyld_type
                                                    session_get_app_cap_sync_rsp;
    uimdiag_mmgsdi_rsp_session_get_cphs_info_pyld_type
                                                    session_get_cphs_info;
    uimdiag_mmgsdi_rsp_session_isim_auth_pyld_type  session_isim_auth;
    uimdiag_mmgsdi_rsp_session_send_crd_status_pyld_type
                                                    session_send_crd_status;
    uimdiag_mmgsdi_rsp_session_srv_pyld_type        session_disable_srv;
    uimdiag_mmgsdi_rsp_session_srv_pyld_type        session_enable_srv;
    uimdiag_mmgsdi_rsp_session_is_srv_avail_pyld_type
                                                    session_is_srv_avail;
    uimdiag_mmgsdi_rsp_session_search_seek_pyld_type
                                                    session_search_rsp;
    uimdiag_mmgsdi_sessionid_type                   session_prov_app_init_complete_rsp;
    uimdiag_mmgsdi_session_compute_ip_auth_rsp_type session_compute_ip_auth_rsp;

    uimdiag_mmgsdi_sessionid_type                   session_ssd_update_rsp;
    uimdiag_mmgsdi_session_confirm_ssd_rsp_type     session_cnf_ssd_rsp;
    uimdiag_mmgsdi_session_bs_chal_rsp_type         session_bs_chal_rsp;
    uimdiag_mmgsdi_session_read_prl_rsp_type        session_read_prl_rsp;
    uimdiag_mmgsdi_rsp_session_get_info_pyld_type   session_get_info_sync_rsp;
    uimdiag_mmgsdi_session_cache_file_size_rsp_type
                                                    session_cache_file_size_rsp;
    uimdiag_mmgsdi_session_read_cache_rsp_type      session_read_cache_rsp;
    uimdiag_mmgsdi_sap_get_atr_rsp_type             sap_get_atr_rsp;
    uimdiag_mmgsdi_sap_send_apdu_rsp_type           sap_send_apdu_rsp;
    uimdiag_mmgsdi_sap_card_reader_status_rsp_type  sap_card_reader_status_rsp;
    uimdiag_mmgsdi_sap_generic_rsp_type             sap_power_on_rsp;
    uimdiag_mmgsdi_sap_generic_rsp_type             sap_power_off_rsp;
    uimdiag_mmgsdi_sap_generic_rsp_type             sap_reset_rsp;
    uimdiag_mmgsdi_card_pdown_rsp_type              card_pdown_rsp;
    uimdiag_mmgsdi_card_pup_rsp_type                card_pup_rsp;
    uimdiag_mmgsdi_activate_onchip_sim_rsp_type     activate_onchip_sim_rsp;
    uimdiag_mmgsdi_send_apdu_rsp_type               send_apdu_rsp;
    uimdiag_mmgsdi_get_all_available_apps_rsp_type get_all_available_apps_rsp;
    uimdiag_mmgsdi_rsp_session_srv_pyld_type        is_service_available_sync_rsp;
    uimdiag_mmgsdi_sap_generic_rsp_type             sap_connect_rsp;
    uimdiag_mmgsdi_sap_disconnect_rsp_type          sap_disconnect_rsp;
    uimdiag_mmgsdi_rsp_session_get_operator_name_type session_get_operator_name;
    uimdiag_mmgsdi_rsp_session_open_ext_pyld_type   session_open_ext;
    uimdiag_mmgsdi_session_get_nvruim_data_sync_rsp_type
                                                    session_get_nvruim_data_sync;
    /* SIM Lock */
    uimdiag_simlock_get_status_rsp_type             get_status_rsp;
    uimdiag_simlock_unlock_rsp_type                 unlock_rsp;
    uimdiag_simlock_get_category_data_rsp_type      get_category_data_rsp;
    uimdiag_simlock_get_category_data_ext_rsp_type  get_category_data_ext_rsp;
  }response;
}uimdiag_cmd_rsp_pyld_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      uimdiag_generic_rsp_type

   DESCRIPTION:     Generic uimdiag response type
    cmd_hdr:        Command header
    rsp_payload:    Response Payload
-----------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uimdiag_cmd_rsp_hdr_type     cmd_hdr;
  uimdiag_cmd_rsp_pyld_type    rsp_payload;
}uimdiag_generic_rsp_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      uimdiag_check_diag_support_rsp

   DESCRIPTION:    Check diag support response type
                   If value is 1 , then uimdiag is supported,
                   If value is 2, GSDIDIAG is supported.
-----------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uimdiag_cmd_rsp_hdr_type     cmd_hdr;
  uint32                       status;
}uimdiag_check_diag_support_rsp_type;


/*=============================================================================

                       FUNCTIONS DECLARATIONS

=============================================================================*/

/*===========================================================================

FUNCTION UIMDIAG_MALLOC

DESCRIPTION
  The uimdiag_malloc calls TMC alloc with different heap_size based the size
  of the allocation required.

DEPENDENCIES
  None.

RETURN VALUE
  void pointer

SIDE EFFECTS
  a pointer is allocated.
===========================================================================*/
void * uimdiag_malloc(dword size);

/*===========================================================================
FUNCTION UIMDIAG_GET_INDEX

DESCRIPTION
    Gets an available space from the req_table. This row is then marked as
    used. When it is no longer necessary, the row should be freed by calling
    uimdiag_free_index.

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None
===========================================================================*/
int uimdiag_get_index(void);

/*===========================================================================
FUNCTION uimdiag_free_index

DESCRIPTION
    Frees up space on req_table that is no longer being used.

ARGUMENTS
   int index: index into the request table

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None
===========================================================================*/
void uimdiag_free_index(int index);

/*===========================================================================

FUNCTION UIMDIAG_FREE

DESCRIPTION
  The uimdiag_free frees the pointer from the regular tmc_heap or the tmc_heap_small
  based on the pointer address, but only if FEATURE_UIMDIAG_USE_SMALL_HEAP is defined
  and FEATURE_LOW_MEMORY_USAGE IS NOT

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  the pointer is freed.
===========================================================================*/
void uimdiag_free(void * ptr);

/*===========================================================================
FUNCTION UIMDIAG_INIT

DESCRIPTION
    Registers sub command codes for GSDI Diag sub system.

ARGUMENTS
    None

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None
===========================================================================*/
void uimdiag_init(void);

/*===========================================================================
FUNCTION: uimdiag_parse_cmd_data

DESCRIPTION:
  Populates the request structure with data from the Diag packet.

INPUTS:
  *req_ptr:      Generic_command struct request pointer
                 which will be populated with data from the diag_req_ptr
  *diag_req_ptr: pointer to raw Diag request packet
   diag_req_len: len of the diag request packet

DEPENDENCIES:
  None

RETURN VALUE:
   UIMDIAG_SUCCESS
   UIMDIAG_ERROR

OUTPUT:
  None
===========================================================================*/
uimdiag_return_enum_type uimdiag_parse_cmd_data (
  uimdiag_generic_req_type  *req_ptr,
  PACKED void               *diag_req_ptr,
  uint32                     diag_req_len
);

#endif /* UIMDIAG_H */
