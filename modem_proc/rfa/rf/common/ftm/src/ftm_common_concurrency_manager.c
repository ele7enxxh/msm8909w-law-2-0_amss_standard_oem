/*!
   @file
   ftm_common_concurrency_manager.c

   @brief
 
*/

/*==============================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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
Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved. 

                       EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: 

when       who     what, where, why
--------------------------------------------------------------------------------
04/10/15   ck      Fix to get correct rfc band mask for LTE CA SCELL
3/26/15    am      [Ported] :Workaround to inform rfc ccmgr still even fails to fix incapability of pcell reconfig 
03/19/15   ck      Bypass reconfig request for LTE CA mode and band/tech transition 
03/19/15   rmb/ck  Update FTM Conc manager state machine if the api returns success.
11/19/14   ck      Add tech protection for ftm ccmgr cleanup
11/13/14   ck      Bypass concurrency management in cal mode and fix implicit declaration 
11/10/14   kg      Add ftm api for setting concurrency manager debug mode
10/22/14   ck      Add allocation status cleanup api
10/22/14   ck      Fix to update alt_path information 
10/22/14   ck      Wrap parameters into a whole struct for future flexiblity
10/16/14   ck      Correct typo from RFM_WLAN_MODES to RFM_ALL_MODES 
10/10/14   ck      Fix DR band support for RxD 
10/03/14   ck      Fix incomplete search criteria through restriction table and incorrect initialization
09/30/14   ck      Fix warnings
09/30/14   ck      Initial revision
 
==============================================================================*/



#include "ftm_common_control.h"
#include "rfc_common.h"
#include "stringl.h" /* for memscpy */
#include "rfm_device_config_type.h"
#include "rfcommon_core_utils.h"
#include "ftm_common_concurrency_manager.h"
#include "rfm.h"
#include "rfcommon_concurrency_manager.h"

static rfm_device_allocation_type static_dev_allocation = 
{
  {
	{RFM_INVALID_MODE,RFM_DEVICE_ALLOCATION_INVALID, SYS_BAND_CLASS_NONE,0, RFM_INVALID_OPERATION, RFM_INVALID_SUB_ID},			
	{RFM_INVALID_MODE,RFM_DEVICE_ALLOCATION_INVALID, SYS_BAND_CLASS_NONE,0, RFM_INVALID_OPERATION, RFM_INVALID_SUB_ID},			
	{RFM_INVALID_MODE,RFM_DEVICE_ALLOCATION_INVALID, SYS_BAND_CLASS_NONE,0, RFM_INVALID_OPERATION, RFM_INVALID_SUB_ID},			
	{RFM_INVALID_MODE,RFM_DEVICE_ALLOCATION_INVALID, SYS_BAND_CLASS_NONE,0, RFM_INVALID_OPERATION, RFM_INVALID_SUB_ID},			
	{RFM_INVALID_MODE,RFM_DEVICE_ALLOCATION_INVALID, SYS_BAND_CLASS_NONE,0, RFM_INVALID_OPERATION, RFM_INVALID_SUB_ID},			
	{RFM_INVALID_MODE,RFM_DEVICE_ALLOCATION_INVALID, SYS_BAND_CLASS_NONE,0, RFM_INVALID_OPERATION, RFM_INVALID_SUB_ID},			
	{RFM_INVALID_MODE,RFM_DEVICE_ALLOCATION_INVALID, SYS_BAND_CLASS_NONE,0, RFM_INVALID_OPERATION, RFM_INVALID_SUB_ID},			
	{RFM_INVALID_MODE,RFM_DEVICE_ALLOCATION_INVALID, SYS_BAND_CLASS_NONE,0, RFM_INVALID_OPERATION, RFM_INVALID_SUB_ID},
  }
};

/*! 
  @brief
  FTM interface to concurrency manager and check combination validity
 
  @details
  Function is to offer an interface in FTM to concurrency manager, including
  static and dynamic concurrency validation. Also maintain a static variable
  to track status on all rfm_dev
 
  @param rfm_device_enum_type rfm_dev,
  current rfm_dev to enter
 
  @param rfcom_band_type_u band,
  current band to use
 
  @param rfm_mode_enum_type tech,
  the target tech to enter
 
  @param rfc_rx_path_type prx_drx
  primary or diversity
 
 
  @return
  TRUE indicates validation is passed
*/

boolean ftm_concurrency_manager_validate_operation( ftm_concurrency_manager_params_type *params)
{
  rfm_devices_configuration_type dev_cfg;
  boolean status = TRUE;
  rfm_bands_bitmask bands_supported;
  rfm_device_allocation_status_type* allocation_status;
  rfm_device_client_info_type dev_client;
  uint8 mask_index = 0;
  uint8 mask_bit_num = 0;
  rfm_device_enum_type dev_index = 0;
  uint16 res_table_index = 0;
  sys_band_class_e_type sys_band = SYS_BAND_CLASS_NONE;

  /* if in calibration, concurrency should be bypassed */ 
  if(rfm_get_calibration_state())
  {    
    MSG(MSG_SSID_FTM, MSG_LEGACY_HIGH, "ftm concurrency management is bypassed in calibration state"); 
    return TRUE;
  }

  /* parameter boundary check */ 
  if(params->rfm_dev >= RFM_MAX_WAN_DEVICES)
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR, "fail to validate cocurrency due to invalid dev %d", params->rfm_dev); 
    return FALSE;
  }


   /*step0: band type conversion rfcom_band to sys_band in rfcommon_core_utils.c */ 
  sys_band = rfcommon_core_convert_rfcom_band_to_sys_band( params->tech, params->band);

  MSG_3(MSG_SSID_FTM, MSG_LEGACY_MED, "INFO: Enter concurrency manager with tech = %d, sys_band = %d on dev %d", params->tech, sys_band, params->rfm_dev);
														  

  /*Step 1: Verify if the band is supported in the mask array, based on band enum value*/

  status = rfm_get_devices_configuration(&dev_cfg);
  if ( ((RFM_DEVICE_2 == params->rfm_dev)  && ((dev_cfg.concurrency_features / RFM_DUAL_RX_DSDS_SUPPORTED)& 0x1))
       && (params->carrier_index == 0) )
  {
    bands_supported = dev_cfg.device_support[params->rfm_dev].dr_bands_supported;
  }
  else if(params->carrier_index != 0)
  {
		bands_supported = dev_cfg.device_support[params->rfm_dev].ca1_bands_supported;
  }
  else
  {
    bands_supported = dev_cfg.device_support[params->rfm_dev].bands_supported;
  }

  mask_bit_num = (sizeof(bands_supported.mask[0])*8);
  mask_index = (uint8)sys_band/mask_bit_num;
  if( !( (bands_supported.mask[mask_index]>>(sys_band % mask_bit_num))&0x1 ) )
  {
	  MSG_2(MSG_SSID_FTM, MSG_LEGACY_ERROR, "unsupported band %d on rfm_dev %d", sys_band, params->rfm_dev);
	  status &= FALSE;
  }

  MSG_3(MSG_SSID_FTM, MSG_LEGACY_MED, "INFO: supported_band_mask[%d] = %d; sys_band mod 64 = %d", mask_index, bands_supported.mask[mask_index], sys_band % mask_bit_num);
  
  /*Step 2: Parse whether there are any static sconcurrency restrictions 
    Need to process dev_cfg.concurrency_restrictions */
  for(dev_index = 0; dev_index < RFM_MAX_WAN_DEVICES; dev_index++)
  {
    if(static_dev_allocation.device_allocation[dev_index].allocation_type == RFM_DEVICE_ASSIGNED)
	{
      MSG_3(MSG_SSID_FTM, MSG_LEGACY_MED, "INFO: discover assigned device on dev_index = %d, tech = %d, band = %d",
            dev_index,
            static_dev_allocation.device_allocation[dev_index].tech,
            static_dev_allocation.device_allocation[dev_index].band);
	  
		for (res_table_index = 0; res_table_index < dev_cfg.concurrency_restrictions.num_restrictions; res_table_index++) 
		{
            
			if(
					(( dev_cfg.concurrency_restrictions.restriction_table[res_table_index].device1 == dev_index ) || (dev_cfg.concurrency_restrictions.restriction_table[res_table_index].device1 == RFM_ALL_DEVICES ))
				&& ((dev_cfg.concurrency_restrictions.restriction_table[res_table_index].band1 == static_dev_allocation.device_allocation[dev_index].band)|| (dev_cfg.concurrency_restrictions.restriction_table[res_table_index].band1 == SYS_BAND_CLASS_NONE))
		        && ((dev_cfg.concurrency_restrictions.restriction_table[res_table_index].mode1 == static_dev_allocation.device_allocation[dev_index].tech)||(dev_cfg.concurrency_restrictions.restriction_table[res_table_index].mode1 == RFM_ALL_MODES)) )
			{
				if (
					(( dev_cfg.concurrency_restrictions.restriction_table[res_table_index].device2 == params->rfm_dev ) || (dev_cfg.concurrency_restrictions.restriction_table[res_table_index].device2 == RFM_ALL_DEVICES ))
				&& ((dev_cfg.concurrency_restrictions.restriction_table[res_table_index].band2 == sys_band)|| (dev_cfg.concurrency_restrictions.restriction_table[res_table_index].band2 == SYS_BAND_CLASS_NONE))
		        && ((dev_cfg.concurrency_restrictions.restriction_table[res_table_index].mode2 == params->tech)||(dev_cfg.concurrency_restrictions.restriction_table[res_table_index].mode2 == RFM_ALL_MODES )) )
				{
					status &= FALSE;
					MSG_3(MSG_SSID_FTM, MSG_LEGACY_ERROR, "restriction table index %d blocks the tech %d on rfm_dev %d", res_table_index, params->tech, params->rfm_dev );

				}
              
			}
			else if(
					(( dev_cfg.concurrency_restrictions.restriction_table[res_table_index].device2 == dev_index ) || (dev_cfg.concurrency_restrictions.restriction_table[res_table_index].device2 == RFM_ALL_DEVICES ))
				&& ((dev_cfg.concurrency_restrictions.restriction_table[res_table_index].band2 == static_dev_allocation.device_allocation[dev_index].band)|| (dev_cfg.concurrency_restrictions.restriction_table[res_table_index].band2 == SYS_BAND_CLASS_NONE))
		        && ((dev_cfg.concurrency_restrictions.restriction_table[res_table_index].mode2 == static_dev_allocation.device_allocation[dev_index].tech)||(dev_cfg.concurrency_restrictions.restriction_table[res_table_index].mode2 == RFM_ALL_MODES)) )
			{
				if (
					(( dev_cfg.concurrency_restrictions.restriction_table[res_table_index].device1 == params->rfm_dev ) || (dev_cfg.concurrency_restrictions.restriction_table[res_table_index].device1 == RFM_ALL_DEVICES ))
				&& ((dev_cfg.concurrency_restrictions.restriction_table[res_table_index].band1 == sys_band)|| (dev_cfg.concurrency_restrictions.restriction_table[res_table_index].band1 == SYS_BAND_CLASS_NONE))
		        && ((dev_cfg.concurrency_restrictions.restriction_table[res_table_index].mode1 == params->tech)||(dev_cfg.concurrency_restrictions.restriction_table[res_table_index].mode1 == RFM_ALL_MODES )) )

				{
					status &= FALSE;
					MSG_3(MSG_SSID_FTM, MSG_LEGACY_ERROR, "restriction table index %d blocks the tech %d on rfm_dev %d", res_table_index, params->tech, params->rfm_dev );

				}
			}

		}
	}
  }
  


  /* Step 3: Validate Device concurrency */
  dev_client.band = sys_band;
  dev_client.rfm_dev = params->rfm_dev;
  dev_client.tech = params->tech;
  allocation_status = rfm_verify_rf_concurrency(&dev_client);

  /* Loop through the curr_status of each rfm_dev from allocation_status and return fail if any of the devices need to be reconfigured*/
  for(dev_index = 0; dev_index < RFM_MAX_WAN_DEVICES; dev_index++)
  {

    /* bypass tech and band transition reconfig request on the same device */ 
    if(dev_index != dev_client.rfm_dev)
    {
      if (allocation_status->curr_status[dev_index] == RFM_DEVICE_RECONFIG_REQUIRED ) 
      {
	/* LTE CA should bypass reconfig request */ 
	if((params->carrier_index>0) && (params->tech == RFM_LTE_MODE))
	{
	  status &= TRUE; 	  	  
          MSG_3(MSG_SSID_FTM, MSG_LEGACY_MED, "Ignore rfconfiguration in LTE CA mode", 
            params->tech,  
            sys_band, params->rfm_dev );
	}
        else
        {
          status &= FALSE;
          MSG_3(MSG_SSID_FTM, MSG_LEGACY_ERROR, "tech %d on dev %d for band %d needs reconfiguration",
	      	  static_dev_allocation.device_allocation[dev_index].tech,  
	      	  dev_index, 
	      	  static_dev_allocation.device_allocation[dev_index].band);
          /* notice that alt_path is not maintained in FTM concurrency manager. Only maintained in rfcmn_concurrency_db */ 		        
        }
	  }
		else
		{		  
		  MSG_2(MSG_SSID_FTM, MSG_LEGACY_MED, "INFO: allocation status %d on device %d and does not require reconfiguration", 
				allocation_status->curr_status[dev_index],
				dev_index);
        status &= TRUE;
      }		
    }
    else
    {
      status &= TRUE;
      MSG_1(MSG_SSID_FTM, MSG_LEGACY_MED, "Ignore reconfig on the requested device %d ", dev_index );      
		}		
  }

  

  /* Step 4: Inform device allocation */
  static_dev_allocation.device_allocation[params->rfm_dev].tech = params->tech;
  static_dev_allocation.device_allocation[params->rfm_dev].band = sys_band;
  static_dev_allocation.device_allocation[params->rfm_dev].allocation_type = (status)?(RFM_DEVICE_ASSIGNED):(RFM_DEVICE_ALLOCATION_INVALID);
   /* this alt_path update currently does not take real effect since rfc concurrency manager will recalculate */ 
  static_dev_allocation.device_allocation[params->rfm_dev].alt_path = allocation_status->alt_path[params->rfm_dev];
  if ( !rfm_inform_device_allocation(&static_dev_allocation))
  {
      MSG(MSG_SSID_FTM, MSG_LEGACY_ERROR, "fail to inform device allocation!");          
      static_dev_allocation.device_allocation[params->rfm_dev].allocation_type = RFM_DEVICE_ALLOCATION_INVALID;
    status &= FALSE;
  }

  MSG_6(MSG_SSID_FTM, MSG_LEGACY_MED, "INFO: exiting concurrency manager with status %d, updated tech = %d , band = %d, dev = %d, alt_path = %d, and allocation result = %d; ", 
		status,
  	static_dev_allocation.device_allocation[params->rfm_dev].tech,  
  	static_dev_allocation.device_allocation[params->rfm_dev].band,
  	params->rfm_dev,
  	static_dev_allocation.device_allocation[params->rfm_dev].alt_path,
  	static_dev_allocation.device_allocation[params->rfm_dev].allocation_type);

  return status;
}




/*! 
  @brief
  FTM interface to clean up allocation status when tech exits
  
  @param rfm_device_enum_type rfm_dev,
  current rfm_dev to enter
 
  @return
  TRUE indicates validation is passed
*/

boolean ftm_concurrency_manager_cleanup_device_allocation( rfm_device_enum_type rfm_dev, rfm_mode_enum_type cur_tech )
{

  /* if in calibration, concurrency should be bypassed */ 
  if(rfm_get_calibration_state())
  {    
    MSG(MSG_SSID_FTM, MSG_LEGACY_HIGH, "ftm concurrency management is bypassed in calibration state"); 
    return TRUE;
  }

  /* do not allow other tech except itself to request cleanup */
  if( (cur_tech == static_dev_allocation.device_allocation[rfm_dev].tech) || (RFM_INVALID_MODE == static_dev_allocation.device_allocation[rfm_dev].tech))
  {
    static_dev_allocation.device_allocation[rfm_dev].tech = RFM_INVALID_MODE;
    static_dev_allocation.device_allocation[rfm_dev].band = SYS_BAND_CLASS_NONE;
    static_dev_allocation.device_allocation[rfm_dev].allocation_type = RFM_DEVICE_ALLOCATION_INVALID;
    static_dev_allocation.device_allocation[rfm_dev].alt_path = 0;
  }
  else
  {
    if (!(cur_tech == static_dev_allocation.device_allocation[rfm_dev].tech))
    {
      MSG_2(MSG_SSID_FTM, MSG_LEGACY_ERROR, "cleanup is not allowed by cur_tech = %d. Only tech %d is permitted", cur_tech, static_dev_allocation.device_allocation[rfm_dev].tech);     
    }
    if (!(RFM_INVALID_MODE == static_dev_allocation.device_allocation[rfm_dev].tech))
    {
      MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR, "dev %d is already cleaned", rfm_dev);     
    }

    return FALSE;
  }
  

  if ( !rfm_inform_device_allocation(&static_dev_allocation))
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR, "fail to inform device allocation on dev %d", rfm_dev); 
    return FALSE;
  }
  else
  {
    MSG_2(MSG_SSID_FTM, MSG_LEGACY_MED, "INFO: clean up allocation status on dev %d for tech %d ", rfm_dev, cur_tech);
    return TRUE;
  }  



}

/*! 
  @brief
  FTM interface to set the debug flags for concurrency manager module
  
  @param *ftm_req_data
  FTM Request Data to configure CDMA Debug mode

  @return
  Response packet for the command
*/
ftm_rsp_pkt_type
ftm_concurrency_manager_configure_debug_mode
(
ftm_common_pkt_type *ftm_req_data
)
{
	ftm_concurrency_mgr_debug_mode_request_packet_type *header =
		(ftm_concurrency_mgr_debug_mode_request_packet_type *)ftm_req_data;

	ftm_rsp_pkt_type ftm_rsp_data;
	ftm_concurrency_mgr_debug_mode_response_packet_type
		*ftm_concurrency_mgr_rsp;

  boolean debug_en;
	boolean ret_val;
  uint32 debug_mask;
	MSG(MSG_SSID_FTM, MSG_LEGACY_HIGH, "ftm_concurrency_manager_configure_debug_mode called");    

  debug_en = (boolean)header->debug_en;
  debug_mask = header->debug_mask;

	ret_val = rfcmn_concurrency_mgr_set_debug_mode(debug_mask, debug_en);
	MSG(MSG_SSID_FTM, MSG_LEGACY_ERROR, "back from debug api");    

	ftm_rsp_data = ftmdiag_create_new_pkt( sizeof (ftm_concurrency_mgr_debug_mode_response_packet_type) );  

	if( ftm_rsp_data.pkt_payload != NULL )
  {
    ftm_concurrency_mgr_rsp = ftm_rsp_data.pkt_payload;
		// Copy data from request to response
		memscpy(ftm_concurrency_mgr_rsp, sizeof(ftm_composite_cmd_header_type), 
						ftm_req_data, sizeof(ftm_composite_cmd_header_type));
		ftm_concurrency_mgr_rsp->ftm_concurrency_mgr_status = (uint32)ret_val;
  }
  else
  {
		MSG(MSG_SSID_FTM, MSG_LEGACY_HIGH, "ftm_concurrency_manager_configure_debug_mode : ftm_rsp_data is NULL");    
  }

	return ftm_rsp_data;

}
