/*!
   @file rfgsm_core_temp_comp.c

   @brief

   @details

*/


/*===========================================================================
Copyright (c) 2008 - 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/rf/core/src/rfgsm_core_temp_comp.c#1 $
$DateTime: 2016/12/13 07:59:51 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/17/16   ds      Added code for temparature compensation optimization.
12/23/15   bp      Stop further processing of therm_read_rsp if temp. compensation is stopped through GSM Tx Disable.
01/27/15   piy     Logging Rx burst RxAGC info and disabling therm reads via debug NV
01/16/14   ec      Use new debug NV mechanism to initialise temp comp logging flag
11/04/14   zbz     Fix temp comp offset
10/30/14   zbz     Correctly compute temp comp offset and indepedently apply for each PCL
09/11/14   sw      use mid-bin temperature if initial therm read is invalid
08/30/14   sw      Remove redundant call to rfgsm_core_therm_read_cleanup() API in rfgsm_core_process_therm_read_rsp()
08/14/14   zbz     Add SMPS/ICQ/temp_rem to FTM log packet
07/01/14   jr      Enable therm read on sub-2 
06/20/14   zbz     Add feature to adjust temp comp table based on therm read during cal
06/12/14   sw      Use GL1 api to map sub_id to as_id
06/12/14   ry      Backout changes to Prevent simultaneous therm read triggers 
05/29/14   ry      Fix errors in previous check-in
05/29/14   ry      Prevent simultaneous therm read triggers 
04/10/14   lcl     Fix Klocwork errors/warnings
03/13/14   sw      Don't increment event_handle_tag when therm read is cancelled
02/28/14   sc      Pass core data through mc to core layers
02/17/14   sc      Clean up mDSP APIs
02/17/14   sw      Remove dev-id and handle-tag from msg sent to GFW
02/11/14   sw      Remove therm read TRITON featurisation
02/06/14   sw      Therm read requires two REG reads
01/17/14   sw      Introduce thermistor read hooks
01/15/14   sc      Remove unused APIs using FW shared memory
10/31/13   sc      Feature-guard FW shared mem buffer population
08/06/13   par     Free therm CCS event handles post a therm read cancellation
07/30/13   pl      Enable new therm read interface
07/29/13   sml     Generic RFSW-FW interaction to generate an ACK for a CCS event
07/24/13   ec      Fix Offtarget Error
07/22/13   ec	   Use new F3s/Null pointer checks
07/12/13   av      GTA EXIT mode
06/25/13   ec      Make changes to support Therm Read Cancellation
06/05/13   svi     Adding Support of Enhanced Temperature Compensation in GSM.
05/30/13   tws     Correct the prototype of rfgsm_core_therm_read_trigger().
                   Pass the UMID for GFW therm_read trigger to MDSP HAL.
04/16/13   sb      Remove headroom table initialization from rfgsm_core_temp_comp_data_init
04/09/13   tsr     Added RF APPS->RF Task MSGR framework for temp comp to make temp comp data thread safe
03/28/13   tsr     Fixed offtarget complier error from previous checkin
03/25/13   tsr     Use device driver api to extract therm read from retrieved ssbi buffer 
03/20/13   ec      Replace deprecated RFM_MAX_DEVICES with RFM_MAX_WAN_DEVICES
03/12/13   tsr     Cleanup the therm read event handle before rfgsm exit
03/12/13   tsr     Added a check for temp comp start flag before therm read schedule
02/28/13   tws     Move temp and vbatt read timers to core handles.
02/13/13   tsr     Ensure that Temp comp calculation is thread safe
02/12/13   sr      changes to check ptrs before we access it. 
02/06/13   tsr     Added GSM temp comp algorithm implementation
01/31/13   tsr     Added framework for sending therm read process cmd to GFW  
01/18/13   tws     Fix KW error. Remove include msm.h
01/17/13   tsr     Fix offtarget compiler errors from Last check-in 
01/14/13   tsr     Added changes for Therm read implementation 
12/17/12   aka     Removing legacy GSM NV support
01/09/13   sr      changes to make temp comp to run in rf app task context. .
11/26/12   aka     Changes to use a path specific gsm handle
11/21/12   av      Added rfm_device parameters to mc/core/mdsp/msm layers.
10/01/12   sb      Made changes to add rfm device to rf device interface and uppler layer APIs 
08/28/12   sb      Disable therm read for Dime bring up
06/11/12   jps     Increased thermistor read frequency to 2 seconds from 10 seconds 
01/28/12   sar     Feature reduction changes.
01/10/12   sr      changes to enable vbatt comp.
10/21/11   dej     Port function to initialize baseband headroom table
08/07/11   av      Port Enhanced temp comp changes
08/19/11   sr      Port the fix incorrect index calculation/bin saturation 
08/15/11   sr      klockwork fixes.
07/05/11   vrb     Remove unnecessary typacast to device intf function
07/01/11   vrb     Replacing direct calls to rfgsm_ds with wrapper functions
03/23/11    sr     temp comp code cleanup.
03/03/11   sr     off-target error fix.
03/03/11    sr     merge from mdm9k branch
01/28/11   bmg     Changed clk_def to timer_def2 to complete timer changes
01/25/11   sr      removed the clk.h dependency
12/03/10    tsr    Set the rfgsm_core_do_temp_comp_calc to False if temp comp stop is called
11/30/10    ap     Modify F3 messages for Voltage Scaling Update during Therm Reads 
11/01/10   lcl     Reduce F3 via NV107
10/13/10    ap     Change call flow for Voltage Scaling Update during Therm reads 
10/11/10   lcl     Changed MSG_LVL to MSG_LEGACY to avoid too many F3 messages
09/20/10    ap     Fix bug in Temp Comp support for Voltage Scaling 
08/20/10    av     Fix compiler error
08/20/10    av     Fix compiler warning
08/20/10    ka     Add missing header files 
08/20/10    ap     Added Temp Comp Support for Enhanced Voltage Scaling 
08/18/10   lcl     Pick up temp comp from QSC6x95: Set default bins as mid-bins
06/15/10   rsr     Pick up temp comp changes from QSC6x95. Specifically picked up the following.
                   [05/02/10  jps  Temp. comp. cleanup 
                    03/29/10  jps  Adding power backoff for max PCL for GMSK only 
                    02/04/10   sr      Lint & kw fixes 
                    01/28/10   sar     Removed therm.h from CMI builds.
                    01/21/10   jps     temp comp updates 
                    01/20/10   sr      Removed un-used code.]  
03/19/10   lcl     Pull in some changes from QSC6295
12/28/09   sr      High/Medium lint errors fix
12/22/09   sr      changed the debug msg macro from legacy to new one.
12/22/09   jps     Removed unused code 
12/17/09   jps     Updated THERMISTOR_SCALE to 2^12 
12/08/09   sr      removed un-used code.
12/05/09   sr      optimized finding of temp_bin_index.
11/30/09   jps     Enabled therm_read function, added more APIs  
10/26/09   sr      Compiler warnings fix
10/12/09   sr      Fixed warnings.
09/17/09   sar     Updated clk interface for CMI conformance.
07/06/09    sr     changes to make the operating band to rfcom_gsm_band type
06/26/09   sr      Function name changed
04/02/09   sr      removed duplicate function definition
03/16/09   sr      added freq comp delay offset calc functions.
03/13/09   sr      code cleanup for SCMM build
10/30/08   sr      Initial revision.

============================================================================*/

#include "rfa_variation.h"
#include "rfcom.h"
#include "msg.h"
#include "time_svc.h"   /* Replaces clk.h*/
#include "time_tod.h"   /* Replaces clk.h*/
#include "rfgsm_nv_cmn.h"
#include "rfgsm_core.h"
#include "rfgsm_core_types.h"
#include "rfgsm_core_util.h"
#include "rfdevice_gsm_intf.h"
#include "rfcommon_nv_mm.h"
#include "rfc_card.h"
#include "rfgsm_msm.h"
#include "rfgsm_msg.h"
#include "rfm_internal.h"
#include "rfgsm_core_temp_comp.h"
#include "rfgsm_mdsp.h"
#include "rfcommon_msg.h"
#include "rfgsm_core_apps_task_processing.h"
#include "rflm_gsm_dm.h"
#include "gl1_hw.h"
#include "rflm_util.h"

#define RF_SATURATE( val, min, max )  MAX( MIN( (val), (max) ), (min) )
#define THERMISTOR_SCALE 255
#define THERMISTOR_RESOLUTION 5
#define RFGSM_CORE_NUM_THERM_READ_REGS 2 
#define RFGSM_CORE_NUM_THERM_READ_SSBI_REGS 3

/* The temperature compensation uses interpolated tables of size
   GSM_PA_TEMP_COMP_TBL_SIZE. The temperature value from the therm services
   is scaled 0..THERMISTOR_SCALE. The bin size between the entries in the
   table is RFGSM_TEMP_BIN_SIZ. Note that this is a scalar, and the
   result of the division can make 1 bin smaller than RFGSM_CORE_TEMP_BIN_SIZ.
*/
#define RFGSM_CORE_TEMP_BIN_SIZ (THERMISTOR_SCALE /GSM_PA_TEMP_COMP_TBL_SIZE)+1  /* + 1 is to round it to 16*/
#define RFGSM_CORE_TEMP_COMP_8_CENTRE_BIN       ( RFGSM_CORE_TEMP_COMP_8_BIN_SIZE >> 1 )
#define RFGSM_CORE_TEMP_COMP_16_CENTRE_BIN      ( RFGSM_CORE_TEMP_COMP_16_BIN_SIZE >> 1 )


#define RFGSM_CORE_ENH_THERM_SCALE            0x0FF
#define RFGSM_CORE_ENH_THERM_MIN              0
#define RFGSM_CORE_ENH_THERM_RANGE (RFGSM_CORE_ENH_THERM_SCALE - RFGSM_CORE_ENH_THERM_MIN + 1)

/*  GSM Tx power compensation w.r.t temperature callback intervals */
static const uint32 RFGSM_CORE_TEMP_COMP_REPEAT_INT = 2000;/* milliseconds*/ 
static const uint32 RFGSM_CORE_TEMP_COMP_START_INT = 2000; /* milliseconds */

#define RFGSM_CORE_INITIAL_THERM_READ   0xFFFF
#define RFGSM_CORE_INVALID_THERM_READ   0x1000
#define RFGSM_CORE_DELTA_OF_THERM_READ	0x0002

static void rfgsm_core_temp_comp_dynamic_range_tbl_init(rfm_device_enum_type rfm_dev);

/*-------------------------------------------------------------------------*/
/*EXTERNAL FUNCTIONS*/
extern void rfgsm_core_set_tx_profile_update_flag( rfgsm_core_handle_type *rfgsm_core_handle_ptr );

extern uint16 rfgsm_nv_get_spare(void);

/*-------------------------------------------------------------------------*/


/* ===========================PUBLIC FUNCTIONS ===============================*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
   
   @details

   @param 

   @retval 
   
*/
boolean rfgsm_core_temp_comp_data_init(rfm_device_enum_type rfm_dev)
{
  rfgsm_core_handle_type *rfgsm_core_handle_ptr;
 
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!", FALSE);

  /*Initialise therm read related variables*/
  rfgsm_core_handle_ptr->therm_read.raw_therm = RFGSM_CORE_INITIAL_THERM_READ;
  rfgsm_core_handle_ptr->therm_read.curr_therm_read = RFGSM_CORE_INITIAL_THERM_READ;
  rfgsm_core_handle_ptr->therm_read.prev_therm_read = RFGSM_CORE_INITIAL_THERM_READ;

 /* Structure to hold temp comp information (default is set to mid-bins) */
  rfgsm_core_handle_ptr->temp_comp.temp_comp_updated = FALSE;
  rfgsm_core_handle_ptr->temp_comp.prev_temp_bin_16 = (RFGSM_CORE_TEMP_COMP_16_BIN_SIZE/2);
  rfgsm_core_handle_ptr->temp_comp.prev_temp_bin_8  = (RFGSM_CORE_TEMP_COMP_8_BIN_SIZE/2);
  rfgsm_core_handle_ptr->temp_comp.temp_bin_16      = (RFGSM_CORE_TEMP_COMP_16_BIN_SIZE/2);
  rfgsm_core_handle_ptr->temp_comp.temp_bin_8       =  (RFGSM_CORE_TEMP_COMP_8_BIN_SIZE/2) ;
  /*This variable is used by RF task for temp comp calc and updated via msg router from rf apps task every therm read*/
  rfgsm_core_handle_ptr->temp_comp.current_temp_bin_16 = (RFGSM_CORE_TEMP_COMP_16_BIN_SIZE/2);

  rfgsm_core_handle_ptr->rfgsm_core_temp_comp_started = FALSE;
  rfgsm_core_handle_ptr->therm_read.rfgsm_core_temp_comp_suspended = FALSE;
  
  /* Check if logging should be enabled based on debug NV */
  rfgsm_core_handle_ptr->therm_read.enable_logging = rfgsm_nv_check_debug_nv(rfm_dev, RFGSM_DEBUG_NV_TEMP_COMP_LOGGING_ENABLE);
  
  rfgsm_core_handle_ptr->rfgsm_core_temp_comp_therm_value = RFGSM_CORE_INVALID_THERM_READ;

  /* Temp comp values for GMSK, *PSK and Unknown modulation schemes */
  rfgsm_core_handle_ptr->rfgsm_core_temp_comp_scaled_value = 0;

  /* Temp Comp is disabled if the Debug NV has been set to disable it*/
  rfgsm_debug_flags.disable_temp_comp = rfgsm_nv_check_debug_nv(rfm_dev, RFGSM_DEBUG_NV_DISABLE_THERM_READ);
  MSG_1(MSG_SSID_RF, MSG_LEGACY_MED,"Therm read is disabled via Debug NV!", 0);

  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize the PA temp comp. clock callback.
*/
void rfgsm_core_temp_comp_init( rfm_device_enum_type rfm_dev )
{
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN_V(rfgsm_core_handle_ptr,"rfgsm_core_handle_ptr is NULL!");

  /* define the PA temp compensation call back */
  timer_def2( &rfgsm_core_handle_ptr->rfgsm_core_temp_comp_clk_cb, NULL );

  rfgsm_core_handle_ptr->therm_read.event_handle_tag = 0;
  rfgsm_core_handle_ptr->therm_read.event_handle = NULL;
  rfgsm_core_handle_ptr->therm_read.therm_read_shared_mem_ptr = NULL;
  rfgsm_core_handle_ptr->therm_read.fw_therm_read_umid = 0;
  rfgsm_core_handle_ptr->therm_read.fw_therm_rsp_umid = 0;
  rfgsm_core_handle_ptr->scripts.process_read_therm_script = NULL;

  /* Initialize temp comp dynamic range table */
  rfgsm_core_temp_comp_dynamic_range_tbl_init(rfm_dev);


  if(rf_common_init_critical_section(&rfgsm_core_handle_ptr->therm_read.crit_sect) == FALSE)
  {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Therm read critical section failed!, Failed to start therm", 0);
  }


}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Starts the callback that calls rfgsm_core_therm_read_trigger .It is called when 
  there is Tx. 
*/
void rfgsm_core_temp_comp_start( rfm_device_enum_type rfm_dev, uint8 sub_id )
{
  /* times set such that the calculation happens 3 secs later so that it
  ** doesn't start running as soon as TX power is enabled because we don't
  ** know if it is a real call
  */
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  uint16 rf_dev_sub_mask = 0;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  rf_dev_sub_mask= ((uint16)rfm_dev | (uint16) (sub_id << 8));

  RF_NULL_CHECK_RTN_V( rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!");

  RFGC_MSG_2( MSG_LEGACY_HIGH," rfgsm_core_temp_comp_start called for sub %d and temp_comp_started %d", sub_id, rfgsm_core_handle_ptr->rfgsm_core_temp_comp_started );

  if ( !rfgsm_core_handle_ptr->rfgsm_core_temp_comp_started )
  {
    timer_reg( &rfgsm_core_handle_ptr->rfgsm_core_temp_comp_clk_cb,
               (timer_t2_cb_type)rfgsm_core_therm_read_trigger,
               (timer_cb_data_type)rf_dev_sub_mask,
               RFGSM_CORE_TEMP_COMP_START_INT,
               RFGSM_CORE_TEMP_COMP_REPEAT_INT);

    rfgsm_core_handle_ptr->rfgsm_core_temp_comp_started  = TRUE;
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Stops the callback that calls rfgsm_core_therm_read_trigger .It is called when there 
  is no transmition. 
*/
void rfgsm_core_temp_comp_stop( rfm_device_enum_type rfm_dev )
{
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN_V( rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!");

  RFGC_MSG_1( MSG_LEGACY_HIGH, "rfgsm_core_temp_comp_stop called with temp_comp_started %d", rfgsm_core_handle_ptr->rfgsm_core_temp_comp_started );

  if ( rfgsm_core_handle_ptr->rfgsm_core_temp_comp_started )
  {
    (void)timer_clr(&rfgsm_core_handle_ptr->rfgsm_core_temp_comp_clk_cb, T_NONE);
    rfgsm_core_handle_ptr->rfgsm_core_temp_comp_started  = FALSE;
  } 
  else
  {
    RFGC_MSG( MSG_LEGACY_HIGH,
          "Trying to stop GSM Temp comp. calculator even though it is not started!!!" );
  }

  
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Suspend temp compensation.
*/
void rfgsm_core_temp_comp_suspend( rfm_device_enum_type rfm_dev )
{
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN_V( rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!" );

  RFGC_MSG_1( MSG_LEGACY_HIGH, "rfgsm_core_temp_comp_suspend called with temp_comp_start %d", rfgsm_core_handle_ptr->rfgsm_core_temp_comp_started );

  if ( rfgsm_core_handle_ptr->rfgsm_core_temp_comp_started )
  {
    rf_common_enter_critical_section(&rfgsm_core_handle_ptr->therm_read.crit_sect);

    rfgsm_core_handle_ptr->therm_read.rfgsm_core_temp_comp_suspended = TRUE;

    rf_common_leave_critical_section( &rfgsm_core_handle_ptr->therm_read.crit_sect );

    timer_pause(&rfgsm_core_handle_ptr->rfgsm_core_temp_comp_clk_cb);

  } 
  else
  {
    RFGC_MSG( MSG_LEGACY_HIGH, "Trying to suspend GSM temp comp even though it is not started!!!" );
  }
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Stops the callback that calls rfgsm_core_therm_read_trigger .It is called when there 
  is no transmition. 
*/
void rfgsm_core_temp_comp_resume( rfm_device_enum_type rfm_dev )
{
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN_V( rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!" );

  RFGC_MSG_1( MSG_LEGACY_HIGH, "rfgsm_core_temp_comp_resume called with temp_comp_started %d", rfgsm_core_handle_ptr->rfgsm_core_temp_comp_started );

  if ( rfgsm_core_handle_ptr->rfgsm_core_temp_comp_started )
  {
    timer_resume(&rfgsm_core_handle_ptr->rfgsm_core_temp_comp_clk_cb);

    rf_common_enter_critical_section( &rfgsm_core_handle_ptr->therm_read.crit_sect );

    rfgsm_core_handle_ptr->therm_read.rfgsm_core_temp_comp_suspended  = FALSE;

    rf_common_leave_critical_section( &rfgsm_core_handle_ptr->therm_read.crit_sect );
  } 
  else
  {
    RFGC_MSG( MSG_LEGACY_HIGH, "Trying to stop GSM Temp comp. calculator even though it is not started!!!" );
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the temperature compensation information from the 
  local structure.
*/
uint8 rfgsm_core_temp_comp_bin_index_get( rfm_device_enum_type rfm_dev )
{
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN( rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!", RFGSM_CORE_TEMP_COMP_16_BIN_SIZE );

  return  rfgsm_core_handle_ptr->temp_comp.current_temp_bin_16;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function updates the temp bin 16bit index
  
  @details
  This function updates the temperature compensation 16bit bin index as recieved from
  rf apps task to be used in temp comp calc in rf task context

  @param
  rfm device, temp bin 16bit index

  @retval None
*/
void rfgsm_core_temp_comp_bin_index_update( rfgsm_core_handle_type *rfgsm_core_handle_ptr, uint8 temp_bin_16 )
{
  rfgsm_core_handle_ptr->temp_comp.current_temp_bin_16 = temp_bin_16;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  calculates the amount of tx compensation in DAC counts based on current 
  temperature. 
*/
void rfgsm_core_temp_comp_calc_for_pcl(rfm_device_enum_type rfm_dev, int16* scaled_value)
{
  uint8 temp_index = 0;
  int16 temp_rem = 0;
  int32 delta = 0;
  uint16 bin_size = 0;
  uint32 therm_range = 0;
  uint32 curr_therm_read_increased_res = 0, rf_enc_therm_max_increased_res = 0,rf_enc_therm_min_increased_res=0;
  boolean enable_logging;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  gsm_tx_static_nv_type *tx_static_nv_data_ptr = NULL;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN_V( rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!" );

  RF_NULL_CHECK_RTN_V( rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl, "rfgsm_tx_nv_tbl is NULL!" );

  /* pointers to Tx static NVs*/
  tx_static_nv_data_ptr  = rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr;

  RF_NULL_CHECK_RTN_V( tx_static_nv_data_ptr, "tx_static_nv_data_ptr is NULL!" );

  enable_logging = rfgsm_core_handle_ptr->therm_read.enable_logging;

  /* If therm value= 0x1000, that means therm_read has not been done or it is invalid */
  if ( rfgsm_core_handle_ptr->rfgsm_core_temp_comp_therm_value == RFGSM_CORE_INVALID_THERM_READ )
  {
    RFGC_MSG( MSG_LEGACY_HIGH, "Therm value is 0x1000. Therm_read has not been done or it is invalid!!!" );
    return;
  }

 /*Increasing the resolution of the read so that when we compute the bin size we can retain
  the resolution and not throw away the floating point*/
  curr_therm_read_increased_res = rfgsm_core_handle_ptr->therm_read.curr_therm_read << THERMISTOR_RESOLUTION;
  rf_enc_therm_max_increased_res = rfcommon_nv_mm_tbl.rf_enh_therm_max << THERMISTOR_RESOLUTION;
  rf_enc_therm_min_increased_res = rfcommon_nv_mm_tbl.rf_enh_therm_min << THERMISTOR_RESOLUTION;

  therm_range = rf_enc_therm_max_increased_res - rf_enc_therm_min_increased_res;

  bin_size = (uint16)(therm_range /(GSM_PA_TEMP_COMP_TBL_SIZE-1));

  /* calculate temperature compensation table segment index */
  temp_index = (curr_therm_read_increased_res-rf_enc_therm_min_increased_res) / bin_size;   

  /* calculate segment remainder */
  temp_rem   = (curr_therm_read_increased_res-rf_enc_therm_min_increased_res) % bin_size; 

  rfgsm_core_handle_ptr->tx_log.temp_remainder = temp_rem;

  /* If the index is not the last entry, find the delta between
    table entries, scale it by the remainder divided by the
    temperature bin size, to get the delta between the Ith entry
    and the correct value.
  */
  if ( temp_index < (GSM_PA_TEMP_COMP_TBL_SIZE-1))
  {
    /* Calculate the difference between table entries */
    delta  = tx_static_nv_data_ptr->temp_comp_data.temp_comp_pcl_pwr_scaling[ temp_index + 1 ] -  
             tx_static_nv_data_ptr->temp_comp_data.temp_comp_pcl_pwr_scaling[ temp_index ];

    /* Scale the difference by the remainder and divide
       it by the bin size, to get the correct difference
       between the Ith entry and the correct value.
       The result is rounded to reduce error to 1/2 LSB.
    */
    delta *= temp_rem;
    delta = delta+ (bin_size/2);
    delta /= bin_size;

    *scaled_value = 
       ( tx_static_nv_data_ptr->temp_comp_data.temp_comp_pcl_pwr_scaling[ temp_index ] + delta);

  }
  else
  {

     *scaled_value = 
       ( tx_static_nv_data_ptr->temp_comp_data.temp_comp_pcl_pwr_scaling[GSM_PA_TEMP_COMP_TBL_SIZE-1]);

  }

  if(enable_logging == TRUE)
  {
    RFGC_MSG_4( MSG_LEGACY_HIGH,
                "GSM Temp Comp: Raw Therm Read: %d, Therm Bin Index: %d, "
                "Therm Remainder  %d, Scaled value in percentage %d",
                rfgsm_core_handle_ptr->therm_read.curr_therm_read, 
                temp_index, 
                temp_rem, 
                *scaled_value);

  }

} /* end of rfgsm_core_calc_temp_comp_tx_calc */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  calculates the amount of tx compensation in DAC counts based on current 
  temperature. 
*/
void rfgsm_core_enh_temp_comp_calc_for_pcl(rfm_device_enum_type rfm_dev, int16* scaled_value)
{
  uint8 temp_index = 0;
  int16 temp_rem = 0, bin_size = 0;
  uint32 therm_range = 0;
  uint32 curr_therm_read_increased_res = 0, rf_enc_therm_max_increased_res = 0,rf_enc_therm_min_increased_res=0;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  
  /*Get pointers to rfm device and Check for NULL pointer.*/
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN_V( rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!" );

  /* If therm value= 0x1000, that means therm_read has not been done or is invalid */
  if ( rfgsm_core_handle_ptr->rfgsm_core_temp_comp_therm_value == RFGSM_CORE_INVALID_THERM_READ )
  {
    RFGC_MSG( MSG_LEGACY_HIGH,"Therm value is 0x1000. Therm_read has not been done or is invalid!" );
    return;
  }

  /*Increasing the resolution of the read so that when we compute the bin size we can retain
  the resolution and not throw away the floating point*/
  curr_therm_read_increased_res = rfgsm_core_handle_ptr->therm_read.curr_therm_read << THERMISTOR_RESOLUTION;
  rf_enc_therm_max_increased_res = rfcommon_nv_mm_tbl.rf_enh_therm_max << THERMISTOR_RESOLUTION;
  rf_enc_therm_min_increased_res = rfcommon_nv_mm_tbl.rf_enh_therm_min << THERMISTOR_RESOLUTION;

  therm_range = rf_enc_therm_max_increased_res - rf_enc_therm_min_increased_res;
  bin_size = (uint16)(therm_range /(GSM_PA_TEMP_COMP_TBL_SIZE-1));
  if (bin_size <= 0)
  {
    RFGC_MSG( MSG_LEGACY_HIGH, "Enh Temp Comp: bin_size <= 0!" );
    return;
  }

  /* 
  Calculate temperature compensation table segment index with increased precision.
  Update core_handle.
  */
  temp_index = (curr_therm_read_increased_res-rf_enc_therm_min_increased_res) / bin_size;
  rfgsm_core_handle_ptr->temp_comp.temp_bin_16 = temp_index;

  /* calculate segment remainder */
  temp_rem   = (curr_therm_read_increased_res-rf_enc_therm_min_increased_res) % bin_size; 

  /* calculate linear regression factor.*/
  *scaled_value = (temp_rem*100)/(bin_size);

  /*Check that the scaled_value is between 0 and 100. If not, print F3 and appply comp based on temp_bin only.*/
  if (*scaled_value < 0 || *scaled_value > 100)
  {
     *scaled_value = 0;
     RFGC_MSG_1( MSG_LEGACY_HIGH, "Enh Temp Comp: error calculating scaled_value. Set to %d", *scaled_value );
  }

  if( rfgsm_core_handle_ptr->therm_read.enable_logging == TRUE )
  {
    RFGC_MSG_4( MSG_LEGACY_HIGH,
                "GSM Enh Temp Comp. Raw Therm Read: %d, Therm Bin Index: %d, "
                "Therm Remainder:  %d, Linearization factor in percentages: %d",
                rfgsm_core_handle_ptr->therm_read.curr_therm_read, 
                temp_index, 
                temp_rem >> THERMISTOR_RESOLUTION, 
                *scaled_value);

  }

} /* end of rfgsm_core_calc_temp_comp_tx_calc */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the PA temp comp value for the max power PCL.

  @retval PA temp comp value for the max power PCL based on the mod type.
*/
int16 rfgsm_core_temp_comp_pwr_get(rfm_device_enum_type rfm_dev)
{
  int16 scaled_value = 0;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN( rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!",  scaled_value );

  return rfgsm_core_handle_ptr->rfgsm_core_temp_comp_scaled_value;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function updates the PA temp comp value for the max power PCL.

  @details
  This function updates the PA temperature compensation information as recieved from
  rf apps task to be used in temp comp calc in rf task context

  @param
  rfm device, temp comp scaled value

  @retval None
*/
void rfgsm_core_temp_comp_pwr_update(rfgsm_core_handle_type *rfgsm_core_handle_ptr, int16 scaled_value)
{
  rfgsm_core_handle_ptr->rfgsm_core_temp_comp_scaled_value = scaled_value;
}




/* ===========================PRIVATE FUNCTIONS ==============================*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function calculates the therm bin.
*/
uint8 rfgsm_core_calc_therm_bin( uint32 therm_value, uint8 bin_array_length, 
                                 uint32 scaled_therm_size ) 
{
   uint16 therm_count_per_bin;
   uint8 bin_index;

   /*First calculate the bin size*/
   therm_count_per_bin = (uint16) (scaled_therm_size / bin_array_length)+1; /* +1 is to round it to 16 */

   /*Now calculate the bin number*/
   bin_index = (uint8)(therm_value/therm_count_per_bin);

   /*Make sure we do not exceed the bin index*/
   if (bin_index > (bin_array_length - 1))
   {
     bin_index = (bin_array_length - 1);
   }

   return bin_index;

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This funtion performs the thermistor temperature read. 
 
*/

#define RFGSM_CORE_TEMP_COMP_THERM_MAX 0xFFF

void rfgsm_core_therm_read_handler(rf_apps_cmd_type* cmd_ptr) 
{
  rfgsm_core_data_type rfgsm_core_data;
  rfm_device_enum_type rfm_dev = RFM_INVALID_DEVICE;
  uint8 sub_id;

  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;  

  boolean temp_comp_suspended = FALSE;
  rfm_dev = (rfm_device_enum_type)cmd_ptr->payload[0];
  sub_id = (rf_task_num_type)cmd_ptr->payload[1];

  RF_NULL_CHECK_RTN_V(cmd_ptr,"therm_read cmd_ptr is NULL!");

  if ( rfgsm_debug_flags.disable_temp_comp == TRUE)
  {
    RFGC_MSG( MSG_LEGACY_HIGH,"disable_temp_comp flag is TRUE! therm read is not done." );
    return;
  }

  rfm_dev = (rfm_device_enum_type)cmd_ptr->payload[0];
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  RF_NULL_CHECK_RTN_V(rfgsm_core_handle_ptr,"rfgsm_core_handle_ptr is NULL!");
 
  /* Set the core data */
  rfgsm_core_data.rf_task = RF_TASK_APPS;
  rfgsm_core_data.rfm_dev = rfm_dev;
  rfgsm_core_data.sub_id = sub_id;
  rfgsm_core_data.as_id = gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_core_data.sub_id);
  rfgsm_core_data.triple_buffer_id = RFLM_GSM_BUFFER_ID_NOT_REQUIRED;
  rfgsm_core_data.rfgsm_core_handle_ptr = rfgsm_core_handle_ptr;

  rf_task_num_type rfgsm_rf_task = rfgsm_core_get_task_id_from_sub_id((uint8)sub_id);

  if(RF_TASK_1 == rfgsm_rf_task)
  {
    if(rfgsm_core_handle_ptr->therm_read.fw_therm_read_umid != rfgsm_sub_to_therm_umid_map[RF_TASK_1].therm_read_umid)/*GERAN_FW_RFM_THERM_READ_CMD*/
    {
      RFGC_MSG_2( MSG_LEGACY_HIGH,"Not building therm event due to mismatch between UMID [0x%x] and SUB_ID [%d] ", rfgsm_core_handle_ptr->therm_read.fw_therm_read_umid, sub_id);
      return;
    }
  }

  if(RF_TASK_2 == rfgsm_rf_task)
  {
    if(rfgsm_core_handle_ptr->therm_read.fw_therm_read_umid != rfgsm_sub_to_therm_umid_map[RF_TASK_2].therm_read_umid)/*GERAN_FW2_RFM_THERM_READ_CMD*/
    {
      RFGC_MSG_2( MSG_LEGACY_HIGH,"Not building therm event due to mismatch between UMID [0x%x] and SUB_ID [%d] ", rfgsm_core_handle_ptr->therm_read.fw_therm_read_umid, sub_id);
      return;
    }
  }

  rf_common_enter_critical_section( &rfgsm_core_handle_ptr->therm_read.crit_sect );

  temp_comp_suspended = rfgsm_core_handle_ptr->therm_read.rfgsm_core_temp_comp_suspended ;

  rf_common_leave_critical_section( &rfgsm_core_handle_ptr->therm_read.crit_sect );

  if (FALSE == temp_comp_suspended)
  {
      /* Build CCS event and send cmd to FW */
      if (rfgsm_core_build_therm_read_event(&rfgsm_core_data))
      {
        /* Send Cmd to FW to process Thermistor read event */
        rfgsm_mdsp_send_gfw_therm_read_process_cmd(rfgsm_core_handle_ptr->therm_read.fw_therm_read_umid, rfgsm_core_handle_ptr->therm_read.event_handle_tag );
      }
      else
      {
        RFGC_MSG( MSG_LEGACY_HIGH,"Build therm read event failed!" );
      }
  }

  return;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function is the call back function to read the thermistor and is called evey 2seconds. 
  This function will read the thermistor value, the read thermistor value will be used for
  tx power temperature compensation. */

/*lint -e{715} t_unused is unused - hence the name */
void rfgsm_core_therm_read_trigger(uint32 t_unused, timer_cb_data_type data)
{
  /* send therm read command to rf_apps_task, which is low priority task */
  rfm_device_enum_type rfm_dev;
  rf_apps_cmd_type *therm_read_cmd = NULL;
  uint8 sub_id = (rf_task_num_type) ((data >> 8) & 0xFF);

  /* Get the pointer to the free buffer of RF command queue. */
  if ( ( therm_read_cmd = rf_apps_get_buf() ) == NULL )
  {
    RF_MSG( RF_ERROR, "rfgsm_core_therm_read_trigger: RF command queue is full!" );
    return;
  }

  rfm_dev = (rfm_device_enum_type) (data & 0xFF);
  
  therm_read_cmd->msgr_hdr.id = RFGSM_CORE_APPS_TASK_THERM_READ_CMD;
  therm_read_cmd->rex_hdr.cmd_hdr.tech_id = RF_APPS_GSM_ID;
  therm_read_cmd->payload[0] = (uint8)rfm_dev;
  therm_read_cmd->payload[1] = (uint8)sub_id;

  RFGC_MSG_2( MSG_LEGACY_HIGH, "Sending therm read cmd to apps task. rfm_dev =%d, sub_id =%d", rfm_dev, sub_id );

  /* place RFGSM_CORE_APPS_TASK_THERM_READ_CMD to RF command queue */
  rf_apps_cmd_put( therm_read_cmd );
   
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function will trigger the rf_app task to apply(Calculate) the tx power
  temperature compensation based on recent therm read value. 
  This can be called upon the receipt of recent therm read event or whenever the
  Tx band changes */

void rfgsm_core_temp_comp_trigger(rfgsm_core_data_type *core_data)
{
  /* send temp comp command to rf_apps_task */
  rf_apps_cmd_type *temp_comp_cmd = NULL;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(core_data->rfm_dev);


  /* Get the pointer to the free buffer of RF command queue. */
  if ( ( temp_comp_cmd = rf_apps_get_buf() ) == NULL )
  {
    RF_MSG( RF_ERROR, "rfgsm_core_temp_comp_trigger: RF command queue is full!" );
    return;
  }

  RF_NULL_CHECK_RTN_V( rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!");

  temp_comp_cmd->msgr_hdr.id = RFGSM_CORE_APPS_TASK_TEMP_COMP_CMD;
  temp_comp_cmd->rex_hdr.cmd_hdr.tech_id = RF_APPS_GSM_ID;
  temp_comp_cmd->payload[0] = (uint8)core_data->rfm_dev;
  /*store upper 8 bits in payload 1*/
  temp_comp_cmd->payload[1] = (uint8)((uint16)rfgsm_core_handle_ptr->therm_read.raw_therm>>8);
  /*store lower 8 bits in payload 2*/
  temp_comp_cmd->payload[2] = (uint8)((uint16)rfgsm_core_handle_ptr->therm_read.raw_therm & 0x00ff);
  temp_comp_cmd->payload[3] = core_data->sub_id;
  /* place RFGSM_CORE_APPS_TASK_TEMP_COMP_CMD to RF APP Task command queue */
  rf_apps_cmd_put( temp_comp_cmd );
   
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function calculates the amount of tx power compensation 
  based on current temperature.
*/
void rfgsm_core_temp_comp_handler(rf_apps_cmd_type* cmd_ptr)
{ 
  uint16 current_therm_read = RFGSM_CORE_TEMP_COMP_THERM_MAX;
  int16 temp_comp_scaled_value = 0;
  rfm_device_enum_type rfm_dev = RFM_INVALID_DEVICE;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  rfgsm_nv_cmn_data_type *rfgsm_nv_cmn_tbl_ptr = NULL;
  
  rfm_dev = (rfm_device_enum_type)cmd_ptr->payload[0];
  current_therm_read = (((uint16)cmd_ptr->payload[1]<<8)& 0xff00)+((uint16)cmd_ptr->payload[2]& 0x00ff);
  uint8 sub_id = cmd_ptr->payload[3];

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN_V( rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!");

  if( rfm_dev >= RFM_MAX_WAN_DEVICES )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"invalid device (rfm_dev=%d)",rfm_dev);
    return;
  }

  rfgsm_nv_cmn_tbl_ptr = rfgsm_nv_get_cmn_tbl_ptr(rfm_dev);

  if(rfgsm_nv_cmn_tbl_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Rfgsm_nv_cmn_tbl_ptr is NULL! Rfgsm_core_temp_comp_handler exits. ",0);
    return;
  }

  if ( rfgsm_debug_flags.disable_temp_comp == FALSE )
  {
    rfgsm_core_handle_ptr->rfgsm_core_temp_comp_therm_value = rfgsm_core_temp_comp_proces_raw_therm_read(rfm_dev,current_therm_read);
  }

  /* Calculate bin based on temperature reading */
  if (rfgsm_core_handle_ptr->rfgsm_core_temp_comp_therm_value != RFGSM_CORE_INVALID_THERM_READ) 
  {          
    rfgsm_core_handle_ptr->temp_comp.prev_temp_bin_8 = rfgsm_core_handle_ptr->temp_comp.temp_bin_8;
    rfgsm_core_handle_ptr->temp_comp.temp_bin_8 = 
    rfgsm_core_calc_therm_bin(rfgsm_core_handle_ptr->rfgsm_core_temp_comp_therm_value, 
                               RFGSM_CORE_TEMP_COMP_8_BIN_SIZE, 
                               RFGSM_CORE_ENH_THERM_RANGE);  // need to pass in nv max - min

    rfgsm_core_handle_ptr->temp_comp.prev_temp_bin_16 = rfgsm_core_handle_ptr->temp_comp.temp_bin_16;
    rfgsm_core_handle_ptr->temp_comp.temp_bin_16 = 
    rfgsm_core_calc_therm_bin(rfgsm_core_handle_ptr->rfgsm_core_temp_comp_therm_value, 
                               RFGSM_CORE_TEMP_COMP_16_BIN_SIZE, 
                               RFGSM_CORE_ENH_THERM_RANGE);
  }
  /* If invalid read, use middle bin */
  else
  {
    rfgsm_core_handle_ptr->temp_comp.prev_temp_bin_16 = rfgsm_core_handle_ptr->temp_comp.temp_bin_16;
    rfgsm_core_handle_ptr->temp_comp.temp_bin_16 = RFGSM_CORE_TEMP_COMP_16_CENTRE_BIN;

    rfgsm_core_handle_ptr->temp_comp.prev_temp_bin_8 = rfgsm_core_handle_ptr->temp_comp.temp_bin_8;
    rfgsm_core_handle_ptr->temp_comp.temp_bin_8 = RFGSM_CORE_TEMP_COMP_8_CENTRE_BIN;
  }

  if (rfgsm_core_handle_ptr->therm_read.enable_logging == TRUE)
  {
    RFGC_MSG_2( MSG_LEGACY_HIGH,
                "GSM Temp Comp 8 Bin Index: %d,GSM original Temp Comp 16 Bin Index: %d",
                rfgsm_core_handle_ptr->temp_comp.temp_bin_8,
                rfgsm_core_handle_ptr->temp_comp.temp_bin_16);
  }

  if (rfgsm_nv_cmn_tbl_ptr->enh_tempcomp_enable_nv == TRUE)
  {
    /* update therm and scaled_value values. */
     rfgsm_core_enh_temp_comp_calc_for_pcl(rfm_dev, &temp_comp_scaled_value);
  }
  else
  {
    /* calculate the pcl temp comp based on new bin */
    rfgsm_core_temp_comp_calc_for_pcl(rfm_dev, &temp_comp_scaled_value);
  }

   /* update the temp comp flag by sending a message to RF Task*/
  rfgsm_core_temp_comp_update_req(rfm_dev, temp_comp_scaled_value, rfgsm_core_handle_ptr->temp_comp.temp_bin_16, sub_id);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Compute the baseband headroom for all bands and store it to a table to be
  accessed later. 
*/

void rfgsm_core_temp_comp_dynamic_range_tbl_init(rfm_device_enum_type rfm_dev)
{
  int32 data;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN_V(rfgsm_core_handle_ptr,"rfgsm_core_handle_ptr is NULL!");

  /* 20*log10((2^N-1)/MAX_AMAM_DAC) */
  rfgsm_msm_compute_bb_headroom(rfm_dev, RFCOM_BAND_GSM850, &data);
  rfgsm_core_handle_ptr->rfgsm_core_bb_headroom_tbl[RFCOM_BAND_GSM850] = data;

  rfgsm_msm_compute_bb_headroom(rfm_dev, RFCOM_BAND_GSM900, &data);
  rfgsm_core_handle_ptr->rfgsm_core_bb_headroom_tbl[RFCOM_BAND_GSM900] = data;

  rfgsm_msm_compute_bb_headroom(rfm_dev, RFCOM_BAND_GSM1800, &data);
  rfgsm_core_handle_ptr->rfgsm_core_bb_headroom_tbl[RFCOM_BAND_GSM1800] = data;

  rfgsm_msm_compute_bb_headroom(rfm_dev, RFCOM_BAND_GSM1900, &data);
  rfgsm_core_handle_ptr->rfgsm_core_bb_headroom_tbl[RFCOM_BAND_GSM1900] = data;
}

/*!
  @brief
  Returns the bb headroom for temperature compensation based on GSM band.
 
  @retval bb headroom
*/
int32 rfgsm_core_temp_comp_get_bb_headroom(rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band)
{
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!", FALSE);

  if(band < RFCOM_NUM_GSM_BANDS)
  {
    return(rfgsm_core_handle_ptr->rfgsm_core_bb_headroom_tbl[band]);
  }
  else
  {
    RFGC_MSG_1( MSG_LEGACY_ERROR, "Invalid GSM band = %d", band );
    return -1;
  }
}

/*----------------------------------------------------------------------------*/
/*!
   @brief this api process the raw therm read value

   @details This api scales the raw therm read value used to calculate therm bins

   @param None

   @retval scaled therm value
   
*/

int16 rfgsm_core_temp_comp_proces_raw_therm_read( rfm_device_enum_type rfm_dev, uint16 current_therm_read ) 
{
  uint32 therm_value = RFGSM_CORE_INVALID_THERM_READ;
  uint32 rfgsm_raw_therm_range = 0;
  boolean enable_logging;

  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN( rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!", (int16)therm_value);

  rfgsm_core_handle_ptr->therm_read.curr_therm_read = current_therm_read;

  enable_logging = rfgsm_core_handle_ptr->therm_read.enable_logging;

  /* Ignore railed values */  
  if(rfgsm_core_handle_ptr->therm_read.curr_therm_read >= RFGSM_CORE_TEMP_COMP_THERM_MAX)
  {
      RFGC_MSG_1( MSG_LEGACY_ERROR, "Invalid thermistor Read = %d. Using previous value.",
           (uint16)rfgsm_core_handle_ptr->therm_read.curr_therm_read);

    rfgsm_core_handle_ptr->therm_read.curr_therm_read = rfgsm_core_handle_ptr->therm_read.prev_therm_read;   
  }
  else
  {
    rfgsm_core_handle_ptr->therm_read.prev_therm_read = rfgsm_core_handle_ptr->therm_read.curr_therm_read;
  }

  rfgsm_core_handle_ptr->tx_log.therm_read = rfgsm_core_handle_ptr->therm_read.curr_therm_read;  

  if( enable_logging )
  {
    RFGC_MSG_1( MSG_LEGACY_HIGH, "Raw Thermistor Read = %d", (uint16)rfgsm_core_handle_ptr->therm_read.curr_therm_read );
  }
    
  /* If therm read is still set to initial value, then return invalid therm read */
  if (rfgsm_core_handle_ptr->therm_read.curr_therm_read == RFGSM_CORE_INITIAL_THERM_READ )
  {
    return (uint16)therm_value;
  }

  rfgsm_raw_therm_range = rfcommon_nv_mm_tbl.rf_enh_therm_max - rfcommon_nv_mm_tbl.rf_enh_therm_min;

  /*sanity check*/
  if (rfgsm_core_handle_ptr->therm_read.curr_therm_read < rfcommon_nv_mm_tbl.rf_enh_therm_min)
  {
    rfgsm_core_handle_ptr->therm_read.curr_therm_read = (uint16)rfcommon_nv_mm_tbl.rf_enh_therm_min;
  }
  else if (rfgsm_core_handle_ptr->therm_read.curr_therm_read > rfcommon_nv_mm_tbl.rf_enh_therm_max)
  {
    rfgsm_core_handle_ptr->therm_read.curr_therm_read = (uint16)rfcommon_nv_mm_tbl.rf_enh_therm_max;
  }

  /*scale, true even if rfi_therm_min does not equal to 0*/
  if (rfgsm_raw_therm_range != 0)
  {
    therm_value = RFGSM_CORE_ENH_THERM_MIN + 
      ( ((rfgsm_core_handle_ptr->therm_read.curr_therm_read - rfcommon_nv_mm_tbl.rf_enh_therm_min) * RFGSM_CORE_ENH_THERM_RANGE) / 
        rfgsm_raw_therm_range); }
  else
  {

    if( enable_logging )
    {
      RFGC_MSG( MSG_LEGACY_ERROR,"RAW RFI RANGE IS NULL!!" );
    }
  }

  if( enable_logging )
  {
    RFGC_MSG_1( MSG_LEGACY_LOW, "Therm value = %d", (uint16)therm_value );
  }
     
  return (int16)therm_value;
} /* end of rfgsm_core_temp_comp_proces_raw_therm_read */



/*----------------------------------------------------------------------------*/

/*!
   @brief boolean rfgsm_core_process_therm_read_rsp(rfm_device_enum_type rfm_dev, uint8 tag)

   @details
   This api process the therm read response. Upon receiving a response from CCS, this api
   retrieves the raw therm read by reading back the therm read RFFE registers. If the rffes
   read back is sucessful, temp comp trigger is sent to RF apps task which calulates the
   temp comp bin index and thermistor value based on PCL for temperature compensation.
   @param
    rfm_device_enum_type
    uint8 tag

   @retval
   TRUE/FALSE
*/

boolean rfgsm_core_process_therm_read_rsp(rfgsm_core_data_type *core_data, uint8 tag, boolean therm_read_ok)
{
  rf_hal_bus_rffe_type read_buf[RFGSM_CORE_NUM_THERM_READ_REGS];
  uint16 num_rffe_retrieved;
  static uint16 prev_raw_therm = 0xffff;
  boolean enable_logging = FALSE;

  enable_logging = core_data->rfgsm_core_handle_ptr->therm_read.enable_logging;

  /* Check if Tempcomp is already stopped. */
  if( FALSE == core_data->rfgsm_core_handle_ptr->rfgsm_core_temp_comp_started )
  {
    RF_MSG_2( MSG_LEGACY_HIGH, "[dev %d] Tempcomp is stopped already. tag %d", core_data->rfm_dev, tag );

	/*Clean up the last therm read event handle */
	rfgsm_core_therm_read_event_cleanup(core_data->rfm_dev);
	return TRUE;
  }

  if (therm_read_ok)
  {
    /* Query the read data */
	if( rfgsm_mdsp_get_therm_data(core_data, &read_buf[0], tag, &num_rffe_retrieved))
    {
	  /*Clean up the therm read event (queing the cleanup cmd to the apps task) */
	  rfgsm_core_therm_read_event_cleanup ( core_data->rfm_dev );

	  if (num_rffe_retrieved > RFGSM_CORE_NUM_THERM_READ_REGS)
	  {
	    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"Number of therm registers, %d, is larger than %d", 
			  num_rffe_retrieved, RFGSM_CORE_NUM_THERM_READ_REGS);			
		      num_rffe_retrieved = RFGSM_CORE_NUM_THERM_READ_REGS;
	  }

	  /*allocate  and construct read therm buffer to pass it to rf device driver*/
	  rfgsm_core_construct_read_therm_script_buffer(core_data->rfgsm_core_handle_ptr, &read_buf[0],num_rffe_retrieved);

	  /*retrieve the raw therm read from device driver*/
	  core_data->rfgsm_core_handle_ptr->therm_read.raw_therm = 
	    rfdevice_gsm_get_therm_read( core_data->rfm_dev,
          (rf_buffer_intf*)core_data->rfgsm_core_handle_ptr->scripts.process_read_therm_script);

	  /*free up the memory allocated read therm script buffer*/
	  rfgsm_core_free_read_therm_script_buffer(core_data->rfgsm_core_handle_ptr);

	  /* Compare previous raw therm value with the current therm value, if there is a 
	  delta and if it is more than RFGSM_CORE_DELTA_OF_THERM_READ then go for temperature
	     compensation calculations otherwise skip it. RFGSM_CORE_DELTA_OF_THERM_READ decided 
	     as 2 because the delta of successive therm reads observed maximum as 2 */ 
	  if( abs_satw(prev_raw_therm - core_data->rfgsm_core_handle_ptr->therm_read.raw_therm) > RFGSM_CORE_DELTA_OF_THERM_READ )
	  {
	    /* Save the current therm value as a prev_raw_therm for next iteration */
		prev_raw_therm = core_data->rfgsm_core_handle_ptr->therm_read.raw_therm;

		/* trigger the temp comp calculations */
		rfgsm_core_temp_comp_trigger(core_data);
	  }
	  else
	  {
	    /* If RFGSM_DEBUG_NV_TEMP_COMP_LOGGING_ENABLE bit is set */
	    if( enable_logging )
	    {
	      /* Skipped the temp comp calculations since there is no change in the prev and current therm read values */
		  RF_MSG( MSG_LEGACY_ERROR,"temp comp calculations skipped!" );
	    }
	  }      
      return TRUE;
    }
    else
    {
      core_data->rfgsm_core_handle_ptr->therm_read.raw_therm = 0xffff;
	  RF_MSG( MSG_LEGACY_ERROR, "Therm read data was not read successfully!" );

	  if ((NULL == core_data->rfgsm_core_handle_ptr->therm_read.event_handle )||(tag != core_data->rfgsm_core_handle_ptr->therm_read.event_handle_tag))
	  {
	    return FALSE;
	  }

	  /*Clean up the therm read event (queing the cleanup cmd to the apps task) */
	  rfgsm_core_therm_read_event_cleanup ( core_data->rfm_dev );
	  return FALSE;
    }
  }
  else
  {
    RF_MSG_1( MSG_LEGACY_ERROR, "Therm read cancelled; event_handle_tag %d", tag );

	/*Clean up the last therm read event handle */
	rfgsm_core_therm_read_event_cleanup(core_data->rfm_dev);

	return TRUE;
  }
} /* end of rfgsm_core_process_therm_read_rsp */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfgsm_core_therm_read_event_handle_cleanup

  @details
  Therm event handle cleanup
 
  @param 
  rfm_device_enum_type rfm_dev
 
  @return
  None
*/

void rfgsm_core_therm_read_event_cleanup(rfm_device_enum_type rfm_dev)
{
  rf_apps_cmd_type *therm_read_cmd = NULL;

 // Get the pointer to the free buffer of RF command queue. 
 if ( ( therm_read_cmd = rf_apps_get_buf() ) == NULL )
 {
   RF_MSG( RF_ERROR, "rfgsm_core_therm_read_trigger: RF command queue is full!" );
   return;
 }

 // Populate the command
 therm_read_cmd->msgr_hdr.id = RFGSM_CORE_APPS_TASK_THERM_READ_CLEANUP_CMD;
 therm_read_cmd->rex_hdr.cmd_hdr.tech_id = RF_APPS_GSM_ID;
 therm_read_cmd->payload[0] = (uint8)rfm_dev;
 
 RFGC_MSG( MSG_LEGACY_HIGH, "Sending therm read event handler cleanup to apps task." );

 // place RFGSM_CORE_APPS_TASK_THERM_READ_CMD to RF command queue 
 rf_apps_cmd_put( therm_read_cmd ); 
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function cleanup therm read event handler from apps task
*/
void rfgsm_core_therm_read_event_cleanup_handler(rf_apps_cmd_type* cmd_ptr)
{
  rfm_device_enum_type rfm_dev = RFM_INVALID_DEVICE;

  RF_NULL_CHECK_RTN_V(cmd_ptr,"therm_read cmd_ptr is NULL!");
  rfm_dev = (rfm_device_enum_type)cmd_ptr->payload[0];

 rfgsm_mdsp_therm_read_event_cleanup(rfm_dev);
}



/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfgsm_mdsp_construct_read_therm_script_script_buffer(rfm_device_enum_type rfm_dev)

  @details
  This api constructs the read_therm_script buffer from rffe read_buf retrieved
  from CCS driver. The read_therm_script is passed to device driver for extracting
  raw therm read

  @param 
  rfm_device_enum_type rfm_dev
 
  @return
  None
*/
void rfgsm_core_construct_read_therm_script_buffer(rfgsm_core_handle_type *rfgsm_core_handle_ptr, 
                                                   rf_hal_bus_rffe_type* read_buf_ptr,
                                                   uint16 num_retrieved)
{
  rfgsm_mdsp_allocate_read_therm_script_buffers(rfgsm_core_handle_ptr);

  rfgsm_mdsp_construct_read_therm_script_buffer((rf_buffer_intf*)rfgsm_core_handle_ptr->scripts.process_read_therm_script,
                                                read_buf_ptr,
                                                num_retrieved);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  rrfgsm_core_construct_read_therm_ssbi_script_buffer (rfm_device_enum_type rfm_dev)

  @details
  This api constructs the read_therm_script buffer from ssbi read_buf retrieved
  from CCS driver. The read_therm_script is passed to device driver for extracting
  raw therm read

  @param 
  rfm_device_enum_type rfm_dev
 
  @return
  None
*/

void rfgsm_core_construct_read_therm_ssbi_script_buffer(rfgsm_core_handle_type *rfgsm_core_handle_ptr,
                                                        rf_hal_bus_sbi_type* read_buf_ptr,
                                                        uint16 num_retrieved)
{

  rfgsm_mdsp_allocate_read_therm_script_buffers(rfgsm_core_handle_ptr);

  rfgsm_mdsp_construct_read_therm_ssbi_script_buffer( (rf_buffer_intf*)rfgsm_core_handle_ptr->scripts.process_read_therm_script,
                                                       read_buf_ptr,
                                                       num_retrieved);

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfgsm_mdsp_free_read_therm_script_buffer(rfm_device_enum_type rfm_dev)

  @details
  This api frees up the read therm buffer from rffe read_buf retrieved
  from CCS driver. The read therm buffer is passed to device driver for extracting
  raw therm read

  @param 
  rfm_device_enum_type rfm_dev
 
  @return
  None
*/
void rfgsm_core_free_read_therm_script_buffer(rfgsm_core_handle_type *rfgsm_core_handle_ptr)
{
  rfgsm_mdsp_free_read_therm_script_buffers(rfgsm_core_handle_ptr);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfgsm_core_update_temp_comp_req(rfm_device_enum_type rfm_dev)

  @details
  This api sends a message from RF apps task to RF task once temp comp bin index and therm value
  based on PCL are updated based on current thermistor value.Upon successful receive of this message,
  RF dispatch will set the temp comp updated flag to apply temperature compensation to TX burst 
 
  @param 
  rfm_device_enum_type rfm_dev
 
  @return
  None
*/

void rfgsm_core_temp_comp_update_req(rfm_device_enum_type rfm_dev, int16 temp_comp_scaled_value, uint8 temp_bin_16,uint8 sub_id)
{
  errno_enum_type msgr_send_status = E_SUCCESS;
  /* send temp comp command to rf task */
  rfa_rf_gsm_update_temp_comp_req_s rfa_rf_gsm_update_temp_comp_req;
  /* populate payload  for temp comp */
  rfa_rf_gsm_update_temp_comp_req.device = rfm_dev;
  rfa_rf_gsm_update_temp_comp_req.temp_comp_scaled_value = temp_comp_scaled_value;
  rfa_rf_gsm_update_temp_comp_req.temp_bin_16=temp_bin_16;
  rfa_rf_gsm_update_temp_comp_req.sub_id = sub_id;

  uint32 umid_temp_comp;
  rf_task_num_type rf_task = rfgsm_core_get_task_id_from_sub_id((uint8)sub_id);
  
  switch (rf_task)
  {
    default:
    case RF_TASK_1:
      umid_temp_comp = RFA_RF_GSM_UPDATE_TEMP_COMP_REQ;
      break;

    case RF_TASK_2:
      umid_temp_comp = RFA_RF_GSM_UPDATE_TEMP_COMP_SUB2_REQ;
      break;
  }

      /* Send a confirmation message */
  msgr_send_status = rfgsm_core_apps_task_send_msgr_req(&rfa_rf_gsm_update_temp_comp_req.hdr,
                                                         umid_temp_comp,
                                                         sizeof(rfa_rf_gsm_update_temp_comp_req));

 if (msgr_send_status != E_SUCCESS)
  {
    RF_MSG_2(RF_ERROR,"RF APPS cmd UPDATE_TEMP_COMP unsuccessful UMID 0x%x: msg_send failed: %d", umid_temp_comp, msgr_send_status );
  }
  else
  {
    RF_MSG_2(RF_HIGH,"RF APPS cmd UPDATE_TEMP_COMP successful UMID 0x%x: msgr_send_success: %d", umid_temp_comp, msgr_send_status );
  }
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfgsm_core_temp_comp_flag_set(rfm_device_enum_type rfm_dev)

  @details
  This api sets the temp comp updated flag.
 
  @param 
  rfm_device_enum_type rfm_dev
 
  @return
  None
*/
void rfgsm_core_temp_comp_flag_set(rfgsm_core_handle_type *rfgsm_core_handle_ptr)
{
  rfgsm_core_handle_ptr->temp_comp.temp_comp_updated =TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfgsm_core_temp_comp_flag_clear(rfm_device_enum_type rfm_dev)

  @details
  This api clears the temp comp updated flag.
 
  @param 
  rfm_device_enum_type rfm_dev
 
  @return
  None
*/
void rfgsm_core_temp_comp_flag_clear(rfm_device_enum_type rfm_dev)
{
  rfgsm_core_handle_type *rfgsm_core_handle_ptr;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN_V(rfgsm_core_handle_ptr,"rfgsm_core_handle_ptr is NULL!");

  rfgsm_core_handle_ptr->temp_comp.temp_comp_updated = FALSE;

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfgsm_core_temp_comp_tbl_ajust(rfm_device_enum_type rfm_dev)

  @details
  This api ajust the enh_temp_comp table based on therm read values stored in NV
  during calibration for all bands.
 
  @param 
  rfm_device_enum_type rfm_dev
 
  @return
  None
*/
void rfgsm_core_temp_comp_tbl_adjust(rfm_device_enum_type rfm_dev)
{
  rfcom_gsm_band_type band = RFCOM_NUM_GSM_BANDS;
  rfgsm_nv_band_data_type *nv_data_ptr = NULL;
  uint32 therm_max_increased_res = 0;
  uint32 therm_min_increased_res = 0;
  uint32 temp_bins[GSM_PA_TEMP_COMP_TBL_SIZE] = {0};
  uint32 temp_bin_width;
  int32 pcl_vector[GSM_PA_TEMP_COMP_TBL_SIZE] = {0};
  uint32 therm_read = 0;
  int32 offset_adjust = 0;
  int32 slope = 0;
  uint32 temp_index = 0;
  uint32 pcl_index = 0;

  /* Calculate temp bins based on max and min values in NV */
  therm_max_increased_res = rfcommon_nv_mm_tbl.rf_enh_therm_max << THERMISTOR_RESOLUTION;
  therm_min_increased_res = rfcommon_nv_mm_tbl.rf_enh_therm_min << THERMISTOR_RESOLUTION;

  temp_bin_width = (therm_max_increased_res - therm_min_increased_res) / (GSM_PA_TEMP_COMP_TBL_SIZE - 1);

  temp_bins[0] = therm_min_increased_res;

  for (temp_index = 1; temp_index < RFNV_GSM_NUM_TEMP_BIN; temp_index++)
  {
    temp_bins[temp_index] = temp_bins[temp_index-1] + temp_bin_width;
  }
  
  /* shift internal enh_temp_comp table based on therm read values stored in NV during calibration for all bands */
  for (band = 0; band < RFCOM_BAND_GSM1800_B; band ++)
  {
    nv_data_ptr = rfgsm_nv_get_tbl_ptr(rfm_dev, band, RFGSM_NV_TX);
    RF_NULL_CHECK_RTN_V(nv_data_ptr," nv_data_ptr is NULL! ");

    therm_read = nv_data_ptr->tx_cal_nv_data.cal_temp.therm_read;

    for (pcl_index = 0; pcl_index < RFNV_NUMBER_OF_PCLS; pcl_index++)
    {
      for (temp_index = 0; temp_index < RFNV_NUMBER_OF_PCLS; temp_index++)
      {
        pcl_vector[temp_index] = nv_data_ptr->tx_static_nv_data_ptr->enh_temp_comp_data.tempcomp_table_gmsk[temp_index][pcl_index];
      }

      for (temp_index = 0; temp_index < RFNV_GSM_NUM_TEMP_BIN; temp_index++)
      {
        if (therm_read <= (temp_bins[temp_index] >> THERMISTOR_RESOLUTION))
        {
          break;
        }
      }

      if (temp_index == 0)
      {
        offset_adjust = pcl_vector[temp_index];
      }
      else if (temp_index == RFNV_GSM_NUM_TEMP_BIN)
      {
        offset_adjust = pcl_vector[temp_index-1];
      }
      else if (therm_read == (temp_bins[temp_index] >> THERMISTOR_RESOLUTION))
      {
        offset_adjust = pcl_vector[temp_index];
      }
      else if (pcl_vector[temp_index-1] < pcl_vector[temp_index])
      {
        slope = ((pcl_vector[temp_index] - pcl_vector[temp_index-1]) << THERMISTOR_RESOLUTION) / ((temp_bins[temp_index] >> THERMISTOR_RESOLUTION) - (temp_bins[temp_index-1] >> THERMISTOR_RESOLUTION));
        offset_adjust = pcl_vector[temp_index] - ((((temp_bins[temp_index] >> THERMISTOR_RESOLUTION) - therm_read) * slope) >> THERMISTOR_RESOLUTION);
      }
      else
      {
        slope = ((pcl_vector[temp_index-1] - pcl_vector[temp_index]) << THERMISTOR_RESOLUTION) / ((temp_bins[temp_index] >> THERMISTOR_RESOLUTION) - (temp_bins[temp_index-1] >> THERMISTOR_RESOLUTION));
        offset_adjust = pcl_vector[temp_index-1] - (((therm_read - (temp_bins[temp_index-1] >> THERMISTOR_RESOLUTION)) * slope) >> THERMISTOR_RESOLUTION);
      }

      for (temp_index = 0; temp_index < RFNV_GSM_NUM_TEMP_BIN; temp_index++)
      {
        nv_data_ptr->tx_static_nv_data_ptr->enh_temp_comp_data.tempcomp_table_8psk[temp_index][pcl_index] -= offset_adjust;
        nv_data_ptr->tx_static_nv_data_ptr->enh_temp_comp_data.tempcomp_table_gmsk[temp_index][pcl_index] -= offset_adjust;
      }
    }
  }
}
