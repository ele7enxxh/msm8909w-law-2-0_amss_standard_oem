#ifndef FTM_COMMON_CONCURRENCY_MANAGER_H
#define FTM_COMMON_CONCURRENCY_MANAGER_H
/*!
   @file
   ftm_common_concurrency_manager.h

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
Copyright (c) 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved. 

                       EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: 

when       who     what, where, why
--------------------------------------------------------------------------------
11/19/14   ck      Add tech protection to ftm ccmgr cleanup
11/10/14   kg      Add ftm api for setting concurrency manager debug mode
10/22/14   ck      Add cleanup api  
10/21/14   ck      Add parameter type for ftm conmgr
09/30/14   ck      Initial revision 
 
==============================================================================*/
typedef struct
{
  rfm_device_enum_type rfm_dev;
  rfcom_band_type_u band;
  rfm_mode_enum_type tech;
  rfc_rx_path_type prx_drx;
  uint8 carrier_index; // 0 for primary 1 for secondary 
}ftm_concurrency_manager_params_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Concurrency Manager Dispatch Debug Mode Request Packet
*/
typedef PACK(struct)
{
  uint8   cmd;                /*!< Command Identification */
  uint8   sub_sys;            /*!< Sub-system Identification. For instance - 10 for FTM HWTC */
  uint16  mode;               /*!< RF Mode Identification. For Instance - 20 for FTM_COMMON_C */
  uint16  ftm_cmd_id;         /*!< FTM Sub Command. Example-  FTM_CNCRY_MGR_CONFIGURE_DEBUG_MODE */
  uint16  req_len;            /*!< Length of Diag Request Packet to the Target */
  uint16  rsp_len;            /*!< Length of response packet to Diag/PC */
  uint8   debug_en;           /*!< Debug Enable */
  uint32  debug_mask;         /*!< Debug Mask */
  uint32  reserved;           /*!< Reserved*/
}
ftm_concurrency_mgr_debug_mode_request_packet_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Concurrency Manager Dispatch Debug Mode Request Packet
*/
typedef PACK(struct)
{
  uint8   cmd;            /*!< Command Identification */
  uint8   sub_sys;        /*!< Sub-system Identification. For instance - 10 for FTM HWTC */
  uint16  mode;           /*!< RF Mode Identification. For Instance - 20 for FTM_COMMON_C */
  uint16  ftm_cmd_id;     /*!< FTM Sub Command. Example-  FTM_CNCRY_MGR_CONFIGURE_DEBUG_MODE */
  uint16  req_len;        /*!< Length of Diag Request Packet to the Target */
  uint16  rsp_len;        /*!< Length of response packet to Diag/PC */
  uint32  ftm_concurrency_mgr_status;   /*!< Status */
}
ftm_concurrency_mgr_debug_mode_response_packet_type;
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                          FUNCTION PROTOTYPE                                */ 
/*----------------------------------------------------------------------------*/

boolean ftm_concurrency_manager_validate_operation( ftm_concurrency_manager_params_type *params );

boolean ftm_concurrency_manager_cleanup_device_allocation( rfm_device_enum_type rfm_dev, rfm_mode_enum_type cur_tech);

ftm_rsp_pkt_type ftm_concurrency_manager_configure_debug_mode(ftm_common_pkt_type *ftm_req_data);

#endif
