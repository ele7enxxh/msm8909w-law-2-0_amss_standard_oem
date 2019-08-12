#ifndef _SECCRYPTDIAG_H
#define _SECCRYPTDIAG_H

/*===========================================================================

                  S E C U R I T Y   S E R V I C E S

            D I A G   I N T E R F A C E   D E F I N I T I O N S


This file defines the DIAG interface (events, logs) used by SEC Crypto Layer.

Changes to these enums must be coordinated with the diag/tools team.
As of the creation date mail had to be sent to asw.diag.request.

Copyright (c) 2003-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/crypto/shared/src/seccryptdiag.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/09/10   ejt     Removed FEATURE_SEC_LOGGING 
07/18/10   nk      Shared Library Framework - Removed FEATURE_SEC
03/30/10   yh      SSL feature clean up.
11/12/04   rv      changes to correct LINT errors
09/29/03   rwh     Added comments about tool output.
09/10/03   rwh     Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "event.h"
#include "log.h"


/*===========================================================================

                 DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/

/*--------------------------------------------------------------------------
The following indicates the nature of the SECCRYPT_EVENT.
--------------------------------------------------------------------------*/
typedef enum
{                                           // Display:
  SECCRYPT_QDSPSERV_COMMAND_EVENT   = 4,    // "QDSP Serv. Cmd. Status:"
  SECCRYPT_QDSPSERV_MODULE_EVENT    = 5,    // "QDSP Serv. Mod. Status:"
  SECCRYPT_QDSPSERV_IMAGE_EVENT     = 6,    // "QDSP Serv. Evt. Status:"

  SECCRYPT_DSP_RSP_EVENT            = 9,    // "Receive DSP Status:"
  SECCRYPT_DSP_TIMEOUT_EVENT        = 10,   // "DSP Response Timeout:"

  SECCRYPT_ARM_RSP_EVENT            = 17,   // "Receive ARM Status:"
  SECCRYPT_ARM_TIMEOUT_EVENT        = 18,   // "ARM Response Timeout:"

  SECCRYPT_MAX_EVENT                = 0xffff /* 2-byte length */
} seccrypt_event_enum_type;

/*--------------------------------------------------------------------------
The command indicator of the SECCRYPT_EVENT or SECCRYPT_CMD_EVENT is
the actual DSP command, or one of the following values:
--------------------------------------------------------------------------*/
 
/* Register application        */
#define SECCRYPT_QDSP_CMD_REGISTER_APP        0x10001

/* Register ISR                     */
#define SECCRYPT_QDSP_CMD_REGISTER_ISR        0x10002 

/* Enable QDSP module               */
#define SECCRYPT_QDSP_CMD_ENABLE              0x10003

/* Disable QDSP module              */
#define SECCRYPT_QDSP_CMD_DISABLE             0x10004

/* Application allowed disabling    */
#define SECCRYPT_QDSP_CMD_DISABLE_EVENT_RSP   0x10005

/* Application acked image change   */
#define SECCRYPT_QDSP_CMD_IMAGE_CHG_EVENT_RSP 0x10006

/* Reset the DSP before enabling    */
#define SECCRYPT_QDSP_CMD_RESET_AND_ENABLE    0x10007


/*--------------------------------------------------------------------------
The payload for the SECCRYPT_EVENT indicates the nature of the event,
the status, the command, the crypto instance identifier, and one reserved 
value.
The display order should be:
"Receive DSP Status: 0xss, Cmd: 0xyyyy, Inst: 0xzzzz, Val1: 0xvvvv"
--------------------------------------------------------------------------*/
typedef struct
{                                           // Display:
  seccrypt_event_enum_type   specific_event;// (see above)
  uint16                     status;        // 0xss
  uint32                     command;       // "Cmd: " 0xyyyy
  uint32                     instance_id;   // "Inst: " 0xzzzz
  uint32                     value1;        // "Val1: " 0xvvvv
}seccrypt_event_payload_type;


/*--------------------------------------------------------------------------
The following indicates the nature of the SECCRYPT_CMD_EVENT.
--------------------------------------------------------------------------*/
typedef enum
{                                           // Display:
  SECCRYPT_DSP_CMD_EVENT            = 8,    // "Issue DSP Cmd:"

  SECCRYPT_ARM_CMD_EVENT            = 16,   // "Issue ARM Cmd:"

  SECCRYPT_QDSP_SERV_CMD_EVENT      = 32,   // "Issue QDSP Serv. Cmd:"

  SECCRYPT_CMD_MAX_EVENT            = 0xffff /* 2-byte length */
} seccrypt_cmd_event_enum_type;

/*--------------------------------------------------------------------------
The payload for the SECCRYPT_CMD_EVENT indicates the nature of the event,
the status, the command, the crypto instance identifier, and one reserved 
value.
The display order should be:
"Issue DSP Cmd: 0xyyyy, Inst: 0xzzzz, Status: 0xss, Val1: 0xvvvv"
--------------------------------------------------------------------------*/
typedef struct
{                                             // Display:
  seccrypt_cmd_event_enum_type specific_event;// (see above)
  uint16                       status;        // "Status: 0xss
  uint32                       command;       // 0xyyyy 
  uint32                       instance_id;   // "Inst: " 0xzzzz
  uint32                       value1;        // "Val1: "0xvvvv
}seccrypt_cmd_event_payload_type;

  /* SLF Clean up : removed FEATURE_SECCRYPT_DIAG_EVENTS
     TODO: clean usage of macros also */
  #define SECCRYPT_DIAG_EVENT_ALLOC( payload ) 
  #define SECCRYPT_CMD_DIAG_EVENT_ALLOC( payload ) 
  #define SECCRYPT_DIAG_EVENT_SET( payload_ptr, ev, stat, cmd, inst_ptr)
  #define SECCRYPT_CMD_DIAG_EVENT_SET( payload_ptr, ev, stat, cmd, inst_ptr)
  #define SECCRYPT_DIAG_EVENT_REPORT( payload_ptr )
  #define SECCRYPT_CMD_DIAG_EVENT_REPORT( payload_ptr )

/*--------------------------------------------------------------------------
SECCRYPT_EVENT enum value (976) or SECCRYPT_CMD_EVENT enum value (977) 
is displayed by QXDM when using a version
of the tool which predates the enum assignment.
The payload is displayed as a sequence of unsigned 8-bit hex values:
2 bytes of specific event;
2 bytes of status ( 0 = success, 1 = failure);
4 bytes of command;
4 bytes of crypto instance id;
4 bytes of zero.

The display order is network order, thus the values are reversed from the
ARM order, e.g. the ARM value for "QDSP Image Event" is 0x0006, 
and is displayed as "06  00"
--------------------------------------------------------------------------*/


/*** LOG Codes  --  Range 158 - 187 assigned by Steve Harris ***/
#define LOG_SEC_C_BASE                      158

#define LOG_SEC_PKX_MM_STATS_C              (LOG_SEC_C_BASE + 0)
#define LOG_SEC_PKX_MM_OP_C                 (LOG_SEC_C_BASE + 1) 
#define LOG_SEC_ENC_STATS_C                 (LOG_SEC_C_BASE + 2) 
#define LOG_SEC_ENC_OP_DES_STATS_C          (LOG_SEC_C_BASE + 3) 
#define LOG_SEC_ENC_OP_ARC4_STATS_C         (LOG_SEC_C_BASE + 4) 
#define LOG_SEC_ENC_OP_PLAINTEXT_C          (LOG_SEC_C_BASE + 5) 
#define LOG_SEC_ENC_OP_CIPHERTEXT_C         (LOG_SEC_C_BASE + 6) 
#define LOG_SEC_ENC_OP_MAC_C                (LOG_SEC_C_BASE + 7) 
#define LOG_SEC_HSH_STATS_C                 (LOG_SEC_C_BASE + 8) 
#define LOG_SEC_HSH_OP_INPUT_C              (LOG_SEC_C_BASE + 9) 
#define LOG_SEC_HSH_OP_HASH_C               (LOG_SEC_C_BASE + 10)

/*** Maximum payload size per variable log record ***/
#define SEC_MAX_LOG_SIZE         900

/*** Enum of all operations supported by crypto instances ***/
typedef enum
{
  /* Public Key Exchange Operations */
  SECLOG_OP_PKX_DH_PRIVATE_KEY = 0x00,
  SECLOG_OP_PKX_DH_PUBLIC_KEY,
  SECLOG_OP_PKX_DH_SECRET_KEY,
  SECLOG_OP_PKX_RSA_PRIME_P,
  SECLOG_OP_PKX_RSA_PRIME_Q,
  SECLOG_OP_PKX_RSA_PRIVATE_KEY,
  SECLOG_OP_PKX_RSA_PUBLIC_KEY,
  SECLOG_OP_PKX_RSA_SECRET_KEY,
  SECLOG_OP_PKX_RSA_ENCRYPTED_KEY,
  SECLOG_OP_PKX_RSA_SIGNATURE_VERIFY,

  /* En- / Decryption Operations */
  SECLOG_OP_ENC_DES_ENCRYPT = 0x20,
  SECLOG_OP_ENC_DES_ENCRYPT_SSLV3_MAC,
  SECLOG_OP_ENC_DES_ENCRYPT_TLS10_MAC,
  SECLOG_OP_ENC_DES_DECRYPT,
  SECLOG_OP_ENC_DES_DECRYPT_SSLV3_MAC,
  SECLOG_OP_ENC_DES_DECRYPT_TLS10_MAC,
  SECLOG_OP_ENC_ARC4_ENCRYPT,
  SECLOG_OP_ENC_ARC4_ENCRYPT_SSLV3_MAC,
  SECLOG_OP_ENC_ARC4_ENCRYPT_TLS10_MAC,
  SECLOG_OP_ENC_ARC4_DECRYPT,
  SECLOG_OP_ENC_ARC4_DECRYPT_SSLV3_MAC,
  SECLOG_OP_ENC_ARC4_DECRYPT_TLS10_MAC,


  SECLOG_OP_ENC_3DES_ENCRYPT,
  SECLOG_OP_ENC_3DES_ENCRYPT_SSLV3_MAC,
  SECLOG_OP_ENC_3DES_ENCRYPT_TLS10_MAC,
  SECLOG_OP_ENC_3DES_DECRYPT,
  SECLOG_OP_ENC_3DES_DECRYPT_SSLV3_MAC,
  SECLOG_OP_ENC_3DES_DECRYPT_TLS10_MAC,


  /* Hashing Operations */
  SECLOG_OP_HSH_MD2_CONT = 0x40,
  SECLOG_OP_HSH_MD2_FINAL,
  SECLOG_OP_HSH_MD5_CONT,
  SECLOG_OP_HSH_MD5_FINAL,
  SECLOG_OP_HSH_SHA_CONT,
  SECLOG_OP_HSH_SHA_FINAL
} seclog_op_type; 

/***************************************************************************/
/***                 Common Security Services Log Types                  ***/
/***************************************************************************/
/*** All log types for the Crypto Layer contain an operation sequence    ***/
/*** number to allow correlation of log records by the processing tools. ***/
/*** Since Crypto Instances have a variety of functions, each main log   ***/
/*** record (statictics) contains the crypto operation performed.        ***/
/***************************************************************************/

/*** ModMath Log Types ***/
LOG_RECORD_DEFINE( LOG_SEC_PKX_MM_STATS_C )

  seclog_op_type op;                      /* Operation type (see above)    */
  uint8          op_seqno;                /* Sequence # for correlation    */
  uint16         duration_ms;             /* Time used for operation [ms]  */

  uint16         base_bit_len;            /* DATA: Bit length of base      */
  uint16         modulo_bit_len;          /* DATA: Bit length of modulo    */
  uint16         exponent_bit_len;        /* DATA: Bit length of exponent  */
  uint16         result_bit_len;          /* DATA: Bit length of result    */

LOG_RECORD_END


LOG_RECORD_DEFINE( LOG_SEC_PKX_MM_OP_C )

  uint8          op_seqno;                /* Sequence # for correlation    */

  uint8          base[128];               /* DATA: Base number             */
  uint8          modulo[128];             /* DATA: Modulo number           */
  uint8          exponent[128];           /* DATA: Exponent number         */
  uint8          result[128];             /* DATA: Resulting number        */

LOG_RECORD_END


/*** En- / decryption Log Types ***/
LOG_RECORD_DEFINE( LOG_SEC_ENC_STATS_C )

  seclog_op_type op;                      /* Operation type (see above)    */
  uint8          op_seqno;                /* Op Sequence # for correlation */
  uint16         duration_ms;             /* Time used for operation [ms]  */

  uint32         plaintext_bit_len;       /* DATA: Bit length of plaintext */ 
  uint32         ciphertext_bit_len;      /* DATA: Bit length of Ciphertxt */

LOG_RECORD_END 


LOG_RECORD_DEFINE( LOG_SEC_ENC_OP_DES_STATS_C )

  uint8          op_seqno;                /* Sequence # for correlation    */

  uint8          key[8];                  /* DATA: Ptr to key used         */
  uint8          cbc_state[8];            /* DATA: Last CBC block (64 bit) */

LOG_RECORD_END 


LOG_RECORD_DEFINE( LOG_SEC_ENC_OP_ARC4_STATS_C )

  uint8          op_seqno;                /* Sequence # for correlation    */

  uint16         sbox_index_1;            /* DATA: S-Box swapping index 1  */
  uint16         sbox_index_2;            /* DATA: S-Box swapping index 2  */
  uint8          sbox_data[256];          /* DATA: S-Box data (256 bytes)  */

LOG_RECORD_END


LOG_RECORD_DEFINE( LOG_SEC_ENC_OP_PLAINTEXT_C )

  uint8          op_seqno;                /* Sequence # for correlation    */
  uint8          frg_seqno;               /* Sequ. # for fragmentation     */

  uint8          data[SEC_MAX_LOG_SIZE];  /* DATA: Plaintext fragment     */

LOG_RECORD_END 


LOG_RECORD_DEFINE( LOG_SEC_ENC_OP_CIPHERTEXT_C )

  uint8          op_seqno;                /* Sequence # for correlation    */
  uint8          frg_seqno;               /* Sequ. # for fragmentation     */

  uint8          data[SEC_MAX_LOG_SIZE];  /* DATA: Ciphertext fragment    */

LOG_RECORD_END 


LOG_RECORD_DEFINE( LOG_SEC_ENC_OP_MAC_C )

  seclog_op_type hash_op;                 /* DATA: Hash algorithm used     */
  uint8          mac[20];                 /* DATA: Resulting MAC           */

LOG_RECORD_END


/*** Hashing Log Types ***/
LOG_RECORD_DEFINE( LOG_SEC_HSH_STATS_C )

  seclog_op_type op;                      /* Operation type (see above)    */
  uint8          op_seqno;                /* Sequence # for correlation    */
  uint16         duration_ms;             /* Time used for operation [ms]  */

  uint32         hash_data_bit_len;       /* DATA: Bit length of input     */

LOG_RECORD_END


LOG_RECORD_DEFINE( LOG_SEC_HSH_OP_INPUT_C )

  uint8          op_seqno;                /* Sequence # for correlation    */
  uint8          frg_seqno;               /* Sequ. # for fragmentation     */

  uint8          data[SEC_MAX_LOG_SIZE];  /* DATA: Hash input data         */

LOG_RECORD_END


LOG_RECORD_DEFINE( LOG_SEC_HSH_OP_HASH_C )

  uint8          op_seqno;                /* Sequence # for correlation    */

  uint8          hash[20];                /* DATA: Hash result data        */

LOG_RECORD_END

#endif   /* _SECCRYPTDIAG_H */





