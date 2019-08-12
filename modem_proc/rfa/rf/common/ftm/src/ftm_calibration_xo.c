/*!
  @file
  ftm_calibration_xo.c

  @brief
  This module contains FTM common XO Calibration code which is used to
  dispatch xo calibration commands to mcs tcxo manager.

  @addtogroup RF_COMMON_FTM_CALIBRATION_XO
  @{
*/

/*==============================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/src/ftm_calibration_xo.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
03/02/13    dw     Skip WCDMA retune for XO Cal
11/01/13    dw     Remove tech RxLM dellocation in XO RxLM allocation
08/12/13   sar     Fixed KW errors.
03/11/13   sar     Updated file for APQ, GNSS only target.
01/21/13    dj     Removing technology exit mode from ftm_xo_cal_exit_mode
12/19/12    dj     Added GSM case to ftm_xo_cal_allocate_rxlm
11/15/12    dj     Fix for some compiler warnings
11/15/12    dj     Offtarget compile fix
11/15/12    dj     Initial Release

==============================================================================*/

#include "rfa_variation.h"
#include "target.h"    /* Target specific definitions            */
#include "comdef.h"    /* Common stuff                           */
#include "ftmicap.h"

#include "ftm.h"
#include "ftm_common.h"
#include "ftm_msg.h"
#include "rxlm_intf.h"
#include "ftm_common_control.h"

#include "DALSys.h"
#include "timetick.h"

#ifdef FEATURE_WCDMA
#include "ftm_wcdma_ctl.h"
#endif

#include "ftm_calibration_xo.h"

/*!< Variable to Keep track of the current chain on which XO Cal is being done*/
/*Note: Currently XO Cal is supported only one device and primary chain*/
static ftm_receive_chain_handle_type ftm_xo_cal_curr_chain =  FTM_RECEIVE_CHAIN_0;

/*!< Variable to keep track of the RXLM XO Cal buffers */
static ftm_lm_buf_type ftm_xo_cal_rxlm_buffer[2]= {{0, FALSE},{0,FALSE},};

/*!< XO Factory Cal parameters */
tcxomgr_cal_fac_param_type ftm_fac_cal_params;

/*!< XO Calibration informational structure */
tcxomgr_cal_info_type ftm_info_pkt;

/*!< XO Calibration FT samples */
tcxomgr_cal_ft_sample_type ftm_samples;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  ftm_xo_calibration

  @details
  This function will call the functions corresponding to the requested XO
  cal action

  @param ftm_req_data
  Pointer to the FTM request packet

  @return
  FTM response packet
*/

ftm_rsp_pkt_type
ftm_xo_calibration
(
   void *ftm_req_data
)
{
  ftm_xo_cal_generic_request_type* req;
  ftm_rsp_pkt_type ftm_rsp_data;
  ftm_xo_cal_generic_response_type* rsp_pkt;

  ftm_rf_technology_type ftm_rf_tech;

  req = (ftm_xo_cal_generic_request_type *)ftm_req_data;
  
  ftm_rsp_data = ftm_common_malloc_rsp_pkt( ftm_req_data,
                                 sizeof(ftm_xo_cal_generic_response_type));

  rsp_pkt = (ftm_xo_cal_generic_response_type *)ftm_rsp_data.pkt_payload;
  
  ftm_rf_tech = ftm_get_rf_technology();

  /* Send back a BAD response packet for failures */
  if ( rsp_pkt == NULL )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG( FTM_ERROR, "XO Calibration : Failed during malloc for rsp_pkt" );
    return ftm_rsp_data;
  } /* if( rsp_pkt == NULL ) */
  
  /* Before XO Cal starts, prepare the phone for XO Cal state */
  if( !ftm_xo_cal_enter_mode(ftm_rf_tech, ftm_xo_cal_curr_chain) )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_MODE;
    /* Return as phone cannot be configured for XO Cal */
    FTM_MSG(FTM_ERROR,"xo_cal_enter_mode failed!");
    return ftm_rsp_data;
  }
  else
  {
    /* Check for supported version */
    if( req->version != 3 )
    {
      ftm_rsp_data.cmd = FTM_RSP_BAD_CMD;
      FTM_MSG_1(FTM_ERROR,"XO Cal: Unknown version: %d", req->version);
    }
    else
    {
      /* Dispatch XO Cal Command */
      switch (req->xo_cal_command)
      {
      case 1:
        FTM_MSG(FTM_HIGH,"Dispatching XO Cal command: DC Cal");
        ftm_xo_cal_dc_coarse_cal(req, rsp_pkt);
        break;
      case 2:
        FTM_MSG(FTM_HIGH,"Dispatching XO Cal command: Collect Samples");
        ftm_xo_cal_collect_sample(req, rsp_pkt);
        break;
      case 3:
        FTM_MSG(FTM_HIGH,"Dispatching XO Cal command: FT Curve Calibration");
        ftm_xo_cal_ft_curve_cal(req, rsp_pkt);
        break;
      default:
        ftm_rsp_data.cmd = FTM_RSP_BAD_CMD;
        FTM_MSG_1(FTM_ERROR,"XO Cal: Unknown command: %d", req->xo_cal_command);
      }

      FTM_MSG(FTM_HIGH,"XO Cal command: Completed");
    }

    /* XO cal has completed, exit XO Cal state */
    ftm_xo_cal_exit_mode( ftm_rf_tech, ftm_xo_cal_curr_chain );
  }

  return ftm_rsp_data;
} /* ftm_xo_calibration */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  ftm_xo_cal_dc_coarse_cal

  @details
  This function will call the tcxomgr functions required to do xo dc
  coarse cal

  @param req
  Pointer to the FTM request packet

  @param rsp_pkt
  Pointer to the FTM response packet

  @return
  None
*/

void
ftm_xo_cal_dc_coarse_cal
(
   ftm_xo_cal_generic_request_type* req,
   ftm_xo_cal_generic_response_type* rsp_pkt
)
{

  #ifndef FEATURE_GNSS_ONLY_NO_WWAN
  /* Setting the state to NOT_CAL prior to cal */
  ftm_fac_cal_params.state = TCXOMGR_CAL_STATE_NOT_CAL;

  /* Initializing TXCO manager structures */
  tcxomgr_cal_param_init(&ftm_fac_cal_params, &ftm_samples, &ftm_info_pkt);

  /* Populate TCXO Manager Structure with calibration parameters from FTM */
  ftm_xo_populate_cal_params(&(req->ftm_xo_cal_parameter), &ftm_fac_cal_params, ftm_xo_cal_curr_chain);

  /* Calling the TCXO Manager API for coarse DC Cal */
  tcxomgr_cal_dc_coarse(&ftm_fac_cal_params, &ftm_samples, &ftm_info_pkt);

  ftm_xo_populate_response(&ftm_fac_cal_params, &ftm_info_pkt, rsp_pkt);

  #endif /*FEATURE_GNSS_ONLY_NO_WWAN */
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  ftm_xo_cal_collect_sample

  @details
  This function will call the tcxomgr functions required to do xo collect
  sample

  @param req
  Pointer to the FTM request packet

  @param rsp_pkt
  Pointer to the FTM response packet

  @return
  None
*/

void
ftm_xo_cal_collect_sample
(
   ftm_xo_cal_generic_request_type* req,
   ftm_xo_cal_generic_response_type* rsp_pkt
)
{

  #ifndef FEATURE_GNSS_ONLY_NO_WWAN
  /* Populate TCXO Manager Structure with calibration parameters from FTM */
  ftm_xo_populate_cal_params(&(req->ftm_xo_cal_parameter), &ftm_fac_cal_params, ftm_xo_cal_curr_chain);

  /* Call XO Manager API to capture samples */
  tcxomgr_cal_samp(&ftm_fac_cal_params, &ftm_samples, &ftm_info_pkt);

  ftm_xo_populate_response(&ftm_fac_cal_params, &ftm_info_pkt, rsp_pkt);
  #endif /* FEATURE_GNSS_ONLY_NO_WWAN */
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  ftm_xo_cal_ft_curve_cal

  @details
  This function will call the tcxomgr functions required to do xo ft
  curve cal.

  @param req
  Pointer to the FTM request packet

  @param rsp_pkt
  Pointer to the FTM response packet

  @return
  None
*/

void
ftm_xo_cal_ft_curve_cal
(
   ftm_xo_cal_generic_request_type* req,
   ftm_xo_cal_generic_response_type* rsp_pkt
)
{

  #ifndef FEATURE_GNSS_ONLY_NO_WWAN
  /* Populate TCXO Manager Structure with calibration parameters from FTM */
  ftm_xo_populate_cal_params(&(req->ftm_xo_cal_parameter), &ftm_fac_cal_params, ftm_xo_cal_curr_chain);

  /* Call XO Manager APIs */
  /* Do one FE capture here */
  tcxomgr_cal_samp(&ftm_fac_cal_params, &ftm_samples, &ftm_info_pkt);
  tcxomgr_cal_c1c2c3(&ftm_fac_cal_params, &ftm_samples, &ftm_info_pkt);
  tcxomgr_cal_dc_fine(&ftm_fac_cal_params, &ftm_samples, &ftm_info_pkt);

  ftm_xo_populate_response(&ftm_fac_cal_params, &ftm_info_pkt, rsp_pkt);
  #endif /* FEATURE_GNSS_ONLY_NO_WWAN */
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  ftm_xo_cal_enter_mode

  @details
  This function will request RXLM XO buffers for the given technologhy &
  chain

  @param ftm_rf_tech
  Technology in which the phone is operating in (CDMA, WCDMA, LTE, etc.)

  @param chain
  Receive chain being used

  @return
  Boolean Pass/Fail
*/

boolean
ftm_xo_cal_enter_mode
(
   ftm_rf_technology_type ftm_rf_tech,
   ftm_receive_chain_handle_type chain
)
{
  boolean ret_val = FALSE; /* Return Value */

  /* Enter mode allowed on Chain 0 or Chain 1 only*/
  if(!((chain == FTM_RECEIVE_CHAIN_0) || (chain == FTM_RECEIVE_CHAIN_1)))
  {
    FTM_MSG_1( FTM_ERROR, "ftm_xo_cal_enter_mode : Invalid chain %d",chain );
    return ret_val;
  }
  /* Get the XO Cal RXLM buffer for the current technology and if successful
  configure the new buffer thru tech specific implementation */
  if ( ftm_xo_cal_allocate_rxlm( ftm_rf_tech, chain) )
  {
    /* Tech specific routine to commit the XO Cal RxLM buffer */
    switch( ftm_rf_tech )
    {
    case FTM_RF_TECH_CDMA:
      #ifdef FEATURE_CDMA1X
      /* Do nothing for 1x, SUITE should have already enforced the Band,Chan */
      ret_val = TRUE;
      #endif
      break;

    case FTM_RF_TECH_LTE:
      #ifdef FEATURE_LTE
      /* Do nothing for LTE, xo cal mode is the same as LTE ftm cal mode */
      ret_val = TRUE;
      #endif
      break;

    case FTM_RF_TECH_WCDMA:
      #ifdef FEATURE_WCDMA
      /* Do nothing for WCDMA, xo cal mode is the same as tds ftm cal mode */
      ret_val = TRUE;
      #endif
      break;

    case FTM_RF_TECH_TDSCDMA:
      #ifdef FTM_HAS_TDSCDMA
      /* Do nothing for TDSCDMA, xo cal mode is the same as tds ftm cal mode */
      ret_val = TRUE;
      #endif
      break;

    case FTM_RF_TECH_GSM:
      #ifdef FEATURE_GSM
      /* Do nothing for GSM, xo cal mode is the same as gsm ftm cal mode */
      ret_val = TRUE;
      #endif
      break;

    default:
      ret_val = FALSE; /* Flag as failure */
      FTM_MSG_1( FTM_ERROR, "ftm_xo_cal_enter_mode : Invalid Mode %d",
                 ftm_rf_tech );
      break;
    }
  }

  /* Return the status */
  return ret_val;
} /*ftm_xo_cal_enter_mode*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  ftm_xo_cal_exit_mode

  @details
  This function will request deallocation of RXLM XO buffers for the given
  technologhy & chain.

  @param ftm_rf_tech
  Technology in which the phone is operating in (CDMA, WCDMA, LTE, etc.)

  @param chain
  Receive chain being used

  @return
  None
*/

void
ftm_xo_cal_exit_mode
(
   ftm_rf_technology_type ftm_rf_tech,
   ftm_receive_chain_handle_type chain
)
{
  /* Deallocate the XO Cal buffer */
  ftm_xo_cal_deallocate_rxlm( ftm_rf_tech );
}/*ftm_xo_cal_exit_mode*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  ftm_xo_cal_allocate_rxlm

  @details
  This function will request RXLM XO buffers for the given technologhy & chain

  @param ftm_rf_tech
  Technology in which the phone is operating in (CDMA, WCDMA, LTE, etc.)

  @param chain
  Receive chain being used

  @return
  Boolean Pass/Fail
*/

boolean
ftm_xo_cal_allocate_rxlm
(
   ftm_rf_technology_type ftm_rf_tech,
   ftm_receive_chain_handle_type chain
)
{
  lm_status_type status;

  /* Only Chain 0 or Chain 1 allowed */
  if(!((chain == FTM_RECEIVE_CHAIN_0) || (chain == FTM_RECEIVE_CHAIN_1)))
  {
    FTM_MSG_1( FTM_ERROR, "ftm_xo_cal_allocate_rxlm : Invalid chain %d",chain );
    return FALSE;
  }

  /* Request a new RxLM buffer for XO Cal*/
  /* No tech currently needs to allocate XO buffer - disable this for now  */
  if ( (!ftm_xo_cal_rxlm_buffer[chain].is_buf_allocated)  && ( ftm_rf_tech == FTM_RF_TECH_UNKNOWN) )
  {
    status = rxlm_allocate_buffer((rxlm_chain_type)chain, LM_UMTS, &(ftm_xo_cal_rxlm_buffer[chain].buf_idx));
    if (status != LM_SUCCESS )
    {
       MSG_ERROR("XO Cal RxLM allocate buffer failed, status:%d",status,0,0);
       return FALSE;
    }
    else
    {
       MSG_HIGH("XO Cal RXLM buffer allocated for chain:%d, buffer index:%d",chain,ftm_xo_cal_rxlm_buffer[chain].buf_idx,0);
    }
    ftm_xo_cal_rxlm_buffer[chain].is_buf_allocated = TRUE;
  }

  return TRUE;
} /*ftm_xo_cal_allocate_rxlm*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  ftm_xo_cal_deallocate_rxlm

  @details
  This function will request RXLM XO buffers for the given technologhy & chain

  @param ftm_rf_tech
  Technology in which the phone is operating in (CDMA, WCDMA, LTE, etc.)

  @return
  None
*/

void
ftm_xo_cal_deallocate_rxlm
(
   ftm_rf_technology_type ftm_rf_tech
)
{
  uint8 i;
  lm_status_type status;
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
  for (i=0; i<2; i++)
  {
    if (ftm_xo_cal_rxlm_buffer[i].is_buf_allocated)
    {
      status = rxlm_deallocate_buffer(ftm_xo_cal_rxlm_buffer[i].buf_idx);
      if (status != LM_SUCCESS )
      {
        MSG_ERROR("XO Cal RxLM allocate buffer failed, status:%d",status,0,0);
      }
      else
      {
        MSG_HIGH("XO Cal RXLM buffer deallocated for chain:%d",i,0,0);
        ftm_xo_cal_rxlm_buffer[i].is_buf_allocated = FALSE;
      }
    }
  }
#endif /* FEATURE_GNSS_ONLY_NO_WWAN */
} /*ftm_xo_cal_deallocate_rxlm*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  ftm_xo_populate_cal_params

  @details
  This function is a XO Cal helper function and populates XO Cal params to
  the TCXO Manager Structure.

  @param p_params
  Pointer to FTM XO Cal parameter structure

  @param p_cal_params
  Pointer to TCXO Manager XO Cal parameter structure

  @param chain
  Receive chain being used

  @return
  None
*/

void
ftm_xo_populate_cal_params
(
   ftm_xo_cal_parameter_type* p_params,
   tcxomgr_cal_fac_param_type* p_cal_params,
   ftm_receive_chain_handle_type chain
)
{

  /* Only Chain 0 or Chain 1 allowed */
  if(!((chain == FTM_RECEIVE_CHAIN_0) || (chain == FTM_RECEIVE_CHAIN_1)))
  {
    FTM_MSG_1(FTM_ERROR,"ftm_xo_populate_cal_params : Invalid chain %d",chain );
    return;
  }
  /* Parameters for the TCXO manager APIs */
  p_cal_params->tech = (tcxomgr_cal_xo_cal_tech)p_params->technology;
  p_cal_params->rx_freq_khz = p_params->rx_freq_khz;
  p_cal_params->offset = p_params->offset;
  p_cal_params->temp_span = p_params->temp_span;
  p_cal_params->min_ft_sample = p_params->min_ft_samples;
  p_cal_params->max_ft_sample = p_params->max_ft_samples;
  p_cal_params->substage_time = p_params->substage_time;
  p_cal_params->update_nv = (tcxomgr_cal_record_enum_type)p_params->update_nv;
  p_cal_params->timeout_time = p_params->timeout_time;
  p_cal_params->xtal = (tcxomgr_cal_xtal_enum_type)p_params->xtal;
  p_cal_params->temp_grad_timeout = p_params->temp_grad_timeout;
  p_cal_params->wb_id = p_params->wb_id;

  if(ftm_xo_cal_rxlm_buffer[chain].is_buf_allocated)
  {
    p_cal_params->rxlm_buffer_id = ftm_xo_cal_rxlm_buffer[chain].buf_idx;
  }
}/*ftm_xo_populate_cal_params*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  ftm_xo_populate_response

  @details
  This function is a XO Cal helper function and populates response packet
  with results.

  @param p_cal_params
  Pointer to TCXO Manager XO Cal parameter structure

  @param p_cal_info
  Pointer to the TCXO Manager factory calibration informational
  structure

  @param p_rsp
  Pointer to the FTM response packet

  @return
  None
*/

void
ftm_xo_populate_response
(
   tcxomgr_cal_fac_param_type* p_cal_params,
   tcxomgr_cal_info_type* p_cal_info,
   ftm_xo_cal_generic_response_type *p_rsp
)
{
  /* Populate response packet with information from XO manager */
  p_rsp->state = p_cal_params->state;
  p_rsp->nom_coef = p_cal_params->nom_coef;
  p_rsp->cal_coef= p_cal_params->cal_coef;
  p_rsp->tcxomgr_cal_info_pkt = *p_cal_info;
} /*ftm_xo_populate_response*/

/*! @} */
