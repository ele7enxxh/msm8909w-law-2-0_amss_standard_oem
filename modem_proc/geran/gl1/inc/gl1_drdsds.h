#ifndef GL1_DRDSDS_H
#define GL1_DRDSDS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      GERAN L1 DRDSDS HEADER FILE

GENERAL DESCRIPTION
This module handles DRDSDS specific functions.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009-2014 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl1/inc/gl1_drdsds.h#1 $
$DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who       what, where, why
--------   --------  ----------------------------------------------------------
25/09/14   pa		CR719024: Part2: Disable DRDSDS on receiving DSC_IND in DUAL_RX Tuneaway Mode.
09/09/14   pa      	CR719024: Implementation of Hybrid Tuneaway Fallback Algorithm

===========================================================================*/

/*===========================================================================

           INCLUDE FILES FOR MODULE

===========================================================================*/
#include "geran_variation.h"
#include "comdef.h"
#include "ms.h"

#include "l2_l1.h"
#include "l2_l1_g.h"
#include "msg.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
/*===========================================================================

           Structures, Enums and Hash definitions

===========================================================================*/
#define GL1_DRDSDS_THIRD_PAGING_BLOCK				(3)
#define GL1_DRDSDS_FOURTH_PAGING_BLOCK				(4)
#define GL1_DRDSDS_THIRD_BAD_PAGING_BLOCK			(3)
/* Threshold in dBmx16 -90dBm = -90*16*/
#define GL1_DRDSDS_RSSI_THRESHOLD					(-1440)
/*6dB => 96 dBx16*/
#define GL1_DRDSDS_SNR_THRESHOLD					(96)

/* Dual Receive Mode (Request Diversity with High/Low Priority */
typedef enum
{
   DRDSDS_RXD_NULL,
   DRDSDS_RXD_LP,
   DRDSDS_RXD_HP
}gl1_drdsds_drmode_rxd_priority;

/* Hybrid Tuneaway States to handle Hybrid Tuneaway Fallback Algorithm */
typedef enum
{
   DRDSDS_TUNEAWAY_MODE_STATE,
   /* 1 in 4 PB Dual Rx Tuneaway Mode state*/
   DRDSDS_DUAL_RX_TUNEAWAY_MODE_STATE,
   DRDSDS_LOW_PRIORITY_MODE_STATE,
   DRDSDS_HIGH_PRIORITY_MODE_STATE
}gl1_drdsds_hybrid_tuneaway_state;

/*Events which triggers Hybrid Tuneaway State machine*/
typedef enum
{
   DRDSDS_IDLE_INIT_EVENT, /* Gl1 eneters into L1_IDLE_INIT substate*/
   DRDSDS_MPH_BLOCK_QUALITY_IND_EVENT, /* Every decode of PCH block (Good, Bad, Unknown)*/
   DRDSDS_MPH_DSC_THRESHOLD_IND_EVENT /*MPH_DSC_THRESHOLD_IND from RR*/
}gl1_drdsds_hybrid_tuneaway_event;

/* Messages passed from L1/RR to Hybrid Tuneaway State Machine */
/* Union: if another message handling requires in future*/
typedef union
{
   mph_block_quality_ind_T	block_quality_ind;
}gl1_drdsds_hybrid_tuneaway_msg;

/*===========================================================================

           Functions declarations

===========================================================================*/
void gl1_drdsds_set_hybrid_tuneaway_state(gas_id_t gas_id);
void gl1_drdsds_hybrid_tuneaway_trigger(gl1_drdsds_hybrid_tuneaway_event event, 
										gl1_drdsds_hybrid_tuneaway_msg *msg, 
										gas_id_t gas_id);
void gl1_drdsds_handle_idle_init_event(gas_id_t gas_id);
void gl1_drdsds_handle_blk_quality_ind_event(mph_block_quality_ind_T *msg, 
											 gas_id_t gas_id);
void gl1_drdsds_handle_dsc_threshold_ind_event(gas_id_t gas_id);
void gl1_drdsds_process_blk_ind_tuneaway_mode(gas_id_t gas_id);
void gl1_drdsds_process_blk_ind_dual_rx_tuneaway_mode(gl1_block_quality_t blk_quality,
													  gas_id_t gas_id);
void gl1_drdsds_process_blk_ind_lp_mode(mph_block_quality_ind_T *msg, 
										gas_id_t gas_id);
void gl1_drdsds_process_blk_ind_hp_mode(mph_block_quality_ind_T *msg, 
										gas_id_t gas_id);
void gl1_drdsds_process_dsc_ind_tuneaway_mode(gas_id_t gas_id);
void gl1_drdsds_process_dsc_ind_dual_rx_tuneaway_mode(gas_id_t gas_id);
void gl1_drdsds_process_dsc_ind_lp_mode(gas_id_t gas_id);
void gl1_drdsds_process_dsc_ind_hp_mode(gas_id_t gas_id);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM*/
#endif /* GL1_DRDSDS_H */
