#ifndef __FLASH_PARAM_H__
#define __FLASH_PARAM_H__

/*=======================================================================
 * FILE:        nand_param.h
 *
 * SERVICES:    Flash Programming Plug-In for Lauterbach FLASH_PROG/ICD TRACE32
 *
 * GENERAL DESCRIPTION
 *
 *   This header file contains the structures that are used to communicate
 *   between our plug-in and Lauterbach TRACE32 Debugger. Also contains
 *   definitions for commands and responses that the FLASH_PROG ICD understands.
 *
 *       Copyright (c) 2008-2009 Qualcomm Technologies Incorporated.
 *	   All Rights Reserved.
 *	   Qualcomm Confidential and Proprietary
 *=======================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/nand/nand_param.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 04/15/10     jz      Added 2X support
 * 01/08/10     op      Support WM7 boot config read/write
 * 11/23/09     wt      Add support for ROFS partition Dump
 * 11/03/09     wt      Add support for XSR USER DATA partition Dump
 * 03/30/09     mh      Add support for 4K page
 * 02/27/09     jz      Replace prefix FLASH_ with FLASH_PROG_ for tools
 * 10/29/08     mm      Initial version
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "DALStdDef.h"

#define FLASH_PROG_BUFFER_SIZE 0x40000


/*===========================================================================

                     STRUCTURE AND TYPE DEFINITIONS FOR MODULE

===========================================================================*/
/* This structure is used to communcate between TRACE32 Debugger and
 * our FLASH plug-in. */
struct flash_prog_param {
   uint32 flashaddr;
   uint32 flashwidth;
   uint32 width;
   uint32 offset;
   uint32 addr;            // in: address of the flash to write/erase
   uint32 size;            // in: size of data to write/erase
   uint32 reserved;
   uint32 status;          // in: 1=program, 2=erase, 5=erase,
                           // out: 0=o.k., 0x100=fail
#if (defined BUILD_NAND && !defined BUILD_BOOT_CHAIN)
   byte   data[FLASH_PROG_BUFFER_SIZE];   // give 128K worth of buffer
                           // this is where TRACE32 will store data that
                           // we'll be writing to the FLASH
#else
   byte   *data;           // pointer to data
#endif
};


/* This structure is used to communcate from our program to T32
 * in an extended fashion for post processing statistics.  It
 * overlays the data area of prog_param and presents
 * a different interface */
struct type_flash_prog_report {
   uint32 input_gap_size;
   uint32 bad_block_gap_size;
   uint32 image_size;
   uint32 reserved1;
   uint32 reserved2;
   uint32 reserved3;
   uint32 reserved4;
   uint32 status;           // out: 0=o.k., 0x100=fail
};


/*===========================================================================

                     MACRO DEFINITIONS FOR MODULE

===========================================================================*/

#define SET_ERR(x) prog_param.reserved=((uint32)(x))

/* command code from type_prog_param.status */
#define FLASH_PROG_PROGRAM                 0x00000001
#define FLASH_PROG_ERASE                   0x00000002

#define FLASH_PROG_USR_PARTI_TBL           0x00000020
#define FLASH_PROG_SECURITY_MODE           0x00000021
#define FLASH_PROG_INITIALIZE              0x00000022
#define FLASH_PROG_FINALIZE                0x00000023

#define FLASH_PROG_VALID_BOOT_BLOCK        0x00000030
#define FLASH_PROG_REPORT_BAD              0x00000031
#define FLASH_PROG_MARK_BLOCK_BAD          0x00000032
#define FLASH_PROG_MARK_BLOCK_GOOD         0x00000033
#define FLASH_PROG_ERASE_BLOCK             0x00000034

/* Fill page with value */
#define FLASH_PROG_FILL_PAGE               0x00000035
#define FLASH_PROG_READ_PAGE               0x00000036
#define FLASH_PROG_READ_OTP_PAGE           0x00000037
#define FLASH_PROG_READ_UUID               0x00000038
#define FLASH_PROG_SET_ECC_STATE           0x00000039

#define FLASH_PROG_SELFTEST                0x00000040
#define FLASH_PROG_REPORT                  0x00000041
#define FLASH_PROG_VERIFY                  0x00000042
#define FLASH_PROG_ERASE_CHIP         0x00000043
#define FLASH_PROG_RETRIEVE_ID             0x00000044

#define FLASH_PROG_STRESS_TEST             0x00000046

#define FLASH_PROG_GET_FLASH_PARAM         0x00000049

/* 2X mode */
#define FLASH_PROG_FILL_2X_PAGE            0x0000004A
#define FLASH_PROG_READ_2X_PAGE            0x0000004B

/* Dump Utitlity Params */
#define FLASH_PROG_DUMP_INIT               0x00000050

/* Dump any partition */
#define FLASH_PROG_PARTI_DUMP              0x00000051

/* Dump MIBIB partition */
#define FLASH_PROG_DUMP_MIBIB_PARTI_BLOCK  0x00000054

/* Multi-Image only */
#define FLASH_PROG_PRINT_MIBIB             0x00000055
#define FLASH_PROG_PRINT_LAYOUT            0x00000056


/* XSR - user data  */
#define FLASH_PROG_DUMP_INIT_XSR_USER_DATA     0x00000057
#define FLASH_PROG_PARTI_DUMP_XSR_USER_DATA    0x00000058

/* XSR - ROFS */
#define FLASH_PROG_DUMP_INIT_XSR_ROFS     0x00000059
#define FLASH_PROG_PARTI_DUMP_XSR_ROFS    0x00000060

/* Display ROFS partitions*/
#define FLASH_DISP_XSR_ROFS_PARTITIONS    0x00000061

/* BLDR UTILS */
#define FLASH_PROG_READ_BCONFIG			  0x00000062
#define FLASH_PROG_WRITE_BCONFIG		  0x00000063

/* Flags to indicate partition table override */
#define FLASH_PROG_PARTI_NO_OVERRRIDE      0x0
#define FLASH_PROG_PARTI_OVERRRIDE         0x1

#if 0
/* Added for HAL API Testing */
#define FLASH_READ_SPARE              0x000000A0
#define FLASH_READ_MAIN_AND_SPARE     0x000000A1
#define FLASH_READ_ID                 0x000000A2
#define FLASH_READ_CUSTOM             0x000000A3
#define FLASH_READ_SPARE_UDATA        0x000000A4
#define FLASH_IS_PAGE_ERASED          0x000000A5
#define FLASH_IS_BLOCK_ERASED         0x000000A6
#define FLASH_BAD_BLOCK_CHECK         0x000000A7
#define FLASH_READ_MPAGES             0x000000A8
#define FLASH_COPY_PAGE               0x000000A9
#define FLASH_PROG_FILL_PAGE_WITH_SPARE    0x000000AA
#endif

/*
 *-------------------------------------------------------
 *  VALUES FROM 0x10000000 - 0xFFFFFFFF
 *  ARE RESERVED EXCLUSIVELY FOR HAL
 *-------------------------------------------------------
 */

#define HAL_FLASH_TEST_UTIL           0x10000000

#define FLASH_HAL_INIT_DEV            0x10000001
#define FLASH_HAL_CFG_DEV             0x10000002
#define FLASH_HAL_ENABLE_ECC_ON_MAIN  0x10000003
#define FLASH_HAL_ENABLE_ECC_ON_SPARE 0x10000004
#define FLASH_HAL_SET_ECC             0x10000005
#define FLASH_HAL_DISPLAY_DEV_CFG     0x10000006
#define FLASH_HAL_SEQ_INIT            0x10000007
#define FLASH_HAL_SEQ_END             0x10000008
#define FLASH_HAL_SEQ_EXEC            0x10000009
#define FLASH_HAL_SEQ_ISSUE_CMD       0x1000000A
#define FLASH_HAL_SEQ_ISSUE_SUBCMD    0x1000000B
#define FLASH_HAL_SEQ_ISSUE_ADDR      0x1000000C
#define FLASH_HAL_SEQ_ISSUE_WAIT      0x1000000D
#define FLASH_HAL_SEQ_ISSUE_READ      0x1000000E
#define FLASH_HAL_SEQ_ISSUE_WRITE     0x1000000F
#define FLASH_HAL_SEQ_DISPLAY         0x10000010

/* Flash.bin related defines */
#define FLASH_NAND_PAGE_SIZE_4096           4096
#define FLASH_NAND_PAGE_SIZE_2048           2048
#define FLASH_NAND_PAGE_SIZE_512            512
#define FLASH_WM_VERSION_WM7_SIGNATURE      0x44
#define FLASH_WM_VERSION_WM7                7
#define FLASH_WM_VERSION_WM6                6


/* This structure is used to communcate between TRACE32 Debugger and
 * our FLASH plug-in. */
extern struct flash_prog_param prog_param;

#endif /*  __FLASH_PARAM_H__ */
