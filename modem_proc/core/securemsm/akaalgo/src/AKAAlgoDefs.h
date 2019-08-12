#ifndef AKAALGODEFS_H
#define AKAALGODEFS_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  A K A   Definitions                
                    
   
   
GENERAL DESCRIPTION

  This is the module for 3G Authentication and Key Agreement, 

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2008 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //components/rel/core.mpss/3.5.c12.3/securemsm/akaalgo/src/AKAAlgoDefs.h#1 $
$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
/* --------------------------------------------------------------------------
** Constants used in the AKA functions. See TR45.AHAG, Enhanced Cryptographic
** Algorithms, Rev B.
** ------------------------------------------------------------------------*/
#define L_RAND   16    /*128      bits 3G TS 33.105.v.3.0(2000-03)*/
#define L_FMK     4    /*32       bits */
#define L_SQN     6    /*48       bits 3G TS 33.105.v.3.0(2000-03)*/
#define L_AMF     2    /*16       bits 3G TS 33.105.v.3.0(2000-03)*/
#define L_MACA    8    /*64       bits 3G TS 33.105.v.3.0(2000-03)*/
#define L_MACS    8    /*64       bits 3G TS 33.105.v.3.0(2000-03)*/
#define L_RES    16    /*32<->128 bits 3G TS 33.105.v.3.0(2000-03)*/
#define L_MILENAGE_RES    8    /*64 bits 3G TS 35.206.v8.0.0.0(2008-12)*/
#define L_CK     16    /*128      bits 3G TS 33.105.v.3.0(2000-03)*/
#define L_IK     16    /*128      bits 3G TS 33.105.v.3.0(2000-03)*/
#define L_AK      6    /*48       bits 3G TS 33.105.v.3.0(2000-03)*/
#define L_AKS     6    /*48       bits 3G TS 33.105.v.3.0(2000-03)*/
#define L_UAK    16

#define AKAALGO_AKA_FMK          "AHAG"      /* Family Key = 0x41484147     */
#define AKAALGO_F0               0x41        /* F0 Type ID                  */ 
#define AKAALGO_F1               0x42        /* F1 Type ID                  */
#define AKAALGO_F1_STAR          0x43        /* F1* Type ID                 */
#define AKAALGO_F2               0x44        /* F2 Type ID                  */
#define AKAALGO_F3               0x45        /* F3 Type ID                  */
#define AKAALGO_F4               0x46        /* F4 Type ID                  */
#define AKAALGO_F5               0x47        /* F5 Type ID                  */
#define AKAALGO_F5_STAR          0x48        /* F5* Type ID                 */

/* Delta to check against SQN wrap around, suggested value: 2 ^ 28. See    */
/* "3GPP TS 33.1.2 V6.10, 3G Security, Security Architecture", C3.1        */
#define AKAALGO_DELTA            0x10000000L

/* L value to limit the age of the SQN. suggested value ???                */
#define AKAALGO_L                0x010000L

#define AKAALGO_ALPHA_LEN        5           /* Last 5 bits of SQN are IND  */

#define AKAALGO_SEQ_MS_ARR_SIZE  (0x01 << (AKAALGO_ALPHA_LEN))

#define AKAALGO_ALPHA_BIT_MASK   0x1F        /* Bit mask of the Alpha */

//#define AKAALGO_SALT             

/* CK_THRESHOLD to limit the life time of Cipher key - 24 bits */
#define AKAALGO_DEFAULT_CK_THRESHOLD   0xFFFFFFL

#define AKAALGO_SSEQ_H_LEN       3           /* SSEQ_H length: 3 bytes      */

#define AKAALGO_CMEA_KEY_LEN     8           /* CMEA key length             */

#define AKAALGO_RES_LEN_LEN      1           /* RES_LEN field length, 1 byte*/

/* Authentication Request Message processing status */
typedef enum
{
  AKAALGO_SQN_SUCCESS = 0,         /* SQN test successful */
  AKAALGO_SQN_ERR_DELTA,           /* SEQ - SEQms > Delta */
  AKAALGO_SQN_ERR_L,               /* SEQms - SEQ > L */
  AKAALGO_SQN_ERR_SEQ_MS,          /* SEQ < SEQ_ARR[ind] */
  AKAALGO_SQN_ERR_NV_WRITE         /* SEQ NV write failure */

} akaalgo_sqn_test_status_type;


/* Return the high 32 bits of a uint64 data variable */
#define AKAALGO_UINT64_HI( arg64_addr ) \
  (*(((uint32 *) arg64_addr ) + 1 ))

/* Return the low 32 bits of a uint64 data variable */
#define AKAALGO_UINT64_LO( arg64_addr ) \
  (*((uint32 *) arg64_addr ))

/* calculates the ack index value */
#define AKAALGO_GET_ACK_INDEX(ack_req) ((ack_req) ? 1 : 0)



#define AKAALGO_MSG_INT_KEY_SIZE_IN_BYTES 16
                      

/* Length of EXT_SSEQ in bytes. 32 bits */
#define AKAALGO_EXT_SSEQ_LENGTH 4


/* set of key id's. */
#define AKAALGO_KEY_ID_SIZE 4


/* set of ack required values */
#define AKAALGO_ACK_REQ_SIZE 2


/* Initializer for duplicate detection map. 
   This map could be 8 bits or 4 bits 
*/
#define AKAALGO_DUP_MAP_INIT_VAL 0xfe


/* increment of 24 bit value of SSEQ which triggers its write to NV */
#define AKAALGO_SSEQ_H_INC_FOR_NV_WRITE 1


/* ------------------------------------------------------------------
  Authentication Request Fields. The PACKED format is what passed to
  MCCSEC or RUIM for processing the AKA.
-------------------------------------------------------------------*/ 
typedef struct
{
  uint64                            randa_hi; /* Random Challenge Num, High */
  uint64                            randa_lo; /* Random Challenge Num, Low  */

  uint64                            con_sqn;  /* Concealed SQN */

  uint16                            amf;      /* Auth Management Field */

  uint64                            mac_a;    /* MAC in Auth Vector */

} akaalgo_auth_req_type;

/* ------------------------------------------------------------------
  Authentication Resynchronization Fields. The PACKED format is what
  passed from MCCSEC or RUIM to the MCCAKA in response to our 
  Authentication Request.
-------------------------------------------------------------------*/
typedef struct
{
  uint64                            con_ms_sqn;     /* Concealed MS SQN */
  uint64                            mac_s;          /* MAC for Auth Resync */

} akaalgo_auth_resync_type;

typedef PACKED struct
{
  uint8                             status;         /* Auth Resync status */
  uint8                             con_ms_sqn[L_SQN]; /* Concealed MS SQN */
  uint8                             mac_s[L_MACS];  /* MAC for Auth Resync */

} akaalgo_packed_auth_resync_type;

/* ------------------------------------------------------------------
  Authentication Response Fields. The PACKED format is what passed
  from MCCSEC or RUIM to the MCCAKA in response to the Authentication
  Request.
-------------------------------------------------------------------*/
typedef struct
{
  uint8                             res_len;        /* RES length */
  uint64                            res_hi;         /* RES Hi */
  uint64                            res_lo;         /* RES Lo */

} akaalgo_auth_resp_type;

typedef PACKED struct
{
  uint8                             status;         /* Auth Resp status */
  uint8                             ck[L_CK];       /* Cipher Key */
  uint8                             ik[L_IK];       /* Integrity Key */
  uint8                             res_len;        /* RES length */
  uint8                             res[L_RES];     /* RES */
} akaalgo_packed_auth_resp_type;

typedef PACKED struct
{
  uint8                           status; /* MAC_A validation failure status */

} akaalgo_packed_maca_val_type;


typedef PACKED union
{
  akaalgo_packed_maca_val_type     maca;             /* MACA Validation */
  akaalgo_packed_auth_resync_type  resync;           /* Auth Resynchronization */
  akaalgo_packed_auth_resp_type    resp;             /* Auth Response */

} akaalgo_resp_buffer;





typedef struct
{
  /*-----------------------------------------------------------------
    The following variables hold essential information for the
    AKA processing.
  -----------------------------------------------------------------*/
  uint8                 fmk[L_FMK];         /* The AKA Family Key. */
  uint8                 k_key[L_KEY];       /* K key */
  
  uint64                seq_arr[SEQ_MS_ARR_SIZE_MAX]; /* SEQms Array, size 32 */
  uint32                delta;              /* DELTA to protect against SQN wrap around */
  uint64                l_val;              /* L value */

  uint8                 status;             /* AKA process status */
  

  uint8                 randa[L_RAND];      /* Random challenge */
  uint8                 con_sqn[L_SQN];     /* Concealed SQN, received from BS */
  uint8                 amf[L_AMF];         /* Auth Management Field */
  uint8                 mac_a[L_MACA];      /* MAC in Auth Vector */

  /* Fields for Authenticaton Resync due to SQN test failure */
  uint8                 con_ms_sqn[L_SQN];  /* Concealed SQN */

  /*-----------------------------------------------------------------
    The following variables are used as scratch pad to hold temporary
    or intermediate values. They are included here to avoid the stack
    overflow when declared inside the functions.
  -----------------------------------------------------------------*/
  uint64                sqn_ms;             /* SQNms - Largest element of SQN array */
  uint8                 sqn_ms_barr[L_SQN]; /* SQNms - in the byte array */

  uint64                seq_ms;             /* SEQms - Largest element of SEQ array */

  uint8                 ak[L_AK];           /* Anonymity Key */
  uint8                 sqn[L_SQN];         /* SQN received from BS */
  uint64                seq;                /* SEQ received from BS */
  uint8                 xmac[L_MACA];       /* Expected MAC_A */
  uint32      		    algo_id;	        /* SHA1 or MILENAGE */
  boolean               is_init;            /* is initialized or not*/
} akaalgo_params_type;

#endif /* AKAALGODEFS_H */
