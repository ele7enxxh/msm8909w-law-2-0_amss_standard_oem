#ifndef RLC_REPORT_EFS_H
#define RLC_REPORT_EFS_H
/**
@file rlc_report_efs.h
@brief
This module implements reporting through the embedded file system. 
*/
/*=============================================================================
  Copyright (c) 2012 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                   R L C   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Private RLC functions for reporting through EFS.


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

#include "rlc.h"

/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/

#define RLC_EFS_FILE_MAX_SIZE 0x10000 /* 64kB */
#define RLC_MAX_NAME_SIZE 32

typedef struct {
  int    file_handle;
  int    attr_handle;
  uint32 file_number;
  uint32 file_size;
  uint32 file_max_size;
  char   handle_name[RLC_MAX_NAME_SIZE];
  char   file_name[RLC_MAX_NAME_SIZE];
  char   attr_name[RLC_MAX_NAME_SIZE];
}
rlc_efs_type;

/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/

/** 
Private function for initializing reporting through EFS for a client.

@param[in]  handle      Client handle to be passed in for the client name.
@param[in]  pAttribute  Internal attribute pointer to fill in.

@return
RLC_SUCCESS --  Successfully setup EFS attribute for the client.
RLC_FAILED --   Error in setting up EFS attribute for the client.
 
@dependencies  
None.
*/

rlc_error_type rlc_report_efs_init(rlc_handle_ptr              handle,
                                           rlc_attribute_type          *pAttribute);

/** 
Private function for reporting through EFS.

@param[in]  pAttribute  Internal attribute pointer to pass in client info.
@param[in]  string      Buffer of data to report on.

@return
RLC_SUCCESS --  Successfully reported through EFS.
RLC_FAILED  --  Error reporting through EFS.
 
@dependencies  
pAttribute must be setup properly in private efs_init function.
*/

rlc_error_type rlc_report_efs(rlc_attribute_type *pAttribute, char* string);

#endif /*RLC_REPORT_EFS_H*/

