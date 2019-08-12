#ifndef _HALQUPETARGET_H
#define _HALQUPETARGET_H

/*
===========================================================================

FILE:   HALqupTarget.h

DESCRIPTION:
    This file contains enumerations for QUP specific to 8960

===========================================================================

        Edit History

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/spi/src/hal/inc/HALqupeTarget.h#1 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
06/12/12 LK     Removed dependency on spi types.
04/19/12 ddk    Added changes for avoiding symbol match with I2C library.
                Added updated HAL implementation used for I2C.
03/28/12 ddk    Added QUP physical address info for BAM cmd desc creation.
02/22/12 ddk    Added SPI BAM mode requirements.
11/06/11 LK     Created

===========================================================================
        Copyright c 2011-2013 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/

#include "QupTypes.h"
#include "HALqupeHWIO.h"


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



#define QUPE_PHYS_ADDR(addr, reg_name)  \
   (addr + HWIO_GSBI_##reg_name##_OFFSET)

/*
  The GSBI QUPS are identical and registers and fields are identical
*/

#define QUPE_FMSK(reg_name, field) (HWIO_GSBI_##reg_name##_##field##_BMSK)
#define QUPE_SHFT(reg_name, field) (HWIO_GSBI_##reg_name##_##field##_SHFT)



#define QUPE_INF(qup_block_address, reg_name, field)\
 (HWIO_GSBI_##reg_name##_INM(qup_block_address,QUPE_FMSK(reg_name, field)) >> QUPE_SHFT(reg_name, field))

//  (in_dword_masked(QUPE_REG_ADDR(qup_idx, reg_name), QUPE_FMSK(reg_name, field)) >> QUPE_SHFT(reg_name, field))

#define QUPE_IN(qup_block_address, reg_name) (HWIO_GSBI_##reg_name##_IN(qup_block_address))

#define QUPE_OUT(qup_block_address, reg_name, data) (HWIO_GSBI_##reg_name##_OUT(qup_block_address,data))
  //(out_dword(QUPE_REG_ADDR(qup_idx, reg_name), data))

#define QUPE_OUTF(qup_block_address, reg_name, field, data)\
         (HWIO_GSBI_##reg_name##_OUTM(qup_block_address,\
                                      QUPE_FMSK(reg_name, field),\
                                      (data) << QUPE_SHFT(reg_name, field)))


#endif


