#ifndef RFLM_CCS_IRAT_INTF_H
#define RFLM_CCS_IRAT_INTF_H

/*!
  @file
  rf_event_intf.h

  @brief
  Types for RF CCS events inside shared memory
 
*/

/*===========================================================================
Copyright (c) 2012 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/api/ccs/rflm_ccs_irat_intf.h#1 $
$DateTime: 2016/12/13 07:59:54 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/23/14   pl     Increase number of Cleanup RFFE transactions from 30 to 60 for setup.
08/23/14   pl     Increase number of Cleanup RFFE transactions from 50 to 100 for cleanup.
08/07/14   rmb    Increase number of RFFE transactions from 30 to 50 for cleanup.
03/21/14   ra     Support FW cleanup of deprecated files
12/02/13   pl     Move definitions to IRAT header files
08/01/13   ra     Initial version
============================================================================*/

#include "rflm_ccs_task_defs.h"
#include "rflm_rf_event_intf.h"
#include "rflm_ccs_rf_event_intf.h"

/* Extended RFFE writes for Trigger and preload, see CCS enqueue function for details */
#define RFLM_CCS_RF_IRAT_PRELOAD_TRANS_SIZE_BYTES 8
#define RFLM_CCS_RF_IRAT_TRNSCVR_TRANS_SIZE_BYTES 8
/* Normal SSBI and RFFE writes for non-TRX, see CCS enqueue function for details */
#define RFLM_CCS_RF_IRAT_SSBI_TRANS_SIZE_BYTES 2
#define RFLM_CCS_RF_IRAT_RFFE_TRANS_SIZE_BYTES 2
#define RFLM_CCS_RF_IRAT_GRFC_TRANS_SIZE_BYTES 3

#define EXE_TIME_PER_SSBI_TRX_TRANSACTION 2.3
#define TASK_SWITCHING_TIME 10
#define RFLM_CCS_RF_IRAT_SIZEOF_CCS_HEADER_BYTES   (((sizeof(rflm_ccs_rf_event_description_buffer_header_type)+3)>>2)<<2)
#define RFLM_CCS_RF_IRAT_SIZEOF_CCS_TASK_BYTES     (((sizeof(ccs_rf_task_buf_item_t)+3)>>2)<<2)
#define RFLM_CCS_RF_IRAT_SIZEOF_EVENT_HEADER_BYTES (RFLM_CCS_RF_IRAT_SIZEOF_CCS_HEADER_BYTES+(RFLM_CCS_RF_MAX_NUM_CCS_TASKS*RFLM_CCS_RF_IRAT_SIZEOF_CCS_TASK_BYTES))

#define RFLM_CCS_IRAT_MAX_NUM_OF_SBI 300
/* Should match with RFLM_MEAS_MAX_NON_TRNSCVR_TRANS in rflm_meas.h */
#define RFLM_CCS_IRAT_MAX_NUM_OF_RFFE 60
/* Should match with RFLM_MEAS_MAX_TB_NON_TRNSCVR_TRANS in rflm_meas.h */
#define RFLM_CCS_IRAT_MAX_NUM_OF_RFFE_CLEANUP 100

#define RFLM_CCS_IRAT_MAX_NUM_OF_GRFC  44
/*! @brief Task buffer size (for 14 tasks) = 
  sizeof(rf_ccs_event_description_buffer_header_type)/4 +
  RFMEAS_MAX_NUM_CCS_TASKS*sizeof(rf_ccs_event_task_description_type)/4 */  
#define RFLM_CCS_IRAT_SCRIPT_TASK_BUF_SIZE (sizeof(rflm_ccs_rf_event_description_buffer_header_type)/4 + \
                                         RFMEAS_MAX_NUM_CCS_TASKS*sizeof(rf_ccs_event_task_description_type)/4)
                                                                         
#define RFLM_CCS_IRAT_SCRIPT_DATA_BUF_SIZE_BYTE (RFLM_CCS_IRAT_MAX_NUM_OF_SBI*4 + \
                                                 RFLM_CCS_IRAT_MAX_NUM_OF_RFFE*2)

#define RFLM_CCS_IRAT_SCRIPT_DATA_BUF_SIZE_WORDS ((RFLM_CCS_IRAT_SCRIPT_DATA_BUF_SIZE_BYTE + 3)>>2)

#ifndef FW_RF_IRAT_EVENT_INTF_H
/*!
  @brief
  Data structure for passing event based RF script to FWs.
*/
typedef struct
{
  /*! @brief Header and tasks for the event */
  rflm_ccs_rf_event_header_task_type header_task;
  /*! @brief Data buffer for the tasks  */
  uint32 data_buf[RFLM_CCS_IRAT_SCRIPT_DATA_BUF_SIZE_WORDS];
} ALIGN(4) rfmeas_event_infor_type;

/*! @brief Define IRAT buffer event handle for callback installation */ 
typedef struct 
{
  uint32 num_events;      /*!< Number of events for this buffer */
  uint32 scratch_mem[2];  /*!< Scratch memory for FW to use per script */
} rfmeas_irat_buf_event_handle_type;

/*! @brief Define IRAT script as an array of size RFLM_CCS_NUM_IRAT_SCRIPT_EVENT 
  each of type rfmeas_event_infor_type */
typedef struct 
{
  rfmeas_event_infor_type script[RFLM_CCS_NUM_IRAT_SCRIPT_EVENT];
  rfmeas_irat_buf_event_handle_type buf_event_handle;
} rfmeas_irat_buf_type;
#endif


/*! @brief MACRO for converting buffer id to script address. Any module wanting
 * to use this API must also include modem_fw_memmap.h */
#define RFLM_CCS_IRAT_SCRIPT_ADDR(buf_id) \
  (FW_SMEM_IRAT_SCRIPT_ADDR + ((buf_id)*sizeof(rfmeas_irat_buf_type)))

/*! @brief MACRO for converting buffer id to script offset. Currently SW and 
 * FW memory maps are aligned and the shared memory is idempotent, in
 * case that changes, SW can still use just the offset API */
#define RFLM_CCS_IRAT_SCRIPT_OFF(buf_id) \
  ((buf_id)*sizeof(rfmeas_irat_buf_type))

/*! @brief Define IRAT script buffer as an array of size RFLM_CCS_NUM_IRAT_SCRIPT_BUFS 
  each of type rfmeas_irat_buf_type */
typedef struct 
{
  rfmeas_irat_buf_type buf[RFLM_CCS_NUM_IRAT_SCRIPT_BUFS];
  rfmeas_event_infor_type rfcmd_event;
} rflm_ccs_irat_script_infor_type;


#endif /* RFLM_CCS_IRAT_INTF_H*/

