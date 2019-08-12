/**
@file rlc.c
@brief
This module implements common RLC.
*/

/*=============================================================================
  Copyright (c) 2013-2014 Qualcomm Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                              R   L   C

GENERAL DESCRIPTION
  Implements common RLC

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/rlc/src/rlc.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/02/14   psu     Update diag macros using optimized versions.
7/17/13	   as      Added learning functionality
11/30/12   cp      File created.

=============================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/

#ifdef FEATURE_WINDOWS_SIM
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <stdarg.h>
#include "comdef.h"
#endif /* FEATURE_WINDOWS_SIM */

#include <memory.h>
#include "stdlib.h"
#include "comdef.h"
#include "rlc_os.h"
#include "rlc.h"
#include "rlc_v.h"
#include "rlc_report_tracer.h"
#include "rlc_report_qxdm.h"
#include "rlc_report_printf.h"
#include "rlc_report_efs.h"
#include "rlc_report_smd.h"
#include "rlc_learning.h"
#include "rlc_correct.h"

#include "rcevt.h"
#include "rcinit.h"

#include "msg_diag_service.h"


/*=============================================================================

                         STATIC VARIABLES

=============================================================================*/

static rlc_internal_struct rlc;

/*=============================================================================
                         
                         INTERNAL FUNCTIONS

=============================================================================*/

/** 
Gets the internal RLC handle from an external RLC handle.

@return
Internal RLC handle.

@dependencies
None.

*/

rlc_internal_handle_type* rlc_map_internal_handle(rlc_handle_ptr handle)
{
  if(handle == NULL)
    return NULL;
  return (rlc_internal_handle_type*)handle->internal_handle;
}

/** 
Gets the internal attribute from the external attribute.

@return
Internal attribute.

@dependencies
None.
 
*/

inline rlc_attribute_type* rlc_map_internal_attribute(rlc_external_attribute_type attr)
{
  return (rlc_attribute_type*)attr;
}

/** 
Verifies that the attribute that was passed in does not have any issues.

@return
RLC_SUCCESS -- Attribute has no issues.
RLC_FAILED -- There is an issue with the attribute.

@dependencies
None.

*/
/*
static rlc_error_type rlc_verify_attribute(rlc_attribute_type *attr)
{
  if(attr->attribute_structure <= RLC_ATTRIBUTE_NONE ||
     attr->attribute_structure >= RLC_ATTRIBUTE_MAX ||
     (attr->dyn_corr_ptr == NULL && attr->report_ptr == NULL && attr->learning_ptr == NULL) ||
     (attr->attribute_structure == RLC_ATTRIBUTE_REPORT && attr->report_ptr == NULL) ||
     (attr->attribute_structure == RLC_ATTRIBUTE_DYNAMIC_CORRECTION && attr->dyn_corr_ptr == NULL) ||
     (attr->attribute_structure == RLC_ATTRIBUTE_REPORT_W_DYNAMIC_CORRECTION && attr->dyn_corr_ptr == NULL) ||
     (attr->attribute_structure == RLC_ATTRIBUTE_REPORT_W_DYNAMIC_CORRECTION && attr->report_ptr == NULL) ||
     (attr->attribute_structure == RLC_ATTRIBUTE_LEARN && attr->learning_ptr == NULL))
  {
    return RLC_FAILED;
  }
  return RLC_SUCCESS;
}
*/

/** 
Initializes the report-based attribute and configures anything
required in the drivers below needed for reporting.

@return
RLC_SUCCESS --          Attribute has been successfully set up.
RLC_FAILED --           There was a general error in setting up the attribute
                        or the attribute was not found.
RLC_NOT_IMPLEMENTED --  The reporting method setup has not been implemented at
                        this point.  Will be done in the future.

@dependencies
Handle must be registered with RLC before this point.

*/

static rlc_error_type rlc_setup_report_attribute(rlc_handle_ptr              handle,
                                                 rlc_attribute_type          *attr_ptr,
                                                 uint32                      reporting_data)
{
  switch(attr_ptr->report_ptr->report_output)
  {
  case RLC_REPORT_OUT_FILE:
  case RLC_REPORT_OUT_CALLBACK:
  case RLC_REPORT_OUT_DATA_PACKET:
	case RLC_REPORT_OUT_SMD:
    return RLC_NOT_IMPLEMENTED;
    //return rlc_report_smd_init(attr_ptr, reporting_data);
  case RLC_REPORT_OUT_PRINTF:
  case RLC_REPORT_OUT_QXDM:
    /* Don't need to do anything to initialize printf */
    return RLC_SUCCESS;
  case RLC_REPORT_OUT_QDSS_TRACER:
    return rlc_report_tracer_init(attr_ptr);
  case RLC_REPORT_OUT_EFS:
    //return rlc_report_efs_init(handle,attr_ptr);
	return RLC_SUCCESS;
  default:
    break;
  }
  return RLC_FAILED;
}

/** 
Initializes the learning-based attribute and configures anything
required in the drivers below needed for reporting.

@return
RLC_SUCCESS --          Attribute has been successfully set up.
RLC_FAILED --           There was a general error in setting up the attribute
                        or the attribute was not found.
RLC_NOT_IMPLEMENTED --  The reporting method setup has not been implemented at
                        this point.  Will be done in the future.

@dependencies
Handle must be registered with RLC before this point.

*/

static rlc_error_type rlc_setup_learning_attribute( rlc_handle_ptr              handle,
                                                    rlc_attribute_type          *attr_ptr,
                                                    uint32                      size,
													uint32						numDimensions,
													uint32						numClusters
													)
{
  /*switch(attr_ptr->learning_ptr->learning_type)
  {
  case RLC_LEARNING_ANOMALY_DETECTION:
    return rlc_learning_anomaly_detection_init(attr_ptr, size, numDimensions, numClusters);
  case RLC_LEARNING_CLUSTERING:
    return RLC_SUCCESS;

  }
  return RLC_FAILED;*/
	return rlc_learning_anomaly_detection_init(attr_ptr, size, numDimensions, numClusters);

}

/** 
Destroys learning attribute.

@return
RLC_SUCCESS --          Attribute has been successfully set up.
RLC_FAILED --           There was a general error in setting up the attribute
                        or the attribute was not found.
RLC_NOT_IMPLEMENTED --  The reporting method setup has not been implemented at
                        this point.  Will be done in the future.

@dependencies
Handle must be registered with RLC before this point.

*/

rlc_error_type rlc_destroy_learning_attribute( rlc_attribute_type          *attr_ptr )
{
  return rlc_learning_anomaly_detection_destroy( attr_ptr );
}

/** 
Initializes the correction-based attribute and configures anything
required in the drivers below needed for reporting.

@return
RLC_SUCCESS --          Attribute has been successfully set up.
RLC_FAILED --           There was a general error in setting up the attribute
                        or the attribute was not found.
RLC_NOT_IMPLEMENTED --  The reporting method setup has not been implemented at
                        this point.  Will be done in the future.

@dependencies
Handle must be registered with RLC before this point.

*/

static rlc_error_type rlc_setup_correcting_attribute( rlc_handle_ptr              handle,
                                                    rlc_attribute_type          *attr_ptr,
													void (*func)(uint32),
													rlc_attribute_type			*attr_learn													
													)
{
  	switch(attr_ptr->dyn_corr_ptr->dyn_corr)
	{
	case RLC_CORR_SET_VAR_BC:
		return rlc_correct_init_sc(attr_ptr, attr_learn, func);
	case RLC_CORR_SET_VAR_WC:
		return RLC_NOT_IMPLEMENTED;
	default:
		break;
	}
	return RLC_FAILED;
	}

/** 
Destroys Correcting Attribute.

@return
RLC_SUCCESS --          Attribute has been successfully set up.
RLC_FAILED --           There was a general error in setting up the attribute
                        or the attribute was not found.
RLC_NOT_IMPLEMENTED --  The reporting method setup has not been implemented at
                        this point.  Will be done in the future.

@dependencies
Handle must be registered with RLC before this point.

*/

static rlc_error_type rlc_destroy_correcting_attribute( rlc_attribute_type          *attr_ptr )
{
  rlc_error_type eError = RLC_NOT_IMPLEMENTED;

  switch(attr_ptr->dyn_corr_ptr->dyn_corr)
	{
	case RLC_CORR_SET_VAR_BC:
		eError = rlc_correct_destroy_sc( attr_ptr );
	case RLC_CORR_SET_VAR_WC:
		return RLC_NOT_IMPLEMENTED;
	default:
		break;
	}

  rlc_free( attr_ptr->dyn_corr_ptr );
  attr_ptr->dyn_corr_ptr = NULL;

	return eError;
	}

/** 
Internal function for dynamic correction on the attribute that was passed in.
This function can be called from RLC context, or client task context.

@return
RLC_SUCCESS --          DC was successful.
RLC_FAILED --           There was a general error in DC or DC
                        method was not found or supported.
RLC_NOT_IMPLEMENTED --  The DC method has not been implemented at
                        this point.  Will be done in the future.

@dependencies
Internal attribute must be non-NULL.

*/
/*
static rlc_error_type rlc_dc_on_attribute(rlc_attribute_type *attr_ptr)
{
  switch(attr_ptr->dyn_corr_ptr->dyn_corr)
  {

  case RLC_CORR_PERFORM_CALLBACK:
  case RLC_CORR_SET_VAR_WC:
    return RLC_NOT_IMPLEMENTED;
  default:
    break;
  }
  return RLC_FAILED;
}
*/
/** 
Internal function for popping a command from the RLC reporting command queue.

@return
RLC_SUCCESS --          Command successfully popped and returned.
RLC_FAILED --           Either a NULL pointer was passed in or the list is empty.

@dependencies
None.

*/

static rlc_error_type rlc_r_pop_command(rlc_command_type **async_command)
{
  if(async_command == NULL)
    return RLC_FAILED;

	rlc_lock_mutex(&rlc.rlc_r_mutex);
  if(rlc.report_command_list_first == NULL)
  {
    /* Empty List */
		rlc_unlock_mutex(&rlc.rlc_r_mutex);
		return RLC_EMPTY;
  }
  else if(rlc.report_command_list_first == rlc.report_command_list_last)
  {
    /* Only 1 command on the queue */
    *async_command = rlc.report_command_list_first;
    rlc.report_command_list_last = NULL;
    rlc.report_command_list_first = NULL;
  }
  else
  {
    /* Multiple commands on the list */
    *async_command = rlc.report_command_list_first;
    rlc.report_command_list_first = rlc.report_command_list_first->next;
  }

	rlc_unlock_mutex(&rlc.rlc_r_mutex);
	return RLC_SUCCESS;
}

/** 
Pushes a command onto the end of the RLC reporting command queue.

@return
RLC_SUCCESS --          Command was properly pushed.
RLC_FAILED --           NULL pointer was passed in for the command.

@dependencies
None.

*/

static rlc_error_type rlc_r_queue_command(rlc_command_type *async_command)
{
  if(async_command == NULL)
    return RLC_FAILED;

	rlc_lock_mutex(&rlc.rlc_r_mutex);
  if(rlc.report_command_list_last == NULL)
  {
    /* Empty List */
    rlc.report_command_list_last = async_command;
    rlc.report_command_list_first = async_command;
  }
  else
  {
    /* One or more commands on the list */
    rlc.report_command_list_last->next = async_command;
    rlc.report_command_list_last = async_command;
  }

	rlc_unlock_mutex(&rlc.rlc_r_mutex);
	return RLC_SUCCESS;
}

/** 
Internal function for popping a command from the RLC learning command queue.

@return
RLC_SUCCESS --          Command successfully popped and returned.
RLC_FAILED --           Either a NULL pointer was passed in or the list is empty.

@dependencies
None.

*/

static rlc_error_type rlc_l_pop_command(rlc_command_type **async_command)
{
	if(async_command == NULL)
    return RLC_FAILED;

	rlc_lock_mutex(&rlc.rlc_l_mutex);
  if(rlc.learn_command_list_first == NULL)
  {
    /* Empty List */
		rlc_unlock_mutex(&rlc.rlc_l_mutex);
		return RLC_EMPTY;
  }
  else if(rlc.learn_command_list_first == rlc.learn_command_list_last)
  {
    /* Only 1 command on the queue */
    *async_command = rlc.learn_command_list_first;
    rlc.learn_command_list_last = NULL;
    rlc.learn_command_list_first = NULL;
  }
  else
  {
    /* Multiple commands on the list */
    *async_command = rlc.learn_command_list_first;
    rlc.learn_command_list_first = rlc.learn_command_list_first->next;
  }

	rlc_unlock_mutex(&rlc.rlc_l_mutex);
	return RLC_SUCCESS;
}

/** 
Internal function for removing all commands for an attribute from the RLC learning command queue.

@return
None.

@dependencies
None.

*/

static void rlc_l_remove_attr_commands(rlc_external_attribute_type attr)
{
  rlc_command_type *cmd,*prev_cmd;
	rlc_lock_mutex(&rlc.rlc_l_mutex);
  
  if(rlc.learn_command_list_first == NULL)
  {
    /* Empty List */
		rlc_unlock_mutex(&rlc.rlc_l_mutex);
		return;
  }
  else if(rlc.learn_command_list_first == rlc.learn_command_list_last)
  {
    /* Only 1 command on the queue */
    cmd = rlc.learn_command_list_first;
    if( cmd->ext_attr == attr )
    {
      rlc_free(cmd);
      rlc.learn_command_list_last = NULL;
      rlc.learn_command_list_first = NULL;
    }
  }
  else
  {
    /* Multiple commands on the list */
    prev_cmd = rlc.learn_command_list_first;
    cmd = prev_cmd->next;

    /* Check first element */
    while( prev_cmd != NULL && prev_cmd->ext_attr == attr )
    {
      if( prev_cmd == rlc.learn_command_list_first )
      {
        cmd = prev_cmd;
        prev_cmd = prev_cmd->next;
        rlc_free( cmd );
        rlc.learn_command_list_first = prev_cmd;
        if( prev_cmd != NULL )
        {
          cmd = prev_cmd->next;
        }
        else
        {
          cmd = NULL;
        }
      }
    }

    if( prev_cmd == NULL )
    {
      /* List is empty */
      rlc.learn_command_list_first = NULL;
      rlc.learn_command_list_last = NULL;
    }

    while( cmd != NULL )
    {
      if( cmd->ext_attr == attr )
      {
        prev_cmd->next = cmd->next;
        rlc_free( cmd );
        cmd = prev_cmd->next;
      }
      else
      {
        prev_cmd = cmd;
        cmd = cmd->next;
      }
    }
  }

	rlc_unlock_mutex(&rlc.rlc_l_mutex);
	return;
}

/** 
Pushes a command onto the end of the RLC learning command queue.

@return
RLC_SUCCESS --          Command was properly pushed.
RLC_FAILED --           NULL pointer was passed in for the command.

@dependencies
None.

*/

static rlc_error_type rlc_l_queue_command(rlc_command_type *async_command)
{
  if(async_command == NULL)
    return RLC_FAILED;

	rlc_lock_mutex(&rlc.rlc_l_mutex);
  if(rlc.learn_command_list_last == NULL)
  {
    /* Empty List */
    rlc.learn_command_list_last = async_command;
    rlc.learn_command_list_first = async_command;
  }
  else
  {
    /* One or more commands on the list */
    rlc.learn_command_list_last->next = async_command;
    rlc.learn_command_list_last = async_command;
  }

	rlc_unlock_mutex(&rlc.rlc_l_mutex);
	return RLC_SUCCESS;
}

/** 
Internal function for popping a command from the RLC correction command queue.

@return
RLC_SUCCESS --          Command successfully popped and returned.
RLC_FAILED --           Either a NULL pointer was passed in or the list is empty.

@dependencies
None.

*/

static rlc_error_type rlc_c_pop_command(rlc_command_type **async_command)
{
  if(async_command == NULL)
    return RLC_FAILED;

	rlc_lock_mutex(&rlc.rlc_c_mutex);
  if(rlc.correct_command_list_first == NULL)
  {
    /* Empty List */
		rlc_unlock_mutex(&rlc.rlc_c_mutex);
		return RLC_EMPTY;
  }
  else if(rlc.correct_command_list_first == rlc.correct_command_list_last)
  {
    /* Only 1 command on the queue */
    *async_command = rlc.correct_command_list_first;
    rlc.correct_command_list_last = NULL;
    rlc.correct_command_list_first = NULL;
  }
  else
  {
    /* Multiple commands on the list */
    *async_command = rlc.correct_command_list_first;
    rlc.correct_command_list_first = rlc.correct_command_list_first->next;
  }

	rlc_unlock_mutex(&rlc.rlc_c_mutex);
	return RLC_SUCCESS;
}

/** 
Pushes a command onto the end of the RLC correction command queue.

@return
RLC_SUCCESS --          Command was properly pushed.
RLC_FAILED --           NULL pointer was passed in for the command.

@dependencies
None.

*/

static rlc_error_type rlc_c_queue_command(rlc_command_type *async_command)
{
  if(async_command == NULL)
	  return RLC_FAILED;

  rlc_lock_mutex(&rlc.rlc_c_mutex);
  if(rlc.correct_command_list_last == NULL)
  {
    /* Empty List */
    rlc.correct_command_list_last = async_command;
    rlc.correct_command_list_first = async_command;
  }
  else
  {
    /* One or more commands on the list */
    rlc.correct_command_list_last->next = async_command;
    rlc.correct_command_list_last = async_command;
  }

	rlc_unlock_mutex(&rlc.rlc_c_mutex);
	return RLC_SUCCESS;
}

/** 
Internal function for selecting the reporting method for reporting on an
attribute.

@return
RLC_SUCCESS --          Reporting was successful.
RLC_FAILED --           There was a general error in reporting or reporting
                        method was not found or supported.
RLC_NOT_IMPLEMENTED --  The reporting method has not been implemented at
                        this point.  Will be done in the future.

@dependencies
Internal attribute must be non-NULL.

*/

static inline rlc_error_type rlc_report_out(rlc_attribute_type *attr_ptr, void* string, uint32 size)
{
  switch(attr_ptr->report_ptr->report_output)
  {
  case RLC_REPORT_OUT_FILE:
  case RLC_REPORT_OUT_CALLBACK:
  case RLC_REPORT_OUT_DATA_PACKET:
  case RLC_REPORT_OUT_SMD:
    return RLC_NOT_IMPLEMENTED;
  case RLC_REPORT_OUT_PRINTF:
    /* Report through printf */
    #ifdef FEATURE_WINDOWS_SIM
    return rlc_report_printf(attr_ptr, (char*)string);
    #endif /* FEATURE_WINDOWS_SIM */
  case RLC_REPORT_OUT_QXDM:
    return rlc_report_qxdm(attr_ptr, (char*)string);
  case RLC_REPORT_OUT_QDSS_TRACER:
    return rlc_report_tracer(attr_ptr, (char*)string);
  case RLC_REPORT_OUT_EFS:
    //return rlc_report_efs(attr_ptr, (char*)string);
	return RLC_SUCCESS;
  default:
    break;
  }
  return RLC_FAILED;
}

static inline rlc_error_type rlc_learn_out(rlc_attribute_type *attr_ptr, rlc_learning_type type, uint32 size, uint32 *data)
{
  rlc_error_type eError = RLC_SUCCESS;

	switch(type)
	{
	case RLC_LEARNING_CLUSTERING:
		return rlc_learning_cluster(attr_ptr);
	case RLC_LEARNING_STATISTICS:
    eError = rlc_get_averages(attr_ptr);
    if( eError != RLC_SUCCESS )
      return eError;
		return rlc_learning_std_dev(attr_ptr);
	default:
		break;
	}
	return RLC_FAILED;
}

static inline rlc_error_type rlc_learn_internal_out(rlc_attribute_type *attr_ptr, rlc_learning_internal_type type, uint32 size, uint32 *data)
{
	switch(type)
	{
	case RLC_LEARN:
    return rlc_learn_out(attr_ptr, attr_ptr->learning_ptr->learning_type, 0, NULL);
	case RLC_LEARNING_DATA:
		return rlc_learning_add(attr_ptr, size, data);
	default:
		break;
	}
	return RLC_FAILED;
}

static inline rlc_error_type rlc_correct_out(rlc_attribute_type *attr_ptr)
{
	switch(attr_ptr->dyn_corr_ptr->dyn_corr)
	{
	case RLC_CORR_SET_VAR_BC:
		return rlc_set_clust(attr_ptr);
	default:
		break;
	}
	return RLC_FAILED;
}


/*=============================================================================
                        
                        EXTERNAL FUNCTIONS

=============================================================================*/

/** 
RLC report task loop.

@return
None.

@dependencies
None.

*/

void rlc_r_task(void)
{
  rlc_command_type *command_ptr = NULL;
  //rlc_command_type *input_cmd_ptr = NULL;
  //rlc_error_type rlc_error = RLC_FAILED;

  rcinit_handshake_startup();

  while(1)
  {
    rlc_sig_wait(rlc.rlc_report_sig);

    while(RLC_EMPTY != rlc_r_pop_command(&command_ptr)) //command_ptr points to popped command after this
		{
			if(command_ptr != NULL)
			{
				rlc_report_out(rlc_map_internal_attribute(command_ptr->ext_attr), command_ptr->payload, command_ptr->payload_size);
				RLC_FREE(command_ptr->payload);
			}

			RLC_FREE(command_ptr);
		}
  }
}

/** 
RLC learn task loop.

@return
None.

@dependencies
None.

*/

void rlc_l_task(void)
{
  rlc_command_type *command_ptr = NULL;
  rlc_error_type rlc_error = RLC_FAILED;

  rcinit_handshake_startup();

  while(1)
  {
    rlc_sig_wait(rlc.rlc_learn_sig);

    while(RLC_EMPTY != rlc_l_pop_command(&command_ptr))
		{
			if(command_ptr != NULL)
			{
			  rlc_error=rlc_learn_internal_out(rlc_map_internal_attribute(command_ptr->ext_attr), command_ptr->type, command_ptr->payload_size, (uint32 *)command_ptr->payload);
				RLC_FREE(command_ptr->payload);
			}

			if( rlc_error != RLC_SUCCESS)
			{
				MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"There was an error learning/clustering: %s\n", rlc_error);
			}

			RLC_FREE(command_ptr);
		}
  }
}

/** 
RLC correct task loop.

@return
None.

@dependencies
None.

*/

void rlc_c_task(void)
{
  rlc_command_type *command_ptr = NULL;
  rlc_error_type rlc_error = RLC_FAILED;

  rcinit_handshake_startup();

  while(1)
  {
    rlc_sig_wait(rlc.rlc_correct_sig);

		while(RLC_EMPTY != rlc_c_pop_command(&command_ptr))
		{
			
			if(command_ptr != NULL)
			{
				rlc_error=rlc_correct_out(rlc_map_internal_attribute(command_ptr->ext_attr));
				//TODO: RLC correct call
				//RLC_FREE(command_ptr->payload);
			}

			if( rlc_error != RLC_SUCCESS)
			{
				MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"There was an error correcting: %s\n", rlc_error);
			}

			RLC_FREE(command_ptr);
		}
	}
}

/** 
Initialization function for RLC.
Initializes command queue, semaphores and mutexes needed for functionality.

@return
None.

@dependencies
None.

*/

void rlc_init(void)
{
  rlc.handle_list = NULL;
  rlc.report_command_list_first = NULL;
  rlc.report_command_list_last = NULL;
  rlc.learn_command_list_first = NULL;
  rlc.learn_command_list_last = NULL;
  rlc.correct_command_list_first = NULL;
  rlc.correct_command_list_last = NULL;

  rlc_heap_init();

  rlc_init_mutex(&rlc.rlc_mutex);
  rlc_init_mutex(&rlc.rlc_r_mutex);
  rlc_init_mutex(&rlc.rlc_l_mutex);
  rlc_init_mutex(&rlc.rlc_c_mutex);

  rlc_init_semaphore(&rlc.rlc_report_sig);
  rlc_init_semaphore(&rlc.rlc_learn_sig);
  rlc_init_semaphore(&rlc.rlc_correct_sig);

  rlc_create_thread(&rlc_r_task);
  rlc_create_thread(&rlc_l_task);
  rlc_create_thread(&rlc_c_task);
}

/** 
Registers a handle with RLC.

@param[out] handle  Pointer to a pointer to a handle that will be populated by the function.
@param[in]  name    Client name to be attached to the handle.

@return
RLC_SUCCESS -- Handle successfully created and pointer to the handle is returned through 'handle'
RLC_NO_MEMORY -- No memory available to create the handle 
 
@dependencies  
None.
*/

rlc_error_type rlc_register_handle( rlc_handle_ptr  *handle, const char* name )
{
  rlc_internal_handle_type *tmpHandle;
  rlc_internal_handle_type* internalHandle = (rlc_internal_handle_type*) rlc_malloc(sizeof(rlc_internal_handle_type));
  rlc_handle_type* externalHandle = (rlc_handle_type*) rlc_malloc(sizeof(rlc_handle_type));
  if(internalHandle == NULL ||
     externalHandle == NULL)
  {
    RLC_FREE(internalHandle);
    RLC_FREE(externalHandle);
    return RLC_NO_MEMORY;
  }

  internalHandle->next =      NULL;
  internalHandle->attribute = NULL;
  externalHandle->internal_handle = internalHandle;
  rlc_strcpy(internalHandle->name,name,RLC_HANDLE_NAME_MAX);
  rlc_strcpy(externalHandle->name,name,RLC_HANDLE_NAME_MAX);
  
  rlc_lock_mutex(&rlc.rlc_mutex);

  tmpHandle = rlc.handle_list;  
  if(tmpHandle != NULL)
  {
    while(tmpHandle->next != NULL)
    {
      tmpHandle = tmpHandle->next;
    }
    tmpHandle->next = internalHandle;
  }
  else
  {
    rlc.handle_list = internalHandle;
  }

  rlc_unlock_mutex(&rlc.rlc_mutex);

  /* Make the external handle equal to the address of the internal */
  *handle = (rlc_handle_ptr)externalHandle;

  return RLC_SUCCESS;
}

/** 
Unregisters a handle from RLC.

@param[out] handle  Pointer to an RLC handle.

@return
None.
 
@dependencies  
None.
*/
void rlc_unregister_handle( rlc_handle_ptr handle )
{
  rlc_internal_handle_type* internalHandle = rlc_map_internal_handle( handle );
  rlc_internal_handle_type* prevHandle = NULL;
  rlc_attribute_type* tmp_attr;
	rlc_attribute_type* attr = NULL;

	if(internalHandle != NULL)
	{
		attr = internalHandle->attribute; 
	}

	if( rlc.handle_list == internalHandle )
  {
    prevHandle = NULL;
  }
  else
  {
    prevHandle = rlc.handle_list;

    while( prevHandle->next != internalHandle && prevHandle != NULL )
      prevHandle = prevHandle->next;

    if( prevHandle == NULL )
    {
      /* Fatal error! Handle not found! */
      return;
    }
  }

  /* Let's clean up this handle of all the memory allocated */
  while( attr != NULL )
  {
    if( attr->attr_mutex != NULL )
    {
      rlc_lock_mutex( &attr->attr_mutex );
      rlc_l_remove_attr_commands(rlc_map_external_attribute(attr));
      rlc_unlock_mutex( &attr->attr_mutex );
      rlc_destroy_mutex( &attr->attr_mutex );
    }

    /* Free Dynamic Correction Ptr */
    if( attr->dyn_corr_ptr != NULL )
    {
      rlc_destroy_correcting_attribute( attr );
    }

    /* Free Report Ptr */ 
    if( attr->report_ptr != NULL )
    {
      //TODO
    }

    /* Free Learning Ptr */
    if( attr->learning_ptr != NULL )
    {
      rlc_learning_anomaly_detection_destroy( attr );
      rlc_free(attr->learning_ptr);
    }

    tmp_attr = attr->next;

    rlc_free( attr );

    attr = tmp_attr;
  }

  if( prevHandle == NULL )
  {
		if( internalHandle != NULL )
		{
			rlc.handle_list = internalHandle->next; 
		}
		else
		{
			rlc.handle_list = NULL;
		}
  }
  else
  {
		if( internalHandle != NULL )
		{
			prevHandle->next = internalHandle->next; 
		}
  }

  /* Free Internal Handle */
  rlc_free( internalHandle );
  /* Free External Handle */
  rlc_free( handle );
}

/** 
Adds a reporting attribute to the client handle within RLC.

@param[in]  handle          Pointer to a client handle.
@param[in]  report_output   Type of reporting to perform.
@param[in]  reporting_data  Reporting-specific data.
@param[out] attr_ret        Client name to be attached to the handle.

@return
RLC_SUCCESS -- Handle successfully created and pointer to the handle is returned through 'handle'
RLC_NO_MEMORY -- No memory available to create the handle 
 
@dependencies  
1) handle has successfully been registered.
*/

rlc_error_type rlc_add_reporting_attribute( rlc_handle_ptr              handle,
                                                    rlc_report_output_type      report_output,
                                                    uint32                          reporting_data,
                                                    rlc_external_attribute_type *attr_ret /* Get attribute pointer */
                                                )
{
  rlc_internal_handle_type *internal_handle = NULL;
  rlc_attribute_type *internal_attribute = NULL;
  rlc_attribute_type *alloced_attribute = NULL;
  rlc_attribute_type *prev_attribute = NULL;
  rlc_report_attribute_type *report_attribute = NULL;
  rlc_error_type status = RLC_FAILED;

  /* Check if the parameters are correct */
  /* TODO: Need to check if it's a valid handle (on the list) */
  if(handle == NULL ||
     attr_ret == NULL)
  {
    return RLC_INVALID_PARAMETER;
  }

  internal_handle = rlc_map_internal_handle(handle);

  alloced_attribute = (rlc_attribute_type *) rlc_malloc (sizeof(rlc_attribute_type));
  report_attribute = (rlc_report_attribute_type *) rlc_malloc (sizeof(rlc_report_attribute_type));
  if(alloced_attribute == NULL || report_attribute == NULL)
  {
    RLC_FREE(alloced_attribute);
    RLC_FREE(report_attribute);
    return RLC_NO_MEMORY;
  }

  internal_attribute = internal_handle->attribute;
  /* Check to see if there are any current attributes */
  if(internal_attribute != NULL)
  {
    while(internal_attribute->next != NULL)
    {
      internal_attribute = internal_attribute->next;
    }
    internal_attribute->next = alloced_attribute;
    prev_attribute = internal_attribute;
    internal_attribute = alloced_attribute;
  }
  else
  {
    /* First attribute for this handle */
    internal_handle->attribute = alloced_attribute;
    internal_attribute = alloced_attribute;
  }

  /* Initialize attribute structure */
  internal_attribute->report_ptr = report_attribute;
  internal_attribute->dyn_corr_ptr = NULL;
  internal_attribute->learning_ptr = NULL;
  internal_attribute->next = NULL;
  internal_attribute->state = RLC_STATE_OK;
  internal_attribute->attribute_structure = RLC_ATTRIBUTE_REPORT;
  internal_attribute->attr_mutex = NULL;
  rlc_init_mutex(&internal_attribute->attr_mutex);

  report_attribute->report_output = report_output;
  
  status = rlc_setup_report_attribute(handle,internal_attribute,reporting_data);
  if(status != RLC_SUCCESS)
  {
    /* Free up the memory that was just allocated since it's useless */
    RLC_FREE(report_attribute);
    RLC_FREE(internal_attribute);
    
    if(prev_attribute != NULL)
    {
      /* If this is not the only attribute on the list.. */
      prev_attribute->next = NULL;
    }
    else
    {
      /* There are no other attributes on the list */
      internal_handle->attribute = NULL;
    }

    return RLC_FAILED;
  }

  *attr_ret = (rlc_external_attribute_type)internal_attribute;

  return RLC_SUCCESS;
}

/** 
Reports on the attribute that has previously been added to the client handle.

@param[in]  attr    External attribute that has been returned from adding report attribute.
@param[in]  mode    Bitmask of the mode that will determine how the reporting is done.
@param[in]  string  Buffer filled with reporting-specific data.
@param[in]  size    Total number of bytes to report in the buffer.

@return
RLC_SUCCESS -- Handle successfully created and pointer to the handle is returned through 'handle'
RLC_NO_MEMORY -- No memory available to create the handle 
 
@dependencies  
1) Attribute attr has successfully been added to the handle.
*/
                                                    
rlc_error_type rlc_report(  rlc_external_attribute_type attr,
                                    rlc_mode_type        mode,
                                    void                            *string,
                                    uint32                          length
                                    )
{
  rlc_command_type *cmdPtr = NULL;

  if( attr   == NULL ||
      string == NULL)
  {
    return RLC_INVALID_PARAMETER;
  }

  if( mode & RLC_REPORT_ASYNC_MODE)
  {
    /* Report asynchronously */
    cmdPtr = (rlc_command_type*)rlc_malloc( sizeof(rlc_command_type) );
    if(cmdPtr == NULL)
    {
      return RLC_NO_MEMORY;
    }

    /* Fill out contents */
    cmdPtr->ext_attr = attr;
    cmdPtr->payload_size = length;

    cmdPtr->payload = rlc_malloc( length );
    if(cmdPtr->payload == NULL)
    {
      RLC_FREE(cmdPtr);
      return RLC_NO_MEMORY;
    }

    rlc_memcpy(cmdPtr->payload, cmdPtr->payload_size, string, length );

    rlc_r_queue_command(cmdPtr);

    /* Wake up rlc */
    rlc_set_sig(rlc.rlc_report_sig);

    return RLC_SUCCESS;
  }
  else
  {
    /* Report synchronously */ 
    return rlc_report_out(rlc_map_internal_attribute(attr), string, length);
  }
}


/** 
Adds a learning attribute to the client handle within RLC.

@param[in]  handle          RLC handle to attach attribute to.
@param[in]  learning_type   Enumeration stating what type of learning is to be done on the data passed in.
@param[in]  size            Size of data, must be a power of 2!
@param[in]	numDimensions	Number of dimensions.
@param[in]	numClusters		Number of clusters
@param[out] attr_ret        Pointer to the external attribute handle.

@return
RLC_SUCCESS -- Handle successfully created and pointer to the handle is returned through 'handle'
RLC_NO_MEMORY -- No memory available to create the handle 
 
@dependencies  
1) Attribute attr has successfully been added to the handle.
*/
                                                    
rlc_error_type rlc_add_learning_attribute( rlc_handle_ptr              handle,
                                           rlc_learning_type           learning_type,
                                           uint32                      size,
										   uint32					   numDimensions,
										   uint32					   numClusters,
                                           rlc_external_attribute_type *attr_ret /* Get attribute pointer */
                                           )
{
  rlc_internal_handle_type *internal_handle = NULL;
  rlc_attribute_type *internal_attribute = NULL;
  rlc_attribute_type *alloced_attribute = NULL;
  rlc_attribute_type *prev_attribute = NULL;
  rlc_learning_attribute_type *learning_attribute = NULL;
  rlc_error_type status = RLC_FAILED;

  /* Check if the parameters are correct */
  /* TODO: Need to check if it's a valid handle (on the list) */
  if(handle == NULL ||
     attr_ret == NULL)
  {
    return RLC_INVALID_PARAMETER;
  }

  internal_handle = rlc_map_internal_handle(handle);

  alloced_attribute = (rlc_attribute_type *) rlc_malloc (sizeof(rlc_attribute_type));
  learning_attribute = (rlc_learning_attribute_type *) rlc_malloc (sizeof(rlc_learning_attribute_type));
  if(alloced_attribute == NULL || learning_attribute == NULL)
  {
    RLC_FREE(alloced_attribute);
    RLC_FREE(learning_attribute);
    return RLC_NO_MEMORY;
  }

  internal_attribute = internal_handle->attribute;
  /* Check to see if there are any current attributes */
  if(internal_attribute != NULL)
  {
    while(internal_attribute->next != NULL)
    {
      internal_attribute = internal_attribute->next;
    }
    internal_attribute->next = alloced_attribute;
    prev_attribute = internal_attribute;
    internal_attribute = alloced_attribute;
  }
  else
  {
    /* First attribute for this handle */
    internal_handle->attribute = alloced_attribute;
    internal_attribute = alloced_attribute;
  }

  /* Initialize attribute structure */
  internal_attribute->report_ptr = NULL;
  internal_attribute->dyn_corr_ptr = NULL;
  internal_attribute->learning_ptr = learning_attribute;
  internal_attribute->next = NULL;
  internal_attribute->state = RLC_STATE_OK;
  internal_attribute->attribute_structure = RLC_ATTRIBUTE_LEARN; 
  internal_attribute->attr_mutex = NULL;
  rlc_init_mutex(&internal_attribute->attr_mutex);

  learning_attribute->learning_type = learning_type;
  
  status = rlc_setup_learning_attribute(handle,internal_attribute,size,numDimensions,numClusters);
  if(status != RLC_SUCCESS)
  {
    /* Free up the memory that was just allocated since it's useless */
    RLC_FREE(learning_attribute);
    RLC_FREE(internal_attribute);
    
    if(prev_attribute != NULL)
    {
      /* If this is not the only attribute on the list.. */
      prev_attribute->next = NULL;
    }
    else
    {
      /* There are no other attributes on the list */
      internal_handle->attribute = NULL;
    }

    return RLC_FAILED;
  }



  *attr_ret = (rlc_external_attribute_type)internal_attribute;


  return RLC_SUCCESS;
}


/** 
Adds data to a learning attribute.

@param[in]  attr    External attribute that has been returned from adding learning attribute.
@param[in]  mode	Whether to learn data synchronously or asynchronously.
@param[in]  numArgs Number of arguments to be read from var args.
@param[in]  data... New data obtained.

@return
RLC_SUCCESS -- Handle successfully created and pointer to the handle is returned through 'handle'
RLC_FAILED  -- Wrong data or size.
RLC_NO_MEMORY -- No memory available to create the handle 
 
@dependencies  
1) Attribute attr has successfully been added to the handle.
2) Learning must have been initialized previously
3) Number of data arguments must match up with dimensions.
*/
rlc_error_type rlc_learn_data(rlc_external_attribute_type		attr,
						                  uint32                        mode,
									            uint32	                      numArgs, 
                              ...)
{
	rlc_command_type *cmdPtr = NULL;
	rlc_error_type eError;
	uint32 i=0;
	uint32 num=0;
	va_list args;
	uint32 *data=NULL;
	rlc_attribute_type* int_attr = rlc_map_internal_attribute(attr);

	if( attr == NULL )
		return RLC_INVALID_PARAMETER;

	/* Consolodate data if adding */

	/* Check for attribute state */
	if( int_attr->state != RLC_STATE_OK )
	{
		/* If we're currently Learning, don't take more data */
		return RLC_LEARNING;
	}

	va_start(args, numArgs);

	data=(uint32 *)rlc_malloc(numArgs * sizeof(uint32));

	if(data == NULL)
	{
		va_end(args);
		return RLC_NO_MEMORY;
	}

	for(i=0; i < numArgs; i++)
	{
		num=va_arg(args, uint32);
		data[i] = num;
	}

	va_end(args);

	/* Check mode of learning */
	if( mode & RLC_LEARN_ASYNC_MODE )
	{
    if( mode & RLC_LEARN_SYNC_MODE &&
        TRUE == rlc_trylock_mutex( &int_attr->attr_mutex ) )
    {
		  eError = rlc_learn_internal_out(int_attr, RLC_LEARNING_DATA, numArgs, data);	
		  RLC_FREE(data);
      rlc_unlock_mutex( &int_attr->attr_mutex );
		  return eError;
    }

		/* Learn asynchronously */
    cmdPtr = (rlc_command_type*)rlc_malloc( sizeof(rlc_command_type) );
		if(cmdPtr == NULL)
		{
			RLC_FREE(data);
			return RLC_NO_MEMORY;
		}

		/* Fill out contents */
		cmdPtr->ext_attr = attr;
		cmdPtr->type = RLC_LEARNING_DATA;

		cmdPtr->payload=data;
		cmdPtr->payload_size=numArgs;

		rlc_l_queue_command(cmdPtr);

		/* Wake up rlc */
		rlc_set_sig(rlc.rlc_learn_sig);

		return RLC_SUCCESS;
	}
	else
	{
		eError = rlc_learn_internal_out(rlc_map_internal_attribute(attr), RLC_LEARNING_DATA, numArgs, data);	
		RLC_FREE(data);
		return eError;
	}
}

/** 
Performs learning on an attribute that has previously been added to the client handle.

@param[in]  attr    External attribute that has been returned from adding learning attribute.
@param[in]  mode	Whether to learn synchronously or asynchronously.
@param[in]  type    Whether to cluster, add, or perform statistical analysis.
@param[in]  numArgs Number of arguments to be read from var args.
@param[in]  data... New data obtained.

@return
RLC_SUCCESS -- Handle successfully created and pointer to the handle is returned through 'handle'
RLC_FAILED  -- Wrong data or size.
RLC_NO_MEMORY -- No memory available to create the handle 
 
@dependencies  
1) Attribute attr has successfully been added to the handle.
2) Learning must have been initialized previously
3) Number of data arguments must match up with dimensions.
*/
rlc_error_type rlc_learn(rlc_external_attribute_type	attr,
						             uint32                       mode)
{
	rlc_command_type *cmdPtr = NULL;
	rlc_error_type eError;
  rlc_attribute_type* attr_int = rlc_map_internal_attribute(attr);

	if( attr == NULL )
		return RLC_INVALID_PARAMETER;

	/* Check mode of learning */
	if( mode & RLC_LEARN_ASYNC_MODE )
	{
		/* Learn asynchronously */

		cmdPtr = (rlc_command_type*)rlc_malloc( sizeof(rlc_command_type) );
		if(cmdPtr == NULL)
		{
			return RLC_NO_MEMORY;
		}

		/* Fill out contents */
		cmdPtr->ext_attr = attr;
		cmdPtr->type = RLC_LEARN;

		rlc_l_queue_command(cmdPtr);

		/* Wake up rlc */
		rlc_set_sig(rlc.rlc_learn_sig);

		return RLC_SUCCESS;
	}
	else
	{
    eError = rlc_learn_out(rlc_map_internal_attribute(attr), attr_int->learning_ptr->learning_type, 0, NULL);	
		return eError;
	}
}

/** 
Performs learning on an attribute that has previously been added to the client handle.

@param[in]  attr    External attribute that has been returned from adding learning attribute.
@param[in]  mode	Whether to learn synchronously or asynchronously.

@return
RLC_SUCCESS -- Handle successfully created and pointer to the handle is returned through 'handle'
RLC_FAILED  -- Wrong data or size.
RLC_NO_MEMORY -- No memory available to create the handle 
 
@dependencies  
1) Attribute attr has successfully been added to the handle.
2) Learning must have been initialized previously
3) Number of data arguments must match up with dimensions.
*/
rlc_error_type rlc_learn_internal(rlc_external_attribute_type	  attr,
						                      uint32                        mode,
                                  rlc_learning_internal_type    type)
{
	rlc_command_type *cmdPtr = NULL;
	rlc_error_type eError;

	if( attr == NULL )
		return RLC_INVALID_PARAMETER;

	/* Check mode of learning */
	if( mode & RLC_LEARN_ASYNC_MODE )
	{
		/* Learn asynchronously */

		cmdPtr = (rlc_command_type*)rlc_malloc( sizeof(rlc_command_type) );
		if(cmdPtr == NULL)
		{
			return RLC_NO_MEMORY;
		}

		/* Fill out contents */
		cmdPtr->ext_attr = attr;
		cmdPtr->type = type;

		rlc_l_queue_command(cmdPtr);

		/* Wake up rlc */
		rlc_set_sig(rlc.rlc_learn_sig);

		return RLC_SUCCESS;
	}
	else
	{
    eError = rlc_learn_internal_out(rlc_map_internal_attribute(attr), type, 0, NULL);	
		return eError;
	}
}

/**
Adds a correcting attribute to the client handle within RLC

@param[in]  handle  RLC handle attribute is attached to.
@param[in]  correcting_type	RLC dynamic correction type.
@param[in]	func	Pointer to function used for correction
@param[in]	attr_learn		External learning attribute with the learned data.
@param[out] attr_ret        Pointer to the external attribute handle.

@return
RLC_SUCCESS -- Handle successfully created and pointer to the handle is returned through 'handle'
RLC_FAILED  -- Wrong data or size.
RLC_NO_MEMORY -- No memory available to create the handle 
*/

rlc_error_type rlc_add_correcting_attribute_sc( rlc_handle_ptr              handle,
										   rlc_dynamic_correction_type correcting_type,
										   void (*func)(uint32),
										   rlc_external_attribute_type attr_learn,
                                           rlc_external_attribute_type *attr_ret /* Get attribute pointer */
										   )
{
  rlc_internal_handle_type *internal_handle = NULL;
  rlc_attribute_type *internal_attribute = NULL;
  rlc_attribute_type *alloced_attribute = NULL;
  rlc_attribute_type *prev_attribute = NULL;
  rlc_correction_attribute_type *correcting_attribute = NULL;
  rlc_error_type status = RLC_FAILED;

  /* Check if the parameters are correct */
  /* TODO: Need to check if it's a valid handle (on the list) */
  if(handle == NULL ||
     attr_ret == NULL)
  {
    return RLC_INVALID_PARAMETER;
  }

  internal_handle = rlc_map_internal_handle(handle);

  alloced_attribute = (rlc_attribute_type *) rlc_malloc (sizeof(rlc_attribute_type));
  correcting_attribute = (rlc_correction_attribute_type *) rlc_malloc (sizeof(rlc_correction_attribute_type));
  if(alloced_attribute == NULL || correcting_attribute == NULL)
  {
    RLC_FREE(alloced_attribute);
    RLC_FREE(correcting_attribute);
    return RLC_NO_MEMORY;
  }

  internal_attribute = internal_handle->attribute;
  /* Check to see if there are any current attributes */
  if(internal_attribute != NULL)
  {
    while(internal_attribute->next != NULL)
    {
      internal_attribute = internal_attribute->next;
    }
    internal_attribute->next = alloced_attribute;
    prev_attribute = internal_attribute;
    internal_attribute = alloced_attribute;
  }
  else
  {
    /* First attribute for this handle */
    internal_handle->attribute = alloced_attribute;
    internal_attribute = alloced_attribute;
  }

  /* Initialize attribute structure */
  internal_attribute->report_ptr = NULL;
  internal_attribute->dyn_corr_ptr = correcting_attribute;
  internal_attribute->learning_ptr = NULL;
  internal_attribute->next = NULL;
  internal_attribute->state = RLC_STATE_OK;
  internal_attribute->attribute_structure = RLC_ATTRIBUTE_DYNAMIC_CORRECTION; 
  internal_attribute->attr_mutex = NULL;
  rlc_init_mutex(&internal_attribute->attr_mutex);

  //learning_attribute->learning_type = learning_type;
  correcting_attribute->dyn_corr = correcting_type;
  
  
  status = rlc_setup_correcting_attribute(handle,internal_attribute, func, rlc_map_internal_attribute(attr_learn));
  if(status != RLC_SUCCESS)
  {
    /* Free up the memory that was just allocated since it's useless */
    RLC_FREE(correcting_attribute);
    RLC_FREE(internal_attribute);
    
    if(prev_attribute != NULL)
    {
      /* If this is not the only attribute on the list.. */
      prev_attribute->next = NULL;
    }
    else
    {
      /* There are no other attributes on the list */
      internal_handle->attribute = NULL;
    }

    return RLC_FAILED;
  }
	


  *attr_ret = (rlc_external_attribute_type)internal_attribute;


  return RLC_SUCCESS;
}

/** 
Performs set cluster correction on the attribute that has previously been added to the client handle.

@param[in]  attr    External attribute that has been returned from adding correction attribute.
@param[in]  mode    Bitmask of the mode that will determine how the correcting is done.



@return
RLC_SUCCESS -- Handle successfully created and pointer to the handle is returned through 'handle'
RLC_NO_MEMORY -- No memory available to create the handle 
 
@dependencies  
1) Attribute attr has successfully been added to the handle.
*/
                                                    
rlc_error_type rlc_correct_sc(  rlc_external_attribute_type attr,
                                    rlc_mode_type        mode
                                    )
{
  rlc_command_type *cmdPtr = NULL;

  if( attr == NULL )
  {
    return RLC_INVALID_PARAMETER;
  }

  if( mode & RLC_CORRECT_ASYNC_MODE)
  {
    /* Correct asynchronously */
    cmdPtr = (rlc_command_type*)rlc_malloc( sizeof(rlc_command_type) );
    if(cmdPtr == NULL)
    {
      return RLC_NO_MEMORY;
    }

    /* Fill out contents */
    cmdPtr->ext_attr = attr;

    rlc_c_queue_command(cmdPtr);

    /* Wake up rlc */
    //rlc_set_sig(rlc.rlc_correct_sig);
	//NOTE: THIS WILL BE WOKEN UP AT THE END OF CLUSTERING

    return RLC_SUCCESS;
  }
  else
  {
    /* Report synchronously */ 
    return rlc_correct_out(rlc_map_internal_attribute(attr));
  }
}


/**
Wakes up the RLC correction task

@return
None

@dependencies
None
*/
void rlc_c_wake()
{
	rlc_set_sig(rlc.rlc_correct_sig);
}

/**
Gets the dimension of best cluster location

@param[in]	attr	Pointer to external RLC correction attribute
@param[in]	dim		Dimension of cluster

@return
uint32 -- Value at dimension of best cluster location

@dependencies
1) Clustering performed
2) Dimension must be within valid range in data
*/

uint32 rlc_get_best( rlc_external_attribute_type attr, uint32 dim )
{
	rlc_attribute_type *attr_ptr;
	rlc_correction_struct *attrstruct;

	if(attr == NULL)
	{
		return 0xFFFF;
	}

	attr_ptr=rlc_map_internal_attribute(attr);
	
	if(attr_ptr == NULL || attr_ptr->dyn_corr_ptr->dyn_corr_handle == NULL)
	{
		return 0xFFFF;
	}

	attrstruct=(rlc_correction_struct *)attr_ptr->dyn_corr_ptr->dyn_corr_handle;

	/* Check that the dimension is within the data bounds */
	if( dim >= attrstruct->learning_ptr->numDimensions )
	{
		return 0xFFFF;
	}

	/* Return location dimension */
	return attrstruct->learning_ptr->clusters[attrstruct->learning_ptr->bestCluster].location[dim];
}

/**
Gets cluster location dimension value (API)

@param[in]	attr_ptr	Pointer to an RLC learning attribute structure
@param[in]	clusterNum	Cluster number
@param[in]	dimension	Dimension of which value you want (first=0)

@return
uint32 -- Value of cluster location at specified dimension

@dependencies
1) Chosen dimension must be less than the number of dimensions
*/

uint32 rlc_get_cluster_location( rlc_external_attribute_type attr_ptr, uint32 clusterNum, uint32 dimension )
{
	rlc_correction_struct *attrstruct=NULL;

	if( attr_ptr == NULL )
		return RLC_INVALID_PARAMETER;

	attrstruct=(rlc_correction_struct *)((rlc_map_internal_attribute(attr_ptr))->dyn_corr_ptr->dyn_corr_handle);

	if(attrstruct == NULL)
	{
		return RLC_FAILED;
	}

	if(dimension >= attrstruct->learning_ptr->numDimensions || clusterNum >= attrstruct->learning_ptr->numClusters)
	{
		return RLC_INVALID_PARAMETER;
	}

	return attrstruct->learning_ptr->clusters[clusterNum].location[dimension];
}

/** 
Map the external attribute from an internal attribute

@param[in]	internal_attr	Internal attribute

@return
rlc_external_attribute_type

@dependencies
None.
*/

rlc_external_attribute_type rlc_map_external_attribute(rlc_attribute_type *internal_attribute)
{
	return (rlc_external_attribute_type)internal_attribute;
}

/** 
Get data associated with an attribute.

@param[in]  attr          	Attribute to get data from.
@param[in]  attr_data_type	Data type which is being requested from attribute.
@param[in]  buffer_size	Size of the client supplied buffer.
@param[out] buffer		Client supplied buffer to fill requested data with.

@return
RLC_SUCCESS – Data correctly populated in buffer
RLC_FAILED – Error retrieving data
 
@dependencies  
1) Attributes must already be created.
*/

rlc_error_type rlc_get_attr_data( rlc_external_attribute_type 	attr,
                                  rlc_attr_data_type 			      attr_data_type,
                                  uint32 				                buffer_size,
                                  void* 					              buffer )
{
  rlc_attribute_type* int_attr = rlc_map_internal_attribute(attr);
  rlc_learning_struct* learning_attr = (rlc_learning_struct *)(int_attr->learning_ptr->learning_handle);
  uint32 nIter;
  
  if( attr == NULL ||
      buffer == NULL ||
      buffer_size == 0 )
  {
    return RLC_INVALID_PARAMETER;
  }

  switch( attr_data_type )
  {
  case RLC_ATTR_DATA_L_GET_AVERAGE:

    rlc_lock_mutex(&int_attr->attr_mutex);

    if( ( sizeof(uint32) * learning_attr->numDimensions ) > buffer_size )
    {
      rlc_unlock_mutex(&int_attr->attr_mutex);
      return RLC_INVALID_SIZE;
    }

    if( learning_attr->avg_found == 0 )
    {
      rlc_unlock_mutex(&int_attr->attr_mutex);
      return RLC_LEARNING;
    }

    rlc_memcpy( buffer, buffer_size, &learning_attr->averages[0], sizeof(uint32) * learning_attr->numDimensions );

    rlc_unlock_mutex(&int_attr->attr_mutex);

    return RLC_SUCCESS;

  case RLC_ATTR_DATA_L_GET_STD_DEV:

    rlc_lock_mutex(&int_attr->attr_mutex);

    if( ( sizeof(uint32) * learning_attr->numDimensions ) > buffer_size )
    {
      rlc_unlock_mutex(&int_attr->attr_mutex);
      return RLC_INVALID_SIZE;
    }

    if( learning_attr->std_dev_found == 0 )
    {
      rlc_unlock_mutex(&int_attr->attr_mutex);
      return RLC_LEARNING;
    }

    rlc_memcpy( buffer, buffer_size, &learning_attr->std_dev[0], sizeof(uint32) * learning_attr->numDimensions );

    rlc_unlock_mutex(&int_attr->attr_mutex);

    return RLC_SUCCESS;

  case RLC_ATTR_DATA_L_GET_CLUST_LOCS:

    rlc_lock_mutex(&int_attr->attr_mutex);

    if( ( sizeof(uint32)*learning_attr->numDimensions*learning_attr->numClusters) > buffer_size )
    {
      rlc_unlock_mutex(&int_attr->attr_mutex);
      return RLC_INVALID_SIZE;
    }

    if(  learning_attr->firstTime == 1 ) 
    {
      rlc_unlock_mutex(&int_attr->attr_mutex);
      return RLC_LEARNING;
    }

    for( nIter = 0; nIter < learning_attr->numClusters; nIter++ )
    {
      rlc_memcpy( (void*)((uint32)buffer + (nIter*sizeof(uint32)*learning_attr->numDimensions)), 
              buffer_size - (nIter*sizeof(uint32)*learning_attr->numDimensions),
              learning_attr->clusters[nIter].location, 
              sizeof(uint32)*learning_attr->numDimensions );  
    }

    rlc_unlock_mutex(&int_attr->attr_mutex);
    
    return RLC_SUCCESS;

  case RLC_ATTR_DATA_L_GET_BEST_CLUST:

    rlc_lock_mutex(&int_attr->attr_mutex);

    if( ( sizeof(uint32) * learning_attr->numDimensions ) > buffer_size )
    {
      rlc_unlock_mutex(&int_attr->attr_mutex);
      return RLC_INVALID_SIZE;
    }

    for( nIter = 0; nIter < learning_attr->numClusters; nIter++ )
    {
      
    }

    rlc_memcpy( buffer, buffer_size, &learning_attr->averages[0], sizeof(uint32) * learning_attr->numDimensions );

    rlc_unlock_mutex(&int_attr->attr_mutex);

    return RLC_SUCCESS;

  case RLC_ATTR_DATA_L_GET_WORST_CLUST:
	case RLC_ATTR_DATA_L_GET_NUM_CLUSTS:
	default:
    break;
  }
  return RLC_FAILED;
}


/**
Gets averages of n-dimensional array

@param[in]	data			2-Dimensional array containing the data values for each dimension.
@param[in]	numDimensions	The number of dimensions of data values.
@param[in]	size			Number of data points.
@param[in]	div_shift		Number of times to divide by to get average.
@param[out] averages		Array storing the resulting averages.

@return
RLC_SUCCESS
RLC_NO_MEMORY

@dependencies
1) Need data
*/

rlc_error_type rlc_get_averages(rlc_attribute_type *attr_ptr)
{
	uint32 i=0;
	uint32 j=0;
	uint32 total=0;
  uint32 rounding_err;
  rlc_learning_struct *temp=NULL;

  temp=(rlc_learning_struct *)(attr_ptr->learning_ptr->learning_handle);

  rlc_lock_mutex( &attr_ptr->attr_mutex );

  rounding_err = temp->size >> 2; /* Add size/2 to the total to increase avg by 0.5 */

  temp->watermark = temp->current;
  
  temp->firstTime = 0;

	for( i=0; i < temp->numDimensions; i++ )
	{
		total=0;
		for( j=0; j < temp->size; j++ )
		{
			total+=temp->data[j].array_ptr[i];
		}
		temp->averages[i]=(( total + rounding_err ) >> temp->div_by_shift);
	}
	
	temp->avg_found=1;

  temp->firstTime = 1;

  rlc_unlock_mutex( &attr_ptr->attr_mutex );

	return RLC_SUCCESS;
}


