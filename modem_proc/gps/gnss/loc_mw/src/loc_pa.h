/******************************************************************************
  @file:  loc_pa.h
  @brief: Location Middleware PA module header

  DESCRIPTION
      This module configures/retrieves the NV setting of location
  engine on the modem, and processes EFS data.

  INITIALIZATION AND SEQUENCING REQUIREMENTS

  -----------------------------------------------------------------------------
  Copyright (c) 2009 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2015 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  -----------------------------------------------------------------------------
 ******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      -------------------------------------------------------
10/06/15   yh       QZSS
01/01/09   wc/etc   Initial version and development

$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_pa.h#1 $
$DateTime: 2016/12/13 07:59:45 $
======================================================================*/

#ifndef LOC_PA_H

#define LOC_PA_H


#include "loc_api_2.h"
#include "loc_task.h"
#include "aries_os_api.h"

/* Union of information that PA-module needs to remember for IOCTLs it 
   is servicing */
typedef union 
{
  loc_fs_operation_e_type   efs_operation;  /* Store the file operation */
  qmiLocSetPositionEngineConfigParametersReqMsgT_v02  
                               setPeCfg;/* Store the request 
                               to set PeConfig */
  qmiLocGetPositionEngineConfigParametersReqMsgT_v02  
                               getPeCfg;/* Store the request 
                               to get PeConfig */                                
} loc_pa_module_ioctl_req_union_type;

typedef struct 
{
   boolean                   pa_cmd_is_in_progress;
   pdsm_pa_e_type            pa_cmd;              // PA CMD type
   pdsm_pa_cmd_e_type        pa_cmd_type;         // PA_CMD_TYPE_READ or PA_CMD_TYPE_WRITE
   
   loc_ioctl_e_type          pa_iotcl_type;
   loc_client_handle_type    pa_client_handle;
   pdsm_pa_mo_method_e_type  mo_method; // 0: CP, 1: UP
   os_TimerMsgType *         pa_process_timer;

   loc_pa_module_ioctl_req_union_type pa_ioctl_req_info; /* Info regarding the IOCTL Req 
                                      being processed */
} loc_pa_module_data_s_type;

/* -----------------------------------------------------------------------*//**
@brief
   Mapping of masks from QMI-Loc Delete Masks to PDAPI Delete Masks. 
   should be removed when locQmiShimConvToLocDeleteAssistData is removed from
   loc_qmi_shim.c
*//* ------------------------------------------------------------------------*/
typedef struct
{
  uint32        q_qmi_mask;
  uint32        q_pdapi_mask;
}loc_pa_mask_map_s_type;

//Note: the next 2 macros are for locQmiShimConvToLocDeleteAssistData compile purpose
//remove them after deprecate DeleteAssistData
#define LOC_PA_CLOCK_INFO_MASK_LEN (21)
#define LOC_PA_CELL_DB_MASK_LEN    (10)

extern loc_pa_mask_map_s_type z_QmiPdsmClockInfoMaskMap[LOC_PA_CLOCK_INFO_MASK_LEN];

extern loc_pa_mask_map_s_type z_QmiPdsmCellDBMaskMap[LOC_PA_CELL_DB_MASK_LEN];

extern boolean loc_pa_init (void);

extern int loc_pa_process_ioctl 
(     
      loc_client_handle_type       client_handle,
      loc_ioctl_e_type             ioctl_type,
      const loc_ioctl_data_u_type* ioctl_data
);

extern int loc_pa_xlate_server_addr_from_locapi_to_pdapi 
(  
      const loc_server_info_s_type* locapi_server_addr_ptr,
      pdsm_server_address_s_type*   pdapi_server_addr_ptr
);

extern int loc_pa_xlate_server_addr_from_pdapi_to_locapi 
(  
      const pdsm_server_address_s_type*  pdapi_server_addr_ptr,
      loc_server_info_s_type*            locapi_server_addr_ptr
);

extern void loc_pa_process_timer_cb
(
      void
);


#endif // LOC_PA_H
