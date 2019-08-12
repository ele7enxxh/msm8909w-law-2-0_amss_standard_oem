#ifndef MBA_NAND_H
#define MBA_NAND_H

/*===========================================================================
Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/securemsm/mba/src/mba_nand/mba_nand.h#1 $
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $

when       who      what, where, why
--------   ---      ------------------------------------
03/25/13   dm       Initial version
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "flash_nand_partition.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define EFS2   "0:EFS2"
#define UINT_MAX 0xFFFFFFFF

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef struct nand_part_details
{
  uint32 efs_start;               /* EFS partition starting block offset     */
  uint32 efs_end;                 /* EFS partition ending block offset       */
  uint32 pages_per_block;         /* number of pages in a block              */
  uint32 page_size_bytes;         /* page size in bytes                      */
}nand_part_details_t;

/* 
typedef struct
{
  const char *name;
  nand_mpu_rg group;
}nand_part_group;
*/
typedef struct mba_nand_addr_s
{
   unsigned int   blockNum;   /**< Block number to access */
   unsigned int   pageNum;    /**< Page number to access */
   unsigned int   columnNum;  /**< Column (byte) number to access */
} mba_nand_addr_t;

typedef struct mba_xpu_addr_s
{
   uint32 upper_addr;
   uint32 lower_addr;
}mba_xpu_addr_t;
#endif /* MBA_NAND_H */
