#ifndef __FLASH_MIBBTB_H__
#define __FLASH_MIBBTB_H__
/*===========================================================================

                Bad Block Table Block Definitions

DESCRIPTION
  This header file gives the external definition of the Bad Block Table
  Block, located in the third non-bad block in NAND flash for Multi-Image
  Boot targets.
  
  The bad block table is allowed to extend as long as needed but must be
  contained within the block.

  Copyright (c) 2004-2009 Qualcomm Technologies Incorporated. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

    This section contains comments describing changes made to this file.
    Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/inc/flash_mibbtb.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/27/07   rt      Updated Copyright statement
12/06/06   dp      Update bad_block array size
03/15/05   drh     Remove unnecessary BBTB_BLOCK which is a duplicate with
                   a different name in miboot.h
11/02/04   drh     Add conditional include
08/14/04   drh     Created.

===========================================================================*/

#include "DALStdDef.h"

/*------------------------------------------------------------
 *
 *  Bad Block Table Definitions
 *
 * ---------------------------------------------------------*/
 
/* Magic numbers used in identifying valid bad block table */
#define BBTB_MAGIC1     0xAA01DE33
#define BBTB_MAGIC2     0xDBFE99AF

/* Must increment this version whenever structure of bad block table
 * changes.
 */
#define BBTB_VERSION   0x1

/*
 * These definitions related to the BBTB are arbitrary, and are not
 * limited by the bad_block_table definition itself.  The size
 * limitations have been added here to allow static alloaction of the
 * BBTB list in memory.
 */

#define MAX_BBTB_PAGES 8

/* size of all fields of the BBTB, minus the bad_block table itself) */
#define SIZEOF_BBTB_INFO (4 * sizeof(uint32))
#define BBTB_BUFFER_SIZE (512*MAX_BBTB_PAGES)

#define MAX_BAD_BLOCKS ((BBTB_BUFFER_SIZE - SIZEOF_BBTB_INFO)/4)


struct bad_block_table {

  /* Bad Block Table magic numbers and version number.
   *   WARNING!!!!
   *   No matter how you change the structure, do not change
   *   the placement of the first three elements so that future
   *   compatibility will always be guaranteed at least for
   *   the identifiers.
   */
  uint32 magic1;
  uint32 magic2;
  uint32 version;

  /* Bad Block Table data.  This portion of the structure may be changed
   * as necessary to accommodate new features.  Be sure to increment
   * version number if you change it.
   *
   * The array of bad block numbers may span more than the first page,
   * so code using the table must read a sufficient number of pages
   * in order to contain all the entries, based on the number of entries
   * indicated in the first page of the structure read.
   */
  uint32 num_entries;   /* Number of bad block entries */
  uint32 bad_block[MAX_BAD_BLOCKS];  /* Array of bad block numbers  */
};

 #endif /* __FLASH_MIBBTB_H__ */

