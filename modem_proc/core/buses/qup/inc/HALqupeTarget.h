#ifndef _HALQUPETARGET_H
#define _HALQUPETARGET_H

/*
===========================================================================

FILE:   HALqupTarget.h

DESCRIPTION:
    This file contains enumerations for QUP specific to 8960

===========================================================================

        Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/buses/qup/inc/HALqupeTarget.h#1 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
07/01/13 LK     Redirected hwio to reference the generated qup_hwio.h.
06/12/12 LK     Removed dependency on spi types.
04/19/12 ddk    Added changes for avoiding symbol match with I2C library.
                Added updated HAL implementation used for I2C.
03/28/12 ddk    Added QUP physical address info for BAM cmd desc creation.
02/22/12 ddk    Added SPI BAM mode requirements.
11/06/11 LK     Created

===========================================================================
        Copyright c 2011-2014 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/

#include "QupTypes.h"
#include "qup_hwio.h"

#define HAL_QUP_MAX_BUS_NUM   12



/** @enum HAL_qupe_SingleTransferSizeLimitType .        */
typedef enum HAL_qupe_SingleTransferSizeLimitType
{
   HAL_QUPE_SINGLE_TRANSFER_SIZE_MIN   = 4  , /**<Min allowed transfer size.*/
   HAL_QUPE_SINGLE_TRANSFER_SIZE_MAX   = 32 , /**<Max allowed transfer size.*/

   HAL_QUPE_SINGLE_TRANSFER_SIZE_EXTEND  = 0x7FFFFFFF
} HAL_qupe_SingleTransferSizeLimitType;

/** @enum HAL_qupe_FifoModeOutTransferCountLimitType.        */
typedef enum HAL_qupe_FifoModeOutTransferCountLimitType
{
   /**
      Max allowed transfer size.
    */
   HAL_QUPE_FIFO_MODE_OUT_TRANSFER_CNT_MAX   = 0xFFFF , 

   HAL_QUPE_FIFO_MODE_OUT_TRANSFER_CNT_EXTEND  = 0x7FFFFFFF
} HAL_qupe_FifoModeOutTransferCountLimitType;


/** @enum HAL_qupe_BlockModeOutTransferCountLimitType.        */
typedef enum HAL_qupe_BlockModeOutTransferCountLimitType
{
   /**
      Max allowed transfer size.
    */
   HAL_QUPE_BLOCK_MODE_OUT_TRANSFER_CNT_MAX   = 0xFFFF , 

   HAL_QUPE_BLOCK_MODE_OUT_TRANSFER_CNT_EXTEND  = 0x7FFFFFFF
} HAL_qupe_BlockModeOutTransferCountLimitType;

/** @enum HAL_qupe_BlockModeInTransferCountLimitType.        */
typedef enum HAL_qupe_BlockModeInTransferCountLimitType
{
   /**
      Max allowed transfer size.
    */
   HAL_QUPE_BLOCK_MODE_IN_TRANSFER_CNT_MAX   = 0xFFFF , 

   HAL_QUPE_BLOCK_MODE_IN_TRANSFER_CNT_EXTEND  = 0x7FFFFFFF
} HAL_qupe_BlockModeInTransferCountLimitType;

/** @enum HAL_qupe_FifoModeInTransferCountLimitType.        */
typedef enum HAL_qupe_FifoModeInTransferCountLimitType
{
   /**
      Max allowed transfer size.
    */
   HAL_QUPE_FIFO_MODE_IN_TRANSFER_CNT_MAX   = 0xFFFF , 

   HAL_QUPE_FIFO_MODE_IN_TRANSFER_CNT_EXTEND  = 0x7FFFFFFF
} HAL_qupe_FifoModeInTransferCountLimitType;

/** @enum HAL_qupe_TimeOutLimitType Time out limit.        */
typedef enum HAL_qupe_TimeOutLimitType
{
   HAL_QUPE_TIME_OUT_MAX   = 0xFFFF, /**<Max allowed timeout.*/

   HAL_QUPE_TIME_OUT_EXTEND  = 0x7FFFFFFF
} HAL_qupe_TimeOutLimitType;

/** @enum HAL_qupe_I2cFreqLimitType I2C frequency limit.        */
typedef enum HAL_qupe_I2cFreqLimitType
{
   HAL_QUPE_FS_FREQ_KHZ_MAX   = 1000, /**<Max allowed timeout.*/

   HAL_QUPE_HS_FREQ_KHZ_MAX  = 3400
} HAL_qupe_I2cFreqLimitType;

/*------------------------------------------------------------------------------
 * MODULE: qup
 *------------------------------------------------------------------------------*/
#define HWIO_INTLOCK()
#define INTLOCK()

#define HWIO_INTFREE()
#define INTFREE()


#define __inp(port)         (*((volatile unsigned char *) (port)))
#define __inpw(port)        (*((volatile unsigned short *) (port)))
#define __inpdw(port)       (*((volatile unsigned int *) (port)))

#define __outp(port, val)   (*((volatile unsigned char *) (port)) = ((unsigned char) (val)))
#define __outpw(port, val)  (*((volatile unsigned short *) (port)) = ((unsigned short) (val)))
#define __outpdw(port, val) (*((volatile unsigned int *) (port)) = ((unsigned int) (val)))


#define in_byte(addr)               (__inp(addr))
#define in_byte_masked(addr, mask)  (__inp(addr) & (mask)) 
#define out_byte(addr, val)         __outp(addr,val)
#define out_byte_masked(io, mask, val, shadow)  \
  HWIO_INTLOCK();    \
  (void) out_byte( io, shadow); \
  shadow = (shadow & (unsigned short)(~(mask))) | ((unsigned short)((val) & (mask))); \
  HWIO_INTFREE()
#define out_byte_masked_ns(io, mask, val, current_reg_content)  \
  (void) out_byte( io, ((current_reg_content & (unsigned short)(~(mask))) | \
                       ((unsigned short)((val) & (mask)))) )

#define in_word(addr)              (__inpw(addr))
#define in_word_masked(addr, mask) (__inpw(addr) & (mask))
#define out_word(addr, val)        __outpw(addr,val)
#define out_word_masked(io, mask, val, shadow)  \
  HWIO_INTLOCK( ); \
  shadow = (shadow & (unsigned short)(~(mask))) |  ((unsigned short)((val) & (mask))); \
  (void) out_word( io, shadow); \
  HWIO_INTFREE( )
#define out_word_masked_ns(io, mask, val, current_reg_content)  \
  (void) out_word( io, ((current_reg_content & (unsigned short)(~(mask))) | \
                       ((unsigned short)((val) & (mask)))) )

#define in_dword(addr)              (__inpdw(addr))
#define in_dword_masked(addr, mask) (__inpdw(addr) & (mask))
#define out_dword(addr, val)        __outpdw(addr,val)
#define out_dword_masked(io, mask, val, shadow)  \
   HWIO_INTLOCK( ); \
   shadow = (shadow & (unsigned int)(~(mask))) | ((unsigned int)((val) & (mask))); \
   (void) out_dword( io, shadow); \
   HWIO_INTFREE( )
#define out_dword_masked_ns(io, mask, val, current_reg_content) \
  (void) out_dword( io, ((current_reg_content & (unsigned int)(~(mask))) | ((unsigned int)((val) & (mask)))) )




/*
  The GSBI QUPS are identical and registers and fields are identical
*/

#define QUPE_PHYS_ADDR(addr, reg_name)  \
   (addr + HWIO_##reg_name##_OFFS)

#define QUPE_FMSK(reg_name, field) (HWIO_##reg_name##_##field##_BMSK)
#define QUPE_SHFT(reg_name, field) (HWIO_##reg_name##_##field##_SHFT)



#define QUPE_INF(qup_block_address, reg_name, field)\
 (HWIO_##reg_name##_INM(qup_block_address,QUPE_FMSK(reg_name, field)) >> QUPE_SHFT(reg_name, field))

#define QUPE_IN(qup_block_address, reg_name) (HWIO_##reg_name##_IN(qup_block_address))

#define QUPE_OUT(qup_block_address, reg_name, data) (HWIO_##reg_name##_OUT(qup_block_address,data))

#define QUPE_OUTF(qup_block_address, reg_name, field, data)\
         (HWIO_##reg_name##_OUTM(qup_block_address,\
                                      QUPE_FMSK(reg_name, field),\
                                      (data) << QUPE_SHFT(reg_name, field)))

#define QUPE_OUTI(qup_block_address, reg_name,ind, data) (HWIO_##reg_name##_OUTI(qup_block_address,ind,data))
#define QUPE_INI(qup_block_address, reg_name,ind) (HWIO_##reg_name##_INI(qup_block_address,ind))

#endif


