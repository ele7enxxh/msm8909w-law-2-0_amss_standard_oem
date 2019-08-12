/*! 
  @file
  rf_qmi_rfrpe_rf_task.c
 
  @brief
  This file includes functions pertaining to Tasks used for RF Tuner Operation
 
  @addtogroup RF_QMI_RFRPE_RF_TASK
  @{
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/qmi_svc/rfrpe/src/rf_qmi_rfrpe_rf_task.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
11/07/13   yb/ndb  Added the support for dynamic tune code size
07/19/13   hm      Fixed CR:515312
04/11/13   ndb     Removed the unused code
02/11/13   aca     QMI response message
02/07/13   aca     RFRPE Task Registration
02/07/13   aca     DIME QMI call back fn registration
02/06/13   aca     DIME QMI server dispatch update
02/04/13   aca     DIME QMI server
09/10/12   gh      Initial Revision.

==============================================================================*/
#include "comdef.h"
#include "rf_qmi_rfrpe_rf_task.h"
#include "rf_qmi_rfrpe_svc.h"
#include "rfm_rf_tuner_umid.h"
#include "rfcommon_msg.h"
#include "rf_cmd_dispatcher.h"
#include "msgr_umid.h"
#include "rfa_msgr.h"

static rf_qmi_rfrpe_state_type rf_qmi_rfpre_state_machine;
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Handler for get scenario request
  
  @param set_scenario_payload
  Request data
*/

void rf_qmi_rfrpe_process_set_scenario_req(rf_qmi_rfrpe_svc_set_scenario_req_s* set_scenario_payload)
{
  uint8 counter = 0;
	boolean execution_status = FALSE;
	rf_qmi_rfrpe_resp_err_code_type err_code = RF_QMI_RFRPE_RESP_ERR_CODE_SUCCESS;
	
  uint8 num_fn = rf_qmi_rfpre_state_machine.call_bk_fn[RF_QMI_SET_SCENARIO_CMD].num_call_bk_fn;
	if(num_fn < RF_QMI_MAX_FUNCTIONS)
	{
	  RF_MSG_1(RF_HIGH, "rf_qmi_rfrpe_rf_task_dispatch: received set_scenario cmd! %d Fn registered", num_fn);
	}
	else
	{	  
		RF_MSG_1(RF_HIGH, "rf_qmi_rfrpe_rf_task_dispatch: received set_scenario cmd! %d Fn registered outside max", num_fn);
		num_fn = RF_QMI_MAX_FUNCTIONS- 1;
	}

	if(set_scenario_payload != NULL)
	{
		RF_MSG_1(RF_HIGH, "set %d scenarios", set_scenario_payload->scenario_len);			
		for(counter = 0; counter < RF_QMI_RFRPE_SVC_MAX_CONCURRENT_SCENARIOS && counter < set_scenario_payload->scenario_len; counter++)
		{
			RF_MSG_2(RF_HIGH, "scenario[%d]: %d", counter, set_scenario_payload->scenarios[counter]);
		}
	
		/* Call all registered functions*/
		for(counter = 0; counter < num_fn && set_scenario_payload != NULL; counter++)
		{
			execution_status = rf_qmi_rfpre_state_machine.call_bk_fn[RF_QMI_SET_SCENARIO_CMD].fn_list[counter].fn_ptr.set_scenario_fp
																									 (rf_qmi_rfpre_state_machine.call_bk_fn[RF_QMI_SET_SCENARIO_CMD].fn_list[counter].class_obj_ptr,
																										set_scenario_payload->scenario_len,
																										set_scenario_payload->scenarios);

			rf_qmi_rfpre_state_machine.call_bk_fn[RF_QMI_SET_SCENARIO_CMD].fn_list[counter].execution_status = execution_status;
			if(execution_status)
			{
				RF_MSG_1(RF_HIGH, "Device[%d] set scneario successful", counter);
			}
			else
			{
				RF_MSG_1(RF_HIGH, "Device[%d] set scneario failed", counter);
				err_code = RF_QMI_RFRPE_RESP_ERR_CODE_INTERNAL_ERROR;
			}
	  }		
		/* Send back response */
		if(!rf_qmi_rfrpe_set_scenarios_resp(set_scenario_payload->qmi_req_handle, err_code))
		{
		  RF_MSG(RF_HIGH, "RF_QMI_SET_SCENARIO_CMD response failed ");
		}
	}
	else /* Request is bad */
	{
	  err_code = RF_QMI_RFRPE_RESP_ERR_CODE_BAD_PARAMETERS;
	}
	
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Handler for get scenario request
  
  @param get_scenario_payload
  Request data
*/

void rf_qmi_rfrpe_process_get_scenario_req(rf_qmi_rfrpe_svc_get_scenario_req_s* get_scenario_payload)
{
  uint8 counter = 0;
  uint8 i = 0;
  boolean execution_status = FALSE;
  uint8 nv_code[RF_QMI_MAX_SIZE_OF_EACH_TUNE_CODE];
  uint8 tune_code_size = 0;
  uint32 current_scenario[RF_QMI_MAX_FUNCTIONS];
  rf_qmi_rfrpe_resp_err_code_type err_code = RF_QMI_RFRPE_RESP_ERR_CODE_SUCCESS;
  uint8 num_fn = rf_qmi_rfpre_state_machine.call_bk_fn[RF_QMI_GET_SCENARIO_CMD].num_call_bk_fn;
	
	if(num_fn < RF_QMI_MAX_FUNCTIONS)
	{
	  RF_MSG_1(RF_HIGH, "rf_qmi_rfrpe_rf_task_dispatch: received get_scenario cmd! %d Fn registered", num_fn);
	}
	else
	{	  
		RF_MSG_1(RF_HIGH, "rf_qmi_rfrpe_rf_task_dispatch: received get_scenario cmd! %d Fn registered outside max", num_fn);
		num_fn = RF_QMI_MAX_FUNCTIONS- 1;
	}


	if(get_scenario_payload != NULL)
	{
		/* Call all registered functions*/
		for(counter = 0; counter < num_fn && get_scenario_payload != NULL ; counter++)
		{
			current_scenario[counter] = 0;
			execution_status = rf_qmi_rfpre_state_machine.call_bk_fn[RF_QMI_GET_SCENARIO_CMD].fn_list[counter].fn_ptr.get_scenario_fp(
                                                                                                                                                   rf_qmi_rfpre_state_machine.call_bk_fn[RF_QMI_GET_SCENARIO_CMD].fn_list[counter].class_obj_ptr,
                                                                                                                                                   (uint8*)&(current_scenario[counter]),
                                                                                                                                                   nv_code,
                                                                                                                                                   &tune_code_size 
                                                                                                                                                 );
			rf_qmi_rfpre_state_machine.call_bk_fn[RF_QMI_GET_SCENARIO_CMD].fn_list[counter].execution_status = execution_status;
			if(execution_status)
			{
                          RF_MSG_3(RF_HIGH, "Device[%d] scneario:[%d] tune_code_size:[%d]", counter, current_scenario, tune_code_size);
                          for(i=0; i<tune_code_size; i++)
                          {
                             RF_MSG_2(RF_HIGH, "NV Code[%d] = %d", i,nv_code[i]);
                          }
			}
			else
			{
				RF_MSG_1(RF_HIGH, "Device[%d] get scenario failed", counter);
				err_code = RF_QMI_RFRPE_RESP_ERR_CODE_INTERNAL_ERROR;
			}
		}
		if(!rf_qmi_rfrpe_get_scenarios_resp(get_scenario_payload->qmi_req_handle, (uint32)num_fn, &current_scenario[0], err_code))
		{
		  RF_MSG(RF_HIGH, "RF_QMI_GET_SCENARIO_CMD response failed ");
		}
	}
	else /* Request is bad */
	{
	  err_code = RF_QMI_RFRPE_RESP_ERR_CODE_BAD_PARAMETERS;
	}

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Handler for get scenario request
  
  @param get_scenario_payload
  Request data
*/

void rf_qmi_rfrpe_process_get_provision_table_req(rf_qmi_rfrpe_svc_get_revision_req_s* get_revision_payload)
{
	uint8 counter = 0;
	uint32 revision = 0;
	uint32 oem_string_len = 0;
	uint16 oem_string[64];
	boolean execution_status = FALSE;
	rf_qmi_rfrpe_resp_err_code_type err_code = RF_QMI_RFRPE_RESP_ERR_CODE_SUCCESS;
  uint8 num_fn = rf_qmi_rfpre_state_machine.call_bk_fn[RF_QMI_GET_PROVISION_TBL_CMD].num_call_bk_fn;

	if(num_fn < RF_QMI_MAX_FUNCTIONS)
	{
	  RF_MSG_1(RF_HIGH, "rf_qmi_rfrpe_rf_task_dispatch: received get_revision cmd! %d Fn registered", num_fn);
	}
	else
	{	  
		RF_MSG_1(RF_HIGH, "rf_qmi_rfrpe_rf_task_dispatch: received get_revision cmd! %d Fn registered outside max", num_fn);
		num_fn = RF_QMI_MAX_FUNCTIONS- 1;
	}

	if(get_revision_payload != NULL)
	{
	  /* Call all registered functions*/
		for(counter = 0; counter < num_fn && get_revision_payload != NULL ; counter++)
		{
		  execution_status = rf_qmi_rfpre_state_machine.call_bk_fn[RF_QMI_GET_PROVISION_TBL_CMD].fn_list[counter].fn_ptr.get_provision_table_fp
																									 (rf_qmi_rfpre_state_machine.call_bk_fn[RF_QMI_GET_PROVISION_TBL_CMD].fn_list[counter].class_obj_ptr,
																									 &revision, &oem_string_len, oem_string);
			rf_qmi_rfpre_state_machine.call_bk_fn[RF_QMI_GET_PROVISION_TBL_CMD].fn_list[counter].execution_status = execution_status;				
			if(execution_status)
			{
			  RF_MSG_4(RF_HIGH, "Device[%d] rev: %d , string[%d]: %d", counter, revision, oem_string_len, oem_string);
			}
			else
			{
			  RF_MSG_1(RF_HIGH, "Device[%d] get revision failed", counter);
				err_code = RF_QMI_RFRPE_RESP_ERR_CODE_INTERNAL_ERROR;
			}
		}
		if(!rf_qmi_rfrpe_get_provisioned_table_info_resp(get_revision_payload->qmi_req_handle, revision, oem_string_len, oem_string, err_code))
		{
		  RF_MSG(RF_HIGH, "RF_QMI_GET_PROVISION_TBL_CMD response failed ");
		}
	}
	else /* Request is bad */
	{
	  err_code = RF_QMI_RFRPE_RESP_ERR_CODE_BAD_PARAMETERS;
	}
	
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch function for RF Tuner commands dispatched from RF_TASK
 
  @details
  This function is called by RF_TASK when it receives messages/commands for
  RF Tuner
  
  @param req_ptr
  Pointer to RF Task command
*/
static void rf_qmi_rfrpe_rf_task_dispatch(rf_cmd_type* req_ptr)
{
  /* Validate Request Pointer */
  if( req_ptr == NULL )
  {
    RF_MSG( RF_ERROR, "rf_qmi_rfrpe_rf_task_dispatch: NULL Request" );
  } 
  else
  {	
		/*==========================================================================*/
		/* Dispatch */
		/*==========================================================================*/	

	  /* Dispatch based on the UMID */
	  switch( req_ptr->msgr_hdr.id )
	  {
	    case RFA_RF_TUNER_SET_SCENARIO_CMD:
				rf_qmi_rfrpe_process_set_scenario_req((rf_qmi_rfrpe_svc_set_scenario_req_s*)(req_ptr->payload));      
	      break;

	    case RFA_RF_TUNER_GET_SCENARIO_CMD:
				rf_qmi_rfrpe_process_get_scenario_req((rf_qmi_rfrpe_svc_get_scenario_req_s*)(req_ptr->payload));
	      break;

	    case RFA_RF_TUNER_GET_REVISION_CMD:
	      rf_qmi_rfrpe_process_get_provision_table_req((rf_qmi_rfrpe_svc_get_revision_req_s*)(req_ptr->payload));			
	      break;

	    default:
	      RF_MSG_1(RF_ERROR, "rf_qmi_rfrpe_rf_task_dispatch: Invalid UMID 0x%x", req_ptr->msgr_hdr.id);
	      break;
	  } /* switch( req_ptr->msgr_hdr.id ) */
  }
  return;

} /* rf_qmi_rfrpe_rf_task_dispatch */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize dispatcher for RF Tuner commands processed through RF Task

  @details
  This function will register all RF Tuner commands to RF Task dispatcher and
  also registers with message router, if required.
 
  @param msgr_client
  Message router client ID for registration
 
  @param mailbox
  Messgae router ID for registration

  @return
  TRUE on success and FALSE on failure
*/
boolean
rf_qmi_rfrpe_rf_task_dispatch_init
(
  msgr_client_t *msgr_client, 
  msgr_id_t mailbox 
)
{
  rf_cmd_dispatch_status_type dis_res; /* cmd_dispatch result */
  boolean init_status = TRUE;
	uint8 cmd_index = 0;
	uint8 fn_index = 0;

  /* Register handler for this function */
  dis_res = rf_cmd_dispatch_register_tech( RF_TASK_MAIN, MSGR_TECH_RFA, MSGR_MODULE_RF_TUNER,
                                           rf_qmi_rfrpe_rf_task_dispatch);

  if ( dis_res != RF_CMD_REGISTRATION_SUCCESS )
  {
    RF_MSG( RF_ERROR, "rf_qmi_rfrpe_rf_task_dispatch_init: Failed RFRPE registration"  );    
    init_status = FALSE;
  }
	else
	{
	  /* Set up state machine */
		for(cmd_index = 0; cmd_index < RF_QMI_MAX_CMD; cmd_index++)
		{
		  rf_qmi_rfpre_state_machine.call_bk_fn[cmd_index].cmd = cmd_index; 
			rf_qmi_rfpre_state_machine.call_bk_fn[cmd_index].num_call_bk_fn = 0;
			for(fn_index = 0; fn_index < RF_QMI_MAX_FUNCTIONS; fn_index++)
			{
			  rf_qmi_rfpre_state_machine.call_bk_fn[cmd_index].fn_list[fn_index].execution_status = FALSE;
			}
		}
	}

  /* Return Status */
  return init_status;

} /* rf_qmi_rfrpe_rf_task_dispatch_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Deinitialize dispatcher for RF Tuner commands processed through RF Task

  @details
  This function will unregister message router commands, release
  allocated semphores and unrgister RF task.
 
  @param msgr_client
  Message router client ID for unregistration
 
  @return
  TRUE on success and FALSE on failure
*/
boolean
rf_qmi_rfrpe_rf_task_dispatch_deinit
(
  msgr_client_t *msgr_client
)
{
  boolean deinit_status = TRUE;
  uint8 cmd_index = 0;
	uint8 fn_index = 0;
	/* Clean up state machine */
	for(cmd_index = 0; cmd_index < RF_QMI_MAX_CMD; cmd_index++)
	{
	  rf_qmi_rfpre_state_machine.call_bk_fn[cmd_index].cmd = 0; 
		rf_qmi_rfpre_state_machine.call_bk_fn[cmd_index].num_call_bk_fn = 0;
		for(fn_index = 0; fn_index < RF_QMI_MAX_FUNCTIONS; fn_index++)
		{
		  rf_qmi_rfpre_state_machine.call_bk_fn[cmd_index].fn_list[fn_index].execution_status = FALSE;
		}
	}
  rf_cmd_dispatch_deregister_tech( RF_TASK_MAIN, MSGR_TECH_RFA, MSGR_MODULE_RF_TUNER);

  /* Return Status */
  return deinit_status;

} /* rf_qmi_rfrpe_rf_task_dispatch_deinit */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Check if the requested function is valid for registration

  @details
  Function will discard duplicate or invalid fn rregistration requests
 
  @param cmd
  Cmd fn is being registered

  @param fn
  Function pointer
 
  @return
  TRUE on success and FALSE on failure
*/

boolean
rf_qmi_valid_for_registration_check
(
	rf_qmi_cmd_type_enum cmd,
	rf_qmi_rfrpe_cb_fn_type cb_fn
)
{
  boolean valid = TRUE;
  uint8 counter = 0;
  uint8 num_fn = rf_qmi_rfpre_state_machine.call_bk_fn[cmd].num_call_bk_fn;
	rf_qmi_cmd_handler_func_type fn = cb_fn.fn_ptr;
	if((num_fn + 1) > RF_QMI_MAX_FUNCTIONS)
	{
		RF_MSG_1( RF_ERROR, "Unable to register function for %d All slots are taken",cmd);
		valid = FALSE;
	}
	else
	{
	  /* Check all registered functions to see if the fn is already registered for */
	  for(counter = 0; counter < num_fn; counter++)
	  {
	    if(cmd == RF_QMI_SET_SCENARIO_CMD &&
				cb_fn.class_obj_ptr == rf_qmi_rfpre_state_machine.call_bk_fn[cmd].fn_list[counter].class_obj_ptr &&
				fn.set_scenario_fp== rf_qmi_rfpre_state_machine.call_bk_fn[cmd].fn_list[counter].fn_ptr.set_scenario_fp)
    	{
    	  RF_MSG_2( RF_ERROR, "Fn already registered at slot %d for cmd %d",counter,cmd);
    	  valid = FALSE;
				break;
    	}
			if(cmd == RF_QMI_GET_SCENARIO_CMD && 
				fn.get_scenario_fp== rf_qmi_rfpre_state_machine.call_bk_fn[cmd].fn_list[counter].fn_ptr.get_scenario_fp)
    	{
    	  RF_MSG_2( RF_ERROR, "Fn already registered at slot %d for cmd %d",counter,cmd);
    	  valid = FALSE;
				break;
    	}
			if(cmd == RF_QMI_GET_PROVISION_TBL_CMD && 
				fn.get_provision_table_fp== rf_qmi_rfpre_state_machine.call_bk_fn[cmd].fn_list[counter].fn_ptr.get_provision_table_fp)
    	{
    	  RF_MSG_2( RF_ERROR, "Fn already registered at slot %d for cmd %d",counter,cmd);
    	  valid = FALSE;
				break;
    	}
	  }
	}
	return valid;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Register requested function for a given command

  @details
  Allows multiple modules to register call back functions for a given CMD
 
  @param cmd
  Cmd fn is being registered

  @param fn
  Callback function pointer
 
  @return
  TRUE on success and FALSE on failure
*/

boolean
rf_qmi_register_function
(
	rf_qmi_cmd_type_enum cmd,
	rf_qmi_rfrpe_cb_fn_type cb_fn

)
{
  boolean status = FALSE;
	uint8 index = 0;
	/* Check if the params are valid */
  if(cmd >= RF_QMI_MAX_CMD)
  {
  	RF_MSG( RF_ERROR, "rf_qmi_register_function: Invalid params ");
		status = FALSE;
  }
	else
	{
		switch(cmd)
		{
		  case RF_QMI_SET_SCENARIO_CMD:
				if(cb_fn.fn_ptr.set_scenario_fp == NULL)
				{
					RF_MSG( RF_ERROR, "Invalid function attempting to register for SET_SCENARIO" );
					status = FALSE;				
				}
				else if(!rf_qmi_valid_for_registration_check(cmd, cb_fn))
				{
					RF_MSG( RF_ERROR, "Unable to register function for SET_SCENARIO");
					status = FALSE;
				}
				else
				{
					index = rf_qmi_rfpre_state_machine.call_bk_fn[cmd].num_call_bk_fn;
					
					rf_qmi_rfpre_state_machine.call_bk_fn[cmd].fn_list[index].execution_status = FALSE;
				  rf_qmi_rfpre_state_machine.call_bk_fn[cmd].fn_list[index].fn_ptr.set_scenario_fp = cb_fn.fn_ptr.set_scenario_fp;
					rf_qmi_rfpre_state_machine.call_bk_fn[cmd].fn_list[index].class_obj_ptr = cb_fn.class_obj_ptr;
					
				  rf_qmi_rfpre_state_machine.call_bk_fn[cmd].num_call_bk_fn++;
					status = TRUE;
					
				  RF_MSG( RF_LOW, "Successfully registerd function for SET_SCENARIO");
				}
				break;
		  case RF_QMI_GET_SCENARIO_CMD:
				if(cb_fn.fn_ptr.get_scenario_fp== NULL)
				{
					RF_MSG( RF_ERROR, "Invalid function attempting to register for GET_SCENARIO" );
					status = FALSE;				
				}
				else if(!rf_qmi_valid_for_registration_check(cmd, cb_fn))
				{
					RF_MSG( RF_ERROR, "Unable to register function for GET_SCENARIO");
					status = FALSE;
				}
				else
				{
				  index = rf_qmi_rfpre_state_machine.call_bk_fn[cmd].num_call_bk_fn;
					
					rf_qmi_rfpre_state_machine.call_bk_fn[cmd].fn_list[index].execution_status = FALSE;
				  rf_qmi_rfpre_state_machine.call_bk_fn[cmd].fn_list[index].fn_ptr.get_scenario_fp = cb_fn.fn_ptr.get_scenario_fp;
					rf_qmi_rfpre_state_machine.call_bk_fn[cmd].fn_list[index].class_obj_ptr = cb_fn.class_obj_ptr;
					
				  rf_qmi_rfpre_state_machine.call_bk_fn[cmd].num_call_bk_fn++;		
					status = TRUE;
					
				  RF_MSG( RF_LOW, "Successfully registerd function for GET_SCENARIO");
					status = FALSE;
				}
				break;
		  case RF_QMI_GET_PROVISION_TBL_CMD:
				if(cb_fn.fn_ptr.get_provision_table_fp == NULL)
				{
					RF_MSG( RF_ERROR, "Invalid function attempting to register for GET_PROVISION_TBL" );
					status = FALSE;				
				}
				else if(!rf_qmi_valid_for_registration_check(cmd, cb_fn))
				{
					RF_MSG( RF_ERROR, "Unable to register function for GET_PROVISION_TBL");
					status = FALSE;
				}
				else
				{
				  index = rf_qmi_rfpre_state_machine.call_bk_fn[cmd].num_call_bk_fn;
					
					rf_qmi_rfpre_state_machine.call_bk_fn[cmd].fn_list[index].execution_status = FALSE;
				  rf_qmi_rfpre_state_machine.call_bk_fn[cmd].fn_list[index].fn_ptr.get_provision_table_fp = cb_fn.fn_ptr.get_provision_table_fp;
					rf_qmi_rfpre_state_machine.call_bk_fn[cmd].fn_list[index].class_obj_ptr = cb_fn.class_obj_ptr;
					
				  rf_qmi_rfpre_state_machine.call_bk_fn[cmd].num_call_bk_fn++;	
					status = TRUE;
					
				  RF_MSG( RF_LOW, "Successfully registerd function for GET_PROVISION_TBL");
				}
				break;				
			default:
				RF_MSG_1( RF_ERROR, "rf_qmi_register_function: Cmd %d not serviced",cmd);
				break;
		}
	}
	return status;
}

/*! @} */
