#ifndef DS707_NV_UTIL_H
#define DS707_NV_UTIL_H


/*===========================================================================

                          D S 7 0 7 _ N V _ U T I L
 
                           H E A D E R   F I L E
 
GENERAL DESCRIPTION
  This software unit contains functions for 3G Data Services
  NV Refresh event handling. It provides APIs for NV & EFS Reads
  on NV Refresh Event

  Copyright (c) 2001-2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_nv_util.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/17/14   niv     Created module
===========================================================================*/

/*===========================================================================
                          INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "sys.h"
#include "list.h"
#include "dstask_v.h"
#include "ds3gsubsmgr.h"
#include "ds3gcfgmgr.h"

/*===========================================================================
                          PUBLIC DATA DECLARATIONS
===========================================================================*/

/*---------------------------------------------------------------------------
  Struct definition: DS707_NV_UTIL_REFRESH_FUNC_TYPE
---------------------------------------------------------------------------*/
typedef struct
{
  list_link_type                           link;
  /*-------------------------------------------------------------------------
  Function pointer to NV read during NV Refresh event
  -------------------------------------------------------------------------*/
  void                                  (*funcptr)();
} ds707_nv_util_refresh_func_type;

/*===========================================================================

                            EXTERNAL FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION      DS707_NV_UTIL_INSERT_LIST

DESCRIPTION   Write the function pointers of NV read APIs in the 
              order in which they are read during task_init into
              nv_refresh_list

DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_nv_util_insert_list(void (*fptr)());

/*===========================================================================
FUNCTION      DS707_NV_UTIL_INIT

DESCRIPTION   Initialize the queue ds707_nv_refresh_list

DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_nv_util_init();


/*===========================================================================
FUNCTION      DS707_NV_UTIL_NVREFRESH

DESCRIPTION   This function gets invoked when NV refresh event 
              occurs during Sim Swap. Read all the NVs from the 
              nv_refresh_list 

DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_nv_util_nvrefresh(sys_modem_as_id_e_type);

/*===========================================================================
FUNCTION      DS707_NV_UTIL_1X_PROFILE_INIT

DESCRIPTION   Wrapper function for init of static and runtime 1x 
              profile structures 

DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_nv_util_1x_profile_init(void);

/*===========================================================================
FUNCTION      DS707_NV_UTIL_TRAFFIC_CH_TEARDOWN_EFS

DESCRIPTION   Wrapper function for Data Services Mode Specific
              Handler interface and init of critical sections
              used for call operations.

DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_nv_util_traffic_ch_teardown_efs(void);

/*===========================================================================
FUNCTION      DS707_NV_UTIL_SO_ASYNC_INIT

DESCRIPTION   Wrapper function for any async data service init 

DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_nv_util_so_async_init(void);

/*===========================================================================
FUNCTION      DS707_NV_UTIL_SO_G3FAX_INIT

DESCRIPTION   Wrapper function for group 3 fax init  

DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_nv_util_so_g3fax_init(void);

/*===========================================================================
FUNCTION      DS707_NV_UTIL_READ_ASYNC_NV

DESCRIPTION   Wrapper function for the the async powerup init NV

DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_nv_util_read_async_nv(void);

/*===========================================================================
FUNCTION      DS707_NV_UTIL_READ_PKT_MGR_NV_SYNC

DESCRIPTION   Wrapper function for 707_related NVs

DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_nv_util_read_pkt_mgr_nv_sync(void);

/*===========================================================================
FUNCTION      DS707_NV_UTIL_INIT_JCDMA_READ_NV

DESCRIPTION   Wrapper function for JCDMA and timer related NVs

DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_nv_util_init_jcdma_read_nv(void);

/*===========================================================================
FUNCTION     DS707_NV_UTIL_READ_DATA_SESSSION_PROFILE_CACHE_INIT

DESCRIPTION   Wrapper function for profile related NVs

DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_nv_util_read_data_session_profile_cache_init(void);

/*===========================================================================
FUNCTION      DS707_NV_UTIL_DATA_SESSION_CHECK_MANDATORY_APNS

DESCRIPTION   Wrapper function for checking the list of enabled 
              mandatory apns in the profile 

DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_nv_util_data_session_check_mandatory_apns(void);

/*===========================================================================
FUNCTION      DS707_NV_UTIL_READ_DSRDUD_INIT_NV

DESCRIPTION   Wrapper function for reading the inactivity 
              restoration time and setting the NV 

DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_nv_util_read_dsrdud_nv(void);

/*===========================================================================
FUNCTION      DS707_NV_UTIL_QCMIP_SET_CRM_VAL

DESCRIPTION   Wrapper function for setting the CRM value

DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_nv_util_qcmip_set_crm_val(void);

/*===========================================================================
FUNCTION      DS707_NV_UTIL_SO_PKT_INIT

DESCRIPTION   Wrapper function for packet data service options 
               during power up

DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_nv_util_so_pkt_init(void);

/*===========================================================================
FUNCTION      DS707_NV_UTIL_DS707_SO_JCDMA_INIT

DESCRIPTION   Wrapper function to initialize the JCDMA module 
               and register for 707 pkt events

DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_nv_util_ds707_so_jcdma_init(void);

/*===========================================================================
FUNCTION      DS707_NV_UTIL_JCDMA_APPRATE_INIT

DESCRIPTION   Wrapper function to initialize the app rate.

DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_nv_util_jcdma_apprate_init(void);

/*===========================================================================
FUNCTION      DS707_NV_UTIL_HDRPAC_DISABLE_1X_HANDDOWN_MODE

DESCRIPTION   Wrapper function for a function called when the 
               handdown to 1x disable IOCTL is called.

DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_nv_util_hdrpac_disable_1x_handdown_mode();

/*===========================================================================
FUNCTION      DS707_NV_UTIL_HRPD_V6_ENABLED_NV

DESCRIPTION   Wrapper function to set the config file for HRPD 
               with IPv6 enabled

DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_nv_util_hrpd_v6_enabled_nv(void);

#ifdef FEATURE_DATA_PS_IPV6
/*==================================================================================
FUNCTION       DS707_NV_UTIL_READ_MIPV6_NV

DESCRIPTION    Read the MIPv6 Behavior NV item & deny iface for 
               v6 call 
 
DEPENDENCIES   None. 

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/
void ds707_nv_util_read_mipv6_nv(void);

/*==================================================================================
FUNCTION       DS707_NV_UTIL_GET_MIPV6_NV

DESCRIPTION    Return status of MIPv6 Behavior NV Item  
 
DEPENDENCIES   None. 

RETURN VALUE   TRUE: MIPV6 is not DISABLED 
               FALSE: MIPV6 is DISABLED 

SIDE EFFECTS   None

=====================================================================================*/
boolean ds707_nv_util_get_mipv6_nv(void);
#endif /* FEATURE_DATA_PS_IPV6 */
/*===========================================================================
FUNCTION      DS707_NV_UTIL_AIMS_NV

DESCRIPTION   Wrapper function for ds707_pkt_mgr_aims_nv

DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_nv_util_aims_nv(void);


/*===========================================================================
FUNCTION      DS707_NV_UTIL_QCMIP_SET_MDR_VAL

DESCRIPTION   Wrapper function for checking the QCMIP and 
              setting the MDR value 

DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_nv_util_qcmip_set_mdr_val(void);

/*===========================================================================
FUNCTION      DS707_NV_UTIL_READ_JCDMA_M51_INIT

DESCRIPTION   Wrapper function for initializing the JCDMA M51 
              related NVs

DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_nv_util_read_jcdma_m51_init(void);

/*===========================================================================
FUNCTION      DS707_NV_UTIL_READ_INTERNAL_AUTH_NV

DESCRIPTION   Wrapper function for reading the internal auth NV 
              item and setting the static variable  

DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_nv_util_read_internal_auth_nv(void);

/*===========================================================================
FUNCTION      DS707_NV_UTIL_READ_DRS_NV

DESCRIPTION   Wrapper function for the init and NV reads for the 
              DSR algorithm 

DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_nv_util_read_drs_nv(void);

/*===========================================================================
FUNCTION      DS707_NV_UTIL_NW_INIT_QOS_MGR_INIT

DESCRIPTION   Wrapper function for NV reads in the QOS module 
              during power up 

DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_nv_util_nw_init_qos_mgr_init(void);

/*===========================================================================
FUNCTION      DS707_NV_UTIL_READ_EHRPD_TO_HRPD_FB_NV

DESCRIPTION   Wrapper function for reading the eHRPD to HRPD 
              fallback configuration related NV 

DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_nv_util_read_ehrpd_to_hrpd_fb_nv(void);

/*===========================================================================
FUNCTION      DS707_NV_UTIL_READ_DNS_V4_ADDR_REQ_NV

DESCRIPTION   Wrapper function for reading the dns v4 addr req 
              nv 

DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_nv_util_read_dns_v4_addr_req_nv(void);

/*===========================================================================
FUNCTION      DS707_NV_UTIL_SET_FAILOVER_IP_MODE

DESCRIPTION   Wrapper function to read the failover ip type 

DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_nv_util_set_failover_ip_mode(void);

/*===========================================================================
FUNCTION      DS707_NV_UTIL_READ_DSHDR_DHCP_NV

DESCRIPTION   Wrapper function to init the dhcp negotiation 
              options

DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_nv_util_read_dshdr_dhcp_nv(void);

/*===========================================================================
FUNCTION      DS707_NV_UTIL_SYS_CHG_HDLR_SILENT_REDIAL_INFO_INIT

DESCRIPTION   Wrapper function to initialise the module.
 
DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_nv_util_sys_chg_hdlr_silent_redial_info_init(void);

/*==================================================================================
FUNCTION       DS707_NV_UTIL_READ_NONEPC_CLEANUP_EFS

DESCRIPTION    Wrapper function for reading EFS items for NONEPC 
               call clean up 
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

=====================================================================================*/
void ds707_nv_util_read_nonepc_call_cleanup_efs(void);

/*===========================================================================
FUNCTION      DS707_NV_UTIL_S101_VCTM_INIT
 
DESCRIPTION   Wrapper function to read and init the vctm data 
              structures 

DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_nv_util_s101_vctm_init(void);

/*===========================================================================
FUNCTION      DS707_NV_UTIL_S101_DCTM_INIT

DESCRIPTION   Wrapper function to read and init the dctm data 
              structures 

DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_nv_util_s101_dctm_init(void);

/*===========================================================================
FUNCTION      DS707_NV_UTIL_MIP_DO_SESSION_CLOSE_NV

DESCRIPTION   Wrapper function to read NV for session close on 
              MIP failure 

DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_nv_util_mip_do_sess_close_nv(void);

/*===========================================================================
FUNCTION      DS707_NV_UTIL_FB_LIST_INIT

DESCRIPTION   Wrapper function to init the parameters related to 
              fallback pdn list 

DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_nv_util_fb_list_init(void);

/*===========================================================================
FUNCTION      DS707_NV_UTIL_ADDITIONAL_DIAL_STR_INIT

DESCRIPTION   Wrapper function to read the NV for additional 
              dial string provisioning

DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_nv_util_additional_dial_str_init(void);

/*===========================================================================
FUNCTION      DS707_NV_UTIL_IGNORE_LAST_PDN_IA_TIMER_NV

DESCRIPTION   Wrapper function to read the NV 
              ignore_last_pdn_ia_timer 

DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_nv_util_ignore_last_pdn_ia_timer_nv(void);

/*===========================================================================
FUNCTION      DS707_NV_UTIL_READ_PDN_OVERRIDE_NV

DESCRIPTION   Wrapper function to read the NV for disable pdn 
              type override 

DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_nv_util_read_pdn_override_nv(void);

/*===========================================================================
FUNCTION      DS707_NV_UTIL_READ_BP_ENABLED_NV

DESCRIPTION   Wrapper function to init backpressure related 
              parameters 

DEPENDENCIES  None.

RETURN VALUE  None
==========================================================================*/
void ds707_nv_util_read_bp_enabled_nv(void);

#ifdef FEATURE_EPC_HANDOFF
/*===========================================================================
FUNCTION      DS707_NV_UTIL_READ_EPC_NV

DESCRIPTION   This function is used to set the EPC handoff 
              related NVs 
 
DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_nv_util_read_epc_nv(void);
#endif /* FEATURE_EPC_HANDOFF */

/*===========================================================================
FUNCTION      DS707_NV_UTIL_INIT_DONE

DESCRIPTION   This function is used to indicate that the NV 
              refresh list has been constructed as part of task_init. 
              Mark the flag as done.

DEPENDENCIES  None.

RETURN VALUE  None.
===========================================================================*/
void ds707_nv_util_init_done(void);

/*==================================================================================
FUNCTION       DS707_NV_UTIL_IS_REFRESH_IN_PROGRESS

DESCRIPTION    Returns the status of nv refresh processing per subs_id 
 
DEPENDENCIES   None.

RETURN VALUE   TRUE: If NV Refresh is in progress
               FALSE: If NV refresh is complete.

SIDE EFFECTS   None

=====================================================================================*/
boolean ds707_nv_util_is_refresh_in_progress(uint32);

/*==================================================================================
FUNCTION       DS707_NV_UTIL_REFRESH_COMPLETE

DESCRIPTION    Returns the status of nv refresh processing.
 
DEPENDENCIES   None.

RETURN VALUE   TRUE: If NV Refresh is in progress 
               FALSE If NV refresh is complete.

SIDE EFFECTS   None

=====================================================================================*/
boolean ds707_nv_util_refresh_complete(uint32);

#endif /* DS707_NV_UTIL_H */
