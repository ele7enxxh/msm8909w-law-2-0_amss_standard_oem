/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S

                  DATA SESSION PROFILE DATA STRUCTURE AND CONTROL FUNCTIONS

GENERAL DESCRIPTION
  The file contains data structures and utilty functions to support Application
  profiles specific requirements and Dormant 2 (Lingering PPP session)
  requirements.

EXTERNALIZED FUNCTIONS

  DS707_DATA_SESSION_SET_PROFILE
    This function reads the EFS files containing data session profile parameters

  DS707_DATA_SESSION_GET_PROFILE
    This function reads data session profile parameters from ds707_data_session_profile_info_type[ ] for
    a particular data_session_profile_id.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

   Copyright (c) 2004-2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_data_session_profile.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/02/15   sd      Fix to read profile from NVRUIM only after subscription 
                   ready event.
02/16/15   sc      Pick up non null APN profile for ALWAYS_SHARE_PROFILE
01/27/15   sd      Fixed MIP to SIP fallback handling for OMH profiles in
                   eHRPD mode.
12/26/14   vm      Enter emergency mode on mandatory APN check failure only if
                   nv minapnlist disallow call is set to TRUE
12/08/14   sd      Fix to not pick up OMH profile if eHRPD is the preferred system.
11/20/14   sd      Fixed index mapping for OMH profiles.
11/10/14   vm      Fix to not update eHRPD capability on mandatory APN check
                   success when eHRPD_attach_allowed flag is already true
10/29/14   sd      Clear ds707_omh_profile_id in route ACL while in eHRPD mode.
09/05/14   sd      Fix to remove addition of OMH profiles to policy manager
                   database.
09/03/14   sd      Fix to prevent priority of default profile from being modified
                   when OMH card is present
08/05/14   ms      DSDA Phase 2 changes.
07/28/14   hr      Sim swap feature
07/23/14   sd      Fix to use credentials from OMH profile in case of card mode
                   during MIP to SIP fallback.
07/18/14   hr      Use 3GPP2 default profile instead of card default 
                   profile in non C2K RATs
07/18/14   hr      To support default APN
07/04/14   vm      Added profile parameters for stateless DHCPv6 
06/30/14   niv     Added wrapper functions & API change for 
                   NV_REFRESH 
06/26/14   ms      Data+MMS - DSDA changes Phase 1.
05/22/14   sd      Fix to bypass OMH and LBS related validations if we are in 
                   eHRPD mode, or if both the existing and requesting profile
                   types are not OMH.
12/23/13   vm      Stack memory optimization changes 
11/18/13   sd      Fix made to start wait_for_emerg_call_timer when handling
                   DS_707_PH_SYS_SEL_PREF_NORMAL command to support back to
                   back emergency calls. 
11/14/13   vm      Fixed KW errors 
06/26/13   sc      Replace all the memcpy and memmove with memscpy and 
                   memsmove as its safer.
06/03/13   sd      ATCoP decoupling changes
05/15/13   sd      Ignore EMC exit notification from CM if a new emergency call
                   is in progress
01/08/13   ssb     Adding utils function to set/get sip_active_profile_index 
12/02/12   jz      Support for Operator PCO 
08/13/12   ss      Removed ds_create_efs_item_file usage. Listed the file in 
                   ds_init_efs_config_file instead.
07/13/12   jz      Support for emergency only mode when mandatory APN is disabled 
07/11/12   sb      Fixed KW Errors
05/25/12   jee     To support resetting DS profiles to default in 3gpp2
05/14/12   sn      Initialized PDN type parameter as V4V6 for OMH profiles to 
                   ensure IP version in profile and app policy matches for 
                   V6 call to be successful.
05/07/12   op      Changed F3 messages in set_ehrpd_params from ERROR to HIGH
05/04/12   jee     Mainlined FEATURE_UIM_SUPPORT_HOTSWAP
04/19/12   sb      Feature Cleanup: Always On Flag FEATURE_UIM_SUPPORT_3GPD
04/18/12   jz      Seperate old JCDMA and Simplified JCDMA
04/12/12   jee     To propagate correct call end reason from MH
04/09/12   msh     Change units for some PPP timers to seconds in EFS
03/26/12   jz      disallow any call when any mandatory APN is diabled, plus 
                   the disallow nv is set to TRUE. 
03/02/12   msh     Fix token parser  
02/27/12   op      Changed F3 error msg for invalid PDN level auth protocol type
01/31/12   jz      fix gonull timer bug, ds707_gonulltmr_get_timer returns a 
                   value in second, in ds707_data_session_profile_set_common_params
                   should input values in second 
01/19/12   jz      PDN failure fallback list 
01/19/12   jz      add mandatory pdn label list 
01/11/12   op      Fix to properly store PDN level auth protocol in 
                   ds707_profile_construct_str_from_ident()
12/23/11   sb      Fixed compiler warnings
11/11/11   jee     To fix Arbitration issue with v4/v6 calls
11/10/11   jz      Add PDN Label
10/27/11   op      Added support for PDN level authentication
10/03/11   msh     Suppport PAP_CHAP AUTH protocol  
09/26/11   msh     Fixed compiler warnings 
09/08/11   ash     Added support for eHRPD to HRPD fallback
08/26/11   ms      Fixed compiler warnings 
08/03/11   var     Added return value for ds707_data_session_get_profile
06/03/11   mg      Notify HDR-CP when eHRPD allowed flag is changed - depending
                   on mandatory APNs check
05/25/11   ack     Adding profile persistence option
05/06/11   ack     Added create profile api support for 3GPP2
04/29/11   ack     Write profile changes to efs
04/11/11   ttv     Added support for Hot Swap feature.
04/06/11   fjia    Specify index before reading PPP parameters from NV
03/31/10   mg      Changing apn string comparison to be case insensitive
03/16/11   sn      Fix to ensure that DS falls back to use default profile when
                   there are no profiles provisioned on the card even though
                   number of valid profiles is provisioned to a value > 0.
03/15/11   mg      Global variable cleanup
02/11/11   ack     Global Variable Cleanup
01/21/11   mg      Added APN class field to profile
01/17/11   ss      Added support for Iface linger.
10/25/10   ttv     Merged the fix of the issue in picking up auth credentials
                   from NV for CT RUIM cards.
10/14/10   sa      Adding debug messages before ASSERT.
10/20/10   op      Added support for PDN throttling timers in profiles
10/13/10   op      Migrated to MSG 2.0 macros
09/03/10   ms      Legacy or Non-OMH apps should always pickup default profile
                   instead of first profile.
08/19/10   mg      throttle system (call dctm api) if ehrpd allowed flag is false
                   (ie mandatory apn(s) not enabled)
08/12/10   ms      Merged the changes to support 3GPD Ext (n15) disabled scenario in OMH.
07/28/10   ttv     Merged the fix for the issue of not reading MIP active
                   profile index from NV item NV_DS_MIP_ACTIVE_PROF_I during
                   call establishment.
05/13/10   ssh     Added support for PDN Inactivity timer
05/04/10   mg      Dual IP feature
04/16/10   ls      Change dshdr_efs* to ds_efs*
03/25/10   ps      Updated KDDI friend/foe logic to allow concurrent connection
                   requested by default profiles or by a profile same as that
                   of current profile.
03/16/10   gc      Lint errors
01/08/10   ps      Changed the level of some extraneous messages from
                   HIGH to LOW.
01/04/10   ls      Remove the check from ds707_sec_pkt_mgr_is_ehrpd_capable
12/18/09   ps      Fixed KlocWork errors.
12/17/09   ms      Compiler Warning fix.
12/10/09   ps      Mainlining OMH feature based on new profile management
                   design.
11/02/09   as      Fixed ARM Compiler warnings - typecasting appropriately
09/10/09   ls      Clean up the Klocwork errors
07/27/09   spn     Added a utility function which would decide based on
                   current profile and current system configuration if a call
                   is allowed to mature or not.
02/09/09   spn     Moved Data Session Profile Info into the pkt_state
12/31/08   psng    Fixed Lint error and compiler warning.
12/04/08   mkv     at+crm=1,phys link gone only set profile id to invalid.
12/03/08   mkv     at+crm=1, phys link down, set current prof id to default.
11/19/08   sn      Merged changes to store Go NULL TIMER values in SECONDS.
11/05/08   psng    Added boundary check for OMH profile related parameters.
10/17/08   cs      Lint fixes.
09/12/08   psng    Added IPV6 support for OMH and replaced Legacy Messages
                   with 707 messages.
09/02/08   psng    Fixed compiler error by replacing the occurences of
                   GONULL_TIMERI_TIMER_DEFAULT with appropriate
                   global varibles.
08/29/08   psng    Added support to handle iface and cback events for OMH.
08/07/08   as      Lint error fixes.
07/01/08   psng    KlocWork Fixes
04/28/08   psng    Added support for multiple app profiles functionality for
                   OMH.
12/04/07   sc      Changes to store LCP, IPCP and AUTH timeout values in MSEC
08/27/07   mga     Fixed High Lint Error
07/02/07   sc      Created the module. First version of the file.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"

#if defined(FEATURE_DATA_IS707)
#include "msg.h"
#include "data_msg.h"
#include "ds3geventmgr.h"
#include "ds707_data_session_profile.h"
#include "ds3gmgr.h"
#include "ds3gutil.h"
#include "ds3gcfgmgr.h"
#include "ds3gmshif.h"
#include "ds3gtimer.h"
#include "ds707_pkt_mgri.h"
#include "ds707_pkt_mgr.h"
#include "ds707_nv_util.h"
#include "ds707_pkt_mgr_hdlr.h"
#include "ds707_sys_chg_hdlr.h"
#include "ps_iface.h"
#include "ps_phys_link.h"
#include "ds_1x_profile.h"
#include "ds707_gonulltmr.h"
#include "dsutil.h"
#include "ds_profile_3gpp2_mgr.h"

#include <stringl/stringl.h>
#include "ds707_roaming.h"
#include "ds707_rmsm.h"
#include "dstaski.h"
#include "ps_ppp_auth.h"
#include "ps_ppp_lcp.h"
#include "ps_ppp_ipcp.h"
#include "ps_ppp_defs.h"
#include "dssdns.h"
#include "ps_aclrules.h"
#include "ps_ppp_config.h"
#include "dsat707mip.h"
#include "ds707_pdn_context.h"
#include "ds3gmgr.h"
#include "dstask_v.h"
#include "ds3gdsdif.h"
#ifdef FEATURE_DATA_CM_CENTRIC_EMERG_MODE
#include "ds3gcmif.h"
#endif
#include "hdrpac.h"

#ifdef FEATURE_CALL_THROTTLE
#include "dsdctm.h"
#endif /* FEATURE_CALL_THROTTLE */

/* lint fixes */
#include "data_err_suppr.h"

#ifdef FEATURE_DATA_PS_IPV6
#include "ps_iface_addr_v6.h"
#endif /* FEATURE_DATA_PS_IPV6 */

#include "ps_policy_mgr.h"
#include "ds707_event_defs.h"
#include "ds707_cback.h"
  #include "nvruimi.h"

#ifdef FEATURE_EHRPD
#include "ds707_sec_pkt_mgr_util.h"
#include "ds707_iface4_hdlr.h"
#include "dstaski.h"
#endif /* FEATURE_EHRPD*/

#ifdef FEATURE_DATA_OPTHO
  #include "ds707_s101.h"
#endif /* FEATURE_DATA_OPTHO */

#include "fs_public.h"
#include "fs_sys_types.h"
#include "fs_errno.h"
#include "modem_mem.h"
#include "mcfg_fs.h"

#ifdef FEATURE_DATA_WLAN_MAPCON
  #include "ds_mppm_ext_i.h"
#endif /*FEATURE_DATA_WLAN_MAPCON*/

#define DS707_PROFILE_MAX_PARAM_BUFF_SIZE 255 /* Max param buffer size*/
#define DS707_DATA_SESS_MAX_SUPPORTED_SUBS 3  /* Max supported subscriptions */

/* Macro to convert ident to mask */
#define CONVERT_ID_TO_MASK( ident ) \
 (uint64)( (uint64)1 << ident )


/*lint -save -e641 suppressing error 641*/
/*lint -save -e655 suppressing error 655*/
/*-------------------------------------------------------------------------- 
  Pointer to allocate memory for profiles
  Max number of profiles is calculated at power up, and memory is
  allocated accordingly.
  Max number of profiles reflect the number of actual allocated
  entries in the cache. It may change due to card event or newly
  created eHRPD profiles.
  These profiles are initialized with default values.
--------------------------------------------------------------------------*/
static ds707_data_session_profile_info_type 
         *ds707_data_session_profile_info = NULL;

static ds707_data_session_profile_info_type 
         *ds707_data_session_profile_info_secondary_subs = NULL;
static ds707_data_session_profile_info_type 
         *ds707_data_session_profile_info_tertiary_subs = NULL;

static ds707_data_session_profile_id 
  ds707_data_session_max_profiles[DS707_DATA_SESS_MAX_SUPPORTED_SUBS] = {0};

static ds707_min_apn_type ds707_mandatory_apn_list[DATA_SESSION_MAX_APN_NUM] = {{0}};

/*--------------------------------------------------------------------------
  Internal structure which contains information about the list of APNs read
  from the UICC card
--------------------------------------------------------------------------*/
static ds707_data_session_acl_info_type ds707_data_session_acl_info;

/*--------------------------------------------------------------------------
  If all the mandatory APNs are not enabled in the profile and in the card
  then set this boolean to FALSE. UE will not be allowed to attach to eHRPD
  if this is set to FALSE. And if nv minapnlist disallow call is set to TRUE,
  UE is not allowed to make any call.
--------------------------------------------------------------------------*/
static boolean ehrpd_attach_allowed;
/*-------------------------------------------------------------------------- 
  Records the NV value for minapnlist_disallow_call
--------------------------------------------------------------------------*/
static boolean ds707_data_sess_minapnlist_disallow_call = FALSE;

#define MAX_PROFILE_PARAM_NAME_SIZE   30 /* Max size of the param name in
                                            an EFS file */
#define MAX_NUM_OF_PROFILE_PARAMS     53 /* Max number of profile params */

/*---------------------------------------------------------------------------
  An array of strings to map the Parameter Name string (mentioned in EFS
  file for each profile) to a token ID. This token ID will be used to modify
  the correct entry in the internal profile registry.
  The token ID is the index of the string entry in the array.
---------------------------------------------------------------------------*/
static char profile_param_name_id[MAX_NUM_OF_PROFILE_PARAMS][MAX_PROFILE_PARAM_NAME_SIZE] =
                                      {"Negotiate_DNS_Server",
                                       "PPP_Session_Close_Timer_DO",
                                       "PPP_Session_Close_Timer_1X",
                                       "Allow_Linger",
                                       "LCP_Ack_Timeout",
                                       "IPCP_Ack_Timeout",
                                       "Auth_Timeout",
                                       "LCP_CReq_Retry_Count",
                                       "IPCP_CReq_Retry_Count",
                                       "Auth_Retry_Count",
                                       "Auth_Protocol",
                                       "PPP_User_Id",
                                       "PPP_Password",
                                       "Data_Rate",
                                       "Data_Mode",
                                       "App_Type",
                                       "App_Priority",
                                       "APN_String",
                                       "PDN_IP_Version_Type",
                                       "PCSCF_Addr_Needed",
                                       "Primary_DNS_Addr_V4",
                                       "Secondary_DNS_Addr_V4",
                                       "Primary_DNS_Addr_V6",
                                       "Secondary_DNS_Addr_V6",
                                       "RAN_Type",
                                       "PDN_Inactivity_Timeout",
                                       "APN_Enabled",
                                       "Failure_Timer_1",
                                       "Failure_Timer_2",
                                       "Failure_Timer_3",
                                       "Failure_Timer_4",
                                       "Failure_Timer_5",
                                       "Failure_Timer_6",
                                       "Disallow_Timer_1",
                                       "Disallow_Timer_2",
                                       "Disallow_Timer_3",
                                       "Disallow_Timer_4",
                                       "Disallow_Timer_5",
                                       "Disallow_Timer_6",
                                       "Linger_timeout_val",
                                       "APN_Class",
                                       "PDN_Level_Auth_Protocol",
                                       "PDN_Level_Auth_User_ID",
                                       "PDN_Level_Auth_Password",
                                       "PDN_Label",
                                       "OP_Reserved_PCO_ID",
                                       "MCC",
                                       "MNC",
                                       "User_App_Data",
                                       "IPV6_Delegation",
                                       "Subs_ID",
                                       "DNS_DHCP_Req_Flag",
                                       "PCSCF_DHCP_Req_Flag"};

/*--------------------------------------------------------------------------
  the iface that came up first and is still active. This iface ptr is
  maintained to to retrieve cdma data session profile id
--------------------------------------------------------------------------*/
static ps_iface_type *oldest_active_iface_ptr;

/*-------------------------------------------------------------------------- 
  Placeholder for valid no of NVRUIM profiles
 --------------------------------------------------------------------------*/
static uint8         num_valid_profiles = 0; 

/*-------------------------------------------------------------------------- 
  Number of valid eHRPD profiles created through profile DB.
 -------------------------------------------------------------------------*/
static uint8         ds707_data_session_ehrpd_valid_profile = 0; 

/*--------------------------------------------------------------------------
  ds707_active_nv_mip_profile is the active profile id specified in NV.
  This is read at power up in ds707_data_session_init()
  Need to revisit. This should be changed to reading the NV everytime it
  is required in compatibility checks
--------------------------------------------------------------------------*/
static int8 ds707_active_nv_mip_profile = -1;

/*--------------------------------------------------------------------------
  ds707_mip_profile_index is the index calculated from the MIP profile id
  specified at call bring up (MIP profile id range: 151-156). If any other
  profile is specified this index is same as the active mip profile in NV.
--------------------------------------------------------------------------*/
static int8 ds707_mip_profile_index     = -1;

static uint8 ds707_sip_active_profile_index = 0;  /*  SIP Active Profile Index   */

/*--------------------------------------------------------------------------
  struct to store default ppp values read from nv
--------------------------------------------------------------------------*/
static data_sess_profile_ppp_params_type ds707_default_ppp_params;
/*--------------------------------------------------------------------------
  To record if it is in emergency call only mode, this is used only for
  DS centric emergency mode control
--------------------------------------------------------------------------*/
static boolean ds707_is_ds_centric_emerg_only_mode = FALSE;
/*--------------------------------------------------------------------------
  To record if it is in an emergency call during the emergency only mode
--------------------------------------------------------------------------*/
static boolean ds707_emerg_call_in_emerg_only_mode = FALSE;

/*--------------------------------------------------------------------------
  This timer is started once an online status is detected during emergency
  only mode. If no emergency call is initiated during timer is running, system
  will be put back off line.
--------------------------------------------------------------------------*/
static rex_timer_type ds707_wait_for_emerg_call_timer = {0};

#ifdef FEATURE_DATA_CM_CENTRIC_EMERG_MODE
/*--------------------------------------------------------------------------
  To record if it is in emergency call only mode, this is used only for
  CM centric emergency mode control
--------------------------------------------------------------------------*/
static boolean ds707_is_cm_centric_emerg_only_mode = FALSE;

/*--------------------------------------------------------------------------
  mandatory APN check status: 1: check passed, 0: check failed
--------------------------------------------------------------------------*/
static uint8 mandatory_apn_check_status = 1;
#endif /*FEATURE_DATA_CM_CENTRIC_EMERG_MODE*/
/*--------------------------------------------------------------------------
  To record if cache memory for storing NVRUIM profile is allocated
--------------------------------------------------------------------------*/
static boolean ds707_is_nvruim_profile_allocated = FALSE;

/*---------------------------------------------------------------------------
    Critical section 
---------------------------------------------------------------------------*/
static rex_crit_sect_type ds707_profile_crit_sect;

/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

/*---------------------------------------------------------------------------
  Profile related path in EFS
---------------------------------------------------------------------------*/
#define PROFILE_DIR      "/application_specific_profiles"
#define PROFILE_PATH     "/application_specific_profiles/profile_000.txt"
#define PROFILE_TMP_PATH "/application_specific_profiles/profile_000.tmp"
#define PROFILE_PATH_LEN 47

/*---------------------------------------------------------------------------
  Default timer values (in seconds)
---------------------------------------------------------------------------*/
#define PDN_THROTTLE_DEF_TIMER_1      0
#define PDN_THROTTLE_DEF_TIMER_2      0
#define PDN_THROTTLE_DEF_TIMER_3      0
#define PDN_THROTTLE_DEF_TIMER_4      0
#define PDN_THROTTLE_DEF_TIMER_5      0
#define PDN_THROTTLE_DEF_TIMER_6      0

/*===========================================================================
MACRO MINUTE_TO_MSEC()

DESCRIPTION
  This macro returns a msec value for the give value in minutes

PARAMETERS
  int16 - value in minutes

RETURN VALUE
  value in msec
===========================================================================*/
#define MINUTE_TO_MSEC(val_minute)                        \
  ((val_minute) * 60000)


/*===========================================================================
MACRO MSEC_TO_MINUTE()

DESCRIPTION
  This macro returns a minute value for the given value in msec

PARAMETERS
  int16 - value in msec

RETURN VALUE
  value in minute
===========================================================================*/
#define MSEC_TO_MINUTE(val_msec)                        \
  ((val_msec) / 60000)

/*===========================================================================
MACRO MINUTE_TO_SECONDS()

DESCRIPTION
  This macro returns a number of seconds value for the given value
  in minutes

PARAMETERS
  int16 - value in minutes

RETURN VALUE
  value in seconds
===========================================================================*/
#define MINUTE_TO_SECONDS(val_minute) ((val_minute) * 60)

/*===========================================================================
MACRO SECONDS_TO_MINUTE()

DESCRIPTION
  This macro returns a number of minute value for the given value
  in seconds

PARAMETERS
  int16 - value in seconds

RETURN VALUE
  value in seconds
===========================================================================*/
#define SECONDS_TO_MINUTE(val_minute) ((val_minute) / 60)


/*===========================================================================
MACRO FAILURE_TIMER_IDX()

DESCRIPTION
  This macro returns an index value for the failure_timer array
  based on the token value

PARAMETERS
  timer - timer token's constant value

RETURN VALUE
  index to place timer
===========================================================================*/
#define FAILURE_TIMER_IDX(timer) (timer - FAILURE_TIMER_1)

/*===========================================================================
MACRO DISALLOW_TIMER_IDX()

DESCRIPTION
  This macro returns an index value for the disallow_timer array
  based on the token value

PARAMETERS
  timer - timer token's constant value

RETURN VALUE
  index to place timer
===========================================================================*/
#define DISALLOW_TIMER_IDX(timer) (timer - DISALLOW_TIMER_1)

/*===========================================================================
  Timer value for "wait for emergency call" timer, in milisecond
===========================================================================*/
#define WAIT_FOR_EMERG_CALL_DURATION 1000

/*===========================================================================
            FORWARD DECLARATIONS
===========================================================================*/
ds707_data_session_profile_id ds707_data_session_get_lbs_profile_id( void );
void ds707_data_sess_wait_for_emerg_timer_cb(uint32 timer_cntx);
#ifdef FEATURE_EHRPD
void ds707_data_sess_enter_emerg_only_mode(void);
void ds707_data_sess_exit_emerg_only_mode(void);
#endif /*FEATURE_EHRPD*/
void ds707_data_sess_set_nvruim_profile_allocated(boolean val);
boolean ds707_data_sess_is_nvruim_profile_allocated(void);
sys_sys_mode_e_type ds707_get_sys_pref_mode(
                      ds707_data_session_profile_info_type *data_sess_prof_ptr);


/*===========================================================================
                        INTERNAL FUNCTION DEFINTIONS
===========================================================================*/
/*===========================================================================
FUNCTION DS707_DATA_SESS_PROFILE_ALLOC

DESCRIPTION
  This function calculates the total number of profiles and allocates
  memory for the profiles.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ds707_data_sess_profile_alloc(void)
{
  ds707_data_session_profile_id ds707_data_session_max_profiles_temp = 0;

  /*-----------------------------------------------------------------------
    Calculate total number of profiles    
  -----------------------------------------------------------------------*/

  /*-----------------------------------------------------------------------
    Default profile
  -----------------------------------------------------------------------*/
  ds707_data_session_max_profiles_temp += DATA_SESSION_NO_DEFAULT_PROFILE;

  /*-----------------------------------------------------------------------
    JCDMA profiles. DATA_SESSION_INTERNATIONAL_ROAMING_PROFILE KDDI profile
    is always initialized irrespective of mode jcdma. So if jcdma is enabled
    there can be 22 max profiles else just add 1 to the max profiles for
    the international_roaming_profile.
  -----------------------------------------------------------------------*/
  if(ds707_roaming_is_curr_mode_jcdma()||
     ds707_roaming_is_simplified_jcdma_mode())
  {
    ds707_data_session_max_profiles_temp +=
                                      DATA_SESSION_JCDMA_NO_VALID_PROFILE;
  }
  else
  {
    ds707_data_session_max_profiles_temp += 1;
  }

#if 0
  /*-----------------------------------------------------------------------
    eHRPD profiles
  -----------------------------------------------------------------------*/
#ifdef FEATURE_EHRPD
  ds707_data_session_max_profiles_temp += DATA_SESSION_EHRPD_NO_VALID_PROFILE;
#endif

  /*--------------------------------------------------------------
   User created profiles
  ---------------------------------------------------------------*/
  ds707_data_session_max_profiles_temp += DATA_SESSION_USER_CREATED_MAX_PROFILES;
#endif

  /*------------------------------------------------------------------------
    NVRUIM profiles are read in ds707_read_data_session_profile_from_nvruim
    only in order to assign correct profile numbers
  -------------------------------------------------------------------------*/
  DATA_IS707_MSG2 (MSG_LEGACY_HIGH, 
                   "ds707_data_session_max_profiles[SYS_MODEM_AS_ID_1] = %d"
                   "ds707_data_session_max_profiles_temp = %d",
                   ds707_data_session_max_profiles[SYS_MODEM_AS_ID_1],
                   ds707_data_session_max_profiles_temp);
 
  DS_ENTER_CRIT_SECTION(&ds707_profile_crit_sect);

  ds707_data_session_max_profiles[SYS_MODEM_AS_ID_1] = 
                                      ds707_data_session_max_profiles_temp;
  /*----------------------------------------------------------------------
    Free the previously allocated profile cache
  -----------------------------------------------------------------*/
  if ( ds707_data_session_profile_info != NULL )
  {
    modem_mem_free(ds707_data_session_profile_info,MODEM_MEM_CLIENT_DATA_CRIT);
    ds707_data_session_profile_info = NULL;
    DATA_IS707_MSG0(MSG_LEGACY_MED,"Nv refresh, freeing prev allocated data" 
                                   "session profile cache");
  }

  /*-----------------------------------------------------------------------
    Allocate memory for the profiles
  -----------------------------------------------------------------------*/
  ds707_data_session_profile_info = 
    (ds707_data_session_profile_info_type *)modem_mem_alloc(
       ( ds707_data_session_max_profiles[SYS_MODEM_AS_ID_1] * 
           sizeof(ds707_data_session_profile_info_type) ), 
       MODEM_MEM_CLIENT_DATA_CRIT);

  if(NULL == ds707_data_session_profile_info )
  {
    ds707_data_sess_set_nvruim_profile_allocated(FALSE);
    DATA_ERR_FATAL("profile memory allocation failed");
    DS_LEAVE_CRIT_SECTION(&ds707_profile_crit_sect);
    return;
  }

  memset((void *)(ds707_data_session_profile_info), 0, 
         (ds707_data_session_max_profiles[SYS_MODEM_AS_ID_1] * 
            sizeof(ds707_data_session_profile_info_type)) );

  DS_LEAVE_CRIT_SECTION(&ds707_profile_crit_sect);
}

/*===========================================================================
FUNCTION DS707_DATA_SESSION_MAP_APNNAME_TO_LABEL

DESCRIPTION
  This function gets the corresponding PDN label based on APN name input
  from cached profile info
DEPENDENCIES
 None

RETURN VALUE
  matched PDN label length, if 0, no match is found

SIDE EFFECTS
  None
===========================================================================*/
static uint32 ds707_data_session_map_apnname_to_label(
        const char  *apn_name,
        const uint8 apn_len,
        char **label_pptr
        )
{
  ds707_data_session_profile_id profile_reg_index = 0;

  if(NULL == apn_name || 0 == apn_len || NULL == label_pptr)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                   "ds707_data_session_map_apnname_to_label, invalid input");
    return 0;
  }
  for ( profile_reg_index = DATA_SESSION_PROFILE_ID_MIN;
         profile_reg_index < ds707_data_session_max_profiles[SYS_MODEM_AS_ID_1];
         profile_reg_index++)
  {
    if ( apn_len == ds707_data_session_profile_info[profile_reg_index].apn_string_len )
    {
      if(0 == strncasecmp( ds707_data_session_profile_info[profile_reg_index].apn_string,
                  apn_name, apn_len ))
      {
	    if(ds707_data_session_profile_info[profile_reg_index].pdn_label_len != 0)
		{		
          *label_pptr = 
            ds707_data_session_profile_info[profile_reg_index].pdn_label;
          return ds707_data_session_profile_info[profile_reg_index].pdn_label_len;
		}
      }
    }
  }
  return 0;
 }

/*===========================================================================
FUNCTION ds707_data_session_policy_and_profile_match

DESCRIPTION
  This function matches PDN address family in the profile with the address
  family mentioned in the policy.

DEPENDENCIES

RETURN VALUE
      True - if the policy and profile addr family match
      False - if the policy and profile addr family dont match

SIDE EFFECTS

===========================================================================*/
boolean ds707_data_session_policy_and_profile_match
(
  ds707_data_session_profile_id profile_reg_index,
  acl_policy_info_type          *policy_info_ptr
)
{
  boolean ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( ( policy_info_ptr->ip_family == IFACE_UNSPEC_ADDR_FAMILY ) ||

       ( ds707_data_session_profile_info[profile_reg_index].pdn_type ==
         DS707_DATA_SESS_PDN_TYPE_V4_V6 ) ||

       ( ds707_data_session_profile_info[profile_reg_index].pdn_type ==
         DS707_DATA_SESS_PDN_TYPE_V4 &&
         policy_info_ptr->ip_family == IFACE_IPV4_ADDR_FAMILY ) ||

       ( ds707_data_session_profile_info[profile_reg_index].pdn_type ==
         DS707_DATA_SESS_PDN_TYPE_V6 &&
         policy_info_ptr->ip_family == IFACE_IPV6_ADDR_FAMILY )
     )
  {
    ret_val = TRUE;
  }
  else
  {
    DATA_IS707_MSG4( MSG_LEGACY_HIGH,
                   "Profile id %d PDN IP Version = %d and "
                   "policy IP version = %d match? %d",
                   ds707_data_session_profile_info[profile_reg_index].data_session_profile_id,
                   ds707_data_session_profile_info[profile_reg_index].pdn_type,
                   policy_info_ptr->ip_family, 
                   ret_val );
  }

  return ret_val;
}

/*===========================================================================
FUNCTION ds707_data_session_iface_and_profile_match

DESCRIPTION
  This function matches PDN address family in the profile with the address
  family of the IFACE. This is called only if the IFACE is up i.e.
  qualified as a V4 or a V6 IFACE.

DEPENDENCIES

RETURN VALUE
      True - if the IFACE and profile addr family match
      False - if the IFACE and profile addr family dont match

SIDE EFFECTS

===========================================================================*/
boolean ds707_data_session_iface_and_profile_match
(
  ds707_data_session_profile_id profile_reg_index,
  ps_iface_type                 *if_ptr
)
{
  if( ( ds707_data_session_profile_info[profile_reg_index].pdn_type ==
        DS707_DATA_SESS_PDN_TYPE_V4_V6 ) ||
      ( ds707_data_session_profile_info[profile_reg_index].pdn_type ==
        DS707_DATA_SESS_PDN_TYPE_V4 &&
        ps_iface_addr_family_is_v4(if_ptr) ) ||
      ( ds707_data_session_profile_info[profile_reg_index].pdn_type ==
        DS707_DATA_SESS_PDN_TYPE_V6 &&
        ps_iface_addr_family_is_v6(if_ptr) )
    )
  {
    return TRUE;
  }
  return FALSE;
}
/*===========================================================================
FUNCTION ds707_data_sess_profile_id_to_reg_index_mapping

DESCRIPTION
  This function takes application profile id as input and returns the actual
  index in the profile registry / database

DEPENDENCIES
  After profile re-arch changes, make sure this function is called only for
  non eHRPD profiles during boot up when profile ID hasn't been initiated.

RETURN VALUE
      -1 - if the given profile id is invalid
      index in the reg - if profile id is valid

SIDE EFFECTS

===========================================================================*/
ds707_data_session_profile_id ds707_data_sess_profile_id_to_reg_index_mapping
(
  ds707_data_session_profile_id profile_id
)
{
  ds707_data_session_profile_id        profile_reg_index = -1;
  uint8 ds707_data_session_jcdma_valid_profile = 0; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-----------------------------------------------------------------------
    default profile
   
    if jcdma enabled 22 profiles
    else 1 jcdma profile (international roaming)
   
    user created profile 10
   
    if ehrpd feature defined 7 profiles
    else 0
   
    SIP if supported 16 profiles
    else 0
   
    array size is max calculated depending on the conditions above in
    function  ds707_data_sess_profile_alloc() called at powerup
  -----------------------------------------------------------------------*/

  /*---------------------------------------------------------------------- 
    if JCDMA is enabled, max profiles is 22, else only 1 profile is
    valid (international roaming)
   
    if ehrpd feature is enabled, max profiles is 7, else 0
  -----------------------------------------------------------------------*/
  if(ds707_roaming_is_curr_mode_jcdma()|| 
     ds707_roaming_is_simplified_jcdma_mode())
  {
    ds707_data_session_jcdma_valid_profile = DATA_SESSION_JCDMA_NO_VALID_PROFILE;
  }
  else
  {
    ds707_data_session_jcdma_valid_profile = 1;
  }

  /*-----------------------------------------------------------------------
    For profile id 0, index is 0.
  -----------------------------------------------------------------------*/
  if (profile_id == 0)
  {
    profile_reg_index = 0;
  }

  /*-----------------------------------------------------------------------
    If profile id is JCDMA, if jcdma is enabled, the profile id is
    the index itself, if its disabled, only 1 profile is valid, so index
    is 1, 0th index is default profile
    This logic will change if more number of profiles are initialized even
    when jcdma is not enabled.
  -----------------------------------------------------------------------*/
  if ((DATA_SESSION_JCDMA_PROFILE_MIN <= profile_id) && (profile_id <= DATA_SESSION_JCDMA_PROFILE_MAX))
  {
    if (profile_id < (DATA_SESSION_JCDMA_PROFILE_MIN + DATA_SESSION_JCDMA_NO_VALID_PROFILE))
    {
      if(ds707_roaming_is_curr_mode_jcdma()|| 
         ds707_roaming_is_simplified_jcdma_mode())
      {
        profile_reg_index = profile_id;
      }
      else if( DATA_SESSION_INTERNATIONAL_ROAMING_PROFILE == profile_id )
      {
        profile_reg_index = 1;
      }
    }
  }

#if 0
  /*-----------------------------------------------------------------------
    If profile id is user created profile id, depending on the number of
    jcdma profiles, calculate the index
  -----------------------------------------------------------------------*/
  if (PROFILE_ID_IS_USER_CREATED(profile_id))
  {
    profile_reg_index = ((profile_id - DATA_SESSION_JCDMA_PROFILE_MAX)
                          + ds707_data_session_jcdma_valid_profile);
  }

  /*-----------------------------------------------------------------------
    If profile id is ehrpd id, depending on the number of jcdma profiles
    and user created profiles, calculate the index
  -----------------------------------------------------------------------*/
#ifdef FEATURE_EHRPD
  if ((DATA_SESSION_EHRPD_PROFILE_MIN <= profile_id) && (profile_id <= DATA_SESSION_EHRPD_PROFILE_MAX))
  {
    if (profile_id < (DATA_SESSION_EHRPD_PROFILE_MIN + DATA_SESSION_EHRPD_NO_VALID_PROFILE))
    {
      profile_reg_index = ((profile_id - DATA_SESSION_USER_CREATED_PROFILE_MAX)
                           + ds707_data_session_jcdma_valid_profile
                           + DATA_SESSION_USER_CREATED_MAX_PROFILES);
    }
  }
#endif
#endif /*if 0*/

  if ( (num_valid_profiles <= DATA_SESSION_NVRUIM_MAX_PROFILES) &&
       (DATA_SESSION_NVRUIM_PROFILE_MIN <= profile_id) &&
       (profile_id < (DATA_SESSION_NVRUIM_PROFILE_MIN + num_valid_profiles)) )
  {
    profile_reg_index = ( (profile_id - DATA_SESSION_NVRUIM_PROFILE_MIN + 1)
                           + ds707_data_session_jcdma_valid_profile
                           + ds707_data_session_ehrpd_valid_profile );
  }

  DATA_IS707_MSG2( MSG_LEGACY_HIGH,
         "Mapping Profile id %d to Profile Reg Index %d",
                   profile_id, profile_reg_index);

  return profile_reg_index;
} /* ds707_data_sess_profile_id_to_reg_index_mapping */

/*===========================================================================
FUNCTION      ds707_data_sess_get_token_id

DESCRIPTION   This function returns the token id associated with each
              profile parameter.

PARAMETERS    char *from - start of the token (param name)
              char *to   - end of the token (param name)

DEPENDENCIES  None

RETURN VALUE  0 - success
              1 - failure

SIDE EFFECTS  None
===========================================================================*/
int16 ds707_data_sess_get_token_id
(
  char *from,
  char *to,
  uint8 *token_id
)
{
  int16 ret_val = -1; /* return value */
  uint8 i;            /* counter for number of params */
  uint8 length;       /* length of the token (param name) */
 /*------------------------------------------------------------------------*/

  /* Get the length of the string that is passed */
  if  ( from > to )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,"Invalid Parameters ");
    return ret_val;
  }

  /* Assumption that the token length will not be greater than 255 */
  length =(uint8)(to-from);

  for (i=0; i<MAX_NUM_OF_PROFILE_PARAMS; i++)
  {
    if (length == strlen(profile_param_name_id[i]))
    {
      if (0 == strncasecmp(from,profile_param_name_id[i],length))
      {
        *token_id = i;
        ret_val = 0;
        break;
      }
    }
  }
  return ret_val;

}

/*===========================================================================
FUNCTION      DS707_FILL_DATA_SESS_PROFILE_WITH_TOKEN_CONTENTS

DESCRIPTION   This function gets the data for each token and populates the
              structure with the appropriate data that corresponds to the
              token number.

PARAMETERS    token_id -  describes the current token which is
                              being populated into the EFS structure

              char *from, *to - start and end of the character array which
                          holds the data to populate the profile structure

              ds707_data_session_profile_info_type * - pointer to the data
                          session profile structure that is being populated

              profile_id -  the profile id of the current
                            profile whose data is being populated

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_fill_data_sess_profile_with_token_contents(
  uint8 token_id,
  char *from,
  char *to,
  ds707_data_session_profile_info_type *data_sess_profile,
  ds707_data_session_profile_id profile_id
  )
{
  int                 atoi_result;
  uint8               length = 0;
  struct ps_in_addr   temp_in_addr;
  struct ps_in6_addr  temp_in6_addr;
  char                temp_array[DS_EFS_READ_BUFFER_SZ];
  int16               dss_errno = 0;
  uint8               idx;

  switch (token_id)
  {

    /* TOKEN #0: negotiate_dns_server */
    case NEGOTIATE_DNS_SERVER:
      if( *from == '0' )
      {
        data_sess_profile->negotiate_dns_server = FALSE;
        data_sess_profile->custom_val_mask |=
          CONVERT_ID_TO_MASK(token_id);
      }
      else if ( *from == '1' )
      {
        data_sess_profile->negotiate_dns_server = TRUE;
        data_sess_profile->custom_val_mask |=
          CONVERT_ID_TO_MASK(token_id);
      }
      else
      {
        DATA_IS707_MSG0( MSG_LEGACY_ERROR,
             "PARSE ERROR: At NEGOTIATE DNS SERVER. Proceed anyways" );
      }
      break;

    /* TOKEN #1: ppp_session_close_timer_DO */
    case PPP_SESSION_CLOSE_TIMER_DO:
    {
      DSUTIL_ATOI(from,to,atoi_result);
      data_sess_profile->ppp_session_close_timer_DO = (uint32)atoi_result;
      data_sess_profile->custom_val_mask |=
        CONVERT_ID_TO_MASK(token_id);
      break;
    }

    /* TOKEN #2: ppp_session_close_timer_1X*/
    case PPP_SESSION_CLOSE_TIMER_1X:
    {
      DSUTIL_ATOI(from,to,atoi_result);
      data_sess_profile->ppp_session_close_timer_1X = (uint32)atoi_result;
      data_sess_profile->custom_val_mask |=
        CONVERT_ID_TO_MASK(token_id);
      break;
    }


    /* TOKEN #3: allow_linger */
    case ALLOW_LINGER:
      if( *from == 'd' || *from == 'D' )
      {
        data_sess_profile->allow_linger = FALSE;
        data_sess_profile->custom_val_mask |=
          CONVERT_ID_TO_MASK(token_id);
      }
      else if ( *from == 'a' || *from == 'A' )
      {
        data_sess_profile->allow_linger = TRUE;
        data_sess_profile->custom_val_mask |=
          CONVERT_ID_TO_MASK(token_id);
      }
      else
      {
        DATA_IS707_MSG0( MSG_LEGACY_ERROR,
         "PARSE ERROR: At ALLOW LINGER. Proceed anyways" );
      }
      break;


    /* TOKEN #4: lcp_ack_timeout */
    case LCP_ACK_TIMEOUT_VAL:
    {
      DSUTIL_ATOI(from,to,atoi_result);
      data_sess_profile->lcp_ack_timeout = (uint16)atoi_result;
      data_sess_profile->custom_val_mask |=
        CONVERT_ID_TO_MASK(token_id);
      break;
    }


    /* TOKEN #5: ipcp_ack_timeout */
    case IPCP_ACK_TIMEOUT_VAL:
    {
      DSUTIL_ATOI(from,to,atoi_result);
      data_sess_profile->ipcp_ack_timeout = (uint16)atoi_result;
      data_sess_profile->custom_val_mask |=
        CONVERT_ID_TO_MASK(token_id);
      break;
    }

    /* TOKEN #6: auth_timeout */
    case AUTH_TIMEOUT_VAL:
    {
      DSUTIL_ATOI(from,to,atoi_result);
      data_sess_profile->auth_timeout = (uint16)atoi_result;
      data_sess_profile->custom_val_mask |=
        CONVERT_ID_TO_MASK(token_id);
      break;
    }


    /* TOKEN #7: lcp_creq_retry_count */
    case LCP_CREQ_RETRY_COUNT:
    {
      DSUTIL_ATOI(from,to,atoi_result);
      data_sess_profile->lcp_creq_retry_count = (uint8) atoi_result;
      data_sess_profile->custom_val_mask |=
        CONVERT_ID_TO_MASK(token_id);
      break;
    }

    /* TOKEN #8: ipcp_creq_retry_count */
    case IPCP_CREQ_RETRY_COUNT:
    {
      DSUTIL_ATOI(from,to,atoi_result);
      data_sess_profile->ipcp_creq_retry_count = (uint8) atoi_result;
      data_sess_profile->custom_val_mask |=
        CONVERT_ID_TO_MASK(token_id);
      break;
    }

    /* TOKEN #9: auth_retry_count */
    case AUTH_RETRY_COUNT:
    {
      DSUTIL_ATOI(from,to,atoi_result);
      data_sess_profile->auth_retry_count = (uint8) atoi_result;
      data_sess_profile->custom_val_mask |=
        CONVERT_ID_TO_MASK(token_id);
      break;
    }

    /* TOKEN #10: auth_protocol */
    case AUTH_PROTOCOL:
      length = to - from;
      if (0 == strncasecmp(from, "PAP", length))
      {
        data_sess_profile->auth_protocol = DATA_SESS_AUTH_PROTOCOL_PAP;
        data_sess_profile->custom_val_mask |=
          CONVERT_ID_TO_MASK(token_id);
      }
      else if ( 0 == strncasecmp(from, "CHAP", length) )
      {
        data_sess_profile->auth_protocol = DATA_SESS_AUTH_PROTOCOL_CHAP;
        data_sess_profile->custom_val_mask |=
          CONVERT_ID_TO_MASK(token_id);
      }
      else if ( 0 == strncasecmp(from, "PAP_CHAP", length) )
      {
        data_sess_profile->auth_protocol = DATA_SESS_AUTH_PROTOCOL_PAP_CHAP;
        data_sess_profile->custom_val_mask |=
          CONVERT_ID_TO_MASK(token_id);
      }
      else if ( 0 == strncasecmp(from, "EAP", length) )
      {
        data_sess_profile->auth_protocol = DATA_SESS_AUTH_PROTOCOL_EAP;
        data_sess_profile->custom_val_mask |=
          CONVERT_ID_TO_MASK(token_id);
      }
      else if ( 0 == strncasecmp(from, "NONE", length) )
      {
        data_sess_profile->auth_protocol = DATA_SESS_AUTH_PROTOCOL_NONE;
        data_sess_profile->custom_val_mask |=
          CONVERT_ID_TO_MASK(token_id);
      }
      else
      {
        DATA_IS707_MSG0( MSG_LEGACY_HIGH,
         "PARSE ERROR: At AUTH PROTOCOL. Proceed anyways." );
      }
      break;

    /* Token 11 - User ID */
    case PPP_USER_ID:
      {
        /*Note: 'to' points to (end of token+1)*/
        length = (uint8)((to-from)/sizeof(char));

        if ( (to > from) &&
             (PPP_MAX_USER_ID_LEN >= length)
           )
        {
          (void)memscpy(data_sess_profile->user_id,
                        PPP_MAX_USER_ID_LEN,
                        from,
                        (to-from));
          /*Donot Null terminate the string here. The profile
            can store just the string upto PPP_MAX_USER_ID_LEN
            and doesn't expect it to be NULL terminated. Any
            string operation on this param needs to cat NULL
            char before using it*/
          data_sess_profile->user_id_len = length;
          data_sess_profile->custom_val_mask |=
            CONVERT_ID_TO_MASK(token_id);
        }
        else
        {
          DATA_IS707_MSG0(MSG_LEGACY_ERROR,
            "PPP user id longer than max permissible length");
        }
        break;
      }

    /* Token 12 - Password */
    case PPP_PASSWORD:
      {
        /*Note: 'to' points to (end of token+1)*/
        length = (uint8)((to-from)/sizeof(char));

        if ( (to > from) &&
             (PPP_MAX_PASSWD_LEN >= length)
           )
        {
          (void)memscpy(data_sess_profile->auth_password,
                        PPP_MAX_PASSWD_LEN,
                        from,
                        (to-from));
          /*Donot Null terminate the string here. The profile
            can store just the string upto PPP_MAX_PASSWD_LEN
            and doesn't expect it to be NULL terminated. Any
            string operation on this param needs to cat NULL
            char before using it*/
          data_sess_profile->auth_password_len = length;
          data_sess_profile->custom_val_mask |=
            CONVERT_ID_TO_MASK(token_id);
        }
        else
        {
          DATA_IS707_MSG0(MSG_LEGACY_ERROR,
            "PPP password longer than max permissible length");
        }
        break;
      }

    /* TOKEN #13: data_rate */
    case DATA_RATE:
      if( *from == 'h' || *from == 'H')
      {
        data_sess_profile->data_rate = DS707_DATARATE_HIGH;
        data_sess_profile->custom_val_mask |=
          CONVERT_ID_TO_MASK(token_id);
      }
      else if ( *from == 'm' || *from == 'M' )
      {
        data_sess_profile->data_rate = DS707_DATARATE_MED;
        data_sess_profile->custom_val_mask |=
          CONVERT_ID_TO_MASK(token_id);
      }
      else if ( *from == 'l' || *from == 'L' )
      {
        data_sess_profile->data_rate = DS707_DATARATE_LOW;
        data_sess_profile->custom_val_mask |=
          CONVERT_ID_TO_MASK(token_id);
      }
      else
      {
        DATA_IS707_MSG0( MSG_LEGACY_ERROR,
             "PARSE ERROR: At DATA RATE. Proceed anyways." );
      }
      break;


    /* TOKEN #14: data_mode */
    case DATA_MODE:
      if( *from == 'h' || *from == 'H')
      {
        data_sess_profile->data_mode = DS707_DATAMODE_CDMA_HDR;
        data_sess_profile->custom_val_mask |=
          CONVERT_ID_TO_MASK(token_id);
      }
      else if ( *from == 'x' || *from == 'X' )
      {
        data_sess_profile->data_mode = DS707_DATAMODE_CDMA_ONLY;
        data_sess_profile->custom_val_mask |=
          CONVERT_ID_TO_MASK(token_id);
      }
      else if ( *from == 'd' || *from == 'D' )
      {
        data_sess_profile->data_mode = DS707_DATAMODE_HDR_ONLY;
        data_sess_profile->custom_val_mask |=
          CONVERT_ID_TO_MASK(token_id);
      }
      else
      {
        DATA_IS707_MSG0( MSG_LEGACY_ERROR,
         "PARSE ERROR: At DATA MODE. Proceed anyways." );
      }
      break;

    /* Token 15 - App type */
    case APP_TYPE:
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                        "ERR: APP_TYPE is Read Only. Skip parsing APP_TYPE");
        break;
      }
    /* Token 16 - App priority */
    case APP_PRIORITY:
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                        "ERR: APP_PRIORITY is Read Only. Skip parsing APP_PRIORITY");
        break;
      }

    /* Token 17 - Apn String */
    case APN_STRING:
      {

        /* Assumption that the token length will not be greater than 255 */
        /*Note: 'to' points to (end of token+1)*/

        length = (uint8)((to-from)/sizeof(char));
        if ( (from > to)  || (length > DS_VSNCP_3GPP2_APN_MAX_VAL_LEN))
        {
          data_sess_profile->apn_string_len = 0;
          DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                          "Error in APN String. Setting APN length to 0.");
        }
        else
        {
          (void)memscpy(data_sess_profile->apn_string,
			DS_VSNCP_3GPP2_APN_MAX_VAL_LEN,
                        from,
                        (to-from));
          /*Donot Null terminate the string here. The profile
            can store just the string upto DS_VSNCP_3GPP2_APN_MAX_VAL_LEN
            and doesn't expect it to be NULL terminated. Any
            string operation on this param needs to cat NULL
            char before using it*/
          data_sess_profile->apn_string_len = length;
          data_sess_profile->custom_val_mask |=
            CONVERT_ID_TO_MASK(token_id);
        }
        break;
      }
    /* Token 18 - PDN IP Version Type */
    case PDN_IP_VERSION_TYPE:
      {
        length = to - from;
        if (0 == strncasecmp(from, "V4", length))
        {
          data_sess_profile->pdn_type = DS707_DATA_SESS_PDN_TYPE_V4;
          data_sess_profile->custom_val_mask |=
            CONVERT_ID_TO_MASK(token_id);
        }
        else if (0 == strncasecmp(from, "V6", length))
        {
          data_sess_profile->pdn_type = DS707_DATA_SESS_PDN_TYPE_V6;
          data_sess_profile->custom_val_mask |=
            CONVERT_ID_TO_MASK(token_id);
        }
        else if (0 == strncasecmp(from, "V4_V6", length))
        {
          data_sess_profile->pdn_type = DS707_DATA_SESS_PDN_TYPE_V4_V6;
          data_sess_profile->custom_val_mask |=
            CONVERT_ID_TO_MASK(token_id);
        }
        /* pdn_type in profile cannot be UNSPEC, it defines what the PDN
           supports - v4, v6 or both */
        else
        {
          DATA_IS707_MSG0(MSG_LEGACY_ERROR,"Undefined PDN Type");
        }
        break;
      }
    /* Token 19 - PCSCF addr needed*/
    case PCSCF_ADDR_NEEDED:
      {
        if (*from == '0')
        {
          data_sess_profile->is_pcscf_addr_needed = FALSE;
          data_sess_profile->custom_val_mask |=
            CONVERT_ID_TO_MASK(token_id);
        }
        else if (*from == '1')
        {
          data_sess_profile->is_pcscf_addr_needed = TRUE;
          data_sess_profile->custom_val_mask |=
            CONVERT_ID_TO_MASK(token_id);
        }
        else
        {
          DATA_IS707_MSG0( MSG_LEGACY_ERROR,
               "PARSE ERROR: At PCSCF ADDR NEEDED. Proceed anyways" );
        }
        break;
      }
   case OP_PCO_ID:
     {
       length = (uint8)((to-from)/sizeof(char));
      if (DS707_DATA_SESS_OP_PCO_ID_LEN != length)
       {
         DATA_IS707_MSG0(MSG_LEGACY_ERROR,
             "Invalid PCO ID length" );
       }
      memset(temp_array, 0, DS_EFS_READ_BUFFER_SZ);
      memscpy(temp_array, DS_EFS_READ_BUFFER_SZ, from, length);
      data_sess_profile->op_pco_id = ds_util_hexatoi(temp_array);
         data_sess_profile->custom_val_mask |=
           CONVERT_ID_TO_MASK(token_id);
       break;
     }
   
    /* Primary V4 DNS Addr */
    case PRIMARY_DNS_ADDR_V4:
      {
        length = to - from;
        if ((length > 0) && (length < DS_EFS_READ_BUFFER_SZ))
        {
          /*-----------------------------------------------------------------
            strlcpy copies the string and adds '\0' in the end. This is
            the reason for copying length+1 characters.
          -----------------------------------------------------------------*/

          (void) strlcpy(temp_array,from,length+1);

          if (DSS_SUCCESS == dss_inet_pton(temp_array,
                                           DSS_AF_INET,
                                           &temp_in_addr,
                                           sizeof(struct ps_in_addr),
                                           &dss_errno ))
          {
            /* copy the address in the profile structure */
            (void) memscpy(&data_sess_profile->v4_dns_addr[PRIMARY_DNS_ADDR].ps_s_addr,
                           sizeof(struct ps_in_addr),
                           &temp_in_addr,
                           sizeof(struct ps_in_addr));
            data_sess_profile->custom_val_mask |=
              CONVERT_ID_TO_MASK(token_id);
          }
          else
          {
            DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                 "Can't set Primary V4 DNS Addr" );
          }

        }
        else
        {
          DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                 "Invalid token length for Primay V4 DNS Addr" );
        }
        break;
      }
    /* Secondary V4 DNS Addr */
    case SECONDARY_DNS_ADDR_V4:
      {
        length = to - from;
        if ((length > 0) && (length < DS_EFS_READ_BUFFER_SZ))
        {
          /*-----------------------------------------------------------------
            strlcpy copies the string and adds '\0' in the end. This is
            the reason for copying length+1 characters.
          -----------------------------------------------------------------*/
          (void) strlcpy(temp_array,from,length+1);

          if (DSS_SUCCESS == dss_inet_pton(temp_array,
                                           DSS_AF_INET,
                                           &temp_in_addr,
                                           sizeof(struct ps_in_addr),
                                           &dss_errno ))
          {
            /* copy the address in the profile structure */
            (void) memscpy(&data_sess_profile->v4_dns_addr[SECONDARY_DNS_ADDR].ps_s_addr,
                           sizeof(struct ps_in_addr),
                           &temp_in_addr,
                           sizeof(struct ps_in_addr));
            data_sess_profile->custom_val_mask |=
              CONVERT_ID_TO_MASK(token_id);
          }
          else
          {
            DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                 "Can't set Secondary V4 DNS Addr" );
          }
        }
        else
        {
          DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                 "Invalid token length for Secondary V4 DNS Addr" );
        }
        break;
      }
    /* Primary V6 DNS Addr */
    case PRIMARY_DNS_ADDR_V6:
      {
        length = to - from;
        if ((length > 0) && (length < DS_EFS_READ_BUFFER_SZ))
        {
          /*-----------------------------------------------------------------
            strlcpy copies the string and adds '\0' in the end. This is
            the reason for copying length+1 characters.
          -----------------------------------------------------------------*/
          (void) strlcpy(temp_array,from,length+1);

          if (DSS_SUCCESS == dss_inet_pton(temp_array,
                                           DSS_AF_INET6,
                                           &temp_in6_addr,
                                           sizeof(struct ps_in6_addr),
                                           &dss_errno ))
          {
            /* copy the address in the profile structure */
            (void) memscpy(&data_sess_profile->v6_dns_addr[PRIMARY_DNS_ADDR],
                           sizeof(struct ps_in6_addr),
                           &temp_in6_addr,
                           sizeof(struct ps_in6_addr));
            data_sess_profile->custom_val_mask |=
              CONVERT_ID_TO_MASK(token_id);
          }
          else
          {
            DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                 "Can't set Primary V6 DNS Addr" );
          }
        }
        else
        {
          DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                 "Invalid token length for Primary V6 DNS Addr" );
        }
        break;
      }

    /* Secondary V6 DNS Addr */
    case SECONDARY_DNS_ADDR_V6:
      {
        length = to - from;
        if ((length > 0) && (length < DS_EFS_READ_BUFFER_SZ))
        {
          /*-----------------------------------------------------------------
            strlcpy copies the string and adds '\0' in the end. This is
            the reason for copying length+1 characters.
          -----------------------------------------------------------------*/
          (void) strlcpy(temp_array,from,length+1);

          if (DSS_SUCCESS == dss_inet_pton(temp_array,
                                           DSS_AF_INET6,
                                           &temp_in6_addr,
                                           sizeof(struct ps_in6_addr),
                                           &dss_errno ))
          {
            /* copy the address in the profile structure */
            (void) memscpy(&data_sess_profile->v6_dns_addr[SECONDARY_DNS_ADDR],
                           sizeof(struct ps_in6_addr),
                           &temp_in6_addr,
                           sizeof(struct ps_in6_addr));
            data_sess_profile->custom_val_mask |=
              CONVERT_ID_TO_MASK(token_id);
          }
          else
          {
            DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                 "Can't set Secondary V6 DNS Addr" );
          }
        }
        else
        {
          DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                 "Invalid token length for Secondary V6 DNS Addr" );
        }
        break;
      }
    /* RAT Type */
    case RAN_TYPE:
      {
        length = to - from;
        if (length == strlen("HRPD"))
        {
          if (0 == strncmp(from, "HRPD", length))
          {
            data_sess_profile->rat_type = DS707_DATA_SESS_RAT_TYPE_HRPD;
            data_sess_profile->custom_val_mask |=
              CONVERT_ID_TO_MASK(token_id);
          }
        }
        else if (length == strlen("EHRPD"))
        {
          if (0 == strncmp(from, "EHRPD", length))
          {
            data_sess_profile->rat_type = DS707_DATA_SESS_RAT_TYPE_EHRPD;
            data_sess_profile->custom_val_mask |=
              CONVERT_ID_TO_MASK(token_id);
          }
        }
        else if (length == strlen("HRPD_EHRPD"))
        {
          if (0 == strncmp(from, "HRPD_EHRPD", length))
          {
            data_sess_profile->rat_type = DS707_DATA_SESS_RAT_TYPE_HRPD_EHRPD;
            data_sess_profile->custom_val_mask |=
              CONVERT_ID_TO_MASK(token_id);
          }
        }
        break;
      }

    /* PDN Inactivity Timeout */
    case PDN_INACTIVITY_TIMEOUT:
    {
      DSUTIL_ATOI(from,to,atoi_result);
      data_sess_profile->pdn_inactivity_timeout =
        MINUTE_TO_MSEC((uint16)atoi_result);
      data_sess_profile->custom_val_mask |=
        CONVERT_ID_TO_MASK(token_id);
      break;
    }

    /* APN enabled flag */
    case APN_ENABLED:
      {
        length = to - from;
        if (length == strlen("TRUE"))
        {
          if (0 == strncmp(from, "TRUE", length))
          {
            data_sess_profile->apn_flag = TRUE;
            data_sess_profile->custom_val_mask |=
              CONVERT_ID_TO_MASK(token_id);
          }
        }
        else if (length == strlen("FALSE"))
        {
          if (0 == strncmp(from, "FALSE", length))
          {
            data_sess_profile->apn_flag = FALSE;
            data_sess_profile->custom_val_mask |=
              CONVERT_ID_TO_MASK(token_id);
          }
        }
        break;
      }

    /* Failure_Timer_1 */
    case FAILURE_TIMER_1:
      {
        DSUTIL_ATOI(from,to,atoi_result);
        idx = FAILURE_TIMER_IDX(FAILURE_TIMER_1);
        data_sess_profile->failure_timer[idx] = (uint32) atoi_result;
        data_sess_profile->custom_val_mask |=
          CONVERT_ID_TO_MASK(token_id);
        break;
      }

    /* Failure_Timer_2 */
    case FAILURE_TIMER_2:
      {
        DSUTIL_ATOI(from,to,atoi_result);
        idx = FAILURE_TIMER_IDX(FAILURE_TIMER_2);
        data_sess_profile->failure_timer[idx] = (uint32) atoi_result;
        data_sess_profile->custom_val_mask |=
          CONVERT_ID_TO_MASK(token_id);
        break;
      }

    /* Failure_Timer_3 */
    case FAILURE_TIMER_3:
      {
        DSUTIL_ATOI(from,to,atoi_result);
        idx = FAILURE_TIMER_IDX(FAILURE_TIMER_3);
        data_sess_profile->failure_timer[idx] = (uint32) atoi_result;
        data_sess_profile->custom_val_mask |=
          CONVERT_ID_TO_MASK(token_id);
        break;
      }

    /* Failure_Timer_4 */
    case FAILURE_TIMER_4:
      {
        DSUTIL_ATOI(from,to,atoi_result);
        idx = FAILURE_TIMER_IDX(FAILURE_TIMER_4);
        data_sess_profile->failure_timer[idx] = (uint32) atoi_result;
        data_sess_profile->custom_val_mask |=
          CONVERT_ID_TO_MASK(token_id);
        break;
      }

    /* Failure_Timer_5 */
    case FAILURE_TIMER_5:
      {
        DSUTIL_ATOI(from,to,atoi_result);
        idx = FAILURE_TIMER_IDX(FAILURE_TIMER_5);
        data_sess_profile->failure_timer[idx] = (uint32) atoi_result;
        data_sess_profile->custom_val_mask |=
          CONVERT_ID_TO_MASK(token_id);
        break;
      }

    /* Failure_Timer_6 */
    case FAILURE_TIMER_6:
      {
        DSUTIL_ATOI(from,to,atoi_result);
        idx = FAILURE_TIMER_IDX(FAILURE_TIMER_6);
        data_sess_profile->failure_timer[idx] = (uint32) atoi_result;
        data_sess_profile->custom_val_mask |=
          CONVERT_ID_TO_MASK(token_id);
        break;
      }

    /* Disallow_Timer_1 */
    case DISALLOW_TIMER_1:
      {
        DSUTIL_ATOI(from,to,atoi_result);
        idx = DISALLOW_TIMER_IDX(DISALLOW_TIMER_1);
        data_sess_profile->disallow_timer[idx] = (uint32) atoi_result;
        data_sess_profile->custom_val_mask |=
          CONVERT_ID_TO_MASK(token_id);
        break;
      }

    /* Disallow_Timer_2 */
    case DISALLOW_TIMER_2:
      {
        DSUTIL_ATOI(from,to,atoi_result);
        idx = DISALLOW_TIMER_IDX(DISALLOW_TIMER_2);
        data_sess_profile->disallow_timer[idx] = (uint32) atoi_result;
        data_sess_profile->custom_val_mask |=
          CONVERT_ID_TO_MASK(token_id);
        break;
      }

    /* Disallow_Timer_3 */
    case DISALLOW_TIMER_3:
      {
        DSUTIL_ATOI(from,to,atoi_result);
        idx = DISALLOW_TIMER_IDX(DISALLOW_TIMER_3);
        data_sess_profile->disallow_timer[idx] = (uint32) atoi_result;
        data_sess_profile->custom_val_mask |=
          CONVERT_ID_TO_MASK(token_id);
        break;
      }

    /* Disallow_Timer_4 */
    case DISALLOW_TIMER_4:
      {
        DSUTIL_ATOI(from,to,atoi_result);
        idx = DISALLOW_TIMER_IDX(DISALLOW_TIMER_4);
        data_sess_profile->disallow_timer[idx] = (uint32) atoi_result;
        data_sess_profile->custom_val_mask |=
          CONVERT_ID_TO_MASK(token_id);
        break;
      }

    /* Disallow_Timer_5 */
    case DISALLOW_TIMER_5:
      {
        DSUTIL_ATOI(from,to,atoi_result);
        idx = DISALLOW_TIMER_IDX(DISALLOW_TIMER_5);
        data_sess_profile->disallow_timer[idx] = (uint32) atoi_result;
        data_sess_profile->custom_val_mask |=
          CONVERT_ID_TO_MASK(token_id);
        break;
      }

    /* Disallow_Timer_6 */
    case DISALLOW_TIMER_6:
      {
        DSUTIL_ATOI(from,to,atoi_result);
        idx = DISALLOW_TIMER_IDX(DISALLOW_TIMER_6);
        data_sess_profile->disallow_timer[idx] = (uint32) atoi_result;
        data_sess_profile->custom_val_mask |=
          CONVERT_ID_TO_MASK(token_id);
        break;
      }

    /* linger_timeout_val */
    case LINGER_TIMEOUT_VAL:
    {
      DSUTIL_ATOI(from,to,atoi_result);
      data_sess_profile->linger_timeout_val = (uint16)(atoi_result);
      data_sess_profile->custom_val_mask |=
        CONVERT_ID_TO_MASK(token_id);
      break;
    }

    case APN_CLASS:
      {
        DSUTIL_ATOI(from,to,atoi_result);
        data_sess_profile->apn_class = (uint8)atoi_result;
        data_sess_profile->custom_val_mask |=
          CONVERT_ID_TO_MASK(token_id);
        break;
      }

    /* PDN_Level_Auth_Protocol */
    case PDN_LEVEL_AUTH_PROTOCOL:
      {
        length = to - from;
        if ( 0 == strncasecmp(from, "PAP", length) )
        {
          data_sess_profile->pdn_level_auth_protocol = DATA_SESS_AUTH_PROTOCOL_PAP;
          data_sess_profile->custom_val_mask |=
            CONVERT_ID_TO_MASK(token_id);
        }
        else if ( 0 == strncasecmp(from, "CHAP", length) )
        {
          data_sess_profile->pdn_level_auth_protocol = DATA_SESS_AUTH_PROTOCOL_CHAP;
          data_sess_profile->custom_val_mask |=
            CONVERT_ID_TO_MASK(token_id);
        }
        else if ( 0 == strncasecmp(from, "NONE", length) )
        {
          data_sess_profile->pdn_level_auth_protocol = DATA_SESS_AUTH_PROTOCOL_NONE;
          data_sess_profile->custom_val_mask |=
            CONVERT_ID_TO_MASK(token_id);
        }
        else if ( 0 == strncasecmp(from, "PAP_CHAP", length) )
        {
          data_sess_profile->pdn_level_auth_protocol = DATA_SESS_AUTH_PROTOCOL_PAP_CHAP;
          data_sess_profile->custom_val_mask |=
            CONVERT_ID_TO_MASK(token_id);
        }
        else
        {
          DATA_IS707_MSG0( MSG_LEGACY_ERROR,
               "Invalid PDN Level Authentication Protocol Type. PDN Level Authentication "
               "will not occur.");
        }
        break;
      }

    /* PDN_Level_User_ID */
    case PDN_LEVEL_USER_ID:
      {
        /*Note: 'to' points to (end of token+1)*/
        length = (uint8)((to-from)/sizeof(char));

        if ( (to > from) &&
             (PPP_MAX_USER_ID_LEN >= length)
           )
        {
          (void)memscpy(data_sess_profile->pdn_level_user_id,
                        PPP_MAX_USER_ID_LEN,
                        from,
                        (to-from));

          data_sess_profile->pdn_level_user_id_len = length;
          data_sess_profile->custom_val_mask |=
               CONVERT_ID_TO_MASK(token_id);
        }
        else
        {
          DATA_IS707_MSG0(MSG_LEGACY_ERROR,
               "PDN level auth user id longer than max permissible length");
        }
        break;
      }

    /* PDN_Level_Auth_Password */
    case PDN_LEVEL_AUTH_PASSWORD:
      {
        /*Note: 'to' points to (end of token+1)*/
        length = (uint8)((to-from)/sizeof(char));

        if ( (to > from) &&
             (PPP_MAX_PASSWD_LEN >= length)
           )
        {
          (void)memscpy(data_sess_profile->pdn_level_auth_password,
                        PPP_MAX_PASSWD_LEN,
                        from,
                        (to-from));

          data_sess_profile->pdn_level_auth_password_len = length;
          data_sess_profile->custom_val_mask |=
               CONVERT_ID_TO_MASK(token_id);
        }
        else
        {
          DATA_IS707_MSG0(MSG_LEGACY_ERROR,
               "PDN level auth password longer than max permissible length");
        }
        break;
      }

    case PDN_LABEL:
      /* Assumption that the token length will not be greater than 255 */
      /*Note: 'to' points to (end of token+1)*/

      length = (uint8)((to-from)/sizeof(char));
      if ( (from > to)  || (length > DS_VSNCP_3GPP2_APN_MAX_VAL_LEN))
      {
        data_sess_profile->pdn_label_len = 0;
        DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                        "Error in PDN Label String. Setting pdn_label_len to 0.");
      }
      else
      {
        (void)memscpy(data_sess_profile->pdn_label,
                      DS_VSNCP_3GPP2_APN_MAX_VAL_LEN,
                      from,
                      (to-from));
        /*Donot Null terminate the string here. The profile
          can store just the string upto DS_VSNCP_3GPP2_APN_MAX_VAL_LEN
          and doesn't expect it to be NULL terminated. Any
          string operation on this param needs to cat NULL
          char before using it*/
        data_sess_profile->pdn_label_len = length;
        data_sess_profile->custom_val_mask |=
          CONVERT_ID_TO_MASK(token_id);
      }
      break;

    case MCC:
      length = (uint8)((to-from)/sizeof(char));
      if (MAX_MCC_DIGITS != length)
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR,
               "Invalid MCC length" );
        return;
      }
      DSUTIL_ATOI(from, to, atoi_result);
      data_sess_profile->mcc = atoi_result;
      data_sess_profile->custom_val_mask |=
          CONVERT_ID_TO_MASK(token_id);
      break;

    case MNC:
      length = (uint8)((to-from)/sizeof(char));
      if(MAX_MNC_DIGITS-1 == length)
      {
        data_sess_profile->mnc.mnc_includes_pcs_digit = FALSE;
      }
      else if(MAX_MNC_DIGITS == length)
      {
        data_sess_profile->mnc.mnc_includes_pcs_digit = TRUE;
      }
      else
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR,
               "Invalid MCC length" );
        return;
      }
      DSUTIL_ATOI(from, to, atoi_result);
      data_sess_profile->mnc.mnc = atoi_result;
      data_sess_profile->custom_val_mask |=
          CONVERT_ID_TO_MASK(token_id);
        break;

    case USER_APP_DATA:
      {
        DSUTIL_ATOI(from,to,atoi_result);
        data_sess_profile->user_app_data = (uint32)atoi_result;
        data_sess_profile->custom_val_mask |=
          CONVERT_ID_TO_MASK(token_id);
        break;
      }

    case DNS_DHCP_REQ_FLAG:
      {
        if (*from == '0')
        {
          data_sess_profile->dns_addr_using_dhcp = FALSE;
          data_sess_profile->custom_val_mask |=
            CONVERT_ID_TO_MASK(token_id);
        }
        else if (*from == '1')
        {
          data_sess_profile->dns_addr_using_dhcp = TRUE;
          data_sess_profile->custom_val_mask |=
            CONVERT_ID_TO_MASK(token_id);
        }
        else
        {
          DATA_IS707_MSG0( MSG_LEGACY_ERROR,
               "PARSE ERROR: At DNS_DHCP_REQ_FLAG. Proceed anyways" );
        }
        break;
      }

    case PCSCF_DHCP_REQ_FLAG:
      {
        if (*from == '0')
        {
          data_sess_profile->pcscf_addr_using_dhcp = FALSE;
          data_sess_profile->custom_val_mask |=
            CONVERT_ID_TO_MASK(token_id);
        }
        else if (*from == '1')
        {
          data_sess_profile->pcscf_addr_using_dhcp = TRUE;
          data_sess_profile->custom_val_mask |=
            CONVERT_ID_TO_MASK(token_id);
        }
        else
        {
          DATA_IS707_MSG0( MSG_LEGACY_ERROR,
               "PARSE ERROR: At PCSCF_DHCP_REQ_FLAG. Proceed anyways" );
        }
        break;
      }
  
    default:
      DATA_IS707_MSG0( MSG_LEGACY_ERROR,
       "ERROR:  Too many parameters. Proceed anyways" );
      break;

  } /* switch (token_number) */

} /* DS707_FILL_DATA_STRUCTURE_WITH_EFS_DATA() */

/*===========================================================================
FUNCTION      DS707_CREATE_EFS_FILE_NAME

DESCRIPTION   This function creates the file name that is to be read from efs
              based on the profile id that is passed to the function.

PARAMETERS    *efs_file_name -  pointer to the file name to which the profile
                                id value will be appended to.

              profile_id_value -  the profile id for which the file name is
                                  to be created.


DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_create_efs_file_name(
  char *efs_file_name,
  ds707_data_session_profile_id profile_id_value
  )
{
  ds707_data_session_profile_id i;
  /* variable that holds the 3 digit profile number as a string */
  char profile_id[4] = "000";

  if (profile_id_value < 1000)
  {
    for (i = 2; i >=0; i--)
    {
      profile_id[i] = (char) (profile_id_value % 10) + '0';
      profile_id_value /= 10;
    }
    /* appending the profile id values to the file name */
    efs_file_name[PROFILE_PATH_LEN - 8] = profile_id[0];
    efs_file_name[PROFILE_PATH_LEN - 7] = profile_id[1];
    efs_file_name[PROFILE_PATH_LEN - 6] = profile_id[2];

    DATA_IS707_MSG_SPRINTF_2(MSG_LEGACY_HIGH,
                             "Creating EFS file name for Profile id %d "
                             "EFS file name %s",
                             profile_id_value,
                             efs_file_name);
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Profile id value is greater than 1000");
  }

}/* ds707_create_efs_file_name() */

/*===========================================================================
FUNCTION      DS707_CREATE_PROFILE_FILE_IN_EFS

DESCRIPTION   For a given profile id, if the profile_xxx.txt is present in the
              efs, creates profile_xxx.tmp file and returns the file
              descriptor.Else creates profile_xxx.txt and returns the fd

PARAMETERS    profile_id         : profile number
              efs_file_name      : file path for profile_xxx.txt
              efs_tmp_file_name  : file path for profile_xxx.tmp
              create_tmp_profile : boolean to indicate if a tmp profile was
                                   created

DEPENDENCIES  None

RETURN VALUE  returns the File Descriptor for the created file

SIDE EFFECTS  None
===========================================================================*/
int ds707_create_profile_file_in_efs(
  char *efs_file_name
)
{
  int fd = -1; /* EFS File Descriptor*/
  int32 dir_result = -1;
  /*----------------------------------------------------------
       Confirm that dir: application_specific_profiles exists.
       If not, create one
  ----------------------------------------------------------*/
  dir_result = ds_path_is_directory(PROFILE_DIR);
  if( 0 != dir_result )
  {
    /* Directory doesn't exist yet */
    DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                    "Create /application_specific_profiles dir in EFS");
    if ( 0 != mcfg_mkdir(PROFILE_DIR,
                    S_IREAD|S_IWRITE|S_IEXEC,
                    MCFG_FS_TYPE_EFS))
    {
      DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                    "efs_mkdir Unable to create dir Err: %d",
                    mcfg_fs_errno(MCFG_FS_TYPE_EFS));
      return -1;
    }
  }

  /* Creating profile_xxx.txt. If there is one existing, it'll be
     opened and O_TRUNC ensures that the file contents are cleared*/
  fd = mcfg_fopen( efs_file_name,
                   MCFG_FS_O_CREAT | MCFG_FS_O_TRUNC | MCFG_FS_O_WRONLY,
                   MCFG_FS_DEFFILEMODE,
                   MCFG_FS_TYPE_EFS,
                   (mcfg_fs_sub_id_e_type)  ds707_curr_subs_id());
  if (-1 == fd)
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                    "EFS_Open Unable to create profile file Err: %d",
                    mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    return fd;
  }

  DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Profile file created successfully");

  return fd;
} /* ds707_create_profile_file_in_efs */

/*===========================================================================
FUNCTION      DS707_READ_DATA_SESSION_PROFILE_FROM_EFS

DESCRIPTION   This function will read from the efs file all the necessary
              data that will populate the EFS structure. If the parameters
              in the EFS file are not valid then these parameters are not
              populated in the profile registry. But we continue to read
              the rest of the parameters in the EFS file.

              EFS File Format

              Param_Name:Param_Val;

              For example -
              Profile_ID:100;

PARAMETERS    file_name - the name of the EFS file to read from

              efs_db - structure that contains info about the EFS file

              data_sess_profile - profile structure that is to be popualted

              profile_id - the profile id of the current
                           profile whose data is being populated

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_read_data_session_profile_from_efs(
  char *file_name,
  ds3gcfgmgr_efs_token_type  *efs_db,
  ds707_data_session_profile_info_type *data_sess_profile,
  ds707_data_session_profile_id profile_id
  )
{
  char *from, *to; /* ptrs to start and end of the token */
  ds3gcfgmgr_efs_token_parse_status_type  ret_val
                                              = DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS;
  boolean param_name = TRUE; /* flag to keep track of the parser state
                                TRUE - parser expects Token Name in the EFS
                                FALSE - parser expects Token Val in the EFS*/
  uint8 token_id = 0;

  /*-----------------------------------------------------------------------*/

  /* Set the seperator as : */
  efs_db->seperator = ':';
/*lint -save -esym(644,token_id) param_name boolean flag takes care of this */
  while (DS3GCFGMGR_EFS_TOKEN_PARSE_EOF
          != (ret_val = ds3gcfgmgr_efs_tokenizer(efs_db, &from, &to )))
  {
    /*------------------------------------------------------------------------
      Token being read. from points to the beginning of the token and
      to points to the (end of the token+1).

      e.g: string= "token;" => from points to 't' and to points to ';'

      The tokenizer automatically skips blank lines and comments (lines
      beginning with #, so no need to check for them here).
    ------------------------------------------------------------------------*/
    if((from == to) || (DS3GCFGMGR_EFS_TOKEN_PARSE_EOL == ret_val))
    {
      /*----------------------------------------------------------------------
        Skip empty tokens.
      ----------------------------------------------------------------------*/
      if (param_name == FALSE)
      {
        param_name = TRUE;
        efs_db->seperator = ':'; 
        DATA_IS707_MSG1( MSG_LEGACY_HIGH, "Token Id %d Param value NULL",
                         token_id);
      }
      continue;
    }
    else if(DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS == ret_val)
    {
      /*---------------------------------------------------------------------
      check if we are looking for param name or param value
      ---------------------------------------------------------------------*/
      if (param_name == TRUE)
      {
        /*------------------------------------------------------------------
        get the token identifier for this param name
        ------------------------------------------------------------------*/
        if (ds707_data_sess_get_token_id(from,to,&token_id) < 0)
        {
          /* This is an error scenario, Skip till the end of the line? */
          DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Incorrect Param Name" );
        }
        else
        {
          DATA_IS707_MSG1( MSG_LEGACY_HIGH, "Token Id: %d",token_id );
          /*-----------------------------------------------------------------
          set param_name as FALSE This means the next token is a
          param value
          -----------------------------------------------------------------*/
          param_name = FALSE;
          /* set the seperator as ; */
          efs_db->seperator = ';';
        }

      }
      /*---------------------------------------------------------------------
      This means that the token is a param value
      ---------------------------------------------------------------------*/
      else
      {
        /*-------------------------------------------------------------------
         pass in the identifier and param value to fill the profile data
         structure
         ------------------------------------------------------------------*/
        ds707_fill_data_sess_profile_with_token_contents( token_id,
                                                          from,to,
                                                          data_sess_profile,
                                                          profile_id
                                                          );
        /*-------------------------------------------------------------------
         set param_name as TRUE This means that next token is a param name
        -------------------------------------------------------------------*/
        param_name = TRUE;
        /* Set the seperator as : */
        efs_db->seperator = ':';
      }
    }
  } /* end of while loop */
/*lint -restore (644,token_id)*/
} /* DS707_READ_FROM_FILE() */

/*===========================================================================
FUNCTION      ds707_data_session_profile_set_ehrpd_params

DESCRIPTION   This function sets the eHRPD data session profile parameters
              for one specific profiles.

PARAMETERS    ds707_data_session_profile_id profile_reg_index
              char apn_string[DS_VSNCP_3GPP2_APN_MAX_VAL_LEN];
              uint8 apn_string_len;
              ds707_data_sess_pdn_type_enum_type pdn_type;
              boolean is_pcscf_addr_needed;
              struct in_addr v4_dns_addr[MAX_NUM_DNS_ADDR];
              struct in6_addr v6_dns_addr[MAX_NUM_DNS_ADDR];
              ds707_data_sess_rat_type_enum_type rat_type;



DEPENDENCIES  None

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_profile_set_ehrpd_params
(
  ds707_data_session_profile_id profile_reg_index,
  char *apn_string,
  uint8 apn_string_len,
  char *pdn_label,
  uint8 pdn_label_len,
  boolean apn_enabled,
  ds707_data_sess_pdn_type_enum_type pdn_type,
  boolean is_pcscf_addr_needed,
  uint16 op_pco_id,
  //struct in_addr v4_pri_dns_addr,
  //struct in_addr v4_sec_dns_addr,
  //struct in6_addr v6_pri_dns_addr,
  //struct in6_addr v6_sec_dns_addr,
  ds707_data_sess_rat_type_enum_type rat_type,
  uint32 *failure_timer_vals,
  uint32 *disallow_timer_vals,
  uint8  pdn_level_auth_protocol,
  char  *pdn_level_user_id,
  uint8  pdn_level_user_id_len,
  char  *pdn_level_auth_password,
  uint8  pdn_level_auth_password_len,
  uint16 mcc,
  uint16 mnc,
  boolean include_mnc_pcs_digit
)
{
  uint8 idx;

  if( profile_reg_index < DATA_SESSION_PROFILE_ID_MIN ||
      profile_reg_index >= ds707_data_session_max_profiles[SYS_MODEM_AS_ID_1] )
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR,
     "Incorrect profile reg %d value.", profile_reg_index );
    return;
  }

  if ((apn_string_len > 0) && (apn_string_len <= DS_VSNCP_3GPP2_APN_MAX_VAL_LEN))
  {
    ds707_data_session_profile_info[profile_reg_index].apn_string_len =
    apn_string_len;

    (void)memscpy(ds707_data_session_profile_info[profile_reg_index].apn_string,
                  DS_VSNCP_3GPP2_APN_MAX_VAL_LEN,
                  apn_string,
                  apn_string_len);
  }
  else
  {
    DATA_IS707_MSG2(MSG_LEGACY_HIGH,
        "Invalid APN string length %d for profile reg index:%d",
        apn_string_len,
        profile_reg_index);
    ds707_data_session_profile_info[profile_reg_index].apn_string_len = 0;
  }

  if ((pdn_label_len > 0) && (pdn_label_len <= DS_VSNCP_3GPP2_APN_MAX_VAL_LEN))
  {
    ds707_data_session_profile_info[profile_reg_index].pdn_label_len =
    pdn_label_len;

    (void)memscpy(ds707_data_session_profile_info[profile_reg_index].pdn_label,
                  DS_VSNCP_3GPP2_APN_MAX_VAL_LEN,
                  pdn_label,
                  pdn_label_len);
  }
  else
  {
    DATA_IS707_MSG2(MSG_LEGACY_HIGH,
        "Invalid PDN label length %d for profile reg index:%d",
        pdn_label_len,
        profile_reg_index);
    ds707_data_session_profile_info[profile_reg_index].pdn_label_len = 0;
  }

  ds707_data_session_profile_info[profile_reg_index].op_pco_id = op_pco_id;
  ds707_data_session_profile_info[profile_reg_index].mcc = mcc;
  ds707_data_session_profile_info[profile_reg_index].mnc.mnc = mnc;
  ds707_data_session_profile_info[profile_reg_index].mnc.mnc_includes_pcs_digit = include_mnc_pcs_digit;


  ds707_data_session_profile_info[profile_reg_index].apn_flag = apn_enabled;

  ds707_data_session_profile_info[profile_reg_index].pdn_type = pdn_type;
  ds707_data_session_profile_info[profile_reg_index].is_pcscf_addr_needed =
    is_pcscf_addr_needed;


  ds707_data_session_profile_info[profile_reg_index].v4_dns_addr[PRIMARY_DNS_ADDR].ps_s_addr = 0;
  ds707_data_session_profile_info[profile_reg_index].v4_dns_addr[SECONDARY_DNS_ADDR].ps_s_addr = 0;

  ds707_data_session_profile_info[profile_reg_index].v6_dns_addr[PRIMARY_DNS_ADDR].ps_s6_addr64[0] = 0;
  ds707_data_session_profile_info[profile_reg_index].v6_dns_addr[PRIMARY_DNS_ADDR].ps_s6_addr64[1] = 0;

  ds707_data_session_profile_info[profile_reg_index].v6_dns_addr[SECONDARY_DNS_ADDR].ps_s6_addr64[0] = 0;
  ds707_data_session_profile_info[profile_reg_index].v6_dns_addr[SECONDARY_DNS_ADDR].ps_s6_addr64[1] = 0;

  //ds707_data_session_profile_info[profile_reg_index].v4_dns_addr[PRIMARY_DNS_ADDR].s_addr =
   // v4_pri_dns_addr.s_addr;
  //ds707_data_session_profile_info[profile_reg_index].v4_dns_addr[SECENDARY_DNS_ADDR].s_addr =
   // v4_sec_dns_addr.s_addr;

  //ds707_data_session_profile_info[profile_reg_index].v6_dns_addr[PRIMARY_DNS_ADDR].s6_addr64[0] =
    //v6_pri_dns_addr.s6_addr64[0];
  //ds707_data_session_profile_info[profile_reg_index].v6_dns_addr[PRIMARY_DNS_ADDR].s6_addr64[1] =
   // v6_pri_dns_addr.s6_addr64[1];

  //ds707_data_session_profile_info[profile_reg_index].v6_dns_addr[SECENDORY_DNS_ADDR].s6_addr64[0] =
    //v6_sec_dns_addr.s6_addr64[0];
  //ds707_data_session_profile_info[profile_reg_index].v6_dns_addr[SECENDORY_DNS_ADDR].s6_addr64[1] =
   // v6_sec_dns_addr.s6_addr64[1];

  ds707_data_session_profile_info[profile_reg_index].rat_type = rat_type;
  ds707_data_session_profile_info[profile_reg_index].pdn_inactivity_timeout = 0;

  for(idx = 0; idx < PDN_THROTTLE_MAX_TIMERS; idx++)
  {
    ds707_data_session_profile_info[profile_reg_index].failure_timer[idx] = failure_timer_vals[idx];
    ds707_data_session_profile_info[profile_reg_index].disallow_timer[idx] = disallow_timer_vals[idx];
  }

  ds707_data_session_profile_info[profile_reg_index].apn_class = 0;

  ds707_data_session_profile_info[profile_reg_index].pdn_level_auth_protocol = 
        pdn_level_auth_protocol;

  if ((pdn_level_user_id_len > 0) && 
      (pdn_level_user_id_len <= PPP_MAX_USER_ID_LEN))
  {
    ds707_data_session_profile_info[profile_reg_index].pdn_level_user_id_len =
          pdn_level_user_id_len;

    (void)memscpy(ds707_data_session_profile_info[profile_reg_index].pdn_level_user_id,
                  PPP_MAX_USER_ID_LEN,
                  pdn_level_user_id,
                  pdn_level_user_id_len);
  }
  else
  {
    DATA_IS707_MSG2(MSG_LEGACY_HIGH,
        "Invalid PDN Level user id string length %d for profile reg index:%d",
        pdn_level_user_id_len,
        profile_reg_index);
    ds707_data_session_profile_info[profile_reg_index].pdn_level_user_id_len = 0;
  }

  if ((pdn_level_auth_password_len > 0) && 
      (pdn_level_auth_password_len <= PPP_MAX_PASSWD_LEN))
  {
    ds707_data_session_profile_info[profile_reg_index].pdn_level_auth_password_len =
          pdn_level_auth_password_len;

    (void)memscpy(ds707_data_session_profile_info[profile_reg_index].pdn_level_auth_password,
                  PPP_MAX_PASSWD_LEN,
                  pdn_level_auth_password,
                  pdn_level_auth_password_len);
  }
  else
  {
    DATA_IS707_MSG2(MSG_LEGACY_HIGH,
        "Invalid PDN Level password string length %d for profile reg index:%d",
        pdn_level_auth_password_len,
        profile_reg_index);
    ds707_data_session_profile_info[profile_reg_index].pdn_level_auth_password_len = 0;
  }

}

/*===========================================================================
FUNCTION      ds707_data_session_profile_set_common_params

DESCRIPTION   This function sets the common data session profile parameters
              for one specific profiles.

PARAMETERS    ds707_data_session_profile_id profile_id
              Boolean negotiate_dns_server - ON or OFF
              uint32 ppp_session_close_timer_DO
              uint32 ppp_session_close_timer_1X
              Boolean allow_linger - allowed or disallowed
              uint16 lcp_ack_timeout - lcp C-Req Retry Timer
              uint16 ipcp_ack_timeout - ipcp C-Req Retry Timer
              uint16 auth_timeout - PAP or CHAP Retry Timer

              uint8 lcp_creq_retry_count - number of retries
              uint8 ipcp_creq_retry_count - number of retrieS
              uint8  auth_retry_count - number of retries

              uint8 auth_protocol- PAP or CHAP
              uint8 data_rate- high, medium or low
              uint8 data_mode - hybrid or 1x


DEPENDENCIES  None

RETURN VALUE

SIDE EFFECTS  As session close timer will be set to go null timer when 
              originating a data call, and because fuction
              ds707_gonulltmr_set_timer use int16 as the timer value
              input, for preventing data overflow, should always use a
              value that is smaller than the maximum value of int16.
===========================================================================*/
void ds707_data_session_profile_set_common_params
(
  ds707_data_session_profile_id profile_reg_index,
  ds707_data_session_profile_id profile_id,
  boolean negotiate_dns_server,
  uint32 ppp_session_close_timer_DO,/*in second*/
  uint32 ppp_session_close_timer_1X,/*in second*/
  boolean allow_linger,
  uint16 lcp_ack_timeout,
  uint16 ipcp_ack_timeout,
  uint16 auth_timeout,
  uint8 lcp_creq_retry_count,
  uint8 ipcp_creq_retry_count,
  uint8 auth_retry_count,
  uint8 auth_protocol,
  uint8 data_rate,
  uint8 data_mode
)
{
  if( profile_reg_index < DATA_SESSION_PROFILE_ID_MIN ||
      profile_reg_index >= ds707_data_session_max_profiles[SYS_MODEM_AS_ID_1] )
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR,
                     "Incorrect profile reg index %d value.",
                     profile_reg_index );
    return;
  }

  /*---------------------------------------------------------------------------
    Set profile id
  ---------------------------------------------------------------------------*/
  ds707_data_session_profile_info[profile_reg_index].data_session_profile_id
    = profile_id;
  /*---------------------------------------------------------------------------
    set DNS server negotiation option
  ---------------------------------------------------------------------------*/
  ds707_data_session_profile_info[profile_reg_index].negotiate_dns_server
    = negotiate_dns_server;
  /*---------------------------------------------------------------------------
    set session close timer (i.e. go null timer) for DO and 1X
  ---------------------------------------------------------------------------*/
  ds707_data_session_profile_info[profile_reg_index].ppp_session_close_timer_DO
    = ppp_session_close_timer_DO;
  ds707_data_session_profile_info[profile_reg_index].ppp_session_close_timer_1X
    = ppp_session_close_timer_1X;
  /*---------------------------------------------------------------------------
    set Linger option (i.e. whether to allow Dormant 2 / Linger state or not)
  ---------------------------------------------------------------------------*/
  ds707_data_session_profile_info[profile_reg_index].allow_linger
    = allow_linger;
  /*---------------------------------------------------------------------------
    set PPP lcp and ipcp options
  ---------------------------------------------------------------------------*/
  ds707_data_session_profile_info[profile_reg_index].lcp_ack_timeout
    = (uint16)(lcp_ack_timeout);
  ds707_data_session_profile_info[profile_reg_index].ipcp_ack_timeout
    = (uint16)(ipcp_ack_timeout);
  ds707_data_session_profile_info[profile_reg_index].auth_timeout
    = (uint16)(auth_timeout);
  ds707_data_session_profile_info[profile_reg_index].lcp_creq_retry_count
    = lcp_creq_retry_count;
  ds707_data_session_profile_info[profile_reg_index].ipcp_creq_retry_count
    = ipcp_creq_retry_count;
  ds707_data_session_profile_info[profile_reg_index].auth_retry_count
    = auth_retry_count;
  ds707_data_session_profile_info[profile_reg_index].auth_protocol
    = auth_protocol;
  /*---------------------------------------------------------------------------
    set ppp user_id and password lengths as 0
  ---------------------------------------------------------------------------*/
  ds707_data_session_profile_info[profile_reg_index].user_id_len = 0;
  ds707_data_session_profile_info[profile_reg_index].auth_password_len = 0;
  /*---------------------------------------------------------------------------
    set phys link data rate and data mode options
  ---------------------------------------------------------------------------*/
  ds707_data_session_profile_info[profile_reg_index].data_rate
    = data_rate;
  ds707_data_session_profile_info[profile_reg_index].data_mode
    = data_mode;
  /*---------------------------------------------------------------------------
   set default values of stateless DHCP parameters
  ---------------------------------------------------------------------------*/
  ds707_data_session_profile_info[profile_reg_index].dns_addr_using_dhcp 
    = FALSE;
  ds707_data_session_profile_info[profile_reg_index].pcscf_addr_using_dhcp
    = FALSE; 

}

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_EHRPD_PROFILE_INIT

DESCRIPTION   This function initializes the profile parameters for
              eHRPD profiles.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_ehrpd_profile_init
(
  data_sess_profile_ppp_params_type         *ppp_params_ptr
)
{
  ds707_data_session_profile_id profile_id;
  ds707_data_session_profile_id profile_reg_index;
  ds707_data_sess_pdn_type_enum_type pdn_type = DS707_DATA_SESS_PDN_TYPE_V4;
  /*array to store default timer values*/
  uint32 default_timer_val[PDN_THROTTLE_MAX_TIMERS] = {
                                                  PDN_THROTTLE_DEF_TIMER_1,
                                                  PDN_THROTTLE_DEF_TIMER_2,
                                                  PDN_THROTTLE_DEF_TIMER_3,
                                                  PDN_THROTTLE_DEF_TIMER_4,
                                                  PDN_THROTTLE_DEF_TIMER_5,
                                                  PDN_THROTTLE_DEF_TIMER_6
                                                  };
/*-----------------------------------------------------------------------*/

  if (ppp_params_ptr == NULL)
  {
    DATA_ERR_FATAL("NULL ppp_params_ptr");
  }

  /*-------------------------------------------------------------------------
    Set the eHRPD parameters to some default values. Set the profiles as
    Common + EHRPD profiles. Now these profiles are valid profiles. If NULL
    APN is not allowed for these profiles then users must overwrite APN
    using EFS.
  -------------------------------------------------------------------------*/

  for (profile_id = DATA_SESSION_EHRPD_PROFILE_MIN;
       profile_id < (DATA_SESSION_EHRPD_PROFILE_MIN + DATA_SESSION_EHRPD_NO_VALID_PROFILE);
       profile_id++)
  {
    // get the actual profile id
    profile_reg_index = ds707_data_sess_profile_id_to_reg_index_mapping(profile_id);
    if (profile_reg_index > DATA_SESSION_PROFILE_ID_MIN)
    {
      /*Initialize all the common params with default values. Set the mask
      to common*/
      /*---------------------------------------------------------------------------
        call kddi_profile_init_element() for all the KDDI specific profiles
        ds707_data_session_profile_set_common_params(
                        ds707_data_session_profile_id profile_id
                        Boolean negotiate_dns_server - ON or OFF
                        uint32 ppp_session_close_timer_DO
                        uint32 ppp_session_close_timer_1X
                        Boolean allow_linger - allowed or disallowed
                        uint16 lcp_ack_timeout - lcp C-Req Retry Timer
                        uint16 ipcp_ack_timeout - ipcp C-Req Retry Timer
                        uint16 auth_timeout - PAP or CHAP Retry Timer
                        uint8 lcp_creq_retry_count - number of retries
                        uint8 ipcp_creq_retry_count - number of retrieS
                        uint8  auth_retry_count - number of retries
                        uint8 auth_protocol- PAP or CHAP
                        uint8 data_rate- high/med/low
                        uint8 data_mode - hybrid or 1x
                        )
      ---------------------------------------------------------------------------*/
      //todo remove this message if not needed
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, "Setting ehrpd profile - with dns negotiation - TRUE");
      ds707_data_session_profile_set_common_params(profile_reg_index,
                                                   profile_id,
                                                   TRUE,
                                                   ds707_gonulltmr_get_timer(DS707_GONULLTMRI_DO),
                                                   ds707_gonulltmr_get_timer(DS707_GONULLTMRI_1X),
                                                   FALSE,
                                                   ppp_params_ptr->lcp_ack_timeout,
                                                   ppp_params_ptr->ipcp_ack_timeout,
                                                   ppp_params_ptr->auth_timeout,
                                                   ppp_params_ptr->lcp_creq_retry_count,
                                                   ppp_params_ptr->ipcp_creq_retry_count,
                                                   ppp_params_ptr->auth_retry_count,
                                                   DATA_SESS_AUTH_PROTOCOL_EAP,
                                                   DS707_DATARATE_HIGH,
                                                   DS707_DATAMODE_CDMA_HDR);

      /*-------------------------------------------------------------------------
        Initialize all the eHRPD parameters with default values. Mark them
        as Common + EHRPD profiles even if they have NULL APN. If user wants to use
        non-NULL value for the APN then it must be programmed via EFS.
      ---------------------------------------------------------------------------*/
#ifdef FEATURE_EHRPD
#ifdef FEATURE_DATA_EHRPD_DUAL_IP
      pdn_type = DS707_DATA_SESS_PDN_TYPE_V4_V6;
#else
      pdn_type = DS707_DATA_SESS_PDN_TYPE_V4;
#endif /* FEATURE_DATA_EHRPD_DUAL_IP  */
#endif /* FEATURE_EHRPD */
      ds707_data_session_profile_set_ehrpd_params( profile_reg_index,
                                                   NULL,
                                                   0,
                                                   NULL,
                                                   0,
                                                   TRUE,
                                                   pdn_type,
                                                   FALSE,
                                                   0,
                                                   //0,0,
                                                   //0,0,
                                                   DS707_DATA_SESS_RAT_TYPE_HRPD_EHRPD,
                                                   default_timer_val,
                                                   default_timer_val,
                                                   0,
                                                   NULL, 0,
                                                   NULL, 0,
                                                   DS707_DATA_SESS_INITIAL_MCCMNC_VALUE,
                                                   DS707_DATA_SESS_INITIAL_MCCMNC_VALUE,
                                                   FALSE );


      ds707_data_session_profile_info[profile_reg_index].profile_type =
        (ds_profile_type_enum_type)( (uint32)PROFILE_COMMON | (uint32)PROFILE_EHRPD );

      /* Set the custom val mask to 0*/
      ds707_data_session_profile_info[profile_reg_index].custom_val_mask = 0;

      /* Set the persistence to be TRUE*/
      ds707_data_session_profile_info[profile_reg_index].is_persistent = TRUE;

      /* Set the opaque data to 0xFFFFFFFF */
      ds707_data_session_profile_info[profile_reg_index].user_app_data =
        0xFFFFFFFF;

      ds707_data_session_profile_info[profile_reg_index].clat_enabled = FALSE;

      ds707_data_session_profile_info[profile_reg_index].ipv6_delegation = FALSE;

      ds707_data_session_profile_info[profile_reg_index].subs_id = ACTIVE_SUBSCRIPTION_1;

      DATA_IS707_MSG3( MSG_LEGACY_HIGH,
      "For Profile id %d Profile Reg Index %d Setting Profile Type as %d",
          profile_id,
          profile_reg_index,
          ds707_data_session_profile_info[profile_reg_index].profile_type);
    }
    else
    {
      DATA_IS707_MSG2( MSG_LEGACY_ERROR,
      "Error Mapping Profile id %d, Profile Reg Index %d is invalid",
          profile_id,
          profile_reg_index);
    }
  } /*end of for loop*/
}

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_INTERNATIONAL_ROAMING_PROFILE_INIT

DESCRIPTION   This function initializes the profile parameters for
              KDDI International roaming profile.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_international_roaming_profile_init(void)
{
  /*
    call kddi_profile_init_element() for all the KDDI specific profiles
    ds707_data_session_profile_set_common_params(
                    ds707_data_session_profile_id_enum_type profile_id
                    Boolean negotiate_dns_server - ON or OFF
                    uint32 ppp_session_close_timer_DO
                    uint32 ppp_session_close_timer_1X
                    Boolean allow_linger - allowed or disallowed
                    uint16 lcp_ack_timeout - lcp C-Req Retry Timer
                    uint16 ipcp_ack_timeout - ipcp C-Req Retry Timer
                    uint16 auth_timeout - PAP or CHAP Retry Timer
                    uint8 lcp_creq_retry_count - number of retries
                    uint8 ipcp_creq_retry_count - number of retrieS
                    uint8  auth_retry_count - number of retries
                    ds707_ppp_auth_enum_type auth_protocol- PAP or CHAP
                    ds707_jcdma_datarate_1x_enum_type data_rate- high/med/low
                    ds707_jcdma_datamode_enum_type data_mode - hybrid or 1x
                    )

  */
  ds707_data_session_profile_id profile_reg_index = -1;

  profile_reg_index = ds707_data_sess_profile_id_to_reg_index_mapping(
     DATA_SESSION_INTERNATIONAL_ROAMING_PROFILE);
  if (profile_reg_index < DATA_SESSION_PROFILE_ID_MIN)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,"Invalid Profile ID passed");
    return;
  }

  ds707_data_session_profile_set_common_params(profile_reg_index,
                                               DATA_SESSION_INTERNATIONAL_ROAMING_PROFILE,
                                               FALSE, 0, 0, FALSE, 1000, 3000, 3000,
                                               20, 10, 10,
                                               DATA_SESS_AUTH_PROTOCOL_PAP_CHAP,
                                               DS707_DATARATE_HIGH,
                                               DS707_DATAMODE_CDMA_HDR);

} /* ds707_data_session_international_roaming_profile_init() */

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_KDDI_PROFILE_INIT_ALL

DESCRIPTION   This function initializes the data session profiles for
                KDDI specific profiles.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_kddi_profile_init_all(
  void
)
{
  ds707_data_session_profile_id profile_reg_index;
  ds707_data_session_profile_id profile_id;
  /*-----------------------------------------------------------------------*/

  /*---------------------------------------------------------------------------
    these values are hardcoded according to the parameters provided by KDDI.
    Later this ugly code should be replaced with some other reliable method
    (like reading these initial values also from EFS)
  ---------------------------------------------------------------------------*/
  /*---------------------------------------------------------------------------
    call kddi_profile_init_element() for all the KDDI specific profiles
    ds707_data_session_profile_set_common_params(
                    ds707_data_session_profile_id profile_id
                    Boolean negotiate_dns_server - ON or OFF
                    uint32 ppp_session_close_timer_DO
                    uint32 ppp_session_close_timer_1X
                    Boolean allow_linger - allowed or disallowed
                    uint16 lcp_ack_timeout - lcp C-Req Retry Timer
                    uint16 ipcp_ack_timeout - ipcp C-Req Retry Timer
                    uint16 auth_timeout - PAP or CHAP Retry Timer
                    uint8 lcp_creq_retry_count - number of retries
                    uint8 ipcp_creq_retry_count - number of retrieS
                    uint8  auth_retry_count - number of retries
                    uint8 auth_protocol- PAP or CHAP
                    uint8 data_rate- high/med/low
                    uint8 data_mode - hybrid or 1x
                    )
  ---------------------------------------------------------------------------*/
  ds707_data_session_profile_set_common_params(DATA_SESSION_JCDMA_INET_PROFILE,
                                               DATA_SESSION_JCDMA_INET_PROFILE,
                                               TRUE, 2700, 900, FALSE, 3000, 3000, 3000,
                                               10, 10, 10,
                                               DATA_SESS_AUTH_PROTOCOL_CHAP,
                                               DS707_DATARATE_HIGH,
                                               DS707_DATAMODE_CDMA_HDR);
  ds707_data_session_profile_set_common_params(DATA_SESSION_EMAIL_WITH_ATTACHMENT_PROFILE,
                                               DATA_SESSION_EMAIL_WITH_ATTACHMENT_PROFILE,
                                               FALSE, 900, 120, TRUE, 1000, 3000, 3000,
                                               20, 10, 10,
                                               DATA_SESS_AUTH_PROTOCOL_CHAP,
                                               DS707_DATARATE_HIGH,
                                               DS707_DATAMODE_CDMA_HDR);
  ds707_data_session_profile_set_common_params(DATA_SESSION_EMAIL_WITHOUT_ATTACHMENT_PROFILE,
                                               DATA_SESSION_EMAIL_WITHOUT_ATTACHMENT_PROFILE,
                                               FALSE, 900, 120, TRUE, 1000, 3000, 3000,
                                               20, 10, 10,
                                               DATA_SESS_AUTH_PROTOCOL_CHAP,
                                               DS707_DATARATE_MED,
                                               DS707_DATAMODE_CDMA_HDR);
  ds707_data_session_profile_set_common_params(DATA_SESSION_PSMS_PROFILE,
                                               DATA_SESSION_PSMS_PROFILE,
                                               FALSE,
                                               900, 120, TRUE, 1000, 3000, 3000,
                                               20, 10, 10,
                                               DATA_SESS_AUTH_PROTOCOL_CHAP,
                                               DS707_DATARATE_LOW,
                                               DS707_DATAMODE_CDMA_HDR);
  ds707_data_session_profile_set_common_params(DATA_SESSION_BREW_PCSV_PROFILE,
                                               DATA_SESSION_BREW_PCSV_PROFILE,
                                               FALSE, 900, 120, TRUE, 1000, 3000, 3000,
                                               20, 10, 10,
                                               DATA_SESS_AUTH_PROTOCOL_CHAP,
                                               DS707_DATARATE_HIGH,
                                               DS707_DATAMODE_CDMA_HDR);
  ds707_data_session_profile_set_common_params(DATA_SESSION_BREW_FA_PROFILE,
                                               DATA_SESSION_BREW_FA_PROFILE,
                                               FALSE, 900, 120, FALSE, 1000, 3000, 3000,
                                               20, 10, 10,
                                               DATA_SESS_AUTH_PROTOCOL_CHAP,
                                               DS707_DATARATE_HIGH,
                                               DS707_DATAMODE_CDMA_ONLY);
  ds707_data_session_profile_set_common_params(DATA_SESSION_BREW_OTHER_PROFILE,
                                               DATA_SESSION_BREW_OTHER_PROFILE,
                                               TRUE, 900, 120, FALSE, 1000, 3000, 3000,
                                               20, 10, 10,
                                               DATA_SESS_AUTH_PROTOCOL_CHAP,
                                               DS707_DATARATE_HIGH,
                                               DS707_DATAMODE_CDMA_HDR);
  ds707_data_session_profile_set_common_params(DATA_SESSION_BREW_IPVT_PROFILE,
                                               DATA_SESSION_BREW_IPVT_PROFILE,
                                               FALSE, 300, 300, FALSE, 1000, 3000, 3000,
                                               20, 10, 10,
                                               DATA_SESS_AUTH_PROTOCOL_CHAP,
                                               DS707_DATARATE_HIGH,
                                               DS707_DATAMODE_CDMA_HDR);
  ds707_data_session_profile_set_common_params(DATA_SESSION_BREW_PTM_PROFILE,
                                               DATA_SESSION_BREW_PTM_PROFILE,
                                               FALSE, 300, 300, FALSE, 1000, 3000, 3000,
                                               20, 10, 10,
                                               DATA_SESS_AUTH_PROTOCOL_CHAP,
                                               DS707_DATARATE_HIGH,
                                               DS707_DATAMODE_CDMA_HDR);
  ds707_data_session_profile_set_common_params(DATA_SESSION_OTA_PROFILE,
                                               DATA_SESSION_OTA_PROFILE,
                                               FALSE, 0, 120, FALSE, 1000, 3000, 3000,
                                               20, 10, 10,
                                               DATA_SESS_AUTH_PROTOCOL_CHAP,
                                               DS707_DATARATE_LOW,
                                               DS707_DATAMODE_CDMA_ONLY);
  ds707_data_session_profile_set_common_params(DATA_SESSION_UHM_PROFILE,
                                               DATA_SESSION_UHM_PROFILE,
                                               FALSE, 0, 0, FALSE, 1000, 3000, 3000,
                                               20, 10, 10,
                                               DATA_SESS_AUTH_PROTOCOL_CHAP,
                                               DS707_DATARATE_LOW,
                                               DS707_DATAMODE_CDMA_ONLY);
  ds707_data_session_profile_set_common_params(DATA_SESSION_KEITAI_UPDATE_PROFILE,
                                               DATA_SESSION_KEITAI_UPDATE_PROFILE,
                                               FALSE, 900, 120, FALSE, 1000, 3000, 3000,
                                                20, 10, 10,
                                               DATA_SESS_AUTH_PROTOCOL_CHAP,
                                               DS707_DATARATE_HIGH,
                                               DS707_DATAMODE_CDMA_HDR);
  ds707_data_session_profile_set_common_params(DATA_SESSION_WAP_FA_PROFILE,
                                               DATA_SESSION_WAP_FA_PROFILE,
                                               FALSE, 900, 120, FALSE, 1000, 3000, 3000,
                                                20, 10, 10,
                                               DATA_SESS_AUTH_PROTOCOL_CHAP,
                                               DS707_DATARATE_HIGH,
                                               DS707_DATAMODE_CDMA_ONLY);
  ds707_data_session_profile_set_common_params(DATA_SESSION_PSMS_CHAT_PROFILE,
                                               DATA_SESSION_PSMS_CHAT_PROFILE,
                                               FALSE, 900, 120, FALSE, 1000, 3000, 3000,
                                                20, 10, 10,
                                               DATA_SESS_AUTH_PROTOCOL_CHAP,
                                               DS707_DATARATE_LOW,
                                               DS707_DATAMODE_CDMA_ONLY);
  ds707_data_session_profile_set_common_params(DATA_SESSION_CORP_PROFILE,
                                               DATA_SESSION_CORP_PROFILE,
                                               TRUE, 900, 120, FALSE, 1000, 3000, 3000,
                                                20, 10, 10,
                                               DATA_SESS_AUTH_PROTOCOL_CHAP,
                                               DS707_DATARATE_HIGH,
                                               DS707_DATAMODE_CDMA_HDR);
  ds707_data_session_profile_set_common_params(DATA_SESSION_BML_PROFILE,
                                               DATA_SESSION_BML_PROFILE,
                                               FALSE, 900, 120, FALSE, 1000, 3000, 3000,
                                                20, 10, 10,
                                               DATA_SESS_AUTH_PROTOCOL_CHAP,
                                               DS707_DATARATE_HIGH,
                                               DS707_DATAMODE_CDMA_HDR);
  ds707_data_session_profile_set_common_params(DATA_SESSION_DEVICE_MANAGEMENT_PROFILE,
                                               DATA_SESSION_DEVICE_MANAGEMENT_PROFILE,
                                               TRUE, 900, 120, FALSE, 1000, 3000, 3000,
                                                20, 10, 10,
                                               DATA_SESS_AUTH_PROTOCOL_CHAP,
                                               DS707_DATARATE_HIGH,
                                               DS707_DATAMODE_CDMA_HDR);
  ds707_data_session_profile_set_common_params(DATA_SESSION_WINGP_WAP_PROFILE,
                                               DATA_SESSION_WINGP_WAP_PROFILE,
                                               FALSE, 0, 360, FALSE, 1000, 3000, 3000,
                                                20, 10, 10,
                                               DATA_SESS_AUTH_PROTOCOL_CHAP,
                                               DS707_DATARATE_HIGH,
                                               DS707_DATAMODE_CDMA_ONLY);
  ds707_data_session_profile_set_common_params(DATA_SESSION_WINGP_EMAIL_PROFILE,
                                               DATA_SESSION_WINGP_EMAIL_PROFILE,
                                               FALSE, 0, 360, FALSE, 1000, 3000, 3000,
                                                20, 10, 10,
                                               DATA_SESS_AUTH_PROTOCOL_CHAP,
                                               DS707_DATARATE_HIGH,
                                               DS707_DATAMODE_CDMA_ONLY);
  ds707_data_session_profile_set_common_params(DATA_SESSION_WINGP_PCSV_PROFILE,
                                               DATA_SESSION_WINGP_PCSV_PROFILE,
                                               FALSE, 0, 360, FALSE, 1000, 3000, 3000,
                                                20, 10, 10,
                                               DATA_SESS_AUTH_PROTOCOL_CHAP,
                                               DS707_DATARATE_HIGH,
                                               DS707_DATAMODE_CDMA_ONLY);
  ds707_data_session_profile_set_common_params(DATA_SESSION_WINGP_PSMS_PROFILE,
                                               DATA_SESSION_WINGP_PSMS_PROFILE,
                                               FALSE, 0, 360, FALSE, 1000, 3000, 3000,
                                                20, 10, 10,
                                               DATA_SESS_AUTH_PROTOCOL_CHAP,
                                               DS707_DATARATE_HIGH,
                                               DS707_DATAMODE_CDMA_ONLY);

  // Set the validity mask to common + KDDI
  for (profile_id = DATA_SESSION_JCDMA_PROFILE_MIN;
        profile_id <= DATA_SESSION_JCDMA_NO_VALID_PROFILE;
        profile_id++)
  {
    // get the profile reg index
    profile_reg_index = ds707_data_sess_profile_id_to_reg_index_mapping(profile_id);

    if (profile_reg_index > DATA_SESSION_PROFILE_ID_MIN)
    {
      // set the validity mask to common + KDDI
      ds707_data_session_profile_info[profile_reg_index].profile_type =
        (ds_profile_type_enum_type)( (uint32)PROFILE_COMMON | (uint32)PROFILE_KDDI );

      /* Profile DB internal params*/
      ds707_data_session_profile_info[profile_reg_index].supported_tech_mask =
        TECH_MASK_3GPP2;

      ds707_data_session_profile_info[profile_reg_index].ip_version[PRIMARY_DNS_ADDR] 
        = V4V6;

      ds707_data_session_profile_info[profile_reg_index].ip_version[SECONDARY_DNS_ADDR] 
        = V4V6;

      ds707_data_session_profile_info[profile_reg_index].is_linger_params_valid 
        = FALSE;

      /* Set the custom val mask to 0*/
      ds707_data_session_profile_info[profile_reg_index].custom_val_mask = 0;

      /* Set the persistence to be TRUE*/
      ds707_data_session_profile_info[profile_reg_index].is_persistent = TRUE;

      /* Set the opaque data to 0xFFFFFFFF */
      ds707_data_session_profile_info[profile_reg_index].user_app_data =
        0xFFFFFFFF;

      ds707_data_session_profile_info[profile_reg_index].clat_enabled = FALSE;

      ds707_data_session_profile_info[profile_reg_index].ipv6_delegation = FALSE;

      ds707_data_session_profile_info[profile_reg_index].subs_id = ACTIVE_SUBSCRIPTION_1;
    }

  }


}
#if 0
/*===========================================================================
FUNCTION      DS707_DATA_SESS_IS_IFACE_AVAILABLE

DESCRIPTION   This function checks if the iface is available to use or not.


PARAMETERS    ps_iface_type*        - pointer to the iface
              acl_policy_info_type* - pointer to policy information

DEPENDENCIES  None

RETURN VALUE  TRUE : Interface is available
                     (either idle or in use but shareable)
              FALSE: Interface is NOT available (in use and not shareable)

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_sess_is_iface_available
(
  ps_iface_type        *if_ptr,
  acl_policy_info_type *policy_info_ptr
)
{
  /*-------------------------------------------------------------------------
    The following rules determine if the iface is available/not
    - if interface is not in use, it is available
    - if interface is in use by a TE2 return FALSE
    - if interface is in use by sockets & routing is also done for
      sockets, then iface is available : return TRUE
    - if interface is in use by TE2 & routing is done by sockets,
      iface is only available for routeable entities (which can be routed
      in presence of TE2 calls), return FALSE for all others
  -------------------------------------------------------------------------*/
  /* lint fix */
  DATA_MDM_ARG_NOT_CONST(if_ptr);
  DATA_MDM_ARG_NOT_CONST(policy_info_ptr);
  /* iface not in use hence available */
  if(!PS_IFACE_IS_IN_USE(if_ptr))
  {
    return TRUE;
  }
  /* iface in use, not available for TE2 */
  else if(!policy_info_ptr->is_sock_orig)
  {
    return FALSE;
  }
  /* iface in use but shareable, hence available for sockets */
  else if(PS_IFACE_IS_SHAREABLE(if_ptr))
  {
    return TRUE;
  }
  /* iface in use by TE2 but shreable by routeable sockets */
  else if(policy_info_ptr->is_routeable)
  {
    return TRUE;
  }
  /* iface in use by TE2 and cannot be shared by non routeable sockets */
  return FALSE;
}/* ds707_data_sess_is_iface_available */
#endif

/*===========================================================================
FUNCTION      DS707_GET_PPP_DEFAULT_DEV_CONSTANT

DESCRIPTION

PARAMETERS

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_get_ppp_default_dev_constant
(
  ds707_data_session_profile_info_type *ds707_data_sess_profile
)
{
  uint32 value = 0;

  /*------------------------------------------------------------------------
    Set LCP ack timeout and retry count values.
  ------------------------------------------------------------------------*/
  if ( ppp_get_device_constant(PPP_UM_SN_DEV, DEFAULT_LCP_ACK_TIMEOUT, &value)
       == 0 )
  {
    ds707_data_sess_profile->lcp_ack_timeout = (uint16)(value);
  }
  else
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                     "Could not get default lcp ack timeout value" );
  }

  if ( ppp_get_device_constant( PPP_UM_SN_DEV, DEFAULT_LCP_REQUEST_TRY, &value)
       == 0)
  {
    ds707_data_sess_profile->lcp_creq_retry_count = (uint8) value;
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Could not get default lcp creq retry count");
  }

  /*------------------------------------------------------------------------
    Set IPCP ack timeout and retry count values.
  ------------------------------------------------------------------------*/
  if ( ppp_get_device_constant(PPP_UM_SN_DEV, DEFAULT_IPCP_ACK_TIMEOUT, &value)
       == 0 )
  {
    ds707_data_sess_profile->ipcp_ack_timeout = (uint16)(value);
  }
  else
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                     "Could not get default ipcp ack timeout value" );
  }

  if ( ppp_get_device_constant(PPP_UM_SN_DEV, DEFAULT_IPCP_REQUEST_TRY, &value)
       == 0 )
  {
    ds707_data_sess_profile->ipcp_creq_retry_count = (uint8) value;
  }
  else
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                     "Could not get default ipcp creq retry count" );
  }

  /*-----------------------------------------------------------------------
    Set Auth timeout and retry count values.
  ------------------------------------------------------------------------*/
  if ( ppp_get_device_constant(PPP_UM_SN_DEV, DEFAULT_AUTH_TIMEOUT, &value)
       == 0)
  {
    ds707_data_sess_profile->auth_timeout = (uint16)(value);
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Could not get default auth timeout value");
  }

  if ( ppp_get_device_constant(PPP_UM_SN_DEV, DEFAULT_AUTH_RETRY, &value) == 0 )
  {
    ds707_data_sess_profile->auth_retry_count = (uint8) value;
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Could not get default auth retry count");
  }
} /* ds707_get_ppp_default_dev_constant */

/*===========================================================================
FUNCTION ds707_data_sess_profile_set_invalid

DESCRIPTION
  This function sets all the profiles as invalid

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void ds707_data_sess_profile_set_invalid(void)
{
  ds707_data_session_profile_id profile_reg_index;

  for (profile_reg_index = DATA_SESSION_PROFILE_ID_MIN;
        profile_reg_index < ds707_data_session_max_profiles[SYS_MODEM_AS_ID_1];
        profile_reg_index++)
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH,
         "Setting all profiles to INVALID" );

    ds707_data_session_profile_info[profile_reg_index].data_session_profile_id =
      DATA_SESSION_PROFILE_ID_INVALID;
    ds707_data_session_profile_info[profile_reg_index].profile_type =
      PROFILE_INVALID;
  }
}

/*===========================================================================
FUNCTION ds707_data_sess_profile_set_valid

DESCRIPTION
  This function sets the correct profile type

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void ds707_data_sess_profile_set_valid(
  ds707_data_session_profile_id profile_reg_index
)
{

  if (profile_reg_index <= DATA_SESSION_PROFILE_ID_MIN)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,"Invalid Profile passed");
    return;
  }

  /*set the validity mask as common profile*/
  ds707_data_session_profile_info[profile_reg_index].profile_type =
    PROFILE_COMMON;

  /*set the validity mask as KDDI profile if current mode is KDDI */
  if (ds707_roaming_is_curr_mode_jcdma()||
      ds707_roaming_is_simplified_jcdma_mode())
  {
    ds707_data_session_profile_info[profile_reg_index].profile_type |=
      PROFILE_KDDI;
  }

#ifdef FEATURE_EHRPD
  /*OR the validity mask with ehrpd */
  ds707_data_session_profile_info[profile_reg_index].profile_type |=
    PROFILE_EHRPD;
#endif

} /* ds707_data_sess_profile_set_valid */
/*===========================================================================
FUNCTION ds707_data_sess_profile_get_default_ppp_opts

DESCRIPTION
  This function reads all the PPP values from NV

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void ds707_data_sess_profile_get_default_ppp_opts
(
  data_sess_profile_ppp_params_type *ppp_params_ptr
)
{
  nv_item_type          ppp_nv_item;


  if (ppp_params_ptr == NULL)
  {
    DATA_ERR_FATAL("NULL ppp_params_ptr");
  }

  DATA_IS707_MSG0( MSG_LEGACY_HIGH,
         "Read default PPP values from NV" );
  /*-----------------------------------------------------------------------
    Initialize LCP and IPCP device constants
  -----------------------------------------------------------------------*/
  ppp_nv_item.ppp_config_data.index = (uint8)PPP_UM_SN_DEV;
  if( NV_DONE_S == ds3gcfgmgr_read_legacy_nv_ex( NV_PPP_CONFIG_DATA_I, &ppp_nv_item, ds707_curr_subs_id() ) )
  {
    // set the values for lcp,ipcp,auth retry and count

    ppp_params_ptr->lcp_ack_timeout =
      ppp_nv_item.ppp_config_data.lcp_ack_timeout;
    ppp_params_ptr->lcp_creq_retry_count =
      ppp_nv_item.ppp_config_data.lcp_req_try;

    ppp_params_ptr->auth_retry_count =
      ppp_nv_item.ppp_config_data.auth_retry;
    ppp_params_ptr->auth_timeout =
      ppp_nv_item.ppp_config_data.auth_timeout;

    ppp_params_ptr->ipcp_ack_timeout =
      ppp_nv_item.ppp_config_data.ipcp_ack_timeout;
    ppp_params_ptr->ipcp_creq_retry_count =
      ppp_nv_item.ppp_config_data.ipcp_req_try;

    ppp_params_ptr->ip6cp_ack_timeout =
      ppp_nv_item.ppp_config_data.ipv6cp_ack_timeout;
    ppp_params_ptr->ip6cp_creq_retry_count =
      ppp_nv_item.ppp_config_data.ipv6cp_req_try;

  }
  else
  {
    /*---------------------------------------------------------------------
     Set the NV item to default values for all its components.
    ---------------------------------------------------------------------*/
    DATA_IS707_MSG0( MSG_LEGACY_HIGH,
         "Can't read default PPP values from NV, reading hardcoded values" );

    ppp_params_ptr->lcp_ack_timeout           = LCP_TIMEOUT;
    ppp_params_ptr->lcp_creq_retry_count      = LCP_REQ_TRY;

    ppp_params_ptr->auth_retry_count          = PPP_AUTH_RETRY_COUNTER_VAL;
    ppp_params_ptr->auth_timeout              = PPP_AUTH_TIMER_VAL;

    ppp_params_ptr->ipcp_ack_timeout          = IPCP_TIMEOUT;
    ppp_params_ptr->ipcp_creq_retry_count     = IPCP_REQ_TRY;

    ppp_params_ptr->ip6cp_ack_timeout         = IPV6CP_TIMEOUT;
    ppp_params_ptr->ip6cp_creq_retry_count    = IPV6CP_REQ_TRY;

  } /* end else */
}

/*===========================================================================
FUNCTION      ds707_data_session_set_default_param

DESCRIPTION   This function is called to set the passed profile with default
              values for all params. This function also takes default ppp params
              as input and sets it in the profile.

PARAMETERS    ds707_data_session_profile_id
              data_sess_profile_ppp_params_type *

DEPENDENCIES  None

RETURN VALUE  TRUE: if default values are set properly
              FALSE: otherwise.

SIDE EFFECTS  None
===========================================================================*/

boolean ds707_data_session_set_default_param
(
  ds707_data_session_profile_id profile_id,
  data_sess_profile_ppp_params_type *ppp_params_ptr
)
{
  ds707_data_session_profile_id profile_reg_index = -1;
  /*array to store default timer values*/
  uint32 default_timer_val[PDN_THROTTLE_MAX_TIMERS] = {
                                                  PDN_THROTTLE_DEF_TIMER_1,
                                                  PDN_THROTTLE_DEF_TIMER_2,
                                                  PDN_THROTTLE_DEF_TIMER_3,
                                                  PDN_THROTTLE_DEF_TIMER_4,
                                                  PDN_THROTTLE_DEF_TIMER_5,
                                                  PDN_THROTTLE_DEF_TIMER_6
                                                  };

  ds707_data_sess_pdn_type_enum_type pdn_type = DS707_DATA_SESS_PDN_TYPE_V4_V6;

  if (ppp_params_ptr == NULL)
  {
    DATA_ERR_FATAL("NULL ppp_params_ptr");
  }
  DATA_IS707_MSG1(MSG_LEGACY_HIGH,
                   "Setting default values for Profile: %d", profile_id);

  profile_reg_index = ds707_data_sess_profile_id_to_reg_index_mapping(profile_id);
  if (profile_reg_index < DATA_SESSION_PROFILE_ID_MIN)
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR,"Invalid Profile ID: %d passed", profile_id);
    return FALSE;
  }

  ds707_data_session_profile_set_common_params(profile_reg_index,
                                               profile_id,
                                               TRUE,
                                               ds707_gonulltmr_get_timer(DS707_GONULLTMRI_DO),
                                               ds707_gonulltmr_get_timer(DS707_GONULLTMRI_1X),
                                               FALSE,
                                               ppp_params_ptr->lcp_ack_timeout,
                                               ppp_params_ptr->ipcp_ack_timeout,
                                               ppp_params_ptr->auth_timeout,
                                               ppp_params_ptr->lcp_creq_retry_count,
                                               ppp_params_ptr->ipcp_creq_retry_count,
                                               ppp_params_ptr->auth_retry_count,
                                               DATA_SESS_AUTH_PROTOCOL_PAP_CHAP,
                                               DS707_DATARATE_HIGH,
                                               DS707_DATAMODE_CDMA_HDR);


  /*set the validity mask as common profile*/

  ds707_data_session_profile_info[profile_reg_index].profile_type =
    PROFILE_COMMON;

  /*set the validity mask as KDDI profile if current mode is KDDI */
  if (ds707_roaming_is_curr_mode_jcdma()||
      ds707_roaming_is_simplified_jcdma_mode())
  {
    ds707_data_session_profile_info[profile_reg_index].profile_type |=
      PROFILE_KDDI;
  }

  /*-------------------------------------------------------------------------
    Initialize all the eHRPD parameters with default values. Mark as eHRPD
    profile
  ---------------------------------------------------------------------------*/
  ds707_data_session_profile_set_ehrpd_params( profile_reg_index,
                                               NULL,
                                               0,
                                               NULL,
                                               0,
                                               TRUE,
                                               pdn_type,
                                               FALSE,
                                               0,
                                               //0,0,
                                               //0,0,
                                               DS707_DATA_SESS_RAT_TYPE_HRPD_EHRPD,
                                               default_timer_val,
                                               default_timer_val,
                                               0,
                                               NULL, 0,
                                               NULL, 0,
                                               DS707_DATA_SESS_INITIAL_MCCMNC_VALUE,
                                               DS707_DATA_SESS_INITIAL_MCCMNC_VALUE,
                                               FALSE );

  /*set the validity mask as common + ehrpd profile*/
  ds707_data_session_profile_info[profile_reg_index].profile_type |=
      PROFILE_EHRPD;

  /* Set the custom val mask to 0*/
  ds707_data_session_profile_info[profile_reg_index].custom_val_mask = 0;

  /* Set the persistence to be TRUE*/
  ds707_data_session_profile_info[profile_reg_index].is_persistent = TRUE;

  /* Profile DB internal params*/
  ds707_data_session_profile_info[profile_reg_index].supported_tech_mask =
      TECH_MASK_3GPP2;

  ds707_data_session_profile_info[profile_reg_index].ip_version[PRIMARY_DNS_ADDR] 
    = V4V6;

  ds707_data_session_profile_info[profile_reg_index].ip_version[SECONDARY_DNS_ADDR] 
    = V4V6;

  ds707_data_session_profile_info[profile_reg_index].is_linger_params_valid 
    = FALSE;

  /* Set the opaque data to 0xFFFFFFFF */
  ds707_data_session_profile_info[profile_reg_index].user_app_data =
      0xFFFFFFFF;

  ds707_data_session_profile_info[profile_reg_index].clat_enabled = FALSE;

  ds707_data_session_profile_info[profile_reg_index].ipv6_delegation = FALSE;

  ds707_data_session_profile_info[profile_reg_index].subs_id = ACTIVE_SUBSCRIPTION_1;

  DATA_IS707_MSG2( MSG_LEGACY_HIGH,
         "For Profile id %d Setting Profile Type as %d",
          profile_id,
          ds707_data_session_profile_info[profile_reg_index].profile_type);

  return TRUE;
}

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_USER_CREATED_PROFILE_INIT

DESCRIPTION   Sets the appropriate user created profile ID in the
              profile database

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_user_created_profile_init(void)
{
  ds707_data_session_profile_id profile_id        = -1;
  ds707_data_session_profile_id profile_reg_index = -1;

  for (profile_id = DATA_SESSION_USER_CREATED_PROFILE_MIN;
       profile_id < (DATA_SESSION_USER_CREATED_PROFILE_MIN + DATA_SESSION_USER_CREATED_MAX_PROFILES);
       profile_id++)
  {
    profile_reg_index = ds707_data_sess_profile_id_to_reg_index_mapping(profile_id);
    if (profile_reg_index > DATA_SESSION_PROFILE_ID_MIN)
    {
     ds707_data_session_profile_info[profile_reg_index].data_session_profile_id =
       profile_id;
    }
    else
    {
     DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                      "Error initializing user created profiles");
     return;
    }
  }
} /* ds707_data_session_user_created_profile_init */

/*===========================================================================
                         EXTERNAL FUNCTION DEFINTIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_INIT_CRIT_SECT

DESCRIPTION   Called once at mobile power-up.  Initializes ds707 profile crit 
              section variables
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_init_crit_sect
(
  void
)
{

  DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                      "Data_Session_Profile Crit Sect -- Initialize");
  DS_INIT_CRIT_SECTION( &ds707_profile_crit_sect );
} /* ds707_data_session_init_crit_sect() */

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_PROFILE_INIT

DESCRIPTION   This function initializes the data session profile parameters for
                different profiles.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_profile_init
(

)
{
  nv_stat_enum_type nv_status = NV_FAIL_S;
  nv_item_type      nv_item;
  /*-----------------------------------------------------------------------*/

  /*-----------------------------------------------------------------------
    Calculate total number of profiles and allocate memory for them.
  -----------------------------------------------------------------------*/
  ds707_data_sess_profile_alloc();

  /*-----------------------------------------------------------------------
    Mark all profiles as invalid. This means that we have not initialized
    the profiles yet.
  -----------------------------------------------------------------------*/
  (void)ds707_data_sess_profile_set_invalid();

  /*---------------------------------------------------------------------------
    Read the default PPP parameters from NV 1206. This function reads auth,
    lcp,ipcp, ip6cp config retry and count values from NV
  ---------------------------------------------------------------------------*/
  ds707_data_sess_profile_get_default_ppp_opts(&ds707_default_ppp_params);

  /*-------------------------------------------------------------------------
    Initialize the default Profile
  -------------------------------------------------------------------------*/
  if (!ds707_data_session_set_default_param(DATA_SESSION_DEFAULT_PROFILE,
                                       &ds707_default_ppp_params))
  {
    DATA_ERR_FATAL("Error creating default profile");
  }

  ds707_data_session_international_roaming_profile_init();

  if(ds707_roaming_is_curr_mode_jcdma()||
     ds707_roaming_is_simplified_jcdma_mode())
  {
   /*-------------------------------------------------------------
    call the initialization function for KDDI specific profiles
    ---------------------------------------------------------------*/
    ds707_data_session_kddi_profile_init_all();
  }
#if 0
#ifdef FEATURE_EHRPD
  /*-------------------------------------------------------------
    call the initialization function for eHRPD specific profiles
  ---------------------------------------------------------------*/
  ds707_data_session_ehrpd_profile_init(&ds707_default_ppp_params);

#endif

 /*-------------------------------------------------------------
    Call the initialization function for User Created profiles.
    This just sets the appropriate profile id in the profile
    database. This is required when we read a user created profile
    file from EFS during boot up.
  ---------------------------------------------------------------*/
  ds707_data_session_user_created_profile_init();
#endif

  /* Read profile info only if in NV mode. In card mode, read profile from 
     NVRUIM while processing subscription ready event. */
  nv_status = ds3gcfgmgr_read_legacy_nv_ex( NV_RTRE_CONFIG_I, 
                                            &nv_item, 
                                            ds707_curr_subs_id() );
  if( ( (nv_status == NV_DONE_S) &&
        (NV_RTRE_CONTROL_USE_RUIM != nv_item.rtre_config) ) ||
      (nv_status != NV_DONE_S) )
  {
    ds707_read_data_session_profile_from_nvruim();
  }
  
  ps_policy_mgr_reg_get_profile_cback(ds707_data_session_get_app_profile_index);

  /*-------------------------------------------------------------------------
    Read the active MIP profile id from NV.
  -------------------------------------------------------------------------*/
  nv_status = ds3gcfgmgr_read_legacy_nv_ex( NV_DS_MIP_ACTIVE_PROF_I, &nv_item, ds707_curr_subs_id() );

  if( nv_status == NV_DONE_S)
  {
    ds707_active_nv_mip_profile = nv_item.ds_mip_active_prof;
    DATA_IS707_MSG1(MSG_LEGACY_MED,
                    "active mip profile id from NV - %d",
                    ds707_active_nv_mip_profile);
  }
  else
  {
    ds707_active_nv_mip_profile = -1;
  }
} /* ds707_data_session_profile_init */

#if 0
/*===========================================================================
FUNCTION      DS707_DATA_SESSION_CREATE_PROFILE

DESCRIPTION   This function is used to create a 3GPP2 profile on modem

PARAMETERS    is_persistent : Indicates whether the created profile should
              be persistent or not

RETURN VALUE  returns the created profile id

SIDE EFFECTS  None
===========================================================================*/
ds707_data_session_profile_id ds707_data_session_create_profile(
  boolean is_persistent
)
{
  ds707_data_session_profile_id profile_id        = -1;
  ds707_data_session_profile_id profile_reg_index = -1;
  ds707_data_session_profile_id def_profile_reg_index = -1;
  ds707_data_session_profile_id tmp_profile_id;
  int fd = -1; /* EFS File Descriptor */
  char efs_file_name[PROFILE_PATH_LEN]=PROFILE_PATH;

  /*---------------------------------------------------------------------------
    Search for the first available profile in the USER_CREATED_PROFILE range
  ---------------------------------------------------------------------------*/
  for (tmp_profile_id = DATA_SESSION_USER_CREATED_PROFILE_MIN;
       tmp_profile_id < (DATA_SESSION_USER_CREATED_PROFILE_MIN
                         +DATA_SESSION_USER_CREATED_MAX_PROFILES);
       tmp_profile_id++)
  {
    profile_reg_index = ds707_data_sess_profile_id_to_reg_index_mapping(tmp_profile_id);

    if ((profile_reg_index >= DATA_SESSION_PROFILE_ID_MIN) &&
        (PROFILE_INVALID == ds707_data_session_profile_info[profile_reg_index].profile_type)
       )
    {
      DATA_IS707_MSG1(MSG_LEGACY_HIGH,
                      "Found first available profile %d",tmp_profile_id);

      if (is_persistent)
      {
        /* Create efs_file_name for specific profile number */
        ds707_create_efs_file_name(efs_file_name,tmp_profile_id);

        /* Create an empty profile file in EFS. This is used to indicate that
           the user has created the profile. So on reboot, when the module inits,
           runs through the profile files in EFS and if it finds a user created
           profile file, updates the profile cache for the profile to a
           valid profile_type
           */
        fd = ds707_create_profile_file_in_efs(efs_file_name);
        if (-1 == fd)
        {
          DATA_IS707_MSG0(MSG_LEGACY_ERROR, "EFS Create failed");
          return profile_id;
        }
        /* Close the file as we don't have anything to do*/
        if (-1 == efs_close (fd))
        {
          DATA_IS707_MSG0(MSG_LEGACY_ERROR, "EFS Close failed");
          return profile_id;
        }
      }


      /* Copy the values from default profile. It's possible that we are reusing a
         profile ID that was already created and deleted */
      def_profile_reg_index =
        ds707_data_sess_profile_id_to_reg_index_mapping(DATA_SESSION_DEFAULT_PROFILE);
      if (def_profile_reg_index >= DATA_SESSION_PROFILE_ID_MIN)
      {
        ds707_data_session_profile_info[profile_reg_index] =
          ds707_data_session_profile_info[def_profile_reg_index];

        /* Copying the default profile also sets the proper profile_type
           So just update the appropriate profile id and persistence*/
        ds707_data_session_profile_info[profile_reg_index].data_session_profile_id =
          tmp_profile_id;
        ds707_data_session_profile_info[profile_reg_index].is_persistent =
          is_persistent;

        /* Clear the custom_val_mask. Refer explanation in
           ds707_data_session_set_profile()for the reason*/
        ds707_data_session_profile_info[profile_reg_index].custom_val_mask =
          0;

        profile_id = tmp_profile_id;
      }
      break;
    }
  }

  if (-1 == profile_id)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "No available profile");
    return profile_id;
  }

  return profile_id;
} /* ds707_data_session_create_profile */


/*===========================================================================
FUNCTION      DS707_DATA_SESSION_DELETE_PROFILE

DESCRIPTION   This function is used to delete a user created 3GPP2 profile on
              modem.
              Delete profile will succeed only for:
              1. Profiles created using the DS Create Profile API and
              2. Profiles not currently in use by any iface

PARAMETERS    profile_id : 3GPP2 profile id

RETURN VALUE  TRUE  : on successful profile delete
              FALSE : on failure

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_session_delete_profile(
        ds707_data_session_profile_id profile_id
)
{
  ds707_data_session_profile_id profile_reg_index = -1;
  char efs_file_name[PROFILE_PATH_LEN]                          = PROFILE_PATH;
  ps_iface_type *iface_ptr                        = NULL;
  uint8 iface_index;

  profile_reg_index = ds707_data_sess_profile_id_to_reg_index_mapping(profile_id);
  if (profile_reg_index <= DATA_SESSION_PROFILE_ID_MIN)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "ds707_data_session_delete_profile: Unknown Profile ID");
    return FALSE;
  }

  /* Check if the profile_id to be deleted is a user created profile
     and is valid*/
  if (PROFILE_ID_IS_USER_CREATED(profile_id) &&
      ds707_data_session_profile_info[profile_reg_index].profile_type !=
       PROFILE_INVALID
     )
  {
    /* If someone is using this profile then don't delete the profile*/
    for (iface_index = 0; iface_index < DS707_MAX_IFACES; iface_index++)
    {
     iface_ptr = NULL ;
     iface_ptr = ds707_pdncntx_get_iface_ptr_from_iface_tbl_index((uint8)iface_index);

     if ((ds707_data_session_get_iface_requesting_profile(iface_ptr) == profile_id) ||
         (ds707_data_session_get_iface_current_profile(iface_ptr)    == profile_id)
      )
     {
       DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                      "Profile in use by Iface Index %d", iface_index);
       return FALSE;
     }
    }

    DATA_IS707_MSG2( MSG_LEGACY_HIGH, 
                     "Deleting profile %d, persistency %d",
                     profile_id, 
                     ds707_data_session_profile_info[profile_reg_index].is_persistent);

    if (ds707_data_session_profile_info[profile_reg_index].is_persistent)
    {
      /*Delete the profile file from EFS*/
      ds707_create_efs_file_name(efs_file_name,profile_id);
      if (-1 == efs_unlink(efs_file_name))
      {
        DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                        "Err: %d while deleting Profile file from EFS",
                        efs_errno);
        return FALSE;
      }
    }

    /*---------------------------------------------------------------------------
     Clear the contents since user may have modified some params.
    ---------------------------------------------------------------------------*/
    memset (&ds707_data_session_profile_info[profile_reg_index],
            0, sizeof (ds707_data_session_profile_info_type));

    /* Set the profile type to be Invalid */
    DATA_IS707_MSG1( MSG_LEGACY_HIGH,
           "For Profile id %d setting Profile Type as PROFILE_INVALID",
            profile_id);
    ds707_data_session_profile_info[profile_reg_index].data_session_profile_id =
      profile_id;
    ds707_data_session_profile_info[profile_reg_index].profile_type =
       PROFILE_INVALID;
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "Profile is not User created or is Invalid");
    return FALSE;
  }

  return TRUE;
}/* ds707_data_session_delete_profile */

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_RESET_PROFILE_TO_DEFAULT

DESCRIPTION   This function resets all the parameters to default for the 
              profile id. 

PARAMETERS    profile id

DEPENDENCIES  None

RETURN VALUE  boolean: true if successful, false if failed

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_session_reset_profile_to_default
(
  ds707_data_session_profile_id profile_id
)
{
  ds707_data_session_profile_id profile_reg_index = -1;
  char efs_file_name[PROFILE_PATH_LEN]    = PROFILE_PATH;
  ps_iface_type *iface_ptr  = NULL;
  int fd                    = -1;
  uint8 iface_index;
  boolean is_persistent;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  profile_reg_index = ds707_data_sess_profile_id_to_reg_index_mapping(profile_id);
  if (profile_reg_index <= DATA_SESSION_PROFILE_ID_MIN)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "ds707_data_session_reset_profile_to_default: Unknown Profile ID");
    return FALSE;
  }

  /* Check if the profile_id to be deleted is a user created profile
     and is valid*/
  if( ds707_data_session_profile_info[profile_reg_index].profile_type != 
      PROFILE_INVALID )
  {
    /* If someone is using this profile then don't delete the profile*/
    for (iface_index = 0; iface_index < DS707_MAX_IFACES; iface_index++)
    {
      iface_ptr = NULL ;
      iface_ptr = ds707_pdncntx_get_iface_ptr_from_iface_tbl_index((uint8)iface_index);

      if( (ds707_data_session_get_iface_requesting_profile(iface_ptr) == profile_id) ||
          (ds707_data_session_get_iface_current_profile(iface_ptr)    == profile_id) )
      {
        DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                        "Profile in use by Iface Index %d", iface_index);
        return FALSE;
      }
    }

    DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                     "Profile not in use by any Iface. Reset to default");

    /*-------------------------------------------------------------------------
      When the profile is reset presistence flag should be kept the same
    -------------------------------------------------------------------------*/
    is_persistent = ds707_data_session_profile_info[profile_reg_index].is_persistent;

    /*-------------------------------------------------------------------------
      Reset all profile parameters to default
    -------------------------------------------------------------------------*/
    if (!ds707_data_session_set_default_param(profile_id, &ds707_default_ppp_params))
    {
      DATA_ERR_FATAL("Error creating default profile");
      return FALSE;
    }

    /*-------------------------------------------------------------------------
      Set the persistence flag to the previous value. Create the profile 
      file in efs if persistence flag is set to TRUE.
    -------------------------------------------------------------------------*/
    ds707_data_session_profile_info[profile_reg_index].is_persistent = is_persistent;

    if (TRUE == is_persistent)
    {
      /* Create efs_file_name for specific profile number */
      ds707_create_efs_file_name(efs_file_name, profile_id);
      
      /* Create an empty profile file in EFS. If the file already exists then 
         its contents will be cleared */
      fd = ds707_create_profile_file_in_efs(efs_file_name);
      if (-1 == fd)
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, "EFS Create failed");
        return FALSE;
      }
      /* Close the file as we don't have anything to do*/
      if (-1 == efs_close (fd))
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, "EFS Close failed");
        return FALSE;
      }
    }
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Profile is Invalid");
    return FALSE;
  }

  return TRUE;
} /* ds707_data_session_reset_profile_to_default() */
#endif

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_SET_PROFILE

DESCRIPTION   This function reads the EFS files containing data session
              profile parameters and stores it in the internal array
              ds707_data_session_profile_info[ ]

PARAMETERS

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_set_profile
(

)
{
  ds707_data_session_profile_id profile_reg_index = 0; /* profile reg index count */
  ds707_data_session_profile_id profile_id        = 0;
  char efs_file_name[PROFILE_PATH_LEN]=PROFILE_PATH;
  ds3gcfgmgr_efs_token_type  efs_db; /* structure used for tokenizing the file*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*------------------------------------------------------------------ 
    Rearch changes: only KDDI profiles will be read by 3GPP2 MH from
    legacy folder. Other profiles will be copied when received profile
    DB boot up event
   ------------------------------------------------------------------*/
  if(ds707_roaming_is_curr_mode_jcdma()|| 
     ds707_roaming_is_simplified_jcdma_mode())
  {

    DS_ENTER_CRIT_SECTION(&ds707_profile_crit_sect);

    for( profile_reg_index = DATA_SESSION_JCDMA_PROFILE_MIN;
         profile_reg_index < DATA_SESSION_JCDMA_PROFILE_MIN + DATA_SESSION_JCDMA_NO_VALID_PROFILE;
       profile_reg_index++ )
    {

      if (ds707_data_session_profile_info[profile_reg_index].profile_type &
          PROFILE_KDDI)
      {
        /* Get the profile id for the current profile reg index */
        profile_id =
          ds707_data_session_profile_info[profile_reg_index].data_session_profile_id;

        /* function to create efs_file_name for specific profile number */
        ds707_create_efs_file_name(efs_file_name,profile_id);

        if(ds3gcfgmgr_efs_file_init(efs_file_name, &efs_db) != -1)
        {
          DATA_IS707_MSG1(MSG_LEGACY_HIGH,
                          "EFS Profile read success for id %d",profile_id);

          /* function that will populate the data_sess_profile structure
          with data within efs file*/
          ds707_read_data_session_profile_from_efs(efs_file_name, &efs_db,
              &ds707_data_session_profile_info[profile_reg_index],
              profile_id);

          ds3gcfgmgr_efs_file_close(&efs_db);
        }
        else
        {
          DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                          "Error opening file ISN from EFS for ID: %d",profile_id);
        }
      }/* if profile is valid */
      else
      {
        //assert for now since these profiles shall always be KDDI if enabled
        ASSERT(0);
      }
    }/* for all KDDI profiles */

    DS_LEAVE_CRIT_SECTION(&ds707_profile_crit_sect);
  }
}/* ds707_data_session_set_profile */

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_GET_PROFILE_ON_NON_DEFAULT_SUBS

DESCRIPTION   This function points to data session profile index from
              ds707_data_session_profile_info_type[ ] for a particular
              data_session_profile_id. If the id provided is out of
              bounds then it returns the profile parameters for DEFAULT
              profile.

PARAMETERS    ds707_data_session_profile_id:
                   Use this type to select which set of
                   profile parameters to return.
              subs_id:
                   subscription ID of the profile
              ds707_data_session_profile_info_type** :
                   ptr to a ptr to strcutre containing data session
                   profile parameters

DEPENDENCIES  None

RETURN VALUE  TRUE:  profile found
              FALSE: profile not valid/not found

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_session_get_profile_on_non_primary_subs
(
  ds707_data_session_profile_id profile_id,
  ds_profile_subs_id_enum_type  subs_id,
  /* output parameter */
  ds707_data_session_profile_info_type** profile_info_ptr
)
{
  ds707_data_session_profile_id         profile_reg_index;
  ds707_data_session_profile_info_type 
              *ds707_data_session_profile_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if ( subs_id == ACTIVE_SUBSCRIPTION_2 )
  {
    ds707_data_session_profile_info_ptr = 
                      ds707_data_session_profile_info_secondary_subs;
  }
  else if( subs_id == ACTIVE_SUBSCRIPTION_3 )
  {
    ds707_data_session_profile_info_ptr = 
                      ds707_data_session_profile_info_tertiary_subs;
  }
  else
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                    "Invalid Subs ID %d to get profile", 
                    subs_id);
    return FALSE;
  }

  if (ds707_data_session_profile_info_ptr == NULL)
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                    "No valid profile on Subs ID %d", 
                    subs_id);
    return FALSE;
  }

  DS_ENTER_CRIT_SECTION(&ds707_profile_crit_sect);

  for (profile_reg_index = DATA_SESSION_PROFILE_ID_MIN;
       profile_reg_index < ds707_data_session_max_profiles[subs_id - ACTIVE_SUBSCRIPTION_1];
       profile_reg_index++)
  {
    if (ds707_data_session_profile_info_ptr[profile_reg_index].data_session_profile_id
          == profile_id)
    {
      *profile_info_ptr = &ds707_data_session_profile_info_ptr[profile_reg_index];
      DS_LEAVE_CRIT_SECTION(&ds707_profile_crit_sect);
      return TRUE;
    }
  } /* for */

  DS_LEAVE_CRIT_SECTION(&ds707_profile_crit_sect);
  
  DATA_IS707_MSG2(MSG_LEGACY_ERROR,
                  "Cannot find profile %d on Subs ID %d", 
                  profile_id, subs_id);
  return FALSE;

}

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_GET_PROFILE

DESCRIPTION   This function points to data session profile index from
              ds707_data_session_profile_info_type[ ] for a particular
              data_session_profile_id on non default subs ID

PARAMETERS    ds707_data_session_profile_id:
                   Use this type to select which set of
                   profile parameters to return.
              subs_id:
                   subscription ID of the profile
              ds707_data_session_profile_info_type** :
                   ptr to a ptr to strcutre containing data session
                   profile parameters

DEPENDENCIES  None

RETURN VALUE  TRUE:  profile found
              FALSE: profile not valid/not found- fallback to default profile

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_session_get_profile
(
  ds707_data_session_profile_id profile_id,
  ds_profile_subs_id_enum_type  subs_id,
  /* output parameter */
  ds707_data_session_profile_info_type** profile_info_ptr
)
{
  ds707_data_session_profile_id profile_reg_index;
  boolean ret_val = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( (subs_id == ACTIVE_SUBSCRIPTION_2) ||
       (subs_id == ACTIVE_SUBSCRIPTION_3) )
  {
    ret_val = ds707_data_session_get_profile_on_non_primary_subs(
                                                            profile_id,
                                                            subs_id,
                                                            profile_info_ptr);
  }
  else
  {
    DS_ENTER_CRIT_SECTION(&ds707_profile_crit_sect);

  /*-------------------------------------------------------------------------
    Get the profile registry index and then return the parameter values
  -------------------------------------------------------------------------*/
    profile_reg_index = ds707_data_session_is_valid_profile(profile_id);
    if ((profile_reg_index < 0) ||
        (ds707_data_session_profile_info[profile_reg_index].subs_id != subs_id))
    {
      DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                      "Invalid Profile id provided. Using DEFAULT profile %d",
                      DATA_SESSION_DEFAULT_PROFILE);

      profile_reg_index = DATA_SESSION_DEFAULT_PROFILE;
      ret_val = FALSE;
    }
    *profile_info_ptr = &ds707_data_session_profile_info[profile_reg_index];
  
    DS_LEAVE_CRIT_SECTION(&ds707_profile_crit_sect);
  }

  return ret_val;
} /* ds707_data_session_get_profile */

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_IS_APN_ENABLED_IN_PROFILE

DESCRIPTION   This function checks if the APN is enabled or not in the profile.

DEPENDENCIES  None

RETURN VALUE  TRUE  - If the APN is enabled
              FALSE - If APN is disabled

SIDE EFFECTS  None
===========================================================================*/
static boolean ds707_data_session_is_apn_enabled_in_profile(
  char *apn_name,
  uint8 apn_len
)
{
  ds707_data_session_profile_id profile_reg_index;
  boolean                       ret_val = FALSE;

  if( apn_name == NULL )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                     "ds707_data_session_is_apn_enabled_in_profile: "
                     "NULL apn_name parameter passed" );
    return FALSE;
  }

  DS_ENTER_CRIT_SECTION(&ds707_profile_crit_sect);

  /* Look for the APN Match in EHRPD profiles */
  for ( profile_reg_index = DATA_SESSION_PROFILE_ID_MIN;
        profile_reg_index < ds707_data_session_max_profiles[SYS_MODEM_AS_ID_1];
        profile_reg_index++)
  {
    if (PROFILE_EHRPD & ds707_data_session_profile_info[profile_reg_index].profile_type)
    {
      if ( apn_len == ds707_data_session_profile_info[profile_reg_index].apn_string_len )
      {
        if( strncasecmp( ds707_data_session_profile_info[profile_reg_index].apn_string,
                     apn_name, apn_len ) == 0 )
        {
          if ( ds707_data_session_profile_info[profile_reg_index].apn_flag == TRUE )
          {
            ret_val = TRUE;
          }
          DS_LEAVE_CRIT_SECTION(&ds707_profile_crit_sect);

          DATA_IS707_MSG2( MSG_LEGACY_HIGH,
                           "APN found in profile %d, enabled? %d",
                           ds707_data_session_profile_info[profile_reg_index].data_session_profile_id,
                           ret_val );
          return ret_val;
        }
      }
    }
  }
  DS_LEAVE_CRIT_SECTION(&ds707_profile_crit_sect);

  DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                   "APN not found in any profiles, return FALSE" );
  return FALSE;
}

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_IS_APN_ENABLED_IN_UICC_CARD

DESCRIPTION   This function checks if the APN is enabled or not. The APN is
              enabled if it is present in the APN control list from the card

DEPENDENCIES  None

RETURN VALUE  TRUE  - If the APN is enabled
              FALSE - If APN is disabled

SIDE EFFECTS  None
===========================================================================*/
static boolean ds707_data_session_is_apn_enabled_in_uicc_card(
  char *apn_name,
  uint8 apn_len
)
{
  int num_apn = 0;
  ds707_data_session_acl_info_type* ds707_acl_info = NULL;

  if( apn_name == NULL )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                     "ds707_data_session_is_apn_enabled_in_uicc_card: "
                     "NULL apn_name parameter passed" );
    return FALSE;
  }

  ds707_acl_info = ds707_data_session_get_acl_info_ptr();
  if ( (ds707_acl_info->ds3g_mmgsdi_init == TRUE) &&
       (ds707_acl_info->num_apn >= 0) )
  {
    for( num_apn = 0; num_apn < ds707_acl_info->num_apn; num_apn++ )
    {
      if( apn_len == ds707_acl_info->profile_apn_list[num_apn].apn_len )
      {
        if( strncasecmp( ds707_acl_info->profile_apn_list[num_apn].apn_name,
                     apn_name, apn_len ) == 0 )
        {
          DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                           "APN present in the APN control list in card, return TRUE" );
          return TRUE;
        }
      }
    }
  }
  else
  {
    DATA_IS707_MSG0( MSG_LEGACY_MED,
                     "UICC card check for APN enabled PASS, return TRUE" );
    return TRUE;
  }

  DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                   "APN not present in the APN control list in card, return FALSE" );
  return FALSE;
}

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_INIT_MANDATORY_APN_LIST

DESCRIPTION   This function reads the file mandatory APNs list and stores in local cache.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  void

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_init_mandatory_apn_list(
  void
)
{
  char efs_file_name[30]="/eHRPD/mandatory_apn_list.txt";
  char *from, *to;           /* ptrs to start and end of the token     */
  ds3gcfgmgr_efs_token_type  efs_db; /* structure used for tokenizing the file */
  ds3gcfgmgr_efs_token_parse_status_type  efs_ret_val
                                              = DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS;
  uint8 length = 0,i=0;
  uint8 apn_string_len = 0;


  if(ds3gcfgmgr_efs_file_init(efs_file_name, &efs_db) == -1)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Error opening file ISN from EFS");
    return;
  }
  /* Set the seperator as ; */
  efs_db.seperator = ';';

  while (DS3GCFGMGR_EFS_TOKEN_PARSE_EOF
         != (efs_ret_val = ds3gcfgmgr_efs_tokenizer(&efs_db, &from, &to )))
  {
    /*------------------------------------------------------------------------
      Token being read. from points to the beginning of the token and
      to point to the end of the token.

      The tokenizer automatically skips blank lines and comments (lines
      beginning with #, so no need to check for them here).
    ------------------------------------------------------------------------*/
    if((from == to) || (DS3GCFGMGR_EFS_TOKEN_PARSE_EOL == efs_ret_val))
    {
      /*----------------------------------------------------------------------
        Skip empty tokens.
      ----------------------------------------------------------------------*/
      continue;
    }
    else if(DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS == efs_ret_val)
    {
      length = (uint8)( to - from);
      if ( (from > to)  || (length > DS_VSNCP_3GPP2_APN_MAX_VAL_LEN))
      {
        apn_string_len = 0;
        DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                        "Error in APN String. Setting APN length to 0.");
      }
      else
      {
        apn_string_len = length;
        if ( ds707_mandatory_apn_list[i].apn_name == NULL )
        {
          ds707_mandatory_apn_list[i].apn_name = 
                       (char*)modem_mem_alloc(
                                       (apn_string_len + 1)* sizeof(char),
                                       MODEM_MEM_CLIENT_DATA_CRIT);
          if( NULL == ds707_mandatory_apn_list[i].apn_name )
          {
            DATA_ERR_FATAL("profile memory re-allocation failed");
            return;
          }
          else
          {
            memset(ds707_mandatory_apn_list[i].apn_name, '\0', (1 + apn_string_len));
            (void)memscpy(ds707_mandatory_apn_list[i].apn_name,
                               apn_string_len,
                      from,
                      apn_string_len);
            ds707_mandatory_apn_list[i].is_modified = FALSE;
          }
        }
      }

      i++;
      if (i >= DATA_SESSION_MAX_APN_NUM)
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                        "Reached MAX number of APNs that can be stored in min apn list. Return.");
        break;
      }
    }
  }
  ds3gcfgmgr_efs_file_close(&efs_db);
}

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_IS_MANDATORY_APN_ENABLED

DESCRIPTION   This function checks whether the list of mandatory APNs are all
              enabled in the profile and in the card, if its present.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  boolean: TRUE: if all the APNs in the mandatory list are enabled
                             in profile and card.
                       FALSE: otherwise.

SIDE EFFECTS  None
===========================================================================*/
static boolean ds707_data_session_is_mandatory_apn_enabled(
  void
)
{
  boolean return_val = TRUE;
  uint8 apn_string_len = 0;
  uint8 i=0;

  while ( i< DATA_SESSION_MAX_APN_NUM )
  {
    if ( ds707_mandatory_apn_list[i].apn_name == NULL )
    {
      i++;
      continue;
    }
    apn_string_len = strlen(ds707_mandatory_apn_list[i].apn_name);
    if ( apn_string_len > 0 )
    {
      if( ds707_data_session_is_apn_enabled_in_profile( ds707_mandatory_apn_list[i].apn_name,
                                                        apn_string_len ) == TRUE )
      {
        if( ( ds707_mandatory_apn_list[i].is_modified )   || 
            ( ds707_data_session_is_apn_enabled_in_uicc_card(ds707_mandatory_apn_list[i].apn_name, apn_string_len) == TRUE )
          )
        {
          i++; 
          continue;
        }
        else
        {
          return_val = FALSE;
          DATA_IS707_MSG_SPRINTF_2(MSG_LEGACY_ERROR, "APN not enabled in UICC card for index i %d,%s",
                                            i, ds707_mandatory_apn_list[i].apn_name);
          break;
        }
      }
      else
      {
        return_val = FALSE;
        DATA_IS707_MSG_SPRINTF_2(MSG_LEGACY_ERROR, "APN not enabled in profile for index i %d,%s",
                                                    i, ds707_mandatory_apn_list[i].apn_name);
          break;
      }
    }
    i++;
  }
  return return_val;
}

/*===========================================================================
FUNCTION      DS707_UPDATE_MIN_APN_EFS

DESCRIPTION   This function updates the modified MIN APN cache info to EFS file to make it persistent.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  void

SIDE EFFECTS  None
===========================================================================*/
void ds707_update_min_apn_efs(
  void
)
{
  
  char tmp_file_name[30]="/eHRPD/mandatory_apn_list.tmp";
  char efs_file_name[30]="/eHRPD/mandatory_apn_list.txt";
  uint8 writebufflen=0,i=0;
  int fd,efs_ret;
  char  *writebuff=NULL;


  /* Creating min apn .tmp file*/
  fd = mcfg_fopen( tmp_file_name,
                   MCFG_FS_O_CREAT | MCFG_FS_O_TRUNC | MCFG_FS_O_WRONLY,
                   MCFG_FS_DEFFILEMODE,
                   MCFG_FS_TYPE_EFS,
                   (mcfg_fs_sub_id_e_type) ds707_curr_subs_id());

  if (-1 == fd)
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                    "EFS_Open Unable to create tmp profile file Err: %d",
                    mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    return ;
  }

  DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                    "Temp profile file created successfully");  

  while (  i < DATA_SESSION_MAX_APN_NUM )
  {
    if ( ds707_mandatory_apn_list[i].apn_name == NULL )
    {
      i++;
      continue;
    }
    else
    {
      //WRITE to file.
      writebufflen = strlen(ds707_mandatory_apn_list[i].apn_name); //+1 for NULL termi?
      writebuff = (char*) modem_mem_alloc(writebufflen+1, MODEM_MEM_CLIENT_DATA);  
      if(writebuff == NULL)
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ds707_update_min_apn_efs():"
                                            "memory allocation failed");
        return;
      }

      memset((void *)writebuff, 0, writebufflen+1);
  
      (void)memscpy(writebuff,
                       writebufflen,
                       ds707_mandatory_apn_list[i].apn_name,
                       writebufflen);

      writebuff[writebufflen++] = ';';

      DATA_IS707_MSG1(MSG_LEGACY_HIGH,
                            "Writing %d bytes to EFS", writebufflen);

      efs_ret = (fs_size_t) mcfg_fwrite(fd, 
                                        (void *)writebuff, 
                                         writebufflen,
                                         MCFG_FS_TYPE_EFS);

      if (-1 == efs_ret || efs_ret != writebufflen)
      {
        DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                            "EFS Write failed. Efs_ret:%d", efs_ret);
        modem_mem_free(writebuff, MODEM_MEM_CLIENT_DATA);
        writebuff = NULL;
        goto EFS_ERR;
      }
      writebufflen = 0; 
      modem_mem_free(writebuff, MODEM_MEM_CLIENT_DATA);
      writebuff = NULL;
      i++;
    }
  }


  if (-1 == mcfg_fclose( fd, MCFG_FS_TYPE_EFS))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "EFS Close failed");
    return;
  }

  if (MCFG_FS_STATUS_OK != mcfg_fs_delete(efs_file_name,
							MCFG_FS_TYPE_EFS, 
							(mcfg_fs_sub_id_e_type) ds707_curr_subs_id()))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "EFS Unlink failed");
    return ;
  }
  
  if (-1 == mcfg_fs_rename(tmp_file_name,
			   efs_file_name,
			   MCFG_FS_TYPE_EFS, 
			   (mcfg_fs_sub_id_e_type)ds707_curr_subs_id()))
  {
    DATA_IS707_MSG2(MSG_LEGACY_ERROR, "EFS Rename failed: %d, subs_id 0x%0x",
				mcfg_fs_errno(MCFG_FS_TYPE_EFS),(mcfg_fs_sub_id_e_type)ds707_curr_subs_id() );
    return;
  }
  return;

EFS_ERR:

  /* delete the Min apn .tmp file */
  if (-1 == mcfg_fclose( fd, MCFG_FS_TYPE_EFS))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "EFS Close failed");
    return ;
  }
  mcfg_fs_delete(tmp_file_name,
                   MCFG_FS_TYPE_EFS, 
                   (mcfg_fs_sub_id_e_type)ds707_curr_subs_id());
  return;
  
}

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_UPDATE_MIN_APN_LIST

DESCRIPTION   This function checks whether the list of mandatory APNs are all
              enabled in the profile and in the card, if its present.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  boolean: TRUE: if all the APNs in the mandatory list are enabled
                             in profile and card.
                       FALSE: otherwise.

SIDE EFFECTS  None
===========================================================================*/
 void ds707_data_session_update_min_apn_list(char *new_apn , char *old_apn)
{
  uint8 new_apn_len = 0,old_apn_len = 0;
  uint8 apn_string_len = 0,i=0;
  boolean  update_min_apn_efs = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( old_apn == NULL )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                   "ds707_data_session_update_min_apn_list: "
                   "invalid input" );
    return ;
  }
  else
  {
    old_apn_len = strlen(old_apn);
  }

  if ( new_apn != NULL )
  {
    new_apn_len = strlen(new_apn);
  }

  /* New apn len can be 0 for the delete profile case */
  if(old_apn_len == 0)
  {
    
    DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                     "ds707_data_session_update_min_apn_list: "
                     "invalid length of input APN strings" );
    return;
  }

  DATA_IS707_MSG2( MSG_LEGACY_HIGH,
                   "ds707_data_session_update_min_apn_list: "
                   " length of input APN strings %d %d", new_apn_len,old_apn_len);

  if(new_apn != NULL)
  { 

    DATA_IS707_MSG_SPRINTF_2( MSG_LEGACY_HIGH,
                                          "Updated min apn list (old) %s ,(new) %s ",
                                           old_apn, new_apn);
  }


  while ( i< DATA_SESSION_MAX_APN_NUM )
  {
    if ( ds707_mandatory_apn_list[i].apn_name == NULL )
    {
      i++;
      continue;
    }
    else
    {
      apn_string_len = strlen(ds707_mandatory_apn_list[i].apn_name);

      if ( apn_string_len > 0 )
      {
        if( strncasecmp( old_apn,
                         ds707_mandatory_apn_list[i].apn_name, apn_string_len ) == 0 )
        {
          if( new_apn == NULL )
          {
            DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                     "ds707_data_session_update_min_apn_list: "
                     " Delete entry at index i", i); 
  

            modem_mem_free(ds707_mandatory_apn_list[i].apn_name,MODEM_MEM_CLIENT_DATA_CRIT);
            ds707_mandatory_apn_list[i].apn_name = NULL;
            ds707_mandatory_apn_list[i].is_modified = FALSE;
            update_min_apn_efs = TRUE;
          }
          else
          {
            ds707_mandatory_apn_list[i].apn_name = (char*)(modem_mem_realloc(ds707_mandatory_apn_list[i].apn_name,  
                                                         new_apn_len+1,// todo +1?
                                                         MODEM_MEM_CLIENT_DATA_CRIT));
            if( NULL == ds707_mandatory_apn_list[i].apn_name )
            {
               DATA_ERR_FATAL("profile memory re-allocation failed");
               return;
            } 
            else
            {
              memset(ds707_mandatory_apn_list[i].apn_name, '0', (1 + new_apn_len));
  
              (void)memscpy(ds707_mandatory_apn_list[i].apn_name,
                                              new_apn_len,new_apn,new_apn_len);
              ds707_mandatory_apn_list[i].apn_name[new_apn_len]= '\0';
              ds707_mandatory_apn_list[i].is_modified = TRUE;
  
              update_min_apn_efs = TRUE;
  
              DATA_IS707_MSG_SPRINTF_2( MSG_LEGACY_HIGH,
                                          "Updated min apn list (old) %s ,(new) %s ",
                                           old_apn, new_apn);
            }
            break;
          }
        }
      }
      i++;
    }
  }

  if ( update_min_apn_efs == TRUE )
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH,"ds707_data_session_update_min_apn_list: APN names changed in cache only");
    ds707_update_min_apn_efs();
  }
}

/*===========================================================================

FUNCTION      DS707_DATA_SESSION_PROCESS_UPDATE_MIN_APN_HDLR

DESCRIPTION   This function checks whether the list of mandatory APNs are all
              enabled in the profile and in the card, if its present.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  boolean: TRUE: if all the APNs in the mandatory list are enabled
                             in profile and card.
                       FALSE: otherwise.

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_process_update_min_apn_hdlr(  ds_cmd_type *ds_cmd_ptr)
{
  ds707_pkt_min_apn_update_cmd_type *min_apn_update_ptr = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DATA_IS707_MSG0(MSG_LEGACY_HIGH, " MIN APN update processing");

  min_apn_update_ptr = ((ds707_pkt_min_apn_update_cmd_type*)(ds_cmd_ptr->cmd_payload_ptr));

  if( min_apn_update_ptr == NULL )
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Invalid input. Skip MIN APN update processing");
  }
  else
  {
    ds707_data_session_update_min_apn_list ( min_apn_update_ptr->new_apn, min_apn_update_ptr->old_apn);
  }
  return;
  
}

#ifdef FEATURE_DATA_MANDATORY_PDN_LABEL
/*===========================================================================
FUNCTION      DS707_DATA_SESSION_IS_PDN_LABEL_ENABLED_IN_PROFILE

DESCRIPTION   This function checks whether the APNs corresponding to the 
              input pdn label are enabled, if any of them is enabled, return
              TRUE, otherwise, return FALSE.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  boolean: TRUE: if any of the APNs corresponding to the
                             input pdn label are is enabled.
                       FALSE: otherwise.

SIDE EFFECTS  None
===========================================================================*/
static boolean ds707_data_session_is_pdn_label_enabled_in_profile(
  char *label_string_ptr,
  uint8 label_len
)
{
  ds707_data_session_profile_id profile_reg_index = DATA_SESSION_PROFILE_ID_MIN;

  if( label_string_ptr == NULL || 
      0 == label_len ||
      label_len > DS_VSNCP_3GPP2_APN_MAX_VAL_LEN)
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                     "ds707_data_session_is_pdn_label_enabled_in_profile: "
                     "invalid input" );
    return FALSE;
  }

  for ( profile_reg_index = DATA_SESSION_PROFILE_ID_MIN;
        profile_reg_index < ds707_data_session_max_profiles[SYS_MODEM_AS_ID_1];
        profile_reg_index++)
  {
    if (PROFILE_EHRPD & ds707_data_session_profile_info[profile_reg_index].profile_type)
    {
      if ( label_len == ds707_data_session_profile_info[profile_reg_index].pdn_label_len )
      {
        if( strncasecmp( ds707_data_session_profile_info[profile_reg_index].pdn_label,
                     label_string_ptr, label_len ) == 0 )
        {
          if ( ds707_data_session_profile_info[profile_reg_index].apn_flag)
          {
            DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                             "APN is enabled in profile %d, return TRUE",
                             ds707_data_session_profile_info[profile_reg_index].data_session_profile_id );
            return TRUE;
          }
          else
          {
            DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                             "APN is disabled in profile %d, continue checking other APNs",
                             ds707_data_session_profile_info[profile_reg_index].data_session_profile_id );
          }
        }
      }
    }
  }

  DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                   "No matched profile or all corresponding APNs are disabled, return FALSE" );
  return FALSE;
}

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_IS_MANDATORY_PDN_LABEL_ENABLED

DESCRIPTION   This function checks whether the list of mandatory pdn labels are 
              all enabled in the profile.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  boolean: TRUE: if all the APNs in the mandatory list are enabled
                             in profile and card.
                       FALSE: otherwise.

SIDE EFFECTS  None
===========================================================================*/
static boolean ds707_data_session_is_mandatory_pdn_label_enabled(
  void
)
{
  boolean return_val = TRUE;
  char efs_file_name[]="/eHRPD/mandatory_pdn_label_list.txt";
  char *from = NULL, *to = NULL;           /* ptrs to start and end of the token     */
  ds3gcfgmgr_efs_token_type  efs_db; /* structure used for tokenizing the file */
  ds3gcfgmgr_efs_token_parse_status_type  efs_ret_val
                                              = DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS;
  uint8 length = 0;
  char label_string[DS_VSNCP_3GPP2_APN_MAX_VAL_LEN] = {0};
  uint8 label_string_len = 0;

  if(ds3gcfgmgr_efs_file_init(efs_file_name, &efs_db) == -1)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "ds707_data_session_is_mandatory_pdn_label_enabled:"
                                     "Error opening file ISN from EFS");
    return return_val;
  }
  /* Set the seperator as ; */
  efs_db.seperator = ';';

  while (DS3GCFGMGR_EFS_TOKEN_PARSE_EOF
         != (efs_ret_val = ds3gcfgmgr_efs_tokenizer(&efs_db, &from, &to )))
  {
    /*------------------------------------------------------------------------
      Token being read. from points to the beginning of the token and
      to point to the end of the token.

      The tokenizer automatically skips blank lines and comments (lines
      beginning with #, so no need to check for them here).
    ------------------------------------------------------------------------*/
    if((from == to) || (DS3GCFGMGR_EFS_TOKEN_PARSE_EOL == efs_ret_val))
    {
      /*----------------------------------------------------------------------
        Skip empty tokens.
      ----------------------------------------------------------------------*/
      continue;
    }
    else if(DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS == efs_ret_val)
    {
      length = (uint8)( to - from);
      if ( (from > to)  || (length > DS_VSNCP_3GPP2_APN_MAX_VAL_LEN))
      {
        label_string_len = 0;
        DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                        "ds707_data_session_is_mandatory_pdn_label_enabled:"
                        "Error in label String. Setting label length to 0.");
      }
      else
      {
        label_string_len = length;
        (void)memscpy(label_string,
                      DS_VSNCP_3GPP2_APN_MAX_VAL_LEN,
                      from,
                      label_string_len);
      }

      if ( label_string_len > 0 )
      {
        if( ds707_data_session_is_pdn_label_enabled_in_profile( label_string,
                                                          label_string_len ) )
        {
          return_val = TRUE;
        }
        else
        {
          return_val = FALSE;
          break;
        }
      }
    }
  }
  ds3gcfgmgr_efs_file_close(&efs_db);
  return return_val;
}
#endif /*FEATURE_DATA_MANDATORY_PDN_LABEL*/

#ifdef FEATURE_EHRPD
/*===========================================================================
FUNCTION      DS707_DATA_SESSION_CHECK_MANDATORY_APNS

DESCRIPTION   This function checks whether the list of mandatory APNs are all
              enabled in the profile and in the card, if its present. And
              accordingly sets the ehrpd_allowed flag to TRUE or FALSE.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_check_mandatory_apns(
  void
)
{
  if ( ds707_data_session_is_mandatory_apn_enabled()
#ifdef FEATURE_DATA_MANDATORY_PDN_LABEL
       &&ds707_data_session_is_mandatory_pdn_label_enabled() 
#endif /*FEATURE_DATA_MANDATORY_PDN_LABEL*/
       )
  {
    DATA_IS707_MSG0( MSG_LEGACY_MED,
                     "Mandatory APNs check passed" );    
#ifdef FEATURE_DATA_CM_CENTRIC_EMERG_MODE
    mandatory_apn_check_status = 1;    
#endif  
    if(ds707_data_session_nv_minapnlist_disallow_call())
    {
      ds707_data_sess_exit_emerg_only_mode();
    }
    ds707_data_session_process_ehrpd_attach_allowed(TRUE);
  }
  else
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                     "Mandatory APNs check fail" ); 
#ifdef FEATURE_DATA_CM_CENTRIC_EMERG_MODE
    mandatory_apn_check_status = 0;
#endif
    if(ds707_data_session_nv_minapnlist_disallow_call())
    {  
      ds707_data_sess_enter_emerg_only_mode();
    }
    ds707_data_session_process_ehrpd_attach_allowed(FALSE);
  }
}
#endif /*FEATURE_EHRPD*/
/*===========================================================================
FUNCTION      DS707_DATA_SESSION_NV_MINAPNLIST_DISALLOW_CALL

DESCRIPTION   Returns the nv configuration for disallowing call when any 
              mandatory APN is disabled 

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  TRUE: to disallow any call when any 
              mandatory APN is disabled 
              FALSE: only disallow eHRPD call when any 
              mandatory APN is disabled 

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_session_nv_minapnlist_disallow_call(void)
{
  return ds707_data_sess_minapnlist_disallow_call;
}
/*===========================================================================
FUNCTION      DS707_DATA_SESSION_DISALLOW_CALL_NV_INIT

DESCRIPTION   Read nv value for whether to disallow any call when a mandaory 
              APN is disabled. 

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_disallow_call_nv_init()
{
/*-------------------------------------------------------------------------*/
  ds707_data_session_read_disallow_call_nv();

  rex_def_timer_ex( &ds707_wait_for_emerg_call_timer,
                    ds707_data_sess_wait_for_emerg_timer_cb, 
                    0);
  return;
}

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_READ_DISALLOW_CALL_NV

DESCRIPTION   Read nv value for whether to disallow any call when a mandaory 
              APN is disabled. 

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_read_disallow_call_nv(void)
{

  char item_file_path[] = "/nv/item_files/data/3gpp2/minapnlist_disallow_call";

  nv_stat_enum_type  status = NV_DONE_S;
/*-------------------------------------------------------------------------*/
  status = ds3gcfgmgr_read_efs_nv_ex(item_file_path, 
                          &ds707_data_sess_minapnlist_disallow_call, 
                          sizeof(boolean), ds707_curr_subs_id()); 
  if (NV_DONE_S != status)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
        "EFS item not configured for minapnlist_disallow_call, "
        "Initializing defaults: false.");
    ds707_data_sess_minapnlist_disallow_call = FALSE;
  }
  return;
}
/*===========================================================================
FUNCTION      DS707_DATA_SESSION_PROFILE_CACHE_INIT

DESCRIPTION   This function initializes the data session profile parameters
              for different profiles. It also sets customized profile
              parameters depending on the EFS files. And finally it registers
              iface events.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_profile_cache_init( void )
{
#ifdef FEATURE_MODEM_CONFIG_REFRESH
  sys_modem_as_id_e_type                   subs_id = SYS_MODEM_AS_ID_NONE;
  ds3geventmgr_filter_type                 filter_info;
#endif
  /*------------------------------------------------------------------------

  *------------------------------------------------------------------------*/

  ds707_nv_util_read_data_session_profile_cache_init();


  /* Moved to profile DB callback processing */
#if 0
  ds707_acl_info = ds707_data_session_get_acl_info_ptr();
  ds707_acl_info->data_session_profile_init = TRUE;

#ifdef FEATURE_EHRPD
#ifdef FEATURE_DATA_CM_CENTRIC_EMERG_MODE
  mandatory_apn_check_status = 1;
#endif
  if (ds3g_get_cm_client_init_status())
  {
  if ( (ds707_acl_info->data_session_profile_init == TRUE) &&
       (ds707_acl_info->ds3g_mmgsdi_init == TRUE) )
  {
    /*---------------------------------------------------------------------
      Check if mandatory APNs are enabled in profile and uicc card.
    ---------------------------------------------------------------------*/
    ds707_data_session_check_mandatory_apns();
  }
  }
#endif /* FEATURE_EHRPD */
#endif

  /*------------------------------------------------------------------------
    Register for ds707 cback events.
  ------------------------------------------------------------------------*/
  ds707_data_session_reg_cback_events();

  /*------------------------------------------------------------------------
    Set the oldest_active_iface_ptr to NULL
  ------------------------------------------------------------------------*/
  ds707_data_session_set_oldest_active_iface_ptr(NULL);

#ifdef FEATURE_MODEM_CONFIG_REFRESH
  /*--------------------------------------------------------------------------
      Now we are ready to listen to NV Refresh event 
      -------------------------------------------------------------------------*/
   memset(&filter_info, 0, sizeof(ds3geventmgr_filter_type));

   filter_info.tech = PS_SYS_TECH_ALL;

   for(subs_id = SYS_MODEM_AS_ID_1; subs_id < DS3GSUBSMGR_SUBS_ID_COUNT; 
        subs_id++)
   {
     filter_info.ps_subs_id = (ps_sys_subscription_enum_type)
              ds3gsubsmgr_subs_id_cm_to_ds((sys_modem_as_id_e_type)subs_id);
     ds3geventmgr_event_reg(DS3GEVENTMGR_NV_REFRESH_EV,
                            DS3GEVENTMGR_CLIENT_ID_3GPP2,
                            &filter_info,
                            ds707_pkt_mgr_ds3g_event_cb,
                            NULL);
   }
#endif

  ds_profile_3gpp2_mgr_func_table_init();
}

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_SET_DEFAULT_PROFILE

DESCRIPTION   This function is called if the profile parameters are not set
              for the default data session profile. In that case, the default
              paramters are read from ps ppp structure and stored in default
              data session profile structure.

PARAMETERS

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_set_default_profile()
{
/*-------------------------------------------------------------------------*/
  if(
      ds707_data_session_profile_info[DATA_SESSION_DEFAULT_PROFILE].data_session_profile_id
      == DATA_SESSION_PROFILE_ID_INVALID)
  {
  /*------------------------------------------------------------------------
    call an API to get values from PS structur. set the values in the
    default structure.
  ------------------------------------------------------------------------*/
  ds707_get_ppp_default_dev_constant(
        &ds707_data_session_profile_info[DATA_SESSION_DEFAULT_PROFILE]);

  /*------------------------------------------------------------------------
    get PPP session close timer value and set it in the default strcture
  ------------------------------------------------------------------------*/
  // how to get this default value
  ds707_data_session_profile_info[DATA_SESSION_DEFAULT_PROFILE].ppp_session_close_timer_DO =
    ds707_gonulltmr_get_timer(DS707_GONULLTMRI_DO);
  ds707_data_session_profile_info[DATA_SESSION_DEFAULT_PROFILE].ppp_session_close_timer_1X =
    ds707_gonulltmr_get_timer(DS707_GONULLTMRI_1X);

  /*------------------------------------------------------------------------
    set the data rate and data mode in the default structure
  ------------------------------------------------------------------------*/
  ds707_data_session_profile_info[DATA_SESSION_DEFAULT_PROFILE].data_rate =
    DS707_DATARATE_HIGH;
  ds707_data_session_profile_info[DATA_SESSION_DEFAULT_PROFILE].data_mode =
    DS707_DATAMODE_CDMA_HDR;
  /*------------------------------------------------------------------------
    set allow linger as false i.e. linger state is not allowed in case of
    default profile.
  ------------------------------------------------------------------------*/
  ds707_data_session_profile_info[DATA_SESSION_DEFAULT_PROFILE].allow_linger
    = FALSE;
  /*------------------------------------------------------------------------
    set linger timeout value to default value (0) for default profile
  ------------------------------------------------------------------------*/
  ds707_data_session_profile_info[DATA_SESSION_DEFAULT_PROFILE].linger_timeout_val
    = 0;
  /*------------------------------------------------------------------------
    set the profile_id of default strcutre as DATA_SESSION_DEFAULT_TYPE
  ------------------------------------------------------------------------*/
  ds707_data_session_profile_info[DATA_SESSION_DEFAULT_PROFILE].data_session_profile_id =
    DATA_SESSION_DEFAULT_PROFILE;
  }
}

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_INFO_INIT

DESCRIPTION   This function initializes the data_session_info type structure.

PARAMETERS    ds707_data_session_info_type *

DEPENDENCIES  None

RETURN VALUE  int: 0 - on success
                    -1 - on failure

SIDE EFFECTS  None
===========================================================================*/
int ds707_data_session_info_init
(
    ds707_data_session_info_type *data_session_info_ptr
)
{
  if ( data_session_info_ptr == NULL)
  {
    DATA_ERR_FATAL("NULL data_session_info_ptr");
    return -1;
  }
  else
  {
    /*---------------------------------------------------------------------------
      initialize both current and requestin profile
    ---------------------------------------------------------------------------*/
    memset((void*)&data_session_info_ptr->iface_profile_info.current_data_session_profile,
           0,
           sizeof(ds707_data_session_profile_info_type));

    data_session_info_ptr->iface_profile_info.current_data_session_profile.data_session_profile_id =
      DATA_SESSION_PROFILE_ID_INVALID;

    memset((void*)&data_session_info_ptr->iface_profile_info.requesting_data_session_profile,
           0,
           sizeof(ds707_data_session_profile_info_type));

    data_session_info_ptr->iface_profile_info.requesting_data_session_profile.data_session_profile_id =
      DATA_SESSION_PROFILE_ID_INVALID;

    data_session_info_ptr->is_iface_lingering = FALSE;

    /*-----------------------------------------------------------------------
      Set PDN ID to 0 and APN String Length as 0
    -----------------------------------------------------------------------*/
    data_session_info_ptr->pdn_id = 0;
    data_session_info_ptr->apn_string_len = 0;

    /*-------------------------------------------------------------------------
      this self referential assignment is done to make sure that
      this is a valid data_session_info structure
    -------------------------------------------------------------------------*/
    data_session_info_ptr->this_session = data_session_info_ptr;
    return 0;
  }
}

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_GET_IFACE_CURRENT_PROFILE

DESCRIPTION   This function reads the current data session profile id from
              data_session_info in the pdn cb associated with this iface.

PARAMETERS    ps_iface_type *: pointer to iface.

DEPENDENCIES  None

RETURN VALUE  TRUE  if iface current profile is valid
              FALSE if PDN CB or current profile is invalid 

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_session_get_iface_current_profile
(
  ps_iface_type *iface_ptr,
  ds707_data_session_profile_info_type **data_sess_profile_ptr
)
{
  ds707_pdn_context_cb_type *ds707_pdn_cb_ptr = NULL;
/*----------------------------------------------------------------------*/
  /* lint fix */
  DATA_MDM_ARG_NOT_CONST(iface_ptr);

  if ( PS_IFACE_IS_VALID(iface_ptr) )
  {
    ds707_pdn_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(iface_ptr);
  }
  
  return ds707_pdncntx_get_current_profile(ds707_pdn_cb_ptr,
                                           data_sess_profile_ptr);
}

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_SET_IFACE_CURRENT_PROFILE

DESCRIPTION   This function sets the current data session profile id in
              data_session_info in the pdn cb associated with this iface.

PARAMETERS    ps_iface_type *:  pointer to iface
              ds707_data_session_profile_id :
                    Current data session profile id for the iface.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_set_iface_current_profile
(
    ps_iface_type *iface_ptr,
    ds707_data_session_profile_id current_profile_id,
    ds_profile_subs_id_enum_type  subs_id
)
{
  ds707_pdn_context_cb_type *ds707_pdn_cb_ptr = NULL;
/*----------------------------------------------------------------------*/
  /* lint fix */
  DATA_MDM_ARG_NOT_CONST(iface_ptr);

  if ( PS_IFACE_IS_VALID(iface_ptr) )
  {
    ds707_pdn_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(iface_ptr);
    ds707_pdncntx_set_current_profile_id(ds707_pdn_cb_ptr, 
                                         current_profile_id,
                                         subs_id);
  }
}

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_GET_IFACE_REQUESTING_PROFILE

DESCRIPTION   This function reads the requesting data session profile from
              data_session_info in the pdn cb associated with this iface.

PARAMETERS    ps_iface_type *:  pointer to iface

DEPENDENCIES  None

RETURN VALUE  TRUE  if iface requesting profile is valid
              FALSE if PDN CB or requesting profile is invalid 

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_session_get_iface_requesting_profile
(
  ps_iface_type                        *iface_ptr,
  ds707_data_session_profile_info_type **data_sess_profile_ptr
)
{
  ds707_data_session_info_type* session_info_ptr = NULL;
  ds707_pdn_context_cb_type *ds707_pdn_cb_ptr = NULL;
/*----------------------------------------------------------------------*/
  /* lint fix */
  DATA_MDM_ARG_NOT_CONST(iface_ptr);

  if ( PS_IFACE_IS_VALID(iface_ptr) )
  {
    ds707_pdn_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(iface_ptr);
    if ( DS707_PDNCNTX_CB_IS_VALID(ds707_pdn_cb_ptr) )
    {
      session_info_ptr = ds707_pdncntx_get_data_session_info_ptr(ds707_pdn_cb_ptr);
      if ( DATA_SESSION_IS_VALID(session_info_ptr) )
      {
        return ds707_pdncntx_get_requesting_profile( ds707_pdn_cb_ptr,
                                                     data_sess_profile_ptr );
      }
    }
  }
  return FALSE;
}

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_SET_IFACE_REQUESTING_PROFILE

DESCRIPTION   This function sets the requesting data session profile id in
              data_session_info in the pdn cb associated with this iface.

PARAMETERS    ps_iface_type *: pointer to iface
              ds707_data_session_profile_id :
                   Requesting data session profile id for the iface.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_set_iface_requesting_profile
(
    ps_iface_type *iface_ptr,
    ds707_data_session_profile_id requesting_profile_id,
    ds_profile_subs_id_enum_type  subs_id
)
{
  ds707_pdn_context_cb_type *ds707_pdn_cb_ptr = NULL;
/*----------------------------------------------------------------------*/
  /* lint fix */
  DATA_MDM_ARG_NOT_CONST(iface_ptr);

  if ( PS_IFACE_IS_VALID(iface_ptr) )
  {
    ds707_pdn_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(iface_ptr);
    ds707_pdncntx_set_requesting_profile_id(ds707_pdn_cb_ptr,
                                            requesting_profile_id,
                                            subs_id);
  }
}

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_SET_IFACE_LINGER_STATUS

DESCRIPTION   This function sets the linger status in the data_session_info
              in pdn cb associated with this iface.

PARAMETERS    ps_iface_type *:  pointer to iface
              boolean        :  TRUE - IFACE lingering else FALSE

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_set_iface_linger_status
(
    ps_iface_type *iface_ptr,
    boolean       is_iface_lingering
)
{
  ds707_pdn_context_cb_type *ds707_pdn_cb_ptr = NULL;
/*----------------------------------------------------------------------*/
  /* lint fix */
  DATA_MDM_ARG_NOT_CONST(iface_ptr);

  if ( PS_IFACE_IS_VALID(iface_ptr) )
  {
    ds707_pdn_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(iface_ptr);
    ds707_pdncntx_set_iface_linger_status(ds707_pdn_cb_ptr, is_iface_lingering);
  }
}

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_GET_IFACE_LINGER_STATUS

DESCRIPTION   This function reads the linger status from data_session_info
              in pdn cb associated with this iface.

PARAMETERS    ps_iface_type *:  pointer to iface
              boolean *      :  TRUE - IFACE lingering else FALSE

DEPENDENCIES  None

RETURN VALUE  int 0- success
                  1- failure

SIDE EFFECTS  None
===========================================================================*/
int ds707_data_session_get_iface_linger_status
(
  ps_iface_type *iface_ptr,
  boolean       *is_iface_lingering
)
{
  ds707_data_session_info_type* session_info_ptr = NULL;
  ds707_pdn_context_cb_type *ds707_pdn_cb_ptr = NULL;
/*----------------------------------------------------------------------*/
  /* lint fix */
  DATA_MDM_ARG_NOT_CONST(iface_ptr);

  if ( PS_IFACE_IS_VALID(iface_ptr) )
  {
    ds707_pdn_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(iface_ptr);
    if ( DS707_PDNCNTX_CB_IS_VALID(ds707_pdn_cb_ptr) )
    {
      session_info_ptr = ds707_pdncntx_get_data_session_info_ptr(ds707_pdn_cb_ptr);
      if ( DATA_SESSION_IS_VALID(session_info_ptr) )
      {
        *is_iface_lingering = session_info_ptr->is_iface_lingering;
        return 0;
      }
      else
      {
        DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                         "Could not get iface linger status. Invalid Data Session");
        return -1;
      }
    }
  }
  return -1;
}

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_GET_IFACE_PDN_ID

DESCRIPTION   This function reads the pdn id from data_session_info
              in pdn cb associated with this iface.

PARAMETERS    ps_iface_type *:  pointer to iface
              uint8 *       :  PDN ID associated with the IFACE

DEPENDENCIES  None

RETURN VALUE  int 0- success
                  1- failure

SIDE EFFECTS  None
===========================================================================*/
int ds707_data_session_get_iface_pdn_id
(
  ps_iface_type *iface_ptr,
  uint8         *pdn_id
)
{
  ds707_data_session_info_type * session_info_ptr = NULL;
  ds707_pdn_context_cb_type *ds707_pdn_cb_ptr = NULL;

  if ( PS_IFACE_IS_VALID(iface_ptr) )
  {
    ds707_pdn_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(iface_ptr);
    if ( DS707_PDNCNTX_CB_IS_VALID(ds707_pdn_cb_ptr) )
    {
      session_info_ptr = ds707_pdncntx_get_data_session_info_ptr(ds707_pdn_cb_ptr);
      if ( DATA_SESSION_IS_VALID(session_info_ptr) )
      {
        *pdn_id = session_info_ptr->pdn_id;
        return 0;
      }
      else
      {
        DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                         "Could not get iface pdn id. Invalid Data Session" );
        return -1;
      }
    }
  }
  return -1;
}

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_SET_IFACE_PDN_ID

DESCRIPTION   This function sets the pdn id in the data_session_info
              in pdn cb associated with this iface.

PARAMETERS    ps_iface_type *:  pointer to iface
              uint8         :  PDN ID to be associated with the IFACE

DEPENDENCIES  None

RETURN VALUE  int 0- success
                  1- failure

SIDE EFFECTS  None
===========================================================================*/
int ds707_data_session_set_iface_pdn_id
(
  ps_iface_type *iface_ptr,
  boolean       pdn_id
)
{
  ds707_data_session_info_type * session_info_ptr = NULL;
  ds707_pdn_context_cb_type *ds707_pdn_cb_ptr = NULL;
/*----------------------------------------------------------------------*/
  /* lint fix */
  DATA_MDM_ARG_NOT_CONST(iface_ptr);

  if ( PS_IFACE_IS_VALID(iface_ptr) )
  {
    ds707_pdn_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(iface_ptr);
    if ( DS707_PDNCNTX_CB_IS_VALID(ds707_pdn_cb_ptr) )
    {
      session_info_ptr = ds707_pdncntx_get_data_session_info_ptr(ds707_pdn_cb_ptr);
      if ( DATA_SESSION_IS_VALID(session_info_ptr) )
      {
        DATA_IS707_MSG1( MSG_LEGACY_MED, "Setting iface pdn id to %d",
                         pdn_id );
        session_info_ptr->pdn_id = pdn_id;
        return 0;
      }
      else
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                        "Could not set iface pdn id. Invalid Data Session");
        return -1;
      }
    }
  }
  return -1;
}

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_GET_IFACE_APN

DESCRIPTION   This function reads the APN from data_session_info
              in pdn cb associated with this iface.

PARAMETERS    ps_iface_type *:  pointer to iface
              uint8*         :  APN Length
              char*          :  APN String

DEPENDENCIES  None

RETURN VALUE  int 0- success
                  1- failure

SIDE EFFECTS  None
===========================================================================*/
int ds707_data_session_get_iface_apn
(
  ps_iface_type *iface_ptr,
  uint8*        apn_length,
  char*         apn_string

)
{
  ds707_data_session_info_type * session_info_ptr = NULL;
  ds707_pdn_context_cb_type *ds707_pdn_cb_ptr = NULL;

  if ( PS_IFACE_IS_VALID(iface_ptr) )
  {
    ds707_pdn_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(iface_ptr);
    if ( DS707_PDNCNTX_CB_IS_VALID(ds707_pdn_cb_ptr) )
    {
      session_info_ptr = ds707_pdncntx_get_data_session_info_ptr(ds707_pdn_cb_ptr);
      if ( DATA_SESSION_IS_VALID(session_info_ptr) )
      {
        if (session_info_ptr->apn_string_len > 0 &&
            session_info_ptr->apn_string_len <= DS_VSNCP_3GPP2_APN_MAX_VAL_LEN )
        {
          *apn_length = session_info_ptr->apn_string_len;
          (void) memscpy( (void *)apn_string,
                          DS_VSNCP_3GPP2_APN_MAX_VAL_LEN,
                          (void *)session_info_ptr->apn_string,
                          session_info_ptr->apn_string_len );
          return 0;
        }
        else
        {
          *apn_length = 0;
          return -1;
        }
      }
      else
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                      "Could not get iface APN. Invalid Data Session");
        return -1;
      }
    }
  }
  return -1;
}

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_SET_IFACE_APN

DESCRIPTION   This function sets the APN in the data_session_info
              in pdn cb associated with this iface.

PARAMETERS    ps_iface_type *:  pointer to iface
              uint8          :  APN Length
              char*          :  APN String

DEPENDENCIES  None

RETURN VALUE  int 0- success
                  1- failure

SIDE EFFECTS  None
===========================================================================*/
int ds707_data_session_set_iface_apn
(
  ps_iface_type *iface_ptr,
  uint8         apn_length,
  char*         apn_string

)
{
  ds707_data_session_info_type * session_info_ptr = NULL;
  ds707_pdn_context_cb_type *ds707_pdn_cb_ptr = NULL;
/*----------------------------------------------------------------------*/
  /* lint fix */
  DATA_MDM_ARG_NOT_CONST(iface_ptr);

  if ( PS_IFACE_IS_VALID(iface_ptr) )
  {
    ds707_pdn_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(iface_ptr);
    if ( DS707_PDNCNTX_CB_IS_VALID(ds707_pdn_cb_ptr) )
    {
      session_info_ptr = ds707_pdncntx_get_data_session_info_ptr(ds707_pdn_cb_ptr);
      if ( DATA_SESSION_IS_VALID(session_info_ptr) )
      {
        if ( apn_length > 0 &&
             apn_length <= DS_VSNCP_3GPP2_APN_MAX_VAL_LEN )
        {
          session_info_ptr->apn_string_len = apn_length;
          (void) memscpy( (void *)session_info_ptr->apn_string,
                          DS_VSNCP_3GPP2_APN_MAX_VAL_LEN,
                          (void *)apn_string,
                          session_info_ptr->apn_string_len );
          return 0;
        }
        else
        {
          session_info_ptr->apn_string_len = 0;
          return -1;
        }
      }
      else
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                        "Could not set iface APN. Invalid Data Session");
        return -1;
      }
    }
  }
  return -1;
}

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_GET_IFACE_IS_V6_DELEGATION_ENABLED

DESCRIPTION   This function sets the APN in the data_session_info
              in pdn cb associated with this iface.

PARAMETERS    ps_iface_type *:  pointer to iface
              uint8          :  APN Length
              char*          :  APN String

DEPENDENCIES  None

RETURN VALUE  int 0- success
                  1- failure

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_session_get_iface_is_v6_delegation_enabled
(
  ps_iface_type *iface_ptr
)
{
  ds707_data_session_profile_info_type *data_sess_profile_info_ptr = NULL;

  if ( PS_IFACE_IS_VALID(iface_ptr) )
  {
    if (TRUE == ds707_data_session_get_iface_requesting_profile(
                                                   iface_ptr,
                                                   &data_sess_profile_info_ptr))
    {
      return data_sess_profile_info_ptr->ipv6_delegation;
    }
  }

  return FALSE;
}/* ds707_data_session_get_iface_is_v6_delegation_enabled */

/*=============================================================================
FUNCTION      DS707_DATA_SESSION_RESOLVE_IFACE_ADDR_FAMILY

DESCRIPTION   This function resolves the iface addr family in case the IFACE
              is not in use. If the addr family passed in the policy is
              UNSPEC then for eHRPD profiles and eHRPD mode of operation, the
              iface address family is the PDN IP family mentioned in the
              profile. For HRPD profiles or HRPD mode of operation, it defaults
              iface address family to V4.

              for eHRPD mode, if the policy is UNSPEC and the pdn_type is
              v4v6 then use failover ip read from NV

PARAMETERS    ds707_data_session_profile_id profile_id
              acl_policy_info_type          *policy_info_ptr

DEPENDENCIES  None

RETURN VALUE  ip_addr_enum_type addr_family - resolved iface address family


SIDE EFFECTS  None
=============================================================================*/
ip_addr_enum_type ds707_data_session_resolve_iface_addr_family
(
  ds707_data_session_profile_id profile_id,
  acl_policy_info_type          *policy_info_ptr
)
{
  ds707_data_session_profile_id profile_reg_index;
  ip_addr_enum_type             addr_family = IFACE_INVALID_ADDR_FAMILY;
/*---------------------------------------------------------------------------*/

  DS_ENTER_CRIT_SECTION(&ds707_profile_crit_sect);

  profile_reg_index = ds707_data_session_is_valid_profile(profile_id);
  if ( (profile_reg_index == DATA_SESSION_PROFILE_ID_INVALID) ||
       (policy_info_ptr == NULL) )
  {
    DS_LEAVE_CRIT_SECTION(&ds707_profile_crit_sect);

    DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                    "Invalid profile id %d or nvalid policy info ptr "
                    "passed to set iface family",
                    profile_id);
    return addr_family;
  }

  /*-------------------------------------------------------------------------
    if policy is UNSPEC then for eHRPD profiles and mode of operation use
    the information from the profile (PDN IP version), if set to v4_v6 then
    use failover ip type NV. If non-eHRPD then set the iface as V4
  -------------------------------------------------------------------------*/
  if ( policy_info_ptr->ip_family == IFACE_UNSPEC_ADDR_FAMILY )
  {
    if ((ds707_data_session_profile_info[profile_reg_index].profile_type
         & PROFILE_EHRPD)
#ifdef FEATURE_EHRPD
        &&
        (ds707_pkt_is_ehrpd_mode_of_operation())
#endif
        )
    {
      if (ds707_data_session_profile_info[profile_reg_index].pdn_type ==
          DS707_DATA_SESS_PDN_TYPE_V4)
      {
        addr_family = IFACE_IPV4_ADDR_FAMILY;
      }
#ifdef FEATURE_DATA_PS_IPV6
      else if (ds707_data_session_profile_info[profile_reg_index].pdn_type ==
               DS707_DATA_SESS_PDN_TYPE_V6)
      {
        addr_family = IFACE_IPV6_ADDR_FAMILY;
      }
      else if (ds707_data_session_profile_info[profile_reg_index].pdn_type ==
               DS707_DATA_SESS_PDN_TYPE_V4_V6)
      {
        addr_family = ds707_pkt_mgri_get_ip_failover_mode();
      }
#endif /* FEATURE_DATA_PS_IPV6 */
      else /* this should never hit, just for the sake of completion */
      {
        DS_LEAVE_CRIT_SECTION(&ds707_profile_crit_sect);
        DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                         "PDN type invalid" );
        return addr_family;
      }
    }/* if eHRPD profile and mode of operation */
    else
    {
      /* for HRPD profile and mode of operation this is set to V4 by default */
      addr_family = IFACE_IPV4_ADDR_FAMILY;
    }
  } /* UNSPEC policy */
  else
  {
    addr_family = policy_info_ptr->ip_family;
  }

  DS_LEAVE_CRIT_SECTION(&ds707_profile_crit_sect);

  DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                  "Resolved IFACE addr family as %d",
                   addr_family);

  return addr_family;
}/* ds707_data_session_resolve_iface_addr_family */

/*=============================================================================
FUNCTION      DS707_DATA_SESSION_IP_VERSION_MATCH

DESCRIPTION   This function checks whether IP version mentioned in the profile
              matches the IP version mentioned in the App policy. Currently
              this function is used only for eHRPD profiles as only eHRPD
              profiles mention IP version i.e. PDN Type in the profiles. This
              also matches the IFACE address family with profile IP version.
              This is valid only if the IFACE is in use.

PARAMETERS    ds707_data_session_profile_id profile_id
              acl_policy_info_type          *policy_info_ptr
              ps_iface_type                 *if_ptr

DEPENDENCIES  None

RETURN VALUE  Boolean - TRUE if two IP version are same else FALSE

SIDE EFFECTS  None
=============================================================================*/
boolean ds707_data_session_ip_version_match
(
  ds707_data_session_profile_id profile_id,
  acl_policy_info_type          *policy_info_ptr,
  ps_iface_type                 *if_ptr
)
{
  ds707_data_session_profile_id profile_reg_index;
  boolean                       ret_val = FALSE;
/*-------------------------------------------------------------------------*/
  profile_reg_index = ds707_data_session_is_valid_profile(profile_id);
  if ( (profile_reg_index == DATA_SESSION_PROFILE_ID_INVALID) ||
       (policy_info_ptr == NULL) ||
       (if_ptr == NULL) )
  {
    DATA_IS707_MSG3(MSG_LEGACY_ERROR,
                    "Invalid profile id %d or policy info ptr 0x%x "
                    "or iface ptr 0x%x passed to IP Version match check",
                    profile_id, policy_info_ptr, if_ptr);
    return FALSE;
  }

  ret_val = ds707_data_session_policy_and_profile_match(profile_reg_index,
                                                        policy_info_ptr);
  /* If the IFACE is in use then match the iface addr family with the profile
     pdn ip version type */

  if ((ret_val == TRUE) && ( PS_IFACE_IS_IN_USE ( if_ptr ) ))
  {
    ret_val = ds707_data_session_iface_and_profile_match(profile_reg_index,
                                                         if_ptr);
  }
    
  return ret_val;
}

/*=============================================================================
FUNCTION      DS707_DATA_SESSION_SUBS_ID_MATCH

DESCRIPTION   This function checks whether subs id mentioned in the profile
              matches the subs id mentioned in the App policy.

PARAMETERS    ds707_data_session_profile_id profile_id
              acl_policy_info_type          *policy_info_ptr

DEPENDENCIES  None

RETURN VALUE  Boolean - TRUE if two subs ID are same else FALSE

SIDE EFFECTS  None
=============================================================================*/
boolean ds707_data_session_subs_id_match
(
  ds707_data_session_profile_id profile_id,
  acl_policy_info_type          *policy_info_ptr
)
{
  ds707_data_session_profile_id profile_reg_index;
  boolean  retval = FALSE;
/*-------------------------------------------------------------------------*/

  if( (policy_info_ptr == NULL) ||
      (policy_info_ptr->subs_id == PS_SYS_DEFAULT_SUBS) )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "Invalid Policy info pointer or Policy has DEFAULT SUBS ID");
    return FALSE;
  }

  DS_ENTER_CRIT_SECTION(&ds707_profile_crit_sect);

  profile_reg_index = ds707_data_session_is_valid_profile(profile_id);
  if (profile_reg_index == DATA_SESSION_PROFILE_ID_INVALID) 
  {
    DS_LEAVE_CRIT_SECTION(&ds707_profile_crit_sect);
    DATA_IS707_MSG2(MSG_LEGACY_ERROR,
                    "Invalid profile id %d or policy info ptr 0x%x "
                    "passed to Subs ID match check",
                    profile_id, policy_info_ptr);
    return FALSE;
  }


  if ( policy_info_ptr->subs_id == 
         (uint32)ds707_data_session_profile_info[profile_reg_index].subs_id )
  {
    retval = TRUE;
  }
  else
  {
    DATA_IS707_MSG2(MSG_LEGACY_HIGH,
                    "Profile PS Subs ID %d and policy PS Subs ID mismatch",
                    ds707_data_session_profile_info[profile_reg_index].subs_id,
                    policy_info_ptr->subs_id);
  }

  DS_LEAVE_CRIT_SECTION(&ds707_profile_crit_sect);

  return retval;

} /* ds707_data_session_subs_id_match */

#ifdef FEATURE_EHRPD
/*=============================================================================
FUNCTION      DS707_DATA_SESSION_EHRPD_FRIEND_PROFILES

DESCRIPTION   This function checks whether the two Ehrpd profiles are friends of
              each other or not. The criterion for friends profile is that they
              should have the same APN string

PARAMETERS    ds707_data_session_profile_info_type *profileA_ptr,
              ds707_data_session_profile_info_type *profileB_ptr,
              acl_policy_info_type          *policy_info_ptr

DEPENDENCIES  Input pointers are non null

RETURN VALUE  Boolean - TRUE if the two profiles are friends else FALSE

SIDE EFFECTS  None
=============================================================================*/
boolean ds707_data_session_ehrpd_friend_profiles
(
  ds707_data_session_profile_info_type *profileA_ptr,
  ds707_data_session_profile_info_type *profileB_ptr,
  acl_policy_info_type          *policy_info_ptr
)
{
/*---------------------------------------------------------------------------*/

  /*------------------------------------------------------------------------- 
    Caller needs to make sure valid profiles are passed to ehrpd friend
    profiles check. If the new call profile is valid while the profile
    stored on PDN CB is invalid, it means there was no previous call and
    caller should skip this check.
   ------------------------------------------------------------------------*/
  if ((profileA_ptr == NULL) || (profileB_ptr == NULL))
  {
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Apart from ehrpd mode of operation we need to check if this compatibility
    is run for handover from eHRPD to LTE. In that case mode of operation
    is not eHRPD since system has moved to LTE. But we still need to perform
    APN check.
    We need to check for last known good sys becuase when AT is in
    undeterministic RAT the other checks will fail & compatibility check can
    be skipped for ehrpd calls
  -------------------------------------------------------------------------*/
  if(ds707_pkt_is_ehrpd_mode_of_operation() ||
#ifdef FEATURE_EPC_HANDOFF
     ds707_is_policy_handoff_class_epc(policy_info_ptr) ||
#endif /* FEATURE_EPC_HANDOFF */
      ds707_pkt_mgr_is_data_on_ehrpd() == TRUE ||
      (ds707_get_last_good_sys() == DS707_SYS_INFO_EPC_SYSTEM))
  {
    /* Check the APN length for two profiles.
     If the length matches then check the APN string
     If both profiles have the sam APN string then return TRUE else FALSE */

    if (profileA_ptr->apn_string_len ==
        profileB_ptr->apn_string_len)
    {
      if (0 == strncasecmp(profileA_ptr->apn_string,
                           profileB_ptr->apn_string,
                           profileA_ptr->apn_string_len))
      {
        DATA_IS707_MSG_SPRINTF_3(MSG_LEGACY_HIGH,
                        "ehrpd_friend_profiles: Same APN: %s in both Profiles: "
                        "%d and %d. Return TRUE",
                        profileA_ptr->apn_string,
                        profileA_ptr->data_session_profile_id,
                        profileB_ptr->data_session_profile_id);

        return TRUE;
      }
    }
  }
  else
  {
    /* We need to return true even if two non-default profiles
       are attempting call bring up in a non-ehrpd domain
       e.g 2 user created profiles
           1 user created and 1 eHRPD profile (101..107)
           1 user created and default profile etc. */
    DATA_IS707_MSG2(MSG_LEGACY_HIGH,
                    "ds707_data_session_ehrpd_friend_profiles: "
                    "sys is not ehrpd or policy type is not handoff."
                    "Skip compatibility check for profile %d and %d "
                    "and return true",
                    profileA_ptr->data_session_profile_id,
                    profileB_ptr->data_session_profile_id);
    return TRUE;
  }
  DATA_IS707_MSG2(MSG_LEGACY_ERROR,
                  "Profiles %d and %d are not compatible",
                  profileA_ptr->data_session_profile_id,
                  profileB_ptr->data_session_profile_id);
  return FALSE;
} /* ds707_data_session_ehrpd_friend_profiles */
#endif

/*=============================================================================
FUNCTION      DS707_GET_MIP_PROFILE_INDEX

DESCRIPTION   This function returns the stored mip profile index.
              (ds707_mip_profile_index)

PARAMETERS

DEPENDENCIES  None

RETURN VALUE  int8 - mip index

SIDE EFFECTS  None
=============================================================================*/
int8 ds707_get_mip_profile_index(void)
{
  return ds707_mip_profile_index;
}

/*=============================================================================
FUNCTION      DS707_SET_MIP_PROFILE_INDEX

DESCRIPTION   This function sets the mip profile index.
              (ds707_mip_profile_index)

PARAMETERS    int8 - Profile Index.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
=============================================================================*/
void ds707_set_mip_profile_index( int8 profile_index )
{
  ds707_mip_profile_index = profile_index;

  return;
}

/*=============================================================================
FUNCTION      DS707_GET_MIP_INDEX_FROM_PROFILE_ID

DESCRIPTION   This function calculates the mip profile index from the profile
              id. This index is used to read mip parameters from NV.
              MIP profile ids range is from 151 - 200. Number of
              valid MIP profiles are 6. Profile id 151 maps to 1st MIP
              profile in NV (index 0) and so on
              If profile id passed to this function is not a MIP profile,
              the index is the active MIP index from NV

PARAMETERS    ds707_data_session_profile_id profile_id

DEPENDENCIES  None

RETURN VALUE  int8 - mip index

SIDE EFFECTS  None
=============================================================================*/
int8 ds707_get_mip_index_from_profile_id(
  ds707_data_session_profile_id profile_id
)
{
  /*-----------------------------------------------------------------------
    If profile id specified is MIP profile, then calculate the index
    else for other profiles, it is the active  mip profile specified in NV.
    This NV is read at powerup in ds707_data_session_init()
  ----------------------------------------------------------------------- */
  ds707_mip_profile_index = ds707_active_nv_mip_profile;
  if( PROFILE_ID_IS_MIP(profile_id) )
  {
    /*-----------------------------------------------------------------------
      index = 151 - 151 = 0. Profile id 151 indexes at the 0th location in
      NV, 152 at 1st and so on.
    ------------------------------------------------------------------------*/
    ds707_mip_profile_index = (int8)(profile_id - DATA_SESSION_MIP_PROFILE_MIN);
  }

  DATA_IS707_MSG2(MSG_LEGACY_MED,
                  "_get_mip_index_from_profile_id: mip index %d from profile id %d",
                  ds707_mip_profile_index, profile_id);
  return ds707_mip_profile_index;
}

/*=============================================================================
FUNCTION      DS707_DATA_SESSION_MIP_FRIEND_PROFILES

DESCRIPTION   This function checks if 2 profiles are friends, when one of them
              is a MIP profile.
   1. Current = MIP   requesting = MIP   --> PASS only if the profile ids
                                             are equal
   2. Current = MIP && requesting = default
       2.1 MIP not matching with NV active profile --> FAIL
       2.2 MIP matching with NV active profile     --> PASS
   3. Current = default && requesting = MIP
       3.1 MIP not matching with NV active profile --> FAIL
       3.2 MIP matching with NV active profile     --> PASS

   All other conditions FAIL

PARAMETERS    ds707_data_session_profile_id requesting_id,
              ds707_data_session_profile_id current_id

DEPENDENCIES  None

RETURN VALUE  Boolean - TRUE if the two profiles are friends else FALSE

SIDE EFFECTS  None
=============================================================================*/
boolean ds707_data_session_mip_friend_profiles
(
  ds707_data_session_profile_id requesting_id,
  ds707_data_session_profile_id current_id
)
{
  int8 active_mip_profile = 0;
  boolean ret_val = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
  Current = MIP and requesting = MIP -> PASS only if the profile ids are equal
  --------------------------------------------------------------------------*/
  if ( PROFILE_ID_IS_MIP(current_id) &&
       PROFILE_ID_IS_MIP(requesting_id) )
  {
    if ( current_id == requesting_id )
    {
      ret_val = TRUE;
    }
    DATA_IS707_MSG3( MSG_LEGACY_MED,
                     "_mip_friend_profiles: curr %d, req %d return %d",
                     current_id, requesting_id, ret_val );
    return ret_val;
  }
  else if( PROFILE_ID_IS_MIP(current_id) &&
           requesting_id == DATA_SESSION_DEFAULT_PROFILE )
  {
    /*-------------------------------------------------------------------------
      Current = MIP && requesting = default
       MIP not matching with NV active profile --> FAIL
       MIP matching with NV active profile     --> PASS
    --------------------------------------------------------------------------*/
    if ( ds707_get_mip_index_from_profile_id(current_id)
         == ds707_active_nv_mip_profile )
    {
      ret_val = TRUE;
    }
    DATA_IS707_MSG4( MSG_LEGACY_MED,
                     "_mip_friend_profiles: curr %d, req %d, active mip %d return %d",
                     current_id, requesting_id, active_mip_profile, ret_val );
    return ret_val;
  }
  else if ( PROFILE_ID_IS_MIP(requesting_id) &&
            current_id == DATA_SESSION_DEFAULT_PROFILE )
  {
    /*-------------------------------------------------------------------------
      Current = default && requesting = MIP
       MIP not matching with NV active profile --> FAIL
       MIP matching with NV active profile     --> PASS
    --------------------------------------------------------------------------*/
    if ( ds707_get_mip_index_from_profile_id(requesting_id)
         == ds707_active_nv_mip_profile )
    {
      ret_val = TRUE;
    }
    DATA_IS707_MSG4( MSG_LEGACY_MED,
                     "_mip_friend_profiles: curr %d, req %d, active mip %d return %d",
                     current_id, requesting_id, active_mip_profile, ret_val );
    return ret_val;
  }

  DATA_IS707_MSG0( MSG_LEGACY_MED, "_mip_friend_profiles: no conditions met return FALSE");
  return FALSE;
}

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_COMPATIBLE_PROFILES

DESCRIPTION   This function checks whether the two profiles are compatible or
              not depending upon the profile ids that are passed. It also
              checks if the iface is available to use or not. This iface
              availability check makes the laptop and socket calls
              non-compatible to each other.

PARAMETERS    ps_iface_type* - iface pointer
              acl_policy_info_type* - pointer to the acl policy
              ds707_data_session_profile_id requesting_id -
                profile id of the new requesting application
              ds707_data_session_profile_id current_id -
                profile id of the current application

DEPENDENCIES  None

RETURN VALUE  Boolean - TRUE if the two profiles are compatible else FALSE

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_session_compatible_profiles
(
  ps_iface_type                 *if_ptr,
  acl_policy_info_type          *policy_info_ptr,
  ds707_data_session_profile_id requesting_id,
  ds707_data_session_profile_info_type *current_prof_ptr
)
{
  ds707_data_session_profile_id requesting_profile_reg_index;
  ds707_data_session_profile_id current_id;
  ds707_mip_profile_type        *ds_1x_runtime_profile_ptr = NULL;
  ds_profile_type_enum_type     requesting_profile_type;
  uint8                         requesting_app_priority;
  ds707_data_session_profile_info_type 
                          *requesting_profile_info = NULL;
#ifdef FEATURE_EHRPD
  ds707_pdn_context_cb_type     *pdn_cb_ptr = NULL;
#endif /* FEATURE_EHRPD */
/*-------------------------------------------------------------------------*/

  /*------------------------------------------------------------------------ 
    If profile ptr is INVALID then the PDN CB is invalid so there is no
    current call so return true
   ------------------------------------------------------------------------*/
  if (current_prof_ptr == NULL)
  {
    return TRUE;
  }

  current_id = current_prof_ptr->data_session_profile_id;

  DATA_IS707_MSG2(MSG_LEGACY_HIGH,
                  "Checking compatibility for profile id %d and %d",
                  requesting_id, current_id);

  /* If current_id is INVALID then there is no current call so return true*/
  if (current_id == DATA_SESSION_PROFILE_ID_INVALID)
  {
    return TRUE;
  }

  ds_1x_runtime_profile_ptr = ds_get_1x_runtime_profile_ptr();

  /*-------------------------------------------------------------------------
    If current or requesting profile is a MIP profile, check MIP profiles
    compatiblity
  -------------------------------------------------------------------------*/
  if ( PROFILE_ID_IS_MIP(current_id) ||
       PROFILE_ID_IS_MIP(requesting_id) )
  {
    if ( ds707_data_session_mip_friend_profiles(requesting_id,current_id) )
    {
      DATA_IS707_MSG2(MSG_LEGACY_MED,
                      "requesting profile id %d compatible with current id %d",
                      requesting_id,
                      current_id);
      return TRUE;
    }
    else
    {
      DATA_IS707_MSG2(MSG_LEGACY_MED,
                      "requesting profile id %d not compatible with current id %d",
                      requesting_id,
                      current_id);
      return FALSE;
    }
  }

  DS_ENTER_CRIT_SECTION(&ds707_profile_crit_sect);
  
  /* Get the profile reg index and perform some sanity on that */
  requesting_profile_reg_index = ds707_data_session_is_valid_profile(requesting_id);
  if (requesting_profile_reg_index == DATA_SESSION_PROFILE_ID_INVALID)
  {
    DS_LEAVE_CRIT_SECTION(&ds707_profile_crit_sect);

    return FALSE;
  }

  requesting_profile_type = 
       ds707_data_session_profile_info[requesting_profile_reg_index].profile_type;
  requesting_profile_info = 
       &ds707_data_session_profile_info[requesting_profile_reg_index];
  requesting_app_priority =
       ds707_data_session_profile_info[requesting_profile_reg_index].app_priority;

  DS_LEAVE_CRIT_SECTION(&ds707_profile_crit_sect);

 /* Get the profile type. The profile type should match for both the profiles.
  But for checking the compatibilty between default profile and application
  specified profile, the profile type do NOT need to be exactly equaled, e.g.
  when JCDMA mode is on, the default profile type is COMMON|KDDI|EHRPD, while a
  KDDI profile type is COMMON|KDDI, they are not equaled, but actually, default
  profile can be the friend of a KDDI profile.*/
  if(
      DATA_SESSION_DEFAULT_PROFILE == requesting_profile_reg_index ||
      DATA_SESSION_DEFAULT_PROFILE == current_id ||
      (requesting_profile_type == current_prof_ptr->profile_type)
      )
  {
    /* check which bit is set in the profile type mask and call the appropriate
       friend function */
#ifdef FEATURE_EHRPD
    if (requesting_profile_type & PROFILE_EHRPD)
    {
      /* Call eHRPD specific friend profile function */
      if (ds707_data_session_ehrpd_friend_profiles(
            requesting_profile_info, current_prof_ptr, policy_info_ptr))
      {
        DATA_IS707_MSG2(MSG_LEGACY_HIGH, 
                        "EHRPD compatibility check passed for %d and %d",
                         requesting_id, current_id);
        return TRUE;
      }

      /* Allow NULL APN to latch onto default PDN if it exists */
      pdn_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(if_ptr);
      if ( ( TRUE == ds707_pdncntx_is_default_pdn(pdn_cb_ptr) ) &&
           ( ds707_pdncntx_is_default_apn(requesting_profile_info)
             == TRUE ) ) 
      {
        DATA_IS707_MSG1(MSG_LEGACY_HIGH, 
                        "Profile %d has default APN, allow it on default PDN",
                         requesting_id);
        return TRUE;
      }
    } /* if eHRPD type profile */
    else 
#endif
    if (requesting_profile_type & PROFILE_KDDI)
    {
      /* Call JCDMA specific friend profile function */
      if(ds707_data_session_friend_profiles(requesting_id, current_id))
      {
        DATA_IS707_MSG2(MSG_LEGACY_HIGH,
                        "JCDMA compatibility check passed for %d and %d",
                         requesting_id, current_id);
        return TRUE;
      }
    } /* if JCDMA type profile */
    else if( ds707_data_session_is_nvruim_profile(requesting_id) )
    {

      /* Skip the profile arbitration for MIP and MIP to SIP fallback cases */
     #ifdef FEATURE_DS_MOBILE_IP
      if( ds_qcmip_get_runtime_val () >  0 )
      {
        return TRUE;
      }
     #endif /* FEATURE_DS_MOBILE_IP */


      /*----------------------------------------------------------------------
        For Tethered call  priority/compatibility check is not  required.
        Thus returning TRUE here to by pass the ACL rule. We are removing the
        is_sock_orig check and using proc id instead. Proc_id 0 or 2 means that it
        is a embedded data call.       
      ----------------------------------------------------------------------*/
      if( PS_IFACE_IS_CALL_FROM_TETHERED_HOST(if_ptr) )
      {
        return TRUE;
      }
      /*---------------------------------------------------------------------------
        Return TRUE if 'is_routeable' is TRUE and Network Model Call is UP (as
        IFACE is shareable by routeable sockets) or it is a Lookup only call.
      ---------------------------------------------------------------------------*/
      else if( ( (policy_info_ptr->is_routeable) &&
                 (ds_1x_runtime_profile_ptr->ds707.ds707_crm_val ==
                                        DS_CRM_VAL_NETMODEL_MODE) &&
                 (ds707_rmsm_is_rm_iface_up() == TRUE) ) ||
                 (policy_info_ptr-> lookup_only) )
      {
        return TRUE;
      }
      else
      {
        if( ds707_data_session_profile_priority_check(
              requesting_app_priority, current_prof_ptr->app_priority ) )
        {
          return TRUE;
        }
      }
    }
    else if (requesting_profile_type & PROFILE_COMMON)
    {
      /* We need to return true even if two non-default profiles
         are attempting call bring up in a non-ehrpd domain
         e.g 2 user created profiles
             1 user created and 1 eHRPD profile (101..107) */
      DATA_IS707_MSG2(MSG_LEGACY_HIGH,
                      "ds707_data_session_compatible_profiles: "
                      "Profile Type: Common. Skip compatibility "
                      "check for profile id %d and %d and return true",
                      requesting_id, current_id);
      return TRUE;
    } /* if Common type profile */

  }

  return FALSE;
} /* ds707_data_session_compatible_profiles */

/*=============================================================================
FUNCTION      DS707_DATA_SESSION_FRIEND_PROFILES

DESCRIPTION   This function checks whether the two profiles are friends of
              each other or not depending upon the profile ids that are passed.

PARAMETERS    ds707_data_session_profile_id id_A - first profile id
              ds707_data_session_profile_id id_B - second profile id

DEPENDENCIES  None

RETURN VALUE  Boolean - TRUE if the two profiles are friends else FALSE

SIDE EFFECTS  None
=============================================================================*/
boolean ds707_data_session_friend_profiles
(
  ds707_data_session_profile_id id_A,
  ds707_data_session_profile_id id_B
)
{
  /*------------------------------------------------------------------------
    this logic is hardcoded for KDDI specific profiles. This is for checking
    friends logic for profiles:
    DATA_SESSION_EMAIL_WITH_ATTACHMENT_TYPE
    DATA_SESSION_EMAIL_WITHOUT_ATTACHMENT_TYPE
    DATA_SESSION_PSMS_TYPE
    DATA_SESSION_BREW_PCSV_TYPE
    These profiles are friends of each other because all of them allow Linger
    state.
  ------------------------------------------------------------------------*/
  if (
      ((id_A == DATA_SESSION_EMAIL_WITH_ATTACHMENT_PROFILE) ||
      (id_A == DATA_SESSION_EMAIL_WITHOUT_ATTACHMENT_PROFILE) ||
      (id_A == DATA_SESSION_PSMS_PROFILE) ||
      (id_A == DATA_SESSION_BREW_PCSV_PROFILE))
      &&
      ((id_B == DATA_SESSION_EMAIL_WITH_ATTACHMENT_PROFILE) ||
      (id_B == DATA_SESSION_EMAIL_WITHOUT_ATTACHMENT_PROFILE) ||
      (id_B == DATA_SESSION_PSMS_PROFILE) ||
      (id_B == DATA_SESSION_BREW_PCSV_PROFILE))
      )
  {
    return TRUE;
  }
  /*------------------------------------------------------------------------
    This logic is hardcoded for KDDI specific profiles. This is for checking
    friends logic for profiles:
    DATA_SESSION_BREW_IPVT_TYPE
    DATA_SESSION_BREW_PTM
    These profiles are friends of each other only in Active state but not
    in the Linger state. But because both these profiles do not ask for
    Linger state, we can safely assume that the state is always Active.
  ------------------------------------------------------------------------*/
  if (
      ((id_A == DATA_SESSION_BREW_IPVT_PROFILE) ||
      (id_A == DATA_SESSION_BREW_PTM_PROFILE))
      &&
      ((id_B == DATA_SESSION_BREW_IPVT_PROFILE) ||
      (id_B == DATA_SESSION_BREW_PTM_PROFILE))
     )
  {
    return TRUE;
  }

/*------------------------------------------------------------------------
  This logic is added to allow data connection requested with default
  profile ID (Internal App) or also for the concurrent Apps using the same
  profile ID.
--------------------------------------------------------------------------*/
  if( (id_A == id_B) ||
      (id_A == DATA_SESSION_DEFAULT_PROFILE) ||
      (id_B == DATA_SESSION_DEFAULT_PROFILE)
    )
  {
    return TRUE;
  }

  return FALSE;
}
/*==========================================================================
FUNCTION      DS707_ASSIGN_PPP_AUTH_INFO

DESCRIPTION   This function assigns ppp_auth_info of the given data session
              profile structure to the provided PPP configuration structure.

PARAMETERS    ppp_dev_opts_type * - PPP configuration structure
              ds707_data_session_profile_info_type - data session profile
              struct

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
==========================================================================*/
void ds707_assign_ppp_auth_info
(
    ppp_auth_info_type *ppp_auth_info_ptr,
    ds707_data_session_profile_info_type *data_sess_profile_info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* lint fix */
  DATA_MDM_ARG_NOT_CONST(data_sess_profile_info);

  ppp_auth_info_ptr->user_id_len =
    data_sess_profile_info->user_id_len;

  if (ppp_auth_info_ptr->user_id_len == 0)
  {
      DATA_IS707_MSG1(MSG_LEGACY_ERROR,
        "PPP user id never written for id:%d",
        data_sess_profile_info->data_session_profile_id
        );
  }
  else
  {
    (void)memscpy(ppp_auth_info_ptr->user_id_info,
	          PPP_MAX_USER_ID_LEN,
                  data_sess_profile_info->user_id,
                  ppp_auth_info_ptr->user_id_len );
  }

  ppp_auth_info_ptr->passwd_len =
    data_sess_profile_info->auth_password_len;

  if (ppp_auth_info_ptr->passwd_len == 0)
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR,
        "PPP password never written for id:%d",
        data_sess_profile_info->data_session_profile_id
        );
  }
  else
  {
    (void)memscpy(ppp_auth_info_ptr->passwd_info,
                  PPP_MAX_PASSWD_LEN,
                  data_sess_profile_info->auth_password,
                  ppp_auth_info_ptr->passwd_len);
  }
}

/*===========================================================================
FUNCTION DS707_DATA_SESSION_GET_AUTH_INFO_FOR_DEFAULT_PROFILE

DESCRIPTION
  This function retrieves the 1x PPP user ID and password from NV.

DEPENDENCIES
  The NV task must be started and running.

RETURN VALUE
  None

SIDE EFFECTS
  Each call to 'ds3gcfgmgr_read_legacy_nv_ex' results in a Wait and watchdog kicking
===========================================================================*/
void ds707_data_session_get_auth_info_for_default_profile
(
  char   * user_id,          /* char array of max size PPP_MAX_USER_ID_LEN. OUTPUT param*/
  uint8  * user_id_len,      /* uint8 length value. OUTPUT param*/
  char   * auth_password,    /* char array of max size PPP_MAX_PASSWD_LEN. OUTPUT param */
  uint8  * auth_password_len /* uint length value. OUTPUT param */
)
{
  nv_stat_enum_type    nv_status;          /* status from NV call          */
  nv_item_type         nv_item;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Read the PPP User ID from NV.
  -------------------------------------------------------------------------*/
  nv_status = ds3gcfgmgr_read_legacy_nv_ex( NV_PPP_USER_ID_I, &nv_item, ds707_curr_subs_id() );

  /*-------------------------------------------------------------------------
    If NV read succeeds, load the user ID into the provided ppp config
  -------------------------------------------------------------------------*/
  if( nv_status == NV_DONE_S)
  {
    *user_id_len = nv_item.ppp_user_id.user_id_len;
    memscpy( user_id, *user_id_len, 
             (char *)nv_item.ppp_user_id.user_id, *user_id_len );
  }

  /*-------------------------------------------------------------------------
    If NV was never written then the length of the User_id is set to 0.
    This will ensure that no user ID is included in the AP response.
  -------------------------------------------------------------------------*/
  else
  {
    /*-----------------------------------------------------------------------
      Set the user_id length to 0.
    -----------------------------------------------------------------------*/
    *user_id_len = 0;
  }

  /*-------------------------------------------------------------------------
    Read the PPP password from NV.  When using RUIM, this will return only
    the PAP password.  The CHAP password cannot be retrived, and is maintained
    only in the card to be used when a CHAP command is issued
  -------------------------------------------------------------------------*/
  nv_status = ds3gcfgmgr_read_legacy_nv_ex( NV_PPP_PASSWORD_I, &nv_item, ds707_curr_subs_id() );

  /*-------------------------------------------------------------------------
    If NV read succeeded, load the password into the provided ppp config
  -------------------------------------------------------------------------*/
  if( nv_status == NV_DONE_S)
  {
    *auth_password_len = nv_item.ppp_password.password_len;
    memscpy( auth_password, *auth_password_len, (char *)nv_item.ppp_password.password,
             *auth_password_len );
  }
  else
  {
#ifdef FEATURE_DATA_PPP_DEFAULT_PASSWD

    /*-----------------------------------------------------------------------
      Whether the NV write succeeded or not, set PPP config appropriately
    -----------------------------------------------------------------------*/
    if( PPP_DEFAULT_SN_PASSWORD_LEN <= PPP_MAX_PASSWD_LEN )
    {
      *auth_password_len = PPP_DEFAULT_SN_PASSWORD_LEN;
      memscpy( auth_password,
	       PPP_DEFAULT_SN_PASSWORD_LEN
               PPP_DEFAULT_SN_PASSWORD,
               PPP_DEFAULT_SN_PASSWORD_LEN );
    }
    else
    {
      *auth_password_len = 0;
    }
#else
    /*-----------------------------------------------------------------------
      Set the user_id length as 0.
    -----------------------------------------------------------------------*/
    *auth_password_len = 0;
#endif /* FEATURE_DATA_PPP_DEFAULT_PASSWD */
  }
} /* ds707_data_session_get_auth_info_for_default_profile() */

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_GET_AUTH_INFO_FROM_PROFILE

DESCRIPTION   The caller of this function provides the profile id. This API
              then retrieves the user id and pasword associated with that
              profile and returns them back to the caller in supplied
              output parameters.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
int8 ds707_data_session_get_auth_info_from_profile
(
 uint8    profile_id,       /* Input Parameter */
 char   * user_id,          /* char array of max size PPP_MAX_USER_ID_LEN. OUTPUT param*/
 uint8  * user_id_len,      /* uint8 length value. OUTPUT param*/
 char   * auth_password,    /* char array of max size PPP_MAX_PASSWD_LEN. OUTPUT param */
 uint8  * auth_password_len /* uint length value. OUTPUT param */
)
{

  *user_id_len = 0;
  *auth_password_len = 0;
  memset(auth_password, 0, PPP_MAX_PASSWD_LEN);
  memset(user_id, 0, PPP_MAX_USER_ID_LEN);

  if (profile_id >= (uint8)ds707_data_session_max_profiles[SYS_MODEM_AS_ID_1])
  {
    DATA_IS707_MSG1( MSG_LEGACY_HIGH,
     "Invalid profile id: %d passed to get auth credentials", profile_id );
    return -1;
  }

  if (profile_id == DATA_SESSION_DEFAULT_PROFILE)
  {
    ds707_data_session_get_auth_info_for_default_profile(user_id,
                   user_id_len, auth_password, auth_password_len);
  }
  else
  {
    memscpy(user_id,
	    PPP_MAX_USER_ID_LEN,
            ds707_data_session_profile_info[profile_id].user_id,
            ds707_data_session_profile_info[profile_id].user_id_len );

    *user_id_len = ds707_data_session_profile_info[profile_id].user_id_len;

    memscpy(auth_password,
	    PPP_MAX_USER_ID_LEN,
            ds707_data_session_profile_info[profile_id].auth_password,
            ds707_data_session_profile_info[profile_id].auth_password_len );

    *auth_password_len = ds707_data_session_profile_info[profile_id].auth_password_len;
  }

  return 0;

} /* ds707_data_session_get_auth_info_from_profile */

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_SET_LINGER_TMR_VAL

DESCRIPTION   This function sets the Linger timer value according to the data
              session profile id and the underlying bearer technology (DO/1X)
              if the configuration option is set to DYNAMIC_LINGER_CONFIG. In
              case of STATIC_LINGER_CONFIG option, the Linger timer value is
              set to some positive value.

PARAMETERS    ps_iface_type * - Iface ptr for which Liger timer is to be set
              ds_linger_tmr_config_enum_type - Config option

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_set_linger_tmr_val
(
  ps_iface_type                   *if_ptr,
  ds_linger_tmr_config_enum_type  config_option
)
{
  ds707_data_session_profile_info_type   *data_sess_profile_info_ptr = NULL;
  uint32                                 linger_timeout_val = 0;
  /*---------------------------------------------------------------------
    Set the Linger timer value according to the data session profile.
  ---------------------------------------------------------------------*/
  /*----------------------------------------------------------------------
    If current data session profile id is valid then use that, otherwise
    use the requesting data session profile id. When we set up the Linger
    value in the linger_cmd_f_ptr we want to use current data session
    profile id. Thus this check is added.
  ----------------------------------------------------------------------*/
  if (FALSE == ds707_data_session_get_iface_current_profile(
                                                   if_ptr,
                                                   &data_sess_profile_info_ptr))
  {

    if (FALSE == ds707_data_session_get_iface_requesting_profile(
                                                   if_ptr,
                                                   &data_sess_profile_info_ptr))
  {
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, 
                      "Fail to set linger timer, invalid profile for iface: 0x%p",
                      if_ptr);
      return;
    }
  }

  if (data_sess_profile_info_ptr->allow_linger == TRUE)
  {
    /*-----------------------------------------------------------------------
      Decide on the Linger timer value to set depending upon the config
      option.
      If config option is DYNAMIC_LINGER_CONFIG then set the timer according
      to the underlying bearer technology.
      If config option is STATIC_LINGER_CONFIG then set the timer with some
      positive value.
    -----------------------------------------------------------------------*/

    DATA_IS707_MSG1(MSG_LEGACY_MED, "mode %d", config_option);
    switch ( config_option )
    {
      case DYNAMIC_LINGER_CONFIG:
      {
        if(ds707_extif_is_pkt_data_sess_on_cdma()
           || ds707_extif_is_pkt_data_sess_on_hdr())
        {
          linger_timeout_val
                      = (uint32)data_sess_profile_info_ptr->linger_timeout_val;
          DATA_IS707_MSG1(MSG_LEGACY_MED, "Linger Timer set in 1X/DO Mode for %d seconds",
                  data_sess_profile_info_ptr->linger_timeout_val);
        }
        else
        {
          /* We should not get here */
          linger_timeout_val = 0;
          DATA_IS707_MSG0(MSG_LEGACY_ERROR, "The Current Data Session is not HDR or 1X. Setting Linger timer to 0.");
        }
        break;
      } /* case DYNAMIC_LINGER_CONFIG */

      case STATIC_LINGER_CONFIG:
      {
        linger_timeout_val = 2; /* this is some positive value which no significance. */
        break;
      } /* case STATIC_LINGER_CONFIG */
    } /* switch */

  } /* if (data_sess_profile_info_ptr->allow_linger == TRUE) */
  else
  {
    DATA_IS707_MSG1(MSG_LEGACY_MED, 
                    "Linger Timer not required for iface: 0x%p",
                    if_ptr);
  }
  if (ps_iface_set_linger_timeout_val(
          if_ptr,
          (SEC_TO_MSEC(linger_timeout_val)) )
        < 0)
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, 
                    "Could not set Linger Timer value for iface: 0x%p",
                    if_ptr);
  }
  else
  {
    DATA_IS707_MSG2(MSG_LEGACY_HIGH, 
                    "Set Linger Timer value: %d sec for iface: 0x%p",
                    linger_timeout_val, if_ptr);
  }
} /* ds707_data_session_set_linger_tmr_val */

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_GET_REG_INDEX

DESCRIPTION   This function loops through all the profiles and matches given
              profile id with profile id present in the profile registry.
              If there is a match then it returns the registry index, else
              returns -1.

PARAMETERS    ds707_data_session_profile_id

DEPENDENCIES  None

RETURN VALUE  reg index - if profile id matches
              -1 - if none of the profiles contain the given profile id

SIDE EFFECTS  None
===========================================================================*/
ds707_data_session_profile_id ds707_data_session_get_reg_index
(
  ds707_data_session_profile_id profile_id
)
{
  ds707_data_session_profile_id profile_reg_index;
  ds707_data_session_profile_id ret_val = -1;

  /*-------------------------------------------------------------------------
    If profile id is MIP, return 0 as reg_index. So for all non-MIP
    parameters default profile will be used.
  ------------------------------------------------------------------------- */
  if ( PROFILE_ID_IS_MIP(profile_id) )
  {
    ret_val = DATA_SESSION_DEFAULT_PROFILE;
    return ret_val;
  }

  for (profile_reg_index = DATA_SESSION_PROFILE_ID_MIN;
       profile_reg_index < ds707_data_session_max_profiles[SYS_MODEM_AS_ID_1];
       profile_reg_index++)
  {
    if (ds707_data_session_profile_info[profile_reg_index].data_session_profile_id
        == profile_id)
    {
      ret_val = profile_reg_index;
      break;
    }
  } /* for */
  return ret_val;
}
/*===========================================================================
FUNCTION      DS707_DATA_SESSION_IS_VALID_PROFILE

DESCRIPTION   This function checks if the profile given as input is a valid
              profile or not. If the profile is valid then it returns the
              profile reg index. This maps the profile ID to the correct
              reg index. If a match is found then it checks whether that
              profile has mask set to valid or invlid.

PARAMETERS    ds707_data_session_profile_id

DEPENDENCIES  None

RETURN VALUE  profile reg index if valid profile
              -1 if invalid profile.

SIDE EFFECTS  None
===========================================================================*/
ds707_data_session_profile_id ds707_data_session_is_valid_profile
(
  ds707_data_session_profile_id profile_id
)
{
  ds707_data_session_profile_id profile_reg_index;
  ds707_data_session_profile_id ret_val = -1;
/*-------------------------------------------------------------------------*/

  /* Get the profile reg index that corresponds to this profile id */
  profile_reg_index = ds707_data_session_get_reg_index(profile_id);

  DS_ENTER_CRIT_SECTION(&ds707_profile_crit_sect);
  
  if ((profile_reg_index >= DATA_SESSION_PROFILE_ID_MIN) &&
     (profile_reg_index < ds707_data_session_max_profiles[SYS_MODEM_AS_ID_1]))
  {
    if (ds707_data_session_profile_info[profile_reg_index].profile_type
          == PROFILE_INVALID)
    {
      DATA_IS707_MSG1(MSG_LEGACY_HIGH, "Profile %d is present but not a valid profile",
                      profile_id);
    }
    else
    {
      ret_val = profile_reg_index;
    }
  }
 
  DS_LEAVE_CRIT_SECTION(&ds707_profile_crit_sect);

  return ret_val;
}

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_CALL_ALLOWED

DESCRIPTION   Utility function to make a decision on whether calls are
              allowed or not, based on profiles.

DEPENDENCIES  None.

RETURN VALUE  TRUE: eHRPD Calls are allowed
              FALSE: eHRPD Calls are not allowed

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_data_session_call_allowed
(
  ds707_data_session_profile_info_type *data_sess_profile_info_ptr,
  ps_iface_net_down_reason_type *call_end_reason
)
{

  ps_phys_link_type          *phys_link_ptr = NULL;
  boolean ret_val = FALSE;
  boolean is_dormant = FALSE;
  sys_sys_mode_e_type curr_mode;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
 /*--------------------------------
   Sanity Check
  -------------------------------*/
  if (data_sess_profile_info_ptr == NULL)
  {
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    If any mandatory APN is disabled, and MinApnList Disallow call
    in NV is set to TRUE, NOT allow any call
  -------------------------------------------------------------------------*/
  if(ds707_data_session_nv_minapnlist_disallow_call() &&
     !ds707_data_session_get_ehrpd_attach_allowed())
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                     "ds707_data_session_call_allowed: mandatory apn disabled"
                     "and NV minapnlist_disallow_call is set to TRUE,"
                     "NOT allow any call"
                     );
    *call_end_reason = PS_NET_DOWN_REASON_MANDATORY_APN_DISABLED;
    return FALSE;
  }

  curr_mode = ds707_get_sys_pref_mode(data_sess_profile_info_ptr);

  /* OPTHO_ToDo: For Optho since sys is going to be LTE, this check is
     going to succeed. Hence no need to add any new checks - Revisit if
     needed */
  if ((curr_mode !=  SYS_SYS_MODE_CDMA) && (curr_mode != SYS_SYS_MODE_HDR)
#ifdef FEATURE_DATA_OPTHO
      && ( ds707_s101_query_sm_state() == DS707_S101_SM_NON_TUNNEL )
#endif /* FEATURE_DATA_OPTHO */
      )
  {
    DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                     "ds707_data_session_call_allowed: current mode is:%d, Allow call",
                     curr_mode);
    return TRUE;
  }
  phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();

  /*-----------------------------------------------------------------------
    If the Call is dormant, then there is possibility that when Phys link
    comes up the AMSS is connected on eHRPD session. Hence allow the call
    to go through to bring up the phys link
  -----------------------------------------------------------------------*/
  if (ds707_pkt_is_dormant(phys_link_ptr) == TRUE ||
      PS_PHYS_LINK_GET_STATE(phys_link_ptr) == PHYS_LINK_NULL)
  {
    ret_val = TRUE;
    is_dormant = TRUE;
  }
  /*-----------------------------------------------------------------------
    If the Call is not dormant, then we allow the call based on the
    current system configuration.
  -----------------------------------------------------------------------*/
  else
  {
    /*---------------------------------------------------------------------
      If profile is default or ehrpd, then check rat type
    ---------------------------------------------------------------------*/
    if ( data_sess_profile_info_ptr->profile_type & PROFILE_EHRPD )
    {
      if (ds707_pkt_is_ehrpd_mode_of_operation() == TRUE)
      {
        if ( ds707_data_session_get_ehrpd_attach_allowed() == FALSE )
        {
          DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                           "ds707_data_session_call_allowed: eHRPD mode, mandatory APN(s) not enabled" );
          *call_end_reason = PS_NET_DOWN_REASON_MANDATORY_APN_DISABLED;
          return FALSE;
        } /* ehrpd attach allowed flag - FALSE */
        else
        {
          if ( data_sess_profile_info_ptr->apn_flag == TRUE )
          {
            if ( ds707_data_session_is_apn_enabled_in_uicc_card(
                           data_sess_profile_info_ptr->apn_string,
                           data_sess_profile_info_ptr->apn_string_len) == FALSE )
            {
              DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                               "ds707_data_session_call_allowed: APN disabled in card, return FALSE");
              *call_end_reason = PS_NET_DOWN_REASON_APN_DISABLED;
              return FALSE;
            }
          }/* apn_enabled = TRUE */
          else
          {
            DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                             "ds707_data_session_call_allowed: APN disabled in profile, return FALSE");
            *call_end_reason = PS_NET_DOWN_REASON_APN_DISABLED;
            return FALSE;
          }
        }
      }  /* ehrpd mode */

      switch ( data_sess_profile_info_ptr->rat_type )
      {
        case DS707_DATA_SESS_RAT_TYPE_EHRPD:
        {
          /*---------------------------------------------------------------
            If application wants to connect to EPC core, then the AMSS
            needs to be on HDR and if on HDR in EHRPD mode of operation
          ---------------------------------------------------------------*/
          if ( ds707_pkt_is_ehrpd_mode_of_operation() == TRUE )
          {
            ret_val = TRUE;
          }
          else
          {
            DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                             "ds707_data_session_call_allowed: Profile RAT "
                             "Type: EHRPD Only, but UE not in eHRPD RAT, "
                             "return FALSE");
            *call_end_reason = PS_NET_DOWN_REASON_UNPREFERRED_RAT;
            ret_val = FALSE;
          }
        }
        break;

        case DS707_DATA_SESS_RAT_TYPE_HRPD:
        {
          /*---------------------------------------------------------------
            If application wants to connect to non-EPC core, then the AMSS
            can be on (HDR but not EHRPD) or (1x)
          ---------------------------------------------------------------*/
          if ( ( (ds707_pkt_mgr_get_current_sys_mode() == SYS_SYS_MODE_HDR) &&
                 ( ds707_pkt_is_ehrpd_mode_of_operation() == FALSE ) ) ||
               ( ds707_pkt_mgr_get_current_sys_mode() == SYS_SYS_MODE_CDMA ) )
          {
            ret_val = TRUE;
          }
          else
          {
            DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                             "ds707_data_session_call_allowed: Profile RAT "
                             "Type: HRPD Only, but UE not in 1X/HRPD RAT, "
                             "return FALSE");
            *call_end_reason = PS_NET_DOWN_REASON_UNPREFERRED_RAT;
            ret_val = FALSE;
          }
        }
        break;

        case DS707_DATA_SESS_RAT_TYPE_HRPD_EHRPD:
        {
          /*---------------------------------------------------------------
            If application does not give any preference then it allow
            the application
          ---------------------------------------------------------------*/
          ret_val = TRUE;
        }
        break;

        default:
        *call_end_reason = PS_NET_DOWN_REASON_UNPREFERRED_RAT;
        DATA_IS707_MSG1( MSG_LEGACY_ERROR,
                         "Invalid RAT Type: %d",
                         data_sess_profile_info_ptr->rat_type );
      }
    }
    else
    {
      /*---------------------------------------------------------------
        If profile id is other than default and ehrpd, then allow call
        if it is not on ehrpd system.
      ---------------------------------------------------------------*/
      if ( ds707_pkt_is_ehrpd_mode_of_operation() == TRUE )
      {
        *call_end_reason = PS_NET_DOWN_REASON_UNPREFERRED_RAT;
        ret_val = FALSE;
      }
      else
      {
        ret_val = TRUE;
      }
    }
  }

  DATA_IS707_MSG4( MSG_LEGACY_MED,
                   "ds707_data_session_call_allowed:%d, is_dormant:%d, sys_mode:%d, ehrpd:%d",
                   ret_val,
                   is_dormant,
                   ds707_pkt_mgr_get_current_sys_mode(),
                   ds707_pkt_is_ehrpd_mode_of_operation());


  return ret_val;
} /* ds707_data_session_call_allowed() */

/*===========================================================================
FUNCTION ds707_read_data_session_profile_from_nvruim

DESCRIPTION
  This function gets profiles from NV item for the Data task and stores them in
  the DS local database and also in PS database.

DEPENDENCIES
  The NV task must be started and running.

RETURN VALUE
  None

SIDE EFFECTS
  Each call to 'dsi_nv_get_item' results in a wait and watchdog kicking
===========================================================================*/
void ds707_read_data_session_profile_from_nvruim(void)
{
   nv_stat_enum_type              nv_status;      /* status from NV call  */
   nv_item_type                   loc_nv_item;
   uint8                          index = 0;          /* 8 bit index */
   ds707_data_session_profile_id  profile_reg_index =
         DATA_SESSION_PROFILE_ID_INVALID;
   ds707_data_session_profile_id  profile_id =
         DATA_SESSION_PROFILE_ID_INVALID;
   uint8                          num_valid_apptype_prof = 0;
   ds707_data_session_profile_info_type*
                                  profile_info_realloc = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   num_valid_profiles = 0;

   if( (NV_RTRE_CONTROL_NO_RUIM != nv_rtre_control()) &&
       (!nvruim_data_3gpd_ext_support()) &&
       !(NVRUIM_NON_3GPD_CARD == nvruim_check_non_3gpd_cdma_card()) )
   {
     DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                      "3GPD Ext not supported in RUIM. Don't read profiles." );
     return;
   }
   nv_status = ds3gcfgmgr_read_legacy_nv_ex(NV_DS_SIP_NUM_VALID_PROFILES_I ,&loc_nv_item, ds707_curr_subs_id());
   if( nv_status == NV_DONE_S )
   {
     num_valid_profiles = loc_nv_item.ds_sip_num_valid_profiles;
     if( num_valid_profiles > DATA_SESSION_NVRUIM_MAX_PROFILES )
     {
       DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                        "Num valid profiles exceeds max supported profiles,"
                        "Limiting num valid profiles to max supported profiles");
       num_valid_profiles = DATA_SESSION_NVRUIM_MAX_PROFILES;
     }

     DATA_IS707_MSG1(MSG_LEGACY_HIGH, "No. of valid profiles %d",
                     num_valid_profiles);

     DS_ENTER_CRIT_SECTION(&ds707_profile_crit_sect);

     /*--------------------------------------------------------------------
       Check if memory is allocated for NVRUIM profiles during boot up.
       If not, reallocate memory.
     --------------------------------------------------------------------*/
     if ( FALSE == ds707_data_sess_is_nvruim_profile_allocated() ) 
     {
       DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                        "Re-allocate profile memory for NVRUIM profiles" );

       profile_info_realloc = 
         (ds707_data_session_profile_info_type *)modem_mem_realloc(
           (void *)ds707_data_session_profile_info,
           (DATA_SESSION_NVRUIM_MAX_PROFILES + 
              ds707_data_session_max_profiles[SYS_MODEM_AS_ID_1])
             * sizeof(ds707_data_session_profile_info_type),
           MODEM_MEM_CLIENT_DATA_CRIT);
       if( NULL == profile_info_realloc )
       {
         DS_LEAVE_CRIT_SECTION(&ds707_profile_crit_sect);
         DATA_ERR_FATAL("profile memory re-allocation failed");
         return;
       }
       /*--------------------------------------------------------------------
         If the ptr block IS successfully resized and the returned value is
         different from the ptr value passed in, the old block passed in must
         be considered deallocated and no longer useable. Hence we are
         sending it back to the global variable.
       --------------------------------------------------------------------*/
       ds707_data_session_profile_info = profile_info_realloc;
       memset( (void *)(ds707_data_session_profile_info +
         ds707_data_session_max_profiles[SYS_MODEM_AS_ID_1]),
         0,
         DATA_SESSION_NVRUIM_MAX_PROFILES *
           sizeof(ds707_data_session_profile_info_type) );

       DATA_IS707_MSG0( MSG_LEGACY_HIGH, "Setting RUIM profiles to INVALID" );
       for ( index = (uint8)ds707_data_session_max_profiles[SYS_MODEM_AS_ID_1];
             index < (uint8)(ds707_data_session_max_profiles[SYS_MODEM_AS_ID_1] + 
                       DATA_SESSION_NVRUIM_MAX_PROFILES);
             index++ ) 
       {
         ds707_data_session_profile_info[index].data_session_profile_id =
           DATA_SESSION_PROFILE_ID_INVALID;
         ds707_data_session_profile_info[index].profile_type =
           PROFILE_INVALID;
       }
       ds707_data_session_max_profiles[SYS_MODEM_AS_ID_1] += 
                                             DATA_SESSION_NVRUIM_MAX_PROFILES;
     }

     for(index=0; index<num_valid_profiles; index++)
     {
       loc_nv_item.ds_sip_profile.index = index;

       nv_status = ds3gcfgmgr_read_legacy_nv_ex(NV_DS_SIP_PROFILE_I, &loc_nv_item, ds707_curr_subs_id());
       if( nv_status == NV_DONE_S)
       {
         DATA_IS707_MSG4( MSG_LEGACY_HIGH,
                          "SUCCESSFUL READING OF SIP PROFILE %d "
                          "APP TYPE=%d APP_PRIORITY=%d nai_entry_index=%d",
                          index,
                          loc_nv_item.ds_sip_profile.app_type,
                          loc_nv_item.ds_sip_profile.app_priority,
                          loc_nv_item.ds_sip_profile.index );

         profile_id = DATA_SESSION_NVRUIM_PROFILE_MIN + index;

         /*--------------------------------------------------------------- 
           Make sure ds707_data_sess_profile_id_to_reg_index_mapping is
           used only for boot up cases.
           --------------------------------------------------------------*/
         if ( FALSE == ds707_data_sess_is_nvruim_profile_allocated() )
         {
         profile_reg_index =
           ds707_data_sess_profile_id_to_reg_index_mapping( profile_id );
         }
         else
         {
           profile_reg_index = 
              ds707_data_session_get_reg_index( profile_id );
         }

         if( (profile_reg_index == DATA_SESSION_PROFILE_ID_INVALID) ||
             (loc_nv_item.ds_sip_profile.app_type == 0)
           )
         {
           DATA_IS707_MSG2( MSG_LEGACY_ERROR,
                            "Mapping of Profile ID to index %d failed or "
                            "App type read for profile %d is zero.",
                            profile_reg_index,
                            loc_nv_item.ds_sip_profile.app_type );
           continue;
         }

         num_valid_apptype_prof++;

         ds707_data_session_profile_info[profile_reg_index].data_session_profile_id
           = profile_id;
         ds707_data_session_profile_info[profile_reg_index].app_type
           = loc_nv_item.ds_sip_profile.app_type;
         ds707_data_session_profile_info[profile_reg_index].app_priority
           = loc_nv_item.ds_sip_profile.app_priority;
         /* TODO: Discuss on why we should add PROFILE_COMMON for profile type */
         ds707_data_session_profile_info[profile_reg_index].profile_type =
           PROFILE_OMH;

         /* Profile DB internal params*/
         ds707_data_session_profile_info[profile_reg_index].supported_tech_mask
           = TECH_MASK_3GPP2;

         ds707_data_session_profile_info[profile_reg_index].ip_version[PRIMARY_DNS_ADDR] 
           = V4V6;

         ds707_data_session_profile_info[profile_reg_index].ip_version[SECONDARY_DNS_ADDR] 
           = V4V6;

         ds707_data_session_profile_info[profile_reg_index].is_linger_params_valid 
           = FALSE;

         /*
            Below parameters are not used for OMH multiple profiles but are
            initialized to default values for sanity.
         */
         /*--------------------------------------------------------------------
              set DNS server negotiation option
         --------------------------------------------------------------------*/
         ds707_data_session_profile_info[profile_reg_index].negotiate_dns_server
           = TRUE;
         /*--------------------------------------------------------------------
              set session close timer (i.e. go null timer) for DO and 1X
         --------------------------------------------------------------------*/
         ds707_data_session_profile_info[profile_reg_index].ppp_session_close_timer_DO
           = ds707_gonulltmr_get_timer(DS707_GONULLTMRI_DO);
         ds707_data_session_profile_info[profile_reg_index].ppp_session_close_timer_1X
           = ds707_gonulltmr_get_timer(DS707_GONULLTMRI_1X);
         /*--------------------------------------------------------------------
              set Linger option
           (i.e. whether to allow Dormant 2 / Linger state or not)
         --------------------------------------------------------------------*/
         ds707_data_session_profile_info[profile_reg_index].allow_linger = FALSE;
         ds707_data_session_profile_info[profile_reg_index].linger_timeout_val
           = 0;
         /*--------------------------------------------------------------------
              set PPP lcp and ipcp options
         --------------------------------------------------------------------*/
         /* THIS SHOULD BE CHANGED TO READ THE DEFAULT VALUES FROM NV
            ds707_data_sess_profile_read_default_ppp_opts(&default_ppp_params)
            READS THIS INFO TODAY*/
         ds707_get_ppp_default_dev_constant(
               &ds707_data_session_profile_info[profile_reg_index]);

         ds707_data_session_profile_info[profile_reg_index].auth_protocol = 0;
         /*--------------------------------------------------------------------
           set ppp user_id and password lengths as 0
         --------------------------------------------------------------------*/
         ds707_data_session_profile_info[profile_reg_index].user_id_len = 0;
         ds707_data_session_profile_info[profile_reg_index].auth_password_len = 0;
         /*--------------------------------------------------------------------
           set phys link data rate and data mode options
         --------------------------------------------------------------------*/
         ds707_data_session_profile_info[profile_reg_index].data_rate
           = DS707_DATARATE_HIGH;
         ds707_data_session_profile_info[profile_reg_index].data_mode
           = DS707_DATAMODE_CDMA_HDR;
         ds707_data_session_profile_info[profile_reg_index].pdn_type
           = DS707_DATA_SESS_PDN_TYPE_V4_V6;
         ds707_data_session_profile_info[profile_reg_index].subs_id
            = ACTIVE_SUBSCRIPTION_1;

       }
       else
       {
         DATA_IS707_MSG1( MSG_LEGACY_ERROR,
              "NV profile info read failed. Status=%d.", nv_status );
       }
     } /*end of for loop*/

     ds707_data_sess_set_nvruim_profile_allocated(TRUE);

     DS_LEAVE_CRIT_SECTION(&ds707_profile_crit_sect);

     /*------------------------------------------------------------------------
       'num_valid_apptype_prof' indicates actual number of profiles in the card
       that has a valid app type. This value should match 'num_valid_profiles'
       in ideal scenario. In case of bad provisioning wherein no profiles were
       provisioned (left to default values) but 'num_valid_profiles' is > 0,
       then 'num_valid_profiles' should be set to zero in order to fallback to
       legacy behavior which uses default profile.
     ------------------------------------------------------------------------*/
     if(num_valid_apptype_prof == 0)
     {
       num_valid_profiles = 0;
     }

     DATA_IS707_MSG2(MSG_LEGACY_HIGH,
                     "No. of profiles with valid app_type is %d; "
                     "No. of valid profiles %d.",
                     num_valid_apptype_prof, num_valid_profiles);
   }
   else
   {
      DATA_IS707_MSG1( MSG_LEGACY_ERROR,
       "Number of valid profiles NV read failed. Read status = %d.",
        nv_status );
   }
} /* ds707_read_data_session_profile_from_nvruim */

/*===========================================================================
FUNCTION DS707_DATA_SESSION_GET_APP_PROFILE_INDEX()

DESCRIPTION
  This function will return the application profile index based on the application type
  passed as the parameter to it.

DEPENDENCIES
  None.

PARAMETERS
  app type : Type of the application for which you want profile index

RETURN VALUE
  Valid profile id : On success.
  -1               : On failure.

SIDE EFFECTS
  None.
===========================================================================*/
ds707_data_session_profile_id ds707_data_session_get_app_profile_index
(
  uint32 app_type
)
{
  ds707_data_session_profile_id app_profile_id = DATA_SESSION_PROFILE_ID_INVALID;
  boolean default_profile_found = FALSE;
  uint8   index = 0 ;         /* 8 bit index */
  int32   profile_reg_index = DATA_SESSION_PROFILE_ID_INVALID;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ENTER_CRIT_SECTION(&ds707_profile_crit_sect);

  for( index = DATA_SESSION_NVRUIM_PROFILE_MIN;
       index < DATA_SESSION_NVRUIM_PROFILE_MIN + num_valid_profiles; index++ )
  {
    profile_reg_index = ds707_data_session_get_reg_index( index );
    if( profile_reg_index == DATA_SESSION_PROFILE_ID_INVALID )
    {
      DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                       "Profile ID not present in Profile Registry");
      continue;
    }

    if( app_type & ds707_data_session_profile_info[profile_reg_index].app_type )
    {
      app_profile_id = index;
      break;
    }
  }

  if( app_profile_id == DATA_SESSION_PROFILE_ID_INVALID )
  {
    for( index = DATA_SESSION_NVRUIM_PROFILE_MIN;
         index < DATA_SESSION_NVRUIM_PROFILE_MIN + num_valid_profiles; index++ )
    {
      profile_reg_index = ds707_data_session_get_reg_index( index );
      if( profile_reg_index == DATA_SESSION_PROFILE_ID_INVALID )
      {
        DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                         "Profile ID not present in Profile Registry");
        continue;
      }

      if( ds707_data_session_profile_info[profile_reg_index].app_type &
          DEFAULT_PROFILE )
      {
        default_profile_found = TRUE;
        app_profile_id = index;
        break;
      }
    }

    if( default_profile_found == TRUE )    /* Default profile is available */
    {
      DATA_IS707_MSG1( MSG_LEGACY_HIGH,
       "App will use default profile. Profile id is %d", app_profile_id );
    }
    else
    {
      DATA_IS707_MSG0( MSG_LEGACY_ERROR, "App profile id lookup failed." );
    }
  }
  else
  {
    DATA_IS707_MSG2( MSG_LEGACY_HIGH,
     "APP profile lookup succeeded. Profile ID is %d App type is %d",
      app_profile_id, app_type );
  }

  DS_LEAVE_CRIT_SECTION(&ds707_profile_crit_sect);

  return app_profile_id;
} /* ds707_data_session_get_app_profile_index */

/*===========================================================================
FUNCTION DS707_DATA_SESSION_GET_LBS_PROFILE_ID

DESCRIPTION
  This function get the profile of minimum priority and compatible with LBS.

DEPENDENCIES
  None

RETURN VALUE
  Valid profile id              on success.
  DATA_SESSION_PROFILE_ID_INVALID:  on failure.

SIDE EFFECTS
  None
===========================================================================*/
ds707_data_session_profile_id ds707_data_session_get_lbs_profile_id(void)
{
  ds707_data_session_profile_id app_profile_id = DATA_SESSION_PROFILE_ID_INVALID;
  int     app_priority_val = -1;
  boolean default_profile_found = FALSE;
  uint8   index = 0;
  int32 profile_reg_index = -1;
/*---------------------------------------------------------------------------*/

  DS_ENTER_CRIT_SECTION(&ds707_profile_crit_sect);
  
  for( index = DATA_SESSION_NVRUIM_PROFILE_MIN;
       index < DATA_SESSION_NVRUIM_PROFILE_MIN + num_valid_profiles; index++ )
  {
    profile_reg_index = ds707_data_session_get_reg_index( index );
    if( profile_reg_index == DATA_SESSION_PROFILE_ID_INVALID )
    {
      DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                       "Profile ID not present in Profile Registry");
      continue;
    }

    if( ds707_data_session_profile_info[profile_reg_index].app_type &
        LBS_APP_TYPE )
    {
      if( ds707_data_session_profile_info[profile_reg_index].app_priority >
          app_priority_val )
      {
        app_priority_val =
              ds707_data_session_profile_info[profile_reg_index].app_priority;
        app_profile_id = index;
      }
    }
  }

  if( app_profile_id == DATA_SESSION_PROFILE_ID_INVALID )
  {
    for( index = DATA_SESSION_NVRUIM_PROFILE_MIN;
         index < DATA_SESSION_NVRUIM_PROFILE_MIN + num_valid_profiles; index++ )
    {
      profile_reg_index = ds707_data_session_get_reg_index( index );
      if( profile_reg_index == DATA_SESSION_PROFILE_ID_INVALID )
      {
        DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                         "Profile ID not present in Profile Registry");
        continue;
      }

      if( ds707_data_session_profile_info[profile_reg_index].app_type &
          DEFAULT_PROFILE )
      {
        default_profile_found = TRUE;
        app_profile_id = index;
        break;
      }
    }

    if( default_profile_found == TRUE )       /* Default profile is available*/
    {
      DATA_IS707_MSG1( MSG_LEGACY_HIGH,
       "LBS will use default profile. Profile id is %d",  app_profile_id );
    }
    else
    {
      DATA_IS707_MSG0( MSG_LEGACY_ERROR, "LBS APP profile lookup failed." );
    }
  }
  else
  {
    DATA_IS707_MSG1( MSG_LEGACY_HIGH,
     "LBS APP profile lookup succeeded. Profile ID is %d",app_profile_id );
  }

  DS_LEAVE_CRIT_SECTION(&ds707_profile_crit_sect);

  return app_profile_id;
} /* ds707_data_session_get_lbs_profile_id */

/*===========================================================================
FUNCTION
  DS707_DATA_SESSION_GET_DEFAULT_PROFILE_ID

DESCRIPTION
  This function returns the default profile ID.

DEPENDENCIES
  None

RETURN VALUE
  Profile IDs in the range 201-216. If no default profile is found
  then this function returns -1.

SIDE EFFECTS
  None
===========================================================================*/
ds707_data_session_profile_id ds707_data_session_get_default_profile_id
(
  void
)
{
  int32 profile_reg_index = 0;
  uint8 index = 0;
  ds707_data_session_profile_id profile_id = DATA_SESSION_PROFILE_ID_INVALID;

  DS_ENTER_CRIT_SECTION(&ds707_profile_crit_sect);
  
  for(index = DATA_SESSION_NVRUIM_PROFILE_MIN;
      index < DATA_SESSION_NVRUIM_PROFILE_MIN + num_valid_profiles;
      index++)
  {
    profile_reg_index = ds707_data_session_get_reg_index( index );
    if(profile_reg_index == DATA_SESSION_PROFILE_ID_INVALID)
    {
      DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                      "Profile index not present in Profile Registry %d",
                      profile_reg_index);
      continue;
    }

    if( ds707_data_session_profile_info[profile_reg_index].app_type &
        DEFAULT_PROFILE )
    {
      profile_id =
        ds707_data_session_profile_info[profile_reg_index].data_session_profile_id;
      DATA_IS707_MSG1(MSG_LEGACY_HIGH,"Default profile ID %d found",
                      profile_id);
      break;
    }
  }

  DS_LEAVE_CRIT_SECTION(&ds707_profile_crit_sect);

  return profile_id;
}/* ds707_data_session_get_default_profile_id */

/*===========================================================================
FUNCTION DS707_DATA_SESSION_COMPATIBILITY_CHECK

DESCRIPTION
  This function will check the compatibility of LBS with current Profile.

DEPENDENCIES
   NONE

PARAMETERS
  this_if_ptr     : Current active iface pointer
  policy_info_ptr : Pointer to current application policy info block

RETURN VALUE
  TRUE  : If LBS app is comaptible with current active profile.
  FALSE : If the LBS app is incomaptible with current active profile.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds707_data_session_compatibility_check
(
  void                 *this_if_ptr,
  acl_policy_info_type *policy_info_ptr
)
{
  ps_iface_type                 *iface_ptr = NULL;
  ds707_data_session_profile_id pending_id =
                                  DATA_SESSION_PROFILE_ID_INVALID;
  ds707_event_payload_type      ds707_event_payload;
  int32                         profile_reg_index;
  uint32                        profile_app_type;
  ds707_mip_profile_type       *ds_1x_runtime_profile_ptr = NULL;
  ds707_data_session_profile_info_type *current_profile_ptr = NULL;
  ds707_data_session_profile_info_type *requesting_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Skip the profile arbitration for MIP and MIP to SIP fallback cases */
#ifdef FEATURE_DS_MOBILE_IP
   if( ds_qcmip_get_runtime_val () >  0 )
   {
     return TRUE;
    }
#endif /* FEATURE_DS_MOBILE_IP */

   ds_1x_runtime_profile_ptr = ds_get_1x_runtime_profile_ptr();
  /*-----------------------------------------------------------------------
    Return TRUE if 'is_routeable' is TRUE and Network Model Call is UP as
    IFACE is shareable by routeable sockets.
  -----------------------------------------------------------------------*/
  if( (policy_info_ptr->is_routeable) &&
      (ds_1x_runtime_profile_ptr->ds707.ds707_crm_val ==
                                     DS_CRM_VAL_NETMODEL_MODE) &&
      (ds707_rmsm_is_rm_iface_up() == TRUE) )
  {
    return TRUE;
  }

  iface_ptr = ( ps_iface_type* ) this_if_ptr;
  ds707_data_session_get_iface_current_profile(iface_ptr, &current_profile_ptr);
  ds707_data_session_get_iface_requesting_profile( iface_ptr,
                                                   &requesting_profile_ptr );

  if( (current_profile_ptr == NULL) &&
      (requesting_profile_ptr == NULL) )
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH,"LBS is First APP." );

    pending_id = ds707_data_session_get_lbs_profile_id();
    if( pending_id == DATA_SESSION_PROFILE_ID_INVALID )
    {
      return FALSE;
    }

    /*-------------------------------------------------------------------------
      Fill in NVRUIM profile ID in policy info pointer as in post proc this
      profile ID will be stored in data session info structure.
    -------------------------------------------------------------------------*/
    policy_info_ptr->data_session_profile_id = pending_id;

    ds707_event_payload.ds707_event = EVENT_DS707_DATA_CALL_REQ_ACCEPTED;
    ds707_event_payload.info.zero = 0;
    event_report_payload( EVENT_DS707, sizeof(ds707_event_payload),
                                       &ds707_event_payload );
    return TRUE;
  }
  else if( current_profile_ptr != NULL )
  {
    profile_reg_index = ds707_data_session_get_reg_index( 
                          current_profile_ptr->data_session_profile_id );
    policy_info_ptr->data_session_profile_id = 
                          current_profile_ptr->data_session_profile_id;
  }
  else
  {
    profile_reg_index = ds707_data_session_get_reg_index( 
                          requesting_profile_ptr->data_session_profile_id );
    policy_info_ptr->data_session_profile_id = 
                          requesting_profile_ptr->data_session_profile_id ;
  }

  DATA_IS707_MSG0( MSG_LEGACY_HIGH, "LBS is non-first app." );

  /*------------------------------------------------------------------------ 
    app_type is not allowed to be modified by clients. Check if the profile
    has been deleted from the cache. If so, disallow the call, otherwise,
    check against the cache value is enough.
   -----------------------------------------------------------------------*/
  if( profile_reg_index == DATA_SESSION_PROFILE_ID_INVALID )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                     "Profile ID not present in Profile Registry");
    return FALSE;
  }

  DS_ENTER_CRIT_SECTION(&ds707_profile_crit_sect);
  
  profile_app_type = ds707_data_session_profile_info[ profile_reg_index ].app_type;

  DS_LEAVE_CRIT_SECTION(&ds707_profile_crit_sect);

  if( profile_app_type & LBS_APP_TYPE )
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, "LBS APP compatible with existing app" );
    ds707_event_payload.ds707_event = EVENT_DS707_DATA_CALL_REQ_ACCEPTED;
    ds707_event_payload.info.zero = 0;
    event_report_payload( EVENT_DS707, sizeof(ds707_event_payload),
                          &ds707_event_payload );
    return TRUE;
  }

  DATA_IS707_MSG0( MSG_LEGACY_HIGH,
       "LBS APP is incompatible with active profile, "
       "rejecting LBS app request for data connection" );

  ds707_event_payload.ds707_event = EVENT_DS707_DATA_CALL_REQ_REJECTED;
  ds707_event_payload.info.zero = 0;
  event_report_payload( EVENT_DS707, sizeof(ds707_event_payload),
                        &ds707_event_payload );

  return FALSE;
} /* ds707_data_session_compatibilty_check */

/*===========================================================================
FUNCTION DS707_DATA_SESSION_PROFILE_PRIORITY_CHECK

DESCRIPTION
  This function will compare priority of profile A and B passed as arguments.

DEPENDENCIES
  NONE

PARAMETERS
  ds707_data_session_profile_id profileA_reg_index
  ds707_data_session_profile_id profileB_reg_index

RETURN VALUE
  TRUE  : If the priority of profileA is equal to the priority of ProfileB
  FALSE : If the priority differs.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds707_data_session_profile_priority_check
(
  uint8 profileA_app_priority,
  uint8 profileB_app_priority
)
{
  ds707_event_payload_type ds707_event_payload;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( profileA_app_priority == profileB_app_priority )
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH,
         "Both apps are of same priority. Thus sharing data connection." );
    ds707_event_payload.ds707_event = EVENT_DS707_DATA_CALL_REQ_ACCEPTED;
    ds707_event_payload.info.zero = 0;
    event_report_payload( EVENT_DS707, sizeof(ds707_event_payload),
                          &ds707_event_payload );
    return TRUE;
  }

  /* Requesting app priority and current profile priority does not match */
  DATA_IS707_MSG0( MSG_LEGACY_HIGH, "Apps priority differs" );

  return FALSE;
} /* ds707_data_session_priority_check */

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_REG_CBACK_EVENTS

DESCRIPTION   Called at powerup.  Registers for cback events.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_reg_cback_events(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (ds707_cback_register(
                            DS707_CBACK_PHYS_LINK_UP_EV,
                            ds707_data_session_event_cback
                          ) == FALSE)
  {
    DATA_IS707_MSG0( MSG_LEGACY_FATAL,
                     "Could not reg PHYS_LINK_UP_EV cback" );
  }
} /* ds707_data_session_reg_cback_events */

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_EVENT_CBACK

DESCRIPTION   Callback registered with DS707 for CBACK events.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_event_cback
(
  ds707_cback_event_type            event,
  const ds707_cback_event_info_type *event_info_ptr
)
{
  ds707_data_session_profile_id requesting_profile_id;
  ps_iface_type                 *curr_iface_ptr = NULL;
  uint8                         curr_iface_index;
  uint8                         index;
  ds707_data_session_profile_info_type *pdn_prof_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch(event)
  {
    case DS707_CBACK_PHYS_LINK_UP_EV:
    {
      for ( curr_iface_index = 0;
            curr_iface_index < DS707_MAX_IFACES;
            curr_iface_index++ )
      {
        curr_iface_ptr =
          ds707_pdncntx_get_iface_ptr_from_iface_tbl_index(curr_iface_index);

        if (ds707_data_session_get_iface_requesting_profile(curr_iface_ptr,
                                                            &pdn_prof_ptr))
        {
          requesting_profile_id = pdn_prof_ptr->data_session_profile_id;
          if( ds707_data_session_is_nvruim_profile(requesting_profile_id) )
          {
            index =
               (uint8)(requesting_profile_id - DATA_SESSION_NVRUIM_PROFILE_MIN); 
            ds707_data_session_set_sip_active_profile_index( index );
            DATA_IS707_MSG1( MSG_LEGACY_MED,
                             "SIP active profile ID %d ", index );
            break;
          }
        }
      } /* For each iface */
      break;
    }

    default:
      break;
  }
} /* ds707_data_session_event_cback */


/*===========================================================================
FUNCTION      DS707_DATA_SESSION_PROFILE_ID_IS_VALID

DESCRIPTION   This function checks if the passed id is a valid id

PARAMETERS    ds707_data_session_profile_id_enum_type - profile id that needs
                                                        to be validated

DEPENDENCIES  None

RETURN VALUE  TRUE - if the id is valid
              FALSE - if the id is invalid

SIDE EFFECTS  None
===========================================================================*/

boolean ds707_data_session_profile_id_is_valid
(
  ds707_data_session_profile_id profile_id
)
{
  if (ds707_data_session_is_valid_profile(profile_id) ==
      DATA_SESSION_PROFILE_ID_INVALID)
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}

/*===========================================================================
FUNCTION     DS707_DATA_SESSION_GET_PPP_AUTH_INFO_FOR_NVRUIM_PROFILE

DESCRIPTION
  This function retrieves the PPP user ID and password from NV/RUIM
  and stores them in the provided PPP configuration structure.

DEPENDENCIES
  The NV task must be running.

RETURN VALUE
  None

SIDE EFFECTS
  Each call to 'ds3gcfgmgr_read_legacy_nv_ex' results in a Wait and watchdog kicking.
===========================================================================*/
void ds707_data_session_get_ppp_auth_info_for_nvruim_profile
(
  ppp_auth_info_type *ppp_auth_info_ptr
)
{
   nv_stat_enum_type    nv_status;          /* Status from NV call          */
   nv_item_type         nv_item;
   uint8                index = 0;

#ifdef FEATURE_DS_MOBILE_IP
   if( ds_qcmip_get_runtime_val() ==  2 )
   {
     is707_get_ppp_auth_info_from_nv( ppp_auth_info_ptr, ds3gcfgmgr_read_legacy_nv_ex );
     return;
   }
#endif

  /*-------------------------------------------------------------------------
    GET the active profile 
  -------------------------------------------------------------------------*/
  index = ds707_data_session_get_sip_active_profile_index();
  DATA_IS707_MSG1( MSG_LEGACY_HIGH,"SIP Active Profile index :  %d", index );

  /*-----------------------------------------------------------------------
    Loading active profile index into provided PPP config. This facilitates
    CHAP response calculation for multiple profiles.
  -----------------------------------------------------------------------*/
  ppp_auth_info_ptr->nai_entry_index = index;

  /*-------------------------------------------------------------------------
    Read the PPP NAI ID from NV FOR ACTIVE PROFILE.
  -------------------------------------------------------------------------*/
  nv_item.ds_sip_nai_info.index = index;
  nv_status = ds3gcfgmgr_read_legacy_nv_ex( NV_DS_SIP_NAI_INFO_I, &nv_item, ds707_curr_subs_id() );

  /*-------------------------------------------------------------------------
    If NV read succeeds, load the user ID into the provided ppp config
  -------------------------------------------------------------------------*/
  if( nv_status == NV_DONE_S )
  {
    ppp_auth_info_ptr->user_id_len = nv_item.ds_sip_nai_info.nai_length;
    memscpy( ppp_auth_info_ptr->user_id_info,
	     ppp_auth_info_ptr->user_id_len,
             (char *)nv_item.ds_sip_nai_info.nai,
             ppp_auth_info_ptr->user_id_len );
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, "User id read successfully" );
  }
  /*-------------------------------------------------------------------------
    If NV was never written then the length of the User_id is set to 0.
    This will ensure that no user ID is included in the AP response.
  -------------------------------------------------------------------------*/
  else
  {
    /*-----------------------------------------------------------------------
      Set the user_id length to 0.
    -----------------------------------------------------------------------*/
    ppp_auth_info_ptr->user_id_len = 0;
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "PPP user ID never written");
  }

  /*-------------------------------------------------------------------------
    Read the PPP password from NV.
  -------------------------------------------------------------------------*/
  nv_item.ds_sip_ppp_ss_info.index = index;
  nv_status = ds3gcfgmgr_read_legacy_nv_ex( NV_DS_SIP_PPP_SS_INFO_I,&nv_item, ds707_curr_subs_id() );

  /*-------------------------------------------------------------------------
    If NV read succeeded, load the ss info  into the provided ppp config
  -------------------------------------------------------------------------*/
  if( nv_status == NV_DONE_S )
  {
    ppp_auth_info_ptr->passwd_len = nv_item.ds_sip_ppp_ss_info.ss_length;
    memscpy( ppp_auth_info_ptr->passwd_info,
	     ppp_auth_info_ptr->passwd_len,
            (char *)nv_item.ds_sip_ppp_ss_info.ss,
            ppp_auth_info_ptr->passwd_len );
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, "Password read successfully" );
  }
  else
  {
 #ifdef FEATURE_DATA_PPP_DEFAULT_PASSWD
    DATA_IS707_MSG0( MSG_LEGACY_MED,
                     "SN PPP Password not provisioned, using default" );
    /*-----------------------------------------------------------------------
      Set PPP config with default value
    -----------------------------------------------------------------------*/
    if( PPP_DEFAULT_SN_PASSWORD_LEN <= PPP_MAX_PASSWD_LEN )
    {
      ppp_auth_info_ptr->passwd_len = PPP_DEFAULT_SN_PASSWORD_LEN;
      memscpy( ppp_auth_info_ptr->passwd_info,
               ppp_auth_info_ptr->passwd_len,
               PPP_DEFAULT_SN_PASSWORD,
               PPP_DEFAULT_SN_PASSWORD_LEN );
    }
    else
    {
      ppp_auth_info_ptr->passwd_len = 0;
      DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                       "Default PPP password too long to fit in PPP config. "
                       "SN PPP password not configured");
    }
#else
    /*-----------------------------------------------------------------------
      Set the user_id length as 0.
    -----------------------------------------------------------------------*/
    ppp_auth_info_ptr->passwd_len = 0;
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "PPP password never written" );
#endif /* FEATURE_DATA_PPP_DEFAULT_PASSWD */
  }
} /* ds707_data_session_get_ppp_auth_info_for_nvruim_profile */

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_IS_NVRUIM_PROFILES_AVAILABLE

DESCRIPTION
  This function checks and returns TRUE if profiles (specified by RUIM 3GPD)
  exists in NV/RUIM.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - If valid profiles present in NV/RUIM
  FALSE - If no valid profiles present in NV/RUIM

SIDE EFFECTS
  None
===========================================================================*/
boolean ds707_data_session_is_nvruim_profiles_available( void )
{
  return ((num_valid_profiles) ? TRUE : FALSE);
}

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_IS_NVRUIM_PROFILE

DESCRIPTION
  This function checks whether passed Profile ID is of NV/RUIM profile type
  (specified by RUIM 3GPD) or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - If Profile ID is of NV/RUIM type.
  FALSE - If Profile ID is not of NV/RUIM type.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds707_data_session_is_nvruim_profile
(
  ds707_data_session_profile_id profile_id
)
{
  int32 profile_reg_index = -1;
  boolean ret_val = FALSE;

  DS_ENTER_CRIT_SECTION(&ds707_profile_crit_sect);

  profile_reg_index = ds707_data_session_is_valid_profile(profile_id);
  if( profile_reg_index == DATA_SESSION_PROFILE_ID_INVALID )
  {
    DS_LEAVE_CRIT_SECTION(&ds707_profile_crit_sect);

    DATA_IS707_MSG1( MSG_LEGACY_ERROR,"Invalid profile id %d",
                     profile_id );
    return FALSE;
  }

  if( ds707_data_session_profile_info[profile_reg_index].profile_type &
      PROFILE_OMH )
  {
    ret_val = TRUE;
  }

  DS_LEAVE_CRIT_SECTION(&ds707_profile_crit_sect);

  return ret_val;
}

/*===========================================================================
FUNCTION
  DS707_DATA_SESSION_GET_DEFAULT_PROFILE

DESCRIPTION
  This function returns the default profile based on if NVRUIM profile is 
  available or not.

DEPENDENCIES
  None

RETURN VALUE
  Default Profile ID in the range 201-216 if NVRUIM profiles are available. 
  Else this function returns the hardcoded default profile.

SIDE EFFECTS
  None
===========================================================================*/
ds707_data_session_profile_id ds707_data_session_get_default_profile
(
  void  
)
{
  ds707_data_session_profile_id profile_id;

  if ( ds707_data_session_is_nvruim_profiles_available() &&
       ( ( (ds707_pkt_mgr_get_current_sys_mode() == SYS_SYS_MODE_HDR) &&
           (ds3gdsdif_ehprd_is_pref_system( SYS_MODEM_AS_ID_1 )
                                                             == FALSE) ) ||
         ( ds707_pkt_mgr_get_current_sys_mode() == SYS_SYS_MODE_CDMA ) ) )
  {
    profile_id = ds707_data_session_get_default_profile_id();
  }
  else
  {
    profile_id = DATA_SESSION_DEFAULT_PROFILE;
  }

  return profile_id;
}

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_GET_ACL_INFO_PTR

DESCRIPTION   Returns pointer to ACL info. (Used in APN enable/disable
              feature in data session module, and ds3gmmgsdi module).

DEPENDENCIES  None

RETURN VALUE  ds707_data_session_acl_info_type*

SIDE EFFECTS  None
===========================================================================*/
ds707_data_session_acl_info_type* ds707_data_session_get_acl_info_ptr(
  void
)
{
  return (&ds707_data_session_acl_info);
} /* ds707_data_session_get_acl_info_ptr */

/*===========================================================================
FUNCTION      DS707_DATA_SESS_IS_EMERG_ONLY_MODE

DESCRIPTION   Returns the global variable that records whether current is in 
              emergency only mode. 

DEPENDENCIES  None

RETURN VALUE  TRUE: Is in emergency only mode 
              FALSE: in normal mode 

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_sess_is_ds_centric_emerg_only_mode(void)
{
  DATA_IS707_MSG1( MSG_LEGACY_MED,
                   "#EMERG_ONLY: ds707_data_sess_is_emerg_only_mode: %d", 
                   ds707_is_ds_centric_emerg_only_mode);

  return ds707_is_ds_centric_emerg_only_mode;
}

#ifdef FEATURE_DATA_CM_CENTRIC_EMERG_MODE
/*===========================================================================
FUNCTION      DS707_DATA_SESS_SET_CM_CENTRIC_EMERG_MODE

DESCRIPTION   To set the status of CM emergency mode 

DEPENDENCIES  None

RETURN VALUE  TRUE: Is in emergency only mode 
              FALSE: in normal mode 

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_sess_set_cm_centric_emerg_mode(boolean is_emerg_mode)
{
  DATA_IS707_MSG1( 
                   MSG_LEGACY_HIGH,
                   "ds707_data_sess_set_cm_centric_emerg_mode: %d",
                   is_emerg_mode
                   );
  ds707_is_cm_centric_emerg_only_mode = is_emerg_mode;
}

/*===========================================================================
FUNCTION      DS707_DATA_SESS_GET_CM_CENTRIC_EMERG_MODE

DESCRIPTION   To get the status of CM emergency mode 

DEPENDENCIES  None

RETURN VALUE  TRUE: Is in emergency only mode 
              FALSE: in normal mode 

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_sess_get_cm_centric_emerg_mode(void)
{
  DATA_IS707_MSG1( 
                   MSG_LEGACY_HIGH,
                   "ds707_data_sess_get_cm_centric_emerg_mode: %d",
                   ds707_is_cm_centric_emerg_only_mode
                   );
  return ds707_is_cm_centric_emerg_only_mode;
    }
#endif /*FEATURE_DATA_CM_CENTRIC_EMERG_MODE*/

#ifdef FEATURE_EHRPD
/*===========================================================================
FUNCTION      DS707_DATA_SESS_ENTER_EMERG_ONLY_MODE

DESCRIPTION   This function is called whenever a mandatory APN is disabled, 
              and the NV for disallow any call under this circumstance is set
              to TRUE. DS will ask CM to put system in LPM mode.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_sess_enter_emerg_only_mode(void)
{
#ifdef FEATURE_DATA_CM_CENTRIC_EMERG_MODE
  DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                   "ds707_data_sess_enter_emerg_only_mode while mode=%d",
                   ds707_is_cm_centric_emerg_only_mode);
    if(!ds3gi_set_cm_emergency_only_mode(TRUE))
  {
      DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                       "ds707_data_sess_enter_emerg_only_mode, failed");
    }
#else
  if(!ds707_is_ds_centric_emerg_only_mode)
  {
    if(ds3g_set_cm_oprt_lpm_mode())
    {
      ds707_is_ds_centric_emerg_only_mode = TRUE;
    }
    else
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                       "#EMERG_ONLY: ds707_data_sess_enter_emerg_only_mode, failed");
    }
  }
  DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                   "#EMERG_ONLY: ds707_data_sess_enter_emerg_only_mode: %d",
                   ds707_is_ds_centric_emerg_only_mode );
#endif /*FEATURE_DATA_CM_CENTRIC_EMERG_MODE*/
}
#endif /*FEATURE_EHRPD*/

#ifdef FEATURE_DATA_CM_CENTRIC_EMERG_MODE
/*===========================================================================
FUNCTION      DS707_DATA_SESS_HANDLE_CM_AC_EMERG_EVENT

DESCRIPTION   handle the event from CM for emergency mode enter/exit

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_sess_handle_cm_ac_emerg_event
(
  ds_cmd_type  *ds_cmd_ptr
)
{
  ds3g_cm_ac_event_info_type  *ac_event_info_ptr = NULL;
  boolean                      is_emerg_mode = FALSE;

  ASSERT(ds_cmd_ptr != NULL);
  ASSERT(ds_cmd_ptr->cmd_payload_ptr != NULL);

  ac_event_info_ptr = (ds3g_cm_ac_event_info_type*)ds_cmd_ptr->cmd_payload_ptr;

  if(ac_event_info_ptr->event == CM_AC_EVENT_EMERGENCY_ENTER)
  {
    is_emerg_mode = TRUE;
  }
  else
  {
    is_emerg_mode = FALSE;
  }

  DATA_IS707_MSG2( MSG_LEGACY_HIGH,
                   "ds707_data_sess_handle_cm_ac_emerg_event: %d, apn check: %d",
                   is_emerg_mode,
                   mandatory_apn_check_status);

  if (is_emerg_mode)
  {
    ds707_data_sess_set_cm_centric_emerg_mode(TRUE);
}
  else
{
    ds707_data_sess_set_cm_centric_emerg_mode(FALSE);
    /*emergency mode exit while apn check failed, put the UE back to emergency mode*/
    if (0 == mandatory_apn_check_status && 
        ds707_data_session_nv_minapnlist_disallow_call())
    {
      ds707_data_sess_enter_emerg_only_mode();
    }
  }
}
#endif /*FEATURE_DATA_CM_CENTRIC_EMERG_MODE*/

#ifdef FEATURE_EHRPD
/*===========================================================================
FUNCTION      DS707_DATA_SESS_EXIT_EMERG_ONLY_MODE

DESCRIPTION   This fucntion is called when mandatory APN check passes. If it 
              is in emergency only mode, by calling this function, it will exit
              this mode.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_sess_exit_emerg_only_mode(void)
{
#ifdef FEATURE_DATA_CM_CENTRIC_EMERG_MODE
  DATA_IS707_MSG1( 
                   MSG_LEGACY_HIGH,
                   "ds707_data_sess_exit_emerg_only_mode while mode=%d",
                   ds707_is_cm_centric_emerg_only_mode
                 );
    if(!ds3gi_set_cm_emergency_only_mode(FALSE))
  {
      DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                       "ds707_data_sess_exit_emerg_only_mode, failed");
  }
#else /*DATA CENTRIC EMERGENCY MODE*/
  if(ds707_is_ds_centric_emerg_only_mode)
  {
    if(ds3g_set_cm_oprt_online_mode())
    {
      ds707_is_ds_centric_emerg_only_mode = FALSE;
    }
    else
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                       "#EMERG_ONLY: ds707_data_sess_exit_emerg_only_mode, failed");
    }
  }
  DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                   "#EMERG_ONLY: ds707_data_sess_exit_emerg_only_mode: %d",
                   ds707_is_ds_centric_emerg_only_mode );
#endif /*FEATURE_DATA_CM_CENTRIC_EMERG_MODE*/
}
#endif /*FEATURE_EHRPD*/

/*===========================================================================
FUNCTION      DS707_DATA_SESS_WAIT_FOR_EMERG_TIMER_CB

DESCRIPTION   Callback function for wait_for_emerg_call_timer

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_sess_wait_for_emerg_timer_cb(uint32 timer_cntx)
{
  ds_cmd_type * cmd_ptr = NULL;

  if((cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO)) == NULL)
  {
    return;
  }
  cmd_ptr->hdr.cmd_id = DS_CMD_707_WAIT_FOR_EMERG_CALL_TIMEOUT;
  ds_put_cmd( cmd_ptr ); 

}
/*===========================================================================
FUNCTION      DS707_DATA_SESS_START_WAIT_FOR_EMERG_TIMER

DESCRIPTION   To start wait_for_emerg_call_timer. The timer is started whenever 
              DS detects system changes to online mode during emergency call
              only mode.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_sess_start_wait_for_emerg_timer(void)
{
  DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                     "#EMERG_ONLY: start wait_for_emerg_timer" );
  rex_set_timer( &ds707_wait_for_emerg_call_timer, 
                       WAIT_FOR_EMERG_CALL_DURATION );

}
/*===========================================================================
FUNCTION      DS707_DATA_SESS_STOP_WAIT_FOR_EMERG_TIMER

DESCRIPTION   To stop wait_for_emerg_call_timer. 

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_sess_stop_wait_for_emerg_timer(void)
{
  if(rex_get_timer(&ds707_wait_for_emerg_call_timer) != 0)
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                   "#EMERG_ONLY: Clear wait for emerg timer" );
    rex_clr_timer( &ds707_wait_for_emerg_call_timer );
  }
}

/*===========================================================================
FUNCTION      DS707_DATA_SESS_HANDLE_CMD_IN_EMERG_ONLY

DESCRIPTION   DS command handler during emergency only mode. 

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_sess_handle_cmd_in_emerg_only
(
  ds_cmd_type *ds_cmd_ptr
)
{
  ds3g_cm_ph_ev_info_type  *cm_ph_event_ptr   = NULL;
  ds3g_call_info_cmd_type  *call_info_cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if((NULL == ds_cmd_ptr) || (!ds707_data_sess_is_ds_centric_emerg_only_mode()))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "ds_cmd_ptr is NULL or not in emergency only mode");
    return;
  }

  if((DS_CMD_707_WAIT_FOR_EMERG_CALL_TIMEOUT != ds_cmd_ptr->hdr.cmd_id) 
     && (NULL == ds_cmd_ptr->cmd_payload_ptr))
  {
    DATA_ERR_FATAL("cmd_payload_ptr is NULL");
    return;
  }
  
  switch(ds_cmd_ptr->hdr.cmd_id)
  {
    case DS_CMD_CM_PH_EV_INFO:
      cm_ph_event_ptr 
                  = (ds3g_cm_ph_ev_info_type *)(ds_cmd_ptr->cmd_payload_ptr);

      if (CM_PH_EVENT_OPRT_MODE == cm_ph_event_ptr->ph_event)
      {
        DATA_IS707_MSG1(MSG_LEGACY_HIGH, 
                        "#EMERG_ONLY: CM_PH_EVENT_OPRT_MODE, mode=%d",
                        cm_ph_event_ptr->oprt_mode);

        if(cm_ph_event_ptr->oprt_mode == SYS_OPRT_MODE_ONLINE)
        {
          ds707_data_sess_start_wait_for_emerg_timer();
        }
      }
      else if (CM_PH_EVENT_SYS_SEL_PREF == cm_ph_event_ptr->ph_event)
      {
        DATA_IS707_MSG1(MSG_LEGACY_HIGH, 
           "#EMERG_ONLY: CM_PH_EVENT_SYS_SEL_PREF, emerg to normal:%d",
           cm_ph_event_ptr->emerg_to_normal_mode);

        if(cm_ph_event_ptr->emerg_to_normal_mode)
        {
          ds707_emerg_call_in_emerg_only_mode = FALSE;
          /* There are 2 scenarios namely 1. E911 call ends and PH state moves to
             normal after ECBM ends, 2. For back to back second emergency call,
             CM moves PH state to normal and then brings up emergency call. Both 
             these cases are supported by starting emergency call wait timer. 
             For the first case, if a new PS call is started during emergency call
             wait timer, device will move to LPM when DS_CM_CALL_EVENT_ORIG is
             handled. */
          ds707_data_sess_start_wait_for_emerg_timer();
        }
      }
      break;

    case DS_CMD_CM_CALL_EV_END:
      call_info_cmd_ptr = (ds3g_call_info_cmd_type*)ds_cmd_ptr->cmd_payload_ptr;
      DATA_IS707_MSG2( MSG_LEGACY_HIGH,
                       "#EMERG_ONLY: DS_CM_CALL_END_CMD, "
                       "call_type:%d, ds707_emerg_call_in_emerg_only_mode: %d", 
                       call_info_cmd_ptr->call_type,
                       ds707_emerg_call_in_emerg_only_mode );

      if(call_info_cmd_ptr->call_type == CM_CALL_TYPE_EMERGENCY &&
         !ds707_emerg_call_in_emerg_only_mode)
      {

        /*call ends while not in emergency call, no need to 
          wait for emergency callback state ends, re-enter LPM mode*/
        ds3g_set_cm_oprt_lpm_mode();
      }
      break;

    case DS_CMD_CM_CALL_EV_ORIG:
      call_info_cmd_ptr = (ds3g_call_info_cmd_type*)ds_cmd_ptr->cmd_payload_ptr;
      DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                       "#EMERG_ONLY: DS_CM_CALL_EVENT_ORIG, call type:%d",
                       call_info_cmd_ptr->call_type );
      ds707_data_sess_stop_wait_for_emerg_timer();

      if(call_info_cmd_ptr->call_type != CM_CALL_TYPE_EMERGENCY)
      {
        /* A call that is not E911 is initiated in emergency only mode, 
           put the UE back to LPM again */
        ds3g_set_cm_oprt_lpm_mode();
      }
      break;

    case DS_CMD_CM_CALL_EV_CONNECT:
      call_info_cmd_ptr = (ds3g_call_info_cmd_type*)ds_cmd_ptr->cmd_payload_ptr;
      DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                       "#EMERG_ONLY: DS_CM_CALL_CONNECTED_CMD, call type:%d",
                       call_info_cmd_ptr->call_type );
      if(call_info_cmd_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
      {
        ds707_emerg_call_in_emerg_only_mode = TRUE;
      }
      else
      {
        /*a call that is not E911 is connected (should not come to this stage) 
          in emergency only mode, put the UE back to LPM again*/
        ds3g_set_cm_oprt_lpm_mode();
      }
      break;

    case DS_CMD_707_WAIT_FOR_EMERG_CALL_TIMEOUT:
      DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                     "#EMERG_ONLY: DS_707_WAIT_FOR_EMERG_CALL_TIMEOUT_CMD, in emerg call:%d", 
                       ds707_emerg_call_in_emerg_only_mode  );
      if(!ds707_emerg_call_in_emerg_only_mode)
      {
        /*wait for emerg call timer expires, re-ener LPM mode*/
        ds3g_set_cm_oprt_lpm_mode();
      }
      break;

    default:
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                     "#EMERG_ONLY: undesired message received" );
      break;
  }
}

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_PROCESS_EHRPD_ATTACH_ALLOWED

DESCRIPTION   This function checks if the ehrpd attach is allowed & sets the 
              ehrpd attach allowed flag. This is set to true/false depending on 
              whether all the mandatory APNs are enabled in card and in profile.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_process_ehrpd_attach_allowed(
  boolean flag
)
{
  ps_iface_tear_down_client_data_info_type tear_down_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&tear_down_info, 0, sizeof(tear_down_info));

  /*-------------------------------------------------------------------------
    If the passed flag is FALSE, eHRPD capability should be set to FALSE as 
    per mandatory APN feature. If the passed flag is TRUE, eHRPD capability 
    should be set to TRUE only if current ehrpd_allowed_flag is FALSE. If 
    both ehrpd_allowed_flag and passed flag are TRUE, eHRPD capability should
    not be changed or else it will interfer with other features like eHRPD to
    HRPD fallback.
  -------------------------------------------------------------------------*/
  if((flag == FALSE) || (ehrpd_attach_allowed == FALSE))
  {
    DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                     "Setting ehrpd attach allowed flag to %d", flag );
    ehrpd_attach_allowed = flag;
    /*-------------------------------------------------------------------------
      If any mandatory APN is disabled, and MinApnList Disallow call
      in NV is set to TRUE, drop the current active call
    -------------------------------------------------------------------------*/
    if(ds707_data_session_nv_minapnlist_disallow_call() &&
       !flag)
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                       "Drop the call because a mandatory APN is disabled "
                       "and minApnList disallow call NV is set to TRUE!" );
      tear_down_info.validity_mask |= PS_CLIENT_DATA_INFO_CALL_END_REASON_MASK;
      tear_down_info.data_info.call_end_reason = 
                                      PS_NET_DOWN_REASON_INTERNAL_APN_DISABLED;
      tear_down_info.validity_mask |= PS_CLIENT_DATA_INFO_LOCAL_ABORT_MASK;    
      tear_down_info.data_info.local_abort = FALSE;
      ds707_pkt_mgr_tear_down_all((void *)&tear_down_info);
    }
    /*-------------------------------------------------------------------------
      If any mandatory APN is disabled / enabled, notify HDR-CP whether eHRPD
      is allowed or not, accordingly HDR-CP will advertise eHRPD capability
      while negotiating a session.
    -------------------------------------------------------------------------*/
    DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                     "Notify HDR-CP eHRPD session allowed flag: %d", flag );
    hdrpac_set_data_ehrpd_capability(flag);
  }	
} /* ds707_data_session_process_ehrpd_attach_allowed() */

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_GET_EHRPD_ATTACH_ALLOWED

DESCRIPTION   This function returns the ehrpd attach allowed flag.

DEPENDENCIES  None

RETURN VALUE  boolean

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_session_get_ehrpd_attach_allowed(
  void
)
{
  DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                   "return ehrpd attach allowed flag %d", ehrpd_attach_allowed );
  return (ehrpd_attach_allowed);
}

/*===========================================================================
FUNCTION      DS707_GET_PDN_THROTTLE_FAILURE_TIMER

DESCRIPTION   This function provides the PDN throttling failure timer value

PARAMETERS    profile_id - Data session profile id
              counter - Throttling counter used as an index

RETURN VALUE  PDN throttle Failure Timer value in msec

SIDE EFFECTS  None
===========================================================================*/
uint32 ds707_get_pdn_throttle_failure_timer
(
  ds707_data_session_profile_info_type *profile_info_ptr,
  uint8                            counter
)
{

  /*-------------------------------------------------------------------------
    Decrement the counter by 1 so that it can be used as an index into timer
    array also check for its validity
  -------------------------------------------------------------------------*/
  counter -= 1;
  if( (profile_info_ptr == NULL) ||
      (PDN_THROTTLE_MAX_TIMERS <= counter) )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid PDN throttle counter value");
    return 0;
  }

  DATA_IS707_MSG2(MSG_LEGACY_MED, "Failure Timer %d: timer_val=%d",
                  counter, profile_info_ptr->failure_timer[counter]);

  return SEC_TO_MSEC(profile_info_ptr->failure_timer[counter]);
}

/*===========================================================================
FUNCTION      DS707_GET_PDN_THROTTLE_DISALLOW_TIMER

DESCRIPTION   This function provides the PDN throttling disallow timer value

PARAMETERS    profile_id - Data session profile id
              counter - Throttling counter used as an index

RETURN VALUE  PDN throttle Disallow Timer value in msec

SIDE EFFECTS  None
===========================================================================*/
uint32 ds707_get_pdn_throttle_disallow_timer
(
  ds707_data_session_profile_info_type *profile_info_ptr,
  uint8                                 counter
)
{

  /*-------------------------------------------------------------------------
    Decrement the counter by 1 so that it can be used as an index into timer
    array also check for its validity
  -------------------------------------------------------------------------*/
  counter -= 1;
  if( (profile_info_ptr == NULL) ||
      (PDN_THROTTLE_MAX_TIMERS <= counter) )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid PDN throttle counter value");
    return 0;
  }

  DATA_IS707_MSG2(MSG_LEGACY_MED, "Disallow Timer %d: timer_val=%d",
                  counter, profile_info_ptr->disallow_timer[counter]);

  return SEC_TO_MSEC(profile_info_ptr->disallow_timer[counter]);
}

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_GET_PROFILE_ID_FROM_REG_INDEX

DESCRIPTION   Given the index, this function returns the profile id.

PARAMETERS    reg_index

RETURN VALUE  ds707_data_session_profile_id

SIDE EFFECTS  None
===========================================================================*/
ds707_data_session_profile_id ds707_data_session_get_profile_id_from_reg_index(
  ds707_data_session_profile_id reg_index
)
{
  if ((reg_index >= DATA_SESSION_PROFILE_ID_MIN) &&
     (reg_index < ds707_data_session_max_profiles[SYS_MODEM_AS_ID_1]))
  {
    return ds707_data_session_profile_info[reg_index].data_session_profile_id;
  }
  else
  {
    return DATA_SESSION_PROFILE_ID_INVALID;
  }
}

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_GET_OLDEST_ACTIVE_IFACE_PTR

DESCRIPTION   This function returns the oldest active iface pointer

PARAMETERS    None

RETURN VALUE  ps_iface_type*: iface ptr

SIDE EFFECTS  None
===========================================================================*/
ps_iface_type* ds707_data_session_get_oldest_active_iface_ptr(void)
{
  return oldest_active_iface_ptr;
}

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_SET_OLDEST_ACTIVE_IFACE_PTR

DESCRIPTION   This function sets the value of oldest active iface pointer
              to the passed value.

PARAMETERS    ps_iface_type*: iface ptr

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_session_set_oldest_active_iface_ptr(
  ps_iface_type* iface_ptr
)
{
  oldest_active_iface_ptr = iface_ptr;
}

/*===========================================================================
FUNCTION      DS707_PROFILE_CONSTRUCT_STR_FROM_IDENT

DESCRIPTION   Construct the string "ParamX:ValueX;\r\n" for the given
              profile identifier

PARAMETERS    profile_ptr       : structure containing data session
                                  profile parameters
              param_ident       : Profile param identifier
              buff              : buff that stores the constructed string
              bufflength        : length of the buffer

DEPENDENCIES

RETURN VALUE  TRUE on SUCCESS
              FALSE on FAILURE

SIDE EFFECTS

===========================================================================*/
static boolean ds707_profile_construct_str_from_ident(
  ds707_data_session_profile_info_type *profile_ptr,
  uint8 param_ident,
  char * buff,
  int bufflength
)
{
  int16  dss_errno = 0;

  char user_id[PPP_MAX_USER_ID_LEN+1]               = {0};
  char auth_password[PPP_MAX_PASSWD_LEN+1]          = {0};
  char apn_string_or_label[DS_VSNCP_3GPP2_APN_MAX_VAL_LEN+1] = {0};
  char tempbuff[DS707_PROFILE_MAX_PARAM_BUFF_SIZE]  = {0};

  DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                   "Constructing string for ds707 profile param ident: %d",
                   param_ident );

  switch( param_ident )
  {
    case NEGOTIATE_DNS_SERVER:
      snprintf(buff,bufflength,"%s:%d;\r\n",
                     profile_param_name_id[param_ident],
                     profile_ptr->negotiate_dns_server);
      break;

    case PPP_SESSION_CLOSE_TIMER_DO:
      snprintf(buff,bufflength,"%s:%lu;\r\n",
                     profile_param_name_id[param_ident],
                     profile_ptr->ppp_session_close_timer_DO);
      break;

    case PPP_SESSION_CLOSE_TIMER_1X:
      snprintf(buff,bufflength,"%s:%lu;\r\n",
                     profile_param_name_id[param_ident],
                     profile_ptr->ppp_session_close_timer_1X);
      break;

    case ALLOW_LINGER:
      if (profile_ptr->allow_linger)
      {
        snprintf(buff,bufflength,"%s:A;\r\n",
                     profile_param_name_id[param_ident]);
      }
      else
      {
        snprintf(buff,bufflength,"%s:D;\r\n",
                       profile_param_name_id[param_ident]);
      }
      break;

    case LCP_ACK_TIMEOUT_VAL:
      snprintf(buff,bufflength,"%s:%d;\r\n",
                     profile_param_name_id[param_ident],
                     profile_ptr->lcp_ack_timeout);
      break;

    case IPCP_ACK_TIMEOUT_VAL:
      snprintf(buff,bufflength,"%s:%d;\r\n",
                     profile_param_name_id[param_ident],
                     profile_ptr->ipcp_ack_timeout);
      break;

    case AUTH_TIMEOUT_VAL:
      snprintf(buff,bufflength,"%s:%d;\r\n",
                     profile_param_name_id[param_ident],
                     profile_ptr->auth_timeout);
      break;

    case LCP_CREQ_RETRY_COUNT:
      snprintf(buff,bufflength,"%s:%d;\r\n",
                     profile_param_name_id[param_ident],
                     profile_ptr->lcp_creq_retry_count);
      break;

    case IPCP_CREQ_RETRY_COUNT:
      snprintf(buff,bufflength,"%s:%d;\r\n",
                     profile_param_name_id[param_ident],
                     profile_ptr->ipcp_creq_retry_count);
      break;

    case AUTH_RETRY_COUNT:
      snprintf(buff,bufflength,"%s:%d;\r\n",
                     profile_param_name_id[param_ident],
                     profile_ptr->auth_retry_count);
      break;

    case AUTH_PROTOCOL:
      if (DATA_SESS_AUTH_PROTOCOL_PAP == profile_ptr->auth_protocol)
      {
        snprintf(buff,bufflength,"%s:PAP;\r\n",
                       profile_param_name_id[param_ident]);
      }
      else if (DATA_SESS_AUTH_PROTOCOL_CHAP == profile_ptr->auth_protocol)
      {
        snprintf(buff,bufflength,"%s:CHAP;\r\n",
                       profile_param_name_id[param_ident]);
      }
      else if (DATA_SESS_AUTH_PROTOCOL_PAP_CHAP == profile_ptr->auth_protocol)
      {
        snprintf(buff,bufflength,"%s:PAP_CHAP;\r\n",
                       profile_param_name_id[param_ident]);
      }
      else if (DATA_SESS_AUTH_PROTOCOL_EAP == profile_ptr->auth_protocol)
      {
        snprintf(buff,bufflength,"%s:EAP;\r\n",
                       profile_param_name_id[param_ident]);
      }
      else if (DATA_SESS_AUTH_PROTOCOL_NONE == profile_ptr->auth_protocol)
      {
        snprintf(buff,bufflength,"%s:NONE;\r\n",
                       profile_param_name_id[param_ident]);
      }
      else
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                        "Str Construct err for AUTH PROTOCOL");
        return FALSE;
      }
      break;

    case PPP_USER_ID:
      /* The string in the profile is not null terminated.
         Null terminate before using*/
      memscpy(user_id,
              PPP_MAX_USER_ID_LEN,
              profile_ptr->user_id,
              profile_ptr->user_id_len);
      user_id[profile_ptr->user_id_len] = '\0';
      snprintf(buff,bufflength,"%s:%s;\r\n",
                     profile_param_name_id[param_ident],
                     user_id);
      break;

    case PPP_PASSWORD:
      /* The string in the profile is not null terminated.
         Null terminate before using*/
      memscpy(auth_password,
              PPP_MAX_PASSWD_LEN,
              profile_ptr->auth_password,
              profile_ptr->auth_password_len);
      auth_password[profile_ptr->auth_password_len] = '\0';
      snprintf(buff,bufflength,"%s:%s;\r\n",
                     profile_param_name_id[param_ident],
                     auth_password);
      break;

    case DATA_RATE:
      if (DS707_DATARATE_HIGH == profile_ptr->data_rate)
      {
        snprintf(buff,bufflength,"%s:H;\r\n",
                     profile_param_name_id[param_ident]);
      }
      else if (DS707_DATARATE_MED == profile_ptr->data_rate)
      {
        snprintf(buff,bufflength,"%s:M;\r\n",
                     profile_param_name_id[param_ident]);
      }
      else if (DS707_DATARATE_LOW == profile_ptr->data_rate)
      {
        snprintf(buff,bufflength,"%s:L;\r\n",
                     profile_param_name_id[param_ident]);
      }
      else
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                        "Str Construct err for DATA_RATE");
        return FALSE;
      }
      break;

    case DATA_MODE:
      if (DS707_DATAMODE_CDMA_HDR == profile_ptr->data_mode)
      {
        snprintf(buff,bufflength,"%s:H;\r\n",
                     profile_param_name_id[param_ident]);
      }
      else if (DS707_DATAMODE_CDMA_ONLY == profile_ptr->data_mode)
      {
        snprintf(buff,bufflength,"%s:X;\r\n",
                     profile_param_name_id[param_ident]);
      }
            else if (DS707_DATAMODE_HDR_ONLY == profile_ptr->data_mode)
      {
        snprintf(buff,bufflength,"%s:D;\r\n",
                     profile_param_name_id[param_ident]);
      }
      else
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                        "Str Construct err for DATA_MODE");
        return FALSE;
      }
      break;

    case APP_TYPE:
      DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                      "Str Construct err: APP_TYPE is Read Only");
      return FALSE;
      

    case APP_PRIORITY:
      DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "Str Construct err: APP_PRIORITY is Read Only");
      return FALSE;
      

    case APN_STRING:
      /* The string in the profile is not null terminated.
         Null terminate before using*/
      memscpy(apn_string_or_label,
              DS_VSNCP_3GPP2_APN_MAX_VAL_LEN,
              profile_ptr->apn_string,
              profile_ptr->apn_string_len);
      apn_string_or_label[profile_ptr->apn_string_len] = '\0';
      snprintf(buff,bufflength,"%s:%s;\r\n",
                     profile_param_name_id[param_ident],
                     apn_string_or_label);
      break;
    case PDN_LABEL:
	  /* The string in the profile is not null terminated.
	     Null terminate before using*/
	  memscpy(apn_string_or_label,
		  DS_VSNCP_3GPP2_APN_MAX_VAL_LEN,
		  profile_ptr->pdn_label,
                  profile_ptr->pdn_label_len);
	  apn_string_or_label[profile_ptr->pdn_label_len] = '\0';
	  snprintf(buff,bufflength,"%s:%s;\r\n",
		  		     profile_param_name_id[param_ident],
				     apn_string_or_label);
	  break;
    case MCC:
	    snprintf(buff,bufflength,"%s:%d;\r\n",
		  		     profile_param_name_id[param_ident],
				     profile_ptr->mcc);
	  break;
    case MNC:
      if(profile_ptr->mnc.mnc_includes_pcs_digit)
      {
        snprintf(buff,bufflength,"%s:%03d;\r\n",
                   profile_param_name_id[param_ident],
                 profile_ptr->mnc.mnc);
      }
      else
      {
        snprintf(buff,bufflength,"%s:%d;\r\n",
                   profile_param_name_id[param_ident],
                 profile_ptr->mnc.mnc);
      }

	  break;    
    case PDN_IP_VERSION_TYPE:
      if (DS707_DATA_SESS_PDN_TYPE_V4 == profile_ptr->pdn_type)
      {
        snprintf(buff,bufflength,"%s:V4;\r\n",
                     profile_param_name_id[param_ident]);
      }
      else if (DS707_DATA_SESS_PDN_TYPE_V6 == profile_ptr->pdn_type)
      {
        snprintf(buff,bufflength,"%s:V6;\r\n",
                     profile_param_name_id[param_ident]);
      }
      else if (DS707_DATA_SESS_PDN_TYPE_V4_V6 == profile_ptr->pdn_type)
      {
        snprintf(buff,bufflength,"%s:V4_V6;\r\n",
                     profile_param_name_id[param_ident]);
      }
      else
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                        "Str Construct err for PDN_IP_VERSION_TYPE");
        return FALSE;
      }
      break;

    case PCSCF_ADDR_NEEDED:
      snprintf(buff,bufflength,"%s:%d;\r\n",
                     profile_param_name_id[param_ident],
                     profile_ptr->is_pcscf_addr_needed);
      break;

     case OP_PCO_ID:
      snprintf( buff,bufflength,"%s:0x%x;\r\n",
                     profile_param_name_id[param_ident],
                     profile_ptr->op_pco_id );
      break;

    case PRIMARY_DNS_ADDR_V4:
      if (DSS_SUCCESS == dss_inet_ntop((const char *)&profile_ptr->v4_dns_addr[PRIMARY_DNS_ADDR].ps_s_addr,
                                        DSS_AF_INET,
                                        tempbuff,
                                        sizeof(tempbuff),
                                        &dss_errno
                                       )
         )
      {
        snprintf(buff,bufflength,"%s:%s;\r\n",
                       profile_param_name_id[param_ident],
                       tempbuff);
      }
      else
      {
        DATA_IS707_MSG1( MSG_LEGACY_ERROR,
                         "Unable to convert PRIMARY_DNS_ADDR_V4 to string, Err:%d",
                         dss_errno );
        return FALSE;
      }
      break;

    case SECONDARY_DNS_ADDR_V4:
      if (DSS_SUCCESS == dss_inet_ntop((const char *)&profile_ptr->v4_dns_addr[SECONDARY_DNS_ADDR].ps_s_addr,
                                        DSS_AF_INET,
                                        tempbuff,
                                        sizeof(tempbuff),
                                        &dss_errno
                                       )
         )
      {
        snprintf(buff,bufflength,"%s:%s;\r\n",
                       profile_param_name_id[param_ident],
                       tempbuff);
      }
      else
      {
        DATA_IS707_MSG1( MSG_LEGACY_ERROR,
                         "Unable to convert SECONDARY_DNS_ADDR_V4 to string, Err:%d",
                         dss_errno );
        return FALSE;
      }
      break;

    case PRIMARY_DNS_ADDR_V6:
      if (DSS_SUCCESS == dss_inet_ntop((const char *)&profile_ptr->v6_dns_addr[PRIMARY_DNS_ADDR],
                                        DSS_AF_INET6,
                                        tempbuff,
                                        sizeof(tempbuff),
                                        &dss_errno
                                       )
         )
      {
        snprintf(buff,bufflength,"%s:%s;\r\n",
                       profile_param_name_id[param_ident],
                       tempbuff);
      }
      else
      {
        DATA_IS707_MSG1( MSG_LEGACY_ERROR,
                         "Unable to convert PRIMARY_DNS_ADDR_V6 to string, Err:%d",
                         dss_errno );
        return FALSE;
      }
      break;

    case SECONDARY_DNS_ADDR_V6:
      if (DSS_SUCCESS == dss_inet_ntop((const char *)&profile_ptr->v6_dns_addr[SECONDARY_DNS_ADDR],
                                        DSS_AF_INET6,
                                        tempbuff,
                                        sizeof(tempbuff),
                                        &dss_errno
                                       )
         )
      {
        snprintf(buff,bufflength,"%s:%s;\r\n",
                       profile_param_name_id[param_ident],
                       tempbuff);
      }
      else
      {
        DATA_IS707_MSG1( MSG_LEGACY_ERROR,
                         "Unable to convert SECONDARY_DNS_ADDR_V6 to string, Err:%d",
                         dss_errno );
        return FALSE;
      }
      break;

    case RAN_TYPE:
      if (DS707_DATA_SESS_RAT_TYPE_HRPD == profile_ptr->rat_type)
      {
        snprintf(buff,bufflength,"%s:HRPD;\r\n",
                     profile_param_name_id[param_ident]);
      }
      else if (DS707_DATA_SESS_RAT_TYPE_EHRPD == profile_ptr->rat_type)
      {
        snprintf(buff,bufflength,"%s:EHRPD;\r\n",
                     profile_param_name_id[param_ident]);
      }
      else if (DS707_DATA_SESS_RAT_TYPE_HRPD_EHRPD == profile_ptr->rat_type)
      {
        snprintf(buff,bufflength,"%s:HRPD_EHRPD;\r\n",
                     profile_param_name_id[param_ident]);
      }
      else
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                        "Str Construct err for RAN_TYPE");
        return FALSE;
      }
      break;

    case PDN_INACTIVITY_TIMEOUT: /*Not present in QMI WDS Spec?*/
      snprintf(buff,bufflength,"%s:%lu;\r\n",
               profile_param_name_id[param_ident],
               MSEC_TO_MINUTE(profile_ptr->pdn_inactivity_timeout));
      break;

    case APN_ENABLED: /*Not present in QMI WDS Spec?*/
      if (profile_ptr->apn_flag)
      {
        snprintf(buff,bufflength,"%s:TRUE;\r\n",
                 profile_param_name_id[param_ident]);
      }
      else
      {
        snprintf(buff,bufflength,"%s:FALSE;\r\n",
                 profile_param_name_id[param_ident]);
      }
      break;

    case FAILURE_TIMER_1:
      snprintf(buff,bufflength,"%s:%lu;\r\n",
         profile_param_name_id[param_ident],
         profile_ptr->failure_timer[0]);
      break;

    case FAILURE_TIMER_2:
      snprintf(buff,bufflength,"%s:%lu;\r\n",
         profile_param_name_id[param_ident],
         profile_ptr->failure_timer[1]);
      break;

    case FAILURE_TIMER_3:
      snprintf(buff,bufflength,"%s:%lu;\r\n",
         profile_param_name_id[param_ident],
         profile_ptr->failure_timer[2]);
      break;

    case FAILURE_TIMER_4:
      snprintf(buff,bufflength,"%s:%lu;\r\n",
         profile_param_name_id[param_ident],
         profile_ptr->failure_timer[3]);
      break;

    case FAILURE_TIMER_5:
      snprintf(buff,bufflength,"%s:%lu;\r\n",
         profile_param_name_id[param_ident],
         profile_ptr->failure_timer[4]);
      break;

    case FAILURE_TIMER_6:
      snprintf(buff,bufflength,"%s:%lu;\r\n",
         profile_param_name_id[param_ident],
         profile_ptr->failure_timer[5]);
      break;

    case DISALLOW_TIMER_1:
      snprintf(buff,bufflength,"%s:%lu;\r\n",
         profile_param_name_id[param_ident],
         profile_ptr->disallow_timer[0]);
      break;

    case DISALLOW_TIMER_2:
      snprintf(buff,bufflength,"%s:%lu;\r\n",
         profile_param_name_id[param_ident],
         profile_ptr->disallow_timer[1]);
      break;

    case DISALLOW_TIMER_3:
      snprintf(buff,bufflength,"%s:%lu;\r\n",
         profile_param_name_id[param_ident],
         profile_ptr->disallow_timer[2]);
      break;

    case DISALLOW_TIMER_4:
      snprintf(buff,bufflength,"%s:%lu;\r\n",
         profile_param_name_id[param_ident],
         profile_ptr->disallow_timer[3]);
      break;

    case DISALLOW_TIMER_5:
      snprintf(buff,bufflength,"%s:%lu;\r\n",
         profile_param_name_id[param_ident],
         profile_ptr->disallow_timer[4]);
      break;

    case DISALLOW_TIMER_6:
      snprintf(buff,bufflength,"%s:%lu;\r\n",
         profile_param_name_id[param_ident],
         profile_ptr->disallow_timer[5]);
      break;

    case LINGER_TIMEOUT_VAL:
      snprintf(buff,bufflength,"%s:%d;\r\n",
         profile_param_name_id[param_ident],
         profile_ptr->linger_timeout_val);
      break;

    case APN_CLASS:
      snprintf(buff,bufflength,"%s:%d;\r\n",
         profile_param_name_id[param_ident],
         profile_ptr->apn_class);
      break;

    case PDN_LEVEL_AUTH_PROTOCOL:
      if (DATA_SESS_AUTH_PROTOCOL_PAP == profile_ptr->pdn_level_auth_protocol)
      {
        snprintf(buff,bufflength,"%s:PAP;\r\n",
                       profile_param_name_id[param_ident]);
      }
      else if (DATA_SESS_AUTH_PROTOCOL_CHAP == profile_ptr->pdn_level_auth_protocol)
      {
        snprintf(buff,bufflength,"%s:CHAP;\r\n",
                       profile_param_name_id[param_ident]);
      }
      else if (DATA_SESS_AUTH_PROTOCOL_NONE == profile_ptr->pdn_level_auth_protocol)
      {
        snprintf(buff,bufflength,"%s:NONE;\r\n",
                       profile_param_name_id[param_ident]);
      }
      else if (DATA_SESS_AUTH_PROTOCOL_PAP_CHAP == profile_ptr->pdn_level_auth_protocol)
      {
        snprintf(buff,bufflength,"%s:PAP_CHAP;\r\n",
                       profile_param_name_id[param_ident]);
      }
      else
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                        "Str Construct err for PDN LEVEL AUTH PROTOCOL");
        return FALSE;
      }
      break;

    case PDN_LEVEL_USER_ID:
      /* The string in the profile is not null terminated.
         Null terminate before using*/
      memscpy(user_id,
              PPP_MAX_USER_ID_LEN,
              profile_ptr->pdn_level_user_id,
              profile_ptr->pdn_level_user_id_len);
      user_id[profile_ptr->pdn_level_user_id_len] = '\0';
      snprintf(buff,bufflength,"%s:%s;\r\n",
                     profile_param_name_id[param_ident],
                     user_id);
      break;

    case PDN_LEVEL_AUTH_PASSWORD:
      /* The string in the profile is not null terminated.
         Null terminate before using*/
      memscpy(auth_password,
              PPP_MAX_PASSWD_LEN,
              profile_ptr->pdn_level_auth_password,
              profile_ptr->pdn_level_auth_password_len);
      auth_password[profile_ptr->pdn_level_auth_password_len] = '\0';
      snprintf(buff,bufflength,"%s:%s;\r\n",
                     profile_param_name_id[param_ident],
                     auth_password);
      break;

    case USER_APP_DATA:
      snprintf(buff,bufflength,"%s:%ld;\r\n",
                     profile_param_name_id[param_ident],
                     profile_ptr->user_app_data);
      break;

    case DNS_DHCP_REQ_FLAG:
      snprintf(buff,bufflength,"%s:%d;\r\n",
                     profile_param_name_id[param_ident],
                     profile_ptr->dns_addr_using_dhcp);
      break;

    case PCSCF_DHCP_REQ_FLAG:
      snprintf(buff,bufflength,"%s:%d;\r\n",
                     profile_param_name_id[param_ident],
                     profile_ptr->pcscf_addr_using_dhcp);
      break;

    default:
      DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Invalid Param Identifier passed");
      return FALSE;
  }

  /*DATA_IS707_MSG_SPRINTF_1( MSG_LEGACY_HIGH,
                            "Constructed string is: %s", buff);
  */

  return TRUE;
}/* ds707_profile_construct_str_from_ident */

/*===========================================================================
FUNCTION      DS707_GET_PROFILE_FD

DESCRIPTION   For a given profile id, if the profile_xxx.txt is present in the
              efs, creates profile_xxx.tmp file and returns the file
              descriptor.Else creates profile_xxx.txt and returns the fd

PARAMETERS    profile_id         : profile number
              efs_file_name      : file path for profile_xxx.txt
              efs_tmp_file_name  : file path for profile_xxx.tmp
              create_tmp_profile : boolean to indicate if a tmp profile was
                                   created

DEPENDENCIES  None

RETURN VALUE  EFS File Descriptor

SIDE EFFECTS  None
===========================================================================*/
int ds707_get_profile_fd(
  ds707_data_session_profile_id profile_id,
  char *efs_file_name,
  char *efs_tmp_file_name,
  boolean *create_tmp_profile
)
{
  int fd = -1; /* EFS File Descriptor*/

  /* function to create efs_file_name for specific profile number */
  ds707_create_efs_file_name(efs_file_name,profile_id);
  ds707_create_efs_file_name(efs_tmp_file_name,profile_id);


  /* Check if the given profile_xxx.txt file exists in efs*/
  fd = mcfg_fopen( efs_file_name,
                   MCFG_FS_O_WRONLY,
                   MCFG_FS_DEFFILEMODE,
                   MCFG_FS_TYPE_EFS,
                   (mcfg_fs_sub_id_e_type) ds707_curr_subs_id());
  if (-1 == fd)
  {
    if (ENOENT == mcfg_fs_errno(MCFG_FS_TYPE_EFS))
    {
      DATA_IS707_MSG1(MSG_LEGACY_HIGH,
                      "%s doesn't exist. Creating one",
                      efs_file_name);
      *create_tmp_profile = FALSE;
    }
    else
    {
      DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                      "EFS_Open Err: %d", mcfg_fs_errno(MCFG_FS_TYPE_EFS));
      return fd;
    }
  }
  else
  {
    DATA_IS707_MSG1(MSG_LEGACY_HIGH,
                      "%s exists. Create temp profile",
                      efs_file_name);
    /* close the profile_xxx.txt file as we no longer need it*/
    (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS);
    fd = -1;
    *create_tmp_profile = TRUE;
  }

  if (TRUE == *create_tmp_profile)
  {
    /* profile_xxx.txt exists. Creating profile_xxx.tmp */
    fd = mcfg_fopen( efs_tmp_file_name,
                   MCFG_FS_O_CREAT | MCFG_FS_O_TRUNC | MCFG_FS_O_WRONLY,
                   MCFG_FS_DEFFILEMODE,
                   MCFG_FS_TYPE_EFS,
                   (mcfg_fs_sub_id_e_type) ds707_curr_subs_id());
    if (-1 == fd)
    {
      DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                      "EFS_Open Unable to create tmp profile file Err: %d",
                      mcfg_fs_errno(MCFG_FS_TYPE_EFS));
      return fd;
    }
    DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                    "Temp profile file created successfully");
  }
  else
  {
    fd = ds707_create_profile_file_in_efs(efs_file_name);
  }

  return fd;
} /* ds707_get_profile_fd */

/*===========================================================================
FUNCTION      DS707_WRITE_PROFILE_TO_EFS

DESCRIPTION   Writes the given profile's contents to EFS

PARAMETERS    profile_id    : profile number
              dsi_3gpp2_ptr : pointer to profile blob

DEPENDENCIES  None

RETURN VALUE  TRUE on Success
              FALSE on Failure

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_write_profile_to_efs(
  ds707_data_session_profile_id profile_id,
  ds707_data_session_profile_info_type *data_sess_profile_ptr
)
{
  char efs_file_name[PROFILE_PATH_LEN]=PROFILE_PATH;
  char efs_tmp_file_name[PROFILE_PATH_LEN]=PROFILE_TMP_PATH;
  boolean create_tmp_profile                              = FALSE;
  int fd                                                  = -1; /*EFS File Desc*/
  uint8 param_ident                                       = 0;
  uint16 buff_size                                        = 0; 
  char *tempbuff                                          = NULL;
  char *writebuff                                         = NULL;
  uint32 writebufflen                                     = 0;
  fs_ssize_t efs_ret                                      = -1;


  /* Create EFS profile file and get the fd */
  fd = ds707_get_profile_fd(profile_id,
                            efs_file_name,
                            efs_tmp_file_name,
                            &create_tmp_profile);

  if (-1 == fd)
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Unable to get fd for profile: %d", profile_id);
    return FALSE;
  }

  buff_size = DS707_PROFILE_MAX_PARAM_BUFF_SIZE * sizeof(char);
  tempbuff = (char*) modem_mem_alloc(buff_size, MODEM_MEM_CLIENT_DATA);
  if(tempbuff == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ds707_write_profile_to_efs():"
                    "memory allocation failed");
    return FALSE;
  }
  writebuff = (char*) modem_mem_alloc(buff_size, MODEM_MEM_CLIENT_DATA);  
  if(writebuff == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ds707_write_profile_to_efs():"
                    "memory allocation failed");
    modem_mem_free(tempbuff, MODEM_MEM_CLIENT_DATA);
    return FALSE;
  }
  memset((void *)tempbuff, 0, buff_size);
  memset((void *)writebuff, 0, buff_size);
     
  for( param_ident = DS707_PROFILE_PARAM_ID_MIN;
       param_ident <= DS707_PROFILE_PARAM_ID_MAX;
       param_ident++ )
  {
    if( data_sess_profile_ptr->custom_val_mask & CONVERT_ID_TO_MASK(param_ident) )
    {
      if (!ds707_profile_construct_str_from_ident(data_sess_profile_ptr,
                                                 param_ident,
                                                 tempbuff,
                                                 buff_size))
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                        "Error constructing str from ident. Bailing out");
        goto EFS_ERR;
      }

      /* strlen+1 to account for the NULL terminator*/
      if( (strlen(tempbuff)+1) < (buff_size -(strlen(writebuff)+1)))
      {
        strlcat(writebuff,tempbuff,buff_size);
      }
      else
      {
        /* No space in writebuff to concatenate tempbuff. So efs_write
           writebuff, clear it and then concatenate tempbuff */
        writebufflen = strlen(writebuff);
        DATA_IS707_MSG1(MSG_LEGACY_HIGH,
                          "Writing %d bytes to EFS", writebufflen);
        efs_ret = (fs_size_t) mcfg_fwrite(fd, 
                                          writebuff, 
                                          writebufflen, 
                                          MCFG_FS_TYPE_EFS);
        if (-1 == efs_ret || efs_ret != writebufflen)
        {
          DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                          "EFS Write failed. Efs_ret:%d", efs_ret);
          goto EFS_ERR;
        }

        /* Now that we have written the existing buffer to efs, clear the
           contents */
        memset((void *)writebuff, 0, buff_size);
        writebufflen = 0;
        strlcat(writebuff,tempbuff,buff_size);
      }
    }
  }

  /* Write the remaining contents in writebuff to efs */
  writebufflen = strlen(writebuff);
  if (writebufflen)
  {
    DATA_IS707_MSG1(MSG_LEGACY_HIGH,
                          "Writing %d bytes to EFS", writebufflen);
    efs_ret = (fs_size_t) mcfg_fwrite(fd, 
                                          writebuff, 
                                          writebufflen, 
                                          MCFG_FS_TYPE_EFS);
    if (-1 == efs_ret || efs_ret != writebufflen)
    {
      DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                      "EFS Write failed. Efs_ret:%d", efs_ret);
      goto EFS_ERR;
    }
  }
  modem_mem_free(tempbuff, MODEM_MEM_CLIENT_DATA);
  modem_mem_free(writebuff, MODEM_MEM_CLIENT_DATA);

  if (-1 == mcfg_fclose( fd, MCFG_FS_TYPE_EFS))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "EFS Close failed");
    return FALSE;
  }

  /* If we wrote the contents to a temp file, delete the orig .txt file
     and rename the .tmp to .txt
  */
  if (create_tmp_profile)
  {
     if (MCFG_FS_STATUS_OK != mcfg_fs_delete(efs_file_name,
                              MCFG_FS_TYPE_EFS, 
                              (mcfg_fs_sub_id_e_type) ds707_curr_subs_id()))
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, "EFS Unlink failed");
      return FALSE;
    }

    if (-1 == mcfg_fs_rename(efs_tmp_file_name,
                             efs_file_name,
                             MCFG_FS_TYPE_EFS, 
                             (mcfg_fs_sub_id_e_type)ds707_curr_subs_id()))
    {
      DATA_IS707_MSG2(MSG_LEGACY_ERROR, "EFS Rename failed: %d, subs_id 0x%0x",
                      mcfg_fs_errno(MCFG_FS_TYPE_EFS),(mcfg_fs_sub_id_e_type)ds707_curr_subs_id() );
      return FALSE;
    }
  }

  return TRUE;

EFS_ERR:
  modem_mem_free(tempbuff, MODEM_MEM_CLIENT_DATA);
  modem_mem_free(writebuff, MODEM_MEM_CLIENT_DATA); 

  /* delete the profile_xxx.tmp/.txt file */
  if (-1 == mcfg_fclose( fd, MCFG_FS_TYPE_EFS))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "EFS Close failed");
    return FALSE;
  }
  if (create_tmp_profile)
  {
    mcfg_fs_delete(efs_tmp_file_name,
                   MCFG_FS_TYPE_EFS, 
                   (mcfg_fs_sub_id_e_type)ds707_curr_subs_id());
  }
  else
  {
    mcfg_fs_delete(efs_file_name,
                   MCFG_FS_TYPE_EFS, 
                   (mcfg_fs_sub_id_e_type)ds707_curr_subs_id());
  }
  return FALSE;

} /* ds707_write_profile_to_efs */

/*===========================================================================
FUNCTION DS707_RESET_DATA_SESSION_PROFILE_FROM_NVRUIM

DESCRIPTION
  This function reset the profile related information.

DEPENDENCIES
 None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_reset_data_session_profile_from_nvruim(void)
{
  DATA_IS707_MSG0(MSG_LEGACY_MED,
                  "Card removed resetting number of valid profiles to 0");
  num_valid_profiles = 0;
} /* ds707_reset_data_session_profile_from_nvruim */

#ifdef FEATURE_DATA_FALLBACK_LIST
/*===========================================================================
FUNCTION      DS707_DATA_SESSION_IS_IN_FALLBACK_APN_LIST

DESCRIPTION   This function checks whether the input apn is in the 
              fallback_apn_list.txt which records all the apn names that
              should fallback upon PDN connection failure.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  boolean: TRUE: is in
                       FALSE: is NOT in

SIDE EFFECTS  None
===========================================================================*/
static boolean ds707_data_session_is_in_fallback_apn_list(
                                                const char  *apn_name,
                                                const uint8 apn_len
                                                )
{
  boolean return_val = FALSE;
  char efs_file_name[30]="/eHRPD/fallback_apn_list.txt";
  char *from = NULL, *to = NULL;/* ptrs to start and end of the token     */
  ds3gcfgmgr_efs_token_type  efs_db; /* structure used for tokenizing the file */
  ds3gcfgmgr_efs_token_parse_status_type  efs_ret_val
                                              = DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS;
  uint8 length = 0;
  /*store each string read from the efs, might be apn name or pdb label*/
  char efs_string[DS_VSNCP_3GPP2_APN_MAX_VAL_LEN] = {0};
  uint8 efs_string_len = 0;

  /*input check*/
  if(NULL == apn_name ||
     0 == apn_len)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "ds707_data_session_is_in_fallback_apn_list, invalid input");
    return FALSE;
  }
  if(ds3gcfgmgr_efs_file_init(efs_file_name, &efs_db) == -1)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Error opening file ISN from EFS");
    return FALSE;
  }
  while (DS3GCFGMGR_EFS_TOKEN_PARSE_EOF
         != (efs_ret_val = ds3gcfgmgr_efs_tokenizer(&efs_db, &from, &to )))
  {
    /*------------------------------------------------------------------------
      Token being read. from points to the beginning of the token and
      to point to the end of the token.

      The tokenizer automatically skips blank lines and comments (lines
      beginning with #, so no need to check for them here).
    ------------------------------------------------------------------------*/
    if((from == to) || (DS3GCFGMGR_EFS_TOKEN_PARSE_EOL == efs_ret_val))
    {
      /*----------------------------------------------------------------------
        Skip empty tokens.
      ----------------------------------------------------------------------*/
      continue;
    }
    else if(DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS == efs_ret_val)
    {
      length = (uint8)( to - from);
      if ( (from > to)  || (length > DS_VSNCP_3GPP2_APN_MAX_VAL_LEN))
      {
        efs_string_len = 0;
        DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                        "Error in APN String. Setting APN length to 0.");
      }
      else
      {
        efs_string_len = length;
        (void)memscpy(efs_string,
                      efs_string_len,
                      from,
                      efs_string_len);
      }
      if ( efs_string_len == apn_len &&
           0 == strncasecmp( efs_string, apn_name, apn_len ))
      {
        return_val = TRUE;
        break;
      }
    }
  }
  ds3gcfgmgr_efs_file_close(&efs_db);
  return return_val;
}
/*===========================================================================
FUNCTION      DS707_DATA_SESSION_IS_IN_FALLBACK_PDN_LIST

DESCRIPTION   This function checks whether the corresponding PDN label of the 
              input apn name is in the fallback_pdn_list.txt which records
              all the apn names that should fallback upon PDN connection
              failure.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  boolean: TRUE: is in
                       FALSE: is NOT in

SIDE EFFECTS  None
===========================================================================*/
static boolean ds707_data_session_is_in_fallback_pdn_list(
                                          const char  *apn_name,
                                          const uint8 apn_len
                                          )
{
  boolean return_val = FALSE;
  char efs_file_name[30]="/eHRPD/fallback_pdn_list.txt";
  char *from = NULL, *to = NULL;/* ptrs to start and end of the token     */
  ds3gcfgmgr_efs_token_type  efs_db; /* structure used for tokenizing the file */
  ds3gcfgmgr_efs_token_parse_status_type  efs_ret_val
                                              = DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS;
  uint8 length = 0;
  /*store each string read from the efs, might be apn name or pdb label*/
  char efs_string[DS_VSNCP_3GPP2_APN_MAX_VAL_LEN] = {0};
  uint8 efs_string_len = 0;
  char *mapped_label = 0;
  uint8 mapped_label_len = 0;

  /*input check*/
  if(NULL == apn_name ||
     0 == apn_len)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "ds707_data_session_is_in_fallback_pdn_list(): Invalid input");
    return FALSE;
  }
  mapped_label_len = 
    (uint8)ds707_data_session_map_apnname_to_label(apn_name, apn_len, &mapped_label);
  if(0 == mapped_label_len)
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED,
         "ds707_data_session_is_in_fallback_pdn_list(): No label found");
    return FALSE;
  }

  if(ds3gcfgmgr_efs_file_init(efs_file_name, &efs_db) == -1)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH,
             "ds707_data_session_is_in_fallback_pdn_list():"
             "Error opening file ISN from EFS");
    return FALSE;
  }
  /* Set the seperator as ; */
  efs_db.seperator = ';';

  while (DS3GCFGMGR_EFS_TOKEN_PARSE_EOF
         != (efs_ret_val = ds3gcfgmgr_efs_tokenizer(&efs_db, &from, &to )))
  {
    /*------------------------------------------------------------------------
      Token being read. from points to the beginning of the token and
      to point to the end of the token.

      The tokenizer automatically skips blank lines and comments (lines
      beginning with #, so no need to check for them here).
    ------------------------------------------------------------------------*/
    if((from == to) || (DS3GCFGMGR_EFS_TOKEN_PARSE_EOL == efs_ret_val))
    {
      /*----------------------------------------------------------------------
        Skip empty tokens.
      ----------------------------------------------------------------------*/
      continue;
    }
    else if(DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS == efs_ret_val)
    {
      length = (uint8)( to - from);
      if ( (from > to)  || (length > DS_VSNCP_3GPP2_APN_MAX_VAL_LEN))
      {
        efs_string_len = 0;
        DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                        "Error in APN String. Setting APN length to 0.");
      }
      else
      {
        efs_string_len = length;
        (void)memscpy(efs_string,
                      efs_string_len,
                      from,
                      efs_string_len);
      }

      if(
         mapped_label_len == efs_string_len &&
         0 == strncasecmp( mapped_label, efs_string, efs_string_len )
         )
      {
        return_val = TRUE;
        break;
      }
    }
  }
  ds3gcfgmgr_efs_file_close(&efs_db);
  DATA_IS707_MSG1(MSG_LEGACY_HIGH,
                  "APN String in fallback pdn list:%d",return_val);
  return return_val;
}

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_IS_IN_FALLBACK_LIST

DESCRIPTION   This function checks both fallback_pdn_list.txt and 
              fallback_apn_list.txt to find whether the input apn is in the
              2 list

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  boolean: TRUE: is in either list
                       FALSE: is NOT in both lists

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_session_is_in_fallback_list(
                                          const char  *apn_name,
                                          const uint8 apn_len
                                          )
{
  boolean is_in = FALSE;
  
  is_in = ds707_data_session_is_in_fallback_apn_list(apn_name,apn_len);
  /*if not in the fallback_apn_list, 
    continue to find if in the fallback_pdn_list*/
  if(!is_in)
  {
    is_in = ds707_data_session_is_in_fallback_pdn_list(apn_name,apn_len);
  }
  return is_in;
}

#endif /*FEATURE_DATA_FALLBACK_LIST*/

/*lint -restore Restore lint error 641*/
/*lint -restore Restore lint error 655*/

/*===========================================================================
FUNCTION       DS707_DATA_SESSION_SET_SIP_ACTIVE_PROFILE_INDEX

DESCRIPTION    This function sets the sip_active_profile_index.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None

===========================================================================*/
void ds707_data_session_set_sip_active_profile_index
(
  uint8 sip_active_profile_index
)
{
  DATA_IS707_MSG1( MSG_LEGACY_MED,"Set SIP active profile index:  %d", 
                   sip_active_profile_index );

  ds707_sip_active_profile_index = sip_active_profile_index;

} /* ds707_data_session_set_sip_active_profile_index() */

/*===========================================================================
FUNCTION       DS707_DATA_SESSION_GET_SIP_ACTIVE_PROFILE_INDEX

DESCRIPTION    This function returns sip_active_profile_index.

DEPENDENCIES   None.

RETURN VALUE   uint8 - sip active profile index.

SIDE EFFECTS   None

===========================================================================*/
uint8 ds707_data_session_get_sip_active_profile_index
(
  void
)
{
  DATA_IS707_MSG1( MSG_LEGACY_MED, "Get SIP active profile index:: %d", 
                   ds707_sip_active_profile_index);

  return ds707_sip_active_profile_index;
} /* ds707_data_session_get_sip_active_profile_index() */

/*===========================================================================
FUNCTION      DS707_DATA_SESS_SET_NVRUIM_PROFILE_ALLOCATED

DESCRIPTION   This function is called when memory has been allocated for 
              storing NVRUIM profiles

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_data_sess_set_nvruim_profile_allocated
(
  boolean val
)
{
  ds707_is_nvruim_profile_allocated = val;
} /* ds707_data_sess_set_nvruim_profile_allocated */

/*===========================================================================
FUNCTION      DS707_DATA_SESS_IS_NVRUIM_PROFILE_ALLOCATED

DESCRIPTION   Returns the global variable that records whether memory has 
              been allocated for storing NVRUIM profiles

DEPENDENCIES  None

RETURN VALUE  boolean
              TRUE  - if memory has been allocated for NVRUIM profiles
              FALSE - if memory has not been allocated

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_sess_is_nvruim_profile_allocated
(
  void
)
{
  DATA_IS707_MSG1( MSG_LEGACY_MED,
                   "Memory is allocated for NVRUIM profiles: %d", 
                   ds707_is_nvruim_profile_allocated);

  return ds707_is_nvruim_profile_allocated;
} /* ds707_data_sess_is_nvruim_profile_allocated */

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_GET_PROFILE_APN_NAME

DESCRIPTION   Checks with MPPM if call is allowed for a profile on 3GPP2.

PARAMETERS    profile_id - The profile to check.
              apn_name   - The apn name holder passed by caller.
 
DEPENDENCIES  None

RETURN VALUE  TRUE   - Call is allowed. 
              FALSE  - Call is not allowed.

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_session_get_profile_apn_name 
( 
  ds707_data_session_profile_info_type * data_sess_prof_ptr,
  ds3g_apn_name_type              * apn_name
)
{
  if (apn_name == NULL || data_sess_prof_ptr  == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "NULL pointers passed for"
                    " ds707_data_session_get_profile_apn_name");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    initialize APN name
  -------------------------------------------------------------------------*/
  memset ( apn_name, 0, sizeof(ds3g_apn_name_type) );

  DATA_IS707_MSG1(MSG_LEGACY_MED,
                  "ds707_data_session_get_profile_apn_name for profile %d",
                  data_sess_prof_ptr->data_session_profile_id);

  /*-------------------------------------------------------------------------
    Get APN string length and string.
  -------------------------------------------------------------------------*/
  apn_name->apn_string_len = data_sess_prof_ptr->apn_string_len;
  if( apn_name->apn_string_len >= DS_SYS_MAX_APN_LEN )
  {
    DATA_IS707_MSG1(MSG_LEGACY_HIGH,
                    "ds707_data_session_get_profile_apn_name."
                    " APN length got from cache = %d, is invalid. Return FALSE.",
                      apn_name->apn_string_len);
    apn_name->apn_string_len = 0;
    return FALSE;
  }

  if( apn_name->apn_string_len > 0 )
  {
    memscpy(apn_name->apn_string,
            apn_name->apn_string_len,
            data_sess_prof_ptr->apn_string,
            data_sess_prof_ptr->apn_string_len);
  }

  return TRUE;
} /*ds707_data_session_get_profile_apn_name*/

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_ALLOC_CACHE_ENTRY
 
DESCRIPTION   Provide profile cache entry for newly created profile.
 
DEPENDENCIES  None

RETURN VALUE  TRUE   - if cache for new profile is allocated successfully
              FALSE  - otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_session_alloc_cache_entry
(
  ds707_data_session_profile_info_type** profile_info_ptr,
  ds_profile_subs_id_enum_type           subs_id
)
{
  ds707_data_session_profile_info_type  *profile_info_realloc = NULL;
  ds707_data_session_profile_info_type **ds707_data_session_profile_ptr = NULL;
  ds707_data_session_profile_id         *max_profiles_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (subs_id)
  {
  case ACTIVE_SUBSCRIPTION_1:
    ds707_data_session_profile_ptr = &ds707_data_session_profile_info;
    break;

  case ACTIVE_SUBSCRIPTION_2:
    ds707_data_session_profile_ptr = 
      &ds707_data_session_profile_info_secondary_subs;
    break;

  case ACTIVE_SUBSCRIPTION_3:
    ds707_data_session_profile_ptr = 
      &ds707_data_session_profile_info_tertiary_subs;
    break;

  default:
    return FALSE;
  }

  DS_ENTER_CRIT_SECTION(&ds707_profile_crit_sect);

  max_profiles_ptr = 
    &ds707_data_session_max_profiles[subs_id - ACTIVE_SUBSCRIPTION_1];

  /*-------------------------------------------------------------------
   Reallocate memory.
   --------------------------------------------------------------------*/
  profile_info_realloc = 
    (ds707_data_session_profile_info_type *)modem_mem_realloc(
      (void *)(*ds707_data_session_profile_ptr),
      (*max_profiles_ptr + 1) * sizeof(ds707_data_session_profile_info_type),
      MODEM_MEM_CLIENT_DATA_CRIT);

  if( NULL == profile_info_realloc )
  {
    DATA_ERR_FATAL("profile memory re-allocation failed");
    DS_LEAVE_CRIT_SECTION(&ds707_profile_crit_sect);
    return FALSE;
  }
  /*--------------------------------------------------------------------
    If the ptr block IS successfully resized and the returned value is
    different from the ptr value passed in, the old block passed in must
    be considered deallocated and no longer useable. Hence we are
    sending it back to the global variable.
  --------------------------------------------------------------------*/

  *ds707_data_session_profile_ptr = profile_info_realloc;
  memset((void *)(*ds707_data_session_profile_ptr + *max_profiles_ptr),
         0, sizeof(ds707_data_session_profile_info_type));

  (*ds707_data_session_profile_ptr)[*max_profiles_ptr].data_session_profile_id =
    DATA_SESSION_PROFILE_ID_INVALID;
  (*ds707_data_session_profile_ptr)[*max_profiles_ptr].profile_type =
    PROFILE_INVALID;

  *profile_info_ptr = &((*ds707_data_session_profile_ptr)[*max_profiles_ptr]);

  /*------------------------------------- 
    Increase number of profiles
   -------------------------------------*/
  (*max_profiles_ptr)++;

  DS_LEAVE_CRIT_SECTION(&ds707_profile_crit_sect);

  if (subs_id == ACTIVE_SUBSCRIPTION_1)
  {
    ds707_data_session_ehrpd_valid_profile++;
  }

  return TRUE;

}/*ds707_data_session_alloc_cache_entry*/

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_DELETE_CACHE_ENTRY

DESCRIPTION   Delete the cache entry for the profile
 
DEPENDENCIES  None

RETURN VALUE  TRUE   - if cache entry is deleted successfully
              FALSE  - otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_session_delete_cache_entry
(
  ds707_data_session_profile_id profile_id,
  ds_profile_subs_id_enum_type  subs_id
)
{
  ds707_data_session_profile_id           delete_index = -1;
  ds707_data_session_profile_info_type  *profile_info_realloc = NULL;
  ds707_data_session_profile_info_type **ds707_data_session_profile_ptr_ptr
                                                                        = NULL;
  ds707_data_session_profile_id         *max_profiles_ptr = NULL;
  ds707_data_session_profile_id          del_profile_id  = -1;
  boolean retval = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (subs_id)
  {
  case ACTIVE_SUBSCRIPTION_1:
    ds707_data_session_profile_ptr_ptr = &ds707_data_session_profile_info;
    break;

  case ACTIVE_SUBSCRIPTION_2:
    ds707_data_session_profile_ptr_ptr = 
      &ds707_data_session_profile_info_secondary_subs;
    break;

  case ACTIVE_SUBSCRIPTION_3:
    ds707_data_session_profile_ptr_ptr = 
      &ds707_data_session_profile_info_tertiary_subs;
    break;

  default:
    return FALSE;
  }

  DS_ENTER_CRIT_SECTION(&ds707_profile_crit_sect);

  max_profiles_ptr = 
    &ds707_data_session_max_profiles[subs_id - ACTIVE_SUBSCRIPTION_1];

  if ( (*ds707_data_session_profile_ptr_ptr == NULL) ||
       (*max_profiles_ptr == 0) )
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR,
                     "No valid profile exists on subs ID %d",
                     subs_id);
    DS_LEAVE_CRIT_SECTION(&ds707_profile_crit_sect);
    return FALSE;
  }

  /*-------------------------------------------------------------------- 
    Search for the profile to be deleted
   --------------------------------------------------------------------*/
  for (delete_index = DATA_SESSION_PROFILE_ID_MIN;
       delete_index < *max_profiles_ptr;
       delete_index++)
  {
    del_profile_id = (*ds707_data_session_profile_ptr_ptr)[delete_index].data_session_profile_id;

    if(del_profile_id == profile_id )
    {
      break;
    }
  }

  if ( delete_index == *max_profiles_ptr )
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR,
                     "Invalid profile num %d to delete",
                     profile_id);
    DS_LEAVE_CRIT_SECTION(&ds707_profile_crit_sect);
    return FALSE;
  }

  DATA_IS707_MSG2( MSG_LEGACY_MED,
                   "Deleting profile %d in cache index %d",
                   profile_id,
                   delete_index);

  (*max_profiles_ptr)--;

  if (subs_id == ACTIVE_SUBSCRIPTION_1)
  {
    ds707_data_session_ehrpd_valid_profile--;
    ds707_data_session_update_min_apn_list(NULL, ((*ds707_data_session_profile_ptr_ptr)[delete_index].apn_string));
  }

  for (; delete_index < *max_profiles_ptr; delete_index++)
  {
    (*ds707_data_session_profile_ptr_ptr)[delete_index] 
                    = (*ds707_data_session_profile_ptr_ptr)[delete_index + 1];
  }

  /*-------------------------------------------------------------------- 
    Do modem_mem_realloc to resize memory. It works as modem_mem_free
    if size is 0.
   --------------------------------------------------------------------*/
  profile_info_realloc = 
        (ds707_data_session_profile_info_type *)
          modem_mem_realloc(
            (void *)(*ds707_data_session_profile_ptr_ptr),
            (*max_profiles_ptr) * sizeof(ds707_data_session_profile_info_type),
            MODEM_MEM_CLIENT_DATA_CRIT);

  /*-------------------------------------------------------------------- 
    If the returned pointer is NULL, memory reallocation failed. Ideally
    we shall not hit this, but if so, the original memory block is left
    untouched.
   --------------------------------------------------------------------*/
  if ( profile_info_realloc == NULL && 
       ((*max_profiles_ptr) != 0) ) 
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Memory reallocation failed");
    retval = FALSE;
  }
  else
  {
    /*--------------------------------------------------------------------
      If the ptr block IS successfully resized and the returned value is
      different from the ptr value passed in, the old block passed in must
      be considered deallocated and no longer useable. Hence we are
      sending it back to the global variable.
     --------------------------------------------------------------------*/
    if ((*max_profiles_ptr) == 0)
    {
      *ds707_data_session_profile_ptr_ptr = NULL;
    }
    else
    {
      *ds707_data_session_profile_ptr_ptr = profile_info_realloc;
    }

    retval = TRUE;
  }
  DS_LEAVE_CRIT_SECTION(&ds707_profile_crit_sect);

  return retval;
}/*ds707_data_session_delete_cache_entry*/

/*===========================================================================
FUNCTION ds707_data_sess_profile_get_stored_ppp_opts

DESCRIPTION
  This function returns stored default ppp opts read from NV during
  boot up.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void ds707_data_sess_profile_get_stored_ppp_opts
(
  data_sess_profile_ppp_params_type **ppp_params_ptr
)
{
  DATA_IS707_MSG0( MSG_LEGACY_MED, "Get default PPP opts" );
  *ppp_params_ptr = &ds707_default_ppp_params;
}

/*===========================================================================
FUNCTION DS707_IS_APN_PREF_PKT_MODE_EX

DESCRIPTION
  This function runs the pref mode check for the PDN for 3GPP2.
  
PARAMETERS  
  profile_id         - 3GPP2 profile ID for this PDN
  mode               - System mode
  subs_id            - Resolved subs ID
      
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE : If mode is the preferred pkt mode for this PDN
  FALSE: If mode is not the preferred pkt mode for this PDN
  
SIDE EFFECTS 
  None.

===========================================================================*/
boolean ds707_is_apn_pref_pkt_mode_ex
( 
  ds707_data_session_profile_info_type *data_sess_prof_ptr,
  sys_sys_mode_e_type               mode,
  sys_modem_as_id_e_type            subs_id
)
{
  ds3g_apn_name_type              apn_name;
  boolean                         ret_val = FALSE;
  ds_pkt_sys_mode_e_type          apn_pkt_mode;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if(data_sess_prof_ptr == NULL)
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                    "In ds707_is_apn_pref_pkt_mode_ex: Null data session ptr ");
    return FALSE;
  }

#ifdef FEATURE_DATA_WLAN_MAPCON
  /*-------------------------------------------------------------------------
    Get APN name for MAPCON. If MAPCON feature is not defined, passing a
    valid APN name ptr is enough, no need to handle the name get failure.
  -------------------------------------------------------------------------*/
  if(ds707_data_session_get_profile_apn_name(data_sess_prof_ptr, &apn_name) == FALSE)
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR,
                     "In ds707_is_apn_pref_pkt_mode_ex: failed to get apn name for "
                     "profile %d, Return FALSE", data_sess_prof_ptr->data_session_profile_id);
    return FALSE;
  }
#endif /* FEATURE_DATA_WLAN_MAPCON */
  apn_pkt_mode = ds3g_sys_sys_mode_to_ds_apn_sys_mode(mode);
  ret_val = ds3g_is_apn_pref_pkt_mode_ex(apn_pkt_mode,
                                         &apn_name,
                                         subs_id);

  DATA_IS707_MSG3( MSG_LEGACY_MED,
                   "ds707_is_apn_pref_pkt_mode_ex: mode %d profile %d ret: %d",
                    mode, data_sess_prof_ptr->data_session_profile_id, ret_val);

  return ret_val;
} /*ds707_is_apn_pref_pkt_mode_ex*/

/*===========================================================================
FUNCTION DS707_GET_SYS_PREF_MODE

DESCRIPTION
  This function returns the sys pref mode for the PDN in 3GPP2.
  
PARAMETERS  
  pointer to the data_session_profile
      
DEPENDENCIES 
  None.
  
RETURN VALUE 
  pref_mode - Sys preferred mode corresponding to the APN
==========================================================================*/
sys_sys_mode_e_type 
ds707_get_sys_pref_mode
(
  ds707_data_session_profile_info_type *data_sess_prof_ptr
)
{
  ds3g_apn_name_type              apn_name;
  ds_pkt_sys_mode_e_type          apn_pkt_mode;
  sys_sys_mode_e_type             pref_mode;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#ifdef FEATURE_DATA_WLAN_MAPCON
  /*-------------------------------------------------------------------------
    Get APN name for MAPCON. If MAPCON feature is not defined, passing a
    valid APN name ptr is enough, no need to handle the name get failure.
  -------------------------------------------------------------------------*/
  if(ds707_data_session_get_profile_apn_name(data_sess_prof_ptr, &apn_name) 
       == FALSE)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Failed to get apn name");
  }
#endif /* FEATURE_DATA_WLAN_MAPCON */

  apn_pkt_mode = ds3g_get_apn_pref_pkt_mode(&apn_name);
  pref_mode = ds3g_pkt_sys_mode_to_sys_sys_mode(apn_pkt_mode);
  return pref_mode;
} /* ds707_get_sys_pref_mode() */

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_IS_OMH_REQD

DESCRIPTION   Checks if OMH is required or not. OMH will be required if the
              following conditions are met.
              1. Call is on 1x/HRPD(non eHRPD) and
              2. Either the existing profile id or the requested profile id
                 supports OMH.

PARAMETERS    this_iface_ptr - current iface pointer
 
DEPENDENCIES  None

RETURN VALUE  TRUE   - If OMH is required. 
              FALSE  - If OMH is not required.

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_session_is_omh_reqd 
( 
  ps_iface_type* this_iface_ptr
)
{
  ds707_data_session_profile_id         current_id; 
  ds707_data_session_profile_id         requesting_id; 
  ds707_data_session_profile_info_type *current_profile_ptr = NULL;
  ds707_data_session_profile_info_type *requesting_profile_ptr = NULL;
  boolean                               ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ds707_data_session_get_iface_current_profile(this_iface_ptr, 
                                               &current_profile_ptr);
  ds707_data_session_get_iface_requesting_profile(this_iface_ptr, 
                                                  &requesting_profile_ptr);

  if(current_profile_ptr == NULL)
  {
    current_id = DATA_SESSION_PROFILE_ID_INVALID;
  }
  else
  {
    current_id = current_profile_ptr->data_session_profile_id;
  }

  if(requesting_profile_ptr == NULL)
  {
    requesting_id = DATA_SESSION_PROFILE_ID_INVALID;
  }
  else
  {
    requesting_id = requesting_profile_ptr->data_session_profile_id;
  }             

  if( ( ( (ds707_pkt_mgr_get_current_sys_mode() == SYS_SYS_MODE_HDR) &&
          (ds3gdsdif_ehprd_is_pref_system(SYS_MODEM_AS_ID_1) == FALSE) ) ||
        ( ds707_pkt_mgr_get_current_sys_mode() == SYS_SYS_MODE_CDMA ) ) &&
      ( (current_id == DATA_SESSION_PROFILE_ID_INVALID &&
         requesting_id == DATA_SESSION_PROFILE_ID_INVALID) ||
        (current_id == DATA_SESSION_PROFILE_ID_INVALID &&
         ds707_data_session_is_nvruim_profile(requesting_id)) ||
        (requesting_id == DATA_SESSION_PROFILE_ID_INVALID &&
         ds707_data_session_is_nvruim_profile(current_id)) )
    )
  {  
    ret_val = TRUE;
  }

  return ret_val;
}

/*===========================================================================
FUNCTION       DS707_DATA_SESSION_GET_NUM_VALID_PROFILES_IN_NVRUIM

DESCRIPTION    Returns number of valid profiles in NVRUIM

PARAMETERS     None.

DEPENDENCIES   None

RETURN VALUE   No. of valid profiles in NVRUIM.

SIDE EFFECTS   None
===========================================================================*/
uint8 ds707_data_session_get_num_valid_profiles_in_nvruim
( 
  void
)
{
  DATA_IS707_MSG1(MSG_LEGACY_HIGH, "Number of valid profiles = %d",
                  num_valid_profiles);
  return num_valid_profiles;
}

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*===========================================================================
FUNCTION      DS707_DATA_SESSION_FREE_NON_PRIMARY_SUBS_PROFILE

DESCRIPTION   Frees the memory for profiles on non-default subscriptions. 
              This is triggered by profile refresh event sent by profile DB. 

PARAMETERS    subs_id - subscription ID on which modem refresh happens
 
DEPENDENCIES  None

RETURN VALUE  TRUE   - If successful
              FALSE  - otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_session_profile_free_non_primary_subs_profile
(
  ds_profile_subs_id_enum_type subs_id
)
{
  ds707_data_session_profile_info_type **ds707_data_session_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (subs_id == ACTIVE_SUBSCRIPTION_2)
  {
    ds707_data_session_profile_ptr = 
      &ds707_data_session_profile_info_secondary_subs;
  }
  else if (subs_id == ACTIVE_SUBSCRIPTION_3)
  {
    ds707_data_session_profile_ptr = 
      &ds707_data_session_profile_info_tertiary_subs;
  }
  else
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, 
                    "Invalid subs id %d passed to "
                    "ds707_data_session_profile_free_non_default_subs_profile",
                    subs_id);
    return FALSE;
  }

  if (*ds707_data_session_profile_ptr != NULL)
  {
    modem_mem_free( (void*)(*ds707_data_session_profile_ptr),
                    MODEM_MEM_CLIENT_DATA_CRIT );

    *ds707_data_session_profile_ptr = NULL;

    ds707_data_session_max_profiles[subs_id - ACTIVE_SUBSCRIPTION_1] = 0;
  }

  return TRUE;

}
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_GET_FIRST_EHRPD_PROFILE_WITH_NULL_APN

DESCRIPTION   Search for the first eHRPD profile with a non-NULL APN 
              and return its profile ID.
 
PARAMETERS    None
 
DEPENDENCIES  None

RETURN VALUE  First eHRPD profile ID with a non-NULL APN. If no such 
              profile is found, return -1. 

SIDE EFFECTS  None
===========================================================================*/
ds707_data_session_profile_id ds707_data_session_get_first_ehrpd_profile_with_non_null_apn
(
  acl_policy_info_type *policy_info_ptr
)
{
  ds707_data_session_profile_id profile_reg_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  for ( profile_reg_index = DATA_SESSION_PROFILE_ID_MIN;
        profile_reg_index < ds707_data_session_max_profiles[SYS_MODEM_AS_ID_1];
        profile_reg_index++)
  {
    if ( (PROFILE_EHRPD & 
            ds707_data_session_profile_info[profile_reg_index].profile_type) &&
         (ds707_data_session_profile_info[profile_reg_index].apn_string_len > 0) &&
         (TRUE == ds707_data_session_policy_and_profile_match(profile_reg_index, 
                                                              policy_info_ptr))
       )
    {
      return ds707_data_session_profile_info[profile_reg_index].data_session_profile_id;
    }
  }

  return DATA_SESSION_PROFILE_ID_INVALID;

}/* ds707_data_session_get_first_ehrpd_profile_with_non_null_apn */

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_GET_PROFILE_FIRST_CALL_ALWAYS_SHARE_PROFILE

DESCRIPTION   Determines the profile ID to use if ALWAYS_SHARE_PROFILE is 
              used for first call bring up 

PARAMETERS    None
 
DEPENDENCIES  None

RETURN VALUE  In 1x/HRPD system, return OMH default profile if OMH is 
              supported, else return first 1x/HRPD profile 0.
              Otherwise, return first eHRPD profile with non NULL APN.

SIDE EFFECTS  None
===========================================================================*/
ds707_data_session_profile_id ds707_data_session_get_profile_for_always_share_profile
(
  acl_policy_info_type *policy_info_ptr
)
{
  if ( ( (ds707_pkt_mgr_get_current_sys_mode() == SYS_SYS_MODE_HDR) &&
         (ds3gdsdif_ehprd_is_pref_system(SYS_MODEM_AS_ID_1) == FALSE) ) ||
       ( ds707_pkt_mgr_get_current_sys_mode() == SYS_SYS_MODE_CDMA ) )
  {
    return ds707_data_session_get_default_profile();
  }
  else
  {
    return ds707_data_session_get_first_ehrpd_profile_with_non_null_apn(
             policy_info_ptr);
  }
}/* ds707_data_session_get_profile_for_always_share_profile */

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_GET_PROFILE_SUBS_ID

DESCRIPTION  This function returns the internal ds707 data session profile 
             subscription ID to the id from CM.
 
PARAMETERS    as_id   - Subscription ID from CM

DEPENDENCIES  None

RETURN VALUE  ds_profile_subs_id_enum_type

SIDE EFFECTS  None
===========================================================================*/
ds_profile_subs_id_enum_type ds707_data_session_get_profile_subs_id
(
  sys_modem_as_id_e_type as_id
)
{
  ds_profile_subs_id_enum_type  ds_profile_subs_id = ACTIVE_SUBSCRIPTION_MAX;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  switch(as_id) 
  {
    case SYS_MODEM_AS_ID_1:
      ds_profile_subs_id = ACTIVE_SUBSCRIPTION_1;
      break;

    case SYS_MODEM_AS_ID_2:
      ds_profile_subs_id =  ACTIVE_SUBSCRIPTION_2;
      break;

#ifdef FEATURE_TRIPLE_SIM
    case SYS_MODEM_AS_ID_3:
     ds_profile_subs_id =  ACTIVE_SUBSCRIPTION_3;
     break;
#endif

    default:
      break;
  }
  return ds_profile_subs_id;
} /* ds707_data_session_get_profile_subs_id */

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_IS_CLAT_ENABLED

DESCRIPTION   This function retrieves the "clat_enabled" flag specified for 
              a profile.

PARAMETERS      profile_number  : profile number 
                subs_id :Subscription id.
                clat_enabled : pointer to store clat_enabled
 
DEPENDENCIES  None

RETURN VALUE  TRUE:  Operation Successful
              FALSE: Operation Failed

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_data_session_is_clat_enabled
(
  ds707_data_session_profile_id  profile_id,
  ds_profile_subs_id_enum_type   subs_id,
  boolean                       *clat_enabled
)
{
  boolean                                ret_val = FALSE;
  ds707_data_session_profile_info_type  *data_sess_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( clat_enabled == NULL )
  {
    return FALSE;
  }

  if( ds707_data_session_get_profile(profile_id,
                                     subs_id,
                                     &data_sess_profile_ptr) == TRUE )
  {
    *clat_enabled = data_sess_profile_ptr->clat_enabled;
    ret_val = TRUE;
  }

  return ret_val;
} /* ds707_data_session_is_clat_enabled */

#endif /* FEATURE_DATA_IS707 */
