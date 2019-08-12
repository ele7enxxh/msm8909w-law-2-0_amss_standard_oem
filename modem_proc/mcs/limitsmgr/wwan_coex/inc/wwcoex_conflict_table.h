#ifndef __WWCOEX_CONFLICT_TABLE_H__
#define __WWCOEX_CONFLICT_TABLE_H__
/*!
  @file wwcoex_conflict_table.h

  @brief
   Interface to the conflict table for the WWAN COEX module
 
*/

/*=============================================================================

  Copyright (c) 2013-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

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

=============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/wwan_coex/inc/wwcoex_conflict_table.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/22/14   sg      Support for spur mitigation
07/03/14   jm      Support for desense calculation in DR-DSDS
01/29/14   jm      Increase max number of concurrent active techs
01/14/14   jm      Ensure freqID is 16-bit value
05/05/13   ag      Moved init function
03/24/13   ag      Initial Revision

=============================================================================*/
/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "comdef.h"
#include "wwcoex_action_iface.h"

#ifdef __cplusplus
extern "C" {
#endif
/*=============================================================================

                       CONSTANTS AND DEFINES

=============================================================================*/
/*! Maximum number of concurrent active tech scenarios */
#define COEX_MAX_CONCURRENT_TECHS        4
#define COEX_MAX_CONCURRENT_SCENARIOS (COEX_MAX_CONCURRENT_TECHS*(COEX_MAX_CONCURRENT_TECHS-1)/2) 
#define WWCOEX_TBL_INVALID  (int8)-1
#define WWCOEX_MAX_TABLES_PER_SCENARIO   2

/* Spur related macros */
#define WWCOEX_MAX_SPUR_TABLE_SIZE       256
#define WWCOEX_SPUR_HANDLE_NONE          0
#define WWCOEX_SPUR_HANDLE_UNKNOWN       0xFFF0

/*! These are duplicate macros to the ones existing in wwan_coex_mgr.h. 
    Compile time assert in place to ensure these are always in sync */
#define WWCOEX_MAX_SUPPORTED_LINKS   24
#define WWCOEX_UNKNOWN_FREQID     0x0000FFF0

/*=============================================================================

                             TYPEDEFS

=============================================================================*/
typedef struct 
{
  cxm_action_type   action;
  wwan_coex_desense_type  desense;
  uint16                  spur_handle_1;
  uint16                  spur_handle_2;
}coex_result_type;

typedef struct 
{
  /* Table for tech 1, tech 2 conflicts */
  coex_result_type arr[WWCOEX_MAX_SUPPORTED_LINKS][WWCOEX_MAX_SUPPORTED_LINKS];

  /* Freq ID offsets of each tech used for this table */
  uint32 tech1_fid_offset;
  uint32 tech2_fid_offset;

  uint32 tech1_num_entries;
  uint32 tech2_num_entries;

  boolean is_valid;
}wwcoex_tbl_type;

typedef struct
{
  /* Tables for tech 1, tech 2 conflicts */
  wwcoex_tbl_type tables[WWCOEX_MAX_TABLES_PER_SCENARIO];

  uint8   tech1;
  uint8   tech2;

  int8           current_tbl;
}wwcoex_conflict_tbls_type;

/*=============================================================================

                        FUNCTION DECLARATIONS

=============================================================================*/


/*=============================================================================

  FUNCTION:  wwcoex_get_table_node

=============================================================================*/
/*!
    @brief
    Get the conflict table node corresponding to the two tech pair. If there
    is no table for the pair, then return an unused node.
 
    @return
    conflict table node

*/
/*===========================================================================*/
wwcoex_conflict_tbls_type* wwcoex_get_table_node
(
  uint8   tech1,
  uint8   tech2
);

/*=============================================================================

  FUNCTION:  wwcoex_invalidate_tables

=============================================================================*/
/*!
    @brief
    Invalidates the conflict tables containing the specified tech
 
    @return
    NONE

*/
/*===========================================================================*/
void wwcoex_invalidate_tables
(
  uint8  tech
);

/*=============================================================================

  FUNCTION:  wwcoex_get_oldest_tbl_index

=============================================================================*/
/*!
    @brief
    Returns index to the oldest table
 
    @return
    NONE

*/
/*===========================================================================*/
int8 wwcoex_get_oldest_tbl_index
(
  wwcoex_conflict_tbls_type*  conflict_tbl
);

/*===========================================================================
FUNCTION wwcoex_set_sub_state

DESCRIPTION
  This API will update the subscription state

DEPENDENCIES 
  None

RETURN VALUE  
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void wwcoex_set_sub_state
(
  uint32 num_standby_stacks,
  uint32 num_active_stacks
);

/*===========================================================================
FUNCTION wwcoex_set_rf_mode

DESCRIPTION
  This API will update the RF mode

DEPENDENCIES 
  None

RETURN VALUE  
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void wwcoex_set_rf_mode
(
  boolean is_drdsds_enabled
);

/*===========================================================================
FUNCTION wwcoex_set_spur_mitigation_mask

DESCRIPTION
  This API will update the RF mode

DEPENDENCIES 
  None

RETURN VALUE  
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void wwcoex_set_spur_mitigation_mask
(
  boolean is_spur_mitigation_enabled
);


#ifdef __cplusplus
}
#endif

#endif /* __WWCOEX_CONFLICT_TABLE_H__ */
