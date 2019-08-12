/*!
  @file
  rflm_diagi.h

  @brief
  RFLM diagnostic services: internal header

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/services/diag/inc/rflm_diagi.h#1 $

when         who     what, where, why
----------   ---     --------------------------------------------------------
2013-09-19   JC      error handler name change
2013-07-18   JC      Port from fw_diagi.h

===========================================================================*/

#ifndef FW_DIAGI_H
#define FW_DIAGI_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "rflm_diag.h"
#include "rflm_diagi_def.h"
#if (defined(TEST_FRAMEWORK) && defined(FEATURE_BOLT_MODEM)) 
#error code not present
#endif
  #include <atomic_ops.h>
#if (defined(TEST_FRAMEWORK) && defined(FEATURE_BOLT_MODEM)) 
#error code not present
#endif


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


extern rflm_diag_trace_msg_t rflm_diag_trace_buffer[RFLM_DIAG_TRACE_BUFFER_LEN];

extern atomic_word_t rflm_diag_trace_seq_num;


/*! @brief Units of trace timestamps.
*/
typedef enum
{
  FW_DIAG_TIMESTAMP_RAW,
  FW_DIAG_TIMESTAMP_PCYCLE,
  FW_DIAG_TIMESTAMP_XO
} rflm_diag_timestamp_units_e;

extern volatile rflm_diag_timestamp_units_e rflm_diag_timestamp_units;



/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif


extern void rflm_diag_msg
(
  const rflm_diag_const_msg_param_t *msg,
  int32 *args
);

extern void rflm_diag_error_init(void);

// Do not mark this "noreturn" even though it doesn't return - we need it
// to generate a valid stack frame.
extern void rflm_diag_error_fatal_handler
(
  const rflm_diag_const_msg_param_t *msg,
  int32 arg1,
  int32 arg2,
  int32 arg3,
  int32 arg4,
  int32 arg5
);

extern void rflm_diag_save_registers(void);



/*===========================================================================

  FUNCTION:  rflm_diag_filename

===========================================================================*/
/*!
    @brief
    Return a pointer to a filename, without path.

    @detail
    Input is a full path/filename. Output is a pointer within the same
    string to just the filename.

    Only '/' is considered a valid pathname separator, which is consistent
    on both Windows and Linux.

    @return
    Pointer to filename.
*/
/*=========================================================================*/
STATIC_INLINE const char * rflm_diag_filename
(
  const char * filename  /*!< Input path/filename */
)
{

  /* The constant message contents have already removed the pathname using
     __builtin_strrchr() */
  return filename;

} /* rflm_diag_filename() */



#ifdef __cplusplus
} // extern "C"
#endif

#endif /* RFLM_DIAGI_H */
