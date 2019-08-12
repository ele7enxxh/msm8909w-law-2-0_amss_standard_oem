#ifndef FLASHI_HAL_NAND_H
#define FLASHI_HAL_NAND_H

/*=======================================================================
 * FILE:        flashi_hal_nand.h
 *
 * SERVICES:    Common header file shared between all NAND devices
 *
 * DESCRIPTION: Common header file shared between all NAND devices
 *
 * Copyright (c) 2008-2011, 2015 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *=======================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash/src/hal/flashi_hal_nand.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 09/14/15     sb      Define max num-of-zeros allowed for an erased page
 * 05/20/11     bb      Flash hal optimization
 * 02/15/11     sb      Adding BAD_BLOCK_BYTE_NUM for 8bit ECC Raw mode
 * 01/13/11     sb      Adding 8bit ECC support
 * 04/15/10     jz      Added define FLASH_NAND_BLOCK_SIZE_64
 * 07/27/09     jz      Moved flash_hal_nor_if_ functions to flashi_hal_nor.h
 * 03/30/08     BB      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flashi_hal.h"

/*-----------------------------------------------------------------------
 *           Definitions and Constants
 *----------------------------------------------------------------------*/

#define FLASH_NAND_PAGE_SIZE_512         512
#define FLASH_NAND_PAGE_SIZE_2048        2048
#define FLASH_NAND_GOOD_BLOCK_FLAG_16    0xFFFF
#define FLASH_NAND_GOOD_BLOCK_FLAG_8     0xFF

#define FLASH_NAND_UD_PER_CW             512  /* User Data per CW */
#define FLASH_NAND_SA_PER_CW             16   /* Spare Area per CW for 4bit BCH ECC*/
#define FLASH_8BIT_ECC_NAND_SA_PER_CW    20   /* Spare Area per CW for 8bit BCH ECC */

#define FLASH_NAND_BYTES_PER_CW           (FLASH_NAND_UD_PER_CW +\
                                           FLASH_NAND_SA_PER_CW)
#define FLASH_8BIT_ECC_NAND_BYTES_PER_CW  (FLASH_NAND_UD_PER_CW +\
                                           FLASH_8BIT_ECC_NAND_SA_PER_CW)

#define FLASH_NAND_BLOCK_SIZE_64         64     /* 64 pages per block */
#define FLASH_NAND_BLOCK_SIZE_128        128    /* 128 pages per block */

/* These defines are pretty much generic to all the devices */
#define NAND_ECC_SIZE                    10     /* ECC parity data size for 4bit RS ECC */
#define NAND_SPARE_SIZE                  6      /* Entire spare area size */

#define NAND_4BIT_ECC_SIZE_8             7      /* ECC parity data size for 4bit BCH ECC x8 NAND */
#define NAND_4BIT_ECC_SIZE_16            8      /* ECC parity data size for 4bit BCH ECC x16 NAND */
#define NAND_8BIT_ECC_SIZE_8             13     /* ECC parity data size for 8bit BCH ECC x8 NAND */
#define NAND_8BIT_ECC_SIZE_16            14     /* ECC parity data size for 8bit BCH ECC x16 NAND */

#define NAND_4BIT_ECC_SPARE_BYTES_8      9      /* Entire spare area size for x8 NAND */
#define NAND_4BIT_ECC_SPARE_BYTES_16     8      /* Entire spare area size for x16 NAND*/
#define NAND_8BIT_ECC_SPARE_BYTES_8      7      /* Entire spare area size for x8 NAND */
#define NAND_8BIT_ECC_SPARE_BYTES_16     6      /* Entire spare area size for x16 NAND*/

#define NAND_UD_BYTE_SIZE                0x200  /* User data size */
#define NAND_CW_PER_PAGE_512             0      /* 1 codeword/page */
#define NAND_CW_PER_PAGE_2048            3      /* 4 codewords/page */
#define NAND_CW_PER_PAGE_4096            7      /* 8 codewords/page */
#define NAND_CW_FOR_BLK_OPRN             0      /* CW count for blk oprn */

/* NAND bad block byte number */
#define NAND_BAD_BLOCK_BYTE_NUM          (NAND_4BIT_ECC_SIZE_8 + NAND_4BIT_ECC_SPARE_BYTES_8 + 1)
#define NAND_8BIT_ECC_BAD_BLOCK_BYTE_NUM (NAND_8BIT_ECC_SIZE_8 + NAND_8BIT_ECC_SPARE_BYTES_8 + 1)

/* Number of bad-block bytes per codeword (CW) */
#define NAND_BAD_BLK_BYTE_CNT_8BIT       1      /* 1 byte/CW */
#define NAND_BAD_BLK_BYTE_CNT_16BIT      2      /* 2 bytes (or 1 word)/CW */
#define NAND_SPARE_SIZE_8BIT             (NAND_SPARE_SIZE - \
                                          NAND_BAD_BLK_BYTE_CNT_8BIT)
#define NAND_SPARE_SIZE_16BIT            (NAND_SPARE_SIZE - \
                                          NAND_BAD_BLK_BYTE_CNT_16BIT)

#define NAND_4BIT_ECC_SPARE_SIZE_8       (FLASH_NAND_SA_PER_CW - \
                                          (NAND_4BIT_ECC_SIZE_8 + NAND_BAD_BLK_BYTE_CNT_8BIT))
#define NAND_4BIT_ECC_SPARE_SIZE_16      (FLASH_NAND_SA_PER_CW - \
                                          (NAND_4BIT_ECC_SIZE_16 + NAND_BAD_BLK_BYTE_CNT_16BIT))

#define NAND_8BIT_ECC_SPARE_SIZE_8       (FLASH_8BIT_ECC_NAND_SA_PER_CW - \
                                          (NAND_8BIT_ECC_SIZE_8 + NAND_BAD_BLK_BYTE_CNT_8BIT))
#define NAND_8BIT_ECC_SPARE_SIZE_16      (FLASH_8BIT_ECC_NAND_SA_PER_CW - \
                                          (NAND_8BIT_ECC_SIZE_16 + NAND_BAD_BLK_BYTE_CNT_16BIT))

#define NAND_4BIT_ECC_N_MAX_ZEROS        4      /* Number of zeros allowed in erased page for 4bit ECC */
#define NAND_8BIT_ECC_N_MAX_ZEROS        8      /* Number of zeros allowed in erased page for 8bit ECC */

/* This buffer is used to pad the NAND controller buffer, if the
   main data and spare data are not enough to fill 528 bytes */
extern uint32 nand_write_buff;

#endif /* FLASHI_HAL_NAND_H */
