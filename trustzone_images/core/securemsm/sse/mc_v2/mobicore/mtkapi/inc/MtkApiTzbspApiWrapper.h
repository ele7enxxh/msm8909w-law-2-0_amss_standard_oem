/**
 * Copyright (C) 2011-2012, Giesecke&Devrient GmbH
 * Axel Heider (axh)
 *
 * Architecture: ARM
 * Platform:     Qualcomm MSM8960
 *
 */


#ifndef __MTK_API_TZBSP_API_WRAPPER_H__
#define __MTK_API_TZBSP_API_WRAPPER_H__

#include "MtkApiTzbsp_qc.h"

//------------------------------------------------------------------------------
uint32_t tzbspApi_core_isSwFuseBlown(
    tzbsp_sw_fuse_t fuse_num,
    bool_t*         is_blown,
    uint32_t        is_blown_sz);

bool_t tzbspApi_core_isSTagArea(
    uint32_t  tag,
    uint32_t  start,
    uint32_t  end);

int tzbspApi_hdmiStatusRead(
    uint32_t*  hdmiEnable,
    uint32_t*  hdmiSense,
    uint32_t*  hdcpAuth);


int tzbspApi_I2COpen(
    TZBSP_I2CPD_DEVICE_ET       i2cPdDeviceId);

int tzbspApi_I2CRead(
    TZBSP_I2CPD_DEVICE_ET       i2cPdDeviceId,
    const tzbspI2cConfig_t*     pConfig,
    tzbspI2cTransactionInfo_t*  pReadInfo);

int tzbspApi_I2CWrite(
    TZBSP_I2CPD_DEVICE_ET       i2cPdDeviceId,
    const tzbspI2cConfig_t*     pConfig,
    tzbspI2cTransactionInfo_t*  pWriteInfo);

int tzbspApi_I2CClose(
    TZBSP_I2CPD_DEVICE_ET       i2cPdDeviceId);


//------------------------------------------------------------------------------
uint32_t tzbspApi_crypto_hash(
    TZBSP_HASH_ALGO_ET  alg,
    const void         *msg,
    uint32_t            msgLen,
    void                *digest,
    uint32_t            digestLen);

uint32_t tzbspApi_crypto_hashInit(
    TZBSP_HASH_ALGO_ET  alg,
    tzbsp_hash_ctx      **hashCtx);

uint32_t tzbspApi_crypto_hashUpdate(
    const tzbsp_hash_ctx  *hashCtx,
    const void            *msg,
    uint32_t              msgLen);

uint32_t tzbspApi_crypto_hashFinal(
    const tzbsp_hash_ctx  *hashCtx,
    void                  *digest,
    uint32_t              digestLen);

uint32_t tzbspApi_crypto_hashFreeCtx(
    tzbsp_hash_ctx *hashCtx);

uint32_t tzbspApi_crypto_cipherInit(
    TZBSP_CIPHER_ALGO_ET  alg,
    tzbsp_cipher_ctx      **cipherCtx);

uint32_t tzbspApi_crypto_cipherFreeCtx(
    tzbsp_cipher_ctx *cipher_ctx);

uint32_t tzbspApi_crypto_cipherSetParam(
    tzbsp_cipher_ctx       *cipherCtx,
    TZBSP_CIPHER_PARAM_ET  paramId,
    const void             *param,
    uint32_t               paramLen);

uint32_t tzbspApi_crypto_cipherGetParam(
    const tzbsp_cipher_ctx  *cipherCtx,
    TZBSP_CIPHER_PARAM_ET   paramId,
    void                    *param,
    uint32_t                *paramLen);

uint32_t tzbspApi_crypto_cipherEncrypt(
    const tzbsp_cipher_ctx  *cipherCtx,
    const void              *pt,
    uint32_t                ptLen,
    void                    *ct,
    uint32_t                *ctLen);

uint32_t tzbspApi_crypto_cipherDecrypt(
    const tzbsp_cipher_ctx  *cipherCtx,
    const void              *ct,
    uint32_t                ctLen,
    void*                   pt,
    uint32_t                *ptLen);

uint32_t tzbspApi_crypto_hmac(
    TZBSP_HMAC_ALGO_ET  alg,
    const void          *msg,
    uint32_t            msgLen,
    const void          *key,
    uint32_t            keyLen,
    void                *msgDigest);

uint32_t tzbspApi_crypto_prng(
    const void  *out,
    uint32_t    outLen);

#endif // __MTK_API_TZBSP_API_WRAPPER_H__
