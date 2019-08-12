#ifndef PM_NPA_H
#define PM_NPA_H

/*! \file pm_npa.h
 *  \n
 *  \brief This file contains enums and struct types used to define different
 *         PMIC resources configurations and also function prototypes to handle
 *         internal RPM clients.
 *  
 *  \n &copy; Copyright 2010-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/npa/inc/pm_npa.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/10/13   rk      Fixed the naming convention in \config.
09/10/13   rk      Code refactoring.
12/14/2010 umr     NPA Transaction support
06/03/2010 umr     Created.
===========================================================================*/
/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "rpm.h"
#include "pm_npa_device.h"
#include "hw_module_type.h"
/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/
typedef struct
{
  void *rail_data; /* Type of client to create */
  pm_npa_resouce_type rail_type;
}pm_npa_pam_client_cfg_type;

typedef struct pm_npa_node_resource_info_type
{
    const char                              *group_name;     /* Name of resource */
    unsigned                                 max_mode;       /* Maximum state/length of vector resource */
    unsigned int                             resource_attributes;     /* Resource Attributes */
    pm_npa_pam_client_cfg_type              *resource_data;  /* User data field  */

    const char                              *node_name;      /* Node name */
	unsigned int                             node_attributes;   /* Node Attributes */
    npa_user_data                           *node_data;       /* User data */
	npa_node_dependency                     *node_dependencies;     /* Dep array */
}pm_npa_node_resource_info;

typedef enum
{
   PM_RESOURCE_OPERATION__INVALID = 0,   // Don't use this index
   PM_RESOURCE_OPERATION__GT = 1,        // Boolean Greater than
   PM_RESOURCE_OPERATION__GT_EQ = 2,     // Boolean Greater than equal
   PM_RESOURCE_OPERATION__LT = 3,        // Boolean Less than
   PM_RESOURCE_OPERATION__LT_EQ = 4,     // Boolean Less than equal to
   PM_RESOURCE_OPERATION__EQ_EQ = 5,     // Boolean equal
   PM_RESOURCE_OPERATION__NOT_EQ = 6,    // Boolean Not equal to
   PM_RESOURCE_OPERATION__NONE = 7,      // This is a no operation dependency. Always True
   PM_RESOURCE_OPERATION__EQ = 8,        // Assignment equal
   PM_RESOURCE_OPERATION__ADD = 9,       // Math addition operation
   PM_RESOURCE_OPERATION__MINUS = 10,    // Math subtraction operation
   PM_RESOURCE_OPERATION__MULTIPLY = 11, // Math multiplication operation
   PM_RESOURCE_OPERATION__DIVIDE = 12,   // Math division operation
}pm_pwr_resource_operation_type;

typedef enum
{
   PM_PWR_OPERAND__INVALID = 0,                  // Don't use this index
   PM_PWR_OPERAND__RESOURCE = 1,                 // Resource Key Operand
   PM_PWR_OPERAND__STATIC = 2,                   // Static constant
   PM_PWR_OPERAND__LUT = 3,                      // Look up Table by Resource Key
   PM_PWR_OPERAND__NA = 4,                       // Not applicable, this is used with PM_COMPONENT_RESOURCE_OPERATION__NONE
   PM_PWR_OPERAND__RECURSIVE_OPERAND = 5,        // Recursive Operand
}pm_pwr_operand_type;

typedef enum
{
   PM_PWR_TIMING_TYPE__NONE = 0,
   PM_PWR_TIMING_TYPE__PRE = 1,            // Pre condition - must be ran before the main resource setting
   PM_PWR_TIMING_TYPE__PRESYNC = 2,        // PreSync condition - must be ran pre-synchronously with main resource setting.
   PM_PWR_TIMING_TYPE__POST = 3,           // Post condition - must be ran after the main resource setting
   PM_PWR_TIMING_TYPE__POSTSYNC = 2        // PostSync condition - must be ran post-synchronously with main resource setting.
}pm_pwr_timing_type;

typedef struct
{
   rpm_resource_type     resource_type;
   unsigned              internal_resource_index; // 1 indexed (not zero indexed) resource index
}pm_pwr_resource;

// This represents the right or left operand of an equation
// example: left_operand >= rigtht_operand evaluates to boolean
typedef struct
{
   pm_pwr_operand_type operand_type; /*< static, component resource, LUT */
   void               *lut_table; /*< This is pointer to the LUT if the operand type is LUT */
   pm_pwr_resource    *resource; /*< This gives the Resource Type and Index */
   unsigned            resource_key; /*< Identifies the KVP that is being used in the expression */
   unsigned            static_value; /*< If the operand_type = static, this is the value of the static */
}pm_pwr_resource_operand;

// These will get created in the order of the index
typedef struct
{
   pm_pwr_resource_operand               *left_operand;
   unsigned                               left_operand_client_index;
   pm_pwr_resource_operation_type         operation_type;
   pm_pwr_resource_operand               *right_operand;
   pm_pwr_timing_type                     timingType; // 1-PRE; 2-PRESYNC; 3-POST; 4-POSTSYNC
}pm_pwr_resource_operation;

typedef struct
{
   pm_pwr_resource_operation *condition;
   unsigned                   result_count; // This is the number of results to execute.
   pm_pwr_resource_operation *results;
}pm_pwr_resource_condition_result;

typedef struct
{
   pm_pwr_resource                     *parent_source_dependency; // parent resource
   pm_pwr_resource                     **child_resource_dependents; // array of child resources
   unsigned                            parent_source_dependency_client_handle; // internal client handle to make calls to the parent resource
   //pm_pwr_resource_condition_result    **pre_operational_dependencies;
   //pm_pwr_resource_condition_result    **post_operational_dependencies;
}pm_pwr_resource_dependency_info;

typedef struct 
{
    uint32                 vreg_ok_us;   // settling time for off-to-on, in us.
    uint32                 stepper_us;   // settling time per stepper, in us
    uint32                 stepper_uv;   // voltage size per stepper, in uv
}pm_pwr_settling_time_info_type;

/*===========================================================================

                        GENERIC FUNCTION PROTOTYPES

===========================================================================*/

npa_resource_state
pm_npa_resource_set (npa_resource      *resource,
                     npa_client_handle  client,
                     npa_resource_state mode_id);

npa_resource_state
pm_npa_process_rsrc (npa_resource *resource,
                     npa_resource_state mode_id,
                     uint32 index);

#endif /* PM_NPA_H */

