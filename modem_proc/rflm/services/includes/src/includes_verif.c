/*!
  @file
  includes_verif.c

  @brief
  Verification of modem_fw macros.

  @detail
  Compiler assertions that macros perform as expected. No code generated
  from this file.

*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/services/includes/src/includes_verif.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "rflm.h"


/*===========================================================================

  Internal consistency checks

===========================================================================*/

COMPILE_ASSERT( (sizeof(uint8) == 1) && (sizeof(int8) == 1) );
COMPILE_ASSERT( (sizeof(uint16) == 2) && (sizeof(int16) == 2) );
COMPILE_ASSERT( (sizeof(uint32) == 4) && (sizeof(int32) == 4) );
COMPILE_ASSERT( (sizeof(uint64) == 8) && (sizeof(int64) == 8) );
COMPILE_ASSERT( sizeof(vect32) == 4);
COMPILE_ASSERT( sizeof(vect64) == 8 );

typedef struct { int x; int y ALIGN(16); } modem_fw_alignment_check_t__ ALIGN(64);
/*lint -save -d__alignof__(x) -e24 */
COMPILE_ASSERT( __alignof__(modem_fw_alignment_check_t__) == 64);
/*lint -restore */
COMPILE_ASSERT(FPOS(modem_fw_alignment_check_t__,y) == 16);
typedef PACK(struct) { uint8 x; uint32 y; } modem_fw_pack_check_t__;
COMPILE_ASSERT( sizeof(modem_fw_pack_check_t__) == 5);

COMPILE_ASSERT(QUANTIZE_DOWN(6,3) == 6);
COMPILE_ASSERT(QUANTIZE_DOWN(7,3) == 6);
COMPILE_ASSERT(QUANTIZE_DOWN(8,3) == 6);
COMPILE_ASSERT(QUANTIZE_DOWN(9,3) == 9);
COMPILE_ASSERT(QUANTIZE_UP(6,3) == 6);
COMPILE_ASSERT(QUANTIZE_UP(7,3) == 9);
COMPILE_ASSERT(QUANTIZE_UP(8,3) == 9);
COMPILE_ASSERT(QUANTIZE_UP(9,3) == 9);

COMPILE_ASSERT(IS_POW2(4) == 1);
COMPILE_ASSERT(IS_POW2(5) == 0);
COMPILE_ASSERT(IS_POW2(6) == 0);
COMPILE_ASSERT(IS_POW2(7) == 0);
COMPILE_ASSERT(IS_POW2(8) == 1);

COMPILE_ASSERT(FLOOR_POW2(4) == 4);
COMPILE_ASSERT(FLOOR_POW2(5) == 4);
COMPILE_ASSERT(FLOOR_POW2(6) == 4);
COMPILE_ASSERT(FLOOR_POW2(7) == 4);
COMPILE_ASSERT(FLOOR_POW2(8) == 8);
COMPILE_ASSERT(FLOOR_POW2(0x400000) == 0x400000);
COMPILE_ASSERT(FLOOR_POW2(0x404000) == 0x400000);
COMPILE_ASSERT(FLOOR_POW2(0x505000) == 0x400000);
COMPILE_ASSERT(FLOOR_POW2(0x606000) == 0x400000);
COMPILE_ASSERT(FLOOR_POW2(0x707000) == 0x400000);
COMPILE_ASSERT(FLOOR_POW2(0x808000) == 0x800000);

COMPILE_ASSERT(CEIL_POW2(4) == 4);
COMPILE_ASSERT(CEIL_POW2(5) == 8);
COMPILE_ASSERT(CEIL_POW2(6) == 8);
COMPILE_ASSERT(CEIL_POW2(7) == 8);
COMPILE_ASSERT(CEIL_POW2(8) == 8);
COMPILE_ASSERT(CEIL_POW2(0x400000) == 0x400000);
COMPILE_ASSERT(CEIL_POW2(0x404000) == 0x800000);
COMPILE_ASSERT(CEIL_POW2(0x505000) == 0x800000);
COMPILE_ASSERT(CEIL_POW2(0x606000) == 0x800000);
COMPILE_ASSERT(CEIL_POW2(0x707000) == 0x800000);
COMPILE_ASSERT(CEIL_POW2(0x808000) == 0x1000000);

COMPILE_ASSERT(POPCOUNT(0) == 0);
COMPILE_ASSERT(POPCOUNT(4) == 1);
COMPILE_ASSERT(POPCOUNT(0xFFFFFFFF) == 32);
COMPILE_ASSERT(POPCOUNT(0x12345678) == 13);

COMPILE_ASSERT(CLZ(0) == 32);
COMPILE_ASSERT(CLZ(0xFFFFFFFF) == 0);
COMPILE_ASSERT(CLZ(0x10000) == 15);
COMPILE_ASSERT(CLZ(0x1234) == 19);

COMPILE_ASSERT(FLOOR_LOG2(1) == 0);
COMPILE_ASSERT(FLOOR_LOG2(2) == 1);
COMPILE_ASSERT(FLOOR_LOG2(3) == 1);
COMPILE_ASSERT(FLOOR_LOG2(4) == 2);

COMPILE_ASSERT(CEIL_LOG2(1) == 0);
COMPILE_ASSERT(CEIL_LOG2(2) == 1);
COMPILE_ASSERT(CEIL_LOG2(3) == 2);
COMPILE_ASSERT(CEIL_LOG2(4) == 2);

COMPILE_ASSERT(IS_POW2(CACHE_LINE_SZ));

COMPILE_ASSERT(CIRC_K(3) == 0);
COMPILE_ASSERT(CIRC_K(255) == 6);
COMPILE_ASSERT(CIRC_K(256) == 7);

COMPILE_ASSERT(CIRC_M(255,0) == 0x060000FF);
COMPILE_ASSERT(CIRC_M(0x1FFFF,0) == 0x0F01FFFF);
COMPILE_ASSERT(CIRC_M(255,0xFF) == 0x16FE00FF);
COMPILE_ASSERT((uint32) CIRC_M(255,-1) == 0xF6FE00FF);

COMPILE_ASSERT(BIT(4) == 16);
COMPILE_ASSERT(BITL(40) == 0x10000000000uLL);

COMPILE_ASSERT(BITMASK(0) == 0);
COMPILE_ASSERT(BITMASK(4) == 0xF);
COMPILE_ASSERT(BITMASK(31) == 0x7FFFFFFF);
COMPILE_ASSERT(BITMASK(32) == 0xFFFFFFFF);
COMPILE_ASSERT(BITMASK(33) == 0xFFFFFFFF);

COMPILE_ASSERT(BITMASKL(0) == 0x0uLL);
COMPILE_ASSERT(BITMASKL(4) == 0xFuLL);
COMPILE_ASSERT(BITMASKL(33) == 0x1FFFFFFFFuLL);
COMPILE_ASSERT(BITMASKL(63) == 0x7FFFFFFFFFFFFFFFuLL);
COMPILE_ASSERT(BITMASKL(64) == 0xFFFFFFFFFFFFFFFFuLL);
COMPILE_ASSERT(BITMASKL(65) == 0xFFFFFFFFFFFFFFFFuLL);

COMPILE_ASSERT(RFLM_HANDLE_COUNTS <= 32);
COMPILE_ASSERT(RFLM_MAX_NUM_HANDLE_SCRIPT_BUF <= 64);

 
