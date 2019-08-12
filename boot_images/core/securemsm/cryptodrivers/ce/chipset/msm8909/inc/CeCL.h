/*===========================================================================

                    Crypto Engine Core API

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR FILE
 
  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/securemsm/cryptodrivers/ce/chipset/msm8909/inc/CeCL.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/31/12   nk      Boot version
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

#define CECL_MAX_BAM_BLOCK_SIZE    (0x8000 - 64)

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

#define CECL_HASH_SHA_IV_LEN        20
#define CECL_HASH_SHA256_IV_LEN     32
#define CECL_HMAC_MAX_KEY_SIZE      16

typedef PACKED struct
{
  uint32                            hmac_key[CECL_HMAC_MAX_KEY_SIZE];
  uint32                            auth_iv[8];       
  uint32                            auth_bytecnt[2];  
  boolean                           firstBlock;
  boolean                           lastBlock;
  uint32                            dataLn;
  uint32                            seg_start;
  uint32                            seg_size;
  uint8                             auth_nonblock_mode;
  uint8                             auth_no_context;
  CeCLHashAlgoType                  algo;
  CeCLHashModeType                  mode;
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
#define CECL_AES_CMAC_MAC_LEN_8     0x2 // 8-byte MAC output

typedef enum 
{
  CECL_CIPHER_MODE_ECB              = 0x0,    
  CECL_CIPHER_MODE_CBC              = 0x1,    
  CECL_CIPHER_MODE_CTR              = 0x2,     
  CECL_CIPHER_MODE_XTS              = 0x3,     
  CECL_CIPHER_MODE_CCM              = 0x4,
  CECL_CIPHER_MODE_CMAC             = 0x5
}CeCLCipherModeType;

typedef enum 
{
  CECL_CIPHER_ENCRYPT               = 0x00, 
  CECL_CIPHER_DECRYPT               = 0x01 
} CeCLCipherDir;

typedef enum 
{
  CECL_CIPHER_ALG_AES128            = 0x0,
  CECL_CIPHER_ALG_AES256            = 0x1
}CeCLCipherAlgType;

typedef PACKED struct 
{
  CeCLCipherAlgType                 algo;
  uint32                            aes_key[CECL_AES_MAX_KEY_SIZE/4];
  uint32                            aes_xts_key[CECL_AES_MAX_KEY_SIZE/4];
  uint32                            xts_du_size;
  uint32                            iv[CECL_AES_MAX_IV_SIZE_BYTES/4];
  uint32                            auth_iv[4];
  uint32                            nonce[CECL_AES_NONCE_SIZE_BYTES/4];
  uint32                            nonceLn;
  CeCLCipherDir                     dir;
  CeCLCipherModeType                mode;
  boolean                           bAESUseHWKey;
  uint32                            dataLn;
}  CeCLCipherCntxType;

typedef PACKED struct
{
  uint8                             *pDataIn;
  uint32                            nDataLen; 
  uint8                             *pDataOut;
} CeCLCipherXferType;

typedef enum
{
  CECL_XFER_MODE_REG                = 0,
  CECL_XFER_MODE_BAM                = 1
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
CeCLErrorType CeCLIOCtlGetHashDigest(uint32 *digest_ptr, uint32 digest_len);


