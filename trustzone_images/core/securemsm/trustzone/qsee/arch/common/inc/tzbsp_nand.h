#ifndef SECCFG_NAND_H
#define SECCFG_NAND_H

/*===========================================================================
Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/arch/common/inc/tzbsp_nand.h#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
05/03/12   sm       Initial version
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <tzbsp_xpu.h>

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define UINT_MAX 0xFFFFFFFF
#define FLASH_PART_NAME_LENGTH 0x10
#define FLASH_NUM_PART_ENTRIES 0x20
#define NO_OF_RGS              0x8
#define NO_OF_RGS_ALLOC        0x4
#define SYSTEM                 "0:SYSTEM" 
#define LE                     0x1


/* Size of the NANDc codeword - hardware defined */
#define CODEWORD_SIZE          0x200 

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef struct fs_device_data *fs_device_t;

typedef struct nand_part_details
{
  uint32 start[NO_OF_RGS];
  uint32 end[NO_OF_RGS];
}nand_part_details_t;

typedef enum
{
  PART_RG0 = 0,                   /* SBL partition   */
  PART_RG1,                       /* MIBIB partition */
  PART_RG2,                       /* SDI to ACDB     */
  PART_RG3,                       /* QDSP to ADSP    */
  PART_RG4,                       /* APPSBL to SCRUB */
  PART_RG5,                       /* CACHE to end    */
  PART_RG7,                       /* EFS partition   */ 
  PART_NONE,
} nand_mpu_rg;

typedef struct
{
  const char *name;
  nand_mpu_rg group;
}nand_part_group;


typedef struct tzbsp_flash_partition_entry_s
{

  /* Name of the partition in the form of 0:ALL, 0:EFS2, etc. */
  char name[FLASH_PART_NAME_LENGTH];

  /* Offset in blocks from beginning of device */
  uint32 offset;

  /* length in blocks of the partition */
  uint32 length;

  /* Partition attributes */
  uint8 attrib1;
  uint8 attrib2;
  uint8 attrib3;

  /* Numeric ID of flash part (first = 0, second = 1) */
  uint8 which_flash;
}tzbsp_flash_partition_entry_t;

typedef struct tzbsp_flash_partition_table_s
{

  /* Partition table magic numbers and version number.
   *   WARNING!!!!
   *   No matter how you change the structure, do not change
   *   the placement of the first three elements so that future
   *   compatibility will always be guaranteed at least for
   *   the identifiers.
   */
  uint32 magic1;
  uint32 magic2;
  uint32 version;

  /* Partition table data.  This portion of the structure may be changed
   * as necessary to accommodate new features.  Be sure to increment
   * version number if you change it.
   */
  uint32 numparts;   /* number of partition entries */
  tzbsp_flash_partition_entry_t part_entry[FLASH_NUM_PART_ENTRIES];
}tzbsp_flash_partition_table_t;

typedef struct tzbsp_nand_parti_table_s
{
  uint32 parti_table_size_bytes;
  tzbsp_flash_partition_table_t *parti_table;
}tzbsp_nand_parti_table_t;


typedef struct tzbsp_nand_addr_s
{
   unsigned int   blockNum;   /**< Block number to access */
   unsigned int   pageNum;    /**< Page number to access */
   unsigned int   columnNum;  /**< Column (byte) number to access */
} tzbsp_nand_addr_t;

/* NAND info to share via SMEM */
 
typedef struct tzbsp_dev_smem_info_s
{
  uint32 version;
  uint32 total_blocks;
  uint32 pages_per_block;
  uint32 bytes_per_page;
  uint32 reserved1; 
  uint32 reserved2; 
  uint32 reserved3; 
  uint32 reserved4; 
}tzbsp_dev_smem_info_t;

typedef struct tzbsp_xpu_addr_s
{
   uint32 upper_addr;
   uint32 lower_addr;
}tzbsp_xpu_addr_t;

int32 tzbsp_check_for_nand_boot(void);

int tzbsp_nand_mpu_config(uint32 mode);

#endif /* TZBSP_NAND_H */
