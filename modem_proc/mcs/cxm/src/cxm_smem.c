/*!
  @file
  cxm_smem.c

  @brief
  Implementation of CXM's SMEM APIs

*/

/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
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

===========================================================================*/
/*==========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cxm/src/cxm_smem.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/14   tak     TDS/GSM Coex
01/30/14   tak     Initial implementation

===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
/* Common */
#include <IxErrno.h>
#include "smp2p.h"
#include "smem_type.h"
#include "cxm_smem.h"
#include "wcn_coex_mgr.h"
#include "cxm_utils.h"
#include "coex_smem_intf.h"
#include "stringl.h"
#include "cxm_trace.h"

/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

STATIC smp2p_context_type *cxm_smem_smp2p_l1_tx_ctx;
STATIC smp2p_context_type *cxm_smem_smp2p_fw_tx_ctx;
STATIC smp2p_context_type *cxm_smem_smp2p_rx_ctx;
STATIC cxm_smem_data_s *cxm_smem_data = NULL;
STATIC boolean cxm_smem_initialized = FALSE;
STATIC cxm_smem_tech_fw_activity_s cxm_smem_tech_fw;

#define CXM_SMEM_ENTER_CRITICAL_SECTION() \
  cxm_smem_data->modem_flag = CXM_SMEM_FLAG_ON; \
  cxm_smem_data->turn = CXM_SMEM_WCN_TURN; \
  while ( cxm_smem_data->wcn_flag == CXM_SMEM_FLAG_ON && cxm_smem_data->turn == CXM_SMEM_WCN_TURN ) \
  { \
  }

#define CXM_SMEM_EXIT_CRITICAL_SECTION() cxm_smem_data->modem_flag = CXM_SMEM_FLAG_OFF
/* Since the updates in FW activity happen much slower than the time it takes WCN to read the data, 
   a 0-1 count is sufficient. So MAX could be as low as 2 */
#define CXM_SMEM_CHECK_COUNT_MAX 100
/*===========================================================================

                    INTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*=============================================================================

  FUNCTION:  cxm_get_smem_data_ptr

=============================================================================*/
/*!
    @brief
    Returns a pointer to the global cxm_smem_data pointer for diag logging.
    A pointer to a pointer is necessary because cxm_smem_data is a static
    pointer variable declared in cxm_smem.c which can change value during
    runtime.  The cxm log LUTs are initialized only once.

    @return
    cxm_smem_data_s **
*/
/*===========================================================================*/
cxm_smem_data_s **cxm_get_smem_data_ptr( void )
{
  return &cxm_smem_data;
}

/*=============================================================================

  FUNCTION:  cxm_get_smem_fw_act_ptr

=============================================================================*/
/*!
    @brief
    Returns a pointer to FW activity state so that the logging module knows
    what index in the FW buffer was just updated.

    @return
    cxm_smem_tech_fw_activity_s*
*/
/*===========================================================================*/
cxm_smem_tech_fw_activity_s* cxm_get_smem_fw_act_ptr( void )
{
  return &cxm_smem_tech_fw;
}

/*===========================================================================

  FUNCTION:  cxm_smem_init

===========================================================================*/
/*!
  @brief
    Initialize SMP2P and SMEM

  @return
    none
*/
/*=========================================================================*/
void cxm_smem_init (
  void
)
{
  int32 result_tx, result_rx, result_fw_tx;
  /*-----------------------------------------------------------------------*/
  if ( !cxm_smem_initialized )
  {
    /*initialize FW activity state*/
    memset( &cxm_smem_tech_fw, 0, sizeof( cxm_smem_tech_fw_activity_s ) );

    /*initialize SMP2P tx for l1 messages - must be before register*/
    result_tx = smp2p_create( &cxm_smem_smp2p_l1_tx_ctx, SMEM_WCN, CXM_SMEM_MDM_TO_WCN_SMP2P );
    if ( result_tx == SMP2P_STATUS_NOT_INIT )
    {
      result_tx = smp2p_wait( cxm_smem_smp2p_l1_tx_ctx );
    }
    /*initialize SMP2P tx for fw messages - must be before register*/
    result_fw_tx = smp2p_create( &cxm_smem_smp2p_fw_tx_ctx, SMEM_WCN, CXM_SMEM_MDM_FW_TO_WCN_SMP2P );
    if ( result_fw_tx == SMP2P_STATUS_NOT_INIT )
    {
      result_fw_tx = smp2p_wait( cxm_smem_smp2p_fw_tx_ctx );
    }
    /* register for SMP2P rx - must be after create */
    result_rx = smp2p_register( &cxm_smem_smp2p_rx_ctx, SMEM_WCN, CXM_SMEM_WCN_TO_MDM_SMP2P );
    if ( result_rx == SMP2P_STATUS_NOT_FOUND )
    {
      result_rx = smp2p_wait( cxm_smem_smp2p_rx_ctx );
    }

    /*initialize SMEM*/
    cxm_smem_data = (cxm_smem_data_s*) smem_alloc( SMEM_COEX_MDM_WCN, sizeof(cxm_smem_data_s) );
    CXM_MSG_3( HIGH, "smem inited result_tx=%d, result_rx=%d, result_fw_tx=%d", result_tx, result_rx, result_fw_tx );
    CXM_ASSERT( result_tx == SMP2P_STATUS_SUCCESS && result_rx == SMP2P_STATUS_SUCCESS && 
                cxm_smem_data != NULL && result_fw_tx == SMP2P_STATUS_SUCCESS)
    cxm_smem_initialized = TRUE;
  }

} /* cxm_smem_init */

/*===========================================================================

  FUNCTION:  cxm_smem_set_toggle_bit

===========================================================================*/
/*!
  @brief
    Set the toggle bit to signal WCN. Reads WCN's ack bit then sets MDM's
    toggle bit to the opposite value.

  @return
    none
*/
/*=========================================================================*/
void cxm_smem_set_toggle_bit (
  void
)
{
  int32 result;
  uint32 rx_data;
  uint32 flag;
  /*-----------------------------------------------------------------------*/

  result = smp2p_read( cxm_smem_smp2p_rx_ctx, &rx_data );
  /*determine whether we interrupt the receiver or not*/
  if ( rx_data & CXM_SMP2P_STATE_BIT )
  {
    flag = SMP2P_FLAGS_NONE;
  }
  else
  {
    flag = SMP2P_FLAGS_NO_INTR;
  }

  if ( rx_data & CXM_SMP2P_ACK_BIT )
  {
    result = smp2p_modify_ex( cxm_smem_smp2p_l1_tx_ctx, CXM_SMP2P_TOGGLE_BIT, 0, flag );
  }
  else
  {
    result = smp2p_modify_ex( cxm_smem_smp2p_l1_tx_ctx, 0, CXM_SMP2P_TOGGLE_BIT, flag );
  }
  CXM_ASSERT( result == SMP2P_STATUS_SUCCESS );

} /* cxm_smem_set_toggle_bit */

/*===========================================================================

  FUNCTION:  cxm_smem_set_state

===========================================================================*/
/*!
  @brief
    Set the state of MDM. This is called when going to sleep or waking up.

  @return
    none
*/
/*=========================================================================*/
void cxm_smem_set_state (
  boolean active
)
{
  int32 result;
  /*-----------------------------------------------------------------------*/
  
  if ( active )
  {
    result = smp2p_modify_ex( cxm_smem_smp2p_l1_tx_ctx, 0, 
                              CXM_SMP2P_STATE_BIT, SMP2P_FLAGS_NO_INTR );
  }
  else
  {
    result = smp2p_modify_ex( cxm_smem_smp2p_l1_tx_ctx, CXM_SMP2P_STATE_BIT, 
                              0, SMP2P_FLAGS_NO_INTR );
  }
  CXM_ASSERT( result == SMP2P_STATUS_SUCCESS );

} /* cxm_smem_set_state */

/*===========================================================================

  FUNCTION:  cxm_smem_handle_sleep_wakeup

===========================================================================*/
/*!
  @brief
    Send sleep/wake-up information over SMEM

  @return
    none
*/
/*=========================================================================*/
void cxm_smem_handle_sleep_wakeup (
  cxm_coex_tech_sleep_wakeup_duration_ind_s *msg 
)
{
  
  /*-----------------------------------------------------------------------*/
  
  if ( cxm_smem_initialized )
  {
    CXM_SMEM_ENTER_CRITICAL_SECTION();
    if ( msg->tech_id == CXM_TECH_LTE )
    {
      cxm_smem_data->tech_mask |= CXM_SMEM_UPDATE_TECH_LTE;
      cxm_smem_data->lte.msg_mask |= CXM_SMEM_UPDATE_MSG_SLEEP_WAKEUP;
      cxm_smem_data->lte.state.is_waking_up = !msg->is_going_to_sleep;
      cxm_smem_data->lte.state.duration = msg->duration;
      CXM_TRACE( 0, 0, 0, 0, CXM_LOG_PKT_EN, CXM_LOG_SMEM_LTE_SLEEP_WAKEUP );
    }
    if ( msg->tech_id == CXM_TECH_GSM1 )
    {
      cxm_smem_data->tech_mask |= CXM_SMEM_UPDATE_TECH_GSM1;
      cxm_smem_data->gsm1.msg_mask |= CXM_SMEM_UPDATE_MSG_SLEEP_WAKEUP;
      cxm_smem_data->gsm1.state.is_waking_up = !msg->is_going_to_sleep;
      cxm_smem_data->gsm1.state.duration = msg->duration;
      CXM_TRACE( 0, 0, 0, 0, CXM_LOG_PKT_EN, CXM_LOG_SMEM_GSM1_SLEEP_WAKEUP );
    }
    if ( msg->tech_id == CXM_TECH_GSM2 )
    {
      cxm_smem_data->tech_mask |= CXM_SMEM_UPDATE_TECH_GSM2;
      cxm_smem_data->gsm2.msg_mask |= CXM_SMEM_UPDATE_MSG_SLEEP_WAKEUP;
      cxm_smem_data->gsm2.state.is_waking_up = !msg->is_going_to_sleep;
      cxm_smem_data->gsm2.state.duration = msg->duration;
      CXM_TRACE( 0, 0, 0, 0, CXM_LOG_PKT_EN, CXM_LOG_SMEM_GSM2_SLEEP_WAKEUP );
    }
    if ( msg->tech_id == CXM_TECH_GSM3 )
    {
      cxm_smem_data->tech_mask |= CXM_SMEM_UPDATE_TECH_GSM3;
      cxm_smem_data->gsm3.msg_mask |= CXM_SMEM_UPDATE_MSG_SLEEP_WAKEUP;
      cxm_smem_data->gsm3.state.is_waking_up = !msg->is_going_to_sleep;
      cxm_smem_data->gsm3.state.duration = msg->duration;
      CXM_TRACE( 0, 0, 0, 0, CXM_LOG_PKT_EN, CXM_LOG_SMEM_GSM3_SLEEP_WAKEUP );
    }
    if ( msg->tech_id == CXM_TECH_TDSCDMA )
    {
      cxm_smem_data->tech_mask |= CXM_SMEM_UPDATE_TECH_TDSCDMA;
      cxm_smem_data->tds.msg_mask |= CXM_SMEM_UPDATE_MSG_SLEEP_WAKEUP;
      cxm_smem_data->tds.state.is_waking_up = !msg->is_going_to_sleep;
      cxm_smem_data->tds.state.duration = msg->duration;
      CXM_TRACE( 0, 0, 0, 0, CXM_LOG_PKT_EN, CXM_LOG_SMEM_TDS_SLEEP_WAKEUP );
    }

    if ( cxm_smem_data->lte.state.is_waking_up || cxm_smem_data->gsm1.state.is_waking_up
         || cxm_smem_data->gsm2.state.is_waking_up || cxm_smem_data->gsm3.state.is_waking_up
         || cxm_smem_data->tds.state.is_waking_up )
    {
      cxm_smem_set_state( TRUE );
    }
    else
    {
      cxm_smem_set_state( FALSE );
    }

    cxm_smem_set_toggle_bit();
    CXM_SMEM_EXIT_CRITICAL_SECTION();
    CXM_MSG_3( HIGH, "sent sleep/wakeup via SMEM, tech=%d, duration=%d, waking_up=%d", msg->tech_id, msg->duration,
               !msg->is_going_to_sleep );
  }

} /* cxm_smem_set_state */

/*===========================================================================

  FUNCTION:  cxm_smem_handle_frame_timing

===========================================================================*/
/*!
  @brief
    Send frame timing information over SMEM

  @return
    none
*/
/*=========================================================================*/
void cxm_smem_handle_frame_timing_ind (
  void *rcv_msgr_msg_ptr
   
)
{
  cxm_coex_frame_timing_ind_s *msg;
  /*-----------------------------------------------------------------------*/
  
  if ( cxm_smem_initialized )
  {
    msg = (cxm_coex_frame_timing_ind_s *) rcv_msgr_msg_ptr;
     
    CXM_SMEM_ENTER_CRITICAL_SECTION();
    if ( msg->tech_id == CXM_TECH_LTE )
    {
      cxm_smem_data->tech_mask |= CXM_SMEM_UPDATE_TECH_LTE;
      cxm_smem_data->lte.msg_mask |= CXM_SMEM_UPDATE_MSG_FRAME_TIMING;
      cxm_smem_data->lte.frame_timing.ta = msg->params.lte.ta;
      cxm_smem_data->lte.frame_timing.dl_frame_time = msg->params.lte.dl_frame_time;
      CXM_TRACE( 0, 0, 0, 0, CXM_LOG_PKT_EN, CXM_LOG_SMEM_LTE_FRAME_TIMING );
    }
    else if ( msg->tech_id == CXM_TECH_TDSCDMA )
    {
      cxm_smem_data->tech_mask |= CXM_SMEM_UPDATE_TECH_TDSCDMA;
      cxm_smem_data->tds.msg_mask |= CXM_SMEM_UPDATE_MSG_FRAME_TIMING;
      cxm_smem_data->tds.frame_timing.ta = msg->params.tds.ta;
      cxm_smem_data->tds.frame_timing.ustmr_val = msg->params.tds.ustmr_val;
      cxm_smem_data->tds.frame_timing.sfn = msg->params.tds.sfn;
      CXM_TRACE( 0, 0, 0, 0, CXM_LOG_PKT_EN, CXM_LOG_SMEM_TDS_FRAME_TIMING );
    }
    cxm_smem_set_toggle_bit();
    CXM_SMEM_EXIT_CRITICAL_SECTION();
    CXM_MSG_4( HIGH, "sent frame timing via SMEM, tech=%d, TA=%d DL frame time=%d, sfn(for TDS only)=%d", msg->tech_id,
               msg->tech_id == CXM_TECH_LTE ? msg->params.lte.ta : msg->params.tds.ta, 
               msg->tech_id == CXM_TECH_LTE ? msg->params.lte.dl_frame_time : msg->params.tds.ustmr_val,
               msg->params.tds.sfn );
  }

} /* cxm_smem_handle_frame_timing */

/*===========================================================================

  FUNCTION:  cxm_smem_handle_tx_adv

===========================================================================*/
/*!
  @brief
    Send tx advanced message over SMEM

  @return
    none
*/
/*=========================================================================*/
void cxm_smem_handle_lte_tx_adv (
  cxm_lte_tx_adv_ntc_info_s *msg,
  boolean thres_check_passed 
)
{
  /*-----------------------------------------------------------------------*/
  
  if ( cxm_smem_initialized )
  {
    CXM_SMEM_ENTER_CRITICAL_SECTION();
    cxm_smem_data->tech_mask |= CXM_SMEM_UPDATE_TECH_LTE;
    cxm_smem_data->lte.msg_mask |= CXM_SMEM_UPDATE_MSG_TX_ADVANCED;
    cxm_smem_data->lte.tx_adv.sfn = msg->sfn;
    cxm_smem_data->lte.tx_adv.channel_type = (cxm_smem_lte_chan_e) msg->channel_type;
    cxm_smem_data->lte.tx_adv.slot0_power = msg->slot0_power;
    cxm_smem_data->lte.tx_adv.slot0_first_rb = msg->slot0_first_rb;
    cxm_smem_data->lte.tx_adv.slot0_last_rb = msg->slot0_last_rb;
    cxm_smem_data->lte.tx_adv.slot1_power = msg->slot1_power;
    cxm_smem_data->lte.tx_adv.slot1_first_rb = msg->slot1_first_rb;
    cxm_smem_data->lte.tx_adv.slot1_last_rb = msg->slot1_last_rb;
    cxm_smem_data->lte.tx_adv.rbs = msg->rbs;

    if ( msg->transmit )
    {
      cxm_smem_data->lte.tx_adv.transmit = thres_check_passed ? CXM_SMEM_TX_ON :
                                           CXM_SMEM_TX_ON_NO_CONFL;
    }
    else
    {
      cxm_smem_data->lte.tx_adv.transmit = CXM_SMEM_TX_OFF;
    }
    CXM_TRACE( 0, 0, 0, 0, CXM_LOG_PKT_EN, CXM_LOG_SMEM_LTE_TX_ADV );
    cxm_smem_set_toggle_bit();
    CXM_SMEM_EXIT_CRITICAL_SECTION();
    CXM_MSG_2( HIGH, "sent tx adv via SMEM, sfn=%d, tx_state=%d", msg->sfn, 
               cxm_smem_data->lte.tx_adv.transmit );
  }

} /* cxm_smem_handle_lte_tx_adv */

/*===========================================================================

  FUNCTION:  cxm_smem_handle_activity_adv_ntc

===========================================================================*/
/*!
  @brief
    Send activity advanced message over SMEM

  @return
    none
*/
/*=========================================================================*/
void cxm_smem_handle_activity_adv_ntc (
  cxm_coex_tech_activity_adv_ntc_ind_s *msg
)
{
  /*-----------------------------------------------------------------------*/
  
  if ( cxm_smem_initialized )
  {
    CXM_SMEM_ENTER_CRITICAL_SECTION();
    if ( msg->tech_id == CXM_TECH_TDSCDMA )
    {
      cxm_smem_data->tech_mask |= CXM_SMEM_UPDATE_TECH_TDSCDMA;
      cxm_smem_data->tds.msg_mask |= CXM_SMEM_UPDATE_MSG_ACT_ADVANCED;
      cxm_smem_data->tds.act_adv.sfn = msg->params.tds.sfn;
      memscpy(cxm_smem_data->tds.act_adv.slot, sizeof(cxm_smem_tds_slot_info_s)*CXM_SMEM_NUM_TDS_SLOTS, 
              msg->params.tds.slot, sizeof(cxm_tds_slot_info_s)*CXM_NUM_TDS_SLOTS);
      CXM_TRACE( 0, 0, 0, 0, CXM_LOG_PKT_EN, CXM_LOG_SMEM_TDS_ACT_ADV );
    }
    cxm_smem_set_toggle_bit();
    CXM_SMEM_EXIT_CRITICAL_SECTION();
    CXM_MSG_2( HIGH, "sent activity adv via SMEM, tech=%d, sfn=%d", msg->tech_id, 
               msg->params.tds.sfn );
  }
}

/*===========================================================================

  FUNCTION:  cxm_smem_handle_activity_timeline

===========================================================================*/
/*!
  @brief
    Send activity timeline message over SMEM

  @return
    none
*/
/*=========================================================================*/
void cxm_smem_handle_activity_timeline (
  cxm_timing_info_s *msg
)
{
  cxm_smem_activity_timeline_s  *mem_to_update = NULL;
  uint8                         i;
  /*-----------------------------------------------------------------------*/
  
  if ( cxm_smem_initialized )
  {
    CXM_SMEM_ENTER_CRITICAL_SECTION();
    switch ( msg->tech_id )
    {
      case CXM_TECH_GSM1:
        cxm_smem_data->tech_mask |= CXM_SMEM_UPDATE_TECH_GSM1;
        cxm_smem_data->gsm1.msg_mask |= CXM_SMEM_UPDATE_MSG_ACT_TIMELINE;
        mem_to_update = &(cxm_smem_data->gsm1.timeline);
        break;
      case CXM_TECH_GSM2:
        cxm_smem_data->tech_mask |= CXM_SMEM_UPDATE_TECH_GSM2;
        cxm_smem_data->gsm2.msg_mask |= CXM_SMEM_UPDATE_MSG_ACT_TIMELINE;
        mem_to_update = &(cxm_smem_data->gsm2.timeline);
        break;
      case CXM_TECH_GSM3:
        cxm_smem_data->tech_mask |= CXM_SMEM_UPDATE_TECH_GSM3;
        cxm_smem_data->gsm3.msg_mask |= CXM_SMEM_UPDATE_MSG_ACT_TIMELINE;
        mem_to_update = &(cxm_smem_data->gsm3.timeline);
        break;
      default:
        break;
    }

    if ( mem_to_update != NULL )
    {
      mem_to_update->num_entries = msg->num_entries;
      for ( i = 0; i < mem_to_update->num_entries; i++ )
      {
        mem_to_update->slot_list[i].frequency = msg->slot_list[i].frequency;
        mem_to_update->slot_list[i].bandwidth = msg->slot_list[i].bandwidth;
        mem_to_update->slot_list[i].start_time = msg->slot_list[i].start_time;
        mem_to_update->slot_list[i].end_time = msg->slot_list[i].end_time;
        mem_to_update->slot_list[i].prio = (cxm_smem_slot_prio_e) msg->slot_list[i].wlan_prio;
        mem_to_update->slot_list[i].pwr = msg->slot_list[i].pwr;
        mem_to_update->slot_list[i].slot_type = msg->slot_list[i].direction == 
          CXM_LNK_DRCTN_UL ? CXM_SMEM_SLOT_TX : CXM_SMEM_SLOT_RX;
        mem_to_update->slot_list[i].slot_type = msg->slot_list[i].link_type == 
          CXM_LNK_TYPE_POWER_MONITOR ? CXM_SMEM_SLOT_MON : mem_to_update->slot_list[i].slot_type;
      }
    }

    switch( msg->tech_id )
    {
      case CXM_TECH_GSM1:
        CXM_TRACE( 0, 0, 0, 0, CXM_LOG_PKT_EN, CXM_LOG_SMEM_GSM1_TIMELINE_V2 );
        break;
      case CXM_TECH_GSM2:
        CXM_TRACE( 0, 0, 0, 0, CXM_LOG_PKT_EN, CXM_LOG_SMEM_GSM2_TIMELINE_V2 );
        break;
      case CXM_TECH_GSM3:
        CXM_TRACE( 0, 0, 0, 0, CXM_LOG_PKT_EN, CXM_LOG_SMEM_GSM3_TIMELINE_V2 );
        break;
      default:
        break;
    }

    cxm_smem_set_toggle_bit();
    CXM_SMEM_EXIT_CRITICAL_SECTION();
    CXM_MSG_2( HIGH, "sent activity timeline via SMEM, tech=%d, num_entries=%d", msg->tech_id, 
               msg->num_entries );
  }
}

/*===========================================================================

  FUNCTION:  cxm_smem_handle_high_prio

===========================================================================*/
/*!
  @brief
    Send activity advanced message over SMEM

  @return
    none
*/
/*=========================================================================*/
void cxm_smem_handle_high_prio (
  cxm_coex_high_prio_ind_s *msg
)
{
  /*pointer to the high prio message we will update. This helps us re-use code since contents
    are thesame irrespective of the tech*/
  cxm_smem_high_prio_s      *msg_to_update = NULL;

  /*-----------------------------------------------------------------------*/
  
  if ( cxm_smem_initialized )
  {
    CXM_SMEM_ENTER_CRITICAL_SECTION();
    if ( msg->tech_id == CXM_TECH_TDSCDMA )
    {
      cxm_smem_data->tech_mask |= CXM_SMEM_UPDATE_TECH_TDSCDMA;
      cxm_smem_data->tds.msg_mask |= CXM_SMEM_UPDATE_MSG_HIGH_PRIO;
      msg_to_update = &(cxm_smem_data->tds.high_prio);
    }
    else if ( msg->tech_id == CXM_TECH_GSM1 )
    {
      cxm_smem_data->tech_mask |= CXM_SMEM_UPDATE_TECH_GSM1;
      cxm_smem_data->gsm1.msg_mask |= CXM_SMEM_UPDATE_MSG_HIGH_PRIO;
      msg_to_update = &(cxm_smem_data->gsm1.high_prio);
    }
    else if ( msg->tech_id == CXM_TECH_GSM2 )
    {
      cxm_smem_data->tech_mask |= CXM_SMEM_UPDATE_TECH_GSM2;
      cxm_smem_data->gsm2.msg_mask |= CXM_SMEM_UPDATE_MSG_HIGH_PRIO;
      msg_to_update = &(cxm_smem_data->gsm2.high_prio);
    }
    else if ( msg->tech_id == CXM_TECH_GSM3 )
    {
      cxm_smem_data->tech_mask |= CXM_SMEM_UPDATE_TECH_GSM3;
      cxm_smem_data->gsm3.msg_mask |= CXM_SMEM_UPDATE_MSG_HIGH_PRIO;
      msg_to_update = &(cxm_smem_data->gsm3.high_prio);
    }
    else if ( msg->tech_id == CXM_TECH_LTE )
    {
      cxm_smem_data->tech_mask |= CXM_SMEM_UPDATE_TECH_LTE;
      cxm_smem_data->lte.msg_mask |= CXM_SMEM_UPDATE_MSG_HIGH_PRIO;
      msg_to_update = &(cxm_smem_data->lte.high_prio);
    }

    if ( msg_to_update != NULL )
    {
      msg_to_update->start = msg->start;
      msg_to_update->end = msg->end;
      msg_to_update->num_freqs = msg->num_freqs;
      memscpy(msg_to_update->freq, sizeof(uint32)*CXM_SMEM_MAX_HIGH_PRIO_FREQ, 
              msg->freq, sizeof(uint32)*COEX_MAX_HIGH_PRIO_FREQ);
    }

    switch( msg->tech_id )
    {
      case CXM_TECH_TDSCDMA:
        CXM_TRACE( 0, 0, 0, 0, CXM_LOG_PKT_EN, CXM_LOG_SMEM_TDS_HIGH_PRIO );
        break;
      case CXM_TECH_GSM1:
        CXM_TRACE( 0, 0, 0, 0, CXM_LOG_PKT_EN, CXM_LOG_SMEM_GSM1_HIGH_PRIO );
        break;
      case CXM_TECH_GSM2:
        CXM_TRACE( 0, 0, 0, 0, CXM_LOG_PKT_EN, CXM_LOG_SMEM_GSM2_HIGH_PRIO );
        break;
      case CXM_TECH_GSM3:
        CXM_TRACE( 0, 0, 0, 0, CXM_LOG_PKT_EN, CXM_LOG_SMEM_GSM3_HIGH_PRIO );
        break;
      case CXM_TECH_LTE:
        CXM_TRACE( 0, 0, 0, 0, CXM_LOG_PKT_EN, CXM_LOG_SMEM_LTE_HIGH_PRIO );
        break;
      default:
        break;
    }

    cxm_smem_set_toggle_bit();
    CXM_SMEM_EXIT_CRITICAL_SECTION();
    CXM_MSG_3( HIGH, "sent high prio via SMEM, tech=%d, start=%d, end=%d", msg->tech_id, 
               msg->start, msg->end);
  }
}

/*===========================================================================

  FUNCTION:  cxm_smem_handle_fw_msg

===========================================================================*/
/*!
  @brief
    Send event indication from FW to WCN via smp2p

  @return
    none
*/
/*=========================================================================*/
void cxm_smem_handle_fw_msg (
  cxm_tech_type tech,
  cxm_port_id_e id,
  boolean assert
)
{
  int32 result;
  uint32 flag;
  uint32 rx_data;
  /*-----------------------------------------------------------------------*/
  
  if ( cxm_smem_initialized )
  {
    result = smp2p_read( cxm_smem_smp2p_rx_ctx, &rx_data );
    /*determine whether we interrupt the receiver or not*/
    if ( rx_data & CXM_SMP2P_STATE_BIT )
    {
      flag = SMP2P_FLAGS_NONE;
    }
    else
    {
      flag = SMP2P_FLAGS_NO_INTR;
    }
  
    if ( tech == CXM_TECH_LTE )
    {
      if ( id == CXM_PORT_A )
      {
        if ( assert )
        {
          result = smp2p_modify_ex( cxm_smem_smp2p_fw_tx_ctx, 0, CXM_SMEM_FW_EVENT_TX_ACTIVE, flag );
        }
        else
        {
          result = smp2p_modify_ex( cxm_smem_smp2p_fw_tx_ctx, CXM_SMEM_FW_EVENT_TX_ACTIVE, 0, flag );
        }
      }
      else if ( id == CXM_PORT_B )
      {
        if ( assert )
        {
          result = smp2p_modify_ex( cxm_smem_smp2p_fw_tx_ctx, 0, CXM_SMEM_FW_EVENT_RX_ACTIVE, flag );
        }
        else
        {
          result = smp2p_modify_ex( cxm_smem_smp2p_fw_tx_ctx, CXM_SMEM_FW_EVENT_RX_ACTIVE, 0, flag );
        }
      }
    }
    CXM_ASSERT( result == SMP2P_STATUS_SUCCESS );
  }

} /* cxm_smem_handle_fw_msg */

/*===========================================================================

  FUNCTION:  cxm_smem_handle_fw_activity

===========================================================================*/
/*!
  @brief
    Sends FW activity to WCN via SMEM. This function gets called
    when port ID is CXM_PORT_TYPE_SMEM_V2_BASED.

  @return
    none
*/
/*=========================================================================*/
void cxm_smem_handle_fw_activity (
  cxm_tech_type tech,
  cxm_rx_tx_activity_info_u activity
)
{
  uint8 read64_check;
  /*-----------------------------------------------------------------------*/

  if ( cxm_smem_initialized && atomic_compare_and_set( &cxm_smem_tech_fw.act[tech].writing, 0, 1 ) )
  {
    switch ( tech )
    {
      case CXM_TECH_TDSCDMA:
        /* update the check bit that allows WCN to do a coherent 64-bit read */
        read64_check = 
          cxm_smem_data->cxm_smem_tds_fw[cxm_smem_tech_fw.act[tech].next_entry].bitmap.check1 == 0 ? 1 : 0;
        activity.check1 = activity.check2 = read64_check;
        /* increment the counter and write the activity.
           Note: the counter must be incremented before writing the activity for the algorithm
           to work correctly*/
        cxm_smem_data->cxm_smem_tds_check_count = (cxm_smem_data->cxm_smem_tds_check_count + 1) % 
          CXM_SMEM_CHECK_COUNT_MAX;
        cxm_smem_data->cxm_smem_tds_fw[cxm_smem_tech_fw.act[tech].next_entry].data = activity.data;
        /* log the data with current buffer index (it's important that we don't increment before logging)*/
        CXM_TRACE( 0, 0, 0, 0, CXM_LOG_PKT_EN, CXM_LOG_SMEM_TDS_FW_DATA_V2 );
        /* keep track of the next array entry to fill */
        cxm_smem_tech_fw.act[tech].next_entry = (cxm_smem_tech_fw.act[tech].next_entry + 1) % 
          CXM_SMEM_TDS_FW_BUF_SIZE;
        
        break;
      case CXM_TECH_GSM1:
        read64_check = 
          cxm_smem_data->cxm_smem_gsm1_fw[cxm_smem_tech_fw.act[tech].next_entry].bitmap.check1 == 0 ? 1 : 0;
        activity.check1 = activity.check2 = read64_check;
        cxm_smem_data->cxm_smem_gsm1_check_count = (cxm_smem_data->cxm_smem_gsm1_check_count + 1) % 
          CXM_SMEM_CHECK_COUNT_MAX;
        cxm_smem_data->cxm_smem_gsm1_fw[cxm_smem_tech_fw.act[tech].next_entry].data = activity.data;
        /* log the data with current buffer index (it's important that we don't increment before logging)*/
        CXM_TRACE( 0, 0, 0, 0, CXM_LOG_PKT_EN, CXM_LOG_SMEM_GSM1_FW_DATA_V2 );
        cxm_smem_tech_fw.act[tech].next_entry = (cxm_smem_tech_fw.act[tech].next_entry + 1) % 
          CXM_SMEM_GSM_FW_BUF_SIZE;
        break;
      case CXM_TECH_GSM2:
        read64_check = 
          cxm_smem_data->cxm_smem_gsm2_fw[cxm_smem_tech_fw.act[tech].next_entry].bitmap.check1 == 0 ? 1 : 0;
        activity.check1 = activity.check2 = read64_check;
        cxm_smem_data->cxm_smem_gsm2_check_count = (cxm_smem_data->cxm_smem_gsm2_check_count + 1) % 
          CXM_SMEM_CHECK_COUNT_MAX;
        cxm_smem_data->cxm_smem_gsm2_fw[cxm_smem_tech_fw.act[tech].next_entry].data = activity.data;
        /* log the data with current buffer index (it's important that we don't increment before logging)*/
        CXM_TRACE( 0, 0, 0, 0, CXM_LOG_PKT_EN, CXM_LOG_SMEM_GSM2_FW_DATA_V2 );
        cxm_smem_tech_fw.act[tech].next_entry = (cxm_smem_tech_fw.act[tech].next_entry + 1) % 
          CXM_SMEM_GSM_FW_BUF_SIZE;
        break;
      case CXM_TECH_GSM3:
        read64_check = 
          cxm_smem_data->cxm_smem_gsm3_fw[cxm_smem_tech_fw.act[tech].next_entry].bitmap.check1 == 0 ? 1 : 0;
        activity.check1 = activity.check2 = read64_check;
        cxm_smem_data->cxm_smem_gsm3_check_count = (cxm_smem_data->cxm_smem_gsm3_check_count + 1) % 
          CXM_SMEM_CHECK_COUNT_MAX;
        cxm_smem_data->cxm_smem_gsm3_fw[cxm_smem_tech_fw.act[tech].next_entry].data = activity.data;
        /* log the data with current buffer index (it's important that we don't increment before logging)*/
        CXM_TRACE( 0, 0, 0, 0, CXM_LOG_PKT_EN, CXM_LOG_SMEM_GSM3_FW_DATA_V2 );
        cxm_smem_tech_fw.act[tech].next_entry = (cxm_smem_tech_fw.act[tech].next_entry + 1) % 
          CXM_SMEM_GSM_FW_BUF_SIZE;
        break;
      default:
        break;
    }
    atomic_set( &cxm_smem_tech_fw.act[tech].writing, 0 );
  }
} /* cxm_smem_handle_fw_activity */





