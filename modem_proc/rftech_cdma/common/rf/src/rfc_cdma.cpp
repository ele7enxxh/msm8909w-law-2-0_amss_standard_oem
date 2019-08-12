/*!
  @file
  rfc_cdma.cpp

  @brief
  <b>This is the RFC interface exposed to the MC layer.</b> Higher layers 
  (Core and MC) should refer to this interface. This module contains all 
  CDMA-specific implementations to support RFC, and is platform independent. 

  @addtogroup RF_RFC_CDMA_MC_INTF
  @{
*/

/*==============================================================================

  Copyright (c) 2008 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
 
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/src/rfc_cdma.cpp#1 $

when       who     what, where, why
-------------------------------------------------------------------------------- 
09/03/14   rsr     Changes to return TX LUT index when querying TX LUT info.
09/09/14   spa     Remove ref to et_modem_chain (deprecated from RFC),hardcode
08/25/14   spa     Add num_grfc pointer as arg to generate_tx_on_off_grfc_script 
07/17/14   spa     Added api to update RFC info for the alternate path
05/06/14   sty     deleted unused rfc_cdma_get_tx_device_cfg_info
04/11/14   APU     Enabled AsDIV
08/26/13   shb     Updates for new C++ transceiver device interface
07/18/13   zhw     Add AsDiv Support for setting GRFC switches
06/30/13   yzw     Fix C2K bands return wrong PA IDs for command FTM_GET_RFFE_DEVICE_INFO 
05/29/13   spa     Added delay param to APIs for getting GRFC scripts
03/21/13   yzw     Add device driver support to query PA device information {mfg_id, prd_id, rev_id}
03/13/13   sn      Added XPT functions
03/06/13   aro     Updated code to get ET modem chain from RFC autogen
12/13/12   APU     Check for band validity in get modem chain APIs.
01/03/13   aro     Interface to query slave modem chain
12/11/12   hdz     Enabled timing support in rfc_cdma_generate_tx_on_off_grfc_script()
12/06/12   APU     Incorporated CC comments and fixed typos.
12/06/12   APU     Replaced LM chains with Modem chains.
11/05/12   zhw     Support for PA On/Off GRFC script in RF On/Off event
10/30/12   zhw     Support for getting device cfg info from RFC
10/23/12   zhw     Support for getting Tx Template ID based on RFC query result
10/16/12   zhw     RF On/Off grfc scripting using overwritten rfc signal
08/22/12   aro     Renamed interface names and type definitions (time prof)
08/22/12   aro     Changes to migrate to new precide time profiler
07/11/12   shb     Updated script_buffer to be of rf_buffer_intf* type 
                   instead of void ptr
07/10/12   rmb     Removed critical section enter/exit and added these in rfc_common.c 
                   for wakeup/sleep function.  
07/09/12   sr      changes to use the rfc_data object instead of rfc_ag function ptr table.
07/09/12   spa     Renamed grfc_buf_ptr to script_buf_ptr 
07/06/12   spa     grfc_script APIs now accept void pointer to script 
                   buffer, removed is_irat flag 
05/22/12   sty     Call cdma_manage_power from rfc_cdma_manage_vregs() 
                   instead of going to the specific card
05/14/12   zw      Removed unused APIs
05/09/12   zw      Added GRFC scripting generation APIs
04/25/12   sty     Added back grfc_buf_ptr->num_grfc
04/19/12   Saul    1X/HDR. Handoff fix. Zero out GRFC num only once.
04/14/12   sty     Added grfc_script pointer to calls to RFC enable/disable API
04/05/12   sr      made changes to put all the GPIOs into low power mode during 
                   sleep
03/09/12   cd      Fixed compiler warning
03/08/12   cd      Added API to get GRFC engine number from RFC 
01/24/12   aro     Overloaded Get Rx warmup function to query enhanced
                   RF Warmup time
01/23/12   aro     Added RFC interface to query enhanced RF WU time
12/12/11   sty     Deleted rfc_cdma_get_rx_recommendations as it is redundant
11/29/11   sty     Added rfc_cdma_get_rx_recommendations()
11/05/11   aak     Enable NPA calls from RFSW for power management on 9x15  
09/12/11   sty     Added rfc_cdma_is_tunable_on_device()
08/25/11   aak     Enable RF NPA calls only for MSM8960 
08/05/11   sty     Deleted deprecated APIs
07/29/11   Saul    IRAT update. Skip setting GPIOs during IRAT.
07/23/11   Saul    Converted rx wake up APIs to use GRFC generate script.
07/20/11   sty     Changed return type of rfc_cdma_get_tx_lut_table_name
07/13/11   sty     Formatting change
07/11/11   cd      Separated GPIO/GRFC config functions for sleep and wakeup
07/07/11   sty     Added rfc_cdma_get_tx_lut_table_name()
06/29/11   cd      New card interfaces to configure GPIO/GRFC signals using 
                   autogen settings 
05/31/11   aro     [1] Added NULL pointer check for Logical Device param
                   [2] Added device internal parameter
05/25/11   aro     Changed RFC data structure to be RX and Tx Path dependent
05/21/11   aro     Changed RFC interface to handle device parameter
05/21/11   aro     Added interface to return Logical Device parameter
04/28/11   sty     Changed arg list in rfc_cdma_manage_vregs()
03/04/11   aro     Renamed RFC config structure name 
02/18/11   sty     Deleted rfc_cdma_get_tx_path
                   Renamed getinstance to get_instance  
01/25/11   sty     Added rfc_cdma_get_warmup_time
01/20/11   sty     Renamed rfc_cdma_card_interface to rfc_cdma_card_intf
01/19/11   sty     Replaced ptrQ with getinstance
01/10/11   sty     [1] Changed return type in rfc_cdma_get_rx_band_config and 
                       rfc_cdma_get_tx_band_config
                   [2] Deleted rfc_cdma_get_mode_config and 
                       rfc_cdma_config_band_data
01/06/11   sty     Fixed warnings
01/06/11   sty     Removed redundant checks, Updated Doxygen
01/06/11   sty     Renamed rfc_Cdma to rfc_cdma_card_intf
01/06/11   sty     Updated Doxygen
01/06/11   sty     Deleted unused APIs and variables
                   Updated Doxygen comments
                   Use rfc_card_cdma.h instead of rfc_card_1x.h
01/05/11   sty     Added manage and config VREG APIs
                   Removed redundant checks
01/05/11   sty     Doxygen changes
01/05/11   sty     Added Doxygen comments
01/04/11   aro     NULL pointer check
01/04/11   sty     Changed all *_1xhdr_* to *_cdma_*
01/04/11   sty     Deleted redundant header files
01/04/11   sty     Return -1 in rfc_1xhdr_get_tx_path on error
12/20/10   sty     Replaced rf_path_enum_type with rfm_path_enum_type
                   renamed rfc_1xhdr to rfc_cdma
12/20/10   sty     Changes towards new RFC design
10/22/10   Saul    Added entries for BC15(AWS)
07/29/10   aro     Common GRFC Design Implementation for 1xHDR
07/21/10   aro     Fixed Compiler Warnings
03/24/10   aro     Zero all HDET temperature compensation values
03/16/10   Saul    Added entries for BC6
03/03/10   Vish    Merged from prerel/poseidon2.1.x: 01/22/10
01/22/10   xw      Added NULL pointer checking before dereferencing band_config_ptr  
02/08/10   aro     Changed Message type
02/05/10   aro     Added NULL pointer Check
01/21/10   sr      Changes needed to support new RFC structures
01/20/10   sr      Changes needed to support new RFC structures
11/12/09   aro     Added Check for RFCOM_1XEVDO_MODE
08/25/09   aro     RFC 1x Design Update
                   [1] Moved HDET Therm Compensation into Band Specific data str
                   [2] Moved RC3, RRI, and CW Comp into 1x core config data str
                   [3] Created Generic functions to query each of these
                       Compensation Values. No more RF Card specific functions
                       are required for this purpose
                   [4] Changed the function/variable name to have "1xhdr"
                       instead of just "1x"
                   [5] Doxygen Documentation Added
10/28/09   sr      Corrected check for 1x or EVDO
11/21/08   dyc     Initial version

==============================================================================*/
#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rfc_card.h"
#include "rfc_card_cdma.h"
#include "rfcommon_core_error.h"
#include "rfcommon_msg.h"
#include "rfc_cdma.h"
#include "rfc_cdma_data.h"
#include "rfcommon_core.h"
#include "rfc_vreg_mgr.h"
#include "rf_hal_buffer.h"
#include "rfcommon_time_profile.h"

#include "rfdevice_logical_component_types.h"


/*============================================================================*/
/*!
  @name Band configuration function

  @brief
  This section contains all RF Band configuration related functions.
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function configures (if applicable) and manages the VREGs

  @details
  Configures the VREGs to the correct settings (such as voltage, SMPS 
  frequency), and executes the command vreg_cmd (to either turn ON or off) the 
  VREG

  @param device
  RF device for which operation is to be performed

  @param vreg_id
  VREG that needs to be managed

  @param vreg_cmd
  Operation to be performed on the VREG
 
  @return
  Flag indicating if the VREGs are configured successfully
*/
boolean 
rfc_cdma_manage_vregs
(
  rfm_device_enum_type device, 
  rfm_mode_enum_type mode, 
  rfm_cdma_band_class_type band,
  rfm_path_state state
)
{
  rf_time_type time_spent_val;
  rf_time_tick_type time_spent_in_func = rf_time_get_tick(); /* time Profiling */
  rfc_cdma_card_intf *rfc_obj = rfc_cdma_card_intf::get_instance();
  boolean result = FALSE; /* Return value */
  rf_path_enum_type vreg_path_num; /* path to be used by VREG manager */
  rfc_rf_chip_number rf_chip; /* Chip number to be used by VREG manager */

  /* get pointer to VREG manager object */
  rfc_vreg_mgr *vreg_mgr_p = rfc_vreg_mgr::get_instance();

  if (vreg_mgr_p == NULL)
  {
    ERR_FATAL( "rfc_cdma_manage_vregs: Could not get pointer to VREG object !",
               0,0,0 );
  }

  /* check for valid RFC object pointer */
  if ( rfc_obj == NULL )
  {
    RF_MSG( RF_ERROR, "rfc_cdma_manage_vregs: Obj not created" );
    return FALSE;
  }

  /* Get vreg path and chip */
  vreg_path_num = rfc_obj->get_vreg_path(device);
  rf_chip = rfc_obj->get_rf_chip_num(device);
  
  /* check for valid path and valid RF Chip num */
  if ( (vreg_path_num != RF_PATH_MAX) && (RF_CHIP_MAX != rf_chip) )
  {
    /* instantiate a VREG parameter object */
    rfc_vreg_param vreg_param(mode);

    result = TRUE;

    /* The rfc_power_management_info structure needs to be populated and sent to
       power manager */
    result &= vreg_param.set_path(vreg_path_num);
    result &= vreg_param.set_band(band);
    result &= vreg_param.set_rfchip_number(rf_chip);
    result &= vreg_param.set_rf_state(state);

    if ( state == RF_PATH_SLEEP_STATE)
    {
      /* disable the rf signals, put the signals into low power mode */
      result &= rfc_common_sleep(vreg_path_num, mode);
    }
    else
    {
      /* enable the rf signals, if the signals are in low power mode */
      result &= rfc_common_wakeup(vreg_path_num, mode);
    }
    
    vreg_mgr_p->cdma_manage_power(vreg_param);

  } /* if ( (vreg_path_num != RF_PATH_MAX) && (RF_CHIP_MAX != rf_chip) ) */
  else
  {
    result = FALSE;
    RF_MSG_2 ( RF_ERROR, "rfc_cdma_manage_vregs: Bad vreg path (%d) OR bad RF "
                         "chip num (%d)", vreg_path_num, rf_chip );
  }
  
  /* get time elapsed in this function */
  time_spent_val = rf_time_get_elapsed( time_spent_in_func, RF_USEC );

  RF_MSG_1( RF_MED, "rfc_cdma_manage_vregs: - time_taken = %d ",
            time_spent_val );
  
  return result;

} /* rfc_cdma_manage_vregs() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query the RF card to get the RF Warmup time needed for the SW and HW to be 
  configured

  @param device
  The RF device to be configured

  @return
  The RF Warmup time in micro-seconds (on success) and a -1 on failure

*/
int32
rfc_cdma_get_warmup_time
(
  rfm_device_enum_type device
)
{
  int32 rf_wup_time;

  rfc_cdma_card_intf *rfc_obj = rfc_cdma_card_intf::get_instance();

  if ( rfc_obj == NULL )
  {
    RF_MSG( RF_ERROR,"rfc_cdma_get_warmup_time: CDMA RFC Obj is NOT created");
    rf_wup_time = -1;
  }
  else
  {
    rf_wup_time = rfc_obj->get_warmup_time(device);
  }  

  return rf_wup_time;

} /* rfc_cdma_get_warmup_time */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query the RF card to get the RF Warmup time needed for the SW and HW to be 
  configured

  @param device
  RFM Device on which RF Warmup time is queried
 
  @param rf_mode
  RFM Mode for which RF warmup time is queried
 
  @param rf_wu_data
  Pointer to the Structure containing the Rx warmup time breakup. Calling
  function will be responsible to pass a valid pointer to a variable as
  RF will not be allocating memory.

  @return
  Flag indicating the validity of Warmup time
 
  @retval TRUE
  Flag to indicate that the warmup time time filled in the container is valid
 
  @retval FALSE
  Flag to indicate that the warmup time time filled in the container is
  not valid
*/
boolean
rfc_cdma_get_enhanced_warmup_time
(
  rfm_device_enum_type device,
  rfm_mode_enum_type rf_mode,
  void * const rf_wu_data
)
{
  boolean ret_val = FALSE;

  rfc_cdma_card_intf *rfc_obj = rfc_cdma_card_intf::get_instance();

  if ( rfc_obj == NULL )
  {
    RF_MSG( RF_ERROR,"rfc_cdma_get_enhanced_warmup_time: CDMA RFC Obj is NOT "
                     "created");
    ret_val = FALSE;
  }
  else
  {
    ret_val = rfc_obj->get_warmup_time( device, rf_mode, rf_wu_data );
  }  

  return ret_val;

} /* rfc_cdma_get_enhanced_warmup_time */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Retrieve the LUT table name (enum) that for a given band and PA state 
 
  @param mode
  RF mode for which table name is to be retrieved - this parm is not used 
  currently, but may be used in future - for instance, if LUT to be used is 
  different for 1x and HDR, for the same band
 
  @param device  
  RF device on which operation is to be performed 
 
  @param band  
  RF Band based on which LUT table name is to be returned  
 
  @param table_name_ptr
  Pointer to variable that holds the table_name

  @param pa_state
  PA state based on which LUT table name is to be returned

  @return 
  TRUE on success, and NULL on failure
    
*/
boolean
rfc_cdma_get_tx_lut_table_name
(
  rfm_mode_enum_type const mode,
  rfm_device_enum_type const device,
  rfm_cdma_band_class_type const band,
  rfm_pa_gain_state_type const pa_state,
  int* table_name_ptr,
  rfdevice_cdma_lut_type* table_type_ptr
)
{

  rfc_cdma_card_intf *rfc_obj = rfc_cdma_card_intf::get_instance();

  if ( rfc_obj == NULL )
  {
    RF_MSG( RF_ERROR,"rfc_cdma_get_tx_lut_table_name: RFC Obj NOT created");    
    return FALSE;
  }
  else
  {
    rfc_obj->get_tx_lut_table_name( mode, device, band, pa_state, 
                                    table_name_ptr, table_type_ptr );
  }  

  return TRUE;

} /* rfc_cdma_get_tx_lut_table_name */

/*! \} */

/*============================================================================*/
/*!
  @name CDMA Platform dependent Configuration

  @brief
  This section contains functions related to query CDMA platform dependent 
  configuration
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Logical Device Parameter

  @details
  This function will return all of the platform-dependent parameters to
  configure the proper hardware devices.

  @param device
  RF Device for which the logical parameter are queried

  @return
  Pointer to the Logical Device parameter structure
*/
const rfc_cdma_logical_device_params_type*
rfc_cdma_get_logical_device_params
(
  rfm_device_enum_type device
)
{

  const rfc_cdma_logical_device_params_type *dev_param;

  /* Get RFC Object */
  rfc_cdma_card_intf *rfc_obj = rfc_cdma_card_intf::get_instance();

  if ( rfc_obj == NULL )
  {
    RF_MSG( RF_ERROR, "rfc_cdma_get_logical_device_params: Obj Not created");
    dev_param = NULL;
  }
  else
  {
    /* Get Device Parameter */
    dev_param = rfc_obj->get_logical_dev_param(device);
  }  

  return dev_param;

} /* rfc_cdma_get_logical_device_params */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function generates the Rx Wakeup GRFCs script for the requested device
  and band.

  @param device
  RF device for which to generate GRFC script

  @param band
  Band for which to generate GRFC script
 
  @param script_buf_ptr
  Output - The requried script
 
  @return
  Status of API execution.
*/
boolean
rfc_cdma_generate_rx_wakeup_grfc_script
( 
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band, 
  rf_buffer_intf *script_buf_ptr,
  int16 delay
)
{ 
  boolean status;
  rfc_cdma_card_intf *rfc_obj = rfc_cdma_card_intf::get_instance();

  if ( rfc_obj == NULL )
  {
    RF_MSG( RF_ERROR, "rfc_cdma_generate_rx_wakeup_grfc_script : Obj is NOT created");
    return FALSE;
  }

  if ( script_buf_ptr == NULL )
  {
    RF_MSG( RF_ERROR, "rfc_cdma_generate_rx_wakeup_grfc_script : script ptr is "
                      "null"); 
    return FALSE;
  }

  if(device >= RFM_INVALID_DEVICE)
  {
    RF_MSG( RF_ERROR, "rfc_cdma_generate_rx_wakeup_grfc_script : Invalid device");
    return FALSE;
  }

  status = rfc_obj->generate_rx_wakeup_grfc_script(device, band, 
                                                   script_buf_ptr,
                                                   delay );

  return status;
} /* rfc_cdma_generate_rx_wakeup_grfc_script */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function generates the Rx Sleep GRFCs script for the requested device
  and band.

  @param device
  RF device for which to generate GRFC script

  @param band
  Band for which to generate GRFC script
 
  @param script_buf_ptr
  Output - The required script
 
  @param is_irat
  Indicates if the API is being called during IRAT operation
 
  @return
  Status of API execution.
*/
boolean
rfc_cdma_generate_rx_sleep_grfc_script
( 
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band, 
  rf_buffer_intf *script_buf_ptr,
  int16 delay
)
{ 
  boolean status;
  rfc_cdma_card_intf *rfc_obj = rfc_cdma_card_intf::get_instance();

  if ( rfc_obj == NULL )
  {
    RF_MSG( RF_ERROR, "rfc_cdma_generate_rx_sleep_grfc_script : Obj is NOT created");
    return FALSE;
  }

  if ( script_buf_ptr == NULL )
  {
    RF_MSG( RF_ERROR, "rfc_cdma_generate_rx_sleep_grfc_script : script ptr is "
                      "null"); 
    return FALSE;
  }

  if(device >= RFM_INVALID_DEVICE)
  {
    RF_MSG( RF_ERROR, "rfc_cdma_generate_rx_sleep_grfc_script : Invalid device");
    return FALSE;
  }

  status = rfc_obj->generate_rx_sleep_grfc_script(device, band, 
                                                  script_buf_ptr,
                                                  delay );

  if( status == FALSE )
  {
    RF_MSG( RF_ERROR, "rfc_cdma_generate_rx_sleep_grfc_script : script generation failed");
  }

  return status;
} /* generate_rx_sleep_grfc_script */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function generates the tx Wakeup GRFCs script for the requested device
  and band.

  @param device
  RF device for which to generate GRFC script

  @param band
  Band for which to generate GRFC script
 
  @param script_buf_ptr
  Output - The required script
 
  @param is_irat
  Indicates if the API is being called during IRAT operation
 
  @return
  Status of API execution.
*/
boolean
rfc_cdma_generate_tx_wakeup_grfc_script
( 
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band, 
  rf_buffer_intf *script_buf_ptr,
  int16 delay
)
{ 
  boolean status;
  rfc_cdma_card_intf *rfc_obj = rfc_cdma_card_intf::get_instance();

  if ( rfc_obj == NULL )
  {
    RF_MSG( RF_ERROR, "rfc_cdma_generate_tx_wakeup_grfc_script : Obj is NOT created");
    return FALSE;
  }

  if ( script_buf_ptr == NULL )
  {
    RF_MSG( RF_ERROR, "rfc_cdma_generate_tx_wakeup_grfc_script : scrpit ptr is "
                      "null"); 
    return FALSE;
  }

  if ( device >= RFM_INVALID_DEVICE )
  {
    RF_MSG( RF_ERROR, "rfc_cdma_generate_tx_wakeup_grfc_script : Invalid device");
    return FALSE;
  }

  status = rfc_obj->generate_tx_wakeup_grfc_script(device, band, 
                                                   script_buf_ptr,
                                                   delay );

  if( status == FALSE )
  {
    RF_MSG( RF_ERROR, "rfc_cdma_generate_tx_wakeup_grfc_script : script generation failed");
  }

  return status;
} /* rfc_cdma_generate_tx_wakeup_grfc_script */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function generates the tx sleep GRFCs script for the requested device
  and band.

  @param device
  RF device for which to generate GRFC script

  @param band
  Band for which to generate GRFC script
 
  @param script_buf_ptr
  Output - The required script
 
  @param is_irat
  Indicates if the API is being called during IRAT operation
 
  @return
  Status of API execution.
*/
boolean
rfc_cdma_generate_tx_sleep_grfc_script
( 
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band, 
  rf_buffer_intf *script_buf_ptr,
  int16 delay
)
{ 
  boolean status;
  rfc_cdma_card_intf *rfc_obj = rfc_cdma_card_intf::get_instance();

  if ( rfc_obj == NULL )
  {
    RF_MSG( RF_ERROR, "rfc_cdma_generate_tx_sleep_grfc_script : Obj is NOT created");
    return FALSE;
  }

  if ( script_buf_ptr == NULL )
  {
    RF_MSG( RF_ERROR, "rfc_cdma_generate_tx_sleep_grfc_script : scrpit ptr is "
                      "null"); 
    return FALSE;
  }

  if(device >= RFM_INVALID_DEVICE)
  {
    RF_MSG( RF_ERROR, "rfc_cdma_generate_tx_sleep_grfc_script : Invalid device");
    return FALSE;
  }

  status = rfc_obj->generate_tx_sleep_grfc_script(device, band, script_buf_ptr,
                                                  delay);

  if( status == FALSE )
  {
    RF_MSG( RF_ERROR, "rfc_cdma_generate_tx_sleep_grfc_script : script generation failed");
  }

  return status;
} /* rfc_cdma_generate_tx_sleep_grfc_script */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function generates the Tx On/Off GRFCs script for the requested device
  and band.

  @param device
  RF device for which to generate GRFC script

  @param band
  Band for which to generate GRFC script
 
  @param tx_on
  True for TX On. False for TX Off
 
  @param script_buf_ptr
  Output - The required script
  
  @param delay
  The timing informaiton to be used
 
  @param num_grfc (output)
  The number of GRFC signals available for this control, zero if no GRFC
  support available
 
  @return
  Status of API execution.
*/
boolean
rfc_cdma_generate_tx_on_off_grfc_script
( 
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band, 
  boolean tx_on,
  rf_buffer_intf *script_buf_ptr,
  int16 delay,
  int32 *num_grfc
)
{
  boolean status;
  rfc_cdma_card_intf *rfc_obj = rfc_cdma_card_intf::get_instance();

  if ( rfc_obj == NULL )
  {
    RF_MSG( RF_ERROR, "rfc_cdma_generate_rf_on_off_grfc_script : Obj is NOT created");
    return FALSE;
  }

  if ( script_buf_ptr == NULL )
  {
    RF_MSG( RF_ERROR, "rfc_cdma_generate_rf_on_off_grfc_script : scrpit ptr is "
                      "null"); 
    return FALSE;
  }

  if ( device >= RFM_INVALID_DEVICE )
  {
    RF_MSG( RF_ERROR, "rfc_cdma_generate_rf_on_off_grfc_script : Invalid device");
    return FALSE;
  }

  status = rfc_obj->generate_tx_on_off_grfc_script(device, band, tx_on,
                                                   script_buf_ptr, delay,
                                                   num_grfc );

  if( status == FALSE )
  {
    RF_MSG( RF_ERROR, "rfc_cdma_generate_rf_on_off_grfc_script : "
                      "script generation failed");
  }

  return status;
} /* rfc_cdma_generate_rf_on_off_grfc_script */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function generates the PA On/Off GRFCs script for the requested device
  and band.

  @param device
  RF device for which to generate GRFC script

  @param band
  Band for which to generate GRFC script
 
  @param pa_on
  True for PA On. False for PA Off
 
  @param script_buf_ptr
  Output - The required script
  
  @return
  Status of API execution.
*/
boolean
rfc_cdma_generate_pa_on_off_grfc_script
( 
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band, 
  boolean pa_on,
  rf_buffer_intf *script_buf_ptr,
  int16 delay
)
{
  boolean status;
  rfc_cdma_card_intf *rfc_obj = rfc_cdma_card_intf::get_instance();

  if ( rfc_obj == NULL )
  {
    RF_MSG( RF_ERROR, "rfc_cdma_generate_pa_on_off_grfc_script : Obj is NOT created");
    return FALSE;
  }

  if ( script_buf_ptr == NULL )
  {
    RF_MSG( RF_ERROR, "rfc_cdma_generate_pa_on_off_grfc_script : scrpit ptr is "
                      "null"); 
    return FALSE;
  }

  if ( device >= RFM_INVALID_DEVICE )
  {
    RF_MSG( RF_ERROR, "rfc_cdma_generate_pa_on_off_grfc_script : Invalid device");
    return FALSE;
  }

  status = rfc_obj->generate_pa_on_off_grfc_script(device, band, pa_on,
                                                   script_buf_ptr, delay );

  if( status == FALSE )
  {
    RF_MSG( RF_ERROR, "rfc_cdma_generate_pa_on_off_grfc_script : "
                      "script generation failed");
  }

  return status;
} /* rfc_cdma_generate_pa_on_off_grfc_script */

#ifdef FEATURE_RF_ASDIV
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function generates the AsDiv GRFCs script for the requested device
  and band.

  @param position
  RF device for which to generate GRFC script
 
  @param cb_data
  Call back data to pass in additional info for extendability

  @param asdiv_script_ptr
  Output - The GRFC script
 
  @return
  True on success and FALSE on failure
*/
boolean
rfc_cdma_generate_asdiv_xsw_script
( 
  rfcommon_asdiv_position_type position,
  rf_buffer_intf *asdiv_script_ptr,
  void* cb_data
)
{
  boolean status;
  rfc_cdma_card_intf *rfc_obj = rfc_cdma_card_intf::get_instance();


  if ( rfc_obj == NULL )
  {
    RF_MSG( RF_ERROR, "rfc_cdma_generate_asdiv_grfc_script : Obj is NOT created");
    return FALSE;
  }

  if ( asdiv_script_ptr == NULL )
  {
    RF_MSG( RF_ERROR, "rfc_cdma_generate_asdiv_grfc_script : scrpit ptr is "
                      "null"); 
    return FALSE;
  }

  status = rfc_obj->generate_asdiv_xsw_script( position, asdiv_script_ptr, 
                                                cb_data );

  if( status == FALSE )
  {
    RF_MSG( RF_ERROR, "rfc_cdma_generate_asdiv_grfc_script : "
                      "script generation failed");
  }

  return status;
} /* rfc_cdma_generate_asdiv_grfc_script */
#endif /* FEATURE_RF_ASDIV */

/*----------------------------------------------------------------------------*/
/*
  @brief
  Returns whether the specified device is tunable or not for a given band

  @param device the RF device that needs to be tuned
   
  @param band RF band to be checked for tuning

  @retval TRUE if the device can be tuned to the givenband, FALSE otherwise
*/
boolean
rfc_cdma_is_tunable_on_device
(
  rfm_device_enum_type device,
  rfm_cdma_band_class_type rf_band
)
{ 
  boolean result; /* api result */
  rfc_cdma_card_intf *rfc_obj = rfc_cdma_card_intf::get_instance();

  if ( rfc_obj == NULL )
  {
    RF_MSG( RF_ERROR, "rfc_cdma_is_tunable_on_device : Obj is NOT created" );
    return FALSE;
  }

  result = rfc_obj->is_device_tuneable(device, rf_band);

  return result;

} /* rfc_cdma_is_tunable_on_device */

/*----------------------------------------------------------------------------*/
/*
  @brief
  Returns sub-bandclasses supported for a given band

  @param device 
  the RF device that needs sub-bc info
   
  @param band 
  RF band to be checked 

  @return
   32-bit value holding the sub-bc info

*/
uint32
rfc_cdma_get_sub_class_for_band
(
  rfm_device_enum_type device,
  rfm_cdma_band_class_type rf_band
)
{ 
  uint32 result; /* api result */
  rfc_cdma_card_intf *rfc_obj = rfc_cdma_card_intf::get_instance();

  if ( rfc_obj == NULL )
  {
    RF_MSG( RF_ERROR, "rfc_cdma_get_sub_class_for_band : Obj is NOT created" );
    result = 0;
  }
  else
  {
    result = rfc_obj->get_sub_class_for_band(device, rf_band);
  }

  return result;

} /* rfc_cdma_get_sub_class_for_band */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the GRFC number for a specific signal type based on 
  device and band information.

  @details
  Based on the RFC signal type provided, this API queries the RFC for the 
  list of signals related to that signal type (Rx/Tx/Init). Common RFC 
  functionality is then used to map the RFC signal back to the modem 
  GRFC information.

  @param device
  RF device for which GRFC information is requested

  @param band
  Band for which GRFC information is requested
  
  @param sig_type
  Indicates which RFC signal type is being reverse mapped to the modem GRFC
 
  @return
  The GRFC number pertaining to the signal type requested
*/
int32
rfc_cdma_get_grfc_engine_num
( 
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band,
  rfc_signal_type sig_type
)
{ 
  int32 grfc_engine_num;
  rfc_cdma_card_intf *rfc_obj = rfc_cdma_card_intf::get_instance();

  if ( rfc_obj == NULL )
  {
    RF_MSG( RF_ERROR, "rfc_cdma_get_grfc_engine_num : Obj is NOT created");
    return -1;
  }

  if(device >= RFM_INVALID_DEVICE)
  {
    RF_MSG( RF_ERROR, "rfc_cdma_get_grfc_engine_num : Invalid device");
    return -1;
  }

  grfc_engine_num = rfc_obj->get_grfc_engine_num(device, band, sig_type);

  return grfc_engine_num;
} /* rfc_cdma_get_grfc_engine_num */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This API returns the TX modem chain based on the device and the band. 
  
  @params device
  RFM device 
   
  @params band 
  RFM CDMA band 
   
  @return
  Tx Modem chain.
 
*/
uint32
rfc_cdma_get_tx_modem_chain
(
  rfm_device_enum_type device ,
  rfm_cdma_band_class_type band  
)
{
  uint32 modem_chain = (uint32) (~0) ;
  const rfc_cdma_logical_device_params_type* logical_dev =
    rfc_cdma_get_logical_device_params ( device ) ;
  if ( logical_dev != NULL )
  {
    if ( band < RFM_CDMA_MAX_BAND )
    {
      modem_chain =
        logical_dev->rfc_tx_device_info.rfc_device_info[band]->modem_chain ;
    }
    else
    {
      RF_MSG_1 ( RF_ERROR , "rfc_cdma_get_tx_modem_chain(): " 
             "Band Check Failed, Band Supplied: %d" , band ) ;
    }
  }
  else
  {
    RF_MSG_1 ( RF_ERROR , "rfc_cdma_get_tx_modem_chain(): " 
             "logical device for RFM device: %d" , device ) ;
  }
  return modem_chain ;
} 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This API returns the Slave TX modem chain based on the device and the band. 
  
  @params device
  RFM device 
   
  @params band 
  RFM CDMA band 
   
  @return
  Tx Modem chain.
*/
uint32
rfc_cdma_get_slave_tx_modem_chain
(
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band  
)
{
  uint32 modem_chain = (uint32) (~0) ;
  const rfc_cdma_logical_device_params_type* logical_dev =
    rfc_cdma_get_logical_device_params ( device ) ;
  if ( logical_dev != NULL )
  {
    if ( band < RFM_CDMA_MAX_BAND )
    {
      /* The concept of slave Tx modem chain is no longer valid on newer
      targets, it has been deprecated from RFC, as the first step in cleanup
      hard code to one */
      modem_chain = 1;
    }
    else
    {
      RF_MSG_1 ( RF_ERROR , "rfc_cdma_get_slave_tx_modem_chain(): " 
             "Band Check Failed, Band Supplied: %d" , band ) ;
    }  
  }
  else
  {
    RF_MSG_1 ( RF_ERROR , "rfc_cdma_get_tx_modem_chain(): " 
             "logical device for RFM device: %d" , device ) ;
  }
  return modem_chain ;
} /* rfc_cdma_get_slave_tx_modem_chain */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This API returns the RX modem chain based on the device and
  the band.
  
  @params device
  RFM device 
   
  @params band 
  RFM CDMA band 
   
  @return
  RX Modem Chain
 
*/

uint32
rfc_cdma_get_rx_modem_chain
(
  rfm_device_enum_type device ,
  rfm_cdma_band_class_type band 
)
{
  uint32 modem_chain = (uint32) (~0) ;
  const rfc_cdma_logical_device_params_type* logical_dev =
    rfc_cdma_get_logical_device_params ( device ) ;
  if ( logical_dev != NULL )
  {
    if ( band < RFM_CDMA_MAX_BAND )
    {
      modem_chain =
        logical_dev->rfc_dev_info.rfc_device_info[band]->modem_chain ;
    }
    else
    {
      RF_MSG_1 ( RF_ERROR , "rfc_cdma_get_rx_modem_chain(): " 
             "Band Check Failed, Band Supplied: %d" , band ) ;
    }  
  }
  else
  {
    RF_MSG_1 ( RF_ERROR , "rfc_cdma_get_rx_modem_chain(): " 
             "logical device for RFM device: %d" , device ) ;
  }
  return modem_chain ;
} 

/*! \} */

/*============================================================================*/
/*!
  @name XPT configure feedback for IQ capture

  @brief
  This section XPT calibration related functions that sets up feedback
  for IQ capture
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  call RFC function to setup feedback path

  @details
  call RFC function to setup feedback path for IQ capture during XPT cal

  @param device_fb
  feedback RF device to which the path is set up

  @param band
  Enum to indicate the cdma band

  @param et_enable
  boolean enable or disable ET
  
  @return
  boolean status
*/
/*----------------------------------------------------------------------------*/

boolean rfc_cdma_setup_fb_path 
(
  rfm_device_enum_type device_fb, 
  rfm_cdma_band_class_type band, 
  boolean et_enable
)
{
  boolean api_status = TRUE ;
  rfc_cdma_card_intf *rfc_obj = rfc_cdma_card_intf::get_instance();
	
  api_status = rfc_obj->setup_fb_path(device_fb, band, et_enable);
	
  return(api_status);
	
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  call RFC function to setup feedback device

  @details
  call RFC function to setup feedback device for IQ capture during XPT cal

  @param device
  Transmitting RF device that sends IQ samples
  
  @param device_fb
  feedback RF device to which the path is set up

  @param band
  Enum to indicate the cdma band

  @param tx_chan
  ARFCN channel number

  @param fb_device_ptr
  rfcmn_device_type common device pointer for feedback device
  
  @return
  boolean status
*/
/*----------------------------------------------------------------------------*/
boolean rfc_cdma_setup_fb_device
(
  rfm_device_enum_type device, 
  rfm_device_enum_type device_fb,
  rfm_cdma_band_class_type band,
  uint32 tx_chan,
  rfdevice_rxtx_common_class**  fb_device_ptr
)
{
  boolean  api_status = TRUE ;
  rfc_cdma_card_intf *rfc_obj = rfc_cdma_card_intf::get_instance();
	
  api_status &= rfc_obj->setup_fb_device(device, device_fb, band, tx_chan, 
                                          fb_device_ptr);

  return api_status;
	
}


/*------------------------------------------------------------------------------------*/
/*!
  @brief
  This API gets RFFE device ID information based on the rfm device and rffe device type for CDMA bands
  
  @params device
  RFM device 
   
  @params band 
  RFM CDMA band 

  @params rffe_device_type 
  RFFE device type

  @params mfg_id 
  Passing in parameter to store manufacture id 

  @params prd_id 
  Passing in parameter to store product id 
  
  @params rev_id 
  Passing in parameter to store revision id 
   
  @return
  TRUE on success and FALSE on failure
 
*/
boolean rfc_cdma_get_rffe_device_info
(
	rfm_device_enum_type rfm_dev, 
	rfm_cdma_band_class_type band, 
	rfdevice_type_enum_type rffe_device_type,
	uint16 *mfg_id,
	uint16 *prd_id,
	uint16 *rev_id
)
{
	boolean  status = TRUE ;
	rfc_cdma_card_intf *rfc_obj = rfc_cdma_card_intf::get_instance();

	/*check the pass in paramters*/
	if ( (rfm_dev < RFM_DEVICE_0) || (rfm_dev >= RFM_INVALID_DEVICE) )
	{
	  RF_MSG_1( RF_ERROR, "rfc_cdma_get_rffe_device_info : Invalid device %d", rfm_dev);
	  status &= FALSE;
	}
	
	if ( (band < RFM_CDMA_BC0) || (band >= RFM_CDMA_MAX_BAND) )
	{
	  RF_MSG_1( RF_ERROR, "rfc_cdma_get_rffe_device_info : Invalid band %d", band);
	  status &= FALSE;
	}

	if ( (rffe_device_type < RFDEVICE_TRANSCEIVER) || (rffe_device_type >= RFDEVICE_TYPE_MAX_NUM) )
	{
	  RF_MSG_1( RF_ERROR, "rfc_cdma_get_rffe_device_info : Invalid device type %d", rffe_device_type);
	  status &= FALSE;
	}	

	/*if pass the basic parameter check*/
	if ( status == TRUE )
	{
	  status = rfc_obj->get_rffe_device_info( rfm_dev, band, rffe_device_type, mfg_id, prd_id, rev_id );
	}

	return status;	
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns if the given device is a Rx device or
  not.

  @param device
  RF device in question.
 
  @return
  TRUE:  Indicates that the device supports Rx.
  FALSE: Indicates that the device does not supports Rx.
                */
boolean
rfc_cdma_is_rx_device
( 
  rfm_device_enum_type device
)
{ 
  boolean status;
  rfc_cdma_card_intf *rfc_obj = rfc_cdma_card_intf::get_instance();

  if ( rfc_obj == NULL )
  {
    RF_MSG( RF_ERROR, "rfc_cdma_is_rx_device : Obj is NOT created");
    return FALSE;
  }

  if(device >= RFM_INVALID_DEVICE)
  {
    RF_MSG( RF_ERROR, "rfc_cdma_is_rx_device : Invalid device");
    return FALSE;
  }

  return ( rfc_obj->is_rfm_device_rx(device) ) ;
} /* rfc_cdma_is_rx_device */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns if the given device is a Tx device or
  not.

  @param device
  RF device in question.
 
  @return
  TRUE:  Indicates that the device supports Tx.
  FALSE: Indicates that the device does not supports Tx.
                */
boolean
rfc_cdma_is_tx_device
( 
  rfm_device_enum_type device
)
{ 
  boolean status;
  rfc_cdma_card_intf *rfc_obj = rfc_cdma_card_intf::get_instance();

  if ( rfc_obj == NULL )
  {
    RF_MSG( RF_ERROR, "rfc_cdma_is_tx_device : Obj is NOT created");
    return FALSE;
  }

  if(device >= RFM_INVALID_DEVICE)
  {
    RF_MSG( RF_ERROR, "rfc_cdma_is_tx_device : Invalid device");
    return FALSE;
  }

  return ( rfc_obj->is_rfm_device_tx(device) ) ;
} /* rfc_cdma_is_tx_device */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  API to update the rf port enumeration for various devices 
 
  @details
  This function is used as part of path swapping procedure. It is called when a
  suitable alternate path is selected for a particular CA or DSDS/DSDA scenario.
  The alternate path info along with the current band and logical device is used
  to determine which specific device info structure for that band should be
  used. Based on the info, rf port enumeration for various rf devices are
  updated. 
 
  @param rfm_dev
  current logical device which needs port reconfiguration
 
  @param curr_band
  current rf band involved in path swapping or port selection 
 
  @param alt_path
  preferred alternate path to use for a specific logical device & band combo
 
  @return status
  TRUE if a successful match is found in RFC AG and rf port info update is
  complete, else return FALSE 
*/
boolean
rfc_cdma_update_alt_rx_path
( 
  rfm_device_enum_type device,
  rf_card_band_type curr_band, 
  uint8 alt_path
)
{ 
  boolean status;
  rfc_cdma_card_intf *rfc_obj = rfc_cdma_card_intf::get_instance();

  if ( rfc_obj == NULL )
  {
    RF_MSG( RF_ERROR,"rfc_intf::rfc_cdma_update_alt_rx_path: "
                     "CDMA RFC Obj is NOT created");
    status = FALSE;
  }
  else
  {
    /* Call RFC API to update the alt path */
    status = rfc_obj->update_rx_path( device, curr_band, alt_path );
  }
  
  return status;  

} /* rfc_cdma_update_alt_rx_path */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get current Rx path for a given RFM device.

  @details
  This method will return current Rx path for a given RFM
  device. Then this value can be used to determine if the path
  switch is neccessary or not.

  @param device
  RF Device for which the logical parameter current Rx path is
  queried

  @return uint8
  Current Rx path for a given RFM device.
*/
uint8
rfc_cdma_get_current_rx_path
(
  rfm_device_enum_type        device ,
  rfm_cdma_band_class_type    band 
)
{

  uint8 current_path = ((uint8)~0) ;
  /* Get RFC Object */
  rfc_cdma_card_intf *rfc_obj = rfc_cdma_card_intf::get_instance() ;

  if ( rfc_obj == NULL )
  {
    RF_MSG( RF_ERROR, "rfc_cdma_get_logical_device_params: Obj Not created");
  }
  else
  {
    /* Get Device Parameter */
    current_path = rfc_obj->get_current_rx_path ( device  , band ) ;
  }  

  return current_path;

} /* rfc_cdma_get_current_rx_path */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  API to return the common RF device for the specified band. (return object of type rfdevice_rxtx_common_class ) 
 
  @details
  This function is used as part of path swapping procedure. It is called when a
  suitable alternate path is selected for a particular CA or DSDS/DSDA scenario.
  The alternate path info along with the current band and logical device is used
  to determine which specific device info structure for that band should be
  used. Based on the info, rf port enumeration for various rf devices are
  updated. 
 
  @param rfm_dev
  current logical device which needs port reconfiguration
 
  @param curr_band
  current rf band involved in path swapping or port selection 
 
  @param alt_path
  preferred alternate path to use for a specific logical device & band combo
 
  @return status
  TRUE if a successful match is found in RFC AG and rf port info update is
  complete, else return FALSE 
*/
rfdevice_rxtx_common_class *
rfc_cdma_get_cmn_rxtx_device
( 
  rfm_device_enum_type device,
  rfm_cdma_band_class_type cdma_band, 
  uint8 alt_path
)
{
    rfc_device_info_type *device_info_ptr = NULL;
    /*Device type used to extract tech specific object*/
    rfdevice_type_enum_type dev_type = RFDEVICE_TYPE_INVALID;
    /*Device instance used to extract tech specific object*/
    uint32 instance = 0;
    uint32 phy_path;

    rfc_cdma_data *rfc_data = rfc_cdma_data::get_instance();
    rfc_cfg_params_type cfg;

    cfg.alternate_path = 0;
    cfg.band = cdma_band;
    cfg.rx_tx = RFC_CONFIG_TX;
    cfg.logical_device = device;
    cfg.req = RFC_REQ_DEFAULT_GET_DATA;

    cfg.path = rfcommon_core_device_to_path(device);

    if (rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr))
    {
        /*Get device type and device instance to retrieve device object*/
        dev_type = device_info_ptr->rf_asic_info[0].device_type;
        instance = device_info_ptr->rf_asic_info[0].instance;
        phy_path = device_info_ptr->rf_asic_info[0].phy_path_num;

        /*Retrieve device object*/
        return rfc_common_get_cmn_device_object((uint8)instance);
    }
    else
    {
        MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR," Could not get device info for CDMA band %d",cdma_band);
        return NULL;
    }

} /* rfc_cdma_update_alt_rx_path */

#endif /* FEATURE_CDMA1X */

/*! @} */
