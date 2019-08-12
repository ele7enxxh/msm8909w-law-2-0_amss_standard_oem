/*****************************************************************************
* Copyright (c) 2013,2015 Qualcomm Technologies, Inc.  All Rights Reserved.       *
* Qualcomm Technologies Proprietary and Confidential.                        *
*****************************************************************************/

#ifndef __QCRYPT_H__
#define __QCRYPT_H__

#include "QICommon.h"

extern "C" {
#include <stdlib.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/pem.h>
}


#ifdef BORING_SSL
inline int EVP_OpenInit(EVP_CIPHER_CTX *ctx,const EVP_CIPHER *type,
                        const unsigned char *ek, int ekl,
                        const unsigned char *iv, EVP_PKEY *priv)
{
  return 0;
}

inline int EVP_OpenFinal(EVP_CIPHER_CTX *ctx, unsigned char *out,
                         int *outl)
{
  return 0;
}

inline int EVP_SealInit(EVP_CIPHER_CTX *ctx, const EVP_CIPHER *type,
                        unsigned char **ek, int *ekl, unsigned char *iv,
                        EVP_PKEY **pubk, int npubk)
{
  return 0;
}

inline int EVP_SealFinal(EVP_CIPHER_CTX *ctx,unsigned char *out,
                         int *outl)
{
  return 0;
}

inline int EVP_OpenUpdate(EVP_CIPHER_CTX *ctx, unsigned char *out,
                          int *outl, const unsigned char *in, int inl)
{
  return 0;
}

inline int EVP_SealUpdate(EVP_CIPHER_CTX *ctx, unsigned char *out,
                          int *outl, const unsigned char *in, int inl)
{
  return 0;
}
#endif


/*===========================================================================
 * Class: QCrypt
 *
 * Description: This class represents an asymmetric key encryption engine
 *
 * Notes: none
 *==========================================================================*/
class QCrypt {

public:

  /** New:
   * static factory method
   */
  static QCrypt *New();

  /** ~QCrypt:
   *
   *  QCrypt class destructor
   **/
  virtual ~QCrypt();

  /** setEncKey:
   * @encKey - encryption key.
   *
   * Set the encryption key
   */
  virtual int setEncKey(char *aEncKey, uint32_t aLen);

  /** setDecKey:
   * @decKey - decryption key.
   *
   * Set the decryption key
   */
  virtual int setDecKey(char *aDecKey, uint32_t aLen);

  /** setEncMsgLen:
   * @enc_len - message len
   *
   * Set length of message to be encrypted.
   *
   */
  virtual uint32_t setEncMsgLen(uint32_t aEncLen);

  /** setDecMsgLen:
   * @dec_len - message len
   *
   * Set length of message to be encrypted.
   *
   */
  virtual void setDecMsgLen(uint32_t aDecLen);

  /** encrypt:
   * @src - Source message buffer
   * @dst - Destination buffer
   *
   * Encrypts message. Returns total encrypted message length.
   *
   */
  virtual uint32_t encrypt(uint8_t *aSrc, uint8_t *aDst);

  /** decrypt:
   * @src - Source message buffer
   * @dst - Destination buffer
   *
   * Decrypts message. Returns total decrypted message length.
   *
   */
  virtual uint32_t decrypt(uint8_t *aSrc, uint8_t *aDst);


private:

  /** enc_msg_hdr_t:
   *
   *  encrypted message header
   **/
  typedef struct {
    uint32_t msg_len;
    uint32_t msg_offset;
    uint32_t ek_len;
    uint32_t ek_offset;
    uint32_t iv_len;
    uint32_t iv_offset;
  } enc_msg_hdr_t;

  /** mKeyPair:
   *
   *  Encryption keypair
   **/
  EVP_PKEY *mKeyPair;


  /** mRsaEncCtx:
   *
   *  Encryption context
   **/
  EVP_CIPHER_CTX *mRsaEncCtx;

  /** mEncMsgLen:
   *
   *  Length of message to be encrypted
   **/
  uint32_t mEncMsgLen;

  /** mDecMsgLen:
   *
   *  Length of message to be decrypted
   **/
  uint32_t mDecMsgLen;

  /** mEkLen:
   *
   *  Length of encrypted key
   **/
  uint32_t mEkLen;

  /** QCrypt:
   *
   *  QCrypt class contructor
   **/
  QCrypt();

  /** Init:
   *
   *  Initialisation method
   **/
  virtual int Init();

  /** headerSize:
   *
   *  Return size of header before start of encrypted message
   **/
  virtual uint32_t headerSize();

  /** encParamsOK:
   *
   *  Validate encryption parameters
   **/
  virtual bool encParamsOK();

  /** decParamsOK:
   *
   *  Validate decryption parameters
   **/
  virtual bool decParamsOK();

  /** hdrValid:
   *
   *  Validate header of encrypted message
   **/
  virtual bool hdrValid(enc_msg_hdr_t *aHdr);

};

#endif
