/*!
   @file
   rfc_card_tdscdma.cpp

   @brief
   This file contains all the TDSCDMA mode specific implementations to support RFC.
   These are platform independent.

*/

/*===========================================================================

Copyright (c) 2011 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:50 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/rf/rfc/src/rfc_card_tdscdma.cpp#1 $

when       who     what, where, why
-------------------------------------------------------------------------------
04/02/15   nsh     Smartly handling Rx2Tx and Tx2Rx scenario for GRFC ASM
11/14/14   qzh     Add check of if RFC TDS is created before calling TDS init in rfm_init
11/02/14   qzh     Get PA related timing from RFC file instead of using default value for GRFC PA
09/03/14   rsr     Changes to return TX LUT index when querying TX LUT info.
07/17/14   ms      Sawless RX support
07/10/14   jyu     Asd bringup with rfc changes and device script
02/10/14   kb      [Merge][XPT] Add support for porper shutdown of WTR during XPT deconfig.
12/13/13   cc      Unify gpio type to rfc_sig_cfg_type for FDD and TDD tech 
09/17/13   ms      Add api to get filtered band
07/26/13   ms      AsDiv support
06/26/13   jyu     Added support to remove the requirement to specify band info 
                   when it is not ready
05/20/13   al      Added functions for split band use
04/30/13   kb      [XPT] Support to set up feedback path and device for EPT
12/21/12   jyu     Modified the get_rf_device_obj function to return double pointer  
09/26/12   jyu     Changed to use logic device instead of logic path 
09/14/12   jyu     Added interface to return TxLUT type information from RFC
08/02/12   nsh     Interface change from "PATH" to "DEVICE"
08/02/12   jyu     Added support to query RFC for device ptr  
07/10/12   rmb     Removed critical section enter/exit and added these in rfc_common.c 
                   for wakeup/sleep function.
07/09/12   sr      changes to use the rfc_data object instead of rfc_ag function ptr table.
06/29/12   sr      RFC changes : use rf_hal_buffer to generate scripts 
06/12/12   zg      Fixed KW errors for sleep/wakeup functions. 
05/29/12   zg      Added new array rf_card_tdscdma_sig_list_notime[]. 
04/12/12   ycl     Added Sleep/wakeup.
04/05/12   sr      made changes to put all the GPIOs into low power mode during sleep. 
11/09/11   zg      Added functions for iRAT GRFC script generation  
08/15/11   zg      Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#include "rfc_card.h"
#include "rfc_card_tdscdma.h"
#include "rfcommon_core_error.h"
#include "rfc_common.h"
#include "rfcommon_core.h"
#include "rfc_tdscdma.h"
#include "rfc_tdscdma_data.h"
#include "rfc_common.h"
#include "rfc_common_tdd.h"


rfc_power_management_info rftdscdma_pm_info;

boolean prx_port_is_sawless = FALSE;
/* ----------------------------------------------------------------------- */
/*!
   @brief
   Placeholder of RF Card TDSCDMA rfc signal list without timing.

   @details
*/
rfc_sig_cfg_type rf_card_tdscdma_sig_list_notime[TDS_SIG_LIST_NOTIME_MAX_LENGTH]; 


/* ----------------------------------------------------------------------- */
/*!
  @brief
  Sets the specified device to the technology mode of operation.

  @details
  This funtion will call the card specific version of the set mode API.

  @parameter
  device: RF logic device

*/
void rfc_tdscdma_mode_config( rfm_device_enum_type device )
{    
  rfc_tdscdma *rfc_obj = rfc_tdscdma::get_instance();

  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_FATAL,"TDSCDMA RFC Obj is NOT created", 0);
    return;
  }

  rfc_obj->config_mode(device);

  return;
} /* rfc_tdscdma_config_mode() */

/* ----------------------------------------------------------------------- */
/*!
  @brief
  This function returns top level ptr to the TDSCDMA configuration.

  @details
  The function will return TDSCDMA configuration such as GPIO/GRFC settings along
  with any other parameter data structures for a particular band.  The void*
  allows for casting for usage under various band types.

  @parameter
  device: RF logic device
  config_data type of data to obtain

  @Returns
  void * ptr of the TDSCDMA configuration ptr.

*/
void *rfc_tdscdma_mode_config_get (rfm_device_enum_type device, rfc_mode_config_data_enum_type config_data)
{
  void *data_config_ptr = NULL;

  rfc_tdscdma *rfc_obj = rfc_tdscdma::get_instance();
  
  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_FATAL,"TDSCDMA RFC Obj is NOT created", 0);
    return (data_config_ptr);
  }

  data_config_ptr = rfc_obj->get_mode_config_data(device, config_data);

  return (data_config_ptr);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Provides generic get and set functionality for the RFC specific
  parameters.

  @details
  It provides IOCTL kind of interface to get and set RFC specific
  parameters.

  @param device: RF logic device
  @param cmd  : Specifies the command to performed by the device.
  @param data : Input and/or Output parameter for the command.

  @retval ret_val : generic return value which depends on the 'cmd' parameter.

*/
int32 rfc_tdscdma_command_dispatch( rfm_device_enum_type device, int32 cmd, void *data )
{
  rfc_tdscdma *rfc_obj = rfc_tdscdma::get_instance();

  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_FATAL,"TDSCDMA RFC Obj is NOT created", 0);
    return 0;
  }

  rfc_obj->command_dispatch(device, cmd, data);

  return 1;
}


/* ----------------------------------------------------------------------- */
/*!
  @brief
  Sets the specified device to the RF Tx band.

  @details
  This function will call the card specific version of the set Tx band API. 

  @parameter
  device: RF logic device
  band RF band

*/
void rfc_tdscdma_rx_band_config( rfm_device_enum_type device, rfcom_tdscdma_band_type band )
{
  rfc_tdscdma *rfc_obj = rfc_tdscdma::get_instance();

  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_FATAL,"TDSCDMA RFC Obj is NOT created", 0);
    return;
  }

  rfc_obj->rx_band_config(device, band);

  return;
} /* rfc_tdscdma_config_rx_band() */


/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function returns top level ptr to the requested Rx band configuration.

   @details
   The function will return Rx configuration such as GPIO/GRFC settings along
   with any other parameter data structures for a particular band.  The void*
   allows for casting for usage under various band types.

   @parameter
   device: RF logic device
   rfcom_tdscdma_band_type  TDSCDMA band for which RFC info requested.
   rfc_band_config_data_enum_type type of data to obtain

   @Returns
   void * ptr of the current Rx band configuration ptr.

*/
void *rfc_tdscdma_rx_band_config_get(rfm_device_enum_type device, rfcom_tdscdma_band_type band,
                                 rfc_band_config_data_enum_type config_data)
{
    void *data_config_ptr = NULL;
    rfc_tdscdma *rfc_obj = rfc_tdscdma::get_instance();

    if ( rfc_obj == NULL )
    {
      MSG_1(MSG_SSID_RF, MSG_LVL_FATAL,"TDSCDMA RFC Obj is NOT created", 0);
      return (data_config_ptr);
    }

    data_config_ptr = rfc_obj->rx_band_config_get(device, band, config_data);

    return (data_config_ptr);
}

/* ----------------------------------------------------------------------- */
/*!
  @brief
  Sets the specified device to the RF Tx band.

  @details
  This function will call the card specific version of the set Tx band API. 

  @parameter
  device: RF logic device
  band RF band

*/
void rfc_tdscdma_tx_band_config( rfm_device_enum_type device, rfcom_tdscdma_band_type band )
{
  rfc_tdscdma *rfc_obj = rfc_tdscdma::get_instance();

  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_FATAL,"TDSCDMA RFC Obj is NOT created. ",0);
    return;
  }

  rfc_obj->tx_band_config(device, band);

  return;
} /* rfc_tdscdma_config_tx_band() */


/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function returns top level ptr to the requested Tx band configuration.

   @details
   The function will return Tx configuration such as GPIO/GRFC settings along
   with any other parameter data structures for a particular band.  The void*
   allows for casting for usage under various band types.

   @parameter
   device: RF logic device
   rfcom_tdscdma_band_type  TDSCDMA band for which RFC info requested.
   rfc_band_config_data_enum_type type of data to obtain

   @Returns
   void * ptr of the current tx band configuration ptr.

*/
void *rfc_tdscdma_tx_band_config_get(rfm_device_enum_type  device, rfcom_tdscdma_band_type band,
                                 rfc_band_config_data_enum_type config_data)
{
    void *data_config_ptr = NULL;
    rfc_tdscdma *rfc_obj = rfc_tdscdma::get_instance();
    
    if ( rfc_obj == NULL )
    {
      MSG_1(MSG_SSID_RF, MSG_LVL_FATAL,"TDSCDMA RFC Obj is NOT created. ",0);
      return (data_config_ptr);
    }

    data_config_ptr = rfc_obj->tx_band_config_get(device, band, config_data);

    return (data_config_ptr);
}


/* ----------------------------------------------------------------------- */
/*!
   @brief
   Invokes MDSP function to configure GRFCs 
 
   @details
   This function invokes the MDSP function to configure the GRFCs specified
   by the mask to either static or timed mode.

*/
void rfc_tdscdma_set_grfc_omode(uint32 mask, uint32 data)
{

} /* rfc_tdscdma_set_grfc_omode */

/* ----------------------------------------------------------------------- */
/*!
   @brief
   Invokes MDSP function to configure GRFCs 
 
   @details
   This function invokes the MDSP function to configure the GRFCs specified
   by the mask to either output or input mode.
*/
void rfc_tdscdma_set_grfc_tristate(uint32 mask, uint32 data)
{
} /* rfc_tdscdma_set_grfc_tristate */

/* ----------------------------------------------------------------------- */
/*!
   @brief
   Invokes MDSP function to configure GRFCs 
 
   @details
   This function invokes the MDSP function to configure the GRFCs specified
   by the mask to either normal or inverted polarity. GRFCs' polarity is
   maintained as a shadow buffer which drives the output level accordingly
   when RFGSM_GRFC_OUT(...) is invoked.

*/
void rfc_tdscdma_set_grfc_polarity(uint32 mask, uint32 data)
{

} /* rfc_tdscdma_set_grfc_polarity */

/* ----------------------------------------------------------------------- */
/*!
   @brief
   Invokes MDSP function to configure GRFCs 
 
   @details
   Based on the previously configured polarity, the desired output level is
   set for the specified GRFCs. For normal polarity, setting a 0 for a GRFC
   will result in the GRFC outputting a 0. For inverted polarity, setting
   a GRFC to 0 will result in the GRFC outputting a 1.
 
   GRFC      GRFC     GRFC
   Polarity  Setting  Output
   --------  -------  ------
   Normal       0       0
   Normal       1       1
   Inverted     0       1
   Inverted     1       0

*/
void rfc_tdscdma_set_grfc_out(uint32 mask, uint32 data)
{

} /* rfc_tdscdma_set_grfc_out */

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function configures the VREGs for TDSCDMA mode of operation.

   @details
   This function configures the VREGs for TDSCDMA mode of operation.

   @parameter
   device: RF logic device
   band:  TDSCDMA band for which VREGS to be configures.
   state : TDSCDMA mode of operation.

   @Returns None
*/
void rfc_tdscdma_manage_vregs
( 
  rfm_device_enum_type  device, 
  rfcom_tdscdma_band_type band, 
  rfm_path_state state
)
{
  rfc_cfg_params_type cfg;
  rfc_device_info_type *device_info_ptr;
  rfc_rf_chip_number wtr_chip_num = RF_CHIP_0;

  cfg.alternate_path = 0;
  cfg.band = band;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  cfg.logical_device = device;
  cfg.rx_tx = RFC_CONFIG_RX;

  rfc_tdscdma *rfc_obj = rfc_tdscdma::get_instance();
  
  rf_path_enum_type path;
  path = rfcommon_core_device_to_path(device);

  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_FATAL,"TDSCDMA RFC Obj is NOT created", 0);
    return;
  }

  /* Protect the rfc sleep/wakeup functions with critical section */
  rf_common_enter_critical_section(rfc_get_lock_data());

  if (  state == RF_PATH_SLEEP_STATE)
  {
    /* disable the rf signals, put the signals into low power mode */
    rfc_common_sleep(path, RFM_TDSCDMA_MODE);
  }
  else
  {
    /* enable the rf signals, if the signals are in low power mode */
    rfc_common_wakeup(path, RFM_TDSCDMA_MODE);
  }

  rf_common_leave_critical_section(rfc_get_lock_data());

  /* Get RFC information */
  if (rfc_tdscdma_get_devices_info_data(&cfg, &device_info_ptr) == TRUE)
  {
     wtr_chip_num = (rfc_rf_chip_number)(device_info_ptr->rf_asic_info[0].instance);
     MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"rfc_tdscdma_manage_vregs:RFC TDSCDMA chip_num %d",wtr_chip_num);
  }
  else
  {
     MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_tdscdma_manage_vregs:RFC TDSCDMA chip_num to default: %d",wtr_chip_num);
  }
  
  /*rftdscdma_pm_info structure is filled out here so all cards version can reference */
  rftdscdma_pm_info.mode = RFM_TDSCDMA_MODE;
  rftdscdma_pm_info.band = band;
  rftdscdma_pm_info.path = path;
  rftdscdma_pm_info.state = state;
  rftdscdma_pm_info.rf_chip_num = wtr_chip_num;  
  
  rfc_obj->manage_vregs(&rftdscdma_pm_info);

  return;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function generates concerts the format of the TDSCDMA GRFCs signal list
  
  @param input_grfc_tbl_ptr
  Pointer to the input grfc script table

  @param output_grfc_tbl_ptr
  Pointer to the output grfc script table
*/

void rfc_tdscdma_convert_sig_cfg_type
(
  rfc_sig_cfg_type* input_grfc_tbl_ptr, 
  rfc_sig_cfg_type* output_grfc_tbl_ptr
)
{
  uint16 i = 0;
  while( (input_grfc_tbl_ptr[i].sig_name != RFC_SIG_LIST_END) && 
         (i < (RFC_MAX_NUM_OF_GRFC -1) )
       )
  {
    output_grfc_tbl_ptr[i].sig_name = input_grfc_tbl_ptr[i].sig_name;
    output_grfc_tbl_ptr[i].start.logic = input_grfc_tbl_ptr[i].start.logic;
    i++;
  }
  output_grfc_tbl_ptr[i].sig_name = RFC_SIG_LIST_END;
  return;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function generates the TDSCDMA GRFCs script for the requested device
  and band.

  @param device
  RF device for which to generate GRFC script

  @param band
  Band for which to generate GRFC script
 
  @param grfc_buf_ptr
  Output - The GRFC script
 
  @param is_irat
  Indicates if the API is being called during IRAT operation
*/
void rfc_meas_tdscdma_generate_grfc_script
(
  uint32 band, 
  rfc_grfc_device_type *device, 
  rfc_grfc_script_type *grfc_buf_ptr,
  boolean is_irat 
)
{
  rfc_sig_cfg_type *rfc_tds_prx_grfc_tbl_ptr = NULL;
  rfc_sig_cfg_type rfc_prx_grfc_tbl_ptr[RFC_MAX_NUM_OF_GRFC];


  /* =========================================================================
  * Get grfc table pointers for particular band and mode. 
  * ========================================================================== 
  */ 
  if(device->primary_rx_device == RFM_INVALID_DEVICE)
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR,
          "rfc_meas_tds_generate_rx_grfc_script: Invalid device",0 );
    return;
  }
  else
  {   
    rfc_tds_prx_grfc_tbl_ptr = 
        (rfc_sig_cfg_type *)rfc_tdscdma_rx_band_config_get( 
            device->primary_rx_device, (rfcom_tdscdma_band_type)band, RFC_GRFC_DATA
            );
  }
     
  /* =========================================================================
  *  For each valid pointer, Look up GRFC Engine Number 
  *         and populate the IRAT GRFC buffer with correct logic.
  * ========================================================================== 
  */ 
  if( rfc_tds_prx_grfc_tbl_ptr == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR,
          "rfc_meas_tds_generate_rx_grfc_script: NULL tbl, band=%d",band);
    return;
  }
  { 
    grfc_buf_ptr->num_grfc = 0;
    rfc_tdscdma_convert_sig_cfg_type(rfc_tds_prx_grfc_tbl_ptr,rfc_prx_grfc_tbl_ptr);

    /* Dime RFC changes: Call the following API with rf_hal_buffer ptr */
    //rfc_common_generate_grfc_script( rfc_prx_grfc_tbl_ptr, grfc_buf_ptr );
  }
  
  grfc_buf_ptr->tech = RFCOM_TDSCDMA_MODE;
}


/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function configures RFC Signals into wakeup state.

   @details
   This function configures RFC Signals into wakeup state.

   @parameter
   device: RF logic device

   @Returns None
*/
void rfc_tdscdma_wakeup
( 
  rfm_device_enum_type  device
)
{
  rfc_tdscdma *rfc_obj = rfc_tdscdma::get_instance();

  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"TDSCDMA RFC Obj is NOT created", 0);
    return;
  }

  rfc_obj->wakeup(device);
}


/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function configures RFC Signals into sleep state.

   @details
   This function configures RFC Signals into sleep state.

   @parameter
   device: RF logic device

   @Returns None
*/
void rfc_tdscdma_sleep
( 
  rfm_device_enum_type  device 
) 
{
  rfc_tdscdma *rfc_obj = rfc_tdscdma::get_instance();

  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"TDSCDMA RFC Obj is NOT created", 0);
    return;
  }

  rfc_obj->sleep(device);
}


/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function return device ptr.

   @details
   This function return device ptr.

   @parameter
   device: RF logic device

   @Returns None
*/
void** rfc_tdscdma_get_rf_device_obj
( 
  rfm_device_enum_type rfm_dev, rfc_rxtx_enum_type rx_tx,
  rfcom_tdscdma_band_type band, rfdevice_type_enum_type dev_type
) 
{
  void** data_config_ptr = NULL;
  rfc_tdscdma *rfc_obj = rfc_tdscdma::get_instance();

  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_FATAL,"TDSCDMA RFC Obj is NOT created",0);
    return (data_config_ptr);
  }

  if (band == RFCOM_BAND_TDSCDMA_ANY_BAND)
  {
    // loop all possible band to find the 1st one that returns a valid device_ptr
    uint8 loop_band;

    for (loop_band = RFCOM_BAND_TDSCDMA_B34; loop_band < RFCOM_NUM_TDSCDMA_BANDS; loop_band++)
    {
      data_config_ptr = rfc_obj->get_rf_device_obj(rfm_dev, rx_tx, (rfcom_tdscdma_band_type)loop_band, dev_type);
      //MSG_1(MSG_SSID_RF, MSG_LVL_MED, "Debug: data_config_ptr", data_config_ptr);
      if (data_config_ptr != NULL)
      {
        //MSG_1(MSG_SSID_RF, MSG_LVL_MED, "Debug: data_config_ptr[0]", data_config_ptr[0]);
        //MSG_1(MSG_SSID_RF, MSG_LVL_MED, "Debug: data_config_ptr[1]", data_config_ptr[1]);
        //MSG_1(MSG_SSID_RF, MSG_LVL_MED, "Debug: data_config_ptr[2]", data_config_ptr[2]);

        if (data_config_ptr[0] != NULL)
        {
        MSG_1(MSG_SSID_RF, MSG_LVL_MED, "TDSCDMA RFC Obj obtained for band %d (when RFCOM_BAND_TDSCDMA_ANY_BAND is passed)", loop_band);
        break;
      }
    }
    }

    if (loop_band == RFCOM_NUM_TDSCDMA_BANDS)
    {
      MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, "TDSCDMA RFC Obj was not initialized for any TDSCDMA band!", 0);
    }
  }
  else
  {
    data_config_ptr = rfc_obj->get_rf_device_obj(rfm_dev, rx_tx, band, dev_type);
  }

  return (data_config_ptr);
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function return band specific PA LUT type information.

   @details
   This function return band specific PA LUT type information.

   @parameter
   device: logic device in rfm_device_type
 
   @parameter
   band:   TDSCDMA band type
 
   @parameter
   rf_asic_band_pa_lut_map*:  Use to return band specific pa lut type information

   @Returns None
*/
void rfc_tdscdma_get_band_pa_lut_type
(
  rfm_device_enum_type  device, 
  rfcom_tdscdma_band_type band, 
  uint32* rf_asic_band_pa_lut_map
)
{   
  rfc_tdscdma *rfc_obj = rfc_tdscdma::get_instance();

  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_FATAL,"TDSCDMA RFC Obj is NOT created", 0);
    return ;
  }

  if ( rf_asic_band_pa_lut_map == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_FATAL, "rf_asic_band_pa_lut_map ptr is empty. ", 0);
    return ;
  }

  rfc_obj->get_band_pa_lut_type(device, band, rf_asic_band_pa_lut_map);
}

/* ----------------------------------------------------------------------- */
/*!
  @brief
  calls RFC function to setup feedback path
  
  @details
  
  @param 
  
  @return
  
*/
boolean rfc_tdscdma_setup_fb_path (rfm_device_enum_type device_fb, rfcom_tdscdma_band_type band, boolean xpt_enable)
{
  boolean api_status = TRUE ;
  rfc_tdscdma *rfc_obj = rfc_tdscdma::get_instance();
	
  api_status = rfc_obj->setup_fb_path(device_fb, band, xpt_enable);
	
  return(api_status);
	
}

/* ----------------------------------------------------------------------- */
/*!
  @brief
  calls RFC function to configure feedback device
  
  @details
  
  @param 
  
  @return
  
*/

boolean rfc_tdscdma_setup_fb_device 
(
   rfm_device_enum_type device, 
   rfm_device_enum_type device_fb,
   rfcom_tdscdma_band_type band, 
   uint16 tx_chan, 
   boolean xpt_enable
)
{
  boolean api_status = TRUE ;
  rfc_tdscdma *rfc_obj = rfc_tdscdma::get_instance();
	
  api_status = rfc_obj->setup_fb_device(device, device_fb, band, tx_chan, xpt_enable);
	
  return(api_status);
	
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function returns the rx band info. for the specified rx channel by querying RFC.

   @param
   split_band: Pointer to boolean which will be set to true if split band is found
   rx_chan: earfcn passed
   rfc_band: pointer to tdscdma band, which will be updated if split band

*/

void rfc_tdscdma_get_rx_band(boolean *split_band, uint16 rx_chan, rfcom_tdscdma_band_type *rfc_band)
{
  rfc_tdscdma *rfc_tdscdma_p = rfc_tdscdma::get_instance();
  if(rfc_tdscdma_p == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfc_tdscdma_get_device:RFC TDSCDMA object not initialized");
  }
  else
  {
    rfc_tdscdma_p->get_rx_band(split_band, rx_chan, rfc_band);
  }
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function returns the tx band info. for the specified tx channel by querying RFC.

   @param
   split_band: Pointer to boolean which will be set to true if split band is found
   tx_chan: earfcn passed
   rfc_band: pointer to tdscdma band, which will be updated if split band

*/
void rfc_tdscdma_get_tx_band(boolean *split_band, uint16 tx_chan, rfcom_tdscdma_band_type *rfc_band)
{
   rfc_tdscdma *rfc_tdscdma_p = rfc_tdscdma::get_instance();

  if(rfc_tdscdma_p == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfc_tdscdma_get_device:RFC TDSCDMA object not initialized");
  }
  else
  {
    rfc_tdscdma_p->get_tx_band(split_band, tx_chan, rfc_band);
  }
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function returns the filtered band info. for the specified tx channel by querying RFC.

   @param
   rfc_band: pointer to tdscdma band, which will be updated if filtered band

*/
void rfc_tdscdma_get_filtered_band(rfcom_tdscdma_band_type band, rfc_coex_filter_status_type filter_status, rfcom_tdscdma_band_type *rfc_band)
{
   rfc_tdscdma *rfc_tdscdma_p = rfc_tdscdma::get_instance();

  if(rfc_tdscdma_p == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfc_tdscdma_get_device:RFC TDSCDMA object not initialized");
  }
  else
  {
    rfc_tdscdma_p->get_filtered_band(band, filter_status, rfc_band);
  }
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function returns the low and high freq for the split band
 
   @param
   split_band: band for which low and high channels are desired
   *low_earfcn: Pointer to low chan
   *high_earfcn: Pointer to high chan
 
   @return

*/
void rfc_tdscdma_get_split_band_earfcns(rfcom_tdscdma_band_type split_band, uint16 *low_earfcn, uint16 *high_earfcn)
{
   rfc_tdscdma *rfc_tdscdma_p = rfc_tdscdma::get_instance();

  if(rfc_tdscdma_p == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfc_tdscdma_get_device:RFC TDSCDMA object not initialized");
  }
  else
  {
    rfc_tdscdma_p->get_tx_hi_lo_earfcn(split_band, low_earfcn, high_earfcn);
  }
}

/* ----------------------------------------------------------------------- */
void* 
rfc_tdscdma_get_asd_xsw_config
(
  rfm_device_enum_type device,
  rfcom_tdscdma_band_type band,
  uint32 position
)
{
  rfc_sig_cfg_type *rfc_asd_sig_info = NULL;
  rfc_tdscdma *rfc_tdscdma_obj = rfc_tdscdma::get_instance();

  if ( rfc_tdscdma_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"tdscdma RFC Obj is NOT created. ",0);
    return rfc_asd_sig_info ;
  }

  rfc_asd_sig_info = 
    (rfc_sig_cfg_type *) rfc_tdscdma_obj->get_asd_xsw_config( device,
                                                             band,
                                                             (int)position );


  return rfc_asd_sig_info;

}

/* ----------------------------------------------------------------------- */
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
#ifdef FEATURE_RF_ASDIV  
boolean
rfc_tdscdma_get_asdiv_xsw_script
( 
  uint32 position,
  rf_buffer_intf *asdiv_script_ptr,
  void* cb_data
)
{
  boolean status;
  rfc_tdscdma *rfc_tdscdma_obj = rfc_tdscdma::get_instance();


  if ( asdiv_script_ptr == NULL )
  {
    RF_MSG( RF_ERROR, "generate_asdiv_xsw_script : scrpit ptr is "
                      "null"); 
    return FALSE;
  }

  if ( rfc_tdscdma_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"tdscdma RFC Obj is NOT created. ",0);
    return FALSE ;
  }

  status = rfc_tdscdma_obj->generate_asdiv_xsw_script((int)position ,asdiv_script_ptr,
                                                             cb_data
                                                             );


  if( status == FALSE )
  {
    RF_MSG( RF_ERROR, "rfc_tdscdma_get_asdiv_xsw_script : "
                      "script generation failed");
  }

  return status;
} /* rfc_tdscdma_get_asdiv_xsw_script */
#endif

rfdevice_rxtx_common_class * rfc_tdscdma_get_common_rxtx_device
(
  rfm_device_enum_type device, 
  rfcom_tdscdma_band_type band, 
  uint32 alt_path
)
{
  rfc_device_info_type *device_info_ptr = NULL;
  /*Device type used to extract tech specific object*/
  rfdevice_type_enum_type dev_type = RFDEVICE_TYPE_INVALID;
  /*Device instance used to extract tech specific object*/
  uint32 instance = 0;
  rfc_cfg_params_type cfg;
  

  cfg.alternate_path = alt_path;
  cfg.band = band;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  cfg.logical_device = device;
  cfg.rx_tx = RFC_CONFIG_TX;
  cfg.path = rfcommon_core_device_to_path(device);
  /* Get RFC information */
  if (rfc_tdscdma_get_devices_info_data(&cfg, &device_info_ptr) == TRUE)
  {
     instance = device_info_ptr->rf_asic_info[0].instance;
     /*Retrieve device object*/
     return rfc_common_get_cmn_device_object((uint8)instance);
  }
  else
  {
     MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR," Could not get device info for TDSCDMA band ",band);
     return NULL;
  }

  
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function returns if GRFC PA existed for TDSCDMA.

   @param
   None

*/
boolean rfc_tdscdma_grfc_pa_existed(void)
{
   rfc_tdscdma *rfc_tdscdma_p = rfc_tdscdma::get_instance();

  if(rfc_tdscdma_p == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_FATAL, "rfc_tdscdma_get_device:RFC TDSCDMA object not initialized");
    return FALSE;
  }
  else
  {
    return (rfc_tdscdma_p->grfc_pa_existed());
  }
}

/* ----------------------------------------------------------------------- */
/*!
  @brief
  Returns whether TDS RFC device is created.
  
  @param
 
  @return TRUE if RFC device is created, FALSE otherwise
  
*/
boolean rfc_tdscdma_is_rfc_tds_created(void)
{
  rfc_tdscdma *rfc_obj = rfc_tdscdma::get_instance();

  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"TDSCDMA RFC Obj is NOT created. ",0);
    return FALSE;
  }

  return TRUE;
}



/* ----------------------------------------------------------------------- */
/*!
  @brief
  Handle the ANT_SEL GRFC signal during Tx to Rx scenario
  1. Will turn on Rx anyway
  2. For Tx, will skip the shared signal with Rx, only turn off the one which is not in Rx logic
  
  @param
  	prx_device,
  	drx_device,
  	tx_deivce,
  	band
 
  @return
  
*/
boolean rfc_tdscdma_tx_rx_ant_sel_control(rfm_device_enum_type prx_device,
                                               rfm_device_enum_type drx_device,
                                               rfm_device_enum_type tx_device,              
                                               rfcom_tdscdma_band_type band,
                                               boolean drx_enable,
                                               rf_buffer_intf * rf_buffer)
{

  rfc_sig_cfg_type* tx_band_config_ptr = NULL;
  rfc_sig_cfg_type* prx_band_config_ptr = NULL;
  rfc_sig_cfg_type* drx_band_config_ptr = NULL;
  rfc_sig_cfg_type* rxd_band_config_ptr = NULL;
  
  boolean fetch_script_status = TRUE;


  /*Part 1. Will turn on Rx by default*/

  prx_band_config_ptr = (rfc_sig_cfg_type*) rfc_tdscdma_rx_band_config_get(prx_device, band, RFC_GRFC_DATA);

  fetch_script_status &= rfc_common_tdd_get_grfc_buffer(prx_band_config_ptr, 
                                                        RFC_ANT_SEL, 
                                                        RFC_START_SCRIPT, 
                                                        rf_buffer, 
                                                        0, 
                                                        NULL, 
                                                        FALSE, 
                                                        FALSE); 


  if (drx_enable)
  {
     drx_band_config_ptr = (rfc_sig_cfg_type*) rfc_tdscdma_rx_band_config_get(drx_device, band, RFC_GRFC_DATA);

     fetch_script_status &= rfc_common_tdd_get_grfc_buffer(drx_band_config_ptr, 
                                                        RFC_ANT_SEL, 
                                                        RFC_START_SCRIPT, 
                                                        rf_buffer, 
                                                        0, 
                                                        NULL, 
                                                        FALSE, 
                                                        FALSE); 
  
  }


  /* Part 2: GRFC  Tx Ant_Sel:	STOP LOGIC  		   */
  tx_band_config_ptr = (rfc_sig_cfg_type*)rfc_tdscdma_tx_band_config_get(tx_device, band, RFC_GRFC_DATA);



  /*    Now we have 
  
           prx_band_config_ptr for prx signals
           drx_band_config_ptr for drx signas.

           tx_band_config_ptr for tx signals

           we need to delete the shared signals with rx in tx signals, and come out with a final tx signal, which is tx_band_config_ptr
                                
       
  */

  rxd_band_config_ptr = rfc_tdscdma_combine_two_signal_list(prx_band_config_ptr, drx_band_config_ptr);

  tx_band_config_ptr = rfc_tdscdma_get_unique_signal_from_seta_to_setb(tx_band_config_ptr, rxd_band_config_ptr);
  

  fetch_script_status &= rfc_common_tdd_get_grfc_buffer(tx_band_config_ptr, 
                                                        RFC_ANT_SEL, 
                                                        RFC_STOP_SCRIPT, 
                                                        rf_buffer, 
                                                        0, 
                                                        NULL, 
                                                        FALSE, 
                                                        FALSE);   



  return fetch_script_status;
}


/* ----------------------------------------------------------------------- */
/*!
  @brief
  Handle the ANT_SEL GRFC signal during Rx to Tx scenario
  1. Will turn on Tx anyway
  2. For Rx, will skip the shared signal with Rx, only turn off the one which is not in Tx logic
  
  @param
  	prx_device,
  	drx_device,
  	tx_deivce,
  	band
 
  @return
  
*/
boolean rfc_tdscdma_rx_tx_ant_sel_control(rfm_device_enum_type prx_device,
                                               rfm_device_enum_type drx_device,
                                               rfm_device_enum_type tx_device,              
                                               rfcom_tdscdma_band_type band,
                                               boolean drx_enable,
                                               rf_buffer_intf * rf_buffer)
{

  rfc_sig_cfg_type* tx_band_config_ptr = NULL;
  rfc_sig_cfg_type* prx_band_config_ptr = NULL;
  rfc_sig_cfg_type* drx_band_config_ptr = NULL;
  rfc_sig_cfg_type* rxd_band_config_ptr = NULL;
  
  boolean fetch_script_status = TRUE;


  /*Part 1. Will turn on Tx by default*/
  tx_band_config_ptr = (rfc_sig_cfg_type*) rfc_tdscdma_tx_band_config_get(tx_device, band, RFC_GRFC_DATA);

  /* Comment out this part since Turning ON Tx has been handled in common Tx AGC


  fetch_script_status &= rfc_common_tdd_get_grfc_buffer(tx_band_config_ptr, 
                                                        RFC_ANT_SEL, 
                                                        RFC_START_SCRIPT, 
                                                        rf_buffer, 
                                                        0, 
                                                        NULL, 
                                                        FALSE, 
                                                        FALSE);  

  */

  prx_band_config_ptr = (rfc_sig_cfg_type*) rfc_tdscdma_rx_band_config_get(prx_device, band, RFC_GRFC_DATA);
  rxd_band_config_ptr = prx_band_config_ptr;

  if (drx_enable)
  {
     drx_band_config_ptr = (rfc_sig_cfg_type*) rfc_tdscdma_rx_band_config_get(drx_device, band, RFC_GRFC_DATA);

	 /*Combine the two signal lists when there is RxD*/
	 rxd_band_config_ptr = rfc_tdscdma_combine_two_signal_list(prx_band_config_ptr, drx_band_config_ptr);
  }


  /*    Now we have 
  
           prx_band_config_ptr for prx signals
           drx_band_config_ptr for drx signas.

           tx_band_config_ptr for tx signals

           we need to delete the shared signals with rx in tx signals, and come out with a final tx signal, which is tx_band_config_ptr
  */

  rxd_band_config_ptr = rfc_tdscdma_get_unique_signal_from_seta_to_setb(rxd_band_config_ptr, tx_band_config_ptr);

  fetch_script_status &= rfc_common_tdd_get_grfc_buffer(rxd_band_config_ptr, 
                                                        RFC_ANT_SEL, 
                                                        RFC_STOP_SCRIPT, 
                                                        rf_buffer, 
                                                        0, 
                                                        NULL, 
                                                        FALSE, 
                                                        FALSE);  

  //ASSERT(0);

  return fetch_script_status;
}


/*Use global variable for easy trakcing*/
static rfc_sig_cfg_type rf_signals_c[30];

/* ----------------------------------------------------------------------- */
/*!
  @brief
  Combine the two rf_signal lists into one, and return the final concatenated signal lists
  
  @param
  	rf_signals_a,
  	rf_signals_b,

 
  @return
       rf_signals_c = rf_signals_a + rf_signals_b
  
*/
rfc_sig_cfg_type * rfc_tdscdma_combine_two_signal_list(rfc_sig_cfg_type * rf_signals_a,
	                                                   rfc_sig_cfg_type * rf_signals_b)
{

	if ((rf_signals_a == NULL) && (rf_signals_b == NULL))
	{
	   /*Two signal lists are both 0, print out error msg*/
	   MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"Both signal lists are NULL. ",0);
	   return NULL;
	}
	else if (rf_signals_a == NULL)
	{
	   /*Print out that a is null, and return b*/
	   MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"rf_signals_a is NULL, will return rf_signals_b ",0);
	   return rf_signals_b;
	}
	else if (rf_signals_b == NULL)
	{
	   /*Print out that b is NULL, and return a*/
	   MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"rf_signals_B is NULL, will return rf_signals_a ",0);
	   return rf_signals_a;
	}
	else
	{
	    int i=0, j=0;

		/*Assign the first half of rf_signals_c with rf_signals_a*/
		while (rf_signals_a[i].sig_name != RFC_SIG_LIST_END)
		{
		   rf_signals_c[i].sig_name = rf_signals_a[i].sig_name;
		   rf_signals_c[i].start = rf_signals_a[i].start;
		   rf_signals_c[i].stop = rf_signals_a[i].stop;
		   i++;
		}

        /*Assign the 2nd half of rf_signals_c with rf_signals_b*/
        while (rf_signals_b[j].sig_name != RFC_SIG_LIST_END)
		{
		   rf_signals_c[i].sig_name = rf_signals_b[j].sig_name;
		   rf_signals_c[i].start = rf_signals_b[j].start;
		   rf_signals_c[i].stop = rf_signals_b[j].stop;
		   i++;
		   j++;
		}

        /*Marked the end of rf_signals_c*/
		rf_signals_c[j].sig_name = RFC_SIG_LIST_END;

		return rf_signals_c;
		
		
	}

}


static rfc_sig_cfg_type rf_signals_unique[30];
/* ----------------------------------------------------------------------- */
/*!
  @brief
  polling the signals in A, if it is not in B, copy it to rf_signals_c
  then return rf_signals_c
  
  @param
  	rf_signals_a,
  	rf_signals_b,

 
  @return
       rf_signals_c = rf_signals_a - rf_signals_b
  
*/
rfc_sig_cfg_type * rfc_tdscdma_get_unique_signal_from_seta_to_setb(rfc_sig_cfg_type * rf_signals_a,
	                                                               rfc_sig_cfg_type * rf_signals_b)
{
	if ((rf_signals_a == NULL) || (rf_signals_b == NULL))
	{
	   /*Two signal lists are both 0, print out error msg*/
	   MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"Either signal lists are NULL. ",0);
	   return NULL;
	}
    else
	{
	   int i=0,j=0, k=0;
	   boolean signal_matched = FALSE;

	   while (rf_signals_a[i].sig_name != RFC_SIG_LIST_END)
	   {
	      signal_matched = FALSE;

		  j=0;
          while(rf_signals_b[j].sig_name != RFC_SIG_LIST_END)
          {
             if (rf_signals_a[i].sig_name == rf_signals_b[j].sig_name)
             {
			 	signal_matched = TRUE;
			    break;
             }
		     j++;
          }

		  if(signal_matched == FALSE)
		  {
		     /*Then copy this signal*/
			 rf_signals_unique[k].sig_name = rf_signals_a[i].sig_name;
		     rf_signals_unique[k].start = rf_signals_a[i].start;
		     rf_signals_unique[k].stop = rf_signals_a[i].stop;
			 k++;
		  }
		  i++;
	   }

	   rf_signals_unique[k].sig_name = RFC_SIG_LIST_END;

	   return rf_signals_unique;
	}

	
}


