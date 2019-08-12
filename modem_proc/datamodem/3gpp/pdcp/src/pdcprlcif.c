/*===========================================================================

DESCRIPTION
This file has APIs relating to interactions between the PDCP layer and RLC.
  
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009-2014 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/pdcp/src/pdcprlcif.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/03/12   kv      Api clean up.
03/13/12   kv      Added L2 header task inclusions.
12/24/10   ttv     Feature wrapped the new WCDMA-L2 changes.
11/15/10   sn      Added changes to use new WCDMA-L2 interface.
05/27/10   sch     changing hwi.h to hw.h
10/15/09   vs      SU Level Api Defeaturization
08/31/09   ss      SU level CMI modifications.
08/18/09   ars     Fixed lint warnings
03/09/07   rr      Changed back return type of pdcp_get_traffic_class to 
                   ds_umts_qos_tclass_enum_type.
02/01/07   vk      Watermark level changes for HSUPA.
02/01/07   rsl     Changes relating to CPU based flow control.
11/06/06   vk      Added Data on Apps support.
09/15/06   rsl     Removed manipulating current_cnt and total_recd_cnt outside
                   of DSM to avoid discrepencies.
08/31/06   rsl     Featurized PDCP
08/18/06   rsl     Code cleanup in register_wm_with_rlc().
07/07/06   rsl     Added support for tuning WM levels at run-time based on
                   chip-id.
06/08/06   rr      Incresed PDCP->PS WM for HSDPA phase II 

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "msg.h"

#ifdef FEATURE_DATA_LTE_OFFTARGET_TEST 
#include "dsut.h"
#endif /* FEATURE_DATA_LTE_OFFTARGET_TEST */

#ifdef FEATURE_WCDMA_NEW_L2_INTERFACE
#include "wl2api.h"
#endif /* FEATURE_WCDMA_NEW_L2_INTERFACE */

#if defined (FEATURE_DATA_WCDMA_PS) || defined (FEATURE_TDSCDMA)

#define FEATURE_DSM_WM_CB
#include "dsm.h"
#include "dsm_queue.h"
#include "wcdmamvsif.h"
#include "rrcdata.h"
#include "err.h"
#include "pdcp.h"
#include "pdcpi.h"
#include "pdcprlcif.h"
#include "pdcpsig.h"
#include "dsmbit.h"

#include "ps_rohc.h"
#include "ps_iphc.h"

#ifdef FEATURE_TDSCDMA
#ifndef FEATURE_SEGMENT_LOADING
#include "tdsl2uldatamodem.h"
#include "tdsl2dldatamodem.h"
//#include "tdsrlcdsapif.h"
#include "tdsl2external.h" 
#include "tdsrrcdataext.h"
#endif /* FEATURE_SEGMENT_LOADING  */
#endif /* FEATURE_TDSCDMA */

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
#ifdef FEATURE_SEGMENT_LOADING
/*---------------------------------------------------------------------------
TDSCDMA and WCDMA interface tables
---------------------------------------------------------------------------*/
extern interface_t *ds_3gpp_ps_tds_tbl;
extern interface_t *ds_3gpp_ps_wcdma_tbl;
#endif /* FEATURE_SEGMENT_LOADING  */
/*---------------------------------------------------------------------------
  Up link RLC parameters
---------------------------------------------------------------------------*/
rlc_ul_register_srvc_type pdcp_rlc_ul_reg_params = {NULL};
rlc_ul_deregister_srvc_type pdcp_rlc_ul_dereg_params = {NULL};

/*---------------------------------------------------------------------------
  Down link RLC parameters
---------------------------------------------------------------------------*/
rlc_dl_register_srvc_type pdcp_rlc_dl_reg_params = {NULL};
rlc_dl_deregister_srvc_type pdcp_rlc_dl_dereg_params = {NULL};
#ifdef FEATURE_TDSCDMA
tdsl2_ul_datamodem_api_type  pdcp_tdrlc_ul_cmd = {NULL};     
tdsl2_dl_datamodem_api_type  pdcp_tdrlc_dl_cmd = {NULL};     
#endif /* FEATURE_TDSCDMA */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
#ifdef FEATURE_TDSCDMA
/*===========================================================================
FUNCTION pdcp_REGISTER_WM_WITH_TDRLC

DESCRIPTION
  This function registers the watermarks with RLC for uplink & downlink data
  
PARAMETERS 
  rlc_dl_id: RLC downlink id to register WM with.
  rlc_ul_id: RLC uplink id to register WM with.
  tx_wm_ptr: tx_wm that will be regd with RLC.
  rx_wm_ptr: rx_wm that will be regd with RLC.
  post_rx_func_ptr: rx func that will be called when there is data on RX path.
  rx_func_para: value passed in while calling post_rx_func.
                     
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
    
SIDE EFFECTS
  None.

===========================================================================*/
void pdcp_register_wm_with_tdrlc
(
  sys_modem_as_id_e_type       subs_id,
  uint8                        rlc_dl_id,
  uint8                        rlc_ul_id,
  dsm_watermark_type          *tx_wm_ptr,
  dsm_watermark_type          *rx_wm_ptr,
  void                        (*post_rx_func_ptr)(byte, byte, void *),
  void                        *rx_func_para
)
{
 
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_SEGMENT_LOADING
  if(ds_3gpp_ps_tds_tbl == NULL)
  {
    DATA_MSG0_ERROR("In pdcp_register_wm_with_tdrlc TDS not active");
    return ;
  }
#endif /* FEATURE_SEGMENT_LOADING  */
  /*-------------------------------------------------------------------------
   Set up the TD RLC uplink registration parameters 
  -------------------------------------------------------------------------*/

  pdcp_tdrlc_ul_cmd.api_id = TDSRLC_UL_MODEMDATA_REG_DEREG_SRVC_REQ;
  /*-----------------------------------------------------------------------
    Uplink Parameters :
    Number of uplink logical channels to register 
    Uplink RLC logical channel ID 
    Uplink Watermark
  -----------------------------------------------------------------------*/
  pdcp_tdrlc_ul_cmd.api_data.ul_reg_dereg_cmd.service = TRUE;//register
  pdcp_tdrlc_ul_cmd.api_data.ul_reg_dereg_cmd.nchan = 1;  
  pdcp_tdrlc_ul_cmd.api_data.ul_reg_dereg_cmd.rlc_id[ 0 ] = rlc_ul_id;
  pdcp_tdrlc_ul_cmd.api_data.ul_reg_dereg_cmd.ul_wm_ptr[ 0 ] = tx_wm_ptr;
  /*-----------------------------------------------------------------------
   Register the watermarks with TD RLC 
  -----------------------------------------------------------------------*/  
#ifdef FEATURE_SEGMENT_LOADING
#ifdef FEATURE_DUAL_WCDMA
  if(ITDSCDMA_tdsl2_ul_submit_datamodem_msg_sub(ds_3gpp_ps_tds_tbl, subs_id, &pdcp_tdrlc_ul_cmd) != TRUE)
#else
  if(ITDSCDMA_tdsl2_ul_submit_datamodem_msg(ds_3gpp_ps_tds_tbl, &pdcp_tdrlc_ul_cmd) != TRUE)
#endif
#else
#ifdef FEATURE_DUAL_WCDMA
  if(tdsl2_ul_submit_datamodem_msg_sub(subs_id, &pdcp_tdrlc_ul_cmd) != TRUE)
#else
  if(tdsl2_ul_submit_datamodem_msg(&pdcp_tdrlc_ul_cmd) != TRUE)
#endif
#endif /* FEATURE_SEGMENT_LOADING */
  {
    DATA_MSG1_ERROR("UL WM registration failed lc_id:%d", 
                     rlc_ul_id);
  }

  /*-------------------------------------------------------------------------
   Set up the TD RLC uplink registration parameters 
  -------------------------------------------------------------------------*/
  pdcp_tdrlc_dl_cmd.api_id = TDSRLC_DL_MODEMDATA_REG_DEREG_SRVC_REQ;

  /*-----------------------------------------------------------------------
    Downlink Parameters :
    Number of downlink logical channels to register 
    Downlink RLC logical channel ID 
    Downlink Watermark
  -----------------------------------------------------------------------*/
  pdcp_tdrlc_dl_cmd.api_data.dl_reg_dereg_cmd.service = TRUE;//register
  pdcp_tdrlc_dl_cmd.api_data.dl_reg_dereg_cmd.nchan = 1;
  pdcp_tdrlc_dl_cmd.api_data.dl_reg_dereg_cmd.rlc_id[ 0 ] = rlc_dl_id;
  pdcp_tdrlc_dl_cmd.api_data.dl_reg_dereg_cmd.dl_wm_ptr[ 0 ] = rx_wm_ptr;
  pdcp_tdrlc_dl_cmd.api_data.dl_reg_dereg_cmd.rlc_post_rx_proc_func_ptr[ 0 ] = 
    post_rx_func_ptr;
  pdcp_tdrlc_dl_cmd.api_data.dl_reg_dereg_cmd.rlc_post_rx_func_ptr_para[ 0 ] = 
    rx_func_para;
  /* Task context */
  pdcp_tdrlc_dl_cmd.api_data.dl_reg_dereg_cmd.context[ 0 ] = FALSE;  

  /*-----------------------------------------------------------------------
   Register the watermarks with RLC 
  -----------------------------------------------------------------------*/
#ifdef FEATURE_SEGMENT_LOADING
#ifdef FEATURE_DUAL_WCDMA
  if(ITDSCDMA_tdsl2_dl_submit_datamodem_msg_sub(ds_3gpp_ps_tds_tbl, subs_id, 
                                                &pdcp_tdrlc_dl_cmd) != TRUE)
#else
  if(ITDSCDMA_tdsl2_dl_submit_datamodem_msg(ds_3gpp_ps_tds_tbl, &pdcp_tdrlc_dl_cmd) != TRUE)
#endif
#else
#ifdef FEATURE_DUAL_WCDMA
  if(tdsl2_dl_submit_datamodem_msg_sub(subs_id, &pdcp_tdrlc_dl_cmd) != TRUE)
#else
  if(tdsl2_dl_submit_datamodem_msg(&pdcp_tdrlc_dl_cmd) != TRUE)
#endif
#endif /* FEATURE_SEGMENT_LOADING  */
  {
    DATA_MSG1_ERROR("DL WM registration failed lc_id:%d", rlc_dl_id);
  } 

  return;
} /* pdcp_register_wm_with_tdrlc() */

/*===========================================================================
FUNCTION pdcp_DEREGISTER_WM_WITH_TDRLC

DESCRIPTION
  This function deregisters the watermarks with TDRLC for UL & DL data

PARAMETERS
  pdcp_instance - Index into pdcp_ctl_blk.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void pdcp_deregister_wm_with_tdrlc
(
  uint8 pdcp_instance
)
{  
  pdcp_rlc_state_type *rlc_state_ptr;        /* RLC state information    */
  sys_modem_as_id_e_type  subs_id = SYS_MODEM_AS_ID_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_HIGH("In pdcp_dereg_wm_with_tdrlc pdcp_inst=%d",
          pdcp_instance, 0, 0);
#ifdef FEATURE_SEGMENT_LOADING
  if(ds_3gpp_ps_tds_tbl == NULL)
  {
    DATA_MSG0_ERROR("In pdcp_deregister_wm_with_tdrlc TDS not active");
    return;
  }
#endif /* FEATURE_SEGMENT_LOADING  */
  /*-------------------------------------------------------------------------
    Get RLC state information for the call.
  -------------------------------------------------------------------------*/
  rlc_state_ptr = &(pdcp_ctl_blk[pdcp_instance].rlc_state);
  subs_id = pdcp_ctl_blk[pdcp_instance].subs_id;

  /*-------------------------------------------------------------------------
   Set up the TD RLC uplink registration parameters 
  -------------------------------------------------------------------------*/
  pdcp_tdrlc_ul_cmd.api_id = TDSRLC_UL_MODEMDATA_REG_DEREG_SRVC_REQ;
  pdcp_tdrlc_dl_cmd.api_id = TDSRLC_DL_MODEMDATA_REG_DEREG_SRVC_REQ;

  /*-------------------------------------------------------------------------
    Deregister the RLC UL & DL watermarks
  -------------------------------------------------------------------------*/
  pdcp_tdrlc_ul_cmd.api_data.ul_reg_dereg_cmd.service = FALSE;//deregister
  pdcp_tdrlc_ul_cmd.api_data.ul_reg_dereg_cmd.nchan = 1;
  pdcp_tdrlc_ul_cmd.api_data.ul_reg_dereg_cmd.rlc_id[ 0 ] = rlc_state_ptr->rlc_ul_id;

  pdcp_tdrlc_dl_cmd.api_data.dl_reg_dereg_cmd.service = FALSE;//deregister
  pdcp_tdrlc_dl_cmd.api_data.dl_reg_dereg_cmd.nchan = 1;
  pdcp_tdrlc_dl_cmd.api_data.dl_reg_dereg_cmd.rlc_id[ 0 ] = rlc_state_ptr->rlc_dl_id;

  /*-------------------------------------------------------------------------
    Post L2 UL command 
  -------------------------------------------------------------------------*/
#ifdef FEATURE_SEGMENT_LOADING
#ifdef FEATURE_DUAL_WCDMA
  if(ITDSCDMA_tdsl2_ul_submit_datamodem_msg_sub(ds_3gpp_ps_tds_tbl,
                      subs_id, &pdcp_tdrlc_ul_cmd) != TRUE)
#else
  if(ITDSCDMA_tdsl2_ul_submit_datamodem_msg(ds_3gpp_ps_tds_tbl, &pdcp_tdrlc_ul_cmd) != TRUE)
#endif
#else
#ifdef FEATURE_DUAL_WCDMA
  if(tdsl2_ul_submit_datamodem_msg_sub(subs_id, &pdcp_tdrlc_ul_cmd) != TRUE)
#else
  if(tdsl2_ul_submit_datamodem_msg(&pdcp_tdrlc_ul_cmd) != TRUE)
#endif
#endif /* #ifdef FEATURE_SEGMENT_LOADING */
  {
    DATA_MSG1_ERROR("UL WM registration failed lc_id:%d", 
                     rlc_state_ptr->rlc_ul_id);
  }

  /*-------------------------------------------------------------------------
    Post L2 DL command 
  -------------------------------------------------------------------------*/
#ifdef FEATURE_SEGMENT_LOADING
#ifdef FEATURE_DUAL_WCDMA
  if(ITDSCDMA_tdsl2_dl_submit_datamodem_msg_sub(ds_3gpp_ps_tds_tbl, subs_id, 
                                                &pdcp_tdrlc_dl_cmd) != TRUE)
#else
  if(ITDSCDMA_tdsl2_dl_submit_datamodem_msg(ds_3gpp_ps_tds_tbl, &pdcp_tdrlc_dl_cmd) != TRUE)
#endif
#else
#ifdef FEATURE_DUAL_WCDMA
   if(tdsl2_dl_submit_datamodem_msg_sub(subs_id, &pdcp_tdrlc_dl_cmd) != TRUE)
#else
  if(tdsl2_dl_submit_datamodem_msg(&pdcp_tdrlc_dl_cmd) != TRUE)
#endif
#endif /* #ifdef FEATURE_SEGMENT_LOADING */
  {
    DATA_MSG1_ERROR("DL WM deregistration failed lc_id:%d", 
                     rlc_state_ptr->rlc_dl_id);
  }

} /* pdcp_deregister_wm_with_tdrlc() */
#endif /* FEATURE_TDSCDMA */

/*===========================================================================
FUNCTION pdcp_REGISTER_WM_WITH_RLC

DESCRIPTION
  This function registers the watermarks with TD RLC for uplink & downlink data
  
PARAMETERS 
  rlc_dl_id: RLC downlink id to register WM with.
  rlc_ul_id: RLC uplink id to register WM with.
  tx_wm_ptr: tx_wm that will be regd with RLC.
  rx_wm_ptr: rx_wm that will be regd with RLC.
  post_rx_func_ptr: rx func that will be called when there is data on RX path.
  rx_func_para: value passed in while calling post_rx_func.
                     
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
    
SIDE EFFECTS
  None.

===========================================================================*/
void pdcp_register_wm_with_rlc
(
  sys_modem_as_id_e_type       subs_id,  
  rlc_lc_id_type               rlc_dl_id,
  rlc_lc_id_type               rlc_ul_id,
  dsm_watermark_type          *tx_wm_ptr,
  dsm_watermark_type          *rx_wm_ptr,
  void                        (*post_rx_func_ptr)(byte, byte, void *),
  void                        *rx_func_para
)
{
  l2_ul_cmd_type              *l2_ul_cmd;
  l2_dl_cmd_type              *l2_dl_cmd;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_SEGMENT_LOADING
  if(ds_3gpp_ps_wcdma_tbl == NULL)
  {
    DATA_MSG0_ERROR("In pdcp_register_wm_with_rlc WCDMA not active");
    return;
  }
#endif /* FEATURE_SEGMENT_LOADING */

  /*-------------------------------------------------------------------------
   Set up the RLC uplink registration parameters 
  -------------------------------------------------------------------------*/
#ifdef FEATURE_SEGMENT_LOADING
  if ((l2_ul_cmd = IWCDMA_l2_ul_get_cmd_buf(ds_3gpp_ps_wcdma_tbl)) == NULL)
#else
  if ((l2_ul_cmd = l2_ul_get_cmd_buf()) == NULL)
#endif /* FEATURE_SEGMENT_LOADING */
  {
    DS_3GPP_MSG0_ERROR("NULL Ptr when sending RLC_UL_REGISTER_SRVC_REQ");
  }
  else /* a good ptr for RLC Data SAP BCCH data has been received */
  {
    /*-----------------------------------------------------------------------
      Uplink Parameters :
      Number of uplink logical channels to register 
      Uplink RLC logical channel ID 
      Uplink Watermark
    -----------------------------------------------------------------------*/

	pdcp_rlc_ul_reg_params.nchan = 1;
	pdcp_rlc_ul_reg_params.rlc_data[ 0 ].lc_id = rlc_ul_id;
	pdcp_rlc_ul_reg_params.rlc_data[ 0 ].ul_wm_ptr = tx_wm_ptr;

    /*-----------------------------------------------------------------------
     Register the watermarks with RLC 
    -----------------------------------------------------------------------*/
    l2_ul_cmd->cmd_hdr.cmd_id = RLC_UL_REGISTER_SRVC_REQ;
#ifdef FEATURE_DUAL_WCDMA
    l2_ul_cmd->cmd_hdr.as_id = subs_id;
#endif /* FEATURE_DUAL_WCDMA */
    l2_ul_cmd->cmd_data.ul_reg = pdcp_rlc_ul_reg_params;
#ifdef FEATURE_SEGMENT_LOADING
    IWCDMA_l2_ul_put_cmd(ds_3gpp_ps_wcdma_tbl, l2_ul_cmd);
#else
    l2_ul_put_cmd(l2_ul_cmd);
#endif /* FEATURE_SEGMENT_LOADING */

  }
#ifdef FEATURE_SEGMENT_LOADING
  if ((l2_dl_cmd = IWCDMA_l2_dl_get_cmd_buf(ds_3gpp_ps_wcdma_tbl)) == NULL)
#else
  if ((l2_dl_cmd = l2_dl_get_cmd_buf()) == NULL)
#endif /* FEATURE_SEGMENT_LOADING */
  {
    DS_3GPP_MSG0_ERROR("NULL Ptr when sending RLC_UL_REGISTER_SRVC_REQ");
  }
  else /* a good ptr for RLC Data SAP BCCH data has been received */
  {
    /*-----------------------------------------------------------------------
      Downlink Parameters :
      Number of downlink logical channels to register 
      Downlink RLC logical channel ID 
      Downlink Watermark
    -----------------------------------------------------------------------*/

    pdcp_rlc_dl_reg_params.nchan = 1;
    pdcp_rlc_dl_reg_params.rlc_data[ 0 ].lc_id = rlc_dl_id;
	pdcp_rlc_dl_reg_params.rlc_data[ 0 ].dl_wm_ptr = rx_wm_ptr;
    pdcp_rlc_dl_reg_params.rlc_data[ 0 ].rlc_post_rx_proc_func_ptr = 
      post_rx_func_ptr;
    pdcp_rlc_dl_reg_params.rlc_data[ 0 ].rlc_post_rx_func_ptr_para = 
      rx_func_para;
    pdcp_rlc_dl_reg_params.rlc_data[ 0 ].context = FALSE;  /* Task context */

    /*-----------------------------------------------------------------------
     Register the watermarks with RLC 
    -----------------------------------------------------------------------*/
    l2_dl_cmd->cmd_hdr.cmd_id = RLC_DL_REGISTER_SRVC_REQ;
    l2_dl_cmd->cmd_data.dl_reg = pdcp_rlc_dl_reg_params;
#ifdef FEATURE_DUAL_WCDMA
    l2_dl_cmd->cmd_hdr.as_id = subs_id;
#endif
#ifdef FEATURE_SEGMENT_LOADING
    IWCDMA_l2_dl_put_cmd(ds_3gpp_ps_wcdma_tbl,l2_dl_cmd);
#else
    l2_dl_put_cmd(l2_dl_cmd);
#endif /* FEATURE_SEGMENT_LOADING */
  }
  return;
} /* pdcp_register_wm_with_rlc() */


/*===========================================================================
FUNCTION pdcp_DEREGISTER_WM_WITH_RLC

DESCRIPTION
  This function deregisters the watermarks with RLC for uplink & downlink data

PARAMETERS
  pdcp_instance - Index into pdcp_ctl_blk.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void pdcp_deregister_wm_with_rlc
(
  uint8 pdcp_instance
)
{
  l2_ul_cmd_type *l2_ul_cmd_ptr;               /* Pointer to L2 UL command */
  l2_dl_cmd_type *l2_dl_cmd_ptr;               /* Pointer to L2 DL command */
  pdcp_rlc_state_type *rlc_state_ptr;        /* RLC state information    */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_HIGH("In pdcp_dereg_wm_with_rlc pdcp_inst=%d",
          pdcp_instance, 0, 0);

#ifdef FEATURE_SEGMENT_LOADING
  if(ds_3gpp_ps_wcdma_tbl == NULL)
  {
    DATA_MSG0_ERROR("In pdcp_deregister_wm_with_rlc WCDMA not active");
    return;
  }
#endif /* FEATURE_SEGMENT_LOADING */
  if(pdcp_validate_pdcp_inst(pdcp_instance) == FALSE)
  {
    DATA_MSG0_HIGH("pdcp_deregister_wm_with_rlc pdcp_inst is not Valid");
    return;
  }
  
  /*-------------------------------------------------------------------------
    Get RLC state information for the call.
  -------------------------------------------------------------------------*/
  rlc_state_ptr = &(pdcp_ctl_blk[pdcp_instance].rlc_state);

  /*-------------------------------------------------------------------------
    Deregister the RLC UL & DL watermarks
  -------------------------------------------------------------------------*/
  pdcp_rlc_ul_dereg_params.nchan = 1;
  pdcp_rlc_ul_dereg_params.rlc_id[ 0 ] = rlc_state_ptr->rlc_ul_id;

  pdcp_rlc_dl_dereg_params.nchan = 1;
  pdcp_rlc_dl_dereg_params.rlc_id[ 0 ] = rlc_state_ptr->rlc_dl_id;

  /*-------------------------------------------------------------------------
    Get an L2 UL command buffer
  -------------------------------------------------------------------------*/
#ifdef FEATURE_SEGMENT_LOADING
  if((l2_ul_cmd_ptr = IWCDMA_l2_ul_get_cmd_buf(ds_3gpp_ps_wcdma_tbl)) != NULL )
#else
  if ( ( l2_ul_cmd_ptr = l2_ul_get_cmd_buf() ) != NULL )
#endif /* FEATURE_SEGMENT_LOADING */
  {
    l2_ul_cmd_ptr->cmd_hdr.cmd_id = RLC_UL_DEREGISTER_SRVC_REQ;
#ifdef FEATURE_DUAL_WCDMA
    l2_ul_cmd_ptr->cmd_hdr.as_id = pdcp_ctl_blk[pdcp_instance].subs_id;
#endif /* FEATURE_DUAL_WCDMA */
    l2_ul_cmd_ptr->cmd_data.ul_dereg = pdcp_rlc_ul_dereg_params;

#ifdef FEATURE_SEGMENT_LOADING
   IWCDMA_l2_ul_put_cmd(ds_3gpp_ps_wcdma_tbl, l2_ul_cmd_ptr);
#else
    l2_ul_put_cmd( l2_ul_cmd_ptr );
#endif /* FEATURE_SEGMENT_LOADING */
  }
  else
  {
    DS_3GPP_MSG0_ERROR( "Cannot get L2 UL cmd buffer");
  }

  /*-------------------------------------------------------------------------
    Get an L2 DL command buffer
  -------------------------------------------------------------------------*/
#ifdef FEATURE_SEGMENT_LOADING
  if ( ( l2_dl_cmd_ptr = IWCDMA_l2_dl_get_cmd_buf(ds_3gpp_ps_wcdma_tbl))!= NULL )
#else
  if ( ( l2_dl_cmd_ptr = l2_dl_get_cmd_buf() ) != NULL )
#endif /* FEATURE_SEGMENT_LOADING */
  {
    l2_dl_cmd_ptr->cmd_hdr.cmd_id = RLC_DL_DEREGISTER_SRVC_REQ;
#ifdef FEATURE_DUAL_WCDMA
    l2_ul_cmd_ptr->cmd_hdr.as_id = pdcp_ctl_blk[pdcp_instance].subs_id;
#endif /* FEATURE_DUAL_WCDMA */
    l2_dl_cmd_ptr->cmd_data.dl_dereg = pdcp_rlc_dl_dereg_params;

#ifdef FEATURE_SEGMENT_LOADING
    IWCDMA_l2_dl_put_cmd(ds_3gpp_ps_wcdma_tbl, l2_dl_cmd_ptr );
#else
    l2_dl_put_cmd( l2_dl_cmd_ptr );
#endif /* FEATURE_SEGMENT_LOADING */
  }
  else
  {
    DS_3GPP_MSG0_ERROR( "Cannot get L2 DL cmd buffer");
  }
} /* pdcp_deregister_wm_with_rlc() */

/*===========================================================================
FUNCTION pdcp_RESET_SENT_CLOSE_TO_RLC()

DESCRIPTION
  This function is called to reset the flag which tracks if we have sent 
  a RLC window close request to trigger RLC peer-to-peer flow control.

PARAMETERS   
  pdcp_inst - Index into call table for call

DEPENDENCIES 
  None.

RETURN VALUE 
  None.

SIDE EFFECTS 
  None.
  
===========================================================================*/
void pdcp_reset_sent_close_to_rlc
(
  uint8 pdcp_inst
)
{
  pdcp_ctl_blk[pdcp_inst].rlc_state.sent_close_to_rlc = FALSE;
} /* pdcp_reset_sent_close_to_rlc */




/*===========================================================================
FUNCTION pdcp_Pdcp_TO_RLC_LO_WATER_CB

DESCRIPTION
  This function is called when the PDCP to RLC watermark reaches the low
  watermark. 
  
PARAMETERS
   wm            : pointer to the watermark
   callback_data : pdcp instance registered with the watermark callback
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  
===========================================================================*/
void pdcp_pdcp_to_rlc_lo_water_cb
(
  dsm_watermark_type *wm,
  void*              callback_data
)
{
  uint32 pdcp_inst;
  comp_cmd_type *pdcp_cmd_ptr;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  pdcp_inst = (uint32)callback_data;

  PDCP_ENTER_CRIT_SECTION ( &pdcp_crit_section );
  if (FALSE == pdcp_validate_pdcp_inst(pdcp_inst))
  {
    DATA_MSG0_HIGH("pdcp_inst invalid in pdcp_pdcp_to_rlc_lo_water_cb");
    PDCP_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
    return;
  }

  if((pdcp_cmd_ptr = comp_get_cmd_buf()) == NULL)
  {
    /*-----------------------------------------------------------------------
      No free command buffers available, log an error.
    -----------------------------------------------------------------------*/
    DS_3GPP_MSG0_ERROR( "No items on PDCP Task free cmd q");
    PDCP_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
    return;
  }
  
  DATA_MSG2_HIGH("pdcp_to_rlc_lo_water_cb, wm= 0x%x, pdcp_inst = %d", 
                  wm, pdcp_inst);
  /*-------------------------------------------------------------------------
    Enable tx flow mask
  -------------------------------------------------------------------------*/
  pdcp_cmd_ptr->hdr.cmd_id = PDCP_SET_FLOW_MASK;
  pdcp_cmd_ptr->hdr.subs_id = pdcp_ctl_blk[pdcp_inst].subs_id;
  PDCP_LEAVE_CRIT_SECTION ( &pdcp_crit_section );  
  pdcp_cmd_ptr->cmd.pdcp_set_flow_mask.pdcp_inst = 0;
  pdcp_cmd_ptr->cmd.pdcp_set_flow_mask.flow_dir = PDCP_TO_RLC;
  pdcp_cmd_ptr->cmd.pdcp_set_flow_mask.flow_mask =  DS_FLOW_ENABLE;
  comp_put_cmd(pdcp_cmd_ptr);

} /* pdcp_pdcp_to_rlc_lo_water_cb */

/*===========================================================================
FUNCTION pdcp_PDCP_TO_RLC_HI_WATER_CB

DESCRIPTION
  This function is called when the PDCP to RLC watermark reaches the high
  watermark. 
  
PARAMETERS
   wm            : pointer to the watermark
   callback_data : pdcp instance registered with the watermark callback
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  
===========================================================================*/
void pdcp_pdcp_to_rlc_hi_water_cb
(
  dsm_watermark_type *wm,
  void*              callback_data
)
{
  uint8 pdcp_inst;
  comp_cmd_type *pdcp_cmd_ptr;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  pdcp_inst = (uint8)((uint32) callback_data);

  PDCP_ENTER_CRIT_SECTION ( &pdcp_crit_section );
  if (FALSE == pdcp_validate_pdcp_inst(pdcp_inst))
  {
    DATA_MSG0_HIGH("pdcp_inst invalid in pdcp_pdcp_to_rlc_hi_water_cb");
    PDCP_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
    return;
  }  

  if((pdcp_cmd_ptr = comp_get_cmd_buf()) == NULL)
  {
    /*-----------------------------------------------------------------------
      No free command buffers available, log an error.
    -----------------------------------------------------------------------*/
    DS_3GPP_MSG0_ERROR( "No items on PDCP Task free cmd q");
    PDCP_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
    return;
  } 
  
  DATA_MSG2_HIGH("pdcp_to_rlc_hi_water_cb, wm= 0x%x, pdcp_inst = %d",
                  wm, pdcp_inst);
  /*-------------------------------------------------------------------------
    Disable tx flow mask
  -------------------------------------------------------------------------*/
  pdcp_cmd_ptr->hdr.cmd_id = PDCP_SET_FLOW_MASK;
  pdcp_cmd_ptr->hdr.subs_id = pdcp_ctl_blk[pdcp_inst].subs_id;
  pdcp_cmd_ptr->cmd.pdcp_set_flow_mask.pdcp_inst = pdcp_inst;
  PDCP_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
  pdcp_cmd_ptr->cmd.pdcp_set_flow_mask.flow_dir = PDCP_TO_RLC;
  pdcp_cmd_ptr->cmd.pdcp_set_flow_mask.flow_mask =  DS_FLOW_DISABLE;
  comp_put_cmd(pdcp_cmd_ptr);
} /* pdcp_pdcp_to_rlc_hi_water_cb */

/*===========================================================================
FUNCTION      PDCP_CLEANUP_RLC_WATERMARKS_CB

DESCRIPTION   
  This function cleans up the watermarks that are owned by data services.

PARAMETERS   
  call_instance - Index into call table for call.
  
DEPENDENCIES  
  None.

RETURN VALUE  
  None.

SIDE EFFECTS  
  None.
===========================================================================*/
void pdcp_cleanup_rlc_watermarks_cb
(
  void *pdcp_inst
)
{
  pdcp_rlc_state_type       *rlc_state_ptr; /* rlc_info for each pdcp_inst */
  uint8                      pdcp_instance;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  pdcp_instance = (uint8) ((uint32) pdcp_inst);
  DATA_MSG1_HIGH("pdcp_cleanup_rlc_watermarks_cb pdcp_inst = %d", 
                  pdcp_instance);

  PDCP_ENTER_CRIT_SECTION ( &pdcp_crit_section );
  if (pdcp_instance < MAX_PDCP_INST) 
  {
    /*-------------------------------------------------------------------------
      Get RLC state information for the call.
    -------------------------------------------------------------------------*/
    rlc_state_ptr = &(pdcp_ctl_blk[pdcp_instance].rlc_state);
  
    dsm_empty_queue ( &rlc_state_ptr->rlc_tx_wm_item );
    dsm_empty_queue ( &rlc_state_ptr->rlc_rx_wm_item );
  }
  else
  {
    DATA_MSG1_ERROR("pdcp inst validation failed %d", pdcp_instance);
  }
  PDCP_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
} /* pdcp_cleanup_rlc_watermarks_cb */

#ifdef FEATURE_TDSCDMA
/*===========================================================================
FUNCTION pdcp_TDRLC_TO_PDCP_LO_WATER_CB

DESCRIPTION
  This function is called when the TDRLC to PDCP watermark reaches the low
  watermark. We send the RLC_OPEN_RX_WIN primitive to RLC so it will in turn
  open the RLC RX window of the UE.
  
PARAMETERS
   wm            : pointer to the watermark
   callback_data : pdcp instance registered with the watermark callback
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  Causes RLC to request the UE's RX window be opened back up for this RLC ID
  
===========================================================================*/
void pdcp_tdrlc_to_pdcp_lo_water_cb
(
  dsm_watermark_type *wm,
  void*              callback_data
)
{
  pdcp_rlc_state_type  *rlc_state_ptr;      /* rlc_info for each pdcp_inst */
  uint8                pdcp_instance;    /* Index into call table for call */
  sys_modem_as_id_e_type subs_id = SYS_MODEM_AS_ID_NONE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*lint -e734*/
  pdcp_instance = (uint8) ((uint32) callback_data);
  /*lint +e734*/

  DATA_MSG2_HIGH("tdrlc_to_pdcp_lo_water_cb, wm= %d, pdcp_inst = %d", 
                   wm, pdcp_instance);
#ifdef FEATURE_SEGMENT_LOADING
  if(ds_3gpp_ps_tds_tbl == NULL)
  {
    DATA_MSG0_ERROR("In pdcp_tdrlc_to_pdcp_lo_water_cb TDS not active");
    return;
  }
#endif /* FEATURE_SEGMENT_LOADING */
  /*-------------------------------------------------------------------------
    Get RLC state information for call.
  -------------------------------------------------------------------------*/
  PDCP_ENTER_CRIT_SECTION ( &pdcp_crit_section );
  rlc_state_ptr = &(pdcp_ctl_blk[pdcp_instance].rlc_state);
  subs_id = pdcp_ctl_blk[pdcp_instance].subs_id;

  /*------------------------------------------------------------------------
    This call back will be called every time the low threshold is crossed.
    i.e. We may cross the low mark many times w/o crossing the hi mark. This
    flag ensures we don't send redundant commands to RLC.
  -------------------------------------------------------------------------*/
  if(rlc_state_ptr->sent_close_to_rlc == FALSE)
  {
    PDCP_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
    return;
  }
 
  /*------------------------------------------------------------------------
   Send the command to TDRLC
  -------------------------------------------------------------------------*/
  rlc_state_ptr->sent_close_to_rlc = FALSE;
  pdcp_tdrlc_ul_cmd.api_id = TDSRLC_UL_DATAMODEM_OPEN_RX_WIN;  
  pdcp_tdrlc_ul_cmd.api_data.ul_rlc_data_id = rlc_state_ptr->rlc_ul_id;
  PDCP_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
#ifdef FEATURE_SEGMENT_LOADING
#ifdef FEATURE_DUAL_WCDMA
  if(ITDSCDMA_tdsl2_ul_submit_datamodem_msg_sub(ds_3gpp_ps_tds_tbl,subs_id, &pdcp_tdrlc_ul_cmd) != TRUE)
#else
  if(ITDSCDMA_tdsl2_ul_submit_datamodem_msg(ds_3gpp_ps_tds_tbl, &pdcp_tdrlc_ul_cmd) != TRUE)
#endif
#else
#ifdef FEATURE_DUAL_WCDMA
  if(tdsl2_ul_submit_datamodem_msg_sub(subs_id, &pdcp_tdrlc_ul_cmd) != TRUE)
#else
  if(tdsl2_ul_submit_datamodem_msg(&pdcp_tdrlc_ul_cmd) != TRUE)
#endif
#endif /* FEATURE_SEGMENT_LOADING */
  {
    DATA_MSG1_ERROR("TDSRLC_UL_DATAMODEM_OPEN_RX_WIN failed lc_id:%d",
                     rlc_state_ptr->rlc_ul_id);
  }  
  
} /* pdcp_rlc_to_pdcp_lo_water_cb */

/*===========================================================================
FUNCTION pdcp_TDRLC_TO_PDCP_HI_WATER_CB

DESCRIPTION
  This function is called when the TDRLC to PDCP watermark reaches the high
  watermark. We send the RLC_CLOSE_RX_WIN primitive to RLC so it will in turn
  close the RLC RX window of the UE.
  
PARAMETERS 
   wm            : pointer to the watermark
   callback_data : pdcp instance registered with the watermark callback
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  Causes RLC to close the UE's RX window to one PDU (for this RLC ID)  

===========================================================================*/
void pdcp_tdrlc_to_pdcp_hi_water_cb
(
  dsm_watermark_type *wm,
  void*              callback_data
)
{
  pdcp_rlc_state_type  *rlc_state_ptr;      /* rlc_info for each pdcp_inst */
  uint8                pdcp_instance;    /* Index into call table for pdcp */
  sys_modem_as_id_e_type subs_id = SYS_MODEM_AS_ID_NONE;
 /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*lint -e734*/
  pdcp_instance = (uint8) ((uint32) callback_data);
  /*lint +e734*/
  
  DATA_MSG2_HIGH("tdrlc_to_pdcp_hi_water_cb, wm= %d, pdcp_inst = %d", 
                  wm, pdcp_instance);
#ifdef FEATURE_SEGMENT_LOADING
  if(ds_3gpp_ps_tds_tbl == NULL)
  {
    DATA_MSG0_ERROR("In pdcp_tdrlc_to_pdcp_hi_water_cb TDS not active");
    return;
  }
#endif /* FEATURE_SEGMENT_LOADING */
   /*-------------------------------------------------------------------------
    Get RLC state information for pdcp inst.
  -------------------------------------------------------------------------*/
  rlc_state_ptr = &(pdcp_ctl_blk[pdcp_instance].rlc_state);
  subs_id = pdcp_ctl_blk[pdcp_instance].subs_id;
  
  /*-------------------------------------------------------------------------
    This call back will be called every time the high threshold is crossed.
    i.e. We may cross the high mark many times w/o crossing the low mark.
    This flag ensures we don't send redundant commands to RLC.
  -------------------------------------------------------------------------*/
  PDCP_ENTER_CRIT_SECTION ( &pdcp_crit_section );
  if(rlc_state_ptr->sent_close_to_rlc == TRUE)
  {
    PDCP_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
    return;
  }

  /*------------------------------------------------------------------------
   Send the command to TDRLC
  -------------------------------------------------------------------------*/
  rlc_state_ptr->sent_close_to_rlc = TRUE;
  pdcp_tdrlc_ul_cmd.api_id = TDSRLC_UL_DATAMODEM_CLOSE_RX_WIN;  
  pdcp_tdrlc_ul_cmd.api_data.ul_rlc_data_id = rlc_state_ptr->rlc_ul_id;
  PDCP_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
#ifdef FEATURE_SEGMENT_LOADING
#ifdef FEATURE_DUAL_WCDMA
  if(ITDSCDMA_tdsl2_ul_submit_datamodem_msg_sub(ds_3gpp_ps_tds_tbl, subs_id, &pdcp_tdrlc_ul_cmd) != TRUE)
#else
  if(ITDSCDMA_tdsl2_ul_submit_datamodem_msg(ds_3gpp_ps_tds_tbl, &pdcp_tdrlc_ul_cmd) != TRUE)
#endif
#else
#ifdef FEATURE_DUAL_WCDMA
  if(tdsl2_ul_submit_datamodem_msg_sub(subs_id, &pdcp_tdrlc_ul_cmd) != TRUE)
#else
  if(tdsl2_ul_submit_datamodem_msg(&pdcp_tdrlc_ul_cmd) != TRUE)
#endif
#endif /* FEATURE_SEGMENT_LOADING */
  {
    DATA_MSG1_ERROR("TDSRLC_UL_DATAMODEM_CLOSE_RX_WIN failed lc_id:%d",
                      rlc_state_ptr->rlc_ul_id);
  }     

}/* pdcp_tdrlc_to_pdcp_hi_water_cb() */

/*===========================================================================
FUNCTION      PDCP_CLEANUP_TDRLC_WATERMARKS

DESCRIPTION   
  This function cleans up the watermarks that are owned by data services.

PARAMETERS   
  call_instance - Index into call table for call.
  
DEPENDENCIES  
  None.

RETURN VALUE  
  None.

SIDE EFFECTS  
  None.
===========================================================================*/
void pdcp_cleanup_tdrlc_watermarks
(
  uint32 pdcp_instance
)
{
  sys_modem_as_id_e_type subs_id = SYS_MODEM_AS_ID_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_SEGMENT_LOADING
  if(ds_3gpp_ps_tds_tbl == NULL)
  {
    DATA_MSG0_ERROR("pdcp_cleanup_tdrlc_watermarks TDS not active");
    return ;
  }
#endif /* FEATURE_SEGMENT_LOADING  */
  
  PDCP_ENTER_CRIT_SECTION ( &pdcp_crit_section );
  /*------------------------------------------------------------------------
   Send the command to TDRLC
  -------------------------------------------------------------------------*/  
  pdcp_tdrlc_dl_cmd.api_id = TDSRLC_DL_MODEMDATA_PDCP_CLEANUP_WM;  
  pdcp_tdrlc_dl_cmd.api_data.cleanup_wm_cmd.cleanup_wm_para = (void *)pdcp_instance;
  pdcp_tdrlc_dl_cmd.api_data.cleanup_wm_cmd.cleanup_wm_cb = 
                                              pdcp_cleanup_rlc_watermarks_cb;
  pdcp_tdrlc_dl_cmd.api_data.cleanup_wm_cmd.lc_id = 
                             pdcp_ctl_blk[pdcp_instance].rlc_state.rlc_dl_id;
  subs_id = pdcp_ctl_blk[pdcp_instance].subs_id;
  PDCP_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
#ifdef FEATURE_SEGMENT_LOADING
#ifdef FEATURE_DUAL_WCDMA
  if(ITDSCDMA_tdsl2_dl_submit_datamodem_msg_sub(ds_3gpp_ps_tds_tbl, subs_id, 
                                                &pdcp_tdrlc_dl_cmd) != TRUE)
#else
  if(ITDSCDMA_tdsl2_dl_submit_datamodem_msg(ds_3gpp_ps_tds_tbl, &pdcp_tdrlc_dl_cmd) != TRUE)
#endif
#else
#ifdef FEATURE_DUAL_WCDMA
  if(tdsl2_dl_submit_datamodem_msg_sub(subs_id, &pdcp_tdrlc_dl_cmd) != TRUE)
#else
  if(tdsl2_dl_submit_datamodem_msg(&pdcp_tdrlc_dl_cmd) != TRUE)
#endif
#endif /* FEATURE_SEGMENT_LOADING */
  {
    DATA_MSG1_ERROR("TDSRLC_DL_MODEMDATA_PDCP_CLEANUP_WM failed lc_id:%d",
                     pdcp_ctl_blk[pdcp_instance].rlc_state.rlc_dl_id);
  } 
} /* pdcp_cleanup_tdrlc_watermarks */
#endif /* FEATURE_TDSCDMA */

/*===========================================================================
FUNCTION pdcp_RLC_TO_PDCP_LO_WATER_CB

DESCRIPTION
  This function is called when the RLC to PDCP watermark reaches the low
  watermark. We send the RLC_OPEN_RX_WIN primitive to RLC so it will in turn
  open the RLC RX window of the UE.
  
PARAMETERS
   wm            : pointer to the watermark
   callback_data : pdcp instance registered with the watermark callback
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  Causes RLC to request the UE's RX window be opened back up for this RLC ID
  
===========================================================================*/
void pdcp_rlc_to_pdcp_lo_water_cb
(
  dsm_watermark_type *wm,
  void*              callback_data
)
{
#ifdef FEATURE_WCDMA_NEW_L2_INTERFACE
  wcdma_l2_ul_cmd_type *l2_ul_cmd;                           /* L2 command */
#else
  l2_ul_cmd_type *l2_ul_cmd;                                 /* L2 command */
#endif /* FEATURE_WCDMA_NEW_L2_INTERFACE */
  pdcp_rlc_state_type  *rlc_state_ptr;      /* rlc_info for each pdcp_inst */
  uint8                pdcp_instance;    /* Index into call table for call */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*lint -e734*/
  pdcp_instance = (uint8) ((uint32) callback_data);
  /*lint +e734*/

  DATA_MSG2_HIGH("rlc_to_pdcp_lo_water_cb, wm= %d, pdcp_inst = %d",
                  wm, pdcp_instance);
  /*-------------------------------------------------------------------------
    Get RLC state information for call.
  -------------------------------------------------------------------------*/
  PDCP_ENTER_CRIT_SECTION ( &pdcp_crit_section );
  rlc_state_ptr = &(pdcp_ctl_blk[pdcp_instance].rlc_state);
  PDCP_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
  /*------------------------------------------------------------------------
    This call back will be called every time the low threshold is crossed.
    i.e. We may cross the low mark many times w/o crossing the hi mark. This
    flag ensures we don't send redundant commands to RLC.
  -------------------------------------------------------------------------*/
  if(rlc_state_ptr->sent_close_to_rlc == FALSE)
  {
    return;
  }

  /*------------------------------------------------------------------------
   Send the command to RLC
  -------------------------------------------------------------------------*/

#ifdef FEATURE_WCDMA_NEW_L2_INTERFACE

#ifdef FEATURE_SEGMENT_LOADING
  if(ds_3gpp_ps_wcdma_tbl == NULL)
  {
    DATA_MSG0_ERROR("In pdcp_rlc_to_pdcp_lo_water_cb WCDMA not active");
    return;
  }
    if ((l2_ul_cmd = IWCDMA_wcdma_l2_ul_get_cmd_buf(ds_3gpp_ps_wcdma_tbl)) == NULL)
#else
  if ((l2_ul_cmd = wcdma_l2_ul_get_cmd_buf()) == NULL)
#endif  /* FEATURE_SEGMENT_LOADING */

#else

#ifdef FEATURE_SEGMENT_LOADING
    if ((l2_ul_cmd = IWCDMA_l2_ul_get_cmd_buf(ds_3gpp_ps_wcdma_tbl)) == NULL)
#else
  if ((l2_ul_cmd = l2_ul_get_cmd_buf()) == NULL)
#endif /* FEATURE_SEGMENT_LOADING */

#endif /* FEATURE_WCDMA_NEW_L2_INTERFACE */
  {
    DATA_MSG0_ERROR("NULL Ptr when sending RLC_OPEN_RX_WIN");
  }
  else
  {
    rlc_state_ptr->sent_close_to_rlc = FALSE;
    l2_ul_cmd->cmd_hdr.cmd_id = RLC_OPEN_RX_WIN;
    l2_ul_cmd->cmd_data.ul_rlc_data_id = rlc_state_ptr->rlc_ul_id;
#ifdef FEATURE_DUAL_WCDMA
    l2_ul_cmd->cmd_hdr.as_id = pdcp_ctl_blk[pdcp_instance].subs_id;
#endif /* FEATURE_DUAL_WCDMA */
#ifdef FEATURE_WCDMA_NEW_L2_INTERFACE
  #ifdef FEATURE_SEGMENT_LOADING
    IWCDMA_wcdma_l2_ul_put_cmd(ds_3gpp_ps_wcdma_tbl, l2_ul_cmd);
  #else
    wcdma_l2_ul_put_cmd(l2_ul_cmd);
  #endif  /* FEATURE_SEGMENT_LOADING */
#else
  #ifdef FEATURE_SEGMENT_LOADING
    IWCDMA_l2_ul_put_cmd(ds_3gpp_ps_wcdma_tbl, l2_ul_cmd);
#else
    l2_ul_put_cmd(l2_ul_cmd);
  #endif  /* FEATURE_SEGMENT_LOADING */
#endif /* FEATURE_WCDMA_NEW_L2_INTERFACE */
  }
} /* pdcp_rlc_to_pdcp_lo_water_cb */


/*===========================================================================
FUNCTION pdcp_RLC_TO_PDCP_HI_WATER_CB

DESCRIPTION
  This function is called when the RLC to PDCP watermark reaches the high
  watermark. We send the RLC_CLOSE_RX_WIN primitive to RLC so it will in turn
  close the RLC RX window of the UE.
  
PARAMETERS 
   wm            : pointer to the watermark
   callback_data : pdcp instance registered with the watermark callback
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  Causes RLC to close the UE's RX window to one PDU (for this RLC ID)  

===========================================================================*/
void pdcp_rlc_to_pdcp_hi_water_cb
(
  dsm_watermark_type *wm,
  void*              callback_data
)
{
#ifdef FEATURE_WCDMA_NEW_L2_INTERFACE
  wcdma_l2_ul_cmd_type *l2_ul_cmd;                           /* L2 command */
#else
  l2_ul_cmd_type *l2_ul_cmd;                                 /* L2 command */
#endif /* FEATURE_WCDMA_NEW_L2_INTERFACE */
  pdcp_rlc_state_type  *rlc_state_ptr;      /* rlc_info for each pdcp_inst */
  uint8                pdcp_instance;    /* Index into call table for pdcp */
 /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*lint -e734*/
  pdcp_instance = (uint8) ((uint32) callback_data);
  /*lint +e734*/
  
  DATA_MSG2_HIGH("rlc_to_pdcp_hi_water_cb, wm= %d, pdcp_inst = %d", 
                  wm, pdcp_instance);
   /*-------------------------------------------------------------------------
    Get RLC state information for pdcp inst.
  -------------------------------------------------------------------------*/
  PDCP_ENTER_CRIT_SECTION ( &pdcp_crit_section );
  rlc_state_ptr = &(pdcp_ctl_blk[pdcp_instance].rlc_state);
  PDCP_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
  
  /*-------------------------------------------------------------------------
    This call back will be called every time the high threshold is crossed.
    i.e. We may cross the high mark many times w/o crossing the low mark.
    This flag ensures we don't send redundant commands to RLC.
  -------------------------------------------------------------------------*/
  if(rlc_state_ptr->sent_close_to_rlc == TRUE)
  {
    return;
  }

  /*------------------------------------------------------------------------
   Send the command to RLC
  -------------------------------------------------------------------------*/

#ifdef FEATURE_WCDMA_NEW_L2_INTERFACE
  #ifdef FEATURE_SEGMENT_LOADING
    if(ds_3gpp_ps_wcdma_tbl == NULL)
    {
      DATA_MSG0_ERROR("In pdcp_rlc_to_pdcp_hi_water_cb WCDMA not active");
      return;
    }
     if ((l2_ul_cmd = IWCDMA_wcdma_l2_ul_get_cmd_buf(ds_3gpp_ps_wcdma_tbl)) == NULL)
  #else
  if ((l2_ul_cmd = wcdma_l2_ul_get_cmd_buf()) == NULL)
  #endif /* FEATURE_SEGMENT_LOADING */
#else
  #ifdef FEATURE_SEGMENT_LOADING
    if ((l2_ul_cmd = IWCDMA_l2_ul_get_cmd_buf(ds_3gpp_ps_wcdma_tbl)) == NULL)
#else
  if ((l2_ul_cmd = l2_ul_get_cmd_buf()) == NULL)
  #endif /* FEATURE_SEGMENT_LOADING */
#endif /* FEATURE_WCDMA_NEW_L2_INTERFACE */
  {
    DATA_MSG0_ERROR("NULL Ptr when sending RLC_CLOSE_RX_WIN");
  }
  else
  {
    rlc_state_ptr->sent_close_to_rlc = TRUE;
    l2_ul_cmd->cmd_hdr.cmd_id = RLC_CLOSE_RX_WIN;
    l2_ul_cmd->cmd_data.ul_rlc_data_id = rlc_state_ptr->rlc_ul_id;
#ifdef FEATURE_DUAL_WCDMA
    l2_ul_cmd->cmd_hdr.as_id = pdcp_ctl_blk[pdcp_instance].subs_id;
#endif /* FEATURE_DUAL_WCDMA */
      
#ifdef FEATURE_WCDMA_NEW_L2_INTERFACE
  #ifdef FEATURE_SEGMENT_LOADING
    IWCDMA_wcdma_l2_ul_put_cmd(ds_3gpp_ps_wcdma_tbl, l2_ul_cmd);
  #else
    wcdma_l2_ul_put_cmd(l2_ul_cmd);
  #endif /* FEATURE_SEGMENT_LOADING */
#else
  #ifdef FEATURE_SEGMENT_LOADING
    IWCDMA_l2_ul_put_cmd(ds_3gpp_ps_wcdma_tbl, l2_ul_cmd);
#else
    l2_ul_put_cmd(l2_ul_cmd);
  #endif /* FEATURE_SEGMENT_LOADING */
#endif /* FEATURE_WCDMA_NEW_L2_INTERFACE */
  }

}/* pdcp_rlc_to_pdcp_hi_water_cb() */


/*===========================================================================
FUNCTION      PDCP_CLEANUP_RLC_WATERMARKS

DESCRIPTION   
  This function cleans up the watermarks that are owned by data services.

PARAMETERS   
  call_instance - Index into call table for call.
  
DEPENDENCIES  
  None.

RETURN VALUE  
  None.

SIDE EFFECTS  
  None.
===========================================================================*/
void pdcp_cleanup_rlc_watermarks
(
  uint32 pdcp_instance
)
{
#ifdef FEATURE_WCDMA_NEW_L2_INTERFACE
  wcdma_l2_dl_cmd_type        *l2_dl_cmd;
#else
  l2_dl_cmd_type              *l2_dl_cmd;
#endif /* FEATURE_WCDMA_NEW_L2_INTERFACE */
  rlci_cleanup_wm_cmd_type    cleanup_wm_cmd;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_WCDMA_NEW_L2_INTERFACE
  #ifdef FEATURE_SEGMENT_LOADING
    if(ds_3gpp_ps_wcdma_tbl == NULL)
    {
      DATA_MSG0_ERROR("In pdcp_cleanup_rlc_watermarks WCDMA not active");
      return;
    }
    if ((l2_dl_cmd = IWCDMA_wcdma_l2_dl_get_cmd_buf(ds_3gpp_ps_wcdma_tbl)) == NULL)
  #else
  if ((l2_dl_cmd = wcdma_l2_dl_get_cmd_buf()) == NULL)
  #endif /* FEATURE_SEGMENT_LOADING */
#else
  #ifdef FEATURE_SEGMENT_LOADING
    if ((l2_dl_cmd = IWCDMA_l2_dl_get_cmd_buf(ds_3gpp_ps_wcdma_tbl)) == NULL)
#else
  if ((l2_dl_cmd = l2_dl_get_cmd_buf()) == NULL)
  #endif /* FEATURE_SEGMENT_LOADING */
#endif /* FEATURE_WCDMA_NEW_L2_INTERFACE */
  {
	DS_3GPP_MSG3_ERROR("NULL Ptr when sending RLC_UL_REGISTER_SRVC_REQ",0,0,0);
  }
  else 
  {
	cleanup_wm_cmd.cleanup_wm_para = (void *)pdcp_instance;
	cleanup_wm_cmd.cleanup_wm_cb = pdcp_cleanup_rlc_watermarks_cb;
	cleanup_wm_cmd.lc_id = pdcp_ctl_blk[pdcp_instance].rlc_state.rlc_dl_id;

	/*-----------------------------------------------------------------------
	 Register the watermarks with RLC 
	-----------------------------------------------------------------------*/
	l2_dl_cmd->cmd_hdr.cmd_id = RLC_PDCP_CLEANUP_WM;
	l2_dl_cmd->cmd_data.cleanup_wm_cmd = cleanup_wm_cmd;
#ifdef FEATURE_DUAL_WCDMA
    l2_dl_cmd->cmd_hdr.as_id = pdcp_ctl_blk[pdcp_instance].subs_id;
#endif /* FEATURE_DUAL_WCDMA */

#ifdef FEATURE_WCDMA_NEW_L2_INTERFACE
  #ifdef FEATURE_SEGMENT_LOADING
    IWCDMA_wcdma_l2_dl_put_cmd(ds_3gpp_ps_wcdma_tbl, l2_dl_cmd);
  #else
	wcdma_l2_dl_put_cmd(l2_dl_cmd);
  #endif /* FEATURE_SEGMENT_LOADING */
#else
  #ifdef FEATURE_SEGMENT_LOADING
    IWCDMA_l2_dl_put_cmd(ds_3gpp_ps_wcdma_tbl, l2_dl_cmd);
#else
	l2_dl_put_cmd(l2_dl_cmd);
  #endif /* FEATURE_SEGMENT_LOADING */
#endif /* FEATURE_WCDMA_NEW_L2_INTERFACE */
  }
} /* pdcp_cleanup_rlc_watermarks */

/*===========================================================================
FUNCTION pdcp_RLC_TO_PDCP_GONE_EMPTY_CB

DESCRIPTION
  This function forwards pkts received on Um interface to ps_iface for 
  pdp-ip call.
  
PARAMETERS   
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None
     
SIDE EFFECTS 
  None.
  
===========================================================================*/
/*lint -e818 -e715*/
void pdcp_rlc_to_pdcp_gone_empty_cb
(
  dsm_watermark_type *wm,
  void               *callback_data
)
{
  uint8          pdcp_inst;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*lint -e734*/
  PDCP_ENTER_CRIT_SECTION ( &pdcp_crit_section );
  pdcp_inst = (uint8)((uint32) callback_data);
  /*lint +e734*/
  pdcp_clr_data_available_sig( pdcp_inst,
                               pdcp_ctl_blk[pdcp_inst].rlc_to_pdcp_sig);
  PDCP_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
}
/*lint +e818 +e715*/

/*===========================================================================
FUNCTION pdcp_SETUP_RLC_TO_PDCP_WM()

DESCRIPTION
  This function initializes the RLC_PDCP_WM.

PARAMETERS   
  pdcp_inst - Index into call table for call

DEPENDENCIES 
  None.

RETURN VALUE 
  None.

SIDE EFFECTS 
  None.
  
===========================================================================*/
void pdcp_setup_rlc_to_pdcp_wm
(
  uint32 pdcp_inst
)
{
  pdcp_rlc_state_type *rlc_state_ptr;     /* rlc_info for each pdcp_inst */
  sys_modem_as_id_e_type   subs_id = SYS_MODEM_AS_ID_NONE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Get RLC state information for pdcp inst.
  -------------------------------------------------------------------------*/
  rlc_state_ptr = &(pdcp_ctl_blk[pdcp_inst].rlc_state);
  subs_id = pdcp_ctl_blk[pdcp_inst].subs_id;

  /*-------------------------------------------------------------------------
    Set up PS->RLC watermark - used for both PDP-IP & PDP-PPP stacks
  -------------------------------------------------------------------------*/
  dsm_set_low_wm(&(rlc_state_ptr->rlc_rx_wm_item), PDCP_RLC_DL_WM_LO);
  dsm_set_hi_wm(&(rlc_state_ptr->rlc_rx_wm_item), PDCP_RLC_DL_WM_HI);
  dsm_set_dne(&(rlc_state_ptr->rlc_rx_wm_item), PDCP_RLC_DL_WM_DNE);
  dsm_set_dne_q_cnt(&(rlc_state_ptr->rlc_rx_wm_item),PDCP_RLC_DL_WM_DNE_QCNT);

   /*-----------------------------------------------------------------------
     Pass the call instance in watermarks registered so that we
     can identify the call when the callback is called
   -----------------------------------------------------------------------*/
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
#endif /* FEATURE_SEGMENT_LOADING   */
  {
      rlc_state_ptr->rlc_rx_wm_item.lowater_func_ptr      = 
        pdcp_tdrlc_to_pdcp_lo_water_cb;
      rlc_state_ptr->rlc_rx_wm_item.hiwater_func_ptr      = 
        pdcp_tdrlc_to_pdcp_hi_water_cb;
  }
  else 
#endif /* FEATURE_TDSCDMA */
  {
  rlc_state_ptr->rlc_rx_wm_item.lowater_func_ptr      = 
    pdcp_rlc_to_pdcp_lo_water_cb;
  rlc_state_ptr->rlc_rx_wm_item.hiwater_func_ptr      = 
    pdcp_rlc_to_pdcp_hi_water_cb;
  }
  rlc_state_ptr->rlc_rx_wm_item.lowater_func_data     = (void*) pdcp_inst;  
  rlc_state_ptr->rlc_rx_wm_item.hiwater_func_data     = (void*) pdcp_inst;

  rlc_state_ptr->rlc_rx_wm_item.non_empty_func_ptr    = NULL;

  rlc_state_ptr->rlc_rx_wm_item.gone_empty_func_ptr   =
    pdcp_rlc_to_pdcp_gone_empty_cb;
  rlc_state_ptr->rlc_rx_wm_item.gone_empty_func_data   = (void*) pdcp_inst;
    
  rlc_state_ptr->rlc_rx_wm_item.each_enqueue_func_ptr = NULL;

  /*-------------------------------------------------------------------------
    Reset the flag which keeps track if we have sent a close req to RLC
    to tigger flow controlling the peer.
  -------------------------------------------------------------------------*/
  pdcp_reset_sent_close_to_rlc((uint8)pdcp_inst);

} /* pdcp_setup_rlc_to_pdcp_wm */



/*===========================================================================
FUNCTION pdcp_SETUP_PDCP_TO_RLC_WM()

DESCRIPTION
  This function initializes the PDCP_RLC_WM.

PARAMETERS   
  pdcp_inst - Index into call table for call

DEPENDENCIES 
  None.

RETURN VALUE 
  None.

SIDE EFFECTS 
  None.
  
===========================================================================*/
void pdcp_setup_pdcp_to_rlc_wm
(
  uint32 pdcp_inst
)
{
  pdcp_rlc_state_type *rlc_state_ptr;     /* rlc_info for each pdcp_inst */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get RLC state information for pdcp inst.
  -------------------------------------------------------------------------*/
  rlc_state_ptr = &(pdcp_ctl_blk[pdcp_inst].rlc_state);
  
  dsm_set_low_wm(&(rlc_state_ptr->rlc_tx_wm_item), PDCP_RLC_UL_WM_LO);
  dsm_set_hi_wm(&(rlc_state_ptr->rlc_tx_wm_item), PDCP_RLC_UL_WM_HI);
  dsm_set_dne(&(rlc_state_ptr->rlc_tx_wm_item), PDCP_RLC_UL_WM_DNE);
  dsm_set_dne_q_cnt(&(rlc_state_ptr->rlc_tx_wm_item),PDCP_RLC_UL_WM_DNE_QCNT);


   /*-----------------------------------------------------------------------
     Pass the call instance in watermarks registered so that we
     can identify the call when the callback is called
   -----------------------------------------------------------------------*/

  rlc_state_ptr->rlc_tx_wm_item.lowater_func_ptr      = 
    pdcp_pdcp_to_rlc_lo_water_cb;
  rlc_state_ptr->rlc_tx_wm_item.lowater_func_data     = (void*) pdcp_inst;

  rlc_state_ptr->rlc_tx_wm_item.hiwater_func_ptr      = 
    pdcp_pdcp_to_rlc_hi_water_cb;
  rlc_state_ptr->rlc_tx_wm_item.hiwater_func_data     = (void*) pdcp_inst;

  rlc_state_ptr->rlc_tx_wm_item.gone_empty_func_ptr   = NULL;
  rlc_state_ptr->rlc_tx_wm_item.non_empty_func_ptr    = NULL;
  rlc_state_ptr->rlc_tx_wm_item.each_enqueue_func_ptr = NULL;
} /* pdcp_setup_pdcp_to_rlc_wm */


/*===========================================================================
FUNCTION PDCP_FROM_RLC_POST_RX_FUNC_PTR

DESCRIPTION
  When data is enqueued onto RLC_TO_PDCP_WM, RLC invokes this callback and this
  sets the PDCP RX signal.
  
PARAMETERS   
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None
     
SIDE EFFECTS 
  None.
  
===========================================================================*/
void pdcp_from_rlc_post_rx_func_ptr
(
  byte x,
  byte y,
  void *pdcp_inst
)
/*lint -e715*/
{
  uint8 pdcp_instance;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*lint -e734*/
  pdcp_instance = (uint8)((uint32) pdcp_inst);
  /*lint +e734*/
  pdcp_set_data_available_sig( pdcp_instance,
                               pdcp_ctl_blk[pdcp_instance].rlc_to_pdcp_sig);
}
/*lint +e715*/

/*===========================================================================
FUNCTION      PDCP_GET_TRAFFIC_CLASS_SUB

DESCRIPTION   
  This function returns Traffic Class for given context.

PARAMETERS   
  subs_id   - CM subs id  
  rlc_dl_id - DL LC id for context .
  
DEPENDENCIES  
  None.

RETURN VALUE  
  ds_umts_qos_tclass_enum_type -- DL Traffic Class for the context.

SIDE EFFECTS  
  None.
===========================================================================*/
ds_umts_qos_tclass_enum_type  pdcp_get_traffic_class_sub
(
  sys_modem_as_id_e_type subs_id,
  rlc_lc_id_type         rlc_dl_id
)
{
  ds_umts_qos_tclass_enum_type tfclass =   DS_UMTS_QOS_TCLASS_RESERVED;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#if (defined(FEATURE_DATA_WCDMA_PS)&&defined(FEATURE_CPU_BASED_FLOW_CONTROL))
  pdcp_rlc_state_type         *rlc_state_ptr;
  uint32                       pdcp_inst;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*Validate subscription id*/
  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    DS_3GPP_MSG2_HIGH("Invalid Subs id(CM):%d in pdcp_get_traffic_class_sub "
                       "with rlc_dl_id:%d",subs_id, rlc_dl_id);
    return tfclass;
  }  

  PDCP_ENTER_CRIT_SECTION ( &pdcp_crit_section );
  for ( pdcp_inst = 0; pdcp_inst < MAX_PDCP_INST; pdcp_inst++ ) 
  {  
    /*-----------------------------------------------------------------------
      Get RLC state information for the call.
    -----------------------------------------------------------------------*/
    rlc_state_ptr = &(pdcp_ctl_blk[pdcp_inst].rlc_state);
    if ( (pdcp_ctl_blk[pdcp_inst].subs_id == subs_id) && 
         (rlc_state_ptr->rlc_dl_id == rlc_dl_id) ) 
	{
      PDCP_LEAVE_CRIT_SECTION ( &pdcp_crit_section );  
	  return pdcp_ctl_blk[pdcp_inst].traf_class;
	}
  }
  PDCP_LEAVE_CRIT_SECTION ( &pdcp_crit_section );
#else
  DATA_MSG0_ERROR("pdcp_get_traffic_class() is not supported");
#endif

  return tfclass;
}

/*===========================================================================
FUNCTION      PDCP_GET_TRAFFIC_CLASS

DESCRIPTION   
  This function returns Traffic Class for given context.

PARAMETERS   
  rlc_dl_id - DL LC id for context .
  
DEPENDENCIES  
  None.

RETURN VALUE  
  ds_umts_qos_tclass_enum_type -- DL Traffic Class for the context.

SIDE EFFECTS  
  None.
===========================================================================*/
ds_umts_qos_tclass_enum_type pdcp_get_traffic_class
(
  rlc_lc_id_type rlc_dl_id
)
{
  sys_modem_as_id_e_type     subs_id = ds3g_get_ps_subs_id();
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return pdcp_get_traffic_class_sub(subs_id, rlc_dl_id);
}

#endif /* FEATURE_DATA_WCDMA_PS */


