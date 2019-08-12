/*===========================================================================

  Copyright (c) 2009-2016 Qualcomm Technologies Incorporated. All Rights Reserved

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

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "data_msg.h"

#ifdef FEATURE_DATA_LTE

#include "mcfg_fs.h"
#include "dsutil.h"
#include "ds_eps_kamgr_int.h"
#include "dsumtspdpreg.h"
#include "modem_mem.h"
#include "ps_route_iface.h"
#include "cm.h"
#include "ds_system_heap.h"
#include "ds3gutil.h"
#include "ds_3gpp_pdn_context.h"
#include "ds_3gpp_pdn_throttle_sm.h"
#include "ds_eps_pdn_context.h"
#include "ds_3gpp_apn_table.h"
#include "ds_3gppi_utils.h"
#include "ps_crit_sect.h"
#include "ps_iface.h"
#include "ps_sys_ioctl.h"
#include "dsumtspdpregint.h"
#include "ds3gdevmgr.h"
#include "ds3geventmgr.h"
#include "ds_loopback_hdlr.h"
#include <stringl/stringl.h>
#ifdef FEATURE_DATA_WLAN_MAPCON
#include "ds_wlan_util.h"
#endif  /* FEATURE_DATA_WLAN_MAPCON */

/*===========================================================================

                          MACRO DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  EFS txt file that contains the configurable keep alive APN name
---------------------------------------------------------------------------*/
#define DS_EPS_KAMGR_PDN  "/data/ds_eps_kamgr_pdn.txt"

#define DS_EPS_KAMGR_IS_3GPP_CURRENT_MODE(call_mode)  \
  (                                                 \
      (call_mode == DS_PKT_SYS_MODE_WCDMA) ||       \
      (call_mode == DS_PKT_SYS_MODE_GSM)   ||       \
      (call_mode == DS_PKT_SYS_MODE_LTE)   ||       \
      (call_mode == DS_PKT_SYS_MODE_TDS)            \
  )


/*---------------------------------------------------------------------------
  Macro to get subscription based timer id for subscription id
    timer_id per subs_id = (subs_id * 1000)+timer_id
---------------------------------------------------------------------------*/
#define DS_EPS_KAMGR_TIMER_ID_FOR_SUBS_ID(subs_id, timer_id) \
          ((unsigned long)(subs_id*1000) + timer_id)

/*---------------------------------------------------------------------------
  Macro to get subscription id from timer id for 
    subs_id = (timer_id / 1000)
---------------------------------------------------------------------------*/
#define DS_EPS_KAMGR_SUBS_ID_FROM_SUBS_TIMER_ID(timer_id) (timer_id/1000)

/*---------------------------------------------------------------------------
  Macro to get timer id from subscription based timer id for 
    subs_id = (timer_id / 1000)
---------------------------------------------------------------------------*/
#define DS_EPS_KAMGR_TIMER_ID_FROM_SUBS_TIMER_ID(timer_id,subs_id) \
    (timer_id - 1000*subs_id)

/*---------------------------------------------------------------------------
  Struct for DS_CMD_3GPP_EPS_KAMGR_QMI_BRING_UP_IND command
---------------------------------------------------------------------------*/
typedef struct
{
  uint16                  profile_id;
  sys_modem_as_id_e_type  subs_id;
}ds_eps_kamgr_qmi_bu_ind_cmd_type;

/*---------------------------------------------------------------------------
  Struct for DS_CMD_3GPP_EPS_KAMGR_SYS_INFO command
---------------------------------------------------------------------------*/
typedef struct
{
  ps_sys_system_status_ex_type  *sys_info_ptr;
  sys_modem_as_id_e_type         subs_id;
}ds_eps_kamgr_sys_info_cmd_type;

/*---------------------------------------------------------------------------
  Struct for KAMGR iface events commands
---------------------------------------------------------------------------*/
typedef struct
{
  ps_iface_type               *iface_ptr;
  ps_iface_event_info_u_type   event_info;
}ds_eps_kamgr_iface_events_cmd_type;

/*---------------------------------------------------------------------------
  Struct for DS_CMD_3GPP_EPS_KAMGR_TIMER_EXPIRY_CB command
---------------------------------------------------------------------------*/
typedef struct
{
  sys_modem_as_id_e_type  subs_id;
  uint8                   timer_id;
}ds_eps_kamgr_timer_cmd_type;

/*===========================================================================

                          GLOBAL VARIABLES

===========================================================================*/
/*---------------------------------------------------------------------------
  Global variable that stores the KAMGR state
---------------------------------------------------------------------------*/
static ds_eps_kamgr_info_type  *ds_eps_kamgr[DS3GSUBSMGR_SUBS_ID_COUNT]
                                                                    = {NULL};

/*---------------------------------------------------------------------------
  Global to inform whether System status cback registration was successful
  or not
---------------------------------------------------------------------------*/
boolean                  ds_eps_kamgr_system_status_cback_reg_status = FALSE;

/*-------------------------------------------------------------------------- 
  These tags are used in EFS configuration file and used for String compare
----------------------------------------------------------------------------*/
const char              *ds_eps_kamgr_tags[2] = {"PROFILE_ID","TIMER_VAL"};

/*---------------------------------------------------------------------------
  Global variable to store if KAMGR has registered for all iface up event
  indications
---------------------------------------------------------------------------*/
static boolean ds_eps_kamgr_all_iface_up_regd[DS3GSUBSMGR_SUBS_ID_COUNT]
                                                                   = {FALSE};

/*---------------------------------------------------------------------------
  Global variable to store if KAMGR has registered for iface events
  indication
---------------------------------------------------------------------------*/
static boolean ds_eps_kamgr_iface_ev_regd[DS3GSUBSMGR_SUBS_ID_COUNT] = {FALSE};

void                    *ds_eps_kamgr_all_iface_up_ev_buf_ptr = NULL;


/*===========================================================================

                         INTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION  DS_EPS_KAMGR_ALLOC_CBACK_BUF

DESCRIPTION
  This function allocates cback buffers for IFACE UP and IFACE DOWN events
  
PARAMETERS
  None
 
DEPENDENCIES
  TRUE: If cback buffer allocation was successful
  FALSE: Cback buffer allocation failure

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static boolean ds_eps_kamgr_alloc_cback_buf
(
  sys_modem_as_id_e_type  subs_id
)
{
  boolean                                  ret_val = TRUE;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return FALSE;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  do
  {
    if(ds_eps_kamgr[subs_indx] == NULL)
    {
      DS_LTE_MSG0_ERROR("ds_eps_kamgr_alloc_cback_buf: Keep alive "
                        "manager instance released");
      ret_val = FALSE;
      break;
    }

    if(ds_eps_kamgr[subs_indx]->ps_iface_up_ev_buf_ptr == NULL)
    {
      ds_eps_kamgr[subs_indx]->ps_iface_up_ev_buf_ptr = ps_iface_alloc_event_cback_buf(
                                            ds_eps_kamgr_iface_up_ev_cback,
                                            NULL);

      if(ds_eps_kamgr[subs_indx]->ps_iface_up_ev_buf_ptr == NULL)
      {
        DS_LTE_MSG0_ERROR("ds_eps_kamgr_alloc_cback_buf: Unable to "
                          "allocate cback buffers");
        ds_eps_kamgr[subs_indx]->active_ind = DS_EPS_KAMGR_RESET;
        ds_eps_kamgr_retry_after_failure(subs_id, PS_NET_DOWN_REASON_INTERNAL_ERROR, TRUE);
        ret_val = FALSE;
        break;
      }
    }

    if(ds_eps_kamgr[subs_indx]->ps_iface_down_ev_buf_ptr == NULL)
    {
      ds_eps_kamgr[subs_indx]->ps_iface_down_ev_buf_ptr = ps_iface_alloc_event_cback_buf(
                                            ds_eps_kamgr_iface_down_ev_cback,
                                            NULL);

      if(ds_eps_kamgr[subs_indx]->ps_iface_down_ev_buf_ptr == NULL)
      {
        DS_LTE_MSG0_ERROR("ds_eps_kamgr_alloc_cback_buf: Unable to "
                          "allocate cback buffers");
        ds_eps_kamgr[subs_indx]->active_ind = DS_EPS_KAMGR_RESET;
        ps_iface_free_event_cback_buf((void *)ds_eps_kamgr[subs_indx]->ps_iface_up_ev_buf_ptr);
        ds_eps_kamgr[subs_indx]->ps_iface_up_ev_buf_ptr = NULL;
        ds_eps_kamgr_retry_after_failure(subs_id, PS_NET_DOWN_REASON_INTERNAL_ERROR, TRUE);
        ret_val = FALSE;
        break;
      }
    }
  }while(0);

  return ret_val;
}

/*==============================================================================
FUNCTION  DS_EPS_KAMGR_PROCESS_PDN_THROTTLE_INFO_FOR_BRING_UP

DESCRIPTION
  This function gets called when keep alive pdn timer expires, we now initate
  a new request for bring up of keep alive PDN
  
PARAMETERS 
  pdn_throttle_info:               Structure containing throttle information

DEPENDENCIES
  None

RETURN VALUE
  TRUE: If Infinite throttling is in progress
  FALSE: If there is no Infinite throttling involved

SIDE EFFECTS
  None

================================================================================*/
static boolean ds_eps_kamgr_process_pdn_throttle_info_for_bring_up
(
   sys_modem_as_id_e_type          subs_id,
   ps_sys_pdn_throttle_info_type                     *throttle_info,
   ps_iface_net_down_reason_type                      down_reason
)
{
   boolean                                             permanent_failure = FALSE;
#ifdef FEATURE_DATA_WLAN_MAPCON
   uint16                                              index = 0;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return permanent_failure;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  DS_LTE_MSG0_LOW("ds_eps_kamgr_process_pdn_throttle_info_for_bring_up: "
                  "Entered");

  if(throttle_info == NULL || ds_eps_kamgr[subs_indx] == NULL)
  {
    DS_LTE_MSG0_LOW("ds_eps_kamgr_process_pdn_throttle_info_for_bring_up: "
                    "Invalid function call made");
    return permanent_failure;
  }

  for(index = 0; index < throttle_info->num_apn; index++)
  {
    if(strncasecmp((const char*)throttle_info->pdn_throttle_info[index].apn_string, 
          (const char*)ds_eps_kamgr[subs_indx]->apn_name.apn_string, DS_SYS_MAX_APN_LEN) == 0)
    {
      DS_LTE_MSG1_MED("ds_eps_kamgr_process_pdn_throttle_info_for_bring_up, "
                    "APN match at index %d ",index);
      break;
    }
  }
  /*---------------------------------------------------------------------------
    If
      None of the IP types are throttled then we revert to default timer logic
    Else
      depending on IP type that is throttled for a longer time, we decide the
      back-off timer value
  ----------------------------------------------------------------------------*/
  if (index == throttle_info->num_apn || 
      (throttle_info->pdn_throttle_info[index].is_ipv4_throttled == FALSE 
       && throttle_info->pdn_throttle_info[index].is_ipv6_throttled == FALSE))
  {
    DS_LTE_MSG0_MED("ds_eps_kamgr_process_pdn_throttle_info_for_bring_up, APN"
                   " not throttled initiating bring up after waiting for "
                   "some time");
    ds_eps_kamgr_retry_after_failure(subs_id, down_reason, FALSE);
  }
  else
  {
    ds_eps_kamgr[subs_indx]->bring_up_timer_val = 
     (throttle_info->pdn_throttle_info[index].remaining_ipv4_throttled_time >
     throttle_info->pdn_throttle_info[index].remaining_ipv6_throttled_time) ?
     (throttle_info->pdn_throttle_info[index].remaining_ipv4_throttled_time) :
     (throttle_info->pdn_throttle_info[index].remaining_ipv6_throttled_time);


    if(ds_eps_kamgr[subs_indx]->bring_up_timer_val == DS_3GPP_PDN_THROT_INFINITE_THROT)
    {
      DS_LTE_MSG0_MED("No retry attempted to bring-up KAMGR, infinite throttling");
      permanent_failure = TRUE;
    }
    else
    {
      rex_set_timer(&(ds_eps_kamgr[subs_indx]->keep_alive_bring_up_timer),
                    (rex_timer_cnt_type)ds_eps_kamgr[subs_indx]->bring_up_timer_val);
    }
  }
#else
   DS_LTE_MSG0_LOW ("ds_eps_kamgr_process_pdn_throttle_info_for_bring_up: "
                     "MAPCON feature disabled");
#endif /* FEATURE_DATA_WLAN_MAPCON */

   return permanent_failure;
}

/*===========================================================================
FUNCTION  DS_EPS_KAMGR_REG_FOR_ALL_IFACE_UP_EV_CBACK

DESCRIPTION
  Registering for all 3GPP ifaces at boot-up so that once KAMGR PDN comes up,
  it is marked to stay active even after AP disconnects the call
  
PARAMETERS
  None
 
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_eps_kamgr_dereg_for_all_iface_up_ev_cback
(
  sys_modem_as_id_e_type  subs_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  ds3gsubsmgr_subs_id_e_type  subs_indx;
  boolean                     iface_ev_ind_regd = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return;
  }

  /*---------------------------------------------------------------------
    cache the registration status
  ---------------------------------------------------------------------*/
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);
  ds_eps_kamgr_all_iface_up_regd[subs_indx] = FALSE;

  /*-------------------------------------------------------------------------
    Since Iface event registration is not per subscription, we would need to
    deregister only once. Check if all iface up event is still registered
  -------------------------------------------------------------------------*/
  for( subs_indx = DS3GSUBSMGR_SUBS_ID_MIN; subs_indx < DS3GSUBSMGR_SUBS_ID_MAX;
       subs_indx++ )
  {
    if( ds_eps_kamgr_all_iface_up_regd[subs_indx] == TRUE )
    {
      iface_ev_ind_regd = TRUE;
      break;
    }
  }

  if( (iface_ev_ind_regd == FALSE) && 
      (ds_eps_kamgr_all_iface_up_ev_buf_ptr != NULL) )
  {
    ps_iface_event_cback_dereg(NULL,
                               IFACE_UP_EV,
                               ds_eps_kamgr_all_iface_up_ev_buf_ptr);

    ps_iface_free_event_cback_buf(ds_eps_kamgr_all_iface_up_ev_buf_ptr);
    ds_eps_kamgr_all_iface_up_ev_buf_ptr = NULL;
  }
  else
  {
    DS_LTE_MSG0_ERROR("IFACE UP Event buffer for all ifaces already released");
  }
}/*ds_eps_kamgr_dereg_for_all_iface_up_ev_cback*/

/*===========================================================================
FUNCTION  DS_EPS_KAMGR_FREE_IFACE_CBACK_BUF

DESCRIPTION
  This function is responsible for cleaning up Iface event cback buffers,
  this function is called during bring-up of KAMGR when:
  1. Bring-up fails due to iface alloaction failure
  2. KAMGR PDN is UP but it state is not valid
  3. KAMGRR could not find a valid iface from KAMGR PDN
  
PARAMETERS 
  None

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_eps_kamgr_free_iface_cback_buf
(
  sys_modem_as_id_e_type  subs_id
)
{ 
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  if(ds_eps_kamgr[subs_indx] != NULL)
  {
    if(ds_eps_kamgr[subs_indx]->ps_iface_down_ev_buf_ptr != NULL)
    {
      ps_iface_free_event_cback_buf(ds_eps_kamgr[subs_indx]->ps_iface_down_ev_buf_ptr);
      ds_eps_kamgr[subs_indx]->ps_iface_down_ev_buf_ptr = NULL;
    }

    if (ds_eps_kamgr[subs_indx]->ps_iface_up_ev_buf_ptr != NULL)
    {
      ps_iface_free_event_cback_buf(ds_eps_kamgr[subs_indx]->ps_iface_up_ev_buf_ptr);
      ds_eps_kamgr[subs_indx]->ps_iface_up_ev_buf_ptr = NULL;
    }
    ds_eps_kamgr[subs_indx]->active_ind = DS_EPS_KAMGR_RESET;
    ds_eps_kamgr_retry_after_failure(subs_id, PS_NET_DOWN_REASON_INTERNAL_ERROR, TRUE);
  }
  else
  {
    DS_LTE_MSG0_LOW ("Keep ALive Manager Instance not active, No clean-up "
                       "needed");
  }
}/*ds_eps_kamgr_free_iface_cback_buf*/

/*===========================================================================
FUNCTION DS_EPS_KAMGR_SETUP_IFACE_EVENT_CBACKS

DESCRIPTION
  This function registers for cbacks with IFACE UP and IFACE DOWN events

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_kamgr_setup_iface_event_cbacks
(
  sys_modem_as_id_e_type   subs_id,
  ps_iface_type                       *ps_iface_ptr
)
{
  int16                       ps_iface_reg_result = -1;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
  boolean                     iface_ev_ind_regd = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ps_iface_ptr == NULL)
  {
    DS_LTE_MSG1_ERROR("Invalid PS iface passed,cannot setup event cbacks: 0x%x"
                      ,ps_iface_ptr);
    return;
  }

  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return;
  }

  /*-------------------------------------------------------------------------
    Since Iface event registration is not per subscription, we would need to
    register only once. Check if all iface up event is already registered
  -------------------------------------------------------------------------*/
  for( subs_indx = DS3GSUBSMGR_SUBS_ID_MIN; subs_indx < DS3GSUBSMGR_SUBS_ID_MAX;
       subs_indx++ )
  {
    if( ds_eps_kamgr_iface_ev_regd[subs_indx] == TRUE )
    {
      iface_ev_ind_regd = TRUE;
      break;
    }
  }

  /*-------------------------------------------------------------------------
    Register for iface events only if previously not registered
  -------------------------------------------------------------------------*/
  if( iface_ev_ind_regd == FALSE )
  {
    subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);
    ps_iface_reg_result = ps_iface_event_cback_reg (ps_iface_ptr,
                                                    IFACE_UP_EV,
                                            ds_eps_kamgr[subs_indx]->ps_iface_up_ev_buf_ptr);

    if(ps_iface_reg_result != 0)
    {
      /*---------------------------------------------------------------------
        Since we were not able to register for iface up event, there is no
        point of cback buffer so we free it up, since during next full
        service when we again try bring-up this will not cause a memory leak
      ----------------------------------------------------------------------*/
      DS_LTE_MSG0_LOW("ds_eps_kamgr_setup_iface_event_cbacks: Iface up event "
                      "registration failed, maybe registration attempted twice");
    }

    ps_iface_reg_result = ps_iface_event_cback_reg (ps_iface_ptr,
                                                    IFACE_DOWN_EV,
                                           ds_eps_kamgr[subs_indx]->ps_iface_down_ev_buf_ptr);

    if (ps_iface_reg_result != 0)
    {
      DS_LTE_MSG0_MED("ds_eps_kamgr_setup_iface_event_cbacks: Iface down event "
                      "registration failed, maybe registration attempted twice");
    }
  }

  if (ps_iface_reg_result == 0)
  {
    DS_3GPP_MSG0_HIGH("EPS KA MGR iface ev reg success");
    /*---------------------------------------------------------------------
      cache the registration status
    ---------------------------------------------------------------------*/
    ds_eps_kamgr_iface_ev_regd[subs_indx] = TRUE;
  }
}/*ds_eps_kamgr_setup_iface_event_cbacks*/

/*==============================================================================
FUNCTION DS_EPS_KAMGR_DEREG_IFACE_CBACKS

DESCRIPTION
  This function is called to clean up all the cback resources associated with
  an iface
 
PARAMETERS 
  this_iface_ptr:          Iface whose call backs and buffers have to be cleaned
                           up

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

================================================================================*/
void ds_eps_kamgr_dereg_iface_cbacks
(
  ps_iface_type                              *this_iface_ptr,
  ds3gsubsmgr_subs_id_e_type                  subs_indx
)
{
  /*---------------------------------------------------------------------------- 
    Since during every active bring up of keep alive manager we check if kamgr 
    iface is valid if already registered, then degerister for that iface as it 
    could possible that new iface will get allocated. so we have register on that 
    iface again with same callback buffers
  -----------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_WLAN_MAPCON
  DS_LTE_MSG0_MED("ds_eps_kamgr_dereg_iface_cbacks: Entered");

  if(ds_eps_kamgr[subs_indx]->ps_iface_down_ev_buf_ptr != NULL)
  {
    ps_iface_event_cback_dereg(this_iface_ptr,
                               IFACE_DOWN_EV,
                               ds_eps_kamgr[subs_indx]->ps_iface_down_ev_buf_ptr);
  }

  if(ds_eps_kamgr[subs_indx]->ps_iface_up_ev_buf_ptr != NULL)
  {
    ps_iface_event_cback_dereg(this_iface_ptr,
                               IFACE_UP_EV,
                               ds_eps_kamgr[subs_indx]->ps_iface_up_ev_buf_ptr);
  }
#endif /* FEATURE_DATA_WLAN_MAPCON */
} /* ds_eps_kamgr_dereg_iface_cbacks */

/*===========================================================================
FUNCTION  DS_EPS_KAMGR_BRING_UP_KEEP_ALIVE_PDN

DESCRIPTION
  This function is responsible for initiating the bring up of the PDN that
  has to be kept alive
  
PARAMETERS 
  None

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If bring up was successful
  FALSE: If bring up failed

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_eps_kamgr_bring_up_keep_alive_pdn 
(
  sys_modem_as_id_e_type  subs_id
)
{
#ifdef FEATURE_DATA_WLAN_MAPCON
  boolean                               bring_up_ret_val = FALSE;
  boolean                               is_keep_alive_pdn_up = FALSE;
  ds_pdn_context_s                      *pdn_cntxt_p = NULL;
  ds3g_apn_name_type                   *apn_name = NULL;
  ds_pkt_sys_mode_e_type                pref_mode = DS_PKT_SYS_MODE_NONE;
  ds_umts_pdp_profile_type               prof_context;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return bring_up_ret_val;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);
  
  DS_LTE_MSG0_LOW("ds_eps_kamgr_bring_up_keep_alive_pdn: Entered");

  do
  {
    if(ds_eps_kamgr[subs_indx] == NULL)            
    {
      DS_LTE_MSG0_ERROR("ds_eps_kamgr_bring_up_keep_alive_pdn: Keep alive "
                        "manager instance released");
      break;
    }

    /*------------------------------------------------------------------------- 
     1. Used to handle a case where due to APN name change to NULL APN,
     APN reconnect feature will tear down the Keep Alive PDN
     2. KAMGR will initiate retry mechanism, where after timer expiry this
     function will be called for bring-up
     3. Since KAMGR does not have support for NULL APN, we want to reject the
     bring-up
    --------------------------------------------------------------------------*/

    if (ds_eps_kamgr[subs_indx]->apn_name.apn_string_len <= 0)
    {
       DS_LTE_MSG1_ERROR ("ds_eps_kamgr_bring_up_keep_alive_pdn: Profile does not"
                          " exist or a case of NULL APN %d; Bring-up failure",
                          (ds_eps_kamgr[subs_indx]->apn_name).apn_string_len);
       break;
    }

    /*------------------------------------------------------------------------- 
     Suppose the Retry timer is running and the APN name is changed in the
     internal structure due to cback, then we need to check if LTE is still
     the preferred mode for the new APN
    --------------------------------------------------------------------------*/
    apn_name = &(ds_eps_kamgr[subs_indx]->apn_name);
    pref_mode = ds3g_get_apn_pref_pkt_mode_ex(apn_name,subs_id);

    if (pref_mode != DS_PKT_SYS_MODE_LTE)
    {
      DS_LTE_MSG1_ERROR("ds_eps_kamgr_bring_up_keep_alive_pdn: Pref mode is %d;"
                        " Not LTE",pref_mode);
      break;
    }
    /*-------------------------------------------------------------------------- 
     Check before iface-ptr before cback buffer allocation 
     if buffer alloc fails then we ensure that retry is initiated
     after some time interval
    ----------------------------------------------------------------------------*/
    if (ds_eps_kamgr[subs_indx]->ps_iface_ptr != NULL)
    {
      ds_eps_kamgr_dereg_iface_cbacks(ds_eps_kamgr[subs_indx]->ps_iface_ptr,subs_indx);
      ds_eps_kamgr[subs_indx]->ps_iface_ptr = NULL;
    }
    
    if(NULL == ds_eps_kamgr[subs_indx]->ps_iface_up_ev_buf_ptr)
    {
      ds_eps_kamgr[subs_indx]->ps_iface_up_ev_buf_ptr = ps_iface_alloc_event_cback_buf(
                                            ds_eps_kamgr_iface_up_ev_cback,
                                            NULL);

      if(NULL == ds_eps_kamgr[subs_indx]->ps_iface_up_ev_buf_ptr)
      {
        DS_LTE_MSG0_ERROR("ds_eps_kamgr_alloc_cback_buf: Unable to "
                          "allocate cback buffers");
        ds_eps_kamgr[subs_indx]->active_ind = DS_EPS_KAMGR_RESET;
        ds_eps_kamgr_retry_after_failure(subs_id, PS_NET_DOWN_REASON_INTERNAL_ERROR, TRUE);
        break;
      }
    }

    if(NULL == ds_eps_kamgr[subs_indx]->ps_iface_down_ev_buf_ptr)
    {
      ds_eps_kamgr[subs_indx]->ps_iface_down_ev_buf_ptr = ps_iface_alloc_event_cback_buf(
                                            ds_eps_kamgr_iface_down_ev_cback,
                                            NULL);

      if(NULL == ds_eps_kamgr[subs_indx]->ps_iface_down_ev_buf_ptr)
      {
        DS_LTE_MSG0_ERROR("ds_eps_kamgr_alloc_cback_buf: Unable to "
                          "allocate cback buffers");
        ds_eps_kamgr[subs_indx]->active_ind = DS_EPS_KAMGR_RESET;
        ps_iface_free_event_cback_buf((void *)ds_eps_kamgr[subs_indx]->ps_iface_up_ev_buf_ptr);
        ds_eps_kamgr[subs_indx]->ps_iface_up_ev_buf_ptr = NULL;
        ds_eps_kamgr_retry_after_failure(subs_id, PS_NET_DOWN_REASON_INTERNAL_ERROR, TRUE);
        break;
      }
    }
    /*-------------------------------------------------------------------------- 
    Performing an active bring-up to bring up the keep alive PDN, 
    In case the PDN is already up we change the status of keep alive flag to 
    true 
    ----------------------------------------------------------------------------*/
    PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
    
    is_keep_alive_pdn_up = ds_pdn_cntxt_is_pdn_associated_for_apn 
                                    ((byte *)(ds_eps_kamgr[subs_indx]->apn_name).apn_string,
                                      subs_id);

    if (!is_keep_alive_pdn_up)
    {
      memset(&prof_context, 0, sizeof(ds_umts_pdp_profile_type));
      ds_umts_get_pdp_profile_all_data_per_subs(
                                (ds_eps_kamgr[subs_indx]->config_info).profile_no,
                                dsumts_subs_mgr_get_subs_id(subs_id),
                                                &(prof_context));
       bring_up_ret_val = ds_eps_pdn_cntxt_allocate_pdn_and_bringup_ifaces
                          ((ds_eps_kamgr[subs_indx]->config_info).profile_no, 
                           &(ds_eps_kamgr[subs_indx]->ps_iface_ptr),
                           &prof_context,
                           subs_id);

       if(bring_up_ret_val == FALSE)
       {
         /*---------------------------------------------------------------------
          Since active bring-up failed we need to free the cback buffers
          allocated. Reset the state of keep alive manager and retry
         ----------------------------------------------------------------------*/
         PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
         DS_LTE_MSG0_ERROR("ds_eps_kamgr_bring_up_keep_alive_pdn: Active "
                           "bring-up failed, Freeing cback buffers allocated");
         ds_eps_kamgr[subs_indx]->ps_iface_ptr = NULL;
         ds_eps_kamgr_free_iface_cback_buf(subs_id);
         break;
       }
    }
    else
    {
       DS_LTE_MSG0_HIGH("ds_eps_kamgr_bring_up_keep_alive_pdn: Keep Alive PDN"
                      " already up, setting the Keep Alive PDN flag");
       is_keep_alive_pdn_up = ds_pdn_cntxt_get_v4_pdn_context(
                                (byte *)(ds_eps_kamgr[subs_indx]->apn_name).apn_string,
                                &pdn_cntxt_p,
                                subs_id);
       if((!is_keep_alive_pdn_up) || 
          (!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntxt_p)))
       {
         is_keep_alive_pdn_up = ds_pdn_cntxt_get_v6_pdn_context(
                                  (byte *)(ds_eps_kamgr[subs_indx]->apn_name).apn_string,
                                  &pdn_cntxt_p,
                                  subs_id);
       }
       if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntxt_p))
       {
         PS_LEAVE_CRIT_SECTION (&global_ps_crit_section);
         ds_eps_kamgr_free_iface_cback_buf (subs_id);
         break;
       }
       pdn_cntxt_p->ds_pdn_context_dyn_p->keep_alive_pdn = TRUE;
       ds_eps_kamgr[subs_indx]->ps_iface_ptr = DSPDNCNTXT_GET_V4_PS_IFACE_PTR(pdn_cntxt_p);
       if(ds_eps_kamgr[subs_indx]->ps_iface_ptr == NULL)
       {
         ds_eps_kamgr[subs_indx]->ps_iface_ptr = DSPDNCNTXT_GET_V6_PS_IFACE_PTR(pdn_cntxt_p);
       }
    }
    PS_LEAVE_CRIT_SECTION (&global_ps_crit_section);

    /*------------------------------------------------------------------------
      Register cbacks for IFACE down and IFACE UP event
    -------------------------------------------------------------------------*/
    if(ds_eps_kamgr[subs_indx]->ps_iface_ptr == NULL)
    {
      DS_LTE_MSG0_ERROR("No Iface allocated for Keep Alive Manager, Exit !!!");
      ds_eps_kamgr_free_iface_cback_buf(subs_id);
      break;
    }

    ds_eps_kamgr_setup_iface_event_cbacks(subs_id, ds_eps_kamgr[subs_indx]->ps_iface_ptr);

    ds_eps_kamgr[subs_indx]->companion_iface = ds_3gpp_pdn_get_companion_iface 
                                                    (ds_eps_kamgr[subs_indx]->ps_iface_ptr);
    DS_LTE_MSG0_LOW("De-registering from all Iface Up Event cback");
    ds_eps_kamgr_dereg_for_all_iface_up_ev_cback(subs_id); 
    bring_up_ret_val = TRUE;
  }while (0);

  DS_LTE_MSG1_HIGH("ds_eps_kamgr_bring_up_keep_alive_pdn, Value returned is %d",
                   bring_up_ret_val);

  return bring_up_ret_val;
#else
  DS_LTE_MSG0_HIGH ("ds_eps_kamgr_bring_up_keep_alive_pdn: MAPCON feature disabled");
  return FALSE;
#endif /* FEATURE_DATA_WLAN_MAPCON */
} /* ds_eps_kamgr_bring_up_keep_alive_pdn */

/*===========================================================================
FUNCTION DS_EPS_KAMGR_CHECK_WLAN_STATUS

DESCRIPTION
  This function process the system info given by MPPM to decide whether
  WLAN is available or not

PARAMETERS
  sys_info_ptr:                    System information reported by MPPM

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE: If WLAN is available
  FALSE: Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_kamgr_check_wlan_status
(
  ps_sys_system_status_ex_type             *sys_info_ptr
)
{
  boolean                                   retval = FALSE;
  uint8                                     index = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(sys_info_ptr == NULL)
  {
    DS_LTE_MSG0_ERROR("Invalid Input Params, Exiting !!!!");
    return retval;
  }

  for(index = 0; index < sys_info_ptr->num_avail_sys; index++)
  {
    if(PS_SYS_NETWORK_WLAN == sys_info_ptr->avail_sys[index].technology)
    {
      retval = TRUE;
      DS_LTE_MSG1_MED("WLAN Available, Entry match at index: %d", index);
    }
  }

  return retval;

}

/*===========================================================================
FUNCTION  DS_EPS_KAMGR_PS_CONF_STATUS_CBACK

DESCRIPTION
  This callback is invoked by PS when conf status is posted by MPPM.
  Specifically the current service status indication.
  
PARAMETERS
    tech_type:                       Technology for which event was registered
    event_name:                      Event that was registered
    subscription_id:                 As of now Default subscription_id
  * event_info_ptr:                  Event Info
  * user_data_ptr:                   Data passed by the user during registration
 

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_eps_kamgr_ps_conf_status_cback
(
  ps_sys_tech_enum_type                    tech_type,
  ps_sys_event_enum_type                   event_name,
  ps_sys_subscription_enum_type            subscription_id,
  void                                   * event_info_ptr,
  void                                   * user_data_ptr
)
{
#ifdef FEATURE_DATA_WLAN_MAPCON
  ds_cmd_type                            * cmd_ptr = NULL;
  ps_sys_system_status_ex_type           * sys_info_ptr = NULL;
  ds_eps_kamgr_sys_info_cmd_type          *data_ptr = NULL;
  sys_modem_as_id_e_type                   subs_id;
/*---------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------- 
    Sanity check done for input params
  ----------------------------------------------------------------------------*/

  if(event_info_ptr == NULL)
  {
    DS_LTE_MSG0_MED("ds_eps_kamgr_ps_conf_status_cback: Event info invalid");
    return;
  }

  if (PS_SYS_TECH_ALL != tech_type || 
      PS_SYS_EVENT_SYSTEM_STATUS_EX != event_name)
  {
    DS_LTE_MSG0_MED("ds_eps_kamgr_ps_conf_status_cback: Invalid Tech or Event");
    return;
  }

  subs_id = ds3gsubsmgr_subs_id_ds_to_cm(
              (ds_sys_subscription_enum_type)subscription_id);

  sys_info_ptr
    = (ps_sys_system_status_ex_type*)modem_mem_alloc(
                                       sizeof(ps_sys_system_status_ex_type),
                                       MODEM_MEM_CLIENT_DATA);

  if (sys_info_ptr == NULL)
  {
    DS_LTE_MSG0_ERROR("ds_eps_kamgr_ps_conf_status_cback: Unable to allocate"
                      " memeory on heap, Exiting !!!!");
    return;
  }
  
  memscpy (sys_info_ptr,sizeof(ps_sys_system_status_ex_type),
           event_info_ptr,sizeof(ps_sys_system_status_ex_type));

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_eps_kamgr_sys_info_cmd_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_EPS_KAMGR_SYS_INFO;
  data_ptr = (ds_eps_kamgr_sys_info_cmd_type*)cmd_ptr->cmd_payload_ptr;
  data_ptr->sys_info_ptr = sys_info_ptr;
  data_ptr->subs_id = subs_id;

  ds_put_cmd_ext(cmd_ptr);

#else
  DS_LTE_MSG0_LOW ("ds_eps_kamgr_ps_conf_status_cback: MAPCON feature "
                    "disabled");
#endif /* FEATURE_DATA_WLAN_MAPCON */
} /* ds_eps_kamgr_ps_conf_status_cback */

/*==============================================================================
FUNCTION  DS_EPS_KAMGR_DEALLOC_KAMGR_INSTANCE

DESCRIPTION
  This function is called to clean-up KAMGR global instance on an NV refresh
  or during KAMGR resource clean-up
  
PARAMETERS 
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

================================================================================*/
void ds_eps_kamgr_dealloc_kamgr_instance
(
   sys_modem_as_id_e_type  subs_id
)
{
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return;
  }

  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  if(ds_eps_kamgr[subs_indx] != NULL)
  {
    DS_LTE_MSG0_MED ("ds_eps_kamgr_clean_up_resources: Freeing Keep alive "
                     "manager instance");
    
    ds_profile_3gpp_deregister_callback_table(&ds_eps_kamgr[subs_indx]->
                                              ds_profile_kamgr_cb_func_tbl);

    modem_mem_free((void *)ds_eps_kamgr[subs_indx], MODEM_MEM_CLIENT_DATA);
    ds_eps_kamgr[subs_indx] = NULL;
  }

  return;
}/* ds_eps_kamgr_dealloc_kamgr_instance */

/*==============================================================================
FUNCTION  DS_EPS_KAMGR_CLEAN_UP_RESOURCES

DESCRIPTION
  This function is called to clean up all the resources associated with the keep
  alive manager
  
PARAMETERS 
  this_iface_ptr:                Iface associated with Keep alive manager

DEPENDENCIES
  None

RETURN VALUE
  TRUE:                          Clean up successful
  FALSE:                         Clean up failed

SIDE EFFECTS
  None

================================================================================*/
void ds_eps_kamgr_clean_up_resources 
(
  sys_modem_as_id_e_type  subs_id
)
{
#ifdef FEATURE_DATA_WLAN_MAPCON
  int16                            ps_errno = -1;
  int16                            ret_val = -1;
  int                              subs_index = PS_SYS_DEFAULT_SUBS;
  ds3geventmgr_filter_type         filter_info;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  /*--------------------------------------------------------------------------   
  At last we de-register from PS for system status notification and then clean 
  up the instance of keep alive manager 
  ----------------------------------------------------------------------------*/

  memset(&filter_info, 0, sizeof(ds3geventmgr_filter_type));

  for (subs_index = PS_SYS_PRIMARY_SUBS; 
        subs_index < PS_SYS_SUBS_MAX; subs_index++)
  {
    ret_val = ps_sys_event_dereg_ex(PS_SYS_TECH_ALL,
                                    PS_SYS_EVENT_SYSTEM_STATUS_EX,
                                    subs_index,
                                    ds_eps_kamgr_ps_conf_status_cback,
                                    &ps_errno);
    if(ret_val < 0)
    {
      DS_LTE_MSG2_HIGH ("ds_eps_kamgr_clean_up_resources, Error while "
                        "de-registering for PS_SYS_EVENT_SYSTEM_STATUS_EX event,"
                        " return value %d and error %d ",ret_val, ps_errno);
    }

    filter_info.ps_subs_id = subs_index;
    if (ds3geventmgr_event_dereg(DS3GEVENTMGR_CLIENT_ID_KAMGR,
                                 DS3GEVENTMGR_DATA_CHANGED_EV,
                                 &filter_info) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("De-Registration for DS3GEVENTMGR_DATA_CHANGED_EV"
                         " failed !");
    }

    if (ds3geventmgr_event_dereg(DS3GEVENTMGR_CLIENT_ID_KAMGR,
                                 DS3GEVENTMGR_DATA_ROAMING_CHANGED_EV,
                                 &filter_info) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("De-Registration for DS3GEVENTMGR_DATA_ROAMING_CHANGED_EV"
                         " failed !");
    }
  }

  ds_eps_kamgr_dealloc_kamgr_instance(subs_id);
#else
  DS_LTE_MSG0_LOW ("ds_eps_kamgr_clean_up_resources: MAPCON feature disabled");
#endif /* FEATURE_DATA_WLAN_MAPCON */
}

/*==============================================================================
FUNCTION DS_EPS_KAMGR_DEREG_IFACE_CBACKS_AND_FREE_CBACK_BUFFERS

DESCRIPTION
  This function is called to clean up all the cback resources associated with
  an iface
 
PARAMETERS 
  this_iface_ptr:          Iface whose call backs and buffers have to be cleaned
                           up

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

================================================================================*/
void ds_eps_kamgr_dereg_iface_cbacks_and_free_cback_buffers
(
  sys_modem_as_id_e_type   subs_id,
  ps_iface_type                                 *this_iface_ptr,
  boolean                                        clean_up
)
{
#ifdef FEATURE_DATA_WLAN_MAPCON
  ds3gsubsmgr_subs_id_e_type  subs_indx;
  ds3gsubsmgr_subs_id_e_type  indx;
  boolean                     iface_ev_ind_regd = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(this_iface_ptr == NULL)
  {
    DS_LTE_MSG0_ERROR("Invalid Input Params: Iface Ptr, Exiting !!!!");
    return;
  }
  /*---------------------------------------------------------------------------- 
    Since during every active bring up of keep alive manager we will be allocating
    a new iface so we free the iface resources every time the keep alive PDN is
    brought down
      - In case of Permanent cause code we also end up cleaning the only instance
        of Keep alive manager
  -----------------------------------------------------------------------------*/
  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return;
  }

  /*---------------------------------------------------------------------
    cache the registration status
  ---------------------------------------------------------------------*/
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);
  ds_eps_kamgr_iface_ev_regd[subs_indx] = FALSE;

  if(ds_eps_kamgr[subs_indx] == NULL)
  {
    DS_LTE_MSG0_ERROR("ds_eps_kamgr_dereg_iface_cbacks_and_free_cback_buffers: "
                      "Keep alive manager released or not allocated yet");
    return;
  }

  /*-------------------------------------------------------------------------
    Since Iface event registration is not per subscription, we would need to
    deregister only once. Check if all iface up event is still registered
  -------------------------------------------------------------------------*/
  for( indx = DS3GSUBSMGR_SUBS_ID_MIN; indx < DS3GSUBSMGR_SUBS_ID_MAX;
       indx++ )
  {
    if( ds_eps_kamgr_iface_ev_regd[indx] == TRUE )
    {
      iface_ev_ind_regd = TRUE;
      break;
    }
  }

  DS_LTE_MSG0_LOW("ds_eps_kamgr_clean_up_iface_cbacks: Entered");

  if( iface_ev_ind_regd == FALSE )
  {
    if(ds_eps_kamgr[subs_indx]->ps_iface_down_ev_buf_ptr != NULL)
  {
    ps_iface_event_cback_dereg(this_iface_ptr,
                               IFACE_DOWN_EV,
                                 ds_eps_kamgr[subs_indx]->ps_iface_down_ev_buf_ptr);
    DS_LTE_MSG0_MED("Freeing iface down event buffer");
      ps_iface_free_event_cback_buf(ds_eps_kamgr[subs_indx]->ps_iface_down_ev_buf_ptr);
      ds_eps_kamgr[subs_indx]->ps_iface_down_ev_buf_ptr = NULL;
  }
  else
  {
    DS_LTE_MSG0_MED("ds_eps_kamgr_dereg_iface_cbacks: Iface down "
                  "event buffer already released");
  }

    if(ds_eps_kamgr[subs_indx]->ps_iface_up_ev_buf_ptr != NULL)
  {
    ps_iface_event_cback_dereg(this_iface_ptr,
                               IFACE_UP_EV,
                                 ds_eps_kamgr[subs_indx]->ps_iface_up_ev_buf_ptr);
    DS_LTE_MSG0_MED("Freeing iface up event buffer");
      ps_iface_free_event_cback_buf(ds_eps_kamgr[subs_indx]->ps_iface_up_ev_buf_ptr);
      ds_eps_kamgr[subs_indx]->ps_iface_up_ev_buf_ptr = NULL;
  }
  else
  {
    DS_LTE_MSG0_LOW("ds_eps_kamgr_dereg_iface_cbacks: "
                  "iface up event buffer already released");
  }
  }

  ds_eps_kamgr[subs_indx]->ps_iface_ptr = NULL;

  if (clean_up == TRUE)
  {
    ds_eps_kamgr_clean_up_resources(subs_id);
  }
#else
  DS_LTE_MSG0_LOW ("ds_eps_kamgr_dereg_iface_cbacks_and_free_cback_buffers: "
                  "MAPCON feature disabled");
#endif /* FEATURE_DATA_WLAN_MAPCON */
}

/*===========================================================================
FUNCTION  DS_EPS_KAMGR_READ_KEEP_ALIVE_APN_DATA

DESCRIPTION
 Read the EFS file to get the Keep alive APN data.
  
PARAMETERS
 efs_data:                Buffer to store the file contents
 
DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If the read operation was successful
  FALSE: If the we could not read the efs file

SIDE EFFECTS
  None

===========================================================================*/
uint32 ds_eps_kamgr_read_keep_alive_apn_data
(
  sys_modem_as_id_e_type    subs_id,
  char  **efs_data
)
{
   char            item_file_path [] = DS_EPS_KAMGR_PDN;
   uint32          size = 0;
   struct fs_stat  temp_buf;
   mcfg_fs_status_e_type  get_result = MCFG_FS_STATUS_EFS_ERR;
/*------------------------------------------------------------------------*/
  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return size;
  }

   memset(&temp_buf, 0, sizeof(struct fs_stat));

   if( mcfg_fs_stat((const char*)item_file_path,
                    &temp_buf,
                    MCFG_FS_TYPE_EFS,
                    (mcfg_fs_sub_id_e_type)subs_id) !=  MCFG_FS_STATUS_OK)
   {
     DATA_MSG_SPRINTF_1(
        MSG_LEGACY_ERROR,
        "ds_eps_kamgr_read_keep_alive_apn_data:Error opening file %s",
        item_file_path);
   }
   else
   {
     *efs_data = (char*)ds_system_heap_mem_alloc(sizeof(char) * temp_buf.st_size);
     
     if(*efs_data == NULL)
     {
       DATA_MSG0_MED("ds_eps_kamgr_read_keep_alive_apn_data: "
                     "Memory allocation failed");
     }
     else
     {
       memset (*efs_data, 0, temp_buf.st_size);
       get_result = mcfg_fs_read((const char *)item_file_path,
                                 (void *)(*efs_data),
                                 temp_buf.st_size,
                                 MCFG_FS_TYPE_EFS,
                                 (mcfg_fs_sub_id_e_type)subs_id);
       if(get_result == MCFG_FS_STATUS_OK)
       {
         size = temp_buf.st_size;
         DATA_MSG_SPRINTF_2(MSG_LEGACY_HIGH,
                            "ds_eps_kamgr_read_keep_alive_apn_data:"
                            "data read from EFS %s and size is %d",
                            *efs_data, size);
       }
       else
       {
         DATA_MSG0_ERROR("EFS file for Keep Alive APN not read successfully");
         ds_system_heap_mem_free(*efs_data);
         *efs_data = NULL;
       }
     }
   }
   return size;
} /* ds_eps_kamgr_read_keep_alive_apn_data */

/*===========================================================================
FUNCTION  DS_EPS_KAMGR_ALL_IFACE_UP_EV_CBACK

DESCRIPTION
  This function gets called when iface up event is posted, called for all
  3GPP ifaces
  
PARAMETERS 
  this_iface_ptr:   Iface on which up event was posted
  event:            Event that caused the cback to be invoked
  event_info:       Event info posted by PS
  user_data_ptr:    Data to be passed to the cback function

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_eps_kamgr_all_iface_up_ev_cback
(
  ps_iface_type                           *this_iface_ptr,
  ps_iface_event_enum_type                 event,
  ps_iface_event_info_u_type               event_info,
  void                                    *user_data_ptr
)
{
#ifdef FEATURE_DATA_WLAN_MAPCON
  ds_cmd_type                            * cmd_ptr = NULL; 
  ds_eps_kamgr_iface_events_cmd_type     *data_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(this_iface_ptr == NULL)
  {
    DS_LTE_MSG0_ERROR("ALL Iface up event cback invoked with NULL iface");
    return;
  }
  /*----------------------------------------------------------------------- 
   This cback is used an indication saying bring-up was successful and to
   reset the retry mechanism counters
  ------------------------------------------------------------------------*/
  if(event != IFACE_UP_EV ||
     this_iface_ptr->name != UMTS_IFACE)
  {
    DS_LTE_MSG2_MED("Cback invoked for a wrong event or iface: %d or 0x%x", 
                    event, this_iface_ptr);
    return;
  }

  DS_LTE_MSG1_HIGH("ds_eps_kamgr_all_iface_up_ev_cback: Received cback for %d "
                   "event",event);

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_eps_kamgr_iface_events_cmd_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DS_LTE_MSG0_ERROR("CMD allocation failed");
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_EPS_KAMGR_IFACE_ALL_UP_EV_CB;
  data_ptr = (ds_eps_kamgr_iface_events_cmd_type*)cmd_ptr->cmd_payload_ptr;
  data_ptr->iface_ptr = this_iface_ptr;
  ds_put_cmd_ext(cmd_ptr);

#else
   DS_LTE_MSG0_HIGH ("ds_eps_kamgr_all_iface_up_ev_cback: MAPCON feature "
                     "disabled");
#endif /* FEATURE_DATA_WLAN_MAPCON */
}/*ds_eps_kamgr_all_iface_up_ev_cback*/

/*===========================================================================
FUNCTION  DS_EPS_KAMGR_REG_FOR_ALL_IFACE_UP_EV_CBACK

DESCRIPTION
  Registering for all 3GPP ifaces at boot-up so that once KAMGR PDN comes up,
  it is marked to stay active even after AP disconnects the call
  
PARAMETERS
  None
 
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_eps_kamgr_reg_for_all_iface_up_ev_cback
(
  sys_modem_as_id_e_type  subs_id
)
{
  ds3gsubsmgr_subs_id_e_type  subs_indx;
  boolean                     iface_ev_ind_regd = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if(ds_eps_kamgr_all_iface_up_ev_buf_ptr == NULL)
  {
    ds_eps_kamgr_all_iface_up_ev_buf_ptr = ps_iface_alloc_event_cback_buf(
                                          ds_eps_kamgr_all_iface_up_ev_cback,
                                          NULL);

    if(ds_eps_kamgr_all_iface_up_ev_buf_ptr != NULL)
    {
      /*-------------------------------------------------------------------------
        Since Iface event registration is not per subscription, we would need to
        register only once. Check if all iface up event is already registered
      -------------------------------------------------------------------------*/
      for( subs_indx = DS3GSUBSMGR_SUBS_ID_MIN; subs_indx < DS3GSUBSMGR_SUBS_ID_MAX;
           subs_indx++ )
      {
        if( ds_eps_kamgr_all_iface_up_regd[subs_indx] == TRUE )
        {
          iface_ev_ind_regd = TRUE;
          break;
        }
      }

      /*-------------------------------------------------------------------------
        Register for all iface up event only if WLAN config NV is turned ON and
        previously not registered
      -------------------------------------------------------------------------*/
      if( (DS_WLAN_OFFLOAD_CONFIG_IWLAN_S2B ==
           ds_wlan_get_wlan_offload_config_nv()) &&
          (iface_ev_ind_regd == FALSE) )
      {
        if( 0 != ps_iface_event_cback_reg(
                                          NULL,
                                          IFACE_UP_EV,
                                          ds_eps_kamgr_all_iface_up_ev_buf_ptr
                                          ) )
      {
          DS_LTE_MSG0_LOW("Registration failure for All iface up event cback");
        }
      }
      else
      {
        /*---------------------------------------------------------------------
          cache the registration status
        ---------------------------------------------------------------------*/
        if (iface_ev_ind_regd == TRUE)
        {
          if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
          {
            return;
          }
          subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);
          ds_eps_kamgr_all_iface_up_regd[subs_indx] = TRUE;
        }
      }
    }
    else
    {
      DS_LTE_MSG0_ERROR("Cback allocation buffer failed !!!!");
    }
  }
}/*ds_eps_kamgr_reg_for_all_iface_up_ev_cback*/

/*===========================================================================
FUNCTION  DS_EPS_KAMGR_IS_DATA_ALLOWED

DESCRIPTION
  This function checks the Data UI or Data Roaming Settings and returns a
  value saying Data is enabled or disabled 
  
PARAMETERS 
  device_setting_info_ptr: Data Setting from PS
  subs_id: Current active PS subs

DEPENDENCIES
  None

RETURN VALUE
  TRUE: If Data is available for use
  FALSE: If Data is disabled

SIDE EFFECTS
  None

===========================================================================*/
static boolean ds_eps_kamgr_is_data_allowed
(
  boolean                                 data_settings,
  boolean                                 roaming_settings,
  sys_modem_as_id_e_type                  subs_id
)
{
  boolean                                 data_allowed = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return data_allowed;
  }

  if(data_settings == TRUE)
  {
    if(roaming_settings == FALSE &&
       ds_3gpp_roaming_get_status(subs_id) == TRUE)
    {
      DS_LTE_MSG0_HIGH("Roaming data disallowed");
      data_allowed = FALSE;
    }
    else
    {
      DS_LTE_MSG0_MED("Data and Roaming settings enabled, no teardown needed");
    }
  }
  else
  {
    DS_LTE_MSG0_HIGH("Data UI disabled");
    data_allowed = FALSE;
  }

  return data_allowed;
}/*ds_eps_kamgr_get_data_disable_status*/

/*===========================================================================
FUNCTION  DS_EPS_KAMGR_DEVICE_EV_HDLR_CB

DESCRIPTION
  This function is called by DS3G as a part of DATA UI/Data Romaing/APN Change
  event
  
PARAMETERS 
  event_id: Event for which cback is invoked
  ps_filter_info: Filter containing TECH type & subs id
  event_info_ptr: Event info
  data_ptr: User passed data

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void ds_eps_kamgr_device_ev_hdlr_cb
(
  ds3geventmgr_event_type       event_id,
  ds3geventmgr_filter_type      *ps_filter_info_p,
  void                          *event_info_ptr,
  void                          *data_ptr
)
{
  ds3gdevmgr_device_settings_info_type   device_settings_info;
  sys_modem_as_id_e_type                 subs_id = SYS_MODEM_AS_ID_NONE;
    ds3gsubsmgr_subs_id_e_type            subs_indx;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    if ((event_info_ptr == NULL) || (ps_filter_info_p == NULL))
    {
      DS_LTE_MSG0_ERROR("event_info_ptr/ps_filter_info_p is NULL !");
      break;
    }

    DS_LTE_MSG1_HIGH("Processing ds_eps_kamgr_device_ev_hdlr_cb. Event: %d", 
                      event_id);

    subs_id = ds3gsubsmgr_subs_id_ds_to_cm(
                (ds_sys_subscription_enum_type)ps_filter_info_p->ps_subs_id);
    if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
    {
      return;
    }
    subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

    if(ds_eps_kamgr[subs_indx] == NULL)
    {
      DS_LTE_MSG0_ERROR("Keep ALive Manager Instance not active, No clean-up "
                        "needed");
      break;
    }

    if (event_id != DS3GEVENTMGR_DATA_CHANGED_EV &&
        event_id != DS3GEVENTMGR_DATA_ROAMING_CHANGED_EV)
    {
      DS_LTE_MSG1_ERROR("Invalid Event: %d received !",event_id);
      break;
    }

    memset((void*)&device_settings_info,0,
           sizeof(ds3gdevmgr_device_settings_info_type));

    /*----------------------------------------------------------------------- 
     This functions checks for 3 entities before deciding whether KAMGR needs
     to be torn down or not:
     1. Iface in not in USE (ref_cnt = 0)
     2. Data UI Disabled & Data Roaming (if UE is roaming) disabled
     3. Wi-Fi OFF
     
     If all the 3 conditions are satisfied then the PDN is torn down
    ------------------------------------------------------------------------*/

    if(ds_eps_kamgr[subs_indx]->ps_iface_ptr == NULL &&
       ds_eps_kamgr[subs_indx]->companion_iface == NULL)
    {
      DS_LTE_MSG0_HIGH("KAMGR iface not allocated yet,no check needed for"
                       " data settings !!!");
    }
    else
    {
      if(ds_eps_kamgr_check_for_pdn_teardown(subs_id))
      {
        ds_3gpp_pdn_cntx_teardown_apn((char *)ds_eps_kamgr[subs_indx]->apn_name.apn_string,
                                       subs_id, 
                                       PS_NET_DOWN_REASON_CLIENT_END);
      }
      else
      {
        DS_LTE_MSG0_HIGH("No forceful teardown of KAMGR PDN needed");
      }
    }
  }while(0);

}/*ds_eps_kamgr_device_ev_hdlr_cb*/

/*===========================================================================
FUNCTION  DS_EPS_KAMGR_EFS_TOKENIZER

DESCRIPTION
  This function matches the tags, PROFILE_ID and TIMER_VAL with the data
  provided in EFS config file
 
PARAMETERS 
  file_data;                 EFS file data
  start:                     Starting point of comparison
  end:                       End point of comparison
  no_tokens:                 Total number of tags

DEPENDENCIES
  None

RETURN VALUE
  TRUE:                      if parsing was successful
  FALSE:                     parsing failure

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_eps_kamgr_efs_tokenizer 
(
   char                             *file_data,
   uint32                            start,
   uint32                            end,
   uint32                            no_tokens
)
{
#ifdef FEATURE_DATA_WLAN_MAPCON
   uint32                            length = 0;
   boolean                           ret_value = FALSE;
/*-------------------------------------------------------------------------*/
   /*---------------------------------------------------------------------- 
    This function is used for case in-sensitive tag matching
   -----------------------------------------------------------------------*/
   if (no_tokens >= DS_EPS_KAMGR_MAX_EFS_TOKENS)
   {
     DS_LTE_MSG0_ERROR("ds_eps_kamgr_efs_tokenizer: Invalid Token Count");
     return ret_value;
   }

   if(end > start)
   {
    length = strlen (ds_eps_kamgr_tags[no_tokens]);
   }
   else
   {
     DS_LTE_MSG0_ERROR("ds_eps_kamgr_efs_tokenizer: Invalid Params Passed");
     return ret_value;
   }

   if(strncasecmp(ds_eps_kamgr_tags[no_tokens],(const char*) (file_data+start), 
                  length) == 0)
   {
     DS_LTE_MSG1_MED ("ds_eps_kamgr_efs_tokenizer: Token %d matched",
                      no_tokens+1);
     ret_value = TRUE;
   }

   DS_LTE_MSG1_MED ("ds_eps_kamgr_efs_tokenizer: Tag matching status %d",
                  ret_value);
   return ret_value;
#else
   DS_LTE_MSG0_LOW ("ds_eps_kamgr_efs_tokenizer: MAPCON feature disabled");
   return FALSE;
#endif /* FEATURE_DATA_WLAN_MAPCON */
}/*ds_eps_kamgr_efs_tokenizer*/

/*===========================================================================
FUNCTION  DS_EPS_KAMGR_FREE_KAMGR_DATA_BUFFER

DESCRIPTION
 Free the EFS file data reserved for Keep alive manager.
  
PARAMETERS
 efs_data:               Pointer to the Memory to be released
 
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_eps_kamgr_free_kamgr_data_buffer
(
   char  **efs_data
)
{
  if (*efs_data == NULL)
  {
    DS_LTE_MSG0_ERROR(
       "ds_eps_kamgr_free_kamgr_data_buffer: Memory already released");
    return;
  }

  ds_system_heap_mem_free(*efs_data);
  *efs_data = NULL;

  DS_LTE_MSG0_LOW(
     "ds_eps_kamgr_free_kamgr_data_buffer: Memory released successfully");
  return;
} /* ds_eps_kamgr_free_kamgr_data_buffer */

/*===========================================================================
FUNCTION  DS_EPS_KAMGR_GET_KEEP_ALIVE_APN_FROM_PROFILE

DESCRIPTION
  This function gets the Keep alive APN name from the profile number extracted
  from EFS file
 
PARAMETERS 
 None
 
DEPENDENCIES 
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_eps_kamgr_get_keep_alive_apn_from_profile
(
  sys_modem_as_id_e_type  subs_id
)
{
#ifdef FEATURE_DATA_WLAN_MAPCON
  ds_umts_pdp_profile_type                  *profile_info = NULL;
  ds_umts_pdp_profile_status_etype           result = DS_UMTS_PDP_FAIL;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  /*--------------------------------------------------------------------- 
    Thisn function is used to extract the APN associated with the profile
    passed as a part of EFS configurable file
  ----------------------------------------------------------------------*/
  do
  {
    profile_info = (ds_umts_pdp_profile_type *)modem_mem_alloc
      (sizeof(ds_umts_pdp_profile_type), MODEM_MEM_CLIENT_DATA);

    if(profile_info == NULL)
    {
      DS_LTE_MSG0_HIGH("ds_eps_kamgr_bring_up_keep_alive_pdn: Unable to "
                       "allocate memory for extracting profile"
                       " information");
      return;
    }

    memset(profile_info, 0, sizeof(ds_umts_pdp_profile_type));
    result = ds_umts_get_pdp_profile_all_data((ds_eps_kamgr[subs_indx]->config_info).profile_no, 
                                                profile_info);

    if (result != DS_UMTS_PDP_SUCCESS)
    {
      break;
    }
    else
    {
       (void)strlcpy((char *)(ds_eps_kamgr[subs_indx]->apn_name).apn_string, (const char*)
               (profile_info->context).apn, DS_UMTS_MAX_APN_STRING_LEN+1);
       (ds_eps_kamgr[subs_indx]->apn_name).apn_string_len = strlen ((const char*)
                                               (profile_info->context).apn); 
    }
  }while (0);

  modem_mem_free((void *)profile_info,MODEM_MEM_CLIENT_DATA);
  profile_info = NULL; 
#else
   DS_LTE_MSG0_LOW ("ds_eps_kamgr_get_keep_alive_apn_from_profile: MAPCON "
                     "feature disabled");
#endif /* FEATURE_DATA_WLAN_MAPCON */
}/*ds_eps_kamgr_get_keep_alive_apn_from_profile*/

/*===========================================================================
FUNCTION  DS_EPS_KAMGR_GET_PARSED_DATA

DESCRIPTION
  This function stores the parsed data in the keep alive manager instance
  for further use
 
PARAMETERS 
  element_no:                The element to be read
  element_data:              buffer to store the element data
  position:                  Postion in the file buffer to start the reading
  file_data:                 Buffer storing the efs file contents
  no_of_chars:               Number of characters to be read for the element

DEPENDENCIES 
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_eps_kamgr_get_parsed_data
(
  sys_modem_as_id_e_type               subs_id,
   ds_eps_kamgr_efs_element_enum_type    element_no,
   char                                * element_data,
   uint32                                position,
   char                                * file_data,
   uint16                                no_of_chars,
   ds_eps_kamgr_efs_info               * efs_info,
   uint32                                file_size
)
{
#ifdef FEATURE_DATA_WLAN_MAPCON
   uint16                            index = 0;
/*-------------------------------------------------------------------------*/
  
  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return;
  }

  while((index < no_of_chars) && (position + index < file_size))
  {
    element_data[index] = file_data[position+index];
    index++;
  }
  element_data [index] = '\0';

  switch (element_no)
  {
    case DS_EPS_KAMGR_EFS_PROFILE_NO:
      efs_info->profile_no = atoi((const char *)element_data);
      DS_LTE_MSG1_MED ("ds_eps_kamgr_get_parsed_data: Extracted profile "
                     "Number %d",efs_info->profile_no);

      if (ds_3gpp_profile_cache_get_cache_index(efs_info->profile_no,
          dsumts_subs_mgr_get_subs_id(subs_id)) == -1)
      {
        DS_LTE_MSG0_ERROR ("ds_eps_kamgr_get_parsed_data: Invalid Profile "
                         "passed, Resetting it to zero");
        efs_info->profile_no = 0;
      }
      break;

    case DS_EPS_KAMGR_EFS_DEFAULT_TIMER_VAL:
      efs_info->default_timer_val = atoi((const char *)element_data);
      DS_LTE_MSG1_MED ("ds_eps_kamgr_get_parsed_data: Extracted default timer"
                       " val %d", efs_info->default_timer_val); 
      break;

    case DS_EPS_KAMGR_EFS_INCREMENTAL_TIMER_VAL:
      efs_info->incremental_timer_val = atoi((const char *)element_data);
      DS_LTE_MSG1_MED ("ds_eps_kamgr_get_parsed_data: Extracted timer increment"
                       " val %d", efs_info->incremental_timer_val);
      break;
    
    case DS_EPS_KAMGR_EFS_MAXIMUM_TIMER_VAL:
      efs_info->maximum_timer_val = atoi((const char *)element_data);
      DS_LTE_MSG1_MED ("ds_eps_kamgr_get_parsed_data: Extracted Maximum Timer"
                       " val %d",efs_info->maximum_timer_val);
      break;

    case DS_EPS_KAMGR_EFS_MAXIMUM_RETRIES:
      efs_info->maximum_retries = atoi ((const char*)element_data);
      DS_LTE_MSG1_MED ("ds_eps_kamgr_get_parsed_data: Extracted Maximum retry"
                       " count %d",efs_info->maximum_retries);
      break;

    default:
      DS_LTE_MSG0_ERROR("ds_eps_kamgr_get_parsed_data: Invalid element number");
      break;
  }
#else
   DS_LTE_MSG0_LOW ("ds_eps_kamgr_get_parsed_data: MAPCON feature disabled");
#endif /* FEATURE_DATA_WLAN_MAPCON */
}/*ds_eps_kamgr_get_parsed_data*/

/*===========================================================================
FUNCTION DS_EPS_KAMGR_LOCAL_CLEAN_UP

DESCRIPTION
  This function cleans up resources w.r.t Keep Alive Manager when Keep Alive
  PDN is being used by secondary client and additional PDN is up 

PARAMETERS
  pdn_context_ptr :             PDN context of Keep Alive PDN

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_kamgr_local_clean_up
(
  sys_modem_as_id_e_type  subs_id
)
{
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);  

  if(ds_eps_kamgr[subs_indx] == NULL)
  {
    DS_LTE_MSG0_ERROR("ds_eps_kamgr_local_clean_up: Keep alive "
                      "manager instance released");
    return;
  }

  /*---------------------------------------------------------------------- 
    Check whether Data UI is disabled
    If disabled check whether Iface is in USE by other client
    If Data is disabled and iface is not in use, trigger OTA tear-down
  -----------------------------------------------------------------------*/
  if(ds_eps_kamgr_check_for_pdn_teardown(subs_id))
  {
    ds_3gpp_pdn_cntx_teardown_apn((char *)ds_eps_kamgr[subs_indx]->apn_name.apn_string,
                                   subs_id, 
                                   PS_NET_DOWN_REASON_CLIENT_END);
  }

  ds_eps_kamgr[subs_indx]->active_ind = DS_EPS_KAMGR_RESET;

}/*ds_eps_kamgr_local_clean_up*/

/*===========================================================================
FUNCTION  DS_EPS_KAMGR_HANDLE_WLAN_DISABLE_IND

DESCRIPTION
  This function takes the required actions on WLAN Disable indication; tearing
  down the Keep Alive PDN if needed 
  
PARAMETERS 
  None

DEPENDENCIES
  None

RETURN VALUE
  TRUE: If the required actions were taken successfully
  FALSE: Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_eps_kamgr_handle_wlan_disable_ind
(
  sys_modem_as_id_e_type  subs_id
)
{
  boolean                                   retval = FALSE;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return retval;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  do
  {
    if(ds_eps_kamgr[subs_indx] == NULL)
    {
      DS_LTE_MSG0_ERROR("ds_eps_kamgr_handle_wlan_disable_ind: Keep alive "
                        "manager instance released");
      break;
    }

    ds_eps_kamgr_local_clean_up(subs_id);
    retval = TRUE;

  }while(0);

  return retval;
}/*ds_eps_kamgr_handle_wlan_disable_ind*/

/*===========================================================================
FUNCTION  DS_EPS_KAMGR_IFACE_DOWN_EV_CBACK

DESCRIPTION
  This function gets called when iface down event is posted
  
PARAMETERS 
  this_iface_ptr:   Iface on which down event was posted
  event:            Event that caused the cback to be invoked
  event_info:       Event info posted by PS
  user_data_ptr:    Data to be passed to the cback function

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_eps_kamgr_iface_down_ev_cback
(
  ps_iface_type                           *this_iface_ptr,
  ps_iface_event_enum_type                 event,
  ps_iface_event_info_u_type               event_info,
  void                                    *user_data_ptr
)
{
#ifdef FEATURE_DATA_WLAN_MAPCON
  ds_cmd_type                        * cmd_ptr = NULL;
  ds_eps_kamgr_iface_events_cmd_type  *data_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(event != IFACE_DOWN_EV )
  {
    DS_LTE_MSG1_ERROR("Cback invoked for a wrong event; %d", event);
    return;
  }

  if(this_iface_ptr == NULL)
  {
    DS_LTE_MSG0_ERROR("Iface not valid as a part of IFACE DOWN Ev cback");
    return;
  }

  DS_LTE_MSG2_HIGH("ds_eps_kamgr_iface_down_ev_cback: Received cback for %d "
                   "event with down reason as 0x%x",event,
                   event_info.iface_down_info.netdown_reason);

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_eps_kamgr_iface_events_cmd_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_EPS_KAMGR_IFACE_DOWN_EV_CB;
  data_ptr = (ds_eps_kamgr_iface_events_cmd_type*)cmd_ptr->cmd_payload_ptr;

  memscpy (&(data_ptr->event_info),
           sizeof(ps_iface_event_info_u_type),
           &event_info, sizeof(ps_iface_event_info_u_type));
  data_ptr->iface_ptr = (void *)this_iface_ptr;
  ds_put_cmd_ext(cmd_ptr);

#else
   DS_LTE_MSG0_HIGH ("ds_eps_kamgr_iface_down_ev_cback: MAPCON feature "
                     "disabled");
#endif /* FEATURE_DATA_WLAN_MAPCON */
}/*ds_eps_kamgr_iface_down_ev_cback*/

/*===========================================================================
FUNCTION  DS_EPS_KAMGR_IFACE_UP_EV_CBACK

DESCRIPTION
  This function gets called when iface up event is posted
  
PARAMETERS 
  this_iface_ptr:   Iface on which up event was posted
  event:            Event that caused the cback to be invoked
  event_info:       Event info posted by PS
  user_data_ptr:    Data to be passed to the cback function

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void ds_eps_kamgr_iface_up_ev_cback
(
  ps_iface_type                           *this_iface_ptr,
  ps_iface_event_enum_type                 event,
  ps_iface_event_info_u_type               event_info,
  void                                    *user_data_ptr
)
{
#ifdef FEATURE_DATA_WLAN_MAPCON
  ds_cmd_type                       * cmd_ptr = NULL;  
  ds_eps_kamgr_iface_events_cmd_type  *data_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*----------------------------------------------------------------------- 
   This cback is used an indication saying bring-up was successful and to
   reset the retry mechanism counters
  ------------------------------------------------------------------------*/
  if(event != IFACE_UP_EV )
  {
    DS_LTE_MSG1_MED("Cback invoked for a wrong event; %d", event);
    return;
  }

  if(this_iface_ptr == NULL)
  {
    DS_LTE_MSG0_ERROR("Invalid Input Params: Iface Info, Exiting !!!!");
    return;
  }

  DS_LTE_MSG1_HIGH("ds_eps_kamgr_iface_up_ev_cback: Received cback for %d "
                   "event",event);

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_eps_kamgr_iface_events_cmd_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_EPS_KAMGR_IFACE_UP_EV_CB;
  data_ptr = (ds_eps_kamgr_iface_events_cmd_type*)cmd_ptr->cmd_payload_ptr;
  data_ptr->iface_ptr = this_iface_ptr;
  ds_put_cmd_ext(cmd_ptr);

#else
   DS_LTE_MSG0_HIGH ("ds_eps_kamgr_iface_up_ev_cback: MAPCON feature "
                     "disabled");
#endif /* FEATURE_DATA_WLAN_MAPCON */
}/*ds_eps_kamgr_iface_up_ev_cback*/

/*===========================================================================
FUNCTION  DS_EPS_KAMGR_KEEP_ALIVE_PDN_TIMER_CB

DESCRIPTION
  This function gets called when keep alive pdn retry timer expires,
  we now initate a new request for bring up of keep alive PDN
  
PARAMETERS 
  cback_param:                  Parameter to be passed to the cback function

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_eps_kamgr_keep_alive_pdn_timer_cb 
(
  unsigned long                               cback_param
)
{
#ifdef FEATURE_DATA_WLAN_MAPCON
  ds_cmd_type                             * cmd_ptr = NULL;
  ds_eps_kamgr_timer_cmd_type  *data_ptr = NULL;
/*--------------------------------------------------------------------------*/
  DS_LTE_MSG0_MED("Keep alive PDN Timer cback : Entered");

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_eps_kamgr_timer_cmd_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_EPS_KAMGR_TIMER_EXPIRY_CB;
  data_ptr = (ds_eps_kamgr_timer_cmd_type*)cmd_ptr->cmd_payload_ptr;
  data_ptr->subs_id
    = ds3gsubsmgr_subs_id_ds3g_to_cm(
       DS_EPS_KAMGR_SUBS_ID_FROM_SUBS_TIMER_ID(cback_param));
  data_ptr->timer_id
    = DS_EPS_KAMGR_TIMER_ID_FROM_SUBS_TIMER_ID(cback_param,
                                               data_ptr->subs_id);
  ds_put_cmd_ext(cmd_ptr);

#else
   DS_LTE_MSG0_LOW ("ds_eps_kamgr_keep_alive_pdn_timer_cb: MAPCON feature "
                     "disabled");
#endif /* FEATURE_DATA_WLAN_MAPCON */
}/*ds_eps_kamgr_keep_alive_pdn_timer_cb*/

/*===========================================================================
FUNCTION  DS_EPS_KAMGR_PARSE_EFS_DATA

DESCRIPTION
  This function parses the data read from the configurable efs file to extract
  profile number and timer values 
 
PARAMETERS 
  efs_data_ptr:              Buffer containing data read from efs file
  file_size:                 file size in bytes

DEPENDENCIES
  None

RETURN VALUE
  TRUE:                      if parsing was successful
  FALSE:                     parsing failure

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_eps_kamgr_parse_efs_data 
(
  sys_modem_as_id_e_type       subs_id,
   char                           *efs_data_ptr,
   uint32                          file_size,
   ds_eps_kamgr_efs_info          *efs_info
)
{
#ifdef FEATURE_DATA_WLAN_MAPCON
   uint32                       index = 0;
   uint32                       position = 0;  //Track position of ':'
   uint16                       no_of_chars = 0; //No. of chars read after ':'
   char                        *data = NULL; 
   uint8                        data_element_no = 0;
   boolean                      ret_val = FALSE;
   uint32                       token_start = 0, token_end = 0, no_tokens = 0;
/*---------------------------------------------------------------------------*/

  do
  {
    /*----------------------------------------------------------------------- 
     EFS File Format:
     PROFILE_ID:2;
     TIMER_VAL:500:600:10000:2;
     
     The Tags PROFILE_ID and TIMER_VAL are not case sensitive
     
     500: Default Timer Value
     600: Incremental Timer Value, Increments to be added every time
     10000: Maximum Timer value
     2: Maximum Number of Retries
    ------------------------------------------------------------------------*/
    if(efs_data_ptr == NULL || file_size == 0 || efs_info == NULL)
    {
      DS_LTE_MSG0_ERROR ("ds_eps_kamgr_parse_efs_data: Invalid Input Parameter");
      break;
    }
    /*--------------------------------------------------------------------------- 
     The data in the buffer is read till we encounter a ':', this indicates start
     of useful data (profile number or timer value)
     We read the data till we encounter a ';' (line delimiter) or a ':' (next info)
    --------------------- --------------------------------------------------------*/
    for(index = 0; index < file_size; index++)
    {
      if(index == 0)
      {
        token_start = index;
      }
      else if (efs_data_ptr[index - 1] == '\n')
      {
        token_start = index;
      }
      if(efs_data_ptr[index] == ':')
      {
        token_end = index - 1;
        if(no_tokens < 2)
        {
         ret_val = ds_eps_kamgr_efs_tokenizer (efs_data_ptr, token_start, 
                                           token_end, no_tokens);
         no_tokens++;
        }
        if(ret_val == FALSE)
        {
         DS_LTE_MSG0_ERROR ("ds_eps_kamgr_parse_efs_data: Tag Matching Failed");
         break;
        }
        token_start = 0;
        token_end = 0;
        no_of_chars = 0;
        index++; position = index;
        while(index < file_size && efs_data_ptr[index] != ';' && 
              efs_data_ptr[index] != ':')
        {
         index++;
         no_of_chars++;
        }
        if(no_of_chars > 0)
        {
         data_element_no++;
         data = (char *)modem_mem_alloc(sizeof(char)*(no_of_chars+1),
                                        MODEM_MEM_CLIENT_DATA);
         if(data == NULL)
         {
           DS_LTE_MSG1_LOW ("ds_eps_kamgr_parse_efs_data: Unable to allocate %d"
                              " bytes also", (no_of_chars+1));
           ret_val = FALSE;
           break;
         }

         ds_eps_kamgr_get_parsed_data (subs_id, data_element_no, data, position, 
                               efs_data_ptr, no_of_chars, efs_info, file_size);
         modem_mem_free((void *)data, MODEM_MEM_CLIENT_DATA);
         data = NULL; 
        }
        index--;
        ret_val = TRUE;
      }
    }    
  }while (0);

  return ret_val;
#else
   DS_LTE_MSG0_LOW ("ds_eps_kamgr_parse_efs_data: MAPCON feature disabled");
   return FALSE;
#endif /* FEATURE_DATA_WLAN_MAPCON */
}/*ds_eps_kamgr_parse_efs_data*/

/*===========================================================================
FUNCTION DS_EPS_KAMGR_PROCESS_SYSTEM_STATUS_INFO

DESCRIPTION
  This function process the system status information recieved and
  decides whether LTE is preferred system or not, if it is we bring up the
  keep alive PDN
 
PARAMETERS
  tech:                            Technology (3gpp/3gpp2)
  rat_val:                         RAT value
  so_mask:                         Mask
 
DEPENDENCIES
  None.

RETURN VALUE  
  TRUE:                            If bring-up successful
  FALSE:                           If bring-up failed

SIDE EFFECTS

===========================================================================*/
boolean ds_eps_kamgr_process_system_status_info
(
  sys_modem_as_id_e_type    subs_id,
  ps_sys_network_enum_type                 tech,
  ps_sys_rat_ex_enum_type                  rat_val 
)
{
#ifdef FEATURE_DATA_WLAN_MAPCON
  boolean                                  bring_up_ret_val = FALSE;
  ps_iface_type                           *companion_iface = NULL;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return bring_up_ret_val;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  if(ds_eps_kamgr[subs_indx] == NULL)
  {
    DS_LTE_MSG0_ERROR("ds_eps_kamgr_process_system_status_info: Keep alive"
                      " manager not active; Error");
    return bring_up_ret_val;
  }
 /*------------------------------------------------------------------------- 
    Case 1:
    WLAN available and LTE in full service, we bring up Keep Alive PDN
  --------------------------------------------------------------------------*/
  if(ds_eps_kamgr[subs_indx]->wlan_availability == TRUE)
  {
    if(tech == PS_SYS_NETWORK_3GPP && rat_val == PS_SYS_RAT_EX_3GPP_LTE )
    {
      DS_LTE_MSG0_MED("LTE full service reported");
      ds_eps_kamgr[subs_indx]->lte_srv_status = TRUE;
      /*------------------------------------------------------------------------ 
        If Keep alive PDN is not up and no back-off timer is in progress, then
        we initiate bring-up of keep alive PDN
      -------------------------------------------------------------------------*/
      if(ds_eps_kamgr[subs_indx]->active_ind != DS_EPS_SET_KAMGR && 
         (!(rex_timer_cnt_type)rex_get_timer(&(ds_eps_kamgr[subs_indx]->keep_alive_bring_up_timer))))
      {
        DS_LTE_MSG0_MED("Keep alive manager not active, Initiating bring up of "
                        "Keep alive PDN");
        bring_up_ret_val = ds_eps_kamgr_bring_up_keep_alive_pdn (subs_id);
        if(bring_up_ret_val == TRUE)
        {
          ds_eps_kamgr[subs_indx]->active_ind = DS_EPS_SET_KAMGR;
        }
      }
      else
      {
        DS_LTE_MSG0_MED("ds_eps_kamgr_process_conf_status_info: Either Keep alive"
                        " manager is active or Keep alive timer is running");
      }
    }
    else
    {
      DS_LTE_MSG0_MED("LTE not the preferrd mode for Keep Alive APN");
      ds_eps_kamgr[subs_indx]->lte_srv_status = FALSE;
    }
  }

  /*------------------------------------------------------------------------- 
    Case 2:
    Non-LTE (UMTS or 3GPP2) in full service, we bring down the Keep Alive PDN
  --------------------------------------------------------------------------*/
  if( (ds_eps_kamgr[subs_indx]->active_ind != DS_EPS_KAMGR_RESET) && 
      (tech == PS_SYS_NETWORK_3GPP) &&
      (rat_val != PS_SYS_RAT_EX_3GPP_LTE) && 
      (rat_val != PS_SYS_RAT_EX_NULL_BEARER) )
  {
    /*------------------------------------------------------------------------- 
      Sys mode has changed from LTE, so we force a tear-down of Keep alive PDN
      if it does not have any active clients.
      Using an already existing function which checks whether any iface is not
      in use before tearing down the PDN context
    --------------------------------------------------------------------------*/
    DS_LTE_MSG0_HIGH("ds_eps_kamgr_process_system_status_info: RAT is no longer"
                   " LTE,tearing down Keep Alive PDN if iface is not in use");

    companion_iface = ds_3gpp_pdn_get_companion_iface (ds_eps_kamgr[subs_indx]->ps_iface_ptr);

    if(PS_IFACE_IS_IN_USE (ds_eps_kamgr[subs_indx]->ps_iface_ptr) == FALSE &&
       PS_IFACE_IS_IN_USE (companion_iface) == FALSE) 
    {
      ds_3gpp_pdn_cntx_teardown_apn((char *)ds_eps_kamgr[subs_indx]->apn_name.apn_string,
                                    subs_id,
                                    PS_NET_DOWN_REASON_UNPREFERRED_RAT);
    }
    ds_eps_kamgr[subs_indx]->lte_srv_status = FALSE;
  }
  else if (ds_eps_kamgr[subs_indx]->active_ind !=  DS_EPS_KAMGR_RESET && 
           tech == PS_SYS_NETWORK_3GPP2)
  {
    companion_iface = ds_3gpp_pdn_get_companion_iface (ds_eps_kamgr[subs_indx]->ps_iface_ptr);
    if(PS_IFACE_IS_IN_USE (ds_eps_kamgr[subs_indx]->ps_iface_ptr) == FALSE &&
       PS_IFACE_IS_IN_USE (companion_iface) == FALSE) 
    {
      ds_eps_kamgr_dereg_iface_cbacks_and_free_cback_buffers(subs_id, 
                                                             ds_eps_kamgr[subs_indx]->ps_iface_ptr,
                                                             FALSE);
      ds_eps_kamgr[subs_indx]->active_ind = DS_EPS_KAMGR_RESET;
    
      DS_LTE_MSG0_HIGH("ds_eps_kamgr_process_system_status_info: RAT changed to"
                       " 3GPP2,tearing down Keep Alive PDN");
    }
    else
    {
      DS_LTE_MSG0_LOW ("ds_eps_kamgr_process_system_status_info: Iface still"
                       " in USE, so clean-up not attempted now");
    }
    ds_eps_kamgr[subs_indx]->lte_srv_status = FALSE;
  }
  /*------------------------------------------------------------------------- 
    Case 3:
    WLAN is no longer available so we tear down the PDN
  --------------------------------------------------------------------------*/
  else
  {
    if(ds_eps_kamgr[subs_indx]->wlan_availability == FALSE && tech == PS_SYS_NETWORK_3GPP &&
       ds_eps_kamgr[subs_indx]->active_ind != DS_EPS_KAMGR_RESET)
    {
      DS_LTE_MSG0_MED("WLAN no longer available so tearing down Keep Alive PDN");
      bring_up_ret_val = ds_eps_kamgr_handle_wlan_disable_ind (subs_id);
    }
  }

  return bring_up_ret_val;
#else
   DS_LTE_MSG0_LOW ("ds_eps_kamgr_process_system_status_info: MAPCON feature "
                     "disabled");
   return FALSE;
#endif /* FEATURE_DATA_WLAN_MAPCON */
}/*ds_eps_kamgr_process_system_status_info*/

/*===========================================================================
FUNCTION DS_EPS_KAMGR_USE_DSD_PREF_SYS_FOR_BRING_UP

DESCRIPTION
  This function uses the DSD provided sys mode information to decide whether
  LTE is available or not. If LTE is available, it attempts to bring-up the
  Keep alive PDN
 
PARAMETERS
  conf_status:                   System information received from PS_SYS event

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE:                           If bring-up successful
  FALSE:                          If bring-up failed

SIDE EFFECTS
  None
===========================================================================*/

boolean ds_eps_kamgr_use_dsd_pref_sys_for_bring_up
(
   sys_modem_as_id_e_type         subs_id,
   ps_sys_system_status_ex_type           *conf_status
)
{
#ifdef FEATURE_DATA_WLAN_MAPCON
  uint32                                   num_avail_sys = 0; 
  ps_sys_network_enum_type                 tech = PS_SYS_NETWORK_MAX; 
  ps_sys_rat_ex_enum_type                  rat_val = PS_SYS_RAT_MAX;  
  uint64                                   so_mask = PS_SYS_SO_EX_UNSPECIFIED;
  boolean                                  bring_up_ret_val = FALSE;
/*---------------------------------------------------------------------------*/
 
  if(conf_status == NULL)
  {
    DS_LTE_MSG0_ERROR("ds_eps_kamgr_use_dsd_pref_sys_for_bring_up: Invalid "
                      "Input Parameter");
  }
  else
  {
    num_avail_sys = conf_status->num_avail_sys;
      
    if(num_avail_sys == 0)
    {
      DS_LTE_MSG0_ERROR("ds_eps_kamgr_use_dsd_pref_sys_for_bring_up: Invalid "
                        "data passed; No preferred system reported via DSD");
    }
    else
    {
      tech = conf_status->avail_sys[PREF_RAT_INDEX].technology;
      rat_val = conf_status->avail_sys[PREF_RAT_INDEX].rat_value;
      so_mask = conf_status->avail_sys[PREF_RAT_INDEX].so_mask;

      DS_LTE_MSG3_MED("ds_eps_kamgr_use_dsd_pref_sys_for_bring_up: Technology: "
                      "%d, Rat_Mask: %d, So_Mask: %d",tech,rat_val,so_mask);

      bring_up_ret_val = ds_eps_kamgr_process_system_status_info (
                                                                  subs_id,
                                                                  tech, 
                                                                  rat_val
                                                                  );
    }
  }

  return bring_up_ret_val;
#else
   DS_LTE_MSG0_LOW ("ds_eps_kamgr_use_dsd_pref_sys_for_bring_up: MAPCON "
                     "feature disabled");
   return FALSE;
#endif /* FEATURE_DATA_WLAN_MAPCON */
}/*ds_eps_kamgr_use_dsd_pref_sys_for_bring_up*/

/*===========================================================================
FUNCTION DS_EPS_KAMGR_SETUP_IFACE_DATA

DESCRIPTION
  This function sets up the iface resources for cback on IFACE UP and
  IFACE DOWN events

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_kamgr_setup_iface_data
(
  sys_modem_as_id_e_type              subs_id,
  ps_iface_type                       *iface_ptr
)
{
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  do
  {
    if(iface_ptr == NULL)
    {
      DS_LTE_MSG0_ERROR("Invalid Input Params: Iface, Exiting !!!!");
      break;
    }
    ds_eps_kamgr[subs_indx]->ps_iface_ptr = iface_ptr;

    /*---------------------------------------------------------------------- 
      ps_iface_ptr is now storing the companion iface so we clean-up companion
      iface ptr
    -----------------------------------------------------------------------*/
    ds_eps_kamgr[subs_indx]->companion_iface = NULL;

    if(ds_eps_kamgr_alloc_cback_buf(subs_id) == FALSE)
    {
      DS_LTE_MSG0_ERROR("cback buffer allocation failures, breaking out !!!");
      break;
    }

    /*------------------------------------------------------------------------
      Register cbacks for IFACE down and IFACE UP change event
    -------------------------------------------------------------------------*/
    ds_eps_kamgr_setup_iface_event_cbacks(subs_id, ds_eps_kamgr[subs_indx]->ps_iface_ptr);

    ds_eps_kamgr[subs_indx]->companion_iface = ds_3gpp_pdn_get_companion_iface 
                                                    (ds_eps_kamgr[subs_indx]->ps_iface_ptr);
  }while (0);

}/*ds_eps_kamgr_setup_iface_data*/

/*===========================================================================
FUNCTION DS_EPS_KAMGR_PROCESS_IFACE_DOWN_EV_CBACK

DESCRIPTION
  This function process the iface down event and decides whether Keep alive
  Manager can re-attempt to bring up the PDN or not
  
PARAMETERS 
  event_info:                    Iface information reported by PS
  user_data:                     User data passed by the function posting cmd

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void ds_eps_kamgr_process_iface_down_ev_cback
(
  void  *cmd_payload_ptr
)
{
#ifdef FEATURE_DATA_WLAN_MAPCON
  ps_sys_ioctl_pdn_throttle_info_type      *pdn_throttle_info = NULL;
  int16                                    ps_errno = 0;
  ds_pkt_sys_mode_e_type                   current_mode = 
                                                      DS_PKT_SYS_MODE_NONE;
  boolean                                  permanent_failure = FALSE;
  ps_iface_type                           *iface_ptr = NULL;
  ds_eps_kamgr_iface_events_cmd_type   *data_ptr = NULL;
  sys_modem_as_id_e_type                   subs_id = SYS_MODEM_AS_ID_NONE;
  ds3gsubsmgr_subs_id_e_type            subs_indx;
  ps_iface_event_info_u_type            iface_down_event_info;
  ds_pdn_context_s                        *pdn_context_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  data_ptr = (ds_eps_kamgr_iface_events_cmd_type*)cmd_payload_ptr;
  iface_ptr = data_ptr->iface_ptr;
  memscpy(&iface_down_event_info,
          sizeof(ps_iface_event_info_u_type),
          &(data_ptr->event_info),
          sizeof(ps_iface_event_info_u_type));

  if( !PS_IFACE_IS_VALID(iface_ptr) )
  {
    return;
  }

  subs_id = ds3gsubsmgr_subs_id_ds_to_cm(PS_IFACE_GET_SUBS_ID(iface_ptr));
  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  if( (ds_eps_kamgr[subs_indx] == NULL) ||
      (ds_eps_kamgr_iface_ev_regd[subs_indx] != TRUE))
  {
    DS_LTE_MSG0_ERROR("ds_eps_kamgr_process_iface_down_ev_cback: Keep alive"
                      " manager released or not allocated yet");
    return;
  }

  /*------------------------------------------------------------------------ 
    Here on receiving the IFACE DOWN event cback
    1. We register for IFACE UP event cback for all 3GPP ifaces
    2. We check whether we need to clean-up KAMGR resources depending on
       the value of flag indicating active status of KAMGR
  -------------------------------------------------------------------------*/
  if(ds_eps_kamgr[subs_indx]->ps_iface_ptr != iface_ptr)
  {
    DS_LTE_MSG2_MED("Cback invoked for a wrong iface: 0x%x instead of:0x%x",
                     iface_ptr, ds_eps_kamgr[subs_indx]->ps_iface_ptr);
    return;
  }

  if(ds_eps_kamgr[subs_indx]->active_ind == DS_EPS_KAMGR_RESET)
  {
    DS_LTE_MSG1_HIGH("Forceful teardown of KAMGR PDN before kick-in: %d ", 
                     ds_eps_kamgr[subs_indx]->active_ind);
    ds_eps_kamgr_reg_for_all_iface_up_ev_cback(subs_id);
    ds_eps_kamgr_dereg_iface_cbacks_and_free_cback_buffers(subs_id,
                                                           iface_ptr, 
                                                           permanent_failure);
    return;
  }

  if(ds_eps_kamgr[subs_indx]->companion_iface != NULL &&
     PS_IFACE_IS_UP(ds_eps_kamgr[subs_indx]->companion_iface) == TRUE)
  {
    DS_LTE_MSG1_HIGH ("Companion Iface: 0x%x is UP, don't initiate retry "
                      "mechanism", ds_eps_kamgr[subs_indx]->companion_iface);
    ds_eps_kamgr_dereg_iface_cbacks_and_free_cback_buffers(subs_id,
                                                  ds_eps_kamgr[subs_indx]->ps_iface_ptr,
                                                  permanent_failure);
    ds_eps_kamgr_setup_iface_data (subs_id, ds_eps_kamgr[subs_indx]->companion_iface);
    return;
  }

  pdn_throttle_info = (ps_sys_ioctl_pdn_throttle_info_type *)modem_mem_alloc
    (sizeof(ps_sys_ioctl_pdn_throttle_info_type), MODEM_MEM_CLIENT_DATA);

  if (pdn_throttle_info == NULL)
  {
    DS_LTE_MSG0_ERROR("ds_eps_kamgr_process_iface_down_ev_cback: Unable to "
                      "allocate memory for throttle information");
    return;
  }
  memset(pdn_throttle_info,0, sizeof(ps_sys_ioctl_pdn_throttle_info_type));
  ds_eps_kamgr[subs_indx]->active_ind = DS_EPS_KAMGR_RESET;

  /*-------------------------------------------------------------------------- 
  The net down reason passed by the network decides whether we re-initiate the 
  PDN connectivity for Keep Alive PDN 
   
  PDN is not throttled: We start a timer and wait for the timer to expire before 
  sending out a new request. 
  The timer duration changes depending on whether the previous cause code was 
  same as the current one or not. 
  After 3 retries (for same cause code) we start a timer for 10 secs after which 
  we retry 
   
  PDN is throttled: Here we gather the PDN throttle information using 
  the ps_sys_ioctl and start a timer for remaining throttled time, when the 
  timer expires we re-initiate the bring-up. 
  In case we the APN is not throttled, we wait for some time before retrying
  ----------------------------------------------------------------------------*/
  current_mode = ds3g_get_apn_pref_pkt_mode_ex(&(ds_eps_kamgr[subs_indx]->apn_name), subs_id);

  DS_LTE_MSG1_MED("ds_eps_kamgr_process_iface_down_ev_cback: Current mode %d",
                  current_mode);

  if(DS_EPS_KAMGR_IS_3GPP_CURRENT_MODE(current_mode) &&
     ds_eps_kamgr[subs_indx]->apn_name.apn_string_len > 0)
  {
    /*-------------------------------------------------------------------------- 
    De-register for cbacks for the Um iface since during the next bring up, we 
    will be allocated a new iface 
    --------------------------------------------------------------------------*/
    DS_LTE_MSG1_MED("Iface Down Cback receieved for IFACE:0x%x", iface_ptr);

    pdn_context_ptr = DSPDNCNTX_GET_PDN_FROM_IFACE(iface_ptr);

    if(ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_ptr))
    {
      if(ds_3gpp_pdn_cntxt_is_apn_throttled(pdn_context_ptr, iface_ptr)
                                                                   == TRUE)
      {
        if(pdn_throttle_info != NULL)
        {
          if(0 > ps_sys_ioctl(PS_SYS_TECH_3GPP,
                       PS_SYS_IOCTL_PDN_THROTTLE_INFO,
                       (void *)pdn_throttle_info,
                       &ps_errno))
          {
            DS_LTE_MSG2_HIGH("ps_sys_ioctl %d operation returned %d, "
                             "retrying with temporaray cause code "
                             "handling logic",
                             PS_SYS_IOCTL_PDN_THROTTLE_INFO, ps_errno);
            ds_eps_kamgr_retry_after_failure(subs_id, iface_down_event_info.
                                         iface_down_info.netdown_reason, 
                                         FALSE);
          }
          else
          {
            permanent_failure = 
                   ds_eps_kamgr_process_pdn_throttle_info_for_bring_up (subs_id,
                                 pdn_throttle_info,
                                 iface_down_event_info.iface_down_info.
                                                     netdown_reason);
          }
        }
      }
      else
      {
        if (ds_eps_kamgr[subs_indx]->retry_counter <= (ds_eps_kamgr[subs_indx]->config_info).maximum_retries)
        {
          ds_eps_kamgr_retry_after_failure (subs_id, iface_down_event_info.
                                            iface_down_info.netdown_reason,
                                            FALSE);
        }
        else
        {
          rex_set_timer(&(ds_eps_kamgr[subs_indx]->keep_alive_bring_up_timer),
                        (rex_timer_cnt_type)(ds_eps_kamgr[subs_indx]->config_info).
                                             maximum_timer_val);
        }
      }
    }
    else
    {
      DS_LTE_MSG1_ERROR("Invalid PDN context associated with "
                        "PS iface:0x%x",iface_ptr);
    }
  }
  else
  {
    DS_LTE_MSG1_HIGH("ds_eps_kamgr_process_iface_down_ev_cback: Current mode"
                     " is not 3gpp, %d Thereby we clean-up iface resources",
                     current_mode);
  }

  ds_eps_kamgr_reg_for_all_iface_up_ev_cback(subs_id);
  ds_eps_kamgr_dereg_iface_cbacks_and_free_cback_buffers(subs_id, 
                                                         ds_eps_kamgr[subs_indx]->ps_iface_ptr,
                                                         permanent_failure);

  modem_mem_free ((void *)pdn_throttle_info, MODEM_MEM_CLIENT_DATA);
  pdn_throttle_info = NULL;
#else
  DS_LTE_MSG0_LOW ("ds_eps_kamgr_process_iface_down_ev_cback: MAPCON feature"
                    " disabled");
#endif /*FEATURE_DATA_WLAN_MAPCON*/
}/*ds_eps_kamgr_process_iface_down_ev_cback*/

/*===========================================================================
FUNCTION DS_EPS_KAMGR_PROCESS_IFACE_UP_EV_CBACK

DESCRIPTION
  This function process the iface up event and resets the retry counter
  
PARAMETERS 
  event_info:                    Iface information reported by PS
  user_data:                     User data passed by the function posting cmd

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void ds_eps_kamgr_process_kamgr_iface_up_ev_cback
(
  void  *cmd_payload_ptr
)
{
#ifdef FEATURE_DATA_WLAN_MAPCON
  ps_iface_type                           *iface_ptr = NULL;
  ds_pdn_context_s                        *pdn_context_ptr = NULL;
  byte                                    *resolved_apn = NULL;
  ds_eps_kamgr_iface_events_cmd_type  *data_ptr = NULL;
  ds3gsubsmgr_subs_id_e_type           subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  data_ptr = (ds_eps_kamgr_iface_events_cmd_type*)cmd_payload_ptr;

  if (NULL == data_ptr)
  {
    return;
  }

  iface_ptr = data_ptr->iface_ptr;

  if(!PS_IFACE_IS_VALID(iface_ptr))
  {
    DS_LTE_MSG1_ERROR("Iface is invalid iface: 0x%x: ", iface_ptr);
    return;
  }

  if( !ds3gsubsmgr_is_subs_id_valid(PS_IFACE_GET_SUBS_ID(iface_ptr)) )
  {
    return;
  }

  subs_indx = ds3gsubsmgr_subs_id_ds_to_ds3g(PS_IFACE_GET_SUBS_ID(iface_ptr));

  if (!ds3gsubsmgr_is_ds3g_subs_index_valid(subs_indx))
  {
    return;
  }

  if((ds_eps_kamgr[subs_indx] == NULL) ||
     (ds_eps_kamgr_iface_ev_regd[subs_indx] != TRUE))
  {
    DS_LTE_MSG0_ERROR("ds_eps_kamgr_process_iface_up_ev_cback: Keep alive"
                      " manager released or not allocated yet");
    return;
  }

  if(ds_eps_kamgr[subs_indx]->ps_iface_ptr != iface_ptr)
  {
    DS_LTE_MSG2_MED("ds_eps_kamgr_process_iface_up_ev_cback: Invoked for wrong"
                    " iface 0x%x instead of 0x%x",
                    iface_ptr, ds_eps_kamgr[subs_indx]->ps_iface_ptr);
    return;
  }
  pdn_context_ptr = DSPDNCNTX_GET_PDN_FROM_IFACE(iface_ptr);

  /*------------------------------------------------------------------------ 
    Here on receiving the IFACE UP event cback for KAMGR Iface
    1. Extract PDN Context associated with Iface
    2. Compare APN (Both Resolved/Profile Context one) with KAMGR APN
    3. On match, we perform reset the retry parameters since bring-up was
       successful
  -------------------------------------------------------------------------*/
  if(ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_ptr))
  {
    (void)ds_pdn_cntxt_get_resolved_apn_name(pdn_context_ptr,
                                             &(resolved_apn));

    if((strncasecmp((char*)ds_eps_kamgr[subs_indx]->apn_name.apn_string,(char*)pdn_context_ptr->
       ds_pdn_context_dyn_p->pdp_profile.context.apn, 
       DS_SYS_MAX_APN_LEN) == 0)||
       ((resolved_apn != NULL) && 
        (strncasecmp((char*)ds_eps_kamgr[subs_indx]->apn_name.apn_string,
        (char*)resolved_apn,DS_SYS_MAX_APN_LEN)== 0)))
    {
      ds_eps_kamgr[subs_indx]->retry_counter = DS_EPS_KAMGR_RESET;
      ds_eps_kamgr[subs_indx]->prev_down_reason = DS_EPS_KAMGR_RESET;           
    }
  }

#else
  DS_LTE_MSG0_LOW ("ds_eps_kamgr_process_iface_up_ev_cback: MAPCON feature"
                     "disabled");
#endif /* FEATURE_DATA_WLAN_MAPCON */
}/*ds_eps_kamgr_process_kamgr_iface_up_ev_cback*/

/*===========================================================================
FUNCTION DS_EPS_KAMGR_PROCESS_ALL_IFACE_UP_EV_CBACK

DESCRIPTION
  This function process the iface up event cback which is received as a part
  of WLAN disable Indication
  
PARAMETERS 
  user_data:                     User data passed by the function posting cmd

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_kamgr_process_all_iface_up_ev_cback
(
  void  *cmd_payload_ptr
)
{
#ifdef FEATURE_DATA_WLAN_MAPCON
  ps_iface_type                           *iface_ptr = NULL;
  ds_pdn_context_s                        *pdn_context_ptr = NULL;
  byte                                    *resolved_apn = NULL;
  ds_eps_kamgr_iface_events_cmd_type  *data_ptr = NULL;
  ds3gsubsmgr_subs_id_e_type           subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  data_ptr = (ds_eps_kamgr_iface_events_cmd_type*)cmd_payload_ptr;

  if (NULL == data_ptr)
  {
    return;
  }
  iface_ptr = data_ptr->iface_ptr;

  do
  {
    if(!PS_IFACE_IS_VALID(iface_ptr))
    {
      DS_LTE_MSG1_ERROR("Iface is invalid iface: 0x%x: ", iface_ptr);
      break;
    }

    if( !ds3gsubsmgr_is_ds3g_subs_index_valid(PS_IFACE_GET_SUBS_ID(iface_ptr)) )
    {
      break;
    }

    subs_indx = ds3gsubsmgr_subs_id_ds_to_ds3g(PS_IFACE_GET_SUBS_ID(iface_ptr));

    if (!ds3gsubsmgr_is_ds3g_subs_index_valid(subs_indx))
    {
      break;
    }

    if( (ds_eps_kamgr[subs_indx] == NULL) ||
        (ds_eps_kamgr_all_iface_up_regd[subs_indx] != TRUE) )
    {
      DS_LTE_MSG0_ERROR("ds_eps_kamgr_process_all_iface_up_ev_cback: Keep alive"
                        " manager released or not allocated yet");
      break;
    }

    if (ds_loopback_hdlr_check_if_loopback_iface(iface_ptr)) 
    {
      DS_LTE_MSG0_ERROR("Iface is loopback iface. Break");
      break;
    }

    DS_LTE_MSG1_MED("ds_eps_kamgr_process_all_iface_up_ev_cback: Cback receieved"
                    " for IFACE:0x%x", iface_ptr);
    /*------------------------------------------------------------------------ 
      Here on receiving the IFACE UP event cback
      1. Extract PDN Context associated with Iface
      2. Compare APN (Both Resolved/Profile Context one) with KAMGR APN
      3. On match, we perform
         3.1 Set Keep Alive PDN flag for the PDN context
         3.2 De-Reg from Iface event cback from all other 3GPP Ifaces
         3.3 Register for Iface up & down event cbacks on the selected iface
    -------------------------------------------------------------------------*/
    pdn_context_ptr = DSPDNCNTX_GET_PDN_FROM_IFACE(iface_ptr);

    if(ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_ptr))
    {
      (void)ds_pdn_cntxt_get_resolved_apn_name(pdn_context_ptr,
                                               &(resolved_apn));

      if((strncasecmp((char*)ds_eps_kamgr[subs_indx]->apn_name.apn_string,
         (char*)pdn_context_ptr->ds_pdn_context_dyn_p->pdp_profile.context.apn, 
          DS_SYS_MAX_APN_LEN) == 0)||
         ((resolved_apn != NULL) && 
          (strncasecmp((char*)ds_eps_kamgr[subs_indx]->apn_name.apn_string,
          (char*)resolved_apn,DS_SYS_MAX_APN_LEN)== 0)))
      {
        DATA_MSG_SPRINTF_1(MSG_LEGACY_ERROR,
                           "APN Match, Setting keep alive PDN flag: %s",
                           pdn_context_ptr->ds_pdn_context_dyn_p->pdp_profile.
                           context.apn);
        pdn_context_ptr->ds_pdn_context_dyn_p->keep_alive_pdn = TRUE;

        ds_eps_kamgr_dereg_for_all_iface_up_ev_cback(PS_IFACE_GET_SUBS_ID(iface_ptr));

        if(ds_eps_kamgr_alloc_cback_buf(PS_IFACE_GET_SUBS_ID(iface_ptr)) == FALSE)
        {
          DS_LTE_MSG0_ERROR("Cback buffer allocation failure, Exiting !!!!");
          break;
        }
        ds_eps_kamgr[subs_indx]->ps_iface_ptr = iface_ptr;
        ds_eps_kamgr_setup_iface_event_cbacks((PS_IFACE_GET_SUBS_ID(iface_ptr)), iface_ptr);
      }
    }

  }while(0);

#else
  DS_LTE_MSG0_LOW ("ds_eps_kamgr_process_all_iface_up_ev_cback: MAPCON feature"
                     "disabled");
#endif /* FEATURE_DATA_WLAN_MAPCON */
}/*ds_eps_kamgr_process_all_iface_up_ev_cback*/

/*===========================================================================
FUNCTION DS_EPS_KAMGR_PROCESS_TIMER_EXPIRY_CBACK

DESCRIPTION
  This function is responsible for bring-up of keep alive manager after timer
  expiry
  
PARAMETERS 
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_kamgr_process_timer_expiry_cback
(
  void  *cmd_payload_ptr
)
{
#ifdef FEATURE_DATA_WLAN_MAPCON
  boolean                                     bring_up_ret_val = FALSE;
  ds_eps_kamgr_timer_cmd_type  *data_ptr = NULL;
  sys_modem_as_id_e_type        subs_id;
  ds3gsubsmgr_subs_id_e_type    subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  data_ptr = (ds_eps_kamgr_timer_cmd_type*)cmd_payload_ptr;
  if (NULL == data_ptr)
  {
    return;
  }

  subs_id = data_ptr->subs_id;
  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  if(ds_eps_kamgr[subs_indx] == NULL)
  {
    DS_LTE_MSG0_ERROR("ds_eps_kamgr_process_timer_expiry_cback: Keep alive"
                      " manager not active; Error");
    return;
  }
  /*------------------------------------------------------------------------ 
    On Timer expiry we check whether LTE is in full service and Keep alive
    manager is not active, if both conditions are satisfied we initiate the
    bring-up again.
  -------------------------------------------------------------------------*/
  if(ds_eps_kamgr[subs_indx]->lte_srv_status == TRUE && 
     ds_eps_kamgr[subs_indx]->active_ind == DS_EPS_KAMGR_RESET &&
     ds_eps_kamgr[subs_indx]->wlan_availability == TRUE) 
  {
    bring_up_ret_val = ds_eps_kamgr_bring_up_keep_alive_pdn(subs_id);
    if(bring_up_ret_val == TRUE)
    {
      ds_eps_kamgr[subs_indx]->active_ind = DS_EPS_SET_KAMGR;
    }
  }
  else
  {
    DS_LTE_MSG0_MED("ds_eps_kamgr_process_timer_expiry_cback: LTE/WLAN not in "
                    "full Service or keep alive manager already active, "
                    "Keep Alive PDN bring up not initiated");
  }

  DS_LTE_MSG1_MED("ds_eps_kamgr_process_timer_expiry_cback: Bring Up status"
                  " after keep alive PDN Timer wait %d", bring_up_ret_val);
#else
  DS_LTE_MSG0_LOW ("ds_eps_kamgr_process_timer_expiry_cback: "
                    "MAPCON feature disabled");
#endif /*FEATURE_DATA_WLAN_MAPCON*/
}/*ds_eps_kamgr_process_timer_expiry_cback*/

/*===========================================================================
FUNCTION DS_EPS_KAMGR_PROCESS_APN_NAME_CHG

DESCRIPTION
  This function is responsible for update the internal structure with the
  updated APN name
  
PARAMETERS 
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_kamgr_process_apn_name_chg
(
  uint16                  profile_id,
  sys_modem_as_id_e_type  subs_id
)
{
#ifdef FEATURE_DATA_WLAN_MAPCON
  ds3gsubsmgr_subs_id_e_type   subs_indx;
  boolean                      bring_up_ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  do
  {
    if(ds_eps_kamgr[subs_indx] == NULL)
    {
      DS_LTE_MSG0_ERROR("ds_eps_kamgr_process_apn_name_chg: Keep alive "
                        "manager instance released");
      break; 
    }

    if (profile_id != ds_eps_kamgr[subs_indx]->config_info.profile_no)
    {
      DS_LTE_MSG0_HIGH ("Keep Alive Manager's APN name has not changed: No "
                         "Update reqired");
      break;
    }

    ds_eps_kamgr_get_keep_alive_apn_from_profile(subs_id);

    if(ds_eps_kamgr[subs_indx]->apn_name.apn_string_len <= 0)
    {
      DS_LTE_MSG1_ERROR ("ds_eps_kamgr_process_apn_name_chg: NULL/Invalid APN"
                         " %d",(ds_eps_kamgr[subs_indx]->apn_name).apn_string_len);
      break;
    }

    DATA_MSG_SPRINTF_1(MSG_LEGACY_MED,
                     "ds_eps_kamgr_process_apn_name_chg Updated APN name: %s ",
                     (ds_eps_kamgr[subs_indx]->apn_name).apn_string);

    /*---------------------------------------------------------------------------
      Consider a scenario:
      1. Keep Alive APN: vzwinternet, Network changes to : vzwapps
      2. Pref mode for vzwapps is WLAN, so Keep Alive PDN torn down
      3. Now Network again changes it back to vzwinternet
      4. Now UE needs to re-initiate the bring-up of Keep Alive PDN
    ----------------------------------------------------------------------------*/

    if (ds_eps_kamgr[subs_indx]->lte_srv_status == TRUE && 
        ds_eps_kamgr[subs_indx]->active_ind == DS_EPS_KAMGR_RESET &&
        (!(rex_timer_cnt_type)rex_get_timer
         (&(ds_eps_kamgr[subs_indx]->keep_alive_bring_up_timer))) &&
         (ds_eps_kamgr[subs_indx]->wlan_availability == TRUE))
    {
      DS_LTE_MSG0_HIGH ("ds_eps_kamgr_process_apn_name_chg: Inititating bring-up"
                        " of Keep Alive PDN");
      bring_up_ret_val = ds_eps_kamgr_bring_up_keep_alive_pdn (subs_id);
      if(bring_up_ret_val == TRUE)
      {
        ds_eps_kamgr[subs_indx]->active_ind = DS_EPS_SET_KAMGR;
      }
    }

  }while (0);

#else
  DS_LTE_MSG0_LOW ("ds_eps_kamgr_process_apn_name_chg: MAPCON feature disabled");
#endif /*FEATURE_DATA_WLAN_MAPCON*/
}/*ds_eps_kamgr_process_apn_name_chg*/

/*===========================================================================
FUNCTION DS_EPS_KAMGR_PROCESS_SYS_INFO

DESCRIPTION
  This function process the system info given by MPPM to decide whether
  Keep alive PDN can be brought up or not

PARAMETERS
  sys_info_ptr:                    System information reported by MPPM

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_kamgr_process_sys_info
(
  void  *cmd_payload_ptr
)
{  
#ifdef FEATURE_DATA_WLAN_MAPCON 
  ps_sys_system_status_ex_type            *sys_info_ptr = NULL;
  uint32                                   num_apns_available = 0;
  uint8                                    index = 0;
  ps_sys_network_enum_type                 tech = PS_SYS_NETWORK_MAX; 
  ps_sys_rat_ex_enum_type                  rat_val = PS_SYS_RAT_MAX;  
  uint64                                   so_mask = PS_SYS_SO_EX_UNSPECIFIED;
  boolean                                  bring_up_ret_val = FALSE;  
  ds_eps_kamgr_sys_info_cmd_type  *data_ptr = NULL;
  ds3gsubsmgr_subs_id_e_type       subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  data_ptr = (ds_eps_kamgr_sys_info_cmd_type*)cmd_payload_ptr;
  
  if (NULL == data_ptr)
  {
    return;
  }

  if( !ds3gsubsmgr_is_subs_id_valid(data_ptr->subs_id) )
  {
    return;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(data_ptr->subs_id);

  /*------------------------------------------------------------------------- 
    Extract the APN name from the Profile ID, we do not have support for NULL
    APN
  --------------------------------------------------------------------------*/
  do
  {
    if(ds_eps_kamgr[subs_indx] == NULL)
    {
      DS_LTE_MSG0_ERROR("ds_eps_kamgr_process_sys_info: Keep alive manager "
                        "released or not allocated yet");
      break;
    }

    if (data_ptr->sys_info_ptr == NULL)
    {
      DS_LTE_MSG0_ERROR ("ds_eps_kamgr_process_sys_info: Invalid Input Param");
      break;
    }

    ds_eps_kamgr_get_keep_alive_apn_from_profile(data_ptr->subs_id);

    if(ds_eps_kamgr[subs_indx]->apn_name.apn_string_len <= 0)
    {
      DS_LTE_MSG1_ERROR ("ds_eps_kamgr_process_sys_info: Profile does not exist"
                         " or a case of NULL APN %d",
                         (ds_eps_kamgr[subs_indx]->apn_name).apn_string_len);
      break;
    }

    sys_info_ptr = (ps_sys_system_status_ex_type *)(data_ptr->sys_info_ptr);
    /*-------------------------------------------------------------------------- 
    Loop through all the available APNs for a match with keep alive APN, then 
    find out the preferred RAT for it 

    If LTE is the preferred RAT we try to bring up the Keep Alive APN, for any 
    other case the bring up is blocked till further cbacks
    ----------------------------------------------------------------------------*/
    num_apns_available = sys_info_ptr->num_avail_apns;

    DS_LTE_MSG1_MED ("ds_eps_kamgr_process_sys_info: Num of APNs passed are"
                     " %d",num_apns_available);

    ds_eps_kamgr[subs_indx]->wlan_availability = ds_eps_kamgr_check_wlan_status 
                                                             (sys_info_ptr);

    if(num_apns_available == 0)
    {
      DS_LTE_MSG0_LOW("ds_eps_kamgr_process_sys_info: Using DSD's preferred"
                       " mode information for bring_up");
      bring_up_ret_val = ds_eps_kamgr_use_dsd_pref_sys_for_bring_up (data_ptr->subs_id,
                                                                 sys_info_ptr);
    }
    else
    {
      for(index = 0; index < num_apns_available; index++)
      {
        if(strncasecmp((const char *)sys_info_ptr->apn_pref_sys_info[index].
                        apn_name, 
                        (const char *)(ds_eps_kamgr[subs_indx]->apn_name).apn_string, 
                        DS_SYS_MAX_APN_LEN) == 0)
        {
          DS_LTE_MSG1_MED("ds_eps_kamgr_process_sys_info: Match found in the"
                          " system information passed by MPPM at %d",index);
          break;
        }
      }

      if(index < num_apns_available)
      {
        tech = sys_info_ptr->apn_pref_sys_info[index].
                                     avail_sys[PREF_RAT_INDEX].technology;
        rat_val = sys_info_ptr->apn_pref_sys_info[index].
                                     avail_sys[PREF_RAT_INDEX].rat_value;
        so_mask = sys_info_ptr->apn_pref_sys_info[index].
                                     avail_sys[PREF_RAT_INDEX].so_mask;

        DS_LTE_MSG3_MED("ds_eps_kamgr_process_sys_info: Technology: %d, "
                        "Rat_Mask:%d, So_Mask:%d",tech,rat_val,so_mask);
        bring_up_ret_val = ds_eps_kamgr_process_system_status_info (
                                                                    data_ptr->subs_id,
                                                                    tech, 
                                                                    rat_val
                                                                    );
      }
      else
      {
        DATA_MSG_SPRINTF_1(MSG_LEGACY_ERROR,
                           "ds_eps_kamgr_process_sys_info:No sys mode for %s "
                           "Using DSD preferred sys mode for bring-up ",
                           (ds_eps_kamgr[subs_indx]->apn_name).apn_string);
        bring_up_ret_val = ds_eps_kamgr_use_dsd_pref_sys_for_bring_up (data_ptr->subs_id,
                                                                   sys_info_ptr);
      }
    }
  }while (0);
  
  if (sys_info_ptr != NULL)
  {
    modem_mem_free(sys_info_ptr, MODEM_MEM_CLIENT_DATA);
  }

  DS_LTE_MSG1_MED("ds_eps_kamgr_process_sys_info: Bring-Up status %d",
                  bring_up_ret_val);

#else
  DS_LTE_MSG0_LOW ("ds_eps_kamgr_process_sys_info: MAPCON feature disabled");
#endif /* FEATURE_DATA_WLAN_MAPCON */
}/*ds_eps_kamgr_process_sys_info*/

/*===========================================================================
FUNCTION  DS_EPS_KAMGR_RETRY_AFTER_FAILURE

DESCRIPTION
  This function incorporates the retry logic after we temporaray cause code
  as net down reason or we are not able to extract throttling info when we
  receive a throttling based cause code
  
PARAMETERS 
  down_reason:                            down reason received from network

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void ds_eps_kamgr_retry_after_failure 
(
  sys_modem_as_id_e_type                           subs_id,
  ps_iface_net_down_reason_type                    down_reason,
  boolean                                          bringup_failure
)
{
#ifdef FEATURE_DATA_WLAN_MAPCON
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  if(ds_eps_kamgr[subs_indx] == NULL)
  {
    DS_LTE_MSG0_ERROR("ds_eps_kamgr_retry_after_failure: Keep alive"
                      "manager not active");
    return;
  }
  /*----------------------------------------------------------------------- 
   If it is a bring-up failure then we retry after a fixed period of time
   everytime (default timer val)
   In other case we use a concept of incremental timer, the timer increment,
   default timer val, maximum number of retries and maximum timer value
   is all configurable through an efs file
  ------------------------------------------------------------------------*/
  if(bringup_failure == FALSE)
  {
    /*------------------------------------------------------------------- 
      If the previous net down reason is same as current down reason, we
      use an incremental timer
      Else, we reset the counter and timer and start with the default
      timer value
    --------------------------------------------------------------------*/
    if(ds_eps_kamgr[subs_indx]->prev_down_reason != down_reason)
    {
      ds_eps_kamgr[subs_indx]->retry_counter = DS_EPS_KAMGR_RESET;
      ds_eps_kamgr[subs_indx]->prev_down_reason = down_reason;
      ds_eps_kamgr[subs_indx]->bring_up_timer_val = DS_EPS_KAMGR_RESET;
    }
  
    if(ds_eps_kamgr[subs_indx]->bring_up_timer_val != DS_EPS_KAMGR_RESET)
    {
      ds_eps_kamgr[subs_indx]->bring_up_timer_val = ds_eps_kamgr[subs_indx]->bring_up_timer_val + 
                            (ds_eps_kamgr[subs_indx]->config_info).incremental_timer_val;
    }
    else
    { 
      ds_eps_kamgr[subs_indx]->bring_up_timer_val = (ds_eps_kamgr[subs_indx]->config_info).
                                                   default_timer_val;
    }
  }
  else
  {
    ds_eps_kamgr[subs_indx]->bring_up_timer_val = (ds_eps_kamgr[subs_indx]->config_info).
                                                    default_timer_val;
  }

  (ds_eps_kamgr[subs_indx]->retry_counter)++;
  rex_set_timer(&(ds_eps_kamgr[subs_indx]->keep_alive_bring_up_timer),
                  (rex_timer_cnt_type)(ds_eps_kamgr[subs_indx]->bring_up_timer_val));
#else
   DS_LTE_MSG0_LOW ("ds_eps_kamgr_retry_after_failure: MAPCON feature"
                     " disabled");
#endif /* FEATURE_DATA_WLAN_MAPCON */
}/*ds_eps_kamgr_retry_after_failure*/

/*===========================================================================

                         EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION  DS_EPS_KAMGR_INIT

DESCRIPTION
  Initialization of all the required structures and variable for Keep alive
  manager
  
PARAMETERS
 None
 
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_eps_kamgr_init (void)
{
#ifdef FEATURE_DATA_WLAN_MAPCON
  int16                                      ps_errno = -1;
  int16                                      ps_sys_reg_ret_val = -1;
  int                                        subs_index = PS_SYS_DEFAULT_SUBS; 
  ds3geventmgr_filter_type                   filter_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_LTE_MSG0_LOW("ds_eps_kamgr_init: Entered during bootup");

  /*-------------------------------------------------------------------------- 
   Register for System confirmation status event with PS to find out the
   preferred mode for the keep alive PDN, if the registration fails we free
   all the resources allocated so far
  ---------------------------------------------------------------------------*/  

  memset(&filter_info, 0, sizeof(ds3geventmgr_filter_type));
  filter_info.tech = PS_SYS_TECH_ALL;

  for (subs_index = PS_SYS_PRIMARY_SUBS; 
        subs_index < PS_SYS_SUBS_MAX; subs_index++)
  {
    ps_sys_reg_ret_val = ps_sys_event_reg_ex(PS_SYS_TECH_ALL,
                                             PS_SYS_EVENT_SYSTEM_STATUS_EX,
                                             subs_index,
                                             ds_eps_kamgr_ps_conf_status_cback,
                                             NULL,
                                             &ps_errno);
    if (ps_sys_reg_ret_val < 0)
    {
      DS_LTE_MSG2_HIGH ("ds_eps_kamgr_init: Error while "
                       "registering for PS_SYS_EVENT_SYSTEM_STATUS_EX event, "
                       "return value %d and error %d, freeing all the resources",
                        ps_sys_reg_ret_val, ps_errno);
      break;
    }
    else
    {
      ds_eps_kamgr_system_status_cback_reg_status = TRUE;
    }

    filter_info.ps_subs_id = subs_index;
    if (ds3geventmgr_event_reg(DS3GEVENTMGR_DATA_CHANGED_EV,
                               DS3GEVENTMGR_CLIENT_ID_KAMGR,
                               &filter_info,
                               ds_eps_kamgr_device_ev_hdlr_cb,
                               NULL) == FALSE)
    {
      DS_LTE_MSG0_ERROR("Registration for DS3GEVENTMGR_DATA_CHANGED_EV"
                         " failed !");
    }

    if (ds3geventmgr_event_reg(DS3GEVENTMGR_DATA_ROAMING_CHANGED_EV,
                               DS3GEVENTMGR_CLIENT_ID_KAMGR,
                               &filter_info,
                               ds_eps_kamgr_device_ev_hdlr_cb,
                               NULL) == FALSE)
    {
      DS_LTE_MSG0_ERROR("Registration for DS3GEVENTMGR_DATA_ROAMING_CHANGED_EV"
                         " failed !");
    }
  }

  /*-------------------------------------------------------------------------- 
   Register for Data Disable cback indication with DS3G
   
   As a part of cback processing check if WLAN is available,
   If not available check Iface in use status
   If neither of the ifaces are in use and WLAN is disabled, force OTA teardown
  ---------------------------------------------------------------------------*/  
  ds_eps_kamgr_reg_for_all_iface_up_ev_cback (SYS_MODEM_AS_ID_1);

#else
   DS_LTE_MSG0_HIGH ("ds_eps_kamgr_init: MAPCON feature disabled");
#endif /* FEATURE_DATA_WLAN_MAPCON */
}

/*===========================================================================
FUNCTION       DS_EPS_KAMGR_PROCESS_CMD

DESCRIPTION    Process DS cmds 
 
PARAMETERS     cmd_ptr :              Command information pointer 

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_eps_kamgr_process_cmd
(
  const ds_cmd_type                    *cmd_ptr
)
{

#ifdef FEATURE_DATA_WLAN_MAPCON

  if(cmd_ptr == NULL)
  {
    DS_LTE_MSG0_ERROR("NULL ptr passed, return");
    return;
  }

  DS_LTE_MSG1_HIGH("ds_eps_kamgr_process_cmd: command ID=%d",cmd_ptr->hdr.cmd_id);

  switch(cmd_ptr->hdr.cmd_id)
  {
    case DS_CMD_3GPP_EPS_KAMGR_SYS_INFO:
      if (cmd_ptr->cmd_payload_ptr != NULL)
      {
        ds_eps_kamgr_process_sys_info (cmd_ptr->cmd_payload_ptr);
      }
      break;

    case DS_CMD_3GPP_EPS_KAMGR_IFACE_DOWN_EV_CB:
      if (cmd_ptr->cmd_payload_ptr != NULL)
      {
        ds_eps_kamgr_process_iface_down_ev_cback(cmd_ptr->cmd_payload_ptr);
      }
      break;

    case DS_CMD_3GPP_EPS_KAMGR_IFACE_UP_EV_CB:
      if (cmd_ptr->cmd_payload_ptr != NULL)
      {
        ds_eps_kamgr_process_kamgr_iface_up_ev_cback(cmd_ptr->cmd_payload_ptr);
      }
      break;

    case DS_CMD_3GPP_EPS_KAMGR_TIMER_EXPIRY_CB:
       if( cmd_ptr->cmd_payload_ptr != NULL )
      {
        ds_eps_kamgr_process_timer_expiry_cback(cmd_ptr->cmd_payload_ptr);
      }
      break;

    case DS_CMD_3GPP_EPS_KAMGR_IFACE_ALL_UP_EV_CB:
      if (cmd_ptr->cmd_payload_ptr != NULL)
      {
        ds_eps_kamgr_process_all_iface_up_ev_cback(cmd_ptr->cmd_payload_ptr);
      }
      break;

    default:
      DS_LTE_MSG1_ERROR("ds_eps_kamgr_process_cmd: Command %d: not for KAMGR",
                        cmd_ptr->hdr.cmd_id);
      break;
  }
#else
   DS_LTE_MSG0_LOW ("ds_eps_kamgr_process_cmd: MAPCON feature disabled");
#endif /* FEATURE_DATA_WLAN_MAPCON */
}

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK*/

/*===========================================================================
FUNCTION  DS_EPS_KAMGR_READ_EFS_INFO

DESCRIPTION
  This function is responsible for reading the EFS file and WLAN offload config
  NV and deciding whether Keep alive manager should be enabled or not
  
PARAMETERS 
  None

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void ds_eps_kamgr_read_efs_info
(
  sys_modem_as_id_e_type  subs_id
)
{
#ifdef FEATURE_DATA_WLAN_MAPCON
  char                                          *efs_data = NULL;
  boolean                                        parsing_status = FALSE; 
  uint16                                         epdg_nv_value = 0;
  uint32                                         file_size = 0;
  ds_eps_kamgr_efs_info                          config_info;
  ds3gsubsmgr_subs_id_e_type          subs_indx = DS3GSUBSMGR_SUBS_ID_INVALID;
/*- - - - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - */

  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  if (ds_eps_kamgr_system_status_cback_reg_status == TRUE)
  {
    /*------------------------------------------------------------------------ 
     Here we use the various EFS items to decide whether keep alive manager
     has to enable or not
     In the configurable EFS file, Profile ID is a must whereas Timer values
     are optional
     There is no support for "NULL APN"
    -------------------------------------------------------------------------*/
    epdg_nv_value = ds_wlan_get_wlan_offload_config_nv ();

    file_size = ds_eps_kamgr_read_keep_alive_apn_data (subs_id, &efs_data);

    if(file_size <= 0)
    {
      DS_LTE_MSG1_ERROR("ds_eps_kamgr_read_efs_info: Invalid File Size %d",
                        file_size);
      ds_eps_kamgr_clean_up_resources(subs_id);
      return;
    }

    memset(&config_info, 0, sizeof(config_info));

    parsing_status = ds_eps_kamgr_parse_efs_data (subs_id, efs_data,file_size,
                                                  &config_info);

    DS_LTE_MSG1_MED("ds_eps_kamgr_read_efs_info: Parsing Status %d",
                    parsing_status);

    ds_eps_kamgr_free_kamgr_data_buffer (&efs_data);

    if(config_info.profile_no == 0)
    {
      DS_LTE_MSG0_ERROR ("ds_eps_kamgr_read_efs_info: Parsing Error, Exiting");
      ds_eps_kamgr_clean_up_resources(subs_id);
      return;
    }

    if (config_info.incremental_timer_val == 0)
    {
      config_info.incremental_timer_val = DS_EPS_KAMGR_INCREMENTAL_TIMER_VAL;
      parsing_status =  TRUE;
    }

    if (config_info.default_timer_val == 0)
    {
      config_info.default_timer_val = DS_EPS_KAMGR_DEFAULT_TIMER_VAL;
      parsing_status =  TRUE;
    }

    if (config_info.maximum_timer_val == 0)
    {
      config_info.maximum_timer_val = DS_EPS_KAMGR_MAX_TIMER_VAL;
      parsing_status =  TRUE;
    }

    if (config_info.maximum_retries == 0)
    {
      config_info.maximum_retries = DS_EPS_KAMGR_MAX_RETRIES;
      parsing_status =  TRUE;
    }

    /*------------------------------------------------------------------------
    Read the NV 70315 and Configurable EFS file to find the Keep Alive PDN,
    If NV70315 is set to 2 (local breakout + IWLAN S2B) and APN name is valid
    we enable the Keep Alive Manager
    -------------------------------------------------------------------------*/
    if(epdg_nv_value == DS_EPS_KEEP_ALIVE_EPDG_VALUE &&
       parsing_status == TRUE)
    {
      DS_LTE_MSG1_MED("Enabling Keep alive manager during boot-up/NV refresh: 0x%x",
                      ds_eps_kamgr[subs_indx]);

      if(ds_eps_kamgr[subs_indx] == NULL)
      {
        ds_eps_kamgr[subs_indx] = (ds_eps_kamgr_info_type *)modem_mem_alloc(
                            sizeof(ds_eps_kamgr_info_type), 
                                                  MODEM_MEM_CLIENT_DATA);

        if(ds_eps_kamgr[subs_indx] != NULL)
        {
          memset(ds_eps_kamgr[subs_indx], 0, sizeof(ds_eps_kamgr_info_type));
         /*----------------------------------------------------------------------- 
         Define a timer that will be used before re-initiating request to bring up 
         Keep alive PDN 
         ------------------------------------------------------------------------*/
          rex_def_timer_ex(&(ds_eps_kamgr[subs_indx]->keep_alive_bring_up_timer),
                           (rex_timer_cb_type)ds_eps_kamgr_keep_alive_pdn_timer_cb,
                           (unsigned long)0);

          (void)memscpy((void *)&(ds_eps_kamgr[subs_indx]->config_info), 
                        sizeof(ds_eps_kamgr[subs_indx]->config_info),
                        (void *)&config_info, sizeof(config_info));
          /*-----------------------------------------------------------------------
           Register for APN change callback
          -----------------------------------------------------------------------*/
          (ds_eps_kamgr[subs_indx]->ds_profile_kamgr_cb_func_tbl).apn_disable_flag_change_handler = 
                                                                             NULL;
          (ds_eps_kamgr[subs_indx]->ds_profile_kamgr_cb_func_tbl).apn_name_change_handler = 
                                                                             NULL;
          (ds_eps_kamgr[subs_indx]->ds_profile_kamgr_cb_func_tbl).link.next_ptr = NULL;
          ds_profile_3gpp_register_callback_table(&ds_eps_kamgr[subs_indx]->
                                                  ds_profile_kamgr_cb_func_tbl);
        }
        else
        {
          DS_LTE_MSG0_ERROR("KAMGR instance could not be allocated !!!!");
        }
      }
    }
    else
    {
      ds_eps_kamgr_clean_up_resources(subs_id);
    }
  }
#else
  DS_LTE_MSG0_LOW ("ds_eps_kamgr_read_efs_info: MAPCON feature"
                    " disabled");
#endif /* FEATURE_DATA_WLAN_MAPCON */
}

/*===========================================================================
FUNCTION  DS_EPS_KAMGR_CHECK_FOR_PDN_TEARDOWN

DESCRIPTION
  This function validates DATA UI/Data Roaming Setting along with WLAN
  availability and In use flag of iface to decide whether KAMGR's PDN
  can be torn down or not
  
PARAMETERS 
  subs_id: Currently active Default data subs

DEPENDENCIES
  None

RETURN VALUE
  TRUE: If PDN is eligible for teardown
  FALSE: If PDN doesn't need to be torn down

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_eps_kamgr_check_for_pdn_teardown
(
  sys_modem_as_id_e_type                 subs_id
)
{
  boolean                                teardown_needed = FALSE;
#ifdef FEATURE_DATA_WLAN_MAPCON
  boolean                                data_settings = FALSE;
  boolean                                roaming_settings = FALSE;
  boolean                                data_is_allowed = FALSE;
  ds_pdn_context_s                      *pdn_context_ptr = NULL;
  ds3gsubsmgr_subs_id_e_type             subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
    {
      break;
    }
    subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

    /*----------------------------------------------------------------------- 
     This functions checks for 3 entities before deciding whether KAMGR needs
     to be torn down or not:
     1. Iface in not in USE (ref_cnt = 0)
     2. Data UI Disabled & Data Roaming (if UE is roaming) disabled
     3. Wi-Fi OFF
     
     If all the 3 conditions are satisfied then the PDN is torn down
    ------------------------------------------------------------------------*/
    if(PS_IFACE_IS_IN_USE(ds_eps_kamgr[subs_indx]->ps_iface_ptr) ||
        PS_IFACE_IS_IN_USE(ds_eps_kamgr[subs_indx]->companion_iface))
    {
      DS_LTE_MSG0_HIGH("Iface is in Use, no tearing down the PDN");
      break;
    }

    if((ds3gdevmgr_get_data_settings(&data_settings, subs_id) == FALSE)||
       (ds3gdevmgr_get_roaming_settings(&roaming_settings, subs_id) == FALSE))
    {
      DS_LTE_MSG0_ERROR("Failure extracting data or roaming settings");
      break;
    }

    data_is_allowed = 
      ds_eps_kamgr_is_data_allowed(data_settings, roaming_settings,
                                            subs_id);

    if(data_is_allowed == FALSE && ds_eps_kamgr[subs_indx]->wlan_availability == FALSE)
    {
      if(ds_eps_kamgr[subs_indx]->ps_iface_ptr != NULL)
      {
        pdn_context_ptr = DSPDNCNTX_GET_PDN_FROM_IFACE(ds_eps_kamgr[subs_indx]->ps_iface_ptr);

        if(ds_pdn_cntx_is_last_prevailing_pdn_in_lte(pdn_context_ptr))
        {
          DS_LTE_MSG0_HIGH("Last standing PDN");
        }
        else
        {
          teardown_needed = TRUE;
        }
      }
    }
  }while(0);
#else
  DS_LTE_MSG0_LOW ("ds_eps_kamgr_check_for_pdn_teardown: MAPCON feature"
                    " disabled");
#endif /* FEATURE_DATA_WLAN_MAPCON */
  return teardown_needed;
}

/*===========================================================================
FUNCTION  DS_EPS_KAMGR_HANDLE_ROAMING_STATUS_CHG

DESCRIPTION
  This function is called from roaming handler when roaming status changes
  
PARAMETERS 
  subs_id: Current Active PS subs

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_eps_kamgr_handle_roaming_status_chg
(
  sys_modem_as_id_e_type                  subs_id
)
{
  ds3gsubsmgr_subs_id_e_type  subs_indx;
#ifdef FEATURE_DATA_WLAN_MAPCON
  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  if(ds_eps_kamgr[subs_indx] == NULL)
  {
    DS_LTE_MSG0_ERROR("ds_eps_kamgr_roaming_status_chg_cback: Keep alive"
                      " manager not active; Error");
    return;
  }
  /*----------------------------------------------------------------------- 
   Roaming Hdlr informs KAMGR of roaming status change and we need to check
   1. If UE is in roaming and roaming setting is disabled
      Then Initiate PDN teardown
  ------------------------------------------------------------------------*/
  if(ds_eps_kamgr_check_for_pdn_teardown(subs_id))
  {
    ds_3gpp_pdn_cntx_teardown_apn((char *)ds_eps_kamgr[subs_indx]->apn_name.apn_string,
                                   subs_id, 
                                   PS_NET_DOWN_REASON_CLIENT_END);
  }
  else
  {
    DS_LTE_MSG0_MED("ds_eps_kamgr_roaming_status_chg_cback: No teardown");
  }
#else
  DS_LTE_MSG0_LOW ("ds_eps_kamgr_inform_roaming_status_chg: MAPCON feature"
                    " disabled");
#endif /* FEATURE_DATA_WLAN_MAPCON */
}
#endif /* FEATURE_DATA_LTE */
