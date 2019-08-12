#ifndef RFLM_CCS_RF_EVENT_LOG_H
#define RFLM_CCS_RF_EVENT_LOG_H

/*!
  @file
  rflm_ccs_rf_event_log.h

  @brief
  Provides API to program RF script during IRAT for all techs
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
 
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/api/ccs/rflm_ccs_rf_event_log.h#1 $ 
$Date: 2016/12/13 $   
when       who     what, where, why 
--------   ---     ------------------------------------------------------------- 
06/23/14   ljl     Initial version
==============================================================================*/
#include "rflm_types.h"
#include "rflm_ccs_rf_event_intf.h"

#ifdef __cplusplus
extern "C" {
#endif


/*----------------------------------------------------------------------------*/
/*! @brief RFLM FBRx log packet ID */
#define RFLM_CCS_RF_EVENT_LOG_PACKET_ID                0x1843
#define RFLM_CCS_RF_EVENT_LOG_PACKET_RAT               0
#define RFLM_CCS_RF_EVENT_LOG_PACKET_HEADER_SZ_BYTES   4

/*----------------------------------------------------------------------------*/
/*! @brief  CCS event time check Log packet structure Version 1 */

typedef struct
{
  uint32  event_id;             /*!< Event ID */
  uint32  priority  :  4;       /*!< Priority of the event/task when event is atomic */
  uint32  tq_idx    :  4;       /*!< Task queue index */
  uint32  task_idx  :  7;       /*!< Task index */
  uint32  late      :  1;       /*!< Event late flag */
  uint32  num_trans : 16;       /*!< Number of Total Transactions */
  uint32  exp_finish_time;
  uint32  deadline_time;
  uint32  actual_end_time;
  uint32  tag0;
  uint32  tag1;
}rflm_ccs_rf_event_time_log_packet_v1;
  

/*----------------------------------------------------------------------------*/
/*!  @brief CCS event time log packet structure */

/// @log_id 0x1849
typedef struct
{
  uint8  log_version;              /*!< Log Packet Version */
  uint8  num_sub_packets;          /*!< number of sub packets */  
  uint16 reserved_bits;
  /// @descriminator log_version
  union  rflm_ccs_rf_event_time_log_packet_versions
  {
    /// @condition 1
    rflm_ccs_rf_event_time_log_packet_v1 v1;   /*!< Log Packet Version 1 */
  } versions;

}rflm_ccs_rf_event_time_log_packet_t;


/*----------------------------------------------------------------------------*/
/*!@brief Enumeration indicating the FBRx Log Packet version */
typedef enum
{
  RFLM_CCS_RF_EVENT_TIME_LOG_VER_1 = 1, /*!< FBRx Log Packet Version 1 */
} rflm_ccs_rf_event_time_log_version_type;


/*----------------------------------------------------------------------------*/
/*!@brief diat log subpacket */
void
rflm_ccs_rf_event_time_check_diag_log_subpacket
(
  rflm_ccs_rf_event_header_task_type *header_task_ptr,
  uint8 tq_idx,
  uint8 task_idx,
  rflm_ccs_rf_event_time_chk_t * evt_chk_ptr,
  uint32 actual_end_time,
  boolean evt_late
);


/*----------------------------------------------------------------------------*/
/*!@brief diat log subpacket initialization */
void
rflm_ccs_rf_event_time_check_diag_init_log_subpacket
(
  void
);


/*----------------------------------------------------------------------------*/
/*!@brief diat log subpacket de-allocation */
void
rflm_ccs_rf_event_time_check_diag_deallocate_log_subpacket
(
  void
);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif  /*RFLM_CCS_RF_EVENT_LOG_H*/

