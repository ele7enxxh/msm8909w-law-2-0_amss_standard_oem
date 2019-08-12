/*!
  @file
  rflm_target.h

  @brief
  Target-specific definitions.

  @detail
  This file contains target specific definitions.
  

*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/services/includes/inc/rflm_target.h#1 $

when         who     what, where, why
----------   ---     --------------------------------------------------------
2013-07-18   JC      Port from modem_fw_target.h

===========================================================================*/

#ifndef RFLM_TARGET_H
#define RFLM_TARGET_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/* Section mappings 
   Add these macros to beginning of function / end of variable 
   declarations to specify memory space used.
   
   Function example:
   RFLM_SECTION_TCM_CODE void rflm_foo(void) 
   {
      ...
   } 
   
   Variable example:
   int32 rflm_foo_array[RFLM_FOO_SIZE] RFLM_SECTION_TCM_DATA
*/
#define RFLM_SECTION_TCM_CODE        SECTION_CODE(".tcm_static_code")
#define RFLM_SECTION_TCM_DATA        SECTION_CODE(".tcm_static_data")
#define RFLM_SECTION_L1WB_TCM_CODE   RFLM_SECTION_TCM_CODE
#define RFLM_SECTION_L1WB_TCM_DATA   RFLM_SECTION_TCM_DATA
#define RFLM_SECTION_L1WB_L2UC_DATA  SECTION_DATA(".data_l1wb_l2uc")
#define RFLM_SECTION_L1WB_L2UC_BSS   RFLM_SECTION_L1WB_L2UC_DATA
#define RFLM_SECTION_L1WT_L2WT_DATA  SECTION_DATA(".data_l1wt_l2wt")
#define RFLM_SECTION_UNCACHED_DATA   SECTION_DATA(".data_uncached")
#define RFLM_SECTION_SMEM             /* Uses default external memory */


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/


#endif /* RFLM_TARGET_H */
