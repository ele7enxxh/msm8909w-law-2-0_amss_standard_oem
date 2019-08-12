/*
  @file
  ftm_calibration_v3_enh_internal_device_cal.c

  @brief
  This module contains common RF calibration code for Data Opcode

  @addtogroup RF_COMMON_FTM_CALV3_DATA

  @brief
  This module provides a common framework to perform Data OpCode operation.
  This module is responsible for calling the WAN technology specific handler
  function to perform various Data operations. This module is also responsible
  for populating the Data results buffer with appropriate calibration data.
  */

/*==============================================================================

  Copyright (c) 2013 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/src/ftm_calibration_v3_enh_internal_device_cal.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
11/25/14    xs     Set rx_prof_in_process to be false once rsb cal is done
11/05/14    xs     Add scell2 (dev4/5) rsb support
09/12/14    xs     Update fbrx rsb result buffer copying and check to avoid copy over-size
09/12/14    xs     Update init buffer size for the case of extra validation data 
08/05/14    xs     Seperate efs write support from normal rsb cal
07/15/14    xs     Add rsb cal in-complete flag
06/27/14    xs     Update FBRx/Rx RSB opcode implementation to solve rsb cal time out of max allowed esc segment 
06/12/14    xs     Add FBRx /Rx RSB Cal with Opcode
05/26/14    id     Fix for FBRx DC Cal functionality with Opcode
05/21/14    brath  Calculate DC results for each nv mode and populate results for wach nv mode structure
04/23/14    brath  Initial version  

==============================================================================*/

#include "comdef.h"
#include "ftm_common_calibration_v3.h"
#include "ftm_calibration_v3_enh_internal_device_cal.h"
#include "ftm_common_data.h"
#include "ftm_common_xpt.h"
#include "ftm_common_control.h"
#include "ftm_msg.h"
#include "ftmdiag.h"
#include "ftm.h"
#include "ftm_rf_cmd.h"
#include "rfcommon_math.h"
#include "modem_mem.h"
#include "rfcommon_mdsp_types.h"
#include "rfcommon_core_xpt.h"
#include "rfcommon_mc.h"
#include "rfcommon_core_utils.h"
#include "diagcmd.h"
#include "math.h"
#include "stringl.h" /* for memscpy */

#ifdef FEATURE_CDMA1X
#include "ftm_1x_control.h"
#include "rf_cdma_msm.h"
#include "rf_cdma_mc.h"
#include "rfdevice_cdma_interface.h"
#endif

#ifdef FEATURE_LTE
#include "rflte_ftm_external_api.h"
#include "rflte_nv.h"
#include "rflte_msm.h"
#include "rflte_core_util.h"
#include "rflte_ftm_mc.h"
#include "rfdevice_lte_interface.h"
#endif

#ifdef FEATURE_WCDMA
#include "ftm_wcdma_ctl.h"
#include "rfwcdma_msm.h"
#include "rfnv_wcdma.h"
#include "rfdevice_wcdma_intf.h"
#endif

#ifdef FEATURE_WCDMA
extern rfcom_wcdma_band_type ftm_curr_wcdma_mode;
extern rfnv_wcdma_nv_tbl_type *rfnv_wcdma_get_nv_tbl_ptr( rfnv_wcdma_supported_mode_type band );
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_TDSCDMA
#include "ftm_tdscdma_ctl.h"
#include "rf_tdscdma_msm.h"
#include "rf_tdscdma_core_util.h"
#endif

#ifdef FEATURE_LTE
extern rflte_nv_tbl_type* rflte_mc_get_nv_tbl_ptr(uint32 carrier_idx);
#endif

#ifdef FEATURE_TDSCDMA
extern int32 rf_tdscdma_msm_get_rms_dbfs_x100_val( void );
#endif

/*============================================================================*/
/*!
  @name Data OpCode handling

  @brief
  This section contains functions for Data opcode handling.
*/
/*! @{ */


/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Deinitialize Enhanced Internal Device Cal OpCode
 
  @details
  This function is called from common sweep function to perform
  data deinitialization of Enhanced Internal Device Cal OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
  
  @return
  Status of deinitialization
*/
boolean
ftm_calibration_enh_internal_device_cal_deinit
(
  struct ftm_calibration_data_type_t *cal_data
)
{
  boolean ret_val = TRUE;
  ftm_cal_enh_int_cal_opcode_data_type *cfg_data;

  /* Shorten data structure pointers */
  cfg_data = &(cal_data->seg_data.oc_payload.enh_int_dev_cal);

  if ( cfg_data->res_buff != NULL )
  {
    /* De-allocate memory which was allocated for results */
    modem_mem_free( cfg_data->res_buff, MODEM_MEM_CLIENT_RFA );
    cfg_data->res_buff = NULL;
  } /* if ( cfg_data->res_buff != NULL ) */
  else
  {
    FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][DATA.{0x%x}] "
               "Deinit : Null ResBuff",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_ENH_INTERNAL_DEVICE_CALIBRATION );
    ret_val = FALSE;
  } /* if ! ( cfg_data->res_buff != NULL ) */

  return ret_val;
} /* ftm_calibration_enh_internal_device_cal_deinit */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize Enhanced Internal Device Cal OpCode
 
  @details
  This function is called from common sweep function to perform
  data initialization of Enhanced Internal Device Cal OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
  
  @return
  Status of initialization
*/
boolean
ftm_calibration_enh_internal_device_cal_init
(
  struct ftm_calibration_data_type_t *cal_data
)
{
  boolean ret_val = TRUE;
  ftm_cal_opcode_reg_data_type reg_data;
  ftm_cal_enh_int_cal_opcode_data_type *cfg_data;
  uint16 max_res_len = 0;

  /* Shorten data structure pointers */
  cfg_data = &(cal_data->seg_data.oc_payload.enh_int_dev_cal);

  reg_data.unpack_fp = ftm_calibration_enh_internal_device_cal_unpack;
  reg_data.process_fp = ftm_calibration_enh_internal_device_cal_process;
  reg_data.res_buff_info.buff_ptr = NULL;
  reg_data.res_buff_info.buff_sz_max = 0;
  reg_data.res_buff_info.buff_sz = 0;

  if ( cfg_data->res_buff == NULL )
  {
    /* Allocate memory for results buffer to store results for the current
    segment */

	max_res_len =  (uint16) MAX_ENH_INTERNAL_DEVICE_CAL_RES_SIZE >(MAX_ENH_INTERNAL_DEVICE_CAL_RSB_VER_LENGTH+sizeof(ftm_enh_internal_device_cal_rsb_result_type))?
					MAX_ENH_INTERNAL_DEVICE_CAL_RES_SIZE : (MAX_ENH_INTERNAL_DEVICE_CAL_RSB_VER_LENGTH+sizeof(ftm_enh_internal_device_cal_rsb_result_type));

	// rsb cal validation was requested to be added after development is done
    cfg_data->res_buff = modem_mem_alloc( max_res_len,
                                          MODEM_MEM_CLIENT_RFA );

    /* Check if the malloc was successfull */
    if ( cfg_data->res_buff != NULL )
    {
      reg_data.res_buff_info.buff_ptr = cfg_data->res_buff;
      reg_data.res_buff_info.buff_sz_max = max_res_len;
      reg_data.res_buff_info.buff_sz = 0;
    } /* if ( cfg_data->res_buff != NULL ) */
    else
    {
      FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][DATA.{0x%x}] "
                 "Init : ResBuff malloc failed",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_ENH_INTERNAL_DEVICE_CALIBRATION );
      reg_data.res_buff_info.buff_ptr = 0;
      reg_data.res_buff_info.buff_sz_max = 0;
      reg_data.res_buff_info.buff_sz = 0;
      ret_val = FALSE;
    } /* if !( cfg_data->res_buff != NULL ) */
  } /* if ( cfg_data->res_buff == NULL ) */
  else
  {
    FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][ENH_INT_DEV_CAL.{0x%x}] "
               "Init : ResBuff not NULL",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_ENH_INTERNAL_DEVICE_CALIBRATION );
    ret_val = FALSE;
  } /* if ! ( cfg_data->res_buff == NULL ) */

  /* Register OpCOde */
  ret_val = ftm_calibration_register_opcode( cal_data,
                                             FTM_CAL_OPCODE_ENH_INTERNAL_DEVICE_CALIBRATION,
                                             reg_data );

  return ret_val;

} /* ftm_calibration_enh_internal_device_cal_init */



/*----------------------------------------------------------------------------*/
/*!
  @brief
  Unpack Enhanced Internal Device Cal OpCode
 
  @details
  This function is called from common sweep function to perform
  unpacking of Enhanced Internal Device Cal OpCode payload
 
  @param cal_data
  Pointer to the data structure containing calibration data
 
  @param instr_payload
  Pointer to the memory containing the payload
 
  @return
  Total size of the payload
*/
uint16
ftm_calibration_enh_internal_device_cal_unpack
( 
  struct ftm_calibration_data_type_t *cal_data,
  struct ftm_cal_opcode_dispatch_info_type_t *oc_disp,
  void const *instr_payload
)
{
    uint16 payload_size = 0; /* Payload Size */
    uint8 *payload_ptr = NULL; /* Paylaod pointer */
   uint8 sig_bw_iter=0;
   uint8 gain_state_iter=0;

  ftm_cal_enh_int_dev_cal_parameter_type *data_unpack=NULL;
  ftm_cal_enh_int_cal_opcode_data_type *cfg_data;

  /* NULL Pointer check */
  if ( ( instr_payload == NULL ) || ( cal_data == NULL ) )
  {
    FTM_MSG_1( FTM_HIGH, "[calv3][Segment X][DATA.{0x%x}] "
               "Unpack : Null Argument",
               FTM_CAL_OPCODE_ENH_INTERNAL_DEVICE_CALIBRATION );
    return 0;
  } /* if ( instr_payload == NULL ) */

  cfg_data = &(cal_data->seg_data.oc_payload.enh_int_dev_cal);

  /* Get pointer to memory to hold Data Opcode unpacked data */
  data_unpack = &(cfg_data->up_param);

  /* Initialize the payload pointer */
  payload_ptr = (uint8*)instr_payload;
  payload_size = 0;

  /* Reset all data */
  memset( data_unpack, 0x0, sizeof(ftm_cal_enh_int_dev_cal_parameter_type) );

  /* Update the payload size and Payload pointer with version*/
  UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );

 /* Update the payload size and Payload pointer with Results Revision */
  memscpy( &(data_unpack->res_ver), sizeof(uint8), 
           payload_ptr, sizeof(uint8) );
  UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );

  /* Update the payload size and Payload pointer with Debug Flag */
  memscpy( &(data_unpack->dbg_mode_en), sizeof(uint8), 
           payload_ptr, sizeof(uint8) );
  UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );

   /* Update the payload size and Payload pointer with Flag*/
  memscpy( &(data_unpack->flag), sizeof(uint16),
           payload_ptr, sizeof(uint16) );
  UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint16) );

  /* Update the payload size and Payload pointer with Technology */
  memscpy( &(data_unpack->tech), sizeof(uint8),
           payload_ptr, sizeof(uint8) );
  UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );
  
  /* Update the payload size and Payload pointer with Band */
  memscpy( &(data_unpack->band), sizeof(uint16),
           payload_ptr, sizeof(uint16) );
  UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint16) );

  /* Update the payload size and Payload pointer with Ref Channel */
  memscpy( &(data_unpack->ref_chan), sizeof(uint32),
          payload_ptr, sizeof(uint32) );
  UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint32) );

   /* Update the payload size and Payload pointer with Internal Cal Type Mask */
  memscpy( &(data_unpack->internal_cal_type_mask), sizeof(uint32),
           payload_ptr, sizeof(uint32) );
  UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint32) );

  /* Update the payload size and Payload pointer with Number of BWs */
  memscpy( &(data_unpack->num_bw), sizeof(uint8),
           payload_ptr, sizeof(uint8) );
  UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );

   /* Update the payload size and Payload pointer with Signal Bandwidth */
  for(sig_bw_iter=0;sig_bw_iter<data_unpack->num_bw;sig_bw_iter++)
  {
   memscpy( &(data_unpack->bw[sig_bw_iter]), sizeof(uint8),
           payload_ptr, sizeof(uint8) );
   UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );
  }

  /* Update the payload size and Payload pointer with Device Mask */
  memscpy( &(data_unpack->device_mask), sizeof(uint16),
           payload_ptr, sizeof(uint16) );
  UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint16) );

  if ((data_unpack->internal_cal_type_mask) & ENH_INT_DEV_CAL_FBDC )
  {
	 FTM_MSG(FTM_MED,"ENH INT CAL: FBRx DC Cal is called");

     /* Update the payload size and Payload pointer with Number of FBRx DC Gain States */
     memscpy( &(data_unpack->instr.cal_instr_payload_fb_dc_type.num_fbrx_gain_states), sizeof(uint8),
            payload_ptr, sizeof(uint8) );
     UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );

     /* Update the payload size and Payload pointer with FB Gain States */
   for(gain_state_iter=0;gain_state_iter<data_unpack->instr.cal_instr_payload_fb_dc_type.num_fbrx_gain_states;gain_state_iter++)
   {
       memscpy( &(data_unpack->instr.cal_instr_payload_fb_dc_type.fbrx_gain_state[gain_state_iter]), sizeof(uint8),
          payload_ptr, sizeof(uint8) );
   UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );
   }

   memscpy( &(data_unpack->instr.cal_instr_payload_fb_dc_type.num_iq_samples), sizeof(uint16),
          payload_ptr, sizeof(uint16) );
   UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint16) );
  }

  if ((data_unpack->internal_cal_type_mask) & ENH_INT_DEV_CAL_RSB )
  {
	FTM_MSG(FTM_MED,"ENH INT CAL: FBRx RSB Cal is called");
    memscpy( &(data_unpack->instr.cal_instr_payload_rsb_type), sizeof(data_unpack->instr.cal_instr_payload_rsb_type),
		payload_ptr,sizeof(data_unpack->instr.cal_instr_payload_rsb_type));	
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(data_unpack->instr.cal_instr_payload_rsb_type) );
	
  }

  if ((data_unpack->internal_cal_type_mask) & ENH_INT_DEV_CAL_RSB_GET_RESULT    )
  {
    FTM_MSG(FTM_MED,"ENH INT CAL: FBRx ENH_INT_DEV_CAL_RSB_GET_RESULT is called");
    memscpy( &(data_unpack->instr.cal_instr_payload_rsb_type), sizeof(data_unpack->instr.cal_instr_payload_rsb_type),
		payload_ptr,sizeof(data_unpack->instr.cal_instr_payload_rsb_type));	
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(data_unpack->instr.cal_instr_payload_rsb_type) );
  }

  /* Store the instruction Pointer */
  oc_disp->payload_ptr = (uint8*)instr_payload;

  if ( cal_data->cal_config.debug_mode_en)
  {
    FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][ENH_INT_DEV_CAL.{0x%x}] "
               "Unpack : %d bytes",
               cal_data->seg_data.cal_instance + 1,
               FTM_CAL_OPCODE_ENH_INTERNAL_DEVICE_CALIBRATION,
               payload_size );
  } /* if ( p_ptr->debug_mode_en == TRUE ) */
 
 return payload_size;
} /* ftm_calibration_enh_internal_device_cal_unpack */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process Enhanced Internal Device Cal OpCode
 
  @details
  This function is called from common sweep function to perform the
  processing of the Enhanced Internal Device Cal OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
 
  @param oc_disp
  Pointer to the data structure containing current opcode dispatch data
 
  @return
  Status of opcode processing
*/
boolean
ftm_calibration_enh_internal_device_cal_process(struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_dispatch_info_type *oc_disp)
{
   boolean ret_val=TRUE;
   ftm_cal_enh_int_cal_opcode_data_type *cfg_data;
   ftm_enh_internal_device_cal_mask_type curr_action;

  /* NULL Pointer check */
  if ( cal_data == NULL )
  {
    FTM_MSG_1( FTM_HIGH, "[calv3][Segment X][ENH_INT_CAL.{0x%x}] "
               "Process : Null Argument",
               FTM_CAL_OPCODE_ENH_INTERNAL_DEVICE_CALIBRATION );  
    return 0;
  } /* if ( cal_data == NULL ) */

  cfg_data = &(cal_data->seg_data.oc_payload.enh_int_dev_cal); 

  curr_action = cfg_data->up_param.internal_cal_type_mask;
  FTM_MSG_1( FTM_HIGH, "ENH INT CAL MASK = %d", curr_action );

  if ( curr_action & ENH_INT_DEV_CAL_FBDC )
  {
   if (cal_data->cal_config.debug_mode_en)
    {
     FTM_MSG_2( FTM_HIGH, "[calv3][Segment %3d][ENH_INT_CAL.{0x%x}] "
                "Process : FBRx DC Cal",
                cal_data->seg_data.cal_instance,
                FTM_CAL_OPCODE_ENH_INTERNAL_DEVICE_CALIBRATION );  
    }

   ret_val=ftm_enh_internal_device_calibration_process_dc( cal_data, 
                                                         &(cfg_data->up_param), 
                                                         oc_disp );
   ret_val=TRUE;
  }
  else if( curr_action & ENH_INT_DEV_CAL_RSB)
  {
    if (cal_data->cal_config.debug_mode_en)
    {
		FTM_MSG_2( FTM_HIGH, "[calv3][Segment %3d][ENH_INT_CAL.{0x%x}] "
				   "Process : FBRx/ Rx RSB Cal",
				   cal_data->seg_data.cal_instance,
				   FTM_CAL_OPCODE_ENH_INTERNAL_DEVICE_CALIBRATION );  
    }
    ret_val = ftm_enh_internal_device_calibration_process_rsb( cal_data, 
                                                         &(cfg_data->up_param), 
                                                         oc_disp );
  }

  else if( curr_action & ENH_INT_DEV_CAL_RSB_GET_RESULT )
  {
    if (cal_data->cal_config.debug_mode_en)
    {
		FTM_MSG_2( FTM_HIGH, "[calv3][Segment %3d][ENH_INT_CAL.{0x%x}] "
				   "Process : FBRx/ Rx RSB ENH_INT_DEV_CAL_RSB_GET_RESULT",
				   cal_data->seg_data.cal_instance,
				   FTM_CAL_OPCODE_ENH_INTERNAL_DEVICE_CALIBRATION );  
    }
	ret_val = ftm_enh_internal_device_calibration_process_rsb_results( cal_data,
                                                 &(cfg_data->up_param),
                                                 //cfg_data->up_param.refChan,
                                                 oc_disp);
    ret_val = TRUE;
  }  
  /*--------------------------------------------------------------------------*/
  /* Incorrect Sub-Opcode */
  /*--------------------------------------------------------------------------*/
  else
  {
    FTM_MSG_3( FTM_ERROR, "[calv3][Segment %3d][ENH_INT_CAL.{0x%x}], action:%d "
				 "Process : Incorrect Action for Enhanced Internal Device Cal Processing",
				 cal_data->seg_data.cal_instance,
				 FTM_CAL_OPCODE_ENH_INTERNAL_DEVICE_CALIBRATION,
				 curr_action);	

    ret_val = FALSE;
  }
  
  return ret_val;
}/* ftm_calibration_enh_internal_device_cal_process*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Call the Enhanced Internal Devica Cal function for FBRx DC Cal
 
  @details
  This function is called from common sweep function to perform
  FBRx DC Cal 
 
  @param cal_data
  Pointer to the data structure containing calibration data\
 
  @param data_param
  Pointer to the data structure containing data unpacked parameter
 
  @param oc_disp
  Pointer to the data structure containing current opcode dispatch data
 
  @return
  Status of opcode processing
*/


boolean 
ftm_enh_internal_device_calibration_process_dc
( 
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_enh_int_dev_cal_parameter_type const *data_param,
  ftm_cal_opcode_dispatch_info_type *oc_disp
)
{
   boolean return_val = TRUE;
   uint8 *res_write_ptr=NULL; 
   uint16 dc_res_size=0;
   //uint8 dc_written_size=0;
   uint16 written_size = 0; /* Total written size */
   uint16 free_size = 0; /* Total free size */
   uint8 res_type = FTM_CAL_OPCODE_ENH_INTERNAL_DEVICE_CALIBRATION;
   uint8 res_action_type=ENH_INTERNAL_DEVICE_CAL_DC_BIT;
   ftm_calibration_enh_int_dev_cal_dc_result_type *cal_result_ptr=NULL;
   ftm_enh_internal_device_cal_data_type *internal_device_cal_data=NULL;
   ftm_enh_internal_device_cal_fb_dc_result_type *dc_result_ptr=NULL;
   uint8 version = 0;
   uint8 i=0;
   uint8 sig_bw_iter=0;

   /* Parameter validation */
  if ( data_param == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_enh_internal_device_calibration_process_dc: NULL argument" );
    return FALSE;
  } /* if ( data_param == NULL ) */

  dc_result_ptr = (ftm_enh_internal_device_cal_fb_dc_result_type*)modem_mem_alloc(
               sizeof(ftm_enh_internal_device_cal_fb_dc_result_type), MODEM_MEM_CLIENT_RFA);

 if( dc_result_ptr == NULL )
  {
    FTM_MSG( FTM_ERROR, "Failed to allocate memory for FBRx DC Results!");
    return FALSE;
  }
 else
 {
    memset(dc_result_ptr, 0, sizeof(ftm_enh_internal_device_cal_fb_dc_result_type));
 }

 cal_result_ptr = (ftm_calibration_enh_int_dev_cal_dc_result_type*)modem_mem_alloc(
               sizeof(ftm_calibration_enh_int_dev_cal_dc_result_type), MODEM_MEM_CLIENT_RFA);

 if( cal_result_ptr== NULL )
  {
    FTM_MSG( FTM_ERROR, "Failed to allocate memory for Enhanced Internal Device DC Calibration Results!");
    modem_mem_free((void*)dc_result_ptr, MODEM_MEM_CLIENT_RFA);
    return FALSE;
  }
 else
 {
    memset(cal_result_ptr, 0, sizeof(ftm_calibration_enh_int_dev_cal_dc_result_type));
 }

 internal_device_cal_data = ftm_common_data_get_enh_internal_device_cal_ptr();
 
 if (internal_device_cal_data == NULL) 
 {
    FTM_MSG( FTM_ERROR, "Failed to allocate memory for Internal Cal device data!");
    modem_mem_free((void*)dc_result_ptr, MODEM_MEM_CLIENT_RFA);
    modem_mem_free((void*)cal_result_ptr, MODEM_MEM_CLIENT_RFA);
    return FALSE;
 }
  
 //Populate internal_device_cal_data fields from the unpacked param's common header
 //Do a loop for each signal bandwidth

 internal_device_cal_data->internal_device_cal_config.res_ver=data_param->res_ver;
 internal_device_cal_data->internal_device_cal_config.dbg_mode_en=data_param->dbg_mode_en;
 internal_device_cal_data->internal_device_cal_config.flag=data_param->flag;
 internal_device_cal_data->internal_device_cal_config.internal_cal_mask=data_param->internal_cal_type_mask;
   FTM_MSG_1( FTM_MED, "FBRx DC Cal: Internal Cal Mask = %d", data_param->internal_cal_type_mask);
 internal_device_cal_data->internal_device_cal_config.tech=data_param->tech;
   FTM_MSG_1( FTM_MED, "FBRx DC Cal: Tech = %d", data_param->tech);
 internal_device_cal_data->internal_device_cal_config.band=data_param->band;
   FTM_MSG_1( FTM_MED, "FBRx DC Cal: Band = %d", data_param->band);
 internal_device_cal_data->internal_device_cal_config.ref_chan=data_param->ref_chan;
   FTM_MSG_1( FTM_MED, "FBRx DC Cal: Ref Channel = %d", data_param->ref_chan);

   FTM_MSG_1( FTM_MED, "FBRx DC Cal: Num BWs = %d", data_param->num_bw);
 for(sig_bw_iter=0;sig_bw_iter<data_param->num_bw;sig_bw_iter++)
 {
     internal_device_cal_data->internal_device_cal_config.bw=data_param->bw[sig_bw_iter];
     return_val=ftm_enh_internal_device_cal_fb_dc(&data_param->instr.cal_instr_payload_fb_dc_type,&dc_res_size,dc_result_ptr);
     if (return_val== FALSE) 
     {
       FTM_MSG( FTM_ERROR, "Failed to call the ftm_enh_internal_device_cal_fb_dc function successfully !");
       modem_mem_free((void*)dc_result_ptr, MODEM_MEM_CLIENT_RFA);
       modem_mem_free((void*)cal_result_ptr, MODEM_MEM_CLIENT_RFA);
       return FALSE;
     }
     else
     {
         FTM_MSG_1(FTM_MED,"Populating the DC results structure for FBRx bandwidth = %d",
                   internal_device_cal_data->internal_device_cal_config.bw);

         memscpy( &(cal_result_ptr->cal_dc_result[sig_bw_iter].fbrx_dc_res.fbrx_mode_nv),sizeof(uint32),
                  &(dc_result_ptr->fbrx_dc_res.fbrx_mode_nv),sizeof(uint32));
         
         FTM_MSG_1( FTM_MED, "FBRx DC Cal: FBRx Mode = %d", cal_result_ptr->cal_dc_result[sig_bw_iter].fbrx_dc_res.fbrx_mode_nv);

         for (i=0; i < data_param->instr.cal_instr_payload_fb_dc_type.num_fbrx_gain_states;i++) 
         {
           memscpy( &(cal_result_ptr->cal_dc_result[sig_bw_iter].fbrx_dc_res.fbrx_gain_state[i]),
                    sizeof(uint8),&(dc_result_ptr->fbrx_dc_res.fbrx_gain_state[i]),sizeof(uint8));
       
           memscpy( &(cal_result_ptr->cal_dc_result[sig_bw_iter].fbrx_dc_res.dcval[i][0]),
                  sizeof(int32),&(dc_result_ptr->fbrx_dc_res.dcval[i][0]),sizeof(int32));

           memscpy( &(cal_result_ptr->cal_dc_result[sig_bw_iter].fbrx_dc_res.dcval[i][1]),
                  sizeof(int32),&(dc_result_ptr->fbrx_dc_res.dcval[i][1]),sizeof(int32));
          }


      FTM_MSG_1(FTM_MED, "FBRx DC Cal: DC_I = %d", cal_result_ptr->cal_dc_result[sig_bw_iter].fbrx_dc_res.dcval[i][0]);  
     }
 }

   if( return_val == TRUE && dc_result_ptr!=NULL)
  {
    /* Now populate the result buffer from cal_result
       */
    /* Initialize the free size */ 
    free_size = oc_disp->res_buff_info.buff_sz_max;

    /*==========================================================================*/
    /* Populate the results buffer */
    /*==========================================================================*/

    /* Initialize the write pointer */
    res_write_ptr = oc_disp->res_buff_info.buff_ptr;

    /*==========================================================================*/
    /* Populate Results Type */
    /*==========================================================================*/
   if ( free_size >= sizeof(uint8) )
    {
      memscpy( res_write_ptr, sizeof(uint8), &(res_type), sizeof(uint8));
      UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8) );
      free_size -= sizeof(uint8);
     } /* if ( free_size >= sizeof(uint8) ) */
    else
    {
      FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][FBRx DC Cal.{0x%x}] "
                 "ResultPush : ResType - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_ENH_INTERNAL_DEVICE_CALIBRATION,
                 sizeof(uint8),
                 free_size );
    } /* if ( free_size >= sizeof(uint8) ) */

  /*==========================================================================*/
   /* Populate version Type */
   /*==========================================================================*/

   if ( free_size >= sizeof(uint8) )
   {
     memscpy( res_write_ptr, sizeof(uint8), &(version), sizeof(uint8));
     UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8) );
     free_size -= sizeof(uint8);
   }
   else
   {
     FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][FBRx DC Cal.{0x%x}] "
                "ResultPush : Version - ReqSize %d FreeSize %d",
                cal_data->seg_data.cal_instance,
                FTM_CAL_OPCODE_ENH_INTERNAL_DEVICE_CALIBRATION,
                sizeof(uint8),
                free_size );
   } /* if ! ( free_size >= sizeof(uint8) ) */

   /*==========================================================================*/
    /* Populate Results Action */
    /*==========================================================================*/

    if ( free_size >= sizeof(uint8) )
    {
      memscpy( res_write_ptr, sizeof(uint8), &(res_action_type), sizeof(uint8));
       UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8));
      free_size -= sizeof(uint8);
    } /* if ( free_size >= sizeof(uint8) ) */
    else
    {
      FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][FBRx DC Cal.{0x%x}] "
                 "ResultPush : ResType - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 ENH_INTERNAL_DEVICE_CAL_DC_BIT,
                 sizeof(uint8),
                 free_size );
    } /* if ( free_size >= sizeof(uint8) ) */

  //Populate DC results per fbrx nv mode
   for(sig_bw_iter=0;sig_bw_iter<data_param->num_bw;sig_bw_iter++)
 {
    /*==========================================================================*/
    /* Populate FB DC NV mode */
    /*==========================================================================*/
    if ( free_size >= sizeof(uint32) )
    {
      memscpy(res_write_ptr, sizeof(uint32), 
              &(cal_result_ptr->cal_dc_result[sig_bw_iter].fbrx_dc_res.fbrx_mode_nv),
              sizeof(uint32));
      UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint32));
      free_size -= sizeof(uint32);
    }
    else
    {
      FTM_MSG_4(FTM_ERROR, "[calv3][Segment %3d][DC Cal:FBRx NV Mode.{0x%x}]"
                "ResultPush : ResType - ReqSize %d FreeSize %d",
                cal_data->seg_data.cal_instance,
                ENH_INTERNAL_DEVICE_CAL_DC_BIT,
                sizeof(uint32),
                free_size );
    }/* if ( free_size >= sizeof(uint32)) */

     /*==========================================================================*/
    /* Populate Number of FB DC Gain states*/
    /*==========================================================================*/
     if ( free_size >= sizeof(uint8))
      {
        memscpy(res_write_ptr, sizeof(uint8), 
                &(data_param->instr.cal_instr_payload_fb_dc_type.num_fbrx_gain_states),
                sizeof(uint8));
        UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8));
        free_size -= sizeof(uint8);
      }
      else
       {
         FTM_MSG_4(FTM_ERROR, "[calv3][Segment %3d][DC Cal:Number of FB Gain states.{0x%x}]"
                   "ResultPush : ResType - ReqSize %d FreeSize %d",
                   cal_data->seg_data.cal_instance,
                   ENH_INTERNAL_DEVICE_CAL_DC_BIT,
                   sizeof(uint8),
                   free_size );
        }/* if ( free_size >= sizeof(uint8)) */
     
    /*==========================================================================*/
    /* Populate FB DC Gain States and corresponding DC values*/
    /*==========================================================================*/
   
    for (i=0; i < data_param->instr.cal_instr_payload_fb_dc_type.num_fbrx_gain_states;i++)
    {
           if ( free_size >= sizeof(uint8))
           {
              memscpy(res_write_ptr, sizeof(uint8), 
              &(cal_result_ptr->cal_dc_result[sig_bw_iter].fbrx_dc_res.fbrx_gain_state[i]),
              sizeof(uint8));
            UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8));
           free_size -= sizeof(uint8);
           }
           else
           {
             FTM_MSG_5(FTM_ERROR, "[calv3][Segment %3d][DC Cal:FBRx_Gain_State[%d].{0x%x}]"
                "ResultPush : ResType - ReqSize %d FreeSize %d",
                cal_data->seg_data.cal_instance,
                i,
                ENH_INTERNAL_DEVICE_CAL_DC_BIT,
                sizeof(uint8),
                free_size );
           }

           if ( free_size >= sizeof(int32))
           {
              FTM_MSG_3(FTM_MED,"DC Val for Mode %d and Gain state %d = %d",cal_result_ptr->cal_dc_result[sig_bw_iter].fbrx_dc_res.fbrx_mode_nv,cal_result_ptr->cal_dc_result[sig_bw_iter].fbrx_dc_res.fbrx_gain_state[i],cal_result_ptr->cal_dc_result[sig_bw_iter].fbrx_dc_res.dcval[i][0]);
              memscpy(res_write_ptr, sizeof(int32), 
              &(cal_result_ptr->cal_dc_result[sig_bw_iter].fbrx_dc_res.dcval[i][0]),
              sizeof(int32));
            UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(int32));
           free_size -= sizeof(int32);
           }
           else
           {
             FTM_MSG_4(FTM_ERROR, "[calv3][Segment %3d][DC Cal:DC_I].{0x%x}]"
                "ResultPush : ResType - ReqSize %d FreeSize %d",
                cal_data->seg_data.cal_instance,
                ENH_INTERNAL_DEVICE_CAL_DC_BIT,
                sizeof(int32),
                free_size );
           }

           if ( free_size >= sizeof(int32))
           {
              memscpy(res_write_ptr, sizeof(int32), 
              &(cal_result_ptr->cal_dc_result[sig_bw_iter].fbrx_dc_res.dcval[i][1]),
              sizeof(int32));
            UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(int32));
           free_size -= sizeof(int32);
           }
           else
           {
             FTM_MSG_4(FTM_ERROR, "[calv3][Segment %3d][DC Cal:DC_Q].{0x%x}]"
                "ResultPush : ResType - ReqSize %d FreeSize %d",
                cal_data->seg_data.cal_instance,
                ENH_INTERNAL_DEVICE_CAL_DC_BIT,
                sizeof(int32),
                free_size );
           }
    }
    }

    /*==========================================================================*/
    /* Update Result Written Size */
    /*==========================================================================*/
    oc_disp->res_buff_info.buff_sz = written_size;

    if ( cal_data->cal_config.debug_mode_en )
    {
      FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][DATA.{0x%x}] "
                 "ResultPush : %d bytes",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_ENH_INTERNAL_DEVICE_CALIBRATION,
                 written_size );
    }
  }

/* Free allocated memory */
  if( dc_result_ptr != NULL )
  {
    modem_mem_free(dc_result_ptr, MODEM_MEM_CLIENT_RFA);
  }
  if( cal_result_ptr != NULL )
  {
   modem_mem_free(cal_result_ptr, MODEM_MEM_CLIENT_RFA);
  }

return return_val;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Call the Enhanced Internal Devica Cal function for FBRx RSB Cal
 
  @details
  This function is called from common sweep function to perform
  FBRx RSB Cal 
 
  @param cal_data
  Pointer to the data structure containing calibration data\
 
  @param data_param
  Pointer to the data structure containing data unpacked parameter
 
  @param oc_disp
  Pointer to the data structure containing current opcode dispatch data
 
  @return
  Status of opcode processing
*/

boolean 
ftm_enh_internal_device_calibration_process_rsb
( 
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_enh_int_dev_cal_parameter_type const *data_param,
  ftm_cal_opcode_dispatch_info_type *oc_disp
)
{
   uint8 *res_write_ptr = NULL; 
   uint16 written_size = 0; /* Total written size */
   uint16 free_size = 0; /* Total free size */
   uint8 res_type = FTM_CAL_OPCODE_ENH_INTERNAL_DEVICE_CALIBRATION;
   uint8 res_action_type = ENH_INT_DEV_CAL_RES_ACTION_RSB;
   ftm_enh_internal_device_cal_data_type *internal_device_cal_data = NULL;
   uint8 version = 0;
   int32 cal_status_error_code = 0;


   /* Config radio request */
   ftm_cal_data_proc_rsb_req_type calv3_rsb_req;

   if(oc_disp ==NULL)
   {
      return FALSE;
   }
   
   /* Parameter validation */
   if ( data_param == NULL )
   {
    FTM_MSG( FTM_ERROR, "ftm_enh_internal_device_calibration_process_rsb: NULL argument" );
    cal_status_error_code = FTM_ENH_INTERNAL_DEVICE_CAL_BAD_INSTRUCTION;
   } /* if ( data_param == NULL ) */

   if(cal_status_error_code ==0)
   {
     internal_device_cal_data = ftm_common_data_get_enh_internal_device_cal_ptr();
 
     if (internal_device_cal_data == NULL) 
     {
       FTM_MSG( FTM_ERROR, "Failed to allocate memory for Internal Cal device data!");
	   cal_status_error_code = FTM_ENH_INTERNAL_DEVICE_CAL_DATA_POINTER_INVALID;
     }
   }
  
   if(cal_status_error_code ==0)
   {
	 //Populate internal_device_cal_data fields from the unpacked param's common header
	 
	 internal_device_cal_data->internal_device_cal_config.res_ver = data_param->res_ver;
	 internal_device_cal_data->internal_device_cal_config.dbg_mode_en = data_param->dbg_mode_en;
	 internal_device_cal_data->internal_device_cal_config.flag = data_param->flag;
	 internal_device_cal_data->internal_device_cal_config.internal_cal_mask = data_param->internal_cal_type_mask;

     internal_device_cal_data->internal_device_cal_config.device_mask = data_param->device_mask;
     internal_device_cal_data->internal_device_cal_config.band = data_param->band;
     internal_device_cal_data->internal_device_cal_config.bw = data_param->bw[0];
     internal_device_cal_data->internal_device_cal_config.tech = data_param->tech;	 
	 internal_device_cal_data->internal_device_cal_config.ref_chan = data_param->ref_chan;
	 ftm_enh_internal_device_calibration_clear_result_buffer();

	 FTM_MSG_5( FTM_MED, "FBRx/Rx RSB Cal:Process: Internal Cal Mask = %d, Tech:%d, Band:%d, Ref_Chan:%d, Bandwidth:%d", 
	 	data_param->internal_cal_type_mask,data_param->tech,data_param->band,data_param->ref_chan,data_param->bw[0] );

     /* Populate the Request Packet to do mline calculations */
     calv3_rsb_req.cmd = DIAG_SUBSYS_CMD_F;
     calv3_rsb_req.sub_sys = FTM;
     calv3_rsb_req.mode = FTM_COMMON_C;
     calv3_rsb_req.cdma_cmd = FTM_INT_CALV3_RSB_CAL;

     memscpy(&calv3_rsb_req.cal_instr, sizeof(ftm_enh_internal_device_cal_instr_payload_rsb_type),
   	                &data_param->instr.cal_instr_payload_rsb_type, sizeof(ftm_enh_internal_device_cal_instr_payload_rsb_type));
   	
   
     ftm_common_send_radio_command( &calv3_rsb_req,
								  sizeof(ftm_cal_data_proc_rsb_req_type) );

   } 
 
  
  /* Now populate the result buffer from cal_result
     */
  /* Initialize the free size */ 

   free_size = oc_disp->res_buff_info.buff_sz_max;

    /*==========================================================================*/
    /* Populate the results buffer */
    /*==========================================================================*/

    /* Initialize the write pointer */
   res_write_ptr = oc_disp->res_buff_info.buff_ptr;

    /*==========================================================================*/
    /* Populate Results Type */
    /*==========================================================================*/
   if ( free_size >= sizeof(uint8) )
   {
      memscpy( res_write_ptr, sizeof(uint8), &(res_type), sizeof(uint8));
      UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8) );
      free_size -= sizeof(uint8);
    } /* if ( free_size >= sizeof(uint8) ) */
   else
   {
      FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][FBRx/Rx RSB Cal.{0x%x}] "
                 "ResultPush : ResType - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_ENH_INTERNAL_DEVICE_CALIBRATION,
                 sizeof(uint8),
                 free_size );
    } /* if ( free_size >= sizeof(uint8) ) */

  /*==========================================================================*/
   /* Populate version Type */
   /*==========================================================================*/

   if ( free_size >= sizeof(uint8) )
   {
     memscpy( res_write_ptr, sizeof(uint8), &(version), sizeof(uint8));
     UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8) );
     free_size -= sizeof(uint8);
   }
   else
   {
     FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][FBRx /Rx RSB Cal.{0x%x}] "
                "ResultPush : Version - ReqSize %d FreeSize %d",
                cal_data->seg_data.cal_instance,
                FTM_CAL_OPCODE_ENH_INTERNAL_DEVICE_CALIBRATION,
                sizeof(uint8),
                free_size );
   } /* if ! ( free_size >= sizeof(uint8) ) */

   /*==========================================================================*/
    /* Populate Results Action */
    /*==========================================================================*/

    if ( free_size >= sizeof(uint8) )
    {
      memscpy( res_write_ptr, sizeof(uint8), &(res_action_type), sizeof(uint8));
       UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8));
      free_size -= sizeof(uint8);
    } /* if ( free_size >= sizeof(uint8) ) */
    else
    {
      FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][FBRx /Rx RSB Cal.{0x%x}] "
                 "ResultPush : ResType - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 ENH_INTERNAL_DEVICE_CAL_RSB_BIT,
                 sizeof(uint8),
                 free_size );
    } /* if ( free_size >= sizeof(uint8) ) */


  /*==========================================================================*/
   /* Populate Results error_mask */
   /*==========================================================================*/

  if ( free_size >= sizeof(uint32) )
  {

	memscpy( res_write_ptr, sizeof(uint32), &(cal_status_error_code), sizeof(uint32));
	UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint32));
	free_size -= sizeof(uint32);
  } /* if ( free_size >= sizeof(uint32) ) */
  else
  {
	FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][FBRx /Rx RSB Cal Resp, error code.{0x%x}] "
			   "ResultPush : Error_Code - ReqSize %d FreeSize %d",
			   cal_data->seg_data.cal_instance,
			   cal_status_error_code,
			   sizeof(uint32),
			   free_size );
  } /* if ( free_size >= sizeof(uint32) ) */


  /*==========================================================================*/
  /* Update Result Written Size */
  /*==========================================================================*/
  oc_disp->res_buff_info.buff_sz = written_size;

  if ( cal_data->cal_config.debug_mode_en )
  {
    FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][DATA.{0x%x}] "
                 "ResultPush : %d bytes",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_ENH_INTERNAL_DEVICE_CALIBRATION,
                 written_size );
  }
  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process the Rsb Cal Sub Opcode Instruction
 
  @details
  Perform the Rsb Cal Instruction  for the Enhanced FTM RSB cal Opcode.
 
    
  @return
  
*/
ftm_rsp_pkt_type 
ftm_enh_internal_device_calibration_process_rsb_instr
( 
  void *ftm_req_data
)
{
  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */

  ftm_cal_data_proc_rsb_req_type* req_pkt;
  uint16 rsb_res_size = 0;
  ftm_cal_sweep_rsb_proc_rsp_pkt_type *rsp_pkt;
  ftm_enh_internal_device_cal_data_type *internal_device_cal_data = NULL;
  uint8 *rsb_result_ptr = NULL;
  uint16 rsb_validation_length = 0;
  boolean ret_val = FALSE;
  FTM_MSG( FTM_HIGH, "ftm_enh_internal_device_calibration_process_rsb_instr : [Start] ");


  /* Create a new reponse packet in the ftm heap. This will also update first 
  10-bytes field in Packet Payload in the FTM response packet. These fields 
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
  Request Packet Length (2), and Response packet Lenth (2). */
  ftm_rsp_data = ftm_common_malloc_rsp_pkt( ftm_req_data,
                                 sizeof(ftm_cal_sweep_rsb_proc_rsp_pkt_type) ); 

    /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  rsp_pkt = (ftm_cal_sweep_rsb_proc_rsp_pkt_type*)ftm_rsp_data.pkt_payload;

  /* Get Calibration Data */
  internal_device_cal_data = ftm_common_data_get_enh_internal_device_cal_ptr();
  internal_device_cal_data->internal_device_cal_status.cal_status = FTM_ENH_INTERNAL_DEVICE_CAL_PROCESSING;

  if (rsp_pkt == NULL  )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_LEN;
	internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_MEM_ALLOC_FAILURE;
    if( internal_device_cal_data->internal_device_cal_config.dbg_mode_en )
    {
       FTM_MSG( FTM_ERROR, "Cal v3 ftm_enh_internal_device_calibration_process_rsb_instr : Failed "
                         "during Payload malloc" );
    }
    return ftm_rsp_data;
  } /* if (rsp_pkt == NULL  ) */
  req_pkt = (ftm_cal_data_proc_rsb_req_type*)ftm_req_data;
  rsp_pkt->rsb_proc_status = ENH_INT_CAL_PROC_NOT_STARTED;
  
  if((internal_device_cal_data->internal_device_cal_config.flag == 3) || (internal_device_cal_data->internal_device_cal_config.flag == 2))
  {
    if( internal_device_cal_data->internal_device_cal_config.tech == FTM_RF_TECH_LTE)
    {
	  rsb_validation_length = MAX_ENH_INTERNAL_DEVICE_CAL_RSB_VER_LENGTH;
    }
	else
	{
	  rsb_validation_length = WCDMA_TDS_ENH_INTERNAL_DEVICE_CAL_RSB_VER_LENTH ;
	}

	if( internal_device_cal_data->internal_device_cal_result.rsb_calver_buff_ptr != NULL )
	{
		ftm_enh_internal_device_calibration_clear_result_buffer();
	}
	
	internal_device_cal_data->internal_device_cal_result.rsb_calver_buff_ptr= (uint8 *) modem_mem_alloc( rsb_validation_length, MODEM_MEM_CLIENT_RFA );
	
	if( internal_device_cal_data->internal_device_cal_result.rsb_calver_buff_ptr== NULL)
	{
		internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_MEM_ALLOC_FAILURE;
		rsp_pkt->rsb_proc_status = ENH_INT_CAL_PROC_COMPLETE_WITH_FAILURE;
	    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
	    internal_device_cal_data->internal_device_cal_result.rsb_cal_res_len= 0;
        /* Throw Debug messages */
        FTM_MSG( FTM_ERROR, "Cal v3 ftm_enh_internal_device_calibration_process_rsb_instr : Failed " );
	    internal_device_cal_data->internal_device_cal_status.cal_status = FTM_ENH_INTERNAL_DEVICE_CAL_ERROR ;
		return ftm_rsp_data;
	}
	memset(internal_device_cal_data->internal_device_cal_result.rsb_calver_buff_ptr,0,rsb_validation_length);
	internal_device_cal_data->internal_device_cal_result.res_max_len = rsb_validation_length ;
  }
  
  rsb_result_ptr = &internal_device_cal_data->internal_device_cal_result.buff_ptr[0];
  rsp_pkt->rsb_proc_status = ENH_INT_CAL_PROC_IN_PROCESSING;  
  ret_val = ftm_enh_internal_device_cal_rsb( &(req_pkt->cal_instr) ,&rsb_res_size, rsb_result_ptr);
  
  if (ret_val == FALSE)
  {
    rsp_pkt->rsb_proc_status = ENH_INT_CAL_PROC_COMPLETE_WITH_FAILURE;
	ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
	internal_device_cal_data->internal_device_cal_result.rsb_cal_res_len = 0;
    /* Throw Debug messages */
    FTM_MSG( FTM_ERROR, "Cal v3 ftm_enh_internal_device_calibration_process_rsb_instr : Failed " );
	internal_device_cal_data->internal_device_cal_status.cal_status = FTM_ENH_INTERNAL_DEVICE_CAL_ERROR ;
  }
  else
  {
     rsp_pkt->rsb_proc_status = ENH_INT_CAL_PROC_COMPLETE_WITH_SUCCESS;
	 internal_device_cal_data->internal_device_cal_status.cal_status = FTM_ENH_INTERNAL_DEVICE_CAL_COMPLETE ;
	 internal_device_cal_data->internal_device_cal_result.rsb_cal_res_len = rsb_res_size;
     FTM_MSG( FTM_HIGH, "ftm_enh_internal_device_calibration_process_rsb_instr : [Done] ");	
  }
  
  /* Return */
  return ftm_rsp_data;    
}



boolean 
ftm_enh_internal_device_calibration_process_rsb_results
( 
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_enh_int_dev_cal_parameter_type const *data_param,
  ftm_cal_opcode_dispatch_info_type *oc_disp
)
{
   ftm_cal_config_rx_data_type *cfg_rx_data;

   uint8 *res_write_ptr = NULL; 
   uint16 written_size = 0; /* Total written size */
   uint16 free_size = 0; /* Total free size */
   uint8 res_type = FTM_CAL_OPCODE_ENH_INTERNAL_DEVICE_CALIBRATION;
   uint8 res_action_type = ENH_INT_DEV_CAL_RES_ACTION_RETURN_RSB_RESULTS ;
   ftm_enh_internal_device_cal_data_type *internal_device_cal_data = NULL;
   uint8 *rsb_result_ptr = NULL;
   uint8 version = 0;
   int32 cal_status_error_code = 0;

   uint16 required_buf_len = 0;

   cfg_rx_data = &(cal_data->seg_data.oc_payload.config_rx);

   cfg_rx_data->rx_prof_in_progress = FALSE;
   FTM_MSG(FTM_MED,"ENH INT CAL: FBRx ENH_INT_DEV_CAL_RSB_GET_RESULT:Process, rx_prof_in_process set to false");

   if(cal_status_error_code ==0)
   {
     internal_device_cal_data = ftm_common_data_get_enh_internal_device_cal_ptr();
     if (internal_device_cal_data == NULL) 
     {
	   cal_status_error_code = FTM_ENH_INTERNAL_DEVICE_CAL_DATA_POINTER_INVALID;
     }
   }

   if(oc_disp ==NULL)
   {
	  FTM_MSG( FTM_ERROR, "ENH INT CAL: FBRx ENH_INT_DEV_CAL_RSB_GET_RESULT:Process: NULL oc_disp" );
	  ftm_enh_internal_device_calibration_clear_result_buffer();

	  return FALSE;
   }
   
   /* Parameter validation */
   if ( data_param == NULL )
   {
    FTM_MSG( FTM_ERROR, "ENH INT CAL: FBRx ENH_INT_DEV_CAL_RSB_GET_RESULT:Process: NULL argument" );
    cal_status_error_code = FTM_ENH_INTERNAL_DEVICE_CAL_BAD_INSTRUCTION;
   } /* if ( data_param == NULL ) */


   if(cal_status_error_code ==0)
   {
	 //Populate internal_device_cal_data fields from the unpacked param's common header
	 if (internal_device_cal_data->internal_device_cal_status.cal_status ==FTM_ENH_INTERNAL_DEVICE_CAL_COMPLETE )
	 {
	   rsb_result_ptr = &internal_device_cal_data->internal_device_cal_result.buff_ptr[0];
	   cal_status_error_code = internal_device_cal_data->internal_device_cal_status.error_log;	 
	 }
	 else if(internal_device_cal_data->internal_device_cal_status.cal_status == FTM_ENH_INTERNAL_DEVICE_CAL_PROCESSING)
	 {
	   cal_status_error_code = FTM_ENH_INTERNAL_DEVICE_CAL_STILL_PROCESSING ; 
	 }
	 else
	 {	   
	   cal_status_error_code = internal_device_cal_data->internal_device_cal_status.error_log ;	   
	 }
   } 
   
  /* Now populate the result buffer from cal_result
     */
  /* Initialize the free size */ 

   free_size = (oc_disp->res_buff_info.buff_sz_max < cal_data->result.buff_size)? 
                 oc_disp->res_buff_info.buff_sz_max : cal_data->result.buff_size ;

    /*==========================================================================*/
    /* Populate the results buffer */
    /*==========================================================================*/

    /* Initialize the write pointer */
   res_write_ptr = oc_disp->res_buff_info.buff_ptr;

    /*==========================================================================*/
    /* Populate Results Type */
    /*==========================================================================*/
   if ( free_size >= sizeof(uint8) )
   {
      memscpy( res_write_ptr, sizeof(uint8), &(res_type), sizeof(uint8));
      UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8) );
      free_size -= sizeof(uint8);
    } /* if ( free_size >= sizeof(uint8) ) */
   else
   {
      FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][FBRx/Rx RSB Cal.{0x%x}] "
                 "ResultPush : ResType - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_ENH_INTERNAL_DEVICE_CALIBRATION,
                 sizeof(uint8),
                 free_size );
    } /* if ( free_size >= sizeof(uint8) ) */

  /*==========================================================================*/
   /* Populate version Type */
   /*==========================================================================*/

   if ( free_size >= sizeof(uint8) )
   {
     if( internal_device_cal_data->internal_device_cal_result.rsb_cal_res_len == sizeof(ftm_enh_internal_device_cal_rsb_result_type))
     {
 		version = 1;
     }
	 memscpy( res_write_ptr, sizeof(uint8), &(version), sizeof(uint8));
     UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8) );
     free_size -= sizeof(uint8);
   }
   else
   {
     FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][FBRx /Rx RSB Cal.{0x%x}] "
                "ResultPush : Version - ReqSize %d FreeSize %d",
                cal_data->seg_data.cal_instance,
                FTM_CAL_OPCODE_ENH_INTERNAL_DEVICE_CALIBRATION,
                sizeof(uint8),
                free_size );
   } /* if ! ( free_size >= sizeof(uint8) ) */

   /*==========================================================================*/
    /* Populate Results Action */
    /*==========================================================================*/

    if ( free_size >= sizeof(uint8) )
    {
      memscpy( res_write_ptr, sizeof(uint8), &(res_action_type), sizeof(uint8));
       UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8));
      free_size -= sizeof(uint8);
    } /* if ( free_size >= sizeof(uint8) ) */
    else
    {
      FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][FBRx /Rx RSB Cal.{0x%x}] "
                 "ResultPush : ResType - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 ENH_INTERNAL_DEVICE_CAL_RSB_BIT,
                 sizeof(uint8),
                 free_size );
    } /* if ( free_size >= sizeof(uint8) ) */

  //=========recompute the needed buffer to copy results
  required_buf_len = sizeof(uint16)+ sizeof(uint32); //flag size, error_code
  if( internal_device_cal_data->internal_device_cal_config.flag != 3)
  {
	  required_buf_len += internal_device_cal_data->internal_device_cal_result.rsb_cal_res_len;
  }
  if( ( internal_device_cal_data->internal_device_cal_config.flag ==2 ) || 
	  (internal_device_cal_data->internal_device_cal_config.flag ==3) )
  {
	required_buf_len += internal_device_cal_data->internal_device_cal_result.rsb_cal_ver_data_len;
  }
  
  if(required_buf_len > free_size)
  {
    cal_status_error_code = FTM_ENH_INTERNAL_DEVICE_CAL_RES_OVERSIZE;
  }

  /*==========================================================================*/
   /* Populate Results error_mask */
   /*==========================================================================*/

  if ( free_size >= sizeof(uint32) )
  {

	memscpy( res_write_ptr, sizeof(uint32), &(cal_status_error_code), sizeof(uint32));
	UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint32));
	free_size -= sizeof(uint32);
  } /* if ( free_size >= sizeof(uint32) ) */
  else
  {
	FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][FBRx /Rx RSB Cal Resp, error code.{0x%x}] "
			   "ResultPush : Error_Code - ReqSize %d FreeSize %d",
			   cal_data->seg_data.cal_instance,
			   cal_status_error_code,
			   sizeof(uint32),
			   free_size );
  } /* if ( free_size >= sizeof(uint8) ) */

  if( cal_status_error_code == 0 )
  {
    if( free_size >= sizeof(uint16) )
    {
	  memscpy( res_write_ptr, sizeof(uint16), &(internal_device_cal_data->internal_device_cal_config.flag), sizeof(uint16));
	  UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint16));
	  free_size -= sizeof(uint16);		
    }
    // Populate Rx /FBRx RSB cal results
	if ((free_size>= internal_device_cal_data->internal_device_cal_result.rsb_cal_res_len ) && (rsb_result_ptr!=NULL))
	{
	  if( internal_device_cal_data->internal_device_cal_result.rsb_cal_res_len !=0)
	  {
	     memscpy(res_write_ptr, internal_device_cal_data->internal_device_cal_result.rsb_cal_res_len, 
			  rsb_result_ptr, internal_device_cal_data->internal_device_cal_result.rsb_cal_res_len );
	     UPDATE_SIZE_N_PTR( written_size, res_write_ptr, internal_device_cal_data->internal_device_cal_result.rsb_cal_res_len);
		 free_size -= internal_device_cal_data->internal_device_cal_result.rsb_cal_res_len;
	  }
	  
	  if((internal_device_cal_data->internal_device_cal_config.flag == 3) || (internal_device_cal_data->internal_device_cal_config.flag == 2))
	  {
         if( free_size >= internal_device_cal_data->internal_device_cal_result.rsb_cal_ver_data_len )
         {
		   memscpy(res_write_ptr, internal_device_cal_data->internal_device_cal_result.rsb_cal_ver_data_len, 
				 internal_device_cal_data->internal_device_cal_result.rsb_calver_buff_ptr, internal_device_cal_data->internal_device_cal_result.rsb_cal_ver_data_len);
		   UPDATE_SIZE_N_PTR( written_size, res_write_ptr, internal_device_cal_data->internal_device_cal_result.rsb_cal_ver_data_len);
		   free_size -= internal_device_cal_data->internal_device_cal_result.rsb_cal_ver_data_len;
		   FTM_MSG_2(FTM_MED,"calv3_enh_internal_device_cal, rsb, copied verification length: %d, free_length: %d", internal_device_cal_data->internal_device_cal_result.rsb_cal_ver_data_len, free_size);
         }
	     else
	     {
	        FTM_MSG_3(FTM_ERROR, "[calv3][Segment %3d][RSB Cal payload]"
				  "ResultPush : Result - ReqSize %d FreeSize %d",
				  cal_data->seg_data.cal_instance,
				  internal_device_cal_data->internal_device_cal_result.rsb_cal_res_len,
				  free_size );
	      }		 
	  }
	}
	
	else
	{
	  FTM_MSG_3(FTM_ERROR, "[calv3][Segment %3d][RSB Cal payload]"
				  "ResultPush : Result - ReqSize %d FreeSize %d",
				  cal_data->seg_data.cal_instance,
				  internal_device_cal_data->internal_device_cal_result.rsb_cal_res_len,
				  free_size );

	}
  }
  else
  {
    FTM_MSG_1( FTM_ERROR, "ENH INT CAL: FBRx ENH_INT_DEV_CAL_RSB_GET_RESULT:Process failed, error_code :%d", cal_status_error_code);   
  }

  /*==========================================================================*/
  /* Update Result Written Size */
  /*==========================================================================*/
  oc_disp->res_buff_info.buff_sz = written_size;

  //if ( cal_data->cal_config.debug_mode_en )
  {
    FTM_MSG_3( FTM_MED, "[calv3][Segment %3d][DATA.{0x%x}] "
                 "ResultPush : %d bytes",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_ENH_INTERNAL_DEVICE_CALIBRATION,
                 written_size );
  }
  ftm_enh_internal_device_calibration_clear_result_buffer();
 
  return TRUE;
}


