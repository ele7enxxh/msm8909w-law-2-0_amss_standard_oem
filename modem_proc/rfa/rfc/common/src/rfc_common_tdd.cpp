/*===========================================================================


      R F C  C o m m o n  GSM/TDSCDMA  c o n t r o l   A P I

                            S o u r c e  F i l e

DESCRIPTION
  This file contains common functions for the RFC GSM/TDSCDMA control code.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2010 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rfc/common/src/rfc_common_tdd.cpp#1 $
  $DateTime: 2016/12/13 07:59:52 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/22/14   nc      Fix the excessive messages because of GPIOs in GRFC 
                   update scripts.
10/13/14   dr      Adding Featurization for Tabasco
07/18/14   Saul    Convert to using GPIO protection APIs.
12/13/13   cc      Unify gpio type to rfc_sig_cfg_type for FDD and TDD tech 
05/13/13   sb      Fix bounds on maximum number of GRFCs
03/11/13   sar     Updated file for APQ, GNSS only target.
12/06/12   sr      made changes to return the timing info correctly for 
                   the requested signal type.
11/02/12   sr      removed un-neccessary debug messages.
08/07/12   sr      separated start/stop script construction based logic info. 
08/01/12   jyu     Added support to allow override logic and time from AG  
07/31/12   jyu     Fixed bug to return all GRFCs 
07/27/12   sr      Added API to append the grfc info for the specified signal type
07/21/12   sr      removed the grfc rf engine references.
07/20/12   sr      Added an API to construct rf_buffer with start timing and logic
07/09/12   sr      changes to use the start and stop logic for TDD signal list.
07/05/12   sr      Added Err message if the signal type is GPIO while generating scripts
07/02/12   sr      Changed file comments.
06/29/12   sr      rfc api name change and removed some apis which are not needed.
06/28/12   sr      dime rfc changes.
05/18/12   sb      made APIs to get base address of fw grfc engine from msm signal and signal type
04/01/12   lcl     Removed empty mdsp interface header file 
03/17/12   jfc     Remove deprecated mdsp_intf_v.h include
03/03/12   jps     Fixes to make sure GSM PA Range GRFC is not assumed of type
                   GRFC RF ENGINE
12/16/11    sr     Force all the GRFC Engines to RFC_LOW in init_default.
12/12/11   vrb     Configure signals to init_state instead of "LOW" when setting to def
11/04/11    sr     added copy signal list API.
10/17/11    sr     changed the rfc api to get the timing info. for the specified signal.
08/26/11   sr      RFC V1.3.35 SS release changes. SS format has changed.
08/20/09   sr      added function to update the GRFC timing.
08/17/11   av      Set GSM GRFCs to default/low state when mode transition to GSM. 
                   Temp change till we change the RF/FW interface to handle grfc logic
                   in burst scripts
08/11/11   av      Added new API for constructing XtoG, GtoX grfc scripts.
08/06/11   sr      RFC Cleanup: removed gsm specific code.
07/08/11   sr     Added new API to get the GRFC number from list of signals
06/07/11  rsr/sr  Fixed pa_range grfc which were swapped.
05/17/11  rsr/sr  Fix to send RF_ENGINE numbers to MDSP, instead of INTERNAL GRFC Engine numbers. 
05/04/11    sr     made changes to configure mode specific GPIOs. 
03/04/11    sr     warnings fix
03/03/11    sr     merge from mdm9k branch
02/09/11   sr      changed the code to interface with latest mdsp fw interface change.
02/01/11   sr      fixed linker errors.
01/31/11   sr      removed un-used code.
01/26/11   aak     Resolve compile errors for 8960 scons
12/07/10   bmg     Moved rf_grfc_pol_buf to a common location.
01/26/11   av      Fixing comment warning
01/21/11   av      For some GRFCS, Pull the signal low: or to a known value as in XtoG, X doesn't care of this GRFC
01/19/11   av      Check for logic of GRFC before populating in CM scripts
01/12/11   lcl     Moved initialization of PA R0/R1 to RFC file so that 
                   tx_ind can be added based on card file
01/04/11   av      Compare W path 1 grfc settings for building WtoG scripts  
12/15/10   av      RFGSMCTL files cleanup 
12/03/10   te      Remove gpio_config call in CM
11/09/10   lcl     Remove compiler warning
10/27/10   av      Put back the LtoG grfc configuration
10/26/10   ap      Removed rfc_manage_vreg_rfa_id variable
10/18/10  av/lcl   Clean up common RFC support
10/11/10   lcl     Changed MSG_LVL to MSG_LEGACY to avoid too many F3 messages
10/09/10   av      Fixes for WtoG with common grfc structure enabled.
10/07/10   av      Reverting the pack fix and externing the function in common_gsmctl itself.
10/07/10   av      Fixing Pack and Compiler error in HC builds
09/24/10   av      Adding LTE band conversion to rfc when getting info from RFC.
09/21/10   av      Compiler warning fix
09/18/10   av      Fix compiler warnings
09/14/10   av      LtoG support and Compiler warning fixes
09/07/10   lcl     Remove hard code number of antenna and fix some C warnings
09/01/10   rsr     Fix for spike in TX/RX every 10 secs due to XO ADC turning off VREG_RFA_ID.
09/01/10   av      GSM RFC Rearch
07/27/10   lcl     Compiler warning fix
05/26/10   lcl     Fixed rf_card and rfcom mis-match.
05/20/10   tws     Initial version
============================================================================*/


/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "comdef.h"
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
extern "C" {
#include "rfc_common_tdd.h"
#include "rfc_bsp.h"
#include "rfc_card.h"
#include "rfcommon_msm.h"
#include "rfc_common.h" 
}

#include "rf_buffer_intf.h"

extern "C" {

/* rfc Signal info table for the specific RF CARD get initialized once during the init. */
extern rfc_signal_info_type *rfc_signal_info_table;
extern uint32 rfc_signals_num;

/* MSM Signal info table for the Specific MSM, get initializd once during init */ 
extern rfc_msm_signal_info_type *rfc_msm_signal_info_table; 
extern uint32 rfc_msm_signals_num;

} /* extern "C" */

extern boolean rfc_common_set_rf_grfc_engine(rfd_internal_grfc_engine_rf_enum_type rf_engine, rfc_logic_type logic);
extern boolean rfc_common_set_grfc(uint8 grfc, rfc_logic_type logic);
extern void rfc_common_init_grfc_rf_engine(rfc_msm_signal_type msm_signal, rfc_logic_type init_state);
#if defined(FEATURE_DIMEPM_MODEM) || defined(FEATURE_JOLOKIA_MODEM) || defined(FEATURE_TABASCO_MODEM)
extern boolean rfc_common_set_gpio(DALGpioSignalType dal_signal, rfc_logic_type logic);
#else
extern boolean rfc_common_set_gpio(DALGpioIdType gpio_id, rfc_logic_type logic);
#endif
extern int32 rfc_common_get_signal_num(int32 rf_sig_name, rfc_signal_type sig_type);



/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function finds the particular signal in the list of signals and return the correspoding
  signal information (GRFC ENGINE NUM, GRFC RF _ENGINE NUM or GPIO NUM).

  @details
  This function finds the particular signal in the list of signals and return the correspoding
  signal information (GRFC ENGINE NUM, GRFC RF _ENGINE NUM or GPIO NUM).

  @param
  rf_signals: List of RF signals.
  sig_type : the Signal type for which the Signal number need to be returned.
  rf_buf_ptr : rf_buffer where the GRFC information to be populated.
*/
boolean rfc_common_tdd_get_grfc_buffer(rfc_sig_cfg_type *rf_signals, rfc_signal_type sig_type,
                                       rfc_script_enum_type script_type, rf_buffer_intf *rf_buffer,
                                       uint8 num_override_data_entry,                                       
                                       rfc_sig_timing_info_type *override_data,
                                       boolean override_logic, boolean override_time)
{
  uint16 i = 0;
  uint16 accu_overrides =0;
  int32 rf_sig_name = RFC_SIG_LIST_END;
  int32 signal_num = -1;
  uint8 grfc_num = 0;
  boolean status = FALSE;
  rfc_msm_signal_type msm_signal = RFC_MSM_SIG_NUM;

  if(rf_signals == NULL || rf_buffer == NULL)
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "rfc_common_tdd_get_grfc_buffer got NULL ptr. rf_signals= %x, rf_buffer = %x",rf_signals,rf_buffer);
    return FALSE;
  }

  /* loop through the signal list and configure them based on their type to specified logic  */
  while (rf_signals[i].sig_name != RFC_SIG_LIST_END)
  {
    rf_sig_name = rf_signals[i].sig_name; /* get the RF-CARD Signal name */
    signal_num = rfc_common_get_signal_num(rf_sig_name, sig_type);
    if (signal_num >= 0)
    {
      msm_signal = rfc_signal_info_table[rf_sig_name].msm_sig_name; /* get the MSM Signal name */
      grfc_num = rfc_msm_signal_info_table[msm_signal].grfc_num;
      
      if (script_type == RFC_START_SCRIPT)
      {
        if(override_data)
        { 
          /* Use override data */
          if(accu_overrides < num_override_data_entry)
          {
            if(override_logic && override_time)
            {
              /* Override logic and time */
              rf_buffer->append_grfc(grfc_num, override_data[accu_overrides].logic, 
                                     override_data[accu_overrides].time_offset);
            }
            else if(override_logic)
            {
              /* Override logic only */
              rf_buffer->append_grfc(grfc_num, override_data[accu_overrides].logic, 
                                     rf_signals[i].start.time_offset);
            }
            else if(override_time)
            {
              /* Override time only */
              rf_buffer->append_grfc(grfc_num, rf_signals[i].start.logic, 
                                     override_data[accu_overrides].time_offset);
            }
            else
            {
              /* Error: inconsistent information about data override. Use AG data */
        rf_buffer->append_grfc(grfc_num, rf_signals[i].start.logic, 
                               rf_signals[i].start.time_offset);
              MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
                  "rfc_common_tdd_get_grfc_buffer got inconsistent data override request", 0);
      }
            accu_overrides++;
          }
          else
          {
            /* error: There are not enough override data to fill in. Use AG data */
            rf_buffer->append_grfc(grfc_num, rf_signals[i].start.logic, 
                                   rf_signals[i].start.time_offset);
            MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
                  "rfc_common_tdd_get_grfc_buffer doesn't have enough override data to fill in. num_override_data_entry=%d ",num_override_data_entry);
          }
        }        
        else
        {
          /* Use RFC AG since override data is empty */
          rf_buffer->append_grfc(grfc_num, rf_signals[i].start.logic, 
                                    rf_signals[i].start.time_offset);
        }
      }

      if (script_type == RFC_STOP_SCRIPT)
      {        
        if(override_data)
        { 
          /* Use override data */
          if(accu_overrides < num_override_data_entry)
          {
            if(override_logic && override_time)
            {
              /* Override logic and time */
              rf_buffer->append_grfc(grfc_num, override_data[accu_overrides].logic, 
                                     override_data[accu_overrides].time_offset);
            }
            else if(override_logic)
            {
              /* Override logic only */
              rf_buffer->append_grfc(grfc_num, override_data[accu_overrides].logic, 
                                     rf_signals[i].stop.time_offset);
            }
            else if(override_time)
            {
              /* Override time only */
              rf_buffer->append_grfc(grfc_num, rf_signals[i].stop.logic, 
                                     override_data[accu_overrides].time_offset);
            }
            else
            {
              /* Error: inconsistent information about data override. Use AG data */
              rf_buffer->append_grfc(grfc_num, rf_signals[i].stop.logic, 
                                     rf_signals[i].stop.time_offset);
              MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
                  "rfc_common_tdd_get_grfc_buffer got inconsistent data override request", 0);
            }
            accu_overrides++;
          }
          else
          {
            /* error: There are not enough override data to fill in. Use AG data */
            rf_buffer->append_grfc(grfc_num, rf_signals[i].stop.logic, 
                                   rf_signals[i].stop.time_offset);
            MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
                  "rfc_common_tdd_get_grfc_buffer doesn't have enough override data to fill in. num_override_data_entry=%d ",num_override_data_entry);
          }
        }        
        else
        {
          /* Use RFC AG since override data is empty */
        rf_buffer->append_grfc(grfc_num, rf_signals[i].stop.logic, 
                               rf_signals[i].stop.time_offset);
      }
      }

      status = TRUE;
    }
    i++;
  }

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function finds the particular signal in the list of signals and return the correspoding
  signal information (GRFC ENGINE NUM, GRFC RF _ENGINE NUM or GPIO NUM).

  @details
  This function finds the particular signal in the list of signals and return the correspoding
  signal information (GRFC ENGINE NUM, GRFC RF _ENGINE NUM or GPIO NUM).

  @param
  rf_signals: List of RF signals.
  sig_type : the Signal type for which the Signal number need to be returned.
  grfc_info : GRFC information to be populated.
*/
boolean rfc_common_tdd_get_grfc_info(rfc_sig_cfg_type *rf_signals, rfc_signal_type sig_type,
                                     rfc_grfc_script_entry_type *grfc_info)
{
  uint16 i = 0;
  int32 rf_sig_name = RFC_SIG_LIST_END;
  uint8 grfc_num = 0;
  boolean status = FALSE;
  rfc_msm_signal_type msm_signal = RFC_MSM_SIG_NUM;

  if(rf_signals == NULL || grfc_info == NULL)
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "rfc_common_gsm_get_grfc_info got NULL ptr. rf_signals= %x, grfc_info = %x",rf_signals,grfc_info);
    return FALSE;
  }

  /* loop through the signal list and configure them based on their type to specified logic  */
  while (rf_signals[i].sig_name != RFC_SIG_LIST_END)
  {
    rf_sig_name = rf_signals[i].sig_name; /* get the RF-CARD Signal name */
    msm_signal = rfc_signal_info_table[rf_sig_name].msm_sig_name; /* get the MSM Signal name */

    if (rfc_msm_signal_info_table[msm_signal].signal_type == sig_type)
    {
      grfc_num = rfc_msm_signal_info_table[msm_signal].grfc_num;
      grfc_info->grfc_id = grfc_num;
      grfc_info->start_logic = rf_signals[i].start.logic;
      grfc_info->start_delta = rf_signals[i].start.time_offset;
      grfc_info->stop_logic = rf_signals[i].stop.logic;
      grfc_info->stop_delta = rf_signals[i].stop.time_offset;
      status = TRUE;      
    }

    i++;
  }

  return status;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function Initializes the specified RF signals into their default state.

  @details
  This function Initializes the specified RF signals into their default state.

  @param rfc_signals: List of RF Signals which need to placed into their default state.
*/
boolean rfc_common_tdd_init_default(rfc_sig_cfg_type *rfc_signals)
{
  boolean ret_status = TRUE;
  uint16 i = 0;
  int32 rf_sig_name = RFC_SIG_LIST_END;

  if (rfc_signals == NULL) /* tech specific rf signals to be configured */
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_signals==NULL", 0);
    return FALSE;
  }

  /* loop through the signal list and configure them to specified default state */
  while (rfc_signals[i].sig_name != RFC_SIG_LIST_END)
  {
    rf_sig_name = rfc_signals[i].sig_name; /* get the RF-CARD Signal name */

    rfc_common_init_rfc_signal(rf_sig_name);

    i++;
  }

  return ret_status;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function configuers the specified RF signals to the specified state(HIGH/LOW).

  @details
  This function configuers the specified RF signals to the specified state(HIGH/LOW). It also setup
  the RFD routing table for the GRFC and RF GRFC engines.

  @param rf_signals: List of RF signals and the logic information to be configured.
*/
boolean rfc_common_tdd_config_signal(rfc_sig_cfg_type *rf_signals)
{
  uint16 i = 0;
  boolean status = TRUE;
  rfc_msm_signal_type msm_signal = RFC_MSM_SIG_NUM;
  int32 rf_sig_name = RFC_SIG_LIST_END;

  if (rf_signals == NULL) /* tech specific rf signals to be configured */
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rf_signals==NULL", 0);
    return FALSE;
  }

  /* make sure that the rfc common has been initialized */
  status = rfc_common_init_status();
  if (!status) 
  {
    return status;
  }

  /* loop through the signal list and configure them based on their type to specified logic  */
  while (rf_signals[i].sig_name != RFC_SIG_LIST_END)
  {
    rf_sig_name = rf_signals[i].sig_name; /* get the RF-CARD Signal name */
    msm_signal = rfc_signal_info_table[rf_sig_name].msm_sig_name; /* get the MSM Signal name */

    switch(rfc_msm_signal_info_table[msm_signal].output_type)
    {
    case RFC_GRFC:
      /* Set the specified GRFC Engine logic */
      status &= rfc_common_set_grfc(rfc_msm_signal_info_table[msm_signal].grfc_num, 
                                           rf_signals[i].start.logic);
      break;
    case RFC_GPIO:
      /* configure the GPIO to the specified logic */
      #if defined(FEATURE_DIMEPM_MODEM) || defined(FEATURE_JOLOKIA_MODEM) || defined(FEATURE_TABASCO_MODEM)
      status &= rfc_common_set_gpio(rfc_signal_info_table[rf_sig_name].dal_signal, rf_signals[i].start.logic);
      #else
      status &= rfc_common_set_gpio(rfc_signal_info_table[rf_sig_name].gpio_id, rf_signals[i].start.logic);
      #endif
      break;

    case RFC_SSBI: /* do nothing for now */
      break;

    default:
      break;
    }

    i++;
  }

  return status;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function populate the GRFC script with the specified RF Signals.

  @details
  This function populate the GRFC script with the specified RF Signals.

  @param path: prx/drx path
  @param mode: tech mode
  @param data: config data type
*/
uint32 rfc_common_tdd_update_start_grfc_script(rfc_sig_cfg_type *rf_signals, void *start_script_ptr)
{
  uint32 i = 0;
  int32 rf_sig_name = RFC_SIG_LIST_END;
  rfc_msm_signal_type msm_signal = RFC_MSM_SIG_NUM;
  uint8 grfc_num = 0x0;
  rf_buffer_intf *start_script_buf = (rf_buffer_intf *)start_script_ptr;

  if(rf_signals == NULL || start_script_buf == NULL)
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "rfc_common_generate_grfc_script got NULL ptr. rf_signals= %x, start_script_buf = %x",
          rf_signals, start_script_buf);
    return FALSE;
  }

  /* loop through the signal list and configure them based on their type to specified logic  */
  while (rf_signals[i].sig_name != RFC_SIG_LIST_END)
  {
    rf_sig_name = rf_signals[i].sig_name; /* get the RF-CARD Signal name */

    msm_signal = rfc_signal_info_table[rf_sig_name].msm_sig_name; /* get the MSM Signal name */
    grfc_num = rfc_msm_signal_info_table[msm_signal].grfc_num;
    if ((grfc_num < RF_HAL_GRFC_MAX_NUM) &&
        (rf_signals[i].start.logic == RFC_LOW ||  rf_signals[i].start.logic == RFC_HIGH))
    {
      start_script_buf->append_grfc(grfc_num, rf_signals[i].start.logic, rf_signals[i].start.time_offset);
    }
    else
    {
        MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR," Invalid Signal while creating GRFC scripts. signal = %d, GPIO = %d", 
              rf_sig_name, rfc_msm_signal_info_table[msm_signal].msm_gpio);
    }

    i++;
  }

  return i;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function populate the GRFC script with the specified RF Signals.

  @details
  This function populate the GRFC script with the specified RF Signals.

  @param path: prx/drx path
  @param mode: tech mode
  @param data: config data type
*/
uint32 rfc_common_tdd_update_grfc_script(rfc_sig_cfg_type *rf_signals, void *start_script_ptr, void *stop_script_ptr)
{
  uint32 i = 0;
  int32 rf_sig_name = RFC_SIG_LIST_END;
  rfc_msm_signal_type msm_signal = RFC_MSM_SIG_NUM;
  uint8 grfc_num = 0;
  uint32 gpio_num = 0;
  uint8 gpio_grfc = 0;

  rf_buffer_intf *start_script_buf = (rf_buffer_intf *)start_script_ptr;
  rf_buffer_intf *stop_script_buf = (rf_buffer_intf *)stop_script_ptr;

  if(rf_signals == NULL || start_script_buf == NULL || stop_script_buf == NULL)
  {
    MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "rfc_common_generate_grfc_script got NULL ptr. rf_signals= %x, start_script_buf = %x, stop_script_buf = %x",
          rf_signals,start_script_buf, stop_script_buf);
    return FALSE;
  }

  /* loop through the signal list and configure them based on their type to specified logic  */
  while (rf_signals[i].sig_name != RFC_SIG_LIST_END)
  {
    rf_sig_name = rf_signals[i].sig_name; /* get the RF-CARD Signal name */

    msm_signal = rfc_signal_info_table[rf_sig_name].msm_sig_name; /* get the MSM Signal name */
    gpio_num = rfc_msm_signal_info_table[msm_signal].msm_gpio;
    grfc_num = rfc_msm_signal_info_table[msm_signal].grfc_num;
    gpio_grfc = rfc_msm_signal_info_table[msm_signal].output_type;

    if (grfc_num < RF_HAL_GRFC_MAX_NUM)
    {  
      if (rf_signals[i].start.logic == RFC_LOW ||  rf_signals[i].start.logic == RFC_HIGH)
      {
        start_script_buf->append_grfc(grfc_num, rf_signals[i].start.logic, rf_signals[i].start.time_offset);
      }
      if (rf_signals[i].stop.logic == RFC_LOW ||  rf_signals[i].stop.logic == RFC_HIGH)
      {
        stop_script_buf->append_grfc(grfc_num, rf_signals[i].stop.logic, rf_signals[i].stop.time_offset);
      }
    }
    else if ((gpio_num != RFC_GPIO_INVALID) && (gpio_grfc !=RFC_GPIO)) // if the signals are timing info signals. then they are not GPIOs
    {
        MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR," Invalid Signal while creating GRFC scripts. signal = %d, GPIO = %d", 
              rf_sig_name, rfc_msm_signal_info_table[msm_signal].msm_gpio);
    }

    i++;
  }

  return i;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function update the rf signals start/stop timing data.

  @details
  This function update the rf signals start/stop timing data.

  @param rf_signals: Signal table list.
  @param sig_type: Signal type for which timing data need to be updated.
  @param start_time_offset: start delta offset to be updated with.
  @param stop_time_offset: stop_delta offset to be updated with.
*/
void rfc_common_tdd_update_timing(rfc_sig_cfg_type *rf_signals, 
                                  rfc_signal_type sig_type, 
                                  int32 start_time_offset, int32 stop_time_offset)
{
  boolean status = FALSE;
  uint32 i = 0;
  int32 rf_sig_name = RFC_SIG_LIST_END;
  rfc_msm_signal_type msm_signal = RFC_MSM_SIG_NUM;

  if (rf_signals == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rf_signal==NULL", 0);
    return;
  }

  /* make sure that the rfc common has been initialized */
  status = rfc_common_init_status();
  if (!status) 
  {
    return;
  }

  /* loop through the signal list and configure them based on their type to specified logic  */
  while (rf_signals[i].sig_name != RFC_SIG_LIST_END)
  {
    rf_sig_name = rf_signals[i].sig_name; /* get the RF-CARD Signal name */
    msm_signal = rfc_signal_info_table[rf_sig_name].msm_sig_name; /* get the MSM Signal name */

    if (rfc_msm_signal_info_table[msm_signal].signal_type == sig_type)
    {
      rf_signals[i].start.time_offset +=  start_time_offset;
      rf_signals[i].stop.time_offset +=  stop_time_offset;
    }

    i++;
  }

  return;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function copies the rfc_signals from src to dest rfc_signals ptr.

  @details
  This function copies the rfc_signals from src to dest rfc_signals ptr.

  @param src_signals: Source rfc Signal table list.
  @param dest_signals: Destination rfc Signal list ptr.
  @return  number of signals copied to destination ptr.
*/
int32 rfc_common_tdd_copy_sig_list(rfc_sig_cfg_type *src_signals, rfc_sig_cfg_type *dest_signals)
{
  uint8 i = 0;
  boolean end_of_sig_list = FALSE;

  if (src_signals == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"src_signals==NULL", 0);
    return 0;
  }

  if (dest_signals == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"dest_signals==NULL", 0);
    return 0;
  }

  while ( !end_of_sig_list )
  {
    dest_signals[i] = src_signals[i];
    if ((src_signals[i].sig_name == RFC_SIG_LIST_END) || (i >= (RFC_MAX_NUM_OF_GRFC-1)))
    {
      end_of_sig_list = TRUE;
    }
    
    i++;
  }

  return i;
}
#endif
