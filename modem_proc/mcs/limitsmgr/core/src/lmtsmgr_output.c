
/*!
  @file
  lmtsmgr_output.c

  @brief
  This file implements the functions that communicate the power limits to
  the different tech RF modules

  @ingroup per_implementation
*/

/*=============================================================================

  Copyright (c) 2013-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

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

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/core/src/lmtsmgr_output.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/04/14   ag      Added TTL markers for profiling
10/27/14   rj      Cleanup SAR related processing
09/10/14   jm      Band avoidance support for 1X/HDR/WCDMA/TDSCDMA
08/12/14   jm      Slot level timeline support
07/22/14   rj      Cleanup FilterPath related BOLT feature in Lmtsmgr
05/02/14   rj      Filter Path support to send GSM Tx specific flag info to RF
04/28/14   jm      L+G Band Avoidance support
03/17/14   jm      WWAN coex based off of RF device assignment
02/05/14   jm      Slot level backoff support for SGLTE/SGTDS + G
11/06/13   rj      Adding prob_aggr_mask support
09/19/13   rj      Adding support for GSM to ASID mapping
06/13/13   rj      SVLTE/SVDO Coex for Triton/DI2.0
04/23/13   rj      Fix for sending pwr limits to GSM and WCDMA
04/06/13   ag      Support for DSDA WWAN Coexistence
02/20/13   rj      Removing Feature FEATURE_LIMITSMGR_ENABLE_OUTPUT
01/10/12   rj      Fixed Compilation error
09/24/12   ag      Initial Revision

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "mcs_variation.h"
#include "comdef.h"
#include "stringl.h"
#include "wwan_coex_mgr.h"
#include "lmtsmgr_task.h"
#include "lmtsmgr_msgr.h"
#include "lmtsmgr_i.h"
#ifdef FEATURE_LTE
#include "rflte_msg.h"
#endif
#include "rf_tdscdma_msg.h"
#include "rfgsm_msg.h"
#ifdef FEATURE_CDMA
#include "rfm_hdr_umid.h"
#include "rfm_1x_umid.h"
#endif
#include "rfwcdma_msg.h"
#include "rfm_umid_list.h"
#include "sar_i.h"

/*=============================================================================

                            TYPEDEFS

=============================================================================*/
#define LMTSMGR_MAX_PLIMIT 32000


/*===========================================================================

                    INTERNAL FUNCTION DEFINITIONS

===========================================================================*/

#ifdef FEATURE_LTE
/*===========================================================================

  FUNCTION:  lmtsmgr_msgr_lte_pwrlim_trans

===========================================================================*/
/*!
  @brief
	Sends power limits to LTE RF over MSGR.

  @return
    None
*/
/*=========================================================================*/
void lmtsmgr_msgr_lte_pwrlim_trans ( void )
{
  /* struct to be filled and sent out */
  rfa_rf_lte_set_tx_plimit_ind_s transmit_limits;

  int16 sar_plimit, coex_plimit, coex_slot_plimit;

  sar_plimit = lmtsmgr.tech_state[CXM_TECH_LTE].pwr_info.curr_limits.sar_plimit[0];
  coex_plimit = lmtsmgr.tech_state[CXM_TECH_LTE].pwr_info.curr_limits.coex_plimit;
  coex_slot_plimit = lmtsmgr.tech_state[CXM_TECH_LTE].pwr_info.curr_limits.coex_slot_plimit;

  memset(&transmit_limits, 0, sizeof(rfa_rf_lte_set_tx_plimit_ind_s));

  /* Set the limits */
  transmit_limits.sg_txpl_params.plimit_type_mask = ( RFA_RF_LTE_ST_SAR_PLIMIT_MASK | 
                                                      RFA_RF_LTE_WWAN_COEX_PLIMIT_MASK | 
                                                      RFA_RF_LTE_SUBFRAME_COEX_PLIMIT_MASK );
  transmit_limits.sg_txpl_params.sar_plimit = sar_plimit;
  transmit_limits.sg_txpl_params.subframe_coex_plimit = coex_slot_plimit;
  transmit_limits.sg_txpl_params.coex_plimit = coex_plimit;

  /* Message indicating we are sending */
  LMTSMGR_MSG_4(HIGH, "Sending limits %d (sar) %d (coex) %d (subframe) to tech %d",
        	        transmit_limits.sg_txpl_params.sar_plimit,
  			          transmit_limits.sg_txpl_params.coex_plimit,
                  transmit_limits.sg_txpl_params.subframe_coex_plimit,
                  CXM_TECH_LTE );

  lmtsmgr_msgr_send_msg( &transmit_limits.hdr,
    			RFA_RF_LTE_SET_TX_PLIMIT_IND,
  			sizeof(rfa_rf_lte_set_tx_plimit_ind_s) );
}
#endif

/*===========================================================================

  FUNCTION:  lmtsmgr_msgr_tdscdma_pwrlim_trans

===========================================================================*/
/*!
  @brief
    Sends power limits to TDSCDMA RF over MSGR.

  @return
    None
*/
/*=========================================================================*/

void lmtsmgr_msgr_tdscdma_pwrlim_trans (void)
{
  /* struct to be filled and sent out */
  rfa_tdscdma_set_coex_tx_pwr_limit_req_s transmit_limits;

  int16 sar_plimit, coex_plimit;

  sar_plimit = lmtsmgr.tech_state[CXM_TECH_TDSCDMA].pwr_info.curr_limits.sar_plimit[0];
  coex_plimit = MIN(lmtsmgr.tech_state[CXM_TECH_TDSCDMA].pwr_info.curr_limits.coex_plimit,
                    lmtsmgr.tech_state[CXM_TECH_TDSCDMA].pwr_info.curr_limits.coex_slot_plimit);

  /* Initially assume the limits have not changed from previous */
  memset(&transmit_limits, 0, sizeof(rfa_tdscdma_set_coex_tx_pwr_limit_req_s));
  transmit_limits.sar_tx_pwr_limit = sar_plimit;
  transmit_limits.coex_tx_pwr_limit = coex_plimit;

  /* Message indicating we are sending */
  LMTSMGR_MSG_3(HIGH, "Sending limits %d %d to tech %d",
  			          transmit_limits.sar_tx_pwr_limit,
  			          transmit_limits.coex_tx_pwr_limit,
                  CXM_TECH_TDSCDMA );

  lmtsmgr_msgr_send_msg( &transmit_limits.hdr,
     			           RFA_RF_TDSCDMA_SET_COEX_TX_PWR_LIMIT_REQ,
  				           sizeof(rfa_tdscdma_set_coex_tx_pwr_limit_req_s) );
}

/*===========================================================================

  FUNCTION:  lmtsmgr_msgr_hdr_pwrlim_trans

===========================================================================*/
/*!
  @brief
    Sends power limits to HDR RF over MSGR.

  @return
    None
*/
/*=========================================================================*/

void lmtsmgr_msgr_hdr_pwrlim_trans(void)
{
#ifdef FEATURE_CDMA
/* struct to be filled and sent out */
  rfa_hdr_set_coex_tx_pwr_limit_req_s transmit_limits;

  int16 sar_plimit, coex_plimit;

  sar_plimit = lmtsmgr.tech_state[CXM_TECH_HDR].pwr_info.curr_limits.sar_plimit[0];
  coex_plimit = lmtsmgr.tech_state[CXM_TECH_HDR].pwr_info.curr_limits.coex_plimit;

  /* Initially assume the limits have not changed from previous */
  memset(&transmit_limits, 0, sizeof(rfa_hdr_set_coex_tx_pwr_limit_req_s));

  transmit_limits.sar_tx_pwr_limit = sar_plimit;
  transmit_limits.coex_tx_pwr_limit = coex_plimit;

  /* Set the RF device */
  transmit_limits.device = lmtsmgr.tech_state[CXM_TECH_HDR].currList.links[0].freqInfo.link_info.rf_device;

  /* Message indicating we are sending */
  LMTSMGR_MSG_3(HIGH, "Sending limits %d %d to tech %d",
  			          transmit_limits.sar_tx_pwr_limit,
  			          transmit_limits.coex_tx_pwr_limit,
                  CXM_TECH_HDR );

  lmtsmgr_msgr_send_msg( &transmit_limits.hdr,
     			           RFA_RF_HDR_SET_COEX_TX_PWR_LIMIT_REQ,
  				           sizeof(rfa_hdr_set_coex_tx_pwr_limit_req_s) );
#endif
}

/*===========================================================================

  FUNCTION:  lmtsmgr_msgr_1x_pwrlim_trans

===========================================================================*/
/*!
  @brief
    Sends power limits to 1X RF over MSGR.

  @return
    None
*/
/*=========================================================================*/

void lmtsmgr_msgr_1x_pwrlim_trans(void)
{
#ifdef FEATURE_CDMA
/* struct to be filled and sent out */
  rfa_1x_set_coex_tx_pwr_limit_req_s transmit_limits;

  int16 sar_plimit, coex_plimit;

  sar_plimit = lmtsmgr.tech_state[CXM_TECH_ONEX].pwr_info.curr_limits.sar_plimit[0];
  coex_plimit = lmtsmgr.tech_state[CXM_TECH_ONEX].pwr_info.curr_limits.coex_plimit;

  /* Initially assume the limits have not changed from previous */
  memset(&transmit_limits, 0, sizeof(rfa_1x_set_coex_tx_pwr_limit_req_s));

  transmit_limits.sar_tx_pwr_limit = sar_plimit;
  transmit_limits.coex_tx_pwr_limit = coex_plimit;

  /* Set the RF device */
  transmit_limits.device = lmtsmgr.tech_state[CXM_TECH_ONEX].currList.links[0].freqInfo.link_info.rf_device;

  /* Message indicating we are sending */
  LMTSMGR_MSG_3(HIGH, "Sending limits %d %d to tech %d",
  			          transmit_limits.sar_tx_pwr_limit,
  			          transmit_limits.coex_tx_pwr_limit,
                  CXM_TECH_ONEX );

  lmtsmgr_msgr_send_msg( &transmit_limits.hdr,
     			           RFA_RF_1X_SET_COEX_TX_PWR_LIMIT_REQ,
  				           sizeof(rfa_1x_set_coex_tx_pwr_limit_req_s) );
#endif
}

/*===========================================================================

  FUNCTION:  lmtsmgr_msgr_gsm_pwrlim_trans

===========================================================================*/
/*!
  @brief
    Sends power limits to GSM RF over MSGR.

  @return
    None
*/
/*=========================================================================*/

void lmtsmgr_msgr_gsm_pwrlim_trans(cxm_tech_type tech_id)
{
/* struct to be filled and sent out */
  rfa_rf_gsm_set_sar_limit_ind_s transmit_limits;
  int16 sar_plimit[SAR_GSM_MAX_LIMIT_TYPE];
  uint8 num_valid_sar, index;

  /* Update global state */
  memscpy( &sar_plimit, 
           SAR_GSM_MAX_LIMIT_TYPE*sizeof(int16),
           &lmtsmgr.tech_state[tech_id].pwr_info.curr_limits.sar_plimit, 
           SAR_GSM_MAX_LIMIT_TYPE*sizeof(int16));
  num_valid_sar = lmtsmgr.tech_state[tech_id].pwr_info.curr_limits.num_valid_sar;

  /* Initially assume the limits have not changed from previous */
  memset(&transmit_limits, 0, sizeof(rfa_rf_gsm_set_sar_limit_ind_s));

  /* Set the RF device */
  transmit_limits.rfa_rf_gsm_sar_limit_params.device = lmtsmgr.tech_state[tech_id].currList.links[0].freqInfo.link_info.rf_device;;

  /* Update limits */
  for (index = 0; index < num_valid_sar; index++)
  {
    if (sar_plimit[index] != DEFAULT_MAX_PLIMIT )
    {
    
      /* Converting dB10 to dB100 
          * X dB10 = 10*X dB100 
          * If limit in EFS = X dB10 
          * Limit if 4UL slots in a frame = X db10 (x1 mW ==> X = 10 log (x1) ) or 10X dB100
          * Limits if 1UL slot in a frame = Z db10 (= z1 mW) or z1/8 = 4x1/8 or z1 = 4*x1 or
                                                  Z = 10 log (z1) or (X + 10log4) dB10 or (10X + 200log2) dB100 
          * Limit if 2UL slots in a frame = Z db10 or 2*z1/8 = 4*x1/8 or z1 = 4*x1/2 or 2*x1
                                                  Z = (X + 10log2) dB10 or (10X + 100log2) dB100
          * Limit if 3UL slots in a frame = Z db10 or 3*z1/8 = 4*x1/8 or z1 = 4*x1/3 or
                                                  Z = (X + 20log2 - 10log3) dB10 or (10X + 200log2 - 100log3) dB100
          **/
  
      /* 1UL slot in a frame */
      transmit_limits.rfa_rf_gsm_sar_limit_params.sar_tx_limit[index][0] = 
          10*sar_plimit[index] + LMTSMGR_NEAREST_INT_ROUND_OFF((2*LMTSMGR_1000_LOG_2_VALUE));
      /* 2UL slot in a frame */
      transmit_limits.rfa_rf_gsm_sar_limit_params.sar_tx_limit[index][1] = 
          10*sar_plimit[index] + LMTSMGR_NEAREST_INT_ROUND_OFF(LMTSMGR_1000_LOG_2_VALUE);
      /* 3UL slot in a frame */
      transmit_limits.rfa_rf_gsm_sar_limit_params.sar_tx_limit[index][2] = 
          10*sar_plimit[index] + LMTSMGR_NEAREST_INT_ROUND_OFF(((2*LMTSMGR_1000_LOG_2_VALUE) - LMTSMGR_1000_LOG_3_VALUE));
      /* 4UL slot in a frame */
      transmit_limits.rfa_rf_gsm_sar_limit_params.sar_tx_limit[index][3] = 10*sar_plimit[index];
    }
    else
    {
      /* 1UL slot in a frame */
      transmit_limits.rfa_rf_gsm_sar_limit_params.sar_tx_limit[index][0] = DEFAULT_MAX_PLIMIT;
      /* 2UL slot in a frame */
      transmit_limits.rfa_rf_gsm_sar_limit_params.sar_tx_limit[index][1] = DEFAULT_MAX_PLIMIT;
      /* 3UL slot in a frame */
      transmit_limits.rfa_rf_gsm_sar_limit_params.sar_tx_limit[index][2] = DEFAULT_MAX_PLIMIT;
      /* 4UL slot in a frame */
      transmit_limits.rfa_rf_gsm_sar_limit_params.sar_tx_limit[index][3] = DEFAULT_MAX_PLIMIT;
    }
  }


  /* Message indicating we are sending */
  LMTSMGR_MSG_3(HIGH, "Sending limits %d %d to tech %d",
  			          transmit_limits.rfa_rf_gsm_sar_limit_params.sar_tx_limit[0][0],
  			          transmit_limits.rfa_rf_gsm_sar_limit_params.sar_tx_limit[1][0],
                      tech_id );

  if (tech_id == CXM_TECH_GSM1)
  {
    lmtsmgr_msgr_send_msg( &transmit_limits.hdr,
     			           RFA_RF_GSM_SET_SAR_LIMIT_IND,
  				           sizeof(rfa_rf_gsm_set_sar_limit_ind_s) );
  }
  else if ((tech_id == CXM_TECH_GSM2) || (tech_id == CXM_TECH_GSM3))
  {
    lmtsmgr_msgr_send_msg( &transmit_limits.hdr,
     			           RFA_RF_GSM_SET_SAR_LIMIT_SUB2_IND,
  				           sizeof(rfa_rf_gsm_set_sar_limit_ind_s) );
  }
  else
  {
    LMTSMGR_MSG_0(ERROR, "INVALID Tech_ID in GSM only sub");
  }

}


/*===========================================================================

  FUNCTION:  lmtsmgr_msgr_wcdma_pwrlim_trans

===========================================================================*/
/*!
  @brief
    Sends power limits to WCDMA RF over MSGR.

  @return
    None
*/
/*=========================================================================*/

void lmtsmgr_msgr_wcdma_pwrlim_trans(void)
{
/* struct to be filled and sent out */
  rfa_rf_wcdma_set_sar_limit_ind_s transmit_limits;

  int16 sar_plimit;

  sar_plimit = lmtsmgr.tech_state[CXM_TECH_WCDMA].pwr_info.curr_limits.sar_plimit[0];

  /* Initially assume the limits have not changed from previous */
  memset(&transmit_limits, 0, sizeof(rfa_rf_wcdma_set_sar_limit_ind_s));

  transmit_limits.sar_tx_limit = sar_plimit;

  /* Set the RF device */
  transmit_limits.device = lmtsmgr.tech_state[CXM_TECH_WCDMA].currList.links[0].freqInfo.link_info.rf_device;;

  /* Message indicating we are sending */
  LMTSMGR_MSG_2(HIGH, "Sending limits %d to tech %d",
  			          transmit_limits.sar_tx_limit,
                      CXM_TECH_WCDMA );

  lmtsmgr_msgr_send_msg( &transmit_limits.hdr,
     			           RFA_RF_WCDMA_SET_SAR_LIMIT_IND,
  				           sizeof(rfa_rf_wcdma_set_sar_limit_ind_s) );
}

/*=============================================================================

  FUNCTION:  lmtsmgr_notify_tech_plimit

=============================================================================*/
/*!
    @brief
    Notify a tech about the new power limits
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_notify_tech_plimit
(
  cxm_tech_type  tech
)
{
  lmtsmgr_power_info_type* pwr_info;
  int16 min_coex_plimit, min_coex_slot_plimit;
  uint8 index = 0;

  pwr_info = &lmtsmgr.tech_state[tech].pwr_info;

  /* If tech is not transmitting, reset the limits and return */
  if (lmtsmgr.tech_state[tech].currList.num_ul_entries == 0)
  {
    sar_core_max_pwr_backoff(pwr_info->new_limits.sar_plimit,
                            &pwr_info->new_limits.num_valid_sar, tech);
    pwr_info->new_limits.batt_plimit = DEFAULT_TXSTOP_LIMIT;
    for (index = 0; index < CXM_TECH_MAX; index++)
    {
      pwr_info->new_limits.coex_plimit[index] = DEFAULT_TXSTOP_LIMIT;
      pwr_info->new_limits.coex_slot_plimit[index] = DEFAULT_TXSTOP_LIMIT;
    }

    sar_core_max_pwr_backoff(pwr_info->curr_limits.sar_plimit,
                            &pwr_info->curr_limits.num_valid_sar, tech);
    pwr_info->curr_limits.batt_plimit = DEFAULT_TXSTOP_LIMIT;
    pwr_info->curr_limits.coex_plimit = DEFAULT_TXSTOP_LIMIT;
    pwr_info->curr_limits.coex_slot_plimit = DEFAULT_TXSTOP_LIMIT;

    return ;
  }


  /* Find the min coex limits */
  min_coex_plimit = pwr_info->new_limits.coex_plimit[0];
  min_coex_slot_plimit = pwr_info->new_limits.coex_slot_plimit[0];
  for (index = 0; index < CXM_TECH_MAX; index++) 
  {
    min_coex_plimit = MIN(min_coex_plimit, pwr_info->new_limits.coex_plimit[index]);
    min_coex_slot_plimit = MIN(min_coex_slot_plimit, pwr_info->new_limits.coex_slot_plimit[index]);
  }

  /* Has any limit changed since last time? */
  if ( (pwr_info->curr_limits.num_valid_sar == pwr_info->new_limits.num_valid_sar) &&
       (pwr_info->curr_limits.coex_plimit == min_coex_plimit) &&
       (pwr_info->curr_limits.coex_slot_plimit == min_coex_slot_plimit) &&
       (pwr_info->curr_limits.batt_plimit == pwr_info->new_limits.batt_plimit) )
  {
    for (index = 0; index < pwr_info->curr_limits.num_valid_sar; index++)
    {
      if (pwr_info->curr_limits.sar_plimit[index] != pwr_info->new_limits.sar_plimit[index])
          break ;
    }

    if (index == pwr_info->curr_limits.num_valid_sar)
	  {
	    return;
  	}
  }

  LMTSMGR_MSG_5(MED, "Stored Coex limits %d (slot %d), New limits %d (slot %d), tech : %d",
                     pwr_info->curr_limits.coex_plimit,
                     pwr_info->curr_limits.coex_slot_plimit,
                     min_coex_plimit, min_coex_slot_plimit, tech);

  LMTSMGR_MSG_4(MED, "Stored SAR limits %d %d, New limits %d %d",
                     pwr_info->curr_limits.sar_plimit[0],
		     pwr_info->curr_limits.sar_plimit[1],
	             pwr_info->new_limits.sar_plimit[0],
		     pwr_info->new_limits.sar_plimit[1]);

  /* Update global state */
  pwr_info->curr_limits.batt_plimit = pwr_info->new_limits.batt_plimit;
  pwr_info->curr_limits.num_valid_sar = pwr_info->new_limits.num_valid_sar;
  memscpy(&pwr_info->curr_limits.sar_plimit, sizeof(int16)*SAR_GSM_MAX_LIMIT_TYPE,
          &pwr_info->new_limits.sar_plimit, sizeof(int16)*SAR_GSM_MAX_LIMIT_TYPE);
  pwr_info->curr_limits.coex_plimit = min_coex_plimit;
  pwr_info->curr_limits.coex_slot_plimit = min_coex_slot_plimit;

  switch(tech)
  {
  /* All techs are not sent any limits currently. This will be enabled once RF SW 
     is ready to handle the power limits */
    case CXM_TECH_ONEX:
      lmtsmgr_msgr_1x_pwrlim_trans();
      break;

    case CXM_TECH_HDR:
      lmtsmgr_msgr_hdr_pwrlim_trans();
      break;

    case CXM_TECH_LTE:
      #ifdef FEATURE_LTE
      lmtsmgr_msgr_lte_pwrlim_trans();
      #else
      LMTSMGR_MSG_1(ERROR,"Tech %d not supported", tech);
      #endif
      break;

    case CXM_TECH_TDSCDMA:
      lmtsmgr_msgr_tdscdma_pwrlim_trans();
      break;
   
    case CXM_TECH_GSM2:
    case CXM_TECH_GSM1:
    case CXM_TECH_GSM3:
      lmtsmgr_msgr_gsm_pwrlim_trans(tech);
      break;

    case CXM_TECH_WCDMA:
      lmtsmgr_msgr_wcdma_pwrlim_trans();
      break;

    default:
      LMTSMGR_MSG_1(ERROR,"Tech %d not supported but have pwr limits", tech);
      break;
  }

}

/*=============================================================================

  FUNCTION:  lmtsmgr_send_freqId_list

=============================================================================*/
/*!
    @brief
    Send freqId list to tech
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_send_freqId_list
(
  cxm_tech_type  tech
)
{
#ifdef FEATURE_MODEM_COEXISTENCE_SW
  cxm_freqid_info_ind_s   freqId_msg;
  lmtsmgr_tech_state_type* tech_st;
  int i;

  TTLMARKER(LMTSMGR_SEND_FREQID_IN);
  
  /* Get a pointer to the lmtsgr tech state */
  tech_st = &lmtsmgr.tech_state[tech];

  if (tech_st->currList.num_entries <= 0)
  {
    LMTSMGR_MSG_1(LOW, "Number of entries in currList is 0 for tech %d", tech);
	TTLMARKER(LMTSMGR_SEND_FREQID_ERR1);
    return;
  }

  /* Initialization... */
  memset(&freqId_msg, 0, sizeof(freqId_msg));

  /* Fill up the freqId message with the current list */
  freqId_msg.freq_data.num_link_info_sets = tech_st->currList.num_entries;
  for (i=0; i<tech_st->currList.num_entries; i++)
  {
    freqId_msg.freq_data.link_list[i] = tech_st->currList.links[i].freqInfo;
  }

  /* Send the message */
  switch (tech)
  {
    case CXM_TECH_GSM1:
      lmtsmgr_msgr_send_msg( &freqId_msg.msg_hdr,
                            MCS_CXM_FREQID_LIST_GSM1_IND,
                            sizeof(freqId_msg) );
      break;

    case CXM_TECH_GSM2:
      lmtsmgr_msgr_send_msg( &freqId_msg.msg_hdr,
                            MCS_CXM_FREQID_LIST_GSM2_IND,
                            sizeof(freqId_msg) );
      break;

    case CXM_TECH_HDR:
      lmtsmgr_msgr_send_msg( &freqId_msg.msg_hdr,
                            MCS_CXM_FREQID_LIST_HDR_IND,
                            sizeof(freqId_msg) );
      break;

    case CXM_TECH_GSM3:
      lmtsmgr_msgr_send_msg( &freqId_msg.msg_hdr,
                            MCS_CXM_FREQID_LIST_GSM3_IND,
                            sizeof(freqId_msg) );
      break;

    case CXM_TECH_WCDMA:
      lmtsmgr_msgr_send_msg( &freqId_msg.msg_hdr,
                            MCS_CXM_FREQID_LIST_WCDMA_IND,
                            sizeof(freqId_msg) );
      break;

    case CXM_TECH_ONEX:
      lmtsmgr_msgr_send_msg( &freqId_msg.msg_hdr,
                            MCS_CXM_FREQID_LIST_ONEX_IND,
                            sizeof(freqId_msg) );
      break;

    case CXM_TECH_TDSCDMA:
      lmtsmgr_msgr_send_msg( &freqId_msg.msg_hdr,
                            MCS_CXM_FREQID_LIST_TDSCDMA_IND,
                            sizeof(freqId_msg) );
      break;

    case CXM_TECH_LTE:
	  lmtsmgr_msgr_send_msg( &freqId_msg.msg_hdr,
                            MCS_CXM_FREQID_LIST_LTE_IND,
                            sizeof(freqId_msg) );
      break;
	  
    default:
      LMTSMGR_MSG_1(ERROR, "Unsupported tech %d", tech);
      break;
  }
  
  TTLMARKER(LMTSMGR_SEND_FREQID_OUT);  
#else
  LMTSMGR_UNUSED(tech);
#endif /* FEATURE_MODEM_COEXISTENCE_SW */

  return;
}

/*=============================================================================

  FUNCTION:  lmtsmgr_update_wwcoex_state

=============================================================================*/
/*!
    @brief
    Update WWAN Coex state
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_update_wwcoex_state
(
  cxm_wwcoex_state_info_s new_state
)
{
#ifdef FEATURE_MODEM_COEXISTENCE_SW
  boolean changed = FALSE;
  cxm_wwcoex_state_update_s wwcoex_info_msg;
  
  if (lmtsmgr.state_info.coex_st != new_state.coex_st)
  {
    /* set the new state */
    lmtsmgr.state_info.coex_st = new_state.coex_st;

    changed = TRUE;
  }

  if (lmtsmgr.state_info.prob_victim_mask != new_state.prob_victim_mask)
  {
    /* set the new state */
    lmtsmgr.state_info.prob_victim_mask = new_state.prob_victim_mask;

    changed = TRUE;
  }

  if (lmtsmgr.state_info.prob_aggr_mask != new_state.prob_aggr_mask)
  {
    /* set the new state */
    lmtsmgr.state_info.prob_aggr_mask = new_state.prob_aggr_mask;

    changed = TRUE;
  }


  if (changed)
  {
    LMTSMGR_MSG_3(HIGH, "Coex State: %d, Victim Mask: 0x%x, Aggr Mask: 0x%x",
              lmtsmgr.state_info.coex_st, lmtsmgr.state_info.prob_victim_mask,
              lmtsmgr.state_info.prob_aggr_mask);

    wwcoex_info_msg.st_info = lmtsmgr.state_info;
    lmtsmgr_msgr_send_msg( &wwcoex_info_msg.msg_hdr,
                           MCS_CXM_WWCOEX_STATE_UPDATE_IND,
                           sizeof(wwcoex_info_msg) );
  }
#else
  LMTSMGR_UNUSED(new_state.coex_st);
#endif
}

/*=============================================================================

  FUNCTION:  lmtsmgr_send_activity_info_req

=============================================================================*/
/*!
    @brief
    Send request for activity timeline information
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_send_activity_info_req
(
   /*! Tech that we need information from */
  cxm_tech_type           tech_id, 

  /*! Type of activity - UL or DL */
  cxm_tech_link_direction  direction,

  /*! Start or Stop request */
  boolean                  reg_flag
)
{
#ifdef FEATURE_MODEM_COEXISTENCE_SW
  cxm_request_activity_info_s  activ_reg_msg;

  activ_reg_msg.reg_flag = reg_flag;
  activ_reg_msg.direction = direction;

  switch (tech_id)
  {
    case CXM_TECH_GSM1:
      lmtsmgr_msgr_send_msg(&activ_reg_msg.msg_hdr, 
                            MCS_CXM_REQUEST_ACTIVITY_INFO_GSM1_IND,
                            sizeof(activ_reg_msg));
      break;

    case CXM_TECH_GSM2:
      lmtsmgr_msgr_send_msg(&activ_reg_msg.msg_hdr, 
                            MCS_CXM_REQUEST_ACTIVITY_INFO_GSM2_IND,
                            sizeof(activ_reg_msg));
      break;

    case CXM_TECH_GSM3:
      lmtsmgr_msgr_send_msg(&activ_reg_msg.msg_hdr, 
                            MCS_CXM_REQUEST_ACTIVITY_INFO_GSM3_IND,
                            sizeof(activ_reg_msg));
      break;

    case CXM_TECH_ONEX:
    case CXM_TECH_HDR:
    case CXM_TECH_LTE:
    case CXM_TECH_TDSCDMA:
    case CXM_TECH_WCDMA:
    default:
      break;
  }
#else
  LMTSMGR_UNUSED(tech_id);
  LMTSMGR_UNUSED(direction);
  LMTSMGR_UNUSED(reg_flag);
#endif
  return ;
}

/*=============================================================================

  FUNCTION:  lmtsmgr_send_filter_path_ind

=============================================================================*/
/*!
    @brief
    Send RF filter path indication
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_send_filter_path_ind
(
  lmtsmgr_filter_path_type send_ind
)
{
  rfm_rf_cmn_bypass_filtered_path_req_s msg_ind;

  memset(&msg_ind, 0, sizeof(msg_ind));

  msg_ind.rsp_required = FALSE;
  msg_ind.use_filtered_path_flag = send_ind.filtered_path_flag;
  msg_ind.use_gsm_aggr_filter_path_flag = send_ind.gsm_aggr_filter_path_flag;

  lmtsmgr_msgr_send_msg( &msg_ind.hdr,
                         RFA_RF_COMMON_BYPASS_FILTERED_PATH_REQ,
                         sizeof(msg_ind) );

  LMTSMGR_MSG_2(HIGH, "Sent filter path indication : GSM Rx %d, Tx %d", 
                        send_ind.filtered_path_flag, send_ind.gsm_aggr_filter_path_flag);
}

/*=============================================================================

  FUNCTION:  lmtsmgr_send_blacklist_info

=============================================================================*/
/*!
    @brief
    Send Blacklist information to appropriate tech for Band Avoidance
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_send_blacklist_info
(
  cxm_tech_type tech_id,
  boolean       send_blank_bl
)
{
  cxm_coex_ba_ind_s msg_ind;

  memset(&msg_ind, 0, sizeof(cxm_coex_ba_ind_s));

  if (!send_blank_bl)
  {
    memscpy(&msg_ind.blacklist_data, sizeof(coex_ba_blist_data),
            &lmtsmgr.band_avoidance.currBlacklist, sizeof(coex_ba_blist_data));
  }
  else
  {
     /* Just set the blacklist id */
     msg_ind.blacklist_data.blacklist_id = lmtsmgr.band_avoidance.currBlacklist.blacklist_id;
  }

  switch (tech_id)
  {
    case CXM_TECH_GSM1:
      lmtsmgr_msgr_send_msg(&msg_ind.msg_hdr, 
                            MCS_CXM_BAND_AVOID_BLIST_GSM1_IND,
                            sizeof(cxm_coex_ba_ind_s));
      break;

    case CXM_TECH_GSM2:
      lmtsmgr_msgr_send_msg(&msg_ind.msg_hdr, 
                            MCS_CXM_BAND_AVOID_BLIST_GSM2_IND,
                            sizeof(cxm_coex_ba_ind_s));
      break;

    case CXM_TECH_LTE:
      lmtsmgr_msgr_send_msg(&msg_ind.msg_hdr, 
                            MCS_CXM_BAND_AVOID_BLIST_LTE_IND,
                            sizeof(cxm_coex_ba_ind_s));
      break;

    case CXM_TECH_ONEX:
      lmtsmgr_msgr_send_msg(&msg_ind.msg_hdr, 
                            MCS_CXM_BAND_AVOID_BLIST_ONEX_IND,
                            sizeof(cxm_coex_ba_ind_s));
      break;

    case CXM_TECH_HDR:
      lmtsmgr_msgr_send_msg(&msg_ind.msg_hdr, 
                            MCS_CXM_BAND_AVOID_BLIST_HDR_IND,
                            sizeof(cxm_coex_ba_ind_s));
      break;

    case CXM_TECH_WCDMA:
      lmtsmgr_msgr_send_msg(&msg_ind.msg_hdr, 
                            MCS_CXM_BAND_AVOID_BLIST_WCDMA_IND,
                            sizeof(cxm_coex_ba_ind_s));
      break;

    case CXM_TECH_TDSCDMA:
      lmtsmgr_msgr_send_msg(&msg_ind.msg_hdr, 
                            MCS_CXM_BAND_AVOID_BLIST_TDSCDMA_IND,
                            sizeof(cxm_coex_ba_ind_s));
      break;

    default:
      break;
  }

}

/*=============================================================================

  FUNCTION:  lmtsmgr_send_sl_activity_info

=============================================================================*/
/*!
    @brief
    Send information to appropriate tech for Slot Level Activity
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_send_sl_activity_info
(
  /*! Receiver of this timeline information */
  cxm_tech_type tech_id_rec,

  /*! Activity specifics of sending tech */
  cxm_timing_info_s* msg_ind
)
{
  switch (tech_id_rec)
  {
    case CXM_TECH_LTE:
      lmtsmgr_msgr_send_msg(&msg_ind->msg_hdr, 
                            MCS_CXM_SET_SLOT_ACTIVITY_TL_LTE_REC_IND,
                            sizeof(cxm_timing_info_s));
      break;

    case CXM_TECH_WCDMA:
      lmtsmgr_msgr_send_msg(&msg_ind->msg_hdr, 
                            MCS_CXM_SET_SLOT_ACTIVITY_TL_WCDMA_REC_IND,
                            sizeof(cxm_timing_info_s));
      break;

    case CXM_TECH_TDSCDMA:
      lmtsmgr_msgr_send_msg(&msg_ind->msg_hdr, 
                            MCS_CXM_SET_SLOT_ACTIVITY_TL_TDS_REC_IND,
                            sizeof(cxm_timing_info_s));
      break;

    default:
      LMTSMGR_MSG_1(ERROR, "Unsupported tech id %d to receive Slot Level Activity TL!", tech_id_rec);
  }

}
