/*!
  @file
  rflm.h

  @brief
  RFLM common types/macros/functions.

  @detail
  This is a meta-include file of all the required standard headers for RFLM code.
  Do not include if API header files used outside of RFLM.
  This is for internal RFLM use only.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/services/includes/inc/rflm.h#1 $

when         who     what, where, why
----------   ---     --------------------------------------------------------
2014-08-26   rca     Port of latest SCRUTINY mode from BOLT.
2014-08-21   rca     SCRUTINY mode run time enable
2014-02-18   kg      Moved header includes order
2013-08-06   jyw     Moved common header files from the def header
2013-07-18   JC      Port from modem_fw.h
===========================================================================*/

#ifndef RFLM_H
#define RFLM_H

// avoid redefinitions from modem_fw
#define MODEM_FW_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*! @brief common header file */
#include <string.h>

// avoid fw warnings in gtest builds - @todo: remove once fw fixes warning
#ifdef GTEST_HAS_POSIX_RE

#ifndef STUB_STRINGL_H
#define STUB_STRINGL_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

extern size_t strlcpy(char *dst, const char *src, size_t size);
static inline size_t memscpy(void *dst, size_t dst_size, const void *src, size_t src_size)
{
  if (src_size > dst_size)
  {
    //src_size=dst_size;
  }  
  memcpy(dst,src,src_size);
  return src_size;
}

#ifdef __cplusplus
}  // extern "C"
#endif

#endif

#else

#include <stringl.h>

#endif


/* common definitions and macros (and comdef.h re-definitions) */
#include "rflm_defs.h"

/* non-target feature definition */
#include "rflm_features.h"

/* RFLM types compatible with MDSP-specific types */
#include "rflm_types.h" 

/* Utility macros and inline functions. */
#include "rflm_util.h"  

/* Target-specific definitions */
#include "rflm_target.h"

/* NOT including rflm_hwintf.h. Include that only in source that needs it. */

/* NOT including q6types.h. Use the vect32/vect64 types instead of Q6V_ macros. */


/* SCRUTINY ONLY enabled on BOLT and THOR for now 08/22/14 (Remove #if later when supported for all that share RFLM) */
#define RFLM_CCS_SCRUTINY_MODE

/*lint -e641 */

#endif /* RFLM_H */
