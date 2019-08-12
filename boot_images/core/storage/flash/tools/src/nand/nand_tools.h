#ifndef __NAND_TOOLS_H__
#define __NAND_TOOLS_H__

/*=======================================================================
 * FILE:      nand_tools.h
 *
 * SERVICES:  Flash Programming Plug-In for Lauterbach FLASH_PROG/ICD TRACE32
 *
 * GENERAL DESCRIPTION
 *    This files has the defines the different nand tools.
 *
 *        Copyright © 2008-2009 Qualcomm Technologies Incorporated.
 *               All Rights Reserved.
 *            QUALCOMM Proprietary/GTDR
 *=======================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/nand/nand_tools.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 03/19/07     mm      Initial version
 *==================================================================*/

/* Macros for the Flash.bin programing on Windows mobile builds*/

/* Size of the flash.bin Image header */
/* 7 Sync + 4 Addr + 4 Length */
#define FLASH_WM_FLASH_BIN_IMAGE_HDR_SIZE          15

/* Size of flash.bin Record/Block header */
/*  4 Record Addr + 4 Record Length + 4 Checksum */
#define FLASH_WM_FLASH_BIN_RECORD_HDR_SIZE         12
#define FLASH_WM_FLASH_BIN_RECORD_ADDR_SIZE        4
#define FLASH_WM_FLASH_BIN_RECORD_LEN_SIZE         4

/* Spare byte used by WM per code word*/
#define FLASH_WM_SPARE_BYTES_PER_CW_8_BIT          5
#define FLASH_WM_SPARE_BYTES_PER_CW_16_BIT         4

#define FLASH_WM_SPARE_BYTES_PER_2K_PAGE           8
#define FLASH_WM_SPARE_BYTES_PER_512_PAGE          4

/* During verification of the write in WM, The max udata
 * that will be read out is 8 bytes considering
 * an 8bit 2K device
 */
#define FLASH_WM_MAX_SPARE_BYTES_READ_OUT          8

/* Block size for the windows mobile 2K page */
#define FLASH_WM_LENGTH_ENITRE_BLK_2K              0x20200

/* Block size for the windows mobile 512 page */
#define FLASH_WM_LENGTH_ENITRE_BLK_512             0x4080

/* Max number of partition in flash.bin*/
#define FLASH_MAX_PARTITION_FLASH_BIN              24

#define NAND_MIBIB_MBN_PAGE_WRITTEN_FLAG          0x00
#define NAND_MIBIB_MBN_DECODE_KEY_VERSION         0x01
#define NAND_MIBIB_MBN_DECODE_KEY_BEGIN           0xFE
#define NAND_MIBIB_MBN_DECODE_KEY_END             0xFD

/* Factory Image Programming related constants     */
#define FLASH_FACTORY_FILE_HDR_SIZE                36
#define FLASH_FACTORY_PARTITION_RECORD_SIZE        16
#define FLASH_FACTORY_IMAGE_PARTI_NAME             4
#define FLASH_FACTORY_IMAGE_SIG_LENGTH             24

/* Factory version support is 0x201 (2.1) */
#define FLASH_FACTORY_IMAGE_VERSION                0x201
#define FLASH_FACTORY_IMAGE_PARAM_LEN              100
#define FLASH_FACTORY_IMAGE_8BIT_WIDTH             8
#define FLASH_FACTORY_IMAGE_16BIT_WIDTH            16

#endif  /* __NAND_IMAGE_HEADERS_H__ */
