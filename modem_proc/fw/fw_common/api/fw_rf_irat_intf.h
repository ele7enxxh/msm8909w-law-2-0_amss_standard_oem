/*!
  @file
  fw_rf_irat_intf.h

  @brief
  Generic interface for RF script for IRAT

*/

/*===========================================================================

  Copyright (c) 2012-2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/06/13   BW       Increase IRAT script max number of RFFE for supporting 
                    UMTS wakeup tuning script size increase
06/13/13   BW       Increase IRAT buffer size for T2L connected mode
04/18/13   BW       Increase IRAT script task buffer size
01/09/13   NB       Add brackets for mathematical expression in macro
10/15/12   BW       Support IRAT script with two event
09/26/12   NB       Adjust number of RFFE and adjust size for SSBI_ACCESS
07/30/12   NB       Added event for RFSW for non-tech FW events.
06/08/12   NB       Rename for clarity about number of CCS tasks 
05/22/12   NB       Event based RF script interface 
===========================================================================*/

#ifndef FW_RF_IRAT_EVENT_INTF_H
#define FW_RF_IRAT_EVENT_INTF_H

#include "rf_event_intf.h"

/*===========================================================================
                      RF script format        
===========================================================================*/

/*! @brief Number of IRAT scripts 2 for startup, 2 for cleanup, 
    and 2 for on-demand TQs, last 3 for T2L connected mode  */
#define FW_RF_NUM_IRAT_SCRIPT_BUFS  9 

/*! @brief Number of event for every IRAT script */
#define FW_RF_NUM_IRAT_SCRIPT_EVENT 2
   
/*! @brief NUmber of SBI' addr/ data pairs in the buffer */
#define RFMEAS_MAX_NUM_OF_SBI    300      

/*! @brief Max number of CCS tasks */
#define RFMEAS_MAX_NUM_CCS_TASKS 14

/*! @brief NUmber of RFFE' addr/ data pairs in the buffer */
#define RFMEAS_MAX_NUM_OF_RFFE   30      

/*! @brief Task buffer size (for 14 tasks) = 
  sizeof(rf_ccs_event_description_buffer_header_type)/4 +
  RFMEAS_MAX_NUM_CCS_TASKS*sizeof(rf_ccs_event_task_description_type)/4 */
#define FW_RF_IRAT_SCRIPT_TASK_BUF_SIZE (sizeof(rf_ccs_event_description_buffer_header_type)/4 + \
                                         RFMEAS_MAX_NUM_CCS_TASKS*sizeof(rf_ccs_event_task_description_type)/4)

#define FW_RF_IRAT_SCRIPT_DATA_BUF_SIZE_BYTE (RFMEAS_MAX_NUM_OF_SBI*4 + \
                                              RFMEAS_MAX_NUM_OF_RFFE*2)

#define FW_RF_IRAT_SCRIPT_DATA_BUF_SIZE_WORDS ((FW_RF_IRAT_SCRIPT_DATA_BUF_SIZE_BYTE + 3)>>2)

/*! @brief Structure containing header and tasks for any event */
typedef struct 
{
  /*! @brief Header of event  */
  rf_ccs_event_description_buffer_header_type header;

  /*! @brief Tasks for the event */
  rf_ccs_event_task_description_type task_buf[RFMEAS_MAX_NUM_CCS_TASKS];
} rfmeas_event_header_task_type;


#ifndef RFLM_CCS_IRAT_INTF_H

/*!
  @brief
  Data structure for passing event based RF script to FWs.
*/
typedef struct
{
  /*! @brief Header and tasks for the event */
  rfmeas_event_header_task_type header_task;
  /*! @brief Data buffer for the tasks  */
  uint32 data_buf[FW_RF_IRAT_SCRIPT_DATA_BUF_SIZE_WORDS];
} ALIGN(4) rfmeas_event_infor_type;

/*! @brief Define IRAT buffer event handle for callback installation */ 
typedef struct 
{
  uint32 num_events;      /*!< Number of events for this buffer */
  uint32 scratch_mem[2];  /*!< Scratch memory for FW to use per script */
} rfmeas_irat_buf_event_handle_type;

/*! @brief Define IRAT script as an array of size FW_RF_NUM_IRAT_SCRIPT_EVENT 
  each of type rfmeas_event_infor_type */
typedef struct 
{
  rfmeas_event_infor_type script[FW_RF_NUM_IRAT_SCRIPT_EVENT];
  rfmeas_irat_buf_event_handle_type buf_event_handle;
} rfmeas_irat_buf_type;

#endif // RFLM_CCS_IRAT_INTF_H

/*! @brief MACRO for converting buffer id to script address. Any module wanting
 * to use this API must also include modem_fw_memmap.h */
#define FW_IRAT_SCRIPT_ADDR(buf_id) \
  (FW_SMEM_IRAT_SCRIPT_ADDR + ((buf_id)*sizeof(rfmeas_irat_buf_type)))

/*! @brief MACRO for converting buffer id to script offset. Currently SW and 
 * FW memory maps are aligned and the shared memory is idempotent, in
 * case that changes, SW can still use just the offset API */
#define FW_IRAT_SCRIPT_OFF(buf_id) \
  ((buf_id)*sizeof(rfmeas_irat_buf_type))

/*! @brief Define IRAT script buffer as an array of size FW_RF_NUM_IRAT_SCRIPT_BUFS 
  each of type rfmeas_irat_buf_type */
typedef struct 
{
  rfmeas_irat_buf_type buf[FW_RF_NUM_IRAT_SCRIPT_BUFS];
  rfmeas_event_infor_type rfcmd_event;
} rfmeas_irat_script_infor_type;

#endif

