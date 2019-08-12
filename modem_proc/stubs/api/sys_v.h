#ifndef SYS_V_H
#define SYS_V_H
/**===========================================================================

              S Y S T E M   H E A D E R   F I L E

@DESCRIPTION
  This header file contains definitions that are shared between Call Manager,
  Call Manager clients and the protocol stacks.


  Organization of the file:

    The file is divided into multiple sections. Each section contains
    type definitions for a particular entity.  The following entities
    are defined in this file:

        Service Indicators
        System Identifiers
        Network Lists
        Mobility Management Information
        Menu Selection
        BCMCS

    Each section is further subdivided to represent different types of
    information.  The types of information include:

        #defines
        enumerated types
        data types
        functions

  NOTE:
    Please do not change the order of the enumerations.  If a new
    enumeration value is needed, add to the end of the enumerated
    type.  Other code may be dependent on the existing order.

Copyright (c) 2002 - 2012 by QUALCOMM Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/**===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/stubs/api/sys_v.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/12/12   sg      Add support for SYS_SERVICE_SEARCH_IRAT_PLMN 
04/30/12   xs      Add support of sending FPLMN list
04/04/12   xs      Codec info propagation
03/13/12   vk      Removal of feature flags added for CELL_DCH feature.
03/02/12   vk      Added feature to get DCH cell info.
11/28/11   ab      Roam status is included in CM_CONN_MODE_SERVICE_STATUS_IND
07/22/11   xs      Added EPS network feature support definition (PS voice indication)
07/04/11   rk      Adding support for voice domain preference
03/01/11   fj      Add sys_block_plmn_e_type for PLMN blocking request.
06/15/10   aj      update to MMSS data structures /utility APIs
06/15/10   aj      support for IRAT measurement support
05/21/10   aj      Updated comments related to MMSS datatypes
04/26/10   rm      Moving sys_channel_type to sys.h for FTD
03/28/10   aj      Add sys_plmn_get_mcc_mnc2() for wildcard mnc support
03/21/10   aj      MMSS support
02/09/10   jqi     ESS support.
12/07/09   fj      Added SYS_ACT_LTE_MASK.
12/03/09   rm      Correct the order of input parameters in 
                   sys_plmn_list_found_plmn - Typo correction
07/10/09   np      Adding LTE Code 
09/07/09   rm      Moving SYS_BAND_MASK_* from sys_v.h to sys.h
07/01/09   rm      Moving few defintions to sys.h from here
06/22/09   rm      Including sys.h
04/06/2009 mj      Update BOOLEAN macro w/ namespace prefix to SYS_BOOLEAN
02/23/09   rm      Moved sys_band_mask_e_type to sys.h
09/12/08   fj/aj   Support Android RIL for ARM9 Reset
09/04/08   ks      Remove info for Neighbors Cell IDs
08/19/08   sv      Increased SYS_NETWORK_LIST_INFO_MAX to 25
                   Added security info as part of sys_wlan_bss_info_s_type
07/30/08   rm      CMI defeaturisation
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

#include "customer.h"  /* Customer configuration file                   */
#include "comdef.h"    /* Common definitions such as byte, uint16, etc. */

#include "sys.h"

/** Macro to convert an expression to a boolean.
*/
#define SYS_BOOLEAN( val )  ( (val) ? TRUE : FALSE )

/** Macro to check if bit-mask e1 contains at least a part of e0.
*/
#define SYS_IS_BIT_CONTAIN( e1, e0 ) ( (SYS_BOOLEAN ((word) e1 & (word) e0)) )

/** Macro to return bits that are set either in e1 or in e0.
*/
#define SYS_BIT_ADD( e1, e0 )        ( (word) e1 | (word) e0 )

/** Macro to get common bits in both e1 and e0.
*/
#define SYS_BIT_COMMON( e1, e0 )     ( (word) e1 & (word) e0 )

/** Macro to take the bits set in e0 out of e1.
*/
#define SYS_BIT_DELETE( e1, e0 )     ( (word) e1 & ~(word) e0 )



#define CMSS_INVALID_ARFCN_INFO          0xFFFF
   /* value for Invalid ARFCN info
   */
#define CMSS_INVALID_BSIC_INFO           0xFF

#define CMSS_INVALID_REFPN_INFO          0xFFFF
   /* value for Invalid REFPN info
   */

/** 
** Macro to prevent lint warning 715 'Symbol not referenced' 
** (cleaner than lint suppressing comments)
** Have the macro expand with Lint suppression 
** -e774 Boolean within 'if' always evaluates to False/True 
*/
#define SYS_ARG_NOT_USED(arg) /*lint -save -e774*/if(arg){}/*lint -restore*/

/**
** Macro to prevent lint warning 818 'Symbol could be declared as pointing to
** const'.
*/
#define SYS_ARG_NOT_CONST(arg) /*lint -e{717,506,774} */ do{if(0){(*arg)=(*arg);}}while(0)


/**     
  System active band union.
*/
typedef union sys_active_band_u
{
  sys_band_mask_type                                  active_band;
  /** Used with SYS_BAND_MASK_* band masks */

  sys_band_mask_type                                  lte_active_band;
  /** Used with SYS_BAND_MASK_LTE_* band masks */

  sys_band_mask_type                                  tds_active_band;
  /** Used with SYS_BAND_MASK_TDS_* band masks */

} sys_active_band_u_type;


/** @}
*
*/

/*------------------------------------------------------------------------------
   DDTM related define.
------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
   Multiple PDP Contexts Defines
------------------------------------------------------------------------------*/

/** @defgroup MPCD Multiple PDP Contexts Defines
*@{
*/

#define SYS_MBMS_MAX_PDP_CONTEXTS_PER_IFACE 5 

#define SYS_MBMS_MAX_IFACE 2 // We support IPV6 and IPV4 Ifaces for MBMS

#define SYS_MBMS_MAX_PDP_CONTEXTS SYS_MBMS_MAX_IFACE* SYS_MBMS_MAX_PDP_CONTEXTS_PER_IFACE

/**==============================================================================

                     S E R V I C E   I N D I C A T O R S

==============================================================================*/

/*------------------------------------------------------------------------------
   Service Indicators Defines
------------------------------------------------------------------------------*/

/** @defgroup SI Service Indicators
*@{
*/






/**
** Cellular and PCS bands to band-class mapping.
**
** Note that IS-683-A PRL only provide support for one cellular band and one
** PCS band. Therefore we define a Cellular and PCS bands and map them to
** different band-classes depanding on the market, as follows:
**
** Cellular band is mapped to band-class 3 for the Japanese market or band-
** class 0 for any other market.
**
** PCS band is mapped to band-class 4 for the Korean market or band-class 1
** for any other market.
*/

  #define SYS_BAND_CLASS_CELL  SYS_BAND_BC0  /**< US Cellular band-class       */

  #define SYS_BAND_CLASS_PCS   SYS_BAND_BC1  /**< US PCS band-class            */


/** Invalid channel.
*/
#define SYS_INVALID_CHANNEL  (sys_chan_type)(-1)



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Extended service info from NAS
*/
typedef enum sys_extend_srv_info_type_e
{
  SYS_EXTEND_SRV_INFO_NONE,         /* No need to start T3402 */
  
  SYS_EXTEND_SRV_INFO_T3402_STARTED /* T3402 to be started */
  
}sys_extend_srv_info_e_type;

/*---------------------------------------------------------------------------
                         Simultaneous mode type
---------------------------------------------------------------------------*/


 


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/**
** structure to hold user zone name information.
*/



/** Structure to define a CDMA/HDR channel as well as mode of operation.
*/
typedef struct
{

  sys_sys_mode_e_type       mode;
    /**< Mode - AMPS, CDMA or HDR, etc */

  sys_channel_type          channel;
    /**< Channel */

} sys_channel_record_type;

/** Structure to define a CDMA/HDR avoided channels and mode */
typedef struct
{
  sys_sys_mode_e_type       mode;
    /**< Mode - AMPS, CDMA or HDR, etc */

  sys_channel_type          channel;
    /**< Channel */

  dword                     uptime;
    /**< Clock Uptime */

  dword                     avoided_time;
    /**< Avoided Time */

} sys_avoided_channel_record_type;


/* Structure to define a HDR system type.
*/
typedef enum
{
  /* High Data Rate (HDR) mode, ChannelNumber field specifies
   * forward and Reverse CDMA channels that are FDD-paired 
   */ 
  SYS_HDR_CHAN_REC_MODE_IS856 = 0, 

  /* A CDMA 2000 system */
  SYS_HDR_CHAN_REC_MODE_IS2000 = 1,

  /* High Data Rate (HDR) mode, ChannelNumber Field 
   *  specifies only the forward CDMA channel 
   */
  SYS_HDR_CHAN_REC_MODE_IS856_FWD_CH = 2, 

  /* For internal use only*/
  SYS_HDR_CHAN_REC_MODE_MAX 

} sys_hdr_chan_rec_mode_e_type;

/* Structure to define a HDR channel record.
*/
typedef struct
{

  sys_hdr_chan_rec_mode_e_type       mode;

  sys_channel_type          channel;
    /* Channel */

} sys_hdr_channel_record_s_type;

/* Structure to define entire linke : Channel and Pilot PN
*/
typedef struct
{
  sys_hdr_chan_rec_mode_e_type    mode;

  sys_channel_type          channel;
    /* Channel */

  sys_pilot_pn_type         pilot_pn;
    /* Pilot PN */

} sys_link_type;

#define SYS_CHAN_CMP_EQUAL(c1, c2) (((c1).band == (c2).band) && \
                                    ((c1).chan_num == (c2).chan_num))

/*--------------------------------------------------------------------------
   PDP Header and Data compression enums
--------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
   Service Indicator Functions
------------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_oprt_mode_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_oprt_mode_e_type  value          Value that is to be checked
                                       for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_oprt_mode_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_oprt_mode_is_valid
(
  sys_oprt_mode_e_type  value
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_sys_mode_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_sys_mode_e_type  value          Value that is to be checked
                                      for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_sys_mode_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_sys_mode_is_valid
(
  sys_sys_mode_e_type  value
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_band_class_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_band_class_e_type  value        Value that is to be checked
                                      for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_band_class_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_band_class_is_valid
(
  sys_band_class_e_type  value
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_block_or_system_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_blksys_e_type  value            Value that is to be checked
                                      for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_blksys_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_block_or_system_is_valid
(
  sys_blksys_e_type  value
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_srv_status_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_srv_status_e_type  value            Value that is to be checked
                                          for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_srv_status_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_srv_status_is_valid
(
  sys_srv_status_e_type  value
);



/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_srv_status_is_srv

@DESCRIPTION

  Function that verifies that the input value is indicates any service.

@PARAMETERS

  sys_srv_status_e_type  value            Value that is to be checked
                                          for validity.

@RETURN VALUE

  boolean  Flag indicating whether the value indicates service( TRUE)
           or no service (FALSE).

==============================================================================*/
boolean sys_srv_status_is_srv
(
  sys_srv_status_e_type  value
);



/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_srv_status_is_full_srv

@DESCRIPTION

  Function that verifies that the input value is indicates full service.

@PARAMETERS

  sys_srv_status_e_type  value            Value that is to be checked
                                          for validity.

@RETURN VALUE

  boolean  Flag indicating whether the value indicates full service( TRUE)
           or no full service (FALSE).

==============================================================================*/
extern boolean sys_srv_status_is_full_srv
(
  sys_srv_status_e_type  value
);

/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_srv_domain_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_srv_domain_e_type  value            Value that is to be checked
                                          for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_service_domain_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_srv_domain_is_valid
(
  sys_srv_domain_e_type  value
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_srv_capability_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_srv_capability_e_type   value       Value that is to be checked
                                          for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_srv_capability_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_srv_capability_is_valid
(
  sys_srv_capability_e_type value
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_roam_status_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_roam_status_e_type value                   Value that is to be checked
                                                 for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_roam_status_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_roam_status_is_valid
(
  sys_roam_status_e_type  value
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_radio_access_tech_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_radio_access_tech_e_type value             Value that is to be checked
                                                 for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_radio_access_tech_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_radio_access_tech_is_valid
(
  sys_radio_access_tech_e_type  value
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_sim_state_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_sim_state_e_type value                     Value that is to be checked
                                                 for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_sim_state_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_sim_state_is_valid
(
  sys_sim_state_e_type  value
);

#ifdef __cplusplus
}
#endif

/* @}
**
*/

/**==============================================================================

                     S Y S T E M   I D E N T I F I E R

==============================================================================*/

/* @defgroup SI System Identifier
**
*/

/*------------------------------------------------------------------------------
   System Identifier Defines
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   System Identifier Enums
------------------------------------------------------------------------------*/

#define SYS_NW_PROFILE_ID_INVALID     (sys_profile_id_type)(-1)

/*------------------------------------------------------------------------------
   System Identifier Functions
------------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_plmn_get_digits

@DESCRIPTION

  Function that returns the each of the digits in the MCC and MNC contained in
  a PLMN ID as integer values.

@PARAMETERS

  sys_plmn_id_s_type                   plmn provided as input from which
            plmn                       to read the MCC and MNC.
  uint32*   mcc_digit_1_ptr            First digit of the MCC.
  uint32*   mcc_digit_2_ptr            Second digit of the MCC.
  uint32*   mcc_digit_3_ptr            Third digit of the MCC.
  uint32*   mnc_digit_1_ptr            First digit of the MNC.
  uint32*   mnc_digit_2_ptr            Second digit of the MNC.
  uint32*   mnc_digit_3_ptr            Third digit of the MNC.

@RETURN VALUE

  void

==============================================================================*/
extern void sys_plmn_get_digits
(
  sys_plmn_id_s_type plmn_id,
  uint32*            mcc_digit_1_ptr,
  uint32*            mcc_digit_2_ptr,
  uint32*            mcc_digit_3_ptr,
  uint32*            mnc_digit_1_ptr,
  uint32*            mnc_digit_2_ptr,
  uint32*            mnc_digit_3_ptr
);



/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_plmn_set_mcc_mnc

@DESCRIPTION

  Function that fills a PLMN ID with the MCC and MNC provided as integer values.

@PARAMETERS

  boolean           mnc_includes_pcs_digit  Flag indicating whether the MNC
                                            includes the third PCS digit (TRUE)
                                            or not (FALSE)
  uint32            mcc                     MCC as an integer value (0 - 999).
  uint32            mnc                     MCC as an integer value (0 - 99 or
                                            0 - 999 if the MCS digit is
                                            included).
  sys_plmn_id_type* plmn_ptr                Pointer to the plmn to populate
                                            input from which to read the MCC and
                                            MNC.

@RETURN VALUE

  boolean  Flag indicating whether or not the plmn_ptr was successfully filled
           with values provided for the MCC and MNC.  If TRUE then *plmn_ptr
           was successfully filled.  If FALSE then the input parameters were
           determined to be out of range and *plmn_ptr was set to 0xFFFFFF.

==============================================================================*/
extern boolean sys_plmn_set_mcc_mnc
(
  boolean               mnc_includes_pcs_digit,
  sys_mcc_type          mcc,
  sys_mnc_type          mnc,
  sys_plmn_id_s_type*   plmn_ptr
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_plmn_set_mcc_mnc2

@DESCRIPTION

  Function that fills a PLMN ID with the MCC and MNC provided as integer values.
  or as wildcard values.
  MCC wildcard = MNC wildcard = 0xFFF

@PARAMETERS

  boolean           mnc_includes_pcs_digit  Flag indicating whether the MNC
                                            includes the third PCS digit (TRUE)
                                            or not (FALSE)
  uint32            mcc                     MCC as an integer value (0 - 999)
                                            or 0xFFF
  uint32            mnc                     MCC as an integer value (0 - 99 or
                                            0 - 999 if the MCS digit is
                                            included).OR wildcard 0xFF or 0xfff
  sys_plmn_id_type* plmn_ptr                Pointer to the plmn to populate
                                            input from which to read the MCC and
                                            MNC.

@RETURN VALUE

  boolean  Flag indicating whether or not the plmn_ptr was successfully filled
           with values provided for the MCC and MNC.  If TRUE then *plmn_ptr
           was successfully filled.  If FALSE then the input parameters were
           determined to be out of range and *plmn_ptr was set to 0xFFFFFF.

==============================================================================*/
extern boolean sys_plmn_set_mcc_mnc2
(
  boolean               mnc_includes_pcs_digit,
  sys_mcc_type          mcc,
  sys_mnc_type          mnc,
  sys_plmn_id_s_type*   plmn_ptr
);

/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_plmn_get_mcc_mnc2

@DESCRIPTION

  Function that returns the MCC and MNC contained in a PLMN ID as integer
  values.Same as sys_plmn_get_mcc_mnc() but adds support for wildcard MNC

@PARAMETERS

  sys_plmn_id_s_type                   plmn provided as input from which
            plmn                       to read the MCC and MNC.
  boolean*  plmn_is_undefined_ptr      Flag indicating that the input plmn
                                       contains a defined MCC and MNC (FALSE)
                                       or was set to 0xFFFFFF (TRUE).
  boolean*  mnc_includes_pcs_digit_ptr Flag indicating whether the MNC included
                                       the third PCS digit (TRUE) or not (FALSE)
  uint32*   mcc_ptr                    MCC converted from Binary Coded Decimal
                                       Digits in the plmn to an integer.
  uint32*   mnc_ptr                    MNC converted from Binary Coded Decimal
                                       Digits in the plmn to an integer or wildcard value

@RETURN VALUE

  void

==============================================================================*/
extern void sys_plmn_get_mcc_mnc2
(
  sys_plmn_id_s_type  plmn,
  boolean*            plmn_is_undefined_ptr,
  boolean*            mnc_includes_pcs_digit_ptr,
  sys_mcc_type*       mcc_ptr,
  sys_mnc_type*       mnc_ptr
);
/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_plmn_id_is_valid

@DESCRIPTION

  Function that checks if each of the digits in the input plmn are between 0
  and 9 or equal to 0xF.

@PARAMETERS

  sys_plmn_id_s_type  plmn                PLMN ID to validate.

@RETURN VALUE

  boolean  Flag indicating if the plmn is valid (TRUE) or not (FALSE).

==============================================================================*/
extern boolean sys_plmn_id_is_valid
(
  sys_plmn_id_s_type  plmn
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_plmn_id_is_undefined

@DESCRIPTION

  Function that checks if each of the digits in the input plmn is equal to
  0xF.  In other words, an undefined plmn contains 0xFFFFFF.

@PARAMETERS

  sys_plmn_id_s_type  plmn                PLMN ID to check.

@RETURN VALUE

  boolean  Flag indicating if the plmn is undefined (TRUE) or not (FALSE).

==============================================================================*/
extern boolean sys_plmn_id_is_undefined
(
  sys_plmn_id_s_type  plmn
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_plmn_undefine_plmn_id

@DESCRIPTION

  Function that sets each of the digits in the plmn_id to 0xF.  The resulting
  undefined *plmn_ptr contains 0xFFFFFF.

@PARAMETERS

  sys_plmn_id_s_type*  plmn_ptr           Pointer to the PLMN ID to undefine.

@RETURN VALUE

  void

==============================================================================*/
extern void sys_plmn_undefine_plmn_id
(
  sys_plmn_id_s_type*  plmn_ptr
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_plmn_match

@DESCRIPTION

  Function that compares two PLMN IDs and determines if they are equal.
  plmn_2 is the ID of the PLMN broadcast by the network and plmn_1 is the
  ID of the PLMN that it is being compared against (e.g., the HPLMN stored
  on the SIM).  If the third MNC digit in the plmn_2 is 0xF, then the plmn_2
  is assumed to contain a two digit MNC so the third MNC digit is not compared.
  This function differs from sys_plmn_match in that it implements a further
  requirement defined in 3GPP TS23.122 Annex A for comparing the HPLMN in
  North American PCS networks.

@PARAMETERS

  sys_plmn_id_s_type  plmn_1                  The first PLMN ID.
  sys_plmn_id_s_type  plmn_2                  The second PLMN ID.

@RETURN VALUE

  boolean  Flag indicating whether the two PLMN IDs are equal (TRUE) or
  not (FALSE).

==============================================================================*/
extern boolean sys_plmn_match
(
  sys_plmn_id_s_type  plmn_1,
  sys_plmn_id_s_type  plmn_2
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_mcc_match

@DESCRIPTION

  Function that compares two PLMN IDs and determines if their MCCs are equal.
 

@PARAMETERS

  sys_plmn_id_s_type  plmn_1                  The first PLMN ID.
  sys_plmn_id_s_type  plmn_2                  The second PLMN ID.

@RETURN VALUE

  boolean  Flag indicating whether the two MCC s are equal (TRUE) or
  not (FALSE).

==============================================================================*/
extern boolean sys_mcc_match
(
  sys_plmn_id_s_type  plmn_1,
  sys_plmn_id_s_type  plmn_2
);

/**==============================================================================

@FUNCTION NAME

  sys_mcc_same_country

@DESCRIPTION

  Function to check if the MCCs are of same country. MCCs from NA with
  different values (in range 310-316) are considered as MCCs of same country.

@RETURN VALUE

  TRUE if MCCs are of same country, FALSE otherwise

==============================================================================*/

extern boolean sys_mcc_same_country
(
  uint32  mcc1,
  uint32  mcc2
);

/**===========================================================================

@FUNCTION sys_undefine_sys_id

@DESCRIPTION
  This function undefines or initializes a system identifier.

@DEPENDENCIES
  None

@RETURN VALUE
  None

@SIDE EFFECTS
  None

===========================================================================*/
extern void sys_undefine_sys_id(

  sys_sys_id_s_type     *sys_id_ptr
);


/**===========================================================================

@FUNCTION sys_sys_id_match

@DESCRIPTION
  This function compares two system identifiers.

@DEPENDENCIES
  None

@RETURN VALUE
  TRUE  = if the two system identifiers are equal
  FALSE = if the two system identifiers are not equal

  Note: TRUE is returned if both system identifiers are undefined.

@SIDE EFFECTS
  None

===========================================================================*/
extern boolean sys_sys_id_match(

  sys_sys_id_s_type     sys_id_1,
  sys_sys_id_s_type     sys_id_2
);

/**===========================================================================

@FUNCTION sys_lac_match

@DESCRIPTION
  This function compares two LAC system identifiers of a UMTS SYS ID.

@DEPENDENCIES
  None

@RETURN VALUE
  TRUE  = if the two LAC are equal
  FALSE = if the two LAC are not equal

  Note: TRUE is returned if both system identifiers are not UMTS type.

@SIDE EFFECTS
  None

===========================================================================*/
extern boolean sys_lac_match(

  sys_sys_id_s_type     sys_id_1,
  sys_sys_id_s_type     sys_id_2
);

/** @}
**
*/

#ifdef __cplusplus
}
#endif

/**==============================================================================

                            N E T W O R K   L I S T S

==============================================================================*/

/** @defgroup NL Network lists
*  @{
*/

/*------------------------------------------------------------------------------
   Network Lists Defines
------------------------------------------------------------------------------*/

#define SYS_FORBIDDEN_PLMN_LIST_MAX_LENGTH  20
  /**< Maximum number of forbidden PLMNs */

#define SYS_PRIORITY_PLMN_LIST_MAX_LENGTH   256


/** no access technology.
*/
#define  SYS_ACT_NONE_MASK         ( (sys_access_tech_e_type) 0)

/** GSM access technology.
*/
#define  SYS_ACT_GSM_MASK         ( (sys_access_tech_e_type) (1<<0) )

/** GSM compact access technology.
*/
#define  SYS_ACT_GSM_COMPACT_MASK ( (sys_access_tech_e_type) (1<<1) )

/** UMTS radio access technology.
*/
#define  SYS_ACT_UMTS_MASK        ( (sys_access_tech_e_type) (1<<2) )

/** LTE radio access technology.
*/
#define  SYS_ACT_LTE_MASK         ( (sys_access_tech_e_type) (1<<3) )

/** GW RAT mask
*/
#define  SYS_ACT_GW_MASK   ((sys_access_tech_e_type) SYS_ACT_GSM_MASK | \
                                                    SYS_ACT_UMTS_MASK)

/** 3GPP all radio access technology.
*/
#define  SYS_ACT_3GPP_ANY_MASK   ((sys_access_tech_e_type) SYS_ACT_GSM_MASK | \
                                  SYS_ACT_UMTS_MASK |  SYS_ACT_LTE_MASK)


typedef enum sys_block_plmn_e
{
  SYS_BLOCK_PLMN_BLOCK,
  SYS_BLOCK_PLMN_UNBLOCK,
  SYS_BLOCK_PLMN_RESET
} sys_block_plmn_e_type;


/**
** Define a structure for simple PLMN lists.
*/
typedef struct sys_plmn_list_s
{
  /** The number of PLMNs in the list.
  */
  uint32                                    length;

  sys_plmn_id_s_type                        plmn[SYS_PLMN_LIST_MAX_LENGTH];
} sys_plmn_list_s_type;



/** Type that captures the network list info after power scan/survey.
*/
typedef struct sys_network_info_s
{
  /** Type of network , GW PLMN etc.
  */
  sys_sys_mode_e_type                      type;

  /** Network info.
  */
  union
  {

    sys_detailed_plmn_list_info_s_type     plmn;

  } info;

} sys_network_info_s_type;


/** List type for Network information.
*/
typedef struct sys_network_list_info_s
{
  /** Number of valid system Info defined in this list.
  */
  int                                       num;

  /** Network info list.
  */
  sys_network_info_s_type                   list[SYS_NETWORK_LIST_INFO_MAX];

} sys_network_list_info_s_type;


/*------------------------------------------------------------------------------
   MMSS Data Types
------------------------------------------------------------------------------*/
/* Maximum number of PLMNs supported for all MMSS operations
*/
#define SYS_MMSS_PLMN_LIST_MAX_LENGTH       640

typedef struct sys_plmn_rat_category_list_info_s
{
  /** The ID of the PLMN.
  */
  sys_plmn_id_s_type                        plmn;

  /** bitmask of RATs that are associated with each PLMN (MCC and
   *  MNC) in the PLMN selector list. For EHPLMN/HPLMN - this will
   *  be all 3GPP RATs. For OPLMN/UPLMN - this is determined by
   *  the ACT bits in those files.
  */
  sys_access_tech_e_type                    act;

  /** Indicates the type of PLMN( HOME, PREFERRED, OTHER)
  */
  sys_detailed_plmn_list_category_e_type    list_category;

  /** The following fields are used to determine how to place each
   *  PLMN DB entry into the equivalent PRL.
  */

  /** Before processing a GEO, the geo_mcc_mnc_act_mask field of each
   *  PLMN is initialized to the all the ACTs associated with it
   *  (in 'act' field). When this PLMN entry is entered into the
   *  GEO being processed , the associated ACT bits being inserted
   *  into the GEO are deleted from this mask. At any given point,
   *  this mask will have information regarding what ACTs are yet
   *  to be entered into the GEO for the associated MCC,MNC (in
   *  'plmn' field)
  */
  sys_access_tech_e_type                    geo_mcc_mnc_act_mask;

  /** Before processing a GEO, the geo_mcc_wcard_mnc_act_mask field of each
   *  PLMN is initialized to the all the 3GPP ACTs . When this MCC
   *  (from 'plmn' field) and wildcard MNC is entered into the GEO
   *  being processed , the associated ACT bits being inserted
   *  into the PRL are deleted from this mask. At any given point,
   *  this mask will have information regarding what ACTs are yet
   *  to be entered into the GEO for the associated MCC along with
   *  wildcard MNC
  */
  sys_access_tech_e_type                    geo_mcc_wcard_mnc_act_mask;

  /** Before processing the PRL, the prl_mcc_act_mask
   *  field of each PLMN is initialized to the all the 3GPP ACTs .
   *  When this MCC (together with specific MNC or with wildcard
   *  MNC) is entered into the PRL , the associated ACT bits are
   *  deleted from this mask. At any given point, this mask will
   *  have information regarding what ACTs are still to be entered
   *  into the PRL for the associated MCC. This is used to
   *  determine if any PLMN db entry is not associcated with 3GPP2
   *  entries and needs to be entered into a seperate GEO in the
   *  equivalent PRL
  */
  sys_access_tech_e_type                    prl_mcc_act_mask;

} sys_plmn_rat_category_list_info_s_type;

/**
*  Define a structure for the PLMN RAT category list.
*/
typedef struct sys_plmn_rat_category_list_s
{
  /** The number of PLMNs in the list.
  */
  uint32                                    length;

  /** PLMN info.
  */
  sys_plmn_rat_category_list_info_s_type    info[SYS_MMSS_PLMN_LIST_MAX_LENGTH];

} sys_plmn_rat_category_list_s_type;


/*------------------------------------------------------------------------------
   Network Lists Functions
------------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_signal_quality_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_signal_quality_e_type value                Value that is to be checked
                                                 for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_signal_quality_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_signal_quality_is_valid
(
  sys_signal_quality_e_type value
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_detailed_plmn_list_category_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_detailed_plmn_list_category_e_type value   Value that is to be checked
                                                 for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_detailed_plmn_list_category_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_detailed_plmn_list_category_is_valid
(
  sys_detailed_plmn_list_category_e_type  value
);



/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_detailed_plmn_list_find_plmn

@DESCRIPTION

  Function that verifies that the given PLMN  is in the PLMN list.

@PARAMETERS

 sys_plmn_rat_category_list_s_type  plmn_list_ptr    The PLMN list to be searched
 sys_plmn_id_s_type                plmn             The PLMN being searched


@RETURN VALUE

 uint32  indicating position of match in PLMN list.

==============================================================================*/
extern boolean sys_detailed_plmn_list_find_plmn
(
  const sys_plmn_rat_category_list_s_type          *plmn_list_ptr,
  sys_plmn_id_s_type                               plmn
);

/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_detailed_plmn_list_found_plmn_rat

@DESCRIPTION

  Function that verifies that the given PLMN and RAT is in the PLMN list.

@PARAMETERS

  sys_detailed_plmn_list_s_type  plmn_list_ptr    The PLMN list to be searched
  sys_plmn_id_s_type             plmn             The PLMN being searched
  sys_radio_access_tech_e_type   rat              The RAT being searched

@RETURN VALUE

  boolean  Flag indicating whether the PLMN and RAT was found in the PLMN list.

==============================================================================*/
extern boolean sys_detailed_plmn_list_found_plmn_rat
(
  const sys_detailed_plmn_list_s_type*      plmn_list_ptr,
  sys_plmn_id_s_type                        plmn,
  sys_radio_access_tech_e_type              rat
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_plmn_list_found_plmn

@DESCRIPTION

  Function that verifies that the given PLMN is in the PLMN list.

@PARAMETERS

  sys_plmn_list_s_type           plmn_list_ptr    The PLMN list to be searched
  sys_plmn_id_s_type             plmn             The PLMN being searched

@RETURN VALUE

  boolean  Flag indicating whether the PLMN and RAT was found in the PLMN list.

==============================================================================*/
extern boolean sys_plmn_list_found_plmn
(
  const sys_plmn_list_s_type*               plmn_list_ptr,
  sys_plmn_id_s_type                        plmn
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_detailed_plmn_list_info_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_detailed_plmn_list_info_s_type value       Value that is to be checked
                                                 for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_detailed_PLMN_list_info_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_detailed_plmn_list_info_is_valid
(
  sys_detailed_plmn_list_info_s_type value
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_detailed_plmn_list_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_detailed_plmn_list_s_type value            Value that is to be checked
                                                 for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_detailed_plmn_list_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_detailed_plmn_list_is_valid
(
  const sys_detailed_plmn_list_s_type* value_ptr
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_plmn_list_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_PLMN_list_s_type value                     Value that is to be checked
                                                 for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_plmn_list_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_plmn_list_is_valid
(
  const sys_plmn_list_s_type* value_ptr
);



/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_user_pref_plmn_list_info_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_user_pref_plmn_list_info_s_type  value     Value that is to be checked
                                                 for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_user_pref_plmn_list_info_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_user_pref_plmn_list_info_is_valid
(
  sys_user_pref_plmn_list_info_s_type  value
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_add_plmn_to_list

@DESCRIPTION

  Function that adds a given plmn to a given plmn list

@PARAMETERS

  sys_plmn_list_s_type * plmn_list 
  sys_plmn_id_s_type   * plmn_ptr

@RETURN VALUE

 None

==============================================================================*/

extern void sys_add_plmn_to_list
(
  sys_plmn_rat_category_list_s_type       * plmn_list, 
  const sys_plmn_id_s_type                * plmn_ptr
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_user_pref_plmn_list_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_user_pref_plmn_list_s_type  value          Value that is to be checked
                                                 for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_user_pref_plmn_list_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_user_pref_plmn_list_is_valid
(
  const sys_user_pref_plmn_list_s_type*  value_ptr
);

#ifdef __cplusplus
}
#endif

/* @}
**
*/

/**==============================================================================

                M O B I L I T Y   M A N A G E M E N T   I N F O

==============================================================================*/

/* @defgroup MMI Mobility Management Info
** @{
*/


/*------------------------------------------------------------------------------
   Mobility Management Enums
------------------------------------------------------------------------------*/






/*------------------------------------------------------------------------------
   Mobility Management Information Functions
------------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_network_name_coding_scheme_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

   sys_network_name_coding_scheme_type_e_type value
                             Value that is to be checked for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_network_name_coding_scheme_type_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_network_name_coding_scheme_is_valid
(
  sys_network_name_coding_scheme_type_e_type value
);

/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_network_short_name_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  const sys_network_short_name_s_type* value_ptr Pointer to the value that is
                                                 to be checked for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_network_short_name_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_network_short_name_is_valid
(
  const sys_network_short_name_s_type* value_ptr
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_network_full_name_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  const sys_network_full_name_s_type* value_ptr  Pointer to the value that is
                                                 to be checked for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_network_full_name_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_network_full_name_is_valid
(
  const sys_network_full_name_s_type* value_ptr
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_time_zone_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_time_zone_type value                       Value that is to be checked
                                                 for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_time_zone_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_time_zone_is_valid
(
  sys_time_zone_type value
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_time_and_time_zone_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  const sys_time_and_time_zone_s_type* value_ptr Pointer to the value that is
                                                 to be checked for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_time_and_time_zone_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_time_and_time_zone_is_valid
(
  const sys_time_and_time_zone_s_type* value_ptr
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_lsa_id_type_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_LSA_identity_type_e_type value             Value that is to be checked
                                                 for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_LSA_identity_type_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_lsa_id_type_is_valid
(
  sys_lsa_identity_type_e_type value
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_lsa_id_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_LSA_identity_s_type value                  Value that is to be checked
                                                 for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_LSA_identity_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_lsa_id_is_valid
(
  sys_lsa_identity_s_type value
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_day_sav_adj_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  sys_daylight_saving_adj_e_type value           Value that is to be checked
                                                 for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_daylight_saving_adj_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_day_sav_adj_is_valid
(
  sys_daylight_saving_adj_e_type value
);


/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_mm_information_is_valid

@DESCRIPTION

  Function that verifies that the input value is within its valid range.

@PARAMETERS

  const sys_mm_information_s_type* value_ptr     Pointer to the value that is
                                                 to be checked for validity.

@RETURN VALUE

  boolean  Flag indicating whether the sys_mm_information_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
extern boolean sys_mm_information_is_valid
(
  const sys_mm_information_s_type* value_ptr
);

#ifdef __cplusplus
}
#endif

/* @}
**
*/

/**==============================================================================

                     M E N U  S E L E C T I O N

==============================================================================*/

/* @defgroup MS Menu Selection
** @{
*/

/*------------------------------------------------------------------------------
   Menu Selection Enums
------------------------------------------------------------------------------*/

/* Enumerations for M51 mode settings - FEATURE_JCDMA_1X
*/



/**==============================================================================

                     UMTS to 1X handover types

==============================================================================*/


/** Invalid is2000 msg type.
*/
#define    SYS_INVALID_IS2000_MSG      (uint8)(-1)



/** From and To technology in hand over operation
*/
typedef enum
{
  SYS_HO_TYPE_NONE = -1,

  SYS_HO_TYPE_VOIP_HDR_1X,

  SYS_HO_TYPE_UMTS_1X,

  SYS_HO_TYPE_MAX

}sys_ho_type_e_type;



/**==============================================================================

  BCMCS - FEATURE_BCMCS

  BCMCS stands for "High rate BroadCast-MultiCast packet data air interface
  Specification"


==============================================================================*/

/*------------------------------------------------------------------------------
   BCMCS Data Types
------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
                                MBMS
----------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
   MBMS (Multimedia broadcast multicast service) MACRO definitions.   
   MBMS is specific to WCDMA.
--------------------------------------------------------------------------*/

#define SYS_MBMS_NUM_MAX_SRVS_PER_REQ 2

/* Maximum number of MBMS services
** that can simultaneously exist.
*/
#define SYS_MAX_MBMS_SERVICES 16



/*---------------------------------------------------------------------------
** MBMS structure definitions
---------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
   MBMS (Multimedia broadcast multicast service) Data Types.   
--------------------------------------------------------------------------*/



/* TMGI structure in int format */
typedef struct
{ 

  /* TRUE  -  TMGI is present
  ** FALSE -  TMGI is not present
  */
  boolean present;

  /* TMGI in uint64 format */
  uint64  tmgi;

}sys_mbms_tmgi_uint_s_type;





typedef uint32 sys_bcmcs_flow_id_type;


/* MBMS session id */
typedef struct
{
  /* TRUE  -  Session id is present
  ** FALSE -  Session id is not present
  */
  boolean present;

  /* Session id_type */
  uint32  session_id;

}sys_mbms_session_id_s_type;


typedef enum{

  SYS_BCMCS_BOM_CACHING_MIN = -1,
  /**< For bounds checking only */

  SYS_BCMCS_BOM_CACHING_STRICT_ENABLE,
  /**< Enable Strict caching */

  SYS_BCMCS_BOM_CACHING_STRICT_DISABLE,
  /**< Disable Strict caching */

  SYS_BCMCS_BOM_CACHING_MAX
  /**< For bounds checking only */

}sys_bcmcs_bom_caching_e_type;




typedef enum 
{
  SYS_SERVICE_SEARCH_NONE = -1,
  SYS_SERVICE_SEARCH_BACKGROUND_AUTOMATIC,
  SYS_SERVICE_SEARCH_BACKGROUND_MANUAL,
  SYS_SERVICE_SEARCH_IRAT_PLMN,
  SYS_SERVICE_SEARCH_MAX
}sys_service_search_e_type;

/*
** Enumeration of phone status change status. Indicates the status of phone
** status change command.
*/
typedef enum
{
  SYS_PH_STATUS_CHGD_NONE = -1,
    /* FOR INTERNAL USE OF CM ONLY!
    */

  /* 0 */
  SYS_PH_STATUS_CHGD_SUCCESS,
    /* Powered down success
    */

  /* 1 */
  SYS_PH_STATUS_CHGD_FAILURE,
    /* Powered down failure
    */

  SYS_PH_STATUS_CHGD_MAX
    /* FOR INTERNAL USE OF CM ONLY!
    */

} sys_ph_status_chgd_status_e_type;

/* Type to update UI with service status info
** when UE is OOS in connected mode 
*/
typedef struct sys_conn_mode_ss_ind_s
{
  /* Service status indicates service availibility */
  sys_srv_status_e_type           service_status;

  /* PLMN */
  sys_plmn_id_s_type              selected_plmn;

  /* RAT */
  sys_radio_access_tech_e_type    rat;

  /* Signal quality */
  sys_signal_quality_e_type       signal_quality;

  /* Signaling strength of the strongest cell in PLMN */
  sys_signal_strength_type        signal_strength;

  /* To store the current status of oos connected mode */
  boolean                         is_connected_mode_oos;

  sys_roam_status_e_type     	  roam_status;
  /**< Indicate current roaming status in connected mode.
    ** Mask used is CM_SS_ROAM_STATUS_MASK.
    */

} sys_conn_mode_ss_ind_s_type;

typedef enum
{
    SYS_GW_CONN_STATUS_NONE = -1,
      /* FOR INTERNAL USE ONLY */
    
    SYS_GW_CONN_STATUS_CONN,   
      /* Connection (being) established */
    
    SYS_GW_CONN_STATUS_BPLMN_SRCH,   
      /* REG initiated HPLMN search is on-going. */
    
    SYS_GW_CONN_STATUS_IDLE,
      /* NAS in idle state */

    SYS_GW_CONN_STATUS_MAX
      /* FOR INTERNAL USE ONLY */

}sys_gw_conn_status_e_type;

/* 
** System Registration status enumeration type
**
** Used by CDMA technology for now.
*/

typedef enum sys_reg_status_e
{
  /* For internal use and range check
  */
  SYS_REG_STATUS_NONE = -1,

  /* The system registration status is unknown.
  */
  SYS_REG_STATUS_UNKNOWN= 0,

  /* The system registration status is invalid.
  */
  SYS_REG_STATUS_INVALID= 1,

  /* The system registration status is valid.
  */
  SYS_REG_STATUS_VALID= 2,

  /* For internal use and range check
  */
  SYS_REG_STATUS_MAX

} sys_reg_status_e_type;
/* Type to detach from CM to mmoc
*/

typedef enum {
/* For internal use and range check
  */
  SYS_PS_DETACH_TYPE_NONE = -1,

/* For triggering detach from MMOC to NAS
 */
  	
  SYS_PS_DETACH_TYPE_MODE_CHANGE,

/* For internal use and range check
  */    
  
  SYS_PS_DETACH_TYPE_MAX

} sys_ps_detach_e_type;

/* This structure contains LTE CS domain registation 
** related information
*/
typedef struct sys_lte_cs_domain_param_s
{

  boolean                                   is_voice_domain_ie_valid;
  /* Is IE voice domain preference and UE's usage setting valid */

  sys_ue_usage_setting_e_type               voice_or_data_centric;
  /* UE Usage setting */

  sys_voice_domain_pref_e_type              voice_domain_pref;
  /* voice domain preference  */
}sys_lte_cs_domain_param_s_type;


typedef enum
{ 
  SYS_FORBIDDEN_LIST_TYPE_NONE = -1,
  /* For internal use only
  */

  SYS_FORBIDDEN_LIST_TYPE_FPLMN           = 0, 
  /* These PLMNs are forbidden for all three LTE, UMTS,GSM 
  */

  SYS_FORBIDDEN_LIST_TYPE_GPRS_FPLMN = 1, 
  /* These PLMNs are forbidden for PS service. For LTE these are always forbidden. 
  ** For GSM/UMTS forbidden if UE is in PS only mode
  */

  SYS_FORBIDDEN_LIST_TYPE_LTE_BACKOFF_FPLMN = 2,
  /* These PLMNs are only forbidden for LTE
  */
   
  SYS_FORBIDDEN_LIST_TYPE_MAX    
  /* internal use only
  */
}sys_forbidden_list_type_e_type;


/** EPS network feature support
*/
typedef struct
{
  boolean                ims_vops_supported;
   /*IMS voice over PS session indicator */

  boolean                lcs_supported;     
   /*Location services indicator in EPC*/

  boolean                emc_bs_supported;  
   /*Emergency bearer services indicator */

  sys_cs_lcs_val_e_type  cs_lcs_val;        
   /*Location services indicator in CS*/
} sys_nw_feature_support_s_type;

typedef enum
{
  SYS_SPEECH_CODEC_INFO,
  SYS_HO_START,
  SYS_HO_FAIL,
  SYS_HO_COMPLETE
} sys_codec_ho_cmd;

typedef struct
{
  sys_codec_ho_cmd        cmd;
  sys_speech_codec_e_type  speech_codec; /*only valid when cmd is SYS_SPEECH_CODEC_INFO */
  uint32                   speech_enc_samp_freq; /* 8000: narrow-band or 16000: wide-band, 0 - unknown: ignore */ 
} sys_codec_ho_info;



#endif /*SYS_V_H*/
