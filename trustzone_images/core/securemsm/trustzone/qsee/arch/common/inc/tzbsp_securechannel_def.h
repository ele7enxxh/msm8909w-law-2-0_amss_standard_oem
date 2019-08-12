#ifndef TZBSPSECURECHANNELDEF_H
#define TZBSPSECURECHANNELDEF_H
/*===========================================================================

                    Secure Channel Key and Header formats

DESCRIPTION
  This file contains the structures and definitions of shared key material
  and headers that get added to every message.
  Shared structures between TZ and Modem
  
 Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/arch/common/inc/tzbsp_securechannel_def.h#1 $ 
  $DateTime: 2016/06/17 14:31:11 $ 
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/12/12   sm       Added source/dest client ID to secure channel header
09/12/11   jay      Created file.

===========================================================================*/

#include "comdef.h"


/*===========================================================================

                        DEFINITIONS AND TYPES

===========================================================================*/
/* Key Material Format 
 
  4 bytes   32 bytes  32 bytes     4 bytes             4 bytes
-----------------------------------------------------------------------
|magic-num|cipher-key|hmac-key|peer-sequence-number|tz-sequence-number| 
----------------------------------------------------------------------- 
*/
#define SC_MAGIC_NUM     (0x7363686E)    /* "schn" in hex */
#define SC_CIPHER_KEYLEN (32)  /* 16 bytes if AES128 and 32 if AES256   */
#define SC_HMAC_KEYLEN   (32)  /* 20 bytes if SHA1 and 32 bytes if SHA2 */
#define SC_PAD_LENGTH    (52)  /* modem side cache line alignment */


/** Key Material Type */
typedef struct
{
  uint32 magic_num;  
  uint8  cipher_key[SC_CIPHER_KEYLEN];
  uint8  hmac_key[SC_HMAC_KEYLEN];
  uint32 peer_sequence_num;      /* Initial sequence number modem will use for sending messages */
  uint32 peer_local_sequence_num;      /* Check for replay attach on peer sequence number */
  uint32 tz_sequence_num; /* Intial sequence number TZ will use for sending messages */
  uint32 tz_local_sequence_num;        /* Check for replay attach on TZ sequence number  */
  uint8  pad_cache_line[SC_PAD_LENGTH];
}__attribute__ ((packed)) sc_key_mat_type;




/* Message Format 
 
 1byte   1byte     1byte 1byte  1byte    1byte       1byte       1byte        4bytes         4bytes  16bytes              32 bytes
----------------------------------------------------------------------------------------------------------------------------------
|source|destination|cid|padding|version|cipher-algo|cipher-mode|padding-type|sequence-number|length|    iv      |cipher-text|hmac|
----------------------------------------------------------------------------------------------------------------------------------

*/ 

/** Cipher supported algorithms */
typedef enum
{
   SC_CIPHER_ALGO_AES_128    = 0,
   SC_CIPHER_ALGO_AES_256,
   SC_CIPHER_ALGO_COUNT,
   SC_CIPHER_ALGO_UNSUPPORTED = 0x7F
} SC_CIPHER_ALGO_ET;

/** Supported feedback modes of operation */
typedef enum
{
   SC_CIPHER_MODE_ECB        = 0,
   SC_CIPHER_MODE_CBC,
   SC_CIPHER_MODE_CTR,
   SC_CIPHER_MODE_COUNT,
   SC_CIPHER_MODE_UNSUPPORTED = 0x7F
} SC_CIPHER_MODE_ET;

/** Supported padding types */
typedef enum
{
  SC_CIPHER_PAD_PKCS7      = 0,
  SC_CIPHER_PAD_NO_PAD,
  SC_CIPHER_PAD_COUNT,
  SC_CIPHER_PAD_UNSUPPORTED = 0x7F
} SC_CIPHER_PAD_ET;

/** Message header type */
typedef struct
{
  uint32 flow;          /* source|destination|client ID|padding        */
  uint32 cipher_info;  /* version|algo|mode|padding */
  uint32 sequence_num;
  uint32 length;      /* length of the encrypted message including IV */
}__attribute__ ((packed)) sc_msg_hdr_type;

/* HMAC Info */
/* SHA2 will be used for HMAC  */
#define SC_HMAC_LENGTH (32)

/* IV can be random bytes or counter value for CTR mode */
#define SC_IV_LENGTH (16)



#endif // TZBSPSECURECHANNELDEF_H
