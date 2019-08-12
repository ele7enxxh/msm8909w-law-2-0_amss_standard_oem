/*!
  @file
  ftm_common_enh_internal_device_calibration_rsb.c

  @brief

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/src/ftm_enh_internal_device_calibration_rsb.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
05/10/16   rs      Compiler warning fix for C2K disabled builds
02/10/15    br     Send tech,band,bandwidth and fbrx gain state as inputs to stg enable and disable apis
11/05/14   xsh     Add scell2 rsb cal support
11/05/14   vs      Fix linker errors on W/T compile out flavor builds
11/03/14   ndb     Fix the compiler error in LTE removed builds
10/08/14    ka     Replace rfc with device api for retrieving device pointer
10/02/14   dps     Changed lna_gain_state to gain_state in rfdevice_rsb_cal_data_type
09/12/14   xsh    Move error log init, tech check before flag check so that flag=3 can reset the error status
09/02/14   dr      Added featurization for Non-W/TDS builds
08/29/14   xsh     Remove RFCal FW state update as it should be done in RF setup
08/29/14   xsh     Added deviceId setup when sending tds IQ capture msg to FW
08/29/14   xsh     Fixed compiler warning  
08/11/14   shb     Fixed offtarget compiler error
08/11/14   xsh     Re-set efs write function pointer
08/11/14   xsh     Revert efs save function pointer set as there is bug in those efs save function
08/11/14   xsh     Set efs write function pointer to what's provided by device driver
08/05/14   xsh     Seperate efs write from normal rsb cal
08/04/14   xsh     Extend wait time after IQMC updated
08/04/14   xsh     Remove Bolt Dependency to support other PLs
07/24/14   xsh     Add rsb cal validation check before update iqmc based on cal data
07/14/14   aa      Fix FBRx DC Cal failures due to incorrect condition buffer ID check
07/09/14   jmf     Perform RSB coeff calc with DC removed
07/08/14   jps     FTM RF Mode Rx1 IQ capture fix
07/07/14   xsh     Clean code
07/07/14   xsh     Add support for multiple gain rsb cal
07/01/14   xsh     Add debug level time profile
06/13/14   kab     Add 3xCA support
05/29/14   brath   Updates to provide TX LO frequency for device cal APIs
5/30/14    xsh     add different gain state rsb cal support
5/29/14    xsh     add lte scell rx rsb cal support
4/15/14    xsh     hooking to fbrx rsb cal to rx rsb cal
4/8/14     xsh     add after rsb cal -recal to get the rsb coef for limit computation
3/6/14     xsh     hooking rsb algorithm to SS
1/8/14     xsh     add rsb hard-code algorithm
11/25/13   xsh     Initial Revision  

==============================================================================*/

#ifndef FEATURE_GNSS_ONLY_NO_WWAN

#include "rfa_variation.h"
#include "comdef.h"
#include "modem_mem.h"
#include "string.h"
#include "stringl.h" /* for memscpy */
#include "fs_estdio.h"
#include "ftm_msg.h"
#include "rfm_mode_types.h"
#include "ftm_common_msm.h"
#include "rfcommon_msm.h"
#include "msm.h"
#include "ftm_common_control.h"
#include "ftm_common_data.h"
#include "ftm_common_enh_internal_device_cal.h"
#include "ftm_enh_internal_device_cal_rsb.h"
#include "ftm_enh_internal_device_cal_rsb_alg.h"
#include "rfcommon_fbrx_api.h"
#include "rfcommon_msm.h"
#include "rfcommon_core_utils.h"
#include "rfcommon_mdsp.h"
#include "math.h"

//#include "rfcommon_math.h"

#ifdef FEATURE_LTE
#include "rflte_util.h"
#include "rflte_mc.h"
#include "rfdevice_lte_interface.h"
#include "rflte_ftm_mc.h"
#include "rflte_msm_iq_capture.h"
#include "rflte_state.h"
extern rflte_ftm_mc_sm_type rflte_ftm_mc_sm;
extern rflte_msm_iq_capture_state_type lte_iq_capture_state_machine;
#endif

#ifdef FEATURE_WCDMA
#include "rfwcdma_mc.h"
#include "ftm_hwtc_wcdma_ino.h"
#include "rfdevice_wcdma_intf.h"
#include "rfwcdma_msm.h"
extern ftm_lm_buf_type ftm_wcdma_rxlm_buffer[RFM_DEVICE_4];
extern rfcom_wcdma_band_type ftm_curr_wcdma_mode;
extern ftm_hwtc_wcdma_data_type ftm_hwtc_wcdma_data;
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_TDSCDMA
#include "rfdevice_tdscdma_intf.h"
#include "ftm_tdscdma_ctl.h"
#include "rf_tdscdma_mdsp.h"
#include "rf_tdscdma_core_util.h"
extern ftm_tdscdma_iq_capture_data_type ftm_tds_iq_capture_data;
extern ftm_lm_buf_type ftm_tdscdma_rxlm_buffer[2];
extern void ftm_tdscdma_util_convert_10pcfl_iq_data_format( uint32 *input_buf,
																  word *output_buf,
																  uint32 num_sample);
extern void ftm_tdscdma_util_convert_20pcfl_iq_data_format( uint32 *input_buf,
																  word *output_buf,
																  uint32 num_sample);
#endif

#ifdef FEATURE_CDMA1X
#include "rf_cdma_data.h"
#include "rf_cdma_utils_freq.h"
#endif

//#ifdef FEATURE_BOLT_MODEM
#include "rflm_api_fbrx_fw_hal.h"

//#endif

#ifdef FEATURE_FACTORY_TESTMODE


#define MAX_RSB_CAL_ELEMENT_LTE ((RFCOM_NUM_LTE_BANDS)*(RFM_MAX_WAN_DEVICES)*6)
#define MAX_RSB_CAL_ELEMENT_WCDMA ((RFCOM_NUM_WCDMA_BANDS)*(RFM_MAX_WAN_DEVICES)*4)
#define MAX_RSB_CAL_ELEMENT_TDSCDMA ((RFCOM_NUM_TDSCDMA_BANDS)*(RFM_MAX_WAN_DEVICES)*4)


typedef boolean (*ftm_enh_internal_device_cal_rsb_save_efs_fp)( rfm_device_enum_type rfm_device, 
                                                               uint8 rf_band, 
                                                               rfdevice_rsb_cal_verfication_data_type* rsb_ver_data);

boolean rsb_efs_empty_write( rfm_device_enum_type rfm_device,                                          
								uint8 rf_band, 
								rfdevice_rsb_cal_verfication_data_type* rsb_ver_data)
{
 	if(rsb_ver_data!=NULL)
 	{
		rsb_ver_data->num_calibrated_item =0;
 	}
	return TRUE;
}

static int16 sat_int32_to_int16(int32 in_val, uint8 * overflow_det)
{
  int16 out_val;
  int16 max_val = 0x7FFF;
  int16 min_val = 0x8000;
  *overflow_det = 0;
  if (in_val > (int32)max_val)
  {
    out_val = max_val;
    *overflow_det = 1;
    FTM_MSG_2(FTM_ERROR, "OVERFLOW DETECTED: %d > %d", in_val, max_val);
  }
  if (in_val < (int32)min_val)
  {
    out_val = min_val;
    *overflow_det = 1;
    FTM_MSG_2(FTM_ERROR, "OVERFLOW DETECTED: %d < %d", in_val, min_val);
  }

  if (*overflow_det == 0)
  {
    out_val = (int16)( in_val  & 0xFFFF );
  }
  return out_val;

}


/*----------------------------------------------------------------------------*/
/*!
  @brief: ftm_calv4_convert_band
  
 
  @details: convert band in ftm enum to tech-dependant enum
  
  @param 
	input:	ftm_rf_tech, rf_band
	output: tech dependant band enum
	  
  @return
  Status of initialization
*/

uint16 ftm_calv4_convert_band
(
  ftm_rf_technology_type ftm_rf_tech,
  uint16 rf_band
)
{
  uint16 ret_band = rf_band;
  switch(ftm_rf_tech)
  {
     case FTM_RF_TECH_CDMA:
        // not needed yet, add it later if needed 
		break;
	  
     case FTM_RF_TECH_WCDMA:
	 	 switch(rf_band)
        {
          case WCDMA_FTM_BAND_1:
          ret_band = RFCOM_BAND_IMT;
          break;

          case WCDMA_FTM_BAND_2:
          ret_band = RFCOM_BAND_1900;
           break;

          case WCDMA_FTM_BAND_3:
          ret_band = RFCOM_BAND_BC3;
          break;

          case WCDMA_FTM_BAND_4:
           ret_band = RFCOM_BAND_BC4;
           break;

         case WCDMA_FTM_BAND_5:
         ret_band = RFCOM_BAND_800;
          break;

         case WCDMA_FTM_BAND_8:
          ret_band = RFCOM_BAND_BC8;
          break;

         case WCDMA_FTM_BAND_9:
          ret_band = RFCOM_BAND_BC9;
          break;

        case WCDMA_FTM_BAND_11:
          ret_band = RFCOM_BAND_BC11;
          break;

        case WCDMA_FTM_BAND_19:
          ret_band = RFCOM_BAND_BC19;
          break;

        default:
         ret_band = RFCOM_BAND_INVALID;
         break;
       }
	   
       break;
     case FTM_RF_TECH_LTE:

		ret_band = (uint16) (rf_band-1);	
       break;
     case FTM_RF_TECH_TDSCDMA:	
	 	switch(rf_band)
       {
          case TDSCDMA_FTM_BAND_B34:
          ret_band = RFCOM_BAND_TDSCDMA_B34;
          break;

         case TDSCDMA_FTM_BAND_B39:
          ret_band = RFCOM_BAND_TDSCDMA_B39;
          break;

        case TDSCDMA_FTM_BAND_B40:
         ret_band = RFCOM_BAND_TDSCDMA_B40;
         break;

        default:
         ret_band = RFCOM_BAND_INVALID;
         break;
        } 

	  break;
     case FTM_RF_TECH_GSM:
     default: 
	 // not supported yet
	   break;  	
  }
  return ret_band;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief: ftm_convert_iq_8b_to_16b
  
 
  @details: rx iq capture data format convert: from 8bit iq data to 16 bit iq data 
  
  @param 
	input:
	iq_ptr: 
	reverseOrder: change iq sequence order

	output: iq_ptr
	  
  @return
  Status of initialization
*/
	
boolean ftm_convert_iq_8b_to_16b
( 
  uint32* iq_ptr,  
  boolean reverseOrder, 
  uint16 sample_size
)
{
	boolean api_status = FALSE;
	int8 twoscomplement8_L =0;
	int8 twoscomplement8_H= 0;
	uint32 id =0;	
	if(iq_ptr == NULL)
		return api_status;
	api_status = TRUE;
		
	for( id = 0; id < sample_size; id ++)
	{
		twoscomplement8_L = (int8) (iq_ptr[id] & 0xFF);
		twoscomplement8_H= (int8) (((iq_ptr[id]>>8) & 0xFF));
		if(reverseOrder)
		{
			iq_ptr[id] = (((int16) twoscomplement8_H) &0xFFFF)|(((int16) twoscomplement8_L)<<16) ;		
		}
		else
		{
			iq_ptr[id] = (((int16) twoscomplement8_L) &0xFFFF)|(((int16) twoscomplement8_H)<<16) ;	
		}
	}
  return api_status;

}


/*----------------------------------------------------------------------------*/
/*!
  @brief: ftm_convert_iq_16b_to_16b
  
 
  @details: rx iq capture data format convert: from 8bit iq data to 16 bit iq data 
  
  @param 
	input:
	iq_ptr: 
	reverseOrder: change iq sequence order

	output: iq_ptr
	  
  @return
  Status of initialization
*/
	
boolean ftm_convert_rsb_iq_format
( 
  uint32* iq_ptr,  
  boolean reverseOrder,
  uint8 convert_type,
  uint16 sample_size
)
{
	boolean api_status = FALSE;
	int8 twoscomplement8_L =0;
	int8 twoscomplement8_H= 0;

	uint32 id =0;	
	if(iq_ptr == NULL)
		return api_status;
	api_status = TRUE;

	if(convert_type == FTM_RX_RSB_IQ_8bit_to_16bit )
	{
	  for( id = 0; id < sample_size; id ++)
	  {
		twoscomplement8_L = (int8) (iq_ptr[id] & 0xFF);
		twoscomplement8_H= (int8) (((iq_ptr[id]>>8) & 0xFF));
		if(reverseOrder)
		{
			iq_ptr[id] = (((int16) twoscomplement8_H) &0xFFFF)|(((int16) twoscomplement8_L)<<16) ;		
		}
		else
		{
			iq_ptr[id] = (((int16) twoscomplement8_L) &0xFFFF)|(((int16) twoscomplement8_H)<<16) ;	
		}
	  }
	}  
	else if((convert_type == FTM_RX_RSB_IQ_16bit_to_16bit)&&(reverseOrder))
	{
		for( id = 0; id < sample_size; id ++)
		{	  
			iq_ptr[id] = ( ( (iq_ptr[id] &0xFFFF )<<16 ) &0xFFFF0000) | ((iq_ptr[id]>>16)& 0xFFFF);			  
		}
	}
  return api_status;

}

/*----------------------------------------------------------------------------*/
/*!
  @brief: ftm_enh_internal_device_cal_rx_iq_capture
  
 
  @details: get rx iq data
  
  @param 
  tech: the tech mode that doing rsb cal

  @param
  carrier: the carrier id, for LTE, 

  @param
  rx_path: rx_path: PRX or DRX

  @param
  sample_size: IQ sample size

  @param: output
  iq_dst_ptr: the dst iq pointer to save the iq data

  @return
  Status of iq capture. TRUE: gets iq data; FALSE: not getting iq data
*/

boolean ftm_enh_internal_device_cal_rx_iq_capture
(
  ftm_rf_technology_type ftm_rf_tech,
  uint8 carrier,
  uint8 rx_path,
  uint16 sample_size,
  uint32* iq_dst_ptr
)
{
	ftm_config_iq_log_type iq_config = {0};
	uint32 *iq_buf_ptr = NULL;
	uint32 total_wait_time_limit = 11000; // hard code here , 1ms
	uint32 total_wait_time = 0;

#ifdef FEATURE_LTE 
	rflte_mc_carrier_type lte_carr_rxpath = RFLTE_MC_CARR_TYPE_PRX;
#endif /*FEATURE_LTE*/ 
	boolean api_status = TRUE;

	 switch(ftm_rf_tech)
	 {
		case FTM_RF_TECH_CDMA:
		case FTM_RF_TECH_GSM:
		default: 
		  // not supported yet
		  break;  
	   #ifdef FEATURE_WCDMA   
		case FTM_RF_TECH_WCDMA:

			ftm_hwtc_wcdma_set_iq_capture_antenna( (enum_iq_antenna_type) rx_path  ); 
			iq_config.testbus_sel = ftm_hwtc_wcdma_data.iq_data.dlog_sel;			
			iq_buf_ptr = (uint32 *)rfwcdma_msm_iq_capture(&iq_config);
			DALSYS_BusyWait(200);
			if( iq_buf_ptr != NULL )
			{
			  memscpy(iq_dst_ptr, sample_size*sizeof(int32), iq_buf_ptr, sample_size*sizeof(int32)); // need to convert it to LTE format
			}
			// free rfwcdma iq buffer
			rfwcdma_msm_free_iq_buffer();
			api_status = TRUE;
	
		  break;
  	   #endif /*FEATURE_WCDMA*/
#ifdef FEATURE_LTE 
		case FTM_RF_TECH_LTE:
			
		    lte_carr_rxpath = ((rx_path ==FTM_ENH_INTERNAL_DEVICE_CAL_RXRSB_RES_PRX1) ||(rx_path ==FTM_ENH_INTERNAL_DEVICE_CAL_RXRSB_RES_PRX2)
				                ||(rx_path ==FTM_ENH_INTERNAL_DEVICE_CAL_RXRSB_RES_PRX3))?
								RFLTE_MC_CARR_TYPE_PRX: RFLTE_MC_CARR_TYPE_DRX;
			rflte_msm_iq_capture_config(carrier,lte_carr_rxpath,sample_size);
			total_wait_time = 0;

			while((lte_iq_capture_state_machine.fw_response_flag == FALSE) &&(total_wait_time < total_wait_time_limit))
			{                
				DALSYS_BusyWait(100);
				total_wait_time += 100;
			}
			
			DALSYS_BusyWait(200);
			if(lte_iq_capture_state_machine.fw_response_flag == TRUE)
			{
			  iq_buf_ptr = (uint32 *) ((lte_carr_rxpath == RFLTE_MC_CARR_TYPE_PRX)? 
				lte_iq_capture_state_machine.prx_data_pointer :lte_iq_capture_state_machine.drx_data_pointer);
			  memscpy(iq_dst_ptr, sample_size*sizeof(int32), iq_buf_ptr, sample_size*sizeof(int32)); 
			  api_status = TRUE;
			}
			else
			{
 				memset(iq_dst_ptr, 0,sample_size*sizeof(int32));
				FTM_MSG(FTM_MED,"lte iq capture from fw flag is FALSE");
				api_status = FALSE;
			}
		  break;
#endif /*FEATURE_LTE*/

		#ifdef FEATURE_TDSCDMA 
		case FTM_RF_TECH_TDSCDMA:
 	
			/* Sending iq capture command */
			ftm_tds_iq_capture_data.IqCaptureStatus = FTM_TDSCDMA_IQ_CAPTURE_NO_RSP; //0=clear the flag at first
			ftm_tds_iq_capture_data.MaxWaitCnt =  100;   //todo: change back to 20
			ftm_tds_iq_capture_data.SampleSize = sample_size;
			rf_tdscdma_mdsp_send_mdsp_msg(RF_TDSCDMA_MDSP_IQ_CAPTURE_CFG_MSG, (((sample_size << 16) & 0xFFFF0000) | rx_path ));

			FTM_MSG_1(FTM_MED,"ftm_tdscdma_tune_to_chan: send IQ capture cmd to TFW: sample = %d", sample_size);
			
			/* Waiting for the result from TFW return message */
			while ( (ftm_tds_iq_capture_data.IqCaptureStatus == FTM_TDSCDMA_IQ_CAPTURE_NO_RSP) &&
					(ftm_tds_iq_capture_data.MaxWaitCnt != 0) )
			{
			  DALSYS_BusyWait(500); // 1ms resolution
			  ftm_tds_iq_capture_data.MaxWaitCnt -- ;
			}
			
			if(ftm_tds_iq_capture_data.IqCaptureStatus == FTM_TDSCDMA_IQ_CAPTURE_SUCCEED )
			{
  			  if(rx_path == FTM_ENH_INTERNAL_DEVICE_CAL_RXRSB_RES_PRX1 )
			  {

				if (rfm_get_calibration_state())
				{
				   ftm_tdscdma_util_convert_10pcfl_iq_data_format(ftm_tds_iq_capture_data.startAddrPrx,(word *) iq_dst_ptr, (uint32)(sample_size));
				}
				 else
				 {
				   ftm_tdscdma_util_convert_20pcfl_iq_data_format(ftm_tds_iq_capture_data.startAddrPrx,(word *) iq_dst_ptr, (uint32)(sample_size));
				 }
				}
			   else
			   {
				 if (rfm_get_calibration_state())
				 {
				   ftm_tdscdma_util_convert_10pcfl_iq_data_format(ftm_tds_iq_capture_data.startAddrDrx,(word *) iq_dst_ptr, (uint32)(sample_size));
				 }
				 else
				 {
				   ftm_tdscdma_util_convert_20pcfl_iq_data_format(ftm_tds_iq_capture_data.startAddrDrx, (word *) iq_dst_ptr, (uint32)(sample_size));
				 }
			   }
			   api_status = TRUE;
			}
			else
			{
			   api_status = FALSE;
			}
			FTM_MSG_1(FTM_MED,"tds do iq capture, status:%d", ftm_tds_iq_capture_data.IqCaptureStatus);
		  break;		  
	#endif /*FEATURE_TDSCDMA*/
	 	}
	 return api_status;	 
}


/*----------------------------------------------------------------------------*/
/*!
  @brief: ftm_enh_internal_device_cal_save_iq
  
 
  @details:iq capture save to efs
  
  @param 

    output: 
  
  @return
 
*/

boolean ftm_enh_internal_device_cal_save_iq
( 
  ftm_rf_technology_type ftm_rf_tech, 
  uint16 band,
  uint8 rxPath, 
  uint32 *iq_data, 
  int16 sampleSize,  
  uint8 is_rsb_updated
)
{
	EFS_EFILE *iq_fp;
	char iq_file_name[128];
	boolean api_status = FTM_IQ_SUCCESS;
	fs_size_t write_result;
	int file_close_res;
	
    memset(iq_file_name,0,sizeof(iq_file_name));
    
	 switch(ftm_rf_tech)
	 {
		case FTM_RF_TECH_CDMA:
		case FTM_RF_TECH_GSM:
		default: 
		  // not supported yet
		  break;  
		   
		case FTM_RF_TECH_WCDMA:
            if(is_rsb_updated == 2)
            {
				snprintf(&iq_file_name[0], 128, "rxIQ_wcdma_band_%d_path_%d_rsbcomped2.bin",band,rxPath );

            }
			else if(is_rsb_updated == 1)
            {
				snprintf(&iq_file_name[0], 128, "rxIQ_wcdma_band_%d_path_%d_rsbcomped.bin",band,rxPath );
            }
			else
			{
				snprintf(&iq_file_name[0], 128, "rxIQ_wcdma_band_%d_path_%d.bin",band,rxPath);
			}	
		  break;
	
		case FTM_RF_TECH_LTE:

		    if(is_rsb_updated==2)
            {
				snprintf(&iq_file_name[0], 128, "rxIQ_lte_band_%d_path_%d_rsbcomped2.bin",band, rxPath);
             }
			else if(is_rsb_updated==1)
            {
				snprintf(&iq_file_name[0], 128, "rxIQ_lte_band_%d_path_%d_rsbcomped.bin", band,rxPath);
             }
			else
			{
				snprintf(&iq_file_name[0], 128, "rxIQ_lte_band_%d_path_%d.bin", band,rxPath);
			}

		  break;
	
		case FTM_RF_TECH_TDSCDMA:
		    if(is_rsb_updated == 2)
            {
				snprintf(&iq_file_name[0], 128, "rxIQ_tdscdma_band_%d_path_%d_rsbcomped2.bin", band,rxPath);
             }
			else if(is_rsb_updated == 1)
            {
				snprintf(&iq_file_name[0], 128, "rxIQ_tdscdma_band_%d_path_%d_rsbcomped.bin", band,rxPath);
             }
			else
			{
				snprintf(&iq_file_name[0], 128, "rxIQ_tdscdma_band_%d_path_%d.bin", band,rxPath);
			}			
		 break;
	}
	iq_fp = efs_efopen( &iq_file_name[0], "w" );
	if(iq_fp !=NULL)
	{
			/* Save samples as they appear in lmem. */
		write_result = efs_efwrite( &iq_data[0], 
									sizeof(uint32),
									sampleSize, iq_fp );	
		if( write_result == 0 )
		{
			api_status = FALSE;
		}
		file_close_res = efs_efclose( iq_fp );
    	if( file_close_res != 0 )
    	{
      		api_status = FALSE;
    	}
	}	
   return api_status;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief: ftm_enh_device_cal_rsb_get_thermistor
  
 
  @details:get thermistor reading
  
  @param 

    output: 
  
  @return
 
*/

boolean ftm_enh_device_cal_rsb_get_thermistor
(
  rfm_device_enum_type device, 
  uint16 band, 
  uint16* enh_therm_val
)
{
	boolean ret_val = FALSE;

   ftm_rf_technology_type ftm_rf_tech = ftm_get_rf_technology();
  
   switch (ftm_rf_tech)
   {
  #ifdef FEATURE_WCDMA    	
      case FTM_RF_TECH_WCDMA:
		ret_val = rfdevice_wcdma_tx_cmd_dispatch(device,
							 ftm_curr_wcdma_mode,
							 RFDEVICE_GET_THERMISTER_VALUE, 
							 enh_therm_val);

		break;
  #endif /*FEATURE_WCDMA*/

  #ifdef  FEATURE_LTE
      case FTM_RF_TECH_LTE:
	  	
		ret_val = rfdevice_lte_tx_cmd_dispatch(device, 
                                                       band, 
                                                       RFDEVICE_LTE_GET_THERMISTER_VALUE,
                                                       enh_therm_val,
                                                       NULL);
		break;
  #endif  /*FEATURE_LTE*/

  #ifdef FEATURE_TDSCDMA
	  case FTM_RF_TECH_TDSCDMA:
	  	ret_val = rfdevice_tdscdma_tx_cmd_dispatch(RFM_DEVICE_0,
                                     RFCOM_TDSCDMA_MODE,
                                     band,
                                     RFDEVICE_GET_THERMISTER_VALUE,
                                     enh_therm_val);
		break;
  #endif /*FEATURE_TDSCDMA*/

	  case FTM_RF_TECH_CDMA: //add it later
	  case FTM_RF_TECH_GSM:// add it later
	  case FTM_RF_TECH_UNKNOWN:
	  default:
	  	enh_therm_val =0;
	    break;
   	}
   return ret_val;

}

/*----------------------------------------------------------------------------*/
/*!
  @brief: get_rxlm_handler
  
 
  @details:get_rxlm_handler as well as rflm_tech_id, which is different from ftm_tech_enum
  
  @param 

    output: 
  
  @return
 
*/
	

boolean get_rxlm_handler
(
  ftm_rf_technology_type ftm_rf_tech,
  rflm_handle_rx_t *rxlm_buffer_idx, 
  rflm_tech_id_t *rflm_tech_id
)
{
	boolean api_status = FALSE;
	switch(ftm_rf_tech)
	{
        #ifdef FEATURE_WCDMA
		case FTM_RF_TECH_WCDMA:
			rxlm_buffer_idx[0] = ftm_wcdma_rxlm_buffer[RFM_DEVICE_0].buf_idx;
			rxlm_buffer_idx[1] = ftm_wcdma_rxlm_buffer[RFM_DEVICE_1].buf_idx;
			*rflm_tech_id = RFLM_TECH_WCDMA;
		//	rxlm_buffer_idx[2] = ftm_wcdma_rxlm_buffer[RFM_DEVICE_2].buf_idx;
		//	rxlm_buffer_idx[3] = ftm_wcdma_rxlm_buffer[RFM_DEVICE_3].buf_idx;			
			api_status = TRUE;
			break;
        #endif /*FEATURE_WCDMA*/
		
#ifdef FEATURE_LTE 
		case FTM_RF_TECH_LTE:
			rxlm_buffer_idx[0] = rflte_ftm_mc_sm.carrier[0].rxlm_prx_buf_idx;
			rxlm_buffer_idx[1] = rflte_ftm_mc_sm.carrier[0].rxlm_drx_buf_idx;
			rxlm_buffer_idx[2] = rflte_ftm_mc_sm.carrier[1].rxlm_prx_buf_idx;
			rxlm_buffer_idx[3] = rflte_ftm_mc_sm.carrier[1].rxlm_drx_buf_idx;
			rxlm_buffer_idx[4] = rflte_ftm_mc_sm.carrier[2].rxlm_prx_buf_idx;
			rxlm_buffer_idx[5] = rflte_ftm_mc_sm.carrier[2].rxlm_drx_buf_idx;			
			*rflm_tech_id = RFLM_TECH_LTE;
			api_status = TRUE;
			break;
#endif /*FEATURE_LTE*/ 
	
     #ifdef FEATURE_TDSCDMA
		case FTM_RF_TECH_TDSCDMA:
			rxlm_buffer_idx[0] = ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_0].buf_idx;
			rxlm_buffer_idx[1] = ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_1].buf_idx;
			*rflm_tech_id = RFLM_TECH_TDSCDMA;
			api_status = TRUE;
			break;
     #endif /*FEATURE_TDSCDMA*/	
	
		case FTM_RF_TECH_CDMA: //add it late
		case FTM_RF_TECH_GSM:// add it later
		case FTM_RF_TECH_UNKNOWN:
		default:
			api_status = FALSE;
			rxlm_buffer_idx[0] = 0;
			rxlm_buffer_idx[1] = 0;
			*rflm_tech_id = RFLM_TECH_1X;
			break;

	}
	
	FTM_MSG_4(FTM_MED, "rxlm prx buf:0x%x, drx buf id:0x%x, carrier 1: prx buf: 0x%x, drx buf id: 0x%x", rxlm_buffer_idx[0],rxlm_buffer_idx[1],
		rxlm_buffer_idx[2],rxlm_buffer_idx[3]);
	return api_status;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief: get_fbrx_lm_handler
  
 
  @details:get_rxlm_handler as well as rflm_tech_id, which is different from ftm_tech_enum
  
  @param 

    output: 
  
  @return
 
*/
boolean get_fbrx_lm_handler
(
  ftm_rf_technology_type ftm_rf_tech,
  rflm_handle_rx_t *fbrx_buffer_idx, 
  rflm_tech_id_t *rflm_tech_id
)
{
	boolean api_status = FALSE;
	switch(ftm_rf_tech)
	{
        #ifdef FEATURE_WCDMA
	case FTM_RF_TECH_WCDMA:
			fbrx_buffer_idx[0] = rfcommon_fbrx_mc_get_tech_handle( RFM_IMT_MODE );
			fbrx_buffer_idx[1] = fbrx_buffer_idx[0];
			*rflm_tech_id = RFLM_TECH_WCDMA;
			api_status = TRUE;
			break;
         #endif
         #ifdef  FEATURE_LTE
		case FTM_RF_TECH_LTE:
			fbrx_buffer_idx[0] = rflte_state_get_fbrxlm_buffer_idx( 0 );			
			fbrx_buffer_idx[1] = rflte_state_get_fbrxlm_buffer_idx( 1 );			
			*rflm_tech_id = RFLM_TECH_LTE;
			api_status = TRUE;
			break;
         #endif

		case FTM_RF_TECH_TDSCDMA:
			fbrx_buffer_idx[0] = rfcommon_fbrx_mc_get_tech_handle( RFM_TDSCDMA_MODE );
			fbrx_buffer_idx[1] = fbrx_buffer_idx[0];
			*rflm_tech_id = RFLM_TECH_TDSCDMA;
			api_status = TRUE;
			break;
	
		case FTM_RF_TECH_CDMA: //add it late
		case FTM_RF_TECH_GSM:// add it later
		case FTM_RF_TECH_UNKNOWN:
		default:
			api_status = FALSE;
			fbrx_buffer_idx[0] = -1;
			fbrx_buffer_idx[1] = -1;
			*rflm_tech_id = RFLM_TECH_1X;
			break;

	}
	
	FTM_MSG_2(FTM_MED, "fbrx lm buf_id:0x%x, carrier 1 buf id:0x%x", fbrx_buffer_idx[0],fbrx_buffer_idx[1]);
	return api_status;
}

ftm_enh_internal_device_cal_error_code_type ftm_enh_internal_device_cal_get_stg_freq
(
  rfdevice_rxtx_common_class * device_ptr,
  int32 stg_offset_khz,
  uint8 debug_mode_en,
  uint64 * stg_freq
)
{
  ftm_enh_internal_device_cal_error_code_type ret_val = FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS;
  ftm_rf_technology_type ftm_rf_tech  = FTM_RF_TECH_UNKNOWN;
  uint32 tx_freq = 0;
  
  #ifdef FEATURE_CDMA
  const rf_cdma_data_status_type *dev_status_r;
  rfm_cdma_band_class_type band;
  #endif

  /* Get Current FTM RF Tech in order to get current Rx Freq */
  ftm_rf_tech = ftm_get_rf_technology();

  /* check for null pointer and throw error */
  if (device_ptr == NULL)
  {
    FTM_MSG(FTM_ERROR, "rfc_common_get_cmn_device_object(0) returns NULL");
    return FTM_ENH_INTERNAL_DEVICE_CAL_FBRX_DEV_ACTION_FAILED;
  }

  /****************************************************************/
  /* Get STG Frequency to tune to from Tx channel information     */
  /****************************************************************/

  switch ( ftm_rf_tech )
  {
  #if defined(FEATURE_CDMA1X) && defined(FEATURE_CDMA) 
  case FTM_RF_TECH_CDMA:
    dev_status_r = rf_cdma_get_device_status( RFM_DEVICE_0 ); // what is the current tx device?
    if( dev_status_r != NULL)
    {
      band = rf_cdma_sys_band_type_to_rf_band_type( (sys_band_class_e_type)dev_status_r->curr_band);
      tx_freq = rf_cdma_get_tx_carrier_freq( band, (int)
                                             dev_status_r->lo_tuned_to_chan );
    }
    else
    {
      FTM_MSG(FTM_ERROR, "rfc_common_get_cmn_device_object(0) returns NULL");
      return FTM_ENH_INTERNAL_DEVICE_CAL_GENERAL_FAILURE;
    }
    break;
  #endif

  #ifdef FEATURE_WCDMA
  case FTM_RF_TECH_WCDMA:
    tx_freq = rfwcdma_core_get_freq_from_ul_arfcn(rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_chan_tx);
    FTM_MSG_1(FTM_HIGH, "ftm_enh_internal_device_cal_get_stg_rxfreq: curr_rx_chan: %d", tx_freq);
    break;
  #endif /* FEATURE_WCDMA */
  #ifdef FEATURE_LTE
  case FTM_RF_TECH_LTE:
    tx_freq = rflte_core_get_tx_freq_from_uarfcn(
                                   rflte_ftm_mc_sm_get_tx_chan(),
                                   rflte_ftm_mc_sm_get_band());
    break;
  #endif
  #ifdef FEATURE_TDSCDMA
  case FTM_RF_TECH_TDSCDMA:
  	tx_freq = rf_tdscdma_core_util_get_freq_from_chan(rf_tdscdma_mc_state.curr_chan_tx);
    break;
  #endif
  default:
    FTM_MSG(FTM_ERROR, "Unsupported Tech Type: Return error handle");
    return FALSE;
    /* Flag Error as Unsupported Tech */
  }
  
  *stg_freq = (uint64) ((tx_freq + stg_offset_khz)*1000);
  return ret_val;

}

ftm_enh_internal_device_cal_error_code_type ftm_enh_internal_device_cal_rsb_stg_on
(
  rfdevice_rxtx_common_class * device_ptr,
  int32 stg_offset_khz,
  uint8 debug_mode_en,
  uint64 * stg_freq_hz,
  rfm_mode_enum_type mode,
  rfcom_band_type_u band,
  rfcom_bw_type_u bw,
  uint8 fbrx_gain_state
)
{
	ftm_enh_internal_device_cal_error_code_type ret_val = FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS;
	boolean api_status = FALSE;
	uint64 stg_freq = 0;
	ret_val = ftm_enh_internal_device_cal_get_stg_freq(device_ptr, stg_offset_khz, debug_mode_en, &stg_freq);
	if(ret_val !=  FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS)
	{
		return ret_val;
	}
	api_status = rfdevice_cmn_enable_stg(
								   device_ptr,
								   stg_freq, 
								   RFDEVICE_EXECUTE_IMMEDIATE,
								   NULL,
                                   mode,
                                   band,
                                   bw,
                                   fbrx_gain_state
								  );
    if (api_status == FALSE)
    {
      FTM_MSG(FTM_ERROR, "rfdevice_cmn_enable_stg returned FALSE ");
      rfcommon_core_xpt_free_all_buffers();  
      return FTM_ENH_INTERNAL_DEVICE_CAL_FBRX_DEV_ACTION_FAILED;
    }

    *stg_freq_hz = stg_freq;
    /* Allow settling time for STG?? */
	return ret_val;
}


ftm_enh_internal_device_cal_error_code_type ftm_enh_internal_device_cal_fbrx_rsb_iq_capture
(
	rfdevice_rxtx_common_class * device_ptr,
	uint8 rsb_cal_alg,
	int32 stg_offset_khz,
	uint16 num_iq_samples,
	uint8 debug_mode_en,
	uint32 * IQSamples,
  uint32 * dcCompVals,
  rfcom_band_type_u band_union,
  rfcom_bw_type_u rfcom_bw,
  uint8 fbrx_gain_state
)
{
  ftm_enh_internal_device_cal_error_code_type ret_val = FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS;
  boolean api_status = TRUE;
  //rfdevice_rxtx_common_class *device_ptr = NULL;

  ftm_rf_technology_type ftm_rf_tech  = FTM_RF_TECH_UNKNOWN;
  rfm_mode_enum_type rfm_mode = RFM_INVALID_MODE;
  uint8 samp_buf_id = 0;
  
  ftm_cal_xpt_instr_payload_iq_capture_type iq_cap_instr;
  char filename[20] = {'\0'};
 
  uint8 iter = 0;
  uint64 stg_freq_hz = 0;
  
  /* Reset Capture Buffers */
  rfcommon_core_xpt_init_buffers(); // must be done before IQ capture code is called - Mark in Documentation for IQ Capture

  /* Get Current FTM RF Tech in order to get current Tx Freq 
     and also to dispatch tech-specific IQ capture command */
  ftm_rf_tech = ftm_get_rf_technology();
  rfm_mode = ftm_common_ftm_tech_to_rfm_mode(ftm_rf_tech);

  if (rsb_cal_alg == FTM_ENH_INTERNAL_DEVICE_CAL_RSB_ALG_STG )
  {
    ftm_enh_internal_device_cal_rsb_stg_on (device_ptr,stg_offset_khz,debug_mode_en,&stg_freq_hz,rfm_mode,band_union,rfcom_bw,fbrx_gain_state);
	FTM_MSG(FTM_MED,"fbrx rsb cal, stg turned on");
  }
	 
  // check if there is going to be too many captures to save on the EFS
  iq_cap_instr.primary_device = RFM_DEVICE_0; /* What is the tx device, doesn't matter */
  iq_cap_instr.feedback_device = RFM_DEVICE_0; /* What is the fbrx device */
  iq_cap_instr.num_samples = num_iq_samples;
  iq_cap_instr.proc_flag = PROC_NONE;
  iq_cap_instr.capture_type = XPT_CAPTURE_FBRX;
  iq_cap_instr.first_trigger = 1;
  iq_cap_instr.last_trigger = 1;


  snprintf(&filename[0],20, "iq_%05d_khz.bin", (int)stg_offset_khz);

    /****************************************************************/
    /*              Setup and Trigger XPT IQ Capture                */
    /****************************************************************/

  ret_val = ftm_enh_internal_device_cal_fbrx_iq_capture(ftm_rf_tech, &iq_cap_instr, &samp_buf_id, debug_mode_en, &filename[0]);
  if(rsb_cal_alg == FTM_ENH_INTERNAL_DEVICE_CAL_RSB_ALG_STG )
  {
	  api_status = rfdevice_cmn_disable_stg(
                                      device_ptr,
                                      RFDEVICE_EXECUTE_IMMEDIATE,
                                      NULL,
                                      stg_freq_hz,
                                      rfm_mode,
                                      band_union,
                                      rfcom_bw,
                                      fbrx_gain_state
                                     );
	  if (api_status == FALSE)
      {
        FTM_MSG_1(FTM_ERROR, "Something went wrong on loop #%d, breaking loop, check FTM Msgs", iter );
        rfcommon_core_xpt_free_all_buffers(); 
        return FTM_ENH_INTERNAL_DEVICE_CAL_FBRX_DEV_ACTION_FAILED;
      }
  }
   	 
  if (ret_val != FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS)
  {
    FTM_MSG_1(FTM_ERROR, "ftm_enh_internal_device_cal_fbrx_iq_capture failed for tech %d", (int)ftm_rf_tech);
	rfcommon_core_xpt_free_all_buffers();
    return ret_val;
  }

  FTM_MSG_1(FTM_HIGH, "Current Capture Buffer is %d", samp_buf_id);

  ret_val = ftm_enh_internal_device_cal_rsb_fetch_iq( samp_buf_id, num_iq_samples,debug_mode_en, IQSamples, dcCompVals);
  
  if (ret_val != FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS)
  {
      FTM_MSG_1(FTM_ERROR, "rfcommon_mdsp_fetch_selftest_iq_data failed with status %d", api_status);
      return FTM_ENH_INTERNAL_DEVICE_CAL_FETCH_SAMPLES_FAILED;
  } 

  rfcommon_core_xpt_free_all_buffers();  // must be done after last IQ capture code is called - Mark in Documentation for IQ Capture

  return ret_val;
}



ftm_enh_internal_device_cal_error_code_type 
ftm_enh_internal_device_cal_rsb_fetch_iq
(
   uint8  samp_buffers,
   uint16 num_samps,
   uint8 debug_mode_en,
   uint32 * IQSamples,
   uint32 * dcCompVals
)
{
  ftm_enh_internal_device_cal_data_type *internal_device_cal_data;
  
  uint16 iter = 0;
  boolean ret_val = FALSE;
  uint32 rx_samp_pcfl20;
  uint8 exponent;
  int32 rxI;
  int32 rxQ;
  int16 rxI_FP16 = 0;
  int16 rxQ_FP16 = 0;
  int32 * rx_samps;
  int32 * tx_samps;
  uint8 overflow_det;
  int32 dc_I = 0;
  int32 dc_Q = 0;
  int16 dc_I_FP16 = 0;
  int16 dc_Q_FP16 = 0;



  internal_device_cal_data = ftm_common_data_get_enh_internal_device_cal_ptr();

  debug_mode_en = internal_device_cal_data->internal_device_cal_config.dbg_mode_en;
//  fbrx_bw = internal_device_cal_data->internal_device_cal_config.bw;


  ret_val = rfcommon_mdsp_fetch_selftest_iq_data(samp_buffers, num_samps, &rx_samps, &tx_samps);

  for (iter = 0; iter < num_samps; iter++)
  { 
      if (iter%8 == 0)
      {
        rx_samp_pcfl20 = (((uint32)*rx_samps) >> 0) & 0xFFFFF;
      }
      else if (iter%8 == 1)
      {
        rx_samp_pcfl20 = ( ( ((uint32)*rx_samps) >> 20 )  +  ( ( (uint32)*(rx_samps + 1) ) << 12 )) & 0xFFFFF ;
      }
      else if (iter%8 == 2)
      {
        rx_samp_pcfl20 = ( ((uint32)*(rx_samps+1) ) >> 8) & 0xFFFFF;
      }
      else if (iter%8 == 3)
      {
        rx_samp_pcfl20 = ( ( ( (uint32)*( rx_samps + 1 ) ) >> 28 ) + ( ( (uint32) *( rx_samps + 2 ) ) << 4 ) ) & 0xFFFFF;
      }
      else if (iter%8 == 4)
      {
        rx_samp_pcfl20 = ( ( ( (uint32) *( rx_samps + 2 ) ) >> 16 ) + (( (uint32) *( rx_samps + 3 ) ) << 16 ) ) & 0xFFFFF;
      }
      else if (iter%8 == 5)
      {
        rx_samp_pcfl20 = (((uint32)*(rx_samps+3)) >> 4) & 0xFFFFF;
      }
      else if (iter%8 == 6)
      {
        rx_samp_pcfl20 = ( ( ((uint32) *( rx_samps + 3 ) ) >> 24 ) + ( ( (uint32) *( rx_samps + 4 ) ) << 8 )) & 0xFFFFF;
      }
      else if (iter%8 == 7)
      {
        rx_samp_pcfl20 = (((uint32)*(rx_samps+4)) >> 12) & 0xFFFFF;
        rx_samps = rx_samps + 5;
      }

      exponent = (uint8)((rx_samp_pcfl20>>16) & 0xFF);
      rxI = (int32)(((int8)((rx_samp_pcfl20>>8) & 0xFF)) << (15-exponent)); 
      rxQ = (int32)(((int8)((rx_samp_pcfl20>>0) & 0xFF)) << (15-exponent)); 

      rxI = rxI>>6;
      rxQ = rxQ>>6;

      rxI_FP16 = sat_int32_to_int16(rxI, &overflow_det);
      if (overflow_det == 1)
      {
        return FTM_ENH_INTERNAL_DEVICE_CAL_ARITHMETIC_OVERFLOW;
      }
      rxQ_FP16 = sat_int32_to_int16(rxQ, &overflow_det);
      if (overflow_det == 1)
      {
        return FTM_ENH_INTERNAL_DEVICE_CAL_ARITHMETIC_OVERFLOW;
      }
      dc_I += rxI_FP16;
      dc_Q += rxQ_FP16;

      if ( (iter <32) &  (debug_mode_en != 0))
      {
        FTM_MSG_6(FTM_MED, "samp ind %4d    =  %16d  +  j *  %16d   ,  %16d + j * %16d, overflow_det = %d", iter, rxI, rxQ, rxI_FP16, rxQ_FP16, overflow_det);
      }
      if ( (iter > 9206) &  (debug_mode_en != 0))
      {
        FTM_MSG_6(FTM_MED, "samp ind %4d    =  %16d  +  j *  %16d   ,  %16d + j * %16d, overflow_det = %d", iter, rxI, rxQ, rxI_FP16, rxQ_FP16, overflow_det);
      }
	  IQSamples[iter] = (uint32) ((((uint32)(rxI_FP16<<16)) & 0xFFFF0000) | ((uint32) rxQ_FP16& 0xFFFF));

  }
  dc_I = (int32)(dc_I/(int32)num_samps);
  dc_Q = (int32)(dc_Q/(int32)num_samps);
  dc_I_FP16 = sat_int32_to_int16(dc_I, &overflow_det);
  if (overflow_det == 1)
  {
    return FTM_ENH_INTERNAL_DEVICE_CAL_ARITHMETIC_OVERFLOW;
  }
  dc_Q_FP16 = sat_int32_to_int16(dc_Q, &overflow_det);
  if (overflow_det == 1)
  {
    return FTM_ENH_INTERNAL_DEVICE_CAL_ARITHMETIC_OVERFLOW;
  }
  *dcCompVals = (uint32) ((((uint32)(dc_I_FP16<<16)) & 0xFFFF0000) | ((uint32) dc_Q_FP16& 0xFFFF));

  if(ret_val == TRUE)
  {
     return FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS;
  }
  else
  {
	return FTM_ENH_INTERNAL_DEVICE_CAL_IQ_CAPTURE_FAILURE;
  }

}




/*!
  @name: ftm_enh_internal_device_cal_rsb

  @brief: rsb calibration
  
 
  @details
  
  @param
  input: req_instr, payload instruction
  output: res_len:  cal result length
             rsb_result: output of feedback droop cal NV
  
  @return
     success or fail
*/

boolean ftm_enh_internal_device_cal_rsb
(
  void const *req_instr,
  uint16 *res_len,
  void *rsp_payload 
)
{
   boolean ret_val = TRUE;
   ftm_enh_internal_device_cal_error_code_type api_status = FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS;
   ftm_rf_technology_type curr_ftm_rf_tech = FTM_RF_TECH_UNKNOWN;

   ftm_enh_internal_device_cal_instr_payload_rsb_type * rsb_instr = (ftm_enh_internal_device_cal_instr_payload_rsb_type *) req_instr;
   uint8 rsb_type = rsb_instr->rsb_type;
   uint8 rsb_cal_alg = rsb_instr->rsb_cal_alg;

  // uint8 iqCaptureType = rsb_instr->iq_capture_type;   // not used for now, reserve it for future usuage
   uint32 sample_size = rsb_instr->iq_sample_size;
   
   ftm_enh_internal_device_cal_rsb_result_type * rsb_result = (ftm_enh_internal_device_cal_rsb_result_type *) rsp_payload;
 
   ftm_enh_internal_device_cal_data_type *internal_device_cal_data = ftm_common_data_get_enh_internal_device_cal_ptr();
   uint8 * rsp_payload_validation_addr = internal_device_cal_data->internal_device_cal_result.rsb_calver_buff_ptr;
   uint8 tech = internal_device_cal_data->internal_device_cal_config.tech;
   uint16 band = internal_device_cal_data->internal_device_cal_config.band;
   uint8  bw = internal_device_cal_data->internal_device_cal_config.bw;     
   uint16 device_mask = internal_device_cal_data->internal_device_cal_config.device_mask;
   int32  stg_offset_khz = rsb_instr->offset;  
   uint8 dbg_mode_en = internal_device_cal_data->internal_device_cal_config.dbg_mode_en;
   uint8 chain_mask = 0;  
   uint8 carrier = rsb_instr->carrier;
   uint8 rx_path[2] = {FTM_ENH_INTERNAL_DEVICE_CAL_RXRSB_RES_PRX1,FTM_ENH_INTERNAL_DEVICE_CAL_RXRSB_RES_DRX1};	
   uint8 rx_path_num = 0;

   rflm_handle_rx_t rxlm_buffer_idx[6]={0,0,0,0,0,0};
   rflm_handle_tx_t fbrx_buffer_idx[4]={0,0,0,0};
   rflm_tech_id_t rflm_tech_id; 
   
   uint32 *iq_buf_ptr = NULL;
  
   int32 rsb_a_coeff, rsb_b_coeff;  
   rfdevice_rsb_cal_data_type rsb_data = {0,0,0,0,0,0,0} ;
   uint16 therm_data =0; 
   
   uint8 rsb_res_dev_index = 0;
   uint8 rsb_res_gain_index = 0;

   ftm_common_msm_iq_cap_info iq_cap_struct_dc_comped;
   
   int id =0; //for loop id
   uint8 rx_path_idx = 0;
   uint32 dcCompVals = 0; // for FBRx RSB computation with DC removal
   uint8 cur_gain_state = rsb_instr->gain_states[0];

   rfdevice_rxtx_common_class* device_ptr = NULL;
   rfcommon_msm_num_carr_bw msm_carr_bw;
   rf_time_tick_type start_tick,start_tick0;
   uint16 flag = internal_device_cal_data->internal_device_cal_config.flag;   // flag for efs write request 
   rfdevice_rsb_cal_verfication_data_type rsb_ver_data;
   uint16 max_cal_elements = 0;
   ftm_enh_internal_device_cal_rsb_save_efs_fp ftm_rsb_efs_save;

   #ifdef FEATURE_CDMA
   const rf_cdma_data_status_type *dev_status_r;
   #endif

   rfcom_band_type_u band_union;
   rfcom_bw_type_u rfcom_bw;
   #ifdef FEATURE_LTE
   rfcom_bw.lte_bw = 3; 
   #endif

   *res_len = 0;
   
   rsb_ver_data.cal_data = NULL;
   rsb_ver_data.num_calibrated_item = 0;
   rsb_ver_data.num_elements = 0;
   
   
  // uint8 rsb_save_efs_func_index = 0;

   ftm_rsb_efs_save = rsb_efs_empty_write;
   max_cal_elements = 0;
   // assign efs write function pointer based on tech 
 
   if(rsb_type == FTM_ENH_INTERNAL_DEVICE_CAL_FB_RSB)
   {
   	ftm_rsb_efs_save = rsb_efs_empty_write;
	max_cal_elements = 0;
   }
   else
   {
     switch (tech)
     {
#ifdef FEATURE_LTE 
      case FTM_RF_TECH_LTE:
        ftm_rsb_efs_save = rfdevice_lte_rx_commit_rsb_data_to_efs;
		max_cal_elements = MAX_RSB_CAL_ELEMENT_LTE;
		break;
#endif /*FEATURE_LTE*/ 
#ifdef FEATURE_WCDMA
	  case FTM_RF_TECH_WCDMA:
		ftm_rsb_efs_save = rfdevice_wcdma_rx_commit_rsb_data_to_efs; 
		max_cal_elements = MAX_RSB_CAL_ELEMENT_WCDMA;
#endif /*FEATURE_WCDMA*/
		break;

	  case FTM_RF_TECH_TDSCDMA:
		  ftm_rsb_efs_save = rsb_efs_empty_write;
		  max_cal_elements = 0; //MAX_RSB_CAL_ELEMENT_TDSCDMA; //tds not supported yet in rfdevice api
		break;
      default:
		  FTM_MSG(FTM_ERROR, "ftm_enh_internal_device_cal_rsb() failure, requested tech not supported!");
		  internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_UNSUPPORTED_TECH ;
		  internal_device_cal_data->internal_device_cal_status.error_mask = (1<< ENH_INTERNAL_DEVICE_CAL_RSB_BIT);
		  return FALSE;
		break;
   	  }
   }

   band = ftm_calv4_convert_band(tech, band);

   start_tick0 = rf_time_get_tick();

   internal_device_cal_data->internal_device_cal_status.error_log  = FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS;
   // init results as default, no rsb comp
   rsb_result->device_mask = 0;
   rsb_result->num_gain_states = rsb_instr->num_gain_states;
   internal_device_cal_data->internal_device_cal_status.error_mask = 0;   

   msm_carr_bw.bw = (rfcom_lte_bw_type) bw;
   msm_carr_bw.bw_kHz = (uint32) bw;
   msm_carr_bw.num_carr = (rfcom_multi_carrier_hspa_id_type) bw;
   
   FTM_MSG_7(FTM_MED,"enh_internal_device_cal:tech: %d, device_mask%d, rsb type %d, stg_offset: %d, alg: %d, carrier:%d, sampleSize: %d",tech, device_mask, rsb_type, stg_offset_khz,
		   rsb_cal_alg, carrier,sample_size);

   FTM_MSG_5(FTM_MED,"enh_internal_device_cal: num_gain:%d, gain states: %d, %d, %d, %d",rsb_instr->num_gain_states, rsb_instr->gain_states[0],rsb_instr->gain_states[1],
   		rsb_instr->gain_states[2],rsb_instr->gain_states[3]);

   FTM_MSG_3(FTM_MED," band:%d, pcell band for scell: %d, pcell chanl for scell:%d ", band, rsb_instr->pcell_band_for_scell, rsb_instr->pcell_ref_chan_for_scell);
   

   curr_ftm_rf_tech = ftm_get_rf_technology();

   if ( tech != curr_ftm_rf_tech)
   {
	   FTM_MSG_2(FTM_ERROR, "Current RF Tech %d does not match Tech for RxRsb/FbRsb Cal %d", curr_ftm_rf_tech, tech );
	   internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_TECH_MISMATCH;
	   internal_device_cal_data->internal_device_cal_status.error_mask = (1<< ENH_INTERNAL_DEVICE_CAL_RSB_BIT);
	   return FALSE;
   }

   if(flag!=3) // if flag=3, write efs only
   {
     if(dbg_mode_en>=2)
     {
       start_tick = rf_time_get_tick();
     }


   
   for (rsb_res_dev_index = 0; rsb_res_dev_index < FTM_ENH_INTERNAL_DEVICE_CAL_RSB_RES_MAX ; rsb_res_dev_index ++)
   {
      for(rsb_res_gain_index =0; rsb_res_gain_index< rsb_instr->num_gain_states; rsb_res_gain_index++)
      {
        rsb_result->gain_states[rsb_res_gain_index ] = rsb_instr->gain_states[rsb_res_gain_index];
		rsb_result->rsb_res[rsb_res_dev_index][rsb_res_gain_index].nv_active = 0;	
	    rsb_result->rsb_res[rsb_res_dev_index][rsb_res_gain_index].sin_theta_final = 0;
	    rsb_result->rsb_res[rsb_res_dev_index][rsb_res_gain_index].cos_theta_final = 16384;
	    rsb_result->rsb_res[rsb_res_dev_index][rsb_res_gain_index].gain_inv_final = 16384; // check for default value
	    rsb_result->rsb_res[rsb_res_dev_index][rsb_res_gain_index].sin_theta_after_cal = 0;
	    rsb_result->rsb_res[rsb_res_dev_index][rsb_res_gain_index].gain_inv_after_cal = 16384;	  
	    rsb_result->rsb_res[rsb_res_dev_index][rsb_res_gain_index].cos_theta_after_cal = 16384;
	    rsb_result->rsb_res[rsb_res_dev_index][rsb_res_gain_index].rsb_before_cal = 0;
	    rsb_result->rsb_res[rsb_res_dev_index][rsb_res_gain_index].rsb_after_cal = 99;
      }
   }
   

   if((rsb_instr->num_gain_states !=1) && (rsb_type == FTM_ENH_INTERNAL_DEVICE_CAL_RX_RSB)) // for rxrsb cal, we can only support one gain per request to sync with dl power time line) 
   {
	   FTM_MSG_2(FTM_ERROR, "Current Rx Rsb cal only support number of %d gain states, requested number of %d gain states", 1, rsb_instr->num_gain_states );
	   internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_NUM_OF_GAIN_STATES_UNSUPPORTED ;
	   internal_device_cal_data->internal_device_cal_status.error_mask = (1<< ENH_INTERNAL_DEVICE_CAL_RSB_BIT);	   
	   return FALSE;
   }
    if(rsb_instr->num_gain_states >4) // for fbrx rsb cal, we can only support one gain per request to sync with dl power time line) 
   {
	   FTM_MSG_2(FTM_ERROR, "Current Fbrx Rsb cal only support number of %d gain states, requested number of %d gain states", 4, rsb_instr->num_gain_states );
	   internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_NUM_OF_GAIN_STATES_UNSUPPORTED ;
	   internal_device_cal_data->internal_device_cal_status.error_mask = (1<< ENH_INTERNAL_DEVICE_CAL_RSB_BIT);
	   return FALSE;
   }
    // lte rx rsb cal, gain state is less than 5, wcdma: gain state is less than 3 
   if(((rsb_instr->gain_states[0] >5) &&(tech==3)) ||((rsb_instr->gain_states[0] >3) &&(tech==1)))
   {
	 FTM_MSG_1(FTM_ERROR, "Requested gain state %d not supported", rsb_instr->gain_states[0] );
	 internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_GAIN_STATE_UNSUPPORTED ;
	 internal_device_cal_data->internal_device_cal_status.error_mask = (1<< ENH_INTERNAL_DEVICE_CAL_RSB_BIT);
	 return FALSE;
   }

   if( rsb_type == FTM_ENH_INTERNAL_DEVICE_CAL_FB_RSB )
   {
     switch (curr_ftm_rf_tech ) 
     {
       #if defined(FEATURE_CDMA1X) && defined(FEATURE_CDMA) 
       case FTM_RF_TECH_CDMA:
         dev_status_r = rf_cdma_get_device_status( RFM_DEVICE_0 ); // what is the current tx device?
         if( dev_status_r != NULL)
         {
           band_union.cdma_band = dev_status_r->curr_band;
		   device_ptr = rfcommon_core_get_tx_device_ptr(RFM_DEVICE_0, RFCOM_1X_MODE, band_union);
         }
         else
         {
           FTM_MSG(FTM_ERROR, "rfcommon_core_get_tx_device_ptr(0) returns NULL");
         }
         break;
       #endif

       #ifdef FEATURE_WCDMA
       case FTM_RF_TECH_WCDMA:
         band_union.wcdma_band = rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_tx;
         device_ptr = rfcommon_core_get_tx_device_ptr(RFM_DEVICE_0, RFCOM_WCDMA_MODE, band_union);
         break;
       #endif /* FEATURE_WCDMA */
       #ifdef FEATURE_LTE
       case FTM_RF_TECH_LTE:
         band_union.lte_band = rflte_ftm_mc_sm_get_band();
         rfcom_bw.lte_bw = rflte_ftm_mc_sm_get_tx_bw();
         device_ptr = rfcommon_core_get_tx_device_ptr(RFM_DEVICE_0, RFCOM_LTE_MODE, band_union);
         break;
       #endif
       #ifdef FEATURE_TDSCDMA
       case FTM_RF_TECH_TDSCDMA:
         band_union.tdscdma_band = rf_tdscdma_mc_state.curr_band_tx;
         device_ptr = rfcommon_core_get_tx_device_ptr(RFM_DEVICE_0, RFCOM_TDSCDMA_MODE, band_union);
         break;
       #endif
       default:
         FTM_MSG(FTM_ERROR, "ftm_enh_internal_device_cal_rsb Unsupported Tech Type");
     }

	   /* check for null pointer and throw error */
     if (device_ptr == NULL) 
     {
	   FTM_MSG(FTM_ERROR, "rfc_common_get_cmn_device_object(0) returns NULL");
	   internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_FBRX_DEV_ACTION_FAILED;
	   internal_device_cal_data->internal_device_cal_status.error_mask = (1<< ENH_INTERNAL_DEVICE_CAL_RSB_BIT);
	   return FALSE;
     }
   }
	   
   iq_buf_ptr = (uint32 *) modem_mem_alloc( sample_size*sizeof(int32), MODEM_MEM_CLIENT_RFA );
   
   if(iq_buf_ptr == NULL )
   {
	  FTM_MSG(FTM_ERROR, "Enhanced internal device cal: rsb: not able to allocate memory, returns NULL");
	  internal_device_cal_data->internal_device_cal_status.error_log  = FTM_ENH_INTERNAL_DEVICE_CAL_MEM_ALLOC_FAILURE;
	  internal_device_cal_data->internal_device_cal_status.error_mask = (1<< ENH_INTERNAL_DEVICE_CAL_RSB_BIT);
	  return FALSE;
   }
   memset((void *) iq_buf_ptr, 0, sample_size*4    ); /* reset buffer */


   if(rsb_type == FTM_ENH_INTERNAL_DEVICE_CAL_FB_RSB)
   {	   
	   ret_val = rfcommon_fbrx_mc_enable_rf_dev(device_ptr, cur_gain_state);
	   if (ret_val == FALSE)
	   {
		 FTM_MSG_1(FTM_ERROR, "Failed to Enable FBRx Device in Gain State %d " , cur_gain_state);
		 internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_FBRX_DEV_ACTION_FAILED;
		 internal_device_cal_data->internal_device_cal_status.error_mask = (1<< ENH_INTERNAL_DEVICE_CAL_RSB_BIT);
		 if(iq_buf_ptr != NULL)
		 {
		   modem_mem_free((void*)iq_buf_ptr, MODEM_MEM_CLIENT_RFA);
		 }
		 return FALSE;
	   }
       FTM_MSG(FTM_MED,"enabled fbrx_dev");
       api_status = ftm_enh_internal_device_cal_fbrx_iq_cap_config(tech, bw);
       if (api_status != FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS)
       {
         FTM_MSG_1(FTM_ERROR, "ftm_enh_internal_device_cal_fbrx_iq_cap_config failed with error code %d", api_status );		 
         rfcommon_fbrx_mc_disable_rf_dev(device_ptr);
         ftm_enh_internal_device_cal_fbrx_iq_cap_deconfig(tech);
         internal_device_cal_data->internal_device_cal_status.error_log = api_status;
		 if(iq_buf_ptr != NULL)
		 {
		   modem_mem_free((void*)iq_buf_ptr, MODEM_MEM_CLIENT_RFA);
		 }
         return FALSE;
       }
	   
	   FTM_MSG(FTM_MED,"fbrx_iq_cap_configured");
	   get_fbrx_lm_handler(tech,fbrx_buffer_idx,&rflm_tech_id);
   }
   else
   {
     get_rxlm_handler(tech, rxlm_buffer_idx,&rflm_tech_id);
   }
   

   if(dbg_mode_en>=2)
   {
     FTM_MSG_3(FTM_MED,
						   "rsb cal: type(0: rx rsb; 1: fbrx rsb) %d, tech: %d, first time span<decoding ftm instruction>: %d us",
						   rsb_type, tech, rf_time_get_elapsed(start_tick,RF_USEC));  

     start_tick = rf_time_get_tick();
   	}
	  // clear current IQMC data 
   rsb_a_coeff = 0;
   rsb_b_coeff = 0x8000;
   
//#if defined(FEATURE_BOLT_MODEM)
   if(rsb_type == FTM_ENH_INTERNAL_DEVICE_CAL_RX_RSB)
   {    
     if(carrier ==0)
     {
	   rfcommon_rx_msm_update_rsb_coeff(rxlm_buffer_idx[0],rsb_a_coeff,rsb_b_coeff, rflm_tech_id,msm_carr_bw,dbg_mode_en); 
	   rfcommon_rx_msm_update_rsb_coeff(rxlm_buffer_idx[1],rsb_a_coeff,rsb_b_coeff,rflm_tech_id,msm_carr_bw,dbg_mode_en);
     }
	 else if(carrier ==1) 
	 { 
	  rfcommon_rx_msm_update_rsb_coeff(rxlm_buffer_idx[2],rsb_a_coeff,rsb_b_coeff, rflm_tech_id,msm_carr_bw,dbg_mode_en); 
	  rfcommon_rx_msm_update_rsb_coeff(rxlm_buffer_idx[3],rsb_a_coeff,rsb_b_coeff,rflm_tech_id,msm_carr_bw,dbg_mode_en);	  
	 }
	 else if(carrier == 2)
	 {
		 rfcommon_rx_msm_update_rsb_coeff(rxlm_buffer_idx[4],rsb_a_coeff,rsb_b_coeff, rflm_tech_id,msm_carr_bw,dbg_mode_en); 
		 rfcommon_rx_msm_update_rsb_coeff(rxlm_buffer_idx[5],rsb_a_coeff,rsb_b_coeff,rflm_tech_id,msm_carr_bw,dbg_mode_en); 	 

	 }
   }
   else if(rsb_type == FTM_ENH_INTERNAL_DEVICE_CAL_FB_RSB)
   {
	 rfcommon_fbrx_msm_update_rsb_coeff( fbrx_buffer_idx[0], rsb_a_coeff,rsb_b_coeff, rflm_tech_id, 0); 
   }
   DALSYS_BusyWait(2500);  // give enough time to let iqmc register reset, clear iqmc done
//#endif
   if(dbg_mode_en>=2)
   {
     FTM_MSG_3(FTM_MED,
						   "rsb cal: type(0: rx rsb; 1: fbrx rsb) %d, tech: %d, second time span <clear iqmc>: %d us",
						   rsb_type, tech, rf_time_get_elapsed(start_tick,RF_USEC));    
   
     start_tick = rf_time_get_tick();
   }
   
   rx_path_num = 1;
   if(rsb_type == FTM_ENH_INTERNAL_DEVICE_CAL_RX_RSB)
   {
	 if(carrier ==0)
	 {
	   chain_mask = (device_mask & 0x03);
	   rx_path[0] = FTM_ENH_INTERNAL_DEVICE_CAL_RXRSB_RES_PRX1;
	   rx_path[1] = FTM_ENH_INTERNAL_DEVICE_CAL_RXRSB_RES_DRX1;
	 }
	 else if(carrier == 1 )
	 {
	   chain_mask =  ((device_mask >>2) & 0x03); 
	   if(chain_mask == 0)
	   {
	   	 FTM_MSG(FTM_ERROR, "requested dev cal doesn't match to carrier 1");
	     internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_CARRIER_NOT_MATCH_DEV_ID;
	     internal_device_cal_data->internal_device_cal_status.error_mask = (1<< ENH_INTERNAL_DEVICE_CAL_RSB_BIT);
		 if(iq_buf_ptr != NULL)
		 {
		   modem_mem_free((void*)iq_buf_ptr, MODEM_MEM_CLIENT_RFA);
		 }
         return FALSE;		 
	   }
	   rx_path[0] = FTM_ENH_INTERNAL_DEVICE_CAL_RXRSB_RES_PRX2;
	   rx_path[1] = FTM_ENH_INTERNAL_DEVICE_CAL_RXRSB_RES_DRX2;
	 }
	 else if(carrier ==2)
	 {
        chain_mask = ((device_mask >>4) & 0x03);
		if(chain_mask == 0 )
		{
		  FTM_MSG(FTM_ERROR, "requested dev cal doesn't match to carrier 2");
		  internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_CARRIER_NOT_MATCH_DEV_ID;
		  internal_device_cal_data->internal_device_cal_status.error_mask = (1<< ENH_INTERNAL_DEVICE_CAL_RSB_BIT);
		  if(iq_buf_ptr != NULL)
		  {
		    modem_mem_free((void*)iq_buf_ptr, MODEM_MEM_CLIENT_RFA);
		  }
                return FALSE;			
		}
		
		rx_path[0] = FTM_ENH_INTERNAL_DEVICE_CAL_RXRSB_RES_PRX3;
		rx_path[1] = FTM_ENH_INTERNAL_DEVICE_CAL_RXRSB_RES_DRX3;
	 }
	 if(chain_mask == 0x03)
	 {
	   rx_path_num = 2;
	 }         
   }
   else if (rsb_type == FTM_ENH_INTERNAL_DEVICE_CAL_FB_RSB)
   {
	 rx_path_num = 1;		  		 
	 rx_path[0] = FTM_ENH_INTERNAL_DEVICE_CAL_RSB_RES_FBRX;
	 rx_path[1] = FTM_ENH_INTERNAL_DEVICE_CAL_RSB_RES_MAX;
   }

   for ( rsb_res_gain_index = 0; rsb_res_gain_index < rsb_instr->num_gain_states; rsb_res_gain_index++)
   {
//#if defined(FEATURE_BOLT_MODEM)

     if( rsb_res_gain_index !=0)
     {
        if(rsb_type == FTM_ENH_INTERNAL_DEVICE_CAL_FB_RSB)
        {			
		  cur_gain_state = rsb_instr->gain_states[rsb_res_gain_index];
		  ret_val = rfcommon_fbrx_mc_enable_rf_dev(device_ptr, cur_gain_state );
	      if (ret_val == FALSE)
	      {
		    FTM_MSG_1(FTM_ERROR, "Failed to Enable FBRx Device in Gain State %d " , cur_gain_state);
		     internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_FBRX_DEV_ACTION_FAILED;
			 internal_device_cal_data->internal_device_cal_status.error_mask = (1<< ENH_INTERNAL_DEVICE_CAL_RSB_BIT);
		    if(iq_buf_ptr != NULL)
		    {
		      modem_mem_free((void*)iq_buf_ptr, MODEM_MEM_CLIENT_RFA);
		    }
		    return FALSE;
	      }
		  FTM_MSG(FTM_MED,"enabled fbrx_dev");
          api_status = ftm_enh_internal_device_cal_fbrx_iq_cap_config(tech, bw);
          if (api_status != FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS)
          {
            FTM_MSG_1(FTM_ERROR, "ftm_enh_internal_device_cal_fbrx_iq_cap_config failed with error code %d", api_status );		 
 
			if(iq_buf_ptr != NULL)
			{
			  modem_mem_free((void*)iq_buf_ptr, MODEM_MEM_CLIENT_RFA);
			}
			rfcommon_fbrx_mc_disable_rf_dev(device_ptr);
            ftm_enh_internal_device_cal_fbrx_iq_cap_deconfig(tech);
            internal_device_cal_data->internal_device_cal_status.error_log = api_status;
			internal_device_cal_data->internal_device_cal_status.error_mask = (1<< ENH_INTERNAL_DEVICE_CAL_RSB_BIT);
            return FALSE;
          }
	   
	      FTM_MSG(FTM_MED,"fbrx_iq_cap_configured");
		  rfcommon_fbrx_msm_update_rsb_coeff( fbrx_buffer_idx[0], rsb_a_coeff,rsb_b_coeff, rflm_tech_id, 0); 
        }
	    else if(rsb_type == FTM_ENH_INTERNAL_DEVICE_CAL_RX_RSB)
        {    
          // we put it here, but if we are using box as src for rxRSB cal, there is no way to do multiple gain_state as time agline mismatch between box DL power and lna_gain_state set
          // do nothing now, unless we will support stg based rx rsb cal later
        }
		DALSYS_BusyWait(2500);	// give enough time to let iqmc register reset, clear iqmc done
		if(dbg_mode_en>=2)
		{
		  FTM_MSG_3(FTM_MED,
						   "rsb cal: type(0: rx rsb; 1: fbrx rsb) %d, gain_state: %d, time span <clear iqmc>: %d us",
						   rsb_type, cur_gain_state, rf_time_get_elapsed(start_tick,RF_USEC));    
          start_tick = rf_time_get_tick();
		}
        
	 }
//#endif	 
 	 for (rx_path_idx = 0; rx_path_idx < rx_path_num; rx_path_idx++)
 	 {
		if(chain_mask == 0x02)
 	    {
          rx_path_idx = 1;
 	    }		
 	 	memset(iq_buf_ptr, 0,sample_size*4); // reset iq data to be zero
 	 	
	    if(rsb_type == FTM_ENH_INTERNAL_DEVICE_CAL_RX_RSB)
	    {
			ret_val = ftm_enh_internal_device_cal_rx_iq_capture(tech, carrier, rx_path[rx_path_idx], sample_size,iq_buf_ptr);
		    if(ret_val == FALSE)
		    {
				FTM_MSG_2(FTM_MED,"rsb_cal failed on IQ capture, tech:%d, band:%d",tech, band);
				internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_IQ_CAPTURE_FAILURE;
				internal_device_cal_data->internal_device_cal_status.error_mask = (1<< ENH_INTERNAL_DEVICE_CAL_RSB_BIT);
  	            rsb_result->device_mask = 0;	
				if(iq_buf_ptr != NULL)
				{
				  modem_mem_free((void*)iq_buf_ptr, MODEM_MEM_CLIENT_RFA);
				}

				return FALSE;
		    }
			if(dbg_mode_en>=2)
			{
			  FTM_MSG_3(FTM_MED,
							   "rsb cal: type(0: rx rsb; 1: fbrx rsb) %d, tech: %d, time span <rx IQ capture>: %d us",
							   rsb_type, tech, rf_time_get_elapsed(start_tick,RF_USEC));	  
			  start_tick = rf_time_get_tick();
			}
			

			if(tech == FTM_RF_TECH_WCDMA )
			{
				ftm_convert_iq_8b_to_16b( &iq_buf_ptr[0], 0, sample_size);
			}
			else if( (tech == FTM_RF_TECH_LTE )||(tech == FTM_RF_TECH_TDSCDMA ) )
			{
				ftm_convert_rsb_iq_format(&iq_buf_ptr[0], 1,FTM_RX_RSB_IQ_16bit_to_16bit,sample_size);		
			}
			if(dbg_mode_en>=2)
			{
			  FTM_MSG_3(FTM_MED,
							   "rsb cal: type(0: rx rsb; 1: fbrx rsb) %d, tech: %d, time span <convert IQ data format>: %d us",
							   rsb_type, tech, rf_time_get_elapsed(start_tick,RF_USEC));	  
			  start_tick = rf_time_get_tick();
			}
			
	    }
		else
		{
			api_status = ftm_enh_internal_device_cal_fbrx_rsb_iq_capture(	device_ptr,
                                                                            rsb_cal_alg,
                                                                            stg_offset_khz,
                                                                            sample_size,
                                                                            dbg_mode_en,
                                                                            iq_buf_ptr,
                                                                            &dcCompVals,
                                                                            band_union,
                                                                            rfcom_bw,
                                                                            cur_gain_state
                                                                        );
			
			if (api_status != FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS)
            {
               FTM_MSG_1(FTM_ERROR, "ftm_enh_internal_device_cal_fbrx_iq_capture failed for tech %d", (int)tech);
			   internal_device_cal_data->internal_device_cal_status.error_log = api_status;
			   if(iq_buf_ptr != NULL)
			   {
				 modem_mem_free((void*)iq_buf_ptr, MODEM_MEM_CLIENT_RFA);
			   }
               return FALSE;
            }
			if(dbg_mode_en>=2)
			{
			  FTM_MSG_3(FTM_MED,
							   "rsb cal: type(0: rx rsb; 1: fbrx rsb) %d, tech: %d, time span <fbrx IQ capture>: %d us",
							   rsb_type, tech, rf_time_get_elapsed(start_tick,RF_USEC));	  
			  start_tick = rf_time_get_tick();
			}
			
		}
		if( dbg_mode_en >= 2 )
		{
			ftm_enh_internal_device_cal_save_iq( tech,band,rx_path[rx_path_idx], iq_buf_ptr, sample_size, 0);
			FTM_MSG_3(FTM_MED,
							   "rsb cal: type(0: rx rsb; 1: fbrx rsb) %d, tech: %d, time span <IQ save>: %d us",
							   rsb_type, tech, rf_time_get_elapsed(start_tick,RF_USEC));	  
			start_tick = rf_time_get_tick();			
		}
		
    	 if(dbg_mode_en >= 4 )
     	{
	 		 for (id = 0;id < 5;id++)
	  		{
				FTM_MSG_4(FTM_MED,"iq_buf_ptr:0x%x, 0x%x, 0x%x, 0x%x", iq_buf_ptr[id*4],iq_buf_ptr[id*4+1],
					iq_buf_ptr[id*4+2],iq_buf_ptr[id*4+3]);
		
	  		}			
     	}
		iq_cap_struct_dc_comped.access_ptr = (uint32*)iq_buf_ptr;
        iq_cap_struct_dc_comped.sample_size = sample_size;
        iq_cap_struct_dc_comped.dcCompVals = dcCompVals;
	    rfcommon_msm_compute_rsb_coeff_from_iq_dc_comped( 
                            iq_cap_struct_dc_comped,
							(int16*) &rsb_a_coeff,
							(int16*) &rsb_a_coeff,
							&rsb_data.i_sq,
							&rsb_data.q_sq,
							&rsb_data.iq);
        if(dbg_mode_en>=2)
        {
		  FTM_MSG_3(FTM_MED,
							   "rsb cal: type(0: rx rsb; 1: fbrx rsb) %d, tech: %d, time span <rsb_coeff_computation>: %d us",
							   rsb_type, tech, rf_time_get_elapsed(start_tick,RF_USEC));	  
			
		  start_tick = rf_time_get_tick();		
        }
	
		/* Read one therm value */
	    //ftm_enh_device_cal_rsb_get_thermistor((rfm_device_enum_type)rx_path[rx_path_idx], band, &therm_data);	
		ftm_enh_device_cal_rsb_get_thermistor(RFM_DEVICE_0, band, &therm_data);	
        rsb_data.gain_state = rsb_instr->gain_states[rsb_res_gain_index];
		if(tech == FTM_RF_TECH_LTE)
        { 
          #ifdef FEATURE_LTE		
		  if(rsb_type == FTM_ENH_INTERNAL_DEVICE_CAL_RX_RSB)
		  {
		    ret_val = rfdevice_lte_rx_calc_rsb_nv_params( (rfm_device_enum_type)rx_path[rx_path_idx], (rfcom_lte_band_type) band, /*RFCOM_BW_LTE_10MHz*/ bw,
		  									rsb_data.i_sq,rsb_data.q_sq, rsb_data.iq, therm_data,
		  									&rsb_data.sin_theta_f,&rsb_data.cos_theta_f,&rsb_data.gain_inv_f,0,rsb_instr->gain_states[rsb_res_gain_index]);	
		  }
		  else
		  {
			 ret_val = rfdevice_lte_fbrx_calc_rsb_nv_params(RFM_DEVICE_0, (rfcom_lte_band_type) band, bw,/*rsb_instr->gain_states[rsb_res_gain_index],*/ &rsb_data);
		  }
          #else
             FTM_MSG(FTM_HIGH,"FEATURE_LTE is not enabled in this build");	  
          #endif
        }
	    else if(tech == FTM_RF_TECH_WCDMA)
	    {
              #ifdef FEATURE_WCDMA
	      if(rsb_type == FTM_ENH_INTERNAL_DEVICE_CAL_RX_RSB)
	      {
		    ret_val = rfdevice_wcdma_rx_get_rsb_cal_data_temp_comp((rfm_device_enum_type)rx_path[rx_path_idx], (rfcom_wcdma_band_type) band, &rsb_data);
		  }
		  else
		  {
		    ret_val = rfdevice_wcdma_get_fbrx_rsb_cal(RFM_DEVICE_0, (rfcom_wcdma_band_type) band, /*RFDEV_WCDMA_BW_1X*/ bw, &rsb_data);
		  }
	      #else
                FTM_MSG(FTM_HIGH,"FEATURE_WCDMA is not enabled in this build");	  
              #endif
	    }

	    else if (tech == FTM_RF_TECH_TDSCDMA)
	    {
                  #ifdef FEATURE_TDSCDMA
		  if(rsb_type == FTM_ENH_INTERNAL_DEVICE_CAL_RX_RSB)
		  {
			ret_val = rfdevice_tdscdma_rx_get_rsb_nv_params((rfm_device_enum_type)rx_path[rx_path_idx], (rfcom_tdscdma_band_type) band, 
		  								    rsb_data.i_sq,rsb_data.q_sq, rsb_data.iq,therm_data,
		  									&rsb_data.sin_theta_f,&rsb_data.cos_theta_f,&rsb_data.gain_inv_f);
		  }
		  else
		  {
			  ret_val = rfdevice_tdscdma_fbrx_get_rsb_nv_params(RFM_DEVICE_0, (rfcom_tdscdma_band_type) band, bw, /*rsb_instr->gain_states[rsb_res_gain_index], */&rsb_data);
		  }
                 #else
                   FTM_MSG(FTM_HIGH,"FEATURE_TDSCDMA is not enabled in this build");	  
                 #endif

	    }
        if(dbg_mode_en >=2)
        {
		  FTM_MSG_3(FTM_MED,
							   "rsb cal: type(0: rx rsb; 1: fbrx rsb) %d, tech: %d, time span <tempcomp and efs write>: %d us",
							   rsb_type, tech, rf_time_get_elapsed(start_tick,RF_USEC));	  
			
		  start_tick = rf_time_get_tick();	
        }
	    FTM_MSG_3(FTM_MED, "before cal, rsb sin_theta: %d, cos_theta: %d, gain_inv: %d", rsb_data.sin_theta_f,rsb_data.cos_theta_f, rsb_data.gain_inv_f);
	    if(ret_val == TRUE)
	    {
	      rsb_result->rsb_res[rx_path[rx_path_idx]][rsb_res_gain_index].nv_active = 1;
		  rsb_result->rsb_res[rx_path[rx_path_idx]][rsb_res_gain_index].sin_theta_final = rsb_data.sin_theta_f;
		  rsb_result->rsb_res[rx_path[rx_path_idx]][rsb_res_gain_index].cos_theta_final = rsb_data.cos_theta_f;
		  rsb_result->rsb_res[rx_path[rx_path_idx]][rsb_res_gain_index].gain_inv_final = rsb_data.gain_inv_f; // hardcode to only one gain state for now
		  rsb_result->device_mask |= (1<<rx_path[rx_path_idx]);
  		  rsb_result->rsb_res[rx_path[rx_path_idx]][rsb_res_gain_index].rsb_before_cal = (int8) floor(compute_rsb(rsb_data)); 			
		  
		  if (dbg_mode_en >= 1)
		  {
			ftm_common_compute_rsb_coeffs_a_b_params(   rsb_data.sin_theta_f,
													  rsb_data.cos_theta_f,
													  rsb_data.gain_inv_f,
													  0,
													  16384,
													  16384,
													  0, /* rsb_cal_is_on - RSB cal not currently in process */
													  &rsb_a_coeff,
													  &rsb_b_coeff );
								
//#if defined(FEATURE_BOLT_MODEM)
			if(rsb_result->rsb_res[rx_path[rx_path_idx]][rsb_res_gain_index].nv_active == 1)
			{
			  if(rsb_type == FTM_ENH_INTERNAL_DEVICE_CAL_RX_RSB)
			  { 		 
				rfcommon_rx_msm_update_rsb_coeff(rxlm_buffer_idx[rx_path_idx+carrier*2],-rsb_a_coeff,-rsb_b_coeff, rflm_tech_id,msm_carr_bw,dbg_mode_en); 			 
			  }
			  else if (rsb_type == FTM_ENH_INTERNAL_DEVICE_CAL_FB_RSB)
			  {
			    rfcommon_fbrx_msm_update_rsb_coeff( fbrx_buffer_idx[0], -rsb_a_coeff,-rsb_b_coeff, rflm_tech_id, ( rfcommon_fbrx_tx_cfg_type) bw);
			  }
			} 		
//#endif

			DALSYS_BusyWait(2500);  /// give HW enough time to update iqmc register, move here to save cal-time
		    memset(iq_buf_ptr, 0,sample_size*4);
		    if( rsb_type == FTM_ENH_INTERNAL_DEVICE_CAL_RX_RSB )
		    {
		      ftm_enh_internal_device_cal_rx_iq_capture(tech, carrier, rx_path[rx_path_idx], sample_size,iq_buf_ptr);
			
		      if(tech == FTM_RF_TECH_WCDMA )
		      {
	 	        ftm_convert_iq_8b_to_16b( iq_buf_ptr, 1, sample_size);
			    //ftm_convert_rsb_iq_format(&iq_buf_ptr[0], 0,FTM_RX_RSB_IQ_8bit_to_16bit,sample_size);
		      }
		      else if( (tech == FTM_RF_TECH_LTE )||(tech == FTM_RF_TECH_TDSCDMA ) )
		      {
			    ftm_convert_rsb_iq_format(&iq_buf_ptr[0], 0,FTM_RX_RSB_IQ_16bit_to_16bit,sample_size);
		      }
		    }
		    else if( rsb_type == FTM_ENH_INTERNAL_DEVICE_CAL_FB_RSB )
		    {
			  api_status = ftm_enh_internal_device_cal_fbrx_rsb_iq_capture(   device_ptr,
                                                                              rsb_cal_alg,
                                                                              stg_offset_khz,
                                                                              sample_size,
                                                                              dbg_mode_en,
                                                                              iq_buf_ptr,
                                                                              &dcCompVals,
                                                                              band_union,
                                                                              rfcom_bw,
                                                                              cur_gain_state
                                                                          );
			   
			  if (api_status != FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS)
			  {
				  FTM_MSG_1(FTM_ERROR, "ftm_enh_internal_device_cal_fbrx_iq_capture failed for tech %d", (int)tech);
				  internal_device_cal_data->internal_device_cal_status.error_log = api_status;
				  if(iq_buf_ptr != NULL)
				  {
					modem_mem_free((void*)iq_buf_ptr, MODEM_MEM_CLIENT_RFA);
				  }
				  return FALSE;
			  }	   

		     }
             iq_cap_struct_dc_comped.access_ptr = (uint32*)iq_buf_ptr;
             iq_cap_struct_dc_comped.sample_size = sample_size;
             iq_cap_struct_dc_comped.dcCompVals = dcCompVals;
             rfcommon_msm_compute_rsb_coeff_from_iq_dc_comped( 
                                iq_cap_struct_dc_comped,
                                (int16*) &rsb_a_coeff,
                                (int16*) &rsb_a_coeff,
                                &rsb_data.i_sq,
                                &rsb_data.q_sq,
                                &rsb_data.iq);
	
 			  /* Read one therm value */
		    //	ftm_enh_device_cal_rsb_get_thermistor((rfm_device_enum_type)rx_path[rx_path_idx], band, &therm_data);	
			  ftm_enh_device_cal_rsb_get_thermistor(RFM_DEVICE_0, band, &therm_data);	
			  therm_data = 0; // not supported
			  ret_val = ftm_compute_rsb_nv_params( 
  							(rfm_device_enum_type)rx_path[rx_path_idx],
  							tech,
  							band, 
  							RFCOM_BW_LTE_10MHz,
  							rsb_data.i_sq,  rsb_data.q_sq,  rsb_data.iq,  therm_data, 
							&rsb_data.sin_theta_f,&rsb_data.cos_theta_f,&rsb_data.gain_inv_f,0);
			  FTM_MSG_3(FTM_MED, "after cal, rsb sin_theta: %d, cos_theta: %d, gain_inv: %d", rsb_data.sin_theta_f,rsb_data.cos_theta_f, rsb_data.gain_inv_f   );
			  if(ret_val == TRUE)
			  {
 			    rsb_result->rsb_res[rx_path[rx_path_idx]][rsb_res_gain_index].cos_theta_after_cal = rsb_data.cos_theta_f;
			    rsb_result->rsb_res[rx_path[rx_path_idx]][rsb_res_gain_index].sin_theta_after_cal = rsb_data.sin_theta_f;	
			    rsb_result->rsb_res[rx_path[rx_path_idx]][rsb_res_gain_index].gain_inv_after_cal = rsb_data.gain_inv_f;
			    rsb_result->rsb_res[rx_path[rx_path_idx]][rsb_res_gain_index].rsb_after_cal = (int8) floor(compute_rsb(rsb_data));  
			  }
			  else
			  {
			    rsb_result->rsb_res[rx_path[rx_path_idx]][0].rsb_after_cal = 0; 	
			  }
		  	}
			if( dbg_mode_en >= 2)  // re-check if iq is good after dynamic iqmc register update
   		    {
   			  ftm_enh_internal_device_cal_save_iq( tech, band,rx_path[rx_path_idx], iq_buf_ptr, sample_size, 1);								  
		    }			
	    }
	    else
	    {
		  rsb_result->rsb_res[rx_path[rx_path_idx]][0].rsb_before_cal = 0; 
		  rsb_result->rsb_res[rx_path[rx_path_idx]][0].rsb_after_cal = 0; 
		  
		 //add error mesg here
	    }
	    FTM_MSG_6(FTM_MED,"rsb cal: tech:%d, band:%d, carrier:%d, rx_path:%d, rsb_before_cal:%d, rsb_after_cal:%d",
	  	tech,band,carrier,rx_path[rx_path_idx],rsb_result->rsb_res[rx_path[rx_path_idx]][0].rsb_before_cal, 
	  	rsb_result->rsb_res[rx_path[rx_path_idx]][0].rsb_after_cal);	
 	 }	
     if( rsb_type == FTM_ENH_INTERNAL_DEVICE_CAL_FB_RSB )
     {
	   rfcommon_fbrx_mc_disable_rf_dev(device_ptr);
       FTM_MSG(FTM_HIGH,"Disabled FBRx setup ");
     }
   	}  	

     if(rsb_result->device_mask != 0)
     {
   	   *res_len = sizeof(ftm_enh_internal_device_cal_rsb_result_type);
	   if(rsb_type == FTM_ENH_INTERNAL_DEVICE_CAL_FB_RSB ) // for fbrx rsb cal, we keep the old path number
	   {
         *res_len -= sizeof(ftm_rsb_result_data_type)*MAX_NUM_RSB_GAIN_STATES*(FTM_ENH_INTERNAL_DEVICE_CAL_RSB_RES_MAX-(FTM_ENH_INTERNAL_DEVICE_CAL_RSB_RES_FBRX+1));
	   }
	
     }
     else
   	  *res_len = 0;
   }
   if(iq_buf_ptr !=NULL)
   	 modem_mem_free(iq_buf_ptr,MODEM_MEM_CLIENT_RFA);

 //#if 0
   if((ret_val==TRUE) && ( max_cal_elements!=0 )&& (flag != 0 ) )
   { 
	 {
		rsb_ver_data.cal_data = (rfdevice_rsb_calibrated_data_type* ) modem_mem_alloc( 
					 max_cal_elements*sizeof(rfdevice_rsb_calibrated_data_type), MODEM_MEM_CLIENT_RFA );
	   
		if(rsb_ver_data.cal_data == NULL )
		{
			 FTM_MSG(FTM_ERROR, "Enhanced internal device cal: rsb: not able to allocate memory, returns NULL");
			 internal_device_cal_data->internal_device_cal_status.error_log  = FTM_ENH_INTERNAL_DEVICE_CAL_MEM_ALLOC_FAILURE;
			 internal_device_cal_data->internal_device_cal_status.error_mask = (1<< ENH_INTERNAL_DEVICE_CAL_RSB_BIT);
			 *res_len = 0;
			 return FALSE;
		}	   
		rsb_ver_data.num_elements = max_cal_elements;   
	  }
      if(flag == 3) 
      {
	    rx_path[0]= (device_mask & 0x03)>0? RFM_DEVICE_0: ( (device_mask & 0x30)>0 ? RFM_DEVICE_4: RFM_DEVICE_2 );
      }
      ret_val = ftm_rsb_efs_save((rfm_device_enum_type) rx_path[0],band,&rsb_ver_data);
	  if(ret_val ==FALSE)
	  {
		  if(rsb_ver_data.cal_data !=NULL)
		  {
             if(rsb_ver_data.num_calibrated_item > max_cal_elements)
             {
			   modem_mem_free(rsb_ver_data.cal_data,MODEM_MEM_CLIENT_RFA);
			   rsb_ver_data.num_elements = rsb_ver_data.num_calibrated_item;
			   max_cal_elements = rsb_ver_data.num_calibrated_item;
			   rsb_ver_data.cal_data =	(rfdevice_rsb_calibrated_data_type*) modem_mem_alloc(max_cal_elements*sizeof(rfdevice_rsb_calibrated_data_type), MODEM_MEM_CLIENT_RFA);
			   if(rsb_ver_data.cal_data!=NULL)
			   {
			     ret_val = ftm_rsb_efs_save((rfm_device_enum_type) rx_path[0],band, &rsb_ver_data);
			     if(ret_val == FALSE)
			     {
				modem_mem_free(rsb_ver_data.cal_data,MODEM_MEM_CLIENT_RFA);
			   	internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_EFS_FILESAVE_FAILED  ;
			   	internal_device_cal_data->internal_device_cal_status.error_mask = (1<< ENH_INTERNAL_DEVICE_CAL_RSB_BIT);
			   	*res_len = 0;
				return FALSE;
			     }
			   }
             }
			 else
			 {
			   modem_mem_free(rsb_ver_data.cal_data,MODEM_MEM_CLIENT_RFA);
			   internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_EFS_FILESAVE_FAILED  ;
			   internal_device_cal_data->internal_device_cal_status.error_mask = (1<< ENH_INTERNAL_DEVICE_CAL_RSB_BIT);
			   *res_len = 0;
			   return FALSE;
			 }
		  	}
	  }
	  if( ( (flag ==2) ||(flag ==3)) &&(ret_val ==TRUE) )
	  {
		if((sizeof(uint16)+rsb_ver_data.num_calibrated_item*sizeof( rfdevice_rsb_calibrated_data_type) ) >
			internal_device_cal_data->internal_device_cal_result.res_max_len )
		{ 		
		  internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_RES_OVERSIZE ;
		  internal_device_cal_data->internal_device_cal_status.error_mask = (1<< ENH_INTERNAL_DEVICE_CAL_RSB_BIT);
		  *res_len = 0;
		  if(rsb_ver_data.cal_data !=NULL)
   	        modem_mem_free(iq_buf_ptr, MODEM_MEM_CLIENT_RFA);
		  FTM_MSG_3(FTM_MED,
                     " rsb cal: type(0: rx rsb; 1: fbrx rsb) %d, tech: %d, complete in %d us",
                     rsb_type, tech, rf_time_get_elapsed(start_tick0,RF_USEC)); 
		  FTM_MSG_2(FTM_MED, "rsb efs write verification data out of max limited size,max_limit_size :%d, requested element:%d", 
		  	internal_device_cal_data->internal_device_cal_result.res_max_len, rsb_ver_data.num_calibrated_item);
		  return FALSE;
		}
		if(rsp_payload_validation_addr !=NULL)
		{
	      memscpy(rsp_payload_validation_addr,sizeof(uint16),(void *)&(rsb_ver_data.num_calibrated_item), sizeof(uint16));
		  internal_device_cal_data->internal_device_cal_result.rsb_cal_ver_data_len =sizeof(uint16);
		  rsp_payload_validation_addr += sizeof(uint16);
          FTM_MSG_1(FTM_MED,"calibrated_num :%d", (rsb_ver_data.num_calibrated_item));
		  if(rsb_ver_data.num_calibrated_item!=0)
          {
		    memscpy(rsp_payload_validation_addr, rsb_ver_data.num_calibrated_item*sizeof(rfdevice_rsb_calibrated_data_type),
		    rsb_ver_data.cal_data, rsb_ver_data.num_calibrated_item*sizeof(rfdevice_rsb_calibrated_data_type));
		    internal_device_cal_data->internal_device_cal_result.rsb_cal_ver_data_len += rsb_ver_data.num_calibrated_item*sizeof(rfdevice_rsb_calibrated_data_type);
		   
          }
		}
	   }
   	}
   
   if(rsb_ver_data.cal_data !=NULL)
   	 modem_mem_free(rsb_ver_data.cal_data, MODEM_MEM_CLIENT_RFA);

   FTM_MSG_3(FTM_MED,
                     "rsb cal: type(0: rx rsb; 1: fbrx rsb) %d, tech: %d, complete in %d us",
                     rsb_type, tech, rf_time_get_elapsed(start_tick0,RF_USEC));   
   return ret_val;  
}

#endif /* FEATURE_FACTORY_TESTMODE */

#endif /* FEATURE_GNSS_ONLY_NO_WWAN */

/*! @} */



