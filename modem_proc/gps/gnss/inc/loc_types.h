#ifndef LOCATION_TYPES_H_INCLUDED
#define LOCATION_TYPES_H_INCLUDED

/**
 * @file
 * @brief Target-specific header file
 *
 * This header file contains target-specific amendments to standard libraries
 * and functions. The file shall not be explicitly used as header as it is
 * force included by build environment.
 */
/*----------------------------------------------------------------------------
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * -------------------------------------------------------------------------*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/inc/loc_types.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include <comdef.h>
#include <inttypes.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#ifdef __hexagon__
/* Hexagon defines 32-bit as long, but PRI macros say it is int.
 * This issues is resolved in newer compiler releases.
 */
#undef PRIu32
#undef PRId32
#undef PRIo32
#undef PRIx32
#undef PRIX32
#define PRIu32 "lu"
#define PRId32 "ld"
#define PRIo32 "lo"
#define PRIx32 "lx"
#define PRIX32 "lX"

#undef SCNd32
#undef SCNi32
#undef SCNo32
#undef SCNu32
#undef SCNx32
#define SCNd32 "ld"
#define SCNi32 "ld"
#define SCNo32 "lo"
#define SCNu32 "lu"
#define SCNx32 "lx"

#endif /* __hexagon__ */

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef float  FLT;
typedef double DBL;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* LOCATION_TYPES_H_INCLUDED */
