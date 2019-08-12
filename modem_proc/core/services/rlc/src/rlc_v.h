#ifndef RLC_V_H
#define RLC_V_H
/**
@file rlc_v.h
@brief
This module implements the general private functions for the RLC module. 
*/
/*=============================================================================
  Copyright (c) 2012-2014 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                   R L C   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  General private functions for the RLC module.


=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/28/12   cp      Initial revision.

=============================================================================*/


/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "comdef.h"
#include <memory.h>
#include "rlc.h"

#ifdef FEATURE_RLC_PERF
#include "timetick.h"
#endif /*FEATURE_RLC_PERF*/

/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/

//#define FEATURE_RLC_PERF
#define RLC_VAR_NAME_MAX 16
#define RLC_HANDLE_NAME_MAX 8

#define RLC_GET_UPPER_32_BIT_FROM_PAYLOAD(x) *(uint32*)x
#define RLC_GET_LOWER_32_BIT_FROM_PAYLOAD(x) *(uint32*)(x + sizeof(uint32))
#define RLC_GET_UPPER_64_BIT_FROM_PAYLOAD(x) *(uint64*)x
#define RLC_GET_LOWER_64_BIT_FROM_PAYLOAD(x) *(uint64*)(x + sizeof(uint64))

#define RLC_FREE(x); \
          if(x) { \
            rlc_free(x);\
            x = NULL;\
          }\

#ifdef FEATURE_RLC_PERF

#define RLC_PERF_DEF(X,Y); \
          uint32 X = 0; \
          uint32 Y = 0; \

#define RLC_PERF_START(X,Y); \
          X = timetick_get(); \

#define RLC_PERF_END(Z,X,Y); \
          Y = timetick_get(); \
          MSG_HIGH(Z,X,Y,Y-X); \

#else /*FEATURE_RLC_PERF*/

#define RLC_PERF_DEF(X,Y);
#define RLC_PERF_START(X,Y);
#define RLC_PERF_END(Z,X,Y);

#endif /*FEATURE_RLC_PERF*/

typedef void* rlc_report_output_handle;
typedef void* rlc_dynamic_correction_handle;
typedef void* rlc_learning_handle;

typedef enum{
  RLC_ATTRIBUTE_NONE,
  RLC_ATTRIBUTE_REPORT,
  RLC_ATTRIBUTE_DYNAMIC_CORRECTION,
  RLC_ATTRIBUTE_REPORT_W_DYNAMIC_CORRECTION,
  RLC_ATTRIBUTE_LEARN,
  RLC_ATTRIBUTE_MAX
}
rlc_attribute_structure_type;

typedef struct{
  rlc_report_output_type        report_output;
  rlc_report_output_handle      report_handle;
}
rlc_report_attribute_type;

typedef struct{
  rlc_dynamic_correction_type   dyn_corr;
  rlc_dynamic_correction_handle dyn_corr_handle;
}
rlc_correction_attribute_type;

typedef struct{
  rlc_learning_type   learning_type;
  rlc_learning_handle learning_handle;
}
rlc_learning_attribute_type;

/* Optimize by using some sort of bitwise operation (fast way) */
/* Create a master list of expressions vs. variables */
typedef struct _rlc_attribute_type{
  rlc_attribute_structure_type  attribute_structure;          
  struct _rlc_attribute_type    *next;
  rlc_attr_state                state;
  rlc_mutex_ptr                 attr_mutex;
  rlc_report_attribute_type     *report_ptr;
  rlc_learning_attribute_type   *learning_ptr;
  rlc_correction_attribute_type *dyn_corr_ptr;
}
rlc_attribute_type;

typedef struct _rlc_internal_handle_type{
  char                                  name[RLC_HANDLE_NAME_MAX];
  rlc_attribute_type                *attribute;
  struct _rlc_internal_handle_type  *next;
}
rlc_internal_handle_type;

typedef struct _rlc_command_type{
  rlc_external_attribute_type       ext_attr;
  struct _rlc_command_type          *next;
  rlc_learning_internal_type			  type; /* Type of operation to be performed if learning */
  rlc_dynamic_correction_type			  corr_type; /* Type of dynamic correction to be performed */
  uint32                            payload_size; /* Size of the payload below */
  void                              *payload; /* Dynamically sized element to pass in client data */
}
rlc_command_type;

typedef struct{
  rlc_internal_handle_type *handle_list; /* Singly linked list of handles */
  rlc_mutex_ptr             rlc_mutex;
  rlc_mutex_ptr             rlc_r_mutex;
  rlc_mutex_ptr             rlc_l_mutex;
  rlc_mutex_ptr             rlc_c_mutex;
  rlc_sig_ptr               rlc_report_sig;
  rlc_sig_ptr               rlc_learn_sig;
  rlc_sig_ptr               rlc_correct_sig;
  rlc_command_type          *report_command_list_first;
  rlc_command_type          *report_command_list_last;
  rlc_command_type          *learn_command_list_first;
  rlc_command_type          *learn_command_list_last;
  rlc_command_type          *correct_command_list_first;
  rlc_command_type          *correct_command_list_last;
}
rlc_internal_struct;

/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/

/** 
Maps the external RLC handle to an internal RLC handle.

@param[in] handle  External RLC handle pointer.

@return
Pointer to the internal RLC handle on success, NULL otherwise.
 
@dependencies  
None.
*/

rlc_internal_handle_type* rlc_map_internal_handle(rlc_handle_ptr handle);

/**
Wakes up the RLC correction task

@return
None

@dependencies
None
*/
void rlc_c_wake(void);

/** 
Map the external attribute from an internal attribute

@param[in]	internal_attr	Internal attribute

@return
rlc_external_attribute_type

@dependencies
None.
*/

rlc_external_attribute_type rlc_map_external_attribute(rlc_attribute_type *internal_attribute);

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

rlc_error_type rlc_destroy_learning_attribute( rlc_attribute_type          *attr_ptr );

#endif /*RLC_V_H*/
