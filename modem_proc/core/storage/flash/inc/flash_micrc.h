#ifndef __FLASH_MICRC_H__
#define __FLASH_MICRC_H__
/*===========================================================================

                Multi-Image Boot CRC structure Definitions

DESCRIPTION
  This header file gives the structure definition of CRC checksum,
  located in last page of the fourth and fifth non-bad blocks in NAND 
  flash for Multi-Image Boot targets.
  
  There are two copies of the data in these blocks which allow us to 
  alternate between them and always have a good copy which points to 
  valid boot loader, even if a programming operation is accidentally
  interrupted

Copyright(c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

    This section contains comments describing changes made to this file.
    Notice that changes are listed in reverse chronological order.

   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/28/05   rt      Created.

===========================================================================*/

#include "comdef.h"

/*------------------------------------------------------------
 *
 *  CRC Definitions
 *
 * ---------------------------------------------------------*/
 

/* Magic numbers used in identifying valid mibib checksum */
/* The magic numbers are really temporary for now; Will be changed later */
#define FLASH_MIBIB_CRC_MAGIC1     0x9D41BEA1
#define FLASH_MIBIB_CRC_MAGIC2     0xF1DED2EA

/* Must increment this version whenever structure of partition table
 * changes.
 */
#define FLASH_MIBIB_CRC_VERSION   0x1


/* Partition table definition */
struct flash_mibib_crc_data;
typedef struct flash_mibib_crc_data *flash_micrc_data_t;

struct flash_mibib_crc_data {

  /* Checksum Data magic numbers and version number.
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
  uint32 crc;   /* CRC32 checksum on MIBIB data */
};

#endif /* __FLASH_MICRC_H__ */

