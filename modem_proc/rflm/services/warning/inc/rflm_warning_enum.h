
/*!
  @file
  rflm_warning_enum.h

  @brief
  warning enumeration macro list

  @detail
  This file specifies RFLM warning id's.  X-macros in rflm_warn.h are then 
  used to auto generate enums, data structures, initialization
  values and code.
 
*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

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
#ifndef RFLM_WARNING_ENUM_H
#define RFLM_WARNING_ENUM_H

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/services/warning/inc/rflm_warning_enum.h#1 $
$Date: 2016/12/13 $ 
when         who     what, where, why
--------   ---     ----------------------------------------------------------- 
09/22/14   rca     Added enum for SCRUTINY task trigger time (GT 1.7ms in future)
04/08/14   jyu     Added enum for TDS slot_update 
02/13/14   bsh     Check action time passed by FW before scheduling LNA update
12/16/13   Saul    CCS. Increase number of tasks per TQ to 32.
11/09/13   ra      Support RFLM macros 
===========================================================================*/


/*===========================================================================

      Macros

===========================================================================*/


/* ! @brief
   This macro is used to generate run-time warning enums and data structure.
   (warning enum will have RFLM_WARNING prepended to the warning name)

   First entry item is the warning name (used for enum and string within
   the warning data structure). Second entry item is mask specifying action
   to take when the warning occurs.  
    TRACE  = internal FW trace message logging.  
    ASSERT = runtime assert failure.  
    MSG    = QXDM message logging 
   
   Warnings should be recoverable implementation related faults 
   such as:
     - missed processing deadlines
     - recoverable HW stalls
     - invalid & ignored commands
     - dropped logging submissions
     etc
   
   Count of warning occurrences are maintained.
   
   To indicate warning, use RFLM_WARNING( < warning name>, msg, ...) macro
   To view warning data structure, use v.v rflm_warning_data in T32   

   Example usage:
   
     if ( msgr_send( ...) != E_SUCCESS )
        RFLM_WARNING( RFLM_WARNING_MSGR_SEND_FAILED, "for message xyz" );
   
*/   
#define RFLM_CONFIG_WARNING_TABLE( entry ) \
  entry( RFLM_WARNING_DM_INVALID_HANDLE,                    					TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_DM_SCRIPT_BUF_ALLOC_FAILED,           					TRACE_EN|ASSERT_EN|MSG_EN    ) \
  entry( RFLM_WARNING_DM_SCRIPT_BUF_LOCATE_FAILED,           					TRACE_EN|ASSERT_EN|MSG_EN    ) \
  entry( RFLM_WARNING_DM_DEALLOC_NULL_SCRIPT_BUF_PTR,           			    TRACE_DIS|ASSERT_DIS|MSG_DIS    ) \
  entry( RFLM_WARNING_DM_TECH_BUF_ALLOC_FAILED,           					    TRACE_EN|ASSERT_DIS|MSG_EN    ) \
  entry( RFLM_WARNING_DM_HANDLE_TECH_DATA_ALLOC_FAILED,           				TRACE_EN|ASSERT_DIS|MSG_EN    ) \
  entry( RFLM_WARNING_DM_HANDLE_TECH_DATA_DEALLOC_FAILED,           			TRACE_EN|ASSERT_DIS|MSG_EN    ) \
  entry( RFLM_WARNING_DM_TECH_BUF_DEALLOC_NULL_PTR,           					TRACE_EN|ASSERT_DIS|MSG_EN    ) \
  entry( RFLM_WARNING_DM_HANDLE_TECH_DATA_NULL_PTR,           					TRACE_EN|ASSERT_DIS|MSG_EN    ) \
  entry( RFLM_WARNING_DM_INVALID_HANDLE_TECH_DATA_PTR, 					        TRACE_EN|ASSERT_DIS|MSG_EN    ) \
  entry( RFLM_WARNING_DM_INVALID_DTR_RX_PTR,           					        TRACE_EN|ASSERT_DIS|MSG_EN    ) \
  entry( RFLM_WARNING_DM_INVALID_DTR_TX_PTR,           					        TRACE_EN|ASSERT_DIS|MSG_EN    ) \
  entry( RFLM_WARNING_DM_DTR_RX_NULL_PTR,           					        TRACE_EN|ASSERT_DIS|MSG_EN    ) \
  entry( RFLM_WARNING_DM_DTR_TX_NULL_PTR,           					        TRACE_EN|ASSERT_DIS|MSG_EN    ) \
  entry( RFLM_WARNING_DM_TECH_BUF_INVALID_PTR,           					    TRACE_EN|ASSERT_DIS|MSG_EN    ) \
  entry( RFLM_WARNING_DM_INVALID_SCRIPT_BUF_PTR,           					    TRACE_EN|ASSERT_DIS|MSG_EN    ) \
  entry( RFLM_WARNING_DM_SCRIPT_INVALID_OWNER_TYPE,         			        TRACE_EN|ASSERT_DIS|MSG_EN    ) \
  entry( RFLM_WARNING_DM_SCRIPT_MULTI_ASSOCIATE_SAME_HANDLE,         			TRACE_EN|ASSERT_DIS|MSG_EN    ) \
  entry( RFLM_WARNING_DM_SCRIPT_MULTI_ASSOCIATE_ACROSS_HANDLES,         	    TRACE_EN|ASSERT_DIS|MSG_EN    ) \
  entry( RFLM_WARNING_DM_SCRIPT_DISASSOCIATE_DIFF_SCRIPT_BUF,         			TRACE_EN|ASSERT_EN|MSG_EN    ) \
  entry( RFLM_WARNING_DM_SCRIPT_DUP_DISASSOCIATE,         				        TRACE_EN|ASSERT_DIS|MSG_EN    ) \
  entry( RFLM_WARNING_DM_SCRIPT_DEALLOC_INVALID_BUF,           					TRACE_EN|ASSERT_EN|MSG_EN    ) \
  entry( RFLM_WARNING_DM_SCRIPT_ASSOCIATE_COUNTER_NOT_ZERO,           			TRACE_EN|ASSERT_DIS|MSG_EN    ) \
  entry( RFLM_WARNING_DM_SCRIPT_BUF_MIN_SZ_ALLOC_FAILED,    					TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_DM_SCRIPT_BUF_CONCURRENT_ACCESS,    						TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_DM_SCRIPT_BUF_OVERFLOW,               					TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_DM_SCRIPT_BUF_OVERFLOW_PADDING,               			TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_DM_SCRIPT_BUF_OVERFLOW_BARRIER_OVERWRITTEN,               TRACE_EN|ASSERT_EN|MSG_EN   ) \
  entry( RFLM_WARNING_DM_TECH_BUF_BARRIER_OVERWRITTEN,                          TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_DM_HANDLE_TECH_DATA_BARRIER_OVERWRITTEN,                  TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_DM_DTR_RX_BUF_BARRIER_OVERWRITTEN,                        TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_DM_DTR_TX_BUF_BARRIER_OVERWRITTEN,                        TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_DM_HANDLE_ALLOC_FAILED,               					TRACE_EN|ASSERT_EN|MSG_EN   ) \
  entry( RFLM_WARNING_DM_HANDLE_REASSOCIATE_SCRIPT_FAILED,               		TRACE_EN|ASSERT_EN|MSG_EN   ) \
  entry( RFLM_WARNING_DM_HANDLE_DEALLOC_FAILED,               					TRACE_EN|ASSERT_EN|MSG_EN   ) \
  entry( RFLM_WARNING_DM_DTR_TX_BUF_ALLOC_FAILED,              					TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_DM_DTR_TX_BUF_DEALLOC_FAILED,              				TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_DM_DTR_RX_BUF_ALLOC_FAILED,              					TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_DM_DTR_RX_BUF_DEALLOC_FAILED,              				TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_DM_TXAGC_MEM_ALLOC_FAILED,              				    TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_DM_TXAGC_MEM_DEALLOC_FAILED,              				TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_DM_RXAGC_MEM_ALLOC_FAILED,              				    TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_DM_RXAGC_MEM_DEALLOC_FAILED,              				TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_DM_FBRX_MEM_ALLOC_FAILED,              				    TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_DM_FBRX_MEM_DEALLOC_FAILED,              				    TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_DM_TECH_DATA_MEM_ALLOC_FAILED,              				TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_MSGR_SEND_FAILED,                     					TRACE_EN|ASSERT_DIS|MSG_DIS  ) \
  entry( RFLM_WARNING_DIAG_LOG_DROP_PKT,                    					TRACE_EN|ASSERT_DIS|MSG_DIS  ) \
  entry( RFLM_WARNING_CCS_SEQ_ERR_MON_SETUP,                					TRACE_EN|ASSERT_DIS|MSG_DIS  ) \
  entry( RFLM_WARNING_CCS_ERROR,                					            TRACE_EN|ASSERT_EN|MSG_DIS  ) \
  entry( RFLM_WARNING_CCS_TASK_PREEMPTED_ERR,                					TRACE_EN|ASSERT_DIS|MSG_DIS  ) \
  entry( RFLM_WARNING_CCS_TQ_IMMINENT_OVERFLOW_P1,          					TRACE_EN|ASSERT_DIS|MSG_DIS ) \
  entry( RFLM_WARNING_CCS_TQ_IMMINENT_OVERFLOW_P2,          					TRACE_EN|ASSERT_DIS|MSG_DIS ) \
  entry( RFLM_WARNING_CCS_TQ_IMMINENT_OVERFLOW_P3,          					TRACE_EN|ASSERT_DIS|MSG_DIS ) \
  entry( RFLM_WARNING_CCS_GSM_EVT_TIME_NOT_CHK,                                 TRACE_DIS|ASSERT_DIS|MSG_DIS ) \
  entry( RFLM_WARNING_CCS_UMTS_EVT_TIME_NOT_CHK,                                TRACE_DIS|ASSERT_DIS|MSG_DIS ) \
  entry( RFLM_WARNING_CCS_C2K_EVT_TIME_NOT_CHK,                                 TRACE_DIS|ASSERT_DIS|MSG_DIS ) \
  entry( RFLM_WARNING_CCS_LTE_EVT_TIME_NOT_CHK,                                 TRACE_DIS|ASSERT_DIS|MSG_DIS ) \
  entry( RFLM_WARNING_CCS_TDS_EVT_TIME_NOT_CHK,                                 TRACE_DIS|ASSERT_DIS|MSG_DIS ) \
  entry( RFLM_WARNING_CCS_UNKNOWN_EVT_TIME_NOT_CHK,                             TRACE_DIS|ASSERT_DIS|MSG_DIS ) \
  entry( RFLM_WARNING_CCS_GSM_EVT_TIME_LATE,                                    TRACE_EN|ASSERT_DIS|MSG_DIS ) \
  entry( RFLM_WARNING_CCS_UMTS_EVT_TIME_LATE,                                   TRACE_EN|ASSERT_DIS|MSG_DIS ) \
  entry( RFLM_WARNING_CCS_C2K_EVT_TIME_LATE,                                    TRACE_EN|ASSERT_DIS|MSG_DIS ) \
  entry( RFLM_WARNING_CCS_LTE_EVT_TIME_LATE,                                    TRACE_EN|ASSERT_DIS|MSG_DIS ) \
  entry( RFLM_WARNING_CCS_TDS_EVT_TIME_LATE,                                    TRACE_EN|ASSERT_DIS|MSG_DIS ) \
  entry( RFLM_WARNING_CCS_LONG_USR_CALLBACK,                                    TRACE_EN|ASSERT_DIS|MSG_DIS ) \
  entry( RFLM_WARNING_CCS_UNKNOWN_EVT_TIME_LATE,                                TRACE_EN|ASSERT_DIS|MSG_DIS ) \
  entry( RFLM_WARNING_CCS_EVT_TIMING_CHK_CONFIG_ERR,                            TRACE_EN|ASSERT_DIS|MSG_DIS ) \
  entry( RFLM_WARNING_LTE_BUFFER_OVERFLOW,                  					TRACE_EN|ASSERT_DIS|MSG_DIS  ) \
  entry( RFLM_WARNING_LTE_LATE_TXAGC_CALL,                   					TRACE_EN|ASSERT_DIS|MSG_EN  ) \
  entry( RFLM_WARNING_IRAT_TRIGGER_PRELOAD_DIFF_PRIORITY,   					TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_PRELOAD_TRIGGER_MISMATCH,        					TRACE_EN|ASSERT_EN|MSG_EN    ) \
  entry( RFLM_WARNING_IRAT_TRIGGER_ON_UNINIT_BUF,           					TRACE_EN|ASSERT_EN|MSG_EN    ) \
  entry( RFLM_WARNING_IRAT_PRELOAD_DIFF_PRIORITY_LOCK_BUF,  					TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_TRIGGER_LOCK_BUF,                					TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_INVALID_SCRIPT_BUF,              					TRACE_EN|ASSERT_EN|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_EMPTY_RESOURCE,                  					TRACE_EN|ASSERT_EN|MSG_EN    ) \
  entry( RFLM_WARNING_IRAT_INVALID_EVENT_TYPE,              					TRACE_EN|ASSERT_EN|MSG_EN    ) \
  entry( RFLM_WARNING_IRAT_INVALID_HANDLE,                  					TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_NO_POLICY_CHECK,                 					TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_INVALIDATE_RESOURCE_WITH_PENDING_CMD,            	TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_OVERWRITE_RESOURCE_WITH_PENDING_CMD,            		TRACE_EN|ASSERT_EN|MSG_EN    ) \
  entry( RFLM_WARNING_IRAT_PRELOAD_ISR_ON_RES_STATE_UNINIT,      				TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_TRIGGER_ISR_ON_RES_STATE_UNINIT,      				TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_TRIGGER_ISR_ON_RES_STATE_PRELOAD_ENQ, 				TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_PRELOAD_ISR_ON_RES_STATE_PRELOAD_DONE, 				TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_TRIGGER_ISR_ON_RES_STATE_PRELOAD_DONE, 				TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_PRELOAD_ISR_ON_RES_STATE_TRIGGER_ENQ, 				TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_PRELOAD_ISR_ON_RES_STATE_TRIGGER_DONE, 				TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_TRIGGER_ISR_ON_RES_STATE_TRIGGER_DONE, 				TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_TRIGGER_ISR_ON_RES_STATE_PRELOAD_ENQ_TRIGGER_ENQ, 	TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_PRELOAD_ISR_ON_RES_STATE_PRELOAD_DONE_TRIGGER_ENQ, 	TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_NO_USER_EVENT_LATE,             					    TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_GSM_TB_TRIGGER_LATE,             					TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_GSM_TA_TRIGGER_LATE,             					TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_C2K_TB_TRIGGER_LATE,             					TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_C2K_TA_TRIGGER_LATE,             					TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_LTE_TB_TRIGGER_LATE,             					TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_LTE_TA_TRIGGER_LATE,             					TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_UMTS_TB_TRIGGER_LATE,            					TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_UMTS_TA_TRIGGER_LATE,            					TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_TDS_TB_TRIGGER_LATE,             					TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_TDS_TA_TRIGGER_LATE,             					TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_EVDO_TB_TRIGGER_LATE,            					TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_EVDO_TA_TRIGGER_LATE,            					TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_GSM_TB_PRELOAD_LATE,             					TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_GSM_TA_PRELOAD_LATE,             					TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_C2K_TB_PRELOAD_LATE,             					TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_C2K_TA_PRELOAD_LATE,             					TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_LTE_TB_PRELOAD_LATE,             					TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_LTE_TA_PRELOAD_LATE,             					TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_UMTS_TB_PRELOAD_LATE,            					TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_UMTS_TA_PRELOAD_LATE,            					TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_TDS_TB_PRELOAD_LATE,             					TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_TDS_TA_PRELOAD_LATE,             					TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_EVDO_TB_PRELOAD_LATE,            					TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_IRAT_EVDO_TA_PRELOAD_LATE,            					TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_LTE_INVALID_PA_STATE_OR_RGI,   				            TRACE_EN|ASSERT_DIS|MSG_EN  ) \
  entry( RFLM_WARNING_LTE_INVALID_POWER_LEVEL_FROM_FW,       					TRACE_EN|ASSERT_DIS|MSG_EN  ) \
  entry( RFLM_WARNING_LTE_LNA_UPDATE_DROPPED_DUE_TO_FW_TIME_IN_PAST,       		TRACE_EN|ASSERT_DIS|MSG_DIS  ) \
  entry( RFLM_WARNING_LTE_LNA_UPDATE_DROPPED_DUE_TO_SOFT_TIME_VIOLATION,       	TRACE_EN|ASSERT_DIS|MSG_DIS  ) \
  entry( RFLM_WARNING_TDS_SLOT_UPDATE_TIMELINE_LATE,    			            TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_TDS_RXAGC_WRONG_RXLM_HANDLE,    					        TRACE_EN|ASSERT_DIS|MSG_DIS   ) \
  entry( RFLM_WARNING_TDS_TXAGC_WRONG_TXLM_HANDLE,    					        TRACE_EN|ASSERT_DIS|MSG_DIS   ) \
  entry( RFLM_WARNING_TDS_SCRIPT_BUFFER_READ_WRITE_COLLIDE,		                TRACE_EN|ASSERT_DIS|MSG_DIS   ) \
  entry( RFLM_WARNING_TDS_CAN_NOT_COMPLETE_INTERNAL_EVENT,		                TRACE_EN|ASSERT_EN|MSG_EN   ) \
  entry( RFLM_WARNING_TDS_DID_NOT_GET_FW_RELEASE_AND_TIME_OUT,		            TRACE_EN|ASSERT_EN|MSG_EN   ) \
  entry( RFLM_WARNING_TDS_PINGPONG_BUFFER_VIOLATION,		                    TRACE_EN|ASSERT_EN|MSG_EN   ) \
  entry( RFLM_WARNING_TDS_INCONSISTENT_INFO_FOR_HEADSTART,                      TRACE_EN|ASSERT_EN|MSG_EN   ) \
  entry( RFLM_WARNING_TDS_INCONSISTENT_INFO_FOR_INTERNAL_EVENT,                 TRACE_EN|ASSERT_EN|MSG_EN   ) \
  entry( RFLM_WARNING_TDS_CCS_MEMORY_NOT_ENOUGH,                                TRACE_EN|ASSERT_EN|MSG_EN   ) \
  entry( RFLM_WARNING_TDS_INTERNAL_SCRIPTS_EXECUTED_ON_NON_CAL_MODE,            TRACE_EN|ASSERT_EN|MSG_EN   ) \
  entry( RFLM_WARNING_TASK_TRIG_TIME_GREATER_THAN_1_7MS_FUTURE,					TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_WARNING_TASK_TRIG_TIME_LESS_THAN_1_7MS_PAST,					    TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_DIAG_POWER_WARNING_OVERWRITE_PEND_READ_BUF,		                TRACE_EN|ASSERT_EN|MSG_EN   ) \
  entry( RFLM_DIAG_POWER_WARNING_SWITCH_IGNORED,		                        TRACE_EN|ASSERT_DIS|MSG_EN   ) \
  entry( RFLM_DIAG_POWER_WARNING_INVALID_HANDLE,                    			TRACE_EN|ASSERT_EN|MSG_EN   ) \
  entry( RFLM_DIAG_POWER_WARNING_NULL_READ_CALLBACK,                    		TRACE_EN|ASSERT_EN|MSG_EN   ) \
  entry( RFLM_WARNING_DM_AUTOPIN_MEM_ALLOC_FAILED,                    		    TRACE_EN|ASSERT_EN|MSG_EN   ) \
  entry( RFLM_WARNING_DM_AUTOPIN_MEM_DEALLOC_FAILED,                    		TRACE_EN|ASSERT_EN|MSG_EN   ) \
  entry( RFLM_WARNING_ADD_HERE,                             					TRACE_EN|ASSERT_DIS|MSG_EN   ) 
  
  



#endif  /* RFLM_WARNING_ENUM_H */
  
