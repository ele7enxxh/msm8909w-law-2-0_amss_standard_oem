#ifndef MBA_BIMC_H
#define MBA_BIMC_H

/**
@file mba_rmb.h
@brief Relay Message Buffer
*/

/*===========================================================================
   Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/mba/inc/mba_bimc.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when       who      what, where, why
--------   ---      ------------------------------------
01/11/12   mm       Made Compatible to Virtio 1.1.1 flat file
12/20/11   mm       Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "comdef.h"
typedef struct mba_ddr_region_s
{
  /* Start address of the DDR region */
  uint32 start;
  /* Size of the memory region, in bytes */
  uint32 size;
  /* TRUE if this region is interleaved, FALSE if non-interleaved. */
  boolean interleaved;
} mba_ddr_region_t;

/* EBI channel interleaving related constants. TODO Not the correct place? */
#define MBA_EBI_CH_NONE             0x0
#define MBA_EBI_CH_0                0x1
#define MBA_EBI_CH_1                0x2
#define MBA_EBI_CH_BOTH             0x3 /* Interleaved. */

uint8 mba_ebi_channel_for_addr(uint32 start, uint32 end);
int mba_read_ddr_size(void);

#endif