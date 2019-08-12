
#ifndef SYS_H
#define SYS_H

/*============================================================================

              S Y S T E M   H E A D E R   F I L E

============================================================================*/

/**
  @file sys.h

  Definitions that are shared between the call manager, its clients,
  and the protocol stacks.
*/
/*
  Organization of the file:

  The file is divided into multiple sections. Each section contains
  type definitions for a particular entity. The following entities
  are defined in this file:

  - Service Indicators
  - System Identifiers
  - Network Lists
  - Mobility Management Information
  - Menu Selection
  - BCMCS

  Each section is further subdivided to represent different types of
  information. The types of information include:

  - #defines
  - enumerated types
  - data types
  - functions

  @note
  Please do not change the order of the enumerations.  If a new
  enumeration value is needed, add to the end of the enumerated
  type.  Other code may be dependent on the existing order.
*/
/*===========================================================================
Copyright (c) 2002-2015 by Qualcomm Technologies INCORPORATED. All Rights Reserved.
Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/public/sys.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/28/14   ss      eMBMS support for QMI
10/25/13   sm      Add QMI HO type for G<->W
10/3/2013  vs      Adding LTE band 27
09/12/13   fj      Added macro to map as_id to inst_id.
07/24/13   vs      Adding LTE band 30
05/21/13   fj      Buffer the LTE PWR_SAVE status on hybr_2 stack.
01/07/12   xs      FR 2972: Enhance QMI to support domain selection preference status 
                   updates 
02/13/13   vs      Adding LTE band 28 and 29
01/07/12   xs      Added PS supplemetary service support
10/17/12   fj      Added as_sys_mode and tac in sys_cell_info_s_type.
07/19/12   jh      Added trace_id for embms_info
08/24/12   fj      Adding LTE band 26.
08/08/11   fj      Adding LTE band 23
20/07/12   fj      Modify SYS_MAX_PRIMARY_PDP_CONTEXTS to 4.
06/05/12   sg      API changes for SGLTE requirement
03/01/12   fj      Added support for DRX.
03/08/12   xs      Added codec info support for VOLTE
03/06/12   jh      Added eMBMS coverage status indication handling
11/30/11   jh      Added feature for dynamic rat acq order change
01/24/12   fj      Add support for EUTRA detection status.
12/20/11   sg      Adding new mode_pref TDS_GSM_WCDMA_LTE / TDS_GSM_WCDMA
12/02/11   vk      Added PS voice only support to Domain selection module.
11/04/11   jh      Added new enum for lte div_duplex
09/15/11   vk      Added "LIST_REJ_IN_RLF" cause to sys_plmn_list_status_e_type
09/20/11   rk      Used to covey that CS registation failed for max attempts
09/09/11   mj      TD-SCDMA defines
09/07/11   gm      Correct misleading comments for sys_cell_access_status_e_type
08/18/11   sn      Added 64QAM HSDPA+/HSUPA indication enum
08/05/11   xs      Added SMS domain preference definition
08/05/11   xs      Added EPS network feature support definition (PS voice indication)
06/21/11   rk      Define UE voice domain preference
05/17/11   xs      Adding LTE band 24-25
04/14/11   rm      Cell broadcast info changes
03/29/11   rn      Send NAS procedure during reg reject.
03/23/11   xs      Added LTE band 41-43
03/21/11   pm      Revert SYS_MAX_PRIMARY_PDP_CONTEXTS to 3 due to GSM data call failure.
03/04/11   fj      Updated SYS_MAX_PRIMARY_PDP_CONTEXTS to 4.
02/15/11   rk      CM interface for indicating UE mode of operation on LTE
                   network to CM clients and UE usage setting to NAS.
02/07/11   rm      Adding lac_id in sys_mm_information_s_type
01/19/11   rm      sys_modem_as_id_e_type related changes
01/18/11   am      Added UMTS BC19-850 band support in CM
12/22/10   sv      Adding DSDS changes
11/19/10   rm      Adding DSAC changes
11/11/10   kl/gm   (Tech Pubs) Edited/added Doxygen comments and markup.
11/03/10   rm      Adding WLAN security info interface
09/29/10   aj      support for DO LTE reselection
09/23/10   cl      Adding support for uplink/downlink lte cell info indication
09/16/10   rm      Adding hopping status changes
08/08/10   chl     Updated WLAN profile size
08/02/10   fj      Add LTE BAND MASK for LTE band 18,19,20,21.
07/14/10   xs      Add SENTINEL metacomments for maximum value sentinel in the enum
07/13/10   fj      Modify SYS_SBAND_LTE_EUTRAN_BAND_MAX value.
07/07/10   fj      Add LTE band 18,19,20,21.
06/15/10   aj      Support for IRAT measurement
06/14/10   aj      Adding support for passing HDR personality info
06/11/10   ay      Added support for EOOS
06/02/10   aj/pm   Add BST support for rat priority list
04/26/10   rm      Adding Modem Statistics changes for 1x/HDR
03/19/10   aj      Add support for MMSS 3.1
03/15/10   sg      Added UMTS BC11-1500 band support in CM
03/10/10   rm      Adding DC HSDPA_PLUS enums
01/25/10   cl      Added support for blocking certain pages during emergency
                   callback mode
11/21/09   rn      Corrected Tab - space
11/18/09   rm      Adding NMR/MRL support
10/22/09   rm      Adding Modem Statistics changes
10/19/09   np      Added Acqusition priority list data structure
10/12/09   ay      Added LTE band information in the band class enum
09/17/09   np      Added No Service cause enum type.
08/13/09   fj      Added SYS_MAX_EPS_BEARERS_CONTEXTS.
12/22/08   fj      Add mode and band support for LTE technology
09/10/09   rm      Removing inclusion of customer.h from /api files
09/07/09   rm      Moving SYS_BAND_MASK_* from sys_v.h to sys.h
08/07/09   rm      Adding HSDPA_PLUS enums
08/07/09   rm      CMI phase-2: Removing featurisation from /api files
07/13/09   aj      Add placeholders for BC 13 and CDMA BC MAX
07/01/09   rm      Moving few defintions from sys_v.h to here
06/10/09   ks      Adding more fields into camped cell info structures
05/27/09   aj      Adding support for updating clients with cdma local time
03/16/09   aj      HDR Rev B support added
02/23/09   rm      Added sys_band_mask_e_type from sys_v.h
02/11/09   ks      Add spare_bits field in the mm_information to indicate
                   no. of spare bits in end octet of n/w names(full/short)
12/08/08   fj      Added SYS_BAND_MASK_ANY
12/15/08   jd      Added Doxygen commenting for customer documentation
11/28/08   rm      CMI defeaturisation
10/02/08   aj      Add support for BC 17,18,19
09/24/08   jd      Removed duplicate entry of uint32 sys_bcmcs_flow_id_type
09/17/08   ks      When OOS in CELL_FACH, UE needs to follow UOOS scanning
                   pattern to re-acquire service
09/12/08   fj/aj   Support Android RIL for ARM9 Reset
09/04/08   ks      Remove info for Neighbors Cell IDs
08/19/08   sv      Increased SYS_NETWORK_LIST_INFO_MAX to 25
                   Added security info as part of sys_wlan_bss_info_s_type
07/18/08   ks      Added new info for Current Operator's Cell ID and
                   Neighbors Cell IDs
07/01/08   rm      Mainlining of MBMS changes
06/18/08   sn      Adding new enum - sys_gw_conn_status_e_type
06/16/08   rk      Adding FEATURE_MM_SUPERSET to remaining feature flags
05/06/08   ak      BCMCS 2.0 changes
04/30/08   cl      Adding support for API sd_misc_sys_avoid_list, which list all
                   the avoided systems. HDR/SCHR use this info to update their
                   scan list when doing OFS.
03/27/08   rn/ic   Added support for new command CM_PH_STATUS_CHGD_REQ
03/14/08   ks      Updating UI with service status when UE is OOS in Connected Mode
12/10/07   sju     Renamed HDR system type enum
11/13/07   ak/sju  Added support for HDR revB
08/08/07   cl      Add OTASP Support for ASW Band
06/22/07   sk      Added bit manipulation macros.
06/07/07   rp      Added support for EGPRS indication
06/06/07   pk/cl   Added support for LN status notification from HDR
05/22/07   rp      Changing size of SYS_USER_PPLMN_LIST_MAX_LENGTH
03/09/07   sk      RAM usage reduction.
03/07/07   jqi     Added support for FEATURE_SMS_OVER_IP
01/03/07   pk      Added support for DO VOIP -> 1X Handoff
08/14/06   ka      Adding persistent enum for band pref.
12/13/06   rp      Adding sys_service_search_e_type
11/17/06   jqi     Added support for HDR active protocol revision information.
11/16/06   pk      Corrected meta comment
11/13/06   ic      Lint fixes
11/07/06   pk      Added support for HYBR_GW and UMTS -> 1X handover
11/06/06   hn/ic   Added missing meta-comments.
10/11/06   jqi     Added CDMA 450M(BC5) block L support.
09/18/06   rp      Added BC15 and BC16 support.
09/17/06   sk      Added band field to sys_wlan_manual_sys_info_s_type.
08/28/06   ka      Adding pdp header comp and data comp enums.
08/21/06   ka/rp   Changing SYS_FORBIDDEN_PLMN_LIST_MAX_LENGTH to 20 on
                   request from NAS team.
07/20/06   mpa     Fixed meta comments for sys_ip_address_s_type.
04/06/06   ka      Adding enums for HSDPA/DTM indications.
04/19/06   pk      Added meta comments for 7200
04/06/06   sk      Added new bcmcs flow status
                   - SYS_BCMCS_FLOW_STATUS_SYS_UNAVAILABLE
03/10/06   pk      Added support for high PSIST
03/10/06   pk      Added mcc and imsi info. to sys_is95_sys_id_s_type
03/05/06   jqi     Merging the code for WCDMA BC3,4,8,9 support.
02/23/06   ka      Merging code
                   01/31/06  up  SYS_PRIORITY_PLMN_LIST_MAX_LENGTH(256)
                   is added
02/03/06   mpa     Reworked sys_bcmcs_flow_status_e_type().
12/15/05   ka      Name change to SYS_PLMN_LIST_NONE
01/09/06   ic      Under FEATURE_HTORPC_METACOMMENTS, merged in meta comments
                   from //depot/asic/msm7500/orphans/00.04/services/cm/sys.h#6
                   (delta from 5 to 6)
01/06/06   ic      Lint cleanup
01/05/06   pk      Added numbering for enums
12/22/05   jqi     Merged background PLMN search support.
11/30/05   ka      Increased number of secondaries to 2.
11/17/05   pk      Added support for BC 5 and BC 11 sub blocks I,J,K
10/14/05   jqi     Added BC14 support.
09/20/05   pk      Removed FEATURE_DATA_PS_UNSUPPORTED from "uint64 v6[2]"
                   addr union in sys_ip_address_s_type
08/29/05   ic      Integration of the BCMCS DS to CM to HDRMC path
06/26/05   ic      Lint and compiler warnings cleanup
05/20/05   ic      Mainlined FEATURE_IS683C_PRL
05/12/05   ic      Removed bcmcs_srv_status_is_srv()
05/11/05   ic      Added metacomments under FEATURE_HTORPC_METACOMMENTS
04/13/05   ic      Put RPC comments under FEATURE_HTORPC_METACOMMENTS
04/05/05   pk      Corrected comment for SYS_MSM_ID_SECONDARY in
                   sys_msm_id_e_type
04/04/05   ic      Added SYS_ARG_NOT_USED()
03/22/05   up      Function sys_mcc_same_country is added to extend MCC
                   matching for NA MCC range 310-316.
03/09/05   kwa     Removed sys_hplmn_match.
02/02/05   ka      Added band class 12 enums
01/10/05   dk      Added sys_plmn_lac_id_s_type and sys_lac_match().
01/06/05   dk      Increased SYS_USER_PPLMN_LIST_MAX_LENGTH to 50 and
                   set SYS_PLMN_LIST_MAX_LENGTH to 40. Removed
                   SYS_OPERATOR_PPLMN_LIST_MAX_LENGTH.
11/19/04   ic      Corrected typo in sys_sys_mode_e_type
09/08/04   jqi     More ps dormant optimization.
08/24/04   ws      Merge with std.h
08/20/04   sj      Added support for PRL Preference.
08/10/04   sj      Added sys_hplmn_match.
07/30/04   sj      Added new enum for srv_domain_camped.
07/08/04   ws      Added RPC meta comments.
06/03/04   ka      Fixed Lint warning
05/17/04   ic      Added type field in sys_ip_address_s_type and
                   sys_ip_addr_enum_type enum (FEATURE_BCMCS)
05/06/04   jqi     Renamed T32, KPCS, BC5,6 with band mapping features.
                   Increased the size of sys_band_mask_type to 64 bits.
05/06/04   ic      Added commas in sys_band_mask_e_type after UMTS 1700.850,
                   800 band support enums
05/03/04   jqi     Added UMTS 1700.850,800 band support.
04/12/04   ws      Fixed typo.
03/17/04   sj      Added new DDTM related typedefs.
03/02/04   ic      Added BCMCS support (FEATURE_BCMCS)
02/06/04   ws      Initial jaguar/mainline merge.
01/28/04   ka      Added multiple PDP context constants
11/24/03   jqi     Added support for SYS_OPRT_MODE_OFFLINE_IF_NOT_FTM.
01/22/04   prk     Removed extra comma in sys_band_mask_e_type.
10/24/03   ws      Added jqi's changes for band preference enum.
10/24/03   ic      Modified to support common sys.h across targets
10/20/03   ws      Added new limited_regional service status type.
09/30/03   vt      Modified to support common cm.h across all targets.
07/10/03   jqi     Added Extended PRL support
07/01/03   az      Added FEATURE_ENHANCED_BAND_CLASS
05/28/03   vt      Defined sys bitmask macro and sband enum.
05/05/03   kwa     Updated the logic in sys_mm_information_is_valid for
                   determining if an sys_mm_information_s_type is valid to
                   account for optional elements within the structure.
                   Modified several functions to pass pointers rather than
                   structures to minimize stack utilization.
02/14/03   vt      Assigned fix numbers to operating mode enums.
02/03/02   prk     Removed references to SYS_ACT_UNKNOWN_MASK.
01/21/03   ws      Updated copyright information for 2003
12/17/02   vt      Defined fixed numbers to an enums for event logging.
11/27/02   vt      Changed sys_jcdma_m513_e_type back to what it was before.
11/22/02   MKS     Defined data type sys_network_name_coding_scheme_type_e_type
                   for indicating the network name coding scheme. Added
                   coding_scheme & add_country_initials as members of
                   sys_network_short_name_s_type & sys_network_full_name_s_type
11/04/02   ic      Changes to sys_jcdma_m513_e_type
10/22/02   ic      Added enumerations for M51 mode settings
09/03/02   PRK     Added NOTE section in funtion header. Removed enumerator
                   tags to prevent lint warnings.
06/11/02   vt      Restored the original order to operating mode enum. All
                   the new additions to the enums were added at the end.
05/16/02   PRK     Changed constants SYS_USER_PPLMN_LIST_MAX_LENGTH,
                   SYS_OPERATOR_PPLMN_LIST_MAX_LENGTH and
                   SYS_FORBIDDEN_PLMN_LIST_MAX_LENGTH from 64 to 10.
05/02/02   PRK     Added SYS_SRV_STATUS_LIMITED to sys_srv_status_e_type.
                   Removed SYS_SRV_DOMAIN_CS_LIMITED from sys_srv_domain_e_type.
04/01/02   PRK     Initial version
===========================================================================*/

#include "comdef.h"    /* Common definitions such as byte, uint16, etc. */
#include "time_svc.h"
#include "cm_gw.h"
#include "bit_mask_256.h"

/*------------------------------------------------------------------------------
   General macros
------------------------------------------------------------------------------*/

/** @addtogroup GM General Macros
    @{ */

/** Maximum size of the WLAN profile. This should be updated
    if the data structure of net_profile_s_type is modified.
*/
#define SYS_WLAN_PROFILE_MAX_SIZE  155

/** Macro to convert a bit number to a bitmask.
*/
#define SYS_BM( val )         (1<<(int)(val))

/** Macro to convert a 64-bit bit number to a bitmask.
*/
#define SYS_BM_64BIT( val )   ((uint64)1<<(int)(val))

/** Macro to convert a 32-bit bit number to a bitmask.
*/
#define SYS_BM_32BIT( val )   (sys_tech_mask_type)(1<<(int)(val))

/** Macros to extract the low/high 32bits of a QWORD (64 bits).
*/
#define QWORD_LOW( qword )  ( (dword) ((uint64) qword & (uint64) 0xFFFFFFFF))
#define QWORD_HIGH( qword ) ( (dword) ((uint64) qword >> 32) )

/** @} */ /* end_addtogroup GM General Macros */

/** @addtogroup DDTM DDTM-Related Definitions
    @{ */

/** Maximum SO list size.
*/
#define SYS_DDTM_MAX_SO_LIST_SIZE 10

/** Maximum allowed SO list size.
*/
#define SYS_DDTM_MAX_ALLOW_SO_LIST_SIZE 37

#define SYS_PCI_SCAN_MAX_NUM_PCI 5
#define SYS_PCI_SCAN_MAX_NUM_PLMN 6

/** Service capability type provided by the protocol stack.
*/
typedef uint16 sys_srv_opt_type;

/** @} */ /* end_addtogroup DDTM */

/* If there is a new band class, append to the end of list.
*/

/** @addtogroup DDTM
    @{ */

/** System band classes and band classes' sub-bands.
*/
typedef enum {
  SYS_SBAND_BC0_A = 0,
    /**< Band Class 0, A-System. */

  SYS_SBAND_BC0_B = 1,
    /**< Band Class 0, B-System. */

    /* Band Class 0 AB , GSM 850 Band? */

  SYS_SBAND_BC1 = 2,
    /**< Band Class 1, all blocks. */

  SYS_SBAND_BC2 = 3,
    /**< Band Class 2 place holder. */

  SYS_SBAND_BC3 = 4,
    /**< Band Class 3, A-System. */

  SYS_SBAND_BC4 = 5,
    /**< Band Class 4, all blocks. */

  SYS_SBAND_BC5 = 6,
    /**< Band Class 5, all blocks. */

  SYS_SBAND_GSM_DCS_1800 = 7,
    /**< GSM Digital Cellular Standard (DCS) band. */

  SYS_SBAND_GSM_EGSM_900 = 8,
    /**< GSM Extended GSM (E-GSM) band. */

  SYS_SBAND_GSM_PGSM_900 = 9,
    /**< GSM Primary GSM (P-GSM) band. */

  SYS_SBAND_BC6 = 10,
    /**< Band Class 6. */

  SYS_SBAND_BC7 = 11,
    /**< Band Class 7. */

  SYS_SBAND_BC8 = 12,
    /**< Band Class 8. */

  SYS_SBAND_BC9 = 13,
    /**< Band Class 9. */

  SYS_SBAND_BC10 = 14,
    /**< Band Class 10. */

  SYS_SBAND_BC11 = 15,
    /**< Band Class 11. */

  SYS_SBAND_GSM_450 = 16,
    /**< GSM 450 band. */

  SYS_SBAND_GSM_480 = 17,
    /**< GSM 480 band. */

  SYS_SBAND_GSM_750 = 18,
    /**< GSM 750 band. */

  SYS_SBAND_GSM_850 = 19,
    /**< GSM 850 band. */

  SYS_SBAND_GSM_RGSM_900 = 20,
    /**< GSM Railways GSM Band. */

  SYS_SBAND_GSM_PCS_1900 = 21,
    /**< GSM PCS band. */

  SYS_SBAND_WCDMA_I_IMT_2000 = 22,
    /**< WCDMA Europe, Japan, and China IMT 2100 band. */

  SYS_SBAND_WCDMA_II_PCS_1900 = 23,
    /**< WCDMA US PCS 1900 band. */

  SYS_SBAND_WCDMA_III_1700 = 24,
    /**< WCDMA Europe and China DCS 1800 band. */

  SYS_SBAND_WCDMA_IV_1700 = 25,
    /**< WCDMA US 1700 band. */

  SYS_SBAND_WCDMA_V_850 = 26,
    /**< WCDMA US850 band. */

  SYS_SBAND_WCDMA_VI_800 = 27,
    /**< WCDMA Japan 800 band. */

  SYS_SBAND_BC12 = 28,
    /**< Band Class 12. */

  SYS_SBAND_BC14 = 29,
   /**< Band Class 14. */

  SYS_SBAND_RESERVED_2 = 30,
   /**< Reserved 2. */

  SYS_SBAND_BC15 = 31,
   /**< Band Class 15. */

  SYS_SBAND_WLAN_US_2400 = 32,
    /**< WLAN US 2400 band. */

  SYS_SBAND_WLAN_EUROPE_2400 = 33,
    /**< WLAN ETSI 2400 band. */

  SYS_SBAND_WLAN_FRANCE_2400 = 34,
    /**< WLAN France 2400 band. */

  SYS_SBAND_WLAN_SPAIN_2400 = 35,
    /**< WLAN Spain 2400 band. */

  SYS_SBAND_WLAN_JAPAN_2400 = 36,
    /**< WLAN Japan 2400 band. */

  SYS_SBAND_WLAN_US_5000 = 37,
    /**< WLAN US 2400 band. */

  SYS_SBAND_WLAN_EUROPE_5000 = 38,
    /**< WLAN Europe 5000 band. */

  SYS_SBAND_WLAN_FRANCE_5000 = 39,
    /**< WLAN France 5000 band. */

  SYS_SBAND_WLAN_SPAIN_5000 = 40,
    /**< WLAN Spain 5000 band. */

  SYS_SBAND_WLAN_JAPAN_5000 = 41,
    /**< WLAN Japan 5000 band. */

  SYS_SBAND_WCDMA_VII_2600 = 48,
    /**< WCDMA Europe 2600 band. */

  SYS_SBAND_WCDMA_VIII_900 = 49,
    /**< WCDMA Europe and Japan 900 band. */

  SYS_SBAND_WCDMA_IX_1700 = 50,
    /**< WCDMA Japan 1700 band. */

  /* 51-55 reserved for WLAN */
  SYS_SBAND_BC16 = 56,
   /**< Band Class 16. */

  SYS_SBAND_BC17 = 57,
   /**< Band Class 17. */

  SYS_SBAND_BC18 = 58,
   /**< Band Class 18. */

  SYS_SBAND_BC19 = 59,
   /**< Band Class 19. */

  SYS_SBAND_WCDMA_XIX_850 = 60,
    /**< WCDMA Japan 850 band. */

  SYS_SBAND_WCDMA_XI_1500 = 61,
    /**< WCDMA 1500 band.  */

  SYS_SBAND_PERSISTENT = 62,
    /**< Persistent value from NV. */

  /** @cond
  */
  SYS_SBAND_MAX = 63  /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

} sys_sband_e_type;
/*~ SENTINEL sys_sband_e_type.SYS_SBAND_MAX */


/** DDTM actions to enhance HDR data throughput.
*/
typedef enum
{
  SYS_DDTM_ACT_SUPPRESS_L2ACK   = 0,
    /**< Do not send L2 ACK on 1X. */

  SYS_DDTM_ACT_SUPPRESS_REG     = 1,
    /**< Suppress 1X registrations. */

  SYS_DDTM_ACT_IGN_SO_PAGES     = 2,
    /**< Ignore 1X pages with specified service options. */

  SYS_DDTM_ACT_SUPPRESS_MO_DBM  = 3,
    /**< Block MO SMS and DBM */

  SYS_DDTM_ACT_ALLOW_SO_PAGES = 4,
    /**< Allow only pages with specified service options. */

  /** @cond
  */
  SYS_DDTM_ACT_MAX   /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

} sys_ddtm_act_e_type;
/*~ SENTINEL sys_ddtm_act_e_type.SYS_DDTM_ACT_MAX */

/** DDTM action mask; converts the bit position of
    sys_ddtm_act_e_type to a bitmask.
*/
typedef enum {

  SYS_DDTM_ACT_MASK_EMPTY           = 0,
    /**< Empty action mask. */

  SYS_DDTM_ACT_MASK_SUPPRESS_L2ACK  = SYS_BM(SYS_DDTM_ACT_SUPPRESS_L2ACK),
    /**< Mask for suppressing L2 acknowledgments in 1X. */

  SYS_DDTM_ACT_MASK_SUPPRESS_REG    = SYS_BM(SYS_DDTM_ACT_SUPPRESS_REG),
    /**< Mask for suppressing 1X registrations. */

  SYS_DDTM_ACT_MASK_IGN_SO_PAGES    = SYS_BM(SYS_DDTM_ACT_IGN_SO_PAGES),
    /**< Mask for ignoring pages with specified service options. */

  SYS_DDTM_ACT_MASK_SUPPRESS_MO_DBM = SYS_BM(SYS_DDTM_ACT_SUPPRESS_MO_DBM),
    /**< Mask for blocking MO SMS and DBM. */

  SYS_DDTM_ACT_MASK_ALLOW_SO_PAGES = SYS_BM(SYS_DDTM_ACT_ALLOW_SO_PAGES),
    /**< Mask for allowing pages with specified service options. */

  SYS_DDTM_ACT_MASK_ALL =  0x3fff,
    /**< Mask for enabling all DDTM actions. */

  SYS_DDTM_ACT_MASK_NO_CHANGE,
    /**< Reserved value for not changing current DDTM action settings. */

  /** @cond
  */
  SYS_DDTM_ACT_MASK_MAX   /* For internal use. */
  /** @endcond
  */

} sys_ddtm_act_mask_e_type;
/*~ SENTINEL sys_ddtm_act_mask_e_type.SYS_DDTM_ACT_MASK_MAX */

/** @} */ /* end_addtogroup DDTM */

/** @addtogroup DDTM
    @{ */

/*------------------------------------------------------------------------------
   Multiple EPS bearer Contexts Defines
------------------------------------------------------------------------------*/

/** Total number of slots for Evolved Packet System (EPS) bearer contexts.
*/
#define SYS_MAX_EPS_BEARERS_CONTEXTS          8
/* (1 for default and 7 for dedicated) */

/** @} */ /* end_addtogroup DDTM */

/*------------------------------------------------------------------------------
   Closed Subscriber Group (CSG) related declarations
------------------------------------------------------------------------------*/

/** Closed Subscriber Group (CSG) identifier.
*/
typedef uint32 sys_csg_id_type;

/** Closed Subscriber Group (CSG) identifier value when undefined.
*/
#define SYS_CSG_ID_INVALID 0xFFFFFFFF

/** Home Node B (HNB) or Home eNode B (HeNB) name length.
*/
#define SYS_HNB_NAME_LEN 48

/** Home Node B (HNB) or Home eNode B (HeNB) name type.
 *  Though the name of the structure indicates home_nodeb,
 *  but it applies to HNB as well as HeNB.
 */
typedef struct
{
   uint8  length;
   uint8  name[SYS_HNB_NAME_LEN];
} sys_home_nodeb_name_type;

/** CSG Information structure.
 */
typedef struct sys_csg_info_s
{
  sys_csg_id_type              csg_id;
  /**< CSG Identifier */

  sys_home_nodeb_name_type     hnb_name;
  /**< This is essentially a HNB or HeNB name and is optional.*/

  boolean                      hybrid_cell;
  /**< This indicates that Femto is a hybrid cell.*/

} sys_csg_info_s_type;

/*------------------------------------------------------------------------------
   Multiple PDP Contexts Defines
------------------------------------------------------------------------------*/

/** @addtogroup MPCD Multiple PDP Context Definitions
    @{ */

/** Maximum number of primary PDP contexts.
*/
#define SYS_MAX_PRIMARY_PDP_CONTEXTS          4

/** Minimum number of primary PDP contexts.
*/
#define SYS_MIN_PRIMARY_PDP_CONTEXTS          1

/** Maximum number of secondary PDP contexts per primary PDP context.
*/
#define SYS_MAX_SEC_PDP_CONTEXT_PER_PRIMARY   2

/** Minimum number of secondary PDP contexts per primary PDP context.
*/
#define SYS_MIN_SEC_PDP_CONTEXT_PER_PRIMARY   0

/** Maximum number of PDP contexts.
*/
#define SYS_MAX_PDP_CONTEXTS                                                 \
  ( SYS_MAX_PRIMARY_PDP_CONTEXTS +                                           \
    ( SYS_MAX_PRIMARY_PDP_CONTEXTS * SYS_MAX_SEC_PDP_CONTEXT_PER_PRIMARY ))

/** Maximum size of the SSID.
*/
#define SYS_WLAN_SSID_MAX_SIZE         (int)(32)

/** Minimum size of the SSID.
*/
#define SYS_WLAN_SSID_MIN_SIZE         (int)(1)

/** Limit for the number of spare bits in the end octet of the network short
    name.
*/
#define SYS_MAX_NW_NAME_SPARE_BITS    7

/** @} */ /* end_addtogroup MPCD Multiple PDP Context Definitions */

/*===============================================================================

                     S E R V I C E   I N D I C A T O R S

==============================================================================*/

/*------------------------------------------------------------------------------
   Service Indicators Defines
------------------------------------------------------------------------------*/

/** @addtogroup SI Service Indicators
    @{ */

/** Set window search size.
*/
typedef byte                      sys_srch_win_type;

/** Base station latitude.
*/
typedef int32                     sys_base_lat_type;

/** Base station longitude.
*/
typedef int32                     sys_base_long_type;

/** Base station ID.
*/
typedef word                     sys_base_id_type;

/** WLAN BSS ID.
*/
typedef uint64                   sys_wlan_bssid_type;

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/*------------------------------------------------------------------------------
   Service Indicators Enums
------------------------------------------------------------------------------*/
/**
Phone measurement modes; currently used only for HDR CP.
*/
typedef enum
{
  /** @cond
  */
  SYS_MEAS_MODE_NONE = -1,  /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

  SYS_MEAS_MODE_HDR_OFF = 0,
    /**< HDR enables network access. */

  SYS_MEAS_MODE_HDR_ON = 1,
    /**< HDR disables network access. */

  /** @cond
  */
  SYS_MEAS_MODE_MAX   /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

} sys_meas_mode_e_type;
/*~ SENTINEL sys_meas_mode_e_type.SYS_MEAS_MODE_MAX */


/* For compatibility with the QXDM database, please do not change the following
   fixed assigned values. If new values are added, please also update the QXDM
   database.
*/
/** Phone operating modes.
*/
typedef enum
{
  /** @cond
  */
  SYS_OPRT_MODE_NONE = -1,   /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

  SYS_OPRT_MODE_PWROFF = 0,
    /**< Phone is powering off. */

  SYS_OPRT_MODE_FTM = 1,
    /**< Phone is in factory test mode. */

  SYS_OPRT_MODE_OFFLINE = 2,
    /**< Phone is offline. */

  SYS_OPRT_MODE_OFFLINE_AMPS = 3,
    /**< Phone is offline analog. */

  SYS_OPRT_MODE_OFFLINE_CDMA = 4,
    /**< Phone is offline CDMA. */

  SYS_OPRT_MODE_ONLINE = 5,
    /**< Phone is online. */

  SYS_OPRT_MODE_LPM = 6,
    /**< Phone is in Low Power Mode (LPM) . */

  SYS_OPRT_MODE_RESET = 7,
    /**< Phone is resetting (i.e., power-cycling). */

  SYS_OPRT_MODE_NET_TEST_GW = 8,
    /**< Phone is conducting network test for GSM/WCDMA. This mode can
         <i>not</i> be set by clients. It can only be set by the lower
         layers of the stack. */

  SYS_OPRT_MODE_OFFLINE_IF_NOT_FTM = 9,
    /**< This mode is only set when there is an offline request during
         power-up. This mode cannot be set by clients. It can
         only be set by the task mode controller. */

  SYS_OPRT_MODE_PSEUDO_ONLINE = 10,
    /**< Phone is pseudo online, transmit disabled. */

  SYS_OPRT_MODE_RESET_MODEM = 11,
    /**< Phone is resetting the modem processor. */

  /** @cond
  */
  SYS_OPRT_MODE_MAX   /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

} sys_oprt_mode_e_type;
/*~ SENTINEL sys_oprt_mode_e_type.SYS_OPRT_MODE_MAX */

/* For compatibility with the QXDM database, please do not change the following
   fixed assigned values. If new values are added, please also update the QXDM
   database.
*/
/** System modes.
*/
typedef enum
{
  /** @cond
  */
  SYS_SYS_MODE_NONE = -1,  /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */

  SYS_SYS_MODE_NO_SRV=0,
    /**< No service; NV_MODE_INACTIVE. */

  SYS_SYS_MODE_AMPS=1,
    /**< Analog Mobile Phone System (AMPS) mode. */

  SYS_SYS_MODE_CDMA=2,
    /**< CDMA mode. */

  SYS_SYS_MODE_GSM=3,
    /**< GSM mode. */

  SYS_SYS_MODE_HDR=4,
    /**< HDR mode. */

  SYS_SYS_MODE_WCDMA=5,
    /**< WCDMA mode. */

  SYS_SYS_MODE_GPS=6,
    /**< GPS mode. */

  SYS_SYS_MODE_GW=7,
    /**< GSM and WCDMA mode. */

  SYS_SYS_MODE_WLAN=8,
    /**< WLAN mode. */

  SYS_SYS_MODE_LTE=9,
    /**< LTE mode. */

  SYS_SYS_MODE_GWL=10,
    /**< GSM, WCDMA, and LTE mode. */

  SYS_SYS_MODE_TDS=11,
  /**< TDS mode. */

  /** @cond
  */
  SYS_SYS_MODE_MAX   /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */

} sys_sys_mode_e_type;
/*~ SENTINEL sys_sys_mode_e_type.SYS_SYS_MODE_MAX */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** Macro for creating a bitmask by shifting '1' left by a specified
    number indicated by 'val'. Useful for defining bitmask enum values.
*/
#define BM( val ) ( 1<< ((int)(val)) )

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/**
  Bitmasks of system modes. A combination of system modes can be created
  by using the ADD_SYSMODE macro.
*/
typedef enum
{

  SYS_SYS_MODE_MASK_NONE        = 0,
    /**< Acquire no system. */

  SYS_SYS_MODE_MASK_NO_SRV      = BM( SYS_SYS_MODE_NO_SRV ),
    /**< No service. */

  SYS_SYS_MODE_MASK_CDMA        = BM( SYS_SYS_MODE_CDMA ),
    /**< CDMA mode systems only. */

  SYS_SYS_MODE_MASK_HDR         = BM( SYS_SYS_MODE_HDR ),
    /**< Acquire HDR mode systems only. */

  SYS_SYS_MODE_MASK_GSM         = BM( SYS_SYS_MODE_GSM ),
    /**< Acquire GSM systems only. */

  SYS_SYS_MODE_MASK_WCDMA       = BM( SYS_SYS_MODE_WCDMA ),
    /**< Acquire WCDMA systems only. */

  SYS_SYS_MODE_MASK_WLAN        = BM( SYS_SYS_MODE_WLAN ),
    /**< Acquire WLAN systems only. */

  SYS_SYS_MODE_MASK_LTE         = BM( SYS_SYS_MODE_LTE ),
    /**< Acquire LTE systems only. */

  SYS_SYS_MODE_MASK_TDS         = BM( SYS_SYS_MODE_TDS ),
    /**< Acquire TDS systems only. */
  /** @cond
  */
  SYS_SYS_MODE_MASK_MAX
  /** @endcond
  */
} sys_sys_mode_mask_e_type;
/*~ SENTINEL sys_sys_mode_mask_e_type.SYS_SYS_MODE_MASK_MAX */

/**
  System band classes.
*/
typedef enum
{
  /** @cond
  */
  SYS_BAND_CLASS_NONE        = -1,
    /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

  SYS_BAND_BC0               = 0,
    /**< Band Class 0: U.S. Cellular band (800 MHz). */

  SYS_BAND_BC1               = 1,
    /**< Band Class 1: U.S. */

  SYS_BAND_BC3               = 3,
    /**< Band Class 3: Japanese Cellular band (800 MHz). */

  SYS_BAND_BC4               = 4,
    /**< Band Class 4: Korean PCS band (1800 MHz). */

  SYS_BAND_BC5               = 5,
    /**< Band Class 5 (450 MHz). */

  SYS_BAND_BC6               = 6,
    /**< Band Class 6 (2 GHz). */

  SYS_BAND_BC7               = 7,
    /**< Band Class 7 (Upper 700 MHz). */

  SYS_BAND_BC8               = 8,
    /**< Band Class 8 (1800 MHz). */

  SYS_BAND_BC9               = 9,
    /**< Band Class 9 (900 MHz). */

  SYS_BAND_BC10              = 10,
    /**< Band Class 10 (Second 800 MHz). */

  SYS_BAND_BC11              = 11,
    /**< Band Class 11: European PAMR Band (400 MHz). */

  SYS_BAND_BC12              = 12,
  /**< Band Class 12: PAMR Band (800 MHz). */

  SYS_BAND_BC13              = 13,
  /**< Band Class 13: currently undefined. */

  SYS_BAND_BC14              = 14,
  /**< Band Class 14 (U.S. PCS 1.9 GHz Band). */

  SYS_BAND_BC15              = 15,
  /**< Band Class 15 (1700-2100 MHz -AWS). */

  SYS_BAND_BC16              = 16,
  /**< Band Class 16 (U.S. 2.5 GHz). */

  SYS_BAND_BC17              = 17,
  /**< Band Class 17 (U.S. 2.5 GHz Forward Link only band). */

  SYS_BAND_BC18              = 18,
  /**< Band Class 18 (700 MHz Public Safety Broadband). */

  SYS_BAND_BC19              = 19,
  /**< Band Class 19 (Lower 700 MHz band).*/

  SYS_BAND_BC_MAX            = 20,
  /**< Upper boundary for CDMA band classes.  */

  /* Reserved 20-39 for CDMA band classes. */

  SYS_BAND_GSM_450           = 40,
    /**< GSM 450 band (450 MHz). */

  SYS_BAND_GSM_480           = 41,
    /**< GSM 480 band (480 MHz). */

  SYS_BAND_GSM_750           = 42,
    /**< GSM 750 band (750 MHz). */

  SYS_BAND_GSM_850           = 43,
    /**< GSM 850 band (850 MHz). */

  SYS_BAND_GSM_EGSM_900      = 44,
    /**< GSM Extended GSM (E-GSM) 900 band (900 MHz). */

  SYS_BAND_GSM_PGSM_900      = 45,
    /**< GSM Primary GSM (P-GSM) 900 band (900 MHz). */

  SYS_BAND_GSM_RGSM_900      = 46,
    /**< GSM Railways GSM (R-GSM) 900 band (900 MHz). */

  SYS_BAND_GSM_DCS_1800      = 47,
    /**< GSM DCS band (1800 MHz). */

  SYS_BAND_GSM_PCS_1900      = 48,
    /**< GSM PCS band (1900 MHz). */

  /* Reserved 49-79 for GSM band classes. */

  SYS_BAND_WCDMA_I_IMT_2000  = 80,
    /**< WCDMA Europe, Japan, and China IMT 2100 band. */

  SYS_BAND_WCDMA_II_PCS_1900 = 81,
    /**< WCDMA U.S. PCS 1900 band. */

  SYS_BAND_WCDMA_III_1700    = 82,
    /**< WCDMA Europe and China DCS 1800 band. */

  SYS_BAND_WCDMA_IV_1700     = 83,
    /**< WCDMA U.S. 1700 band. */

  SYS_BAND_WCDMA_V_850       = 84,
    /**< WCDMA U.S. 850 band. */

  SYS_BAND_WCDMA_VI_800      = 85,
    /**< WCDMA Japan 800 band. */

  SYS_BAND_WCDMA_VII_2600    = 86,
    /**< WCDMA Europe 2600 band. */

  SYS_BAND_WCDMA_VIII_900    = 87,
    /**< WCDMA Europe and China 900 band. */

  SYS_BAND_WCDMA_IX_1700     = 88,
    /**< WCDMA Japan 1700 band. */

  /* Reserved 89 for WCDMA BC10-1700 band classes. */

  SYS_BAND_WCDMA_XI_1500     = 90,
    /**< WCDMA 1500 band. */

  SYS_BAND_WCDMA_XIX_850     = 91,
    /**< WCDMA Japan 850 band. */

  /* Reserved 85-109 for WCDMA band classes. */

  SYS_BAND_WLAN_US_2400    = 110,
    /**< WLAN U.S. 2400 MHz band. */

  SYS_BAND_WLAN_JAPAN_2400 = 111,
    /**< WLAN Japan 2400 MHz band. */

  SYS_BAND_WLAN_EUROPE_2400 = 112,
    /**< WLAN Europe 2400 MHz band. */

  SYS_BAND_WLAN_FRANCE_2400 = 113,
    /**< WLAN France 2400 MHz band. */

  SYS_BAND_WLAN_SPAIN_2400 = 114,
    /**< WLAN Spain 2400 MHz band. */

  SYS_BAND_WLAN_US_5000    = 115,
    /**< WLAN U.S. 5000 MHz band. */

  SYS_BAND_WLAN_JAPAN_5000 = 116,
    /**< WLAN Japan 5000 MHz band. */

  SYS_BAND_WLAN_EUROPE_5000 = 117,
    /**< WLAN Europe 5000 MHz band. */

  SYS_BAND_WLAN_FRANCE_5000 = 118,
    /**< WLAN France 5000 MHz band. */

  SYS_BAND_WLAN_SPAIN_5000 = 119,
    /**< WLAN Spain 5000 MHz band. */

  /* LTE bands*/

  SYS_BAND_LTE_EUTRAN_BAND1 = 120,
  /**< UL:1920-1980; DL:2110-2170. */

  SYS_BAND_LTE_EUTRAN_BAND2 = 121,
  /**< UL:1850-1910; DL:1930-1990. */

  SYS_BAND_LTE_EUTRAN_BAND3 = 122,
  /**< UL:1710-1785; DL:1805-1880. */

  SYS_BAND_LTE_EUTRAN_BAND4 = 123,
  /**< UL:1710-1755; DL:2110-2155. */

  SYS_BAND_LTE_EUTRAN_BAND5 = 124,
  /**< UL: 824-849; DL: 869- 894. */

  SYS_BAND_LTE_EUTRAN_BAND6 = 125,
  /**< UL: 830-840; DL: 875-885. */

  SYS_BAND_LTE_EUTRAN_BAND7 = 126,
  /**< UL:2500-2570; DL:2620-2690. */

  SYS_BAND_LTE_EUTRAN_BAND8 = 127,
  /**< UL: 880-915; DL: 925-960. */

  SYS_BAND_LTE_EUTRAN_BAND9 = 128,
  /**< UL:1749.9-1784.9; DL:1844.9-1879.9. */

  SYS_BAND_LTE_EUTRAN_BAND10 = 129,
  /**< UL:1710-1770; DL:2110-2170. */

  SYS_BAND_LTE_EUTRAN_BAND11 = 130,
  /**< UL:1427.9-1452.9; DL:1475.9-1500.9. */

  SYS_BAND_LTE_EUTRAN_BAND12 = 131,
  /**< UL:698-716; DL:728-746. */

  SYS_BAND_LTE_EUTRAN_BAND13 = 132,
  /**< UL: 777-787; DL: 746-756. */

  SYS_BAND_LTE_EUTRAN_BAND14 = 133,
  /**< UL: 788-798; DL: 758-768. */

  /* Reserved for BAND 15, 16 */

  SYS_BAND_LTE_EUTRAN_BAND17 = 136,
  /**< UL: 704-716; DL: 734-746. */

  SYS_BAND_LTE_EUTRAN_BAND18 = 137,
  /**< UL: 815-830; DL: 860-875. */

  SYS_BAND_LTE_EUTRAN_BAND19 = 138,
  /**< UL: 830-845; DL: 875-890. */

  SYS_BAND_LTE_EUTRAN_BAND20 = 139,
  /**< UL: 832-862; DL: 791-821. */

  SYS_BAND_LTE_EUTRAN_BAND21 = 140,
  /**< UL: 1447.9-1462.9; DL: 1495.9-1510.9. */

  SYS_BAND_LTE_EUTRAN_BAND23 = 142,
  /**< UL: 2000-2020; DL: 2180-2200 */

  SYS_BAND_LTE_EUTRAN_BAND24 = 143,
  /**< UL: 1626.5-1660.5; DL: 1525 -1559. */

  SYS_BAND_LTE_EUTRAN_BAND25 = 144,
  /**< UL: 1850-1915; DL: 1930 -1995 . */

  SYS_BAND_LTE_EUTRAN_BAND26 = 145,
  /**< UL: 814-849; DL: 859 -894 . */

  SYS_BAND_LTE_EUTRAN_BAND27 = 146,
       /**< UL: 807.5 -	824;  DL: 852 –	869 */

  SYS_BAND_LTE_EUTRAN_BAND28 = 147,
  /**<UL: 703-748; DL: 758-803 . */

  SYS_BAND_LTE_EUTRAN_BAND29 = 148,
    /**<  UL: 1850-1910 or 1710 - 1755;  DL: 716-728. */

  SYS_BAND_LTE_EUTRAN_BAND30 = 149,
    /**<  UL: 2305 – 2315 ;  DL: 2350 – 2360 */

  SYS_BAND_LTE_EUTRAN_BAND31 = 150,
    /**<  UL: 452.5 –	457.5 ;  DL: 462.5 – 467.5  */

  SYS_BAND_LTE_EUTRAN_BAND32 = 151,
    /**< DL: 9920 – 10359 */

  SYS_BAND_LTE_EUTRAN_BAND33 = 152,
  /**< UL: 1900-1920; DL: 1900-1920. */

  SYS_BAND_LTE_EUTRAN_BAND34 = 153,
  /**< UL: 2010-2025; DL: 2010-2025. */

  SYS_BAND_LTE_EUTRAN_BAND35 = 154,
  /**< UL: 1850-1910; DL: 1850-1910. */

  SYS_BAND_LTE_EUTRAN_BAND36 = 155,
  /**< UL: 1930-1990; DL: 1930-1990. */

  SYS_BAND_LTE_EUTRAN_BAND37 = 156,
  /**< UL: 1910-1930; DL: 1910-1930. */

  SYS_BAND_LTE_EUTRAN_BAND38 = 157,
  /**< UL: 2570-2620; DL: 2570-2620. */

  SYS_BAND_LTE_EUTRAN_BAND39 = 158,
  /**< UL: 1880-1920; DL: 1880-1920. */

  SYS_BAND_LTE_EUTRAN_BAND40 = 159,
  /**< UL: 2300-2400; DL: 2300-2400. */

  SYS_BAND_LTE_EUTRAN_BAND41 = 160,
  /**< UL: 2496-2690; DL: 2496-2690. */

  SYS_BAND_LTE_EUTRAN_BAND42 = 161,
  /**< UL: 3400-3600; DL: 3400-3600. */

  SYS_BAND_LTE_EUTRAN_BAND43 = 162,
  /**< UL: 3600-3800; DL: 3600-3800. */

  SYS_BAND_LTE_EUTRAN_BAND125 = 163,
    /**< DL: 64835 – 64859 */

  SYS_BAND_LTE_EUTRAN_BAND126 = 164,
    /**< DL: 64860 – 64974 */

  SYS_BAND_LTE_EUTRAN_BAND127 = 165,
    /**< DL: 64975 – 64999  */

  SYS_BAND_LTE_EUTRAN_BAND252 = 166,
    /**< DL: 5150 – 5250  */

  SYS_BAND_LTE_EUTRAN_BAND255 = 167,
    /**< DL: 5725 – 5850  */

  SYS_BAND_LTE_EUTRAN_BAND66 = 168,
    /**< UL: 1710-1780; DL: 2110-2200 */


      /* TD-SCDMA bands */
  SYS_BAND_TDS_BANDA = 170,
  /**< TDS Band A 1900-1920 MHz, 2010-2020 MHz */

  SYS_BAND_TDS_BANDB = 171,
  /**< TDS Band B 1850-1910 MHz, 1930-1990 MHz */

  SYS_BAND_TDS_BANDC = 172,
  /**< TDS Band C 1910-1930 MHz */

  SYS_BAND_TDS_BANDD = 173,
  /**< TDS Band D 2570-2620 MHz */

  SYS_BAND_TDS_BANDE = 174,
  /**< TDS Band E 2300-2400 MHz */

  SYS_BAND_TDS_BANDF = 175,
  /**< TDS Band F 1880-1920 MHz */

  /** @cond
  */
  SYS_BAND_CLASS_MAX   /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */
} sys_band_class_e_type;
/*~ SENTINEL sys_band_class_e_type.SYS_BAND_CLASS_MAX */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** CDMA channel number.
*/
typedef uint16 sys_channel_num_type;

/** Pilot PN; valid values: 0-511.
*/
typedef uint16 sys_pilot_pn_type;

/** @brief CDMA/HDR channel.
*/
typedef struct
{
  sys_band_class_e_type    band;
    /**< Band class: 0, 1, etc. */

  sys_channel_num_type     chan_num;
    /**< CDMA channel number. */

} sys_channel_type;


/** @brief Pilot strength measurement for a channel.
*/
typedef struct sys_pilot_info_s
{
  sys_pilot_pn_type         pilot_pn;
    /**< Pilot PN. */

  uint16                    pilot_pn_phase;
    /**< Pilot PN phase. */

  boolean                   is_chan_incl;
    /**< Channel included. */

  sys_channel_type          channel;
    /**< Channel: only valid if is_chan_incl = TRUE. */

  uint8                     pilot_strength;
    /**< Pilot strength. */

} sys_pilot_info_s_type;

/** System band class mask.
*/
typedef uint64                sys_band_mask_type;

/** Band class bitmask data type.
*/
typedef sys_band_mask_type    sys_band_mask_e_type;
    /* Acquire WCDMA Japan 1700 systems only */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

#define SYS_BAND_MASK_EMPTY             0
     /**< No band selected. */

#define SYS_BAND_MASK_BC0_A             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC0_A))
    /**< Acquire band class 0, A-side systems only. */

#define SYS_BAND_MASK_BC0_B             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC0_B))
    /**< Acquire band class 0, B-side systems only. */

#define SYS_BAND_MASK_BC0               ((sys_band_mask_type) (SYS_BM_64BIT(SYS_SBAND_BC0_A) | \
                                        SYS_BM_64BIT(SYS_SBAND_BC0_B)))
    /**< Acquire band class 0 systems only. */

#define SYS_BAND_MASK_BC1               ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC1))
    /**< Acquire band class 1 systems only. */

#define SYS_BAND_MASK_BC2               ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC2))
    /**< Acquire band class 2 place holder. */

#define SYS_BAND_MASK_BC3               ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC3))
    /**< Acquire band class 3 systems only. */

#define SYS_BAND_MASK_BC4               ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC4))
    /**< Acquire band class 4 systems only. */

#define SYS_BAND_MASK_BC5               ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC5))
    /**< Acquire band class 5 systems only. */

#define SYS_BAND_MASK_GSM_DCS_1800      ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_GSM_DCS_1800))
    /**< Acquire band class 8 systems only. */

#define SYS_BAND_MASK_GSM_EGSM_900      ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_GSM_EGSM_900))
    /**< Acquire GSM Extended GSM systems only. */

#define SYS_BAND_MASK_GSM_PGSM_900      ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_GSM_PGSM_900))
    /**< Acquire GSM Primary GSM systems only. */

#define SYS_BAND_MASK_BC6               ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC6))
    /**< Acquire band class 6 systems only. */

#define SYS_BAND_MASK_BC7               ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC7))
    /**< Acquire band class 7 systems only. */

#define SYS_BAND_MASK_BC8               ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC8))
    /**< Acquire band class 8 systems only. */

#define SYS_BAND_MASK_BC9               ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC9))
    /**< Acquire band class 9 systems only. */

#define SYS_BAND_MASK_BC10              ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC10))
    /**< Acquire band class 10 systems only. */

#define SYS_BAND_MASK_BC11              ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC11))
    /**< Acquire band class 11 systems only. */

#define SYS_BAND_MASK_BC12              ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC12))
    /**< Acquire band class 12 systems only. */

#define SYS_BAND_MASK_BC14              ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC14))
    /**< Acquire band class 14 systems only. */

#define SYS_BAND_MASK_BC15              ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC15))
    /**< Acquire band class 15 systems only. */

#define SYS_BAND_MASK_BC16              ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC16))
    /**< Acquire band class 16 systems only. */

#define SYS_BAND_MASK_BC17              ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC17))
    /**< Acquire band class 17 systems only. */

#define SYS_BAND_MASK_BC18              ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC18))
    /**< Acquire band class 18 systems only. */

#define SYS_BAND_MASK_BC19              ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC19))
    /**< Acquire band class 19 systems only. */

#define SYS_BAND_MASK_GSM_450           ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_GSM_450))
    /**< Acquire GSM 450 systems only. */

#define SYS_BAND_MASK_GSM_480           ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_GSM_480))
    /**< Acquire GSM 480 systems only. */

#define SYS_BAND_MASK_GSM_750           ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_GSM_750 ))
    /**< Acquire GSM 750 systems only. */

#define SYS_BAND_MASK_GSM_850           ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_GSM_850 ))
    /**< Acquire GSM 850 systems only. */

#define SYS_BAND_MASK_GSM_RGSM_900      ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_GSM_RGSM_900))
    /**< Acquire GSM Railway GSM systems only. */

#define SYS_BAND_MASK_GSM_PCS_1900      ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_GSM_PCS_1900))
    /**< Acquire GSM PCS systems only. */

#define SYS_BAND_MASK_WCDMA_I_IMT_2000  ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_WCDMA_I_IMT_2000))
    /**< Acquire WCDMA Europe, Japan, and China IMT 2100 systems only. */

#define SYS_BAND_MASK_WCDMA_II_PCS_1900 ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_WCDMA_II_PCS_1900))
    /**< Acquire WCDMA U.S. PCS 1900 system only. */

#define SYS_BAND_MASK_WCDMA_III_1700    ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_WCDMA_III_1700))
    /**< Acquire WCDMA Europe and China 1800 systems only. */

#define SYS_BAND_MASK_WCDMA_IV_1700     ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WCDMA_IV_1700 ))
    /**< Acquire WCDMA U.S. 1700 systems only. */

#define SYS_BAND_MASK_WCDMA_V_850       ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WCDMA_V_850 ))
    /**< Acquire WCDMA U.S. 850 systems only. */

#define SYS_BAND_MASK_WCDMA_VI_800      ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WCDMA_VI_800 ))
    /**< Acquire WCDMA Japan 800 systems only. */

#define SYS_BAND_MASK_WCDMA_VII_2600       ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WCDMA_VII_2600 ))
    /**< Acquire WCDMA Europe 2600 systems only. */

#define SYS_BAND_MASK_WCDMA_VIII_900       ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WCDMA_VIII_900 ))
    /**< Acquire WCDMA Europe and China 900 systems only. */

#define SYS_BAND_MASK_WCDMA_IX_1700       ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WCDMA_IX_1700 ))
    /**< Acquire WCDMA Japan 1700 systems only. */

#define SYS_BAND_MASK_WCDMA_XI_1500      ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WCDMA_XI_1500 ))
    /**< Acquire WCDMA 1500 systems only. */

#define SYS_BAND_MASK_WCDMA_XIX_850      ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WCDMA_XIX_850 ))
    /**< Acquire WCDMA Japan 850 systems only. */

#define SYS_BAND_MASK_WLAN_US_2400      ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WLAN_US_2400 ))
    /**< Acquire WLAN U.S. 2400 systems only. */

#define SYS_BAND_MASK_WLAN_EUROPE_2400  ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WLAN_EUROPE_2400 ))
    /**< Acquire WLAN Europe 2400 systems only. */

#define SYS_BAND_MASK_WLAN_FRANCE_2400  ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WLAN_FRANCE_2400 ))
    /**< Acquire WLAN France 2400 systems only. */

#define SYS_BAND_MASK_WLAN_SPAIN_2400   ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WLAN_SPAIN_2400 ))
    /**< Acquire WLAN Spain 2400 systems only. */

#define SYS_BAND_MASK_WLAN_JAPAN_2400   ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WLAN_JAPAN_2400 ))
    /**< Acquire WLAN Japan 2400 systems only. */

#define SYS_BAND_MASK_WLAN_US_5000      ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WLAN_US_5000 ))
    /**< Acquire WLAN U.S. 5000 systems only. */

#define SYS_BAND_MASK_WLAN_EUROPE_5000  ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WLAN_EUROPE_5000 ))
    /**< Acquire WLAN Europe 5000 systems only. */

#define SYS_BAND_MASK_WLAN_FRANCE_5000  ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WLAN_FRANCE_5000 ))
    /**< Acquire WLAN France 5000 systems only. */

#define SYS_BAND_MASK_WLAN_SPAIN_5000   ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WLAN_SPAIN_5000 ))
    /**< Acquire WLAN Spain 5000 systems only. */

#define SYS_BAND_MASK_WLAN_JAPAN_5000   ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WLAN_JAPAN_5000 ))
    /**< Acquire WLAN Japan 5000 systems only. */

/** Acquire U.S./JAPAN/EUROPE/FRANCE/SPAIN 2400 WLAN systems in respective countries. */
#define SYS_BAND_MASK_WLAN_2400         (SYS_BAND_MASK_WLAN_US_2400      | \
                                         SYS_BAND_MASK_WLAN_JAPAN_2400   | \
                                         SYS_BAND_MASK_WLAN_EUROPE_2400  | \
                                         SYS_BAND_MASK_WLAN_FRANCE_2400  | \
                                         SYS_BAND_MASK_WLAN_SPAIN_2400)

/** Acquire U.S./JAPAN/EUROPE/FRANCE/SPAIN 5000 WLAN systems in respective countries. */
#define SYS_BAND_MASK_WLAN_5000         (SYS_BAND_MASK_WLAN_US_5000      | \
                                         SYS_BAND_MASK_WLAN_JAPAN_5000   | \
                                         SYS_BAND_MASK_WLAN_FRANCE_5000  | \
                                         SYS_BAND_MASK_WLAN_EUROPE_5000  | \
                                         SYS_BAND_MASK_WLAN_SPAIN_5000)

/** Acquire any of WLAN 2400/5000 systems in U.S./JAPAN/EUROPE/FRANCE/SPAIN. */
#define SYS_BAND_MASK_WLAN_ANY          (SYS_BAND_MASK_WLAN_2400 | \
                                         SYS_BAND_MASK_WLAN_5000)

/** Acquire any systems within the specified mode preferences. */
#define SYS_BAND_MASK_ANY               ((sys_band_mask_e_type)0x7FFFFFFFFFFFFFFFULL)
                                         /* No, this doesn't say "FULL".
                                         ** "ULL" designates
                                         **"unsigned long long" */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/* If there is a new band class, append to the end of list.
   per 3GPP 36101-830.
*/
/** LTE system band classes.
*/
typedef enum {
  SYS_SBAND_LTE_EUTRAN_BAND1 = 0,
  /**< UL:1920-1980; DL:2110-2170. */

  SYS_SBAND_LTE_EUTRAN_BAND2 = 1,
  /**< UL:1850-1910; DL:1930-1990. */

  SYS_SBAND_LTE_EUTRAN_BAND3 = 2,
  /**< UL:1710-1785; DL:1805-1880. */

  SYS_SBAND_LTE_EUTRAN_BAND4 = 3,
  /**< UL:1710-1755; DL:2110-2155. */

  SYS_SBAND_LTE_EUTRAN_BAND5 = 4,
  /**< UL: 824-849; DL: 869-894. */

  SYS_SBAND_LTE_EUTRAN_BAND6 = 5,
  /**< UL: 830-840; DL: 875-885. */

  SYS_SBAND_LTE_EUTRAN_BAND7 = 6,
  /**< UL:2500-2570; DL:2620-2690. */

  SYS_SBAND_LTE_EUTRAN_BAND8 = 7,
  /**< UL: 880-915; DL: 925-960. */

  SYS_SBAND_LTE_EUTRAN_BAND9 = 8,
  /**< UL:1749.9-1784.9; DL:1844.9-1879.9. */

  SYS_SBAND_LTE_EUTRAN_BAND10 = 9,
  /**< UL:1710-1770; DL:2110-2170. */

  SYS_SBAND_LTE_EUTRAN_BAND11 = 10,
  /**< UL:1427.9-1452.9; DL:1475.9-1500.9. */

  SYS_SBAND_LTE_EUTRAN_BAND12 = 11,
  /**< UL:698-716; DL:728-746. */

  SYS_SBAND_LTE_EUTRAN_BAND13 = 12,
  /**< UL: 777-787; DL: 746-756. */

  SYS_SBAND_LTE_EUTRAN_BAND14 = 13,
  /**< UL: 788-798; DL: 758-768. */

  SYS_SBAND_LTE_EUTRAN_BAND17 = 16,
  /**< UL: 704-716; DL: 734-746. */

  SYS_SBAND_LTE_EUTRAN_BAND18 = 17,
  /**< UL: 815-830; DL: 860-875. */

  SYS_SBAND_LTE_EUTRAN_BAND19 = 18,
  /**< UL: 830-845; DL: 875-890. */

  SYS_SBAND_LTE_EUTRAN_BAND20 = 19,
  /**< UL: 832-862; DL: 791-821. */

  SYS_SBAND_LTE_EUTRAN_BAND21 = 20,
  /**< UL: 1447.9-1462.9; DL: 1495.9-1510.9. */

  SYS_SBAND_LTE_EUTRAN_BAND23 = 22,
  /**< UL: 2000-2020; DL: 2180-2200 */

  SYS_SBAND_LTE_EUTRAN_BAND24 = 23,
  /**< UL: 1626.5-1660.5; DL: 1525 -1559. */

  SYS_SBAND_LTE_EUTRAN_BAND25 = 24,
  /**< UL: 1850-1915; DL: 1930 -1995 . */

  SYS_SBAND_LTE_EUTRAN_BAND26 = 25,
  /**< UL: 814-849; DL: 859 -894 . */

  SYS_SBAND_LTE_EUTRAN_BAND27 = 26,
       /**<  UL: 807.5 -	824;  DL: 852 –	869 */

  SYS_SBAND_LTE_EUTRAN_BAND28 = 27,
  /**< UL: 703-748; DL: 758-803 . */

  SYS_SBAND_LTE_EUTRAN_BAND29 = 28,
    /**<  UL: 1850-1910 or 1710 - 1755;  DL: 716-728. */
  
  SYS_SBAND_LTE_EUTRAN_BAND30 = 29,
    /**<  UL: 2305 – 2315 ;  DL: 2350 – 2360 */

  SYS_SBAND_LTE_EUTRAN_BAND31 = 30,
    /**<  UL: 452.5 –	457.5 ;  DL: 462.5 –467.5  */

  SYS_SBAND_LTE_EUTRAN_BAND32 = 31,
    /**< DL: 9920 – 10359 */

  SYS_SBAND_LTE_EUTRAN_BAND33 = 32,
  /**< UL: 1900-1920; DL: 1900-1920. */

  SYS_SBAND_LTE_EUTRAN_BAND34 = 33,
  /**< UL: 2010-2025; DL: 2010-2025. */

  SYS_SBAND_LTE_EUTRAN_BAND35 = 34,
  /**< UL: 1850-1910; DL: 1850-1910. */

  SYS_SBAND_LTE_EUTRAN_BAND36 = 35,
  /**< UL: 1930-1990; DL: 1930-1990. */

  SYS_SBAND_LTE_EUTRAN_BAND37 = 36,
  /**< UL: 1910-1930; DL: 1910-1930. */

  SYS_SBAND_LTE_EUTRAN_BAND38 = 37,
  /**< UL: 2570-2620; DL: 2570-2620. */

  SYS_SBAND_LTE_EUTRAN_BAND39 = 38,
  /**< UL: 1880-1920; DL: 1880-1920. */

  SYS_SBAND_LTE_EUTRAN_BAND40 = 39,
  /**< UL: 2300-2400; DL: 2300-2400. */

  SYS_SBAND_LTE_EUTRAN_BAND41 = 40,
  /**< UL: 2496-2690; DL: 2496-2690 */

  SYS_SBAND_LTE_EUTRAN_BAND42 = 41,
  /**< UL: 3400-3600; DL: 3400-3600 */

  SYS_SBAND_LTE_EUTRAN_BAND43 = 42,
  /**< UL: 3600-3800; DL: 3600-3800 */

  SYS_SBAND_LTE_EUTRAN_BAND60 = 59,
    /**< BAND_PREF_NO_CHG as this band is not in use at the moment. */

  SYS_SBAND_LTE_EUTRAN_BAND125 = 60,
    /**< DL: 64835 – 64859 */

  SYS_SBAND_LTE_EUTRAN_BAND126 = 61,
    /**< DL: 64860 – 64974 */

  SYS_SBAND_LTE_EUTRAN_BAND127 = 63,
    /**< DL: 64975 – 64999 */

  SYS_SBAND_LTE_EUTRAN_BAND66 = 65,
    /**< UL: 1710-1780; DL: 2110-2200 */

  SYS_SBAND_LTE_EUTRAN_BAND252 = 251,
    /**< DL: 5150 – 5250 */

  SYS_SBAND_LTE_EUTRAN_BAND255 = 254
    /**< DL: 5725 – 5850 */

  /**< Maximum number of EUTRAN bands is defined as a preprocessor MACRO below
  */
} sys_sband_lte_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*
** Whenever this feature name is present, it indicates that the data type and
** macros are present in the build.The value can be 256 or 64 to reflect the
** number of LTE bands supported by target.
*/
#define LTE_BAND_NUM 256  /* Value can be set to 256 or 64. */

#if (LTE_BAND_NUM == 256)

typedef bit_mask_256_t        sys_lte_band_mask_e_type; /* data struct definition */
#define SYS_SBAND_LTE_EUTRAN_BAND_MAX ((sys_sband_lte_e_type)255) /* MAX value for LTE bands for bound checking */
#define FEATURE_LTE_UPLINK_64_PLUS
#else

typedef sys_band_mask_type    sys_lte_band_mask_e_type; /* data struct definition */
#define SYS_SBAND_LTE_EUTRAN_BAND_MAX ((sys_sband_lte_e_type)64)                /* MAX value for LTE bands for bound checking */

#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* These are constant values associated ONLY with the LTE bands. The functions
** will provide the constant value as per the feature value defined above.
*/
sys_lte_band_mask_e_type sys_lte_band_pref_lte_const_none(void);
sys_lte_band_mask_e_type sys_lte_band_pref_lte_const_any(void);
sys_lte_band_mask_e_type sys_lte_band_pref_lte_const_no_chg(void);

/* Constant for NO LTE bands */
#define SYS_LTE_BAND_MASK_CONST_NONE sys_lte_band_pref_lte_const_none()

/* Constant to notify CM to continue using the existing LTE bands. */
#define SYS_LTE_BAND_MASK_CONST_NO_CHG sys_lte_band_pref_lte_const_no_chg()

/* Constant enabling all the DEFINED LTE bands. When a new LTE band is defined
** in future as per any new spec, this API will need to be updated.
*/
#define SYS_LTE_BAND_MASK_CONST_ANY sys_lte_band_pref_lte_const_any()

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Conversion APIs to convert the LTE bands from legacy to new data types. */
sys_sband_lte_e_type sys_map_lte_band_to_sband(sys_band_class_e_type);
sys_lte_band_mask_e_type sys_map_band_mask_to_lte_band_mask(sys_band_mask_e_type band_pref);
sys_band_mask_e_type sys_map_lte_band_mask_to_band_mask(sys_lte_band_mask_e_type band_pref);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* LTE BAND MASK MACROS:
** All the operations on the data type sys_lte_band_mask_e_type should be
** carried out exclusively with the MACROS defined below.
** This will enable the modem code to compile for 64 and 256 LTE bands without
** any issues.
*/

#if (LTE_BAND_NUM == 256)

/* Remove mask B from mask A and store result in mask A*/
#define SYS_LTE_BAND_MASK_REMOVE_MASK(bit_mask_ptr_A,bit_mask_ptr_B)    bit_mask_256_remove_mask(bit_mask_ptr_A,bit_mask_ptr_B)
/* Add mask B to mask A and store result in mask A*/
#define SYS_LTE_BAND_MASK_ADD_MASK(bit_mask_ptr_A,bit_mask_ptr_B)       bit_mask_256_add_mask(bit_mask_ptr_A,bit_mask_ptr_B)
/* TRUE if any common bit set in mask A and mask B */
#define SYS_LTE_BAND_MASK_CHK_IF_OVERLAP(bit_mask_ptr_A,bit_mask_ptr_B) bit_mask_256_masks_overlap(bit_mask_ptr_A,bit_mask_ptr_B)
/* Return a bitmask which has the bits set in both mask A and mask B */
#define SYS_LTE_BAND_MASK_COMMON(bit_mask_ptr_A,bit_mask_ptr_B)         bit_mask_256_masks_common(bit_mask_ptr_A,bit_mask_ptr_B)
/* TRUE if same bits set in both masks */
#define SYS_LTE_BAND_MASK_CHK_IF_EQUAL(bit_mask_ptr_A,bit_mask_ptr_B)   bit_mask_256_masks_are_equal(bit_mask_ptr_A,bit_mask_ptr_B)
/* TRUE if no bits set in the bit mask */
#define SYS_LTE_BAND_MASK_CHK_IF_EMPTY(bit_mask_ptr)                    bit_mask_256_mask_is_empty(bit_mask_ptr)
/* Reset all the bits in the mask */
#define SYS_LTE_BAND_MASK_CLR_ALL_BANDS(bit_mask_ptr)                   bit_mask_256_clear_all(bit_mask_ptr)
/* Set all the bits in the mask. */
#define SYS_LTE_BAND_MASK_SET_ALL_BANDS(bit_mask_ptr)                   bit_mask_256_set_all(bit_mask_ptr)
/* Check if the Bit in the bitmask_ptr corresponding to the LTE band_enum as per sys_sband_lte_e_type is set.  */
#define SYS_LTE_BAND_MASK_IS_BAND_PRESENT(bit_mask_ptr,band_enum)       bit_mask_256_bit_is_present(bit_mask_ptr,band_enum)
/* Reset the Bit in the bitmask_ptr corresponding to the LTE band_enum as per sys_sband_lte_e_type. */
#define SYS_LTE_BAND_MASK_REMOVE_BAND(bit_mask_ptr,band_enum)           bit_mask_256_clear_bit(bit_mask_ptr,band_enum)
/* Set the Bit in the bitmask_ptr corresponding to the LTE band_enum as per sys_sband_lte_e_type*/
#define SYS_LTE_BAND_MASK_ADD_BAND(bit_mask_ptr,band_enum)              bit_mask_256_set_bit(bit_mask_ptr,band_enum)
/* TRUE if all the bits set in mask B are also set in mask A */
#define SYS_LTE_BAND_MASK_CHK_IF_SUBSET(bit_mask_ptr_A,bit_mask_ptr_B)  bit_mask_256_mask_is_subset(bit_mask_ptr_A,bit_mask_ptr_B)
/* Perform the XOR operation on 2 bitmasks. return the result as (mask A XOR mask B) */
#define SYS_LTE_BAND_MASK_XOR_MASKS(bit_mask_ptr_A,bit_mask_ptr_B)      bit_mask_256_mask_xor_masks(bit_mask_ptr_A,bit_mask_ptr_B)
/* Toggle all the bits in the mask. */
#define SYS_LTE_BAND_MASK_INVERT_ALL_BANDS(bit_mask_ptr)                bit_mask_256_inv_all(bit_mask_ptr)

#else

/* Remove mask B from mask A and store result in mask A*/
#define SYS_LTE_BAND_MASK_REMOVE_MASK(bit_mask_ptr_A,bit_mask_ptr_B)    (*(bit_mask_ptr_A) &= ~(*(bit_mask_ptr_B)))
/* Add mask B to mask A and store result in mask A*/
#define SYS_LTE_BAND_MASK_ADD_MASK(bit_mask_ptr_A,bit_mask_ptr_B)       (*(bit_mask_ptr_A) |= *(bit_mask_ptr_B))
/* TRUE if any common bit set in mask A and mask B */
#define SYS_LTE_BAND_MASK_CHK_IF_OVERLAP(bit_mask_ptr_A,bit_mask_ptr_B) ((*(bit_mask_ptr_A) & (*(bit_mask_ptr_B))) != 0 )
/* Return a bitmask which has the bits set in both mask A and mask B */
#define SYS_LTE_BAND_MASK_COMMON(bit_mask_ptr_A,bit_mask_ptr_B)         (*(bit_mask_ptr_A) & (*(bit_mask_ptr_B)))
/* TRUE if same bits set in both masks */
#define SYS_LTE_BAND_MASK_CHK_IF_EQUAL(bit_mask_ptr_A,bit_mask_ptr_B)   (*(bit_mask_ptr_A) ==  *(bit_mask_ptr_B))
/* TRUE if no bits set in the bit mask */
#define SYS_LTE_BAND_MASK_CHK_IF_EMPTY(bit_mask_ptr)                    (0 == *(bit_mask_ptr))
/* Reset all the bits in the mask */
#define SYS_LTE_BAND_MASK_CLR_ALL_BANDS(bit_mask_ptr)                   (*(bit_mask_ptr) = 0)
/* Set all the bits in the mask. */
#define SYS_LTE_BAND_MASK_SET_ALL_BANDS(bit_mask_ptr)                   (*(bit_mask_ptr) = 0xFFFFFFFFFFFFFFFF)
/* Check if the Bit in the bitmask_ptr corresponding to the LTE band_enum as per sys_sband_lte_e_type is set.  */
#define SYS_LTE_BAND_MASK_IS_BAND_PRESENT(bit_mask_ptr,band_enum)       ((*(bit_mask_ptr)  &    SYS_BM_64BIT(band_enum)) != 0)
/* Reset the Bit in the bitmask_ptr corresponding to the LTE band_enum as per sys_sband_lte_e_type. */
#define SYS_LTE_BAND_MASK_REMOVE_BAND(bit_mask_ptr,band_enum)           ((*(bit_mask_ptr)  &= ~(SYS_BM_64BIT(band_enum))))
/* Set the Bit in the bitmask_ptr corresponding to the LTE band_enum as per sys_sband_lte_e_type*/
#define SYS_LTE_BAND_MASK_ADD_BAND(bit_mask_ptr,band_enum)              ((*(bit_mask_ptr)  |=   SYS_BM_64BIT(band_enum)))
/* TRUE if all the bits set in mask B are also set in mask A */
#define SYS_LTE_BAND_MASK_CHK_IF_SUBSET(bit_mask_ptr_A,bit_mask_ptr_B)  ((*(bit_mask_ptr_A) &   *(bit_mask_ptr_B)) == *(bit_mask_ptr_B))
/* Perform the XOR operation on 2 bitmasks. return the result as (mask A XOR mask B) */
#define SYS_LTE_BAND_MASK_XOR_MASKS(bit_mask_ptr_A,bit_mask_ptr_B)      (*(bit_mask_ptr_A) ^ (*(bit_mask_ptr_B)))
/* Toggle all the bits in the mask. */
#define SYS_LTE_BAND_MASK_INVERT_ALL_BANDS(bit_mask_ptr)                (~(*(bit_mask_ptr)))

#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*
*  Enumeration of LTE system band class mask.
*  It converts bit position of sys_lte_sband_e_type to a bit
*  mask. To keep numbering scheme consistent,
*  use sys_lte_sband_e_type instead of numbers directly.
*/

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/* LTE band class bitmask data type. It is a combination of
*  enumeration of LTE system band class mask. */

#define SYS_BAND_MASK_LTE_EMPTY             0
     /**< No LTE band selected. */

#define SYS_BAND_MASK_LTE_BAND1             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND1))
    /**< Acquire UL: 1920-1980; DL: 2110-2170. */

#define SYS_BAND_MASK_LTE_BAND2             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND2))
    /**< Acquire UL: 1850-1910; DL: 1930-1990. */

#define SYS_BAND_MASK_LTE_BAND3             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND3))
    /**< Acquire UL: 1710-1785; DL: 1805-1880. */

#define SYS_BAND_MASK_LTE_BAND4             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND4))
    /**< Acquire UL: 1710-1755; DL: 2110-2115. */

#define SYS_BAND_MASK_LTE_BAND5             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND5))
    /**< Acquire UL: 824-849; DL: 869-894. */

#define SYS_BAND_MASK_LTE_BAND6             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND6))
    /**< Acquire UL: 830-840; DL: 875-885. */

#define SYS_BAND_MASK_LTE_BAND7             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND7))
    /**< Acquire UL: 2500-2570; DL: 2620-2690. */

#define SYS_BAND_MASK_LTE_BAND8             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND8))
    /**< Acquire UL: 880-915; DL: 925-960. */

#define SYS_BAND_MASK_LTE_BAND9             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND9))
    /**< Acquire UL: 1749.9-1784.9; DL: 1844.9-1879.9. */

#define SYS_BAND_MASK_LTE_BAND10             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND10))
    /**< Acquire UL: 1710-1770; DL: 2110-2170. */

#define SYS_BAND_MASK_LTE_BAND11             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND11))
    /**< Acquire UL:1427.9-1452.9; DL: 1475.9-1500.9. */

#define SYS_BAND_MASK_LTE_BAND12             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND12))
    /**< Acquire UL:698-716; DL: 728-746. */

#define SYS_BAND_MASK_LTE_BAND13             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND13))
    /**< Acquire UL: 777-787; DL: 746-756. */

#define SYS_BAND_MASK_LTE_BAND14             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND14))
    /**< Acquire UL: 788-798; DL: 758-768. */

#define SYS_BAND_MASK_LTE_BAND17             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND17))
    /**< Acquire UL: 704-716; DL: 734-746. */
#define SYS_BAND_MASK_LTE_BAND18             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND18))
    /**< Acquire UL: 815–830; DL: 860–875. */

#define SYS_BAND_MASK_LTE_BAND19             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND19))
    /**< Acquire UL: 830–845; DL: 875–890. */

#define SYS_BAND_MASK_LTE_BAND20             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND20))
    /**< Acquire UL: 832–862; DL: 791–821. */

#define SYS_BAND_MASK_LTE_BAND21             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND21))
    /**< Acquire UL: 1447.9–1462.9; DL: 1495.9–1510.9. */

#define SYS_BAND_MASK_LTE_BAND23             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND23))
   /**< Acquire UL: 2000–2020; DL: 2180-2200 */

#define SYS_BAND_MASK_LTE_BAND24             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND24))
    /**< Acquire UL: 1626.5-1660.5; DL: 1525 -1559. */

#define SYS_BAND_MASK_LTE_BAND25             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND25))
    /**< Acquire UL: 1850-1915; DL: 1930 -1995  */

#define SYS_BAND_MASK_LTE_BAND26             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND26))
    /**< Acquire UL: 814-849; DL: 859 -894  */

#define SYS_BAND_MASK_LTE_BAND27             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND27))
       /**< Acquire UL: 807.5 -	824;  DL: 852 –	869 */

#define SYS_BAND_MASK_LTE_BAND28             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND28))
      /**< Acquire UL: 703-748; DL: 758-803 . */

#define SYS_BAND_MASK_LTE_BAND29             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND29))
    /**< Acquire UL: 1850-1910 or 1710 - 1755;  DL: 716-728. */

#define SYS_BAND_MASK_LTE_BAND30             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND30))
       /**< Acquire UL: 2305 – 2315 ;  DL: 2350 – 2360 */

#define SYS_BAND_MASK_LTE_BAND31             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND31))
       /**< Acquire UL: 452.5 -	457.5;  DL: 462.5 –	467.5 */

#define SYS_BAND_MASK_LTE_BAND32             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND32))
    /**< DL: 9920 – 10359 */

#define SYS_BAND_MASK_LTE_BAND33             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND33))
    /**< Acquire UL: 1900-1920; DL: 1900-1920. */

#define SYS_BAND_MASK_LTE_BAND34             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND34))
    /**< Acquire UL: 2010-2025; DL: 2010-2025. */

#define SYS_BAND_MASK_LTE_BAND35             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND35))
    /**< Acquire UL: 1850-1910; DL: 1850-1910. */

#define SYS_BAND_MASK_LTE_BAND36             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND36))
    /**< Acquire UL: 1930-1990; DL: 1930-1990. */

#define SYS_BAND_MASK_LTE_BAND37             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND37))
    /**< Acquire UL: 1910-1930; DL: 1910-1930. */

#define SYS_BAND_MASK_LTE_BAND38             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND38))
    /**< Acquire UL: 2570-2620; DL: 2570-2620. */

#define SYS_BAND_MASK_LTE_BAND39             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND39))
    /**< Acquire UL: 1880-1920; DL: 1880-1920. */

#define SYS_BAND_MASK_LTE_BAND40             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND40))
    /**< Acquire UL: 2300-2400; DL: 2300-2400. */

#define SYS_BAND_MASK_LTE_BAND41             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND41))
    /**< Acquire UL: 2496-2690; DL: 2496-2690. */

#define SYS_BAND_MASK_LTE_BAND42             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND42))
    /**< Acquire UL: 3400-3600; DL: 3400-3600. */

#define SYS_BAND_MASK_LTE_BAND43             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND43))
    /**< Acquire UL: 3600-3800; DL: 3600-3800. */

#define SYS_BAND_MASK_LTE_BAND125             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND125))
    /**< DL: 64835 – 64859 */

#define SYS_BAND_MASK_LTE_BAND126             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND126))
    /**< DL: 64860 – 64974 */

#define SYS_BAND_MASK_LTE_BAND127             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND127))
    /**< DL: 64975 – 64999 */

#define SYS_MAX_ACQ_PREF_RAT_LIST_NUM 10
    /**< Maximum number of acquisition lists. */
#define SYS_WCDMA_MAX_MONITORED_LTE_CELLS 16
   /**<. Maximum number of LTE cells monitored */


/**
*  Enumeration of TD-SCDMA system band classes. If there is a new
*  band class, append to the end of list. per 3GPP TS 125 102
*/
typedef enum {
  SYS_SBAND_TDS_BANDA = 0,
  /**< TDS Band A 1900-1920 MHz, 2010-2020 MHz */

  SYS_SBAND_TDS_BANDB = 1,
  /**< TDS Band B 1850-1910 MHz, 1930-1990 MHz */

  SYS_SBAND_TDS_BANDC = 2,
  /**< TDS Band C 1910-1930 MHz */

  SYS_SBAND_TDS_BANDD = 3,
  /**< TDS Band D 2570-2620 MHz */

  SYS_SBAND_TDS_BANDE = 4,
  /**< TDS Band E 2300-2400 MHz */

  SYS_SBAND_TDS_BANDF = 5,
  /**< TDS Band F 1880-1920 MHz */

  SYS_SBAND_TDS_BAND_MAX
  /**< Maximum number of TD-SCDMA bands */

} sys_sband_tds_e_type;

/**
*  Enumeration of TD-SCDMA system band class mask.
*  It converts bit position of sys_tds_sband_e_type to a bit
*  mask. To keep numbering scheme consistent,
*  use sys_tds_sband_e_type instead of numbers directly.
**
*/

/**
*  TD-SCDMA band class bit mask data type. It is a combination of
*  enumeration of TD-SCDMA system band class mask.
*/

#define SYS_BAND_MASK_TDS_EMPTY             0
     /**< No TDS band selected */

#define SYS_BAND_MASK_TDS_BANDA             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_TDS_BANDA))
     /**< TDS Band A 1900-1920 MHz, 2010-2020 MHz */

#define SYS_BAND_MASK_TDS_BANDB             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_TDS_BANDB))
  /**< TDS Band B 1850-1910 MHz, 1930-1990 MHz */

#define SYS_BAND_MASK_TDS_BANDC             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_TDS_BANDC))
  /**< TDS Band C 1910-1930 MHz */

#define SYS_BAND_MASK_TDS_BANDD             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_TDS_BANDD))
  /**< TDS Band D 2570-2620 MHz */

#define SYS_BAND_MASK_TDS_BANDE             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_TDS_BANDE))
  /**< TDS Band E 2300-2400 MHz */

#define SYS_BAND_MASK_TDS_BANDF             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_TDS_BANDF))
  /**< TDS Band F 1880-1920 MHz */



/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** Channel data type.
*/
typedef word  sys_chan_type;

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** Personal Communication Services (PCS) frequency blocks/cellular systems.
*/
typedef enum
{
  /** @cond
  */
  SYS_BLKSYS_NONE = -1,   /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

  /* 0 */
  SYS_BLKSYS_PCS_A,
    /**< PCS block A. */

  SYS_BLKSYS_PCS_B,
    /**< PCS block B. */

  SYS_BLKSYS_PCS_C,
    /**< PCS block C. */

  SYS_BLKSYS_PCS_D,
    /**< PCS block D. */

  /* 4 */
  SYS_BLKSYS_PCS_E,
    /**< PCS block E. */

  SYS_BLKSYS_PCS_F,
    /**< PCS block F. */

  SYS_BLKSYS_PCS_CELL_A,
    /**< Cellular system A. */

  SYS_BLKSYS_PCS_CELL_B,
    /**< Cellular system B. */

  /* 8 */
  SYS_BLKSYS_PCS_G,
    /**< BC5/11 G-Block. */

  SYS_BLKSYS_PCS_H,
    /**< BC5/11 H-Block. */

  SYS_BLKSYS_PCS_I,
    /**< BC5/11 I-Block. */

  SYS_BLKSYS_PCS_J,
    /**< BC5/11 J-Block. */

  /* 12 */
  SYS_BLKSYS_PCS_K,
    /**< BC5/11 K-Block. */

  SYS_BLKSYS_PCS_L,
    /**< BC5 L-BLOCK. */

  /** @cond
  */
  SYS_BLKSYS_MAX   /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

} sys_blksys_e_type;
/*~ SENTINEL sys_blksys_e_type.SYS_BLKSYS_MAX */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Preferred Roaming List (PRL) system types.
*/
typedef enum {

  SYS_PRL_AVAIL_BC0_A = 0,
    /**< Available systems on Band Class 0, A-System only. */

  SYS_PRL_AVAIL_BC0_B = 1,
    /**< Available systems on Band Class 0, B-System only. */

  /** @cond
  */
  SYS_PRL_MAX
  /** @endcond
  */
} sys_prl_e_type;
/*~ SENTINEL sys_prl_e_type.SYS_PRL_MAX */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Service status; indicates service availability.
*/
typedef enum
{
  /** @cond
  */
  SYS_SRV_STATUS_NONE = -1,  /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */

  /* 0 */
  SYS_SRV_STATUS_NO_SRV,
    /**< No service. */

  SYS_SRV_STATUS_LIMITED,
    /**< Limited service. */

  SYS_SRV_STATUS_SRV,
    /**< Service available. */

  SYS_SRV_STATUS_LIMITED_REGIONAL,
    /**< Limited regional service. */

  /* 4 */
  SYS_SRV_STATUS_PWR_SAVE,
    /**< MS is in power save or deep sleep. */

  SYS_SRV_STATUS_NO_SRV_INTERNAL,
  /**< No service. FOR INTERNAL USE ONLY! */

  SYS_SRV_STATUS_LIMITED_INTERNAL,
  /**< Limited service. FOR INTERNAL USE ONLY! */

  SYS_SRV_STATUS_LIMITED_REGIONAL_INTERNAL,
  /**< Limited regional service. FOR INTERNAL USE ONLY! */

  SYS_SRV_STATUS_PWR_SAVE_INTERNAL,
  /**< Power save. FOR INTERNAL USE ONLY! */

  /** @cond
  */
  SYS_SRV_STATUS_MAX   /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

} sys_srv_status_e_type;
/*~ SENTINEL sys_srv_status_e_type.SYS_SRV_STATUS_MAX */

/** Service type.
*/
typedef enum
{
  /** @cond
  */
  SYS_SRV_DOMAIN_NONE = -1,   /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */
  /* 0 */
  SYS_SRV_DOMAIN_NO_SRV,
    /**< No service. */

  SYS_SRV_DOMAIN_CS_ONLY,
    /**< Circuit switched only capable. */

  SYS_SRV_DOMAIN_PS_ONLY,
    /**< Packet switched only capable. */

  SYS_SRV_DOMAIN_CS_PS,
    /**< Circuit and packet switched capable. */

  /* 4 */
  SYS_SRV_DOMAIN_CAMPED,
    /**< MS found the right system but not yet registered or attached. */

  /** @cond
  */
  SYS_SRV_DOMAIN_MAX   /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */
} sys_srv_domain_e_type;
/*~ SENTINEL sys_srv_domain_e_type.SYS_SRV_DOMAIN_MAX */

/**
** The following enumerated type defines the CS service capabilities
** of LTE network.
*/
typedef enum
{
  /** @cond
  */
  SYS_LTE_CS_CAPABILITY_NONE = -1,     /* FOR INTERNAL USE ONLY */
  /** @endcond
  */

  SYS_LTE_CS_CAPABILITY_NO_VALUE           = 0,
   /**< FULL service on CS domain is available. */

  SYS_LTE_CS_CAPABILITY_CSFB_NOT_PREFFERED = 1,
   /**< CSFB is not preffered. */

  SYS_LTE_CS_CAPABILITY_SMS_ONLY           = 2,
   /**< CS registation is for SMS only. */

  SYS_LTE_CS_CAPABILITY_LIMITED            = 3,
   /**< Used to covey that CS registation failed for
   ** max attach or TAU attempts. */

  SYS_LTE_CS_CAPABILITY_BARRED             = 4,
   /**< Used to covey that CS registation failed for
   ** max attach or TAU attempts. */

  /** @cond
  */
  SYS_LTE_CS_CAPABILITY_MAX         /* FOR INTERNAL USE ONLY */
  /** @endcond
  */

}sys_lte_cs_capability_e_type;
/*~ SENTINEL sys_lte_cs_capability_e_type.SYS_LTE_CS_CAPABILITY_MAX */

/**
** The following enumerated type defines the UE
** of mode of operation when operating in  LTE network.
*/

typedef enum
{
  /** @cond
  */
  SYS_LTE_UE_MODE_NONE               = -1, /* FOR INTERNAL USE ONLY */
  /** @endcond
  */

  SYS_LTE_UE_MODE_PS_MODE1           = 0,
   /**< UE registers only to EPS services,
       and UE's usage setting is voice centric. */

  SYS_LTE_UE_MODE_PS_MODE2           = 1,
   /**< UE registers only to EPS services,
        and UE's usage setting is data centric. */

  SYS_LTE_UE_MODE_CS_PS_MODE1        = 2,
   /**< UE registers to both EPS and non-EPS services,
        and UE's usage setting  is voice centric. */

  SYS_LTE_UE_MODE_CS_PS_MODE2        = 3,
   /**<  UE registers to both EPS and non-EPS services,
         and UE's usage setting is data centric. */

  /** @cond
  */
  SYS_LTE_UE_MODE_MAX          /* FOR INTERNAL USE ONLY */
  /** @endcond
  */

}sys_lte_ue_mode_of_operation_e_type;
/*~ SENTINEL sys_lte_ue_mode_of_operation_e_type.SYS_LTE_UE_MODE_MAX */


/* Defines UE usage setting
** Voice centric, Data centric and by default value.
** Applicable to LTE capable UE only.
*/
typedef enum
{
  /** @cond
  */
  SYS_UE_USAGE_SETTING_NONE   = -1,  /* FOR INTERNAL USE ONLY */
  /** @endcond */

  SYS_UE_USAGE_SETTING_VOICE_CENTRIC = 0,
    /**< UE usage is set to voice centric. */

  SYS_UE_USAGE_SETTING_DATA_CENTRIC  = 1,
    /**< UE usage is set to data centric. */

  SYS_UE_USAGE_SETTING_NO_CHANGE = 2,
   /**< No change in UE usage setting */

  /**@cond
  */
  SYS_UE_USAGE_SETTING_MAX          /* FOR INTERNAL USE ONLY */
  /**@endcond */

} sys_ue_usage_setting_e_type;
/*~ SENTINEL sys_ue_usage_setting_e_type.SYS_UE_USAGE_SETTING_MAX */

/* Defines UE voice domain preference.
** Applicable to LTE capable UE only.
*/
typedef enum
{
  /** @cond
  */
  SYS_VOICE_DOMAIN_PREF_NONE   = -1,  /* FOR INTERNAL USE ONLY */
  /** @endcond */


  SYS_VOICE_DOMAIN_PREF_CS_VOICE_ONLY   = 0,
    /**< CS Voice only. */

  SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_ONLY   = 1,
    /**< IMS PS Voice only. */

  SYS_VOICE_DOMAIN_PREF_CS_VOICE_PREFERRED   = 2,
    /**< CS voice preferred, IMS PS Voice as secondary. */

  SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_PREFERRED   = 3,
    /**< IMS PS voice preferred, CS Voice as secondary. */

  SYS_VOICE_DOMAIN_PREF_NO_CHANGE = 4,
    /**< No change in voice domain pref */

  /**@cond
  */
  SYS_VOICE_DOMAIN_PREF_MAX   /* FOR INTERNAL USE ONLY */
  /**@endcond */

} sys_voice_domain_pref_e_type;
/*~ SENTINEL sys_voice_domain_pref_e_type.SYS_VOICE_DOMAIN_PREF_MAX */

/* Used to indicate if current LTE service
** supports emergency over IMS or not.
*/
typedef enum
{
  SYS_IMS_LTE_EMERG_SUPP_NONE = -1,
  /* For internal use only
  */

  SYS_IMS_LTE_EMERG_SUPP_NOT_AVAILABLE,
  /* IMS LTE emergency not supported on current system. It indicates that if
  ** LTE is acquired, the current LTE system does not support emergency calls
  ** over IMS.
  */

  SYS_IMS_LTE_EMERG_SUPP_AVAILABLE,
  /* It indicates that if LTE is acquired, the current LTE system supports
  ** emergency calls over IMS.
  */

  SYS_IMS_LTE_EMERG_SUPP_UNKNOWN,
  /* It indicates that if LTE is acquired it is not known whether current
  ** system supports emergency. This can be the state if CM gets camped ind
  ** from NAS but has not yet recieved service confirmation indicating IMS
  ** emergency bearer support
  */

  SYS_IMS_LTE_EMERG_SUPP_MAX
  /* internal use only
  */
}sys_ims_lte_emerg_supp_e_type;

/** @Brief discussion about possible IMS registrations status. */
typedef enum
{
  /** @cond
   */
  SYS_IMS_REG_STATE_UNKNOWN = -1,  /* FOR INTERNAL USE ONLY */
  /** @endcond */

  SYS_IMS_REG_STATE_NEEDS_REG = 0,
    /**<IMS is supported over LTE and needs registration */

  SYS_IMS_REG_STATE_REG_IN_PROGRESS = 1,
    /**< IMS registration is in progress */

  SYS_IMS_REG_STATE_REGISTERED      = 2,
    /**< IMS registration is successfull */

  SYS_IMS_REG_STATE_NEEDS_DEREG = 3,
    /**< IMS De-registration is need */

  SYS_IMS_REG_STATE_DEREG_IN_PROGRESS = 4,
    /**< IMS De-registration is in progress */

  SYS_IMS_REG_STATE_DEREGISTERED = 5,
    /**< IMS is deregistered */

  /**@cond
  */
  SYS_IMS_REG_STATE_MAX  /* FOR INTERNAL USE ONLY */
  /**@endcond */

}sys_ims_reg_state_e_type;
/*~ SENTINEL sys_ims_reg_state_e_type.SYS_IMS_REG_STATE_MAX */

/** Domain selection domains */
typedef enum
{
  /** @cond
   */
  SYS_CM_DOMAIN_SEL_DOMAIN_NONE=-1,
    /**< Voice centric devices: No voice, disabling LTE */

  SYS_CM_DOMAIN_SEL_DOMAIN_IMS,
    /**< Supported for voice and sms domain selection */

  SYS_CM_DOMAIN_SEL_DOMAIN_1X,
    /**< Voice/SMS is supported over 1X network for LTE
    */

  SYS_CM_DOMAIN_SEL_DOMAIN_3GPP,
    /**< Voice/SMS is supported over 3GPP network */

  SYS_CM_DOMAIN_SEL_DOMAIN_NO_VOICE,
    /**< Data centric devices: No voice, stay on LTE */

  SYS_CM_DOMAIN_SEL_DOMAIN_NO_SMS,
    /**< No SMS, stay on LTE */

  SYS_CM_DOMAIN_SEL_HDR_DOMAIN_1X,
    /**< voice domain over HDR is 1x */

  /**@cond
  */
  SYS_CM_DOMAIN_SEL_DOMAIN_MAX  /* Internal use. */
  /**@endcond */

} sys_cm_domain_sel_domain_e_type;
/*~ SENTINEL sys_cm_domain_sel_domain_e_type.SYS_CM_DOMAIN_SEL_DOMAIN_MAX */

/** Error cause codes for IMS registration. */
typedef enum
{
  /** @cond
   */
  SYS_IMS_REG_END_CAUSE_NONE    = -1, /* Internal use. */
  /** @endcond
   */

  SYS_IMS_REG_END_CAUSE_NORMAL  = 0,
  /**< No failure to report. */

  SYS_IMS_REG_END_CAUSE_FAIL    = 1,
  /**< Generic failure from IMS */

  SYS_IMS_REG_END_CAUSE_TEMPORARY    = 2,
      /**< Temporary failure from IMS */
  /** @cond
   */
  SYS_IMS_REG_END_CAUSE_MAX /* Internal use. */
  /** @endcond */
} sys_cm_domain_sel_ims_reg_end_cause_e_type;
/*~ SENTINEL sys_cm_domain_sel_ims_reg_end_cause_e_type.SYS_IMS_REG_END_CAUSE_MAX */

/** IMS stack availability */
typedef enum
{
  /** @cond
   */
  SYS_IMS_STACK_STATUS_NONE    = -1, /* Internal use. */
  /** @endcond
   */

  SYS_IMS_STACK_STATUS_DISABLED  = 0,
  /**< No failure to report. */

  SYS_IMS_STACK_STATUS_ENABLED_QCOM   = 1,
  /**< INT IMS stack is enabled */

  SYS_IMS_STACK_STATUS_ENABLED_THPY    = 2,
   /**< EXT IMS stack is enabled */

  SYS_IMS_STACK_STATUS_MAX /* Internal use. */
  /** @endcond */
}sys_ims_stack_enable_status_e_type;
/*~ SENTINEL sys_cm_domain_sel_ims_reg_end_cause_e_type.SYS_IMS_REG_END_CAUSE_MAX */

/** Service capabilities of a network.
*/
typedef enum
{
  /** @cond
  */
  SYS_SRV_CAPABILITY_NONE = -1,   /*FOR INTERNAL USE ONLY! */
  /** @endcond
  */

  SYS_SRV_CAPABILITY_NOT_AVAILABLE,
    /**< Network cannot provide the service. */

  SYS_SRV_CAPABILITY_CS_SERVICE_ONLY,
    /**< Network can provide a circuit switched service. */

  SYS_SRV_CAPABILITY_PS_SERVICE_ONLY,
    /**< Network can provide a packet switched service. */

  SYS_SRV_CAPABILITY_CS_PS_SERVICE,
    /**< Network can provide a circuit and packet switched service. */

  /** @cond
  */
  SYS_SRV_CAPABILITY_MAX   /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */
} sys_srv_capability_e_type;
/*~ SENTINEL sys_srv_capability_e_type.SYS_SRV_CAPABILITY_MAX */

/** Location services indicator in CS
*/
typedef enum
{
  /** @cond
  */
  SYS_CS_LCS_NONE = -1,   /*FOR INTERNAL USE ONLY! */
  /** @endcond
  */

  SYS_CS_LCS_INFO_NOT_AVAILBLE,
    /**< no information about support of location services
         via CS domain is available */

  SYS_CS_LCS_NOT_SUPPORTED    ,
    /**< location services via CS domain not supported */

  SYS_CS_LCS_SUPPORTED,
    /*< location services via CS domain supported */

  /** @cond
  */
  SYS_CS_LCS_MAX   /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */

}sys_cs_lcs_val_e_type;
/*~ SENTINEL sys_cs_lcs_val_e_type.SYS_CS_LCS_MAX */

/* Defines UE SMS domain preference
** Applicable to LTE capable UE only.
*/
typedef enum
{
  /** @cond
  */
  SYS_SMS_DOMAIN_PREF_NONE   = -1,  /* FOR INTERNAL USE ONLY */
  /** @endcond */


  SYS_SMS_DOMAIN_PREF_PS_SMS_NOT_ALLOWED   = 0,
    /**< SMS service is not to be invoked over IP network */


  SYS_SMS_DOMAIN_PREF_PS_SMS_PREF   = 1,
    /**< SMS service is preferred to be invoked over IP network*/

  /**@cond
  */
  SYS_SMS_DOMAIN_PREF_MAX = 2  /* FOR INTERNAL USE ONLY */
  /**@endcond */

} sys_sms_domain_pref_e_type;
/*~ SENTINEL sys_sms_domain_pref_e_type.SYS_SMS_DOMAIN_PREF_MAX */

/** Roaming status.

The values for these enumerates were derived from the roaming display
indicators described in 3GPP2 C.R1001-A.

@note1hang The actual value of the enumerate is not consistent with the value
listed in the standard (e.g., the values of SYS_ROAM_STATUS_OFF and
SYS_ROAM_STATUS_ON are reversed from the standard).
*/
typedef enum
{
  /** @cond
  */
  SYS_ROAM_STATUS_NONE = -1,   /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */

  /* 0 */
  SYS_ROAM_STATUS_OFF,
    /**< Roaming indicator off. */

  SYS_ROAM_STATUS_ON,
    /**< Roaming indicator on. */

  SYS_ROAM_STATUS_BLINK,
    /**< Roaming indicator flashing. */

  SYS_ROAM_STATUS_OUT_OF_NEIGHBORHOOD,
    /**< Out of neighborhood. */

  /* 4 */
  SYS_ROAM_STATUS_OUT_OF_BLDG,
    /**< Out of building. */

  SYS_ROAM_STATUS_PREF_SYS,
    /**< Roaming: preferred system. */

  SYS_ROAM_STATUS_AVAIL_SYS,
    /**< Roaming: available system. */

  SYS_ROAM_STATUS_ALLIANCE_PARTNER,
    /**< Roaming: alliance partner. */

  /* 8 */
  SYS_ROAM_STATUS_PREMIUM_PARTNER,
    /**< Roaming: premium partner. */

  SYS_ROAM_STATUS_FULL_SVC,
    /**< Roaming: full service functionality. */

  SYS_ROAM_STATUS_PARTIAL_SVC,
    /**< Roaming: partial service functionality. */

  SYS_ROAM_STATUS_BANNER_ON,
    /**< Roaming banner on. */

  /* 12 */
  SYS_ROAM_STATUS_BANNER_OFF,
    /**< Roaming banner off. */

  SYS_ROAM_STATUS_UNKNOWN = 0xFF,
  /** @cond
  */
  SYS_ROAM_STATUS_MAX  = 0xFF+1  /*   FOR INTERNAL USE ONLY!
      Values 0x00 - 0xFF are used in the standard */
  /** @endcond
  */
  
} sys_roam_status_e_type;

/** Radio Access Technologies (RATs).
*/
typedef enum
{
  /** @cond
  */
  SYS_RAT_NONE = -1,  /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */

  SYS_RAT_GSM_RADIO_ACCESS,
    /**< GSM radio access technology. */

  SYS_RAT_UMTS_RADIO_ACCESS,
    /**< UMTS radio access technology. */

  SYS_RAT_LTE_RADIO_ACCESS,
    /**< LTE radio access technology. */

  SYS_RAT_TDS_RADIO_ACCESS,
    /**< TD-SCDMA radio access technology */

  /** @cond
  */
  SYS_RAT_MAX   /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */

} sys_radio_access_tech_e_type;
/*~ SENTINEL sys_radio_access_tech_e_type.SYS_RAT_MAX */

/** State of the SIM card.
*/
typedef enum
{
  /** @cond
  */
  SYS_SIM_STATE_NONE = -1,    /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */

  /* 0 */
  SYS_SIM_STATE_NOT_AVAILABLE,
    /**< The SIM is not available. */

  SYS_SIM_STATE_AVAILABLE,
    /**< The SIM is available. */

  SYS_SIM_STATE_CS_INVALID,
    /**< The SIM has been marked by the network
         as invalid for CS services. */

  SYS_SIM_STATE_PS_INVALID,
    /**< The SIM has been marked by the network
         as invalid for PS services. */

  /* 4 */
  SYS_SIM_STATE_CS_PS_INVALID,
    /**< The SIM has been marked by the network
         as invalid for CS and PS services. */

  /** @cond
  */
  SYS_SIM_STATE_MAX   /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */
} sys_sim_state_e_type;
/*~ SENTINEL sys_sim_state_e_type.SYS_SIM_STATE_MAX */



/** PLMN domain in CAMP_ONLY mode.
*/
typedef enum
{
  /** @cond
  */
 SYS_REG_DOMAIN_NONE = -1,   /*FOR INTERNAL USE ONLY! */
  /** @endcond
  */

 SYS_REG_DOMAIN_NOT_APPLICABLE,
    /**< This to indicate that the field is not applicable.
         This value will be set in other than CAMP_ONLY mode. */

 SYS_REG_DOMAIN_NO_SERVICE,
    /**<  OOS service. */

 SYS_REG_DOMAIN_CAMPED_ONLY,
    /**< Plmn cannot provide the service. */

 SYS_REG_DOMAIN_CS_SERVICE_ONLY,
    /**< Plmn can provide a circuit switched service. */

 SYS_REG_DOMAIN_PS_SERVICE_ONLY,
    /**< Plmn can provide a packet switched service. */

 SYS_REG_DOMAIN_CS_PS_SERVICE,
    /**< Plmn can provide a circuit and packet switched service. */

  /** @cond
  */
 SYS_REG_DOMAIN_MAX   /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */
} sys_reg_domain_e_type;


/** MSM identification.
*/
typedef enum
{
  /** @cond
  */
  SYS_MSM_ID_NONE = -1,   /* used only for bounds checking. */
  /** @endcond
  */
  /* 0 */
  SYS_MSM_ID_6500,
    /**< MSM ID is 6500. */

  SYS_MSM_ID_6250,
    /**< MSM ID is 6250. */

  SYS_MSM_ID_MAIN,
    /**< MSM that has control of DPRAM. */

  SYS_MSM_ID_SECONDARY,
    /**< MSM that does not have control of DPRAM. */

  /* 4 */
  /** @cond
  */
  SYS_MSM_ID_MAX   /* Used for arrays & bounds checking. */
  /** @endcond
  */
} sys_msm_id_e_type;
/*~ SENTINEL sys_msm_id_e_type.SYS_MSM_ID_MAX */

/*--------------------------------------------------------------------------
              Domain Specific Access Control (DSAC) information
---------------------------------------------------------------------------*/

/** Enumeration of Domain access state information
*/
typedef enum
{
  SYS_CELL_ACCESS_NONE = -1,
    /**< FOR INTERNAL USE OF CM ONLY!
    */

  SYS_CELL_ACCESS_NORMAL_ONLY,
    /**< Normal Calls are only allowed
    */

  SYS_CELL_ACCESS_EMERGENCY_ONLY,
    /**< Emergency calls are only allowed
    */

  SYS_CELL_ACCESS_NO_CALLS,
    /**< No calls are allowed
    */

  SYS_CELL_ACCESS_ALL_CALLS,
    /**< All calls are allowed
    */

  SYS_CELL_ACCESS_MAX
    /**< FOR INTERNAL USE OF CM ONLY!
    */
}sys_cell_access_status_e_type;
/*~ SENTINEL sys_cell_access_status_e_type.SYS_CELL_ACCESS_MAX */

/** Structure to define a DSAC information
*/
typedef struct
{
  sys_cell_access_status_e_type    cs_bar_status;
    /**< Call barring status for CS calls */

  sys_cell_access_status_e_type    ps_bar_status;
   /**< Call barring status for PS calls */

}sys_domain_access_bar_s_type;

/** LTE emergency call access barred information provided by LTE RRC
    from SIB2 message.
*/
typedef enum
{
  /** @cond
  */
  SYS_LTE_EMERG_ACCESS_NONE = -1,
  /* For Internal CM use */

  SYS_LTE_EMERG_ACCESS_ALLOWED,
  /**< Emergency call access allowed for the cell */

  SYS_LTE_EMERG_ACCESS_BARRED,
  /**< Emergency call access barred for the cell */

  SYS_LTE_EMERG_ACCESS_UNAVAILABLE,
  /**< Emergency call barred status not available from Lower layers */

  SYS_LTE_EMERG_ACCESS_MAX
  /*   For Internal CM use */

  /** @endcond
  */
}sys_lte_emerg_access_status_e_type;

/** Simultaneous modes, needed for Mobile Station Modem (MSM) 7600 to distinguish
    between WCDMA/1X (WX) and non-WX modes of operation.
*/
typedef enum
{
  /** @cond
  */
  SYS_SIMULTANEOUS_MODE_NONE = -1, /*   For Internal SD use */
  /** @endcond
  */
  SYS_SIMULTANEOUS_MODE_NON_WX,    /**< Not WX mode. */

  SYS_SIMULTANEOUS_MODE_WX ,       /**< Simultaneous WCDMA and 1X. */

  /** @cond
  */
  SYS_SIMULTANEOUS_MODE_MAX        /*   For Internal SD use */
  /** @endcond
  */

} sys_simultaneous_mode_e_type;
/*~ SENTINEL sys_simultaneous_mode_e_type.SYS_SIMULTANEOUS_MODE_MAX */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** High-Speed Downlink Packet Access (HSDPA) and High-Speed Uplink Packet
    Access (HSUPA) indication types.

    HSDPA and HSUPA are available with WCDMA and TD-SCDMA services.
*/
typedef enum
{
  /** @cond
  */
  SYS_HS_IND_NONE = -1,  /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

  SYS_HS_IND_HSDPA_HSUPA_UNSUPP_CELL,
    /**< Cell does not support either HSDPA or HSUPA. */

  SYS_HS_IND_HSDPA_SUPP_CELL,
    /**< Cell supports HSDPA. */

  SYS_HS_IND_HSUPA_SUPP_CELL,
    /**< Cell supports HSUPA. */

  SYS_HS_IND_HSDPA_HSUPA_SUPP_CELL,
    /**< Cell supports HSDPA and HSUPA. */

  /* HSDPA_PLUS status indicator values */

  SYS_HS_IND_HSDPAPLUS_SUPP_CELL,
    /**< Cell supports HSDPA+. */

  SYS_HS_IND_HSDPAPLUS_HSUPA_SUPP_CELL,
    /**< Cell supports HSDPA+ and HSUPA. */

  SYS_HS_IND_DC_HSDPAPLUS_SUPP_CELL,
    /**< Cell supports DC HSDPA+. */

  SYS_HS_IND_DC_HSDPAPLUS_HSUPA_SUPP_CELL,
    /**< Cell supports DC HSDPA+ and HSUPA. */

  SYS_HS_IND_HSDPAPLUS_64QAM_HSUPA_SUPP_CELL,
    /**< Cell supports 64QAM HSDPA+ and HSUPA */

  SYS_HS_IND_HSDPAPLUS_64QAM_SUPP_CELL,
    /**< Cell supports 64QAM HSDPA+  */

  SYS_HS_IND_DC_HSDPAPLUS_DC_HSUPA_SUPP_CELL,
    /**< Cell supports DC HSUPA. */

  /** @cond
  */
  SYS_HS_IND_MAX  /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */
} sys_hs_ind_e_type;
/*~ SENTINEL sys_hs_ind_e_type.SYS_HS_IND_MAX */

/** Dual Transfer Mode (DTM) allows simultaneous CS and PS calls only during GSM
    service.
*/
typedef enum
{
  /** @cond
  */
  SYS_DTM_SUPPORT_NONE = -1,  /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

  SYS_DTM_SUPPORT_NOT_AVAIL,
    /**< Dual Transfer mode support is not available. */

  SYS_DTM_SUPPORT_AVAIL,
    /**< Dual Transfer mode support is available. */

  /** @cond
  */
  SYS_DTM_SUPPORT_MAX  /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

} sys_dtm_support_e_type;
/*~ SENTINEL sys_dtm_support_e_type.SYS_DTM_SUPPORT_MAX */

/** Indicates whether Evolved GPRS (EGPRS) is available.
*/
typedef enum
{
  /** @cond
  */
  SYS_EGPRS_SUPPORT_NONE = -1,  /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

  SYS_EGPRS_SUPPORT_NOT_AVAIL,
    /**< EGPRS support is not available. */

  SYS_EGPRS_SUPPORT_AVAIL,
    /**< EGPRS support is available. */

  /** @cond
  */
  SYS_EGPRS_SUPPORT_MAX   /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

} sys_egprs_support_e_type;
/*~ SENTINEL sys_egprs_support_e_type.SYS_EGPRS_SUPPORT_MAX */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** Maximum length for a user zone name.
*/
#define  SYS_PUZL_MAX_UZ_NAME_LEN          32

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** @brief Name of the current user zone selected in a CDMA system.
*/
typedef struct
{
  byte     uz_name_encoding;
    /**< Type of encoding used for the name. */

  byte     uz_name_n_octets;
    /**< Number of bytes used for the name. */

  byte     uz_name[SYS_PUZL_MAX_UZ_NAME_LEN];
    /**< Buffer to hold encoded octets. */

} sys_puzl_uz_name_s_type;

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/*------------------------------------------------------------------------------
   HDR related Defines^M
------------------------------------------------------------------------------*/

/** HDR acquisition modes.
*/
typedef enum {

  SYS_HDR_ACQ_MEASURE_RF,       /**< Only acquire if the raw power is high
                                     enough. */

  SYS_HDR_ACQ_MODE_MICRO,       /**< Micro acquisitions. */

  SYS_HDR_ACQ_MODE_SHALLOW,     /**< Shallow acquisitions only. */

  SYS_HDR_ACQ_MODE_DEEP,        /**< Deep acquisitions only. */

  SYS_HDR_ACQ_MODE_FULL,        /**< Shallow and deep acquisitions. */

  /** @cond
  */
  SYS_HDR_ACQ_MODE_MAX          /* For internal use only */
  /** @endcond
  */

} sys_hdr_acq_mode_e_type;
/*~ SENTINEL sys_hdr_acq_mode_e_type.SYS_HDR_ACQ_MODE_MAX */

/** Active protocol revision for various technologies.
*/
typedef enum {


  SYS_ACTIVE_PROT_NONE,           /**< The protocol revision is unknown.
                                       When HDR system ID is first acquired
                                       and no protocol revision is
                                       available at this point. */

  SYS_ACTIVE_PROT_HDR_START,      /**< Lower boundary (excluding) for a valid
                                       HDR protocol revision */

  SYS_ACTIVE_PROT_HDR_REL0,       /**< HDR release 0. */

  SYS_ACTIVE_PROT_HDR_RELA,       /**< HDR release A (physical layer
                                       sub-type 2 is active in the current
                                       personality). */

  SYS_ACTIVE_PROT_HDR_RELB,       /**< HDR release B. */

  SYS_ACTIVE_PROT_HDR_END,        /**< Internal use for HDR range check. */

                                  /* If adding other technologies, pls
                                     reserve some entries for HDR. */

  /** @cond
  */
  SYS_ACTIVE_PROT_MAX             /*   Internal use for enum range check. */
  /** @endcond
  */

} sys_active_prot_e_type;
/*~ SENTINEL sys_active_prot_e_type.SYS_ACTIVE_PROT_MAX */

/** Personality types for various technologies. The currently defined
    enumerations are applicable only for HDR.
*/
typedef enum {

  SYS_PERSONALITY_NONE,           /**< The personality is unknown (e.g., when
                       the system is first acquired and no
                     personality is available at that point). */

  SYS_PERSONALITY_HDR_START,      /**< For range checking. */

  SYS_PERSONALITY_HRPD,           /**< Personality does not have EHRPD (Evolved High Rate Packet Data)
                                       capability. */

  SYS_PERSONALITY_EHRPD,          /**< Personality has EHRPD capability. */

  SYS_PERSONALITY_HDR_END,        /**< Upper limit for HDR personality range. */

  /* If adding other technologies, please reserve some entries for HDR. */

  /** @cond
  */
  SYS_PERSONALITY_MAX             /*   Internal use for enum range check */
  /** @endcond
  */

} sys_personality_e_type;


/** PDP header compression types.
*/
typedef enum
{
  /** @cond
  */
  SYS_PDP_HEADER_COMP_NONE = -1,  /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

  SYS_PDP_HEADER_COMP_OFF = 0,
    /**< PDP header compression is OFF. Preserves earlier values of
         h_comp=FALSE for no header compression. */

  SYS_PDP_HEADER_COMP_ON  = 1,
    /**< PDP header compression is ON. Preserves earlier value of h_comp=TRUE
         for default header compression. */

  SYS_PDP_HEADER_COMP_RFC1144,
    /**< PDP header compression based on RFC 1144.
         3GPP TS 44.065 (section 6.5.1.1.4). */

  SYS_PDP_HEADER_COMP_RFC2507,
    /**< PDP header compression based on RFC 2507.
         3GPP TS 44.065 (section 6.5.1.1.4). */

  SYS_PDP_HEADER_COMP_RFC3095,
    /**< PDP header compression based on RFC 3095.
         3GPP TS 44.065 release-6 version-6.5.0
        (sections 6.5.4 and 6.5.5). */

  /** @cond
  */
  SYS_PDP_HEADER_COMP_MAX   /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

} sys_pdp_header_comp_e_type;
/*~ SENTINEL sys_pdp_header_comp_e_type.SYS_PDP_HEADER_COMP_MAX */

/** PDP data compression types.
*/
typedef enum
{
  /** @cond
  */
  SYS_PDP_DATA_COMP_NONE = -1,  /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */
  SYS_PDP_DATA_COMP_OFF = 0,
    /**< PDP data compression is OFF. Preserves the earlier value of
       d_comp=FALSE for no data compression. */

  SYS_PDP_DATA_COMP_ON  = 1,
    /**< Default PDP data compression is enabled. Preserves the earlier
       value of d_comp=TRUE for default data compression. */

  SYS_PDP_DATA_COMP_V42_BIS,
    /**< Data compression standard specified in
         3GPP TS 44.065 (section 6.6.1.1.4). */

  SYS_PDP_DATA_COMP_V44,
    /**< Data compression standard specified in
         3GPP TS 44.065 (section 6.6.1.1.4). */

  /** @cond
  */
  SYS_PDP_DATA_COMP_MAX  /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

} sys_pdp_data_comp_e_type;
/*~ SENTINEL sys_pdp_data_comp_e_type.SYS_PDP_DATA_COMP_MAX */


/**
  DRX
 */
typedef enum sys_drx_cn_coefficient_s1_e
{
  /** @cond
  */
  SYS_DRX_CN_COEFFICIENT_S1_NONE = -1,  /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */
  SYS_DRX_NOT_SPECIFIED_BY_MS = 0,
  SYS_DRX_CN_COEFFICIENT_S1_CN6_T32   = 6,    /*!< CN = 6,  T = 32 */
  SYS_DRX_CN_COEFFICIENT_S1_CN7_T64   = 7,    /*!< CN = 7, T = 64 */
  SYS_DRX_CN_COEFFICIENT_S1_CN8_T128  = 8,    /*!< CN = 8, T = 128 */
  SYS_DRX_CN_COEFFICIENT_S1_CN9_T256  = 9,    /*!< CN = 9 T = 256 */
  /** @cond
  */
  SYS_DRX_CN_COEFFICIENT_S1_MAX         /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */
} sys_drx_cn_coefficient_s1_e_type;
/*~ SENTINEL sys_drx_cn_coefficient_s1_e_type.SYS_DRX_CN_COEFFICIENT_S1_MAX */


/**
  NAS procedures
** NAS procedures   - order of following procedures is to be maintained
*/
typedef enum
{

  /** @cond
  */
  SYS_NAS_PROCEDURE_NONE = -1,  /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

  SYS_NAS_PROCEDURE_MM = 1,
    /**< MM Procedure */

  SYS_NAS_PROCEDURE_CM,
    /**< CM Procedure */

  SYS_NAS_PROCEDURE_GMM_NON_COMBINED,
    /**< GMM Non-combined Procedure */

  SYS_NAS_PROCEDURE_SM,
    /**< SM Procedure */

  SYS_NAS_PROCEDURE_GMM_COMBINED,
    /**< GMM Combined Procedure */

  SYS_NAS_PROCEDURE_MM_GMM_COMBINED,
    /**< MM GMM Combined Procedure */

  SYS_NAS_PROCEDURE_EMM_NON_COMBINED_EPS,
    /**< EMM Non-combined EPS Procedure */

  SYS_NAS_PROCEDURE_EMM_COMBINED_NON_EPS,
    /**< EMM Combined Non-EPS Procedure */

  SYS_NAS_PROCEDURE_EMM_COMBINED,
    /**< EMM Combined Procedure */

  SYS_NAS_PROCEDURE_ESM,
    /**< ESM Procedure */

  /** @cond
  */
  SYS_NAS_PROCEDURE_MAX  /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */
}sys_nas_procedure_e_type;
/*~ SENTINEL sys_nas_procedure_e_type.SYS_NAS_PROCEDURE_MAX */


/*------------------------------------------------------------------------------
   WLAN related defines
------------------------------------------------------------------------------*/

/** WLAN scan types.
*/
typedef enum
{
  /** @cond
  */
  SYS_WLAN_SCAN_TYPE_NONE = -1,  /*   FOR INTERNAL USE ONLY! */
  /** @endcond
  */

  SYS_WLAN_SCAN_TYPE_ACTIVE,
    /**< Active scanning.  */

  SYS_WLAN_SCAN_TYPE_PASSIVE,
    /**< Passive scanning. */

  /** @cond
  */
  /* FOR INTERNAL USE ONLY! */
  SYS_WLAN_SCAN_TYPE_MAX
  /** @endcond
  */

} sys_wlan_scan_type_e_type;
/*~ SENTINEL sys_wlan_scan_type_e_type.SYS_WLAN_SCAN_TYPE_MAX */


/** WLAN BSS types.
*/
typedef enum
{
  /** @cond
  */
  SYS_WLAN_BSS_TYPE_NONE = -1,  /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */

  SYS_WLAN_BSS_TYPE_ADHOC,
    /**< Adhoc type BSS. */

  SYS_WLAN_BSS_TYPE_INFRA,
    /**< Infrastructure mode BSS. */

  SYS_WLAN_BSS_TYPE_ANY,
    /**< Any BSS mode type. */

  SYS_WLAN_BSS_TYPE_ADHOC_START,
    /**< Adhoc type BSS created by the MS. */

  /** @cond
  */
  SYS_WLAN_BSS_TYPE_MAX   /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */

} sys_wlan_bss_e_type;
/*~ SENTINEL sys_wlan_bss_e_type.SYS_WLAN_BSS_TYPE_MAX */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** @brief SSID of WLAN system.
*/
typedef struct sys_wlan_ssid_s
{

  uint8                                len;
    /**< Length of the SSID; if == 0, then SSID = broadcast SSID.  */

  char                                 ssid[SYS_WLAN_SSID_MAX_SIZE];
    /**< SSID of the WLAN system. */

} sys_wlan_ssid_s_type;

/*~ FIELD sys_wlan_ssid_s.ssid VARRAY SYS_WLAN_SSID_MAX_SIZE
    LENGTH sys_wlan_ssid_s.len */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** Technology type and version.
*/
typedef enum
{
  /** @cond
  */
  SYS_TECH_NONE                        = -1,  /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

  SYS_TECH_WLAN_80211A                 = 0,
    /**< WLAN 802.11a technology. */

  SYS_TECH_WLAN_80211B                 = 1,
    /**< WLAN 802.11b technology. */

  SYS_TECH_WLAN_80211G                 = 2,
    /**< WLAN 802.11g technology. */

  /** @cond
  */
  SYS_TECH_RESERVED                    = 30,  /* Reserved values for CM use */
  /** @endcond
  */

  /** @cond
  */
    SYS_TECH_MAX   /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

} sys_tech_e_type;
/*~ SENTINEL sys_tech_e_type.SYS_TECH_MAX */

/** Turns hybrid mode on and off.
*/
typedef enum
{
  /** @cond
  */
    SYS_HYBR_PREF_NONE                      = -1, /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

  SYS_HYBR_PREF_OFF                         = 0,
    /**< Hybrid mode is OFF. */

  SYS_HYBR_PREF_ON                          = 1,
    /**< Hybrid mode is ON. */

  /** @cond
  */
  SYS_BYBR_PREF_MAX     /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

} sys_hybr_pref_e_type;
/*~ SENTINEL sys_hybr_pref_e_type.SYS_BYBR_PREF_MAX */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

typedef union
{
  sys_band_mask_e_type  chgwt_band_cap;
    /**< UE capable bands for the associated RAT used for
         reselection, redirection, and a PLMN search. */

  sys_lte_band_mask_e_type  lte_band_cap;
    /**< UE capable bands for the associated RAT used for
         reselection, redirection, and a PLMN search. */
}sys_band_cap_u_type;


/** @brief Information related to each RAT in the RAT priority list.
*/
typedef struct   sys_priority_list_info_s
{

  sys_band_cap_u_type   band_cap;
    /**< UE capable bands for the associated RAT used for
         reselection, redirection, and a PLMN search. */

  sys_sys_mode_e_type   acq_sys_mode;
    /**< Defines the RAT. */

  uint32 acq_sys_time_interval;
    /**< Time interval given for a RAT to acquire a system. */

  boolean               bst_rat_acq_required;
    /**< This flag decides if an acquisition on the associated RAT should occur
       as part of the current service request. */

  sys_band_cap_u_type   bst_band_cap;
    /**< The bands that have to be searched as part of this service request
         if bst_rat_acq_required = TRUE. If bst_rat_acq_required = FALSE,
         bst_band_cap should be ignored and bst_band_cap is used for PLMN/cell
         selection. */

} sys_priority_list_info_s_type;

/** @brief Type used by the Enhanced Out-of-Service algorithms. This type
    specifies if a timer-based search should be used during system acquisition
    and if the next search is a continuation of the previous one.
*/
typedef struct eoos_gwl_scan_s
{
  boolean      new_scan;
    /**< Continue with the previous search or start a new search */

  boolean      use_timer;
    /**<  Use a timer-based search if TRUE. */
} eoos_gwl_scan_s_type;

/** @brief RAT priority list.
*/
typedef struct sys_rat_pri_list_info_s
{

   uint32 num_items;
      /**< Number of valid items in the list. */

   uint32 next_acq_sys_index;
      /**< Pointer to which 3GPP system will be the next to be acquired. For
         example, with priority_list_info [GSM(index=0), 1X, WCDMA, LTE],
       if the next_acq_sys_index is 2, the WCDMA system is the first that
       will be attempted to be acquired, then the LTE system. */

   sys_priority_list_info_s_type  priority_list_info[SYS_MAX_ACQ_PREF_RAT_LIST_NUM];
      /**< In the priority_list_info, system acquisition order starts from 0,
         so index 0 is first, and SYS_MAX_ACQ_PREF_RAT_LIST_NUM-1 is the last
       to be attempted. For example, with priority_list_info
       [GSM(index=0)1X WCDMA LTE], GSM is first preferred to be acquired,
       second is 1X, third is WCDMA, and LTE is last. */

   eoos_gwl_scan_s_type scan_type;
      /**< The type of scan to be used during 3GPP system acquisition. */

}sys_rat_pri_list_info_s_type;

/** @brief UE capability.
*/
typedef struct sys_mmss_ue_capability_s
{
   sys_rat_pri_list_info_s_type    rat_pri_list_info;
     /**< RAT priority list information. */

} sys_mmss_ue_capability_s_type;

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */


/*------------------------------------------------------------------------------
   Service Indicator Data Types
------------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif

/** System ID type.
*/
typedef enum
{

  SYS_SYS_ID_TYPE_UNDEFINED,
    /**< System ID is undefined. */

  SYS_SYS_ID_TYPE_UMTS,
    /**< UMTS (WCDMA/GSM/LTE) system ID. */

  SYS_SYS_ID_TYPE_IS95,
    /**< IS-95 system ID. */

  SYS_SYS_ID_TYPE_IS856,
    /**< IS-856 (HDR) system ID. */

  SYS_SYS_ID_TYPE_WLAN
    /**< WLAN system ID. */

} sys_sys_id_type_e_type;

typedef enum
{

  SYS_CELL_BROADCAST_CAPABILITY_NONE = -1,
    /**< Internal use only */

  SYS_CELL_BROADCAST_CAPABILITY_UNKNOWN,
    /**< Cell broadcast capability not known */

  SYS_CELL_BROADCAST_CAPABILITY_OFF,
    /**< Cell broadcast capability OFF */

  SYS_CELL_BROADCAST_CAPABILITY_ON,
    /**< Cell broadcast capability ON */

  SYS_CELL_BROADCAST_CAPABILITY_MAX
    /**< Internal use only */

}sys_cell_broadcast_cap_e_type;
/*~ SENTINEL sys_cell_broadcast_cap_e_type.SYS_CELL_BROADCAST_CAPABILITY_MAX */


/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */


/*------------------------------------------------------------------------------
   System Identifier Data Types
------------------------------------------------------------------------------*/
/** @brief IS-95 system ID.
*/
typedef struct sys_is95_sys_id_s
{
  uint16         sid;
    /**< System ID. */

  uint16         nid;
    /**< Network ID. */

  word           mcc;
    /**< Mobile County Code. */

  byte           imsi_11_12;
    /**< IMSI_11_12. */

} sys_is95_sys_id_s_type;

/** Public Land Mobile Network (PLMN) ID.

  A PLMN ID is defined within a GSM network to be the combination of a Mobile
  Country Code (MCC) and Mobile Network Code (MNC). A PLMN ID is stored in
  three octets, as specified in 3G TS 24.008, Section 10.5.1.3 and shown below:

  <pre>
                                        Bit
                 |  8  |  7  |  6  |  5  |  4  |  3  |  2  |  1  |
                 |===============================================|
         octet 1 |      MCC Digit 2      |      MCC Digit 1      |
                 |-----------------------------------------------|
         octet 2 |      MNC Digit 3      |      MCC Digit 3      |
                 |-----------------------------------------------|
         octet 3 |      MNC Digit 2      |      MNC Digit 1      |
                 |===============================================|
  </pre>

  @brief The criteria for comparing one PLMN against another is specified in
  3GPP TS 23.122 Annex A.
*/
typedef struct sys_plmn_id_s
{

  byte                                identity[3];
    /**< Public Land Mobile Network ID. */

} sys_plmn_id_s_type;

/** Location Area Code (LAC).
*/
typedef word                          sys_lac_type;

/** @brief PLMN plus LAC.
*/
typedef struct sys_plmn_lac_id_s
{
  sys_plmn_id_s_type   plmn;  /**< Public Land Mobile Network. */
  sys_lac_type         lac;   /**< Location Area Code. */
}sys_plmn_lac_id_s_type;

/** @brief Type for system ID.
*/
typedef union sys_sys_id_u
{

  sys_is95_sys_id_s_type               is95;
    /**< IS-95 system ID. */

  byte                                 is856[16];
    /**< IS-856 system ID. */

  sys_plmn_id_s_type                   plmn;
    /**< UMTS (WCDMA/GSM) system ID. */

  sys_wlan_ssid_s_type                 ssid;
    /**< WLAN system ID. */

  sys_plmn_lac_id_s_type  plmn_lac;
    /**< UMTS (WCDMA/GSM) PLMN and LAC ID. */

} sys_sys_id_u_type;

/*~ PARTIAL sys_sys_id_u */
/*~ CASE SYS_SYS_ID_TYPE_UNDEFINED sys_sys_id_u.void */
/*~ CASE SYS_SYS_ID_TYPE_IS95      sys_sys_id_u.is95 */
/*~ CASE SYS_SYS_ID_TYPE_IS856     sys_sys_id_u.is856 */
/*~ CASE SYS_SYS_ID_TYPE_UMTS      sys_sys_id_u.plmn_lac */
/*~ CASE SYS_SYS_ID_TYPE_WLAN      sys_sys_id_u.ssid */

/** @brief Groups information for wireless network systems under acquisition.
*/
typedef struct sys_sys_id_s
{

  sys_sys_id_type_e_type              id_type;
    /**< Type of the system ID. */

  sys_sys_id_u_type                   id;
    /**< The system identifier. */

} sys_sys_id_s_type;
/*~ FIELD sys_sys_id_s.id DISC sys_sys_id_s.id_type */

/** Cell ID.
*/
typedef uint32                        sys_cell_id_type;

/** @brief Camped cell information.
*/
typedef struct sys_cell_info_s
{
  sys_cell_id_type         cell_id;
    /**< Cell ID. */

  sys_plmn_id_s_type       plmn_id;
    /**<  PLMN ID of the serving system. */

  sys_lac_type             lac_id;
    /**<  LAC of the serving system. */

  uint16                   arfcn;
    /**< Absolute Radio Frequency Channel Number (ARFCN) of the serving system
       (applicable only in GSM mode). Range is 0 to 1023. */

  uint8                    bsic;
    /**< Base Station Identity Code of the serving system (applicable only
       in GSM mode). Range is 0 to 63 (see 3GPP TS 24.008, section
     10.5.1.3). */

  uint16                   psc;
    /**< The serving Primary Scrambling Code. Applicable for WCDMA systems */

  uint16                   uarfcn_dl;
    /**< Downlink the UMTS Terrestrial Radio Access (UTRA) Absolute Radio
         Frequency Channel Number (UARFCN) of the serving system. */

  uint16                   uarfcn_ul;
    /**< Uplink the UARFCN of the serving system.  */

  uint16                   refpn;
    /**< Pseudo-random Noise (PN) number of the base station (applicable only
         in CDMA mode). */

  uint32                   earfcn_dl;
    /**< Downlink the Evolved-UTRA ARFCN (EARFCN) of the serving system.
         Applicable for LTE systems. */

  uint32                   earfcn_ul;
    /**<  Uplink the EARFCN of serving system. Applicable for LTE systems. */

  sys_sys_mode_e_type      as_sys_mode;
    /**< The AS RAT info for the cell info. */

  sys_lac_type             tac;
    /**< TAC information for LTE. */

  sys_lte_emerg_access_status_e_type   emerg_access_status;
    /**< Emergency access barred info from SIB2 message */

  sys_cell_access_status_e_type        lte_cell_access_status;
    /**< Cell access info for the LTE cell */
  
  sys_band_mask_e_type     current_band;
   /** < band mask */

}sys_cell_info_s_type;

/** Mobile country code type.
*/
typedef uint32                        sys_mcc_type;

/** Mobile network code type.
*/
typedef uint32                        sys_mnc_type;

/** Profile ID.
*/
typedef byte                          sys_profile_id_type;

#define SD_PLMN_MCC_LIST_MAX 15

typedef struct {
  /*valid entries in the list
  */
  uint8     count;
 /* the MCC list
 */
  uint32 mcc_list[SD_PLMN_MCC_LIST_MAX];

} sys_plmn_mcc_list;

  /* pointer to sys_plmn_mcc_list, to hold the PLMN sys MCCs for HDR
  */


#ifdef __cplusplus
extern "C" {
#endif

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME  sys_plmn_get_mcc_mnc

==============================================================================*/
/**
  Returns the MCC and MNC contained in a PLMN ID as integer values.

@note1hang This is a re-entrant function.

@param[in] plmn                       PLMN provided as the input from which
                                      to read the MCC and MNC.
@param[in] plmn_is_undefined_ptr      Flag indicating that the input PLMN
                                      contains a defined MCC and MNC
                                      (FALSE) or was set to 0xFFFFFF
                                      (TRUE).
@param[in] mnc_includes_pcs_digit_ptr Flag indicating whether the
                                      MNC included the third PCS digit
                                      (TRUE) or not (FALSE).
@param[in] mcc_ptr                    MCC converted from binary coded
                                      decimal digits in the PLMN to an
                                      integer.
@param[in] mnc_ptr                    MNC converted from binary coded
                                      decimal digits in the PLMN to an
                                      integer.

@return
  None.

@dependencies
  None.
*/
extern void sys_plmn_get_mcc_mnc
(
  sys_plmn_id_s_type  plmn,
  boolean*            plmn_is_undefined_ptr,
  boolean*            mnc_includes_pcs_digit_ptr,
  sys_mcc_type*       mcc_ptr,
  sys_mnc_type*       mnc_ptr
);
/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME  sys_plmn_get_mcc
   
==============================================================================*/
/**
  Returns the MCC contained in a PLMN ID as integer value.

@note1hang This is a re-entrant function.

@param[in] plmn                       PLMN provided as the input from which
                                      to read the MCC and MNC.

@param[in] mcc_ptr                    MCC converted from binary coded
                                      decimal digits in the PLMN to an
                                      integer.

@return
  None.

@dependencies
  None.
*/
extern void sys_plmn_get_mcc
(
  sys_plmn_id_s_type  plmn,
  sys_mcc_type*       mcc_ptr
);

#ifdef __cplusplus
}
#endif

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */


/** Wildcard MCC value.
*/
#define SYS_WILDCARD_MCC             0xFFF

/** Wildcard MNC value.
*/
#define SYS_WILDCARD_MNC             0xFFF

/** Maximum number of 3GPP RATs supported: GSM, WCDMA, TD-SCDMA, and LTE.
*/
#define SYS_MAX_NUM_3GPP_RATS        4

/** Size of the PLMN with Active entry in the PLMN DB.
*/
#define SYS_PLMN_W_ACT_ENTRY_SIZE    5

/** Maximum network information list.
*/
#define SYS_NETWORK_LIST_INFO_MAX          (int)(25)

/** Minimum network information list.
*/
#define SYS_NETWORK_LIST_INFO_MIN          (int)(1)

/** Maximum number of user-preferred or controlled PLMNs.
*/
#define SYS_USER_PPLMN_LIST_MAX_LENGTH      85

/** Minimum number of user-preferred or controlled PLMNs.
*/
#define SYS_USER_PPLMN_LIST_MIN_LENGTH      1

/** Maximum length of the PLMN list.
*/
#define SYS_PLMN_LIST_MAX_LENGTH            40

#define SYS_EHPLMN_LIST_MAX_LENGTH         128

/** Minimum length of the PLMN list.
*/
#define SYS_PLMN_LIST_MIN_LENGTH             1

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/*------------------------------------------------------------------------------
   Network Lists Enums
------------------------------------------------------------------------------*/

/** Type of PLMN in the PLMN list.
*/
typedef enum
{
  /** @cond
  */
  SYS_DETAILED_PLMN_LIST_CATEGORY_NONE = -1,     /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */

  SYS_DETAILED_PLMN_LIST_CATEGORY_HPLMN,
    /**< Home PLMN. */

  SYS_DETAILED_PLMN_LIST_CATEGORY_PREFERRED,
    /**< Preferred PLMN category. */

  SYS_DETAILED_PLMN_LIST_CATEGORY_USER_PREFERRED,
    /**< User controlled PLMN (preferred) stored on the SIM. */

  SYS_DETAILED_PLMN_LIST_CATEGORY_OPERATOR_PREFERRED,
    /**< Operator controlled PLMN stored on the SIM. */

  SYS_DETAILED_PLMN_LIST_CATEGORY_OTHER,
    /**< Other. */

  SYS_DETAILED_PLMN_LIST_ALLOWED_CSG,
    /**< Allowed CSG list */

  SYS_DETAILED_PLMN_LIST_OPERATOR_CSG,
    /**< Operator CSG list */

  SYS_DETAILED_PLMN_LIST_OTHER_CSG,
    /**< CSGs not in Allowed/Operator List */

  /** @cond
  */
  SYS_DETAILED_PLMN_LIST_CATEGORY_MAX            /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */
} sys_detailed_plmn_list_category_e_type;
/*~ SENTINEL sys_detailed_plmn_list_category_e_type.SYS_DETAILED_PLMN_LIST_CATEGORY_MAX */

/** PLMN's signal quality.
*/
typedef enum
{
  /** @cond
  */
  SYS_SIGNAL_QUALITY_NONE = -1,                  /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */
  SYS_SIGNAL_QUALITY_HIGH,    /**< &nbsp; */
  SYS_SIGNAL_QUALITY_LOW,     /**< &nbsp; */

  /** @cond
  */
  SYS_SIGNAL_QUALITY_MAX                         /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */

} sys_signal_quality_e_type;
/*~ SENTINEL sys_signal_quality_e_type.SYS_SIGNAL_QUALITY_MAX */

/** Access technologies that are defined for the elementary file,
    EFPLMNwAcT (user controlled PLMN selector with access technology). The
    #defines are to be used in a bitmask. Refer to 3GPP TS 11.11.
*/
typedef uint16 sys_access_tech_e_type;

/** Status of a manual PLMN search.
*/
typedef enum
{
  /** @cond
  */
  SYS_PLMN_LIST_NONE = -1,       /* FOR INTERNAL USE ONLY */
  /** @endcond
  */

  SYS_PLMN_LIST_SUCCESS,    /**< &nbsp; */
  SYS_PLMN_LIST_AS_ABORT,   /**< &nbsp; */
  SYS_PLMN_LIST_REJ_IN_RLF,
  SYS_PLMN_LIST_PARTIAL,

  /** @cond
  */
  SYS_PLMN_LIST_MAX              /* FOR INTERNAL USE ONLY */
  /** @endcond
  */

} sys_plmn_list_status_e_type;
/*~ SENTINEL sys_plmn_list_status_e_type.SYS_PLMN_LIST_MAX */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/*------------------------------------------------------------------------------
   Network Lists Data Types
------------------------------------------------------------------------------*/

/** PLMN's signal strength, in units that allow direct comparison of
    the signal strength between GSM and WCDMA cells.
*/
typedef int32 sys_signal_strength_type;

/** @brief PLMN information.
*/
typedef struct sys_detailed_plmn_list_info_s
{

  sys_plmn_id_s_type                        plmn;
    /**< PLMN ID. */

  sys_radio_access_tech_e_type              rat;
    /**< Radio Access Technology of the PLMN. */

  sys_srv_capability_e_type                 plmn_service_capability;
    /**< Type of service domain the PLMN can provide. */

  boolean                                   plmn_forbidden;
    /**< Whether the PLMN is forbidden. */

  sys_detailed_plmn_list_category_e_type    list_category;
    /**< Type of PLMN. */

  sys_signal_quality_e_type                 signal_quality;
    /**< Signal quality of the PLMN. */

  sys_signal_strength_type                  signal_strength;
    /**< Signal strength of the PLMN. */

  sys_csg_info_s_type                       csg_info;
  /**< CSG information. */

  uint32				    earfcn;			  
    /**< LTE EARFCN. */

} sys_detailed_plmn_list_info_s_type;

/** @brief Detailed PLMN list.
*/
typedef struct sys_detailed_plmn_list_s
{

  uint32                                    length;
    /**< The number of PLMNs in the list. */

  sys_detailed_plmn_list_info_s_type        info[SYS_PLMN_LIST_MAX_LENGTH];
    /**< PLMN information. */

} sys_detailed_plmn_list_s_type;

/*~ FIELD sys_detailed_plmn_list_s.info VARRAY SYS_PLMN_LIST_MAX_LENGTH
    LENGTH sys_detailed_plmn_list_s.length */

/** @brief Information regarding the user-preferred/controlled network. The
    access_tech_mask is a bitmask indicated by the sys_access_tech_e_type
    enumerates.
*/
typedef struct sys_user_pref_plmn_list_info_s
{
  sys_plmn_id_s_type                        plmn;
    /**< PLMN ID. */
  sys_access_tech_e_type                    access_tech;
    /**< Radio access technology of the PLMN. */

} sys_user_pref_plmn_list_info_s_type;

/** @brief List of user-preferred/controlled networks.
*/
typedef struct sys_user_pref_plmn_list_s
{

  byte                                      length;
    /**< Number of PLMNs in the list. */

  sys_user_pref_plmn_list_info_s_type       info[SYS_USER_PPLMN_LIST_MAX_LENGTH];
    /**< User preferred/controlled network. */

} sys_user_pref_plmn_list_s_type;
/*~ FIELD sys_user_pref_plmn_list_s.info VARRAY SYS_USER_PPLMN_LIST_MAX_LENGTH
    LENGTH sys_user_pref_plmn_list_s.length */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

#define sys_plmn_rat_mask_s_type sys_user_pref_plmn_list_s_type
#define WPA_RSN_MAX_PAIRWISE_CIPHER_SUITE  5
#define WPA_RSN_MAX_AKM_SUITE              2

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** WLAN security type.
*/
typedef enum
{
  SYS_TECH_WLAN_SEC_NONE = 0,    /**< &nbsp; */
  SYS_TECH_WLAN_SEC_WEP  = 1,    /**< &nbsp; */
  SYS_TECH_WLAN_SEC_WPA  = 2,    /**< &nbsp; */
  SYS_TECH_WLAN_SEC_WPA2 = 3     /**< &nbsp; */
} sys_wlan_security_e_type;

/** Cipher suite type.
*/
typedef enum
{
  SYS_WLAN_SEC_CIPHER_SUITE_WEP_40  = 1,            /**< &nbsp; */
  SYS_WLAN_SEC_CIPHER_SUITE_TKIP    = 2,            /**< &nbsp; */
  SYS_WLAN_SEC_CIPHER_SUITE_RSVD    = 3,            /**< &nbsp; */
  SYS_WLAN_SEC_CIPHER_SUITE_CCMP    = 4,            /**< &nbsp; */
  SYS_WLAN_SEC_CIPHER_SUITE_WEP_104 = 5,            /**< &nbsp; */
  SYS_WLAN_SEC_CIPHER_SUITE_VENDOR_SPECIFIC = 6,    /**< &nbsp; */
  SYS_WLAN_SEC_CIPHER_SUITE_INVALID = 7             /**< &nbsp; */
} sys_wlan_cipher_suite_e_type;

/** Association and Key Management (AKM) suite type.
*/
typedef enum
{
  SYS_WLAN_SEC_AKM_SUITE_RSVD    = 0,            /**< &nbsp; */
  SYS_WLAN_SEC_AKM_SUITE_8021X   = 1,            /**< &nbsp; */
  SYS_WLAN_SEC_AKM_SUITE_PSK     = 2,            /**< &nbsp; */
  SYS_WLAN_SEC_AKM_SUITE_VENDOR_SPECIFIC = 3,    /**< &nbsp; */
  SYS_WLAN_SEC_AKM_SUITE_INVALID = 4             /**< &nbsp; */
} sys_wlan_akm_suite_e_type;

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** @brief WLAN WPA security information.
*/
typedef struct sys_wlan_security_wpa_rsn_info_type
{

  sys_wlan_cipher_suite_e_type  grp_cipher_suite;
    /**< Cipher used for WLAN security. */

  uint16                        pairwise_cipher_count;
    /**< Total cipher pairs used. */

  sys_wlan_cipher_suite_e_type  pairwise_cipher_suites[WPA_RSN_MAX_PAIRWISE_CIPHER_SUITE];
    /**< List of cipher suites used in security. */

  uint16                        akm_count;
    /**< Number of AKM suites. */

  sys_wlan_akm_suite_e_type     akm_suites[WPA_RSN_MAX_AKM_SUITE];
    /**< Array of AKM suite.s */

} sys_wlan_security_wpa_rsn_info_s_type;

/** @brief WLAN security information.
*/
typedef struct sys_wlan_security_info_s
{

  sys_wlan_security_e_type                 sec_type;
    /**< Type of WLAN security. */

  sys_wlan_security_wpa_rsn_info_s_type    wpa_rsn_info;
    /**< WLAN WPA security information. */

} sys_wlan_security_info_s_type;

/** @brief BSS information in the WLAN.
*/
typedef struct sys_wlan_bss_info_s
{

  sys_chan_type                             chan;
    /**< Channel on which this BSS provides service. */

  sys_band_class_e_type                     band;
    /**< Band in which this BSS provides service. */

  sys_wlan_bss_e_type                       bss_type;
    /**< Type of BSS: infrastructure or independent. */

  sys_wlan_bssid_type                       bssid;
    /**< BSS ID. */

  sys_wlan_ssid_s_type                      ssid;
    /**< SSID. */

  uint16                                    rssi;
    /**< Measured RSSI of the BSS. */

  sys_tech_e_type                           tech;
    /**< WLAN technology in use. */

  sys_wlan_security_info_s_type             security_info;
    /**< WLAN security in use. */

} sys_wlan_bss_info_s_type;

/** @brief BSS network list.
*/
typedef struct sys_wlan_bss_info_list_s
{

    int                            num_bss;
    /**< Number of BSS in this list.  */

    sys_wlan_bss_info_s_type       bss_info[SYS_NETWORK_LIST_INFO_MAX];
    /**< BSS list. */

}sys_wlan_bss_info_list_s_type;
/*~ FIELD sys_wlan_bss_info_list_s.bss_info VARRAY SYS_NETWORK_LIST_INFO_MAX
    LENGTH sys_wlan_bss_info_list_s.num_bss */


#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/*------------------------------------------------------------------------------
   Mobility Management Info Defines
------------------------------------------------------------------------------*/
/*
Define constants used for mobility management information.
According to the standard, 3GPP TS 24.0008, the short name and
full name can be from 3 to N characters in length.
*/

/** @name Mobility Management Information Defines
    @{ */

#define SYS_MAX_SHORT_NAME_CHARS      255
#define SYS_MIN_SHORT_NAME_CHARS      1
#define SYS_MAX_FULL_NAME_CHARS       255
#define SYS_MIN_FULL_NAME_CHARS       1

/** @} */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** Network name coding scheme; the format is
    specified by 3GPP TS 24.008 and 03.38.
*/
typedef enum
{
  /** @cond
  */
  SYS_NW_NAME_CODING_SCHEME_NONE                             = -1,
    /* FOR INTERNAL USE ONLY */
  /** @endcond
  */

  SYS_NW_NAME_CODING_SCHEME_CELL_BROADCAST_DEFAULT_ALPHABET  =  0,
    /**< Cell broadcast data coding, default alphabet, language unspecified. */

  SYS_NW_NAME_CODING_SCHEME_UCS2                             =  1,
    /**< UCS2 coding scheme. */

  SYS_NW_NAME_CODING_SCHEME_RESERVED                         =  2,
    /**< Reserved. */

  /** @cond
  */
  SYS_NW_NAME_CODING_SCHEME_MAX
    /* FOR INTERNAL USE ONLY */
  /** @endcond
  */

} sys_network_name_coding_scheme_type_e_type;
/*~ SENTINEL sys_network_name_coding_scheme_type_e_type.SYS_NW_NAME_CODING_SCHEME_MAX */

/**  Local Service Area (LSA) identity; the format is specified by
     3GPP TS 24.008 and 23.003.
*/
typedef enum
{
  /** @cond
  */
  SYS_LSA_ID_TYPE_NONE         = -1,     /*   FOR INTERNAL USE ONLY! */
  /** @endcond
  */

  SYS_LSA_ID_TYPE_PLMN_SIG_NUM =  0,
    /**< The LSA is a PLMN significant number. */

  SYS_LSA_ID_TYPE_UNIVERSAL    =  1,
    /**< The LSA is a universal LSA. */

  /** @cond
  */
  SYS_LSA_ID_TYPE_MAX                    /*   FOR INTERNAL USE ONLY! */
  /** @endcond
  */

} sys_lsa_identity_type_e_type;
/*~ SENTINEL sys_lsa_identity_type_e_type.SYS_LSA_ID_TYPE_MAX */

/** Daylight savings adjustment; the values for the enumerated constants
    are dictated by 3GPP TS 24.008.
**/
typedef enum
{
  /** @cond
  */
  SYS_DAY_SAV_ADJ_NONE          = -1,            /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */

  SYS_DAY_SAV_ADJ_NO_ADJUSTMENT =  0,  /**< &nbsp; */
  SYS_DAY_SAV_ADJ_PLUS_ONE_HOUR =  1,  /**< &nbsp; */
  SYS_DAY_SAV_ADJ_PLUS_TWO_HOUR =  2,  /**< &nbsp; */

  /** @cond
  */
  SYS_DAY_SAV_ADJ_MAX                            /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */
} sys_daylight_saving_adj_e_type;
/*~ SENTINEL sys_daylight_saving_adj_e_type.SYS_DAY_SAV_ADJ_MAX */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/*------------------------------------------------------------------------------
   Mobility Management Info Data Types
------------------------------------------------------------------------------*/

/** @brief Network's short name and full name.
*/
typedef struct sys_network_short_name_s
{
  uint8                                       length;
    /**< Number of characters in the short name. */

  sys_network_name_coding_scheme_type_e_type  coding_scheme;
    /**< Coding scheme used to encode the short name of the network. */

  boolean                                     add_country_initials;
    /**< Flag that indicates whether the MS should add the country initials to
         the short name of the network. */

  uint8                                       name[SYS_MAX_SHORT_NAME_CHARS];
    /**< Short name of the network. */

  byte                                         spare_bits;
    /**< Number of spare bits in the end octet of the network short name. */

} sys_network_short_name_s_type;

/*~ FIELD sys_network_short_name_s.name VARRAY SYS_MAX_SHORT_NAME_CHARS
    LENGTH sys_network_short_name_s.length */

/** @brief Characteristics of the name of the given network.
*/
typedef struct sys_network_full_name_s
{
  uint8                                        length;
    /**< Number of characters in the full name. */

  sys_network_name_coding_scheme_type_e_type   coding_scheme;
    /**< The coding scheme used to encode the full name of the network. */

  boolean                                      add_country_initials;
    /**< Flag that indicates whether MS should add the country initials to
         the full name of the network. */

  uint8                                        name[SYS_MAX_FULL_NAME_CHARS];
    /**< The full name of the network.          */

  byte                                         spare_bits;
    /**< Number of spare bits in the end octet of the network full name. */

} sys_network_full_name_s_type;
/*~ FIELD sys_network_full_name_s.name VARRAY SYS_MAX_FULL_NAME_CHARS
    LENGTH sys_network_full_name_s.length */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/* No information on LTM is present
** in 3.7.2.3.2.26 Sync Channel Message
** or HDR sector parameter message
** Could have also be named as NO_INFO. */

#define SYS_LTM_OFFSET_INVALID     0xFF /**< Land-to-Mobile information missing. */

/* No information on Leap seconds is available
** at present from Sync Channel Message
** or sector parameter message */

#define SYS_LEAP_SECONDS_NO_INFO   0xFF  /**< No information on leap seconds is
                                              available. */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** Daylight savings information. 3.7.2.3.2.26 synch channel message only. This
    is not present in the HDR sector parameter message.
*/
typedef enum
{
  /** @cond
  */
  SYS_DAYLT_SAVINGS_NONE    = -1,
     /* INTERNAL USE */
  /** @endcond
  */

  SYS_DAYLT_SAVINGS_OFF = 0,
    /**< Daylight savings is not in effect. */

  SYS_DAYLT_SAVINGS_ON = 1,
    /**< Daylight savings is in effect. */

  /** @cond
  */
  SYS_DAYLT_SAVINGS_MAX
     /* INTERNAL USE */
  /** @endcond
  */

} sys_daylt_savings_e_type;
/*~ SENTINEL sys_daylt_savings_e_type.SYS_DAYLT_SAVINGS_MAX */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

#define   SYS_DAYLT_SAVINGS_NO_INFO SYS_DAYLT_SAVINGS_OFF
#define   SYS_DAYLT_SAVINGS_INVALID SYS_DAYLT_SAVINGS_OFF

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** @brief CDMA time-related information obtained from the synch
    channel message.
*/
typedef struct sys_cdma_time_s {

  byte                     lp_sec;
    /**< Number of leap seconds that have occurred since
         the start of system time. */

  byte                     ltm_offset;
    /**< Offset of local time from system time (this is a twos-complement
         value). */

  sys_daylt_savings_e_type daylt_savings;
    /**< Daylight savings indicator. */

} sys_cdma_time_s_type;

/** @brief HDR system time obtained from the sector parameter
    message.
*/
typedef struct sys_hdr_time_s
{
  byte lp_sec;
    /**< Number of leap seconds that have occurred since
         the start of system time. */

  int16 ltm_offset;
    /**< Offset of local time from system time. */

}sys_hdr_time_s_type;

/** @brief CDMA and HDR time-related information.
*/
typedef union sys_time_info_u {

  sys_cdma_time_s_type    cdma_time;
    /**< Time obtained from the CDMA synch channel message. */

  sys_hdr_time_s_type     hdr_time;
    /**< Time obtained from the HDR sector parameter message. */

} sys_time_info_u_type;
/*~ IF (_DISC_ == SYS_SYS_MODE_CDMA ) sys_time_info_u.cdma_time */
/*~ IF (_DISC_ == SYS_SYS_MODE_HDR ) sys_time_info_u.hdr_time */
/*~ DEFAULT sys_time_info_u.void */

/** @brief CDMA/HDR time-related information. The sys_mode parameter acts as a
    discriminator to determine which time information is valid. If sys_mode is
    SYS_SYS_MODE_NO_SRV, the time information is invalid.
*/
typedef struct sys_time_info_s {

  sys_sys_mode_e_type    sys_mode;
    /**< Mode (CDMA or HDR ) which last updated the time. */

  sys_time_info_u_type    time;
    /**< Union of CDMA and HDR time information structures. These are not the
         same, since HDR does not provide daylight savings information. */

} sys_time_info_s_type;
/*~ FIELD sys_time_info_s.time DISC sys_time_info_s.sys_mode */

/** Time zone, as an offset from universal time (i.e., the difference
    between local time and universal time), in increments of 15 minutes.
    The format for this type is specified in 3G TS 24.008 and 23.040.
*/
typedef int sys_time_zone_type;

/** @brief Time zone and universal time mobility management information.
    Each field is received as binary coded digits as specified in
    3GPP TS 24.008 and 23.040.
*/
typedef struct sys_time_and_time_zone_s
{
  uint8                                     year;       /**< Year field. */
  uint8                                     month;      /**< Month field. */
  uint8                                     day;        /**< Day field. */
  uint8                                     hour;       /**< Hour field. */
  uint8                                     minute;     /**< Minute field. */
  uint8                                     second;     /**< Second field. */
  sys_time_zone_type                        time_zone;  /**< Timezone field. */
} sys_time_and_time_zone_s_type;

/** @brief Local Service Area identity. The format
    is specified by 3GPP TS 24.008 and 23.003.
*/
typedef struct sys_lsa_identity_s
{
  sys_lsa_identity_type_e_type              type;
    /**< Localized Service Area identity. */
  uint8                                     identity[3];
    /**< Value of the LSA ID. */
} sys_lsa_identity_s_type;

/** @brief Currently registered network's mobility management information (i.e.,
    the network's name, date, time, and time zone).

    All fields in this structure are optional. If the "available" flag is
    TRUE, the corresponding data field contains valid information.
    If the "available" flag is FALSE, the corresponding data field
    should be ignored.
*/
typedef struct sys_mm_information_s
{
  /*
  Flags that indicate whether the particular mm information is
  available.
  */
  boolean                                   plmn_avail;
    /**< Whether the PLMN is available. */
  boolean                                   full_name_avail;
    /**< Whether the network full name is available. */
  boolean                                   short_name_avail;
    /**< Whether the network short name is available. */
  boolean                                   univ_time_and_time_zone_avail;
    /**< Whether the universal time is available. */
  boolean                                   time_zone_avail;
    /**< Whether the timezone is available. */
  boolean                                   lsa_identity_avail;
    /**< Whether the LSA ID is available. */
  boolean                                   daylight_saving_adj_avail;
    /**< Whether daylight saving information is available. */
  boolean                                   lac_avail;
   /**< Whether location area code (LAC) information is available. */


  sys_plmn_id_s_type                        plmn;
    /**< PLMN information. */
  sys_network_full_name_s_type              full_name;
    /**< Network full name. */
  sys_network_short_name_s_type             short_name;
    /**< Network short name. */
  sys_time_and_time_zone_s_type             univ_time_and_time_zone;
    /**< Universal Time Coordinated (UTC) time zone information. */
  sys_time_zone_type                        time_zone;
    /**< Current time zone information. */
  sys_lsa_identity_s_type                   lsa_identity;
    /**< LSA ID. */
  sys_daylight_saving_adj_e_type            daylight_saving_adj;
    /**< Daylight saving adjustment. */
  sys_lac_type                              lac_id;
    /**< LAC id. */
  sys_radio_access_tech_e_type              rat;
    /**< RAT associated with mm info */
} sys_mm_information_s_type;
#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** M511 table values.
*/
typedef enum
{
  /** @cond
  */
  SYS_JCDMA_M511_MIN = -1,   /*   used only for bounds checking           */
  /** @endcond
  */

  SYS_JCDMA_M511_PKT,
    /**< User-selected packet from the M511 menu. */

  SYS_JCDMA_M511_ASYNC_FAX,
    /**< User-selected async/fax from the M511 menu. */

  /** @cond
  */
  SYS_JCDMA_M511_MAX         /*   Used for arrays & bounds checking       */
  /** @endcond
  */

} sys_jcdma_m511_e_type;
/*~ SENTINEL sys_jcdma_m511_e_type.SYS_JCDMA_M511_MAX */

/** M512 table values.
*/
typedef enum
{
  /** @cond
  */
  SYS_JCDMA_M512_MIN = -1,   /*   Used only for bounds checking          */
  /** @endcond
  */

  SYS_JCDMA_M512_STD,
    /**< User-selected standard data rate. */

  SYS_JCDMA_M512_HIGH,
    /**< User-selected high data rate. */

  /** @cond
  */
  SYS_JCDMA_M512_MAX         /*   Used for arrays & bounds checking      */
  /** @endcond
  */

} sys_jcdma_m512_e_type;
/*~ SENTINEL sys_jcdma_m512_e_type.SYS_JCDMA_M512_MAX */

/** M513 table values.
*/
typedef enum
{
  /** @cond
  */
  SYS_JCDMA_M513_MIN = -1,   /*   Used only for bounds checking        */
  /** @endcond
  */

  SYS_JCDMA_M513_AUTO,       /**< AUTO setting. */
  SYS_JCDMA_M513_19200,      /**< 19200 bps. */
  SYS_JCDMA_M513_115200,     /**< 115200 bps. */
  SYS_JCDMA_M513_230400,     /**< 230400 bps. */

  /** @cond
  */
  SYS_JCDMA_M513_MAX         /*   Used for arrays & bounds checking    */
  /** @endcond
  */

} sys_jcdma_m513_e_type;
/*~ SENTINEL sys_jcdma_m513_e_type.SYS_JCDMA_M513_MAX */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** Maximum size (in bytes) of the IS2000 message.
*/
#define    SYS_MAX_IS2000_MSG_SIZE     (256)

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/**
  UMTS-to-CDMA handover status causes.
*/
typedef enum
{
  SYS_UMTS_CDMA_HANDOVER_NONE,              /**< &nbsp; */
  SYS_UMTS_CDMA_HANDOVER_SUCCESS,           /**< &nbsp; */
  SYS_UMTS_CDMA_HANDOVER_ERR_NO_SRV,        /**< &nbsp; */
  SYS_UMTS_CDMA_HANDOVER_ERR_ACC_IN_PROG,   /**< &nbsp; */
  SYS_UMTS_CDMA_HANDOVER_ERR_IN_TC,         /**< &nbsp; */
  SYS_UMTS_CDMA_HANDOVER_ERR_UHDM_PARSE,    /**< &nbsp; */
  SYS_UMTS_CDMA_HANDOVER_ERR_CDMA_LOCK,     /**< &nbsp; */
  SYS_UMTS_CDMA_HANDOVER_ERR_OTHERS,        /**< &nbsp; */
  SYS_UMTS_CDMA_HANDOVER_ERR_BAD_MSG_TYPE,  /**< &nbsp; */

  /** @cond
  */
  SYS_UMTS_CDMA_HANDOVER_MAX
  /** @endcond
  */

}sys_umts_cdma_handover_e_type;
/*~ SENTINEL sys_umts_cdma_handover_e_type.SYS_UMTS_CDMA_HANDOVER_MAX */

/** UMTS-to-CDMA handover error statuses.
*/
typedef enum
{
  SYS_HO_STATUS_NONE,                       /**< &nbsp; */
  SYS_HDR_CDMA_HO_SUCCESS,                  /**< &nbsp; */
  SYS_HDR_CDMA_HO_ERR_NO_SRV,               /**< &nbsp; */
  SYS_HDR_CDMA_HO_ERR_ACC_IN_PROG,          /**< &nbsp; */
  SYS_HDR_CDMA_HO_ERR_IN_TC,                /**< &nbsp; */
  SYS_HDR_CDMA_HO_ERR_UHDM_PARSE,           /**< &nbsp; */
  SYS_HDR_CDMA_HO_ERR_CDMA_LOCK,            /**< &nbsp; */
  SYS_HDR_CDMA_HO_ERR_OTHERS,               /**< &nbsp; */

  /** @cond
  */
  SYS_HO_STATUS_MAX
  /** @endcond
  */

} sys_ho_status_e_type;
/*~ SENTINEL sys_ho_status_e_type.SYS_HO_STATUS_MAX */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** @brief IS2000 message.
*/
typedef struct {

  uint8                                type;
    /**< Message type. */

  uint32                               payload_len;
    /**< Message payload length in bits; maximum size
         is SYS_MAX_IS2000_MSG_SIZE * 8. */

  byte                                 payload[SYS_MAX_IS2000_MSG_SIZE];
    /**< Message payload. */

} sys_is2000_msg_s_type;

/*~ FIELD sys_is2000_msg_s_type.payload VARRAY SYS_MAX_IS2000_MSG_SIZE
    LENGTH ( sys_is2000_msg_s_type.payload_len / 8 ) */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/*===============================================================================

  BCMCS - FEATURE_BCMCS

  BCMCS stands for "High rate BroadCast-MultiCast packet data air interface
  Specification"


==============================================================================*/

/*------------------------------------------------------------------------------
   BCMCS Defines
------------------------------------------------------------------------------*/
/** Maximum number of data flows supported. Remote functions should always use
    this value.
 */
#define SYS_BCMCS_MAX_FLOWS     40

/** Maximum number of monitored data flows.
*/
#define SYS_BCMCS_MAX_MON_FLOWS 10

/** Maximum number of data flows supported for BCMCS_2.0.
*/
#define SYS_BCMCS_20_MAX_FLOWS     40

/** Maximum number of monitored data flows for BCMCS_2.0.
*/
#define SYS_BCMCS_20_MAX_MON_FLOWS 10

/** Maximum number of data flows supported for BCMCS_1.0.
*/
#define SYS_BCMCS_10_MAX_FLOWS     10

/** Maximum number of data flows supported for non-BCMCS.
*/
#define SYS_BCMCS_MIN_FLOWS     1
/** Maximum number of monitored data flows for non-BCMCS.
*/
#define SYS_BCMCS_MIN_MON_FLOWS 1

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/*-------------------------------------------------------------------------------
   BCMCS Enums
------------------------------------------------------------------------------*/

/** Broadcast/Multicast Specification (BCMCS) data service availability.
*/
typedef enum
{
  /** @cond
  */
  SYS_BCMCS_SRV_STATUS_NONE = -1,
    /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */

  SYS_BCMCS_SRV_STATUS_NO_SRV,   /**< No BCMCS service is available.  */
  SYS_BCMCS_SRV_STATUS_SRV,      /**< BCMCS service is available.  */

  /** @cond
  */
  SYS_BCMCS_SRV_STATUS_MAX
    /*   FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

} sys_bcmcs_srv_status_e_type;
/*~ SENTINEL sys_bcmcs_srv_status_e_type.SYS_BCMCS_SRV_STATUS_MAX */

/** BCMCS flow status values.
*/
typedef enum
{
  /** @cond
  */
  SYS_BCMCS_FLOW_STATUS_MIN = -1,
    /* For bounds checking only */
  /** @endcond
  */

  SYS_BCMCS_FLOW_STATUS_NONE = 0,
    /**< Used for initialization or to indicate that there is no flow status
       information sent with the event or report. */

  /*---------------------------------------------------------------
    No-op
  ---------------------------------------------------------------*/
  SYS_BCMCS_FLOW_STATUS_CANCELLED = 100,
    /**< A new flow request was received before the previous one is done
         processing. Cancel the remaining lookup and start processing
         the new request. */

  /*---------------------------------------------------------------
    Success - Flow is monitored  (DESIRED_MONITORED)
  ---------------------------------------------------------------*/
  SYS_BCMCS_FLOW_STATUS_MONITORED = 200,
    /**< Flow is active and included in the broadcast overhead message. */

  /*---------------------------------------------------------------
    Successful request and db lookup, but the flow cannot be
    monitored yet.

    Considered a temporary Failure - start a deregistration timer.
  ---------------------------------------------------------------*/
  /*-------------------------------------
    DESIRED_UNMONITORED
  -------------------------------------*/
  SYS_BCMCS_FLOW_STATUS_UNABLE_TO_MONITOR = 300,
    /**< Flow is available in overhead but cannot be monitored
         because its channel assignment conflicts with other flows. */

  /*-------------------------------------
    DESIRED_UNAVAILABLE
  -------------------------------------*/
  SYS_BCMCS_FLOW_STATUS_REQUESTED,
    /**< Registered but not included in broadcast overhead message. */

  SYS_BCMCS_FLOW_STATUS_TIMEOUT,
    /**< Broadcast supervision timeout. The Access Terminal (AT) is on a
       system where broadcast is available. The AT waited for three times
     BroadcastOverheadPeriod, and the flow did not appear in the BC OVHD
     message. */

  SYS_BCMCS_FLOW_STATUS_LOST,
    /**< The AT lost acquisition and temporarily disabled its monitored
       flows. It is as if the flows were not in the BC OVHD message, as
     with SYS_BCMCS_FLOW_STATUS_REQUESTED. Another flow status will be
     sent when the AT re-acquires. */

  SYS_BCMCS_FLOW_STATUS_SYS_UNAVAILABLE,
    /**< The AT cannot process the flow request because BCMCS is in the
         CLOSED state (HDR not acquired). */

  SYS_BCMCS_FLOW_STATUS_REACHED_MAX_MON_FLOWS,
    /**< The flow is available in OVHD, but the AT cannot monitor it
         because it has reached the maximum number of flows it can
         concurrently monitor. */

  /*---------------------------------------------------------------
    Failure (permanent) - flow should be deregistered.
  ---------------------------------------------------------------*/
  /*-------------------------------------
    AN is explicitely rejecting the flow
  -------------------------------------*/
  SYS_BCMCS_FLOW_STATUS_REJECTED_NOT_AVAILABLE = 400,
    /**< Flow was rejected explicitly by the network; the BCMCS flow ID
         and/or BCMCS program ID are not available. */

  SYS_BCMCS_FLOW_STATUS_REJECTED_NOT_TRANSMITTED,
    /**< Flow was rejected explicitly by the network; the BCMCS flow ID
         and/or BCMCS program ID were not transmitted. */

  SYS_BCMCS_FLOW_STATUS_REJECTED_INVALID_AUTH_SIG,
    /**< Flow was rejected explicitly by the network; invalid
         authorization signature. */

  /*-------------------------------------
    DB Lookup Failure
  -------------------------------------*/
  SYS_BCMCS_FLOW_STATUS_DB_UNAVAILABLE = 500,
    /**< A lookup in the XML file could not be completed because the database
         is in an invalid state. */

  SYS_BCMCS_FLOW_STATUS_NO_MAPPING,
    /**< The XML file does not contain mapping for the current subnet. */

  SYS_BCMCS_FLOW_STATUS_ID_NOT_FOUND_FOR_GIVEN_MULTICAST_IP,
    /**< A lookup in the XML file cannot find a flow ID for
         the given multicast IP in the current subnet. */

  /** @cond
  */
  SYS_BCMCS_FLOW_STATUS_MAX
    /* For bounds checking only */
  /** @endcond
  */

} sys_bcmcs_flow_status_e_type;
/*~ SENTINEL sys_bcmcs_flow_status_e_type.SYS_BCMCS_FLOW_STATUS_MAX */

/** BCMCS flow status values.
*/
typedef enum
{
  /** @cond
  */
  SYS_BCMCS_REG_STATUS_MIN = -1,  // For bounds checking only
  /** @endcond
  */

  SYS_BCMCS_REG_STATUS_NONE = 0,
    /**< Used for initialization or to indicate that there is no
         registration status information sent with the event or report. */

  SYS_BCMCS_REG_STATUS_SUCCESS,
    /**< FlowRegistration message was sent successfully for this flow. */

  SYS_BCMCS_REG_STATUS_NOT_ALLOWED,
    /**< Could not send a FlowRegistration message; not allowed. */

  SYS_BCMCS_REG_STATUS_GEN_FAILURE,
    /**< Could not send a FlowRegistration message; generic failure. */

  /** @cond
  */
  SYS_BCMCS_REG_STATUS_MAX  // For bounds checking only
  /** @endcond
  */

} sys_bcmcs_reg_status_e_type;
/*~ SENTINEL sys_bcmcs_reg_status_e_type.SYS_BCMCS_REG_STATUS_MAX */

/** Flow service types.
*/
typedef enum
{
  /** @cond
  */
  SYS_FLOW_SERVICE_NONE = -1,    /* For bounds checking only */
  /** @endcond
  */

  SYS_FLOW_SERVICE_BCMCS = 0,
    /**< Flow service type is BCMCS. */

  SYS_FLOW_SERVICE_MBMS = 1,
    /**< Flow service type is MBMS. */

  /** @cond
  */
  SYS_FLOW_SERVICE_MAX    /* For bounds checking only */
  /** @endcond
  */

} sys_flow_service_e_type;
/*~ SENTINEL sys_flow_service_e_type.SYS_FLOW_SERVICE_MAX */


/*------------------------------------------------------------------------------
   BCMCS Data Types
------------------------------------------------------------------------------*/

/** The supported IP address types; used to discriminate the union below
    (same as ip_addr_type from ps_ip_addr.h).
*/
typedef enum
{
  SYS_IP_ANY_ADDR     = 0,   /**< &nbsp; */
  SYS_IPV4_ADDR       = 4,   /**< &nbsp; */
  SYS_IPV6_ADDR       = 6,   /**< &nbsp; */
  SYS_IP_ADDR_INVALID = 255  /**< &nbsp; */

} sys_ip_addr_enum_type;

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** @brief IP address.
*/
typedef struct sys_ip_address_s
{
  sys_ip_addr_enum_type type;   /**< IPv4 or IPv6. */

  union {
    uint32 v4;    /**< Stores the address for IPv4. */
    uint64 v6[2]; /**< Stores the address for IPv6. */
  } addr;

} sys_ip_address_s_type;


/*~ IF ( (_DISC_ == SYS_IP_ANY_ADDR) ||
         (_DISC_ == SYS_IPV6_ADDR) ||
         (_DISC_ == SYS_IP_ADDR_INVALID) )
         sys_ip_address_s.addr.v6 */
/*~ IF ( _DISC_ == SYS_IPV4_ADDR )
         sys_ip_address_s.addr.v4 */
/*~ FIELD sys_ip_address_s.addr DISC sys_ip_address_s.type */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/*---------------------------------------------------------------------------
                                MBMS
----------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
   MBMS (Multimedia broadcast multicast service) MACRO definitions.
   MBMS is specific to WCDMA.
--------------------------------------------------------------------------*/

/** Maximum services allowed per request.
*/
#define SYS_MBMS_NUM_MAX_SRVS_PER_REQ 2

/* TMGI is a parameter used in
** MBMS service act req. Gives
** the max length of TMGI.
*/

/** Maximum length of the Temporary Mobile Group ID.
*/
#define SYS_MAX_MBMS_TMGI_LENGTH 15

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/*---------------------------------------------------------------------------
** MBMS enum definitions
---------------------------------------------------------------------------*/
/** Multimedia Broadcast/Multicast (MBMS) service status.
*/
typedef enum
{
  /** @cond
  */
  SYS_MBMS_SRV_STATUS_NONE = -1,
    /* For internal use only */
  /** @endcond
  */

  SYS_MBMS_SRV_STATUS_UNAVAILABLE_NETWORK_FAILURE,
    /**< MBMS service in unavailable due to a network failure. */

  SYS_MBMS_SRV_STATUS_UNAVAILABLE_SETUP_FAILURE,
    /**< MBMS service in unavailable due to a setup failure on the UE side. */

  SYS_MBMS_SRV_STATUS_UNAVAILABLE_FAILURE_OTHER,
    /**< MBMS service in unavailable due to another failure. */

  SYS_MBMS_SRV_STATUS_AVAILABLE,
    /**< MBMS service is available. */

  SYS_MBMS_SRV_STATUS_MONITORED,
    /**< MBMS service is monitored. */

  SYS_MBMS_SRV_STATUS_OTHER,
    /**< Reserved for future use. */

  SYS_MBMS_SRV_STATUS_RESERVED1,
    /**< Reserved for future use. */

  /** @cond
  */
  SYS_MBMS_SRV_STATUS_MAX  /* For internal use only */
  /** @endcond
  */

} sys_mbms_srv_status_e_type;
/*~ SENTINEL sys_mbms_srv_status_e_type.SYS_MBMS_SRV_STATUS_MAX */

/* To be discarded for phase 2a */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** @brief Kept for backwards compatibility. This structure has been superseded
    by sys_mbms_tmgi_uint_s_type.
*/
typedef struct
{

  uint32  service_id;      /**< Service ID. */
  boolean plmn_id_present; /**< Indicates whether PLMN ID is present. */
  uint32  plmn_id;         /**< PLMN ID if plmn_id_present is TRUE. */

} sys_mbms_tmgi_uint_old_s_type;

/** @brief MBMS logical channel ID.
*/
typedef struct
{

  boolean present;
  /**< TRUE  --  Logical channel ID is present.\n
       FALSE --  Logical channel ID is not present. */

  uint32  log_chan_id;
  /**< Logical channel ID; used between Data Services (DS) and Radio Link
       Control (RLC) on a traffic plane.  */

}sys_mbms_log_chan_id_s_type;

/** @brief TMGI (Temporary Mobile Group Identity) structure in array format.
*/
typedef struct
{

  boolean present;
    /**< TRUE if TMGI is present; FALSE otherwise. */

  uint8   length;
    /**< Length of TMGI. */

  uint8   value[SYS_MAX_MBMS_TMGI_LENGTH];
    /**< Actual TMGI value. */

}sys_mbms_tmgi_s_type;

/** @brief MBMS service priority.
*/
typedef struct
{

  boolean present;
    /**< TRUE if priority is present; FALSE otherwise. */

  uint32   priority;
    /**< Priority of a MBMS service request. */

}sys_mbms_srv_priority_s_type;

/** @brief MBMS information.
*/
typedef struct
{

  sys_mbms_tmgi_s_type              tmgi;
    /**< TMGI required for MBMS service. */

  sys_mbms_tmgi_uint_old_s_type     tmgi_conv;
    /**< Converted TMGI used by the MBMS manager. */

  sys_mbms_srv_priority_s_type      srv_priority;
    /**< Priority requested for MBMS. */

  sys_mbms_srv_status_e_type        srv_status;
    /**< Gives confirmation of MBMS activate or deactivate request. */

  sys_mbms_log_chan_id_s_type       srv_log_chan_id;
    /**< Logical channel ID; reported from the MBMS manager to the CM. */

}sys_mbms_srv_info_s_type;

/** @brief Flow ID structure. This type overloads the BCMCS flow ID structure
    to hold MBMS-specific information. This is done to resume
    cm_bcmcs_cmd_flow_request().
*/
typedef struct
{

  sys_ip_address_s_type ip_address;
    /**< IP address for the flow (MBMS and BCMCS). */

  uint16                port_number;
    /**< Port address for the flow (MBMS and BCMCS). */

  sys_flow_service_e_type  flow_service;
    /**< Identifies the type of flow (e.g., MBMS, BCMCS). */

  sys_mbms_srv_info_s_type mbms_srv_info;
    /**< Information specific to an MBMS service request. */

} sys_bcmcs_flow_addr_s_type;

/** @brief Flow status element type.
*/
typedef struct
{

  sys_bcmcs_flow_addr_s_type   flow_address;
    /**< Address of flow depending on BCMCS or MBMS. */

  sys_bcmcs_flow_status_e_type flow_status;
    /**< Status of flow depending on BCMCS or MBMS. */

} sys_bcmcs_flow_status_element_s_type;

/** @brief Status of a flow and its registration status.
*/
typedef struct {

  sys_bcmcs_flow_addr_s_type   flow_address;
    /**< Address of the flow, depending on whether BCMCS or MBMS. */

  sys_bcmcs_reg_status_e_type  reg_status;
    /**< Status of the registration. */

} sys_bcmcs_reg_status_element_s_type;

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** Registration setup.
*/
typedef enum
{
  /** @cond
  */
  SYS_BCMCS_REG_SETUP_MIN = -1,  /* For bounds checking only */
  /** @endcond
  */

  SYS_BCMCS_REG_SETUP_ALLOWED,
  /**< Registration setup allowed. */

  SYS_BCMCS_REG_SETUP_NOT_ALLOWED,
  /**< Registration setup not allowed. */

  /** @cond
  */
  SYS_BCMCS_REG_SETUP_MAX   /* For bounds checking only */
  /** @endcond
  */

}sys_bcmcs_reg_setup_e_type;
/*~ SENTINEL sys_bcmcs_reg_setup_e_type.SYS_BCMCS_REG_SETUP_MAX */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/*===============================================================================

                     Manual selection types

==============================================================================*/

#define  SYS_MAX_MANUAL_CHANNELS        5

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** Type of GW manual selection.
*/
typedef enum
{
  /** @cond
  */
  /* FOR INTERNAL USE ONLY! */
  SYS_MANUAL_SEL_TYPE_NONE = -1,
  /** @endcond
  */

  SYS_MANUAL_SEL_TYPE_USER_SPECIFIED,
    /**< System ID should be defined in the manual selection. */

  SYS_MANUAL_SEL_TYPE_HPLMN,
    /**< Select only the HPLMN. */

  SYS_MANUAL_SEL_TYPE_RPLMN,
    /**< Search for Registered PLMN (RPLMN) only. */

  /** @cond
  */
  SYS_MANUAL_SEL_TYPE_MAX,  /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */

} sys_manual_sel_type_e_type;
/*~ SENTINEL sys_manual_sel_type_e_type.SYS_MANUAL_SEL_TYPE_MAX */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** @brief A manual system profile is stored in packed format in profile_buf.
    This must be unpacked with net_unpack_profile() to net_profile_s_type before
    use.
*/
typedef struct sys_manual_sys_profile_s{


  word                                   profile_size;
    /**< Size of the profile buffer. */

  byte                                   profile_buf[SYS_WLAN_PROFILE_MAX_SIZE];
    /**< Buffer to store the profile information. The valid length is
       indicated by profile_size. */

} sys_manual_sys_profile_s_type;

/*~ FIELD sys_manual_sys_profile_s.profile_buf
    VARRAY SYS_WLAN_PROFILE_MAX_SIZE LENGTH sys_manual_sys_profile_s.profile_size */

/** @brief Parameters that define the WLAN manual system.
*/
typedef struct sys_wlan_manual_sys_info_s
{

  sys_wlan_bssid_type                    bssid;
    /**< BSS ID for WLAN manual selection; can be 0. */

  sys_chan_type                          channel;
    /**< Channel to search for BSS ID/System ID. If channel = 0, use any
         channel, otherwise it specifies the absolute channel for WLAN. */

  sys_band_mask_type                     band;
    /**< Band to use for manual system selection. */

  sys_profile_id_type                    profile_id;
    /**< Profile ID to use. */

  sys_wlan_bss_e_type                    bss_type;
    /**< BSS type. */

  sys_manual_sys_profile_s_type          profile;
    /**< Profile information for the manual system. */

} sys_wlan_manual_sys_info_s_type;

/** @brief Parameters that define the GSM/WCDMA manual system.
*/
typedef struct sys_gw_manual_sys_info_s
{

  sys_manual_sel_type_e_type             sel_type;
    /**< Manual selection type. */

} sys_gw_manual_sys_info_s_type;

/** @brief Parameters that define the CDMA manual system.
*/
typedef struct sys_cdma_manual_sys_info_s
{

  int                                    num_chans;
    /**< Number of channels; if num_chans = 0, use any applicable channel. */

  sys_chan_type                          chans[SYS_MAX_MANUAL_CHANNELS];
    /**< Channels to search for BSS ID/System ID. */

} sys_cdma_manual_sys_info_s_type;

/** @brief Parameters that define the manual system.
*/
typedef struct sys_manual_sys_info_s
{

  sys_sys_id_s_type                    sys_id;
    /**< System ID for manual selection; can also be wildcards. */

  union
  {
    sys_wlan_manual_sys_info_s_type    wlan;
    /**< Additional parameters for WLAN manual system selection (applicable
           only if sys_id is WLAN). */

    sys_gw_manual_sys_info_s_type      gw;
    /**< Additional parameters for GSM/WCDMA manual system
           selection (applicable only if sys_id is PLMN). */

    sys_cdma_manual_sys_info_s_type    cdma;
    /**< Additional parameters for CDMA/HDR/AMPS manual system
           selection (applicable only if sys_id is IS95/IS856). */

  } prm;

} sys_manual_sys_info_s_type;

/*~ FIELD sys_manual_sys_info_s.prm DISC _OBJ_.sys_id.id_type */
/*~ CASE SYS_SYS_ID_TYPE_WLAN sys_manual_sys_info_s.prm.wlan */
/*~ CASE SYS_SYS_ID_TYPE_UMTS sys_manual_sys_info_s.prm.gw */
/*~ CASE SYS_SYS_ID_TYPE_IS95 sys_manual_sys_info_s.prm.cdma */
/*~ DEFAULT sys_manual_sys_info_s.prm.void */

/** @brief Dummy structure.
*/
typedef struct {

  int a;   /**< Dummy member. */

} sys_wlan_acq_fail_list_info_s_type;

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** Enumeration for Location Notification status.
*/
typedef enum
{
  /** @cond
  */
  SYS_HDR_LOCATION_NOTIFICATION_STATUS_NONE = -1, /* Internal CM use */
  /** @endcond
  */

  SYS_HDR_LOCATION_NOTIFICATION_STATUS_SUCCESSFUL,/**< LN was successful. */
  SYS_HDR_LOCATION_NOTIFICATION_STATUS_FAILED,    /**< LN failed. */

  /** @cond
  */
  SYS_HDR_LOCATION_NOTIFICATION_STATUS_MAX        /* Internal CM use */
  /** @endcond
  */

} sys_hdr_location_notification_status_e_type;
/*~ SENTINEL sys_hdr_location_notification_status_e_type.SYS_HDR_LOCATION_NOTIFICATION_STATUS_MAX */


/** Enumeration of no service cause. Indicates the cause when phone
    enters no service status. Carried in No Service indication.
*/
typedef enum
{
  /** @cond
  */
  SYS_NO_SVC_CAUSE_NONE = -1,  /* FOR INTERNAL USE ONLY */
  /** @endcond
  */

  SYS_NO_SVC_CAUSE_RLF,
    /**< Radio link failure cause. */

  SYS_NO_SVC_CAUSE_NORMAL,
    /**< Normal cause. */

  SYS_NO_SVC_CAUSE_IRAT_TO_LTE,
    /**< pseudo no service returned by NAS due to reselection to LTE from
         3GPP2 RAT. */

  SYS_NO_SVC_CAUSE_NO_RETRY,
  /**< GStack deactivated for IMS registration success case in SGLTE mode */

  SYS_NO_SVC_CAUSE_GW_NOT_AVAILABLE,
  /**< CS service not found during CSFB call orig */

  /** @cond
  */
  SYS_NO_SVC_CAUSE_MAX
    /* FOR INTERNAL USE ONLY */
  /** @endcond
  */
}sys_no_svc_cause_e_type;
/*~ SENTINEL sys_no_svc_cause_e_type.SYS_NO_SVC_CAUSE_MAX */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/*===============================================================================

                     Statistics Types

==============================================================================*/

/*=*************************************************************
********************  STATS structures *************************
***************************************************************/

/*=**** STATS structures for CDMA 1X *****/

/** @name Statistics CDMA 1X Bitmask Values
    @{ */

#define SYS_CDMA_INFO_MASK_NONE                0
#define SYS_CDMA_STATE_INFO_BIT_MASK         SYS_BM_32BIT(0)
#define SYS_CDMA_CHANNEL_MEAS_INFO_BIT_MASK    SYS_BM_32BIT(1)
#define SYS_CDMA_ACTIVE_SET_INFO_BIT_MASK      SYS_BM_32BIT(2)
#define SYS_CDMA_SERV_SYS_INFO_BIT_MASK        SYS_BM_32BIT(3)
#define SYS_CDMA_CHANNEL_CONFIG_INFO_BIT_MASK  SYS_BM_32BIT(4)
#define SYS_CDMA_CP_CALL_STAT_BIT_MASK         SYS_BM_32BIT(5)
#define SYS_CDMA_REG_STAT_BIT_MASK             SYS_BM_32BIT(6)
#define SYS_CDMA_INFO_MASK_ALL                 (SYS_BM_32BIT(7) - 1)
#define SYS_CDMA_MAX_ASET_PILOTS                   6

/** @} */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** State of CDMA protocol.
*/
typedef enum sys_cdma_prot_state_e
{
  SYS_CDMA_PROT_INIT    = 1,      /**< Initial state. */
  SYS_CDMA_PROT_IDLE    = 2,      /**< Idle state. */
  SYS_CDMA_PROT_ACCESS  = 3,      /**< Access state. */
  SYS_CDMA_PROT_TRAFFIC = 4,      /**< Traffic state. */
  SYS_CDMA_PROT_EXIT    = 5       /**< Exit state. */
} sys_cdma_prot_state_e_type;

/** Status of the CDMA slotted mode.
*/
typedef enum sys_cdma_slotted_mode_e
{
  SYS_CDMA_UNSLOTTED_MODE   = 0,    /**< Unslotted mode. */
  SYS_CDMA_SLOTTED_MODE     = 1     /**< Slotted mode. */
} sys_cdma_slotted_mode_e_type;

/** Paging Channel (PCH) rate for CDMA.
*/
typedef enum sys_cdma_pch_rate_e
{
  SYS_CDMA_PCH_FULL_RATE   = 0,     /**< Full rate. */
  SYS_CDMA_PCH_HALF_RATE   = 1      /**< Half rate. */
} sys_cdma_pch_rate_e_type;

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** @brief CDMA Mobile Identification Number (MIN).
*/
typedef struct sys_cdma_min_s
{
  dword                   min1;  /**< 24 bits. */
  dword                   min2;  /**< 10 bits. */
} sys_cdma_min_s_type;

/** @brief State information for CDMA network and protocol.
*/
typedef struct sys_cdma_state_info_s
{
 sys_cdma_prot_state_e_type    cdma_prot_state;
   /**< Protocol state. */
 uint8                         band_class;
   /**< Band class supported. */
 sys_cdma_slotted_mode_e_type  slotted_mode;
   /**< Slotted mode or not. */
 sys_cdma_pch_rate_e_type      pch_rate;
   /**< Paging Channel rate. */
 uint16                        active_so;
   /**< Active Service Option. */
 sys_cdma_min_s_type           min;
   /**< Mobile Identification Number of current state. */
 int8                          ms_pref_sci;
   /**< Mobile Station preferred SCI (0-7). */
 uint8                         qpch_status;
   /**< Quick Paging Channel (QPCH) ON (1) or OFF (0). */   /* 1X L1 */
 uint8                   num_active_pilots;
   /**< Number of active pilots in traffic. */
}sys_cdma_state_info_s_type;

/** @brief Channel measurement information.
*/
typedef struct sys_cdma_channel_meas_s
{
  int16           rx_pwr;           /**< Rx instantaneous power in dBm. */
  int16           tx_pwr;           /**< Tx instantaneous power in dBm.*/
  uint16          pch_loss_count;   /**< Paging Channel loss count. */
  uint16          tch_loss_count;   /**< Traffic Channel loss count. */
  uint16          idle_ho_count;    /**< Idle hand off count. */
  uint16          acc_req_count;    /**< Number of access channel requests. */
  uint16          acc_rsp_count;    /**< Number of access channel responses. */
  uint16          failed_acc_att_count;  /**< Unsuccessful access attempts. */
  uint16          t41_expiry_count; /**< T41 timer expiration count. */
}sys_cdma_channel_meas_s_type;

/** @brief CDMA active set type.
*/
typedef struct sys_cdma_active_set
{
  uint16 pn;      /**< Pseudo-Noise. */
  uint16 ecio;    /**< Ec/Io. */
}sys_cdma_active_set_type;

/** @brief CDMA active set information.
*/
typedef struct sys_cdma_active_set_info_s
{
  sys_cdma_active_set_type   pilot_info[SYS_CDMA_MAX_ASET_PILOTS];
    /**< CDMA active set type list. */

}sys_cdma_active_set_info_s_type;

/** @brief Serving system information.
*/
typedef struct sys_cdma_serv_sys_info_s
{
  uint16                 pilot_pn_offset;
    /**< Pilot Pseudo-Noise offset. */
  uint16                 channel;
    /**< Channel. */
  uint8                  pagech;
    /**< Paging Channel. */
  uint16                 base_id;
    /**< Base station ID */
  uint16                 sid;
    /**< System ID. */
  uint16                 nid;
    /**< Network ID. */
  uint8                  packet_zone_id;
    /**< Packet zone ID. */
  uint8                  p_rev;
    /**< Protocol revision. */
  uint8                  min_p_rev;
    /**< Mobile Identification Number revision. */
  int8                   nw_pref_sci;
    /**< Network preferred SCI (0-7). */
}sys_cdma_serv_sys_info_s_type;

/** @brief Channel configuration information.
*/
typedef struct sys_cdma_channel_config_s
{
  uint16                 fsch0_mux;
    /**< Forward Supplemental Channel MUX. */

  uint8                  fsch0_rc;
    /**< Forward Supplemental Channel Radio Configuration. */

  boolean                fsch0_turbo_supported;
    /**< Whether turbo is supported. */

  uint16                 rsch0_mux;
    /**< Reverse Supplemental Channel MUX. */

  uint8                  rsch0_rc;
    /**< Reverse Supplemental Channel Radio Configuration. */

  boolean                rsch0_turbo_supported;
    /**< Reverse Supplemental Channel turbo. */

  boolean                fsch0_active;
    /**< Forward Supplemental Channel active. */

  uint8                  fsch0_duration;
    /**< Forward Supplemental Channel duration. */

  uint8                  fsch0_sccl_index;
    /**< Sample correlate, choose largest index. */

  uint8                  fsch0_num_pilots;
    /**< Number of pilots. */

  boolean                rsch0_active;
    /**< Reverse Supplemental Channel active. */

  uint8                  rsch0_duration;
    /**< Reverse Supplemental Channel duration. */

  uint8                  scrm_seq_num;
    /**< Sequence number. */

}sys_cdma_channel_config_s_type;

/** @brief Control Protocol call statistics.
*/
typedef struct sys_cdma_cp_call_stat_s
{
  uint16                 orig_acc_att_count;
  /**<  Origination access attempts count. */

  uint16                 page_rsp_acc_att_count;
  /**< Page response access attempts count. */

}sys_cdma_cp_call_stat_s_type;

/** @brief Registration status.
*/
typedef struct sys_cdma_reg_stat_s
{

  uint16                 powerdown_reg_count;
    /**< Power down registration count. */

  uint16                 timer_reg_count;
    /**< Timer-based registration count. */

  uint16                 parameter_reg_count;
    /**< Parameter-based registration count. */

  uint16                 powerup_reg_count;
    /**< Power up registration count. */

  uint16                 ordered_reg_count;
    /**< Ordered registration count. */

  uint16                 zone_reg_count;
    /**< Zone-based registration count. */

  uint16                 distance_reg_count;
    /**< Distance-based registration count. */

}sys_cdma_reg_stat_s_type;

/** @brief CDMA 1X payload structure.
*/
typedef struct sys_cdma_information_s
{
  uint32                                 changed_bitmask;
    /**< Bitmask of the changed field. */

  sys_cdma_state_info_s_type             state_info;
    /**< CDMA state information. */

  sys_cdma_channel_meas_s_type           meas_info;
    /**< Channel measurement information. */

  sys_cdma_active_set_info_s_type        active_set_info;
    /**< Active set information. */

  sys_cdma_serv_sys_info_s_type          serv_sys_info;
    /**< Serving system information. */

  sys_cdma_channel_config_s_type         channel_config_info;
    /**< Channel configuration information. */

  sys_cdma_cp_call_stat_s_type           call_stat;
    /**< Originations/terminations statistics. */

  sys_cdma_reg_stat_s_type               reg_stat;
    /**< Registration information. */

  int32 cdma_1x_rx0_agc;
    /**< Automatic Gain Control. Range - 0 to -106 dB */

  int32 cdma_1x_rx1_agc;
    /**< Automatic Gain Control. Range - 0 to -106 dB */


}sys_cdma_information_s_type;

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/*----- STATS structures for HDR -----*/

/* Bitmask values */

/** @name Statistics for HDR Bitmask Values
    @{ */

#define SYS_HDR_INFO_MASK_NONE           0
#define SYS_HDR_INFO_MASK_ALL            SYS_BM_32BIT(0)

/** @} */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** State of HDR protocol stack.
*/
typedef enum sys_hdr_prot_state_e
{
  SYS_HDR_PROT_INACTIVE  = 0, /**< Inactive state. */
  SYS_HDR_PROT_INIT      = 1, /**< Initialization state. */
  SYS_HDR_PROT_IDLE      = 2, /**< Idle state. */
  SYS_HDR_PROT_CONNECTED = 3  /**< Connected state. */

} sys_hdr_prot_state_e_type;

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** @brief HDR embodied information.
*/
typedef struct sys_hdr_information_s
{
  uint32                    changed_bitmask;
    /**< Indicates the fields that have changed. */

  sys_channel_type          channel;
    /**< Channel: band and frequency information. */

  sys_hdr_prot_state_e_type hdr_prot_state;
    /**< HDR protocol state: inactive, initialized, idle, or connected. */

  int16   c2i;
    /**< SINR of the serving sector in dB. */

  int32   rx_dbm;
    /**< Rx power in dBm. */

  int32   tx_dbm;
    /**< Tx power in dBm. */

  int16   ecio;
    /**< Ec/Io of reference sector in dB. */

  uint32  uati_24;
    /**< Three least significant bits of the Unique Access Terminal
       Identifier (UATI). */

  uint8   color_code;
    /**< Color code for the UATI. */

  uint8   subnet_mask;
    /**< Subnet mask for the UATI. */

  uint16  mac_index;
    /**< Current MAC index. */

  int32 cdma_evdo_rx0_agc;
    /**< Automatic Gain Control. Range - 0 to -106 dB */

  int32 cdma_evdo_rx1_agc;
    /**< Automatic Gain Control. Range - 0 to -106 dB */


}sys_hdr_information_s_type;

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/*----- STATS structures for CM/SD -----*/

/* Bit mask values */

/** @name Statistics for CM/SD Bitmask Values
    @{ */

#define SYS_CM_INFO_MASK_NONE         0
#define SYS_CM_PH_PREF_BIT_MASK             SYS_BM_32BIT(0)
#define SYS_CM_SYS_SEL_INFO_BIT_MASK        SYS_BM_32BIT(1)
#define SYS_CM_PRL_INFO_BIT_MASK            SYS_BM_32BIT(2)
#define SYS_CM_CALL_STAT_BIT_MASK           SYS_BM_32BIT(3)
#define SYS_CM_INFO_MASK_ALL                (SYS_BM_32BIT(4) - 1)

/** @} */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

   /* For compatibility with QXDM database, please do not change the following
    fix assigned values. If new values are added please also update the QXDM
    database. */

/** PRL preferences.
*/
typedef enum sys_stats_prl_pref_e {

  /** @cond
  */
  SYS_STATS_PRL_PREF_NONE=0,   /* internal */
  /** @endcond
  */

  SYS_STATS_PRL_PREF_AVAIL_BC0_A = SYS_BM(SYS_PRL_AVAIL_BC0_A),
    /**< Acquire available system only on the B side. */

  SYS_STATS_PRL_PREF_AVAIL_BC0_B = SYS_BM(SYS_PRL_AVAIL_BC0_B),
    /**< Acquire available system only on the B side.  */

  SYS_STATS_PRL_PREF_ANY = 0x3FFF,
    /**< Acquire any available systems.  */

  SYS_STATS_PRL_PREF_NO_CHANGE,
    /**< Do not change the PRL preference.  */

  SYS_STATS_PRL_PREF_PERSISTENT,
    /**< Return to persistent preference setting.  */

  /** @cond
  */
  SYS_STATS_PRL_PREF_MAX   /* internal */
  /** @endcond
  */

} sys_stats_prl_pref_e_type;
/*~ SENTINEL sys_stats_prl_pref_e_type.SYS_STATS_PRL_PREF_MAX */

 /* For compatibility with QXDM database, please do not change the following
    fix assigned values. If new values are added please also update the QXDM
    database. */

/** Mode preferences.
*/
typedef enum sys_stats_mode_pref_e {

  /** @cond
  */
  SYS_STAT_MODE_PREF_NONE=-1,   /* internal */
  /** @endcond
  */

  SYS_STAT_MODE_PREF_AMPS_ONLY=0,
    /**< Service is limited to analog only (NV_MODE_ANALOG_ONLY). */

  SYS_STAT_MODE_PREF_DIGITAL_ONLY=1,
    /**< Service is limited to digital (CDMA, HDR, GW) only
       (NV_MODE_DIGITAL_ONLY). */

  SYS_STAT_MODE_PREF_AUTOMATIC=2,
    /**< Determine the mode automatically (NV_MODE_AUTOMATIC). */

  SYS_STAT_MODE_PREF_EMERGENCY=3,
    /**< Emergency mode (NV_MODE_EMERGENCY). Intended to be used internally
       by the CM.\ A client is not allowed to use it as parameter to change
     the system selection preference command.  */

  /* For compatibility with QPST, do not change values or
  ** order. We start with NV_MODE_CELL_CDMA_ONLY+1 (i.e. 9).
  */
  SYS_STAT_MODE_PREF_CDMA_ONLY = 9,
    /**< Service is limited to CDMA only.  */

  SYS_STAT_MODE_PREF_HDR_ONLY=10,
    /**< Service is limited to HDR only. */

  SYS_STAT_MODE_PREF_CDMA_AMPS_ONLY=11,
    /**< Service is limited to CDMA and AMPS only.  */

  SYS_STAT_MODE_PREF_GPS_ONLY=12,
    /**< Service is limited to GPS only. */

  /*
  ** The following enums are the radio access technologies for
  ** WCDMA and GSM.
  */
  SYS_STAT_MODE_PREF_GSM_ONLY=13,
    /**< Service is limited to GSM only. */

  SYS_STAT_MODE_PREF_WCDMA_ONLY=14,
    /**< Service is limited to WCMDA only. */

  /*
  ** Others
  */
  SYS_STAT_MODE_PREF_PERSISTENT=15,
    /**< Return to persistent mode preference. ph_ptr->main_stack_info.pref_info.mode_pref stores the
       equivalent mapped CM type. */

  SYS_STAT_MODE_PREF_NO_CHANGE=16,
    /**< Do not change the mode preference. ph_ptr->main_stack_info.pref_info.mode_pref stores the
       equivalent mapped CM type. */

  /*
  ** Even though logically the below belong with
  ** enums listed above, they were added at the end
  ** to not break compatibility with test tools
  */

  SYS_STAT_MODE_PREF_ANY_BUT_HDR=17,
    /**< Use any service but HDR.  */

  SYS_STAT_MODE_PREF_CURRENT_LESS_HDR=18,
    /**< Remove HDR component from current mode preference. ph_ptr->main_stack_info.pref_info.mode_pref
       stores the equivalent mapped CM type. */

  SYS_STAT_MODE_PREF_GSM_WCDMA_ONLY=19,
    /**< Service is limited to GSM and WCDMA only. */

  SYS_STAT_MODE_PREF_DIGITAL_LESS_HDR_ONLY=20,
    /**< Acquire digital, non-HDR mode systems only (CDMA or GSM or WCDMA ). */

  SYS_STAT_MODE_PREF_CURRENT_LESS_HDR_AND_AMPS=21,
    /**< Remove the HDR and AMPS component from the current mode preference.
       ph_ptr->main_stack_info.pref_info.mode_pref stores the equivalent mapped CM type. */

  SYS_STAT_MODE_PREF_CDMA_HDR_ONLY=22,
    /**< Acquire only CDMA or HDR systems. */

  SYS_STAT_MODE_PREF_CDMA_AMPS_HDR_ONLY=23,
    /**< Acquire only CDMA or AMPS or HDR systems. */

  SYS_STAT_MODE_PREF_CURRENT_LESS_AMPS=24,
    /**< Remove the AMPS component from the current mode preference.
       ph_ptr->main_stack_info.pref_info.mode_pref stores the equivalent mapped CM type. */

  /* Clients check CM_API_WLAN for WLAN support for following
  ** preferences.
  */

  SYS_STAT_MODE_PREF_WLAN_ONLY=25,
    /**< Acquire only WLAN systems. */

  SYS_STAT_MODE_PREF_CDMA_WLAN=26,
    /**< Acquire only CDMA and WLAN systems. */

  SYS_STAT_MODE_PREF_HDR_WLAN=27,
    /**< Acquire only HDR and WLAN systems. */

  SYS_STAT_MODE_PREF_CDMA_HDR_WLAN=28,
    /**< Acquire only CDMA, HDR, and WLAN systems. */

  SYS_STAT_MODE_PREF_GSM_WLAN=29,
    /**< Acquire only GSM and WLAN systems. */

  SYS_STAT_MODE_PREF_WCDMA_WLAN=30,
    /**< Acquire only WCDMA and WLAN systems. */

  SYS_STAT_MODE_PREF_GW_WLAN=31,
    /**< Acquire only GSM/WCDMA and WLAN systems. */

  SYS_STAT_MODE_PREF_CURRENT_PLUS_WLAN=32,
    /**< Acquire WLAN systems in addition to the current system. */

  SYS_STAT_MODE_PREF_CURRENT_LESS_WLAN=33,
    /**< Remove WLAN systems from the current system. */

  SYS_STAT_MODE_PREF_CDMA_AMPS_HDR_WLAN_ONLY = 34,
    /**< Acquire CDMA, AMPS, HDR, and WLAN systems. */

  SYS_STAT_MODE_PREF_CDMA_AMPS_WLAN_ONLY = 35,
    /**< Acquire CDMA, AMPS, and WLAN systems. */

  SYS_STAT_MODE_PREF_INTERSECT_OR_FORCE = 36,
    /**< Acquire the mode that is common to (the intersection of) the
         current mode and the requested mode. If the intersection is NULL,
         force the requested preferences. */

  SYS_STAT_MODE_PREF_ANY_BUT_HDR_WLAN = 37,
    /**< Use any service except HDR and WLAN. */

  SYS_STAT_MODE_PREF_LTE_ONLY = 38,
    /**< Service is limited to LTE only. */

  SYS_STAT_MODE_PREF_TDS_ONLY = 39,
    /**< Service is limited to TD-SCMA only */

  SYS_STAT_MODE_PREF_TDS_GSM = 40,
    /**< Service is limited to TD-SCMA or GSM only */

  SYS_STAT_MODE_PREF_TDS_GSM_LTE = 41,
    /**< Service is limited to TD-SCDMA or GSM or LTE */

  SYS_STAT_MODE_PREF_TDS_GSM_WCDMA_LTE = 42,
    /**< Service is limited to TD-SCDMA or GSM or WCDMA or LTE */

  SYS_STAT_MODE_PREF_TDS_GSM_WCDMA = 43,
    /**< Service is limited to TD-SCDMA or GSM or WCDMA */

  /** @cond
  */
  SYS_STAT_MODE_PREF_MAX   /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

} sys_stats_mode_pref_e_type;
/*~ SENTINEL sys_stats_mode_pref_e_type.SYS_STAT_MODE_PREF_MAX */

/** System roam status.

  @note1hang The roaming indicator values (SYS_ROAM_STATUS_OFF and
  SYS_ROAM_STATUS_ON) that are defined in db.h are swapped compared to
  the PRL roaming indicator values that are defined in TSB-58.
*/
typedef enum {

  SYS_STAT_ROAM_OFF        = SYS_ROAM_STATUS_OFF,
  /**< Roaming indicator is OFF. */

  SYS_STAT_ROAM_ON         = SYS_ROAM_STATUS_ON,
  /**< Roaming indicator is ON. */

  SYS_STAT_ROAM_FLASH      = SYS_ROAM_STATUS_BLINK,
  /**< Roaming indicator is flashing. */

  /** @cond
  */
  SYS_STAT_ROAM_MAX        = 0xFF
  /** @endcond
  */

} sys_stat_roam_e_type;

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** @brief Phone information.
*/
typedef struct sys_cm_ph_info_s
{

 sys_stats_prl_pref_e_type      pref_sys;
  /**< Preferred system. */

 sys_stats_mode_pref_e_type     mode_pref;
  /**< Mode. */

 sys_band_mask_e_type           band_cap;
  /**< PCS/Cellular/AWS supported band capability. */

}sys_cm_ph_info_s_type;

/** @brief System selection information structure.
*/
typedef struct sys_cm_sys_sel_info_s
{

 sys_sys_id_s_type          sel_sys;
  /**< Selected system. */

 sys_stat_roam_e_type       roam_ind;
  /**< Roaming indication. */

}sys_cm_sys_sel_info_s_type;

/** @brief CM payload.
*/
typedef struct sys_cm_information_s
{

  uint32                        changed_bitmask;
    /**< Bitmask. */

  sys_cm_ph_info_s_type         ph_info;
    /**< Phone information. */

  sys_cm_sys_sel_info_s_type    sys_sel_info;
    /**< System selection information. */

  word                          prl_ver;
    /**< PRL version. */

  uint16                        send_key_orig_count;
    /**< Number of times the SEND key was pressed. */

}sys_cm_information_s_type;

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/*----- STATS structures for DS -----*/

/* Bit mask values */
/** @name Status for DS Bitmask Values
    @{ */

#define SYS_DS_INFO_MASK_NONE              0
#define SYS_DS_INFO_BIT_MASK               SYS_BM_32BIT(0)
#define SYS_DS_INFO_MASK_ALL               SYS_DS_INFO_BIT_MASK

/** @} */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** @brief Forward and reverse link data.
*/
typedef struct sys_ds_hdr_link_info_s
{

  uint32 hdr_fwd_data_rate;
    /**< HDR forward link data rate in bytes/sec. */

  uint32 hdr_rev_data_rate;
    /**< HDR reverse link data rate in bytes/sec. */

}sys_ds_hdr_link_info_s_type;

/** @brief Data Services payload.
*/
typedef struct sys_ds_information_s
{

  uint32                   changed_bitmask;
  /**< Bitmask indicating which fields changed. */

  sys_ds_hdr_link_info_s_type  ds_hdr_link_info;
  /**< Forward and reverse link data. */

}sys_ds_information_s_type;

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/*----- STATS structures for NAS -----*/

/* NAS bitmask values */

/** @name Statistics for NAS Bitmask Values
    @{ */

#define SYS_NAS_INFO_MASK_NONE          0
#define SYS_NAS_REGISTRATION_INFO_MASK  SYS_BM_64BIT(0)
#define SYS_NAS_ATTACH_TYPE_MASK        SYS_BM_64BIT(1)
#define SYS_NAS_PERIODIC_TIMER_MASK   SYS_BM_64BIT(2)
#define SYS_NAS_CS_REJECT_CAUSE_MASK    SYS_BM_64BIT(3)
#define SYS_NAS_PS_REJECT_CAUSE_MASK    SYS_BM_64BIT(4)
#define SYS_NAS_PS_CALL_INFO_MASK       SYS_BM_64BIT(5)

/** All NAS statistics bitmask values.
*/
#define SYS_NAS_INFO_MASK_ALL           ( SYS_NAS_REGISTRATION_INFO_MASK | \
                                          SYS_NAS_ATTACH_TYPE_MASK       | \
                                          SYS_NAS_PERIODIC_TIMER_MASK    | \
                                          SYS_NAS_CS_REJECT_CAUSE_MASK   | \
                                          SYS_NAS_PS_REJECT_CAUSE_MASK   | \
                                          SYS_NAS_PS_CALL_INFO_MASK )

/** @} */

#define SYS_TMSI_SIZE   4

#define SYS_PTMSI_SIZE  SYS_TMSI_SIZE

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** @brief Registered network information.
*/
typedef struct sys_registered_nw_info_s
{
   byte        tmsi [SYS_TMSI_SIZE];        /**< TMSI value. */
   byte        ptmsi[SYS_PTMSI_SIZE];       /**< PTMSI value. */
   boolean     gprs_attach_status;          /**< Attached or detached. */
   sys_plmn_id_s_type  rplmn;               /**< PLMN info */
} sys_registered_nw_info_s_type;

/** @brief Periodic timer.
*/
typedef struct sys_nas_periodic_timers_s
{
  dword     T3212_value;
    /**< Periodic location update timer value (T3212). */

  dword     T3312_value;
    /**< Periodic Routing Area Update (RAU) timer value (T3312). */

} sys_nas_periodic_timers_s_type;

/** @brief Packet-Switched call information.
*/
typedef struct sys_ps_call_info_s
{
   pdp_address_T    pdp_address;  /**< Active PDP context: IP address. */
   qos_T            qos;          /**< Active PDP context: QoS. */
   apn_T            apn;          /**< Active PDP context: APN. */

}sys_ps_call_info_s_type;

/** @brief Non-Access Stratum information.
*/
typedef struct sys_nas_information_s
{
   uint16                         changed_bitmask;
     /**< Indicates changed information. */
   byte                           attach_type;
     /**< Attached type. */
   byte                           cs_reject_cause;
     /**< Mobility Management (MM) reject cause code. */
   byte                           ps_reject_cause;
     /**< GPRS MM (GMM) reject cause code. */
   sys_registered_nw_info_s_type  reg_nw_info;
     /**< Registered Network information. */
   sys_nas_periodic_timers_s_type periodic_timers;
     /**< Periodic timer */
   sys_ps_call_info_s_type        ps_call_info;
     /**< Active PS call information. */
   sys_sys_mode_e_type                  sys_mode;
   /**< RAT on which nas reported statistics */

} sys_nas_information_s_type;

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/*----- STATS structures for RRC -----*/

/** @name Statistics Bitmask Values
    @{ */

#define SYS_WCDMA_INFO_MASK_NONE                           0
#define SYS_WCDMA_SERVING_CELL_INFO_BIT_MASK               SYS_BM_64BIT(0)
#define SYS_WCDMA_SERVING_CELL_POWER_INFO_BIT_MASK         SYS_BM_64BIT(1)
#define SYS_WCDMA_ACTIVE_SET_REFERENCE_CELL_INFO_BIT_MASK  SYS_BM_64BIT(2)
#define SYS_WCDMA_ACTIVE_SET_INFO_BIT_MASK                 SYS_BM_64BIT(3)
#define SYS_WCDMA_MONITORED_SET_BIT_MASK                   SYS_BM_64BIT(4)
#define SYS_WCDMA_RRC_STATE                                SYS_BM_64BIT(5)

/** All statistics bitmask values.
*/
#define SYS_WCDMA_INFO_MASK_ALL (SYS_WCDMA_SERVING_CELL_INFO_BIT_MASK | \
                                 SYS_WCDMA_SERVING_CELL_POWER_INFO_BIT_MASK | \
                                 SYS_WCDMA_ACTIVE_SET_REFERENCE_CELL_INFO_BIT_MASK | \
                                 SYS_WCDMA_ACTIVE_SET_INFO_BIT_MASK | \
                                 SYS_WCDMA_MONITORED_SET_BIT_MASK | \
                                 SYS_WCDMA_RRC_STATE )

/** @} */

/** @name Modem Statistics Macro Definitions
    @{ */

#define SYS_WCDMA_INVALID_CELL_ID     0xFFFFFFFF
#define SYS_WCDMA_INVALID_URA         0xFFFFFFFF
#define SYS_WCDMA_MAX_ASET_CELLS               6
#define SYS_WCDMA_MAX_MONITORED_FREQ           3
#define SYS_WCDMA_MAX_MONITORED_CELLS_PER_FREQ 8
#define SYS_WCDMA_MAX_MONITORED_GSM_CELLS      8

/** @} */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/* Enums */

typedef enum
{
  SYS_EUTRA_DETECTION_UNKNOWN = 0,
    /**< when EUTRA CELL info is not available in case of state transition */
  SYS_EUTRA_CELL_PRESENT,       /**< when EUTRA cell is detected */
  SYS_EUTRA_CELL_NOT_PRESENT,   /**< when EUTRA cell is detected */
  SYS_EUTRA_DETECTION_UNSUPPORTED,
    /**< when EUTRA detection is not supported as SIB19 IE is not set*/

/** @cond
*/
  SYS_EUTRA_DETECTION_MAX   /**< FOR INTERNAL USE ONLY! */
/** @endcond
*/
} sys_eutra_status_enum_type;

/** Network operation mode information.
*/
typedef enum
{
   SYS_NETWORK_OP_MODE_1,              /** &nbsp; *///!< SYS_NETWORK_OP_MODE_1
   SYS_NETWORK_OP_MODE_2,              /** &nbsp; *///!< SYS_NETWORK_OP_MODE_2
   SYS_NETWORK_OP_MODE_3,              /** &nbsp; *///!< SYS_NETWORK_OP_MODE_3
   SYS_PS_DOMAIN_SYS_INFO_UNAVAILABLE  /** &nbsp; *///!< SYS_PS_DOMAIN_SYS_INFO_UNAVAILABLE
} sys_network_op_mode_e_type;

/** WCDMA Radio Resource Control state type.
*/
typedef enum
{
  SYS_WCDMA_RRC_STATE_DISCONNECTED, /** &nbsp; */
  SYS_WCDMA_RRC_STATE_CELL_FACH,    /** &nbsp; */
  SYS_WCDMA_RRC_STATE_CELL_DCH,     /** &nbsp; */
  SYS_WCDMA_RRC_STATE_CELL_PCH,     /** &nbsp; */
  SYS_WCDMA_RRC_STATE_URA_PCH,      /** &nbsp; */
} sys_wcdma_rrc_state_e_type;

/* enum type definition for spreading factor */
typedef enum {
  SYS_WCDMA_L1_SF_4,
  SYS_WCDMA_L1_SF_8,
  SYS_WCDMA_L1_SF_16,
  SYS_WCDMA_L1_SF_32,
  SYS_WCDMA_L1_SF_64,
  SYS_WCDMA_L1_SF_128,
  SYS_WCDMA_L1_SF_256,
  SYS_WCDMA_L1_SF_512,
  SYS_WCDMA_L1_NUM_SF
} sys_wcdma_l1_sf_e_type;

typedef enum {
  SYS_WCDMA_L1_DL_PHYCHAN_PCCPCH_S,
  SYS_WCDMA_L1_DL_PHYCHAN_PCCPCH_N,
  SYS_WCDMA_L1_DL_PHYCHAN_SCCPCH0,
  SYS_WCDMA_L1_DL_PHYCHAN_SCCPCH1,
  SYS_WCDMA_L1_DL_PHYCHAN_PICH,
  SYS_WCDMA_L1_DL_PHYCHAN_AICH,
  SYS_WCDMA_L1_DL_PHYCHAN_HS_RACH_AICH,
  SYS_WCDMA_L1_DL_PHYCHAN_DPCH,
  SYS_WCDMA_L1_DL_PHYCHAN_HS_RACH_FDPCH,
  SYS_WCDMA_L1_DL_PHYCHAN_FDPCH,
  SYS_WCDMA_L1_DL_PHYCHAN_PDSCH,
  SYS_WCDMA_L1_NUM_DL_PHYCHAN,
  SYS_WCDMA_L1_DL_PHYCHAN_NOCHAN
} sys_wcdma_l1_dl_phychan_e_type;

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** @brief WCDMA cell information.
*/
typedef struct
{
  uint16 psc;                 /**< Primary Scrambling Code. */
  int16 rscp;                 /**< Received Signal Code Power. */
  int16 ecio;                 /**< Ec/Io. */
  int16 squal;   /*S_qual */
  int16 srxlev;    /*S_rxlev */
  int16 rank;    /* Rank of the cell */
  uint8 set;    /* Set of the cell */
} sys_wcdma_cell_info_s_type;

/** @brief WCDMA serving cell information.
*/
typedef struct
{

  uint16 uarfcn;
    /**< Serving cell DL UARFCN. */

  uint16 psc;
    /**< Serving cell primary scrambling code. */

  sys_plmn_id_s_type plmn_id;
    /**< PLMN ID. */

  sys_lac_type lac_id;
    /**< Indicates the LAC. */

  uint16  rac_id;
    /**< Indicates the RAC. */

  uint32 ura;
    /**< UTRAN Registration Area (URA). */

  uint32 cell_id;
    /**< Indicates the service cell ID. */

  sys_network_op_mode_e_type nw_operation_mode;
    /**< Network operation mode. */

} sys_wcdma_serving_cell_info_s_type;

/** @brief WCDMA serving cell power information.
*/
typedef struct
{

  uint16 psc;
    /**< Serving cell Primary Scrambling Code. */

  int16 rscp;
    /**< Received Signal Code Power. */

  int16 ecio;
    /**< Ec/Io. */

  int16 squal;
   /*S_qual */

  int16 srxlev;
  /*S_rxlev */

}sys_wcdma_serving_cell_power_info_s_type;

/** @brief WCDMA active set reference Radio Link information.
*/
typedef struct
{

  uint16 uarfcn;
    /**< DL UARFCN. */

  uint16 psc;
    /**< Primary Scrambling Code. */

  sys_plmn_id_s_type plmn_id;
    /**< PLMN ID. */

  sys_lac_type lac_id;
    /**< Indicates the LAC. */

  uint16  rac_id;
    /**< Indicates the RAC. */

  uint32 cell_id;
    /**< Indicates the network cell. */

} sys_wcdma_active_set_reference_rl_info_s_type;

/** @brief WCDMA active set information.
*/
typedef struct sys_wcdma_active_set_info_s
{

  uint8 num_rls;
    /**< Number of Radio Links in the active set. */

  uint16 psc[SYS_WCDMA_MAX_ASET_CELLS];
    /**< Primary Scrambling Codes. */

  uint32 cell_id[SYS_WCDMA_MAX_ASET_CELLS];
    /**< Cell ID. */

  int16  rscp[SYS_WCDMA_MAX_ASET_CELLS];
    /**< Received Signal Code Power. */

  int16 ecio[SYS_WCDMA_MAX_ASET_CELLS];
    /**< Ec/Io. */

  /* spreading factor of the channel */
  sys_wcdma_l1_sf_e_type sf;

  /* Physical channel type FDPCH/DPCH */
  sys_wcdma_l1_dl_phychan_e_type phy_chan_type;

  /* Slot format for the channel */
  uint8 slot_format;

  /*indicates CM On or not*/
  boolean is_compressed_mode_on;
  
} sys_wcdma_active_set_info_s_type;
/*~ FIELD sys_wcdma_active_set_info_s.psc
    VARRAY SYS_WCDMA_MAX_ASET_CELLS LENGTH sys_wcdma_active_set_info_s.num_rls */
/*~ FIELD sys_wcdma_active_set_info_s.cell_id
    VARRAY SYS_WCDMA_MAX_ASET_CELLS LENGTH sys_wcdma_active_set_info_s.num_rls */
/*~ FIELD sys_wcdma_active_set_info_s.rscp
    VARRAY SYS_WCDMA_MAX_ASET_CELLS LENGTH sys_wcdma_active_set_info_s.num_rls */
/*~ FIELD sys_wcdma_active_set_info_s.ecio
    VARRAY SYS_WCDMA_MAX_ASET_CELLS LENGTH sys_wcdma_active_set_info_s.num_rls */

/** @brief Frequency information of WCDMA monitored systems.
*/
typedef struct sys_wcdma_monitored_freq_info_s
{

  uint16 uarfcn;
    /**< UARFCN. */

  uint8 num_of_cells;
    /**< Number of cells in the frequency. */

  sys_wcdma_cell_info_s_type cell[SYS_WCDMA_MAX_MONITORED_CELLS_PER_FREQ];
    /**< Cells information. */

} sys_wcdma_monitored_freq_info_s_type;
/*~ FIELD sys_wcdma_monitored_freq_info_s.cell
    VARRAY SYS_WCDMA_MAX_MONITORED_CELLS_PER_FREQ LENGTH sys_wcdma_monitored_freq_info_s.num_of_cells */

/** @brief GSM cell information.
*/
typedef struct
{
  /*ARFCN*/
  uint16 arfcn;      /**< ARFCB; range 0-1023. */
  /*BSIC-NCC*/
  uint8 bsic_ncc;    /**< BSIC-NCC; range 0-7, 0XFF is invalid. */
  /*BSIC-BCC*/
  uint8 bsic_bcc;    /**< BSIC-BCC; range 0-7, 0XFF is invalid. */
  /*RSSI*/
  int16 rssi;        /**< RSSI. */
  /*BCCH*/
  uint16 bcch;       /**< BCCH */

  int16 rank;    /* Rank of the cell */
} sys_wcdma_gsm_cell_info_s_type;

/** @brief LTE cell information.
*/
typedef struct
{

   /*EARFCN*/
   uint32 earfcn;

   /* The physical cell id of the detected cell */
   uint16 cell_id;

   /* Max RSRP comb.value combined across Tx-Rx pairs. In linear scale. */
   int16 rsrp;

   /* Max RSRQ value combined across Tx-Rx pairs. In linear scale. */
   int16 rsrq;

   /* LTE cell rxlev */
   int16  s_rxlev;

   /* check if a cell is LTE */
   boolean is_tdd_freq;

} sys_wcdma_lte_cell_info_s_type;


/** @brief Monitored set information.
*/
typedef struct sys_wcdma_monitored_set_info_s
{

  uint8 num_of_freqs;
    /**< Number of frequencies. */

  sys_wcdma_monitored_freq_info_s_type freq[SYS_WCDMA_MAX_MONITORED_FREQ];
    /**< Frequency information. */

  uint8 num_gsm_cells;
    /**< Number of GSM cells. */  /* Range 0-WCDMA_MAX_MONITORED_GSM_CELLS */

  sys_wcdma_gsm_cell_info_s_type gsm_cell[SYS_WCDMA_MAX_MONITORED_GSM_CELLS];
    /**< GSM cell information. */

  uint8 num_lte_cells;
      /**< Number of LTE cells. */  /* Range 0- SYS_WCDMA_MAX_MONITORED_LTE_CELLS */

  sys_wcdma_lte_cell_info_s_type lte_cell[SYS_WCDMA_MAX_MONITORED_LTE_CELLS];


} sys_wcdma_monitored_set_info_s_type;
/*~ FIELD sys_wcdma_monitored_set_info_s.freq
    VARRAY SYS_WCDMA_MAX_MONITORED_FREQ LENGTH sys_wcdma_monitored_set_info_s.num_of_freqs */
/*~ FIELD sys_wcdma_monitored_set_info_s.gsm_cell
    VARRAY SYS_WCDMA_MAX_MONITORED_GSM_CELLS LENGTH sys_wcdma_monitored_set_info_s.num_gsm_cells */

/** @brief Generic event from the WCDMA stack.
*/
typedef struct
{

  sys_wcdma_serving_cell_info_s_type        wcdma_serving_cell_info;
    /**< Serving system cell information. */

  sys_wcdma_serving_cell_power_info_s_type  wcdma_serving_cell_power_info;
    /**< Power range of the serving cell. */

  sys_wcdma_active_set_reference_rl_info_s_type
                                     wcdma_active_set_reference_rl_info;
    /**< Active set reference Radio Link information. */

  sys_wcdma_active_set_info_s_type          wcdma_active_set_info;
    /**< Active set information. */

  sys_wcdma_monitored_set_info_s_type       wcdma_monitored_set_info;
    /**< Monitored set information. */

  sys_wcdma_rrc_state_e_type                wcdma_rrc_state;
    /**< Radio Resource Control state type. */

  uint16 gw_dl_bler;
    /**< Block Error Ratio. Range - 0 to 63. */

  int32 w_agc;                             
    /* rx agc, only WCDMA need AGC, GSM can use rssi */
    /**< Automatic Gain Control. Range - 0 to -106 dB */

  int32 w_txagc;
    /**< Automatic Gain Control. Range - 0 to -106 dB */


} sys_wcdma_event_info_type;

/** @brief Radio Resource Control message from the WCDMA stack.
*/
typedef struct
{

  uint64 changed_bitmask;
    /**< Bitmask. */

  sys_wcdma_event_info_type wcdma_event_info;
    /**< Generic event from the WCDMA stack. */

} sys_wcdma_information_s_type;

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/*---- RR structures ----*/


/* Bitmask values */
/** @name Radio Resource Bitmask Values
    @{ */

#define SYS_GERAN_ENG_MODE_NONE                0
#define SYS_GERAN_ENG_MODE_TIMING_ADVANCE      SYS_BM_64BIT(0)
#define SYS_GERAN_ENG_MODE_NMR_INFO            SYS_BM_64BIT(1)
#define SYS_GERAN_ENG_MODE_RX_QUAL             SYS_BM_64BIT(2)
#define SYS_GERAN_ENG_MODE_TX_PWR              SYS_BM_64BIT(3)
#define SYS_GERAN_ENG_MODE_SCELL_INFO          SYS_BM_64BIT(4)
#define SYS_GERAN_ENG_MODE_SCELL_MEASUREMENTS  SYS_BM_64BIT(5)
#define SYS_GERAN_ENG_MODE_AMR_INFO            SYS_BM_64BIT(6)
#define SYS_GERAN_ENG_MODE_DTX_UL_STATUS       SYS_BM_64BIT(7)
#define SYS_GERAN_ENG_MODE_TBF_INFO            SYS_BM_64BIT(8)
#define SYS_GERAN_ENG_MODE_NW_PARAMS           SYS_BM_64BIT(9)
#define SYS_GERAN_ENG_MODE_GPRS_UL_CS          SYS_BM_64BIT(10)
#define SYS_GERAN_ENG_MODE_GPRS_DL_CS          SYS_BM_64BIT(11)
#define SYS_GERAN_ENG_MODE_EGPRS_UL_MCS        SYS_BM_64BIT(12)
#define SYS_GERAN_ENG_MODE_EGPRS_DL_MCS        SYS_BM_64BIT(13)
#define SYS_GERAN_ENG_MODE_EDGE_MEAS           SYS_BM_64BIT(14)
#define SYS_GERAN_ENG_SCELL_CONFIG             SYS_BM_64BIT(15)
#define SYS_GERAN_ENG_HOPPING_STATUS           SYS_BM_64BIT(16)
#define SYS_GERAN_ENG_MODE_CURRENT_L1_TIMESLOT SYS_BM_64BIT(17)

/** All radio resource bitmask values.
*/
#define SYS_GERAN_ENG_MODE_ALL ( SYS_GERAN_ENG_MODE_TIMING_ADVANCE | \
                       SYS_GERAN_ENG_MODE_NMR_INFO       | \
                       SYS_GERAN_ENG_MODE_RX_QUAL        | \
                       SYS_GERAN_ENG_MODE_TX_PWR         | \
                       SYS_GERAN_ENG_MODE_SCELL_INFO     | \
                       SYS_GERAN_ENG_MODE_SCELL_MEASUREMENTS | \
                       SYS_GERAN_ENG_MODE_AMR_INFO       | \
                       SYS_GERAN_ENG_MODE_DTX_UL_STATUS  | \
                       SYS_GERAN_ENG_MODE_TBF_INFO       | \
                       SYS_GERAN_ENG_MODE_NW_PARAMS      | \
                       SYS_GERAN_ENG_MODE_GPRS_UL_CS     | \
                       SYS_GERAN_ENG_MODE_GPRS_DL_CS     | \
                       SYS_GERAN_ENG_MODE_EGPRS_UL_MCS   | \
                       SYS_GERAN_ENG_MODE_EGPRS_DL_MCS   | \
                       SYS_GERAN_ENG_MODE_EDGE_MEAS      | \
                       SYS_GERAN_ENG_SCELL_CONFIG        | \
                       SYS_GERAN_ENG_HOPPING_STATUS | \
                       SYS_GERAN_ENG_MODE_CURRENT_L1_TIMESLOT)

/** @} */

#define SYS_ENG_MODE_MAX_NMR_ITEMS        6
#define SYS_ENG_MODE_TX_PWR_NUM_TIMESLOTS 8

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** Engine mode cell callback cause type.
*/
typedef enum
{
  SYS_ENG_MODE_NO_CAUSE = 1          /**< &nbsp; *///!< SYS_ENG_MODE_NO_CAUSE

} sys_eng_mode_cell_cb_cause_e_type;

/** Reception type of a channel.
*/
typedef enum
{
  RX_LEVEL_TCH_PDTCH = 0,     /**< &nbsp; */
  RX_LEVEL_BCCH               /**< &nbsp; */

} sys_rx_level_channel_type_e_type;

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** @brief Neighbor cell information.
*/
typedef struct
{
  uint32              cell_id;
    /**< Cell ID. */

  /* range 0..65535, cell id not available: 0xffffffff
     Data in sys_mode_ncell_id_info_s_type is invalid when cell_id invalid*/

  uint16              lac;
    /**< Location Area Code. */       /* range 0..65535 */

  sys_plmn_id_s_type  plmn;
    /**< PLMN ID. */

} sys_mode_ncell_id_info_s_type;

/** @brief Network Measurement Report.
*/
typedef struct
{
  uint16                   arfcn;
    /**< Absolute Radio Frequency Channel Number. */

  uint8                    bsic;
    /**< Base Station Identity Code. */

  uint8                    rxlev;
    /**< Receive level. */

  sys_mode_ncell_id_info_s_type  ncell_id;
    /**< Neighboring cell ID. */

  int32                        C1; 
    /*C1 as defined in 45.008 6.4, default: 0*/
    
  int32                        C2; 
    /*C1 as defined in 45.008 6.4, default: 0*/
    
  int32                        C31; 
    /*C31 as defined in 45.008 10.1.2, default: 0*/
    
  int32                        C32; 
    /*C32 as defined in 45.008 10.1.2, default: 0*/

} sys_nmr_element_s_type;

/** @brief Network Measurement Report engine mode.
*/
typedef struct sys_eng_mode_nmr_info_s
{
  uint8                     num_of_items;
    /**< Number of NMRs. Set to zero if NMR information is invalid or there are
      no neighbors to report. */

  sys_nmr_element_s_type     nmr[SYS_ENG_MODE_MAX_NMR_ITEMS];
    /**< NMR array. */

  time_type                time_stamp;
    /**< Indicates when information was collected. */

} sys_eng_mode_nmr_info_s_type;
/*~ FIELD sys_eng_mode_nmr_info_s.nmr
    VARRAY SYS_ENG_MODE_MAX_NMR_ITEMS LENGTH sys_eng_mode_nmr_info_s.num_of_items */

/** @brief Serving cell information (currently camped cell). Describes the
    identity of the serving cell.
*/
typedef struct
{
  uint32                   cell_id;
    /**< Cell ID. Range is 0 to 65535; cell ID not available = 0xffffffff
         (all data in eng_mode_scell_info_t is invalid when cell_id is invalid). */

  sys_lac_type             lac;
    /**< Location Area Code. */                      /* range 0..65535 */

  sys_plmn_id_s_type       plmn;
    /**< Public Land Mobile Network. */

  uint16                   arfcn;
    /**< Absolute Radio Frequency Channel Number. */     /*range 0..1023 */

  uint8                    bsic;
    /**< Base Station Identity Code. */             /*range 0…63, see 24.008 10.5.1.3*/

  sys_eng_mode_cell_cb_cause_e_type cell_info_cause;
    /**< Cause code of the serving cell. */

  uint16                   rac;
    /**< RAC (not used). */        /*range 0..255, invalid=0xffff (i.e. GPRS not supported)*/

} sys_eng_mode_scell_info_s_type;

/** @brief Serving cell measurements, including signal strength and quality.
*/
typedef struct
{
  int32                   C1;
    /**< C1 as defined in 45.008 6.4, default: 0. */

  int32                   C2;
    /**< C2 as defined in 45.008 6.4, default: 0. */

  uint16                  rx_lev;
    /**< RX_LEV range is 0 to 63. See 3GPP TS 45.008 Section 8.1.4; default = 0. */

  uint16                  rx_qual_sub;
    /**< Reception quality in dedicated mode. The range is 0 to 7; 0xffff is
         invalid. */

  sys_rx_level_channel_type_e_type channel_type;
    /**< Indicates channel rx_lev is measured on. */

} sys_eng_mode_scell_measurements_s_type;

/** @brief Dynamic information about the current Adaptive Multirate (AMR)
    voice call.
*/
typedef struct
{
  uint8   ul_chan_mode; /**< Uplink channel mode.  */

  uint8   dl_chan_mode; /**< Downlink channel mode: \n
                              0 -- Full rate.\n
                              1 -- Half rate.\n
                              2 -- Enhanced full rate.\n
                              3 -- AMR full rate.\n
                              4 -- AMR half rate.\n
                              0xFF -- Invalid mode. */

  uint8   ul_acs[4];    /**< Uplink Active Code Set; AMR coding group. */

  uint8   dl_acs[4];    /**< 0 -- 4.75 kbit/s.\n
                             1 -- 5.15 kbit/s.\n
                             2 -- 5.90 kbit/s.\n
                             3 -- 6.70 kbit/s.\n
                             4 -- 7.40 kbit/s.\n
                             5 -- 7.95 kbit/s.\n
                             6 -- 10.2 kbit/s.\n
                             7 -- 12.2 kbit/s.\n
                             0xFF -- Undefined.  */

  boolean dl_dtx;       /**< 1 = On, 0 = Off; default = 0. */

  uint16  dl_c_over_i;  /**< Range 0..63, 0xFFFF -- Invalid. */

} sys_eng_mode_amr_info_s_type;

/** @brief Dynamic information about the currently active EGPRS Token Bucket
    Filter (TBF).
*/
typedef struct
{
  uint8 mac_mode;       /**< As specified in 3GPP TS 4.060 chapter 11.2.7.2.\n
                             0 -- Dynamic Allocation.\n
                             1 -- Extended Dynamic Allocation.\n
                             0xff -- Invalid. */

  uint8  tbf_mode;      /**< 1 -- GPRS.\n
                             2 -- EGPRS.\n
                             0xff -- Invalid. */

} sys_eng_mode_tbf_info_s_type;

/** @brief Network parameters, such as Network Mode of Operation (NMO), which
    typically apply to an operator.
*/
typedef struct
{
  uint8 nc_mode;
    /**< Network Control mode; relates to how the mobile sends measurements and
       performs reselection:
          - 0 -- NC1 mode.
          - 1 -- NC2 mode.
          - 0xff -- Invalid. */

  uint8 nmo;
    /**< Network Mode of Operation:
         - 0 -- NMO I.
         - 1 -- NMO II.
         - 2 -- NMO III.
         - 0xff -- Invalid. */

} sys_eng_mode_nw_params_s_type;

/** @brief Quality measurements taken while an EGPRS TBF is active.
*/
typedef struct
{

  uint8 lq_meas_mode;
    /**< Link Quality measurement mode. */
    /* spec 4.60 chap 11.2.7.2 ref for detail values 0 to 3. 0xff = invalid */

  uint8 bep_period;
    /**< Bit Error Probability measured during EGPRS TBF. */
    /* spec ref 4.60 cha 12.24.1 and 5.08  chap 10.2.3.2.1 0xff = invalid */

} sys_eng_mode_edge_meas_s_type;

/** @brief Serving cell information that describes the configuration data of
    the cell.
*/
typedef struct
{

  uint8     pbcch_present;
    /**< Presence of PBCCH in cell:
       - 0 -- No.
       - 1 -- Yes.
       - 0xff -- Invalid. */

  uint8     gprs_rxlev_access_min; /*0..63, 0xff - invalid, see 45.008*/
  uint8     gprs_ms_txpwr_max_cch; /*0..31, 0xff - invalid, see 45.005, 45.008*/	   

} sys_eng_mode_scell_config_s_type;

/** @brief Determines whether hopping is enabled or disabled.
*/
typedef struct
{
  uint8    hopping_status;
    /**< Hopping status:
       - 0 -- No.
       - 1 -- Yes.
       - 0xff -- Invalid. */
} sys_eng_mode_hopping_status_s_type;

/***********************************************************************/
/************************ ENG_MODE_CURRENT_L1_TIMESLOT *****************/
/***********************************************************************/
typedef struct
{
  uint8 timeslot; /*timeslot number 0..7, 0xff - invalid*/
} sys_eng_mode_curr_l1_ts_s_type;

/** @brief Final Radio Resource (RR) structure.
*/
typedef struct
{

  uint64                     changed_bitmask;
    /**< Indicates the updated values in GSM. */

  uint32                     timing_advance_value;
    /**< Time offset; changes with the mobile distance from the base station. */

  sys_eng_mode_nmr_info_s_type nmr_info;
    /**< Network Measurement Report engine mode. */

  uint32                     rx_qual;
    /**< Reception quality. */  /*0xffffffff - Invalid*/

  int16                   tx_pwr_in_use[SYS_ENG_MODE_TX_PWR_NUM_TIMESLOTS];
    /**< Transmission power in use. */                      /*0 - Invalid*/

  sys_eng_mode_scell_info_s_type      scell_info;
    /**< Serving cell information (currently camped cell). */

  sys_eng_mode_scell_measurements_s_type  scell_measurements;
    /**< Serving cell measurements, including signal strength and quality. */

  sys_eng_mode_amr_info_s_type   amr_info;
    /**< Dynamic information about the current AMR voice call. */

  uint8                      dtx_ul_status;
    /**< Discontinuous Transmission status. */    /*0 - Off, 1 - On, 0xff - Invalid*/

  sys_eng_mode_tbf_info_s_type   tbf_info;
    /**< Dynamic information about the currently active EGPRS TBF. */

  sys_eng_mode_nw_params_s_type  nw_params;
    /**< Network parameters, such as NMO, which apply to an operator. */

  uint8                      gprs_ul_cs;
    /**< GPRS coding scheme used in Upload GPRS TBFs. */      /* Range 0-3, 0xff - invalid */

  uint8                      gprs_dl_cs;
    /**< GPRS coding scheme used in Download GPRS TBFs. */    /* Range 0-3, 0xff - invalid */

  uint8                      egprs_ul_mcs;
    /**< EGPRS coding scheme used in Upload EGPRS TBFs. */    /* Range 0-8, 0xff - invalid */

  uint8                      egprs_dl_mcs;
    /**< EGPRS coding scheme used in DL EGPRS TBFs. */        /* Range 0-8 , 0xff - invalid */

  sys_eng_mode_edge_meas_s_type       edge_meas;
    /**< Quality measurements taken while an EGPRS TBF is active. */

  sys_eng_mode_scell_config_s_type    scell_config;
    /**< Serving cell information that describes the configuration data of the
       cell. */

  /*ENG_MODE_HOPPING_STATUS*/
  sys_eng_mode_hopping_status_s_type  hopping_status;

  uint16 g_bcch;
    /**< Channel number. ARFCN. Range - 0 to 1023 */

  uint32 g_ta;
    /**< Timing Advance. Range - 0 to 63 */

  /*ENG_MODE_CURRENT_L1_TIMESLOT*/
  sys_eng_mode_curr_l1_ts_s_type      current_timeslot;

} sys_gsm_information_s_type;

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** @addtogroup SI Service Indicators
    @{ */

/*----- STATS structures for LTE -----*/

/* Bit mask values */
/** @name Status for LTE Bitmask Values
    @{ */

#define SYS_LTE_INFO_MASK_NONE             0
#define SYS_LTE_MEAS_INFO_BIT_MASK         SYS_BM_32BIT(0)
#define SYS_LTE_INFO_MASK_ALL              SYS_LTE_MEAS_INFO_BIT_MASK

/** @} */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** @brief LTE measurement info.
*/

/**< The number of cells to be reported per frequency, or in the
     case of GSM, number of freqs in a frequency group */
#define SYS_LTE_NGBR_IND_NUM_CELLS      8

/**< The number of frequencies to report per rat
     case of GSM, number of freqs in a frequency group */
#define SYS_LTE_NGBR_IND_NUM_FREQS      2

#define SYS_WCDMA_MAX_CELLS_REPORTED    8
#define SYS_WCDMA_SRCH_INTER_FREQ_MAX   2
#define SYS_WCDMA_SRCH_LTE_EARFCN_MAX   4


/** @brief LTE cell param info. */
typedef struct sys_lte_ngbr_ind_l_cell_s
{
  uint16      pci;
  /**< Physical cell ID */

  int16       rsrq;
  /**< The current Reference Signal Receive Quality as measured by L1.
       The quantities are in dB.  Range: -20.0 ..-3.0 dB. x10 */

  int16       rsrp;
  /**< The current Reference Signal Receive Power in dBm x10 as measured by L1.
       Range: -44..-140 */

  int16       rssi;
  /**< The current Received Signal Strength Indication as measured by L1.
       Values are in dBm x10 with Range: -120.0 .. 0  */

  struct
  {
    int16     srxlev;
  /**< Srxlev */

  } idle;

} sys_lte_ngbr_ind_l_cell_s_type;

/**< @brief LTE intra configuration parameters
*/
typedef struct sys_lte_ngbr_ind_intra_freq_idle_param_s
{
  uint8     cell_resel_priority;
  /**< Priority for serving freq */

  uint8     s_non_intra_search;
  /**< S non intra search threshold to control non-intra freq searches */

  uint8     thresh_serving_low;
  /**< threshServingLow */

  uint8     s_intra_search;
  /**< Threshold current cell meas must fall below to consider
       intra freq for reselection. */

} sys_lte_ngbr_ind_intra_freq_idle_param_s_type;

/** @brief LTE neighbor inter freq. idle info. */
typedef struct sys_lte_ngbr_ind_inter_freq_idle_param_s
{
  uint8      threshX_low;
  /**< Srxlev-value of an evaluated cell must be greater than these values to
       be considered for reselection */

  uint8      threshX_high;
  /**< Srxlev-value of an evaluated cell must be lesser than these values to
       be considered for reselection */

  uint8      cell_resel_priority;
  /**< Cell reselection priority */

} sys_lte_ngbr_ind_inter_freq_idle_param_s_type;

/** @brief LTE neighbor intra freq. info. */
typedef struct sys_lte_ngbr_ind_intra_freq_param_s
{
  uint32      earfcn;
  /**< Earfcn of serving cell*/

  uint16      serving_cell_id;
  /**< LTE Serving cell id */

  uint8       num_lte_cells;
  /**< number of LTE Serving cells */

  sys_lte_ngbr_ind_l_cell_s_type
                    cell[SYS_LTE_NGBR_IND_NUM_CELLS];
  /**< The lte cell paramters */

  sys_lte_ngbr_ind_intra_freq_idle_param_s_type   idle;
  /**< Idle state parameters. */

} sys_lte_ngbr_ind_intra_freq_param_s_type;
/*~ FIELD sys_lte_ngbr_ind_intra_freq_param_s.cell
    VARRAY SYS_LTE_NGBR_IND_NUM_CELLS LENGTH sys_lte_ngbr_ind_intra_freq_param_s.num_lte_cells */


/** @brief LTE inter freq neighbor info. */
typedef struct sys_lte_ngbr_ind_inter_freq_layer_param_s
{
  uint32      earfcn;
  /**< Earfcn of serving cell*/

  uint8       num_lte_cells;
  /**< The number of LTE cells */

  sys_lte_ngbr_ind_l_cell_s_type
                     cell[SYS_LTE_NGBR_IND_NUM_CELLS];
  /**< The lte cell paramters */

  sys_lte_ngbr_ind_inter_freq_idle_param_s_type   idle;
  /**< Per-freq idle param */

} sys_lte_ngbr_ind_inter_freq_layer_param_s_type;
/*~ FIELD sys_lte_ngbr_ind_inter_freq_layer_param_s.cell
    VARRAY SYS_LTE_NGBR_IND_NUM_CELLS LENGTH sys_lte_ngbr_ind_inter_freq_layer_param_s.num_lte_cells */


/** @brief
   Inter frequency parameters for the neighbor power indication
*/
typedef struct sys_lte_ngbr_ind_inter_freq_param_s
{
  uint8          num_freqs;
  /**< Number of freq. */

  sys_lte_ngbr_ind_inter_freq_layer_param_s_type
                  freqs[SYS_LTE_NGBR_IND_NUM_FREQS];
  /**< Frequency info */

} sys_lte_ngbr_ind_inter_freq_param_s_type;
/*~ FIELD sys_lte_ngbr_ind_inter_freq_param_s.freqs
    VARRAY SYS_LTE_NGBR_IND_NUM_FREQS LENGTH sys_lte_ngbr_ind_inter_freq_param_s.num_freqs */


/** @brief GSM neighbor ind info */
typedef struct sys_lte_ngbr_ind_g_cell_s
{
  uint16      arfcn;
  /**< GSM frequency being reported */

  boolean     band_1900;
  /**< Band indicator for the GSM ARFCN, only valid if arfcn is in the
       overlapping region. If TRUE and the cell is in the overlapping region
       it is on the 1900 band, if FALSE it is on the 1800 band */

  boolean     cell_id_valid;
  /**< Flag indicating whether the BSIC ID is valid */

  uint8       bsic_id;
  /**< BSIC ID */

  int16       rssi;
  /**< Measured RSSI value in Db x10 */

  struct
  {
    int16     srxlev;
  /**< Srxlev */
  } idle;

} sys_lte_ngbr_ind_g_cell_s_type;

/*! @brief GERAN frequency group.

  This defines a group of GERAN frequencies with associated priority and
  reselection parameters.  This GERAN frequency group is analogous to
  a frequency in another RAT.

  Defined in 36.331 Section 6.4
  */
typedef struct
{
  uint8     cell_resel_priority;
  /**< The priority of this frequency group */

  uint8     thresh_gsm_high;
  /**< Reselection threshold for high priority layers.
    */

  uint8     thresh_gsm_low;
  /**< Reselection threshold for low priority layers.
    */

  uint8     ncc_permitted;
  /**<  Bitmask specifying whether a neighbour with a particular
    Network Color Code is to be reported.  Bit "n" set to 1 means
    that a neighbour with NCC "n" should be included in the report.
    This flag is synonymous with a blacklist in other RATs
   */

} sys_lte_ngbr_ind_irat_gsm_idle_freq_group_s_type;

/** @brief Per frequency group GSM parameters */
typedef struct sys_lte_ngbr_ind_gsm_freq_group_info_s
{
  uint8      num_gsm_arfcn;
  /**< the number of gsm arfcns indicated and the
       size of the gsm array */

  sys_lte_ngbr_ind_g_cell_s_type   arfcn[SYS_LTE_NGBR_IND_NUM_CELLS];
  /**< The gsm cell paramters */

  sys_lte_ngbr_ind_irat_gsm_idle_freq_group_s_type  idle;
  /**< Per-group idle parameters */

} sys_lte_ngbr_ind_gsm_freq_group_info_s_type;
/*~ FIELD sys_lte_ngbr_ind_gsm_freq_group_info_s.arfcn
    VARRAY SYS_LTE_NGBR_IND_NUM_CELLS LENGTH sys_lte_ngbr_ind_gsm_freq_group_info_s.num_gsm_arfcn */


/** @brief LTE GSM neighbor info. */
typedef struct sys_lte_ngbr_ind_gsm_info_s
{
  uint8          num_freq_groups;
  /**< The number of GSM frequency groups and the size of the freq_group
       array */

  sys_lte_ngbr_ind_gsm_freq_group_info_s_type
                 freq_group[SYS_LTE_NGBR_IND_NUM_FREQS];
  /**< GSM freq group info. */

} sys_lte_ngbr_ind_gsm_info_s_type;
/*~ FIELD sys_lte_ngbr_ind_gsm_info_s.freq_group
    VARRAY SYS_LTE_NGBR_IND_NUM_FREQS LENGTH sys_lte_ngbr_ind_gsm_info_s.num_freq_groups */


/** @brief LTE WCDMA neighbor cell info. */
typedef struct sys_lte_ngbr_ind_w_cell_s
{
  uint16               psc;
  /**< Scrambling code*/

  int16                cpich_rscp;
  /**< This is the absolute power level of the CPICH as received by the
       UEin dBm x10   Defined in 25.304*/

  int16                cpich_ecno;
  /**< CPICH Ec/No is the ratio of the received energy per PN chip
       for the CPICH to the total received power spectral density at
       the UE antenna connector. in dB x10 .Defined in 25.304 */

  struct
  {
    int16              srxlev;
  /**< Srxlev */
  } idle;

} sys_lte_ngbr_ind_w_cell_s_type;

/** @brief
    Idle mode inter-rat per WCDMA frequency information
*/
typedef struct sys_lte_ngbr_ind_wcdma_freq_idle_info_s
{
  uint8     cell_resel_priority;
  /**< Cell reselection priority 0..7 */

  uint16    thresh_Xhigh;
  /**< Reselection threshold. Upper limit */

  uint16    thresh_Xlow;
  /**< Reselection threshold. Lower limit */

}sys_lte_ngbr_ind_wcdma_freq_idle_info_s_type;

/** @brief WCDMA Frequency */
typedef struct sys_lte_ngbr_ind_wcdma_freq_info_s
{
  uint16         uarfcn;
  /**< wcdma Layer frequency */

  uint8          num_wcdma_cells;
  /**< The number of wcdma cells indicated and the
    size of the wcdma array */

  sys_lte_ngbr_ind_w_cell_s_type
                cell[SYS_LTE_NGBR_IND_NUM_CELLS];
  /**< The wcdma cell paramters */

  sys_lte_ngbr_ind_wcdma_freq_idle_info_s_type  idle;
  /**< The wcdma cell freq idle paramters */

} sys_lte_ngbr_ind_wcdma_freq_info_s_type;
/*~ FIELD sys_lte_ngbr_ind_wcdma_freq_info_s.cell
    VARRAY SYS_LTE_NGBR_IND_NUM_CELLS LENGTH sys_lte_ngbr_ind_wcdma_freq_info_s.num_wcdma_cells */


/** @brief WCDMA Parameters */
typedef struct sys_lte_ngbr_ind_wcdma_info_s
{
  uint8      num_wcdma_freqs;
  /**< The number of WCDMA frequencies, and the size of the freq array */

  sys_lte_ngbr_ind_wcdma_freq_info_s_type   freq [SYS_LTE_NGBR_IND_NUM_FREQS];
  /**< WCDMA neighbor cell freq info. */

} sys_lte_ngbr_ind_wcdma_info_s_type;
/*~ FIELD sys_lte_ngbr_ind_wcdma_info_s.freq
    VARRAY SYS_LTE_NGBR_IND_NUM_FREQS LENGTH sys_lte_ngbr_ind_wcdma_info_s.num_wcdma_freqs */


/** @brief
  This message indicates to upper layers the current signal quality of
  neighbor cells as reported by LTE L1.

  Due to the dynamic nature of the a cellular environment there will
  be times where no serving or neigbor info is available (such as in
  RLF/OOS or certain types of handover.

  No aggregation is done at an L1 level, and this is considered a current
  snapshot of the L1 conditions.  It is possible to use previous snapshots
  for a period of 'n' seconds while valid is not TRUE so that sudden jumps
  of this type are not presented to the UI.
*/
typedef struct sys_lte_ngbr_ind_s
{
  boolean     valid;
  /**< When FALSE none of the fields in the structure are valid */

  boolean     idle;
  /**< Flag to indicate whether the UE is in idle mode.
       If TRUE the UE is in idle mode all idle sub-structs
       will be considered valid. If FALSE all idle mode sub-structs
       should be considered invalid */

  sys_lte_ngbr_ind_intra_freq_param_s_type   lte_intra;
  /**< LTE intra-frquency parameters */

  sys_lte_ngbr_ind_inter_freq_param_s_type   lte_inter;
  /**< LTE inter-frquency parameters */

  sys_lte_ngbr_ind_gsm_info_s_type           gsm;
  /**< GSM cell information */

  sys_lte_ngbr_ind_wcdma_info_s_type         wcdma;
  /**< WCDMA cell information */

} sys_lte_ngbr_ind_s_type;

typedef struct sys_lte_meas_info_s
{
  boolean                is_service_available;
  /**< flag to indicate if there is service. */

  sys_sys_mode_e_type    sys_mode;
  /**< system mode. Valid only is
       is_service_available is true. */

  sys_lte_ngbr_ind_s_type          lte_nbr_info;
  /**< LTE Signal and Nbr Information */

}sys_lte_meas_info_s_type;


/** @brief LTE statistics payload.
*/
typedef struct sys_lte_information_s
{

  uint32                    changed_bitmask;
  /**< Bitmask indicating which fields changed. */

  sys_lte_meas_info_s_type  lte_meas_info;
  /**< LTE RAT measurement info. */

}sys_lte_information_s_type;

/** Modem functional modules.
*/
typedef enum
{
  /** @cond
  */
  SYS_MODEM_MODULE_NONE = -1,   /* For CM internal use only */
  /** @endcond
  */

  SYS_MODEM_MODULE_NAS = 0,
    /**< NAS. */

  SYS_MODEM_MODULE_GSM = 1,
    /**< RR. */

  SYS_MODEM_MODULE_WCDMA = 2,
    /**< RRC. */

  SYS_MODEM_MODULE_CDMA  = 3,
    /**< CDMA - 1X. */

  SYS_MODEM_MODULE_HDR   = 4,
    /**< HDR. */

  SYS_MODEM_MODULE_CM    = 5,
    /**< CM/SD. */

  SYS_MODEM_MODULE_DS    = 6,
    /**< DS. */

  SYS_MODEM_MODULE_LTE   = 7,
    /**< LTE. */

  /** @cond
  */
  SYS_MODEM_MODULE_MAX   /* For CM internal use only */
  /** @endcond
  */

} sys_modem_module_e_type;
/*~ SENTINEL sys_modem_module_e_type.SYS_MODEM_MODULE_MAX */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** @brief Payload to be sent to clients.
*/

/**
** Enumeration of Active Subscription ID's
** Note: WMS APIs are a client of this enum
*/
typedef enum {

  SYS_MODEM_AS_ID_NONE = -1,
    /**< Internal Only */

  SYS_MODEM_AS_ID_1,
    /**< Subscription ID 1 */

  SYS_MODEM_AS_ID_2,
    /**< Subscription ID 2 */
  
  SYS_MODEM_AS_ID_3,
    /**< Subscription ID 3 */

  SYS_MODEM_AS_ID_NO_CHANGE,
    /**< No Change in Subscription ID */

  SYS_MODEM_AS_ID_MAX
    /**< Internal Only */

} sys_modem_as_id_e_type;
/*~ SENTINEL sys_modem_as_id_e_type.SYS_MODEM_AS_ID_MAX */

/**
** Bit mask of Active Subscription ID(s)
*/
#define SYS_MODEM_AS_ID_MASK_NONE         (0)

#define SYS_MODEM_AS_ID_MASK_1            SYS_BM(SYS_MODEM_AS_ID_1)

#define SYS_MODEM_AS_ID_MASK_2            SYS_BM(SYS_MODEM_AS_ID_2)

#define SYS_MODEM_AS_ID_MASK_3            SYS_BM(SYS_MODEM_AS_ID_3)

#define SYS_MODEM_AS_ID_MASK_ANY          (SYS_BM(SYS_MODEM_AS_ID_1)| \
                                           SYS_BM(SYS_MODEM_AS_ID_2)| \
                                           SYS_BM(SYS_MODEM_AS_ID_3))


/**
** Enumeration of Active Subscription ID's
** Note: WMS APIs are a client of this enum
*/
typedef enum {

  SYS_MODEM_STACK_ID_NONE = -1,
    /**< Internal Only */

  SYS_MODEM_STACK_ID_1,
    /**< Stack ID 1 */

  SYS_MODEM_STACK_ID_2,
    /**< Stack ID 2 */

  SYS_MODEM_STACK_ID_MAX
    /**< Internal Only */

} sys_modem_stack_id_e_type;
/*~ SENTINEL sys_modem_as_id_e_type.SYS_MODEM_AS_ID_MAX */


/**
** Macro to map inst_id in msg_hdr to Subscription ID.
** Assume protocol layer will not fill in inst_id value equal or bigger than
** SYS_MODEM_AS_ID_NO_CHANGE.
*/
#define INST_ID_TO_SYS_AS_ID(id) \
  ( (id)>0 && (id)<=(uint8)MAX_AS_IDS? (sys_modem_as_id_e_type)((id)-1) : SYS_MODEM_AS_ID_1)


/**
** Macro to map Subscription ID to inst_id in msg_hdr.
** Assume protocol layer will not fill in AS_ID equal or bigger than
** SYS_MODEM_AS_ID_NO_CHANGE.
*/
#define SYS_AS_ID_TO_INST_ID(as_id) \
  ( (as_id)>=SYS_MODEM_AS_ID_1 && (as_id)<MAX_AS_IDS ? (uint8)((as_id)+1) : 1)


typedef struct sys_modem_stats_info_s
{
  sys_modem_module_e_type type;   /**< Module responsible for generating a
                       report. */

  sys_modem_as_id_e_type   asubs_id;  /*Active Subscription*/

  union
  {
    sys_nas_information_s_type    nas_info;   /**< NAS structure. */
    sys_gsm_information_s_type    gsm_info;   /**< GSM structure. */
    sys_wcdma_information_s_type  wcdma_info; /**< RRC structure. */
    sys_cdma_information_s_type   cdma_info;  /**< CDMA 1X structure. */
    sys_hdr_information_s_type    hdr_info;   /**< HDR structure. */
    sys_cm_information_s_type     cm_info;    /**< CM/SD structure. */
    sys_ds_information_s_type     ds_info;    /**< DS structure. */
    sys_lte_information_s_type    lte_info;   /**< LTE structure. */
  }prm;

} sys_modem_stats_info_s_type;
/*~ FIELD sys_modem_stats_info_s.prm DISC sys_modem_stats_info_s.type */
/*~ CASE SYS_MODEM_MODULE_NAS sys_modem_stats_info_s.prm.nas_info */
/*~ CASE SYS_MODEM_MODULE_GSM sys_modem_stats_info_s.prm.gsm_info */
/*~ CASE SYS_MODEM_MODULE_WCDMA sys_modem_stats_info_s.prm.wcdma_info */
/*~ CASE SYS_MODEM_MODULE_CDMA  sys_modem_stats_info_s.prm.cdma_info */
/*~ CASE SYS_MODEM_MODULE_HDR   sys_modem_stats_info_s.prm.hdr_info */
/*~ CASE SYS_MODEM_MODULE_CM    sys_modem_stats_info_s.prm.cm_info */
/*~ CASE SYS_MODEM_MODULE_DS    sys_modem_stats_info_s.prm.ds_info */
/*~ CASE SYS_MODEM_MODULE_LTE   sys_modem_stats_info_s.prm.lte_info */
/*~ DEFAULT sys_modem_stats_info_s.prm.void */

/** @} */ /* end_addtogroup SI Service Indicators */

/** WLAN security information **/

typedef enum sys_wlan_sec_param_e
{
  SYS_WLAN_SEC_PARAM_NONE     = -1,
  /* For CM internal use only */
  SYS_WLAN_SEC_PARAM_ACQ_FAIL = 0,
  /* ACQ_FAIL */
  SYS_WLAN_SEC_PARAM_SECURITY_INFO = 1,
  /* SECURITY INFO */
  SYS_WLAN_SEC_PARAM_MAX
  /* For CM internal use only */
}sys_wlan_sec_param_e_type;

typedef enum sys_wlan_acquire_fail_reason_e
{
  SYS_WLAN_ACQ_FAIL_REASON_NONE = 0,
  /* NONE */
  SYS_WLAN_ACQ_FAIL_REASON_GENERIC_FAILURE = 1,
  /* GENERIC_FAILURE */
  SYS_WLAN_ACQ_FAIL_REASON_AUTH_REJECTED = 2,
  /* AUTH_REJECTED */
  SYS_WLAN_ACQ_FAIL_REASON_UNREACHABLE = 3,
  /* UNREACHABLE */
  SYS_WLAN_ACQ_FAIL_REASON_SEC_FAILED = 4,
  /* SEC_FAILED */
  SYS_WLAN_ACQ_FAIL_REASON_MAX
  /* For CM internal use only */
}sys_wlan_acquire_fail_reason_e_type;

/** Payload to be sent to clients **/
typedef struct sys_wlan_sec_params_s
{
  sys_wlan_sec_param_e_type    type;

  union
  {
    sys_wlan_acquire_fail_reason_e_type   err_cause;
    /* Connection fail error cause */
    sys_wlan_security_info_s_type         sec_info;
    /* Security info */
  }cmd;

}sys_wlan_sec_params_s_type;
/*~ FIELD sys_wlan_sec_params_s.cmd DISC sys_wlan_sec_params_s.type */
/*~ CASE SYS_WLAN_SEC_PARAM_ACQ_FAIL     sys_wlan_sec_params_s.cmd.err_cause */
/*~ CASE SYS_WLAN_SEC_PARAM_SECURITY_INFO sys_wlan_sec_params_s.cmd.sec_info */
/*~ DEFAULT sys_wlan_sec_params_s.cmd.void */


#if defined(FEATURE_TRIPLE_SIM)
#define MAX_AS_IDS 3
//#elif defined(FEATURE_DUAL_SIM) || defined(FEATURE_GW_G_DUAL_STANDBY)
/* NUMBER OF Active Subscription IDs to work in hybrid manner */
//#define MAX_AS_IDS 2
#else
#define MAX_AS_IDS 2
#endif

/** Enumeration of Dual Standby Preferences
*/
typedef enum {

  SYS_MODEM_DS_PREF_NONE,
    /**< Internal use, range checking */

  SYS_MODEM_DS_PREF_SINGLE_STANDBY,
    /**<
    ** Phone is in Single Standby mode
    */

  SYS_MODEM_DS_PREF_DUAL_STANDBY,
    /**<
    ** Phone is in Dual Standby mode
    */

  SYS_MODEM_DS_PREF_NO_CHANGE,
    /**<
    ** Do not change the Dual Standby preference.
    ** should be mapped to the existing value.
    ** Only temporary value and should never be written to NV.
    */

  SYS_MODEM_DS_PREF_DUAL_STANDBY_NO_TUNEAWAY,
    /**<
    ** Phone is in Dual Standby mode with no tune away.
    */

  SYS_MODEM_DS_PREF_AUTO,
    /**<
    ** Standby mode is determined by the number of subscriptions
    */

  SYS_MODEM_DS_PREF_AUTO_NO_TUNEAWAY,
  /**<
  ** Standby mode is determined by the number of subscriptions.
  ** With no tuneaway.
  */

  SYS_MODEM_DS_PREF_TRIPLE_STANDBY,
     /* TSTS mode */

  SYS_MODEM_DS_PREF_TRIPLE_STANDBY_NO_TUNEAWAY,

  SYS_MODEM_DS_PREF_NO_STANDBY_PREF,
  /**<
  **None of th stacks will be active
  */

  SYS_MODEM_DS_PREF_MAX
    /**< Internal use for range checking
    ** should never be written to NV.
    */

} sys_modem_dual_standby_pref_e_type;
/*~ SENTINEL sys_modem_dual_standby_pref_e_type.SYS_MODEM_DS_PREF_MAX */

/** Enumeration of device mode
*/
typedef enum {

  SYS_MODEM_DEVICE_MODE_NONE = -1,
  /**< Internal use, range checking */

  SYS_MODEM_DEVICE_MODE_SINGLE_SIM,
  /**< Device is single sim */

  SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_STANDBY,
  /**< Device is dual sim dual standy */

  SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_ACTIVE,
  /**< Device is dual sim dual active */

  SYS_MODEM_DEVICE_MODE_TRIPLE_SIM_TRIPLE_STANDBY,
  /**< Device is Triple sim Triple standy */

  SYS_MODEM_DEVICE_MODE_MAX
  /**< Internal use for range checking
   ** should never be written to NV.
   */

} sys_modem_device_mode_e_type;

/** @brief Cell Broadcast information
*/
typedef struct sys_cb_info_s
{
  sys_modem_as_id_e_type   subscription_id;
    /**< subscription id. */

  sys_cell_broadcast_cap_e_type   cell_bc_capability;
    /**< Cell broadcast capability. */

}sys_cell_broadcast_info_s_type;

/** @brief Division Duplex information
*/
typedef enum {
   SYS_DIV_DUPLEX_UNKNOWN = -1,
   /**< internal use only when div duplex not received */
   SYS_DIV_DUPLEX_NONE,
   /**< None */

   SYS_DIV_DUPLEX_FREQ,
   /**< FDD- Frequency Division Duplex */

   SYS_DIV_DUPLEX_TIME,
   /**< TDD - Time Division Duplex */

} sys_div_duplex_e_type;

typedef enum {
   SYS_EMBMS_COVERAGE_STATUS_INVALID = -1,
   /**< eMBMS Coverage Status INVALID. 
   ** Returned when invalid value received */

   SYS_EMBMS_COVERAGE_STATUS_NOT_AVAILABLE,
   /**< eMBMS Coverage Not Available */

   SYS_EMBMS_COVERAGE_STATUS_AVAILABLE,
   /**< eMBMS Coverage Available */

   SYS_EMBMS_COVERAGE_STATUS_UNKNOWN,
   /**< eMBMS Coverage Status Unknown */

   SYS_EMBMS_COVERAGE_STATUS_NOT_AVAIL_DUE_TO_UEMODE, 
   /**< eMBMS Coverage Status not available for modes 
    **  like DSDS/DSDA/1xSRLTE */
   
   SYS_EMBMS_COVERAGE_STATUS_NOT_AVAIL_DUE_TO_E911, 
   /**< eMBMS Coverage Status not available due to 
    **  ongoing E911 emergency mode */
   
   SYS_EMBMS_COVERAGE_STATUS_MAX
   /**< eMBMS Coverage Status INVALID. 
   ** Returned when invalid value received */
}sys_embms_coverage_status_e_type;

typedef struct sys_embms_info_s
{
  sys_embms_coverage_status_e_type     embms_coverage_status;
    /** < Indicate whether eMBMS is supported in current serving system.
    **  The value be Available, Unavailable or Unknown. Pass thorugh
    **  from CM. This field is valid only if sys_mode or gw_sys_mode
    **  is set to LTE
    */

  int16                                trace_id;
    /** < Indicate debug trace id, range 0 to 32767, -1 if not used
    ** CM will pass the debug_trace_id delievered from LTE RRC */

}sys_embms_info_s_type;



/** @brief Speech codec types
*/
typedef enum {
   SYS_SPEECH_CODEC_NONE = 0x0000,
   SYS_SPEECH_CODEC_QCELP13K,
   SYS_SPEECH_CODEC_EVRC,
   SYS_SPEECH_CODEC_EVRC_B,
   SYS_SPEECH_CODEC_EVRC_WB,
   SYS_SPEECH_CODEC_EVRC_NW,
   SYS_SPEECH_CODEC_AMR_NB,
   SYS_SPEECH_CODEC_AMR_WB,
   SYS_SPEECH_CODEC_GSM_EFR,
   SYS_SPEECH_CODEC_GSM_FR,
   SYS_SPEECH_CODEC_GSM_HR,   
   SYS_SPEECH_CODEC_G711U,
   SYS_SPEECH_CODEC_G723,	
   SYS_SPEECH_CODEC_G711A,	
   SYS_SPEECH_CODEC_G722,	  
   SYS_SPEECH_CODEC_G711AB, 
   SYS_SPEECH_CODEC_G729,	 
   SYS_SPEECH_CODEC_EVS_NB,
   SYS_SPEECH_CODEC_EVS_WB,
   SYS_SPEECH_CODEC_EVS_SWB,
   SYS_SPEECH_CODEC_EVS_FB,
   SYS_SPEECH_CODEC_MAX = 0x7FFF
} sys_speech_codec_e_type;

/** @brief Speech codec types
*/
typedef enum {
   SYS_NETWORK_TYPE_NONE = 0,
   SYS_NETWORK_TYPE_GSM,
   SYS_NETWORK_TYPE_WCDMA,
   SYS_NETWORK_TYPE_CDMA,
   SYS_NETWORK_TYPE_LTE,
   SYS_NETWORK_TYPE_TD,
   SYS_NETWORK_TYPE_WLAN
} sys_network_mode_e_type;

/** ENUM of Apps using audio hardware
*/
typedef uint32 sys_vsid_type;

typedef enum sys_vsid_app_e{

SYS_VSID_APP_NONE = -1,

SYS_VSID_APP_CS_VOICE = 0,

SYS_VSID_APP_IMS_LTE = 1,

SYS_VSID_APP_IMS_WLAN = 2,

SYS_VSID_APP_MAX,

}cmph_vsid_app_e_type;


/* structure to hold audio codec info for apps Audio
   management and UI indication
*/
typedef struct
{
  sys_network_mode_e_type  network_mode;  /*either 1X/GSM/WCDMA/LTE/TDSCDMA */

  /* Speech codec type*/
  sys_speech_codec_e_type  speech_codec_type;

  /* Speech sample rate*/
  uint32                   speech_enc_samp_freq; /* 8000: narrow-band or 16000: wide-band, 0 - unknown: ignore */

}sys_speech_info_s_type;


/* supplementary service domain preference */
typedef enum
{
  SYS_SUPP_SRV_DOMAIN_PREF_NONE   = -1,  /* FOR INTERNAL USE ONLY */
  SYS_SUPP_SRV_DOMAIN_PREF_AUTO,  /* use voice domain pref result */
  SYS_SUPP_SRV_DOMAIN_PREF_CS_ONLY , /* use CS domain for supplement service otherwise reject*/
  SYS_SUPP_SRV_DOMAIN_PREF_PS_ONLY , /* use PS domain for supplement service otherwise reject */
  SYS_SUPP_SRV_DOMAIN_PREF_PS_PREF     /* The Supplementary service is expected  to be  routed to  PS
                                       domain if ICS interface is supported (or UT/SIP interface is available).
                                       Otherwise, send it to CS. */
} sys_supp_srv_domain_pref_e_type;
#define SYS_MODEM_AS_ID_1_MASK    BM(SYS_MODEM_AS_ID_1)

#define SYS_MODEM_AS_ID_2_MASK    BM(SYS_MODEM_AS_ID_2)

#define SYS_MODEM_AS_ID_3_MASK    BM(SYS_MODEM_AS_ID_3)

#define SYS_MODEM_AS_ID_ALL_ACTIVE_MASK    (BM(SYS_MODEM_AS_ID_1)|BM(SYS_MODEM_AS_ID_2)|BM(SYS_MODEM_AS_ID_3))

#define SYS_MODEM_AS_ID_NO_ACTIVE_MASK    (uint8)0

#define SYS_MODEM_AS_ID_NO_CHANGE_MASK    (uint8)0xFF

typedef enum
{
  SYS_UE_MODE_NONE = -1,
  /* Ue Mode None - INTERNAL USE */

  SYS_UE_MODE_NORMAL,
  /* NORMAL is UE Mode used when UE is in legacy behavior.
   */

  SYS_UE_MODE_SGLTE,
  /* SGLTE: mode for simultaneous voice and data. UE can function in either
   * single stack or dual-stack.
   */

  SYS_UE_MODE_CSFB,
  /* CSFB: mode for circuit-switched fallback.
   */

  SYS_UE_MODE_CSFB_ONLY,
  /* CSFB_ONLY: This is CSFB mode, and UE is not allowed to move to SV_DATA.
   */

  SYS_UE_MODE_SGLTE_ONLY,
  /* SGLTE_ONLY: this is used only for testing. It is SGLTE mode and UE
   * will not move out of this mode.
   */

  SYS_UE_MODE_SVLTE,
  /* SVLTE: mode for simultaneous voice and data. UE can function in either
   * single stack or dual-stack.
   */

  SYS_UE_MODE_SVLTE_ONLY,
  /* SVLTE_ONLY: this is used only for testing. It is SVLTE mode and UE
   * will not move out of this mode.
   */

  SYS_UE_MODE_1XSRLTE_CSFB_PREFERRED,
  /*  SYS_UE_MODE_1XSRLTE_CSFB_PREFERRED: Attempt to connect as CSFB, but fallback
   *  to 1XSRLTE if CSFB is not supported on the network.
   */

  SYS_UE_MODE_GSMSRLTE_CSFB_PREFERRED,
  /*  SYS_UE_MODE_GSMSRLTE_CSFB_PREFERRED: Attempt to connect as CSFB, but fallback
   *  to GSMSRLTE if CSFB is not supported on the network.
   */

  SYS_UE_MODE_GSMSRLTE_ONLY,
  /* GSMSRLTE_ONLY: this is used only for testing. It is GSMSRLTE mode and UE
   * will not move out of this mode.
   */

  SYS_UE_MODE_1XSRLTE_ONLY
  /* 1XSRLTE_ONLY: this is used only for testing. It is 1XSRLTE mode and UE
   * will not move out of this mode.
   */
} sys_ue_mode_e_type;


/*  Alternate names for SYS_UE_MODE_**** for backward compatibility (the
 *  UE modes used to be exclusivly SGLTE.
 */
#define SYS_UE_MODE_SGLTE_HOME      SYS_UE_MODE_SGLTE
#define SYS_UE_MODE_SGLTE_ROAM      SYS_UE_MODE_CSFB
#define SYS_UE_MODE_SGLTE_TEST_ONLY SYS_UE_MODE_SGLTE_ONLY



typedef enum {

  SYS_VOICE_HO_NONE = 0,

  SYS_VOICE_HO_G_2_G, /* Handover from G to G*/

  SYS_VOICE_HO_G_2_W, /* Handover from G to W  */

  SYS_VOICE_HO_W_2_W, /* Handover from W to W  */

  SYS_VOICE_HO_W_2_G, /* Handover from W to G */

  SYS_VOICE_HO_SRVCC_L_2_G, /* Handover from L to G due to SRVCC */

  SYS_VOICE_HO_SRVCC_L_2_W, /* Handover from L to W due to SRVCC */

  SYS_VOICE_HANDOVER_1XCSFB, /* Handover from L to 1X due to 1XCSFB */

  SYS_VOICE_HANDOVER_GWCSFB, /*Handover from L to GW due to 3GPPCSFB */ 

  SYS_VOICE_HO_SRVCC_L_2_C, /* Handover from L to 1X due to 1XSRVCC */

  SYS_VOICE_HO_DRVCC_WIFI_2_C, /* Handover from Wifi to Cdma
                                                      either native 1x or 1xcsfb for drvcc */

  SYS_VOICE_HO_DRVCC_WIFI_2_GW /* Handover from Wifi to G/W
                                                        either native G/W or ppcsfb for drvcc */

}sys_voice_handover_e_type;

typedef enum {
  SYS_DRVCC_NONE = 0,
  SYS_DRVCC_NATIVE_ONEX, /* DRVCC to 1X */
  SYS_DRVCC_ONEXCSFB,    /* DRVCC to 1XCSFB */
  SYS_DRVCC_3GPP,        /* DRVCC to G/W */
  SYS_DRVCC_PPCSFB,     /* DRVCC to 3GPPCSFB */
  SYS_DRVCC_MAX         
} sys_drvcc_type_e_type;

/*  Subscription-level feature specification.
 */
typedef enum
{
  SYS_SUBS_FEATURE_MODE_NORMAL,     /* subs configured for normal mode */
  SYS_SUBS_FEATURE_MODE_SVLTE,      /* subs configured for SVLTE */
  SYS_SUBS_FEATURE_MODE_SGLTE,      /* subs configured for SGLTE */
  SYS_SUBS_FEATURE_MODE_SRLTE,      /* subs configured for SRLTE */
  SYS_SUBS_FEATURE_MODE_DUAL_MULTIMODE
} sys_subs_feature_t;



/*  Overall device-level feature specification.  This is an enum that combines
    all the information contained in subscription-level features into a
    single enum for components that are not designed to handle subscription-
    level information.
 */
typedef enum
{
  SYS_OVERALL_FEATURE_MODE_NORMAL,    /* phone configured for normal mode */
  SYS_OVERALL_FEATURE_MODE_MULTISIM,  /* phone configured for multi-sim mode */
  SYS_OVERALL_FEATURE_MODE_SVLTE,     /* phone configured for SVLTE */
  SYS_OVERALL_FEATURE_MODE_SGLTE,     /* phone configured for SGLTE */
  SYS_OVERALL_FEATURE_MODE_SRLTE,     /* phone configured for SRLTE */
} sys_overall_feature_t;

typedef enum {

  SUBS_CAPABILITY_NONE = -1,

  SUBS_CAPABILITY_NO_CHANGE, /* For MMODE internal use incase there is no change in subs cap*/
  
  SUBS_CAPABILITY_GSM_ONLY,
  
  SUBS_CAPABILITY_MULTIMODE,

  SUBS_CAPABILITY_MAX

} subs_capability_e_type;


/*  "Specializations" of modem.  These indicate variations in how modem operates
 *  that may not be standards-compliant.  The specialization required for a
 *  given device is specified in the 'specialization' attribute of policyman's
 *  device_configuration XML element.  The name which maps to each specialization
 *  is given in the comments below. SYS_SPECIAL_NONE is the default if nothing
 *  is specified.
 */
typedef enum
{
  SYS_SPECIAL_NONE                    = 0,    /* none */
  SYS_SPECIAL_SBM_RAT_SEARCH_ORDER    = 1,    /* sbm_rat_search_order */
  SYS_SPECIAL_SLTE                    = 2,    /* SLTE subfeature of SVLTE */
  SYS_SPECIAL_DSDA_ROAM               = 3,    /* For WCDMA; device will always be in DSDA */
  SYS_SPECIAL_DSDS_ROAM               = 4,    /* For WCDMA; device is DSDA in home, DSDS roaming */
  SYS_SPECIAL_SBM_PERIODIC_SEARCH     = 5,    /* SBM periodic search */  
  SYS_SPECIAL_VOLTE_E911_CALLS        = 6,    /* Special handling required for VOLTE E911 calls */
} sys_specialization_t;


/*  Modes of operation for E911 calls.
 */
typedef enum
{
  SYS_CALL_MODE_INVALID = -1,
  SYS_CALL_MODE_NORMAL,
  SYS_CALL_MODE_VOLTE_ONLY
} sys_call_mode_t;

/*! @brief Enumeration for establishment call type
*/
typedef enum
{
  SYS_LTE_REESTAB_CALLTYPE_NORMAL, /* Normal */
  SYS_LTE_REESTAB_CALLTYPE_VOLTE, /* Volte */
  SYS_LTE_REESTAB_CALLTYPE_VT, /* Video */
  SYS_LTE_REESTAB_CALLTYPE_SMS_OVER_IP, /* SMS over IP */
  SYS_LTE_REESTAB_CALLTYPE_SMS /* SMS over NAS, SMS over S102*/
} sys_lte_reestab_calltype_e;

/* Scan scope for Acquisition or BPLMN search */
typedef enum
{
  SYS_SCAN_SCOPE_NONE = -1,   /* FOR INTERNAL USE ONLY! */
  SYS_SCAN_SCOPE_FULL_BAND,
  SYS_SCAN_SCOPE_ACQ_DB,
  SYS_SCAN_SCOPE_MAX   /* FOR INTERNAL USE ONLY! */
} sys_scan_scope_e_type;


/*Enumeration for the type of Scans for different searches*/
typedef enum scan_scope_rule_e {

  SYS_SCAN_SCOPE_RULE_FULL_BAND = 0,
  /**< Default scan_scope type i.e FULL_BAND for all searches */  
  
  SYS_SCAN_SCOPE_RULE_ACQ_DB_BPLMN = BM(0),
  /**< ACQ_DB scan_scope type for BPLMN search */  

  SYS_SCAN_SCOPE_RULE_ACQ_DB_OOS = BM(1),
   /**< ACQ_DB scan_scope type for OOS search */  

  SYS_SCAN_SCOPE_RULE_ACQ_DB_RLF = BM(2),
   /**< ACQ_DB scan_scope type for RLF searches */  

  
  SYS_SCAN_SCOPE_RULE_ACQ_DB_PWR_UP = BM(3),
   /**< ACQ_DB scan_scope type for RLF searches */  

  SYS_SCAN_SCOPE_RULE_ACQ_DB_NO_TRM = BM(4),
   /**< ACQ_DB scan_scope when there is no TRM */  

/** @cond  */
  SYS_SCAN_SCOPE_RULE_MAX = BM(7)
/** @endcond */
  } sys_scan_scope_rule_e_type;

typedef enum
{

  SYS_CSG_SELECTION_CONFIG_NONE =  -1,  /*FOR INTERNAL USE ONLY!*/

  SYS_CSG_SELECTION_CONFIG_1,

  SYS_CSG_SELECTION_CONFIG_MAX          /*FOR INTERNAL USE ONLY!*/

}sys_csg_selection_config_e_type;


typedef enum
{
  /*FOR INTERNAL USE ONLY!*/
  SYS_AS_IRAT_NONE = 0,

  //default behavior for measurements
  SYS_AS_IRAT_ENABLED,

  //SXLTE type measurements
  SYS_AS_IRAT_SXLTE_ENABLED,

  //measurements disabled (Future use)
  SYS_AS_IRAT_DISABLED 
} sys_as_irat_status_e_type;


typedef struct
{
  /* Addtional action to MMOC */
  uint8    action;

  /* additional payload */
  sys_ue_mode_e_type ue_mode;

  /* to remember if we are operation in SRLTE mode or not */
  boolean    is_ue_mode_substate_srlte;  
}sys_addtl_action_s_type;

typedef struct
{
uint8 version; 
uint8 FirstPLMNTimer; /* this is the first timer value after which LRRC sends the partial list */
uint8 MorePLMNTimer; /*subsequent timer value for partial plmn list */
uint8 MaxManualPLMNTimer; /*Maximum timer value*/
uint8 reserved[12];
}sys_manual_search_timer_value_type;

typedef enum sys_block_plmn_cause_e
{
  SYS_BLOCK_PLMN_CAUSE_NONE = -1,
  SYS_BLOCK_PLMN_CAUSE_OTHERS,
  SYS_BLOCK_PLMN_CAUSE_VOICE_NOT_AVAILABLE,
  SYS_BLOCK_PLMN_CAUSE_MAX_REGISTRATION_FAILURE,
  SYS_BLOCK_PLMN_CAUSE_PRIORITY,
  SYS_BLOCK_PLMN_CAUSE_MAX
} sys_block_plmn_cause_e_type;

/*ENUM for defining the priority of the Data Call*/

typedef enum sys_data_priority_e_type 
{

SYS_DATA_PRIORITY_NULL = -1, /*FOR INTERNAL USE ONLY!*/
SYS_DATA_PRIORITY_LOW,
SYS_DATA_PRIORITY_MED, 
SYS_DATA_PRIORITY_HIGH,
SYS_DATA_PRIORITY_MAX
}sys_data_priority_e_type; /*FOR INTERNAL USE ONLY!*/ 

typedef enum
{
  SYS_DATA_ROAMING_NULL = -1, /*FOR INTERNAL USE ONLY!*/
  SYS_DATA_ROAMING_ON, 
  SYS_DATA_ROAMING_INTERNATIONAL_OFF ,
  SYS_DATA_ROAMING_OFF,
  SYS_DATA_ROAMING_MAX  /*FOR INTERNAL USE ONLY!*/ 
}sys_data_roaming_e_type; 

/* Enum for timer T3346 status 
*/
typedef enum 
{
 SYS_NAS_T3346_STATUS_UNKNOWN    = -1,
 /**< FOR INTERNAL USE ONLY */

 SYS_NAS_T3346_STATUS_START,
 /**< T3346 timer started */

 SYS_NAS_T3346_STATUS_STOP,
 /**< T3346 timer stopped */

 SYS_NAS_T3346_STATUS_EXPIRED,
 /**< T3346 timer expired */

 SYS_NAS_T3346_STATUS_MAX   
 /**< FOR INTERNAL USE ONLY */

} sys_nas_t3346_status_e_type;


typedef enum cm_dds_cause_e {
  DDS_CAUSE_NONE = -1, 
  /**< FOR INTERNAL CM USE ONLY! */
  
  DDS_CAUSE_PERMANENT,
  /**< DDS is triggered by user */
  
  DDS_CAUSE_TEMPORARY,
  /**< DDS is triggered by MMS or other temporary cause */
  
  DDS_CAUSE_MAX 
 /**< FOR INTERNAL USE ONLY */
}cm_dds_cause_e_type;

/* Enum for PSM status 
*/

typedef enum 
{
SYS_PSM_STATUS_NONE    = -1,
/**< FOR INTERNAL USE ONLY */

SYS_PSM_STATUS_READY,
/**< Ready to enter PSM */

SYS_PSM_STATUS_NOT_READY_NOT_IDLE,
/**< Not in ECM_IDLE connected state */

SYS_PSM_STATUS_NOT_READY_EMC_CALL,
/**< Not ready to enter PSM due to Emergency call */

SYS_PSM_STATUS_MAX   
 /**< FOR INTERNAL USE ONLY */

} sys_psm_status_e_type;

/** @brief Type to hold rat acquisition priority order
*/
typedef struct cm_acq_pri_order_pref_s{

  uint16                    num_rat;
    /**< This indicates num of rats in rac_acq_order_prefs[] */

  sys_sys_mode_e_type       acq_order[SYS_MAX_ACQ_PREF_RAT_LIST_NUM];
    /**< This indicates rat acquisition order preference in priority */

}cm_acq_pri_order_pref_s_type;

typedef struct sys_pci_scan_pci_info_s
{
  uint32 freq;
  uint16 cell_id;
  uint8 num_plmn;
  sys_plmn_id_s_type plmn[SYS_PCI_SCAN_MAX_NUM_PLMN];
  /*!< Cell identity (received in SIB1) */
  uint32 global_cell_id;
}sys_pci_scan_cell_info_s_type;

typedef struct sys_pci_scan_signal_info_s
{
  int16 rsrp; /* Combined RSRP */
  int16 rsrp_rx0; /* RX0 RSRP */
  int16 rsrp_rx1; /* RX1 RSRP */
  int16 rsrq; /* Combined RSRQ */
  int16 rsrq_rx0; /* RX0 RSRQ */
  int16 rsrq_rx1; /* RX1 RSRQ */  
}sys_pci_scan_signal_info_s_type;

typedef struct sys_pci_scan_plmn_list_s
{
  uint16 num_cell;
  sys_pci_scan_cell_info_s_type cell_info[SYS_PCI_SCAN_MAX_NUM_PCI];
  /* Signal info corresponds to the first PCI listed above if num_pci > 0, otherwise not valid */
  sys_pci_scan_signal_info_s_type signal_info;   
}sys_pci_scan_plmn_list_s_type;

typedef union sys_found_plmn_list_u
{
  sys_detailed_plmn_list_s_type  plmn_list;
  sys_pci_scan_plmn_list_s_type  pci_plmn_list;
} sys_found_plmn_list_u_type;

/**
  RPM FTAI list action.
*/
typedef enum
{
  /** @cond
  */
  SYS_RPM_FTAI_LIST_NONE = -1,   /* internal */
  /** @endcond
  */

  /* 0 */
  SYS_RPM_FTAI_LIST_CLEAR = 0,
  /* Clear the FTAI list */

  SYS_RPM_FTAI_LIST_READ  = 1,
  /* Read the FTAI list from EFS */

  /** @cond
  */
  SYS_RPM_FTAI_LIST_MAX          /* internal */
  /** @endcond
  */
}sys_rpm_ftai_list_action_e_type;

/**
  Call end event reasons. Specific to 3GPP
*/
typedef enum sys_call_end_event_e {

  /** @cond
  */
  SYS_CALL_END_EVENT_NONE = -1,   /* internal */
  /** @endcond
  */

  /* 0 */
  SYS_CALL_END_EVENT_NORMAL_CALL_END    = 0,
    /**< Inform that call is ended normally. */
  
  SYS_CALL_END_EVENT_RX_DISCONNECT      = 1,
    /**< Receive DISCONNECT message from network on CC of device and it is abnormally ended case. */
  
  SYS_CALL_END_EVENT_RX_RELEASE         = 2,
    /**< Receive RELEASE message from network on CC of device and it is abnormally ende case. */
  
  SYS_CALL_END_EVENT_HANDOVER_FAIL      = 3,
    /**< Call is ended due to same RAT or IRAT handover failure. */

  /* 4 */
  SYS_CALL_END_EVENT_RLF_OR_WEAK_SIGNAL = 4,
    /**< Call is ended due to Radio Link Failure. */
  
  SYS_CALL_END_EVENT_RX_RRC_RELEASE     = 5,
    /**< Receive RRC RELEASE message from device base station and it is abnormally ended case. */
  
  SYS_CALL_END_EVENT_TX_DISCONNECT      = 10,
    /**< Send DISCONNECT message from device network and it is abnormally ended case. */
  
  SYS_CALL_END_EVENT_TX_RELEASE         = 11,
    /**< Send RELEASE message to network and it is abnormally ended case. */
  
  SYS_CALL_END_EVENT_OTHERS             = 20,
    /**< Call is ended by other reason. */
  
  /** @cond
  */
  SYS_CALL_END_EVENT_MAX   /* internal */
  /** @endcond
  */

} sys_call_end_event_e_type;

/**
  Handover failure. Specific to 3GPP
*/
typedef enum sys_handover_failure_e {

  /** @cond
  */
  SYS_HANDOVER_FAILURE_NONE = -1,   /* internal */
  /** @endcond
  */
  
  /* 0 */
  SYS_HANDOVER_FAILURE_NO_HANDOVER_FAILURE = 0,
  SYS_HANDOVER_FAILURE_G_TO_G_FAILURE      = 1,
  SYS_HANDOVER_FAILURE_G_TO_W_FAILURE      = 2,
  SYS_HANDOVER_FAILURE_G_TO_L_FAILURE      = 3,

  /* 4 */
  SYS_HANDOVER_FAILURE_W_TO_G_FAILURE      = 4,
  SYS_HANDOVER_FAILURE_W_TO_W_FAILURE      = 5,
  SYS_HANDOVER_FAILURE_W_TO_L_FAILURE      = 6,
  SYS_HANDOVER_FAILURE_L_TO_G_FAILURE      = 7,

  /* 8 */
  SYS_HANDOVER_FAILURE_L_TO_W_FAILURE      = 8,
  SYS_HANDOVER_FAILURE_L_TO_L_FAILURE      = 9,
  SYS_HANDOVER_FAILURE_G_TO_T_FAILURE      = 10,
  SYS_HANDOVER_FAILURE_T_TO_G_FAILURE      = 11,

  /* 12 */
  SYS_HANDOVER_FAILURE_T_TO_T_FAILURE      = 12,
  SYS_HANDOVER_FAILURE_T_TO_L_FAILURE      = 13,
  SYS_HANDOVER_FAILURE_L_TO_T_FAILURE      = 14,

  /** @cond
  */
  SYS_HANDOVER_FAILURE_MAX   /* internal */
  /** @endcond
  */
  
} sys_handover_failure_e_type;

/**
  RLF or weak signal reasons. Specific to 3GPP
*/
typedef enum sys_rlf_weak_signal_e {

  /** @cond
  */
  SYS_RLF_WEAK_SIGNAL_NONE = -1,   /* internal */
  /** @endcond
  */

  /* 0 */
  SYS_RLF_WEAK_SIGNAL_RLF_TIMER_EXP         = 0,
  SYS_RLF_WEAK_SIGNAL_RANDOM_ACCESS_FAILURE = 1,
  SYS_RLF_WEAK_SIGNAL_L2_FAILURE            = 2,
  SYS_RLF_WEAK_SIGNAL_OTHER                 = 3,

  /** @cond
  */
  SYS_RLF_WEAK_SIGNAL_MAX   /* internal */
  /** @endcond
  */

} sys_rlf_weak_signal_e_type;

/**
  RAB status. Specific to 3GPP
*/
typedef enum sys_rab_status_e {

  /** @cond
  */
  SYS_RAB_STATUS_NONE = -1,   /* internal */
  /** @endcond
  */

  /* 0 */
  SYS_RAB_STATUS_NO_RAB    = 0,
  SYS_RAB_STATUS_CS_RAB    = 1,
  SYS_RAB_STATUS_PS_RAB    = 2,
  SYS_RAB_STATUS_CS_PS_RAB = 3,
  
  /** @cond
  */
  SYS_RAB_STATUS_MAX   /* internal */
  /** @endcond
  */

} sys_rab_status_e_type;


#endif /* #ifndef SYS_H */
