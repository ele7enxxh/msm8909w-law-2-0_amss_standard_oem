#ifndef FLASH_NOR_PARTITION_H
#define FLASH_NOR_PARTITION_H

/*=============================================================================
 *
 * FILE:      flash_nor_partition.h
 * 
 * DESCRIPTION: Function and data structure declarations specific to the NOR
 *            partition layer
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2010 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *===========================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/src/dal/flash_nor_partition.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 09/10/10     eo      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flash_dal.h"
#include "flash_nor_partition_tbl.h"

/*===================================================================
 *
 *                Partition Table Definitions
 *
 ====================================================================*/

/* Number of partition entries that can be support */
#define FLASH_NOR_PARTI_NUM_ENTRIES    4

/* Macro definition related to partition attribute flags */
#define FLASH_NOR_PARTI_ATTR_BASE      0xFF

/* Attribute Masks */
#define FLASH_NOR_PARTI_ATTRIB1_BMSK   0x000000FF
#define FLASH_NOR_PARTI_ATTRIB2_BMSK   0x0000FF00

/* Attribute Shift Offsets */
#define FLASH_NOR_PARTI_ATTRIB1_SHFT   0
#define FLASH_NOR_PARTI_ATTRIB2_SHFT   8

#define FLASH_NOR_PARTI_ATTRIB( val, attr ) \
  (((val) & (attr##_BMSK)) >> attr##_SHFT)

/* NOR partition primary attributes */
enum flash_nor_partition_attrib
{
  /* No specific attribute defintion. Fall back to default operations
     within the NOR driver. */
  FLASH_NOR_PARTI_DEFAULT_ATTRIB = 0xFF,

  /* This corresponds to read only partitions. */
  FLASH_NOR_PARTI_READ_ONLY = 0,

  /* This corresponds to read/write based operations that
     sequentially operate on the pages per block.
  */
  FLASH_NOR_PARTITION_READWRITE  
};

/* NOR partition secondary attributes */
enum flash_nor_partition_attrib2
{
  /* No specific attribute defintion. Fall back to default operations
     within the NOR driver. */
  FLASH_NOR_PARTI_DEFAULT_ATTRIB2 = 0xFF
};

/* NOR Partition table entry definition */
struct flash_nor_partition_entry;
typedef struct flash_nor_partition_entry *flash_nor_partentry_t;

struct flash_nor_partition_entry 
{
  /* Name of the partition in the form of 0:ALL, 0:EFS2, etc. */
  char name[FLASH_NOR_PARTI_NAME_LENGTH];

  /* Offset in blocks from beginning of device */
  uint32 offset;

  /* length in blocks of the partition */
  uint32 length;

  /* Partition attributes */
  uint8 attrib1;
  uint8 attrib2;
};

/* NOR User partition table entry definition */
struct flash_nor_usr_partition_entry;
typedef struct flash_nor_usr_partition_entry *flash_nor_usr_partentry_t;

struct flash_nor_usr_partition_entry 
{
  /* Name of the partition in the form of 0:ALL, 0:EFS2, etc. */
  char name[FLASH_NOR_PARTI_NAME_LENGTH];

  /* Size in KB for the partition */
  uint32 img_size;

  /* Attributes for this partition - This get copied to the attribx flags in
     system partition table */
  uint8 reserved_flag1;
  uint8 reserved_flag2;
};

/* NOR System partition table definition */
struct flash_nor_partition_table;
typedef struct flash_nor_partition_table *flash_nor_partable_t;

struct flash_nor_partition_table 
{
  /* Partition table data.  This portion of the structure may be changed
   * as necessary to accommodate new features.
   */
  uint32 numparts;   /* number of partition entries */
  struct flash_nor_partition_entry part_entry[FLASH_NOR_PARTI_NUM_ENTRIES];
};


/* NOR User partition table definition */
struct flash_nor_usr_partition_table;
typedef struct flash_nor_usr_partition_table *flash_nor_usr_partable_t;


struct flash_nor_usr_partition_table 
{
  /* Partition table data.  This portion of the structure may be changed
   * as necessary to accommodate new features.
   */
  uint32 numparts;   /* number of partition entries */
  struct flash_nor_usr_partition_entry part_entry[FLASH_NOR_PARTI_NUM_ENTRIES];
};

/* Table of NOR partitions */
extern struct flash_nor_usr_partition_table nor_ram_table;

/*
 * Opens a partition found by a name string passed in.  Modifies the private
 * data of the device structure to limit the "view" of the device by the
 * device driver for all subsequent call to device functions.
 */
int nor_partition_update_limits(flash_client_ctxt *client_ctxt, const unsigned char 
  *partition_name);

#endif /* FLASH_NOR_PARTITION_H */
