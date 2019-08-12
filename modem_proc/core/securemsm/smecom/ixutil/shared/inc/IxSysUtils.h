#ifndef __IXSYSUTILS_H__
#define __IXSYSUTILS_H__

/*===========================================================================

                       I X   S Y S   U T I L S

                        H E A D E R  F I L E

FILE:  IxSysUtils.h

GENERAL DESCRIPTION

  This header file defines the interface to IxSysUtils.

  This interface has methods for interacting with the system software (kernel).

 PUBLIC CLASSES AND STATIC FUNCTIONS:

 INITIALIZATION & SEQUENCING REQUIREMENTS:


  Copyright (c) 2008 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/smecom/ixutil/shared/inc/IxSysUtils.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

===========================================================================*/


/*===========================================================================

               INCLUDE FILES

============================================================================*/

#include "IxErrno.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "rex.h"

/*-------------------------------------------------------------------------
                              DATA TYPES
--------------------------------------------------------------------------*/

/* Type of priority */
typedef enum
{
 IXSYSUTILS_PRI_MIN,      // The lowest process priority
 IXSYSUTILS_PRI_MAX,      // The highest process priority
 IXSYSUTILS_PRI_CUSTOM    // Custom process priority, value to be specified
} IxSysUtilsPriEType;

typedef rex_priority_type IxSysUtilsProcessPriType;

/*------------------------------------------------------------------------
                              FUNCTIONS
------------------------------------------------------------------------*/
/*========================================================================
FUNCTION:  IxSysUtils_GetPri()

DESCRIPTION:
    Queries the kernel to retrieve the priority of the currently 
    executing process/task.

INPUT/OUTPUT PARAMETERS:
    @param  IxSysUtilsProcessPriType* pPri - pointer to a pre allocated
            structure of type IxSysUtilsProcessPriType. This function
            will set the value of pPri. The memory ownership is retained
            by the caller. [output]

RETURN VALUE:
    Error Code. E_SUCCESS is returned if there is no error.

SIDE EFFECTS:
=========================================================================*/
IxErrnoType IxSysUtils_GetPri ( IxSysUtilsProcessPriType* pPri );


/*========================================================================
FUNCTION:  IxSysUtils_GetPri()

DESCRIPTION:
    Sets the priority of the currently executing process/task as specified
    in the input params.

INPUT/OUTPUT PARAMETERS:
    @param  IxSysUtilsPriEType ePri - enum type indicating the
            priority type. [input]
    @param  IxSysUtilsProcessPriType value - value to set the process/task
            priority. [input]

RETURN VALUE:
    Error Code. E_SUCCESS is returned if there is no error.

SIDE EFFECTS:
=========================================================================*/
IxErrnoType IxSysUtils_SetPri ( IxSysUtilsPriEType ePri, 
                                IxSysUtilsProcessPriType value );

#endif /* __IXOBJECTREGISTRY_H__ */

#ifdef __cplusplus
}
#endif
