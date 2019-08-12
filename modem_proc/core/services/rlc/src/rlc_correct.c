/**
@file rlc_correct.c
@brief
This module implements the Correction module in RLC
*/
/*=============================================================================
  Copyright (c) 2013 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                   R L C   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Correction functions for the RLC module.


=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/12/13   as      Initial revision.

=============================================================================*/


/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "rlc.h"
#include "rlc_v.h"
#include "rlc_os.h"
#include "rlc_correct.h"
#include "rlc_learning.h"
#include <stdlib.h>
#include <stdio.h>

/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/




/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/

/*
Sets up correction information for a set cluster correction attribute.

@param[in]	attr_ptr	Pointer to an RLC correction attribute structure
@param[in]	attr_learn	Pointer to an RLC learning attribute that contains learning data.

@return
RLC_SUCCESS
RLC_FAILURE

@dependencies
None.
*/

rlc_error_type rlc_correct_init_sc( rlc_attribute_type *attr_ptr, rlc_attribute_type *attr_learn, void(*func)(uint32) )
{
	rlc_correction_struct *attr_struct=NULL;
	rlc_correction_list *temp;

	if(attr_ptr == NULL || attr_learn == NULL)
	{
		return RLC_FAILED;
	}

	attr_struct=(rlc_correction_struct *)rlc_malloc(sizeof(rlc_correction_struct));

	if(attr_struct == NULL)
	{
		return RLC_FAILED;
	}
	attr_struct->sc_func=func;
	attr_struct->learning_ptr=(rlc_learning_struct *)(attr_learn->learning_ptr->learning_handle);

	if( attr_struct->learning_ptr == NULL )
	{
		return RLC_NO_MEMORY;
	}

	/* Go to learning struct, add to list of corrections */

	if( attr_struct->learning_ptr->correction_head == NULL )
	{
		attr_struct->learning_ptr->correction_head=(rlc_correction_list *)rlc_malloc(sizeof(rlc_correction_list));
		if(attr_struct->learning_ptr->correction_head == NULL)
		{
			return RLC_NO_MEMORY;
		}
		attr_struct->learning_ptr->correction_head->correct=attr_ptr;
		attr_struct->learning_ptr->correction_head->next=NULL;
	}
	else
	{
		temp=attr_struct->learning_ptr->correction_head;
		while(temp->next != NULL)
		{
			temp=temp->next;
		}
		temp->next=(rlc_correction_list *)rlc_malloc(sizeof(rlc_correction_list));
		if( temp->next == NULL)
		{
			return RLC_NO_MEMORY;
		}
		temp=temp->next;
		temp->correct=attr_ptr;
		temp->next=NULL;
	}

	/* Link correction struct to attribute */
	attr_ptr->dyn_corr_ptr->dyn_corr_handle=attr_struct;

	

	return RLC_SUCCESS;
}

/*
Destroyed SC Correcting Attribute.

@param[in]	attr_ptr	Pointer to an RLC correction attribute structure

@return
RLC_SUCCESS
RLC_FAILURE

@dependencies
None.
*/

rlc_error_type rlc_correct_destroy_sc( rlc_attribute_type *attr_ptr )
{
	if(attr_ptr == NULL )
	{
		return RLC_FAILED;
	}

  rlc_free( attr_ptr->dyn_corr_ptr->dyn_corr_handle );

	return RLC_SUCCESS;
}

/*
Passes in cluster to specified function

@param[in]	attr_ptr	Pointer to an RLC correction attribute structure
@param[in]	type		Type of set variable correction
@param[in]	func		Callback function

@return
RLC_SUCCESS
RLC_FAILURE

@dependencies
None
*/

rlc_error_type rlc_set_clust( rlc_attribute_type *attr_ptr )
{
	rlc_correction_struct *attrstruct=NULL;

	if(attr_ptr == NULL)
	{
		return RLC_INVALID_PARAMETER;
	}

	if(attr_ptr->dyn_corr_ptr->dyn_corr != RLC_CORR_SET_VAR_BC && attr_ptr->dyn_corr_ptr->dyn_corr != RLC_CORR_SET_VAR_WC)
	{
		return RLC_FAILED;
	}

	attrstruct=(rlc_correction_struct *)(attr_ptr->dyn_corr_ptr->dyn_corr_handle);

	if(attr_ptr->dyn_corr_ptr->dyn_corr == RLC_CORR_SET_VAR_BC)
	{
		attrstruct->sc_func(attrstruct->learning_ptr->bestCluster);
	}

	return RLC_SUCCESS;
}
