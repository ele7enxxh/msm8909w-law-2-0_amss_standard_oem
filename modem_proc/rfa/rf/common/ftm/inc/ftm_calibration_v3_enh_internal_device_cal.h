
#ifndef FTM_CALIBRATION_V3_ENH_INTERNAL_DEVICE_CAL_H
#define FTM_CALIBRATION_V3_ENH_INTERNAL_DEVICE_CAL_H
/*!
  @file
  ftm_calibration_v3_data.h

  @brief
  This module contains prototypes and definitions used by common RF calibration
  code.
*/

/*==============================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_calibration_v3_enh_internal_device_cal.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
07/07/14   xsh   RSB cal opcode implementation update to solve rsb cal time out of max allowed esc segment
06/12/14   xsh   Add FBRx/Rx RSB Cal with Opcode
05/26/14    id     Fix for FBRx DC Cal functionality with Opcode
05/21/14   brath    Add enum for max possible NV modes,Update result structure for FBRx DC,Remove instruction size from payload.
04/23/14   brath     Initial version

==============================================================================*/

#include "comdef.h"
#include "ftm_common_xpt.h"
#include "rfcommon_mdsp_types.h"
#include "fw_dpd_intf.h"
#include "ftm_enh_internal_device_cal_fb_dc.h"
#include "ftm_enh_internal_device_cal_rsb.h"
#include "ftm_common_enh_internal_device_cal.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Maximum allowed Data opcode results size in bytes for the current segment.
*/

#define FTM_CAL_V3_INVALID_LTE_BW 255
#define FTM_CAL_V3_FBRX_NUM_TX_CONFIG_MAX 14
#define FTM_CAL_V3_FBRX_NUM_NV_MODES 4

/*============================================================================*/

/*!
  @brief
  Enumeration of all Enh Int Dev Cal Actions.
*/
typedef enum
{
  ENH_INT_DEV_CAL_FBDROOP         = 0x1,
  /*!< Perform FBRx Droop Cal */

  ENH_INT_DEV_CAL_GRP_DELAY       = 0x2,
  /*!< Perform Tx BBF Group Delay Cal */

  ENH_INT_DEV_CAL_IIP2            = 0x4,
  /*!< Perform Rx IIP2 Cal */
    
  ENH_INT_DEV_CAL_FBDC            = 0x8,
  /*!< Perform FBRx DC Cal */

  ENH_INT_DEV_CAL_RSB             = 0x10, 
  /*!< Perform RSB Cal */

  ENH_INT_DEV_CAL_RSB_GET_RESULT  = 0x100, 
  /*!< Request RSB Data */  

} ftm_enh_int_dev_cal_action_type;

typedef enum
{
  ENH_INT_DEV_CAL_RES_ACTION_FBDROOP = 0,
  ENH_INT_DEV_CAL_RES_ACTION_GROUP_DELAY = 1,
  ENH_INT_DEV_CAL_RES_ACTION_IP2 = 2,
  ENH_INT_DEV_CAL_RES_ACTION_DC = 3,  
  ENH_INT_DEV_CAL_RES_ACTION_RSB = 4,
  ENH_INT_DEV_CAL_RES_ACTION_RETURN_RSB_RESULTS = 5,
  ENH_INT_DEV_CAL_RES_ACTION_MAX = 6
} ftm_enh_int_dev_cal_res_action_enum_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM Common Dispatch Request Packet to be used for Rsb Calibration for
  calibration v3 sweep
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16  mode;
  /*!< RF Mode Identification. For Instance - 0 for FTM_RFM_C */

  uint16  cdma_cmd;
  /*!< FTM Sub Command. Example-  FTM_CDMA_RFM_1X_WAKEUP_RX */

  ftm_enh_internal_device_cal_instr_payload_rsb_type cal_instr;
  /*!< FTM Rsb Cal Payload Ptr */

} ftm_cal_data_proc_rsb_req_type;

/*============================================================================*/

/*!
  @brief
  Enumeration of all Enh Int Dev Cal Processing status
*/
typedef enum
{
  ENH_INT_CAL_PROC_NOT_STARTED, 
  ENH_INT_CAL_PROC_IN_PROCESSING,
  ENH_INT_CAL_PROC_COMPLETE_WITH_SUCCESS,
  ENH_INT_CAL_PROC_COMPLETE_WITH_FAILURE
}ftm_enh_int_dev_cal_process_status_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM Rsb Cal Proc sweep Response packet
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16 mode;
  /*!< RF Mode Identification. For Instance - 0 for FTM_COMMON_C */

  uint16 cdma_cmd;
  /*!< FTM Sub Command. Example-  FTM_CALIBRATION_SWEEP */

  uint16 req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16 rsp_len;
  /*!< Length of response packet to Diag/PC */

  ftm_enh_int_dev_cal_process_status_type rsb_proc_status;  // 0, not_started, 1, on-processing; 2: done with success; 3: done with failure
  /*!< Rsb Proc Status */

} ftm_cal_sweep_rsb_proc_rsp_pkt_type;



typedef union
{
  ftm_enh_internal_device_cal_instr_payload_fb_dc_type cal_instr_payload_fb_dc_type;
  ftm_enh_internal_device_cal_instr_payload_rsb_type cal_instr_payload_rsb_type;
}ftm_enh_internal_device_cal_instr_payload;


/*!
  @brief
  Structure used to hold the unpacked data opcode parameter
*/
  typedef struct
{
  /* Indicates the results revision, 0 for now */
  uint8 res_ver; 

  /* Indicates if the debug mode is enabled or not */
  uint8 dbg_mode_en; 

  /* flag, open for future usuage */
  uint16 flag;    

  /* tech */  
  uint8  tech;        
  
  /* band */ 
  uint16 band;        

  /* reference channel */
  uint32  ref_chan;     

  /* Indicating the internal cal type mask */
  ftm_enh_internal_device_cal_mask_type internal_cal_type_mask;  

  /* Number of fbrx signal bandwidth */
  uint8 num_bw;         

  /* fbrx signal bandwidths */
    uint8 bw[FTM_CAL_V3_FBRX_NUM_TX_CONFIG_MAX];   
 
  /* Device Mask */ 
  uint16 device_mask;

  ftm_enh_internal_device_cal_instr_payload instr; 

}ftm_cal_enh_int_dev_cal_parameter_type;

typedef struct
{
 ftm_enh_internal_device_cal_fb_dc_result_type cal_dc_result[FTM_CAL_V3_FBRX_NUM_NV_MODES];
}ftm_calibration_enh_int_dev_cal_dc_result_type;


/*----------------------------------------------------------------------------*/
/*! Structure to hold data pertaining to Radio Retune OpCode */
typedef struct
{
  uint8 *res_buff;
  /*!< 
  @brief 
  Temporary Data result buffer used to store calibration results for 
  the current segment. 

  @details 
  This buffer will be reused in every segment. The purpose 
  of this buffer to pre-allocate the enough memory to store the maximum 
  possible data results for a given segment. When data operation 
  is called, pointer to this buffer is sent; so that the corresponding 
  data function can fill the calibration results. At the 
  end of the segment, the data in this buffer will be pushed to results 
  buffer. 

  This buffer is allocated during Calibration configuration, and deallocated 
  at the end of the sweep. 
  */

  ftm_cal_enh_int_dev_cal_parameter_type up_param;
  /*!< Structure holding all the unpacked payload for Data opcode */

} ftm_cal_enh_int_cal_opcode_data_type;

/*----------------------------------------------------------------------------*/
boolean 
ftm_enh_internal_device_calibration_process_dc
( 
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_enh_int_dev_cal_parameter_type const *data_param,
  ftm_cal_opcode_dispatch_info_type *oc_disp
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type 
ftm_enh_internal_device_calibration_process_rsb_instr
( 
  void *ftm_req_data
);

/*----------------------------------------------------------------------------*/
boolean 
ftm_enh_internal_device_calibration_process_rsb
( 
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_enh_int_dev_cal_parameter_type const *data_param,
  ftm_cal_opcode_dispatch_info_type *oc_disp
);

/*----------------------------------------------------------------------------*/
boolean 
ftm_enh_internal_device_calibration_process_rsb_results
( 
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_enh_int_dev_cal_parameter_type const *data_param,
  ftm_cal_opcode_dispatch_info_type *oc_disp
);


/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_enh_internal_device_cal_init
(
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_enh_internal_device_cal_deinit
(
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
uint16
ftm_calibration_enh_internal_device_cal_unpack
( 
  struct ftm_calibration_data_type_t *cal_data,
  struct ftm_cal_opcode_dispatch_info_type_t *oc_disp,
  void const *instr_payload
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_enh_internal_device_cal_process
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_dispatch_info_type *oc_disp
);


/*----------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif /* FTM_CALIBRATION_V3_ENH_INTERNAL_DEVICE_CAL_H */


