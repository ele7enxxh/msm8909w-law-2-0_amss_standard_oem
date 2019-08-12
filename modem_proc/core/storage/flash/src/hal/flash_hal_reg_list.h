#ifndef FLASH_HAL_REG_LIST_H
#define FLASH_HAL_REG_LIST_H
/*=======================================================================
 * FILE:        flash_hal_reg_list.h
 *
 * SERVICES:    Header file for Register list preparation functionality
 *
 * DESCRIPTION: When using data mover, HAL prepares a list of register
 *              read/write operations and returns to driver. This file
 *              contains header file declarations for the register list
 *              preparation.
 *
 * Copyright (c) 2008-2009, 2012, 2014 Qualcomm Technologies Incorporated.
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
 *  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash/src/hal/flash_hal_reg_list.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 06/13/14     sb      Update to support 8909
 * 09/05/12     sv      Remove dynamic selection of register access method
 * 07/21/09     sv      Use flash_hal wrapper api's for memory operations
 * 03/12/09     eo      Fix FLASH_HAL_IN2 macro syntax
 * 06/30/08     eo      Fix for FLASH_HAL_IN2 APIs
 * 06/30/08     bb      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

/*-----------------------------------------------------------------------
 *           Definitions and Constants
 *----------------------------------------------------------------------*/

#define FLASH_HAL_GET_VADDR(ctlr,reg)      HAL_##ctlr##_##reg##_VADDR
#define FLASH_HAL_GET_PADDR(ctlr,reg)      HAL_##ctlr##_##reg##_PADDR
#define FLASH_HAL_GET_VADDRI(ctlr,reg,cs)  HAL_##ctlr##_##reg##_VADDR(cs)
#define FLASH_HAL_GET_PADDRI(ctlr,reg,cs)  HAL_##ctlr##_##reg##_PADDR(cs)

/* IN Macros */
#ifndef ENABLE_FLASH_BAM
  #define FLASH_HAL_USE_HWIO
#endif

#if defined(FLASH_HAL_USE_HWIO)
/* Direct mapping ofr IN/OUT macros uses HWIO */

/* Register read of high and low words without any options */
#define FLASH_HAL_IN2(ctlr, reg, dest_hi, dest_lo)  \
  *((uint32 *) dest_lo) = HWIO_IN(##reg)

/* Standard register read without any options */
#define FLASH_HAL_IN(ctlr, reg, dest)               \
   *((uint32 *) dest) = HWIO_IN(##reg)

/* Register Read with Chip select */
#define FLASH_HAL_INI(ctlr, reg, cs, dest)          \
   *((uint32 *)dest) = HWIO_INI(##reg, cs)

/* Register Read with options */
#define FLASH_HAL_INS(ctlr, reg, dest, op)          \
   *((uint32 *) dest) = HWIO_IN(##reg)

/* OUT Macros */

/* Register Write */
#define FLASH_HAL_OUT(ctlr,reg,val)                 \
   HWIO_OUT(##reg, val)

/* Register write with chip select info */
#define FLASH_HAL_OUTI(ctlr, reg, cs, val)          \
   HWIO_OUTI(##reg, cs, val)

/* Register write with options */
#define FLASH_HAL_OUTS(ctlr, reg, val, op)          \
   HWIO_OUT(##reg, val)

/* Memory Copy Macros  */
#define FLASH_HAL_COPY_TO_DEVICE(addr, src, len)    \
   flash_hal_memcpy((void *)(addr), (void *)src, len)

#define FLASH_HAL_COPY_FROM_DEVICE(addr, dest, len) \
   flash_hal_memcpy((void *)dest, (void *)(addr), len)

#else
/* Indirect mapping ofr IN/OUT macros uses register read/write lists */

/* Register read of high and low words without any options */
#define FLASH_HAL_IN2(ctlr, reg, dest_hi, dest_lo)   do  { \
    flash_hal_gen_read_list((uint32)dest_hi,             \
    FLASH_HAL_GET_PADDR(ctlr, reg)+2, FLASH_HAL_DEST_VIRTUAL, 4); \
    flash_hal_gen_read_list((uint32)dest_lo,             \
    FLASH_HAL_GET_PADDR(ctlr, reg), FLASH_HAL_DEST_VIRTUAL, 4);   \
  }while(0)

/* Standard register read without any options */
#define FLASH_HAL_IN(ctlr, reg, dest)               \
    flash_hal_gen_read_list((uint32)dest,           \
      FLASH_HAL_GET_PADDR(ctlr, reg), 0, 4)

/* Register Read with Chip select */
#define FLASH_HAL_INI(ctlr, reg, cs, dest)          \
   flash_hal_gen_read_list((uint32)dest,            \
     FLASH_HAL_GET_PADDRI(ctlr,reg,cs),0,4)

/* Register Read with options */
#define FLASH_HAL_INS(ctlr, reg, dest, op)          \
   flash_hal_gen_read_list((uint32)dest,            \
     FLASH_HAL_GET_PADDR(ctlr, reg), op, 4)

/* OUT Macros */

/* Register Write */
#define FLASH_HAL_OUT(ctlr,reg,val)                               \
   flash_hal_gen_write_list(FLASH_HAL_GET_PADDR(ctlr, reg),       \
     (uint32)val, 0, 4)

/* Register write with chip select info */
#define FLASH_HAL_OUTI(ctlr, reg, cs, val)                        \
   flash_hal_gen_write_list(FLASH_HAL_GET_PADDRI(ctlr,reg,cs),    \
     (uint32)val, 0, 4)

/* Register write with options */
#define FLASH_HAL_OUTS(ctlr, reg, val, op)                        \
   flash_hal_gen_write_list(FLASH_HAL_GET_PADDR(ctlr, reg),       \
     (uint32)val, op, 4)

/* Memory Copy Macros  */
#define FLASH_HAL_COPY_TO_DEVICE(addr, src, len)                  \
   flash_hal_gen_read_list((uint32) addr, (uint32) src,           \
     FLASH_HAL_SRC_VIRTUAL, len)

#define FLASH_HAL_COPY_FROM_DEVICE(addr, dest, len)               \
   flash_hal_gen_read_list((uint32)dest,                          \
     (uint32) addr, FLASH_HAL_DEST_VIRTUAL, len)

#endif

/*---------------------------------------------------------------------
 *             Function Declarations
 * ------------------------------------------------------------------*/

/* Adds a register read command to the HAL register list */
void flash_hal_gen_read_list(uint32 dst_addr, uint32 src_addr, uint32 dwOp,
  uint32 op_len);

/* Adds a register write command to the HAL register list */
void flash_hal_gen_write_list(uint32 dst_addr, uint32 dwVal, uint32 dwOp,
  uint32 op_len);

#endif /* FLASH_HAL_REG_LIST_H */
