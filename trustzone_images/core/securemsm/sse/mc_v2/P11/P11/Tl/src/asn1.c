/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*
--
-- Representation of RSA private key with information for the CRT algorithm.
--
RSAPrivateKey ::= SEQUENCE {
    version           Version,
    modulus           INTEGER,  -- n
    publicExponent    INTEGER,  -- e
    privateExponent   INTEGER,  -- d
    prime1            INTEGER,  -- p
    prime2            INTEGER,  -- q
    exponent1         INTEGER,  -- d mod (p-1)
    exponent2         INTEGER,  -- d mod (q-1)
    coefficient       INTEGER,  -- (inverse of q) mod p
    >>> NOT SUPPORTED otherPrimeInfos   OtherPrimeInfos OPTIONAL
}

RSAPublicKey ::= SEQUENCE {
    modulus           INTEGER,  -- n
    publicExponent    INTEGER   -- e
}

PrivateKeyInfo ::= SEQUENCE {
   version Version,
   privateKeyAlgorithm PrivateKeyAlgorithmIdentifier,
   privateKey PrivateKey
   >>> NOT SUPPORTED attributes [0] IMPLICIT Attributes OPTIONAL }

AlgorithmIdentifier  ::=  SEQUENCE  {
  algorithm               OBJECT IDENTIFIER,
  parameters              ANY DEFINED BY algorithm OPTIONAL  }
*/

//#define TL_LOG_FILE

#include "stringl.h"
#include "asn1.h"
#include "tlLog.h"
#include "tlP11TypesInt.h"

// ASN tags we use
#define ASN_TAG_INTEGER       0x02
#define ASN_TAG_OCTECTSTRING  0x04
#define ASN_TAG_SEQUENCE      0x30


static tlApiResult_t DER_decode_dlength(
    const uint8_t tag,
    const uint8_t *src,
    const uint32_t src_length,
    uint8_t *dst,
    uint32_t *dst_len,
    uint32_t *field_len,
    bool_t trim)
{
  tlApiResult_t rv = TLAPI_OK;
  uint32_t len = 0;
  uint32_t prefix_len = 2;
  TL_LOG_I("+DER_decode_dlength");
  do {
    // input parameters are supposed to have been sanitised already
    // src_len is known to be > 3
    // check that the value type corresponds to what we've been asked to decode
    if (src[0] != tag) {
      TL_LOG_E("Invalid TAG: %02X, expecting: %02X",src[0],tag);
      rv = E_TLAPI_INVALID_INPUT;
      break;
    }

    // get length
    // for PKCS11 purposes, no sequences (RSA Public or Private keys) can be
    // longer than what we can represent with 1+2 bytes for the length, i.e.
    // 82 FF FF -> length=FFFF=65535
    if (src[1] < 128) {
      // short form
      len=src[1];
      prefix_len=2;
    } else {
      //long form
      if ((src[1] & 0x7F) == 1) {
        if (src_length < 3) {
          TL_LOG_E("Invalid input, too short or malformed!");
          rv = E_TLAPI_CR_INCONSISTENT_DATA;
          break;
        }
        len = src[2];
        prefix_len=3;
      } else if ((src[1] & 0x7F) == 2) {
        if (src_length < 4) {
          TL_LOG_E("Invalid input, too short or malformed!");
          rv = E_TLAPI_CR_INCONSISTENT_DATA;
          break;
        }
        len = (src[2] << 8) + src[3];
        prefix_len=4;
      } else {
        // NOT SUPPORTED!
        TL_LOG_I(" Invalid/Unsupported length for INTEGER: %02X",src[1]);
      }
    }
    if (src_length < prefix_len+len) {
      TL_LOG_E("Invalid input, too short or malformed!");
      rv = E_TLAPI_CR_INCONSISTENT_DATA;
      break;
    }
    if (!trim) {
      if (*dst_len < len) {
        TL_LOG_E("Output buffer size too small. Have %d, want %d bytes",*dst_len,len);
        rv = E_TLAPI_BUFFER_TOO_SMALL;
        break;
      }
      // set output
      *dst_len = len;
      if (dst)
        memscpy(dst,len,src+prefix_len,len);
    } else {
      // we try to trim it while reading it
      uint32_t i=0;
      const uint8_t* data=src+prefix_len;
      for(i=0; i<len; i++) {
        if (data[i] != 0)
          break;
      }
      // data[i] is the first != 0
      if (*dst_len < (len-i)) {
        TL_LOG_E("Output buffer size too small, even with trimming. Have %d, want %d bytes",*dst_len,len-i);
        rv = E_TLAPI_BUFFER_TOO_SMALL;
        break;
      }
      // set output
      *dst_len = len-i;
      if (dst)
        memscpy(dst,len-i,data+i,len-i);
    }
    *field_len = prefix_len + len;
  } while(0);
  TL_LOG_I("-DER_decode_dlength: 0x%08X. Length: %d bytes",rv,len);
  return rv;
}

static tlApiResult_t DER_decode_INTEGER(
    const uint8_t *src,
    const uint32_t src_length,
    uint8_t *dst,
    uint32_t *dst_len,
    uint32_t *field_len)
{
  tlApiResult_t rv = TLAPI_OK;

  TL_LOG_I("+DER_decode_INTEGER");
  do {
/*
    TL_LOG_I("src=0x%08X", src);
    TL_LOG_I("src_length=%d", src_length);
    TL_LOG_I("dst=0x%08X", dst);
    TL_LOG_I("dst_len=0x%08X", dst_len);
    TL_LOG_I("*dst_len=%d", *dst_len);
    TL_LOG_I("field_len=0x%08X", field_len);
        */
    if (!src || !dst_len || !field_len || (*dst_len == 0) || (src_length < 2)) {
      TL_LOG_E("Invalid argument");
      rv = E_TLAPI_INVALID_PARAMETER;
      break;
    }
    rv = DER_decode_dlength(ASN_TAG_INTEGER,src,src_length,dst,dst_len,field_len,TRUE);
    TL_LOG_I("DER_decode_INTEGER. Length: %d bytes",*dst_len);

  }while (0);
  TL_LOG_I("-DER_decode_INTEGER: 0x%08X",rv);
  return rv;
}

static tlApiResult_t DER_decode_SEQUENCE(
    const uint8_t *src,
    const uint32_t src_length,
    uint32_t *field_len,
    uint32_t *prefix_len)
{
  tlApiResult_t rv = TLAPI_OK;
  uint32_t payload_len=0x10000;

  TL_LOG_I("+DER_decode_SEQUENCE");
  do {
    if (!src || !field_len || (src_length < 3)) {
      TL_LOG_E("Invalid argument");
      rv = E_TLAPI_INVALID_PARAMETER;
      break;
    }
    rv = DER_decode_dlength(ASN_TAG_SEQUENCE,src,src_length,NULL,&payload_len,field_len,FALSE);
    if (prefix_len)
      *prefix_len = *field_len - payload_len;

  }while (0);
  TL_LOG_I("-DER_decode_SEQUENCE: 0x%08X. Length: %d bytes",rv,payload_len);
  return rv;
}

static tlApiResult_t DER_decode_OCTETSTRING(
    const uint8_t *src,
    const uint32_t src_length,
    uint32_t *field_len,
    uint32_t *prefix_len)
{
  tlApiResult_t rv = TLAPI_OK;
  uint32_t payload_len=0x10000;

  TL_LOG_I("+DER_decode_OCTETSTRING");
  do {
    if (!src || !field_len || (src_length < 3)) {
      TL_LOG_E("Invalid argument");
      rv = E_TLAPI_INVALID_PARAMETER;
      break;
    }
    rv = DER_decode_dlength(ASN_TAG_OCTECTSTRING,src,src_length,NULL,&payload_len,field_len,FALSE);
    if (prefix_len)
      *prefix_len = *field_len - payload_len;

  }while (0);
  TL_LOG_I("-DER_decode_OCTETSTRING: 0x%08X. Length: %d bytes",rv,payload_len);
  return rv;
}

tlApiResult_t DER_decode_RSAPublicKey(
    const uint8_t *src,
    const uint32_t src_length,
    uint8_t *public_exponent,
    uint32_t *public_exponent_len,
    uint8_t *modulus,
    uint32_t *modulus_len)
{
  tlApiResult_t rv = TLAPI_OK;
  uint32_t fieldLen = 0;
  uint32_t sequencePrefixLen = 0;
  uint32_t offset = 0;

  TL_LOG_I("+DER_decode_RSAPublicKey. Length of blob: %d",src_length);
  do {
    // check input
    if (!src || !public_exponent || !modulus || !modulus_len ||
        (*modulus_len < MODULUS_MAX_LEN)) {
      TL_LOG_E("Invalid argument");
      rv = E_TLAPI_INVALID_PARAMETER;
      break;
    }
    // sequence of 2 integers: modulus + exponent
    rv = DER_decode_SEQUENCE(src,src_length,&fieldLen,&sequencePrefixLen);
    if (rv != TLAPI_OK) break;

    offset += sequencePrefixLen;
    // decode the modulus
    rv = DER_decode_INTEGER(
        src+offset,
        src_length-offset,
        modulus,
        modulus_len,
        &fieldLen);
    if (rv != TLAPI_OK) break;

    offset += fieldLen;
    // decode the public exponent
    rv = DER_decode_INTEGER(
        src+offset,
        src_length-offset,
        public_exponent,
        public_exponent_len,
        &fieldLen);
    if (rv != TLAPI_OK) break;
    // done
    TL_LOG_I("-DER_decode_RSAPublicKey. PublicExponent: %02X %02X %02X %02X, len: %d, Modulus length: %d",
          public_exponent[0],public_exponent[1],public_exponent[2],public_exponent[3],
          *public_exponent_len,
          *modulus_len);

  } while(0);

  TL_LOG_I("-DER_decode_RSAPublicKey: 0x%08X.",rv);
  return rv;
}

tlApiResult_t DER_decode_PrivateKeyInfo(
    const uint8_t *src,
    const uint32_t src_length,
    tlp11_rsa_private_key_t *key
)
{
  tlApiResult_t rv = TLAPI_OK;
  uint32_t fieldLen = 0;
  uint32_t sequencePrefixLen = 0;
  uint32_t offset = 0;
  uint8_t version[4];
  uint32_t versionLen = sizeof(version);
  CK_BYTE  algIdRSAEncryption[] = {
    0x30, 0x0D,
      0x06, 0x09,
        0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01,
      0x05, 0x00 };
  struct {
    uint8_t   *dst;
    uint32_t  *dst_len;
  } elements[]= {
      {version, &versionLen},
      {key->modulus, &key->modulus_len},
      {key->public_exponent, &key->public_exponent_len},
      {key->exponent, &key->exponent_len},
      {key->prime1, &key->prime1_len},
      {key->prime2, &key->prime2_len,},
      {key->exponent1, &key->exponent1_len,},
      {key->exponent2, &key->exponent2_len,},
      {key->coefficient, &key->coefficient_len}
  };


  TL_LOG_I("+DER_decode_PrivateKeyInfo. Length of blob: %d",src_length);
  do {
    // check input
    if (!src || !key) {
      TL_LOG_E("Invalid argument");
      rv = E_TLAPI_INVALID_PARAMETER;
      break;
    }
    // sequence of (version,algId,privateKey)
    rv = DER_decode_SEQUENCE(src,src_length,&fieldLen,&sequencePrefixLen);
    if (rv != TLAPI_OK) break;
    offset += sequencePrefixLen;

    // version, skip
    rv = DER_decode_INTEGER(
        src+offset,
        src_length-offset,
        version,
        &versionLen,
        &fieldLen);
    if (rv != TLAPI_OK) break;
    offset += fieldLen;

    // trick for algId: we know beforehand what it looks like, so we just memcmp
    if (src_length-offset < sizeof(algIdRSAEncryption)) {
      TL_LOG_E("Invalid argument: buffer too short, cannot match algIdRSAEncryption");
      rv = E_TLAPI_INVALID_PARAMETER;
      break;
    }
    if (0 != memcmp(src+offset,algIdRSAEncryption,sizeof(algIdRSAEncryption))) {
      TL_LOG_E("Invalid argument: cannot match algIdRSAEncryption");
      rv = E_TLAPI_INVALID_PARAMETER;
      break;
    }
    offset += sizeof(algIdRSAEncryption);

    // now we have an OCTECT STRING whose content is the sequence representing
    // the private key
    rv = DER_decode_OCTETSTRING(src+offset,src_length-offset,&fieldLen,&sequencePrefixLen);
    if (rv != TLAPI_OK) break;
    offset += sequencePrefixLen;

    // now decode the sequence prefix
    rv = DER_decode_SEQUENCE(src+offset,src_length-offset,&fieldLen,&sequencePrefixLen);
    if (rv != TLAPI_OK) break;
    offset += sequencePrefixLen;

    // version, skip
    versionLen=sizeof(version);
    for (unsigned int i=0; i<LENGTH(elements); i++) {
      rv = DER_decode_INTEGER(
        src+offset,
        src_length-offset,
        elements[i].dst,
        elements[i].dst_len,
        &fieldLen);
      if (rv != TLAPI_OK) break;
      offset += fieldLen;
    }

    // done
    TL_LOG_I("DER_decode_PrivateKeyInfo. PublicExponent: %02X %02X %02X %02X, len: %d, Modulus length: %d, PublicExponent length: %d",
          key->public_exponent[0],key->public_exponent[1],key->public_exponent[2],key->public_exponent[3],
          key->public_exponent_len,
          key->modulus_len,
          key->exponent_len);

  } while(0);

  TL_LOG_I("-DER_decode_PrivateKeyInfo: 0x%08X.",rv);
  return rv;
}

static tlApiResult_t DER_encode_dlength(
    const uint8_t tag,
    const uint8_t *src,
    const uint32_t src_len,
    uint8_t *dst,
    uint32_t *dst_len,
    uint32_t *prefix_len,
    bool_t integer)
{
  tlApiResult_t rv = TLAPI_OK;
  uint32_t len = 0;
  uint32_t trail = 0;
  uint32_t data_len = 0;
  TL_LOG_I("+DER_encode_dlength");
  do {
    // input parameters are supposed to have been sanitised already

    // do we need to add a trailing zero?
    if (integer && src && (src[0] & 0x80)) {
      TL_LOG_I("trail=1");
      trail = 1;
    }

    data_len = src_len + trail;

    //determine how many bytes we need to represents length
    if (data_len < 128) {
      len = 1+1 + data_len;
    } else if (data_len < 256) {
      len = 1+(1+1) + data_len;
    } else if (data_len < (1 << 16)) {
      len = 1+(1+2) + data_len;
    } else {
      TL_LOG_I(" Input buffer too long! %d",src_len);
      rv = E_TLAPI_INVALID_INPUT;
      break;
    }

    if ((*dst_len < len) && dst) {
      TL_LOG_I(" Output buffer too small! %d vs %d",src_len,*dst_len);
      rv = E_TLAPI_BUFFER_TOO_SMALL;
      break;
    }
    *prefix_len = len - data_len;
    *dst_len = len;

    if (dst) {

      // now write the content
      dst[0]=tag;
      dst++;

      // write length
      if (data_len < 128) {
        dst[0]=(uint8_t)data_len;
        dst += 1;
      } else if (data_len < 256) {
        dst[0] = 0x81;
        dst[1] = (uint8_t)data_len;
        dst += 2;
      } else /* if (data_len < (1 << 16)) */ {
        dst[0] = 0x82;
        dst[1] = (data_len >> 8) & 0xFF;
        dst[2] = (data_len) & 0xFF;
        dst += 3;
      }
      if (trail) {
        dst[0]=0x0;
        dst++;
      }
      // now copy the data itself
      // there are cases, e.g. SEQUENCE or OCTETSTRING when we want to write
      // only the prefix and not the data
      if (src) {
        memscpy(dst,len,src,src_len);
      }
    }
  } while(0);
  TL_LOG_I("-DER_encode_dlength: 0x%08X. Length: %d bytes",rv,len);
  return rv;
}

static tlApiResult_t DER_encode_INTEGER(
    const uint8_t *src,
    const uint32_t src_len,
    uint8_t *dst,
    uint32_t *dst_len,
    uint32_t *prefix_len)
{
  tlApiResult_t rv = TLAPI_OK;
  TL_LOG_I("+DER_encode_INTEGER");
  do {
    if (!src || !dst_len) {
      TL_LOG_E("Invalid argument");
      rv = E_TLAPI_INVALID_PARAMETER;
      break;
    }
    rv = DER_encode_dlength(ASN_TAG_INTEGER,src,src_len,dst,dst_len,prefix_len,TRUE);
  } while(0);
  TL_LOG_I("-DER_encode_INTEGER: 0x%08X",rv);
  return rv;
}

static tlApiResult_t DER_encode_SEQUENCE(
    const uint32_t src_len,
    uint8_t *dst,
    uint32_t *dst_len,
    uint32_t *prefix_len)
{
  tlApiResult_t rv = TLAPI_OK;
  TL_LOG_I("+DER_encode_SEQUENCE");
  do {
    if (!prefix_len || !dst_len) {
      TL_LOG_E("Invalid argument");
      rv = E_TLAPI_INVALID_PARAMETER;
      break;
    }
    rv = DER_encode_dlength(ASN_TAG_SEQUENCE,NULL,src_len,dst,dst_len,prefix_len,FALSE);
  } while(0);
  TL_LOG_I("-DER_encode_SEQUENCE: 0x%08X",rv);
  return rv;
}

static tlApiResult_t DER_encode_OCTETSTRING(
    const uint32_t src_len,
    uint8_t *dst,
    uint32_t *dst_len,
    uint32_t *prefix_len)
{
  tlApiResult_t rv = TLAPI_OK;
  TL_LOG_I("+DER_encode_OCTETSTRING");
  do {
    if (!prefix_len || !dst_len) {
      TL_LOG_E("Invalid argument");
      rv = E_TLAPI_INVALID_PARAMETER;
      break;
    }
    rv = DER_encode_dlength(ASN_TAG_OCTECTSTRING,NULL,src_len,dst,dst_len,prefix_len,FALSE);
  } while(0);
  TL_LOG_I("-DER_encode_OCTETSTRING: 0x%08X",rv);
  return rv;
}

tlApiResult_t DER_encode_PrivateKeyInfo(
    const tlp11_rsa_private_key_t *key,
    uint8_t *dst,
    size_t *dst_len
)
{
  tlApiResult_t rv = TLAPI_OK;
  uint32_t fieldLen = 0;
  uint32_t prefixLen = 0;
  uint32_t offset = 0;
  uint8_t version[] = {0x02, 0x01, 0x00}; // pre-encoded
  uint32_t rsaPrivateKey_len = 0;
  uint32_t rsaPrivateKey_seq_len = 0;
  uint32_t octetString_len = 0;
  CK_BYTE  algIdRSAEncryption[] = {
    0x30, 0x0D,
      0x06, 0x09,
        0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01,
      0x05, 0x00 };
  uint32_t externalSequenceContent_len = 0;
  uint32_t externalSequence_len = 0;
  uint32_t out_len = 0;
  unsigned int i = 0;

  TL_LOG_I("+DER_encode_PrivateKeyInfo");
  do {
    // check input
    if (!dst_len || !key
        ) {
      TL_LOG_E("Invalid argument");
      rv = E_TLAPI_INVALID_PARAMETER;
      break;
    }
    {
      struct {
        const uint8_t   *src;
        const uint32_t  src_len;
      } elements[]= {
          {version, sizeof(version)},
          {key->modulus, key->modulus_len},
          {key->public_exponent, key->public_exponent_len},
          {key->exponent, key->exponent_len},
          {key->prime1, key->prime1_len},
          {key->prime2, key->prime2_len,},
          {key->exponent1, key->exponent1_len,},
          {key->exponent2, key->exponent2_len,},
          {key->coefficient, key->coefficient_len}
      };

      // first we need to find out the length of all components to set the length
      // of sequences and octet string

      for (i=0; i<LENGTH(elements); i++) {
        rv = DER_encode_INTEGER(
          elements[i].src,
          elements[i].src_len,
          NULL,
          &fieldLen,
          &prefixLen);
        if (rv != TLAPI_OK) break;
        rsaPrivateKey_len += fieldLen;
      }

      rv = DER_encode_SEQUENCE(
          rsaPrivateKey_len,
          NULL,
          &rsaPrivateKey_seq_len,
          &prefixLen);
      if (rv != TLAPI_OK) break;

      rv = DER_encode_OCTETSTRING(
          rsaPrivateKey_seq_len,
          NULL,
          &octetString_len,
          &prefixLen);
      if (rv != TLAPI_OK) break;

      externalSequenceContent_len = sizeof(version) + octetString_len + sizeof(algIdRSAEncryption);

      // add the length of the preamble
      rv = DER_encode_SEQUENCE(
          externalSequenceContent_len,
          NULL,
          &externalSequence_len,
          &prefixLen);
      if (rv != TLAPI_OK) break;

      // now we have the full length, and we can check it against what was passed in
      if (*dst_len < externalSequence_len) {
       TL_LOG_E("Output buffer too small: %d vs %d",*dst_len,externalSequence_len);
        rv = E_TLAPI_INVALID_PARAMETER;
        break;
      }
      TL_LOG_I("Overall size needed: %d bytes",externalSequence_len);

      out_len = *dst_len;

      // now walk back in order and fill the fields
      TL_LOG_I("offset: %d",offset);
      rv = DER_encode_SEQUENCE(
          externalSequenceContent_len,
          dst,
          &out_len,
          &prefixLen);
      if (rv != TLAPI_OK) break;
      offset = prefixLen;
      out_len = *dst_len - offset;
      TL_LOG_I("offset: %d",offset);

      memscpy(dst+offset,out_len,version,sizeof(version));
      offset += sizeof(version);
      out_len = *dst_len - offset;
      TL_LOG_I("offset: %d",offset);

      // now copy alg
      memscpy(dst+offset,out_len,algIdRSAEncryption,sizeof(algIdRSAEncryption));
      offset += sizeof(algIdRSAEncryption);
      out_len = *dst_len - offset;
      TL_LOG_I("offset: %d",offset);

      // octet string
      rv = DER_encode_OCTETSTRING(
          rsaPrivateKey_seq_len,
          dst+offset,
          &out_len,
          &prefixLen);
      if (rv != TLAPI_OK) break;
      offset += prefixLen;
      out_len = *dst_len - offset;
      TL_LOG_I("offset: %d",offset);

      rv = DER_encode_SEQUENCE(
          rsaPrivateKey_len,
          dst+offset,
          &out_len,
          &prefixLen);
      if (rv != TLAPI_OK) break;
      offset += prefixLen;
      out_len = *dst_len - offset;
      TL_LOG_I("offset: %d",offset);

      // start encoding our elements
      for (i=0; i<LENGTH(elements); i++) {
        rv = DER_encode_INTEGER(
          elements[i].src,
          elements[i].src_len,
          dst+offset,
          &out_len,
          &prefixLen);
        if (rv != TLAPI_OK) break;
        offset += out_len;
        out_len = *dst_len - offset;
        TL_LOG_I("offset: %d",offset);
      }
      *dst_len = offset;
    }
    // done
  } while(0);

  TL_LOG_I("-DER_encode_PrivateKeyInfo: 0x%08X, length=%d", rv,(dst_len)?(*dst_len):0);
  return rv;
}
