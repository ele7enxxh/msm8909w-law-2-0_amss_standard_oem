#ifndef _I2CTRANSFER_H_
#define _I2CTRANSFER_H_
/*
===========================================================================

FILE:   I2cTransfer.h

DESCRIPTION:
    This file contains the structures for transfer 


===========================================================================

        Edit History

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/buses/api/i2c/I2cTransfer.h#1 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
09/26/11 LK     Created

===========================================================================
        Copyright c 2011 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/

#include "I2cTransferCfg.h"

#include "I2cTypes.h"


typedef struct I2cBuffDesc 
{
   uint8  *pBuff;        /**< Virtual address of the buffer. */
   uint32 uBuffSize;     /**< Size of the buffer in bytes. */
//   uint32 uPhysAddr;    
} I2cBuffDesc;

typedef enum _eTranDirection{
   I2cTranDirIn, /**< input from device. */
   I2cTranDirOut /**< going to the device, a write. */
} I2cTranDirection;
   
typedef   enum _eTranCtxt{
   I2cTrCtxNotASequence,  /**< Transfer is not part of a sequence. */
   I2cTrCtxSeqStart   ,   /**< Transfer is the first in a sequence. */
   I2cTrCtxSeqContinue ,  /**< Transfer constinues a started sequence. */
   I2cTrCtxSeqEnd         /**< Transfer ends the sequence. */
}  I2cTranCtxt ;

typedef struct I2cTransfer 
{
   I2cBuffDesc          *pI2cBuffDesc;    /**< IO Vec Buffer. */
   uint32                uTrSize;         /**< Transfer total number of bytes. */
   I2cTransferConfig     tranCfg;         /**< Transfer configuration. */
   I2cTranDirection      eTranDirection;  /**< Transfer direction. */
   I2cTranCtxt           eTranCtxt ;      /**< Transfer context. */

} I2cTransfer;

typedef struct I2cIoResult
{
  int32  nOperationResult;               /**< This is the operation result. */
  uint32 uOutByteCnt;                    /**< This indicates num bytes sent. */
  uint32 uInByteCnt;                    /**< This indicates num bytes received. */
} I2cIoResult;

typedef struct I2cSequence 
{
   I2cTransfer *pTransfer;   /**< Pointer to array of transfers. */
   uint32      uNumTransfers;/**< Number of transfers in this sequence. */
   //uint32      uTimeoutMsec; /**< Sequence Transfer timeout in milliseconds.*/
} I2cSequence;

#endif // _I2CTRANSFER_H_

