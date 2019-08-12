#ifndef ICBCFGI_H
#define ICBCFGI_H
/*
===========================================================================

FILE:         icbcfgi.h

DESCRIPTION:  Internal header file for the ICB boot library.

===========================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/buses/icb/src/common/icbcfgi.h#1 $

when         who     what, where, why
----------   ---     ------------------------------------------------------
2014/04/07   vg      Added icbcfgi.h file to support  64Gb x 4POR DDR in modem
2013/02/26   sds     Initial revision.

===========================================================================
             Copyright (c) 2013-2014 Qualcomm Technologies Incorporated.
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
