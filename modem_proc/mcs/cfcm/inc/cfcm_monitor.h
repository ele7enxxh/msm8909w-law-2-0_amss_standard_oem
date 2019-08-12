/*!
  @file
  cfcm_monitor.h

  @brief
  CFCM monitor related header file .

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

  @author
  rohitj

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

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cfcm/inc/cfcm_monitor.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
12/15/14   sg      QSH related modifications
11/21/14   rj      Adding MDM_TEMP and VDD_PEAK_CURR monitors
10/27/14   rj      Adding QTF support for CFCM
08/11/14   rj      Adding support for QSH in CFCM
07/03/14   rj      BUS BW changes
04/07/14   rj      initial version
==============================================================================*/

#ifndef CFCM_MONITOR_H
#define CFCM_MONITOR_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include "cfcm.h"

/* QMI Files */
#include "mqcsi_conn_mgr.h"
#include "qmi_si.h"
#include "qmi_idl_lib.h"
#include "common_flow_control_management_v01.h"
#include "cfcm_msg_internal.h"

/*==============================================================================

                   INTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*! @brief whether Bus Bandwidth Level is Valid
*/
#define CFCM_IS_BUS_BW_LEVEL_VALID(l)  (((l) >= QMI_CFCM_BUS_BW_NO_THROTTLE_V01) && \
                                       ((l) <= QMI_CFCM_BUS_BW_CRITICAL_THROTTLE_V01))

#define CFCM_MONITOR_THERMAL_PA_NPA_NODE      "/therm/mitigate/pa"
#define CFCM_MONITOR_THERMAL_RA_NPA_NODE      "/therm/mitigate/cx_vdd_limit"
#define CFCM_MONITOR_MDM_TEMP_NPA_NODE        "/therm/mitigate/modem"
#define CFCM_MONITOR_VDD_PEAK_CURR_NPA_NODE   "/therm/mitigate/modem_current"

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

extern void cfcm_monitor_init( void );

/*=============================================================================

  FUNCTION:  cfcm_monitor_deinit

=============================================================================*/
/*!
    @brief
        De-Initializes CFCM QMI layer's functionalities
 
    @return
        None
*/
/*===========================================================================*/
void cfcm_monitor_deinit 
(
  void
);

extern void cfcm_monitor_proc_reg
(
  const cfcm_reg_req_type_s*    msg_ptr
);

extern void cfcm_monitor_proc_dereg
(
  const cfcm_dereg_req_type_s*   msg_ptr,
  uint32                       monitor_mask
);

extern void cfcm_monitor_compute_fc_cmd
(
  cfcm_cmd_type_s*             fc_cmd_ptr,
  uint32                       monitor_mask,
  cfcm_client_e client   /*!< the client id */
);

extern void cfcm_monitor_proc_update
(
  cfcm_monitor_ind_msg_s*       msg_ptr
);

extern boolean cfcm_monitor_registered
( 
  cfcm_monitor_e monitor
);

/*==============================================================================

  FUNCTION:  cfcm_monitor_large_pool_dl_event_cb

==============================================================================*/
/*!
    @brief
    callback function registered with large dsm_pool indicating when memory is 
    low.

    @return
    None
*/
/*============================================================================*/
EXTERN void cfcm_monitor_large_pool_dl_event_cb
(
  dsm_mempool_id_enum_type pool_id, /*!< pool triggered the event */
  dsm_mem_level_enum_type  event,   /*!< level reached */
  dsm_mem_op_enum_type     op       /*!< operation that triggered the event */
);

/*==============================================================================

  FUNCTION:  cfcm_monitor_large_pool_ul_event_cb

==============================================================================*/
/*!
    @brief
    callback function registered with large dsm_pool indicating when memory is 
    low.

    @return
    None
*/
/*============================================================================*/
EXTERN void cfcm_monitor_large_pool_ul_event_cb
(
  dsm_mempool_id_enum_type pool_id, /*!< pool triggered the event */
  dsm_mem_level_enum_type  event,   /*!< level reached */
  dsm_mem_op_enum_type     op       /*!< operation that triggered the event */
);

/*==============================================================================

  FUNCTION:  cfcm_monitor_small_pool_dl_event_cb

==============================================================================*/
/*!
    @brief
    callback function registered with small dsm_pool indicating when memory is 
    low.

    @return
    None
*/
/*============================================================================*/
EXTERN void cfcm_monitor_small_pool_dl_event_cb
(
  dsm_mempool_id_enum_type pool_id, /*!< pool triggered the event */
  dsm_mem_level_enum_type  event,   /*!< level reached */
  dsm_mem_op_enum_type     op       /*!< operation that triggered the event */
);

/*==============================================================================

  FUNCTION:  cfcm_monitor_dup_pool_dl_event_cb

==============================================================================*/
/*!
    @brief
    callback function registered with dup dsm_pool indicating when memory is 
    low.

    @return
    None
*/
/*============================================================================*/
EXTERN void cfcm_monitor_dup_pool_dl_event_cb
(
  dsm_mempool_id_enum_type pool_id, /*!< pool triggered the event */
  dsm_mem_level_enum_type  event,   /*!< level reached */
  dsm_mem_op_enum_type     op       /*!< operation that triggered the event */
);

/*==============================================================================

  FUNCTION:  cfcm_monitor_dup_pool_ul_event_cb

==============================================================================*/
/*!
    @brief
    callback function registered with dup dsm_pool indicating when memory is 
    low.

    @return
    None
*/
/*============================================================================*/
EXTERN void cfcm_monitor_dup_pool_ul_event_cb
(
  dsm_mempool_id_enum_type pool_id, /*!< pool triggered the event */
  dsm_mem_level_enum_type  event,   /*!< level reached */
  dsm_mem_op_enum_type     op       /*!< operation that triggered the event */
);

/*==============================================================================

  FUNCTION:  cfcm_monitor_large_pool_ipa_dl_event_cb

==============================================================================*/
/*!
    @brief
    callback function registered with large dsm_pool indicating when memory is 
    low.

    @return
    None
*/
/*============================================================================*/
EXTERN void cfcm_monitor_large_pool_ipa_dl_event_cb
(
  dsm_mempool_id_enum_type pool_id, /*!< pool triggered the event */
  dsm_mem_level_enum_type  event,   /*!< level reached */
  dsm_mem_op_enum_type     op       /*!< operation that triggered the event */
);

/*=============================================================================

  FUNCTION:  cfcm_qmi_process_req

=============================================================================*/
/*!
    @brief
        Callback function called by QCSI infrastructure when a REQ message to
        CFCM is received
 
    @note
    	QCSI infrastructure decodes the data before forwarding it to this layer
 
    @return
   	 qmi_csi_cb_error
*/
/*===========================================================================*/

qmi_csi_cb_error cfcm_qmi_process_req (
  void           *connection_handle,
  qmi_req_handle  req_handle,
  unsigned int    msg_id,
  void           *req_c_struct,
  unsigned int    req_c_struct_len,
  void           *service_cookie
);

/*==============================================================================

  FUNCTION:  cfcm_monitor_get_cmd_string

==============================================================================*/
/*!
    @brief
    Returns the string associated with a flow control command.
*/
/*============================================================================*/
extern const char * cfcm_monitor_get_cmd_string
(
  cfcm_cmd_e cmd 
);

/*==============================================================================

  FUNCTION:  cfcm_monitor_qsh_analysis

==============================================================================*/
/*!
    @brief
    Performs QSH analysis on this module.
*/
/*============================================================================*/
extern void cfcm_monitor_qsh_analysis
(
  void
);

/*==============================================================================

  FUNCTION:  cfcm_monitor_qsh_reset

==============================================================================*/
/*!
    @brief
    Clears the statistics.
*/
/*============================================================================*/
extern void cfcm_monitor_qsh_reset
(
  void
);


/*===========================================================================

                                UNIT TEST

===========================================================================*/


void cfcm_test_thermal_monitor_via_diag
(
  uint8 input 
);

void cfcm_test_dsm_monitor_via_diag
(
  uint8 input 
);

#endif /* CFCM_MONITOR_H */

