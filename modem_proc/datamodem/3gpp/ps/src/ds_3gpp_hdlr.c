/*!
  @file
  ds_3gpp_hdlr.c

  @brief
  REQUIRED brief one-sentence description of this C module.

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2008-2015 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/src/ds_3gpp_hdlr.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/09/14   ds      Adding FEATURE_DATA_PS_ROHC
06/07/13   nd      Added support for filling PCO of MT calls(primary/secondary)
                   from incoming call request and not from profile(See 23.060).
12/14/12   ss      Not changing 3gpp_sys_mode to LTE upon receiving PDN_REJ.
11/22/11   nd      Made data flow control NV configurable in power save mode.
09/25/12   ss      3GPP MH LTE-Off compilation fixes
06/28/12   ss      Fixed the issue of WCDMA context_transfer_hdlr not getting
                   registered.
01/18/12   vb      Removed function ds_3gpp_is_ps_data_available(refer to ds_3gpp_api.c)
01/02/12   msh     Coan: Feature cleanup
12/08/11    rr     Added support to update p-cscf address from NW bearer modification
                   indiacation
12/14/11   vk      Ignore pref sys change event
10/05/11   ttv     Added DSDS changes for new mode handler.
09/05/11   ss      Changes to support RPM.
07/01/11   ttv     Added changes to computer NSAPI for all active bearers
                   using given APN.
06/06/11   sa      Added NV item 3gpp_data_pkt_orig_str.
05/04/11   ttv     Added changes needed for SESSION_PARAMS IOCTL and NSAPI api.
03/30/11   sa      Added fix to update the ds_3gpp_sys_mode to be inline
                   with the ds3gi_mode.
03/24/11   sa      Added MT PDP Auto answer support.
03/17/11   sa      Added support for bearer tech change event indication.
03/10/11   ttv     Added support for LTE watermark tracing.
03/02/11   ss      Added support for configurable RAB_REESTAB RETRY.
02/23/11   ttv     Added support for watermark tracing.
02/22/11   ttv     Merged the changes of adding wrapper function to get
                   cumulative iface stats.
02/02/11   ttv     Added support for get session params.
12/01/10   ss      Support for SIM REFRESH VOTING for WCDMA and GPRS.
11/19/10   ars     Changed proc of setupBridgeInterfaceEvent to command based
06/17/10   as      Fusion additions
05/17/10   hs      Added APN disable and PDN inactivity feature support.
04/21/10   sa      Added support for Loopback Mode B.
04/03/10   vk      Added support for MSG2.0 based logging
04/02/10   vrk     UMTS interRAT support
02/04/10   vd      Using the changed flow functions
01/23/10   vk      Added helper macros for virtual function dispatch
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"

#include <stringl/stringl.h>

#ifdef FEATURE_DATA_3GPP
#include "cm.h"
#include "msg.h"
#include "err.h"
#include "amssassert.h"
#include "rex.h"
#include "ds3gmgr.h"
#include "ds3geventmgr.h"
#include "ds3gtimer.h"
#include "ds3gmshif.h"
#include "dstask_v.h"
#include "dstaski.h"
#include "dsmsgr.h"
//#include "smix.h"
#include "dsumtspdpregint.h"
#include "dsumts_rmsm.h"
#include "ds_3gpp_rmsm_autoconfig_sm.h"
#include "ds_3gpp_bearer_context.h"
#include "ds_3gpp_nv_manager.h"
#include "ds_3gpp_pdn_context.h"
#include "ds_3gpp_apn_table.h"
#include "ds_dsd_apm_ext_i.h"
#include "ds_3gpp_network_override_hdlr.h"
#include "ds3gcmif.h"
#include "ds_3gpp_pdn_throt_config.h"
#include "ds_3gpp_srv_req_throt_config.h"
#include "ds_3gpp_cc_sp_throt_config.h"

#include "ps_iface_defs.h"
#include "dsumtsps.h"
#ifdef FEATURE_DATA_WCDMA_PS
#include "ds_wcdma_bearer_context.h"
#endif /* FEATURE_DATA_WCDMA_PS */
#ifdef FEATURE_DATA_RPM
#include "ds_3gpp_rpm.h"
#endif /* FEATURE_DATA_RPM */

#if defined(FEATURE_GSM_GPRS)
#include "ds_gprs_bearer_context.h"
#endif /* FEATURE_GSM_GPRS */

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
#include "ds_umts_bearer_context.h"
#include "ds_umts_pdn_context.h"
#include "ds_umts_flow_manager.h"
#include "ds_umts_bearer_flow_manager.h"
#include "dsumtspsmthdlr.h" //VRK: for dsumtsps_mt_reg_table_match_emb_profile
#endif /* FEATURE_DATA_WCDMA_PS || FEATURE_GSM_GPRS */

#ifdef FEATURE_DATA_LTE
#include "ds_eps_tlb.h"
#include "ds_eps_pdn_context.h"
#include "ds_eps_bearer_context.h"
#include "ds_eps_flow_manager.h"
#include "ds_eps_pdn_throttle_sm.h"
#include "ds_eps_bearer_flow_manager.h"
#endif /* FEATURE_DATA_LTE */

#include "ds_3gpp_flow_manager.h"
#include "ds_3gpp_bearer_flow_manager.h"
#include "ds_3gpp_hdlr.h"
#include "ds_3gppi_utils.h"
#include "dserrno.h"
#include "ds_3gpp_dyn_mem_hdlr.h"
#include "ds_eps_kamgr_ext.h"
#include "ds_eps_throughput_hdlr.h"
#include "ps_sys_event.h"
#include "ds3gpp_ext_msg.h"

#if defined(FEATURE_GSM_GPRS) || defined(FEATURE_DATA_WCDMA_PS)
#include "ps_iphc.h"

#ifdef FEATURE_DATA_PS_ROHC
#include "ps_rohc.h"
#endif /* FEATURE_DATA_PS_ROHC */

#endif /*FEATURE_GSM_GPRS || FEATURE_DATA_WCDMA_PS */


#include "ds_3gpp_throttle_sm.h"
#include "ds_3gpp_pdn_throttle_sm.h"

#define DS_LTE_DISABLE 0

#include "ds_profile_3gppi.h"
#include "ds_3gpp_profile_api.h"
#include "ds_3gppi_utils.h"
#include "ds_dsd_apm_ext_i.h"
#include "ds3gdsdif.h"

#ifdef FEATURE_DATA_LTE
#include "ds_3gpp_pdn_limit_hdlr.h"
#include "ds_eps_stats.h"
#endif /*FEATURE_DATA_LTE*/

#include "ds_3gpp_roaming_hdlr.h"
#include "dsucsdhdlr.h"
#include "ds_loopback_hdlr.h"
#include "ds_3gpp_embms_hdlr.h"
#include "ds_3gpp_network_override_hdlr.h"
#include "ds_3gpp_auth_mgr.h"
#include "ds_3gpp_embms_tmgi_tbl_mgr.h"
#include "ds_eps_tlb.h"
#include "ds_3gpp_op_pco_hdlr.h"
#include "ds_3gpp_device_ev_hdlr.h"
#include "ds_3gpp_stats.h"
#include "ds3gprofile_validator.h"

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================
  Cookie to ensure that dynamic memory allocated is valid
===========================================================================*/
#define DS3GPP_PER_SUBS_INFO_COOKIE 0xabcabcab

/*===========================================================================
TYPEDEF DS_3GPP_EVENT_BUF_TYPE

DESCRIPTION
  This is the type that is placed on the event callback queues.
===========================================================================*/
typedef struct
{
  q_link_type              q_link;  
  ds_3gpp_event_cback_type event_callback_fptr;
  uint32                   user_data;
} ds_3gpp_event_buf_type;

/*===========================================================================
   This Structure is used to maintain all the global variables of 
   ds_3gpp_hdlr.c. 
   PLEASE USE THIS STRUCTURE WHENEVER YOU WANT TO ADD A NEW GLOBAL
   VARIABLE.
===========================================================================*/
typedef struct
{
  ds3gpp_per_subs_info_type       *per_subs_info_p[DS3GSUBSMGR_SUBS_ID_MAX];

  /*---------------------------------------------------------------------------
   Queue to hold list of registered callbacks for
   DS_3GPP_PH_EV_OPRT_MODE_CHG_CMD
  ---------------------------------------------------------------------------*/
  q_type                           event_q[DS_3GPP_EV_MAX];

#if defined(FEATURE_GSM_GPRS) || defined(FEATURE_DATA_WCDMA_PS)
  ds3g_hdlr_func_tbl_type          wcdma_handler_tbl;
  ds3g_hdlr_func_tbl_type          gprs_handler_tbl;
#endif /*(FEATURE_GSM_GPRS || FEATURE_DATA_WCDMA_PS)*/

#ifdef FEATURE_TDSCDMA
  ds3g_hdlr_func_tbl_type          tdscdma_handler_tbl;
#endif /* FEATURE_TDSCDMA */

  ds3g_hdlr_func_tbl_type          eps_handler_tbl;
  ds_profile_3gpp_cb_func_tbl_type ds_profile_3gpp_cb_func_tbl;

  /*-----------------------------------------------------------------------------
    Global critical section
  -----------------------------------------------------------------------------*/
  rex_crit_sect_type               global_crit_sect;

} ds3gpp_info_type;

/*---------------------------------------------------------------------------
  Struct for DS_CMD_3GPP_APN_NAME_CHG command
---------------------------------------------------------------------------*/
typedef struct
{
  uint16                  profile_id;
  sys_modem_as_id_e_type  subs_id;
}ds_3gpp_apn_name_chg_cmd_type;


/*===========================================================================

                         GLOBAL VARIABLES

===========================================================================*/



/*===========================================================================
   This Structure is used to maintain all the global variables of 
   ds_3gpp_hdlr.c. 
   PLEASE USE THIS STRUCTURE WHENEVER YOU WANT TO ADD A NEW GLOBAL
   VARIABLE.
===========================================================================*/
static ds3gpp_info_type ds3gpp_info;


static boolean ds_3gpp_nv_refresh_in_progress[SYS_MODEM_AS_ID_MAX] = { FALSE };

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================
FUNCTION DS3GPP_PER_SUBS_INFO_INIT
 
DESCRIPTION
  This function initializes all subscription-specific globals in 
  ds_3gpp_hdlr.c
 
PARAMETERS 
  Subscription id

DEPENDENCIES 
  None

RETURN VALUE 
  void
 
SIDE EFFECTS 
===========================================================================*/
static void ds3gpp_per_subs_info_init
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS3GPP_INFO_INIT
 
DESCRIPTION
  This function initializes all global information in ds_3gpp_hdlr.c.
 
PARAMETERS 
  None

DEPENDENCIES 
  None

RETURN VALUE 
  void
 
SIDE EFFECTS 
===========================================================================*/
static rex_sigs_type ds3gpp_info_init
(
  void
);

/*===========================================================================
FUNCTION DS3GPP_SET_PER_SUBS_INFO_HNDL
 
DESCRIPTION
  This function sets the handle to subscription-specific information in 
  ds_3gpp_hdlr.c
 
PARAMETERS 
  1. Subscription id
  2. Handle to Subscription Specific Information

DEPENDENCIES 
  None

RETURN VALUE 
  None
 
SIDE EFFECTS 
===========================================================================*/
static void ds3gpp_set_per_subs_info_hndl
(
  sys_modem_as_id_e_type     subs_id,
  ds3gpp_per_subs_info_type  *hndl
);

/*===========================================================================
FUNCTION DS3GPP_GET_DS_PROFILE_CB_FUNC_TBL
 
DESCRIPTION
  This function returns a handle to the DS Profile Callback Function Table
 
PARAMETERS 
  None

DEPENDENCIES 
  None

RETURN VALUE 
  Handle to the DS Profile Callback Function Table
 
SIDE EFFECTS 
===========================================================================*/
static ds_profile_3gpp_cb_func_tbl_type* ds3gpp_get_ds_profile_cb_func_tbl
(
  void
);

/*===========================================================================
FUNCTION DS3GPP_GET_HANDLER_TBL
 
DESCRIPTION
  This function returns a handle to the 3GPP Handler Table for the 
  specified mode.
 
PARAMETERS 
  System Mode

DEPENDENCIES 
  None

RETURN VALUE 
  Pointer to the 3GPP Handler Table
 
SIDE EFFECTS 
===========================================================================*/
static ds3g_hdlr_func_tbl_type* ds3gpp_get_handler_tbl
(
  sys_sys_mode_e_type mode
);

/*===========================================================================
FUNCTION ds3gpp_subsmgr_event_cb

DESCRIPTION
  This function calls the call back registered for DS3GSUBSMGR Events

PARAMETERS
  event_id - event type
  *event_info_ptr - event payload
  *data_ptr - not used
  
DEPENDENCIES
  None.

RETURN VALUE  
  None.
 
SIDE EFFECTS

===========================================================================*/
void ds3gpp_subsmgr_event_cb
(
  ds3gsubsmgr_event_type       event_id,
  ds3gsubsmgr_event_info_type *event_info_ptr,
  void                        *data_ptr
);

/*===========================================================================
FUNCTION ds3gpp_ds3g_event_cb

DESCRIPTION
  This function calls the call back registered for DS3G Events

PARAMETERS
  event_id - event type
  *event_info_ptr - event payload
  *data_ptr - not used
  
DEPENDENCIES
  None.

RETURN VALUE  
  None.
 
SIDE EFFECTS

===========================================================================*/
void ds3gpp_ds3g_event_cb
(
  ds3geventmgr_event_type       event_id,
  ds3geventmgr_filter_type      *filter_info_ptr,
  void                          *event_info_ptr,
  void                          *data_ptr
);

#ifdef FEATURE_DATA_LTE
/*===========================================================================
FUNCTION DS_3GPP_PROCESS_APN_DISABLED_CMD
 
DESCRIPTION
   This function is used to handle the event when the admin APN is disabled or
  enabled in the profile.
            LTE is disabled even if one APN in the min APN list is disabled
  Similarly LTE enable is called all the APN in min APN list is enabled
 

PARAMETERS 
  profile_num
  apn_disable_flag

DEPENDENCIES 
  
RETURN VALUE 
  void
 
SIDE EFFECTS 
  
===========================================================================*/
static void ds_3gpp_process_apn_disabled_cmd
(
  uint16  profile_num,
  boolean apn_disable_flag,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_APN_DISABLE_HDLR
 
DESCRIPTION
  This function is used to handle the event when the APN is disabled or
  enabled in the profile.
  LTE is disabled even if one APN in the min APN list is disabled
  Similarly LTE enable is called all the APN in min APN list is enabled

PARAMETERS 
  profile_num
  apn_disable_flag

DEPENDENCIES 
  
RETURN VALUE 
  void
 
SIDE EFFECTS 
  
===========================================================================*/
static void ds_3gpp_apn_disable_hdlr
(
  uint16  profile_num,
  sys_modem_as_id_e_type subs_id,
  boolean new_apn_disable_flag
);
#endif /*FEATURE_DATA_LTE */

/*===========================================================================
FUNCTION DS_3GPP_MODE_PREF_CHANGE_HDLR

DESCRIPTION
  This Handler is triggered while mode_pref  changes. This is required when 
  IRAT happens due to mode change.

PARAMETERS
  subs_id             -- Subscription ID where mode_pref_change occurred.
  new_pref_mode -- New preferred mode after mode_pref_change.
  old_pref_mode -- Old preferred mode abefore mode_pref_change.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_mode_pref_change_hdlr
(
  sys_modem_as_id_e_type     subs_id,
  sys_sys_mode_e_type        new_pref_mode,
  sys_sys_mode_e_type        old_pref_mode
);

/*===========================================================================

                                FUNCTIONS

===========================================================================*/

#ifdef FEATURE_DATA_LTE


/*===========================================================================
FUNCTION DS_3GPP_MAX_PDN_CONN_PER_BLK_CHG_HDLR
 
DESCRIPTION
  This function is invoked when Max PDN Conn Per Block parameter
  is changed in the profile.

PARAMETERS 
  profile_num
  max_pdn_conn_per_blk

DEPENDENCIES 
  None. 
  
RETURN VALUE 
  void
 
SIDE EFFECTS 
  
===========================================================================*/
static void ds_3gpp_max_pdn_conn_per_blk_chg_hdlr
(
  uint16 profile_num,
  sys_modem_as_id_e_type subs_id,
  uint16 max_pdn_conn_per_blk
)
{
  ds_umts_pdp_profile_type           *pdp_profile_ptr = NULL;
  ds_umts_pdp_profile_status_etype   result;
  uint8                              index;

  if(ds_3gpp_pdn_limit_is_enabled(subs_id))
  {
    DS_3GPP_MSG3_HIGH("Max PDN Conn Per Blk changed for profile %d, "
                      "new value: %d",profile_num,max_pdn_conn_per_blk,0);

    pdp_profile_ptr = (ds_umts_pdp_profile_type*) 
                       modem_mem_alloc(sizeof(ds_umts_pdp_profile_type),
                       MODEM_MEM_CLIENT_DATA);

    /*------------------------------------------------------------------------- 
    Allocate and Initialize memory before filling in the report
    -------------------------------------------------------------------------*/
    if(pdp_profile_ptr == NULL)
    {
      DS_3GPP_MSG0_ERROR("Mem Alloc to pdp_profile_ptr failed");
      goto function_exit;
    }
    memset(pdp_profile_ptr, 0, sizeof(ds_umts_pdp_profile_type));


    result = ds_umts_get_pdp_profile_all_data_per_subs(profile_num, 
                          dsumts_subs_mgr_get_subs_id(subs_id), pdp_profile_ptr);

    if( result != DS_UMTS_PDP_SUCCESS )
    {
      DS_3GPP_MSG1_ERROR("Error in retrieving definition for prof number:%d",
                         profile_num);
      goto function_exit;
    }

    if((index = ds_3gpp_pdn_limit_tbl_find_entry
                  ((byte*)pdp_profile_ptr->context.apn, subs_id)) != 
         DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY)
    {
      ds_3gpp_pdn_limit_update_max_pdn_conn_per_blk(index,
                                                    subs_id ,
                                                    max_pdn_conn_per_blk);
      ds_3gpp_pdn_limit_reset_pdn_conn_cntr(index, subs_id);
      ds_3gpp_pdn_limit_start_pdn_conn_tmr(index, subs_id);
    }
  }

function_exit:
  if (pdp_profile_ptr != NULL) 
  {
    modem_mem_free(pdp_profile_ptr, MODEM_MEM_CLIENT_DATA_CRIT);
  }
}

/*===========================================================================
FUNCTION DS_3GPP_PDN_REQ_WAIT_TIME_CHG_HDLR
 
DESCRIPTION
  This function is invoked when PDN Req Wait Time parameter
  is changed in the profile.

PARAMETERS 
  profile_num
  pdn_req_wait_time

DEPENDENCIES 
  None. 
  
RETURN VALUE 
  void
 
SIDE EFFECTS 
  
===========================================================================*/
static void ds_3gpp_pdn_req_wait_time_chg_hdlr
(
  uint16 profile_num,
  sys_modem_as_id_e_type subs_id,
  uint16 pdn_req_wait_time
)
{
  ds_umts_pdp_profile_type           *pdp_profile_ptr = NULL;
  ds_umts_pdp_profile_status_etype   result;
  uint8                              index;

  if(ds_3gpp_pdn_limit_is_enabled(subs_id))
  {
    DS_3GPP_MSG3_HIGH("PDN Req Wait Time changed for profile %d, "
                      "new value: %d",profile_num,pdn_req_wait_time,0);

    pdp_profile_ptr = (ds_umts_pdp_profile_type*) 
                       modem_mem_alloc(sizeof(ds_umts_pdp_profile_type),
                       MODEM_MEM_CLIENT_DATA);

    /*------------------------------------------------------------------------- 
    Allocate and Initialize memory before filling in the report
    -------------------------------------------------------------------------*/
    if(pdp_profile_ptr == NULL)
    {
      DS_3GPP_MSG0_ERROR("Mem Alloc to pdp_profile_ptr failed");
      return;
    }
    memset(pdp_profile_ptr, 0, sizeof(ds_umts_pdp_profile_type));

    result = ds_umts_get_pdp_profile_all_data_per_subs(profile_num,
                           dsumts_subs_mgr_get_subs_id(subs_id), pdp_profile_ptr);

    if( result != DS_UMTS_PDP_SUCCESS )
    {
      DS_3GPP_MSG1_ERROR("Error in retrieving definition for prof number:%d",
                         profile_num);
      goto function_exit;
    }

    if((index = ds_3gpp_pdn_limit_tbl_find_entry
                  ((byte*)pdp_profile_ptr->context.apn, subs_id)) !=
         DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY)
    {
      ds_3gpp_pdn_limit_update_pdn_req_wait_time(index, subs_id, 
                                                 pdn_req_wait_time);

      /* Check whether PDN Wait Timer is running. 
         We restart the timer with the new value, only if it is 
         already running */
      if(ds_3gpp_pdn_limit_is_pdn_req_wait_timer_in_progress(index, subs_id))
      {
        ds_3gpp_pdn_limit_start_pdn_wait_tmr(index, subs_id);
      }
    }
  }

function_exit:
  if (pdp_profile_ptr != NULL) 
  {
    modem_mem_free(pdp_profile_ptr, MODEM_MEM_CLIENT_DATA_CRIT);
  }
}

/*===========================================================================
FUNCTION DS_3GPP_MAX_PDN_CONN_TIME_CHG_HDLR
 
DESCRIPTION
  This function is invoked when the Max PDN Conn Time parameter
  is changed in the profile.

PARAMETERS 
  profile_num
  pdn_req_wait_time

DEPENDENCIES 
  None. 
  
RETURN VALUE 
  void
 
SIDE EFFECTS 
  
===========================================================================*/
static void ds_3gpp_max_pdn_conn_time_chg_hdlr
(
  uint16 profile_num,
  sys_modem_as_id_e_type subs_id,
  uint16 max_pdn_conn_time
)
{
  ds_umts_pdp_profile_type           *pdp_profile_ptr = NULL;
  ds_umts_pdp_profile_status_etype   result;
  uint8                              index;


  DS_3GPP_MSG3_HIGH("Max PDN Conn Time changed for profile %d, "
                    "new value: %d",profile_num,max_pdn_conn_time,0);


  if(ds_3gpp_pdn_limit_is_enabled(subs_id))
  {
    pdp_profile_ptr = (ds_umts_pdp_profile_type*) 
                       modem_mem_alloc(sizeof(ds_umts_pdp_profile_type),
                       MODEM_MEM_CLIENT_DATA);

    /*------------------------------------------------------------------------- 
    Allocate and Initialize memory before filling in the report
    -------------------------------------------------------------------------*/
    if(pdp_profile_ptr == NULL)
    {
      DS_3GPP_MSG0_ERROR("Mem Alloc to pdp_profile_ptr failed");
      return;
    }
    memset(pdp_profile_ptr, 0, sizeof(ds_umts_pdp_profile_type));

    result = ds_umts_get_pdp_profile_all_data_per_subs(profile_num, 
                              dsumts_subs_mgr_get_subs_id(subs_id), pdp_profile_ptr);

    if( result != DS_UMTS_PDP_SUCCESS )
    {
      DS_3GPP_MSG1_ERROR("Error in retrieving definition for prof number:%d",
                          profile_num);
      goto function_exit;
    }

    if((index = ds_3gpp_pdn_limit_tbl_find_entry
                  ((byte*)pdp_profile_ptr->context.apn, subs_id)) 
                    != DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY)
    {
      ds_3gpp_pdn_limit_update_max_pdn_conn_time(index, subs_id, 
                                                 max_pdn_conn_time);
      ds_3gpp_pdn_limit_reset_pdn_conn_cntr(index, subs_id);
      ds_3gpp_pdn_limit_start_pdn_conn_tmr(index, subs_id);      
    }
  }
function_exit:
  if (pdp_profile_ptr != NULL) 
  {
    modem_mem_free(pdp_profile_ptr, MODEM_MEM_CLIENT_DATA_CRIT);
  }
}

/*===========================================================================
FUNCTION DS_3GPP_APN_DISABLE_HDLR
 
DESCRIPTION
   This function is used to handle the event when the APN is disabled or
  enabled in the profile.
  LTE is disabled even if one APN in the min APN list is disabled
  Similarly LTE enable is called all the APN in min APN list is enabled

PARAMETERS 
  profile_num
  apn_disable_flag

DEPENDENCIES 
  
RETURN VALUE 
  void
 
SIDE EFFECTS 
  
===========================================================================*/
static void ds_3gpp_apn_disable_hdlr
(
  uint16  profile_num,
  sys_modem_as_id_e_type subs_id,
  boolean new_apn_disable_flag
)
{
  ds_cmd_type                         *cmd_ptr = NULL;
  ds_profile_db_apn_disable_info_type *apn_disable_info_ptr = NULL;
  /*---------------------------------------------------------------------------*/

  if(ds_3gpp_profile_cache_get_cache_index(profile_num, 
                       dsumts_subs_mgr_get_subs_id(subs_id)) == -1)
  {
    DS_3GPP_MSG1_ERROR("Invalid profile_num %d ", profile_num);
     return;
  }

  DS_3GPP_MSG2_HIGH("APN Disabled flag changed to %d for profile %d ", 
                    new_apn_disable_flag, profile_num);

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_profile_db_apn_disable_info_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_PROFILE_DB_APN_DISABLE;
  apn_disable_info_ptr = (ds_profile_db_apn_disable_info_type*)cmd_ptr->cmd_payload_ptr;

  apn_disable_info_ptr->apn_disable_flag = new_apn_disable_flag;
  apn_disable_info_ptr->subs_id = subs_id;                                          
  apn_disable_info_ptr->profile_num = profile_num;
  ds_put_cmd_ext(cmd_ptr);

  }

/*===========================================================================
FUNCTION DS_3GPP_PROCESS_APN_DISABLED_CMD
 
DESCRIPTION
   This function is used to handle the event when the admin APN is disabled or
  enabled in the profile.
            LTE is disabled even if one APN in the min APN list is disabled
  Similarly LTE enable is called all the APN in min APN list is enabled
 

PARAMETERS 
  profile_num
  apn_disable_flag

DEPENDENCIES 
  
RETURN VALUE 
  void
 
SIDE EFFECTS 
  
===========================================================================*/
static void ds_3gpp_process_apn_disabled_cmd
(
  uint16  profile_num,
  boolean apn_disable_flag,
  sys_modem_as_id_e_type subs_id
)
{
  uint16                              profiles_list
                                        [DS_3GPP_CFG_PROFILES_MAX + 1];
  int                                 num_profiles=0;
  uint16                              profile=0;
  uint8                               index=0;
  boolean                             min_apn_list=FALSE;
  int                                 disabled_profile_cnt=0;
  boolean                             profile_match=FALSE;
  ds_umts_pdp_profile_status_etype    ret_val=DS_UMTS_PDP_NOT_INITIALIZED;

  ds_umts_pdp_profile_status_etype    profile_status = DS_UMTS_PDP_SUCCESS;
  byte                                prof_apn_name
                                        [DS_UMTS_MAX_APN_STRING_LEN+1];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(prof_apn_name, 0, DS_UMTS_MAX_APN_STRING_LEN+1);

  /*-------------------------------------------------------------------------
    If APN Disabled flag is set to TRUE, we will throttle the APN.
    We will also tear down any PDN Connection which is active using that
    profile.
   
    If APN Disabled is set to FALSE, we will unthrottle the APN. 
  -------------------------------------------------------------------------*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }


  profile_status = ds_umts_get_pdp_profile_context_info_apn_per_subs
                     (profile_num, dsumts_subs_mgr_get_subs_id(subs_id), 
                      prof_apn_name, DS_UMTS_MAX_APN_STRING_LEN);

  DATA_3GPP_MSG_SPRINTF_3(MSG_LEGACY_MED,
                          "Profile status: %d. "
                          "APN Disabled flag changed to %d for APN %s",
                          profile_status, apn_disable_flag, prof_apn_name );

  if (apn_disable_flag)
  {
    if (profile_status == DS_UMTS_PDP_SUCCESS)
    {
      ds_3gpp_pdn_throt_perform_apn_disabled_throttling
        (prof_apn_name, subs_id);
    }

    ds_3gpp_pdn_cntx_teardown_pdn_per_profile
        (profile_num, PS_NET_DOWN_REASON_INTERNAL_APN_DISABLED);
  }
  else
  {
    if (profile_status == DS_UMTS_PDP_SUCCESS)
    {
      ds_3gpp_pdn_throt_unblock_apn_disabled_throttling
        (prof_apn_name, subs_id);
    }
  }

  /*-------------------------------------------------------------------------
    Read the Min APN list to check if the disabled APN is present in it.
  -------------------------------------------------------------------------*/
  if(ds_3gpp_cfg_get_subsid(DS_3GPP_CFG_PROFILE_LIST, 
                    (unsigned char *) &profiles_list[0],
                    (sizeof(uint16) * DS_3GPP_CFG_PROFILES_MAX),
                     subs_id) == -1)
  {
      DS_3GPP_MSG0_ERROR("Cannot read EFS Config item DS_3GPP_CFG_PROFILE_LIST");
      return;
  }

  num_profiles = profiles_list[0];
  if((num_profiles == 0) || (num_profiles >=  DS_3GPP_CFG_PROFILES_MAX))
  {
    DS_3GPP_MSG1_ERROR("Min Profile List has %d profiles",num_profiles);
    return;
  }

  /* 
      APN list is non-empty.
      We need to check if the profile exists in the min APN list.
      Depending on the setting of the  APN Disable flag,
           we either disbale or enable LTE.
      NOTE : Even if we call LTE disable/enable multiple times, CM will ignore.
      No special handling is needed from Data side to handle this.

    */
  // Get count of number of apn_disable_flag set 
  for(index = 1; index <= num_profiles; index++)
  {
    profile = profiles_list[index];
    DS_3GPP_MSG1_HIGH("profile index %d",index);
    if(profile == profile_num)
    {
      profile_match=TRUE; // used in case of apn_disable_flag
    }
      ret_val =ds_umts_get_pdp_profile_apn_disable_flag_per_subs(profile, 
                        dsumts_subs_mgr_get_subs_id(subs_id), &min_apn_list);
    if(ret_val==DS_UMTS_PDP_SUCCESS && min_apn_list == TRUE)
    {
      disabled_profile_cnt++; //disable_profile_cnt ;
    }
  }
   DS_3GPP_MSG3_HIGH("profile match %d for profile %d . disabled_profile_cnt %d ",
                     profile_match, profile_num,disabled_profile_cnt);
  if(profile_match)
  {
    if(apn_disable_flag)
    {
      ds3g_msh_disable_lte_selection_ex(subs_id);
    }
    else if( apn_disable_flag == FALSE && disabled_profile_cnt == 0)
    {
      ds3g_msh_enable_lte_selection_ex(subs_id);
    }
    else
    {
      /* 
         LTE is disabled already and the current profile cannot enable LTE
         selection because there is atleast one other profile which is
         requesting for LTE to be disabled
      */
      DS_3GPP_MSG1_ERROR("Can't enable LTE selection for profile :%d,",
                         profile_num);

    }
  }
  else
  {
    DS_3GPP_MSG1_ERROR("Profile %d not in min APN list ",profile_num);
  }
  
  return;
} /*ds_3gpp_process_apn_disabled_cmd*/
/*===========================================================================
FUNCTION DS3GPP_SET_DS_LTE_FLAG

DESCRIPTION
  This function sets the lte flag based on whether we have disabled or 
  enabled LTE from ds module.   This is based on the boolean variable 

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE


SIDE EFFECTS
  None.

===========================================================================*/
void ds3gpp_set_ds_lte_flag
(
  boolean                 lte_flag,
  sys_modem_as_id_e_type  subs_id
)
{
  ds3gpp_per_subs_info_type* hndl = NULL;
  /*----------------------------------------------------------------------*/

  hndl = ds3gpp_get_per_subs_info_hndl(subs_id);

  if (ds3gpp_validate_per_subs_info_hndl(hndl))
  {
    if(lte_flag)
    { 
      hndl->current_ds_lte_flag = DS_3GPP_LTE_ENABLED; 
    }
    else
    {
      hndl->current_ds_lte_flag = DS_3GPP_LTE_DISABLED; 
    } 

    DS_3GPP_MSG2_MED("current_ds_lte_flag for Subs id(CM): %d is %d",
                      subs_id, hndl->current_ds_lte_flag);
  }
} /* ds3gpp_set_ds_lte_flag */

/*===========================================================================
FUNCTION DS3GPP_IS_LTE_FLAG_DISABLED

DESCRIPTION
  This function returns TRUE if LTE Flag is disabled

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3gpp_is_lte_flag_disabled
(
  sys_modem_as_id_e_type     subs_id
)
{
  boolean                    flag = FALSE;
  ds3gpp_per_subs_info_type* hndl = NULL;
  /*----------------------------------------------------------------------*/

  hndl = ds3gpp_get_per_subs_info_hndl(subs_id);

  if (ds3gpp_validate_per_subs_info_hndl(hndl))
  {
    if(hndl->current_ds_lte_flag == DS_3GPP_LTE_DISABLED)
    {
      flag = TRUE;
    }

    DS_3GPP_MSG2_MED("current_ds_lte_flag for Subs id(CM): %d is %d",
                      subs_id, hndl->current_ds_lte_flag);
  }

  return flag;
} /* ds3gpp_is_lte_flag_disabled */



/*===========================================================================
FUNCTION DS_3GPP_ROAMING_DISALLOWED_CHG_HDLR
 
DESCRIPTION
  This function is invoked when the Roaming Disallowed Flag is changed.

PARAMETERS 
  Profile number
  Roaming Disallowed Flag
  
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  void
 
SIDE EFFECTS 
  
===========================================================================*/
static void ds_3gpp_roaming_disallowed_chg_hdlr
(
  uint16  profile_num,
  sys_modem_as_id_e_type subs_id,
  boolean roaming_disallowed_flag
)
{
  ds_cmd_type             *cmd_ptr = NULL;
  ds_3gpp_roam_info_type  *roam_info_ptr = NULL;
  /*----------------------------------------------------------------------*/

  DS_3GPP_MSG2_HIGH("Roaming Disallowed flag changed for profile %d, "
                    "new value: %d",profile_num,roaming_disallowed_flag);

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_3gpp_roam_info_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_ROAMING_DISALLOWED_FLAG_CHG;
  roam_info_ptr = (ds_3gpp_roam_info_type*)cmd_ptr->cmd_payload_ptr;
  roam_info_ptr->profile_num = profile_num;
  roam_info_ptr->roaming_disallowed_flag = roaming_disallowed_flag;
  roam_info_ptr->subs_id = subs_id;
  ds_put_cmd_ext(cmd_ptr);
} /* ds_3gpp_roaming_disallowed_chg_hdlr */
#endif /* FEATURE_DATA_LTE */

/*===========================================================================
FUNCTION       DS_3GPP_APN_NAME_CHANGE_CB_HDLR

DESCRIPTION    Callback handler to process apn_name_change

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
 void ds_3gpp_apn_name_change_cb_hdlr
(
  uint16                             profile_num,
  sys_modem_as_id_e_type             subs_id
)
{
  ds_umts_pdp_profile_type           profile_info;
  ds_umts_pdp_profile_status_etype   result = DS_UMTS_PDP_FAIL;
  ds_umts_pdp_subs_e_type            ds_subs_id = DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(subs_id == SYS_MODEM_AS_ID_NONE)
  {
    DS_3GPP_MSG0_LOW("Error in subs_id");
    return;
  }

  ds_subs_id = dsumts_subs_mgr_get_subs_id(subs_id);

  memset(&profile_info, 0, sizeof(ds_umts_pdp_profile_type));

  result = ds_umts_get_pdp_profile_all_data_per_subs(profile_num, 
                                                     ds_subs_id, &profile_info);

  if( result != DS_UMTS_PDP_SUCCESS )
  {
    DS_3GPP_MSG1_LOW("Error in retrieving definition for prof number:%d",
                      profile_num);
    return;
  }
  
  ds_3gpp_roaming_process_disallowed_flag_chg_cmd(profile_num,
                                                  profile_info.roaming_disallowed, 
                                                  subs_id);
#ifdef FEATURE_DATA_LTE
  ds_3gpp_process_apn_disabled_cmd(profile_num, profile_info.apn_disable_flag, subs_id);

  ds_eps_kamgr_process_apn_name_chg(profile_num, subs_id);

  if (ds_dsd_apm_is_apn_in_attach_pdn_list(subs_id, (char*)profile_info.context.apn)) 
  {
     ds_3gpp_pdn_cntxt_enable_lte_each_plmnid(subs_id); 
      /*Disables current plmnid if profile available on plmn id*/
      ds_3gpp_pdn_cntxt_disable_lte_if_attach_profile_unavailable(subs_id);
  }
#endif 
  return;

}/* ds_3gpp_apn_name_change_cb_hdlr */

/*===========================================================================
FUNCTION DS_3GPP_APN_NAME_CHG_HDLR
 
DESCRIPTION
  This function is invoked when the APN name is changed.

PARAMETERS 
  Profile number
  Subs ID
  New APN name
  
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  void
 
SIDE EFFECTS 
  
===========================================================================*/
static void ds_3gpp_apn_name_chg_hdlr
(
  uint16                      profile_num,
  sys_modem_as_id_e_type      subs_id,
  void                        *old_apn_ptr
)
{
  ds_cmd_type                          *cmd_ptr = NULL; 
  ds_profile_db_apn_name_chg_info_type *apn_name_chg_info_ptr = NULL;
  byte                                 *old_apn = (byte *)old_apn_ptr;
 /*----------------------------------------------------------------------*/
#ifdef FEATURE_DATA_LTE
  ds_dsd_apm_apn_change_cb_hdlr(profile_num,subs_id,old_apn_ptr);
#endif

  if(old_apn == NULL)
  {
    DS_3GPP_MSG0_HIGH("New apn is NULL");
    return;
  }

  DATA_3GPP_MSG_SPRINTF_2(MSG_LEGACY_HIGH,"APN chg hdlr:New_apn:%s:profile_id:%d",
                          old_apn,profile_num);

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_profile_db_apn_name_chg_info_type));
  if( cmd_ptr == NULL || cmd_ptr->cmd_payload_ptr == NULL )
  {
    ASSERT(0);
  }        

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_PROFILE_DB_APN_NAME_CHANGE;
  apn_name_chg_info_ptr = (ds_profile_db_apn_name_chg_info_type *)cmd_ptr->cmd_payload_ptr;
  apn_name_chg_info_ptr->profile_num = profile_num;
  memscpy((void *)apn_name_chg_info_ptr->old_apn_name,
          DS_UMTS_MAX_APN_STRING_LEN+1,
          (void *)old_apn,
          DS_UMTS_MAX_APN_STRING_LEN+1);
  apn_name_chg_info_ptr->subs_id = subs_id;
  ds_put_cmd_ext(cmd_ptr);

  
  return;
}

/*===========================================================================
FUNCTION DS_3GPP_PDP_TYPE_CHG_HDLR
 
DESCRIPTION
  This function is invoked when the PDP type is changed.

PARAMETERS 
  Profile number
  New PDP Type 
  
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  void
 
SIDE EFFECTS 
  
===========================================================================*/
static void ds_3gpp_pdp_type_chg_hdlr
(
  uint16  profile_num,
  sys_modem_as_id_e_type subs_id,
  ds_umts_pdp_type_enum_type  new_pdp_type
)
{
  ds_cmd_type                     *cmd_ptr = NULL; 
  ds_profile_db_pdp_chg_info_type *pdp_chg_info_ptr = NULL;
  /*----------------------------------------------------------------------*/
  DS_3GPP_MSG2_HIGH("PDP type chged,Profile_Number:%d New PDP type:%d",
                    profile_num,new_pdp_type);

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_profile_db_pdp_chg_info_type));
  if( cmd_ptr == NULL || cmd_ptr->cmd_payload_ptr == NULL )
  {
    ASSERT(0);
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_PROFILE_DB_PDP_TYPE_CHANGE;
  pdp_chg_info_ptr = (ds_profile_db_pdp_chg_info_type*)cmd_ptr->cmd_payload_ptr;
  pdp_chg_info_ptr->profile_num = profile_num;
  pdp_chg_info_ptr->new_pdp_type = new_pdp_type;
  pdp_chg_info_ptr->subs_id = subs_id;
  ds_put_cmd_ext(cmd_ptr);
  return;
}


/*===========================================================================
FUNCTION DS3GPP_GET_CURRENT_NETWORK_MODE

DESCRIPTION
  This function returns the current mode within the 3GPP RATs. This mode
  is maintained by the 3GPP mode handler and is used to dispatch mode
  specific functions

PARAMETERS
  Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  sys_sys_mode_e_type - mode currently in

SIDE EFFECTS
  None.

===========================================================================*/
sys_sys_mode_e_type ds3gpp_get_current_network_mode
( 
  sys_modem_as_id_e_type     subs_id
)
{
  sys_sys_mode_e_type        mode = SYS_SYS_MODE_NONE;
  ds3gpp_per_subs_info_type* hndl = NULL;
  /*----------------------------------------------------------------------*/

  hndl = ds3gpp_get_per_subs_info_hndl(subs_id);

  if (ds3gpp_validate_per_subs_info_hndl(hndl))
  {
    mode = hndl->internal_sys_mode;
  }

  return mode;

} /* ds3gpp_get_current_network_mode */

/*===========================================================================
FUNCTION DS3GPP_UPDATE_NETWORK_MODE

DESCRIPTION
  This function updates the network mode maintained inside the 3GPP mode
  handler.

PARAMETERS
  network_mode - the mode to which we have to update.
  subs_id      - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds3gpp_update_network_mode
( 
  sys_sys_mode_e_type     network_mode,
  sys_modem_as_id_e_type  subs_id
)
{
  ds3gpp_per_subs_info_type* hndl = NULL;
  /*----------------------------------------------------------------------*/

  hndl = ds3gpp_get_per_subs_info_hndl(subs_id);

  if (ds3gpp_validate_per_subs_info_hndl(hndl))
  {
    DS_3GPP_MSG3_LOW("Updated internal sys mode for Subs id(CM): %d. Old: %d, "
                     "New: %d", subs_id, hndl->internal_sys_mode, network_mode);

    hndl->internal_sys_mode = network_mode;

  }
} /* ds3gpp_update_network_mode */

/*===========================================================================
FUNCTION DS_3GPP_PROCESS_OPRT_MODE_CHG_CMD

DESCRIPTION
  This function processes change in oprt_mode as advertized by
  CM_PH_EVENT_OPRT_MODE cmd
 
PARAMETERS
  oprt_mode - specfies the operating mode of the UE (specified by CM)
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_process_oprt_mode_chg_cmd
(
  sys_oprt_mode_e_type  oprt_mode
)
{
  ds_3gpp_event_info_u_type event_info;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  event_info.oprt_mode = oprt_mode;
  DS_3GPP_MSG1_HIGH("ds_3gpp_process_oprt_mode_chg_cmd: Invoking registered "
                    "callbacks for event: DS_3GPP_OPRT_MODE_CHG_EV, oprt_mode: %d", 
                    (uint32)oprt_mode);

  ds_3gpp_event_invoke_cbs(DS_3GPP_OPRT_MODE_CHG_EV, event_info);
}


/*===========================================================================
FUNCTION DS3GPP_GET_LAST_FULL_SRV_MODE

DESCRIPTION
  This function fetches the latest 3GPP mode in Full Service.

PARAMETERS
  subs_id      - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  Latest 3GPP mode to be in Full Service

SIDE EFFECTS
  None.

===========================================================================*/
sys_sys_mode_e_type ds3gpp_get_last_full_srv_mode
( 
  sys_modem_as_id_e_type     subs_id
)
{
  ds3gpp_per_subs_info_type* hndl = NULL;
  sys_sys_mode_e_type        mode = SYS_SYS_MODE_NONE;
  /*----------------------------------------------------------------------*/

  hndl = ds3gpp_get_per_subs_info_hndl(subs_id);

  if (ds3gpp_validate_per_subs_info_hndl(hndl))
  {
    DS_3GPP_MSG2_MED("Last full srv mode for Subs id(CM): %d is "
                     "%d", subs_id, hndl->last_full_srv_mode);

    mode = hndl->last_full_srv_mode;
  }

  return mode;
} /* ds3gpp_get_last_full_srv_mode */

/*===========================================================================
FUNCTION DS3GPP_UPDATE_LAST_FULL_SRV_MODE

DESCRIPTION
  This function records the latest 3GPP mode in Full Service.

PARAMETERS
  network_mode - the mode to which we have to update.
  subs_id      - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds3gpp_update_last_full_srv_mode
( 
  sys_sys_mode_e_type        network_mode,
  sys_modem_as_id_e_type     subs_id
)
{
  ds3gpp_per_subs_info_type* hndl = NULL;
  /*----------------------------------------------------------------------*/

  hndl = ds3gpp_get_per_subs_info_hndl(subs_id);

  if (ds3gpp_validate_per_subs_info_hndl(hndl))
  {
    DS_3GPP_MSG3_LOW("Updated last full srv for Subs id(CM): %d. Old: %d, "
                     "New: %d", subs_id, hndl->internal_sys_mode, network_mode);

    hndl->last_full_srv_mode = network_mode;

  }
} /* ds3gpp_update_last_full_srv_mode */

/*===========================================================================
FUNCTION  DS3GPP_SYS_CHANGE_HANDLER

DESCRIPTION

  Called by 3G DSMGR when the serving system changes.


PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void ds3gpp_sys_change_handler
(
 sys_modem_as_id_e_type        subs_id,
 sys_sys_mode_e_type           new_mode,
 sys_sys_mode_e_type           old_mode
)
{
  boolean                                    send_null_bearer = FALSE;
  ps_iface_ioctl_bearer_tech_changed_type    tech_change_info;
  static  sys_sys_mode_e_type                ds_full_service_mode
                                               = SYS_SYS_MODE_NONE;
  boolean                                    is_ps_srv_domain_pref = FALSE;
  sys_srv_domain_e_type                      srv_domain = SYS_SRV_DOMAIN_NO_SRV;
  sys_srv_status_e_type                      curr_srv_status = SYS_SRV_STATUS_NO_SRV;
#ifdef FEATURE_DATA_WLAN_MAPCON
  ds_sys_system_status_info_type             bearer_tech_ex;
  sint15                                     ps_errno;
#endif /* FEATURE_DATA_WLAN_MAPCON */
  boolean                                    advertise_empty_list = FALSE;
  ds3gpp_per_subs_info_type                 *hndl = NULL;
  sys_plmn_id_s_type                         current_camped_plmn;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_3GPP_MSG3_LOW("Sys change handler new mode:%d old mode:%d on sub id %d"
                      ,new_mode,old_mode,subs_id);

  memset(&current_camped_plmn, 0, sizeof(sys_plmn_id_s_type));

  hndl = ds3gpp_get_per_subs_info_hndl(subs_id);

  if (!ds3gpp_validate_per_subs_info_hndl(hndl))
  {
    return;
  }

  curr_srv_status = ds3g_get_current_srv_status_ex(subs_id);

  if (curr_srv_status == SYS_SRV_STATUS_SRV)
  {
    if (DS_3GPP_IS_3GPP_CURRENT_MODE(new_mode) == FALSE)
    {
      advertise_empty_list = TRUE;
    }

    ds_3gpp_roaming_advertise_apns_blocked_due_to_roaming
      (subs_id, advertise_empty_list);
  }

  srv_domain = ds3g_get_current_regd_srv_domain_ex(subs_id);

  if(srv_domain == SYS_SRV_DOMAIN_PS_ONLY ||
     srv_domain == SYS_SRV_DOMAIN_CS_PS)
  {
    is_ps_srv_domain_pref = TRUE;
  }

#ifdef FEATURE_DATA_LTE

  ds_eps_pdn_cntxt_sys_change_handler(new_mode, old_mode,subs_id);

#endif /* FEATURE_DATA_LTE */

#ifndef FEATURE_DATA_WCDMA_PS
if (new_mode == SYS_SYS_MODE_WCDMA)
{
  DS_3GPP_MSG0_ERROR("Sys change handler invalid feature WCDMA undefined");
  return;
}
#endif /* FEATURE_DATA_WCDMA_PS */

#ifndef FEATURE_GSM_GPRS
if (new_mode == SYS_SYS_MODE_GSM)
{
  DS_3GPP_MSG0_ERROR("Sys change handler invalid feature GSM_GPRS undefined");
  return;
}
#endif /* FEATURE_GSM_GPRS */

#ifndef FEATURE_TDSCDMA
if (new_mode == SYS_SYS_MODE_TDS)
{
  DS_3GPP_MSG0_ERROR("Sys change handler invalid feature TDSCDMA undefined");
  return;
}
#endif /* FEATURE_TDSCDMA */

/*---------------------------------------------------------------------------
  Calling umts sys change handler to teardown all the calls if new system RAT
  is CDMA and old RAT is 3GPP except LTE 
---------------------------------------------------------------------------*/
  if (((old_mode == SYS_SYS_MODE_WCDMA)||
       (old_mode == SYS_SYS_MODE_GSM)
#ifdef FEATURE_TDSCDMA
         ||
       (old_mode == SYS_SYS_MODE_TDS)
#endif /*FEATURE_TDSCDMA*/
      )&&
     (new_mode == SYS_SYS_MODE_CDMA ||
      new_mode == SYS_SYS_MODE_HDR))
  {
    DS_3GPP_MSG0_HIGH("Tearing down calls on G/W/T RAT as 3GPP2 is acquired ");
	
#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
    dsUmtsBearerCntxt_teardown_all_calls(subs_id);
#endif

  }

  if(DS_3GPP_IS_3GPP_CURRENT_MODE(new_mode))
  {
    ds3gpp_update_network_mode(new_mode, subs_id); 

    if(is_ps_srv_domain_pref == TRUE)
    {
      /*----------------------------------------------------------------------- 
        This is to handle the following scenario:
        1. UE is camped on W and RPLMN.
        2. UE tried for W-->L IRAT.
        3. PLMN change indication HPLMN and context transfer indication.
        4. TAU is dropped by NW  (No LTE Radio reported in SS event).
        5. UE moved back to W.
        6. Now current camped PLMN should be RPLMN.
        7. But, DS3G won't post PLMN change as PLMN in DS3G cache is same.
      -----------------------------------------------------------------------*/
      if (new_mode == SYS_SYS_MODE_WCDMA ||
          new_mode == SYS_SYS_MODE_GSM ||
          new_mode == SYS_SYS_MODE_TDS)
      {
        ds3g_get_current_plmn_id(&current_camped_plmn, subs_id);
        if (TRUE == ds_3gpp_throt_sm_is_plmn_non_null(current_camped_plmn))
        {
          ds_3gpp_roaming_inform_new_plmn_info(subs_id, current_camped_plmn);
        }
      }  

      /*------------------------------------------------------------------------- 
        If old_srv_domain is SYS_SRV_DOMAIN_CS_ONLY or SYS_SRV_DOMAIN_NO_SRV or
        SYS_SRV_DOMAIN_CAMPED and new srv_domain is PS srv domain
        reset pdn blocked in apn reject sm
        ------------------------------------------------------------------------*/
      if((hndl->old_srv_domain  == SYS_SRV_DOMAIN_CS_ONLY) || 
         (hndl->old_srv_domain  == SYS_SRV_DOMAIN_NO_SRV)  ||
         (hndl->old_srv_domain  == SYS_SRV_DOMAIN_CAMPED))
      {
        /* Reset PDN blocked in APN reject SM for new network attach */
        ds_3gpp_pdn_throt_set_apn_reject_pdn_blocked(FALSE, subs_id);
      }
    /* Reset Service request throttling on RAT change only for a specific config */
      if (ds_3gpp_throttle_is_srv_req_throttle_behavior_t3402_enabled(subs_id))
      {
        if ( (ds3g_get_current_srv_status() == SYS_SRV_STATUS_SRV) &&
             (SYS_SYS_MODE_LTE != new_mode) )
        {
#if defined(FEATURE_DATA_LTE) && (defined(FEATURE_GSM_GPRS) || defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_TDSCDMA))

          DS_3GPP_MSG0_LOW("Sys mode changed, reset Service Request throttling SM if needed");
          ds_3gpp_bearer_cntxt_reset_srv_req_throt(subs_id, new_mode);
#endif
        }
      }  

      ds_bearer_cntxt_sys_change_handler(new_mode, subs_id);

      /*---------------------------------------------------------------------
        Advertise throttling information to MPPM and APM
      ---------------------------------------------------------------------*/

      if (ds_3gpp_nv_manager_get_enable_pdn_throt_per_rat(subs_id))
      {
        if (ds3g_get_current_srv_status_ex(subs_id) == SYS_SRV_STATUS_SRV || 
            new_mode == SYS_SYS_MODE_LTE)
        {
          ds_3gpp_throt_sm_advertise_blocked_apns_to_external_clients(subs_id);
        }
      }

      if(ds3g_get_current_srv_status_ex(subs_id) == SYS_SRV_STATUS_SRV)
      {
    
        ds3gpp_update_last_full_srv_mode(new_mode,subs_id);
    
        /*---------------------------------------------------------------------
          Resume functionality should be invoked only when FULL_SRV on new RAT 
          is reported
        ---------------------------------------------------------------------*/
        DS_BEARER_CTXT_VF_CALL
        (
          resume_validate,
          subs_id,
          new_mode,
          old_mode,
          subs_id
        );

        /*---------------------------------------------------------------------
          fill in tech change info structure based on old mode
        ---------------------------------------------------------------------*/
        ds_3gpp_pdn_cntx_get_bearer_tech_type(
                                      old_mode,
                                      &(tech_change_info.old_bearer_tech),
                                      subs_id);
    
        /*---------------------------------------------------------------------
          fill in tech change info structure based on new mode
        ---------------------------------------------------------------------*/
        ds_3gpp_pdn_cntx_get_bearer_tech_type(
                                    new_mode,
                                    &(tech_change_info.new_bearer_tech),
                                    subs_id);
    
       /*----------------------------------------------------------------------
         Post IFACE_BEARER_TECH_CHANGED_EV
       ----------------------------------------------------------------------*/
        ds_3gpp_pdn_cntx_post_iface_generic_ind(
                                IFACE_BEARER_TECH_CHANGED_EV,
                                (void*)&tech_change_info,
                                subs_id);
      /*  Inter-RAT handover (G->W and W->G ): Reset cause code specifc throttling */
        if((ds_3gpp_throt_sm_is_cc_sp_throttling_enabled(subs_id) == TRUE) && 
            (new_mode != ds_full_service_mode) )
        {
          // call throtling reset
          ds_3gpp_pdn_throt_reset_pdn_throttle(subs_id);
        }
    
        ds_full_service_mode = new_mode;
    

#ifdef FEATURE_DATA_WLAN_MAPCON
        /*---------------------------------------------------------------------
          fill in bearer tech info structure based on new mode
        ---------------------------------------------------------------------*/
        if( ds_3gpp_bearer_cntxt_get_bearer_type_ex( new_mode,
                                                     &bearer_tech_ex,
                                                     &ps_errno,
                                                     subs_id ) == 0)
        {
          /*-------------------------------------------------------------------
            Post IFACE_BEARER_TECH_EX_CHANGED_EV
          -------------------------------------------------------------------*/
          ds_3gpp_pdn_cntx_post_iface_generic_ind(
                                  IFACE_BEARER_TECH_EX_CHANGED_EV,
                                  (void *)&bearer_tech_ex,
                                  subs_id);
        }
        else
        {
          DS_3GPP_MSG0_ERROR("Could not fetch bearer tech ex info");
        }
#endif /* FEATURE_DATA_WLAN_MAPCON */
  
      }
      else
      {
        /*System is not in Full Service. Send NULL Bearer*/
        send_null_bearer = TRUE;
      }
    }
    else
    {
      send_null_bearer = TRUE;
    }
  }

  if(new_mode == SYS_SYS_MODE_NO_SRV &&
     (
       (old_mode == SYS_SYS_MODE_WCDMA) ||
       (old_mode == SYS_SYS_MODE_GSM)||
       (old_mode == SYS_SYS_MODE_LTE)
#ifdef FEATURE_TDSCDMA
       ||
       (old_mode == SYS_SYS_MODE_TDS)
#endif
      )
    )
  {
   /* System moved from one of the 3GPP modes to NO SRV mode.
      Send NULL Bearer*/
    send_null_bearer = TRUE;
  }


  if(send_null_bearer)
  {
    /*-----------------------------------------------------------------------
       Post IFACE_BEARER_TECH_CHANGED_EV
    -----------------------------------------------------------------------*/
    ds_3gpp_pdn_cntx_get_bearer_tech_type(old_mode,
                                          &(tech_change_info.old_bearer_tech),
                                          subs_id);

    tech_change_info.new_bearer_tech.current_network = PS_IFACE_NETWORK_3GPP;
    tech_change_info.new_bearer_tech.data.umts_type.rat_mask =
      PS_IFACE_3GPP_NULL_BEARER;

    ds_3gpp_pdn_cntx_post_iface_generic_ind(
                              IFACE_BEARER_TECH_CHANGED_EV,
                              (void *)&tech_change_info,
                              subs_id);

#ifdef FEATURE_DATA_WLAN_MAPCON
    /*-------------------------------------------------------------------------
      fill in bearer tech info structure based on new mode
    -------------------------------------------------------------------------*/
    if (ds_3gpp_bearer_cntxt_get_bearer_type_ex( new_mode,
                                                 &bearer_tech_ex,
                                                 &ps_errno ,
                                                 subs_id ) == 0)
    {
      /*----------------------------------------------------------------------------
        Post IFACE_BEARER_TECH_EX_CHANGED_EV
      -----------------------------------------------------------------------------*/
      ds_3gpp_pdn_cntx_post_iface_generic_ind(
                             IFACE_BEARER_TECH_EX_CHANGED_EV,
                             (void *)&bearer_tech_ex,
                             subs_id);
    }
    else
    {
      DS_3GPP_MSG0_ERROR("Could not fetch bearer tech ex info");
    }
#endif /* FEATURE_DATA_WLAN_MAPCON */

  }

  if((old_mode == SYS_SYS_MODE_CDMA ||
      old_mode == SYS_SYS_MODE_HDR) &&
     (new_mode == SYS_SYS_MODE_LTE))
  {
    ds_3gpp_bearer_cntxt_reorig_all_dorm_bearers(subs_id);
  }

  /*-------------------------------------------------------------------------
    Initialize the bearer throughput stats parameters if we are moving to
    3GPP mode and the report throughput Info indication timer is running
  -------------------------------------------------------------------------*/
  if (((DS_3GPP_IS_3GPP_CURRENT_MODE(old_mode) == FALSE) ||
       (old_mode == SYS_SYS_MODE_NO_SRV)) &&
      (DS_3GPP_IS_3GPP_CURRENT_MODE(new_mode) == TRUE) &&
      (is_ps_srv_domain_pref == TRUE))
  {
    DS_3GPP_MSG0_LOW("Sys chg hdlr: Initializing Bearer throughput Stats");
    ds_3gpp_bearer_context_init_bearer_throughput_stats(subs_id);
  }
  /*----------------------------------------------------------------------- 
    Update the old srv_domain here
    -----------------------------------------------------------------------*/
  hndl->old_srv_domain = srv_domain;
  return;
} /* ds3gpp_sys_change_handler() */

#ifdef FEATURE_WCDMA
/*===========================================================================
FUNCTION DS_3GPP_PS_DATA_GENERIC_IND_HDLR()

DESCRIPTION
  This function is called by 3G DSManager when certain lower layer events 
  occur. These events are generally needed by upper layers or applications.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void ds_3gpp_ps_data_generic_ind_hdlr
(
  ds3g_call_info_cmd_type        *call_info_ptr,
  sys_modem_as_id_e_type          subs_id

)
{
  void                              *event_data_ptr = NULL;
  uint8                              bearer_inst;
  ds_bearer_context_s               *bearer_context_ptr;
  ps_iface_fast_dormancy_status_type fd_status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(call_info_ptr != NULL);

  DATA_MSG0_MED("ds_3gpp_ps_data_generic_ind_hdlr()");

  switch(call_info_ptr->call_event)
  {
    case CM_CALL_EVENT_PS_SIG_REL_CNF:
    {
      DS_3GPP_MSG0_HIGH("Processing fast dormancy status CM callback");
      /*-------------------------------------------------------------------------
        Determine the error code and generate an indication to PS.
      -------------------------------------------------------------------------*/
      switch(call_info_ptr->mode_info.info.gw_ps_call.ps_sig_rel_status)
      {
        case PS_SIG_REL_SUCCESS:
          fd_status.dorm_status = PS_FAST_DORMANCY_STATUS_SUCCESS;
          fd_status.failure_reason = PS_FAST_DORMANCY_STATUS_FAIL_NONE;
          break;

        case PS_SIG_REL_FAILURE:
          fd_status.dorm_status = PS_FAST_DORMANCY_STATUS_FAILURE;
          fd_status.failure_reason = PS_FAST_DORMANCY_STATUS_FAIL_RRC;
          break;

        default:
          DS_3GPP_MSG1_ERROR("ds_3gpp_ps_data_generic_ind_hdlr: Invalid ps_sig_rel_status %d", 
                    call_info_ptr->mode_info.info.gw_ps_call.ps_sig_rel_status);
          /* Don't generate an event since we don't know the status for this. */
          return;
      }

      event_data_ptr = (void *)(&fd_status);

      /*-------------------------------------------------------------------------
        Generate a ps_iface event for all corresponding ifaces.
      -------------------------------------------------------------------------*/
      for (bearer_inst = 0; 
                  bearer_inst < DS_3GPP_MAX_BEARER_CONTEXT; 
                          bearer_inst++)
                                       
      {
        bearer_context_ptr = ds_bearer_cntxt_get_instance_ptr(bearer_inst);

        if(!ds_bearer_cntx_validate_bearer_subscription_internal
            (bearer_context_ptr, subs_id))
        {
          continue;
        }
        else if (bearer_context_ptr->ds_bearer_context_dyn_p->is_default)
        {
          ds_3gpp_bearer_gen_fast_dormancy_event(bearer_context_ptr, 
                                                 subs_id, 
                                    event_data_ptr );
        }
      } /* for each bearer */
      
    } /* CM_CALL_EVENT_PS_SIG_REL_CNF */
    break;
    
    default:
      DATA_MSG1_ERROR("ds_3gpp_ps_data_generic_ind_hdlr :Unhandled call event %d", 
                call_info_ptr->call_event);
  }
} /* ds_3gpp_ps_data_generic_ind_handler() */

#endif /* FEATURE_WCDMA */
/*===========================================================================
FUNCTION  DS_EPS_PDN_CNTXT_PREF_SYS_CHG_HDLR

DESCRIPTION
  This function calls teh eps pdn contect function which is responsible
  for the teardown of the context when the preferred system changes,
  as indication from CNE-DSD

PARAMETERS
  cne_sys_e_type old_pref_sys: New preferred system we are in
  cne_sys_e_type new_pref_sys: Old preferred system we are in

DEPENDENCIES
  presence of cne-dsd api's

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
#ifdef FEATURE_DATA_LTE
LOCAL void ds3gpp_dsd_pref_sys_chg_hdlr
(
  sys_sys_mode_e_type old_pref_sys,
  sys_sys_mode_e_type new_pref_sys
)
{

  DS_3GPP_MSG2_MED("dsd pref sys change handler old sys:%d new sys:%d",
                    old_pref_sys, new_pref_sys);

  /*-------------------------------------------------------------------------
    Ignore pref system change event for now. We want to avoid tearing down
    LTE contexts too soon (as CNE's handdown timer is only 22s. We will
    wait for EPC's EDCT timer to expire before tearing down LTE contexts.
  -------------------------------------------------------------------------*/
  return;
}
#endif /* FEATURE_DATA_LTE */

/*===========================================================================
FUNCTION  DS3GPP_ORIG_CALL_HDLR_EXT

DESCRIPTION
  This function is invoked when the user initiates a data call. It specifies
  the origination parameters for a PS call.

PARAMETERS   :
  *dial_string              - Ptr to Dial string
  dial_string_len           - Len of dial string
  orig_params               - structure containing pointers to orig params
                              of different modes. This also has the flags
                              indicating if the orig params of the corres
                              mode got modified.
  cm_srv_type               - CM Service type. Ignored.
  mode_info_ptr             - Any mode specific information. For UMTS
                              contains the call instance.
  subs_id                   - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL boolean ds3gpp_orig_call_hdlr_ext
(
  const byte                           *dial_string,
        uint8                          dial_string_len,
        ds3g_orig_params_s             orig_params,
        cm_srv_type_e_type             *cm_srv_type,
        void                           *mode_info_ptr,
        sys_modem_as_id_e_type          subs_id
)
{

  const ds_3gpp_pdn_call_info_type *orig_info_ptr; /* Ptr to Orig Info        */
  ds_3gpp_orig_params_s            mm_orig_params;
  sys_sys_mode_e_type              network_mode;
  ds_bearer_context_s              *bearer_cntx_p;
  ds_pdn_context_s                 *pdn_cntx_p;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  orig_info_ptr = (const ds_3gpp_pdn_call_info_type *)mode_info_ptr;

  ASSERT(orig_info_ptr != NULL );

  /*-------------------------------------------------------------------------
    Get the bearer cntx
  -------------------------------------------------------------------------*/
  bearer_cntx_p = ds_bearer_cntxt_get_bearer_cntxt((const void *)mode_info_ptr);

  if(ds_bearer_cntx_validate_bearer_context(bearer_cntx_p) == FALSE)
  {
    ASSERT(0);
    return DS3G_FAILURE;
  }


  DS_3GPP_MSG2_HIGH("3GPP Call_Orig_hdlr_ext bearer cntx 0x%x inst %d",
           bearer_cntx_p,orig_info_ptr->call_instance);

  /*-------------------------------------------------------------------------
    Get the PDN cntx assigned to this bearer cntx
  -------------------------------------------------------------------------*/
  pdn_cntx_p = (ds_pdn_context_s*)ds_bearer_cntxt_get_pdn_cntxt(bearer_cntx_p);

  /*-------------------------------------------------------------------------
    Get the current network mode.
  -------------------------------------------------------------------------*/
  network_mode = ds3gpp_get_current_network_mode(subs_id);
  if( !DS_3GPP_IS_3GPP_CURRENT_MODE(network_mode))
  {
    DS_3GPP_MSG1_ERROR("DS 3GPP Invalid network_mode:%d",network_mode);
    return DS3G_FAILURE;
  }

  /*-------------------------------------------------------------------------
    Memset the container that carries GW PS or LTE PS orig params.
  -------------------------------------------------------------------------*/
  memset(&mm_orig_params,
         0x0,
         sizeof(ds_3gpp_orig_params_s));

  mm_orig_params.network_mode = network_mode;

  /*-------------------------------------------------------------------------
    Ensure that all the flags denoting a change in the respective params
    structure are reset to FALSE
  -------------------------------------------------------------------------*/
  if(orig_params.cdma_orig_params_changed != NULL)
  {
    *(orig_params.cdma_orig_params_changed) = FALSE;
  }

  if(orig_params.gw_cs_orig_params_changed != NULL)
  {
    *(orig_params.gw_cs_orig_params_changed) = FALSE;
  }

  if(orig_params.gw_ps_orig_params_changed != NULL)
  {
    *(orig_params.gw_ps_orig_params_changed) = FALSE;
  }
#ifdef FEATURE_DATA_LTE
  if(orig_params.lte_orig_params_changed != NULL)
  {
    *(orig_params.lte_orig_params_changed) = FALSE;
  }
#endif /* FEATURE_DATA_LTE */

  /*-------------------------------------------------------------------------
    Set the service type and the changed flag according to the mode

    Map the incoming params into the appropriate fields of the structure
    mm_orig_params. this is the struct that will be passed into the mode
    specific orig call handlers
  -------------------------------------------------------------------------*/
  if(
     network_mode == SYS_SYS_MODE_WCDMA ||
     network_mode == SYS_SYS_MODE_GSM
#ifdef FEATURE_TDSCDMA
      ||
      network_mode == SYS_SYS_MODE_TDS
#endif
    )
  {
    mm_orig_params.orig_params_u.gw_ps_orig_params_ptr =
                                               orig_params.gw_ps_orig_params_p;
    /*-------------------------------------------------------------------------
      Set CM service type to be GSM, WCDMA , LTE or TDS
      CM decide to use it similar based on mode preference
    -------------------------------------------------------------------------*/
    *cm_srv_type  = CM_SRV_TYPE_TDS_GSM_WCDMA_LTE;

    if (orig_params.gw_ps_orig_params_changed != NULL)
    *(orig_params.gw_ps_orig_params_changed) = TRUE;
  }
#ifdef FEATURE_DATA_LTE
  else if(network_mode == SYS_SYS_MODE_LTE)
  {
    mm_orig_params.orig_params_u.lte_ps_orig_params_ptr =
                                               orig_params.lte_orig_params_p;

    if (orig_params.lte_orig_params_changed != NULL)
    *(orig_params.lte_orig_params_changed) = TRUE;
  }
#endif /* FEATURE_DATA_LTE */

  if(bearer_cntx_p->ds_bearer_context_dyn_p->is_default)
  {
    if(ds_pdn_cntxt_get_orig_params(pdn_cntx_p,
                                    &mm_orig_params,
                                    orig_info_ptr
                                   )!= DS3G_SUCCESS )
    {
      return DS3G_FAILURE;
    }
  }

  return ds_bearer_cntxt_get_orig_params(bearer_cntx_p,
                                         &mm_orig_params,
                                         orig_info_ptr
                                        );

} /* ds3gpp_orig_call_hdlr_ext */

/*===========================================================================
FUNCTION  ds3gpp_orig_call_hdlr

DESCRIPTION
  This function is invoked when the user initiates a data call. It specifies
  the origination parameters for a PS call.

PARAMETERS   :
  *dial_string              - Ptr to Dial string
  dial_string_len           - Len of dial string
  *cdma_orig_params_ptr     - Ptr to CMDA call origination parameters.
                              Ignored for PS call.
  *cdma_params_changed      - Boolean indicating whether CDMA origination
                              parameters changed or not. Ignored for
                              PS call.
  *gw_cs_orig_params_ptr    - Ptr to CS call origination parameters. Not used
                              for PS call.
  *gw_cs_params_changed     - Boolean indicating whether CS origination
                              parameters changed or not. Ignored for
                              PS call.
  *gw_ps_orig_params_ptr    - Ptr to PS call origination parameters.
  *gw_ps_params_changed     - Boolean indicating whether PS origination
                              parameters changed or not.
  cm_srv_type               - CM Service type. Ignored.
  mode_info_ptr             - Any mode specific information. For UMTS
                              contains the call instance.
  subs_id                   - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL boolean ds3gpp_orig_call_hdlr
(
  const byte                           *dial_string,
        uint8                           dial_string_len,
        cm_cdma_orig_params_s_type     *cdma_orig_params_ptr,
        boolean                        *cdma_params_changed,
        cm_gw_cs_orig_params_s_type    *gw_cs_orig_params_ptr,
        boolean                        *gw_cs_params_changed,
        cm_gw_ps_orig_params_s_type    *gw_ps_orig_params_ptr,
        boolean                        *gw_ps_params_changed,
        cm_srv_type_e_type             *cm_srv_type,
        void                           *mode_info_ptr,
        sys_modem_as_id_e_type          subs_id
  /*lint -esym(715,dial_string, dial_string_len) */
  /*lint -esym(715,cdma_orig_params_ptr,gw_cs_orig_params_ptr) */
  /*lint -esym(818,cdma_orig_params_ptr,gw_cs_orig_params_ptr) */
  /*lint -esym(818,mode_info_ptr) */
)
{
  ds3g_orig_params_s orig_params;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Map the incoming params into the struct taken by orig_call_hdlr_ext */
  orig_params.cdma_orig_params_p = cdma_orig_params_ptr;
  orig_params.cdma_orig_params_changed = cdma_params_changed;

  orig_params.gw_cs_orig_params_p = gw_cs_orig_params_ptr;
  orig_params.gw_cs_orig_params_changed = gw_cs_params_changed;

  orig_params.gw_ps_orig_params_p = gw_ps_orig_params_ptr;
  orig_params.gw_ps_orig_params_changed = gw_ps_params_changed;

#ifdef FEATURE_DATA_LTE
  orig_params.lte_orig_params_p = NULL;
  orig_params.lte_orig_params_changed = NULL;
#endif /* FEATURE_DATA_LTE */

  return ds3gpp_orig_call_hdlr_ext(dial_string,
                                   dial_string_len,
                                   orig_params,
                                   cm_srv_type,
                                   mode_info_ptr,
                                   subs_id);

} /* dsumtsps_orig_call_hdlr() */
  /*lint +esym(715,dial_string, dial_string_len) */
  /*lint +esym(715,cdma_orig_params_ptr,gw_cs_orig_params_ptr) */
  /*lint +esym(818,cdma_orig_params_ptr,gw_cs_orig_params_ptr) */
  /*lint +esym(818,mode_info_ptr) */

/*===========================================================================
FUNCTION  DSUMTSPS_CALL_ENDED_HDLR

DESCRIPTION
  This function is invoked when the CM_CALL_EVENT_END event is received from
  Call Mgr. The function clean ups all call-related state and shut down
  protocol layers.

PARAMETERS
  cmd_ptr   - Ptr to the command information
  subs_id   - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void ds3gpp_call_ended_hdlr
(
  ds_cmd_type            *cmd_ptr_i,
  boolean                 call_was_connected, /*lint -esym(818, cmd_ptr_i) */
  sys_modem_as_id_e_type  subs_id
)
{
  const ds_cmd_type *cmd_ptr = cmd_ptr_i;
#ifdef FEATURE_QXDM_WM_TRACING
  uint32 callid;
#endif /* FEATURE_QXDM_WM_TRACING */

  ds_bearer_context_s           *bearer_cntx_p;
  sys_sys_mode_e_type            network_mode;
  ds3g_call_info_cmd_type       *call_info_cmd_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DS_3GPP_MSG0_ERROR( "NULL pointer passed" );
    return;
  }
  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  /*-------------------------------------------------------------------------
     Get the bearer cntx for the call ID.
  -------------------------------------------------------------------------*/
  bearer_cntx_p =
      ds_bearer_cntxt_get_bearer_from_call_id(call_info_cmd_ptr->call_id);

  if(!ds_bearer_cntx_validate_bearer_context(bearer_cntx_p))
  {
    return;
  }

  /*-------------------------------------------------------------------------
    Get the current network mode.
  -------------------------------------------------------------------------*/
  network_mode = ds3gpp_get_current_network_mode(subs_id);

   if( !DS_3GPP_IS_3GPP_CURRENT_MODE(network_mode))
  {
    DS_3GPP_MSG3_ERROR("DS 3GPP Invalid network_mode:%d",network_mode,0,0);
    return;
  }

  /*-------------------------------------------------------------------------
    Call mode specific function to handle SDF-ID.
  -------------------------------------------------------------------------*/
  ds_flow_manager_check_sdf_id(
            (cm_call_mode_info_s_type*) &(call_info_cmd_ptr->mode_info), subs_id);



  ds_bearer_cntxt_deactivate_cnf
  (
    bearer_cntx_p,
    call_info_cmd_ptr,
    0,                                              //regular deactivation
    call_was_connected
  );

#ifdef FEATURE_QXDM_WM_TRACING
  /*-------------------------------------------------------------------------
    Deregister the watermarks from FC.
  -------------------------------------------------------------------------*/
  callid = ds_bearer_cntxt_get_call_id(bearer_cntx_p);   //Get the call_id

  switch (network_mode)
  {
    case SYS_SYS_MODE_GSM:
    case SYS_SYS_MODE_WCDMA:
    case SYS_SYS_MODE_LTE:
      DS_3GPP_MSG2_MED("MH Client: Deregistering from FC for call id %d network_mode = %d",
              callid, network_mode);
      fc_post_deregister_cmd_wm_trace(callid, UNIFIED_3GPP_MODE_HANDLER_CLIENT);
      break;

    default:
      DS_3GPP_MSG1_MED("MH: Not deregistering for UNKNOWN tech, call id %d",
              callid);
      break;
  }
#endif /* FEATURE_QXDM_WM_TRACING */
} /* ds3gpp_call_ended_hdlr() */
  /*lint +esym(818, cmd_ptr_i) */

/*===========================================================================
FUNCTION  DSUMTSPS_CALL_CALL_ID_HDLR

DESCRIPTION
  This function is invoked by 3G DS manager to specify the call id allcoated
  for the call. This is called by 3G DS manager immediately after the orig
  call handler is called.

  The mode_info_ptr points to the call instance that was grabbed during
  route alloction. Populate the call id into call instance

PARAMETERS
  call_id         - Call ID
  mode_info_ptr_i - Mode Info Ptr passed by CM
  subs Id         - Subscription Id
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
 None.

===========================================================================*/
LOCAL void ds3gpp_call_id_hdlr
(
  cm_call_id_type         call_id,
  void                   *mode_info_ptr_i, /*lint -esym(818, mode_info_ptr_i)*/
  sys_modem_as_id_e_type  subs_id
)
{
  const void * mode_info_p = mode_info_ptr_i;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_DUAL_SIM
  ds_bearer_context_s    *bearer_context_p = NULL;
#endif /* FEATURE_DUAL_SIM */
  
  if(!ds_bearer_cntxt_update_call_id(mode_info_p, call_id))
  {
    DATA_ERR_FATAL(" Incorrect mode_info in ds3gpp_call_id_hdlr");
  }

  /*----------------------------------------------------------------------------
    Updating bearer_cntxt_call_mode.
  ----------------------------------------------------------------------------*/
#ifdef FEATURE_DUAL_SIM
  bearer_context_p = ds_bearer_cntxt_get_bearer_cntxt(mode_info_p);

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }
  
  bearer_context_p->ds_bearer_context_dyn_p->call_mode = 
    ds3gpp_get_current_network_mode(subs_id);
  
#endif /* FEATURE_DUAL_SIM */
} /* ds3gpp_call_id_hdlr() */
  /*lint +esym(818, mode_info_ptr_i) */

/*===========================================================================
FUNCTION DSUMTSPS_USER_END_HDLR

DESCRIPTION
  This function is invoked when the user or TE ends the call. It may attempt
  to shut down lower layer protocols.

PARAMETERS
  call_id     Call identifier
  *end_params Call end parameters
  subs_id     Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void ds3gpp_user_end_hdlr
(
  cm_call_id_type          call_id,
  cm_end_params_s_type     *end_params,
  boolean                  *delay_disconnect,
  void                     *user_end_ptr,
  sys_modem_as_id_e_type   subs_id
)
{
  ds_bearer_context_s          *bearer_cntx_p;/*Index into the call table*/
  ds_3gpp_pdn_call_info_type   *mode_sp_info;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(user_end_ptr == NULL)
  {
    ASSERT( 0);
    return;
  }
  mode_sp_info = (ds_3gpp_pdn_call_info_type *)(user_end_ptr);

  if(mode_sp_info->mt_flag == TRUE)
  {
    DS_3GPP_MSG1_HIGH("MT call end with call id: %d",call_id);
    *delay_disconnect = FALSE;
    mode_sp_info->mt_flag = FALSE;
    end_params->call_id   = call_id;
    end_params->info_type = CM_CALL_MODE_INFO_GW_PS;
    end_params->end_params.gw_ps_end.cause = SM_ACTIVATION_REJECTED_UNSPECIFIED;
    return;
  }

  bearer_cntx_p = ds_bearer_cntxt_get_bearer_cntxt(user_end_ptr);
  ASSERT(bearer_cntx_p != NULL);

  /*-------------------------------------------------------------------------
    Configure the IFACE with mode_info_ptr configuration
  -------------------------------------------------------------------------*/
  ds_bearer_cntxt_get_end_params
  (
    bearer_cntx_p,
    end_params,
    mode_sp_info,
    delay_disconnect
  );
} /* ds3gpp_user_end_hdlr() */

/*===========================================================================
FUNCTION DS3GPP_PER_SUBS_IS_SERVICE_REQ_ALLOWED_ON_PLMN

DESCRIPTION
  This function is used to check if the service request throttling timer is 
  currently running on the PLMN passed into the function.

  If the throttling algorithm has never been kicked off in this PLMN or if the
  throttling timer is not running currently for the PLMN, the UE can send the 
  service request over the air.

PARAMETERS
  plmn_id - The PLMN id in which this the service request is to be sent over air
  subs_id - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If it is ok to send Service request over the air for this PLMN
  FALSE - otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean ds3gpp_per_subs_is_service_req_allowed_on_plmn
(
  sys_plmn_id_s_type          plmn_id,
  sys_modem_as_id_e_type      subs_id
)
{
  void*    sm_ptr  = NULL;
  boolean  ret_val = TRUE;
  /*-----------------------------------------------------------------------*/

  DS_3GPP_ENTER_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);

  /*----------------------------------------------------------------------------
    Get a handle to the Service request pointer. Remember that here we are not
    looking to instantiate a core state machine.
  ----------------------------------------------------------------------------*/
  sm_ptr = ds_3gpp_throt_sm_get_throttling_sm(FALSE, subs_id);

  if(sm_ptr != NULL)
  {
    if(ds_3gpp_throt_sm_get_throttling_state(sm_ptr, &ret_val, subs_id) == TRUE)
    {
      ret_val = !ret_val;
      DS_3GPP_MSG1_MED("Throt SM consulted. Return %d to NAS",ret_val);
    }
    else
    {
      DS_3GPP_MSG0_HIGH("Func to query throt state ret FALSE. Allow srv req");
      /*------------------------------------------------------------------------
        Set the ret_val to TRUE. This is just to be sure that the function that
        we called did not reset the contents of the memory pointed to by the
        pointer passed into the function.
      ------------------------------------------------------------------------*/
      ret_val = TRUE;
    }
  }
  else
  {
    DS_3GPP_MSG0_HIGH("Srv req throt sm not avail. Allow srv req");
  }
  DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);
  return ret_val;
} /* ds3gpp_per_subs_is_service_req_allowed_on_plmn */

/*===========================================================================
FUNCTION DS3GPP_IS_SERVICE_REQ_ALLOWED_ON_PLMN

DESCRIPTION
  This function is used to check if the service request throttling timer is
  currently running on the PLMN passed into the function.

  If the throttling algorithm has never been kicked off in this PLMN or if the
  throttling timer is not running currently for the PLMN, the UE can send the
  service request over the air.

PARAMETERS
  plmn_id - The PLMN id in which this the service request is to be sent over air

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If it is ok to send Service request over the air for this PLMN
  FALSE - otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3gpp_is_service_req_allowed_on_plmn
(
  sys_plmn_id_s_type          plmn_id
)
{
  sys_modem_as_id_e_type      subs_id = ds3g_get_ps_subs_id();
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ds3gpp_per_subs_is_service_req_allowed_on_plmn(plmn_id, subs_id);

} /* ds3gpp_is_service_req_allowed_on_plmn */

/*===========================================================================
FUNCTION DS3GPP_VALIDATE_EPS_FUNC_INVOKE

DESCRIPTION
  This function is used to validate if eps functions can be invoked


PARAMETERS
  call_id - This is the call_id with which a bearer is associated

DEPENDENCIES
  None.

RETURN VALUE
  boolean - DS3G_SUCCESS or DS3G_FAILURE.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3gpp_validate_eps_func_invoke
(
  cm_call_id_type  call_id 
)
{
  boolean ret_val = DS3G_FAILURE;
  ds_bearer_context_s *bearer_context_p = NULL;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 /*------------------------------------------------------------------------- 
   Get the bearer context from the call id
   Check the bearer context's call mode
   Return TRUE if the call mode is LTE
  -------------------------------------------------------------------------*/
  bearer_context_p = ds_bearer_cntxt_get_bearer_from_call_id(call_id);

  if((bearer_context_p != NULL) &&
     (bearer_context_p->ds_bearer_context_dyn_p->call_mode == SYS_SYS_MODE_LTE))
  {
    DS_3GPP_MSG0_HIGH("Bearer context's call mode is LTE, return SUCCESS");
    ret_val = DS3G_SUCCESS;
  }

  return ret_val;

}


/*===========================================================================
FUNCTION DS3GPP_BEARER_MODIFY_REJ_IND_HDLR

DESCRIPTION
  This function is used to handle the Bearer modification rejection indication.
  The indication is given when NW rejects the bearer modification.

  This function is applicable only to EPS mode of operation.

PARAMETERS
  mode_info_ptr - mode specific info passed in through the CM
  Subs_id       - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds3gpp_bearer_modify_rej_ind_hdlr
(
  cm_call_mode_info_s_type   *mode_info_ptr,
  sys_modem_as_id_e_type      subs_id
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Invoke the flow manager call function to handle bearer modification
    rejection.
  -------------------------------------------------------------------------*/
  ds_flow_manager_nw_bearer_modify_rej(mode_info_ptr, subs_id);

  return;

} /* ds3gpp_bearer_modify_rej_ind_hdlr*/

/*===========================================================================
FUNCTION DS3GPP_BEARER_MODIFY_IND_HDLR

DESCRIPTION
  This function is used to handle the Bearer modification indication.
  The indication can initiated for a NW bearer modification or can be a
  response to UE initiated bearer modification.

  This function is applicable only to EPS mode of operation.

PARAMETERS
  mode_info_ptr - mode specific info passed in through the CM
  Subs_id       - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  boolean - DS3G_SUCCESS or DS3G_FAILURE.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3gpp_bearer_modify_ind_hdlr
(
  cm_call_mode_info_s_type  *mode_info_ptr,
  cm_call_id_type            call_id,
  sys_modem_as_id_e_type     subs_id
)
{
  boolean rval = DS3G_SUCCESS;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(mode_info_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("ds3gpp_bearer_modify_ind_hdlr: mode_info_ptr NULL");
    return DS3G_FAILURE;
  }

  /*---------------------------------------------------------------------- 
    Check if EPS functions can be invoked, if they can update the network
    mode to LTE.
    This is necessary because sometimes in W/G -- L IRAT CNE might delay
    updating the network mode by a few ms.
  ------------------------------------------------------------------------*/
  if(ds3gpp_validate_eps_func_invoke(call_id))
  {
    DS_3GPP_MSG0_MED("Updating the internal 3GPP network mode to LTE");
    ds3gpp_update_network_mode(SYS_SYS_MODE_LTE, subs_id);
  }
  /*-------------------------------------------------------------------------
    Invoke the flow manager call function to handle bearer modification
    rejection.
  -------------------------------------------------------------------------*/
  rval = ds_flow_manager_nw_bearer_modify_ind
           (mode_info_ptr, call_id, subs_id);
  if(rval)
  {
    ds_3gpp_pdn_cntx_update_pco(mode_info_ptr, call_id);
  }
  return rval;
} /* ds3gpp_bearer_modify_ind_hdlr*/

/*===========================================================================
FUNCTION  DS3GPP_IS_PS_DATA_AVAIL_IND_ALLOWED

DESCRIPTION
  This function queries ds3gi_srv_status maintained by ds3gmgr to check if
  we need to flow control the phys link and not send PS DATA AVAILABLE ind
  to CM.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3gpp_is_ps_data_avail_ind_allowed
(
   sys_modem_as_id_e_type    subs_id
)
{
  sys_srv_status_e_type curr_srv_status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  curr_srv_status = ds3g_get_current_srv_status_ex(subs_id);

  if((curr_srv_status == SYS_SRV_STATUS_SRV) || /* Full Service */
     ((ds_3gpp_nv_manager_get_ps_data_avail_on_power_save(subs_id)) &&
      (curr_srv_status == SYS_SRV_STATUS_PWR_SAVE))) /* Power Save */
  {
    DS_3GPP_MSG0_HIGH("ds3gpp_is_ps_data_avail_ind_allowed -Yes- Full Srv/Pwr "
                      "Save(nv based)");
    return TRUE;
  }
  else
  {
    DS_3GPP_MSG0_HIGH("ds3gpp_is_ps_data_avail_ind_allowed -No- Pwr"
                      "Save(nv based)/No/Limited Srv");
    return FALSE;
  }

}/*ds3gpp_is_ps_data_avail_ind_allowed*/

/*===========================================================================
FUNCTION  DS3GPP_DATA_SUSPEND_IND_HDLR

DESCRIPTION
  This function suspends transmission of data to the RAT of the current mode
  for all active PDP contexts during inter-RAT change.

  If data transmission is already suspended and current service request is
  NO_SERVICE or LIMITED_SERVICE, then respective function is called.

  Else if, data transmission is not already suspended, then respective
  function to suspend it is called.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void ds3gpp_data_suspend_ind_hdlr
(
 sys_modem_as_id_e_type        subs_id
)
{
  boolean                    send_ps_data_avail = FALSE;
  ds3gpp_per_subs_info_type* hndl = NULL;
  /*----------------------------------------------------------------------*/

  send_ps_data_avail = ds3gpp_is_ps_data_avail_ind_allowed(subs_id);
  DS_3GPP_MSG3_HIGH("Data Suspend Ind Hdlr Called on sub %d, "
                    "send_ps_data_avail: %d",
                    subs_id,
                    send_ps_data_avail,
                    0);

  hndl = ds3gpp_get_per_subs_info_hndl(subs_id);

  if (!ds3gpp_validate_per_subs_info_hndl(hndl))
  {
    DS_3GPP_MSG1_ERROR("Per Subs Hndl is Invalid for cm subs id: %d",subs_id);
    return;
  }

  /* "Not Suspended State" */
  if(hndl->data_suspend_flag == FALSE)
  {
    /*-------------------------------------------------------------------------
      Suspend data transmission  to the RAT in the current mode.
    -------------------------------------------------------------------------*/
    hndl->data_suspend_flag = TRUE;
    hndl->sent_ps_data_available = FALSE;

    /*-------------------------------------------------------------------------
      Call the mode specific suspend handler
    -------------------------------------------------------------------------*/
    ds_bearer_cntxt_suspend_ind(send_ps_data_avail, subs_id);
  }
  /* "Suspended State/PS Data Avail Not Sent" */
  else if (hndl->sent_ps_data_available == FALSE)
  {

    /*
      Enable phys link flow for all phys links if send_ps_data_avail == TRUE
      otherwise disable flow

    */
    DS_3GPP_MSG1_MED("Data already suspended,Updating phys_link_flow %d",
                      send_ps_data_avail);

    ds_bearer_cntxt_update_phys_link_flow_status
      (send_ps_data_avail, subs_id);
  }
} /* ds3gpp_data_suspend_ind_hdlr() */


/*===========================================================================
FUNCTION  DS3GPP_DATA_RESUME_IND_HDLR

DESCRIPTION
  This function resumes transmission of data to the RAT of the new mode
  for all active PDP-IP contexts and any active PDP-PPP context in
  WCDMA mode. PDP type PPP contexts are dropped after a RAT change to
  GPRS since PDP type PPP calls are not currently supported in GPRS mode.
  If data transmission is already resumed, nothing is done.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void ds3gpp_data_resume_ind_hdlr
(
  sys_modem_as_id_e_type  subs_id
)
{
  sys_sys_mode_e_type              network_mode;
                                   /* System camped on. UMTS/GPRS*/
  ds3gpp_per_subs_info_type*       hndl = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_3GPP_MSG1_HIGH("Data Resume Ind Hdlr Called on sub %d",
                    subs_id);

  hndl = ds3gpp_get_per_subs_info_hndl(subs_id);

  if (!ds3gpp_validate_per_subs_info_hndl(hndl))
  {
    return;
  }

  /*-------------------------------------------------------------------------
    If suspend state = FALSE, no need to do anything. Just return.
  -------------------------------------------------------------------------*/
  if (hndl->data_suspend_flag == FALSE)
  {
    DS_3GPP_MSG0_HIGH("Data Resume Ind Hdlr Called when data_suspend is false");
    return;
  }

  hndl->data_suspend_flag = FALSE;
  hndl->sent_ps_data_available = FALSE;

  /*-------------------------------------------------------------------------
    Get the current network mode.
  -------------------------------------------------------------------------*/
  network_mode = ds3gpp_get_current_network_mode(subs_id);

  /*-------------------------------------------------------------------------
    Call the mode specific resume handler
  -------------------------------------------------------------------------*/
  if( DS_3GPP_IS_3GPP_CURRENT_MODE(network_mode))
  {
    ds_bearer_cntxt_resume_ind(subs_id);
    ds_3gpp_pdn_cntx_resume_ind_hdlr(subs_id);
  }
  else
  {
    DS_3GPP_MSG1_ERROR("Resume Ind: invalid network_mode:%d",network_mode);
  }

  /*-------------------------------------------------------------------------
     Generate technology changed event to PS just once for this iface
   -------------------------------------------------------------------------*/

} /* ds3gpp_data_resume_ind_hdlr() */

/*===========================================================================
FUNCTION  DS3GPP_COEX_DATA_FLOW_DISABLE_HDLR

DESCRIPTION
  This function disable PS flows for a subscription with specific mask.

PARAMETERS
  subs_id: subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds3gpp_coex_data_flow_disable_hdlr
(
  sys_modem_as_id_e_type  subs_id
)
{
  uint32                bearer_cntx_inst = 0;
  ds_bearer_context_s  *bearer_cntx_p = NULL;
  ds_pdn_context_s     *pdn_cntxt_p = NULL;
  sys_sys_mode_e_type   current_nw_mode = SYS_SYS_MODE_NONE; 
  ds_3gpp_iface_s      *ds3gpp_iface_v4_p = NULL;
#ifdef FEATURE_DATA_PS_IPV6
  ds_3gpp_iface_s      *ds3gpp_iface_v6_p = NULL;
#endif /* FEATURE_DATA_PS_IPV6 */
  boolean               disable_flow = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Send PS phys link disable for all bearers for the corresponding subs id
  -----------------------------------------------------------------------*/
  for( bearer_cntx_inst = 0;
       bearer_cntx_inst < DS_3GPP_MAX_BEARER_CONTEXT;
       bearer_cntx_inst++ )
  {
    disable_flow = TRUE;
    bearer_cntx_p = ds_bearer_cntxt_get_instance_ptr(bearer_cntx_inst);

    /*-------------------------------------------------------------------
     Logic for disable flow is as follows:
       - By default, bearer will be torn down
       - If bearer subs_id does not match the passed subs_id,
          then don't disable flow
       - If sys mode is LTE and pdn is the emergency PDN,
          then don't disable flow
       - If sys mode is LTE and iface has IMS apn type,
          then don't disable flow
    -------------------------------------------------------------------*/
    
    if( !ds_bearer_cntx_validate_bearer_subscription_internal(
                                                    bearer_cntx_p, subs_id) )
    {
      disable_flow = FALSE;
    }
    else
    {
      pdn_cntxt_p =
        (ds_pdn_context_s*)ds_bearer_cntxt_get_pdn_cntxt(bearer_cntx_p);

      if(ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntxt_p))
      {
        ds3gpp_iface_v4_p =
          (pdn_cntxt_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
           [DS_3GPP_PDN_CONTEXT_IPV4_IFACE]);

  #ifdef FEATURE_DATA_PS_IPV6
        ds3gpp_iface_v6_p =
          (pdn_cntxt_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
           [DS_3GPP_PDN_CONTEXT_IPV6_IFACE]);
  #endif /* FEATURE_DATA_PS_IPV6 */

        /* Do not flow control emergency call */
        if (DS_3GPP_PDN_SUPPORTS_EMERGENCY_SERVICES(pdn_cntxt_p) == TRUE)
        {
          DS_3GPP_MSG0_MED("PDN is emergency. Do not flow control.");
          disable_flow = FALSE;
        }

        current_nw_mode =
          ds3gpp_get_current_network_mode(
             pdn_cntxt_p->ds_pdn_context_dyn_p->subs_id);

        if (current_nw_mode == SYS_SYS_MODE_LTE)
        {
          if (ds3gpp_iface_v4_p != NULL && 
              PS_IFACE_GET_APN_TYPE(&(ds3gpp_iface_v4_p->ps_iface))
                 == PS_IFACE_APN_TYPE_IMS)
          {
            DS_3GPP_MSG1_MED("Do not Tear down v4 apn type: %d",
                         PS_IFACE_GET_APN_TYPE(&(ds3gpp_iface_v4_p->ps_iface)));
            disable_flow = FALSE;
          }

    #ifdef FEATURE_DATA_PS_IPV6
          if (ds3gpp_iface_v6_p != NULL && 
              PS_IFACE_GET_APN_TYPE(&(ds3gpp_iface_v6_p->ps_iface))
                 == PS_IFACE_APN_TYPE_IMS)
          {
            DS_3GPP_MSG1_MED("Do not Tear down v6 apn type: %d,",
                         PS_IFACE_GET_APN_TYPE(&(ds3gpp_iface_v6_p->ps_iface)));
            disable_flow = FALSE;
          }
    #endif /* FEATURE_DATA_PS_IPV6 */

          /*-------------------------------------------------------------------
           For 3rd party IMS, the APN Type will not be set in the policy.
           Instead, the APN type will be set from the UI for a particular
           APN. In this case, MH needs to match the APN for the PDN with
           the IMS APN in the device settings. If they match, then it is
           a 3rd part IMS PDN and MH should not flow control.
          -------------------------------------------------------------------*/

          if (ds_3gpp_pdn_cntxt_is_pdn_third_party_ims(pdn_cntxt_p,subs_id) 
              == TRUE)
          {
            DS_3GPP_MSG0_MED("PDN APN matches with IMS PDN APN in device "
                             "config. Do not flow control");
            disable_flow = FALSE;
          }
        }
      }
    }

    if( disable_flow )
    {
      ps_phys_link_disable_flow(&(bearer_cntx_p->phys_link),
                                DS_FLOW_COEX_MASK);
    }
  }
  return;
} /* ds3gpp_coex_data_flow_disable_hdlr() */

/*===========================================================================
FUNCTION  DS3GPP_COEX_DATA_FLOW_ENABLE_HDLR

DESCRIPTION
  This function enable PS flows for a subscription with COEX mask.

PARAMETERS
  subs_id: subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds3gpp_coex_data_flow_enable_hdlr
(
  sys_modem_as_id_e_type  subs_id
)
{
  uint32                bearer_cntx_inst = 0;
  ds_bearer_context_s  *bearer_cntx_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Send PS phys link enable for all bearers for the corresponding subs id
  -----------------------------------------------------------------------*/
  for( bearer_cntx_inst = 0;
       bearer_cntx_inst < DS_3GPP_MAX_BEARER_CONTEXT;
       bearer_cntx_inst++ )
  {
    bearer_cntx_p = ds_bearer_cntxt_get_instance_ptr(bearer_cntx_inst);

    if( ds_bearer_cntx_validate_bearer_subscription_internal(bearer_cntx_p, subs_id) )
    {
      ps_phys_link_enable_flow(&(bearer_cntx_p->phys_link), DS_FLOW_COEX_MASK);
    }
  }
  return;
} /* ds3gpp_coex_data_flow_enable_hdlr() */

/*===========================================================================
FUNCTION  DS3GPP_GET_TOTAL_ULDL_WM_CNT_HDLR

DESCRIPTION
  This function returns total UL & DL WM count for all active bearers

PARAMETERS
  subs_id: subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uint64 ds3gpp_get_total_uldl_wm_cnt_hdlr
(
  sys_modem_as_id_e_type  subs_id
)
{
  uint32                bearer_cntx_inst = 0;
  ds_bearer_context_s  *bearer_cntx_p = NULL;
  uint32                total_uldl_wm_cnt = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    
  -----------------------------------------------------------------------*/
  for( bearer_cntx_inst = 0;
       bearer_cntx_inst < DS_3GPP_MAX_BEARER_CONTEXT;
       bearer_cntx_inst++ )
  {
    bearer_cntx_p = ds_bearer_cntxt_get_instance_ptr(bearer_cntx_inst);

    if( ds_bearer_cntxt_validate_bearer_subscription(bearer_cntx_p, subs_id) )
    {
      total_uldl_wm_cnt += bearer_cntx_p->lte_rx_wm.wm_item.total_rcvd_cnt;
      total_uldl_wm_cnt += bearer_cntx_p->rx_wm.wm_item.total_rcvd_cnt;
      total_uldl_wm_cnt += bearer_cntx_p->tx_wm.wm_item.total_rcvd_cnt;
    }
  }
  return total_uldl_wm_cnt;
} /* ds3gpp_get_total_uldl_wm_cnt_hdlr() */

/*===========================================================================
FUNCTION  DS_3GPP_REPORT_THROUGHPUT_INFO_HDLR

DESCRIPTION
   This function sends a report containing the throughput indication
   for all active ifaces on the current subscription id
 
PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_report_throughput_info_hdlr
(
  ds3gsubsmgr_subs_id_e_type      ds3g_subs_id
)
{
  sys_sys_mode_e_type               current_mode        = SYS_SYS_MODE_NONE;
  uint8                             num_active_ifaces   = 0;
  int16                             dss_errno           = 0;
  sys_modem_as_id_e_type            cm_subs_id          = SYS_MODEM_AS_ID_NONE;
  uint16                            num_bytes_reqd      = 0;
  ps_sys_throughput_info_list_type *report_p            = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*------------------------------------------------------------------------- 
    Validate Incoming subs ID
    -------------------------------------------------------------------------*/
  cm_subs_id = ds3gsubsmgr_subs_id_ds3g_to_cm(ds3g_subs_id);
  
  if (!ds3gsubsmgr_is_subs_id_valid(cm_subs_id))
  {
    return;
  }
  /*------------------------------------------------------------------------- 
    Is current mode a 3GPP mode?
    -------------------------------------------------------------------------*/  
  current_mode = ds3g_get_current_network_mode_ex(cm_subs_id);

  if(!DS_3GPP_IS_3GPP_CURRENT_MODE(current_mode))
  {
    DS_3GPP_MSG1_ERROR("Current mode %d not a valid 3GPP mode."
                       " Not generating throughput report", current_mode);
    return;
  }
  /*------------------------------------------------------------------------- 
    Return immediately if there are no active ds 3GPP ifaces being used
    -------------------------------------------------------------------------*/
  num_active_ifaces =  ds3gpp_get_num_active_ds_ifaces_on_subs_id(cm_subs_id);

  if(num_active_ifaces == 0)
  {
    DS_3GPP_MSG1_LOW("No active DS 3GPP ifaces to report throughput stats "
                       "on DS3G subs %d",ds3g_subs_id);
    return;
  }
  /*------------------------------------------------------------------------- 
    Estimate the total memory to cache the report
    -------------------------------------------------------------------------*/
  num_bytes_reqd    = (sizeof(uint8) +
                      (num_active_ifaces * sizeof(ps_sys_throughput_info_type)));

  if(num_bytes_reqd ==0)
  {
    DS_3GPP_MSG1_HIGH("%d Bytes reqd to report t'put Ind", num_bytes_reqd);
    return;
  }
  report_p = (ps_sys_throughput_info_list_type*) 
                        modem_mem_alloc(num_bytes_reqd,MODEM_MEM_CLIENT_DATA);
  /*------------------------------------------------------------------------- 
    Allocate and Initialize memory before filling in the report
    -------------------------------------------------------------------------*/
  if(report_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("Mem Alloc to report t'put Ind failed");
    return;
  }
  memset(report_p, 0, num_bytes_reqd);

  /*------------------------------------------------------------------------- 
    Current SYS Mode is a 3GPP mode. Atleast one DS iface is active/in use
    Report throughput Information per DS iface for all active ifaces
    -------------------------------------------------------------------------*/
  if(FALSE == ds_3gpp_report_all_iface_throughput_info(cm_subs_id, report_p,
                                                       num_active_ifaces))
  {
    DS_3GPP_MSG1_HIGH("reporting Throughput Info for CM subs ID failed:%d", 
                      cm_subs_id);
    return;
  }
  /*------------------------------------------------------------------------- 
   Evaluate if the indication can be sent
    -------------------------------------------------------------------------*/
  if(report_p->num_trpt_info == 0)
  {
    DS_3GPP_MSG0_HIGH("Info List generated is empty");

  }
  else
  {
    /*----------------------------------------------------------------------- 
      Send Indication to PS
      -----------------------------------------------------------------------*/
    if (0 > ps_sys_conf_set_ex(PS_SYS_TECH_ALL,
                               PS_SYS_CONF_THROUGHPUT_INFO,
    (ps_sys_subscription_enum_type)ds3gsubsmgr_subs_id_ds3g_to_ds(ds3g_subs_id),
                               report_p,
                              &dss_errno))
    {
      DS_3GPP_MSG0_HIGH("Sending Throughput Info Ind failed");
    }
  }

  /*------------------------------------------------------------------------- 
    Release the memory that was used to report Throughput Indication
    -------------------------------------------------------------------------*/
  if(report_p != NULL)
  {
    modem_mem_free(report_p, MODEM_MEM_CLIENT_DATA);
  }
  return;
}

/*===========================================================================
FUNCTION DSUMTSPS_PS_DATA_FAIL_IND_HDLR()

DESCRIPTION
  This function is called by 3G DSManager when service acquistion fails while
  PS is in dormant mode.
  Flow is enabled on all active PDP contexts to allow for new data to come in.
  When new data arrives on the watermark, ds3g_msh_ps_data_available_ind()
  is again sent to CM, to try and acquire service again.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void ds3gpp_ps_data_fail_ind_hdlr
(
  sys_modem_as_id_e_type    subs_id
)
{
  ds3gpp_per_subs_info_type* hndl = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_3GPP_MSG0_HIGH(" In ds3gpp_ps_data_fail_ind()");

  hndl = ds3gpp_get_per_subs_info_hndl(subs_id);

  if (!ds3gpp_validate_per_subs_info_hndl(hndl))
  {
    return;
  }

  /*-------------------------------------------------------------------------
    check whether data transmission to the RAT is suspended.
  -------------------------------------------------------------------------*/
  if (hndl->data_suspend_flag == FALSE)
  {
    DS_3GPP_MSG0_ERROR("PS Data Fail Ind received when not in suspended state, ignoring");
    return;
  }

  /*-------------------------------------------------------------------------
    Ignore if this is unsolicited response.
  -------------------------------------------------------------------------*/
  if ( hndl->sent_ps_data_available == FALSE)
  {
    DS_3GPP_MSG0_ERROR("Unsolicited PS Data Fail Ind, ignoring");
    return;
  }

  /*-----------------------------------------------------------------------
    Clear the data available flag, so that when new data arrives on the
    watermark, this can be set and ds3g_msh_ps_data_available_ind()
    can be sent to CM.
  -----------------------------------------------------------------------*/
  hndl->sent_ps_data_available = FALSE;

  /*-----------------------------------------------------------------------
    For all active bearer contexts,
    1. disable flow, if we have service
    2. post phys_link_down_ind
  -----------------------------------------------------------------------*/
  ds_bearer_cntxt_ps_data_fail_ind_hdlr(FALSE, subs_id);

} /* ds3gpp_ps_data_fail_ind_hdlr()*/

/*===========================================================================

FUNCTION DS_3GPP_REFRESH_ACTUAL_THROUGHPUT_HDLR

DESCRIPTION
  When invoked this handler updates the bearer information for all the
  active bearers

DEPENDENCIES
   None

RETURN VALUE
  None
SIDE EFFECTS
   None

===========================================================================*/
void ds_3gpp_refresh_actual_throughput_hdlr
( 
  ds3gsubsmgr_subs_id_e_type   ds3gsubs_id,
  uint16                       timer_expiry_count
)
{
  sys_modem_as_id_e_type              cm_subs_id        = SYS_MODEM_AS_ID_NONE;
  sys_sys_mode_e_type                 current_mode      = SYS_SYS_MODE_NONE;
  uint32                              duration          = 0;
  uint32                              sample_interval   = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*------------------------------------------------------------------------- 
    Validate Incoming subs ID
    -------------------------------------------------------------------------*/
  cm_subs_id = ds3gsubsmgr_subs_id_ds3g_to_cm(ds3gsubs_id);

  if (!ds3gsubsmgr_is_subs_id_valid(cm_subs_id))
  {
    DS_3GPP_MSG1_HIGH("act t'put: Invalid DS3G subs ID %d to"
                                     "report tput",cm_subs_id);
    return;
  }

  /*------------------------------------------------------------------------- 
    Get the periodic refresh interval
    -------------------------------------------------------------------------*/
  duration     = ds3gi_get_actual_throughput_refresh_interval(ds3gsubs_id);

  if (duration == 0)
  {
    DS_3GPP_MSG1_HIGH("act t'put: DS3G subs id %d timer "
                                      "exp interval 0.",ds3gsubs_id);
    return;
  }
  /*------------------------------------------------------------------------- 
    calculate sampling interval .Skip refreshing if the value is 0
    -------------------------------------------------------------------------*/
  sample_interval = (duration * timer_expiry_count);

  DS_3GPP_MSG1_MED("act t'put: periodicity %d msec",
                  sample_interval);

  /*--------------------------------------------------------------------------- 
    Proceed further only if the current network mode is a 3GPP mode for the
    current subscription
    ---------------------------------------------------------------------------*/
  current_mode = ds3g_get_current_network_mode_ex(cm_subs_id);

  if(!DS_3GPP_IS_3GPP_CURRENT_MODE(current_mode))
  {
    DS_3GPP_MSG2_MED("Not a valid sys mode %d on Subs id(CM) %d",current_mode,
                       cm_subs_id);
    return;
  }

  /*------------------------------------------------------------------------- 
    Refresh actual throughput rates for all active bearer contexts
    -------------------------------------------------------------------------*/
  if(!ds_3gpp_bearer_context_refresh_actual_throughput(cm_subs_id,
                                                       sample_interval))
  {
    DS_3GPP_MSG1_MED("No active bearers on Subs id(CM) %d to refresh Act tput"
                      "values ", cm_subs_id);
  }

  return;
}


#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
/*===========================================================================
FUNCTION  DSUMTSPS_CALL_CONNECTED_HDLR

DESCRIPTION
   This function is called by 3G DSManager when the call is connected ( PDP
   context) established. This function sets up the protocol stacks based on
   the PDP type, configures them and kick starts the protocol stack.

PARAMETERS   :
  call_id,        - Call ID
  *mode_info_ptr  - Pointer to the mode-specific call information provided by
                    Call Manager
  *delay_connect  - pointer to flag indicating whether to send a delayed
                    connect
  *end_params     - CM specific call end parameters
  Subs_id         - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL boolean dsumts_call_connected_hdlr
(
  cm_call_id_type                 call_id,
  cm_call_mode_info_s_type       *mode_info_ptr_i,
  boolean                        *delay_connect,
  cm_end_params_s_type           *end_params,
  sys_modem_as_id_e_type          subs_id
  /*lint -esym(818, mode_info_ptr_i) */
)
{
  const cm_call_mode_info_s_type *mode_info_p = mode_info_ptr_i;
  ds_pdn_context_s               *pdn_cntx_p = NULL;
  ds_bearer_context_s            *bearer_cntx_p = NULL;
  sys_sys_mode_e_type            network_mode;
  boolean                        result = DS3G_SUCCESS;
#ifdef FEATURE_QXDM_WM_TRACING
  uint32 callid;
#endif /* FEATURE_QXDM_WM_TRACING */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /*-------------------------------------------------------------------------
    Fill in the call end parameters : DS manager uses passes this info to cm
    for a call release if we return an error from here
  -------------------------------------------------------------------------*/
  end_params->call_id   = call_id;
  end_params->info_type = CM_CALL_MODE_INFO_GW_PS;

  /*----------------------------------------------------------------------
    Call should be connected immediately.
  ----------------------------------------------------------------------*/
  *delay_connect = FALSE;

  /*-------------------------------------------------------------------------
    We should not get the Call Connected event from Call Manager while
    we are suspended. Return DS3G_FAILURE if this happens.
  -------------------------------------------------------------------------*/

  if (ds3gpp_get_ps_data_suspend_flag(subs_id) == TRUE)
  {
    DS_3GPP_MSG1_ERROR("Suspended:Invalid Call Conn Cmd for call_id:%d",call_id);
    return DS3G_FAILURE;
  }

  if ( mode_info_p == NULL )
  {
    DATA_ERR_FATAL(" mode_info_p = NULL in call_connected_hdlr");
    return DS3G_FAILURE;
  }

  /*-------------------------------------------------------------------------
     Get the bearer cntx for the call ID.
  -------------------------------------------------------------------------*/
  bearer_cntx_p = ds_bearer_cntxt_get_bearer_from_call_id(call_id);

  if(!ds_bearer_cntx_validate_bearer_context(bearer_cntx_p))
  {
    return DS3G_FAILURE;
  }

  /*------------------------------------------------------------------------ 
    If the bearer is in PENDING_REDIAL state. Return DS3g_FAILURE
    ------------------------------------------------------------------------*/
  if(bearer_cntx_p->ds_bearer_context_dyn_p->state == 
     DS_BEARER_CONTEXT_STATE_PEND_REDIAL)
  {
    DS_3GPP_MSG0_HIGH("Bearer in pending redial state! Ignore");
    return DS3G_FAILURE;
  }

  /*-------------------------------------------------------------------------
    Get the pdn cntx associated with the bearer cntx
  -------------------------------------------------------------------------*/
  pdn_cntx_p = (ds_pdn_context_s*)ds_bearer_cntxt_get_pdn_cntxt(bearer_cntx_p);

  if (!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return DS3G_FAILURE;
  }

  /*-------------------------------------------------------------------------
    Get the current network mode.
  -------------------------------------------------------------------------*/
  network_mode = ds3gpp_get_current_network_mode(subs_id);

  /*-------------------------------------------------------------------------
    Configure the IFACE with mode_info_ptr configuration
  -------------------------------------------------------------------------*/
  if( (network_mode == SYS_SYS_MODE_WCDMA) ||
      (network_mode == SYS_SYS_MODE_GSM)
#ifdef FEATURE_TDSCDMA
      ||
      (network_mode == SYS_SYS_MODE_TDS)
#endif
    )
  {

    if(bearer_cntx_p->ds_bearer_context_dyn_p->is_default)
    {
      ps_phys_link_type *phys_link = &(bearer_cntx_p->phys_link);
      if(PS_PHYS_LINK_GET_STATE(phys_link) != PHYS_LINK_RESUMING)
      {
      if(ds_pdn_cntxt_activate_ind
         (
           pdn_cntx_p,
           mode_info_p,
           end_params,
           subs_id
         ) != DS3G_SUCCESS)
      {
        return DS3G_FAILURE;
      }
    }
    }
    else
    {
      /*-------------------------------------------------------------------------
        Invoke PDN bearerActivationInd Callback
      -------------------------------------------------------------------------*/
      if(ds_pdn_cntxt_bearer_activate_ind
         (call_id, mode_info_ptr_i,NULL, subs_id) != DS3G_SUCCESS)
      {
        return DS3G_FAILURE;
      }
    }


    if (ds_bearer_cntxt_activate_cnf
        (
          bearer_cntx_p,
          mode_info_p,
          end_params,
          subs_id
        ) != DS3G_SUCCESS)
    {
      return DS3G_FAILURE;
    }
  }
  else
  {
    DS_3GPP_MSG3_ERROR("pdn cntx:%x Invalid network_mode:%d",
              pdn_cntx_p,network_mode,0);
    return DS3G_FAILURE;
  }

#ifdef FEATURE_QXDM_WM_TRACING
  callid = ds_bearer_cntxt_get_call_id(bearer_cntx_p);
  DS_3GPP_MSG1_MED("MH Client: CALL_ID values (%d)",callid);

  /*-------------------------------------------------------------------------
    Register with FC for watermark tracing
  -------------------------------------------------------------------------*/
  switch (network_mode)
  {
    case SYS_SYS_MODE_WCDMA:
      DS_3GPP_MSG1_MED("MH Client: Registering with FC for UMTS_TECH, call id %d",
              callid);
      fc_post_reg_cmd_wm_trace(UMTS_TECH, //tech id
                               callid,
                               UNIFIED_3GPP_MODE_HANDLER_CLIENT,
                               ds3gpp_report_wm_stats_cb,
                               (uint32)bearer_cntx_p->ds_bearer_context_dyn_p
                               ->client_data_ptr);
      break;

    case SYS_SYS_MODE_GSM:
      DS_3GPP_MSG1_MED("MH: Registering with FC for GSM_TECH, call id %d",
              callid);
      fc_post_reg_cmd_wm_trace(GSM_TECH, //tech id
                               callid,
                               UNIFIED_3GPP_MODE_HANDLER_CLIENT,
                               ds3gpp_report_wm_stats_cb,
                               (uint32)bearer_cntx_p->ds_bearer_context_dyn_p
                               ->client_data_ptr);
      break;

    default:
      DS_3GPP_MSG2_MED("MH: Not registering for UNKNOWN tech, call id %d mode %d",
              callid, network_mode);
      break;
  }
#endif /* FEATURE_QXDM_WM_TRACING */
  return ( result );

} /* dsumtsps_call_connected_hdlr() */
  /*lint +esym(818, mode_info_ptr_i) */

/*===========================================================================

FUNCTION DSUMTS_INCOMING_CALL_HANDLER

DESCRIPTION

    This handler is invoked when the CM_CALL_EVENT_INCOM event is rx'ed from
    Call Mgr.
    The return value indicates the action to be taken: answer, notify user or
    reject call. The serial mode is changed only if the return value
    indicates answer.

DEPENDENCIES
   None

RETURN VALUE
   ds3g_incom_e_type value

SIDE EFFECTS
   None

===========================================================================*/
/*ARGSUSED*/
LOCAL ds3g_incom_e_type dsumts_incoming_call_handler
(
  ds_cmd_type            *cmd_ptr_i,
  cm_call_id_type         call_id,
  cm_ans_params_s_type   *ans_params,
  cm_end_params_s_type   *end_params,
  sys_modem_as_id_e_type  subs_id
)
{
  const cm_gw_ps_call_info_s_type  *ps_call_info = NULL;
  ds_bearer_context_s              *bearer_cntx_p;
  ds_bearer_context_s              *def_bearer_cntx_p;
  ds_pdn_context_s                 *pdn_cntx_p;
  ds3g_call_info_cmd_type          *call_info_cmd_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd_ptr_i == NULL) || (cmd_ptr_i->cmd_payload_ptr == NULL) )
  {
    DS_3GPP_MSG0_ERROR( "NULL pointer passed" );
    return DS3G_IGNORE;
  }

  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr_i->cmd_payload_ptr;

  end_params->call_id = call_id;
  end_params->info_type = CM_CALL_MODE_INFO_GW_PS;
  end_params->end_params.gw_ps_end.cause =SM_ACTIVATION_REJECTED_UNSPECIFIED;

  ps_call_info = &(call_info_cmd_ptr->mode_info.info.gw_ps_call);

  DS_3GPP_MSG2_HIGH("Incoming call handler, call id %d, pdp group id %d",
                    call_id, ps_call_info->pdp_group_id);
  /*-------------------------------------------------------------------------
     Get the bearer cntx for the call ID.
  -------------------------------------------------------------------------*/
  bearer_cntx_p = ds_bearer_cntxt_get_bearer_from_call_id(call_id);

  /*-------------------------------------------------------------------------
     Verify it is a new call ID.
  -------------------------------------------------------------------------*/
  if (bearer_cntx_p != NULL)
  {
    DS_3GPP_MSG1_ERROR("INCOM_CALL_HDLR: call with call id[%d] already present",
                       call_id);
    end_params->end_params.gw_ps_end.cause = SM_INSUFFICIENT_RESOURCES;
    return DS3G_REJECT_CALL;
  }
  if (ps_call_info->is_primary_pdp )
  {
    return dsUmtsPdnCntx_incomingCall(
                                  call_id,
                                  cmd_ptr_i,
                                  ans_params,
                                  end_params,
                                  subs_id
                                  );
  }
  else
  {

    /*---------------------------------------------------------------------
        Retrieve the PDN from pdp group id.
    -----------------------------------------------------------------------*/
    pdn_cntx_p = ds_pdn_cntxt_get_pdn_from_pdp_group_id(ps_call_info->pdp_group_id);
      
    if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
    {
      DS_3GPP_MSG2_ERROR("pdn context does not exist: group_id=%d call_id=%d",
                          ps_call_info->pdp_group_id,
                          call_id);
      end_params->end_params.gw_ps_end.cause = SM_UNKNOWN_PDP_CONTEXT;
      return DS3G_REJECT_CALL;
    }

    /*-------------------------------------------------------------------------
      Ensure that the PDN state is correct. If the PDN state is in GOING_DOWN,
      then we have to reject the bearer activation.
      As a result of rejecting bearer activation, we will get a CALL_END from
      CM during which we clean up our resources.
    -------------------------------------------------------------------------*/
    if(pdn_cntx_p->ds_pdn_context_dyn_p->state == DS_PDN_CONTEXT_STATE_GOING_DOWN)
    {
      DS_3GPP_MSG0_HIGH("Got incoming call while PDN going down - Rejecting");
      return DS3G_REJECT_CALL;
    }
    /*---------------------------------------------------------------------
      Verify if the default bearer exists.
    -----------------------------------------------------------------------*/
    def_bearer_cntx_p = (ds_bearer_context_s*) pdn_cntx_p->ds_pdn_context_dyn_p
      ->def_bearer_context_ptr;

    if(!ds_bearer_cntx_validate_bearer_context(def_bearer_cntx_p))
    {
      end_params->end_params.gw_ps_end.cause = SM_UNKNOWN_PDP_CONTEXT;
      return DS3G_REJECT_CALL;
    }

    /*-----------------------------------------------------------------------
      Get a bearer by calling the alloc function.
    -----------------------------------------------------------------------*/
    bearer_cntx_p = ds_bearer_cntxt_allocate_new_bearer(subs_id);

    if(!ds_bearer_cntx_validate_bearer_context(bearer_cntx_p))
    {
      end_params->end_params.gw_ps_end.cause = SM_INSUFFICIENT_RESOURCES;
      return DS3G_REJECT_CALL;
    }
    /*----------------------------------------------------------------------
      Update the bearer information
    ---------------------------------------------------------------------*/
    if(ds_pdn_cntxt_store_bearer_context(pdn_cntx_p,(void *)bearer_cntx_p) != \
                                                DS3G_SUCCESS)
    {
      DS_3GPP_MSG0_ERROR("Could not store the allocated bearer");
      ds_bearer_cntxt_return_bearer_to_free_pool(bearer_cntx_p);
      return DS3G_REJECT_CALL;
    }
    if(!dsUmtsBearerCntxt_validatebearer(
         &umts_bearer_context_tbl[(uint32)bearer_cntx_p->
         ds_bearer_context_dyn_p->client_data_ptr]))
    {
      DS_3GPP_MSG0_ERROR("Umts Bearer context passed is Invalid");
      return DS3G_REJECT_CALL;
    }

    /*------------------------------------------------------------------------- 
      Associate the mode specific blob that is going to be populated to the 
      bearer context pointer.
    -------------------------------------------------------------------------*/
    bearer_cntx_p->ds_bearer_context_dyn_p->vobj_data_ptr_tbl[DS_3GPP_UMTS_VOBJ] =\
      (void*)&umts_bearer_context_tbl
              [(uint32)(bearer_cntx_p->ds_bearer_context_dyn_p->client_data_ptr)];

    /*----------------------------------------------------------------------
      Update the bearer information with incoming PCO for secondary.
      To Do: Test for primary also(MT primary call)?check spec for primary.
    ---------------------------------------------------------------------*/
    ((ds_umts_bearer_context_s*)(bearer_cntx_p->ds_bearer_context_dyn_p->
                                 vobj_data_ptr_tbl[DS_3GPP_UMTS_VOBJ]))->
                                 ds_umts_bearer_context_dyn_p->pco.valid  = FALSE;
    if(ps_call_info->config_options.valid)
    {
      DS_3GPP_MSG0_HIGH("PCO present in secondary activation request");      
      memscpy((void*)&((ds_umts_bearer_context_s*)(bearer_cntx_p->
      ds_bearer_context_dyn_p->vobj_data_ptr_tbl[DS_3GPP_UMTS_VOBJ]))->
      ds_umts_bearer_context_dyn_p->pco, 
      sizeof(protocol_cfg_options_T),
      (void*)&(ps_call_info->config_options), sizeof(protocol_cfg_options_T) );
    }
    /*-----------------------------------------------------------------------
      Update PDN cntx info in bearer cntx. Set is_default to false
      since this is a dediacted bearer for that PDN context.
    -----------------------------------------------------------------------*/
    ds_bearer_cntxt_register_pdn_context(pdn_cntx_p,
                                         bearer_cntx_p,
                                         FALSE);

    bearer_cntx_p->ds_bearer_context_dyn_p->cm_call_id = call_id;

    /* call flow context incoming call hdlr */
    if (ds_flow_manager_nw_bearer_activation_req(pdn_cntx_p,
                                                 bearer_cntx_p,
                                               &(call_info_cmd_ptr->mode_info),
                                                 end_params) !=
                                           DS3G_SUCCESS)
    {
      DS_3GPP_MSG0_ERROR("Flow manager bearer act req failed");
      ds_bearer_cntxt_free_bearer_inst(bearer_cntx_p);
      if(ds_pdn_cntxt_release_bearer_context(pdn_cntx_p,(void *)bearer_cntx_p) !=
                                                   DS3G_SUCCESS)
      {
        DS_3GPP_MSG0_HIGH("could not release the allocated bearer");
      }
      ds_bearer_cntxt_return_bearer_to_free_pool(bearer_cntx_p);
      return DS3G_REJECT_CALL;
    }

    return DS3G_ALERT_USER;
  }
}  /* dsumts_incoming_call_handler */

/*===========================================================================

FUNCTION DSUMTS_ANSWER_HDLR

DESCRIPTION

   This handler is invoked when a call origination occurs due to a MT
   request. All the parameters needed for call origination are filled
   in the answer_params.

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
LOCAL void dsumts_answer_hdlr
(
  cm_call_id_type        call_id,
  cm_ans_params_s_type   *answer_params,
  ie_cm_cc_cause_s_type  *cause_ptr,
  sys_modem_as_id_e_type  subs_id
)
{
  ds_bearer_context_s          *bearer_cntx_p;
  ds_pdn_context_s             *pdn_cntx_p;
  ds_3gpp_pdn_call_info_type   call_info;

  if ((answer_params == NULL)|| (cause_ptr == NULL))
  {
    DS_3GPP_MSG1_HIGH("Null input parameters, call id: %d",call_id);
    return;
  }

  memset(&call_info, 0, sizeof(call_info));
  /*-------------------------------------------------------------------------
    Initialize the protocol contents index used to be 0. We will fill it
    as required.
  -------------------------------------------------------------------------*/
  answer_params->info_type = CM_CALL_MODE_INFO_GW_PS;
  answer_params->ans_params.gw_ps_ans.config_options.num_protocol_ids = 0;
  cause_ptr->present = FALSE;

  /*-------------------------------------------------------------------------
     Get the bearer cntx for the call ID.
  -------------------------------------------------------------------------*/
  bearer_cntx_p = ds_bearer_cntxt_get_bearer_from_call_id(call_id);

  if(!ds_bearer_cntx_validate_bearer_context(bearer_cntx_p))
  {
    answer_params->ans_params.gw_ps_ans.accept = FALSE;
    return;
  }

  if(bearer_cntx_p->ds_bearer_context_dyn_p->is_default)
  {
    answer_params->ans_params.gw_ps_ans.primary_pdp = TRUE;
  }
  else
  {
    answer_params->ans_params.gw_ps_ans.primary_pdp = FALSE;
  }
  call_info.call_instance = (uint32)(bearer_cntx_p->
                                     ds_bearer_context_dyn_p->client_data_ptr);

  /*-------------------------------------------------------------------------
    Get the pdn cntx associated with the bearer cntx
  -------------------------------------------------------------------------*/
  pdn_cntx_p = (ds_pdn_context_s*)ds_bearer_cntxt_get_pdn_cntxt(bearer_cntx_p);

  if ((NULL == pdn_cntx_p) || 
      !ds_pdn_cntxt_validate_pdn_subscription(pdn_cntx_p, subs_id))
  {
    DS_3GPP_MSG0_ERROR("PDN Subscription Validation failed");
    answer_params->ans_params.gw_ps_ans.accept = FALSE;
    return;
  }

  if(dsUmtsBearerCntxt_getAnswerParams(bearer_cntx_p,
                                       answer_params) != DS3G_SUCCESS)
  {
    answer_params->ans_params.gw_ps_ans.accept = FALSE;
    return;
  }/* if dsUmtsPdnCntx_getAnswerParams != DS3G_SUCCESS */

  if(dsUmtsPdnCntx_getAnswerParams(pdn_cntx_p,
                                   answer_params,
                                   &call_info) != DS3G_SUCCESS)
  {
    answer_params->ans_params.gw_ps_ans.accept = FALSE;
    return;
  }/* if dsUmtsPdnCntx_getAnswerParams != DS3G_SUCCESS */

  answer_params->ans_params.gw_ps_ans.accept = TRUE;
  return;


} /*  dsumts_answer_hdlr */

/*===========================================================================
FUNCTION DS_3GPP_CONTEXT_MODIFY_CNF_HDLR

DESCRIPTION
  This function is used to handle the context modification confirmation.
  The indication is a response to UE initiated pdp context modification.

  This function is applicable only to UMTS mode of operation.

PARAMETERS
  mode_info_ptr - mode specific info passed in through the CM
  subs_id       - Subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_context_modify_cnf_hdlr
(
  cm_call_id_type            call_id,
  cm_call_mode_info_s_type  *mode_info_ptr,
  sys_modem_as_id_e_type     subs_id
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Invoke the flow manager call function to handle bearer modification
    rejection.
  -------------------------------------------------------------------------*/
  (void)ds_flow_manager_nw_context_modify_cnf(call_id, mode_info_ptr, subs_id);
} /* ds_3gpp_context_modify_cnf_hdlr*/

/*===========================================================================
FUNCTION DS_3GPP_CONTEXT_MODIFY_REJ_HDLR

DESCRIPTION
  This function is used to handle the context modification rejection.
  The indication is a response to UE initiated pdp context modification.

  This function is applicable only to UMTS mode of operation.

PARAMETERS
  mode_info_ptr - mode specific info passed in through the CM
  subs_id       - Subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_context_modify_rej_hdlr
(
  cm_call_id_type            call_id,
  cm_call_mode_info_s_type  *mode_info_ptr,
  sys_modem_as_id_e_type     subs_id
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Invoke the flow manager call function to handle bearer modification
    rejection.
  -------------------------------------------------------------------------*/
  (void)ds_flow_manager_nw_context_modify_rej(call_id, mode_info_ptr, subs_id);
} /* ds_3gpp_context_modify_rej_hdlr*/

/*===========================================================================
FUNCTION DS_3GPP_NW_CONTEXT_MODIFY_IND_HDLR

DESCRIPTION
  This function is used to handle the context modification rejection.
  The indication is a response to UE initiated pdp context modification.

  This function is applicable only to UMTS mode of operation.

PARAMETERS
  mode_info_ptr - mode specific info passed in through the CM
  subs_id       - Subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_nw_context_modify_ind_hdlr
(
  cm_call_id_type            call_id,
  cm_call_mode_info_s_type  *mode_info_ptr,
  sys_modem_as_id_e_type     subs_id
)
{
  /*-------------------------------------------------------------------------
    Invoke the flow manager call function to handle nw bearer modify ind.
  -------------------------------------------------------------------------*/
  (void)ds_flow_manager_nw_context_modify_ind(call_id, mode_info_ptr, subs_id);
}

/*===========================================================================
FUNCTION DS_3GPP_NW_CONTEXT_MODIFY_REQ_HDLR

DESCRIPTION
  This function is used to handle the context modification request from NW

  This function is applicable only to UMTS mode of operation.

PARAMETERS
  mode_info_ptr - mode specific info passed in through the CM
  subs_id       - Subscription id

DEPENDENCIES
  None.

RETURN VALUE
  boolean - DS3G_SUCCESS or DS3G_FAILURE.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_nw_context_modify_req_hdlr
(
  cm_call_id_type                 call_id,
  ds_cmd_type                    *cmd_ptr,
  cm_pdp_act_res_params_s_type   *act_res_params_ptr,
  sys_modem_as_id_e_type          subs_id
)
{
  /*-------------------------------------------------------------------------
    Invoke the flow manager call function to handle bearer modification
    rejection.
  -------------------------------------------------------------------------*/
  (void) ds_flow_manager_nw_context_modify_req(call_id, cmd_ptr,
                                               act_res_params_ptr, subs_id);
}

#endif /* FEATURE_DATA_WCDMA_PS || FEATURE_GSM_GPRS */

#if defined(FEATURE_DATA_LTE) && (defined(FEATURE_GSM_GPRS) || defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_TDSCDMA))
/*===========================================================================
FUNCTION DS3GPP_CNTXT_TRANSFER_IND_HDLR

DESCRIPTION  This function processes CM_CALL_CNTXT_TRANSFER_IND from Call
             Manager. This ind signifies that L<->GW inter RAT transition is
             in progress.

PARAMETERS
  sys_mode;          - source RAT
  *mode_info_ptr     - Pointer to CM provided mode specific info, which
                       contains the target RAT


DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void ds3gpp_cntxt_transfer_ind_hdlr
(
  sys_sys_mode_e_type       target_rat,
  sys_modem_as_id_e_type    subs_id
)
{
  sys_sys_mode_e_type mode = SYS_SYS_MODE_NONE;
  /*----------------------------------------------------------------------*/

  mode = ds3gpp_get_current_network_mode(subs_id);

  DS_3GPP_MSG2_HIGH("CNTXT_TRANSFER_IND_HDLR Called ds_3gpp_sys_mode:%d",
                     mode,target_rat);
  
  ds_3gpp_set_cntxt_transfer_info(target_rat,TRUE, subs_id);
  /* ---------------------------------------------------------------------------
     Here we process the Bearer Contxt Transfer Indication. 
     If atleast one bearer context can be transferred successfully to target_rat,
     we proceed with executing PDN Cntxt specific operations for 
     Context Transfer Indication.
  ---------------------------------------------------------------------------*/
  if (ds_bearer_cntxt_transfer_ind(target_rat, subs_id) == TRUE)
  {
    ds_pdn_cntxt_handle_cntxt_transfer_ind(target_rat,subs_id);
  }
}

#endif /* defined(FEATURE_DATA_LTE) && defined(FEATURE_DS_UMTS_REARCH) */

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) || defined (FEATURE_TDSCDMA)
/*===========================================================================
FUNCTION DSUMTS_HDLR_INIT

DESCRIPTION
  This function initializes the 3GPP Packet-Switched Data mode-specific
  handlers. It is invoked during DS Task power-up.
  It creates interfaces for each pdn context and initializes the
  parameters for each pdn context.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  Signal mask containing the REX signals that the handlers want to wait on.

SIDE EFFECTS
  None.

===========================================================================*/
rex_sigs_type dsumts_hdlr_init( void )
{
  rex_sigs_type            requested_sigs = 0; 
                            /* Signal mask to wait on */
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_p = NULL;

  dsUmtsPdnCntx_umtsInit();
  dsUmtsBearerCntxt_init();
  ds_umts_flow_manager_init();
  ds_umts_bearer_flow_manager_init();

#ifdef FEATURE_WCDMA

  hdlr_tbl_p = ds3gpp_get_handler_tbl(SYS_SYS_MODE_WCDMA);

  if (hdlr_tbl_p)
  {
    memset(hdlr_tbl_p,0x0,sizeof(ds3g_hdlr_func_tbl_type));
    /*-------------------------------------------------------------------------
      Initialize the WCDMA handler function table
    -------------------------------------------------------------------------*/
    hdlr_tbl_p->originate_call_handler          = ds3gpp_orig_call_hdlr;
    hdlr_tbl_p->call_id_handler                 = ds3gpp_call_id_hdlr;
    hdlr_tbl_p->user_end_handler                = ds3gpp_user_end_hdlr;
    hdlr_tbl_p->call_ended_handler              = ds3gpp_call_ended_hdlr;
    hdlr_tbl_p->call_connected_handler          = dsumts_call_connected_hdlr;
    hdlr_tbl_p->call_conf_handler               = NULL;
    hdlr_tbl_p->setup_handler                   = NULL;
    hdlr_tbl_p->switch_call_handler             = NULL;
    hdlr_tbl_p->force_call_down_handler         = NULL;
    hdlr_tbl_p->incoming_call_handler           = dsumts_incoming_call_handler;
    hdlr_tbl_p->user_answer_handler             = dsumts_answer_hdlr;
  /*  These handlers are to be defined.
    hdlr_tbl_p->bearer_activate_handler         = NULL;// EPS specific
  */
    /*-------------------------------------------------------------------------
      InterRAT related procedures
    -------------------------------------------------------------------------*/
    hdlr_tbl_p->sys_change_handler        =  ds3gpp_sys_change_handler;
    hdlr_tbl_p->data_suspend_ind_handler  =  ds3gpp_data_suspend_ind_hdlr;
    hdlr_tbl_p->data_resume_ind_handler   =  ds3gpp_data_resume_ind_hdlr;
    hdlr_tbl_p->ps_data_fail_ind_handler  =  ds3gpp_ps_data_fail_ind_hdlr;

  #ifdef FEATURE_DATA_LTE
    hdlr_tbl_p->cntxt_transfer_ind_hdlr   =  ds3gpp_cntxt_transfer_ind_hdlr;
  #endif /* FEATURE_DATA_LTE */

    /*-------------------------------------------------------------------------
      QOS related procedures
    -------------------------------------------------------------------------*/
    hdlr_tbl_p->context_modify_cnf_handler =
                                                ds_3gpp_context_modify_cnf_hdlr;
    hdlr_tbl_p->context_modify_rej_handler =
                                                ds_3gpp_context_modify_rej_hdlr;
  /*  These handlers are to be defined.
    hdlr_tbl_p->context_prom_ind_handler  =  dsumtsps_call_promotion_hdlr;
  */
  #if defined (FEATURE_LTE) && defined (FEATURE_DATA_WCDMA_PS)
    hdlr_tbl_p->context_mod_ind_handler   =
                                                ds_3gpp_nw_context_modify_ind_hdlr;
  #endif /* (FEATURE_LTE) && (FEATURE_DATA_WCDMA_PS) */

    hdlr_tbl_p->nw_modify_req_handler =
                                             ds_3gpp_nw_context_modify_req_hdlr;
    /*-------------------------------------------------------------------------
      RAB related procedures are specific to WCDMA
    -------------------------------------------------------------------------*/
    hdlr_tbl_p->rab_rel_handler                 =
                                            ds_bearer_cntxt_call_rab_release_hdlr;
    hdlr_tbl_p->rab_re_estab_ind_handler        =
                                       ds_bearer_cntxt_call_rab_re_estab_ind_hdlr;
    hdlr_tbl_p->rab_re_estab_rej_handler        =
                                      ds_bearer_cntxt_call_rab_re_estab_rej_hdlr ;
    hdlr_tbl_p->rab_re_estab_fail_handler       =
                                     ds_bearer_cntxt_call_rab_re_estab_fail_hdlr ;

  /*  These handlers are to be defined.
    hdlr_tbl_p->get_call_connection_info_handler =
                                          dswpsd_get_call_connection_info_hdlr;
    hdlr_tbl_p->get_data_counters_handler =  dswpsd_get_data_counters_hdlr;
  */
    hdlr_tbl_p->get_data_counters_handler        =
                                     ds_3gpp_pdn_cntx_get_iface_stats;
    hdlr_tbl_p->isim_refresh_voting_handler      =
                                   dsWcdmaBearerCntxt_refresh_voting_hdlr;
    hdlr_tbl_p->ps_data_generic_ind_handler = 
                                   ds_3gpp_ps_data_generic_ind_hdlr;
    hdlr_tbl_p->bsr_reset_hdlr = NULL;
    hdlr_tbl_p->plmn_change_hdlr = ds_3gpp_plmn_change_hdlr;
    hdlr_tbl_p->report_throughput_info_hdlr = ds_3gpp_report_throughput_info_hdlr;
    hdlr_tbl_p->refresh_actual_throughput_hdlr = ds_3gpp_refresh_actual_throughput_hdlr;
    hdlr_tbl_p->send_downlink_throughput_estimation_indicaiton = NULL;
    hdlr_tbl_p->prepare_downlink_estimated_throughput_report = NULL;
    hdlr_tbl_p->get_downlink_throughput_t_accumulate = NULL;
    hdlr_tbl_p->mode_pref_change_handler = ds_3gpp_mode_pref_change_hdlr;

    hdlr_tbl_p->coex_flow_enable_hdlr  = ds3gpp_coex_data_flow_enable_hdlr;
    hdlr_tbl_p->coex_flow_disable_hdlr = ds3gpp_coex_data_flow_disable_hdlr;

    hdlr_tbl_p->config_modem_power_stats =  ds_wcdma_config_modem_power_stats;
    hdlr_tbl_p->get_modem_power_stats =  ds_wcdma_get_modem_power_stats;
    hdlr_tbl_p->request_modem_power_stats = NULL;

    hdlr_tbl_p->get_total_uldl_wm_cnt_hdlr = ds3gpp_get_total_uldl_wm_cnt_hdlr;
    hdlr_tbl_p->bringdown_phys_link_hdlr = ds_3gpp_hdlr_bring_down_phys_link_hdlr;

    /*-------------------------------------------------------------------------
      Register the mode-specific handlers with 3G DS Manager
    -------------------------------------------------------------------------*/
    ds3g_register_mode_sp_handler( SYS_SYS_MODE_WCDMA,
                                   DS_CALL_TYPE_PKT,
                                   hdlr_tbl_p);

  }

  /*-------------------------------------------------------------------------
    Initialize wcdma specific bearer context
  -------------------------------------------------------------------------*/
  dsWcdmaBearerCntxt_init();
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_GPRS

  hdlr_tbl_p = ds3gpp_get_handler_tbl(SYS_SYS_MODE_GSM);

  if (hdlr_tbl_p)
  {
    memset(hdlr_tbl_p,0x0,sizeof(ds3g_hdlr_func_tbl_type));
    /*-------------------------------------------------------------------------
     Initialize the GPRS handler function table
    -------------------------------------------------------------------------*/
    hdlr_tbl_p->originate_call_handler           = ds3gpp_orig_call_hdlr;
    hdlr_tbl_p->call_id_handler                  = ds3gpp_call_id_hdlr;
    hdlr_tbl_p->user_end_handler                 = ds3gpp_user_end_hdlr;
    hdlr_tbl_p->call_ended_handler               = ds3gpp_call_ended_hdlr;
    hdlr_tbl_p->call_conf_handler                = NULL;
    hdlr_tbl_p->call_connected_handler           = dsumts_call_connected_hdlr;
    hdlr_tbl_p->setup_handler                    = NULL;

    hdlr_tbl_p->incoming_call_handler       = dsumts_incoming_call_handler;
    hdlr_tbl_p->user_answer_handler         = dsumts_answer_hdlr;

    hdlr_tbl_p->switch_call_handler              = NULL;
    hdlr_tbl_p->force_call_down_handler          = NULL;

  /* These handlers are to be defined
    hdlr_tbl_p->get_call_connection_info_handler =
                                          dsgpsd_get_call_connection_info_hdlr;
    hdlr_tbl_p->get_data_counters_handler        =
                                                 dsgpsd_get_data_counters_hdlr;
    hdlr_tbl_p->bearer_activate_handler          = NULL;
  */
    hdlr_tbl_p->get_data_counters_handler        =
                                                ds_3gpp_pdn_cntx_get_iface_stats;
    /*-------------------------------------------------------------------------
      Only RAB Re-establish-reject is needed for GPRS to reset the status.
      RAB Re-establish_ind is needed for GPRS as it can be received before 
      sys_mode change
      Other RAB related procedures not valid for GPRS.
    -------------------------------------------------------------------------*/
    hdlr_tbl_p->rab_rel_handler                  = ds_bearer_cntxt_call_rab_release_hdlr;
    hdlr_tbl_p->rab_re_estab_ind_handler         = 
                                ds_bearer_cntxt_call_rab_re_estab_ind_hdlr;
    hdlr_tbl_p->rab_re_estab_rej_handler         =
                                      ds_bearer_cntxt_call_rab_re_estab_rej_hdlr;
    hdlr_tbl_p->rab_re_estab_fail_handler        = NULL;
    hdlr_tbl_p->complete_ll_connect_handler      = NULL;

    /*-------------------------------------------------------------------------
      InterRAT related procedures
    -------------------------------------------------------------------------*/
    hdlr_tbl_p->sys_change_handler       =  ds3gpp_sys_change_handler;
    hdlr_tbl_p->data_suspend_ind_handler =  ds3gpp_data_suspend_ind_hdlr;
    hdlr_tbl_p->data_resume_ind_handler  =  ds3gpp_data_resume_ind_hdlr;
    hdlr_tbl_p->ps_data_fail_ind_handler =  ds3gpp_ps_data_fail_ind_hdlr;

  #ifdef FEATURE_DATA_LTE
    hdlr_tbl_p->cntxt_transfer_ind_hdlr  =  ds3gpp_cntxt_transfer_ind_hdlr;
  #endif /* FEATURE_DATA_LTE */

    /*-------------------------------------------------------------------------
      QOS related procedures
    -------------------------------------------------------------------------*/
  /* These handlers are to be defined
    hdlr_tbl_p->context_prom_ind_handler =  dsumtsps_call_promotion_hdlr;
    hdlr_tbl_p->context_mod_ind_handler  =  dsumtsps_context_modify_hdlr;
    hdlr_tbl_p->context_modify_cnf_handler =
                                              dsumtsps_context_modify_cnf_hdlr;
    hdlr_tbl_p->context_modify_rej_handler =
                                              dsumtsps_context_modify_rej_hdlr;
  */
    hdlr_tbl_p->context_modify_cnf_handler =
                                                ds_3gpp_context_modify_cnf_hdlr;
    hdlr_tbl_p->context_modify_rej_handler =
                                                ds_3gpp_context_modify_rej_hdlr;
    hdlr_tbl_p->context_mod_ind_handler    =
                                                ds_3gpp_nw_context_modify_ind_hdlr;

    hdlr_tbl_p->nw_modify_req_handler =
                                             ds_3gpp_nw_context_modify_req_hdlr;

    hdlr_tbl_p->isim_refresh_voting_handler      =
                                    ds_gprs_bearer_cntxt_refresh_voting_hdlr;
    hdlr_tbl_p->bsr_reset_hdlr = NULL;
    hdlr_tbl_p->plmn_change_hdlr = ds_3gpp_plmn_change_hdlr;
    hdlr_tbl_p->report_throughput_info_hdlr = ds_3gpp_report_throughput_info_hdlr;
    hdlr_tbl_p->send_downlink_throughput_estimation_indicaiton = NULL;
    hdlr_tbl_p->prepare_downlink_estimated_throughput_report = NULL;
    hdlr_tbl_p->get_downlink_throughput_t_accumulate = NULL;
    hdlr_tbl_p->refresh_actual_throughput_hdlr = ds_3gpp_refresh_actual_throughput_hdlr;
    hdlr_tbl_p->mode_pref_change_handler = ds_3gpp_mode_pref_change_hdlr;

    hdlr_tbl_p->coex_flow_enable_hdlr  = ds3gpp_coex_data_flow_enable_hdlr;
    hdlr_tbl_p->coex_flow_disable_hdlr = ds3gpp_coex_data_flow_disable_hdlr;
    hdlr_tbl_p->config_modem_power_stats =  ds_gprs_config_modem_power_stats;
    hdlr_tbl_p->get_modem_power_stats =  ds_gprs_get_modem_power_stats;
    hdlr_tbl_p->request_modem_power_stats = NULL;

    hdlr_tbl_p->get_total_uldl_wm_cnt_hdlr = ds3gpp_get_total_uldl_wm_cnt_hdlr;
    hdlr_tbl_p->bringdown_phys_link_hdlr = ds_3gpp_hdlr_bring_down_phys_link_hdlr;

    /*--------------------------------------------------------------------------
     Register the GPRS mode-specific handlers with 3G DS Manager
   --------------------------------------------------------------------------*/
    ds3g_register_mode_sp_handler( SYS_SYS_MODE_GSM,
                                   DS_CALL_TYPE_PKT,
                                   hdlr_tbl_p);
  }
  /*-------------------------------------------------------------------------
    Initialize gprs specific bearer context
  -------------------------------------------------------------------------*/
  ds_gprs_bearer_cntxt_init();
#endif /* FEATURE_GSM_GPRS */

#ifdef FEATURE_TDSCDMA

  hdlr_tbl_p = ds3gpp_get_handler_tbl(SYS_SYS_MODE_TDS);

  if (hdlr_tbl_p)
  {
    memset(hdlr_tbl_p,0x0,sizeof(ds3g_hdlr_func_tbl_type));
    /*-------------------------------------------------------------------------
      Initialize the TDSCDMA handler function table
    -------------------------------------------------------------------------*/
    hdlr_tbl_p->originate_call_handler          = ds3gpp_orig_call_hdlr;
    hdlr_tbl_p->call_id_handler                 = ds3gpp_call_id_hdlr;
    hdlr_tbl_p->user_end_handler                = ds3gpp_user_end_hdlr;
    hdlr_tbl_p->call_ended_handler              = ds3gpp_call_ended_hdlr;
    hdlr_tbl_p->call_connected_handler          = dsumts_call_connected_hdlr;
    hdlr_tbl_p->call_conf_handler               = NULL;
    hdlr_tbl_p->setup_handler                   = NULL;
    hdlr_tbl_p->switch_call_handler             = NULL;
    hdlr_tbl_p->force_call_down_handler         = NULL;
    hdlr_tbl_p->incoming_call_handler           = dsumts_incoming_call_handler;
    hdlr_tbl_p->user_answer_handler             = dsumts_answer_hdlr;
  /*  These handlers are to be defined.
    hdlr_tbl_p->bearer_activate_handler         = NULL;// EPS specific
  */
    /*-------------------------------------------------------------------------
      InterRAT related procedures
    -------------------------------------------------------------------------*/
    hdlr_tbl_p->sys_change_handler        =  ds3gpp_sys_change_handler;
    hdlr_tbl_p->data_suspend_ind_handler  =  ds3gpp_data_suspend_ind_hdlr;
    hdlr_tbl_p->data_resume_ind_handler   =  ds3gpp_data_resume_ind_hdlr;
    hdlr_tbl_p->ps_data_fail_ind_handler  =  ds3gpp_ps_data_fail_ind_hdlr;
    #ifdef FEATURE_DATA_LTE
    hdlr_tbl_p->cntxt_transfer_ind_hdlr   =  ds3gpp_cntxt_transfer_ind_hdlr;
    #endif /* FEATURE_LTE */
    /*-------------------------------------------------------------------------
      QOS related procedures
    -------------------------------------------------------------------------*/
  /*  These handlers are to be defined.
    hdlr_tbl_p->context_prom_ind_handler  =  dsumtsps_call_promotion_hdlr;
    hdlr_tbl_p->context_mod_ind_handler   =  dsumtsps_context_modify_hdlr;
    hdlr_tbl_p->context_modify_cnf_handler =
                                             dsumtsps_context_modify_cnf_hdlr;
    hdlr_tbl_p->context_modify_rej_handler =
                                              dsumtsps_context_modify_rej_hdlr;
  */
    /*-------------------------------------------------------------------------
      RAB related procedures are specific to WCDMA
    -------------------------------------------------------------------------*/
    hdlr_tbl_p->rab_rel_handler                 =
                                            ds_bearer_cntxt_call_rab_release_hdlr;
    hdlr_tbl_p->rab_re_estab_ind_handler        =
                                       ds_bearer_cntxt_call_rab_re_estab_ind_hdlr;
    hdlr_tbl_p->rab_re_estab_rej_handler        =
                                      ds_bearer_cntxt_call_rab_re_estab_rej_hdlr ;
    hdlr_tbl_p->rab_re_estab_fail_handler       =
                                     ds_bearer_cntxt_call_rab_re_estab_fail_hdlr ;

  /*  These handlers are to be defined.
    hdlr_tbl_p->get_call_connection_info_handler =
                                          dswpsd_get_call_connection_info_hdlr;
    hdlr_tbl_p->get_data_counters_handler =  dswpsd_get_data_counters_hdlr;
  */
    hdlr_tbl_p->get_data_counters_handler        =
                                     ds_3gpp_pdn_cntx_get_iface_stats;
    hdlr_tbl_p->isim_refresh_voting_handler      =
                                   dsWcdmaBearerCntxt_refresh_voting_hdlr;
    hdlr_tbl_p->bsr_reset_hdlr = NULL;
    hdlr_tbl_p->plmn_change_hdlr = ds_3gpp_plmn_change_hdlr;
    hdlr_tbl_p->send_downlink_throughput_estimation_indicaiton = NULL;
    hdlr_tbl_p->prepare_downlink_estimated_throughput_report = NULL;
    hdlr_tbl_p->get_downlink_throughput_t_accumulate = NULL;
    hdlr_tbl_p->report_throughput_info_hdlr = ds_3gpp_report_throughput_info_hdlr;
    hdlr_tbl_p->refresh_actual_throughput_hdlr = ds_3gpp_refresh_actual_throughput_hdlr;
    hdlr_tbl_p->mode_pref_change_handler = ds_3gpp_mode_pref_change_hdlr;  

    hdlr_tbl_p->coex_flow_enable_hdlr  = ds3gpp_coex_data_flow_enable_hdlr;
    hdlr_tbl_p->coex_flow_disable_hdlr = ds3gpp_coex_data_flow_disable_hdlr;
    hdlr_tbl_p->config_modem_power_stats =  ds_tdscdma_config_modem_power_stats;
    hdlr_tbl_p->get_modem_power_stats =  ds_tdscdma_get_modem_power_stats;
    hdlr_tbl_p->request_modem_power_stats = NULL;

    hdlr_tbl_p->get_total_uldl_wm_cnt_hdlr = ds3gpp_get_total_uldl_wm_cnt_hdlr;
    hdlr_tbl_p->bringdown_phys_link_hdlr = ds_3gpp_hdlr_bring_down_phys_link_hdlr;

    /*-------------------------------------------------------------------------
      Register the mode-specific handlers with 3G DS Manager
    -------------------------------------------------------------------------*/
    ds3g_register_mode_sp_handler( SYS_SYS_MODE_TDS,
                                   DS_CALL_TYPE_PKT,
                                   hdlr_tbl_p);

  }
  /*-------------------------------------------------------------------------
    Initialize wcdma specific bearer context
  -------------------------------------------------------------------------*/
  ds_tds_bearer_cntxt_init();
#endif /* FEATURE_TDSCDMA */
  return requested_sigs;
}
#endif /* FEATURE_DATA_WCDMA_PS || FEATURE_GSM_GPRS || FEATURE_TDSCDMA */


#ifdef FEATURE_DATA_LTE

/*===========================================================================
FUNCTION  ds3gpp_eps_bearer_activte_ind_hdlr

DESCRIPTION
  This function is invoked when the NW send a bearer activate indication.

PARAMETERS   :
  call_id                   - CM returned call-id.
  mode_info_ptr_i           - Bearer related information.
  subs_id                   - Subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL boolean ds3gpp_eps_bearer_activte_ind_hdlr
(
  cm_call_id_type             call_id,
  cm_call_mode_info_s_type   *mode_info_ptr_i,
  cm_act_bearer_rsp_s_type   *cm_act_res_p,
  sys_modem_as_id_e_type      subs_id
)
{
  /*lint -e550*/
  sys_sys_mode_e_type    network_mode;
  ds_bearer_context_s   *bearer_context_p;
#ifdef FEATURE_QXDM_WM_TRACING
  uint32                 callid;
#endif /* FEATURE_QXDM_WM_TRACING */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* check if it is default bearer scenario*/
  if(mode_info_ptr_i->info.lte_call.lbi == ESM_INVALID_BEARER_ID)
  {
    /*-------------------------------------------------------------------------
      Get the bearer context from the SDF ID
    -------------------------------------------------------------------------*/
    bearer_context_p = \
      ds_bearer_cntxt_get_bearer_from_sdf_id(mode_info_ptr_i->info.lte_call.sdf_id);

    if (bearer_context_p == NULL)
    {
      DS_3GPP_MSG1_HIGH("Cannot find bearer corresponding to the SDF ID 0x%x,\
                       Passing on to see if this is a N/w inited QOS call",
                       mode_info_ptr_i->info.lte_call.sdf_id);
    }
    else
    {
    /*----------------------------------------------------------------------------
      Store the CM call id in the bearer context.
      Here is where we store the call ID for UE initiated calls.
      We need to store call_id before rejecting the bearer otherwise call 
      cleanup would fail.
      For N/w init calls, bearer allocation and storing of the CM call ID will
      happen in PDN context activate bearer indication handler
    ---------------------------------------------------------------------------*/
      ds_bearer_cntxt_store_call_id(bearer_context_p, call_id);
    }
  }

  /*-------------------------------------------------------------------------
    Get the current network mode.
  -------------------------------------------------------------------------*/
  network_mode = ds3gpp_get_current_network_mode(subs_id);
  if(network_mode != SYS_SYS_MODE_LTE)
  {
    DS_3GPP_MSG1_ERROR("DS 3GPP Invalid network_mode:%d",network_mode);
    return DS3G_FAILURE;
  }

  /*-------------------------------------------------------------------------
    We should not get the bearer activate ind event from Call Manager while
    we are suspended. Return DS3G_FAILURE if this happens.
  -------------------------------------------------------------------------*/
  if (ds3gpp_get_ps_data_suspend_flag(subs_id) == TRUE)
  {
    DS_3GPP_MSG1_ERROR("Suspended:Invalid bearer activate ind call_id:%d",call_id);
    //return DS3G_FAILURE;
  }

  /*-------------------------------------------------------------------------
    Invoke PDN bearerActivationInd Callback
  -------------------------------------------------------------------------*/
  if(ds_pdn_cntxt_bearer_activate_ind(call_id,
                                      mode_info_ptr_i,
                                      cm_act_res_p,
                                      subs_id) != DS3G_SUCCESS)
  {
     return DS3G_FAILURE;
  }

  /*-------------------------------------------------------------------------
    Invoke BEARER bearerActivationInd Callback
  -------------------------------------------------------------------------*/
  if(ds_bearer_cntxt_bearer_activate_ind(call_id,
                                         mode_info_ptr_i, 
                                         subs_id) != DS3G_SUCCESS)
  {
    return DS3G_FAILURE;
  }

#ifdef FEATURE_QXDM_WM_TRACING
  callid = ds_bearer_cntxt_get_call_id(bearer_context_p);
  DS_3GPP_MSG1_MED("MH Client: CALL_ID value (%d)",callid);

  /*-------------------------------------------------------------------------
    Register with FC for watermark tracing
  -------------------------------------------------------------------------*/
  switch (network_mode)
  {
    case SYS_SYS_MODE_LTE:
      DS_3GPP_MSG1_MED("MH Client: Registering with FC for LTE_TECH, call id %d",
              callid);
      fc_post_reg_cmd_wm_trace(LTE_TECH, //tech id
                               callid,
                               UNIFIED_3GPP_MODE_HANDLER_CLIENT,
                               ds3gpp_report_wm_stats_cb,
                               (uint32)bearer_context_p->client_data_ptr);
      break;

    default:
      DS_3GPP_MSG2_MED("Not registering with FC for call_id: %d, mode %d",
                        callid, network_mode);
      break;
  }
#endif /* FEATURE_QXDM_WM_TRACING */

  return(DS3G_SUCCESS);

} /* ds3gpp_eps_bearer_activte_ind_hdlr() */

/*===========================================================================
FUNCTION DS3GPP_EPS_PDN_CONN_REJ_IND_HDLR

DESCRIPTION
  This function is used to handle the PDN Conn Reject indication.
  It calls the bearer deactivate function which does the clean up of the
  bearer and intiates the clean up of the PDN context.

  This function is applicable only to EPS mode of operation.

PARAMETERS
  call_info_p - Call info from DSMGR which contains CM provided mode_info
  subs_id     - Subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds3gpp_eps_pdn_conn_rej_ind_hdlr
(
  ds3g_call_info_cmd_type       *call_info_p,
  sys_modem_as_id_e_type         subs_id
)
{
  ds_bearer_context_s            *bearer_context_p;
  ds_pdn_context_s               *pdn_cntx_p;
  cm_call_mode_info_s_type       *mode_info_ptr;
  ds_eps_attach_sm_s             *attach_sm_ptr = NULL;
  ds_3gpp_iface_s                *ds_3gpp_v4_iface_p = NULL;
  ds_3gpp_iface_s                *ds_3gpp_v6_iface_p = NULL;
  ds3g_eps_attach_sm_info_type    attach_info;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mode_info_ptr = (cm_call_mode_info_s_type*) &(call_info_p->mode_info);
  bearer_context_p = \
    ds_bearer_cntxt_get_bearer_from_sdf_id(mode_info_ptr->info.lte_call.sdf_id);

  if(bearer_context_p == NULL)
  {
    DS_3GPP_MSG1_HIGH("Cannot locate bearer from SDF ID 0x%x while proc PDN conn Rej",
               mode_info_ptr->info.lte_call.sdf_id);
    return;
  }

  /*-------------------------------------------------------------------------
    Check the bearer mode.   
  -------------------------------------------------------------------------*/  
  if(bearer_context_p->ds_bearer_context_dyn_p->call_mode != SYS_SYS_MODE_LTE)
  {
    DS_3GPP_MSG2_ERROR("Invalid bearer call_mode:%d, changing it to %d",
                       (bearer_context_p->ds_bearer_context_dyn_p->call_mode), 
                        SYS_SYS_MODE_LTE);
    bearer_context_p->ds_bearer_context_dyn_p->call_mode = SYS_SYS_MODE_LTE;    
  }

  /*-------------------------------------------------------------------------
    Get the PDN context from the bearer context
  -------------------------------------------------------------------------*/
  pdn_cntx_p = (ds_pdn_context_s*)ds_bearer_cntxt_get_pdn_cntxt(bearer_context_p);

  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return;
  }

  /*-------------------------------------------------------------------------
    Invoke the EPS specific PDN connectivity reject indication handler
  -------------------------------------------------------------------------*/
  ds_eps_pdn_cntxt_pdn_conn_rej_ind_hdlr(pdn_cntx_p, mode_info_ptr);

  attach_sm_ptr = ds_eps_get_attach_sm_per_sub_id(subs_id);
  if(attach_sm_ptr == NULL)
  {
    DS_3GPP_MSG1_ERROR("Invalid Attach SM ptr for cm sub id %d",subs_id);
    return;
  }

  /*-------------------------------------------------------------------------
    Send deactivate to clean up bearer. Set call_was_connected flag to FALSE
    Down reason is set to PS_NET_DOWN_REASON_ACTIVATION_REJECT while the
    extended info code is set to NOT_SPECIFIED
    However if we have a cached pdn connectivity request and if it is GW
    originated and the if the previous request was rejected because of a
    sequence number mismatchwe don't want to tear down the PDN context,
    We instead will use the same profile parameters.
    Also ensure that the client is in_use
  -------------------------------------------------------------------------*/
  ds_3gpp_v4_iface_p = 
      pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV4_IFACE];
  ds_3gpp_v6_iface_p = 
      pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV6_IFACE];

  if((attach_sm_ptr->is_gw_originated) && 
     (ds_eps_pdn_cntx_is_pdn_conn_req_pending(subs_id)) &&
     ((mode_info_ptr->info.lte_call.esm_local_cause.valid) &&
      (mode_info_ptr->info.lte_call.esm_local_cause.local_cause == EMM_DETACHED)) &&
     (((ds_3gpp_v4_iface_p != NULL) &&
       (ds_3gpp_v4_iface_p->state == DS_3GPP_IFACE_IN_USE_STATE)) ||
     ((ds_3gpp_v6_iface_p != NULL) &&
      (ds_3gpp_v6_iface_p->state == DS_3GPP_IFACE_IN_USE_STATE))))
  {
    DS_3GPP_MSG3_HIGH("Re originating cached PDN conn request:%d,cached_paging_info:%d on Subs Id(CM)"
                      "%d", attach_sm_ptr->cached_seq_num,attach_sm_ptr->cached_paging_response,
                      subs_id);
    DS_3GPP_MSG3_HIGH("Setting the pdn context and bearer context to PEND_REDIAL",
                      0,0,0);
    bearer_context_p->ds_bearer_context_dyn_p->state = DS_BEARER_CONTEXT_STATE_PEND_REDIAL;
    pdn_cntx_p->ds_pdn_context_dyn_p->state = DS_PDN_CONTEXT_STATE_PEND_REDIAL;
    ds_eps_attach_sm_set_state(DS_EPS_REDIAL_GET_PDN_CONN_IND,subs_id);
    attach_info.responding_to_page = attach_sm_ptr->cached_paging_response;
    attach_info.seq_num = attach_sm_ptr->cached_seq_num;
    ds3gpp_eps_get_pdn_conn_ind_hdlr(&attach_info,subs_id);
  }
  else
  {
    attach_sm_ptr->is_gw_originated = FALSE;
    if((!mode_info_ptr->info.lte_call.esm_local_cause.valid) ||
      (mode_info_ptr->info.lte_call.esm_local_cause.local_cause != EMM_DETACHED))
    {
      DS_3GPP_MSG3_HIGH("Resetting cached seq_num",0,0,0);
      attach_sm_ptr->cached_seq_num = 0;
      attach_sm_ptr->cached_paging_response = FALSE;
    }
    ds_bearer_cntxt_deactivate_cnf
      (
         bearer_context_p,
         call_info_p,
         2,               //end_type=reject
         FALSE
      );
  }

  return;

} /* ds3gpp_eps_pdn_conn_rej_ind_hdlr */

/*===========================================================================
FUNCTION DS3GPP_EPS_PDN_CONN_FAIL_IND_HDLR

DESCRIPTION
  This function is used to handle the PDN Conn Fail indication.
  The PDN Conn Fail is given as a result of a PDN conn req being aborted.
  It calls the bearer deactivate function which does the clean up of the
  bearer and intiates the clean up of the PDN context.

  This function is applicable only to EPS mode of operation.

PARAMETERS
  mode_info_ptr - mode specific info passed in through the CM
  subs_id       - Subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds3gpp_eps_pdn_conn_fail_ind_hdlr
(
  ds3g_call_info_cmd_type          *call_info_p,
  sys_modem_as_id_e_type            subs_id
)
{
  ds_bearer_context_s            *bearer_context_p;
  cm_call_mode_info_s_type       *mode_info_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mode_info_ptr = (cm_call_mode_info_s_type*) &(call_info_p->mode_info);
  bearer_context_p = \
    ds_bearer_cntxt_get_bearer_from_sdf_id(mode_info_ptr->info.lte_call.sdf_id);

  if(bearer_context_p == NULL)
  {
    DS_3GPP_MSG1_HIGH("Cannot locate bearer from SDF ID: %d while proc PDN conn Fail",
               mode_info_ptr->info.lte_call.sdf_id);
    return;
  }

  /*-------------------------------------------------------------------------
    Send deactivate to clean up bearer. Set call_was_connected flag to FALSE
    Down reason is set to PS_NET_DOWN_REASON_ACTIVATION_REJECT while the
    extended info code is set to NOT_SPECIFIED
  -------------------------------------------------------------------------*/
  ds_bearer_cntxt_deactivate_cnf
  (
    bearer_context_p,
    call_info_p,
    1,
    FALSE
  );

  return;

} /* ds3gpp_eps_pdn_conn_fail_ind_hdlr */

/*===========================================================================
FUNCTION DS3GPP_EPS_PDN_CONN_REQ_ERR_HDLR

DESCRIPTION
  This function is used to handle the PDN Conn request error.
  It calls the bearer deactivate function which does the clean up of the
  bearer and intiates the clean up of the PDN context.

  This function is applicable only to EPS mode of operation.

PARAMETERS
  cmd_ptr       - command info
  subs_id       - Subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds3gpp_eps_pdn_conn_req_err_hdlr
(
  ds_cmd_type               *cmd_ptr,
  sys_modem_as_id_e_type     subs_id
)
{ 
  ds_bearer_context_s           *bearer_context_p;
  ds3g_call_info_cmd_type       *call_info_cmd_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_3GPP_MSG0_HIGH( "In ds3gpp_eps_pdn_conn_req_err_hdlr" );
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DS_3GPP_MSG0_ERROR( "NULL pointer passed" );
    return;
  }
  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  bearer_context_p = \
    ds_bearer_cntxt_get_bearer_from_sdf_id(call_info_cmd_ptr->sdf_id);

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }
  DS_3GPP_MSG0_HIGH( "calling ds_bearer_cntxt_deactivate_cnf" );
  ds_bearer_cntxt_deactivate_cnf
  (
    bearer_context_p,
    call_info_cmd_ptr,
    0,
    FALSE
  );

  return;
}

/*===========================================================================
FUNCTION DS3GPP_EPS_GET_PDN_CONN_IND_HDLR

DESCRIPTION
  This function is used to handle the incoming attach request from CM.
  It dispatches the EPS PDN context function to appropriately send out a
  PDN Connectivity request.

PARAMETERS
  attach_info_p.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds3gpp_eps_get_pdn_conn_ind_hdlr
(
  ds3g_eps_attach_sm_info_type    *attach_info_ptr,
  sys_modem_as_id_e_type  subs_id
)
{

  sys_sys_mode_e_type            network_mode;
  sys_plmn_id_s_type             current_plmn_id;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get the current network mode.
  -------------------------------------------------------------------------*/
  network_mode = ds3gpp_get_current_network_mode(subs_id);
  if(network_mode != SYS_SYS_MODE_LTE)
  {
    DS_3GPP_MSG1_ERROR("DS 3GPP Invalid network_mode:%d,\
                        Updating DS3GPP internal mode to LTE",
                        network_mode);
    ds3gpp_update_network_mode(SYS_SYS_MODE_LTE, subs_id);
  }

  memset(&current_plmn_id, 0, sizeof(sys_plmn_id_s_type));
  
  ds_3gpp_pdn_cntx_get_current_lte_plmn(&current_plmn_id, subs_id);    

  if (TRUE == ds_3gpp_throt_sm_is_plmn_non_null(current_plmn_id))
  {
    ds_3gpp_roaming_inform_new_plmn_info(subs_id, current_plmn_id);
  }

  /*-------------------------------------------------------------------------
    Invoke the EPS PDN context function to initiate the PDN connectivity req
  -------------------------------------------------------------------------*/
  if(ds_eps_pdn_cntxt_get_pdn_conn_ind_hdlr(attach_info_ptr,subs_id) == DS3G_SUCCESS)
  {
    /*-------------------------------------------------------------------------
      Set the flag to TRUE. Flag is used to indicate that the attach process
      has been initiated
    -------------------------------------------------------------------------*/
    ds3gpp_set_is_attach_initiated_flag(TRUE, subs_id);
  }

} /* ds3gpp_eps_get_pdn_conn_ind_hdlr */


/*===========================================================================
FUNCTION DS3GPP_BEARER_ALLOC_REJ_IND_HDLR

DESCRIPTION
  This function is used to handle the Bearer allocation rejection indication.
  The indication is given when the NW reject the alocation request.

  This function is applicable only to EPS mode of operation.

PARAMETERS
  mode_info_ptr - mode specific info passed in through the CM
  subs_id       - Subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds3gpp_bearer_alloc_rej_ind_hdlr
(
  cm_call_mode_info_s_type  *mode_info_ptr,
  sys_modem_as_id_e_type     subs_id
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Invoke the flow manager call function to handle bearer allocation
    rejection.
  -------------------------------------------------------------------------*/
   ds_flow_manager_nw_bearer_alloc_rej(mode_info_ptr, subs_id);

  return;
} /* ds3gpp_bearer_alloc_rej_ind_hdlr */

/*===========================================================================
FUNCTION DS3GPP_BEARER_ALLOC_FAIL_IND_HDLR

DESCRIPTION
  This function is used to handle the Bearer allocation failure indication.
  The indication is given because UE internally could not process the allocation.

  This function is applicable only to EPS mode of operation.

PARAMETERS
  mode_info_ptr - mode specific info passed in through the CM
  subs_id       - Subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds3gpp_bearer_alloc_fail_ind_hdlr
(
  cm_call_mode_info_s_type  *mode_info_ptr,
  sys_modem_as_id_e_type     subs_id
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Invoke the flow manager call function to handle bearer allocation
    rejection.
  -------------------------------------------------------------------------*/
  ds_flow_manager_nw_bearer_alloc_fail(mode_info_ptr, subs_id);

  return;

} /* ds3gpp_bearer_alloc_fail_ind_hdlr */




/*===========================================================================
FUNCTION DSEPS_HDLR_INIT

DESCRIPTION
  This function initializes the 3GPP Packet-Switched Data mode-specific
  handlers. It is invoked during DS Task power-up.
  It creates interfaces for each pdn context and initializes the
  parameters for each pdn context.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  Signal mask containing the REX signals that the handlers want to wait on.

SIDE EFFECTS
  None.

===========================================================================*/
rex_sigs_type dseps_hdlr_init( void )
{
  rex_sigs_type             requested_sigs = 0;
                             /* Signal mask to wait on */
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_p = NULL;

  ds_eps_pdn_cntxt_init();
  ds_eps_bearer_cntxt_init();
  ds_eps_flow_manager_init();
  ds_eps_bearer_flow_manager_init();

  hdlr_tbl_p = ds3gpp_get_handler_tbl(SYS_SYS_MODE_LTE);

  if (hdlr_tbl_p)
  {
    /*-------------------------------------------------------------------------
     Initialize the EPS handler function table
    -------------------------------------------------------------------------*/

    hdlr_tbl_p->originate_call_handler_ext       = ds3gpp_orig_call_hdlr_ext;
    hdlr_tbl_p->call_id_handler                  = ds3gpp_call_id_hdlr;
    hdlr_tbl_p->user_end_handler                 = ds3gpp_user_end_hdlr;
    hdlr_tbl_p->call_ended_handler               = ds3gpp_call_ended_hdlr;

    /* Not Valid */
    hdlr_tbl_p->call_conf_handler                = NULL;
    hdlr_tbl_p->call_connected_handler           = NULL;
    hdlr_tbl_p->setup_handler                    = NULL;
    hdlr_tbl_p->incoming_call_handler            = NULL;
    hdlr_tbl_p->user_answer_handler              = NULL;
    hdlr_tbl_p->switch_call_handler              = NULL;
    hdlr_tbl_p->force_call_down_handler          = NULL;

    hdlr_tbl_p->bearer_activate_handler          =
                                          ds3gpp_eps_bearer_activte_ind_hdlr;
  //dslteps_eps_bearer_activate_ind_hdlr;

    /*-------------------------------------------------------------------------
      TODO: Miscellaneous
    -------------------------------------------------------------------------*/
    hdlr_tbl_p->get_data_counters_handler        =
                                          ds_3gpp_pdn_cntx_get_iface_stats;
    hdlr_tbl_p->isim_refresh_voting_handler      =
                                  ds_eps_bearer_cntxt_isim_refresh_voting_hdlr;

    /*-------------------------------------------------------------------------
      TODO: RAB related procedures
    -------------------------------------------------------------------------*/
    hdlr_tbl_p->rab_rel_handler                  =
                                   ds_bearer_cntxt_call_rab_release_hdlr;
    hdlr_tbl_p->rab_re_estab_ind_handler         =
                                   ds_bearer_cntxt_call_rab_re_estab_ind_hdlr;
    hdlr_tbl_p->rab_re_estab_rej_handler         =
                                   ds_bearer_cntxt_call_rab_re_estab_rej_hdlr;
    hdlr_tbl_p->rab_re_estab_fail_handler        = NULL;
    hdlr_tbl_p->complete_ll_connect_handler      = NULL;
    hdlr_tbl_p->get_pdn_conn_ind_hdlr           =
                                   ds3gpp_eps_get_pdn_conn_ind_hdlr;

    hdlr_tbl_p->pdn_conn_rej_ind_hdlr = ds3gpp_eps_pdn_conn_rej_ind_hdlr;
    hdlr_tbl_p->pdn_conn_fail_ind_hdlr = ds3gpp_eps_pdn_conn_fail_ind_hdlr;
    hdlr_tbl_p->pdn_conn_req_err_hdlr =  ds3gpp_eps_pdn_conn_req_err_hdlr;


    /*-------------------------------------------------------------------------
      TODO: InterRAT related procedures
    -------------------------------------------------------------------------*/
    hdlr_tbl_p->data_suspend_ind_handler =  ds3gpp_data_suspend_ind_hdlr;
    hdlr_tbl_p->data_resume_ind_handler  =  ds3gpp_data_resume_ind_hdlr;
    hdlr_tbl_p->ps_data_fail_ind_handler =  ds3gpp_ps_data_fail_ind_hdlr;
    hdlr_tbl_p->sys_change_handler       =  ds3gpp_sys_change_handler;
    #if defined(FEATURE_DATA_LTE) && (defined(FEATURE_GSM_GPRS) || defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_TDSCDMA))
    hdlr_tbl_p->cntxt_transfer_ind_hdlr  =  ds3gpp_cntxt_transfer_ind_hdlr;
    #endif /* (FEATURE_GSM_GPRS || FEATURE_DATA_WCDMA_PS || FEATURE_TDSCDMA) */

    /*-------------------------------------------------------------------------
      TODO: QOS related procedures
    -------------------------------------------------------------------------*/
    hdlr_tbl_p->bearer_alloc_rej_ind_hdlr =
                                           ds3gpp_bearer_alloc_rej_ind_hdlr;
    hdlr_tbl_p->bearer_alloc_fail_ind_hdlr=
                                           ds3gpp_bearer_alloc_fail_ind_hdlr;
    hdlr_tbl_p->bearer_mod_rej_ind_hdlr =
                                           ds3gpp_bearer_modify_rej_ind_hdlr;
    hdlr_tbl_p->bearer_mod_ind_hdlr =  ds3gpp_bearer_modify_ind_hdlr;
    //hdlr_tbl_p->context_prom_ind_handler =  dsumtsps_call_promotion_hdlr;
    //hdlr_tbl_p->context_mod_ind_handler  =  dsumtsps_context_modify_hdlr;
    //hdlr_tbl_p->context_modify_cnf_handler =
      //                                        dsumtsps_context_modify_cnf_hdlr;
    //hdlr_tbl_p->context_modify_rej_handler =
      //                                        dsumtsps_context_modify_rej_hdlr;

    hdlr_tbl_p->bsr_reset_hdlr = ds_3gpp_bsr_reset_handler;
    hdlr_tbl_p->plmn_change_hdlr = ds_3gpp_plmn_change_hdlr;
    hdlr_tbl_p->send_downlink_throughput_estimation_indicaiton = 
       ds_eps_dl_tput_timer_expire_hdlr;
    hdlr_tbl_p->report_throughput_info_hdlr = ds_3gpp_report_throughput_info_hdlr;
    hdlr_tbl_p->refresh_actual_throughput_hdlr = ds_3gpp_refresh_actual_throughput_hdlr;
    hdlr_tbl_p->get_downlink_throughput_t_accumulate = ds_eps_dl_estimated_throughput_t_accumuate;
    hdlr_tbl_p->prepare_downlink_estimated_throughput_report = ds_eps_dl_tput_reporting_frequency_modify_hdlr;
    hdlr_tbl_p->mode_pref_change_handler = ds_3gpp_mode_pref_change_hdlr;

    hdlr_tbl_p->coex_flow_enable_hdlr  = ds3gpp_coex_data_flow_enable_hdlr;
    hdlr_tbl_p->coex_flow_disable_hdlr = ds3gpp_coex_data_flow_disable_hdlr;
    hdlr_tbl_p->config_modem_power_stats =  ds_eps_config_modem_power_stats;
    hdlr_tbl_p->get_modem_power_stats =  ds_eps_get_modem_power_stats;
    hdlr_tbl_p->request_modem_power_stats = ds_eps_request_modem_power_stats;

    hdlr_tbl_p->get_total_uldl_wm_cnt_hdlr = ds3gpp_get_total_uldl_wm_cnt_hdlr;
    hdlr_tbl_p->bringdown_phys_link_hdlr = ds_3gpp_hdlr_bring_down_phys_link_hdlr;

    /*--------------------------------------------------------------------------
     Register the GPRS mode-specific handlers with 3G DS Manager
   --------------------------------------------------------------------------*/
    ds3g_register_mode_sp_handler( SYS_SYS_MODE_LTE,
                                   DS_CALL_TYPE_PKT,
                                   hdlr_tbl_p);
  }
  return (requested_sigs);
} /* dseps_hdlr_init */

#endif /* FEATURE_DATA_LTE */

/*===========================================================================
FUNCTION DS3GPP_GET_IS_ATTACH_INITIATED_FLAG

DESCRIPTION
  This function returns the flag ds3gpp_is_attach_initiated

PARAMETERS
  subs_id - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  True if attach has been initiated, False otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3gpp_get_is_attach_initiated_flag
(
  sys_modem_as_id_e_type subs_id
)
{
  boolean                    flag = FALSE;
  ds3gpp_per_subs_info_type* hndl = NULL;
  /*----------------------------------------------------------------------*/

  hndl = ds3gpp_get_per_subs_info_hndl(subs_id);

  if (ds3gpp_validate_per_subs_info_hndl(hndl))
  {
    flag = hndl->attach_is_initiated;
    DS_3GPP_MSG2_LOW("Attach initiated flag for Subs Id(CM): %d is %d",
                     subs_id, flag);
  }

  return flag;
} /* ds3gpp_get_is_attach_initiated_flag */

/*===========================================================================
FUNCTION DS3GPP_SET_IS_ATTACH_INITIATED_FLAG

DESCRIPTION
  This function resets the flag ds3gpp_is_attach_initiated

PARAMETERS
  flag_val - New Flag Value
  subs_id  - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds3gpp_set_is_attach_initiated_flag
(
  boolean                flag_val,
  sys_modem_as_id_e_type subs_id
)
{
  ds3gpp_per_subs_info_type* hndl = NULL;
  /*----------------------------------------------------------------------*/

  hndl = ds3gpp_get_per_subs_info_hndl(subs_id);

  if (ds3gpp_validate_per_subs_info_hndl(hndl))
  {
    hndl->attach_is_initiated = flag_val;
    DS_3GPP_MSG2_MED("Attach initiated flag for Subs Id(CM): %d is %d",
                      subs_id, flag_val);
  }
  return;
} /* ds3gpp_set_is_attach_initiated_flag */

/*===========================================================================
FUNCTION DS3GPP_SET_IS_ATTACH_COMPLETE_FLAG

DESCRIPTION
  This function sets the flag ds3gpp_is_attach_complete to the value passed in
  the argument

PARAMETERS
  flag_val - New Flag Value
  subs_id  - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void ds3gpp_set_is_attach_complete_flag
(
  boolean                flag_val,
  sys_modem_as_id_e_type subs_id
)
{

  ds3gpp_per_subs_info_type* hndl = NULL;
  /*----------------------------------------------------------------------*/

  hndl = ds3gpp_get_per_subs_info_hndl(subs_id);

  if (ds3gpp_validate_per_subs_info_hndl(hndl))
  {
    hndl->attach_is_complete = flag_val;
    DS_3GPP_MSG2_MED("Attach complete flag for Subs Id(CM): %d is %d",
                      subs_id, flag_val);
  }

} /* ds3gpp_set_is_attach_complete_flag */

/*===========================================================================
FUNCTION DS3GPP_GET_IS_ATTACH_COMPLETE_FLAG

DESCRIPTION
  This function gets the flag ds3gpp_is_attach_complete

PARAMETERS
  Subs_Id - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3gpp_get_is_attach_complete_flag
(
  sys_modem_as_id_e_type subs_id
)
{
  boolean                    flag = FALSE;
  ds3gpp_per_subs_info_type* hndl = NULL;
  /*----------------------------------------------------------------------*/

  hndl = ds3gpp_get_per_subs_info_hndl(subs_id);

  if (ds3gpp_validate_per_subs_info_hndl(hndl))
  {
    flag = hndl->attach_is_complete;
    DS_3GPP_MSG2_MED("Attach complete flag for cm subs id: %d is %d",
                      subs_id, flag);
  }

  return flag;
} /* ds3gpp_get_is_attach_complete_flag */

/*===========================================================================
FUNCTION DS3GPP_SET_PS_DATA_SUSPEND_FLAG

DESCRIPTION
  This function sets the PS Data Suspend Flag to the value passed in
  the argument

PARAMETERS
  flag_val - New Flag Value
  subs_id  - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void ds3gpp_set_ps_data_suspend_flag
(
  boolean                flag_val,
  sys_modem_as_id_e_type subs_id
)
{

  ds3gpp_per_subs_info_type* hndl = NULL;
  /*----------------------------------------------------------------------*/

  hndl = ds3gpp_get_per_subs_info_hndl(subs_id);

  if (ds3gpp_validate_per_subs_info_hndl(hndl))
  {
    hndl->data_suspend_flag = flag_val;
    DS_3GPP_MSG2_MED("PS Data Suspend flag for cm subs id: %d is %d",
                      subs_id, flag_val);
  }

} /* ds3gpp_set_ps_data_suspend_flag */

/*===========================================================================
FUNCTION DS3GPP_GET_PS_DATA_SUSPEND_FLAG

DESCRIPTION
  This function gets the PS Data Suspend Flag

PARAMETERS
  Subs_Id - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3gpp_get_ps_data_suspend_flag
(
  sys_modem_as_id_e_type subs_id
)
{
  boolean                    flag = FALSE;
  ds3gpp_per_subs_info_type* hndl = NULL;
  /*----------------------------------------------------------------------*/

  hndl = ds3gpp_get_per_subs_info_hndl(subs_id);

  if (ds3gpp_validate_per_subs_info_hndl(hndl))
  {
    flag = hndl->data_suspend_flag;
  }

  return flag;
} /* ds3gpp_get_ps_data_suspend_flag */

/*===========================================================================
FUNCTION DS3GPP_SET_PS_DATA_AVAILABLE_FLAG

DESCRIPTION
  This function sets the PS Data Available to the value passed in
  the argument

PARAMETERS
  flag_val - New Flag Value
  subs_id  - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void ds3gpp_set_ps_data_available_flag
(
  boolean                flag_val,
  sys_modem_as_id_e_type subs_id
)
{

  ds3gpp_per_subs_info_type* hndl = NULL;
  /*----------------------------------------------------------------------*/

  hndl = ds3gpp_get_per_subs_info_hndl(subs_id);

  if (ds3gpp_validate_per_subs_info_hndl(hndl))
  {
    hndl->sent_ps_data_available = flag_val;
    DS_3GPP_MSG2_MED("PS Data available flag for subs id: %d is %d",
                      subs_id, flag_val);
  }

} /* ds3gpp_set_ps_data_available_flag */

/*===========================================================================
FUNCTION DS3GPP_GET_PS_DATA_AVAILABLE_FLAG

DESCRIPTION
  This function gets the PS Data Available Flag

PARAMETERS
  subs_id  - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3gpp_get_ps_data_available_flag
(
  sys_modem_as_id_e_type subs_id
)
{
  boolean                    flag = FALSE;
  ds3gpp_per_subs_info_type* hndl = NULL;
  /*----------------------------------------------------------------------*/

  hndl = ds3gpp_get_per_subs_info_hndl(subs_id);

  if (ds3gpp_validate_per_subs_info_hndl(hndl))
  {
    flag = hndl->sent_ps_data_available;
    DS_3GPP_MSG2_MED("PS Data Available flag for Subs Id(CM): %d is %d",
                      subs_id, flag);
  }

  return flag;
} /* ds3gpp_get_ps_data_available_flag */

/*===========================================================================
FUNCTION DS_3GPPI_ALLOC_EVENT_BUF

DESCRIPTION
  This function is invoked while registering for DS 3GPP events to
  allocate an event buffer.
    
DEPENDENCIES
  None

RETURN VALUE
  Pointer to event buffer on success
  NULL on allcoation failure

SIDE EFFECTS
  None

===========================================================================*/
LOCAL ds_3gpp_event_buf_type* ds_3gppi_event_alloc_buf
(
  ds_3gpp_event_cback_type callback_fptr,
  uint32                   user_data
)
{
 ds_3gpp_event_buf_type * event_buf;

 event_buf = DS_3GPP_MEM_ALLOC(sizeof(ds_3gpp_event_buf_type));
 if(event_buf != NULL)
 {
   memset(event_buf, 0, sizeof(ds_3gpp_event_buf_type));
   event_buf->event_callback_fptr = callback_fptr;
   event_buf->user_data = user_data;
 }
 return event_buf;
}

/*===========================================================================
FUNCTION DS_3GPPI_EVENT_PUT_BUF

DESCRIPTION
  This function queues the event buffer in the appropriate event queue
 
PARAMETERS 
 event - specifies the event
 event_buf - sepcifies event buffer to be queued for "event"
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void ds_3gppi_event_put_buf
(
  ds_3gpp_event_enum_type event,
  ds_3gpp_event_buf_type *event_buf
)
{
  if(event >= DS_3GPP_EV_MIN && event < DS_3GPP_EV_MAX)
  {
    q_put(&(ds3gpp_info.event_q[event]), &(event_buf->q_link));
  }
}

/*===========================================================================
FUNCTION DS_3GPP_EVENT_INIT

DESCRIPTION
  This function initializes the 3GPP Events module
 
PARAMETERS 
 None
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_event_init(void)
{
  ds_3gpp_event_enum_type event;
  for(event = DS_3GPP_EV_MIN; event < DS_3GPP_EV_MAX; event++)
  {
    q_init(&ds3gpp_info.event_q[event]);
  }
}
/*===========================================================================
FUNCTION DS_3GPP_REGISTER_EVENT_CB

DESCRIPTION
  This function registers a callback function DS 3GPP events.
    
DEPENDENCIES
  None

RETURN VALUE
  DS3G_SUCCESS - on successful registration
  DS3G_FAILURE - if registration fails

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_3gpp_event_register_cb
(
  ds_3gpp_event_enum_type  event,
  ds_3gpp_event_cback_type callback_fptr,
  uint32                   user_data
)
{
  ds_3gpp_event_buf_type *event_buf = NULL;
  
  if(callback_fptr == NULL)
  {
    DS_3GPP_MSG1_ERROR("ds_3gpp_event_register_cb: Event registration failed. "
                       "callback_fptr NULL, event: %d", event);
    return DS3G_FAILURE;
  }

  if(event < DS_3GPP_EV_MIN || event >= DS_3GPP_EV_MAX)
  {
    DS_3GPP_MSG1_ERROR("ds_3gpp_event_register_cb: Event registration failed. "
                      "Invalid event %d", event);
    return DS3G_FAILURE;
  }

  event_buf = ds_3gppi_event_alloc_buf(callback_fptr, user_data);
  if(event_buf == NULL)
  {
    DS_3GPP_MSG1_ERROR("ds_3gpp_event_register_cb:  Event registration failed. "
                       "Unable to alloc memory for event buffer. Event %d", 
                        event);
    return DS3G_FAILURE;
  }

  DS_3GPP_MSG3_HIGH("ds_3gpp_event_register_cb: Registered function: %x, "
                    "event: %d, user_data: %d", callback_fptr, event, user_data);

  ds_3gppi_event_put_buf(event, event_buf);

  return DS3G_SUCCESS;
}

#ifdef TEST_FRAMEWORK
#error code not present
#endif

/*===========================================================================
FUNCTION DS_3GPP_EVENT_INVOKE_CBS

DESCRIPTION
  This function invokes all the registered callback functions for a
  specific event
 
PARAMETERS
  event - event for which to invoke the registered callbacks
  event_info - info regarding the event 
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_event_invoke_cbs
(
  ds_3gpp_event_enum_type event,
  ds_3gpp_event_info_u_type event_info
)
{
   ds_3gpp_event_buf_type *event_buf;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Obain the event buffers, one at a time, for the specific event
     from the event pool.
   */
  if(event < DS_3GPP_EV_MIN
      || event >= DS_3GPP_EV_MAX )
  {
    return;
  }

  for(event_buf = q_check(&ds3gpp_info.event_q[event]);
        event_buf != NULL;
            event_buf = q_next((&ds3gpp_info.event_q[event]), 
                                 &(event_buf->q_link)))
  {    

    if(event_buf->event_callback_fptr == NULL)
    {
      DS_3GPP_MSG0_ERROR("ds_3gpp_process_oprt_mode_chg_cmd: NULL "
                         "event_cback_f_ptr");
      continue;
    }
    
    /* Invoke event callback function with appropriate args */
    DS_3GPP_MSG2_HIGH("ds_3gpp_event_invoke_cbs: invoking fptr %x with "
                      "user_data %d", event_buf->event_callback_fptr, 
                                      event_buf->user_data); 
    event_buf->event_callback_fptr(event_info, event_buf->user_data);
  }
}

/*===========================================================================
FUNCTION ds_3gpp_profile_hdlr_init

DESCRIPTION
  This initializes the 3GPP specific DS profile event handler function table

PARAMETERS
  void  
  
DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
===========================================================================*/
void  ds_3gpp_profile_hdlr_init
(
  void
)
{
  ds_profile_3gpp_cb_func_tbl_type* cb_func_tbl_p = 
                                      ds3gpp_get_ds_profile_cb_func_tbl();
  /*-----------------------------------------------------------------------*/

  if  (cb_func_tbl_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("cb_func_tbl_p is NULL");
    return;
  }


  cb_func_tbl_p->apn_name_change_handler = ds_3gpp_apn_name_chg_hdlr;

  cb_func_tbl_p->pdp_type_chg_hdlr = ds_3gpp_pdp_type_chg_hdlr;

#ifdef FEATURE_DATA_LTE

  cb_func_tbl_p->apn_disable_flag_change_handler = ds_3gpp_apn_disable_hdlr;

  cb_func_tbl_p->roaming_disallowed_flag_chg_hdlr = 
                                        ds_3gpp_roaming_disallowed_chg_hdlr;
  cb_func_tbl_p->max_pdn_conn_per_blk_chg_hdlr = 
                                       ds_3gpp_max_pdn_conn_per_blk_chg_hdlr;
  cb_func_tbl_p->max_pdn_conn_time_chg_hdlr =
                                          ds_3gpp_max_pdn_conn_time_chg_hdlr;
  cb_func_tbl_p->pdn_req_wait_time_chg_hdlr =
                                          ds_3gpp_pdn_req_wait_time_chg_hdlr;
#endif /* FEATURE_DATA_LTE */

  cb_func_tbl_p->link.next_ptr = NULL;
  ds_profile_3gpp_register_callback_table(cb_func_tbl_p);

} /*ds_3gpp_profile_hdlr_init*/

/*===========================================================================
FUNCTION DS3GPP_HDLR_INIT

DESCRIPTION
  This function initializes the 3GPP Packet-Switched Data mode-specific
  handlers. It is invoked during DS Task power-up.
  It creates interfaces for each pdn context and initializes the
  parameters for each pdn context.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  Signal mask containing the REX signals that the handlers want to wait on.

SIDE EFFECTS
  None.

===========================================================================*/
rex_sigs_type ds3gpp_hdlr_init( void )
{
  rex_sigs_type requested_sigs = 0;              /* Signal mask to wait on */

  ds_bearer_cntxt_3gpp_init();
  ds_pdn_cntxt_3gpp_init();
  ds_flow_manager_3gpp_init();
  ds_bearer_flow_manager_3gpp_init();
#ifdef FEATURE_DATA_LTE
  ds_3gpp_pdn_throt_apn_rej_sm_init();
#endif
#ifdef FEATURE_DATA_RPM
  ds_3gpp_rpm_init();
#endif /* FEATURE_DATA_RPM */
  ds_3gpp_network_override_hdlr_init();
  ds_3gpp_roaming_hdlr_powerup_init();
  return requested_sigs;
}
/*===========================================================================
FUNCTION DS_3GPP_HDLR_READ_SUBS_ID

DESCRIPTION
  This function ireads the subscription related info for each subs_id on
  device mode change like pdn limit,roaming etc.,

PARAMETERS
  subs_id -subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_hdlr_read_subs_id
(
  sys_modem_as_id_e_type    subs_id
)
{
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;  
  }
  //ds3g_read_ipv6_efs_nv_per_subs_id(subs_id);
  ds_3gpp_nv_manager_init_per_subs_id(subs_id);
#ifdef FEATURE_DATA_LTE
  ds_dsd_apm_init_per_subsid(subs_id);   
  ds_3gpp_pdn_limit_powerup_init(subs_id);
  ds_3gpp_pdn_throt_apn_rej_sm_init_per_subs_id(subs_id);
#endif
#ifdef FEATURE_DATA_RPM
  ds_3gpp_rpm_read_efs_per_subs(subs_id);
#endif
  ds_3gpp_roaming_hdlr_powerup_init_per_subs_id(subs_id);
  ds_3gpp_srv_req_read_throttle_config_from_efs_per_subs_id(subs_id);
  ds_3gpp_pdn_throt_read_config_from_efs_per_subs_id(subs_id);
  ds_3gpp_cc_sp_throt_read_config_from_efs_per_subs_id(subs_id);
  ds_3gppi_utils_read_items_from_efs_per_subs_id(subs_id);
}

/*===========================================================================
FUNCTION ds3gpp_init_all_crit_sect

DESCRIPTION
  This function initializes the critical section used by 3gpp

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void ds3gpp_init_all_crit_sect( void )
{
  ds3gpp_init_global_crit_section();

  /* Initialize 3gpp flow critical section */
  memset(&ds_3gpp_flow_crit_sect, 0, sizeof(rex_crit_sect_type));
  DS_3GPP_INIT_CRIT_SECTION(&ds_3gpp_flow_crit_sect);

  /* Initialize 3gpp throttle SM critical section */
  memset(&ds_3gppi_throttle_sm_crit_sect, 0, sizeof(rex_crit_sect_type));
  DS_3GPP_INIT_CRIT_SECTION(&ds_3gppi_throttle_sm_crit_sect);

}

/*===========================================================================
FUNCTION DS3GPP_INIT

DESCRIPTION
  This function initializes the 3GPP Packet-Switched Data mode-specific
  handlers. It is invoked during DS Task power-up.
  It creates interfaces for each pdn context and initializes the
  parameters for each pdn context.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  Signal mask containing the REX signals that the handlers want to wait on.

SIDE EFFECTS
  None.

===========================================================================*/
rex_sigs_type ds3gpp_init( void )
{
  rex_sigs_type requested_sigs = 0;              /* Signal mask to wait on */
  /*-----------------------------------------------------------------------*/

  /* Register 3GPP MH command processing function with DS task*/
  ds_cmd_process_register(DS_MODULE_3GPP_MH, ds3gpp_process_cmds);

#if defined(FEATURE_GSM_GPRS) || defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_TDSCDMA)
  /*lint -e718 -e746*/
  iphc_powerup_init();
  /*lint +e718 +e746*/
  /*lint -e718 -e746*/
#ifdef FEATURE_DATA_PS_ROHC
  rohc_powerup_init();
#endif /* FEATURE_DATA_PS_ROHC */
  /*lint +e718 +e746*/
#endif /* (FEATURE_GSM_GPRS || FEATURE_DATA_WCDMA_PS || FEATURE_TDSCDMA) */

  ds_3gppi_utils_init();

  requested_sigs |= ds3gpp_info_init();
  requested_sigs |= ds3gpp_hdlr_init();

  ds_3gpp_dyn_mem_hdlr_init();
  ds_3gpp_op_pco_init();
  /*----------------------------------------------------------------------------
    This initialization is needed to create a handle to the Service Request
    throttling SM. This in turn performs the power-up procedures needed for the
    core state machine implementation.
  ----------------------------------------------------------------------------*/
  ds_3gpp_throt_sm_init();

  return requested_sigs;
}

/*===========================================================================
FUNCTION DS3GPP_INIT

DESCRIPTION
  This function does event registrations during powerup for
   interested events.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_hdlr_event_reg()
{
  uint16        client_id_ds3gsubsmgr = 0;
  ds3geventmgr_filter_type       filter_info;
  sys_modem_as_id_e_type         subs_id = SYS_MODEM_AS_ID_NONE;
/* ------------------------------------------------ */

  if (ds3gsubsmgr_event_reg(DS3GSUBSMGR_DDS_CHANGE_EV,
                            ds3gpp_subsmgr_event_cb,
                            NULL, &client_id_ds3gsubsmgr ) == FALSE) 
  {
    DS_3GPP_MSG0_ERROR("Registration for DDS mode EV failed !");
  }
 
  if (ds3gsubsmgr_event_reg(DS3GSUBSMGR_DEVICE_MODE_EV, 
                            ds3gpp_subsmgr_event_cb,
                            NULL, &client_id_ds3gsubsmgr ) == FALSE) 
  {
    DS_3GPP_MSG0_ERROR("Registration for Device mode EV failed !");
  }
  memset(&filter_info, 0, sizeof(ds3geventmgr_filter_type));
  filter_info.tech = PS_SYS_TECH_ALL;

  for(subs_id = SYS_MODEM_AS_ID_1; subs_id < DS3GSUBSMGR_SUBS_ID_COUNT;
                                                                subs_id++)
  {
    filter_info.ps_subs_id = (ps_sys_subscription_enum_type)
              ds3gsubsmgr_subs_id_cm_to_ds((sys_modem_as_id_e_type)subs_id);

    if (ds3geventmgr_event_reg(DS3GEVENTMGR_MODE_LPM_EV,
                               DS3GEVENTMGR_CLIENT_ID_3GPP,
                               &filter_info,
                               ds3gpp_ds3g_event_cb,
                               NULL) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("Registration for MODE LPM EV failed !");
    }

    if (ds3geventmgr_event_reg(DS3GEVENTMGR_SUB_NOT_AVAIL_EV,
                               DS3GEVENTMGR_CLIENT_ID_3GPP,
                               &filter_info,
                               ds3gpp_ds3g_event_cb,
                               NULL) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("Registration for CARD REFRESH EV failed !");
    }

    if(ds3geventmgr_event_reg(DS3GEVENTMGR_NV_REFRESH_EV,
                              DS3GEVENTMGR_CLIENT_ID_3GPP,
                              &filter_info,
                              ds3gpp_ds3g_event_cb,
                              NULL) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("Registration for NV REFRESH EV failed !");
    }

    if(ds3geventmgr_event_reg(DS3GEVENTMGR_THERMAL_MITIGATION_EV,
                              DS3GEVENTMGR_CLIENT_ID_3GPP,
                              &filter_info,
                              ds3gpp_ds3g_event_cb,
                              NULL) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("Registration for THERMAL MITIGATION EV failed !");
    }
    if (ds3geventmgr_event_reg(DS3GEVENTMGR_LATENCY_INFO_EV,
                               DS3GEVENTMGR_CLIENT_ID_3GPP,
                               &filter_info,
                               ds3gpp_ds3g_event_cb,
                               NULL) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("Registration for Latency Info EV failed !");
    }
    if (ds3geventmgr_event_reg(DS3GEVENTMGR_PLMN_LIST_CHG_EV,
                               DS3GEVENTMGR_CLIENT_ID_3GPP,
                               &filter_info,
                               ds3gpp_ds3g_event_cb,
                               NULL) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("Registration for PLMN LIST CHG EV failed !");
    }
  }
  ds3gpp_device_ev_hdlr_init();
} /* ds_3gpp_hdlr_event_reg */

#ifdef TEST_FRAMEWORK
#error code not present
#endif
/*===========================================================================
FUNCTION  DS_3GPP_STOP_SENDING_DATA_AVAIL

DESCRIPTION
 CM suspends DS when there is a voice call on the non PS subscription. If
 CM responds to our data available indication with an err, then we need to
 stop sending any more data available ind to CM and disable flow. This will
 happen unless we are resumed by CM at the end of voice call.

PARAMETERS
  call_instance - Index into call table for the call

DEPENDENCIES
  Called in DS TASK context.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void ds_3gpp_stop_sending_data_avail
(
  const ds_cmd_type            *cmd_ptr
)
{
  sys_modem_as_id_e_type      *subs_id_p = NULL;
  /*---------------------------------------------------------------*/

  /*---------------------------------------------------------------
  Set data_available to False so that we stop sending data avail ind
  to CM
  ------------------------------------------------------------------*/

  if (cmd_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("cmd ptr is NULL");
    return;
  }

  subs_id_p = (sys_modem_as_id_e_type*)cmd_ptr->cmd_payload_ptr;

  if (subs_id_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("subs_id_p is NULL");
    return;
  }

  DS_3GPP_MSG0_MED("ds_3gpp_stop_sending_data_avail");

  ds3gpp_ps_data_fail_ind_hdlr(*subs_id_p);
}

/*===========================================================================

FUNCTION DS3GPP_PROCESS_CMDS

DESCRIPTION 
  This function processes the DS commands 

PARAMETERS
  cmd_ptr   - Ptr to the command information

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3gpp_process_cmds
(
  ds_cmd_type *cmd_ptr
)
{
  if ( cmd_ptr == NULL )
  {
    DATA_ERR_FATAL("null cmd_ptr");
    return;
  }

  switch( cmd_ptr->hdr.cmd_id )
  {
#if ((defined(FEATURE_DATA_WCDMA_CS)) || (defined(FEATURE_DATA_GCSD)))
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
       UMTS CS Data Commands
      - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      case DS_CMD_3GPP_UCSD_HANDOVER:
      case DS_CMD_3GPP_UCSD_APP_ORIG_CALL:
      case DS_CMD_3GPP_UCSD_APP_END_CALL:
      case DS_CMD_3GPP_UCSD_APP_ANSWER_CALL:
      case DS_CMD_3GPP_UCSD_APP_CONFIRM_CALL:
#ifdef FEATURE_MULTIMEDIA_FALLBACK
      #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

#ifdef FEATURE_MMGSDI_SESSION_LIB
      case DS_CMD_3GPP_UCSD_MMGSDI_PROC:
#endif /* FEATURE_MMGSDI_SESSION_LIB */
        ds_ucsd_process_cmds (cmd_ptr);
        break;
#endif /* FEATURE_DATA_WCDMA_CS || FEATURE_DATA_GCSD */

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) ||defined (FEATURE_DATA_LTE)
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      UMTS RMSM Commands
    - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case DS_CMD_3GPP_RMSM_RM_WANTS_PKT_CALL:
    case DS_CMD_3GPP_RMSM_RM_PPP_CONFIGURED:
    case DS_CMD_3GPP_RMSM_RM_BRING_UP_UM_IFACE:
    case DS_CMD_3GPP_RMSM_RM_PPP_UP:
    case DS_CMD_3GPP_RMSM_RM_PPP_DOWN:
    case DS_CMD_3GPP_RMSM_UM_IFACE_UP:
    case DS_CMD_3GPP_RMSM_UM_IFACE_DOWN:
#ifdef FEATURE_DATA_TE_MT_PDP
    case DS_CMD_3GPP_RMSM_UM_WANTS_MT_PKT_CALL:
    case DS_CMD_3GPP_RMSM_RM_REJECT_MT_PKT_CALL:
    case DS_CMD_3GPP_RMSM_UM_LOCKED:
#endif /*FEATURE_DATA_TE_MT_PDP*/
    case DS_CMD_3GPP_RMSM_UM_PPP_DOWN:
    case DS_CMD_3GPP_RMSM_UM_IFACE_DOWN_ARB:
    case DS_CMD_3GPP_RMSM_ENTITLEMENT_CHECK_COMPLETE:
    case DS_CMD_3GPP_RMSM_REDIAL_CHECK_COMPLETE:
    case DS_CMD_3GPP_RMSM_NO_REDIALS_REMAINING:
    case DS_CMD_3GPP_RMSM_IPCP_RSP_TIMER:
    case DS_CMD_3GPP_RMSM_IPV6CP_RSP_TIMER:
    case DS_CMD_3GPP_RMSM_ADDR_CFG_COMPLETE:
    case DS_CMD_3GPP_RMSM_ADDR_CFG_END:
    case DS_CMD_3GPP_RMSM_ADDR_CFG_FAIL:
    case DS_CMD_3GPP_RMSM_ADDR_CFG_CHANGED:
    case DS_CMD_3GPP_RMSM_ENTITLEMENT_CHECK_FAIL:
    case DS_CMD_3GPP_RMSM_DUN_CTRL_RSP:
    case DS_CMD_3GPP_RMSM_RM_IPCP_CREQ_RECD:
    case DS_CMD_3GPP_RMSM_RM_IPV6CP_CREQ_RECD:
    case DS_CMD_3GPP_RMSM_PPPRM_EVENT:
    case DS_CMD_3GPP_RMSM_UM_EVENT:
    case DS_CMD_3GPP_RMSM_UM_PHYS_LINK_EVENT:
      dsumts_rmsm_process_cmd(cmd_ptr);
      break;

    case DS_CMD_3GPP_RMSM_AUTOCONFIG_SM:
      ds_3gpp_rmsm_autoconfig_sm_process_ev_cmd(cmd_ptr);
      break;

    case DS_CMD_3GPP_RMSM_AT_UM_IFACE_EV:
    case DS_CMD_3GPP_RMSM_AT_SEND_DATA:
    case DS_CMD_3GPP_RMSM_AT_FLOW:
      ds_3gpp_rmsm_at_process_cmd(cmd_ptr);
      break;
#endif /* FEATURE_DATA_WCDMA_PS || FEATURE_GSM_GPRS || FEATURE_DATA_LTE */

    case DS_CMD_3GPP_ROAMING_STATUS_CHG:
    case DS_CMD_3GPP_DORM_REORIG:
    case DS_CMD_3GPP_DORM_REORIG_TMR_EXP:
    case DS_CMD_3GPP_DPM_DEREG:
    case DS_CMD_3GPP_IFACE_IN_USE:
    case DS_CMD_3GPP_IFACE_OUT_OF_USE:
    case DS_CMD_3GPP_PHYS_LINK_UP:
    case DS_CMD_3GPP_PHYS_LINK_DOWN:
    case DS_CMD_3GPP_IFACE_UP:
    case DS_CMD_3GPP_IFACE_DOWN:
    case DS_CMD_3GPP_PHYS_LINK_FLOW_ENABLE:
    case DS_CMD_3GPP_PDN_CONN_REQ_RESEND:
    case DS_CMD_3GPP_FLOW_FLTR_ID_CONFLICT:
    case DS_CMD_3GPP_FLOW_FLTR_PRECED_CONFLICT:
    case DS_CMD_3GPP_FLOW_INVOKE_LOWER_LAYER:
    case DS_CMD_3GPP_TLB_MODE_B_HANDLER:
    case DS_CMD_3GPP_PDN_INACTIVITY_TMR_EXP:
    case DS_CMD_3GPP_PDN_POLLING_TMR_EXP:
    case DS_CMD_3GPP_PDN_APN_PARAM_CHG_TMR_EXP:
    case DS_CMD_3GPP_DO_SILENT_REDIAL:
    case DS_CMD_3GPP_BRIDGE_INTERFACE_SETUP:
    case DS_CMD_3GPP_DSM_MEM_CTRL_EV:
    case DS_CMD_3GPP_IFACE_FLOW_ENABLE:
    case DS_CMD_3GPP_IFACE_FLOW_DISABLE:
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      This RMSM command is handled by legacy UMTS MH if present else by
      the unified 3GPP MH.
    - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case DS_CMD_3GPP_SYS_CHG_SS_INFO:
    case DS_CMD_3GPP_IFACE_BRING_UP_RSP:
    case DS_CMD_3GPP_MT_SEND_REJECT:
#ifdef FEATURE_DATA_TE_MT_PDP
    case DS_CMD_3GPP_MT_TE_ANS_TIMEOUT:
#endif /* FEATURE_DATA_TE_MT_PDP */

    case DS_CMD_3GPP_CALL_STOP_DATA_AVAIL_IND:
#ifdef FEATURE_DATA_RPM
    case DS_CMD_3GPP_RPM_LR3_TIMER_EXP:
    case DS_CMD_3GPP_RPM_HOURLY_TIMER_EXP:
    case DS_CMD_3GPP_RPM_MMGSDI_READ_PARAM_CNF:
    case DS_CMD_3GPP_RPM_MMGSDI_READ_PARAM_FAIL:
#endif /* FEATURE_DATA_RPM */
    case DS_CMD_3GPP_MMGSDI_CARD_ERROR:
    case DS_CMD_3GPP_IFACE_TEARDOWN_INVALID_PCSCF:
    case DS_CMD_3GPP_IFACE_TEARDOWN_INVALID_DNS_ADDR:
    case DS_CMD_3GPP_IFACE_TEARDOWN_INVALID_PCSCF_DNS_ADDR:
    case DS_CMD_3GPP_PDN_THROT_CLEAR_APN_REJ_T3402_TMR:
#if defined (FEATURE_DATA_REL10) || defined (FEATURE_DATA_RPM)
    case DS_CMD_3GPP_PDN_THROT_UNBLOCK_APN_TIMER_EXP:
    case DS_CMD_3GPP_PDN_THROT_IMSI_INFO_AVAIL:
#endif /*FEATURE_DATA_REL10 || FEATURE_DATA_RPM*/
#ifdef FEATURE_DATA_REL10
    case DS_CMD_3GPP_PDN_THROT_TIMESTAMP_APN_THROT_TMR:
    case DS_CMD_3GPP_PDN_THROT_UNBLOCK_ALL_APN:
    case DS_CMD_3GPP_PDN_THROT_SAMPLING_TIMER_EXP:
#endif /*FEATURE_DATA_REL10*/
#ifdef FEATURE_DATA_LTE
    case DS_CMD_3GPP_PROFILE_DB_PARAM_CHG:
#endif /* FEATURE_DATA_LTE */
    case DS_CMD_3GPP_RRC_PCH_IND_PROC:
    case DS_CMD_3GPP_IFACE_UP_EMERGENCY:
    case DS_CMD_3GPP_PDN_REMOVE_UNUSED:
    case DS_CMD_3GPP_INIT_AFTER_POWERUP:
    case DS_CMD_3GPP_RLC_PURGE_CNF:
    case DS_CMD_3GPP_CALL_PENDING_CLR:
    case DS_CMD_3GPP_TD_CONFIG_UL_DATA_RATE:
#ifdef FEATURE_DATA_LTE
    case DS_CMD_3GPP_PDN_LIMIT_PDN_CONN_TMR_EXP:
    case DS_CMD_3GPP_PDN_LIMIT_PDN_REQ_THROT_TMR_EXP:
    case DS_CMD_3GPP_PDN_LIMIT_PDN_WAIT_TMR_EXP:
    case DS_CMD_3GPP_PDN_LIMIT_PDN_REQ_TMR_START:
    case DS_CMD_3GPP_PDN_LIMIT_SAMPLING_TIMER_EXP:
    case DS_CMD_3GPP_CALL_END_MODE_CHG_LTE:
#endif /*FEATURE_DATA_LTE*/
    case DS_CMD_3GPP_SYS_SEL_PREF_ERR:
    case DS_CMD_3GPP_SYS_SEL_PREF_NO_ERR:
    case DS_CMD_3GPP_SYS_SEL_RETRY_TIMER_EXP:
    case DS_CMD_3GPP_THROT_TMR_EXP:
    case DS_CMD_3GPP_ROAMING_DISALLOWED_FLAG_CHG:
    case DS_CMD_3GPP_PDN_THROT_RESET:
    case DS_CMD_3GPP_CM_NOTIFY:
    case DS_CMD_3GPP_PROFILE_DB_PDP_TYPE_CHANGE:
    case DS_CMD_3GPP_PROFILE_DB_APN_NAME_CHANGE:
    case DS_CMD_3GPP_PROFILE_DB_APN_DISABLE:
    case DS_CMD_3GPP_NV_REFRESH:
    case DS_CMD_3GPP_MMGSDI_UICC_3G_RESET:
    case DS_CMD_3GPP_THERMAL_MITIGATION:
#ifdef FEATURE_DATA_LTE 
    case DS_CMD_3GPP_PLMN_BLOCK_TMR_EXP_CMD:
    case DS_CMD_3GPP_EPS_THROT_TMR_EXP_CMD:
    case DS_CMD_3GPP_EPS_THROT_T3402_TMR_EXP_CMD:
#endif /*FEATURE_DATA_LTE*/
    case DS_CMD_3GPP_RAU_COMPLTE_CMD:
    case DS_CMD_3GPP_PLMN_LIST_CHG:
      ds3gpp_processPSCmd(cmd_ptr);
      break;

#ifdef FEATURE_DATA_EMBMS
    case DS_CMD_3GPP_EMBMS_IFACE_BRING_UP:
    case DS_CMD_3GPP_EMBMS_SEND_TMGI_ACTIVATED_IND:
    case DS_CMD_3GPP_EMBMS_SEND_TMGI_DEACTIVATED_IND:
      ds_3gpp_embms_process_cmds(cmd_ptr);
      break;
#endif /* FEATURE_DATA_EMBMS */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     GCSD Commands
    - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case DS_CMD_3GPP_GCSD_DISCONNECT:
    case DS_CMD_3GPP_GCSD_RX_DCD_ON:
    case DS_CMD_3GPP_GCSD_TX_DATA_FLUSHED:
#if ((defined(FEATURE_WCDMA) && defined(FEATURE_DATA_WCDMA_CS)) || \
   (defined(FEATURE_GSM) && defined(FEATURE_DATA_GCSD)))
      ds_ucsd_process_cmds (cmd_ptr);
#endif
      break;

#ifdef FEATURE_GSM_GPRS
    case DS_CMD_3GPP_GSNDCP_REG_CNF:
    case DS_CMD_3GPP_GSNDCP_RESUME_CNF:
      ds_gprs_bearer_cntxt_process_gsndcp_ev_cmd(cmd_ptr);
      break;
#endif /* FEATURE_GSM_GPRS */

#if defined (FEATURE_DATA_WLAN_MAPCON) && defined ( FEATURE_DATA_LTE)
    case DS_CMD_3GPP_EPS_KAMGR_SYS_INFO:
    case DS_CMD_3GPP_EPS_KAMGR_IFACE_DOWN_EV_CB:
    case DS_CMD_3GPP_EPS_KAMGR_IFACE_UP_EV_CB:
    case DS_CMD_3GPP_EPS_KAMGR_TIMER_EXPIRY_CB:
    case DS_CMD_3GPP_EPS_KAMGR_IFACE_ALL_UP_EV_CB:
      ds_eps_kamgr_process_cmd(cmd_ptr);
      break;
#endif /*FEATURE_DATA_WLAN_MAPCON && FEATURE_DATA_LTE */

#ifdef FEATURE_DATA_TEST_LOOPBACK
    case DS_CMD_3GPP_LOOPBACK_HDLR:
      ds_loopback_process_ps_cmd (cmd_ptr);
      break;
#endif /*FEATURE_DATA_TEST_LOOPBACK */

    default:
      DS_3GPP_MSG1_HIGH("Unexpected cmd: %d, in ds3gpp_process_cmds, ignored.",
                        cmd_ptr->hdr.cmd_id);
      break;
  }

  return;
} /* ds3gpp_process_cmds() */

/*===========================================================================
FUNCTION  ds_3gpp_hdlr_nv_refresh_cmd

DESCRIPTION
 This functions is invoked a part of nv refresh command per subscription

PARAMETERS
  cmd_ptr

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_hdlr_nv_refresh_cmd
(
  const ds_cmd_type      *cmd_ptr
)
{
  sys_modem_as_id_e_type  subs_id = SYS_MODEM_AS_ID_NONE;
  subs_id = *((sys_modem_as_id_e_type *)cmd_ptr->cmd_payload_ptr);

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }
  DS_3GPP_MSG1_HIGH("DS3GPP Nv Refresh for SUB ID %d",subs_id);

  ds_3gpp_nv_refresh_in_progress[subs_id] = TRUE;
  
  /* Reset network override table */
  ds_3gpp_network_override_hdlr_reset(subs_id);

  /*Reset pdn throttling*/
  ds_3gpp_pdn_throt_reset_pdn_throttle(subs_id);

  /*Reset and initialize nvmanager*/
  ds_3gpp_nv_manager_reset_per_subs_id(subs_id);
  ds_3gpp_nv_manager_init_per_device_nv();
  ds_3gpp_nv_manager_init_per_subs_id(subs_id);

  /*reset and initialize Service request throttling*/
  ds_3gpp_throt_sm_go_null(subs_id,TRUE);
  ds_3gpp_srv_req_throt_init(subs_id);



#ifdef FEATURE_DATA_LTE
  /*read apm per subscription*/
  ds_dsd_apm_init_per_subsid(subs_id);

  /*reset and initialize pdn limit handler*/
  ds_3gpp_pdn_limit_clear_per_subs_id(subs_id);
  ds_3gpp_pdn_limit_powerup_init(subs_id);

  /*reset and initialize apn reject info*/
  ds_3gpp_pdn_throt_apn_rej_sm_refresh_per_subs_id(subs_id);
  ds_3gpp_pdn_throt_apn_rej_sm_init_per_subs_id(subs_id);
#endif
  /*reset and initialize auth manager*/
  ds_3gpp_auth_mgr_nv_refresh_per_subs_info(subs_id);
  ds_3gpp_auth_mgr_per_subs_info_init(subs_id);

#ifdef FEATURE_DATA_RPM
  /*reset and initialize rpm module*/
  ds_3gpp_rpm_process_iccid_chg(subs_id);
  ds_3gpp_rpm_read_efs_per_subs(subs_id);
#endif

  /*reset and initialize roaming module*/
  ds_3gpp_roaming_flush_wait_timer_list();
  ds_3gpp_roaming_hdlr_clear_per_subs_id(subs_id);
  ds_3gpp_roaming_hdlr_powerup_init_per_subs_id(subs_id);

  /*reset and initialize pdn throttling and srv req throttling efs*/
  ds_3gpp_pdn_throt_reset_config_from_efs_per_subs_id(subs_id);
  ds_3gpp_srv_req_throt_reset_config_from_efs_per_subs_id(subs_id);
  ds_3gpp_srv_req_read_throttle_config_from_efs_per_subs_id(subs_id);
  ds_3gpp_pdn_throt_read_config_from_efs_per_subs_id(subs_id);

  /*reset and initialize cc_sp throttling info*/
  ds_3gpp_cc_sp_throt_reset_per_subs_id(subs_id);
  ds_3gpp_cc_sp_throt_read_config_from_efs_per_subs_id(subs_id);

  /*reset and initialize apn disconnect order*/
  ds_3gppi_utils_clear_apn_disconn_order_list_per_subsid(subs_id);
  ds_3gppi_utils_read_items_from_efs_per_subs_id(subs_id);

#ifdef FEATURE_DATA_LTE
  /*clean-up KAMGR global instance variable and Initialize KAMGR*/
  ds_eps_kamgr_dealloc_kamgr_instance(subs_id);
  ds_eps_kamgr_read_efs_info(subs_id);
#endif /* FEATURE_DATA_LTE */

  ds3g_profile_validator_clear_state_per_subs_id(subs_id);

  ds3geventmgr_set_and_notify_nv_refresh_status(DS3GEVENTMGR_CLIENT_ID_3GPP);

  ds_3gpp_nv_refresh_in_progress[subs_id] = FALSE;

  DS_3GPP_MSG1_HIGH("DS3GPP Nv Refresh for SUB ID Done %d",subs_id);

  return;
}
/*===========================================================================
FUNCTION  ds_3gpp_hdlr_nv_refresh_in_progress

DESCRIPTION
 This functions is used to check whether nv refresh is in progress or not.

PARAMETERS
  subs_id - subscription id

DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE- nv refresh in progress
          - FALSE nv refresh in progress.

SIDE EFFECTS
  None.

===========================================================================*/

boolean ds_3gpp_hdlr_nv_refresh_in_progress
(
  sys_modem_as_id_e_type  subs_id
)
{
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return FALSE;
  }
  return ds_3gpp_nv_refresh_in_progress[subs_id];
}

/*===========================================================================

FUNCTION DSUMTSPS_PROCESS_PS_CMD

DESCRIPTION

  This function processes the PS commands received by the DS task.
  The  commands that can be received by the mode specific handler
  are as follows:

  DSUMTSPS_IFACE_UP_CMD: Command to bring up the UMTS Interface

  DSUMTSPS_PHYS_LINK_UP_CMD: Command to bring up the UMTS Traffic Channel

  DSUMTSPS_PHYS_LINK_DOWN_CMD: Command to tear down the UMTS Traffic Channel

  DSUMTSPS_IFACE_DOWN_CMD: Command to tear down the UMTS Interface

  DSUMTSPS_SEND_MT_REJECT_CMD: Command to send MT reject when the MT timer
                               expires

PARAMETERS
  cmd_ptr   - Ptr to the command information

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3gpp_processPSCmd
(
  const ds_cmd_type *cmd_ptr                         /* Pointer to Command */
)
{
  ds_3gpp_pdn_throt_sm_info_type      *throt_sm_info_ptr = NULL;
  ds3g_call_info_cmd_type             *call_info_cmd_ptr = NULL;
#ifdef FEATURE_DATA_LTE
  ds_3gpp_profile_db_param_chg_type   *param_chg_ptr = NULL;
  ds_profile_db_apn_disable_info_type *apn_disable_info_ptr = NULL;
  ds_umts_pdp_profile_type             pdp_profile;
  ds_umts_pdp_profile_status_etype     result;
#endif /* FEATURE_DATA_LTE */
  sys_modem_as_id_e_type               subs_id = SYS_MODEM_AS_ID_NONE;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( cmd_ptr == NULL )
  {
    DATA_ERR_FATAL("PROC_PS_CMD: null cmd_ptr!");
    return;
  }

  /*-------------------------------------------------------------------------
    Invoke the appropriate function to process the command, based on the
    command id.
  -------------------------------------------------------------------------*/
  switch(cmd_ptr->hdr.cmd_id)
  {
    case DS_CMD_3GPP_IFACE_UP:
    case DS_CMD_3GPP_IFACE_DOWN:
    case DS_CMD_3GPP_IFACE_IN_USE:
    case DS_CMD_3GPP_IFACE_OUT_OF_USE:
    //case DSUMTSPS_SEND_MT_REJECT_CMD:
    case DS_CMD_3GPP_IFACE_BRING_UP_RSP:
    case DS_CMD_3GPP_MT_SEND_REJECT:
  #ifdef FEATURE_DATA_TE_MT_PDP
    case DS_CMD_3GPP_MT_TE_ANS_TIMEOUT:
  #endif /* FEATURE_DATA_TE_MT_PDP */
    case DS_CMD_3GPP_SYS_CHG_SS_INFO:
    case DS_CMD_3GPP_PDN_CONN_REQ_RESEND:
    case DS_CMD_3GPP_PDN_INACTIVITY_TMR_EXP:
    case DS_CMD_3GPP_PDN_POLLING_TMR_EXP:
    case DS_CMD_3GPP_PDN_APN_PARAM_CHG_TMR_EXP:
    case DS_CMD_3GPP_DO_SILENT_REDIAL:
    case DS_CMD_3GPP_BRIDGE_INTERFACE_SETUP:
    case DS_CMD_3GPP_MMGSDI_CARD_ERROR:
    case DS_CMD_3GPP_IFACE_TEARDOWN_INVALID_PCSCF:
    case DS_CMD_3GPP_IFACE_TEARDOWN_INVALID_DNS_ADDR:
    case DS_CMD_3GPP_IFACE_TEARDOWN_INVALID_PCSCF_DNS_ADDR:
    case DS_CMD_3GPP_PDN_REMOVE_UNUSED:
  #ifdef FEATURE_EMERGENCY_PDN
    case DS_CMD_3GPP_IFACE_UP_EMERGENCY:
  #endif /* FEATURE_EMERGENCY_PDN */
    case DS_CMD_3GPP_ROAMING_STATUS_CHG:
    case DS_CMD_3GPP_PDN_THROT_RESET:
    case DS_CMD_3GPP_CM_NOTIFY:
    case DS_CMD_3GPP_MMGSDI_UICC_3G_RESET:
    case DS_CMD_3GPP_THERMAL_MITIGATION:
      ds_pdn_cntxt_process_ps_cmd(cmd_ptr);
      break;

    case DS_CMD_3GPP_PHYS_LINK_UP:
    case DS_CMD_3GPP_PHYS_LINK_DOWN:
    case DS_CMD_3GPP_PHYS_LINK_FLOW_ENABLE:
    case DS_CMD_3GPP_DORM_REORIG_TMR_EXP:
    case DS_CMD_3GPP_DORM_REORIG:
    case DS_CMD_3GPP_DPM_DEREG:
    case DS_CMD_3GPP_DSM_MEM_CTRL_EV:
    case DS_CMD_3GPP_RRC_PCH_IND_PROC:
    case DS_CMD_3GPP_RLC_PURGE_CNF:
    case DS_CMD_3GPP_CALL_PENDING_CLR:
    case DS_CMD_3GPP_TD_CONFIG_UL_DATA_RATE:
#ifdef FEATURE_DATA_LTE
    case DS_CMD_3GPP_CALL_END_MODE_CHG_LTE:
#endif /* FEATURE_DATA_LTE */
      ds_bearer_cntxt_process_ps_cmd(cmd_ptr);
      break;

    case DS_CMD_3GPP_FLOW_FLTR_ID_CONFLICT:
    case DS_CMD_3GPP_FLOW_FLTR_PRECED_CONFLICT:
    case DS_CMD_3GPP_FLOW_INVOKE_LOWER_LAYER:
      ds_flow_manager_process_ps_cmd(cmd_ptr);
      break;

    case DS_CMD_3GPP_IFACE_FLOW_ENABLE:
    case DS_CMD_3GPP_IFACE_FLOW_DISABLE:
      dsumts_rmsmi_iface_flow_cmd_hdlr ( cmd_ptr );
      break;

#ifdef FEATURE_DATA_LTE
    case DS_CMD_3GPP_TLB_MODE_B_HANDLER:
      dsEpsTlb_modeB_hdlr_cmd(cmd_ptr);
      break;
#endif /* FEATURE_DATA_LTE */

    case DS_CMD_3GPP_CALL_STOP_DATA_AVAIL_IND:
      ds_3gpp_stop_sending_data_avail( cmd_ptr );
      break;

#ifdef FEATURE_DATA_RPM

    case DS_CMD_3GPP_RPM_LR3_TIMER_EXP:
      ds_3gpp_rpm_lr3_tmr_exp_hdlr(cmd_ptr);
      break;

    case DS_CMD_3GPP_RPM_HOURLY_TIMER_EXP:
      ds_3gpp_rpm_hourly_tmr_exp_hdlr(cmd_ptr);
      break;

    case DS_CMD_3GPP_RPM_MMGSDI_READ_PARAM_CNF:
      ds_3gpp_rpm_mmgsdi_read_resp_cmd_hdlr(cmd_ptr);
      break;

    case DS_CMD_3GPP_RPM_MMGSDI_READ_PARAM_FAIL:
      ds_3gpp_rpm_mgsdi_read_fail_cmd_hdlr(cmd_ptr);
      break;

#endif /* FEATURE_DATA_RPM */

    case DS_CMD_3GPP_PDN_THROT_UNBLOCK_APN_TIMER_EXP:
      if(cmd_ptr->cmd_payload_ptr != NULL)
      {
        throt_sm_info_ptr = 
          (ds_3gpp_pdn_throt_sm_info_type*)cmd_ptr->cmd_payload_ptr;

        ds_3gpp_pdn_throt_unblock_apn_timer_exp_cmd_hdlr
          ((void*)(throt_sm_info_ptr->throt_sm_ptr),
           throt_sm_info_ptr->subs_id);
      }
      break;

#ifdef FEATURE_DATA_REL10
    case DS_CMD_3GPP_PDN_THROT_TIMESTAMP_APN_THROT_TMR:
      if(cmd_ptr->cmd_payload_ptr != NULL)
      {
        throt_sm_info_ptr = 
          (ds_3gpp_pdn_throt_sm_info_type*)cmd_ptr->cmd_payload_ptr;

        ds_3gpp_pdn_throt_timestamp_apn_throt_tmr_cmd_hdlr
          ((void*)(throt_sm_info_ptr->throt_sm_ptr),
           throt_sm_info_ptr->subs_id);
      }
      break;

    case DS_CMD_3GPP_PDN_THROT_IMSI_INFO_AVAIL:
      if(cmd_ptr->cmd_payload_ptr != NULL)
      {
        subs_id = ((sys_modem_as_id_e_type)
               (*((sys_modem_as_id_e_type*)cmd_ptr->cmd_payload_ptr)));
        ds_3gpp_pdn_throt_imsi_info_avail_cmd_hdlr(subs_id);
#ifdef FEATURE_DATA_LTE
        if(ds_3gpp_pdn_limit_is_enabled(subs_id))
        {
          ds_3gpp_pdn_limit_imsi_info_avail_cmd_hdlr
              (subs_id);
        }
#endif
      }
      break;

    case DS_CMD_3GPP_PDN_THROT_UNBLOCK_ALL_APN:
      if(cmd_ptr->cmd_payload_ptr != NULL)
      {
        call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

        ds_3gpp_pdn_throt_unblock_all_globally_blked_apn
          (call_info_cmd_ptr->subs_id);
      }
      break;

    case DS_CMD_3GPP_PDN_THROT_SAMPLING_TIMER_EXP:
      if(cmd_ptr->cmd_payload_ptr != NULL)
      {
        ds_3gpp_pdn_throt_sampling_timer_exp_cmd_hdlr
          ((sys_modem_as_id_e_type)
           (*((sys_modem_as_id_e_type*)cmd_ptr->cmd_payload_ptr)));
      }
      break;

#endif /*FEATURE_DATA_REL10*/
    case DS_CMD_3GPP_INIT_AFTER_POWERUP:
      ds_3gppi_utils_read_items_from_efs();
#if defined (FEATURE_DATA_WLAN_MAPCON) && defined (FEATURE_DATA_LTE) 
      ds_eps_kamgr_read_efs_info(ds3g_get_ps_subs_id());
#endif

      ds3g_profile_validator_powerup_init();

      ds_3gpp_hdlr_event_reg();

      ds3gpp_device_ev_hdlr_init_after_powerup();
      ds_3gpp_pdn_context_per_subs_info_init(ds3g_get_ps_subs_id());
      break;

#ifdef FEATURE_DATA_LTE
    case DS_CMD_3GPP_PDN_THROT_CLEAR_APN_REJ_T3402_TMR:
      if(cmd_ptr->cmd_payload_ptr != NULL)
      {
        ds_3gpp_pdn_throt_apn_reject_t3402_timer_expiry_cmd_hdlr 
                                     ((sys_modem_as_id_e_type)(*((sys_modem_as_id_e_type*)
                                                                 cmd_ptr->cmd_payload_ptr)));
      }
      break;

    case DS_CMD_3GPP_PROFILE_DB_PARAM_CHG:
      if(cmd_ptr->cmd_payload_ptr != NULL)
      {
        param_chg_ptr = 
          (ds_3gpp_profile_db_param_chg_type*)cmd_ptr->cmd_payload_ptr;

        if(param_chg_ptr->mask & DS_3GPP_PROFILE_APN_PARAM_CHG)
        {

          ds_3gpp_apn_param_change_cmd_hdlr(param_chg_ptr->profile_num,
                                     (sys_modem_as_id_e_type)
                                     param_chg_ptr->subs_id,
                                     param_chg_ptr->user_data);
        }

        if((param_chg_ptr->mask & DS_3GPP_PROFILE_CLR_THROT) &&
            ds_3gpp_nv_manager_get_global_throttling
             ((sys_modem_as_id_e_type)param_chg_ptr->subs_id))
        { 
          result = ds_umts_get_pdp_profile_all_data_per_subs(
                                     param_chg_ptr->profile_num, 
                                     dsumts_subs_mgr_get_subs_id
                                     ((sys_modem_as_id_e_type)
                                      param_chg_ptr->subs_id), 
                                     &pdp_profile);

          if( result != DS_UMTS_PDP_SUCCESS)
          {
             DS_3GPP_MSG1_ERROR("Error in retrieving definition for prof number:%d",
                         param_chg_ptr->profile_num);
          }
          else
          {
              
            ds_3gpp_pdn_cntxt_validate_auth_params_and_reset_t3396_throt(
                                    &pdp_profile,
                                     NULL,
                                    (sys_modem_as_id_e_type)param_chg_ptr->subs_id);
          }
        }
      }
      break;

    case DS_CMD_3GPP_PDN_LIMIT_PDN_CONN_TMR_EXP:
    case DS_CMD_3GPP_PDN_LIMIT_PDN_REQ_THROT_TMR_EXP:
    case DS_CMD_3GPP_PDN_LIMIT_PDN_WAIT_TMR_EXP:
    case DS_CMD_3GPP_PDN_LIMIT_PDN_REQ_TMR_START:
    case DS_CMD_3GPP_PDN_LIMIT_SAMPLING_TIMER_EXP:
      ds_3gpp_pdn_limit_process_cmds(cmd_ptr);
      break;

#endif

    case DS_CMD_3GPP_SYS_SEL_PREF_ERR:
      ds_3gpp_throt_sm_sys_sel_pref_err_cmd_hdlr();
      break;

    case DS_CMD_3GPP_SYS_SEL_PREF_NO_ERR:
      ds_3gpp_throt_sm_sys_sel_pref_no_err_cmd_hdlr();
 #ifdef FEATURE_DATA_LTE
      ds_eps_pdn_cntxt_sys_sel_pref_no_err_cmd_hdlr
         (cmd_ptr->cmd_payload_ptr);
 #endif /* FEATURE_DATA_LTE */
      break;

    case DS_CMD_3GPP_SYS_SEL_RETRY_TIMER_EXP:
      ds_3gpp_throt_sys_sel_retry_timer_expiry_cmd_hdlr();
      break;

    case DS_CMD_3GPP_THROT_TMR_EXP:
#ifdef FEATURE_DATA_LTE 
    case DS_CMD_3GPP_PLMN_BLOCK_TMR_EXP_CMD:
    case DS_CMD_3GPP_EPS_THROT_TMR_EXP_CMD:
    case DS_CMD_3GPP_EPS_THROT_T3402_TMR_EXP_CMD:
 #endif /* FEATURE_DATA_LTE */
   case DS_CMD_3GPP_RAU_COMPLTE_CMD:
      ds_3gpp_throt_sm_process_cmds(cmd_ptr);
      break;

    case DS_CMD_3GPP_PLMN_LIST_CHG:
    case DS_CMD_3GPP_ROAMING_DISALLOWED_FLAG_CHG:
      ds_3gpp_roaming_hdlr_process_cmds(cmd_ptr);
      break;

     case DS_CMD_3GPP_NV_REFRESH:
       ds_3gpp_hdlr_nv_refresh_cmd(cmd_ptr);
     break;

    case DS_CMD_3GPP_PROFILE_DB_PDP_TYPE_CHANGE:
    {
      ds_3gpp_pdn_cntx_pdp_type_chg_cmd_hdlr(cmd_ptr);
      break;
    }

    case DS_CMD_3GPP_PROFILE_DB_APN_NAME_CHANGE:
    {
      ds_3gpp_pdn_cntx_apn_name_chg_cmd_hdlr(cmd_ptr);
      break;
    }

#ifdef FEATURE_DATA_LTE
    case DS_CMD_3GPP_PROFILE_DB_APN_DISABLE:
      if(cmd_ptr->cmd_payload_ptr != NULL)
      {
        apn_disable_info_ptr = 
          (ds_profile_db_apn_disable_info_type*)cmd_ptr->cmd_payload_ptr;

        ds_3gpp_process_apn_disabled_cmd
          (apn_disable_info_ptr->profile_num,          
           apn_disable_info_ptr->apn_disable_flag,
           apn_disable_info_ptr->subs_id);
      }
      break;
#endif /* FEATURE_DATA_LTE */
         
    default:
      DS_3GPP_MSG1_HIGH("Unexpected cmd: %d, in ds3gpp_processPSCmd, ignored.",
               cmd_ptr->hdr.cmd_id);
      break;
  } /* switch( cmd_id ) */

} /* dsumtsps_process_ps_cmd() */

/*===========================================================================
FUNCTION DS_3GPP_HDLR_BRING_DOWN_PHYS_LINK_HDLR

DESCRIPTION
  This function brings down the phys link of all the active bearers for the
  given subscription

PARAMETERS
  subs_id - Subscription id

DEPENDENCIES
  None.

RETURN VALUE
  uint32 - call id.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_hdlr_bring_down_phys_link_hdlr
(
  sys_modem_as_id_e_type  subs_id
)
{
  int index = 0;
  ds_bearer_context_s     *bearer_context_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (index = 0; index < DS_3GPP_MAX_BEARER_CONTEXT; index++)
  {
    bearer_context_p = ds_bearer_cntxt_get_instance_ptr(index);
    if(ds_bearer_cntxt_validate_bearer_subscription (bearer_context_p, subs_id))
    {
      if( (bearer_context_p->ds_bearer_context_dyn_p)->state
                                       == DS_BEARER_CONTEXT_STATE_UP)
      {     
        ps_phys_link_down_ind_ex( &(bearer_context_p->phys_link), 
			          PS_EIC_QOS_NETWORK_L2_LINK_RELEASED );
        /*-------------------------------------------------------------------
         Register each enqueue function ptr.      
        -------------------------------------------------------------------*/
        (bearer_context_p->tx_wm.wm_item).
          each_enqueue_func_ptr =(wm_cb_type)ds_bearer_cntxt_reorig_dorm_call_cb;
        (bearer_context_p->tx_wm.wm_item).
          each_enqueue_func_data =(void*) bearer_context_p;
      }
    }
  }
} /* ds_3gpp_hdlr_bring_down_phys_link_hdlr */

#ifdef FEATURE_QXDM_WM_TRACING
/*===========================================================================
FUNCTION DS_BEARER_CNTXT_GET_CALL_ID

DESCRIPTION
  This function gives the call id.

PARAMETERS
  bearer_context_p - bearer context pointer

DEPENDENCIES
  None.

RETURN VALUE
  uint32 - call id.

SIDE EFFECTS
  None.

===========================================================================*/
uint32 ds_bearer_cntxt_get_call_id
(
  ds_bearer_context_s* bearer_context_p
)
{
  uint32 group_id = 0;
  uint32 rab_id = 0;

  /*-------------------------------------------------------------------------
    Make sure bearer context ptr is not NULL.
  -------------------------------------------------------------------------*/
  if (bearer_context_p == NULL)
  {
    DATA_ERR_FATAL("NULL bearer context pointer for WM tracing.");
  }

  /*-------------------------------------------------------------------------
    Get the group id and nsapi/rab_id from the bearer context.
  -------------------------------------------------------------------------*/
  group_id = (uint16)(((ds_pdn_context_s*)\
                     (bearer_context_p->pdn_context_ptr))->pdp_group_id);
  rab_id   = (uint16)(bearer_context_p->nsapi);

  return ((uint32)(rab_id << 16) + group_id);
} /* ds_bearer_cntxt_get_call_id */

/*===========================================================================

FUNCTION DS3GPP_REPORT_WM_STATS_CB

DESCRIPTION
  This function internally call bearer context function to report wm status.

PARAMETERS
  bearer_instance - bearer context instance
  wm_stat_ptr     - pointer where we store the wm values

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3gpp_report_wm_stats_cb
(
  uint32          bearer_instance,
  fc_wm_stat_type *wm_stat_ptr
)
{
  if( wm_stat_ptr == NULL)
    return;

  ds_bearer_cntxt_report_wm_stats(bearer_instance, wm_stat_ptr);
} /* ds3gpp_report_wm_stats_cb */

#endif /* FEATURE_QXDM_WM_TRACING */


/*===========================================================================
FUNCTION DS3GPP_GET_NSAPI_FROM_APN

DESCRIPTION
  This function returns the nsapi values associated with given apn value.
  Currently we are giving, nsapi of all bearers.

PARAMETERS
  apn         - access point name value
  nsapi_table            - pointer where we store nsapi values
  ps_error               - incase any error occured, details of it

DEPENDENCIES
  None.

RETURN VALUE
  0  - Success
  -1 - Failure
       DS_EFAULT   if NULL pointers passed.
       DS_ENETDOWN if PDN contexts are not valid.

SIDE EFFECTS
  None.
===========================================================================*/
int ds3gpp_get_nsapi_from_apn
(
  void                    *arg_val_ptr,
  sys_modem_as_id_e_type   subs_id,
  int16                   *ps_errno
)
{
  ps_sys_ioctl_3gpp_nsapi_type* nsapi_info_p = NULL;
  ds_pdn_context_s     *pdn_context_ptr = NULL;
  ds_bearer_context_s  *bearer_context_ptr = NULL;
  byte                 *temp_apn = NULL;
  uint8                nsapi_count = 0, temp_index = 0;
  /*-----------------------------------------------------------------------*/

  if( ps_errno == NULL )
  {
    DS_3GPP_MSG0_ERROR("ps_errno is NULL");
    return -1;
  }

  if (arg_val_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("arg_val_ptr is NULL");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  nsapi_info_p = (ps_sys_ioctl_3gpp_nsapi_type*)arg_val_ptr;

  temp_apn = (byte*)nsapi_info_p->apn;

  /*-----------------------------------------------------------------------------
    For a given APN, store all NSAPIs.
    First try to get the v4 pdn context, if it succeed we will go ahead and
    compute the NSAPIs. Otherwise get the v6 pdn context and same process.
    Getting both v4 and v6 PDN contexts failed means apn is not a valid one.
  -----------------------------------------------------------------------------*/
  if( (ds_pdn_cntxt_get_v4_pdn_context(temp_apn, 
                                       &pdn_context_ptr,
                                       subs_id
                                       ) == DS3G_SUCCESS )   ||
      ( pdn_context_ptr != NULL ) )
  {
    /*-----------------------------------------------------------------------------
     Ensure the pdn_context is valid.
    -----------------------------------------------------------------------------*/
    if((ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_ptr)))
    {
      /*-----------------------------------------------------------------------------
        Every PDN context can have maximum 8 bearer contexts associated with it.
        Check for every bearer context for validity.
        If a valid bearer context is found, store the NSAPI.
      -----------------------------------------------------------------------------*/
      for( temp_index = 0;
           temp_index < DS_3GPP_MAX_BEARER_CONTEXT_PER_PDN_CONTEXT;
           temp_index++ )
      {
        bearer_context_ptr =
                (ds_bearer_context_s*)(pdn_context_ptr->ds_pdn_context_dyn_p->
                                       bearer_context_ptr_tbl[temp_index]);

        if(ds_bearer_cntx_validate_bearer_context_internal(bearer_context_ptr))
        {
          nsapi_info_p->nsapi[nsapi_count] = 
            bearer_context_ptr->ds_bearer_context_dyn_p->nsapi;
          nsapi_count++;
        }
      }
    }
    else
    {
      DS_3GPP_MSG0_ERROR("PDN context is not in use.(v4)");
      *ps_errno = DS_ENETDOWN;
      return -1;
    }
  }
  else if( ( ds_pdn_cntxt_get_v6_pdn_context(temp_apn, 
                                             &pdn_context_ptr,
                                             subs_id
                                             ) == DS3G_SUCCESS )   ||
           ( pdn_context_ptr != NULL ))
  {
    /*-----------------------------------------------------------------------------
      Check if in_use flag is set to TRUE.
      If in_use flag is FALSE PDN context is not valid.
    -----------------------------------------------------------------------------*/
    if(ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_ptr))
    {
      /*-----------------------------------------------------------------------------
        Every PDN context can have maximum 8 bearer contexts associated with it.
        Check for every bearer context for validity.
        If a valid bearer context is found, store the NSAPI.
      -----------------------------------------------------------------------------*/
      for( temp_index = 0;
           temp_index < DS_3GPP_MAX_BEARER_CONTEXT_PER_PDN_CONTEXT;
           temp_index++ )
      {
      bearer_context_ptr =
                (ds_bearer_context_s*)(pdn_context_ptr->ds_pdn_context_dyn_p->
                                       bearer_context_ptr_tbl[temp_index]);
        if(ds_bearer_cntx_validate_bearer_context_internal(bearer_context_ptr))
        {
          nsapi_info_p->nsapi[nsapi_count] = 
            bearer_context_ptr->ds_bearer_context_dyn_p->nsapi;
          nsapi_count++;
        }
      }
    }
    else
    {
      DS_3GPP_MSG0_ERROR("PDN context is not in use.(v6)");
      *ps_errno = DS_ENETDOWN;
      return -1;
    }
  }
  else
  {
    DS_3GPP_MSG0_ERROR("APN is not matched in any pdn context either v4 or v6");
    *ps_errno = DS_ENETDOWN;
    return -1;
  }

  nsapi_info_p->num_nsapi = nsapi_count;
  for( temp_index = 0; temp_index < nsapi_count; temp_index++ )
  {
    DS_3GPP_MSG1_MED("bearer_id = %d", nsapi_info_p->nsapi[temp_index]);
  }
  return 0;

} /* ds3gpp_get_nsapi_from_apn */

#ifdef FEATURE_DATA_LTE
/*===========================================================================
FUNCTION DS_3GPP_BSR_RESET_HANDLER

DESCRIPTION
  This handler triggers tearing down all PDN connections without active
  clients when the Better system Reslection(BSR) computation is not in
  progress. Tearing down PDN connections will be triggered only when the
  current sys mode is not 3GPP specific mode
PARAMETERS
  None
DEPENDENCIES
  None.

RETURN VALUE
  None
SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_bsr_reset_handler
(
  sys_modem_as_id_e_type  subs_id
)
{
  sys_sys_mode_e_type preferred_mode = SYS_SYS_MODE_NONE;
  boolean bsr_status = TRUE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  preferred_mode  = ds3gdsdif_get_preferred_sys(
                            ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id));
  bsr_status    = ds3g_bsr_is_in_progress(subs_id);

  DS_3GPP_MSG2_MED("Current Pref mode %d Is BSR in Progress? %d",
                      preferred_mode,bsr_status);

  if(bsr_status == FALSE)
  {
    /*--------------------------------------------------------------
       Tear down PDN connections if Preferred mode  is not W,G,L and
       if LTE is not the target rat during handoff
      --------------------------------------------------------------*/

    if ( (ds3g_get_target_rat_ex(subs_id) != SYS_SYS_MODE_LTE) &&
          !(DS_3GPP_IS_3GPP_CURRENT_MODE(preferred_mode)) &&
         (ds_bearer_cntxt_bearers_on_lte(subs_id) == TRUE)
        )
    {
      DS_3GPP_MSG0_MED("Tearing down PDN Connections");
      ds_3gpp_pdn_cntxt_tear_down(subs_id);
    }
  }
  else
  {
    DS_3GPP_MSG0_MED("BSR compuation in progress.Do not teardown"
                     "PDN connections");
  }
} /* ds_3gpp_bsr_reset_handler */
#endif /* FEATURE_DATA_LTE */

/*===========================================================================
FUNCTION DS_3GPP_PLMN_CHANGE_HANDLER

DESCRIPTION
  This Handler is triggered while PLMN  changes. this is to reset the 
  network override iptype table for cause code 50 and 51 throttling

PARAMETERS
  old_plmn_id - Old PLMN id
  subs_id     - Subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_plmn_change_hdlr
(
  sys_plmn_id_s_type	  old_plmn_id,
  sys_modem_as_id_e_type  subs_id
)
{
  sys_plmn_id_s_type new_plmn_id;
  /*----------------------------------------------------------------------*/

  memset(&new_plmn_id, 0, sizeof(sys_plmn_id_s_type));

  ds3g_get_current_plmn_id(&new_plmn_id, subs_id);
  
  if (FALSE == ds_3gpp_throt_sm_is_plmn_non_null(new_plmn_id))
  {
    DS_3GPP_MSG0_HIGH("No need to process PLMN change as new PLMN is NULL");
    return;
  }

  /* Reset network override table */
  ds_3gpp_network_override_hdlr_reset(subs_id);
  if(ds_3gpp_throt_sm_is_cc_sp_throttling_enabled(subs_id) == TRUE)
  {
    ds_3gpp_pdn_throt_reset_pdn_throttle(subs_id);
  }

  if(ds_3gppi_is_rel_11_supported())
  {
     /*-------------------------------------------------------------------
     Reset the permenant throttle after plmn change and also reset the 
     max simultanous pdp context to DS_3GPP_MAX_PDN_CONTEXT 
     ------------------------------------------------------------------*/
    ds_3gpp_pdn_throt_reset_pdn_throttle_if_required(subs_id); 
    ds_3gpp_pdn_set_max_simul_active_pdp_context(DS_3GPP_MAX_BEARER_CONTEXT, subs_id);
  }

  if (ds_3gpp_is_rpm_enabled(subs_id))
  {
    ds_3gpp_rpm_plmn_id_chg_hdlr(old_plmn_id, subs_id);
  }

  ds_3gpp_roaming_inform_new_plmn_info(subs_id, new_plmn_id);
  
} /* ds_3gpp_plmn_change_hdlr */

/*===========================================================================
FUNCTION DS_3GPP_MODE_PREF_CHANGE_HDLR

DESCRIPTION
  This Handler is triggered while mode_pref  changes. This is required when 
  IRAT happens due to mode change.

PARAMETERS
  subs_id       -- Subscription ID where mode_pref_change occurred.
  new_pref_mode -- New preferred mode after mode_pref_change.
  old_pref_mode -- Old preferred mode abefore mode_pref_change.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_mode_pref_change_hdlr
(
  sys_modem_as_id_e_type  subs_id,
  sys_sys_mode_e_type     new_pref_mode,
  sys_sys_mode_e_type     old_pref_mode
)
{
 if (!DS_3GPP_IS_3GPP_CURRENT_MODE(new_pref_mode))
 {
   DS_3GPP_MSG1_HIGH("Non-3gpp mode in mode_pref_change: %d, ignoring",
                     new_pref_mode);
   return;
 }

 /* Invoke mode_pref_change_hdlr only if new_pref_mode is not same as old_pref_mode */
 if (new_pref_mode != old_pref_mode)
 {
   ds_bearer_cntxt_mode_pref_change(subs_id);
 }

} /* ds_3gpp_mode_pref_change_hdlr */

/*===========================================================================
FUNCTION DS_3GPP_SET_CNTXT_TRANSFER_INFO

DESCRIPTION
  This function is used to set ds_3gpp_cntxt_transfer_info parameter with
  target rat and cntxt_transfer_flag

PARAMETERS
  sys_sys_mode_e_type : Target RAT
  Boolean             : cntxt_transfer_flag
  subs_id             : Subscription ID

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_set_cntxt_transfer_info
(
  sys_sys_mode_e_type     target_rat,
  boolean                 cntxt_tranfer_is_recvd,
  sys_modem_as_id_e_type  subs_id
)
{
  ds3gpp_per_subs_info_type* hndl = NULL;
  /*----------------------------------------------------------------------*/

  hndl = ds3gpp_get_per_subs_info_hndl(subs_id);

  if (ds3gpp_validate_per_subs_info_hndl(hndl))
  {
    hndl->cntxt_transfer_info.target_rat  = target_rat;
    hndl->cntxt_transfer_info.cntxt_tranfer_is_recvd = cntxt_tranfer_is_recvd;

    DS_3GPP_MSG3_HIGH("Context transfer Target Rat is %d and cntxt transfer "
                      "flag is %d, Subs Id(CM): %d", 
                      target_rat, cntxt_tranfer_is_recvd, subs_id);
  }
  return;
} /* ds_3gpp_set_cntxt_transfer_info */

/*===========================================================================
FUNCTION DS_3GPP_GET_TARGET_RAT_FROM_CNTXT_TRANSFER_INFO

DESCRIPTION
  This function is used to get ds_3gpp_cntxt_transfer_info parameter with
  target rat

PARAMETERS
  subs_id: Subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
sys_sys_mode_e_type ds_3gpp_get_target_rat_from_cntxt_transfer_info
(
  sys_modem_as_id_e_type  subs_id
)
{
  sys_sys_mode_e_type         target_rat = SYS_SYS_MODE_NONE;
  ds3gpp_per_subs_info_type  *hndl = NULL;
  /*----------------------------------------------------------------------*/

  hndl = ds3gpp_get_per_subs_info_hndl(subs_id);

  if (ds3gpp_validate_per_subs_info_hndl(hndl))
  {
    target_rat = hndl->cntxt_transfer_info.target_rat;
    DS_3GPP_MSG2_LOW("Context Transfer Info Target RAT for Subs Id(CM):"
                     "%d is %d", subs_id, target_rat);
  }

  return target_rat;
} /* ds_3gpp_get_target_rat_from_cntxt_transfer_info */

/*===========================================================================
FUNCTION DS_3GPP_GET_CNTXT_TRANSFER_IND_IS_RECVD

DESCRIPTION
  This function is used to get ds_3gpp_cntxt_transfer_info parameter with
  target rat

PARAMETERS
  subs_id: Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if Context Transfer Indication was received
  FALSE, otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_3gpp_get_cntxt_transfer_ind_is_recvd
(
    sys_modem_as_id_e_type  subs_id
)
{
  boolean                    flag = FALSE;
  ds3gpp_per_subs_info_type* hndl = NULL;
  /*----------------------------------------------------------------------*/

  hndl = ds3gpp_get_per_subs_info_hndl(subs_id);

  if (ds3gpp_validate_per_subs_info_hndl(hndl))
  {
    flag = hndl->cntxt_transfer_info.cntxt_tranfer_is_recvd;
    DS_3GPP_MSG2_MED("Context Transfer Info received: %d for Subs Id(CM): %d",
                      flag, subs_id);
  }

  return flag;
} /* ds_3gpp_get_cntxt_transfer_ind_is_recvd */

/*===========================================================================
FUNCTION DS3GPP_GET_PER_SUBS_INFO_HNDL
 
DESCRIPTION
  This function returns a handle to subscription-specific information in 
  ds_3gpp_hdlr.c
 
PARAMETERS 
  Subscription id

DEPENDENCIES 
  None

RETURN VALUE 
  Handle to subscription-specific information.
 
SIDE EFFECTS 
===========================================================================*/
ds3gpp_per_subs_info_type* ds3gpp_get_per_subs_info_hndl
(
  sys_modem_as_id_e_type subs_id
)
{
  ds3gpp_per_subs_info_type *hndl = NULL;
  /*----------------------------------------------------------*/

  if (ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    hndl = ds3gpp_info.per_subs_info_p[subs_id];
  }

  return hndl;
} /* ds3gpp_get_per_subs_info_hndl */

/*===========================================================================
FUNCTION DS3GPP_SET_PER_SUBS_INFO_HNDL
 
DESCRIPTION
  This function sets the handle to subscription-specific information in 
  ds_3gpp_hdlr.c
 
PARAMETERS 
  1. Subscription id
  2. Handle to Subscription Specific Information

DEPENDENCIES 
  None

RETURN VALUE 
  None
 
SIDE EFFECTS 
===========================================================================*/
static void ds3gpp_set_per_subs_info_hndl
(
  sys_modem_as_id_e_type     subs_id,
  ds3gpp_per_subs_info_type  *hndl
)
{
  /*----------------------------------------------------------*/
  if (ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    ds3gpp_info.per_subs_info_p[subs_id] = hndl;
  }
} /* ds3gpp_set_per_subs_info_hndl */

/*===========================================================================
FUNCTION DS3GPP_VALIDATE_PER_SUBS_INFO_HNDL
 
DESCRIPTION
  This function validates the handle to subscription-specific information in 
  ds_3gpp_hdlr.c
 
PARAMETERS 
  1. Handle to Subscription Specific Information

DEPENDENCIES 
  None

RETURN VALUE 
  TRUE, if the handle is VALID
  FALSE, if invalid.
 
SIDE EFFECTS 
===========================================================================*/
boolean ds3gpp_validate_per_subs_info_hndl
(
  ds3gpp_per_subs_info_type *hndl
)
{
  boolean ret_val = FALSE;
  /*----------------------------------------------------------*/

  if (hndl != NULL && hndl->cookie == DS3GPP_PER_SUBS_INFO_COOKIE)
  {
    ret_val = TRUE;
  }
  else
  {
    DS_3GPP_MSG1_ERROR("Subs info hndl:0x%x is not valid",hndl);
  }
  
  return ret_val;
} /* ds3gpp_validate_per_subs_info_hndl */

/*===========================================================================
FUNCTION DS3GPP_INIT_GLOBAL_CRIT_SECTION
 
DESCRIPTION
  This function initializes the 3GPP Global Critical Section.
 
PARAMETERS 
  None

DEPENDENCIES 
  None

RETURN VALUE 
  None
 
SIDE EFFECTS 
===========================================================================*/
void ds3gpp_init_global_crit_section
(
  void
)
{
  memset(&(ds3gpp_info.global_crit_sect), 0, sizeof(rex_crit_sect_type));
  DS_3GPP_INIT_CRIT_SECTION( &(ds3gpp_info.global_crit_sect) );
} /* ds3gpp_init_global_crit_section */

/*===========================================================================
FUNCTION DS3GPP_ENTER_GLOBAL_CRIT_SECTION
 
DESCRIPTION
  This function enters the 3GPP Global Critical Section.
 
PARAMETERS 
  None

DEPENDENCIES 
  None

RETURN VALUE 
  None
 
SIDE EFFECTS 
===========================================================================*/
void ds3gpp_enter_global_crit_section
(
  void
)
{
  DS_3GPP_ENTER_CRIT_SECTION( &(ds3gpp_info.global_crit_sect) );
} /* ds3gpp_enter_global_crit_section */

/*===========================================================================
FUNCTION DS3GPP_LEAVE_GLOBAL_CRIT_SECTION
 
DESCRIPTION
  This function leaves the 3GPP Global Critical Section.
 
PARAMETERS 
  None

DEPENDENCIES 
  None

RETURN VALUE 
  None
 
SIDE EFFECTS 
===========================================================================*/
void ds3gpp_leave_global_crit_section
(
  void
)
{
  DS_3GPP_LEAVE_CRIT_SECTION( &(ds3gpp_info.global_crit_sect) );
} /* ds3gpp_leave_global_crit_section */

/*===========================================================================
FUNCTION DS3GPP_GET_HANDLER_TBL
 
DESCRIPTION
  This function returns a handle to the 3GPP Handler Table for the 
  specified mode.
 
PARAMETERS 
  System Mode

DEPENDENCIES 
  None

RETURN VALUE 
  Pointer to the 3GPP Handler Table
 
SIDE EFFECTS 
===========================================================================*/
static ds3g_hdlr_func_tbl_type* ds3gpp_get_handler_tbl
(
  sys_sys_mode_e_type mode
)
{
  ds3g_hdlr_func_tbl_type *hdlr_tbl_p = NULL;

  switch (mode)
  {
#ifdef FEATURE_DATA_LTE
    case SYS_SYS_MODE_LTE:
      hdlr_tbl_p = &ds3gpp_info.eps_handler_tbl;
      break;
#endif /* FEATURE_DATA_LTE */
#ifdef FEATURE_DATA_WCDMA_PS
    case SYS_SYS_MODE_WCDMA:
      hdlr_tbl_p = &ds3gpp_info.wcdma_handler_tbl;
      break;
#endif /* FEATURE_DATA_WCDMA_PS */
#ifdef FEATURE_GSM_GPRS
    case SYS_SYS_MODE_GSM:
      hdlr_tbl_p = &ds3gpp_info.gprs_handler_tbl;
      break;
#endif /* FEATURE_GSM_GPRS */
#ifdef FEATURE_TDSCDMA
    case SYS_SYS_MODE_TDS:
      hdlr_tbl_p = &ds3gpp_info.tdscdma_handler_tbl;
      break;
#endif /* FEATURE_TDSCDMA */
    default:
      DS_3GPP_MSG1_ERROR ("Invalid sys mode passed: %d", 
                          mode );
      break;
  }

  return hdlr_tbl_p;
} /* ds3gpp_get_handler_tbl */

/*===========================================================================
FUNCTION DS3GPP_PER_SUBS_INFO_INIT
 
DESCRIPTION
  This function initializes all subscription-specific globals in 
  ds_3gpp_hdlr.c
 
PARAMETERS 
  Subscription id

DEPENDENCIES 
  None

RETURN VALUE 
  void
 
SIDE EFFECTS 
===========================================================================*/
static ds_profile_3gpp_cb_func_tbl_type* ds3gpp_get_ds_profile_cb_func_tbl
(
  void
)
{
  return &ds3gpp_info.ds_profile_3gpp_cb_func_tbl;
}

/*===========================================================================
FUNCTION DS3GPP_PER_SUBS_INFO_INIT
 
DESCRIPTION
  This function initializes all subscription-specific globals in 
  ds_3gpp_hdlr.c
 
PARAMETERS 
  Subscription id

DEPENDENCIES 
  None

RETURN VALUE 
  void
 
SIDE EFFECTS 
===========================================================================*/
static void ds3gpp_per_subs_info_init
(
  sys_modem_as_id_e_type subs_id
)
{
  ds3gpp_per_subs_info_type* per_subs_info_p = NULL;
  /*-----------------------------------------------------------------------*/

  per_subs_info_p = ds3gpp_get_per_subs_info_hndl(subs_id);

  do
  {
    if (per_subs_info_p != NULL)
    {
      DS_3GPP_MSG1_ERROR ("Memory already allocated for Subs Id(CM): %d", 
                          subs_id );
      break;
    }
    else    
    {
      per_subs_info_p = 
        (ds3gpp_per_subs_info_type*)modem_mem_alloc
                                      (sizeof(ds3gpp_per_subs_info_type),
                                         MODEM_MEM_CLIENT_DATA);
      if (per_subs_info_p == NULL)
      {
        DS_3GPP_MSG1_ERROR ("Memory allocation failed for Subs Id(CM): %d", 
                            subs_id );
        break;
      }

      per_subs_info_p->cookie = DS3GPP_PER_SUBS_INFO_COOKIE;
      per_subs_info_p->subs_id = subs_id;

      per_subs_info_p->attach_is_complete = FALSE;
      per_subs_info_p->attach_is_initiated = FALSE;

#ifdef FEATURE_DATA_LTE
      per_subs_info_p->internal_sys_mode = SYS_SYS_MODE_LTE;
      per_subs_info_p->last_full_srv_mode = SYS_SYS_MODE_LTE;
#elif defined FEATURE_GSM_GPRS
      per_subs_info_p->internal_sys_mode = SYS_SYS_MODE_GSM;
      per_subs_info_p->last_full_srv_mode = SYS_SYS_MODE_GSM;
#elif defined FEATURE_DATA_WCDMA_PS
      per_subs_info_p->internal_sys_mode = SYS_SYS_MODE_WCDMA;
      per_subs_info_p->last_full_srv_mode = SYS_SYS_MODE_WCDMA;
#elif defined FEATURE_TDSCDMA
      per_subs_info_p->internal_sys_mode =  SYS_SYS_MODE_TDS;
      per_subs_info_p->last_full_srv_mode = SYS_SYS_MODE_TDS;
#endif

      per_subs_info_p->data_suspend_flag = FALSE;
#ifdef FEATURE_DATA_LTE
      per_subs_info_p->current_ds_lte_flag = DS_3GPP_LTE_NOT_SET;
#endif /* FEATURE_DATA_LTE */
      per_subs_info_p->sent_ps_data_available = FALSE;
      per_subs_info_p->cntxt_transfer_info.cntxt_tranfer_is_recvd = FALSE;
      per_subs_info_p->cntxt_transfer_info.target_rat = SYS_SYS_MODE_NO_SRV;

      ds3gpp_set_per_subs_info_hndl(subs_id, per_subs_info_p);
    }
  }
  while (0); 

} /* ds3gpp_per_subs_info_init */

/*===========================================================================
FUNCTION DS3GPP_INFO_INIT
 
DESCRIPTION
  This function initializes all global information in ds_3gpp_hdlr.c.
 
PARAMETERS 
  None

DEPENDENCIES 
  None

RETURN VALUE 
  void
 
SIDE EFFECTS 
===========================================================================*/
static rex_sigs_type ds3gpp_info_init
(
  void
)
{
  sys_modem_as_id_e_type subs_id = ds3g_get_ps_subs_id();
  rex_sigs_type          requested_sigs = 0;
  /*---------------------------------------------------------------------*/

  /*======================================================================
     During powerup, we allocate 3GPP Info structure only for
     DDS.
   
     If Dual SIM / Triple SIM Device mode event is received at
     a later stage, we will allocate the structure for other
     subscriptions. 
  ======================================================================*/

  ds3gpp_per_subs_info_init(subs_id);

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) || \
    defined(FEATURE_TDSCDMA)
  requested_sigs |= dsumts_hdlr_init();
#endif /* FEATURE_DATA_WCDMA_PS || FEATURE_GSM_GPRS ||
          defined(FEATURE_TDSCDMA)*/

#ifdef FEATURE_DATA_LTE
  requested_sigs |= dseps_hdlr_init();
#endif /* FEATURE_DATA_LTE */

  ds_3gpp_event_init();

  ds_3gpp_profile_hdlr_init();

  return requested_sigs;
} /* ds3gpp_info_init */

/*===========================================================================
FUNCTION ds3gpp_subsmgr_event_cb

DESCRIPTION
  This function calls the call back registered for DS3GSUBSMGR Events

PARAMETERS
  event_id - event type
  *event_info_ptr - event payload
  *data_ptr - not used
  
DEPENDENCIES
  None.

RETURN VALUE  
  None.
 
SIDE EFFECTS

===========================================================================*/
void ds3gpp_subsmgr_event_cb
(
  ds3gsubsmgr_event_type       event_id,
  ds3gsubsmgr_event_info_type *event_info_ptr,
  void                        *data_ptr
)
{
  sys_modem_as_id_e_type curr_subs_id = SYS_MODEM_AS_ID_NONE;
  uint8                  i;
  /*-----------------------------------------------------------------------*/

  DS_3GPP_MSG1_HIGH("DS3G Subs Mgr event %d",event_id);

  if (event_info_ptr == NULL)
  {
    DS_3GPP_MSG0_HIGH("Event info ptr is NULL");
    return;
  }

  switch (event_id)
  {
    case DS3GSUBSMGR_DDS_CHANGE_EV:
    {
      // ds3gsubsmgr should expose API for this check
      if (event_info_ptr->device_mode.curr == 
            SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_STANDBY ||
          event_info_ptr->device_mode.curr == 
            SYS_MODEM_DEVICE_MODE_TRIPLE_SIM_TRIPLE_STANDBY)
      {
        ds_3gpp_pdn_cntxt_local_teardown(FALSE, 
              ds3gsubsmgr_subs_id_ds3g_to_cm(event_info_ptr->def_data_subs.prev));
      }
      break;
    } /* case DS3GSUBSMGR_DDS_CHANGE_EV */

    case DS3GSUBSMGR_DEVICE_MODE_EV:
    {
      switch (event_info_ptr->device_mode.curr)
      {
        case SYS_MODEM_DEVICE_MODE_SINGLE_SIM:
        {
          curr_subs_id = ds3gsubsmgr_subs_id_ds3g_to_cm
                           (event_info_ptr->def_data_subs.curr);
          ds_3gpp_hdlr_read_subs_id(curr_subs_id);
          ds3gpp_per_subs_info_init(curr_subs_id);
          ds_3gpp_pdn_context_per_subs_info_init(curr_subs_id);
          ds_bearer_cntxt_per_subs_info_init(curr_subs_id);
          ds_3gpp_srv_req_throt_init(curr_subs_id);
          ds_3gpp_pdn_throt_sm_per_apn_power_up_init(curr_subs_id);
          ds_3gpp_pdn_throt_apn_disable_powerup_throttle(curr_subs_id);
          ds_3gpp_auth_mgr_per_subs_info_init(curr_subs_id);
          ds_3gpp_network_override_hdlr_per_subs_init(curr_subs_id);
#ifdef FEATURE_DATA_LTE
          ds_eps_tlb_per_subs_info_init(curr_subs_id);
          ds_eps_pdn_throttle_sm_init(curr_subs_id);
#endif /* FEATURE_DATA_LTE */
#ifdef FEATURE_DATA_EMBMS
          ds_3gpp_embms_per_subs_info_init(curr_subs_id);
          ds_3gpp_embms_tmgi_tbl_mgr_init_context_table(curr_subs_id);
#endif /* FEATURE_DATA_EMBMS */
 
#ifdef FEATURE_DATA_REL10
          if(ds_3gpp_nv_manager_get_nv_lte_rel_10(curr_subs_id))
          {
            ds_3gpp_apn_throt_init(curr_subs_id);
          }
#endif /*FEATURE_DATA_REL10*/

#if defined (FEATURE_DATA_WLAN_MAPCON) && defined (FEATURE_DATA_LTE) 
          ds_eps_kamgr_read_efs_info(curr_subs_id);
#endif /* FEATURE_DATA_WLAN_MAPCON && FEATURE_DATA_LTE */
          break;
        } /* case SYS_MODEM_DEVICE_MODE_SINGLE_SIM */
     
        case SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_STANDBY:
        case SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_ACTIVE:
        case SYS_MODEM_DEVICE_MODE_TRIPLE_SIM_TRIPLE_STANDBY:
        {
          for (i=0; i< DS3GSUBSMGR_SUBS_ID_MAX; i++)
          {
            ds_3gpp_hdlr_read_subs_id(i);
            ds3gpp_per_subs_info_init(i);
            ds_3gpp_pdn_context_per_subs_info_init(i);
            ds_bearer_cntxt_per_subs_info_init(i);
            ds_3gpp_srv_req_throt_init(i);
            ds_3gpp_pdn_throt_sm_per_apn_power_up_init(i);
            ds_3gpp_pdn_throt_apn_disable_powerup_throttle(i);
            ds_3gpp_auth_mgr_per_subs_info_init(i);
            ds_3gpp_network_override_hdlr_per_subs_init(i);
#ifdef FEATURE_DATA_LTE
            ds_eps_tlb_per_subs_info_init(i);
            ds_eps_pdn_throttle_sm_init(i);
#endif /* FEATURE_DATA_LTE */
#ifdef FEATURE_DATA_EMBMS
            ds_3gpp_embms_per_subs_info_init(i);
            ds_3gpp_embms_tmgi_tbl_mgr_init_context_table(i);
#endif  /* FEATURE_DATA_EMBMS */

#ifdef FEATURE_DATA_REL10
            if(ds_3gpp_nv_manager_get_nv_lte_rel_10(i))
            {
              ds_3gpp_apn_throt_init(i);
            }
#endif /*FEATURE_DATA_REL10*/

#if defined (FEATURE_DATA_WLAN_MAPCON) && defined (FEATURE_DATA_LTE) 
            ds_eps_kamgr_read_efs_info(i);
#endif /* FEATURE_DATA_WLAN_MAPCON && FEATURE_DATA_LTE */
          }
          break;
        } /* case SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_STANDBY */
     
        default:
        {
          DS_3GPP_MSG0_MED("Ignore Subsmgr event");
          break;
        }
      } /* switch (event_info_ptr->device_mode.curr)*/

      break;

    } /* case DS3GSUBSMGR_DEVICE_MODE_EV */

    default:
       break;
  } /* switch (event_id) */

  return ;
} /* ds3gpp_subsmgr_event_cb */

/*===========================================================================
FUNCTION ds3gpp_ds3g_event_cb

DESCRIPTION
  This function calls the call back registered for DS3G Events

PARAMETERS
  event_id - event type
  *event_info_ptr - event payload
  *data_ptr - not used
  
DEPENDENCIES
  None.

RETURN VALUE  
  None.
 
SIDE EFFECTS

===========================================================================*/
void ds3gpp_ds3g_event_cb
(
  ds3geventmgr_event_type       event_id,
  ds3geventmgr_filter_type      *filter_info_ptr,
  void                          *event_info_ptr,
  void                          *data_ptr
)
{
  ds_cmd_type                               *cmd_ptr;
  ds_3gpp_pdn_cntxt_cm_notify_cmd_info_type *cm_notify_cmd_info_ptr = NULL;
  sys_sys_mode_e_type                       current_nw_mode = SYS_SYS_MODE_NONE; 
  sys_modem_as_id_e_type                    subs_id = SYS_MODEM_AS_ID_NONE; 
  sys_modem_as_id_e_type                    *subs_id_ptr = NULL;
  ds3geventmgr_th_mit_action_type           *th_mit_action_ptr = NULL;
  ds_mgr_latency_info_ext_type              ds_mgr_low_latency_info;
  ps_sys_low_latency_traffic_info_type      *ps_latency_info = NULL;
  boolean ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( filter_info_ptr == NULL )
  {
    DS_3GPP_MSG0_ERROR("Invalid input data, Sub ID Mask not provided");
    return;
  }

  if((filter_info_ptr->ps_subs_id < PS_SYS_PRIMARY_SUBS || 
     filter_info_ptr->ps_subs_id > PS_SYS_TERTIARY_SUBS))
  {
    DS_3GPP_MSG1_ERROR("Invalid Subs Mask: %d",filter_info_ptr->ps_subs_id);
    return;
  }

  subs_id = ds3gsubsmgr_subs_id_ds_to_cm((ds_sys_subscription_enum_type)
                                         filter_info_ptr->ps_subs_id);

  DS_3GPP_MSG1_HIGH("Processing DS3G event on subs id %d", subs_id);

  switch (event_id) 
  {
    case DS3GEVENTMGR_MODE_LPM_EV:
      /*----------------------------------------------------------------------- 
        Set the Bit mask corresponding to 3GPP MH for LPM/Processing event.
        If the bit mask cannot be set, then there is a possibility that CM
        may shutdown the stack, before DS 3GPP MH completes its processing
        (in which case, graceful shutdown may not have completed)
      -----------------------------------------------------------------------*/
      if(ds3geventmgr_set_lpm_pwr_off_status(
           DS3GEVENTMGR_LPM_PWROFF_3GPP_CTRL_BIT_MSK) 
         == FALSE)
      {
        DS_3GPP_MSG1_HIGH("Cannot set lpm pwr off %d Bit msk",
                          DS3GEVENTMGR_LPM_PWROFF_3GPP_CTRL_BIT_MSK);
      }

      cmd_ptr = 
        ds_allocate_cmd_buf(sizeof(ds_3gpp_pdn_cntxt_cm_notify_cmd_info_type));
      if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
      {
        ASSERT(0);
        return;
      }

      cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_CM_NOTIFY;
      cm_notify_cmd_info_ptr = 
        (ds_3gpp_pdn_cntxt_cm_notify_cmd_info_type*)cmd_ptr->cmd_payload_ptr;
      cm_notify_cmd_info_ptr->subs_id = subs_id;
      cm_notify_cmd_info_ptr->cm_notify_type =DS_3GPP_PDN_CNTXT_PWROFF_LPM_MODE;

      ds_put_cmd(cmd_ptr);
      break;

    case DS3GEVENTMGR_SUB_NOT_AVAIL_EV:
      /*----------------------------------------------------------------------- 
        Set the Bit mask corresponding to 3GPP MH for SUB_NOT_AVAIL Processing
        event. If the bit mask cannot be set, then there is a possibility that
        CM may shutdown the stack, before DS 3GPP MH completes its processing
        (in which case, graceful shutdown may not have completed)
      -----------------------------------------------------------------------*/
      if(ds3geventmgr_set_sub_not_avail_status
                                (DS3GEVENTMGR_SUB_NOT_AVAIL_3GPP_CTRL_BIT_MSK,
                                 subs_id) == FALSE)
      {
        DS_3GPP_MSG2_HIGH("Cannot set SUB_NOT_AVAIL %d Bit msk on subs %d",
                          DS3GEVENTMGR_SUB_NOT_AVAIL_3GPP_CTRL_BIT_MSK,
                          subs_id);
      }

      current_nw_mode = ds3g_get_current_network_mode_ex(subs_id);

      if (current_nw_mode == SYS_SYS_MODE_GSM ||
          current_nw_mode == SYS_SYS_MODE_WCDMA ||
          current_nw_mode == SYS_SYS_MODE_LTE ||
          current_nw_mode == SYS_SYS_MODE_TDS)
      {
        cmd_ptr = 
         ds_allocate_cmd_buf(sizeof(ds_3gpp_pdn_cntxt_cm_notify_cmd_info_type));
        if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
        {
          ASSERT(0);
          return;
        }

        cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_CM_NOTIFY;
        cm_notify_cmd_info_ptr = 
          (ds_3gpp_pdn_cntxt_cm_notify_cmd_info_type*)cmd_ptr->cmd_payload_ptr;
        cm_notify_cmd_info_ptr->subs_id = subs_id;
        cm_notify_cmd_info_ptr->cm_notify_type 
                                       = DS_3GPP_PDN_CNTXT_SUB_NOT_AVAIL_NOTIFY;

        ds_put_cmd(cmd_ptr);
      }
      else
      {
        ds3geventmgr_clr_and_notify_sub_not_avail_status(
           DS3GEVENTMGR_SUB_NOT_AVAIL_3GPP_CTRL_BIT_MSK,
           subs_id,
           ds3gi_sub_not_avail_subs_id_mask);

        DS_3GPP_MSG2_HIGH("Network mode %d is not a 3GPP mode. Not processing "
                          "SUB_NOT_AVAIL event for 3GPP MH on subs %d",
                          current_nw_mode,
                          subs_id);
      }
      break;

    case DS3GEVENTMGR_NV_REFRESH_EV:
      cmd_ptr = ds_allocate_cmd_buf(sizeof(sys_modem_as_id_e_type));
      if((NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr))
      {
        ASSERT(0);
        return;
      }

      cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_NV_REFRESH;
      subs_id_ptr = (sys_modem_as_id_e_type*)(cmd_ptr->cmd_payload_ptr);
      *subs_id_ptr = subs_id;
      ds_put_cmd(cmd_ptr);
      break;
    case DS3GEVENTMGR_LATENCY_INFO_EV:
      if(event_info_ptr == NULL)
      {
         DS_3GPP_MSG0_ERROR("ds3gpp_ds3g_event_cb: event_info_ptr NULL");
         return;
      }
      memset(&ds_mgr_low_latency_info, 0,sizeof(ds_mgr_latency_info_ext_type));
      ps_latency_info = &(((ds3geventmgr_event_info_type*)event_info_ptr)->latency_info);
      ds_mgr_low_latency_info.traffic_state = (ds_low_latency_traffic_state_enum_type)ps_latency_info->traffic_state;
      ds_mgr_low_latency_info.filter_handle = ps_latency_info->filter_handle;
      ds_mgr_low_latency_info.pkt_interval_time_in_ms = 
                             ps_latency_info->pkt_interval_time_in_ms;
      ds_mgr_low_latency_info.pkt_arrival_delta_in_ms = 
                             ps_latency_info->pkt_arrival_delta_in_ms;
      ds_mgr_low_latency_info.pkt_max_wait_time_in_ms = 
                             ps_latency_info->pkt_max_wait_time_in_ms;
      DS_3GPP_MSG1_HIGH("ds_3gpp_low_latency_ind_cmd: traffic state:%d",
                          ds_mgr_low_latency_info.traffic_state);
      ret_val = dsmsgrsnd_per_subs_msg_send_ext(
                          DS_MGR_IND_LOW_LATENCY,
                          MSGR_DS_MGR, 
                          (msgr_hdr_struct_type*)(&ds_mgr_low_latency_info),
                          sizeof(ds_mgr_latency_info_ext_type),
                          SYS_AS_ID_TO_INST_ID(subs_id));
      break;
    case DS3GEVENTMGR_THERMAL_MITIGATION_EV:
      cmd_ptr = ds_allocate_cmd_buf(sizeof(ds3geventmgr_th_mit_action_type));
      if((NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr))
      {
        ASSERT(0);
        return;
      }

      cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_THERMAL_MITIGATION;
      th_mit_action_ptr 
               = (ds3geventmgr_th_mit_action_type*)(cmd_ptr->cmd_payload_ptr);
      *th_mit_action_ptr = 
        ((ds3geventmgr_event_info_type*)event_info_ptr)->
                                               thermal_mitigation_info.action;
      ds_put_cmd(cmd_ptr);
      break;

    case DS3GEVENTMGR_PLMN_LIST_CHG_EV:
      if(event_info_ptr == NULL)
      {
        return;
      }
      cmd_ptr = ds_allocate_cmd_buf(sizeof(sys_modem_as_id_e_type));
      if((NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr))
      {
        ASSERT(0);
        return;
      }

      cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_PLMN_LIST_CHG;
      subs_id_ptr = (sys_modem_as_id_e_type*)(cmd_ptr->cmd_payload_ptr);
      *subs_id_ptr = subs_id;
      ds_put_cmd(cmd_ptr);
      break;

    default:
      DS_3GPP_MSG2_HIGH("Unrecognized DS3G event %d on subs id %d", 
                        event_id, subs_id);
      break;
  }

  return;
} /*ds3gpp_ds3g_event_cb*/

/*===========================================================================
FUNCTION ds3gpp_get_subs_id_for_w_td_mode

DESCRIPTION
  This function returns the Subscription on which the current mode is
  WCDMA or TDSCDMA.
  This temporary hack is used because we do not support W/TD + W/TD
  DSDA configurations yet. 

PARAMETERS
  Sys Mode
  
DEPENDENCIES
  None.

RETURN VALUE  
  None.
 
SIDE EFFECTS

===========================================================================*/
sys_modem_as_id_e_type ds3gpp_get_subs_id_for_w_td_mode
(
  void
)
{
  ds3gpp_per_subs_info_type* hndl = NULL;
  int                        i = 0;
  /*----------------------------------------------------------------------*/

  for (i=0; i < DS3GSUBSMGR_SUBS_ID_MAX; i++)
  {
    hndl = ds3gpp_get_per_subs_info_hndl(i);

    if (!ds3gpp_validate_per_subs_info_hndl(hndl))
    {
      continue;
    }

    if (hndl->internal_sys_mode == SYS_SYS_MODE_WCDMA || 
        hndl->internal_sys_mode == SYS_SYS_MODE_TDS)
    {
      break;
    }
  }

  return i;
} /* ds3gpp_get_subs_id_from_mode */

/*===========================================================================
FUNCTION ds_3gpp_map_radio_access_to_sys_mode

DESCRIPTION
  This function maps sys_radio_access_tech_e_type to sys_sys_mode_e_type.

PARAMETERS
  sys_radio_access - system rat
  
DEPENDENCIES
  None.

RETURN VALUE  
  sys_sys_mode - system mode
 
SIDE EFFECTS 
  None. 

===========================================================================*/
sys_sys_mode_e_type ds_3gpp_map_radio_access_to_sys_mode
(
    sys_radio_access_tech_e_type sys_radio_access
)
{
  sys_sys_mode_e_type sys_sys_mode;
/*----------------------------------------------------------------------*/

  switch (sys_radio_access) 
  {
    case SYS_RAT_GSM_RADIO_ACCESS:
      sys_sys_mode = SYS_SYS_MODE_GSM;
      break;
    case SYS_RAT_UMTS_RADIO_ACCESS:
      sys_sys_mode = SYS_SYS_MODE_WCDMA;
      break;
    case SYS_RAT_LTE_RADIO_ACCESS:
      sys_sys_mode = SYS_SYS_MODE_LTE;
      break;
    case SYS_RAT_TDS_RADIO_ACCESS:
      sys_sys_mode = SYS_SYS_MODE_TDS;
      break;
    default:
      sys_sys_mode = SYS_SYS_MODE_NONE;
      break;
  }
  return sys_sys_mode;
} /* ds_3gpp_map_radio_access_to_sys_mode */

/*===========================================================================
FUNCTION ds_3gpp_map_sys_mode_to_ds_sys_rat

DESCRIPTION
  This function maps CM Sys Mode to DS SYS RAT

PARAMETERS
  sys_sys_mode_e_type - CM Sys Mode
  
DEPENDENCIES
  None.

RETURN VALUE  
  ds_sys_rat_ex_enum_type - DS Sys RAT
 
SIDE EFFECTS 
  None. 

===========================================================================*/
ds_sys_rat_ex_enum_type ds_3gpp_map_sys_mode_to_ds_sys_rat
(
  sys_sys_mode_e_type sys_mode
)
{
  ds_sys_rat_ex_enum_type ds_sys_rat = DS_SYS_RAT_EX_NULL_BEARER;
/*----------------------------------------------------------------------*/

  switch (sys_mode) 
  {
    case SYS_SYS_MODE_GSM:
      ds_sys_rat = DS_SYS_RAT_EX_3GPP_GERAN;
      break;
    case SYS_SYS_MODE_WCDMA:
      ds_sys_rat = DS_SYS_RAT_EX_3GPP_WCDMA;
      break;
    case SYS_SYS_MODE_LTE:
      ds_sys_rat = DS_SYS_RAT_EX_3GPP_LTE;
      break;
    case SYS_SYS_MODE_TDS:
      ds_sys_rat = DS_SYS_RAT_EX_3GPP_TDSCDMA;
      break;
    default:
      ds_sys_rat = DS_SYS_RAT_EX_NULL_BEARER;
      break;
  }
  return ds_sys_rat;
} /* ds_3gpp_map_sys_mode_to_ds_sys_rat */

#endif /* FEATURE_DATA_3GPP */

/*===========================================================================

                                UNIT TEST

===========================================================================*/
