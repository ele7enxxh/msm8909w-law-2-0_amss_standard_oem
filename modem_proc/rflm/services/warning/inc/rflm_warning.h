/*!
  @file
  rflm_warning.h

  @brief
  header file for app warnings.
  
  @detail
  To add new warning enum, JUST add new entry in RFLM_CONFIG_WARNING_TABLE macro in
  rflm_warning_enum.h.  NOTHING else needs to be done.
  
  To indicate occurrence of a warning, add in your code:
    RFLM_WARNING( <warning enum name>, "warning description");
  where <warning enum name> is the 1st parameter added in 
  RFLM_CONFIG_WARNING_TABLE entry
  
  Depending on how the warning is setup, the RFLM_WARNING will output 
  trace message and/or trigger an assertion failure.
  
*/

/*===========================================================================

  Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/services/warning/inc/rflm_warning.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
01/28/14   jc      Change warning assert handling to after trace/msg
12/16/13   Saul    CCS. Increase number of tasks per TQ to 32.
06/21/13   JC      Created
===========================================================================*/

#ifndef RFLM_WARNING_H
#define RFLM_WARNING_H

#include "rflm_types.h"
#include "rflm_warning_enum.h"

/*===========================================================================

      Constants

===========================================================================*/

/*! @brief Bitmasks for warning flags. These must be unique.
*/
#define RFLM_WARNING_ASSERT_EN   (0x1)
#define RFLM_WARNING_TRACE_EN    (0x2)
#define RFLM_WARNING_MSG_EN      (0x4)          


#define RFLM_WARNING_EFS_OVERRIDE_SIZE  (16)

/*===========================================================================

      Macros

===========================================================================*/

/*! @brief Generate a warning of the specified type.
    type is the warning type, from RFLM_CONFIG_WARNING_TABLE (no RFLM_WARNING prefix)
    msg and additional arguments are printf-style additional info
*/
#define RFLM_WARNING(type,msg, ...) do { \
  uint32 _action = rflm_warning_indicate( type ); \
  if (_action & RFLM_WARNING_TRACE_EN) \
  { \
    RFLM_DIAG_TRACE_ERROR(RFLM_DIAG_MSG_SSID_NONE, #type ": " msg, ## __VA_ARGS__); \
  } \
  if (_action & RFLM_WARNING_MSG_EN) \
  { \
    RFLM_DIAG_MSG_HIGH( #type ": " msg, ## __VA_ARGS__); \
  } \
  if ( _action & RFLM_WARNING_ASSERT_EN) \
  { \
    RFLM_DIAG_ERR_FATAL("Error due to " #type); \
  } \
} while (0)


#define RFLM_WARNING_VAR(type,msg, ...) do {                        \
  uint32 _action = rflm_warning_indicate( type ); 								  \
  if (_action & RFLM_WARNING_TRACE_EN) 														  \
  { 																															  \
  	RFLM_DIAG_TRACE_ERROR(RFLM_DIAG_MSG_SSID_NONE, "Warning (%d):" msg, (int)type, ## __VA_ARGS__ );  \
  } 																															  \
  if (_action & RFLM_WARNING_MSG_EN) 															  \
  { 																															  \
    RFLM_DIAG_MSG_HIGH( "Warning (%d):" msg, (int)type, ## __VA_ARGS__ ); \
  } \
  if ( _action & RFLM_WARNING_ASSERT_EN) \
  { \
    RFLM_DIAG_ERR_FATAL("Warning (%d):" msg, (int)type, ## __VA_ARGS__ ); \
  } \
} while (0)

  

/*===========================================================================

      Typedefs

===========================================================================*/

/* ! @brief 
      Used with APP_CONFIG_PARAM_TABLE to generate app param enumeration list
*/
#define RFLM_PARAM_ENUM(N,A) N,

/* ! @brief Enumeration list of run-time warnings.
            Note this does not need change when new warnings are added.
*/
typedef enum
{
  RFLM_CONFIG_WARNING_TABLE(RFLM_PARAM_ENUM)   /*lint !e123 */
  RFLM_WARNING_ENUM_COUNT
} rflm_warning_e;


/*! @brief  Structure containing warning name, occurrence counter and action
            flags. Note this does not need change when new warnings are added.
*/
typedef struct
{
  const char   *name;   ///< name of warning
  uint32        count;  ///< count of occurrences
  uint8         action; ///< bitmask indicating action to take
} rflm_warning_t;


typedef struct
{
  rflm_warning_t array[RFLM_WARNING_ENUM_COUNT];
} rflm_warning_array_s;



/*! @brief  Structure containing EFS override configuration
*/
typedef struct
{
  int8              num_entries;    ///< # of override entries
  struct 
  { 
    rflm_warning_e  warning_id;    
    uint8           action;
  }                 entry[RFLM_WARNING_EFS_OVERRIDE_SIZE];  
} rflm_warning_efs_ovrd_s;



/*===========================================================================

      Functions

===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

extern rflm_warning_t rflm_warning_data[ RFLM_WARNING_ENUM_COUNT ];

extern uint32 rflm_warning_indicate( rflm_warning_e warning_enum );

extern void rflm_warning_clear_counters( void );

extern void rflm_warning_print_non_zero_count( void );

extern void rflm_warning_set_override_action( rflm_warning_efs_ovrd_s *ovrd_param );

#ifdef __cplusplus
}
#endif

#endif /* RFLM_WARNING_H */

