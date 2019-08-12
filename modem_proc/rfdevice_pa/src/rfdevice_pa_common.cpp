
/*!
  @file
  rfdevice_pa_common.cpp

  @brief
  Common interface for 3rd party pa devices

*/

/*==============================================================================

  Copyright (c) 2014 - 2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_pa/src/rfdevice_pa_common.cpp#1 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
3/8/2016   wxl     PA initialize after PA object created(for 3rd PA to save current consumption) 
12/11/15  ndb/hm   Fix the P1 KW warnings
07/23/14   sn      Add EFS PA support to physical device constructor
06/26/14   vv      physical device creation support
06/25/14   sn      KW fix
06/24/14   sn      EFS dat file PA support
05/20/14   aca     Therm update to support 256 bins
05/12/14   vv      Physical device creation support
05/08/14   sn      Driver back calculation update
04/17/14   sn      PA ICQ pointer validation
03/28/14   sn      Fix for 16 bit ICQ support
03/19/14   sn      Fix for set_current_bias non-cfg API call
03/03/14   vv      Added new API on_off(with cfg)
02/26/14   vv      GSM port selection (GSM vs EDGE) fix
01/11/14   vv      Updated config API to read config ag settings
11/15/13   vv      Updating device information to the object member variables
11/05/13   vv      Added new APIs to set current bias(set_current_bias) and gain range(set_gain_range),
                   Added private APIs set_pa()(Common API to configure PA settings) and rfdevice_pa_param_validity() (Checks parameter validity)
09/17/13   vs      Added API to modify autogen register settings
09/11/13   vs      Added support for PA port A/B switch api
09/04/13   sr/zhw  Add explicit default PA Icq value flag in PA device driver
08/02/13   vs      Remove check for icq_pdm value equals zero
07/07/13   sbo     Added BW support for PA device
6/25/13    vbh     Added explicit trigger for the PA device
05/28/13   vs      API support for PA therm
05/15/13   vs      Updated changes for RFFE script timing
04/11/13   jyu     Return script in set_gain_range() for TDSCDMA mode
04/10/13   sn      Cleanup PA interface
04/09/13   vs      Updated Invalid port to 7F to avoid possible errors
04/02/13   vs      updated support for 5 alternative pa support in macro for
                   ManID,PrdID and Port number
03/26/13   sr      Added debug log msg support.
03/18/13   sb      Fix PA control NV timing hook for all PA devices
03/05/13   sr      Added self_test support.
03/05/13   vrb     Ignoring the MSB of prd_rev, used as a bit indicator
02/27/13   lcl     Removed incorrect code protection.  pa_settings->num_regs can
                   before larger than RFDEVICE_PA_MAX_REG_SIZE.
02/19/13   sr      Split the autogen with on/off/range/bias scripts.
02/06/13   sr      Added PA revision support.
02/05/13   sr      changes to return FALSE by un-supported api.
02/04/13   sr      Fix the logic while resetting the PA before usid program.
01/29/13   spa     Call PA_OFF in sleep API for CDMA
01/29/13   spa     Added mode and band as members of PA class
01/22/13   sr      changes to create the device only if PA Reg settings available.
01/21/13   vrb     Bug Fix: Populating Bias Reg setting with Don't Care support
01/19/13   vrb     Update Max script size to be 5 - 2G PA requirement
01/19/13   vrb     Filter out 'Don't Care' registers when creating script: 2G PA req
01/17/13   sr      changes to assignUSID and then create PA device
01/16/13   sr      Added pa command sequence request support.
01/14/13   sr      PA autogen design changes.
12/20/12   aca     Per device LUT mapping
12/18/12   aca     WCDMA B11 support
12/18/12   aca     Vendor LUT update
12/17/12   aca     Set bias 1x defaulted to PA on state
12/13/12   aca     B18 support and B3/B4 HCPA hack update
12/05/12   aca     Added support for LTE B40
12/03/12   aca     Added support for timing info in scripts
11/21/12   aca     Added support for LTE B20
11/14/12   aca     Added support for LTE B26
11/06/12   aca     Added support for LTE B13
11/02/12   tks     Invert the PA mapping for WCDMA to R0R1 format
11/01/12   aca     Added support for LTE B9,11,19,21,41 and W B9
10/30/12   tks     Fixed compiler warnings
10/29/12   tks     Hooked up the pa device functions to use the autogen data &
                   commented out the new code for each tech
10/28/12   tks     Modified constructor to initialize the default lut settings
                   for all bands and pa mode of operation (et/ept/apt)
10/25/12   tks     Fixed set current bias function to use the new autogen data
10/24/12   tks     Added support for preparing script from autogen data
10/18/12   tks     Fixed compiler warnings
10/17/12   tks     Added support for modifying pa current bias in ftm mode
10/12/12   tks     Removed debug F3 messages to fix GSM ISR overrrun
09/13/12   tks     Removed the hard-coded usid reprogram for HCPA
09/12/12   spa     Removed hard coding of PA data for CDMA
08/29/12   tks     Pass mode as a parameter in rf_pa_get_data
08/28/12   sty     CDMA-specific changes
08/28/12   vss     Fix LTE B1 PA issue caused due to CDMA hack
08/28/12   vss     Added changes needed for LTE bringup
08/28/12   sty     Added changes needed for CDMA
08/28/12   tks     Added support to reprogram HCPA usid
08/19/12   sr      device_type member variable is set correctly.
08/17/12   tks     Modified PA script size from 0x0F to 0x03
08/14/12   spa     Renamed slave_id to bus
08/14/12   spa     Use device config info to populate fields in constructor
07/26/12   tks     Added support for reading & writing to hw
07/22/12   tks     Added support for creating a device
07/18/12   tks     Added RFFE ID's as private member variables
07/18/12   tks     Removed support for command dispatch
07/18/12   tks     Added support to store band port mapping info specific to PA
                   device
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

#include "rfdevice_pa_common.h"
#include "rf_buffer_intf.h"
#include "rf_hal_bus_types.h"
#include "rfcommon_msg.h"
#include "rfdevice_msg_log.h"
#include "rfcommon_time_profile.h"
#include "rf_rffe_common.h"
#include "rfdevice_pa_data.h"
#include "rfdevice_efs_pa_data.h"
extern "C" {
#include "fs_lib.h"
#include "fs_public.h"
#include "modem_mem.h"
}
#define RFDEVICE_PA_INVALID_PORT 0x7F
#define INVALID_TEMPERATURE 0x7F
#define RFDEVICE_PA_DEFAULT_MASK 0xFF

#define RFDEVICE_MFG_ID_GET(x) (((x) >> 22) & 0x3FF)
#define RFDEVICE_PRD_ID_GET(x) (((x) >> 14) & 0xFF)
#define RFDEVICE_PORT_NUM_GET(x) ((x) & 0x7F)
#define RFDEVICE_SEC_PORT_NUM_GET(x)  (((x) >> 7) & 0x7F)

rfdevice_pa_data * rfdevice_pa_data_create(uint16 mfg_id, uint8 prd_id, uint8 rev_id);


/*!
  @brief
  create 3rd party pa devices

  @details
  create 3rd party pa devices and register itself to the parent physical device object passed to it.

  @param rfdevice_physical_ptr : parent physical device object of the apa device
  @param logical_device_cfg : logical device info of the pa device
*/

rfdevice_logical_component* create_gen_pa_object(rfdevice_physical_device *rfdevice_physical_ptr,rfc_logical_device_info_type *logical_device_cfg)
{
  boolean init_status = FALSE;
  rfdevice_logical_component *temp_obj;
  rfc_phy_device_info_type *phy_device_cfg=NULL;

  /*NULL pointer check*/
  if ( (rfdevice_physical_ptr == NULL) ||
       (logical_device_cfg == NULL)              ||
       ( ((rfdevice_physical_third_party*)rfdevice_physical_ptr)->third_party_phy_dev_cfg == NULL) )
  {
    RF_MSG( RF_ERROR, "create_gen_pa_object failed ! NULL Pointer detected");
    return NULL;
  }

  phy_device_cfg=((rfdevice_physical_third_party*)rfdevice_physical_ptr)->third_party_phy_dev_cfg;

  temp_obj = new rfdevice_pa_common(rfdevice_physical_ptr ,phy_device_cfg,logical_device_cfg); /* create the PA device */

  if (temp_obj != NULL )
  {
    init_status = temp_obj->self_test(phy_device_cfg->manufacturer_id,
                                      phy_device_cfg->product_id,
                                      phy_device_cfg->product_rev); /* check to make sure No error */
    if (init_status == FALSE)
    {
      delete temp_obj; /* destroy the PA device object, as it failed to initialize the device correctly. */
      temp_obj = NULL;
      return NULL;
    }
    else
    {
      return ( ((rfdevice_physical_third_party*)rfdevice_physical_ptr)->store_object(temp_obj) );
    }
  }
  else
  {
    RF_MSG( RF_ERROR, "rfdevice_physical_third_party GEN_PA creation failed ! returning NULL");

  }

  return NULL;

}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Programs the PA device USID. It creates the PA device object and programs device
  with required reset procedure and programs the USID.

*/
boolean rfdevice_pa_program_device_id_phy(rfc_phy_device_info_type *phy_device_info)
{
  rfdevice_pa_common *pa_dev = NULL;
  boolean init_status = FALSE;

  if (phy_device_info == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"GEN PA phy_device_info == NULL!", 0);
    return FALSE;
  }

  /*Temporarily fillup logical info and phy dev obj ptr for temporary device creation*/

  rfdevice_physical_device* phy_dev_obj_ptr=NULL;
  rfc_logical_device_info_type logical_device_info;
  logical_device_info.rf_device_type = RFDEVICE_PA;
  logical_device_info.rf_device_id = GEN_PA;
  logical_device_info.rf_asic_id = 0;

  pa_dev = new rfdevice_pa_common(phy_dev_obj_ptr,phy_device_info,&logical_device_info);

  if (pa_dev != NULL)
  {
    init_status = pa_dev->self_test(phy_device_info->manufacturer_id,
                                    phy_device_info->product_id,
                                    phy_device_info->product_rev); /* check to make sure No error */
    if (init_status == TRUE)
    {
      // reset the device based on default USID. This is a requirement from few RFFE devices that the
      // device need to be reset before programming the assigned USID.
      pa_dev->reset(phy_device_info->default_usid_range_start);
    }

        rffe_program_assigned_usid(phy_device_info);

    delete pa_dev;

    return TRUE;
  }

  return FALSE;
}

/*==============================================================================

                      PA object maintenance functions

==============================================================================*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Constructor overlaoded for physical device support

*/
rfdevice_pa_common::rfdevice_pa_common(rfdevice_physical_device *phy_dev_obj_ptr,rfc_phy_device_info_type *phy_device_info, rfc_logical_device_info_type *logical_device_info)
{
  uint8 i = 0;
  rfdevice_pa_info_type pa_info = {0};
  uint8 swtcnt;

  if((phy_device_info != NULL) && (logical_device_info != NULL) && (logical_device_info->rf_device_type == RFDEVICE_PA))
  {

    this->init_status = TRUE;

    /*Update the parent physical device object ptr to the logical component*/
    rfdevice_pa_common::parent_physical_device_ptr=phy_dev_obj_ptr;


    /*! Assign all device information to the object member variables */
    this->rf_device_id       =  logical_device_info->rf_device_id;
    this->rf_device_type    =   logical_device_info->rf_device_type;
    this->instance_num      =   logical_device_info->rf_asic_id;
    this->rffe_bus          =   (uint8)  phy_device_info->bus[0];
    this->default_usid      =   (uint8)  phy_device_info->default_usid_range_start;
    this->assigned_usid     =   (uint8)  phy_device_info->assigned_usid;
    this->gsid              =   (uint8)  phy_device_info->group_id;
    this->mfg_id            =   (uint16) phy_device_info->manufacturer_id;
    this->prd_id            =   (uint16) phy_device_info->product_id;
    this->rev_id            =   (uint16) phy_device_info->product_rev;

    /* Ignoring the MSB of prd_rev, used as a bit indicator */
    this->rev_id           &= (~RFC_SKIP_RFFE_DETECT_BIT_IND);

    this->low_gain_range = 0xFF;

    this->pa_data_ptr = check_efs_pa(this->mfg_id, (uint8)this->prd_id, (uint8)this->rev_id);
    if (this->pa_data_ptr == NULL)
    {
      this->pa_data_ptr = rfdevice_pa_data_create(this->mfg_id, (uint8)this->prd_id, (uint8)this->rev_id);
    }
    if (this->pa_data_ptr == NULL)
    {
      this->init_status = FALSE;
      print_device_info();
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"PA Settings Data not available!", 0);
    }
    
    if (this->pa_data_ptr != NULL)
    {
      this->pa_data_ptr->device_info_get( &pa_info );
    }
  
    // reset the internal RFFE script buffer
    reset_rffe_script();

    /*! lowest PA range supported by the PA device */
    this->low_gain_range  = (uint16)(pa_info.num_pa_ranges - 1);

    this->lpm_pm_trig_flag = pa_info.lpm_pm_trig_flag;
    /*! Initialize default PA port types for all techs */
    for(i = 0; i < RFM_CDMA_MAX_BAND; i++)
    {
      this->cdma_band_port[i]  = RFDEVICE_PA_INVALID_PORT;
    }

    /* Set all WCDMA port information to invalid */
    for(i = 0; i < RFCOM_NUM_WCDMA_BANDS; i++)
    {
      this->wcdma_band_port[i]  = RFDEVICE_PA_INVALID_PORT;
    }

    /* Set all GSM port information to invalid */
    for(i = 0; i < RFCOM_NUM_GSM_BANDS; i++)
    {
      this->gsm_band_pri_port[i]  = RFDEVICE_PA_INVALID_PORT;
      this->gsm_band_sec_port[i]  = RFDEVICE_PA_INVALID_PORT;
    }

    /* Set all LTE port information to invalid */
    for(swtcnt=0; swtcnt< TOTAL_PA_SWITCH_STATE; swtcnt++)
    {
      for(i = 0; i < RFCOM_NUM_LTE_BANDS; i++)
      {
        this->lte_band_port[i][swtcnt]  = RFDEVICE_PA_INVALID_PORT;
      }

      for(i = 0; i < RFCOM_NUM_TDSCDMA_BANDS; i++)
      {
        this->tdscdma_band_port[i][swtcnt]  = RFDEVICE_PA_INVALID_PORT;
      }
    }

    /*set default port switch state*/
    for(i = 0; i < RFCOM_NUM_LTE_BANDS; i++)
    {
      this->switch_state_lte[i] = DEFAULT_PA_SWITCH_STATE;
    }

    /* set all Tdscdma port information to invalid*/
    for(i = 0; i < RFCOM_NUM_TDSCDMA_BANDS; i++)
    {
      this->switch_state_tdscdma[i] = DEFAULT_PA_SWITCH_STATE;
    }
  }
  else
  {
    RF_MSG(RF_ERROR, "rfdevice_pa_common: construction failed. Device not valid ");
    this->pa_data_ptr                = NULL;
    this->parent_physical_device_ptr = NULL;  
    this->init_status                = FALSE;
    this->rffe_bus                   = 0;
    this->default_usid               = 0;
    this->assigned_usid              = 0;
    this->gsid                       = 0;
    this->low_gain_range             = 0;
    this->lpm_pm_trig_flag           = FALSE;	     
  }

  /* Need to do PA INIT after PA object created*/
   init();
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Constructor

*/
rfdevice_pa_common::rfdevice_pa_common(rfc_device_cfg_info_type *device_info)
{
  uint8 i = 0;
  rfdevice_pa_info_type pa_info = {0};
  uint8 swtcnt;



  /*! Assign all device information to the object member variables */
  this->rf_device_id       =  device_info->rf_device_id;
  this->rf_device_type    = device_info->rf_device_type;
  this->rffe_bus          = (uint8) device_info->bus[0];
  this->default_usid      = (uint8) device_info->default_usid_range_start;
  this->assigned_usid     = (uint8) device_info->assigned_usid;
  this->gsid              = (uint8) device_info->group_id;
  this->mfg_id            = (uint16) device_info->manufacturer_id;
  this->prd_id            = (uint16) device_info->product_id;
  this->rev_id            = (uint16) device_info->product_rev;

  /* Ignoring the MSB of prd_rev, used as a bit indicator */
  this->rev_id           &= (~RFC_SKIP_RFFE_DETECT_BIT_IND);

  this->init_status = TRUE;
  this->low_gain_range = 0xFF;

  this->pa_data_ptr = check_efs_pa(this->mfg_id, (uint8)this->prd_id, (uint8)this->rev_id);
  if (this->pa_data_ptr == NULL)
  {
    this->pa_data_ptr = rfdevice_pa_data_create(this->mfg_id, (uint8)this->prd_id, (uint8)this->rev_id);
  }
  if (this->pa_data_ptr == NULL)
  {
    this->init_status = FALSE;
    print_device_info();
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"PA Settings Data not available!", 0);
  }

  if (this->pa_data_ptr != NULL)
  {
    this->pa_data_ptr->device_info_get( &pa_info );
  }

  // reset the internal RFFE script buffer
  reset_rffe_script();

  /*! lowest PA range supported by the PA device */
  this->low_gain_range  = (uint16)(pa_info.num_pa_ranges - 1);


  /*! Initialize default PA port types for all techs */
  for(i = 0; i < RFM_CDMA_MAX_BAND; i++)
  {
    this->cdma_band_port[i]  = RFDEVICE_PA_INVALID_PORT;
  }

  /* Set all WCDMA port information to invalid */
  for(i = 0; i < RFCOM_NUM_WCDMA_BANDS; i++)
  {
    this->wcdma_band_port[i]  = RFDEVICE_PA_INVALID_PORT;
  }

  /* Set all GSM port information to invalid */
  for(i = 0; i < RFCOM_NUM_GSM_BANDS; i++)
  {
    this->gsm_band_pri_port[i]  = RFDEVICE_PA_INVALID_PORT;
    this->gsm_band_sec_port[i]  = RFDEVICE_PA_INVALID_PORT;
  }

  /* Set all LTE port information to invalid */
  for(swtcnt=0; swtcnt< TOTAL_PA_SWITCH_STATE; swtcnt++)
  {
    for(i = 0; i < RFCOM_NUM_LTE_BANDS; i++)
    {
      this->lte_band_port[i][swtcnt]  = RFDEVICE_PA_INVALID_PORT;
    }

    for(i = 0; i < RFCOM_NUM_TDSCDMA_BANDS; i++)
    {
      this->tdscdma_band_port[i][swtcnt]  = RFDEVICE_PA_INVALID_PORT;
    }
  }

  /*set default port switch state*/
  for(i = 0; i < RFCOM_NUM_LTE_BANDS; i++)
  {
    this->switch_state_lte[i] = DEFAULT_PA_SWITCH_STATE;
  }

  /* set all Tdscdma port information to invalid*/
  for(i = 0; i < RFCOM_NUM_TDSCDMA_BANDS; i++)
  {
    this->switch_state_tdscdma[i] = DEFAULT_PA_SWITCH_STATE;
  }

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Destructor

*/
rfdevice_pa_common::~rfdevice_pa_common()
{
  if(this->pa_data_ptr != NULL)
  {
    delete this->pa_data_ptr;
    this->pa_data_ptr = NULL;
  }

}

/*==============================================================================

                      PA device API implementation

==============================================================================*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure PA for specific mode of operation

  @details
  This function can be used to configure the pa for a specific radio access
  technology and corresponding band of operation

  @param mode
  enum indicating the radio access technology

  @param band
  enum indicating band number

  @param script_buffer
  void pointer to pass address of buffer script

  @param write_imm
  boolean to indicate write to hw or prepare script

  @param script_timing
  time offset for the script where a negative value refers to the time BEFORE the event
*/
boolean rfdevice_pa_common::config( rfdevice_pa_cfg* pa_params_p,
                                    rf_buffer_intf *script_buffer,
                                    rf_device_execution_type execution_type,
                                    int16 script_timing)
{
  boolean status = TRUE;
  rfdevice_pa_cfg_params_type pa_params_cfg= {RFDEVICE_PA_DATA_REQ_INVALID,0,0,0};
  rfdevice_pa_reg_settings_type pa_reg_ag = { 0 };
  uint8 num_trans = 0;
  uint8 num_idx = 0;
  uint8 port_num = RFDEVICE_PA_INVALID_PORT;
  int16 timing;


//Validity check on requested PA settings
  if(!rfdevice_pa_param_validity(pa_params_p->mode,pa_params_p->band, RFDEVICE_PA_BW_NOT_APPLICABLE,
                                 script_buffer, execution_type, script_timing))
  {
    RF_MSG_4(RF_ERROR, "PA config invalid param: usid=%d, mode %d, band %d, bw %d",
             this->assigned_usid, pa_params_p->mode, pa_params_p->band.lte_band, pa_params_p->bw);
    return FALSE;
  }

  /*Get the port number for band and mode requested*/
  port_num = get_port(pa_params_p->mode, pa_params_p->band, pa_params_p->is_mod_type_gmsk);

  /*Check if invalid port info*/
  if (port_num == RFDEVICE_PA_INVALID_PORT)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Invalid port number read", 0);
    return FALSE;
  }

  RFDEVICE_LOG_MSG_6(LOG_RFA_DEVICE_PA_C, MSG_LEGACY_HIGH, "config(usid=%d, mode=%d, band=%d,bw=%d, exec_type=%d, timing=%d)",
                     this->assigned_usid, pa_params_p->mode, pa_params_p->band.lte_band, pa_params_p->bw,execution_type, script_timing);

  //Update PA configuration settings

  pa_params_cfg.port  = port_num;

  pa_params_cfg.req = RFDEVICE_PA_CONFIG_DATA;

  //Setup the requested PA configuration

  /*Obtain the ag settings from PA ag file */
  status &= pa_data_ptr->settings_data_get(&pa_params_cfg, &pa_reg_ag);

  /*Prepare the script buffer with the obtained ag settings*/
  status &= prepare_buffer(&pa_reg_ag, script_buffer, execution_type, script_timing, &num_trans);

  if ((pa_params_p->trigger_type != RFDEVICE_PA_TRIGGER_NONE) && (num_trans != 0))
  {
    pa_params_cfg.port = 0;
    pa_params_cfg.req = RFDEVICE_PA_TRIGGER_DATA;  /* PA Trigger reg settings */
    status &= pa_data_ptr->settings_data_get(&pa_params_cfg, &pa_reg_ag);
    status &= prepare_buffer(&pa_reg_ag, script_buffer, execution_type, script_timing, &num_trans);

    if(!status)
    {
      RF_MSG(RF_ERROR, "PA config failed in trigger");
      return status;
    }

  }

  if (num_trans < RFDEVICE_PA_MAX_REG_SIZE)
  {

    //timing change in loop to update each element of script
    if (script_timing != RFCOM_INVALID_TIMING_OFFSET)
    {

      if ( num_trans > 0 && (num_trans - 1) < RFDEVICE_NUM_TRANS_TIMING_CALC - 1)
      {
        timing = script_timing - rfdevice_timing_back_calc[num_trans -1];
      }
      else
      {
        timing = script_timing - rfdevice_timing_back_calc[RFDEVICE_NUM_TRANS_TIMING_CALC -1];
      }

      for (num_idx = 0; num_idx < num_trans; num_idx++)
      {
        this->rffe_script[num_idx].start_delta =timing;
      }
    }
    status = rffe_prepare_buffer(script_buffer, this->rffe_script,
                                 execution_type, this->rffe_bus,
                                 this->assigned_usid, num_trans);
  }

  if(!status)
  {
    RF_MSG(RF_ERROR, "PA Config failed");
    return status;
  }

  return TRUE;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  set PA gain range with cfg as param

  @details
  This function can be used to configure gain values for a particular
  band  and mode

  @param cfg
  PA config parameter that contains: on_off,mode, band ,pa_gain_range, pa_icq_pdm ,mod_type

  @param script_buffer
  void pointer to pass address of buffer script

  @param execution_type
  boolean to indicate write to hw or prepare script

  @param script_timing
  time offset for the script where a negative value refers to the time BEFORE the event
*/
boolean rfdevice_pa_common::set_pa_txagc (  rfdevice_pa_cfg* pa_params_p,
    rf_buffer_intf *script_buffer,
    rf_device_execution_type execution_type,
    int16 script_timing)

{

  boolean status = TRUE;
  rfdevice_pa_cfg_params_type pa_params_cfg= {RFDEVICE_PA_DATA_REQ_INVALID,0,0,0};
  rfdevice_pa_reg_settings_type pa_reg_ag = { 0 };
  uint8 num_trans = 0;
  uint8 num_idx = 0;
  uint8 port_num = RFDEVICE_PA_INVALID_PORT;
  uint16 pa_icq = 0;
  int16 timing;

//Validity check on requested PA settings
  if(!rfdevice_pa_param_validity(pa_params_p->mode,pa_params_p->band, RFDEVICE_PA_BW_NOT_APPLICABLE,
                                 script_buffer, execution_type, script_timing))
  {
    RF_MSG_6(RF_ERROR, "PA Set TXAGC invalid param: usid=%d mode %d, band %d, bw %d, range %d, mod %d",
             this->assigned_usid,pa_params_p->mode, pa_params_p->band.lte_band, pa_params_p->bw,
             pa_params_p->pa_gain_range, pa_params_p->is_mod_type_gmsk);
    return FALSE;
  }

  /*Get the port number for band and mode requested*/
  port_num = get_port(pa_params_p->mode, pa_params_p->band, pa_params_p->is_mod_type_gmsk);

  /*Check if invalid port info*/
  if (port_num == RFDEVICE_PA_INVALID_PORT)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Invalid port number read", 0);
    return FALSE;
  }

  if (pa_params_p->use_default_icq_pdm == FALSE)
  {
    if (pa_params_p->pa_icq_array_count == 1 && pa_params_p->pa_icq_array_p !=NULL)
    {
      pa_icq = *(pa_params_p->pa_icq_array_p);
    }
    else
    {
      RF_MSG_2(RF_ERROR, "PA Set TXAGC failed in set current bias pa_icq_array_count %d, pa_icq_array_p %d",pa_params_p->pa_icq_array_count, pa_params_p->pa_icq_array_p);
    }
  }
  else
  {
    RFDEVICE_LOG_MSG(LOG_RFA_DEVICE_PA_C, MSG_LEGACY_HIGH, "PA Set TXAGC, using default ICQ for current bias");
  }

  RFDEVICE_LOG_MSG_8(LOG_RFA_DEVICE_PA_C, MSG_LEGACY_HIGH,
                     "set_pa_txagc(usid=%d, mode=%d, band=%d, pa_range=%d, icq=%d, mod_type=%d exec_type=%d, timing=%d)",
                     this->assigned_usid, pa_params_p->mode, pa_params_p->band.cdma_band,
                     pa_params_p->pa_gain_range,pa_icq, pa_params_p->is_mod_type_gmsk,
                     execution_type, script_timing);

  /*update rfdevice_pa_cfg_params_type to obtain ag register settings*/
  pa_params_cfg.pa_range = (uint8)pa_params_p->pa_gain_range;
  pa_params_cfg.port  = port_num;

  pa_params_cfg.req = RFDEVICE_PA_SET_BIAS_DATA;

  //setup a default value to enable Bias regsiter exsitence verification
  pa_reg_ag.bias_reg_idx = RF_REG_INVALID;

  /*Obtain the ag settings from PA ag file */
  status &= pa_data_ptr->settings_data_get(&pa_params_cfg, &pa_reg_ag);

  /*Prepare the script buffer with the obtained ag settings*/
  status &= prepare_buffer(&pa_reg_ag, script_buffer, execution_type, script_timing, &num_trans);

  if(!status)
  {
    RF_MSG(RF_ERROR, "PA Set TXAGC failed in set current bias");
    return status;
  }

  if (pa_params_p->use_default_icq_pdm == FALSE && pa_reg_ag.bias_reg_idx != RF_REG_INVALID)
  {
    if (pa_reg_ag.num_regs == RFDEVICE_PA_NUM_8BIT_ICQ_REGS)
    {
      this->rffe_script[num_trans-RFDEVICE_PA_NUM_8BIT_ICQ_REGS].data = 0xFF & (pa_icq);

    }
    else if (pa_reg_ag.num_regs == RFDEVICE_PA_NUM_16BIT_ICQ_REGS)
    {
      this->rffe_script[num_trans-RFDEVICE_PA_NUM_16BIT_ICQ_REGS].data = 0xFF & (pa_icq);
      this->rffe_script[num_trans-RFDEVICE_PA_NUM_8BIT_ICQ_REGS].data = (0xFF00 & (pa_icq)) >> 8;
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "Only 8 bit and 16 bit ICQ values are supported, invalid number of regs %d", pa_reg_ag.num_regs);
    }
  }

  //reset it back to default value
  pa_reg_ag.bias_reg_idx = RF_REG_INVALID;

  //Update PA request type
  pa_params_cfg.req =RFDEVICE_PA_SET_RANGE_DATA;

  /*Obtain the ag settings from PA ag file */
  status &= pa_data_ptr->settings_data_get(&pa_params_cfg, &pa_reg_ag);

  /*Prepare the script buffer with the obtained ag settings*/
  status &= prepare_buffer(&pa_reg_ag, script_buffer, execution_type, script_timing, &num_trans);

  if(!status)
  {
    RF_MSG(RF_ERROR, "PA Set TXAGC failed in gain range");
    return status;
  }

  if (pa_params_p->trigger_type != RFDEVICE_PA_TRIGGER_NONE)
  {

    pa_params_cfg.port = 0;
    pa_params_cfg.req = RFDEVICE_PA_TRIGGER_DATA;  /* PA Trigger reg settings */
    status &= pa_data_ptr->settings_data_get(&pa_params_cfg, &pa_reg_ag);
    status &= prepare_buffer(&pa_reg_ag, script_buffer, execution_type, script_timing, &num_trans);

    if(!status)
    {
      RF_MSG(RF_ERROR, "PA Set TXAGC failed in trigger");
      return status;
    }

  }
  if (num_trans < RFDEVICE_PA_MAX_REG_SIZE)
  {


    //timing change in loop to update each element of script
    if (script_timing != RFCOM_INVALID_TIMING_OFFSET) {

      if (num_trans > 0 && (num_trans - 1) < RFDEVICE_NUM_TRANS_TIMING_CALC - 1)
      {
        timing = script_timing - rfdevice_timing_back_calc[num_trans -1];
      }
      else
      {
        timing = script_timing - rfdevice_timing_back_calc[RFDEVICE_NUM_TRANS_TIMING_CALC -1];
      }

      for (num_idx = 0; num_idx < num_trans; num_idx++)
      {
        this->rffe_script[num_idx].start_delta = timing;
      }
    }


    status = rffe_prepare_buffer(script_buffer, this->rffe_script,
                                 execution_type, this->rffe_bus,
                                 this->assigned_usid, num_trans);
  }

  if(!status)
  {
    RF_MSG(RF_ERROR, "PA Set TXAGC failed");
    return status;
  }

  return status;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  set PA ON OFF with cfg as param

  @details
  This function can be used to configure gain values for a particular
  band  and mode

  @param cfg
  PA config parameter that contains: on_off,mode, band ,pa_gain_range, pa_icq_pdm ,mod_type

  @param script_buffer
  void pointer to pass address of buffer script

  @param execution_type
  boolean to indicate write to hw or prepare script

  @param script_timing
  time offset for the script where a negative value refers to the time BEFORE the event
*/
boolean rfdevice_pa_common::set_pa_on_off ( rfdevice_pa_cfg* pa_params_p,
    rf_buffer_intf *script_buffer,
    rf_device_execution_type execution_type,
    int16 script_timing)

{

  boolean status = TRUE;

  rfdevice_pa_cfg_params_type pa_params_cfg= {RFDEVICE_PA_DATA_REQ_INVALID,0,0,0};
  rfdevice_pa_reg_settings_type pa_reg_ag = { 0 };
  uint8 num_trans = 0;
  uint8 num_idx = 0;
  uint8 port_num = RFDEVICE_PA_INVALID_PORT;
  int16 timing;

  //Validity check on requested PA settings
  if(!rfdevice_pa_param_validity(pa_params_p->mode,pa_params_p->band, RFDEVICE_PA_BW_NOT_APPLICABLE,
                                 script_buffer, execution_type, script_timing))
  {
    RF_MSG_5(RF_ERROR, "PA Set on_off invalid param: usid=%d mode %d, band %d, on_off %d, mod %d",
             this->assigned_usid,pa_params_p->mode, pa_params_p->band.lte_band,
             pa_params_p->on_off, pa_params_p->is_mod_type_gmsk);
    return FALSE;
  }

  /*Get the port number for band and mode requested*/
  port_num = get_port(pa_params_p->mode, pa_params_p->band, pa_params_p->is_mod_type_gmsk);

  /*Check if invalid port info*/
  if (port_num == RFDEVICE_PA_INVALID_PORT)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Invalid port number read", 0);
    return FALSE;
  }


  RFDEVICE_LOG_MSG_7(LOG_RFA_DEVICE_PA_C, MSG_LEGACY_HIGH,
                     "set_pa_on_off(usid=%d, mode=%d, band=%d, on_off=%d, mod_type=%d exec_type=%d, timing=%d)",
                     this->assigned_usid, pa_params_p->mode, pa_params_p->band.cdma_band,
                     pa_params_p->on_off, pa_params_p->is_mod_type_gmsk,
                     execution_type, script_timing);

  /*update rfdevice_pa_cfg_params_type to obtain ag register settings*/
  pa_params_cfg.port  = port_num;

  //Update PA request type
  if (pa_params_p->on_off)
  {
    pa_params_cfg.req =RFDEVICE_PA_ON_DATA;
  }
  else
  {
    pa_params_cfg.req =RFDEVICE_PA_OFF_DATA;
  }

  /*Obtain the ag settings from PA ag file */
  status &= pa_data_ptr->settings_data_get(&pa_params_cfg, &pa_reg_ag);

  /*Prepare the script buffer with the obtained ag settings*/
  status &= prepare_buffer(&pa_reg_ag, script_buffer, execution_type, script_timing, &num_trans);

  if(!status)
  {
    RF_MSG(RF_ERROR, "PA Set ON_OFF failed");
    return status;
  }

  if  (num_trans == 0 && pa_params_p->on_off == TRUE)
  {
    RF_MSG(RF_MED, "PA does not have exclusive ON register, using low PA gain and default ICQ to turn ON PA");
    pa_params_p->pa_gain_range = this->low_gain_range;
    pa_params_p->use_default_icq_pdm = TRUE;
    set_pa_txagc(pa_params_p, script_buffer, execution_type, script_timing);
  }

  if ((pa_params_p->trigger_type != RFDEVICE_PA_TRIGGER_NONE) && (num_trans != 0))
  {
    pa_params_cfg.port = 0;
    pa_params_cfg.req = RFDEVICE_PA_TRIGGER_DATA;  /* PA Trigger reg settings */
    status &= pa_data_ptr->settings_data_get(&pa_params_cfg, &pa_reg_ag);
    status &= prepare_buffer(&pa_reg_ag, script_buffer, execution_type, script_timing, &num_trans);

    if(!status)
    {
      RF_MSG(RF_ERROR, "PA Set ON_OFF failed in trigger");
      return status;
    }

  }
  if (num_trans < RFDEVICE_PA_MAX_REG_SIZE)
  {
    //timing change in loop to update each element of script
    if (script_timing != RFCOM_INVALID_TIMING_OFFSET)
    {
      if (num_trans > 0 && (num_trans - 1) < RFDEVICE_NUM_TRANS_TIMING_CALC - 1)
      {
        timing = script_timing - rfdevice_timing_back_calc[num_trans -1];
      }
      else
      {
        timing = script_timing - rfdevice_timing_back_calc[RFDEVICE_NUM_TRANS_TIMING_CALC -1];
      }

      for (num_idx = 0; num_idx < num_trans; num_idx++)
      {
        this->rffe_script[num_idx].start_delta = timing;
      }
    }

    status = rffe_prepare_buffer(script_buffer, this->rffe_script,
                                 execution_type, this->rffe_bus,
                                 this->assigned_usid, num_trans);
  }

  if(!status)
  {
    RF_MSG(RF_ERROR, "PA Set ON_OFF failed");
    return status;
  }

  return status;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief trigger/latch the pending pa register settings.

  @param script_buffer
  void pointer to pass address of buffer script

  @param write_imm
  boolean to indicate write to hw or prepare script

  @param script_timing
  time offset for the script where a negative value refers to the time BEFORE the event

  @return
  returns TRUE if successful else FALSE.
*/
boolean rfdevice_pa_common::set_pa_trigger(rfdevice_pa_cfg* pa_params_p, rf_buffer_intf *script_buffer, rf_device_execution_type execution_type,
    int16 script_timing)
{
  boolean status = TRUE;
  rfdevice_pa_cfg_params_type pa_cfg = {RFDEVICE_PA_DATA_REQ_INVALID,0,0,0};
  rfdevice_pa_reg_settings_type pa_settings = {0};
  uint8 num_trans = 0;
  if (pa_params_p->trigger_type == RFDEVICE_PA_TRIGGER_ALL || pa_params_p->trigger_type == RFDEVICE_PA_TRIGGER_ON_OFF || pa_params_p->trigger_type == RFDEVICE_PA_TRIGGER_TXAGC)
  {
    RFDEVICE_LOG_MSG_3(LOG_RFA_DEVICE_PA_C, MSG_LEGACY_HIGH,"trigger(usid=%d, exec_type=%d, timing=%d)",
                       this->assigned_usid, execution_type, script_timing);

    pa_cfg.port = 0;
    pa_cfg.req = RFDEVICE_PA_TRIGGER_DATA;  /* PA Trigger reg settings */
    status &= pa_data_ptr->settings_data_get(&pa_cfg, &pa_settings);
    status &= prepare_buffer(&pa_settings, script_buffer, execution_type, script_timing, &num_trans);

    if (num_trans < RFDEVICE_PA_MAX_REG_SIZE)
    {
      status &= rffe_prepare_buffer(script_buffer, this->rffe_script,
                                    execution_type, this->rffe_bus,
                                    this->assigned_usid, num_trans);
    }

    if(!status)
    {
      RF_MSG(RF_ERROR, "PA Trigger failed");
      return status;
    }
  }
  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dummy init member

  @details
  PA class derives from rfdevice_class which has a member init(). This
  member when not defined in asm class will cause it to behave as an
  implicity abstract class resulting in compile error.

*/
void rfdevice_pa_common::init()
{

  boolean status = TRUE;
  rfdevice_pa_cfg_params_type pa_params_cfg= {RFDEVICE_PA_DATA_REQ_INVALID,0,0,0};
  rfdevice_pa_reg_settings_type pa_reg_ag = { 0 };
  uint8 num_trans = 0;
  uint8 num_idx = 0;
  uint8 port_num = RFDEVICE_PA_INVALID_PORT;
  int16 timing;

  pa_params_cfg.port  = 0;

  pa_params_cfg.req = RFDEVICE_PA_INIT_DATA;

  //Setup the requested PA configuration

  /*Obtain the ag settings from PA ag file */
  status &= pa_data_ptr->settings_data_get(&pa_params_cfg, &pa_reg_ag);

  /*Prepare the script buffer with the obtained ag settings*/
  status &= prepare_buffer(&pa_reg_ag, NULL, RFDEVICE_EXECUTE_IMMEDIATE, 0, &num_trans);

  /*Trigger PA INIT*/
  if ((num_trans != 0) && (!this->lpm_pm_trig_flag))
  	{
  	   pa_params_cfg.port = 0;
       pa_params_cfg.req = RFDEVICE_PA_TRIGGER_DATA;  /* PA Trigger INIT settings */
       status &= pa_data_ptr->settings_data_get(&pa_params_cfg, &pa_reg_ag);
       status &= prepare_buffer(&pa_reg_ag, NULL, RFDEVICE_EXECUTE_IMMEDIATE, 0, &num_trans);

       if(!status)
       {
          RF_MSG(RF_ERROR, "PA INIT failed in trigger");
          return;
       }
 
  	}

  if (num_trans < RFDEVICE_PA_MAX_REG_SIZE)
  {

    status = rffe_prepare_buffer(NULL, this->rffe_script,
                                 RFDEVICE_EXECUTE_IMMEDIATE, this->rffe_bus,
                                 this->assigned_usid, num_trans);
  }

  if(!status)
  {
    RF_MSG(RF_ERROR, "PA init failed");
  }

 
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Put the PA in sleep or low power mode

  @details
  This function can be used to put the PA in sleep mode or ultra low power mode

  @param script_buffer
  void pointer to pass address of buffer script

  @param write_imm
  boolean to indicate write to hw or prepare script

  @param script_timing
  time offset for the script where a negative value refers to the time BEFORE the event
*/
boolean rfdevice_pa_common::sleep
(
  rfdevice_pa_cfg* pa_params_p,
  rf_buffer_intf *script_buffer,
  rf_device_execution_type execution_type,
  int16 script_timing
)
{

boolean status = TRUE;
  rfdevice_pa_cfg_params_type pa_params_cfg= {RFDEVICE_PA_DATA_REQ_INVALID,0,0,0};
  rfdevice_pa_reg_settings_type pa_reg_ag = { 0 };
  uint8 num_trans = 0;
  uint8 num_idx = 0;
  uint8 port_num = RFDEVICE_PA_INVALID_PORT;
  int16 timing;

  pa_params_cfg.port  = 0;

  pa_params_cfg.req = RFDEVICE_PA_SLEEP_DATA;

  //Setup the requested PA configuration

  /*Obtain the ag settings from PA ag file */
  status &= pa_data_ptr->settings_data_get(&pa_params_cfg, &pa_reg_ag);

  /*Prepare the script buffer with the obtained ag settings*/
  status &= prepare_buffer(&pa_reg_ag, script_buffer, execution_type, script_timing, &num_trans);

  if ((pa_params_p->trigger_type != RFDEVICE_PA_TRIGGER_NONE) && (num_trans != 0) && (!this->lpm_pm_trig_flag))
  {
    pa_params_cfg.port = 0;
    pa_params_cfg.req = RFDEVICE_PA_TRIGGER_DATA;  /* PA Trigger reg settings */
    status &= pa_data_ptr->settings_data_get(&pa_params_cfg, &pa_reg_ag);
    status &= prepare_buffer(&pa_reg_ag, script_buffer, execution_type, script_timing, &num_trans);

    if(!status)
    {
      RF_MSG(RF_ERROR, "PA sleep failed in trigger");
      return status;
    }

  }

  if (num_trans < RFDEVICE_PA_MAX_REG_SIZE)
  {

    //timing change in loop to update each element of script
    if (script_timing != RFCOM_INVALID_TIMING_OFFSET)
    {

      if ( num_trans > 0 && (num_trans - 1) < RFDEVICE_NUM_TRANS_TIMING_CALC - 1)
      {
        timing = script_timing - rfdevice_timing_back_calc[num_trans -1];
      }
      else
      {
        timing = script_timing - rfdevice_timing_back_calc[RFDEVICE_NUM_TRANS_TIMING_CALC -1];
      }

      for (num_idx = 0; num_idx < num_trans; num_idx++)
      {
        this->rffe_script[num_idx].start_delta =timing;
      }
    }
    status = rffe_prepare_buffer(script_buffer, this->rffe_script,
                                 execution_type, this->rffe_bus,
                                 this->assigned_usid, num_trans);
  }

  if(!status)
  {
    RF_MSG(RF_ERROR, "PA sleep failed");
    return status;
  }

  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Wakeup the PA

  @details
  This function can be used to pull PA out of sleep mode

  @param script_buffer
  void pointer to pass address of buffer script

  @param write_imm
  boolean to indicate write to hw or prepare script

  @param script_timing
  time offset for the script where a negative value refers to the time BEFORE the event
*/
boolean rfdevice_pa_common::wakeup
(
  rfdevice_pa_cfg* pa_params_p,
  rf_buffer_intf *script_buffer,
  rf_device_execution_type execution_type,
  int16 script_timing
)
{
  boolean status = TRUE;
  rfdevice_pa_cfg_params_type pa_params_cfg= {RFDEVICE_PA_DATA_REQ_INVALID,0,0,0};
  rfdevice_pa_reg_settings_type pa_reg_ag = { 0 };
  uint8 num_trans = 0;
  uint8 num_idx = 0;
  uint8 port_num = RFDEVICE_PA_INVALID_PORT;
  int16 timing;

  pa_params_cfg.port  = 0;
	
  pa_params_cfg.req = RFDEVICE_PA_WAKEUP_DATA;

  //Setup the requested PA configuration

  /*Obtain the ag settings from PA ag file */
  status &= pa_data_ptr->settings_data_get(&pa_params_cfg, &pa_reg_ag);

  /*Prepare the script buffer with the obtained ag settings*/
  status &= prepare_buffer(&pa_reg_ag, script_buffer, execution_type, script_timing, &num_trans);

  if ((pa_params_p->trigger_type != RFDEVICE_PA_TRIGGER_NONE) && (num_trans != 0) && (!this->lpm_pm_trig_flag))
  {
    pa_params_cfg.port = 0;
    pa_params_cfg.req = RFDEVICE_PA_TRIGGER_DATA;  /* PA Trigger reg settings */
    status &= pa_data_ptr->settings_data_get(&pa_params_cfg, &pa_reg_ag);
    status &= prepare_buffer(&pa_reg_ag, script_buffer, execution_type, script_timing, &num_trans);

    if(!status)
    {
      RF_MSG(RF_ERROR, "PA wakeup failed in trigger");
      return status;
    }

  }

  if (num_trans < RFDEVICE_PA_MAX_REG_SIZE)
  {

    //timing change in loop to update each element of script
    if (script_timing != RFCOM_INVALID_TIMING_OFFSET)
    {

      if ( num_trans > 0 && (num_trans - 1) < RFDEVICE_NUM_TRANS_TIMING_CALC - 1)
      {
        timing = script_timing - rfdevice_timing_back_calc[num_trans -1];
      }
      else
      {
        timing = script_timing - rfdevice_timing_back_calc[RFDEVICE_NUM_TRANS_TIMING_CALC -1];
      }

      for (num_idx = 0; num_idx < num_trans; num_idx++)
      {
        this->rffe_script[num_idx].start_delta =timing;
      }
    }
    status = rffe_prepare_buffer(script_buffer, this->rffe_script,
                                 execution_type, this->rffe_bus,
                                 this->assigned_usid, num_trans);
  }

  if(!status)
  {
    RF_MSG(RF_ERROR, "PA wakeup failed");
    return status;
  }

  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Obtain PA script size

  @return script_size
  uint16 value of size of the rffe script
*/
uint16 rfdevice_pa_common::get_script_size()
{
  uint16 script_size = 0;

  // Get the script size from autogen table

  return script_size;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Store Tx band port information

  @details
  Band port information which is pushed from RFC is stored in the device using
  this API

  @param mode
  enum indicating the radio access technology

  @param band
  enum indicating the band number

  @param data
  void pointer to pass or receive desired information
*/

boolean rfdevice_pa_common::set_band_map
(
  rfcom_mode_enum_type mode,
  rfcom_band_type_u band,
  int32 *data_ptr,
  uint8 size
)
{

  uint8 pri_port = RFDEVICE_PA_INVALID_PORT;	/* primary port */
  uint8 sec_port = RFDEVICE_PA_INVALID_PORT;	/* secondary port */
  uint8 index = 0;								/* counter for device #*/
  uint8 switch_st = DEFAULT_PA_SWITCH_STATE;

  if (data_ptr == NULL)
  {
    print_device_info();
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid band_port_map_info. mode = %d band = %d", mode, band.cdma_band);
    return FALSE;
  }


  for(index = 0; index < size; index++)
  {
    if (this->mfg_id == RFDEVICE_MFG_ID_GET(data_ptr[index]) && this->prd_id == RFDEVICE_PRD_ID_GET(data_ptr[index]))
    {
      pri_port = (uint8)RFDEVICE_PORT_NUM_GET(data_ptr[index]);
      sec_port = (uint8)RFDEVICE_SEC_PORT_NUM_GET(data_ptr[index]);
      break;
    }
  }

  switch (mode)
  {
  case RFCOM_WCDMA_MODE:
    if (band.wcdma_band < RFCOM_NUM_WCDMA_BANDS)
    {
      this->wcdma_band_port[band.wcdma_band] = pri_port;
    }
    break;

  case RFCOM_1X_MODE:
  case RFCOM_1XEVDO_MODE:
    if (band.cdma_band < RFM_CDMA_MAX_BAND)
    {
      this->cdma_band_port[band.cdma_band] = pri_port;
    }
    break;

  case RFCOM_GSM_MODE:
    if (band.gsm_band < RFCOM_NUM_GSM_BANDS)
    {
      this->gsm_band_pri_port[band.gsm_band] = pri_port;
      this->gsm_band_sec_port[band.gsm_band] = sec_port;
    }
    break;

  case RFCOM_LTE_MODE:
    if (band.lte_band < RFCOM_NUM_LTE_BANDS)
    {
      switch_st = this->switch_state_lte[band.lte_band];
      this->lte_band_port[band.lte_band][switch_st] = pri_port;
    }
    break;

  case RFCOM_TDSCDMA_MODE:
    if (band.tdscdma_band < RFCOM_NUM_TDSCDMA_BANDS)
    {
      switch_st = this->switch_state_tdscdma[band.tdscdma_band];
      this->tdscdma_band_port[band.tdscdma_band][switch_st] = pri_port;
    }
    break;

  default:
    break;
  }

  if (pri_port == RFDEVICE_PA_INVALID_PORT)
  {
    print_device_info();
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"Un-supported Tech = %d and band = %d", mode, band.cdma_band);
  }


  return TRUE;
}

/*---------------------------------------------------------------------------*/
/*
@brief
set the temperature sensor
@returns True or false
 */

boolean rfdevice_pa_common::pa_therm_on_off(boolean turn_on_off)
{
  boolean status = TRUE;
  uint8 reg_read =0;
  rfdevice_pa_cfg_params_type pa_cfg;
  rfdevice_pa_cmd_seq_type pa_cmd_seq = {0};

  if (turn_on_off == TRUE)                                                 /*if request it to measure */
  {
    pa_cfg.req = RFDEVICE_PA_THERM_ON_DATA;
  }
  else
  {
    pa_cfg.req = RFDEVICE_PA_THERM_OFF_DATA;
  }

  status &= this->pa_data_ptr->sequence_data_get(&pa_cfg, &pa_cmd_seq);
  status &= process_cmd_sequence(this->assigned_usid, &pa_cmd_seq, &reg_read);

  return(status);
}

/*---------------------------------------------------------------------------*/
/*
@brief
Function returns the address of the Look up table for temperature reading
@returns True or false
*/
boolean rfdevice_pa_common::get_pa_temperature(int16 *temperature)
{
  boolean status = TRUE;
  uint8 reg_read = 0;
  int16 **lut_addr;
  rfdevice_pa_cfg_params_type pa_cfg;
  rfdevice_pa_cmd_seq_type pa_cmd_seq = {0};
  int16 *temp_addr = 0;
  uint8 lut_size = 0;
  lut_addr = &temp_addr;

  if (temperature == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Temperature pointer = NULL!", 0);
    return FALSE;
  }

  pa_cfg.req = RFDEVICE_PA_THERM_READ_DATA;
  status &= this->pa_data_ptr->sequence_data_get(&pa_cfg, &pa_cmd_seq);
  status &= process_cmd_sequence(this->assigned_usid, &pa_cmd_seq, &reg_read);
  status &= pa_data_ptr->get_temperature_lut(lut_addr, &lut_size);

  if(reg_read > lut_size || *lut_addr==NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Therm LUT read is NULL", 0);
    return FALSE;
  }

  if (status == TRUE)
  {
    *lut_addr = *lut_addr + reg_read;
    *temperature = **lut_addr;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"temperature read :%d", *temperature );
  }

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

boolean rfdevice_pa_common::set_switch_state(rfcom_mode_enum_type mode,
    rfcom_band_type_u band,
    uint8 switch_state_set)   //port indexed  as 0 and 1
{
  boolean status = FALSE;

  if(switch_state_set >= TOTAL_PA_SWITCH_STATE)
  {
    return(FALSE);
  }

  /*Start of case for tech band port switch state*/
  switch (mode)
  {
  case RFCOM_LTE_MODE:
    if (band.lte_band < RFCOM_NUM_LTE_BANDS)
    {
      this->switch_state_lte[band.lte_band] = switch_state_set;
      status = TRUE;
    }
    break;

  case RFCOM_TDSCDMA_MODE:
    if (band.tdscdma_band < RFCOM_NUM_TDSCDMA_BANDS)
    {
      this->switch_state_tdscdma[band.tdscdma_band] = switch_state_set;
      status = TRUE;
    }
    break;

  default:
    break;
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

  @param pa_mod_cfg
  structure with settings of register type to be modified in autogen file
  based on tech and band

  @param pa_mod_reg_ag
  register settings to be modified in autogen file
  contains registers address and value to be updated
*/

boolean rfdevice_pa_common::modify_pa_ag_setting(rfdevice_pa_mod_cfg_type *pa_mod_cfg,
    rfdevice_mod_reg_info_type *pa_mod_reg_ag)
{
  boolean status = FALSE;
  uint8 index =0;
  uint8 port_num = RFDEVICE_PA_INVALID_PORT;

  /*Variables to acess the ag settings and ag register information*/
  rfdevice_pa_cfg_params_type pa_params_cfg = {RFDEVICE_PA_DATA_REQ_INVALID,0,0,0};

  /*Variable to store the ag register info for a given PA register type*/
  rfdevice_pa_reg_settings_type pa_reg_ag = {0};

  /*Check if invalid data is passed to the function*/
  if ((pa_mod_cfg->reg_type) >= RFDEVICE_PA_MAX_CMD
      || pa_mod_reg_ag->reg_data == NULL
      || pa_mod_reg_ag->reg_addr == NULL
     )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid register settings information", 0);
    return FALSE;
  }

  /*Get the port number for band and mode requested*/
  port_num = get_port(pa_mod_cfg->mode, pa_mod_cfg->band, FALSE);

  /*Check if invalid port info*/
  if (port_num == RFDEVICE_PA_INVALID_PORT)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid port number read", 0);
    return FALSE;
  }

  /*Match the PA register type of rfdevice_pa_mod_cfg_type against
    rfdevice_pa_cfg_params_type*/
  switch (pa_mod_cfg->reg_type)
  {
  case RFDEVICE_PA_ON:
    pa_params_cfg.req = RFDEVICE_PA_ON_DATA; /* PA ON reg settings */
    break;

  case RFDEVICE_PA_OFF:
    pa_params_cfg.req = RFDEVICE_PA_OFF_DATA;
    break;

  case RFDEVICE_PA_RANGE:
    pa_params_cfg.req = RFDEVICE_PA_SET_RANGE_DATA;
    break;

  case RFDEVICE_PA_BIAS:
    pa_params_cfg.req = RFDEVICE_PA_SET_BIAS_DATA;
    break;

  default:
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid register type information %d", pa_params_cfg.req);
    return FALSE;
    break;
  }

  /*update rfdevice_pa_cfg_params_type to obtain ag register settings*/
  pa_params_cfg.pa_range = pa_mod_cfg->gain_range;
  pa_params_cfg.port  = port_num;
  pa_params_cfg.pa_bias = 0;
  status = pa_data_ptr->settings_data_get(&pa_params_cfg, &pa_reg_ag);

  /*check if ag num registers doesnt match input num registers to be modified*/
  if ((status == TRUE)&&(pa_reg_ag.num_regs == pa_mod_reg_ag->max_num_reg))
  {
    /*scan through the register addresses for validation*/
    for ( index=0; index < (pa_reg_ag.num_regs); index++)
    {
      if (pa_reg_ag.addr[index] != pa_mod_reg_ag->reg_addr[index])
      {
        MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Failed to match register address with autogen settings", 0);
        return FALSE;
      }
    }

    /*on successful verification of register addresses copy register contents*/
    memscpy( pa_reg_ag.data , sizeof(int16)*pa_reg_ag.num_regs,
             pa_mod_reg_ag->reg_data, sizeof(int16)*pa_mod_reg_ag->max_num_reg );

    status = TRUE;
  }

  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Mismatch between number of registers in autogen and register info update ", 0);
    status = FALSE;
  }

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief performs the self test procedure for the device and returns TRUE or FALSE.

  @details
  returns TRUE if self test is successful, FALSE otherwise.
*/
boolean rfdevice_pa_common::self_test(uint32 mfg_id, uint32 prd_id, uint32 rev_id)
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
  PA device based on the 'executio_type' request.

  @param settings: PA device settings info. like regAddr and regValues.
  @param script_buffer : buffer to populate the PA device register settings.
  @param execution_type : IMMIDEATE -> send settings to PA device
                          CREATE_SCRIPT -> populate the settings to 'script_buffer'.
  @param timing : timing value to populate into the script_buffer. Not applicable if the
                  execution_type = IMMEDIATE.

  @return TRUE - if successful else FALSE.
*/

boolean rfdevice_pa_common::prepare_buffer(rfdevice_pa_reg_settings_type *pa_settings,
    rf_buffer_intf *script_buffer,
    rf_device_execution_type execution_type,
    int16 timing, uint8 *num_trans)
{
  boolean status = FALSE;
  uint8 script_idx = 0;

  if (pa_settings == NULL )
  {
    print_device_info();
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"pa_settings = 0x%X", pa_settings);
    return status;
  }

  script_idx = (*num_trans);

  for (uint8 i=0; i < pa_settings->num_regs; i++)
  {
    if (RFFE_INVALID_REG_ADDR != pa_settings->addr[i])
    {
      if (script_idx >= RFDEVICE_PA_MAX_REG_SIZE)
      {
        MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"Script Index %d out of bounds; Script Size: %d",
              script_idx, RFDEVICE_PA_MAX_REG_SIZE);
        break;
      }

      this->rffe_script[script_idx].addr = pa_settings->addr[i];
      this->rffe_script[script_idx].data = (uint8)(pa_settings->data[i]);
      this->rffe_script[script_idx].start_delta = timing;
      //RF_MSG_2(RF_MED, "PA ICQ reg addr %d, value %d", this->rffe_script[script_idx].addr, this->rffe_script[script_idx].data);
      script_idx++;
    }
  }

  *num_trans = script_idx;

  status = TRUE;                      //return true if buffer operation was success
  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  get the PA port information for the requested mode and band.

  @param mode: technology info.
  @param band : frequency band.

  @return port - pa port value for the requested tech and band.
*/
uint8 rfdevice_pa_common::get_port(rfcom_mode_enum_type mode, rfcom_band_type_u band, boolean mod_type)
{
  uint8 port = RFDEVICE_PA_INVALID_PORT;
  uint8 switch_st = DEFAULT_PA_SWITCH_STATE;

  switch (mode)
  {
  case RFCOM_WCDMA_MODE:
    if (band.wcdma_band < RFCOM_NUM_WCDMA_BANDS)
    {
      port = this->wcdma_band_port[band.wcdma_band];
    }
    break;

  case RFCOM_1X_MODE:
  case RFCOM_1XEVDO_MODE:
    if (band.cdma_band < RFM_CDMA_MAX_BAND)
    {
      port = this->cdma_band_port[band.cdma_band];
    }
    break;

  case RFCOM_GSM_MODE:
    if (band.gsm_band < RFCOM_NUM_GSM_BANDS)
    {
      if (mod_type)
      {
        //Select GSM port settings
        port = this->gsm_band_pri_port[band.gsm_band];
      }
      else
      {
        //Select EDGE port settings
        port = this->gsm_band_sec_port[band.gsm_band];
      }
    }
    break;

  case RFCOM_LTE_MODE:
    if (band.lte_band < RFCOM_NUM_LTE_BANDS)
    {
      switch_st = this->switch_state_lte[band.lte_band];
      port = this->lte_band_port[band.lte_band][switch_st];
    }
    break;

  case RFCOM_TDSCDMA_MODE:
    if (band.tdscdma_band < RFCOM_NUM_TDSCDMA_BANDS)
    {
      switch_st = this->switch_state_tdscdma[band.tdscdma_band];
      port = this->tdscdma_band_port[band.tdscdma_band][switch_st];

    }
    break;

  default:
    break;
  }

  if (port == RFDEVICE_PA_INVALID_PORT)
  {
    print_device_info();
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"Un-supported Tech = %d and band = %d", mode, band.cdma_band);
  }

  return port;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Reset the PA device. The reset sequemce for each device can be different. So, get
  the register sequence writes/reads based on the PA device and execute the sequence.

  @param None.
  @return None.
*/
void rfdevice_pa_common::reset(uint8 usid)
{
  boolean status = FALSE;
  rfdevice_pa_cfg_params_type pa_cfg;
  rfdevice_pa_cmd_seq_type pa_cmd_seq = {0};

  pa_cfg.req = RFDEVICE_PA_RESET_DATA;

  /* get the PA regsiter settings for the requested configuration */
  status = pa_data_ptr->sequence_data_get(&pa_cfg, &pa_cmd_seq);
  if (status == TRUE)
  {
    status = process_cmd_sequence(usid, &pa_cmd_seq, (uint8 *)NULL);
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process the PA register read/write sequence commands. These sequence cmds are
  processed as immediate/direct write/read to device. The construnction of buffer
  script is not supported.

  @param None.
  @return None.
*/
boolean rfdevice_pa_common::process_cmd_sequence(uint8 usid, rfdevice_pa_cmd_seq_type *cmd_seq,
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
    case RFDEVICE_PA_CMD_WRITE:
      dev_setup.half_rate = FALSE;
      status &= rffe_write_to_hw(this->rffe_bus, usid, cmd_seq->cmds[i].addr, cmd_seq->cmds[i].data, &dev_setup);
      break;

    case RFDEVICE_PA_CMD_WRITE_EXT:
      dev_setup.half_rate = FALSE;
      status &= rffe_write_to_hw_ext(this->rffe_bus, usid, cmd_seq->cmds[i].addr, cmd_seq->cmds[i].data, &dev_setup);
      break;

    case RFDEVICE_PA_CMD_DELAY:
      DALSYS_BusyWait(cmd_seq->cmds[i].data);
      break;

    case RFDEVICE_PA_CMD_READ:
      if (reg_read != NULL)
      {
        dev_setup.half_rate = TRUE;
        status &= rffe_read_from_hw(rffe_bus, usid, cmd_seq->cmds[i].addr, reg_read, &dev_setup);
        *reg_read &= (cmd_seq->cmds[i].mask);
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
      MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid PA Sequence Command! cmd = %d addr=0x%x data=0x%x",
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
  Checks parameter validity

  @details
  This function can be used to check the parameter validity passed onto the PA by the techs

  @param mode : enum indicating the radio access technology
  @param band : enum indicating band number
  @param pa_range : PA gain range value.
  @param mod_type : Signal modulation type, applicable only for GSM.
  @param script_buffer : void pointer to pass address of buffer script
  @param execution_type : boolean to indicate write to hw or prepare script
  @param script_timing : time offset for the script where a negative value refers to the time BEFORE the event
*/
//Note - Funtion will be updated with some more changes soon
boolean rfdevice_pa_common::rfdevice_pa_param_validity
(
  rfcom_mode_enum_type mode,
  rfcom_band_type_u band,
  rfdevice_pa_bw_enum_type bw,
  rf_buffer_intf *script_buffer,
  rf_device_execution_type dev_action,
  int16 script_timing
)
{
  boolean status = TRUE;
  /* Dev action check */
  if( dev_action != RFDEVICE_CREATE_SCRIPT && dev_action != RFDEVICE_EXECUTE_IMMEDIATE)
  {
    RF_MSG( RF_ERROR, "Invalid Script for PA request");
    return FALSE;
  }

  /* Null pointer check */
  if( dev_action == RFDEVICE_CREATE_SCRIPT && script_buffer == NULL)
  {
    RF_MSG( RF_ERROR, "Script not allocated for PA request");
    return FALSE;
  }

  switch (mode)
  {
#ifdef FEATURE_WCDMA
  case RFCOM_WCDMA_MODE:
    /* check bands and bws supported */
    if(band.wcdma_band > RFCOM_NUM_WCDMA_BANDS)
    {
      RF_MSG_1( RF_ERROR, "Invalid WCDMA band %d", band.tdscdma_band);
      return FALSE;
    }
    if(bw >= RFDEVICE_PA_BW_INVALID)
    {
      RF_MSG_1( RF_ERROR, "Invalid WCDMA bw %d", bw);
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
    if(bw >= RFDEVICE_PA_BW_INVALID)
    {
      RF_MSG_1( RF_ERROR, "Invalid CDMA bw %d", bw);
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
    if(bw >= RFDEVICE_PA_BW_INVALID)
    {
      RF_MSG_1( RF_ERROR, "Invalid GSM bw %d", bw);
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
    if(bw >= RFDEVICE_PA_BW_INVALID)
    {
      RF_MSG_1( RF_ERROR, "Invalid LTE bw %d", bw);
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
    if(bw >= RFDEVICE_PA_BW_INVALID)
    {
      RF_MSG_1( RF_ERROR, "Invalid TDSCDMA bw %d", bw);
      return FALSE;
    }
    break;
#endif /*ifdef FEATURE_TDSCDMA*/
  default:
    RF_MSG_1(RF_ERROR, "PA request for invalid tech %d", mode);
    status = FALSE;
    break;
  }
  return status;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Print-out the device information like manufacturer_id, product_id, usid_id, etc.

  @param None.
  @return None.
*/
void rfdevice_pa_common::print_device_info(void)
{
  MSG_5(MSG_SSID_RF, MSG_LEGACY_HIGH,"PA rfdevice Info: bus=%d assigned_usid=%d, mfg_id=0x%X, prd_id=0x%X, rev_id=0x%X",
        this->rffe_bus, this->assigned_usid, this->mfg_id, this->prd_id, this->rev_id);
}

/*----------------------------------------------------------------------------*/  //**vs
/*!
  @brief
  reset the internal rffe_script buffer with default values.

  @param None.
  @return None.
*/
void rfdevice_pa_common::reset_rffe_script(void)
{
  /* initialize the internal RFFE script buffer */
  for (uint8 i = 0; i < RFDEVICE_PA_MAX_REG_SIZE; i++)
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

rfdevice_pa_data* rfdevice_pa_common::check_efs_pa(uint32 mfg_id, uint32 prd_id, uint32 rev_id)
{

  struct fs_dirent *dirent;
  struct fs_stat    temp_buf;
  boolean match_not_found = TRUE;
  char file_path[MAX_PATH_SIZE] = "\0";                       // full file path
  uint8* data = NULL;                                         // ptr to EFS data
  int32 data_size = 0;                                        // size of EFS data
  uint8 i=0;
  uint16 j=0;
  uint8 num_regs;

  static const char path[] =  "/efs_pa/";

  strlcat(file_path,path,MAX_PATH_SIZE);

  EFSDIR * dirp = efs_opendir (path);

  while ((dirent = efs_readdir (dirp)) != 0)
  {
    RFDEVICE_LOG_MSG_1(LOG_RFA_DEVICE_PA_C, MSG_LEGACY_MED, 
                       "File name %s", dirent->d_name);

    file_path[0]= '\0';
    strlcat(file_path,path,MAX_PATH_SIZE);
    /* Construct complete file_path and call EFS Stat */
    strlcat(file_path,dirent->d_name,MAX_PATH_SIZE);

    if(efs_stat(file_path,&temp_buf) == -1)
    {
      RFDEVICE_LOG_MSG_1(LOG_RFA_DEVICE_PA_C, MSG_LEGACY_MED, 
                         "get_info_data: File not found! %s", file_path);
    }

    data_size = (int32)temp_buf.st_size;

    // Allocate memory for data
    data = (uint8*)modem_mem_alloc(data_size, MODEM_MEM_CLIENT_RFA);

    if(data == NULL)
    {
      RFDEVICE_LOG_MSG_2(LOG_RFA_DEVICE_PA_C, MSG_LEGACY_MED,
                         "get_info_data: Memory Allocation failed for info data! file= %s, size=%d",
                         file_path, data_size);
      return NULL;					
    }

    // Obtain contents
    data_size = efs_get(file_path, data, temp_buf.st_size);


    if ((mfg_id == (data[0] | data[1]<<8)) && (prd_id == data[2]) && rev_id == data[4])
    {
      efs_closedir (dirp);
      return (rfdevice_efs_pa_data::get_instance(data));

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
