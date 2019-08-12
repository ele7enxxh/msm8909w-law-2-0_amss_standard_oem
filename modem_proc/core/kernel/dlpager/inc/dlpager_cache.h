#ifndef __DLPAGER_CACHE_H__
#define __DLPAGER_CACHE_H__

/*===========================================================================
 * FILE:         dlpager_cache.h
 *
 * SERVICES:     DL PAGER
 *
 * DESCRIPTION:  cache routines
 *
 * Copyright (c) 2015 Qualcomm Technologies Incorporated.
 * All Rights Reserved. QUALCOMM Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/kernel/dlpager/inc/dlpager_cache.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/01/15   ao      Initial revision
===========================================================================*/

void dlpager_dczero(unsigned addr, unsigned size);
void dlpager_icache_clean_128(unsigned addr, unsigned size_128);
#endif
