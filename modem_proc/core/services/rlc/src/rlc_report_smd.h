#ifndef RLC_REPORT_SMD_H
#define RLC_REPORT_SMD_H
/**
@file rlc_report_smd.h
@brief
This module implements reporting through the shared memory driver. 
*/
/*=============================================================================
  Copyright (c) 2012 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                   R L C   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Private RLC functions for reporting through SMD.


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
#include "rlc_v.h"

/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/

#define RLC_REPORT_TO_APPS_MASK  RLC_REPORT_TO_APPS_INTERNAL_MASK
#define RLC_REPORT_TO_MPSS_MASK  RLC_REPORT_TO_MPSS_INTERNAL_MASK
#define RLC_REPORT_TO_WCNSS_MASK RLC_REPORT_TO_WCNSS_INTERNAL_MASK
#define RLC_REPORT_TO_ADSP_MASK  RLC_REPORT_TO_ADSP_INTERNAL_MASK

#define RLC_SMD_NUM_PORTS   256
#define RLC_SMD_MAGIC       0xDEADBEEFuL

typedef struct{
  uint32 smd_channel;
  uint32 bit_mask;
  void   *smdPort;
}
rlc_smd_protocol_table;

typedef struct{
  void* txPort;
  void* rxPort;
}
rlc_smd_table_type;

typedef struct {
  void* smdPortPtr;
}
rlc_smd_report_to_table_type;

typedef struct{
  rlc_smd_table_type  rlc_smd_port_table[RLC_SMD_NUM_PORTS];
  rlc_smd_report_to_table_type rlc_smd_report_to_table[RLC_REPORT_TO_MAX];
}
rlc_smd_internal_type;

typedef struct{
  uint32 reporting_port;
  uint32 smd_port;
}
rlc_smd_client_handle;

/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/

/** 
Private function for openning SMD ports that are available in order to
allow for reporting to various peripheral images.

@return
None.
 
@dependencies  
None.
*/

void rlc_smd_init(void);

/** 
Private function for setting up client information within internal attribute.

@param[in]  pAttribute      Internal attribute pointer to pass in 
                            and write client info.
@param[in]  reporting_data  Data to pass into the SMD initialization.  This info
                            is used in setting up the SMD port.

@return
RLC_SUCCESS --  Successfully setup SMD client for reporting.
RLC_FAILED  --  Error setting up SMD client for reporting.
 
@dependencies  
None.
*/

rlc_error_type rlc_report_smd_init(rlc_attribute_type *pAttribute, uint32 reporting_data);

/** 
Private function for reporting through SMD.

@param[in]  pAttribute  Internal attribute pointer to pass in client info.
@param[in]  string      Buffer of data to report on.

@return
RLC_SUCCESS --  Successfully reported through SMD.
RLC_FAILED  --  Error reporting through SMD.
 
@dependencies  
SMD should be initialized by this point and attribute information
should be filled out in init function.
*/

rlc_error_type rlc_report_smd(rlc_attribute_type *pAttribute, char* string);

#endif /*RLC_REPORT_SMD_H*/
