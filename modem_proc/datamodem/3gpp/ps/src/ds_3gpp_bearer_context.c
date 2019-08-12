/*!
  @file
  ds_bearer_context.c

  @brief
  REQUIRED brief one-sentence description of this C module.

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2008-2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the expressf
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/src/ds_3gpp_bearer_context.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/12/13   ss      Cleanup bearers in PEND_REDIAL state upon sys_chg as 
                   lower layers wouldn't send cntxt_transfer when registration
                   is not successful on a RAT.
11/20/12   ss      Fixed the issue of BP removal NV read failure.
11/27/12   ss      Clearing service_req throttle flag when RAB_REESTAB_IND
                   is received from N/W (unsolicited).
08/13/12   ss      Moving efs write operation to dsutil performed during 
                   bootup.
08/10/12   ss      Delegating RAT specific handling during call_end.
08/08/12   ss      Optimization to trigger Dynamic BP only when threshold
                   num of acks are received.
06/19/12   nd      MT PDP pkt call.
07/04/12   ss      Freeing the BP timer handle in deact_cnf and avoiding
                   crash due to using stale timer handle.
06/07/12   ss      Enabled Dynamic BP by default and set the default tmr_val
                   to 2 seconds.
05/29/12   ss      Fix to not post multiple mem_ctrl_ev cmds to DS task.
05/24/12   kv      Avoid Rab reestab for GSM when we reorginate call from 
                   dormancy.
03/27/12   ss      Fixed issue of RAB_REESTAB_IND received before resume ind
                   resulting in error in data path setup.
03/22/12   ss      Addition of RAB_FAILURE and ESM local cause codes.
12/21/11   ss      Added support for watermark tuning.
11/17/11   sa      Added support for V6 Loopback Mode B.
10/21/11   nd      Fixed compilation warnings.
10/05/11   ttv     Added DSDS changes for new mode handler.
09/09/11   rr      Added support for fallback from Dual IP to single IP when nw rejects 
                   with specific cause codes in UMTS mode
04/07/11   ttv     Addec changes for IOCTL PS_IFACE_IOCTL_UMTS_GET_IM_CN_FLAG.
03/24/11   sa      Added MT PDP Auto answer support.
03/24/11   hs      Changed UL flow control to kick in from WM call back func.
03/17/11   sa      Added support for bearer tech change event indication.
03/02/11   ss      Added support for configurable RAB_REESTAB RETRY.
02/23/11   ttv     Added support for watermark tracing.
02/04/11   ss      Fixed an issue of data transfer not resuming on 2nd bearer
                   after RL failure and recovery.
05/17/10   hs      Added APN disable and PDN inactivity feature support.
04/03/10   vk      Added support for MSG2.0 based logging
04/02/10   vrk     UMTS interRAT support
01/23/10   vk      Added helper macros for virtual function dispatch
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h" 
#include "comdef.h"


#define FEATURE_DSM_WM_CB
#ifdef FEATURE_DATA_3GPP
#include "ds_3gpp_bearer_context.h"
#include "ds_3gpp_pdn_context.h"
#include "ds_3gpp_nv_manager.h"
#include "ds_3gpp_apn_table.h"
#include "ds_3gpp_hdlr.h"
#include "dstask_v.h"
#include "ds3gmgr.h"
#include "ds3gmshif.h"
#include "ds3gflowmgr.h"
#include "ds3gcfgmgr.h"
#include "ds_3gpp_dyn_mem_hdlr.h"
#include "ds_loopback_hdlr.h"
#ifdef FEATURE_DATA_EMBMS
#include "ds_3gpp_embms_hdlr.h"
#endif /*FEATURE_DATA_EMBMS*/
#ifdef FEATURE_DATA_WCDMA_PS
#include "ds_wcdma_bearer_context.h"
#endif
#include "ds_flow_control.h"
#include "ran.h"
#include "ps_utils.h"
#include "ds_3gpp_flow_context.h"
#include "ds_3gpp_bearer_flow_manager.h"
#include "ds_3gpp_flow_manager.h"
#include "dsmsgr.h"
#include "ds_3gpp_msg.h"
#include "ds_dsd_apm_ext_i.h"
#include "ps_dpm.h"
#include "ps_sys.h"
#include "ds3gsubsmgr.h"


#ifdef FEATURE_DATA_LTE
#include "ds_eps_bearer_context.h"
#include "ds_eps_pdn_context.h"
#include "ds_eps_tlb.h"
#include "lte_rrc_ext_msg.h"
#endif /* FEATURE_DATA_LTE */

#ifdef FEATURE_DATA_WCDMA_PS
#include "ds_umts_bearer_context.h"  
#endif

#ifdef FEATURE_DATA_TE_MT_PDP
#include "dsumtspsmthdlr.h"
#endif /* FEATURE_DATA_TE_MT_PDP */
#include "ds_3gpp_throttle_sm.h"
#include "ds3gpp_api.h"


#include "event_defs.h"
#include "ps_wmk_logging.h"
#include "ps_crit_sect.h"
#include "ps_iface_defs.h"
#include "ds_3gpp_pdn_throttle_sm.h"
#include "ds_3gpp_network_override_hdlr.h"
#include "pstimer.h"
#include "ps_sys_conf.h"

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define DS_BEARER_CTXT_VFR_CALL(rval, func, subs_id, ...)  \
          DS_3GPPI_VFR_CALL(ds_bearer_context_ftbl, rval, func, subs_id, \
                            __VA_ARGS__)

#define DS_BEARER_CTXT_VFR_CALL_NO_ARG(rval, func, subs_id)  \
          DS_3GPPI_VFR_CALL_NO_ARG(ds_bearer_context_ftbl, rval, func, subs_id)

#define DS_BEARER_CTXT_VF_CALL_NO_ARG(func, subs_id)  \
          DS_3GPPI_VF_CALL_NO_ARG(ds_bearer_context_ftbl, func, subs_id)

#define DS_BEARER_CTXT_VFR_CALL_PER_BEARER(rval, func, bearer_context_p, ...)  \
            DS_3GPPI_VFR_CALL_PER_BEARER(ds_bearer_context_ftbl, rval, func, bearer_context_p, __VA_ARGS__)

#define DS_BEARER_CTXT_VF_CALL_PER_BEARER(func, bearer_context_p, ...)  \
            DS_3GPPI_VF_CALL_PER_BEARER(ds_bearer_context_ftbl, func, bearer_context_p, __VA_ARGS__)

#define DS_BEARER_CTXT_VF_CALL_NO_ARG_PER_BEARER(func, bearer_context_p )  \
            DS_3GPPI_VF_CALL_NO_ARG_PER_BEARER(ds_bearer_context_ftbl, func, bearer_context_p)

/*  Default timer values for fast dormancy timers in Milli Seconds 
  if EFS Reading fails */
#define DS_3GPP_FD_DEFAULT_OVERALL_TIMER_VALUE   6000 

#define DS_3GPP_FD_DEFAULT_AGRESSIVE_TIMER_VALUE 1000

#define DS_3GPP_MIN_IP_PACKET_SIZE               20

/*===========================================================================
   This Structure is used to maintain all global variables of
   ds_3gpp_bearer_context.
   PLEASE USE THIS STRUCTURE WHENEVER YOU WANT TO ADD A NEW GLOBAL VARIABLE.
===========================================================================*/
typedef struct
{
  ds_bearer_cntxt_per_subs_info_type *per_subs_info_p[DS3GSUBSMGR_SUBS_ID_MAX];

  /*----------------------------------------------------------------------------
  Bearer context table. This table contains all the bearers. It is this table
  that is updated whenever there is a change in a bearer context.

  Out of the DS_3GPP_MAX_BEARER_CONTEXT declared here, 
  DS_3GPP_MAX_BEARER_CONTEXT_PER_PDN_CONTEXT bearers are tied to one PDN cntx.
  Out of these bearers tied to the same PDN context, one is default and the 
  others are treated to be secondary calls.
  ----------------------------------------------------------------------------*/
  ds_bearer_context_s           bearer_context_tbl[DS_3GPP_MAX_BEARER_CONTEXT];
   
  /*-----------------------------------------------------------------------------
    This flag is used to keep track of mem_ctrl_ev posted to DS task. This is to 
    avoid posting multiple commands to DS task when a mem_ctrl_ev is already 
    pending.
   -----------------------------------------------------------------------------*/
  boolean                       mem_ctrl_ev_posted;
} ds_bearer_cntxt_info_type;

/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

/*----------------------------------------------------------------------------
  EPS/umts specific function tables. We allocate memory for SYS_SYS_MODE_MAX
  table. However, we will be using only those corresponding to WCDMA, GPRS
  and EPS.
----------------------------------------------------------------------------*/
ds_bearer_context_ftbl_s ds_bearer_context_ftbl[DS_3GPPI_SYS_MODE_MAX] = {{NULL}};

/* structure to store fast dormancy relatedinformation   */
static ds_3gpp_fd_info_s      ds_3gpp_fd_info;



/*-----------------------------------------------------------------------------
  This table has pointers to the bearer up and down function call backs. 
  The declaration of this call back table and populating the same with the 
  appropriate function pointers happends in PDN context. These call back fns
  are also implemented in the PDN context. 
-----------------------------------------------------------------------------*/
extern bearer_context_notifier_cb_tbl_s bearer_context_notifier_cb_tbl;

/*===========================================================================
   This Structure is used to maintain all global variables of
   ds_3gpp_bearer_context.c
   PLEASE USE THIS STRUCTURE WHENEVER YOU WANT TO ADD A NEW GLOBAL VARIABLE.
===========================================================================*/
static ds_bearer_cntxt_info_type ds_bearer_cntxt_info = {{NULL}};

static boolean access_barring_rab_retry = FALSE;
/*===========================================================================
  Cookie to ensure that dynamic memory allocated is valid
===========================================================================*/
#define DS_BEARER_CNTXT_PER_SUBS_INFO_COOKIE 0xdefdefde

static uint16  access_barring_rab_retry_timer = 
               DS_BEARER_CNTXT_RAB_REESTABLISH_MIN_RETRY_TIME;

static uint16  access_barring_rab_retry_counter = 
               DS_BEARER_CNTXT_ACCESS_BARR_RAB_RETRY_COUNTER;




/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_SET_PER_SUBS_INFO_HNDL

DESCRIPTION
  This function sets the handle to the Bearer Context Information
  corresponding to the given Subscription.

PARAMETERS
  subs_id          : Subscription Id
  hndl             : Handle to the Bearer Context Information for
                     the given Subscription.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void ds_bearer_cntxt_set_per_subs_info_hndl
(
  sys_modem_as_id_e_type               subs_id,
  ds_bearer_cntxt_per_subs_info_type  *hndl
);

#ifdef FEATURE_DATA_LTE

/*===========================================================================
FUNCTION ds_bearer_cntxt_lte_deactivate_req_hdlr

DESCRIPTION
  This function listens to LTE Deactivated Request from NAS . 
  DS use this as an indication of LTE de-activation and clears all pending  redial calls.

  
PARAMETERS  
  msgrtype - the id of the received message 
  dsmsg    - pointer to the received message 

DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_bearer_cntxt_lte_deactivate_req_hdlr
(
  msgr_umid_type               msgrtype,
  const msgr_hdr_struct_type * dsmsg
);
#endif /* FEATURE_DATA_LTE */

/*===========================================================================
FUNCTION ds_bearer_cntxt_post_clr_pending_calls_cmd

DESCRIPTION
  This function posts a command to DS Task to clear pending calls if any.
  
PARAMETERS  
  subs_id: Subscription Id

DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_bearer_cntxt_post_clr_pending_calls_cmd
(
  sys_modem_as_id_e_type subs_id
);

 /*=============================================================================
FUNCTION DS_BEARER_CNTXT_DPM_EV_CB

DESCRIPTION
  This is the call back function that is called when the Dpm events are invoked.
  Based on these events; the watermark's nonemmpty function callback is set
  reset.
PARAMETERS
  IN
    dpm_event       - The event that happened. This is used to differentiate
                      the events that happened : registration, deregistration
                      reregistration
    event_info_ptr   - Passed in as void*, this gives us the event details 
            
DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/

void ds_bearer_cntxt_dpm_ev_cb
(
  ps_dpm_um_ev_enum_type             dpm_event, 
  void * event_info_ptr
);


/*===========================================================================

FUNCTION DS_BEARER_CNTXT_FLUSH_PS_L2_WATERMARKS

DESCRIPTION
  This function flushes PS-L2 watermarks

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_bearer_cntxt_flush_ps_l2_watermarks
(
  void
);

/*===========================================================================
FUNCTION ds_3gpp_bearer_cntx_alloc_dyn_mem

DESCRIPTION
  This function allocates dyn memory.
  
PARAMETERS  
  index   - index where the memory would be allocated
  subs_id - subscription id

DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

=============================================================================*/
static boolean ds_bearer_cntx_alloc_dyn_mem
(
  int                    index,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION ds_bearer_cntxt_get_mem_ctrl_ev_posted_flag

DESCRIPTION
  This function gets the Mem Ctrl Event posted Flag
 
PARAMETERS
  None
    
DEPENDENCIES
  None

RETURN VALUE
  TRUE - If Mem Cntrl Event was already posted
  FALSE- Otherwise
   
SIDE EFFECTS 
  None
  
=============================================================================*/
static boolean ds_bearer_cntxt_get_mem_ctrl_ev_posted_flag
(
  void
);

/*===========================================================================
FUNCTION ds_bearer_cntxt_set_mem_ctrl_ev_posted_flag

DESCRIPTION
  This function sets the Mem Ctrl Event posted Flag
 
PARAMETERS
  flag: Mem Cntrl Event posted Flag
    
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
static void ds_bearer_cntxt_set_mem_ctrl_ev_posted_flag
(
  boolean flag
);

/*===========================================================================
FUNCTION DS_3GPP_BEARER_CNTXT_OK_TO_RELEASE_CONNECTION

DESCRIPTION
  This function checks if it is OK to Relese signalling connection with NW 

PARAMETERS
None
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE-- if YES
  FALSE- if NO

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_3gpp_bearer_cntxt_ok_to_release_connection
(
  void
);

/*===========================================================================
FUNCTION DS_3GPP_BEARER_CNTXT_IS_ANY_BEARER_UP

DESCRIPTION
  This function checks if any bearer is in UP state or not

PARAMETERS
None
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE-- if YES
  FALSE- if NO

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_3gpp_bearer_cntxt_is_any_bearer_up
(
  void
);

/*===========================================================================
FUNCTION  ds_3gpp_bearer_cntxt_init_fast_domrnacy_params

DESCRIPTION
  This function will initialize the fast dormancy paramters. 
  Enabled flag and Timer values. If GCF NV is enabled and FD 
  is running few GCF test cases will fail

PARAMETERS
  None
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_bearer_cntxt_init_fast_domrnacy_params
(
  void
);

#ifdef FEATURE_DATA_LTE
/*===========================================================================
FUNCTION  DS_BEARER_CNTXT_RESEND_CALL_END

DESCRIPTION
  This function sends pdn_disconnect as CM returned error for CALL_CMD_END
  with cause CM_CALL_CMD_ERR_MODE_INFO_CHG_LTE.

PARAMETERS 
  call_id - call_id for which CALL_END error is returned.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void ds_bearer_cntxt_resend_call_end
(
  byte  call_id
);
#endif /* FEATURE_DATA_LTE */

/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION ds_bearer_cntxt_setup_sw_path

DESCRIPTION
  This function sets up the SW path instead of HW in case of DPM registration
  failure or Memory allocation failure while DPM registration
  
PARAMETERS
  bearer_context_p       - bearer_context to be registered

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds_bearer_cntxt_setup_sw_path
(
   ds_bearer_context_s                  *bearer_context_ptr
)
{
  dsm_watermark_type                    *wm_ptr = NULL;
/*-------------------------------------------------------------------------*/
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_ptr))
  {
    return;
  }

  switch (bearer_context_ptr->ds_bearer_context_dyn_p->call_mode)
  {
    case SYS_SYS_MODE_LTE:
      wm_ptr = &(bearer_context_ptr->lte_rx_wm.wm_item);
      break;

    case SYS_SYS_MODE_WCDMA:
    case SYS_SYS_MODE_GSM:
    case SYS_SYS_MODE_TDS:
      wm_ptr = &(bearer_context_ptr->rx_wm.wm_item);
      break;

    default:
      DS_3GPP_MSG0_ERROR ("Invalid 3GPP RAT, SW path not setup");
      break;
  }

  if(wm_ptr != NULL)
  {
    wm_ptr->non_empty_func_ptr = 
      (wm_cb_type)ds_3gpp_bearer_cntxt_rx_data_at_u_pdp_cb;
    wm_ptr->non_empty_func_data = NULL;
  }
}

/*===========================================================================
FUNCTION   ds_bearer_cntxt_dpm_setup

DESCRIPTION
  This function is used to setup registration and re-registration info with
  DPM.
  
PARAMETERS  
  bearer_context_p:                 Bearer context
  info:                             Reg or Re-reg info to be setup

DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE:           If Successful
  FALSE:          On Failure
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_bearer_cntxt_dpm_setup
(
  ds_bearer_context_s                *bearer_context_p,
  boolean                             api_based_flag,
  void                               *info
)
  {
  boolean                             rval = FALSE;
  ds_sys_rat_ex_enum_type             rat = DS_SYS_RAT_EX_3GPP_MAX;
  ps_dpm_um_rereg_bearer_info_type   *rereg_info = NULL;
  ps_dpm_um_reg_bearer_info_type     *reg_info = NULL;
  dsm_watermark_type                 *wm_ptr = NULL;
/*------------------------------------------------------------------------*/
  if (!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return rval;
  }

  if (info == NULL)
  {
    DS_3GPP_MSG0_ERROR ("Input Param: Info invalid");
    return rval;
  }

  switch( bearer_context_p->ds_bearer_context_dyn_p->call_mode)
  {
     case SYS_SYS_MODE_WCDMA:
      rat = DS_SYS_RAT_EX_3GPP_WCDMA;
      wm_ptr = &(bearer_context_p->rx_wm.wm_item);
      rval = TRUE;
       break;

     case SYS_SYS_MODE_TDS:
      rat = DS_SYS_RAT_EX_3GPP_TDSCDMA;
      wm_ptr = &(bearer_context_p->rx_wm.wm_item);
      rval = TRUE;
       break;

     case SYS_SYS_MODE_GSM:
      rat = DS_SYS_RAT_EX_3GPP_GERAN;
      wm_ptr = &(bearer_context_p->rx_wm.wm_item);
      rval = TRUE;
       break;

    case SYS_SYS_MODE_LTE:
      rat = DS_SYS_RAT_EX_3GPP_LTE;
      wm_ptr = &(bearer_context_p->lte_rx_wm.wm_item);
      rval = TRUE;
      break;

    default:
      DS_3GPP_MSG0_ERROR ("Invalid SYS Mode for Bearer");
      break;
  }

  if(rval)
  {
    if(api_based_flag)
    {
      rereg_info = (ps_dpm_um_rereg_bearer_info_type *)info;
      rereg_info->rat = (ps_sys_rat_ex_enum_type)rat;
      rereg_info->l2_to_ps_wm_ptr =  wm_ptr;
    }
    else
    {
      reg_info = (ps_dpm_um_reg_bearer_info_type *)info;
      reg_info->rat = (ps_sys_rat_ex_enum_type)rat;
      reg_info->l2_to_ps_wm_ptr =  wm_ptr;
    }
  }
  return rval;
}

/*===========================================================================
FUNCTION ds_bearer_cntx_register_dpm

DESCRIPTION
  This function registers the bearer WM with dpm module

PARAMETERS
  bearer_context_p       - bearer_context to be registered

DEPENDENCIES
  None.

RETURN VALUE


SIDE EFFECTS
  None

===========================================================================*/
void ds_bearer_cntx_register_dpm
(
   ds_bearer_context_s                *bearer_context_p
)
{
  ds_pdn_context_s                     *pdn_cntx_p = NULL;
  ps_dpm_um_reg_bearer_info_type       *reg_info = NULL; 
  int32                                 ps_dpm_reg_ret = 0;
  boolean                               rval = FALSE;
/*--------------------------------------------------------------------------*/

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }

  pdn_cntx_p = bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr;
  
  if (!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return;
  }

  do
  {
    reg_info = (ps_dpm_um_reg_bearer_info_type *)modem_mem_alloc 
               (sizeof(ps_dpm_um_reg_bearer_info_type), MODEM_MEM_CLIENT_DATA);

    if (reg_info == NULL)
    {
      DATA_ERR_FATAL("Memory allocation failure, Cannot Re-register with DPM so reverting to SW path");
      ds_bearer_cntxt_setup_sw_path (bearer_context_p);
      break;
    }
    
    memset(reg_info, 0, sizeof(ps_dpm_um_reg_bearer_info_type));

    reg_info->v4_iface_ptr = DSPDNCNTXT_GET_V4_PS_IFACE_PTR(pdn_cntx_p);
    reg_info->v6_iface_ptr = DSPDNCNTXT_GET_V6_PS_IFACE_PTR(pdn_cntx_p);

    rval = ds_bearer_cntxt_dpm_setup(bearer_context_p,FALSE,(void *)reg_info);

    if (!rval)
    {
      DS_3GPP_MSG1_ERROR ("Invalid Registration Info for DPM: 0x%x",reg_info);
      ASSERT (0);
      break;
    }

    reg_info->ps_phys_link_ptr = &(bearer_context_p->phys_link);
    ps_dpm_reg_ret = ps_dpm_um_reg_bearer(reg_info);

    if (DSS_SUCCESS == ps_dpm_reg_ret)
    {
      DS_3GPP_MSG1_HIGH("Dpm Registration Succesful for Phys Link 0x%x",
                        reg_info->ps_phys_link_ptr);
      bearer_context_p->ds_bearer_context_dyn_p->is_registered_with_dpm = TRUE;
    }
    else
    {
      DS_3GPP_MSG1_ERROR ("DPM Registration for Phys Link:0x%x failed",
                          reg_info->ps_phys_link_ptr);
      ASSERT (0);
      ds_bearer_cntxt_setup_sw_path(bearer_context_p);
    }
  }while (0);

  if(reg_info != NULL)
  {
    modem_mem_free ((void *)reg_info, MODEM_MEM_CLIENT_DATA);
  }
}

/*===========================================================================
FUNCTION ds_bearer_cntx_register_dpm_tlb_mode

DESCRIPTION
  This function registers the bearer WM with dpm module in TLB Mode A

PARAMETERS
  bearer_context_p       - bearer_context to be registered

DEPENDENCIES
  None.

RETURN VALUE


SIDE EFFECTS
  None

===========================================================================*/
void ds_bearer_cntx_register_dpm_tlb_mode
(
   ds_bearer_context_s                *bearer_context_p
)
{
  ds_pdn_context_s                     *pdn_cntx_p = NULL;
  ps_dpm_um_reg_tlb_bearer_info_type   *tlb_reg_info = NULL; 
  int32                                 ps_dpm_reg_ret = 0;
  dsm_watermark_type                   *wm_ptr = NULL;
  ds_bearer_cntxt_um_wm_threshold_struct_type  um_wm_threshold;
  sys_modem_as_id_e_type   subs_id= SYS_MODEM_AS_ID_NONE;
/*--------------------------------------------------------------------------*/

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }

  pdn_cntx_p = bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr;
  
  if (!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return;
  }

  if(! ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p,&subs_id))
  {
    return;
  }

  um_wm_threshold = ds_3gpp_nv_get_um_wm_threshold(subs_id);  
  wm_ptr = &(bearer_context_p->tlb_rx_wm.wm_item);

  dsm_empty_queue (wm_ptr);

  dsm_set_low_wm(wm_ptr, um_wm_threshold.lte_ul_lo);
  dsm_set_hi_wm(wm_ptr, um_wm_threshold.lte_ul_hi);
  dsm_set_dne(wm_ptr, um_wm_threshold.lte_ul_dne);
  dsm_set_dne_q_cnt(wm_ptr, DSLTEPS_UL_WM_DEFAULT_DNE_QCNT);

  do
  {
    tlb_reg_info = (ps_dpm_um_reg_tlb_bearer_info_type *)modem_mem_alloc 
                   (sizeof(ps_dpm_um_reg_tlb_bearer_info_type), 
                    MODEM_MEM_CLIENT_DATA);

    if (tlb_reg_info == NULL)
    {
      DS_3GPP_MSG0_ERROR ("Memory allocation failure, Cannot Re-register with "
                          "DPM");
      break;
    }
    
    memset(tlb_reg_info, 0, sizeof(ps_dpm_um_reg_tlb_bearer_info_type));

    tlb_reg_info->v4_iface_ptr = DSPDNCNTXT_GET_V4_PS_IFACE_PTR(pdn_cntx_p);
    tlb_reg_info->v6_iface_ptr = DSPDNCNTXT_GET_V6_PS_IFACE_PTR(pdn_cntx_p);

    tlb_reg_info->rat = PS_SYS_RAT_EX_3GPP_LTE;
    tlb_reg_info->l2_to_ps_wm_ptr = &(bearer_context_p->lte_rx_wm.wm_item);

    tlb_reg_info->ps_phys_link_ptr = &(bearer_context_p->phys_link);
    tlb_reg_info->hw_to_ps_wm_ptr = &(bearer_context_p->tlb_rx_wm.wm_item);

    ps_dpm_reg_ret = ps_dpm_um_reg_tlb_bearer(tlb_reg_info);

    if (DSS_SUCCESS == ps_dpm_reg_ret)
    {
      DS_3GPP_MSG1_HIGH("Dpm Registration Succesful for Phys Link 0x%x",
                        tlb_reg_info->ps_phys_link_ptr);
      bearer_context_p->ds_bearer_context_dyn_p->is_registered_with_dpm = TRUE;
    }
    else
    {
      DS_3GPP_MSG1_ERROR ("DPM Registration for Phys Link:0x%x failed",
                          tlb_reg_info->ps_phys_link_ptr);
    }
  }while (0);

  if(tlb_reg_info != NULL)
  {
    modem_mem_free ((void *)tlb_reg_info, MODEM_MEM_CLIENT_DATA);
  }
}

/*===========================================================================
FUNCTION ds_bearer_cntx_deregister_dpm

DESCRIPTION
  This function deregisters the bearer with dpm module

PARAMETERS
  bearer_context_p       - bearer_context to be registered
    
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_bearer_cntx_deregister_dpm
(
  ds_bearer_context_s                  *bearer_context_p
)
{
  ds_pdn_context_s                     *pdn_cntx_p;
  ps_dpm_um_dereg_bearer_info_type     *dereg_info = NULL; 
  int32                                 ps_dpm_dereg_ret = 0;
/*-------------------------------------------------------------------------*/
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }

  pdn_cntx_p = bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr;

  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return;
  }
  do
  {
    dereg_info = (ps_dpm_um_dereg_bearer_info_type *)modem_mem_alloc 
                 (sizeof(ps_dpm_um_dereg_bearer_info_type), 
                  MODEM_MEM_CLIENT_DATA);

    /*-------------------------------------------------------------------- 
     Is an Assert needed in case of Memory allocation failure, i feel just
     a memory allocation failure should not stop de-registration.
     ASSERT will help us catch us a Memory leak 
    ---------------------------------------------------------------------*/
    if (dereg_info == NULL)
    {
      DATA_ERR_FATAL("Memory allocation failure, Cannot De-register with DPM");
      break;
    }

    memset (dereg_info, 0, sizeof(ps_dpm_um_dereg_bearer_info_type));
  
    dereg_info->v4_iface_ptr = DSPDNCNTXT_GET_V4_PS_IFACE_PTR(pdn_cntx_p);
    dereg_info->v6_iface_ptr = DSPDNCNTXT_GET_V6_PS_IFACE_PTR(pdn_cntx_p);
  
    dereg_info->ps_phys_link_ptr = &(bearer_context_p->phys_link);
    ps_dpm_dereg_ret = ps_dpm_um_dereg_bearer(dereg_info); 

    if (DSS_SUCCESS == ps_dpm_dereg_ret)
    {
      DS_3GPP_MSG1_HIGH("DPM De-Registration Succesful for Phys Link: 0x%x",
                        dereg_info->ps_phys_link_ptr);
    }
    else
    {
      DS_3GPP_MSG1_ERROR ("DPM De-registration failed for Phys Link: 0x%x",
                          dereg_info->ps_phys_link_ptr);
    }
  }while (0);

  if(dereg_info != NULL)
  {
    modem_mem_free ((void *)dereg_info, MODEM_MEM_CLIENT_DATA);
  }
}
/*===========================================================================
FUNCTION ds_bearer_cntx_deregister_dpm_iface

DESCRIPTION
  This function deregisters the iface with dpm module for the associated
  bearers

PARAMETERS
  bearer_context_p       - bearer_context to be registered
  iface_ptr              - iface to be derigstered
    
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void ds_bearer_cntx_deregister_dpm_iface
(
   ds_pdn_context_s                 *pdn_cntx_p,
   ps_iface_type                    *iface_ptr
)
{
  ps_dpm_um_dereg_iface_info_type   *dereg_info = NULL;
  int32                              ps_dpm_dereg_ret = 0; 
/*--------------------------------------------------------------------------*/

  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return;
  }

  if (iface_ptr == NULL)
  {
    DS_3GPP_MSG3_ERROR("Iface passed is Invalid",0,0,0);
    return;
  }

  do
  {
    dereg_info = (ps_dpm_um_dereg_iface_info_type *)modem_mem_alloc 
                 (sizeof(ps_dpm_um_dereg_iface_info_type), 
                  MODEM_MEM_CLIENT_DATA);
    
    if (dereg_info == NULL)
    {
      DATA_ERR_FATAL("Unable to allocate heap memory, de-registration failure");
      break;
    } 

    memset(dereg_info, 0, sizeof(ps_dpm_um_dereg_iface_info_type));
  
    if(iface_ptr == DSPDNCNTXT_GET_V4_PS_IFACE_PTR(pdn_cntx_p))
    {
      dereg_info->v4_iface_ptr = iface_ptr;
    }
    else
    {
      dereg_info->v6_iface_ptr = iface_ptr;
    }

    ps_dpm_dereg_ret = ps_dpm_um_dereg_iface(dereg_info);

    if (DSS_SUCCESS == ps_dpm_dereg_ret)
    {
      DS_3GPP_MSG1_HIGH("DPM De-Registration Succesful for Iface: 0x%x",
                        iface_ptr);
    }
    else
    {
      DS_3GPP_MSG1_ERROR ("DPM De-registration failed for Iface: 0x%x",
                          iface_ptr);
    }
  }while (0);

  if(dereg_info != NULL)
  {
    modem_mem_free ((void *)dereg_info, MODEM_MEM_CLIENT_DATA);
  }
}
/*===========================================================================
FUNCTION ds_bearer_cntx_reregister_dpm

DESCRIPTION
  This function reregisters the iface with dpm module
  in case of a context transfer
PARAMETERS
  bearer_context_p       - bearer_context to be registered
  iface_ptr              - iface to be derigstered     
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void  ds_bearer_cntx_reregister_dpm
(
  ds_bearer_context_s *bearer_context_p
)
{
  ps_dpm_um_rereg_bearer_info_type        *rereg_info;
  int32                                    ps_dpm_rereg_ret = 0;
  ds_pdn_context_s                        *pdn_cntx_p = NULL;
  boolean                                  rval = FALSE;
/*-------------------------------------------------------------------------*/
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }

  pdn_cntx_p = bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr;

  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return;
  } 

  do
  {
    rereg_info = (ps_dpm_um_rereg_bearer_info_type *)modem_mem_alloc 
                 (sizeof(ps_dpm_um_rereg_bearer_info_type), 
                  MODEM_MEM_CLIENT_DATA);

    if (rereg_info == NULL)
    {
      DS_3GPP_MSG0_ERROR ("Memory allocation failure, Cannot Re-register with "
                          "DPM");
      ds_bearer_cntxt_setup_sw_path(bearer_context_p);
      break;
    }

    memset(rereg_info, 0, sizeof(ps_dpm_um_rereg_bearer_info_type));

    rereg_info->v4_iface_ptr = DSPDNCNTXT_GET_V4_PS_IFACE_PTR(pdn_cntx_p);
    rereg_info->v6_iface_ptr = DSPDNCNTXT_GET_V6_PS_IFACE_PTR(pdn_cntx_p);

    rval = ds_bearer_cntxt_dpm_setup(bearer_context_p,TRUE,(void *)rereg_info);

    if (!rval)
    {
      DS_3GPP_MSG1_ERROR ("Invalid Registration Info for DPM: 0x%x",&rereg_info);
      break;
    }

    rereg_info->ps_phys_link_ptr = &(bearer_context_p->phys_link);
    ps_dpm_rereg_ret = ps_dpm_um_rereg_bearer(rereg_info); 

    if (DSS_SUCCESS == ps_dpm_rereg_ret)
    {
      DS_3GPP_MSG1_HIGH("Dpm ReRegistration Succesful for Phys Link 0x%x",
                        rereg_info->ps_phys_link_ptr);
    }
    else
    {
      DS_3GPP_MSG0_ERROR ("Re-registration with DPM failed during IRAT");
      ds_bearer_cntxt_setup_sw_path(bearer_context_p);
    }
  }while (0);

  if(rereg_info != NULL)
  {
    modem_mem_free ((void *)rereg_info, MODEM_MEM_CLIENT_DATA);
  }
}

/*===========================================================================
FUNCTION ds_bearer_cntxt_get_subs_id_from_bearer_internal

DESCRIPTION
  This function fetches the subs id to which the bearer belongs. There will
  be no F3 if bearer context is invalid
 
PARAMETERS
  bearer_context_p       - bearer_context
  subs_id_p              - subscription id
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if successfully fetched subs Id
  FALSE, otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_bearer_cntxt_get_subs_id_from_bearer_internal
(
  ds_bearer_context_s    *bearer_context_p,
  sys_modem_as_id_e_type *subs_id_p
)
{
  boolean                 ret_val = FALSE;
  /*----------------------------------------------------------------------*/ 

  if(ds_bearer_cntx_validate_bearer_context_internal(bearer_context_p))
  {
    if (subs_id_p != NULL && 
          ds3gsubsmgr_is_subs_id_valid
            (bearer_context_p->ds_bearer_context_dyn_p->subs_id))
    {
      *subs_id_p = bearer_context_p->ds_bearer_context_dyn_p->subs_id ;
       ret_val = TRUE;
    }
  }
  return ret_val;
} /* ds_bearer_cntxt_get_subs_id_from_bearer_internal */

/*===========================================================================
FUNCTION ds_bearer_cntxt_get_subs_id_from_bearer

DESCRIPTION
  This function fetches the subs id to which the bearer belongs.
 
PARAMETERS
  bearer_context_p       - bearer_context
  subs_id_p              - subscription id
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if successfully fetched subs Id
  FALSE, otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_bearer_cntxt_get_subs_id_from_bearer
(
  ds_bearer_context_s    *bearer_context_p,
  sys_modem_as_id_e_type *subs_id_p
)
{
  boolean                 ret_val = FALSE;
  /*----------------------------------------------------------------------*/ 

  if(ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    if (subs_id_p != NULL && 
          ds3gsubsmgr_is_subs_id_valid
            (bearer_context_p->ds_bearer_context_dyn_p->subs_id))
    {
      *subs_id_p = bearer_context_p->ds_bearer_context_dyn_p->subs_id ;
       DS_3GPP_MSG1_MED("getting Subs_id from PDN Subs Id(CM):%d",*subs_id_p);
       ret_val = TRUE;
    }
  }
  return ret_val;
} /* ds_bearer_cntxt_get_subs_id_from_bearer */

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_CLEANUP_PENDING_CALLS

DESCRIPTION
  This function cleans up bearers in PEND_REDIAL state after a rat change. 
  
PARAMETERS  
  bearer_context_p - bearer context pointer
  new_mode         - new_rat
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
static void ds_bearer_cntxt_cleanup_pending_calls
(
  ds_bearer_context_s  *bearer_context_p,
  sys_sys_mode_e_type  new_mode
);

/*===========================================================================

FUNCTION DS_BEARER_CNTXT_IS_ANY_OTHER_BEARER_UP

DESCRIPTION
  This function checks if any other bearer is in UP/UP_DORMANT state

PARAMETERS  
  Bearer Context ptr

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_bearer_cntx_is_any_other_bearer_up
(
  ds_bearer_context_s *bearer_context_p
);

/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_FREE_BEARER_INST

DESCRIPTION
  This function cleans up the bearer instance on a call end.

PARAMETERS
  IN
    bearer_context_p - Pointer to the bearer context to be cleaned up.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void ds_bearer_cntxt_free_bearer_inst
(
  ds_bearer_context_s* bearer_context_p
)
{
  sys_sys_mode_e_type    mode = SYS_SYS_MODE_NONE;
  sys_modem_as_id_e_type subs_id = SYS_MODEM_AS_ID_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds3gpp_enter_global_crit_section();
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    ds3gpp_leave_global_crit_section();
    return;
  }

  if (!ds_bearer_cntxt_get_subs_id_from_bearer(bearer_context_p, &subs_id))
  {
    ASSERT(0);
  }
  else
  {
    mode = ds3gpp_get_current_network_mode(subs_id);
  }

  ds_3gpp_dyn_mem_hdlr_free_bearers_dyn_mem(bearer_context_p, subs_id);

  /*---------------------------------------------------------------------------
    Resetting all the fields to their default values.
  ---------------------------------------------------------------------------*/
  bearer_context_p->ds_bearer_context_dyn_p->state = DS_BEARER_CONTEXT_STATE_DOWN;
  bearer_context_p->ds_bearer_context_dyn_p->cm_call_id = CM_CALL_ID_INVALID;
  bearer_context_p->ds_bearer_context_dyn_p->nsapi = 0;
  bearer_context_p->ds_bearer_context_dyn_p->call_mode = mode;
  bearer_context_p->ds_bearer_context_dyn_p->is_transferable_to_GW = TRUE;
  bearer_context_p->ds_bearer_context_dyn_p->is_registered_with_dpm = FALSE;
  bearer_context_p->ds_bearer_context_dyn_p->
    ul_registered_with_ltepdcp = FALSE;
  bearer_context_p->rm_flow_enabled = TRUE;
  bearer_context_p->ds_bearer_context_dyn_p->reorig_cmd_enqueued = FALSE;
#ifdef FEATURE_DATA_IMS
  bearer_context_p->ds_bearer_context_dyn_p->im_cn_subsystem_flag = FALSE;
#endif
  bearer_context_p->ds_bearer_context_dyn_p->srv_rsp_exp = FALSE;

  /*Resetting the bearer id in the Phys Link strucute during bearer free*/
  PS_PHYS_LINK_SET_BEARER_ID(&(bearer_context_p->phys_link),0);

  /* 
    Set the SDF ID to be invalid. This would come in handy should there be
    a bug while reusing the bearer contexts
  */
  bearer_context_p->ds_bearer_context_dyn_p->sdf_id = 0xFFFF;
  /*-------------------------------------------------------------------------
    We should not set the in_use flag to FALSE here. It will always be done
    from the PDN context.
  -------------------------------------------------------------------------*/
  /*------------------------------------------------------------------------
    Reset the values of the byte counters to zero
  ------------------------------------------------------------------------*/
  bearer_context_p->ds_bearer_context_dyn_p->tx_bytes = 0;
  bearer_context_p->ds_bearer_context_dyn_p->rx_bytes = 0;

  bearer_context_p->ds_bearer_context_dyn_p->wm_flush_state = DS_BEARER_CONTEXT_UM_WM_STATE_INVALID;
  /*-----------------------------------------------------------------------
    Reset the rab_reestab_retry counter for this bearer.
  -----------------------------------------------------------------------*/
  DS_BEARER_CNTXT_RESET_RAB_REESTAB_RETRY_CNT(bearer_context_p);

  /*----------------------------------------------------------------------
    Set each enqueue func ptr to NULL here
  ----------------------------------------------------------------------*/
  (bearer_context_p->tx_wm.wm_item).each_enqueue_func_ptr = NULL;
  (bearer_context_p->tx_wm.wm_item).each_enqueue_func_data = NULL;

  /*----------------------------------------------------------------------
    Set dl's non empty func ptr to NULL here
  ----------------------------------------------------------------------*/
  (bearer_context_p->rx_wm.wm_item).non_empty_func_ptr = NULL;
  (bearer_context_p->rx_wm.wm_item).non_empty_func_data = NULL;

  (bearer_context_p->lte_rx_wm.wm_item).non_empty_func_ptr = NULL;
  (bearer_context_p->lte_rx_wm.wm_item).non_empty_func_data = NULL;

  (bearer_context_p->tlb_rx_wm.wm_item).non_empty_func_ptr = NULL;
  (bearer_context_p->tlb_rx_wm.wm_item).non_empty_func_data = NULL;
  ds3gpp_leave_global_crit_section();

} /* ds_bearer_cntxt_free_bearer_inst() */

/*===========================================================================
FUNCTION      DS_BEARER_CNTXT_PHYS_LINK_DOWN

DESCRIPTION
  Called by the embedded application.  Brings down the EPS/UMTS traffic 
  channel. It is also called on IFace Down command.

PARAMETERS
  phys_link_ptr - phys link ptr for the call
  info_ptr      - not used

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
int ds_bearer_cntxt_phys_link_down
(
  ps_phys_link_type *phys_link_ptr_i,
  void              *info_ptr
)
{
  const ps_phys_link_type *phys_link_ptr = phys_link_ptr_i;
  ds_cmd_type     *cmd_ptr;                      /* DS Task message        */
  uint32          call_instance;
  ds_3gpp_pdn_call_info_type *call_info_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_instance =  (uint32)(phys_link_ptr->client_data_ptr);

  DS_3GPP_MSG1_HIGH("DSUMTSPS PHYS LINK DOWN CMD :Call Inst:%d", call_instance);

  /*------------------------------------------------------------------------
    Phys link down command handler for secondary. However reuse existing
    DS3GPP_PHYS_LINK_DOWN_CMD event. The flags set in the call table will
    give enough information to differentiate between a secondary and a
    primary
  ------------------------------------------------------------------------*/

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_3gpp_pdn_call_info_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return (-1);
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_PHYS_LINK_DOWN;
  call_info_ptr = (ds_3gpp_pdn_call_info_type*)cmd_ptr->cmd_payload_ptr;

  call_info_ptr->call_instance = call_instance;
  call_info_ptr->info_ptr = info_ptr;

    ds_put_cmd_ext(cmd_ptr);

  return(0);
} /* ds_bearer_cntxt_phys_link_down() */

/*===========================================================================
                             PHYS_LINK_DOWN, PHYS_LINK_NULL Functions
===========================================================================*/

/*===========================================================================
FUNCTION  DS_BEARER_CNTXT_PHYS_LINK_DOWN_CMD_HDLR

DESCRIPTION
  Called in DS TASK context in response to a DS_BEARER_CNTXT_PHYS_LINK_DOWN 
  command on the UMTS/EPS iface. From the new implementation, it looks like
  this function need not be handled. There is no one who calls PHYS_LINK_DOWN
  Even the function body has been left empty for now. This function will be 
  removed if it is deemed unnecessary.

PARAMETERS
  call_instance - dummy variable.
  info_ptr      - void pointer

DEPENDENCIES
  Called in DS TASK context.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_bearer_cntxt_phys_link_down_cmd_hdlr
(
  uint32 bearer_instance,
  void *info_ptr
)
{
  ds_bearer_context_s  *bearer_context_p;
  boolean              retVal;
  ds_pdn_context_s *pdn_cntx_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(bearer_instance < DS_3GPP_MAX_BEARER_CONTEXT);

  /*---------------------------------------------------------------------------
    Get the bearer context pointer
  ---------------------------------------------------------------------------*/
  bearer_context_p = ds_bearer_cntxt_get_instance_ptr(bearer_instance);

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }

  pdn_cntx_p =  bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr;
  
  if(ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p)&&
	(pdn_cntx_p->ds_pdn_context_dyn_p->num_active_bearer_context == 0))
  {
    DS_3GPP_MSG0_ERROR("bearer already cleaned up from PDN, returning ...");
    return;
  }
  
  retVal = ds_bearer_cntxt_tear_down(bearer_context_p, info_ptr);
  if(retVal != DS3G_SUCCESS)
  {
    if(bearer_context_p->ds_bearer_context_dyn_p->pdn_context_cb_tbl_ptr
       ->bearer_down_ind_cb != NULL)
    {
      ds_3gpp_down_reason_t  net_down_reason;
      net_down_reason.reason.ps_iface_down_reason = 
        PS_NET_DOWN_REASON_NOT_SPECIFIED;
      net_down_reason.reason_T = DS_3GPP_PS_IFACE_NET_DOWN_REASON_T;
      bearer_context_p->ds_bearer_context_dyn_p->pdn_context_cb_tbl_ptr
        ->bearer_down_ind_cb(
                      bearer_context_p->ds_bearer_context_dyn_p->
                      pdn_context_ptr,\
  		              bearer_context_p,
                    net_down_reason,
		              FALSE);
    }/* Post bearer down indication with call_was_connected as False*/
  }
} /* ds_bearer_cntxt_phys_link_down_cmd_hdlr() */


/*===========================================================================
FUNCTION  DS_BEARER_CNTXT_PHYS_LINK_FLOW_CMD_HDLR

DESCRIPTION
  Called in DS TASK context in response to DS3GPP_PHYS_LINK_FLOW_ENABLE_CMD
  or DS3GPP_PHYS_LINK_FLOW_DISABLE_CMD. Need to flow control the phys link
  appropriately.

PARAMETERS
   cmd_ptr - information relating to the DS comand.

DEPENDENCIES
  Called in DS TASK context.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_bearer_cntxt_phys_link_flow_cmd_hdlr
(
  const ds_cmd_type *cmd_ptr
)
{
  uint32    bearer_instance;
  ds_bearer_context_s        *bearer_cntxt_p = NULL;
  ds_3gpp_pdn_call_info_type *call_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DS_3GPP_MSG0_ERROR("NULL ptr passed, return");
    return;
  }

  call_info_ptr = (ds_3gpp_pdn_call_info_type*)cmd_ptr->cmd_payload_ptr;
  bearer_instance = call_info_ptr->call_instance;

  if (cmd_ptr->hdr.cmd_id == DS_CMD_3GPP_PHYS_LINK_FLOW_ENABLE) 
  {
    DS_3GPP_MSG1_HIGH("PHYS LINK ENABLE FLOW on bearer context %d",bearer_instance);

    bearer_cntxt_p = ds_bearer_cntxt_get_instance_ptr((uint8)bearer_instance);
    if (ds_bearer_cntx_validate_bearer_context(bearer_cntxt_p))
    {

      ps_phys_link_enable_flow(&(bearer_cntxt_p->phys_link), 
                              (uint32)call_info_ptr->info_ptr);
    }
    else 
    {
      DS_3GPP_MSG1_ERROR("Bearer context invalid! bearer_instance:%d",bearer_instance);
    }
  }
  else 
  {
    DATA_MSG1_ERROR("Incorrect cmd_id = %d", cmd_ptr->hdr.cmd_id);
    return;
  }
} /*ds_bearer_cntxt_phys_link_flow_cmd_hdlr*/

#ifdef FEATURE_UE_INITIATED_DORMANCY 
/*===========================================================================
 DS_3GPP_BEARER_CNTXT_HANDLE_GO_DORMANT_REQUEST
  
 DESCRIPTION
  This function handles go_dormant request from APPS.
  Checks if any data is pending in WM or any other beareris UP 
  other than the one on which go_dormant is requested
  
 PARAMETERS
  bearer_cntx_p: Bearer on which GO_DORMANT is requested
  bearer_cntx_inst: Bearer Instance
  ps_errno: Error cause to be returned
  
 DEPENDENCIES
  None.
  
 RETURN VALUE
  0- Dormancy request sent to NW
 -1 - if request is not honoured
  
 SIDE EFFECTS
  None.
  
 ===========================================================================*/
LOCAL int ds_3gpp_bearer_cntxt_handle_go_dormant_request
(
  ds_bearer_context_s *bearer_cntx_p,
  uint32               bearer_cntx_inst,
  ps_iface_fast_dormancy_status_type*  fd_status_p,
  int16                *ps_errno
)
{  
  boolean             index;
  ds_bearer_context_s *other_bearer_cntx_p;
  dsm_watermark_type  *tx_wm = NULL;

  /*--------------------------------------------------------------------------  */
  if(ds_bearer_cntx_validate_bearer_context(bearer_cntx_p))
  {
    if (bearer_cntx_p->ds_bearer_context_dyn_p->state != DS_BEARER_CONTEXT_STATE_UP)
    {
      DS_3GPP_MSG1_ERROR("GO DORMANT IOCTL fail. bearer_state: %d "
                            ,bearer_cntx_p->ds_bearer_context_dyn_p->state);
     fd_status_p->dorm_status = PS_FAST_DORMANCY_STATUS_FAILURE;
     fd_status_p->failure_reason = PS_FAST_DORMANCY_STATUS_FAIL_INVALID_PDN_STATE;
     *ps_errno =  DS_EINVAL;
      return -1;
    }
  }
  /* Check if any other PDPs are active i.e. not dormant */
  for (index = 0; index < DS_3GPP_MAX_BEARER_CONTEXT; index++)
  {
    other_bearer_cntx_p = ds_bearer_cntxt_get_instance_ptr(index);
    if (!ds_bearer_cntx_validate_bearer_context_internal(other_bearer_cntx_p))
    {
      continue;
    }
    if (other_bearer_cntx_p->ds_bearer_context_dyn_p->state == 
                                         DS_BEARER_CONTEXT_STATE_UP) 
    {
      /*-------------------------------------------------------------------
         If there is still data in watermark, then do not go dormant. 
          Fail IOCTL
          -------------------------------------------------------------------*/
      tx_wm = &(other_bearer_cntx_p->tx_wm.wm_item);
      if(tx_wm->current_cnt != 0)
      {
        DS_3GPP_MSG1_ERROR("GO DORMANT IOCTL fail. Data present in WM for "
                            "bearer instance [%d].",index);
        fd_status_p->dorm_status = PS_FAST_DORMANCY_STATUS_FAILURE;
        fd_status_p->failure_reason = PS_FAST_DORMANCY_STATUS_FAIL_DATA_ACTIVITY;
        *ps_errno =  DS_EINVAL;
         return -1;
      }
    }
    if(((other_bearer_cntx_p->ds_bearer_context_dyn_p->state == 
                                   DS_BEARER_CONTEXT_STATE_UP) ||
       (other_bearer_cntx_p->ds_bearer_context_dyn_p->state == 
                     DS_BEARER_CONTEXT_STATE_UP_DORMANT_REESTAB))
       &&
      (index != bearer_cntx_inst) )
    {
      DS_3GPP_MSG1_HIGH("GO DORMANT with other active bearer inst [%d]",
                              index);
      DS_3GPP_MSG1_ERROR("GO DORMANT disallowed, ps_errno [%d]",
                              DS_EINVAL);
      fd_status_p->dorm_status = PS_FAST_DORMANCY_STATUS_FAILURE;
      fd_status_p->failure_reason = PS_FAST_DORMANCY_STATUS_FAIL_OTHER_PDN_UP;
      *ps_errno =  DS_EINVAL;
      return -1; 
    }
  } /* for */
      
  /* send CM command */
  if (!ds3g_msh_go_dormant())
  {
    *ps_errno =  DS_ETRYAGAIN;
    return -1; 
  }  
  return 0;
}

/*===========================================================================
 DS_3GPP_BEARER_CNTXT_DORMANCY_DELAY_TMR_EXPIRY_CB
  
 DESCRIPTION
  This is the callback function for dormancy delay timer expiry
  checks if there is any data activity within this time period and
  takes necessary action
  
 PARAMETERS
callback_data: phys_link_ptr on which do_dormant is requested
  
 DEPENDENCIES
  None.
  
 RETURN VALUE
 None
  
 SIDE EFFECTS
  None.
  
 ===========================================================================*/
void ds_3gpp_bearer_cntxt_dormancy_dealy_tmr_expiry_cb
(
  unsigned long callback_data
)
{
  ps_phys_link_type   *phys_link_ptr;
  uint32                bearer_cntx_inst;
  ds_bearer_context_s *bearer_cntx_p;
  int16                ps_errno;
  ds_3gpp_delay_dormancy_s* info_p = NULL;
  ps_iface_fast_dormancy_status_type  fd_status;
  boolean                   gen_fd_event = FALSE;
  sys_modem_as_id_e_type       subs_id = SYS_MODEM_AS_ID_NONE;
  /*---------------------------------------------------------------------*/

  phys_link_ptr = (ps_phys_link_type*)callback_data;
    DS_3GPP_MSG1_HIGH("Dormancy Delay Timer Expiry cb, cb_data:0x%x",
                                                        phys_link_ptr);

  ASSERT ( phys_link_ptr != NULL );
  bearer_cntx_inst = (uint32)phys_link_ptr->client_data_ptr;
  bearer_cntx_p = ds_bearer_cntxt_get_instance_ptr(bearer_cntx_inst);
	
  if ((!ds_3gpp_pdn_cntxt_validate_and_get_delay_dormancy_info_ptr(&info_p)) 
      || (!ds_bearer_cntx_validate_bearer_context(bearer_cntx_p)))
  {
    ds_3gpp_pdn_cntxt_free_memory_for_delay_dormancy_stats();
    return;
  }
   /* freeing timer */
  rex_delete_timer_ex(info_p->timer_p);
   
  ds_bearer_cntxt_get_subs_id_from_bearer(bearer_cntx_p, &subs_id);
  
  if(PS_PHYS_LINK_GET_STATE(phys_link_ptr) == PHYS_LINK_DOWN)
  {
    if(bearer_cntx_p->ds_bearer_context_dyn_p->state != 
                               DS_BEARER_CONTEXT_STATE_UP)
  {
    DS_3GPP_MSG0_ERROR("phys_link on which GO_DORMANT Requested already DOWN");
  }
  else
  {
      fd_status.dorm_status = PS_FAST_DORMANCY_STATUS_SUCCESS;
      fd_status.failure_reason = PS_FAST_DORMANCY_STATUS_FAIL_NONE;
      gen_fd_event =  TRUE;
    }
  }
  else
  {
    /* check if data activity is present */
    if (TRUE == ds_3gpp_bearer_cntxt_init_or_check_data_statistics(
                                       DS_3GPP_CHECK_DATA_ACTIVITY))
    {
       DS_3GPP_MSG0_HIGH("Data Activity is present, not honouring"
                                  "GO_DORMANT Request after Timer Expiry");
  
      fd_status.dorm_status = PS_FAST_DORMANCY_STATUS_FAILURE;
      fd_status.failure_reason = PS_FAST_DORMANCY_STATUS_FAIL_DATA_ACTIVITY;
      gen_fd_event =  TRUE;
    }
    else if(0 != ds_3gpp_bearer_cntxt_handle_go_dormant_request(bearer_cntx_p,
                                                           bearer_cntx_inst,
                                                           &fd_status,
                                                           &ps_errno))
    {
      DS_3GPP_MSG0_HIGH("Dormancy Request not honored");
      gen_fd_event =  TRUE;       
    }
  }

  if (gen_fd_event == TRUE)
  {
    DS_3GPP_MSG2_HIGH("generating dormancy status event, status:%d, cause:%d", 
                                    fd_status.dorm_status, fd_status.failure_reason);
    ds_3gpp_bearer_gen_fast_dormancy_event(bearer_cntx_p, subs_id, (void*)&fd_status );
  }

  ds_3gpp_pdn_cntxt_free_memory_for_delay_dormancy_stats();
  return;
}

/*===========================================================================
FUNCTION ds_3gpp_bearer_gen_fast_dormancy_event()

DESCRIPTION
  This function is called to send fast dormancy status event to PS/QMI.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_bearer_gen_fast_dormancy_event
(
  ds_bearer_context_s               *bearer_context_ptr,
  sys_modem_as_id_e_type             subs_id,
  void                              *event_data_ptr
)
{
  ps_iface_type                     *iface_ptr = NULL;
  uint8                              iface_inst;
  ds_pdn_context_s                  *pdn_context_ptr;
  ds_3gpp_iface_s                   *ds_iface_ptr;
  /* ------------------------------------------------------*/  
  if(!ds_bearer_cntxt_validate_bearer_subscription
      (bearer_context_ptr, subs_id))
  {
    return;
  }

  pdn_context_ptr = (ds_pdn_context_s *) (bearer_context_ptr-> \
                                   ds_bearer_context_dyn_p->pdn_context_ptr);
  if (!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_ptr))
  {
    return;
  }
  if((bearer_context_ptr->ds_bearer_context_dyn_p->state 
          == DS_BEARER_CONTEXT_STATE_UP
       || bearer_context_ptr->ds_bearer_context_dyn_p->state 
          == DS_BEARER_CONTEXT_STATE_UP_DORMANT
       || bearer_context_ptr->ds_bearer_context_dyn_p->state
          == DS_BEARER_CONTEXT_STATE_UP_DORMANT_REESTAB) &&
      (bearer_context_ptr->ds_bearer_context_dyn_p->call_mode 
       == SYS_SYS_MODE_WCDMA) &&
       (bearer_context_ptr->ds_bearer_context_dyn_p->is_default))
  {
  /*-------------------------------------------------------------------------
    Generate technology changed event to PS, and retrieve the iface specific
    to a given call instance.
  -------------------------------------------------------------------------*/
    for (iface_inst = 0; 
                iface_inst < DS_3GPP_MAX_IFACE_PER_PDN_CONTEXT; 
                        iface_inst ++)
    {
      ds_iface_ptr = pdn_context_ptr->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[iface_inst];                            
  
      iface_ptr = (ds_iface_ptr != NULL) ? &(ds_iface_ptr->ps_iface) : NULL;
  
      if (PS_IFACE_IS_IN_USE(iface_ptr))                
      {
        DS_3GPP_MSG1_HIGH("ds_3gpp_ps_data_generic_ind_hdlr: Posting iface generic"
                          "ind for iface %d", iface_inst);
        ps_iface_generic_ind (iface_ptr,
                              IFACE_FAST_DORMANCY_STATUS_EV,
                              event_data_ptr );
      }
    } /* for each iface*/

  } /* if baarer UP*/
  return;
}/* ds_3gpp_bearer_gen_fast_dormancy_event */

#endif /*FEATURE_UE_INITIATED_DORMANCY */

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_PHYS_LINK_IOCTL_HDLR

DESCRIPTION
  Callback called from application context for secondary PDP context.
  Central function for embedded applications to bring down or
  modify the secondary context.

PARAMETERS
  *phys_link_ptr    - Ptr to the phys link.
  ioctl_name        - Type of request
  argval_ptr        - QoS specified by the application
  *ps_errno         - Ptr to error code (if any)

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  Calls into UMTSPS to bring up or down the secondary phys link handler.

===========================================================================*/
LOCAL int ds_bearer_cntxt_phys_link_ioctl_hdlr
(
  ps_phys_link_type        *phys_link_ptr_i,
  ps_phys_link_ioctl_type  ioctl_name,
  void                     *argval_ptr,
  int16                    *ps_errno
)
{
  ps_phys_link_type *phys_link_ptr = phys_link_ptr_i;
  uint32 bearer_cntx_inst;
  ds_bearer_context_s *bearer_cntx_p;
#ifdef FEATURE_UE_INITIATED_DORMANCY 
  uint32           delay_timer_value = 0;
  ds_3gpp_delay_dormancy_s* info_p = NULL;
#endif /* FEATURE_UE_INITIATED_DORMANCY */
  int ret_val = 0;
  int16  ps_errno_local=0;
  sys_modem_as_id_e_type subs_id = SYS_MODEM_AS_ID_NONE;
#ifdef FEATURE_QXDM_WM_TRACING
  uint32 *call_id;
#endif /* FEATURE_QXDM_WM_TRACING */
  ds_pdn_context_s *pdn_cntxt_p = NULL;
  ps_iface_fast_dormancy_status_type fd_status;

  /*-------------------------------------------------------------------------*/

  ASSERT ((phys_link_ptr != NULL)&& (ps_errno!= NULL));

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section); 
  ds3gpp_enter_global_crit_section();
  
  bearer_cntx_inst = (uint32)phys_link_ptr->client_data_ptr;
  bearer_cntx_p = ds_bearer_cntxt_get_instance_ptr(bearer_cntx_inst);
  if(!ds_bearer_cntx_validate_bearer_context(bearer_cntx_p))
  {
    *ps_errno = DS_EINVAL;
	
    ds3gpp_leave_global_crit_section();	
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section); 
    return -1;
  }

  pdn_cntxt_p = (ds_pdn_context_s*)
                (bearer_cntx_p->ds_bearer_context_dyn_p->pdn_context_ptr);

  if (! ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntxt_p,&subs_id))
  {
    *ps_errno = DS_EINVAL;

    ds3gpp_leave_global_crit_section();
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section); 
    return -1;
  }
  /*-------------------------------------------------------------------------
    Switch on type of request
  -------------------------------------------------------------------------*/
  switch (ioctl_name)
  {
    case PS_PHYS_LINK_IOCTL_GO_ACTIVE:      

      DS_3GPP_MSG1_HIGH("Rcvd IOCTL_GO_ACTIVE for bearer cntx instance %d ",
               bearer_cntx_inst);

       /*-------------------------------------------------------------------
        If the context was dormant, bring up the physical link
      -------------------------------------------------------------------*/
      if (bearer_cntx_p->ds_bearer_context_dyn_p->state == DS_BEARER_CONTEXT_STATE_UP ||
          bearer_cntx_p->ds_bearer_context_dyn_p->state == DS_BEARER_CONTEXT_STATE_UP_DORMANT ||
          bearer_cntx_p->ds_bearer_context_dyn_p->state == DS_BEARER_CONTEXT_STATE_UP_DORMANT_REESTAB)      
      {
        DS_3GPP_MSG1_HIGH("Processing GO_ACTIVE for bearer instance %d ",
                 bearer_cntx_inst);

        if(PS_PHYS_LINK_GET_STATE(phys_link_ptr) == PHYS_LINK_DOWN)
        {
          /*---------------------------------------------------------------
          Bring up the physical link.
          ---------------------------------------------------------------*/
          ret_val = ps_phys_link_up_cmd(phys_link_ptr,
                                    ps_errno,
                                    NULL);

          if (ret_val == 0)
          {
            /*-----------------------------------------------------------------
              An error occured. We just checked that phys link was down. We 
              should not have got 0 in ret_val.
            -----------------------------------------------------------------*/
            DS_3GPP_MSG1_ERROR("Bringing up phys link that is already up %d",
                               (uint32)phys_link_ptr->client_data_ptr);
            ret_val = -1;
          }          
          else if (*ps_errno != DS_EWOULDBLOCK)
          {
            /*-----------------------------------------------------------------
              An error occured. End the call.
            -----------------------------------------------------------------*/
            ret_val = ps_phys_link_go_null_cmd(phys_link_ptr,
                                  &ps_errno_local,
                                  NULL);
            if (ps_errno_local != DS_EWOULDBLOCK) 
            {
              //VRK:TODO Need to check if we shd use CLOSENETINPROGRESS
              DS_3GPP_MSG2_ERROR("Was expecting DS_EWOULDBLOCK (%d), got something else (%d)",
                    DS_EWOULDBLOCK,ps_errno);
              ASSERT(0);  
            }
            ret_val=-1;
          }
          else{
            ret_val=0;
          }
        }
        else
        {
          DS_3GPP_MSG1_HIGH("Phys link already up, send UpInd bearer ist:%d",
                    bearer_cntx_inst);
          ps_phys_link_up_ind(phys_link_ptr);

          /*-------------------------------------------------------------------
            Send PS phys link disable for the bearer if subs is coex flow
            disabled
          -------------------------------------------------------------------*/
          if( !ds3gflowmgr_get_coex_data_flow_status(subs_id) )
          {
            ps_phys_link_disable_flow(phys_link_ptr, DS_FLOW_COEX_MASK);
          }
        }
      }
      else
      {
        DS_3GPP_MSG1_ERROR("GO ACTIVE called in wrong state %d", 
                       bearer_cntx_p->ds_bearer_context_dyn_p->state);
        *ps_errno = DS_EINVAL;
        ret_val = -1;
      }
      break;

#ifdef FEATURE_UE_INITIATED_DORMANCY 
  case PS_PHYS_LINK_IOCTL_GO_DORMANT:
      DS_3GPP_MSG0_HIGH("Rcvd PS_PHYS_LINK_IOCTL_GO_DORMANT ");      

      if((PS_PHYS_LINK_GET_STATE(phys_link_ptr) == PHYS_LINK_DOWN) &&
         (bearer_cntx_p->ds_bearer_context_dyn_p->state != 
                                 DS_BEARER_CONTEXT_STATE_UP))
      {
        DS_3GPP_MSG0_ERROR("GO DORMANT when phys_link already DOWN, ");
        ret_val = 0;
        break;
      }

      /* GO_Dormant request with or without Delay timer value configured will 
              not be honored if Delay timer is already running. we can figure out this
               by checking if dynamic delay dormancy pointer is NULL or not */
      if (ds_3gpp_pdn_cntxt_validate_and_get_delay_dormancy_info_ptr(&info_p))
		{
        DS_3GPP_MSG0_HIGH("Delay Dormancy Tmr is Running, Not honoring" 
                                               " New GO_DORMANT Request ");
        *ps_errno =  DS_EINVAL;
         ret_val = -1;
         break;
		}

      if(argval_ptr != NULL)
      {
        delay_timer_value = (*(uint32*)argval_ptr);
      }
      /* SOMC check if Dormancy Delay timer is configured */
      if (delay_timer_value > 0)
      {
        DS_3GPP_MSG1_HIGH("Delay Timer Included in GO_DORMANT Value: %dms",
                                                        delay_timer_value);
        /*SOMC  As delay timer is configured, start timer for that duration
                  and delay connection release */
        /*Init dynamic memory for data statistics */
        if ( NULL == ds_3gpp_pdn_cntxt_alloc_memory_for_delay_dormancy_stats())
        {
          DS_3GPP_MSG0_HIGH("Dynamic memory initialization failed ");
          *ps_errno =  DS_NOMEMORY;
            ret_val = -1;
            break;
        }
		
        if((!ds_3gpp_pdn_cntxt_validate_and_get_delay_dormancy_info_ptr(&info_p))
            || ((info_p->timer_p =
         rex_create_timer_ex(ds_3gpp_bearer_cntxt_dormancy_dealy_tmr_expiry_cb,
                            (unsigned long)phys_link_ptr))== NULL))
        {
          DS_3GPP_MSG0_HIGH("Delay Dormancy Timer allocation failed ");
          *ps_errno =  DS_EINVAL;
          ret_val = -1; 
          break;
        }
      
        /* Initilaize/cache current data stats before starting delay timer */
        ds_3gpp_bearer_cntxt_init_or_check_data_statistics(
                                                  DS_3GPP_INIT_DATA_STATS);
        rex_set_timer(info_p->timer_p,(rex_timer_cnt_type)delay_timer_value);
      }
      else
      {
        ret_val = ds_3gpp_bearer_cntxt_handle_go_dormant_request(bearer_cntx_p,
                                                              bearer_cntx_inst,
                                                              &fd_status,
                                                              ps_errno);
      }         
      break;

    case PS_PHYS_LINK_IOCTL_ABORT_GO_DORMANT:
      DS_3GPP_MSG0_HIGH("Rcvd PS_PHYS_LINK_IOCTL_ABORT_GO_DORMANT ");
	  
      if((ds_3gpp_pdn_cntxt_validate_and_get_delay_dormancy_info_ptr(&info_p)) 
          && (0 != rex_get_timer(info_p->timer_p)))
      {
        DS_3GPP_MSG0_HIGH("Dormancy Delay Timer running, Abort it ");
        /* Free the timer, it internally clears the timer and frees the memory  */
        rex_delete_timer_ex(info_p->timer_p);
        ds_3gpp_pdn_cntxt_free_memory_for_delay_dormancy_stats();
      }         
      else
      {
         DS_3GPP_MSG0_HIGH("Dormancy Delay Timer not running/Invalid," 
                                          "Ignoring ABORT Request");
        *ps_errno =  DS_EINVAL;
        ret_val = -1;
      }
      break;
#endif /* FEATURE_UE_INITIATED_DORMANCY */

#ifdef FEATURE_QXDM_WM_TRACING
    case PS_PHYS_LINK_IOCTL_GET_CALL_ID:
      DATA_MSG0_HIGH("Call ID query");
      call_id = (uint32*)argval_ptr;
      *call_id = ds_bearer_cntxt_get_call_id(bearer_cntx_p);
      break;
#endif /* FEATURE_QXDM_WM_TRACING */


    default:
      DS_3GPP_MSG1_ERROR("Invalid request from IOCTL: %d",ioctl_name);
      *ps_errno =  DS_EINVAL;
      ret_val = -1;
      break;
  }
  ds3gpp_leave_global_crit_section();
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section); 

  return ret_val;  
} /* ds_bearer_cntxt_phys_link_ioctl_hdlr() */

/*===========================================================================
FUNCTION DSUMTSPS_INITIALIZE_L2_WMS

DESCRIPTION
   This function initializes the SNDCP/PDCP UL and DL watermarks for a call
   instance.
  
PARAMETERS
  bearer_context_p - Pointer to the bearer instance to work on.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL void ds_bearer_cntxt_initialize_l2_wms
(
  ds_bearer_context_s * bearer_context_p
)
{
  int dne_value = (int) 0xFFFFFFFF;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(bearer_context_p == NULL) 
  {
    DS_3GPP_MSG0_ERROR("Bearer Context passed is invalid");
    return;
  }

  dsm_queue_init(&(bearer_context_p->tx_wm.wm_item),
                 dne_value,  /* No MAX queue size */
                 &(bearer_context_p->tx_wm.wm_queue));

  dsm_queue_init ( &(bearer_context_p->rx_wm.wm_item),
                   dne_value,  /* No MAX queue size */
                   &(bearer_context_p->rx_wm.wm_queue));
  dsm_queue_init ( &(bearer_context_p->lte_rx_wm.wm_item),
                   dne_value,  /* No MAX queue size */
                   &(bearer_context_p->lte_rx_wm.wm_queue));
  dsm_queue_init ( &(bearer_context_p->tlb_rx_wm.wm_item),
                   dne_value,  /* No MAX queue size */
                   &(bearer_context_p->tlb_rx_wm.wm_queue));

}/* ds_bearer_cntxt_initialize_l2_wms() */

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_PROC_PCH_CMD

DESCRIPTION
  This function handles PCH indication from RRC.

PARAMETERS   
  *user_data - PCH indication
  sys_mode -   System mode in which PCH command is posted

DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
  
SIDE EFFECTS 
  None.
===========================================================================*/
static void ds_bearer_cntxt_proc_pch_cmd
(
  ds_3gpp_pch_event_e_type  pch_event,
  sys_sys_mode_e_type       sys_mode,
  sys_modem_as_id_e_type    subs_id
)
{
  int index = 0;
  ds_bearer_context_s     *bearer_context_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_3GPP_MSG3_HIGH("PCH indication: %d RRC mode %d ",pch_event, sys_mode,0);

  for (index = 0; index < DS_3GPP_MAX_BEARER_CONTEXT; index++)
  {
    bearer_context_p = ds_bearer_cntxt_get_instance_ptr(index);

    if(!ds_bearer_cntx_validate_bearer_subscription_internal (bearer_context_p, subs_id))
    {
  
      DS_3GPP_MSG3_LOW("PCH indication: RRC mode %d bearer inst [%d] state: %d ",
                         sys_mode, index, 
                         DS_BEARER_CONTEXT_STATE_DOWN);
    }
    else
    {
      DS_3GPP_MSG3_HIGH("PCH indication: RRC mode %d bearer inst [%d] state: %d ",
         sys_mode, index, (bearer_context_p->ds_bearer_context_dyn_p)->state );

      if(((bearer_context_p->ds_bearer_context_dyn_p)->state 
                                       == DS_BEARER_CONTEXT_STATE_UP) &&
         ((bearer_context_p->ds_bearer_context_dyn_p)->call_mode
                                       == sys_mode))
      {     
        if (pch_event == DS_3GPP_EVENT_IN_PCH)
        {          
          ps_phys_link_down_ind_ex( &(bearer_context_p->phys_link),
                                     PS_EIC_QOS_NETWORK_L2_LINK_RELEASED );
          /*-------------------------------------------------------------------
            Register each enqueue function ptr.      
          -------------------------------------------------------------------*/
          (bearer_context_p->tx_wm.wm_item).each_enqueue_func_ptr =
  		              (wm_cb_type)ds_bearer_cntxt_reorig_dorm_call_cb;
          (bearer_context_p->tx_wm.wm_item).each_enqueue_func_data =
  		                      (void*) bearer_context_p;   

          DS_3GPP_MSG0_MED("ds_bearer_cntxt_proc_pch_cmd: posted phys_link_down_ind");
        }
        else
        {
          ps_phys_link_up_ind_ex( &(bearer_context_p->phys_link),
                                   PS_EIC_QOS_NETWORK_L2_LINK_REESTAB_IND );
          /*-------------------------------------------------------------------
            De-register each enqueue function ptr.      
          -------------------------------------------------------------------*/
          (bearer_context_p->tx_wm.wm_item).each_enqueue_func_ptr = NULL;
          (bearer_context_p->tx_wm.wm_item).each_enqueue_func_data = NULL;
           DS_3GPP_MSG0_MED("ds_bearer_cntxt_proc_pch_cmd: posted phys_link_up_ind");

          /*-------------------------------------------------------------------
            Send PS phys link disable for the bearer if subs is coex flow
            disabled
          -------------------------------------------------------------------*/
          if( !ds3gflowmgr_get_coex_data_flow_status(subs_id) )
          {
            ps_phys_link_disable_flow(&(bearer_context_p->phys_link),
                                      DS_FLOW_COEX_MASK);
        }
      }
      }
    }//else
  }//for
}  /* ds_bearer_cntxt_proc_pch_cmd */
 /*===========================================================================
FUNCTION DS_BEARER_CNTXT_TD_PROC_CMD

DESCRIPTION
  This function handles the request to update ul wm levels from
  TDRRC 

PARAMETERS
  rab_id  - Identifier used to uniquely identify a data bearer
  ul_data_rate - Updated data rate of the bearer
  subs_id      - Subscription on which this call is made

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void ds_bearer_cntxt_td_proc_cmd
(
  uint32 rab_id,
  uint32 ul_data_rate,
  sys_modem_as_id_e_type subs_id
)
{
  int index = 0;
  ds_pdn_context_s*       pdn_cntx_p = NULL;
  ds_bearer_cntxt_tds_um_wm_threshold_struct_type  tds_um_wm_threshold;
  ds_bearer_context_s    *bearer_context_p = NULL;
  /*-----------------------------------------------------------------------*/

  DS_3GPP_MSG2_HIGH("TD UL DATA RATE Indication rab_id %d data rate %d",
                    rab_id, ul_data_rate);


  for (index = 0; index < DS_3GPP_MAX_BEARER_CONTEXT; index++)
  {
    bearer_context_p = ds_bearer_cntxt_get_instance_ptr(index);

    if(!ds_bearer_cntx_validate_bearer_subscription_internal 
         (bearer_context_p, subs_id))
    {

      DS_3GPP_MSG3_LOW("TD UL DATA RATE Indication Invalid BEARER: "
                       "rab_id %d data rate %d Subs Id(CM) %d",
                       rab_id, ul_data_rate,subs_id);
    }
    else
    {
      /* Get the PDN context */
      pdn_cntx_p = (ds_pdn_context_s *)
                     bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr;
      tds_um_wm_threshold =  ds_3gpp_nv_get_tds_wm_threshold(subs_id);  
      if (bearer_context_p->ds_bearer_context_dyn_p->nsapi == rab_id)
      {
        if (ul_data_rate <= DSTPSD_UL_RATE_16)
        {
          dsm_set_low_wm(&bearer_context_p->tx_wm.wm_item,
                         DSTPSD_UL_WM_LO_16);
          if(bearer_context_p->tx_wm.wm_item.hiwater_func_ptr == NULL)
          {
            dsm_set_hi_wm(&bearer_context_p->tx_wm.wm_item,
                          DSTPSD_UL_WM_DNE_16);
          }
          else
          {
            dsm_set_hi_wm(&bearer_context_p->tx_wm.wm_item,
                          DSTPSD_UL_WM_HI_16);
          }
          dsm_set_dne(&bearer_context_p->tx_wm.wm_item,
                      DSTPSD_UL_WM_DNE_16);

        }
        else if ((ul_data_rate <= DSTPSD_UL_RATE_32)&&
                 (ul_data_rate > DSTPSD_UL_RATE_16 ))
        {
          dsm_set_low_wm(&bearer_context_p->tx_wm.wm_item,
                         DSTPSD_UL_WM_LO_32);
          if(bearer_context_p->tx_wm.wm_item.hiwater_func_ptr == NULL)
          {
            dsm_set_hi_wm(&bearer_context_p->tx_wm.wm_item,
                          DSTPSD_UL_WM_DNE_32);
          }
          else
          {
            dsm_set_hi_wm(&bearer_context_p->tx_wm.wm_item,
                          DSTPSD_UL_WM_HI_32);
          }
          dsm_set_dne(&bearer_context_p->tx_wm.wm_item,
                      DSTPSD_UL_WM_DNE_32);
        }
        else if ((ul_data_rate <= DSTPSD_UL_RATE_64 )&&
                 (ul_data_rate > DSTPSD_UL_RATE_32 ))
        {
          dsm_set_low_wm(&bearer_context_p->tx_wm.wm_item,
                         DSTPSD_UL_WM_LO_64);
          if(bearer_context_p->tx_wm.wm_item.hiwater_func_ptr == NULL)
          {
            dsm_set_hi_wm(&bearer_context_p->tx_wm.wm_item,
                          DSTPSD_UL_WM_DNE_64);
          }
          else
          {
            dsm_set_hi_wm(&bearer_context_p->tx_wm.wm_item,
                          DSTPSD_UL_WM_HI_64);
          }
          dsm_set_dne(&bearer_context_p->tx_wm.wm_item,
                      DSTPSD_UL_WM_DNE_64);

        }

        else if ((ul_data_rate <= DSTPSD_UL_RATE_128 )&&
                 (ul_data_rate > DSTPSD_UL_RATE_64 ))
        {
          dsm_set_low_wm(&bearer_context_p->tx_wm.wm_item,
                         DSTPSD_UL_WM_LO_128);
          if(bearer_context_p->tx_wm.wm_item.hiwater_func_ptr == NULL)
          {
            dsm_set_hi_wm(&bearer_context_p->tx_wm.wm_item,
                          DSTPSD_UL_WM_DNE_128);
          }
          else
          {
            dsm_set_hi_wm(&bearer_context_p->tx_wm.wm_item,
                          DSTPSD_UL_WM_HI_128);
          }
          dsm_set_dne(&bearer_context_p->tx_wm.wm_item,
                      DSTPSD_UL_WM_DNE_128);

        }
        else if ((ul_data_rate <= DSTPSD_UL_RATE_256)&&
                 (ul_data_rate > DSTPSD_UL_RATE_128))
        {
          dsm_set_low_wm(&bearer_context_p->tx_wm.wm_item,
                         DSTPSD_UL_WM_LO_256);
          if(bearer_context_p->tx_wm.wm_item.hiwater_func_ptr == NULL)
          {
            dsm_set_hi_wm(&bearer_context_p->tx_wm.wm_item,
                          DSTPSD_UL_WM_DNE_256);
          }
          else
          {
            dsm_set_hi_wm(&bearer_context_p->tx_wm.wm_item,
                          DSTPSD_UL_WM_HI_256);
          }
          dsm_set_dne(&bearer_context_p->tx_wm.wm_item,
                      DSTPSD_UL_WM_DNE_256);
        }
        else if ((ul_data_rate <= DSTPSD_UL_RATE_384 )&&
                 (ul_data_rate > DSTPSD_UL_RATE_256))
        {
          dsm_set_low_wm(&bearer_context_p->tx_wm.wm_item,
                         DSTPSD_UL_WM_LO_384);
          if(bearer_context_p->tx_wm.wm_item.hiwater_func_ptr == NULL)
          {
            dsm_set_hi_wm(&bearer_context_p->tx_wm.wm_item,
                          DSTPSD_UL_WM_DNE_384);
          }
          else
          {
            dsm_set_hi_wm(&bearer_context_p->tx_wm.wm_item,
                          DSTPSD_UL_WM_HI_384);
          }
          dsm_set_dne(&bearer_context_p->tx_wm.wm_item,
                      DSTPSD_UL_WM_DNE_384);

        }        
        else if (ul_data_rate > DSTPSD_UL_RATE_384)
        {
          dsm_set_low_wm(&bearer_context_p->tx_wm.wm_item,
                         tds_um_wm_threshold.tds_ul_lo);
          if(bearer_context_p->tx_wm.wm_item.hiwater_func_ptr == NULL)
          {
            dsm_set_hi_wm(&bearer_context_p->tx_wm.wm_item,
                          tds_um_wm_threshold.tds_ul_dne);
          }
          else
          {
            dsm_set_hi_wm(&bearer_context_p->tx_wm.wm_item,
                          tds_um_wm_threshold.tds_ul_hi);
          }
          dsm_set_dne(&bearer_context_p->tx_wm.wm_item,
                      tds_um_wm_threshold.tds_ul_dne);

        }

        DS_3GPP_MSG3_HIGH("WM levels after update - Lo: %d, Hi: %d, DNE: %d",
                          bearer_context_p->tx_wm.wm_item.lo_watermark,
                          bearer_context_p->tx_wm.wm_item.hi_watermark,
                          bearer_context_p->tx_wm.wm_item.dont_exceed_cnt);


      }
      else
      {
         DS_3GPP_MSG3_HIGH("TD UL DATA RATE Indication: rab_id not found %d",
                         rab_id,0,0);
      }

    }
  }
   
}
/*===========================================================================
FUNCTION      DS_BEARER_CNTXT_PHYS_LINK_UP

DESCRIPTION
  Brings up the UMTS traffic channel for primary and secondary contexts.

PARAMETERS
  phys_link_ptr - phys link ptr for the call
  info_ptr      - not used

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL int ds_bearer_cntxt_phys_link_up
(
  ps_phys_link_type *phys_link_ptr_i,
  void              *info_ptr
)
{
  const ps_phys_link_type   *phys_link_ptr = phys_link_ptr_i;
  ds_cmd_type                *cmd_ptr;
  uint32                    bearer_instance;
  ds_3gpp_pdn_call_info_type *call_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  bearer_instance =  (uint32)(phys_link_ptr->client_data_ptr);

  DS_3GPP_MSG1_HIGH("DSUMTSPS PHYS LINK up cmd Bearer Inst:%d",bearer_instance);

  /*-------------------------------------------------------------------------
    The flags set in the call table will
    give enough information to differentiate between a secondary and a
    primary
  -------------------------------------------------------------------------*/
  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_3gpp_pdn_call_info_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return (-1);
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_PHYS_LINK_UP;
  call_info_ptr = (ds_3gpp_pdn_call_info_type*)cmd_ptr->cmd_payload_ptr;

  call_info_ptr->call_instance = bearer_instance;
  call_info_ptr->info_ptr = info_ptr;

    ds_put_cmd_ext(cmd_ptr);

  return(0);

} /* ds_bearer_cntxt_phys_link_up */

/*===========================================================================
FUNCTION  ds_3gpp_bearer_cntxt_pending_redial_timer_expiry_cb

DESCRIPTION
  This function will be a call back for pending redial Timer expiry and posts 
  a command to DS Task

PARAMETERS
  cb_data: Subscription Id passed as Callback Data

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_bearer_cntxt_pending_redial_timer_expiry_cb
(
  unsigned long cb_data
)
{  
  sys_modem_as_id_e_type subs_id =  (sys_modem_as_id_e_type)cb_data;
  /*----------------------------------------------------------------------*/

  DS_3GPP_MSG0_HIGH("Pending Redial Timer Expiry callback," 
  	                       "Posting DS_3GPP_CLR_PENDING_CALL_CMD");
  
  ds_bearer_cntxt_post_clr_pending_calls_cmd(subs_id); 
}/* ds_3gpp_bearer_cntxt_pending_redial_timer_expiry_cb */

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_PER_SUBS_INFO_INIT

DESCRIPTION
  This function initializes the Bearer Context Information for the given
  subscription.
 
PARAMETERS
  subs_id : Subscriptio Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_bearer_cntxt_per_subs_info_init
(
  sys_modem_as_id_e_type subs_id
)
{
  ds_bearer_cntxt_per_subs_info_type* per_subs_info_p = NULL;
  /*-----------------------------------------------------------------------*/

  per_subs_info_p = ds_bearer_cntxt_get_per_subs_info_hndl(subs_id);

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
        (ds_bearer_cntxt_per_subs_info_type*)
          modem_mem_alloc (sizeof(ds_bearer_cntxt_per_subs_info_type),
                           MODEM_MEM_CLIENT_DATA);
      if (per_subs_info_p == NULL)
      {
        DS_3GPP_MSG1_ERROR ("Memory allocation failed for Subs Id(CM): %d", 
                            subs_id );
        break;
      }

      per_subs_info_p->cookie = DS_BEARER_CNTXT_PER_SUBS_INFO_COOKIE;
      per_subs_info_p->subs_id = subs_id;

      per_subs_info_p->service_request_pending_flag = FALSE;
      per_subs_info_p->timer_running = FALSE;
      per_subs_info_p->pending_redial_tmr_running = FALSE;
      per_subs_info_p->uplink_data_bitmask = 0;
      per_subs_info_p->rab_reject_local_cause = FALSE;

      /*-------------------------------------------------------------------------
        Define the timer for rab reestablishment
      -------------------------------------------------------------------------*/
      rex_def_timer_ex(&(per_subs_info_p->tmr_rab_reestab),
                       (rex_timer_cb_type) ds_bearer_cntxt_reorig_dorm_timer_cb,
                        subs_id);

     /* Allocating timer for cleaningup calls in Pending Redial State */
     rex_def_timer_ex
     (
       &(per_subs_info_p->pending_redial_tmr),
       (rex_timer_cb_type)ds_3gpp_bearer_cntxt_pending_redial_timer_expiry_cb,
       subs_id 
     );

     ds_bearer_cntxt_set_per_subs_info_hndl(subs_id, per_subs_info_p);
    }
  }
  while (0); 
} /* ds_bearer_cntxt_per_subs_info_init */

#ifdef TEST_FRAMEWORK
#error code not present
#endif

/*===========================================================================
FUNCTION  ds_3gpp_bearer_cntxt_init_fast_domrnacy_params

DESCRIPTION
  This function will initialize the fast dormancy paramters. 
  Enabled flag and Timer values. If GCF NV is enabled and FD 
  is running few GCF test cases will fail

PARAMETERS
  None
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_bearer_cntxt_init_fast_domrnacy_params
(
  void
)
{
  nv_stat_enum_type   status;
  nv_item_type        nv_item; 
  boolean             gcf_flag = FALSE;
  /*----------------------------------------------------------------*/
  ds_3gpp_bearer_cntxt_set_modem_fd_timer_1_value(
                                       DS_3GPP_FD_DEFAULT_OVERALL_TIMER_VALUE);
  ds_3gpp_bearer_cntxt_set_modem_fd_timer_2_value( 
                                     DS_3GPP_FD_DEFAULT_AGRESSIVE_TIMER_VALUE);
  /* Read GCF NV Item from EFS */
  status = ds3gcfgmgr_read_legacy_nv_ex( NV_GPRS_ANITE_GCF_I,
                                         &nv_item,
                                         SYS_MODEM_AS_ID_1);

  if(NV_DONE_S == status)
  {
    gcf_flag = nv_item.gprs_anite_gcf;
  }
  else
  {
    /* If problem reading NV_GPRS_ANITE_GCF_I from NV */
    DS_3GPP_MSG0_HIGH("Error reading NV_GPRS_ANITE_GCF_I from NV");
  }

  if(gcf_flag == TRUE)
  {
    /* as GCF NV is Enabled Modem Fast Dormancy should be Disabled 
       by default */
    ds_3gpp_bearer_cntxt_set_modem_fd_enabled_flag(FALSE);
  }
  else
  {
    /*  Read FD Enabled flag from EFS */
    ds3gcfgmgr_read_fast_dormancy_enabled_flag_from_efs();

    /* Read EFS and store Fast Dormancy  Timer information */ 
    if (TRUE != ds3gcfgmgr_read_fast_dormancy_info_from_efs())
    {
      /* Init to default Timer Values */
      DS_3GPP_MSG0_HIGH("EFS reading failed for FD Timers, Inited to default");
    }

    DS_3GPP_MSG3_HIGH("FD_INFO:Enabled: %d, Timer_1_value: %d ms, Timer_2_value:"
                                           "%d ms",ds_3gpp_fd_info.is_FD_enabled,
                                                   ds_3gpp_fd_info.timer_1_value, 
                                                   ds_3gpp_fd_info.timer_2_value);
  }
}

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_3GPP_INIT

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
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_bearer_cntxt_3gpp_init( void )
{ 
  uint32                  bearer_cntx_inst;
  ds_bearer_context_s     *bearer_context_ptr;
  nv_stat_enum_type       status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  char access_barring_rab_retry_timer_path[] = 
               "/nv/item_files/data/3gpp/access_barring_rab_retry_timer"; 

  char access_barring_rab_retry_counter_path[] = 
               "/nv/item_files/data/3gpp/access_barring_rab_retry_counter"; 

  /*--------------------------------------------------------------------------
    For each bearer in the bearer context table, initialize all the fields
    that are associated with the bearer.
  --------------------------------------------------------------------------*/
  for( bearer_cntx_inst=0;\
       bearer_cntx_inst<DS_3GPP_MAX_BEARER_CONTEXT;\
       bearer_cntx_inst++ )
  {
    bearer_context_ptr = ds_bearer_cntxt_get_instance_ptr(bearer_cntx_inst);

    memset( (void*)bearer_context_ptr, 0x0, (sizeof(ds_bearer_context_s)) );

     /* Initializes the SNDCP/PDCP UL and DL watermarks for each bearer
   instance. */
     ds_bearer_cntxt_initialize_l2_wms(bearer_context_ptr);
    /*-------------------------------------------------------------------------
        Now create the phys link that is associated with the bearer. We have only
        one phys link per bearer. Hence we call this API with the pointer to the
        phys link and 1 as the second argument.
     -------------------------------------------------------------------------*/
    (void) ps_phys_link_create(&(bearer_context_ptr->phys_link), DS_3GPP_MAX_PHYS_LINK_PER_BEARER);
      /*lint -e774 -e506 -e641 Boolean within 'left side of && 
        within left side of && within left side of && within if' always evaluates to True*/
    PS_PHYS_LINK_SET_CAPABILITY( &(bearer_context_ptr->phys_link),
                      (int)PS_PHYS_LINK_CAPABILITY_FLOW_DECOUPLED);
      /*lint +e774 +e506 +e641 */


    bearer_context_ptr->
      phys_link.phys_link_down_cmd_f_ptr =
           ds_bearer_cntxt_phys_link_down;

    bearer_context_ptr->
      phys_link.phys_link_go_null_cmd_f_ptr = ds_bearer_cntxt_phys_link_down;

    bearer_context_ptr->phys_link.phys_link_up_cmd_f_ptr =
           ds_bearer_cntxt_phys_link_up;
      /* Hari: TODO define qos release and modify handlers*/        
    bearer_context_ptr->phys_link.ioctl_f_ptr =
           ds_bearer_cntxt_phys_link_ioctl_hdlr;

  }/* for DS_3GPP_MAX_BEARER_CONTEXT */



 
    /*------------------------------------------------------------------------:
    Read access_barring_rab_retry_timer NV setting.
  -------------------------------------------------------------------------*/
  
  status = ds3gcfgmgr_read_efs_nv(
                           access_barring_rab_retry_timer_path,
                           (void *)&access_barring_rab_retry_timer,
                           sizeof(uint16)
                          );

  if (status != NV_DONE_S)
  {
    access_barring_rab_retry_timer = 
        DS_BEARER_CNTXT_RAB_REESTABLISH_MIN_RETRY_TIME;

    DS_3GPP_MSG1_ERROR("Failed reading Access Barring RAB RETRY TIMER,"
                       "Setting it to default value:%d",
                       access_barring_rab_retry_timer);
  }
  else
  {
    if (access_barring_rab_retry_timer < 
        DS_BEARER_CNTXT_RAB_REESTABLISH_MIN_RETRY_TIME )
    {
      access_barring_rab_retry_timer = 
        DS_BEARER_CNTXT_RAB_REESTABLISH_MIN_RETRY_TIME;
    }

    DS_3GPP_MSG1_HIGH("Access Barring RAB retry timer set to %d",
                      access_barring_rab_retry_timer);
  }


  /*------------------------------------------------------------------------:
    Read access_barring_rab_retry_counter NV setting.
  -------------------------------------------------------------------------*/
  
  status = ds3gcfgmgr_read_efs_nv(
                           access_barring_rab_retry_counter_path,
                           (void *) &access_barring_rab_retry_counter,
                           sizeof(uint16)
                          );

  if (status != NV_DONE_S)
  {
    access_barring_rab_retry_counter = 
      DS_BEARER_CNTXT_ACCESS_BARR_RAB_RETRY_COUNTER;

    DS_3GPP_MSG1_ERROR("Failed reading Access Barring RAB RETRY COUNTER,"
                       "Setting it to default value:%d",
                       access_barring_rab_retry_counter);
  }
  else
  {
    DS_3GPP_MSG1_HIGH("Access Barring RAB retry counter set to %d",
                      access_barring_rab_retry_counter);
  }

#ifdef FEATURE_DATA_LTE

  dsmsgrrcv_reg_hdlr(  
    LTE_RRC_DEACTIVATE_REQ,
    (dsmsgrcv_msg_hdlr_f)ds_bearer_cntxt_lte_deactivate_req_hdlr
  );
#endif /* FEATURE_DATA_LTE */

 ds_3gpp_bearer_cntxt_init_fast_domrnacy_params();

  /* double check if it needs to be done in init*/
  ps_dpm_um_ev_cback_reg(PS_SYS_TECH_3GPP, ds_bearer_cntxt_dpm_ev_cb);
/* Register the callback here */
  ps_comp_logging_register_fetch_iface_cback(SYS_SYS_MODE_WCDMA,
                                             ds_3gpp_bearer_cntxt_comp_log_fetch_iface);

} /* ds_bearer_cntxt_3gpp_init() */

/*=============================================================================
FUNCTION DS_BEARER_CNTXT_DPM_EV_CB

DESCRIPTION
  This is the call back function that is called when the Dpm events are invoked.
  Based on these events; the watermark's nonemmpty function callback is set
  reset.
PARAMETERS
  IN
    dpm_event       - The event that happened. This is used to differentiate
                      the events that happened : registration, deregistration
                      reregistration
    event_info_ptr   - Passed in as void*, this gives us the event details 
            
DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void ds_bearer_cntxt_dpm_ev_cb
(
   ps_dpm_um_ev_enum_type                    dpm_event, 
   void                                    * event_info_ptr
)
{
  dsm_watermark_type  *rx_wm_p = NULL; 
  dsm_watermark_type  *irat_src_rx_wm_p = NULL;
  ps_dpm_um_ev_cback_info_type             *cback_info_ptr = NULL;
  ps_phys_link_type                        *phys_link_ptr = NULL;
  ds_bearer_context_s                      *bearer_context_ptr = NULL;
  int                                       loopback_index = -1;
  uint32                                    phys_link_client_info = 0;
#ifdef FEATURE_DATA_EMBMS
  ds_3gpp_embms_ctrl_blk_type               *ds_3gpp_embms_blk = NULL;
#endif /*FEATURE_DATA_EMBMS*/
/*---------------------------------------------------------------------------*/
  cback_info_ptr = (ps_dpm_um_ev_cback_info_type *)event_info_ptr;

  if(cback_info_ptr == NULL)
  {
     DS_3GPP_MSG0_ERROR("Event Info Pointer is Null");
     return;
  }

  phys_link_ptr = cback_info_ptr->phys_link_ptr;

#ifdef FEATURE_DATA_EMBMS
  if(cback_info_ptr->v4_iface_ptr == cback_info_ptr->v6_iface_ptr &&
     cback_info_ptr->v4_iface_ptr != NULL)
  {
    if(phys_link_ptr != NULL)
    {
      ds_3gpp_embms_blk = (ds_3gpp_embms_ctrl_blk_type *)
                                      phys_link_ptr->client_data_ptr;
      if(ds_3gpp_embms_blk != NULL)
      {
        ds_3gpp_embms_initialize_rlc_to_ps_wm (ds_3gpp_embms_blk);
      }
    }
    else
    {
      DS_3GPP_MSG0_ERROR ("Invalid Phys Link Ptr passed by DPM for EMBMS");
    }
    return;
  }
  else
#endif /*FEATURE_DATA_EMBMS*/
  {
    if(phys_link_ptr != NULL)
    {
      phys_link_client_info = (uint32)phys_link_ptr->client_data_ptr;
      DS_3GPP_MSG1_MED ("Call Instance retrieved from Phys Link: %d",
                        phys_link_client_info);
      loopback_index = phys_link_client_info;
    }
    else
    {
      DS_3GPP_MSG1_HIGH ("Invalid Phys Link Ptr passed by DPM, Event is %d",
                         dpm_event);
    }
  }

  /* Loopback scenario */
  if(ds_loopback_hdlr_is_instance_enabled(loopback_index))
  {
    switch(dpm_event)
    {
      case PS_DPM_UM_EV_BEARER_REG_COMPLETE:
        if(!cback_info_ptr->is_hw_accelerated)
        {          
          ds_loopback_hdlr_set_nonempty_callback(loopback_index);
        }
        break;

      case PS_DPM_UM_EV_BEARER_DEREG_COMPLETE:
        ds_loopback_hdlr_reset_nonempty_callback(loopback_index);
        break;

      case PS_DPM_UM_EV_IFACE_REG_COMPLETE:
      case PS_DPM_UM_EV_IFACE_DEREG_COMPLETE:
      case PS_DPM_UM_EV_BEARER_REREG_COMPLETE:
        DS_3GPP_MSG1_HIGH("Un-handled event: %d. Ignoring",dpm_event);
        break; 
      
      default:
      {
        DS_3GPP_MSG3_HIGH("Un-expected event: %d. Ignoring",dpm_event,0,0);
      }
      break;
    }
  }
  else
  {
    if(phys_link_ptr != NULL)
    {
      if(phys_link_client_info < DS_3GPP_MAX_BEARER_CONTEXT)
      {
        bearer_context_ptr = 
          ds_bearer_cntxt_get_instance_ptr(phys_link_client_info);

        /* Regular scenario */
        if(!ds_bearer_cntx_validate_bearer_context(bearer_context_ptr))
        {
          return;
        }
      }
     
      /* rx_wm_p-> represents the rat associated with rat on which event
         happened ,src_wm_p -> represent the other wm; which we deregister
         in case of a rereg event */
      switch(cback_info_ptr->rat)
      {
        case DS_SYS_RAT_EX_3GPP_LTE:
            rx_wm_p = &(bearer_context_ptr->lte_rx_wm.wm_item);
            irat_src_rx_wm_p = &(bearer_context_ptr->rx_wm.wm_item);
           break;

         case DS_SYS_RAT_EX_3GPP_GERAN:
         case DS_SYS_RAT_EX_3GPP_WCDMA:
         case DS_SYS_RAT_EX_3GPP_TDSCDMA:
           rx_wm_p = &(bearer_context_ptr->rx_wm.wm_item);
           irat_src_rx_wm_p = &(bearer_context_ptr->lte_rx_wm.wm_item);
           break;

         default:
            DS_3GPP_MSG1_HIGH("Unhandled rat: %d. Ignoring",cback_info_ptr->rat);
         break;
      }
    }

    if((rx_wm_p == NULL) ||
       (irat_src_rx_wm_p == NULL))
    {
       DS_3GPP_MSG0_ERROR("rx_wm_p or irat_src_rx_wm_p is NULL");
       return;       
    }

    switch(dpm_event)
    {
      case PS_DPM_UM_EV_BEARER_REG_COMPLETE:

        if(!cback_info_ptr->is_hw_accelerated)
        {
          rx_wm_p->non_empty_func_ptr = 
               (wm_cb_type)ds_3gpp_bearer_cntxt_rx_data_at_u_pdp_cb;
          rx_wm_p->non_empty_func_data = (void*) bearer_context_ptr;
        }  
        break;

      case PS_DPM_UM_EV_BEARER_DEREG_COMPLETE:
        rx_wm_p->non_empty_func_ptr = NULL;
        rx_wm_p->non_empty_func_data = NULL;
  	  
        break;

      case PS_DPM_UM_EV_BEARER_REREG_COMPLETE:
        if(!cback_info_ptr->is_hw_accelerated)
        {
           rx_wm_p->non_empty_func_ptr = 
                 (wm_cb_type)ds_3gpp_bearer_cntxt_rx_data_at_u_pdp_cb;
           rx_wm_p->non_empty_func_data = (void*) bearer_context_ptr;
        }
        break;

      case PS_DPM_UM_EV_IFACE_REG_COMPLETE:
      case PS_DPM_UM_EV_IFACE_DEREG_COMPLETE:
        DS_3GPP_MSG1_HIGH("Un-handled event: %d. Ignoring",dpm_event);
        break;  

      default:
          DS_3GPP_MSG1_HIGH("Un-expected event: %d. Ignoring",dpm_event);
        break;
    }
  }
} /* ds_bearer_cntxt_dpm_ev_cb */

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_REGISTER_PDN_CONTEXT

DESCRIPTION
  This fucntion is called by PDN context to register itself with the bearers
  associated with it. This function attaches the PDN, bearer contexts and the
  phys links associated with them. The bearer callbacks to the PDN context is
  also stored in the bearer context. This function is called per bearer.

PARAMETERS
  IN
    pdn_context_p     - pointer to the pdn context stored as void* in bearer.
    bearer_context_p  - pointer to bearer context
    pdn_context_cb_tbl_p
                      - pointer to the call call back table. Stored in bearer
		        to be called later when bearer goes up / down
    is_default        - Boolean that tells if the bearer is default or not.
DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_bearer_cntxt_register_pdn_context
( 
  void                             *pdn_context_ptr,  
  ds_bearer_context_s              *bearer_context_ptr,
  boolean                          is_default
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_ptr) == FALSE ||
      ds_bearer_cntx_validate_bearer_context(bearer_context_ptr) == FALSE )
  {
    ASSERT(0);
    return;
  }

  /*---------------------------------------------------------------------------
    Set the is_default flag of the bearer based on the argument passed into the
    function.
  ---------------------------------------------------------------------------*/
  bearer_context_ptr->ds_bearer_context_dyn_p->is_default = is_default;   

  /*---------------------------------------------------------------------------
    Initialize the other fields of the bearer context - like the phys link,
    pdn_context call back table pointer, pdn_context.
  ---------------------------------------------------------------------------*/

  bearer_context_ptr->ds_bearer_context_dyn_p->pdn_context_ptr = pdn_context_ptr;
  bearer_context_ptr->rm_flow_enabled = TRUE; 
  return;
} /* ds_bearer_cntxt_register_pdn_context() */

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_DEREGISTER_PDN_CONTEXT

DESCRIPTION
  This fucntion is called by PDN context to deregister itself with the bearer
  associated with it. This function detaches the PDN, bearer contexts and the
  phys links associated with them. This function is called per bearer.

PARAMETERS
  IN
    bearer_context_p  - pointer to bearer context that is to be updated.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_bearer_cntxt_deregister_pdn_context( 
  ds_bearer_context_s              *bearer_context_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ds_bearer_cntx_validate_bearer_context(bearer_context_ptr) == FALSE)
  {
    ASSERT(0);
    return;
  }

  /*---------------------------------------------------------------------------
    Initialize the other fields of the bearer context - like the phys link,
    pdn_context call back table pointer, pdn_context.
  ---------------------------------------------------------------------------*/

  bearer_context_ptr->ds_bearer_context_dyn_p->pdn_context_ptr = NULL;
  
  if(bearer_context_ptr->ds_bearer_context_dyn_p->is_default == TRUE)
  {
    /*-----------------------------------------------------------------------
      Enable Bridge events if it is the default bearer.
      We want only the default bearer to act on flow control on RM phys link
    -----------------------------------------------------------------------*/
    //ps_phys_link_free_event_cback_buf(bearer_context_ptr->ds_bearer_context_dyn_p->rm_flow_enable_buf_ptr);
    //bearer_context_ptr->ds_bearer_context_dyn_p->rm_flow_enabled = TRUE;
  }

  /*---------------------------------------------------------------------------
    Set the is_default flag of the bearer to TRUE
  ---------------------------------------------------------------------------*/
  bearer_context_ptr->ds_bearer_context_dyn_p->is_default = FALSE;

  return;
} /* ds_bearer_cntxt_deregister_pdn_context() */

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_GET_BEARER_FROM_CALL_ID

DESCRIPTION
  This function is used to find the bearer context, given a CM call id

PARAMETERS
  IN
    cm_call_id_type - CM call ID

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the bearer context if there is a CM call ID match
  NULL if there is no match

SIDE EFFECTS
  None.

===========================================================================*/
ds_bearer_context_s* ds_bearer_cntxt_get_bearer_from_call_id
(
  cm_call_id_type call_id
)
{
  int                 bearer_cntx_inst;
  ds_bearer_context_s *bearer_context_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(bearer_cntx_inst = 0;\
      bearer_cntx_inst < DS_3GPP_MAX_BEARER_CONTEXT;\
      bearer_cntx_inst++)
  {
    bearer_context_p = ds_bearer_cntxt_get_instance_ptr(bearer_cntx_inst);

    if( ds_bearer_cntx_validate_bearer_context_internal(bearer_context_p) 
        && bearer_context_p->ds_bearer_context_dyn_p->cm_call_id == call_id)
    {
      DS_3GPP_MSG1_HIGH("Found the bearer context with CM call id: %d",call_id);
      break;
    }
  }
  /*---------------------------------------------------------------------------
    If we are not able to find a match based on the call id, the following
    will be printed. NULL will be returned since bearer_context_p is NULL
  ---------------------------------------------------------------------------*/
  if(bearer_cntx_inst >= DS_3GPP_MAX_BEARER_CONTEXT)
  {
    DS_3GPP_MSG1_ERROR("Cannot find the requested bearer context with call id: %d",call_id);
    bearer_context_p = NULL;
  }

  return bearer_context_p;
} /* ds_bearer_cntxt_get_bearer_from_call_id() */

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_GET_BEARER_CNTXT

DESCRIPTION
  This function is used to get the bearer context from the call info. It
  gets the call instance from the call info and returns a pointer to the
  bearer context.

PARAMETERS
  IN
    const void* mode_specific_info_p - This is type cast into the call info

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the bearer context if the call id is valid.
  NULL otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
ds_bearer_context_s* ds_bearer_cntxt_get_bearer_cntxt
(
  const void* mode_specific_info_p
)
{
  int                 bearer_cntx_inst;
  ds_bearer_context_s *bearer_context_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for( bearer_cntx_inst = 0;\
       bearer_cntx_inst < DS_3GPP_MAX_BEARER_CONTEXT;\
       bearer_cntx_inst++)
  {
    bearer_context_p = ds_bearer_cntxt_get_instance_ptr(bearer_cntx_inst);

    if( ds_bearer_cntx_validate_bearer_context_internal(bearer_context_p) && 
        bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr ==\
         (void*)((ds_3gpp_pdn_call_info_type*)mode_specific_info_p)->call_instance)
    {
      DS_3GPP_MSG1_HIGH("Found the bearer context with instance id: %d",\
          ((ds_3gpp_pdn_call_info_type*)mode_specific_info_p)->call_instance);
      break;
    }
  }
  if(bearer_cntx_inst >= DS_3GPP_MAX_BEARER_CONTEXT)
  {
    DS_3GPP_MSG1_ERROR("Cannot find the requested bearer context with instance: %d",\
        ((ds_3gpp_pdn_call_info_type*)mode_specific_info_p)->call_instance);
    bearer_context_p = NULL;
  }
  return bearer_context_p;
} /* ds_bearer_cntxt_get_bearer_cntxt() */

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_UPDATECALLID 

DESCRIPTION
  This function finds the bearer context based on the call info pointer
  passed into it and updates it with the call id. This function is called
  after we initiate a call with CM. CM gets back to 3gpp handler with
  call_id handler and this function is inturn called by 3gpp hdlr to
  update the bearer with the CM call id

PARAMETERS
  IN
    mode_specific_info_p - call info to identify the bearer context
    call_id              - CM call id to update the bearer with

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the bearer was found and the call Id was stored in it.
  FALSE if the bearer could not be found

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_bearer_cntxt_update_call_id
( 
  const void      *mode_specific_info_p,
  cm_call_id_type call_id
)
{
  ds_bearer_context_s *bearer_context_p;
  boolean             retVal = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(mode_specific_info_p != NULL);
  bearer_context_p = ds_bearer_cntxt_get_bearer_cntxt(mode_specific_info_p);

  //If we are able to locate the bearer context, update it with the call id from CM
  if(ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    bearer_context_p->ds_bearer_context_dyn_p->cm_call_id = call_id;
    retVal = TRUE;
  }
  else
  {
    DS_3GPP_MSG1_HIGH("Cannot locate the bearer context corresponding to the call inst :%d",\
        ((ds_3gpp_pdn_call_info_type*)mode_specific_info_p)->call_instance);
    retVal = FALSE;
  }
  return retVal;
}/* ds_bearer_cntxt_updateCallId() */

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_IS_DEFAULT

DESCRIPTION
  This function is an accessor function that tells if a given bearer context
  is default or not.

PARAMETERS
  IN
    bearer_context_p - pointer to the bearer context that we are interested in

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the bearer is default
  FALSE if the bearer is not default.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_cntxt_is_default(ds_bearer_context_s* bearer_context_p)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return FALSE;
  }

  return bearer_context_p->ds_bearer_context_dyn_p->is_default;
} /* ds_bearer_cntxt_is_default() */

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_IS_DEFAULT_CALL_ID
DESCRIPTION
  This function is an accessor function that tells if a given call id maps to 
  default bearer or not.

PARAMETERS

    call ID  - call ID  that we are interested in

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the bearer is default
  FALSE if the bearer is not default.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_cntxt_is_default_call_id(cm_call_id_type   call_id)
{

  ds_bearer_context_s* bearer_context_p;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  bearer_context_p = ds_bearer_cntxt_get_bearer_from_call_id(call_id);

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return FALSE;
  }

  return bearer_context_p->ds_bearer_context_dyn_p->is_default;
} /* ds_bearer_cntxt_is_default() */

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_GETPDNCNTXT

DESCRIPTION
  This function is used to get the PDN context from the bearer context.

PARAMETERS
  IN
    bearer_context_p - Pointer to the bearer context.

DEPENDENCIES
  None.

RETURN VALUE
  Void* - Pointer to the PDN context on Success
          NULL on failure

SIDE EFFECTS
  None.

===========================================================================*/
void* ds_bearer_cntxt_get_pdn_cntxt(ds_bearer_context_s* bearer_context_p)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return NULL;
  }

  return bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr;
} /* ds_bearer_cntxt_getPdnCntxt() */

#if 0 //unused so far
/* Function Header */
ds_umts_umts_qos_params_type* ds_bearer_cntxt_get_umts_qos_val
(
  ds_bearer_context_s* bearer_context_p
)
{
  return ( bearer_context_p->ds_bearer_context_dyn_p->vobj_data_ptr_tbl
  [DS_3GPP_UMTS_VOBJ] == NULL? NULL:\
    &( ((ds_umts_bearer_context_s*)bearer_context_p->ds_bearer_context_dyn_p
    ->vobj_data_ptr_tbl[DS_3GPP_UMTS_VOBJ])->\
    umts_qos_val) );
}

/* Function Header */
ds_umts_gprs_qos_params_type* ds_bearer_cntxt_get_gprs_qos_val
(
  ds_bearer_context_s* bearer_context_p
)
{
  return ( bearer_context_p->ds_bearer_context_dyn_p->vobj_data_ptr_tbl
  [DS_3GPP_UMTS_VOBJ] == NULL? NULL:\
    &( ((ds_umts_bearer_context_s*)bearer_context_p->ds_bearer_context_dyn_p
    ->vobj_data_ptr_tbl[DS_3GPP_UMTS_VOBJ])->\
    gprs_qos_val) );
}
#endif //#if 0

/*===========================================================================
FUNCTION  DS_3GPP_REPORT_BEARER_THROUGHPUT_STATS

DESCRIPTION 
 This function fills bearer throughput stats for the incoming bearer context

PARAMETERS 
  ds_bearer_context_s*                       - Pointer to the bearer context
 
  ps_sys_bearer_throughput_info_type*        - Pointer to fill in the bearer
                                               throughput stats
 
DEPENDENCIES 
  This utility function will be used only when
  * The bearer context is valid
        &
  * Throughput Info Indication timer is running
 
RETURN VALUE
  TRUE   - If the bearer throughput stats for the given bearer is reported
            successfully
 
  FALSE  - Otherwise
            
SIDE EFFECTS 
  None

===========================================================================*/
boolean ds_3gpp_report_bearer_throughput_stats
(
  ds_bearer_context_s                         *bearer_context_p,
  ps_sys_bearer_throughput_info_type          *bearer_info_p

)
{
  boolean                      ret_val              = FALSE;
#ifdef FEATURE_DATA_LTE
  ds_eps_bearer_context_s     *eps_bearer_cntxt_p   = NULL;
#endif /* FEATURE_DATA_LTE */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*------------------------------------------------------------------------- 
    Validate the arguments
    -------------------------------------------------------------------------*/
  if (!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return ret_val;
  }

  if (bearer_info_p == NULL)
  {
    DS_3GPP_MSG0_HIGH("bearer Info ptr is NULL");
    return ret_val;
  }

  DS_3GPP_MSG2_MED("Bearer context 0x%x Bearer call mode %d",bearer_context_p,
                    bearer_context_p->ds_bearer_context_dyn_p->call_mode);
  /*-------------------------------------------------------------------------- 
    Use EPS bearer ID if the bearer call mode is LTE otherwise use 'nsapi'
    as the bearer ID
    --------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_LTE
  if (bearer_context_p->ds_bearer_context_dyn_p->call_mode == SYS_SYS_MODE_LTE)
  {
    eps_bearer_cntxt_p = (ds_eps_bearer_context_s *)
                       (bearer_context_p->ds_bearer_context_dyn_p->
                        vobj_data_ptr_tbl[DS_3GPP_EPS_VOBJ]);
    /*----------------------------------------------------------------------- 
      validate EPS bearer context
      -----------------------------------------------------------------------*/
    if(!ds_eps_bearer_cntxt_validate_bearer_cntxt(eps_bearer_cntxt_p))
    {
      return FALSE;
    }
    /*---------------------------------------------------------------------- 
      Bearer call mode LTE. Fill EPS bearer ID as the bearer ID
      ----------------------------------------------------------------------*/
    bearer_info_p->bearer_rlp_mac_id        = eps_bearer_cntxt_p->
                                              ds_eps_bearer_context_dyn_p->
                                              eps_bearer_id;
  }
  else
#endif /* FEATURE_DATA_LTE */
  {
    /*---------------------------------------------------------------------- 
      Bearer call mode UMTS. Fill nsapi as the bearer ID
      ----------------------------------------------------------------------*/
    bearer_info_p->bearer_rlp_mac_id        = bearer_context_p->
                                              ds_bearer_context_dyn_p->
                                              nsapi;
  }
  /*------------------------------------------------------------------------- 
    Get the latest bearer throughput stats which are cached in the
    bearer control block
    -------------------------------------------------------------------------*/
  bearer_info_p->bearer_uplink_actual_rate  = (uint32)bearer_context_p->
                                              ds_bearer_context_dyn_p->
                                              bearer_throughput_stats.actual_ul_rate;
  bearer_info_p->bearer_uplink_queue_size   = bearer_context_p->
                                              rx_wm.wm_item.current_cnt;
  bearer_info_p->is_default_bearer          = bearer_context_p->
                                              ds_bearer_context_dyn_p->
                                              is_default;
  ret_val = TRUE;

  return ret_val;
}
/*===========================================================================
FUNCTION  DS_3GPP_REPORT_BEARER_THROUGHPUT_STATS_FOR_PDN

DESCRIPTION 
 This utility function reports the bearer throughput statistics for all the
 bearers associated with the given PDN

PARAMETERS 
  
  ds_pdn_context_s*                        - Pointer to PDN context 
 
  ds_sys_bearer_throughput_info_list_type* - Pointer to fill in the
                                             the bearer throughput stats
                                             for all the bearers associated
                                             with the PDN context
 
DEPENDENCIES 
  This utility function will be used only when
  * There is atleast one bearer associated with the PDN
        &
  * Throughput Info Indication timer is running
 
RETURN VALUE
  TRUE   - If the bearer throughput stats is reported successfully for
           the Input PDN
 
  FALSE  - Otherwise
            
SIDE EFFECTS None

===========================================================================*/
boolean ds_3gpp_report_bearer_throughput_stats_for_pdn
(
  ds_pdn_context_s                          *pdn_context_p,
  ds_sys_bearer_throughput_info_list_type   *bearer_info_list_p
)
{
  uint8                               index                   = 0;
  boolean                             ret_val                 = FALSE;
  ds_bearer_context_s                *bearer_cntxt_p          = NULL;
  uint8                               bearer_info_list_index  = 0;
  ds_sys_bearer_throughput_info_type *bearer_info_p          = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------- 
    Validate the input arguments
    -------------------------------------------------------------------------*/
  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_p))
  {
    return ret_val;
  }

  if (bearer_info_list_p == NULL)
  {
    DS_3GPP_MSG0_HIGH("Bearer Info List NULL. Not reporting bearer list");
    return ret_val;
  }
  /*------------------------------------------------------------------------- 
    Fill in bearer throughput statistics for all active bearers one bearer
    at a time
    -------------------------------------------------------------------------*/
  for (index = 0; index < DS_3GPP_MAX_BEARER_CONTEXT_PER_PDN_CONTEXT;index++)
  {
    bearer_cntxt_p =(ds_bearer_context_s*)pdn_context_p->ds_pdn_context_dyn_p
                                            ->bearer_context_ptr_tbl[index];
    bearer_info_list_index = bearer_info_list_p->num_bearers;
    bearer_info_p = &bearer_info_list_p->
                                    bearer_trpt_info[bearer_info_list_index];
    /*----------------------------------------------------------------------- 
      Validate Bearer context
      -----------------------------------------------------------------------*/
    if (!ds_bearer_cntx_validate_bearer_context_internal(bearer_cntxt_p))
    {
      continue;
    }
    /*----------------------------------------------------------------------- 
      Get the bearer throughput statistics for the current bearer
      -----------------------------------------------------------------------*/
    if (TRUE==ds_3gpp_report_bearer_throughput_stats(bearer_cntxt_p,bearer_info_p))
    {
      bearer_info_list_p->num_bearers++;
    }
  }
  /*--------------------------------------------------------------------------- 
    Return True if there is atleast one block filled in the bearer info list
    ---------------------------------------------------------------------------*/
  if (bearer_info_list_p->num_bearers > 0)
  {
    ret_val = TRUE;
  }

  DS_3GPP_MSG2_MED("%d Bearer(s) associated with PDN 0x%x",
                   bearer_info_list_p->num_bearers,pdn_context_p);
  return  ret_val;
}
/*===========================================================================
FUNCTION  DS_3GPP_BEARER_CNTXT_INVOKE_BEARER_BRINGUP

DESCRIPTION
  This function calls the bearer bring_up function. It also cleans up the
  bearer and calls bearer_down_ind_cb() if bringup fails.

PARAMETERS
  bearer_instance  - call index to be able to locate the bearer
  info_ptr       - ptr to network_params_info_type

DEPENDENCIES
  Assumes bearer is already allocated.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_bearer_cntxt_invoke_bearer_bringup
(
  uint32              bearer_instance,
  void                *info_ptr
)
{
  boolean                     useDefaultDialString = FALSE;
  ds_3gpp_pdn_call_info_type     call_info;
  boolean                     retVal = DS3G_SUCCESS;
  ds_3gpp_down_reason_t       net_down_reason;
  ds_bearer_context_s         *bearer_context_p;
  ds_pdn_context_s            *pdn_cntx_p;
  ds_bearer_flow_manager_s    *bflow_manager_p = NULL;  
  ds_flow_context_s           *flow_context_p = NULL;
#ifdef FEATURE_DATA_TE_MT_PDP
  cm_call_id_type             call_id = CM_CALL_ID_INVALID;
#endif /* FEATURE_DATA_TE_MT_PDP */
  sys_modem_as_id_e_type         subs_id = SYS_MODEM_AS_ID_NONE;
  /*-----------------------------------------------------------------------*/

  ASSERT(bearer_instance < DS_3GPP_MAX_BEARER_CONTEXT);
  /*---------------------------------------------------------------------------
    Get the bearer context pointer
  ---------------------------------------------------------------------------*/
  bearer_context_p = ds_bearer_cntxt_get_instance_ptr(bearer_instance);

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;

  }
  pdn_cntx_p = (ds_pdn_context_s*) bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr;

  if(!(ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p, &subs_id)))
  {
     return;
  }

  /*-------------------------------------------------------------------------
    Save the call instance to pass to ds3g_msh_initiate_call(). This will
    be used in the dsumtsps_orig_call_hdlr() to determine which call
    was being originated.
  -------------------------------------------------------------------------*/

  call_info.call_instance    = bearer_instance;
  call_info.info_ptr         = info_ptr;
  call_info.mt_flag          = FALSE;

  /* new UE initiated call */
  useDefaultDialString = ds_pdn_cntxt_use_default_dial_string( pdn_cntx_p,
                                                          info_ptr );
#ifdef FEATURE_DATA_TE_MT_PDP
    /* Check if call orig is due to a MT request */
    if ( (call_id = dsumtsps_mt_check_call_orig(pdn_cntx_p)) 
         != CM_CALL_ID_INVALID )
    {
      DS_3GPP_MSG3_HIGH("MT call orig, pdn_cntxt: %x, bearer_inst: %d, cm_call_id: %d",
                         pdn_cntx_p, bearer_instance, call_id);
      call_info.mt_flag          = TRUE;
      /* call_info.user_data        = (void *)call_id; */
    }
#endif /* FEATURE_DATA_TE_MT_PDP */
  retVal = ds_bearer_cntxt_bring_up(info_ptr,
                                  &call_info,
                                    useDefaultDialString,
                                    subs_id);

  if(retVal != DS3G_SUCCESS)
  {

    DS_3GPP_MSG1_ERROR("Error bringing up Bearer Context for Call Inst %d",\
               bearer_instance);

    ps_phys_link_enable_flow(&(bearer_context_p->phys_link),
                             DS_FLOW_UMTS_MASK);

    if (bearer_context_p->ds_bearer_context_dyn_p->is_default)
    {
      /*--------------------------------------------------------------------
        Error in initiating a primary call.
        Make sure that we call flow go null ind on default flows of all 
        ifaces associated with the PDN context
        Set the Bearer context to be in down state and call the bearer down
      call back to the PDN context.
      --------------------------------------------------------------------*/
      ds_pdn_cntxt_unbind_default_flows(bearer_context_p->ds_bearer_context_dyn_p
                                        ->pdn_context_ptr);        
      
      /* VRK:TODO This should not be under is_default */
      ps_phys_link_gone_ind(&(bearer_context_p->phys_link));
      ds_bearer_cntxt_free_bearer_inst(bearer_context_p);
    }//default bearer
    else
    {
      DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      /*-------------------------------------------------------------------------
        Update precedence_bit_mask and Release flow _context
        Bearer_down will release the bflowmgr and flowMgr          
      -------------------------------------------------------------------------*/    

      bflow_manager_p=ds_bearer_flow_manager_find_flow_manager(bearer_context_p);
      if(ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
      {
        /*-----------------------------------------------------------------------
          There should be only one pending flow context in the bearer flow 
          manager list.
        -----------------------------------------------------------------------*/
        if(list_size(&(bflow_manager_p->ds_bearer_flow_manager_dyn_p->flow_context)) 
           != 1)
        {
          DS_3GPP_MSG0_ERROR("Found more than one pending flow context");                        
        }
        flow_context_p = list_peek_front(&(bflow_manager_p->
                                           ds_bearer_flow_manager_dyn_p
                                           ->flow_context));          

      }
      else
      {
        DS_3GPP_MSG3_ERROR("Cound not find associated bearer flow manager to clean up",
                           0,0,0);      
      }
      if(flow_context_p != NULL)
      {
        if(ds_flow_manager_update_precedence_bit_mask(
                        pdn_cntx_p,flow_context_p,FALSE,FALSE,TRUE)==FALSE)
        {
          DS_3GPP_MSG0_ERROR("Error in clearing precedence bit mask");
        }     
      }
      else
      {
        DS_3GPP_MSG0_ERROR("Cound not find a pending flow context");      
      }               
      ds_flow_context_release_flow_context(flow_context_p);
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    }//dedicated bearer  

  
    /*-----------------------------------------------------------------------
      We have already cleaned up at the bearer level. We should send
      bearer down indication to PDN context with the boolean call_was_conn
      set to False.
      This way the PDN context will be freed up as well.
    -----------------------------------------------------------------------*/
    if (bearer_context_p->ds_bearer_context_dyn_p->pdn_context_cb_tbl_ptr->
        bearer_down_ind_cb != NULL)
    {
      net_down_reason.reason.ps_iface_down_reason = PS_NET_DOWN_REASON_NOT_SPECIFIED;
      net_down_reason.reason_T = DS_3GPP_PS_IFACE_NET_DOWN_REASON_T;
      bearer_context_p->ds_bearer_context_dyn_p->pdn_context_cb_tbl_ptr->
        bearer_down_ind_cb(
                    bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr,\
                  bearer_context_p,
                   net_down_reason,
                FALSE);
    }
  }
  else
  {
    /*------------------------------------------------------------------------
      Move the state of the bearer to COMING UP
    ------------------------------------------------------------------------*/
    bearer_context_p->ds_bearer_context_dyn_p->state = DS_BEARER_CONTEXT_STATE_COMING_UP;
  }
}
/*===========================================================================
FUNCTION  DS_BEARER_CNTXT_PHYS_LINK_UP_CMD_HDLR

DESCRIPTION
  Called in DS TASK context in response to a phys up command on
  the UMTS iface

  This function handles both the reorigination from dormancy as well as new
  call bring up. The difference between these two cases is identified by the
  dormant_reorig flag.

PARAMETERS
  call_instance  - call index to be able to locate the bearer
  info_ptr       - Not used.
  dormant_reorig - if this is reorig due to dormancy

DEPENDENCIES
  Called in DS TASK context.  Assumes that PS_IFACE has already
  set phys_link state to COMING_UP.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void ds_bearer_cntxt_phys_link_up_cmd_hdlr
(
  uint32      bearer_instance,
  void        *info_ptr
)
{
  ps_iface_type               *iface_v4_ptr = NULL; /* v4 Interface for call */
#ifdef FEATURE_DATA_PS_IPV6
  ps_iface_type               *iface_v6_ptr = NULL; /* v6 Interface for call */
#endif
  ds_pdn_context_s*           pdn_cntx_p = NULL;
  ds_bearer_context_s         *bearer_context_p;
  boolean                     valid_iface_state = FALSE;
  ds_umts_pdp_type_enum_type  pdn_pdp_type;
  boolean                     dormant_reorig;
  sys_modem_as_id_e_type      subs_id  = SYS_MODEM_AS_ID_NONE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ASSERT(bearer_instance < DS_3GPP_MAX_BEARER_CONTEXT);
 
  /*---------------------------------------------------------------------------
    Get the bearer context pointer
  ---------------------------------------------------------------------------*/

  bearer_context_p = ds_bearer_cntxt_get_instance_ptr(bearer_instance);
  
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
	DS_3GPP_MSG0_ERROR("Set phys_link to NULL");
    PS_PHYS_LINKI_SET_STATE(&(bearer_context_p->phys_link),PHYS_LINK_NULL);
    return;
  }

  /* Get the PDN context */
  pdn_cntx_p = (ds_pdn_context_s*)
                   (bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr);

  if(! ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p,&subs_id))
  {
     return;
  }

  DS_3GPP_MSG2_HIGH("PHYS LINK UP CMD HDLR. Bearer state: %d, Bearer instance %d",
                    bearer_context_p->ds_bearer_context_dyn_p->state, 
                    bearer_instance);
  /* Post phys link up indication if we are in PCH state  */
  if((bearer_context_p->ds_bearer_context_dyn_p->state == DS_BEARER_CONTEXT_STATE_UP) &&
     (PS_PHYS_LINK_GET_STATE(&(bearer_context_p->phys_link)) == PHYS_LINK_RESUMING)) 
  { 
    DS_3GPP_MSG0_HIGH("ds_bearer_cntxt_phys_link_up_cmd_hdlr: in PCH state ");
    ps_phys_link_up_ind_ex(&(bearer_context_p->phys_link),
                            PS_EIC_QOS_NETWORK_L2_LINK_REESTAB_IND);

    /*---------------------------------------------------------------------
      Send PS phys link disable for the bearer if subs is coex flow
      disabled
    ---------------------------------------------------------------------*/
    if( !ds3gflowmgr_get_coex_data_flow_status(subs_id) )
    {
      ps_phys_link_disable_flow(&(bearer_context_p->phys_link),
                                DS_FLOW_COEX_MASK);
    }

    /*--------------------------------------------------------------------- 
      if a PDN is in COMING_UP state, Flow mgr would be in ORIG state.
      Posting ACTIVE on the other PDN would result in starting the 
      flow monitor timer which could lead to data priority being lowered
      before the PDN is connected.
    ---------------------------------------------------------------------*/
    if (!dsPdnContext_is_any_normal_pdn_coming_up_ex(subs_id))
    {
      ds3gflowmgr_notify_ps_call_state_per_sub(subs_id,
                                               DS3GFLOWMGR_PS_CALL_ACTIVE_EV);
    }
    return; 
  }

  /* ds_bearer_cntxt_reorig_dorm calls phys_link_up_cmd to put the phys link in
     PHYS_LINK_COMING_UP state. We are ignoring the callback here */
  if(bearer_context_p->ds_bearer_context_dyn_p->state == 
     DS_BEARER_CONTEXT_STATE_UP_DORMANT_REESTAB) 
  {
     DS_3GPP_MSG0_MED("Phys link coming up. Ignoring DS3GPP_PHYS_LINK_UP_CMD");
    return; 
  }

  /*-------------------------------------------------------------------------
    It is possible that we enqueue a PHYS_LINK_UP command on N/W init QOS calls
    In such a case, we do not have to do any origination with CM.

    The ps_phys_link_up_cmd is called only to ensure a proper state transition
    for the phys link.
  -------------------------------------------------------------------------*/
  if(bearer_context_p->ds_bearer_context_dyn_p->state == 
     DS_BEARER_CONTEXT_STATE_PEND_RAB_REG)
  {
    DS_3GPP_MSG0_HIGH("Network initiated call. No need to do any mode spec handling");
    return;
  }

  if(bearer_context_p->ds_bearer_context_dyn_p->state == 
     DS_BEARER_CONTEXT_STATE_ANSWER)
  {
    if(ds_bearer_cntxt_answer_call(bearer_context_p) == DS3G_SUCCESS)
    {

      /*------------------------------------------------------------------------
        Move the state of the bearer to COMING UP
      ------------------------------------------------------------------------*/
      DS_3GPP_MSG2_HIGH("Call %d answered, moving bearer 0x%x to coming up state",
                        bearer_context_p->ds_bearer_context_dyn_p->cm_call_id,
                        bearer_context_p);
      bearer_context_p->ds_bearer_context_dyn_p->state =
         DS_BEARER_CONTEXT_STATE_COMING_UP;
    }
    return;
  }
  /*---------------------------------------------------------------------
    If we are getting out of dormancy, call state should be up.
    We are checking the call state from the pdn context and setting the
    reorig flag accordingly.
  ---------------------------------------------------------------------*/
  
  /* It could be that Physlink UP can happen during call cleanup and may lead to
        New PDN Request which is not the expectation */
        
  if(pdn_cntx_p->ds_pdn_context_dyn_p->tear_down_in_progress == TRUE)
  {
    DS_3GPP_MSG0_HIGH("Cleanup in progress, return");
    PS_PHYS_LINKI_SET_STATE(&(bearer_context_p->phys_link),PHYS_LINK_DOWN);
    return;
  }
  
  if ( bearer_context_p->ds_bearer_context_dyn_p->state != DS_BEARER_CONTEXT_STATE_DOWN &&
       bearer_context_p->ds_bearer_context_dyn_p->state != DS_BEARER_CONTEXT_STATE_PEND_REDIAL ) 
  {
    dormant_reorig = TRUE;
  }
  else
  {
    dormant_reorig = FALSE;
  }

  if(dormant_reorig == FALSE)
  {
     ds_3gpp_bearer_cntxt_invoke_bearer_bringup (bearer_instance, info_ptr);
  }
  else //Getting out of dormancy
  {
    DS_3GPP_MSG0_MED("Reoriginating from dormancy");
    /*---------------------------------------------------------------------
    Avoid handling dormancy reorig if the bearer is not dormant
    --------------------------------------------------------------------*/
    if( bearer_context_p->ds_bearer_context_dyn_p->state != 
        DS_BEARER_CONTEXT_STATE_UP_DORMANT &&
        bearer_context_p->ds_bearer_context_dyn_p->state != 
        DS_BEARER_CONTEXT_STATE_UP_DORMANT_REESTAB )
    {
      DS_3GPP_MSG2_ERROR("Bearer 0x%x is in state %d. Invalid state to "
                                 "reorig from dormancy ", bearer_context_p,
                                                    bearer_context_p->
                         ds_bearer_context_dyn_p->state);
      if (bearer_context_p->ds_bearer_context_dyn_p->state == DS_BEARER_CONTEXT_STATE_UP &&
          PS_PHYS_LINK_GET_STATE(&(bearer_context_p->phys_link)) != PHYS_LINK_UP) 
      {
        DS_3GPP_MSG1_ERROR("Bearer is UP but Physlink is in state: %d Assert",
                   PS_PHYS_LINK_GET_STATE(&(bearer_context_p->phys_link)));
        ASSERT(0);
      }
      return;
    }
    /*--------------------------------------------------------------------------- 
      Get the Iface from profile.
    ---------------------------------------------------------------------------*/
    pdn_pdp_type = ds_pdn_cntxt_get_pdn_context_ip_support(pdn_cntx_p);
    if(pdn_pdp_type == DS_UMTS_PDP_PPP ||
       pdn_pdp_type == DS_UMTS_PDP_IPV4)
    {
      iface_v4_ptr = 
        &(pdn_cntx_p->ds_pdn_context_dyn_p
          ->ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV4_IFACE]->ps_iface);
    }
#ifdef FEATURE_DATA_PS_IPV6
    else if(pdn_pdp_type == DS_UMTS_PDP_IPV6)
    {
      iface_v6_ptr = 
        &(pdn_cntx_p->ds_pdn_context_dyn_p
          ->ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV6_IFACE]->ps_iface); 
    }
    else if(pdn_pdp_type == DS_UMTS_PDP_IPV4V6)
    {
      iface_v4_ptr = 
        &(pdn_cntx_p->ds_pdn_context_dyn_p->
          ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV4_IFACE]->ps_iface);
      iface_v6_ptr = 
        &(pdn_cntx_p->ds_pdn_context_dyn_p->
          ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV6_IFACE]->ps_iface);
    }
#endif /* FEATURE_DATA_PS_IPV6 */
    else
    {
      DATA_ERR_FATAL("PDP type is not valid.");
    }

    /*---------------------------------------------------------------------
      Check if iface state is valid, if so trigger dormancy re-orig  
    ---------------------------------------------------------------------*/
    if (((iface_v4_ptr != NULL) &&
        ((ps_iface_state(iface_v4_ptr) == IFACE_UP) ||
         (ps_iface_state(iface_v4_ptr) == IFACE_ROUTEABLE)))           
#ifdef FEATURE_DATA_PS_IPV6
        ||
        ((iface_v6_ptr != NULL) &&
                          ((ps_iface_state(iface_v6_ptr) == IFACE_UP) ||
         (ps_iface_state(iface_v6_ptr) == IFACE_ROUTEABLE)))
#endif
       )
    {
      valid_iface_state = TRUE;
    }          

    if ((valid_iface_state) && 
        (bearer_context_p->ds_bearer_context_dyn_p->state != 
         DS_BEARER_CONTEXT_STATE_DOWN) &&
        (bearer_context_p->ds_bearer_context_dyn_p->state != 
         DS_BEARER_CONTEXT_STATE_GOING_DOWN))
    {   
      /*-----------------------------------------------------------------
        If we are in data suspend mode we should not reoriginate a call. We
        should disable flow on the interface and inform NAS that data is
        available.
      -----------------------------------------------------------------*/
      if (ds3gpp_get_ps_data_suspend_flag(subs_id) == TRUE)
      {
        if((!ds3gpp_get_ps_data_available_flag(subs_id)) && 
           ds3gpp_is_ps_data_avail_ind_allowed(subs_id))
        {
          DS_3GPP_MSG3_HIGH("phys_link_up:%d data_suspend:1 sending data_available",\
                            (uint32) bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr,0,0);
          ds3g_msh_ps_data_available_ind(subs_id);
          ds3gpp_set_ps_data_available_flag(TRUE, subs_id);
        }

        /*-----------------------------------------------------------------
          phys link state can be other than UP/DOWN in those cases flow enable
          will not get any effect so we will check phys link state in resume and
          post post phys link up before enabling the flow.
        -----------------------------------------------------------------*/
        /*--------------------------------------------------------------
          Disable flow on the interface
        --------------------------------------------------------------*/
        ps_phys_link_disable_flow( &(bearer_context_p->
                                     phys_link),
                           DS_FLOW_UMTS_MASK);

        return;
      }
      ds_bearer_cntxt_dormancy_reorig(bearer_context_p);
    } // if(IFACE_UP || ROUTEABLE)
    else
    {
      DS_3GPP_MSG0_ERROR("Invalid Iface/Bearer context state, cannot reorig");
    }
  } // is dormant_reorig
  return;
} /* ds_bearer_cntxt_phys_link_up_cmd_hdlr() */

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_PREPARE_FOR_TEARDOWN

DESCRIPTION
  This function is used to prepare the bearer context for teardown, Certain
  values would be set and appropriate teardown parameters will be populated.
 

PARAMETERS
  bearer_context_p - pointer to the bearer on which call is to be ended
  info_ptr         - void pointer

DEPENDENCIES
  None.

RETURN VALUE
  DS3G_SUCCESS (TRUE)
  DS3G_FAILURE (FALSE) on any failure

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_cntxt_prepare_for_teardown
(
  ds_3gpp_pdn_call_info_type    *mode_sp_info,
  ds_bearer_context_s        *bearer_context_p
)
{
  boolean                    retVal = FALSE;
  sys_sys_mode_e_type        network_mode;  /* mode currently in */
  sys_modem_as_id_e_type              subs_id = SYS_MODEM_AS_ID_NONE;
  ds_bearer_cntxt_per_subs_info_type  *per_subs_info_hndl = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if (mode_sp_info == NULL)
  {
    DS_3GPP_MSG0_ERROR("mode_sp_info is NULL");
    return retVal;
  }
  
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return retVal;
  }
          
  DS_3GPP_MSG1_HIGH("ds_bearer_cntxtt Tear down call :Inst:%d",\
		  (uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr);

  /*-------------------------------------------------------------------------
    Get the mode that we are currently operating in. Ensure that it is one
    of the 3GPP modes
  -------------------------------------------------------------------------*/
  if (!ds_bearer_cntxt_get_subs_id_from_bearer(bearer_context_p, &subs_id))
  {
    return retVal;
  }
  else
  {
    network_mode = ds3gpp_get_current_network_mode(subs_id);
  }

   if( !DS_3GPP_IS_3GPP_CURRENT_MODE(network_mode))
  {
    DS_3GPP_MSG1_ERROR("DS 3GPP Invalid network_mode:%d",network_mode);
    return retVal;
  }

  per_subs_info_hndl = 
    ds_bearer_cntxt_get_per_subs_info_hndl(subs_id);

  if (!ds_bearer_cntxt_validate_per_subs_info_hndl
        (per_subs_info_hndl))
  {
    DS_3GPP_MSG1_ERROR("Bearer Cntxt per subs info handle is invalid"
                       "for subs id",subs_id);
    return retVal;
  }

  /*-------------------------------------------------------------------------
    Memset the mode specific info that is to be sent to CM to hang up call
  -------------------------------------------------------------------------*/
  memset(mode_sp_info,0,sizeof(ds_3gpp_pdn_call_info_type));

  /*-------------------------------------------------------------------------
    Intialize the mt_flag to FALSE
  -------------------------------------------------------------------------*/
  mode_sp_info->mt_flag = FALSE;

  /*-------------------------------------------------------------------------
    Save the call instance to pass to ds3g_msh_hangup_call(). We don't use
    this at present.
  -------------------------------------------------------------------------*/
  mode_sp_info->call_instance = (uint32)bearer_context_p->ds_bearer_context_dyn_p
    ->client_data_ptr;

  /*-------------------------------------------------------------------------
   * Regardless of whether there is uplink data to be sent or not, we unset
   * the bit field in the bit mask that is used to determine if we should
   * send re-orig request on timer expiry
  -------------------------------------------------------------------------*/
  DS_BEARER_CNTXT_UNSET_BITMASK (per_subs_info_hndl, bearer_context_p);

  /*-----------------------------------------------------------------------
    Reset the rab_reestab_retry counter for this bearer.
  -----------------------------------------------------------------------*/
  DS_BEARER_CNTXT_RESET_RAB_REESTAB_RETRY_CNT(bearer_context_p);

  retVal = DS3G_SUCCESS;
  return retVal;
}
/*===========================================================================
FUNCTION DS_BEARER_CNTXT_TEARDOWN

DESCRIPTION
  This function is used to bring down a call on a bearer. The iface down cmd
  is processed in the PDN context and in turn calls this function. 

PARAMETERS
  bearer_context_p - pointer to the bearer on which call is to be ended
  info_ptr         - void pointer

DEPENDENCIES
  None.

RETURN VALUE
  DS3G_SUCCESS (TRUE) on Successful call hang up with CM
  DS3G_FAILURE (FALSE) on any failure

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_cntxt_tear_down(ds_bearer_context_s* bearer_context_p, void *info_ptr)
{
  boolean                    retVal = FALSE;
  ds_3gpp_pdn_call_info_type    mode_sp_info;   /* Information for a call    */

  ds_bearer_flow_manager_s   *bflow_manager_p = NULL;  
  ds_flow_context_s          *flow_context_p = NULL;
  ds_pdn_context_s           *pdn_context_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if(ds_bearer_cntx_validate_bearer_context(bearer_context_p) == FALSE)
  {
    ASSERT(0);
    return FALSE;
  }

  retVal = ds_bearer_cntxt_prepare_for_teardown(&mode_sp_info,bearer_context_p);

  if (!retVal)
  {
    DS_3GPP_MSG0_ERROR("ds_bearer_cntxt_prepare_for_teardown failed");
    return retVal;
  }

  /*----------------------------------------------------------------------------
     Reset retval for future usage
    ----------------------------------------------------------------------------*/
  retVal = FALSE;
  
  if (ds_bearer_cntxt_tear_down_call
      (
        bearer_context_p,
        &mode_sp_info,
        info_ptr
      ) == DS3G_FAILURE )
  {
    /*-----------------------------------------------------------------------
      The call has already ended. Do local cleanup
    -----------------------------------------------------------------------*/
    DS_3GPP_MSG1_HIGH("Call already ended Bearer Inst:%d",\
		    (uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr);
    retVal = TRUE;
  }
  else
  {
    if(bearer_context_p->ds_bearer_context_dyn_p->state == DS_BEARER_CONTEXT_STATE_DOWN)
    {
      /*-----------------------------------------------------------------------
        Do not move bearer to going down state if it already in down state, this 
        can be the default bearer which is in down state when SEC PDPs are up
       -----------------------------------------------------------------------*/
      DS_3GPP_MSG0_HIGH("Bearer is in down state already, not moving to going down");
      pdn_context_ptr = bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr;
      if (ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_ptr))
      {
        if (PS_PHYS_LINK_GET_STATE(&(bearer_context_p->phys_link)) == PHYS_LINK_DOWN)
        {
        ASSERT((pdn_context_ptr->ds_pdn_context_dyn_p->num_active_bearer_context) > 1);
      }
      else
      {
         /*Phys link not in down state, so this is not a QChat scenario*/
          DS_3GPP_MSG1_HIGH("Phys link state: %d,  do local clean up", 
                     PS_PHYS_LINK_GET_STATE(&(bearer_context_p->phys_link)));
          retVal = TRUE;
        }
      }
      else
      {
        DS_3GPP_MSG0_ERROR("PDN Context ptr is invalid");
      }
	  
    }
    else
    {
      DS_3GPP_MSG1_HIGH("Bearer state:%d", bearer_context_p->ds_bearer_context_dyn_p->state);
      bearer_context_p->ds_bearer_context_dyn_p->state = DS_BEARER_CONTEXT_STATE_GOING_DOWN;
    }
  }

  if (retVal)
  {
    DS_3GPP_MSG1_HIGH("Error in tearing down bearer, do local cleanup: Bearer Inst:%d",
             (uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr);

    if(bearer_context_p->ds_bearer_context_dyn_p->is_default)
    {
      ds_pdn_cntxt_unbind_default_flows(bearer_context_p->ds_bearer_context_dyn_p
                                        ->pdn_context_ptr);

      /*---------------------------------------------------------------------
        Disable signal if this is the last phys link. 
        Each enqueue func_ptr needs to be reset when bearer inst is freed.
        This needs to be done to avoid race conditions when phys_link_gone is
        posted and flow gets enabled on the phys_link leading to each_enq_f
        called when the bearer is getting freed..
       ---------------------------------------------------------------------*/
      ds_bearer_cntxt_free_bearer_inst(bearer_context_p);
      ps_disable_sig(bearer_context_p->rx_sig);

      ps_phys_link_enable_flow(&(bearer_context_p->phys_link),
                               DS_FLOW_UMTS_MASK);
      ps_phys_link_gone_ind(&(bearer_context_p->phys_link));
    }
    else
    {
      //VRK:TODO make subfunction to call from phys_link_up_cmd_hdlr as well as tear_down
      //This code is executed only for UMTS
      DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      /*-------------------------------------------------------------------------
        Update precedence_bit_mask and Release flow _context
        Bearer_down will release the bflowmgr and flowMgr          
      -------------------------------------------------------------------------*/    

      bflow_manager_p=ds_bearer_flow_manager_find_flow_manager(bearer_context_p);
      if(ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
      {
        /*-----------------------------------------------------------------------
          There should be only one pending flow context in the bearer flow 
          manager list.
        -----------------------------------------------------------------------*/
        if(list_size(&(bflow_manager_p->ds_bearer_flow_manager_dyn_p->flow_context)) 
           != 1)
        {
          DS_3GPP_MSG0_ERROR("Found more than one pending flow context");                        
        }
        flow_context_p = list_peek_front(&(bflow_manager_p->ds_bearer_flow_manager_dyn_p
                                           ->flow_context));          

      }
      else
      {
        DS_3GPP_MSG0_ERROR("Cound not find associated bearer flow manager to clean up");      
      }
      if(flow_context_p != NULL)
      {
        if(ds_flow_manager_update_precedence_bit_mask(
                          (ds_pdn_context_s*)bearer_context_p->
                          ds_bearer_context_dyn_p->pdn_context_ptr,
                                     flow_context_p,FALSE,FALSE,TRUE)==FALSE)
        {
          DS_3GPP_MSG0_ERROR("Error in clearing precedence bit mask");
        }     
      }
      else
      {
        DS_3GPP_MSG0_ERROR("Cound not find a pending flow context");      
      }               
      ds_flow_context_release_flow_context(flow_context_p);
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    }//dedicated bearer    
    
  }
  return (!retVal);
}/* ds_bearer_cntxt_tearDown() */

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_GET_BEARER_INST_TO_REORIG

DESCRIPTION
  This function is used to get a bearer instance on which reorigination is
  to be requested. This is used in the timer_expired command processing to
  get a bearer to work on.

  This uses the bit mask to determine a bearer that is still waiting on RAB
  reestablishment and returns it to the caller.

PARAMETERS
  Subs Id: Subscription Id

DEPENDENCIES
  Depends on whether the uplink_data_bitmask is null or non-null

RETURN VALUE
  Pointer to the bearer context awaiting RAB reestab if bitmask is non null
  NULL if bitmask is NULL

SIDE EFFECTS
  None.

===========================================================================*/
ds_bearer_context_s* ds_bearer_cntxt_get_bearer_inst_to_reorig
(
  sys_modem_as_id_e_type subs_id
)
{
  uint32               index;
  boolean              found_bearer_instance = FALSE;
  ds_bearer_context_s* bearer_context_p;
  ds_bearer_cntxt_per_subs_info_type  *per_subs_info_hndl = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  per_subs_info_hndl = 
    ds_bearer_cntxt_get_per_subs_info_hndl(subs_id);

  if (!ds_bearer_cntxt_validate_per_subs_info_hndl
       (per_subs_info_hndl))
  {
    DS_3GPP_MSG1_ERROR("Bearer Cntxt per subs info handle is invalid for"
                       "subs id",subs_id);
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Loop through all the bearers. Query the bit field corresponding to this 
    bearer in the uplink_data_bitmask. If the field is set, then return this
    bearer to the calling function.
  -------------------------------------------------------------------------*/
  for(index=0; index < DS_3GPP_MAX_BEARER_CONTEXT; index++)
  {
    bearer_context_p = ds_bearer_cntxt_get_instance_ptr(index);
    
    if (!ds_bearer_cntx_validate_bearer_subscription_internal 
           (bearer_context_p, subs_id))
    {
      continue;
    }
	
    found_bearer_instance = 
      DS_BEARER_CNTXT_QUERY_BITMASK (per_subs_info_hndl, bearer_context_p);

    if (found_bearer_instance == TRUE)
        break;
    }
  	
  return(index < DS_3GPP_MAX_BEARER_CONTEXT? bearer_context_p: NULL);
} /* ds_bearer_cntxt_get_bearer_inst_to_reorig() */

/*=============================================================================
FUNCTION ds_bearer_cntxt_proc_purgecmdcbhdlr

DESCRIPTION

PARAMETERS
  ds3gpp_purge_cnf_ev which consist of
  cnf_status    - true if if purge status is RLC_PURGE_REQ_SUCCESS(success )             
                   false otherwise
  bearer_context_p - bearer context pointer
DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
#ifdef FEATURE_WCDMA
void ds_bearer_cntxt_proc_purgecmdcbhdlr
(
  ds_bearer_context_purge_cnf_ev_type *ds3gpp_purge_cnf_ev
)
{
  ds_bearer_context_s    *bearer_context_p = NULL; 
  sys_modem_as_id_e_type  subs_id = SYS_MODEM_AS_ID_NONE;
  /*--------------------------------------------------------------------------*/

  if (ds3gpp_purge_cnf_ev == NULL)
  {
    DS_3GPP_MSG0_ERROR("ds3gpp_purge_cnf_ev is NULL");
    return;
  }

  bearer_context_p = ds3gpp_purge_cnf_ev->bearer_context_p;

  if(!ds_bearer_cntxt_get_subs_id_from_bearer(bearer_context_p, &subs_id))
  {
    return;
  }

  DS_BEARER_CTXT_VF_CALL
  (
    purgecmdcbhdr, 
    subs_id,
    ds3gpp_purge_cnf_ev
  );
} /* ds_bearer_cntxt_proc_purgecmdcbhdlr */

#endif /* FEATURE_WCDMA */

/*=============================================================================
FUNCTION ds_bearer_cntxt_clr_pending_calls

DESCRIPTION
    This function will clear the calls in Pending Redial state if conditions are met.

PARAMETERS
  client_data_ptr: Subs Id passed as Client Data Ptr

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/

void ds_bearer_cntxt_clr_pending_calls
(
  void* client_data_ptr
)
{
  ds_bearer_context_s     * bearer_context_p= NULL;
  sys_sys_mode_e_type       new_network_mode;
  uint8                     bearer_cntx_inst;
  ds_bearer_cntxt_per_subs_info_type  *per_subs_info_hndl = NULL;
  sys_modem_as_id_e_type              *subs_id_p = 
                                        (sys_modem_as_id_e_type*)client_data_ptr;
  /*---------------------------------------------------------------------------*/

  if (subs_id_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("subs_id_p is NULL");
    return;
  }

  per_subs_info_hndl = 
    ds_bearer_cntxt_get_per_subs_info_hndl(*subs_id_p);

  if (!ds_bearer_cntxt_validate_per_subs_info_hndl
        (per_subs_info_hndl))
  {
    DS_3GPP_MSG1_ERROR("Bearer Cntxt per subs info handle is invalid for"
                       "subs id",*subs_id_p);
    return;
  }

  /* Clearing Pending Redial Timer as LTE is Deactivated  */
   if(per_subs_info_hndl->pending_redial_tmr_running == TRUE)
   {
      DS_3GPP_MSG0_HIGH("Clearing Pending Redial Timer as cleaningup"
                        "of Pending Calls");  
     (void)rex_clr_timer(&(per_subs_info_hndl->pending_redial_tmr));  
      per_subs_info_hndl->pending_redial_tmr_running = FALSE;		 
   }

  new_network_mode = ds3gpp_get_current_network_mode(*subs_id_p); 
   
  for( bearer_cntx_inst=0;\
      bearer_cntx_inst<DS_3GPP_MAX_BEARER_CONTEXT;\
      bearer_cntx_inst++ )
  {
    bearer_context_p = ds_bearer_cntxt_get_instance_ptr(bearer_cntx_inst);

    if (!ds_bearer_cntxt_validate_bearer_subscription
         (bearer_context_p, *subs_id_p))
    {
      continue;
    }
    else
    {
      new_network_mode = ds3gpp_get_current_network_mode(*subs_id_p);
    }

    /*-----------------------------------------------------------------------
     Cleanup PEND_REDIAL bearers as lower layers wouldn't send CNTX_TRSFER
     when registration fails on a RAT. Example:
     1. UE on W, originates a call, lower layers camp on LTE and reject this
        call with GW_L_RAT_CHANGE. Bearer state: PEND_REDIAL.
     2. CNTXT_TRSFER to LTE. TAU doesn't succeed and registration fails.
     3. UE comes back to W, no CNTX_TRSFER in this case.
     4. Bearer remains in PEND_REDIAL till app initiates call teardown.
     -------------------------------------------------------------------------*/
    
     ds_bearer_cntxt_cleanup_pending_calls(bearer_context_p, new_network_mode);
  }
   
}/* ds_bearer_cntxt_clr_pending_calls */
/*===========================================================================
FUNCTION  ds_bearer_cntxt_proc_dereg_dpm_cmd

DESCRIPTION
  This function is called when we process the command to deregister with
  dpm as part of context transfer.

PARAMETERS   
  *cmd_ptr - ds cmd ptr

DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
  
SIDE EFFECTS 
  None.
===========================================================================*/
void ds_bearer_cntxt_proc_dereg_dpm_cmd
(
  const ds_cmd_type *cmd_ptr
)
{
  ds_bearer_context_s  *bearer_context_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DS_3GPP_MSG0_ERROR("NULL ptr passed, return");
    return;
  }

  bearer_context_p =(ds_bearer_context_s*)(*((void**)cmd_ptr->cmd_payload_ptr));
  
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }

  DS_BEARER_CTXT_VF_CALL_NO_ARG_PER_BEARER
  (   
    cntxt_transfer_ind_setup,
    bearer_context_p
  );

  if(bearer_context_p->ds_bearer_context_dyn_p->is_registered_with_dpm)
  {
    DS_3GPP_MSG3_HIGH("Context Transfer Dpm Reregistration: %d",\
                        bearer_context_p->phys_link.bearer_id,0,0); 
    ds_bearer_cntx_reregister_dpm(bearer_context_p);
  }
}

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_PROCESSPSCMD

DESCRIPTION
  This function processes the bearer context specific PS commands received 
  by the DS task

PARAMETERS
  IN
    const ds_cmd_type* - Pointer to the command buffer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_bearer_cntxt_process_ps_cmd
(
  const ds_cmd_type *cmd_ptr                         /* Pointer to Command */
)
{
  ds_3gpp_rrc_pch_ind_proc_cmd_type   *rrc_pch_ind_proc_ptr = NULL;
  ds_3gpp_ul_rab_info_type            *ul_rab_info_ptr = NULL;
  ds_3gpp_pdn_call_info_type          *call_info_ptr = NULL;
#ifdef FEATURE_DATA_LTE
  ds3g_call_info_cmd_type             *call_info_cmd_ptr = NULL;
#endif /* FEATURE_DATA_LTE */
#ifdef FEATURE_DATA_WCDMA_PS
  ds_bearer_context_purge_cnf_ev_type *purge_cnf_ev_ptr = NULL;
#endif /* FEATURE_DATA_WCDMA_PS */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(cmd_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("NULL cmd_ptr passed, return");
    return;
  }

  /*-------------------------------------------------------------------------
    Invoke the appropriate function to process the command, based on the
    command id.
  -------------------------------------------------------------------------*/
  DS_3GPP_MSG1_MED("ds_bearer_cntxt_processPSCmd: received %d", cmd_ptr->hdr.cmd_id );
  switch(cmd_ptr->hdr.cmd_id)
  {
  
    case DS_CMD_3GPP_PHYS_LINK_UP:
  
      /*---------------------------------------------------------------------
        Command to bring up the Traffic Channel
        the parameter is actually bearer_cntx_inst and not call_instance
        Called only for dormancy reoriginations.
      ---------------------------------------------------------------------*/
      if (cmd_ptr->cmd_payload_ptr != NULL)
      {
        call_info_ptr = 
          (ds_3gpp_pdn_call_info_type*)cmd_ptr->cmd_payload_ptr;
       
        if(call_info_ptr->call_instance >= DS_3GPP_MAX_BEARER_CONTEXT)
      {
        DS_3GPP_MSG1_ERROR("Invalid bearer_cntx_inst %d",
                             call_info_ptr->call_instance);
        ASSERT(0);
        return;
      }
        ds_bearer_cntxt_phys_link_up_cmd_hdlr(call_info_ptr->call_instance,
                                              call_info_ptr->info_ptr);
      break;
      }

    case DS_CMD_3GPP_PHYS_LINK_DOWN:

      /*---------------------------------------------------------------------
        Command to tear down the Traffic Channel
        the parameter is actually bearer_cntx_inst and not call_instance
        Still unclear as to who would be the clients invoking this command.
      ---------------------------------------------------------------------*/
      if (cmd_ptr->cmd_payload_ptr != NULL)
      {
        call_info_ptr=(ds_3gpp_pdn_call_info_type*)cmd_ptr->cmd_payload_ptr;

        if(call_info_ptr->call_instance >= DS_3GPP_MAX_BEARER_CONTEXT)
      {
        DS_3GPP_MSG1_ERROR("Invalid bearer_cntx_inst %d",
                             call_info_ptr->call_instance);
        ASSERT(0);
        return;
      }
        ds_bearer_cntxt_phys_link_down_cmd_hdlr(call_info_ptr->call_instance,
                                                call_info_ptr->info_ptr);
      }
      break;    

    case DS_CMD_3GPP_PHYS_LINK_FLOW_ENABLE:
      /*---------------------------------------------------------------------
      Flow Control the phys link based on the posted command.
      ---------------------------------------------------------------------*/
      ds_bearer_cntxt_phys_link_flow_cmd_hdlr(cmd_ptr);
      
      break;

    case DS_CMD_3GPP_DORM_REORIG_TMR_EXP:
      ds_bearer_cntxt_proc_reorig_dormtmr_exp_cmd(cmd_ptr->cmd_payload_ptr);
      break;
      
    case DS_CMD_3GPP_DPM_DEREG:
      DS_3GPP_MSG3_MED("ds3gpp_processPSCmd: DS3GPP_DEREG_DPM",\
               0,0,0);
      ds_bearer_cntxt_proc_dereg_dpm_cmd(cmd_ptr);
      break;

    case DS_CMD_3GPP_DORM_REORIG:
      ds_bearer_cntxt_proc_dorm_reorig_cmd(cmd_ptr);
      break;

    case DS_CMD_3GPP_DSM_MEM_CTRL_EV:
      ds3gpp_enter_global_crit_section();
      ds_bearer_cntxt_set_mem_ctrl_ev_posted_flag(FALSE);
      ds3gpp_leave_global_crit_section();
      ds_bearer_cntxt_flush_ps_l2_watermarks();
      break;
    
    case DS_CMD_3GPP_RRC_PCH_IND_PROC:
      if( cmd_ptr->cmd_payload_ptr != NULL)
      {
        rrc_pch_ind_proc_ptr
          = (ds_3gpp_rrc_pch_ind_proc_cmd_type*)cmd_ptr->cmd_payload_ptr;
        ds_bearer_cntxt_proc_pch_cmd(rrc_pch_ind_proc_ptr->pch_event,
                                     rrc_pch_ind_proc_ptr->call_mode,
				     rrc_pch_ind_proc_ptr->subs_id);
      }
      break;

    case DS_CMD_3GPP_TD_CONFIG_UL_DATA_RATE:
      if( cmd_ptr->cmd_payload_ptr != NULL )
      {
        ul_rab_info_ptr = (ds_3gpp_ul_rab_info_type*)cmd_ptr->cmd_payload_ptr;
         ds_bearer_cntxt_td_proc_cmd(ul_rab_info_ptr->rab_id,
                                     ul_rab_info_ptr->ul_data_rate,
                                     ul_rab_info_ptr->subs_id);
      }
      break;

#ifdef FEATURE_WCDMA
    case DS_CMD_3GPP_RLC_PURGE_CNF:
      if (cmd_ptr->cmd_payload_ptr != NULL)
      {
        purge_cnf_ev_ptr = 
          (ds_bearer_context_purge_cnf_ev_type*)cmd_ptr->cmd_payload_ptr;

      ds_bearer_cntxt_proc_purgecmdcbhdlr(
                  (ds_bearer_context_purge_cnf_ev_type*)(purge_cnf_ev_ptr)); 
      }
	  break;
#endif /* FEATURE_WCDMA */

    case DS_CMD_3GPP_CALL_PENDING_CLR:
      DS_3GPP_MSG0_MED("ds3gpp_processPSCmd: Received DS_3GPP_CLR_PENDING_CALLS");
      ds_bearer_cntxt_clr_pending_calls (cmd_ptr->cmd_payload_ptr);
      break;
	  
#ifdef FEATURE_DATA_LTE
    case DS_CMD_3GPP_CALL_END_MODE_CHG_LTE:
      DS_3GPP_MSG0_MED("ds3gpp_processPSCmd: Received DS_CMD_3GPP_CALL_END_MODE_CHG_LTE");
      call_info_cmd_ptr = (ds3g_call_info_cmd_type*)(cmd_ptr->cmd_payload_ptr);
      if (NULL != call_info_cmd_ptr)
      {
        ds_bearer_cntxt_resend_call_end(call_info_cmd_ptr->call_id);
      }
      else
      {
        DS_3GPP_MSG0_ERROR("Cmd payload ptr is NULL");
      }
      break;  
#endif /* FEATURE_DATA_LTE */

    default:
      DATA_MSG1_ERROR("Unknown cmd to UMTS mode Sp. hdlr %d", cmd_ptr->hdr.cmd_id);
      break;

  } /* switch( cmd_id ) */

} /* ds_bearer_cntxt_processPSCmd() */

#ifdef FEATURE_DATA_IMS
/*===========================================================================
FUNCTION DS_BEARER_CNTXT_ISIMCNCALLUP

DESCRIPTION
  This fn is used during call orig to find out if there is already a call
  with the IM_CN_subsystem flag set to TRUE

PARAMETERS
  subs_id: Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the IMCN call is up, False otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_cntxt_is_IMCN_call_up
(
  sys_modem_as_id_e_type subs_id
)
{
  int                   bearer_index;
  ds_bearer_context_s*  bearer_context_p;
  boolean               im_cn_flag = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Check the bearer cntx table to see if there if any call has the IM CN flag 
    set.
  -------------------------------------------------------------------------*/                                          
  for(bearer_index=0;bearer_index < DS_3GPP_MAX_BEARER_CONTEXT;bearer_index++)
  {  

    bearer_context_p = ds_bearer_cntxt_get_instance_ptr(bearer_index); 
    
     if(!ds_bearer_cntx_validate_bearer_subscription_internal(bearer_context_p,subs_id))
     {
       continue;
     }
    /*-----------------------------------------------------------------------
      If a call is in UP state and IM CN flag is set, set im_cn_flag
      = TRUE.
    -----------------------------------------------------------------------*/
    if( (bearer_context_p->ds_bearer_context_dyn_p->state == 
         DS_BEARER_CONTEXT_STATE_UP ||
         bearer_context_p->ds_bearer_context_dyn_p->state == 
         DS_BEARER_CONTEXT_STATE_UP_DORMANT ||
         bearer_context_p->ds_bearer_context_dyn_p->state == 
         DS_BEARER_CONTEXT_STATE_UP_DORMANT_REESTAB ) && 
        (bearer_context_p->ds_bearer_context_dyn_p->im_cn_subsystem_flag == TRUE) )
    {
      DS_3GPP_MSG1_HIGH("Call with IM CN flag already up: bearer cntx inst:%d",
             bearer_index);
      im_cn_flag = TRUE;
      break;
    }
    else if((ds_3gpp_pdn_cntx_validate_pdn_context(bearer_context_p->
             ds_bearer_context_dyn_p->pdn_context_ptr)) && 
             (bearer_context_p->ds_bearer_context_dyn_p->state 
              == DS_BEARER_CONTEXT_STATE_COMING_UP) &&
      (((ds_pdn_context_s*)(bearer_context_p->
       ds_bearer_context_dyn_p->pdn_context_ptr))->ds_pdn_context_dyn_p
       -> pdp_profile.im_cn_flag == TRUE) )
    {
      /*-----------------------------------------------------------------------
        Check the profile to see if there if any call in pending state
        with the IM CN flag.
      
        If the call is in coming up state, that means a call with IM CN
        flag is pending.
      -----------------------------------------------------------------------*/                                          
      
        DS_3GPP_MSG1_HIGH("Call with IM CN flag pending: bearer cntx inst:%d",
          bearer_index);
        im_cn_flag = TRUE;
        break;      
    }
  } /* for .. */

  DS_3GPP_MSG1_HIGH("In ds_bearer_cntxt_isIMCNCallUp: returning %d", im_cn_flag);

  return im_cn_flag;
} /* ds_bearer_cntxt_isIMCNCallUp() */

/*===========================================================================
FUNCTION  DS_BEARER_CNTXT_GET_IMCN_SUBSYSTEM_FLAG_FOR_IFACE

DESCRIPTION
   This function returns the value of IM CN subsystem flag for an iface. It
   looks up the call table to get the IM CN Subsystem flag.
   
PARAMETERS   :
  *iface_ptr     - Pointer to iface on which query is made
  im_cn_status   - boolean pointer to return the status of IM CN Flag.
  *ps_errno      - Error if any
  
DEPENDENCIES
  None.

RETURN VALUE
   0 - If the IM CN flag is successfully retrieved.
   -1 - If the iface is not valid.

SIDE EFFECTS
  None.

===========================================================================*/
int ds_bearer_cntxt_get_IMCN_subsystem_flag_for_iface
(
  const ps_iface_type      *iface_ptr,
  boolean                  *im_cn_status,
  sint15                   *ps_errno
)
{
  ds_pdn_context_s     *pdn_cntx_ptr = NULL;
  ds_bearer_context_s  *bearer_context_ptr = NULL;

  if( (iface_ptr == NULL) || (im_cn_status == NULL) )
  {
    DS_3GPP_MSG3_ERROR("Invalid arguments. iface_ptr = 0x%x, im_cn_status= 0x%x",
                       iface_ptr, im_cn_status, 0);
    return -1;
  }

  pdn_cntx_ptr = (ds_pdn_context_s*)(((ds_3gpp_iface_s*)
                         (iface_ptr->client_data_ptr))->client_data_ptr);

  if (!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_ptr))
  {
    return -1;
  }

  bearer_context_ptr = 
         (ds_bearer_context_s*)(pdn_cntx_ptr->ds_pdn_context_dyn_p->
                                 def_bearer_context_ptr);
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_ptr))
  {
    return -1;
  }

  *im_cn_status = bearer_context_ptr->ds_bearer_context_dyn_p->im_cn_subsystem_flag;

  return 0;
}/* ds_bearer_cntxt_get_IMCN_subsystem_flag_for_iface */

#endif /* FEATURE_DATA_IMS */

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_ALLOC

DESCRIPTION
  This function marks a bearer context to be in use by allocating a CM call
  id to it. There is no other processing done here. This is mainly used for
  MT calls in UMTS and in the default bearer activation in EPS.

PARAMETERS
  IN
    ds_bearer_context_s* - Pointer to the bearer that has to be allocated.
    cm_call_id_type      - This is the call id that is to be stored in bearer

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_bearer_cntxt_alloc
(
  ds_bearer_context_s* bearer_context_p,
  cm_call_id_type      cm_call_id
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }

  bearer_context_p->ds_bearer_context_dyn_p->cm_call_id = cm_call_id;
} /* ds_bearer_cntxt_alloc() */

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_INCOMINGCALL

DESCRIPTION
  This function results in a transition of the bearer context state.
  This is called from 3gpp handler if USER_ALERT is given to it by
  PDN context.

PARAMETERS
  IN
    ds_bearer_context_s* - The bearer whose state is to be changed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_bearer_cntxt_incoming_call(ds_bearer_context_s* bearer_context_p)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*
    Change the state of the bearer to COMING_UP
  */
  DS_3GPP_MSG0_HIGH("Changing bearer context state to coming UP");

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }

  bearer_context_p->ds_bearer_context_dyn_p->state = 
    DS_BEARER_CONTEXT_STATE_COMING_UP;
} /* ds_bearer_cntxt_incomingCall() */

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_ANSWERCALL

DESCRIPTION
  This function is used to answer an incoming call. This is the point of
  convergence of the two different paths that an MT call could take (Auto
  answer and Application answer).

PARAMETERS
  IN
    ds_bearer_context_s* - The bearer on which the incoming call is to be
                           answered.

DEPENDENCIES
  None.

RETURN VALUE
  DS3G_SUCCESS - if the call was initiated successfully or if hang up was
                 initiated successfully on a failed call initiation
  DS3G_FAILURE - if the hang up was unsuccessful. Local clean up happens in
                 this case.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_cntxt_answer_call(ds_bearer_context_s* bearer_context_p)
{
  ds_3gpp_pdn_call_info_type  call_info;
  boolean                  retVal = DS3G_SUCCESS; 
  ds_3gpp_down_reason_t    net_down_reason;
  sys_modem_as_id_e_type      subs_id; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_3GPP_MSG0_HIGH("Call origination due to MT request");
  
  if(!ds_bearer_cntxt_get_subs_id_from_bearer(bearer_context_p, &subs_id))
  {
    return FALSE;
  }
  
  call_info.mt_flag = TRUE;
  /*-------------------------------------------------------------------
    If answer call failed, then need to send MT reject
  -------------------------------------------------------------------*/
  
  if (ds3g_msh_answer_call_ex
       (bearer_context_p->ds_bearer_context_dyn_p->cm_call_id,
        subs_id) == DS3G_FAILURE)
  {
    DS_3GPP_MSG2_ERROR("Error in answering call, Bearer cntxt: %d call_id %d",\
		    (uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr,
		    bearer_context_p->ds_bearer_context_dyn_p->cm_call_id);

    /* Enable flow on the phys link*/
    ps_phys_link_enable_flow(&(bearer_context_p->phys_link),
                           DS_FLOW_UMTS_MASK);
    
    /* Send the phys link gone indication */
    ps_phys_link_gone_ind(&(bearer_context_p->phys_link));
    net_down_reason.reason.ps_iface_down_reason = PS_NET_DOWN_REASON_NOT_SPECIFIED;
    net_down_reason.reason_T = DS_3GPP_PS_IFACE_NET_DOWN_REASON_T;

    /* answer failed, call bearer down to clean up*/
    if (bearer_context_p->ds_bearer_context_dyn_p
                        ->pdn_context_cb_tbl_ptr->bearer_down_ind_cb != NULL)
    {
      bearer_context_p->ds_bearer_context_dyn_p
                        ->pdn_context_cb_tbl_ptr->bearer_down_ind_cb(
                    bearer_context_p->ds_bearer_context_dyn_p
                        ->pdn_context_ptr,\
                    bearer_context_p,
                    net_down_reason,
                    FALSE);
    }
    if (ds3g_msh_hangup_call_ex(bearer_context_p->ds_bearer_context_dyn_p->
                               cm_call_id, &call_info, subs_id)== DS3G_FAILURE)
    {
      /*-----------------------------------------------------------------------
        The call has already ended.
      -----------------------------------------------------------------------*/
      DS_3GPP_MSG1_HIGH("Call already ended call_id :%d",\
                           bearer_context_p->ds_bearer_context_dyn_p
                        ->cm_call_id);

      /*Do a local clean up*/
      ds_bearer_cntxt_free_bearer_inst(bearer_context_p);

      retVal = DS3G_FAILURE;
    }
    else
    {
       bearer_context_p->ds_bearer_context_dyn_p->state = 
         DS_BEARER_CONTEXT_STATE_GOING_DOWN;
    }
  }
  return retVal;

} /* ds_bearer_cntxt_answerCall() */

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_ALLOCATE_NEW_BEARER

DESCRIPTION
  This function is used to return a bearer from the pool of available bearer
  contexts. The allocation and deallocation details are completely hidden from
  the caller. We could change the implementation of this function at a later
  point and make bearer allocations completely dynamic instead of allocating
  a free bearer from the pool of statically created bearers.

PARAMETERS
  Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  ds_bearer_context_s*

SIDE EFFECTS
  None.
===========================================================================*/
ds_bearer_context_s* ds_bearer_cntxt_allocate_new_bearer
(
  sys_modem_as_id_e_type subs_id
)
{
  int                  loop_index;
  ds_bearer_context_s* bearer_context_p = NULL;
  sys_sys_mode_e_type    mode = 
                           ds3gpp_get_current_network_mode(subs_id);
#if defined (FEATURE_DATA_WCDMA_PS) || defined (FEATURE_GSM_GPRS)
  sys_sys_mode_e_type  umts_network_mode = SYS_SYS_MODE_NONE;
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#if defined FEATURE_WCDMA
  umts_network_mode = SYS_SYS_MODE_WCDMA;
#elif defined FEATURE_TDSCDMA
  umts_network_mode =  SYS_SYS_MODE_TDS;
#elif defined FEATURE_GSM_GPRS
  umts_network_mode = SYS_SYS_MODE_GSM;
#endif

  for (loop_index = 0; loop_index < DS_3GPP_MAX_BEARER_CONTEXT; loop_index++)
  {
    bearer_context_p = ds_bearer_cntxt_get_instance_ptr(loop_index);

    if(!ds_bearer_cntx_validate_bearer_context_internal(bearer_context_p))
    {
      if(!ds_bearer_cntx_alloc_dyn_mem(loop_index, subs_id))
      {
        DS_3GPP_MSG0_ERROR("Failure to allocate dynamic memory, Heap failure");
        return NULL;
      }

      /*Update the bearer context mode with the global ds_3gpp_sys_mode*/
      bearer_context_p->ds_bearer_context_dyn_p->call_mode = 
        mode;
      bearer_context_p->ds_bearer_context_dyn_p->cm_call_id = 
        CM_CALL_ID_INVALID;
      bearer_context_p->ds_bearer_context_dyn_p->state = 
        DS_BEARER_CONTEXT_STATE_DOWN;
      bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr = 
        (void*)loop_index;
      /* Initialize as non-default */
      bearer_context_p->ds_bearer_context_dyn_p->is_default = FALSE;
      /* Initialize SDF ID to be invalid */
      bearer_context_p->ds_bearer_context_dyn_p->sdf_id = 0xFFFF;
      /* If NW provided TI for GW, the bearer can be transfered to GW RAT. */
      bearer_context_p->ds_bearer_context_dyn_p->is_transferable_to_GW = TRUE;        
      
      bearer_context_p->phys_link.client_data_ptr =\
           bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr;             

      bearer_context_p->ds_bearer_context_dyn_p->
        ul_registered_with_ltepdcp = FALSE;              

      /* Initialize call_mode to 3GPP sys mode */
     /*---------------------------------------------------------------------
        Initialize the signal for the bearer context for data in the DL dir.
        We are adding support for having one signal per bearer context. This
        simplifies the design to look for data in the DL direction.

        However if QOS will have to be implemented between the different WMs
        present in a PDN context, we will have to revisit this section.
      ---------------------------------------------------------------------*/
      /*  Move rx sig to static */
      bearer_context_p->rx_sig = (ps_sig_enum_type)
      ((uint32)PS_3GPP_UM_RX_Q_SIGNAL_0 + (loop_index));
      
      /* Populate the PDN context call back pointers table */
      bearer_context_p->ds_bearer_context_dyn_p->pdn_context_cb_tbl_ptr = \
            &bearer_context_notifier_cb_tbl;

      /*------------------------------------------------------------------------
        Set the initial values of the byte counters to zero
      ------------------------------------------------------------------------*/
      bearer_context_p->ds_bearer_context_dyn_p->tx_bytes = 0;
      bearer_context_p->ds_bearer_context_dyn_p->rx_bytes = 0;

      bearer_context_p->ds_bearer_context_dyn_p->wm_flush_state = 
                                        DS_BEARER_CONTEXT_UM_WM_STATE_INVALID;
      bearer_context_p->ds_bearer_context_dyn_p->teardown_on_modify_ind = FALSE;

      /*------------------------------------------------------------------------
        Set bearer_down_reason_rab_failure to 0
      ------------------------------------------------------------------------*/
      bearer_context_p->ds_bearer_context_dyn_p->down_reason        = 
        PS_NET_DOWN_REASON_NOT_SPECIFIED;
      bearer_context_p->ds_bearer_context_dyn_p->down_reason_ps_eic = 
        PS_EIC_NOT_SPECIFIED;

      bearer_context_p->ds_bearer_context_dyn_p->subs_id = subs_id;

     /*-------------------------------------------------------------------------
      Associate the mode specific blob that is going to be populated to the 
      bearer context pointer.
     -------------------------------------------------------------------------*/
#ifdef FEATURE_LTE
     DS_BEARER_CTXT_VOBJ_REG
     (
       bearer_context_p, 
       SYS_SYS_MODE_LTE, 
       &eps_bearer_context_tbl[(uint32)bearer_context_p->ds_bearer_context_dyn_p->
       client_data_ptr] 
     );
      DS_3GPP_MSG0_HIGH("Allocating LTE eps bearer context");
#endif

#if defined (FEATURE_DATA_WCDMA_PS) || defined (FEATURE_GSM_GPRS)
      DS_BEARER_CTXT_VOBJ_REG
     (
       bearer_context_p, 
       umts_network_mode, 
       &umts_bearer_context_tbl[(uint32)bearer_context_p->ds_bearer_context_dyn_p->
       client_data_ptr] 
     );
#endif

     DS_3GPP_MSG0_HIGH("Allocating WCDMA umts bearer context");
     break;
    }
  }

  if (loop_index == DS_3GPP_MAX_BEARER_CONTEXT)
  {
    DS_3GPP_MSG0_ERROR("MAX Bearer contexts have been allocated");
    bearer_context_p = NULL;
  }

  return (void*)bearer_context_p;
} /* ds_bearer_cntxt_allocate_new_bearer */

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_RETURN_BEARER_TO_FREE_POOL

DESCRIPTION
  This function is used to return a bearer to the pool of available bearer
  contexts. This function just resets the in_use flag inside the bearer.

PARAMETERS
  IN
    bearer_context_p - pointer to the bearer to be marked as free.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_bearer_cntxt_return_bearer_to_free_pool
(
  ds_bearer_context_s* bearer_context_p
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }

  ds_bearer_cntx_free_dyn_mem(bearer_context_p);

}
/*===========================================================================
  FUNCTION DS_BEARER_CNTXT_REORIG_DORM

  DESCRIPTION
    This function is responsible for reoriginating the call from dormancy.
    This function ends up calling the DSMGR function for RAB reestab.
    The logic to check if there is a Rab-reestab request already pending is
    also present in this function.
  
  PARAMETERS   
   *bearer_context_p - Pointer to the bearer context that we are working on
   dsm_item_type*    - reorig packet pointer
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    DS3G_SUCCESS : Successful setup
    DS3G_FAILURE : Setup error
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
void ds_bearer_cntxt_reorig_dorm
(
  ds_bearer_context_s* bearer_context_p,
  dsm_item_type*       reorig_pkt_ptr
)
{
  cm_rab_reestab_params_s_type  rab_params;
  int16                         ps_errno = 0;
  ds_pdn_context_s*             pdn_cntx_p = NULL;
  sys_modem_as_id_e_type        subs_id = SYS_MODEM_AS_ID_NONE;

#ifdef FEATURE_DATA_LTE
  ds_eps_bearer_context_s             *eps_bearer_context_p = NULL;
  int                                 v4_iface_reestab_call_type = 
                                        SYS_LTE_REESTAB_CALLTYPE_NORMAL;
  int                                 v6_iface_reestab_call_type = 
                                        SYS_LTE_REESTAB_CALLTYPE_NORMAL;
#endif /* FEATURE_DATA_LTE */
  ds_bearer_cntxt_per_subs_info_type  *per_subs_info_hndl = NULL;
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if (!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }

  pdn_cntx_p = (ds_pdn_context_s*)
               bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr;
  if(! ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p,&subs_id))
  {
    return;
  }

  per_subs_info_hndl = 
    ds_bearer_cntxt_get_per_subs_info_hndl(subs_id);

  if (!ds_bearer_cntxt_validate_per_subs_info_hndl
        (per_subs_info_hndl))
  {
    DS_3GPP_MSG1_ERROR("Bearer Cntxt per subs info handle is invalid for"
                       "subs id",subs_id);
    return;
  }

  /*-------------------------------------------------------------------------
    RAB-reestablishment can happen on the bearer only if it is DORMANT.
    If the bearer is in any other state, then we do nothing for the reorig
    request.
  -------------------------------------------------------------------------*/
  if(bearer_context_p->ds_bearer_context_dyn_p->state == DS_BEARER_CONTEXT_STATE_UP_DORMANT)
  {
    if(ds3gpp_get_ps_data_suspend_flag(subs_id) == TRUE)
    {
      DS_3GPP_MSG3_HIGH("UL data bearer:%d data_suspend:1 sending data_available",\
         (uint32) bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr,0,0);

      if( (!ds3gpp_get_ps_data_available_flag(subs_id)) && 
           ds3gpp_is_ps_data_avail_ind_allowed(subs_id) )
      {
        DS_3GPP_MSG3_HIGH("UL data bearer:%d data_suspend:1 sending data_available",\
                          (uint32) bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr,0,0);
        ds3g_msh_ps_data_available_ind(subs_id);
        ds3gpp_set_ps_data_available_flag(TRUE, subs_id);
      }

      /*--------------------------------------------------------------
        Disable flow on the interface
      --------------------------------------------------------------*/
      ps_phys_link_disable_flow( &(bearer_context_p->phys_link),
                         DS_FLOW_UMTS_MASK);    
      return;
    }
    else
    {
      /*--------------------------------------------------------------------
        RAB reestablishment can happen only for wcdma, lte and tds modes.
        For all other modes,eg Gsm, we don't establish Rab and hence
        return.
      --------------------------------------------------------------------*/
      if(ds3gpp_get_current_network_mode(subs_id) == SYS_SYS_MODE_GSM)
       {
         DS_3GPP_MSG0_HIGH("ds_3gpp_sys_mode is GSM, returning");
         return;
       }
    } 
    DS_3GPP_MSG1_HIGH("Disabling flow and Reoriginating PDP Call on bearer ID: %d ",\
       (uint32) bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr);
    
    if (ds_3gpp_nv_get_max_rab_reestab_retry_cnt(subs_id) == RAB_REESTAB_RETRY_CNT_INVALID)
    {
    /*-------------------------------------------------------------------------
      Disable flow on this phys link so that no further packets can be queued.
      Similarly the socket app should also not write data into this phys link.
      This way, we prevent more reorig commands from being put into DS task.
    -------------------------------------------------------------------------*/ 
      DS_3GPP_MSG1_HIGH("Disabling flow and Reoriginating PDP Call on bearer ID: %d ",\
              (uint32) bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr);

      ps_phys_link_disable_flow(&(bearer_context_p->phys_link),
                              DS_FLOW_UMTS_MASK);
    }
    else
    {
       DS_3GPP_MSG1_HIGH("max_rab_reestab_req_retry_cnt: %d is not invalid"
			  " Not disabling phys_link_flow", 
			  ds_3gpp_nv_get_max_rab_reestab_retry_cnt(subs_id));
    }

    /*-------------------------------------------------------------------------
      In the uplink_data_bitmask, we set the bit field corresponding to this
      bearer. This way we prevent a reorig request from being sent out due to
      the same bearer.
    -------------------------------------------------------------------------*/
    DS_BEARER_CNTXT_SET_BITMASK(per_subs_info_hndl, bearer_context_p);
    
    /*-------------------------------------------------------------------------
      If the timer to re-establish RABs is already running or if a service
      request to reestab rab has already been sent out, then we do nothing.
      We return from this function.
    -------------------------------------------------------------------------*/
    if( (per_subs_info_hndl->service_request_pending_flag == TRUE ) || 
        (per_subs_info_hndl->timer_running == TRUE)  )
    {
      DS_3GPP_MSG2_LOW("Rab Re-est.req pend: %d or Timer_running: %d.Ignoring reorig.",\
                         per_subs_info_hndl->service_request_pending_flag,
                        per_subs_info_hndl->timer_running);
      return;
    }
    
    /*-------------------------------------------------------------------------
      Set the state of the bearer to RAB_REESTAB.
    -------------------------------------------------------------------------*/

    bearer_context_p->ds_bearer_context_dyn_p->state = DS_BEARER_CONTEXT_STATE_UP_DORMANT_REESTAB;

    /*--------------------------------------------------------------------- 
     Set Phys link to COMING UP state
     --------------------------------------------------------------------*/
    if( ps_phys_link_up_cmd(&(bearer_context_p->phys_link), 
                                             &ps_errno, NULL) != 0)
    {
       DS_3GPP_MSG1_HIGH("phys_link_up_cmd failed errno: %d",
                          ps_errno);
    }
    
    /*-------------------------------------------------------------------------
      Clear the RAB reestablish timer. This will be started again either in
      rab_re_est_ind or in rab_re_est_rej handler or in the registered timer
      call back
    -------------------------------------------------------------------------*/

    DS_BEARER_CNTXT_CLR_TIMER (per_subs_info_hndl);

    /*-------------------------------------------------------------------------
      Initialize rab_params.
    -------------------------------------------------------------------------*/
    memset(&rab_params,0, sizeof(cm_rab_reestab_params_s_type));
#ifdef FEATURE_DATA_LTE
    if (bearer_context_p->ds_bearer_context_dyn_p->call_mode == SYS_SYS_MODE_LTE)
    {
      eps_bearer_context_p = (ds_eps_bearer_context_s *)
        DS_BEARER_CTXT_VOBJ(bearer_context_p, SYS_SYS_MODE_LTE);

      if (!ds_eps_bearer_cntxt_validate_bearer_cntxt(eps_bearer_context_p))
      {
        return;
      }

      rab_params.sys_mode = SYS_SYS_MODE_LTE;
      rab_params.rab_reestab_params.cm_lte_rab_reestab_params.eps_bearer_id = 
                   eps_bearer_context_p->ds_eps_bearer_context_dyn_p->eps_bearer_id;

      /*--------------------------------------------------------------------- 
        Determine RAB Re-establishment call type
        ---------------------------------------------------------------------*/
      v4_iface_reestab_call_type = pdn_cntx_p->ds_pdn_context_dyn_p->
                                  rab_reestab_call_type[DS_3GPP_PDN_CONTEXT_IPV4_IFACE]; 
      v6_iface_reestab_call_type = pdn_cntx_p->ds_pdn_context_dyn_p->
                                  rab_reestab_call_type[DS_3GPP_PDN_CONTEXT_IPV6_IFACE]; 
      /*--------------------------------------------------------------------- 
        If the RE-Estab Calltype is VOLTE in atleast one of the Ifaces then
        request for Re-Estab Call type as VOLTE
        ---------------------------------------------------------------------*/
      if ((v4_iface_reestab_call_type == PS_IFACE_3GPP_LTE_REESTAB_CALLTYPE_VOLTE)||
          (v6_iface_reestab_call_type == PS_IFACE_3GPP_LTE_REESTAB_CALLTYPE_VOLTE))
      {
        rab_params.sys_lte_reestab_calltype = SYS_LTE_REESTAB_CALLTYPE_VOLTE;
      }
      else
      {
        rab_params.sys_lte_reestab_calltype = SYS_LTE_REESTAB_CALLTYPE_NORMAL;
      }
      DS_3GPP_MSG2_HIGH("Requesting reestab for bearer id %d with Re-estab Call type %d",
                        eps_bearer_context_p->ds_eps_bearer_context_dyn_p->eps_bearer_id,
                        rab_params.sys_lte_reestab_calltype);
    }
#endif /* FEATURE_DATA_LTE */

    if (bearer_context_p->ds_bearer_context_dyn_p->call_mode == SYS_SYS_MODE_WCDMA)
    {
      rab_params.sys_mode = SYS_SYS_MODE_WCDMA;
      rab_params.rab_reestab_params.cm_wcdma_rab_reestab_params.rabid = 
                        (rabm_rabid_T) bearer_context_p->ds_bearer_context_dyn_p->nsapi; 
      DS_3GPP_MSG1_HIGH("Requesting reestab for rab id %d",
                        bearer_context_p->ds_bearer_context_dyn_p->nsapi);
    }
#ifdef FEATURE_TDSCDMA
    else if(ds3gpp_get_current_network_mode(subs_id) == SYS_SYS_MODE_TDS)
    {
      rab_params.sys_mode = SYS_SYS_MODE_TDS;
      rab_params.rab_reestab_params.cm_wcdma_rab_reestab_params.rabid = 
                        (rabm_rabid_T) bearer_context_p->ds_bearer_context_dyn_p->nsapi; 
      DS_3GPP_MSG1_HIGH("Requesting reestab for rab id %d",
                        bearer_context_p->ds_bearer_context_dyn_p->nsapi);
    }
#endif /* FEATURE_TDSCDMA */
    /*------------------------------------------------------------------------- 
      Indicate to PS the Physlink pointer, Rx watermark count,
      Tx watermark count and dormancy status.
      Dormancy status is set to REORIG_FROM_DORMANCY
    ---------------------------------------------------------------------------*/
    ds_bearer_cntxt_report_ps_wmk_logging(bearer_context_p,
                                          PS_WMK_LOGGING_REORIG_FROM_DORMANCY);

      DS_3GPP_MSG1_HIGH("WM logging Packet onBearer instance: %d",
       (uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr);

    if ( reorig_pkt_ptr != NULL )
    {
      if (0 != ps_wmk_logging_reorig_pkt(DS_SYS_TECH_3GPP,
                                         PS_WMK_UPLINK_PKT,
                                         reorig_pkt_ptr))
      { 
        DS_3GPP_MSG1_ERROR("WM logging Packet Error onBearer instance: %d",
          (uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr);
      }
    }
    else
    {
      DS_3GPP_MSG0_ERROR("WM logging error, Invalid Pkt_ptr");
    }

    ds3gflowmgr_notify_ps_call_state_per_sub(subs_id,
                                             DS3GFLOWMGR_PS_CALL_ORIG_EV);
    
    (void)ds3g_msh_call_rab_reestab_req(
                    bearer_context_p->ds_bearer_context_dyn_p->cm_call_id,
                    &rab_params
                    );
    /*-------------------------------------------------------------------------
     * Set the service_request_pending_flag to TRUE so that we do not post 
     * another request while there is OTA service request still to be answered
    -------------------------------------------------------------------------*/
    per_subs_info_hndl->service_request_pending_flag = TRUE;

    /*-------------------------------------------------------------------------
     * Also mark the bearer context that was used to send out the service req
     * by setting the srv_rsp_exp to TRUE
    -------------------------------------------------------------------------*/
    bearer_context_p->ds_bearer_context_dyn_p->srv_rsp_exp = TRUE;
  }
  else
  {
    if((bearer_context_p->ds_bearer_context_dyn_p->state == DS_BEARER_CONTEXT_STATE_UP) &&
       (PS_PHYS_LINK_GET_STATE(&(bearer_context_p->phys_link)) == PHYS_LINK_DOWN))
    {
      DS_3GPP_MSG0_HIGH("PCH state: phys link up cmd and clear enqueue fun ptr");

      /*--------------------------------------------------------------------- 
        Clear each enqueu function pointer
      --------------------------------------------------------------------*/
      (bearer_context_p->tx_wm.wm_item).each_enqueue_func_ptr = NULL;
      (bearer_context_p->tx_wm.wm_item).each_enqueue_func_data = NULL;
      /*--------------------------------------------------------------------- 
        Set Phys link to COMING UP state
      --------------------------------------------------------------------*/
      if( ps_phys_link_up_cmd(&(bearer_context_p->phys_link), 
                                                  &ps_errno, NULL) != 0)
      {
        DS_3GPP_MSG1_HIGH("phys_link_up_cmd failed errno: %d", ps_errno);
      }
      
      /*--------------------------------------------------------------------- 
        if a PDN is in COMING_UP state, Flow mgr would be in ORIG state.
        Posting ACTIVE on the other PDN would result in starting the 
        flow monitor timer which could lead to data priority being lowered
        before the PDN is connected.
      ---------------------------------------------------------------------*/
      if (!dsPdnContext_is_any_normal_pdn_coming_up_ex(subs_id))
      {
        ds3gflowmgr_notify_ps_call_state_per_sub(subs_id,
			                         DS3GFLOWMGR_PS_CALL_ACTIVE_EV);
      }
    }
    DS_3GPP_MSG2_HIGH("Bearer not in DORMANT state. Bearer: %d, State:%d",\
		    (uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr,
		    bearer_context_p->ds_bearer_context_dyn_p->state);
  }
  return; 
} /* dsWcdmaBearerCntxt_reorig_dorm() */

/*===========================================================================
  FUNCTION DS_3GPP_BEARER_CNTXT_REORIG_ALL_DORM_BEARERS

  DESCRIPTION
    This function is responsible for reoriginating all the dormant calls.
  
  PARAMETERS   
    subs_id: Subscription Id
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
void ds_3gpp_bearer_cntxt_reorig_all_dorm_bearers
(
  sys_modem_as_id_e_type subs_id
)
{
  uint32               index;
  ds_bearer_context_s* bearer_context_p;
  boolean              found_bearer_instance = FALSE;
  ds_bearer_cntxt_per_subs_info_type  *per_subs_info_hndl = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  per_subs_info_hndl = 
    ds_bearer_cntxt_get_per_subs_info_hndl(subs_id);

  if (!ds_bearer_cntxt_validate_per_subs_info_hndl
       (per_subs_info_hndl))
  {
    DS_3GPP_MSG1_ERROR("Bearer Cntxt per subs info handle is invalid for"
                       "subs id",subs_id);
    return;
  }

  /*-------------------------------------------------------------------------
    Loop through all the bearers. Query the bit field corresponding to this 
    bearer in the uplink_data_bitmask. If the field is set, then call the 
    reorig_dorm function.
  -------------------------------------------------------------------------*/
  for(index=0; index < DS_3GPP_MAX_BEARER_CONTEXT; index++)
  {
    bearer_context_p = ds_bearer_cntxt_get_instance_ptr(index);

    if(ds_bearer_cntx_validate_bearer_subscription_internal
         (bearer_context_p, subs_id))
    {
      found_bearer_instance = \
        DS_BEARER_CNTXT_QUERY_BITMASK (per_subs_info_hndl, bearer_context_p);

      if((found_bearer_instance == TRUE) &&
         ((bearer_context_p->ds_bearer_context_dyn_p->state == 
           DS_BEARER_CONTEXT_STATE_UP_DORMANT) || 
          (bearer_context_p->ds_bearer_context_dyn_p->state == 
           DS_BEARER_CONTEXT_STATE_UP_DORMANT_REESTAB)))
      {
        DS_3GPP_MSG1_HIGH("Rab-Reorig for Bearer Inst:%d",
                          (uint32)bearer_context_p->ds_bearer_context_dyn_p->
                          client_data_ptr);
        ds_bearer_cntxt_reorig_dorm(bearer_context_p, NULL);
      }
    }
  }
} /* ds_3gpp_bearer_cntxt_reorig_all_dorm_bearers */

/*===========================================================================
FUNCTION  DS_BEARER_CNTXT_PROC_DORM_REORIG_CMD

DESCRIPTION
  This function is called when we process the command to reestablish RAB
  due to enqueuing data into the watermarks.

PARAMETERS   
  *cmd_ptr - ds cmd ptr

DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
  
SIDE EFFECTS 
  None.
===========================================================================*/
void ds_bearer_cntxt_proc_dorm_reorig_cmd
(
  const ds_cmd_type *cmd_ptr
)
{
  ds_bearer_context_s  *bearer_context_p = NULL;
  ds3gpp_per_subs_info_type* hndl = NULL;
  sys_modem_as_id_e_type   subs_id = SYS_MODEM_AS_ID_NONE;
  boolean                  ps_data_suspend = FALSE;
  boolean                  send_ps_data_avail = FALSE;
  ds_bearer_context_reorig_cmd_type*   data_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DS_3GPP_MSG0_ERROR("NULL ptr passed, return");
    return;
  }

  data_ptr = (ds_bearer_context_reorig_cmd_type*)cmd_ptr->cmd_payload_ptr;

  bearer_context_p =(ds_bearer_context_s*)(data_ptr->bearer_context_p);
  
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    /* free the packet here if valid */
    if (data_ptr->reorig_pkt_ptr != NULL)
    {
      dsm_free_packet( &data_ptr->reorig_pkt_ptr );
    }
    return;
  }

  subs_id = bearer_context_p->ds_bearer_context_dyn_p->subs_id;
  ps_data_suspend = ds3gpp_get_ps_data_suspend_flag(subs_id);
  
  hndl = ds3gpp_get_per_subs_info_hndl(subs_id);
  
  /*Handle if suspend indication was recieved when there was no data in wm,
    will send data available indication to ds during enqueue*/
  if(ds3gpp_validate_per_subs_info_hndl(hndl) && ps_data_suspend)
  {
    if ((!hndl->sent_ps_data_available) && 
  	 PS_PHYS_LINK_FLOW_ENABLED(&(bearer_context_p->phys_link)))
    {
      send_ps_data_avail = ds3gpp_is_ps_data_avail_ind_allowed(subs_id);
      ds_bearer_cntxt_suspend_ind(send_ps_data_avail, subs_id);
    }
  }
  
  /*-------------------------------------------------------------------------
   * Call the function to reorig
  -------------------------------------------------------------------------*/
  ds_bearer_cntxt_reorig_dorm(bearer_context_p, data_ptr->reorig_pkt_ptr);
  /* free the packet after logging  */
  if (data_ptr->reorig_pkt_ptr != NULL)
  {
    dsm_free_packet( &data_ptr->reorig_pkt_ptr );
  }

  /*-------------------------------------------------------------------------
    Set the reorig_cmd_enqueued to FALSE which indicates that we have 
    processed the reorig request on this bearer
  -------------------------------------------------------------------------*/
  bearer_context_p->ds_bearer_context_dyn_p->reorig_cmd_enqueued = FALSE;  
} /*ds_bearer_cntxt_proc_dorm_reorig_cmd()*/


/*===========================================================================
FUNCTION  DS_BEARER_CNTXT_HANDLE_MAX_RAB_REESTAB_RETRY_CNT

DESCRIPTION
  This function handles the max rab_reestab_retry count for a partciular
  bearer. The uplink WM is flushed, phys_link flow is enabled and 
  uplink_data_bitmask would be unset for the bearer. The retry counter
  would be reset to zero.
  
PARAMETERS
  bearer_context_p    The bearer context pointer

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void ds_bearer_cntxt_handle_max_rab_reestab_retry_cnt
(
  ds_bearer_context_s* bearer_context_p
)
{
  sys_modem_as_id_e_type               subs_id = SYS_MODEM_AS_ID_NONE;
  ds_bearer_cntxt_per_subs_info_type  *per_subs_info_hndl = NULL;
  /*------------------------------------------------------------------------*/

  if(!ds_bearer_cntxt_get_subs_id_from_bearer(bearer_context_p, &subs_id))
  {
    return;
  }

  per_subs_info_hndl = 
    ds_bearer_cntxt_get_per_subs_info_hndl(subs_id);

  if (!ds_bearer_cntxt_validate_per_subs_info_hndl
        (per_subs_info_hndl))
  {
    DS_3GPP_MSG1_ERROR("Bearer Cntxt per subs info handle is invalid for"
                       "subs id",subs_id);
    return;
  }

  DS_3GPP_MSG1_HIGH("Handle max rab_reestab_retry for bearer id %d",
                     bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr);

  /*------------------------------------------------------------------------
    Flush the uplink watermark 
  ------------------------------------------------------------------------*/
  dsm_empty_queue( &(bearer_context_p->tx_wm.wm_item) );

  if(PS_PHYS_LINK_GET_STATE(&(bearer_context_p->phys_link)) != PHYS_LINK_DOWN)
  {
    ps_phys_link_down_ind(&(bearer_context_p->phys_link));
  }
  
 
  /*----------------------------------------------------------------------
    Enable phys_link flow for this bearer
  ----------------------------------------------------------------------*/
  ps_phys_link_enable_flow(&(bearer_context_p->phys_link),
                               DS_FLOW_UMTS_MASK);

  /*-------------------------------------------------------------------------
    Unset the bitmask corresponding to this bearer 
  -------------------------------------------------------------------------*/
  DS_BEARER_CNTXT_UNSET_BITMASK (per_subs_info_hndl, bearer_context_p);
  
  /*-----------------------------------------------------------------------
    Reset the rab_reestab_retry counter for this bearer.
  -----------------------------------------------------------------------*/
  DS_BEARER_CNTXT_RESET_RAB_REESTAB_RETRY_CNT(bearer_context_p);
  /*------------------------------------------------------------------------ 
    Reset the access barring rab retry 
  ------------------------------------------------------------------------*/
  ds_bearer_cntxt_reset_access_barring_retry();

  if ( per_subs_info_hndl->uplink_data_bitmask == 0 )
  {
    if ( per_subs_info_hndl->timer_running == TRUE )
    {
      DS_BEARER_CNTXT_CLR_TIMER (per_subs_info_hndl);
    }
  }
} /* ds_bearer_cntxt_handle_max_rab_reestab_retry_cnt */

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_REORIG_DORM_TIMER_CB()

DESCRIPTION
  This function is registered as the timer call back. When the timer to do
  Rab_reestab retry expires, this function is called. The processing is 
  serialized by posting a command to DS.  
      
PARAMETERS
   callback_data : Bearer context pointer registered with the timer

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
  
===========================================================================*/
void ds_bearer_cntxt_reorig_dorm_timer_cb
(
  void*              callback_data /* Pass Subs id here */
)
{
  ds_cmd_type               *cmd_ptr;
  sys_modem_as_id_e_type    *subs_id_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_3GPP_MSG0_HIGH("Re orig dorm timer call back");

  /*
   * This function is executed in the timer call back.
   * So all we do here is to put a command to DS and exit
   */
  if( (cmd_ptr = ds_allocate_cmd_buf(sizeof(sys_modem_as_id_e_type))) == NULL ||
       cmd_ptr->cmd_payload_ptr == NULL)
  {
    ASSERT(0);
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_DORM_REORIG_TMR_EXP;
  subs_id_p = (sys_modem_as_id_e_type*)cmd_ptr->cmd_payload_ptr;
  *subs_id_p = (sys_modem_as_id_e_type)callback_data;
  ds_put_cmd_ext(cmd_ptr);

  return;
} /* ds_bearer_cntxt_reorig_dorm_timer_cb */
/*===========================================================================
FUNCTION ds_bearer_cntxt_dpm_dereg_cb

DESCRIPTION
  This function is registered as the downlink watermark's gone empty function pointer.
  When the watermark is empty, this function is called. The processing is 
  serialized by posting a command to DS.  
      
PARAMETERS
   callback_data : 

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
  
===========================================================================*/
void ds_bearer_cntxt_dpm_dereg_cb
(
  dsm_watermark_type *wm_p,
  void               *call_back_data
)
{
  ds_cmd_type         *cmd_ptr = NULL;
  void                **data_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_3GPP_MSG3_HIGH("Re orig dorm timer call back",0,0,0);

  if(wm_p == NULL)
  {
    DATA_ERR_FATAL("Watermark Pointer is Null");
    return;
  }
  wm_p->gone_empty_func_ptr = NULL;
  wm_p->gone_empty_func_data = NULL; 
  /*
   * This function is executed in the timer call back.
   * So all we do here is to put a command to DS and exit
   */
  
  cmd_ptr = ds_allocate_cmd_buf(sizeof(void*));

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_DPM_DEREG;
  data_ptr = (void**)cmd_ptr->cmd_payload_ptr;
  *data_ptr = call_back_data;

    ds_put_cmd_ext(cmd_ptr);

  return;

} 


/*===========================================================================
FUNCTION DS_BEARER_CNTXT_RESET_ACCESS_BARRING_RETRY

DESCRIPTION
  This function resets access_barring_retry 
      
PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
  
===========================================================================*/
void ds_bearer_cntxt_reset_access_barring_retry
(
   void
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  access_barring_rab_retry = FALSE;
}

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_CHECK_IF_MAX_RAB_RETRIES_PERFORMED

DESCRIPTION
  This function checks if maximum rab retries have been performed.
  The number is different if access_barring_rab_retry_counter is set
      
PARAMETERS
  ds_bearer_context_s* bearer_context_p

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
  
===========================================================================*/
boolean ds_bearer_cntxt_check_if_max_rab_retries_performed
(
  ds_bearer_context_s                 * bearer_context_p,
  ds_bearer_cntxt_per_subs_info_type  *per_subs_info_hndl,
  sys_modem_as_id_e_type               subs_id
)
{
  boolean ret_val = FALSE;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return ret_val;
  }

  if (per_subs_info_hndl == NULL)
  {
    DS_3GPP_MSG0_ERROR("Per subs info hdl is NULL");
    return ret_val;
  }

  /*-------------------------------------------------------------------------- 
    If access barring retry is not enabled check for MAX_RAB_REQ_RETRY_COUNT
    However if access barring is enabled check for
    access_barring_rab_retry_counter
  --------------------------------------------------------------------------*/
  if((bearer_context_p->ds_bearer_context_dyn_p->rab_reestab_retry_cnt 
                  == ds_3gpp_nv_get_max_rab_reestab_retry_cnt(subs_id)) &&
                 (per_subs_info_hndl->rab_reject_local_cause == FALSE)
                  && (!access_barring_rab_retry))
  {
    DS_3GPP_MSG0_HIGH("Non access barring max rab retries attempted");
    ret_val = TRUE;
  }
  else if ((access_barring_rab_retry) && 
           bearer_context_p->ds_bearer_context_dyn_p->rab_reestab_retry_cnt 
           == access_barring_rab_retry_counter)
  {
    DS_3GPP_MSG0_HIGH("Access barring max rab retries attempted");
    ret_val = TRUE;
  }

  return ret_val; 
}


/*===========================================================================
FUNCTION DSWCDMABEARERCNTXT_PROC_REORIG_DORMTMR_EXP_CMD()

DESCRIPTION
  This function is called when we process the command posted to DS as a result
  of dormancy timer expiry. This simply calls the function to reestab RAB.
      
PARAMETERS
  client_data_ptr: Subs Id passed as Client Data Ptr

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
  
===========================================================================*/
void ds_bearer_cntxt_proc_reorig_dormtmr_exp_cmd
(
  void* client_data_ptr 
)
{
  uint32 ran_num;
  ds_bearer_context_s* bearer_context_p;/* Bearer context pointer */
  sys_sys_mode_e_type  current_sys_mode = SYS_SYS_MODE_NONE;
  sys_modem_as_id_e_type               *subs_id_p = 
                                         (sys_modem_as_id_e_type*)client_data_ptr;
  ds_bearer_cntxt_per_subs_info_type  *per_subs_info_hndl = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (subs_id_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("subs_id_p is NULL");
    return;
  }

  per_subs_info_hndl = 
    ds_bearer_cntxt_get_per_subs_info_hndl(*subs_id_p);

  if (!ds_bearer_cntxt_validate_per_subs_info_hndl
        (per_subs_info_hndl))
  {
    DS_3GPP_MSG1_ERROR("Bearer Cntxt per subs info handle is invalid for"
                       "subs id",*subs_id_p);
    return;
  }  

  /* Set the timer_running flag to FALSE */
  per_subs_info_hndl->timer_running = FALSE;

  current_sys_mode = ds3g_get_current_network_mode_ex(*subs_id_p);

  if( DS_3GPP_IS_3GPP_CURRENT_MODE(current_sys_mode))
  {
    /*-------------------------------------------------------------------------
      If the OTA request to reestablish RAB is already pending, then we restart
      the timer, provided there are RABs that need to be set up. This is 
      determined by the uplink_data_bitmask being non-null.
    -------------------------------------------------------------------------*/
    if(per_subs_info_hndl->service_request_pending_flag == TRUE)
    {
      if(per_subs_info_hndl->uplink_data_bitmask != 0)
      {
        if (!access_barring_rab_retry)
        {
          ran_num = ps_utils_generate_16bit_rand_num_by_range(
                    DS_BEARER_CNTXT_RAB_REESTABLISH_MIN_RETRY_TIME, 
                    DS_BEARER_CNTXT_RAB_REESTABLISH_MIN_RETRY_TIME * 4 );
        }
        else
        {
          DS_3GPP_MSG1_MED("Setting rab_retry_timer to ACCESS_BARRING "
                           "RETRY VAL of %d",
                           access_barring_rab_retry_timer);
          ran_num = access_barring_rab_retry_timer;
        }
    
        DS_BEARER_CNTXT_SET_TIMER (per_subs_info_hndl,ran_num);
      }
    }
    else
    {
      if(per_subs_info_hndl->uplink_data_bitmask != 0)
      {
        /*-----------------------------------------------------------------------
          Call the function to get a bearer context on which we can initiate a
	        RAB reestablishment.
        -----------------------------------------------------------------------*/
        bearer_context_p = ds_bearer_cntxt_get_bearer_inst_to_reorig(*subs_id_p);
        if(ds_bearer_cntx_validate_bearer_context(bearer_context_p))
        {
          if (ds_3gpp_nv_get_max_rab_reestab_retry_cnt(*subs_id_p) != RAB_REESTAB_RETRY_CNT_INVALID)
          {
            
            /*-----------------------------------------------------------------
              Rab re-estab retry cnt mechanism will be triggerred only when 
              reject cause is not local reject from NAS.
            -----------------------------------------------------------------*/
            if (ds_bearer_cntxt_check_if_max_rab_retries_performed
               (bearer_context_p,per_subs_info_hndl, *subs_id_p))
            {
              ds_bearer_cntxt_handle_max_rab_reestab_retry_cnt(bearer_context_p);

              /*----------------------------------------------------------------- 
                Get the bearer context for the next bearer which has uplink 
                data to send.
              ------------------------------------------------------------------*/
              bearer_context_p = ds_bearer_cntxt_get_bearer_inst_to_reorig(*subs_id_p);
            }

            /*--------------------------------------------------------------------
              Check if the bearer_context is not NULL and uplink_data_bitmask is
              set. This is required to handle the scenario when multiple bearers
              have data to send and one bearer reaches max_retry_cnt, in such a 
              case DS would send service_request for the next bearer which has 
              uplink bitmask set.
            --------------------------------------------------------------------*/
            if (bearer_context_p != NULL && 
                per_subs_info_hndl->uplink_data_bitmask != 0)
            {
              DS_3GPP_MSG3_HIGH("TIMER CB FOR Rab-Reorig called; Bearer Inst:%d",\
                                (uint32)bearer_context_p->ds_bearer_context_dyn_p
                                ->client_data_ptr,0,0);

              ds_bearer_cntxt_reorig_dorm(bearer_context_p, NULL);

              /*---------------------------------------------------------------
                Increment retry_cnt only if reject cause is not local
              ---------------------------------------------------------------*/
              if (per_subs_info_hndl->rab_reject_local_cause == FALSE)
              {
                DS_BEARER_CNTXT_INC_RAB_REESTAB_RETRY_CNT(bearer_context_p);
              }
            }
          }
          else
          {
            DS_3GPP_MSG3_HIGH("TIMER CB FOR Rab-Reorig called; Bearer Inst:%d",\
                     (uint32)bearer_context_p->ds_bearer_context_dyn_p
                              ->client_data_ptr,0,0);
            ds_bearer_cntxt_reorig_dorm(bearer_context_p, NULL);
          }
        }
        else
        {
	    /*-----------------------------------------------------------------------
	    technically we should not reach here. If we do, the case is such that
	    the uplink_data_bitmask is non null but we could not find a bearer that
	    needs RAB to be set up. This could possibly be due to corruption of
	    the bitmask. We ignore the reorig req and set the bitmask to NULL.
	    -----------------------------------------------------------------------*/
	      DS_3GPP_MSG0_ERROR("Could not locate a bearer to reorig on. Ignoring cmd");
          per_subs_info_hndl->uplink_data_bitmask = 0;
        }
      }
    }
  }
  else
  {
    DS_3GPP_MSG0_HIGH("Sys mode non-3gpp, Not reorig'ing dormant bearer");
  }
  
  
  /*------------------------------------------------------------------------ 
    Reset the access barring rab retry 
    ------------------------------------------------------------------------*/
  ds_bearer_cntxt_reset_access_barring_retry();
  return;
} /* ds_bearer_cntxt_proc_reorig_dormtmr_exp_cmd() */

#ifdef FEATURE_DATA_LTE
/*===========================================================================
FUNCTION DS_BEARER_CNTXT_GET_BEARER_FROM_EPS_ID

DESCRIPTION
  This function is used to find the bearer context, given a EPS bearer id

PARAMETERS
  IN
    eps_bearer_id_T - eps bearer id
    subs_id         - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the bearer context if there is a EPS bearer ID match
  NULL if there is no match

SIDE EFFECTS
  None.

===========================================================================*/
ds_bearer_context_s* ds_bearer_cntxt_get_bearer_from_eps_id
(
  eps_bearer_id_T        eps_bearer_id,
  sys_modem_as_id_e_type subs_id
)
{
  int                 bearer_cntx_inst;
  ds_bearer_context_s *bearer_context_p = NULL;
  ds_eps_bearer_context_s * eps_bearer_context = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(bearer_cntx_inst = 0;
      bearer_cntx_inst < DS_3GPP_MAX_BEARER_CONTEXT;
      bearer_cntx_inst++) 
  {
    bearer_context_p = ds_bearer_cntxt_get_instance_ptr(bearer_cntx_inst);

    if (!ds_bearer_cntx_validate_bearer_subscription_internal(bearer_context_p,subs_id))
    {
      continue;
    }

    eps_bearer_context = (ds_eps_bearer_context_s *)
      DS_BEARER_CTXT_VOBJ(bearer_context_p, SYS_SYS_MODE_LTE);

    if(!ds_eps_bearer_cntxt_validate_bearer_cntxt(eps_bearer_context))
    {
      continue;
    }

    if(eps_bearer_context->ds_eps_bearer_context_dyn_p->eps_bearer_id == eps_bearer_id)
    {
      DS_3GPP_MSG1_HIGH("Found the bearer context with eps bearer id %d",eps_bearer_id);
      break;
    }
  }
  /*---------------------------------------------------------------------------
    If we are not able to find a match based on the call id, the following
    will be printed. NULL will be returned since bearer_context_p is NULL
  ---------------------------------------------------------------------------*/
  if(bearer_cntx_inst >= DS_3GPP_MAX_BEARER_CONTEXT)
  {
    DS_3GPP_MSG1_ERROR("Cannot find the requested bearer context with eps bearer id: %d",
             eps_bearer_id);
    bearer_context_p = NULL;
  }

  return bearer_context_p;
} /* ds_bearer_cntxt_get_bearer_from_call_id() */

/*===========================================================================
FUNCTION  DS_BEARER_CNTXT_GET_RESEND_CALL_END

DESCRIPTION
  This function sends pdn_disconnect as CM returned error for CALL_CMD_END
  with cause CM_CALL_CMD_ERR_MODE_INFO_CHG_LTE.

PARAMETERS 
  call_id - call_id for which CALL_END error is returned.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void ds_bearer_cntxt_resend_call_end
(
  byte  call_id
)
{
  boolean               ret_val = FALSE;
  ds_bearer_context_s  *bearer_cntxt_p = NULL;

  bearer_cntxt_p = ds_bearer_cntxt_get_bearer_from_call_id((cm_call_id_type)call_id);

  if (NULL == bearer_cntxt_p)
  {
    DS_3GPP_MSG0_ERROR("Invalid bearer_cntxt_p");
    return;
  }

  ret_val = ds3g_msh_pdn_disconnect_req_ex(NULL,
  	                             bearer_cntxt_p->ds_bearer_context_dyn_p->sdf_id,
  	                             call_id,
  	                             bearer_cntxt_p->ds_bearer_context_dyn_p->nsapi,
  	                             FALSE,
  	                             bearer_cntxt_p->ds_bearer_context_dyn_p->subs_id);

  DS_3GPP_MSG1_MED("PDN Disonnect returned %d", ret_val);
} /* ds_bearer_cntxt_resend_call_end */

#endif /* FEATURE_DATA_LTE */

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_STORE_CALL_ID 

DESCRIPTION
  This function stores the CM call id in the bearer context struct pointed to
  by bearer_context_p

PARAMETERS
  IN
    bearer_context_p     - Pointer to the bearer context
    call_id              - CM call id to update the bearer with

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_bearer_cntxt_store_call_id
(
  ds_bearer_context_s *bearer_context_p,
  cm_call_id_type     call_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }

  bearer_context_p->ds_bearer_context_dyn_p->cm_call_id = call_id;
}

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_GET_BEARER_FROM_SDF_ID

DESCRIPTION
  This function is used to fetch the bearer context using the SDF Id.
  
PARAMETERS
  sdf_id - SDF ID to use to locate the bearer

DEPENDENCIES
  None.

RETURN VALUE
 bearer_context_p  - Pointer to Bearer context
 NULL if the bearer cannot be located

SIDE EFFECTS
  None.
===========================================================================*/
ds_bearer_context_s* ds_bearer_cntxt_get_bearer_from_sdf_id(uint16 sdf_id)
{
  int loop_index;
  ds_bearer_context_s *bearer_context_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*------------------------------------------------------------------------
    Search into the table only if the SDF ID passed into this function is
    not invalid. 0xFFFF is treated as an invalid SDF Id. This is usually got
    during N/W init QOS calls. In such a case, we should not return a bearer
  ------------------------------------------------------------------------*/
  if(sdf_id != 0xFFFF)
  {
    /*------------------------------------------------------------------------
      Valid SDF ID. Perform search in the bearer context table.
    ------------------------------------------------------------------------*/
    for (loop_index = 0; loop_index < DS_3GPP_MAX_BEARER_CONTEXT; loop_index++)
    {
      bearer_context_p = ds_bearer_cntxt_get_instance_ptr(loop_index);

      if(ds_bearer_cntx_validate_bearer_context_internal(bearer_context_p) && 
         bearer_context_p->ds_bearer_context_dyn_p->sdf_id == sdf_id)
      {
        break;
      }
    }/* FOR LOOP*/

    if (loop_index >= DS_3GPP_MAX_BEARER_CONTEXT)
    {
      bearer_context_p = NULL;
  }
  }

  return bearer_context_p;
} /* ds_bearer_cntxt_get_bearer_from_sdf_id */


/*===========================================================================
FUNCTION DS_BEARER_CNTXT_SET_BEARER_STATE

DESCRIPTION
  This function is used to set the bearer to a particular state
  
PARAMETERS
  bearer_context_p  - Pointer to Bearer context
  state             - State to set the bearer context to

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_bearer_cntxt_set_bearer_state
(
  ds_bearer_context_s* bearer_context_p,
  ds_bearer_context_state_e state
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }
  bearer_context_p->ds_bearer_context_dyn_p->state = state;
} /* ds_bearer_cntxt_set_bearer_state */

/*===========================================================================
FUNCTION ds_bearer_cntxt_CALL_RAB_RE_ESTAB_IND_HDLR

DESCRIPTION
  This function handles rab reject indication for 3GPP modes. This will in
  turn dispatch any mode specific functions
  
PARAMETERS  
  call_id         -   CM call id
  mode_info_ptr   -   mode specific call info
  end_params      -   Call end param
  subs_id         -   Subscription Id
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE if success, FALSE otherwise.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_bearer_cntxt_call_rab_re_estab_ind_hdlr
(
  cm_call_id_type           call_id,
  cm_call_mode_info_s_type  *mode_info_ptr,
  cm_end_params_s_type      *end_params,
  sys_modem_as_id_e_type     subs_id
)
{
  ds_bearer_context_s                      *bearer_context_p = NULL;
  sys_sys_mode_e_type   network_mode;
  ds_pdn_context_s                         *pdn_cntx_p = NULL;
  ps_iface_ioctl_bearer_tech_changed_type    tech_change_info;
  uint32                 ran_num;
  void                                     *sm_ptr = NULL;
  ds_bearer_cntxt_per_subs_info_type       *per_subs_info_hndl = NULL;
  ds_3gpp_fd_info_s*     report_p;
  int16                  dss_errno;
  boolean                                   ret_val = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------    
    Get the 3GPP mode we are operating in
  -------------------------------------------------------------------------*/
  network_mode = ds3gpp_get_current_network_mode(subs_id);

  /*-------------------------------------------------------------------------    
    Fill in the call end parameters : DS manager uses passes this info to cm
    for a call release if we return an error from here\
    VRK:TODO move this to mode specific hdlrs
  -------------------------------------------------------------------------*/
  end_params->call_id   = call_id;

  if(network_mode == SYS_SYS_MODE_LTE)
  {
    end_params->info_type = CM_CALL_MODE_INFO_LTE;
  }
  else if(network_mode == SYS_SYS_MODE_WCDMA || 
          network_mode == SYS_SYS_MODE_GSM
#ifdef FEATURE_TDSCDMA
          ||
          network_mode == SYS_SYS_MODE_TDS
#endif
         )
  {
    end_params->info_type = CM_CALL_MODE_INFO_GW_PS;
  }

  /*-------------------------------------------------------------------------
    Get the bearer context from call id
  -------------------------------------------------------------------------*/
  bearer_context_p =  ds_bearer_cntxt_get_bearer_from_call_id(call_id);

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return DS3G_FAILURE;
  }

  per_subs_info_hndl = 
    ds_bearer_cntxt_get_per_subs_info_hndl(subs_id);

  if (!ds_bearer_cntxt_validate_per_subs_info_hndl
        (per_subs_info_hndl))
  {
    DS_3GPP_MSG1_ERROR("Bearer Cntxt per subs info hndl invalid for "
                       "Subs Id(CM):%d", subs_id);
    return DS3G_FAILURE;
  }

  if (bearer_context_p->ds_bearer_context_dyn_p->state == 
      DS_BEARER_CONTEXT_STATE_UP_DORMANT ||
      bearer_context_p->ds_bearer_context_dyn_p->state == 
      DS_BEARER_CONTEXT_STATE_UP_DORMANT_REESTAB)
  {
    bearer_context_p->ds_bearer_context_dyn_p->state = 
      DS_BEARER_CONTEXT_STATE_UP_DORMANT_REESTAB;

    DS_3GPP_MSG2_HIGH(
       "RAB reestab ind -from dormancy. call id:%d bearer instance:%d",
       call_id,
       (uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr);
  }
  else
  {
    DS_3GPP_MSG2_HIGH(
       "RAB reestab ind call id:%d  bearer inst:%d during bearer setup",
       call_id,
       (uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr);
  }

  /*-------------------------------------------------------------------------
    Get the PDN cntx assigned to this bearer cntx
  -------------------------------------------------------------------------*/
  pdn_cntx_p = (ds_pdn_context_s*)
                   ds_bearer_cntxt_get_pdn_cntxt(bearer_context_p);

  if (!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p, &subs_id))
  {
    return DS3G_FAILURE;
  }
  
  /*-------------------------------------------------------------------------
   * Reset the service_request_pending_flag if the response if obtained 
   * as a result of trying to reorig using this bearer
  -------------------------------------------------------------------------*/
  if(bearer_context_p->ds_bearer_context_dyn_p->srv_rsp_exp == TRUE)
  {
    per_subs_info_hndl->service_request_pending_flag = FALSE;
    bearer_context_p->ds_bearer_context_dyn_p->srv_rsp_exp = FALSE;
  }
  else
  {
    /*  checking if fast dormancy needs to be triggered for NW initiated 
        reestablishment */
    if ((ds_3gpp_bearer_cntxt_is_modem_fd_enabled()== TRUE)&&
        (network_mode != SYS_SYS_MODE_LTE)&&
        (bearer_context_p->ds_bearer_context_dyn_p->state == 
                          DS_BEARER_CONTEXT_STATE_UP_DORMANT_REESTAB))
    {
      /*  give Indication to PS to start FD algorithm*/
      report_p = (ds_3gpp_fd_info_s*) 
              modem_mem_alloc(sizeof(ds_3gpp_fd_info_s),MODEM_MEM_CLIENT_DATA);

      if (report_p != NULL)
      {
        report_p->timer_1_value = ds_3gpp_fd_info.timer_1_value;
        report_p->timer_2_value = ds_3gpp_fd_info.timer_2_value;
      }
      DS_3GPP_MSG0_HIGH("NW Init RAB-Reestablishment, Start FD Ind to PS");
      if (0 > ps_sys_conf_set_ex(PS_SYS_TECH_3GPP,
                                 PS_SYS_CONF_START_FAST_DORMANCY,
       (ps_sys_subscription_enum_type)ds3gsubsmgr_subs_id_cm_to_ds(subs_id),
                                  report_p,
                                  &dss_errno))
     {
       DS_3GPP_MSG0_HIGH("Sending Start fast dormancy Ind Failed");
     }
 
     if (report_p!=NULL)
     {
       modem_mem_free(report_p, MODEM_MEM_CLIENT_DATA);
     }
    }
  }


    /*--------------------------------------------------------------------------
      Get the throttling state macine and reset it. NULL checking need not be
      performed here as there is no dereferencing of the opaque handle in this
    function. 
    As per Latest VZW Spec, VZW LTE Data Retry Reqs spec:
    3.3.3 Per System Nature of Throttling, throttling SM needs to be cleared 
    on receiving unsolicited RAB_REESTAB_IND from NW.
    --------------------------------------------------------------------------*/
  sm_ptr = ds_3gpp_throt_sm_get_throttling_sm(FALSE, subs_id);
  ds_3gpp_throt_sm_reset(sm_ptr, subs_id);

  /*-------------------------------------------------------------------------
    Unset the bitmask corresponding to this bearer 
  -------------------------------------------------------------------------*/
  DS_BEARER_CNTXT_UNSET_BITMASK (per_subs_info_hndl, bearer_context_p);

  /*-----------------------------------------------------------------------
    Reset the rab_reestab_retry counter for this bearer.
  -----------------------------------------------------------------------*/
  DS_BEARER_CNTXT_RESET_RAB_REESTAB_RETRY_CNT(bearer_context_p);

  if(per_subs_info_hndl->uplink_data_bitmask == 0)
  {
    if(per_subs_info_hndl->timer_running == TRUE)
    {
      DS_BEARER_CNTXT_CLR_TIMER (per_subs_info_hndl)
    }
  }
  else
  {
    if(per_subs_info_hndl->timer_running == FALSE)
    {
      /*-----------------------------------------------------------------------    
        Set the timer with the random value between 
        DS_BEARER_CNTXT_RAB_REESTABLISH_MIN_RETRY_TIME and 
        DS_BEARER_CNTXT_RAB_REESTABLISH_MIN_RETRY_TIME * 4 to send out RAB
        reestablishment request again. Once the timer expires, the dormant call
        reorig function will be called and RAB reestablishment is tried again.
      -----------------------------------------------------------------------*/
      ran_num = ps_utils_generate_16bit_rand_num_by_range(
        DS_BEARER_CNTXT_RAB_REESTABLISH_MIN_RETRY_TIME,
           DS_BEARER_CNTXT_RAB_REESTABLISH_MIN_RETRY_TIME * 4);

      DS_BEARER_CNTXT_SET_TIMER (per_subs_info_hndl,ran_num)
    }
   }
  /*---------------------------------------------------------------------------
   * Now that we have the Reorigination done, we should reset the each enqueue 
   * function pointer to NULL so that sucessive enqueuing of packet to the WM 
   * do not cause a reorig request from being sent to NAS
  ---------------------------------------------------------------------------*/
  (bearer_context_p->tx_wm.wm_item)
  .each_enqueue_func_ptr = NULL;
  /*------------------------------------------------------------------------ 
    Reset the access barring rab retry 
  ------------------------------------------------------------------------*/
  ds_bearer_cntxt_reset_access_barring_retry();

  /*---------------------------------------------------------------------------
    Before proceeding, we have to make sure that there is no initiation to tear
    down the call. If there has been an attempt to tear down the call, we just
    have to return failure indicating CM that the lower layers will not be set
    up
  ---------------------------------------------------------------------------*/

  if(bearer_context_p->ds_bearer_context_dyn_p->state ==
     DS_BEARER_CONTEXT_STATE_GOING_DOWN)
  {
    DS_3GPP_MSG2_HIGH(
       "Recd RAB REESTAB IND aft call tear down. Call Id:%d Bearer:%d",
       call_id,
       (uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr);
    return DS3G_SUCCESS;
  }

  /*-------------------------------------------------------------------------    
    Fake out a bearer tech changed event for the PDN. This will cause QMI to
    query lower layers again for bearer rate and send a rate change ind to
    QMI clients. 
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    fill in tech change info structure based for old mode 
  -------------------------------------------------------------------------*/
  ds_3gpp_pdn_cntx_get_bearer_tech_type(network_mode,
                                        &(tech_change_info.old_bearer_tech),
                                        subs_id);

  /*-------------------------------------------------------------------------
    fill in tech change info structure based for new mode 
  -------------------------------------------------------------------------*/
  ds_3gpp_pdn_cntx_get_bearer_tech_type(network_mode,
                                        &(tech_change_info.new_bearer_tech),
                                        subs_id);

  DS_3GPP_MSG0_HIGH("Posting from RAB Re-estab indication");
  ds_3gpp_pdn_cntx_post_iface_generic_ind_for_pdn(pdn_cntx_p,
                                       IFACE_BEARER_TECH_CHANGED_EV,
                                       (void *)&tech_change_info);

  /*---------------------------------------------------------------------------
    Now dispatch the mode specific rab re-estab ind hdlr
  ---------------------------------------------------------------------------*/
  ret_val = ds_bearer_cntxt_rab_re_estab_ind(bearer_context_p, mode_info_ptr);

  /*-----------------------------------------------------------------------
    If no other bearer is active, It means this is the first bearer to go
    active. Hence send PS_CALL_ACTIVE_IND to DS3G flowmgr.
    This check is added to avoid sending duplicate indications to
    DS3Gflowmgr.

    if a PDN is in COMING_UP state, Flow mgr would be in ORIG state.
    Posting ACTIVE on the other PDN would result in starting the 
    flow monitor timer which could lead to data priority being lowered
    before the PDN is connected.
  -----------------------------------------------------------------------*/
  if( (ret_val == TRUE) &&
      (!ds_bearer_cntx_is_any_other_bearer_per_sub_active(bearer_context_p)) &&
      (!dsPdnContext_is_any_normal_pdn_coming_up_ex(subs_id)))
  {
    DS_3GPP_MSG0_MED("PS_DATA_CALL_ACTIVE_IND");
    ds3gflowmgr_notify_ps_call_state_per_sub(subs_id,
                                             DS3GFLOWMGR_PS_CALL_ACTIVE_EV);
  }
  return ret_val;
} /* ds_bearer_cntxt_call_rab_re_estab_ind_hdlr */

/*===========================================================================
FUNCTION ds_bearer_cntxt_REORIG_DORM_CALL_CB

DESCRIPTION
  This function sends a command to DS task to trigger dormant re-orig. Called
  when data is enqueued into the Um Tx watermark. Since it runs in PS task,
  we are queuing a command to DS.
  
PARAMETERS  
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.

===========================================================================*/
void ds_bearer_cntxt_reorig_dorm_call_cb
(
  dsm_watermark_type *wm_p,
  void               *call_back_data
)
{
  ds_cmd_type         *cmd_ptr = NULL;
  ds_bearer_context_s *bearer_context_p = NULL;
  ds_bearer_context_reorig_cmd_type *data_ptr = NULL;
  dsm_item_type                     *log_item = NULL; /* Packet to be logged */
  uint16                            logged_size = 0;
  uint32                            current_cnt = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  bearer_context_p = (ds_bearer_context_s*)call_back_data;

  ds3gpp_enter_global_crit_section();
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    ds3gpp_leave_global_crit_section();
    return;
  }  
  
  /*-------------------------------------------------------------------------
    Not posting RAB_REESTAB_REQ for PDCP Status PDUs enqueued to the WM.
  -------------------------------------------------------------------------*/
  current_cnt = dsm_queue_cnt(&(bearer_context_p->tx_wm.wm_item));

  if (current_cnt < DS_3GPP_MIN_IP_PACKET_SIZE)
  {
    DS_3GPP_MSG1_HIGH("Each enqueue cb: Current_cnt in tx_wm is: %d",
  	               current_cnt);
    ds3gpp_leave_global_crit_section();
    return;
  }
  
  if(bearer_context_p->ds_bearer_context_dyn_p->reorig_cmd_enqueued == FALSE)
  {
    DS_3GPP_MSG0_HIGH("Each enqueue function call back. Posting re-orig"
                      " command to DS");

    /*------------------------------------------------------------------------
     * This function is executed in the PS context.
     * So all we do here is to put a command to DS and exit
    ------------------------------------------------------------------------*/
    cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_bearer_context_reorig_cmd_type));

    if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
    {
      ds3gpp_leave_global_crit_section();
      ASSERT(0);
      return;
    }

    cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_DORM_REORIG;
    data_ptr = (ds_bearer_context_reorig_cmd_type*)cmd_ptr->cmd_payload_ptr;
    data_ptr->bearer_context_p = (void*)bearer_context_p;
    data_ptr->reorig_pkt_ptr = NULL;

    /* Capture the first packet responsible for re-origination from dormancy for logging 
       purpose. This is done by Duping the first Packet in tx_wm (DSM Item) and passing 
       the same in Command Payload to command handler which does logging 
       Each enqueue is called as soon as Packet is enqueued and is protected under 
       DSM LOCK. So Duping the item here will make sure we have correct Packet info
       to log even if DSM item in WM gets freed somehow
       */
    if ((dsm_queue_cnt(&(bearer_context_p->tx_wm.wm_item)) == 1) &&
         (bearer_context_p->ds_bearer_context_dyn_p->state ==
                                      DS_BEARER_CONTEXT_STATE_UP_DORMANT))
    { 
       if((log_item = (dsm_item_type*)q_check(
           bearer_context_p->tx_wm.wm_item.q_ptr)) != NULL)
       {
           logged_size = dsm_dup_packet( &(data_ptr->reorig_pkt_ptr), log_item, 0, 
                                         PS_WMK_LOGGING_REORIG_PKT_LOG_SIZE );
           
           DS_3GPP_MSG1_HIGH("Duping first re-orig packet from dormancy, size: %d",\
                                                                       logged_size);
       }
    }

    /*------------------------------------------------------------------------
      Here, we set the reorig_cmd_enqueued flag to true. This is to avoid
      multiple reorig commands from being enqueued for the same bearer when
      a bunch of packets are enqueued into the WM.

      This flag is cleared when we process this command which gives rise to a
      question as to what happens if a packet is subsequently enqueued into
      the WM. It is handled as follows: As soon as there is a reorig request,
      we disable flow on the phys link. Thus subsequent enqueuing of packets
      does not happen on the same bearer until the flow on the phys link is
      enabled. This way, we ensure that we do not overrun the DS command buf.

      Put the command only after setting reorig_cmd_enqueued flag to avoid 
      race condition between PS and DS task, race condition will happen if 
      DS processes the command before setting reorig_cmd_enqueued
    ------------------------------------------------------------------------*/
    bearer_context_p->ds_bearer_context_dyn_p->reorig_cmd_enqueued = TRUE;
    ds_put_cmd_ext(cmd_ptr);

  }
 
  ds3gpp_leave_global_crit_section();

} /* ds_bearer_cntxt_reorig_dorm_call_cb() */

/*===========================================================================
FUNCTION ds_bearer_cntxt_RAB_RELEASE_PDP_PROTO_STACK

DESCRIPTION
  This function registers a non_empty_function callback pointer in the
  PS to PDCP watermark used for PDP-IP calls. When data appears on the
  watermark again, the callback will be invoked to initiate  RAB
  re-establishment
  
PARAMETERS  
  bearer_context_p   - bearer context pointer
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
LOCAL void ds_bearer_cntxt_rab_release_pdp_proto_stack
(
  ds_bearer_context_s        *bearer_context_p
)
{
  dsm_watermark_type*  tx_wm_p;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }

  DS_3GPP_MSG2_HIGH("Uncoupling PDCP watermark for PDP-IP call Cid:%d Br In:%d",
      bearer_context_p->ds_bearer_context_dyn_p->cm_call_id,(uint32)
                    bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr);

  /*-------------------------------------------------------------------------  
    Get the PDCP state information for the call.
  -------------------------------------------------------------------------*/
  tx_wm_p = &(bearer_context_p->tx_wm.wm_item);

  /*-------------------------------------------------------------------------  
    Register each enqueue function ptr.
    Call id will be passed as parameter when watermark cb functions take a
    parameter.
  -------------------------------------------------------------------------*/  
  tx_wm_p->each_enqueue_func_ptr = ds_bearer_cntxt_reorig_dorm_call_cb;
  tx_wm_p->each_enqueue_func_data = (void*) bearer_context_p;
} /* dsBearer_rab_release_pdp_proto_stack() */

/*===========================================================================
FUNCTION DS_3GPP_BEARER_CNTXT_GET_RETRY_TIMER_VAL

DESCRIPTION
  This function gets the retry timer value for rab re-establishment
  
PARAMETERS  
  consult_throt_sm - boolean flag indicating if we should consult throt sm
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  timer val (uint32)
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
uint32 ds_3gpp_bearer_cntxt_get_retry_timer_val
(
  boolean                consult_throt_sm,
  sys_modem_as_id_e_type subs_id
)
{

  void                 *sm_ptr;      
  uint32               throt_timer_val;

  uint32               timer_val;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*----------------------------------------------------------------------------
    First get a random number between a 100 and 400 ms
	If access barring rab retry is supported simply set the rab retry timer to
    DS_BEARER_CNTXT_RAB_REESTABLISH_MIN_RETRY_TIME
  ----------------------------------------------------------------------------*/
  if (!access_barring_rab_retry)
  {
  timer_val = ps_utils_generate_16bit_rand_num_by_range(
                   DS_BEARER_CNTXT_RAB_REESTABLISH_MIN_RETRY_TIME,
                     DS_BEARER_CNTXT_RAB_REESTABLISH_MIN_RETRY_TIME * 4);
  }
  else
  {
    timer_val = access_barring_rab_retry_timer;
  }

  if(consult_throt_sm == TRUE)
  {
    /*----------------------------------------------------------------------
      We have to consult the SM since throttling has kicked in.
      Get a handle to the system wide throttling object. This is an opaque
      handle.
      Call this function with argument FALSE meaning we need not create a sm
      if it does not already exists. Here we are interested in only retrieving
      the SM for getting the back off timer
    ----------------------------------------------------------------------*/
    sm_ptr = ds_3gpp_throt_sm_get_throttling_sm(FALSE, subs_id);

    if(ds_3gpp_throt_sm_get_back_off_timer_val
         (sm_ptr,&throt_timer_val, subs_id) == TRUE)
    {
      if (throt_timer_val != 0) 
      {
        /*----------------------------------------------------------------------
          Throttling algorithm returned a non zero timer
          Use the value as dictated by the back-off algorithm

          If the throttling algorithm returned a zero timer, the random number
          between 100 and 400 ms will be used.
        ----------------------------------------------------------------------*/
        timer_val = throt_timer_val;
      }
      
    } // got a timer value from the throt sm
  }

  return timer_val;
} /* ds_3gpp_bearer_cntxt_get_retry_timer_val */

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_REORIG_FROM_DORM_ON_PEND_DATA_IN_TX_WM

DESCRIPTION
  This function attempts to reoriginate from dormancy if there is pending data
  in Tx WM.
  
PARAMETERS  
  bearer_context_ptr - Bearer context ptr
 
DEPENDENCIES 
  None.
  
RETURN VALUE 
  void 
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_bearer_cntxt_reorig_from_dorm_on_pend_data_in_tx_wm
(
  ds_bearer_context_s *bearer_context_p
)
{
  sys_modem_as_id_e_type subs_id = SYS_MODEM_AS_ID_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds_bearer_cntxt_get_subs_id_from_bearer(bearer_context_p, &subs_id))
  {
    return;
  }
  /*----------------------------------------------------------------------- 
    If there is pending data in Tx WM's trigger reorig request
    ----------------------------------------------------------------------*/

  if((bearer_context_p->tx_wm.wm_item.current_cnt != 0) 
      && (ds3gpp_get_ps_data_suspend_flag(subs_id) == FALSE))
  {
    DS_3GPP_MSG1_HIGH("Data in Tx WM:Rab re-est immly:Bearer Ins:%d",
             (uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr);
    ds_bearer_cntxt_reorig_dorm(bearer_context_p, NULL); 
  }

  return;

} /*ds_bearer_cntxt_reorig_from_dorm_on_pend_data_in_tx_wm */


/*===========================================================================
FUNCTION ds_bearer_cntxt_CALL_RAB_RELEASE_HDLR

DESCRIPTION
  This function handles rab release procedure for 3GPP modes.
  
PARAMETERS  
  call_id    - CM call id
  end_param  - call end parameters
  subs_id    - Subscription Id
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE if success, FALSE otherwise.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_bearer_cntxt_call_rab_release_hdlr
(
  cm_call_id_type           call_id,
  cm_end_params_s_type      *end_params,
  sys_modem_as_id_e_type     subs_id
)
{
  ds_bearer_context_s    *bearer_context_p = NULL;
  boolean              retVal = DS3G_FAILURE;
  dsm_watermark_type   *tx_wm_ptr=NULL;       /* Watermark pointer      */
  int16                dss_errno;
  ds_3gpp_fd_info_s    *report_p;
  sys_sys_mode_e_type   network_mode = SYS_SYS_MODE_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*-------------------------------------------------------------------------
    Fill in the call end parameters : DS manager uses passes this info to cm
    for a call release if we return an error from here
  -------------------------------------------------------------------------*/
  network_mode = ds3gpp_get_current_network_mode(subs_id);
  end_params->call_id   = call_id;
  end_params->info_type = CM_CALL_MODE_INFO_LTE;

  /*-------------------------------------------------------------------------
    Get the bearer context from the call ID.
  -------------------------------------------------------------------------*/
  bearer_context_p = ds_bearer_cntxt_get_bearer_from_call_id(call_id);

  /*-------------------------------------------------------------------------
    Verify that the call instance is valid.
  -------------------------------------------------------------------------*/
  if(ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    DS_3GPP_MSG2_HIGH(
       "RAB Rel Hdlr: call id:%d Bearer instance:%d", call_id,
       (uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr);

    DS_BEARER_CTXT_VF_CALL_NO_ARG_PER_BEARER
    (
      dereg_pdcp, 
      bearer_context_p    
    );
    /* it is possible that we get bearer release in GSM mode after resume 
        during L2G CCO IRAT */
    if (network_mode == SYS_SYS_MODE_GSM) 
    {
       bearer_context_p->ds_bearer_context_dyn_p->state =
       DS_BEARER_CONTEXT_STATE_UP_DORMANT;
       DS_3GPP_MSG0_HIGH("RAB Rel Hdlr: in GSM mode, move bearer to UP_DORMANT");
       return DS3G_SUCCESS;
    }
    if(bearer_context_p->ds_bearer_context_dyn_p->state !=
       DS_BEARER_CONTEXT_STATE_UP_DORMANT)
    {
      /*---------------------------------------------------------------------
        Indicate to PS the Physlink pointer, Rx watermark count,
        Tx watermark count and dormancy status.
        Dormancy status is set to DORMANT
      ---------------------------------------------------------------------*/
      ds_bearer_cntxt_report_ps_wmk_logging(bearer_context_p,
                                            PS_WMK_LOGGING_DORMANT);

      /*---------------------------------------------------------------------
        Call is not dormant. Go ahead and put it to dormancy
      ---------------------------------------------------------------------*/
    ds_bearer_cntxt_rab_release_pdp_proto_stack(bearer_context_p);

      /*---------------------------------------------------------------------
      Send Phy link down ind to PS IFACE.
      ---------------------------------------------------------------------*/
      ps_phys_link_down_ind_ex( &(bearer_context_p->phys_link),
                              PS_EIC_QOS_NETWORK_L2_LINK_RELEASED );

    tx_wm_ptr = &(bearer_context_p->tx_wm.wm_item);

    if(tx_wm_ptr->current_cnt < tx_wm_ptr->hi_watermark)
    {
      /*---------------------------------------------------------------------
        Enable flow on the phys link.
      ---------------------------------------------------------------------*/
      if(bearer_context_p->ds_bearer_context_dyn_p->tx_hi == FALSE)
      {
        ps_phys_link_enable_flow(&(bearer_context_p->phys_link), 
                             DS_FLOW_LTE_PDCP_MASK|DS_FLOW_UMTS_MASK);
      }
      else
      {
        DS_3GPP_MSG0_HIGH("Rab Rel Hdlr: WM level is between HI and LO, not enabling flow");
      }
    }

    /*-----------------------------------------------------------------------
        Change the state of the bearer context to reflect dormancy.

        However if the bearer is in PENDING RAB REG state, Post bearer up 
        indication so that we end up posting iface up ind. Also there is no
        need to re-orig immediately. This should happen before the bearer
        state is changed.
    -----------------------------------------------------------------------*/

      if(bearer_context_p->ds_bearer_context_dyn_p->state == 
         DS_BEARER_CONTEXT_STATE_PEND_RAB_REG)
      {
        /*-------------------------------------------------------------------
          Post bearer up indication to PDN context.
        -------------------------------------------------------------------*/
        bearer_context_p->ds_bearer_context_dyn_p->pdn_context_cb_tbl_ptr->bearer_up_ind_cb(\
                                 bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr,
                                 bearer_context_p, NULL);

        /*-------------------------------------------------------------------
          Change the state of the bearer context to reflect dormancy.
        -------------------------------------------------------------------*/
        bearer_context_p->ds_bearer_context_dyn_p->state = 
          DS_BEARER_CONTEXT_STATE_UP_DORMANT;
      }
      else
      {
        /*-------------------------------------------------------------------
         The Bearer is being released from a state other than PEND_RAB_REG

         If data is sitting in the Watermarks (PS to PDCP), trigger a rab 
         re-establishment immly. We could have lost Radio Link and
         transitioned to dormancy or the network explicily put us dormant
         when we had data to send. Just call the each enqueue function that
         would trigger rab re-estab 
        -------------------------------------------------------------------*/

        /*-------------------------------------------------------------------
          Change the state of the bearer context to reflect dormancy before 
          calling reorig_dorm. reorig_dorm checks if the bearer is in dormant
        -------------------------------------------------------------------*/
        bearer_context_p->ds_bearer_context_dyn_p->state
          = DS_BEARER_CONTEXT_STATE_UP_DORMANT;
        ds_bearer_cntxt_reorig_from_dorm_on_pend_data_in_tx_wm(bearer_context_p);
      } // if bearer in PEND_RAB_REG state
    retVal = DS3G_SUCCESS;
  }
    else
    {
      /*---------------------------------------------------------------------
        Even if a RAB is received in DORMANT state check for any pending data
        and trigger re origination
      ---------------------------------------------------------------------*/
      DS_3GPP_MSG0_HIGH("RAB release called while in Dormant state. "
                        "Ignoring event");

      ds_bearer_cntxt_reorig_from_dorm_on_pend_data_in_tx_wm(bearer_context_p);
      retVal = DS3G_SUCCESS;
    } /* Bearer state is not in UP DORMANT*/

    DS_BEARER_CTXT_VF_CALL_NO_ARG_PER_BEARER
    (
      rab_rel_ind, 
      bearer_context_p    
    );

    /*-----------------------------------------------------------------------
      If no other bearer is active, It means this is the last bearer to go
      down. Hence send PS_CALL_INACTIVE_IND to DS3G flowmgr.
      This check is added to avoid sending duplicate indications to
      DS3Gflowmgr
    -----------------------------------------------------------------------*/
    if( !ds_bearer_cntx_is_any_other_bearer_per_sub_active(bearer_context_p) )
  {
      DS_3GPP_MSG0_MED("PS_DATA_CALL_INACTIVE_IND");
      ds3gflowmgr_notify_ps_call_state_per_sub(subs_id,
                                               DS3GFLOWMGR_PS_CALL_DORMANT_EV);
  }
  
  } /* bearer context pointer is non-null*/
  
  /*  Stop Fast Dormancy if this bearer is the last Active bearer */ 
  if((ds_3gpp_bearer_cntxt_is_modem_fd_enabled() == TRUE)&&
     (network_mode != SYS_SYS_MODE_LTE)&&
     (ds_bearer_cntx_is_any_other_bearer_up(bearer_context_p) == FALSE))
  {  
    DS_3GPP_MSG0_HIGH("Last bearer Put into Dormant state, Ind to stop FD");
    report_p = (ds_3gpp_fd_info_s*) 
              modem_mem_alloc(sizeof(ds_3gpp_fd_info_s),MODEM_MEM_CLIENT_DATA);

    if (0 > ps_sys_conf_set_ex(PS_SYS_TECH_3GPP,
                               PS_SYS_CONF_STOP_FAST_DORMANCY,
    (ps_sys_subscription_enum_type)ds3gsubsmgr_subs_id_cm_to_ds(subs_id),
                               (void*)report_p,
                               &dss_errno))
    {
      DS_3GPP_MSG0_HIGH("Sending Stop fast dormancy Ind Failed");
    }

    if (report_p != NULL)
    {
      modem_mem_free(report_p, MODEM_MEM_CLIENT_DATA);
    }
  }
  return retVal;
} /* ds_bearer_cntxt_call_rab_release_hdlr */

/*===========================================================================
FUNCTION ds_bearer_cntxt_CALL_RAB_RE_ESTAB_REJ_HDLR

DESCRIPTION
  This function handles rab reject indication for 3GPP modes
  
PARAMETERS  
  call_id     -   CM call id
  call_info   -   mode specific call info
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE if success, FALSE otherwise.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_bearer_cntxt_call_rab_re_estab_rej_hdlr
(
  cm_call_id_type           call_id,
  dsgwl_ps_call_info_u_type *call_info
)
{
  ds_bearer_context_s*                bearer_context_p; 
                                          /* bearer context pointer */

  uint32                              ran_num;   
                                         /* Temp. var to store the 31-bit  */
                                         /* random binary frac bet 0 & 1   */
  ps_extended_info_code_enum_type ps_eic =  
                                        PS_EIC_QOS_NETWORK_L2_LINK_REESTAB_REJ;
                                         /* cause code to higher layer */

  boolean               is_throttle_needed = FALSE;

  void                  *sm_ptr = NULL;

  ds_pdn_context_s      *pdn_cntx_p = NULL;
  ds_bearer_cntxt_per_subs_info_type  *per_subs_info_hndl = NULL;
#ifdef FEATURE_DATA_LTE
  emm_failure_type     emm_cause; /* EMM detailed failure cause */
#endif /* FEATURE_DATA_LTE */
  sys_modem_as_id_e_type subs_id;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Get the bearer context from call id
  -------------------------------------------------------------------------*/
  bearer_context_p =  ds_bearer_cntxt_get_bearer_from_call_id(call_id);

  if (!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    DS_3GPP_MSG1_ERROR("RAB reestab rej hdlr. Call id:%d. Could not locate bearer",\
		    call_id);
    return DS3G_FAILURE;
  }

  /*-------------------------------------------------------------------------
    Get the PDN cntx assigned to this bearer cntx
  -------------------------------------------------------------------------*/
  pdn_cntx_p = (ds_pdn_context_s*)
                   ds_bearer_cntxt_get_pdn_cntxt(bearer_context_p);

  DS_3GPP_MSG3_HIGH("RAB reestab reject call id:%d bearer instance:%d",\
		   call_id,(uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr,0);

  if (FALSE == ds_3gpp_pdn_cntxt_get_subs_id_from_pdn( pdn_cntx_p, &subs_id))
  {
    return DS3G_FAILURE;
  }

  per_subs_info_hndl = 
     ds_bearer_cntxt_get_per_subs_info_hndl(subs_id);

  if (!ds_bearer_cntxt_validate_per_subs_info_hndl
        (per_subs_info_hndl))
  {
    DS_3GPP_MSG1_ERROR("Bearer Cntxt per subs info handle is invalid for"
                       "subs id",subs_id);
    return DS3G_FAILURE;
  }

  /*-------------------------------------------------------------------------
    Reset the rab_reject local cause variable to FALSE explicitly. 
    It would be set when NAS provides local cause for rab_reject
  --------------------------------------------------------------------------*/
  DS_3GPP_MSG0_MED("Resetting rab_reject_local_cause");

  per_subs_info_hndl->rab_reject_local_cause = FALSE;  
  /*-------------------------------------------------------------------------
   * Reset the service_request_pending_flag if the response if obtained as a
   * result of trying to reorig using this bearer
  -------------------------------------------------------------------------*/
  if(bearer_context_p->ds_bearer_context_dyn_p->srv_rsp_exp == TRUE)
  {
    per_subs_info_hndl->service_request_pending_flag = FALSE;
    bearer_context_p->ds_bearer_context_dyn_p->srv_rsp_exp = FALSE;

    /*-------------------------------------------------------------------------
     * Perform throttling if the PDN is not an emergency PDN 
    -------------------------------------------------------------------------*/
    if(!DS_3GPP_PDN_SUPPORTS_EMERGENCY_SERVICES(pdn_cntx_p))
    {
      is_throttle_needed = 
        ds_3gpp_bearer_cntxt_examine_if_throt_needed(call_info, subs_id);
    }

    if(is_throttle_needed == TRUE)
    {
      /*----------------------------------------------------------------------
        Get a handle to the system wide throttling object. This is an opaque
        handle.
      ----------------------------------------------------------------------*/
      sm_ptr = ds_3gpp_throt_sm_get_throttling_sm(TRUE, subs_id);

      /*----------------------------------------------------------------------
        Now feed the failure into the system wide throttling state machine.
        Get the timer value to use from the sm

        Call this function with argument TRUE meaning we need to create a sm
        if it does not already exists. 
      ----------------------------------------------------------------------*/
      ds_3gpp_throt_sm_feed_failure(sm_ptr, subs_id, pdn_cntx_p);
    }


  } /* Service request expected on this bearer */
  else
  {
    /*-----------------------------------------------------------------------
      We do not need to handle rab_reestab_rej if serv_req is not pending .  
      Alt: skip handling rab_reestab_rej if bearer state = UP_DORMANT_REESTAB                                                                                                                                         
    -----------------------------------------------------------------------*/
    if(per_subs_info_hndl->service_request_pending_flag == FALSE) 
    {
      /*---------------------------------------------------------------------
        Handling race condition scenario:         
        MH sends REESTAB_REQ while REESTAB_IND is in cmd Q
        MH processes the REESTAB_IND
        ESM sends RAB_REESTAB_REJ since the DRB is already setup.       
      ---------------------------------------------------------------------*/
      DS_3GPP_MSG0_HIGH("ignoring RAB_REESTAB_REJ, service_request_pending_flag is FALSE");
      return DS3G_SUCCESS;
    }
  }

  /*-------------------------------------------------------------------------
    Reset the state of the bearer context to be DORMANT 
  -------------------------------------------------------------------------*/
  bearer_context_p->ds_bearer_context_dyn_p->state = 
    DS_BEARER_CONTEXT_STATE_UP_DORMANT;
   DS_3GPP_MSG0_HIGH("Setting each_enqueue_func_ptr in rab_re_estab_rej_hdlr");
  (bearer_context_p->tx_wm.wm_item).each_enqueue_func_ptr =
               (wm_cb_type)ds_bearer_cntxt_reorig_dorm_call_cb;
  (bearer_context_p->tx_wm.wm_item).each_enqueue_func_data =  
    (void*) bearer_context_p;

  /*-------------------------------------------------------------------------    
    Get the cause from CM struct depending on the sys mode
    VRK:TODO Move to mode specific
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_LTE
  if(bearer_context_p->ds_bearer_context_dyn_p->call_mode == SYS_SYS_MODE_LTE)
  {
    if(call_info->lte_call.esm_cause.valid)
    {
      DS_3GPP_MSG1_HIGH("RAB_REESTAB_REJ esm cause %d", 
               call_info->lte_call.esm_cause.esm_cause);
      if((call_info->lte_call.esm_cause.esm_cause ==
                                          (int)ACCESS_CLASS_DSAC_REJECTION)|| 
        (call_info->lte_call.esm_cause.esm_cause == (int)CONGESTTION))
      {
        ps_eic = PS_EIC_NETWORK_BUSY;       
      }             
    }//Cause is not valid
    else
    {
      DS_3GPP_MSG0_HIGH("RAB_REESTAB_REJ esm cause is not valid ");
    }   
	/*--------------------------------------------------------------------- 
     Check for EMM cause code
     If EMM cause type is LTE_NAS_IRAT_CONN_EST_FAILURE and emm cause is
     LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED with an access barring time
     of 0 ms, DS should do an infinite retry mechanism at an interval of
     100 ms or greater
     ---------------------------------------------------------------------*/
    emm_cause.cause_type = 
        (emm_irat_failure_cause_type)call_info->lte_call.emm_failure_cause.
         cause_type;

    emm_cause.access_barring_rem_time = 
      call_info->lte_call.emm_failure_cause.access_barring_rem_time;

    memscpy((void *)&emm_cause.cause, sizeof(uint16),
            (void *)&(call_info->lte_call.emm_failure_cause.
            detailed_cause),sizeof(uint16));

    if ((emm_cause.cause_type == LTE_NAS_IRAT_CONN_EST_FAILURE) &&
        (emm_cause.cause.est_status == 
         LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED) &&
        (emm_cause.access_barring_rem_time == 
         ACCESS_BARRING_INFINITE_RETRY_TIME))
    {
      DS_3GPP_MSG0_HIGH("ACCESS BARRING RAB Reject, INFINITE RAB RETRY");
      access_barring_rab_retry = TRUE;
  }
  }
  else
#endif /* FEATURE_DATA_LTE */
  if(bearer_context_p->ds_bearer_context_dyn_p->call_mode == SYS_SYS_MODE_GSM)
  {
    DS_3GPP_MSG0_HIGH("RAB_REESTAB_REJ, call_mode is GSM, returning");
    return DS3G_SUCCESS;
  }
  else if(bearer_context_p->ds_bearer_context_dyn_p->call_mode == 
          SYS_SYS_MODE_WCDMA
#ifdef FEATURE_TDSCDMA
           ||
          bearer_context_p->ds_bearer_context_dyn_p->call_mode == 
          SYS_SYS_MODE_TDS
#endif
         )
  {
    DS_3GPP_MSG1_HIGH("RAB_REESTAB_REJ sm cause %d", 
              call_info->gw_ps_call.rab_cause);
    if((call_info->gw_ps_call.rab_cause == ACCESS_CLASS_DSAC_REJECTION)|| 
      (call_info->gw_ps_call.rab_cause == CONGESTTION))
    {
      ps_eic = PS_EIC_NETWORK_BUSY;       
    }      
    else if (call_info->gw_ps_call.rab_cause == RAB_RB_DS_L2_CLEANUP_COLLISION)
    {
      DS_3GPP_MSG0_MED("Setting rab_reject_local_cause to TRUE");
      per_subs_info_hndl->rab_reject_local_cause = TRUE;
    }    
  }
  /*-------------------------------------------------------------------------  
    Send Phy link down ind to PS IFACE.
  -------------------------------------------------------------------------*/
  ps_phys_link_down_ind_ex(&(bearer_context_p->phys_link),
                           ps_eic);  

  if (ds3gpp_get_ps_data_suspend_flag(subs_id) == TRUE) 
  {
    /*-----------------------------------------------------------------------
      As we are suspended, no need to retry rab reestablishment.      
    -----------------------------------------------------------------------*/
    DS_3GPP_MSG0_HIGH("RAB_REESTAB_REJ while ds_3gpp_data_suspend");
    return DS3G_SUCCESS;
  }

  /*-------------------------------------------------------------------------    
    If the bit mask is non-NULL and the timer is not running, then get the 
    timer value from the function. This function return the timer val based
    on the retry/throttle algorithm. In the absense of the algorithm, it
    returns a random number between 100 and 400 ms
  -------------------------------------------------------------------------*/
  if(per_subs_info_hndl->uplink_data_bitmask != 0)
  {
    /*-------------------------------------------------------------------------    
     Rab re-estab retry  timer will be set even if the retry cnt is Zero.
     this is to avoid immediate Rab re-estab req if there is any Up Link data
    ------------------------------------------------------------------------*/
    if((per_subs_info_hndl->timer_running == FALSE) && 
       (per_subs_info_hndl->service_request_pending_flag == FALSE))
    {
      ran_num = ds_3gpp_bearer_cntxt_get_retry_timer_val(is_throttle_needed, 
                                                         subs_id);

      DS_3GPP_MSG1_HIGH("Starting back off timer for %d msec",ran_num);
  
      DS_BEARER_CNTXT_SET_TIMER (per_subs_info_hndl,ran_num)
    }
  }
  else
  {
    if(per_subs_info_hndl->timer_running == TRUE)
    {
      DS_BEARER_CNTXT_CLR_TIMER (per_subs_info_hndl)
    }
  }

  return ( DS3G_SUCCESS );

} /* ds_bearer_cntxt_call_rab_re_estab_rej_hdlr() */

/*===========================================================================
FUNCTION  DS_BEARER_CNTXT_CALL_RAB_RE_ESTAB_FAIL_HDLR

DESCRIPTION
   This function is called by 3G DSManager when the RAB re-establishment
failure event is received.
RABM signalling procedures succeeded but NO RAB was established. Spec
does not have any procedures to recover from this. So go ahead and
start call release now by returning an error: 3G DS manager will initiate
call release when this fn returns an error.

PARAMETERS   
  call_id,        - Call ID 
  *end_params     - CM specific call end parameters
  subs_id         - Subscription Id
  
DEPENDENCIES 
  None.
  
RETURN VALUE 
  DS3G_FAILURE : so that the call is torn down 
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_bearer_cntxt_call_rab_re_estab_fail_hdlr
(
  cm_call_id_type           call_id,
  cm_end_params_s_type      *end_params,
  sys_modem_as_id_e_type     subs_id
)
{
  ds_bearer_context_s*                 bearer_context_p; 
  uint32               ran_num;
  ds_bearer_cntxt_per_subs_info_type  *per_subs_info_hndl = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get the bearer context from the call id
  -------------------------------------------------------------------------*/
  bearer_context_p =  ds_bearer_cntxt_get_bearer_from_call_id(call_id); 

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    DS_3GPP_MSG3_ERROR("RAB reestab fail. Call id:%d. Could not locate bearer",\
		    call_id,0,0);
    return DS3G_FAILURE;
  }

  per_subs_info_hndl = 
    ds_bearer_cntxt_get_per_subs_info_hndl(subs_id);

  if (!ds_bearer_cntxt_validate_per_subs_info_hndl
        (per_subs_info_hndl))
  {
    DS_3GPP_MSG1_ERROR("Bearer Cntxt per subs info handle is invalid for Subs Id(CM)"
                        ,subs_id);
    return DS3G_FAILURE;
  }

  DS_3GPP_MSG2_HIGH("RAB reestab fail call id:%d bearer instance:%d",\
		  call_id,(uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr);

  /*-------------------------------------------------------------------------
   * Reset the service_request_pending_flag if the response if obtained as a 
   * result of trying to reorig using this bearer
  -------------------------------------------------------------------------*/
  if(bearer_context_p->ds_bearer_context_dyn_p->srv_rsp_exp == TRUE)
  {
    per_subs_info_hndl->service_request_pending_flag = FALSE;
    bearer_context_p->ds_bearer_context_dyn_p->srv_rsp_exp = FALSE;
  }

  /*-------------------------------------------------------------------------
   * Unset the bitmask corresponding to the call (bearer) that is to be ended
  -------------------------------------------------------------------------*/
  DS_BEARER_CNTXT_UNSET_BITMASK (per_subs_info_hndl, bearer_context_p);
  
  /*-----------------------------------------------------------------------
    Reset the rab_reestab_retry counter for this bearer.
  -----------------------------------------------------------------------*/
  DS_BEARER_CNTXT_RESET_RAB_REESTAB_RETRY_CNT(bearer_context_p);
 /*----------------------------------------------------------------------- 
  Reset Access Barring RAB retry
  -----------------------------------------------------------------------*/
  ds_bearer_cntxt_reset_access_barring_retry();
  if(per_subs_info_hndl->uplink_data_bitmask == 0)
  {
    if(per_subs_info_hndl->timer_running == TRUE)
    {
      DS_BEARER_CNTXT_CLR_TIMER (per_subs_info_hndl)
    }
  }
  else
  {
    if(per_subs_info_hndl->timer_running == FALSE)
    {
      /*-----------------------------------------------------------------------    
        Set the timer with the random value between 
        DS_BEARER_CNTXT_RAB_REESTABLISH_MIN_RETRY_TIME and 
        DS_BEARER_CNTXT_RAB_REESTABLISH_MIN_RETRY_TIME * 4 to send out RAB
        reestablishment request again. Once the timer expires, the dormant call
        reorig function will be called and RAB reestablishment is tried again.
      -----------------------------------------------------------------------*/
      ran_num = ps_utils_generate_16bit_rand_num_by_range(
                          DS_BEARER_CNTXT_RAB_REESTABLISH_MIN_RETRY_TIME, 
                          DS_BEARER_CNTXT_RAB_REESTABLISH_MIN_RETRY_TIME * 4);
    
      DS_BEARER_CNTXT_SET_TIMER(per_subs_info_hndl,ran_num)
    }
  }
  
  bearer_context_p->ds_bearer_context_dyn_p->state = DS_BEARER_CONTEXT_STATE_UP_DORMANT;
  DS_3GPP_MSG0_HIGH("Setting each_enqueue_func_ptr in rab_re_estab_fail_hdlr");
 (bearer_context_p->tx_wm.wm_item).each_enqueue_func_ptr =
               (wm_cb_type)ds_bearer_cntxt_reorig_dorm_call_cb;
 (bearer_context_p->tx_wm.wm_item).each_enqueue_func_data = 
   (void*) bearer_context_p;
					                     

  DS_3GPP_MSG0_HIGH("RAB re-estab fail: triger call release");

  /* Call end reason is RAB failure */
  bearer_context_p->ds_bearer_context_dyn_p->down_reason =
     PS_NET_DOWN_REASON_RAB_FAILURE;
  bearer_context_p->ds_bearer_context_dyn_p->down_reason_ps_eic = 
    PS_EIC_NETWORK_RAB_FAILURE;
  
  /*-------------------------------------------------------------------------    
  Fill in the call end parameters : DS manager uses passes this info to cm
  for a call release. For now FAIL indication is given only in WCDMA. Hence
  we are filling the info type with code specific to GW.
  -------------------------------------------------------------------------*/
  end_params->call_id   = call_id;
  end_params->info_type = CM_CALL_MODE_INFO_GW_PS;
  
  return DS3G_FAILURE;

} /* dsWcdmaBearerCntxt_call_rab_re_estab_fail_hdlr() */
/*===========================================================================
FUNCTION  DS_3GPP_CC_SP_THROTTLING

DESCRIPTION
  This function is used for cause code specific throttling for UMTS 

PARAMETERS
rejected_pdn_cntx_p     - pointer to pdn_cntxt that got rejected
pdn_throt_sm_ptr        - pointer to throttle sm
current_bearer_mode     - current mode of the bearer
down_reason             - pdn reject cause code given by the network

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void ds_bearer_cntxt_cc_sp_throttling
(
  ds_pdn_context_s                 *rejected_pdn_cntx_p,
  void                             *pdn_throt_sm_ptr,
  sys_sys_mode_e_type               current_bearer_mode,
  ps_iface_net_down_reason_type     down_reason
)
{
  ds_umts_pdp_type_enum_type	  pdn_pdp_type = DS_UMTS_PDP_MAX;

  /*--------------------------------------------------------------------------*/
  DS_3GPP_MSG0_HIGH("in ds_bearer_cntxt_cc_sp_throttling");
  if (!ds_3gpp_pdn_cntx_validate_pdn_context(rejected_pdn_cntx_p))
  {
   return;
  }
  
  pdn_pdp_type =
     rejected_pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.pdp_type;
  
  if((pdn_pdp_type != DS_UMTS_PDP_IPV4) && 
     (pdn_pdp_type != DS_UMTS_PDP_IPV4V6) &&
     (pdn_pdp_type != DS_UMTS_PDP_IPV6) )
  {
   DS_3GPP_MSG0_ERROR("invalid pdn PDP type, returning");
   return;
  }
  if( (current_bearer_mode == SYS_SYS_MODE_WCDMA) ||
      (current_bearer_mode == SYS_SYS_MODE_GSM)	 ||
      (current_bearer_mode == SYS_SYS_MODE_TDS) )
  {		

	 /*---------------------------------------------------------------------
           Perform regular throttling on the rejected PDP type -- IPV4V6
           -----------------------------------------------------------------------*/
        DS_3GPP_MSG0_HIGH("Perform regular throt on both IP types");

        ds_3gpp_pdn_throt_perform_reg_throttling(pdn_throt_sm_ptr,
                                                 rejected_pdn_cntx_p,
                                              (void *)down_reason,
                                              DS_UMTS_PDP_IPV4V6,
                                              DS_3GPP_THROT_RAT_GLOBAL);

        ds_pdn_cntxt_recompute_ip_support_for_apn(rejected_pdn_cntx_p);
  }
}/*ds_bearer_cntxt_cc_sp_throttling*/
/*===========================================================================
FUNCTION  DS_3GPP_BEARER_CNTXT_GLOBAL_THROTTLING

DESCRIPTION
  This function is used for global throttling for UMTS as well as LTE  

PARAMETERS
rejected_pdn_cntx_p     - pointer to pdn_cntxt that got rejected
pdn_throt_sm_ptr        - pointer to throttle sm
current_bearer_mode     - current mode of the bearer
down_reason             - pdn reject cause code given by the network

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_bearer_cntxt_global_throttling
(
  ds_pdn_context_s              *rejected_pdn_cntx_p,
  void                          *pdn_throt_sm_ptr,
  sys_sys_mode_e_type            current_bearer_mode,
  ps_iface_net_down_reason_type  down_reason
)
{
  ds_umts_pdp_type_enum_type       pdn_pdp_type = DS_UMTS_PDP_MAX;
  ds_dsd_apm_ind_pay_load          ind_payload;
  sys_modem_as_id_e_type        subs_id = SYS_MODEM_AS_ID_NONE;
  ds_3gpp_throt_rat_e_type         rat_type = DS_3GPP_THROT_RAT_GLOBAL;
                                   /* RAT Type Indicates the RAT on which
                                      Throttling is to be performed */
  /*--------------------------------------------------------------------------*/

  memset(&ind_payload, 0, sizeof(ds_dsd_apm_ind_pay_load));

  if (!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(rejected_pdn_cntx_p, &subs_id))
  {
    return;
  }
 
  pdn_pdp_type =
      rejected_pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.pdp_type;

  if( (pdn_pdp_type != DS_UMTS_PDP_IPV4) && 
      (pdn_pdp_type != DS_UMTS_PDP_IPV4V6) &&
      (pdn_pdp_type != DS_UMTS_PDP_IPV6) )
  {
    DS_3GPP_MSG0_ERROR("invalid pdn PDP type, returning");
    return;
  }

  if (ds_3gpp_nv_manager_get_enable_pdn_throt_per_rat(subs_id))
  {
    rat_type = ds_3gpp_throt_map_cm_sys_mode_to_rat_type(current_bearer_mode);  
  }

  if(current_bearer_mode == SYS_SYS_MODE_LTE)
  { 
    switch(down_reason)
    {
      case PS_NET_DOWN_REASON_IP_V4_ONLY_ALLOWED:
       /*---------------------------------------------------------------------
         Perform regular throttling on the rejected PDP type -- IPV6
         Recompute the IP support for the APN. Update APN table if necessary
         Save the overridden network type
       ---------------------------------------------------------------------*/
         DS_3GPP_MSG1_HIGH("Perform regular throt on the rejected IP type:%d",
                            DS_UMTS_PDP_TYPE_IPV6);
   

         ds_3gpp_pdn_throt_perform_reg_throttling
         (
           pdn_throt_sm_ptr,
           rejected_pdn_cntx_p,
           (void *)down_reason,
           DS_UMTS_PDP_IPV6,
           rat_type
         );
     
         ds_pdn_cntxt_recompute_ip_support_for_apn(rejected_pdn_cntx_p);
     
         DS_3GPP_APM_SET_INVALID_V6_IPADDR(
                                        ind_payload, 
                                        PS_NET_DOWN_REASON_IP_V4_ONLY_ALLOWED);
     
         ds_3gpp_network_override_hdlr_set_overridden_ip(
             rejected_pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.apn, 
             PDP_IPV4, subs_id);
 
         break;
 
       case PS_NET_DOWN_REASON_IP_V6_ONLY_ALLOWED:
       /* ---------------------------------------------------------------------
         Perform regular throttling on the rejected PDP type -- IPV4
         Recompute the IP support for the APN. Update APN table if necessary
       -----------------------------------------------------------------------*/
         DS_3GPP_MSG1_HIGH("Perform regular throt on the rejected IP type:%d",
                            DS_UMTS_PDP_TYPE_IPV4);
         
         ds_3gpp_pdn_throt_perform_reg_throttling(pdn_throt_sm_ptr,
                                                  rejected_pdn_cntx_p,
                                                  (void *)down_reason,
                                                  DS_UMTS_PDP_IPV4,
                                                  rat_type);
          
         ds_pdn_cntxt_recompute_ip_support_for_apn(rejected_pdn_cntx_p);
         
         DS_3GPP_APM_SET_INVALID_V4_IPADDR(
                                         ind_payload, 
                                         PS_NET_DOWN_REASON_IP_V6_ONLY_ALLOWED);
         
         ds_3gpp_network_override_hdlr_set_overridden_ip(
             rejected_pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.apn, 
             PDP_IPV6, subs_id);
         break;
 
       default: 
         ds_3gpp_pdn_throt_perform_reg_throttling(
                                             pdn_throt_sm_ptr,
                                             rejected_pdn_cntx_p,
                                             (void *)down_reason,
                                             pdn_pdp_type,
                                             rat_type);
     }

     if( (TRUE == ds_pdn_cntxt_is_default(rejected_pdn_cntx_p)) &&
         (ind_payload.ind_field_mask != 0))
     {
       DS_3GPP_APM_SET_PROFILE_ID(ind_payload, 
          (uint16)rejected_pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile_num);
       ind_payload.subs_id = subs_id;
       (void)ds_dsd_apm_ind(DS_DSD_APM_ATTACH_REJECT_IND, &ind_payload);
     }
     
  }
  else if( (current_bearer_mode == SYS_SYS_MODE_WCDMA) ||
           (current_bearer_mode == SYS_SYS_MODE_GSM)   ||
           (current_bearer_mode == SYS_SYS_MODE_TDS) )
  {
      
    switch(down_reason)
    {
      case PS_NET_DOWN_REASON_OPERATOR_DETERMINED_BARRING: /*  8*/
      case PS_NET_DOWN_REASON_UNKNOWN_PDP:                 /* 28*/
      case PS_NET_DOWN_REASON_AUTH_FAILED:                 /* 29*/
      case PS_NET_DOWN_REASON_OPTION_NOT_SUPPORTED:        /* 32*/
      case PS_NET_DOWN_REASON_OPTION_UNSUBSCRIBED:         /* 33*/
      case PS_NET_DOWN_REASON_NSAPI_ALREADY_USED:          /* 35*/
      case PS_NET_DOWN_REASON_PROTOCOL_ERROR:              /*111*/
        ds_3gpp_pdn_throt_handle_perm_conn_failure(pdn_throt_sm_ptr, 
			                           (void *)down_reason,
                                                   DS_UMTS_PDP_IPV4V6,
                                                   subs_id,
                                                   rejected_pdn_cntx_p);
        break;

      case PS_NET_DOWN_REASON_IP_V4_ONLY_ALLOWED:          /* 50*/
      /*---------------------------------------------------------------------
        Perform regular throttling on the rejected PDP type -- IPV6
        Recompute the IP support for the APN. Update APN table if necessary
        Save the overridden network type
      -----------------------------------------------------------------------*/
        DS_3GPP_MSG1_HIGH("Perform regular throt on the rejected IP type:%d",
                           DS_UMTS_PDP_TYPE_IPV6);

        ds_3gpp_pdn_throt_perform_reg_throttling(pdn_throt_sm_ptr,
                                                 rejected_pdn_cntx_p,
                                                 (void *)down_reason,
                                                 DS_UMTS_PDP_IPV6,
                                                 rat_type);

        ds_pdn_cntxt_recompute_ip_support_for_apn(rejected_pdn_cntx_p);

        ds_3gpp_network_override_hdlr_set_overridden_ip(
             rejected_pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.apn, 
             PDP_IPV4, subs_id);
 
        break;

      case PS_NET_DOWN_REASON_IP_V6_ONLY_ALLOWED:          /* 51*/
      /*---------------------------------------------------------------------
        Perform regular throttling on the rejected PDP type -- IPV4
        Recompute the IP support for the APN. Update APN table if necessary
      ------------------------------------------------------------------- --*/
        DS_3GPP_MSG1_HIGH("Perform regular throt on the rejected IP type:%d",
                           DS_UMTS_PDP_TYPE_IPV4);

        ds_3gpp_pdn_throt_perform_reg_throttling(pdn_throt_sm_ptr,
                                                 rejected_pdn_cntx_p,
                                                 (void *)down_reason,
                                                 DS_UMTS_PDP_IPV4,
                                                 rat_type);

        ds_pdn_cntxt_recompute_ip_support_for_apn(rejected_pdn_cntx_p);

        ds_3gpp_network_override_hdlr_set_overridden_ip(
            rejected_pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.apn, 
            PDP_IPV6, subs_id);
        break;

      default:
        DS_3GPP_MSG1_HIGH("No specific throtlling mechanism for the given"
                          "cause code ps_net_down_reason:%d so performing"
                          "regular throttling", down_reason);
          
        ds_3gpp_pdn_throt_perform_reg_throttling(pdn_throt_sm_ptr,
                                                 rejected_pdn_cntx_p,
                                                 (void *)down_reason,
                                                 pdn_pdp_type,
                                                 rat_type);
    }
  }
  else
  {
    DS_3GPP_MSG1_ERROR("Invalid bearer mode %d", current_bearer_mode);
  }
} /*ds_3gpp_bearer_cntxt_global_throttling*/

/*===========================================================================
FUNCTION  DS_BEARER_CNTXT_UPDATE_PHYS_LINK_FLOW_STATUS

DESCRIPTION
  This function enables/disables phys link flows for all active PDP contexts
  This function is called during InterRAT scenarios.  

PARAMETERS
  boolean is_enabled.
  subs_id Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_bearer_cntxt_update_phys_link_flow_status
(
  boolean                is_enabled,
  sys_modem_as_id_e_type subs_id
)
{
  ds_bearer_context_s* bearer_context_p; //Pointer to bearer context
  uint32               bearer_cntx_inst;
  /*-----------------------------------------------------------------------*/

  DS_3GPP_MSG1_HIGH("ds_bearer_cntxt_all_phys_link_flow_control,flow state: %d",
           is_enabled);
  /*-------------------------------------------------------------------------
    For all active bearer contexts, disable phys link flow
  -------------------------------------------------------------------------*/
  for( bearer_cntx_inst=0;\
     bearer_cntx_inst<DS_3GPP_MAX_BEARER_CONTEXT;\
     bearer_cntx_inst++ )
  {
    bearer_context_p = ds_bearer_cntxt_get_instance_ptr(bearer_cntx_inst);    
            
    if(!ds_bearer_cntxt_validate_bearer_subscription
         (bearer_context_p, subs_id))
    {
      continue;
    }
            
    if (bearer_context_p->ds_bearer_context_dyn_p->state == 
        DS_BEARER_CONTEXT_STATE_UP ||
        bearer_context_p->ds_bearer_context_dyn_p->state == 
        DS_BEARER_CONTEXT_STATE_UP_DORMANT ||
        bearer_context_p->ds_bearer_context_dyn_p->state == 
        DS_BEARER_CONTEXT_STATE_UP_DORMANT_REESTAB )
    {

   
      DS_3GPP_MSG2_HIGH("bearer_cntx_p 0x%x, flow state: %d",
               bearer_context_p,is_enabled);

      if(is_enabled)
      {
        /*-------------------------------------------------------------------
          Enable flow on the phys link.
        -------------------------------------------------------------------*/
        ps_phys_link_enable_flow( 
                         &(bearer_context_p->phys_link),
                         DS_FLOW_UMTS_MASK);

      }
      else
      {
        /*-------------------------------------------------------------------
          Disable flow on the phys link.
        -------------------------------------------------------------------*/
        ps_phys_link_disable_flow( 
                         &(bearer_context_p->phys_link),
                         DS_FLOW_UMTS_MASK);
      }      

    }// valid for bearer state
    
  }// end for all bearer_contexts    

}/*ds_bearer_cntxt_update_phys_link_flow_status*/


/*===========================================================================
FUNCTION DSBEARERCNTXT_IS_PS_DATA_AVAILABLE

DESCRIPTION
   This function returns TRUE if data is available in any of the uplink
   buffers, otherwise, it returns FALSE.DS will check if there is UL data
  in the WM only if the bearer is in Dormant state Set Active_Flag to True;
  if there is data ;else False. 


PARAMETERS
  subs_id: Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If Data is available
  FALSE - If Data is not available

SIDE EFFECTS
  None.

===========================================================================*/
boolean dsBearerCntxt_is_ps_data_available
(
  sys_modem_as_id_e_type subs_id
)
{
  ds_bearer_context_s *bearer_context_p;
  uint32              bearer_cntx_inst;
  ps_phys_link_type   *phys_link_ptr = NULL;  
  /*-----------------------------------------------------------------------*/  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    VRK:TODO INTER-RAT Do we need this check?
    If there is a call in Coming or or going down state, return true so that 
    we don't loose PS signalling connection after RAU.
  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  DS_3GPP_MSG0_HIGH("Checking for Uplink Data on DS UM watermark");
  /*--------------------------------------------------------------------------
    For each bearer in the bearer context table, check the UL WM.
  --------------------------------------------------------------------------*/

  for( bearer_cntx_inst=0;\
       bearer_cntx_inst<DS_3GPP_MAX_BEARER_CONTEXT;\
       bearer_cntx_inst++ )
  {
    bearer_context_p = ds_bearer_cntxt_get_instance_ptr(bearer_cntx_inst);
    ds3gpp_enter_global_crit_section();

    if(!ds_bearer_cntx_validate_bearer_subscription_internal
         (bearer_context_p, subs_id))
    {
      ds3gpp_leave_global_crit_section(); 
      continue;
    }

    if((bearer_context_p->ds_bearer_context_dyn_p->state == 
        DS_BEARER_CONTEXT_STATE_PEND_REDIAL) ||
      (bearer_context_p->ds_bearer_context_dyn_p->state == 
       DS_BEARER_CONTEXT_STATE_COMING_UP))
    {
      DS_3GPP_MSG1_HIGH("Bearer_inst:%d Pending Redial, ps_data_available",
                         bearer_cntx_inst);
      ds3gpp_leave_global_crit_section();
      return TRUE;
    }

    phys_link_ptr = &(bearer_context_p->phys_link);

    if (PS_PHYS_LINK_GET_STATE(phys_link_ptr) == PHYS_LINK_RESUMING)
    {
      DS_3GPP_MSG2_HIGH("Phys link: 0x%x is in RESUMING state, bearer_inst: %d",
                         phys_link_ptr,
  	                 bearer_context_p->ds_bearer_context_dyn_p->index);
      ds3gpp_leave_global_crit_section();
      return TRUE;
    }    

    DS_3GPP_MSG3_HIGH("Bearer_inst:%d, state: %d, ul_reg_with_pdcp: %d",
		       bearer_cntx_inst, 
		       bearer_context_p->ds_bearer_context_dyn_p->state,
		       bearer_context_p->ds_bearer_context_dyn_p->\
                                            ul_registered_with_ltepdcp);

   /*If all the bearers are in Up state ; do not check the WM always
      set Active_Flag to False.This is because; Active Flag in TAU will
        be used to reestabilish the bearers in dormant state.*/
    if(!bearer_context_p->ds_bearer_context_dyn_p->ul_registered_with_ltepdcp 
       && ( (bearer_context_p->ds_bearer_context_dyn_p->state == 
        DS_BEARER_CONTEXT_STATE_UP_DORMANT_REESTAB) ||
       (bearer_context_p->ds_bearer_context_dyn_p->state == 
        DS_BEARER_CONTEXT_STATE_UP_DORMANT) ||
       (bearer_context_p->ds_bearer_context_dyn_p->state == 
        DS_BEARER_CONTEXT_STATE_UP)))
    {
    
      DS_3GPP_MSG2_HIGH("Bearer_inst:%d UM watermark cnt:%d",
                         bearer_cntx_inst,
                         bearer_context_p->tx_wm.wm_item.current_cnt);
      if( bearer_context_p->tx_wm.wm_item.current_cnt > 0 )
      {
        DS_3GPP_MSG0_HIGH("Uplink Data on DS UM watermark");
        ds3gpp_leave_global_crit_section();
        return TRUE;
      }
      if((bearer_context_p->tx_wm.wm_item)
         .each_enqueue_func_ptr == NULL)
      {
        DS_3GPP_MSG0_HIGH("tx_wm each_enqueue is set to NULL");
      }
      else
      {
        DS_3GPP_MSG0_ERROR("tx_wm each_enqueue is nonNULL");
      }
      
     
    } /*  UP_DORMANT_REESTAB || UP_DORMANT || UP*/
	
    ds3gpp_leave_global_crit_section();
  }/* for all bearer contexts */

  return FALSE;
} /* dsBearerCntxt_is_ps_data_available */



/*===========================================================================
FUNCTION  DS_BEARER_CNTXT_PS_DATA_FAIL_IND_HDLR

DESCRIPTION
  This function enables phys link flows for all active PDP contexts, if we 
  are allowed to send data_available_ind based on the srv status.
  This function is called during InterRAT scenarios.  

PARAMETERS
  boolean allowed_to_send_data_avail.
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_bearer_cntxt_ps_data_fail_ind_hdlr
(
  boolean                allowed_to_send_data_avail,
  sys_modem_as_id_e_type subs_id
)
{
  ds_bearer_context_s* bearer_context_p; //Pointer to bearer context
  uint32               bearer_cntx_inst;
  phys_link_state_type phys_link_state = PHYS_LINK_NULL;
  /*-----------------------------------------------------------------------*/

  DS_3GPP_MSG0_HIGH("ds_bearer_cntxt_ps_data_fail_ind_hdlr ");
  /*-------------------------------------------------------------------------
    For all active bearer contexts, disable phys link flow
  -------------------------------------------------------------------------*/
  for( bearer_cntx_inst=0;\
     bearer_cntx_inst<DS_3GPP_MAX_BEARER_CONTEXT;\
     bearer_cntx_inst++ )
  {

    bearer_context_p = ds_bearer_cntxt_get_instance_ptr(bearer_cntx_inst);

    if(!ds_bearer_cntx_validate_bearer_subscription_internal
        (bearer_context_p, subs_id))
    {
      continue;
    }

    if (bearer_context_p->ds_bearer_context_dyn_p->state == 
        DS_BEARER_CONTEXT_STATE_UP ||
        bearer_context_p->ds_bearer_context_dyn_p->state == 
        DS_BEARER_CONTEXT_STATE_UP_DORMANT ||
        bearer_context_p->ds_bearer_context_dyn_p->state == 
        DS_BEARER_CONTEXT_STATE_UP_DORMANT_REESTAB )
    {    
      if(!(allowed_to_send_data_avail))
      {      
        /*-------------------------------------------------------------------
          Enable flow on the phys link.
          ps_data_available/ps_data_fail_ind will only enable/disable flow
        -------------------------------------------------------------------*/
        
        DS_3GPP_MSG1_HIGH("bearer_cntx_p 0x%x, disabling flow",bearer_context_p);

        phys_link_state = PS_PHYS_LINK_GET_STATE(&(bearer_context_p->phys_link));

        /*-------------------------------------------------------------------
          Flow control is ineffective in transient states. So we need to move
          phys link state to down before enabling the flow
        -------------------------------------------------------------------*/
        /*---------------------------------------------------------------------  
          Check phys link state and post phys link up. After getting resume, we 
          cannot blindly post flow_enable() as it will not take effect if phys
          link is in transition states.
        ---------------------------------------------------------------------*/  
        if (!( ( PHYS_LINK_UP   == phys_link_state) || 
               ( PHYS_LINK_DOWN == phys_link_state) ) )
        {
          ps_phys_link_down_ind_ex( &(bearer_context_p->phys_link),
                                     PS_EIC_QOS_NETWORK_SVC_NOT_AVAILABLE);
        }
        ps_phys_link_disable_flow(&(bearer_context_p->phys_link),
                                 DS_FLOW_UMTS_MASK);
      }
    }// for UP, UP_DORMANT, UP_DORMANT_REESTAB bearers
    
  }// end for all bearer_contexts    

}/*ds_bearer_cntxt_ps_data_fail_ind_hdlr*/

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_SYS_CHANGE_HANDLER

DESCRIPTION
   This function updates the DSMGR call table with the new sys mode

PARAMETERS
   new_mode
   subscription id

DEPENDENCIES
  None.

RETURN VALUE
  NONE

SIDE EFFECTS
  None.

===========================================================================*/
void ds_bearer_cntxt_sys_change_handler
(
  sys_sys_mode_e_type    new_mode,
  sys_modem_as_id_e_type subs_id
)
{
  ds_bearer_context_s *bearer_context_p;
  uint32              bearer_cntx_inst;
  /*-----------------------------------------------------------------------*/    

  /*--------------------------------------------------------------------------
    For each bearer in the bearer context table, update the nw mode
  --------------------------------------------------------------------------*/
  for( bearer_cntx_inst=0;\
       bearer_cntx_inst<DS_3GPP_MAX_BEARER_CONTEXT;\
       bearer_cntx_inst++ )
  {
    bearer_context_p = ds_bearer_cntxt_get_instance_ptr(bearer_cntx_inst);

    if(!ds_bearer_cntxt_validate_bearer_subscription(bearer_context_p, subs_id))
    {
      continue;
    }

    /*---------------------------------------------------------------------
      If moving from L <-> G/W, do not update the call_mode for any active
      or redial pending call, call mode will be updated later
    ---------------------------------------------------------------------*/
    if(((new_mode == SYS_SYS_MODE_LTE)||(bearer_context_p->
                     ds_bearer_context_dyn_p->call_mode == SYS_SYS_MODE_LTE)) &&
        (bearer_context_p->ds_bearer_context_dyn_p->state != DS_BEARER_CONTEXT_STATE_DOWN))
    {
      DS_3GPP_MSG3_HIGH("call id:%d call mode not changed, bearer state: %d, call mode: %d",
                        bearer_context_p->ds_bearer_context_dyn_p->cm_call_id,
                        bearer_context_p->ds_bearer_context_dyn_p->state,                        
                        bearer_context_p->ds_bearer_context_dyn_p->call_mode);
      continue;
    }

    /*-------------------------------------------------------------------
      Update the call_mode in bearer context and the DSMGR call table,
      only if the bearer is not in COMING_UP or GOING_DOWN state.     
    -------------------------------------------------------------------*/
    if( bearer_context_p->ds_bearer_context_dyn_p->state != 
        DS_BEARER_CONTEXT_STATE_PEND_REDIAL ) 
    {
      bearer_context_p->ds_bearer_context_dyn_p->call_mode = new_mode; 
      if(bearer_context_p->ds_bearer_context_dyn_p->cm_call_id != CM_CALL_ID_INVALID)
      {   
          
        DS_3GPP_MSG2_HIGH("Changing mode for call id:%d :mode:%d",
                  bearer_context_p->ds_bearer_context_dyn_p->cm_call_id,new_mode);

        if( !ds3g_msh_set_new_call_mode(bearer_context_p->ds_bearer_context_dyn_p
                                        ->cm_call_id,
                new_mode ) )
        {
          DS_3GPP_MSG1_ERROR("Mode set failed for call id :%d", 
                    bearer_context_p->ds_bearer_context_dyn_p->cm_call_id);
        } 
       
      } /* !CM_CALL_ID_INVALID */
    }/* !DS_BEARER_CONTEXT_STATE_PEND_REDIAL */

  }/* for all bearer contexts */

  return;
} /* ds_bearer_cntxt_sys_change_handler */

/*===========================================================================
FUNCTION DS_BEARER_CNTX_ACTIVATE_CNF

DESCRIPTION
  This is the function that is called from 3gpp_hdlr on call connected.
  This in turn invoked GPRS or WCDMA bearer level functions.
  
PARAMETERS  
  bearer_context_p  - bearer context obj ptr
  mode_info_p       - mode info params
  end_params_p      - call end parameters
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  DS3G_SUCCESS if success, DS3G_FAILURE otherwise.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean 
ds_bearer_cntxt_activate_cnf
(
  ds_bearer_context_s             *bearer_context_p,
  const cm_call_mode_info_s_type  *mode_info_p,
  cm_end_params_s_type            *end_parms_p,
  sys_modem_as_id_e_type           subs_id
)
{  boolean rval = DS3G_FAILURE;
   /*------------------------------------------------------------------------*/

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return rval;
  }  

  DS_BEARER_CTXT_VFR_CALL_PER_BEARER
  (
    rval, 
    activateCnf, 
    bearer_context_p, 
    mode_info_p, 
    end_parms_p
  );

  return rval;
}

/*===========================================================================
FUNCTION DS_3GPP_BEARER_CNTXT_GET_DATA_TRFR_ACTIVITY

DESCRIPTION
  This function is used to find out if any data was transferred in the UL
  or DL direction in the previous polling period.

  This function just calls the mode specific function to query data activity
  in the lower layers
  
PARAMETERS  
  bearer_context_p  - bearer context obj ptr
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
   TRUE - if any data was Tx or Rx in the polling interval
   FALSE - otherwise
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_bearer_cntxt_get_data_trfr_activity
(
  ds_bearer_context_s  *bearer_context_p
)
{
  boolean  rval = FALSE;
  sys_modem_as_id_e_type subs_id = SYS_MODEM_AS_ID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds_bearer_cntxt_get_subs_id_from_bearer(bearer_context_p, &subs_id))
  {
    return rval;
  }  

  DS_BEARER_CTXT_VFR_CALL
  (
    rval, 
    get_data_trfr_activity, 
    subs_id,
    bearer_context_p
  );

  return rval;
}

/*===========================================================================
FUNCTION DS_3GPP_BEARER_CNTX_GET_BEARER_TYPE

DESCRIPTION
  This function is used to get the underlying bearer type. This makes a call
  to the mode specific bearer function and populates the incoming param.
  
PARAMETERS  
  bearer_context_p  - bearer context obj ptr
  bearer_type_p     - bearer_type structure that has to be filled in
  ps_errno          - error code to be returned to PS in case of failure.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
   Integer value is returned.
   0 - SUCCESS
  -1 - FAILURE
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
int ds_3gpp_bearer_cntxt_get_bearer_type
(
  ds_bearer_context_s             *bearer_context_p,
  ps_iface_bearer_technology_type *bearer_type_p,                          
  sint15                          *ps_errno
)
{
  int rval = 0;
  sys_sys_mode_e_type    network_mode = SYS_SYS_MODE_NONE;
  ds_pdn_context_s       *pdn_cntx_p = NULL;  
  sys_modem_as_id_e_type  subs_id  = SYS_MODEM_AS_ID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( bearer_type_p == NULL || 
      !ds_bearer_cntx_validate_bearer_context(bearer_context_p) ) 
  {
    DS_3GPP_MSG0_ERROR("bearer_type_p is NULL or "
                       "Bearer Context passed is invalid");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  pdn_cntx_p = (ds_pdn_context_s*)
               (bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr);

  if(! ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p, &subs_id))
  {
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*--------------------------------------------------------------------------
    We are in one of the 3GPP modes of operation
    Configure the current network to be 3GPP. This is just an alias to 
    PS_IFACE_NETWORK_UMTS
  --------------------------------------------------------------------------*/
  bearer_type_p->current_network = PS_IFACE_NETWORK_3GPP;

  /*--------------------------------------------------------------------------
    Initialize the RAT to be NULL. This is usually returned to the upper layer
    if we are "out of service"
  --------------------------------------------------------------------------*/
  bearer_type_p->data.umts_type.rat_mask = PS_IFACE_3GPP_NULL_BEARER;

  /*--------------------------------------------------------------------------
    Under the following conditions, 3GPP IOCTL hdlr should return -1
      * When the mode maintained by DSMGR is a non 3GPP mode
      * When we are not in service.
    Get the network mode and the service status from DSMGR

    If we are in one of the 3GPP modes but OOS, we return NULL BEARER RAT
  --------------------------------------------------------------------------*/
  network_mode = ds_3gpp_get_apn_current_mode (pdn_cntx_p);

  if( DS_3GPP_IS_3GPP_CURRENT_MODE(network_mode))
  {
    if(ds3g_get_current_srv_status_ex(subs_id) == SYS_SRV_STATUS_SRV)
    {
      /*------------------------------------------------------------------------
        We are in service.
        Invoke the mode specific function to fill in the RAT mask
      ------------------------------------------------------------------------*/
      DS_BEARER_CTXT_VFR_CALL
      (
        rval, 
        get_bearer_tech_type, 
        subs_id,
        bearer_type_p, 
        ps_errno,
        subs_id
      );
    }

  }

  return rval;
} /* ds_3gpp_bearer_cntxt_get_bearer_type */

#ifdef FEATURE_DATA_WLAN_MAPCON
/*===========================================================================
FUNCTION DS_3GPP_BEARER_CNTX_GET_BEARER_TYPE_EX

DESCRIPTION
  This function is used to get the underlying bearer type. 
  
PARAMETERS  
  network_mode      - network sys mode
  bearer_type_ex_p  - bearer_type structure that has to be filled in
  ps_errno          - error code to be returned to PS in case of failure
  subs_id           - subscription id
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
   Integer value is returned.
   0 - SUCCESS
  -1 - FAILURE
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
int ds_3gpp_bearer_cntxt_get_bearer_type_ex
(
  sys_sys_mode_e_type              network_mode,
  ds_sys_system_status_info_type  *bearer_type_ex_p,                          
  sint15                          *ps_errno,
  sys_modem_as_id_e_type          subs_id
)
{
  int rval = 0;
  boolean                  is_ps_srv_domain_pref = FALSE;
  sys_srv_domain_e_type    srv_domain = SYS_SRV_DOMAIN_NO_SRV;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if(bearer_type_ex_p == NULL) 
  {
    DS_3GPP_MSG0_ERROR("bearer_type_ex_p = NULL");
    *ps_errno = DS_EFAULT;
    return -1;
  }

 /* Initializing the Variables */
  bearer_type_ex_p->technology = DS_SYS_NETWORK_3GPP;
  bearer_type_ex_p->rat_value = DS_SYS_RAT_EX_NULL_BEARER;
  bearer_type_ex_p->so_mask = DS_SYS_SO_EX_UNSPECIFIED;

  srv_domain = ds3g_get_current_regd_srv_domain_ex(subs_id);

  if(srv_domain == SYS_SRV_DOMAIN_PS_ONLY ||
     srv_domain == SYS_SRV_DOMAIN_CS_PS)
  {
    is_ps_srv_domain_pref = TRUE;
  }

  /*--------------------------------------------------------------------------
    Under the following conditions, 3GPP IOCTL hdlr should return -1
      * When the mode maintained by DSMGR is a non 3GPP mode
      * When we are not in service.
    Get the network mode and the service status from DSMGR

    If we are in one of the 3GPP modes but OOS, we return NULL BEARER RAT
  --------------------------------------------------------------------------*/
  if( (network_mode == SYS_SYS_MODE_GSM) ||
      (network_mode == SYS_SYS_MODE_WCDMA) ||
      (network_mode == SYS_SYS_MODE_LTE) 
#ifdef FEATURE_TDSCDMA
      ||
      (network_mode == SYS_SYS_MODE_TDS)
#endif
     )
  {
    if((ds3g_get_current_srv_status_ex(subs_id) == SYS_SRV_STATUS_SRV) && 
       (is_ps_srv_domain_pref == TRUE))
    {
       DS_BEARER_CTXT_VFR_CALL
      (
        rval, 
        get_bearer_tech_type_ex, 
        subs_id,
        bearer_type_ex_p,
        subs_id
      );   
    }
    else
    {
      if ((network_mode == SYS_SYS_MODE_LTE) && 
          (ds3g_get_current_srv_status_ex(subs_id) == SYS_SRV_STATUS_LIMITED) &&
          (is_ps_srv_domain_pref == TRUE))
      {
        bearer_type_ex_p->rat_value = DS_SYS_RAT_EX_3GPP_LTE;
        bearer_type_ex_p->so_mask = DS_SYS_SO_EX_3GPP_LTE_LIMITED_SRVC;
      }
    }
  }

  DS_3GPP_MSG1_LOW("Network mode: %d ", network_mode);
  
  DS_3GPP_MSG3_LOW("Bearer Tech Ex: Tech = %d, RAT = %d, SO Mask = %d",
                   bearer_type_ex_p->technology,
                   bearer_type_ex_p->rat_value,
                   bearer_type_ex_p->so_mask);
   
  return rval;
} /* ds_3gpp_bearer_cntxt_get_bearer_type_ex */
  
/*===========================================================================
FUNCTION DS_3GPP_BEARER_CNTX_GET_SO_MASK

DESCRIPTION
  This function is used to fetch the SO mask info based on
  capability information such hsdpa_call_status for WCDMA or 
  egprs_support for GSM. 
  
PARAMETERS  
  subs_id: Subscription Id
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  SO mask
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
uint64 ds_3gpp_bearer_cntxt_get_so_mask
(
  sys_modem_as_id_e_type  subs_id
)
{
  uint64 so_mask = DS_SYS_SO_EX_UNSPECIFIED;

  DS_BEARER_CTXT_VFR_CALL
  (
    so_mask,
    getSoMask,
    subs_id,
    subs_id
  );

  return so_mask;
} /* ds_3gpp_bearer_cntxt_get_so_mask */
#endif /* FEATURE_DATA_WLAN_MAPCON */

/*==========================================================================
FUNCTION DS_3GPP_BEARER_CNTXT_GET_BEARER_RATE

DESCRIPTION This function returns the tx and rx rates of current call's
            channel.
    
PARAMETERS
   call_inst        - index into the call table
  *argval_ptr       - Bearer rate info will be put in here
  *ps_errno         - Ptr to error code (if any)
   subs_id          - Subscription Id
  
  
DEPENDENCIES
  None.
  
RETURN VALUE
  0 - SUCCESS
  -1 - FAILURE

SIDE EFFECTS 
  None
===========================================================================*/
int ds_3gpp_bearer_cntxt_get_bearer_rate
(
  uint32                   call_inst,
  void                     *argval_ptr,
  sint15                  *ps_errno,
  sys_modem_as_id_e_type   subs_id
)
{
  int rval = -1;

  DS_BEARER_CTXT_VFR_CALL
  (
    rval, 
    get_bearer_rate, 
    subs_id,
    call_inst,
    argval_ptr, 
    ps_errno,
    subs_id
  );
  
  return rval;
}

/*===========================================================================
FUNCTION DS_3GPP_BEARER_CNTXT_EXAMINE_IF_THROT_NEEDED

DESCRIPTION
  This function is used to examine the cause code of rejection and determine
  if we need to perform throttling. This function in turn dispatches the mode
  specific handler to figure out if throttling is needed

PARAMETERS
  call_info_ptr  - Pointer to the call info to find out rejection cause
  subs_id        - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if throttling is needed
  FALSE otherwise. 
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_bearer_cntxt_examine_if_throt_needed
(
  dsgwl_ps_call_info_u_type *call_info_ptr,
  sys_modem_as_id_e_type     subs_id
)
{
  boolean is_throttle_needed = FALSE;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*--------------------------------------------------------------------------
    Invoke the mode specific function that determines if we have to perform
    throttling.
  --------------------------------------------------------------------------*/
  DS_BEARER_CTXT_VFR_CALL
  (
    is_throttle_needed, 
    examine_if_throt_needed, 
    subs_id,
    call_info_ptr,
    subs_id
  );

  return is_throttle_needed;
} /* ds_3gpp_bearer_cntxt_examine_if_throt_needed */

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_PS_TO_L2_FLOW_ENABLE_CB

DESCRIPTION
  This function sends a flow enable message to ds task
  
PARAMETERS  
  bearer_context_p   - bearer context pointer
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_bearer_cntxt_ps_to_L2_flow_enable_cb
(
 dsm_watermark_type *wm,
 /*lint -esym(715,wm) */ 
 /*lint -esym(818,wm) */ 
 void*              callback_data
)
{
  uint32                               bearer_instance = 0;
  ds_bearer_context_s                 *bearer_context_p = NULL;
  sys_modem_as_id_e_type   subs_id= SYS_MODEM_AS_ID_NONE;
  ds_bearer_cntxt_um_wm_threshold_struct_type      um_wm_threshold;
  ds_bearer_cntxt_tds_um_wm_threshold_struct_type  tds_um_wm_threshold;
  ds_pdn_context_s                                 *pdn_cntx_p = NULL;

  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  bearer_instance = (uint32) callback_data;

  bearer_context_p = ds_bearer_cntxt_get_instance_ptr(bearer_instance);

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }

  pdn_cntx_p = bearer_context_p->ds_bearer_context_dyn_p
                  ->pdn_context_ptr;

  if (!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return;
  }

  if(!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p,&subs_id))
  {
    return;
  }

  um_wm_threshold =  ds_3gpp_nv_get_um_wm_threshold(subs_id);  
  tds_um_wm_threshold =  ds_3gpp_nv_get_tds_wm_threshold(subs_id);  
 

  /* No need to post message if wm is already in lo state 
  */
  if (!bearer_context_p->ds_bearer_context_dyn_p->tx_hi) 
  {
    DS_3GPP_MSG0_MED("Not posting tx flow enable msg - state not hi");
    return;
  }

  if (bearer_context_p->ds_bearer_context_dyn_p->call_mode
                                                  == SYS_SYS_MODE_LTE)
  {
    if(bearer_context_p->tx_wm.wm_item.dont_exceed_cnt != 
      um_wm_threshold.lte_ul_dne)
    {
      dsm_set_dne(&bearer_context_p->tx_wm.wm_item,
                um_wm_threshold.lte_ul_dne);
    }
  }    


  if (bearer_context_p->ds_bearer_context_dyn_p->call_mode
                                                  == SYS_SYS_MODE_LTE)
  {
    if(bearer_context_p->tx_wm.wm_item.dont_exceed_cnt != 
      um_wm_threshold.lte_ul_dne)
    {
      dsm_set_dne(&bearer_context_p->tx_wm.wm_item,
                um_wm_threshold.lte_ul_dne);
    }
  }    


  if (bearer_context_p->ds_bearer_context_dyn_p->call_mode
                                                  == SYS_SYS_MODE_GSM)
  {
    if(bearer_context_p->tx_wm.wm_item.dont_exceed_cnt != 
      um_wm_threshold.gprs_ul_dne)
    {
      dsm_set_dne(&bearer_context_p->tx_wm.wm_item,
                um_wm_threshold.gprs_ul_dne);
    }
  }    

  if (bearer_context_p->ds_bearer_context_dyn_p->call_mode
                                                  == SYS_SYS_MODE_TDS)
  {
    if(bearer_context_p->tx_wm.wm_item.dont_exceed_cnt != 
      tds_um_wm_threshold.tds_ul_dne)
    {
      dsm_set_dne(&bearer_context_p->tx_wm.wm_item,
                tds_um_wm_threshold.tds_ul_dne);
    }
  }    

  if (bearer_context_p->ds_bearer_context_dyn_p->call_mode
                                                  == SYS_SYS_MODE_WCDMA)
  {
    if(bearer_context_p->tx_wm.wm_item.dont_exceed_cnt != 
         um_wm_threshold.wcdma_ul_dne)
    {
      dsm_set_dne(&bearer_context_p->tx_wm.wm_item,
                um_wm_threshold.wcdma_ul_dne);
    }
  }  
  
  /* Set tx wm state to lo
  */
  bearer_context_p->ds_bearer_context_dyn_p->tx_hi = FALSE;

  ps_phys_link_enable_flow_ex(&(bearer_context_p->phys_link),
                              DS_FLOW_LTE_PDCP_MASK);
} /* ds_bearer_cntxt_ps_to_L2_flow_enable_cb() */
/*lint +esym(715,wm) */ 
/*lint +esym(818,wm) */ 


/*===========================================================================
FUNCTION DS_BEARER_CNTXT_PS_TO_L2_FLOW_DISABLE_CB

DESCRIPTION
  This function diables the Phys link flow. It also sets the tx_hi in bearer
  context structure

  Note: This function runs in PS task context since it is a WM call back fn.
  
  To reduce the latency of FC kicking in, this is being done this way today.
  
PARAMETERS  
      
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_bearer_cntxt_ps_to_L2_flow_disable_cb
(
 dsm_watermark_type *wm, 
 /*lint -esym(715,wm) */ 
 /*lint -esym(818,wm) */ 
 void*              callback_data
)
{
  uint32 bearer_instance;
  ds_bearer_context_s  *bearer_context_p = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  bearer_instance = (uint32) callback_data;

  bearer_context_p = ds_bearer_cntxt_get_instance_ptr(bearer_instance);

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }

  /* No need to post message if wm is already in hi state 
  */
  if (bearer_context_p->ds_bearer_context_dyn_p->tx_hi) 
  {
    DS_3GPP_MSG0_MED("Not posting tx flow disable msg - state not lo");
    return;
  }

  /* Set tx wm state to hi
  */
  bearer_context_p->ds_bearer_context_dyn_p->tx_hi = TRUE;

  /*----------------------------------------------------------------------------
    Disabling the phys link flow instead of sending a message and a command 
    subsequently.
  ----------------------------------------------------------------------------*/
  ps_phys_link_disable_flow_ex(&(bearer_context_p->phys_link),
                             DS_FLOW_LTE_PDCP_MASK);

} /* ds_3gpp_bearer_cntxt_ps_to_L2_flow_disable_cb() */
/*lint +esym(715,wm) */ 
/*lint +esym(818,wm) */ 
#ifdef FEATURE_DATA_LTE
/*===========================================================================
FUNCTION ds_bearer_cntxt_lte_deactivate_req_hdlr

DESCRIPTION
  This function processes LTE Deactivated Request from NAS and tries to clenup 
  Pending calls if any.
  
PARAMETERS  
  msgrtype - the id of the received message 
  dsmsg    - pointer to the received message 

DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_bearer_cntxt_lte_deactivate_req_hdlr
(
  msgr_umid_type               msgrtype,
  const msgr_hdr_struct_type * dsmsg
)
{ 
  sys_modem_as_id_e_type subs_id = ds3g_get_ps_subs_id(); //Vishal: Temporary
  /*-----------------------------------------------------------------------*/

  if(msgrtype != LTE_RRC_DEACTIVATE_REQ )
  {
     return;	
  }
  DS_3GPP_MSG0_HIGH( " Rx'd LTE_RRC_DEACTIVATE_REQ from NAS ");   
  ds_bearer_cntxt_post_clr_pending_calls_cmd(subs_id);
  
}/*ds_bearer_cntxt_lte_deactivate_req_hdlr */
#endif /* FEATURE_DATA_LTE */

/*===========================================================================
FUNCTION ds_bearer_cntxt_post_clr_pending_calls_cmd

DESCRIPTION
  This function posts a command to DS Task to clear pending calls if any.
  
PARAMETERS  
  subs_id: Subscription Id

DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_bearer_cntxt_post_clr_pending_calls_cmd
(
  sys_modem_as_id_e_type subs_id
)
{
  ds_cmd_type     *cmd_ptr;
  sys_modem_as_id_e_type *subs_id_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if( (cmd_ptr = ds_allocate_cmd_buf(sizeof(sys_modem_as_id_e_type)) ) == NULL ||
       cmd_ptr->cmd_payload_ptr == NULL)
  {
    ASSERT(0);
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_CALL_PENDING_CLR;
  DS_3GPP_MSG0_HIGH("Posting CLR_PENDING_REDIAL_CMD to DS task");	

  subs_id_p = (sys_modem_as_id_e_type*)cmd_ptr->cmd_payload_ptr;
  *subs_id_p = subs_id;
  ds_put_cmd( cmd_ptr );
     
  return;
} /* ds_bearer_cntxt_post_clr_pending_calls_cmd */

/*===========================================================================
FUNCTION ds_bearer_cntxt_GETORIGPARAMS

DESCRIPTION
  Populates applicable call origination params in ds_3gpp_orig_params_s, 
  including QOS and default (match all) TFT for default bearer. 
  
PARAMETERS  
  bearer_context_p  - bearer context obj ptr
  mm_orig_params_p  - orig params
  ds3gi_call_info   - call info
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  DS3G_SUCCESS if success, DS3G_FAILURE otherwise.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_bearer_cntxt_get_orig_params
(
  ds_bearer_context_s           *bearer_context_p,
  ds_3gpp_orig_params_s         *mm_orig_params_p,                          
  const ds_3gpp_pdn_call_info_type *ds3gi_call_info
)
{
  boolean rval = DS3G_FAILURE;
  sys_modem_as_id_e_type subs_id = SYS_MODEM_AS_ID_NONE;
  /*-------------------------------------------------------------------------*/

  if (!ds_bearer_cntxt_get_subs_id_from_bearer(bearer_context_p, &subs_id))
  {
    return FALSE;
  }

  DS_BEARER_CTXT_VFR_CALL
  (
    rval, 
    getOrigParams, 
    subs_id,
    bearer_context_p, 
    mm_orig_params_p, 
    ds3gi_call_info
  );

  return rval;
}

/*===========================================================================
FUNCTION ds_bearer_cntxt_GETENDPARAMS

DESCRIPTION
  Populates call end params to use for bringing down the data call, 
  including reason code.
  
PARAMETERS  
  bearer_context_p  - bearer context obj ptr
  end_params_p      - call end parameters
  ds3gi_call_info   - call info
  delay_disconnect  - disconnect to be delayed or not
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_bearer_cntxt_get_end_params
(
  ds_bearer_context_s           *bearer_context_p,
  cm_end_params_s_type          *end_params,
  ds_3gpp_pdn_call_info_type       *ds3gi_call_info,
  boolean                       *delay_disconnect
)
{
  
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }

#if defined (FEATURE_DATA_WCDMA_PS) || defined (FEATURE_GSM_GPRS)
  dsUmtsBearerCntxt_getEndParams
  (
    bearer_context_p,
    end_params,
    ds3gi_call_info,
    delay_disconnect
  );
#endif /* defined (FEATURE_DATA_WCDMA_PS) || defined (FEATURE_GSM_GPRS) */
}

#ifdef FEATURE_DATA_LTE
/*===========================================================================
FUNCTION ds_bearer_cntxt_deactivate_last_bearer_if_needed

DESCRIPTION
  This function checks 
  1. If all bearers are down
  2. If all PDN's are down

  If one of the above is not fulfilled a fres attach is not tried
 
PARAMETERS 
 subs_id
 
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/

void ds_bearer_cntxt_deactivate_last_bearer_if_needed
(
   sys_modem_as_id_e_type  subs_id   
)
{
  uint32                    bearer_cntx_inst= 0;  
  ds_bearer_context_s      *bearer_cntx_iter_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    If GET_PDN_CONN_IND for LTE attach is cached then check if this is the
    last bearer that is getting decativated. If it is then post the command
    again for GET_PDN_CONN_IND
  -------------------------------------------------------------------------*/
  DS_3GPP_MSG3_HIGH("Subs Id(CM): %d, eps_attach_sm_state: %d,"
                     "Internal sys_mode: %d",
                     subs_id, ds_eps_attach_sm_get_state(subs_id),
                     ds3gpp_get_current_network_mode(subs_id));

  /*--------------------------------------------------------------------------
    If attach request is cached, attach SM can be in CACHED state or 
    Attach SM can be in NULL state and cached_seq_num would be valid, 
    this happens when Attach request is received while the PDN 
    is being torn down.
  ---------------------------------------------------------------------------*/
  if( (ds_eps_attach_sm_get_state(subs_id) == DS_EPS_CACHED_GET_PDN_CONN_IND) ||
      (ds_eps_pdn_cntx_is_pdn_conn_req_pending(subs_id)))
  {
    DS_3GPP_MSG3_HIGH("GET_PDN_CONN_IND for LTE Attach is cached,"
                      " checking for GOING_DOWN bearers",0,0,0);

    for( bearer_cntx_inst=0;\
         bearer_cntx_inst<DS_3GPP_MAX_BEARER_CONTEXT;\
         bearer_cntx_inst++ )
    {
      bearer_cntx_iter_p = ds_bearer_cntxt_get_instance_ptr(bearer_cntx_inst);
  
     /* No need to consider bearer that got deactivate_cnf */
      if(!ds_bearer_cntx_validate_bearer_subscription_internal
           (bearer_cntx_iter_p, subs_id))
      {
        continue;
      }
      
      /*-------------------------------------------------------------------------
        If any other bearer is still stuck in GOING_DOWN state then we cannot
        process GET_PDN_CONN_IND yet
      -------------------------------------------------------------------------*/
      if(bearer_cntx_iter_p->ds_bearer_context_dyn_p->state == 
         DS_BEARER_CONTEXT_STATE_GOING_DOWN)            
      {
        DS_3GPP_MSG3_HIGH("Bearer_inst:%d call_id:%d still waiting for call_end, "
                          "delaying GET_PDN_CONN_IND processing",
                    (uint32)bearer_cntx_inst,bearer_cntx_iter_p->
                          ds_bearer_context_dyn_p->cm_call_id,0);
        return;
      }     
             
    }
    /*-------------------------------------------------------------------------
      No other bearer is in GOING_DOWN state, Also check if PDN context
      is in going down state,
      If PDN context is in going down state it means iface associated with the
      PDN needs to be brought down, Hence dont proprocess GET_PDN_CONN_IND here
      It will be processed when the PDN is cleaned up:w!
    -------------------------------------------------------------------------*/
    if (!ds_3gpp_pdn_cntxt_is_any_pdn_going_down(subs_id))
    {
      DS_3GPP_MSG3_HIGH("No Pending bearers, ready to process GET_PDN_CONN_IND",
                         0,0,0);
      ds_eps_attach_sm_handle_last_bearer_deact(subs_id);    
    }
  }

  return;
} /*ds_bearer_cntxt_deactivate_last_bearer_if_needed*/
#endif
/*===========================================================================
FUNCTION ds_bearer_cntxt_DEACTIVATECNF

DESCRIPTION
   Called to indicate deactivate of Bearer Context. Tears down the data path
   including deregistration with lower layers if needed, and changes state to
   DOWN. Notifies PdnContext of state change. This is a virtual function and 
   calls mode specific function.
  
PARAMETERS  
  bearer_context_p   - bearer context obj ptr
  mode_info_p        - mode specific info from CM
  call_was_connected - was call connected
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_bearer_cntxt_deactivate_cnf
(
  ds_bearer_context_s        *bearer_context_p,
  const ds3g_call_info_cmd_type  *call_info_p,
  uint8                      end_type,
  boolean                    call_was_connected
)
{  
#ifdef FEATURE_DATA_LTE
  boolean                   is_dedicated_bearer = FALSE;
#endif /* FEATURE_DATA_LTE */
  ds_pdn_context_s*         pdn_cntx_p = NULL;
  sys_modem_as_id_e_type    subs_id  = SYS_MODEM_AS_ID_NONE;

  /*-----------------------------------------------------------------------*/

  if (!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }

  pdn_cntx_p = (ds_pdn_context_s*)bearer_context_p->ds_bearer_context_dyn_p->
                pdn_context_ptr;

  if(! ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p,&subs_id))
  {
    return;
  }

#ifdef FEATURE_DATA_LTE
  ds_3gpp_pdn_cntxt_detach_and_block_plmn_if_attach_profile_unavailable(subs_id);
#endif /* FEATURE_DATA_LTE */

  /*------------------------------------------------------------------------ 
    Before deactivating, check if pdn_contexts apn param change flag is set
    If set, make a few validations and make a decision, if the bearer and PDN
    should continue to remain in PENDING_REDIAL mode or move it to GOING_DOWN
    Basically if the validation fails, a redial on the same PDN is not
    possible and hence move the state to GOING DOWN.
   
    The redial mechanism is currently only supported for default bearers
  --------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_LTE
  if((memcmp(bearer_context_p, (ds_bearer_context_s *)
            pdn_cntx_p->ds_pdn_context_dyn_p->def_bearer_context_ptr, 
            sizeof(ds_bearer_context_s)) == 0))
  {
    if(pdn_cntx_p->ds_pdn_context_dyn_p->apn_param_chg_teardown == TRUE)
    {
      ds_eps_pdn_cntxt_move_bearer_from_pending_redial_to_down_if_needed
        (bearer_context_p);
    }
  }
  else
  {
    is_dedicated_bearer = TRUE;
  }
#endif /* FEATURE_DATA_LTE */


  DS_BEARER_CTXT_VF_CALL_PER_BEARER
  (
    deactivateCnf, 
    bearer_context_p, 
    call_info_p,
    end_type,
    call_was_connected
  );

#ifdef FEATURE_DATA_LTE
  ds_bearer_cntxt_deactivate_last_bearer_if_needed(subs_id); 
  /*-------------------------------------------------------------------------- 
    If pdn context is in PENDING REDIAL state and if apn param chg teardown is
    set to TRUE, send out a PDN connectivity request out.
   
    We need not send out redial request in case of dedicated bearers since
    they have to be torn down
    --------------------------------------------------------------------------*/
  if (ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p) &&
      ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    if((pdn_cntx_p->ds_pdn_context_dyn_p->state == 
        DS_PDN_CONTEXT_STATE_PEND_REDIAL) && 
       (pdn_cntx_p->ds_pdn_context_dyn_p->apn_param_chg_teardown) &&
       (is_dedicated_bearer == FALSE)
      )
    {
      DS_3GPP_MSG0_HIGH("Sending out PDN connectivity req, Apn param chg");
      ds_eps_handle_apn_param_chg_redial(pdn_cntx_p,bearer_context_p);
    }

    if((pdn_cntx_p->ds_pdn_context_dyn_p->state == DS_PDN_CONTEXT_STATE_PEND_REDIAL) &&
        pdn_cntx_p->ds_pdn_context_dyn_p->redial_reason == DS_3GPP_REDIAL_OP_PCO_CHG)
    {
      DS_3GPP_MSG0_HIGH("Redial because of OP PCO chg");
      pdn_cntx_p->ds_pdn_context_dyn_p->redial_reason = DS_3GPP_NO_REDIAL;
      pdn_cntx_p->ds_pdn_context_dyn_p->state = DS_PDN_CONTEXT_STATE_COMING_UP;
      DS_BEARER_CTXT_VF_CALL_NO_ARG_PER_BEARER
      (
         redial,
         bearer_context_p    
      );
  }
   
  }
#endif /* FEATURE_DATA_LTE */
} /* ds_bearer_cntxt_deactivate_cnf */

/*===========================================================================
FUNCTION ds_bearer_cntxt_DORMANCYREORIG

DESCRIPTION
  Dormancy reorig actions happen here. This function pointer could be set to
  NULL if dormancy is not supported in some modes.
  
PARAMETERS  
  bearer_context_p  - bearer context obj ptr
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_bearer_cntxt_dormancy_reorig
(
  ds_bearer_context_s        *bearer_context_p
)
{
  
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }

  DS_BEARER_CTXT_VF_CALL_PER_BEARER
  (
    dormancyReorig, 
    bearer_context_p,
    NULL
  );
}

/*===========================================================================
FUNCTION ds_bearer_cntxt_BEARERACTIVATEIND

DESCRIPTION
  Called to indicate berarer activation request indication.
  
PARAMETERS  
  call_id           - call ID
  mode_info_ptr_i   - mode info ptr
  subs_id           - Subscription Id
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  DS3G_SUCCESS if success, DS3G_FAILURE otherwise.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_bearer_cntxt_bearer_activate_ind
(
  cm_call_id_type                  call_id,
  const cm_call_mode_info_s_type   *mode_info_ptr_i,
  sys_modem_as_id_e_type           subs_id
)
{
  boolean rval = DS3G_FAILURE;

  DS_BEARER_CTXT_VFR_CALL
  (
    rval, 
    bearerActivateInd, 
    subs_id,
    call_id, 
    mode_info_ptr_i
  );

  return rval;
}

/*===========================================================================
FUNCTION ds_bearer_cntxt_BRING_UP

DESCRIPTION
  This function is used to initiate a call. Since the CM API is different 
  for LTE and UMTS, this function pointer will be used to invoke the mode
  specific function to initiate a call in the corresponding mode.
  
PARAMETERS  
  info_p            - bearer context obj ptr
  call_info         - mode info params
  useDefaultDialStr - whether to use default dial string
  subs_id           - Subscription Id
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  DS3G_SUCCESS if success, DS3G_FAILURE otherwise.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_bearer_cntxt_bring_up
(
  void                    *info_p,
  ds_3gpp_pdn_call_info_type *call_info,
  boolean                     useDefaultDialStr,
  sys_modem_as_id_e_type      subs_id
)
{
  boolean              rval = DS3G_FAILURE;


  DS_BEARER_CTXT_VFR_CALL
  (
    rval, 
    bring_up, 
    subs_id,
    info_p, 
    call_info,
    useDefaultDialStr
  );

  /*---------------------------------------------------------------------
   If a call orig is successful, then notify call orig event to
   DS3G flowmgr.
   TODO: check if rval is set correctly
  ---------------------------------------------------------------------*/
  if( rval )
  {
    ds3gflowmgr_notify_ps_call_state_per_sub(subs_id,
                                             DS3GFLOWMGR_PS_CALL_ORIG_EV);
  }

  return rval;
}

/*===========================================================================
FUNCTION DS_3GPP_BEARER_CONTEXT_GO_NULL

DESCRIPTION
  This function is used to cleanup the bearer. It calls the mode specific bearer
  to clean itself up.
  
PARAMETERS  
  bearer_cntx_p      - bearer context obj ptr
  down_reason        - reason for bearer deactivation
  call_was_connected - whether the call was connected or not.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None
 
SIDE EFFECTS 
 None 
===========================================================================*/
void ds_3gpp_bearer_cntxt_go_null
(
  ds_bearer_context_s * bearer_cntx_p,
  ds_3gpp_down_reason_t down_reason,
  boolean               call_was_connected
)
{
  if(!ds_bearer_cntx_validate_bearer_context(bearer_cntx_p))
  {
    return;
  }

  DS_BEARER_CTXT_VF_CALL_PER_BEARER
  (
    go_null,
    bearer_cntx_p, 
    down_reason, 
    call_was_connected
  );
  
  return;
}


/*===========================================================================
FUNCTION ds_bearer_cntxt_REDIAL

DESCRIPTION
  This function is used to redial a call. Since the CM API is different 
  for LTE and UMTS, this function pointer will be used to invoke the mode
  specific function to initiate a call in the corresponding mode.
  
PARAMETERS    
  call_info         - mode info params  
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  DS3G_SUCCESS if success, DS3G_FAILURE otherwise.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_bearer_cntxt_redial
(  
  ds_bearer_context_s     *bearer_context_p
)
{
  boolean rval = DS3G_FAILURE;
  sys_modem_as_id_e_type subs_id = SYS_MODEM_AS_ID_NONE;
  /*------------------------------------------------------------------------*/

  if (!ds_bearer_cntxt_get_subs_id_from_bearer(bearer_context_p, &subs_id))
  {
    return rval;
  }

  DS_BEARER_CTXT_VFR_CALL
  (
    rval, 
    redial, 
    subs_id,
    bearer_context_p    
  );

  return rval;
} /* ds_bearer_cntxt_redial */

/*===========================================================================
FUNCTION ds_bearer_cntxt_TEAR_DOWN

DESCRIPTION
  This function is used to initiate the tear down of a call. Since the APIs
  are different in UMTS and LTE modes, we need mode specific handlers. These
  function pointers are initialized in the corresponding mode specific files
  and are accessed while processing phys_link_down command
 
PARAMETERS  
  bearer_context_p  - bearer context obj ptr
  call_info         - call info params
  info_ptr          - void pointer
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  DS3G_SUCCESS if success, DS3G_FAILURE otherwise.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_bearer_cntxt_tear_down_call
(
  ds_bearer_context_s     *bearer_context_p,
  ds_3gpp_pdn_call_info_type *call_info,
  void                    *info_ptr
)
{
  boolean rval = DS3G_FAILURE;

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return rval;
  }

  DS_BEARER_CTXT_VFR_CALL_PER_BEARER
  (
    rval, 
    tear_down, 
    bearer_context_p, 
    call_info,
    info_ptr
  );

  return rval;
}

/*===========================================================================
FUNCTION ds_3gpp_bearer_cntxt_get_ps_rat_mask_for_hs_status

DESCRIPTION
  This function returns the RAT MASK corresponding to the passed HS status.

PARAMETERS
  hs_status - the HS status
  subs_id   - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  RAT mask 

SIDE EFFECTS
  None.
  
===========================================================================*/
uint32 ds_3gpp_bearer_cntxt_get_ps_rat_mask_for_hs_status
( 
  sys_hs_ind_e_type       hs_status,
  sys_modem_as_id_e_type  subs_id
)
{  
  uint32 rval = 0;

  DS_BEARER_CTXT_VFR_CALL
  (
    rval, 
    getratMaskforHs, 
    subs_id,
    hs_status,
    subs_id
  );

  return rval;
} /* ds_3gpp_bearer_cntxt_get_ps_rat_mask_for_hs_status */

/*===========================================================================
FUNCTION ds_3gpp_bearer_cntxt_get_cell_srv_ind

DESCRIPTION
  This function returns the cached cell srv ind.

PARAMETERS
  subs_id      - subscription id

DEPENDENCIES
  None.

RETURN VALUE
  cell srv ind 

SIDE EFFECTS
  None.
  
===========================================================================*/
cm_cell_srv_ind_s_type ds_3gpp_bearer_cntxt_get_cell_srv_ind
(
  sys_modem_as_id_e_type  subs_id
)
{
  cm_cell_srv_ind_s_type rval = {-1,-1,-1,-1};
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  DS_BEARER_CTXT_VFR_CALL
  (
    rval,
    getCellSrvInd,
    subs_id,
    subs_id
  );

  return rval;
} /* ds_3gpp_bearer_cntxt_get_cell_srv_ind */

/*===========================================================================
FUNCTION ds_3gpp_bearer_cntxt_set_cell_srv_ind

DESCRIPTION
  This function sets the cached cell srv ind.

PARAMETERS
  cell_srv_ind - passed cell srv ind
  subs_id      - subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void ds_3gpp_bearer_cntxt_set_cell_srv_ind
(
  cm_cell_srv_ind_s_type  cell_srv_ind,
  sys_modem_as_id_e_type  subs_id
)
{
  DS_BEARER_CTXT_VF_CALL
  (
    setCellSrvInd, 
    subs_id,
    cell_srv_ind,
    subs_id
  );
} /* ds_3gpp_bearer_cntxt_set_cell_srv_ind */

/*===========================================================================
FUNCTION ds_3gpp_bearer_cntxt_get_ps_rat_mask_for_egprs_support

DESCRIPTION
  This function returns the RAT MASK corresponding to the passed egprs support.

PARAMETERS
  egprs_support - egprs supported or not
  subs_id       - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  RAT mask 

SIDE EFFECTS
  None.
  
===========================================================================*/
uint32 ds_3gpp_bearer_cntxt_get_ps_rat_mask_for_egprs_support
( 
  sys_egprs_support_e_type  egprs_support,
  sys_modem_as_id_e_type    subs_id
)
{  
  uint32 rval = 0;

  DS_BEARER_CTXT_VFR_CALL
  (
    rval, 
    getratMaskforEgprsSupport, 
    subs_id,
    egprs_support,
    subs_id
  );

  return rval;
} /* ds_3gpp_bearer_cntxt_get_ps_rat_mask_for_egprs_support */

/*===========================================================================
FUNCTION ds_3gpp_bearer_cntxt_get_prev_div_duplex

DESCRIPTION
  This function returns the previous lte div duplex reported by CM.

PARAMETERS
  subs_id - subscription id

DEPENDENCIES
  None.

RETURN VALUE
  Last div duplex reported by CM.

SIDE EFFECTS
  None.
===========================================================================*/
sys_div_duplex_e_type ds_3gpp_bearer_cntxt_get_prev_div_duplex
(
  sys_modem_as_id_e_type  subs_id
)
{
  sys_div_duplex_e_type rval = 0;
/*-------------------------------------------------------------------------*/

  DS_BEARER_CTXT_VFR_CALL
  (
    rval, 
    get_div_duplex,
    subs_id,
    subs_id
  );

  return rval;
} /* ds_3gpp_bearer_cntxt_get_prev_div_duplex */

/*===========================================================================
FUNCTION ds_3gpp_bearer_cntxt_set_prev_div_duplex

DESCRIPTION
  This function returns the previous lte div duplex reported by CM.

PARAMETERS
  div_duplex - div_duplex value
  subs_id    - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  Last lte div duplex reported by CM.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_bearer_cntxt_set_prev_div_duplex
(
  sys_div_duplex_e_type   div_duplex,
  sys_modem_as_id_e_type  subs_id
)
{
  DS_BEARER_CTXT_VF_CALL
  (
    set_div_duplex,
    subs_id,
    div_duplex,
    subs_id
  );
} /* ds_3gpp_bearer_cntxt_set_prev_div_duplex */

/*===========================================================================
FUNCTION ds_3gpp_bearer_cntxt_comp_log_fetch_iface

DESCRIPTION

  This Functions returns the iface corresponding to link control id

PARAMETERS
  logging_info_ptr - the Log information 
  ip_version       - ip version v4 or v6

DEPENDENCIES
  None.

RETURN VALUE
  iface  

SIDE EFFECTS
  None.
  
===========================================================================*/
ps_iface_type *ds_3gpp_bearer_cntxt_comp_log_fetch_iface
( 
  ps_comp_logging_instance_config_s *logging_info_ptr,
  ip_version_enum_type               ip_version
)
{  
  ps_iface_type *iface = NULL;

  /* Temporary hack needed to obtain subs_id. Remove this hack once we
     support W/TD + W/TD DSDA configuration */
  sys_modem_as_id_e_type  subs_id = 
                            ds3gpp_get_subs_id_for_w_td_mode();

  DS_BEARER_CTXT_VFR_CALL
  (
    iface,
    complogfetchiface,
    subs_id,
    logging_info_ptr, 
    ip_version
  );

  return iface;
} /* ds_3gpp_bearer_cntxt_comp_log_fetch_iface */

/*===========================================================================
FUNCTION ds_bearer_cntxt_RAB_RE_ESTAB_IND

DESCRIPTION
  This function is to perform any mode specific operations on Rab re-estab
  indication.
  
PARAMETERS  
  mode_info_ptr     - mode specific info from CM
  bearer_context_p  - bearer context obj ptr
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  DS3G_SUCCESS if success, DS3G_FAILURE otherwise.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean 
ds_bearer_cntxt_rab_re_estab_ind
(
  ds_bearer_context_s* bearer_context_p,
  cm_call_mode_info_s_type  *mode_info_ptr
)
{
  boolean rval = DS3G_FAILURE;

  
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return rval;
  }

  if (bearer_context_p->ds_bearer_context_dyn_p->call_mode == SYS_SYS_MODE_GSM)
  {
    DS_3GPP_MSG1_HIGH("bearer_mode is GSM sys_rat is %d. Changing bearer_mode",
                      (mode_info_ptr->info.gw_ps_call.sys_rat));

    switch(mode_info_ptr->info.gw_ps_call.sys_rat)
    {
#ifdef FEATURE_WCDMA
      case SYS_RAT_UMTS_RADIO_ACCESS:
        bearer_context_p->ds_bearer_context_dyn_p->call_mode = SYS_SYS_MODE_WCDMA;
        break;
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_TDSCDMA
      case SYS_RAT_TDS_RADIO_ACCESS:
        bearer_context_p->ds_bearer_context_dyn_p->call_mode = SYS_SYS_MODE_TDS;
        break;
#endif /* FEATURE_TDSCDMA */
      default:
        DS_3GPP_MSG1_ERROR("Invalid sys_rat: %d in mode_info_p",
                            mode_info_ptr->info.gw_ps_call.sys_rat);
        return rval;
    }
  }

  DS_BEARER_CTXT_VFR_CALL_PER_BEARER
  (
    rval, 
    rab_re_estab_ind, 
    bearer_context_p,
    mode_info_ptr    
  );

  return rval;
}

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_MODE_PREF_CHANGE

DESCRIPTION
  This Handler is triggered while mode_pref  changes. This is required when 
  IRAT happens due to mode change.

PARAMETERS
  subs_id             -- Subscription ID where mode_pref_change occurred.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds_bearer_cntxt_mode_pref_change
(
  sys_modem_as_id_e_type subs_id
)
{
  uint32              bearer_index = 0;
  ds_bearer_context_s *bearer_cntxt_p = NULL;
  /*-----------------------------------------------------------------------*/


  for (bearer_index = 0; bearer_index < DS_3GPP_MAX_BEARER_CONTEXT; bearer_index++)
  {
    bearer_cntxt_p = ds_bearer_cntxt_get_instance_ptr(bearer_index);

    if (ds_bearer_cntx_validate_bearer_subscription_internal(bearer_cntxt_p, subs_id))
    {
      switch (bearer_cntxt_p->ds_bearer_context_dyn_p->state)
      {
        case DS_BEARER_CONTEXT_STATE_UP:
        case DS_BEARER_CONTEXT_STATE_UP_DORMANT:
        case DS_BEARER_CONTEXT_STATE_UP_DORMANT_REESTAB:
          DS_BEARER_CTXT_VF_CALL_NO_ARG_PER_BEARER
          (
            mode_pref_change, 
            bearer_cntxt_p   
          );
          break;

        default:
          DS_3GPP_MSG3_ERROR("Bearer mode_pref change for bearer_idx: %d in state: %d",
                              bearer_index, 
                              bearer_cntxt_p->ds_bearer_context_dyn_p->state, 0);
          break;
      }
    }
  }
} /* ds_bearer_cntxt_mode_pref_change */

/*===========================================================================
FUNCTION  DS3GPP_GET_V4_IFACE_PTR_FROM_NSAPI

DESCRIPTION
  This function returns the v4 PS IFace ptr corresponding to the NSAPI.
  This is used for the test framework.

PARAMETERS
  nsapi
  subs_id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
ps_iface_type *ds_3gpp_bearer_cntxt_get_v4_iface_ptr_from_nsapi
(
  uint8                  nsapi,
  sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_iface_s       *ds_iface_p=NULL;
  int i = 0;
  ds_pdn_context_s *pdn_cntx_p = NULL;
  ps_iface_type *iface_ptr = NULL;
  ds_bearer_context_s   *bearer_context_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(i=0; i< DS_3GPP_MAX_BEARER_CONTEXT; i++)
  {
    bearer_context_p = ds_bearer_cntxt_get_instance_ptr(i);

    if((ds_bearer_cntx_validate_bearer_subscription_internal
          (bearer_context_p, subs_id)) &&
       (bearer_context_p->ds_bearer_context_dyn_p->nsapi == nsapi))
    {
      pdn_cntx_p = (ds_pdn_context_s *)
                     bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr;
    }
  }

  if(ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    ds_iface_p = pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
                   [DS_3GPP_PDN_CONTEXT_IPV4_IFACE];
    if(ds_iface_p != NULL)
  {
      iface_ptr = &(ds_iface_p->ps_iface);
    }
  }

  return iface_ptr;
} /*ds_3gpp_bearer_cntxt_get_v4_iface_ptr_from_nsapi */

/*===========================================================================
FUNCTION  DS3GPP_GET_V6_IFACE_PTR_FROM_NSAPI

DESCRIPTION
  This function returns the v6 PS IFace ptr corresponding to the NSAPI.
  This is used for the test framework.

PARAMETERS
  nsapi
  subs_id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
ps_iface_type *ds_3gpp_bearer_cntxt_get_v6_iface_ptr_from_nsapi
(
  uint8                  nsapi,
  sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_iface_s      *ds_iface_p=NULL;
  int i = 0;
  ds_pdn_context_s *pdn_cntx_p = NULL;
  ps_iface_type *iface_ptr = NULL;
  ds_bearer_context_s  *bearer_context_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(i=0; i< DS_3GPP_MAX_BEARER_CONTEXT; i++)
  {
    bearer_context_p = ds_bearer_cntxt_get_instance_ptr(i);

    if((ds_bearer_cntx_validate_bearer_subscription_internal 
          (bearer_context_p, subs_id)) &&
       (bearer_context_p->ds_bearer_context_dyn_p->nsapi == nsapi))
    {
      pdn_cntx_p = (ds_pdn_context_s *)
                     bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr;
    }
  }

  if(ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    ds_iface_p = pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
                   [DS_3GPP_PDN_CONTEXT_IPV6_IFACE];

    if(ds_iface_p != NULL)
  {
      iface_ptr = &(ds_iface_p->ps_iface);
    }
  }

  return iface_ptr;
} /*ds_3gpp_bearer_cntxt_get_v6_iface_ptr_from_nsapi */

/*===========================================================================
FUNCTION ds_bearer_cntxt_suspend_ind

DESCRIPTION
  This function is to perform any mode specific operations on data suspend
  indication.
  
PARAMETERS  
  send_ps_data_avail - we are not allowed to send data_available_ind
  if we are OOS or in limited service
  subs_id            - Subscription Id
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_bearer_cntxt_suspend_ind
(
  boolean                send_ps_data_avail,
  sys_modem_as_id_e_type subs_id
)
{
  ds_bearer_context_s      *bearer_context_p= NULL;
  uint32                    bearer_cntx_inst= 0;  
  dsm_watermark_type       *tx_wm_ptr=NULL; /* Watermark pointer      */
  /*-----------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------
    Call the mode specific suspend handler for each bearer.
  --------------------------------------------------------------------------*/
  for( bearer_cntx_inst=0;\
       bearer_cntx_inst<DS_3GPP_MAX_BEARER_CONTEXT;\
       bearer_cntx_inst++ )
  {
    bearer_context_p = ds_bearer_cntxt_get_instance_ptr(bearer_cntx_inst);

    if (!ds_bearer_cntxt_validate_bearer_subscription(bearer_context_p, subs_id))
    {
      continue;
    }

    if((bearer_context_p->ds_bearer_context_dyn_p->state == DS_BEARER_CONTEXT_STATE_UP_DORMANT_REESTAB) ||
       (bearer_context_p->ds_bearer_context_dyn_p->state == DS_BEARER_CONTEXT_STATE_UP_DORMANT) ||
       (bearer_context_p->ds_bearer_context_dyn_p->state == DS_BEARER_CONTEXT_STATE_UP))
    {
      DS_BEARER_CTXT_VF_CALL_NO_ARG_PER_BEARER
      (    
        suspend_ind,   
        bearer_context_p        
      );

      /*---------------------------------------------------------------------
        If there is data in the tx watermark, send the data available
        indication to CM right away.
      ---------------------------------------------------------------------*/
      tx_wm_ptr = &(bearer_context_p->tx_wm.wm_item);
      if ((!ds3gpp_get_ps_data_available_flag(subs_id))   &&
          (tx_wm_ptr->current_cnt > 0) &&
          (send_ps_data_avail)
         )
      {
        DS_3GPP_MSG1_HIGH("suspend_ind:%d sending data_available",\
                          (uint32) bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr);
        ds3g_msh_ps_data_available_ind(subs_id);
        ds3gpp_set_ps_data_available_flag(TRUE, subs_id);
      }

      /*---------------------------------------------------------------
        If ds_3gpp_data_available is set to TRUE, disable flow on the
        interface to stop too much data from accumulating in the
        watermark.
      ---------------------------------------------------------------*/
      if (!send_ps_data_avail || 
          (ds3gpp_get_ps_data_available_flag(subs_id) == TRUE))
      {
        /*-------------------------------------------------------------
          Disable flow with UMTS mask.
        -------------------------------------------------------------*/
        ps_phys_link_disable_flow(
                        &(bearer_context_p->phys_link),
                        DS_FLOW_UMTS_MASK);
      }
      else
      {
        /*-------------------------------------------------------------
         We are allowed to send data_available_ind and there is no 
         data in watermarks.     
        -------------------------------------------------------------*/

        ps_phys_link_enable_flow(
                       &(bearer_context_p->phys_link),
                       DS_FLOW_UMTS_MASK);
      }
      DS_3GPP_MSG2_HIGH("Bearer_inst:%d UM watermark cnt:%d",
         (uint32)bearer_cntx_inst,bearer_context_p->
                        tx_wm.wm_item.current_cnt);

    }// if bearer is UP/UP_DORMANT/UP_DORMANT_REESTAB
  }//end for each bearer  
} /* ds_bearer_cntxt_suspend_ind */

/*===========================================================================
FUNCTION ds_bearer_cntxt_RESUME_IND

DESCRIPTION
  This function is to perform any mode specific operations on data resume
  indication.
  
PARAMETERS  
  subs_id - Subscription Id
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_bearer_cntxt_resume_ind
(
  sys_modem_as_id_e_type subs_id
)
{
  ds_bearer_context_s      *bearer_context_p= NULL;
  uint32                    bearer_cntx_inst= 0;
  /*--------------------------------------------------------------------------
    Call the mode specific resume handler for each bearer.
  --------------------------------------------------------------------------*/
  for( bearer_cntx_inst=0;\
       bearer_cntx_inst<DS_3GPP_MAX_BEARER_CONTEXT;\
       bearer_cntx_inst++ )
  {
    bearer_context_p = ds_bearer_cntxt_get_instance_ptr(bearer_cntx_inst);

    if (!ds_bearer_cntxt_validate_bearer_subscription(bearer_context_p,subs_id))
    {
      continue;
    }

    if((bearer_context_p->ds_bearer_context_dyn_p->state == 
        DS_BEARER_CONTEXT_STATE_UP_DORMANT_REESTAB) ||
       (bearer_context_p->ds_bearer_context_dyn_p->state == 
        DS_BEARER_CONTEXT_STATE_UP_DORMANT) ||
       (bearer_context_p->ds_bearer_context_dyn_p->state == 
        DS_BEARER_CONTEXT_STATE_UP))
    {
      DS_BEARER_CTXT_VF_CALL_NO_ARG_PER_BEARER
      (   
        resume_ind,
        bearer_context_p    
      );
    }// if bearer is UP/UP_DORMANT/UP_DORMANT_REESTAB
  }//end for each bearer

}/* ds_bearer_cntxt_resume_ind */

/*===========================================================================
FUNCTION  DS_3GPP_BEARER_CNTXT_GET_PHYS_LINK_PTR_FROM_NSAPI

DESCRIPTION
  This function returns the UMTS PHYS Link ptr corresponding to the NSAPI.
  This is used for the test framework. 
  
PARAMETERS
  nsapi
  subs_id

DEPENDENCIES
  None.

RETURN VALUE
  Phys Link Ptr

SIDE EFFECTS
  None.

===========================================================================*/
ps_phys_link_type *ds_3gpp_bearer_cntxt_get_phys_link_ptr_from_nsapi
(
  uint8                  nsapi,
  sys_modem_as_id_e_type subs_id
)
{ 
  int index =0;
  ds_bearer_context_s     *bearer_context_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(index=0; index < DS_3GPP_MAX_BEARER_CONTEXT; index++)
  {
    bearer_context_p = ds_bearer_cntxt_get_instance_ptr(index);

    if(ds_bearer_cntx_validate_bearer_subscription_internal(bearer_context_p, subs_id))
    {
      if(bearer_context_p->ds_bearer_context_dyn_p->nsapi == nsapi)
      {
        return(&(bearer_context_p->phys_link));
      } /* bearer_id matches*/
    }  /* if valid index */
  } /* for index */
  return(NULL);
} /* ds_3gpp_bearer_cntxt_get_phys_link_ptr_from_nsapi */

/*===========================================================================

FUNCTION DS_BEARER_CNTXT_FLUSH_PS_L2_WATERMARKS

DESCRIPTION
  This function flushes PS-L2 watermarks

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_bearer_cntxt_flush_ps_l2_watermarks
(
  void
)
{
  int index =0;
  ds_bearer_context_s   *bearer_context_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(index=0; index < DS_3GPP_MAX_BEARER_CONTEXT; index++)
  {
    bearer_context_p = ds_bearer_cntxt_get_instance_ptr(index);

    /* flush the watermark for this bearer */
    if(!ds_bearer_cntx_validate_bearer_context_internal(bearer_context_p))
    {
      continue;
    }
#ifdef FEATURE_DATA_WCDMA_PS
    if (bearer_context_p->ds_bearer_context_dyn_p->call_mode == 
                                                            SYS_SYS_MODE_WCDMA) 
    {
      ds_tds_w_cdma_bearer_cntxt_empty_watermark(bearer_context_p);
    }
    else
#endif /* FEATURE_DATA_WCDMA_PS */
    {
      dsm_empty_queue ( &(bearer_context_p->tx_wm.wm_item) );
    }
    dsm_empty_queue ( &(bearer_context_p->rx_wm.wm_item) );
    dsm_empty_queue ( &(bearer_context_p->lte_rx_wm.wm_item) );
  }
} /*ds_bearer_cntxt_flush_ps_l2_watermarks*/

/*===========================================================================

FUNCTION DS_3GPP_BEARER_CNTXT_MEM_CTRL_EV_CB

DESCRIPTION
  This function posts cmd to DS task to notify DSM Memory control event.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_bearer_cntxt_mem_ctrl_ev_cb
(
  dsm_mempool_id_enum_type pool_id,            /* memory pool ID */
  dsm_mem_level_enum_type  event,              /* type of memory event */
  dsm_mem_op_enum_type     op
)
{
  ds_cmd_type *cmd_ptr = NULL;
  ds_bearer_context_event_type  *ds_bearer_context_event_ptr = NULL;
  boolean     post_cmd = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds3gpp_enter_global_crit_section();
  if (ds_bearer_cntxt_get_mem_ctrl_ev_posted_flag() == FALSE)
  {
    post_cmd = TRUE;
  }
  ds3gpp_leave_global_crit_section();

  if (post_cmd == TRUE && 
      op == DSM_MEM_OP_NEW &&
      pool_id == DSM_DS_LARGE_ITEM_POOL)
  {
    cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_bearer_context_event_type));

    if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
    {                                                                      
      ASSERT(0);
      return;                                                              
    }                                                                      

    DS_3GPP_MSG3_HIGH( "dsm mem ev cb: free [%d], event [%d], pool_id [%d]", 
                        DSM_POOL_FREE_CNT(pool_id), event, pool_id );                                                           

    cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_DSM_MEM_CTRL_EV;
    ds_bearer_context_event_ptr = 
      (ds_bearer_context_event_type*)cmd_ptr->cmd_payload_ptr;

    ds_bearer_context_event_ptr->event_type = DS_BEARER_CONTEXT_EV_DSM_MEM_CTRL;
    ds_bearer_context_event_ptr->cmd_info.dsm_info.pool_id = pool_id;
    ds_bearer_context_event_ptr->cmd_info.dsm_info.event = event;
    ds_bearer_context_event_ptr->cmd_info.dsm_info.op = op;

    ds_put_cmd_ext( cmd_ptr ); 
  
    ds3gpp_enter_global_crit_section();
    ds_bearer_cntxt_set_mem_ctrl_ev_posted_flag(TRUE);
    ds3gpp_leave_global_crit_section();
  }
  else
  {
    DS_3GPP_MSG3_HIGH("dsm mem ev cb: post cmd: %d, op: %d, pool_id: %d",
                       post_cmd,op,pool_id);
  }

  return;

} /*ds_3gpp_bearer_cntxt_mem_ctrl_ev_cb*/

/*===========================================================================

FUNCTION DS_BEARER_CNTXT_LOCAL_CLEANUP

DESCRIPTION
  This function peforms local call cleanup

PARAMETERS  
  Bearer Context ptr

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void ds_bearer_cntx_local_cleanup 
(
  ds_bearer_context_s *bearer_context_p
)
{
  /*-----------------------------------------------------------------------*/

  DS_3GPP_MSG0_HIGH("Performing local call cleanup ");

  ps_phys_link_enable_flow(&(bearer_context_p->phys_link), DS_FLOW_UMTS_MASK);

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }

  ps_phys_link_enable_flow(&(bearer_context_p->phys_link),
                            DS_FLOW_UMTS_MASK);

  if (bearer_context_p->ds_bearer_context_dyn_p->is_default)
  {
    /*-------------------------------------------------------------------
      Error in setting up default bearer -> Make sure that we call flow 
      go null ind on default flows of all ifaces associated with the PDN 
      context          
    --------------------------------------------------------------------*/
    ds_pdn_cntxt_unbind_default_flows(bearer_context_p->
                                      ds_bearer_context_dyn_p->pdn_context_ptr);     
    
    /*---------------------------------------------------------------------
      Disable signal if this is the last phys link
    ---------------------------------------------------------------------*/
    ps_disable_sig(bearer_context_p->rx_sig);   
  }

  /*-------------------------------------------------------------------
    Set the Bearer context to be in down state and call the bearer down
    call back to the PDN context.
  --------------------------------------------------------------------*/

  ps_phys_link_gone_ind(&(bearer_context_p->phys_link));
  /* Is it safe to free the bearer inst ?*/
  ds_bearer_cntxt_free_bearer_inst(bearer_context_p);

  /*-----------------------------------------------------------------------
    We have already cleaned up at the bearer level. We should send
    bearer down indication to PDN context.
    This way the PDN context will be freed up as well.
  -----------------------------------------------------------------------*/
  if (bearer_context_p->ds_bearer_context_dyn_p->pdn_context_cb_tbl_ptr->
      bearer_down_ind_cb != NULL)
  {
	  ds_3gpp_down_reason_t  net_down_reason;
      net_down_reason.reason.ps_iface_down_reason = PS_NET_DOWN_REASON_NOT_SPECIFIED;
	  net_down_reason.reason_T = DS_3GPP_PS_IFACE_NET_DOWN_REASON_T;
      bearer_context_p->ds_bearer_context_dyn_p->
        pdn_context_cb_tbl_ptr->bearer_down_ind_cb(
                  bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr,
                  bearer_context_p,
                  net_down_reason,
                  FALSE);
  }
}

/*===========================================================================

FUNCTION DS_BEARER_CNTXT_IS_ANY_OTHER_BEARER_UP

DESCRIPTION
  This function checks if any other bearer is in UP/UP_DORMANT state

PARAMETERS  
  Bearer Context ptr

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_bearer_cntx_is_any_other_bearer_up
(
  ds_bearer_context_s *bearer_context_p
)
{
  boolean ret_val = FALSE;
  ds_bearer_context_s *temp_bearer_context_p = NULL;
  uint8 bearer_cntx_index = 0;
  sys_modem_as_id_e_type    subs_id = SYS_MODEM_AS_ID_NONE;
  ds_bearer_context_state_e state = DS_BEARER_CONTEXT_STATE_DOWN;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_bearer_cntxt_get_subs_id_from_bearer(bearer_context_p, &subs_id) )
  {
    return ret_val;
  }

  for(bearer_cntx_index = 0; bearer_cntx_index < DS_3GPP_MAX_BEARER_CONTEXT;
      bearer_cntx_index++)
  {
    temp_bearer_context_p = ds_bearer_cntxt_get_instance_ptr(bearer_cntx_index);

    if(temp_bearer_context_p == bearer_context_p)
    {
      DS_3GPP_MSG2_MED("temp_bearer_cntxt:0x%x, bearer_context_p:0x%x",
                       temp_bearer_context_p,bearer_context_p);
      continue;
    }
    else if (!ds_bearer_cntx_validate_bearer_subscription_internal
               (temp_bearer_context_p, subs_id))
    {
      continue;
    }

    state = temp_bearer_context_p->ds_bearer_context_dyn_p->state;

    if((state == DS_BEARER_CONTEXT_STATE_UP ) ||
       (state == DS_BEARER_CONTEXT_STATE_UP_DORMANT) ||
       (state == DS_BEARER_CONTEXT_STATE_UP_DORMANT_REESTAB))
    {
      DS_3GPP_MSG1_HIGH("Bearer index:%d is UP",bearer_cntx_index);
      ret_val = TRUE;
      break;
    }
    else
    {
      DS_3GPP_MSG1_MED("Bearer context state is %d",
                        temp_bearer_context_p->ds_bearer_context_dyn_p->state);
    }
  }

  DS_3GPP_MSG1_MED("ds_bearer_cntx_is_any_other_bearer_up returns %d",ret_val);
  return ret_val;
} /* ds_bearer_cntx_is_any_other_bearer_up */

/*===========================================================================

FUNCTION DS_BEARER_CNTXT_IS_ANY_OTHER_BEARER_PER_SUB_ACTIVE

DESCRIPTION
  This function checks if any other bearer is in UP state only for the SUBS
  of the bearer context

PARAMETERS  
  Bearer Context ptr

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_bearer_cntx_is_any_other_bearer_per_sub_active
(
  ds_bearer_context_s *bearer_context_p
)
{
  boolean                 ret_val = FALSE;
  ds_bearer_context_s    *temp_bearer_context_p = NULL;
  uint8                   bearer_cntx_index = 0;
  sys_modem_as_id_e_type  subs_id = SYS_MODEM_AS_ID_NONE;
  sys_modem_as_id_e_type  temp_subs_id = SYS_MODEM_AS_ID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds_bearer_cntxt_get_subs_id_from_bearer(bearer_context_p, &subs_id) )
  {
    return ret_val;
  }

  for(bearer_cntx_index = 0; bearer_cntx_index < DS_3GPP_MAX_BEARER_CONTEXT;
      bearer_cntx_index++)
  {
    temp_bearer_context_p = ds_bearer_cntxt_get_instance_ptr(bearer_cntx_index);

    if((temp_bearer_context_p == bearer_context_p) ||
       (!ds_bearer_cntx_validate_bearer_context_internal(temp_bearer_context_p)))
    {
      DS_3GPP_MSG2_MED("temp_bearer_cntxt:0x%x, bearer_context_p:0x%x",
                       temp_bearer_context_p, bearer_context_p);
      continue;
    }

    /*-----------------------------------------------------------------------
      Check the temp subs_id, Perform this function check only for bearers
      of the same subscription.
    -----------------------------------------------------------------------*/
    if( !ds_bearer_cntxt_get_subs_id_from_bearer(temp_bearer_context_p,
                                                 &temp_subs_id))
    {
      continue;
    }

    if (subs_id != temp_subs_id)
    {
      DS_3GPP_MSG2_MED("Subs ID mismatch, continue "
                       "Temp Subs Id(CM):%d Subs Id(CM):%d",
                       temp_subs_id,subs_id);
      continue;
    }

    if(temp_bearer_context_p->ds_bearer_context_dyn_p->state ==
       DS_BEARER_CONTEXT_STATE_UP)
    {
      DS_3GPP_MSG1_HIGH("Bearer index:%d is UP",bearer_cntx_index);
      ret_val = TRUE;
      break;
    }
	else
	{
	  DS_3GPP_MSG1_MED("Bearer context state is %d",
						temp_bearer_context_p->ds_bearer_context_dyn_p->state);
	}
  }

  DS_3GPP_MSG1_MED("ds_bearer_cntx_is_any_other_bearer_up returns %d",ret_val);
  return ret_val;
} /* ds_bearer_cntx_is_any_other_bearer_per_sub_active */

/*===========================================================================
FUNCTION DS_3GPP_BEARER_CONTEXT_INIT_BEARER_THROUGHPUT_STATS

DESCRIPTION
  This function initializes the bearer throughput statistics parameters
  for all active bearer contexts. This will be required to get a better
  estimate of the actual throughput rates on all active bearers
 
PARAMETERS  
  subs_id: Subscription Id
    
DEPENDENCIES 
  The bearer throughput stats are initialized before the timer that is
  used to refresh actual throughout timer starts.
  
RETURN VALUE 
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_bearer_context_init_bearer_throughput_stats
(
  sys_modem_as_id_e_type subs_id
)
{
  uint8                   index             = 0;
  ds_bearer_context_s    *bearer_context_p  = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (index = 0; index < DS_3GPP_MAX_BEARER_CONTEXT;index++) 
  {
    bearer_context_p = ds_bearer_cntxt_get_instance_ptr(index);

    if(!ds_bearer_cntx_validate_bearer_subscription_internal(bearer_context_p,
                                                    subs_id))
    {
      continue;
    }
    /*----------------------------------------------------------------------- 
      Initialize Bearer throughput parameters for all active bearers based
      on their current bearer call mode
      -----------------------------------------------------------------------*/
    DS_BEARER_CTXT_VF_CALL_NO_ARG_PER_BEARER
    (
       init_bearer_throughput_stats,
       bearer_context_p
    );
  }

  return;
} /* ds_3gpp_bearer_context_init_bearer_throughput_stats */

#if defined(FEATURE_DATA_LTE) && (defined(FEATURE_GSM_GPRS) || defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_TDSCDMA))

/*===========================================================================

FUNCTION ds_bearer_cntxt_transfer_ind

DESCRIPTION
  This function is to perform any mode specific operations on cntxt transfer
  indication.
  
PARAMETERS  
  None.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE,if atleast one Bearer was successfully transferred to target rat
  FALSE, otherwise 
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean 
ds_bearer_cntxt_transfer_ind
(
  sys_sys_mode_e_type       target_rat,
  sys_modem_as_id_e_type    subs_id
)
{
  ds_bearer_context_s                *bearer_context_p= NULL;
  uint32                    bearer_cntx_inst= 0;
  boolean                   ret_val = FALSE;
  dsm_watermark_type                 *rx_wm_p= NULL;
  ds_bearer_cntxt_per_subs_info_type *per_subs_info_hndl = NULL;
  /*------------------------------------------------------------------------*/

  per_subs_info_hndl = 
    ds_bearer_cntxt_get_per_subs_info_hndl(subs_id);

  if (!ds_bearer_cntxt_validate_per_subs_info_hndl
        (per_subs_info_hndl))
  {
    DS_3GPP_MSG1_ERROR("Bearer Cntxt per subs info handle is invalid for "
                       "Subs Id(CM)",subs_id);
    return FALSE;
  }
  /*--------------------------------------------------------------------------
     Call the mode specific release and for each bearer for context
     transfer across LTE<->GW. Release hdlr will be called in source RAT,
     Setup handler will be called in Target RAT.
  --------------------------------------------------------------------------*/

  DS_3GPP_MSG1_HIGH("cntxt_transfer_ind received for mode:%d",target_rat);
  
  for( bearer_cntx_inst=0;\
       bearer_cntx_inst<DS_3GPP_MAX_BEARER_CONTEXT;\
       bearer_cntx_inst++ )
  {
    bearer_context_p = ds_bearer_cntxt_get_instance_ptr(bearer_cntx_inst);

    if(!ds_bearer_cntxt_validate_bearer_subscription(bearer_context_p, subs_id))
    {
      continue;
    }

    /*---------------------------------------------------------------------
      If moving from L to G/W and the bearer is not transferable to GW
    ---------------------------------------------------------------------*/
    if((bearer_context_p->ds_bearer_context_dyn_p->call_mode == SYS_SYS_MODE_LTE) &&
       (bearer_context_p->ds_bearer_context_dyn_p->is_transferable_to_GW == FALSE) &&
       ((target_rat == SYS_SYS_MODE_WCDMA)||(target_rat == SYS_SYS_MODE_GSM)||
        (target_rat == SYS_SYS_MODE_TDS)))
    {
      DS_3GPP_MSG2_HIGH("call id:%d not transferable to GWT, maintaining mode:%d",
                        bearer_context_p->ds_bearer_context_dyn_p->cm_call_id,
                        bearer_context_p->ds_bearer_context_dyn_p->call_mode);
      continue;
    }

    switch (bearer_context_p->ds_bearer_context_dyn_p->state)
    {
      case DS_BEARER_CONTEXT_STATE_UP_DORMANT_REESTAB:
      case DS_BEARER_CONTEXT_STATE_UP_DORMANT:
      case DS_BEARER_CONTEXT_STATE_UP:
      case DS_BEARER_CONTEXT_STATE_GOING_DOWN:
        
      {      
      
        /*---------------------------------------------------------------------
        Context transfer cannot be made if 
        current call mode is LTE and the received target rat is also LTE 
        current call mode is GWT and the received target rat is also GWT.

          The following scenario needs to be considered:
          1. A non-transferrable bearer is established on LTE.
          2. Due to CSFB call, UE moves to GSM/WCDMA/TDSCDMA, 
             bearer_mode didn't change.
          3. CM SS event reports Full Srv on GSM, attach SM reset to NULL state.
             DSD notified.
          4. UE comes back to LTE, attach SM remains in NULL state and preferred system 
             doesn't change.

          To handle this scenario, the second context_transfer_ind is not
          ignored if bearer is not-transferable.
        ---------------------------------------------------------------------*/
        if ((bearer_context_p->ds_bearer_context_dyn_p->call_mode == SYS_SYS_MODE_LTE && 
             target_rat == SYS_SYS_MODE_LTE && 
             bearer_context_p->ds_bearer_context_dyn_p->is_transferable_to_GW == TRUE)||
            (bearer_context_p->ds_bearer_context_dyn_p->call_mode != SYS_SYS_MODE_LTE
             && target_rat != SYS_SYS_MODE_LTE))
        {      
             DS_3GPP_MSG3_HIGH("call mode:%d not transferable to %d, maintaining mode:%d",
                        bearer_context_p->ds_bearer_context_dyn_p->call_mode,
                        target_rat,bearer_context_p->ds_bearer_context_dyn_p->call_mode);
             continue;
        }

      if (bearer_context_p->ds_bearer_context_dyn_p->call_mode != target_rat)
      {	
        DS_3GPP_MSG2_HIGH("cntxt_transfer_ind_release for Bearer_inst:%d, call_mode:%d",
           (uint32)bearer_cntx_inst,bearer_context_p->ds_bearer_context_dyn_p->call_mode);
	DS_BEARER_CTXT_VF_CALL_NO_ARG_PER_BEARER
        (   
          cntxt_transfer_ind_release,
          bearer_context_p
        );
      }

      bearer_context_p->ds_bearer_context_dyn_p->call_mode = target_rat;

      if(bearer_context_p->ds_bearer_context_dyn_p->cm_call_id != CM_CALL_ID_INVALID)
      {   
          
        DS_3GPP_MSG3_HIGH("Changing mode for call id:%d :mode:%d",
                                        bearer_context_p->ds_bearer_context_dyn_p->
                          cm_call_id,target_rat,0);

        if( !ds3g_msh_set_new_call_mode(bearer_context_p->ds_bearer_context_dyn_p->
                                        cm_call_id,
                  target_rat ) )
        {
          DS_3GPP_MSG1_ERROR("Mode set failed for call id :%d", 
                    bearer_context_p->ds_bearer_context_dyn_p->cm_call_id);
        } 
       
      } /* !CM_CALL_ID_INVALID */


      /*In case of Lte; we will wait for Wcdma/Gsm Watermark to go empty; 
      in case of wcdma/gsm/tds we will wait for Lte watermark to go empty*/
      if (bearer_context_p->ds_bearer_context_dyn_p->call_mode == SYS_SYS_MODE_LTE)
      {
        rx_wm_p = &(bearer_context_p->rx_wm.wm_item);
      }
      else
      {
        rx_wm_p = &(bearer_context_p->lte_rx_wm.wm_item);
      }

     
      if(bearer_context_p->ds_bearer_context_dyn_p->is_registered_with_dpm)
      {
        if(dsm_is_wm_empty(rx_wm_p))
        {
          DS_3GPP_MSG3_HIGH("cntxt_transfer_ind_setup for Bearer_inst:%d, call_mode:%d",
              (uint32)bearer_cntx_inst,bearer_context_p->ds_bearer_context_dyn_p->call_mode,0);

     DS_BEARER_CTXT_VF_CALL_NO_ARG_PER_BEARER
      (   
        cntxt_transfer_ind_setup,
          bearer_context_p
      );

            DS_3GPP_MSG3_HIGH("Context Transfer Dpm Reregistration: %d",\
                        bearer_context_p->phys_link.bearer_id,0,0); 
            ds_bearer_cntx_reregister_dpm(bearer_context_p);
          }
        else
        {
          DS_3GPP_MSG3_HIGH("Context Transfer Registering for Callback: %d",\
		                   bearer_context_p->phys_link.bearer_id,0,0);   
          rx_wm_p->gone_empty_func_ptr = ds_bearer_cntxt_dpm_dereg_cb;
          rx_wm_p->gone_empty_func_data = (void*) bearer_context_p; 
        }
      }

        ret_val = TRUE;
        break;

    }// if bearer is UP/UP_DORMANT/UP_DORMANT_REESTAB

            case DS_BEARER_CONTEXT_STATE_PEND_REDIAL:
        {
          DS_3GPP_MSG2_HIGH("Context Transfer Indication received in Pending Redial state, "
                            "bearer instance: %d,new_mode: %d", 
                             bearer_cntx_inst,
                             target_rat);
                                             
       /*----------------------------------------------------------------------
             Here we check whether the system moved from G/W to L and back to G/W. 
             We also check whether the call id is INVALID.
             If both conditions are true then we need to cleanup the call.  
             Otherwise the call will be stuck in Pending redial state.
  
             We need to handle the case of L -> G/W -> L once we support pending redial
             calls while moving from L -> G/W.
         -----------------------------------------------------------------------------*/ 

        /*------------------------------------------------------------------------------- 
          Also tear down a call in PENDING REDIAL STATE if another bearer is in UP state
          Ex of such a case
          Call 1 brought up on G/W/T on the non attach profile  --> Bearer is UP
          Call 2 in coming up state on G/W/T but on the attach profile, Now the call 2
          is marked as PENDING REDIAL but no redial ever happens on moving to LTE because
          call 1's bearer was UP, If not for this check this bearer will be left hanging
          in pending redial state on LTE.
          -------------------------------------------------------------------------------*/
         
            if (bearer_context_p->ds_bearer_context_dyn_p->cm_call_id == CM_CALL_ID_INVALID)
          {
            DS_3GPP_MSG3_HIGH("Call id is invalid",0,0,0);

          if ((bearer_context_p->ds_bearer_context_dyn_p->call_mode != SYS_SYS_MODE_LTE &&
              (target_rat != SYS_SYS_MODE_LTE)) ||
              (ds_bearer_cntx_is_any_other_bearer_up(bearer_context_p) && 
               target_rat == SYS_SYS_MODE_LTE))
            {
            DS_3GPP_MSG3_HIGH("Cleaning up bearer context:0x%x in PENDIGN REDIAL state",
                              bearer_context_p,0,0);
              ds_bearer_cntx_local_cleanup(bearer_context_p);
            }
          }
                                             
          break;
        } // if bearer is PEND_REDIAL
  
        default:
        {
          DS_3GPP_MSG1_HIGH("Bearer transfer indication received in %d state, ignoring",
                            bearer_context_p->ds_bearer_context_dyn_p->state);
          break;
        }
      } 
  }//end for each bearer    

  return ret_val;
}/* ds_bearer_cntxt_transfer_ind */

/*===========================================================================
FUNCTION  DS_3GPP_BEARER_CNTXT_RESET_SRV_REQ_THROT

DESCRIPTION
  This function resets service request throttling SM. This is called during
  RAT change. 

PARAMETERS
  sys_modem_as_id_e_type  - subscription ID. 
  sys_sys_mode_e_type     - new mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_bearer_cntxt_reset_srv_req_throt
(
  sys_modem_as_id_e_type subs_id,
  sys_sys_mode_e_type    target_rat
)
{
  ds_cmd_type              *cmd_ptr = NULL;
  uint32                    timerval = 0;
  ds_bearer_cntxt_per_subs_info_type  *per_subs_info_hndl = NULL;
    sys_modem_as_id_e_type            *subs_id_p = NULL;
  
  /*---------------------------------------------------------------------
    Clear the service request throttling on context transfer from L to 
    W/TD/G(L -> W/TD/G IRAT) or W/TD -> L
  ---------------------------------------------------------------------*/	
  ds_3gpp_throt_sm_go_null(subs_id,FALSE);

  timerval = ds_3gpp_bearer_cntxt_get_retry_timer_val(TRUE, subs_id);
	   
  DS_3GPP_MSG3_HIGH("Reset Srv Req throt: throttle timer is running "
                    "throttle timer value: %d", timerval, 0, 0);

  per_subs_info_hndl = ds_bearer_cntxt_get_per_subs_info_hndl(subs_id);

  if (!ds_bearer_cntxt_validate_per_subs_info_hndl
      (per_subs_info_hndl))
  {
    DS_3GPP_MSG1_ERROR("Bearer Cntxt per subs info handle is invalid for"
                       "subs id",subs_id);
    return;
  } 

  if( (per_subs_info_hndl->timer_running == TRUE) && (timerval > 0) )
  {       
    /*---------------------------------------------------------------------
      Clear the RAB reestablish timer. This will be started again either in
      rab_re_est_ind or in rab_re_est_rej handler or in the registered timer
      call back.
      post DS3GPP_REORIG_DORM_TMR_EXP_CMD command for requesting rab 
      re-establishment on conncted handover               
    -----------------------------------------------------------------------*/
      DS_3GPP_MSG0_HIGH("ds_bearer_cntxt_transfer_ind: clearing back off timer"
                      " to allow srv req on context transfer");
      DS_BEARER_CNTXT_CLR_TIMER (per_subs_info_hndl); 

      ds_bearer_cntxt_reset_access_barring_retry();
    if( target_rat == SYS_SYS_MODE_GSM )
    {  
      return;
    }

      if( (cmd_ptr = ds_allocate_cmd_buf(sizeof(sys_modem_as_id_e_type))) == NULL ||
           cmd_ptr->cmd_payload_ptr == NULL )
    {
        ASSERT(0);
        return;
    }

      cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_DORM_REORIG_TMR_EXP;
      subs_id_p = (sys_modem_as_id_e_type*)cmd_ptr->cmd_payload_ptr;
      *subs_id_p = subs_id;

      DS_3GPP_MSG0_HIGH("ds_bearer_cntxt_transfer_ind: posting DS3GPP_REORIG"
                        "_DORM_TMR_EXP_CMD to trigger srv req on connected HO");

      ds_put_cmd_ext(cmd_ptr);
    }
} /* ds_3gpp_bearer_cntxt_reset_srv_req_throt */         		

#endif /*defined FEATURE_LTE && defined(FEATURE_GSM_GPRS || FEATURE_DATA_WCDMA_PS || FEATURE_TDSCDMA)*/

/*===========================================================================
FUNCTION  DS_3GPP_BEARER_CONTEXT_REFRESH_ACTUAL_THROUGHPUT

DESCRIPTION
  This utility function invokes the mode specific functions to refresh the
  current actual UL throughput, based on the current system mode
 

PARAMETERS
  sys_modem_as_id_e_type  - subscription ID 
  uint32                  - sampling interval to calculate the actual
                            throughput rate
DEPENDENCIES
  Assumes bearer is already allocated.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_3gpp_bearer_context_refresh_actual_throughput
(
  sys_modem_as_id_e_type    cm_subs_id,
  uint32                    sampling_interval
)
{
  boolean                   ret_val           = FALSE;
  uint8                     index             = 0;
  ds_bearer_context_s      *bearer_context_p  = NULL;
  boolean                   refresh_status    = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------- 
    If the timer has just started, initialize the baerer throughput stats
    to calculate a better estimate of the actual throughput when
    the timer expires
    -------------------------------------------------------------------------*/
  if (sampling_interval == 0)
  {
    DS_3GPP_MSG0_HIGH("Sampling Interval is 0 msec. Skip refreshing actual"
                      " throughput");
    /*----------------------------------------------------------------------- 
      Update the current actual throughput count numbers so that we may be
      able to get a better estimate of the actual throughput for all
      active bearers
      -----------------------------------------------------------------------*/
    ds_3gpp_bearer_context_init_bearer_throughput_stats(cm_subs_id);
    return ret_val;
  }

  /*------------------------------------------------------------------------- 
    Validate the subscription ID
    -------------------------------------------------------------------------*/
  if (!ds3gsubsmgr_is_subs_id_valid(cm_subs_id))
  {
    return ret_val;
  }
  /*-------------------------------------------------------------------------- 
    Invoke the mode specific function to refresh actual throughput
    --------------------------------------------------------------------------*/
  for (index = 0; index < DS_3GPP_MAX_BEARER_CONTEXT; index++)
  {
    bearer_context_p = ds_bearer_cntxt_get_instance_ptr(index);

    if (!ds_bearer_cntx_validate_bearer_subscription_internal(bearer_context_p,
                                                     cm_subs_id))
    {
      continue;
    }
    /*----------------------------------------------------------------------- 
      Refresh the actual throughput values, based on the current bearer
      call mode
      -----------------------------------------------------------------------*/
    DS_BEARER_CTXT_VFR_CALL_PER_BEARER
    (
       refresh_status,
       refresh_actual_throughput,
       bearer_context_p,
       cm_subs_id,
       sampling_interval
    );
    /*----------------------------------------------------------------------- 
      Return Success if atleast one bearer actual throughput stats are
      refreshed successfully
      -----------------------------------------------------------------------*/
    ret_val |= refresh_status;
  }

  if (ret_val == FALSE)
  {
    DS_3GPP_MSG1_HIGH("No active bearers on Subs Id(CM) %d",cm_subs_id);
  }
  return ret_val;
} /* ds_3gpp_bearer_context_refresh_actual_throughput */

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_CLEANUP_PENDING_CALLS

DESCRIPTION
  This function cleans up bearers in PEND_REDIAL state after a rat change. 
  
PARAMETERS  
  bearer_context_p - bearer context pointer
  new_mode         - new_rat
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
static void ds_bearer_cntxt_cleanup_pending_calls
(
  ds_bearer_context_s  *bearer_context_p,
  sys_sys_mode_e_type  new_mode
)
{
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }

  if ( (bearer_context_p->ds_bearer_context_dyn_p->state == DS_BEARER_CONTEXT_STATE_PEND_REDIAL) &&
       (bearer_context_p->ds_bearer_context_dyn_p->cm_call_id == CM_CALL_ID_INVALID) )
  {
    /*------------------------------------------------------------------------------- 
      1. Here we check whether the system moved from G/W to L and back to G/W. 
         We also check whether the call id is INVALID.
         If both conditions are true then we need to cleanup the call.  
         Otherwise the call will be stuck in Pending redial state.
  
       - We need to handle the case of L -> G/W -> L once we support pending redial
         calls while moving from L -> G/W.
    
      2. Also tear down a call in PENDING REDIAL STATE if another bearer is in UP 
         state.
         Ex of such a case
         Call 1 brought up on G/W/T on the non attach profile  --> Bearer is UP
         Call 2 in coming up state on G/W/T but on the attach profile, 
         Now the call 2 is marked as PENDING REDIAL but no redial ever happens on 
         moving to LTE because call 1's bearer was UP, If not for this check this 
         bearer will be left hanging in pending redial state on LTE.
    -------------------------------------------------------------------------------*/
    if ( ((bearer_context_p->ds_bearer_context_dyn_p->call_mode != SYS_SYS_MODE_LTE) &&
         (new_mode != SYS_SYS_MODE_LTE)) ||
         (ds_bearer_cntx_is_any_other_bearer_up(bearer_context_p) && 
          new_mode == SYS_SYS_MODE_LTE) )
    {
      DS_3GPP_MSG1_HIGH("Bearer_inst: %d is in PEND_REDIAL state, cleaning...",
                        (uint32)(bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr));
      ds_bearer_cntx_local_cleanup(bearer_context_p);
    }
  }

} /* ds_bearer_cntxt_cleanup_pending_calls */


#ifdef FEATURE_QXDM_WM_TRACING
/*===========================================================================
FUNCTION DS_BEARER_CNTXT_REPORT_WM_STATS

DESCRIPTION
  This function internally call umts or lte specific function to report wm 
  status. 
  
PARAMETERS  
  bearer_instance - bearer context instance
  wm_stat_ptr     - pointer where we store the wm values
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_bearer_cntxt_report_wm_stats
(
  uint32          bearer_instance,
  fc_wm_stat_type *wm_stat_ptr
)
{
  ds_bearer_context_s    *bearer_context_p = NULL;
  sys_modem_as_id_e_type  subs_id = SYS_MODEM_AS_ID_NONE;
  /*----------------------------------------------------------------------*/

  /*---------------------------------------------------------------------------
    Get the bearer context pointer
  ---------------------------------------------------------------------------*/
  bearer_context_p = ds_bearer_cntxt_get_instance_ptr(bearer_instance);

  if (!ds_bearer_cntxt_get_subs_id_from_bearer(bearer_context_p, &subs_id))
  {
    return;
  }

  DS_BEARER_CTXT_VF_CALL
  (    
    report_wm_stats,
    subs_id,
    bearer_instance,
    wm_stat_ptr
  );
} /* ds_bearer_cntxt_report_wm_stats */
#endif /* FEATURE_QXDM_WM_TRACING */

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_START_BP_TIMER

DESCRIPTION
  This function starts the BP removal timer
  
PARAMETERS  
  *bearer_cntxt_p  - bearer_context pointer
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_bearer_cntxt_start_bp_timer
(
  ds_bearer_context_s  *bearer_cntxt_p,
  sys_modem_as_id_e_type subs_id
)
{
  ps_timer_handle_type  tmr_hndl_dyn_back_pressure;
  uint32                bearer_inst;
  ds_bearer_cntxt_tput_opt_struct_type tput_opt_params;

  /*------------------------------------------------------------------------
    The timer is started is in PS task context, this code is to 
    prevent a race condition between PS and DS task when timer starts at
    the same time when network ends the data call.
  ------------------------------------------------------------------------*/
  ds3gpp_enter_global_crit_section();
  if(!ds_bearer_cntx_validate_bearer_context(bearer_cntxt_p))
  {
    ds3gpp_leave_global_crit_section();
    return;
  }

  tput_opt_params = ds_3gpp_nv_get_back_pressure_params(subs_id);
  tmr_hndl_dyn_back_pressure = 
        bearer_cntxt_p->ds_bearer_context_dyn_p->tmr_hndl_dyn_back_pressure;
  bearer_inst = 
        (uint32)(bearer_cntxt_p->ds_bearer_context_dyn_p->client_data_ptr);
  ds3gpp_leave_global_crit_section();

  if (TRUE == ps_timer_is_running(tmr_hndl_dyn_back_pressure))
  {
    DS_3GPP_MSG0_HIGH("BP timer is already running, not starting again");
    return;
  }

  /*--------------------------------------------------------------------------
    It is possible that the  IRAT from G to W, data was resumed and a
    DL TCP session started, in this case BP timer handle would not have been 
    allocated.
  --------------------------------------------------------------------------*/
  if (PS_TIMER_INVALID_HANDLE == tmr_hndl_dyn_back_pressure)
  {
    DS_3GPP_MSG0_HIGH("disable_back_pressure(): timer_hdl not allocated yet."
                      "Allocate new hndl");

    tmr_hndl_dyn_back_pressure
        = ps_timer_alloc(ds_bearer_cntxt_tmr_dyn_back_pressure_exp_hdlr,
                         (void *)bearer_cntxt_p);

    if (PS_TIMER_INVALID_HANDLE == tmr_hndl_dyn_back_pressure)
    {
      DS_3GPP_MSG1_ERROR("Invalid PS timer handle for bearer inst: %d",
                          bearer_inst);
      return;
    }
  }

  if (PS_TIMER_SUCCESS != 
      ps_timer_start(tmr_hndl_dyn_back_pressure, 
                     (1000 * tput_opt_params.timer_val)))
  {
    DS_3GPP_MSG1_ERROR("Failed to start timer for bearer inst: %d",
                        bearer_inst);
  }
  else
  {
    DS_3GPP_MSG1_HIGH("Started BP timer for %d seconds", 
                        tput_opt_params.timer_val);
  }

  ds3gpp_enter_global_crit_section();
  if(!ds_bearer_cntx_validate_bearer_context(bearer_cntxt_p))
  {
    ps_timer_free(tmr_hndl_dyn_back_pressure);
    ds3gpp_leave_global_crit_section();
    return;
  }

  /* Update bearer_cntxt_p with new timer handle if its allocated here */
  bearer_cntxt_p->ds_bearer_context_dyn_p->tmr_hndl_dyn_back_pressure =
                                                   tmr_hndl_dyn_back_pressure;  
  ds3gpp_leave_global_crit_section();     
}/* ds_bearer_cntxt_start_bp_timer */

/*===========================================================================
FUNCTION DS_WCDMA_BEARER_CNTXT_TMR_BACK_PRESSURE_DISABLE_EXP_HDLR

DESCRIPTION
  This is the timer exp hdlr for dynamic back pressure disable. This would be 
  called in PS task context.

PARAMETERS
  user_data_ptr - user data.

DEPENDENCIES
  None.

RETURN VALUE
  None 

SIDE EFFECTS
  None.
===========================================================================*/
void ds_bearer_cntxt_tmr_dyn_back_pressure_exp_hdlr
(
  void              *user_data_ptr
)
{
  uint32                  wm_hi_level = 0;
  ds_bearer_context_s     *bearer_cntxt_p = NULL;
  ds_bearer_context_wm_s  *watermark_p    = NULL;
  uint32                  new_ack_cnt = 0;
  ds_pdn_context_s                     *pdn_cntx_p = NULL;
  sys_modem_as_id_e_type   subs_id= SYS_MODEM_AS_ID_NONE;
  ds_bearer_cntxt_tput_opt_struct_type tput_opt_params;
  ds_bearer_cntxt_um_wm_threshold_struct_type  um_wm_threshold;
  ds_bearer_cntxt_tds_um_wm_threshold_struct_type  tds_um_wm_threshold; 

  DS_3GPP_MSG3_HIGH("TMR Dynamic BP exp hdlr", 0,0,0);

  bearer_cntxt_p = (ds_bearer_context_s *)user_data_ptr;

  /*------------------------------------------------------------------------
    The timer expiry processing is in PS task context, this code is to 
    prevent a race condition between PS and DS task when timer expires at
    the same time when network ends the data call.
  ------------------------------------------------------------------------*/
  ds3gpp_enter_global_crit_section();

  if(!ds_bearer_cntx_validate_bearer_context(bearer_cntxt_p))
  {
    ds3gpp_leave_global_crit_section();
    return;
  }

  pdn_cntx_p = bearer_cntxt_p->ds_bearer_context_dyn_p->pdn_context_ptr;
  if (!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    ds3gpp_leave_global_crit_section();
    return;
  }

  if(! ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p,&subs_id))
  {
    ds3gpp_leave_global_crit_section();
    return;
  }

  if (ds3gpp_get_ps_data_suspend_flag(subs_id) == TRUE)
  {
    DS_3GPP_MSG0_HIGH("Data Suspend is true, ignoring back pressure tmr_exp");
    /*----------------------------------------------------------------------
      Reset the Ack counters
    ----------------------------------------------------------------------*/
    bearer_cntxt_p->ds_bearer_context_dyn_p->tcp_dl_ack_counter =
           bearer_cntxt_p->ds_bearer_context_dyn_p->last_tcp_dl_ack_count = 0;
    ds3gpp_leave_global_crit_section();
    return;
  }

  tput_opt_params = ds_3gpp_nv_get_back_pressure_params(subs_id);
  um_wm_threshold = ds_3gpp_nv_get_um_wm_threshold(subs_id);
  tds_um_wm_threshold =  ds_3gpp_nv_get_tds_wm_threshold(subs_id);


    /*------------------------------------------------------------------------
      Ensure that BP timer is restarted and BP remains disabled only if the 
      number of new ACKs received in the BP timer duration equals the threshold 
      ACK cnt. This ensures that low throughput background TCP DL traffic 
      doesn't trigger BP removal.
    ------------------------------------------------------------------------*/
    new_ack_cnt = (uint32)(bearer_cntxt_p->ds_bearer_context_dyn_p->tcp_dl_ack_counter - 
                   bearer_cntxt_p->ds_bearer_context_dyn_p->last_tcp_dl_ack_count);

    if (new_ack_cnt < tput_opt_params.tcp_threshold_ack_cnt)
    {
      DS_3GPP_MSG3_HIGH("Curr_ack_cnt %d, last_ack_cnt: %d. Enabling BP for bearer inst: %d",
                       bearer_cntxt_p->ds_bearer_context_dyn_p->tcp_dl_ack_counter,
                       bearer_cntxt_p->ds_bearer_context_dyn_p->last_tcp_dl_ack_count,
                       (uint32)bearer_cntxt_p->ds_bearer_context_dyn_p->client_data_ptr);

      /*----------------------------------------------------------------------
        Reset the Ack counters and enable back pressure
      ----------------------------------------------------------------------*/
      bearer_cntxt_p->ds_bearer_context_dyn_p->tcp_dl_ack_counter =
             bearer_cntxt_p->ds_bearer_context_dyn_p->last_tcp_dl_ack_count = 0;

      bearer_cntxt_p->tx_wm.wm_item.hiwater_func_ptr = 
                         (wm_cb_type)ds_bearer_cntxt_ps_to_L2_flow_disable_cb;

      /*------------------------------------------------------------------------
        Reset HI_WM to original value
      ------------------------------------------------------------------------*/
      if (bearer_cntxt_p->ds_bearer_context_dyn_p->call_mode == SYS_SYS_MODE_LTE)
      {
        wm_hi_level = um_wm_threshold.lte_ul_hi;
      }
      else if (bearer_cntxt_p->ds_bearer_context_dyn_p->call_mode == 
               SYS_SYS_MODE_WCDMA)
      {
        wm_hi_level = um_wm_threshold.wcdma_ul_hi;
      }
      else if (bearer_cntxt_p->ds_bearer_context_dyn_p->call_mode == 
               SYS_SYS_MODE_GSM)
      {
        wm_hi_level = um_wm_threshold.gprs_ul_hi;
      }
#ifdef FEATURE_TDSCDMA
      else if (bearer_cntxt_p->ds_bearer_context_dyn_p->call_mode == SYS_SYS_MODE_TDS)
      {
        switch (bearer_cntxt_p->tx_wm.wm_item.lo_watermark)
        {
           case DSTPSD_UL_WM_LO_16:
             wm_hi_level = DSTPSD_UL_WM_HI_16;
             break;
           case DSTPSD_UL_WM_LO_32:
             wm_hi_level = DSTPSD_UL_WM_HI_32;
             break;
           case DSTPSD_UL_WM_LO_64:
             wm_hi_level = DSTPSD_UL_WM_HI_64;
             break;
           case DSTPSD_UL_WM_LO_128:
             wm_hi_level = DSTPSD_UL_WM_HI_128;
             break;
           case DSTPSD_UL_WM_LO_256:
             wm_hi_level = DSTPSD_UL_WM_HI_256;
             break;
           case DSTPSD_UL_WM_LO_384:
             wm_hi_level = DSTPSD_UL_WM_HI_384;
             break;
           default:
             wm_hi_level = tds_um_wm_threshold.tds_ul_hi;
             break;
          }
      }
#endif /* FEATURE_TDSCDMA */
      else
      {
        DS_3GPP_MSG3_ERROR("invalid call_mode: %d in bearer_inst: %d",
                            bearer_cntxt_p->ds_bearer_context_dyn_p->call_mode, 
                            (uint32)(bearer_cntxt_p->ds_bearer_context_dyn_p
                                     ->client_data_ptr),
                             0);
        
        ds3gpp_leave_global_crit_section();
        return;
      }
      
      ds3gpp_leave_global_crit_section();
    
      watermark_p = &(bearer_cntxt_p->tx_wm);
      if (NULL != watermark_p)
      {
      DS_3GPP_MSG1_HIGH("Setting HI WM level back to %d", wm_hi_level);
        dsm_set_hi_wm(&(watermark_p->wm_item), wm_hi_level);
      }
    }
    else
    {
      DS_3GPP_MSG3_HIGH("Curr_ack_cnt %d, last_ack_cnt: %d, restart timer for bearer inst: %d",
                          bearer_cntxt_p->ds_bearer_context_dyn_p->
                          tcp_dl_ack_counter,
                          bearer_cntxt_p->ds_bearer_context_dyn_p->
                          last_tcp_dl_ack_count,
                          (uint32)bearer_cntxt_p->ds_bearer_context_dyn_p->
                          client_data_ptr);

      ds3gpp_leave_global_crit_section();

      /* Disable Back Pressure */
      ds_bearer_cntxt_disable_back_pressure(bearer_cntxt_p);

      /* Start the timer */
      ds_bearer_cntxt_start_bp_timer(bearer_cntxt_p,subs_id);
    }

} /* ds_wcdma_bearer_cntxt_tmr_back_pressure_disable_exp_hdlr */

/*===========================================================================
FUNCTION DS_WCDMA_BEARER_CNTXT_DISABLE_BACK_PRESSURE

DESCRIPTION
  This function forwards pkts received to the lower layers for 
  pdp-ip call.
  This function is registered with ps_iface for a PDP-IP call 
  
PARAMETERS  
  *bearer_cntxt_p  - bearer_context pointer
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_bearer_cntxt_disable_back_pressure
(
  ds_bearer_context_s  *bearer_cntxt_p
)
{
  uint32 wm_dne_level = 0;
  ds_bearer_context_wm_s  *watermark_p = NULL;
  ds_pdn_context_s                     *pdn_cntx_p = NULL;
  sys_modem_as_id_e_type   subs_id= SYS_MODEM_AS_ID_NONE;
  ds_bearer_cntxt_um_wm_threshold_struct_type  um_wm_threshold;
  ds_bearer_cntxt_tds_um_wm_threshold_struct_type  tds_um_wm_threshold; 

  /*------------------------------------------------------------------------
    The timer is started is in PS task context, this code is to 
    prevent a race condition between PS and DS task when timer starts at
    the same time when network ends the data call.
  ------------------------------------------------------------------------*/
  ds3gpp_enter_global_crit_section();

  if(!ds_bearer_cntx_validate_bearer_context(bearer_cntxt_p))
  {
    ds3gpp_leave_global_crit_section();
    return;
  }

  pdn_cntx_p = bearer_cntxt_p->ds_bearer_context_dyn_p->pdn_context_ptr;
  if (!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    ds3gpp_leave_global_crit_section();
    return;
  }

  if(!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p,&subs_id))
  {
    ds3gpp_leave_global_crit_section();
    return;
  }

  DS_3GPP_MSG2_HIGH("Disable back pressure for bearer_inst: %d, bearer_mode: %d",
                    (uint32)(bearer_cntxt_p->ds_bearer_context_dyn_p->client_data_ptr),
                    (bearer_cntxt_p->ds_bearer_context_dyn_p->call_mode));

  /*-------------------------------------------------------------------------
    Disable back pressure as UL Acks are being received
  -------------------------------------------------------------------------*/
  bearer_cntxt_p->tx_wm.wm_item.hiwater_func_ptr = NULL;
  um_wm_threshold = ds_3gpp_nv_get_um_wm_threshold(subs_id);
  tds_um_wm_threshold =  ds_3gpp_nv_get_tds_wm_threshold(subs_id);


  /*-------------------------------------------------------------------------
    Set HI_WM to DNE
  -------------------------------------------------------------------------*/
  if (bearer_cntxt_p->ds_bearer_context_dyn_p->call_mode == SYS_SYS_MODE_LTE)
  {
    wm_dne_level = um_wm_threshold.lte_ul_dne;
  }
  else if (bearer_cntxt_p->ds_bearer_context_dyn_p->call_mode == SYS_SYS_MODE_WCDMA)
  {
    wm_dne_level = um_wm_threshold.wcdma_ul_dne;
  }
  else if (bearer_cntxt_p->ds_bearer_context_dyn_p->call_mode == SYS_SYS_MODE_GSM)
  {
    wm_dne_level = um_wm_threshold.gprs_ul_dne;
  }
#ifdef FEATURE_TDSCDMA
  else if (bearer_cntxt_p->ds_bearer_context_dyn_p->call_mode == SYS_SYS_MODE_TDS)
  {
    switch (bearer_cntxt_p->tx_wm.wm_item.lo_watermark)
    {
       case DSTPSD_UL_WM_LO_16:
         wm_dne_level = DSTPSD_UL_WM_DNE_16;
         break;
       case DSTPSD_UL_WM_LO_32:
         wm_dne_level = DSTPSD_UL_WM_DNE_32;
         break;
       case DSTPSD_UL_WM_LO_64:
         wm_dne_level = DSTPSD_UL_WM_DNE_64;
         break;
       case DSTPSD_UL_WM_LO_128:
         wm_dne_level = DSTPSD_UL_WM_DNE_128;
         break;
       case DSTPSD_UL_WM_LO_256:
         wm_dne_level = DSTPSD_UL_WM_DNE_256;
         break;
       case DSTPSD_UL_WM_LO_384:
         wm_dne_level = DSTPSD_UL_WM_DNE_384;
         break;
       default:
         wm_dne_level = tds_um_wm_threshold.tds_ul_dne;
         break;
      }

  }
#endif /* FEATURE_TDSCDMA */
  else
  {
    DS_3GPP_MSG2_ERROR("invalid call_mode: %d in bearer_inst: %d",
                        bearer_cntxt_p->ds_bearer_context_dyn_p->call_mode, 
                        (uint32)(bearer_cntxt_p->ds_bearer_context_dyn_p->client_data_ptr));
    ds3gpp_leave_global_crit_section();
    return;
  }

  /*-------------------------------------------------------------------------
    Update the last_tcp_ack_count and start the timer
  -------------------------------------------------------------------------*/
  bearer_cntxt_p->ds_bearer_context_dyn_p->last_tcp_dl_ack_count = 
    bearer_cntxt_p->ds_bearer_context_dyn_p->tcp_dl_ack_counter;

  ds3gpp_leave_global_crit_section();
  
  watermark_p = &(bearer_cntxt_p->tx_wm);
  if (NULL != watermark_p)
  {
    DS_3GPP_MSG1_HIGH("Setting UL WM HI to DNE: %d", wm_dne_level);
    dsm_set_hi_wm(&(watermark_p->wm_item), wm_dne_level);
  }
} /* ds_bearer_cntxt_disable_back_pressure */


/*===========================================================================
FUNCTION DS_BEARER_CNTXT_CLEANUP_CALL_RESOURCES

DESCRIPTION
  This function clears bearer resources when call end is initiated.
  
PARAMETERS  
  pdn_cntx_p - PDN context for which the bearer needs to be cleaned.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_bearer_cntxt_cleanup_call_resources
(
  ds_pdn_context_s  *pdn_cntx_p
)
{
  uint8                bearer_index;
  ds_bearer_context_s  *bearer_context_p = NULL;
  ds_bearer_cntxt_per_subs_info_type  *per_subs_info_hndl = NULL;
  sys_modem_as_id_e_type               subs_id = SYS_MODEM_AS_ID_NONE;
  /*------------------------------------------------------------------------*/

  if(!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p, &subs_id))
  {
     return;
  }

  per_subs_info_hndl = 
    ds_bearer_cntxt_get_per_subs_info_hndl(subs_id);

  if (!ds_bearer_cntxt_validate_per_subs_info_hndl
        (per_subs_info_hndl))
  {
    DS_3GPP_MSG1_ERROR("Bearer Cntxt per subs info handle is invalid for"
                       "Subs Id(CM): %d",subs_id);
    return;
  }
  /*-------------------------------------------------------------------------
    Unset the bitmask for all bearer contexts in this pdn context
  -------------------------------------------------------------------------*/
  for(bearer_index = 0;
    bearer_index < DS_3GPP_MAX_BEARER_CONTEXT_PER_PDN_CONTEXT;
    bearer_index++)
  {
    bearer_context_p =\
      (ds_bearer_context_s*)pdn_cntx_p->ds_pdn_context_dyn_p->bearer_context_ptr_tbl[bearer_index];

    if(!ds_bearer_cntx_validate_bearer_context_internal(bearer_context_p))
    {
      /*----------------------------------------------------------------------- 
      This entry in the bearer context table is NULL. Other entries could
      have non-NULL values.
    -----------------------------------------------------------------------*/
      continue;
    }

    DS_BEARER_CNTXT_UNSET_BITMASK(per_subs_info_hndl, bearer_context_p);

    /*-----------------------------------------------------------------------
      Reset the rab_reestab_retry counter for this bearer.
    -----------------------------------------------------------------------*/
    DS_BEARER_CNTXT_RESET_RAB_REESTAB_RETRY_CNT(bearer_context_p);

    /*----------------------------------------------------------------------
      Stop the Dynamic BP timer before initiating call end
    -----------------------------------------------------------------------*/
    if ( (PS_TIMER_INVALID_HANDLE != 
          bearer_context_p->ds_bearer_context_dyn_p->tmr_hndl_dyn_back_pressure) &&
         (PS_TIMER_SUCCESS != 
          ps_timer_cancel(bearer_context_p->ds_bearer_context_dyn_p->tmr_hndl_dyn_back_pressure)) )
    {
      DS_3GPP_MSG1_ERROR("Error in stopping BP timer with hndl: %d",
                           bearer_context_p->ds_bearer_context_dyn_p->tmr_hndl_dyn_back_pressure);
    }
    else
    {
      DS_3GPP_MSG0_HIGH("BP timer stopped");
    }

    /* flush the watermark for this bearer */
    DS_BEARER_CTXT_VF_CALL_NO_ARG_PER_BEARER
    (
      empty_watermark,
      bearer_context_p    
    );
  }

  if((per_subs_info_hndl->uplink_data_bitmask == 0) && 
     (per_subs_info_hndl->timer_running == TRUE))
  { 
    DS_BEARER_CNTXT_CLR_TIMER (per_subs_info_hndl)
  }
} /* ds_bearer_cntxt_cleanup_call_resources */

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_IS_ACTIVE_BEARER_CNTXT

DESCRIPTION
  This function returns TRUE if there is an active bearer context in the
  system. Else, it returns FALSE.
.
  
PARAMETERS  
  pdn_cntx_p - PDN context for which the bearer needs to be cleaned.
  subs_id    - Subscription Id
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  boolean
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_bearer_cntxt_is_active_bearer_cntxt
(
  sys_modem_as_id_e_type subs_id
)
{
  ds_bearer_context_s* bearer_context_p; //Pointer to bearer context
  uint32               bearer_cntx_inst;
  ds_bearer_context_state_e state = DS_BEARER_CONTEXT_STATE_DOWN;
  /*-----------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    loop through and check if there is an active bearer context.
  -------------------------------------------------------------------------*/
  for( bearer_cntx_inst=0;\
       bearer_cntx_inst<DS_3GPP_MAX_BEARER_CONTEXT;\
       bearer_cntx_inst++ )
  {
    bearer_context_p = ds_bearer_cntxt_get_instance_ptr(bearer_cntx_inst);
    
    if(!ds_bearer_cntxt_validate_bearer_subscription(bearer_context_p,subs_id))
    {
       continue;
    }
            
    state = bearer_context_p->ds_bearer_context_dyn_p->state;

    if (state == DS_BEARER_CONTEXT_STATE_UP ||
        state == DS_BEARER_CONTEXT_STATE_UP_DORMANT ||
        state == DS_BEARER_CONTEXT_STATE_UP_DORMANT_REESTAB )
    {
      return TRUE;
    }
  }
  return FALSE;
}/*ds_bearer_cntxt_is_active_bearer_cntxt*/

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_BEARERS_ON_LTE

DESCRIPTION
  This function returns TRUE if all the active bearers are in LTE mode.
  Else, it returns FALSE.
  
PARAMETERS  
  subs_id - Subscription Id
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  boolean
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_bearer_cntxt_bearers_on_lte
(
  sys_modem_as_id_e_type subs_id
)
{
  ds_bearer_context_s* bearer_context_p; //Pointer to bearer context
  uint32               bearer_cntx_inst;
  boolean              bearer_valid_lte = FALSE;

  /*-------------------------------------------------------------------------
    loop through and check if there is any bearer which is not in LTE mode.
  -------------------------------------------------------------------------*/
  for( bearer_cntx_inst=0;
       bearer_cntx_inst < DS_3GPP_MAX_BEARER_CONTEXT;
       bearer_cntx_inst++ )
  {
    bearer_context_p = ds_bearer_cntxt_get_instance_ptr(bearer_cntx_inst);
    
    if(ds_bearer_cntx_validate_bearer_subscription_internal (bearer_context_p, subs_id))
    {
      bearer_valid_lte = TRUE;
          
     if ( (bearer_context_p->ds_bearer_context_dyn_p->call_mode != SYS_SYS_MODE_LTE) )
     {
       return FALSE;
     }
    }
  }

  return bearer_valid_lte;

} /* ds_bearer_cntxt_bearers_on_lte */

/*===========================================================================
FUNCTION ds_3gpp_bearer_cntx_alloc_dyn_mem

DESCRIPTION
  This function allocates dyn memory.
 
PARAMETERS
  index   - index where the memory would be allocated
  subs_id - Subscription Id
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

=============================================================================*/
static boolean ds_bearer_cntx_alloc_dyn_mem
(
  int                    index,
  sys_modem_as_id_e_type subs_id
)
{
  boolean ret_val = FALSE;
  ds_bearer_context_dyn_s *bearer_context_dyn_p = NULL;
  ds_bearer_context_s      *bearer_context_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((index < 0) || (index >= DS_3GPP_MAX_BEARER_CONTEXT_PER_PDN_CONTEXT))
  {
    DS_3GPP_MSG1_ERROR("Index:%d is out of bounds",index);
    return ret_val;
  }

  bearer_context_p = ds_bearer_cntxt_get_instance_ptr(index);

  bearer_context_dyn_p = bearer_context_p->ds_bearer_context_dyn_p;

  if (bearer_context_dyn_p == NULL)
  {
    bearer_context_dyn_p = (ds_bearer_context_dyn_s *)modem_mem_alloc
                         (sizeof(ds_bearer_context_dyn_s),MODEM_MEM_CLIENT_DATA);

    if (bearer_context_dyn_p != NULL)
    {
      memset(bearer_context_dyn_p,0,sizeof(ds_bearer_context_dyn_s));
      bearer_context_dyn_p->cookie = BEARER_CNTXT_VALID_COOKIE;
      bearer_context_p->ds_bearer_context_dyn_p = bearer_context_dyn_p;
      bearer_context_dyn_p->index = index;
      DS_3GPP_MSG2_HIGH("Bearer Context dyn memory allocated at index %d,"
                        "Subs Id(CM): %d", index, subs_id);
      ret_val = ds_3gpp_dyn_mem_hdlr_alloc_bearers_dyn_mem(index, subs_id);
      /*--------------------------------------------------------------------------- 
        If ret_val is not successful free umts/eps bearer contexts and then free
        bearer context too, We don't know what allocation failed and
        hence its safe to free everything
        ---------------------------------------------------------------------------*/
      if (ret_val)
      {
        DS_3GPP_MSG0_HIGH("Umts and EPS bearers dyn mem allocated");
      }
      else
      {
        ds_3gpp_dyn_mem_hdlr_free_bearers_dyn_mem(bearer_context_p, subs_id);
        ds_bearer_cntx_free_dyn_mem(bearer_context_p);
      }
    }
    else
    {
      DS_3GPP_MSG0_ERROR("Heap mem allocation failure");
    }
  }
  else
  {
    DS_3GPP_MSG1_ERROR("Bearer context already has dyn memory allocated at index:%d",
                       index);
  }

  return ret_val;
} /* ds_bearer_cntx_alloc_dyn_mem */

/*===========================================================================
FUNCTION DS_BEARER_CNTX_VALIDATE_BEARER_CONTEXT_INTERNAL

DESCRIPTION
  This function checks if the bearer context has been allocated dynamic memory

PARAMETERS
  bearer_context_p       - bearer_context to be validated
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_bearer_cntx_validate_bearer_context_internal
(
  ds_bearer_context_s *bearer_context_p
)
{
  ds_bearer_context_dyn_s *bearer_context_dyn_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(bearer_context_p == NULL)
  {
    return FALSE;
  }

  bearer_context_dyn_p = bearer_context_p->ds_bearer_context_dyn_p;

  if ((bearer_context_dyn_p != NULL) && 
       bearer_context_dyn_p->cookie == BEARER_CNTXT_VALID_COOKIE)

  {
    return TRUE;
  }

  return FALSE;
} /*ds_bearer_cntx_validate_bearer_context_internal*/

/*===========================================================================
FUNCTION DS_BEARER_CNTX_VALIDATE_BEARER_CONTEXT

DESCRIPTION
  This function checks if the bearer context has been allocated dynamic memory
  but report for failure

PARAMETERS
  bearer_context_p       - bearer_context to be validated
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_bearer_cntx_validate_bearer_context
(
  ds_bearer_context_s *bearer_context_p
)
{
  if( bearer_context_p == NULL ||
      !ds_bearer_cntx_validate_bearer_context_internal(bearer_context_p))
  {
    DS_3GPP_MSG1_ERROR("Bearer context:0x%x is INVALID",bearer_context_p);
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================
FUNCTION ds_bearer_cntx_free_bearer_context

DESCRIPTION
  This function frees dynamic bearer context

PARAMETERS
  bearer_context_p       - bearer_context to be freed
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
void ds_bearer_cntx_free_dyn_mem
(
  ds_bearer_context_s *bearer_context_p
)
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
{
  /*----------------------------------------------------------------------- 
    Don't care about the cookie here since we are freeing memory anyways.
   -----------------------------------------------------------------------*/
  if((bearer_context_p != NULL) && 
     (bearer_context_p->ds_bearer_context_dyn_p != NULL))
  {
    DS_3GPP_MSG0_MED("Freeing dyn allocated bearer pointer");
    modem_mem_free(bearer_context_p->ds_bearer_context_dyn_p,MODEM_MEM_CLIENT_DATA);
    bearer_context_p->ds_bearer_context_dyn_p = NULL;
  }
  else
  {
    DS_3GPP_MSG0_ERROR("Bearer context passed is invalid");
  }
}
/*===========================================================================
FUNCTION ds_3gpp_bearer_cntxt_rx_data_at_u_pdp_cb

DESCRIPTION
  This function is the nonempty callback function for the bearer's downlink
  watermark. As part of nonempty callback function processing we set the rx_sig

PARAMETERS
  bearer_context_p       - bearer_context to be freed
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_bearer_cntxt_rx_data_at_u_pdp_cb
(
 dsm_watermark_type *wm,
 void*              callback_data
)
{
  ds_bearer_context_s       *bearer_context_p;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Get call instance from the callbackdata passed in
  -------------------------------------------------------------------------*/
  bearer_context_p = (ds_bearer_context_s*) callback_data;
 
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }

  /*-------------------------------------------------------------------------
    Set Rx Signal.
  -------------------------------------------------------------------------*/
  /*lint -e641 */
  PS_SET_SIGNAL(bearer_context_p->rx_sig);
  /*lint +e641 */
} /* ds_eps_bearer_cntxt_ipa_to_ps_data_cb() */
    

/*===========================================================================
FUNCTION  DS_3GPP_BEARER_CNTXT_GET_NUM_FREE_BEARERS

DESCRIPTION
  This function returns the total number of free bearers in the
  bearer resource pool.
PARAMETERS 
  None
DEPENDENCIES
  None.

RETURN VALUE
  uint8     - total number of bearers in use

SIDE EFFECTS
  None.

===========================================================================*/
uint8 ds_3gpp_bearer_cntxt_get_num_free_bearers(void)
{
  int   num_free_bearers                    = 0;
  uint8 index                               = 0;  
  ds_bearer_context_s     *bearer_context_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (index =0; index <DS_3GPP_MAX_BEARER_CONTEXT; index++)
  {
    bearer_context_p = ds_bearer_cntxt_get_instance_ptr(index);

    if (!ds_bearer_cntx_validate_bearer_context_internal(bearer_context_p))
    {
      num_free_bearers++;
    }
  }
  return (uint8)num_free_bearers;
} /* ds_3gpp_bearer_cntxt_get_num_free_bearers */

/*===========================================================================
FUNCTION ds_bearer_cntx_report_ps_wmk_logging

DESCRIPTION
  This function reports PS watermark per IFace

PARAMETERS
  bearer_context_p       - bearer_context to be freed
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
void ds_bearer_cntxt_report_ps_wmk_logging
(
  ds_bearer_context_s             *bearer_context_p,
  ps_wmk_logging_dorm_state_type     ps_event
)
{
  ds_pdn_context_s             *pdn_cntx_p = NULL;
  ps_iface_type                *ps_iface_v4_p = NULL; 
  ps_iface_type                *ps_iface_v6_p = NULL; 
  int                           api_ret_val;

  if(bearer_context_p == NULL)
  {
    return;
  }

  DS_3GPP_MSG3_HIGH("WM logging onBearer instance: %d Tx WM count:%d,"
                    " Rx WM count:%d",
      (uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr,
       bearer_context_p->tx_wm.wm_item.current_cnt,
       bearer_context_p->rx_wm.wm_item.current_cnt);

  pdn_cntx_p = bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr;
  if(pdn_cntx_p == NULL)
  {
    DS_3GPP_MSG3_ERROR("PDN cntxt passed is not valid",0,0,0);
    return;
  }

  ps_iface_v4_p = DSPDNCNTXT_GET_V4_PS_IFACE_PTR(pdn_cntx_p);
  ps_iface_v6_p = DSPDNCNTXT_GET_V6_PS_IFACE_PTR(pdn_cntx_p);
 /*------------------------------------------------------------------------- 
   Indicate to PS the Physlink pointer, Rx watermark count,
   Tx watermark count and dormancy status.
   Dormancy status is set to REORIG_FROM_DORMANCY
 ---------------------------------------------------------------------------*/
  if (ps_iface_v4_p != NULL) 
  {
   /*------------------------------------------------------------------------- 
     Indicate to PS the Physlink pointer, Rx watermark count,
     Tx watermark count and dormancy status.
     Dormancy status is set to DORMANT
    -------------------------------------------------------------------------*/
    api_ret_val =  ps_wmk_logging_event_report_ex(&(bearer_context_p->phys_link),
                                  EVENT_DS_UM_QUEUE_STATS_EX ,
                                  ps_event,
                                  bearer_context_p->rx_wm.wm_item.current_cnt,
                                  bearer_context_p->tx_wm.wm_item.current_cnt,
                                  ps_iface_v4_p
                                  );

    if (api_ret_val == 0)
    {
      DS_3GPP_MSG3_HIGH("Succcesful Logging for event %d IPV4 PS Iface 0x%x"
                        ,ps_event,ps_iface_v4_p,0);
    }
    else
    {
      DS_3GPP_MSG3_ERROR("Failure to log event %d PS Iface IPV4 0x%x "
                         ,ps_event,ps_iface_v4_p,0);
    }
  }

  if (ps_iface_v6_p != NULL) 
  {
  /*------------------------------------------------------------------------- 
   Indicate to PS the Physlink pointer, Rx watermark count,
   Tx watermark count and dormancy status.
   Dormancy status is set to DORMANT
  -------------------------------------------------------------------------*/
  api_ret_val =  ps_wmk_logging_event_report_ex(&(bearer_context_p->phys_link),
                                  EVENT_DS_UM_QUEUE_STATS_EX,
                                  ps_event,
                                  bearer_context_p->rx_wm.wm_item.current_cnt,
                                  bearer_context_p->tx_wm.wm_item.current_cnt,
                                  ps_iface_v6_p
                                  );

    if (api_ret_val == 0)
    {
      DS_3GPP_MSG3_HIGH("Succcesful Logging for event %d IPV6 PS Iface 0x%x"
                        ,ps_event,ps_iface_v6_p,0);
    }
    else
    {
      DS_3GPP_MSG3_ERROR("Failure to log event %d PS Iface IPV6 0x%x "
                         ,ps_event,ps_iface_v6_p,0);
    }
  }
}

/*===========================================================================
FUNCTION DS_BEARER_CNTX_VALIDATE_BEARER_SUBSCRIPTION_INTERNAL

DESCRIPTION
  This function validates whether the subs id of the bearer matches the
  subs id that was passed in as parameter. No F3 if bearer context is invalid
  or does not belong to subs

PARAMETERS
  bearer_context_p : Bearer Context Ptr
  subs_id          : Subscription Id 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if Bearer Subscription Id is valid
  FALSE, otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_bearer_cntx_validate_bearer_subscription_internal
(
  ds_bearer_context_s     *bearer_context_p,
  sys_modem_as_id_e_type   subs_id
)
{
  sys_modem_as_id_e_type   bearer_subs_id = SYS_MODEM_AS_ID_NONE;
  boolean                  ret_val = TRUE;
  /*-----------------------------------------------------------------------*/

  if(!ds_bearer_cntxt_get_subs_id_from_bearer_internal(bearer_context_p, 
                                                       &bearer_subs_id))
  {
    ret_val =  FALSE;
  }
  else if (bearer_subs_id != subs_id)
  {
    ret_val = FALSE;
  }

  return ret_val;
} /*ds_bearer_cntxt_validate_bearer_subscription_internal*/

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_VALIDATE_BEARER_SUBSCRIPTION

DESCRIPTION
  This function validates whether the subs id of the bearer matches the
  subs id that was passed in as parameter.

PARAMETERS
  bearer_context_p : Bearer Context Ptr
  subs_id          : Subscription Id 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if Bearer Subscription Id is valid
  FALSE, otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_bearer_cntxt_validate_bearer_subscription
(
  ds_bearer_context_s     *bearer_context_p,
  sys_modem_as_id_e_type   subs_id
)
{
  sys_modem_as_id_e_type   bearer_subs_id = SYS_MODEM_AS_ID_NONE;
  boolean                  ret_val = TRUE;
  /*-----------------------------------------------------------------------*/

  if(!ds_bearer_cntxt_get_subs_id_from_bearer(bearer_context_p, 
                                             &bearer_subs_id))
  {
    ret_val =  FALSE;
  }
  else if (bearer_subs_id != subs_id)
  {
    ret_val = FALSE;
  }

  return ret_val;
} /*ds_bearer_cntxt_validate_bearer_subscription*/

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_GET_PER_SUBS_INFO_HNDL

DESCRIPTION
  This function fetches a handle to the Bearer Context Information
  corresponding to the given Subscription.

PARAMETERS
  subs_id          : Subscription Id 

DEPENDENCIES
  None.

RETURN VALUE
  Handle to the Bearer Context Information for the given Subscription.

SIDE EFFECTS
  None.
===========================================================================*/
ds_bearer_cntxt_per_subs_info_type* ds_bearer_cntxt_get_per_subs_info_hndl
(
  sys_modem_as_id_e_type subs_id
)
{
  ds_bearer_cntxt_per_subs_info_type *hndl = NULL;
  /*----------------------------------------------------------*/

  if (ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    hndl =  ds_bearer_cntxt_info.per_subs_info_p[subs_id];
  }

  return hndl;
} /* ds_bearer_cntxt_get_per_subs_info_hndl */

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_SET_PER_SUBS_INFO_HNDL

DESCRIPTION
  This function sets the handle to the Bearer Context Information
  corresponding to the given Subscription.

PARAMETERS
  subs_id          : Subscription Id
  hndl             : Handle to the Bearer Context Information for
                     the given Subscription.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_bearer_cntxt_set_per_subs_info_hndl
(
  sys_modem_as_id_e_type        subs_id,
  ds_bearer_cntxt_per_subs_info_type  *hndl
)
{
  /*----------------------------------------------------------*/
  if (ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    ds_bearer_cntxt_info.per_subs_info_p[subs_id] = hndl;
  }
} /* ds_bearer_cntxt_set_per_subs_info_hndl */


/*===========================================================================
FUNCTION DS_BEARER_CNTXT_VALIDATE_PER_SUBS_INFO_HNDL

DESCRIPTION
  This function validates the handle to the Subscription Specific
  Bearer Context Information. 
  
PARAMETERS
  hndl  : Handle to the Subscription specific
          Bearer Context Information

DEPENDENCIES
  None.

RETURN VALUE
  TRUE,  if the handle is valid
  FALSE, if invalid

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_bearer_cntxt_validate_per_subs_info_hndl
(
  ds_bearer_cntxt_per_subs_info_type *hndl
)
{
  boolean ret_val = FALSE;
  /*----------------------------------------------------------*/

  if (hndl != NULL && hndl->cookie == DS_BEARER_CNTXT_PER_SUBS_INFO_COOKIE)
  {
    ret_val = TRUE;
  }
  
  return ret_val;
} /* ds_bearer_cntxt_validate_per_subs_info_hndl */

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_GET_INSTANCE_PTR

DESCRIPTION
  This function fetches the Bearer Context Pointer corresponding to the
  given Bearer Instance.
 
PARAMETERS
  Bearer Instance

DEPENDENCIES
  None.

RETURN VALUE
  Bearer Context Pointer.

SIDE EFFECTS
  None.
===========================================================================*/
ds_bearer_context_s* ds_bearer_cntxt_get_instance_ptr
(
  uint8 instance
)
{
  return ((instance < DS_3GPP_MAX_BEARER_CONTEXT) ? 
            &(ds_bearer_cntxt_info.bearer_context_tbl[instance]): NULL);
} /* ds_bearer_cntxt_get_instance_ptr */

/*===========================================================================
FUNCTION ds_bearer_cntxt_get_mem_ctrl_ev_posted_flag

DESCRIPTION
  This function gets the Mem Ctrl Event posted Flag
 
PARAMETERS
  None
    
DEPENDENCIES
  None

RETURN VALUE
  TRUE - If Mem Cntrl Event was already posted
  FALSE- Otherwise

SIDE EFFECTS
  None

=============================================================================*/
static boolean ds_bearer_cntxt_get_mem_ctrl_ev_posted_flag
(
  void
)
{
  return ds_bearer_cntxt_info.mem_ctrl_ev_posted;
} /* ds_bearer_cntxt_get_mem_ctrl_ev_posted_flag */

/*===========================================================================
FUNCTION ds_bearer_cntxt_set_mem_ctrl_ev_posted_flag

DESCRIPTION
  This function sets the Mem Ctrl Event posted Flag
 
PARAMETERS
  flag: Mem Cntrl Event posted Flag
    
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
static void ds_bearer_cntxt_set_mem_ctrl_ev_posted_flag
(
  boolean flag
)
{
  ds_bearer_cntxt_info.mem_ctrl_ev_posted = flag;
} /* ds_bearer_cntxt_set_mem_ctrl_ev_posted_flag */

/*===========================================================================
FUNCTION ds_3gpp_bearer_cntxt_is_modem_fd_enabled

DESCRIPTION
  This function returns if modem fast dormancy is enabled in EFS or not

PARAMETERS
None
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE-- if YES
  FALSE- if NO

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_3gpp_bearer_cntxt_is_modem_fd_enabled()
{
  /* ----------------------------------------------------- */
  return (ds_3gpp_fd_info.is_FD_enabled);
}

/*===========================================================================
FUNCTION ds_3gpp_bearer_cntxt_set_modem_fd_enabled_flag

DESCRIPTION
  This function sets modem fast dormancy is enabled flag

PARAMETERS
boolean value to enable/disable
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE-- if YES
  FALSE- if NO

SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_bearer_cntxt_set_modem_fd_enabled_flag
(
  boolean enabled_flag
)
{
  /* ----------------------------------------------------- */
  ds_3gpp_fd_info.is_FD_enabled = enabled_flag;
}

/*===========================================================================
FUNCTION ds_3gpp_bearer_cntxt_set_modem_fd_timer_1_value

DESCRIPTION
  This function sets modem fast dormancy timer value

PARAMETERS
value 
    
DEPENDENCIES
  None.

RETURN VALUE
 None

SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_bearer_cntxt_set_modem_fd_timer_1_value
(
  uint32 timer_value
)
{
  ds_3gpp_fd_info.timer_1_value = timer_value;
}

/*===========================================================================
FUNCTION ds_3gpp_bearer_cntxt_set_modem_fd_timer_2_value

DESCRIPTION
  This function sets modem fast dormancy timer value

PARAMETERS
value 
    
DEPENDENCIES
  None.

RETURN VALUE
 None

SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_bearer_cntxt_set_modem_fd_timer_2_value
(
  uint32 timer_value
)
{
    ds_3gpp_fd_info.timer_2_value = timer_value;
}

/*===========================================================================
FUNCTION DS_3GPP_BEARER_CNTXT_OK_TO_RELEASE_CONNECTION

DESCRIPTION
  This function checks if it is OK to Relese signalling connection with NW 

PARAMETERS
None
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE-- if YES
  FALSE- if NO

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_3gpp_bearer_cntxt_ok_to_release_connection()
{
  ds_bearer_context_s*   bearer_cntxt_p = NULL;
  uint8                  index;
  uint8                  retVal = TRUE;
  dsm_watermark_type    *tx_wm = NULL;
  /* ----------------------------------------------------------------- */
  for(index = 0; index < DS_3GPP_MAX_BEARER_CONTEXT;index++)
  {
    bearer_cntxt_p = ds_bearer_cntxt_get_instance_ptr(index);

    if(!ds_bearer_cntx_validate_bearer_context_internal(bearer_cntxt_p))
    {
      continue;
    }
    tx_wm = &(bearer_cntxt_p->tx_wm.wm_item);

    /* Ccheck for bearer states and WN Counts for Pending data */
    if((bearer_cntxt_p->ds_bearer_context_dyn_p->state == 
                                       DS_BEARER_CONTEXT_STATE_COMING_UP)||
      (bearer_cntxt_p->ds_bearer_context_dyn_p->state == 
                                      DS_BEARER_CONTEXT_STATE_GOING_DOWN)||
      (dsm_queue_cnt(tx_wm) != 0) || 
      ((bearer_cntxt_p->ds_bearer_context_dyn_p->state == 
                                       DS_BEARER_CONTEXT_STATE_UP) && 
      (PS_PHYS_LINK_GET_STATE(&(bearer_cntxt_p->phys_link)) == PHYS_LINK_DOWN)))
    {
      DS_3GPP_MSG3_HIGH("Inst: %d, bearer_state: %d, WM_Cnt: %d",
                        index,
                        bearer_cntxt_p->ds_bearer_context_dyn_p->state,
                        dsm_queue_cnt(tx_wm));
      DS_3GPP_MSG1_HIGH("PCH_State: %d", TRUE);
      retVal = FALSE;
      break;
    }
  }
  return retVal; 
}/* ds_3gpp_bearer_cntxt_ok_to_release_connection*/

/*===========================================================================
FUNCTION DS_3GPP_BEARER_CNTXT_IS_ANY_BEARER_UP

DESCRIPTION
  This function checks if any bearer is in UP state or not

PARAMETERS
None
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE-- if YES
  FALSE- if NO

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_3gpp_bearer_cntxt_is_any_bearer_up()
{
  ds_bearer_context_s* bearer_cntxt_p = NULL;
  uint8                index;
  boolean              ret_val = FALSE; 
  /* ----------------------------------------------------------------------*/
   for(index = 0; index < DS_3GPP_MAX_BEARER_CONTEXT;index++)
  {
    bearer_cntxt_p = ds_bearer_cntxt_get_instance_ptr(index);
 
    if(ds_bearer_cntx_validate_bearer_context_internal(bearer_cntxt_p)&&
       (bearer_cntxt_p->ds_bearer_context_dyn_p->state == 
                                 DS_BEARER_CONTEXT_STATE_UP))
    {
      DS_3GPP_MSG1_MED(" Bearer Context is UP, Instance: %d",index);
      ret_val = TRUE;
      break;
    }
  }
  return ret_val;
}/* ds_3gpp_bearer_cntxt_is_any_bearer_up */

/*===========================================================================
FUNCTION DS_3GPP_BEARER_CNTXT_IS_ANY_PDN_CLEAN_UP_IN_PROGRESS

DESCRIPTION
  This function checks if any PDN clean up is in progress

PARAMETERS
None
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE-- if YES
  FALSE- if NO

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_3gpp_bearer_cntxt_is_any_pdn_clean_up_in_progress
(
  sys_modem_as_id_e_type subs_id
)
{
  boolean                  ret_val = FALSE;
  uint8                    bearer_index = 0;
  ds_bearer_context_s     *bearer_context_p= NULL;
  ds_pdn_context_s        *pdn_cntx_p = NULL;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for( bearer_index = 0;bearer_index < DS_3GPP_MAX_BEARER_CONTEXT;
       bearer_index++)
  {
    bearer_context_p = ds_bearer_cntxt_get_instance_ptr(bearer_index);

    if(!ds_bearer_cntx_validate_bearer_subscription_internal(bearer_context_p, subs_id))
    {
      continue;
    }

    DS_3GPP_MSG2_HIGH("Bearer index:%d is in state:%d",bearer_index,
                       bearer_context_p->ds_bearer_context_dyn_p->state);

    if(bearer_context_p->ds_bearer_context_dyn_p->state == 
       DS_BEARER_CONTEXT_STATE_DOWN)   
    {
       pdn_cntx_p = bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr;
       if (ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
       {
         if(pdn_cntx_p->ds_pdn_context_dyn_p->state != DS_PDN_CONTEXT_STATE_DOWN)
         {
           DS_3GPP_MSG1_HIGH("Pdn :%d is in state:%d",
             pdn_cntx_p->ds_pdn_context_dyn_p->state);
           ret_val = TRUE;  
           break;
         }
       }
    }
    if((bearer_context_p->ds_bearer_context_dyn_p->state == 
         DS_BEARER_CONTEXT_STATE_GOING_DOWN)||
       ((bearer_context_p->ds_bearer_context_dyn_p->state == 
           DS_BEARER_CONTEXT_STATE_PEND_REDIAL)&&
        (bearer_context_p->ds_bearer_context_dyn_p->cm_call_id != CM_CALL_ID_INVALID)))
    {
       DS_3GPP_MSG1_HIGH("default bearer:0x%p is in pending redial and call end not"
                         " yet received ", bearer_context_p);
       ret_val = TRUE;  
       break;
    }
  }

  return ret_val;
}



/*===========================================================================
FUNCTION DS_3GPP_BEARER_CNTXT_HANDLE_CONNECTION_RELEASE_REQUEST

DESCRIPTION
  This function handles connection release SYS IOCTL from PS and sends
  Signalling Connection Release Request to CM if applicable

PARAMETERS
  arg_val_ptr- carrys required information
  ps_errno- type of error
    
DEPENDENCIES
  None.

RETURN VALUE
 0- Success
 -1- Failure

SIDE EFFECTS
  None

===========================================================================*/
int ds_3gpp_bearer_cntxt_handle_connection_release_request
(
  void                      *arg_val_ptr,
  int16                     *ps_errno
)
{
  int       ret_val = -1;
  /* ------------------------------------------------------------------------ */
  /* check if any bearer is UP */
  if (ds_3gpp_bearer_cntxt_is_any_bearer_up() == FALSE)
  {
    DS_3GPP_MSG0_HIGH("No Bearers are UP, Ignoring Dormancy Request");
   *ps_errno = DS_EINVAL;
    return 0; 
  }
  /* 	check if any bearer is COMING_UP State or has UL data pending in WM */
  if(ds_3gpp_bearer_cntxt_ok_to_release_connection()== FALSE)
  {
    DS_3GPP_MSG0_HIGH("Not OK to Reease Conection, Ignoring");
    *ps_errno = DS_EINVAL;
    return ret_val; 
  }
 
  /* send CM command */
  if (!ds3g_msh_go_dormant())
  {
    *ps_errno =  DS_ETRYAGAIN;
    ret_val = -1; 
  } 
  else
  {
    DS_3GPP_MSG0_HIGH("Connection Release Request sent to CM Successfully");
   *ps_errno =  DS_ENOERR;
    ret_val = 0; 
  }
  return ret_val;
}/*ds_3gpp_bearer_cntxt_handle_connection_release_request */

#ifdef FEATURE_DATA_LTE
/*===========================================================================
FUNCTION DS_3GPP_BEARER_CNTXT_GET_EPS_ID_FROM_PHYSLINK

DESCRIPTION
  This function gets the LTE EPS beater ID of the bearer corresponding to the 
   physlink information passed

PARAMETERS
 eps_id_p - pointer for eps_id info
 phys_link_ptr - physlink pointer for whuch corresponding eps_id is required
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_3gpp_bearer_cntxt_get_eps_id_from_physlink
(
  uint8*             eps_id_p,
  ps_phys_link_type* phys_link_ptr
)
{
  uint8                 bearer_cntx_inst;
  ds_bearer_context_s   *bearer_context_p = NULL;
  ds_eps_bearer_context_s * eps_bearer_context = NULL;
  boolean                entry_found =  FALSE;
 // sys_modem_as_id_e_type subs_id = SYS_MODEM_AS_ID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 /* check if there is an entry for physlink ptr passed and get correcponding 
    bearer contect ptr  */
  for(bearer_cntx_inst = 0;
      bearer_cntx_inst < DS_3GPP_MAX_BEARER_CONTEXT;
      bearer_cntx_inst++) 
  {
    bearer_context_p = ds_bearer_cntxt_get_instance_ptr(bearer_cntx_inst);

    if (ds_bearer_cntx_validate_bearer_context_internal(bearer_context_p) &&
        (&(bearer_context_p->phys_link) == phys_link_ptr))
    {
      entry_found = TRUE;
      //ds_bearer_cntxt_get_subs_id_from_bearer(bearer_context_p,&subs_id );
      break;
    }
  }
  /* If physlink entry is found, check if curreent serving system is LTE or not.
        If LTE, then get the EPS_ID from eps bearer context ptr info.
        If not, ignore and return FALSE   */

  if (entry_found == TRUE && 
      (bearer_context_p->ds_bearer_context_dyn_p->call_mode == SYS_SYS_MODE_LTE)) /*TODO: call_mode or network mode which is better??? */
  {
    eps_bearer_context = (ds_eps_bearer_context_s *)
      DS_BEARER_CTXT_VOBJ(bearer_context_p, SYS_SYS_MODE_LTE);

    if(!ds_eps_bearer_cntxt_validate_bearer_cntxt(eps_bearer_context))
    {
      return FALSE;
    }

    *eps_id_p =  eps_bearer_context->ds_eps_bearer_context_dyn_p->eps_bearer_id;
    DS_3GPP_MSG1_HIGH("ds_3gpp_bearer_cntxt_get_eps_id_from_physlink(): eps_id: %d",
                                                                        *eps_id_p);
    return TRUE;
  }
  return FALSE;
}/* ds_3gpp_bearer_cntxt_get_eps_id_from_physlink */

/*===========================================================================
FUNCTION DS_3GPP_DPM_BEARER_ERR_CB

DESCRIPTION
  This is a callback function called from PS Task when malformed 
  number of packets on particular bearer reaches threshhold.

PARAMETERS
  phys_link_ptr - physical_link_ptr
  prev_resync_passed -  previous resync flag value passed
  
DEPENDENCIES
  None.

RETURN VALUE
  NoneE.
  
SIDE EFFECTS
  None.
  
===========================================================================*/
void ds_3gpp_bearer_cntxt_dpm_bearer_err_cb
(
  ps_phys_link_type* phys_link_ptr,
  boolean            prev_resync_passed
)
{ 
  ds_3gpp_bearer_err_info_type* bearer_err_info_p = NULL;
  uint8        eps_id = 0;
  /*-------------------------------------------------------- */
  DS_3GPP_MSG2_HIGH("ds_3gpp_bearer_cntxt_dpm_bearer_err_cb(): Physlink_p: 0x%x,"
                         "prev_sync_flag: %d", phys_link_ptr, prev_resync_passed);
  
  bearer_err_info_p = ds_3gpp_get_bearer_err_info_ptr();
  
  if ((bearer_err_info_p != NULL) && (bearer_err_info_p->pdpc_cb_fptr != NULL))
  {
    if( ds_3gpp_bearer_cntxt_get_eps_id_from_physlink(&eps_id, phys_link_ptr))
    {
     (void) bearer_err_info_p->pdpc_cb_fptr(eps_id, 
                                            prev_resync_passed);
    }
    else
    {
      DS_3GPP_MSG0_ERROR("ds_3gpp_bearer_cntxt_dpm_bearer_err_cb():couldn't get EPS_ID");
    }
  }
}/*  ds_3gpp_bearer_cntxt_dpm_bearer_err_cb */

#endif /* FEATURE_DATA_LTE */

#endif /* FEATURE_DATA_3GPP */
/*===========================================================================

                                UNIT TEST

===========================================================================*/
