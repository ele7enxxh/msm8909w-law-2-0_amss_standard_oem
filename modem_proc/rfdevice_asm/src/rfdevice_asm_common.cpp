/*!
  @file
  rfdevice_asm_common.cpp

  @brief
  Common interface for 3rd party ASM devices

*/

/*==============================================================================

  Copyright (c) 2014- 2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_asm/src/rfdevice_asm_common.cpp#1 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
12/11/15  ndb/hm   Fix the P1 KW warnings
03/24/15   vv      Add device creation support when rfc skip check is enabled
01/22/15   px      Move non-critical F3 messages to device logs
09/16/14   sn      ASM set band RFM Device support
08/15/14   qzh     Fix the length mismatch for LTE_active_bands in asm common part
08/05/14   cc      ASM sleep support
08/04/14   sn      EFS ASM support
07/22/14   sn      Added SV voting to handle dual chain scenarios
06/25/14   vv      physical device creation support
06/26/14   qzh     Fix log id mismatch in asm component for debugging purpose.
06/18/14   aca     ASM therm support
06/11/14   sn      updated state machine to handle disable
06/02/14   sn      state machine to handle disable tx
05/28/14   aca     Per port trigger support
05/28/14   aca     TP130 MBHB alternate part support
05/12/14   vv      Physical device creation support
05/08/14   sn      Driver back calculation update
05/05/14   sn      ASM class based API
04/22/14   aca     TP130 Dev3 support
01/13/14   vv      Skipping device Construction if asm_data_ptr is NULL
01/11/14   vv      Updated init() API to read init ag settings
12/30/13   vv      Added a voting mechanism to track Active LTE bands for rx
                   Added time profiling for enable tx/rx & disbale tx/rx
                   Added a private API for Parameter validity checking
11/15/13   vv      Updating device information to the object member variables
09/17/13   vs      Added API to modify autogen register settings
08/19/13   vs      Changed the port init for TDSCDMA
07/26/13   cc      Add a new derived class to handle dual ASM path for SHDR case
06/21/13   vs      updated the driver interface for asm port status based on band
06/13/13   vbh     Added explicit trigger for the ASM device
05/31/13   vs      updated port state changes
05/15/13   vs      Updated changes for RFFE script timing
05/13/13   jfc     Store product_rev in private variable during construction
04/10/13   sn      Cleanup ASM interface
04/02/13   vs      updated support for 5 alternative asm support in macro for
                   ManID,PrdID and Port number
03/26/13   sr      added debug msg log support.
03/07/13   sr      Fixed reg mask issue which broke LTE functionality.
03/05/13   sr      Added self_test support.
03/04/13   yzw    Program Diversity ASM properly for LTE CA mode
02/19/13   sr      Split the autogen with on/off/trigger scripts.
02/05/13   sr      Fixed the don't care register handling for LTE.
02/04/13   sr      Fixed the logic error to reset the device before programming USID
02/04/13   sr      added error handling code in the constructor
02/01/13   sr      Masked reg write fix.
12/19/12   aca     CW fix
12/13/12   aca     Infinite loop fix
12/11/12   aca     LTE CA shadow register support
12/04/12   aca    ASM timing, CA support
08/27/12   jyu     Fixed bug to set TDSCDMA TX
08/24/12   jyu     Added support for TDSCDMA
08/19/12   sr      device_type member variable is set correctly.
08/17/12   tks     Modified the trigger register address in config api
08/14/12   spa     Renamed slave_id to bus
08/14/12   spa     Use device config info to populate fields in constructor
08/12/12   tks     Added support for returning script or direct write to hw
08/11/12   tks     Added support for storing band port information sent by rfc
07/22/12   tks     Added support for creating a device
07/18/12   tks     Added RFFE ID's as private member variables
07/18/12   tks     Removed support for command dispatch
07/18/12   tks     Added support to store band port mapping info specific to
                   ASM device
07/09/12   tks     Initial version

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

#include "rfdevice_asm_common.h"
#include "rf_buffer_intf.h"
#include "rf_hal_bus_types.h"
#include "rfcommon_msg.h"
#include "rfcommon_time_profile.h"
#include "rf_rffe_common.h"
#include "rfdevice_msg_log.h"
#include "rfdevice_asm_data.h"
#ifdef FEATURE_RF_HAS_TP_CARDS
#error code not present
#endif

#include "rfdevice_efs_asm_data.h"
#ifdef TEST_FRAMEWORK
#error code not present
#endif
extern "C" {
#include "fs_lib.h"
#include "fs_public.h"
#include "modem_mem.h"
}

#define RFDEVICE_ASM_INVALID_PORT 0xFF

#define RFDEVICE_ASM_MFG_ID_GET(x) (((x) >> 22) & 0x3FF)
#define RFDEVICE_ASM_PRD_ID_GET(x) (((x) >> 14) & 0xFF)
#define RFDEVICE_ASM_PORT_NUM_GET(x) ((x) & 0xFF)

rfdevice_asm_data * rfdevice_asm_data_create(uint16 mfg_id, uint8 prd_id, uint8 rev_id);

/*!
  @brief
  create 3rd party asm devices

  @details
  create 3rd party asm devices and register itself to the parent physical device object passed to it.

  @param rfdevice_physical_ptr : parent physical device object of the asm device
  @param logical_device_cfg : logical device info of the asm device
*/

rfdevice_logical_component* create_gen_asm_object(rfdevice_physical_device *rfdevice_physical_ptr,rfc_logical_device_info_type *logical_device_cfg)
{
  boolean init_status = FALSE;
  rfdevice_logical_component *temp_obj;
  rfc_phy_device_info_type *phy_device_cfg=NULL;

  /*NULL pointer check*/
  if ( (rfdevice_physical_ptr == NULL)           ||
       (logical_device_cfg == NULL)              ||
       ( ((rfdevice_physical_third_party*)rfdevice_physical_ptr)->third_party_phy_dev_cfg == NULL) )
  {
    RF_MSG( RF_ERROR, "create_gen_asm_object failed ! NULL Pointer detected");
    return NULL;
  }

  phy_device_cfg=((rfdevice_physical_third_party*)rfdevice_physical_ptr)->third_party_phy_dev_cfg;

#ifdef FEATURE_RF_HAS_TP_CARDS
  #error code not present
#endif
  {
    temp_obj = new rfdevice_asm_common(rfdevice_physical_ptr,phy_device_cfg,logical_device_cfg); /* create the ASM device */
  }
  if (temp_obj !=NULL)
  {
    init_status = temp_obj->self_test(phy_device_cfg->manufacturer_id,
                                      phy_device_cfg->product_id,
                                      phy_device_cfg->product_rev); /* check to make sure No error */
    if (init_status == FALSE)
    {
      delete temp_obj; /* destroy the ASM device object, as it failed to initialize the device correctly. */
      temp_obj = NULL;

    }
    else
    {
      return ( ((rfdevice_physical_third_party*)rfdevice_physical_ptr)->store_object(temp_obj) );
    }
  }
  else
  {
    RF_MSG( RF_ERROR, "rfdevice_physical_third_party GEN_ASM creation failed ! returning NULL");

  }

  return NULL;

}

/*==============================================================================

                      ASM object maintenance functions

==============================================================================*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
   Overloaded Constructor for physical device support

*/
rfdevice_asm_common::rfdevice_asm_common(rfdevice_physical_device *phy_dev_obj_ptr,
    rfc_phy_device_info_type *phy_device_info,
    rfc_logical_device_info_type *logical_device_info)
{
  uint8 i = 0;
  uint8 rxtx = 0;
  uint8 swtcnt = 0;
  uint8 rfmds = 0;

  /*Reset the parameters*/
  this->LTE_active_bands = 0x0;
  this->sv_vote = 0x0;

  if((phy_device_info != NULL) && (logical_device_info != NULL) && (logical_device_info->rf_device_type == RFDEVICE_ASM))
  {

    this->init_status = TRUE;

    /*Update the parent physical device object ptr to the logical component*/
    rfdevice_asm_common::parent_physical_device_ptr=phy_dev_obj_ptr;

    /*! Assign all device information to the object member variables */
    this->rf_device_id       =  logical_device_info->rf_device_id;
    this->rf_device_type    =   logical_device_info->rf_device_type;
    this->rffe_bus          =   (uint8)  phy_device_info->bus[0];
    this->default_usid      =   (uint8)  phy_device_info->default_usid_range_start;
    this->assigned_usid     =   (uint8)  phy_device_info->assigned_usid;
    this->gsid              =   (uint8)  phy_device_info->group_id;
    this->mfg_id            =   (uint16) phy_device_info->manufacturer_id;
    this->prd_id            =   (uint16) phy_device_info->product_id;
    this->rev_id            =   (uint16) phy_device_info->product_rev;

    /* Ignoring the MSB of prd_rev, used as a bit indicator */
    this->rev_id           &= (~RFC_SKIP_RFFE_DETECT_BIT_IND);

    this->trigger_write_flag = FALSE;
    this->non_trigger_script_timing = 0;


    this->asm_data_ptr = check_efs_asm(this->mfg_id, (uint8)this->prd_id, (uint8)this->rev_id);
    if (this->asm_data_ptr == NULL)
    {
      this->asm_data_ptr = rfdevice_asm_data_create(this->mfg_id, (uint8)this->prd_id, (uint8)this->rev_id);
    }

    if (this->asm_data_ptr != NULL)
    {


      /* reset the lte shadow regs */
      memset(&lte_shadow_regs[0], 0, sizeof(uint8) * RFDEVICE_ASM_MAX_REGS);
      // reset the internal RFFE script buffer
      reset_rffe_script();

      /*All bands inactive by dfault*/
      this->LTE_active_bands=0x0;

      this->sv_vote	  = 0x0;

      /*reset vote bank*/
      memset(&rx_tx_vote[0][0],0,sizeof(uint64) * RFM_NUM_MODES * RFDEVICE_ASM_RXTX_MAX);

      /*! Initialize default ASM port types for all techs */

      for (rxtx=0; rxtx < RFDEVICE_ASM_RXTX_MAX; rxtx++)
      {
        for(i = 0; i < RFM_CDMA_MAX_BAND; i++)
        {
		  for(rfmds = 0; rfmds < RFM_MAX_DEVICES; rfmds ++)
		  {
          this->cdma_band_port[rfmds][i][rxtx]  = RFDEVICE_ASM_INVALID_PORT;
		  }
        }

        /* Set all WCDMA port information to invalid */
        for(i = 0; i < RFCOM_NUM_WCDMA_BANDS; i++)
        {
		  for(rfmds = 0; rfmds < RFM_MAX_DEVICES; rfmds ++)
		  {
          this->wcdma_band_port[rfmds][i][rxtx]  = RFDEVICE_ASM_INVALID_PORT;
		  }
        }

        /* Set all GSM port information to invalid */
        for(i = 0; i < RFCOM_NUM_GSM_BANDS; i++)
        {
	      for(rfmds = 0; rfmds < RFM_MAX_DEVICES; rfmds ++)
		  {
          this->gsm_band_port[rfmds][i][rxtx]  = RFDEVICE_ASM_INVALID_PORT;
		  }
        }
      }

      for(swtcnt=0; swtcnt< TOTAL_ASM_SWITCH_STATE; swtcnt++)
      {
        for (rxtx=0; rxtx < RFDEVICE_ASM_RXTX_MAX; rxtx++)
        {
          /* Set all LTE port information to invalid */
          for(i = 0; i < RFCOM_NUM_LTE_BANDS; i++)
          {
		  		  for(rfmds = 0; rfmds < RFM_MAX_DEVICES; rfmds ++)
		  {
            this->lte_band_port[rfmds][i][rxtx][swtcnt]  = RFDEVICE_ASM_INVALID_PORT;
			}
			
          }

          /* set all Tdscdma port information to invalid*/
          for(i = 0; i < RFCOM_NUM_TDSCDMA_BANDS; i++)
          {
		  		  for(rfmds = 0; rfmds < RFM_MAX_DEVICES; rfmds ++)
		  {
            this->tdscdma_band_port[rfmds][i][rxtx][swtcnt]  = RFDEVICE_ASM_INVALID_PORT;
			}
          }
        }
      }

      for(rxtx=0; rxtx < RFDEVICE_ASM_RXTX_MAX; rxtx++)
      {
        /* Set all LTE port information to invalid */
        for(i = 0; i < RFCOM_NUM_LTE_BANDS; i++)
        {
          this->switch_state_lte[i][rxtx] = DEFAULT_ASM_SWITCH_STATE;
        }

        /* set all Tdscdma port information to invalid*/
        for(i = 0; i < RFCOM_NUM_TDSCDMA_BANDS; i++)
        {
          this->switch_state_tdscdma[i][rxtx] = DEFAULT_ASM_SWITCH_STATE;
        }
      }

      /*One Time RFFE IMMEDIATE write of the INIT ag settings for certain ASMs during bootup*/
      init();
    }
    else
    {
      this->init_status = FALSE;
      print_device_info();
      MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"ASM Settings Data not available!");
    }
  }
  else
   {
     RF_MSG(RF_ERROR, "rfdevice_qtuner: construction failed. Device not valid ");
     this->asm_data_ptr                = NULL; 
     this->init_status                 = FALSE;
     this->rffe_bus                    = 0;  
     this->default_usid                = 0;  
     this->assigned_usid               = 0;  
     this->gsid                        = 0;  
     this->trigger_write_flag          = FALSE;  
     this->non_trigger_script_timing   = 0;  
     this->parent_physical_device_ptr  = NULL;  	    
     print_device_info(); 
   }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Destructor

*/
rfdevice_asm_common::~rfdevice_asm_common()
{
  delete this->asm_data_ptr;
}

/*==============================================================================

                      ASM device API implementation

==============================================================================*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dummy init member

  @details
  ASM class derives from rfdevice_class which has a member init(). This
  member when not defined in asm class will cause it to behave as an
  implicity abstract class resulting in compile error.

*/
void rfdevice_asm_common::init()
{
  boolean status   = FALSE;
  rfdevice_asm_cfg_params_type asm_cfg;
  rfdevice_asm_reg_settings_type asm_settings = {0};
  uint8 num_trans = 0;
  uint8 mask = 0xFF;
  asm_cfg.req = RFDEVICE_INIT_DATA;
  asm_cfg.port = 0;
  rfdevice_asm_cfg asm_cfg_s;
  asm_cfg_s.trigger_type = RFDEVICE_ASM_TRIGGER_ALL;

  if (this->asm_data_ptr == NULL)
  {
    print_device_info();
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"ASM Settings Data not available!");
    return;
  }


  status = asm_data_ptr->settings_data_get(&asm_cfg, &asm_settings);

  if (status)
  {
    /* populate(prepare) the script_buffer with register settings */
    status &= prepare_buffer(&asm_settings, mask, NULL, NULL, RFDEVICE_EXECUTE_IMMEDIATE, RFCOM_INVALID_TIMING_OFFSET, &num_trans);

    if (num_trans >0 && num_trans < RFDEVICE_ASM_MAX_REG_SIZE)
    {

      status &= rffe_prepare_buffer(NULL, this->rffe_script,
                                    RFDEVICE_EXECUTE_IMMEDIATE, this->rffe_bus,
                                    this->assigned_usid, num_trans);
    }
    status &= trigger(&asm_cfg_s, NULL, RFDEVICE_EXECUTE_IMMEDIATE, RFCOM_INVALID_TIMING_OFFSET);

    if(!status)
    {
      MSG_5(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFFE INIT ag Settings write not sucessfull bus=%d assigned_usid=0x%x, mfg_id=0x%x, prd_id=0x%x, rev_id=0x%x",
            this->rffe_bus, this->assigned_usid, this->mfg_id, this->prd_id, this->rev_id);
    }

  }
  else
  {
    RFDEVICE_LOG_MSG_5(LOG_RFA_DEVICE_ASM_C, MSG_LEGACY_HIGH,"INIT Settings not available for this ASM rfdevice Info: bus=%d assigned_usid=0x%x, mfg_id=0x%x, prd_id=0x%x, rev_id=0x%x",
                       this->rffe_bus, this->assigned_usid, this->mfg_id, this->prd_id, this->rev_id);
  }


}

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
boolean rfdevice_asm_common::rfdevice_asm_param_validity
(
  rfcom_mode_enum_type mode,
  rfcom_band_type_u band,
  rf_buffer_intf *script_buffer,
  rf_device_execution_type execution_type,
  int16 script_timing
)
{
  boolean status = TRUE;
  /* Dev action check */
  if( execution_type != RFDEVICE_CREATE_SCRIPT && execution_type != RFDEVICE_EXECUTE_IMMEDIATE)
  {
    RF_MSG( RF_ERROR, "Invalid Script for ASM request");
    return FALSE;
  }

  /* Null pointer check */
  if( execution_type == RFDEVICE_CREATE_SCRIPT && script_buffer == NULL)
  {
    RF_MSG( RF_ERROR, "Script not allocated for ASM request");
    return FALSE;
  }

  switch (mode)
  {
#ifdef FEATURE_WCDMA
  case RFCOM_WCDMA_MODE:
    /* check bands and bws supported */
    if(band.wcdma_band > RFCOM_NUM_WCDMA_BANDS)
    {
      RF_MSG_1( RF_ERROR, "Invalid WCDMA band %d", band.wcdma_band);
      return FALSE;
    }
    break;
#endif /*ifdef FEATURE_WCDMA*/
#ifdef FEATURE_CDMA1X
  case RFCOM_1X_MODE:
  case RFCOM_1XEVDO_MODE:
    /* check bands and bws supported */
    if(band.cdma_band> RFM_CDMA_MAX_BAND)
    {
      RF_MSG_1( RF_ERROR, "Invalid CDMA band %d", band.cdma_band);
      return FALSE;
    }
    break;
#endif /*ifdef FEATURE_CDMA1X*/
#ifdef FEATURE_GSM
  case RFCOM_GSM_MODE:
    /* check bands and bws supported */
    if(band.gsm_band> RFCOM_NUM_GSM_BANDS)
    {
      RF_MSG_1( RF_ERROR, "Invalid GSM band %d", band.gsm_band);
      return FALSE;
    }
    break;
#endif  /*ifdef FEATURE_GSM*/
#ifdef FEATURE_LTE
  case RFCOM_LTE_MODE:
    /* check bands and bws supported */
    if(band.lte_band> RFCOM_NUM_LTE_BANDS)
    {
      RF_MSG_1( RF_ERROR, "Invalid LTE band %d", band.lte_band);
      return FALSE;
    }
    break;
#endif /*ifdef FEATURE_LTE*/
#ifdef FEATURE_TDSCDMA
  case RFCOM_TDSCDMA_MODE:
    /* check bands and bws supported */
    if(band.tdscdma_band > RFCOM_NUM_TDSCDMA_BANDS)
    {
      RF_MSG_1( RF_ERROR, "Invalid TDSCDMA band %d", band.tdscdma_band);
      return FALSE;
    }
    break;
#endif /*ifdef FEATURE_TDSCDMA*/
  default:
    RF_MSG_1(RF_ERROR, "ASM request for invalid tech %d", mode);
    status = FALSE;
    break;
  }
  return status;
}

/*!
  @brief
  ASM ON/OFF control

  @details
  This function can be used to turn the ASM on or off

  @param mode
  enum indicating the radio access technology

  @param band
  enum indicating band number

  @param on_off
  indicates whether to turn the pa on or off

  @param script_buffer
  void pointer to pass address of buffer script

  @param write_imm
  boolean to indicate write to hw or prepare script

  @param script_timing
  time offset for the script where a negative value refers to the time BEFORE the event
*/
boolean rfdevice_asm_common::config(rfdevice_asm_cfg *asm_cfg_p,
                                    boolean asm_enable,
                                    rf_buffer_intf *script_buffer,
                                    rf_device_execution_type execution_type,
                                    int16 timing)
{
  boolean status   = FALSE;
  rfdevice_asm_cfg_params_type asm_cfg;
  rfdevice_asm_reg_settings_type asm_settings = {0};
  uint8 num_trans = 0;
  uint8 mask = 0xFF;
  uint8 *shadow_reg = NULL;
  uint16 num_idx = 0;

  if (this->asm_data_ptr == NULL)
  {
    print_device_info();
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"ASM Settings Data not available!", 0);
    return status;
  }

  /* get the ASM regsiter settings for the requested configuration */
  if (asm_enable)
  {
    asm_cfg.req = RFDEVICE_ASM_ON_DATA;
  }
  else
  {
    asm_cfg.req = RFDEVICE_ASM_OFF_DATA;
  }

  asm_cfg.port = get_port(asm_cfg_p->rfm_device, asm_cfg_p->mode, asm_cfg_p->band, (rfdevice_asm_rxtx_enum_type)((uint8)asm_cfg_p->asm_rx_tx));


  status = asm_data_ptr->settings_data_get(&asm_cfg, &asm_settings);

  if (status)
  {
    if (asm_cfg_p->mode == RFCOM_LTE_MODE) // hack to support masked write for LTE CA case
    {
      // reset the Shadow registers
      if (asm_cfg_p->asm_cell == RFDEVICE_ASM_CELL_0)
      {
        memset(&lte_shadow_regs[0], 0, sizeof(uint8) * RFDEVICE_ASM_MAX_REGS);
      }
      else
      {
        mask = get_reg_mask(asm_cfg.port);
        if (mask != 0xFF)
        {
          shadow_reg = &(this->lte_shadow_regs[0]);
        }
      }
    }

    /* populate(prepare) the script_buffer with register settings */
    status = prepare_buffer(&asm_settings, mask, shadow_reg, script_buffer, execution_type, timing, &num_trans);

    if (asm_cfg_p->trigger_type != RFDEVICE_ASM_TRIGGER_NONE)
    {
      if(asm_cfg_p->trigger_type == RFDEVICE_ASM_TRIGGER_ENABLE_RX || asm_cfg_p->trigger_type == RFDEVICE_ASM_TRIGGER_DISABLE_RX)
      {
        asm_cfg.port = get_port(asm_cfg_p->rfm_device,asm_cfg_p->mode, asm_cfg_p->band, RFDEVICE_ASM_RX);
      }
      else if (asm_cfg_p->trigger_type == RFDEVICE_ASM_TRIGGER_ENABLE_TX || asm_cfg_p->trigger_type == RFDEVICE_ASM_TRIGGER_DISABLE_TX)
      {
        asm_cfg.port = get_port(asm_cfg_p->rfm_device,asm_cfg_p->mode, asm_cfg_p->band, RFDEVICE_ASM_TX);
      }
      else
      {
        MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Trigger type: %d not supported. Defaulting to port 0", asm_cfg_p->trigger_type);
        asm_cfg.port = 0;
      }
      RFDEVICE_LOG_MSG_2(LOG_RFA_DEVICE_ASM_C, MSG_LEGACY_HIGH,"trigger requested: %d, port=%d", asm_cfg_p->trigger_type, asm_cfg.port);
      asm_cfg.req= RFDEVICE_ASM_TRIGGER_DATA;

      status &= asm_data_ptr->settings_data_get(&asm_cfg, &asm_settings);
      status &= prepare_buffer(&asm_settings, mask, NULL, script_buffer, execution_type,  timing, &num_trans);

    }


    //timing change in loop to update each element of script
    if (timing!=0)
    {
      for (num_idx=0; num_idx < num_trans; num_idx++)
      {
        this->rffe_script[num_idx].start_delta = timing - rfdevice_timing_back_calc[num_trans -1];
      }
    }

    /* Set trigger script timing same as the ASM API script */
    if (num_trans < RFDEVICE_ASM_MAX_REG_SIZE)
    {

      status &= rffe_prepare_buffer(script_buffer, this->rffe_script,
                                    execution_type, this->rffe_bus,
                                    this->assigned_usid, num_trans);
    }

  }
  else
  {
    print_device_info();
    MSG_4(MSG_SSID_RF, MSG_LEGACY_ERROR,"Failed to get ASM Settings data! (mode=%d, band=%d, on_off=%d, timing=%d",
          asm_cfg_p->mode, asm_cfg_p->band.lte_band, asm_enable, timing);
  }

  if (status == FALSE)
  {
    print_device_info();
    MSG_4(MSG_SSID_RF, MSG_LEGACY_ERROR,"Failed to configure ASM! (mode=%d, band=%d, on_off=%d, timing=%d)",
          asm_cfg_p->mode, asm_cfg_p->band.lte_band, asm_enable, timing);
  }

  return status;
}
/*----------------------------------------------------------------------------*/
boolean rfdevice_asm_common::enable
(
  rfdevice_asm_cfg *asm_cfg_p,
  rf_buffer_intf *buff_obj_ptr,
  rf_device_execution_type dev_action,
  int16 script_timing
)
{
  boolean ret_val;

  if (asm_cfg_p->asm_rx_tx == RFDEVICE_ASM_RX_0)
  {
    ret_val = this->enable_rx ( asm_cfg_p, buff_obj_ptr, dev_action, script_timing);

  }
  else
  {
    ret_val = this->enable_tx ( asm_cfg_p, buff_obj_ptr, dev_action, script_timing);
  }
  return ret_val;
}
/*----------------------------------------------------------------------------*/
boolean rfdevice_asm_common::disable
(
  rfdevice_asm_cfg *asm_cfg_p,
  rf_buffer_intf *buff_obj_ptr,
  rf_device_execution_type dev_action,
  int16 script_timing
)
{
  boolean ret_val;
  if (asm_cfg_p->asm_rx_tx == RFDEVICE_ASM_RX_0)
  {
    ret_val = this->disable_rx ( asm_cfg_p, buff_obj_ptr, dev_action, script_timing);
  }
  else
  {
    ret_val = this->disable_tx ( asm_cfg_p, buff_obj_ptr, dev_action, script_timing);
  }
  return ret_val;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure ASM Rx for specific asm_cfg_p->mode of operation

  @details
  This function can be used to configure the asm for a specific radio access
  technology and corresponding band of operation

  @param asm_cfg_p->mode
  enum indicating the radio access technology

  @param band
  enum indicating band number

  @param script_buffer
  void pointer to pass address of buffer script

  @param execution_type
  boolean to indicate write to hw or prepare script

  @param script_timing
  time offset for the script where a negative value refers to the time BEFORE the event
*/
boolean rfdevice_asm_common::enable_rx(rfdevice_asm_cfg *asm_cfg_p,
                                       rf_buffer_intf *script_buffer,
                                       rf_device_execution_type execution_type,
                                       int16 script_timing)
{
  rf_time_tick_type start_tick = rf_time_get_tick();
  boolean status   = FALSE;



  //Validity check requested ASM settings
  if(!rfdevice_asm_param_validity(asm_cfg_p->mode, asm_cfg_p->band, script_buffer, execution_type, script_timing))
  {
    RF_MSG_3(RF_ERROR, "ASM enable_rx invalid param: usid=0x%x asm_cfg_p->mode %d, band %d",
             this->assigned_usid,asm_cfg_p->mode, asm_cfg_p->band.lte_band);
    return status;
  }


  status = config(asm_cfg_p, TRUE, script_buffer,
                  execution_type, script_timing);


  if(!status)
  {
    RF_MSG_7(RF_ERROR, "config_rx failed (usid=0x%x, mode=%d, band=%d  exec_type=%d, timing=%d,Active LTE bands=0x%lx%08lx)",
             this->assigned_usid, asm_cfg_p->mode, asm_cfg_p->band.lte_band, execution_type, script_timing,(this->LTE_active_bands >> 32), (this->LTE_active_bands
& 0xffffffff));
  }
  else
  {

    this->rx_tx_vote[asm_cfg_p->mode][RFDEVICE_ASM_RX] = this->rx_tx_vote[asm_cfg_p->mode][RFDEVICE_ASM_RX] |( ((uint64)0x01) << asm_cfg_p->band.lte_band );

    this->sv_vote = this->sv_vote | (((uint32)0x01) << asm_cfg_p->mode);

    //Voting mechanism to track Active LTE bands.
    //During CA(carrier aggregation),helps to determine if multiple cells are active on the ASM.
    if (asm_cfg_p->mode==RFCOM_LTE_MODE || asm_cfg_p->mode==RFM_LTE_FTM_MODE)
    {
      //Updating LTE Active Bands
      this->LTE_active_bands=this->LTE_active_bands |( ((uint64)0x01) << asm_cfg_p->band.lte_band );
    }

    /* TIME PROFILING  */
    RFDEVICE_LOG_MSG_8(LOG_RFA_DEVICE_ASM_C, MSG_LEGACY_HIGH,"ASM Enable Rx for usid=0x%x, mode=%d, band=%d exec_type=%d, timing=%d Active LTE bands=0x%lx%08lx, completed in %d us",
                       this->assigned_usid, asm_cfg_p->mode, asm_cfg_p->band.lte_band,  execution_type, script_timing,(this->LTE_active_bands >> 32), (this->LTE_active_bands
& 0xffffffff),rf_time_get_elapsed(start_tick,RF_USEC));


  }

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure ASM Tx for specific asm_cfg_p->mode of operation

  @details
  This function can be used to configure the asm for a specific radio access
  technology and corresponding band of operation

  @param asm_cfg_p->mode
  enum indicating the radio access technology

  @param band
  enum indicating band number

  @param script_buffer
  void pointer to pass address of buffer script

  @param execution_type
  boolean to indicate write to hw or prepare script

  @param script_timing
  time offset for the script where a negative value refers to the time BEFORE the event
*/
boolean rfdevice_asm_common::enable_tx(rfdevice_asm_cfg *asm_cfg_p,
                                       rf_buffer_intf *script_buffer,
                                       rf_device_execution_type execution_type,
                                       int16 script_timing)
{
  rf_time_tick_type start_tick = rf_time_get_tick();
  boolean status   = FALSE;



  //Validity check requested ASM settings
  if(!rfdevice_asm_param_validity(asm_cfg_p->mode, asm_cfg_p->band, script_buffer, execution_type, script_timing))
  {
    RF_MSG_3(RF_ERROR, "ASM enable_rx invalid param: usid=0x%x asm_cfg_p->mode %d, band %d",
             this->assigned_usid,asm_cfg_p->mode, asm_cfg_p->band.lte_band);
    return status;
  }

  status = config(asm_cfg_p, TRUE, script_buffer,
                  execution_type, script_timing);

  if(!status)
  {
    RF_MSG_5(RF_ERROR, "config_Tx failed (usid=0x%x, mode=%d, band=%d exec_type=%d, timing=%d)",
             this->assigned_usid, asm_cfg_p->mode, asm_cfg_p->band.lte_band,  execution_type, script_timing);
  }
  else
  {

    this->rx_tx_vote[asm_cfg_p->mode][RFDEVICE_ASM_TX] = this->rx_tx_vote[asm_cfg_p->mode][RFDEVICE_ASM_TX] |( ((uint64)0x01) << asm_cfg_p->band.lte_band );

    /* TIME PROFILING  */
    RFDEVICE_LOG_MSG_6(LOG_RFA_DEVICE_ASM_C, MSG_LEGACY_HIGH,"ASM Enable Tx for usid=0x%x, mode=%d, band=%d, exec_type=%d, timing=%d completed in %d us",
                       this->assigned_usid, asm_cfg_p->mode, asm_cfg_p->band.lte_band, execution_type, script_timing,rf_time_get_elapsed(start_tick,RF_USEC));
  }

  return status;
}





/*----------------------------------------------------------------------------*/
/*!
  @brief
  Disables the ASM RX switch

  @details
  Disables the ASM RX switch

  @param asm_cfg_p->mode
  enum indicating the radio access technology

  @param band
  enum indicating band number

  @param script_buffer
  void pointer to pass address of buffer script

  @param execution_type
  boolean to indicate write to hw or prepare script

  @param script_timing
  time offset for the script where a negative value refers to the time BEFORE the event
*/
boolean rfdevice_asm_common::disable_rx(rfdevice_asm_cfg *asm_cfg_p,
                                        rf_buffer_intf *script_buffer,
                                        rf_device_execution_type execution_type,
                                        int16 script_timing)
{

  rf_time_tick_type start_tick = rf_time_get_tick();
  boolean status   = FALSE;

  //Validity check for requested ASM settings
  if(!rfdevice_asm_param_validity(asm_cfg_p->mode, asm_cfg_p->band, script_buffer, execution_type, script_timing))
  {
    RF_MSG_3(RF_ERROR, "ASM disble_rx invalid param: usid=0x%x asm_cfg_p->mode %d, band %d",
             this->assigned_usid,asm_cfg_p->mode, asm_cfg_p->band.lte_band);
    return status;
  }


  //Updating the requested LTE band to inactive in the band mask
  this->LTE_active_bands=this->LTE_active_bands &(~( ((uint64)0x01) << asm_cfg_p->band.lte_band ));

  //Voting mechanism to track Active LTE bands.
  //During CA(carrier aggregation),checks if multiple cells are active and disables the ASM only if all LTE bands are inactive.
  if (asm_cfg_p->mode==RFCOM_LTE_MODE || asm_cfg_p->mode==RFM_LTE_FTM_MODE)
  {
    if(this->LTE_active_bands)
    {
      RF_MSG_5(RF_HIGH, "ASM Disable_rx cannot be executed as there are active LTE bands in the ASM  usid=0x%x asm_cfg_p->mode %d, band %d Active LTE bands=0x%lx%08lx",
               this->assigned_usid,asm_cfg_p->mode, asm_cfg_p->band.lte_band,(this->LTE_active_bands >> 32), (this->LTE_active_bands
& 0xffffffff));
      return TRUE;
    }
  }

  this->sv_vote = this->sv_vote & (~((uint32)0x01 << asm_cfg_p->mode));

  if(this->sv_vote)
  {
    RFDEVICE_LOG_MSG_2(LOG_RFA_DEVICE_ASM_C, MSG_LEGACY_HIGH, "ASM Disable_rx cannot be executed as there are active techs using the ASM usid=0x%x asm_cfg_p->mode %d",
                       this->assigned_usid, asm_cfg_p->mode);
    return TRUE;
  }

  if (!(this->rx_tx_vote[asm_cfg_p->mode][RFDEVICE_ASM_RX] & ( ((uint64)0x01) << asm_cfg_p->band.lte_band )))
  {
    RF_MSG_3(RF_HIGH, "ASM Disable_rx sequence anomaly, since enable_rx was not called for the ASM  usid=0x%x asm_cfg_p->mode %d, band %d",
             this->assigned_usid,asm_cfg_p->mode, asm_cfg_p->band.lte_band);

  }
  /*reset the bit for specified band/mode/RX */
  this->rx_tx_vote[asm_cfg_p->mode][RFDEVICE_ASM_RX] = this->rx_tx_vote[asm_cfg_p->mode][RFDEVICE_ASM_RX] & (~( ((uint64)0x01) << asm_cfg_p->band.lte_band ));

  status = config(asm_cfg_p, FALSE, script_buffer,
                  execution_type, script_timing);

  if(!status)
  {
    RF_MSG_7(RF_ERROR, "config_rx failed (usid=0x%x, mode=%d, band=%d exec_type=%d, timing=%d, Active LTE bands=0x%lx%08lx)",
             this->assigned_usid, asm_cfg_p->mode, asm_cfg_p->band.lte_band, execution_type, script_timing,(this->LTE_active_bands >> 32), (this->LTE_active_bands
& 0xffffffff));
  }
  else
  {

    /* TIME PROFILING  */
    RFDEVICE_LOG_MSG_8(LOG_RFA_DEVICE_ASM_C, MSG_LEGACY_HIGH,"ASM Disable Rx for usid=0x%x, mode=%d, band=%d exec_type=%d, timing=%d, Active LTE bands=0x%lx%08lx, completed in %d us",
                       this->assigned_usid, asm_cfg_p->mode, asm_cfg_p->band.lte_band,  execution_type, script_timing,(this->LTE_active_bands >> 32), (this->LTE_active_bands
& 0xffffffff),rf_time_get_elapsed(start_tick,RF_USEC));


  }


  return status;
}

/*----------------------------------------------------------------------------*/
/*!
 @brief
  Disables the ASM TX switch

  @details
  Disables the ASM TX switch

  @param asm_cfg_p->mode
  enum indicating the radio access technology

  @param band
  enum indicating band number

  @param script_buffer
  void pointer to pass address of buffer script

  @param execution_type
  boolean to indicate write to hw or prepare script

  @param script_timing
  time offset for the script where a negative value refers to the time BEFORE the event
*/
boolean rfdevice_asm_common::disable_tx(rfdevice_asm_cfg *asm_cfg_p,
                                        rf_buffer_intf *script_buffer,
                                        rf_device_execution_type execution_type,
                                        int16 script_timing)
{

  rf_time_tick_type start_tick = rf_time_get_tick();
  boolean status   = FALSE;


  //Validity check for requested ASM settings
  if(!rfdevice_asm_param_validity(asm_cfg_p->mode, asm_cfg_p->band, script_buffer, execution_type, script_timing))
  {
    RF_MSG_3(RF_ERROR, "ASM disble_tx invalid param: usid=0x%x asm_cfg_p->mode %d, band %d",
             this->assigned_usid,asm_cfg_p->mode, asm_cfg_p->band.lte_band);
    return status;
  }


  if( !(this->rx_tx_vote[asm_cfg_p->mode][RFDEVICE_ASM_TX] & ( ((uint64)0x01) << asm_cfg_p->band.lte_band )))
  {
    RF_MSG_3(RF_HIGH, "ASM Disable_tx sequence anomaly since enable_tx was not called for the ASM  usid=0x%x asm_cfg_p->mode %d, band %d",
             this->assigned_usid,asm_cfg_p->mode, asm_cfg_p->band.lte_band);

  }
  /*reset the bit for specified band/mode/TX */
  this->rx_tx_vote[asm_cfg_p->mode][RFDEVICE_ASM_TX] = this->rx_tx_vote[asm_cfg_p->mode][RFDEVICE_ASM_TX] & (~( ((uint64)0x01) << asm_cfg_p->band.lte_band ));


  if (this->rx_tx_vote[asm_cfg_p->mode][RFDEVICE_ASM_RX] & ( ((uint64)0x01) << asm_cfg_p->band.lte_band ))
  {
    RFDEVICE_LOG_MSG_3(LOG_RFA_DEVICE_ASM_C, MSG_LEGACY_HIGH, "ASM Disable_tx cannot be executed since ASM is used for RX usid=0x%x asm_cfg_p->mode %d, band %d",
                       this->assigned_usid, asm_cfg_p->mode, asm_cfg_p->band.lte_band);
    return TRUE;

  }

  status = config(asm_cfg_p, FALSE, script_buffer,
                  execution_type, script_timing);

  if(!status)
  {
    RF_MSG_5(RF_ERROR, "config_tx failed (usid=0x%x, mode=%d, band=%d  exec_type=%d, timing=%d)",
             this->assigned_usid, asm_cfg_p->mode, asm_cfg_p->band.lte_band,  execution_type, script_timing);
  }
  else
  {
    /* TIME PROFILING  */
    RFDEVICE_LOG_MSG_6(LOG_RFA_DEVICE_ASM_C, MSG_LEGACY_HIGH,"ASM Disable tx for usid=0x%x, mode=%d, band=%d exec_type=%d, timing=%d completed in %d us",
                       this->assigned_usid, asm_cfg_p->mode, asm_cfg_p->band.lte_band,  execution_type, script_timing,rf_time_get_elapsed(start_tick,RF_USEC));
  }



  return status;
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
boolean rfdevice_asm_common::sleep(rfdevice_asm_cfg *asm_cfg_p,
                                   rf_buffer_intf *script_buffer,
                                   rf_device_execution_type execution_type,
                                   int16 script_timing)
{
  boolean status	 = FALSE;
  rfdevice_asm_cfg_params_type asm_cfg;
  rfdevice_asm_reg_settings_type asm_settings = {0};
  uint8 num_trans = 0;
  uint8 mask = 0xFF;
  uint16 num_idx = 0;

//Validity check for requested ASM settings
  if(!rfdevice_asm_param_validity(asm_cfg_p->mode, asm_cfg_p->band, script_buffer, execution_type, script_timing))
  {
    RF_MSG_3(RF_ERROR, "Sleep ASM invalid Params usid=0x%x asm_cfg_p->mode %d, band %d",
             this->assigned_usid,asm_cfg_p->mode, asm_cfg_p->band.lte_band);
    return status;
  }

  if (this->asm_data_ptr == NULL)
  {
	print_device_info();
	MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"ASM Settings Data not available!", 0);
	return status;
  }

  asm_cfg.req = RFDEVICE_ASM_SLEEP_DATA;
  asm_cfg.port = 0;
  status = asm_data_ptr->settings_data_get(&asm_cfg, &asm_settings);
  
  if(this->sv_vote)
  {
    RFDEVICE_LOG_MSG_2(LOG_RFA_DEVICE_ASM_C, MSG_LEGACY_HIGH, "ASM sleep cannot be executed as there are active techs using the ASM  usid=0x%x asm_cfg_p->mode %d",
             this->assigned_usid,asm_cfg_p->mode);
    return TRUE;
  }  
  
  if(status)
  {
    status &= prepare_buffer(&asm_settings, mask, NULL, script_buffer, execution_type,  script_timing, &num_trans);
  
    //timing change in loop to update each element of script
    if (script_timing!=0)
    {
  	for (num_idx=0; num_idx < num_trans; num_idx++)
  	{
  	  this->rffe_script[num_idx].start_delta = script_timing - rfdevice_timing_back_calc[num_trans -1];
  	}
    }
    
    /* Set trigger script timing same as the ASM API script */
    if (num_trans < RFDEVICE_ASM_MAX_REG_SIZE)
    {
    
  	status &= rffe_prepare_buffer(script_buffer, this->rffe_script,
  								  execution_type, this->rffe_bus,
  								  this->assigned_usid, num_trans);
        RFDEVICE_LOG_MSG_2(LOG_RFA_DEVICE_ASM_C, MSG_LEGACY_HIGH, "Sleep ASM addr=0x%d data=0x%d", 
                           asm_settings.addr[0], asm_settings.data[0]);
    }
  }
  else
  {
    print_device_info();
    RFDEVICE_LOG_MSG_3(LOG_RFA_DEVICE_ASM_C, MSG_LEGACY_ERROR, "No Sleep script support! (mode=%d, band=%d, timing=%d",
                       asm_cfg_p->mode, asm_cfg_p->band.lte_band, script_timing);
  return TRUE;
}
  if (status == FALSE)
  {
    print_device_info();
    MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"Failed to sleep ASM! (mode=%d, band=%d, timing=%d)",
          asm_cfg_p->mode, asm_cfg_p->band.lte_band, script_timing);
  }
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
boolean rfdevice_asm_common::wakeup(rfdevice_asm_cfg *asm_cfg_p,
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
boolean rfdevice_asm_common::set_tx_band_map
(
  rfm_device_enum_type rfm_device, 
  rfcom_mode_enum_type mode,
  rfcom_band_type_u band,
  int32 *data_ptr,
  uint8 size
)
{
  boolean status = FALSE;
  status = set_port(rfm_device, mode, band, RFDEVICE_ASM_TX, data_ptr, size);
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
boolean rfdevice_asm_common::set_rx_band_map
(
  rfm_device_enum_type rfm_device, 
  rfcom_mode_enum_type mode,
  rfcom_band_type_u band,
  int32 *data_ptr,
  uint8 size
)
{
  boolean status = FALSE;
  status = set_port(rfm_device, mode, band, RFDEVICE_ASM_RX, data_ptr, size);
  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Obtain ASM script size

  @return script_size
  uint16 value of size of the rffe script
*/
uint16 rfdevice_asm_common::get_script_size()
{
  uint16 script_size = 0;

  // Get the script size from autogen table

  return script_size;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief performs the self test procedure for the device and returns TRUE or FALSE.

  @details
  returns TRUE if self test is successful, FALSE otherwise.
*/
boolean rfdevice_asm_common::self_test(uint32 mfg_id, uint32 prd_id, uint32 rev_id)
{
  boolean status = FALSE;

  /* Ignoring the MSB of prd_rev, used as a bit indicator */
  rev_id           &= (~RFC_SKIP_RFFE_DETECT_BIT_IND);

  if (mfg_id == this->mfg_id  &&
      prd_id == this->prd_id &&
      rev_id == this->rev_id)
  {
    status = TRUE;
  }

  status &= this->init_status;

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  prepare the PA settings script buffer or directly write the settings to the
  ASM device based on the 'executio_type' request.

  @param settings: ASM device settings info. like regAddr and regValues.
  @param script_buffer : buffer to populate the ASM device register settings.
  @param execution_type : IMMIDEATE -> send settings to ASM device
                          CREATE_SCRIPT -> populate the settings to 'script_buffer'.
  @param timing : timing value to populate into the script_buffer. Not applicable if the
                  execution_type = IMMEDIATE.

  @return TRUE - if successful else FALSE.
*/

boolean rfdevice_asm_common::prepare_buffer(rfdevice_asm_reg_settings_type *asm_settings,
    uint8 mask, uint8 *shadow_reg,
    rf_buffer_intf *script_buffer,
    rf_device_execution_type execution_type,
    int16 timing,
    uint8 *num_trans)
{
  boolean status = FALSE;
  uint8 script_idx = 0;
  uint8 addr = 0x0;
  uint8 *data_ptr = NULL;
  uint16 lcl_data;

  if (asm_settings == NULL )
  {
    print_device_info();
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"asm_settings = 0x%X", asm_settings);
    return status;
  }

  if ((asm_settings->num_regs > RFDEVICE_ASM_MAX_REG_SIZE)||((*num_trans) > RFDEVICE_ASM_MAX_REG_SIZE))
  {
    print_device_info();
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"ASM number reg settings(%d) > RFDEVICE_ASM_MAX_REG_SIZE(%d)",
          asm_settings->num_regs, RFDEVICE_ASM_MAX_REG_SIZE);
    return status;
  }

  script_idx = (*num_trans);    //**vs added for incrementing buffer from previous read

  for (uint8 i=0; i < asm_settings->num_regs; i++)
  {

    data_ptr = (uint8*) &asm_settings->data[i];

    lcl_data = data_ptr[1]<<8 | data_ptr[0];


    if (RFDEVICE_ASM_REG_INVALID != (int16)lcl_data)
    {

      if (script_idx >= RFDEVICE_ASM_MAX_REG_SIZE)
      {
        MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"Script Index %d out of bounds; Script Size: %d",
              script_idx, RFDEVICE_ASM_MAX_REG_SIZE);
        break;
      }

      rffe_script[script_idx].addr = asm_settings->addr[i];
      rffe_script[script_idx].start_delta = timing;
      rffe_script[script_idx].data = (uint8)(lcl_data);

      script_idx++;

    }
  }

  *num_trans = script_idx; // update with number transactions added to buffer.

  if (shadow_reg != NULL) /* this is a hack to make the mask work for one ASM device */
  {
    addr = rffe_script[0].addr;
    rffe_script[0].data = (shadow_reg[addr] & ~mask) |
                          ((uint8)(rffe_script[0].data) & mask);
    shadow_reg[addr] = rffe_script[0].data; // updated the shadow reg
  }


  /* Return ASM ON/OFF control status */
  status = TRUE;
  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  set the ASM port information for the requested asm_cfg_p->mode and asm_cfg_p->band.

  @param asm_cfg_p->mode: technology info.
  @param band : frequency asm_cfg_p->band.
  @param rxtx : Rx or Tx.
  @param port : Port number for the tech and asm_cfg_p->band.

  @return status - TRUE if no errors else FALSE.
*/
boolean rfdevice_asm_common::set_port(rfm_device_enum_type rfm_device, 
                                      rfcom_mode_enum_type mode, 
									  rfcom_band_type_u band,
                                      rfdevice_asm_rxtx_enum_type rxtx,
                                      int32 *data_ptr, uint8 size)
{
  boolean status = FALSE;
  uint8 i = 0;
  uint8 port = RFDEVICE_ASM_INVALID_PORT; /* port for this device */
  uint8 switch_st = DEFAULT_ASM_SWITCH_STATE;

  if (data_ptr == NULL || rxtx >= RFDEVICE_ASM_RXTX_MAX)
  {
    print_device_info();
    MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid band_port_map_info. asm_cfg_p->mode = %d band = %d rxtx = %d",
          mode, band.lte_band, rxtx);
    return status;
  }

  for (i = 1; i < size; i++)
  {
    if (this->mfg_id == RFDEVICE_ASM_MFG_ID_GET(data_ptr[i]) &&
        this->prd_id == RFDEVICE_ASM_PRD_ID_GET(data_ptr[i]))
    {
      port = (uint8)RFDEVICE_ASM_PORT_NUM_GET(data_ptr[i]);
      break; // found the port info for this device.
    }
  }

  switch (mode)
  {
  case RFCOM_WCDMA_MODE:
    if (band.wcdma_band < RFCOM_NUM_WCDMA_BANDS)
    {
      this->wcdma_band_port[rfm_device][band.wcdma_band][rxtx] = port;
      status = TRUE;
    }
    break;

  case RFCOM_1X_MODE:
  case RFCOM_1XEVDO_MODE:
    if (band.cdma_band < RFM_CDMA_MAX_BAND)
    {
      this->cdma_band_port[rfm_device][band.cdma_band][rxtx] = port;
      status = TRUE;
    }
    break;

  case RFCOM_GSM_MODE:
    if (band.gsm_band < RFCOM_NUM_GSM_BANDS)
    {
      this->gsm_band_port[rfm_device][band.gsm_band][rxtx] = port;
      status = TRUE;
    }
    break;

  case RFCOM_LTE_MODE:
    if (band.lte_band < RFCOM_NUM_LTE_BANDS)
    {
      switch_st = this->switch_state_lte[band.lte_band][rxtx];   /*For LTE and TDSCDMA only*/
      this->lte_band_port[rfm_device][band.lte_band][rxtx][switch_st] = port;
      status = TRUE;
    }
    break;

  case RFCOM_TDSCDMA_MODE:
    if (band.tdscdma_band < RFCOM_NUM_TDSCDMA_BANDS)
    {
      switch_st = this->switch_state_tdscdma[band.tdscdma_band][rxtx];
      this->tdscdma_band_port[rfm_device][band.tdscdma_band][rxtx][switch_st] = port;
      status = TRUE;
    }
    break;

  default:
    break;
  }

  if (status == FALSE || port == RFDEVICE_ASM_INVALID_PORT)
  {
    print_device_info();
    MSG_4(MSG_SSID_RF, MSG_LEGACY_ERROR,"Set_port: Un-supported Tech = %d, band = %d, rxtx = %d port=%d",
          mode, band.lte_band, rxtx, port);
  }

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  get the ASM port information for the requested mode and band.

  @param mode: technology info.
  @param band : frequency band.
  @param rxtx : Rx or Tx.

  @return port - asm port value for the requested tech and band.
*/
uint8 rfdevice_asm_common::get_port(rfm_device_enum_type rfm_device, rfcom_mode_enum_type mode, 
                                    rfcom_band_type_u band, rfdevice_asm_rxtx_enum_type rxtx)
{
  uint8 port = RFDEVICE_ASM_INVALID_PORT;
  uint8 switch_st = DEFAULT_ASM_SWITCH_STATE;

  if (rxtx >= RFDEVICE_ASM_RXTX_MAX)
  {
    return port;
  }

  switch (mode)
  {
  case RFCOM_WCDMA_MODE:
    if (band.wcdma_band < RFCOM_NUM_WCDMA_BANDS)
    {
      port = this->wcdma_band_port[rfm_device][band.wcdma_band][rxtx];
    }
    break;

  case RFCOM_1X_MODE:
  case RFCOM_1XEVDO_MODE:
    if (band.cdma_band < RFM_CDMA_MAX_BAND)
    {
      port = this->cdma_band_port[rfm_device][band.cdma_band][rxtx];
    }
    break;

  case RFCOM_GSM_MODE:
    if (band.gsm_band < RFCOM_NUM_GSM_BANDS)
    {
      port = this->gsm_band_port[rfm_device][band.gsm_band][rxtx];
    }
    break;

  case RFCOM_LTE_MODE:
    if (band.lte_band < RFCOM_NUM_LTE_BANDS)
    {
      switch_st = this->switch_state_lte[band.lte_band][rxtx];
      port = this->lte_band_port[rfm_device][band.lte_band][rxtx][switch_st];
    }
    break;

  case RFCOM_TDSCDMA_MODE:
    if (band.tdscdma_band < RFCOM_NUM_TDSCDMA_BANDS)
    {
      switch_st = this->switch_state_tdscdma[band.tdscdma_band][rxtx];
      port = this->tdscdma_band_port[rfm_device][band.tdscdma_band][rxtx][switch_st];
    }
    break;

  default:
    break;
  }

  if (port == RFDEVICE_ASM_INVALID_PORT)
  {
    print_device_info();
    MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"Un-supported Tech = %d, band = %d, rxtx = %d",
          mode, band.lte_band, rxtx);
  }

  return port;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  reset the device.

  @param None.
  @return None.
*/

void rfdevice_asm_common::reset(uint8 usid)
{
  // nothing todo for now
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Print-out the device information like manufacturer_id, product_id, usid_id, etc.

  @param None.
  @return None.
*/
void rfdevice_asm_common::print_device_info(void)
{
  RFDEVICE_LOG_MSG_5(LOG_RFA_DEVICE_ASM_C, MSG_LEGACY_HIGH, "ASM rfdevice Info: bus=%d assigned_usid=0x%x, mfg_id=0x%x, prd_id=0x%x, rev_id=0x%x",
                     this->rffe_bus, this->assigned_usid, this->mfg_id, this->prd_id, this->rev_id);
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  initialize the shadow register's data and mask.

  @return None.
*/
uint8 rfdevice_asm_common::get_reg_mask(uint8 port)
{
  uint8 mask = 0xFF;

  /* this is a hack for only one SWITCH to just to support
  existing CA rf-card.  */
  if (this->prd_id == 0xE && this->mfg_id == 0x1B0)
  {
    if (port == 0 || port >= 8)
    {
      mask = 0x38;
    }
    else
    {
      mask = 0x07;
    }
  }

  /* this is a hack for only one SWITCH to just to support
  existing CA rf-card.  */
  if (this->prd_id == 0xF && this->mfg_id == 0x248)
  {
    if (port == 3 || port == 4 || port == 5 || port == 6 || port == 8)
    {
      mask = 0xF0;
    }
    else
    {
      mask = 0x0F;
    }
  }

  return mask;
}


/*----------------------------------------------------------------------------*/  //**vs
/*!
  @brief
  reset the internal rffe_script buffer with default values.

  @param None.
  @return None.
*/
void rfdevice_asm_common::reset_rffe_script(void)
{
  /* initialize the internal RFFE script buffer */
  for (uint8 i = 0; i < RFDEVICE_ASM_MAX_REG_SIZE; i++)
  {
    rffe_script[i].rd_wr = RF_BUFFER_WRITE;
    rffe_script[i].rd_delay = FALSE;
    rffe_script[i].half_rate = FALSE;
    rffe_script[i].num_bytes = 1;
    rffe_script[i].addr = 0x0;
    rffe_script[i].data = 0x0;
    rffe_script[i].start_delta = 0;
  }

}

/*----------------------------------------------------------------------------*/
/*Start of temperary fix for backward compatibility*/
/*!
  @brief
  Update the switch state of the port
  Returns True if port state is valid
  Else returns False
  @param port state
  @return status
*/
boolean rfdevice_asm_common::set_switch_state(uint8 switchstate)   //port indexed  as 0 and 1
{
  uint8 i = 0;
  uint8 rxtx = 0;

  if (switchstate >= TOTAL_ASM_SWITCH_STATE)
  {
    return(FALSE);
  }

  else
  {
    for(rxtx=0; rxtx < RFDEVICE_ASM_RXTX_MAX; rxtx++)
    {
      /* Set all LTE port information to invalid */
      for(i = 0; i < RFCOM_NUM_LTE_BANDS; i++)
      {
        this->switch_state_lte[i][rxtx] = switchstate;
      }

      /* set all Tdscdma port information to invalid*/

      for(i = 0; i < RFCOM_NUM_TDSCDMA_BANDS; i++)
      {
        this->switch_state_tdscdma[i][rxtx] = switchstate;
      }
    }

    return(TRUE);
  }

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return the state of the port
  Returns True if port state is valid
  Else returns False
  @param get_port_state pointer to return state value
  @return status
*/
boolean rfdevice_asm_common::get_switch_state(uint8 *switch_state_get)
{
  if ((switch_state_get == NULL)||(this->switch_state_lte[0][0] >= TOTAL_ASM_SWITCH_STATE))
  {
    return(FALSE);
  }

  else
  {
    *switch_state_get = this->switch_state_lte[0][0];
    return(TRUE);
  }
}
/*End of temperary fix for backward compatibility*/
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return the state of the port
  Returns True if port state is valid
  Else returns False
  @param get_port_state pointer to return state value
  @return status
*/
boolean rfdevice_asm_common::set_rx_switch_state(rfcom_mode_enum_type mode,
    rfcom_band_type_u band,
    uint8 switch_state_set)
{
  boolean status = FALSE;

  if(switch_state_set >= TOTAL_ASM_SWITCH_STATE)
  {
    return(FALSE);
  }

  status = set_band_switch_state(mode,band,RFDEVICE_ASM_RX,switch_state_set);

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return the state of the port
  Returns True if port state is valid
  Else returns False
  @param get_port_state pointer to return state value
  @return status
*/
boolean rfdevice_asm_common::set_tx_switch_state(rfcom_mode_enum_type mode,
    rfcom_band_type_u band,
    uint8 switch_state_set)
{
  boolean status = FALSE;

  if(switch_state_set >= TOTAL_ASM_SWITCH_STATE)
  {
    return(FALSE);
  }

  status = set_band_switch_state(mode,band,RFDEVICE_ASM_TX,switch_state_set);

  return status;
}
boolean rfdevice_asm_common::trigger(rfdevice_asm_cfg *asm_cfg_p, rf_buffer_intf *script_buffer, rf_device_execution_type execution_type,
                                     int16 script_timing)
{
  boolean status = TRUE;
  rfdevice_asm_cfg_params_type asm_config;
  rfdevice_asm_reg_settings_type asm_settings = {0};
  uint8 num_trans = 0;
  uint8 mask = 0xFF;

  RFDEVICE_LOG_MSG_3(LOG_RFA_DEVICE_ASM_C, MSG_LEGACY_HIGH,"trigger(usid=0x%x, exec_type=%d, timing=%d)",
                     this->assigned_usid, execution_type, script_timing);

  if (this->asm_data_ptr == NULL)
  {
    print_device_info();
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"ASM Settings Data not available!", 0);
    return status;
  }

  if (asm_cfg_p->trigger_type != RFDEVICE_ASM_TRIGGER_NONE && asm_cfg_p->trigger_type != RFDEVICE_ASM_TRIGGER_SLEEP && asm_cfg_p->trigger_type != RFDEVICE_ASM_TRIGGER_WAKEUP)
  {
    if(asm_cfg_p->trigger_type == RFDEVICE_ASM_TRIGGER_ENABLE_RX || asm_cfg_p->trigger_type == RFDEVICE_ASM_TRIGGER_DISABLE_RX)
    {
      asm_config.port = get_port(asm_cfg_p->rfm_device,asm_cfg_p->mode, asm_cfg_p->band, RFDEVICE_ASM_RX);
    }
    else if (asm_cfg_p->trigger_type == RFDEVICE_ASM_TRIGGER_ENABLE_TX || asm_cfg_p->trigger_type == RFDEVICE_ASM_TRIGGER_DISABLE_TX)
    {
      asm_config.port = get_port(asm_cfg_p->rfm_device,asm_cfg_p->mode, asm_cfg_p->band, RFDEVICE_ASM_TX);
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Trigger type requested: %d is not supported. Defaulting to port 0", asm_cfg_p->trigger_type);
      asm_config.port = 0;
    }

    RFDEVICE_LOG_MSG_2(LOG_RFA_DEVICE_ASM_C, MSG_LEGACY_HIGH,"trigger requested: %d, port=%d", asm_cfg_p->trigger_type, asm_config.port);
    asm_config.req= RFDEVICE_ASM_TRIGGER_DATA;

    status &= asm_data_ptr->settings_data_get(&asm_config, &asm_settings);

    status &= prepare_buffer(&asm_settings, mask, NULL, script_buffer, execution_type,  script_timing, &num_trans);

    if (num_trans < RFDEVICE_ASM_MAX_REG_SIZE)
    {

      status &= rffe_prepare_buffer(script_buffer, this->rffe_script,
                                    execution_type, this->rffe_bus,
                                    this->assigned_usid, num_trans);
    }
  }

  return status;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Update the switch state of the port
  Returns True if port state is valid
  Else returns False
  @param port state
  @return status
*/
boolean rfdevice_asm_common::set_band_switch_state(rfcom_mode_enum_type mode,
    rfcom_band_type_u band,
    rfdevice_asm_rxtx_enum_type rxtx,
    uint8 switch_state_set)   //port indexed  as 0 and 1
{
  boolean status = FALSE;

  /*Start of case for tech band port switch state*/
  switch (mode)
  {
  case RFCOM_WCDMA_MODE:
  case RFCOM_1X_MODE:
  case RFCOM_1XEVDO_MODE:
  case RFCOM_GSM_MODE:
    break;

  case RFCOM_LTE_MODE:
    if (band.lte_band < RFCOM_NUM_LTE_BANDS)
    {
      this->switch_state_lte[band.lte_band][rxtx] = switch_state_set;
      status = TRUE;
    }
    break;

  case RFCOM_TDSCDMA_MODE:
    if (band.tdscdma_band < RFCOM_NUM_TDSCDMA_BANDS)
    {
      this->switch_state_tdscdma[band.tdscdma_band][rxtx] = switch_state_set;
      status = TRUE;
    }
    break;

  default:
    break;
  }

  return status;
}
/*---------------------------------------------------------------------------*/
/*
@brief
set the temperature sensor
@returns True or false
 */

boolean rfdevice_asm_common::asm_therm_on_off(boolean turn_on_off)
{
  boolean status = TRUE;
  uint8 reg_read =0;
  rfdevice_asm_cfg_params_type asm_cfg;
  rfdevice_asm_cmd_seq_type asm_cmd_seq = {0};

  if (turn_on_off == TRUE)                                                 /*if request it to measure */
  {
    asm_cfg.req = RFDEVICE_ASM_THERM_ON_DATA;
  }
  else
  {
    asm_cfg.req = RFDEVICE_ASM_THERM_OFF_DATA;
  }

  print_device_info();

  status &= this->asm_data_ptr->sequence_data_get(&asm_cfg, &asm_cmd_seq);
  status &= process_cmd_sequence(this->assigned_usid, &asm_cmd_seq, &reg_read);

  return(status);
}

/*---------------------------------------------------------------------------*/
/*
@brief
Function returns the address of the Look up table for temperature reading
@returns True or false
*/
boolean rfdevice_asm_common::get_asm_temperature(int16 *temperature)
{
  boolean status = TRUE;
  uint8 reg_read = 0;
  int16 **lut_addr;
  rfdevice_asm_cfg_params_type asm_cfg;
  rfdevice_asm_cmd_seq_type asm_cmd_seq = {0};
  int16 *temp_addr = 0;
  uint8 lut_size = 0;
  lut_addr = &temp_addr;

  if (temperature == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Temperature pointer = NULL!", 0);
    return FALSE;
  }

  print_device_info();

  asm_cfg.req = RFDEVICE_ASM_THERM_READ_DATA;
  status &= this->asm_data_ptr->sequence_data_get(&asm_cfg, &asm_cmd_seq);
  status &= process_cmd_sequence(this->assigned_usid, &asm_cmd_seq, &reg_read);
  status &= asm_data_ptr->get_temperature_lut(lut_addr, &lut_size);

  if(reg_read >= lut_size || *lut_addr==NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Therm LUT read is NULL. ASM doesnt support THERM reads.", 0);
    return FALSE;
  }

  if (status == TRUE)
  {
    *lut_addr = *lut_addr + reg_read;
    *temperature = **lut_addr;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"Temp :%d degrees C", *temperature );
  }

  return status;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process the ASM register read/write sequence commands. These sequence cmds are
  processed as immediate/direct write/read to device. The construction of buffer
  script is not supported.

  @param None.
  @return None.
*/
boolean rfdevice_asm_common::process_cmd_sequence(uint8 usid, rfdevice_asm_cmd_seq_type *cmd_seq,
    uint8 *reg_read)
{

  uint8 i = 0;
  boolean status = TRUE;
  rf_hal_bus_resource_script_settings_type dev_setup = {1, FALSE, FALSE, FALSE, TRUE};

  if (cmd_seq == NULL)
  {
    return FALSE;
  }

  while ( i < cmd_seq->num_cmds)
  {
    switch(cmd_seq->cmds[i].cmd)
    {
    case RFDEVICE_ASM_CMD_WRITE:
      dev_setup.half_rate = FALSE;
      status &= rffe_write_to_hw(this->rffe_bus, usid, cmd_seq->cmds[i].addr, cmd_seq->cmds[i].data, &dev_setup);
      RFDEVICE_LOG_MSG_3(LOG_RFA_DEVICE_ASM_C, MSG_LEGACY_HIGH,"ASM Seq write slave=%d addr=0x%x, data=0x%x",
                         usid, cmd_seq->cmds[i].addr, cmd_seq->cmds[i].data);
      break;

    case RFDEVICE_ASM_CMD_WRITE_EXT:
      dev_setup.half_rate = FALSE;
      status &= rffe_write_to_hw_ext(this->rffe_bus, usid, cmd_seq->cmds[i].addr, cmd_seq->cmds[i].data, &dev_setup);
      RFDEVICE_LOG_MSG_3(LOG_RFA_DEVICE_ASM_C, MSG_LEGACY_HIGH,"ASM Seq write slave=%d addr=0x%x, data=0x%x",
                         usid, cmd_seq->cmds[i].addr, cmd_seq->cmds[i].data);
      break;

    case RFDEVICE_ASM_CMD_DELAY:
      DALSYS_BusyWait(cmd_seq->cmds[i].data);
      RFDEVICE_LOG_MSG_2(LOG_RFA_DEVICE_ASM_C, MSG_LEGACY_HIGH,"ASM slave %d, Seq delay=%d",
                         usid, cmd_seq->cmds[i].data);
      break;

    case RFDEVICE_ASM_CMD_READ:
      if (reg_read != NULL)
      {
        dev_setup.half_rate = TRUE;
        status &= rffe_read_from_hw(rffe_bus, usid, cmd_seq->cmds[i].addr, reg_read, &dev_setup);
        *reg_read &= (cmd_seq->cmds[i].mask);
        RFDEVICE_LOG_MSG_3(LOG_RFA_DEVICE_ASM_C, MSG_LEGACY_HIGH,"ASM Seq read slave=%d addr=0x%x, data=0x%x",
                           usid, cmd_seq->cmds[i].addr, *reg_read);
      }
      else
      {
        status = FALSE;
        print_device_info();
        MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Reg READ Failed reg addr = 0x%X and reg_data is NULL",
              cmd_seq->cmds[i].addr);
      }
      break;

    default:
      status = FALSE;
      print_device_info();
      MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid ASM Sequence Command! cmd = %d addr=0x%x data=0x%x",
            cmd_seq->cmds[i].cmd, cmd_seq->cmds[i].addr, cmd_seq->cmds[i].data);
      break;
    }

    i++;
  }

  return status;
}




/*----------------------------------------------------------------------------*/
/*!
  @brief
  modify autogen settings of a given device

  @details
  This function can be used to dynamically update the default autogen settings
  of particular registers. Returns True if operation is successful else returns False

  @param asm_mod_cfg
  structure with settings of register type to be modified in autogen file
  based on tech and band

  @param asm_mod_reg_ag
  register settings to be modified in autogen file
  contains registers address and value to be updated
*/

boolean rfdevice_asm_common::modify_asm_ag_setting(rfdevice_asm_mod_cfg_type *asm_mod_cfg,
    rfdevice_mod_reg_info_type *asm_mod_reg_ag)
{
  boolean status = FALSE;
  uint8 index =0;
  uint8 port_num = RFDEVICE_ASM_INVALID_PORT;

  /*Variables to acess the ag settings and ag register information*/
  rfdevice_asm_cfg_params_type asm_params_cfg;

  /*Variable to store the ag register info for a given PA register type*/
  rfdevice_asm_reg_settings_type asm_reg_ag = {0};

  /*Check if invalid data is passed to the function*/
  if (  ( asm_mod_cfg->reg_type) >= RFDEVICE_ASM_MAX_REG
        ||( asm_mod_cfg->txrx)     >= RFDEVICE_RXTX_MAX
        ||( asm_mod_reg_ag->reg_data) == NULL
        ||( asm_mod_reg_ag->reg_addr) == NULL
     )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid register settings information", 0);
    return FALSE;
  }

  /*Get the port number for band and mode requested*/
  port_num = get_port(RFM_DEVICE_0,asm_mod_cfg->mode, asm_mod_cfg->band,
                      (rfdevice_asm_rxtx_enum_type)asm_mod_cfg->txrx);

  /*Check if invalid port info*/
  if (port_num == RFDEVICE_ASM_INVALID_PORT)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid port number read", 0);
    return FALSE;
  }

  /*Match the ASM register type of rfdevice_asm_mod_cfg_type against rfdevice_asm_cfg_params_type*/
  switch (asm_mod_cfg->reg_type)
  {
  case RFDEVICE_ASM_ON:
    asm_params_cfg.req = RFDEVICE_ASM_ON_DATA;
    break;

  case RFDEVICE_ASM_OFF:
    asm_params_cfg.req = RFDEVICE_ASM_OFF_DATA;
    break;

  default:
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid register type information %d", asm_params_cfg.req);
    return FALSE;
    break;
  }

  /*update rfdevice_asm_cfg_params_type to obtain ag register settings*/
  asm_params_cfg.port  = port_num;
  status = asm_data_ptr->settings_data_get(&asm_params_cfg, &asm_reg_ag);

  /*check if ag num registers doesnt match input num registers to be modified*/
  if ((status == TRUE)&&(asm_reg_ag.num_regs == asm_mod_reg_ag->max_num_reg))
  {
    /*scan through the register addresses for validation*/
    for ( index=0; index < (asm_reg_ag.num_regs); index++)
    {
      if (asm_reg_ag.addr[index] != asm_mod_reg_ag->reg_addr[index])
      {
        MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Failed to match register address with autogen settings", 0);
        return FALSE;
      }
    }

    /*on successful verification of register addresses copy register contents*/
    memscpy( asm_reg_ag.data , sizeof(int16)*asm_reg_ag.num_regs,
             asm_mod_reg_ag->reg_data, sizeof(int16)*asm_mod_reg_ag->max_num_reg );

    status = TRUE;
  }

  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Mismatch between number of registers in autogen and register info update ", 0);
    status = FALSE;
  }

  return status;
}

rfdevice_asm_data* rfdevice_asm_common::check_efs_asm(uint32 mfg_id, uint32 prd_id, uint32 rev_id)
{

  struct fs_dirent *dirent;
  struct fs_stat    temp_buf;
  boolean match_not_found = TRUE;
  char file_path[MAX_PATH_SIZE] = "\0"; // full file path
  uint8* data = NULL;                                         // ptr to EFS data
  int32 data_size = 0;                                        // size of EFS data
  uint8 i=0;
  uint16 j=0;
  uint8 num_regs;

  static const char path[] =  "/efs_asm/";
#ifdef TEST_FRAMEWORK
  #error code not present
#endif
  EFSDIR * dirp = efs_opendir (path);


  while ((dirent = efs_readdir (dirp)) != 0)
  {
    RFDEVICE_LOG_MSG_1(LOG_RFA_DEVICE_ASM_C, MSG_LEGACY_MED, "File name %s", dirent->d_name);

    file_path[0]= '\0';
    strlcat(file_path,path,MAX_PATH_SIZE);
    /* Construct complete file_path and call EFS Stat */
    strlcat(file_path,dirent->d_name,MAX_PATH_SIZE);

    if(efs_stat(file_path,&temp_buf) == -1)
    {
      RFDEVICE_LOG_MSG_1(LOG_RFA_DEVICE_ASM_C, MSG_LEGACY_MED, "get_info_data: File not found! %s", file_path);
    }

    data_size = (int32)temp_buf.st_size;

    // Allocate memory for data
    data = (uint8*)modem_mem_alloc(data_size, MODEM_MEM_CLIENT_RFA);

    if(data == NULL)
    {
      RFDEVICE_LOG_MSG_2(LOG_RFA_DEVICE_ASM_C, MSG_LEGACY_MED, 
                         "get_info_data: Memory Allocation failed for info data! file= %s, size=%d",
                         file_path, data_size);
      return NULL;
    }

    // Obtain contents
    data_size = efs_get(file_path, data, temp_buf.st_size);


    if ((mfg_id == (data[0] | data[1]<<8)) && (prd_id == data[2]) && rev_id == data[4])
    {
      efs_closedir (dirp);
      return (rfdevice_efs_asm_data::get_instance(data));

    }

    if (data != NULL )
    {
      modem_mem_free((void*)data,MODEM_MEM_CLIENT_RFA);
    }
    i++;

  }
  efs_closedir (dirp);

  return NULL;

}
