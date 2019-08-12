#ifndef FLASH_NAND_PARTITION_H
#define FLASH_NAND_PARTITION_H

/*=============================================================================
 *
 * FILE:      flash_nand_partition.h
 * 
 * DESCRIPTION: Function and data structure declarations specific to the NAND
 *            partition layer
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2008-2010 Qualcomm Technologies Incorporated.
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
 *  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/src/dal/flash_nand_partition.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 07/25/11     eo      Add nand_partition_table for SMEM AARM init
 * 02/08/10     bb      Cleanup
 * 10/05/09     mh      Rename nand_partition_open
 * 10/01/09     eo      Add DM support to MDM9k target
 * 08/28/08     dp      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flash_dal.h"

/* System partition table */
struct nand_parti_table
{
  uint32 parti_table_size_bytes;
  uint32 *parti_table;
};

/* Table of MIBIB partitions */
extern struct nand_parti_table nand_partition_table;

/*
 * Reads the MIBIB from the SMEM 
 */
int smem_retrieve_mibib(flash_client_ctxt *client_ctxt);

/*
 * Reads the MIBIB from the flash 
 */
int nand_retrieve_mibib(flash_client_ctxt *client_ctxt);

/*
 * Opens a partition found by a name string passed in.  Modifies the private
 * data of the device structure to limit the "view" of the device by the
 * device driver for all subsequent call to device functions.
 */
int nand_partition_update_limits(flash_client_ctxt *client_ctxt, const unsigned char 
  *partition_name);

#endif /* FLASH_NAND_PARTITION_H */
