/*!
   @file
   rfgsm_core_rx.c

   @brief

   @details

*/

/*! @todo satheesha 10/30/08 do we need to keep the edit history??? */

/*===========================================================================
Copyright (c) 2008-2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/rf/core/src/rfgsm_core_rx.c#1 $
$DateTime: 2016/12/13 07:59:51 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/23/15   par     Fix incorrect AFC being applied in GBTA cases
02/23/15   sml     Fixing OFT complier errors
02/23/15   sml     Passing band info while generating GSM Tx/Rx 
                   SBI scripts to support band split across 2 WTRs
01/27/15   piy     Logging Rx burst RxAGC info and disabling therm reads via debug NV
01/19/15   sw      Use tuner start delta to program XSW
01/16/14   ec      Use new debug NV flags for SAWless functionality
01/13/15   sml     GL1 -> RF handshake mechanism to handles cases when 
                   GSM ASD response is not being received as scripts 
                   weren't executed
12/19/14   hoh     LIF data implementation
12/15/14   tws     Fix issue with sawless F3s on non-sawless RFC 
12/02/14   sml     Read WTR or RF front end devices status in between Rx/Tx 
                   burst along with GRFC status
11/25/14   sb      HL/LL algorithm changes and bug fixes

11/19/14  huibingl take the channel number discontinuity into account when do GSM RX cal data interpolation
11/12/14   sw      Rename RX/TX script types to reflect actual devices
10/17/14   rr      Separate rx/tx alternate path core handles
10/20/14   sw      Use correct band for GSM ASDiv
10/02/14  ec/cpm   Fixes for V2 and V4 calibration on SAWless targets
09/23/14   zbz     changes to support GSM and LTE/TDS coexistence related RFC changes
09/18/14   sw      Boolean to indicate ASD scripts to be built only once
08/14/14   sc      Coex fix for non-tx capable device accessing Tx NV
08/11/14   sc      Ensure chain update with alternate path and include IRAT
08/01/14   ec      Add code to handle HL NV items and updated overrides for SAWless targets
07/31/14   tsr     GSM Alternate path support
07/31/14   sw      Support both rffe and grfc-based xsw for ASD
07/31/14   sg      Configure only ASD GRFCs (not tuners) during enter mode/wakeup
07/30/14   hoh     Add enhanced rx feature support 
07/28/14   tsr     DR-DSDS: Add support for device driver to handle worst case rx alpha for IDLE mode RX
07/28/14   zbz     Fix RSB compensation coeffs
07/11/14   ec      Set Rx Linearity mode on SAWLess targets based on decision table
07/02/14   tsr     DR-DSDS: Add worst case rx alpha for IDLE mode RX
06/18/14   tsr     RXD diversity call flow changes to enable DRX path along with PRX path
06/12/14   tsr     Update RX ARFCN in rfgsm_core_handle for RXD device
06/12/14   hk      Band to be passed into rfc_gsm_rx_burst_config instead of core handle 
06/02/14   sml     Adding necessary check condition in the case where tuners are not present 
                   and ASD switch is present to avoid un-necessary prints
05/13/14   tsr     Retrieve rx stop settings for PRX device as there is only one RX Stop for both PRX and DRX
04/29/14   tsr     Mainline GSM RFC config v2 apis 
04/24/14   ch      Replaced ATUNER_SCRIPT with RX_FE_CONFIG_SCRIPT
04/24/14   sw      Introduce rffe-based api for ASD processing
04/16/14   sb      IP2 cal Interface and functionality changes
04/14/14   tsr     Add DRX gain in FTM mode for RX diversity
04/10/14   tsr     Add RFLM RX gain and spectral inv coefficients for DRX
03/20/14   sw      Update ASD code to use new ASDIV Manager api
03/11/14   tsr     Added RXD support for RX burst 
03/13/14   sw      ASD Enhancements
03/11/14   tsr     Program device scripts dynamically through CCS
03/11/14   sc      Expose RFLM overhead to RFSW to ensure GL1 account for delay
03/05/14   tsr     Fixed offtarget build
03/04/14   sw      Introduce ASD functionality
02/28/14   tsr     Separate RX burst scripts and TX burst scripts retrieval from device driver
02/28/14   sc      Pass core data through mc to core layers
02/25/14   tsr     Update RX Alpha based on wtr writes and pll settling time from device driver
02/24/14   tsr     Deprecate Feature D3925
02/18/14   tsr     GSM RX Diversity 
01/30/14   sc      Manage data table independent of device
01/15/14   sc      Remove unused APIs using FW shared memory
12/13/13   ry     Remove hardcoding of RFM_DEVICE_0
11/07/13   sc      Rx gain API using Rx burst type
10/31/13   sc      Feature-guard FW shared mem buffer population
10/23/13   tsr     Mainline D3295 changes for BOLT
10/23/13   sg      GSM ASDiv Tuner Support
10/21/13   sc      Modify dynamic event building for scenario-specific wrappers
10/14/13   sc      Mainline core DM and RFLM code
10/14/13   sml     Limiting the number of LNA ranges to five
                   depending upon a flag which needs to be 
                   updated by WTR 
10/09/13   tsr     Dynamic RF CCS event changes for WTR3925
10/03/13   tsr     Added support retrieve to individual device scripts
10/03/13   sb      Add changes to receive MSGR cmd from MCS to switch to filtered/unfiltered 
                   path for SGLTE coexistence (GSM DCS +LTE B39/TDS B39/B34)
10/01/13   sb      Disable SGLTE Coexistence filtered path cal data
09/17/13   sb      Fix check for right rx burst type 
09/17/13   sb      Support for SGLTE/SGTDS coexistence
09/12/13   sc      RFLM interface encompassed by core DM
08/27/13   sml     Extracting ASD script depending on device id through
                   call back data
08/16/13   ndb     Clean-up the SPI based antenna tuner support
08/14/13   sc      Revert Rx burst type variable name
08/05/13   sml     Fixing complier errors in off-target
08/02/13   sml     Fixing Complier warnings
07/30/13   sml     removing un-necessary comments
07/29/13   sml/svi ASD support for GSM tech
07/23/13   sc      Update RFLM data information (featurised)
07/22/13   ec	   Use new F3s/Null pointer checks
05/06/13   aka     Added rfgsm_core_get_lna_gain API
04/09/13   sr      removed legacy antenna tuner code which is not used.
02/05/13   sb      Created separate APIs to configure RF front end devices based on whether
                   they are called every TX/RX burst or only when the band is changed.
01/2/13    aka     Changes to support multi chain GSM operation in FTM mode
01/09/13   sr      Removed un-used temp comp function prototype.
12/10/12   tws     Move global variables to the gsm core handler. 
                   Remove ftm_gsm_rssi_to_dbm.
12/03/12   sc      Cleanup mDSP shared variable interface
12/03/12   aka     Changes to use NV ptr from rfgsm_core_handle_ptr
11/29/12   tws     Added rfm_device parameters to FTM GSM.
11/28/12   aka     Changes to extend path based computations
11/26/12   aka     Changes to use a path specific gsm handle
11/21/12   av      Added rfm_device parameters to mc/core/mdsp/msm layers.
11/19/12   sc      RF-Only mDSP update shared variables cleanup
10/16/12   tsr     Cleanup GSM RF bringup flags 
10/11/12   sc      Cleanup up unrequired Rx burst & SBI/Non-SBI API burst buffer params
10/02/12   tc      Remove obsolete functions and globals. 
10/01/12   sb      Made changes to add rfm device to rf device interface and uppler layer APIs 
09/18/12   tc      Remove FEATURE_RF_HAS_QFE1320-related functionality. 
08/21/12   sc      Rename RF GSM debug Ant tuner flag 
08/07/12   sc      Using seperate Rx and Tx scripts for CCS buffer tracability
08/06/12   sc      Pass in zero for build Rx burst num_slots param as only used for Tx
07/17/12   tsr     Changes for GSM RX verification on RUMI
07/26/12   sr      Changes to APIS to take rfm_dev as parameter.
07/20/12   sr      Made changes to pass the RFM_DEVICE and rf_buffer to rfc_gsm
07/06/12   sc      Edit API parameters to support CCS buffer format
07/02/12   sr      renamed rfc_common_gsmctl.h to rfc_common_tdd.h as it is shared with TDS.
06/21/12   sc      Rx setup burst API support for shared memory interface
06/20/12   shb     Updated GSM device APIs to accept script buffer
07/02/12   sn      PA/ASM device interface and scripting support
06/21/12   vb/rsr  Added support for QFE1510
12/20/11   sn      Added support for QFE1320
10/28/11    sr     made changes use the tx_nv_tbl ptr for any tx NV items
                   and rx_nv_tbl for any rx NV items.
10/21/11   dej     Made rfgsm_core_ten_log_of_power_val not static
10/19/11   sb      Apply therm read in idle frame and enable FTM temp comp and vbatt comp during idle frame
10/11/11    sr     optimized code for ISR over-run issue.
09/15/11    sr     added antenna tuner support code.
09/06/11   sr      read the temp during Rx burst setup.
08/25/11   sar      Removed obsolete file bbrx.h.
08/15/11   sr      klockwork fixes.
08/06/11   sr      RFC Cleanup: removed un-used code.
07/01/11   vrb     Replacing direct calls to rfgsm_ds with wrapper functions
05/26/11   sr     Added rfc rx band config call in tune rx. 
03/03/11   sr     off-target error fix.
03/03/11    sr     merge from mdm9k branch
02/07/11   av      FTM IRAT support : Added RSSI to DB api for GSM. (from scmm) 
01/17/11   dej     Mainline RF_DEVICE_MERGE_INTERIM_FEATURE
10/11/10   lcl    Changed MSG_LVL to MSG_LEGACY to avoid too many F3 messages
09/01/10   av      GSM RFC Rearch
06/24/10   rsr     support for scmm/mdm9k merge
06/16/10   rsr     Temp comp updates from QSC6x95
06/07/10   lcl     Removed rfc_config_band_data from rfgsm_core_tune_rx to 
                   fix WCDMA UL issue during W2G in DCH state.
03/18/10   lcl     Moved rfgsm_cm from RFC to this file since it can be shared.
03/18/10   lcl     Pull in lint fix from QSC6295
12/28/09   sr      High/Medium lint errors fix
12/22/09   sr      changed the debug msg macro from legacy to new one.
10/21/09   sr      new device driver interface changes.
10/12/09   sr      Fixed warnings.
09/17/09   sar     Removed deprecated file, adie.h for CMI conformance.
07/20/09   sr      Fixed warnings.
07/08/09   av      Added calls to rfc to configure the band data.
07/06/09   sr     changes to make the operating band to rfcom_gsm_band type
07/04/009  sr      spec inversion setting in the tune_rx based on the band.
06/19/009  sr      fixed the continuous rx gain range problem.
08/06/009  sr      unused code cleanup.
03/27/009  sr      rfgsm_core_ctl name changed to rfc_card_gsmctl
03/18/09   sr      Changed the rfgsm_mdsp__() name to reflect new name
03/16/09   sr      code clean-up.
03/13/09   sr      code clean-up for SCMM build
10/30/08   sr      Initial revision.

============================================================================*/

#include "rfa_variation.h"
#include "rfcom.h"
#include "msg.h"
#include "ftm.h"
#include "ftm_gsm_rfctl.h"
#include "rfgsm_core_types.h"
#include "rfgsm_core_handle.h"
#include "rfdevice_gsm_intf.h"
#include "rfgsm_nv_cmn.h"
#include "rfgsm_mdsp.h"
#include "rfgsm_msm.h"
#include "rfc_card_gsm.h"
#include "rfgsm_core_timing_consts.h"
#include "rfgsm_core.h"
#include "rfc_card.h"
#include "rfc_common_tdd.h" 
#include "rfcommon_msg.h"
#include "rfm_internal.h"
#include "rf_hal_buffer.h"
#include "rfgsm_data_manager.h"

#ifdef FEATURE_RF_ASDIV
#include "rfc_common.h"
#include "rfcommon_asdiv_manager.h"
#include "rfdevice_logical_component_types.h"

/* Default tuner start time */
#define RFC_GSM_DEFAULT_ANTENNA_TUNER_START_DELTA -150 /* QS */
#endif

extern int16 ftm_gsm_rssi_to_dbm; /*Added from SCMM*/

/* Flag to tell NV to load 5th gainrange data */
boolean rf_has_five_gainranges = FALSE;


volatile dBx16_T rf_debug_force_rssi_to_dbm = 0;

/*---------------------------------------------------------------------------
  Rx RSSI -> dB translation tables. For each entry in rssi_range_tbl, there
  is a corresponding entry in ten_log_power_x16_tbl.
---------------------------------------------------------------------------*/
#define SIZEOF_RSSI_RANGE_TBL \
          ( sizeof(rssi_range_tbl)/sizeof(rssi_range_tbl[0]) )

static const uint32 rssi_range_tbl[] =
{
/* Include auto-generated rssi range table */
  #include "rfgsm_rssi.m"
};

static const uint16 ten_log_power_x16_tbl[] =
{
/* Include auto-generated 10log(rssi)*16 table */
  #include "rfgsm_10logrssix16.m"
};

/*----------------------------------------------------------------------------*/
/* EXTERNAL FUNCTIONS*/
extern void rfgsm_device_init_vars(void);

/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*!
  @details
  This function sets up the Rx scripts to program the synthesizer to the
  requested channel.

  @retval
  TRUE/FALSE - sucessful/unsuccessful tune.
*/
boolean rfgsm_core_tune_rx(rfgsm_core_data_type *core_data, uint16 arfcn)
{
  rfcom_gsm_band_type band;
  boolean use_coex_filtered_path = FALSE;
  rfc_gsm_coex_band_info_type band_split_info;
  boolean flag = FALSE;
  boolean use_hl_mode = FALSE;

  band = core_data->rfgsm_core_handle_ptr->band;
  band_split_info.band = band;
  band_split_info.status = RFC_FILTERED_PATH_FALSE;

  /*Set/clear filtered path flag*/
  core_data->rfgsm_core_handle_ptr->use_coex_filtered_path_rx = rfcommon_core_get_rx_filter_status();

  use_coex_filtered_path = core_data->rfgsm_core_handle_ptr->use_coex_filtered_path_rx;
 
  if (use_coex_filtered_path == TRUE)/* Filtered path needs to be use based on the channel*/
  {
    band_split_info.status = RFC_FILTERED_PATH_TRUE;

    flag = rfc_get_split_band_type(core_data->rfm_dev, RFC_CONFIG_RX, core_data->rfgsm_core_handle_ptr->band, arfcn, &band_split_info );


    if (flag == TRUE)
    {
      if (core_data->rfgsm_core_handle_ptr->rx_burst == RF_RX_BURST)
      {

        /* updated band enum info*/
        band = band_split_info.band;

      }
    }
    else
    {
      band = rfc_get_unfiltered_band_type(core_data->rfm_dev,RFC_CONFIG_RX,core_data->rfgsm_core_handle_ptr->band);
    }
  }
  else
  {

      band = rfc_get_unfiltered_band_type(core_data->rfm_dev, RFC_CONFIG_RX, core_data->rfgsm_core_handle_ptr->band);
  }
  /*This API retrieves RF configuration for all FE devices including GRFCs, ASM, ATUNER etc. for a give RX burst*/
  rfc_gsm_rx_burst_config( core_data->rfm_dev,
                           band,
                           arfcn, 
                           core_data->rfgsm_core_handle_ptr->rx_alternate_path );
  /* Setup band dependent settings */
  rfdevice_gsm_set_rx_band( core_data->rfm_dev,
                            band,
                            RFDEVICE_CREATE_SCRIPT, 
                            (rf_buffer_intf *)core_data->rfgsm_core_handle_ptr->scripts.rx_start_script[RX_WTR_SCRIPT]);

  /* If we are on SAWless target, check if we should use HL mode for this channel and then update device */
  if ( (rfgsm_core_get_sawless_status(core_data->rfm_dev)) == TRUE )
  {
    /* Check if an override is set */
    if ( (core_data->rfgsm_core_handle_ptr->sawless_linearity_control.linearity_override == RFGSM_SAWLESS_NO_OVERRIDE)
         || (core_data->rfgsm_core_handle_ptr->sawless_linearity_control.linearity_override == RFGSM_SAWLESS_CHAN_OVERRIDE) )
    {
      if ( (rfgsm_core_is_arfcn_in_high_lin(core_data->rfm_dev, arfcn, band, &use_hl_mode)) == FALSE )
      {
        MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Could not get lin state for SAWless device %d!",core_data->rfm_dev);
      }

      /* Always use HL mode in power scan bursts unless overridden in NV */
      if (core_data->rfgsm_core_handle_ptr->rx_burst == RF_PWR_BURST)
      {
        if ( core_data->rfgsm_core_handle_ptr->sawless_linearity_control.disable_hl_power_scan == TRUE)
        {
          use_hl_mode = FALSE;
        }
        else
        {
          use_hl_mode = TRUE; 
        }
      }
    }/* ^ RFGSM_SAWLESS_NO_OVERRIDE or RFGSM_SAWLESS_CHAN_OVERRIDE ^ */
    else if ( core_data->rfgsm_core_handle_ptr->sawless_linearity_control.linearity_override == RFGSM_SAWLESS_HL_OVERRIDE )
    {
      use_hl_mode = TRUE;
    }/* ^ RFGSM_SAWLESS_HL_OVERRIDE ^ */
    else if ( core_data->rfgsm_core_handle_ptr->sawless_linearity_control.linearity_override == RFGSM_SAWLESS_LL_OVERRIDE )
    {
      use_hl_mode = FALSE;
    }/* ^ RFGSM_SAWLESS_LL_OVERRIDE ^ */

    if ( core_data->rfgsm_core_handle_ptr->sawless_linearity_control.enable_logging == TRUE)
    {
      MSG_3( MSG_SSID_RF,
             MSG_LEGACY_HIGH,
             "Using SAWless linearity state %d for dev %d, override %d",
             use_hl_mode,
             core_data->rfm_dev,
             core_data->rfgsm_core_handle_ptr->sawless_linearity_control.linearity_override );
    }

    /* Set the device linearity mode based on ARFCN linearity table or override setting */
    rfdevice_gsm_cmd_dispatch(core_data->rfm_dev, RF_PATH_0, RFDEVICE_GSM_SET_HIGH_LIN_MODE, &use_hl_mode);
  } /* ^ sawless_status==TRUE ^ */

  /* Tell the library to tune */
  rfdevice_gsm_tune_rx( core_data->rfm_dev, 
                        arfcn,
                        core_data->rfgsm_core_handle_ptr->enh_rx_on_flag,
                        core_data->rfgsm_core_handle_ptr->enh_rx_freq_offset);

  return( TRUE );
}

/*----------------------------------------------------------------------------*/
/*!
  @details
  This function computes and validates the Rx power at the antenna.  It
  accomplishes this by using the following formula:

              10log(rssi) - Grx

  where Grx represents the entire Rx chain gain from the antenna input to
  baseband.

  NOTE: Grx = G(digital) + G(rf)

  The design of this function assumes that rf_set_rx_gain() was
  previously invoked;  the gain value that is required for the Rx power
  computation is established there.

  @param
  rssi_value    : input,  (I^2 + Q^2) value
  dBm_x16_value : output, absolute power result
  gain_range    : output, gain range used in deriving the dBm_x16_value
                  (only provided for informational purposes - logging)
  stored_gain  : gain to be computed 

*/
rf_measurement_T rfgsm_core_compute_rx_power(uint32 rssi_value,
                                             dBx16_T *dBm_x16_value,
                                             rf_gain_range_T *gain_range,
                                             const rf_gain_T *stored_gain, 
                                             rfm_device_enum_type rfm_dev)
{
  rf_measurement_T  power_result = RF_NOISY;
  dBx16_T           dBx16_result;
  int16 interim=0; /*Added from SCMM*/


  if (stored_gain != NULL)
  {
    if (rf_debug_force_rssi_to_dbm != 0)
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR," *** Forcing the RSSI->dBm = %d ***",
            rf_debug_force_rssi_to_dbm);
      *dBm_x16_value  = (dBx16_T)(rf_debug_force_rssi_to_dbm * 16);
      *gain_range     = stored_gain->range;
      return (power_result);
    }

    if (rfgsm_core_ten_log_of_power_val(rssi_value, &dBx16_result))
    {
      *dBm_x16_value  = dBx16_result - stored_gain->valx16;
      *gain_range     = stored_gain->range;

      /*
      ** Validate the signal level against the particular gain range
      ** windows. The windows are calculated at the time that the
      ** gain range is set.
      */
      if (*dBm_x16_value > (stored_gain->range_high_limitx16 + RFCOM_HOT_DELTA))
      {
        power_result   = RF_SATURATED;
      }
      else if (*dBm_x16_value > stored_gain->range_high_limitx16)
      {
        power_result   = RF_HOT;
      }
      else if (*dBm_x16_value < 
               (stored_gain->range_low_limitx16 - RFCOM_COLD_DELTA))
      {
        power_result   = RF_NOISY;
      } 
      else if (*dBm_x16_value < stored_gain->range_low_limitx16)
      {
        power_result   = RF_COLD;
      }
      else
      {
        power_result = RF_IN_RANGE;
      }
    } 
    else
    {
      /* The RSSI should be non-zero coming from the MDSP because of
      ** thermal induced and radio noise floor.
      */
      *dBm_x16_value  = RFGSM_MIN_RX_POWER_REPORTED;
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR," *** RSSI = 0 ***",0);
    }
  } 
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"No stored gain!",0);
  }

  if(ftm_gsm_cal_mode_enabled(rfm_dev)) /*Check if in FTM cal mode*/
  {
    if ( (*dBm_x16_value) & 0x4000 )
    {
      interim = (int16)((*dBm_x16_value) | 0x8000);
    }
    else
    {
      interim = (int16)(*dBm_x16_value);
    }
    ftm_gsm_rssi_to_dbm = interim / 16;
  }


  return(power_result);
} /* end of rfgsm_compute_rx_power */

/*----------------------------------------------------------------------------*/
/*!
  @details
  This function performs a binary search.

*/
int16 rfgsm_core_utils_binary_search(const uint32 data[], uint32 input, 
                                     uint16 sizeof_data) 
{
  int16 low, mid = 0, high;

  //Lint, type casted to sint15 
  low =0; high=(int16)sizeof_data - 1;
  while (low <= high)
  {
    mid = (high + low)/2;
    if ( data[mid] < input )
    {
      low = mid + 1;
    } 
    else if ( data[mid] > input )
    {
      high = mid - 1;
    } 
    else
    {
      /* found mid ! */
      break;
    }
  }

  /* Ensure that we are returning the lower point of a line segment */
  /* for the interpolation algorithm                                */
  if (data[mid] > input)
  {
    mid--;
  }

  return(mid);

}

/*----------------------------------------------------------------------------*/
/*!
  @details
  This function computes the 10log10(power).  It implemets this by
  linearly interpolating a log LUT.  The output of the LUT is a 10log10 value.
  The LUT provided a maximum error of 1/16 dB.
*/
boolean rfgsm_core_ten_log_of_power_val(uint32 power, dBx16_T *dBx16_result_ptr)
{
  boolean status = FALSE;

  int16  index;
  uint32  a,b;
  uint16  f_of_a, f_of_b;

  /* Can't take log10(0) = undefined */
  if (power > 0)
  {
    index = rfgsm_core_utils_binary_search(rssi_range_tbl, power, 
                                           SIZEOF_RSSI_RANGE_TBL);

    if (index < 0)
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid index for the RSSI table. index=%d",index);      
      return status;
    }

    /* Linear interpolate the result value */
    a      = rssi_range_tbl[index];
    f_of_a = ten_log_power_x16_tbl[index];
    b      = rssi_range_tbl[index+1];
    f_of_b = ten_log_power_x16_tbl[index+1];

    *dBx16_result_ptr = (dBx16_T) (f_of_a + ((power-a)*(f_of_b-f_of_a))/(b-a));

    status = TRUE;
  }

  return(status);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief update rx_alpha and mon_alpha  timing parameters  to core dm.

  @details This api updates rx_alpha and mon_alpha  timing parameters to core dm memory space
  where it is retrieved by GFW for RX burst

  @param rfm device
 
  @retVal None

*/
void rfgsm_core_update_rx_timing(rfgsm_core_data_type *core_data)
{
  rfc_gsm_rx_timing_info_type *rx_timing  = NULL;
  int32 time_offset = 0;

  RF_NULL_CHECK_RTN_V(core_data->rfgsm_core_handle_ptr->rfc_gsm_core_ptr, "rfc_gsm_core_ptr is NULL!");
  RF_NULL_CHECK_RTN_V(core_data->rfgsm_core_handle_ptr->rfc_gsm_core_ptr->rx_timing, "rx_timing is NULL Rx timings not updated!");
 
  /* Get the Rx timings from rfc structure */
  rx_timing = core_data->rfgsm_core_handle_ptr->rfc_gsm_core_ptr->rx_timing;

  /* Update the Rx burst alpha based on the number of SBIs and PLL Settling Time */
  /* Calculate the Rx alpha value in qs time */

  time_offset = (rx_timing->rx_burst_processing_time_from_pll_qs - rx_timing->pll_settling_time_qs);

  rx_timing->rx_alpha_qs = RFGSM_RF_RX_SETUP_TIME_QS(rx_timing->rx_burst_sbis_before_pll,
                                                     rx_timing->pll_settling_time_qs);
  rx_timing->rx_alpha_qs += RFLM_GSM_RX_MAX_PROC_OVERHEAD_QS;
 
  if (time_offset > 0)
  {
    rx_timing->rx_alpha_qs += time_offset;
    RF_MSG_1(RF_HIGH,"rfgsm_mdsp_update_rx_timing,  Updated rx alpha (qs) :%d  ", rx_timing->rx_alpha_qs );
  }

  /* The overhead is added to accomodate worst case delay for RX burst writes due to rffe bus contention.
     This RX alpha is used for IDLE mode RX by GL1 in DR-DSDS mode */
  rx_timing->idle_mode_rx_alpha_qs = rx_timing->rx_alpha_qs + RFGSM_WORST_CASE_RFFE_BUS_DELAY;

  /* Write Rx alpha value to RFLM DM */
  if( !rfgsm_core_dm_set_rx_alpha(core_data->sub_id,(uint16)rx_timing->rx_alpha_qs, (uint16)rx_timing->idle_mode_rx_alpha_qs) )
  {
    RF_MSG_1(RF_ERROR, "Rx timings are NOT updated for device %d", core_data->rfm_dev);
    return;
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize RFGSM Enter mode for RX

  @details

  @param 

*/
void rfgsm_core_enter_mode_rx_init(rfgsm_core_data_type *core_data)
{
  boolean use_reduced_rx_sbi_start_time = TRUE;

  /*@TODO  Both this device cmd will be removed once WTR removes the code from device driver */
  rfdevice_gsm_cmd_dispatch( core_data->rfm_dev, RF_PATH_0, RFDEVICE_GSM_USE_REDUCED_RX_SBI_TIME, &use_reduced_rx_sbi_start_time);

  /* update the timing related variables to core DM */
  rfgsm_core_update_rx_timing(core_data);
}

/*----------------------------------------------------------------------------*/
/*!
  @details
  This functions sets up the Rx scripts to configure the Rx gain and the LNA
  based on the requested Rx level.  The gain range switch points are
  adjusted by the calibration results to apply the correct gain setting.

  This function relies on the arfcn information to be valid for the given
  burst so that the appropriate CAL offsets can be applied; therefore,
  rf_tune_to_channel() must be called first.

  rx_lvl_dBm - 1/16 dB format: sign bit, 11 decimal bits, 4 fractional bits

  @retVal
  TRUE - requested gain was set.
  FALSE - unable to set the requested gain.
*/
boolean rfgsm_core_set_rx_gain(rfgsm_core_data_type *core_data,
                               dBx16_T rx_lvl_dbm,
                               rf_gain_T *gain,
                               uint8 lna_range,
                               uint16 arfcn,
                               rfgsm_enh_rx_params_type enh_rx_params)
{
  boolean set_radio_gain = FALSE;
  rfgsmlib_rx_gain_range_type lna_range_to_set = RFGSMLIB_GAIN_RANGE_1;
  uint16 char_offset_bin_indx = 0;

  /* Mask off MSB in case it is being used for HL\LL indication on SAWless target */
  lna_range = FTM_GSM_MASK_OFF_LNA_STATE_MSB(lna_range);

  /*Set/clear filtered path flag*/
  core_data->rfgsm_core_handle_ptr->use_coex_filtered_path_rx = rfcommon_core_get_rx_filter_status();

  core_data->rfgsm_core_handle_ptr->rf_tune_commanded = TRUE;

  core_data->rfgsm_core_handle_ptr->rf_rx_arfcn = arfcn;
  if (core_data->enable_rx_diversity) 
  {
    core_data->rfgsm_core_handle_ptr_rxd->rf_rx_arfcn = arfcn;
  }

  /*Add characterized offset due to filtered path*/

  if(core_data->rfgsm_core_handle_ptr->use_coex_filtered_path_rx == TRUE)
  {
    char_offset_bin_indx = rfgsm_core_get_arfcn_bin_index_char_offset_tbl(core_data->rfm_dev, 
                                                                          core_data->rfgsm_core_handle_ptr->band, 
                                                                          core_data->rfgsm_core_handle_ptr->rf_rx_arfcn,
                                                                          RFC_CONFIG_RX );
    if (char_offset_bin_indx < RFNV_MAX_ARFCN_BINS)
    {
      rx_lvl_dbm += (dBx16_T)core_data->rfgsm_core_handle_ptr->rfgsm_rx_nv_tbl->rx_static_nv_data_ptr->coex_rx_char_offset_info_tbl.rx_char_offset_tbl[char_offset_bin_indx];
    }
  }

  if (gain == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"gain == NULL!",0);
    return (set_radio_gain);
  }

  if(ftm_gsm_cal_mode_enabled(core_data->rfm_dev)) /*Check if in FTM cal mode*/
  {
    /* When in ftm continuous rx mode this function is called from acquisition.
    and the lna_range parameter will null. So, Use the rx_lvl_dbm to get lna
    range.  */
    if (ftm_gsm_get_lna_range_from_rx_lvl(core_data->rfm_dev)) 
    {
      lna_range = (uint8) rx_lvl_dbm ;
    }

    lna_range_to_set = (rfgsmlib_rx_gain_range_type)(lna_range + 1);
    rfdevice_gsm_set_rx_gain_range(core_data->rfm_dev, (int32)lna_range_to_set);
    rfgsm_core_set_radio_gain(core_data->rfm_dev, rx_lvl_dbm, gain, enh_rx_params); /*Added from SCMM*/
	core_data->rfgsm_core_handle_ptr->valx16 = gain->valx16;
    
    set_radio_gain = TRUE;

    return( set_radio_gain );
  } 

  if (core_data->rfgsm_core_handle_ptr->rf_tune_commanded == FALSE)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid burst type or tune_to_channel fail",0);
    return (set_radio_gain);
  }

  rfgsm_core_set_radio_gain(core_data->rfm_dev, rx_lvl_dbm, gain, enh_rx_params);
  set_radio_gain = TRUE;
  core_data->rfgsm_core_handle_ptr->valx16 = gain->valx16;

  /* Update the library */
  lna_range_to_set = (rfgsmlib_rx_gain_range_type)gain->range;
  rfdevice_gsm_set_rx_gain_range(core_data->rfm_dev, (int32)lna_range_to_set);
  
  /* 4 Rx's possible in GPRS with a single tune command */ 
  /* reset error check flag for the next iteration */
  core_data->rfgsm_core_handle_ptr->rf_tune_commanded = FALSE;

  return( set_radio_gain );
}

/*----------------------------------------------------------------------------*/
/*!
  @details
  This functions returns LNA gain for a given Rx channel and LNA state in
  rx_lvl_dBm - 1/16 dB format: sign bit, 11 decimal bits, 4 fractional bits

  @retVal
  LNA gain in dBm16
*/
dBx16_T rfgsm_core_get_lna_gain(rfm_device_enum_type rfm_dev, uint16 arfcn, uint16 gain_range)
{
  dBx16_T   fc_gain;
  uint8 arfcn_index;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  uint16 arfcn_linearity = 0;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr,"rfgsm_core_handle_ptr is NULL!", 0);
  
  /* Update the SAWless override for v4 cal if MSB of gain word is set */
  if ( FTM_GSM_CHECK_LNA_STATE_MSB(gain_range) )
  {
    rfgsm_core_handle_ptr->sawless_linearity_control.linearity_override = RFGSM_SAWLESS_HL_OVERRIDE;
  }
  else
  {
    rfgsm_core_handle_ptr->sawless_linearity_control.linearity_override = RFGSM_SAWLESS_LL_OVERRIDE;
  }

  arfcn_linearity = rfgsm_map_arfcn_linearly(rfgsm_core_handle_ptr->band, arfcn);
  arfcn_index =  rfgsm_nv_get_closest_arfcn_index( rfm_dev, arfcn_linearity);
  fc_gain = rfgsm_nv_rx_freq_comp (rfm_dev, (rfnv_gsm_lna_state_type)FTM_GSM_MASK_OFF_LNA_STATE_MSB(gain_range), arfcn_index);
  return fc_gain;
}

/*----------------------------------------------------------------------------*/
/*!
  @details
*/
void rfgsm_core_set_radio_gain(rfm_device_enum_type rfm_dev, dBx16_T rx_lvl_dbm,  rf_gain_T *gain, rfgsm_enh_rx_params_type enh_rx)
{
  dBx16_T   fc_gain;
  dBx16_T   actual_switch_1_2, actual_switch_2_3, actual_switch_3_4;
  dBx16_T   actual_switch_4_5 = 0;
  uint8 arfcn_index;
  boolean is_enh_rx_supported = FALSE;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  rfgsm_nv_rx_switch_type *rx_sw_pt_ptr = NULL;
  uint16 arfcn_linearity = 0;
  boolean use_hl_mode = FALSE;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN_V(rfgsm_core_handle_ptr,"rfgsm_core_handle_ptr is NULL!");

  arfcn_linearity = rfgsm_map_arfcn_linearly(rfgsm_core_handle_ptr->band, rfgsm_core_handle_ptr->rf_rx_arfcn);
  arfcn_index =  rfgsm_nv_get_closest_arfcn_index( rfm_dev, arfcn_linearity );

  rx_sw_pt_ptr = rfgsm_core_handle_ptr->rfgsm_nv_rx_freq_comp_switch_ptr;

  if ( rfgsm_core_get_sawless_switchpoints(rfm_dev, 
                                           rfgsm_core_handle_ptr->rf_rx_arfcn, 
                                           rfgsm_core_handle_ptr->band, 
                                           rx_sw_pt_ptr) == FALSE)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Could not get Rx swichpoints for device %d ", rfm_dev);
    return;
  }

  RF_NULL_CHECK_RTN_V(rx_sw_pt_ptr,"rx_sw_pt_ptr is NULL!");

  // Get the switchpoints from NV
  actual_switch_1_2 = rx_sw_pt_ptr->rx_switch_1_2_dBm[arfcn_index];
  actual_switch_2_3 = rx_sw_pt_ptr->rx_switch_2_3_dBm[arfcn_index];
  actual_switch_3_4 = rx_sw_pt_ptr->rx_switch_3_4_dBm[arfcn_index];
  actual_switch_4_5 = rx_sw_pt_ptr->rx_switch_4_5_dBm[arfcn_index];

  /* Initialize LIF freq offset to 0 every frame since device driver uses it blindly */
  rfgsm_core_handle_ptr->enh_rx_freq_offset = 0;

  rfdevice_gsm_get_rx_enh_rx_support(rfm_dev, &is_enh_rx_supported);
  if (is_enh_rx_supported)
  {
    rfgsm_core_handle_ptr->enh_rx_on_flag = enh_rx.enh_rx_enable_flag;

    if( enh_rx.enh_rx_enable_flag == FALSE )
    {
      // ZIF mode
      rfgsm_core_handle_ptr->enh_rx_freq_offset = 0;
    }
    else
    {
      /* LIF mode - check that the freq offset passed from L1 
        matches the expected value, currently +/-135KHz */
      if( enh_rx.enh_rx_offset_freq == RF_LIF_FREQ_OFFSET_DEFAULT_HZ || 
          enh_rx.enh_rx_offset_freq == -RF_LIF_FREQ_OFFSET_DEFAULT_HZ )
      {
        rfgsm_core_handle_ptr->enh_rx_freq_offset = enh_rx.enh_rx_offset_freq;
      }
      else /* else throw an error */
      {
        MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,
              "LIF is active but enh_rx_offset_freq is incorrect! %d %d", 
              enh_rx.enh_rx_enable_flag, enh_rx.enh_rx_offset_freq);
      }
    } // ^ ! if (!enh_rx.enh_rx_enable_flag)
  } // ^ if (is_enh_rx_supported)

  /* Set the LNA and the Programmable Gain Amplifier */
  if ( rx_lvl_dbm < actual_switch_1_2)
  {
    /* ---------------------------------------------------------------- */
    /* Get the frequency compensated Rx chain gain value                */
    /* NOTE: This gain value encompasses the complete transfer of power */
    /* from the input of the antenna to baseband.                       */
    /* ---------------------------------------------------------------- */
    fc_gain = rfgsm_nv_rx_freq_comp( rfm_dev, RFNV_GSM_LNA_STATE_1, arfcn_index );
    gain->valx16 = fc_gain;
  
    /* Specify the window of valid Rx powers for this gain range */
    gain->range_high_limitx16 = actual_switch_1_2 + RF_GAIN_RANGE_SPEC_MAX_ERROR_dBx16;
    gain->range_low_limitx16  = (int16)BSP_RX_POWER_LOWER_LIMIT_dBm_x16;
    gain->range               = GAIN_RANGE_1;  
  }

  else if ( rx_lvl_dbm < actual_switch_2_3)
  {
    fc_gain = rfgsm_nv_rx_freq_comp( rfm_dev, RFNV_GSM_LNA_STATE_2, arfcn_index );
    gain->valx16   = fc_gain;

    /* Specify the window of valid Rx powers for this gain range */
    gain->range_high_limitx16 = actual_switch_2_3 + 
                                  RF_GAIN_RANGE_SPEC_MAX_ERROR_dBx16;
    gain->range_low_limitx16 = actual_switch_1_2 - 
                                  RF_GAIN_RANGE_SPEC_MAX_ERROR_dBx16;
    gain->range               = GAIN_RANGE_2;
  } 
  else if ( rx_lvl_dbm < actual_switch_3_4)
  {
    fc_gain = rfgsm_nv_rx_freq_comp( rfm_dev, RFNV_GSM_LNA_STATE_3, arfcn_index );
    gain->valx16 = fc_gain;

    /* Specify the window of valid Rx powers for this gain range */
    gain->range_high_limitx16 = actual_switch_3_4 + 
                                  RF_GAIN_RANGE_SPEC_MAX_ERROR_dBx16;
    gain->range_low_limitx16  = actual_switch_2_3 - 
                                  RF_GAIN_RANGE_SPEC_MAX_ERROR_dBx16; 
    gain->range               = GAIN_RANGE_3;
  } 
  else
  {
    if( rfgsm_core_handle_ptr->rf_has_five_gainranges )
    {
      if( rx_lvl_dbm < actual_switch_4_5 )
      {
        fc_gain = rfgsm_nv_rx_freq_comp( rfm_dev, RFNV_GSM_LNA_STATE_4, arfcn_index );
        gain->valx16 = fc_gain;

         /* Specify the window of valid Rx powers for this gain range */
        gain->range_high_limitx16 = actual_switch_4_5 + 
                                      RF_GAIN_RANGE_SPEC_MAX_ERROR_dBx16;
        gain->range_low_limitx16 = actual_switch_3_4 - 
                                     RF_GAIN_RANGE_SPEC_MAX_ERROR_dBx16; 
        gain->range               = GAIN_RANGE_4;
      } 
      else
      {
        fc_gain = rfgsm_nv_rx_freq_comp (rfm_dev, RFNV_GSM_LNA_STATE_5, arfcn_index);
         gain->valx16 = fc_gain;

         /* Specify the window of valid Rx powers for this gain range */
         gain->range_high_limitx16 = BSP_RX_POWER_UPPER_LIMIT_dBm_x16;
         gain->range_low_limitx16 = actual_switch_4_5 - 
                                      RF_GAIN_RANGE_SPEC_MAX_ERROR_dBx16; 
         gain->range               = GAIN_RANGE_5;
      }
    } 
    else /* if !rf_has_five_gainranges */
    {
      fc_gain = rfgsm_nv_rx_freq_comp( rfm_dev, RFNV_GSM_LNA_STATE_4, arfcn_index );
      gain->valx16 = fc_gain;

       /* Specify the window of valid Rx powers for this gain range */
      gain->range_high_limitx16 = actual_switch_4_5 + 
                                            RF_GAIN_RANGE_SPEC_MAX_ERROR_dBx16;
      gain->range_low_limitx16 = actual_switch_3_4 - 
                                            RF_GAIN_RANGE_SPEC_MAX_ERROR_dBx16; 
      gain->range               = GAIN_RANGE_4;
    }
  }
 
  rfgsm_core_handle_ptr->lna_range = (uint8)gain->range;

  if(rfgsm_core_handle_ptr->rx_debug_logging_enable == TRUE)
  {
    MSG_7(MSG_SSID_RF, MSG_LEGACY_HIGH,"Rx Debug Logging: Rx_lvl_dbm: %d, Gain_range: %d, fc_gain: %d,"
                                       "LNA Switch Points - 1_2: [%d], 2_3: [%d], 3_4: [%d], 4_5: [%d]",
                                       rx_lvl_dbm,
                                       gain->range,
                                       fc_gain,
                                       actual_switch_1_2,
                                       actual_switch_2_3,
                                       actual_switch_3_4,
                                       actual_switch_4_5);
  }

}

#ifdef FEATURE_RF_ASDIV
/*----------------------------------------------------------------------------*/
/*!
  @details
   This function add antenna switch script to Rx Burst if needed.
   Then the processed command is discarded.

  @param rfm_dev : the RF device in use.
  @param probe burst : a boolean to know whether we need to swap antenna position
                       during MON burst
  @param start_buffer : pointer to the script to append.
  @param stop_buffer : pointer to the script to append.
*/
static void rfgsm_core_rx_asd_processing(rfgsm_core_data_type *core_data, rfgsm_rx_burst_type rx_burst_type, rf_burst_type burst, rf_buffer_intf* start_asd_buffer, rf_buffer_intf* stop_asd_buffer,
                                  rf_buffer_intf* start_tuner_buffer, rf_buffer_intf* stop_tuner_buffer)
{
   
   rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
   rfcommon_asdiv_asm_conifg_type get_asm_config;
   rfcommon_asdiv_position_type current_pos;
   rfcommon_asdiv_send_script_type send_script_cb;
   rfcommon_asdiv_switch_start_notify_type switch_start_cb;
   rfcommon_asdiv_update_tuner_type update_tuner_cb;
   rfgsm_asdiv_xsw_script_data xsw_script_data;   
   uint8 command;
   int16 tuner_start_delta;
   rfm_device_enum_type rfm_dev = core_data->rfm_dev;   
   
   rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

   if ( rfgsm_core_handle_ptr == NULL )
   {
     MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!!! for device %d ", rfm_dev, 0);
     return;
   }
     
   if(!rfc_gsm_get_timing_info(rfm_dev, rfgsm_core_handle_ptr->band, RFDEVICE_TUNER_MANAGER, RFC_TIMING_TUNER_CTL, &tuner_start_delta))
   {
     /* In the event of an error, set the start delta to a default */
     tuner_start_delta = RFC_GSM_DEFAULT_ANTENNA_TUNER_START_DELTA;
     MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RX ASD Processing: tuner_start_delta is set to default of -150 us!!! for device %d ", rfm_dev);
   }   

   xsw_script_data.device = rfm_dev;
   xsw_script_data.band = rfgsm_core_handle_ptr->band; 
   xsw_script_data.immediate = FALSE; /* FALSE = Create script (and not execute switch immediately) */
   xsw_script_data.start_delta = tuner_start_delta;   /* Start delta used for both tuner and xsw */

   command = rfgsm_core_handle_ptr->set_antenna_position;
   get_asm_config.cb_func = rfc_gsm_append_asdiv;
   get_asm_config.cb_data = &xsw_script_data;
   send_script_cb.cb_func = NULL;
   send_script_cb.cb_data = NULL;
   switch_start_cb.cb_func = NULL;
   switch_start_cb.cb_data = NULL;
   update_tuner_cb.cb_func = NULL;
   update_tuner_cb.cb_data = 0;
   
   current_pos = rfcommon_asdiv_get_current_position(rfm_dev);   

   if ((command != RF_INVALID_VALUE_8BIT) && (current_pos != (uint32)command))
   {
      if (rx_burst_type == RFGSM_RX_PROBE_BURST)
      {

        if(!rfcommon_asdiv_manager_config_switch(rfm_dev, RFM_EGSM_MODE, ((command == 0)? 1 : 0), 
                                              start_asd_buffer, start_tuner_buffer, get_asm_config, send_script_cb, switch_start_cb, update_tuner_cb,
                                              NULL, NULL, FALSE, FALSE, tuner_start_delta, 0))
        {
           MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"Switch and tuner scripts were not built successfully ", 0, 0);
        }

        if(!rfcommon_asdiv_manager_config_switch(rfm_dev, RFM_EGSM_MODE, command, 
                                              stop_asd_buffer, stop_tuner_buffer, get_asm_config, send_script_cb, switch_start_cb, update_tuner_cb,
                                              NULL, NULL, FALSE, FALSE, 0, 0))
        {
           MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"Tuner and switch scripts were not built successfully ", 0, 0);
        }
        rfgsm_core_handle_ptr->configure_asd_scripts = FALSE;
	/* ASD response check error timer start */
	rfgsm_core_asd_rsp_check_start(rfm_dev);
      }
      else
      {
         if(!rfcommon_asdiv_manager_config_switch(rfm_dev, RFM_EGSM_MODE, command, 
                                              start_asd_buffer, start_tuner_buffer, get_asm_config, send_script_cb, switch_start_cb, update_tuner_cb,
                                              NULL, NULL, FALSE, FALSE, tuner_start_delta, 0))
         {
            MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"Tuner and switch scripts were not built successfully ", 0, 0);
         }
      }   
      rfgsm_core_handle_ptr->configure_asd_scripts = FALSE;
      rfgsm_core_handle_ptr->set_prev_ant_pos = FALSE;
      /* ASD response check error timer start */
      rfgsm_core_asd_rsp_check_start(rfm_dev);
   }
   else
   {
      if (rx_burst_type == RFGSM_RX_PROBE_BURST)
      {
         /* Boolean value TRUE (for gen_script_only) makes sure script is generated but "switch_in_progress" flag is not set */
         if(!rfcommon_asdiv_manager_config_switch(rfm_dev, RFM_EGSM_MODE, ((current_pos == 0)? 1 : 0), 
                                              start_asd_buffer, start_tuner_buffer, get_asm_config, send_script_cb, switch_start_cb, update_tuner_cb,
                                              NULL, NULL, TRUE, FALSE, tuner_start_delta, 0))
         {
            MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"Tuner and switch scripts were not built successfully ", 0, 0);
         }

         if(!rfcommon_asdiv_manager_config_switch(rfm_dev, RFM_EGSM_MODE, current_pos, 
                                                  stop_asd_buffer, stop_tuner_buffer, get_asm_config, send_script_cb, switch_start_cb, update_tuner_cb,
                                                  NULL, NULL, TRUE, FALSE, 0, 0))
         {
            MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"Tuner and switch scripts were not built successfully ", 0, 0);
         }      
      }
      /* Immediately after wakeup, we would need to just get the ASD GRFC scripts. The tuner scripts are already obtained in the 
         rfgsm_mc_tune_to_chan() as ASDiv manager already has the correct switch position updated */
      if(rfgsm_core_handle_ptr->set_prev_ant_pos)
      {
        rfc_gsm_append_asdiv(current_pos, start_asd_buffer, get_asm_config.cb_data);
        rfgsm_core_handle_ptr->set_prev_ant_pos = FALSE;
      }
   }
}
#endif

/*----------------------------------------------------------------------------*/
/*!
  @details
  This function clears the WTR RX burst scripts

  @param rfm_dev : rfm device
 
  @retval True/False
*/

boolean rfgsm_core_clear_rx_burst_device_scripts(rfm_device_enum_type rfm_dev )
{
  rfgsm_core_handle_type *rfgsm_core_handle_ptr;
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr,"rfgsm_core_handle_ptr is NULL!", FALSE);
  
  if(!rf_buffer_clear_script( (rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.rx_start_script[RX_WTR_SCRIPT], RF_BUFFER_ALL)||
	 !rf_buffer_clear_script( (rf_buffer_intf *)rfgsm_core_handle_ptr->scripts.rx_stop_script[RX_WTR_SCRIPT], RF_BUFFER_ALL))
  {
    RF_MSG(RF_ERROR,"RF GSM WTR RX START/STOP script buffer was unsuccessfully cleared");
    return FALSE;
  }

  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
   @fn rfgsm_core_populate_device_rx_burst_buffer 
   @brief This function retrieves wtr device driver scripts for GSM Burst and stores them in rfgsm_core_handles pointer
   @param  rfm_dev - RFM device processing for
   @param  burst - Rx, Mon, 
   @retval boolean - success/failure indication
*/
boolean rfgsm_core_populate_rx_burst_device_buffers( rfgsm_core_data_type* core_data,
                                                     boolean enable_rx_diversity,
                                                     rf_burst_type burst)
{
  rfgsmlib_sbi_burst_struct burst_script_infor;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr;
  uint16 buffer_mask;
  rfdevice_operation_mode rxdevice_oper_mode = RFDEVICE_OPER_MODE_STANDALONE ;
  burst_script_infor.burst = burst;
  burst_script_infor.burst_alpha = 625;
  burst_script_infor.scripts = NULL;
  burst_script_infor.override_alpha = FALSE;
  burst_script_infor.burst_timing_offset_adjust_qs = 0;

  /*if GL1 Flag to lower CCS prio in IDLE mode is set then WTR RX burst start time (AutoB writes only) needs to be adjusted*/
  if (core_data->gl1_lower_rx_ccs_prio) 
  {
    burst_script_infor.burst_timing_offset_adjust_qs = RFGSM_WORST_CASE_RFFE_BUS_DELAY;
  }

  /*Populate WTR Burst Scripts based on operating mode Standalone or IRAT*/
  if(core_data->rfgsm_core_handle_ptr->rfgsm_buffer == RFGSM_AWAY_BUFFER)
  {
    rxdevice_oper_mode = RFDEVICE_OPER_MODE_MEAS;
  }
  else
  {
    rxdevice_oper_mode = RFDEVICE_OPER_MODE_STANDALONE;
  }
  /*DRX RX burst Writes are appended to PRX script based on enable_rx_diversity flag*/
  if(!rfdevice_gsm_set_start_script(core_data->rfm_dev, 
                                    rxdevice_oper_mode,
                                    &burst_script_infor, 
                                    (rf_buffer_intf*)core_data->rfgsm_core_handle_ptr->scripts.rx_start_script[RX_WTR_SCRIPT],
                                    &buffer_mask,
                                    enable_rx_diversity,
                                    (rfcom_gsm_band_type)((rf_buffer_intf*)core_data->rfgsm_core_handle_ptr->band)))
  {
    RF_MSG_2(RF_ERROR,"RF GSM WTR RX start script retrieval failed for rfm_dev:|%d|Operating mode:|%d|",
                      core_data->rfm_dev,rxdevice_oper_mode);
    return FALSE;
  }

  /*DRX RX burst Writes are appended to PRX script based on enable_rx_diversity flag*/
  if(!rfdevice_gsm_rx_set_stop_script(core_data->rfm_dev, 
                                     (rf_buffer_intf*)core_data->rfgsm_core_handle_ptr->scripts.rx_stop_script[RX_WTR_SCRIPT],
                                     (rfcom_gsm_band_type)((rf_buffer_intf*)core_data->rfgsm_core_handle_ptr->band)))
  {
    RF_MSG_2(RF_ERROR,"RF GSM WTR RX stop script retrieval failed for rfm_dev:|%d|Operating mode:|%d|",
                      core_data->rfm_dev,rxdevice_oper_mode);
    return FALSE;
  }

  /*Populating Device Status Read Event Scripts During Rx Burst */
  if(core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_RX_BURST].enable)
  {
    rfgsm_core_populate_device_status_read_buffers(core_data, RF_RX_BURST);
  }
  
  rfgsm_core_log(core_data->rfm_dev);

  return TRUE;
} /* rfgsm_core_populate_rx_burst_device_buffer */

/*----------------------------------------------------------------------------*/
/*!
  @details
  This function sets up the GRFC and SBI/uwire buffer required for the
  Rx burst.

  @param burst : burst type
  @param buffer : buffer to use for the burst
*/
void rfgsm_core_setup_rx_burst(rfgsm_core_data_type* core_data,
                               rf_burst_type burst,
                               rfgsm_rx_burst_type rx_burst_type)
{
  rfdevice_rsb_coef_type rfdev_rsb[RFCOM_NUM_GSM_BANDS];
  uint32 rsb_a_coef, rsb_b_coef;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = core_data->rfgsm_core_handle_ptr;

  rfgsm_core_dm_set_rx_gain(core_data, 
                            core_data->rfgsm_core_handle_ptr->rx_burst_type,
                            core_data->rfgsm_core_handle_ptr->rx_burst_num, 
                            core_data->rfgsm_core_handle_ptr->valx16);

  rfdevice_gsm_cmd_dispatch(core_data->rfm_dev, RF_PATH_0, RFDEVICE_GSM_GET_RX_RSB_DATA, &rfdev_rsb[0]);
  /* Two's complement for inversion . RTR returns 17Q15 format coefficients but mdsp requires 16Q14 format.  
     rfgsm_msm_get_bbrx_iq_mismatch_gain gets the value of the coefficients and returns a 16Q14 format   */

#ifdef FEATURE_BOLT_MODEM
  rsb_a_coef = -(rfgsm_msm_get_coefa_bbrx_iqmc_compensated(rfdev_rsb[rfgsm_core_handle_ptr->band].rsb_a_coef));
#else
  rsb_a_coef = -(rfdev_rsb[rfgsm_core_handle_ptr->band].rsb_a_coef);
#endif
  rsb_b_coef = -(rfgsm_msm_get_coefb_bbrx_iqmc_compensated(rfdev_rsb[rfgsm_core_handle_ptr->band].rsb_b_coef, rfgsm_core_handle_ptr->rxlm_buf_index));

  rfgsm_core_dm_set_spectral_inv(core_data,
                                 rfgsm_core_handle_ptr->rx_burst_type,
                                 rfgsm_core_handle_ptr->rx_burst_num, 
                                 rsb_a_coef, rsb_b_coef);

#ifdef FEATURE_RF_ASDIV 
  /* ASD only applies to PRX */
  if( rfcommon_asdiv_is_supported() && rfgsm_core_is_device_prx(core_data->rfm_dev) 
      &&(((rfgsm_core_handle_ptr->set_antenna_position != RF_INVALID_VALUE_8BIT) &&
          (rfgsm_core_handle_ptr->configure_asd_scripts == TRUE ))||
          (rfgsm_core_handle_ptr->set_prev_ant_pos == TRUE)||(rx_burst_type == RFGSM_RX_PROBE_BURST)))  
{
    /* Call ASD processing to update GRFCs and tuner scripts if ASDiv is supported in the card */
    rfgsm_core_rx_asd_processing(core_data, 
                                 rx_burst_type, 
                                 burst, 
                                 (rf_buffer_intf*)rfgsm_core_handle_ptr->scripts.rx_start_script[RX_XSW_SCRIPT], 
                                 (rf_buffer_intf*)rfgsm_core_handle_ptr->scripts.rx_stop_script[RX_XSW_SCRIPT],
                                 (rf_buffer_intf*)rfgsm_core_handle_ptr->scripts.rx_start_script[RX_ATUNER_SCRIPT], 
                                 (rf_buffer_intf*)rfgsm_core_handle_ptr->scripts.rx_stop_script[RX_ATUNER_SCRIPT]);
  }
#endif   

} /* end of rfgsm_setup_rx_burst */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function calls all the rfgsm_core functionality to create the RF CCS for all the
  bursts requested from GL1.

  This function can be called from RF Task context from the rfgsm_cmd_processing dispatcher
  or iRAT scenarios.

  @param core_data                 : RF Multimode device
  @param rx_burst_event_ptr  : Pointer to shared mem
*/
boolean rfgsm_core_build_rx_burst_ccs_events( rfgsm_core_data_type* core_data,
                                              void* rx_burst_event_ptr)
{
  boolean ret_status = FALSE;

  /* Populate the Rx RFLM event data table */
  ret_status = rfgsm_core_dm_dynamic_rx_event_wrapper(core_data,
                                                      core_data->rfgsm_core_handle_ptr->rx_burst_type, /*retrieved from rfgsm_core_handle_ptr*/
                                                      core_data->rfgsm_core_handle_ptr->rx_burst_num);
  if( FALSE == ret_status )
  {
    RF_MSG(MSG_LEGACY_ERROR,"RF GSM Rx RFLM dynamic event prog failed!");
    return ret_status;
  }
      
  ret_status = rfgsm_core_populate_burst_rflm_data_info(core_data,
                                                        core_data->rfgsm_core_handle_ptr->rx_burst, 
                                                        0,
                                                        rx_burst_event_ptr);
  if (FALSE == ret_status)
  {
    RF_MSG(MSG_LEGACY_ERROR,"RF GSM Rx RFLM data info update failed!");
    return ret_status;
  }

  return ret_status;
}
/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function updates the frequency error (in ppb).
   For example, if user passes a value of 2054  (ppb) it corresponds to 2054/1024 = 2.005 ppm. The resolution of frequency error is hence .001 ppm.
   @details

*/
void rfgsm_core_update_rx_freq_error(rfgsm_core_data_type *core_data, int32  freq_err)
{
  core_data->rfgsm_core_handle_ptr->rfgsm_curr_freq_error = freq_err;

  /* update the device layer with gsm freq error information*/
  rfdevice_gsm_cmd_dispatch( core_data->rfm_dev, RF_PATH_0, RFDEVICE_GSM_SET_RX_FREQ_ERROR, &freq_err);
}