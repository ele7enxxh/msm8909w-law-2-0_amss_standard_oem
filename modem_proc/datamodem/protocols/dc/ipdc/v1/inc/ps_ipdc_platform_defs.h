#ifndef PS_IPDC_PLATFORM_DEFS_H
#define PS_IPDC_PLATFORM_DEFS_H

#ifdef __cplusplus
extern "C" {
#endif

/*==========================================================================

                 P S _ I P D C _ P L A T F O R M _ D E F S . H

GENERAL DESCRIPTION
  This file contains header file includes and macro definitions which vary
  based on platform where IPDC code is being compiled.
  Currently allows common code to exist for:
  - UE Build
  - MOB Build
  - TBS Build
  - IPDC Standalone Tool
  - IPDC Offtarget Tool
  - IPDC Q6sim Tool

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2015 QUALCOMM Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*==========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/dc/ipdc/v1/inc/ps_ipdc_platform_defs.h#1 $
  $DateTime: 2013/01/10 21:41:01 &
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/30/15   ss      Initial version

===========================================================================*/

/* Configure this specific to platform */

// Enable if IPDC code is running on TBS/BOA
// #define IPDC_ON_TBS

// Enable if IPDC code is running on MOB
// #define IPDC_ON_MOB

// Enable if IPDC code is running on the standalone tool
// #define IPDC_ON_STANDALONE_TOOL

// Enable if IPDC code is running on UE Target
#define IPDC_ON_TARGET

// Enable if IPDC code is running on Q6sim
//#define IPDC_ON_Q6SIM

// Enable if IPDC code is running on IPDC offtarget tool
//#define IPDC_ON_OFFTARGET_TOOL

// Enable if validatiaon on decompressor in offtarget tool or Q6sim 
//#define IPDC_VALIDATE


/*==========================================================================

                       INCLUDE FILES FOR MODULE

===========================================================================*/

/*--------------------------------------------------------------------------
  These header files should be included only for UE or MOB
---------------------------------------------------------------------------*/
#if defined(IPDC_ON_TARGET) || defined(IPDC_ON_MOB) || defined(IPDC_ON_Q6SIM) || defined(IPDC_ON_OFFTARGET_TOOL)
/* Only comdef header file included here.
   Not including rest to avoid increasing all object file sizes.
   Let's individually include required files in each IPDC file */
#include "comdef.h"
#endif

#if defined(IPDC_ON_Q6SIM)||defined (IPDC_ON_OFFTARGET_TOOL)
#include "dsm.h"
#include "ps_in.h"
#endif

#if defined(IPDC_ON_Q6SIM)
#include "hexagon_sim_timer.h"
#endif

/*--------------------------------------------------------------------------
  These header files should be included only for TBS
---------------------------------------------------------------------------*/
#if IPDC_ON_TBS
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "netInit.h"
#include "dsm.h"
#endif

/*==========================================================================

                    PLATFORM SPECIFIC MACRO DEFINITIONS

===========================================================================*/
/*--------------------------------------------------------------------------
  Compile time defined value of the maximum number of IPDC instances that
  may be created on the UE 
---------------------------------------------------------------------------*/
#if defined(IPDC_ON_TARGET)
#define IPDC_MAX_INSTANCE_COUNT           2
#else
#define IPDC_MAX_INSTANCE_COUNT           15
#endif

#if defined(IPDC_ON_OFFTARGET_TOOL)
  /*--------------------------------------------------------------------------
    VS dose not implement snprintf, use sprinf instead
  ---------------------------------------------------------------------------*/
  #define snprintf(buf,buf_len,format,...) sprintf(buf,format,__VA_ARGS__)
#endif

#if defined(IPDC_ON_TBS) || defined(IPDC_ON_STANDALONE_TOOL) || defined(IPDC_ON_Q6SIM) || defined(IPDC_ON_OFFTARGET_TOOL)
/*--------------------------------------------------------------------------
  memscpy definition not available on TBS and Standalone tool
---------------------------------------------------------------------------*/
#define memscpy(a,b,c,d) memcpy(a,c,d)

/*--------------------------------------------------------------------------
  memsmove definition not available on TBS and Standalone tool
---------------------------------------------------------------------------*/
#define memsmove(a,b,c,d) memmove(a,c,d)
#endif

/*--------------------------------------------------------------------------
  Custom implementation for TBS memscpy
---------------------------------------------------------------------------*/
#if defined(IPDC_ON_TARGET) || defined(IPDC_ON_MOB)|| defined(IPDC_ON_Q6SIM) || defined(IPDC_ON_OFFTARGET_TOOL)

#define IPDC_MEMSCPY memscpy

#else  

#define IPDC_MEMSCPY ipdc_memscpy

#endif

/*--------------------------------------------------------------------------
  Q6 optimized comparison operations for on target only
---------------------------------------------------------------------------*/
#if defined(IPDC_ON_TARGET) || defined(IPDC_ON_MOB)

#define IPDC_MIN(x,y) Q6_R_min_RR(x, y)
#define IPDC_GT(x,y) Q6_p_cmp_gtu_RR(x,y)
#define IPDC_LT(x,y) Q6_p_cmp_ltu_RR(x,y)
#define IPDC_ADD(x,y) Q6_R_add_RR(x,y)
#define IPDC_ADD_ACC(x,y,z) Q6_R_addacc_RR(x,y,z)

#else

#define IPDC_MIN(x,y) ( x < y ? x : y )
#define IPDC_GT(x,y) (x > y)
#define IPDC_LT(x,y) (x < y)
#define IPDC_ADD(x,y) (x + y)
#define IPDC_ADD_ACC(x,y,z) (x + y + z)

#endif

/*--------------------------------------------------------------------------
  Different inline macro for off target 
---------------------------------------------------------------------------*/
#ifdef IPDC_ON_OFFTARGET_TOOL
#define IPDC_INLINE __inline
#define EXTERN extern
#define STATIC static
#else
#define IPDC_INLINE static inline
#endif

/*--------------------------------------------------------------------------
  Macro to enable customer decompress code
---------------------------------------------------------------------------*/
//#define IPDC_CUSTOMER_DECOMP

/*--------------------------------------------------------------------------
  Macro to enable different optimization item 
---------------------------------------------------------------------------*/
  #define IPDC_OPTMIZED_HASH
#if !defined(TEST_FRAMEWORK) && !defined(IPDC_ON_OFFTARGET_TOOL)
  #define Q6_OPTIMIZED_HKEY_COMPUTATION
  #define IPDC_FAST_CMP
  #define IPDC_FAST_HASH
  #define IPDC_FAST_SHRINK
#endif

#if !defined(IPDC_ON_OFFTARGET_TOOL)
#define IPDC_NO_SH_HASH 
#endif

/*===========================================================================
MACRO IPDCI_MEMALLOC

DESCRIPTION
  This macro is a wrapper to PS function which calls the fully re-entrant 
  rex_malloc().
DEPENDENCIES
  None

RETURN VALUE
  pointer to memory allocated
  NULL if failed to allocate memory

SIDE EFFECTS
  Allocates memory of the requested size
===========================================================================*/
#if defined(IPDC_ON_TARGET) || defined(IPDC_ON_MOB)

#define ipdci_memalloc ps_system_heap_mem_alloc

#else

#define ipdci_memalloc malloc

#endif

/*===========================================================================
FUNCTION IPDCI_MEMFREE

DESCRIPTION
  This macro is a wrapper to PS function which calls the fully re-entrant 
  rex_free().

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Frees the passed memory buffer
===========================================================================*/
#if defined(IPDC_ON_TARGET) || defined(IPDC_ON_MOB)

#define ipdci_memfree(mem_ptr) \
  if ((mem_ptr) != NULL) \
  { \
    PS_SYSTEM_HEAP_MEM_FREE(mem_ptr); \
  }
  
#else

#define ipdci_memfree(mem_ptr) \
  if ((mem_ptr) != NULL) \
  { \
    free(mem_ptr); \
  }

#endif

/*===========================================================================
MACROS: IPDCI_NTOHS, IPIPDCI_HTONS, IPDCI_HTONL

DESCRIPTION
  These macros are wrappers for the Host-to-Network and Network-to-Host
  conversion functions
DEPENDENCIES
  None

RETURN VALUE
  The network or host ordered value

SIDE EFFECTS
  None
===========================================================================*/
#if defined(IPDC_ON_TARGET) || defined(IPDC_ON_MOB)|| defined(IPDC_ON_Q6SIM)

#define ipdci_ntohs ps_ntohs
#define ipdci_ntohl ps_ntohl
#define ipdci_htons ps_htons
#define ipdci_htonl ps_htonl

#else

#define ipdci_ntohs NTOHS
#define ipdci_ntohl NTOHL
#define ipdci_htons HTONS
#define ipdci_htonl HTONL

#endif


/* Definitions needed for standalone tool */
#if defined (IPDC_ON_STANDALONE_TOOL)
typedef unsigned char byte;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long int uint32;
#endif

#ifdef __cplusplus
}
#endif

#endif /* PS_IPDC_PLATFORM_DEFS_H */
