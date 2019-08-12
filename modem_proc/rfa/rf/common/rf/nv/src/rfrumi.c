



/*!
  @file rfrumi.c

  @brief
  This file contains all declarations and definitions for BOLT RUMI
  bring up
 

  @details

*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/08/15   mmd     Enabling RUMI featurization
09/26/14   as      Fixing non LTE config for Jolokia
08/14/14  tws/cpm  extended NV items for GSM Rumi
07/18/14   dr      Modified NV items for Jolokia Rumi
10/24/13   vv      Removing RUMI featurization
10/14/13   bsh     Added B3 and B4 support 
10/07/13   bsh     Added B7 and B41 support 
09/05/13   bsh     Added B17 critical NV: rx_c1_chan
08/16/13   bsh     Changing to B17 for LTE
08/07/13   dps     Remove empty initialization for rfrumi_item_list for Bolt
                   off-target compilation
08/01/13   bsh     Change val.rf_bc_config value
07/26/13   bsh     Initial checkin 
 
===========================================================================*/


#include "stringl.h" /* for memscpy */
#include "rfrumi.h"
#include "rfcommon_nv.h"
#include "rfnv_items.h"
#include "modem_mem.h"
#include "nv_items.h"
#ifdef FEATURE_LTE
#include "rflte_nv.h"
#endif
#include "rfgsm_nv_cmn.h"


#ifdef T_RUMI_EMULATION

#define RFRUMI_NV_ITEM_SIZE 5
#define RFFUMI_NV_ITEM_SIZE_B17 16
/*----------------------------------------------------------------------------*/
/* New structure for holding rumi item code and item value*/
typedef struct
{
   rfnv_item_id_enum_type  item_id;
   nv_item_type            val;
} rfrumi_nv_type;

typedef struct
{
   rfnv_item_id_enum_type  item_id;
   rfnv_item_type            val;
} rfrumi_rfnv_type;


void rfrumi_set_item_list_b17(void);
void rfrumi_set_item_list_b7(void);
void rfrumi_set_item_list_b41(void);
void rfrumi_set_item_list_b3(void);
void rfrumi_set_item_list_b4(void);

/*----------------------------------------------------------------------------*/
/* Step 1: Update the size in the above definition */
rfrumi_nv_type rfrumi_item_list[RFRUMI_NV_ITEM_SIZE];
rfrumi_nv_type rfrumi_item_list_b17[RFFUMI_NV_ITEM_SIZE_B17];
rfrumi_rfnv_type rfrumi_rfnv_item_list_b17[1];
rfrumi_nv_type rfrumi_item_list_b7[7];
rfrumi_rfnv_type rfrumi_rfnv_item_list_b7[1];
rfrumi_rfnv_type rfrumi_rfnv_item_list_b41[7];
rfrumi_rfnv_type rfrumi_rfnv_item_list_b3[7];
rfrumi_rfnv_type rfrumi_rfnv_item_list_b4[7];

/*----------------------------------------------------------------------------*/
/* Step 2: Assign values to the place holder: item_id and item value
   Please pay attention to array index numbers*/

void rfrumi_set_item_list(void)
{

   /*Common*/
   rfrumi_item_list[0].item_id = RFNV_RF_HW_CONFIG_I;
   rfrumi_item_list[0].val.rf_hw_config = (byte)0;       /*Configurable RF card*/

   rfrumi_item_list[1].item_id = RFNV_RF_BC_CONFIG_I;
   rfrumi_item_list[1].val.rf_bc_config = 562952216790407;
   

   rfrumi_item_list[2].item_id = RFNV_RF_BC_CONFIG_DIV_I;
   rfrumi_item_list[2].val.rf_bc_config_div = 562952216790279;

   #ifdef FEATURE_LTE
   rfrumi_item_list[3].item_id = RFNV_LTE_BC_CONFIG_I;
   rfrumi_item_list[3].val.lte_bc_config.lte_bc_config = 1099562027739; /*Only LTE B3, B4, B7, B17, B41 enabled*/
   #endif

   rfrumi_item_list[4].item_id = RFNV_RF_BC_CONFIG_C2_I;
   rfrumi_item_list[4].val.rf_bc_config = (uint64)0;
   /*End common*/

   
   rfrumi_set_item_list_b3();
   rfrumi_set_item_list_b4();
   rfrumi_set_item_list_b7();
   rfrumi_set_item_list_b17();
   rfrumi_set_item_list_b41();
   


} /* rfrumi_set_item_list */


/*----------------------------------------------------------------------------*/
/* Step 3: Add the case statement for the item code defined in step 2 above*/
/*!
  @brief
  This function copies one RF NV data item, as specified by 
  "item_code/identifier" into the pointer passed.
 
  @param item_code : Item to put
  @param rumi_item_ptr  : Pointer where to put the item
*/
boolean rfrumi_nv_get_item(rfnv_item_id_enum_type item_code,
                           nv_item_type *rumi_item_ptr)
{

   static int32 rfrumi_nv_init_done = FALSE;
   uint32 loop_ctr, loop_ctr2;

   if (!rfrumi_nv_init_done)
   {
      rfrumi_set_item_list();
      rfrumi_nv_init_done = TRUE;
   }
   switch (item_code)
   {
   case RFNV_RF_HW_CONFIG_I:
      rumi_item_ptr->rf_hw_config = rfrumi_item_list[0].val.rf_hw_config;
      break;

   case RFNV_RF_BC_CONFIG_I:
      rumi_item_ptr->rf_bc_config = rfrumi_item_list[1].val.rf_bc_config;
      break;

   case RFNV_RF_BC_CONFIG_DIV_I:
      rumi_item_ptr->rf_bc_config_div = rfrumi_item_list[2].val.rf_bc_config_div;
      break;

   #ifdef FEATURE_LTE
   case RFNV_LTE_BC_CONFIG_I:
      rumi_item_ptr->lte_bc_config.lte_bc_config =
         rfrumi_item_list[3].val.lte_bc_config.lte_bc_config;
      break;
    #endif

   case RFNV_RF_BC_CONFIG_C2_I:
      rumi_item_ptr->rf_bc_config = rfrumi_item_list[4].val.rf_bc_config;
      break;

      /*Rx channel*/
   #ifdef FEATURE_LTE
   case RFNV_LTE_B17_RX_CAL_CHAN_I:
   case RFNV_LTE_B17_C1_RX_CAL_CHAN_I:
      for (loop_ctr = 0; loop_ctr < LTE_RX_CAL_CHAN_SIZE; loop_ctr++)
      {
         rumi_item_ptr->lte_b17_rx_cal_chan[loop_ctr] =
            rfrumi_item_list_b17[0].val.lte_b17_rx_cal_chan[loop_ctr];
      }
      break;

      /*PA Range map*/
   case RFNV_LTE_B17_PA_RANGE_MAP_I:
      for (loop_ctr = 0; loop_ctr < 4; loop_ctr++)
      {
         rumi_item_ptr->lte_b17_pa_range_map[loop_ctr] =
            rfrumi_item_list_b17[1].val.lte_b17_pa_range_map[loop_ctr];
      }
      break;

      /*LNA Range Rise fall*/
   case RFNV_LTE_B17_LNA_RANGE_RISE_FALL_I:
      for (loop_ctr = 0; loop_ctr < 6; loop_ctr++)
      {
         rumi_item_ptr->lte_b17_lna_range_rise_fall[loop_ctr].rise_threshold =
            rfrumi_item_list_b17[2].val.lte_b17_lna_range_rise_fall[loop_ctr].rise_threshold;
         rumi_item_ptr->lte_b17_lna_range_rise_fall[loop_ctr].fall_threshold =
            rfrumi_item_list_b17[2].val.lte_b17_lna_range_rise_fall[loop_ctr].fall_threshold;
      }
      break;

      /*C1 LNA Range Rise Fall*/
   case RFNV_LTE_B17_C1_LNA_RANGE_RISE_FALL_I:
      for (loop_ctr = 0; loop_ctr < 6; loop_ctr++)
      {
         rumi_item_ptr->lte_b17_c1_lna_range_rise_fall[loop_ctr].rise_threshold =
            rfrumi_item_list_b17[2].val.lte_b17_lna_range_rise_fall[loop_ctr].rise_threshold;
         rumi_item_ptr->lte_b17_c1_lna_range_rise_fall[loop_ctr].fall_threshold =
            rfrumi_item_list_b17[2].val.lte_b17_lna_range_rise_fall[loop_ctr].fall_threshold;
      }
      break;

      /*Max Tx power*/
   case RFNV_LTE_B17_MAX_TX_POWER_I:
      rumi_item_ptr->lte_b17_max_tx_power = rfrumi_item_list_b17[3].val.lte_b17_max_tx_power;
      break;


      /*Timer hystersis*/
   case RFNV_LTE_B17_TIMER_HYSTERISIS_I:
      rumi_item_ptr->lte_b17_timer_hysterisis = rfrumi_item_list_b17[4].val.lte_b17_timer_hysterisis;
      break;


      /*PA Rise Fall Threshold*/
   case RFNV_LTE_B17_PA_RISE_FALL_THRESHOLD_I:
      rumi_item_ptr->lte_b17_pa_rise_fall_threshold[0].rise_threshold = rfrumi_item_list_b17[5].val.lte_b17_pa_rise_fall_threshold[0].rise_threshold;
      rumi_item_ptr->lte_b17_pa_rise_fall_threshold[0].fall_threshold = rfrumi_item_list_b17[5].val.lte_b17_pa_rise_fall_threshold[0].fall_threshold;
      rumi_item_ptr->lte_b17_pa_rise_fall_threshold[1].rise_threshold = rfrumi_item_list_b17[5].val.lte_b17_pa_rise_fall_threshold[1].rise_threshold;
      rumi_item_ptr->lte_b17_pa_rise_fall_threshold[1].fall_threshold = rfrumi_item_list_b17[5].val.lte_b17_pa_rise_fall_threshold[1].fall_threshold;
      rumi_item_ptr->lte_b17_pa_rise_fall_threshold[2].rise_threshold = rfrumi_item_list_b17[5].val.lte_b17_pa_rise_fall_threshold[2].rise_threshold;
      rumi_item_ptr->lte_b17_pa_rise_fall_threshold[2].fall_threshold = rfrumi_item_list_b17[5].val.lte_b17_pa_rise_fall_threshold[2].fall_threshold;
      break;

      /*EXP HDET vs AGC*/
   case RFNV_LTE_B17_EXP_HDET_VS_AGC_I:
      for (loop_ctr = 0; loop_ctr < LTE_HDET_VS_AGC_SIZ; loop_ctr++)
      {
         rumi_item_ptr->lte_b17_exp_hdet_vs_agc[loop_ctr] = rfrumi_item_list_b17[6].val.lte_b17_exp_hdet_vs_agc[loop_ctr];
      }
      break;

   case RFNV_LTE_B17_MIN_TX_POWER_DB10_I:
      memscpy(rumi_item_ptr,
              sizeof(rfnv_lte_min_tx_power_nv_db10_type),
              &(rfrumi_rfnv_item_list_b17[0].val),
              sizeof(rfnv_lte_min_tx_power_nv_db10_type));
      break;

   case RFNV_LTE_B17_TX_ROT_ANGLE_PA_STATE_I:
      rumi_item_ptr->lte_b17_tx_rot_angle_pa_state[0] = rfrumi_item_list_b17[8].val.lte_b17_tx_rot_angle_pa_state[0];
      rumi_item_ptr->lte_b17_tx_rot_angle_pa_state[1] = rfrumi_item_list_b17[8].val.lte_b17_tx_rot_angle_pa_state[1];
      rumi_item_ptr->lte_b17_tx_rot_angle_pa_state[2] = rfrumi_item_list_b17[8].val.lte_b17_tx_rot_angle_pa_state[2];
      rumi_item_ptr->lte_b17_tx_rot_angle_pa_state[3] = rfrumi_item_list_b17[8].val.lte_b17_tx_rot_angle_pa_state[3];
      break;

   case RFNV_LTE_B17_PA_COMPENSATE_UP_DOWN_I:
      rumi_item_ptr->lte_b17_pa_compensate_up_down[0].up_comp = rfrumi_item_list_b17[9].val.lte_b17_pa_compensate_up_down[0].up_comp;
      rumi_item_ptr->lte_b17_pa_compensate_up_down[0].down_comp = rfrumi_item_list_b17[9].val.lte_b17_pa_compensate_up_down[0].down_comp;
      rumi_item_ptr->lte_b17_pa_compensate_up_down[1].up_comp = rfrumi_item_list_b17[9].val.lte_b17_pa_compensate_up_down[1].up_comp;
      rumi_item_ptr->lte_b17_pa_compensate_up_down[1].down_comp = rfrumi_item_list_b17[9].val.lte_b17_pa_compensate_up_down[1].down_comp;
      rumi_item_ptr->lte_b17_pa_compensate_up_down[2].up_comp = rfrumi_item_list_b17[9].val.lte_b17_pa_compensate_up_down[2].up_comp;
      rumi_item_ptr->lte_b17_pa_compensate_up_down[2].down_comp = rfrumi_item_list_b17[9].val.lte_b17_pa_compensate_up_down[2].down_comp;
      rumi_item_ptr->lte_b17_pa_compensate_up_down[3].up_comp = rfrumi_item_list_b17[9].val.lte_b17_pa_compensate_up_down[3].up_comp;
      rumi_item_ptr->lte_b17_pa_compensate_up_down[3].down_comp = rfrumi_item_list_b17[9].val.lte_b17_pa_compensate_up_down[3].down_comp;
      break;

   case RFNV_LTE_B17_TX_LIN_VS_TEMP_I:
      for (loop_ctr = 0; loop_ctr < 4; loop_ctr++)
      {
         for (loop_ctr2 = 0; loop_ctr2 < NV_TEMP_TABLE_SIZ; loop_ctr2++)
         {
            rumi_item_ptr->lte_b17_tx_lin_vs_temp[loop_ctr][loop_ctr2] = rfrumi_item_list_b17[10].val.lte_b17_tx_lin_vs_temp[loop_ctr][loop_ctr2];
         }
      }
      break;

   case RFNV_LTE_B17_TX_LIMIT_VS_TEMP_I:
      for (loop_ctr = 0; loop_ctr < LTE_TX_TEMP_COMP_SIZ; loop_ctr++)
      {
         rumi_item_ptr->lte_b17_tx_limit_vs_temp[loop_ctr] = rfrumi_item_list_b17[11].val.lte_b17_tx_limit_vs_temp[loop_ctr];
      }
      break;

   case RFNV_LTE_B17_MPR_BASED_PA_SWITCHPOINTS_SHIFT_I:
      for (loop_ctr = 0; loop_ctr < LTE_MPR_SIZ; loop_ctr++)
      {
         rumi_item_ptr->lte_b17_mpr_based_pa_switchpoints_shift[loop_ctr] = rfrumi_item_list_b17[12].val.lte_b17_mpr_based_pa_switchpoints_shift[loop_ctr];
      }
      break;

   case RFNV_LTE_B17_TX_MPR_BACKOFF_I:
      for (loop_ctr = 0; loop_ctr < LTE_MPR_SIZ; loop_ctr++)
      {
         rumi_item_ptr->lte_b17_tx_mpr_backoff[loop_ctr] = rfrumi_item_list_b17[13].val.lte_b17_tx_mpr_backoff[loop_ctr];
      }
      break;

   case RFNV_LTE_B17_TX_DIGITAL_GAIN_COMP_I:
      for (loop_ctr = 0; loop_ctr < LTE_MPR_SIZ; loop_ctr++)
      {
         rumi_item_ptr->lte_b17_tx_digital_gain_comp[loop_ctr] = rfrumi_item_list_b17[14].val.lte_b17_tx_digital_gain_comp[loop_ctr];
      }
      break;

   case RFNV_LTE_B17_TX_AGC_OFFSET_I:
      for (loop_ctr = 0; loop_ctr < LTE_MPR_SIZ; loop_ctr++)
      {
         rumi_item_ptr->lte_b17_tx_agc_offset[loop_ctr] = rfrumi_item_list_b17[15].val.lte_b17_tx_agc_offset[loop_ctr];
      }
      break;

      /*LTE: rx channel list for B7*/
   case RFNV_LTE_B7_RX_CAL_CHAN_I:
      for (loop_ctr = 0; loop_ctr < LTE_RX_CAL_CHAN_SIZE; loop_ctr++)
      {
         rumi_item_ptr->lte_b7_rx_cal_chan[loop_ctr] = rfrumi_item_list_b7[0].val.lte_b7_rx_cal_chan[loop_ctr];
      }
      break;

      /*LTE B7: PA range map*/
   case RFNV_LTE_B7_PA_RANGE_MAP_I:
      rumi_item_ptr->lte_b7_pa_range_map[0] = rfrumi_item_list_b7[1].val.lte_b7_pa_range_map[0];
      rumi_item_ptr->lte_b7_pa_range_map[1] = rfrumi_item_list_b7[1].val.lte_b7_pa_range_map[1];
      rumi_item_ptr->lte_b7_pa_range_map[2] = rfrumi_item_list_b7[1].val.lte_b7_pa_range_map[2];
      rumi_item_ptr->lte_b7_pa_range_map[3] = rfrumi_item_list_b7[1].val.lte_b7_pa_range_map[3];
      break;

      /*LTE B7: LNA range for both C0 and C1*/
   case RFNV_LTE_B7_LNA_RANGE_RISE_FALL_I:
      rumi_item_ptr->lte_b7_lna_range_rise_fall[0].rise_threshold = rfrumi_item_list_b7[2].val.lte_b7_lna_range_rise_fall[0].rise_threshold;
      rumi_item_ptr->lte_b7_lna_range_rise_fall[1].rise_threshold = rfrumi_item_list_b7[2].val.lte_b7_lna_range_rise_fall[1].rise_threshold;
      rumi_item_ptr->lte_b7_lna_range_rise_fall[2].rise_threshold = rfrumi_item_list_b7[2].val.lte_b7_lna_range_rise_fall[2].rise_threshold;
      rumi_item_ptr->lte_b7_lna_range_rise_fall[3].rise_threshold = rfrumi_item_list_b7[2].val.lte_b7_lna_range_rise_fall[3].rise_threshold;
      rumi_item_ptr->lte_b7_lna_range_rise_fall[4].rise_threshold = rfrumi_item_list_b7[2].val.lte_b7_lna_range_rise_fall[4].rise_threshold;
      rumi_item_ptr->lte_b7_lna_range_rise_fall[5].rise_threshold = rfrumi_item_list_b7[2].val.lte_b7_lna_range_rise_fall[5].rise_threshold;

      rumi_item_ptr->lte_b7_lna_range_rise_fall[0].fall_threshold = rfrumi_item_list_b7[2].val.lte_b7_lna_range_rise_fall[0].fall_threshold;
      rumi_item_ptr->lte_b7_lna_range_rise_fall[1].fall_threshold = rfrumi_item_list_b7[2].val.lte_b7_lna_range_rise_fall[1].fall_threshold;
      rumi_item_ptr->lte_b7_lna_range_rise_fall[2].fall_threshold = rfrumi_item_list_b7[2].val.lte_b7_lna_range_rise_fall[2].fall_threshold;
      rumi_item_ptr->lte_b7_lna_range_rise_fall[3].fall_threshold = rfrumi_item_list_b7[2].val.lte_b7_lna_range_rise_fall[3].fall_threshold;
      rumi_item_ptr->lte_b7_lna_range_rise_fall[4].fall_threshold = rfrumi_item_list_b7[2].val.lte_b7_lna_range_rise_fall[4].fall_threshold;
      rumi_item_ptr->lte_b7_lna_range_rise_fall[5].fall_threshold = rfrumi_item_list_b7[2].val.lte_b7_lna_range_rise_fall[5].fall_threshold;
      break;

      /*LTE: Max Tx Power*/
   case RFNV_LTE_B7_MAX_TX_POWER_I:
      rumi_item_ptr->lte_b7_max_tx_power = rfrumi_item_list_b7[3].val.lte_b7_max_tx_power;
      break;

      /*LTE: Hysterisis*/
   case RFNV_LTE_B7_TIMER_HYSTERISIS_I:
      rumi_item_ptr->lte_b7_timer_hysterisis = rfrumi_item_list_b7[4].val.lte_b7_timer_hysterisis;
      break;

      /*LTE: PA rise fall threshold*/
   case RFNV_LTE_B7_PA_RISE_FALL_THRESHOLD_I:
      rumi_item_ptr->lte_b7_pa_rise_fall_threshold[0].rise_threshold = rfrumi_item_list_b7[5].val.lte_b7_pa_rise_fall_threshold[0].rise_threshold;
      rumi_item_ptr->lte_b7_pa_rise_fall_threshold[0].rise_threshold = rfrumi_item_list_b7[5].val.lte_b7_pa_rise_fall_threshold[0].fall_threshold;
      rumi_item_ptr->lte_b7_pa_rise_fall_threshold[1].rise_threshold = rfrumi_item_list_b7[5].val.lte_b7_pa_rise_fall_threshold[1].rise_threshold;
      rumi_item_ptr->lte_b7_pa_rise_fall_threshold[1].rise_threshold = rfrumi_item_list_b7[5].val.lte_b7_pa_rise_fall_threshold[1].fall_threshold;
      rumi_item_ptr->lte_b7_pa_rise_fall_threshold[2].rise_threshold = rfrumi_item_list_b7[5].val.lte_b7_pa_rise_fall_threshold[2].rise_threshold;
      rumi_item_ptr->lte_b7_pa_rise_fall_threshold[2].rise_threshold = rfrumi_item_list_b7[5].val.lte_b7_pa_rise_fall_threshold[2].fall_threshold;
      break;

      /*LTE: HDET vs AGC: Values picked from old reference QCN*/
   case RFNV_LTE_B7_EXP_HDET_VS_AGC_I:
      rumi_item_ptr->lte_b7_exp_hdet_vs_agc[0] = rfrumi_item_list_b7[6].val.lte_b7_exp_hdet_vs_agc[0];
      rumi_item_ptr->lte_b7_exp_hdet_vs_agc[1] = rfrumi_item_list_b7[6].val.lte_b7_exp_hdet_vs_agc[1];
      rumi_item_ptr->lte_b7_exp_hdet_vs_agc[2] = rfrumi_item_list_b7[6].val.lte_b7_exp_hdet_vs_agc[2];
      rumi_item_ptr->lte_b7_exp_hdet_vs_agc[3] = rfrumi_item_list_b7[6].val.lte_b7_exp_hdet_vs_agc[3];
      rumi_item_ptr->lte_b7_exp_hdet_vs_agc[4] = rfrumi_item_list_b7[6].val.lte_b7_exp_hdet_vs_agc[4];
      rumi_item_ptr->lte_b7_exp_hdet_vs_agc[5] = rfrumi_item_list_b7[6].val.lte_b7_exp_hdet_vs_agc[5];
      rumi_item_ptr->lte_b7_exp_hdet_vs_agc[6] = rfrumi_item_list_b7[6].val.lte_b7_exp_hdet_vs_agc[6];
      rumi_item_ptr->lte_b7_exp_hdet_vs_agc[7] = rfrumi_item_list_b7[6].val.lte_b7_exp_hdet_vs_agc[7];
      rumi_item_ptr->lte_b7_exp_hdet_vs_agc[8] = rfrumi_item_list_b7[6].val.lte_b7_exp_hdet_vs_agc[8];
      rumi_item_ptr->lte_b7_exp_hdet_vs_agc[9] = rfrumi_item_list_b7[6].val.lte_b7_exp_hdet_vs_agc[9];
      rumi_item_ptr->lte_b7_exp_hdet_vs_agc[10] = rfrumi_item_list_b7[6].val.lte_b7_exp_hdet_vs_agc[10];
      rumi_item_ptr->lte_b7_exp_hdet_vs_agc[11] = rfrumi_item_list_b7[6].val.lte_b7_exp_hdet_vs_agc[11];
      rumi_item_ptr->lte_b7_exp_hdet_vs_agc[12] = rfrumi_item_list_b7[6].val.lte_b7_exp_hdet_vs_agc[12];
      rumi_item_ptr->lte_b7_exp_hdet_vs_agc[13] = rfrumi_item_list_b7[6].val.lte_b7_exp_hdet_vs_agc[13];
      rumi_item_ptr->lte_b7_exp_hdet_vs_agc[14] = rfrumi_item_list_b7[6].val.lte_b7_exp_hdet_vs_agc[14];
      rumi_item_ptr->lte_b7_exp_hdet_vs_agc[15] = rfrumi_item_list_b7[6].val.lte_b7_exp_hdet_vs_agc[15];
      break;

      /*B41 Rx Channel*/
   case RFNV_LTE_B41_RX_CAL_CHAN_I:
      memscpy(rumi_item_ptr,
              sizeof(uint16)*16,
              &(rfrumi_rfnv_item_list_b41[0].val),
              sizeof(uint16)*16);
      break;

      /*LTE B41: PA range map*/
   case RFNV_LTE_B41_PA_RANGE_MAP_I:
      memscpy(rumi_item_ptr,
              sizeof(int8)*4,
              &(rfrumi_rfnv_item_list_b41[1].val),
              sizeof(int8)*4);
      break;

      /*LTE B41: LNA range for both C0 and C1*/
   case RFNV_LTE_B41_LNA_RANGE_RISE_FALL_I:
      memscpy(rumi_item_ptr,
              sizeof(rfnv_lte_lna_range_rise_fall_type) * 6,
              &(rfrumi_rfnv_item_list_b41[2].val),
              sizeof(rfnv_lte_lna_range_rise_fall_type) * 6);
      break;

      /*LTE 41: Max Tx Power*/
   case RFNV_LTE_B41_MAX_TX_POWER_I:
      memscpy(rumi_item_ptr,
              sizeof(int8),
              &(rfrumi_rfnv_item_list_b41[3].val),
              sizeof(int8));
      break;

      /*LTE 41: Hysterisis*/
   case RFNV_LTE_B41_TIMER_HYSTERISIS_I:
      memscpy(rumi_item_ptr,
              sizeof(int16),
              &(rfrumi_rfnv_item_list_b41[4].val),
              sizeof(int16));
      break;

      /*LTE 41: PA rise fall threshold*/
   case RFNV_LTE_B41_PA_RISE_FALL_THRESHOLD_I:
      memscpy(rumi_item_ptr,
              sizeof(rfnv_lte_pa_rise_fall_threshold_type) * 3,
              &(rfrumi_rfnv_item_list_b41[5].val),
              sizeof(rfnv_lte_pa_rise_fall_threshold_type) * 3);
      break;

      /*LTE 41: HDET vs AGC: Values picked from old reference QCN*/
   case RFNV_LTE_B41_EXP_HDET_VS_AGC_I:
      memscpy(rumi_item_ptr,
              sizeof(int16)*16,
              &(rfrumi_rfnv_item_list_b41[6].val),
              sizeof(int16)*16);
      break;

         /*B3 Rx Channel*/
   case RFNV_LTE_B3_RX_CAL_CHAN_I:
   case RFNV_LTE_B3_C1_RX_CAL_CHAN_I:
      memscpy(rumi_item_ptr,
              sizeof(uint16)*16,
              &(rfrumi_rfnv_item_list_b3[0].val),
              sizeof(uint16)*16);
      break;

      /*LTE B3: PA range map*/
   case RFNV_LTE_B3_PA_RANGE_MAP_I:
      memscpy(rumi_item_ptr,
              sizeof(int8)*4,
              &(rfrumi_rfnv_item_list_b3[1].val),
              sizeof(int8)*4);
      break;

      /*LTE B3: LNA range for both C0 and C1*/
   case RFNV_LTE_B3_LNA_RANGE_RISE_FALL_I:
      memscpy(rumi_item_ptr,
              sizeof(rfnv_lte_lna_range_rise_fall_type) * 6,
              &(rfrumi_rfnv_item_list_b3[2].val),
              sizeof(rfnv_lte_lna_range_rise_fall_type) * 6);
      break;

      /*LTE 3: Max Tx Power*/
   case RFNV_LTE_B3_MAX_TX_POWER_I:
      memscpy(rumi_item_ptr,
              sizeof(int8),
              &(rfrumi_rfnv_item_list_b3[3].val),
              sizeof(int8));
      break;

      /*LTE 3: Hysterisis*/
   case RFNV_LTE_B3_TIMER_HYSTERISIS_I:
      memscpy(rumi_item_ptr,
              sizeof(int16),
              &(rfrumi_rfnv_item_list_b3[4].val),
              sizeof(int16));
      break;

      /*LTE 3: PA rise fall threshold*/
   case RFNV_LTE_B3_PA_RISE_FALL_THRESHOLD_I:
      memscpy(rumi_item_ptr,
              sizeof(rfnv_lte_pa_rise_fall_threshold_type) * 3,
              &(rfrumi_rfnv_item_list_b3[5].val),
              sizeof(rfnv_lte_pa_rise_fall_threshold_type) * 3);
      break;

      /*LTE 3: HDET vs AGC: Values picked from old reference QCN*/
   case RFNV_LTE_B3_EXP_HDET_VS_AGC_I:
      memscpy(rumi_item_ptr,
              sizeof(int16)*16,
              &(rfrumi_rfnv_item_list_b3[6].val),
              sizeof(int16)*16);
      break;


         /*B4 Rx Channel*/
   case RFNV_LTE_B4_RX_CAL_CHAN_I:
   case RFNV_LTE_B4_C1_RX_CAL_CHAN_I:
      memscpy(rumi_item_ptr,
              sizeof(uint16)*16,
              &(rfrumi_rfnv_item_list_b4[0].val),
              sizeof(uint16)*16);
      break;

      /*LTE B4: PA range map*/
   case RFNV_LTE_B4_PA_RANGE_MAP_I:
      memscpy(rumi_item_ptr,
              sizeof(int8)*4,
              &(rfrumi_rfnv_item_list_b4[1].val),
              sizeof(int8)*4);
      break;

      /*LTE B4: LNA range for both C0 and C1*/
   case RFNV_LTE_B4_LNA_RANGE_RISE_FALL_I:
      memscpy(rumi_item_ptr,
              sizeof(rfnv_lte_lna_range_rise_fall_type) * 6,
              &(rfrumi_rfnv_item_list_b4[2].val),
              sizeof(rfnv_lte_lna_range_rise_fall_type) * 6);
      break;

      /*LTE 4: Max Tx Power*/
   case RFNV_LTE_B4_MAX_TX_POWER_I:
      memscpy(rumi_item_ptr,
              sizeof(int8),
              &(rfrumi_rfnv_item_list_b4[3].val),
              sizeof(int8));
      break;

      /*LTE 4: Hysterisis*/
   case RFNV_LTE_B4_TIMER_HYSTERISIS_I:
      memscpy(rumi_item_ptr,
              sizeof(int16),
              &(rfrumi_rfnv_item_list_b4[4].val),
              sizeof(int16));
      break;

      /*LTE 4: PA rise fall threshold*/
   case RFNV_LTE_B4_PA_RISE_FALL_THRESHOLD_I:
      memscpy(rumi_item_ptr,
              sizeof(rfnv_lte_pa_rise_fall_threshold_type) * 3,
              &(rfrumi_rfnv_item_list_b4[5].val),
              sizeof(rfnv_lte_pa_rise_fall_threshold_type) * 3);
      break;

      /*LTE 4: HDET vs AGC: Values picked from old reference QCN*/
   case RFNV_LTE_B4_EXP_HDET_VS_AGC_I:
      memscpy(rumi_item_ptr,
              sizeof(int16)*16,
              &(rfrumi_rfnv_item_list_b4[6].val),
              sizeof(int16)*16);
      break;

      /*CA NVs*/
   case RFNV_LTE_B3_CA_BC_CONFIG_I:
       memscpy(rumi_item_ptr,
             sizeof(uint64),
             &(rfrumi_rfnv_item_list_b3[0].val),
             sizeof(uint64));
      break;

    case RFNV_LTE_B7_CA_BC_CONFIG_I:
       memscpy(rumi_item_ptr,
             sizeof(uint64),
             &(rfrumi_rfnv_item_list_b7[0].val),
             sizeof(uint64));
      break;
    #endif
      /* GSM LNA switch points */
     case RFNV_GSM_C0_GSM850_LNA_SWPT_I:
     case RFNV_GSM_C0_GSM900_LNA_SWPT_I:
     case RFNV_GSM_C0_GSM1800_LNA_SWPT_I:
     case RFNV_GSM_C0_GSM1900_LNA_SWPT_I:
     case RFNV_GSM_C1_GSM850_LNA_SWPT_I:
     case RFNV_GSM_C1_GSM900_LNA_SWPT_I:
     case RFNV_GSM_C1_GSM1800_LNA_SWPT_I:
     case RFNV_GSM_C1_GSM1900_LNA_SWPT_I:
       {
         gsm_lna_switchpoint_type swpts = {-36, -45, -25, -33, -22, -42, 127, 127 };
         // All bands/devices use the same switchpoints
         NOTUSED(item_code);
         memscpy( rumi_item_ptr, sizeof(swpts),
                  &swpts, sizeof(swpts) );
       }
       break;

       /* GSM envelope frequency gain  */     
     case RFNV_GSM_C0_GSM850_ENV_GAIN_I:
     case RFNV_GSM_C0_GSM900_ENV_GAIN_I:
     case RFNV_GSM_C0_GSM1800_ENV_GAIN_I:
     case RFNV_GSM_C0_GSM1900_ENV_GAIN_I:
       {
         gsm_env_gain_type env_gain;
         uint8 n;
         for (n=0; n<RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE; n++)
         {
           env_gain.env_gain_freq_rgi_f1[n] = 1024;
           env_gain.env_gain_freq_rgi_f2[n] = 1024;
           env_gain.env_gain_freq_rgi_f3[n] = 1024;
         }
         memscpy( rumi_item_ptr, sizeof(env_gain),
                 &env_gain, sizeof(env_gain) );
       }
       break;
         
       /* GSM Vbatt config */     
     case RFNV_GSM_C0_GSM850_VBATT_I:
     case RFNV_GSM_C0_GSM900_VBATT_I:
     case RFNV_GSM_C0_GSM1800_VBATT_I:
     case RFNV_GSM_C0_GSM1900_VBATT_I:
       {
         gsm_vbatt_type vbatt = {0};         
         vbatt.vbatt_levels[0] = 3200;
         vbatt.vbatt_levels[1] = 3700;
         vbatt.vbatt_levels[2] = 4400;         
         memscpy( rumi_item_ptr, sizeof(vbatt),
                  &vbatt, sizeof(vbatt) );
       }
       break;
 
       /* GSM tx gain config */       
     case RFNV_GSM_C0_GSM850_LINEAR_TX_GAIN_PARAM_I:
     case RFNV_GSM_C0_GSM900_LINEAR_TX_GAIN_PARAM_I:
     case RFNV_GSM_C0_GSM1800_LINEAR_TX_GAIN_PARAM_I:
     case RFNV_GSM_C0_GSM1900_LINEAR_TX_GAIN_PARAM_I:
       {
         gsm_linear_tx_gain_param_type tx_gain;
         tx_gain.gsm_linear_tx_gain_val = 83;
         tx_gain.edge_linear_tx_gain_val = 47;
         tx_gain.charpredist_env_gain[0] = 5;
         tx_gain.charpredist_env_gain[1] = 15;
         tx_gain.charpredist_env_gain[2] = 40;
         tx_gain.charpredist_env_gain[3] = 60;
         tx_gain.charpredist_env_gain[4] = 1;
         tx_gain.charpredist_env_gain[5] = 2;
         tx_gain.charpredist_env_gain[6] = 3;
         tx_gain.charpredist_env_gain[7] = 4;
         tx_gain.charpredist_env_gain[8] = 7;
         tx_gain.charpredist_env_gain[9] = 9;
         tx_gain.charpredist_env_gain[10] = 20;
         tx_gain.charpredist_env_gain[11] = 25;
         tx_gain.charpredist_env_gain[12] = 30;
         tx_gain.charpredist_env_gain[13] = 50;
         tx_gain.charpredist_env_gain[14] = 55;
         tx_gain.charpredist_env_gain[15] = 0;         
         memscpy( rumi_item_ptr, sizeof(tx_gain),
                  &tx_gain, sizeof(tx_gain) );
       }
       break;
         
       /* GSM polar path delay */     
     case RFNV_GSM_C0_GSM850_POLAR_PATH_DELAY_I:
     case RFNV_GSM_C0_GSM900_POLAR_PATH_DELAY_I:
     case RFNV_GSM_C0_GSM1800_POLAR_PATH_DELAY_I:
     case RFNV_GSM_C0_GSM1900_POLAR_PATH_DELAY_I:
       {
         int16 polar_path_delay = 925;
         memscpy( rumi_item_ptr, sizeof(polar_path_delay),
                  &polar_path_delay, sizeof(polar_path_delay) );
       }
       break;

       /* GSM power levels */     
     case RFNV_GSM_C0_GSM850_POWER_LEVELS_I:
     case RFNV_GSM_C0_GSM900_POWER_LEVELS_I:
     case RFNV_GSM_C0_GSM1800_POWER_LEVELS_I:
     case RFNV_GSM_C0_GSM1900_POWER_LEVELS_I:
       {
         gsm_power_levels power_levels = {450, 650, 850, 1050, 1250, 1450, 1650,
                                          1850, 2050, 2250, 2450, 2650, 2850, 3020, 3250, 3250 };
         memscpy( rumi_item_ptr, sizeof(power_levels),
                  &power_levels, sizeof(power_levels) );
       }
       break;

       /* GSM tx timings */     
     case RFNV_GSM_C0_GSM850_TX_TIMING_I:
     case RFNV_GSM_C0_GSM900_TX_TIMING_I:
     case RFNV_GSM_C0_GSM1800_TX_TIMING_I:
     case RFNV_GSM_C0_GSM1900_TX_TIMING_I:
       {
         gsm_tx_timing_data_type tx_timing = { -60, 9, -4, 5, -3, -10, 0 };
         memscpy( rumi_item_ptr, sizeof(tx_timing),
                  &tx_timing, sizeof(tx_timing) );
       }
       break;

       /* GSM   pa range map */     
     case RFNV_GSM_C0_GSM850_PA_RANGE_MAP_I:
     case RFNV_GSM_C0_GSM900_PA_RANGE_MAP_I:
     case RFNV_GSM_C0_GSM1800_PA_RANGE_MAP_I:
     case RFNV_GSM_C0_GSM1900_PA_RANGE_MAP_I:
       {
         uint32 pa_range_map = 1985229328;         
         memscpy( rumi_item_ptr, sizeof(pa_range_map),
                  &pa_range_map, sizeof(pa_range_map) );
       }
       break;
       
       /* GSM  extended PA switch points */    
     case RFNV_GSM_C0_GSM850_EXTENDED_PA_SWPT_I:
     case RFNV_GSM_C0_GSM900_EXTENDED_PA_SWPT_I:
     case RFNV_GSM_C0_GSM1800_EXTENDED_PA_SWPT_I:
     case RFNV_GSM_C0_GSM1900_EXTENDED_PA_SWPT_I:
       { 
         gsm_extended_pa_switchpoint_type pa_swtps;
         pa_swtps.gmsk_pa_swpt_r0_to_r1 = 65535;
         pa_swtps.gmsk_pa_swpt_r1_to_r2 = 65535;
         pa_swtps.gmsk_pa_swpt_r2_to_r3 = 65535;
         pa_swtps.gmsk_pa_swpt_r3_to_r4 = 65535;
         pa_swtps.gmsk_pa_swpt_r4_to_r5 = 65535;
         pa_swtps.gmsk_pa_swpt_r5_to_r6 = 65535;
         pa_swtps.gmsk_pa_swpt_r6_to_r7 = 65535;
         pa_swtps.edge_pa_swpt_r0_to_r1 = 65535;
         pa_swtps.edge_pa_swpt_r1_to_r2 = 65535;
         pa_swtps.edge_pa_swpt_r2_to_r3 = 65535;
         pa_swtps.edge_pa_swpt_r3_to_r4 = 65535;
         pa_swtps.edge_pa_swpt_r4_to_r5 = 65535;
         pa_swtps.edge_pa_swpt_r5_to_r6 = 65535;
         pa_swtps.edge_pa_swpt_r6_to_r7 = 65535;
         pa_swtps.pa_predist_swpt1 = 2250;
         pa_swtps.pa_predist_swpt2 = 65535;
         memscpy( rumi_item_ptr, sizeof(pa_swtps),
                  &pa_swtps, sizeof(pa_swtps) );
       }
       break;


       /* GSM  temperature  compensation */
     case RFNV_GSM_C0_GSM850_TEMP_COMP_I:
     case RFNV_GSM_C0_GSM900_TEMP_COMP_I:
     case RFNV_GSM_C0_GSM1800_TEMP_COMP_I:
     case RFNV_GSM_C0_GSM1900_TEMP_COMP_I:
       {
         gsm_temp_comp_data_type temp_comp = {0};
         memscpy( rumi_item_ptr, sizeof(temp_comp),
                  &temp_comp, sizeof(temp_comp) );
       }
       break;

       /* GSM  SMPS PDM table */   
     case RFNV_GSM_C0_GSM850_EXTENDED_SMPS_PDM_TBL_I:
     case RFNV_GSM_C0_GSM900_EXTENDED_SMPS_PDM_TBL_I:
     case RFNV_GSM_C0_GSM1800_EXTENDED_SMPS_PDM_TBL_I:
     case RFNV_GSM_C0_GSM1900_EXTENDED_SMPS_PDM_TBL_I:
       {
         gsm_smps_pdm_tbl_type smps_pdm;
         uint8 n;
         for(n=0;n<RFGSM_MAX_TX_GAIN_RANGE;n++)
         {
           smps_pdm.cal_smps_pdm_tbl[n] = 3400;
         }
         for (n=0;n<RFNV_NUMBER_OF_PCLS;n++)
         {
           smps_pdm.gsm_smps_pdm_tbl[n] = 3400;
           smps_pdm.edge_smps_pdm_tbl[n] = 3400;
         }
         memscpy( rumi_item_ptr, sizeof(smps_pdm),
                  &smps_pdm, sizeof(smps_pdm) );
       }
       break;

       /* GSM  PA ramp table */   
     case RFNV_GSM_C0_GSM850_POLAR_RAMP_PROFILE_I:
     case RFNV_GSM_C0_GSM900_POLAR_RAMP_PROFILE_I:
     case RFNV_GSM_C0_GSM1800_POLAR_RAMP_PROFILE_I:
     case RFNV_GSM_C0_GSM1900_POLAR_RAMP_PROFILE_I:
       {
         gsm_pa_ramp_lut_type pa_ramp_lut;
         // Ramp UP
         pa_ramp_lut.ramp_up[0] = 0;
         pa_ramp_lut.ramp_up[1] = 0;
         pa_ramp_lut.ramp_up[2] = 0;
         pa_ramp_lut.ramp_up[3] = 150;
         pa_ramp_lut.ramp_up[4] = 300;
         pa_ramp_lut.ramp_up[5] = 496;
         pa_ramp_lut.ramp_up[6] = 758;
         pa_ramp_lut.ramp_up[7] = 1124;
         pa_ramp_lut.ramp_up[8] = 1581;
         pa_ramp_lut.ramp_up[9] = 2123;
         pa_ramp_lut.ramp_up[10] = 2737;
         pa_ramp_lut.ramp_up[11] = 3411;
         pa_ramp_lut.ramp_up[12] = 4128;
         pa_ramp_lut.ramp_up[13] = 4872;
         pa_ramp_lut.ramp_up[14] = 5626;
         pa_ramp_lut.ramp_up[15] = 6367;
         pa_ramp_lut.ramp_up[16] = 7077;
         pa_ramp_lut.ramp_up[17] = 7737;
         pa_ramp_lut.ramp_up[18] = 8331;
         pa_ramp_lut.ramp_up[19] = 8842;
         pa_ramp_lut.ramp_up[20] = 9252;
         pa_ramp_lut.ramp_up[21] = 9556;
         pa_ramp_lut.ramp_up[22] = 9740;
         pa_ramp_lut.ramp_up[23] = 9800;
         pa_ramp_lut.ramp_up[24] = 9800;
         pa_ramp_lut.ramp_up[25] = 9800;
         pa_ramp_lut.ramp_up[26] = 9800;
         pa_ramp_lut.ramp_up[27] = 9800;
         pa_ramp_lut.ramp_up[28] = 9800;
         pa_ramp_lut.ramp_up[29] = 9800;
         // Ramp DOWN
         pa_ramp_lut.ramp_down[0] = 9800;
         pa_ramp_lut.ramp_down[1] = 9740;
         pa_ramp_lut.ramp_down[2] = 9556;
         pa_ramp_lut.ramp_down[3] = 9252;
         pa_ramp_lut.ramp_down[4] = 8842;
         pa_ramp_lut.ramp_down[5] = 8331;
         pa_ramp_lut.ramp_down[6] = 7737;
         pa_ramp_lut.ramp_down[7] = 7077;
         pa_ramp_lut.ramp_down[8] = 6367;
         pa_ramp_lut.ramp_down[9] = 5626;
         pa_ramp_lut.ramp_down[10] = 4872;
         pa_ramp_lut.ramp_down[11] = 4128;
         pa_ramp_lut.ramp_down[12] = 3411;
         pa_ramp_lut.ramp_down[13] = 2737;
         pa_ramp_lut.ramp_down[14] = 2123;
         pa_ramp_lut.ramp_down[15] = 1581;
         pa_ramp_lut.ramp_down[16] = 1124;
         pa_ramp_lut.ramp_down[17] = 758;
         pa_ramp_lut.ramp_down[18] = 496;
         pa_ramp_lut.ramp_down[19] = 300;
         pa_ramp_lut.ramp_down[20] = 150;
         pa_ramp_lut.ramp_down[21] = 0;
         pa_ramp_lut.ramp_down[22] = 0;
         pa_ramp_lut.ramp_down[23] = 0;
         pa_ramp_lut.ramp_down[24] = 0;
         pa_ramp_lut.ramp_down[25] = 0;
         pa_ramp_lut.ramp_down[26] = 0;
         pa_ramp_lut.ramp_down[27] = 0;
         pa_ramp_lut.ramp_down[28] = 0;
         pa_ramp_lut.ramp_down[29] = 0;
         memscpy( rumi_item_ptr, sizeof(pa_ramp_lut),
                  &pa_ramp_lut, sizeof(pa_ramp_lut) );
       }
       break;

       /* GSM  AMAM temperature compensation  */   
     case RFNV_GSM_C0_GSM850_AMAM_TEMP_COMP_I:
     case RFNV_GSM_C0_GSM900_AMAM_TEMP_COMP_I:
     case RFNV_GSM_C0_GSM1800_AMAM_TEMP_COMP_I:
     case RFNV_GSM_C0_GSM1900_AMAM_TEMP_COMP_I:
       {
         amam_temp_comp amam_temp = {0};
         memscpy( rumi_item_ptr, sizeof(amam_temp),
                  &amam_temp, sizeof(amam_temp) );
       }
       break;

       /* GSM   enhanced APT table */   
     case RFNV_GSM_C0_GSM850_ENH_APT_I:
     case RFNV_GSM_C0_GSM900_ENH_APT_I:
     case RFNV_GSM_C0_GSM1800_ENH_APT_I:
     case RFNV_GSM_C0_GSM1900_ENH_APT_I:
       {
         enh_apt_data_type enh_apt = {13, 3400, 3950};
         memscpy( rumi_item_ptr, sizeof(enh_apt),
                  &enh_apt, sizeof(enh_apt) );
       }
       break;

     default:
         return (FALSE);
   }

   return (TRUE);

} /* rfrumi_nv_get_item */


void rfrumi_set_item_list_b17(void)
{
   uint8 loop_ctr, loop_ctr2;

   /*LTE: rx channel list for B17*/
   rfrumi_item_list_b17[0].item_id = RFNV_LTE_B17_RX_CAL_CHAN_I;
   for (loop_ctr = 0; loop_ctr < LTE_RX_CAL_CHAN_SIZE; loop_ctr++)
   {
      rfrumi_item_list_b17[0].val.lte_b17_rx_cal_chan[loop_ctr] = (uint16)(23730 + loop_ctr * 7);
   }

   /*LTE: PA range map*/
   rfrumi_item_list_b17[1].item_id = RFNV_LTE_B17_PA_RANGE_MAP_I;
   rfrumi_item_list_b17[1].val.lte_b17_pa_range_map[0] = (int8)2;
   rfrumi_item_list_b17[1].val.lte_b17_pa_range_map[1] = (int8)2;
   rfrumi_item_list_b17[1].val.lte_b17_pa_range_map[2] = (int8)3;
   rfrumi_item_list_b17[1].val.lte_b17_pa_range_map[3] = (int8)3;

   /*LTE: LNA range for both C0 and C1*/
   rfrumi_item_list_b17[2].item_id = RFNV_LTE_B17_LNA_RANGE_RISE_FALL_I;
   rfrumi_item_list_b17[2].val.lte_b17_lna_range_rise_fall[0].rise_threshold = (int16) - 640;
   rfrumi_item_list_b17[2].val.lte_b17_lna_range_rise_fall[1].rise_threshold = (int16) - 590;
   rfrumi_item_list_b17[2].val.lte_b17_lna_range_rise_fall[2].rise_threshold = (int16) - 480;
   rfrumi_item_list_b17[2].val.lte_b17_lna_range_rise_fall[3].rise_threshold = (int16) - 370;
   rfrumi_item_list_b17[2].val.lte_b17_lna_range_rise_fall[4].rise_threshold = (int16) - 260;
   rfrumi_item_list_b17[2].val.lte_b17_lna_range_rise_fall[5].rise_threshold = (int16)0;

   rfrumi_item_list_b17[2].val.lte_b17_lna_range_rise_fall[0].fall_threshold = (int16) - 600;
   rfrumi_item_list_b17[2].val.lte_b17_lna_range_rise_fall[1].fall_threshold = (int16) - 550;
   rfrumi_item_list_b17[2].val.lte_b17_lna_range_rise_fall[2].fall_threshold = (int16) - 440;
   rfrumi_item_list_b17[2].val.lte_b17_lna_range_rise_fall[3].fall_threshold = (int16) - 330;
   rfrumi_item_list_b17[2].val.lte_b17_lna_range_rise_fall[4].fall_threshold = (int16) - 220;
   rfrumi_item_list_b17[2].val.lte_b17_lna_range_rise_fall[5].fall_threshold = (int16)0;

   /*LTE: Max Tx Power*/
   rfrumi_item_list_b17[3].item_id = RFNV_LTE_B17_MAX_TX_POWER_I;
   rfrumi_item_list_b17[3].val.lte_b17_max_tx_power = (int8)23;

   /*LTE: Hysterisis*/
   rfrumi_item_list_b17[4].item_id = RFNV_LTE_B17_TIMER_HYSTERISIS_I;
   rfrumi_item_list_b17[4].val.lte_b17_timer_hysterisis = (int16)4;

   /*LTE: PA rise fall threshold*/
   rfrumi_item_list_b17[5].item_id = RFNV_LTE_B17_PA_RISE_FALL_THRESHOLD_I;
   rfrumi_item_list_b17[5].val.lte_b17_pa_rise_fall_threshold[0].rise_threshold = (uint16)765;
   rfrumi_item_list_b17[5].val.lte_b17_pa_rise_fall_threshold[0].fall_threshold = (uint16)735;
   rfrumi_item_list_b17[5].val.lte_b17_pa_rise_fall_threshold[1].rise_threshold = (uint16)765;
   rfrumi_item_list_b17[5].val.lte_b17_pa_rise_fall_threshold[1].fall_threshold = (uint16)735;
   rfrumi_item_list_b17[5].val.lte_b17_pa_rise_fall_threshold[2].rise_threshold = (uint16)765;
   rfrumi_item_list_b17[5].val.lte_b17_pa_rise_fall_threshold[2].fall_threshold = (uint16)735;

   /*LTE: HDET vs AGC: Values picked from old reference QCN*/
   rfrumi_item_list_b17[6].item_id = RFNV_LTE_B17_EXP_HDET_VS_AGC_I;
   rfrumi_item_list_b17[6].val.lte_b17_exp_hdet_vs_agc[0] = (uint16)931;
   rfrumi_item_list_b17[6].val.lte_b17_exp_hdet_vs_agc[1] = (uint16)942;
   rfrumi_item_list_b17[6].val.lte_b17_exp_hdet_vs_agc[2] = (uint16)958;
   rfrumi_item_list_b17[6].val.lte_b17_exp_hdet_vs_agc[3] = (uint16)979;
   rfrumi_item_list_b17[6].val.lte_b17_exp_hdet_vs_agc[4] = (uint16)1007;
   rfrumi_item_list_b17[6].val.lte_b17_exp_hdet_vs_agc[5] = (uint16)1051;
   rfrumi_item_list_b17[6].val.lte_b17_exp_hdet_vs_agc[6] = (uint16)1111;
   rfrumi_item_list_b17[6].val.lte_b17_exp_hdet_vs_agc[7] = (uint16)1195;
   rfrumi_item_list_b17[6].val.lte_b17_exp_hdet_vs_agc[8] = (uint16)1306;
   rfrumi_item_list_b17[6].val.lte_b17_exp_hdet_vs_agc[9] = (uint16)1452;
   rfrumi_item_list_b17[6].val.lte_b17_exp_hdet_vs_agc[10] = (uint16)1645;
   rfrumi_item_list_b17[6].val.lte_b17_exp_hdet_vs_agc[11] = (uint16)1891;
   rfrumi_item_list_b17[6].val.lte_b17_exp_hdet_vs_agc[12] = (uint16)2207;
   rfrumi_item_list_b17[6].val.lte_b17_exp_hdet_vs_agc[13] = (uint16)2613;
   rfrumi_item_list_b17[6].val.lte_b17_exp_hdet_vs_agc[14] = (uint16)3066;
   rfrumi_item_list_b17[6].val.lte_b17_exp_hdet_vs_agc[15] = (uint16)3519;

   /*Min Tx Power: Please note two different types*/
   rfrumi_item_list_b17[7].item_id = RFNV_LTE_B17_MIN_TX_POWER_DB10_I;
   rfrumi_rfnv_item_list_b17[0].val[0] = 01;
   rfrumi_rfnv_item_list_b17[0].val[1] = 00;
   rfrumi_rfnv_item_list_b17[0].val[2] = 0x48;
   rfrumi_rfnv_item_list_b17[0].val[3] = 0xFE;


   /*Tx Rot angle*/
   rfrumi_item_list_b17[8].item_id = RFNV_LTE_B17_TX_ROT_ANGLE_PA_STATE_I;
   rfrumi_item_list_b17[8].val.lte_b17_tx_rot_angle_pa_state[0] = 0;
   rfrumi_item_list_b17[8].val.lte_b17_tx_rot_angle_pa_state[1] = 0;
   rfrumi_item_list_b17[8].val.lte_b17_tx_rot_angle_pa_state[2] = 0;
   rfrumi_item_list_b17[8].val.lte_b17_tx_rot_angle_pa_state[3] = 0;

   /*PA Compensate*/
   rfrumi_item_list_b17[9].item_id = RFNV_LTE_B17_PA_COMPENSATE_UP_DOWN_I;
   rfrumi_item_list_b17[9].val.lte_b17_pa_compensate_up_down[0].up_comp = 0;
   rfrumi_item_list_b17[9].val.lte_b17_pa_compensate_up_down[0].down_comp = 0;
   rfrumi_item_list_b17[9].val.lte_b17_pa_compensate_up_down[1].up_comp = 0;
   rfrumi_item_list_b17[9].val.lte_b17_pa_compensate_up_down[1].down_comp = 0;
   rfrumi_item_list_b17[9].val.lte_b17_pa_compensate_up_down[2].up_comp = 0;
   rfrumi_item_list_b17[9].val.lte_b17_pa_compensate_up_down[2].down_comp = 0;
   rfrumi_item_list_b17[9].val.lte_b17_pa_compensate_up_down[3].up_comp = 0;
   rfrumi_item_list_b17[9].val.lte_b17_pa_compensate_up_down[3].down_comp = 0;

   /*Tx Lin vs temp*/
   rfrumi_item_list_b17[10].item_id = RFNV_LTE_B17_TX_LIN_VS_TEMP_I;
   for (loop_ctr = 0; loop_ctr < 4; loop_ctr++)
   {
      for (loop_ctr2 = 0; loop_ctr2 < NV_TEMP_TABLE_SIZ; loop_ctr2++)
      {
         rfrumi_item_list_b17[10].val.lte_b17_tx_lin_vs_temp[loop_ctr][loop_ctr2] = 0;
      }
   }

   /*Tx Limit vs temp*/
   rfrumi_item_list_b17[11].item_id = RFNV_LTE_B17_TX_LIMIT_VS_TEMP_I;
   for (loop_ctr = 0; loop_ctr < LTE_TX_TEMP_COMP_SIZ; loop_ctr++)
   {
      rfrumi_item_list_b17[11].val.lte_b17_tx_limit_vs_temp[loop_ctr] = 0;
   }

   /*MPR based PA switch points*/
   rfrumi_item_list_b17[12].item_id = RFNV_LTE_B17_MPR_BASED_PA_SWITCHPOINTS_SHIFT_I;
   for (loop_ctr = 0; loop_ctr < LTE_MPR_SIZ; loop_ctr++)
   {
      rfrumi_item_list_b17[12].val.lte_b17_mpr_based_pa_switchpoints_shift[loop_ctr] = 0;
   }

   /*MPR Back off*/
   rfrumi_item_list_b17[13].item_id = RFNV_LTE_B17_TX_MPR_BACKOFF_I;
   for (loop_ctr = 0; loop_ctr < LTE_MPR_SIZ; loop_ctr++)
   {
      rfrumi_item_list_b17[13].val.lte_b17_tx_mpr_backoff[loop_ctr] = 0;
   }

   /*Tx Digital gain*/
   rfrumi_item_list_b17[14].item_id = RFNV_LTE_B17_TX_DIGITAL_GAIN_COMP_I;
   for (loop_ctr = 0; loop_ctr < LTE_MPR_SIZ; loop_ctr++)
   {
      rfrumi_item_list_b17[14].val.lte_b17_tx_digital_gain_comp[loop_ctr] = 0;
   }

   /*Tx AGC offset*/
   rfrumi_item_list_b17[15].item_id = RFNV_LTE_B17_TX_AGC_OFFSET_I;
   for (loop_ctr = 0; loop_ctr < LTE_MPR_SIZ; loop_ctr++)
   {
      rfrumi_item_list_b17[15].val.lte_b17_tx_agc_offset[loop_ctr] = 0;
   }

}

void rfrumi_set_item_list_b7(void)
{

   uint32 loop_ctr;

   /*LTE: rx channel list for B7*/
   rfrumi_item_list_b7[0].item_id = RFNV_LTE_B7_RX_CAL_CHAN_I;
   for (loop_ctr = 0; loop_ctr < LTE_RX_CAL_CHAN_SIZE; loop_ctr++)
   {
      rfrumi_item_list_b7[0].val.lte_b7_rx_cal_chan[loop_ctr] = (uint16)(20750 + loop_ctr * 46);
   }

   /*LTE B7: PA range map*/
   rfrumi_item_list_b7[1].item_id = RFNV_LTE_B7_PA_RANGE_MAP_I;
   rfrumi_item_list_b7[1].val.lte_b7_pa_range_map[0] = (int8)2;
   rfrumi_item_list_b7[1].val.lte_b7_pa_range_map[1] = (int8)2;
   rfrumi_item_list_b7[1].val.lte_b7_pa_range_map[2] = (int8)3;
   rfrumi_item_list_b7[1].val.lte_b7_pa_range_map[3] = (int8)3;

   /*LTE B7: LNA range for both C0 and C1*/
   rfrumi_item_list_b7[2].item_id = RFNV_LTE_B7_LNA_RANGE_RISE_FALL_I;
   rfrumi_item_list_b7[2].val.lte_b7_lna_range_rise_fall[0].rise_threshold = (int16) - 640;
   rfrumi_item_list_b7[2].val.lte_b7_lna_range_rise_fall[1].rise_threshold = (int16) - 590;
   rfrumi_item_list_b7[2].val.lte_b7_lna_range_rise_fall[2].rise_threshold = (int16) - 480;
   rfrumi_item_list_b7[2].val.lte_b7_lna_range_rise_fall[3].rise_threshold = (int16) - 370;
   rfrumi_item_list_b7[2].val.lte_b7_lna_range_rise_fall[4].rise_threshold = (int16) - 260;
   rfrumi_item_list_b7[2].val.lte_b7_lna_range_rise_fall[5].rise_threshold = (int16)0;

   rfrumi_item_list_b7[2].val.lte_b7_lna_range_rise_fall[0].fall_threshold = (int16) - 600;
   rfrumi_item_list_b7[2].val.lte_b7_lna_range_rise_fall[1].fall_threshold = (int16) - 550;
   rfrumi_item_list_b7[2].val.lte_b7_lna_range_rise_fall[2].fall_threshold = (int16) - 440;
   rfrumi_item_list_b7[2].val.lte_b7_lna_range_rise_fall[3].fall_threshold = (int16) - 330;
   rfrumi_item_list_b7[2].val.lte_b7_lna_range_rise_fall[4].fall_threshold = (int16) - 220;
   rfrumi_item_list_b7[2].val.lte_b7_lna_range_rise_fall[5].fall_threshold = (int16)0;

   /*LTE: Max Tx Power*/
   rfrumi_item_list_b7[3].item_id = RFNV_LTE_B7_MAX_TX_POWER_I;
   rfrumi_item_list_b7[3].val.lte_b7_max_tx_power = (int8)23;

   /*LTE: Hysterisis*/
   rfrumi_item_list_b7[4].item_id = RFNV_LTE_B7_TIMER_HYSTERISIS_I;
   rfrumi_item_list_b7[4].val.lte_b7_timer_hysterisis = (int16)4;

   /*LTE: PA rise fall threshold*/
   rfrumi_item_list_b7[5].item_id = RFNV_LTE_B7_PA_RISE_FALL_THRESHOLD_I;
   rfrumi_item_list_b7[5].val.lte_b7_pa_rise_fall_threshold[0].rise_threshold = (uint16)765;
   rfrumi_item_list_b7[5].val.lte_b7_pa_rise_fall_threshold[0].fall_threshold = (uint16)735;
   rfrumi_item_list_b7[5].val.lte_b7_pa_rise_fall_threshold[1].rise_threshold = (uint16)765;
   rfrumi_item_list_b7[5].val.lte_b7_pa_rise_fall_threshold[1].fall_threshold = (uint16)735;
   rfrumi_item_list_b7[5].val.lte_b7_pa_rise_fall_threshold[2].rise_threshold = (uint16)765;
   rfrumi_item_list_b7[5].val.lte_b7_pa_rise_fall_threshold[2].fall_threshold = (uint16)735;

   /*LTE: HDET vs AGC: Values picked from old reference QCN*/
   rfrumi_item_list_b7[6].item_id = RFNV_LTE_B7_EXP_HDET_VS_AGC_I;
   rfrumi_item_list_b7[6].val.lte_b7_exp_hdet_vs_agc[0] = (uint16)931;
   rfrumi_item_list_b7[6].val.lte_b7_exp_hdet_vs_agc[1] = (uint16)942;
   rfrumi_item_list_b7[6].val.lte_b7_exp_hdet_vs_agc[2] = (uint16)958;
   rfrumi_item_list_b7[6].val.lte_b7_exp_hdet_vs_agc[3] = (uint16)979;
   rfrumi_item_list_b7[6].val.lte_b7_exp_hdet_vs_agc[4] = (uint16)1007;
   rfrumi_item_list_b7[6].val.lte_b7_exp_hdet_vs_agc[5] = (uint16)1051;
   rfrumi_item_list_b7[6].val.lte_b7_exp_hdet_vs_agc[6] = (uint16)1111;
   rfrumi_item_list_b7[6].val.lte_b7_exp_hdet_vs_agc[7] = (uint16)1195;
   rfrumi_item_list_b7[6].val.lte_b7_exp_hdet_vs_agc[8] = (uint16)1306;
   rfrumi_item_list_b7[6].val.lte_b7_exp_hdet_vs_agc[9] = (uint16)1452;
   rfrumi_item_list_b7[6].val.lte_b7_exp_hdet_vs_agc[10] = (uint16)1645;
   rfrumi_item_list_b7[6].val.lte_b7_exp_hdet_vs_agc[11] = (uint16)1891;
   rfrumi_item_list_b7[6].val.lte_b7_exp_hdet_vs_agc[12] = (uint16)2207;
   rfrumi_item_list_b7[6].val.lte_b7_exp_hdet_vs_agc[13] = (uint16)2613;
   rfrumi_item_list_b7[6].val.lte_b7_exp_hdet_vs_agc[14] = (uint16)3066;
   rfrumi_item_list_b7[6].val.lte_b7_exp_hdet_vs_agc[15] = (uint16)3519;

   rfrumi_rfnv_item_list_b7[0].item_id = RFNV_LTE_B7_CA_BC_CONFIG_I;
   rfrumi_rfnv_item_list_b7[0].val[0] = 0x44;
   rfrumi_rfnv_item_list_b7[0].val[1] = 0x00;
   rfrumi_rfnv_item_list_b7[0].val[2] = 0x01;

   /*End LTE*/
}

void rfrumi_set_item_list_b41(void)
{

   /*Rx Channel*/
   rfrumi_rfnv_item_list_b41[0].item_id = RFNV_LTE_B41_RX_CAL_CHAN_I;
   rfrumi_rfnv_item_list_b41[0].val[0] = 0x14;
   rfrumi_rfnv_item_list_b41[0].val[1] = 0x9B;
   rfrumi_rfnv_item_list_b41[0].val[2] = 0x46;
   rfrumi_rfnv_item_list_b41[0].val[3] = 0x9B;
   rfrumi_rfnv_item_list_b41[0].val[4] = 0x78;
   rfrumi_rfnv_item_list_b41[0].val[5] = 0x9B;
   rfrumi_rfnv_item_list_b41[0].val[6] = 0xAA;
   rfrumi_rfnv_item_list_b41[0].val[7] = 0x9B;
   rfrumi_rfnv_item_list_b41[0].val[8] = 0xDC;
   rfrumi_rfnv_item_list_b41[0].val[9] = 0x9B;
   rfrumi_rfnv_item_list_b41[0].val[10] = 0x72;
   rfrumi_rfnv_item_list_b41[0].val[11] = 0x9C;
   rfrumi_rfnv_item_list_b41[0].val[12] = 0x08;
   rfrumi_rfnv_item_list_b41[0].val[13] = 0x9D;
   rfrumi_rfnv_item_list_b41[0].val[14] = 0x9E;
   rfrumi_rfnv_item_list_b41[0].val[15] = 0x9D;
   rfrumi_rfnv_item_list_b41[0].val[16] = 0x34;
   rfrumi_rfnv_item_list_b41[0].val[17] = 0x9E;
   rfrumi_rfnv_item_list_b41[0].val[18] = 0xCA;
   rfrumi_rfnv_item_list_b41[0].val[19] = 0x9E;
   rfrumi_rfnv_item_list_b41[0].val[20] = 0x60;
   rfrumi_rfnv_item_list_b41[0].val[21] = 0x9F;
   rfrumi_rfnv_item_list_b41[0].val[22] = 0xF6;
   rfrumi_rfnv_item_list_b41[0].val[23] = 0x9F;
   rfrumi_rfnv_item_list_b41[0].val[24] = 0x8C;
   rfrumi_rfnv_item_list_b41[0].val[25] = 0xA0;
   rfrumi_rfnv_item_list_b41[0].val[26] = 0x22;
   rfrumi_rfnv_item_list_b41[0].val[27] = 0xA1;
   rfrumi_rfnv_item_list_b41[0].val[28] = 0xB8;
   rfrumi_rfnv_item_list_b41[0].val[29] = 0xA1;
   rfrumi_rfnv_item_list_b41[0].val[30] = 0x44;
   rfrumi_rfnv_item_list_b41[0].val[31] = 0xA2;

   /*LTE B41: PA range map*/
   rfrumi_rfnv_item_list_b41[1].item_id = RFNV_LTE_B41_PA_RANGE_MAP_I;
   rfrumi_rfnv_item_list_b41[1].val[0] = 0x2;
   rfrumi_rfnv_item_list_b41[1].val[1] = 0x2;
   rfrumi_rfnv_item_list_b41[1].val[2] = 0x3;
   rfrumi_rfnv_item_list_b41[1].val[3] = 0x3;

   /*LTE B41: LNA range for both C0 and C1*/
   rfrumi_rfnv_item_list_b41[2].item_id = RFNV_LTE_B41_LNA_RANGE_RISE_FALL_I;
   rfrumi_rfnv_item_list_b41[2].val[0] = 0x80;
   rfrumi_rfnv_item_list_b41[2].val[1] = 0xFD;
   rfrumi_rfnv_item_list_b41[2].val[2] = 0xA8;
   rfrumi_rfnv_item_list_b41[2].val[3] = 0xFD;

   rfrumi_rfnv_item_list_b41[2].val[4] = 0xB2;
   rfrumi_rfnv_item_list_b41[2].val[5] = 0xFD;
   rfrumi_rfnv_item_list_b41[2].val[6] = 0xDA;
   rfrumi_rfnv_item_list_b41[2].val[7] = 0xFD;

   rfrumi_rfnv_item_list_b41[2].val[8] = 0x20;
   rfrumi_rfnv_item_list_b41[2].val[9] = 0xFE;
   rfrumi_rfnv_item_list_b41[2].val[10] = 0x48;
   rfrumi_rfnv_item_list_b41[2].val[11] = 0xFE;

   rfrumi_rfnv_item_list_b41[2].val[12] = 0x8E;
   rfrumi_rfnv_item_list_b41[2].val[13] = 0xFE;
   rfrumi_rfnv_item_list_b41[2].val[14] = 0xB6;
   rfrumi_rfnv_item_list_b41[2].val[15] = 0xFE;

   rfrumi_rfnv_item_list_b41[2].val[16] = 0xFC;
   rfrumi_rfnv_item_list_b41[2].val[17] = 0xFE;
   rfrumi_rfnv_item_list_b41[2].val[18] = 0x24;
   rfrumi_rfnv_item_list_b41[2].val[19] = 0xFF;

   rfrumi_rfnv_item_list_b41[2].val[20] = 0x0;
   rfrumi_rfnv_item_list_b41[2].val[21] = 0x0;

   rfrumi_rfnv_item_list_b41[2].val[22] = 0x0;
   rfrumi_rfnv_item_list_b41[2].val[23] = 0x0;

   /*LTE 41: Max Tx Power*/
   rfrumi_rfnv_item_list_b41[3].item_id = RFNV_LTE_B41_MAX_TX_POWER_I;
   rfrumi_rfnv_item_list_b41[3].val[0] = (int8)23;

   /*LTE 41: Hysterisis*/
   rfrumi_rfnv_item_list_b41[4].item_id = RFNV_LTE_B41_TIMER_HYSTERISIS_I;
   rfrumi_rfnv_item_list_b41[4].val[0] = 0x04;
   rfrumi_rfnv_item_list_b41[4].val[1] = 0x00;

   /*LTE 41: PA rise fall threshold*/
   rfrumi_rfnv_item_list_b41[5].item_id = RFNV_LTE_B41_PA_RISE_FALL_THRESHOLD_I;
   rfrumi_rfnv_item_list_b41[5].val[0] = 0xFD;
   rfrumi_rfnv_item_list_b41[5].val[1] = 0x2;
   rfrumi_rfnv_item_list_b41[5].val[2] = 0xDF;
   rfrumi_rfnv_item_list_b41[5].val[3] = 0x2;
   rfrumi_rfnv_item_list_b41[5].val[4] = 0xFD;
   rfrumi_rfnv_item_list_b41[5].val[5] = 0x2;
   rfrumi_rfnv_item_list_b41[5].val[6] = 0xDF;
   rfrumi_rfnv_item_list_b41[5].val[7] = 0x2;
   rfrumi_rfnv_item_list_b41[5].val[8] = 0xFD;
   rfrumi_rfnv_item_list_b41[5].val[9] = 0x2;
   rfrumi_rfnv_item_list_b41[5].val[10] = 0xDF;
   rfrumi_rfnv_item_list_b41[5].val[11] = 0x2;

   /*LTE 41: HDET vs AGC: Values picked from old reference QCN*/
   rfrumi_rfnv_item_list_b41[6].item_id = RFNV_LTE_B41_EXP_HDET_VS_AGC_I;
   rfrumi_rfnv_item_list_b41[6].val[0] = 0xA3;
   rfrumi_rfnv_item_list_b41[6].val[1] = 0x3;
   rfrumi_rfnv_item_list_b41[6].val[2] = 0xAE;
   rfrumi_rfnv_item_list_b41[6].val[3] = 0x3;
   rfrumi_rfnv_item_list_b41[6].val[4] = 0xBE;
   rfrumi_rfnv_item_list_b41[6].val[5] = 0x3;
   rfrumi_rfnv_item_list_b41[6].val[6] = 0xD3;
   rfrumi_rfnv_item_list_b41[6].val[7] = 0x3;
   rfrumi_rfnv_item_list_b41[6].val[8] = 0xEF;
   rfrumi_rfnv_item_list_b41[6].val[9] = 0x3;
   rfrumi_rfnv_item_list_b41[6].val[10] = 0x1B;
   rfrumi_rfnv_item_list_b41[6].val[11] = 0x4;
   rfrumi_rfnv_item_list_b41[6].val[12] = 0x57;
   rfrumi_rfnv_item_list_b41[6].val[13] = 0x4;
   rfrumi_rfnv_item_list_b41[6].val[14] = 0xAB;
   rfrumi_rfnv_item_list_b41[6].val[15] = 0x4;
   rfrumi_rfnv_item_list_b41[6].val[16] = 0x1A;
   rfrumi_rfnv_item_list_b41[6].val[17] = 0x5;
   rfrumi_rfnv_item_list_b41[6].val[18] = 0xAC;
   rfrumi_rfnv_item_list_b41[6].val[19] = 0x5;
   rfrumi_rfnv_item_list_b41[6].val[20] = 0x6D;
   rfrumi_rfnv_item_list_b41[6].val[21] = 0x6;
   rfrumi_rfnv_item_list_b41[6].val[22] = 0x63;
   rfrumi_rfnv_item_list_b41[6].val[23] = 0x7;
   rfrumi_rfnv_item_list_b41[6].val[24] = 0x9F;
   rfrumi_rfnv_item_list_b41[6].val[25] = 0x8;
   rfrumi_rfnv_item_list_b41[6].val[26] = 0x35;
   rfrumi_rfnv_item_list_b41[6].val[27] = 0xA;
   rfrumi_rfnv_item_list_b41[6].val[28] = 0xFA;
   rfrumi_rfnv_item_list_b41[6].val[29] = 0xB;
   rfrumi_rfnv_item_list_b41[6].val[30] = 0xBF;
   rfrumi_rfnv_item_list_b41[6].val[31] = 0xD;

}

void rfrumi_set_item_list_b3(void)
{
   /*LTE B3: Rx Channel*/
   rfrumi_rfnv_item_list_b3[0].item_id = RFNV_LTE_B3_RX_CAL_CHAN_I;
   rfrumi_rfnv_item_list_b3[0].val[0]  = 0x07;
   rfrumi_rfnv_item_list_b3[0].val[1]  = 0x4B;
   rfrumi_rfnv_item_list_b3[0].val[2]  = 0x41;
   rfrumi_rfnv_item_list_b3[0].val[3]  = 0x4B;
   rfrumi_rfnv_item_list_b3[0].val[4]  = 0x7A;
   rfrumi_rfnv_item_list_b3[0].val[5]  = 0x4B;
   rfrumi_rfnv_item_list_b3[0].val[6]  = 0xAF;
   rfrumi_rfnv_item_list_b3[0].val[7]  = 0x4B;
   rfrumi_rfnv_item_list_b3[0].val[8]  = 0xE1;
   rfrumi_rfnv_item_list_b3[0].val[9]  = 0x4B;
   rfrumi_rfnv_item_list_b3[0].val[10] = 0x13;
   rfrumi_rfnv_item_list_b3[0].val[11] = 0x4C;
   rfrumi_rfnv_item_list_b3[0].val[12] = 0x45;
   rfrumi_rfnv_item_list_b3[0].val[13] = 0x4C;
   rfrumi_rfnv_item_list_b3[0].val[14] = 0x77;
   rfrumi_rfnv_item_list_b3[0].val[15] = 0x4C;
   rfrumi_rfnv_item_list_b3[0].val[16] = 0xA5;
   rfrumi_rfnv_item_list_b3[0].val[17] = 0x4C;
   rfrumi_rfnv_item_list_b3[0].val[18] = 0xD3;
   rfrumi_rfnv_item_list_b3[0].val[19] = 0x4C;
   rfrumi_rfnv_item_list_b3[0].val[20] = 0x01;
   rfrumi_rfnv_item_list_b3[0].val[21] = 0x4D;
   rfrumi_rfnv_item_list_b3[0].val[22] = 0x2F;
   rfrumi_rfnv_item_list_b3[0].val[23] = 0x4D;
   rfrumi_rfnv_item_list_b3[0].val[24] = 0x5D;
   rfrumi_rfnv_item_list_b3[0].val[25] = 0x4D;
   rfrumi_rfnv_item_list_b3[0].val[26] = 0x8B;
   rfrumi_rfnv_item_list_b3[0].val[27] = 0x4D;
   rfrumi_rfnv_item_list_b3[0].val[28] = 0xB9;
   rfrumi_rfnv_item_list_b3[0].val[29] = 0x4D;
   rfrumi_rfnv_item_list_b3[0].val[30] = 0xE7;
   rfrumi_rfnv_item_list_b3[0].val[31] = 0x4D;

   /*LTE B3: PA range map*/
   rfrumi_rfnv_item_list_b3[1].item_id = RFNV_LTE_B3_PA_RANGE_MAP_I;
   rfrumi_rfnv_item_list_b3[1].val[0] = 0x2;
   rfrumi_rfnv_item_list_b3[1].val[1] = 0x2;
   rfrumi_rfnv_item_list_b3[1].val[2] = 0x3;
   rfrumi_rfnv_item_list_b3[1].val[3] = 0x3;

   /*LTE B3: LNA range for both C0 and C1*/
   rfrumi_rfnv_item_list_b3[2].item_id = RFNV_LTE_B3_LNA_RANGE_RISE_FALL_I;
   rfrumi_rfnv_item_list_b3[2].val[0] = 0x80;
   rfrumi_rfnv_item_list_b3[2].val[1] = 0xFD;
   rfrumi_rfnv_item_list_b3[2].val[2] = 0xA8;
   rfrumi_rfnv_item_list_b3[2].val[3] = 0xFD;

   rfrumi_rfnv_item_list_b3[2].val[4] = 0xB2;
   rfrumi_rfnv_item_list_b3[2].val[5] = 0xFD;
   rfrumi_rfnv_item_list_b3[2].val[6] = 0xDA;
   rfrumi_rfnv_item_list_b3[2].val[7] = 0xFD;

   rfrumi_rfnv_item_list_b3[2].val[8] = 0x20;
   rfrumi_rfnv_item_list_b3[2].val[9] = 0xFE;
   rfrumi_rfnv_item_list_b3[2].val[10] = 0x48;
   rfrumi_rfnv_item_list_b3[2].val[11] = 0xFE;

   rfrumi_rfnv_item_list_b3[2].val[12] = 0x8E;
   rfrumi_rfnv_item_list_b3[2].val[13] = 0xFE;
   rfrumi_rfnv_item_list_b3[2].val[14] = 0xB6;
   rfrumi_rfnv_item_list_b3[2].val[15] = 0xFE;

   rfrumi_rfnv_item_list_b3[2].val[16] = 0xFC;
   rfrumi_rfnv_item_list_b3[2].val[17] = 0xFE;
   rfrumi_rfnv_item_list_b3[2].val[18] = 0x24;
   rfrumi_rfnv_item_list_b3[2].val[19] = 0xFF;

   rfrumi_rfnv_item_list_b3[2].val[20] = 0x0;
   rfrumi_rfnv_item_list_b3[2].val[21] = 0x0;

   rfrumi_rfnv_item_list_b3[2].val[22] = 0x0;
   rfrumi_rfnv_item_list_b3[2].val[23] = 0x0;

   /*LTE B3: Max Tx Power*/
   rfrumi_rfnv_item_list_b3[3].item_id = RFNV_LTE_B3_MAX_TX_POWER_I;
   rfrumi_rfnv_item_list_b3[3].val[0] = (int8)23;

   /*LTE B3: Hysterisis*/
   rfrumi_rfnv_item_list_b3[4].item_id = RFNV_LTE_B3_TIMER_HYSTERISIS_I;
   rfrumi_rfnv_item_list_b3[4].val[0] = 0x04;
   rfrumi_rfnv_item_list_b3[4].val[1] = 0x00;

   /*LTE B3: PA rise fall threshold*/
   rfrumi_rfnv_item_list_b3[5].item_id = RFNV_LTE_B3_PA_RISE_FALL_THRESHOLD_I;
   rfrumi_rfnv_item_list_b3[5].val[0] = 0xFD;
   rfrumi_rfnv_item_list_b3[5].val[1] = 0x2;
   rfrumi_rfnv_item_list_b3[5].val[2] = 0xDF;
   rfrumi_rfnv_item_list_b3[5].val[3] = 0x2;
   rfrumi_rfnv_item_list_b3[5].val[4] = 0xFD;
   rfrumi_rfnv_item_list_b3[5].val[5] = 0x2;
   rfrumi_rfnv_item_list_b3[5].val[6] = 0xDF;
   rfrumi_rfnv_item_list_b3[5].val[7] = 0x2;
   rfrumi_rfnv_item_list_b3[5].val[8] = 0xFD;
   rfrumi_rfnv_item_list_b3[5].val[9] = 0x2;
   rfrumi_rfnv_item_list_b3[5].val[10] = 0xDF;
   rfrumi_rfnv_item_list_b3[5].val[11] = 0x2;

   /*LTE B3: HDET vs AGC: Values picked from old reference QCN*/
   rfrumi_rfnv_item_list_b3[6].item_id = RFNV_LTE_B3_EXP_HDET_VS_AGC_I;
   rfrumi_rfnv_item_list_b3[6].val[0] = 0xA3;
   rfrumi_rfnv_item_list_b3[6].val[1] = 0x3;
   rfrumi_rfnv_item_list_b3[6].val[2] = 0xAE;
   rfrumi_rfnv_item_list_b3[6].val[3] = 0x3;
   rfrumi_rfnv_item_list_b3[6].val[4] = 0xBE;
   rfrumi_rfnv_item_list_b3[6].val[5] = 0x3;
   rfrumi_rfnv_item_list_b3[6].val[6] = 0xD3;
   rfrumi_rfnv_item_list_b3[6].val[7] = 0x3;
   rfrumi_rfnv_item_list_b3[6].val[8] = 0xEF;
   rfrumi_rfnv_item_list_b3[6].val[9] = 0x3;
   rfrumi_rfnv_item_list_b3[6].val[10] = 0x1B;
   rfrumi_rfnv_item_list_b3[6].val[11] = 0x4;
   rfrumi_rfnv_item_list_b3[6].val[12] = 0x57;
   rfrumi_rfnv_item_list_b3[6].val[13] = 0x4;
   rfrumi_rfnv_item_list_b3[6].val[14] = 0xAB;
   rfrumi_rfnv_item_list_b3[6].val[15] = 0x4;
   rfrumi_rfnv_item_list_b3[6].val[16] = 0x1A;
   rfrumi_rfnv_item_list_b3[6].val[17] = 0x5;
   rfrumi_rfnv_item_list_b3[6].val[18] = 0xAC;
   rfrumi_rfnv_item_list_b3[6].val[19] = 0x5;
   rfrumi_rfnv_item_list_b3[6].val[20] = 0x6D;
   rfrumi_rfnv_item_list_b3[6].val[21] = 0x6;
   rfrumi_rfnv_item_list_b3[6].val[22] = 0x63;
   rfrumi_rfnv_item_list_b3[6].val[23] = 0x7;
   rfrumi_rfnv_item_list_b3[6].val[24] = 0x9F;
   rfrumi_rfnv_item_list_b3[6].val[25] = 0x8;
   rfrumi_rfnv_item_list_b3[6].val[26] = 0x35;
   rfrumi_rfnv_item_list_b3[6].val[27] = 0xA;
   rfrumi_rfnv_item_list_b3[6].val[28] = 0xFA;
   rfrumi_rfnv_item_list_b3[6].val[29] = 0xB;
   rfrumi_rfnv_item_list_b3[6].val[30] = 0xBF;
   rfrumi_rfnv_item_list_b3[6].val[31] = 0xD;

   rfrumi_rfnv_item_list_b3[0].item_id = RFNV_LTE_B3_CA_BC_CONFIG_I;
   rfrumi_rfnv_item_list_b3[0].val[0] = 0x44;
   rfrumi_rfnv_item_list_b3[0].val[1] = 0x00;
   rfrumi_rfnv_item_list_b3[0].val[2] = 0x01;
    

}
void rfrumi_set_item_list_b4(void)
{

   /*LTE B4: Rx Channel*/
   rfrumi_rfnv_item_list_b4[0].item_id = RFNV_LTE_B4_RX_CAL_CHAN_I;
   rfrumi_rfnv_item_list_b4[0].val[0]  = 0xEE;
   rfrumi_rfnv_item_list_b4[0].val[1]  = 0x4D;
   rfrumi_rfnv_item_list_b4[0].val[2]  = 0x0C;
   rfrumi_rfnv_item_list_b4[0].val[3]  = 0x4E;
   rfrumi_rfnv_item_list_b4[0].val[4]  = 0x2A;
   rfrumi_rfnv_item_list_b4[0].val[5]  = 0x4E;
   rfrumi_rfnv_item_list_b4[0].val[6]  = 0x48;
   rfrumi_rfnv_item_list_b4[0].val[7]  = 0x4E;
   rfrumi_rfnv_item_list_b4[0].val[8]  = 0x66;
   rfrumi_rfnv_item_list_b4[0].val[9]  = 0x4E;
   rfrumi_rfnv_item_list_b4[0].val[10] = 0x84;
   rfrumi_rfnv_item_list_b4[0].val[11] = 0x4E;
   rfrumi_rfnv_item_list_b4[0].val[12] = 0xA2;
   rfrumi_rfnv_item_list_b4[0].val[13] = 0x4E;
   rfrumi_rfnv_item_list_b4[0].val[14] = 0xC0;
   rfrumi_rfnv_item_list_b4[0].val[15] = 0x4E;
   rfrumi_rfnv_item_list_b4[0].val[16] = 0xDE;
   rfrumi_rfnv_item_list_b4[0].val[17] = 0x4E;
   rfrumi_rfnv_item_list_b4[0].val[18] = 0xFC;
   rfrumi_rfnv_item_list_b4[0].val[19] = 0x4E;
   rfrumi_rfnv_item_list_b4[0].val[20] = 0x1A;
   rfrumi_rfnv_item_list_b4[0].val[21] = 0x4F;
   rfrumi_rfnv_item_list_b4[0].val[22] = 0x38;
   rfrumi_rfnv_item_list_b4[0].val[23] = 0x4F;
   rfrumi_rfnv_item_list_b4[0].val[24] = 0x56;
   rfrumi_rfnv_item_list_b4[0].val[25] = 0x4F;
   rfrumi_rfnv_item_list_b4[0].val[26] = 0x74;
   rfrumi_rfnv_item_list_b4[0].val[27] = 0x4F;
   rfrumi_rfnv_item_list_b4[0].val[28] = 0x92;
   rfrumi_rfnv_item_list_b4[0].val[29] = 0x4F;
   rfrumi_rfnv_item_list_b4[0].val[30] = 0xAF;
   rfrumi_rfnv_item_list_b4[0].val[31] = 0x4F;

   /*LTE B4: PA range map*/
   rfrumi_rfnv_item_list_b4[1].item_id = RFNV_LTE_B4_PA_RANGE_MAP_I;
   rfrumi_rfnv_item_list_b4[1].val[0] = 0x2;
   rfrumi_rfnv_item_list_b4[1].val[1] = 0x2;
   rfrumi_rfnv_item_list_b4[1].val[2] = 0x3;
   rfrumi_rfnv_item_list_b4[1].val[3] = 0x3;

   /*LTE B4: LNA range for both C0 and C1*/
   rfrumi_rfnv_item_list_b4[2].item_id = RFNV_LTE_B4_LNA_RANGE_RISE_FALL_I;
   rfrumi_rfnv_item_list_b4[2].val[0] = 0x80;
   rfrumi_rfnv_item_list_b4[2].val[1] = 0xFD;
   rfrumi_rfnv_item_list_b4[2].val[2] = 0xA8;
   rfrumi_rfnv_item_list_b4[2].val[3] = 0xFD;

   rfrumi_rfnv_item_list_b4[2].val[4] = 0xB2;
   rfrumi_rfnv_item_list_b4[2].val[5] = 0xFD;
   rfrumi_rfnv_item_list_b4[2].val[6] = 0xDA;
   rfrumi_rfnv_item_list_b4[2].val[7] = 0xFD;

   rfrumi_rfnv_item_list_b4[2].val[8] = 0x20;
   rfrumi_rfnv_item_list_b4[2].val[9] = 0xFE;
   rfrumi_rfnv_item_list_b4[2].val[10] = 0x48;
   rfrumi_rfnv_item_list_b4[2].val[11] = 0xFE;

   rfrumi_rfnv_item_list_b4[2].val[12] = 0x8E;
   rfrumi_rfnv_item_list_b4[2].val[13] = 0xFE;
   rfrumi_rfnv_item_list_b4[2].val[14] = 0xB6;
   rfrumi_rfnv_item_list_b4[2].val[15] = 0xFE;

   rfrumi_rfnv_item_list_b4[2].val[16] = 0xFC;
   rfrumi_rfnv_item_list_b4[2].val[17] = 0xFE;
   rfrumi_rfnv_item_list_b4[2].val[18] = 0x24;
   rfrumi_rfnv_item_list_b4[2].val[19] = 0xFF;

   rfrumi_rfnv_item_list_b4[2].val[20] = 0x0;
   rfrumi_rfnv_item_list_b4[2].val[21] = 0x0;

   rfrumi_rfnv_item_list_b4[2].val[22] = 0x0;
   rfrumi_rfnv_item_list_b4[2].val[23] = 0x0;

   /*LTE 4: Max Tx Power*/
   rfrumi_rfnv_item_list_b4[3].item_id = RFNV_LTE_B4_MAX_TX_POWER_I;
   rfrumi_rfnv_item_list_b4[3].val[0] = (int8)23;

   /*LTE 4: Hysterisis*/
   rfrumi_rfnv_item_list_b4[4].item_id = RFNV_LTE_B4_TIMER_HYSTERISIS_I;
   rfrumi_rfnv_item_list_b4[4].val[0] = 0x04;
   rfrumi_rfnv_item_list_b4[4].val[1] = 0x00;

   /*LTE 4: PA rise fall threshold*/
   rfrumi_rfnv_item_list_b4[5].item_id = RFNV_LTE_B4_PA_RISE_FALL_THRESHOLD_I;
   rfrumi_rfnv_item_list_b4[5].val[0] = 0xFD;
   rfrumi_rfnv_item_list_b4[5].val[1] = 0x2;
   rfrumi_rfnv_item_list_b4[5].val[2] = 0xDF;
   rfrumi_rfnv_item_list_b4[5].val[3] = 0x2;
   rfrumi_rfnv_item_list_b4[5].val[4] = 0xFD;
   rfrumi_rfnv_item_list_b4[5].val[5] = 0x2;
   rfrumi_rfnv_item_list_b4[5].val[6] = 0xDF;
   rfrumi_rfnv_item_list_b4[5].val[7] = 0x2;
   rfrumi_rfnv_item_list_b4[5].val[8] = 0xFD;
   rfrumi_rfnv_item_list_b4[5].val[9] = 0x2;
   rfrumi_rfnv_item_list_b4[5].val[10] = 0xDF;
   rfrumi_rfnv_item_list_b4[5].val[11] = 0x2;

   /*LTE 4: HDET vs AGC: Values picked from old reference QCN*/
   rfrumi_rfnv_item_list_b4[6].item_id = RFNV_LTE_B4_EXP_HDET_VS_AGC_I;
   rfrumi_rfnv_item_list_b4[6].val[0] = 0xA3;
   rfrumi_rfnv_item_list_b4[6].val[1] = 0x3;
   rfrumi_rfnv_item_list_b4[6].val[2] = 0xAE;
   rfrumi_rfnv_item_list_b4[6].val[3] = 0x3;
   rfrumi_rfnv_item_list_b4[6].val[4] = 0xBE;
   rfrumi_rfnv_item_list_b4[6].val[5] = 0x3;
   rfrumi_rfnv_item_list_b4[6].val[6] = 0xD3;
   rfrumi_rfnv_item_list_b4[6].val[7] = 0x3;
   rfrumi_rfnv_item_list_b4[6].val[8] = 0xEF;
   rfrumi_rfnv_item_list_b4[6].val[9] = 0x3;
   rfrumi_rfnv_item_list_b4[6].val[10] = 0x1B;
   rfrumi_rfnv_item_list_b4[6].val[11] = 0x4;
   rfrumi_rfnv_item_list_b4[6].val[12] = 0x57;
   rfrumi_rfnv_item_list_b4[6].val[13] = 0x4;
   rfrumi_rfnv_item_list_b4[6].val[14] = 0xAB;
   rfrumi_rfnv_item_list_b4[6].val[15] = 0x4;
   rfrumi_rfnv_item_list_b4[6].val[16] = 0x1A;
   rfrumi_rfnv_item_list_b4[6].val[17] = 0x5;
   rfrumi_rfnv_item_list_b4[6].val[18] = 0xAC;
   rfrumi_rfnv_item_list_b4[6].val[19] = 0x5;
   rfrumi_rfnv_item_list_b4[6].val[20] = 0x6D;
   rfrumi_rfnv_item_list_b4[6].val[21] = 0x6;
   rfrumi_rfnv_item_list_b4[6].val[22] = 0x63;
   rfrumi_rfnv_item_list_b4[6].val[23] = 0x7;
   rfrumi_rfnv_item_list_b4[6].val[24] = 0x9F;
   rfrumi_rfnv_item_list_b4[6].val[25] = 0x8;
   rfrumi_rfnv_item_list_b4[6].val[26] = 0x35;
   rfrumi_rfnv_item_list_b4[6].val[27] = 0xA;
   rfrumi_rfnv_item_list_b4[6].val[28] = 0xFA;
   rfrumi_rfnv_item_list_b4[6].val[29] = 0xB;
   rfrumi_rfnv_item_list_b4[6].val[30] = 0xBF;
   rfrumi_rfnv_item_list_b4[6].val[31] = 0xD;
}

#endif