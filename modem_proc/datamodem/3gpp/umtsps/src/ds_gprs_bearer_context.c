

/*!
  @file
  ds_gprs_bearer_context.c

  @brief
  REQUIRED brief one-sentence description of this C module.

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2010-2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/umtsps/src/ds_gprs_bearer_context.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/10/12   ss      Delegating RAT specific handling during call_end.
07/04/12   ss      Freeing the BP timer handle in deact_cnf and avoiding
                   crash due to using stale timer handle.
01/02/12   msh     Coan: Feature cleanup
12/21/11   ss      Added support for watermark tuning.
11/11/11   nd      Feature cleanup.
9/09/11    rr      Added support for fallback from Dual IP to single IP when nw rejects 
                   with specific cause codes in UMTS mode
06/03/11   ttv     Added missing NULL checks.
03/17/11   sa      Added support for bearer tech change event indication.
02/23/11   ttv     Added support for watermark tracing.
01/31/11   ss      Added support for reporting bearer tech type in WCDMA 
                   and GPRS.
12/01/10   ss      Support for SIM REFRESH VOTING for WCDMA and GPRS.

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_GSM_GPRS
#include "dsutil.h"

#define FEATURE_DSM_WM_CB

#include "ds_gprs_bearer_context.h"
#include "ds_3gpp_nv_manager.h"
#include "ds_3gpp_pdn_context.h"
#include "ds_3gpp_bearer_context.h"
#include "dstask_v.h"

#include <stringl/stringl.h>
#include "cm.h"
#include "sys.h"
#include "ds3gmgr.h"
#include "gsndcp.h"
#include "msg.h"
#include "ps_ppp.h"
//#include "ps_iface_ioctl.h"
#include "ps_iface_defs.h"
#include "ps_phys_link.h"
#include "ps_sys_ioctl.h"
#include "ds_flow_control.h"
#include "ds3gmshif.h"
#include "ds3gflowmgr.h"
#include "geran_qmi.h"
#include "comptask_v.h"

#ifdef TEST_FRAMEWORK
#error code not present
#endif

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/


/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================
FUNCTION DS_GPRS_BEARER_CNTXT_SETUP_PDP_PS_TO_SNDCP_WM

DESCRIPTION
  This function initializes the PS->SNDCP watermark for PDP calls.
  
PARAMETERS
  *bearer_context_p - This is the bearer to work with.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL void ds_gprs_bearer_cntxt_setup_pdp_ps_to_sndcp_wm
(
  ds_bearer_context_s*    bearer_context_p
);

/*===========================================================================
FUNCTION DS_GPRS_BEARER_CNTXT_SETUP_PDP_SNDCP_TO_PS_WM

DESCRIPTION
  This function initializes the SNDCP->PS watermark. 
  
PARAMETERS
  *bearer_context_p - This is the bearer to work with.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
  
===========================================================================*/
LOCAL void ds_gprs_bearer_cntxt_setup_pdp_sndcp_to_ps_wm
(
  ds_bearer_context_s*    bearer_context_p
);

/*===========================================================================

                                FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION DS_GPRS_BEARER_CNTXT_PDP_IP_TX_UM_DATA_CB

DESCRIPTION
  This function forwards pkts received to the lower layers for 
  pdp-ip call.
  This function is registered with ps_iface for a PDP-IP call 
  
PARAMETERS  
  *phys_link_ptr   - Interface ptr for the call.
  **item_ptr       - Payload
  *meta_info_ptr   - Not used.
  *pkt_instance    - Call instance.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
  
SIDE EFFECTS 
  None.

VRK:TODO Move this to 3gpp_bearer_context: This is not mode specific  
===========================================================================*/
LOCAL void ds_gprs_bearer_cntxt_pdp_ip_tx_um_data_cb
(
  ps_phys_link_type *phys_link_ptr,
  dsm_item_type     **item_ptr,
  ps_meta_info_type *meta_info_ptr,
  void              *tx_info_ptr
)
{
  ds_bearer_context_s     *bearer_context_p;
  ds_bearer_context_wm_s  *tx_watermark_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  bearer_context_p = (ds_bearer_context_s*)tx_info_ptr;

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }
  /*-------------------------------------------------------------------------
    Get PDCP state information for call.
  -------------------------------------------------------------------------*/
  tx_watermark_ptr = &(bearer_context_p->tx_wm);

  /*-------------------------------------------------------------------------
    Enqueue this item to PDCP
  -------------------------------------------------------------------------*/
#ifdef DS_3GPP_RX_TX_DBG_MSG
  DS_GPRS_MSG1_HIGH("ip tx um data cb. Bearer Instance:%d ",\
		  (uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr);
#endif /* DS_3GPP_RX_TX_DBG_MSG */

  dsm_enqueue(&tx_watermark_ptr->wm_item, item_ptr);


  PS_META_INFO_FREE(&meta_info_ptr);

} /* ds_gprs_bearer_cntxt_pdp_ip_tx_um_data_cb() */


/*===========================================================================
FUNCTION DS_GPRS_BEARER_CNTXT_PDP_IP_RX_UM_DATA_CB

DESCRIPTION
  This function forwards pkts received on Um interface to ps_iface for 
  pdp-ip call.
  
PARAMETERS   
  sig           - Rx signal for call 
  *user_data_p  - stores bearer context information
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
   TRUE - Success
   FALSE - Failure
   
SIDE EFFECTS 
  None.

VRK:TODO Move this to 3gpp_bearer_context: This is not mode specific  
===========================================================================*/
LOCAL boolean ds_gprs_bearer_cntxt_pdp_ip_rx_um_data_cb
(
  ps_sig_enum_type sig,
  void             *user_data_p
)
{
  ds_bearer_context_s    *bearer_context_p;  /* Pointer to bearer context   */
  dsm_item_type          *item_ptr;          /* Pointer to DSM item         */  
  ds_bearer_context_wm_s *rx_watermark_ptr;  
  boolean                result = TRUE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get the bearer context from the call back data
  -------------------------------------------------------------------------*/
  bearer_context_p = (ds_bearer_context_s*)user_data_p;

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return FALSE;
  }

#ifdef DS_3GPP_RX_TX_DBG_MSG
  DS_GPRS_MSG1_HIGH("ip rx um data cb. Bearer Instance:%d ",\
  	  (uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr);
#endif /* DS_3GPP_RX_TX_DBG_MSG */
  
  /*-------------------------------------------------------------------------
    RM_FLOW_ENABLED flag is set to TRUE always for Embedded calls.
    For Laptop calls, this flag is set based on the flow events happening on
    the Rm Phys link.
  -------------------------------------------------------------------------*/
  if(bearer_context_p->rm_flow_enabled == TRUE)
  {    
    /*-----------------------------------------------------------------------
      Get PDCP state information for the call.
    -----------------------------------------------------------------------*/
    rx_watermark_ptr = &(bearer_context_p->rx_wm);

    /*-----------------------------------------------------------------------
      The PDCP wm item has IP packets queued up. De-queue and send it on the
      correct physlink.
    -----------------------------------------------------------------------*/
    item_ptr = (dsm_item_type *)dsm_dequeue(&(rx_watermark_ptr->wm_item));
    
    if (item_ptr != NULL)
    {
#ifdef DS_3GPP_RX_TX_DBG_MSG
      DS_GPRS_MSG1_HIGH("ip rx um data cb. Got data, Bearer Instance:%d ",\
  	  (uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr);
#endif /* DS_3GPP_RX_TX_DBG_MSG */
      /*---------------------------------------------------------------------
        Data ready for PPP processing. Invoke function to process data
        stream.  
      ---------------------------------------------------------------------*/
      /*---------------------------------------------------------------------
        Call ps_phys_link_input with the dsm chain. This routes the packet to
        the correct iface (v4 or v6) and bridges it to the RM iface if need be 
      ---------------------------------------------------------------------*/
      (void)ps_phys_link_input(&(bearer_context_p->phys_link),&item_ptr,NULL);
      result = FALSE;
    }
    
  }
  return result;

  

} /* ds_gprs_bearer_cntxt_pdp_ip_tx_um_data_cb() */

/*===========================================================================
FUNCTION DS_GPRS_GET_COST_OF_MODEM_POWER

DESCRIPTION
  This returns the cost of modem power on the current subscription ID
  on GPRS RAT
 
PARAMETERS
  *argval_ptr       - Pointer to fill in the cost of modem Power
  *ps_errno         - PS Error code in case of error
 
DEPENDENCIES
  None.

RETURN VALUE
  0   - If the modem power has been determined successfully from Lower
        layers
  -1  - otherwise 

SIDE EFFECTS
  None.

===========================================================================*/
int16 ds_gprs_get_cost_of_modem_power
(
   void               *argval_ptr,
   int16              *ps_errno
)
{
  geran_cost_of_modem_power_t         geran_mli_mdm_pwr_cost;  
  int16                               ret_val                 = -1;
  ps_sys_modem_power_cost_enum_type  *cost_modem_pwr_p        = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - -  - - - - - - - - - - - - */

  if(ps_errno == NULL)
  {
    DS_3GPP_MSG0_HIGH("ps_errno is NULL");
    return ret_val;
  }

  if(argval_ptr == NULL)
  {
    DS_3GPP_MSG0_HIGH("Invalid args to get cost of mdm power");
    *ps_errno = DS_EFAULT;
    return ret_val;
  }
  cost_modem_pwr_p = (ps_sys_modem_power_cost_enum_type *)argval_ptr;

  /*------------------------------------------------------------------------- 
    Query WCDMA ML1 to get modem power cost
    -------------------------------------------------------------------------*/
  geran_mli_mdm_pwr_cost = geran_get_cost_of_modem_power();

  switch (geran_mli_mdm_pwr_cost)
  {
    case LOW_POWER:
      *cost_modem_pwr_p = DS_SYS_MODEM_POWER_COST_LOW;
       break;
    case MED_POWER:
      *cost_modem_pwr_p = DS_SYS_MODEM_POWER_COST_MEDIUM;
       break;
    case HIGH_POWER:
      *cost_modem_pwr_p = DS_SYS_MODEM_POWER_COST_HIGH;
       break;
    case NA_POWER:
    default:
      *cost_modem_pwr_p = DS_SYS_MODEM_POWER_COST_UNKNOWN;
      break;
  }
   DS_3GPP_MSG1_HIGH("GPRS reports cost of Modem power %d", *cost_modem_pwr_p);    
    *ps_errno = DS_ENOERR;
     ret_val = 0;
  
  return ret_val;
}
/*===========================================================================
FUNCTION DS_GPRS_BEARER_CNTXT_RX_DATA_AT_U_PDP_CB()

DESCRIPTION
  This function is called when there is data in the PDCP RX queue to signal PS
  that there is data for processing.
  This is called from the pdcp task context.

PARAMETERS
  pdcp_id: PDCP logical channel ID
  num_sdus: Number of SDUs queued into WM for each TTI
  callback_data : Callback data registered with PDCP: Call instance
  is registered with the PDCP

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void ds_gprs_bearer_cntxt_rx_data_at_u_pdp_cb
(
  byte pdcp_id,
  byte num_sdus,
  void *callback_data
)
{
  DS_3GPP_MSG3_HIGH ("CallBack function Noop %d %d %x",
                     pdcp_id, num_sdus, callback_data); 
  return;
} /* ds_gprs_bearer_cntxt_rx_data_at_u_pdp_cb() */


#ifdef FEATURE_DATA_WLAN_MAPCON
/*===========================================================================
FUNCTION DS_GPRS_BEARER_CNTXT_GET_SO_MASK_FOR_EGPRS_SUPPORT

DESCRIPTION
  This function returns the SO MASK corresponding to the passed EGPRS 
  support.

PARAMETERS
  subs_id - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  RAT mask 

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL uint64 ds_gprs_bearer_cntxt_get_so_mask_for_egprs_support
(
  sys_modem_as_id_e_type  subs_id
)
{
  uint64 so_mask = DS_SYS_SO_EX_UNSPECIFIED;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cm_cell_srv_ind_s_type cell_srv_ind = dsUmtsBearerCntxtGetCellSrvInd(subs_id);

  switch (cell_srv_ind.egprs_supp)
  {
    case SYS_EGPRS_SUPPORT_NOT_AVAIL:
      so_mask = DS_SYS_SO_EX_3GPP_GPRS;
      break;

    case SYS_EGPRS_SUPPORT_AVAIL:
      so_mask = DS_SYS_SO_EX_3GPP_EDGE;
      break;

    default:
      DS_GPRS_MSG1_HIGH("Egprs_support value [%d] unrecognized!",
                        cell_srv_ind.egprs_supp);
      break;
  }
  
  return so_mask;
} /* ds_gprs_bearer_cntxt_get_so_mask_for_egprs_support */
#endif /* FEATURE_DATA_WLAN_MAPCON */


/*===========================================================================
FUNCTION DS_GPRS_BEARER_CNTXT_PS_TO_SNDCP_FLOW_ENABLE_CB

DESCRIPTION
  This function is called when the SNDCP determines that uplink data flow
  from PS should be enabled.
    
PARAMETERS
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  
===========================================================================*/
void ds_gprs_bearer_cntxt_ps_to_sndcp_flow_enable_cb
(
  dsm_watermark_type *wm,
  void*              callback_data
)
{
/* VRK:TODO Add support*/
#if 0 
  ds_bearer_context_s*        bearer_context_p;
  ds_cmd_type                *cmd_ptr;           /* DS Task message        */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  bearer_context_p= (ds_bearer_context_s*) callback_data;
  if( (cmd_ptr = ds_get_cmd_buf()) == NULL )
  {
	DATA_ERR_FATAL("DS_GPRS cant get cmd buf from DS task");
	return;
  }

  /*-------------------------------------------------------------------------
	Enable flow on the phys link.    
  -------------------------------------------------------------------------*/
  cmd_ptr->hdr.cmd_id = DS3GPP_PHYS_LINK_FLOW_ENABLE_CMD;
  cmd_ptr->cmd.ds3gpp_phys_link_flow_cmd.bearer_context_p = bearer_context_p;
  cmd_ptr->cmd.ds3gpp_phys_link_flow_cmd.info_ptr = 
                                                  (void *)DS_FLOW_SNDCP_MASK;
  ds_put_cmd(cmd_ptr);
#endif /* VRK:TODO if 0 */

} /* ds_gprs_bearer_cntxt_ps_to_sndcp_flow_enable_cb() */

/*===========================================================================
FUNCTION DS_GPRS_BEARER_CNTXT_REGISTER_WM_WITH_SNDCP

DESCRIPTION
  This function registers the watermarks with SNDCP for uplink & downlink data
  
PARAMETERS 
   from_sndcp_pkt_wm_item_ptr  - Pointer to WM used between PS & SNDCP for
                    downlink data
   from_sndcp_post_rx_fn_ptr - callback Function installed in the watermark
                    for downlink data - called by SNDCP when new data is in
   to_sndcp_pkt_wm_item_ptr - Pointer to WM used between PS & SNDCP for 
                    uplink data    
   *bearer_context_p - This is the bearer to work with.                  
                   
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
    
SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void ds_gprs_bearer_cntxt_register_wm_with_sndcp
( 
  dsm_watermark_type          *from_sndcp_pkt_wm_item_ptr,
  void                        (*from_sndcp_post_rx_fn_ptr)(byte, byte, void *),
  dsm_watermark_type          *to_sndcp_pkt_wm_item_ptr ,
  ds_bearer_context_s*         bearer_context_p
)
{
  comp_cmd_type          *sndcp_cmd;
  sys_modem_as_id_e_type  subs_id = SYS_MODEM_AS_ID_NONE;
  /*----------------------------------------------------------------------*/

  
  if (!ds_bearer_cntxt_get_subs_id_from_bearer(bearer_context_p, &subs_id))
  {
    return;
  }

   /*-----------------------------------------------------------------------
     Register the UL watermarks with SNDCP 
   -----------------------------------------------------------------------*/
  if((sndcp_cmd = comp_get_cmd_buf()) == NULL)
  {
    /*-----------------------------------------------------------------------
      No free command buffers available, log an error.
    -----------------------------------------------------------------------*/
    DS_3GPP_MSG0_ERROR( "No cmd buffers available in COMP task free cmd q");
    return;
  }

  memset(sndcp_cmd, 0, sizeof(comp_cmd_type));

  sndcp_cmd->hdr.cmd_id = GSNDCP_REG_WM;
  sndcp_cmd->cmd.gsndcp_reg_wm.nsapi  = bearer_context_p->ds_bearer_context_dyn_p->nsapi;
  sndcp_cmd->cmd.gsndcp_reg_wm.to_sndcp_wm_ptr = to_sndcp_pkt_wm_item_ptr;
  sndcp_cmd->cmd.gsndcp_reg_wm.from_sndcp_wm_ptr =
                                               from_sndcp_pkt_wm_item_ptr;
  sndcp_cmd->cmd.gsndcp_reg_wm.from_sndcp_post_rx_func_ptr = 
                                                from_sndcp_post_rx_fn_ptr;
  sndcp_cmd->cmd.gsndcp_reg_wm.from_sndcp_rx_func_parm = 
                                                 (void*) bearer_context_p;
#ifdef FEATURE_DUAL_SIM
  sndcp_cmd->cmd.gsndcp_reg_wm.subs_id = subs_id;
#endif /* FEATURE_DUAL_SIM */
  comp_put_cmd(sndcp_cmd);

  return;
} /* ds_gprs_bearer_cntxt_register_wm_with_sndcp() */


/*===========================================================================
FUNCTION DS_GPRS_BEARER_CNTXT_DEREGISTER_WM_WITH_SNDCP

DESCRIPTION
  This function deregisters the watermarks with SNDCP for uplink &
  downlink data

PARAMETERS
  *bearer_context_p - This is the bearer to work with.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void ds_gprs_bearer_cntxt_deregister_wm_with_sndcp
(
  ds_bearer_context_s*    bearer_context_p
)
{
  comp_cmd_type           *sndcp_cmd;
  sys_modem_as_id_e_type  subs_id = SYS_MODEM_AS_ID_NONE;
  /*----------------------------------------------------------------------*/

  
  if (!ds_bearer_cntxt_get_subs_id_from_bearer(bearer_context_p, &subs_id))
  {
    return;
  }

  /*-------------------------------------------------------------------------
    Deregister the SNDCP UL & DL watermarks
  -------------------------------------------------------------------------*/
  if((sndcp_cmd = comp_get_cmd_buf()) == NULL)
  {
    /*-----------------------------------------------------------------------
      No free command buffers available, log an error.
    -----------------------------------------------------------------------*/
    DS_3GPP_MSG0_ERROR( "No cmd buffers available in COMP task free cmd q");
    return;
  }

  memset(sndcp_cmd, 0, sizeof(comp_cmd_type));

  sndcp_cmd->hdr.cmd_id = GSNDCP_DEREG_WM;
  sndcp_cmd->cmd.gsndcp_dereg_wm.nsapi = bearer_context_p->
    ds_bearer_context_dyn_p->nsapi;
#ifdef FEATURE_DUAL_SIM
  sndcp_cmd->cmd.gsndcp_dereg_wm.subs_id = subs_id;
#endif /* FEATURE_DUAL_SIM */

  comp_put_cmd(sndcp_cmd);

} /* ds_gprs_bearer_cntxt_deregister_wm_with_sndcp() */

/*===========================================================================
FUNCTION DS_GPRS_BEARER_CNTXT_SETUP_PDP_PS_TO_SNDCP_WM

DESCRIPTION
  This function initializes the PS->SNDCP watermark for PDP calls.
  
PARAMETERS
  *bearer_context_p - This is the bearer to work with.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL void ds_gprs_bearer_cntxt_setup_pdp_ps_to_sndcp_wm
(
  ds_bearer_context_s*    bearer_context_p
)
{
  ds_bearer_context_wm_s*      watermark_p;
  ds_pdn_context_s                     *pdn_cntx_p = NULL;
  sys_modem_as_id_e_type  subs_id = SYS_MODEM_AS_ID_NONE;
  ds_bearer_cntxt_um_wm_threshold_struct_type  um_wm_threshold;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }

  pdn_cntx_p = bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr;
  if (!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return;
  }

  pdn_cntx_p = (ds_pdn_context_s*)
               bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr;

  if(!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p,&subs_id))
  {
    return;
  }

  um_wm_threshold = ds_3gpp_nv_get_um_wm_threshold(subs_id);

  watermark_p = &bearer_context_p->tx_wm;  
  
  /*-------------------------------------------------------------------------
    Set up PS->SNDCP watermark 
  -------------------------------------------------------------------------*/
  dsm_set_low_wm(&(watermark_p->wm_item), 
                 um_wm_threshold.gprs_ul_lo);
  dsm_set_hi_wm(&(watermark_p->wm_item),
                um_wm_threshold.gprs_ul_hi);
  /*-------------------------------------------------------------------------
    Set the threshold for PS->SNDCP Q. 
    SNDCP to PS can hold a max of 3 IPpkt 
  -------------------------------------------------------------------------*/
  dsm_set_dne(&(watermark_p->wm_item),
              um_wm_threshold.gprs_ul_dne);
  dsm_set_dne_q_cnt(&(watermark_p->wm_item),DSGPSD_UL_WM_DEFAULT_DNE_QCNT);
  

   /*-----------------------------------------------------------------------
     Pass the call instance in watermarks registered so that we
     can identify the call when the callback is called
   -----------------------------------------------------------------------*/
  //VRK:TODO Need to use ds_gprs_bearer_cntxt_ps_to_sndcp_flow_disable_cb
  // and 

  /* VRK:TODO where is this ds_gprs_bearer_cntxt_ps_tx_flow_enable_cb */
  watermark_p->wm_item.lowater_func_ptr      = 
                             ds_bearer_cntxt_ps_to_L2_flow_enable_cb;
  watermark_p->wm_item.lowater_func_data     = 
        (void*)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr;

  /* VRK:TODO where is this ds_gprs_bearer_cntxt_ps_tx_flow_disable_cb */
  watermark_p->wm_item.hiwater_func_ptr      = 
                             ds_bearer_cntxt_ps_to_L2_flow_disable_cb;
  watermark_p->wm_item.hiwater_func_data     = 
        (void*)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr;

  watermark_p->wm_item.gone_empty_func_ptr   = NULL;

  watermark_p->wm_item.non_empty_func_ptr    = NULL;

  watermark_p->wm_item.each_enqueue_func_ptr = NULL;

  
} /* ds_gprs_bearer_cntxt_setup_pdp_ps_to_sndcp_wm() */


/*===========================================================================
FUNCTION DS_GPRS_BEARER_CNTXT_SETUP_PDP_SNDCP_TO_PS_WM

DESCRIPTION
  This function initializes the SNDCP->PS watermark. 
  
PARAMETERS
  *bearer_context_p - This is the bearer to work with.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
  
===========================================================================*/
LOCAL void ds_gprs_bearer_cntxt_setup_pdp_sndcp_to_ps_wm
(
  ds_bearer_context_s*    bearer_context_p
)
{
  ds_bearer_context_wm_s*      watermark_p;
  ds_pdn_context_s                     *pdn_cntx_p = NULL;
  sys_modem_as_id_e_type  subs_id = SYS_MODEM_AS_ID_NONE;
  ds_bearer_cntxt_um_wm_threshold_struct_type  um_wm_threshold;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((bearer_context_p == NULL) ||
    (!ds_bearer_cntx_validate_bearer_context(bearer_context_p)))
  {
    return;
  }

  pdn_cntx_p = bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr;
  if (!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return;
  }
  
  if(!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p, &subs_id))
  {
    return;
  }

  um_wm_threshold = ds_3gpp_nv_get_um_wm_threshold(subs_id); 

  watermark_p = &bearer_context_p->rx_wm;

  dsm_set_low_wm(&(watermark_p->wm_item), 
                 um_wm_threshold.gprs_dl_lo);
  dsm_set_hi_wm(&(watermark_p->wm_item),
                um_wm_threshold.gprs_dl_hi);
  dsm_set_dne(&(watermark_p->wm_item), 
              um_wm_threshold.gprs_dl_dne);
  dsm_set_dne_q_cnt(&(watermark_p->wm_item),DSGPSD_DL_WM_DEFAULT_DNE_QCNT);
  
  /*-----------------------------------------------------------------------
    SNDCP will implement the WM based CBs for flow control.
  -----------------------------------------------------------------------*/

  //watermark_p->wm_item.gone_empty_func_ptr  = NULL;

  //watermark_p->wm_item.lowater_func_ptr     = NULL;

  //watermark_p->wm_item.hiwater_func_ptr     = NULL;

  //watermark_p->wm_item.non_empty_func_ptr   = NULL;

} /* ds_gprs_bearer_cntxt_setup_pdp_sndcp_to_ps_wm() */

/*===========================================================================
FUNCTION      DS_GPRS_BEARER_CNTXT_CLEANUP_SNDCP_WATERMARKS

DESCRIPTION   
  This function cleans up the watermarks that are owned by data services.

PARAMETERS   
  *bearer_context_p - This is the bearer to work with.
  
DEPENDENCIES  
  None.

RETURN VALUE  
  None.

SIDE EFFECTS  
  None.
===========================================================================*/
void ds_gprs_bearer_cntxt_cleanup_sndcp_watermarks
(
  ds_bearer_context_s* bearer_context_p
)
{

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }

  dsm_empty_queue ( &(bearer_context_p->tx_wm.wm_item) );
  dsm_empty_queue ( &(bearer_context_p->rx_wm.wm_item) );

#if 0
  /*-----------------------------------------------------------------------
    Set the total_rcvd_cnt in the PS->PDCP and PDCP->PS watermark to 0.
  -----------------------------------------------------------------------*/
  device_state_ptr->ps_tx_wm_item.total_rcvd_cnt       = 0;
  device_state_ptr->ps_rx_wm_item.total_rcvd_cnt       = 0;

  /*-----------------------------------------------------------------------
    Set the amount of data dropped due to PDCP rab re-estab process failing
  -----------------------------------------------------------------------*/
  device_state_ptr->ps_total_tx_data_flushed          = 0;

#endif

} /* ds_gprs_bearer_cntxt_cleanup_sndcp_watermarks() */


/*===========================================================================
FUNCTION DS_GPRS_BEARER_CNTXT_DEACTIVATE_CNF()

DESCRIPTION
  This is the function that is registered for GPRS in the umts_bearer context
  function table. The registration happens in gprs_init.
  This function is called when the call_ended_hdlr is invoked from DSMGR.
  This function is responsible for cleanin up the lower layers.
      
PARAMETERS
  *bearer_context_p - This is the bearer to be released

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
  
===========================================================================*/
LOCAL void ds_gprs_bearer_cntxt_deactivate_cnf
(
  ds_bearer_context_s* bearer_context_p
)
{
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 

  /*-------------------------------------------------------------------------
    Deregister UL and DL watermarks with PDCP
  -------------------------------------------------------------------------*/
  ds_gprs_bearer_cntxt_deregister_wm_with_sndcp(bearer_context_p);

  if(bearer_context_p->ds_bearer_context_dyn_p->is_registered_with_dpm)
  {
    ds_bearer_cntx_deregister_dpm(bearer_context_p); 
  }

  bearer_context_p->ds_bearer_context_dyn_p->is_registered_with_dpm = FALSE;
  /*-------------------------------------------------------------------------
    Cleanup UL and DL PDCP watermarks.
  -------------------------------------------------------------------------*/
  ds_gprs_bearer_cntxt_cleanup_sndcp_watermarks(bearer_context_p);

  /*-------------------------------------------------------------------------
    Free the PS timer allocated for dynamic BP disable. This is required here 
    as a call which might be originated on W could have an inter-RAT, 
    transferred to G and ended in G.
  -------------------------------------------------------------------------*/
  if (PS_TIMER_SUCCESS != 
      ps_timer_free(bearer_context_p->ds_bearer_context_dyn_p->
                    tmr_hndl_dyn_back_pressure))
  {
    DS_GPRS_MSG1_ERROR("Failed to deallocate timer for bearer inst: %d",
                        (uint32)(bearer_context_p->ds_bearer_context_dyn_p->
                                 client_data_ptr));
  }

  /*-------------------------------------------------------------------------
    Clear the timer handle as stale handle could cause issues during inter-RAT
  --------------------------------------------------------------------------*/
  bearer_context_p->ds_bearer_context_dyn_p->tmr_hndl_dyn_back_pressure = 
    PS_TIMER_INVALID_HANDLE;

} /* ds_gprs_bearer_cntxt_deactivate_cnf() */


/*===========================================================================
FUNCTION DS_GPRS_BEARER_CNTXT_TRANSFER_SETUP_HDLR

DESCRIPTION

  This function handles all the GPRS specific operations during
  IRAT to GPRS. 
  
PARAMETERS
  None  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None 

===========================================================================*/
void ds_gprs_bearer_cntxt_transfer_setup_hdlr
(
  ds_bearer_context_s  *bearer_context_p
)
{

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }  

  /*------------------------------------------------------------------------------ 
    If the Bearer on the source RAT was in UP state, maintain the same Bearer
    state until we receive resume Indication on GPRS. This is to cover a possible
    case, where the device before resuming moves back to the source RAT
    ------------------------------------------------------------------------------*/
  if(bearer_context_p->ds_bearer_context_dyn_p->state == DS_BEARER_CONTEXT_STATE_UP)
  {
    bearer_context_p->ds_bearer_context_dyn_p->is_ho2GPRS_in_progress = TRUE;
  }
  return;
}/*ds_gprs_bearer_cntxt_transfer_setup_hdlr()*/

/*===========================================================================
FUNCTION DS_GPRS_BEARER_CNTXT_TRANSFER_RELEASE_HDLR

DESCRIPTION

  If already registered,this function deregisters the watermark with SNDCP
  upon context transfer Ind when moving from GPRS to any RAT.
  It also removes the first SDU from the SNDCP Uplink Watermark since this
  may be an incomplete SDU and free it.
  
  
PARAMETERS
  None  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None 

===========================================================================*/
void ds_gprs_bearer_cntxt_transfer_release_hdlr
(
  ds_bearer_context_s  *bearer_context_p
)
{
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }  

  /*------------------------------------------------------------------------ 
    Nothing to do if we receive a context transfer Indication, to release
    connection on GPRS before even having setup. This can happen when 
    the device momentarily moves to GPRS but no connections have fully
    yet resume'd before moving out from GPRS.
    ------------------------------------------------------------------------*/
  if(bearer_context_p->ds_bearer_context_dyn_p->is_ho2GPRS_in_progress == FALSE)
  {
    DS_GPRS_MSG3_HIGH("Release connection to GPRS Radio Layer Inst:0x%x ",
             bearer_context_p,0,0);

    /*-------------------------------------------------------------------------
      Deregister UL and DL watermarks with SNDCP
    -------------------------------------------------------------------------*/
    ds_gprs_bearer_cntxt_deregister_wm_with_sndcp(bearer_context_p);

    DS_GPRS_MSG3_HIGH("Changing bearer state to DORMANT Inst:0x%x ",
             bearer_context_p,0,0);
    bearer_context_p->ds_bearer_context_dyn_p->state =
       DS_BEARER_CONTEXT_STATE_UP_DORMANT;
    DS_GPRS_MSG3_HIGH("Setting each_enqueue_func_ptr in gprs_suspend_hdlr",
                      0,0,0);
    (bearer_context_p->tx_wm.wm_item).each_enqueue_func_ptr =
                 (wm_cb_type)ds_bearer_cntxt_reorig_dorm_call_cb;
    (bearer_context_p->tx_wm.wm_item).each_enqueue_func_data =
       (void*) bearer_context_p;

    /*-------------------------------------------------------------------
      Send Phy link down ind 
    -------------------------------------------------------------------*/
    ps_phys_link_down_ind_ex(
                     &(bearer_context_p->phys_link),
                     PS_EIC_QOS_NETWORK_L2_LINK_RELEASED);
  }
  else
  {
    DS_GPRS_MSG2_HIGH("Rel cntxt X'er for Bearer 0x%x on mode %d",
                      bearer_context_p,
                      bearer_context_p->ds_bearer_context_dyn_p->call_mode);
    /*----------------------------------------------------------------------- 
      Reset the Hand over to GPRS flag, as the UE is moving out of GPRS now
      -----------------------------------------------------------------------*/
    bearer_context_p->ds_bearer_context_dyn_p->is_ho2GPRS_in_progress = FALSE;
  }

  return;

} /* ds_gprs_bearer_cntxt_transfer_release_hdlr() */

/*===========================================================================
FUNCTION DS_GPRS_BEARER_CNTXT_SUSPEND_IND_HDLR

DESCRIPTION

  This function deregisters the watermark with SNDCP. It also removes the first 
  SDU from the SNDCP Uplink Watermark since this may be an 
  incomplete SDU and free it.
  
  
PARAMETERS
   allowed_to_send_data_avail - we are not allowed to send data_available_ind
  if we are OOS or in limited service.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None 

===========================================================================*/
void ds_gprs_bearer_cntxt_suspend_ind_hdlr
(
  ds_bearer_context_s  *bearer_context_p
)
{
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }  

  /*------------------------------------------------------------------------ 
    Nothing to do if we receive a suspend Indication before having 
    resume'd connection on GPRS. This might happen when suspend Ind is Rx'ed
    during IRAT to GPRS, when there is active data call on the source RAT &
    context transfer set up to GPRS has happened, but the connection on GPRS
    has not resume'd fully yet. 
    ------------------------------------------------------------------------*/
  if(bearer_context_p->ds_bearer_context_dyn_p->is_ho2GPRS_in_progress == FALSE)
  {
    DS_GPRS_MSG1_HIGH("Suspend connection to GPRS Radio Layer Inst:0x%x ",
                       bearer_context_p);

  /*-------------------------------------------------------------------------
    Deregister UL and DL watermarks with SNDCP
  -------------------------------------------------------------------------*/
  ds_gprs_bearer_cntxt_deregister_wm_with_sndcp(bearer_context_p);

  DS_GPRS_MSG1_HIGH("Changing bearer state to DORMANT Inst:0x%x ",
           bearer_context_p);
  bearer_context_p->ds_bearer_context_dyn_p->state =
     DS_BEARER_CONTEXT_STATE_UP_DORMANT;
  DS_GPRS_MSG0_HIGH("Setting each_enqueue_func_ptr in gprs_suspend_hdlr");
  (bearer_context_p->tx_wm.wm_item).each_enqueue_func_ptr =
               (wm_cb_type)ds_bearer_cntxt_reorig_dorm_call_cb;
  (bearer_context_p->tx_wm.wm_item).each_enqueue_func_data =
     (void*) bearer_context_p;

  /*-------------------------------------------------------------------
    Send Phy link down ind 
  -------------------------------------------------------------------*/
  ps_phys_link_down_ind_ex(
                   &(bearer_context_p->phys_link),
                   PS_EIC_QOS_NETWORK_L2_LINK_RELEASED);
  }
  else
  {
    DS_GPRS_MSG1_HIGH("Bearer 0x%x Rx'ed suspend Ind before resuming connection",
                      bearer_context_p);
  }

  return;

} /* ds_gprs_bearer_cntxt_suspend_ind_hdlr() */

/*===========================================================================
FUNCTION DS_GPRS_BEARER_CNTXT_MODE_PREF_CHANGE_HDLR

DESCRIPTION

  This function deregisters the watermark with SNDCP. It also removes the first 
  SDU from the SNDCP Uplink Watermark since this may be an 
  incomplete SDU and free it.
  
  
PARAMETERS
   allowed_to_send_data_avail - we are not allowed to send data_available_ind
  if we are OOS or in limited service.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None 

===========================================================================*/
void ds_gprs_bearer_cntxt_mode_pref_change_hdlr
(
  ds_bearer_context_s  *bearer_context_p
)
{
  ds_gprs_bearer_cntxt_suspend_ind_hdlr(bearer_context_p);
} /* ds_gprs_bearer_cntxt_mode_pref_change_hdlr */

/*===========================================================================
FUNCTION DS_GPRS_BEARER_CNTXT_PDP_IP_RESUME_REQ_REG_CNF

DESCRIPTION
  This function posts a command to set up the transmit function once 
  registration with SNDCP is complete.

PARAMETERS
  Context - PDP context instance
  cnf_status - Indicates whether registration was successful or not.
  Setup the transmit function only if registration was successful. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds_gprs_bearer_cntxt_pdp_ip_resume_req_reg_cnf
(
  void          *context,
  boolean       cnf_status
)
{
  ds_cmd_type                    *cmd_ptr;                 /* DS Task message */
  ds_bearer_context_s            *bearer_context_p = (ds_bearer_context_s*)context;
  ds_gprs_gsndcp_reg_cnf_ev_type *reg_cnf_ev_ptr = NULL;

  DS_GPRS_MSG0_HIGH("DS3GPP GSNDCP RESUME REG CNF EV: ");

  /*------------------------------------------------------------------------
    Post command to DS task to process in the proper task context
   ------------------------------------------------------------------------*/

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_gprs_gsndcp_reg_cnf_ev_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_GSNDCP_RESUME_CNF;
  reg_cnf_ev_ptr = (ds_gprs_gsndcp_reg_cnf_ev_type*)cmd_ptr->cmd_payload_ptr;

  reg_cnf_ev_ptr->bearer_context_p = bearer_context_p;
  reg_cnf_ev_ptr->cnf_status = cnf_status;

  ds_put_cmd(cmd_ptr);
} /* ds_gprs_bearer_cntxt_pdp_ip_resume_req_reg_cnf */

/*===========================================================================
FUNCTION DS_GPRS_BEARER_CNTXT_PDP_IP_RESUME_REQ_REG_CNF_HDLR

DESCRIPTION
  This function sets up the transmit function once registration with 
  SNDCP is complete.

PARAMETERS
  Context - PDP context instance
  cnf_status - Indicates whether registration was successful or not.
  Setup the transmit function only if registration was successful. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds_gprs_bearer_cntxt_pdp_ip_resume_req_reg_cnf_hdlr
(
  ds_bearer_context_s            *bearer_context_p,
  boolean                         cnf_status
)
{
  ps_phys_link_type                      *phys_link_p = NULL;
                                               /*Phys link ptr of the context*/
  int                                     ret_val;
  int16                                   ps_errno;
  dsm_item_type                          *item_ptr;        
                                               /* DSM item ptr */
  ds_bearer_context_wm_s                 *tx_watermark_p;
  ds_bearer_context_wm_s                 *rx_watermark_p;
  ps_phys_link_link_protocol_handle_type  protocol_handle;
  phys_link_state_type                    phys_link_state = PHYS_LINK_NULL;
  sys_modem_as_id_e_type                  subs_id = SYS_MODEM_AS_ID_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds_bearer_cntxt_get_subs_id_from_bearer(bearer_context_p, &subs_id))
  {
    return;
  }

  if((ds3gpp_get_ps_data_suspend_flag(subs_id) == TRUE) ||
     ((bearer_context_p->ds_bearer_context_dyn_p->state != 
       DS_BEARER_CONTEXT_STATE_UP)&&
     (bearer_context_p->ds_bearer_context_dyn_p->state != 
     DS_BEARER_CONTEXT_STATE_UP_DORMANT)&&
     (bearer_context_p->ds_bearer_context_dyn_p->state != 
      DS_BEARER_CONTEXT_STATE_UP_DORMANT_REESTAB)))
  {
    DS_GPRS_MSG0_HIGH(" Resume Reg req called in wrong state");
    return;
  }

  if ( cnf_status == TRUE )
  {
    /*-----------------------------------------------------------------------  
      Set the Tx function.
      VRK:TODO if tx function is the same for gwl then don't need this
    -----------------------------------------------------------------------*/
    

    ps_phys_link_set_tx_function(&(bearer_context_p->phys_link),\
                                 ds_gprs_bearer_cntxt_pdp_ip_tx_um_data_cb,\
                                 bearer_context_p);

    /*---------------------------------------------------------------------------
     * Set the link layer protocol to be NONE and fill the protocol handle 
     * correctly so that when a packet is input into this phys link, it is routed
     * to the approp iface. 
    ---------------------------------------------------------------------------*/

    /*---------------------------------------------------------------------------
     * We are filling the protocol handle structure with the iface pointers
    ---------------------------------------------------------------------------*/    
    memset(&protocol_handle, 0x0, sizeof(ps_phys_link_link_protocol_handle_type));

    if (((ds_pdn_context_s*)(bearer_context_p->ds_bearer_context_dyn_p->
                             pdn_context_ptr) == NULL) || 
        (!ds_3gpp_pdn_cntx_validate_pdn_context((ds_pdn_context_s*)
                                           (bearer_context_p->ds_bearer_context_dyn_p
                                            ->pdn_context_ptr))))
    {
      return;
    }

    if(((ds_pdn_context_s*)(bearer_context_p->ds_bearer_context_dyn_p->
                            pdn_context_ptr))->ds_pdn_context_dyn_p
		->ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV4_IFACE] != NULL)
    {
      protocol_handle.none_handle.handle.ip_proto_handle.v4_iface_ptr = 
	      &(((ds_pdn_context_s*)(bearer_context_p->ds_bearer_context_dyn_p->
                               pdn_context_ptr))->
          ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV4_IFACE]
          ->ps_iface);
    }

    if(((ds_pdn_context_s*)(bearer_context_p->ds_bearer_context_dyn_p
                            ->pdn_context_ptr))->ds_pdn_context_dyn_p
		->ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV6_IFACE] != NULL)
    {
      protocol_handle.none_handle.handle.ip_proto_handle.v6_iface_ptr = \
     &(((ds_pdn_context_s*)(bearer_context_p->
                            ds_bearer_context_dyn_p->pdn_context_ptr))->\
       ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV6_IFACE]
       ->ps_iface);
    }

    protocol_handle.none_handle.high_protocol = 
  	                              PS_PHYS_LINK_HIGHER_LAYER_PROTOCOL_IP;

    /*---------------------------------------------------------------------------
      Set the link layer protocol to NONE and pass the protocol handle into the
      function that will do this. The protocol handle has just been populated
      with the higher layer protocol and the pointers to the ifaces that might
      communicate over this physlink.
    ---------------------------------------------------------------------------*/
    (void) ps_phys_link_set_link_protocol(&(bearer_context_p->
                                            phys_link),
                                          PS_PHYS_LINK_LINK_PROTOCOL_NONE,
                                          protocol_handle);


    /*-------------------------------------------------------------------------
      Set the handler for the downlink signal for the call.
      VRK:TODO if sig hdlr is the same for gwl then don't need this
    -------------------------------------------------------------------------*/
    (void)ps_set_sig_handler(bearer_context_p->rx_sig,
                  ds_gprs_bearer_cntxt_pdp_ip_rx_um_data_cb, 
                  (void *)bearer_context_p);

    /*-------------------------------------------------------------------------  
      Enable the rx signal.
    -------------------------------------------------------------------------*/
    ps_enable_sig(bearer_context_p->rx_sig);    

    /*-------------------------------------------------------------------------  
      Set the PS->SNDCP watermark.
    -------------------------------------------------------------------------*/
    tx_watermark_p = &(bearer_context_p->tx_wm);
    ds_gprs_bearer_cntxt_setup_pdp_ps_to_sndcp_wm(bearer_context_p);    

    /*-------------------------------------------------------------------------  
      Set the SNDCP->PS watermark.
    -------------------------------------------------------------------------*/
    rx_watermark_p = &(bearer_context_p->rx_wm);
    ds_gprs_bearer_cntxt_setup_pdp_sndcp_to_ps_wm(bearer_context_p);    

    /*-----------------------------------------------------------------------
     Register WM with SNDCP 
    -----------------------------------------------------------------------*/
    ds_gprs_bearer_cntxt_register_wm_with_sndcp(
                &rx_watermark_p->wm_item,
                ds_gprs_bearer_cntxt_rx_data_at_u_pdp_cb,  
                &tx_watermark_p->wm_item,
                bearer_context_p);
    phys_link_state = PS_PHYS_LINK_GET_STATE(&(bearer_context_p->phys_link));
    /*---------------------------------------------------------------------  
      Check phys link state and post phys link up. After getting resume, we 
      cannot blindly post flow_enable() as it will not take effect if phys
      link is in transition states.
    ---------------------------------------------------------------------*/  
    if (!(PHYS_LINK_UP == phys_link_state))
    {
      ps_phys_link_up_ind( &(bearer_context_p->phys_link) );
    }

    ps_phys_link_enable_flow(&(bearer_context_p->phys_link),
                               DS_FLOW_UMTS_MASK);

    /*---------------------------------------------------------------------
      Send PS phys link disable for the bearer if subs is coex flow
      disabled
    ---------------------------------------------------------------------*/
    if( !ds3gflowmgr_get_coex_data_flow_status(subs_id) )
    {
      ps_phys_link_disable_flow(&(bearer_context_p->phys_link),
                                DS_FLOW_COEX_MASK);
    }
  }
  else
  {

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_DATA_LTE)   

    /*-----------------------------------------------------------------------
     If there are any downlink SDUs in the PDCP watermark, free them.
    -----------------------------------------------------------------------*/
    while((item_ptr =
             (dsm_item_type*)dsm_dequeue(&(bearer_context_p->
                                           rx_wm.wm_item)))
             !=  NULL)
    {
      dsm_free_packet(&item_ptr);
    }
#endif /* defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_DATA_LTE) */

    /*---------------------------------------------------------------------
      If we were Dormant before Inter-RAT, phys link will be in down state.
      ps_phys_link_down_cmd will be no-op if phys link is down. 
      ps_phys_link_down_cmd will return NULL in ret_val and ps_eerno will 
      not be set to DS_EWOULDBLOCK for this case. 
      Since registration with SNDCP failed, to end the call, put phys link 
      in up state and then call ps_phys_link_down_cmd. This sequence will 
      clean up the call.
    -----------------------------------------------------------------------*/    
    phys_link_p = &(bearer_context_p->phys_link);
    if(PS_PHYS_LINK_GET_STATE(phys_link_p) == PHYS_LINK_DOWN)
    {
      ps_phys_link_up_ind(phys_link_p);
    }   

    ret_val = ps_phys_link_down_cmd(phys_link_p,
                                    &ps_errno,
                                    NULL);

    if ((ret_val == 0) || (ps_errno != DS_EWOULDBLOCK))
    {
      if (bearer_context_p->ds_bearer_context_dyn_p->is_default)
      {
        /*-------------------------------------------------------------------
          Error in setting up default bearer -> Make sure that we call flow 
          go null ind on default flows of all ifaces associated with the PDN 
          context          
        --------------------------------------------------------------------*/
        ds_pdn_cntxt_unbind_default_flows(bearer_context_p->
                                          ds_bearer_context_dyn_p->pdn_context_ptr);        
      }

      /*-------------------------------------------------------------------
        Set the Bearer context to be in down state and call the bearer down
	    call back to the PDN context.
      --------------------------------------------------------------------*/

      /* Is it safe to free the bearer inst ?*/
      ds_bearer_cntxt_free_bearer_inst(bearer_context_p);
    
      /*---------------------------------------------------------------------
        Each enqueue func_ptr needs to be reset when bearer inst is freed.
        This needs to be done to avoid race conditions when phys_link_gone is
        posted and flow gets enabled on the phys_link leading to each_enq_f
        called when the bearer is getting freed.
      ---------------------------------------------------------------------*/
      ps_phys_link_enable_flow(&(bearer_context_p->phys_link),
                               DS_FLOW_UMTS_MASK);
      ps_phys_link_gone_ind(&(bearer_context_p->phys_link));

      /*-----------------------------------------------------------------------
        We have already cleaned up at the bearer level. We should send
        bearer down indication to PDN context with the boolean call_was_conn
        set to TRUE.
        This way the PDN context will be freed up as well.
      -----------------------------------------------------------------------*/
      if (bearer_context_p->ds_bearer_context_dyn_p->
          pdn_context_cb_tbl_ptr->bearer_down_ind_cb != NULL)
      {
        ds_3gpp_down_reason_t          net_down_reason;
        net_down_reason.reason.ps_iface_down_reason = PS_NET_DOWN_REASON_NOT_SPECIFIED;
        net_down_reason.reason_T = DS_3GPP_PS_IFACE_NET_DOWN_REASON_T;
        bearer_context_p->ds_bearer_context_dyn_p->pdn_context_cb_tbl_ptr->bearer_down_ind_cb(
  	                  bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr,\
  		              bearer_context_p,
                    net_down_reason ,
		              TRUE);
      }
    }
  }  
} /* ds_gprs_bearer_cntxt_pdp_ip_resume_req_reg_cnf_hdlr() */


/*===========================================================================
  FUNCTION DS_GPRS_BEARER_CNTXT_RESUME_PDP_IP_PROTO_STACK

  DESCRIPTION
    This function initializes the protocol stack components specific to 
    GPRS for a PDP type IP call
  
  PARAMETERS   
   *bearer_context_p - Pointer to the bearer context that we are working on   
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    DS3G_SUCCESS : Successful setup
    DS3G_FAILURE : Setup error
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
LOCAL boolean ds_gprs_bearer_cntxt_resume_pdp_ip_proto_stack
(
  ds_bearer_context_s            *bearer_context_p  
)
{ 
  sys_modem_as_id_e_type subs_id = SYS_MODEM_AS_ID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds_bearer_cntxt_get_subs_id_from_bearer(bearer_context_p, &subs_id))
  {
    return FALSE;
  }

  DS_GPRS_MSG1_HIGH("Bearer Inst:%d Setting GPRS spec:PDP-IP stack component",
          (uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr);  

  gsndcp_pdp_register_req
  (
    bearer_context_p->ds_bearer_context_dyn_p->nsapi,                    /* nsapi                  */
    NULL,                                       /* pdp_ul_suspend_fnc_ptr */
    NULL,                                       /* pdp_ul_resume_fnc_ptr  */
    NULL,                                       /* pdp_dl_fnc_ptr         */
    (void *)bearer_context_p,                   /* pdp_context            */
    TRUE,                                       /* cipher                 */
    ds_gprs_bearer_cntxt_pdp_ip_resume_req_reg_cnf
#ifdef FEATURE_DUAL_SIM
    ,subs_id
#endif /* FEATURE_DUAL_SIM */
  );
  return DS3G_SUCCESS;

}  /* ds_gprs_bearer_cntxt_resume_pdp_ip_proto_stack() */

/*===========================================================================
FUNCTION DS_EPS_BEARER_CNTXT_EMPTY_WATERMARK()

DESCRIPTION
  This is the function flushes the uplink WM.
      
PARAMETERS
  
DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  
  
===========================================================================*/
LOCAL void ds_gprs_bearer_cntxt_empty_watermark
(
  ds_bearer_context_s            *bearer_context_p  
)
{
  dsm_empty_queue ( &(bearer_context_p->tx_wm.wm_item) );
} /* ds_gprs_bearer_cntxt_empty_watermark */

/*===========================================================================
FUNCTION DS_GPRS_BEARER_CNTXT_RESUME_IND_HDLR()

DESCRIPTION
  This is the function that is registered for GPRS in the umts_bearer context
  function table. The registration happens in gprs_init.
  This function is called when the resume_ind_hdlr is invoked from DSMGR.
  This function is responsible for setting up the lower layers on RAT change.
      
PARAMETERS
  *bearer_context_p - This is the bearer to be set up  

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
  
===========================================================================*/
LOCAL void ds_gprs_bearer_cntxt_resume_ind_hdlr
(
  ds_bearer_context_s            *bearer_context_p  
)
{
  ds_umts_pdp_type_enum_type    pdp_type;    
  dsm_watermark_type   *tx_wm_p=NULL; 
#ifdef FEATURE_QXDM_WM_TRACING
  uint32 callid;
#endif /* FEATURE_QXDM_WM_TRACING */

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }
/*--------------------------------------------------------------------------*/

  DS_GPRS_MSG1_HIGH("Resuming connection to GPRS Radio Layer Inst:0x%x ",
           bearer_context_p);

  tx_wm_p = &(bearer_context_p->tx_wm.wm_item);
  /*-------------------------------------------------------------------------
    set each enqueue cb to null as we enter gprs mode
  -------------------------------------------------------------------------*/
  tx_wm_p->each_enqueue_func_ptr = NULL;
  tx_wm_p->each_enqueue_func_data = NULL;

  if (((ds_pdn_context_s*)(bearer_context_p->ds_bearer_context_dyn_p->
                           pdn_context_ptr) == NULL) || 
      (!ds_3gpp_pdn_cntx_validate_pdn_context((ds_pdn_context_s*)
                                   (bearer_context_p->ds_bearer_context_dyn_p
                                    ->pdn_context_ptr))))
  {
    return;
  }

  bearer_context_p->ds_bearer_context_dyn_p->is_ho2GPRS_in_progress = FALSE;
  /*-------------------------------------------------------------------------
    We need to know what kind of PDP call we originated in the first place.
    This is required for configuring the PROTOCOL stacks appropriately
  -------------------------------------------------------------------------*/
  pdp_type = ((ds_pdn_context_s*)(bearer_context_p->ds_bearer_context_dyn_p
                                  ->pdn_context_ptr))->
    ds_pdn_context_dyn_p->pdp_profile.context.pdp_type;
  switch(pdp_type)
  {
    case DS_UMTS_PDP_PPP:
      /* VRK:TODO DO we need PPP in GPRS? 
      ds_gprs_bearer_cntxt_setup_pdp_ppp_proto_stack
                                         (
                                          bearer_context_p,
                                          mode_info_p
                                        );
      */
      break;

    case DS_UMTS_PDP_IP :
#ifdef FEATURE_DATA_PS_IPV6
    case DS_UMTS_PDP_IPV6:
    case DS_UMTS_PDP_IPV4V6:
#endif      
      {
        /*-------------------------------------------------------------------
          Setup IP stack only if it is not a Secondary call
        --------------------------------------------------------------------*/
        ds_gprs_bearer_cntxt_resume_pdp_ip_proto_stack
                                             (
                                               bearer_context_p                                                    
                                              );
#ifdef FEATURE_QXDM_WM_TRACING
        callid = ds_bearer_cntxt_get_call_id(bearer_context_p);
        fc_post_modify_cmd_wm_trace(
           callid, UNIFIED_3GPP_MODE_HANDLER_CLIENT, GSM_TECH);
#endif /* FEATURE_QXDM_WM_TRACING */
      }      
      break;

    default:
      DS_GPRS_MSG2_ERROR(
         "Unknown PDP Type: pdp type:%d Bearer Inst:%d",
         pdp_type,
         (uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr);
  }
  return;
} /*ds_gprs_bearer_cntxt_resume_ind_hdlr()*/

/*===========================================================================
FUNCTION DS_GPRS_BEARER_CNTXT_PDP_IP_REQ_REG_CNF

DESCRIPTION
  This function posts a command to set up the transmit function once 
  registration with SNDCP is complete.

PARAMETERS
  Context - PDP context instance
  cnf_status - Indicates whether registration was successful or not.
  Setup the transmit function only if registration was successful. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds_gprs_bearer_cntxt_pdp_ip_req_reg_cnf
(
  void          *context,
  boolean       cnf_status
)
{
  ds_cmd_type                    *cmd_ptr;                 /* DS Task message */
  ds_bearer_context_s            *bearer_context_p = (ds_bearer_context_s*)context;
  ds_gprs_gsndcp_reg_cnf_ev_type *reg_cnf_ev_ptr = NULL;

  DS_GPRS_MSG0_HIGH("DS3GPP GSNDCP REG CNF EV: ");

  /*------------------------------------------------------------------------
    Post command to DS task to process in the proper task context
   ------------------------------------------------------------------------*/

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_gprs_gsndcp_reg_cnf_ev_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_GSNDCP_REG_CNF;
  reg_cnf_ev_ptr = (ds_gprs_gsndcp_reg_cnf_ev_type*)cmd_ptr->cmd_payload_ptr;

  reg_cnf_ev_ptr->bearer_context_p = bearer_context_p;
  reg_cnf_ev_ptr->cnf_status = cnf_status;

  ds_put_cmd(cmd_ptr);
} /* ds_gprs_bearer_cntxt_pdp_ip_req_reg_cnf */

/*===========================================================================
FUNCTION DS_GPRS_BEARER_CNTXT_PDP_IP_REQ_REG_CNF_HDLR

DESCRIPTION
  This function sets up the transmit function once registration with 
  SNDCP is complete.

PARAMETERS
  Context - PDP context instance
  cnf_status - Indicates whether registration was successful or not.
  Setup the transmit function only if registration was successful. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds_gprs_bearer_cntxt_pdp_ip_req_reg_cnf_hdlr
(
  ds_bearer_context_s            *bearer_context_p,
  boolean                         cnf_status
)
{  
  int                                    ret_val;
  int16                                  ps_errno;
  ds_bearer_context_wm_s                 *rx_watermark_p;  
  ds_bearer_context_wm_s                 *tx_watermark_p;  
  ps_phys_link_link_protocol_handle_type  protocol_handle;
  sys_modem_as_id_e_type                  subs_id = SYS_MODEM_AS_ID_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds_bearer_cntxt_get_subs_id_from_bearer(bearer_context_p, &subs_id))
  {
    return;
  }

  if((ds3gpp_get_ps_data_suspend_flag(subs_id) == TRUE) ||
     ((bearer_context_p->ds_bearer_context_dyn_p->state != 
       DS_BEARER_CONTEXT_STATE_UP)&&
     (bearer_context_p->ds_bearer_context_dyn_p->state != 
      DS_BEARER_CONTEXT_STATE_UP_DORMANT)&&
     (bearer_context_p->ds_bearer_context_dyn_p->state != 
      DS_BEARER_CONTEXT_STATE_UP_DORMANT_REESTAB)))
  {
    DS_GPRS_MSG0_HIGH(" Reg req called in wrong state");
    return;
  }

  if ( cnf_status == TRUE )
  {
    /*-----------------------------------------------------------------------  
      Set the Tx function.
    -----------------------------------------------------------------------*/
    

    ps_phys_link_set_tx_function(&(bearer_context_p->
                                   phys_link),\
                                 ds_gprs_bearer_cntxt_pdp_ip_tx_um_data_cb,\
                                 bearer_context_p);

    /*---------------------------------------------------------------------------
     * Set the link layer protocol to be NONE and fill the protocol handle 
     * correctly so that when a packet is input into this phys link, it is routed
     * to the approp iface. 
    ---------------------------------------------------------------------------*/

    /*---------------------------------------------------------------------------
     * We are filling the protocol handle structure with the iface pointers
    ---------------------------------------------------------------------------*/
    memset(&protocol_handle, 0x0, sizeof(ps_phys_link_link_protocol_handle_type));

    if (((ds_pdn_context_s*)(bearer_context_p->ds_bearer_context_dyn_p->
                             pdn_context_ptr) == NULL) || 
        (!ds_3gpp_pdn_cntx_validate_pdn_context((ds_pdn_context_s*)
                                           (bearer_context_p->ds_bearer_context_dyn_p
                                            ->pdn_context_ptr))))
    {
      return;
    }

    if(((ds_pdn_context_s*)(bearer_context_p->ds_bearer_context_dyn_p
                            ->pdn_context_ptr))->
     ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV4_IFACE]
        != NULL)
    {
      protocol_handle.none_handle.handle.ip_proto_handle.v4_iface_ptr = \
  	      &(((ds_pdn_context_s*)(bearer_context_p->
                                 ds_bearer_context_dyn_p->pdn_context_ptr))->
            ds_pdn_context_dyn_p->
  		      ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV4_IFACE]->ps_iface);
    }

    if(((ds_pdn_context_s*)(bearer_context_p->ds_bearer_context_dyn_p->
                            pdn_context_ptr))->
       ds_pdn_context_dyn_p->
  		ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV6_IFACE] != NULL)
    {
      protocol_handle.none_handle.handle.ip_proto_handle.v6_iface_ptr = \
            &(((ds_pdn_context_s*)(bearer_context_p->ds_bearer_context_dyn_p->
                                   pdn_context_ptr))->
              ds_pdn_context_dyn_p->
  		      ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV6_IFACE]->ps_iface);
    }

    protocol_handle.none_handle.high_protocol = 
  	                              PS_PHYS_LINK_HIGHER_LAYER_PROTOCOL_IP;

    /*---------------------------------------------------------------------------
      Set the link layer protocol to NONE and pass the protocol handle into the
      function that will do this. The protocol handle has just been populated
      with the higher layer protocol and the pointers to the ifaces that might
      communicate over this physlink.
    ---------------------------------------------------------------------------*/
    (void) ps_phys_link_set_link_protocol(&(bearer_context_p->
                                            phys_link),
                                          PS_PHYS_LINK_LINK_PROTOCOL_NONE,
                                          protocol_handle);


    /*-------------------------------------------------------------------------
      Set the handler for the downlink signal for the call.
    -------------------------------------------------------------------------*/
    (void)ps_set_sig_handler(bearer_context_p->rx_sig,
                  ds_gprs_bearer_cntxt_pdp_ip_rx_um_data_cb, 
                  (void *)bearer_context_p);

    /*-------------------------------------------------------------------------  
      Enable the rx signal.
    -------------------------------------------------------------------------*/
    ps_enable_sig(bearer_context_p->rx_sig);

    /*-------------------------------------------------------------------------  
      Enable rm_flow_enabled
    -------------------------------------------------------------------------*/
    bearer_context_p->rm_flow_enabled = TRUE;

    /*-------------------------------------------------------------------------  
      Set the PS->SNDCP watermark.
    -------------------------------------------------------------------------*/    
    tx_watermark_p = &(bearer_context_p->tx_wm);
    ds_gprs_bearer_cntxt_setup_pdp_ps_to_sndcp_wm(bearer_context_p);

    //device_state_ptr->ps_tx_wm_item.total_rcvd_cnt       = 0;

    /*-------------------------------------------------------------------------  
      Set the SNDCP->PS watermark.
    -------------------------------------------------------------------------*/
    rx_watermark_p = &(bearer_context_p->rx_wm);
    ds_gprs_bearer_cntxt_setup_pdp_sndcp_to_ps_wm(bearer_context_p);

    /*-------------------------------------------------------------------------
      Cleanup the Tx watermark just in case any data got enqueued after
      the previous call ended.
    -------------------------------------------------------------------------*/
    dsm_empty_queue(&tx_watermark_p->wm_item);

    /*-----------------------------------------------------------------------
     Register WM with SNDCP 
    -----------------------------------------------------------------------*/
    ds_gprs_bearer_cntxt_register_wm_with_sndcp(
                &rx_watermark_p->wm_item,
                ds_gprs_bearer_cntxt_rx_data_at_u_pdp_cb,  
                &tx_watermark_p->wm_item,
                bearer_context_p);

    /*----------------------------------------------------------------------
       Notify PDN that the bearer setup is complete
    ----------------------------------------------------------------------*/
    DS_GPRS_MSG1_HIGH("BEARER UP IND CB for bearer :0x%x",bearer_context_p);

    if(dsUmtsBearerCntxt_activation(bearer_context_p) == DS3G_FAILURE)
    {
      DS_3GPP_MSG0_ERROR ("Bearer activation failed for GPRS, "
                          "Freeing resources");
      ret_val = ps_phys_link_down_cmd(&(bearer_context_p->phys_link),
                                      &ps_errno,
                                      NULL);
      if((ret_val == 0) || (ps_errno != DS_EWOULDBLOCK))
      {
        if(bearer_context_p->ds_bearer_context_dyn_p->is_registered_with_dpm)
        {
          ds_bearer_cntx_deregister_dpm(bearer_context_p);   
          bearer_context_p->ds_bearer_context_dyn_p->is_registered_with_dpm
             = FALSE;
          goto cleanup;   
        }
      }
    }
    return;
  }
  else
  {
    /*-----------------------------------------------------------------------
      End the call since we cannot setup the link to SNDCP
    -----------------------------------------------------------------------*/    
    ret_val = ps_phys_link_down_cmd(&(bearer_context_p->phys_link),
                                    &ps_errno,
                                    NULL);

    if ((ret_val == 0) || (ps_errno != DS_EWOULDBLOCK))
    {
      goto cleanup;
    }
    return;
  }
cleanup:
  if (bearer_context_p->ds_bearer_context_dyn_p->is_default)
  {
    /*-------------------------------------------------------------------
      Error in setting up default bearer -> Make sure that we call flow 
      go null ind on default flows of all ifaces associated with the PDN 
      context          
    --------------------------------------------------------------------*/
    ds_pdn_cntxt_unbind_default_flows(bearer_context_p->ds_bearer_context_dyn_p
                                      ->pdn_context_ptr);        
  }

  /*-------------------------------------------------------------------
    Set the Bearer context to be in down state and call the bearer down
    call back to the PDN context.
  --------------------------------------------------------------------*/
  
  /* Is it safe to free the bearer inst ?*/
  ds_bearer_cntxt_free_bearer_inst(bearer_context_p);

  /*---------------------------------------------------------------------
    Each enqueue func_ptr needs to be reset when bearer inst is freed.
    This needs to be done to avoid race conditions when phys_link_gone is
    posted and flow gets enabled on the phys_link leading to each_enq_f
    called when the bearer is getting freed.
  ---------------------------------------------------------------------*/
  ps_phys_link_enable_flow(&(bearer_context_p->phys_link),
                           DS_FLOW_UMTS_MASK);
  ps_phys_link_gone_ind(&(bearer_context_p->phys_link));

  /*-----------------------------------------------------------------------
    We have already cleaned up at the bearer level. We should send
    bearer down indication to PDN context with the boolean call_was_conn
    set to FALSE.
    This way the PDN context will be freed up as well.
  -----------------------------------------------------------------------*/
  if (bearer_context_p->ds_bearer_context_dyn_p->pdn_context_cb_tbl_ptr
      ->bearer_down_ind_cb != NULL)
  {
    ds_3gpp_down_reason_t net_down_reason;
    net_down_reason.reason.ps_iface_down_reason = PS_NET_DOWN_REASON_NOT_SPECIFIED;
    net_down_reason.reason_T = DS_3GPP_PS_IFACE_NET_DOWN_REASON_T;
    bearer_context_p->ds_bearer_context_dyn_p->pdn_context_cb_tbl_ptr->
      bearer_down_ind_cb(
                  bearer_context_p->ds_bearer_context_dyn_p->
                  pdn_context_ptr,\
                  bearer_context_p,
                  net_down_reason,
                  FALSE);
  }

} /* ds_gprs_bearer_cntxt_pdp_ip_req_reg_cnf_hdlr() */

/*===========================================================================
  FUNCTION DS_GPRS_BEARER_CNTXT_SETUP_PDP_IP_PROTO_STACK

  DESCRIPTION
    This function initializes the protocol stack components specific to 
    GPRS for a PDP type IP call
  
  PARAMETERS   
   *bearer_context_p - Pointer to the bearer context that we are working on
   *mode_info_ptr    - Pointer to the mode-specific call information provided  
                       by Call Manager
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    DS3G_SUCCESS : Successful setup
    DS3G_FAILURE : Setup error
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
LOCAL boolean ds_gprs_bearer_cntxt_setup_pdp_ip_proto_stack
(
  ds_bearer_context_s            *bearer_context_p,
  const cm_call_mode_info_s_type *mode_info_ptr
)
{
  sys_modem_as_id_e_type subs_id = SYS_MODEM_AS_ID_NONE;
  /*---------------------------------------------------------------*/

  if (!ds_bearer_cntxt_get_subs_id_from_bearer(bearer_context_p, &subs_id))
  {
    return FALSE;
  }

  DS_GPRS_MSG1_HIGH("Bearer Inst:%d Setting GPRS spec:PDP-IP stack component",
          (uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr);  

  gsndcp_pdp_register_req
  (
    bearer_context_p->ds_bearer_context_dyn_p->nsapi,                    /* nsapi                  */
    NULL,                                       /* pdp_ul_suspend_fnc_ptr */
    NULL,                                       /* pdp_ul_resume_fnc_ptr  */
    NULL,                                       /* pdp_dl_fnc_ptr         */
    (void *)bearer_context_p,                   /* pdp_context            */
    TRUE,                                       /* cipher                 */
    ds_gprs_bearer_cntxt_pdp_ip_req_reg_cnf
#ifdef FEATURE_DUAL_SIM
    ,subs_id
#endif /* FEATURE_DUAL_SIM */
  );
  return DS3G_SUCCESS;

}  /* ds_gprs_bearer_cntxt_setup_pdp_ip_proto_stack() */

/*===========================================================================
FUNCTION DS_GPRS_BEARER_CNTXT_ACTIVATE_CNF()

DESCRIPTION
  This is the function that is registered for GPRS in the umts_bearer context
  function table. The registration happens in gprs_init.
  This function is called when the call_connected_hdlr is invoked from DSMGR.
  This function is responsible for setting up the lower layers.
      
PARAMETERS
  *bearer_context_p - This is the bearer to be set up
  *mode_info_p      - The CM call mode info structure
  *end_params_p     - End params structure that is to be populated if there
                      is a failure somewhere.

DEPENDENCIES
  None.

RETURN VALUE
  DS3G_SUCCESS or DS3G_FAILURE depending on the status of setting up lower
  layers.
  
SIDE EFFECTS
  None.
  
===========================================================================*/
LOCAL boolean ds_gprs_bearer_cntxt_activate_cnf
(
  ds_bearer_context_s            *bearer_context_p,
  const cm_call_mode_info_s_type *mode_info_p,
  cm_end_params_s_type           *end_parms_p
)
{
  ds_umts_pdp_type_enum_type    pdp_type;
  boolean                       result;
/*--------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    We need to know what kind of PDP call we originated in the first place.
    This is required for configuring the PROTOCOL stacks appropriately
  -------------------------------------------------------------------------*/

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return FALSE;
  }

  if (((ds_pdn_context_s*)(bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr) 
       == NULL) || 
      (!ds_3gpp_pdn_cntx_validate_pdn_context((ds_pdn_context_s*)
                                    (bearer_context_p->ds_bearer_context_dyn_p->
                                     pdn_context_ptr))))
  {
    return FALSE;
  }

  pdp_type = ((ds_pdn_context_s*)(bearer_context_p->ds_bearer_context_dyn_p->
                                  pdn_context_ptr))->
    ds_pdn_context_dyn_p->pdp_profile.context.pdp_type;

  switch(pdp_type)
  {
    case DS_UMTS_PDP_IP :
#ifdef FEATURE_DATA_PS_IPV6
    case DS_UMTS_PDP_IPV6:
    case DS_UMTS_PDP_IPV4V6:
#endif      
      {
        /*-------------------------------------------------------------------
          Setup IP stack only if it is not a Secondary call
        --------------------------------------------------------------------*/
        result = ds_gprs_bearer_cntxt_setup_pdp_ip_proto_stack
                                                  (
                                                    bearer_context_p,
                                                    mode_info_p
                                                  );
      }      
      break;

    default:
      DS_GPRS_MSG2_ERROR(
         "Unknown PDP Type: pdp type:%d Bearer Inst:%d",
         pdp_type,
         (uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr);
      result = DS3G_FAILURE;
  }
  return result;
} /*ds_gprs_bearer_cntxt_activate_cnf()*/

/*===========================================================================
FUNCTION  DS_GPRS_BEARER_CNTXT_BEARER_TECH_TYPE_HDLR

DESCRIPTION
  This function returns the current data bearer tech.
  
PARAMETERS  
  *bearer_tech_ptr - bearer_tech_ptr
  *ps_errno        - ps error code
   subs_id         - Subscription id
    
DEPENDENCIES
  None.
  
RETURN VALUE
  0  - SUCCESS
  -1 - FAILURE
    
SIDE EFFECTS
  None.
  
===========================================================================*/
int ds_gprs_bearer_cntxt_bearer_tech_type_hdlr
(
  ps_iface_bearer_technology_type    *bearer_tech_ptr,
  sint15                             *ps_errno,
  sys_modem_as_id_e_type              subs_id
)
{
  cm_cell_srv_ind_s_type  cell_srv_ind;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DS_GPRS_MSG1_HIGH("Bearer tech ptr is 0x%x", bearer_tech_ptr);

  if ( bearer_tech_ptr == NULL )
  {
    DS_GPRS_MSG0_HIGH("Bearer_tech_ptr is NULL, "); 
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /* Start off with a clean rat_mask */
  bearer_tech_ptr->data.umts_type.rat_mask = 0x0;

  /*-------------------------------------------------------------------------
    Use last reported EGPRS support status
  -------------------------------------------------------------------------*/
  cell_srv_ind = dsUmtsBearerCntxtGetCellSrvInd(subs_id);
  bearer_tech_ptr->data.umts_type.rat_mask = 
    ds_gprs_bearer_cntxt_get_ps_rat_mask_for_egprs_support(
       cell_srv_ind.egprs_supp);


  DS_GPRS_MSG1_HIGH("setting rat mask to %d", 
            bearer_tech_ptr->data.umts_type.rat_mask);
  
  return 0;
} /* ds_gprs_bearer_cntxt_bearer_tech_type_hdlr */

#ifdef FEATURE_DATA_WLAN_MAPCON
/*===========================================================================
FUNCTION  DS_GPRS_BEARER_CNTXT_BEARER_TECH_TYPE_HDLR

DESCRIPTION
  This function returns the current data bearer tech.
  
PARAMETERS  
  *bearer_tech_ptr - bearer_tech_ptr
   subs_id         - subscription id
    
DEPENDENCIES
  None.
  
RETURN VALUE
  0  - SUCCESS
  -1 - FAILURE
    
SIDE EFFECTS
  None.
  
===========================================================================*/
int ds_gprs_bearer_cntxt_bearer_tech_type_ex_hdlr
(
  ds_sys_system_status_info_type    *bearer_tech_ex_ptr,
  sys_modem_as_id_e_type             subs_id
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_GPRS_MSG0_LOW("Mode passed in is GPRS");

  ASSERT(bearer_tech_ex_ptr);

  bearer_tech_ex_ptr->technology = DS_SYS_NETWORK_3GPP;
  bearer_tech_ex_ptr->rat_value = DS_SYS_RAT_EX_3GPP_GERAN;
      
  bearer_tech_ex_ptr->so_mask = ds_3gpp_bearer_cntxt_get_so_mask(subs_id);

  return 0;
} /* ds_gprs_bearer_cntxt_bearer_tech_type_hdlr */
#endif /* FEATURE_DATA_WLAN_MAPCON */

/*===========================================================================
FUNCTION ds_gprs_bearer_cntxt_calculate_actual_throughput

DESCRIPTION
  This function calculates the actual throughput for all active GPRS calls

PARAMETERS
  int index.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None.

===========================================================================*/

boolean ds_gprs_bearer_cntxt_calculate_actual_throughput
(
   ds_bearer_context_s    *bearer_context_p,
   sys_modem_as_id_e_type  cm_subs_id,
   uint32                  sampling_interval
)
{
  boolean                     ret_val             = FALSE;
  geran_bearer_data_rate_t    geran_data_rate;  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------- 
    Sampling Interval is not required for GPRS as the bearer data rates
    are directly acquired from Lower layers
    -------------------------------------------------------------------------*/
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return ret_val;
  }

  if(!ds3gsubsmgr_is_subs_id_valid(cm_subs_id))
  {
    return ret_val;
  }
  DS_3GPP_MSG2_HIGH("Updating Actual throughput for Bearer 0x%x bearer call "
                    "mode %d",bearer_context_p,
                    bearer_context_p->ds_bearer_context_dyn_p->call_mode);

  /*------------------------------------------------------------------------- 
    Get the allowed rate from the Lower layers
    -------------------------------------------------------------------------*/
#ifdef FEATURE_DUAL_SIM
  geran_get_bearer_data_rates_as_id(&geran_data_rate, cm_subs_id);
#else
  geran_get_bearer_data_rates(&geran_data_rate);
#endif /* FEATURE_DUAL_SIM */

  DS_3GPP_MSG2_HIGH("GERAN LL reports Current tx rate %d bps current rx %d "
                    "bps",geran_data_rate.current_tx_bearer_data_rate,
                    geran_data_rate.current_rx_bearer_data_rate);

  /*------------------------------------------------------------------------- 
    GERAN reports in bps. Convert it to Kbps
    -------------------------------------------------------------------------*/
  bearer_context_p->ds_bearer_context_dyn_p->
     bearer_throughput_stats.actual_ul_rate    = 
                     DSUTIL_BITS_TO_KBITS(geran_data_rate.current_tx_bearer_data_rate);

  bearer_context_p->ds_bearer_context_dyn_p->
     bearer_throughput_stats.actual_dl_rate    = 
                     DSUTIL_BITS_TO_KBITS(geran_data_rate.current_rx_bearer_data_rate);

  ret_val = TRUE;

  return ret_val;
}
/*===========================================================================
FUNCTION  DS_GPRS_BEARER_CNTXT_BEARER_RATE_HDLR
DESCRIPTION
  This function returns the current data bearer rate.  APIs to support
  this functionality is not available from GPRS stack, so they all return
  -1 (unknown) now.
  
PARAMETERS  
  call_inst - index into call table
  *argval_ptr - return value
  *ps_errno - ps error code
   subs_id         - subscription id
    
DEPENDENCIES
  None.
  
RETURN VALUE
  0 - SUCCESS
  -1 - FAILURE
    
SIDE EFFECTS
  None.
  
===========================================================================*/
int ds_gprs_bearer_cntxt_bearer_rate_hdlr
(
  uint32                   call_inst,
  void                    *argval_ptr,
  sint15                  *ps_errno,
  sys_modem_as_id_e_type   subs_id
) 
{
  geran_bearer_data_rate_t geran_data_rate;
  ps_iface_ioctl_data_bearer_rate *rate_info = 
              (ps_iface_ioctl_data_bearer_rate*)argval_ptr;

  //Initialize elements of geran_data_rate
  memset(&geran_data_rate, 0, sizeof(geran_bearer_data_rate_t));

#ifdef FEATURE_DUAL_SIM
  geran_get_bearer_data_rates_as_id(&geran_data_rate, subs_id);
#else
  geran_get_bearer_data_rates(&geran_data_rate);
#endif /* FEATURE_DUAL_SIM */

  /*-------------------------------------------------------------------------
  Right now, there is no support from lower layers for any type of tx and rx
  rates, setting all values to -1 which means unknown.
  -------------------------------------------------------------------------*/
  rate_info->max_tx_bearer_data_rate = geran_data_rate.max_tx_bearer_data_rate;
  rate_info->max_rx_bearer_data_rate = geran_data_rate.max_rx_bearer_data_rate;
  rate_info->current_tx_bearer_data_rate = geran_data_rate.current_tx_bearer_data_rate;
  rate_info->current_rx_bearer_data_rate = geran_data_rate.current_rx_bearer_data_rate;

  DS_GPRS_MSG2_MED("setting gprs max_tx_rate to %d, max_rx_rate to %d",
          rate_info->max_tx_bearer_data_rate, rate_info->max_rx_bearer_data_rate);

  DS_GPRS_MSG2_MED("setting gprs current_tx_rate to %d, current_rx_rate to %d",
          rate_info->current_tx_bearer_data_rate, 
          rate_info->current_rx_bearer_data_rate);

  rate_info->avg_tx_bearer_data_rate = -1;
  rate_info->avg_rx_bearer_data_rate = -1;

  DS_GPRS_MSG0_LOW("setting gprs avg data rates to -1");
  return 0;
} /* dsgpsd_get_data_bearer_rate_hdlr() */

/*===========================================================================
FUNCTION DS_GPRS_BEARER_CNTXT_INIT

DESCRIPTION
  This function is used to initialize the GPRS bearer context.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_gprs_bearer_cntxt_init( void )
{   
  ds_umts_bearer_context_ftbl_s *gprs_bearer_context_ftbl_p;

  gprs_bearer_context_ftbl_p = 
      &(umts_bearer_context_ftbl[DS_3GPPI_SYS_MODE_GSM]);

  memset(
      (void*)gprs_bearer_context_ftbl_p,
      0x0,
      (sizeof(ds_umts_bearer_context_ftbl_s) )
      );

  /*---------------------------------------------------------------------------
    Fill up the GPRS function table with the functions implemented in this
    file.
  ---------------------------------------------------------------------------*/
  DS_UMTS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, activateCnf, \
                        ds_gprs_bearer_cntxt_activate_cnf);  
  DS_UMTS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, deactivateCnf, \
                        ds_gprs_bearer_cntxt_deactivate_cnf);
  DS_UMTS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, suspend_ind, \
                        ds_gprs_bearer_cntxt_suspend_ind_hdlr);
  DS_UMTS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, resume_ind, \
                        ds_gprs_bearer_cntxt_resume_ind_hdlr);
  DS_UMTS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, getBearerTechType,
                        ds_gprs_bearer_cntxt_bearer_tech_type_hdlr);
  DS_UMTS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, getratMaskforEgprsSupport, \
                        ds_gprs_bearer_cntxt_get_ps_rat_mask_for_egprs_support);
#ifdef FEATURE_DATA_WLAN_MAPCON
  DS_UMTS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, getBearerTechTypeEx,
                        ds_gprs_bearer_cntxt_bearer_tech_type_ex_hdlr);
  DS_UMTS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, getSoMask,
                        ds_gprs_bearer_cntxt_get_so_mask_for_egprs_support);
#endif /* FEATURE_DATA_WLAN_MAPCON */
  DS_UMTS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, getBearerRate, \
                        ds_gprs_bearer_cntxt_bearer_rate_hdlr); 
  DS_UMTS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, empty_watermark, \
                        ds_gprs_bearer_cntxt_empty_watermark);

  DS_UMTS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, cntxt_transfer_ind_setup, \
                                    ds_gprs_bearer_cntxt_transfer_setup_hdlr);

#if defined(FEATURE_DATA_LTE) && defined (FEATURE_GSM_GPRS)
  DS_UMTS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, cntxt_transfer_ind_release, \
                                    ds_gprs_bearer_cntxt_transfer_release_hdlr);
#endif /* defined(FEATURE_DATA_LTE) && defined(FEATURE_GSM_GPRS) */
 
  DS_UMTS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, mode_pref_change, \
                             ds_gprs_bearer_cntxt_mode_pref_change_hdlr);
 
#ifdef FEATURE_QXDM_WM_TRACING
  DS_UMTS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, report_wm_stats, \
                      ds_gprs_bearer_cntxt_report_wm_stats);

#endif /* FEATURE_QXDM_WM_TRACING */
  DS_UMTS_BEARER_CTXT_VF_REG(SYS_SYS_MODE_GSM, refresh_actual_throughput, \
                                    ds_gprs_bearer_cntxt_calculate_actual_throughput);
}

/*===========================================================================

FUNCTION DSUMTSPS_PROCESS_GSNDCP_EV_CMD

DESCRIPTION

  This function processes the GSNDCP REGISTRATION/RESUMPTION COMPLETE 
  commands received by the DS task. The following are handled:

  DSUMTSPS_GSNDCP_REG_CNF_CMD: Command to indicate registration proceeding
                               call setup with GSNDCP is complete

  DSUMTSPS_GSNDCP_RES_CNF_CMD: Command to indicate registration proceeding 
                               resumption with GSNDCP is complete

PARAMETERS
  cmd_ptr   - Ptr to the command information

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_gprs_bearer_cntxt_process_gsndcp_ev_cmd
(
  const ds_cmd_type *cmd_ptr                         /* Pointer to Command */
)
{
  ds_bearer_context_s            *bearer_context_p;
  boolean                        cnf_status;
  ds_gprs_gsndcp_reg_cnf_ev_type *reg_cnf_ev_ptr = NULL;

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DS_GPRS_MSG0_ERROR("NULL ptr passed, return");
    return;
  }

  reg_cnf_ev_ptr = (ds_gprs_gsndcp_reg_cnf_ev_type*)cmd_ptr->cmd_payload_ptr;

  bearer_context_p = reg_cnf_ev_ptr->bearer_context_p;
  cnf_status = reg_cnf_ev_ptr->cnf_status;

  /*-------------------------------------------------------------------------
    Invoke the appropriate function to process the command, based on the
    command id.
  -------------------------------------------------------------------------*/
  switch(cmd_ptr->hdr.cmd_id)
  {
    case DS_CMD_3GPP_GSNDCP_REG_CNF:
    
      ds_gprs_bearer_cntxt_pdp_ip_req_reg_cnf_hdlr(bearer_context_p, 
                                                   cnf_status);
    
      break;

    case DS_CMD_3GPP_GSNDCP_RESUME_CNF:
    
      ds_gprs_bearer_cntxt_pdp_ip_resume_req_reg_cnf_hdlr(bearer_context_p, 
                                                          cnf_status);
    
      break;

    default:
      DS_GPRS_MSG1_ERROR("Unknown cmd to DS3GPP Mode hdlr: %d",
                         cmd_ptr->hdr.cmd_id);
      break;

  } /* switch( cmd_id ) */
}

/*===========================================================================
FUNCTION DS_GPRS_BEARER_CONTEXT_REFRESH_VOTING_HDLR

DESCRIPTION   
  This function checks the state of all GPRS PS calls and decides whether an
  ISIM refresh will be allowed to proceed.

PARAMETERS   
   subs_id         - subscription id
  
DEPENDENCIES  
  None.

RETURN VALUE  
  TRUE  - if SIM Refresh can proceed from mode handler perspective.
  FALSE - if SIM Refresh cannot proceed from mode handler perspective.

SIDE EFFECTS  
  None.
===========================================================================*/
boolean ds_gprs_bearer_cntxt_refresh_voting_hdlr
(
  sys_modem_as_id_e_type      subs_id
)
{
  /* Since GPRS does not have the concept of dormancy, but IMS call will 
  always be up, GPRS will always vote yes */
  DS_GPRS_MSG0_HIGH("GPRS SIM refresh handler invoked, voting YES.");
  return TRUE;
} /* ds_gprs_bearer_cntxt_refresh_voting_hdlr */

/*===========================================================================
FUNCTION DS_GPRS_BEARER_CNTXT_GET_PS_RAT_MASK_FOR_EGPRS_SUPPORT

DESCRIPTION
  This function returns the RAT MASK corresponding to the passed EGPRS 
  support.

PARAMETERS
  egprs_support - Indicates whether EGPRS is supported or not

DEPENDENCIES
  None.

RETURN VALUE
  RAT mask 

SIDE EFFECTS
  None.
===========================================================================*/
uint32 ds_gprs_bearer_cntxt_get_ps_rat_mask_for_egprs_support
(
  sys_egprs_support_e_type  egprs_support
)
{
  uint32 rat_mask = PS_IFACE_3GPP_NULL_BEARER;

  switch (egprs_support)
  {
    case SYS_EGPRS_SUPPORT_NOT_AVAIL:
      rat_mask = PS_IFACE_3GPP_GPRS;
      break;

    case SYS_EGPRS_SUPPORT_AVAIL:
      rat_mask = PS_IFACE_3GPP_EDGE;
      break;

    default:
      DS_GPRS_MSG1_HIGH("Egprs_support value [%d] unrecognized!", egprs_support);
      break;
  }

  return rat_mask;
} /* ds_gprs_bearer_cntxt_get_ps_rat_mask_for_egprs_support */

#ifdef FEATURE_QXDM_WM_TRACING
/*===========================================================================
FUNCTION DS_GPRS_BEARER_CNTXT_REPORT_WM_STATS

DESCRIPTION
  This function stores the current watermark status.

PARAMETERS
  bearer_instance - bearer context instance 
  call_id         - call id
  wm_stat_ptr     - where the wm values are stored

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_gprs_bearer_cntxt_report_wm_stats
(
  uint32          bearer_instance,
  uint32          call_id,
  fc_wm_stat_type *wm_stat_ptr
)
{
  ds_bearer_context_s         *bearer_context_p;
  int                  index;

  /*---------------------------------------------------------------------------
    Get the bearer context pointer
  ---------------------------------------------------------------------------*/
  if( bearer_instance >= DS_3GPP_MAX_BEARER_CONTEXT )
  {
    DS_GPRS_MSG0_ERROR("bearer_instance is greater than or equal to "
              "DS_3GPP_MAX_BEARER_CONTEXT ");
    return;
  }
  else
  {
    bearer_context_p = ds_bearer_cntxt_get_instance_ptr(bearer_instance);
  }

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }

  /*---------------------------------------------------------------------------
    WM_DIR_RLC_PS_DL
  ---------------------------------------------------------------------------*/
  /* Index to wm_info array provided by FC */
  index = wm_stat_ptr->num_wms;

  if(index >= MAX_WM_CNT) 
  {
    DS_GPRS_MSG0_ERROR("Max WM count reached. Cannot report statistics");
    return;
  }

  DS_GPRS_MSG0_MED("Reporting statistics for WM_DIR_SNDCP_PS_DL");
  /* Report the technology */
  wm_stat_ptr->wm_info[index].tech_id = GSM_TECH;

  /* Report the Call ID */
  wm_stat_ptr->wm_info[index].call_id = call_id;

  /* Specify the WM for which the stats are being reported */
  wm_stat_ptr->wm_info[index].wm_id = WM_DIR_SNDCP_PS_DL;

  /* Query DSM for wm stats */
  dsm_get_wm_stats(&(bearer_context_p->rx_wm.wm_item), 
                   &(wm_stat_ptr->wm_info[index].wm_counts));
  wm_stat_ptr->num_wms++;

  /*---------------------------------------------------------------------------
    WM_DIR_RLC_PS_UL
  ---------------------------------------------------------------------------*/
  /* Index to wm_info array provided by FC */
  index = wm_stat_ptr->num_wms;

  if(index >= MAX_WM_CNT) 
  {
    DS_GPRS_MSG0_ERROR("Max WM count reached. Cannot report statistics");
    return;
  }

  DS_GPRS_MSG0_MED("Reporting statistics for WM_DIR_SNDCP_PS_UL");
  /* Report the technology */
  wm_stat_ptr->wm_info[index].tech_id=GSM_TECH;

  /* Report the Call ID */
  wm_stat_ptr->wm_info[index].call_id = call_id;

  /* Specify the WM for which the stats are being reported */
  wm_stat_ptr->wm_info[index].wm_id = WM_DIR_SNDCP_PS_UL;

  /* Query DSM for wm stats */
  dsm_get_wm_stats(&(bearer_context_p->tx_wm.wm_item), 
                   &(wm_stat_ptr->wm_info[index].wm_counts));
  wm_stat_ptr->num_wms++;
} /* ds_gprs_bearer_cntxt_report_wm_stats */
#endif /* FEATURE_QXDM_WM_TRACING */

/*===========================================================================

                                UNIT TEST

===========================================================================*/

#endif /* FEATURE_GSM_GPRS */
