#ifndef _MMDEFS_H
#define _MMDEFS_H
/*===========================================================================

                   S T A N D A R D    D E C L A R A T I O N S

  Defines common types used within Multimedia Subsystem. Attempt is made to
  align to C99 standard and intention is to keep the list of commonly used
  types small.

===========================================================================*/

/** @file mmdefs.h
*/

/*===========================================================================
Copyright (C) 2010-2011, 2012 Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
======================================================================== */

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/mmutils/mmdefs.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
2012/04/16 sw      (Tech Pubs) Updated for new Doxygen version; reorganized
                   organization of PDF.
2011/11/18 sw      (Tech Pubs) Added Doxygen comments for Interface Spec.
2010/05/17 act     Removing SWIG. Updating to use stdint.h on compilers that
                   we support.
2010/05/12 act     Moving TRUE/FALSE, bool, and char types outside STDINT_H
                   protection since C99 doesn't define those types.
2010/05/11 act     Adding char_t type.
2010/04/29 act     Added protection for redefinition of C99 types. Changed
                   _WIN32 check.
2010/04/27 act     Changed a few comments, removed some less-used definitions
2010/04/23 PR      Initial revision from comdef.h (stripped version)
===========================================================================*/

/** @addtogroup apr_core_data_types
@{ */
/* -----------------------------------------------------------------------*/
/** @name Standard Integer Types
@{ */
/* ----------------------------------------------------------------------- */

#if /* Supported Compilers */ \
    defined(__ARMCC_VERSION) || \
    defined(__GNUC__)

  /* If we're hosted, fall back to the system's stdint.h, which might have
   * additional definitions.
   */

  #include "stdint.h"

#else /* Unsupported Compilers */

  /* The following definitions are the same accross platforms.  This first
   * group are the sanctioned types.
   */

  typedef unsigned long long uint64_t;  /**< Unsigned 64-bit integer type. */
  typedef unsigned long int  uint32_t;  /**< Unsigned 32-bit integer type. */
  typedef unsigned short     uint16_t;  /**< Unsigned 16-bit integer type. */
  typedef unsigned char      uint8_t;   /**< Unsigned  8-bit integer type. */

  typedef signed long long   int64_t;   /**< Signed 64-bit integer type. */
  typedef signed long int    int32_t;   /**< Signed 32-bit integer type. */
  typedef signed short       int16_t;   /**< Signed 16-bit integer type. */
  typedef signed char        int8_t;    /**< Signed  8-bit integer type. */

#endif /* Supported Compilers */

/** @} */ /* end_name Standard Integer Types */
/** @} */ /* end_addtogroup apr_core_data_types */

/** @addtogroup apr_core_constants_macros
@{ */
/* ------------------------------------------------------------------------
** Constants
** ------------------------------------------------------------------------ */
#undef TRUE
#undef FALSE

#define TRUE   (1)  /**< Boolean value for TRUE. */
#define FALSE  (0)  /**< Boolean value for FALSE. */

#ifndef NULL
  #define NULL (0)  /**< NULL value. */
#endif

/** @} */ /* end_addtogroup apr_core_constants_macros */

/** @addtogroup apr_core_data_types
@{ */
/* ------------------------------------------------------------------------*/
/** @name Character and Boolean Types
@{ */
/* ------------------------------------------------------------------------ */
typedef char char_t;           /**< Eight-bit character type. */
typedef unsigned char bool_t;  /**< Eight-bit Boolean type. */

/** @} */ /* end_name Character and Boolean */
/** @} */ /* end_addtogroup apr_core_data_types */

#endif /* _MMDEFS_H */

