/*!
  @file
  rflm_diagi_def.h

  @brief
  rflm_diag internal definitions.

  @detail
  This file gets included by both C and assembly.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/services/diag/inc/rflm_diagi_def.h#1 $

when         who     what, where, why
----------   ---     --------------------------------------------------------
2013-12-02   jc      Increased trace buffer size to 64kB (from 32kB)
2013-07-18   JC      Port from fw_diagi_def.h

===========================================================================*/

#ifndef RFLM_DIAGI_DEF_H
#define RFLM_DIAGI_DEF_H

#include "rflm_defs.h"
#include "rflm_target.h"


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/



/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Section definitions - prefer TCM if available. 
*/
#ifdef RFLM_SECTION_TCM_DATA
#define TCM_SECTION  RFLM_SECTION_TCM_DATA
#else
#define TCM_SECTION
#endif

#ifdef RFLM_SECTION_TCM_CODE
#define CODE_SECTION  RFLM_SECTION_TCM_CODE
#else
#define CODE_SECTION 
#endif

#ifdef RFLM_SECTION_L1WB_L2UC_BSS
#define TRACE_BUFFER_SECTION RFLM_SECTION_L1WB_L2UC_BSS
#else
#define TRACE_BUFFER_SECTION
#endif



/*! @brief log2 of the number of items in the circular trace buffer.
    This must be large enough that every thread gets a chance to finish its
    update before the index wraps around.
    Each item will use 32 bytes, so total size will be 32*2^x bytes. 
    Allow overriding externally if desired. 
*/
#ifndef RFLM_DIAG_TRACE_BUFFER_LEN_LOG2
#define RFLM_DIAG_TRACE_BUFFER_LEN_LOG2     11
#endif

/*! @brief Number of items in the circular trace buffer.
*/
#define RFLM_DIAG_TRACE_BUFFER_LEN          ( 1 << (RFLM_DIAG_TRACE_BUFFER_LEN_LOG2) )

/*! @brief Mask to convert sequence number to trace buffer index.
*/
#define RFLM_DIAG_TRACE_BUFFER_MASK         ( RFLM_DIAG_TRACE_BUFFER_LEN - 1 )



/*! @brief Byte offsets into the trace message structure.
*/
#define TRACE_OFS_MSG             0       
#define TRACE_OFS_SEQ             4
#define TRACE_OFS_TIMESTAMP       8
#define TRACE_OFS_ARG             12

#ifndef __ASSEMBLER__

/*! @brief Trace message item.
*/
typedef struct CACHE_ALIGN
{
  const rflm_diag_const_msg_param_t *msg;  /*!< Constant message parameters */
  uint32 seq;                            /*!< Sequence number */
  uint32 timestamp;                      /*!< Timestamp */
  int32 arg[5];                          /*!< Message arguments */

} rflm_diag_trace_msg_t;

COMPILE_ASSERT(OFFSETOF(rflm_diag_trace_msg_t, msg)       == TRACE_OFS_MSG);
COMPILE_ASSERT(OFFSETOF(rflm_diag_trace_msg_t, seq)       == TRACE_OFS_SEQ);
COMPILE_ASSERT(OFFSETOF(rflm_diag_trace_msg_t, timestamp) == TRACE_OFS_TIMESTAMP);
COMPILE_ASSERT(OFFSETOF(rflm_diag_trace_msg_t, arg)       == TRACE_OFS_ARG);

COMPILE_ASSERT((TRACE_OFS_ARG % 8) == 4);
COMPILE_ASSERT((TRACE_OFS_ARG-TRACE_OFS_TIMESTAMP) == 4);
COMPILE_ASSERT((TRACE_OFS_MSG % 8) == 0);
COMPILE_ASSERT((TRACE_OFS_SEQ-TRACE_OFS_MSG) == 4);

#endif  // __ASSEMBLER__


#endif /* RFLM_DIAGI_DEF_H */
