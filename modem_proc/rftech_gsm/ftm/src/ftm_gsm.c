/*===========================================================================

               G S M    S P E C I F I C   T E S T  F U N C T I O N S

DESCRIPTION
   This file contains implementations for GSM specific test functions

   Copyright (c) 2001, 2002 by Qualcomm Technologies Incorporated.  All Rights Reserved.
   Copyright (c) 2007, 2012 by Qualcomm Technologies Incorporated.  All Rights Reserved.
   Copyright (c) 2013, 2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.
   Copyright (c) 2015       by Qualcomm Technologies Incorporated.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  Initially, comments will be entered manually. Later, as the module is
  placed under version control, the version control tool will automatic-
  ally insert change comments into this section. [Delete this comment].

  
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/ftm/src/ftm_gsm.c#1 $ 
  $DateTime: 2016/12/13 07:59:51 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/08/15   par     Add new API for checking if in Cal mode
10/23/14   sc      Enable event type control from RFSW 
03/31/14   tsr     Add support for GSM RX Diversity
05/13/13   tsr     Fix and Enable Multislot RX Calibration 
04/16/13   svi     Replaced memcpy() by safer memscpy().  
03/28/13   aka     GSM frame sweep feature changes.
11/27/12   tws     Make global variables device dependent.
11/26/12   aka     Changes to use a path specific gsm handle
11/23/12   tws     Remove deprecated GERAN includes.
11/21/12   av      Added rfm_device parameters to mc/core/mdsp/msm layers.
11/19/12   sc      RF-Only mDSP update shared variables cleanup
11/13/12   ec	   Update to use new geran api for DSDA  
06/13/12   nv      Added API's to set GSM TX freq offset
05/11/12   sb      Added changes for debug 
04/01/12   lcl     Removed empty mdsp interface header file 
03/23/12   sr      Removed the FEATURE_GSM_MDSP_QDSP6 featurization.
02/28/12   dej     Hooked up PA start/stop delta adjustments in FTM mode
03/17/12   jfc     Remove deprecated mdsp_intf_v.h include
02/15/12   sar     Removed FTM_HAS_13BIT_GSM_PCDAC and FEATURE_FTM_GSM_DEBUG.
09/14/11   can     Merge MDM9K tips to NikeL.
08/02/11   sr      Made changes to mdsp_intf type for the amam table
04/29/11   av      Change in ftm_gsm_rx_disable(). The wait is now on the task that 
                   is running: FTM or RF task. Takes care of crash between G to L FTM
                   transition
03/11/11   sr      Removed un-used featurization
31/01/11   sr      Removed un-used code
01/25/11   sr      removed the clk.h dependency
08/12/10    rsr    Removing reference to unused Headers for QDSP6 targets.
06/15/10    rsr     Picking up Temp Comp changes from QSC6x95
03/19/10   lcl     Pull in changes from QS6295
02/04/10   sr      Lint & kw fixes 
01/06/10   sar     Renamed ftm_task.h to rf_test_task_v.h.
12/08/09   sr      added func prototype to fix warning.
10/26/09   sr      Compiler warnings fix
09/17/09   sar     Updated clk interface for CMI conformance.
07/27/09   rsr     CR181048 fix.
03/31/09   sr      include file cleanup
10/25/08   et      warnings
08/08/07   jfc     Mainlined T_MSM6275 feature
08/08/07   jfc     Mainlined T_MSM6280 feature
07/13/06    xw     Added 7200 support. 
03/28/06   rsr     Lint cleanup. Changed RX burst API to accept the number of
                   frames over which RSSI is averaged.
03/09/06    ka     Merged in 6280 changes.
05/11/05    rv     Fixed compilation errors with new GL1 interface.
05/04/05   rsr     Removed warnings.Cleanup
03/04/05   rsr      Added ftm_tx_stop_funct. Added support for OPLL transmit waveform.
02/09/05   rsr/lcl  Renamed FEATURE_POLAR_RF to FEATURE_POLAR
01/26/05   lcl     Changed interface to set PA ramp and PA transition.
12/01/04    xw     Updated ftm_gsm_dvt_set_pa_lut() for Raven.
08/18/03   lcl     Added support for 850 and 1900 bands.
08/12/03   lcl     Added ftm_gsm_rx_disable() for WCDMA to turn off GSM 
                   receive bursting before transition to WCDMA mode.
7/21/03     rsr    Cleaned up unnecessary variables. 
7/18/03     rsr    Cleaned up set_tx_power_level functions.
4/11/03     rsr    Renamed all files with "ftm_" prefix
4/9/03      rsr    Removed setup_burst , select_tx_burst , select_rx_burst , select_tx_cont.
             Cleaned up unused variables and removed duplicate function prototypes/declarations.
3/06/02     rsr    Put in fix to remove invalid sequence number error when reading RSSI.
11/08/02    rsr    Removed support for B1 internal VCO.
           Removed DVT option support to switch between 4/3 and 5/4 division
           plan for the PLL.
           Changed file to use rfgsm_pt.[ch] (RF API provided by Ian) 
07/11/01    ai     Added TX command support
05/25/01    ai     Implemented basic functionality (GRFC) using ASW MDSP drivers
05/21/01    ai     Stripped non-ASW supported MDSP stuff
02/05/01   avi     Created.

=============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"
#include "ftm_gsm_prot.h"
#include "ftm_gsm_rfprot.h"
#include "mdsp_intf.h"
#include "time_svc.h"   /* Replaces clk.h*/
#include "time_tod.h"   /* Replaces clk.h*/
#include "ftm_gsm.h"
#include "ftm_gsm_rfctl.h"
#include "stringl.h"

/*The following two includes reqd for RF_MAX_TX_script_entries macro defn.*/
#include "ftm.h"
#include "rf_test_task_v.h"
#include "rfgsm_nv_cmn.h"
#include "rfgsm_core_timing_consts.h"
#include "rfgsm_core_handle.h"
#include "rfgsm_core.h"
#include "gl1_hw.h"


/*============================================================================
             LOCAL/EXTERNAL VARIABLES                                                      
=============================================================================*/
/* Pre-stored table of PA LUT's for all power levels and GSM systems */




/* pointer to debug function */
boolean (*ftm_debug_funct)(uint8) = NULL;

/*===========================================================================
             FUNCTION IMPLEMENTATIONS
===========================================================================*/


/*===========================================================================

FUNCTION FTM_GSM_TX_BURST_COMMAND

DESCRIPTION
  Parameters:
  - pointer to ftm_tx_burst_type structure

  Issues the TX burst command with the specified source for TX modulation data

DEPENDENCIES
    PA LUT is supposed to be setup. SBI clock must be setup. VCTCXO is
    assumed to have been tuned

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_gsm_tx_burst_command( rfm_device_enum_type rfm_dev,
                               const ftm_tx_burst_type* ftm_tx_burst)
{
  ftm_gsm_rfctl_vars_s * vars_ptr = ftm_gsm_get_vars_ptr(rfm_dev);

  memscpy( (void *) &vars_ptr->tx_brst, sizeof(vars_ptr->tx_brst), (void *) ftm_tx_burst, sizeof(ftm_tx_burst_type));
  
  /* install the TX burst processing function pointer */
  if ( (ftm_enum_tx_data_source_type) ftm_tx_burst -> data_source == FTM_TX_DATA_SOURCE_PSDRND ||\
       (ftm_enum_tx_data_source_type) ftm_tx_burst -> data_source == FTM_TX_DATA_SOURCE_BUFFER ||\
       (ftm_enum_tx_data_source_type) ftm_tx_burst -> data_source == FTM_TX_DATA_SOURCE_TONE\
     )
  {
    vars_ptr->ftm_tx_funct = ftm_gsm_do_tx_burst_buffer_data;
    vars_ptr->ftm_tx_stop_funct = NULL;
  }
  else
  {
    vars_ptr->ftm_num_bursts_tx = 0L;
    vars_ptr->ftm_cont_tx = OFF;
  } 

  /* specify the number of bursts to TX */
  if ( vars_ptr->ftm_cont_tx == ftm_tx_burst -> infinite_duration )
  {   /* turn ON continuous bursting */
    vars_ptr->ftm_num_bursts_tx = 0;
  }
  else
  {  /* turn OFF continuous bursting and set a number of bursts */
    vars_ptr->ftm_num_bursts_tx = ftm_tx_burst -> num_bursts;
  }
}

/*===========================================================================

FUNCTION FTM_GSM_TX_CONT_COMMAND

DESCRIPTION
  Parameters:
  - pointer to ftm_tx_burst_type structure

  Issues the continuous TX START or STOP command with the specified
  source for TX modulation data (tone, PN sequence, or random buffer data

DEPENDENCIES
    PA LUT is supposed to be setup. SBI clock must be setup. VCTCXO is
    assumed to have been tuned

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_gsm_tx_cont_command( rfm_device_enum_type rfm_dev,
                              const ftm_tx_burst_type* ftm_tx_burst)
{
  ftm_gsm_rfctl_vars_s * vars_ptr = ftm_gsm_get_vars_ptr(rfm_dev);

  memscpy( (void *) &vars_ptr->tx_brst, sizeof(vars_ptr->tx_brst), (void *) ftm_tx_burst, sizeof(ftm_tx_burst_type));
  
  switch ( (ftm_enum_tx_data_source_type) ftm_tx_burst -> data_source )
  {
  case FTM_TX_DATA_SOURCE_BUFFER:
  case FTM_TX_DATA_SOURCE_PSDRND:
    vars_ptr->ftm_tx_funct = ftm_gsm_do_tx_cont_rnd;
    vars_ptr->ftm_tx_stop_funct = ftm_gsm_do_tx_stop_cont;
    break;

  case FTM_TX_DATA_SOURCE_TONE:
    vars_ptr->ftm_tx_funct = ftm_gsm_do_tx_cont_tone;
    vars_ptr->ftm_tx_stop_funct = ftm_gsm_do_tx_stop_cont;
    break;

    /* unknown data source, do not TX at all */
  default:
    vars_ptr->ftm_num_bursts_tx = 0L;
    vars_ptr->ftm_cont_tx = OFF;
    break;
  }
}

/*===========================================================================

FUNCTION FTM_GSM_RX_BURST_COMMAND

DESCRIPTION
  Parameters:
  - pointer to ftm_rx_burst_type structure
  - Num of frames over which to average the RSSI.
    The average result is stored in ftm_gsm_state.ftm_rssi.

  Configures GRFC, SBI , and uWire buffers associated with the detection of
  an RX burst

DEPENDENCIES
    SBI clock must be setup. VCTCXO is assumed to have been tuned

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_gsm_rx_burst_command( rfm_device_enum_type rfm_dev,
                               const ftm_rx_burst_type* ftm_rx_burst, 
                               uint8 num_frames_avg )
{
  ftm_gsm_rfctl_vars_s * vars_ptr = ftm_gsm_get_vars_ptr(rfm_dev);

  /* Only start/stop Rx when it's necessary                        */
  /*                                                               */
  /* Is burst on? To turn on/off burst    Action                   */
  /*===============================================================*/
  /* no         turn off          no action                        */
  /* no         turn on           turn on                          */
  /* yes        turn off          turn off                         */
  /* yes        turn on           no action                        */

  switch ( (ftm_enum_rx_function_type) ftm_rx_burst -> ftm_rx_funct )
  {
  
  case FTM_RX_FUNCTION_GET_DC_OFFSETS:
  case FTM_RX_FUNCTION_GET_RSSI:
    ftm_gsm_set_no_frame_avg( rfm_dev, num_frames_avg);
    if ( vars_ptr->rxBurstOn == FALSE )
    {
      /* Setting up flag to count three frames after which RSSI will become available*/
      vars_ptr->beginning_rx = 0;
      vars_ptr->rx_slot_num = ftm_rx_burst->slot_num;
      vars_ptr->stop_rx = FTM_GSM_MIN_RX_FRAME_CNT;
      vars_ptr->ftm_rx_funct = ftm_gsm_do_rx_cmd_rx_burst_metrics;
      vars_ptr->ftm_num_bursts_rx = 0L;
      vars_ptr->ftm_cont_rx = ON;

      /* The burst is on */
      vars_ptr->rxBurstOn = TRUE;

	  ftm_gsm_set_ave_rssi_update_flag( rfm_dev, FALSE );
    }
    break;

  case FTM_RX_FUNCTION_SINGLE_BURST:
    ftm_gsm_set_no_frame_avg( rfm_dev, num_frames_avg);
    if ( vars_ptr->rxBurstOn == FALSE )
    {
      /* Setting up flag to count three frames after which RSSI will become available*/
      vars_ptr->beginning_rx = 0;
      vars_ptr->stop_rx = FTM_GSM_MIN_RX_FRAME_CNT;
	  
      vars_ptr->ftm_rx_funct = ftm_gsm_do_rx_burst_processing;
      vars_ptr->ftm_num_bursts_rx = 0L;
      vars_ptr->ftm_cont_rx = ON;

      /* The burst is on */
      vars_ptr->rxBurstOn = TRUE;

      ftm_gsm_set_ave_rssi_update_flag( rfm_dev, FALSE );
    }
    break;

  case FTM_RX_FUNCTION_RF_RX_HW_ON:
    if ( vars_ptr->rxBurstOn == TRUE )
    {
      /*If Rx is currently bursting , turn it off before doing cont rx.*/
      /* Setting up counter to count two frames during which the 
         RSSI values of previous frames will have to be read. We don't need 
         to have the RSSI but we read it to keep the MDSP RSSI read locn. and 
         RSSI write locn. pointers in sync, 
      */
      vars_ptr->stop_rx = 0;
      
      /* Force RSSI reading to zero */
      vars_ptr->ftm_gsm_state.ftm_rssi = 0;
      vars_ptr->ftm_gsm_state.ftm_rssi_drx = 0;
      /* The burst is off */
      vars_ptr->rxBurstOn = FALSE;

      /*Wait 3 frames for the rx to turn off and rssi to get settled*/
      ftm_clk_rex_wait(20);

    }
    /*Turn on cont rx */
    vars_ptr->ftm_rx_funct = ftm_gsm_do_rx_cmd_rx_cont;
    vars_ptr->ftm_num_bursts_rx = 0L;
    vars_ptr->ftm_cont_rx = ON;
    vars_ptr->beginning_rx = 0;
    vars_ptr->rxBurstOn = TRUE;
    break;

  case FTM_RX_FUNCTION_STOP_CONTINOUS_RX:
  case FTM_RX_FUNCTION_RF_RX_HW_OFF:
    /* Turn off the channel filter when done */
    /* --------------------------------------------------------------------
    **          Establish Miscellaneous Control Settings
    ** --------------------------------------------------------------------
    */
    if ( vars_ptr->rxBurstOn == TRUE )
    {
      /* Setting up counter to count two frames during which the 
         RSSI values of previous frames will have to be read. We don't need 
         to have the RSSI but we read it to keep the MDSP RSSI read locn. and 
         RSSI write locn. pointers in sync, 
      */
      vars_ptr->stop_rx = 0;

      /* Force RSSI reading to zero */
      vars_ptr->ftm_gsm_state.ftm_rssi = 0;
      vars_ptr->ftm_gsm_state.ftm_rssi_drx = 0;

      /* The burst is off */
      vars_ptr->rxBurstOn = FALSE;

    }
    break;

    /* unknown RX function, do not RX at all */
  default:
    vars_ptr->ftm_num_bursts_rx = 0L;
    vars_ptr->ftm_cont_rx = OFF;
    break;
  }
}


/*===========================================================================

FUNCTION FTM_GSM_RX_DISABLE

DESCRIPTION
    This function disable the GSM receive bursting.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_gsm_rx_disable( rfm_device_enum_type rfm_dev )
{
  ftm_rx_burst_type rxDisable;

  rxDisable.ftm_rx_funct = (enum_type)FTM_RX_FUNCTION_STOP_CONTINOUS_RX;

  ftm_gsm_rx_burst_command( rfm_dev, &rxDisable, 0 );

  rf_ftm_sig_rex_wait(20);

} /* ftm_gsm_rx_disable */


/*===========================================================================

FUNCTION FTM_GSM_SET_PA_START_DELTA

DESCRIPTION
   This function Modifies the pa start time
   
DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   It modifies pa_start_delta used by ftm_gsm_prepare_tx_burst()

===========================================================================*/
void ftm_gsm_set_pa_start_delta( rfm_device_enum_type rfm_dev,
                                 int16 delta )
{
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  rfgsm_core_handle_ptr = rfgsm_core_handle_get( rfm_dev );

  RF_NULL_CHECK_RTN_V(rfgsm_core_handle_ptr,"rfgsm_core_handle_ptr is NULL!");

  //allow for the change to be affected in the gsm_test_prepare_tx_burst/cont
  rfgsm_core_handle_ptr->rf_pa_start_time = delta;
  rfgsm_core_handle_ptr->tx_timing_adjust_flag = TRUE;
}


/*===========================================================================

FUNCTION FTM_GSM_SET_PA_STOP_DELTA

DESCRIPTION
   This function Modifies the pa stop time
   
DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   It modifies pa_start_delta used by ftm_gsm_prepare_tx_burst()

===========================================================================*/
void ftm_gsm_set_pa_stop_delta( rfm_device_enum_type rfm_dev,
                                int16 delta )
{
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN_V(rfgsm_core_handle_ptr,"rfgsm_core_handle_ptr is NULL!");
  
  //allow for the change to be affected in the gsm_test_prepare_tx_burst/cont
  rfgsm_core_handle_ptr->rf_pa_stop_time = delta;
  rfgsm_core_handle_ptr->tx_timing_adjust_flag = TRUE;
}

/*===========================================================================

FUNCTION ftm_gsm_is_in_calibration_mode

DESCRIPTION
  This function checks if device in ftm calibration state

DEPENDENCIES
  ftm calibration mode should be explicity set by tools during GSM calibration

RETURN VALUE
  boolean  TRUE/FALSE

SIDE EFFECTS
  None

==================================================================*/
boolean ftm_gsm_is_in_calibration_mode(void)
{ 
  if ( ftm_mode == FTM_MODE  && 
       rfm_get_calibration_state()== TRUE)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

