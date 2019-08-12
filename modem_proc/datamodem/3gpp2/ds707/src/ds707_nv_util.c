/*===========================================================================
                        DS707 NV UTIL
                        
GENERAL DESCRIPTION
  This module is responsible for handling NV reads for NV items
  that are necessary for task_init during sim swap in NV refresh
  event


EXTERNALIZED FUNCTIONS
  

INITIALIZATION AND SEQUENCING REQUIREMENTS
  
  Copyright (c) 2013 - 2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_nv_util.c#1 $
  $DateTime: 2016/12/13 08:00:03 $
  $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/17/14    niv     Created Module
===========================================================================*/

/*===========================================================================
                          INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "ttl_map.h"

#ifdef FEATURE_DATA_LTE_OFFTARGET_TEST
#include "dsut.h"
#include "lte_tlb.h"
#include "dslteps_ut_msg.h"
#endif /* FEATURE_DATA_LTE_OFFTARGET_TEST */

#include "amssassert.h"
#include "msg.h"
#include "err.h"
#include "nv.h"
#include "rex.h"
#include "task.h"
#include "queue.h"
#include "data_msg.h"
#include "ds_epc_timer.h"
#include "ds_epc_pdn_ctl.h"
#include "dsutil.h"

#ifdef  FEATURE_DS_EPC_LOGGING
#include "ds_epc_logging.h"
#endif /*FEATURE_DS_EPC_LOGGING*/

#include "modem_mem.h"
#include "task.h"
#include "dsutil.h"
#include "rcinit_rex.h"

#ifdef FEATURE_DATA_PS_IPV6
#include "dsgen_iface6_hdlr.h"
#ifdef FEATURE_DATA_IS707
#include "ds707_ifaceAny_hdlr.h"
#include "ds707_async_mgr.h"
#include "ds707_so_async.h"
#include "ds707_so_g3fax.h"
#include "ds707_pkt_mgr.h"
#include "ds_profile_3gpp2_mgr.h"
#include "ds707_devmgr.h"
#include "ds707_nv_util.h"
#include "ds707_data_session_profile.h"
#include "ds707_so_pkt.h"
#include "ds707_so_jcdma.h"
#include "ds707_jcdma_m51.h"
#include "dsgen_iface_hdlr.h"
#include "ds707_drs.h"
#include "ds707_epzid_mgr.h"
#include "ds707_pdn_context.h"
#include "ds707_roaming.h"
#endif/* FEATURE_DATA_IS707 */
#endif/* FEATURE_DATA_PS_IPV6 */

#include "ds_3gpp_hdlr.h"

#ifdef FEATURE_DATA_EMBMS
#include "ds_3gpp_embms_hdlr.h"
#endif /* FEATURE_DATA_EMBMS */

#include "dstask_v.h"
#include "dstaski.h"
#include "ds3gmgr.h"
#include "ds3gcfgmgr.h"
#include "dsat_v.h"
#include "dsati.h"
#include "dog_hb_rex.h"
#include "ds_profile_db_internal.h"

#ifdef FEATURE_DS_RDUD
#include "dsrdud.h"
#endif /* FEATURE_DS_RDUD */

#ifdef FEATURE_HDR_QOS
#include "dsrlpi.h" 
#include "ds707_sec_pkt_mgr.h"
#include "ds707_sec_pkt_mgr_util.h"
#include "ds707_sec_pkt_mgr_handoff.h"
#include "hdrmrlpctl.h"
#ifdef FEATURE_NIQ_EHRPD
#include "ds707_nw_init_qos_mgr.h"
#endif /*FEATURE_NIQ_EHRPD*/
#ifdef FEATURE_HDR_EMPA
#include "hdrrsp.h"
#endif /* FEATURE_HDR_EMPA */
#endif /* FEATURE_HDR_QOS */


#ifdef FEATURE_DATA_OPTHO
#include "ds707_s101.h"
#include "hdrcom_api.h"
#include "ds707_s101_vctm.h"
#include "ds707_s101_dctm.h"
#endif /* FEATURE_DATA_OPTHO */

#ifdef FEATURE_DATA_MIP_AUTH_FAIL_CLOSE_SESS
#define MIP_FAIL_DO_SESS_CLOSE_NV_PATH \
     "/nv/item_files/data/3gpp2/mip_fail_do_sess_close"
#endif /*FEATURE_DATA_MIP_AUTH_FAIL_CLOSE_SESS*/

#ifdef FEATURE_HDR
#include "hdrds.h"
#include "hdrpac.h"
#include "dshdr_an_mgr.h"
#include "dshdr_dhcp_mgr.h"
#ifdef FEATURE_DS_CAM
#include "dscam.h"
#endif /* FEATURE_DS_CAM */
#endif /* FEATURE_HDR */

#ifdef FEATURE_DATA_LTE_OFFTARGET_TEST
#include "ds3gpp_ext_msg.h"
#endif
#ifdef FEATURE_DATA_TEST_LOOPBACK
#include "ds_loopback_hdlr.h"
#endif /* FEATURE_DATA_TEST_LOOPBACK */

#ifdef FEATURE_EPC_HANDOFF
#include "ds_epc_hdlr.h"
#endif /*FEATURE_EPC_HANDOFF*/

#include "ds_dsd_ext_i.h"
#include "ds_dsd_apm_ext_i.h"

#ifdef FEATURE_DATA_WLAN_MAPCON
#include "ds_mppm_ext_i.h"
#include "ds_eps_kamgr_ext.h"
#include "ds_wlan_proxy_mode_hdlr.h"
#include "ds_epc_ho_throttle_mgr.h"
#endif /* FEATURE_DATA_WLAN_MAPCON */


#ifdef FEATURE_DATA_IWLAN_S2B
#include "ds_iwlan_s2b_ext.h"
#endif /* FEATURE_DATA_IWLAN_S2B */

#include "ds3gstkproc.h"
#include "ds3gdsdif.h"
#include "ds3gcsdhdlr.h"
#include "ds3gtimer.h"
#include "sm_mgr.h"

#ifdef FEATURE_EHRPD
#include "ds707_ps_evt_hdlr.h"
#endif /* FEATURE_EHRPD */

#include "ds_3gpp_profile_api.h"
#include "ds_epc_sm_defs.h"
#include "ds_1x_profile.h"

/*===========================================================================
                          VARIABLES AND MACRO DEFINITIONS
===========================================================================*/
#define DISABLE_CS_DATA_SERVICE_NV_PATH \
			"/nv/item_files/data/3gpp2/disable_cs_data_service"


/*===========================================================================
                              GLOBAL VARIABLES
===========================================================================*/
static list_type ds707_nv_refresh_list;
uint32 num_of_fptrs = 0;
static boolean ds707_nv_list_valid = FALSE;
static boolean ds707_nv_refresh_in_progress[DS3GSUBSMGR_SUBS_ID_MAX] = { FALSE };
static boolean mipv6_nv_status = FALSE; //MIPV6 calls currently NOT supported
/*===========================================================================

                            EXTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION       DS707_NV_UTIL_INIT

DESCRIPTION    Initialize the queue ds707_nv_refresh_list
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void ds707_nv_util_init()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
   /*------------------------------------------------------------------------------
   Init the NV Refresh list
   --------------------------------------------------------------------------------*/
   (void)list_init(&ds707_nv_refresh_list);
}/* ds707_nv_util_init */


/*===========================================================================
FUNCTION       DS707_NV_UTIL_INSERT_LIST

DESCRIPTION    Store the function pointers to read the NV and EFS later during NV REFRESH
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void ds707_nv_util_insert_list(void (*fptr)())
{
  ds707_nv_util_refresh_func_type *fp = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*------------------------------------------------------------------------------
  Check if the NV list has already been populated by the NV read function pointers
  --------------------------------------------------------------------------------*/
  if ( ds707_nv_list_valid == TRUE )
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH,"NV list already completed");
    return;
  }

  /*------------------------------------------------------------------------------
  Allocate memory for a new function pointer in the NV refresh 
  list 
  --------------------------------------------------------------------------------*/
  fp = (void*)modem_mem_alloc(sizeof(ds707_nv_util_refresh_func_type),
                                       MODEM_MEM_CLIENT_DATA);

  /*------------------------------------------------------------------------------
  Validate the memory allocation
  --------------------------------------------------------------------------------*/
  if( fp == NULL ) 
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "Allocate memory failed for node of nv list");
    return;
  }

  memset((void*)fp, 0, sizeof(ds707_nv_util_refresh_func_type));

  /*------------------------------------------------------------------------------
  Add the new function pointer to the back of the list
  --------------------------------------------------------------------------------*/
  fp->funcptr = fptr;
  list_push_back(&ds707_nv_refresh_list, &(fp->link) );

  num_of_fptrs =(uint32)list_size(&ds707_nv_refresh_list);
  return;
} /* ds707_nv_util_insert_list */

/*==================================================================================
FUNCTION       DS707_NV_UTIL_NVREFRESH

DESCRIPTION    Wrapper function to read the linked list containing function pointers 
               for the NV and EFS items
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/

void ds707_nv_util_nvrefresh(sys_modem_as_id_e_type subs_id)
{
  ds707_nv_util_refresh_func_type *fp = NULL;

/*-----------------------------------------------------------------------------------*/
  if(subs_id >= DS3GSUBSMGR_SUBS_ID_MAX)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,"Invalid subs id");
    return;
  }
  
  ds707_nv_refresh_in_progress[subs_id]= TRUE;

  ds707_pkt_mgr_local_abort(subs_id,PS_NET_DOWN_REASON_NV_REFRESH_IN_PROGRESS);

  /*-------------------------------------------------------------------------------------------
   Read the ds707_nv_refresh_list till the last function pointer  
   ---------------------------------------------------------------------------------------------*/   
  fp =(ds707_nv_util_refresh_func_type *)
                              list_peek_front(&ds707_nv_refresh_list);
  while (fp) 
  {
    fp->funcptr();
    fp = (ds707_nv_util_refresh_func_type *)list_peek_next(&ds707_nv_refresh_list,&(fp->link));                                        
  }

  ds707_nv_util_refresh_complete(subs_id);

  return;
}/* ds707_nv_util_nvrefresh */


/*==================================================================================
FUNCTION       DS707_NV_UTIL_SO_ASYNC_INIT

DESCRIPTION    Wrapper function for aync data service options to 
               be done in power up 
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/
void ds707_nv_util_so_async_init(void)
{
/*-----------------------------------------------------------------------------------*/

#ifdef FEATURE_DATA_IS707_ASYNC
   ds707_so_async_init();
#ifdef FEATURE_MODEM_CONFIG_REFRESH
   /*-------------------------------------------------------------------------------------------
   Write the function pointers in the NV List
   ---------------------------------------------------------------------------------------------*/
  ds707_nv_util_insert_list(&ds707_so_async_init);  
#endif /* FEATURE_MODEM_CONFIG_REFRESH  */
#endif /* FEATURE_DATA_IS707_ASYNC  */
  return;
}/* ds707_nv_util_so_async_init */


/*==================================================================================
FUNCTION       DS707_NV_UTIL_SO_G3FAX_INIT

DESCRIPTION    Wrapper function for init of Group 3 fax service 
               options to be done during power up 
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/

void ds707_nv_util_so_g3fax_init(void)
{
/*-----------------------------------------------------------------------------------*/

#ifdef FEATURE_DATA_IS707_ASYNC
   ds707_so_g3fax_init();
#ifdef FEATURE_MODEM_CONFIG_REFRESH
   /*-------------------------------------------------------------------------------------------
   Write the function pointers in the NV List
   ---------------------------------------------------------------------------------------------*/
  ds707_nv_util_insert_list(&ds707_so_g3fax_init);  
#endif /* FEATURE_MODEM_CONFIG_REFRESH */
#endif /* FEATURE_DATA_IS707_ASYNC */
  return;
}/* ds707_nv_util_so_g3fax_init */


/*==================================================================================
FUNCTION       DS707_NV_UTIL_1X_PROFILE_INIT

DESCRIPTION    Wrapper function for init of static and runtime 
               structs for 1x profile 
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/
void ds707_nv_util_1x_profile_init(void)
{
/*-----------------------------------------------------------------------------------*/
   ds_1x_profile_init();  

#ifdef FEATURE_MODEM_CONFIG_REFRESH
   /*-----------------------------------------------------------------------------------
   Write the function pointers in the NV List
   -------------------------------------------------------------------------------------*/
  ds707_nv_util_insert_list(&ds_1x_profile_init);  
#endif /* FEATURE_MODEM_CONFIG_REFRESH */
  return;
}/* ds707_nv_util_1x_profile_init */

/*==================================================================================
FUNCTION       DS707_NV_UTIL_TRAFFIC_CH_TEARDOWN_EFS 

DESCRIPTION     
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/
void ds707_nv_util_traffic_ch_teardown_efs(void)
{
  ds707_pkt_mgr_traffic_ch_teardown_efs();

#ifdef FEATURE_MODEM_CONFIG_REFRESH
   /*-----------------------------------------------------------------------------------
   Write the function pointers in the NV List
   -------------------------------------------------------------------------------------*/
  ds707_nv_util_insert_list(&ds707_pkt_mgr_traffic_ch_teardown_efs);  
#endif /* FEATURE_MODEM_CONFIG_REFRESH */
  return;
}

/*==================================================================================
FUNCTION       DS707_NV_UTIL_READ_ASYNC_NV

DESCRIPTION    Read the EFS item for DISABLE CS DATA SERVICE
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/
void ds707_nv_util_read_async_nv(void)
{
   static boolean  disable_cs_data_service = TRUE;
   nv_stat_enum_type         read_status = NV_DONE_S;
/*-----------------------------------------------------------------------------------*/
  read_status = ds3gcfgmgr_read_efs_nv_ex(DISABLE_CS_DATA_SERVICE_NV_PATH, 
                               &disable_cs_data_service, 
                               sizeof(boolean), ds707_curr_subs_id());

#ifdef FEATURE_DATA_IS707_ASYNC
  ds707_async_set_cs_data_service(disable_cs_data_service);
#endif /* FEATURE_DATA_IS707_ASYNC */
  return;
}/* ds707_nv_util_read_async_nv */

#ifdef FEATURE_DATA_PS_IPV6
/*==================================================================================
FUNCTION       DS707_NV_UTIL_READ_MIPV6_NV

DESCRIPTION    Read the MIPv6 Behavior NV item & deny iface for 
               v6 call 
 
DEPENDENCIES   None. 

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/
void ds707_nv_util_read_mipv6_nv(void)
{
  nv_item_type         nv_item;
  nv_stat_enum_type    nv_status;
/*-----------------------------------------------------------------------------------*/
  nv_status = ds3gcfgmgr_read_legacy_nv( NV_MIPV6_BEHAVIOR_I, &nv_item );
  if (nv_status==NV_DONE_S && (nv_item.mipv6_behavior == NV_MIPV6_REQUIRED \
                     || nv_item.mipv6_behavior == NV_MIPV6_TO_SIPV6_FAILOVER )) 
  {
    DATA_IS707_MSG0 (MSG_LEGACY_ERROR, "MIP NV is enabled.");
    mipv6_nv_status = TRUE;
  }
} /* ds707_nv_util_read_mipv6_nv */

/*==================================================================================
FUNCTION       DS707_NV_UTIL_GET_MIPV6_NV

DESCRIPTION    Return status of MIPv6 Behavior NV Item  
 
DEPENDENCIES   None. 

RETURN VALUE   TRUE: MIPV6 is not DISABLED 
               FALSE: MIPV6 is DISABLED 

SIDE EFFECTS   None

=====================================================================================*/
boolean ds707_nv_util_get_mipv6_nv(void)
{
  return mipv6_nv_status;
} /* ds707_nv_util_get_mipv6_nv */
#endif /* FEATURE_DATA_PS_IPV6 */

/*==================================================================================
FUNCTION       DS707_NV_UTIL_READ_PKT_MGR_NV_SYNC 

DESCRIPTION    Wrapper function for 707 specific NVs
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/

void ds707_nv_util_read_pkt_mgr_nv_sync(void)
{
/*-----------------------------------------------------------------------------------*/

   /*-------------------------------------------------------------------------------------------
   Read 707_specific NV items    
   ---------------------------------------------------------------------------------------------*/    
   ds707_pkt_mgr_nv_sync();

#ifdef FEATURE_MODEM_CONFIG_REFRESH
   /*-------------------------------------------------------------------------------------------
   Write the function pointers in the NV List
   ---------------------------------------------------------------------------------------------*/
   ds707_nv_util_insert_list(&ds707_pkt_mgr_nv_sync);   
#endif /* FEATURE_MODEM_CONFIG_REFRESH  */

  return;
}/* ds707_nv_util_read_pkt_mgr_nv_sync */


/*==================================================================================
FUNCTION       DS707_NV_UTIL_INIT_JCDMA_READ_NV

DESCRIPTION    This function is used to init JCDMA mode and go null timer.
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/
void ds707_nv_util_init_jcdma_read_nv(void)
{
/*-----------------------------------------------------------------------------------*/

   /*----------------------------------------------------------------------
   Initialize JCDMA mode and go-null timer
   ------------------------------------------------------------------------*/
   ds707_jcdma_mode_gonulltmr_init();

#ifdef FEATURE_MODEM_CONFIG_REFRESH
   /*-------------------------------------------------------------------------------------------
   Write the function pointers in the NV List
   ---------------------------------------------------------------------------------------------*/
   ds707_nv_util_insert_list(&ds707_jcdma_mode_gonulltmr_init); 
#endif /* FEATURE_MODEM_CONFIG_REFRESH  */

   return;
}/* ds707_nv_util_read_jcdma_nv */


/*==================================================================================
FUNCTION       DS707_NV_UTIL_READ_DATA_SESSION_PROFILE_CACHE_INIT 

DESCRIPTION    This function is a wrapper funtion to set all 
               profile related NVs 
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/
void ds707_nv_util_read_data_session_profile_cache_init(void)
{
/*-----------------------------------------------------------------------------------*/

  /*----------------------------------------------------------------------
  Initialize data session profile strcture.
  ------------------------------------------------------------------------*/
  ds707_data_session_profile_init(); 
   
  /*------------------------------------------------------------------------
  Set profile parameters to custom values depending on EFS files 
  --------------------------------------------------------------------*/
  ds707_data_session_set_profile();
   
#ifdef FEATURE_EHRPD
  ds707_data_session_disallow_call_nv_init();
  ds707_data_session_init_mandatory_apn_list();
#endif /* FEATURE_EHRPD */


#ifdef FEATURE_MODEM_CONFIG_REFRESH
  ds707_nv_util_insert_list(&ds707_data_session_profile_init); 
  ds707_nv_util_insert_list(&ds707_data_session_set_profile); 
  
#ifdef FEATURE_EHRPD
  ds707_nv_util_insert_list(&ds707_data_session_read_disallow_call_nv); 
  ds707_nv_util_insert_list(&ds707_data_session_init_mandatory_apn_list); 
#endif /* FEATURE_EHRPD */
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

  return;
}/* ds707_nv_util_read_data_session_profile_cache_init */


/*==================================================================================
FUNCTION       DS707_NV_UTIL_DS707_DATA_SESSION_CHECK_MANDATORY_APNS 

DESCRIPTION    Wrapper function for checking the list of enabled 
               mandatory apns in the profile
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/

void ds707_nv_util_data_session_check_mandatory_apns(void)
{

/*-----------------------------------------------------------------------------------*/

   /*-------------------------------------------------------------------------------------------
   Read the list of enabled mandatory apns and set ehrpd_allowed 
   flag accordingly
   ---------------------------------------------------------------------------------------------*/    
   ds707_data_session_check_mandatory_apns();


#ifdef FEATURE_MODEM_CONFIG_REFRESH
   /*-------------------------------------------------------------------------------------------
   Write the function pointers in the NV List
   ---------------------------------------------------------------------------------------------*/
   ds707_nv_util_insert_list(&ds707_data_session_check_mandatory_apns); 
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

   return;
}/* ds707_nv_util_ds707_data_session_check_mandatory_apns */


/*==================================================================================
FUNCTION       ds707_nv_util_read_dsrdud_nv

DESCRIPTION   Wrapper function for reading the inactivity 
              restoration time and setting the NV.
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/
void ds707_nv_util_read_dsrdud_nv(void)
{
/*-----------------------------------------------------------------------------------*/

   /*-------------------------------------------------------------------------------------------
   Read the inactivity restoration time and set NV   
   ---------------------------------------------------------------------------------------------*/    
   dsrdud_read_nv();

#ifdef FEATURE_MODEM_CONFIG_REFRESH
   /*-------------------------------------------------------------------------------------------
   Write the function pointers in the NV List
   ---------------------------------------------------------------------------------------------*/
   ds707_nv_util_insert_list(&dsrdud_read_nv); 
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

   return;
}/* ds707_nv_util_read_dsrdud_nv */

/*==================================================================================
FUNCTION       DS707_NV_UTIL_QCMIP_SET_CRM_VAL

DESCRIPTION    Wrapper function for setting the CRM value
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/
void ds707_nv_util_qcmip_set_crm_val(void)
{
/*-----------------------------------------------------------------------------------*/

   /*-------------------------------------------------------------------------------------------
   Set the CRM value   
   ---------------------------------------------------------------------------------------------*/    
   ds707_pkt_mgr_qcmip_set_crm_val();

#ifdef FEATURE_MODEM_CONFIG_REFRESH
   /*-------------------------------------------------------------------------------------------
   Write the function pointers in the NV List
   ---------------------------------------------------------------------------------------------*/
   ds707_nv_util_insert_list(&ds707_pkt_mgr_qcmip_set_crm_val); 
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

   return;
}/* ds707_nv_util_qcmip_set_crm_val */


/*==================================================================================
FUNCTION       DS707_NV_UTIL_QCMIP_SET_MDR_VAL

DESCRIPTION    Wrapper function for checking the QCMIP and 
               setting the MDR value 

 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/
void ds707_nv_util_qcmip_set_mdr_val(void)
{
/*-----------------------------------------------------------------------------------*/


   /*-------------------------------------------------------------------------------------------
   Set the MDR value   
   ---------------------------------------------------------------------------------------------*/    
   ds707_pkt_mgr_qcmip_set_mdr_val();

#ifdef FEATURE_MODEM_CONFIG_REFRESH
   /*-------------------------------------------------------------------------------------------
   Write the function pointers in the NV List
   ---------------------------------------------------------------------------------------------*/
   ds707_nv_util_insert_list(&ds707_pkt_mgr_qcmip_set_mdr_val); 
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

   return;
}/* ds707_nv_util_qcmip_set_mdr_val */


/*==================================================================================
FUNCTION       DS707_NV_UTIL_SO_PKT_INIT

DESCRIPTION    Wrapper function for packet data service options 
               during power up 
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/
void ds707_nv_util_so_pkt_init(void)
{
/*-----------------------------------------------------------------------------------*/

   /*-------------------------------------------------------------------------------------------
   Power up initialization for pkt data service options    
   ---------------------------------------------------------------------------------------------*/    
   ds707_so_pkt_init();


#ifdef FEATURE_MODEM_CONFIG_REFRESH
   /*-------------------------------------------------------------------------------------------
   Write the function pointers in the NV List
   ---------------------------------------------------------------------------------------------*/
   ds707_nv_util_insert_list(&ds707_so_pkt_init); 
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

   return;
}/* ds707_nv_util_so_pkt_init */


/*==================================================================================
FUNCTION       DS707_NV_UTIL_DS707_SO_JCDMA_INIT

DESCRIPTION    Wrapper function to initialize the JCDMA module 
               and register for 707 pkt events 
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/
void ds707_nv_util_ds707_so_jcdma_init(void)
{
/*-----------------------------------------------------------------------------------*/

   /*-------------------------------------------------------------------------------------------
   Initializes the JCDMA module and register for pkt events    
   ---------------------------------------------------------------------------------------------*/    
  (void)ds707_so_jcdma_init();

#ifdef FEATURE_MODEM_CONFIG_REFRESH
   /*-------------------------------------------------------------------------------------------
   Write the function pointers in the NV List
   ---------------------------------------------------------------------------------------------*/
   ds707_nv_util_insert_list(&ds707_so_jcdma_init); 
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

   return;
}/* ds707_nv_util_ds707_so_jcdma_init */


/*==================================================================================
FUNCTION       DS707_NV_UTIL_JCDMA_APPRATE_INIT

DESCRIPTION    Wrapper function to initialize the app rate 
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/
void ds707_nv_util_jcdma_apprate_init(void)
{
/*-----------------------------------------------------------------------------------*/

   /*-------------------------------------------------------------------------------------------
   Initializes the app rate to default [HIGH].    
   ---------------------------------------------------------------------------------------------*/    
   ds707_jcdma_apprate_init();

#ifdef FEATURE_MODEM_CONFIG_REFRESH
   /*-------------------------------------------------------------------------------------------
   Write the function pointers in the NV List
   ---------------------------------------------------------------------------------------------*/
   ds707_nv_util_insert_list(&ds707_jcdma_apprate_init); 
#endif /* FEATURE_MODEM_CONFIG_REFRESH */ 

   return;
}/* ds707_nv_util_jcdma_apprate_init */



/*==================================================================================
FUNCTION       DS707_NV_UTIL_READ_JCDMA_M51_INIT

DESCRIPTION    Wrapper function for initializing the JCDMA M51 
               related NVs
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/
void ds707_nv_util_read_jcdma_m51_init(void)
{
/*-----------------------------------------------------------------------------------*/

   /*-------------------------------------------------------------------------------------------
   Initializes the JCDMA M51 module.    
   ---------------------------------------------------------------------------------------------*/    
   ds707_jcdma_m51_init();

#ifdef FEATURE_MODEM_CONFIG_REFRESH
   /*-------------------------------------------------------------------------------------------
   Write the function pointers in the NV List
   ---------------------------------------------------------------------------------------------*/
   ds707_nv_util_insert_list(&ds707_jcdma_m51_init); 
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

   return;

}/* ds707_nv_util_read_jcdma_m51_init */



/*==================================================================================
FUNCTION       DS707_NV_UTIL_HDRPAC_DISABLE_1X_HANDDOWN_MODE

DESCRIPTION    Wrapper function for a function called when the 
               handdown to 1x disable IOCTL is called.
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/
void ds707_nv_util_hdrpac_disable_1x_handdown_mode()
{
/*-----------------------------------------------------------------------------------*/

   /*-------------------------------------------------------------------------------------------
   Called when handdown to 1x dsiable IOCTL is called    
   ---------------------------------------------------------------------------------------------*/    
   if(ds707_roaming_is_curr_mode_jcdma() == TRUE ||
      ds707_roaming_is_simplified_jcdma_mode() == TRUE )
   {
     hdrpac_disable_1x_handdown_mode();
   }
   else
   {
     DATA_IS707_MSG0(MSG_LEGACY_ERROR,"JCDMA not enabled");
   }
   return;
}/* ds707_nv_util_hdrpac_disable_1x_handdown_mode */

/*==================================================================================
FUNCTION       DS707_NV_UTIL_READ_INTERNAL_AUTH_NV

DESCRIPTION    Wrapper function for reading the internal auth NV 
               item and setting the static variable
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/
void ds707_nv_util_read_internal_auth_nv(void)
{
/*-----------------------------------------------------------------------------------*/

   /*-------------------------------------------------------------------------------------------
   Read internal auth NV    
   ---------------------------------------------------------------------------------------------*/    
   dsgen_iface_hdlr_init();

#ifdef FEATURE_MODEM_CONFIG_REFRESH
   /*-------------------------------------------------------------------------------------------
   Write the function pointers in the NV List
   ---------------------------------------------------------------------------------------------*/
   ds707_nv_util_insert_list(&dsgen_iface_hdlr_init); 
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

   return;
}/* ds707_nv_util_read_internal_auth_nv */


/*==================================================================================
FUNCTION       DS707_NV_UTIL_READ_DRS_NV

DESCRIPTION    Wrapper function for the init and NV reads for the 
               DSR algorithm
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/
void ds707_nv_util_read_drs_nv(void)
{
/*-----------------------------------------------------------------------------------*/

   /*-------------------------------------------------------------------------------------------
   Init the parameters for the DSR algorithm
   ---------------------------------------------------------------------------------------------*/    
   ds707_drs_init();

#ifdef FEATURE_MODEM_CONFIG_REFRESH
   /*-------------------------------------------------------------------------------------------
   Write the function pointers in the NV List
   ---------------------------------------------------------------------------------------------*/
   ds707_nv_util_insert_list(&ds707_drs_init); 
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

   return;
}/* ds707_nv_util_read_drs_nv */


/*==================================================================================
FUNCTION       DS707_NV_UTIL_NW_INIT_QOS_MGR_INIT

DESCRIPTION    Wrapper function for NV reads in the QOS module 
               during power up
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/
void ds707_nv_util_nw_init_qos_mgr_init(void)
{
/*-----------------------------------------------------------------------------------*/

   /*-------------------------------------------------------------------------------------------
   Init the QoS module and NV parameters
   ---------------------------------------------------------------------------------------------*/    
   ds707_nw_init_qos_mgr_init();

#ifdef FEATURE_MODEM_CONFIG_REFRESH
   /*-------------------------------------------------------------------------------------------
   Write the function pointers in the NV List
   ---------------------------------------------------------------------------------------------*/
   ds707_nv_util_insert_list(&ds707_nw_init_qos_mgr_init); 
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

   return;
}/* ds707_nv_util_nw_init_qos_mgr_init */


/*==================================================================================
FUNCTION       DS707_NV_UTIL_READ_EHRPD_TO_HRPD_FB_NV

DESCRIPTION    Wrapper function for reading the eHRPD to HRPD 
               fallback configuration related NV
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/
void ds707_nv_util_read_ehrpd_to_hrpd_fb_nv(void)
{
/*-----------------------------------------------------------------------------------*/

   /*-------------------------------------------------------------------------------------------
   Read eHRPD to HRPD fallback NV
   ---------------------------------------------------------------------------------------------*/    
   ds707_pkt_mgr_read_ehrpd_to_hrpd_fb_nv();

#ifdef FEATURE_MODEM_CONFIG_REFRESH
   /*-------------------------------------------------------------------------------------------
   Write the function pointers in the NV List
   ---------------------------------------------------------------------------------------------*/
   ds707_nv_util_insert_list(&ds707_pkt_mgr_read_ehrpd_to_hrpd_fb_nv); 
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

   return;
}/* ds707_nv_util_read_ehrpd_to_hrpd_fb_nv */


/*==================================================================================
FUNCTION       DS707_NV_UTIL_READ_DNS_V4_ADDR_REQ_NV

DESCRIPTION    Wrapper function for reading the dns v4 addr req 
               nv
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/
void ds707_nv_util_read_dns_v4_addr_req_nv(void)
{
/*-----------------------------------------------------------------------------------*/

   /*-------------------------------------------------------------------------------------------
   Read DNS v4 req NV 
   ---------------------------------------------------------------------------------------------*/    
   ds707_pkt_mgr_read_dns_v4_addr_req_nv();

#ifdef FEATURE_MODEM_CONFIG_REFRESH
   /*-------------------------------------------------------------------------------------------
   Write the function pointers in the NV List
   ---------------------------------------------------------------------------------------------*/
   ds707_nv_util_insert_list(&ds707_pkt_mgr_read_dns_v4_addr_req_nv); 
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

   return;
}/* ds707_nv_util_read_dns_v4_addr_req_nv */


/*==================================================================================
FUNCTION       DS707_NV_UTIL_SET_FAILOVER_IP_MODE

DESCRIPTION    Wrapper function to read the failover ip type 
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/
void ds707_nv_util_set_failover_ip_mode(void)
{
/*-----------------------------------------------------------------------------------*/

   /*-------------------------------------------------------------------------------------------
   Read failover IP type   
   ---------------------------------------------------------------------------------------------*/    
   ds707_pkt_mgr_set_failover_ip_mode();

#ifdef FEATURE_MODEM_CONFIG_REFRESH
   /*-------------------------------------------------------------------------------------------
   Write the function pointers in the NV List
   ---------------------------------------------------------------------------------------------*/
   ds707_nv_util_insert_list(&ds707_pkt_mgr_set_failover_ip_mode); 
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

   return;
}/* ds707_nv_util_set_failover_ip_mode */



/*==================================================================================
FUNCTION       DS707_NV_UTIL_HRPD_V6_ENABLED_NV

DESCRIPTION    Wrapper function to set the config file for HRPD 
               with IPv6 enabled 
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/
void ds707_nv_util_hrpd_v6_enabled_nv(void)
{
/*-----------------------------------------------------------------------------------*/

   /*-------------------------------------------------------------------------------------------
   Read EFS item and write into config file    
   ---------------------------------------------------------------------------------------------*/    
   ds707_pkt_mgr_read_hrpd_v6_enabled();

   ds707_nv_util_read_mipv6_nv();

#ifdef FEATURE_MODEM_CONFIG_REFRESH
   /*-------------------------------------------------------------------------------------------
   Write the function pointers in the NV List
   ---------------------------------------------------------------------------------------------*/
   ds707_nv_util_insert_list(&ds707_pkt_mgr_read_hrpd_v6_enabled); 
   ds707_nv_util_insert_list(&ds707_nv_util_read_mipv6_nv); 
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

   return;
}/* ds707_nv_util_hrpd_v6_enabled_nv */



/*==================================================================================
FUNCTION       DS707_NV_UTIL_READ_DSHDR_DHCP_NV

DESCRIPTION    Wrapper function to init the dhcp negotiation 
               options
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/
void ds707_nv_util_read_dshdr_dhcp_nv(void)
{
/*-----------------------------------------------------------------------------------*/

   /*-------------------------------------------------------------------------------------------
   Init DHCP negotiation options    
   ---------------------------------------------------------------------------------------------*/    
   dshdr_dhcp_powerup_init();

#ifdef FEATURE_MODEM_CONFIG_REFRESH
   /*-------------------------------------------------------------------------------------------
   Write the function pointers in the NV List
   ---------------------------------------------------------------------------------------------*/
   ds707_nv_util_insert_list(&dshdr_dhcp_powerup_init); 
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

   return;
}/* ds707_nv_util_read_dshdr_dhcp_nv */


/*==================================================================================
FUNCTION       DS707_NV_UTIL_SYS_CHG_HDLR_SILENT_REDIAL_INFO_INIT 

DESCRIPTION    Wrapper function for initializing the module
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None
=====================================================================================*/

void ds707_nv_util_sys_chg_hdlr_silent_redial_info_init(void)
{

/*-----------------------------------------------------------------------------------*/

   /*-------------------------------------------------------------------------------------------
   Init module    
   ---------------------------------------------------------------------------------------------*/    
   ds707_sys_chg_hdlr_silent_redial_info_init();

#ifdef FEATURE_MODEM_CONFIG_REFRESH
   /*-------------------------------------------------------------------------------------------
   Write the function pointers in the NV List
   ---------------------------------------------------------------------------------------------*/
   ds707_nv_util_insert_list(&ds707_sys_chg_hdlr_silent_redial_info_init); 
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

   return;
}/* ds707_nv_util_sys_chg_hdlr_silent_redial_info_init */


/*==================================================================================
FUNCTION       DS707_NV_UTIL_READ_NONEPC_CLEANUP_EFS

DESCRIPTION    Wrapper function for reading EFS items for NONEPC 
               call clean up 
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/
void ds707_nv_util_read_nonepc_call_cleanup_efs(void)
{
/*-----------------------------------------------------------------------------------*/

   /*-------------------------------------------------------------------------------------------
   Read EFS item for NONEPC call cleanup    
   ---------------------------------------------------------------------------------------------*/    
   ds707_sys_chg_hdlr_read_nonepc_call_cleanup();

#ifdef FEATURE_MODEM_CONFIG_REFRESH
   /*-------------------------------------------------------------------------------------------
   Write the function pointers in the NV List
   ---------------------------------------------------------------------------------------------*/
   ds707_nv_util_insert_list(&ds707_sys_chg_hdlr_read_nonepc_call_cleanup); 
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

   return;
}/* ds707_nv_util_read_nonepc_call_cleanup_efs */

#ifdef FEATURE_DATA_OPTHO
/*==================================================================================
FUNCTION       DS707_NV_UTIL_S101_VCTM_INIT

DESCRIPTION    Wrapper function to read and init the vctm data 
               structures
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/
void ds707_nv_util_s101_vctm_init(void)
{
/*-----------------------------------------------------------------------------------*/

   /*-------------------------------------------------------------------------------------------
   Init vctm parameters      
   ---------------------------------------------------------------------------------------------*/    
   ds707_s101_vctm_init();

#ifdef FEATURE_MODEM_CONFIG_REFRESH
   /*-------------------------------------------------------------------------------------------
   Write the function pointers in the NV List
   ---------------------------------------------------------------------------------------------*/
   ds707_nv_util_insert_list(&ds707_s101_vctm_init); 
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

   return;
}/* ds707_nv_util_s101_vctm_init */


/*==================================================================================
FUNCTION       DS707_NV_UTIL_S101_DCTM_INIT

DESCRIPTION    Wrapper function to read and init the dctm data 
               structures
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/
void ds707_nv_util_s101_dctm_init(void)
{
/*-----------------------------------------------------------------------------------*/

   /*-------------------------------------------------------------------------------------------
   Init dctm parameters      
   ---------------------------------------------------------------------------------------------*/    
   ds707_s101_dctm_init();

#ifdef FEATURE_MODEM_CONFIG_REFRESH
   /*-------------------------------------------------------------------------------------------
   Write the function pointers in the NV List
   ---------------------------------------------------------------------------------------------*/
   ds707_nv_util_insert_list(&ds707_s101_dctm_init); 
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

   return;
}/* ds707_nv_util_s101_dctm_init */
#endif /* FEATURE_DATA_OPTHO */

/*==================================================================================
FUNCTION       DS707_NV_UTIL_MIP_DO_SESS_CLOSE_NV

DESCRIPTION    Wrapper function to read NV for session close on 
               MIP failure
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/
void ds707_nv_util_mip_do_sess_close_nv(void)
{
/*-----------------------------------------------------------------------------------*/

   /*-------------------------------------------------------------------------------------------
   Read NV item     
   ---------------------------------------------------------------------------------------------*/    
   ds707_pkt_mgr_init_mip_do_sess_close_nv();

#ifdef FEATURE_MODEM_CONFIG_REFRESH
   /*-------------------------------------------------------------------------------------------
   Write the function pointers in the NV List
   ---------------------------------------------------------------------------------------------*/
   ds707_nv_util_insert_list(&ds707_pkt_mgr_init_mip_do_sess_close_nv); 
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

   return;
}/* ds707_nv_util_mip_do_sess_close_nv */


/*==================================================================================
FUNCTION       DS707_NV_UTIL_FB_LIST_INIT

DESCRIPTION    Wrapper function to init the parameters related to 
               fallback pdn list
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/
void ds707_nv_util_fb_list_init(void)
{
/*-----------------------------------------------------------------------------------*/

   /*-------------------------------------------------------------------------------------------
   Read NV item     
   ---------------------------------------------------------------------------------------------*/    
   ds707_pkt_mgr_fb_list_init();

#ifdef FEATURE_MODEM_CONFIG_REFRESH
   /*-------------------------------------------------------------------------------------------
   Write the function pointers in the NV List
   ---------------------------------------------------------------------------------------------*/
   ds707_nv_util_insert_list(&ds707_pkt_mgr_fb_list_init); 
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

   return;
}/* ds707_nv_util_fb_list_init */


/*==================================================================================
FUNCTION       DS707_NV_UTIL_ADDITIONAL_DIAL_STR_INIT

DESCRIPTION    Wrapper function to read the NV for additional 
               dial string provisioning
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/
void ds707_nv_util_additional_dial_str_init(void)
{
/*-----------------------------------------------------------------------------------*/

   /*-------------------------------------------------------------------------------------------
   Read NV item     
   ---------------------------------------------------------------------------------------------*/    
   ds707_pkt_mgr_additional_dial_str_init();

   
#ifdef FEATURE_MODEM_CONFIG_REFRESH
   /*-------------------------------------------------------------------------------------------
   Write the function pointers in the NV List
   ---------------------------------------------------------------------------------------------*/
   ds707_nv_util_insert_list(&ds707_pkt_mgr_additional_dial_str_init); 
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

   return;
}/* ds707_nv_util_additional_dial_str_init */


/*==================================================================================
FUNCTION       DS707_NV_UTIL_IGNORE_LAST_PDN_IA_TIMER_NV

DESCRIPTION    Wrapper function to read the NV 
               ignore_last_pdn_ia_timer 
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/
void ds707_nv_util_ignore_last_pdn_ia_timer_nv(void)
{
/*-----------------------------------------------------------------------------------*/

   /*-------------------------------------------------------------------------------------------
   Read NV item     
   ---------------------------------------------------------------------------------------------*/    
   ds707_pkt_mgr_init_ignore_last_pdn_ia_timer_nv();

#ifdef FEATURE_MODEM_CONFIG_REFRESH
   /*-------------------------------------------------------------------------------------------
   Write the function pointers in the NV List
   ---------------------------------------------------------------------------------------------*/
   ds707_nv_util_insert_list(&ds707_pkt_mgr_init_ignore_last_pdn_ia_timer_nv); 
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

   return;
}/* ds707_nv_util_ignore_last_pdn_ia_timer_nv */


/*==================================================================================
FUNCTION       DS707_NV_UTIL_READ_PDN_OVERRIDE_NV

DESCRIPTION    Wrapper function for pdn override config
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/
void ds707_nv_util_read_pdn_override_nv(void)
{
/*-----------------------------------------------------------------------------------*/

   /*-------------------------------------------------------------------------------------------
   Set the NV for disable pdn override type   
   ---------------------------------------------------------------------------------------------*/    
   ds707_pdncntx_read_pdn_override_nv();

#ifdef FEATURE_MODEM_CONFIG_REFRESH
   /*-------------------------------------------------------------------------------------------
   Write the function pointers in the NV List
   ---------------------------------------------------------------------------------------------*/
   ds707_nv_util_insert_list(&ds707_pdncntx_read_pdn_override_nv);
#endif /* FEATURE_MODEM_CONFIG_REFRESH */
   return;
}/* ds707_nv_util_read_pdn_override_nv */


/*==================================================================================
FUNCTION       DS707_NV_UTIL_READ_BP_ENABLED_NV

DESCRIPTION    Wrapper function to init the backpressure related 
               parameters 
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/
void ds707_nv_util_read_bp_enabled_nv(void)
{
/*-----------------------------------------------------------------------------------*/

   /*-------------------------------------------------------------------------------------------
   Set the parameters   
   ---------------------------------------------------------------------------------------------*/    
   ds707_pkt_mgr_read_bp_enabled_nv();

#ifdef FEATURE_MODEM_CONFIG_REFRESH
   /*-------------------------------------------------------------------------------------------
   Write the function pointers in the NV List
   ---------------------------------------------------------------------------------------------*/
   ds707_nv_util_insert_list(&ds707_pkt_mgr_read_bp_enabled_nv);
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

   return;
}/* ds707_nv_util_read_bp_enabled_nv */


#ifdef FEATURE_EPC_HANDOFF
/*==================================================================================
FUNCTION       DS707_NV_UTIL_READ_EPC_NV

DESCRIPTION    Init the EFS Handoff related parameters
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/

void ds707_nv_util_read_epc_nv(void)
{
  nv_stat_enum_type           read_efs_nv_retval;

#ifdef FEATURE_DATA_WLAN_MAPCON
  epc_ho_retry_nv_from_efs    ho_default_struct;
  char ho_retry_file_path[] = 
                       "/nv/item_files/data/3gpp2/epc_handoff_retry_count";
  
  rex_timer_cnt_type          ho_retry_timer_val = 0;
/*---------------------------------------------------------------*/
  /** Read the HO retry value duration value from the EFS item */
  memset(&read_efs_nv_retval,0,sizeof(read_efs_nv_retval));
  memset(&ho_default_struct, 0, sizeof(epc_ho_retry_nv_from_efs));
  read_efs_nv_retval = ds3gcfgmgr_read_efs_nv_ex(ho_retry_file_path, 
                                     &ho_default_struct, 
                                     sizeof(ho_default_struct), ds707_curr_subs_id());
   if (NV_DONE_S == read_efs_nv_retval)
   {
    ds_epc_iwlan_ho_retry_count = ho_default_struct.data_epc_ho_retry_value;
 
     DATA_EPC_MSG1(MSG_LEGACY_MED, 
                  "EFS item configured for EPC handoff retry count, value: %d",
                  ds_epc_iwlan_ho_retry_count);
   }
   else
   {
     /** Set EPC data context duration to default value */
    ds_epc_iwlan_ho_retry_count = DS_EPC_IWLAN_HO_RETRY_COUNT;
  
     DATA_EPC_MSG1(MSG_LEGACY_ERROR, 
                  "EFS item not configured for EPC iwlan handoff retry count, "
                  "using default value: %d", ds_epc_iwlan_ho_retry_count);
   }
  /*-------------------------------------------------------------------------
    Read EFS file for handoff throttle timer series
   -------------------------------------------------------------------------*/
  ds_epc_ho_throttle_mgr_read_timer_val();
#endif /*FEATURE_DATA_WLAN_MAPCON*/

   if (ds_epc_read_srat_timer_val(&ho_retry_timer_val) == FALSE)
   {
	 ho_retry_timer_val = DS_EPC_SRAT_CLEANUP_DURATION;
	 DATA_EPC_MSG1(MSG_LEGACY_ERROR, 
				   "EFS item not configured for EPC handoff retry timer val, "
				   "using default value: %d", ho_retry_timer_val);
   }
   
   ds_epc_iwlan_ho_retry_timer_val = ho_retry_timer_val;
   
   /*-------------------------------------------------------------------------
	 Read EFS file for WWAN handoff backoff timer
	-------------------------------------------------------------------------*/
   if (ds_epc_wwan_retry_timer_efs_read(
									&ds_epc_wwan_ho_retry_count,
									 ds_epc_wwan_ho_retry_backoff_timer)== FALSE)
   {
    ds_epc_wwan_ho_retry_count = DS_EPC_WWAN_HO_RETRY_COUNT;
	 ds_epc_wwan_ho_retry_backoff_timer[0] = ho_retry_timer_val;
   
	 DATA_EPC_MSG2(MSG_LEGACY_ERROR, 
				   "EFS item not configured for EPC wwan handoff backoff timer,"
				   "use timer val %d, retry_count %d",
				   ds_epc_wwan_ho_retry_backoff_timer[0],
				   ds_epc_wwan_ho_retry_count);
   }

   return;
}/* ds707_nv_util_read_epc_nv */
#endif /* FEATURE_EPC_HANDOFF */

/*==================================================================================
FUNCTION       DS707_NV_UTIL_AIMS_NV

DESCRIPTION    Wrapper function for ds707_pkt_mgr_aims_nv
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/
void ds707_nv_util_aims_nv(void)
{
/*-----------------------------------------------------------------------------------*/

   ds707_pkt_mgr_aims_nv();


#ifdef FEATURE_MODEM_CONFIG_REFRESH
   /*--------------------------------------------------------------------------------
   Write the function pointers in the NV List
   ----------------------------------------------------------------------------------*/
   ds707_nv_util_insert_list(&ds707_pkt_mgr_aims_nv);
#endif
   return;
}/* ds707_nv_util_aims_nv */


/*==================================================================================
FUNCTION       DS707_NV_UTIL_INIT_DONE

DESCRIPTION    Set the static variable ds707_nv_list_valid to 
               TRUE 
 
DEPENDENCIES   None.

RETURN VALUE   None. 

SIDE EFFECTS   None

=====================================================================================*/

void ds707_nv_util_init_done(void)
{
/*---------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
  Set the status of the status of the NV_list_valid to TRUE  
  -------------------------------------------------------------------------*/
  ds707_nv_list_valid = TRUE;
  return;
}/* ds707_nv_util_init_done */

/*==================================================================================
FUNCTION       DS707_NV_UTIL_REFRESH_COMPLETE

DESCRIPTION    Clear throttling info, init EPZID mgr module 
               to complete hotswap/NV Refresh. Mark the NV
               Refresh progress per subs_id as FALSE.
 
DEPENDENCIES   None.

RETURN VALUE   TRUE: NV Refresh is complete 

SIDE EFFECTS   None

=====================================================================================*/

boolean ds707_nv_util_refresh_complete(uint32 subs_id)
{
/*-----------------------------------------------------------------------------------*/
  if(subs_id >= DS3GSUBSMGR_SUBS_ID_MAX )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,"Invalid subs id");
	return FALSE;
  }

#ifdef FEATURE_EPC_HANDOFF
    /*-------------------------------------------------------------------------
  Init the epc efs items.
   -------------------------------------------------------------------------*/   
  ds707_nv_util_read_epc_nv();
#endif /* FEATURE_EPC_HANDOFF */

    /*-------------------------------------------------------------------------
  Clear the PDN throttling ifnormation for prev card/NV set MBN file.
     -------------------------------------------------------------------------*/   

  ds707_pdnctx_reset_throttle_on_sim_swap();

  /*-------------------------------------------------------------------------
   Invoke the EPZID Init module as part of NV refresh tasks during   
   Sim Swap   
   -------------------------------------------------------------------------*/   
  ds707_epzid_mgr_init();

  /*-------------------------------------------------------------------------
    Set the current 707 network (1X or HDR) the data session is on to NONE
  -------------------------------------------------------------------------*/
  ds707_pkt_set_last_data_sess_network(SYS_SYS_MODE_NO_SRV); 

  ds707_pkt_mgr_update_cdsn(SYS_SYS_MODE_NO_SRV);  

  /*-------------------------------------------------------------------------
   Read the EFS for Disable CS Data Service as part of NV Refresh   
   -------------------------------------------------------------------------*/   
  ds707_nv_util_read_async_nv();

    /*-------------------------------------------------------------------------
  Clear the SYS change related timers and state machine 
     -------------------------------------------------------------------------*/   

  ds707_sys_chg_sr_process_nv_refresh_ind();

  /*--------------------------------------------------------------------
   Disable the 1x handdown mode if UE is in JCDMA mode after HOT SWAP
  --------------------------------------------------------------------*/

  ds707_nv_util_hdrpac_disable_1x_handdown_mode();

  ds707_nv_refresh_in_progress[subs_id] = FALSE;
  return TRUE;
}/* ds707_nv_util_refresh_complete */

/*==================================================================================
FUNCTION       DS707_NV_UTIL_IS_REFRESH_IN_PROGRESS

DESCRIPTION    Returns the status of nv refresh processing per 
               subs_id 
 
DEPENDENCIES   None.

RETURN VALUE   TRUE: If NV Refresh is in progress
                         FALSE If NV refresh is complete.

SIDE EFFECTS   None

=====================================================================================*/
boolean ds707_nv_util_is_refresh_in_progress(uint32 subs_id)
{

  if(subs_id >= DS3GSUBSMGR_SUBS_ID_MAX )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,"Invalid subs id");
	return FALSE;
  }

  return ds707_nv_refresh_in_progress[subs_id];
}/* ds707_nv_util_is_refresh_in_progress */

