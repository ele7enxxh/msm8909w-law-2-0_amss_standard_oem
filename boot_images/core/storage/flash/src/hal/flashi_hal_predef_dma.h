#ifndef FLASH_HAL_PREDEF_DMA_H
#define FLASH_HAL_PREDEF_DMA_H

/*====================================================================
 *
 * FILE:        flash_hal_predef_dma.h
 *
 * SERVICES:    Common header file for predefined DMA support in flash HAL
 *
 * DESCRIPTION: The perdefined DMA support in flash HAL is to improve
 *              through pre-allocated DMA lists.
 *
 *
 * PUBLIC CLASSES AND STATIC FUNCTIONS:
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:
 *
 * Copyright (c) 2009-2010 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *==================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/src/hal/flashi_hal_predef_dma.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 02/05/10     mcg     Support to get DMOV configs from DAL
 * 03/12/09     bb      Initial Revision
 *===================================================================*/

#include "flash_hal.h"

/*------------------------ DMA Specific Macros ----------------------*/

/* IMPORTANT NOTE: The following macros and values SHOULD MATCH with
   the DMA hardware used. Please make sure that the bit definitions
   match with the DMOV HAL header files. Ideally these vaues should
   come from DMOV header files. But to avoid DMOV compile time dependency
   we are hardcoding these value here */

/* Data Mover command word definitions */
#define FLASH_DMA_CMD_WORD_LC_SHFT   31
#define FLASH_DMA_CMD_WORD_LC_BMSK   ((uint32)1 << FLASH_DMA_CMD_WORD_LC_SHFT)

#define FLASH_DMA_CMD_WORD_FR_SHFT   22
#define FLASH_DMA_CMD_WORD_FR_BMSK   (1 << FLASH_DMA_CMD_WORD_FR_SHFT)

#define FLASH_DMA_CMD_WORD_OCU_SHFT  21
#define FLASH_DMA_CMD_WORD_OCU_BMSK  (1 << FLASH_DMA_CMD_WORD_OCU_SHFT)

#define FLASH_DMA_CMD_WORD_OCB_SHFT  20
#define FLASH_DMA_CMD_WORD_OCB_BMSK  (1 << FLASH_DMA_CMD_WORD_OCB_SHFT)

#define FLASH_DMA_CMD_WORD_TCB_SHFT  19
#define FLASH_DMA_CMD_WORD_TCB_BMSK  (1 << FLASH_DMA_CMD_WORD_TCB_SHFT)

#define FLASH_DMA_CMD_WORD_DAH_SHFT  18
#define FLASH_DMA_CMD_WORD_DAH_BMSK  (1 << FLASH_DMA_CMD_WORD_DAH_SHFT)

#define FLASH_DMA_CMD_WORD_SAH_SHFT  17
#define FLASH_DMA_CMD_WORD_SAH_BMSK  (1 << FLASH_DMA_CMD_WORD_SAH_SHFT)

#define FLASH_DMA_CMD_WORD_DST_END_CTL_SHFT          14
#define FLASH_DMA_CMD_WORD_DST_END_CTL_BMSK          \
  (7 << FLASH_DMA_CMD_WORD_DST_END_CTL_SHFT)

#define FLASH_DMA_CMD_WORD_SRC_END_CTL_SHFT          11
#define FLASH_DMA_CMD_WORD_SRC_END_CTL_BMSK          \
  (7 << FLASH_DMA_CMD_WORD_SRC_END_CTL_SHFT)

#define FLASH_DMA_CMD_WORD_DST_CRCI_CTL_SHFT         7
#define FLASH_DMA_CMD_WORD_DST_CRCI_CTL_BMSK         \
  (0xf << FLASH_DMA_CMD_WORD_DST_CRCI_CTL_SHFT)

#define FLASH_DMA_CMD_WORD_SRC_CRCI_CTL_SHFT         3
#define FLASH_DMA_CMD_WORD_SRC_CRCI_CTL_BMSK         \
   (0xf << FLASH_DMA_CMD_WORD_SRC_CRCI_CTL_SHFT)

#define FLASH_DMA_CMD_WORD_ADDR_MODE_SHFT            0
#define FLASH_DMA_CMD_WORD_ADDR_MODE_BMSK            \
  (7 << FLASH_DMA_CMD_WORD_ADDR_MODE_SHFT)

/* Data Mover command pointer list entry definitions */
#define FLASH_DMA_CMD_PTR_LIST_LP_SHFT               31
#define FLASH_DMA_CMD_PTR_LIST_LP_BMSK               \
  ((uint32)1 << FLASH_DMA_CMD_PTR_LIST_LP_SHFT)

#define FLASH_DMA_CMD_PTR_LIST_PT_SHFT               29
#define FLASH_DMA_CMD_PTR_LIST_PT_BMSK               \
  ((uint32)3 << FLASH_DMA_CMD_PTR_LIST_PT_SHFT)

#define FLASH_DMA_CMD_PTR_LIST_ADDR_SHFT             0
#define FLASH_DMA_CMD_PTR_LIST_ADDR_BMSK             ((uint32)0x1FFFFFFF)


/*------------------ Flash Wrapper Macros ------------------------------*/

/* No Command options */
#define FLASH_PREDEF_DMA_CMD_NONE         0

/* Other channel block */
#define FLASH_PREDEF_DMA_CMD_OCB          FLASH_DMA_CMD_WORD_OCB_BMSK

/* Other channel unblock */
#define FLASH_PREDEF_DMA_CMD_OCU          FLASH_DMA_CMD_WORD_OCU_BMSK

/* Last command */
#define FLASH_PREDEF_DMA_CMD_LC           FLASH_DMA_CMD_WORD_LC_BMSK

/* Last command pointer */
#define FLASH_PREDEF_DMA_CP_LP            FLASH_DMA_CMD_PTR_LIST_LP_BMSK

/* Destination CRCI */
#define FLASH_PREDEF_DMA_CMD_DEST_CRCI(a)    \
  ((a) << FLASH_DMA_CMD_WORD_DST_CRCI_CTL_SHFT)

/* Source CRCI */
#define FLASH_PREDEF_DMA_CMD_SRC_CRCI(a)     \
  ((a) << FLASH_DMA_CMD_WORD_SRC_CRCI_CTL_SHFT)

/* Default DMA command length */
#define FLASH_HAL_CMD_LEN  4

/* Sets the adddress of a command list in command pointer list */
#define SET_CMD_PTR(a)      (((uint32)(a)) >> 3)

/* Sets the adddress of the last command list in command pointer list */
#define SET_LAST_CMD_PTR(a) ((((uint32)(a)) >> 3) | FLASH_PREDEF_DMA_CP_LP)

#endif /* FLASH_HAL_PREDEF_DMA_H */
