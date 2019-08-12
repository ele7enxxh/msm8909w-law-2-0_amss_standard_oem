/*!
  @file
  rfdevice_asm_tuner.cpp

  @brief
  Common interface for 3rd party ASM Tuner devices

*/

/*==============================================================================

  Copyright (c) 2015 - 2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_asm/src/rfdevice_asm_tuner.cpp#1 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
01/23/15   sr     Initial version

==============================================================================*/
#include <stringl/stringl.h>

#ifdef __cplusplus
extern "C"
{
#endif

// Include C specific header files
#include "rfm_internal.h"

#ifdef __cplusplus
}/* extern "C"*/
#endif

#include "rfdevice_asm_tuner.h"
#include "rf_buffer_intf.h"
#include "rf_hal_bus_types.h"
#include "rfcommon_msg.h"
#include "rfcommon_time_profile.h"
#include "rf_rffe_common.h"
#include "rfc_common.h"
#include "rfdevice_msg_log.h"
#include "rfdevice_asm_data.h"


// 2 bits of GRFC logic per scenario in the lower 16 bits.
#define GET_SCENARIO_GRFC_LOGIC(grfc_data, scenario) ( (grfc_data >> (scenario *2)) & 0x3)

// 16 MSB bits will will have the rfc_signal number
#define GET_RFC_SIGNAL_NUM(grfc_data) ( (grfc_data >> 16) & 0xFFFF)

/*!
  @brief
  create 3rd party asm tuner devices

  @details
  create 3rd party asm tuner devices and register itself to the parent physical device object passed to it.

  @param rfdevice_physical_ptr : parent physical device object of the asm tuner device
  @param logical_device_cfg : logical device info of the asm tuner device
*/

rfdevice_logical_component* create_gen_asm_tuner_object(rfdevice_physical_device *rfdevice_physical_ptr,rfc_logical_device_info_type *logical_device_cfg)
{
  boolean init_status = FALSE;
  rfdevice_logical_component *temp_obj = NULL;
  rfc_phy_device_info_type *phy_device_cfg=NULL;

  /*NULL pointer check*/
  if ( (rfdevice_physical_ptr == NULL)           ||
       (logical_device_cfg == NULL)              ||
       ( ((rfdevice_physical_third_party*)rfdevice_physical_ptr)->third_party_phy_dev_cfg == NULL) )
  {
    RF_MSG( RF_ERROR, "create_gen_asm_tuner_object failed ! NULL Pointer detected");
    return NULL;
  }

  phy_device_cfg=((rfdevice_physical_third_party*)rfdevice_physical_ptr)->third_party_phy_dev_cfg;

  temp_obj = new rfdevice_asm_tuner(rfdevice_physical_ptr, phy_device_cfg, logical_device_cfg); /* create the ASM device */

  if (temp_obj !=NULL)
  {
    init_status = TRUE;
    temp_obj = ( ((rfdevice_physical_third_party*)rfdevice_physical_ptr)->store_object(temp_obj) );
  }
  else
  {
    RF_MSG( RF_ERROR, "rfdevice_physical_third_party GEN_ASM creation failed ! returning NULL");
  }

  return temp_obj;

}

/*==============================================================================

                      ASM Tuner object maintenance functions

==============================================================================*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
   Overloaded Constructor for physical device support

*/
rfdevice_asm_tuner::rfdevice_asm_tuner(rfdevice_physical_device *phy_dev_obj_ptr,
                                       rfc_phy_device_info_type *phy_device_info,
                                       rfc_logical_device_info_type *logical_device_info)
{
  uint8 rxtx = 0;
  uint8 band = 0;
  if((phy_device_info == NULL) || (logical_device_info == NULL) || 
     (logical_device_info->rf_device_type != RFDEVICE_ASM))
  {
    this->init_status = FALSE;
    print_device_info();
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"ASM Settings Data not available!");
    return;
  }

  this->init_status = TRUE;
  this->band_data_size = 0;
  this->scenario = 0;

  /*Update the parent physical device object ptr to the logical component*/
  rfdevice_asm_tuner::parent_physical_device_ptr=phy_dev_obj_ptr;

  /*! Assign all device information to the object member variables */
  this->rf_device_id = logical_device_info->rf_device_id;
  this->rf_device_type = logical_device_info->rf_device_type;

  /*! Initialize default ASM Tuner band data for all techs */
  for (uint8 rfm_dev = 0; rfm_dev < RFM_MAX_WAN_DEVICES; rfm_dev++)
  {
    for (rxtx=0; rxtx < RFDEVICE_ASM_RXTX_MAX; rxtx++)
    {
      for(band = 0; band < RFM_CDMA_MAX_BAND; band++)
      {
        this->cdma_band_data[rfm_dev][band][rxtx]  = NULL;  
      }

      /* Set all WCDMA port information to invalid */
      for(band = 0; band < RFCOM_NUM_WCDMA_BANDS; band++)
      {
        this->wcdma_band_data[rfm_dev][band][rxtx]  = NULL;
      }

      /* Set all GSM port information to invalid */
      for(band = 0; band < RFCOM_NUM_GSM_BANDS; band++)
      {
        this->gsm_band_data[rfm_dev][band][rxtx]  = NULL;
      }

      /* Set all LTE port information to invalid */
      for(band = 0; band < RFCOM_NUM_LTE_BANDS; band++)
      {
        this->lte_band_data[rfm_dev][band][rxtx]  = NULL;
      }

      /* set all Tdscdma port information to invalid*/
      for(band = 0; band < RFCOM_NUM_TDSCDMA_BANDS; band++)
      {
        this->tdscdma_band_data[rfm_dev][band][rxtx]  = NULL;
      }
    }
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Destructor

*/
rfdevice_asm_tuner::~rfdevice_asm_tuner()
{
  // nothing to be done
}

/*==============================================================================

                      ASM device API implementation

==============================================================================*/


/*!
  @brief
  Checks parameter validity

  @details
  This function can be used to check the parameter validity passed onto the ASM by the techs

  @param mode : enum indicating the radio access technology
  @param band : enum indicating band number
  @param script_buffer : void pointer to pass address of buffer script
  @param execution_type : boolean to indicate write to hw or prepare script
  @param script_timing : time offset for the script where a negative value refers to the time BEFORE the event
*/
boolean rfdevice_asm_tuner::verify_params(rfdevice_asm_cfg *asm_cfg_p,
                                          rf_buffer_intf *script_buffer,
                                          rf_device_execution_type execution_type,
                                          int16 script_timing)
{
  boolean status = TRUE;

  if (asm_cfg_p == NULL)
  {
    RF_MSG( RF_ERROR, "asm_cfg_p parameter NULL for ASM request");
    return FALSE;
  }

  /* Null pointer check */
  if( execution_type == RFDEVICE_CREATE_SCRIPT && script_buffer == NULL)
  {
    RF_MSG( RF_ERROR, "Script not allocated for ASM request");
    return FALSE;
  }

  switch (asm_cfg_p->mode)
  {
  case RFCOM_WCDMA_MODE:
    if(asm_cfg_p->band.wcdma_band >= RFCOM_NUM_WCDMA_BANDS)
    {
      RF_MSG_1( RF_ERROR, "Invalid WCDMA band %d", asm_cfg_p->band.wcdma_band);
      status = FALSE;
    }
    break;
  case RFCOM_1X_MODE:
  case RFCOM_1XEVDO_MODE:
    if(asm_cfg_p->band.cdma_band >= RFM_CDMA_MAX_BAND)
    {
      RF_MSG_1( RF_ERROR, "Invalid CDMA band %d", asm_cfg_p->band.cdma_band);
      status = FALSE;
    }
    break;
  case RFCOM_GSM_MODE:
    if(asm_cfg_p->band.gsm_band >= RFCOM_NUM_GSM_BANDS)
    {
      RF_MSG_1( RF_ERROR, "Invalid GSM band %d", asm_cfg_p->band.gsm_band);
      status = FALSE;
    }
    break;
  case RFCOM_LTE_MODE:
    if(asm_cfg_p->band.lte_band >= RFCOM_NUM_LTE_BANDS)
    {
      RF_MSG_1( RF_ERROR, "Invalid LTE band %d", asm_cfg_p->band.lte_band);
      status = FALSE;
    }
    break;
  case RFCOM_TDSCDMA_MODE:
    if(asm_cfg_p->band.tdscdma_band >= RFCOM_NUM_TDSCDMA_BANDS)
    {
      RF_MSG_1( RF_ERROR, "Invalid TDSCDMA band %d", asm_cfg_p->band.tdscdma_band);
      status = FALSE;
    }
    break;
  default:
    RF_MSG_1(RF_ERROR, "ASM request for invalid tech %d", asm_cfg_p->mode);
    status = FALSE;
    break;
  }

  return status;
}

/*----------------------------------------------------------------------------*/
boolean rfdevice_asm_tuner::enable(rfdevice_asm_cfg *asm_cfg_p,
                                   rf_buffer_intf *buff_obj_ptr,
                                   rf_device_execution_type dev_action,
                                   int16 script_timing)
{
  boolean ret_val = TRUE;
  int32 *band_data = NULL;

  //Validity check for requested ASM settings
  if(!verify_params(asm_cfg_p,buff_obj_ptr, dev_action, script_timing))
  {
    return FALSE;
  }

  // do not disturb the Tuner state for SCell; return gracefully.
  if (asm_cfg_p->asm_cell != RFDEVICE_ASM_CELL_0)
  {
    return ret_val;
  }

  // get the rfc_band_data info based on rfm_dev/tech/band/rx_tx
  band_data = get_band_data(asm_cfg_p->rfm_device, asm_cfg_p->mode, 
                            asm_cfg_p->band, asm_cfg_p->asm_rx_tx);

  // process the rfc_band_data GRFC info based on the dev_action(script or immediate)
  ret_val &= prepare_buffer(band_data, this->band_data_size, buff_obj_ptr, dev_action, script_timing);

  return ret_val;
}
/*----------------------------------------------------------------------------*/
boolean rfdevice_asm_tuner::disable(rfdevice_asm_cfg *asm_cfg_p,
                                    rf_buffer_intf *buff_obj_ptr,
                                    rf_device_execution_type dev_action,
                                    int16 script_timing)
{
  boolean ret_val = TRUE;

  return ret_val;
}

/*----------------------------------------------------------------------------*/

/*!
  @brief
  Put the ASM in sleep or low power asm_cfg_p->mode

  @details
  This function can be used to put the ASM in sleep asm_cfg_p->mode or ultra low power asm_cfg_p->mode

  @param script_buffer
  void pointer to pass address of buffer script

  @param write_imm
  boolean to indicate write to hw or prepare script

  @param script_timing
  time offset for the script where a negative value refers to the time BEFORE the event
*/
boolean rfdevice_asm_tuner::sleep(rfdevice_asm_cfg *asm_cfg_p,
                                   rf_buffer_intf *script_buffer,
                                   rf_device_execution_type execution_type,
                                   int16 script_timing)
{
  boolean status = TRUE;

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Wakeup the ASM

  @details
  This function can be used to pull ASM out of sleep asm_cfg_p->mode

  @param script_buffer
  void pointer to pass address of buffer script

  @param write_imm
  boolean to indicate write to hw or prepare script

  @param script_timing
  time offset for the script where a negative value refers to the time BEFORE the event
*/
boolean rfdevice_asm_tuner::wakeup(rfdevice_asm_cfg *asm_cfg_p,
                                    rf_buffer_intf *script_buffer,
                                    rf_device_execution_type execution_type,
                                    int16 script_timing)
{
  /* ASM wakeup is successful */
  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Store Tx band port information

  @details
  Band port information which is pushed from RFC is stored in the device using
  this API

  @param asm_cfg_p->mode
  enum indicating the radio access technology

  @param band
  enum indicating the band number

  @param data
  void pointer to pass or receive desired information
*/
boolean rfdevice_asm_tuner::set_tx_band_map(rfm_device_enum_type rfm_device, 
                                            rfcom_mode_enum_type mode,
                                            rfcom_band_type_u band,
                                            int32 *data_ptr,
                                            uint8 size)
{
  boolean status = FALSE;

  status = set_band_data(rfm_device, mode, band, RFDEVICE_ASM_TX_0, data_ptr, size);

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Store the Rx band port information

  @details
  Band port information which is pushed from RFC is stored in the device using
  this API

  @param asm_cfg_p->mode
  enum indicating the radio access technology

  @param band
  enum indicating the band number

  @param data
  integer to pass or receive desired information
*/
boolean rfdevice_asm_tuner::set_rx_band_map(rfm_device_enum_type rfm_device, 
                                            rfcom_mode_enum_type mode,
                                            rfcom_band_type_u band,
                                            int32 *data_ptr,
                                            uint8 size)
{
  boolean status = FALSE;

  status = set_band_data(rfm_device, mode, band, RFDEVICE_ASM_RX_0, data_ptr, size);

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Obtain ASM script size

  @return script_size
  uint16 value of size of the rffe script
*/
uint16 rfdevice_asm_tuner::get_script_size()
{
  uint16 script_size = 0;

  // Get the script size from autogen table

  return script_size;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  set the ASM Tuner band_data information for the requested asm_cfg_p->mode and asm_cfg_p->band.

  @param asm_cfg_p->mode: technology info.
  @param band : frequency asm_cfg_p->band.
  @param rxtx : Rx or Tx.
  @param data_ptr : band_data info. ptr for the tech and asm_cfg_p->band.

  @return status - TRUE if no errors else FALSE.
*/
boolean rfdevice_asm_tuner::set_band_data(rfm_device_enum_type rfm_device, 
                                          rfcom_mode_enum_type mode, 
                                          rfcom_band_type_u band,
                                          rfdevice_asm_rx_tx_type rxtx,
                                          int32 *data_ptr, uint8 size)
{
  boolean status = FALSE;
  uint8 i = 0;

  if ((data_ptr == NULL) || (size == 0) || (rxtx >= RFDEVICE_ASM_RX_TX_MAX) || (rfm_device >= RFM_MAX_WAN_DEVICES))
  {
    print_device_info();
    MSG_5(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid band_data. rfm_device = %d asm_cfg_p->mode = %d band = %d rxtx = %d size=%d",
          rfm_device, mode, band.lte_band, rxtx, size);
    return status;
  }

  this->band_data_size = size;

  switch (mode)
  {
  case RFCOM_WCDMA_MODE:
    if (band.wcdma_band < RFCOM_NUM_WCDMA_BANDS)
    {
      this->wcdma_band_data[rfm_device][band.wcdma_band][rxtx] = data_ptr;
      status = TRUE;
    }
    break;

  case RFCOM_1X_MODE:
  case RFCOM_1XEVDO_MODE:
    if (band.cdma_band < RFM_CDMA_MAX_BAND)
    {
      this->cdma_band_data[rfm_device][band.cdma_band][rxtx] = data_ptr;
      status = TRUE;
    }
    break;

  case RFCOM_GSM_MODE:
    if (band.gsm_band < RFCOM_NUM_GSM_BANDS)
    {
      this->gsm_band_data[rfm_device][band.gsm_band][rxtx] = data_ptr;
      status = TRUE;
    }
    break;

  case RFCOM_LTE_MODE:
    if (band.lte_band < RFCOM_NUM_LTE_BANDS)
    {
      this->lte_band_data[rfm_device][band.lte_band][rxtx] = data_ptr;
      status = TRUE;
    }
    break;

  case RFCOM_TDSCDMA_MODE:
    if (band.tdscdma_band < RFCOM_NUM_TDSCDMA_BANDS)
    {
      this->tdscdma_band_data[rfm_device][band.tdscdma_band][rxtx] = data_ptr;
      status = TRUE;
    }
    break;

  default:
    break;
  }

  if (status == FALSE)
  {
    print_device_info();
    MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"Set_port: Un-supported Tech = %d, band = %d, rxtx = %d",
          mode, band.lte_band, rxtx);
  }

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  get the ASM Tuner band_data information for the requested mode and band.

  @param mode: technology info.
  @param band : frequency band.
  @param rxtx : Rx or Tx.

  @return band_data - rfc band_data info. ptr for the requested tech and band.
*/
int32 * rfdevice_asm_tuner::get_band_data(rfm_device_enum_type rfm_device, rfcom_mode_enum_type mode, 
                                         rfcom_band_type_u band, rfdevice_asm_rx_tx_type rxtx)
{
  int32 *band_data = NULL;

  if ((rxtx >= RFDEVICE_ASM_RX_TX_MAX) || (rfm_device >= RFM_MAX_WAN_DEVICES))
  {
    return band_data;
  }

  switch (mode)
  {
  case RFCOM_WCDMA_MODE:
    if (band.wcdma_band < RFCOM_NUM_WCDMA_BANDS)
    {
      band_data = wcdma_band_data[rfm_device][band.wcdma_band][rxtx];
    }
    break;

  case RFCOM_1X_MODE:
  case RFCOM_1XEVDO_MODE:
    if (band.cdma_band < RFM_CDMA_MAX_BAND)
    {
      band_data = cdma_band_data[rfm_device][band.cdma_band][rxtx];
    }
    break;

  case RFCOM_GSM_MODE:
    if (band.gsm_band < RFCOM_NUM_GSM_BANDS)
    {
      band_data = gsm_band_data[rfm_device][band.gsm_band][rxtx];
    }
    break;

  case RFCOM_LTE_MODE:
    if (band.lte_band < RFCOM_NUM_LTE_BANDS)
    {
      band_data = lte_band_data[rfm_device][band.lte_band][rxtx];
    }
    break;

  case RFCOM_TDSCDMA_MODE:
    if (band.tdscdma_band < RFCOM_NUM_TDSCDMA_BANDS)
    {
      band_data = tdscdma_band_data[rfm_device][band.tdscdma_band][rxtx];
    }
    break;

  default:
    break;
  }

  if (band_data == NULL)
  {
    print_device_info();
    MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"Un-supported Tech = %d, band = %d, rxtx = %d",
          mode, band.lte_band, rxtx);
  }

  return band_data;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Print-out the device information like manufacturer_id, product_id, usid_id, etc.

  @param None.
  @return None.
*/
void rfdevice_asm_tuner::print_device_info(void)
{
}

boolean rfdevice_asm_tuner::trigger(rfdevice_asm_cfg *asm_cfg_p, 
                                    rf_buffer_intf *script_buffer, 
                                    rf_device_execution_type execution_type,
                                    int16 script_timing)
{
  boolean status = TRUE;

  return status;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  prepare the rf-script buffer using the band_data GRFC information.

  @param band_data: band_data information ptr which contains the GRFC info.
  @param script_buffer : rf_buffer script where the GRFC info.
  @param execution_type : Immediate or Script request from clients.

  @return band_data - rfc band_data info. ptr for the requested tech and band.
*/
boolean rfdevice_asm_tuner::prepare_buffer(int32 *band_data, uint8 band_data_size,
                                           rf_buffer_intf *script_buffer,
                                           rf_device_execution_type execution_type,
                                           int16 timing)
{
  boolean status = TRUE;

  if (band_data == NULL)
  {
    print_device_info();
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid band_data_info., band_data = 0x%x", 0);
    return FALSE;
  }

  for (uint8 i = 1; i <= band_data_size; i++)
  {
    uint8 grfc_logic = RFDEVICE_ASM_TUNER_SCN_LOGIC_GET(band_data[i], this->scenario);
    uint16 rfc_signal = RFDEVICE_ASM_TUNER_GRFC_NUMBER_GET(band_data[i]);
    boolean valid_flag = RFDEVICE_ASM_TUNER_VALID_FLAG_GET(band_data[i]);
    int32 grfc_num = rfc_common_get_grfc_num(rfc_signal);

    if (grfc_logic == RFC_CONFIG_ONLY || grfc_num == -1 || valid_flag == FALSE) 
    {
      continue; // nothing need to be done for the GRFC.
    }

    if (execution_type == RFDEVICE_EXECUTE_IMMEDIATE)
    {
      status &= rfc_common_set_grfc((uint8)grfc_num, (rfc_logic_type)grfc_logic);
    }
    else
    {
      status &= script_buffer->append_grfc((uint16)grfc_num, (uint8)grfc_logic, timing, TRUE);
    }
  }

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function updates the scenario state of the ASM Tuner. Scenario state will be
  used to update the GRFC state of the device.

  @param scenario_num: scenario number to be used.

  @return boolean - FALSE if any failure conditions else TRUE.
*/
boolean rfdevice_asm_tuner::update_scenario(uint8 scenario_num)
{
  if (RFDEVICE_ASM_TUNER_MAX_SCENARIO_NUM <= scenario_num)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Un-supported scenario. scenario = %d", scenario_num);
    return FALSE;
  }

  this->scenario = scenario_num;
  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This is called by 200ms tick originated by L1. This updates the Hw with correct
  GRFC state based on the requested tech/band/rxtx and the current user scenario state.

  @param asm_cfg_p: tech/band/rxtx information.
  @param script_buffer : rf_buffer script where the GRFC info. need to populated.
  @param execution_type : Immediate or Script request from clients.
  @param script_timing : GRFC timing info.

  @return boolean - FALSE if any failure conditions else TRUE.
*/
boolean rfdevice_asm_tuner::update_state(rfdevice_asm_cfg *asm_cfg_p, rf_buffer_intf *buff_obj_ptr,
                                         rf_device_execution_type dev_action,
                                         int16 script_timing)
{
  boolean ret_val = TRUE;
  int32 *band_data = NULL;

  //Validity check for requested ASM settings
  if(!verify_params(asm_cfg_p,buff_obj_ptr, dev_action, script_timing))
  {
    return FALSE;
  }

  // do not disturb the Tuner state for SCell; return gracefully.
  if (asm_cfg_p->asm_cell != RFDEVICE_ASM_CELL_0)
  {
    return ret_val;
  }

  // get the rfc_band_data info based on rfm_dev/tech/band/rx_tx
  band_data = get_band_data(asm_cfg_p->rfm_device, asm_cfg_p->mode, 
                            asm_cfg_p->band, asm_cfg_p->asm_rx_tx);

  // process the rfc_band_data GRFC info based on the dev_action(script or immediate)
  ret_val &= prepare_buffer(band_data, this->band_data_size, buff_obj_ptr, dev_action, script_timing);

  return ret_val;

}

