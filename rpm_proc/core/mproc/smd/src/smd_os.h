
/*---------------------------------------------------------------------------
   SMD_OS.H  - Shared Memory Driver OS Header File
---------------------------------------------------------------------------*/
/**
  @file smd_os.h

*/
/* This file contains declarations for the Shared Memory Driver OS abstraction 
   files, using DALSYS system calls. */

/* NOTE: For the output PDF generated using Doxygen and Latex, all file and group 
         descriptions are maintained in the SMD_mainpage file. To change any of the 
         the file/group text for the PDF, edit the SMD_mainpage file, or contact 
         Tech Pubs.

         The above description for this file is part of the "smd" group description 
         in the SMD_mainpage file. 
*/

/*--------------------------------------------------------------------------
Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
---------------------------------------------------------------------------*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/rpm.bf/2.1.1/core/mproc/smd/src/smd_os.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/26/13   bt      Remove OS string copy macro.
08/17/12   bt      Move SMEM memory access macros to smd_proc.h.
08/13/12   bt      Make memory access volatile to prevent optimizing to smaller 
                   accesses - RPM requires word-access, cannot byte-access.
04/05/12   bt      Moved interrupt defs, headers to smd_proc.h.
03/30/12   pa      Removed inclusion of tramp.h
02/02/12   bt      Cleaned up header inclusions.
02/01/12   pa      Removing inclusion of deprecated customer.h
05/30/12   bm      Use ARM(cortex) specific implementation of mem barrier for 
                   RPM
01/20/12   bm      Add support for RPM processor.
08/31/11   bt      Remove redefinition of ERR_FATAL, now provided by Err.
07/11/11   bm      + Moved NULL interrupt definitions to this file
                   + Typedefined smd_os_proc_type and smd_os_intr_type.
04/29/11   bm      Removed unused SMD macros
12/08/10   rs      + Added generic memory barrier
                   + Fixed ASSERT macro for DSPS
11/26/10   rs      Cleaned up unused header files and macros
10/21/10   rs      Initial revision (derived from src/os/rex/smd_os.h).
===========================================================================*/


/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#ifndef SMD_OS_H
#define SMD_OS_H

#include "comdef.h"
#include "string.h"
#include "DALSys.h"

#if !defined(IMAGE_DSPS_PROC) && !defined(IMAGE_RPM_PROC)
#include "DALFramework.h"
#include "assert.h"
#else
#include "smem_v.h"
#endif

#ifdef IMAGE_RPM_PROC
#include "cortex-m3.h"
#endif

#include "err.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                        MACRO DEFINITIONS

===========================================================================*/
/* -------------------------------------------------------------
   Memory Barrier 
------------------------------------------------------------- */
#if defined(IMAGE_DSPS_PROC)
/** Defines the generic memory barrier: write and read to
    unchached/unbuffered memory location. it is defined here
    to prevent re-definition of the macro since memory
    barrier is generally a function provided by the OS. */
#define SMD_MEMORY_BARRIER() smem_generic_memory_barrier()
#else
#define SMD_MEMORY_BARRIER() memory_barrier()
#endif

#if defined(IMAGE_DSPS_PROC) || defined(IMAGE_RPM_PROC)
#ifdef ASSERT
#undef ASSERT
#endif
#define ASSERT( cond ) \
      if( !( cond ) ) \
      {                 \
        while(1);         \
      }
#endif

#ifdef IMAGE_RPM_PROC
#define ERR(...)    { while(0);}
#endif

#ifndef NOTUSED
/* Eliminates compiler warnings for error checking variables stubbed out in
 * ASSERT statements on certain processors.
 */
#define NOTUSED(i) if(i){}
#endif


/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/

/** SMD Lite DAL specific OS context structure */
typedef struct smd_os_context_struct
{
  /** Critical section to protect SMD Lite open port list */
  DALSYSSyncHandle open_list_cs;
} smd_os_context_type;

/*===========================================================================

                        PUBLIC VARIABLE DECLARATIONS

===========================================================================*/

/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
#ifdef __cplusplus
}
#endif
#endif /* SMD_OS_H */
