#ifndef UIM_H
#define UIM_H
/*===========================================================================

               E X T E R N A L   U I M  T A S K   D E F I N I T I O N S

DESCRIPTION
  This contains all the declarations for the Authentication Task.

Copyright (c) 2001-2008,2010-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/api/uim.h#1 $
$DateTime: 2012/11/19 20:01:34
$ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/10/14   tl      Introduce EF 3GPDUPPExt
12/11/13   vv      Added support for the EF-PSISMSC in DF TELECOM
10/01/13   vv      Added support for the EF-PWS
06/27/13   spo     Added support for files under DF TELECOM
12/05/12   js      UIM Parallel Processing changes
08/10/12   av      Removed RPC support along with RPC style comments
07/13/12   vv      Added support for the CSG EFs
11/23/11   shr     Removed legacy GSDI interfaces
11/02/11   bcho    Added support for Get Operator Name API
10/21/11   nmb     Added RPM Files for SIM EF enumerations
08/24/11   nmb     Additional RPM Files for USIM EF path lookups
04/21/11   nmb     Added RPM Files for USIM EF path lookups
07/06/10   yb      Removing dependency on Rex from Apps side
06/28/10   nmb     Updates for JCDMA CSIM support
03/02/10   jk      Added MMSS files
02/18/10   vs      Merged CSIM functionality
12/08/09   js      Added support for UIM_CSIM_ARR file
10/28/09   mib     Added support for additional files
10/28/09   nb      Release 7 Enhanced Network Selection
10/23/09   kp      Moved shared enums from uim_v.h to uim.h
08/13/09   nb      Moved few declarations from uim_v.h
07/17/09   yb      Added support for BCSMS service in cdma service table
03/02/09   sun     Removed Private header inclusion
12/17/08   nb      Common Modem Interface
12/04/08   nb      Fixed QTF compilation problem
12/03/08   nb      Fixed Compilation Error
11/26/08   nb      Removed Featurization Flags for Common Modem Interface.
09/23/08   jk      Polling value changed from 28 s to 25 s to align 1x PCH
                   and QPCH page decodes
09/17/08   vs      Updated clash entry for poll delay timer SIG
09/15/08   vs      Added clash entry for UIM USB ICCD pre-suspend timer SIG
08/26/08   kk      Few comments from uim directory management review
08/20/08   vs      Added SIG clash entries for poll delay and fallback to ISO
08/14/08   kk      Added handling for SELECT EFs with non-generic file ids
07/22/08   js      Fixed UIM_MAX_PATH_ELEMENTS issue. Updated path length to 4
                   for all UIM commands. MAX path length in MMGSDI is set to 5.
07/17/08   vs      Added support for UIM USB UICC ICCD interface
07/17/08   kk      Added support for PKCS 15
06/21/08   kk      Added PKCS15 Support
04/30/08   kk      Added support for MMS EFs
03/24/08   sun     Added support for OMA BCAST
03/20/08   nk      Added OMH EF and CST Support
01/02/08   jk      Added functions to check and remove memory allocs on cmd q
11/19/07   js      Radio Access Technology Balancing support
11/08/07   vs/nk   Added EUIMID and OMH feature support
10/03/07   jk      Compile Fix for HRPD Off
09/27/07   tml     Lint fixes
09/10/07   tml     Added MFLO support, remove featurization for 1000 PBM
08/16/07   jk      Rearranged featuization to support DATA Auth on 7K
07/02/07   sun     Added support for ODD INS Class
05/21/07   jk      Add API so that others can check if UIM is in Recovery mode
05/02/07   sun     Added support for MBMS
05/02/07   sp      Changed the value of UIM_MAX_POLL_DURATION to prevent
                   overflow.
04/12/07   sun     Featurized Phonebook
02/13/07   pv      Increase the UIM_POLL_OFFSET by 1000 milli seconds. and
                   define the maximum delay between regular wake-up poll
                   timer and always on poll timer.
02/09/07   sk      Added support for 1000 phonebook entries.
11/13/06   pv      Add UIM_PASSIVE as a vote to poll UIM.
10/05/06   pv      Rectify the value for UIM_MAX_EF_BY_PATH_CACHE_ITEMS
09/21/06   sun     Change RPC meta comments for uim_get_xxx_path_from_file_id
                   to allow a null to be returned in the output path pointer.
09/11/06   jk      Changed featurization so that 7200 and 7500 generate identical
                   RPC Files
08/09/06   pv      Featurize the usage of members of nv_uim_config_params_type
                   under FEATURE_UIM_NO_ADDITIONAL_CONFIG_PARAMS for
                   targets not picking up the NV dependencies
07/25/06   jk      Added Signal for EFSLOG
07/05/06   jar     Fixes to allow for more than 4 ANRs to be used.
06/12/06   pv      Add UIM_RECOVER_COMPLETE command.
                   Re-define UIM_MAX_NUM_CMD_ATTEMPTS to use the config param.
                   Define uim_return_type.
                   Add UIM_OPTION_RECOVERY_CMD to options.
                   Extern uim_recovery_cmd.
06/05/06   pv      Add definition for UIM_MAX_BAD_BLOCK_RETRIES to be used
                   while in T=1 protocol to identify maximum re-try count.
04/18/06   nk      Un-featurize code to work when FEATURE_UIM_UICC is not defined
04/19/06   pv      Metadata comments for RPC export for 7200.
04/03/06   pv      Lint Fixes
03/21/06   tml     added cfis support
03/14/06   pv      Added code to wait until uim_mv_init is done by tmc.
03/09/06   pv      Added new command to clear the response cache
03/08/06   jar     Merged in HZI from branch
02/16/06   pv      Added support for UIM_USIM_EHPLMN
02/15/06   pv      Reduced UIM_MAX_USIM_ITEMS to 88
02/14/06   nk      Merged for: Implement a cached select mechanism.  Which
                   would not send down the select to the card if the response
                   is available from the cache.
01/30/06   pv      Add more enemurations to indicate power down reason to GSDI
11/22/05   jk      Metadata comments for RPC export
08/29/05   jk      Added functions to get path to files from EF file ID
08/25/07   pv      Added options to pdown command.
08/04/05   wli     Added comments
08/02/05   wli     Added HOT SWAP feature.
07/22/05   pv      Got definitions for Aggregation of Traffic channel
                   voting bits from uim.c need to use them in uimgen.c
07/18/05   sun     Added support for USIM files in ENS
06/13/05   pv      Code to support UIM commands with allocated memory.
06/08/05   wli     Defined UIM_MAX_NUM_CMD_ATTEMPTS for maximum command retry.
06/02/05   pv      Code for FEATURE_UIM_ISIM & FEATURE_UIM_SESSION_MANAGEMENT
05/10/05   pv      Added code for BT sim access - Code to handle
                   UIM_RESET_SWITCH_UIM_PASSIVE_R, UIM_POWER_UP_UIM_PASSIVE_R,
                   and UIM_POWER_DOWN_R commands and responses
04/25/05   jk      Added FEATURE_UIM_RUIM around new HW ESN issue to fix compile
                   issue for non-RUIM
04/22/05   jk      Added function to return HW ESN for BREW
03/14/05   wli     Added 820B EFs
02/28/05   sun     Added file - EF MWIS
01/14/05   sun     Added support for Cingular Wireless
12/16/04   jk      Merged changes for Orange support from mainline
12/16/04   jk      Fixed CR # 54476 - Streaming APDU size up to 260 bytes
11/22/04   jk      Finished adding support for 3GPD
11/21/04   wli     Dual slot merge and debug.
10/06/04   jar     Merged in 4.2x Additional EFs
09/28/04   wli     Added enum items to status.
09/01/04   jk      Added EF to table for HRPD AN
09/01/04   ck      Added support for CPHS files
09/01/04   ck      Added support for CSIM
06/16/04   ck      Added protocol type to uim_dfs_present structure
06/16/04   wli     Added ORANGE DF and its related EFs.
06/11/04   wli     Added EFs to support JCDMA.
05/17/04   ck      Added data structures to support ISO 7816 streaming API.
                   Modified WIM and UTIL APIs to include channel information
                   and removed AID information in the command.
                   Prefixed polling related definitions with UIM
03/26/04   ts      Added a transaction timer signal for signaling the UIM task
                   when detecting excessive null procedure bytes.
12/19/03   jk      Added processing for R-UIM 3GPD Services.
12/09/03   ck      Changed the min poll duration to 5 sec and added a constant
                   UIM_POLL_OFFSET which is 1sec.
10/17/03   ck      Added support to stream APDU.
08/15/03   ck      Added UIM_CDMA_TC and UIM_GW_TC bit to voting interface.
08/14/03   ck      Added support for dual slots.
07/09/03   ck      Changed the polling interval timer from 29 secs to 28 secs
                   as were failing the polling tests in some corner case
                   scenarios.
07/09/03   ck      Removed the featurization around the funtion uim_nv_init()
03/28/03   ts      Changed FEATURE_UIM_USIM feature to FEATURE_UIM_UICC for
                   UICC functions, seperate from USIM.
03/28/03   ts      Externalized uim_process_card response for T=1 protocol.
02/26/03   jk      Added UIM UTIL and WIM functionality
03/04/03   ck      Externalised the function uim_get_enum_value_for_path and
                   added an item UIM_NO_SUCH_ITEM to the uim_items_enum_type
02/20/03   ck      Cast the MAX_POLL_DURATION value to unsigned.
02/19/03   ts      Fixed lint errors from GSTK support code.
02/13/03   wli     Added to support GSTK.
01/17/03   ck      Added uim_aid_type field to all CHV related commands and
                   featurized it under FEATURE_UIM_USIM
01/07/03   ts      Added EFrplmnat
10/25/02   ck      Added an enum UIM_POLL_ERR_S to uim_status_type
10/11/02   ck      Externalized the function uim_nv_init which reads the first
                   instruction class from NV.
07/24/02   ck      Added UIM_TC to uim_voter_type.
07/24/02   ck      Added definitions for Telecom, Solsa and Mexe DFs.
05/21/02   ts      Added support to receive 256 byte transfers from the card.
04/09/02   ck      Modified the featuization around UIM_CLK_OFF_S to include
                   FEATURE_UIM_USIM.
02/11/02   ts      Added more enum constants for EFs that are identified but must
                   selected by the path field in the commands.  This is because
                   their path is determined by the card.
02/13/02   wli     Deleted #ifdef around UIM_xxx_DF definitions.
01/22/02   ck      Added support for clock stop mode.
01/08/02   ck      Wrapped the call to uim_return_cdma_svc_availabililty around
                   the feature FEATURE_UIM_RUIM
12/18/01   ck      Added support for EFs in DCS1800 DF.
12/13/01   ts      Fixed some more comments with regard to feature switches.
12/04/01   ts      Fixed a few comment lines.
11/13/01   ts      Added support for UIM Application Toolkit (UTK).
10/11/01   ck      Enhanced the SEEK interface to include path in the command.
07/27/01   ts      Cleaned up some indents in the code.
07/26/01   wli     Moved some definitions from uimgen.h
07/23/01   ck      Added support for EF ECC, EF SPC STATUS, EF MAX PRL and
                   added a function definition to export the response to
                   store esn_me.
05/28/01   ck      Added support for R-UIM, GSM and USIM modules.
01/15/01   ck      Created module from AUTH header module rev 1.5 from Common
                   archive.


===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

/*===========================================================================

                        DEFINITION DEPENDANCIES

===========================================================================*/
/* Size of CDMA service table */
#define UIM_CDMA_SVC_TABLE_SIZE  12

#define UIM_EF_UNDER_MF_START    0x0
#define UIM_RUIM_EF_START        0x1
#define UIM_GSM_EF_START         0x2
#define UIM_TELECOM_EF_START     0x3
#define UIM_USIM_EF_START        0x4
#define UIM_DCS1800_EF_START     0x5
#define UIM_WIM_EF_START         0x6
#define UIM_ORANGE_EF_START      0x7
#define UIM_CINGULAR_EF_START    0x8
#define UIM_ISIM_EF_START        0x9
#define UIM_HZI_EF_START         0xA
#define UIM_MFLO_EF_START        0xB
#define UIM_CSIM_EF_START        0xC
#define UIM_EF_PATH_START        0xF

/* b1-b3 of P2 */
#define UIM_P2_SRCH_FORWARD  0x04
#define UIM_P2_SRCH_BACKWARD 0x05
#define UIM_P2_SRCH_ENHANCED 0x06

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
typedef enum {
  UIM_NO_SUCH_MODE = 0,        /* No such mode */
  UIM_ICC          = 1,        /* ICC protocol */
  UIM_CDMA         = UIM_ICC,  /* CDMA mode of operation uses ICC protocol */
  UIM_GSM          = UIM_ICC,  /* GSM mode of operation uses ICC protocol */
  UIM_UICC         = 2,        /* UICC protocol */
  UIM_WCDMA        = UIM_UICC, /* WCMDA mode of operation uses UICC protocol */
  UIM_MAXIMUM_MODE = 3
}uim_protocol_type;

/* SELECT_F command */
typedef enum {
  UIM_NO_SUCH_FILE = 0,    /* No such file */
  UIM_MF  = 1,             /* Master File */
  UIM_DF  = 2,             /* Dedicated File */
  UIM_EF  = 3,             /* Elementary File */
  UIM_ADF = 4,             /* Application DF */
  UIM_MAXIMUM_FILE = 5
} uim_file_type;

typedef enum
{
   /* EFs at the MF level */
  /* (0x0)00 => 0 */
  UIM_ICCID           = (UIM_EF_UNDER_MF_START << 8),/* ICCID */
  UIM_ELP,                    /* Extended Language Preference */
  UIM_DIR,                    /* EF DIR for USIM */
  UIM_ARR,                    /* Access rule reference */

   /* EFs in CDMA DF*/
  /* (0x1)00 => 0 */
  UIM_CDMA_CC         = (UIM_RUIM_EF_START << 8),  /* Call Count */
  UIM_CDMA_IMSI_M,              /* IMSI_M */
  UIM_CDMA_IMSI_T,              /* IMSI_T */
  UIM_CDMA_TMSI,                /* TMSI */
  UIM_CDMA_ANALOG_HOME_SID,     /* Analog Home SID */
  UIM_CDMA_ANALOG_OP_PARAMS,    /* Analog Operational Parameters */
  UIM_CDMA_ANALOG_LOCN_AND_REGN_IND,  /* Analog Locn and Regn Indicators */
  UIM_CDMA_HOME_SID_NID,        /* CDMA Home SID and NID */
  UIM_CDMA_ZONE_BASED_REGN_IND, /* CDMA Zone Based Regn Indicators */
  UIM_CDMA_SYS_REGN_IND,        /* CDMA System/Network Regn Indicators */

  /* (0x1)0A => 10 */
  UIM_CDMA_DIST_BASED_REGN_IND, /* CDMA Distance Based Regn Indicators */
  UIM_CDMA_ACCOLC,              /* Access Overload Class */
  UIM_CDMA_CALL_TERM_MODE_PREF, /* Call Termination Mode Preferences */
  UIM_CDMA_SCI,                 /* Suggested Slot Cycle Index */
  UIM_CDMA_ANALOG_CHAN_PREF,    /* Analog Channel Preferences */
  UIM_CDMA_PRL,                 /* Preferred Roaming List */
  UIM_CDMA_RUIM_ID,             /* Removable UIM ID */
  UIM_CDMA_CDMA_SVC_TABLE,      /* CDMA Service Table */
  UIM_CDMA_SPC,                 /* Service Programming Code */
  UIM_CDMA_OTAPA_SPC_ENABLE,    /* OTAPA/SPC-Enable */

  /* (0x1)14 => 20 */
  UIM_CDMA_NAM_LOCK,            /* NAM-Lock */
  UIM_CDMA_OTASP_OTAPA_FEATURES,/* OTASP-OTAPA Parameters */
  UIM_CDMA_SERVICE_PREF,        /* Service Preferences */
  UIM_CDMA_ESN_ME,              /* ESN_ME */
  UIM_CDMA_RUIM_PHASE,          /* R-UIM Phase */
  UIM_CDMA_PREF_LANG,           /* Preferred Languages */
  UIM_CDMA_UNASSIGNED_1,        /* Not assigned yet */
  UIM_CDMA_SMS,                 /* Short Messages */
  UIM_CDMA_SMS_PARAMS,          /* Short Message Service Parameters */
  UIM_CDMA_SMS_STATUS,          /* SMS Status */

  /* (0x1)1E => 30 */
  UIM_CDMA_SUP_SVCS_FEATURE_CODE_TABLE, /* Sup Services Feature Code Table */
  UIM_CDMA_UNASSIGNED_2,        /* Not assigned yet */
  UIM_CDMA_HOME_SVC_PVDR_NAME,  /* CDMA Home Service Provider Name */
  UIM_CDMA_UIM_ID_USAGE_IND,    /* R-UIM ID usage indicator */
  UIM_CDMA_ADM_DATA,            /* CDMA Administrative Data */
  UIM_CDMA_MSISDN,              /* Mobile Directory Number */
  UIM_CDMA_MAXIMUM_PRL,         /* Max size of the PRL */
  UIM_CDMA_SPC_STATUS,          /* SPC Status */
  UIM_CDMA_ECC,                 /* Emergency Call Codes */
  UIM_CDMA_3GPD_ME3GPDOPC,      /* 3GPD Operational Capabilities */

  /* (0x1)28 => 40 */
  UIM_CDMA_3GPD_3GPDOPM,        /* 3GPD Operational Mode */
  UIM_CDMA_3GPD_SIPCAP,         /* 3GPD Simple IP Capabilities */
  UIM_CDMA_3GPD_MIPCAP,         /* 3GPD Mobile IP Capabilities */
  UIM_CDMA_3GPD_SIPUPP,         /* 3GPD Simple IP User Profile Parameters */
  UIM_CDMA_3GPD_MIPUPP,         /* 3GPD Mobile IP User Profile Parameters */
  UIM_CDMA_3GPD_SIPSP,          /* 3GPD Simple IP Status Parameters */
  UIM_CDMA_3GPD_MIPSP,          /* 3GPD Mobile IP Status Parameters */
  UIM_CDMA_3GPD_SIPPAPSS,       /* 3GPD Simple IP PAP SS Parameters */
  UIM_CDMA_UNASSIGNED_3,        /* Reserved */
  UIM_CDMA_UNASSIGNED_4,        /* Reserved */

  /* (0x1)32 => 50 */
  UIM_CDMA_PUZL,                /* Preffered User Zone List */
  UIM_CDMA_MAXPUZL,             /* Maximum PUZL */
  UIM_CDMA_MECRP,               /* ME-specific Config REquest Param */
  UIM_CDMA_HRPDCAP,             /* HRPD Access Auth Capability Param */
  UIM_CDMA_HRPDUPP,             /* HRPD Access Auth User Profile Param */
  UIM_CDMA_CSSPR,               /* CUR_SSPR_P_REV */
  UIM_CDMA_ATC,                 /* Acceess Terminal Class */
  UIM_CDMA_EPRL,                /* Extended Preffered Roaming List */
  UIM_CDMA_BCSMS_CONFIG,        /* Broadcast SMS Configuration */
  UIM_CDMA_BCSMS_PREF,          /* Broadcast SMS Preferences */

  /* (0x1)3C => 60 */
  UIM_CDMA_BCSMS_TABLE,         /* Broadcast SMS Table */
  UIM_CDMA_BCSMS_PARAMS,        /* Broadcast SMS Paramaters */
  UIM_CDMA_MMS_NOTIF,           /* MMS Notification */
  UIM_CDMA_MMS_EXT8,            /* MMS Extension 8 */
  UIM_CDMA_MMS_ICP,             /* MMS Issuer Connectivity Parameters */
  UIM_CDMA_MMS_UP,              /* MMS User Preferences */
  UIM_CDMA_SMS_CAP,             /* SMS Capabilities */
  UIM_CDMA_3GPD_IPV6CAP,        /* IPv6 Capabilities */
  UIM_CDMA_3GPD_MIPFLAGS,       /* Mobile IP Flags */
  UIM_CDMA_3GPD_TCPCONFIG,      /* TCP Configurations */

  /* (0x1)46 => 70 */
  UIM_CDMA_3GPD_DGC,            /* Data Generic Configurations */
  UIM_CDMA_BROWSER_CP,          /* Browser Connectivity Parameters */
  UIM_CDMA_BROWSER_BM,          /* Browser Bookmarks */
  UIM_CDMA_3GPD_SIPUPPEXT,      /* Simple IP User Profile Parameters Extension */
  UIM_CDMA_MMS_CONFIG,          /* MMS Configuration */
  UIM_CDMA_JAVA_DURL,           /* Java Download URL */
  UIM_CDMA_3GPD_MIPUPPEXT,      /* Mobile IP User Profile Parameters Extension */
  UIM_CDMA_BREW_DLOAD,          /* BREW Download */
  UIM_CDMA_BREW_TSID,           /* BREW Teleservice ID */
  UIM_CDMA_BREW_SID,            /* BREW Subscriber ID */

  /* (0x1)50 => 80 */
  UIM_CDMA_LBS_XCONFIG,         /* LBS XTRA Configuration */
  UIM_CDMA_LBS_XSURL,           /* LBS XTRA Server URLs */
  UIM_CDMA_LBS_V2CONFIG,        /* LBS V2 Configuration */
  UIM_CDMA_LBS_V2PDEADDR,       /* LBS V2 PDE Address */
  UIM_CDMA_LBS_V2MPCADDR,       /* LBS V2 MPC Address */
  UIM_CDMA_BREW_AEP,            /* BREW Application Execution Policy */
  UIM_CDMA_MODEL,               /* Device Model information */
  UIM_CDMA_RC,                  /* Root Certificate */
  UIM_CDMA_APP_LABELS,          /* Application Labels*/
  UIM_CDMA_USER_AGENT_STRING,   /* JCDMA Agent String */

  /* (0x1)5A => 90 */
  UIM_CDMA_GID2,                /* JCDMA GID2 */
  UIM_CDMA_GID1,                /* JCDMA GID1 */
  UIM_CDMA_GID_REFERENCES,      /* JCDMA GID References */
  UIM_CDMA_IMSI_STATUS,         /* JCDMA IMSI Status */
  UIM_CDMA_ME_DOWNLOADABLE_DATA, /* JCDMA ME Downloadable Data */
  UIM_CDMA_ME_SETTING_DATA,     /* JCDMA ME Setting Data */
  UIM_CDMA_ME_USER_DATA,        /* JCDMA User Data */
  UIM_CDMA_RESERVED2,           /* JCDMA Reserved */
  UIM_CDMA_RESERVED1,           /* JCDMA Reserved */
  UIM_CDMA_UIM_SVC_TABLE,       /* JCDMA UIM Service Table */

  /* (0x1)64 => 100 */
  UIM_CDMA_UIM_FEATURES,        /* JCDMA UIM Features */
  UIM_CDMA_UIM_VERSION,         /* JCDMA UIM Version */
  UIM_CDMA_HRPD_HRPDUPP,        /* HRPD User Profile Parameters */
  UIM_CDMA_SF_EUIM_ID,          /* Short Form EUIMID */
  UIM_CDMA_3GPD_UPPEXT,         /* User Profile Parameters Extension */

  /* EFs in DF GSM */
  /* (0x2)00 => 0 */
  UIM_GSM_LP          = (UIM_GSM_EF_START << 8) ,  /* Language Preference */
  UIM_GSM_IMSI,            /* IMSI */
  UIM_GSM_KC,              /* Ciphering Key Kc */
  UIM_GSM_PLMN,            /* PLMN selector */
  UIM_GSM_HPLMN,           /* HPLMN search period */
  UIM_GSM_ACM_MAX,         /* ACM Maximum value */
  UIM_GSM_SST,             /* SIM Service table */
  UIM_GSM_ACM,             /* Accumulated call meter */
  UIM_GSM_GID1,            /* Group Identifier Level 1 */
  UIM_GSM_GID2,            /* Group Identifier Level 2 */

  /* (0x2)0A => 10 */
  UIM_GSM_SPN,             /* Service Provider Name */
  UIM_GSM_PUCT,            /* Price Per Unit and currency table */
  UIM_GSM_CBMI,            /* Cell broadcast message identifier selection */
  UIM_GSM_BCCH,            /* Broadcast control channels */
  UIM_GSM_ACC,             /* Access control class */
  UIM_GSM_FPLMN,           /* Forbidden PLMNs */
  UIM_GSM_LOCI,            /* Location information */
  UIM_GSM_AD,              /* Administrative data */
  UIM_GSM_PHASE,           /* Phase identification */
  UIM_GSM_VGCS,            /* Voice Group Call service */

  /* (0x2)14 => 20 */
  UIM_GSM_VGCSS,           /* Voice Group Call service status */
  UIM_GSM_VBS,             /* Voice Broadcast service  */
  UIM_GSM_VBSS,            /* Voice Broadcast service status */
  UIM_GSM_EMLPP,           /* Enhanced multi level pre-emption and priority */
  UIM_GSM_AAEM,            /* Automatic Answer for eMLPP service */
  UIM_GSM_CBMID,           /* Cell Broadcast Message id for data dload */
  UIM_GSM_ECC,             /* Emergency Call Codes */
  UIM_GSM_CBMIR,           /* Cell Broadcast Message id range selection */
  UIM_GSM_DCK,             /* De-personalization control keys */
  UIM_GSM_CNL,             /* Co-operative network list */

  /* (0x2)1E => 30 */
  UIM_GSM_NIA,             /* Network's indication of alerting */
  UIM_GSM_KCGPRS,          /* GPRS ciphering key */
  UIM_GSM_LOCIGPRS,        /* GPRS location information */
  UIM_GSM_SUME,            /* Setup Menu elements */
  UIM_GSM_PLMNWACT,        /* PLMN Selector with Access technology */
  UIM_GSM_OPLMNWACT,       /* Operator controlled PLMNWACT */
  UIM_GSM_HPLMNACT,        /* HPLNMN Access technology */
  UIM_GSM_CPBCCH,          /* CPBCCH information */
  UIM_GSM_INVSCAN,         /* Investigation PLMN Scan */
  UIM_GSM_RPLMNAT,         /* RPLMN  Last used Access Technology */

  /* (0x2)28 => 40 */
  UIM_GSM_VMWI,            /* CPHS: Voice Mail Waiting Indicator */
  UIM_GSM_SVC_STR_TBL,     /* CPHS: Service String Table */
  UIM_GSM_CFF,             /* CPHS: Call Forwarding Flag */
  UIM_GSM_ONS,             /* CPHS: Operator Name String */
  UIM_GSM_CSP,             /* CPHS: Customer Service Profile */
  UIM_GSM_CPHSI,           /* CPHS: CPHS Information */
  UIM_GSM_MN,              /* CPHS: Mailbox Number */
  UIM_GSM_ONS_SHORT,       /* CPHS: Operator Name Short String */
  UIM_GSM_PNN,             /* PLMN Network Name */
  UIM_GSM_OPL,             /* Operator PLMN List  */

/* EFs at SoLSA */
  /* (0x2)32 => 50 */
  UIM_GSM_SAI,             /* SoLSA access indicator */
  UIM_GSM_SLL,             /* SoLSA LSA list */

/* EFs at MExE Level */
  UIM_GSM_MEXE_ST,         /* MExE Service table */
  UIM_GSM_ORPK,            /* Operator Root Public Key */
  UIM_GSM_ARPK,            /* Administrator Root Public Key */
  UIM_GSM_TPRPK,           /* Third party Root public key */
  UIM_GSM_MBDN,            /* Mail Box Dialing Number*/
  UIM_GSM_EXT6,            /* Extension 6*/
  UIM_GSM_MBI,             /* Mail Box Identifier*/
  UIM_GSM_MWIS,            /* Message Wating Indication Status*/

  /* (0x2)3C => 60 */
  UIM_GSM_EXT1,            /* Extension 1*/
  UIM_GSM_SPDI,            /* Service Provider Display Information*/
  UIM_GSM_CFIS,            /* Call Forwarding Indicator Status */

/* image file */           /* not sure where they are used */
  UIM_GSM_IMG,
  UIM_GSM_IMG1INST1,
  UIM_GSM_IMG1INST2,
  UIM_GSM_IMG1INST3,
  UIM_GSM_IMG2INST1,
  UIM_GSM_IMG2INST2,
  UIM_GSM_IMG2INST3,

  /* (0x2)46 => 70 */
  UIM_GSM_IMG3INST1,
  UIM_GSM_IMG3INST2,
  UIM_GSM_IMG3INST3,

/* EFs at the telecom DF in GSM and CDMA */
  /* (0x3)00 => 0 */
  UIM_TELECOM_ADN     = (UIM_TELECOM_EF_START << 8),/* Abbrev Dialing Numbers */
  UIM_TELECOM_FDN,         /* Fixed dialling numbers */
  UIM_TELECOM_SMS,         /* Short messages */
  UIM_TELECOM_CCP,         /* Capability Configuration Parameters */
  UIM_TELECOM_ECCP,        /* Extended CCP */
  UIM_TELECOM_MSISDN,      /* MSISDN */
  UIM_TELECOM_SMSP,        /* SMS parameters */
  UIM_TELECOM_SMSS,        /* SMS Status */
  UIM_TELECOM_LND,         /* Last number dialled */
  UIM_TELECOM_SDN,         /* Service Dialling numbers */

  /* (0x3)0A => 10 */
  UIM_TELECOM_EXT1,        /* Extension 1 */
  UIM_TELECOM_EXT2,        /* Extension 2 */
  UIM_TELECOM_EXT3,        /* Extension 3 */
  UIM_TELECOM_BDN,         /* Barred Dialing Numbers */
  UIM_TELECOM_EXT4,        /* Extension 4 */
  UIM_TELECOM_SMSR,        /* SMS reports */
  UIM_TELECOM_CMI,         /* Comparison Method Information */
  UIM_TELECOM_SUME,        /* Setup Menu elements */
  UIM_TELECOM_ARR,         /* Access Rule reference */
  UIM_TELECOM_ICE_DN,      /* In Case of Emergency – Dialling Number */

  /* (0x3)14 => 20 */
  UIM_TELECOM_ICE_FF,      /* In Case of Emergency – Free Format */
  UIM_TELECOM_PSISMSC,     /* Public Service Identity of the SM-SC */

  /* EFs at DF GRAPHICS under DF Telecom */
  UIM_IMAGE,                /* Image instance data files */
  UIM_TELECOM_ICE_GRAPHICS, /* In Case of Emergency – Graphics */

  /* EFs at the DF PHONEBOOK under  DF Telecom */
  UIM_TELECOM_PBR,         /* Phone book reference file */
  UIM_TELECOM_PSC,         /* Phone book synchronization center */
  UIM_TELECOM_CC,          /* Change counter */
  UIM_TELECOM_PUID,        /* Previous Unique Identifier */

  /* EFs at the DF PHONEBOOK under  DF Multimedia */
  UIM_TELECOM_MML,         /* Multimedia Messages List */
  UIM_TELECOM_MMDF,        /* Multimedia Messages Data File */

  /* (0x3)1E => 30 */
  UIM_TELECOM_MLPL,        /* MMSS Location Assisted Priority List */
  UIM_TELECOM_MSPL,        /* MMSS System Priority List */
  UIM_TELECOM_MMSSMODE,    /* MMSS Mode Settings */

  /* EFs of USIM ADF */
  /* (0x4)00 => 0 */
  UIM_USIM_LI             = ( UIM_USIM_EF_START << 8), /* Language Indication */
  UIM_USIM_IMSI,           /* IMSI */
  UIM_USIM_KEYS,           /* Ciphering and Integrity keys */
  UIM_USIM_KEYSPS,         /* C and I keys for packet switched domain */
  UIM_USIM_PLMNWACT,       /* User controlled PLMN selector with access tech */
  UIM_USIM_UPLMNSEL,       /* UPLMN selector */
  UIM_USIM_HPLMN,          /* HPLMN search period */
  UIM_USIM_ACM_MAX,        /* ACM maximum value */
  UIM_USIM_UST,            /* USIM Service table */
  UIM_USIM_ACM,            /* Accumulated Call meter */

  /* (0x4)0A => 10 */
  UIM_USIM_GID1,           /* Group Identifier Level  */
  UIM_USIM_GID2,           /* Group Identifier Level 2 */
  UIM_USIM_SPN,            /* Service Provider Name */
  UIM_USIM_PUCT,           /* Price Per Unit and Currency table */
  UIM_USIM_CBMI,           /* Cell Broadcast Message identifier selection */
  UIM_USIM_ACC,            /* Access control class */
  UIM_USIM_FPLMN,          /* Forbidden PLMNs */
  UIM_USIM_LOCI,           /* Location information */
  UIM_USIM_AD,             /* Administrative data */
  UIM_USIM_CBMID,          /* Cell Broadcast msg identifier for data download */

  /* (0x4)14 => 20 */
  UIM_USIM_ECC,            /* Emergency call codes */
  UIM_USIM_CBMIR,          /* Cell broadcast msg identifier range selection */
  UIM_USIM_PSLOCI,         /* Packet switched location information */
  UIM_USIM_FDN,            /* Fixed dialling numbers */
  UIM_USIM_SMS,            /* Short messages */
  UIM_USIM_MSISDN,         /* MSISDN */
  UIM_USIM_SMSP,           /* SMS parameters */
  UIM_USIM_SMSS,           /* SMS Status */
  UIM_USIM_SDN,            /* Service dialling numbers */
  UIM_USIM_EXT2,           /* Extension 2 */

  /* (0x4)1E => 30 */
  UIM_USIM_EXT3,           /* Extension 3 */
  UIM_USIM_SMSR,           /* SMS reports */
  UIM_USIM_ICI,            /* Incoming call information */
  UIM_USIM_OCI,            /* Outgoing call information */
  UIM_USIM_ICT,           /* Incoming call timer */
  UIM_USIM_OCT,            /* Outgoing call timer */
  UIM_USIM_EXT5,           /* Extension 5 */
  UIM_USIM_CCP2,           /* Capability Configuration Parameters 2 */
  UIM_USIM_EMLPP,          /* Enhanced Multi Level Precedence and Priority */
  UIM_USIM_AAEM,           /* Automatic answer for eMLPP service */

  /* (0x4)28 => 40 */
  UIM_USIM_GMSI,           /* Group identity */
  UIM_USIM_HIDDENKEY,      /* key for hidden phonebook entries */
  UIM_USIM_BDN,            /* Barred dialling numbers */
  UIM_USIM_EXT4,           /* Extension 4 */
  UIM_USIM_CMI,            /* Comparison Method information */
  UIM_USIM_EST,            /* Enabled services table */
  UIM_USIM_ACL,            /* Access Point Name Control List */
  UIM_USIM_DCK,            /* De-personalization Control Keys */
  UIM_USIM_CNL,            /* Co-operative network list */
  UIM_USIM_START_HFN,      /* Init values for Hyper-frame number */

  /* (0x4)32 => 50 */
  UIM_USIM_THRESHOLD,      /* Max value of START */
  UIM_USIM_OPLMNWACT,      /* Operator controlled PLMN sel with access tech */
  UIM_USIM_OPLMNSEL,       /* OPLMN selector */
  UIM_USIM_HPLMNWACT,      /* HPLMN selector with access tech */
  UIM_USIM_ARR,            /* Access Rule reference */
  UIM_USIM_RPLMNACT,       /* RPLMN last used access tech */
  UIM_USIM_NETPAR,         /* Network parameters */
  UIM_USIM_VMWI,           /* CPHS: Voice Mail Waiting Indicator */
  UIM_USIM_SVC_STR_TBL,    /* CPHS: Service String Table */
  UIM_USIM_CFF,            /* CPHS: Call Forwarding Flag */

  /* (0x4)3C => 60 */
  UIM_USIM_ONS,            /* CPHS: Operator Name String */
  UIM_USIM_CSP,            /* CPHS: Customer Service Profile */
  UIM_USIM_CPHSI,          /* CPHS: CPHS Information */
  UIM_USIM_MN,             /* CPHS: Mailbox Number */
  UIM_USIM_ONS_SHORT,      /* CPHS: Operator Name Short String */
  UIM_USIM_PNN,            /* PLMN Netowrk Operator Name */
  UIM_USIM_OPL,            /* Operator PLMN List */

/* EFs of ORANGE DF under USIM ADF */
  UIM_USIM_ORANGE_DFS,     /* Dynamic Flags Status */
  UIM_USIM_ORANGE_D2FS,    /* Dynamic2 Flag Setting */
  UIM_USIM_ORANGE_CSP2,    /* Customer Service Profile Line2*/

  /* (0x4)46 => 70 */
  UIM_USIM_ORANGE_PARAMS,  /* EF Parmams - Welcome Message */

/*EFs at DF PHONEBOOK under USIM ADF */
  UIM_USIM_PBR,            /* Phone book reference file */
  UIM_USIM_PSC,            /* Phone book synchronization center */
  UIM_USIM_CC,             /* Change counter */
  UIM_USIM_PUID,           /* Previous Unique Identifier */

/*EFs at DF GSM under USIM ADF */

  UIM_USIM_KC,             /* GSM ciphering key Kc */
  UIM_USIM_KCGPRS,         /* GPRS ciphering key */
  UIM_USIM_CPBCCH,         /* CPBCCH information */
  UIM_USIM_INVSCAN,        /* Investigation scan */

/*EFs at DF MEXe under USIM ADF */
  UIM_USIM_MEXE_ST,        /* MExE Service table */

  /* (0x4)50 => 80 */
  UIM_USIM_ORPK,           /* Operator Root Public Key */
  UIM_USIM_ARPK,           /* Administrator Root Public Key */
  UIM_USIM_TPRPK,          /* Third party Root public key */
  UIM_USIM_MBDN,           /* Mailbox Dialing Number*/
  UIM_USIM_EXT6,           /* Extension 6 */
  UIM_USIM_MBI,            /* Mailbox Identifier*/
  UIM_USIM_MWIS,           /* Message Waiting Indication Status*/
  UIM_USIM_SPDI,           /* Service Provider Display Information*/
  UIM_USIM_SPT_TABLE,      /* Cingular TST*/
  UIM_USIM_EHPLMN,         /* Equivalent HPLMN */

  /* (0x4)5A => 90 */
  UIM_USIM_CFIS,           /* Call Forwarding Indicator Status */
  UIM_USIM_VGCSCA,         /* Voice Group Call Service Ciphering Algorithm */
  UIM_USIM_VBSCA,          /* Voice Broadcast Service Ciphering Algorithm */
  /* EFs related to Bootstrapping under USIM ADF */
  UIM_USIM_GBABP,          /* GBA BootStrapping Parameters */
  UIM_USIM_GBANL,          /* GBA NAF Derivation List Associated with
                              NAF Derivation Proc*/
/* EFs related to MBMS under USIM ADF */
  UIM_USIM_MSK,            /* MBMS Service Key */
  UIM_USIM_MUK,            /* MBMS User Key */
  /* EFs for MMS Notification */
  UIM_USIM_MMSN,           /* MMS Notification */
  UIM_USIM_MMSICP,         /* MMS Issuer connectivity parameters */
  UIM_USIM_MMSUP,          /* MMS User Preferences */

  /* (0x4)64 => 100 */
  UIM_USIM_MMSUCP,         /* MMS User connectivity parameters */
  UIM_USIM_EXT8,           /* Extension 8 */
  UIM_USIM_EPSLOCI,        /* EPS Location File*/
  UIM_USIM_EPSNSC,         /* EPS NAS Security Context */
  UIM_USIM_SPNI,           /* Service Provider Name Icon */
  UIM_USIM_PNNI,           /* PLMN Network Name Icon */
  UIM_USIM_NCP_IP,         /* Connectivity Parameters for USIM IP connections */
  UIM_USIM_PSEUDO,         /* Pseudonym */
  UIM_USIM_UPLMNWLAN,      /* User controlled PLMN selector for I-WLAN Access */
  UIM_USIM_0PLMNWLAN,      /* Operator controlled PLMN selector for I-WLAN Access */

  /* (0x4)6E => 110 */
  UIM_USIM_UWSIDL,         /* User controlled WLAN Specific IDentifier List */
  UIM_USIM_OWSIDL,         /* Operator controlled WLAN Specific IDentifier List */
  UIM_USIM_WRI,            /* WLAN Reauthentication Identity */
  UIM_USIM_HWSIDL,         /* Home I-WLAN Specific Identifier List */
  UIM_USIM_WEHPLMNPI,      /* I-WLAN Equivalent HPLMN Presentation Indication */
  UIM_USIM_WHPI,           /* I-WLAN HPLMN Priority Indication */
  UIM_USIM_WLRPLMN,        /* I-WLAN Last Registered PLMN */
  UIM_USIM_HPLMNDAI,       /* HPLMN Direct Access Indicator */
  UIM_USIM_ACSGL,          /* Allowed CSG Lists and corresponding indications */
  UIM_USIM_CSGT,           /* CSG Indication */

  /* (0x4)78 => 120 */
  UIM_USIM_HNBN,           /* Home (e)NodeB Name */
  UIM_USIM_OCSGL,          /* Operator CSG Lists and corresponding indications */
  UIM_USIM_OCSGT,          /* Operator CSG Type */
  UIM_USIM_OHNBN,          /* Operator Home (e)NodeB Name */
  UIM_USIM_EHPLMNPI,       /* Equivalnet HPLMN Presentation Indication */
  UIM_USIM_LRPLMNSI,       /* Last RPLMN Selection Indication */
  UIM_USIM_NASCONFIG,      /* NAS Configuration parameter information */
  UIM_USIM_PWS,            /* Public Warning System configuration */
  /* Efs for PKCS #15 support */
  UIM_PKCS15_ODF,          /* Object Directory File */
  UIM_PKCS15_TI,           /* Token Information File */

  /* (0x4)82 => 130 */
  UIM_PKCS15_US,           /* Unused space Information File */

  /* EFs of DCS1800 DF */
  /* (0x5)00 => 0 */
  UIM_DCS1800_IMSI        = (UIM_DCS1800_EF_START << 8) ,  /* Imsi */
  UIM_DCS1800_KC,          /* Ciphering Key Kc */
  UIM_DCS1800_PLMN,        /* PLMN selector */
  UIM_DCS1800_SST,         /* SIM Service table */
  UIM_DCS1800_BCCH,        /* Broadcast control channels */
  UIM_DCS1800_ACC,         /* Access control class */
  UIM_DCS1800_FPLMN,       /* Forbidden PLMNs */
  UIM_DCS1800_LOCI,        /* Location information */
  UIM_DCS1800_AD,          /* Administrative data */

/* EFs given by path */

  /* (0xF)00 => 0 */
  UIM_EF_BY_PATH      = (UIM_EF_PATH_START << 8),  /* File selection by path */

  UIM_USIM_ACT_HPLMN,      /*Acting HPLMN*/
  UIM_USIM_RAT,            /* Radio Access Technology Indicator */

  /* EFs for PKCS #15 */
  UIM_PKCS15_PRKDF,        /* Private Key Directory File */
  UIM_PKCS15_PUKDF,        /* Public Key Directory File */
  UIM_PKCS15_SKDF,         /* Secret Key Directory File */
  UIM_PKCS15_CDF,          /* Certificate Directory File */
  UIM_PKCS15_DODF,         /* Data Object Directory File */
  UIM_PKCS15_AODF,         /* Authentication Object Directory File */
  UIM_USIM_RPM_ENABLED_FLAG, /* RPM Enabled Flag EF */

  /* (0xF)0A => 10 */
  UIM_USIM_RPM_PARAM,      /* RPM Parameters */
  UIM_USIM_RPM_CNTR_LR,    /* RPM Leak Rate Counter */
  UIM_USIM_RPM_CNTRS,      /* RPM Counters */
  UIM_USIM_RPM_VI,         /* RPM Version Information */

  /* EFs of WIM DF */
  UIM_WIM_ODF           = (UIM_WIM_EF_START << 8), /* WIM ODF */
  UIM_WIM_TOKEN_INFO,      /* Generic token info and capabilities */
  UIM_WIM_UNUSED_SPACE,    /* Unused Space */

  /* EFs of ORANGE DF */
  UIM_ORANGE_DFS        = (UIM_ORANGE_EF_START << 8), /* Dynamic Flags Status */
  UIM_ORANGE_D2FS,         /* Dynamic2 Flag Setting */
  UIM_ORANGE_CSP2,         /* Customer Service Profile Line2*/
  UIM_ORANGE_PARAMS,       /* Welcome Message */

  UIM_CINGULAR_ACT_HPLMN      = (UIM_CINGULAR_EF_START << 8), /* Dynamic Flags Status */ /*Acting HPLMN*/
  UIM_CINGULAR_SPT_TABLE,      /*Support Table*/
  UIM_RPM_ENABLED_FLAG,        /* RPM Enabled Flag EF */
  UIM_RPM_PARAM,               /* RPM Parameters */
  UIM_RPM_CNTR_LR,             /* RPM Leak Rate Counter */
  UIM_RPM_CNTRS,               /* RPM Counters */
  UIM_RPM_VI,                  /* RPM Version Information */

  /* EFs of ISIM ADF */
  UIM_ISIM_IMPI             = ( UIM_ISIM_EF_START << 8), /* IMS private user identity */
  UIM_ISIM_DOMAIN,          /* Home Network Domain Name */
  UIM_ISIM_IMPU,            /* IMS public user identity */
  UIM_ISIM_ARR,             /* Access Rule Reference */
  UIM_ISIM_KEYS,            /* Ciphering and Integrity Keys for IMS */
  UIM_ISIM_AD,              /* Administrative Data */

  UIM_HZI_HZ                = (UIM_HZI_EF_START << 8),
  UIM_HZI_CACHE1,              /*UHZI-Cache1*/
  UIM_HZI_CACHE2,              /*UHZI-Cache2*/
  UIM_HZI_CACHE3,              /*UHZI-Cache3*/
  UIM_HZI_CACHE4,              /*UHZI-Cache4*/
  UIM_HZI_SUBSCRIBED_LAC_CI_1, /*UHZI-Subscribed_Lac_CI_1*/
  UIM_HZI_SUBSCRIBED_LAC_CI_2, /*UHZI-Subscribed_Lac_CI_2*/
  UIM_HZI_SUBSCRIBED_LAC_CI_3, /*UHZI-Subscribed_Lac_CI_3*/
  UIM_HZI_SUBSCRIBED_LAC_CI_4, /*UHZI-Subscribed_Lac_CI_4*/
  UIM_HZI_TAGS,                /*UHZI- Tags*/

  UIM_HZI_UHZI_SETTINGS,       /*UHZI- Settings*/

  /* EF under RAN DF in MediaFlo Application */
  UIM_MFLO_RAN_RFR    = (UIM_MFLO_EF_START << 8),   /* RAN File Revision */
  UIM_MFLO_RAN_RF,                                  /* Radio Frequency */
  /* EF under MFAPP DF in MediaFlo Application */
  UIM_MFLO_MFAPP_MFR,                               /* MediaFLO application File Revision */
  UIM_MFLO_MFAPP_BCSID,                             /* Billing and Customer Service provider ID */
  UIM_MFLO_MFAPP_MFT,                               /* MediaFLO Feature Table */
  UIM_MFLO_MFAPP_SUBTYPE,                           /* Subcriber Type */
  UIM_MFLO_MFAPP_SUBPROF,                           /* Subcriber Profile */
  UIM_MFLO_MFAPP_UTSD,                              /* Usage Tracking Service Data */
  UIM_MFLO_MFAPP_UIP,                               /* UI Preference */
  UIM_MFLO_MFAPP_PCSET,                             /* Parental Control Setting */

  UIM_MFLO_MFAPP_MCHSET,                            /* Multi-presentation view Channel Setting */
  UIM_MFLO_MFAPP_RFUD,                              /* Reserved for Future User Data */

 /* EFs in CSIM ADF*/
  /* (0xC)00 => 0 */
  UIM_CSIM_ARR        = (UIM_CSIM_EF_START << 8),   /* Access Rule Reference */
  UIM_CSIM_CALL_COUNT,               /* Call Count */
  UIM_CSIM_IMSI_M,                   /* IMSI_M */
  UIM_CSIM_IMSI_T,                   /* IMSI_T */
  UIM_CSIM_TMSI,                     /* TMSI */
  UIM_CSIM_ANALOG_HOME_SID,          /* Analog Home SID */
  UIM_CSIM_ANALOG_OP_PARAMS,         /* Analog Operational Parameters */
  UIM_CSIM_ANALOG_LOCN_AND_REGN_IND, /* Analog Locn and Registration Indicators */
  UIM_CSIM_CDMA_HOME_SID_NID,        /* CDMA Home SID and NID */
  UIM_CSIM_CDMA_ZONE_BASED_REGN_IND, /* CDMA Zone Based Registration Indicators */

  /* (0xC)0A => 10 */
  UIM_CSIM_CDMA_SYS_REGN_IND,        /* CDMA System/Network Regn Indicators */
  UIM_CSIM_CDMA_DIST_BASED_REGN_IND, /* CDMA Distance Based Registration Indicators */
  UIM_CSIM_ACCOLC,                   /* Access Overload Class */
  UIM_CSIM_CALL_TERM_MODE_PREF,      /* Call Termination Mode Preferences */
  UIM_CSIM_SSCI,                     /* Suggested Slot Cycle Index */
  UIM_CSIM_ANALOG_CHAN_PREF,         /* Analog Channel Preferences */
  UIM_CSIM_PRL,                      /* Preferred Roaming List */
  UIM_CSIM_RUIM_ID,                  /* Removable UIM ID */
  UIM_CSIM_CST,                      /* CSIM Service Table */
  UIM_CSIM_SPC,                      /* Service Programming Code */

  /* (0xC)14 => 20 */
  UIM_CSIM_OTAPA_SPC_ENABLE,         /* OTAPA/SPC-Enable */
  UIM_CSIM_NAM_LOCK,            /* NAM-Lock */
  UIM_CSIM_OTASP_OTAPA_FEATURES,/* OTASP-OTAPA Parameters */
  UIM_CSIM_SERVICE_PREF,        /* Service Preferences */
  UIM_CSIM_ESN_ME,              /* ESN_ME */
  UIM_CSIM_UNASSIGNED_1,        /* Reserved 1 */
  UIM_CSIM_LANG_IND,            /* Language Indication */
  UIM_CSIM_FDN,                 /* Fixed Dialling Number */
  UIM_CSIM_SMS,                 /* Short Messages */
  UIM_CSIM_SMS_PARAMS,          /* Short Message Service Parameters */

  /* (0xC)1E => 30 */
  UIM_CSIM_SMS_STATUS,          /* SMS Status */
  UIM_CSIM_SSFC,                /* Sup Services Feature Code Table */
  UIM_CSIM_SPN,                 /* CDMA Home Service Provider Name */
  UIM_CSIM_USGIND,              /* UIM_ID/SF_EUIMID Usage Indicator */
  UIM_CSIM_AD,                  /* CDMA Administrative Data */
  UIM_CSIM_MSISDN,              /* Mobile Directory Number */
  UIM_CSIM_MAXIMUM_PRL,         /* Max size of the PRL */
  UIM_CSIM_SPC_STATUS,          /* SPC Status */
  UIM_CSIM_ECC,                 /* Emergency Call Codes */
  UIM_CSIM_3GPD_ME3GPDOPC,      /* 3GPD Operational Capabilities */

  /* (0xC)28 => 40 */
  UIM_CSIM_3GPD_3GPDOPM,        /* 3GPD Operational Mode */
  UIM_CSIM_3GPD_SIPCAP,         /* 3GPD Simple IP Capabilities */
  UIM_CSIM_3GPD_MIPCAP,         /* 3GPD Mobile IP Capabilities */
  UIM_CSIM_3GPD_SIPUPP,         /* 3GPD Simple IP User Profile Parameters */
  UIM_CSIM_3GPD_MIPUPP,         /* 3GPD Mobile IP User Profile Parameters */
  UIM_CSIM_3GPD_SIPSP,          /* 3GPD Simple IP Status Parameters */
  UIM_CSIM_3GPD_MIPSP,          /* 3GPD Mobile IP Status Parameters */
  UIM_CSIM_3GPD_SIPPAPSS,       /* 3GPD Simple IP PAP SS Parameters */
  UIM_CSIM_UNASSIGNED_2,        /* Reserved 2 */
  UIM_CSIM_UNASSIGNED_3,        /* Reserved 3 */

  /* (0xC)32 => 50 */
  UIM_CSIM_PUZL,                /* Preffered User Zone List */
  UIM_CSIM_MAXPUZL,             /* Maximum PUZL */
  UIM_CSIM_MECRP,               /* ME-specific Config REquest Param */
  UIM_CSIM_HRPDCAP,             /* HRPD Access Auth Capability Param */
  UIM_CSIM_HRPDUPP,             /* HRPD Access Auth User Profile Param */
  UIM_CSIM_CSSPR,               /* CUR_SSPR_P_REV */
  UIM_CSIM_ATC,                 /* Acceess Terminal Class */
  UIM_CSIM_EPRL,                /* Extended Preffered Roaming List */
  UIM_CSIM_BCSMS_CONFIG,        /* Broadcast SMS Configuration */
  UIM_CSIM_BCSMS_PREF,          /* Broadcast SMS Preferences */

  /* (0xC)3C => 60 */
  UIM_CSIM_BCSMS_TABLE,         /* Broadcast SMS Table */
  UIM_CSIM_BCSMS_PARAMS,        /* Broadcast SMS Paramaters */
  UIM_CSIM_BAKPARA,             /* Currently Used BAK Paramaters */
  UIM_CSIM_UPBAKPARA,           /* Updated BAK Paramaters */
  UIM_CSIM_MMSN,                /* MMS Notification */
  UIM_CSIM_MMS_EXT8,            /* MMS Extension 8 */
  UIM_CSIM_MMS_ICP,             /* MMS Issuer Connectivity Parameters */
  UIM_CSIM_MMS_UP,              /* MMS User Preferences */
  UIM_CSIM_MMS_UCP,             /* MMS User Connectivity Parameters */
  UIM_CSIM_AUTH_CAP,            /* Authentication Capability */

  /* (0xC)46 => 70 */
  UIM_CSIM_3GCIK,               /* 3G Cipher and Integrity Keys */
  UIM_CSIM_DCK,                 /* De-Personalization Control Keys */
  UIM_CSIM_GID1,                /* JCDMA GID1 */
  UIM_CSIM_GID2,                /* JCDMA GID2 */
  UIM_CSIM_CNL,                 /* CDMA Co-operative Network List */
  UIM_CSIM_HOME_TAG,            /* Home System Tag */
  UIM_CSIM_GROUP_TAG,           /* Group Tag List */
  UIM_CSIM_SPECIFIC_TAG,        /* Specific Tag List */
  UIM_CSIM_CALL_PROMPT,         /* Call Prompt List */
  UIM_CSIM_SF_EUIM_ID,          /* Short Form EUIMID */

  /* (0xC)50 => 80 */
  UIM_CSIM_EST,                 /* Enabled Service Table */
  UIM_CSIM_HIDDEN_KEY,          /* Key for Hidden Phone Book Entry */
  UIM_CSIM_LCS_VERSION,         /* LCS Protocol Version */
  UIM_CSIM_LCS_CP,              /* LCS Connectivity Parameter */
  UIM_CSIM_SDN,                 /* Service Dialling Numbers */
  UIM_CSIM_EXT2,                /* Extension 2 */
  UIM_CSIM_EXT3,                /* Extension 3 */
  UIM_CSIM_ICI,                 /* Incoming Call Information */
  UIM_CSIM_OCI,                 /* Outgoing Call Information */
  UIM_CSIM_EXT5,                /* Extension 5 */

  /* (0xC)5A => 90 */
  UIM_CSIM_CCP2,                /* Capability Configuration Parameters 2 */
  UIM_CSIM_APP_LABELS,          /* Application Labels */
  UIM_CSIM_DEV_MODEL,           /* Device Model Information */
  UIM_CSIM_ROOT_CERT,           /* Root Certificate */
  UIM_CSIM_SMS_CAP,             /* SMS Capabilities */
  UIM_CSIM_3GPD_MIPFLAGS,       /* Mobile IP Flags */
  UIM_CSIM_3GPD_SIPUPPEXT,      /* SimpleIP User Profile Parameters Extension */
  UIM_CSIM_3GPD_MIPUPPEXT,      /* MobileIP User Profile Parameters Extension */
  UIM_CSIM_3GPD_IPV6CAP,        /* IPv6 Capabilities */
  UIM_CSIM_3GPD_TCPCONFIG,      /* TCP Configurations */

  /* (0xC)64 => 100 */
  UIM_CSIM_3GPD_DGC,            /* Data Generic Configurations */
  UIM_CSIM_BROWSER_CP,          /* Browser Connectivity Parameters */
  UIM_CSIM_BROWSER_BM,          /* Browser Boookmarks */
  UIM_CSIM_MMS_CONFIG,          /* MMS Configuration */
  UIM_CSIM_JDL,                 /* Java Download URL */
  UIM_CSIM_PBR,                 /* Phone book reference file */
  UIM_CSIM_PSC,                 /* Phone book synchronization center */
  UIM_CSIM_CHANGE_COUNTER,      /* Change counter */
  UIM_CSIM_PUID,                /* Previous Unique Identifier */
  UIM_CSIM_LBS_XCONFIG,         /* LBS XTRA Configuration */

  /* (0xC)6E => 110 */
  UIM_CSIM_LBS_XSURL,           /* LBS XTRA Server URLs */
  UIM_CSIM_LBS_V2CONFIG,        /* LBS V2 Configuration */
  UIM_CSIM_LBS_V2PDEADDR,       /* LBS V2 PDE Address */
  UIM_CSIM_LBS_V2MPCADDR,       /* LBS V2 MPC Address */
  UIM_CSIM_BREW_DLOAD,          /* BREW Download */
  UIM_CSIM_BREW_TSID,           /* BREW Teleservice ID */
  UIM_CSIM_BREW_SID,            /* BREW Subscriber ID */
  UIM_CSIM_BREW_AEP,            /* BREW Application Execution Policy */
  UIM_CSIM_EXT_ME_SETTING_DATA, /* JCDMA Extended ME Setting Data */
  UIM_CSIM_USER_AGENT_STRING,   /* JCDMA User Agent String */

  /* (0xC)78 => 120 */
  UIM_CSIM_IMSI_STATUS,         /* JCDMA IMSI Status */
  UIM_CSIM_ME_DOWNLOADABLE_DATA,/* JCDMA ME Downloadable Data */
  UIM_CSIM_ME_SETTING_DATA,     /* JCDMA ME Setting Data */
  UIM_CSIM_ME_USER_DATA,        /* JCDMA ME User Data */
  UIM_CSIM_UIM_SVC_TABLE,       /* JCDMA UIM Service Table */
  UIM_CSIM_UIM_FEATURES,        /* JCDMA UIM Features */
  UIM_CSIM_UIM_VERSION,         /* JCDMA Signature and Version */
  UIM_CSIM_3GPD_UPPEXT,         /* User Profile Parameters Extension */

  UIM_NO_SUCH_ITEM         /* No such item */
} uim_items_enum_type;

/* Return type structure for service allocation and activation */
typedef struct {
  boolean allocated;    /* If the service is activated */
  boolean activated;    /* If the service is allocated */
} uim_svc_table_return_type;

/* CDMA service table service items */
typedef enum {
  UIM_CDMA_SVC_NO_SUCH_ITEM,
  UIM_CDMA_SVC_CHV_DISABLE,
  UIM_CDMA_SVC_ADN,
  UIM_CDMA_SVC_FDN,
  UIM_CDMA_SVC_SMS,
  UIM_CDMA_SVC_EXT1,
  UIM_CDMA_SVC_EXT2,
  UIM_CDMA_SVC_SMS_PARAMS,
  UIM_CDMA_SVC_LND,
  UIM_CDMA_SVC_HOME_SVC_PROVIDER_NAME,
  UIM_CDMA_SVC_SDN,
  UIM_CDMA_SVC_EXT3,
  UIM_CDMA_SVC_SMS_PP_DOWNLOAD,
  UIM_CDMA_SVC_MENU_SEL,
  UIM_CDMA_SVC_PROACTIVE_UIM,
  UIM_CDMA_SVC_3GPD_SIP,
  UIM_CDMA_SVC_3GPD_MIP,
  UIM_CDMA_SVC_3GPD_MSG_EXT,
  UIM_CDMA_SVC_BCSMS,
  UIM_CDMA_SVC_HRPD,
  UIM_CDMA_SVC_RC,
  UIM_CDMA_SVC_BROWSER,
  UIM_CDMA_SVC_JAVA,
  UIM_CDMA_SVC_BREW,
  UIM_CDMA_SVC_LBS,
  UIM_CDMA_IPV6,
  UIM_CDMA_SVC_MAX_ITEM
}uim_cdma_svc_table_enum_type;

/* Indicate command handling options */
typedef enum {
  UIM_NONE          = 0x00, /* Disabled */
  UIM_AUTH          = 0x01, /* uim power-down bit for Auth task */
  UIM_MC            = 0x02, /* uim power-down bit for MC task */
  UIM_PROACTIVE_UIM = 0x4,  /* power up bit if the UIM is pro active */
  UIM_UI            = 0x8,  /* uim power down bit for UI task */
  UIM_UICC_SESSION  = 0x10,   /* bit indicating a UICC session */
  UIM_USIM_SESSION  = UIM_UICC_SESSION, /* Replaced USIM with UICC voter type */
  UIM_SESSION       = 0x20, /* any module that wants to execute a series of cmds
                               without powering down between commands */
  UIM_TC            = 0x40, /* uim powerdown bit for traffic channel */
  UIM_CDMA_TC       = 0x80, /* uim voting for CDMA traffic channel */
  UIM_GW_TC         = 0x100,/* uim voting for GSM/WCDMA traffic channel */
  UIM_PASSIVE       = 0x200 /* uim voting for presence detection in passive mode */
} uim_voter_type;

typedef enum {
  UIM_NO_SUCH_SRCH_MODE   = 0,   /* No such search mode */
  UIM_SIMPLE_SRCH         = 1,   /* Simple search */
  UIM_ENHANCED_SRCH       = 2,   /* Enhanced Search */
  UIM_MAXIMUM_SRCH_MODE   = 3
} uim_srch_mode;

typedef enum {
  UIM_SRCH_FORWARD   = UIM_P2_SRCH_FORWARD,  /* Search forward */
  UIM_SRCH_BACKWARD  = UIM_P2_SRCH_BACKWARD  /* Search backward */
} uim_srch_dir_type;

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================

FUNCTION UIM_RETURN_CDMA_SVC_AVAILABILITY

DESCRIPTION
  This function checks availabilities for a specified service.

DEPENDENCIES
  The CDMA service table has to be read first.

RETURN VALUE
  TRUE :   The service is activated.
  FALSE:   The service is unavailable.

SIDE EFFECTS
  None.

===========================================================================*/
extern uim_svc_table_return_type uim_return_cdma_svc_availabililty (
 uim_cdma_svc_table_enum_type item,  /* The service to be checked*/
 const byte                   *table /* Pointer to the service table */
);


/*===========================================================================

FUNCTION UIM_GET_ESN_ME

DESCRIPTION
  This function returns the HW ME ESN cached by TMC at power up.

DEPENDENCIES
  None

RETURN VALUE
  The HW ME ESN.  If the ESN was not cached properly, 0 is returned

SIDE EFFECTS
  None

===========================================================================*/
extern uint32 uim_get_esn_me (
  void
);

/*===========================================================================

FUNCTION UIM_POWER_CONTROL

DESCRIPTION
  This procedure changes the state of UIM.

DEPENDENCIES
  Time delay to be used.

RETURN VALUE
  None.

SIDE EFFECTS
  Internally sets the UIM_TOGLLE_STATE_SIG

===========================================================================*/
extern uint32 uim_power_control
(
  uim_voter_type    mask,
  boolean           uim_reqd
);

#endif /* UIM_H */
