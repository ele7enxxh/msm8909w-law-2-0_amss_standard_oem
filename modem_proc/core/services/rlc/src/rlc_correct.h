#ifndef RLC_CORRECT_H
#define RLC_CORRECT_H
/**
@file rlc_correct.h
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

#include "comdef.h"
#include "rlc.h"
#include "rlc_learning.h"
#include "rlc_v.h"


/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/

typedef struct {
	rlc_learning_struct *learning_ptr;
	void (*sc_func)(uint32);
	uint32 count;
}
rlc_correction_struct;



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

rlc_error_type rlc_correct_init_sc( rlc_attribute_type *attr_ptr, rlc_attribute_type *attr_learn, void(*func)(uint32) );

/*
Destroys SC Correction Attribute.

@param[in]	attr_ptr	Pointer to an RLC correction attribute structure

@return
RLC_SUCCESS
RLC_FAILURE

@dependencies
None.
*/

rlc_error_type rlc_correct_destroy_sc( rlc_attribute_type *attr_ptr );

/*
Sets location of variable to best cluster location

@param[in]	attr_ptr	Pointer to an RLC correction attribute structure

@return
RLC_SUCCESS
RLC_FAILURE

@dependencies
None
*/

rlc_error_type rlc_set_clust( rlc_attribute_type *attr_ptr);

#endif /*RLC_CORRECT_H*/
