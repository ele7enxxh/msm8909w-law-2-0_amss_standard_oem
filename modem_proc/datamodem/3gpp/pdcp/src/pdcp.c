/*===========================================================================

                           P D C P  R R C  I F

                           H E A D E R   F I L E


DESCRIPTION
  This file contains data declarations and function prototypes that are
  internal to the PDCP layer.
  
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2005 - 2014 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/pdcp/src/pdcp.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/11/11   nd      Feature cleanup.
02/23/11   ttv     Added support for watermark tracing.
02/11/11   ttv     Added NULL check for PDCP to PS WM.
01/22/10   sch     changed init value for rlc_dl_id/rlc_ul_id to invalid
11/23/09   sch     pdcp_reg_wm cmd now contains lc_info (struct) instead of 
                   lc_info_ptr (pointer).
08/31/09   ss      SU level CMI modifications.
07/02/09   vk      Fix to send pending UL data already present in WM during
                   WM registration to RLC.
07/04/09   pp      hton/ntoh* macros replaced with ps_hton/ntoh*.
06/26/09   sch     Null pointer error-check in pdcp_pdcp_to_ps_flow_enable_cb
03/04/09   sa      AU level CMI modifications.
02/01/07   rsl     Changes relating to CPU based flow control.
12/11/06   rsl     Updated rohc_decompress parameter list based on new API.
11/06/06   vk      Added Data on Apps support.
10/30/06   rsl     Discard SN PDU if lossless support is not configured by 
                   network.
10/27/06   rsl     Discard pkt received with invalid pdu_type.
09/19/06   rsl     Removed setting PS and PDCP WM to null in transparent case
                   so that WMs can be tuned dynamically based on rab_rates.
09/01/06   rsl     Moved IPHC and ROHC powerup_init to umts_hdlr_init.
08/31/06   rsl     Featurized PDCP
08/28/06   rsl     Fixed typo in setup_ps_to_pdcp_wm.
08/18/06   rsl     Fixed validate_instance() and code cleanup.
08/31/06   mjp     Added IPHC and ROHC Logging
06/15/06   rr      Fixed Network flow control in transparent mode. DS HI/LO WM 
                   is initialized to RLC->PDCP HI/LO WM callbacks.
06/07/06   rsl     Fixed compile issue in rx_data_hdlr seen with RVCT.
===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_WCDMA_PS
#define FEATURE_DSM_WM_CB
#include "wcdmamvsif.h"
#include "pdcpi.h"
#include "pdcprlcif.h"
#include "ds_3gpp_bearer_context.h"
#include "ds_3gpp_pdn_context.h"
#include "msg.h"
#include "amssassert.h"
#include "rrcdata.h"
#include "dsmbit.h"
#include "ps_rohc.h"
#include "ps_iphc.h"
#include "pdcprrcif.h"
#include "pdcpsig.h"
#include "comptask_v.h"

#include "ps_phys_link.h"
#include "data_err_suppr.h" 

#ifdef FEATURE_QXDM_WM_TRACING
#include "fc_wm.h"
#include "dsm_queue.h"
#endif

#ifdef FEATURE_TDSCDMA
#ifndef FEATURE_SEGMENT_LOADING
//#include "tdsrlcdsapif.h"
#include "tdsl2external.h"
#include "tdsrrcdataext.h"
#endif /* FEATURE_SEGMENT_LOADING */
#endif /* FEATURE_TDSCDMA */

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
#ifdef FEATURE_SEGMENT_LOADING
/*---------------------------------------------------------------------------
TDSCDMA and WCDMA interface tables
---------------------------------------------------------------------------*/
extern interface_t *ds_3gpp_ps_tds_tbl;
#endif /* FEATURE_SEGMENT_LOADING */
/*-------------------------------------------------------------------------
  global PDCP control block
-------------------------------------------------------------------------*/
pdcp_ctl_type pdcp_ctl_blk[MAX_PDCP_INST] = {{NULL}};

/* global PDCP critical section */
rex_crit_sect_type pdcp_crit_section = {{NULL}};


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION      PDCP_INITIALIZE_RLC_STATE

DESCRIPTION   This function initialized the rlc state structure that is part
              of the PDCP control block.

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None
===========================================================================*/
boolean pdcp_initialize_rlc_state
(
  void
)
{

  int i = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MSG0_MED("In pdcp_initialize_rlc_state()");
  PDCP_ENTER_CRIT_SECTION ( &pdcp_crit_section );
  /*-------------------------------------------------------------------------
  Initialize TX & RX queues and watermarks that PDCP owns, and will be used
  by RLC.
  -------------------------------------------------------------------------*/
  for ( i = 0; i < MAX_PDCP_INST; i++ ) 
  {
    memset(&pdcp_ctl_blk[i].rlc_state.rlc_tx_wm_item,
	       0,
	       sizeof(dsm_watermark_type));
    memset(&(pdcp_ctl_blk[i].rlc_state.rlc_tx_q), 0, sizeof(q_type));
	dsm_queue_init (&pdcp_ctl_blk[i].rlc_state.rlc_tx_wm_item,
					4500,
					&(pdcp_ctl_blk[i].rlc_state.rlc_tx_q) );

    memset(&pdcp_ctl_blk[i].rlc_state.rlc_rx_wm_item,
	       0,
	       sizeof(dsm_watermark_type));
    memset(&(pdcp_ctl_blk[i].rlc_state.rlc_rx_q), 0, sizeof(q_type));
	dsm_queue_init (&pdcp_ctl_blk[i].rlc_state.rlc_rx_wm_item,
					340160,
					&(pdcp_ctl_blk[i].rlc_state.rlc_rx_q) );
    pdcp_ctl_blk[i].rlc_state.sent_close_to_rlc = FALSE;
  }
  PDCP_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
  return TRUE;
} /* pdcp_initialize_rlc */

/*===========================================================================
FUNCTION      PDCP_POWERUP_INIT

DESCRIPTION   This function should be called at UE powerup from ps task init.
              Initializes all variables associated with pdcp control block.


DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None
===========================================================================*/
void pdcp_powerup_init
(
  void
)
{
  uint32 i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MSG0_MED("In pdcp_powerup_init()");

  memset(&pdcp_crit_section, 0, sizeof(rex_crit_sect_type));
  PDCP_INIT_CRIT_SECTION ( &pdcp_crit_section );
  PDCP_ENTER_CRIT_SECTION ( &pdcp_crit_section );
  /*-------------------------------------------------------------------------
  Initialize TX & RX queues and watermarks that PDCP owns, and will be used
  by RLC.
  -------------------------------------------------------------------------*/
  if (pdcp_initialize_rlc_state() == FALSE)
  {
    DS_3GPP_MSG0_ERROR("pdcp_init_rlc_state failed");
  }
  
  /*-------------------------------------------------------------------------
  Initialize other fields in pdcp ctl blk.
  -------------------------------------------------------------------------*/
  for ( i = 0; i < MAX_PDCP_INST; i++ ) 
  {
    pdcp_ctl_blk[i].pdcp_entry_valid = FALSE;
    pdcp_ctl_blk[i].next_rx_sn = 0;
    pdcp_ctl_blk[i].pdcp_state = PDCP_DOWN;
    pdcp_ctl_blk[i].pdcp_to_rlc_flow_mask = DS_FLOW_DISABLE;
    pdcp_ctl_blk[i].pdcp_to_ps_flow_mask = DS_FLOW_DISABLE;
    pdcp_ctl_blk[i].ps_to_pdcp_wm = NULL;
    pdcp_ctl_blk[i].pdcp_to_ps_wm = NULL;

    /*-------------------------------------------------------------------------
    Reset UL and DL pid tables for all PDCP inst.
    -------------------------------------------------------------------------*/
    pdcp_reset_pid_tables ((uint8)i);
    
    pdcp_ctl_blk[i].rohc_hdl = NULL;
    pdcp_ctl_blk[i].iphc_hdl = NULL;
    pdcp_ctl_blk[i].rohc_dl_cid_inc_info = ROHC_INVALID_CID_INC;
    pdcp_ctl_blk[i].rohc_ul_cid_inc_info = ROHC_INVALID_CID_INC;

    /*lint -e732 -e641 -e64*/
    pdcp_ctl_blk[i].ps_to_pdcp_sig = COMP_MAX_SIGNALS;
    pdcp_ctl_blk[i].rlc_to_pdcp_sig = (comp_sig_enum_type)(COMP0_RLC_TO_PDCP_DATA_SIG + i);
    pdcp_ctl_blk[i].is_mcast_call = FALSE;

	/*-------------------------------------------------------------------------
	  Set the handler for the RLC to PDCP signal for the call.
	-------------------------------------------------------------------------*/
	(void)comp_set_sig_handler(
	  pdcp_ctl_blk[i].rlc_to_pdcp_sig,
	  pdcp_rx_sig_hdlr,
	  (void *)i);
	/*lint +e732 +e641 +e64*/
	
    pdcp_ctl_blk[i].rlc_state.rlc_dl_id = PDCP_INVALID_RLC_LC_ID;
    pdcp_ctl_blk[i].rlc_state.rlc_ul_id = PDCP_INVALID_RLC_LC_ID;
    pdcp_ctl_blk[i].pdcp_wmk_hi = FALSE;
  }
  PDCP_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
}/* pdcp_powerup_init */


/*===========================================================================
FUNCTION PDCP_VALIDATE_PDCP_INST

DESCRIPTION This function returns TRUE if the entry corresponding to given
            pdcp_inst is valid.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE - if given pdcp_inst is valid
  FALSE - validation fails

SIDE EFFECTS
  None
===========================================================================*/

boolean pdcp_validate_pdcp_inst
(
  uint32 pdcp_inst
)
{
  if (pdcp_inst < MAX_PDCP_INST) 
  {
	return pdcp_ctl_blk[pdcp_inst].pdcp_entry_valid;
  }
  return FALSE;
} 

/*===========================================================================
FUNCTION PDCP_PS_TO_PDCP_EACH_ENQUEUE_CB

DESCRIPTION
  
PARAMETERS
   wm            : pointer to the watermark
   callback_data : Call instance registered with the watermark callback

  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  
===========================================================================*/
/*lint -e818*/
void pdcp_ps_to_pdcp_each_enqueue_cb
(
  dsm_watermark_type *wm,
 /*lint -esym(715,wm) */
 void*              callback_data
)
{
  uint8 pdcp_inst;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  pdcp_inst = (uint8) ((uint32) callback_data);
  
  /*lint -e734*/
  pdcp_set_data_available_sig( pdcp_inst, 
                               pdcp_ctl_blk[pdcp_inst].ps_to_pdcp_sig);
  /*lint +e734*/
} /* pdcp_ps_to_pdcp_each_enqueue_cb() */
/*lint +e818*/

/*===========================================================================
FUNCTION PDCP_PS_TO_PDCP_GONE_EMPTY_CB

DESCRIPTION
  
PARAMETERS
   wm            : pointer to the watermark
   callback_data : Call instance registered with the watermark callback
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  
===========================================================================*/
/*lint -e818*/
void pdcp_ps_to_pdcp_gone_empty_cb
(
 dsm_watermark_type *wm,
 /*lint -esym(715,wm) */
 void*              callback_data
)
{
  uint8 pdcp_inst;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  pdcp_inst = (uint8) ((uint32) callback_data);
  /*lint -e734*/
  pdcp_clr_data_available_sig( pdcp_inst,
                               pdcp_ctl_blk[pdcp_inst].ps_to_pdcp_sig);
  /*lint +e734*/
} /* dswpsd_ps_to_pdcp_non_empty_cb() */
/*lint +e818*/

/*===========================================================================
FUNCTION PDCP_SETUP_PS_TO_PDCP_WM

DESCRIPTION
  This function initializes the PS->PDCP watermark for PDP calls.
  
PARAMETERS
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
  
===========================================================================*/
void pdcp_setup_ps_to_pdcp_wm
(
  uint32 pdcp_inst
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((pdcp_ctl_blk[pdcp_inst].ps_to_pdcp_wm) == NULL) 
  {
	DATA_MSG0_ERROR("ps_to_pdcp_wm is NULL");
	return;
  }
  PDCP_ENTER_CRIT_SECTION ( &pdcp_crit_section );
  
  /*-------------------------------------------------------------------------
    Set up PS->PDCP watermark - used for both PDP-IP & PDP-PPP stacks
	Watermark counts are setup in the mode handler.
  -------------------------------------------------------------------------*/
  (pdcp_ctl_blk[pdcp_inst].ps_to_pdcp_wm)->non_empty_func_ptr     = NULL;
  (pdcp_ctl_blk[pdcp_inst].ps_to_pdcp_wm)->gone_empty_func_ptr    = NULL;
  (pdcp_ctl_blk[pdcp_inst].ps_to_pdcp_wm)->each_enqueue_func_ptr  = NULL;

  if (pdcp_ctl_blk[pdcp_inst].pdcp_state == PDCP_UP) 
  {
    (pdcp_ctl_blk[pdcp_inst].ps_to_pdcp_wm)->gone_empty_func_ptr    = 
      pdcp_ps_to_pdcp_gone_empty_cb;
    (pdcp_ctl_blk[pdcp_inst].ps_to_pdcp_wm)->gone_empty_func_data   = 
      (void*) pdcp_inst;
    
    (pdcp_ctl_blk[pdcp_inst].ps_to_pdcp_wm)->each_enqueue_func_ptr  = 
      pdcp_ps_to_pdcp_each_enqueue_cb;
    (pdcp_ctl_blk[pdcp_inst].ps_to_pdcp_wm)->each_enqueue_func_data = 
      (void*)pdcp_inst;
  }
  PDCP_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
} /* pdcp_setup_ps_to_pdcp_wm() */

/*===========================================================================
FUNCTION PDCP_PDCP_TO_PS_FLOW_DISABLE_CB

DESCRIPTION
  This function is called when the PDCP to PS watermark reaches the high
  watermark. We send the PDCP_CLOSE_RX_WIN primitive to PDCP so it will in turn
  close the PDCP RX window of the UE.
  
PARAMETERS 
   wm            : pointer to the watermark
   callback_data : Call instance registered with the watermark callback
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  Causes PDCP to close the UE's RX window to one PDU (for this PDCP ID)  

===========================================================================*/
/*lint -e818*/
void pdcp_pdcp_to_ps_flow_disable_cb
(
  dsm_watermark_type *wm,
  /*lint -esym(715,wm) */
  void*              callback_data
)
{
  uint8 pdcp_inst ;                 /* Index into call table for call */
  comp_cmd_type *pdcp_cmd = NULL;
 /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  pdcp_inst = (uint8) ((uint32) callback_data);

  if (FALSE == pdcp_validate_pdcp_inst(pdcp_inst))
  {
    DATA_MSG0_ERROR("pdcp_inst invalid in high_wmk_cb");
    return;
  }

  if (pdcp_ctl_blk[pdcp_inst].pdcp_wmk_hi == TRUE)
  {
    DATA_MSG0_LOW("PDCP Wmk already Hi");
    return;
  }
  
  if( (pdcp_cmd = comp_get_cmd_buf()) == NULL)
  {
    /*-----------------------------------------------------------------------
      No free command buffers available, log an error.
    -----------------------------------------------------------------------*/
    DATA_MSG0_ERROR("No items on PDCP Task free cmd q");
    return;
  }    

  DATA_MSG0_HIGH("Sending PDCP close Rx window request");
  pdcp_ctl_blk[pdcp_inst].pdcp_wmk_hi = TRUE;
  /*------------------------------------------------------------------------
   Send the command to PDCP
  -------------------------------------------------------------------------*/
  pdcp_cmd->hdr.cmd_id = PDCP_SET_FLOW_MASK;
  pdcp_cmd->hdr.subs_id = pdcp_ctl_blk[pdcp_inst].subs_id;  
  /*lint -e734 */
  pdcp_cmd->cmd.pdcp_set_flow_mask.pdcp_inst = pdcp_inst;
  /*lint +e734*/
  pdcp_cmd->cmd.pdcp_set_flow_mask.flow_dir = PDCP_TO_PS;
  pdcp_cmd->cmd.pdcp_set_flow_mask.flow_mask =  DS_FLOW_DISABLE;

  comp_put_cmd(pdcp_cmd);
      
}/* pdcp_pdcp_to_ps_flow_disable_cb() */
/*lint +e818*/


/*===========================================================================
FUNCTION PDCP_PDCP_TO_PS_FLOW_ENABLE_CB

DESCRIPTION
  This function is called when the PDCP to PS watermark reaches the low
  watermark. We send the PDCP_OPEN_RX_WIN primitive to PDCP so it will in turn
  open the PDCP RX window of the UE.
  
PARAMETERS
   wm            : pointer to the watermark
   callback_data : Call instance registered with the watermark callback
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  Causes PDCP to request the UE's RX window be opened back up for this PDCP ID
  
===========================================================================*/
/*lint -e818*/
void pdcp_pdcp_to_ps_flow_enable_cb
(
  /*lint -e818 */
  dsm_watermark_type *wm,
  /*lint -esym(715,wm) */
  void*              callback_data
)
{
  uint8 pdcp_inst ;                 /* Index into call table for call */
  comp_cmd_type *pdcp_cmd = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  pdcp_inst = (uint8) ((uint32) callback_data);

  if (FALSE == pdcp_validate_pdcp_inst(pdcp_inst))
  {
    DATA_MSG0_ERROR("pdcp_inst invalid in high_wmk_cb");
    return;
  }

  if (pdcp_ctl_blk[pdcp_inst].pdcp_wmk_hi == FALSE)
  {
    DATA_MSG0_LOW("PDCP Wmk already Lo");
    return;
  }
  
  if( (pdcp_cmd = comp_get_cmd_buf()) == NULL)
  {
    /*-----------------------------------------------------------------------
      No free command buffers available, log an error.
    -----------------------------------------------------------------------*/
    DATA_MSG0_ERROR( "No items on PDCP Task free cmd q");
    return;
  }    

  DATA_MSG0_HIGH("Sending PDCP open Rx window request");

  /*------------------------------------------------------------------------
   Send the command to PDCP
  -------------------------------------------------------------------------*/
  pdcp_ctl_blk[pdcp_inst].pdcp_wmk_hi = FALSE;
  pdcp_cmd->hdr.cmd_id = PDCP_SET_FLOW_MASK;
  pdcp_cmd->hdr.subs_id = pdcp_ctl_blk[pdcp_inst].subs_id;  
  /*lint -e734 */
  pdcp_cmd->cmd.pdcp_set_flow_mask.pdcp_inst = pdcp_inst;
  /*lint +e734 */
  pdcp_cmd->cmd.pdcp_set_flow_mask.flow_dir = PDCP_TO_PS;
  pdcp_cmd->cmd.pdcp_set_flow_mask.flow_mask =  DS_FLOW_ENABLE;

  comp_put_cmd(pdcp_cmd);
} /* pdcp_pdcp_to_ps_flow_enable_cb() */
/*lint +e818 */


/*===========================================================================
FUNCTION PDCP_SETUP_PDCP_TO_PS_WM

DESCRIPTION
  This function initializes the PDCP->PS watermark. Used for pdp-ip & pdp-ppp
  calls
  
PARAMETERS
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
  
===========================================================================*/
void pdcp_setup_pdcp_to_ps_wm
(
  uint32 pdcp_inst
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ((pdcp_ctl_blk[pdcp_inst].pdcp_to_ps_wm) == NULL) 
  {
	DATA_MSG0_ERROR("pdcp_to_ps_wm is NULL");
	return;
  }
  PDCP_ENTER_CRIT_SECTION ( &pdcp_crit_section );

  /*-------------------------------------------------------------------------
    Set up PDCP->PS watermark - used for both PDP-IP & PDP-PPP stacks
	Lo, HI and DNE counts are setup in Mode Handler.
  -------------------------------------------------------------------------*/
  (pdcp_ctl_blk[pdcp_inst].pdcp_to_ps_wm)->gone_empty_func_ptr  = NULL;
 // (pdcp_ctl_blk[pdcp_inst].pdcp_to_ps_wm)->non_empty_func_ptr   = NULL;
  (pdcp_ctl_blk[pdcp_inst].pdcp_to_ps_wm)->lowater_func_ptr     = NULL;
  (pdcp_ctl_blk[pdcp_inst].pdcp_to_ps_wm)->hiwater_func_ptr     = NULL;

  if (pdcp_ctl_blk[pdcp_inst].pdcp_state == PDCP_UP) 
  {
    (pdcp_ctl_blk[pdcp_inst].pdcp_to_ps_wm)->lowater_func_ptr     = 
      pdcp_pdcp_to_ps_flow_enable_cb;
    (pdcp_ctl_blk[pdcp_inst].pdcp_to_ps_wm)->lowater_func_data    = 
      (void*) pdcp_inst;
    
    (pdcp_ctl_blk[pdcp_inst].pdcp_to_ps_wm)->hiwater_func_ptr     = 
      pdcp_pdcp_to_ps_flow_disable_cb;
    (pdcp_ctl_blk[pdcp_inst].pdcp_to_ps_wm)->hiwater_func_data    = 
      (void*) pdcp_inst;
  }

  /*---------------------------------------------------------------------------
    Reconfigure the Watermark levels based on the advertized rate
  ---------------------------------------------------------------------------*/
  pdcp_update_dl_wm_levels(pdcp_ctl_blk[pdcp_inst].dl_data_rate, 
		           pdcp_ctl_blk[pdcp_inst].pdcp_to_ps_wm);

  PDCP_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
} /* pdcp_setup_pdcp_to_ps_wm() */


#ifdef FEATURE_QXDM_WM_TRACING
/*===========================================================================
FUNCTION  PDCP_GET_CALL_INFO_FOR_WM_TRACING

DESCRIPTION
  Get group_id and rab_id from the mode handler

PARAMETERS
  call_instance - Index into call table for the call
  wm_stat_ptr - Pointer to structure where fc stores watermark info
  wm_id       - Watermark ID for which the stats are required
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static uint32 pdcp_get_call_info_wm_tracing
(
  void *rx_func_para  
)
{
  uint32 group_id = 0;
  uint32 rab_id = 0;    
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  //Get the call_id
  ds_bearer_context_s* bearer_context_p = (ds_bearer_context_s*)rx_func_para;
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
  rab_id = (uint16)(bearer_context_p->nsapi); 
  return ((uint32)(rab_id << 16) + group_id);   
}
#endif /* FEATURE_QXDM_WM_TRACING */

/*===========================================================================
FUNCTION pdcp_REGISTER_WM_HDLR

DESCRIPTION This is the handler function for the reigister_wm command posted
            from the DS mode handler.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE - if registration succeeds
  FALSE - if parameter validation fails
  
SIDE EFFECTS
  None
===========================================================================*/
boolean  pdcp_register_wm_hdlr
(
  const comp_cmd_type *cmd_ptr
)
{
  dsm_watermark_type *tx_wm_ptr;
  dsm_watermark_type *rx_wm_ptr;
  void               (*post_rx_func_ptr)(byte, byte, void *);
  void               *rx_func_para;
  uint32              pdcp_inst;
  ps_phys_link_type  *phys_link_ptr = NULL;
  rohc_handle_type    rohc_handle;
  void               *iphc_handle;
#ifdef FEATURE_QXDM_WM_TRACING  
  uint32 call_id = 0;
#endif
  sys_modem_as_id_e_type     subs_id = ds3g_get_ps_subs_id();  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(cmd_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("cmd_ptr passed is NULL");
    return FALSE;
  }

  subs_id = cmd_ptr->hdr.subs_id;

  /*Validate subscription id*/
  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    DS_3GPP_MSG1_ERROR("Invalid Subs id(CM):%d in pdcp_register_wm_hdlr ",
                        subs_id);
    return FALSE;
  }

  if (cmd_ptr->hdr.cmd_id != PDCP_REG_WM)
  {
    DATA_MSG1_ERROR("cmd id is not REG_WM %d", cmd_ptr->hdr.cmd_id);
    ASSERT(0);
    return FALSE;
  }
  PDCP_ENTER_CRIT_SECTION ( &pdcp_crit_section );
  tx_wm_ptr = cmd_ptr->cmd.pdcp_reg_wm.ps_to_pdcp_wm_ptr;
  rx_wm_ptr = cmd_ptr->cmd.pdcp_reg_wm.pdcp_to_ps_wm_ptr;
  post_rx_func_ptr = cmd_ptr->cmd.pdcp_reg_wm.pdcp_to_ps_post_rx_func_ptr;
  rx_func_para = cmd_ptr->cmd.pdcp_reg_wm.pdcp_to_ps_rx_func_para;
  pdcp_inst = pdcp_get_pdcp_inst_given_lc_id (subs_id,
		                              cmd_ptr->cmd.pdcp_reg_wm.dl_lc_id,
                                              cmd_ptr->cmd.pdcp_reg_wm.ul_lc_id);

  /*-------------------------------------------------------------------------
  if pdcp_inst passed in does not have a valid ctl blk entry, return false.
  -------------------------------------------------------------------------*/
  if (pdcp_validate_pdcp_inst(pdcp_inst) == FALSE) 
  {
    DATA_MSG1_ERROR("pdcp inst validation failed %d", pdcp_inst);
	PDCP_BRANCH_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
    return FALSE;
  }

#ifdef FEATURE_CPU_BASED_FLOW_CONTROL
  pdcp_ctl_blk[pdcp_inst].traf_class = cmd_ptr->cmd.pdcp_reg_wm.traf_class;
#endif /* FEATURE_CPU_BASED_FLOW_CONTROL */
  /*-------------------------------------------------------------------------
  If tx or rx wm is null, don't register wm.
  -------------------------------------------------------------------------*/
  if ((tx_wm_ptr == NULL) || (rx_wm_ptr == NULL))
  {
    DATA_MSG2_ERROR("tx_wm_ptr = %d, rx_wm_ptr = %d", 
			              tx_wm_ptr, rx_wm_ptr);
	PDCP_BRANCH_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
    return FALSE;
  }

  pdcp_ctl_blk[pdcp_inst].ps_to_pdcp_wm = tx_wm_ptr;
  pdcp_ctl_blk[pdcp_inst].pdcp_to_ps_wm = rx_wm_ptr;
  pdcp_setup_ps_to_pdcp_wm(pdcp_inst);
  pdcp_setup_pdcp_to_ps_wm(pdcp_inst);

  /*-------------------------------------------------------------------------
    Set the handler for the PS to PDCP signal for the call.
	Set the handler for the RLC to PDCP signal for the call.
  -------------------------------------------------------------------------*/
  (void)comp_set_sig_handler(
    pdcp_ctl_blk[pdcp_inst].ps_to_pdcp_sig,
    pdcp_tx_sig_hdlr,
    (void *)pdcp_inst);
#ifdef FEATURE_COMP_LO_TASK
  (void)comp_lo_set_sig_handler(
	pdcp_ctl_blk[pdcp_inst].rlc_to_pdcp_sig,
	pdcp_rx_sig_hdlr,
	(void *)pdcp_inst);
  (void)comp_lo_set_sig_handler(
	pdcp_ctl_blk[pdcp_inst].ps_to_pdcp_sig,
	pdcp_tx_sig_hdlr,
	(void *)pdcp_inst);
#endif

  /*-------------------------------------------------------------------------
  if this pdcp inst is in transparent state, then register these wm with RLC.
  -------------------------------------------------------------------------*/
  if (pdcp_ctl_blk[pdcp_inst].pdcp_state == PDCP_TRANSPARENT) 
  {
    DATA_MSG1_HIGH("pdcp in transparent state, pdcp_inst: %d", pdcp_inst);
    
    /*----------------------------------------------------------------------
      In transparent case, RLC->PS WM HI/LO level should trigger Flow control 
      to NW. Set the HI/LO callbacks to RLC->PDCP HI/LO callbacks so that
      NW can be flow controlled when RLC->PS WM reaches HI/LO.
    -----------------------------------------------------------------------*/
    rx_wm_ptr->lowater_func_ptr     =  pdcp_pdcp_to_ps_flow_enable_cb;
    rx_wm_ptr->lowater_func_data    =  (void*) pdcp_inst;
    
    rx_wm_ptr->hiwater_func_ptr     =  pdcp_pdcp_to_ps_flow_disable_cb;
    rx_wm_ptr->hiwater_func_data    =  (void*) pdcp_inst;

#ifdef FEATURE_TDSCDMA
#ifdef FEATURE_SEGMENT_LOADING
#ifdef FEATURE_DUAL_WCDMA
   if( (ds_3gpp_ps_tds_tbl != NULL) && 
        (ITDSCDMA_rrc_is_tds_active_sub(ds_3gpp_ps_tds_tbl, subs_id) == TRUE) )
#else
    if( (ds_3gpp_ps_tds_tbl != NULL) && 
        (ITDSCDMA_rrc_is_tds_active(ds_3gpp_ps_tds_tbl) == TRUE) )
#endif
    
#else
#ifdef FEATURE_DUAL_WCDMA
     if(rrc_is_tds_active_sub(subs_id) == TRUE)
#else
    if(rrc_is_tds_active() == TRUE)
#endif
#endif /* FEATURE_SEGMENT_LOADING */
    {
      pdcp_register_wm_with_tdrlc (subs_id, 
                                   cmd_ptr->cmd.pdcp_reg_wm.dl_lc_id,
                                   cmd_ptr->cmd.pdcp_reg_wm.ul_lc_id,
                                   tx_wm_ptr,
                                   rx_wm_ptr,
                                   post_rx_func_ptr,
                                   (void *)rx_func_para);
    }
    else 
#endif /* FEATURE_TDSCDMA */
    {    
    pdcp_register_wm_with_rlc (subs_id,
		               cmd_ptr->cmd.pdcp_reg_wm.dl_lc_id,
                               cmd_ptr->cmd.pdcp_reg_wm.ul_lc_id,
                               tx_wm_ptr,
                               rx_wm_ptr,
                               post_rx_func_ptr,
                               (void *)rx_func_para);
    }
    if(rx_wm_ptr->current_cnt >= PDCP_RLC_DL_WM_HI)  {
      pdcp_pdcp_to_ps_flow_disable_cb(
                                     rx_wm_ptr,
                                     (void *)pdcp_inst); 
    }
  }
  else if (pdcp_ctl_blk[pdcp_inst].pdcp_state == PDCP_UP) 
  {
    DATA_MSG1_HIGH("pdcp in UP state, pdcp_inst: %d", pdcp_inst);
    /*-----------------------------------------------------------------------
    reg both tx and rx wm with PDCP, update PDCP ctl table
    -----------------------------------------------------------------------*/
    pdcp_ctl_blk[pdcp_inst].pdcp_to_ps_post_rx_func_ptr = post_rx_func_ptr;
    pdcp_ctl_blk[pdcp_inst].pdcp_to_ps_rx_func_para = rx_func_para;
    pdcp_ctl_blk[pdcp_inst].pdcp_to_rlc_flow_mask = DS_FLOW_ENABLE;
    pdcp_ctl_blk[pdcp_inst].pdcp_to_ps_flow_mask = DS_FLOW_ENABLE;

   /*-------------------------------------------------------------------------
       Setup and register PDCP's tx and rx wm with RLC.
     -------------------------------------------------------------------------*/
   pdcp_setup_pdcp_to_rlc_wm(pdcp_inst);
   pdcp_setup_rlc_to_pdcp_wm(pdcp_inst);

#ifdef FEATURE_TDSCDMA
#ifdef FEATURE_SEGMENT_LOADING
#ifdef FEATURE_DUAL_WCDMA
   if((ds_3gpp_ps_tds_tbl != NULL) && 
        ITDSCDMA_rrc_is_tds_active_sub(ds_3gpp_ps_tds_tbl, subs_id) == TRUE)
#else
    if((ds_3gpp_ps_tds_tbl != NULL) && 
        ITDSCDMA_rrc_is_tds_active(ds_3gpp_ps_tds_tbl) == TRUE)
#endif
    
#else
#ifdef FEATURE_DUAL_WCDMA
     if(rrc_is_tds_active_sub(subs_id) == TRUE)
#else
    if(rrc_is_tds_active() == TRUE)
#endif
#endif /* FEATURE_SEGMENT_LOADING  */
    {
      pdcp_register_wm_with_tdrlc (subs_id, 
                                   cmd_ptr->cmd.pdcp_reg_wm.dl_lc_id,
                                   cmd_ptr->cmd.pdcp_reg_wm.ul_lc_id,
                                  &(pdcp_ctl_blk[pdcp_inst].rlc_state.rlc_tx_wm_item),
                                  &(pdcp_ctl_blk[pdcp_inst].rlc_state.rlc_rx_wm_item),
                                  pdcp_from_rlc_post_rx_func_ptr,
                                  (void *)pdcp_inst);  
    }
    else 
#endif /* FEATURE_TDSCDMA */
    {
      pdcp_register_wm_with_rlc (subs_id,
		                 cmd_ptr->cmd.pdcp_reg_wm.dl_lc_id,
                                 cmd_ptr->cmd.pdcp_reg_wm.ul_lc_id,
                                 &(pdcp_ctl_blk[pdcp_inst].rlc_state.rlc_tx_wm_item),
                                 &(pdcp_ctl_blk[pdcp_inst].rlc_state.rlc_rx_wm_item),
                                 pdcp_from_rlc_post_rx_func_ptr,
                                 (void *)pdcp_inst);  
    }
#ifdef FEATURE_QXDM_WM_TRACING
    //Get the call_id    
    call_id = pdcp_get_call_info_wm_tracing(rx_func_para);
 
    //Register PDCP client with FC
    DATA_MSG1_MED(("PDCP Client: Registering with FC, call_id %d", 
                    call_id);
    fc_post_reg_cmd_wm_trace(UNKNOWN_TECH, //tech id
                          call_id,
                          PDCP_CLIENT,
                          pdcp_report_wm_stats_cb, 
                          pdcp_inst);    
#endif
    if(rx_wm_ptr->current_cnt >= PDCP_RLC_DL_WM_HI)
    {
      pdcp_pdcp_to_ps_flow_disable_cb(
                                      rx_wm_ptr,
                                      (void *)pdcp_inst
                                     ); 
    }
  }
  else
  {
    DATA_MSG0_ERROR("WM reg while PDCP in Down state");
	PDCP_BRANCH_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
    return FALSE;
  }
  /*-------------------------------------------------------------------------
    Setting up ROHC logging if ROHC is being used
  -------------------------------------------------------------------------*/

  rohc_handle = pdcp_ctl_blk[pdcp_inst].rohc_hdl;

  if (rohc_handle != NULL) 
  {
    
    if(ds_bearer_cntx_validate_bearer_context((ds_bearer_context_s*)rx_func_para))
    {
    phys_link_ptr = &(((ds_bearer_context_s*)rx_func_para)->phys_link);
    }
    if(rohc_set_logging(rohc_handle,&(phys_link_ptr->dpl_link_cb)) == -1)
    {
      DATA_MSG0_ERROR("Unable to register phys_link with ROHC handle");
	  PDCP_BRANCH_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
      return FALSE;
    }
  }

  /*-------------------------------------------------------------------------
    Setting up IPHC logging if IPHC is being used
  -------------------------------------------------------------------------*/

  iphc_handle = pdcp_ctl_blk[pdcp_inst].iphc_hdl;

  if (iphc_handle != NULL) 
  {

    if(ds_bearer_cntx_validate_bearer_context((ds_bearer_context_s*)rx_func_para))
    {
    phys_link_ptr = &(((ds_bearer_context_s*)rx_func_para)
                      ->phys_link);
    }

    if(iphc_set_logging(iphc_handle,&(phys_link_ptr->dpl_link_cb)) == -1)
    {
      DATA_MSG0_ERROR("Unable to register phys_link with IPHC handle");
	  PDCP_BRANCH_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
      return FALSE;
    }
  }

  /*-------------------------------------------------------------------------
    Set PDCP Tx signal just in case the watermark already contains data 
    ready for transmission
  -------------------------------------------------------------------------*/
  pdcp_set_data_available_sig( (uint8)pdcp_inst, 
                               pdcp_ctl_blk[pdcp_inst].ps_to_pdcp_sig);

  PDCP_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
  return TRUE;

} /* pdcp_register_wm */


/*===========================================================================
FUNCTION PDCP_DEREGISTER_WM_HDLR

DESCRIPTION
  This function deregisters the WMs between PS & PDCP.

DEPENDENCIES
  None

RETURN VALUE
  TRUE - if parameter validation passes
  FALSE - if parameter validation fails
  
SIDE EFFECTS
  Can result in WM dereg with RLC if PDCP was initially in transparent state.
===========================================================================*/
void pdcp_deregister_wm_hdlr
( 
  const comp_cmd_type *cmd_ptr
)
{
  uint8 pdcp_inst = 0;
#ifdef FEATURE_QXDM_WM_TRACING
  uint32 call_id = 0;
  uint32 call_instance = 0;
#endif
  sys_modem_as_id_e_type     subs_id = ds3g_get_ps_subs_id();
  dsm_watermark_type        *tx_wm_ptr = NULL;
  dsm_watermark_type        *rx_wm_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(cmd_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("cmd_ptr passed is NULL");
    return;
  }
  tx_wm_ptr = cmd_ptr->cmd.pdcp_dereg_wm.ps_to_pdcp_wm_ptr;
  rx_wm_ptr = cmd_ptr->cmd.pdcp_dereg_wm.pdcp_to_ps_wm_ptr;
  subs_id = cmd_ptr->hdr.subs_id;
  /*Validate subscription id*/
  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    DS_3GPP_MSG1_ERROR("Invalid Subs id(CM):%d in pdcp_register_wm_hdlr "
                       ,subs_id);
    return ;
  }
  
  if (cmd_ptr->hdr.cmd_id != PDCP_DEREG_WM)
  {
    DATA_MSG1_ERROR("cmd id is not REG_WM %d", cmd_ptr->hdr.cmd_id);
    ASSERT(0);
    return;
  }

  PDCP_ENTER_CRIT_SECTION ( &pdcp_crit_section );
  for ( pdcp_inst = 0; pdcp_inst < MAX_PDCP_INST; pdcp_inst++ ) 
  {
    
  /*-------------------------------------------------------------------------
    PDCP can be in down state when pdcp receives this, hence this check
    There is no need to deregister with RLC when PDCP is in transparent
    state because RLC channels are released by the time we get here.  
  -------------------------------------------------------------------------*/
    if ((pdcp_ctl_blk[pdcp_inst].ps_to_pdcp_wm == tx_wm_ptr) &&
        (pdcp_ctl_blk[pdcp_inst].pdcp_to_ps_wm == rx_wm_ptr) &&
        (pdcp_ctl_blk[pdcp_inst].subs_id == subs_id))
    {
      if (pdcp_ctl_blk[pdcp_inst].pdcp_state == PDCP_UP)
      {
#ifdef FEATURE_QXDM_WM_TRACING
       /*-------------------------------------------------------------------------
        Deregister the watermarks from FC.
       -------------------------------------------------------------------------*/
       call_instance = (uint32)pdcp_ctl_blk[pdcp_inst].pdcp_to_ps_rx_func_para;
       DATA_MSG1_MED("Getting call instance from pdcp_inst. Call instance is: %d",
                      call_instance);
       //Get the call_id
       call_id = pdcp_get_call_info_wm_tracing(
                              pdcp_ctl_blk[pdcp_inst].pdcp_to_ps_rx_func_para);

       DATA_MSG1_HIGH("PDCP client: Deregistering from FC for call id: %d",
                       call_id);

       fc_post_deregister_cmd_wm_trace(call_id, PDCP_CLIENT);
#endif
      }
      /*-------------------------------------------------------------------------
      reset both tx and rx wm to NULL.
      -------------------------------------------------------------------------*/
#ifdef FEATURE_TDSCDMA
#ifdef FEATURE_SEGMENT_LOADING
#ifdef FEATURE_DUAL_WCDMA
   if(ds_3gpp_ps_tds_tbl != NULL && 
         ITDSCDMA_rrc_is_tds_active_sub(ds_3gpp_ps_tds_tbl, subs_id) == TRUE)
#else
      if(ds_3gpp_ps_tds_tbl != NULL && 
         ITDSCDMA_rrc_is_tds_active(ds_3gpp_ps_tds_tbl) == TRUE)
#endif
      
#else
#ifdef FEATURE_DUAL_WCDMA
     if(rrc_is_tds_active_sub(subs_id) == TRUE)
#else
      if(rrc_is_tds_active() == TRUE)
#endif
#endif /* FEATURE_SEGMENT_LOADING */
      {
        pdcp_deregister_wm_with_tdrlc ( pdcp_inst ); 
      }
      else 
#endif /* FEATURE_TDSCDMA */ 
      {
	  pdcp_deregister_wm_with_rlc ( pdcp_inst );
      }
      pdcp_ctl_blk[pdcp_inst].ps_to_pdcp_wm = NULL;
      pdcp_ctl_blk[pdcp_inst].pdcp_to_ps_wm = NULL;
    }
  }
  PDCP_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
} /* pdcp_dereigster_wm */


/*===========================================================================
FUNCTION PDCP_SET_FLOW_MASK_HDLR

DESCRIPTION
  This function handles the set flow mask command posted by the mode 
  handler / RLC.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  Modifies the tx/rx flow mask to the value specified by mode handler / RLC.
===========================================================================*/
void pdcp_set_flow_mask_hdlr
( 
  const comp_cmd_type *cmd_ptr
)
{
  uint32                     pdcp_inst = 0;
  pdcp_flow_dir_e_type       flow_dir = PDCP_TO_PS;
  ds3g_flow_e_type           flow_mask = DS_FLOW_ENABLE;
  sys_modem_as_id_e_type     subs_id = ds3g_get_ps_subs_id();
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(cmd_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("cmd_ptr passed is NULL");
    return;
  }
  pdcp_inst = cmd_ptr->cmd.pdcp_set_flow_mask.pdcp_inst;
  flow_dir = cmd_ptr->cmd.pdcp_set_flow_mask.flow_dir;
  flow_mask = cmd_ptr->cmd.pdcp_set_flow_mask.flow_mask;
  
  subs_id = cmd_ptr->hdr.subs_id;
 
  MSG_HIGH("pdcp set flow mask, pdcp_inst: %d, flow_dir: %d, flow_mask: %d", 
             pdcp_inst, flow_dir, flow_mask);

  if (cmd_ptr->hdr.cmd_id != PDCP_SET_FLOW_MASK) 
  {
    DATA_MSG1_ERROR("cmd id is not SET_FLOW_MASK %d",
                     cmd_ptr->hdr.cmd_id);
    ASSERT(0);
    return;
  }

  if (subs_id != pdcp_ctl_blk[pdcp_inst].subs_id) 
  {
    DATA_MSG2_ERROR("CMD Subs ID %d but  pdcp_inst subs_id is %d", 
                     subs_id, pdcp_ctl_blk[pdcp_inst].subs_id);
    return;
  }  

  PDCP_ENTER_CRIT_SECTION ( &pdcp_crit_section );
  if (pdcp_ctl_blk[pdcp_inst].pdcp_state == PDCP_UP) 
  {
    /*-------------------------------------------------------------------------
    Based on the flow direction specified, modify the associated flow mask
    -------------------------------------------------------------------------*/
    if (flow_dir == PDCP_TO_RLC) 
    {
      pdcp_ctl_blk[pdcp_inst].pdcp_to_rlc_flow_mask = flow_mask;
      if (flow_mask == DS_FLOW_ENABLE) 
      {
        pdcp_set_data_available_sig( (uint8)pdcp_inst,
                                     pdcp_ctl_blk[pdcp_inst].ps_to_pdcp_sig);
      }
      else if (flow_mask == DS_FLOW_DISABLE) 
      {
        pdcp_clr_data_available_sig( (uint8)pdcp_inst,
                                     pdcp_ctl_blk[pdcp_inst].ps_to_pdcp_sig);
      }
    }
    else if (flow_dir == PDCP_TO_PS) 
    {
      pdcp_ctl_blk[pdcp_inst].pdcp_to_ps_flow_mask = flow_mask;
      if (flow_mask == DS_FLOW_ENABLE) 
      {
        pdcp_set_data_available_sig( (uint8)pdcp_inst,
                                     pdcp_ctl_blk[pdcp_inst].rlc_to_pdcp_sig);
      }
      else if (flow_mask == DS_FLOW_DISABLE) 
      {
        pdcp_clr_data_available_sig( (uint8)pdcp_inst,
                                     pdcp_ctl_blk[pdcp_inst].rlc_to_pdcp_sig);
      }
    }
    else
    {
      DATA_MSG0_ERROR("Invalid flow dir");
    }
  }
  
  /*-------------------------------------------------------------------------
  On the DL, when mode handler wants to flow control PDCP_TO_PS, PDCP has to
  communicate this to RLC since the mode handler has no knowledge of PDCP 
  state.
  -------------------------------------------------------------------------*/
  else if (pdcp_ctl_blk[pdcp_inst].pdcp_state == PDCP_TRANSPARENT) 
  {
    if (flow_dir == PDCP_TO_PS) 
    {
      if (flow_mask == DS_FLOW_ENABLE) 
      {
#ifdef FEATURE_TDSCDMA
#ifdef FEATURE_SEGMENT_LOADING
#ifdef FEATURE_DUAL_WCDMA
       if(ds_3gpp_ps_tds_tbl != NULL && 
           ITDSCDMA_rrc_is_tds_active_sub(ds_3gpp_ps_tds_tbl, subs_id) == TRUE)
#else
        if(ds_3gpp_ps_tds_tbl != NULL && 
           ITDSCDMA_rrc_is_tds_active(ds_3gpp_ps_tds_tbl) == TRUE)
#endif
        
#else
#ifdef FEATURE_DUAL_WCDMA
         if(rrc_is_tds_active_sub(subs_id) == TRUE)
#else
        if(rrc_is_tds_active() == TRUE)
#endif
#endif /* FEATURE_SEGMENT_LOADING */
        {
          pdcp_tdrlc_to_pdcp_lo_water_cb(
            &(pdcp_ctl_blk[pdcp_inst].rlc_state.rlc_rx_wm_item),
            (void *)pdcp_inst);
        }
        else
#endif /* FEATURE_TDSCDMA */
        {
          pdcp_rlc_to_pdcp_lo_water_cb(
            &(pdcp_ctl_blk[pdcp_inst].rlc_state.rlc_rx_wm_item),
            (void *)pdcp_inst);
        }
      }
      else if (flow_mask == DS_FLOW_DISABLE) 
      {
#ifdef FEATURE_TDSCDMA
#ifdef FEATURE_SEGMENT_LOADING
#ifdef FEATURE_DUAL_WCDMA
       if(ds_3gpp_ps_tds_tbl != NULL && 
            ITDSCDMA_rrc_is_tds_active_sub(ds_3gpp_ps_tds_tbl, subs_id) == TRUE)
#else
        if(ds_3gpp_ps_tds_tbl != NULL && 
            ITDSCDMA_rrc_is_tds_active(ds_3gpp_ps_tds_tbl) == TRUE)
#endif
        
#else
#ifdef FEATURE_DUAL_WCDMA
         if(rrc_is_tds_active_sub(subs_id) == TRUE)
#else
        if(rrc_is_tds_active() == TRUE)
#endif
#endif /* FEATURE_SEGMENT_LOADING */
        {
          pdcp_rlc_to_pdcp_hi_water_cb(
            &(pdcp_ctl_blk[pdcp_inst].rlc_state.rlc_rx_wm_item),
            (void *)pdcp_inst);
        }
        else
#endif /* FEATURE_TDSCDMA */
        {
          pdcp_rlc_to_pdcp_hi_water_cb(
            &(pdcp_ctl_blk[pdcp_inst].rlc_state.rlc_rx_wm_item),
            (void *)pdcp_inst);
        }
      }
      else
      {
        DATA_MSG1_ERROR("Invalid flow mask %d", flow_mask);
      }
    }
  }
  else
  {
    DATA_MSG0_LOW("PDCP in DOWN state while receiving SET_FLOW_MASK CMD");
  }
  PDCP_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
} /* pdcp_set_flow_mask */

/*===========================================================================
FUNCTION PDCP_ROHC_COMP

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
boolean pdcp_rohc_comp
( 
  uint8            pdcp_inst,
  dsm_item_type  **item_ptr,
  uint16          *cid
)
{    
  errno_enum_type               err_num = E_FAILURE;
  
  DATA_MDM_ARG_NOT_CONST(cid);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /*-----------------------------------------------------------------
     Compress dequeued data using ROHC.
    -----------------------------------------------------------------*/
    if (rohc_compress (pdcp_ctl_blk[pdcp_inst].rohc_hdl,
                       NULL,
                       NULL,
                       item_ptr,
                       &err_num) == -1)
    {
      DATA_MSG1_ERROR("Compression failed errno=%d", err_num);
      dsm_free_packet(item_ptr);
      return FALSE;
    }
  return TRUE;
}
/*===========================================================================
FUNCTION PDCP_TX_DATA_HDLR

DESCRIPTION
  This is the function that handles data on the transmit path, compresses
  data on PDCP WM and enqueues compressed data onto the RLC WM
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
boolean pdcp_tx_data_hdlr
( 
  uint8 pdcp_inst
)
{
  dsm_item_type                *item_ptr;  
  errno_enum_type               err_num = E_FAILURE;
  dsm_watermark_type           *ps_to_pdcp_wm;
  uint8                         pid = MAX_PID_ENTRIES;
  uint8                         pdcp_hdr = 0;
  iphc_packet_type_e_type       iphc_pkt_type = IPHC_PACKET_TYPE_NORMAL_IPV4;
  uint16                        cid = ROHC_INVALID_CID;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*---------------------------------------------------------------------
    Check to make sure PDCP instance value falls in the right range.
  ---------------------------------------------------------------------*/
  if (pdcp_inst >= MAX_PDCP_INST)
  {
    DATA_MSG1_ERROR("pdcp_inst > max_pdcp_inst %d", pdcp_inst);
    return TRUE;
  }
  
  PDCP_ENTER_CRIT_SECTION ( &pdcp_crit_section );
  /*---------------------------------------------------------------------
    Make sure this pdcp instance has a valid pdcp control block entry.
  ---------------------------------------------------------------------*/
  if (pdcp_validate_pdcp_inst( pdcp_inst ) == TRUE) 
  {
    /*---------------------------------------------------------------------
    Get the PS to PDCP WM based on pdcp instance.
    ---------------------------------------------------------------------*/
    ps_to_pdcp_wm = pdcp_ctl_blk[pdcp_inst].ps_to_pdcp_wm;
    if (ps_to_pdcp_wm == NULL) 
    {
      DATA_MSG0_ERROR("ps to pdcp wm null");
	  PDCP_BRANCH_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
      return TRUE;
    }
  
    /*---------------------------------------------------------------------
    Check if tx flow mask for this pdcp_inst is enabled.
    ---------------------------------------------------------------------*/
    if (pdcp_ctl_blk[pdcp_inst].pdcp_to_rlc_flow_mask == DS_FLOW_ENABLE) 
    {
      /*-------------------------------------------------------------------
      Dequeue data from pdcp WM.
      -------------------------------------------------------------------*/
      item_ptr = (dsm_item_type *)dsm_dequeue(ps_to_pdcp_wm);
      
      if (item_ptr != NULL) 
      {
        if ((pdcp_ctl_blk[pdcp_inst].rohc_hdl == NULL) &&
            (pdcp_ctl_blk[pdcp_inst].iphc_hdl == NULL))
        {
          pid = 0;
        }
        else
        {
          if ( pdcp_ctl_blk[pdcp_inst].rohc_hdl != NULL )
          {
            if (pdcp_rohc_comp(pdcp_inst,
                               &item_ptr,
                               &cid) != FALSE)
            {
                pid = pdcp_ctl_blk[pdcp_inst].ul_hash_table[ROHC_PKT_FORMAT];
              }
            else
            {
			  PDCP_BRANCH_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
              return TRUE;
            }
          }
          if ( pdcp_ctl_blk[pdcp_inst].iphc_hdl != NULL )
          {
            /*-----------------------------------------------------------------
            Compress dequeued data using IPHC.
            -----------------------------------------------------------------*/
            if (iphc_compress (pdcp_ctl_blk[pdcp_inst].iphc_hdl,
                               &item_ptr,
                               &iphc_pkt_type) == -1)
            {
              DATA_MSG1_ERROR("Compression failed errno=%d", err_num);
              dsm_free_packet(&item_ptr);
			  PDCP_BRANCH_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
              return TRUE;
            }
            /*lint -e641*/
            /*-----------------------------------------------------------------
              This means it's an uncompressed packet.
            -----------------------------------------------------------------*/
            if ((iphc_pkt_type == IPHC_PACKET_TYPE_NORMAL_IPV4) ||
                (iphc_pkt_type == IPHC_PACKET_TYPE_NORMAL_IPV6))
            {
              pid = 0;
            }
            /*-----------------------------------------------------------------
              Extract the pid value from the UL PID mapping table based on the
              packet type returned by the compressor.
            -----------------------------------------------------------------*/
            else if ((iphc_pkt_type >= IPHC_FULL_HEADER) &&
                     (iphc_pkt_type <= IPHC_CONTEXT_STATE)) 
            {
              /*lint -e64*/
              pid = pdcp_ctl_blk[pdcp_inst].ul_hash_table[iphc_pkt_type];
              /*lint +e64*/
            }
            /*-----------------------------------------------------------------
              These iphc pkt types are not supported in PDCP, these are treated
              as full header packets.
            -----------------------------------------------------------------*/
            else 
            {
              DATA_MSG1_ERROR("iphc_compress returned invalid pkt type %d",
                               iphc_pkt_type);
              dsm_free_packet(&item_ptr);
			  PDCP_BRANCH_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
              ASSERT(0);
              return TRUE;
            }
            /*lint +e641*/
          }
        }
        /*-----------------------------------------------------------------
          This means pkt type returned from compressor is not found in
          pdcp pid mapping table.  Free the packet and return.
        -----------------------------------------------------------------*/
        if (pid == MAX_PID_ENTRIES) 
        {
          DATA_MSG1_ERROR("invalid pid returned %d", pid);
          dsm_free_packet(&item_ptr);
		  PDCP_BRANCH_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
          return TRUE;
        }

        if (pdcp_ctl_blk[pdcp_inst].pdcp_info.pdcp_pdu_hdr == PRESENT) 
        {
          /*-----------------------------------------------------------------
            Append the PDU type (3 bits) to pdcp_hdr.
          -----------------------------------------------------------------*/
          /*lint -e641*/
          pdcp_hdr |= (PDCP_DATA_PDU << NUM_PDCP_PID_BITS);
          /*lint +e641*/
          
          /*-----------------------------------------------------------------
            Append pid to pdcp hdr.
          -----------------------------------------------------------------*/
          pdcp_hdr |= pid;

          /*-----------------------------------------------------------------
            pdcp hdr is now pushed down onto the original item_ptr.
          -----------------------------------------------------------------*/
          if (dsm_pushdown(&item_ptr,
                           &pdcp_hdr,
                           PDCP_HDR_LEN,
                           DSM_DS_SMALL_ITEM_POOL) != (PDCP_HDR_LEN))
          {
            DATA_MSG0_ERROR("pushdown failed");
            dsm_free_packet(&item_ptr);
			PDCP_BRANCH_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
            return TRUE;
          }
        }
        /*-----------------------------------------------------------------
        Enqueue compressed data onto the RLC TX WM.
        -----------------------------------------------------------------*/
        dsm_enqueue(&(pdcp_ctl_blk[pdcp_inst].rlc_state.rlc_tx_wm_item),
                    &item_ptr);
        DATA_MSG2_HIGH("pdcp_tx_data_hdlr: enquing data with pdcp_hdr %d pdcp_inst %d",
                        pdcp_hdr, pdcp_inst);
		PDCP_BRANCH_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
        return FALSE;
      }
    }
  }
  else
  {
    DATA_MSG1_ERROR("pdcp_inst validation failed %d", pdcp_inst);
  }
  PDCP_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
  return TRUE;
} /* pdcp_tx_data_hdlr */

/*===========================================================================
FUNCTION PDCP_ENQUEUE_FB_PKT

DESCRIPTION  This function enqueues the feedback packet generated by the
             decompressor.  Fb_pkt should not be compressed.  pdcp_hdr is
             appended and sent to the PDCP_TO_RLC_WM.
  
PARAMETERS   
fb_pkt - feedback packet to be sent to compressor
pid - PID value to be attached as part of pdcp header
pdcp_inst - index into the pdcp control block
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None
     
SIDE EFFECTS 
  None.
  
===========================================================================*/
void pdcp_enqueue_fb_pkt
(
  dsm_item_type        **fb_pkt,
  uint8                  pid,
  uint8                  pdcp_inst
)
{
  uint8                pdcp_hdr = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (pid == MAX_PID_ENTRIES) 
  {
    DATA_MSG1_ERROR("invalid pid for fb_pkt %d", pid);
    dsm_free_packet(fb_pkt);
    return;    
  }
  
  /*-----------------------------------------------------------------
    Append the PDU type (3 bits) to pdcp_hdr.
  -----------------------------------------------------------------*/
  /*lint -e641*/
  pdcp_hdr |= (PDCP_DATA_PDU << NUM_PDCP_PID_BITS);
  /*lint +e641*/
  /*-----------------------------------------------------------------
    Append PID (5 bits) to pdcp_hdr.
  -----------------------------------------------------------------*/
  pdcp_hdr |= pid;
    
  /*-----------------------------------------------------------------
    Append pdcp_hdr to fb_pkt
  -----------------------------------------------------------------*/
  if (dsm_pushdown(fb_pkt,
                   &pdcp_hdr,
                   PDCP_HDR_LEN,
                   DSM_DS_SMALL_ITEM_POOL) != (PDCP_HDR_LEN))
  {
    DATA_MSG0_ERROR("pushdown failed");
    dsm_free_packet(fb_pkt);
  }
  dsm_enqueue(&(pdcp_ctl_blk[pdcp_inst].rlc_state.rlc_tx_wm_item),
              fb_pkt);
} /* pdcp_enqueue_fb_pkt */

/*===========================================================================
FUNCTION PDCP_ROHC_DECOMP

DESCRIPTION  
  
PARAMETERS   
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None
     
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean pdcp_rohc_decomp
(
  dsm_item_type        **item_ptr,
  uint16                 cid,
  uint8                  pdcp_inst
)
{
  boolean               decomp_failed = FALSE;
  dsm_item_type        *rohc_fb_pkt = NULL;
  errno_enum_type       err_num = E_FAILURE;
  ip_version_enum_type  decomp_hdr = IP_V4;
  uint8                 pid = MAX_PID_ENTRIES;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (cid == ROHC_INVALID_CID) 
  {
    /*-----------------------------------------------------------------
    Decompress packet using ROHC.
    -----------------------------------------------------------------*/
    if (rohc_decompress (pdcp_ctl_blk[pdcp_inst].rohc_hdl,
                         NULL,
                         NULL,
                         item_ptr,
                         &decomp_hdr,
                         &rohc_fb_pkt,
						 0,
                         &err_num) == -1)
    {
      DATA_MSG1_ERROR("Decompression failed errno=%d", err_num);
      dsm_free_packet(item_ptr);
      decomp_failed = TRUE;
    }
    if (rohc_fb_pkt != NULL) 
    {
      pid = pdcp_ctl_blk[pdcp_inst].ul_hash_table[ROHC_PKT_FORMAT];
      pdcp_enqueue_fb_pkt(&rohc_fb_pkt,
                          pid, pdcp_inst);
    }
  }
  else
  {
    /*-----------------------------------------------------------------
    Decompress packet using ROHC.
    -----------------------------------------------------------------*/
    if (rohc_decompress (pdcp_ctl_blk[pdcp_inst].rohc_hdl,
                         NULL,
                         &cid,
                         item_ptr,
                         &decomp_hdr,
                         &rohc_fb_pkt,
						 0,
                         &err_num) == -1)
    {
      DATA_MSG1_ERROR("Decompression failed errno=%d", err_num);
      dsm_free_packet(item_ptr);
      decomp_failed = TRUE;
    }
    /* There is no way to know the cid to be attached to fb_pkt
    for now, this is still TBD */
    if (rohc_fb_pkt != NULL) 
    {
      pid = pdcp_ctl_blk[pdcp_inst].ul_hash_table[cid];
      pdcp_enqueue_fb_pkt(&rohc_fb_pkt,
                          pid, pdcp_inst);
    }
  }
  return decomp_failed;
} 

/*===========================================================================
FUNCTION PDCP_RX_DATA_HDLR

DESCRIPTION
  This function forwards pkts received on Um interface to ps_iface for 
  pdp-ip call.
  
PARAMETERS   
  pdcp_inst - index into the pdcp control block
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None
     
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean pdcp_rx_data_hdlr
(
  uint8 pdcp_inst
)
{
  dsm_watermark_type   *wm_ptr;
  dsm_item_type        *item_ptr;
  pdcp_pdu_e_type      pdu_type;
  uint16               rx_sn = 0;
  byte                 i = 0, j = 0;
  dsm_item_type        *iphc_fb_pkt = NULL;
  errno_enum_type      err_num = E_FAILURE;
  pkt_e_type           pkt_type = NO_HEADER_COMPRESSION;
  boolean              decomp_failed = FALSE;
  uint8                hdr_byte = 0;
  uint8                pid_index;
  uint8                pid = MAX_PID_ENTRIES;
  uint16               cid = ROHC_INVALID_CID;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------
    Check to make sure PDCP instance value falls in the right range.
  ---------------------------------------------------------------------*/
  if (pdcp_inst >= MAX_PDCP_INST) 
  {
    return TRUE;
  }
  
  PDCP_ENTER_CRIT_SECTION ( &pdcp_crit_section );
  if (pdcp_validate_pdcp_inst( pdcp_inst ) == TRUE) 
  {
    wm_ptr = &(pdcp_ctl_blk[pdcp_inst].rlc_state.rlc_rx_wm_item);
    
    /*---------------------------------------------------------------------
      Dequeue from RLC_PDCP_WM only if the flow mask is enabled.
    ----------------------------------------------------------------------*/
    if (pdcp_ctl_blk[pdcp_inst].pdcp_to_ps_flow_mask == DS_FLOW_ENABLE) 
    {
      item_ptr = (dsm_item_type *)dsm_dequeue(wm_ptr);
      if (item_ptr != NULL) 
      {
        /*---------------------------------------------------------------------
          If pdcp hdr is present, extract the header before passing the packet
          to the decompressor.
        ----------------------------------------------------------------------*/
        if (pdcp_ctl_blk[pdcp_inst].pdcp_info.pdcp_pdu_hdr == PRESENT) 
        {
          if (dsm_pullup(&item_ptr,&hdr_byte, PDCP_HDR_LEN) != 
              PDCP_HDR_LEN)
          {
            DATA_MSG0_ERROR("Could not extract pdcp header");
            dsm_free_packet(&item_ptr);
            PDCP_BRANCH_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
            return TRUE;
          }
          /*-----------------------------------------------------------------
          Extract the PDU type (first 3 bits) from the PDU.
          -----------------------------------------------------------------*/
          pdu_type = (pdcp_pdu_e_type)((hdr_byte & 0xE0) >> NUM_PDCP_PID_BITS);
          if (pdu_type == PDCP_DATA_PDU)
          {
            pdcp_ctl_blk[pdcp_inst].next_rx_sn++;
          }
          else if (pdu_type == PDCP_SEQ_NUM_PDU) 
          {
            /*-------------------------------------------------------------
              Extract the Sequence Number from the PDU. This is expected 
              only when SN sync procedure is triggered because of RLC PDU 
              size change.
            -------------------------------------------------------------*/
            if ((pdcp_ctl_blk[pdcp_inst].pdcp_info.support_for_lossless == FALSE) ||
                (dsm_pullup(&item_ptr, &rx_sn, PDCP_SEQ_NUM_LEN) != 
                 PDCP_SEQ_NUM_LEN))
            {
              DATA_MSG1_ERROR("Discarding SN PDU, support_for_lossless = %d",
                     pdcp_ctl_blk[pdcp_inst].pdcp_info.support_for_lossless);
              dsm_free_packet(&item_ptr);
              PDCP_BRANCH_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
              return TRUE;
            }
            pdcp_ctl_blk[pdcp_inst].next_rx_sn = ps_ntohs(rx_sn);
          }
          else
          {
            DATA_MSG1_ERROR("Discarding packet, invalid pdu_type = %d", pdu_type);
            dsm_free_packet(&item_ptr);
            PDCP_BRANCH_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
            return TRUE;
          }
          /*-----------------------------------------------------------------
            Extract the PID value (the last 5 bits in hdr_byte) from the PDU.
          -----------------------------------------------------------------*/
          pid_index = hdr_byte & 0x1F;
          pkt_type = pdcp_ctl_blk[pdcp_inst].dl_pid_map_table[pid_index].pkt_type;
          
          DATA_MSG2_HIGH("pdcp_rx_data_hdlr with pkt_type %d pdcp_inst %d", 
                          pkt_type, pdcp_inst);
          if (pkt_type >= INVALID_PKT_TYPE) 
          {
            DATA_MSG0_ERROR("Invalid pkt type");
            dsm_free_packet(&item_ptr);
            PDCP_BRANCH_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
            return TRUE;
          }
        }
        /*-----------------------------------------------------------------
          If pdcp hdr is not present, increment rx seq num and move on.  
        -----------------------------------------------------------------*/
        else
        {
          pdcp_ctl_blk[pdcp_inst].next_rx_sn++;
        }
        
        /*-----------------------------------------------------------------
          If pkt type suggested using ROHC or ROHC is cfg but PDCP header is
          not present, either way decomp pkt using ROHC.
        -----------------------------------------------------------------*/
        if (pdcp_ctl_blk[pdcp_inst].rohc_hdl != NULL)
        {
          decomp_failed = pdcp_rohc_decomp (&item_ptr,
                                            cid,
                                            pdcp_inst);
        }
        /*-----------------------------------------------------------------
          If pkt type suggested using IPHC or IPHC is cfg but PDCP header
          is not present, either way decomp pkt using IPHC.
        -----------------------------------------------------------------*/
        if (( pdcp_ctl_blk[pdcp_inst].iphc_hdl != NULL ) &&
             ( pkt_type != NO_HEADER_COMPRESSION ) )
        {
          /*-----------------------------------------------------------------
            Decompress packet using IPHC.
          -----------------------------------------------------------------*/
          /*lint -e50 */
          if (iphc_decompress ( pdcp_ctl_blk[pdcp_inst].iphc_hdl,
                                &item_ptr,
								(iphc_packet_type_e_type *)&pkt_type,
								&iphc_fb_pkt )== -1)
          {
            DATA_MSG1_ERROR("Decompression failed errno=%d", err_num);
            dsm_free_packet(&item_ptr);
            decomp_failed = TRUE;
          }
          /*lint +e50 */
          /*-----------------------------------------------------------------
          Feedback packet may be generated by the decompressor.  FB pkt
          shouldn't be compressed, enqueue it directly onto the RLC tx WM.
          -----------------------------------------------------------------*/
          if (iphc_fb_pkt != NULL) 
          {
            pid = pdcp_ctl_blk[pdcp_inst].ul_hash_table[IPHC_PACKET_TYPE_CONTEXT_STATE];
            pdcp_enqueue_fb_pkt (&iphc_fb_pkt,
                                 pid, pdcp_inst);
          }
        }
        /*-----------------------------------------------------------------
          If decompression succeeded, enqueue item onto PS WM and invoke
          the mode handler's RX FUNC PTR.
        -----------------------------------------------------------------*/
        if ( (decomp_failed == FALSE) && (item_ptr != NULL) &&
			          (pdcp_ctl_blk[pdcp_inst].pdcp_to_ps_wm != NULL) )
        {
          dsm_enqueue(pdcp_ctl_blk[pdcp_inst].pdcp_to_ps_wm,
                      &item_ptr);
          pdcp_ctl_blk[pdcp_inst].pdcp_to_ps_post_rx_func_ptr (i,
                                                    j,
                                                    pdcp_ctl_blk[pdcp_inst].pdcp_to_ps_rx_func_para);
          PDCP_BRANCH_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
          return FALSE;
        }
      }
      else /* item_ptr == NULL */
      {
        PDCP_BRANCH_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
        return TRUE;
      }
    }
    else
    {
      DATA_MSG0_HIGH("RLC to PDCP flow disabled");
    }
  }
  PDCP_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
  return TRUE;
} /* pdcp_rx_data_hdlr */

#ifdef FEATURE_QXDM_WM_TRACING
/*===========================================================================
FUNCTION  PDCP_REPORT_WM_STATS

DESCRIPTION
  Reports the watermark statistics by collecting relevant information from DSM

PARAMETERS
  pdcp_inst - pdcp control block instance
  rx_func_para - unique call_inst/bearer indentifier
  call_instance - Index into call table for the call
  wm_stat_ptr - Pointer to structure where fc stores watermark info
  wm_id       - Watermark ID for which the stats are required
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void pdcp_report_wm_stats
(
  uint32 pdcp_inst,
  void   *rx_func_para,
  fc_wm_stat_type *wm_stat_ptr,
  fc_wm_id_enum_type wm_id
)
{
  uint32 call_id = 0;

  /* Index to wm_info array provided by FC */
  int index = wm_stat_ptr->num_wms;

  if(index >= MAX_WM_CNT) {
      DATA_MSG0_ERROR("Max WM count reached. Cannot report statistics");
      return;
  }

  //Get the call_id  
  call_id = pdcp_get_call_info_wm_tracing(rx_func_para);

  switch (wm_id) {
  case WM_DIR_RLC_PDCP_DL:
    DATA_MSG0_MED("Reporting statistics for WM_DIR_RLC_PDCP_DL");
    /* Report the technology */
    wm_stat_ptr->wm_info[index].tech_id=UNKNOWN_TECH;

    /* Report the Call ID */
    wm_stat_ptr->wm_info[index].call_id = call_id;

    /* Specify the WM for which the stats are being reported */
    wm_stat_ptr->wm_info[index].wm_id = WM_DIR_RLC_PDCP_DL;

    /* query DSM for wm stats */
    dsm_get_wm_stats(&(pdcp_ctl_blk[pdcp_inst].rlc_state.rlc_rx_wm_item), 
                     &(wm_stat_ptr->wm_info[index].wm_counts));
    wm_stat_ptr->num_wms++;
    break;

  case WM_DIR_RLC_PDCP_UL:
    DATA_MSG0_MED("Reporting statistics for WM_DIR_RLC_PDCP_UL");
    wm_stat_ptr->wm_info[index].tech_id=UNKNOWN_TECH;

    wm_stat_ptr->wm_info[index].call_id = call_id;
    wm_stat_ptr->wm_info[index].wm_id = WM_DIR_RLC_PDCP_UL;
    dsm_get_wm_stats(&(pdcp_ctl_blk[pdcp_inst].rlc_state.rlc_tx_wm_item), 
                     &(wm_stat_ptr->wm_info[index].wm_counts));
    wm_stat_ptr->num_wms++;
    break;

  default:
    break;
  }
}

/*===========================================================================
FUNCTION  PDCP_REPORT_WM_STATS_CB

DESCRIPTION
  Called by FC to gather watermark statistics

PARAMETERS
  call_instance - Index into call table for the call
  wm_stat_ptr - Pointer to structure where fc stores watermark info
DEPENDENCIES
  Called in FC TASK context.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void pdcp_report_wm_stats_cb
(
  uint32 pdcp_inst,
  fc_wm_stat_type *wm_stat_ptr
)
{
  
  DATA_MSG1_MED("pdcp_report_wm_stats_cb called by FC for pdcp_inst %d",
                 pdcp_inst);
  
  /*------------------------------------------------------------
  Collect statistics for DL and UL watermarks
  --------------------------------------------------------------*/
  pdcp_report_wm_stats(pdcp_inst, 
                       pdcp_ctl_blk[pdcp_inst].pdcp_to_ps_rx_func_para,
                       wm_stat_ptr,
                       WM_DIR_RLC_PDCP_DL);
  pdcp_report_wm_stats(pdcp_inst, 
                       pdcp_ctl_blk[pdcp_inst].pdcp_to_ps_rx_func_para,
                       wm_stat_ptr,
                       WM_DIR_RLC_PDCP_UL);

}

#endif /* FEATURE_QXDM_WM_TRACING */

#endif /* FEATURE_DATA_WCDMA_PS */


