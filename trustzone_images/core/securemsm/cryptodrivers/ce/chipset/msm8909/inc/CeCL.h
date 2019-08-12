/*===========================================================================

                    Crypto Engine Core API

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR FILE
 
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/cryptodrivers/ce/chipset/msm8909/inc/CeCL.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/09/12   yk      Modification for Crypto5
08/21/10   bm      added support for reg interface xfers
04/29/10   bm      initial version
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"

/*===========================================================================
                 DEFINITIONS AND TYPE DECLARATIONS
===========================================================================*/

typedef uint32 ce_ptr;

#define CECL_MAX_BAM_BLOCK_SIZE    (0x8000 - 64)
#define CECL_HASH_DIGEST_BLOCK_SIZE 64
#define CECL_AUTH_IV_COUNT          8

typedef enum
{
  CECL_CRYPTO3                      = 0x1,
  CECL_CRYPTO4                      = 0x2,
  CECL_CRYPTO5                      = 0x3
} CeCLCeVersionType;

typedef enum
{
  CECL_ERROR_SUCCESS                = 0x0,
  CECL_ERROR_FAILURE                = 0x1,
  CECL_ERROR_INVALID_PARAM          = 0x2,
  CECL_ERROR_NOT_SUPPORTED          = 0x3
} CeCLErrorType;

typedef enum
{
  CECL_IOCTL_HASH_VERSION_NUM       = 0x1,
  CECL_IOCTL_SET_HASH_CNTXT         = 0x2,
  CECL_IOCTL_GET_HASH_CNTXT         = 0x3,
  CECL_IOCTL_HASH_XFER              = 0x4
} CeCLIoCtlHashType;

typedef enum
{
  CECL_IOCTL_CIPHER_VERSION_NUM     = 0x1,
  CECL_IOCTL_SET_CIPHER_CNTXT       = 0x2,
  CECL_IOCTL_GET_CIPHER_CNTXT       = 0x3,
  CECL_IOCTL_CIPHER_XFER            = 0x4
} CeCLIoCtlCipherType;

typedef enum
{
  CECL_IOCTL_SET_HASH_CIPHER_CNTXT  = 0x1,
  CECL_IOCTL_GET_HASH_CIPHER_CNTXT  = 0x2
} CeCLIoCtlHashCipherType;

#define CECL_VERSION_NUM_SIZE       1 //byte

typedef enum
{
  CECL_HASH_ALGO_SHA1               = 0x1,
  CECL_HASH_ALGO_SHA256             = 0x2
} CeCLHashAlgoType;

typedef enum
{
  CECL_HASH_MODE_HASH               = 0x0, // Plain SHA
  CECL_HASH_MODE_HMAC               = 0x1  // HMAC SHA
} CeCLHashModeType;

typedef enum
{
  CECL_HASH_FIRST                   = 0, 
  CECL_HASH_LAST                    = 1  
} CeCLHashSeqType;

typedef enum
{
  CECL_CMAC_MODE_OFF               = 0,
  CECL_CMAC_MODE_128               = 1,
  CECL_CMAC_MODE_256               = 2
} CeClCmacModeType;

#define CECL_HASH_SHA_IV_LEN        20
#define CECL_HASH_SHA256_IV_LEN     32
#define CECL_HMAC_MAX_KEY_SIZE      16

typedef PACKED struct
{
  CeCLHashAlgoType                  algo;
  CeCLHashModeType                  mode;
  CeCLHashSeqType                   seq;
  CeClCmacModeType                  cmac_mode;
  uint32                            hmac_key[CECL_HMAC_MAX_KEY_SIZE];
  uint32                            auth_key[8];
  uint32                            auth_iv[CECL_AUTH_IV_COUNT];       
  uint32                            auth_bytecnt[2];  
  boolean                           firstBlock;
  boolean                           lastBlock;
  boolean                           bAESUseHWKey;
  uint32                            dataLn;
  uint32                            seg_start;
  uint32                            seg_size;
  uint32                            opad[CECL_HASH_DIGEST_BLOCK_SIZE/4];
} CeCLHashAlgoCntxType;

typedef PACKED struct
{
  uint8                             *buff_ptr;
  uint32                            buff_len;
} CeCLHashXferType;

#define CECL_AES_MAX_IV_SIZE_BYTES  32
#define CECL_AES_MAX_KEY_SIZE       32
#define CECL_AES128_KEY_SIZE        16
#define CECL_AES_NONCE_SIZE_BYTES   16
#define CECL_AES_CMAC_MAC_LEN_16    0xf // 16-byte MAC output 
#define CECL_AES_CCM_MAC_LEN_16     0xf // 16-byte MAC output 

typedef enum 
{
  CECL_CIPHER_MODE_ECB              = 0x0,    
  CECL_CIPHER_MODE_CBC              = 0x1,    
  CECL_CIPHER_MODE_CTR              = 0x2,     
  CECL_CIPHER_MODE_XTS              = 0x3,     
  CECL_CIPHER_MODE_CCM              = 0x4,
  CECL_CIPHER_MODE_CTS              = 0x5
}CeCLCipherModeType;

typedef enum 
{
  CECL_CIPHER_ENCRYPT               = 0x00, 
  CECL_CIPHER_DECRYPT               = 0x01 
} CeCLCipherDir;

typedef enum 
{
  CECL_CIPHER_ALG_AES128            = 0x0,
  CECL_CIPHER_ALG_AES256            = 0x1,
  CECL_CIPHER_ALG_TRIPLE_DES        = 0x2,
}CeCLCipherAlgType;

typedef enum
{
  CECL_CE_HW_OEM_KEY                = 0x0,  //SHK Key Type
  CECL_CE_HW_QC_KEY                 = 0x1   //PHK Key Type
} CeClCipherHWKeyType;

typedef PACKED struct 
{
  CeCLCipherAlgType                 algo;
  CeCLCipherModeType                mode;
  CeCLCipherDir                     dir;
  uint32                            aes_key[CECL_AES_MAX_KEY_SIZE/4];
  uint32                            aes_xts_key[CECL_AES_MAX_KEY_SIZE/4];
  uint32                            xts_du_size;
  uint32                            iv[CECL_AES_MAX_IV_SIZE_BYTES/4];
  uint32                            ccm_cntr[CECL_AES_MAX_IV_SIZE_BYTES/4];
  uint32                            nonce[CECL_AES_NONCE_SIZE_BYTES/4];
  uint32                            nonceLn;
  boolean                           firstBlock;
  boolean                           lastBlock;
  boolean                           bAESUseHWKey;
  CeClCipherHWKeyType               bAESUseHWKey_Type;
  uint32                            dataLn;
  uint32                            outdataLn;
  uint32                            seg_start;
  uint32                            seg_size;
  uint32                            payloadLn;
  uint32                            macLn;
  uint32                            hdrLn;
  uint32                            hdr_pad;
  uint32                            auth_bytecnt[2];
  uint32                            auth_iv[CECL_AUTH_IV_COUNT];       
}  CeCLCipherCntxType;

typedef PACKED struct
{
  uint8                             *pDataIn;
  uint32                            nDataLen; 
  uint8                             *pDataOut;
  uint32                            nDataOutLen; 
  uint8                             *pCntx;
} CeCLCipherXferType;

typedef enum
{
  CECL_XFER_MODE_REG          = 0x0,
  CECL_XFER_MODE_BAM          = 0x1,
  CECL_XFER_MODE_BAM_SHARING  = 0x2
}CeCLXferModeType;

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/

CeCLErrorType CeClClockEnable(void);
CeCLErrorType CeClClockDisable(void);
CeCLErrorType CeCLReverseBytes(uint8*, uint8*, uint32 byte_count);
CeCLErrorType CeClGetCeVersion(CeCLCeVersionType* CeEngine);

/*===========================================================================
*
* FUNCTION: CeClInit()
*
* DESCRIPTION:
*
* DEPENDENCIES:
* 
* RETURN VALUE: TRUE if successful and FALSE otherwise
*
===========================================================================*/
CeCLErrorType CeClInit(CeCLXferModeType);

/*===========================================================================
*
* FUNCTION: CeClDeinit()
*
* DESCRIPTION:
* 
* DEPENDENCIES:
* 
* RETURN VALUE: TRUE if successful and FALSE otherwise
*
===========================================================================*/
CeCLErrorType CeClDeinit(void);

/*===========================================================================
*
* FUNCTION: CeClReset()
*
* DESCRIPTION:
* This function resets the crypto core in the given environment
* 
* DEPENDENCIES:
* 
* RETURN VALUE: TRUE if successful and FALSE otherwise
*
===========================================================================*/
CeCLErrorType CeClReset(void);

/*===========================================================================
*
* FUNCTION: CeClIOCtl()
*
* DESCRIPTION:
* 
* DEPENDENCIES:
* 
* RETURN VALUE:
*
===========================================================================*/             
CeCLErrorType CeClIOCtlHash(CeCLIoCtlHashType   ioCtlVal,
                        uint8*          pBufIn, 
                        uint32          dwLenIn, 
                        uint8*          pBufOut, 
                        uint32          dwLenOut, 
                        uint32*         pdwActualOut);

/*===========================================================================
*
* FUNCTION: CeClIOCtl()
*
* DESCRIPTION:
* 
* DEPENDENCIES:
* 
* RETURN VALUE:
*
===========================================================================*/             
CeCLErrorType CeClIOCtlCipher(CeCLIoCtlCipherType   ioCtlVal,
                        uint8*          pBufIn, 
                        uint32          dwLenIn, 
                        uint8*          pBufOut, 
                        uint32          dwLenOut, 
                        uint32*         pdwActualOut);

/*===========================================================================
*
* FUNCTION: CeClIOCtl()
*
* DESCRIPTION:
* 
* DEPENDENCIES:
* 
* RETURN VALUE:
*
===========================================================================*/             
CeCLErrorType CeClIOCtlHashOperation(uint8 *ctx, uint8 *datain_ptr, 
                                     uint32 nDataLen, uint8 *dataout_ptr);

/*===========================================================================
*
* FUNCTION: CeClIOCtl()
*
* DESCRIPTION:
* 
* DEPENDENCIES:
* 
* RETURN VALUE:
*
===========================================================================*/             
CeCLErrorType CeClIOCtlCipherOperation(uint8 *ctx, uint8 *datain_ptr, 
                                       uint32 nDataLen, uint8 *dataout_ptr);

/*===========================================================================
*
* FUNCTION: CeClIOCtl()
*
* DESCRIPTION:
* 
* DEPENDENCIES:
* 
* RETURN VALUE:
*
===========================================================================*/             

CeCLErrorType CeClIOCtlHashCipherOperation(uint8 *hash_ctx, uint8 *cipher_ctx, uint8 *datain_ptr, 
                                           uint32 nDataLen, uint8 *dataout_ptr);

/*===========================================================================
*
* FUNCTION: CeClIOCtl()
*
* DESCRIPTION:
* 
* DEPENDENCIES:
* 
* RETURN VALUE:
*
===========================================================================*/             
CeCLErrorType CeCLIOCtlGetHashDigest(uint32 *digest_ptr, uint32 digest_len);

/*===========================================================================
*
* FUNCTION: CeClIOCtlHashCipher()
*
* DESCRIPTION:
* 
* DEPENDENCIES:
* 
* RETURN VALUE:
*
===========================================================================*/             
CeCLErrorType CeClIOCtlHashCipher (CeCLIoCtlHashCipherType ioCtlVal, 
                                   uint8* pHashBufIn, 
                                   uint8* pCipherBufIn,                          
                                   uint8* pHashBufOut,
                                   uint8* pCipherBufOut);

/*===========================================================================
*
* FUNCTION: CeCLAppsDebugBlown()
*
* DESCRIPTION: Checks if the debug fuses for APPS has been blown
* 
* DEPENDENCIES:
* 
* RETURN VALUE: CeCL_Error_Success if blown else CeCL_Error_Failure
*
===========================================================================*/             
#ifndef USES_NO_CP
CeCLErrorType CeCLAppsDebugBlown(void);
#endif
