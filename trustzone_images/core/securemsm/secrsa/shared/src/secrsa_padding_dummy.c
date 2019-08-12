/*=========================================================================== 
   @file secrsa_padding.c 
   Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
  
   This file provides PKCS1 v1.5 & v2.1padding encode and decode implementation.
  ===========================================================================*/
/*===========================================================================

                           EDIT HISTORY FOR FILE
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/secrsa/shared/src/secrsa_padding_dummy.c#1 $
  $DateTime: 2016/06/17 14:31:11 $ 
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  04/11/11   qxu     initial version
=============================================================================*/

/*===========================================================================

                             INCLUDE FILES

===========================================================================*/
#include "secrsa.h"
#include "secrsa_env.h"
#include "secrsa_mem.h"
/*===========================================================================

                             MACRO DEFINITIONS

===========================================================================*/
#define MAX_DER_HEADER_LEN   32

/** hash descriptor */
typedef struct PKCS1_DER_ENC_DESC_
{
   /** name of hash */
   char *name;
   /** Size of digest in octets */
   unsigned long hashsize;
   /** Input block size in octets */
   unsigned long blocksize;
   /** ASN.1 OID */
   unsigned long OID[16];
   /** Length of OID */
   unsigned long OIDlen;
   /**DER encoding of the DigestInfo header **/
   unsigned char DERheader[MAX_DER_HEADER_LEN];
   /** DigestInfo header Length of DER encoding */
   unsigned long DERheaderlen;
} PKCS1_DER_ENC_DESC;

#if 0
PKCS1_DER_ENC_DESC DER_enc_hash[CE_HASH_IDX_MAX] = { 
   {
      0// "", 0, 0, 0, {0}, 0, {0}, 0
   },
   {
      0// "", 0, 0, 0, {0}, 0, {0}, 0
   },
   {
      "sha1",
      20,
      64,
      /* OID */
      { 1, 3, 14, 3, 2, 26,},
      6,
      {0x30, 0x21, 0x30, 0x09, 0x06, 0x05, 0x2b, 0x0e, 0x03, 0x02, 0x1a, 0x05, 0x00, 0x04, 0x14,},
      15,
   },
   {
      "sha256",
      32,
      64,
      /* OID */
      { 2, 16, 840, 1, 101, 3, 4, 2, 1,},
      9,
      {0x30, 0x31, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01, 0x05, 0x00, 0x04, 0x20,},
      19
   },
   {
      "sha384",
      48,
      128,
      /* OID */
      { 2, 16, 840, 1, 101, 3, 4, 2, 2,},
      9,
      {0x30, 0x41, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x02, 0x05, 0x00, 0x04, 0x30,},
      19
   },
   {
      "sha512",
      64,
      128,
      /* OID */
      { 2, 16, 840, 1, 101, 3, 4, 2, 3,},
      9,
      {0x30, 0x51, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x03, 0x05, 0x00, 0x04, 0x40,},
      19
   }
};

unsigned char lhash_0_sha1[] = {
   0xda, 0x39, 0xa3, 0xee, 0x5e, 0x6b, 0x4b, 0x0d,
   0x32, 0x55, 0xbf, 0xef, 0x95, 0x60, 0x18, 0x90, 
   0xaf, 0xd8, 0x07, 0x09
};

unsigned char lhash_0_sha256[] = {
   0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14,
   0x9a, 0xfb, 0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24, 
   0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c, 
   0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55
};

#if 0 
/* For future support */
unsigned char lhash_0_sha384[] = {
   0x38, 0xb0, 0x60, 0xa7, 0x51, 0xac, 0x96, 0x38,
   0x4c, 0xd9, 0x32, 0x7e, 0xb1, 0xb1, 0xe3, 0x6a,
   0x21, 0xfd, 0xb7, 0x11, 0x14, 0xbe, 0x07, 0x43,
   0x4c, 0x0c, 0xc7, 0xbf, 0x63, 0xf6, 0xe1, 0xda,
   0x27, 0x4e, 0xde, 0xbf, 0xe7, 0x6f, 0x65, 0xfb,
   0xd5, 0x1a, 0xd2, 0xf1, 0x48, 0x98, 0xb9, 0x5b
};

unsigned char lhash_0_sha512[] = {
   0xcf, 0x83, 0xe1, 0x35, 0x7e, 0xef, 0xb8, 0xbd,
   0xf1, 0x54, 0x28, 0x50, 0xd6, 0x6d, 0x80, 0x07,
   0xd6, 0x20, 0xe4, 0x05, 0x0b, 0x57, 0x15, 0xdc,
   0x83, 0xf4, 0xa9, 0x21, 0xd3, 0x6c, 0xe9, 0xce,
   0x47, 0xd0, 0xd1, 0x3c, 0x5d, 0x85, 0xf2, 0xb0,
   0xff, 0x83, 0x18, 0xd2, 0x87, 0x7e, 0xec, 0x2f,
   0x63, 0xb9, 0x31, 0xbd, 0x47, 0x41, 0x7a, 0x81,
   0xa5, 0x38, 0x32, 0x7a, 0xf9, 0x27, 0xda, 0x3e
};
#endif /* For future support */
#endif // #if 0
/*===========================================================================

                            FUNCTION DEFINITIONS

===========================================================================*/

/** 
  PKCS #1 v1.5 encode.
 
  @param[in]  msg              The data to encode
  @param[in]  msglen           The length of the data to encode (octets)
  @param[in]  padding_type     Block type to use in padding
  @param[in]  modulus_bitlen   The bit length of the RSA modulus
  @param[in]  hashidx          The hash index used for signature
  @param[out] out              [out] The destination for the encoded data
  @param[in,out] outlen        [in/out] The max size and resulting size of the encoded data

  @return 
   CE_SUCCESS     - Function executes successfully. \n
   CE_ERROR_NOT_SUPPORTED - the feature is not supported. \n
   CE_ERROR_INVALID_PACKET - invalid packet. \n
   CE_ERROR_BUFFER_OVERFLOW - not enough space for output. \n

  @dependencies
  None. 
 */
int ce_pkcs1_v1_5_padding_encode
(
   const unsigned char     *msg,
   int                     msglen,
   CE_RSA_PADDING_TYPE     padding_type,
   int                     modulus_bitlen,
   int                     hashidx,
   unsigned char           *encmsg,
   int                     *encmsglen
)
{
  return CE_ERROR_NOT_SUPPORTED;
#if 0
   unsigned char           *tmp_enc = NULL;
   int                     enchash_len = 0;
   int                     modulus_len = 0;
   int                     ps_len = 0;
   int                     rand_len = 0;
   unsigned char           *ps = NULL;
   int                     ce_status = CE_SUCCESS;


   /* i/p param check */
   if( NULL == msg
       ||
       0 == msglen
       ||
       NULL == encmsg
       ||
       0 == *encmsglen )
   {
      return CE_ERROR_INVALID_ARG;
   }

   do
   {
      /* valid padding? */
      if ( padding_type != CE_RSA_PAD_PKCS1_V1_5_ENC && padding_type != CE_RSA_PAD_PKCS1_V1_5_SIG )
      {
         return CE_ERROR_NOT_SUPPORTED;
      }

      if ( padding_type == CE_RSA_PAD_PKCS1_V1_5_SIG )
      {
         /* Hash algo supported? */
         if ( hashidx == 0 || hashidx >= CE_HASH_IDX_MAX )
         {
            ce_status = CE_ERROR_NOT_SUPPORTED;
            break;
         }
      }

      modulus_len = (modulus_bitlen >> 3) + (modulus_bitlen & 7 ? 1 : 0);
   
      /* test message size */
      if( padding_type == CE_RSA_PAD_PKCS1_V1_5_ENC )
      {
         if((modulus_len < 11)
            || 
            msglen > (modulus_len - 11) )
         {
            ce_status = CE_ERROR_MESSAGE_TOO_LONG;
            break;
         }
      }
      else
      if( padding_type == CE_RSA_PAD_PKCS1_V1_5_SIG )
      {
         if((modulus_len < 11 + DER_enc_hash[hashidx].DERheaderlen)
            ||
            (msglen > (modulus_len - 11 - DER_enc_hash[hashidx].DERheaderlen)))
         {
            ce_status = CE_ERROR_MESSAGE_TOO_LONG;
            break;
         }
      }

   
      if ( *encmsglen < modulus_len )
      {
         *encmsglen = modulus_len;
         ce_status = CE_ERROR_BUFFER_OVERFLOW;
         break;
      }
   
      /* generate an octets string PS */
      if ( padding_type == CE_RSA_PAD_PKCS1_V1_5_ENC )
      {
         ps_len = modulus_len - msglen - 3;
         encmsg[0] = 0x00;
         encmsg[1] = 0x02;  /* block_type 1 or 2 */
         ps = &encmsg[2];
         /* get random data */
         rand_len = ce_prng_getdata(ps, ps_len);
         if( rand_len != ps_len )
         {
            ce_status = CE_ERROR_FAILURE;
            break;
         }         

         /* replace all 0's in the buffer with non-zero random data */
         ce_util_replace_zeros(ps, ps_len);
         
         encmsg[2 + ps_len] = 0x00;
         memcpy(&encmsg[2 + ps_len + 1], msg, msglen);
      }
      else
      {
         /*hash is produced from SHA1 Hash function*/
         if ( msglen != DER_enc_hash[hashidx].hashsize )
         {
            ce_status = CE_ERROR_BUFFER_OVERFLOW;
            break;
         }
         enchash_len = DER_enc_hash[hashidx].DERheaderlen + msglen;
         ps_len = modulus_len - enchash_len - 3;
         encmsg[0] = 0x00;
         encmsg[1] = 0x01;  /* block_type 1 or 2 */
         ps = &encmsg[2];
         memset(ps, 0xFF, ps_len);
         /* create string of length modulus_len */
         encmsg[2 + ps_len] = 0x00;
         tmp_enc = &encmsg[2 + ps_len + 1]; 
         memcpy(tmp_enc, DER_enc_hash[hashidx].DERheader, DER_enc_hash[hashidx].DERheaderlen );
         tmp_enc += DER_enc_hash[hashidx].DERheaderlen;
         memcpy(tmp_enc, msg, msglen);
      }   
      *encmsglen = modulus_len;
   
   }while( 0 );

   return ce_status;
#endif //#if 0
} /* ce_pkcs_1_v1_5_encode */

/** 
  PKCS #1 v1.5 decode.

  @param[in] msg              The encoded data to decode
  @param[in] msglen           The length of the encoded data (octets)
  @param[in] block_type       Block type to use in padding (\sa ltc_pkcs_1_v1_5_blocks)
  @param[in] modulus_bitlen   The bit length of the RSA modulus
  @param[out] decmsg          Destination of decoding
  @param[in, out] decmsglen   The max size and resulting size of the decoding


  @return 
   CE_SUCCESS     - Function executes successfully. \n
   CE_ERROR_NOT_SUPPORTED - the feature is not supported. \n
   CE_ERROR_INVALID_PACKET - invalid packet. \n
   CE_ERROR_BUFFER_OVERFLOW - not enough space for output. \n

 */
int ce_pkcs1_v1_5_padding_decode
(
   const unsigned char   *msg, 
   int                   msglen,
   CE_RSA_PADDING_TYPE   padding_type,
   int                   modulus_bitlen,
   int                   hashidx,
   unsigned char         *decmsg, 
   int                   *decmsglen
)
{
  return CE_ERROR_NOT_SUPPORTED;
#if 0
   int            modulus_len = 0;
   int            ps_len = 0;
   int            i = 0;
   int            ce_status = CE_SUCCESS;

   /* i/p param check */
   if( NULL == msg
       ||
       0 == msglen
       ||
       NULL == decmsg
       ||
       0 == *decmsglen )
   {
      return CE_ERROR_INVALID_ARG;
   }

   do
   {
      /* valid padding? */
      if ( padding_type != CE_RSA_PAD_PKCS1_V1_5_ENC )
      {
         ce_status = CE_ERROR_NOT_SUPPORTED;
         break;
      }
   
      modulus_len = (modulus_bitlen >> 3) + (modulus_bitlen & 7 ? 1 : 0);
   
      /* test message size */
      if ( msglen > modulus_len || modulus_len < 11 )
      {
         ce_status = CE_ERROR_MESSAGE_TOO_LONG;
         break;
      }
   
      if ( (msg[0] != 0x00) || (msg[1] != (unsigned char)padding_type) )
      {
         ce_status = CE_ERROR_INVALID_PACKET;
      }
   
      /* generate an octets string PS */
      /*encoded message EM = 0x00||0x02||PS||0x00||M */
      for ( i = 2; i < modulus_len; i++ )
      {
         /*0x00 is the seperator between DER padding and the message*/
         if ( msg[i] == 0x00 )
            break;
      }

      ps_len = i - 2;
      if ( i == modulus_len || ps_len < 8 )
      {
         /* if there was no octet with hexadecimal value 0x00 to separate ps from message,
          * or the length of ps is less than 8 octets.
          */
         ce_status = CE_ERROR_INVALID_PACKET;
      }

   }while( 0 );
   
   if ( CE_SUCCESS != ce_status )
   {
      return CE_ERROR_FAILURE;
   }
   else
   {
      if ( *decmsglen < (msglen - (2 + ps_len + 1)) )
      {
         /*Buffer Overflow*/
         return CE_ERROR_FAILURE;
      }

      *decmsglen = (msglen - (2 + ps_len + 1));
      memcpy(decmsg, &msg[2 + ps_len + 1], *decmsglen);
      return CE_SUCCESS;
   }
#endif // #if 0
} /* ce_pkcs1_v1_5_decode */


void i2osp(uint8 *out, uint8 *counter, uint8 len)
{
   uint8    i, j;

   for( i = 0, j = len - 1; i < len; i++, j-- )
   {
      out[i] = counter[j];
   }
}


/**
  PKCS #1 MGF1 (Mask Generation Function).
   
  @param[in]  hashidx          The hash index for encryption 
  @param[in]  seed             The data to use as seed 
  @param[in]  seedlen          The length of the seed
  @param[out] mask             The pointer hold the generated mask
  @param[in]  masklen          The lenght of requested mask 
  
  @return 
   CE_SUCCESS     - Function executes successfully. \n
   CE_ERROR_NOT_SUPPORTED - the feature is not supported. \n
   CE_ERROR_INVALID_PACKET - invalid packet. \n
   CE_ERROR_BUFFER_OVERFLOW - not enough space for output. \n

  @dependencies
  None. 
 */
int ce_pkcs1_mgf1
(
   int                   hash_idx,
   unsigned char         *seed,
   unsigned long         seedlen,
   unsigned char         *mask,
   unsigned long         masklen
)
{
  return CE_ERROR_NOT_SUPPORTED;
#if 0
   int                   ce_status = CE_SUCCESS;
   unsigned long         counter = 0;
   uint8                 temp_counter[4] = {0};
   int                   i = 0;
   int                   hashlen = 0;
   ce_hash_ctx           *hash_ctx = NULL;
   unsigned char         *temp_buf = NULL;

   /* i/p param check */
   if ( hash_idx < CE_HASH_IDX_SHA1
        ||
        hash_idx > CE_HASH_IDX_SHA256   // only supports SHA1 & SHA256
        ||
        NULL == seed
        ||
        0 == seedlen 
        ||
        NULL == mask 
        ||
        0 == masklen )
   {
      return CE_ERROR_INVALID_ARG;
   }

   /* get the hash length */
   hashlen = DER_enc_hash[hash_idx].hashsize;

   do
   {
      for ( ;i < masklen; counter++ )
      {
         ce_status = ce_hash_init(hash_idx, &hash_ctx);
         if ( CE_SUCCESS != ce_status )
         {
            break;
         }

         ce_status = ce_hash_update(hash_ctx,(uint8*)seed, seedlen);
         if ( CE_SUCCESS != ce_status )
         {
            break;
         }

         i2osp(temp_counter, (uint8*)&counter, 4);
         
         ce_status = ce_hash_update(hash_ctx, temp_counter, 4);
         if ( CE_SUCCESS != ce_status )
         {
            break;
         }

         if ( masklen >= i + hashlen )
         {
            ce_status = ce_hash_final(hash_ctx,(uint8*)(mask+i), hashlen);
            if ( CE_SUCCESS != ce_status )
            {
               break;
            }
         }
         else
         {
            temp_buf = (unsigned char*)malloc(hashlen);
            if ( NULL == temp_buf )
            {
               ce_status = CE_ERROR_NO_MEMORY;
               break;
            }

            ce_status = ce_hash_final(hash_ctx,(uint8*)(temp_buf), hashlen);
            if ( CE_SUCCESS != ce_status )
            {
               break;
            }
            memcpy((mask+i), temp_buf, (hashlen - ((i+hashlen) - masklen)));
         }
         ce_hash_free_ctx(hash_ctx);
         i += hashlen;
      }
   }while( 0 );

   if( temp_buf )
   {
      memset(temp_buf, 0, hashlen);
      free(temp_buf);
   }

   return ce_status;
#endif // #if 0
}/* ce_pkcs1_mgf1 */


/**
  PKCS #1 OAEP encode.
 
  @param[in] msg              The data to encode
  @param[in] msglen           The length of the data to encode (octets) 
  @param[in] lparam           Label to  identify which system 
                              encoded the message
  @param[in] lparamlen        The length of Label
  @param[in] modulus_bitlen   The bit length of the RSA modulus
  @param[in] hashidx          The hash index for encryption
  @param[out] out             [out] The destination for the encoded data
  @param[in,out] outlen       [in/out] The max size and resulting size of the encoded data

  @return 
   CE_SUCCESS     - Function executes successfully. \n
   CE_ERROR_NOT_SUPPORTED - the feature is not supported. \n
   CE_ERROR_INVALID_PACKET - invalid packet. \n
   CE_ERROR_BUFFER_OVERFLOW - not enough space for output. \n

  @dependencies
  None. 
 */
int ce_pkcs1_oaep_padding_encode
(
   const unsigned char     *msg,
   unsigned long           msglen,
   const unsigned char     *lparam,
   unsigned long           lparamlen,
   unsigned long           modulus_bitlen,
   int                     hash_idx,
   unsigned char           *out,
   unsigned long           *outlen
)
{
  return CE_ERROR_NOT_SUPPORTED;
#if 0
   int                   ce_status = CE_SUCCESS;
   int                   modulus_len = 0;
   unsigned long         hashlen = 0;
   unsigned char         *lhash = NULL;
   unsigned char         *DB = NULL;
   unsigned long         DBlen = 0;
   unsigned char         *dbMask = NULL;
   unsigned char         *maskedDB = NULL;
   unsigned long         PSlen = 0;
   unsigned char         *seed = NULL;
   unsigned long         seedlen = 0;
   unsigned char         *seedMask = NULL;
   unsigned char         *maskedSeed = NULL;
   unsigned long         i = 0;


   /* i/p param check */
   if ( NULL == msg
        ||
        0 == msglen
        ||
        0 == modulus_bitlen
        ||
        hash_idx < CE_HASH_IDX_SHA1
        ||
        hash_idx > CE_HASH_IDX_SHA256   // only supports SHA1 & SHA256
        ||
        NULL == out
      )
   {
      return CE_ERROR_INVALID_ARG;
   }

   do
   {
      modulus_len = (modulus_bitlen >> 3) + (modulus_bitlen & 7 ? 1 : 0);

      hashlen = DER_enc_hash[hash_idx].hashsize;

      /* check size limitations */
      if ( *outlen < modulus_len
           ||
           msglen > modulus_len - (2*hashlen) - 2 
         )
      {
         ce_status = CE_ERROR_MESSAGE_TOO_LONG;
         break;
      }

      /* calculate hash of label */
      switch ( hash_idx )
      {
         case CE_HASH_IDX_SHA1:
            lhash = lhash_0_sha1;
            break;
   
         case CE_HASH_IDX_SHA256:
            lhash = lhash_0_sha256;
            break;
      }

      /* Label lparam can be an empty string */
      /* if it is not */
      if ( NULL != lparam )
      {
         lhash = NULL;
         lhash = (unsigned char*)malloc(hashlen);
         if ( NULL == lhash )
         {
            ce_status = CE_ERROR_NO_MEMORY;
            break;
         }
         ce_status = ce_hash(hash_idx, lparam, lparamlen, lhash, hashlen);
         if ( CE_SUCCESS != ce_status )
         {
            ce_status = CE_ERROR_FAILURE;
            break;
         }
      }
      /* PS = (0x00)1 (0x00)2 (0x00)3 ... (0x00)(modulus_len - msglen - 2*hashlen - 2) */
      PSlen = modulus_len - msglen - (2*hashlen) - 2;

      /* DBlen = modulus_len - haslen - 1 */
      DBlen = modulus_len - hashlen - 1;

      /* DB = lhash || PS || 0x01 || msg */
      DB = (unsigned char*)malloc(DBlen);
      if ( NULL == DB )
      {
         ce_status = CE_ERROR_NO_MEMORY;
         break;
      }
      memset(DB, 0, DBlen);
      memcpy(DB, lhash, hashlen);
      DB[hashlen+PSlen] = 0x01;
      memcpy((DB+hashlen+PSlen+1), msg, msglen);

      /* seed = random octet string of length hashlen */
      seedlen = hashlen;
      seed = (unsigned char*)malloc(seedlen);
      if ( NULL == seed )
      {
         ce_status = CE_ERROR_NO_MEMORY;
         break;
      }

      /* get a random seed of hashlen */
      if ( seedlen != ce_prng_getdata(seed, seedlen) )
      {
         ce_status = CE_ERROR_FAILURE;
         break;
      }

      /* dbMask = MGF (seed, k – hLen – 1) */
      dbMask = (unsigned char*)malloc(DBlen);
      if ( NULL == dbMask )
      {
         ce_status = CE_ERROR_NO_MEMORY;
         break;
      }
      ce_status = ce_pkcs1_mgf1(hash_idx, seed, seedlen, dbMask, DBlen);
      if ( CE_SUCCESS != ce_status )
      {
         break;
      }

      /* maskedDB = DB ^ dbMask */
      maskedDB = DB;
      for ( i = 0; i < DBlen; i++ )
      {
         maskedDB[i] = DB[i] ^ dbMask[i];
      }

      /* seedMask = MGF (maskedDB, hashlen) */
      seedMask = (unsigned char*)malloc(seedlen);
      if ( NULL == seedMask )
      {
         ce_status = CE_ERROR_NO_MEMORY;
         break;
      }
      ce_status = ce_pkcs1_mgf1(hash_idx, maskedDB, DBlen, seedMask, seedlen);
      if ( CE_SUCCESS != ce_status )
      {
         break;
      }

      /* maskedSeed = seed ^ seedMask */
      maskedSeed = seed;
      for ( i = 0; i < seedlen; i++ )
      {
         maskedSeed[i] = seed[i] ^ seedMask[i];
      }

      /* EM = 0x00 || maskedSeed || maskedDB */
      out[0] = 0;
      memcpy((out+1), maskedSeed, seedlen);
      memcpy((out+1+seedlen), maskedDB, DBlen);

   }while( 0 );
   /* clean up */
   if ( (NULL != lparam) && (NULL != lhash) )
   {
      memset(lhash, 0, hashlen);
      free(lhash);
   }

   if ( NULL != DB )
   {
      memset(DB, 0, DBlen); 
      free(DB);
   }

   if ( NULL != seed )
   {
      memset(seed, 0, seedlen);  
      free(seed);
   }

   if ( NULL != dbMask )
   {
      memset(dbMask, 0, DBlen);   
      free(dbMask);
   }

   if ( NULL != seedMask )
   {
      memset(seedMask, 0, seedlen);   
      free(seedMask);
   }
   
   return ce_status;
#endif // #if 0
}
/**
  PKCS #1 OAEP decode.

  @param[in] msg             The encoded data to decode
  @param[in] msglen          The length of the encoded data (octets)
  @param[in] lparam          Label to  identify which system 
                             encoded the message
  @param[in] lparamlen       The length of Label
  @param[in] modulus_bitlen  The bit length of the RSA modulus
  @param[out] out            Destination of decoding
  @param[in, out] outlen     The max size and resulting size of 
                             the decoding


  @return 
   CE_SUCCESS     - Function executes successfully. \n
   CE_ERROR_INVALID_PACKET - invalid packet. \n
   CE_ERROR_BUFFER_OVERFLOW - not enough space for output. \n

 */
int ce_pkcs1_oaep_padding_decode
(
   const unsigned char       *msg,
   unsigned long             msglen,
   const unsigned char       *lparam,
   unsigned long             lparamlen,
   unsigned long             modulus_bitlen,
   int                       hash_idx,
   unsigned char             *out,
   unsigned long             *outlen
)
{
  return CE_ERROR_NOT_SUPPORTED;
#if 0
   int                   ce_status = CE_SUCCESS;
   int                   modulus_len = 0;
   unsigned long         hashlen = 0;
   unsigned char         *lhash = NULL;
   unsigned char         *lhash_ = NULL;
   unsigned char         *DB = NULL;
   unsigned long         DBlen = 0;
   unsigned char         *dbMask = NULL;
   unsigned char         *maskedDB = NULL;
   unsigned long         PSlen = 0;
   unsigned char         *seed = NULL;
   unsigned long         seedlen = 0;
   unsigned char         *seedMask = NULL;
   unsigned char         *maskedSeed = NULL;
   unsigned long         i = 0;


   /* i/p param check */
   if ( NULL == msg
        ||
        0 == msglen
        ||
        0 == modulus_bitlen
        ||
        hash_idx < CE_HASH_IDX_SHA1
        ||
        hash_idx > CE_HASH_IDX_SHA256   // only supports SHA1 & SHA256
        ||
        NULL == out
      )
   {
      return CE_ERROR_INVALID_ARG;
   }

   do
   {
      modulus_len = (modulus_bitlen >> 3) + (modulus_bitlen & 7 ? 1 : 0);

      hashlen = DER_enc_hash[hash_idx].hashsize;

      /* check size limitations */
      if ( *outlen < modulus_len )
      {
         ce_status = CE_ERROR_INVALID_ARG;
         break;
      }

      /* EM = Y || maskedSeed || maskedDB */
      if ( 0x00 != msg[0] )
      {
         ce_status = CE_ERROR_FAILURE;
      }
      DBlen = modulus_len - hashlen - 1;

      seedlen = hashlen;
      maskedSeed = &msg[1];
      maskedDB = &msg[1+seedlen];

      /* seedMask = MGF(maskedDB, seedlen) */
      seedMask = (unsigned char*)malloc(seedlen);
      if ( NULL == seedMask )
      {
         ce_status = CE_ERROR_NO_MEMORY;
         break;
      }

      if ( CE_SUCCESS != ce_pkcs1_mgf1(hash_idx, maskedDB, DBlen, seedMask, seedlen) )
      {
         ce_status = CE_ERROR_FAILURE;
         break;
      }

      /* seed = maskedSeed ^ seedMask */
      seed = seedMask;
      for ( i = 0; i < seedlen; i++ )
      {
         seed[i] = maskedSeed[i] ^ seedMask[i];
      }

      /* dbMask = MGF(seed, DBlen) */
      dbMask = (unsigned char*)malloc(DBlen);
      if ( NULL == seedMask )
      {
         ce_status = CE_ERROR_NO_MEMORY;
         break;
      }
      if ( CE_SUCCESS != ce_pkcs1_mgf1(hash_idx, seed, seedlen, dbMask, DBlen) )
      {
         ce_status = CE_ERROR_FAILURE;
         break;
      }

      /* DB = maskedDB ^ dbMask */
      DB = dbMask;
      for ( i = 0; i < DBlen; i++ )
      {
         DB[i] = maskedDB[i] ^ dbMask[i];
      }

      /* calculate hash of label */
      switch ( hash_idx )
      {
         case CE_HASH_IDX_SHA1:
            lhash = lhash_0_sha1;
            break;
   
         case CE_HASH_IDX_SHA256:
            lhash = lhash_0_sha256;
            break;
      }

      /* Label lparam can be an empty string */
      /* if it is not */
      if ( NULL != lparam )
      {
         lhash = NULL;
         lhash = (unsigned char*)malloc(hashlen);
         if ( NULL == lhash )
         {
            ce_status = CE_ERROR_NO_MEMORY;
            break;
         }
         if ( CE_SUCCESS != ce_hash(hash_idx, lparam, lparamlen, lhash, hashlen) )
         {
            ce_status = CE_ERROR_FAILURE;
            break;
         }
      }

      /* DB = lhash_ || PS || 0x01 || M */
      lhash_ = DB;
      if ( 0 != memcmp(lhash, lhash_, hashlen) )
      {
         ce_status = CE_ERROR_FAILURE;
      }

      /* PS = (0x00)1 (0x00)2 (0x00)3 ... (0x00)(modulus_len - msglen - 2*hashlen - 2) */
      PSlen = 0;
      for ( i=hashlen; i < DBlen; i++, PSlen++ )
      {
         if ( 0x00 == DB[i] )
         {
            continue;
         }
         else  if ( 0x01 == DB[i] )
         {
            break;
         }
         else
         {
            ce_status = CE_ERROR_FAILURE;
            continue;
         }
      }

      /* could not find the '0x01' */
      if( i == DBlen )
      {
         ce_status = CE_ERROR_FAILURE;
         break;
      }

      /* Decoding failed*/
      if ( CE_SUCCESS != ce_status )
      {
         break;
      }

      *outlen = DBlen - hashlen - PSlen - 1;    
      memcpy(out, (DB+hashlen+PSlen+1), *outlen);

   }while( 0 );

   /* clean up */
   if ( (NULL != lparam) && (NULL != lhash) )
   {
      memset(lhash, 0, hashlen);   
      free(lhash);
   }

   if ( NULL != seedMask )
   {
      memset(seedMask, 0, seedlen);   
      free(seedMask);
   }

   if ( NULL != dbMask )
   {
      memset(dbMask, 0, DBlen);   
      free(dbMask);
   }

   if ( CE_SUCCESS != ce_status )
   {
      return CE_ERROR_FAILURE;
   }else{
      return CE_SUCCESS;
   }
#endif // #if 0
}


/**
  PKCS #1 PSS encode.
 
  @param[in] msghash         The data to encode
  @param[in] msghashlen      The length of the data to encode(octets)
  @param[in] saltlen         The desired length of salt
  @param[in] modulus_bitlen  The bit length of the RSA modulus
  @param[in] hashidx         The hash index used for signature
  @param[out] out            [out] The destination for the encoded data
  @param[in,out] outlen      [in/out] The max size and resulting size of the encoded data

  @return 
   CE_SUCCESS     - Function executes successfully. \n
   CE_ERROR_INVALID_ARG - Invalid input parameter. \n
   CE_ERROR_NOT_SUPPORTED - the feature is not supported. \n
   CE_ERROR_INVALID_PACKET - invalid packet. \n
   CE_ERROR_BUFFER_OVERFLOW - not enough space for output. \n

  @dependencies
  None. 
 */
int ce_pkcs1_pss_padding_encode
(
   const unsigned char     *msghash,
   unsigned long           msghashlen,
   unsigned long           saltlen,
   unsigned long           modulus_bitlen,
   int                     hash_idx,
   unsigned char           *out,
   unsigned long           *outlen
)
{
  return CE_ERROR_NOT_SUPPORTED;
#if 0
   int                   ce_status = CE_SUCCESS;
   int                   modulus_len = 0;
   unsigned char         *m_ = NULL;
   unsigned long         m_len = NULL;
   unsigned char         *hash = NULL;
   unsigned long         hashlen = 0;
   unsigned char         *salt = NULL;
   unsigned char         *DB = NULL;
   unsigned long         DBlen = 0;
   unsigned char         *dbMask = NULL;
   unsigned char         *maskedDB = NULL;
   unsigned long         PSlen = 0;
   unsigned long         i = 0;

   /* i/p param check */
   if ( NULL == msghash
        ||
        0 == msghashlen
        ||
        hash_idx < CE_HASH_IDX_SHA1
        ||
        hash_idx > CE_HASH_IDX_SHA256   // only supports SHA1 & SHA256
        ||
        NULL == out
      )
   {
      return CE_ERROR_INVALID_ARG;
   }

   do
   {
      modulus_len = (modulus_bitlen >> 3) + (modulus_bitlen & 7 ? 1 : 0);

      hashlen = DER_enc_hash[hash_idx].hashsize;

      /* check size limitations */
      if ( *outlen < modulus_len
           ||
           *outlen < hashlen + saltlen + 2 
         )
      {
         ce_status = CE_ERROR_MESSAGE_TOO_LONG;
         break;
      }

      /* M’ = (0x)00 00 00 00 00 00 00 00 || msgHash || salt */
      m_len = 8 + msghashlen + saltlen;
      m_ = (unsigned char*)malloc(m_len);
      if ( NULL == m_ )
      {
         ce_status = CE_ERROR_NO_MEMORY;
         break;
      }
      memset(m_, 0, m_len);
      memcpy((m_ + 8), msghash, msghashlen);

      salt = m_ + 8 + msghashlen;

      /* Generate Random salt */
      if(saltlen != ce_prng_getdata((uint8*)(m_ + 8 + msghashlen), saltlen))
      {
         ce_status = CE_ERROR_FAILURE;
         break;
      }

      /* hash M' */
      hash = (unsigned char*)malloc(hashlen);
      if ( NULL == hash )
      {
         ce_status = CE_ERROR_NO_MEMORY;
         break;
      }

      /* hash = hash(M') */
      ce_status = ce_hash(hash_idx, m_, m_len, hash, hashlen);
      if ( CE_SUCCESS != ce_status )
      {
         ce_status = CE_ERROR_FAILURE;
         break;
      }

      /* PS = (0x00)1 (0x00)2 (0x00)3 ... (0x00)(modulus_len - hashlen - saltlen - 2) */
      PSlen = modulus_len - hashlen - saltlen - 2;

      /* DB = PS | 0x01 | salt */
      DBlen = modulus_len - hashlen - 1;
      DB = (unsigned char*)malloc(DBlen);
      if ( NULL == DB )
      {
         ce_status = CE_ERROR_NO_MEMORY;
         break;
      }

      memset(DB, 0, DBlen);
      DB[PSlen] = 0x01;
      memcpy((DB + PSlen + 1), salt, saltlen);

      /* dbMask = MGF (H, DBlen) */
      dbMask = (unsigned char*)malloc(DBlen);
      if ( NULL == dbMask )
      {
         ce_status = CE_ERROR_NO_MEMORY;
         break;
      }

      ce_status = ce_pkcs1_mgf1(hash_idx, hash, hashlen, dbMask, DBlen);
      if ( CE_SUCCESS != ce_status )
      {
         ce_status = CE_ERROR_FAILURE;
         break;
      }

      /* maskedDB = DB ^ dbMask */
      maskedDB = dbMask;
      for ( i = 0; i < DBlen; i++ )
      {
         maskedDB[i] = DB[i] ^ dbMask[i];
      }

      /* EM =  maskedDB | H | 0xbc */
      memcpy(out, maskedDB, DBlen);
      memcpy((out + DBlen), hash, hashlen);
      out[modulus_len-1] = 0xBC;

      /* now clear the 8*modulus_len - modulus_bitlen most significant bits */
      out[0] &= 0xFF >> ((modulus_len<<3) - (modulus_bitlen-1));

      /* store output size */
      *outlen = modulus_len;    
   }while( 0 );

   /* clean up */
   if ( m_ )
      free(m_);

   if ( hash )
      free(hash);

   if ( DB )
      free(DB);

   if ( dbMask )
      free(dbMask);
   
   return ce_status;
#endif // #if 0
}/* ce_pkcs1_pss_padding_encode */

/**
  PKCS #1 PSS verify.

  @param[in] msghash          The encoded data to decode
  @param[in] msghashlen       The length of the encoded data (octets)
  @param[in] sig              The signature
  @param[in] siglen           The signature length
  @param[in] saltlen          The length of salt
  @param[in] modulus_bitlen   The bit length of the RSA modulus
  @param[in] hashidx          The hash index used for signature

  @return 
   CE_SUCCESS     - Function executes successfully. \n
   CE_ERROR_NOT_SUPPORTED - the feature is not supported. \n
   CE_ERROR_INVALID_PACKET - invalid packet. \n
   CE_ERROR_BUFFER_OVERFLOW - not enough space for output. \n

 */
int ce_pkcs1_pss_padding_verify
(
   const unsigned char       *msghash,
   unsigned long             msghashlen,
   const unsigned char       *sig,
   unsigned long             siglen,
   unsigned long             saltlen,
   unsigned long             modulus_bitlen,
   int                       hash_idx
)
{
  return CE_ERROR_NOT_SUPPORTED;
#if 0
   int                   ce_status = CE_SUCCESS;
   int                   modulus_len = 0;
   unsigned char         *m_ = NULL;
   unsigned long         m_len = 0;
   unsigned char         *hash = NULL;
   unsigned long         hashlen = 0;
   unsigned char         *hash_ = NULL;
   unsigned char         *salt = NULL;
   unsigned char         *DB = NULL;
   unsigned long         DBlen = 0;
   unsigned char         *dbMask = NULL;
   unsigned char         *maskedDB = NULL;
   unsigned long         maskedDBlen = 0;
   unsigned long         PSlen = 0;
   unsigned long         i = 0;

   /* i/p param check */
   if ( NULL == msghash
        ||
        0 == msghashlen
        ||
        NULL == sig
        ||
        0 == siglen
        ||
        hash_idx < CE_HASH_IDX_SHA1
        ||
        hash_idx > CE_HASH_IDX_SHA256   // only supports SHA1 & SHA256
      )
   {
      return CE_ERROR_INVALID_ARG;
   }

   do
   {
      modulus_len = (modulus_bitlen >> 3) + (modulus_bitlen & 7 ? 1 : 0);

      hashlen = DER_enc_hash[hash_idx].hashsize;

      /* check size limitations */
      if ( siglen < modulus_len
           ||
           siglen < hashlen + saltlen + 2
           ||
		   modulus_len < hashlen + saltlen + 2 
		 )
      {
         ce_status = CE_ERROR_INVALID_SIGNATURE;
         break;
      }

      /* sig = maskedDB | hash | 0xbc */
      if ( sig[siglen-1] != 0xBC )
      {
         ce_status = CE_ERROR_INVALID_SIGNATURE;
         break;
      }

      maskedDB = sig;
      maskedDBlen = modulus_len - hashlen - 1;

      hash_ = &sig[maskedDBlen];

      if ( (sig[0] & ~(0xFF >> ((modulus_len<<3) - (modulus_bitlen-1)))) != 0 )
      {
         ce_status = CE_ERROR_INVALID_SIGNATURE;
         break;
      }

      /* dbMask = MGF (H, DBlen) */
      DBlen = modulus_len - hashlen - 1;
      dbMask = (unsigned char*)malloc(DBlen);
      if ( NULL == dbMask )
      {
         ce_status = CE_ERROR_NO_MEMORY;
         break;
      }
      DB = dbMask;
      ce_status = ce_pkcs1_mgf1(hash_idx, hash_, hashlen, dbMask, DBlen);
      if ( CE_SUCCESS != ce_status )
      {
         ce_status = CE_ERROR_FAILURE;
         break;
      }

      /* DB = maskedDB ^dbMask */
      for ( i = 0; i < DBlen; i++ )
      {
         DB[i] = maskedDB[i] ^ dbMask[i];
      }

      DB[0] &= 0xFF >> ((modulus_len<<3) - (modulus_bitlen-1));

      /* PS = (0x00)1 (0x00)2 (0x00)3 ... (0x00)(modulus_len - hashlen - saltlen - 2) */
      PSlen = modulus_len - hashlen - saltlen - 2;

      /* DB = PS | 0x01 | salt */
      for ( i = 0; i < PSlen; i++ )
      {
         if ( DB[i] != 0 )
         {
            ce_status = CE_ERROR_INVALID_SIGNATURE;
            break;
         }
      }
      if ( CE_SUCCESS != ce_status )
      {
         break;
      }
      if ( DB[PSlen] != 0x01 )
      {
         ce_status = CE_ERROR_INVALID_SIGNATURE;
         break;
      }

      salt = &DB[PSlen+1];

      /* M’ = (0x)00 00 00 00 00 00 00 00 || msghash || salt */
      m_len = 8 + msghashlen + saltlen;
      m_ = (unsigned char*)malloc(m_len);
      if ( NULL == m_ )
      {
         ce_status = CE_ERROR_NO_MEMORY;
         break;
      }
      memset(m_, 0, m_len);
      memcpy((m_+ 8), msghash, msghashlen);
      memcpy((m_+8+msghashlen), salt, saltlen);

      /* hash M' */
      hash = (unsigned char*)malloc(hashlen);
      if ( NULL == hash )
      {
         ce_status = CE_ERROR_NO_MEMORY;
         break;
      }

      /* hash = hash(M') */
      ce_status = ce_hash(hash_idx, m_, m_len, hash, hashlen);
      if ( CE_SUCCESS != ce_status )
      {
         ce_status = CE_ERROR_FAILURE;
         break;
      }

      if ( 0 != memcmp(hash, hash_, hashlen) )
      {
         ce_status = CE_ERROR_INVALID_SIGNATURE;
      }
   }while( 0 );

   /* clean up */
   if ( dbMask )
      free(dbMask);

   if ( m_ )
      free(m_);

   if ( hash )
      free(hash);
   
   return ce_status;
#endif // #if 0
}/* ce_pkcs1_pss_padding_verify */


