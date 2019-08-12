/*===========================================================================

                             D S 3 G U T I L . C
 
GENERAL DESCRIPTION
  Utility functions for DSMgr module

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2008 - 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/src/ds3gutil.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/07/16   ds      Removed logic to read NV 1896 for IPv6 and setting it to default.
07/13/12   jz      Support for emergency only mode when mandatory APN is disabled  
01/25/12   nd      Deprecating API cm_get_pref_srv_type_for_packet_orig usage.
01/02/12   msh     Coan: Feature cleanup
11/28/11   jee     Fix to stay in partial context after handoff to LTE
03/29/11   ttv     Merged changes to ensure that WLAN was selected as pref
                   mode in its presence in DSDS enabled builds.
03/04/11   ttv     Added changes to get the correct sys mdoe incase of dualsim.
11/22/10   ttv     Added support for DSDS.
09/09/10   op      Migrated to MSG 2.0 macros
09/22/09   vrk     Merged LTE related changes.
03/04/09   sa      AU level CMI modifications.
11/26/08   sn      Fixed off-target lint errors.
07/15/08   ar      Initial version.  Partitioned from ds3gmgr.c

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef  FEATURE_DATA_LTE_OFFTARGET_TEST 
#include "dsut.h"
#endif /* FEATURE_DATA_LTE_OFFTARGET_TEST */
#include "cm.h"
#include "ds3gcmif.h"
#include "ds3gcfgmgr.h"
#include "ds3gmgr.h"
#include "ds3gsubsmgr.h"
#include "ds3gmgrint.h"
#include "ds3gmshif.h"
#include "ds3grdmif.h"
#include "ds3gtimer.h"
#include "err.h"
#include "msg.h"
#include "rex.h"
#include "sys.h"
#include "data_msg.h"
#include "ds3gdsdif.h"
#include "dsutil.h"
#include "dstaski.h"
#include "ds3gutil.h"
#ifdef FEATURE_EPC_HANDOFF
#include "ds707_pkt_mgr.h"
#endif
#ifdef FEATURE_EHRPD
#include "ds707_devmgr.h"
#endif

#ifdef FEATURE_DATA_WLAN_MAPCON
  #include "ds_mppm_ext_i.h"
#endif /*FEATURE_DATA_WLAN_MAPCON*/

#include "dsm_pool.h"
#include "ds3gsiolib.h"
#include "ds_3gpp_bearer_context.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
  Structure to store subscription specific IPv6 config info read from EFS/NV
---------------------------------------------------------------------------*/
typedef struct
{
  boolean                         pdsn_as_proxy_ipv6_dns_server;
  ip6_sm_config_type              sm_config;
  struct ps_in6_addr              primary_dns;
  struct ps_in6_addr              secondary_dns;
  ds3g_ipv6_iid_type              iid_info;
}ds3g_ipv6_efs_nv_subs_specific_info;

/*---------------------------------------------------------------------------
  Structure to store IPv6 config info read from EFS/NV
---------------------------------------------------------------------------*/
typedef struct
{
  boolean                               is_enabled;
  ds3g_ipv6_efs_nv_subs_specific_info  *subs_info[DS3GSUBSMGR_SUBS_ID_MAX];
  ps_iface_ip_ver_failover_e_type       failover_mode;
}ds3g_ipv6_efs_nv_config_info;

#ifdef FEATURE_8960_SGLTE_FUSION
/*--------------------------------------------------------------------------
  Structure used to read the timer NV items from the item file in EFS
--------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{ 
  boolean disable_flag;
} disable_dsd_sync_nv_type;
#endif /* FEATURE_8960_SGLTE_FUSION */

/*--------------------------------------------------------------------------
  Structure used to store the DS3G Util EFS item values. Any new 
  DS3G Util EFS item read should be stored here.
--------------------------------------------------------------------------*/
typedef struct
{
  boolean wlan_3g_ip_continuity_enabled;
} ds3g_efs_info_type;

static ds3g_efs_info_type ds3g_efs_info[DS3GSUBSMGR_SUBS_ID_MAX] = {{0}};

#ifdef FEATURE_DATA_PS_IPV6
/*---------------------------------------------------------------------------
  Global variable which is updated from the NV during powerup 
---------------------------------------------------------------------------*/
static ds3g_ipv6_efs_nv_config_info ipv6_nv_efs_info = {NULL} ;
#endif /* FEATURE_DATA_PS_IPV6 */

/*---------------------------------------------------------------------------
  Global variable to store EHRPD_AUTH_IN_USIM NV value
---------------------------------------------------------------------------*/
static boolean ds3g_ehprd_auth_in_usim = FALSE;

#ifdef FEATURE_DATA_MUX_USB_TEST
/*---------------------------------------------------------------------------
  Global variable to store NV 6873 value
---------------------------------------------------------------------------*/
static boolean ds3g_is_mux_nv_set = FALSE;
#endif /* FEATURE_DATA_MUX_USB_TEST */

/*---------------------------------------------------------------------------
  Global variable to store Mode pref NV
---------------------------------------------------------------------------*/
static nv_mode_enum_type ds3g_nv_mode_pref[DS3GSUBSMGR_SUBS_ID_MAX] = {0};

#ifdef FEATURE_8960_SGLTE_FUSION
/*---------------------------------------------------------------------------
   DISABLE DSD SYNC NV item
---------------------------------------------------------------------------*/
static boolean ds3g_disable_dsd_sync = FALSE;
#endif /* FEATURE_8960_SGLTE_FUSION */

#ifdef FEATURE_DATA_SDIO_MUX_A2_DUN
/*---------------------------------------------------------------------------
  FALSE indicates that SDIO A2 MUX port is enabled
  TRUE indicates that Legacy USB port is enabled. 
---------------------------------------------------------------------------*/
static boolean ds3g_is_mdm_tethering_enabled = FALSE;
#endif /* FEATURE_DATA_SDIO_MUX_A2_DUN */

/*---------------------------------------------------------------------------
  Variable used to identify if DUN path is accelerated
---------------------------------------------------------------------------*/
static uint8 ds3g_dun_acc_cfg = 0;

/*---------------------------------------------------------------------------
  Variable used to store refres vote ok NV
---------------------------------------------------------------------------*/
static boolean ds3g_refresh_vote_ok[DS3GSUBSMGR_SUBS_ID_MAX] = {FALSE};

/*------------------------------------------------------------------------- 
  Variable to store default behaviour PAP CHAP
---------------------------------------------------------------------------*/
static uint8 ds3g_config_pdn_level_auth[DS3GSUBSMGR_SUBS_ID_MAX] = {0};
static boolean ds3g_config_latency = TRUE;

#ifdef FEATURE_DATA_IS707
/*---------------------------------------------------------------------------
  Variable to indicate if auth from client or NV should be used
---------------------------------------------------------------------------*/
static boolean ds3g_is_auth_internal = TRUE;
#endif /* FEATURE_DATA_IS707 */

/*---------------------------------------------------------------------------
  Variable to store enable arbitration
---------------------------------------------------------------------------*/
static boolean ds3g_enable_arbitration = FALSE;

/*---------------------------------------------------------------------------
  Variable to store Suppress GSM on SRVCC or CSFB NV
---------------------------------------------------------------------------*/
static boolean ds3g_suppress_gsm_on_srvcc_csfb[DS3GSUBSMGR_SUBS_ID_MAX]
                                                                   = {FALSE};

/*---------------------------------------------------------------------------
  Global variable to store DS3G Common NV items
---------------------------------------------------------------------------*/
static char ds3g_nv_item_file_path[] = 
"/nv/item_files/modem/data/3gpp/lteps/attach_profile\n\
/nv/item_files/data/dsd/ds_apn_switching\n\
/nv/item_files/data/dsd/data_1x_acq_hyst_timer\n\
/nv/item_files/data/dsd/data_1x_oos_hyst_timer\n\
/nv/item_files/data/dsd/data_hdr_acq_hyst_timer\n\
/nv/item_files/data/dsd/data_hdr_oos_hyst_timer\n\
/nv/item_files/data/dsd/data_lte_acq_hyst_timer\n\
/nv/item_files/data/dsd/data_lte_oos_hyst_timer\n\
/nv/item_files/data/dsd/disable_data_dsd_sync\n\
/nv/item_files/data/dsd/data_wlan_throttle_timer\n\
/nv/item_files/data/dsd/data_wlan_acq_hyst_timer\n\
/nv/item_files/modem/data/dsmgr/dual_data_flow_control\n\
/nv/item_files/modem/data/dsmgr/config_pdn_level_auth\n\
/nv/item_files/data/3gpp2/enable_arbitration\n\
/nv/item_files/data/common/ds_ml1_latency_ind\n\
/nv/item_files/modem/data/dsmgr/suppress_gsm_on_srvcc_csfb\n\
/nv/item_files/modem/data/dsmgr/monitor_flow_timer\n\
/nv/item_files/modem/data/dsmgr/wlan_3g_ip_continuity_enabled\n";

#define DS3G_DISABLE_DSD_SYNC_NV_PATH   \
          "/nv/item_files/data/dsd/disable_data_dsd_sync"

#define DS3G_MDM_CONFIG_NV_PATH   \
          "/nv/item_files/data/common/dsmgr/a2_mdm_config_info"

#define DS3G_DUN_ACC_CFG_PATH   \
          "/nv/item_files/modem/data/3gpp/ps/3gpp_data_dun_acc_cfg"

#define DS3G_REFRESH_VOTE_OK_NV_PATH   \
          "/nv/item_files/modem/uim/mmgsdi/refresh_vote_ok"

#define DS3G_INTERNAL_AUTH_NV_PATH   \
          "/nv/item_files/data/3gpp2/internal_auth"

#define DS3G_ENABLE_ARBITRATION_NV_PATH   \
          "/nv/item_files/data/3gpp2/enable_arbitration"

#define DS3G_CONFIG_PDN_LEVEL_AUTH     \
          "/nv/item_files/modem/data/dsmgr/config_pdn_level_auth"

#define DS3G_SUPPRESS_GSM_ONSRVCC_CSFB     \
          "/nv/item_files/modem/data/dsmgr/suppress_gsm_on_srvcc_csfb"

#define DS3G_CONFIG_LATENCY_IND     \
          "/nv/item_files/data/common/ds_ml1_latency_ind"

#define DS3G_WLAN_3G_IP_CONTINUITY_ENABLED     \
          "/nv/item_files/modem/data/dsmgr/wlan_3g_ip_continuity_enabled"

/*===========================================================================

                      INTERNAL FUNCTION DEFINITIONS

==========================================================================*/


/*===========================================================================
FUNCTION      DS3G_MEM_CTRL_EV_CB

DESCRIPTION   This function will invoke client functions when a DSM Mem 
              Control Event occurs.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_mem_ctrl_ev_cb
(
  dsm_mempool_id_enum_type  pool_id,
  dsm_mem_level_enum_type   event,
  dsm_mem_op_enum_type      op
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds3g_siolib_mem_ctrl_ev_cb(pool_id, event, op);
  ds_3gpp_bearer_cntxt_mem_ctrl_ev_cb(pool_id, event, op);

  return;
} /* ds3g_mem_ctrl_ev_cb() */

/*===========================================================================
FUNCTION      DS3G_REGISTER_MEM_EVENT_CALLBACKS

DESCRIPTION   This function will register call functions with DSM memory events

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_register_mem_event_callbacks( void )
{
  /*-----------------------------------------------------------------------
    Register all DSM_DS_SMALL_ITEM_POOL related events.
  -----------------------------------------------------------------------*/
  dsm_reg_mem_event_cb ( DSM_DS_SMALL_ITEM_POOL,
                         DSM_MEM_LEVEL_SIO_MANY,
                         DSM_MEM_OP_FREE,
                         ds3g_mem_ctrl_ev_cb );
  
  dsm_reg_mem_event_cb ( DSM_DS_SMALL_ITEM_POOL,
                         DSM_MEM_LEVEL_SIO_MANY,
                         DSM_MEM_OP_NEW,
                         ds3g_mem_ctrl_ev_cb );

  dsm_reg_mem_event_cb ( DSM_DS_SMALL_ITEM_POOL,
                         DSM_MEM_LEVEL_SIO_FEW,
                         DSM_MEM_OP_NEW,
                         ds3g_mem_ctrl_ev_cb );
  
  dsm_reg_mem_event_cb ( DSM_DS_SMALL_ITEM_POOL,
                         DSM_MEM_LEVEL_DNE,
                         DSM_MEM_OP_NEW,
                         ds3g_mem_ctrl_ev_cb );
  
  /*-----------------------------------------------------------------------
    Register all DSM_DS_LARGE_ITEM_POOL related events.
  -----------------------------------------------------------------------*/
  dsm_reg_mem_event_cb ( DSM_DS_LARGE_ITEM_POOL,
                         DSM_MEM_LEVEL_SIO_MANY,
                         DSM_MEM_OP_FREE,
                         ds3g_mem_ctrl_ev_cb );

  dsm_reg_mem_event_cb ( DSM_DS_LARGE_ITEM_POOL,
                         DSM_MEM_LEVEL_SIO_MANY,
                         DSM_MEM_OP_NEW,
                         ds3g_mem_ctrl_ev_cb );
  
  dsm_reg_mem_event_cb ( DSM_DS_LARGE_ITEM_POOL,
                         DSM_MEM_LEVEL_SIO_FEW,
                         DSM_MEM_OP_NEW,
                         ds3g_mem_ctrl_ev_cb );
  
  dsm_reg_mem_event_cb ( DSM_DS_LARGE_ITEM_POOL,
                         DSM_MEM_LEVEL_DNE,
                         DSM_MEM_OP_NEW,
                         ds3g_mem_ctrl_ev_cb );
  
  /*------------------------------------------------------------------ 
    Overwriting levels set in DSM init
    Number of data in flight is RTT * BW. The levels have to be inline
    with this.
  ------------------------------------------------------------------*/
#define DS3G_DSM_DS_LARGE_ITEM_SIO_MANY_MARK 100  
#define DS3G_DSM_DS_LARGE_ITEM_SIO_FEW_MARK  50

  dsm_reg_mem_event_level( DSM_DS_LARGE_ITEM_POOL,
                           DSM_MEM_LEVEL_SIO_FEW,
                           DS3G_DSM_DS_LARGE_ITEM_SIO_FEW_MARK );

  dsm_reg_mem_event_level( DSM_DS_LARGE_ITEM_POOL,
                           DSM_MEM_LEVEL_SIO_MANY,
                           DS3G_DSM_DS_LARGE_ITEM_SIO_MANY_MARK );

  /* Set to 1 for all features in dsm_init
  dsm_reg_mem_event_level( DSM_DS_LARGE_ITEM_POOL,
                           DSM_MEM_LEVEL_DNE,
                           DSMI_DS_LARGE_ITEM_DONT_EXCEED);
  */
  
} /* ds3g_siolib_register_mem_event_callbacks() */

/*===========================================================================
FUNCTION      DS3G_READ_IPV6_EFS_NV

DESCRIPTION   Resets common IPv6 efs items used by 3gpp & 3gpp2 MHs

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_reset_ipv6_efs_nv_per_subs_id
(
  sys_modem_as_id_e_type subs_id
)
{
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return;
  }

  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g( subs_id );

  if( ipv6_nv_efs_info.subs_info[subs_indx]!= NULL )
  {
    modem_mem_free((void *)ipv6_nv_efs_info.subs_info[subs_indx],
                   MODEM_MEM_CLIENT_DATA);
    ipv6_nv_efs_info.subs_info[subs_indx] = NULL;
  }
  return;
} /* ds3g_reset_ipv6_efs_nv_per_subs_id() */

#ifdef FEATURE_DATA_PS_IPV6
/*===========================================================================
FUNCTION      DS3G_READ_IPV6_EFS_NV_PER_SUBS_ID

DESCRIPTION   Read & store common IPv6 efs items used by 3gpp & 3gpp2 MHs 
              per subscription

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_read_ipv6_efs_nv_per_subs_id
(
  sys_modem_as_id_e_type  subs_id
)
{
  nv_item_type                ds_nv_item;
  nv_stat_enum_type           status;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  /*-------------------------------------------------------------------------
    Enable IPv6 irrespective of value of NV 1896 defined in EFS.
  -------------------------------------------------------------------------*/
    ipv6_nv_efs_info.is_enabled = IPV6_ENABLED_DEFAULT;

  if( ipv6_nv_efs_info.subs_info[subs_indx] == NULL )
  {
    ipv6_nv_efs_info.subs_info[subs_indx] =
      (ds3g_ipv6_efs_nv_subs_specific_info*)
        modem_mem_alloc (sizeof(ds3g_ipv6_efs_nv_subs_specific_info),
                         MODEM_MEM_CLIENT_DATA);

    if( ipv6_nv_efs_info.subs_info[subs_indx] != NULL)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Ipv6 nv info subs ID allocated %d",
                      subs_id);
    }
    else
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                      "Cannot allocate memory for ipv6 NV for subs_id:%d",
                      subs_id);
      return;
    }
  }

  /*-----------------------------------------------------------------------
    Initialize all subscription specific NV values to zero
  -----------------------------------------------------------------------*/
  memset(ipv6_nv_efs_info.subs_info[subs_indx],
         0,
         sizeof(ds3g_ipv6_efs_nv_subs_specific_info));

  /*-----------------------------------------------------------------------
    Read the IP6 State Machine config info
  -----------------------------------------------------------------------*/
  status = ds3gcfgmgr_read_legacy_nv_ex( NV_IPV6_SM_CONFIG_I,
                                         &ds_nv_item,
                                         subs_id);
  if (NV_DONE_S != status)
  {
    ds_nv_item.ipv6_sm_config.init_sol_delay   =
                                          IP6_SM_DEFAULT_INIT_SOL_DELAY;
    ds_nv_item.ipv6_sm_config.sol_interval     =
                                            IP6_SM_DEFAULT_SOL_INTERVAL;
    ds_nv_item.ipv6_sm_config.resol_interval   =
                                          IP6_SM_DEFAULT_RESOL_INTERVAL;
    ds_nv_item.ipv6_sm_config.max_sol_attempts =
                                        IP6_SM_DEFAULT_MAX_SOL_ATTEMPTS;
    ds_nv_item.ipv6_sm_config.max_resol_attempts =
                                      IP6_SM_DEFAULT_MAX_RESOL_ATTEMPTS;
    ds_nv_item.ipv6_sm_config.pre_ra_exp_resol_time =
                                   IP6_SM_DEFAULT_PRE_RA_EXP_RESOL_TIME;
  }

  ipv6_nv_efs_info.subs_info[subs_indx]->sm_config.init_sol_delay =
                    ((ds_nv_item.ipv6_sm_config.init_sol_delay > 0) ?
                      ds_nv_item.ipv6_sm_config.init_sol_delay :
                      IP6_SM_DEFAULT_INIT_SOL_DELAY
                    );
  
  ipv6_nv_efs_info.subs_info[subs_indx]->sm_config.sol_interval =
                    ((ds_nv_item.ipv6_sm_config.sol_interval > 0) ?
                      ds_nv_item.ipv6_sm_config.sol_interval :
                      IP6_SM_DEFAULT_SOL_INTERVAL
                    );

  ipv6_nv_efs_info.subs_info[subs_indx]->sm_config.resol_interval =
                    ((ds_nv_item.ipv6_sm_config.resol_interval > 0) ?
                      ds_nv_item.ipv6_sm_config.resol_interval :
                      IP6_SM_DEFAULT_RESOL_INTERVAL
                    );

  ipv6_nv_efs_info.subs_info[subs_indx]->sm_config.max_sol_attempts =
                    ((ds_nv_item.ipv6_sm_config.max_sol_attempts > 0) ?
                      ds_nv_item.ipv6_sm_config.max_sol_attempts :
                      IP6_SM_DEFAULT_MAX_SOL_ATTEMPTS
                    );

  ipv6_nv_efs_info.subs_info[subs_indx]->sm_config.max_resol_attempts =
                    ((ds_nv_item.ipv6_sm_config.max_resol_attempts > 0) ?
                      ds_nv_item.ipv6_sm_config.max_resol_attempts :
                      IP6_SM_DEFAULT_MAX_RESOL_ATTEMPTS
                    );

  ipv6_nv_efs_info.subs_info[subs_indx]->sm_config.pre_ra_exp_resol_time =
                    ((ds_nv_item.ipv6_sm_config.pre_ra_exp_resol_time > 0) ?
                      ds_nv_item.ipv6_sm_config.pre_ra_exp_resol_time :
                      IP6_SM_DEFAULT_PRE_RA_EXP_RESOL_TIME
                    );

  status = ds3gcfgmgr_read_legacy_nv_ex( NV_PDSN_AS_PROXY_IPV6_DNS_SERVER_I,
                                         &ds_nv_item, subs_id );
  if (NV_DONE_S == status)
  {
    ipv6_nv_efs_info.subs_info[subs_indx]->pdsn_as_proxy_ipv6_dns_server
                                    = ds_nv_item.pdsn_as_proxy_ipv6_dns_server;
  }
  else
  {
    ipv6_nv_efs_info.subs_info[subs_indx]->pdsn_as_proxy_ipv6_dns_server 
      = FALSE;
  }

  /*-------------------------------------------------------------------------
    Read the values of DNS addrs from NV and store:
    Get Primary DNS IP addresses from NV.
    Make IP addresses as NULL if NV was never written.
  -------------------------------------------------------------------------*/
  status = ds3gcfgmgr_read_legacy_nv_ex( NV_IPV6_PRIMARY_DNS_I, &ds_nv_item,
                                         subs_id );
  if (NV_DONE_S == status)
  {
    /*-----------------------------------------------------------------------
      Store number retrieved from NV.
    -----------------------------------------------------------------------*/
    ipv6_nv_efs_info.subs_info[subs_indx]->primary_dns.in6_u.u6_addr64[0] =
      ds_nv_item.ipv6_primary_dns.prefix;
    ipv6_nv_efs_info.subs_info[subs_indx]->primary_dns.in6_u.u6_addr64[1] =
      ds_nv_item.ipv6_primary_dns.iid;
  }
  else
  {
    /* NV item not populated; use default */
  }

  /*-------------------------------------------------------------------------
    Get Secondary DNS IP addresses from NV.
    Make IP addresses as NULL if NV was never written.
  -------------------------------------------------------------------------*/
  status = ds3gcfgmgr_read_legacy_nv_ex( NV_IPV6_SECONDARY_DNS_I, &ds_nv_item,
                                         subs_id );
  if (NV_DONE_S == status)
  {
    /*-----------------------------------------------------------------------
      Store number retrieved from NV.
    -----------------------------------------------------------------------*/
    ipv6_nv_efs_info.subs_info[subs_indx]->secondary_dns.in6_u.u6_addr64[0] =
      ds_nv_item.ipv6_secondary_dns.prefix;
    ipv6_nv_efs_info.subs_info[subs_indx]->secondary_dns.in6_u.u6_addr64[1] =
      ds_nv_item.ipv6_secondary_dns.iid;
  }
  else
  {
    /* NV item not populated; use default */
  }

  /*-------------------------------------------------------------------------
    Read IPv6 failover config nv
  -------------------------------------------------------------------------*/
  status = ds3gcfgmgr_read_legacy_nv(NV_IPV6_FAILOVER_CONFIG_I, &ds_nv_item);
  if(NV_DONE_S == status)
  {
    ipv6_nv_efs_info.failover_mode = (ps_iface_ip_ver_failover_e_type)
                                      ds_nv_item.ipv6_failover_config;
  }
  else
  {
    ipv6_nv_efs_info.failover_mode = IPV6_PREFERRED;
  }

  DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "failover_mode = %d",
                  ipv6_nv_efs_info.failover_mode);

  /*-------------------------------------------------------------------------
    Get IPv6 IID info from NV.
  -------------------------------------------------------------------------*/
  status = ds3gcfgmgr_read_legacy_nv_ex( NV_PS_IPV6_IID_I, &ds_nv_item,
                                         subs_id );
  if (NV_DONE_S == status)
  {
    /*-----------------------------------------------------------------------
      Store number retrieved from NV.
    -----------------------------------------------------------------------*/
    ipv6_nv_efs_info.subs_info[subs_indx]->iid_info.iid_enum_type =
      IPV6_USER_IID;
    ipv6_nv_efs_info.subs_info[subs_indx]->iid_info.iid_val =
      ds_nv_item.ps_ipv6_iid.iid_val;
  }
  else
  {
    /* NV item not populated; use default */
    ipv6_nv_efs_info.subs_info[subs_indx]->iid_info.iid_enum_type =
      IPV6_RANDOM_IID;
    ipv6_nv_efs_info.subs_info[subs_indx]->iid_info.iid_val = 0;
  }

  return;
} /* ds3g_read_ipv6_efs_items() */
#endif /* FEATURE_DATA_PS_IPV6 */

/*===========================================================================
FUNCTION      DS3G_READ_EHRPD_AUTH_IN_USIM

DESCRIPTION   Read & store ehrpd auth in usim NV item

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_read_ehrpd_auth_in_usim( void )
{
  nv_stat_enum_type  nv_status;
  nv_item_type       nv_item;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Default case "Don't read from card" */
  ds3g_ehprd_auth_in_usim = FALSE;

  nv_status = ds3gcfgmgr_read_legacy_nv( NV_EHRPD_AUTH_IN_USIM_I, &nv_item );
  if( nv_status == NV_DONE_S)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Read ACL from card");
    ds3g_ehprd_auth_in_usim = nv_item.ehrpd_auth_in_usim ;
  }
  else
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "NV_EHRPD_AUTH_IN_USIM_I read failed");
  }
  return;
}/* ds3g_read_ehrpd_auth_in_usim() */

#ifdef FEATURE_DATA_MUX_USB_TEST
/*===========================================================================
FUNCTION      DS3G_READ_QMI_PORT_LIST

DESCRIPTION   Read & store QMI port list

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_read_qmi_port_list( void )
{
  nv_item_type  nv_item;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(ds3gcfgmgr_read_legacy_nv(NV_QMI_PORT_LIST_I, &nv_item) == NV_DONE_S)
  {
    ds3g_is_mux_nv_set =(nv_item.qmi_port_list.port_list[0].qmi_port == 0x300);
  }
  return;
} /* ds3g_read_qmi_port_list() */
#endif /* FEATURE_DATA_MUX_USB_TEST */

/*===========================================================================
FUNCTION      DS3G_READ_NV_MODE_PREF

DESCRIPTION   Function to read NV mode preference.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_read_nv_mode_pref
(
  sys_modem_as_id_e_type  subs_id
)
{
  nv_item_type                nv_item;
  ds3gsubsmgr_subs_id_e_type  subs_index;
  cm_mode_pref_e_type         cm_mode_pref;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read the preferred mode NV item (NV 10) and store it */
  if( NV_DONE_S ==
      ds3gcfgmgr_read_legacy_nv_ex(NV_PREF_MODE_I, &nv_item, subs_id) )
  {
    nv_item.pref_mode.nam = nv_item.curr_nam;
  }
  else
  {
    nv_item.pref_mode.nam = 0;
  }

  if ( NV_DONE_S ==
       ds3gcfgmgr_read_legacy_nv_ex( NV_PREF_MODE_I ,&nv_item , subs_id))
  {
    subs_index = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);
    ds3g_nv_mode_pref[subs_index] = nv_item.pref_mode.mode;

    cm_mode_pref = ds3gsubsmgr_convert_nv_mode_to_cm_mode(nv_item.pref_mode.mode);
    ds3gsubsmgr_set_mode_pref(subs_id, cm_mode_pref);
  }
  else
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "ds3g_read_nv_mode_pref. NV read unsuccessful for subs_id:%d",
                    subs_id);
  }
  return;
} /* ds3g_read_nv_mode_pref() */

#ifdef FEATURE_8960_SGLTE_FUSION
/*===========================================================================
FUNCTION      DS3G_READ_DSD_SYNC_NV

DESCRIPTION    Utility to read from NV item - DISABLE DATA DSD SYNC

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_read_dsd_sync_nv( void )
{
  nv_stat_enum_type         read_efs_nv_retval;
  disable_dsd_sync_nv_type  disable_dsd_sync_nv;
  char                      item_file_path[] = DS3G_DISABLE_DSD_SYNC_NV_PATH;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_3GMGR_MSG0(MSG_LEGACY_LOW,"ds3g_read_dsd_sync_nv()");
  
  /* Read the NV item */
  read_efs_nv_retval = ds3gcfgmgr_read_efs_nv(item_file_path,
                                              &disable_dsd_sync_nv,
                                              sizeof(disable_dsd_sync_nv));

  if (NV_DONE_S == read_efs_nv_retval)
  {
    ds3g_disable_dsd_sync = disable_dsd_sync_nv.disable_flag;
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "NV item configured, value: %d",
                    ds3g_disable_dsd_sync);
  }
  else
  {
    ds3g_disable_dsd_sync = FALSE;
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "NV item not configured. Returning value: %d",
                    ds3g_disable_dsd_sync);
  }

  return;
} /* ds3g_read_dsd_sync_nv() */
#endif /* FEATURE_8960_SGLTE_FUSION */

#ifdef FEATURE_DATA_SDIO_MUX_A2_DUN
/*===========================================================================
FUNCTION      DS3G_READ_MDM_TETHERING_ENABLED_NV

DESCRIPTION    Utility to read from NV item - MDM TETHERED ENABLED

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_read_mdm_tethering_enabled_nv( void )
{
  char item_file_path[] = DS3G_MDM_CONFIG_NV_PATH;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-----------------------------------------------------------------------
    Read item file into config_info.
    If item file does not exist write default value to it.
  -----------------------------------------------------------------------*/ 
  retval = ds3gcfgmgr_read_efs_nv(
             item_file_path,
             &ds3g_is_mdm_tethering_enabled,
             sizeof(boolean));

  if ( 0 != retval )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_MED,
                    "Reading EFS item failed errno: %d, using default",retval);
  }
  return;
} /* ds3g_read_mdm_tethering_enabled_nv() */
#endif /* FEATURE_DATA_SDIO_MUX_A2_DUN */

/*===========================================================================
FUNCTION      DS3G_READ_DUN_ACC_CFG_NV

DESCRIPTION    Utility to read from NV item - DUN ACC CFG

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_read_dun_acc_cfg_nv( void )
{
  char item_file_path[] = DS3G_DUN_ACC_CFG_PATH;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*------------------------------------------------------------------------
    Get the config item to figure out if DUN Data path is accelerated.
    Store this info in the global variable ds3g_siolib_dun_acc_cfg
  ------------------------------------------------------------------------*/
  if(ds3gcfgmgr_read_efs_nv(item_file_path,
                            (unsigned char*)&(ds3g_dun_acc_cfg),
                            sizeof(uint8)) == 0)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "DUN acceleration cfg item set to: %d 0-OFF, 1-ON",
                    ds3g_dun_acc_cfg);
  }
  else
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "Cannot get config item. Not accelerating DUN path");
  }
  return;
} /* ds3g_read_mdm_tethering_enabled_nv() */

/*===========================================================================
FUNCTION      DS3G_READ_REFRESH_VOTE_OK_NV

DESCRIPTION    Utility to read from NV item - REFRESH VOTE OK

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_read_refresh_vote_ok_nv
(
  sys_modem_as_id_e_type  subs_id
)
{
  char                        item_file_path[] = DS3G_REFRESH_VOTE_OK_NV_PATH;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
  nv_stat_enum_type           read_status;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g( subs_id );

  if (!ds3gsubsmgr_is_ds3g_subs_index_valid(subs_indx))
  {
    return;
  }

  read_status = ds3gcfgmgr_read_efs_nv_ex(item_file_path,
                                          &ds3g_refresh_vote_ok[subs_indx],
                                          sizeof(boolean),
                                          subs_id);
  if (read_status != NV_DONE_S)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "refresh_vote_ok EFS item not configured, setting default to TRUE");
    ds3g_refresh_vote_ok[subs_indx] = TRUE;
  }
  return;
} /* ds3g_read_refresh_vote_ok_nv() */

/*===========================================================================
FUNCTION DS3G_READ_EPDG_3G_IP_CONTINUITY_ENABLED

DESCRIPTION
  This functions tell whether EPDG-3G IP Continuity is enabled or not

DEPENDENCIES
  None

RETURN VALUE
  TRUE, EPDG-3G IP Continuity is enabled
  FALSE, otherwise

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_read_epdg_3g_ip_continuity_enabled
(
  sys_modem_as_id_e_type subs_id
)
{
  char                        item_file_path[] = DS3G_WLAN_3G_IP_CONTINUITY_ENABLED;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
  nv_stat_enum_type           read_status;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g( subs_id );

  if (!ds3gsubsmgr_is_ds3g_subs_index_valid(subs_indx))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR, "Subs id is invalid: %d", subs_indx);
    return;
  }

  read_status = ds3gcfgmgr_read_efs_nv_ex
                  (item_file_path, 
                   &ds3g_efs_info[subs_indx].wlan_3g_ip_continuity_enabled,
                   sizeof(uint8),subs_id);

  if (read_status != NV_DONE_S)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_MED,
                    "DS3G_WLAN_3G_IP_CONTINUITY_ENABLED not configured, "
                    "setting default to FALSE");
    ds3g_efs_info[subs_indx].wlan_3g_ip_continuity_enabled = FALSE;
  }
  return;
} /* ds3g_read_epdg_3g_ip_continuity_enabled */

/*===========================================================================
FUNCTION DS3G_IS_EPDG_3G_IP_CONTINUITY_ENABLED

DESCRIPTION
  This functions tell whether EPDG-3G IP Continuity is enabled or not

DEPENDENCIES
  None

RETURN VALUE
  TRUE, EPDG-3G IP Continuity is enabled
  FALSE, otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_is_epdg_3g_ip_continuity_enabled
(
  ps_sys_subscription_enum_type    ps_subs_id
)
{
  sys_modem_as_id_e_type subs_id = SYS_MODEM_AS_ID_NONE;
  /*-----------------------------------------------------------------------*/

  subs_id = ds3gsubsmgr_subs_id_ds_to_cm
              ((ds_sys_subscription_enum_type)ps_subs_id);

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR, "Subs id is invalid: %d", subs_id);
    return FALSE;
  }

  DATA_3GMGR_MSG2(MSG_LEGACY_HIGH, 
                  "WLAN 3G IP Continuity Enabled:%d, Subs Id: %d", 
                  ds3g_efs_info[subs_id].wlan_3g_ip_continuity_enabled,
                  subs_id);

  return ds3g_efs_info[subs_id].wlan_3g_ip_continuity_enabled;
} /* ds3g_is_epdg_3g_ip_continuity_enabled */

/*===========================================================================
FUNCTION      DS3G_READ_CONFIG_PDN_LEVEL_AUTH

DESCRIPTION   Read config_pdn_level_auth NV item

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_read_config_pdn_level_auth
(
  sys_modem_as_id_e_type  subs_id
)
{
  char                        item_file_path[] = DS3G_CONFIG_PDN_LEVEL_AUTH;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
  nv_stat_enum_type           read_status;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g( subs_id );

  if (!ds3gsubsmgr_is_ds3g_subs_index_valid(subs_indx))
  {
    return;
  }

  read_status = ds3gcfgmgr_read_efs_nv_ex(item_file_path,
                                          &ds3g_config_pdn_level_auth[subs_indx],
                                          sizeof(uint8),
                                          subs_id);
  if (read_status != NV_DONE_S)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "Config_PDN_Level_auth EFS item not configured, "
                    "setting default to TRUE");
    ds3g_config_pdn_level_auth[subs_indx] = DS3G_PDN_AUTH_CHAP;
  }
  return;
} /* ds3g_read_config_pdn_level_auth */

#ifdef FEATURE_DATA_IS707
/*===========================================================================
FUNCTION      DS3G_READ_INTERNAL_AUTH_NV

DESCRIPTION   Reads the Nv item internal auth and stores the value in the 
              static variable is_auth_internal

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_read_internal_auth_nv( void )
{
  char item_file_path[] = DS3G_INTERNAL_AUTH_NV_PATH;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( NV_DONE_S != ds3gcfgmgr_read_efs_nv(item_file_path,
                                          (void*)&ds3g_is_auth_internal,
                                          sizeof(boolean)) )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "INTERNAL_AUTH read failed, setting default = TRUE");
    ds3g_is_auth_internal = TRUE;
  }
  else 
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "INTERNAL_AUTH read returned %d", ds3g_is_auth_internal);
  }
  return;
} /* ds3g_read_internal_auth_nv() */
#endif /* FEATURE_DATA_IS707 */

/*===========================================================================
FUNCTION      DS3G_READ_ENABLE_ARBITRATION_NV

DESCRIPTION   Reads the Nv item enable arbitration and stores the value in 
              the static variable

RETURN VALUE  None

DEPENDENCIES  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_read_enable_arbitration_nv( void )
{
  nv_stat_enum_type  retval;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /** Read if arbitration is enabled from the EFS item */
  retval = ds3gcfgmgr_read_efs_nv(DS3G_ENABLE_ARBITRATION_NV_PATH,
                                  &ds3g_enable_arbitration,
                                  sizeof(boolean));
  if (0 != retval)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "EFS item not configured. arbitration enabled");
  }

  DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Arbitration Enabled = %d",
                  ds3g_enable_arbitration);

  return;
} /* ds3g_read_enable_arbitration_nv() */

/*===========================================================================
FUNCTION      DS3G_READ_SUPPRESS_GSM_ON_SRVCC_CSFB

DESCRIPTION   Read suppress_gsm_on_srvcc_csfb NV item

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_read_suppress_gsm_on_srvcc_csfb
(
  sys_modem_as_id_e_type  subs_id
)
{
  char                        item_file_path[] = DS3G_SUPPRESS_GSM_ONSRVCC_CSFB;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
  nv_stat_enum_type           read_status;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g( subs_id );

  if (!ds3gsubsmgr_is_ds3g_subs_index_valid(subs_indx))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR, "Subs id is invalid: %d", subs_indx);
    return;
  }

  read_status = ds3gcfgmgr_read_efs_nv_ex(
                    item_file_path,
                    &ds3g_suppress_gsm_on_srvcc_csfb[subs_indx],
                    sizeof(boolean),
                    subs_id);
  if (read_status != NV_DONE_S)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_MED,
                    "Suppress GSM on SRVCC/CSFB EFS item not configured, "
                    "setting default to FALSE");
    ds3g_suppress_gsm_on_srvcc_csfb[subs_indx] = FALSE;
  }
  return;
} /* ds3g_read_suppress_gsm_on_srvcc_csfb */

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS3G_UTIL_INIT

DESCRIPTION   This function is used to initialize the ds3g util module 
              during power up 
 
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_util_init( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_3GMGR_MSG0(MSG_LEGACY_LOW,"ds3g util init");
  ds3g_register_mem_event_callbacks();
  return;
}/* ds3g_util_init() */

/*===========================================================================
FUNCTION      DS3G_READ_LATENCY_INFO

DESCRIPTION   Read ml1 latency NV item

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_read_latency_info( void )
{
  char               item_file_path[] = DS3G_CONFIG_LATENCY_IND;
  nv_stat_enum_type  read_status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* to be changed to read subs based NV when subs changes are present*/
  read_status = ds3gcfgmgr_read_efs_nv(item_file_path,
                               &ds3g_config_latency,
                               sizeof(boolean));

  if (NV_DONE_S == read_status)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "ml1 latency item configured, value: %d",
                    ds3g_config_latency);
  }
  else
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "Config ML1 LATENCY item not configured, "
                    "setting default to TRUE");
    ds3g_config_latency = TRUE;
  }

  return;
}
/*===========================================================================
FUNCTION      ds3g_util_read_nv

DESCRIPTION   This function reads all the ds3g specific NV items
 
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_util_read_nv( void )
{
  ds3gsubsmgr_subs_id_e_type  subs_indx;
  sys_modem_as_id_e_type      cm_subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "ds3g_util_read_nv");

  for( subs_indx = DS3GSUBSMGR_SUBS_ID_MIN;
       subs_indx < DS3GSUBSMGR_SUBS_ID_MAX;
       subs_indx++ )
  {
    cm_subs_id = ds3gsubsmgr_subs_id_ds3g_to_cm(subs_indx);

  /*-------------------------------------------------------------------------
     Read & store Mode pref NV items for all subscriptions
  -------------------------------------------------------------------------*/
    ds3g_read_nv_mode_pref(cm_subs_id);

  /*-------------------------------------------------------------------------
     Read & store IPv6 efs/nv items for all subscriptions
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_PS_IPV6
    ds3g_read_ipv6_efs_nv_per_subs_id( cm_subs_id );
#endif /* FEATURE_DATA_PS_IPV6 */

    /*-----------------------------------------------------------------------
     Read & store Refresh vote ok nv items for all subscriptions
    -----------------------------------------------------------------------*/
    ds3g_read_refresh_vote_ok_nv( cm_subs_id );

    /*-----------------------------------------------------------------------
     Read & store PDN Level Auth nv items for subscription
    -----------------------------------------------------------------------*/
    ds3g_read_config_pdn_level_auth(cm_subs_id) ;

    /*-----------------------------------------------------------------------
     Read & store suppress gsm on SRVCC/CSFB nv items for subscription
    -----------------------------------------------------------------------*/
    ds3g_read_suppress_gsm_on_srvcc_csfb(cm_subs_id);

    /*-----------------------------------------------------------------------
     Read & store EPDG-3G IP Continuity NV item per subscription
    -----------------------------------------------------------------------*/
    ds3g_read_epdg_3g_ip_continuity_enabled(cm_subs_id);
  }

  ds3g_read_latency_info(); 
  /*-------------------------------------------------------------------------
     Read & store EHRPD_AUTH_IN_USIM NV item
  -------------------------------------------------------------------------*/
  ds3g_read_ehrpd_auth_in_usim();

  /*-------------------------------------------------------------------------
     Read & store QMI port list NV item
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_MUX_USB_TEST
  ds3g_read_qmi_port_list();
#endif /* FEATURE_DATA_MUX_USB_TEST */

  /*-------------------------------------------------------------------------
     Read & store Disable DSD sync NV item
  -------------------------------------------------------------------------*/
#ifdef FEATURE_8960_SGLTE_FUSION
  ds3g_read_dsd_sync_nv();
#endif /* FEATURE_8960_SGLTE_FUSION */

  /*-------------------------------------------------------------------------
     Read & store MDM Tethering enabled NV item
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_SDIO_MUX_A2_DUN
  ds3g_read_mdm_tethering_enabled_nv();
#endif /* FEATURE_DATA_SDIO_MUX_A2_DUN */

  /*-------------------------------------------------------------------------
     Read & store DUN Accelerated config NV item
  -------------------------------------------------------------------------*/
  ds3g_read_dun_acc_cfg_nv();

  /*-------------------------------------------------------------------------
     Read & store Internal Auth NV item
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_IS707
  ds3g_read_internal_auth_nv();
#endif /* FEATURE_DATA_IS707 */

  /*-------------------------------------------------------------------------
     Read & store Enable Arbitration NV item
  -------------------------------------------------------------------------*/
  ds3g_read_enable_arbitration_nv();

  return;
} /* ds3g_util_read_nv() */

/*===========================================================================
FUNCTION      DS3G_PROCESS_MCFG_REFRESH_IND

DESCRIPTION   This function process MCFG refresh ind and re-reads all ds3g/dsd 
              NV items.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_process_mcfg_refresh_ind
(
  sys_modem_as_id_e_type  subs_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "ds3g_process_mcfg_refresh_ind subs_id:%d",
                  subs_id);

  /*-------------------------------------------------------------------------
     Re-read mode pref nv items for subscription
  -------------------------------------------------------------------------*/
  ds3g_read_nv_mode_pref( subs_id );

  ds3g_read_ehrpd_auth_in_usim();
  ds3gmmgsdi_nv_refresh_ind( subs_id );

  /*-----------------------------------------------------------------------
   Read & store Refresh vote ok nv items for subscription
  -----------------------------------------------------------------------*/
  ds3g_read_refresh_vote_ok_nv( subs_id );
  /*-----------------------------------------------------------------------
   Read & store PDN Level Auth nv items for subscription
  -----------------------------------------------------------------------*/
  ds3g_read_config_pdn_level_auth(subs_id) ;
  /*-------------------------------------------------------------------------
     Reset and re-read IPv6 efs/nv items for subscription
  -------------------------------------------------------------------------*/
  ds3g_reset_ipv6_efs_nv_per_subs_id(subs_id);
  ds3g_read_ipv6_efs_nv_per_subs_id(subs_id);

  /*-------------------------------------------------------------------------
   Read & store suppress gsm on SRVCC/CSFB nv items for subscription
  -------------------------------------------------------------------------*/
  ds3g_read_suppress_gsm_on_srvcc_csfb( subs_id );

  /*-------------------------------------------------------------------------
     Notify DSD regarding MCFG refresh indication
  -------------------------------------------------------------------------*/
  ds_dsd_ext_notify_nv_refresh_ind( subs_id );

  /*-----------------------------------------------------------------------
     Read & store EPDG-3G IP Continuity NV item per subscription
  -----------------------------------------------------------------------*/
  ds3g_read_epdg_3g_ip_continuity_enabled(subs_id);

  return;
}/* ds3g_process_mcfg_refresh_ind() */

/*===========================================================================
FUNCTION      DS3G_IS_PREF_PKT_MODE

DESCRIPTION   This function determines if the specified mode is the currently 
              preferred mode for packet data calls.
  
DEPENDENCIES  None

RETURN VALUE  TRUE: mode is preferred pkt mode
              FALSE: mode is not preferred pkt mode

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_is_pref_pkt_mode
(
  sys_sys_mode_e_type  mode
)
{
  sys_modem_as_id_e_type subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  subs_id = ds3g_get_ps_subs_id();
  return ds3g_is_pref_pkt_mode_ex(mode,subs_id);
} /* ds3g_is_pref_pkt_mode() */

/*===========================================================================
FUNCTION      DS3G_IS_PREF_PKT_MODE_EX

DESCRIPTION   This function determines if the specified mode is the currently 
              preferred mode for packet data calls on passed sub.
  
DEPENDENCIES  None

RETURN VALUE  TRUE: mode is preferred pkt mode
              FALSE: mode is not preferred pkt mode

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_is_pref_pkt_mode_ex
(
  sys_sys_mode_e_type     mode,
  sys_modem_as_id_e_type  subs_id
)
{
  sys_sys_mode_e_type pref_pkt_mode;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( SYS_SYS_MODE_LTE == ds3g_get_target_rat_ex(subs_id) )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "ds3g_is_pref_pkt_mode_ex: LTE attach in progress");
    pref_pkt_mode = SYS_SYS_MODE_LTE;
  }
  else
  {
    pref_pkt_mode = ds3g_get_pref_sys_mode_for_packet_orig();
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "ds3g_is_pref_pkt_mode_ex: %d", pref_pkt_mode);
  }

  return( mode == pref_pkt_mode );
} /* ds3g_is_pref_pkt_mode_ex() */

/*===========================================================================
FUNCTION      DS3G_SYS_SYS_MODE_TO_DS_APN_SYS_MODE

DESCRIPTION   This function is a utility function to convert cm sys sys mode 
              to ds apn pkt sys mode.
  
DEPENDENCIES  None

RETURN VALUE  TRUE or FALSE

SIDE EFFECTS  None
===========================================================================*/
ds_pkt_sys_mode_e_type ds3g_sys_sys_mode_to_ds_apn_sys_mode
(
  sys_sys_mode_e_type  sys_sys_mode
)
{
  ds_pkt_sys_mode_e_type  pkt_sys_mode = DS_PKT_SYS_MODE_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch (sys_sys_mode)
  {
    case SYS_SYS_MODE_NONE:
      pkt_sys_mode = DS_PKT_SYS_MODE_NONE;
      break;
    case SYS_SYS_MODE_NO_SRV:
      pkt_sys_mode = DS_PKT_SYS_MODE_NO_SRV;
      break;
    case SYS_SYS_MODE_AMPS:
      pkt_sys_mode = DS_PKT_SYS_MODE_AMPS;
      break;
    case SYS_SYS_MODE_CDMA:
      pkt_sys_mode = DS_PKT_SYS_MODE_CDMA;
      break;
    case SYS_SYS_MODE_GSM:
      pkt_sys_mode = DS_PKT_SYS_MODE_GSM;
      break;
    case SYS_SYS_MODE_HDR:
      pkt_sys_mode = DS_PKT_SYS_MODE_HDR;
      break;
    case SYS_SYS_MODE_WCDMA:
      pkt_sys_mode = DS_PKT_SYS_MODE_WCDMA;
      break;
    case SYS_SYS_MODE_GPS:
      pkt_sys_mode = DS_PKT_SYS_MODE_GPS;
      break;
    case SYS_SYS_MODE_GW:
      pkt_sys_mode = DS_PKT_SYS_MODE_GW;
      break;
    case SYS_SYS_MODE_WLAN:
      pkt_sys_mode = DS_PKT_SYS_MODE_WLAN;
      break;
    case SYS_SYS_MODE_LTE:
      pkt_sys_mode = DS_PKT_SYS_MODE_LTE;
      break;
    case SYS_SYS_MODE_GWL:
      pkt_sys_mode = DS_PKT_SYS_MODE_GWL;
      break;
    case SYS_SYS_MODE_TDS:
      pkt_sys_mode = DS_PKT_SYS_MODE_TDS;
      break;
    default:
      break;
  }

  return pkt_sys_mode;
} /* ds3g_sys_sys_mode_to_ds_apn_sys_mode() */

/*===========================================================================
FUNCTION      DS3G_GET_APN_PREF_PKT_MODE

DESCRIPTION   This function returns the currently preferred mode for packet 
              data calls on the specified PDN when MPPM is enabled.

DEPENDENCIES  None

RETURN VALUE  TRUE or FALSE

SIDE EFFECTS  None
===========================================================================*/
ds_pkt_sys_mode_e_type ds3g_get_apn_pref_pkt_mode
(
  ds3g_apn_name_type  *apn_name
)
{
  sys_modem_as_id_e_type  subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  subs_id = ds3g_get_ps_subs_id();
  return ds3g_get_apn_pref_pkt_mode_ex(apn_name,subs_id);
} /* ds3g_get_apn_pref_pkt_mode() */

/*===========================================================================
FUNCTION      DS3G_GET_APN_PREF_PKT_MODE_EX

DESCRIPTION   This function returns the currently preferred mode for packet 
              data calls on the specified PDN when MPPM is enabled
              passed sub.

DEPENDENCIES  None

RETURN VALUE  TRUE or FALSE

SIDE EFFECTS  None
===========================================================================*/
ds_pkt_sys_mode_e_type ds3g_get_apn_pref_pkt_mode_ex
(
  ds3g_apn_name_type      *apn_name,
  sys_modem_as_id_e_type   subs_id
)
{
  ds_pkt_sys_mode_e_type  pref_pkt_mode = DS_PKT_SYS_MODE_NONE;
#ifdef FEATURE_DATA_WLAN_MAPCON
  mppm_radio_type                 pref_radio_index;
  ds_mppm_apn_name_type           mppm_apn_name;
  ds_mppm_pref_in_s_type          mppm_input_params;
#endif /*FEATURE_DATA_WLAN_MAPCON*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (apn_name == NULL)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "ds3g_get_pdn_pref_pkt_mode: "
                    "passed NULL APN ptr, return DS_PKT_SYS_MODE_NONE");
    return DS_PKT_SYS_MODE_NONE;
  }

#ifndef FEATURE_DATA_WLAN_MAPCON
  /*--------------------------------------------------------------------
    For non-MAPCON, get the WWAN mode. 
  -----------------------------------------------------------------------*/
  pref_pkt_mode = ds3g_sys_sys_mode_to_ds_apn_sys_mode
                                 (ds3g_get_current_network_mode_ex(subs_id));
#else
  /*--------------------------------------------------------------------
    For MAPCON, get the PDN network mode from MPPM. 
  -----------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
    Get APN string length and string from registry.
  -------------------------------------------------------------------------*/
  memset (&mppm_apn_name, 0, sizeof(ds_mppm_apn_name_type));
  mppm_apn_name.apn_string_len = apn_name->apn_string_len;
  strlcpy(mppm_apn_name.apn_string,
          apn_name->apn_string,
          mppm_apn_name.apn_string_len+1);
  mppm_input_params.dsd_subs_id = (ps_sys_subscription_enum_type) 
       ds3gsubsmgr_subs_id_ds3g_to_ds(ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id));

  /*-------------------------------------------------------------------------
    Get MPPM pref radio and return TRUE if it is 1x/HDR. Else return FALSE.
  -------------------------------------------------------------------------*/
  pref_radio_index = DS_MPPM_RADIO_MIN +
                   ds_mppm_get_pref_rat_for_apn_string_ex(mppm_apn_name,
                                                          mppm_input_params,
                                                          NULL, 
                                                          NULL);

  switch (pref_radio_index)
  {
    case DS_MPPM_RADIO_1X:
    case DS_MPPM_RADIO_HDR:
      pref_pkt_mode = DS_PKT_SYS_MODE_CDMA;
      break;

    case DS_MPPM_RADIO_LTE:
      pref_pkt_mode = DS_PKT_SYS_MODE_LTE;
      break;

    case DS_MPPM_RADIO_GSM:
      pref_pkt_mode = DS_PKT_SYS_MODE_GSM;
      break;

    case DS_MPPM_RADIO_TDSCDMA:
      pref_pkt_mode = DS_PKT_SYS_MODE_TDS;
      break;

    case DS_MPPM_RADIO_WCDMA:
      pref_pkt_mode = DS_PKT_SYS_MODE_WCDMA;
      break;

    case DS_MPPM_RADIO_WLAN_LB:
      pref_pkt_mode = DS_PKT_SYS_MODE_WLAN;
      break;

    case DS_MPPM_RADIO_IWLAN:
      pref_pkt_mode = DS_PKT_SYS_MODE_IWLAN;
      break;

    default:
      pref_pkt_mode = DS_PKT_SYS_MODE_NONE;
      break;
  }

#endif /* FEATURE_DATA_WLAN_MAPCON */

  return pref_pkt_mode;
} /* ds3g_get_apn_pref_pkt_mode_ex() */

/*===========================================================================
FUNCTION      DS3G_IS_APN_PREF_PKT_MODE

DESCRIPTION   This function determines for a PDN if the specified mode is the
              currently preferred mode for packet data calls.

DEPENDENCIES  None

RETURN VALUE  TRUE or FALSE

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_is_apn_pref_pkt_mode
(
  ds_pkt_sys_mode_e_type  mode,
  ds3g_apn_name_type     *apn_name
)
{
  sys_modem_as_id_e_type  subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  subs_id = ds3g_get_ps_subs_id();
  
  return ds3g_is_apn_pref_pkt_mode_ex(mode, apn_name, subs_id);
} /* ds3g_is_apn_pref_pkt_mode() */

/*===========================================================================
FUNCTION      DS3G_IS_APN_PREF_PKT_MODE_EX

DESCRIPTION   This function determines for a PDN if the specified mode is the
              currently preferred mode for packet data calls for passed sub.

DEPENDENCIES  None

RETURN VALUE  TRUE or FALSE

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_is_apn_pref_pkt_mode_ex
(
  ds_pkt_sys_mode_e_type  mode,
  ds3g_apn_name_type     *apn_name,
  sys_modem_as_id_e_type  subs_id
)
{
  ds_pkt_sys_mode_e_type  pref_pkt_mode = DS_PKT_SYS_MODE_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  pref_pkt_mode = ds3g_get_apn_pref_pkt_mode_ex(apn_name, subs_id);

  return( mode == pref_pkt_mode );
} /* ds3g_is_apn_pref_pkt_mode_ex() */

/*===========================================================================
FUNCTION      DS3G_GET_PREF_SYS_MODE_FOR_PACKET_ORIG

DESCRIPTION   This function determines the currently preferred mode for packet
              data calls.

DEPENDENCIES  None
  
RETURN VALUE  sys_sys_mode_e_type: sys mode

SIDE EFFECTS  None
===========================================================================*/
sys_sys_mode_e_type ds3g_get_pref_sys_mode_for_packet_orig( void )
{
  sys_sys_mode_e_type  pref_mode = SYS_SYS_MODE_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    pref_mode = ds3g_get_current_network_mode();

  DATA_3GMGR_MSG1(MSG_LEGACY_MED,
                  "ds3g_is_pref_pkt_mode: Current preferred mode %d", 
                  pref_mode);
  return pref_mode;
} /* ds3g_get_pref_sys_mode_for_packet_orig() */

/*===========================================================================
FUNCTION      DS3G_IS_DORM_ALLOWED_IN_PKT_MODE

DESCRIPTION   This function determines if dormancy is allowed in the currently 
              preferred mode for packet data calls.

DEPENDENCIES  None

RETURN VALUE  TRUE or FALSE

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_is_dorm_allowed_in_pkt_mode
(
  sys_sys_mode_e_type  mode
)
{
  sys_sys_mode_e_type  pref_mode;
  boolean              dorm_allowed = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  pref_mode = ds3g_get_current_network_mode();

  switch (pref_mode)
  {
  /*--------------------------------------------------------------------
    If the preferred mode is CDMA/HDR/WLAN and if the 
    current packet data mode is CDMA, allow dormancy 
  -----------------------------------------------------------------------*/
    case SYS_SYS_MODE_HDR:
    case SYS_SYS_MODE_CDMA:
#ifdef FEATURE_WLAN
    case SYS_SYS_MODE_WLAN:
#endif /* FEATURE_WLAN */
      if(mode == SYS_SYS_MODE_CDMA)
      {
        DATA_MSG0(MSG_LEGACY_LOW,"Dorm allowed");
        dorm_allowed = TRUE;
      }
      else
      {
        DATA_MSG0(MSG_LEGACY_LOW,"Dorm NOT allowed");
      }
      break;

#ifdef FEATURE_EPC_HANDOFF
  case SYS_SYS_MODE_LTE:
#ifdef FEATURE_EHRPD
    /*-----------------------------------------------------------------------
      If the call is active and the system changes to LTE, then CM will give
      call ended event. As part of processing call ended, we check if this call
      can go dormant. 

      When LTE is acquired, the call can not go dormant if call was on 1x or HRPD. 
      This is legacy behavior. 
      However, the call should go dormant (not to be torn down) if the call was 
      on eHRPD. Hence this logic.

      Call can go dormant if partial context is being maintained
      -----------------------------------------------------------------------*/
      if(((mode == SYS_SYS_MODE_CDMA) 
          && (ds707_pkt_mgr_is_data_on_ehrpd() == TRUE))
          || (TRUE == ds707_devmgr_in_partial_ctx()))
      {
        DATA_MSG0(MSG_LEGACY_LOW,"Dormancy allowed");
        dorm_allowed = TRUE;
      }
      else
      {
        DATA_MSG0(MSG_LEGACY_LOW,"Dormancy NOT allowed");
      }
#endif
    break;
#endif
    case SYS_SYS_MODE_GSM:
    case SYS_SYS_MODE_WCDMA:
    case SYS_SYS_MODE_TDS:
      break;

    case SYS_SYS_MODE_NO_SRV:
      dorm_allowed = TRUE;
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "pref mode is %d, dorm allowed",
                      pref_mode);
      break;

    default:
      /* irrelevant mode ... look at the current system mode to decide */
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Unknown pref mode is %d", pref_mode);
      break;
  }
  return (dorm_allowed);
} /* ds3g_is_dorm_allowed_in_pkt_mode() */

/*===========================================================================
FUNCTION      DS3G_IS_PREF_CS_DOMAIN_MODE

DESCRIPTION   This function determines if the specified mode is the currently
              preferred mode for CS data calls.

DEPENDENCIES  None

RETURN VALUE  TRUE or FALSE

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_is_pref_cs_domain_mode
(
  sys_sys_mode_e_type  mode
)
{
  sys_sys_mode_e_type  pref_cs_mode = SYS_SYS_MODE_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  pref_cs_mode = ds3g_get_current_cs_domain_mode();

  DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                  "ds3g_is_pref_cs_domain_mode: Current preferred CS mode %d", 
                  pref_cs_mode);
  return (mode == pref_cs_mode);
}/* ds3g_is_pref_cs_domain_mode() */

/*===========================================================================
FUNCTION      DS3G_SET_CM_OPRT_LPM_MODE

DESCRIPTION   Set CM operation mode to LPM

DEPENDENCIES  None

RETURN VALUE  TRUE: Successfully set 
              FALSE: Fail to set 

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_set_cm_oprt_lpm_mode( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "ds3g_set_cm_oprt_online_mode");
  return cm_ph_cmd_oprt_mode(NULL,
                             NULL,
                             ds3g_get_cm_client_id(),
                             SYS_OPRT_MODE_LPM);
} /* ds3g_set_cm_oprt_lpm_mode() */

/*===========================================================================
FUNCTION      DS3G_SET_CM_OPRT_ONLINE_MODE

DESCRIPTION   Set CM operation mode to online

DEPENDENCIES  None

RETURN VALUE  TRUE: Successfully set 
              FALSE: Fail to set 

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_set_cm_oprt_online_mode(void)
{
  boolean ret_val = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "ds3g_set_cm_oprt_online_mode");
  ret_val = cm_ph_cmd_oprt_mode(NULL,
                                NULL,
                                ds3g_get_cm_client_id(),
                                SYS_OPRT_MODE_ONLINE);
  if(cm_ph_cmd_get_ph_info(NULL, NULL, ds3g_get_cm_client_id()) != TRUE)
  {
  	DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                    "ds3g_set_cm_oprt_online_mode: Problem requesting initial "
                    "CM PH info ");
  	ret_val = FALSE;
  }
  return ret_val;
} /* ds3g_set_cm_oprt_online_mode() */

/*===========================================================================
FUNCTION      DS3G_GET_IPV6_SM_CONFIG_INFO

DESCRIPTION   Get IPv6 SM config info configured through NV item

DEPENDENCIES  None

RETURN VALUE  ip6_sm_config_type* - IPv6 SM config info

SIDE EFFECTS  None
===========================================================================*/
ip6_sm_config_type* ds3g_get_ipv6_sm_config_info
(
  sys_modem_as_id_e_type  subs_id
)
{
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return NULL;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g( subs_id );

  if( ipv6_nv_efs_info.subs_info[subs_indx] != NULL )
  {
    return &(ipv6_nv_efs_info.subs_info[subs_indx]->sm_config);
  }
  else
  {
    return NULL;
  }
} /* ds3g_get_ipv6_sm_config_info() */

/*===========================================================================
FUNCTION      DS3G_GET_IPV6_PRIMARY_DNS_ADDR

DESCRIPTION   Get IPv6 primary DNS server address

DEPENDENCIES  None

RETURN VALUE  ps_in6_addr* - Ptr containing the primary DNS server addr

SIDE EFFECTS  None
===========================================================================*/
struct ps_in6_addr* ds3g_get_ipv6_primary_dns_addr
(
  sys_modem_as_id_e_type  subs_id
)
{
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return NULL;
  }

  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g( subs_id );

  if( ipv6_nv_efs_info.subs_info[subs_indx] != NULL )
  {
    return &(ipv6_nv_efs_info.subs_info[subs_indx]->primary_dns);
  }
  else
  {
    return NULL;
  }
} /* ds3g_get_ipv6_primary_dns_addr() */

/*===========================================================================
FUNCTION      DS3G_GET_IPV6_SECONDARY_DNS_ADDR

DESCRIPTION   Get IPv6 secondary DNS server address

DEPENDENCIES  None

RETURN VALUE  ps_in6_addr* - Ptr containing the secondary DNS server addr

SIDE EFFECTS  None
===========================================================================*/
struct ps_in6_addr* ds3g_get_ipv6_secondary_dns_addr
(
  sys_modem_as_id_e_type  subs_id
)
{
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return NULL;
  }

  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g( subs_id );

  if( ipv6_nv_efs_info.subs_info[subs_indx] != NULL )
  {
    return &(ipv6_nv_efs_info.subs_info[subs_indx]->secondary_dns);
  }
  else
  {
    return NULL;
  }
} /* ds3g_get_ipv6_secondary_dns_addr() */

/*===========================================================================
FUNCTION      DS3G_IS_IPV6_ENABLED

DESCRIPTION   Returns whether IPv6 is enabled through NV or not.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  TRUE: IPv6 enabled
              FALSE: IPv6 disabled

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_is_ipv6_enabled( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "ipv6 enabled = %d",
                  ipv6_nv_efs_info.is_enabled);
  return ipv6_nv_efs_info.is_enabled;
} /* ds3g_is_ipv6_enabled() */

/*===========================================================================
FUNCTION      DS3G_IS_PDSN_AS_PROXY_IPv6_DNS_SERVER

DESCRIPTION   Returns whether PDSN acts as proxy IPv6 DNS server

DEPENDENCIES  None

RETURN VALUE  TRUE or FALSE

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_is_pdsn_as_proxy_ipv6_dns_server
(
  sys_modem_as_id_e_type  subs_id
)
{
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return FALSE;
  }

  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g( subs_id );

  if (ipv6_nv_efs_info.subs_info[subs_indx] != NULL)
  {
    DATA_3GMGR_MSG1(
       MSG_LEGACY_LOW,
       "pdsn_as_proxy_ipv6_dns_server = %d",
       ipv6_nv_efs_info.subs_info[subs_indx]->pdsn_as_proxy_ipv6_dns_server);

    return ipv6_nv_efs_info.subs_info[subs_indx]->pdsn_as_proxy_ipv6_dns_server;
  }
  else
  {
    return FALSE;
  }
} /* ds3g_is_pdsn_as_proxy_ipv6_dns_server() */

/*===========================================================================
FUNCTION      DS3G_GET_FAILOVER_MODE

DESCRIPTION   Returns the failover mode set in NV

DEPENDENCIES  None.

RETURN VALUE  Boolean.

SIDE EFFECTS  None.
===========================================================================*/
ps_iface_ip_ver_failover_e_type ds3g_get_failover_mode( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "failover_mode = %d",
                  ipv6_nv_efs_info.failover_mode);
  return ipv6_nv_efs_info.failover_mode;
} /* ds3g_get_failover_mode() */

/*===========================================================================
FUNCTION      DS3G_GET_IPV6_IID_INFO

DESCRIPTION   Returns IPv6 iid info

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_get_ipv6_iid_info
(
  sys_modem_as_id_e_type  subs_id,
  ds3g_ipv6_iid_type     *iid_info_p
)
{
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return;
  }

  if( iid_info_p == NULL )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "NULL iid indo ptr");
    return;
  }

  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g( subs_id );

  if (ipv6_nv_efs_info.subs_info[subs_indx] != NULL)
  {
    iid_info_p->iid_enum_type =
      ipv6_nv_efs_info.subs_info[subs_indx]->iid_info.iid_enum_type;
    iid_info_p->iid_val =
      ipv6_nv_efs_info.subs_info[subs_indx]->iid_info.iid_val;
  }
  else
  {
    iid_info_p->iid_enum_type = IPV6_RANDOM_IID;
    iid_info_p->iid_val = 0;
  }
} /* ds3g_get_ipv6_iid_info() */

/*===========================================================================
FUNCTION      DS3G_READ_EHRPD_AUTH_IN_USIM

DESCRIPTION   Returns ehrpd auth in usim NV item per subscription

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_get_ehrpd_auth_in_usim( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ds3g_ehprd_auth_in_usim;
}/* ds3g_get_ehrpd_auth_in_usim()*/

#ifdef FEATURE_DATA_MUX_USB_TEST
/*===========================================================================
FUNCTION      DS3G_IS_MUX_NV_ITEM_SET

DESCRIPTION   This function sreturns true if NV item 6873 is set to MUX ports 
 
DEPENDENCIES  None

RETURN VALUE  TRUE  - true if nv item has mux port 
              FALSE - false if NV item doesnt have mux ports

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_is_mux_nv_item_set( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ds3g_is_mux_nv_set;
} /* ds_is_mux_nv_item_set() */
#endif

/*===========================================================================
FUNCTION      DS3G_GET_NV_MODE_PREF

DESCRIPTION   This function returns mode preference NV value
 
DEPENDENCIES  None

RETURN VALUE  NV Mode pref

SIDE EFFECTS  None
===========================================================================*/
nv_mode_enum_type ds3g_get_nv_mode_pref
(
  sys_modem_as_id_e_type  subs_id
)
{
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return (nv_mode_enum_type)0;
  }

  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g( subs_id );
  return ds3g_nv_mode_pref[subs_indx];
} /* ds3g_get_nv_mode_pref() */

#ifdef FEATURE_8960_SGLTE_FUSION
/*===========================================================================
FUNCTION      DS3G_GET_DISABLE_DSD_SYNC

DESCRIPTION   Returns Disable DSD sync NV item

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_get_disable_dsd_sync( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ds3g_disable_dsd_sync;
}
#endif /* FEATURE_8960_SGLTE_FUSION */

#ifdef FEATURE_DATA_SDIO_MUX_A2_DUN
/*===========================================================================
FUNCTION      DS3G_GET_MDM_TETHERING_ENABLED

DESCRIPTION   Returns MDM tethering enabled NV item

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_get_mdm_tethering_enabled( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ds3g_is_mdm_tethering_enabled;
}
#endif /* FEATURE_DATA_SDIO_MUX_A2_DUN */
/*===========================================================================
FUNCTION      DS3G_GET_DUN_ACC_CFG

DESCRIPTION   Returns DUN acc cfg NV item

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
uint8 ds3g_get_dun_acc_cfg( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ds3g_dun_acc_cfg;
} /* ds3g_get_dun_acc_cfg() */

/*===========================================================================
FUNCTION      DS3G_GET_REFRESH_VOTE_OK_INFO

DESCRIPTION   Returns refresh vote ok NV item

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_get_refresh_vote_ok_info
(
  sys_modem_as_id_e_type  subs_id
)
{
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return FALSE;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g( subs_id );

  return ds3g_refresh_vote_ok[subs_indx];
} /* ds3g_get_refresh_vote_ok_info() */

/*===========================================================================
FUNCTION      DS3G_GET_CONFIG_PDN_LEVEL_AUTH

DESCRIPTION   Returns config_pdn_level_auth NV item

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
uint8 ds3g_get_config_pdn_level_auth
(
  sys_modem_as_id_e_type  subs_id
)
{
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return DS3G_PDN_AUTH_CHAP;
  }

  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g( subs_id );
  return ds3g_config_pdn_level_auth[subs_indx];
} /* ds3g_get_config_pdn_level_auth() */

#ifdef FEATURE_DATA_IS707
/*===========================================================================
FUNCTION      DS3G_GET_AUTH_INTERNAL_NV

DESCRIPTION   Returns Auth internal NV item

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_get_auth_internal_nv( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ds3g_is_auth_internal;
} /* ds3g_get_auth_internal_nv() */
#endif /* FEATURE_DATA_IS707 */

/*===========================================================================
FUNCTION      DS3G_GET_ENABLE_ARBITRATION

DESCRIPTION   Returns enable arbitartion NV item

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_get_enable_arbitration( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ds3g_enable_arbitration;
} /* ds3g_get_enable_arbitration() */

/*===========================================================================
FUNCTION      DS3G_GET_SUPPRESS_GSM_ON_SRVCC_CSFB

DESCRIPTION   Returns Suppress GSM on SRVCC/CSFB NV item

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_get_suppress_gsm_on_srvcc_csfb
(
  sys_modem_as_id_e_type  subs_id
)
{
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR, "Invalid subs ID %d",subs_id);
    return FALSE;
  }

  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g( subs_id );

  return ds3g_suppress_gsm_on_srvcc_csfb[subs_indx];
} /* ds3g_get_suppress_gsm_on_srvcc_csfb() */

/*===========================================================================
FUNCTION      DS3G_PKT_SYS_MODE_TO_SYS_SYS_MODE

DESCRIPTION   This function is a utility function to convert ds apn pkt 
              sys mode to sys sys mode.

DEPENDENCIES  None

RETURN VALUE  Mapped SYS mode : sys_sys_mode_e_type

SIDE EFFECTS  None
===========================================================================*/
sys_sys_mode_e_type ds3g_pkt_sys_mode_to_sys_sys_mode
(
  ds_pkt_sys_mode_e_type  ds_pkt_mode
)
{
  sys_sys_mode_e_type  sys_mode = SYS_SYS_MODE_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch(ds_pkt_mode)
  {
    case DS_PKT_SYS_MODE_NONE:
      sys_mode = SYS_SYS_MODE_NONE;
      break;

    case DS_PKT_SYS_MODE_NO_SRV:
      sys_mode = SYS_SYS_MODE_NO_SRV;
      break;

    case DS_PKT_SYS_MODE_AMPS:
      sys_mode = SYS_SYS_MODE_AMPS;
      break;

    case DS_PKT_SYS_MODE_CDMA:
      sys_mode = SYS_SYS_MODE_CDMA;
      break;

    case DS_PKT_SYS_MODE_GSM:
      sys_mode = SYS_SYS_MODE_GSM;
      break;

    case DS_PKT_SYS_MODE_HDR:
      sys_mode = SYS_SYS_MODE_HDR;
      break;

    case DS_PKT_SYS_MODE_WCDMA:
      sys_mode = SYS_SYS_MODE_WCDMA;
      break;

    case DS_PKT_SYS_MODE_GPS:
      sys_mode = SYS_SYS_MODE_GPS;
      break;

    case DS_PKT_SYS_MODE_GW:
      sys_mode = SYS_SYS_MODE_GW;
      break;

    case DS_PKT_SYS_MODE_WLAN:
      sys_mode = SYS_SYS_MODE_WLAN;
      break;

    case DS_PKT_SYS_MODE_LTE:
      sys_mode = SYS_SYS_MODE_LTE;
      break;

    case DS_PKT_SYS_MODE_GWL:
      sys_mode = SYS_SYS_MODE_GWL;
      break;

    case DS_PKT_SYS_MODE_TDS:
      sys_mode = SYS_SYS_MODE_TDS;
      break;
 
    default:
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "Invalid mapping from DS mode"
                      "to SYS mode, DS mode is %d",ds_pkt_mode);
      break;
  }

  return sys_mode;
} /* ds3g_pkt_sys_mode_to_sys_sys_mode() */

#ifdef FEATURE_DATA_LTE
/*===========================================================================
FUNCTION      DS3G_GET_DIV_DUPLEX_FROM_SO_MASK

DESCRIPTION   Utility function to get LTE div duplex from so mask.

DEPENDENCIES  None.

RETURN VALUE  The div duplex for LTE

SIDE EFFECTS  None.
===========================================================================*/
sys_div_duplex_e_type ds3g_get_div_duplex_from_so_mask
(
  uint32  so_mask
)
{
  sys_div_duplex_e_type  div_duplex;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (so_mask & DS_SYS_SO_EX_3GPP_LTE_FDD)
  {
      div_duplex = SYS_DIV_DUPLEX_FREQ;
  }
  else if (so_mask & DS_SYS_SO_EX_3GPP_LTE_TDD)
  {
      div_duplex = SYS_DIV_DUPLEX_TIME;
  }
  else
  {
      div_duplex = SYS_DIV_DUPLEX_NONE;
  }

  return div_duplex;
} /* ds3g_get_div_duplex_from_so_mask() */

/*===========================================================================
FUNCTION      DS3G_GET_SO_MASK_FROM_DIV_DUPLEX

DESCRIPTION   Utility function to get so mask from div duplex.

DEPENDENCIES  None

RETURN VALUE  The so mask for LTE

SIDE EFFECTS  None
===========================================================================*/
uint32 ds3g_get_so_mask_from_div_duplex
(
  sys_div_duplex_e_type  div_duplex
)
{
  uint32  so_mask = DS_SYS_SO_EX_UNSPECIFIED;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch(div_duplex)
  {
    case SYS_DIV_DUPLEX_NONE:
      break;

    case SYS_DIV_DUPLEX_FREQ:
      so_mask = DS_SYS_SO_EX_3GPP_LTE_FDD;
      break;

    case SYS_DIV_DUPLEX_TIME:
      so_mask = DS_SYS_SO_EX_3GPP_LTE_TDD;
      break;

    default:
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "div duplex value [%d] unrecognized!", 
                      div_duplex);
      break;
  }

  return so_mask;
} /* ds3g_get_so_mask_from_div_duplex() */
#endif /* FEATURE_DATA_LTE */

/*===========================================================================
FUNCTION      DS3G_CONVERT_DSD_RADIO_INFO

DESCRIPTION   This is the function to convert dsd radio info to ps status

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_convert_dsd_radio_info
(
  dsd_radio_type                   radio_idx,
  ds_dsd_ext_radio_info_type      *dsd_radio_info,
  ps_sys_subscription_enum_type    ps_subs_id,
  ps_sys_system_status_info_type  *dsd_sys_info
)
{
  sys_modem_as_id_e_type  subs_id;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (dsd_radio_info == NULL || dsd_sys_info == NULL)
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                    "NULL ptr detected dsd_radio_info:0x%x dsd_sys_info:0x%x",
                    dsd_radio_info, dsd_sys_info);
    return;
  }

  DATA_3GMGR_MSG3(MSG_LEGACY_HIGH,
                  "RAT mask 0x%x SO mask 0x%lx for radio_idx %d",
                  dsd_radio_info->rat_mask,dsd_radio_info->so_mask,radio_idx);

  /* Convert PS subs ID to CM subs id */
  subs_id = ds3gsubsmgr_subs_id_ds_to_cm(
              (ds_sys_subscription_enum_type)ps_subs_id);

  if(0 == dsd_radio_info->rat_mask)
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                    "RAT mask %d invalid for radio %d",
                    dsd_radio_info->rat_mask, 
                    radio_idx);
    return;
  }

  switch(radio_idx)
  {
    case DS_DSD_RADIO_1X:
      dsd_sys_info->technology = PS_SYS_NETWORK_3GPP2;

      if( PS_SYS_RAT_3GPP2_NULL_BEARER == 
           ( dsd_radio_info->rat_mask & PS_SYS_RAT_3GPP2_NULL_BEARER ) )
      {
        dsd_sys_info->rat_value = PS_SYS_RAT_EX_NULL_BEARER;
      }
      else
      {
        dsd_sys_info->rat_value = PS_SYS_RAT_EX_3GPP2_1X;
      }

      if(PS_SYS_SO_3GPP2_1X_IS95 == 
        (dsd_radio_info->so_mask & PS_SYS_SO_3GPP2_1X_IS95))
      {
        dsd_sys_info->so_mask |= PS_SYS_SO_EX_3GPP2_1X_IS95;
      }

      if(PS_SYS_SO_3GPP2_1X_IS2000 == 
        (dsd_radio_info->so_mask & PS_SYS_SO_3GPP2_1X_IS2000))
      {
        dsd_sys_info->so_mask |= PS_SYS_SO_EX_3GPP2_1X_IS2000;
      }

      if(PS_SYS_SO_3GPP2_1X_IS2000_REL_A == 
        (dsd_radio_info->so_mask & PS_SYS_SO_3GPP2_1X_IS2000_REL_A))
      {
        dsd_sys_info->so_mask |= PS_SYS_SO_EX_3GPP2_1X_IS2000_REL_A;
      }
      break;

    case DS_DSD_RADIO_GSM:
    case DS_DSD_RADIO_WCDMA:
    case DS_DSD_RADIO_TDSCDMA:
    case DS_DSD_RADIO_LTE:
      dsd_sys_info->technology = PS_SYS_NETWORK_3GPP;

      /* if the passed radio is not the preferred radio, then NULL BEARER */
      if ((PS_SYS_RAT_3GPP_NULL_BEARER == 
           (dsd_radio_info->rat_mask & PS_SYS_RAT_3GPP_NULL_BEARER)))
      {
        dsd_sys_info->rat_value = PS_SYS_RAT_EX_NULL_BEARER;
        dsd_sys_info->so_mask = PS_SYS_SO_EX_UNSPECIFIED;

        if( (radio_idx == DS_DSD_RADIO_LTE) &&
            (dsd_radio_info->srv_status == SYS_SRV_STATUS_LIMITED) )
        {
          dsd_sys_info->so_mask |= PS_SYS_SO_EX_3GPP_LTE_LIMITED_SRVC;
        }
      }
      else
      {
        if(radio_idx == DS_DSD_RADIO_GSM) /* case DS_DSD_RADIO_GSM */
        {
          dsd_sys_info->rat_value = PS_SYS_RAT_EX_3GPP_GERAN;
        }
        else if(radio_idx == DS_DSD_RADIO_WCDMA) /* case DS_DSD_RADIO_WCDMA */
        {
          dsd_sys_info->rat_value = PS_SYS_RAT_EX_3GPP_WCDMA;
        }
        else if(radio_idx == DS_DSD_RADIO_LTE) /* case DS_DSD_RADIO_LTE */
        {
          dsd_sys_info->rat_value = PS_SYS_RAT_EX_3GPP_LTE;
        }
        else /* case DS_DSD_RADIO_TDSCDMA */
        {
          dsd_sys_info->rat_value = PS_SYS_RAT_EX_3GPP_TDSCDMA;
        }
        /* query 3GPP MH for so mask */
        dsd_sys_info->so_mask |= ds_3gpp_bearer_cntxt_get_so_mask(subs_id);
      }
      break;

    case DS_DSD_RADIO_HDR:
      dsd_sys_info->technology = PS_SYS_NETWORK_3GPP2;

      if( PS_SYS_RAT_3GPP2_NULL_BEARER == 
           ( dsd_radio_info->rat_mask & PS_SYS_RAT_3GPP2_NULL_BEARER ) )
      {
        dsd_sys_info->rat_value = PS_SYS_RAT_EX_NULL_BEARER;
      }
      else
      {
        dsd_sys_info->rat_value = PS_SYS_RAT_EX_3GPP2_HRPD;
      }

      if( ( PS_SYS_RAT_3GPP2_EHRPD == 
            (dsd_radio_info->rat_mask & PS_SYS_RAT_3GPP2_EHRPD) ) &&
            ( (dsd_radio_info->so_mask & PS_SYS_SO_3GPP2_EVDO_EMPA_EHRPD) ||
              (dsd_radio_info->so_mask & PS_SYS_SO_3GPP2_EVDO_MMPA_EHRPD) ) )
      {
        dsd_sys_info->rat_value = PS_SYS_RAT_EX_3GPP2_EHRPD;
      }

      if(PS_SYS_RAT_3GPP2_EVDO_REV0 == 
        (dsd_radio_info->rat_mask & PS_SYS_RAT_3GPP2_EVDO_REV0))
      {
        dsd_sys_info->so_mask |= PS_SYS_SO_EX_3GPP2_HDR_REV0_DPA;
      }

      if(PS_SYS_RAT_3GPP2_EVDO_REVA == 
        (dsd_radio_info->rat_mask & PS_SYS_RAT_3GPP2_EVDO_REVA))
      {
        if(PS_SYS_SO_3GPP2_EVDO_DPA == 
           (dsd_radio_info->so_mask & PS_SYS_SO_3GPP2_EVDO_DPA))
        {
          dsd_sys_info->so_mask |= PS_SYS_SO_EX_3GPP2_HDR_REVA_DPA;
        }
        if(PS_SYS_SO_3GPP2_EVDO_MFPA == 
           (dsd_radio_info->so_mask & PS_SYS_SO_3GPP2_EVDO_MFPA))
        {
          dsd_sys_info->so_mask |= PS_SYS_SO_EX_3GPP2_HDR_REVA_MPA;
        }
        if(PS_SYS_SO_3GPP2_EVDO_EMPA == 
           (dsd_radio_info->so_mask & PS_SYS_SO_3GPP2_EVDO_EMPA))
        {
          dsd_sys_info->so_mask |= PS_SYS_SO_EX_3GPP2_HDR_REVA_EMPA;
        }
        if(PS_SYS_SO_3GPP2_EVDO_EMPA_EHRPD == 
           (dsd_radio_info->so_mask & PS_SYS_SO_3GPP2_EVDO_EMPA_EHRPD))
        {
          dsd_sys_info->rat_value = PS_SYS_RAT_EX_3GPP2_EHRPD;
          dsd_sys_info->so_mask |= PS_SYS_SO_EX_3GPP2_HDR_REVA_EMPA;
        }
      }

      if(PS_SYS_RAT_3GPP2_EVDO_REVB == 
        (dsd_radio_info->rat_mask & PS_SYS_RAT_3GPP2_EVDO_REVB))
      {
        if(PS_SYS_SO_3GPP2_EVDO_DPA == 
           (dsd_radio_info->so_mask & PS_SYS_SO_3GPP2_EVDO_DPA))
        {
          dsd_sys_info->so_mask |= PS_SYS_SO_EX_3GPP2_HDR_REVB_DPA;
        }
        if(PS_SYS_SO_3GPP2_EVDO_MFPA == 
           (dsd_radio_info->so_mask & PS_SYS_SO_3GPP2_EVDO_MFPA))
        {
          dsd_sys_info->so_mask |= PS_SYS_SO_EX_3GPP2_HDR_REVB_MPA;
        }
        if(PS_SYS_SO_3GPP2_EVDO_EMPA == 
           (dsd_radio_info->so_mask & PS_SYS_SO_3GPP2_EVDO_EMPA))
        {
          dsd_sys_info->so_mask |= PS_SYS_SO_EX_3GPP2_HDR_REVB_EMPA;
        }
        if(PS_SYS_SO_3GPP2_EVDO_EMPA_EHRPD == 
           (dsd_radio_info->so_mask & PS_SYS_SO_3GPP2_EVDO_EMPA_EHRPD))
        {
          dsd_sys_info->rat_value = PS_SYS_RAT_EX_3GPP2_EHRPD;
          dsd_sys_info->so_mask |= PS_SYS_SO_EX_3GPP2_HDR_REVB_EMPA;
        }
        if(PS_SYS_SO_3GPP2_EVDO_MMPA == 
           (dsd_radio_info->so_mask & PS_SYS_SO_3GPP2_EVDO_MMPA))
        {
          dsd_sys_info->so_mask |= PS_SYS_SO_EX_3GPP2_HDR_REVB_MMPA;
        }
        if(PS_SYS_SO_3GPP2_EVDO_MMPA_EHRPD == 
           (dsd_radio_info->so_mask & PS_SYS_SO_3GPP2_EVDO_MMPA_EHRPD))
        {
          dsd_sys_info->rat_value = PS_SYS_RAT_EX_3GPP2_EHRPD;
          dsd_sys_info->so_mask |= PS_SYS_SO_EX_3GPP2_HDR_REVB_MMPA;
        }
      }

      if(PS_SYS_RAT_3GPP2_FMC == 
        (dsd_radio_info->rat_mask & PS_SYS_RAT_3GPP2_FMC))
      {
        dsd_sys_info->so_mask |= PS_SYS_SO_EX_3GPP2_EVDO_FMC;
      }

      break;

    default:
      DATA_MSG0_ERROR("ds_mppm_convert_dsd_radio_info: Unknown old radio");
      break;
  }

  DATA_3GMGR_MSG3(MSG_LEGACY_HIGH,
                 "ds3g_convert_dsd_radio_info: RAT mask 0x%x SO mask 0x%lx"
                 " for Technology %d",
                 dsd_sys_info->rat_value, 
                 dsd_sys_info->so_mask, 
                 dsd_sys_info->technology);

  return;
} /* ds3g_convert_dsd_radio_info() */

/*===========================================================================
FUNCTION      DS3G_UPDATE_DATA_CONFIG_INFO

DESCRIPTION   Puts all the required nv item file paths in the 
              data_config_info.conf file. Would be called during powerup.
 
DEPENDENCIES  None

RETURN VALUE  length of string added to buffer for success. 
              -1 for failure.

SIDE EFFECTS  None
===========================================================================*/
int32 ds3g_update_data_config_info
(
  char   *file_paths_buf,
  uint32  efs_conf_file_size
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (NULL == file_paths_buf)
  {
    DATA_3GMGR_MSG0( MSG_LEGACY_LOW, "file_paths_buf is NULL");
    return -1;
  }

  /* Concatenate string to buffer */
  return strlcat(file_paths_buf,
                 ds3g_nv_item_file_path,
                 efs_conf_file_size);
} /* ds3g_update_data_config_info() */

/*===========================================================================
FUNCTION      DS3G_GET_EFS_FILE_PATH_SIZE

DESCRIPTION   Returns the total ds3g EFS item file paths size

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
uint32 ds3g_get_efs_file_path_size( void )
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  return sizeof(ds3g_nv_item_file_path);
} /* ds3g_get_efs_file_path_size() */
/*===========================================================================
FUNCTION      DS3G_GET_CONFIG_LATENCY_INFO

DESCRIPTION   Returns config latency info

DEPENDENCIES  None

RETURN VALUE  TRUE/FALSE -indicates the latency info nv value

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_get_config_latency_info()
{
  return ds3g_config_latency;
} 
