/*!
   @file rfgsm_core_tx_power.c

   @brief

   @details

*/



/*===========================================================================
Copyright (c) 2011 - 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/rf/core/src/rfgsm_core_tx_power.c#1 $
$DateTime: 2016/12/13 07:59:51 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/08/16   bp      Put the QPOET to standby mode after GSM Tx burst ends
12/07/15   dm      Changing p1, p1 to int16 from uint16 to solve the issue with GSM Temp comp function
10/28/15   par     Use static power_pcl LUT for conversion in FTM
10/27/15   par     Implemented rfgsm_core_tx_get_closest_pcl_index_vs_power
                   with core pcl_vs_power tables in the FTM_STATE_GSM mode.
                   then also unified application of tx_power_error for signalling mode.
10/13/15   bp      make use of true ftm_gsm_get_frame_sweep_cal_running() 
                   to nullify temptr/volt compensation out of TX AGC
04/02/15   par     Use max pcl if multislot APT feature is not enabled
02/05/14   par     Few F3 cleanup
01/16/14   ec      Use new debug NV mechanism for logging
11/28/14   mw      Make RFGC_MSG messages in function rfgsm_core_compute_pcl_vs_power() more meaningful
11/17/14   sc      Read and calculate delay match DM1-3 in RFLM GSM TxFE config
10/23/14   sc      Enable event type control from RFSW
10/10/14   ck      Apply enh temp comp interpolation at min and max pcl index
10/07/14   sc      Program all delay match settings in RFLM DRIF
09/30/14   tws     Cap RGI applied to max calibrated RGI value
09/24/14   hoh     Add multislot APT control feature
09/23/14   zbz     changes to support GSM and LTE/TDS coexistence related RFC changes
09/09/14   ch      Mainline FEATURE_RF_HAS_QTUNER 
08/14/14   zbz     Add SMPS/ICQ/temp_rem to FTM log packet
05/07/14   ch      Store tx power in dm for CL tuner
06/05/14   ggs     AMAM/AMPM tables should only be recalculated 
                   for GSM when modulation type is 8PSK
04/28/14   sc      Scale the Tx PA ramps based on subscription
04/24/14   ggs     Port CR581892: Added code to re-run power control if backoff 
                   parameters change
04/22/14   ggs     Port CR636669: Check if frame sweep calibration is running 
                   prior to overriding RGI LUT to 8PSK.
03/19/14   hoh     Added support for 8 PA gain states; removed unused vbatt code
03/10/14   zbz     Add feature to control PA (ICQ) BIAS through NV item
03/05/14   tsr     Mainline v2 device scripts for dynamic CCS programming 
03/05/14   zbz     Fix KW errors
02/28/14   sc      Pass core data through mc to core layers
02/25/14   ggs     Port CR5082237: rfgsm_gsm_multislot_tx_pwr_limit 
                   and rfgsm_edge_multislot_tx_pwr_limit 
                   need to be calculated based on multislot_max_tx_power value 
                   present in the NV and PCL vs power adjustment
                   (if non-zero as programmed by GSM cal v4)
02/17/14   sc      Clean up mDSP APIs
01/30/14   sc      Manage data table independent of device
01/21/14   tsr     WTR TXAGC support for Multislot TX
01/21/14   ggs     Initial changes for DPD and DCOC on Bolt
01/15/14   sc      Program envelope gain to Tx DM data regardless of update
01/14/14   av      Delay Calibration and npler based delay support
12/10/13   sc      Add set envelope gain API functionality
11/22/13   sw      Fix "Warning:unused-but-set-variable" GNU compiler warnings
10/21/13   sc      Set the PA ramp tables to RFLM GSM DM
10/17/13   ggs     Add support to override amam and ampm tables with default
                   values and disable pre-distortion and disable Vbatt/temp
                   comp for BOLT using debug flags
10/14/13   sc      Mainline core DM and RFLM code
10/11/13   ec	   Fix bug in Enhanced Temp Comp where signed NV values are assigned to 
				   unsigned integers
10/03/13   sb      Add changes to receive MSGR cmd from MCS to switch to filtered/unfiltered 
                   path for SGLTE coexistence (GSM DCS +LTE B39/TDS B39/B34)
10/02/13   tws     QPOET GSM Optimisations. 
08/06/13   ec	   Modify power control algorithm to fix potential issue with L1
				   backoff parameters and SAR limiting.
08/05/13   sc      Scale PA ramp table and switch ping-pong id
07/22/13   ec      Use new F3s/Null pointer checks
07/18/13   svi     Adding [QTUNER][RL] GSM NV Support.
07/08/13   svi     Closed Loop Mode Fix.
07/08/13   svi     Adding Support of Closed Loop Mode for Antenna Tuner.
07/03/13   ec      Adjust Tx Power algorithm to account for coex backoff passed from
                   Layer 1
06/19/13   svi      Fix vbatt comp issues.
06/17/13   svi     "TX_GTR_THRES" NV values interpretation fix.
06/05/13   svi     Adding Support of Enhanced Temperature Compensation in GSM.
04/22/13   sb      Add FTM hooks to override QPOET bias
04/17/13   aka     Changes to support predist_override
04/18/13   sb      Add FTM hooks for overriding envelope gain, PA scaling and SMPS bias
04/10/13   tc      Add QPOET/QTUNER timing override facility
04/09/13   sc      Remove unused SMPS PDM API call
04/09/13   tsr     Added RF APPS->RF Task MSGR framework for temp comp to make temp comp data thread safe
04/02/13   aka     Support for band change in frame sweep functionality. 
03/28/13   aka     GSM frame sweep feature changes.
03/27/13   sb      Added support for Enhanced APT
03/08/13   lcl     Fixed proper RGI logging
03/05/13   sb      Correctly map the cal channel frequencies before interpolation
02/07/13   tsr     Ensure that Temp comp calculation is thread safe
02/05/13   sb      Made changes to fix path delay not getting applied in FTM
01/2/13    aka     Changes to support multi chain GSM operation in FTM mode
01/15/13   tc      Port changes from Nikel to address CR403861 (envelope 
                   backoff values extended to achieve min Tx power levels)
01/09/13   sr      Made changes to run vbatt/temp comp in rf app task context.
01/08/13   ggs     RF_ON needs to be added to a new dedicated transition event 
12/19/12   sb      Optimize GSM APT
12/11/12   sb      Add hooks for GSM APT 
12/10/12   tws     Move global variables to the gsm core handler.
12/06/12   tws     Fix Klocwork errors.
12/05/12   aka     Removed some hardcoding for device id
12/03/12   aka     Changes to use NV ptr from rfgsm_core_handle_ptr
11/28/12   aka     Changes to use rfgsm_core_handle_ptr from path based handler
11/26/12   aka     Changes to use a path specific gsm handle
11/21/12   av      Added rfm_device parameters to mc/core/mdsp/msm layers.
11/16/12   sb      Made changes to convert RF_ON to SSBI 
10/01/12   sb      Made changes to add rfm device to rf device interface and uppler layer APIs 
09/25/12   tc      Mainline RF_HAS_RFA_GSM_TASK. 
09/18/12   tc      Remove FEATURE_RF_HAS_QFE1320-related functionality.  
09/04/12   sr      Clean-up of pa_range over-ride API.
08/28/12   sb/tsr  Temporary fix to change RGI value in online mode
08/24/12   sb/tsr  Made changes to populate envelope gain/PA scale in the tx transition buffer
08/16/12   ec      Cleaning up FTM PA range override implementation
08/10/12   sr      Made changes to add the rf_on signal to transition tx script.
08/08/12   sr      Made changes to call the rfc_tx_gain api to populate the 
                   tx transition scripts with tx gain related settings.
08/01/12   ec      Added code to process linear rgi, pa range and path delay settings passed       
                   from FTM 
07/30/12   sc      Added temporary gain parameter struct to enable FW SU release
07/20/12   sr      Made changes to pass the RFM_DEVICE and rf_buffer to rfc_gsm
07/18/12   ec      Added code to update tx power from rfgsm_card when FTM updates it       
                   via MSGR 
06/19/12   sc      Added shared memory interface support for Tx power APIs
07/02/12   sn      PA/ASM device interface and scripting support
05/30/12   jps     KW fix       
05/30/12   jps     KW fix       
05/30/12   jps     Compiler warning fix       
05/30/12   jps     Apply Temp Comp offset based on TX pwr_in_dbm instead of       
                   absolute PCL to account for multi-slot backoff 
05/23/12   lcl     Fixed dBm to PCL conversion issue.
05/08/12   sn      Updated rfgsm_core_get_pcl_from_power for low band case
05/04/12   sb      Added changes to support TXP indicator for GSM
04/18/12   jps/lcl PA Range and PA R0/R1 logic decoupling       
03/07/12   jps/lcl temporary workaround to un-gate bringup              
02/17/12   sn      Added support for SMPS PDM control in FTM mode
02/10/12   sb      Changes to support FTM logging for GSM
01/20/12   sn      Check parange_override_val before updating QFE1320 FW interface
12/20/11   sn      Added support for QFE1320
01/10/12   sr      changes to enable vbatt comp.
11/12/11   sb      Added a function which converts measured power into PCL index for temp comp in FTM mode
10/28/11    sr     made changes use the tx_nv_tbl ptr for any tx NV items
                   and rx_nv_tbl for any rx NV items.
10/27/11    sr     Added SAR state logging for tx F3 msgs.       
10/21/11   dej     Ported Extention of temp comp MAX_DYNAMIC_RANGE limit from MDM9k
10/19/11   sb      Apply therm read in idle frame and enable FTM temp comp and vbatt comp during idle frame
10/13/11    sr     made changes add F3 msgs via DPC task.
10/11/11    sr     optimized code for ISR over-run issue.
10/04/11   vrb     Added PCL Index to the Tx burst info F3 message
09/29/11   vrb     PCL conversion for low bands (pcl - 5) is not required
09/22/11   sb      Making FTM Temp Comp enable RFC dependent
08/09/11   av      Added NV support for Digital Baseband compensation 
08/08/11   av      Support for Digital Baseband Compensation
09/08/11   sr      Added GST(Gain State Tracking) feature
08/07/11   av      Port Enhanced temp comp changes
09/02/11   sr      ported SAR feature from MDM9k.
08/06/11   sr      RFC Cleanup: removed un-used code, moved the msm related 
                   consts to HAL layer
08/02/11   sr      Made changes to use band specific tx_gain nv item.
07/05/2011 vrb     Remove unnecessary typacast to device intf function
07/01/2011 vrb     Replacing direct calls to rfgsm_ds with wrapper functions
06/07/2011 rsr/sr  Disabling the temp/vbatt comp until we have valid NV values.
05/31/2011 rsr/sr  made changes to control pa-Range from QRCT GUI
05/17/2011 rsr/sr  Fix for env gain length in HWTC.  uint8 ->uint16
03/30/11   vrb     Changes for new GFW SW Interface
03/23/11    sr     temp comp code cleanup.
03/08/11   sr      Initial revision. grouped tx pwr related functions to this file.

============================================================================*/

#include "rfa_variation.h"
#include "rfcom.h"
#include "msg.h"
#include "ftm_gsm_rfctl.h"
#include "rfdevice_gsm_intf.h"
#include "rfdevice_type_defs.h"
#include "rfgsm_core_types.h"
#include "rfgsm_nv_cmn.h"
#include "rfgsm_msm.h"
#include "rfgsm_core_tx.h"
#include "mdsp_intf.h"
#include "rfgsm_core_util.h"
#include "rfc_card_gsm.h"
#include "rfgsm_core.h"
#include "rfcommon_core_sar.h"
#include "rfcommon_msg.h"
#include "rfgsm_core_temp_comp.h"
#include "rfgsm_core_vbatt_comp.h"
#include "rfm_internal.h" // For Bring up flags

/* Temporary workaround to un-gate bring up.  Should be remove after fix */
#include "rfcommon_nv_mm.h"

#include "rfgsm_data_manager.h"

#define RFGSM_CORE_TX_LOWBAND_SAR_LIMIT_MIN  500  /* 5 dBm -- dB100*/
#define RFGSM_CORE_TX_HIGHBAND_SAR_LIMIT_MIN 0  /* 0 dBm */

#define RFGSM_CORE_MAX_NON_PREDIST_BB_BOOST -100
#define RFGSM_CORE_MAX_ENV_BACKOFF_dBx100 2490

uint16 rfgsm_debug_rgi_fix = 0;

//PA scale dynamic range 0 - 33, resolution 0.25 Equation
static const uint16 rfgsm_core_gsmctl_pa_scale_int[] = 
{
  32768, 31838, 30934, 30057, 29204, 28375, 27570, 26788, 26028, 25290, 24572, 23875, 23197, 
  22539, 21900, 21278, 20675, 20088, 19518, 18964, 18426, 17903, 17396, 16902, 16422, 15956, 
  15504, 15064, 14636, 14221, 13818, 13426, 13045, 12675, 12315, 11966, 11626, 11296, 10976, 
  10664, 10362, 10068,  9782,  9504,  9235,  8973,  8718,  8471,  8230,  7997,  7770,  7550, 
  7335,  7127,  6925,  6728,  6538,  6352,  6172,  5997,  5827,  5661,  5501,  5345,  5193, 
  5046,  4902,  4763,  4628,  4497,  4369,  4245,  4125,  4008,  3894,  3783,  3676,  3572, 
  3470,  3372,  3276,  3183,  3093,  3005,  2920,  2837,  2757,  2678,  2602,  2529,  2457, 
  2387,  2319,  2253,  2190,  2127,  2067,  2008,  1951,  1896,  1842,  1790,  1739,  1690, 
  1642,  1595,  1550,  1506,  1463,  1422,  1381,  1342,  1304,  1267,  1231,  1196,  1162, 
  1129,  1097,  1066,  1036,  1006,   978,   950,   923,   897,   871,   847,   823,   799, 
  777,   755,   733
};


/*----------------------------------------------------------------------------*/
/*EXTERNAL FUNCTIONS*/
uint16 rfgsm_map_arfcn_linearly(rfcom_gsm_band_type band, uint16 arfcn);
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*INTERNAL FUNCTIONS*/
void rfgsm_core_gsmctl_set_tx_power( rfm_device_enum_type rfm_dev, 
                                     uint8 sub_id,
                                     rfgsm_power_profile_type pwr_profile[5],
                                     uint8 num_tx_slots,
                                     uint8 num_assigned_uplink_slots );

uint16 rfgsm_core_tx_get_closest_pcl_index_vs_power(rfm_device_enum_type rfm_dev,
                                                    int16 tx_pwr);

/*----------------------------------------------------------------------------*/

/*!
  @details
   This function calculates the amam tables for the specificed power
   leves and returns a pointer to the table.

  @param pwr_profile[5] : power level for 5 possible tx bursts.

  @retval
  pointer to the profile to be used.

  @pre
     NV must have been initialized.
*/
void rfgsm_core_set_tx_power ( rfgsm_core_data_type *core_data,
                               rfgsm_power_profile_type pwr_profile[5],
                               uint8 num_tx_slots,
                               uint8 num_assigned_uplink_slots)
{
  int i;
  boolean program_amam_ampm_tables = FALSE;

  for (i = 0 ; i <num_tx_slots; i++)
  {

    if ((pwr_profile[i].modulation == RF_MOD_8PSK) && 
       (core_data->rfgsm_core_handle_ptr->amam_ampm_recalc_required)) 
    {
      rfgsm_core_set_tx_profile_update_flag(core_data->rfgsm_core_handle_ptr);
      rfgsm_core_update_linear_amam_ampm_profiles(core_data);

      /* Reset the amam_ampm recalc required flag */
      core_data->rfgsm_core_handle_ptr->amam_ampm_recalc_required = FALSE;

      /* Clear the temp comp flag since amam temp comp is also performed 
        in update linear amam profiles */
      rfgsm_core_temp_comp_flag_clear(core_data->rfm_dev);
    }

    if ((pwr_profile[i].modulation == RF_MOD_8PSK) && 
       (core_data->rfgsm_core_handle_ptr->temp_comp.temp_comp_updated)) 
    {
      rfgsm_core_set_tx_profile_update_flag(core_data->rfgsm_core_handle_ptr);
      rfgsm_core_update_linear_amam_temp_comp(core_data);
      /* Clear the temp comp flag */
      rfgsm_core_temp_comp_flag_clear(core_data->rfm_dev);
    }

    if (core_data->rfgsm_core_handle_ptr->last_pwr_level[i] != pwr_profile[i].power_level)
    {
      rfgsm_core_set_tx_profile_update_flag(core_data->rfgsm_core_handle_ptr);
      core_data->rfgsm_core_handle_ptr->last_pwr_level[i] = pwr_profile[i].power_level;
    }

    if (core_data->rfgsm_core_handle_ptr->last_mod_type[i] != pwr_profile[i].modulation)
    {
      rfgsm_core_set_tx_profile_update_flag(core_data->rfgsm_core_handle_ptr);
      core_data->rfgsm_core_handle_ptr->last_mod_type[i] = pwr_profile[i].modulation;
    }

    if (core_data->rfgsm_core_handle_ptr->last_desense_backoff[i] != pwr_profile[i].backoff[RFGSM_COEX_DESENSE_BACKOFF] )
    {
       rfgsm_core_set_tx_profile_update_flag(core_data->rfgsm_core_handle_ptr);
       core_data->rfgsm_core_handle_ptr->last_desense_backoff[i] = pwr_profile[i].backoff[RFGSM_COEX_DESENSE_BACKOFF];
    }

    if (core_data->rfgsm_core_handle_ptr->last_vbatt_backoff[i] != pwr_profile[i].backoff[RFGSM_COEX_VBATT_BACKOFF] )
    {
       rfgsm_core_set_tx_profile_update_flag(core_data->rfgsm_core_handle_ptr);
       core_data->rfgsm_core_handle_ptr->last_vbatt_backoff[i] = pwr_profile[i].backoff[RFGSM_COEX_VBATT_BACKOFF];
    }

  }

  if (num_tx_slots != core_data->rfgsm_core_handle_ptr->last_num_tx_slots)
  {
    rfgsm_core_set_tx_profile_update_flag(core_data->rfgsm_core_handle_ptr);
    core_data->rfgsm_core_handle_ptr->last_num_tx_slots = num_tx_slots;
  }

  if ( core_data->rfgsm_core_handle_ptr->tx_profile_update_required ) 
  {
    /* Toggle ping-pong if ramp mod required to ensure not overwriting last table */
    if( !rfgsm_core_tx_pa_ramp_data_buffer_switch( core_data->rfm_dev ) )
    {
      RF_MSG(MSG_LEGACY_ERROR, "RF GSM prog Tx ramp table ping-pong failed!");
      return;
    }

    rfgsm_core_clear_tx_profile_update_flag(core_data->rfgsm_core_handle_ptr);
    rfgsm_core_gsmctl_set_tx_power(core_data->rfm_dev, core_data->sub_id, &pwr_profile[0], num_tx_slots, num_assigned_uplink_slots);
  }

  /* Set the ramp table pointers in RFLM GSM DM */
  if( !rfgsm_core_dm_set_ramp_tables( core_data->sub_id, 
                                      core_data->triple_buffer_id,
                                      core_data->rfgsm_core_handle_ptr->pa_ramp_buffer_id,
                                      num_tx_slots ) )
  {
    RF_MSG(MSG_LEGACY_ERROR, "RF GSM set Tx ramp table programming failed!");
    return;
  }

  /* Set the amam and ampm table pointers in RFLM GSM DM */
  if( !rfgsm_core_dm_set_amam_ampm_tables( core_data->sub_id,
                                           core_data->triple_buffer_id, 
                                           core_data->rfgsm_core_handle_ptr->amam_ampm_buffer_id ))
  {
    RF_MSG(MSG_LEGACY_ERROR, "RF GSM set AMAM AMPM table programming failed!");
    return;
  }

  for (i=0; i<num_tx_slots; i++)
  {
     /* Set the envelope gain value for this slot in DM Tx data table vars */
     if (!rfgsm_core_dm_set_env_gain_val( core_data->sub_id,
                                          core_data->triple_buffer_id,
                                          i,
                                          core_data->rfgsm_core_handle_ptr->rfc_gsmctl_linear_pa_tx_gain_buff[i].txCfg2GainSetupEnvGain ) )
     {
       RF_MSG_1(MSG_LEGACY_ERROR, "RF GSM prog FTM Tx core DM set envelope gain val failed for slot %d!", i);
       return;
     }

     /* Set the predistortion flag for this slot in DM Tx data table vars */
    if (!rfgsm_core_dm_set_predist_flag( core_data->sub_id,
                                         core_data->triple_buffer_id,
                                         i,
                                         core_data->rfgsm_core_handle_ptr->rfc_gsmctl_linear_pa_tx_gain_buff[i].txCfg2GainSetupPredist ) )
    {
      RF_MSG_1(MSG_LEGACY_ERROR, "RF GSM prog Tx core DM set predist flag failed for slot %d!", i);
      return;
    }

    program_amam_ampm_tables |= core_data->rfgsm_core_handle_ptr->rfc_gsmctl_linear_pa_tx_gain_buff[i].txCfg2GainSetupPredist;
    
    /* Write Tx power value to RFLM DM for Closed Loop tuner*/    
    if( !rfgsm_core_dm_set_tx_power( core_data->sub_id,
                                     core_data->triple_buffer_id,
                                     i,
                                     core_data->rfgsm_core_handle_ptr->rfc_gsmctl_linear_pa_tx_gain_buff[i].txPower) )
    {
      RF_MSG_1(MSG_LEGACY_ERROR, "Tx power is NOT updated for device %d", core_data->rfm_dev);
      return;
    }
  }/* for (i=0; i<num_tx_slots; i++) */

  /* If any of the Tx slots use pre-distortion, set the AMAM AMPM program flag to TRUE in DM Tx data table vars */
  if ( program_amam_ampm_tables == TRUE )
  {
     if(!rfgsm_core_dm_set_program_amam_ampm_tables_flag( core_data->sub_id,
                                                          core_data->triple_buffer_id,
                                                          TRUE ) )
     {
        RF_MSG(MSG_LEGACY_ERROR, "RF GSM prog Tx core set program amam ampm table flag failed!");
        return;
     }
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @details
   This function apply SAR limit passed from MCS

  @param sar_limit_gmsk[5] : SAR limits for GSM for all 5 different multislot configs
               sar_limit_8psk[5]  : SAR limits for EDGE for all 5 different multislot configs

*/

void rfgsm_core_set_simult_sar_limit(rfgsm_core_handle_type *rfgsm_core_handle_ptr, int16 sar_limit_gmsk[5], int16 sar_limit_8psk[5])
{
  uint32 index = 0;

  /* Fill in the power profile structure for the RF driver for each tx slot required */
  for (index = 0; index < RFGSM_MAX_TX_SLOTS_PER_FRAME; index++)
  {
    rfgsm_core_handle_ptr->simult_sar_limit_gmsk[index] = sar_limit_gmsk[index];
    rfgsm_core_handle_ptr->simult_sar_limit_8psk[index] = sar_limit_8psk[index];
  }

  /* Update tx profile due to Tx pwr limit change*/
  rfgsm_core_set_tx_profile_update_flag(rfgsm_core_handle_ptr);
}

/*----------------------------------------------------------------------------*/
/*!
  @details
   This function gets current Tx pwr limit for a given rfm device, modulation and no of uplink slot.
   This limit is the value as requested by MCS for current state of all tx chains.

  @param rfm_dev : RFM device for which tx pwr limit is required
               mod_type : 0 for GMSK, 1 for 8PSK
               uplink slots : No of Uplink slots in a frame
               simult_tx_pwr_limit : Variable to return tx_pwr_limit               
*/

int16 rfgsm_core_get_simult_tx_pwr_limit(rfm_device_enum_type rfm_dev, rfgsm_modulation_type mod_type, uint8 uplink_slots)
{

  rfgsm_core_handle_type *rfgsm_core_handle_ptr;
  int16 simult_tx_pwr_limit = 0;
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
	
  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!", simult_tx_pwr_limit);

  if (mod_type == RF_MOD_GMSK)
  {
    simult_tx_pwr_limit = rfgsm_core_handle_ptr->simult_sar_limit_gmsk[uplink_slots -1];
  }
  else if (mod_type == RF_MOD_8PSK)
  {
    simult_tx_pwr_limit = rfgsm_core_handle_ptr->simult_sar_limit_8psk[uplink_slots -1];
  }
  else
  {
    simult_tx_pwr_limit = 0;
    RFGC_MSG( MSG_LEGACY_ERROR, "rfgsm_core_get_simult_tx_pwr_limit, bad modulation!" );
  }
  
  return simult_tx_pwr_limit;
}



/*----------------------------------------------------------------------------*/
/*!
  @details
   This function sets the transmit power in dBm. 
   It is used in FTM mode only. If this value is -1, the NV PCL vs dBm 
   table is used during transmit.

   @param powdBm : the power to set in dBm
   @param index : the index to which this power is set in  bursts

*/
int8 rfgsm_core_set_tx_pow_dBm (rfgsm_core_handle_type *rfgsm_core_handle_ptr, int16  powdBm, int8 index)
{
  if(rfgsm_core_handle_ptr->rfgsm_core_tx_pow_mod[index].power_level != powdBm)
  {
    rfgsm_core_handle_ptr->rfgsm_core_tx_pow_mod[index].power_level = powdBm;
  }

  return(index);
}



/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details

   @param 

   @retval 
   
*/
void rfgsm_core_set_tx_profile_update_flag(rfgsm_core_handle_type *rfgsm_core_handle_ptr)
{
  rfgsm_core_handle_ptr->tx_profile_update_required = TRUE;
}



/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details

   @param 

   @retval 
   
*/
void rfgsm_core_clear_tx_profile_update_flag(rfgsm_core_handle_type *rfgsm_core_handle_ptr)
{
  rfgsm_core_handle_ptr->tx_profile_update_required = FALSE;
}



/*!
   @brief
  This function sets the tx power setting override flag.

   @details
DEPENDENCIES
  RF in Linear mode.

   @param 

   @retval 
  None
   
*/

void rfgsm_core_set_slot_override_flag( rfgsm_core_handle_type *rfgsm_core_handle_ptr, boolean override_val )
{
  rfgsm_core_handle_ptr->slot_override_flag = override_val;

  if (override_val == FALSE)
  {
    /* This allows us to hit the point to set power
     * when we go from linear gain buffer to tx frame matrix
     */
    rfgsm_core_set_tx_profile_update_flag(rfgsm_core_handle_ptr);
  }
}


/*----------------------------------------------------------------------------*/

void rfgsm_core_set_bias_override_flag( rfm_device_enum_type rfm_dev, uint8 override_flag )
{
  rfgsm_core_handle_type * rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN_V(rfgsm_core_handle_ptr,"rfgsm_core_handle_ptr is NULL!");

  rfgsm_core_handle_ptr->rfgsm_core_ftm_bias_override_flag = override_flag;

}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function computes the freq compensation factor that needs to be
   be applied to the Pmeas versus RGI table

   @param
   band - Current band

   @param
   arfcn - Current channel
*/
void rfgsm_core_compute_linear_freq_comp (rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band, uint16 arfcn)
{
  int32 rgi_index=0;
  uint16 f=0, f1=0, f2=0, f3=0;
  int32 freq_interp_fac;

  /* helper variable to check if interpolation is required*/
  boolean use_interpolation = FALSE;

  /* Index of freq when no interpolation is required*/
  uint8 freq_index = 0;

  /* Indices of frequencies for which interploation will be done*/
  /* interploate between low and high freq index if required*/
  uint8 freq_index_low = 0, freq_index_high = 0;
  uint16 freq_low = 0, freq_high = 0;
  
  /* get a pointer to Tx cal data*/
  gsm_tx_cal_data_nv_type *tx_cal_nv_data_ptr = NULL;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN_V(rfgsm_core_handle_ptr,"rfgsm_core_handle_ptr is NULL!");

  tx_cal_nv_data_ptr = &(rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_cal_nv_data);

  f = rfgsm_map_arfcn_linearly(band, arfcn);

  f1 = rfgsm_map_arfcn_linearly(band, tx_cal_nv_data_ptr->tx_cal_chan[0]);
  f2 = rfgsm_map_arfcn_linearly(band, tx_cal_nv_data_ptr->tx_cal_chan[1]);
  f3 = rfgsm_map_arfcn_linearly(band, tx_cal_nv_data_ptr->tx_cal_chan[2]);


  /* Cases where no interpolation is required.
   * Assumption: f1 is the leftmost freq in band. and f3 is right most freq in band. 
   */
  if (f <= f1)
  {
	use_interpolation = FALSE;
    freq_index =0;
  }
  else if (  (f== f2) || ( (f3==f2) && (f>f2) )  )
    {
    use_interpolation = FALSE;
    freq_index = 1;
    }
  else if (f>= f3)
  {
    use_interpolation = FALSE;
    freq_index = 2;
  }
  else if ( (f > f1) && (f < f2) )
  {
    use_interpolation = TRUE;
    freq_index_low  = 0;  
    freq_index_high = 1;
    freq_low  = f1;  
    freq_high = f2;  
  }
  else if ( (f>f2) && (f<f3) )
    {
    use_interpolation = TRUE;
    freq_index_low  = 1;  
    freq_index_high = 2;  
    freq_low  = f2;  
    freq_high = f3;  
  }

  if(use_interpolation == TRUE)
  {
    /* interpolate and calculate rgi vs pmeas table for all RGIs and gain states */
    freq_interp_fac = (((f - freq_low)<< 8)+((freq_high-freq_low)/2)) /(freq_high-freq_low); /*lint !e701 */

    for (rgi_index = 0; rgi_index < RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE; rgi_index++)
    {
    /* GMSK case */
      if (tx_cal_nv_data_ptr->gsm_rgi_table[0].rgi_list[rgi_index] != 0xFF)
      {
        rfgsm_core_handle_ptr->rfgsm_pmeas_g0[rgi_index] = ((int16) tx_cal_nv_data_ptr->gsm_pmeas_table[0].tx_pmeas_data_table[freq_index_low].pmeas[rgi_index] + 
                             ((((int16)tx_cal_nv_data_ptr->gsm_pmeas_table[0].tx_pmeas_data_table[freq_index_high].pmeas[rgi_index] - (int16)tx_cal_nv_data_ptr->gsm_pmeas_table[0].tx_pmeas_data_table[freq_index_low].pmeas[rgi_index]) * freq_interp_fac)>> 8));
        rfgsm_core_handle_ptr->rfgsm_pmeas_g1[rgi_index] = ((int16) tx_cal_nv_data_ptr->gsm_pmeas_table[1].tx_pmeas_data_table[freq_index_low].pmeas[rgi_index] + 
                             ((((int16)tx_cal_nv_data_ptr->gsm_pmeas_table[1].tx_pmeas_data_table[freq_index_high].pmeas[rgi_index] - (int16)tx_cal_nv_data_ptr->gsm_pmeas_table[1].tx_pmeas_data_table[freq_index_low].pmeas[rgi_index]) * freq_interp_fac)>> 8));
        rfgsm_core_handle_ptr->rfgsm_pmeas_g2[rgi_index] = ((int16) tx_cal_nv_data_ptr->gsm_pmeas_table[2].tx_pmeas_data_table[freq_index_low].pmeas[rgi_index] + 
                             ((((int16)tx_cal_nv_data_ptr->gsm_pmeas_table[2].tx_pmeas_data_table[freq_index_high].pmeas[rgi_index] - (int16)tx_cal_nv_data_ptr->gsm_pmeas_table[2].tx_pmeas_data_table[freq_index_low].pmeas[rgi_index]) * freq_interp_fac)>> 8));
        rfgsm_core_handle_ptr->rfgsm_pmeas_g3[rgi_index] = ((int16) tx_cal_nv_data_ptr->gsm_pmeas_table[3].tx_pmeas_data_table[freq_index_low].pmeas[rgi_index] + 
                             ((((int16)tx_cal_nv_data_ptr->gsm_pmeas_table[3].tx_pmeas_data_table[freq_index_high].pmeas[rgi_index] - (int16)tx_cal_nv_data_ptr->gsm_pmeas_table[3].tx_pmeas_data_table[freq_index_low].pmeas[rgi_index]) * freq_interp_fac)>> 8)); 
        rfgsm_core_handle_ptr->rfgsm_pmeas_g4[rgi_index] = ((int16) tx_cal_nv_data_ptr->gsm_pmeas_table[4].tx_pmeas_data_table[freq_index_low].pmeas[rgi_index] + 
        			         ((((int16)tx_cal_nv_data_ptr->gsm_pmeas_table[4].tx_pmeas_data_table[freq_index_high].pmeas[rgi_index] - (int16)tx_cal_nv_data_ptr->gsm_pmeas_table[4].tx_pmeas_data_table[freq_index_low].pmeas[rgi_index]) * freq_interp_fac)>> 8));
        rfgsm_core_handle_ptr->rfgsm_pmeas_g5[rgi_index] = ((int16) tx_cal_nv_data_ptr->gsm_pmeas_table[5].tx_pmeas_data_table[freq_index_low].pmeas[rgi_index] + 
        			         ((((int16)tx_cal_nv_data_ptr->gsm_pmeas_table[5].tx_pmeas_data_table[freq_index_high].pmeas[rgi_index] - (int16)tx_cal_nv_data_ptr->gsm_pmeas_table[5].tx_pmeas_data_table[freq_index_low].pmeas[rgi_index]) * freq_interp_fac)>> 8));
        rfgsm_core_handle_ptr->rfgsm_pmeas_g6[rgi_index] = ((int16) tx_cal_nv_data_ptr->gsm_pmeas_table[6].tx_pmeas_data_table[freq_index_low].pmeas[rgi_index] + 
        			         ((((int16)tx_cal_nv_data_ptr->gsm_pmeas_table[6].tx_pmeas_data_table[freq_index_high].pmeas[rgi_index] - (int16)tx_cal_nv_data_ptr->gsm_pmeas_table[6].tx_pmeas_data_table[freq_index_low].pmeas[rgi_index]) * freq_interp_fac)>> 8));
        rfgsm_core_handle_ptr->rfgsm_pmeas_g7[rgi_index] = ((int16) tx_cal_nv_data_ptr->gsm_pmeas_table[7].tx_pmeas_data_table[freq_index_low].pmeas[rgi_index] + 
        			         ((((int16)tx_cal_nv_data_ptr->gsm_pmeas_table[7].tx_pmeas_data_table[freq_index_high].pmeas[rgi_index] - (int16)tx_cal_nv_data_ptr->gsm_pmeas_table[7].tx_pmeas_data_table[freq_index_low].pmeas[rgi_index]) * freq_interp_fac)>> 8)); 
      }
    /*EDGE case*/
      if (tx_cal_nv_data_ptr->edge_rgi_table[0].rgi_list[rgi_index] != 0xFF)
      {
        rfgsm_core_handle_ptr->rfgsm_edge_pmeas_g0[rgi_index] = ((int16) tx_cal_nv_data_ptr->edge_pmeas_table[0].tx_pmeas_data_table[freq_index_low].pmeas[rgi_index] + 
                             ((((int16)tx_cal_nv_data_ptr->edge_pmeas_table[0].tx_pmeas_data_table[freq_index_high].pmeas[rgi_index] - (int16)tx_cal_nv_data_ptr->edge_pmeas_table[0].tx_pmeas_data_table[freq_index_low].pmeas[rgi_index]) * freq_interp_fac)>> 8));
        rfgsm_core_handle_ptr->rfgsm_edge_pmeas_g1[rgi_index] = ((int16) tx_cal_nv_data_ptr->edge_pmeas_table[1].tx_pmeas_data_table[freq_index_low].pmeas[rgi_index] + 
                             ((((int16)tx_cal_nv_data_ptr->edge_pmeas_table[1].tx_pmeas_data_table[freq_index_high].pmeas[rgi_index] - (int16)tx_cal_nv_data_ptr->edge_pmeas_table[1].tx_pmeas_data_table[freq_index_low].pmeas[rgi_index]) * freq_interp_fac)>> 8));
        rfgsm_core_handle_ptr->rfgsm_edge_pmeas_g2[rgi_index] = ((int16) tx_cal_nv_data_ptr->edge_pmeas_table[2].tx_pmeas_data_table[freq_index_low].pmeas[rgi_index] + 
                             ((((int16)tx_cal_nv_data_ptr->edge_pmeas_table[2].tx_pmeas_data_table[freq_index_high].pmeas[rgi_index] - (int16)tx_cal_nv_data_ptr->edge_pmeas_table[2].tx_pmeas_data_table[freq_index_low].pmeas[rgi_index]) * freq_interp_fac)>> 8));
        rfgsm_core_handle_ptr->rfgsm_edge_pmeas_g3[rgi_index] = ((int16) tx_cal_nv_data_ptr->edge_pmeas_table[3].tx_pmeas_data_table[freq_index_low].pmeas[rgi_index] + 
                             ((((int16)tx_cal_nv_data_ptr->edge_pmeas_table[3].tx_pmeas_data_table[freq_index_high].pmeas[rgi_index] - (int16)tx_cal_nv_data_ptr->edge_pmeas_table[3].tx_pmeas_data_table[freq_index_low].pmeas[rgi_index]) * freq_interp_fac)>> 8)); 
        rfgsm_core_handle_ptr->rfgsm_edge_pmeas_g4[rgi_index] = ((int16) tx_cal_nv_data_ptr->edge_pmeas_table[4].tx_pmeas_data_table[freq_index_low].pmeas[rgi_index] + 
        		             ((((int16)tx_cal_nv_data_ptr->edge_pmeas_table[4].tx_pmeas_data_table[freq_index_high].pmeas[rgi_index] - (int16)tx_cal_nv_data_ptr->edge_pmeas_table[4].tx_pmeas_data_table[freq_index_low].pmeas[rgi_index]) * freq_interp_fac)>> 8));
        rfgsm_core_handle_ptr->rfgsm_edge_pmeas_g5[rgi_index] = ((int16) tx_cal_nv_data_ptr->edge_pmeas_table[5].tx_pmeas_data_table[freq_index_low].pmeas[rgi_index] + 
        		             ((((int16)tx_cal_nv_data_ptr->edge_pmeas_table[5].tx_pmeas_data_table[freq_index_high].pmeas[rgi_index] - (int16)tx_cal_nv_data_ptr->edge_pmeas_table[5].tx_pmeas_data_table[freq_index_low].pmeas[rgi_index]) * freq_interp_fac)>> 8));
        rfgsm_core_handle_ptr->rfgsm_edge_pmeas_g6[rgi_index] = ((int16) tx_cal_nv_data_ptr->edge_pmeas_table[6].tx_pmeas_data_table[freq_index_low].pmeas[rgi_index] + 
        		             ((((int16)tx_cal_nv_data_ptr->edge_pmeas_table[6].tx_pmeas_data_table[freq_index_high].pmeas[rgi_index] - (int16)tx_cal_nv_data_ptr->edge_pmeas_table[6].tx_pmeas_data_table[freq_index_low].pmeas[rgi_index]) * freq_interp_fac)>> 8));
        rfgsm_core_handle_ptr->rfgsm_edge_pmeas_g7[rgi_index] = ((int16) tx_cal_nv_data_ptr->edge_pmeas_table[7].tx_pmeas_data_table[freq_index_low].pmeas[rgi_index] + 
        		             ((((int16)tx_cal_nv_data_ptr->edge_pmeas_table[7].tx_pmeas_data_table[freq_index_high].pmeas[rgi_index] - (int16)tx_cal_nv_data_ptr->edge_pmeas_table[7].tx_pmeas_data_table[freq_index_low].pmeas[rgi_index]) * freq_interp_fac)>> 8)); 
      }
    }
  }
  else
  {
    /*No interploation, use decided freq index and copy the values*/
    for (rgi_index = 0; rgi_index < RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE; rgi_index++)
    {
        /* GMSK case.*/
        rfgsm_core_handle_ptr->rfgsm_pmeas_g0[rgi_index] = (int16) tx_cal_nv_data_ptr->gsm_pmeas_table[0].tx_pmeas_data_table[freq_index].pmeas[rgi_index];
        rfgsm_core_handle_ptr->rfgsm_pmeas_g1[rgi_index] = (int16) tx_cal_nv_data_ptr->gsm_pmeas_table[1].tx_pmeas_data_table[freq_index].pmeas[rgi_index];
        rfgsm_core_handle_ptr->rfgsm_pmeas_g2[rgi_index] = (int16) tx_cal_nv_data_ptr->gsm_pmeas_table[2].tx_pmeas_data_table[freq_index].pmeas[rgi_index];
        rfgsm_core_handle_ptr->rfgsm_pmeas_g3[rgi_index] = (int16) tx_cal_nv_data_ptr->gsm_pmeas_table[3].tx_pmeas_data_table[freq_index].pmeas[rgi_index];	   
        rfgsm_core_handle_ptr->rfgsm_pmeas_g4[rgi_index] = (int16) tx_cal_nv_data_ptr->gsm_pmeas_table[4].tx_pmeas_data_table[freq_index].pmeas[rgi_index];
        rfgsm_core_handle_ptr->rfgsm_pmeas_g5[rgi_index] = (int16) tx_cal_nv_data_ptr->gsm_pmeas_table[5].tx_pmeas_data_table[freq_index].pmeas[rgi_index];
        rfgsm_core_handle_ptr->rfgsm_pmeas_g6[rgi_index] = (int16) tx_cal_nv_data_ptr->gsm_pmeas_table[6].tx_pmeas_data_table[freq_index].pmeas[rgi_index];
        rfgsm_core_handle_ptr->rfgsm_pmeas_g7[rgi_index] = (int16) tx_cal_nv_data_ptr->gsm_pmeas_table[7].tx_pmeas_data_table[freq_index].pmeas[rgi_index];	 
      /*EDGE case*/
        rfgsm_core_handle_ptr->rfgsm_edge_pmeas_g0[rgi_index] = (int16) tx_cal_nv_data_ptr->edge_pmeas_table[0].tx_pmeas_data_table[freq_index].pmeas[rgi_index];
        rfgsm_core_handle_ptr->rfgsm_edge_pmeas_g1[rgi_index] = (int16) tx_cal_nv_data_ptr->edge_pmeas_table[1].tx_pmeas_data_table[freq_index].pmeas[rgi_index];
        rfgsm_core_handle_ptr->rfgsm_edge_pmeas_g2[rgi_index] = (int16) tx_cal_nv_data_ptr->edge_pmeas_table[2].tx_pmeas_data_table[freq_index].pmeas[rgi_index];
        rfgsm_core_handle_ptr->rfgsm_edge_pmeas_g3[rgi_index] = (int16) tx_cal_nv_data_ptr->edge_pmeas_table[3].tx_pmeas_data_table[freq_index].pmeas[rgi_index];
        rfgsm_core_handle_ptr->rfgsm_edge_pmeas_g4[rgi_index] = (int16) tx_cal_nv_data_ptr->edge_pmeas_table[4].tx_pmeas_data_table[freq_index].pmeas[rgi_index];
        rfgsm_core_handle_ptr->rfgsm_edge_pmeas_g5[rgi_index] = (int16) tx_cal_nv_data_ptr->edge_pmeas_table[5].tx_pmeas_data_table[freq_index].pmeas[rgi_index];
        rfgsm_core_handle_ptr->rfgsm_edge_pmeas_g6[rgi_index] = (int16) tx_cal_nv_data_ptr->edge_pmeas_table[6].tx_pmeas_data_table[freq_index].pmeas[rgi_index];
        rfgsm_core_handle_ptr->rfgsm_edge_pmeas_g7[rgi_index] = (int16) tx_cal_nv_data_ptr->edge_pmeas_table[7].tx_pmeas_data_table[freq_index].pmeas[rgi_index];
    }
  }
} /* end of rfgsm_compute_linear_freq_comp */


/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function uses pcl vs pwr err data from NV to adjust PCL vs power table.
   It uses channel based interpolation and saves new PCL to pwr mapping in core handler for a given device

   @param
   rfm_dev - RF device

   @param
   band - Current band

   @param
   arfcn - Current channel
*/
void rfgsm_core_compute_pcl_vs_power (rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band, uint16 arfcn)
{
  int32 pcl_count=0;
  uint16 f = 0, f1 = 0, f2 = 0, f3 = 0, slot_count = 0;
  int32 freq_interp_fac;
  int16 gmsk_multislot_limit_adj = 0, edge_multislot_limit_adj = 0;

  /* helper variable to check if interpolation is required*/
  boolean use_interpolation = FALSE;

  /* Index of freq when no interpolation is required*/
  uint8 freq_index = 0;

  /* Indices of frequencies for which interploation will be done*/
  /* interploate between low and high freq index if required*/
  uint8 freq_index_low = 0, freq_index_high = 0;
  uint16 freq_low = 0, freq_high = 0;
  
  gsm_tx_static_nv_type *tx_static_nv_data_ptr = NULL;
  gsm_tx_cal_data_nv_type *tx_cal_nv_data_ptr = NULL;

  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN_V(rfgsm_core_handle_ptr,"rfgsm_core_handle_ptr is NULL!");
  
  tx_static_nv_data_ptr = rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr;
  tx_cal_nv_data_ptr = &(rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_cal_nv_data);

  f = rfgsm_map_arfcn_linearly(band, arfcn);

  f1 = rfgsm_map_arfcn_linearly(band, tx_cal_nv_data_ptr->tx_cal_chan[0]);
  f2 = rfgsm_map_arfcn_linearly(band, tx_cal_nv_data_ptr->tx_cal_chan[1]);
  f3 = rfgsm_map_arfcn_linearly(band, tx_cal_nv_data_ptr->tx_cal_chan[2]);

  /* Cases where no interpolation is required.
   * Assumption: f1 is the leftmost freq in band. and f3 is right most freq in band. 
   */
  if (f <= f1)
  {
    use_interpolation = FALSE;
    freq_index =0;
  }
  else if (  (f== f2) || ( (f3==f2) && (f>f2) )  )
  {
    use_interpolation = FALSE;
    freq_index = 1;
  }
  else if (f>= f3)
  {
    use_interpolation = FALSE;
    freq_index = 2;
  }
  else if ( (f > f1) && (f < f2) )
  {
    use_interpolation = TRUE;
    freq_index_low  = 0;  
    freq_index_high = 1;
    freq_low  = f1;  
    freq_high = f2;  
  }
  else if ( (f>f2) && (f<f3) )
  {
    use_interpolation = TRUE;
    freq_index_low  = 1;  
    freq_index_high = 2;  
    freq_low  = f2;  
    freq_high = f3;  
  }

  if(use_interpolation == TRUE)
  {
    /* interpolate and calculate PCL vs power curve */
    freq_interp_fac = (((f - freq_low)<< 8)+((freq_high-freq_low)/2)) /(freq_high-freq_low); /*lint !e701 */

    if (( tx_cal_nv_data_ptr->gsm_tx_pwr_err_per_pcl.tx_pwr_err_data[freq_index_low].data_valid == TRUE )&& 
        ( tx_cal_nv_data_ptr->gsm_tx_pwr_err_per_pcl.tx_pwr_err_data[freq_index_high].data_valid == TRUE ))
    {
      RFGC_MSG( MSG_LEGACY_LOW, "rfgsm_core_compute_pcl_vs_power, tx_pwr_err valid!" );

      for (pcl_count = 0; pcl_count < RFNV_NUMBER_OF_PCLS; pcl_count++)
      {
        /* GMSK case */
        rfgsm_core_handle_ptr->rfgsm_gsm_pcl_vs_power[pcl_count] = tx_static_nv_data_ptr->power_levels[pcl_count]- 
                                                                   (int16)(tx_cal_nv_data_ptr->gsm_tx_pwr_err_per_pcl.tx_pwr_err_data[freq_index_low].data[pcl_count] +
                                                                   (((tx_cal_nv_data_ptr->gsm_tx_pwr_err_per_pcl.tx_pwr_err_data[freq_index_high].data[pcl_count] - tx_cal_nv_data_ptr->gsm_tx_pwr_err_per_pcl.tx_pwr_err_data[freq_index_low].data[pcl_count]) * freq_interp_fac)>> 8)
                                                                   );
        /* EDGE case */
        rfgsm_core_handle_ptr->rfgsm_edge_pcl_vs_power[pcl_count] = tx_static_nv_data_ptr->power_levels[pcl_count]- 
                                                                   (int16)(tx_cal_nv_data_ptr->edge_tx_pwr_err_per_pcl.tx_pwr_err_data[freq_index_low].data[pcl_count] +
                                                                   (((tx_cal_nv_data_ptr->edge_tx_pwr_err_per_pcl.tx_pwr_err_data[freq_index_high].data[pcl_count] - tx_cal_nv_data_ptr->edge_tx_pwr_err_per_pcl.tx_pwr_err_data[freq_index_low].data[pcl_count]) * freq_interp_fac)>> 8)
                                                                   );
      }
      /* Update multislot pwr with new values*/  
      if (band == RFCOM_BAND_GSM850 || band == RFCOM_BAND_GSM900)
      {
        gmsk_multislot_limit_adj = (int16)(tx_cal_nv_data_ptr->gsm_tx_pwr_err_per_pcl.tx_pwr_err_data[freq_index_low].data[RFNV_NUMBER_OF_PCLS-2] +
                                                                     (((tx_cal_nv_data_ptr->gsm_tx_pwr_err_per_pcl.tx_pwr_err_data[freq_index_high].data[RFNV_NUMBER_OF_PCLS-2] - tx_cal_nv_data_ptr->gsm_tx_pwr_err_per_pcl.tx_pwr_err_data[freq_index_low].data[RFNV_NUMBER_OF_PCLS-2]) * freq_interp_fac)>> 8)
                                                                     );
        edge_multislot_limit_adj = (int16)(tx_cal_nv_data_ptr->edge_tx_pwr_err_per_pcl.tx_pwr_err_data[freq_index_low].data[RFNV_NUMBER_OF_PCLS-5] +
                                                                     (((tx_cal_nv_data_ptr->edge_tx_pwr_err_per_pcl.tx_pwr_err_data[freq_index_high].data[RFNV_NUMBER_OF_PCLS-5] - tx_cal_nv_data_ptr->edge_tx_pwr_err_per_pcl.tx_pwr_err_data[freq_index_low].data[RFNV_NUMBER_OF_PCLS-5]) * freq_interp_fac)>> 8)
                                                                     );
      }
      else
      {
        gmsk_multislot_limit_adj = (int16)(tx_cal_nv_data_ptr->gsm_tx_pwr_err_per_pcl.tx_pwr_err_data[freq_index_low].data[RFNV_NUMBER_OF_PCLS-1] +
                                                                     (((tx_cal_nv_data_ptr->gsm_tx_pwr_err_per_pcl.tx_pwr_err_data[freq_index_high].data[RFNV_NUMBER_OF_PCLS-1] - tx_cal_nv_data_ptr->gsm_tx_pwr_err_per_pcl.tx_pwr_err_data[freq_index_low].data[RFNV_NUMBER_OF_PCLS-1]) * freq_interp_fac)>> 8)
                                                                     );
        edge_multislot_limit_adj = (int16)(tx_cal_nv_data_ptr->edge_tx_pwr_err_per_pcl.tx_pwr_err_data[freq_index_low].data[RFNV_NUMBER_OF_PCLS-3] +
                                                                     (((tx_cal_nv_data_ptr->edge_tx_pwr_err_per_pcl.tx_pwr_err_data[freq_index_high].data[RFNV_NUMBER_OF_PCLS-3] - tx_cal_nv_data_ptr->edge_tx_pwr_err_per_pcl.tx_pwr_err_data[freq_index_low].data[RFNV_NUMBER_OF_PCLS-3]) * freq_interp_fac)>> 8)
                                                                     );
      }
    }
    else
    {
      RFGC_MSG( MSG_LEGACY_LOW, "rfgsm_core_compute_pcl_vs_power TX v2 cal is being used" );

      for (pcl_count = 0; pcl_count < RFNV_NUMBER_OF_PCLS; pcl_count++)
      {
        rfgsm_core_handle_ptr->rfgsm_gsm_pcl_vs_power[pcl_count] = tx_static_nv_data_ptr->power_levels[pcl_count];
        rfgsm_core_handle_ptr->rfgsm_edge_pcl_vs_power[pcl_count] = tx_static_nv_data_ptr->power_levels[pcl_count];
      }
    }
  }
  else
  {
     /* No interpolation, use decided freq index and compensate the pcl vs power curve */
    if ( tx_cal_nv_data_ptr->gsm_tx_pwr_err_per_pcl.tx_pwr_err_data[freq_index].data_valid == TRUE )
    {
      RFGC_MSG( MSG_LEGACY_LOW,"rfgsm_core_compute_pcl_vs_power tx_pwr_err tx_pwr_err valid!" );

      for (pcl_count = 0; pcl_count < RFNV_NUMBER_OF_PCLS; pcl_count++)
      {
        /* GMSK case.*/
        rfgsm_core_handle_ptr->rfgsm_gsm_pcl_vs_power[pcl_count] = tx_static_nv_data_ptr->power_levels[pcl_count] -
                                                                     tx_cal_nv_data_ptr->gsm_tx_pwr_err_per_pcl.tx_pwr_err_data[freq_index].data[pcl_count];
        /*EDGE case*/
        rfgsm_core_handle_ptr->rfgsm_edge_pcl_vs_power[pcl_count] = tx_static_nv_data_ptr->power_levels[pcl_count] -
                                                                     tx_cal_nv_data_ptr->edge_tx_pwr_err_per_pcl.tx_pwr_err_data[freq_index].data[pcl_count];
      }
      /* Update multislot pwr with new values*/
      if (band == RFCOM_BAND_GSM850 || band == RFCOM_BAND_GSM900)
      {
        gmsk_multislot_limit_adj = tx_cal_nv_data_ptr->gsm_tx_pwr_err_per_pcl.tx_pwr_err_data[freq_index].data[RFNV_NUMBER_OF_PCLS-2];
        edge_multislot_limit_adj = tx_cal_nv_data_ptr->edge_tx_pwr_err_per_pcl.tx_pwr_err_data[freq_index].data[RFNV_NUMBER_OF_PCLS-5];
      }
      else
      {
        gmsk_multislot_limit_adj = tx_cal_nv_data_ptr->gsm_tx_pwr_err_per_pcl.tx_pwr_err_data[freq_index].data[RFNV_NUMBER_OF_PCLS-1];
        edge_multislot_limit_adj = tx_cal_nv_data_ptr->edge_tx_pwr_err_per_pcl.tx_pwr_err_data[freq_index].data[RFNV_NUMBER_OF_PCLS-3];
      }
    }
    else
    {
      RFGC_MSG( MSG_LEGACY_LOW, "rfgsm_core_compute_pcl_vs_power TX v2 cal is being used" );
      /* Initialize default value from PCL vs power NV */
      for (pcl_count = 0; pcl_count < RFNV_NUMBER_OF_PCLS; pcl_count++)
      {
        rfgsm_core_handle_ptr->rfgsm_gsm_pcl_vs_power[pcl_count] = tx_static_nv_data_ptr->power_levels[pcl_count];
        rfgsm_core_handle_ptr->rfgsm_edge_pcl_vs_power[pcl_count] = tx_static_nv_data_ptr->power_levels[pcl_count];
      }
    }
  }
  /* Update the multislot limit in core handle */
  for ( slot_count = 0; slot_count < RFGSM_MAX_MULTISLOTS; slot_count++ )
  {
    rfgsm_core_handle_ptr->rfgsm_gsm_multislot_tx_pwr_limit[slot_count] = tx_static_nv_data_ptr->multislot_max_tx_power.gsm_multislot_tx_pwr_gmsk[slot_count] - gmsk_multislot_limit_adj;
    rfgsm_core_handle_ptr->rfgsm_edge_multislot_tx_pwr_limit[slot_count] = tx_static_nv_data_ptr->multislot_max_tx_power.gsm_multislot_tx_pwr_8psk[slot_count] - edge_multislot_limit_adj;
  }
} /* end of rfgsm_core_compute_pcl_vs_power */

/*----------------------------------------------------------------------------*/
/*!
   @brief returns the correct RGI table and Pmeas table depending on edge/gsm and pa range.

   @param rfm_dev - Logical RF device
   @param pa_range - PA range
   @param mod_type - Modulation type
   @param **pmeas_tbl - pointer to power measure table
   @param **rgi_tbl - pointer to RGI lsit
   @param *rgi_list_size - Pointer to size of RGI list


   @retval
   none
*/
void rfgsm_core_get_rgi_pmeas_table( rfm_device_enum_type rfm_dev, 
                                     uint8 pa_range, 
                                     rfgsm_modulation_type mod_type,
                                     int32 **pmeas_tbl, 
                                     uint8 **rgi_tbl,
                                     uint8 *rgi_list_size )
{

  gsm_tx_cal_data_nv_type *tx_cal_nv_data_ptr = NULL;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN_V( rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!");

  if ((rgi_tbl == NULL) || (pmeas_tbl == NULL))
  {
     RFGC_MSG( MSG_LEGACY_ERROR, "rgi or pmeas Table is NULL!" );
     return;
  }

  /* get a pointer to Tx cal data*/
  tx_cal_nv_data_ptr = &(rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_cal_nv_data);
  
  if (mod_type == RF_MOD_GMSK)
  {
     if (tx_cal_nv_data_ptr->gsm_rgi_table[pa_range].data_valid == TRUE)
     {
       *rgi_tbl = (uint8 *)(tx_cal_nv_data_ptr->gsm_rgi_table[pa_range].rgi_list);
       *rgi_list_size = tx_cal_nv_data_ptr->gsm_rgi_table[pa_range].rgi_list_size;
     }
     else
     {
       RFGC_MSG_1( MSG_LEGACY_ERROR, "rfgsm_core_get_rgi_pmeas_table: Invalid RGI list PA range: %d", pa_range );
     }

     switch (pa_range)
     {
     case 0: 
        *pmeas_tbl = rfgsm_core_handle_ptr->rfgsm_pmeas_g0;
        break;
     case 1: 
        *pmeas_tbl = rfgsm_core_handle_ptr->rfgsm_pmeas_g1;
        break;
     case 2: 
        *pmeas_tbl = rfgsm_core_handle_ptr->rfgsm_pmeas_g2;
        break;
     case 3: 
        *pmeas_tbl = rfgsm_core_handle_ptr->rfgsm_pmeas_g3;
        break;
     case 4: 
        *pmeas_tbl = rfgsm_core_handle_ptr->rfgsm_pmeas_g4;
        break;
     case 5: 
        *pmeas_tbl = rfgsm_core_handle_ptr->rfgsm_pmeas_g5;
        break;
     case 6: 
        *pmeas_tbl = rfgsm_core_handle_ptr->rfgsm_pmeas_g6;
        break;
     case 7: 
        *pmeas_tbl = rfgsm_core_handle_ptr->rfgsm_pmeas_g7;
        break;

     default:
        break;
     }
  }
  else if (mod_type == RF_MOD_8PSK)
  {
    if (tx_cal_nv_data_ptr->edge_rgi_table[pa_range].data_valid == TRUE)
    {
      *rgi_tbl = (uint8 *)(tx_cal_nv_data_ptr->edge_rgi_table[pa_range].rgi_list);
      *rgi_list_size = tx_cal_nv_data_ptr->edge_rgi_table[pa_range].rgi_list_size;
    }
    else
    {
      RFGC_MSG_1( MSG_LEGACY_ERROR, "rfgsm_core_get_rgi_pmeas_table: Invalid RGI list PA range: %d", pa_range);	 
    }

    switch (pa_range)
    {
    case 0: 
      *pmeas_tbl = rfgsm_core_handle_ptr->rfgsm_edge_pmeas_g0;
      break;
    case 1: 
      *pmeas_tbl = rfgsm_core_handle_ptr->rfgsm_edge_pmeas_g1;
      break;
    case 2: 
      *pmeas_tbl = rfgsm_core_handle_ptr->rfgsm_edge_pmeas_g2;
      break;
    case 3: 
      *pmeas_tbl = rfgsm_core_handle_ptr->rfgsm_edge_pmeas_g3;
      break;
    case 4: 
      *pmeas_tbl = rfgsm_core_handle_ptr->rfgsm_edge_pmeas_g4;
      break;
    case 5: 
      *pmeas_tbl = rfgsm_core_handle_ptr->rfgsm_edge_pmeas_g5;
      break;
    case 6: 
      *pmeas_tbl = rfgsm_core_handle_ptr->rfgsm_edge_pmeas_g6;
      break;
    case 7: 
      *pmeas_tbl = rfgsm_core_handle_ptr->rfgsm_edge_pmeas_g7;
      break;

    default:
      break;
    }
  }

}


/*!
  @brief
  This function computes the freq compensation factor that needs to be
  be applied to the TX power to be applied to envelope gain. This is to
  improve PCL v/s TX power linearity
 
  @details
  1)  A portion of the power control is done on digital base-band gain (dig BB gain)
  2)  The current power control algorithm assumes the TX  DAC Vs Pout characteristics is
      perfectly linear, i.e there is a dB/dB change in Pout for each dB delta in dig BB gain.
  3)  In reality, there is always some signal compression (or even expansion) on the cascaded
      RTR+PA blocks. Consequently, the dig BB gain estimation is not accurate and is reflected
      on up to 0.5dB overshoot when TX level is commanded to increase just 0.1dB
  4)  Proposed solution is to linearize the dig BB gain estimation by applying a slope correction:
      a.  digBBgain_Comp =  digBBgain/M(RGI) = digBBgain*(1/M(RGI)) where (1/M(RGI)) is obtained from
      a characterized table
  5)  The dig BB gain slope compensation coefficients can be derived by performing TX  DA Cal at nom
      envgain* and at a reduced envgain
  6)  This compensation is only applied to GMSK modulation

      *envgain = NV#5477, NV_GSM_LINEAR_TX_GAIN_PARAM_I

  @param
  *delta_dbx100 - Pointer to the difference between requested set tx power and
  the pmeas from NV for corresponding rgi in dbx100
 
  rgi_index - Closest RGI index which gives a power greater than the requested
  tx power
 
*/
void rfgsm_core_compute_env_gain_freq_comp
(
rfm_device_enum_type rfm_dev,
int16 *delta_dbx100,
uint8 rgi_index
)
{
  uint16 f,f1,f2,f3; //rfgsm_core_handle_ptr->rf_tx_arfcn
  uint16 bb_comp_f1=0, bb_comp_f2=0, bb_comp_f3=0;
  uint32 freq_interp_fac = 0;
  int32 interim_bb_delta = 0;
  /* pointers to Tx NVs*/
  gsm_tx_static_nv_type *tx_static_nv_data_ptr = NULL;
  gsm_tx_cal_data_nv_type *tx_cal_nv_data_ptr = NULL;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN_V(rfgsm_core_handle_ptr,"rfgsm_core_handle_ptr is NULL!");

  f = rfgsm_core_handle_ptr->rf_tx_arfcn;
  tx_static_nv_data_ptr = rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr;
  tx_cal_nv_data_ptr = &(rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_cal_nv_data);

  /*Get the corresponding channel number:*/
  if (rfgsm_core_handle_ptr->tx_band==RFCOM_BAND_GSM900)
  {
    if ( (f >= 975) && (f < 1024) )
    {
      f = f - 975;
    }
    else if (f <= 124)
    {
      f = f + 49;
    }
  }
  else
  {
    f = f;
  }

  if((rgi_index > RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE-1) ||
     (*delta_dbx100 > RFGSM_CORE_MAX_ENV_BACKOFF_dBx100))
  {
    RFGC_MSG_2( MSG_LEGACY_ERROR,"Compute env gain freq comp: Invalid arguments: %d, %d",
          *delta_dbx100, rgi_index);
    *delta_dbx100 = 0;
    return;
  }
  
  f1 = tx_cal_nv_data_ptr->tx_cal_chan[0];
  f2 = tx_cal_nv_data_ptr->tx_cal_chan[1];
  f3 = tx_cal_nv_data_ptr->tx_cal_chan[2];

  bb_comp_f1 = tx_static_nv_data_ptr->env_gain.env_gain_freq_rgi_f1[rgi_index];
  bb_comp_f2 = tx_static_nv_data_ptr->env_gain.env_gain_freq_rgi_f2[rgi_index];
  bb_comp_f3 = tx_static_nv_data_ptr->env_gain.env_gain_freq_rgi_f3[rgi_index];

  /* Perform frequency compensation for the env. gain compensation */
  if(f <= f1)
  {
    interim_bb_delta = bb_comp_f1 * (*delta_dbx100);
    interim_bb_delta = (interim_bb_delta) >> 10;
  }
  
  if((f == f2) || ( (f3 == f2) && (f > f2) ))
  {
    interim_bb_delta = bb_comp_f2 * (*delta_dbx100);     
    interim_bb_delta = (interim_bb_delta) >> 10; 
  }
  
  if(f >= f3)
  {
    interim_bb_delta = bb_comp_f3 * (*delta_dbx100);
    interim_bb_delta = (interim_bb_delta) >> 10;
  }
  
  if((f > f1) && (f < f2))
  {
    freq_interp_fac = (((f-f1)<< 8)+((f2-f1)/2)) /(f2-f1); /*lint !e701 */
    interim_bb_delta = (*delta_dbx100) * (uint16)(bb_comp_f1 + (((bb_comp_f2 - bb_comp_f1)*freq_interp_fac)>>8));
    interim_bb_delta = (interim_bb_delta) >> 10;
  }
  
  if((f > f2) && (f < f3))
  {
    freq_interp_fac = (((f-f2)<< 8)+((f3-f2)/2)) /(f3-f2); /*lint !e701 */
    interim_bb_delta = (*delta_dbx100) * (uint16)(bb_comp_f2 + (((bb_comp_f3 - bb_comp_f2)*freq_interp_fac)>>8));
    interim_bb_delta = (interim_bb_delta) >> 10;
  }
  
  *delta_dbx100 = (int16)(interim_bb_delta);

}


/*----------------------------------------------------------------------------*/
/*!
   @brief
   1) Get the right RGI table and Pmeas table (depending on edge/gsm and pa range).                      
   2) Go through Pmeas Table and find the first power > rfgsm_pcl_pwr_dbm. This is
     the reqd RGI index.  

   @param
   rfgsm_pcl_pwr_dbm - 

   @param
   pa_range - PA range

   @param
   slot_num - 

   @param
   mod_type - Modulation type

   @retval
   return backoff value
*/
uint16  rfgsm_core_set_tx_agc ( rfm_device_enum_type rfm_dev, int32 rfgsm_pcl_pwr_dbm, uint8 pa_range, uint8 slot_num,
                               rfgsm_modulation_type mod_type, uint8 *rgi_ptr)
{
  int32 * pmeas_tbl = NULL;
  uint8 * rgi_tbl = NULL;
  uint8 i = 0, rgi_index_hi = 0, rgi_index_lo = 0;
  uint16 env_val = 0;
  uint8 rgi_index=0xFF;
  uint8 rgi_list_size=0;
  int32 max_pmeas=-9999;
  uint8 max_pmeas_rgi_index=0;
  int16 delta_dbx100 = 0;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  gsm_tx_static_nv_type *tx_static_nv_data_ptr = NULL;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!", env_val);

  /* pointers to Tx static NVs*/
  tx_static_nv_data_ptr  = rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr;

  /* STEP 1: Get RGI and PMEAS tables*/ 
  rfgsm_core_get_rgi_pmeas_table(rfm_dev, pa_range, mod_type, &pmeas_tbl, &rgi_tbl, &rgi_list_size);

  if ((rgi_tbl == NULL) || (pmeas_tbl == NULL))
  {
     RFGC_MSG_1( MSG_LEGACY_ERROR, "rgi or pmeas Table is NULL: Invalid pa_range : %d", pa_range );
     return 0;
  }

  /*Now, enter the loop to find the approriate RGI value:*/
  for (i = 0; i < RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE; i++)
  {

     /* Keep track of Pmeas and corresponding RGI */
     if (pmeas_tbl[i] > max_pmeas)
     {
        max_pmeas = pmeas_tbl[i];
        max_pmeas_rgi_index = i;
     }

     /* If the req'd power level is greater than the maximum calibrated power value, 
      * i.e. the next entry of the PMEAS table is set to 0xFF, then limit the req'd 
      * power to the maximum power possible. Also, make sure you get into the case 
      * statement below and decrease the value of i into the previous valid PMEAS entry.*/
     if (rgi_tbl[i] == 0xFF)
     {
        break;
     }
     /* Go until you find a PMEAS entry that is greater than the power level
      that needs to be set. After that also take the PMEASE entry that is 
    one lower. Then use the one that is closest and backoff accordingly.
    Note: we can only go higher by 1dB (100 dBm/100), take this into account.*/
     if (pmeas_tbl[i] >= rfgsm_pcl_pwr_dbm)
     {
        if (i==0)
        {
           rgi_index = 0;
        }
        else
        {
           rgi_index_hi = i; 
           rgi_index_lo = i-1;

           if (((rfgsm_pcl_pwr_dbm-pmeas_tbl[rgi_index_lo]) < (pmeas_tbl[rgi_index_hi]-rfgsm_pcl_pwr_dbm)) &&
               ((rfgsm_pcl_pwr_dbm-pmeas_tbl[rgi_index_lo]) <= 100))
           {
              rgi_index = rgi_index_lo;
           }
           else
           {
              rgi_index = rgi_index_hi;
           }
        }
        break;
     }
  }

  /* If no entry is found in the pmeas table, it implies that requested power is greater than calibrated
     power. In that case use the RGI corresponding to the max power possible
  */
  if (rgi_index == 0xFF)
  {
     RFGC_MSG_1( MSG_LEGACY_HIGH, "No entry found in Pmeas table!!! Using max_pmeas_rgi_index= %d", 
                 max_pmeas_rgi_index );

     rgi_index = max_pmeas_rgi_index;
  }

  // Sanity test the rgi index against the rgi_list_size
  if( rgi_index >= rgi_list_size )
  {
    // Limit the RGI index to rgi_list_size-1
    RFGC_MSG_4( MSG_LEGACY_ERROR, "RGI above max size: RGI_index=%d, list_size=%d, pa_range=%d, mod_type=%d", 
                rgi_index, rgi_list_size, pa_range, mod_type);
    	
    rgi_index = rgi_list_size-1;
  }

  if(((int16)(pmeas_tbl[rgi_index] - rfgsm_pcl_pwr_dbm) < (RFGSM_CORE_MAX_NON_PREDIST_BB_BOOST)) )
  {
    /* For GMSK and non-predist. 8PSK limit BB gain from -24.9dB to +1dB */
    delta_dbx100 = RFGSM_CORE_MAX_NON_PREDIST_BB_BOOST;
    RFGC_MSG_1( MSG_LEGACY_HIGH, "GMSK/non-PD BB gain range is -24.9dB to +1dB ! BB gain requested = %d dBx100", 
        (delta_dbx100) );
  }
  else if(((int16)(pmeas_tbl[rgi_index] - rfgsm_pcl_pwr_dbm) > (RFGSM_CORE_MAX_ENV_BACKOFF_dBx100)) )
  {
    /* For GMSK and non-predist. 8PSK limit BB gain from -24.9dB to +1dB */
    delta_dbx100 = RFGSM_CORE_MAX_ENV_BACKOFF_dBx100;
    RFGC_MSG_1( MSG_LEGACY_HIGH, "GMSK/non-PD BB gain range is -24.9dB to +1dB ! BB gain requested = %d dBx100", 
        (delta_dbx100) );
  }
  else
  {
    delta_dbx100 = (int16)(pmeas_tbl[rgi_index] - rfgsm_pcl_pwr_dbm);
  }

  /* Scale the delta that will be applied to baseband gain (only for GMSK && high PA range) */
  if((pa_range == 0) && (mod_type ==RF_MOD_GMSK) )
  {
    rfgsm_core_compute_env_gain_freq_comp(rfm_dev, &delta_dbx100, rgi_index);
    if(delta_dbx100 < RFGSM_CORE_MAX_NON_PREDIST_BB_BOOST)
    {
      /* For GMSK limit BB gain to +1dB after BB compensation */
      delta_dbx100 = RFGSM_CORE_MAX_NON_PREDIST_BB_BOOST;
    }
  }

  if (mod_type == RF_MOD_GMSK)
  {
     env_val = (uint16)rfgsm_msm_get_env_backedoff_value(mod_type, delta_dbx100,tx_static_nv_data_ptr->tx_gain_param.gsm_linear_tx_gain_val);
  }
  else if (mod_type == RF_MOD_8PSK)
  {
     env_val = (uint16)rfgsm_msm_get_env_backedoff_value(mod_type, delta_dbx100,tx_static_nv_data_ptr->tx_gain_param.edge_linear_tx_gain_val);
  }

  if (rfgsm_debug_rgi_fix != 0)
  {
    (void)rfdevice_gsm_set_rgi(rfm_dev, rfgsm_debug_rgi_fix, mod_type, slot_num, TRUE);
  }
  else
  {
    (void)rfdevice_gsm_set_rgi(rfm_dev, rgi_tbl[rgi_index], mod_type, slot_num, TRUE);
  }

  /* Return the RGI value to the caller for logging purpose */
  if(rgi_ptr != NULL)
  {
     *rgi_ptr = rgi_index;
  }

  /*return backoff value:*/
  return env_val;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
  This function gets the tx power setting override flag.

   @details

   @param 

   @retval 
  Boolean TRUE:  Override
          FALSE: No Override

*/
static boolean rfgsm_core_get_slot_override_flag( rfm_device_enum_type rfm_dev )
{
  rfgsm_core_handle_type * rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!", FALSE);

  return rfgsm_core_handle_ptr->slot_override_flag;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
  for a given PCL return the tx power in dBm by considering the modulation type (GMSK or 8PSK) and
  the number of active uplink slots

   @details
  for a given PCL return the tx power in dBm by considering the modulation type (GMSK or 8PSK) and
  the number of active uplink slots
 
   @param 
  'mod_type' - GMSK or 8PSK
  'pcl'      - PCL for which power in dBm is needed.
  'uplink_slots' - the number of active tx slots.
 
   @retval 
  power_in-dbm':  the tx power in dBm for given PCL, modulation type and number of active tx slots.

*/
int32 rfgsm_core_tx_pwr_in_dbm_get(rfm_device_enum_type rfm_dev, rfgsm_modulation_type mod_type, uint16 pcl, uint8 uplink_slots, uint8 slot_count)
{
  int32 pwr_in_dbm = 0;
  int16 multislot_max_pwr_dbm = 0;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN( rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!", pwr_in_dbm );
  RF_NULL_CHECK_RTN( rfgsm_core_handle_ptr->rfgsm_core_tx_pow_mod, "rfgsm_core_tx_pow_mod is NULL!", pwr_in_dbm );

  if (mod_type == RF_MOD_GMSK)
  {
    multislot_max_pwr_dbm = rfgsm_core_handle_ptr->rfgsm_gsm_multislot_tx_pwr_limit[uplink_slots-1];
  }
  else
  {
    multislot_max_pwr_dbm = rfgsm_core_handle_ptr->rfgsm_edge_multislot_tx_pwr_limit[uplink_slots-1];
  }

  if(ftm_gsm_cal_mode_enabled(rfm_dev)) /*Check if in FTM cal mode*/
  {
    /* in FTM the pcl is the slot index, it is NOT the actual PCL value. so, the
    rfgsm_mdsp_tx_pow_mod[] contains the power in dBm to tx for each uplink slots */ 
    pwr_in_dbm = rfgsm_core_handle_ptr->rfgsm_core_tx_pow_mod[slot_count].power_level;
  }
  else
  {
    /* get the PCL to power in dBm mapping value */
    if (mod_type == RF_MOD_GMSK)
    {
      pwr_in_dbm = rfgsm_core_handle_ptr->rfgsm_gsm_pcl_vs_power[pcl];
    }
    else
    {
      pwr_in_dbm = rfgsm_core_handle_ptr->rfgsm_edge_pcl_vs_power[pcl];
    }
	
    /* make sure that the power in dBm doesn't exceed the max pwr allowed */
    if ( pwr_in_dbm > multislot_max_pwr_dbm )
    {
      pwr_in_dbm = multislot_max_pwr_dbm;
    }
  }

  return pwr_in_dbm;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
  apply the SAR limiting factor and return the pwr_in_dbm after applying the SAR limit.

   @details
  apply the SAR limiting factor and return the pwr_in_dbm after applying the SAR limit.
 
   @param 
  'mod_type' - GMSK or 8PSK
  'pwr_in_dbm'- pwr in dBm for which SAR limiting need to be applied.
  'uplink_slots'  - the number of active tx slots.
 
   @retval 
  power_in-dbm':  the tx power in dBm after applying the SAR limiting.

*/
int32 rfgsm_core_tx_pwr_apply_sar_limiting(rfm_device_enum_type rfm_dev, int32 pwr_in_dbm, rfgsm_modulation_type mod_type, uint8 uplink_slots)
{
  rfnv_gsm_sar_unit_type sar_cap={RFNV_GSM_SAR_BACK_OFF_LIMIT_DEFAULT,RFNV_GSM_SAR_BACK_OFF_LIMIT_DEFAULT};
  int16 sar_limit = 0;
  int16 simult_sar_limit = 0;
  rfm_common_sar_state_type sar_state;
  int32 pwr_after_sar_limit;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr;
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!", pwr_in_dbm);

  pwr_after_sar_limit = pwr_in_dbm;

  /* Get the SAR limit for GMSK and 8PSK according to the number of UL slots and SAR index */
  sar_state = rfcommon_core_sar_get_state();
  rfgsm_core_handle_ptr->tx_log.sar_state = (int32)sar_state;

  /* Get the Simultaneous Tx pwr limit according to the modulation and number of UL slots*/
  simult_sar_limit = rfgsm_core_get_simult_tx_pwr_limit(rfm_dev, mod_type, uplink_slots);

  rfgsm_nv_cmn_get_sar_value(rfm_dev, uplink_slots, sar_state, &sar_cap);

  /* If the sar_state is within the range, cap the output power to the SAR limit NV LUT */
  if ((sar_state != RF_SAR_STATE_DEFAULT) && (sar_state < RF_SAR_STATE_MAX))
  {
    switch (mod_type)
    {
    case RF_MOD_GMSK:
      sar_limit = sar_cap.sar_gmsk;
      break;

    case RF_MOD_8PSK:
      sar_limit = sar_cap.sar_8psk;
      break;

    case RF_MOD_UNKNOWN:
      sar_limit = RFNV_GSM_SAR_BACK_OFF_LIMIT_DEFAULT; /* 127 dBm -- no back off */
      break;
    default:
      sar_limit = RFNV_GSM_SAR_BACK_OFF_LIMIT_DEFAULT; /* 127 dBm -- no back off */
      break;
    }

  /* Don't apply SAR limiting in cal mode*/
  if(!ftm_gsm_cal_mode_enabled(rfm_dev))
  {
    /* Don't allow SAR limit goes lower than the standard */
    if (rfgsm_core_handle_ptr->tx_band ==RFCOM_BAND_GSM850 || rfgsm_core_handle_ptr->tx_band == RFCOM_BAND_GSM900)
    {
      if (sar_limit < RFGSM_CORE_TX_LOWBAND_SAR_LIMIT_MIN)
      {
        sar_limit = RFGSM_CORE_TX_LOWBAND_SAR_LIMIT_MIN;
      }
    }
    else
    {
      if (sar_limit < RFGSM_CORE_TX_HIGHBAND_SAR_LIMIT_MIN)
      {
        sar_limit = RFGSM_CORE_TX_HIGHBAND_SAR_LIMIT_MIN;
      }
    }

   /* Final SAR limit is Min of DSI based SAR and Simultaneous transmir max pwr limit */
    sar_limit = MIN(sar_limit, simult_sar_limit);

    /* Cap it to the SAR limit */
    if (pwr_after_sar_limit > sar_limit)
    {
      pwr_after_sar_limit = sar_limit;
    }
  }
}

  return pwr_after_sar_limit;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
  This function returns the TX offset based on the temperature and the TX power

   @details
  This function returns the TX offset based on the temperature and the TX power

   @param 
  'pwr_in_dbm' - TX power to be set in dbx100 (PCL power +/- multislot backoff     
                 +/- SAR backoff)     
  ' scaled_temp_K' - Temp Comp scaling factor
  'mod_type'  - Modulation type
   @retval 
  'pcl_pwr_offset' - PCL based Tx power compesation to be applied. 

*/
int32 rfgsm_core_get_temp_comp_pwr_offset(rfm_device_enum_type rfm_dev, int32 pwr_in_dbm, int32 scaled_temp_K, 
                                     rfgsm_modulation_type mod_type)
{

  uint8 pcl_index = 0;
  uint8 pcl_count = 0;
  uint16 pcl_pwr_offset = 0;
  int32 temp_offset = 0;
  int32 lin_interp_fac = 0;
  int16 p1=0,p2=0;
  rfcom_gsm_band_type tx_band;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  gsm_tx_static_nv_type *tx_static_nv_data_ptr  = NULL;
  int16 *pcl_vs_pwr;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!", 0);

  tx_static_nv_data_ptr  = rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr;
  tx_band = rfgsm_core_handle_ptr->tx_band;

  /* get the mapping PCL to power in dBm value */
  if (mod_type == RF_MOD_GMSK)
  {
    pcl_vs_pwr = rfgsm_core_handle_ptr->rfgsm_gsm_pcl_vs_power;
  }
  else
  {
    pcl_vs_pwr = rfgsm_core_handle_ptr->rfgsm_edge_pcl_vs_power;
  }

  /* Find the PCL index */
  while(pcl_count < RFNV_NUMBER_OF_PCLS)
  {
    if ( pwr_in_dbm >= pcl_vs_pwr[pcl_count])
    {
      pcl_index = pcl_count;
    }
    else
    {
      break;
    }
    pcl_count++;
  }
  if ((tx_band == RFCOM_BAND_GSM900 || tx_band == RFCOM_BAND_GSM850) && (pcl_index == (RFNV_NUMBER_OF_PCLS-1)))  
  {
    pcl_index = RFNV_NUMBER_OF_PCLS-2; /* Because total number of PCLs for low bands is 15 */
  }  

  if((pcl_index == 0) ||
     ((tx_band == RFCOM_BAND_GSM900 || tx_band == RFCOM_BAND_GSM850) && (pcl_index == (RFNV_NUMBER_OF_PCLS-2))) ||
     ((tx_band == RFCOM_BAND_GSM1800 || tx_band == RFCOM_BAND_GSM1900) && (pcl_index == (RFNV_NUMBER_OF_PCLS-1))) )
  {
    if(scaled_temp_K <= 0)
    {
      if (mod_type == RF_MOD_GMSK)
      {
        pcl_pwr_offset = tx_static_nv_data_ptr->temp_comp_data.cold_temp_comp_pcl_pwr_offset_gsmk[pcl_index];
      }
      else
      {
        pcl_pwr_offset = tx_static_nv_data_ptr->temp_comp_data.cold_temp_comp_pcl_pwr_offset_8psk[pcl_index];
      }
    }
    else /*scaled_temp_K > 0 */
    {
      if (mod_type == RF_MOD_GMSK)
      {
        pcl_pwr_offset = tx_static_nv_data_ptr->temp_comp_data.hot_temp_comp_pcl_pwr_offset_gsmk[pcl_index];
      }
      else
      {
        pcl_pwr_offset = tx_static_nv_data_ptr->temp_comp_data.hot_temp_comp_pcl_pwr_offset_8psk[pcl_index];
      }
    }
  }
  else /* pcl_index !=0 or pcl_index != max pcl_index; then linearly interpolate the offsets between index i+1 and i */
  {
    /* bound check */
    if(pcl_index+1 >= RFNV_NUMBER_OF_PCLS)
    {
      RFGC_MSG_1( MSG_LEGACY_ERROR, "Incorrect pcl index(%d). Temp offset not applied!", pcl_index );
      return 0;
    }
    p1 = pcl_vs_pwr[pcl_index];
    p2 = pcl_vs_pwr[pcl_index+1];
    lin_interp_fac = (((pwr_in_dbm-p1)<< 8)+((p2-p1)/2)) /(p2-p1);
    if(scaled_temp_K <= 0)
    {      
      if (mod_type == RF_MOD_GMSK)
      {
        pcl_pwr_offset = ((int16) tx_static_nv_data_ptr->temp_comp_data.cold_temp_comp_pcl_pwr_offset_gsmk[pcl_index] + 
                             ((((int16)tx_static_nv_data_ptr->temp_comp_data.cold_temp_comp_pcl_pwr_offset_gsmk[pcl_index+1] - tx_static_nv_data_ptr->temp_comp_data.cold_temp_comp_pcl_pwr_offset_gsmk[pcl_index])*lin_interp_fac) >> 8));
      }
      else
      {
        pcl_pwr_offset = ((int16) tx_static_nv_data_ptr->temp_comp_data.cold_temp_comp_pcl_pwr_offset_8psk[pcl_index] + 
                               ((((int16)tx_static_nv_data_ptr->temp_comp_data.cold_temp_comp_pcl_pwr_offset_8psk[pcl_index+1] - tx_static_nv_data_ptr->temp_comp_data.cold_temp_comp_pcl_pwr_offset_8psk[pcl_index])*lin_interp_fac) >> 8));
      }
    }
    else /*scaled_temp_K > 0 */
    {
      if (mod_type == RF_MOD_GMSK)
      {
        pcl_pwr_offset = ((int16) tx_static_nv_data_ptr->temp_comp_data.hot_temp_comp_pcl_pwr_offset_gsmk[pcl_index] + 
                             ((((int16)tx_static_nv_data_ptr->temp_comp_data.hot_temp_comp_pcl_pwr_offset_gsmk[pcl_index+1] - tx_static_nv_data_ptr->temp_comp_data.hot_temp_comp_pcl_pwr_offset_gsmk[pcl_index])*lin_interp_fac) >> 8));
      }
      else
      {
        pcl_pwr_offset = ((int16) tx_static_nv_data_ptr->temp_comp_data.hot_temp_comp_pcl_pwr_offset_8psk[pcl_index] + 
                             ((((int16)tx_static_nv_data_ptr->temp_comp_data.hot_temp_comp_pcl_pwr_offset_8psk[pcl_index+1] - tx_static_nv_data_ptr->temp_comp_data.hot_temp_comp_pcl_pwr_offset_8psk[pcl_index])*lin_interp_fac) >> 8));
      }
    } 
  } /* pcl_index */

  temp_offset = pcl_pwr_offset*scaled_temp_K;
  /*rfgsm_nv_tbl->hot_temp_comp_pcl_pwr_off_gsmk is in dbX100 and scale value is in percentage as we need final value in dbx100.. divide by 100*/
  temp_offset = temp_offset/100;

  if( rfgsm_core_handle_ptr->therm_read.enable_logging == TRUE )
  {
      RFGC_MSG_5( MSG_LEGACY_MED, "DBG: PCL pwr offset = %d, Temp comp power offset = %d, pwr_in_dbm = %d, equivalent pcl_index = %d, scaled_temp_K = %d",
            pcl_pwr_offset, temp_offset, pwr_in_dbm, pcl_index, scaled_temp_K );
      RFGC_MSG_3( MSG_LEGACY_MED, "DBG: p1 = %d, p2 = %d, lin_intrep_fac = %d", p1, p2, lin_interp_fac );
  }

  return temp_offset; 

}
/*----------------------------------------------------------------------------*/
/*!
   @brief
  This function returns the tx power compensation value to be applied for enhanced temperature compensation.

   @details
  This function returns the tx power compensation value to be applied for enhanced temperature compensation.

   @param 
  'rfm_dev' - rf device.
  'pwr_in_dbm' - desired output power
  'mod_type'  - modulation used
   @retval 
  'tx_pwr_offset' - Tx power compensation to be applied. 

*/
int16 rfgsm_core_get_enh_temp_comp_pwr_offset(rfm_device_enum_type rfm_dev, int16 scaled_value, uint16 pcl, int16 pwr_in_dbm, rfgsm_modulation_type mod_type)
{
  uint8  pcl_index = 0, pcl_count = 0, temp_bin=0;
  int16 p1_offset=0,   p2_offset=0;
  int32  tx_pwr_offset = 0;
  int16  *pcl_vs_pwr;
  
  rfcom_gsm_band_type    tx_band;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  gsm_tx_static_nv_type  *tx_static_nv_data_ptr  = NULL;
 
  /* Getting core handle and Tx nvs pointers and check that they are non-NULL.*/
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN( rfgsm_core_handle_ptr,"rfgsm_core_handle_ptr is NULL!", 0 );

  tx_static_nv_data_ptr  = rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr;
  tx_band = rfgsm_core_handle_ptr->tx_band;

  /*Get Temp_bin*/
  temp_bin=rfgsm_core_handle_ptr->temp_comp.temp_bin_16;

  /* get the mapping PCL to power in dBm value */
  if (mod_type == RF_MOD_GMSK)
  {
    pcl_vs_pwr = rfgsm_core_handle_ptr->rfgsm_gsm_pcl_vs_power;
  }
  else
  {
    pcl_vs_pwr = rfgsm_core_handle_ptr->rfgsm_edge_pcl_vs_power;
  }

  /* Find the PCL index */
  while(pcl_count < RFNV_NUMBER_OF_PCLS)
  {
    if ( pwr_in_dbm >= pcl_vs_pwr[pcl_count])
    {
      pcl_index = pcl_count;
    }
    else
    {
      break;
    }
    pcl_count++;
  }
  if ((tx_band == RFCOM_BAND_GSM900 || tx_band == RFCOM_BAND_GSM850) && (pcl_index == (RFNV_NUMBER_OF_PCLS-1)))  
  {
    pcl_index = RFNV_NUMBER_OF_PCLS-2; /* Because total number of PCLs for low bands is 15 */
  }  

   /* pcl bound check */
  if( ((tx_band == RFCOM_BAND_GSM900 || tx_band == RFCOM_BAND_GSM850) && (pcl_index >= (RFNV_NUMBER_OF_PCLS-1)))
  	|| ((tx_band == RFCOM_BAND_GSM1800 || tx_band == RFCOM_BAND_GSM1900) && (pcl_index >= RFNV_NUMBER_OF_PCLS)))
  {
    RFGC_MSG_1( MSG_LEGACY_ERROR," Incorrect pcl index(%d). Temp offset not applied!", pcl_index );
    return 0;
  }

  if (temp_bin > RFGSM_CORE_TEMP_COMP_16_BIN_SIZE - 1)
  {    
    RFGC_MSG_1( MSG_LEGACY_HIGH," Invalid temp_bin %d, and take the max temp_bin instead", temp_bin);
    temp_bin = RFGSM_CORE_TEMP_COMP_16_BIN_SIZE - 1;
  }

  /* no interpolation is needed at max temp_bin */  
  if(temp_bin == RFGSM_CORE_TEMP_COMP_16_BIN_SIZE-1) 
  {
    if (mod_type == RF_MOD_GMSK)
    {
      tx_pwr_offset = tx_static_nv_data_ptr->enh_temp_comp_data.tempcomp_table_gmsk[temp_bin][pcl_index];
    }
    else
    {
      tx_pwr_offset = tx_static_nv_data_ptr->enh_temp_comp_data.tempcomp_table_8psk[temp_bin][pcl_index];
    }
  }
  else  /* interpolation over temp_bin */
  {
    if (mod_type == RF_MOD_GMSK)
    {
      p1_offset =  tx_static_nv_data_ptr->enh_temp_comp_data.tempcomp_table_gmsk[temp_bin][pcl_index];
      p2_offset =  tx_static_nv_data_ptr->enh_temp_comp_data.tempcomp_table_gmsk[temp_bin+1][pcl_index];
    }
    else if (mod_type == RF_MOD_8PSK)
    {
      p1_offset =  tx_static_nv_data_ptr->enh_temp_comp_data.tempcomp_table_8psk[temp_bin][pcl_index];
      p2_offset =  tx_static_nv_data_ptr->enh_temp_comp_data.tempcomp_table_8psk[temp_bin+1][pcl_index];
    }
    else
    {
      RFGC_MSG_1( MSG_LEGACY_ERROR,"Enh Temp comp: Wrong modulation type. Default to GMSK. value = %d", mod_type );
      p1_offset =  tx_static_nv_data_ptr->enh_temp_comp_data.tempcomp_table_gmsk[temp_bin][pcl_index];
      p2_offset =  tx_static_nv_data_ptr->enh_temp_comp_data.tempcomp_table_gmsk[temp_bin+1][pcl_index];
    }
    /*linear regression to get the correct power offset to apply */
    tx_pwr_offset = (p1_offset*(100-scaled_value)+p2_offset*scaled_value)/100;	
  }
  if( rfgsm_core_handle_ptr->therm_read.enable_logging == TRUE )
  {
    RFGC_MSG_6( MSG_LEGACY_MED, "DBG: pcl_index = %d, Temp_bin = %d, p1_offset = %d, p2_offset = %d, scaled_value = %d, Enhanced Temp Comp power offset = %d",
                pcl_index, temp_bin, p1_offset, p2_offset,  scaled_value,  tx_pwr_offset );
  }

  return tx_pwr_offset; 
}

/*----------------------------------------------------------------------------*/


/*!
   @brief
   This function calculates the offset to the tx power based on the backoff parameters passed from GL1
   in units of dBm100.

   This function should be applied as a flat backoff to the power in dBm before any other modification
   (SAR limiting, temp comp, etcetera).

   This is a backoff and so we are expecting an absolute value which will be subtracted from the initial
   power. For example, if L1 want to reduce the power by 0.5 dBm then they will pass us a backoff value 
   of 50, which we will subtract from the power in dBm initially calculated from the passed PCL.

   @details
   We will apply the greatest backoff of the two values (L1 vbatt backoff vs
   L1 desense backoff)
  
   @param
    pwr_in_dbm
    coex_backoff_params
  
   @retval 
    coex_power_backoff

*/
int32 rfgsm_core_get_coex_backoff(int32 pwr_in_dbm, int16* backoff_params )
{
    int16 vbatt_droop_offset = 0;
    int16 desense_offset     = 0;
    int32 coex_power_backoff   = 0;

    if (backoff_params == NULL) {
        MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "NULL ptr passed as coex backoff parameters!");
        return 0;
    }

    vbatt_droop_offset = backoff_params[RFGSM_COEX_VBATT_BACKOFF];
    desense_offset = backoff_params[RFGSM_COEX_DESENSE_BACKOFF];

    /* Apply the greatest backoff out of vbatt_droop and desense */
    coex_power_backoff = MAX(vbatt_droop_offset, desense_offset);

    /* Return the coex_power_backoff */
    return coex_power_backoff;
}

/*----------------------------------------------------------------------------*/

/*!
   @brief
  This function returns the tx power compensation value to be applied based on the temp comp and
  vbatt compenasation.

   @details
  This function returns the tx power compensation value to be applied based on the temp comp and
  vbatt compenasation.

   @param 
  'band' - tx band for which tx power compensation to be calculated.
  ' mod_type' - GMSK or 8PSK
  'pcl'  - PCL value
  'pa_range' - PA range (gain state) in which tx compensation to be calculated.
   @retval 
  'tx_pwr_offset' - Tx power compesation to be applied. 

*/
int32 rfgsm_core_tx_pwr_offset_get(rfm_device_enum_type rfm_dev,
                                   rfcom_gsm_band_type band, 
                                   rfgsm_modulation_type mod_type, 
                                   uint16 pcl, uint8 pa_range, int32 pwr_in_dbm,
                                   int16* backoff_params ) 
{

  int32 temp_backoff=0; 
  int32 vbatt_comp = 0;
  int32 tx_pwr_offset = 0;
  int32 scaled_value = 0;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  rfgsm_nv_cmn_data_type *rfgsm_nv_cmn_tbl_ptr = NULL;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!", tx_pwr_offset);
  
  rfgsm_nv_cmn_tbl_ptr = rfgsm_nv_get_cmn_tbl_ptr(rfm_dev);

  RF_NULL_CHECK_RTN(rfgsm_nv_cmn_tbl_ptr, "rfgsm_nv_cmn_tbl_ptr is NULL!", tx_pwr_offset);

      /****************************  COMPENSATIONS IN LINEAR ***************************************************
      *  NOTE:- ALL TEMP and VBATT compensations are only applied to AMSS (not to FTM mode)                    *
      *                                                                                                        *
      *  1), 2), 3) Based on band, modulation, hot and cold temp the appropriate NV is chosen to compute the temp  *                                                                                                    *
      *  offset to be computed (in dbx100). the nv is multiplied with the scaling factor computed in           *                                                                                                 *
      *  rfgsm_core_temp_comp_tx_calc() function.                                                              * 
      *                                                                                                        *       *
      *                                                                                                        *
      *  4) Vbatt Compensation (MAx power only) (Legacy Vbatt comp) :- This vbatt comp has been in existence   *
      *     for many targets and is used to backoff max power only.                                            *
      *                                                                                                        *
      *  5) Vbatt Compensation vs. Pa Gain range :- This compensation is applied to all the                    *
      *     gain ranges (and all PCLS). The compensation value could be different for each gain range          * 

      **********************************************************************************************************/


  if ( ((band == RFCOM_BAND_GSM900 || band==RFCOM_BAND_GSM850) && (pcl == 14)) ||
       ((band == RFCOM_BAND_GSM1800 || band == RFCOM_BAND_GSM1900) && (pcl == 15)) )
   {
        /* Vbatt backoff only for max PCL */
     vbatt_comp = rfgsm_core_vbatt_comp_pwr_get(rfm_dev);

   }

  /* Clear temp backoff before calculating for each slot */
  temp_backoff = 0;

  /* Temp Comp 1 from Note above. */
  scaled_value = rfgsm_core_temp_comp_pwr_get(rfm_dev); 
  
 if (rfgsm_nv_cmn_tbl_ptr->enh_tempcomp_enable_nv == TRUE)
  {
    /*get power offset to apply in enhanced temperature compensation mode.*/
    temp_backoff = rfgsm_core_get_enh_temp_comp_pwr_offset(rfm_dev, scaled_value, pcl, pwr_in_dbm, mod_type);
  }
  else
  {
    /*
    Get the temp offset based on the pwr_in_dbm instead of PCL.
    Reason: If the multislot NVs are populated the actual pwr_in_dbm != nominal PCL power
    Thus, we need to apply the temp offset by looking at the pwr_in_dbm and not PCL.
    */
    temp_backoff = rfgsm_core_get_temp_comp_pwr_offset(rfm_dev, pwr_in_dbm, scaled_value, mod_type);
  }
 
  rfgsm_core_handle_ptr->tx_log.temp_comp_backoff = temp_backoff;

  /* apply modulation based temp comp */
  tx_pwr_offset += temp_backoff;

  /*Only Apply Vbatt_Comp if APT is disabled, i.e. in BYPASS or INVALID MODE.*/
  if ( rfgsm_core_handle_ptr->apt_mode == BYPASS_MODE || 
       rfgsm_core_handle_ptr->apt_mode == INVALID_MODE )
   {
     //vbatt_comp += rfgsm_core_handle_ptr->vbatt_comp_info.vbatt_comp_parange_dBx100[pa_range];
     tx_pwr_offset -= vbatt_comp; 
   }
  
  if( rfgsm_core_handle_ptr->vbatt_comp_info.enable_logging == TRUE )
  {
    RFGC_MSG_2( MSG_LEGACY_MED, "GSM VBatt Comp: vbatt comp: %d, Total vbatt and temp comp:  %d ", vbatt_comp, tx_pwr_offset );
  }

  return tx_pwr_offset;
}


/*----------------------------------------------------------------------------*/
boolean rfgsm_core_get_enh_apt_bias( rfm_device_enum_type rfm_dev, 
                                      uint16* bias, 
									  apt_mode_type* mode, 
									  uint16 pcl, 
									  rfgsm_modulation_type mod_type, 
									  uint8 num_slots )
{
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  /* hysteresis in the BYPASS/FORCED APT switch (mV).*/
  uint8 hysteresis = 50;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!", FALSE);

  /*Read Current APT Mode.*/
  *mode = rfgsm_core_handle_ptr->apt_mode;

  if (pcl >= rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr->enh_apt_data.pcl_threshold)
  {
    if( ((*mode == FORCED_APT_MODE) || (*mode == REGULAR_APT_MODE)) &&
        (rfgsm_core_handle_ptr->vbatt_comp_info.vbatt_value_mv <= rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr->enh_apt_data.vbatt_thresold) )
    {
      *mode = BYPASS_MODE;
      
    }
    else if ( ((*mode == BYPASS_MODE) || (*mode == INVALID_MODE)) &&
        (rfgsm_core_handle_ptr->vbatt_comp_info.vbatt_value_mv >= rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr->enh_apt_data.vbatt_thresold+hysteresis) )
    {
      *mode = FORCED_APT_MODE;
      
    }
  }
  else
  {
    if( ((*mode == FORCED_APT_MODE) || (*mode == REGULAR_APT_MODE)) &&
        (rfgsm_core_handle_ptr->vbatt_comp_info.vbatt_value_mv <= rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr->enh_apt_data.vbatt_thres_low_pwr) )
    {
      *mode = BYPASS_MODE;
    }
    else if ( ((*mode == BYPASS_MODE) || (*mode == INVALID_MODE)) &&
        (rfgsm_core_handle_ptr->vbatt_comp_info.vbatt_value_mv >= rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr->enh_apt_data.vbatt_thres_low_pwr+hysteresis) )
    {
      *mode = FORCED_APT_MODE;
    }
  }

  /* FOR FORCED APT or REGULAR APT mode or Bypass mode*/

  if (pcl < RFNV_NUMBER_OF_PCLS)
  {
    if (mod_type == RF_MOD_8PSK)
    {
      *bias = rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr->smps_pdm_tbl.edge_smps_pdm_tbl[pcl];
    }
    else
    {
      *bias = rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr->smps_pdm_tbl.gsm_smps_pdm_tbl[pcl];
    }

    // Keep legacy way of picking the higher SMPS setting for single slot Tx or > 1 Tx slot w/o PAPM multislot control feature
    if (!rfc_gsm_get_papm_multislot_ctl_support(rfm_dev, rfgsm_core_handle_ptr->tx_band) && num_slots > 1)
    {
      if ( rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr->smps_pdm_tbl.gsm_smps_pdm_tbl[pcl] >= rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr->smps_pdm_tbl.edge_smps_pdm_tbl[pcl])
      {             
        *bias = rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr->smps_pdm_tbl.gsm_smps_pdm_tbl[pcl];
      }
      else
      {
        *bias = rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr->smps_pdm_tbl.edge_smps_pdm_tbl[pcl];
      }
    }
  }
  else
  {
    *bias = rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr->smps_pdm_tbl.gsm_smps_pdm_tbl[15];
  }

  return TRUE;

}

/*----------------------------------------------------------------------------*/

/*!
   @brief
  This function sets up all TX power parameters for both linear and polar
  TX case.

   @details

   @param 

   @retval 
  rfgsm_linear_pa_r0_r1_data_type

*/
#define RFGSM_CORE_TX_EDGE_PAP                    330  /* 3.3 dB EDGE Peak to Average Power.*/
#define RFGSM_CORE_TX_COMP_MARGIN                 30   /* 0.3 dB is recommended to avoid any baseband signal clipping that may result from computation residual errors */
void rfgsm_core_gsmctl_set_tx_power( rfm_device_enum_type rfm_dev, 
                                     uint8 sub_id,
                                     rfgsm_power_profile_type pwr_profile[5],
                                     uint8 num_tx_slots,
                                     uint8 num_assigned_uplink_slots )
{
  int32 pwr_in_dbm = 0;
  int32 pwr_in_dbm_adjusted = 0;
  int32 tx_pwr_error = 0;
  int32 tx_pwr_offset = 0;
  int16 multislot_max_pwr_dbm = 0;
  int16 multislot_max_pwr_dbm_adjusted = 0;
  int32 env_gain_delta = 0;
  uint8 uplink_slots = 0;
  uint8 slot_count = 0;
  uint16 swpt_count = 0;
  uint16 edge_env_gain = 0;
  rfnv_tx_gain_range_info_type* tx_swpts; 
  rfgsm_modulation_type mod_type;
  uint16 pcl = 0;
  uint16 max_pcl_index = 0;
  uint16 pcl_index = 0;
  uint16 bias[RFGSM_MAX_TX_SLOTS_PER_FRAME];
  boolean bias_flag = FALSE;
  uint16 pa_scale = 0;
  uint16 predist_flag = 0;
  uint16 amam_tbl_bank = 0;
  int32 maxpwr;
  apt_mode_type mode[RFGSM_MAX_TX_SLOTS_PER_FRAME];
  int32 set_pwr_limit;
  uint8 pa_range;
  rfgsm_core_ftm_slot_override_type *ftm_temp;
  gsm_tx_static_nv_type *tx_static_nv_data_ptr = NULL;
  gsm_tx_cal_data_nv_type *tx_cal_nv_data_ptr = NULL;
  rfc_gsm_core_config_type *rfc_gsm_core_ptr = NULL;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  rfgsm_modulation_type mod_for_bias = RF_MOD_UNKNOWN;
  uint8 ftm_num_slots = 0;
  uint8 slot_index = 0; //default slot index where the max power has been found
  uint8 cnt = 0;
  boolean override_start_time = FALSE; // for online mode, do not override RFC timing when not performing Tx calibration

  uint8 rgi_log=0xFF;
  uint16 char_offset_bin_indx =0;

  uint32 dm1_coarse_tau = 0xFFFFFFFF;
  uint32 dm2_fine_tau = 0xFFFFFFFF;
  uint32 dm3_frac_tau = 0xFFFFFFFF;
  boolean enable_logging;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  
  RF_NULL_CHECK_RTN_V( rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!" );

  enable_logging = rfgsm_core_handle_ptr->rfgsm_enable_pcl_logging;

  rfc_gsm_core_ptr = rfgsm_core_handle_ptr->rfc_gsm_core_ptr;

  RF_NULL_CHECK_RTN_V( rfc_gsm_core_ptr, "gsm rfc_ptr is NULL! Exiting TX power setting...");

  /* pointers to Tx NVs*/
  tx_static_nv_data_ptr  = rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr;
  RF_NULL_CHECK_RTN_V( tx_static_nv_data_ptr, "tx_static_nv_data_ptr is NULL! Exiting TX power setting...");
  tx_cal_nv_data_ptr = &(rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_cal_nv_data);
  RF_NULL_CHECK_RTN_V( tx_cal_nv_data_ptr, "tx_cal_nv_data_ptr is NULL! Exiting TX power setting...");

  if (num_tx_slots > RFGSM_MAX_TX_SLOTS_PER_FRAME)
  {
    RFGC_MSG_2( MSG_LEGACY_ERROR,"Incorrect number of tx slots %d, max is %d ", num_tx_slots, RFGSM_MAX_TX_SLOTS_PER_FRAME);
    return;
  }

  if(ftm_gsm_cal_mode_enabled(rfm_dev)) /* Check if in cal or normal FTM mode (aka FTM_STATE_GSM) */
  {
    /* note: this first loop only purpose, is to derive "ftm_num_slots/max_pcl_index",
     *  which is used in rfgsm_core_get_enh_apt_bias()
     */
    maxpwr = -1;
    for (slot_count = 0; slot_count < RFGSM_MAX_TX_SLOTS_PER_FRAME; slot_count++)
    {
      ftm_temp = &rfgsm_core_handle_ptr->ftm_slot_override[slot_count];
      
      if (ftm_temp->tx_pow_dbm != RF_INVALID_VALUE_16BIT)
      {
        /* predict slot index of max requested power across slots */
        if(maxpwr < ftm_temp->tx_pow_dbm)
        {
          slot_index = slot_count;
          maxpwr = ftm_temp->tx_pow_dbm;
        }
        
        ftm_num_slots++;
      }
      else if ((slot_count < num_tx_slots) &&
               (maxpwr < rfgsm_core_handle_ptr->rfgsm_core_tx_pow_mod[slot_count].power_level) &&
               (rfgsm_core_handle_ptr->slot_override_flag == FALSE))
      {
        /* likely no ftm_slot_override[] have been set on this frame
         * but we have been triggered to re-adjust TX AGC
         */
        ftm_temp->tx_pow_dbm = rfgsm_core_handle_ptr->rfgsm_core_tx_pow_mod[slot_count].power_level;
        ftm_temp->modulation = rfgsm_core_handle_ptr->rfgsm_core_tx_pow_mod[slot_count].modulation;
        
        slot_index = slot_count;
        maxpwr = ftm_temp->tx_pow_dbm;
        ftm_num_slots++;        
      }
    }
    
    /* Get the corresponding PCl of the max TX power in a frame*/
    if (ftm_num_slots)
    {
      ftm_temp = &rfgsm_core_handle_ptr->ftm_slot_override[slot_index];
      max_pcl_index = rfgsm_core_tx_get_closest_pcl_index_vs_power(rfm_dev, ftm_temp->tx_pow_dbm);
    }
    else
    {
      RFGC_MSG(MSG_LEGACY_ERROR,
               "rfgsm_core_gsmctl_set_tx_power:"
               ":could not predict max_pcl_index with no ftm_slot_override[].tx_pow_dbm");      
    }
    
    /* FTM has updated the tx power in dBm, then */
    for (slot_count = 0; slot_count < RFGSM_MAX_TX_SLOTS_PER_FRAME; slot_count++)
    {
      /* Reset bias flag */
      bias_flag = FALSE;

      //Reset APT bias and mode
      bias[slot_count] = 0xFFFF;
      mode[slot_count] = INVALID_MODE;

      /* Use pointer to avoid copying overhead */
      ftm_temp = &rfgsm_core_handle_ptr->ftm_slot_override[slot_count];
      
      /* Get the corresponding PCL of the TX power in a frame */
      pcl_index = rfgsm_core_tx_get_closest_pcl_index_vs_power(rfm_dev, ftm_temp->tx_pow_dbm);

      if (ftm_temp->linear_rgi != 0xFFFF)
      {
         /* If sweep calibration is running, RGI based setting is used for 
            characterized DPD cal, and in that case WTR RGI LUTs should be 
            always forced to be 8PSK LUT */
        if (ftm_gsm_get_frame_sweep_cal_running(rfm_dev) == TRUE)
        {
          rfdevice_gsm_set_rgi(rfm_dev, ftm_temp->linear_rgi,
                               RF_MOD_8PSK,
                               slot_count,
                               TRUE);
        }
        else
        {
           rfdevice_gsm_set_rgi(rfm_dev, ftm_temp->linear_rgi,
                                ftm_temp->modulation,
                                slot_count,
                                TRUE);
        }
        rfgsm_core_handle_ptr->rgi_override_val = ftm_temp->linear_rgi;
        rfgsm_core_handle_ptr->rfc_gsmctl_linear_pa_tx_gain_buff[slot_count].txCfg2GainSetupPaScale = ftm_temp->pa_scaling;
        rfgsm_core_handle_ptr->rfc_gsmctl_linear_pa_tx_gain_buff[slot_count].txCfg2GainSetupEnvGain = ftm_temp->env_gain;

        RFGC_MSG_4( MSG_LEGACY_HIGH, "Overriding Linear RGI =%d, env_gain=%d, pa_scale=%d in tx slot %d",
                                      ftm_temp->linear_rgi,ftm_temp->env_gain,ftm_temp->pa_scaling, slot_count );

        /* Scale the PA ramp according to the PA scale value */
        if( !rfgsm_core_scale_tx_pa_ramp_data( rfm_dev,
                                               sub_id,
                                               slot_count,
                                               ftm_temp->pa_scaling ) )
        {
          RFGC_MSG(MSG_LEGACY_ERROR, "RF GSM prog FTM Tx ramp scaling to core DM failed!");
          return;
        }

        /*Update RGI value in the Tx log structure*/
        rfgsm_core_handle_ptr->tx_log.tx_slot_log[slot_count].rgi = ftm_temp->linear_rgi;

        /* update current_rgi for control of pa_icq_bias with NV*/
        rfgsm_core_handle_ptr->current_rgi = ftm_temp->linear_rgi;
        ftm_temp->linear_rgi = 0xFFFF;
      }
      if ( ftm_temp->pa_range != 0xFFFF )
      {
        rfgsm_core_handle_ptr->parange_override_val = ftm_temp->pa_range;

        mod_for_bias = ftm_temp->modulation;

        if (rfgsm_core_handle_ptr->rfgsm_core_ftm_bias_override_flag == 0 )
        {
          // if multislot APT feature is disabled use max pcl to obtain the APT bias
          if (!rfc_gsm_get_papm_multislot_ctl_support(rfm_dev, rfgsm_core_handle_ptr->tx_band))
          {
            if ((ftm_num_slots == 0) && (max_pcl_index < pcl_index))
            {
              RFGC_MSG_1(MSG_LEGACY_ERROR, "rfgsm_core_gsmctl_set_tx_power:no ftm slots, max_pcl_index=%d is set empirically",pcl_index);
              max_pcl_index = pcl_index;
            }
            else
            {
              pcl_index = max_pcl_index;
            }
          }

          // Get the mode of QPOET
          (void)rfgsm_core_get_enh_apt_bias(rfm_dev, &bias[slot_count], &mode[slot_count], pcl_index, mod_for_bias, ftm_num_slots );  

            //Check if bias is zero
            if (bias[slot_count] <= 0)
            {
               /* if bias is zero, use bias value from TX cal bias table*/
               if (rfgsm_core_handle_ptr->parange_override_val < RFCOM_GSM_NUM_PA_STATE)
               {
               bias[slot_count] = tx_static_nv_data_ptr->smps_pdm_tbl.cal_smps_pdm_tbl[ rfgsm_core_handle_ptr->parange_override_val];
               }
            }
          }
          else
          {
            bias[slot_count] = ftm_temp->smps_bias;
            mode[slot_count] = FORCED_APT_MODE;
          }

        if (bias[slot_count] <= 0)
          {
            /*default values*/
          mode[slot_count] = BYPASS_MODE;
          bias[slot_count] = 3700;
          }

        if (( (RF_HW_WTR2965_NON_CA_4373_V2_SAW == (rf_hw_type)rfcommon_nv_get_rf_config())) ||
                (RF_HW_WTR2965_NON_CA_4373_V2 == (rf_hw_type)rfcommon_nv_get_rf_config()) )
        {
          if((rfgsm_core_handle_ptr->tx_log.tx_slot_log[slot_count].apt_mode == mode[slot_count])&&
              (rfgsm_core_handle_ptr->tx_log.tx_slot_log[slot_count].smps_pdm == bias[slot_count]))
            {
               bias_flag = FALSE;
            }
          else
            {
               bias_flag = TRUE;   //Set QPOET bias
            }
        }
        else
        {
          bias_flag = TRUE;    //Set QPOET bias irrespective of the RF card type
        }

        rfc_gsm_tx_gain_config( rfm_dev, rfgsm_core_handle_ptr->tx_band,
                                ftm_temp->modulation, slot_count, ftm_temp->tx_pow_dbm, rfgsm_core_handle_ptr->parange_override_val,
                                rfgsm_core_handle_ptr->scripts.tx_trans_script[TX_TRANS_FE_SCRIPT][slot_count], 
                                bias_flag, bias[slot_count], mode[slot_count], override_start_time);

        rfgsm_core_handle_ptr->tx_log.tx_slot_log[slot_count].smps_pdm = bias[slot_count];
        rfgsm_core_handle_ptr->tx_log.tx_slot_log[slot_count].apt_mode = mode[slot_count];

        /*Update Current APT Mode with last Tx slot's APT mode */ 
        rfgsm_core_handle_ptr->apt_mode = mode[slot_count];

        RFGC_MSG_2( MSG_LEGACY_HIGH, "Overriding pa_range to %d in tx slot %d", rfgsm_core_handle_ptr->parange_override_val, slot_count );
        rfgsm_core_handle_ptr->tx_log.tx_slot_log[slot_count].pa_range = rfgsm_core_handle_ptr->parange_override_val;
        rfgsm_core_handle_ptr->tx_log.tx_slot_log[slot_count].mod_type = ftm_temp->modulation;
        ftm_temp->pa_range = 0xFFFF;
      }

      if (ftm_temp->tx_pow_dbm != 0xFFFF)
      {
        rfgsm_core_handle_ptr->rfgsm_core_tx_pow_mod[slot_count].power_level = ftm_temp->tx_pow_dbm;

        if ((slot_count < num_tx_slots) && (rfgsm_core_handle_ptr->parange_override_val == 0xff))
        {
          rfgsm_core_handle_ptr->rfgsm_core_tx_pow_mod[slot_count].modulation = pwr_profile[slot_count].modulation;
        }
        else
        {
          rfgsm_core_handle_ptr->rfgsm_core_tx_pow_mod[slot_count].modulation = ftm_temp->modulation;
        }
        
        RFGC_MSG_4(MSG_LEGACY_HIGH, "Updating tx pow to power_level=%d _dBm100, modulation=%d, pcl_index=%d, in tx slot %d",
                   rfgsm_core_handle_ptr->rfgsm_core_tx_pow_mod[slot_count].power_level,
                   rfgsm_core_handle_ptr->rfgsm_core_tx_pow_mod[slot_count].modulation,
                   pcl_index, slot_count);

        rfgsm_core_handle_ptr->tx_log.tx_slot_log[slot_count].pwr_in_dbm = ftm_temp->tx_pow_dbm;

        /*Storing Tx Power info for Closed Loop Tuner*/
        rfgsm_core_handle_ptr->rfc_gsmctl_linear_pa_tx_gain_buff[slot_count].txPower = ftm_temp->tx_pow_dbm;        

        ftm_temp->tx_pow_dbm = 0xFFFF;
      }
      /*! @todo Add additional code to update from other parameters in rfgsm_core_handle_ptr->ftm_slot_override here*/
    } /* for (slot_count = 0; slot_count < RFGSM_MAX_TX_SLOTS_PER_FRAME; slot_count++) */

    if (rfgsm_core_handle_ptr->ftm_path_delay_override_val != 0xFFFF)
    { 
      /*Write to scaled delay value*/
      rfgsm_core_dm_set_tx_path_delay( RFLM_GSM_DEFAULT_SUBSCRIPTION,
                                       rfgsm_core_handle_ptr->buffer_id, 
                                       (int64)rfgsm_core_handle_ptr->ftm_path_delay_override_val );

      RFGC_MSG_1( MSG_LEGACY_HIGH, "Path Delay set to %d", rfgsm_core_handle_ptr->ftm_path_delay_override_val );
      rfgsm_core_handle_ptr->tx_log.delay_val = rfgsm_core_handle_ptr->ftm_path_delay_override_val;
    }
  }
  else /* SIG, NON-SIG sate */
  {
    for (slot_count=0; slot_count < num_tx_slots; slot_count++)
    {
      pcl = pwr_profile[slot_count].power_level;

      if (pcl > max_pcl_index)
      {
        max_pcl_index = pcl;

        //Store the slot for which pwr index is the max in frame
        slot_index = slot_count;
      }
    }

    mod_for_bias = pwr_profile[slot_index].modulation; 
    

  }

  if (rfgsm_core_handle_ptr->slot_override_flag == TRUE)
  {
    /* Slot override is enabled so all Tx parameters are set, return*/
    return;   
  }

  /* Use max of assigned / actual UL TX slots to calculate back off */
  uplink_slots= MAX( num_tx_slots, num_assigned_uplink_slots);
  if (uplink_slots <= 0)
  {
    RFGC_MSG_1( MSG_LEGACY_ERROR,"Incorrect number of uplink_slots %d ", uplink_slots );
    return;
  }

  rfgsm_core_handle_ptr->tx_log.num_slots = num_tx_slots;

  /* Check whether linear or not and then take appropriate step: */
  for (slot_count=0; slot_count < num_tx_slots; slot_count++)
  {
    /* Reset bias flag */
    bias_flag = FALSE;

    /* Reset APT bias and mode */
    bias[slot_count] = 0xFFFF;
    mode[slot_count] = INVALID_MODE;

    /* Modulation, PCL and Power requests consistancy */
    if (ftm_gsm_cal_mode_enabled(rfm_dev) == FALSE) /* in SIG/FTM_STATE_GSM_NS states */
    {
      mod_type = pwr_profile[slot_count].modulation;

      pcl = pwr_profile[slot_count].power_level;

      if (pcl >= RFNV_NUMBER_OF_PCLS)
      {
        RFGC_MSG_1(MSG_LEGACY_ERROR, "rfgsm_core_gsmctl_set_tx_power::pcl_index %d not valid", pcl);
        pcl = RFNV_NUMBER_OF_PCLS - 1;
      }
      
      pwr_in_dbm = tx_static_nv_data_ptr->power_levels[pcl];
    }
    else /* in FTM_STATE_GSM */
    {
      /* in FTM the pcl is the slot index, it is NOT the actual PCL value. so, the
       * rfgsm_mdsp_tx_pow_mod[] contains the power in dBm to tx for each uplink slots
       */
      mod_type = rfgsm_core_handle_ptr->rfgsm_core_tx_pow_mod[slot_count].modulation;

      if (rfgsm_core_handle_ptr->rfgsm_core_tx_pow_mod[slot_count].modulation != pwr_profile[slot_count].modulation)
      {
        RFGC_MSG_5(MSG_LEGACY_ERROR,
                   "rfgsm_core_gsmctl_set_tx_power:"
                   ":FTM modulation compromised pwr_profile[%d].modulation=%d,"
                   " rfgsm_core_tx_pow_mod[%d].modulation=%d, mode_for_bias=%d",
                   slot_count,
                   pwr_profile[slot_count].modulation,
                   slot_count,
                   rfgsm_core_handle_ptr->rfgsm_core_tx_pow_mod[slot_count].modulation,
                   mod_for_bias);
      }
      
      pwr_in_dbm = rfgsm_core_handle_ptr->rfgsm_core_tx_pow_mod[slot_count].power_level;
   
      pcl = rfgsm_core_tx_get_closest_pcl_index_vs_power(rfm_dev, (int16)pwr_in_dbm);

      if ((ftm_num_slots == 0) && (max_pcl_index < pcl))
      {
        /* in FTM_STATE_GSM none of the ftm_slot_override[].pwr_in_dbm are valid
         * then this empirical max_pcl_index is picked.
         */
        RFGC_MSG_1(MSG_LEGACY_ERROR,
                   "rfgsm_core_gsmctl_set_tx_power:"
                   ":no ftm slots for max_pcl_index=%d is set empirically", pcl);
        max_pcl_index = pcl;
      }

    } /* end FTM_STATE_GSM */

    /* get the PCL to power in dBm mapping value and limits adjustement */
    switch(mod_type)
    {
      case RF_MOD_8PSK:
        {
          multislot_max_pwr_dbm = tx_static_nv_data_ptr->multislot_max_tx_power.gsm_multislot_tx_pwr_8psk[uplink_slots - 1];
          multislot_max_pwr_dbm_adjusted = rfgsm_core_handle_ptr->rfgsm_edge_multislot_tx_pwr_limit[uplink_slots - 1];
          pwr_in_dbm_adjusted = rfgsm_core_handle_ptr->rfgsm_edge_pcl_vs_power[pcl];          
        }
        break;

      default:
        {
          /* Check if modulation type is valid, if not force to default mod type */
          if (mod_type >= RF_MOD_UNKNOWN)
          {
            RFGC_MSG_1(MSG_LEGACY_HIGH, "Modulation type %d is not valid", mod_type);
            mod_type = RF_MOD_GMSK;
          }
        } /* break is omitted here, because mod_type is unsigned and if not 8PSK, then GMSK will be our best option. */
      case RF_MOD_GMSK:
        {
          multislot_max_pwr_dbm = tx_static_nv_data_ptr->multislot_max_tx_power.gsm_multislot_tx_pwr_gmsk[uplink_slots - 1];
          multislot_max_pwr_dbm_adjusted = rfgsm_core_handle_ptr->rfgsm_gsm_multislot_tx_pwr_limit[uplink_slots - 1];
          pwr_in_dbm_adjusted =  rfgsm_core_handle_ptr->rfgsm_gsm_pcl_vs_power[pcl];
        }
        break;
    }
    
    /* make sure that the power in dBm doesn't exceed the max pwr allowed */
    if ((pwr_in_dbm_adjusted <= multislot_max_pwr_dbm_adjusted) ||
        (ftm_gsm_cal_mode_enabled(rfm_dev) == TRUE))
    {
      /* incorporate also this PCL error (tx_pwr_error) in FTM mode.
       * note that during calibration the error is 0.
       */
      tx_pwr_error = tx_static_nv_data_ptr->power_levels[pcl] - pwr_in_dbm_adjusted;

      if (enable_logging)
      {
        RFGC_MSG_3(MSG_LEGACY_MED,
                   "rfgsm_core_gsmctl_set_tx_power::pwr(dBm100)=%d p_err[%d]=%d",
                   pwr_in_dbm , pcl, tx_pwr_error);
      }
    }
    else
    {
      tx_pwr_error = multislot_max_pwr_dbm - multislot_max_pwr_dbm_adjusted;

      pwr_in_dbm = multislot_max_pwr_dbm;

      if (enable_logging)
      {
        RFGC_MSG_3(MSG_LEGACY_MED,
                   "rfgsm_core_tx_get_pcl_power_error:"             
                   ":multislot limited max pwr(dBm100)=%d, p_err[%d]=%d",
                   pwr_in_dbm , pcl, tx_pwr_error);
      }
    }
        
    /* Subtract the coex_backoff (if any) from the calculated power before anything else happens */
    pwr_in_dbm -= rfgsm_core_get_coex_backoff( pwr_in_dbm, pwr_profile[slot_count].backoff );

    /* apply SAR limiting */
    pwr_in_dbm = rfgsm_core_tx_pwr_apply_sar_limiting(rfm_dev, pwr_in_dbm, mod_type, uplink_slots);
    rfgsm_core_handle_ptr->tx_log.tx_slot_log[slot_count].pwr_in_dbm = pwr_in_dbm;

    /* derive AMAM table, Pre-distortion Flags, PA range, from Switch Points  */
    tx_swpts = rfgsm_nv_get_tx_gain_info_ptr(rfm_dev, rfgsm_core_handle_ptr->tx_band, mod_type);

    if (tx_swpts == NULL)
    {
      RFGC_MSG_1( MSG_LEGACY_ERROR,"tx_swpts in NULL for band = %d ", rfgsm_core_handle_ptr->tx_band );
      return;
    }

    /* Go over all swithcpoints and find the switchpoint we need to use */
    for ( swpt_count = 0 ; swpt_count < tx_swpts->num_gain_ranges; swpt_count ++ )
    {
      if ( pwr_in_dbm < tx_swpts->switch_point[swpt_count] )
      {
        /* found the switch point we need to use */
        break;
      }
    }

    predist_flag = tx_swpts->predistortion[swpt_count];
    amam_tbl_bank = tx_swpts->amam_tbl_bank[swpt_count];

    if(ftm_gsm_get_predist_override(rfm_dev) && (predist_flag != 0))
    {
      /*override predist_flag if predist_override is set*/
      predist_flag = 0;
      RFGC_MSG( MSG_LEGACY_HIGH, "disabling predistortion in FTM cal mode" );
    }
    else if ( (rfgsm_debug_flags.predist_override == TRUE) && (predist_flag != 0) )
    {
      /* Disable pre-distortion if predist_override debug flag is set*/
      predist_flag = 0;
      RFGC_MSG(MSG_LEGACY_ERROR, "Predistortion Disabled by override debug flag");
    }

    pa_range = (uint8)tx_swpts->pa_state[swpt_count];

    /* tx_pwr_offset = temp_comp + vbatt comp based on pcl, mod_type and pa_range, or coex backoff from Gl1,
       - depending which gives the lower tx power */ 
    if (
        ((ftm_gsm_cal_mode_enabled(rfm_dev) == TRUE) &&
         (rfc_gsm_core_ptr->ftm_temp_comp_enable_flag == FALSE)) ||
        (ftm_gsm_get_frame_sweep_cal_running(rfm_dev) == TRUE)
        )
    {
      tx_pwr_offset = 0; // no tx offset applied.
    }
    else
    {
      /* Get a tx power offset based on vbatt/temp comp and GL1 coex backoff */
      tx_pwr_offset = rfgsm_core_tx_pwr_offset_get(rfm_dev, rfgsm_core_handle_ptr->tx_band, mod_type, pcl,pa_range, pwr_in_dbm, pwr_profile[slot_count].backoff);
      rfgsm_core_handle_ptr->tx_log.tx_slot_log[slot_count].tx_pwr_offset = tx_pwr_offset;
    }

    /*Add characterized offset due to filtered path*/

    if (rfgsm_core_handle_ptr->use_coex_filtered_path_tx == TRUE)
    {
      char_offset_bin_indx = rfgsm_core_get_arfcn_bin_index_char_offset_tbl(rfm_dev, rfgsm_core_handle_ptr->tx_band, rfgsm_core_handle_ptr->rf_tx_arfcn,RFC_CONFIG_TX );
      if (char_offset_bin_indx < RFNV_MAX_ARFCN_BINS)
      {
        pwr_in_dbm += tx_static_nv_data_ptr->coex_tx_char_offset_info_tbl.tx_char_offset_data_tbl[char_offset_bin_indx];
      }

    }


    if (! predist_flag ) /* pre-distortion OFF */
    {
      pwr_in_dbm += (tx_pwr_offset - tx_pwr_error);

      pa_scale = RFGSM_MSM_PA_SCALE_UNITY_GAIN; 
      edge_env_gain = rfgsm_core_set_tx_agc( rfm_dev, pwr_in_dbm, pa_range, slot_count, mod_type, &rgi_log); 
    }
    else /* pre-distortion ON */
    {
      edge_env_gain = RFGSM_MSM_EDGE_ENVELOPE_UNITY_GAIN;

      /* Pa scale for max power is the first index. From there on for every .25 dB delta from max
         power we have to go down one index in the pa scale table. */
      /* maxpwr is the limit for nominal power (pwr_in_dbm) + compensation */
      maxpwr = tx_cal_nv_data_ptr->amam_max_dbm
               + rfgsm_core_temp_comp_get_bb_headroom(rfm_dev, rfgsm_core_handle_ptr->tx_band)
               - RFGSM_CORE_TX_COMP_MARGIN
               - RFGSM_CORE_TX_EDGE_PAP;

      /* This is the limit for the nominal power value (pwr_in_dBm)
       * note also tx_power_err compensation is already included
       * in the amam_max_dbm so will not appear in the envelope gain.
       */
      set_pwr_limit = tx_cal_nv_data_ptr->amam_max_dbm - RFGSM_CORE_TX_EDGE_PAP;

      /* Sanity check nominal power limit */
      if (pwr_in_dbm > set_pwr_limit )
      {
        pwr_in_dbm = set_pwr_limit;
      }

      /* Sanity check max compensation limit */
      env_gain_delta = 0;
      if ((pwr_in_dbm + tx_pwr_offset) > maxpwr )
      {
        env_gain_delta = maxpwr - pwr_in_dbm;
      }
      else
      {
        env_gain_delta = tx_pwr_offset;
      }
      edge_env_gain = (uint16)rfgsm_msm_get_env_backedoff_value(mod_type, (-(env_gain_delta)),edge_env_gain);

      pa_scale = rfgsm_core_gsmctl_pa_scale_int[((uint8)((set_pwr_limit - pwr_in_dbm)/25))];

      /* Set RGI from NV for predistortion case. */
      rfdevice_gsm_set_rgi(rfm_dev, tx_cal_nv_data_ptr->rgi_for_pred, mod_type, slot_count, TRUE);

      rgi_log = tx_cal_nv_data_ptr->rgi_for_pred;

    } /* pre-distortion ON */

    /* update current_rgi for control of pa_icq_bias with NV*/
    rfgsm_core_handle_ptr->current_rgi = rgi_log;

    /* populate the mdsp tx config buffer for each slot */
    rfgsm_core_handle_ptr->rfc_gsmctl_linear_pa_tx_gain_buff[slot_count].txCfg2GainSetupPredist = predist_flag;
    rfgsm_core_handle_ptr->rfc_gsmctl_linear_pa_tx_gain_buff[slot_count].txCfg2GainSetupAmamTblIdx = amam_tbl_bank;
    rfgsm_core_handle_ptr->rfc_gsmctl_linear_pa_tx_gain_buff[slot_count].txCfg2GainSetupPaScale = pa_scale;
    rfgsm_core_handle_ptr->rfc_gsmctl_linear_pa_tx_gain_buff[slot_count].txCfg2GainSetupEnvGain = edge_env_gain;    
    rfgsm_core_handle_ptr->rfc_gsmctl_linear_pa_tx_gain_buff[slot_count].txPower = pwr_in_dbm;    

    if( !rfgsm_core_scale_tx_pa_ramp_data( rfm_dev,
                                           sub_id,
                                           slot_count,
                                           pa_scale ) )
    {
      RFGC_MSG(MSG_LEGACY_ERROR, "RF GSM prog Tx ramp scaling to core DM failed!");
      return;
    }

    // make sure that the parange_override_val value is not set; if it is set then the pa range 
    // value is controlled by FTM.  So, don't use the switch point settings to change the pa range.

    if (rfgsm_core_handle_ptr->parange_override_val == 0xFF)
    {
      // if multislot APT feature is disabled use max pcl to obtain the APT bias
      if (!rfc_gsm_get_papm_multislot_ctl_support(rfm_dev, rfgsm_core_handle_ptr->tx_band))
      {
        pcl = max_pcl_index;
      }

      (void)rfgsm_core_get_enh_apt_bias(rfm_dev, &bias[slot_count], &mode[slot_count], pcl, mod_type, num_tx_slots);

      if (bias[slot_count] <= 0)
      {
        if (rfgsm_core_handle_ptr->parange_override_val < RFCOM_GSM_NUM_PA_STATE)
        {
          bias[slot_count] = tx_static_nv_data_ptr->smps_pdm_tbl.cal_smps_pdm_tbl[ rfgsm_core_handle_ptr->parange_override_val];
        }

        if (bias[slot_count] <= 0)
        {
          /*default values*/
          mode[slot_count] = BYPASS_MODE;
          bias[slot_count] = 3700;
        }
      }


      if (( (RF_HW_WTR2965_NON_CA_4373_V2_SAW == (rf_hw_type)rfcommon_nv_get_rf_config())) ||
                (RF_HW_WTR2965_NON_CA_4373_V2 == (rf_hw_type)rfcommon_nv_get_rf_config()) )
      {
          if((rfgsm_core_handle_ptr->tx_log.tx_slot_log[slot_count].apt_mode == mode[slot_count])&&
              (rfgsm_core_handle_ptr->tx_log.tx_slot_log[slot_count].smps_pdm == bias[slot_count]))
          {
             bias_flag = FALSE;
          }
          else
          {
             bias_flag = TRUE;   //Set QPOET bias
          }
      }
      else
      {
        bias_flag = TRUE;   //Set QPOET bias irrespective of the RF card type
      }

      rfc_gsm_tx_gain_config( rfm_dev, rfgsm_core_handle_ptr->tx_band, mod_type, slot_count, pwr_in_dbm, pa_range,
                              rfgsm_core_handle_ptr->scripts.tx_trans_script[TX_TRANS_FE_SCRIPT][slot_count], 
                              bias_flag, bias[slot_count], mode[slot_count], override_start_time);

      /*Update Current APT Mode with last Tx slot's APT mode */ 
      rfgsm_core_handle_ptr->apt_mode = mode[slot_count];

    }

    /*set the TX transition buffer*/
    rfgsm_core_set_tx_transition_buffer( rfm_dev,
                                         slot_count,
                                         mod_type);


    /* populate the log pkt */
    rfgsm_core_handle_ptr->tx_log.tx_slot_log[slot_count].power_index = pcl;
    rfgsm_core_handle_ptr->tx_log.tx_slot_log[slot_count].pre_dist_flag = predist_flag;
    rfgsm_core_handle_ptr->tx_log.tx_slot_log[slot_count].pa_scale = pa_scale;
    rfgsm_core_handle_ptr->tx_log.tx_slot_log[slot_count].env_gain = edge_env_gain;
    rfgsm_core_handle_ptr->tx_log.tx_slot_log[slot_count].pa_range = pa_range;
    rfgsm_core_handle_ptr->tx_log.tx_slot_log[slot_count].mod_type = mod_type;
    rfgsm_core_handle_ptr->tx_log.tx_slot_log[slot_count].rgi = rgi_log;
    rfgsm_core_handle_ptr->tx_log.tx_slot_log[slot_count].smps_pdm = bias[slot_count];
    rfgsm_core_handle_ptr->tx_log.tx_slot_log[slot_count].apt_mode = mode[slot_count];

  } // for (slot_count=0; slot_count < num_tx_slots; slot_count++)

  rfgsm_core_handle_ptr->tx_log.log_flag = TRUE;

}

/*----------------------------------------------------------------------------*/
/*!
   @brief
  This function returns the pa range setting currently in use for a given slot.

   @details

   @param 

   @retval
 None
   
*/
uint8 rfgsm_core_get_pa_range(rfm_device_enum_type rfm_dev, uint8 slotcnt)
{
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!", 0);

#ifdef GERAN_NIKEL_FW_INTERFACE // for NikeL : GFW dependency
  return(uint8)( rfgsm_core_handle_ptr->rfc_gsmctl_linear_pa_tx_gain_buff[slotcnt].txPaSlotGrfcState[0]<<1 | 
                 rfgsm_core_handle_ptr->rfc_gsmctl_linear_pa_tx_gain_buff[slotcnt].txPaSlotGrfcState[1] ); 
#else
  return(uint8)( rfgsm_core_handle_ptr->rfc_gsmctl_linear_pa_tx_gain_buff[slotcnt].r0<<1 | 
                 rfgsm_core_handle_ptr->rfc_gsmctl_linear_pa_tx_gain_buff[slotcnt].r1 ); 
#endif
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Given a power level in dBm, this function returns the PCL that is 'closet'
   to the value in power level NV items (NV_GSM_xxxx_POWER_LEVELS_I).

   @details

   @param
   pwr_dBm  -- power level in dB100

   @retval
   pcl_index -- the PCL that closet to the power input level.
   
*/
 

uint16 rfgsm_core_get_pcl_from_power( rfm_device_enum_type rfm_dev, rfgsm_modulation_type mod_type, int16 pwr_dBm )
{
  uint16 pcl_index = 0;
  uint16 num_pcl;
  int16 delta1;
  int16 delta2;
  int16 *pcl_vs_pwr;
  boolean enable_logging;

  rfgsm_core_handle_type *rfgsm_core_handle_ptr;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!", pcl_index);

  if (mod_type == RF_MOD_GMSK)
  {
    pcl_vs_pwr = rfgsm_core_handle_ptr->rfgsm_gsm_pcl_vs_power;
  }
  else
  {
    pcl_vs_pwr = rfgsm_core_handle_ptr->rfgsm_edge_pcl_vs_power;
  }

  enable_logging = rfgsm_core_handle_ptr->rfgsm_enable_pcl_logging;

  if ((rfgsm_core_handle_ptr->tx_band == RFCOM_BAND_GSM900 || rfgsm_core_handle_ptr->tx_band == RFCOM_BAND_GSM850))
  {
    /* Low band has one less number of PCL -- 14 */
    num_pcl=RFNV_NUMBER_OF_PCLS-2;
  }
  else
  {
    /* High band has 16 PCL */
    num_pcl=RFNV_NUMBER_OF_PCLS-1;
  }

  /* This while loop will always return the pcl index that is bias high.  In another word, given:
     1) power_level NV:  {500,700,900,1100,1300,1500,1700,1900,2100,2300,2500,2700,2900,3100,3250}
     2) input power level:  501
   
     The while loop will return pcl_index=1 even though the input power level is closer to pcl_index=0.
  */
  while ((pwr_dBm > pcl_vs_pwr[pcl_index]) && (pcl_index<num_pcl))
  {
    pcl_index++;
  }

  /* The following if-else statement will find the pcl_index that is closest to the input power level */
  if (pcl_index==0)
  {
    /* Do nothing.  The while loop only return pcl_index=0 'if' the input power level is
       less than or equal to what's in power_level NV.
    */

    if ( enable_logging == TRUE )
    {
      RFGC_MSG_1( MSG_LEGACY_LOW, "Lowest NV power level=%d", pcl_vs_pwr[pcl_index] );
    }
  }
  else if (pcl_index == num_pcl)
  {
    /* Take the diff and do absolute conversion */
    delta1 = pcl_vs_pwr[pcl_index] - pwr_dBm;
    delta1 = (delta1 <0) ? -(delta1) : delta1; 
  
    /* Take the diff and do absolute conversion */
    delta2 = pcl_vs_pwr[pcl_index-1] - pwr_dBm;
    delta2 = (delta2 <0) ? -(delta2) : delta2;

    if ( enable_logging == TRUE )
    {
      RFGC_MSG_6( MSG_LEGACY_LOW,
                  "a[%d]=%d, delta1=%d, a[%d]=%d, delta2=%d",
                  pcl_index,
                  pcl_vs_pwr[pcl_index],
                  delta1,
                  pcl_index-1,
                  pcl_vs_pwr[pcl_index-1],
                  delta2 );
    }  

    if (delta2 <= delta1)
    {
      /* power_level is closer to a[pcl_count-1]; */
      pcl_index = pcl_index-1;
    }
  }
  else
  {
    /* Known that the while loop bias high, comparison to the lower pcl_index is needed */
    delta1 = pcl_vs_pwr[pcl_index-1] - pwr_dBm;
    delta1 = (delta1 <0) ? -(delta1) : delta1; 
 
    delta2 = pcl_vs_pwr[pcl_index] - pwr_dBm;
    delta2 = (delta2 <0) ? -(delta2) : delta2;

    if ( enable_logging == TRUE )
    {
      RFGC_MSG_6( MSG_LEGACY_LOW,
                  "a[%d]=%d, delta1=%d, a[%d]=%d, delta2=%d",
                  pcl_index-1,
                  pcl_vs_pwr[pcl_index-1],
                  delta1,
                  pcl_index,
                  pcl_vs_pwr[pcl_index],
                  delta2 );
    }

    if (delta1 <= delta2)
    {
      pcl_index = pcl_index -1;
    }
  }

  return(pcl_index);
 }


/*----------------------------------------------------------------------------*/
/*!
   @brief

   utility returning the closest PCL (index) from a requested  Tx power (dBm100)

   This function is typically used by the NORMAL FTM-GSM RF controller.
   a prerequisite is to have updated the internal rfgsm_core_handle
   conversion tables rfgsm_gsm/edge_pcl_vs_power.

   it returns a pcl index using a mid range power bins; for example, given:
     1) tx_power_levels NV:  {500,700,900,1100,...,3100,3250,3250}

     2) the input power levels:
        tx_pwr: 600 returns pcl_index=1, and
        tx_pwr: 599 returns pcl_index=0.

     3) an invalid power request (ie. 0xffff) 
        is translated into a negative power request and returns pcl_index=0

     4) upper bounds validity and monotony of the tx_power_levels NV are checked
        from (RFNV_NUMBER_OF_PCLS - 3) pcl_index.
   
   @todo: the loop is not particularly efficient but more readable.

   @param
   rfm_dev                - RF logical device

   @param
   tx_pwr                 - the wanted TX power (dBm100)

   @return
   uint16 pcl_index       - with power in the mid ranges bin.

*/

uint16 rfgsm_core_tx_get_closest_pcl_index_vs_power
(
 rfm_device_enum_type rfm_dev,
 int16 tx_pwr
)
{
  uint16 pcl_index = 0;
  uint16 pcl_index_next = 1;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  gsm_power_levels *tx_power_levels_ptr = NULL;
  
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  
  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!", pcl_index);

  tx_power_levels_ptr
    = &(rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr->power_levels);

  RF_NULL_CHECK_RTN(tx_power_levels_ptr,
                    "tx_power_levels_ptr is NULL:"
                    ":invalid internal calibration",
                    pcl_index);
  
  /* power is scaled since comparing at mid bins */
  tx_pwr *= 2;
  
  for (pcl_index = 0;
       pcl_index < RFNV_NUMBER_OF_PCLS;
       pcl_index++)
  {
    pcl_index_next = ((pcl_index < RFNV_NUMBER_OF_PCLS - 1) ? (pcl_index + 1) : pcl_index);
    
    /* PCL index with highest bias preferred 
     * and the loop will break at high PCL indexes if the curve is concave,
     * this insure to match the PCL index which has the highest power available.
     */
    if ((tx_pwr < ((*tx_power_levels_ptr)[pcl_index] + (*tx_power_levels_ptr)[pcl_index_next])) ||
        ((pcl_index >= (RFNV_NUMBER_OF_PCLS - 3)) &&
         ((*tx_power_levels_ptr)[pcl_index] >= (*tx_power_levels_ptr)[pcl_index_next])))
    {
      /* converted tx_pwr back for this block of traces only. */
      tx_pwr /= 2;
      if((tx_pwr < (*tx_power_levels_ptr)[pcl_index]) && pcl_index > 0)
      {
        RFGC_MSG_5(MSG_LEGACY_MED,
                   "rfgsm_core_tx_get_closest_pcl_index_vs_power:"
                   ":found tx_pwr=%d in pcl_index bin p-1[%d]=%d, p0[%d]=%d",
                   tx_pwr,
                   pcl_index - 1,
                   (*tx_power_levels_ptr)[pcl_index - 1],
                   pcl_index,
                   (*tx_power_levels_ptr)[pcl_index]
                  );
      }
      else
      {
        RFGC_MSG_5(MSG_LEGACY_MED,
                   "rfgsm_core_tx_get_closest_pcl_index_vs_power:"
                   ":found tx_pwr=%d in pcl_index bin p0[%d]=%d, p1[%d]=%d",
                   tx_pwr,
                   pcl_index,
                   (*tx_power_levels_ptr)[pcl_index],
                   pcl_index_next,
                   (*tx_power_levels_ptr)[pcl_index_next]
                  );
      }      
      break;
    }
  }
  
  return((pcl_index < RFNV_NUMBER_OF_PCLS) ? pcl_index : (RFNV_NUMBER_OF_PCLS - 1));
}
