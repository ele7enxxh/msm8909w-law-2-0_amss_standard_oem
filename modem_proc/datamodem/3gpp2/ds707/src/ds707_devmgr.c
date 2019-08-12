/** 
  @file ds707_devmgr.c 
  @brief 
   Handles device manager
*/
/** Contains API's to handle device manager */

/*===========================================================================

                         D S 7 0 7 _ D E V M G R
GENERAL DESCRIPTION
  This file contains the device manager functionality, this handles the LCP
  & AUTH incase of eHRPD 

EXTERNALIZED FUNCTIONS

INTERNAL FUNCTIONS 

 Copyright (c) 2011 - 2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_devmgr.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when       who     what, where, why
--------   ---     ------------------------------------------------------- 
10/21/14   sc      Added code to perform graceful PPP termination during hard
                   failure and then update eHRPD availability  
06/30/14   niv     Added wrapper functions & API change for 
                   NV_REFRESH 
05/28/14   vm      Changes to ensure system info is cleared when device manager
                   is aborted in CLOSED state
01/06/14   vm      Changes to use correct PCMT value during IRAT success and
                   failure
08/13/12   ss      Removed ds_create_efs_item_file usage. Listed the file
                   in ds_init_efs_config_file instead.
06/19/12   jee     To do graceful teardown when card REFRESH event happens
04/18/12   ash     Added device manager persistent control block.
03/08/12   jee     Propagate call end reasons correctly
03/07/12   jz      LCP should NOT be gracefully teminated when in dormant 
                   for JCDMA in eHRPD call 
02/24/12   ash     Count PPP failures towards fallback once per call.
01/26/12   ash     Reset Hard/Soft failure counts upon AUTH Success.
01/16/12   jee     Setting the default PCMT values to 0
01/03/12   jee     Fix to set PCMT value to iRAT when LTE attach happens
11/28/11   jee     Fix to stay in partial context after handoff to LTE
11/28/11   jee     Fix to handle PPP fail reason
11/28/11   jz      support ppp control packet on dos
11/18/11   jee     Simplify devmgr_stop() implementation
11/02/11   jee     To fix graceful termination issue
10/25/11   jee     To fix DCTM issue
10/15/11   jee     Added support to abort devmgr
10/04/11   jee     To handle device manager when hot swap happens
09/29/11   ssh     Test instrumentation
10/04/11   vpk     Gracefully close PPP 
09/30/11   ash     Don't set fail_reason as TIMEOUT for LCP termination
09/20/11   jee     Fix to handle new call when device manager is cleaning up
09/14/11   ash     Cancel PPP setup timer after AUTH success
09/08/11   ash     Added support for eHRPD to HRPD fallback
08/16/11   jee     To make go to partial context in OoS state 
04/18/11   jee     Created Module
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#if defined(T_WINNT)
#error code not present
#endif /* WINNT */
#include "amssassert.h"
#include "ds707_devmgr.h"
#include "ds3gtimer.h"
#include "dsutil.h"
#include "data_msg.h"
#include "ds707_pdn_context.h"
#include "ds707_sys_chg_hdlr.h"
#include "ds707_roaming.h"
#include "ds707_pkt_mgr.h"
#include "dstaski.h"
#include "ps_iface_defs.h"
#include "rex.h"
#include "ds707_data_session_profile.h"
#include "sys.h"
#include "ds_3gpp2_vendor.h"
#include "ds3gmgr.h"
#include "ds3gcfgmgr.h"
#ifdef FEATURE_EHRPD_HRPD_FALLBACK
#include "hdrpac.h"
#endif /* FEATURE_EHRPD_HRPD_FALLBACK */
#ifdef FEATURE_DATA_OPTHO
  #include "ds707_s101.h"
#endif /* FEATURE_DATA_OPTHO */

#ifdef FEATURE_DATA_IS707
#include "dsdctm.h"
#endif /* FEATURE_DATA_IS707 */

#ifdef FEATURE_DATA_PKT_CLASSIFIER
  #include "ds707_pkt_classifier.h"
#endif /* FEATURE_DATA_PKT_CLASSIFIER */


#ifdef FEATURE_EHRPD
/*===========================================================================
                LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/
/** Max PPP setup timeout value for eHRPD */
#define DS707_DEVMGR_MAX_PPP_SETUP_TIMEOUT  40

/** Default values for PCMT eHRPD & iRAT timers */
#define DS707_DEVMGR_DEFAULT_PCMT_VAL_EHRPD  0
#define DS707_DEVMGR_DEFAULT_PCMT_VAL_IRAT   0

#ifdef FEATURE_DATA_SEND_PPP_CTL_ON_DOS
#define PPP_ON_DOS_EFS_PATH       "/nv/item_files/data/3gpp2/ds_ppp_ctl_packet_on_dos"
#endif 

#ifdef FEATURE_EHRPD_HRPD_FALLBACK
/** for ehrpd to hrpd fallback, explicit AUTH failures are considered
  hard failure any other LCP/AUTH failure is considered soft 
  failure. */
typedef enum {
  DS707_DEVMGR_PPP_SOFT_FAILURE = 0,
  DS707_DEVMGR_PPP_HARD_FAILURE = 1
}ds707_devmgr_ppp_failure_type;
#endif /* FEATURE_EHRPD_HRPD_FALLBACK */

/** Device manager persistent control block. */
static ds707_devmgr_persist_cb_type ds707_devmgr_persist_cb;

/** Forward declaration */
typedef struct ds707_devmgr_cb_type ds707_devmgr_cb_type_f;

/** Device manager control block info
*/
typedef struct
{
  ppp_dev_enum_type        device;       /**<PPP device       */
  ds707_devmgr_state_type  state;        /**<Device manager state */
  void                *ppp_event_handle; /**<PPP event handle */
  uint32               pcmt_timeout;     /**<PCMT timeout     */
  ds707_devmgr_partial_ctxt_cfg_type efs_config_info; /**<EFS item values */
  ds707_data_session_profile_info_type *data_session_profile_ptr; 
                                         /**<Data session profile info ptr*/
  ds707_devmgr_cb_type_f *this_ptr;      /**<This pointer*/ 
  ps_phys_link_type *ps_phys_link_ptr;  /**<physical link   */
  boolean                 is_ppp_failure_processed; 
                                         /**<PPP failure processed by devmgr */
} ds707_devmgr_cb_type;

/** Device manager control block has only one instance, in
 *  future we might need multiple instances */
static ds707_devmgr_cb_type ds707_devmgr_cb;

static void ds707_devmgr_pcm_timer_cb
(
  unsigned long    timer_id
);

static boolean ds707_devmgr_transition_state
(
  ds707_devmgr_state_type state
);

static int ds707_devmgr_ppp_event_register
(
  void
);

static void ds707_devmgr_handle_ppp_ev_cback
(
  ppp_dev_enum_type      ppp_dev,
  ppp_protocol_e_type    protocol,
  ppp_event_e_type       ppp_event,
  void                  *user_data,
  uint32                 session_handle,
  ppp_fail_reason_e_type ppp_fail_reason
);

static boolean ds707_devmgr_pcm_timer_start
(
  void
);

static void ds707_devmgr_pcm_timer_stop
(
  void
);

static int ds707_devmgr_device_configure
(
  void
);

static boolean ds707_devmgr_device_reconfig
(
  void
);

/*===========================================================================
                      FUNCTION DEFINITIONS
===========================================================================*/

#ifdef FEATURE_EHRPD_HRPD_FALLBACK
/*===========================================================================
FUNCTION DS707_DEVMGR_HANDLE_PPP_FAILURE
===========================================================================*/
/** 
  This Function will handle PPP (LCP/AUTH) failure and inform HDR to switch
  HDR personality to HRPD upon max HARD/SOFT failures are encountered.
 
  @param
    fail types:
       DS707_DEVMGR_PPP_SOFT FAILURE - indicates soft ppp failure. 
       DS707_DEVMGR_PPP_HARD_FAILURE - indicates hard ppp falure.
  
  @dependencies
    None

  @return
   FAIL (-1) on error 
   TRUE if Maximum failures reached
   FALSE otherwise
*/
int8 ds707_devmgr_handle_ppp_failure
(
  ds707_devmgr_ppp_failure_type fail_type /* Hard/Soft failure */
)
{
  ds707_devmgr_ppp_failure_counts_type *ppp_failure_counts_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( TRUE == ds707_is_ehrpd_to_hrpd_fb_enabled() 
/* 
 * In tunnel mode, we do not want to trigger PPP failure for fallback to HRPD Feature
 */
#ifdef FEATURE_DATA_OPTHO
    && ( ds707_s101_query_sm_state() == DS707_S101_SM_NON_TUNNEL )
#endif /* FEATURE_DATA_OPTHO */
     )
  {
    DATA_IS707_MSG3(MSG_LEGACY_MED, "PPP failure with failure type = %d. "
                    "Max PPP hard failure %d, soft failure %d", 
                    fail_type, ds707_get_max_ppp_hard_failures(), 
                    ds707_get_max_ppp_soft_failures()); 

    /*---------------------------------------------------------------------- 
       Initialize ppp failure control block pointer;
    ----------------------------------------------------------------------*/
    ppp_failure_counts_ptr = &ds707_devmgr_persist_cb.ppp_failure_counts;

    switch (fail_type)
    {
      case DS707_DEVMGR_PPP_HARD_FAILURE:
        /*---------------------------------------------------------------------- 
          if hard failure is configured then count this as hard failure
          else count it as soft failure.
        ----------------------------------------------------------------------*/
        if (ds707_get_max_ppp_hard_failures() > 0)
        {
          ppp_failure_counts_ptr->hard_failure_count++;
        }
        else if (ds707_get_max_ppp_soft_failures() > 0)
        {
          ppp_failure_counts_ptr->soft_failure_count++;
        }
        else
        {
          return -1;
        }
        break;

      case DS707_DEVMGR_PPP_SOFT_FAILURE:
        /*---------------------------------------------------------------------- 
          Increase soft failure count based on configuration.
        ----------------------------------------------------------------------*/
        if(ds707_get_max_ppp_soft_failures() > 0)
        {
          ppp_failure_counts_ptr->soft_failure_count++;
        }
        else
        {
          return -1;
        }
        break;

      default:
        /* Invalid ppp failure type */
        return -1;
        break;
    }

    if ( ds707_get_max_ppp_soft_failures() > 0 && 
         ( ppp_failure_counts_ptr->soft_failure_count == 
             ds707_get_max_ppp_soft_failures() ) )
    {  
      DATA_IS707_MSG0(MSG_LEGACY_MED, 
                      "Max Soft PPP failure reached."); 
  
      ds707_set_ppp_failure_flag(TRUE);
      ppp_failure_counts_ptr->hard_failure_count = 0;
      ppp_failure_counts_ptr->soft_failure_count = 0;
      ppp_failure_counts_ptr->max_ppp_failure_reached   = TRUE;

      /*---------------------------------------------------------------------- 
        Inform HDR to switch personality to HRPD.
      ----------------------------------------------------------------------*/
      hdrpac_set_data_ehrpd_capability (FALSE);
      return TRUE;
    }

    if ( ds707_get_max_ppp_hard_failures() > 0 &&
         ( ppp_failure_counts_ptr->hard_failure_count == 
             ds707_get_max_ppp_hard_failures() ) )
    {  
      DATA_IS707_MSG0(MSG_LEGACY_MED, 
                      "Max Hard PPP failure reached."
                      "Cleanup all the calls & fallback to HRPD");       
      return TRUE;
    }
  }

  return FALSE;
} /* ds707_devmgr_handle_ppp_failure() */
#endif /* FEATURE_EHRPD_HRPD_FALLBACK */

/*===========================================================================
FUNCTION DS707_DEVMGR_POWERUP_INIT
===========================================================================*/
/**
  Device manager powerup init

  @param
  None

  @return 
  None
	
  @dependencies 
  None
*/
void ds707_devmgr_powerup_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------- 
   Initialize persistant control block;
  ---------------------------------------------------------------------*/
  memset (&ds707_devmgr_persist_cb, 0, sizeof(ds707_devmgr_persist_cb_type));

  /** Register PCMT */
  ds3gi_timer_register(&ds707_devmgr_persist_cb.pcmt_hdl,
                       ds707_devmgr_pcm_timer_cb,
                       DS3G_TIMER_PCM,
                       DS3G_TIMER_TYPE_REX_TIMER);

} /* ds707_devmgr_powerup_init() */

/*===========================================================================
FUNCTION DS707_DEVMGR_START
===========================================================================*/
/**
  Configures the PPP device, registers LCP & AUTH events

  @param device                     [in] PPP device
  @param data_sess_profile_info_ptr [in] Data session profile info pointer

  @return 
  0  -- Success \n
  -1 -- Failure \n
	
  @dependencies 
  This must be called once dssnet wants to bringup the device manager
*/
int	ds707_devmgr_start
(
  ppp_dev_enum_type                     device,
  ds707_data_session_profile_info_type *data_sess_profile_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_IS707_MSG2(MSG_LEGACY_MED, "Devmgr PPP start, dev %d, state %d", 
                  device, ds707_devmgr_cb.state);

  /** Sanity check, Device should be Um only */
  if (PPP_UM_SN_DEV != device)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Device should be Um");
    return -1;
  }

  if (NULL == data_sess_profile_info_ptr)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "data_sess_profile_info_ptr is NULL");
    return -1;
  }

  /** Handle based on device manager state */
  switch (ds707_devmgr_cb.state)
  {
    case DEVMGR_CLOSED_STATE:
      /** store the info passed from dssnet */
      ds707_devmgr_cb.device = device;
      ds707_devmgr_cb.data_session_profile_ptr = data_sess_profile_info_ptr;

      ds707_devmgr_cb.this_ptr = (ds707_devmgr_cb_type_f *)&ds707_devmgr_cb;

      /** Move to CONFIG state */
      if (TRUE != ds707_devmgr_transition_state(DEVMGR_CONFIG_STATE))
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Device configure/reg failed");
        /** Set this_ptr to NULL */
        ds707_devmgr_cb.this_ptr = NULL;
        return -1;
      }
      break;

    case DEVMGR_LINGER_STATE:
      (void)ds707_devmgr_transition_state(DEVMGR_UP_STATE);
      break;

    case DEVMGR_CLOSING_STATE:
      /** store the info passed from dssnet */
      ds707_devmgr_cb.device = device;
      ds707_devmgr_cb.data_session_profile_ptr = data_sess_profile_info_ptr;

      /** Reconfiure the PPP device */
      if (FALSE == ds707_devmgr_device_reconfig())
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, "PPP device reconfigure failed");
        return -1;
      }

      /** Move to RESTART state for new ppp_start */
      if (FALSE == ds707_devmgr_transition_state(DEVMGR_RESTART_STATE))
      {
        DATA_ERR_FATAL("Transition to restart state failed");
        return -1;
      }
      break;

    case DEVMGR_CONFIG_STATE:
    case DEVMGR_UP_STATE:
    case DEVMGR_RESTART_STATE:
    default:
      /** Check if PPP is configured to avoid race conditions */
      if(FALSE == ppp_is_device_configured(ds707_devmgr_cb.device))
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Device not configured");
        return -1;
      }

      DATA_IS707_MSG1(MSG_LEGACY_MED, 
                      "Ignoring Devmgr PPP start, devmgr state %d",
                      ds707_devmgr_cb.state);
      return 0;
  }

  return 0;
} /* ds707_devmgr_start() */

/*===========================================================================
FUNCTION DS707_DEVMGR_CONFIGURE
===========================================================================*/
/**
  Configures the PPP device, registers LCP & AUTH events

  @param
  None

  @return 
  0  -- Success \n
  -1 -- Failure \n
	
  @dependencies 
  This must be called once dssnet wants to bringup the device manager
*/
static int ds707_devmgr_configure
(
  void
)
{
  int retval;
  char item_file_path[] = "/nv/item_files/data/3gpp2/ehrpd_partial_context";
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /** Config & reg is done only once */
  if(FALSE == ppp_is_device_configured(ds707_devmgr_cb.device))
  {
    if (0 != ds707_devmgr_device_configure())
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Device configure failed");
      return -1;
    }
  
    if (0 != ds707_devmgr_ppp_event_register())
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                      "LCP/AUTH event reg failed, reset PPP device");
      if (0 != ppp_device_reset(ds707_devmgr_cb.device))
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ppp_device_reset() failed");
      }
      return -1;
    }
  
    ds_3gpp2_vendor_init();
  }
    
  /** This is to handle the below cases when in REFRESH state
   *  1)PCMT timer values are changed. or 2) When card
   *  refresh/LPM/PWROFF happens & the partial context
   *  for the current call is disabled */
    /** Read the PCM timer value from the EFS item */
    retval = ds3gcfgmgr_read_efs_nv_ex(item_file_path, 
                            &ds707_devmgr_cb.efs_config_info, 
                            sizeof(ds707_devmgr_cb.efs_config_info), ds707_curr_subs_id()); 
    if (0 != retval)
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR,
          "EFS item not configured for partial context, using default values");

      /** Set timer default timer values */
      ds707_devmgr_cb.efs_config_info.pcmt_val_ehrpd 
                                        = DS707_DEVMGR_DEFAULT_PCMT_VAL_EHRPD;
      ds707_devmgr_cb.efs_config_info.pcmt_val_irat 
                                        = DS707_DEVMGR_DEFAULT_PCMT_VAL_IRAT;
    }
  
    DATA_IS707_MSG2(MSG_LEGACY_MED,
                    "EFS PCMT values, eHRPD %d, iRAT %d", 
                    ds707_devmgr_cb.efs_config_info.pcmt_val_ehrpd,
                    ds707_devmgr_cb.efs_config_info.pcmt_val_irat);
  
    /** Initially set the PCMT working value to eHRPD standalone
     *  PCMT value */
    ds707_devmgr_cb.pcmt_timeout 
                            = ds707_devmgr_cb.efs_config_info.pcmt_val_ehrpd;

  return 0;
} /*ds707_devmgr_configure() */

/*===========================================================================
FUNCTION DS707_DEVMGR_SET_PCMT_TO_EHRPD_VAL
===========================================================================*/
/**
  Re-configures the PCM timeout value to eHRPD standalone value.

  @param
  None

  @return 
  None
	
  @dependencies 
  None
*/
void ds707_devmgr_set_pcmt_to_ehrpd_val
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (DEVMGR_CLOSED_STATE != ds707_devmgr_cb.state && 
      DEVMGR_CLOSING_STATE != ds707_devmgr_cb.state)
  {
    /** Set the PCMT working value to eHRPD standalone PCMT value. */
    ds707_devmgr_cb.pcmt_timeout 
                        = ds707_devmgr_cb.efs_config_info.pcmt_val_ehrpd;
    DATA_3GMGR_MSG2(MSG_LEGACY_MED, "PCM timeout set to PCMT eHRPD value %d."
                    "Devmgr state is %d", ds707_devmgr_cb.pcmt_timeout, 
                    ds707_devmgr_cb.state);
  }

  return;
}

/*===========================================================================
FUNCTION      DS707_DEVMGR_DEVICE_CONFIGURE
===========================================================================*/
/**
  Configure the common PPP Parameters

  @param
  None

  @return 
  0  -- Success \n
  -1 -- Failure \n

  @dependencies 
  None
*/
static int ds707_devmgr_device_configure
(
  void
)
{
  ppp_dev_config_type  ppp_dev_config;
#ifdef FEATURE_DATA_SEND_PPP_CTL_ON_DOS
  nv_stat_enum_type    read_status = NV_DONE_S;
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == ds707_devmgr_cb.data_session_profile_ptr)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "Data session profile info pointer is NULL");
    return -1;
  }

  /** Check if the profile type is eHRPD */
  if (!(ds707_devmgr_cb.data_session_profile_ptr->profile_type & PROFILE_EHRPD))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR, "Invalid profile type %d", 
                    ds707_devmgr_cb.data_session_profile_ptr->profile_type);
    return -1;
  }

  memset(&ppp_dev_config, 0, sizeof(ppp_dev_config_type));

  ppp_dev_config.lcp_mode = PPP_CP_ACTIVE;
  ppp_dev_config.hdlc_mode = HDLC_HW_PREF;
  ppp_dev_config.auth_role = PPP_AUTH_ROLE_AUTHENTICATEE;
  ppp_dev_config.auth_proto_mask = PPP_AUTH_EAP;

#ifdef FEATURE_DATA_PKT_CLASSIFIER
  if (ds707_is_pkt_classifier_enabled()) 
  {
    ppp_dev_config.auth_role = PPP_AUTH_ROLE_NONE;
    ppp_dev_config.auth_proto_mask = PPP_AUTH_DEFAULT;
  }
#endif /* FEATURE_DATA_PKT_CLASSIFIER */

  /** For default profile or ehrpd profile, if mode of operation
    is ehrpd then read eap paramters else read from NV. */
  DATA_3GMGR_MSG1(MSG_LEGACY_MED, "Retrieving auth info for profile id %d",
                  ds707_devmgr_cb.data_session_profile_ptr->data_session_profile_id);
  is707_get_ppp_auth_info_from_nv( &(ppp_dev_config.auth_info), 
                                     ds3gcfgmgr_read_legacy_nv_ex );

  /** Setting max ppp setup time to 40 sec for eHRPD. This is need
    because EAP doesn't have any retry mechanism. So in case there 
    is no response from the network during auth phase then this 
    timer will termiate the PPP session on the AT side. */ 
  ppp_dev_config.setup_final_phase = pppAUTHENTICATE;
  ppp_dev_config.setup_timeout = DS707_DEVMGR_MAX_PPP_SETUP_TIMEOUT;

  ppp_dev_config.tx_f_ptr = ds707_pkt_tx_um_SN_data;
  ppp_dev_config.tx_user_data = NULL;
  ppp_dev_config.rx_signal = PS_PPP_UM_SN_RX_SIGNAL;
  ppp_dev_config.rx_sig_f_ptr = ds707_rx_UM_SN_sig_hdlr;
  ppp_dev_config.bridge_dev = PPP_INVALID_DEV;

  /** If the UE is running in eHRPD mode and user_id is set to
    zero, then there was something wrong with the eHRPD NV item. 
    In case of error, do not proceed further. Terminate the call 
    gracefully for embedded IPv4 call. Other callers should also 
    check and make sure to gracefully end the call incase of 
    failure. */
  if (ppp_dev_config.auth_info.eap_user_id_len == 0) 
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "eHRPD failed, do not proceed");
    return -1;
  }
#ifdef FEATURE_DATA_SEND_PPP_CTL_ON_DOS
  /*-----------------------------------------------------------------------
    get whether ppp control packect on DoS is enabled or not
  -----------------------------------------------------------------------*/
  read_status = ds3gcfgmgr_read_efs_nv_ex(PPP_ON_DOS_EFS_PATH, 
                               &ppp_dev_config.dos_is_enabled, 
                               sizeof(boolean), ds707_curr_subs_id());
  if(read_status != NV_DONE_S)
  {
    DATA_IS707_MSG0( MSG_LEGACY_MED, 
              "PPP on DoS: no efs parameter read, set to default value." );
    ppp_dev_config.dos_is_enabled = FALSE;
  }
#endif /*FEATURE_DATA_SEND_PPP_CTL_ON_DOS*/
  if (0 != ppp_device_configure(ds707_devmgr_cb.device, &ppp_dev_config))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "PPP Configure Device Failed, do not proceed");
    return -1;
  }

  return 0;
}/*ds707_devmgr_device_configure() */

/*===========================================================================
FUNCTION DS707_DEVMGR_PPP_EVENT_REGISTER
===========================================================================*/
/**
  Register for LCP & AUTH events

  @param
  None

  @return 
  0  -- Success \n
  -1 -- Failure \n

  @dependencies 
  None
*/
static int ds707_devmgr_ppp_event_register
(
  void
)
{
  ppp_event_alloc_info_type  ppp_event_alloc_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&ppp_event_alloc_info, 0, sizeof(ppp_event_alloc_info_type));

  ppp_event_alloc_info.lcp_cb_f = ds707_devmgr_handle_ppp_ev_cback;
  ppp_event_alloc_info.auth_cb_f = ds707_devmgr_handle_ppp_ev_cback;
  
  /** Currently we don't use user_data, but in future it
   *  might be used */
  ppp_event_alloc_info.lcp_user_data = (void *)(&ds707_devmgr_cb);
  ppp_event_alloc_info.auth_user_data = (void *)(&ds707_devmgr_cb);
  
  ds707_devmgr_cb.ppp_event_handle 
          = ppp_event_alloc_ex(ds707_devmgr_cb.device, &ppp_event_alloc_info);
  if(NULL == ds707_devmgr_cb.ppp_event_handle)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Unable to acquire a PPP event handle");
    return -1;
  }
  
  /** Register for LCP DOWN/RESYNC events */
  if (0 != ppp_event_register(
                          ds707_devmgr_cb.ppp_event_handle,
                          PPP_PROTOCOL_LCP,
                          (PPP_DOWN_EV_MASK|PPP_RESYNC_EV_MASK)))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Failed to register LCP events");
    return -1;
  }

  /** Register for AUTH SUCESS/FAILURE/AUTH_STARTING events */
  if(0 != ppp_event_register(
              ds707_devmgr_cb.ppp_event_handle,
              PPP_PROTOCOL_AUTH,
              (PPP_SUCCESS_EV_MASK|PPP_FAILURE_EV_MASK|PPP_STARTING_EV_MASK)))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Failed to register AUTH events");
    return -1;
  }

  return 0;
} /* ds707_devmgr_ppp_event_register() */

/*===========================================================================
FUNCTION DS707_DEVMGR_PPP_EVENT_DEREGISTER
===========================================================================*/
/**
  De-register LCP & AUTH events

  @param
  None

  @return 
  TRUE  -- Success \n
  FALSE -- Failure \n

  @dependencies 
  None
*/
static boolean ds707_devmgr_ppp_event_deregister
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(NULL == ds707_devmgr_cb.ppp_event_handle)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid PPP event handle");
    return FALSE;
  }
  
  /** De-Register for LCP DOWN/RESYNC events */
  if(0 != ppp_event_deregister(
                          ds707_devmgr_cb.ppp_event_handle,
                          PPP_PROTOCOL_LCP,
                          (PPP_DOWN_EV_MASK|PPP_RESYNC_EV_MASK)))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Failed to de-register LCP events");
    return FALSE;
  }

  /** De-Register for AUTH SUCESS/FAILURE/AUTH_STARTING events */
  if(0 != ppp_event_deregister(
                    ds707_devmgr_cb.ppp_event_handle,
                    PPP_PROTOCOL_AUTH,
                    (PPP_UP_EV_MASK|PPP_FAILURE_EV_MASK|PPP_STARTING_EV_MASK)))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Failed to de-register AUTH events");
    return FALSE;
  }

  if (0 != ppp_event_dealloc
                (ds707_devmgr_cb.device, &ds707_devmgr_cb.ppp_event_handle))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Failed to de-alloc PPP event handle");
    return FALSE;
  }

  return TRUE;
} /* ds707_devmgr_ppp_event_deregister() */

/*===========================================================================
FUNCTION DS707_DEVMGR_TRANSITION_STATE
===========================================================================*/
/**
  Handles the device manager state transition

  @param state [in] Device manager new state

  @return 
  TRUE  -- Success \n
  FALSE -- Failure \n

  @dependencies 
  None
*/
static boolean ds707_devmgr_transition_state
(
  ds707_devmgr_state_type  state
)
{
  int16 ret;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /** Sanity check */
  if (&ds707_devmgr_cb != (ds707_devmgr_cb_type *)ds707_devmgr_cb.this_ptr)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid this pointer");
    return FALSE;
  }

  /** Change state */
  DATA_IS707_MSG2(MSG_LEGACY_MED, "old state(%d) --> new state(%d)",
                  ds707_devmgr_cb.state, state);
  ds707_devmgr_cb.state = state;

  switch (state)
  {
    case DEVMGR_CONFIG_STATE:
      /** Configure the device & register LCP/AUTH events */
      if (0 != ds707_devmgr_configure())
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "Device config/reg failed, change state to CLOSED state");
        /** Change state to CLOSED state */
        ds707_devmgr_cb.state = DEVMGR_CLOSED_STATE;
        return FALSE;  
      }

      if (NULL == ds707_devmgr_cb.ps_phys_link_ptr)
      {
        /** Grab physical link, required for graceful PPP closing*/
        ds707_devmgr_cb.ps_phys_link_ptr = ds707_pkt_mgr_get_pri_phys_link(); 
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                        "Holding on to physical link");
      }
      else
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Already got physical link");
      }
      
      /** Set the sys info to EPC system */
      ds707_sys_chg_hdlr_chg_sys_info(DS707_SYS_INFO_EPC_SYSTEM);
      break;

    case DEVMGR_UP_STATE:
      /** Stop PCMT */
      ds707_devmgr_pcm_timer_stop();

      /** Set the PCMT working value to eHRPD standalone PCMT */
      ds707_devmgr_cb.pcmt_timeout 
                          = ds707_devmgr_cb.efs_config_info.pcmt_val_ehrpd;

      break;

    case DEVMGR_LINGER_STATE:
      /** Bring down the phys link */
      ds707_pkt_mgr_bring_down_phys_link();

      if (TRUE != ds707_devmgr_pcm_timer_start())
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, "PCMT already running");
      }
      break;

    case DEVMGR_CLOSING_STATE:
    
      /** Do graceful or non-graceful PPP termination based on
       *  whether AT is in eHRPD or iRAT mode & phys link flow is
       *  enabled */
      if ((ds707_pkt_is_ehrpd_mode_of_operation())
          && (TRUE == PS_PHYS_LINK_FLOW_ENABLED(
                                            ds707_devmgr_cb.ps_phys_link_ptr)))
      {
        /** No graceful termination for JCDMA if in dormant */
        if(ds707_roaming_is_curr_mode_jcdma()&&
                 PHYS_LINK_UP != 
                 PS_PHYS_LINK_GET_STATE(ds707_devmgr_cb.ps_phys_link_ptr))
        {
          DATA_IS707_MSG0(MSG_LEGACY_HIGH, "JCDMA mode when PHYS LINK NOT UP,Abort PPP");
          ppp_device_reset(ds707_devmgr_cb.device);
        }
        else
        {
          /** EHRPD, do graceful termination */
          DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Graceful PPP termination");
          ppp_device_stop(ds707_devmgr_cb.device);
        }
      }

      else
      {
        /** iRAT, no graceful termination */
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Abort PPP");
        ppp_device_reset(ds707_devmgr_cb.device);
      }

      break;

    case DEVMGR_CLOSED_STATE:
      /** Required for ehrpd-1x/hrpd handoff. If all ifaces are
        cleaned, move sys_info state to NO_SYSTEM. Next time call is 
        brought up, it will be changed to EPC or NON_EPC depending on 
        the system */

      /** Release physical link */
      if (NULL != ds707_devmgr_cb.ps_phys_link_ptr)
      {
        ret = ds707_pkt_mgr_release_pri_phys_link(
                                             ds707_devmgr_cb.ps_phys_link_ptr);
                    
        DATA_IS707_MSG0(MSG_LEGACY_MED, "Releasing physical link");
        
        ds707_devmgr_cb.ps_phys_link_ptr = NULL;
        if (0 != ret)
        {
          DATA_IS707_MSG1(MSG_LEGACY_ERROR,"Failed to release physical link %d"
                          , ret);
        }
      }
      ds707_sys_chg_hdlr_chg_sys_info(DS707_SYS_INFO_NO_SYSTEM);

      /** Stop PCMT if it is running & move to CLOSED state */
      ds707_devmgr_pcm_timer_stop();

      /** De-register LCP & AUTH events */
      ds707_devmgr_ppp_event_deregister();
      
      /** Vendor cleanup */
      ds_3gpp2_vendor_cleanup();

#ifdef FEATURE_DATA_OPTHO
      ds707_s101_ppp_lcp_ev(FALSE);
#endif /* FEATURE_DATA_OPTHO */

      /** Unconfigure ppp device */
      if (0 != ppp_device_unconfigure(ds707_devmgr_cb.device))
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ppp_device_unconfigure() failed");
      }

      /** Clear Devmgr Control block */
      memset( &ds707_devmgr_cb, 0 , sizeof(ds707_devmgr_cb) );
      break;

    case DEVMGR_RESTART_STATE:
      /** No-op */
      break;

    default:
      return FALSE;
  }

  return TRUE;
} /* ds707_devmgr_transition_state() */

/*===========================================================================
FUNCTION DS707_DEVMGR_DEVICE_RECONFIG
===========================================================================*/
/**
  Reconfigures the PPP device

  @param
  None

  @return 
  TRUE  -- Success \n
  FALSE -- Failure \n

  @dependencies 
  None
*/
static boolean ds707_devmgr_device_reconfig
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /** Another PDN is trying to come up while LCP is tearing
   *  down, Expidiate LCP temination with ppp_device_reset  */
  if (0 != ppp_device_reset(ds707_devmgr_cb.device))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ppp_device_reset() failed");
    return FALSE;
  }

  /** Unconfigure & reconfigure the ppp device, so that AT picks the latest
    config info */
  if (0 != ppp_device_unconfigure(ds707_devmgr_cb.device))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ppp_device_unconfigure() failed");
    return FALSE;
  }

  if (0 != ds707_devmgr_device_configure())
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ds707_devmgr_device_configure() failed");
    return FALSE;
  }

  /** Clear ppp failure flag */
  ds707_devmgr_cb.is_ppp_failure_processed = FALSE;
  return TRUE;
} /* ds707_devmgr_device_reconfig() */

/*===========================================================================
FUNCTION DS707_DEVMGR_HANDLE_LCP_EVENT
===========================================================================*/
/**
  Handle LCP up/down/resync events

  @param cmd_ptr Command pointer

  @return 
  None

  @dependencies 
  Device manager should be configured
*/
static void ds707_devmgr_handle_lcp_event
(
  const ds_cmd_type *cmd_ptr
)
{
#ifdef FEATURE_EHRPD_HRPD_FALLBACK
  ds707_devmgr_ppp_failure_counts_type *ppp_failure_counts_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------- 
   Initialize ppp failure control block pointer;
  ---------------------------------------------------------------------*/
  ppp_failure_counts_ptr = &ds707_devmgr_persist_cb.ppp_failure_counts;
#endif /* FEATURE_EHRPD_HRPD_FALLBACK */

  if ( (NULL == cmd_ptr) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "cmd_ptr is NULL");
    return;
  }

  DATA_IS707_MSG3(MSG_LEGACY_MED, 
        "Handling LCP event %d after task switch, Devmgr state %d, Protocol %d", 
        ((ds707_ppp_ev_cmd_type* ) cmd_ptr->cmd_payload_ptr)->ppp_event, 
                  ds707_devmgr_cb.state, 
        ((ds707_ppp_ev_cmd_type* ) cmd_ptr->cmd_payload_ptr)->protocol);

  /** Callback handles only LCP events */
  if (PPP_PROTOCOL_LCP != ((ds707_ppp_ev_cmd_type* ) cmd_ptr->cmd_payload_ptr)->protocol)
  {
    return;
  }

  /** Device manager should not be in CLOSED state & this_ptr
   *  should be valid */
  if ((DEVMGR_CLOSED_STATE == ds707_devmgr_cb.state) 
      || (&ds707_devmgr_cb != (ds707_devmgr_cb_type *)ds707_devmgr_cb.this_ptr))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "devmgr is in CLOSED state or devmgr this_ptr is NULL");
    return;
  }

  switch (((ds707_ppp_ev_cmd_type* ) cmd_ptr->cmd_payload_ptr)->ppp_event)
  {
    case PPP_RESYNC_EV:
      switch (ds707_devmgr_cb.state)
      {
        /** Stop PCMT if in LINGER state */
        case DEVMGR_LINGER_STATE:
          ds707_devmgr_pcm_timer_stop();
          break;

        case DEVMGR_CLOSED_STATE:
        case DEVMGR_CONFIG_STATE:
        case DEVMGR_UP_STATE:
        case DEVMGR_CLOSING_STATE:
        default:
          DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Ignore LCP RESYNC event");
          break;
      }
      break;

    case PPP_DOWN_EV:
#ifdef FEATURE_EHRPD_HRPD_FALLBACK
      /** PPP DOWN with fail reason indicates LCP level failure. For
          eHRPD to HRPD fallback feature all LCP failures are
          considered SOFT failure */
      if (FALSE == ds707_devmgr_cb.is_ppp_failure_processed)
      {  
        if (PPP_FAIL_REASON_NONE != 
            ((ds707_ppp_ev_cmd_type* ) cmd_ptr->cmd_payload_ptr)->fail_reason)
        {
          ds707_devmgr_handle_ppp_failure( DS707_DEVMGR_PPP_SOFT_FAILURE );
          ds707_devmgr_cb.is_ppp_failure_processed = TRUE;
	}
      }
#endif /* FEATURE_EHRPD_HRPD_FALLBACK */

      switch (ds707_devmgr_cb.state)
      {
        case DEVMGR_RESTART_STATE:
          (void)ds707_devmgr_transition_state(DEVMGR_CONFIG_STATE);
          break;

        case DEVMGR_LINGER_STATE:
        case DEVMGR_CLOSED_STATE:
        case DEVMGR_CONFIG_STATE:
        case DEVMGR_UP_STATE:
        case DEVMGR_CLOSING_STATE:
          (void)ds707_devmgr_transition_state(DEVMGR_CLOSED_STATE);

#ifdef FEATURE_CALL_THROTTLE
          /** Post to DCTM if there is LCP timeout or option mismatch */   
          if((PPP_FAIL_REASON_TIMEOUT == 
              ((ds707_ppp_ev_cmd_type* ) 
               cmd_ptr->cmd_payload_ptr)->fail_reason) 
              || (PPP_FAIL_REASON_OPTION_MISMATCH == 
                  ((ds707_ppp_ev_cmd_type* ) 
                   cmd_ptr->cmd_payload_ptr)->fail_reason))
          {
            dctm_post_ppp_call_status(PPP_DOWN_EV,
                                  DSDCTM_PPP_PROTOCOL_LCP,
                                 ((ds707_ppp_ev_cmd_type* ) 
                                  cmd_ptr->cmd_payload_ptr)->fail_reason);
          }
#endif /* FEATURE_CALL_THROTTLE */

#ifdef FEATURE_EHRPD_HRPD_FALLBACK
          /** Devmgr shall post DCTM success after all Dssnets have posted 
              failures to CM. DCTM success is sent so that CM can clear the
              throttling counters. To accomplish this Devmgr marks 
              max_pp_failure flag upon encountering max SOFT/HARD failure. 
              If this flag is set when devmgr gets PPP_DOWN (LCP),
              devmgr posts dctm success. */ 
          if (TRUE == ppp_failure_counts_ptr->max_ppp_failure_reached)
          {
#ifdef FEATURE_CALL_THROTTLE
            dctm_post_ppp_call_status( PPP_SUCCESS_EV,
                                       DSDCTM_PPP_PROTOCOL_LCP,
                                       PPP_FAIL_REASON_NONE );
    
#endif /* FEATURE_CALL_THROTTLE */
            ppp_failure_counts_ptr->max_ppp_failure_reached = FALSE;
          }
#endif /* FEATURE_EHRPD_HRPD_FALLBACK */

          break;

        default:
          DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Ignore LCP DOWN event");
          break;
      }
      break;

    default:
      return;
  }
  
  return;
} /* ds707_devmgr_handle_lcp_event() */

/*===========================================================================
FUNCTION DS707_DEVMGR_HANDLE_AUTH_EVENT
===========================================================================*/
/**
  Handle AUTH SUCCESS/STARTING events

  @param cmd_ptr Command pointer

  @return 
  None

  @dependencies 
  Device manager should be configured
*/
static void ds707_devmgr_handle_auth_event
(
  const ds_cmd_type *cmd_ptr
)
{
#ifdef FEATURE_EHRPD_HRPD_FALLBACK
ds707_devmgr_ppp_failure_counts_type *ppp_failure_counts_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*---------------------------------------------------------------------- 
   Initialize ppp failure control block pointer;
  ---------------------------------------------------------------------*/
  ppp_failure_counts_ptr = &ds707_devmgr_persist_cb.ppp_failure_counts;
#endif /* FEATURE_EHRPD_HRPD_FALLBACK */

  if (NULL == cmd_ptr)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "cmd_ptr is NULL");
    return;
  }

  DATA_IS707_MSG3(MSG_LEGACY_MED, 
        "Handling Auth event %d after task switch, Devmgr state %d, Protocol %d", 
        ((ds707_ppp_ev_cmd_type* ) cmd_ptr->cmd_payload_ptr)->ppp_event,
                   ds707_devmgr_cb.state, 
        ((ds707_ppp_ev_cmd_type* ) cmd_ptr->cmd_payload_ptr)->protocol);

  /** Callback handles only AUTH events */
  if (PPP_PROTOCOL_AUTH != 
      ((ds707_ppp_ev_cmd_type* ) cmd_ptr->cmd_payload_ptr)->protocol)
  {
    return;
  }

  /** Device manager should not be in CLOSED state & this_ptr
   *  should be valid */
  if ((DEVMGR_CLOSED_STATE == ds707_devmgr_cb.state) 
      || (&ds707_devmgr_cb != (ds707_devmgr_cb_type *)ds707_devmgr_cb.this_ptr))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "devmgr is in CLOSED state or devmgr this_ptr is NULL");
    return;
  }

  switch (((ds707_ppp_ev_cmd_type* ) cmd_ptr->cmd_payload_ptr)->ppp_event)
  {
    case PPP_SUCCESS_EV:

#ifdef FEATURE_EHRPD_HRPD_FALLBACK
      /** Reset Hard/Soft failure counts upon AUTH Success. */
      DATA_IS707_MSG0(MSG_LEGACY_MED, 
                      "Resetting PPP failure counts.");
      memset (ppp_failure_counts_ptr, 
              0, 
              sizeof (ds707_devmgr_ppp_failure_counts_type) );
#endif /* FEATURE_EHRPD_HRPD_FALLBACK */
      switch (ds707_devmgr_cb.state)
      {
        case DEVMGR_CONFIG_STATE:
          /** Move to UP state when first PPP SUCCESS event for AUTH is
           *  received. Even if AUTH is not configured we do receive
           *  PPP_SUCCESS_EV after LCP complete */
          (void)ds707_devmgr_transition_state(DEVMGR_UP_STATE);
          break;

        case DEVMGR_LINGER_STATE:
          /** Stay in LINGER state */
          (void)ds707_devmgr_transition_state(DEVMGR_LINGER_STATE);
          break;

        case DEVMGR_CLOSED_STATE:
          DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid devmgr state");
          return;

        case DEVMGR_UP_STATE:
        case DEVMGR_CLOSING_STATE:
        case DEVMGR_RESTART_STATE:
        default:
          DATA_IS707_MSG0(MSG_LEGACY_MED, "Ignoring AUTH SUCCESS event");
          return;
      }

#ifdef FEATURE_CALL_THROTTLE
      /** Post auth success to DCTM */
      dctm_post_ppp_call_status(PPP_SUCCESS_EV,
                                DSDCTM_PPP_PROTOCOL_AUTH,
                                PPP_FAIL_REASON_NONE);
#endif /* FEATURE_CALL_THROTTLE */

#ifdef FEATURE_DATA_OPTHO
      ds707_s101_ppp_auth_ev(TRUE);
#endif /* FEATURE_DATA_OPTHO */

      break;
    
    case PPP_STARTING_EV:
      /** Call PDN context so that AUTH starting event is posted to
       * each of the PDNs */ 
      ds707_pdncntx_ppp_event_hdl(
         ((ds707_ppp_ev_cmd_type* ) cmd_ptr->cmd_payload_ptr)->protocol,
         ((ds707_ppp_ev_cmd_type* ) cmd_ptr->cmd_payload_ptr)->ppp_event,
         ((ds707_ppp_ev_cmd_type* ) cmd_ptr->cmd_payload_ptr)->fail_reason);
      break;

    case PPP_FAILURE_EV:
#ifdef FEATURE_EHRPD_HRPD_FALLBACK
      if (FALSE == ds707_devmgr_cb.is_ppp_failure_processed)
      {  
        if (PPP_FAIL_REASON_NONE != 
            ((ds707_ppp_ev_cmd_type* ) cmd_ptr->cmd_payload_ptr)->fail_reason)
        {
          /** All AUTH failures are considered as hard failure. */
          ds707_devmgr_handle_ppp_failure ( DS707_DEVMGR_PPP_HARD_FAILURE );
        }
        ds707_devmgr_cb.is_ppp_failure_processed = TRUE;
      }
#endif /* FEATURE_EHRPD_HRPD_FALLBACK */

      /** Auth failure, Move to Closing state */
      (void)ds707_devmgr_transition_state(DEVMGR_CLOSING_STATE);    

#ifdef FEATURE_CALL_THROTTLE
      /** Post AUTH failure to DCTM */
      DATA_IS707_MSG0(MSG_LEGACY_MED,    
          "Posting PPP_FAILURE_EV with DSDCTM_PPP_PROTOCOL_EHRPD_AUTH to DCTM");
      dctm_post_ppp_call_status(PPP_FAILURE_EV, 
                                DSDCTM_PPP_PROTOCOL_EHRPD_AUTH, 
                                PPP_FAIL_REASON_AUTH_FAILURE);
#endif /* FEATURE_CALL_THROTTLE */
      break;

    default:
      return;
  }
  
  return;
} /* ds707_devmgr_handle_auth_event() */

/*===========================================================================
FUNCTION DS707_DEVMGR_HANDLE_PPP_EV_CBACK
===========================================================================*/
/**
  Called to handle LCP/AUTH event callback

  @param ppp_dev         [in] PPP device
  @param protocol        [in] Protocol type
  @param ppp_event       [in] PPP event
  @param user_data       [in] User data passed
  @param session_handle  [in] Session handle
  @param ppp_fail_reason [in] Failure reason

  @return 
  None

  @dependencies 
  Device manager should be configured
*/
static void ds707_devmgr_handle_ppp_ev_cback
(
  ppp_dev_enum_type      ppp_dev,                /* PPP Device             */
  ppp_protocol_e_type    protocol,               /* PPP Protocol           */
  ppp_event_e_type       ppp_event,              /* Down/Up/Resync         */
  void                  *user_data,
  uint32                 session_handle,
  ppp_fail_reason_e_type ppp_fail_reason
)
{
 ds_cmd_type *cmd_ptr = NULL;
 ds707_ppp_ev_cmd_type *ppp_ev_cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_IS707_MSG3(MSG_LEGACY_HIGH, "Rcvd dev %d protocol %d event %d",
                  ppp_dev, protocol, ppp_event);

  /** callback handles only LCP & AUTH events */
  if ((PPP_PROTOCOL_LCP != protocol) && (PPP_PROTOCOL_AUTH != protocol))
  {
    return;
  }

  /** Setup the command ID for required LCP/AUTH events */
  if (PPP_PROTOCOL_LCP == protocol)
  {
    switch (ppp_event)
    {
      case PPP_RESYNC_EV:
      case PPP_DOWN_EV:
        /** Valid LCP event, continue processing */
        break;
  
      default:
        return;
    }
  }

  else
  {
    switch (ppp_event)
    {
      case PPP_SUCCESS_EV:
      case PPP_STARTING_EV:
      case PPP_FAILURE_EV:
        /** Valid AUTH event, continue processing */
        break;
  
      default:
        return;
    }
  }
  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds707_ppp_ev_cmd_type));
  /** Get command buffer to do task switch */
  if( (NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr) )
  {
    ASSERT(0);
    return;
  }
 
  ppp_ev_cmd_ptr = (ds707_ppp_ev_cmd_type* ) cmd_ptr->cmd_payload_ptr;


  /** Copy the values into the command buffer */
  ppp_ev_cmd_ptr->ppp_dev          = ppp_dev;
  ppp_ev_cmd_ptr->protocol         = protocol;
  ppp_ev_cmd_ptr->ppp_event        = ppp_event;
  ppp_ev_cmd_ptr->user_data        = user_data;
  ppp_ev_cmd_ptr->session_handle   = session_handle;
  ppp_ev_cmd_ptr->fail_reason      = ppp_fail_reason;

  cmd_ptr->hdr.cmd_id = DS_CMD_707_DEVMGR_PPP_EV;

  /** Post command to DS */
  ds_put_cmd(cmd_ptr);

  return;
} /* ds707_devmgr_handle_ppp_ev_cback() */

/*===========================================================================
FUNCTION      DS707_DEVMGR_PCM_TIMER_START
===========================================================================*/
/**
  This function starts pcmt timer, if and only if the timer is not running. 
  Calling this function while the timer is running will not have any effect.

  @param
  None

  @return 
  TRUE  -- Success \n
  FALSE -- Failure \n

  @dependencies 
  Timer should not be running for this function to take effect
*/
static boolean ds707_devmgr_pcm_timer_start
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /** Start the PCM timer if it is not running */
  if (FALSE == ds3gi_is_timer_running(DS3G_TIMER_PCM))
  {
    /** Convert the timeout value to msec */
    ds3gi_start_timer(DS3G_TIMER_PCM, 
                      SEC_TO_MSEC(ds707_devmgr_cb.pcmt_timeout));
    DATA_IS707_MSG1(MSG_LEGACY_HIGH, "Started PCM timer for %d ms", 
                    SEC_TO_MSEC(ds707_devmgr_cb.pcmt_timeout));
    return TRUE;
  }

  return FALSE;
} /* ds707_devmgr_pcm_timer_start() */

/*===========================================================================
FUNCTION      DS707_DEVMGR_PCM_TIMER_STOP
===========================================================================*/
/**
  This function stops PCMT

  @param
  None

  @return 
  None

  @dependencies 
  Timer should be running for this function to take effect
*/
static void ds707_devmgr_pcm_timer_stop
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /** Stop the PCM timer */
  ds3gi_stop_timer(DS3G_TIMER_PCM);
} /* ds707_devmgr_pcm_timer_stop() */

/*===========================================================================
FUNCTION      DS707_DEVMGR_PCM_TIMER_CB
===========================================================================*/
/**
  PCM Timer callback function

  @param timer_id [in] ds3g timer ID

  @return 
  None

  @dependencies 
  Calback should be registerd with the timer
*/
static void ds707_devmgr_pcm_timer_cb
(
  unsigned long    timer_id
)
{
  ds_cmd_type           *cmd_ptr;                    /* Pointer to command */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(NULL == (cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO)))
  {
    ASSERT(0);
    return;
  }

  switch ((ds3g_timer_enum_type)timer_id)
  {
    case DS3G_TIMER_PCM:
      /** Post command to DS */
      cmd_ptr->hdr.cmd_id = DS_CMD_707_DEVMGR_PCMT_TIMEOUT;
      break;

    default:
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Bad timer id on callback %d",
                      timer_id);
      return;
  }
  
  ds_put_cmd(cmd_ptr);
} /* ds707_devmgr_pcm_timer_cb() */

/*===========================================================================
FUNCTION      DS707_DEVMGR_PROCESS_CMD
===========================================================================*/
/**
  Process commands posted through DS

  @param cmd_ptr [in] Command pointer

  @return 
  None

  @dependencies 
  None
*/
void ds707_devmgr_process_cmd
(
  const ds_cmd_type *cmd_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == cmd_ptr)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "cmd_ptr is NULL");
    return;    
  }

  DATA_IS707_MSG2(MSG_LEGACY_MED, "process command %d, devmgr state %d",
                  cmd_ptr->hdr.cmd_id, ds707_devmgr_cb.state);

  /** Invoke the appropriate function to process the command,
    based on the command id */
  switch(cmd_ptr->hdr.cmd_id)
  {
    case DS_CMD_707_DEVMGR_PCMT_TIMEOUT:
      if (DEVMGR_LINGER_STATE != ds707_devmgr_cb.state)
      {
        /** Don't assert since PCMT can expire in invalid devmgr state.  
            ex: PCMT timer expires & before DS posts the command, LCP DOWN is 
            received. This sends the devmgr to CLOSED state. Later PCMT timer 
            expires in devmgr CLOSED state */
        return;
      }

      (void)ds707_devmgr_transition_state(DEVMGR_CLOSING_STATE);    
      break;

     case DS_CMD_707_DEVMGR_PPP_EV:
       
      if (PPP_PROTOCOL_LCP == ((ds707_ppp_ev_cmd_type* ) cmd_ptr->cmd_payload_ptr)->protocol)
      {
        /** Handle LCP events */
        ds707_devmgr_handle_lcp_event(cmd_ptr);
      }

      else
      {
        /** Handle AUTH events */
        ds707_devmgr_handle_auth_event(cmd_ptr);  
      }
      break;

    default:
      return;
  }

  return;
} /* ds707_devmgr_process_cmd() */

/*===========================================================================
FUNCTION      DS707_DEVMGR_ALL_PDN_DOWN_IND
===========================================================================*/
/**
  Called by PDN context manager when all the PDNs are down

  @param
  None

  @return 
  None

  @dependencies 
  None
*/
void ds707_devmgr_all_pdn_down_ind
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_IS707_MSG1(MSG_LEGACY_MED, 
        "received all pdn down ind in devmgr state %d", ds707_devmgr_cb.state);

  /** Handle based on device manager state */
  switch (ds707_devmgr_cb.state)
  {
    case DEVMGR_UP_STATE:
      /** Move to LINGER state if the PARTIAL_CONTEXT feature is
       *  enabled, PCMT timeout value is not zero, AT is in EPC system */
#if defined(FEATURE_DATA_PARTIAL_CONTEXT)
      if ((0 != ds707_devmgr_cb.pcmt_timeout) && 
          (DS707_SYS_INFO_EPC_SYSTEM  == 
                                  ds707_sys_chg_hdlr_get_current_system()))
      {
        (void)ds707_devmgr_transition_state(DEVMGR_LINGER_STATE);
        return;
      }
#endif /* FEATURE_PARTIAL_CONTEXT */

      /** Move to Closing state directly */
      (void)ds707_devmgr_transition_state(DEVMGR_CLOSING_STATE);
      break;

    case DEVMGR_CONFIG_STATE:
      /** Move to Closing state directly */
      (void)ds707_devmgr_transition_state(DEVMGR_CLOSING_STATE);
      break;

    case DEVMGR_RESTART_STATE:
      /** Move to Closed state since no PDNs are up. */
      (void)ds707_devmgr_transition_state(DEVMGR_CLOSED_STATE);
      break;

    case DEVMGR_CLOSED_STATE:
    case DEVMGR_LINGER_STATE:
    case DEVMGR_CLOSING_STATE:
    default:
      return;
  }
  
  return;
} /* ds707_devmgr_all_pdn_down_indication() */

/*===========================================================================
FUNCTION      DS707_DEVMGR_EPC_TO_NONEPC_HANDOFF_IND
===========================================================================*/
/**
  EPC to NONEPC handoff indication

  @param
  None

  @return 
  None

  @dependencies 
  None
*/
void ds707_devmgr_epc_to_nonepc_handoff_ind 
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_IS707_MSG1(MSG_LEGACY_MED, 
                  "received EPC_NONEPC handoff ind in devmgr state %d", 
                  ds707_devmgr_cb.state);

  switch (ds707_devmgr_cb.state)
  {
    case DEVMGR_LINGER_STATE:
      /** Stop PCMT */
      ds707_devmgr_pcm_timer_stop();

      /** When handoff happens from EPC to NONEPC, terminate the
       *  partial context */
      (void)ds707_devmgr_transition_state(DEVMGR_CLOSING_STATE);
      break;

    case DEVMGR_CLOSED_STATE:
    case DEVMGR_CONFIG_STATE:
    case DEVMGR_UP_STATE:
    case DEVMGR_CLOSING_STATE:
    case DEVMGR_RESTART_STATE:
    default:
      break;
  }

  return;
} /* ds707_devmgr_epc_to_nonepc_handoff_ind() */

/*===========================================================================
FUNCTION DS707_DEVMGR_ABORT
===========================================================================*/
/**
  Aborts device manager

  @param
  None

  @return 
  0 - Success
  -1 - Failure
	
  @dependencies 
  None
*/
int32 ds707_devmgr_abort 
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_IS707_MSG1(MSG_LEGACY_MED, "devmgr abort in devmgr state %d", 
                  ds707_devmgr_cb.state);

  switch (ds707_devmgr_cb.state)
  {
    case DEVMGR_CONFIG_STATE:
    case DEVMGR_UP_STATE:
    case DEVMGR_LINGER_STATE:
    case DEVMGR_RESTART_STATE:
    case DEVMGR_CLOSING_STATE:
    case DEVMGR_CLOSED_STATE:

      /** Abort PPP */
      DATA_IS707_MSG0(MSG_LEGACY_MED, "Abort PPP");
      ppp_device_reset(ds707_devmgr_cb.device);

      /** Move to closed state */
      (void)ds707_devmgr_transition_state(DEVMGR_CLOSED_STATE);
      break;

    default:
      return -1;
  }

  return 0;
} /* ds707_devmgr_abort() */

/*===========================================================================
FUNCTION DS707_DEVMGR_STOP
===========================================================================*/
/**
  Cleans up device manager (can be either graceful or abort)

  @param
  None

  @return 
  ds707_devmgr_stop_reason - Success, Failure or Pending
	
  @dependencies 
  None
*/
ds707_devmgr_stop_reason ds707_devmgr_stop 
(
  void
)
{
  ds707_devmgr_stop_reason stop_reason = DS707_DEVMGR_STOP_PENDING;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_IS707_MSG1(MSG_LEGACY_MED, "devmgr stop in devmgr state %d", 
                  ds707_devmgr_cb.state);

  switch (ds707_devmgr_cb.state)
  {
    case DEVMGR_CONFIG_STATE:
    case DEVMGR_UP_STATE:
    case DEVMGR_LINGER_STATE:
      /** Stop PCMT */
      ds707_devmgr_pcm_timer_stop();

      /** terminate the partial context */
      (void)ds707_devmgr_transition_state(DEVMGR_CLOSING_STATE);
      break;

    case DEVMGR_RESTART_STATE:
      /** Move to closed state */
      (void)ds707_devmgr_transition_state(DEVMGR_CLOSED_STATE);
      break;

    case DEVMGR_CLOSED_STATE:
      /** Notify that device manger is already in CLOSED state */
      stop_reason = DS707_DEVMGR_STOP_COMPLETE;
      break;

    case DEVMGR_CLOSING_STATE:
    default:
      break;
  }

  return stop_reason;
} /* ds707_devmgr_stop() */

/*===========================================================================
FUNCTION      DS707_DEVMGR_IN_PARTIAL_CTX
===========================================================================*/
/**
  Checks if device manager is in partial context

  @param
  None

  @return 
  TRUE  - devmgr is in partial context
  FALSE - devmgr is not in partial context

  @dependencies 
  None
*/
boolean ds707_devmgr_in_partial_ctx
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (DEVMGR_LINGER_STATE == ds707_devmgr_cb.state)
  {
    return TRUE;
  }

  return FALSE;
} /* ds707_devmgr_in_partial_ctx() */

/*===========================================================================
FUNCTION      DS707_DEVMGR_UP_OR_LINGERING
===========================================================================*/
/**
  Checks if device manager is in UP or partial context (lingering) state

  @param
  None

  @return 
  TRUE  - devmgr is in UP/Lingering state
  FALSE - devmgr is not up

  @dependencies 
  None
*/
boolean ds707_devmgr_up_or_lingering
(
  void
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (DEVMGR_LINGER_STATE == ds707_devmgr_cb.state ||
      DEVMGR_UP_STATE     == ds707_devmgr_cb.state
     )
  {
    return TRUE;
  }

  return FALSE;

} /* ds707_devmgr_up_or_lingering */

/*===========================================================================
FUNCTION      DS707_DEVMGR_LTE_ATTACH_INIT_IND
===========================================================================*/
/**
  Indicates that the AT has initiated attach to LTE

  @param
  None

  @return 
  None

  @dependencies 
  None
*/
void ds707_devmgr_lte_attach_init_ind
(
  void
)
{
  DATA_3GMGR_MSG1(MSG_LEGACY_MED, "LTE attach init ind. Devmgr state is %d", 
                  ds707_devmgr_cb.state);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (DEVMGR_CLOSED_STATE != ds707_devmgr_cb.state && 
      DEVMGR_CLOSING_STATE != ds707_devmgr_cb.state)
  {
    ds707_devmgr_cb.pcmt_timeout 
                        = ds707_devmgr_cb.efs_config_info.pcmt_val_irat;        
    DATA_3GMGR_MSG1(MSG_LEGACY_MED, "PCM timeout set to PCMT IRAT value %d.",
                    ds707_devmgr_cb.pcmt_timeout);
  }

  return;
} /* ds707_devmgr_lte_attach_init_ind() */

/*===========================================================================
FUNCTION      DS707_DEVMGR_DISABLE_PARTIAL_CONTEXT
===========================================================================*/
/**
  Disable partial context for the current call

  @param
  None

  @return 
  None

  @dependencies 
  None
*/
void ds707_devmgr_disable_partial_context
(
  void
)
{
  /** Set PCMT timeout value to 0, so that partial context is
   *  disabled */
  DATA_3GMGR_MSG0(MSG_LEGACY_MED, "PCMT timeout set to 0");
  ds707_devmgr_cb.pcmt_timeout = 0;
} /* ds707_devmgr_disable_partial_context() */

/*===========================================================================
FUNCTION      DS707_DEVMGR_GET_STATE
===========================================================================*/
/**
  Returns device manager state

  @param
  None

  @return 
  state

  @dependencies 
  None
*/
ds707_devmgr_state_type ds707_devmgr_get_state 
(
  void
)
{
  return ds707_devmgr_cb.state;
} /* ds707_devmgr_get_state() */

/*===========================================================================
FUNCTION      DS707_DEVMGR_GET_PPP_FAILURE_COUNTS
===========================================================================*/
/**
  Returns the address of device manager ppp failure count structure

  @param
  None

  @return 
  ds707_devmgr_ppp_failure_counts_type *

  @dependencies 
  None
*/
ds707_devmgr_ppp_failure_counts_type* ds707_devmgr_get_ppp_failure_counts
(
  void
)
{
  return &ds707_devmgr_persist_cb.ppp_failure_counts;
} /* ds707_devmgr_get_persist_cb() */

#endif /* FEATURE_EHRPD */
