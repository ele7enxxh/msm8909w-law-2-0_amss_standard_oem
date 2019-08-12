#ifndef SECUTIL_H
#define SECUTIL_H

/** @file secutil.h
 * @brief
 * This file contains the definitions of the constants, data structures and
 * and interfaces that provides Cryptographic Utility Library.
 * 
 */
/*===========================================================================
  Copyright (c) 2008 - 2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/crypto/secutil.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/11/13   ejt     Klocwork fixes
11/09/10   dm      Removed all references to sec_strlcat(), sec_strlcpy() and secutil_log_opaque
08/28/10   dm      Crypto API files rearchitecture
09/22/10   mh      Add tags to export secutil_get_random() rapi functions
06/06/10   qxu     added mainpage, dscriptions for functions, constants and data types
                   to doxygenate API document.
09/09/09   bm      removed FEATURE_HTORPC_METACOMMENTS for CMI Compliance
08/28/09   ejt     removed #include "customer.h" and FEATURE flags for CMI compliance
08/26/09   ejt     removed contence of seccrypt_v.h for CMI compliance
08/26/09   ejt     added contence of seccrypt_v.h for CMI compliance
02/06/09   qxu     port INTLOCK/INTFREE to new critical section mechnism on QDSP6 Hexgon. 

Copyright (c) 2008 - 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved. 
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "secerrno.h"
#include "msg.h"
#include "ixipc.h"

/** @addtogroup crypto_secutilapi 
  @{ */

/** 
  A substitute for changing all the uses of secutil_log_opaque at once.
 */
#define secutil_log_opaque(pid, pdata, plen)  \
  secutil_log_opaque_level( (pid), (pdata), (plen), MSG_LVL_MED);

/** 
  Defines the secutil random number buffer.
 */
typedef struct
{
  boolean  done;       /**< For compatibility with 1x. */
}secutil_rnd_type;

extern secutil_rnd_type secutil_rnd;

/**
  SSL opaques (byte arrays) logging IDs.
*/
typedef enum
{
  
  SECUTIL_OPAQUE_UNKNOWN = 0x0, /**< Generic log ID, prints extra message 
                                     before. */

  /*  Key exchange parameters */
  SECUTIL_OPAQUE_DH_BASE,          /**< DH common base.        */
  SECUTIL_OPAQUE_DH_MOD,           /**< DH common modulo.      */
  SECUTIL_OPAQUE_DH_EXP,           /**< DH exponent.           */
  SECUTIL_OPAQUE_DH_CLNT_PK,       /**< DH client public key.  */
  SECUTIL_OPAQUE_DH_SVR_PK,        /**< DH server public key.  */
  SECUTIL_OPAQUE_DH_CLNT_SK,       /**< DH client secret key.  */
  SECUTIL_OPAQUE_DH_SVR_SK,        /**< DH server secret key.  */
  SECUTIL_OPAQUE_RSA_CLNT_SK,      /**< RSA client secret key. */
  SECUTIL_OPAQUE_RSA_CLNT_EK,      /**< RSA client encryption key. */

  /*   Generic Handshake Protocol IDs */
  SECUTIL_OPAQUE_SVR_MD5_HASH,     /**< HSP MD5 hash (server). */
  SECUTIL_OPAQUE_SVR_SHA_HASH,     /**< HSP SHA hash (server). */
  SECUTIL_OPAQUE_SVR_VER_DATA,     /**< HSP verify data (server). */
  SECUTIL_OPAQUE_CLNT_MD5_HASH,    /**< HSP MD5 hash (client). */
  SECUTIL_OPAQUE_CLNT_SHA_HASH,    /**< HSP SHA hash (client). */
  SECUTIL_OPAQUE_CLNT_VER_DATA,    /**< HSP verify data (client). */

  /*  Key Generation IDs */
  SECUTIL_OPAQUE_KEY_MATERIAL,     /**< Key material. */
  SECUTIL_OPAQUE_MASTER_SECRET,    /**< Master secret. */
  SECUTIL_OPAQUE_CLN_MAC_SECRET,   /**< Client MAC generation secret. */
  SECUTIL_OPAQUE_CLN_ENC_KEY,      /**< Client Encryption key. */
  SECUTIL_OPAQUE_CLN_ENC_IV,       /**< Client Encryption IV. */
  SECUTIL_OPAQUE_SVR_MAC_SECRET,   /**< Server MAC generation secret. */
  SECUTIL_OPAQUE_SVR_ENC_KEY,      /**< Server Encryption key. */
  SECUTIL_OPAQUE_SVR_ENC_IV        /**< Server Encryption IV. */
} secutil_opaque_logid_enum_type;

/**
   Provides QXDM output based on a run-time message level.
*/
#define SEC_MSG_LVL(m_lvl, x_fmt, a, b, c) \
  do{                              \
    switch( m_lvl)                 \
    {                              \
    default:                       \
    case MSG_LEGACY_FATAL:            \
      MSG_FATAL(x_fmt, a, b, c);   \
      break;                       \
    case MSG_LEGACY_ERROR:            \
      MSG_ERROR(x_fmt, a, b, c);   \
      break;                       \
    case MSG_LEGACY_HIGH:             \
      MSG_HIGH(x_fmt, a, b, c);    \
      break;                       \
    case MSG_LEGACY_MED:              \
      MSG_MED(x_fmt, a, b, c);     \
      break;                       \
    case MSG_LEGACY_LOW:              \
      MSG_LOW(x_fmt, a, b, c);     \
      break;                       \
    }/* endswitch */               \
  }while(0)


/**
  Gets random data from the searcher task and merges it into the random data 
  buffer storage area.

  @param[in,out] rnd_data_ptr   Pointer to the collected random data.
  @param[in] rnd_data_len       Length of the collected random data in bytes.

  @return    
  None.

  @dependencies
  The task calling this function is assumed to have higher priority than the 
  tasks using the random data. 

  @sideeffects    
  Updates the random data buffer storage area.
*/
void secutil_fill_rnd_data
(
  uint8*  rnd_data_ptr,
  uint16  rnd_data_len
);
/**
  Compares two byte buffers with byte_len.

  @param[in] value1_ptr Pointer to value1.
  @param[in] value2_ptr Pointer to value2.
  @param[in] byte_len   Byte length.
  
  @return
  None.

  @dependencies
  None.
*/
extern uint16 secutil_byte_compare
(
  const uint8*  value1_ptr,
  const uint8*  value2_ptr,
  const uint16  byte_len
);

/**
  Obtains a random number for security layer encryption functions. 

  The buffer is guaranteed to have no string of 8 bits set to zero in a row.
  If sufficient random data has been collected, the random buffer is returned; 
  otherwise, no data is returned.

  @param[in,out] random_ptr   Buffer to be filled with the random data.
  @param[in]     random_len   Amount of random data requested (maximum = 128).

  @return
  E_SUCCESS  - Enough data was collected. \n
  E_AGAIN    - More data needs to be collected. 
  
  @dependencies    
  The tasks calling this function are assumed to have lower priority than the 
  task filling the random data.
*/
secerrno_enum_type secutil_get_random
(
  uint8*  random_ptr,
  uint16  random_len
);

/**
  Logs arbitrary byte arrays from the handshake phase of the SSL.

  The message is output only if the severity level parameter is greater than or 
  equal to the compile-time message limit.

  @param[in] pid       Process ID.
  @param[in] pdata     Log message.
  @param[in] plen      Log message length.
  @param[in] msg_lvl   Message level.

  @return
  None.

  @dependencies
  None.
*/
extern void secutil_log_opaque_level
(
        secutil_opaque_logid_enum_type  pid,
  const uint8*                          pdata,
  const uint16                          plen,
  const uint32                          msg_lvl
);

/**
  Returns a unique key stored in SFS on the modem side.

  @param[in,out] key_ptr    Buffer to be filled in with the key data.
  @param[in] key_len        Length of the buffer passed in.

  @return
  E_SUCCESS - Success. \n
  An error code on failure.
  
  @dependencies    
  None.
*/
secerrno_enum_type secutil_get_sfs_key
(
  uint8*  key_ptr,
  uint32  key_len
);
 
/** @} */  /* end_addtogroup_crypto_secutilapi */

#endif /* SECUTIL_H */

