#ifndef _I2CREGISTERTRANSFER_H_
#define _I2CREGISTERTRANSFER_H_
/*
===========================================================================

FILE:   I2cRegisterTransfer.h

DESCRIPTION:
    This file contains macros to encapsulate i2c register operations. 


===========================================================================

        Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/buses/api/i2c/I2cRegisterTransfer.h#1 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
08/25/15 VG     Created

===========================================================================
        Copyright c 2015 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/
#include "I2cTransfer.h"


typedef struct I2cRegSeqObj
{
   I2cTransfer aTr[2];       /**< transfer array. */ 
   I2cIoVec    io_vec[2];    /**< vector array. */ 
   uint8       offset[2];    /**< offset buffer. */ 
   I2cSequence seq;          /**< Sequence obj. */
} I2cRegSeqObj ;

typedef enum I2cRegOffsetType 
{
   I2cRegOffset_8Bit, /**< 8 bit offset. */  
   I2cRegOffset_16Bit /**< 16 bit offset. */
} I2cRegOffsetType;

typedef struct I2cSimpleTrObj
{
   I2cTransfer aTr[1];       /**< transfer array. */ 
   I2cIoVec    io_vec[1];    /**< vector array. */ 
} I2cSimpleTrObj ;

#define I2C_CUST_TR_OBJ(NAME, NUM_VECTORS) \
struct I2cCustTrqObj##NAME                \
{                                         \
   I2cTransfer aTr[1];                    \
   I2cIoVec    io_vec[(NUM_VECTORS)];     \
} ##NAME ;                                \


#define I2C_CUST_SEQ_OBJ(NAME, NUM_TRANSFERS, NUM_VECTORS) \
struct I2cCustSeqObj##NAME                \
{                                         \
   I2cTransfer  aTr[(NUM_TRANSFERS)];     \
   I2cIoVec     io_vec[(NUM_VECTORS)];    \
   I2cSequence  seq;                      \
} ##NAME ;                                \


/** @brief Initialize a transfer obj.

    @param[in,out] pTr             Pointer to transfer.
    @param[in]     pVec            Pointer to vector to set the tranfers vector.
    @param[in]     eTranDirection  Transfer direction.
    @param[in]     eTranCtxt       Transfer context.

    @return             Nothing.
  */
static __inline void I2c_TrObj_Init
(
   I2cTransfer          *pTr,            
   I2cIoVec              *pVec,          
   I2cTranDirection      eTranDirection, 
   I2cTranCtxt           eTranCtxt       
)
{
   pTr->eTranDirection   = eTranDirection;
   pTr->eTranCtxt        = eTranCtxt;
   pTr->pIoVec           = pVec;
   pTr->uTrSize          = 0;
}

/** @brief Initializes the vector at index with buffer and size and updates transfer size.

    @param[in,out] pTr             Pointer to transfer.
    @param[in]     uBuffIndex      Index of vector/buffer.
    @param[in]     pBuff           Pointer to buffer.
    @param[in]     size            Size of buffer.

    @return             Nothing.
  */
static __inline void I2c_TrObj_AddBuffer
(
   I2cTransfer           *pTr      , 
   uint32                uBuffIndex,
   uint8                 *pBuff    ,
   uint32                size
)
{
   pTr->pIoVec[uBuffIndex].pBuff = pBuff;
   pTr->pIoVec[uBuffIndex].size  = size;
   pTr->uTrSize                 += size;
}

/** @brief Initializes a sequence object.

    @param[in,out] pSeq             Pointer to transfer.
    @param[in]     pTr              Pointer to transfer or transfer array.
    @param[in]     uNumTr           Number of transfers in the transfer array.

    @return             Nothing.
  */
static __inline void I2c_SeqObj_Init
(
   I2cSequence           *pSeq, 
   I2cTransfer           *pTr ,
   uint32                uNumTr
)
{
   pSeq->pTransfer     = pTr;
   pSeq->uNumTransfers = uNumTr;
}

/** @brief Initializes a Register tranfer object for read transaction.

    @param[in,out] pRegObj          Pointer to register object.

    @return             Nothing.
  */
static __inline void I2c_RegObj_InitRead(I2cRegSeqObj *pRegObj)
{
   I2c_SeqObj_Init(&pRegObj->seq, pRegObj->aTr, 2);
   I2c_TrObj_Init(&(pRegObj->aTr[0]),&(pRegObj->io_vec[0]), I2cTranDirOut, I2cTrCtxSeqStart);
   I2c_TrObj_Init(&(pRegObj->aTr[1]),&(pRegObj->io_vec[1]), I2cTranDirIn, I2cTrCtxSeqEnd);
} 

/** @brief Initializes a Register tranfer object for write transaction.

    @param[in,out] pRegObj          Pointer to register object.

    @return             Nothing.
  */
static __inline void I2c_RegObj_InitWrite(I2cRegSeqObj *pRegObj)
{
   I2c_SeqObj_Init(&(pRegObj->seq), &(pRegObj->aTr[0]), 1);
   I2c_TrObj_Init(&(pRegObj->aTr[0]),&(pRegObj->io_vec[0]), I2cTranDirOut, I2cTrCtxNotASequence);
} 

/** @brief Sets the offset for this Register tranfer object.

    @param[in,out] pRegObj          Pointer to register object.
    @param[in]     offset           Register offset .
    @param[in]     offsetType       Offset type 8 bit vs 16 bit .

    @return             Nothing.
  */
static __inline void I2c_RegObj_SetOffset
(
   I2cRegSeqObj *pRegObj,
   uint32 offset, 
   I2cRegOffsetType offsetType
)
{
   uint32 uSize;

   pRegObj->offset[0] = (uint8)(offset &0xFF);     
   if (offsetType == I2cRegOffset_16Bit) {
     uSize = 2;
     pRegObj->offset[1] = (uint8)((offset >> 8) &0xFF);
   }
   else {
     uSize = 1;
   }
   // set transfer[0], vector[0] buffer
   I2c_TrObj_AddBuffer(&(pRegObj->aTr[0]), 0,pRegObj->offset, uSize);
}

/** @brief Sets the read vector on the register access object.

    @param[in,out] pRegObj          Pointer to register object.
    @param[in,out] pBuff            Pointer to buffer to put the data.
    @param[in]     size             Size of the buffer.

    @return             Nothing.
  */
static __inline void I2c_RegObj_SetReadIoVector
(
   I2cRegSeqObj *pRegObj,
   uint8        *pBuff, 
   uint32       size
)
{
   // set transfer[1], vector[1] buffer     
   I2c_TrObj_AddBuffer(&(pRegObj->aTr[1]), 0,pBuff, size);     
}

/** @brief Sets the write vector on the register access object.

    @param[in,out] pRegObj          Pointer to register object.
    @param[in,out] pBuff            Pointer to buffer from where to get the data.
    @param[in]     size             Size of the buffer.

    @return             Nothing.
  */
static __inline void I2c_RegObj_SetWriteIoVector
(
   I2cRegSeqObj *pRegObj,
   uint8        *pBuff, 
   uint32       size
)
{
   // set transfer[0], vector[1] buffer          
   I2c_TrObj_AddBuffer(&(pRegObj->aTr[0]), 1,pBuff, size);     
}

#endif // _I2CREGISTERTRANSFER_H_

