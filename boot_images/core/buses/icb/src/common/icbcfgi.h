#ifndef ICBCFGI_H
#define ICBCFGI_H
/*
===========================================================================

FILE:         icbcfgi.h

DESCRIPTION:  Internal header file for the ICB boot library.

===========================================================================

                             Edit History

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/buses/icb/src/common/icbcfgi.h#1 $

when         who     what, where, why
----------   ---     ------------------------------------------------------
2013/02/26   sds     Initial revision.

===========================================================================
             Copyright (c) 2013 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
===========================================================================
*/

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include "comdef.h"

/* -----------------------------------------------------------------------
** Definitions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */
typedef struct
{
  uint64 start_addr;
  uint64 end_addr;
} icbcfg_mem_region_type;

#endif /* ICBCFGI_H */
