/*!
  @file
  rflm_cmn_asd.h

  @brief
  RFLM common antenna switch diversity header file
  
  @detail
  Provides the following API's for TECH RFLM:
  * rflm_cmn_asd_command - to submit ASD RF event
  * rflm_cmn_asd_init
  * rflm_cmn_asd_mark_pending
  * rflm_cmn_asd_rf_event_pointer_get
  * rflm_cmn_asd_storage_size_get
  * rflm_cmn_asd_status_get
  
  Usage example:
  * Populate RF event (associated with token=token_val) 
    // get asd_handle
    asd_handle = ...
  
    // get pointer to ASDiv RF event
    uint32 *ptr = rflm_cmn_asd_rf_event_pointer_get( asd_handle );  
    
    // update RF event starting at &ptr[0]
    ...
    
    // indicate there is pending ASDiv RF event
    rflm_cmn_asd_mark_pending( asd_handle, token_val );

  * Submit to CCS
     // get asd_handle
    asd_handle = ...
    
    // submit to CCS
    rflm_cmn_asd_command( trig_time, 
                          ccs_dest_ptr, 
                          ccs_dst_length_bytes, 
                          ccs_tq, 
                          asd_handle,
                          tech_id );
 
*/

/*===========================================================================

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/rftech/common/inc/rflm_cmn_asd.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
2014-04-24   hdz     Change to RFLM_x definition
2014-04-24   hdz     Added void rflm_cmn_asd_abort_pending_event( uint32 handle_id )
2014-01-30   jc      ported from modem Dime FW (fw_rf.c)

===========================================================================*/

#ifndef RFLM_CMN_ASD_H
#define RFLM_CMN_ASD_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "rflm_ccs_rf_intf.h"
//#include "rflm_ccs_rf.h"
#include "rf_hal_ccs.h"

/*===========================================================================

 Preprocessor Definitions and Constants

===========================================================================*/
/*! @brief Max number of CCS tasks in the ASD event */
#define RFLM_ASD_MAX_NUM_CCS_TASKS 16

/*! @brief Maximum size of ASD script buffer size in word */
#define RFLM_ASD_SCRIPT_DATA_BUF_SIZE_WORDS 32

/*===========================================================================

 Type Declarations

===========================================================================*/

/*! @brief Structure containing header and tasks for ASD event */
typedef struct 
{
  /*! @brief Header of event  */
  rflm_ccs_rf_event_description_buffer_header_type header;

  /*! @brief Tasks for the event */
  rf_ccs_event_task_description_type task_buf[RFLM_ASD_MAX_NUM_CCS_TASKS];
} rflm_cmn_rf_asd_event_header_task_type;


/*! @brief Data structure for storing ASD event  */
typedef struct
{
  /*! @brief Header and tasks for the ASD event */
  rflm_cmn_rf_asd_event_header_task_type header_task;
  /*! @brief Data buffer for the tasks  */
  uint32 data_buf[RFLM_ASD_SCRIPT_DATA_BUF_SIZE_WORDS];
} ALIGN(4) rflm_cmn_rf_asd_event_infor_type;


/*! @brief ASD script status information enum. */
typedef enum
{
  /*! @brief This value indicates SW abort the script and no action from FW */
  RFLM_CMN_RF_ASD_CMD_ABORTED,
  /*! @brief This value indicates an outstanding ASD update in FW */
  RFLM_CMN_RF_ASD_CMD_PENDING,
  /*! @brief This value indicates that FW has commited the event for ASD */
  RFLM_CMN_RF_ASD_CMD_COMMITTED
} rflm_cmn_asd_cmd_status_e; 


/*! @brief Opaque struct pointer to asd info  */
typedef struct rflm_cmn_rf_asd_info_type *rflm_cmn_asd_script_handle_t;


/*===========================================================================

 Function prototypes

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

extern 
rflm_ccs_event_info_s rflm_cmn_asd_command(
  /*!< Start time of script, if time needs to be immediate, 
    pass FW_RF_EVENT_TIME_IMMEDIATE */
  uint32 start_offset,
  /*!< CCS desination memory for RFFE addr/data information. 
    This is pointer in CCS memory */
  uint32 *ccs_dest_ptr,
  /*!< Length of destination CCS memory in bytes */
  uint32 length_bytes,
  /*!< TQ for programming QTF script RFFE tasks */
  rflm_ccs_rf_tq_handle_t *tq,
  /*!< pointer to ASDiv info */  
  rflm_cmn_asd_script_handle_t asd_cfg_ptr,  
  /*!< ASD Tech ID */
  uint16 tech );


extern 
void rflm_cmn_asd_init( rflm_cmn_asd_script_handle_t asd_info_ptr );

extern 
void rflm_cmn_asd_mark_pending( rflm_cmn_asd_script_handle_t asd_info_ptr, uint32 token );

extern 
uint32 * rflm_cmn_asd_rf_event_pointer_get( rflm_cmn_asd_script_handle_t asd_info_ptr );

extern 
uint32 rflm_cmn_asd_storage_size_get( void );

extern
void rflm_cmn_asd_status_set_abort( rflm_cmn_asd_script_handle_t asd_handle );

extern 
void rflm_cmn_asd_status_get( rflm_cmn_asd_script_handle_t asd_handle,
                              rflm_cmn_asd_cmd_status_e     *script_status, 
                              rflm_ccs_rf_event_status_type *ccs_evt_status );

extern boolean rflm_cmn_asd_allocate_memory( uint32 handle_id );

extern boolean rflm_cmn_asd_deallocate_memory( uint32 handle_id );

extern void rflm_cmn_asd_abort_pending_event( uint32 handle_id );

#ifdef __cplusplus
}  // extern "C"
#endif 


#endif /* RFLM_CMN_ASD_H */
