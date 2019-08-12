#ifndef MCCSEC_H
#define MCCSEC_H


/*===========================================================================

DESCRIPTION
  This file contains data and function declarations necessary for the
  CDMA 3G AKA, Message Integrity and Encryption support.


  Copyright (c) 2004 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/mccsec.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/04/12   ppr     Feature Cleanup
05/17/06   fh      Merged in new development of AKA/AES features

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "1x_variation.h"
#include "comdef.h"


#if defined(FEATURE_IS2000_REL_A_AES)


/* <EJECT> */
/*===========================================================================

                        DECLARATIONS

===========================================================================*/

/* --------------------------------------------------------------------------
** Constants used in the AKA functions. See TR45.AHAG, Enhanced Cryptographic
** Algorithms, Rev B.
** ------------------------------------------------------------------------*/

#define MCCSEC_AKA_FMK          "AHAG"      /* Family Key = 0x41484147     */
#define MCCSEC_F0               0x41        /* F0 Type ID                  */
#define MCCSEC_F1               0x42        /* F1 Type ID                  */
#define MCCSEC_F1_STAR          0x43        /* F1* Type ID                 */
#define MCCSEC_F2               0x44        /* F2 Type ID                  */
#define MCCSEC_F3               0x45        /* F3 Type ID                  */
#define MCCSEC_F4               0x46        /* F4 Type ID                  */
#define MCCSEC_F5               0x47        /* F5 Type ID                  */
#define MCCSEC_F5_STAR          0x48        /* F5* Type ID                 */

/* Delta to check against SQN wrap around, suggested value: 2 ^ 28. See    */
/* "3GPP TS 33.1.2 V6.10, 3G Security, Security Architecture", C3.1        */
#define MCCSEC_DELTA            0x10000000L

/* L value to limit the age of the SQN. suggested value ???                */
#define MCCSEC_L                0x010000L

#define MCCSEC_ALPHA_LEN        5           /* Last 5 bits of SQN are IND  */

#define MCCSEC_SEQ_MS_ARR_SIZE  (0x01 << (MCCSEC_ALPHA_LEN))

#define MCCSEC_ALPHA_BIT_MASK   0x1F        /* Bit mask of the Alpha */

//#define MCCSEC_SALT

/* CK_THRESHOLD to limit the life time of Cipher key - 24 bits */
#define MCCSEC_DEFAULT_CK_THRESHOLD   0xFFFFFFL

#define MCCSEC_SSEQ_H_LEN       3           /* SSEQ_H length: 3 bytes      */

#define MCCSEC_CMEA_KEY_LEN     8           /* CMEA key length             */

#define MCCSEC_RES_LEN_LEN      1           /* RES_LEN field length, 1 byte*/

/* Authentication Request Message processing status */
typedef enum
{
  MCCSEC_SQN_SUCCESS = 0,         /* SQN test successful */
  MCCSEC_SQN_ERR_DELTA,           /* SEQ - SEQms > Delta */
  MCCSEC_SQN_ERR_L,               /* SEQms - SEQ > L */
  MCCSEC_SQN_ERR_SEQ_MS,          /* SEQ < SEQ_ARR[ind] */
  MCCSEC_SQN_ERR_NV_WRITE         /* SEQ NV write failure */

} mccsec_sqn_test_status_type;


/* Return the high 32 bits of a uint64 data variable */
#define MCCSEC_UINT64_HI( arg64_addr ) \
  (*(((uint32 *) arg64_addr ) + 1 ))

/* Return the low 32 bits of a uint64 data variable */
#define MCCSEC_UINT64_LO( arg64_addr ) \
  (*((uint32 *) arg64_addr ))

/* calculates the ack index value */
#define MCCSEC_GET_ACK_INDEX(ack_req) ((ack_req) ? 1 : 0)



#define MCCSEC_MSG_INT_KEY_SIZE_IN_BYTES 16


/* Length of EXT_SSEQ in bytes. 32 bits */
#define MCCSEC_EXT_SSEQ_LENGTH 4


/* set of key id's. */
#define MCCSEC_KEY_ID_SIZE 4


/* set of ack required values */
#define MCCSEC_ACK_REQ_SIZE 2


/* Initializer for duplicate detection map.
   This map could be 8 bits or 4 bits
*/
#define MCCSEC_DUP_MAP_INIT_VAL 0xfe


/* increment of 24 bit value of SSEQ which triggers its write to NV */
#define MCCSEC_SSEQ_H_INC_FOR_NV_WRITE 1


typedef enum
{
  MCCSEC_SSEQ_STATUS_NULL,
  MCCSEC_SSEQ_OK_24_BIT_VALUE,
  MCCSEC_SSEQ_OK_FUTURE_SEQ_NUM,
  MCCSEC_SSEQ_OK_REPLAY_WINDOW,
  MCCSEC_SSEQ_DUPLICATE,
  MCCSEC_SSEQ_OUT_OF_RANGE

} mccsec_sseq_validation_status_type;


typedef struct
{
  /* status of sseq validation */
  mccsec_sseq_validation_status_type sseq_status;


  /* calculated 32 bit crypto sync value */
  uint32 ext_sseq;


  /* LSB's, 8 bit value of v from rx_ext_sseq for updating
     duplicate detection map
  */
  uint8 v;

} mccsec_fwd_sseq_info_type;


typedef struct
{
  /* The indication if BS supports the message integrity.
  ** It can be set by ANSI-41 SPM and UHDM.
  */
  boolean msg_integrity_sup;

  /* The MI algorithm other than the default one that BS supports */
  uint8   sig_integrity_sup;

} mccsec_bs_security_cap_type;


/*===========================================================================

                      EXTERNAL VARIABLE DECLARATIONS

===========================================================================*/
/* ext_sseq. This is the cryto sync for receive and transmit */
extern uint32 mccsec_tx_ext_sseq[MCCSEC_ACK_REQ_SIZE][MCCSEC_KEY_ID_SIZE];
extern uint32 mccsec_rx_ext_sseq[MCCSEC_ACK_REQ_SIZE][MCCSEC_KEY_ID_SIZE];

/* <EJECT> */
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/



/*===========================================================================

FUNCTION MCCSEC_POWERUP_LOAD

DESCRIPTION
  This functions loads the AKA related items from NV upon powerup. If some
  items are not active in NV, it assigns them the default values. It also
  initializes variables for MCCSEC.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If NV items are loaded successfully.
  FALSE - Otherewise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean mccsec_powerup_load
(
  byte      cur_nam
);


/*===========================================================================

FUNCTION mccsec_init_sseq

DESCRIPTION
  Initializes crpto sync for all key id's, and when ack_req bit is set
  and not set. Initializes duplicate detection map.

  Also sets the flag indicating that crypto sync just got initialized.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void mccsec_init_sseq(

  /* The crypto sync value to initialize to. This is 24 bit number.
     e.g. 0x00FADE7C where FADE7C is the 24 bit value. */
  uint32 new_sseq_h,

  uint8  key_id

  );


/*===========================================================================

FUNCTION MCCSEC_COPY_SSEQ

DESCRIPTION
  This function copies the SSEQ in the array from source key_id to the
  destination key_id.

DEPENDENCIES
  This is mainly used in the 3G to 2G One-way roaming.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsec_copy_sseq
(
  uint8   dest_key_id,
  uint8   src_key_id
);




/*===========================================================================

FUNCTION MCCSEC_SAVE_SSEQ

DESCRIPTION
  This function saves the SSEQ into the cache for the SID/NID change. Note
  the previous saved SSEQ will be overwritten.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsec_save_sseq ( void );


/*===========================================================================

FUNCTION MCCSEC_SWAP_SSEQ

DESCRIPTION
  This function swaps the previous saved SSEQ from cache and the current
  SSEQ, for restoring the SSEQ during a SID/NID change.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsec_swap_sseq ( void );

/*===========================================================================

FUNCTION MCCSEC_GET_NEW_SSEQ_H

DESCRIPTION
  This function returns the next SSEQ_H to be proposed in the ROP, Security
  Mode Request Message, and Authentication Response Message.

DEPENDENCIES
  None.

RETURN VALUE
  The new_sseq_h value is returned.

SIDE EFFECTS
  None.

===========================================================================*/
uint32  mccsec_get_new_sseq_h
(
  void
);


/*===========================================================================

FUNCTION MCCSEC_BS_SUPPORTS_MI

DESCRIPTION
  This function returns whether BS supports Message Integrity.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If BS supports the Message Integrity.
  FALSE - If BS doesn't support the Message Integrity.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean mccsec_bs_supports_mi
(
  void
);


/*===========================================================================

FUNCTION MCCSEC_SET_BS_MI_CAP

DESCRIPTION
  This function sets the base station's Message Integrity capability.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void mccsec_set_bs_mi_cap
(
  boolean mi_supported
);


/*===========================================================================

FUNCTION MCCSEC_WRITE_SSEQ_TO_NV

DESCRIPTION
  This function writes the SSEQ value to NV.

DEPENDENCIES
  None.

RETURN VALUE
  NV write status.

SIDE EFFECTS
  None.

===========================================================================*/
nv_stat_enum_type mccsec_write_sseq_to_nv
(
  uint32  sseq
);


/*===========================================================================

FUNCTION MCCSEC_PRINT_KEY

DESCRIPTION
  This function print out the key information.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void mccsec_print_key
(
  const uint8   *key_ptr
);

/*===========================================================================

FUNCTION mccsec_update_rx_ext_sseq

DESCRIPTION
  Updates rx_ext_sseq for the supplied key id and ack_req bit only.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void mccsec_update_rx_ext_sseq(

  boolean ack_req,
    /* Acknowledgement required bit */

  uint8 key_id,
    /* key id to be used for indexing */

  uint32 ext_sseq
    /* crypto sync value to update to */

  );


/*===========================================================================

FUNCTION mccsec_get_sseq_dup_detection_map

DESCRIPTION
  Returns 8 bit duplicate detection map for the supplied key id
  and ack_req bit.

DEPENDENCIES
  None.

RETURN VALUE
  8 bit duplicate detection map

SIDE EFFECTS
  None.

===========================================================================*/
uint8 mccsec_get_sseq_dup_detection_map(

  /* Acknowledgement required bit */
  boolean ack_req,

  /* key id to be used for indexing */
  uint8 key_id

  );


/*===========================================================================

FUNCTION mccsec_update_sseq_dup_detection_map

DESCRIPTION
  Updates 8 bit duplicate detection map.

  This function implements the suggested algorithm from the standard.

  Here is an excerpt from the footnote #15 at the end of the section
  "2.3.12.4.1.5 Duplicate Detection of Security Sequence Number"

    A simple implementation is for the receiver to maintain an N-bit bitmap,
    w, to represent the sequence numbers in segment #1. The order of w is
    most significant bit first (i.e., w[N-1], w[N-2],..., w[1], w[0]).
    Let w[0] represent V, w[1] represent ((V - 1) mod 256), etc.
    w[i] = '1' means sequence number i has already been received.
    When a message with sequence number i has been decrypted successfully,
    w[i] is set to '1'. Whenever V moves from V1 to V2,
    left-shift w ((V2 - V1) mod 256) times. Each time w is left-shifted,
    stuff a '0' at the eight end of w. After all the shifting,
    set w(0) to '1'.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void mccsec_update_sseq_dup_detection_map(

  /* Acknowledgement required bit */
  boolean ack_req,

  /* key id to be used for indexing */
  uint8 key_id,

  /* 8 bits of SDU_SSEQ from the L2 header of the incoming message */
  uint8 sdu_sseq,

  /* sseq info */
  const mccsec_fwd_sseq_info_type * sseq_info

);

/*===========================================================================

FUNCTION mccsec_get_rx_ext_sseq

DESCRIPTION
  Returns the current value of rx_ext_sseq associated with the
  supplied ack_req bit and key id.

  It does NOT modify rx_ext_sseq.

DEPENDENCIES
  None.

RETURN VALUE
  32 bit crypto sync value.

SIDE EFFECTS
  None.

===========================================================================*/
uint32 mccsec_get_rx_ext_sseq(

  boolean ack_req,
    /* Acknowledgement required bit */

  uint8 key_id
    /* key id to be used for indexing */

  );

/*===========================================================================

FUNCTION mccsec_get_tx_ext_sseq

DESCRIPTION
  Returns the current value of tx_ext_sseq associated with the
  supplied ack_req bit and key id.

  It does NOT modify tx_ext_sseq.

DEPENDENCIES
  None.

RETURN VALUE
  32 bit crypto sync value

SIDE EFFECTS
  None.
===========================================================================*/
uint32 mccsec_get_tx_ext_sseq(

  boolean ack_req,
    /* Acknowledgement required bit */

  uint8 key_id
    /* key id to be used for indexing */

  );


/*===========================================================================

FUNCTION mccsec_get_and_inc_tx_ext_sseq

DESCRIPTION
  Returns current value of tx_ext_sseq and then increments it by 1.

  i.e. it saves the current tx_ext_sseq in a local variable, then increments
  tx_ext_sseq and returns unchanged value from the local variable.

DEPENDENCIES
  None.

RETURN VALUE
  32 bit crypto sync value.

SIDE EFFECTS
  tx_ext_sseq is incremented by 1.

===========================================================================*/
uint32 mccsec_get_and_inc_tx_ext_sseq(

  boolean ack_req,
    /* Acknowledgement required bit */

  uint8 key_id
    /* key id to be used for indexing */

  );

/*===========================================================================

FUNCTION mccsec_get_sseq_validation_info

DESCRIPTION
  Performs SSEQ validation and returns its status. Also constructs the 32 bit
  crypto sync to use for MACI validation.

DEPENDENCIES
  None.

RETURN VALUE
  returns mccsec_fwd_sseq_info_type structure with SSEQ
  validation information.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsec_get_sseq_validation_info(

  uint8 sdu_sseq,
    /* 8 bits of SDU_SSEQ from the L2 header of the incoming message */

  uint8 sdu_key_id,
    /* key id included in the received message */

  boolean ack_req,
    /* Whether this message requires an Ack to be sent back or not */

  mccsec_fwd_sseq_info_type * sseq_info
    /* gathered sseq info. This is the output variable */

  );

/*===========================================================================

FUNCTION mccsec_get_cs_init_info

DESCRIPTION
  Returns whether crypto sync is initialized or not.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - if crypto sync just got initialized
  FALSE - otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccsec_get_cs_init_info(void);

/*===========================================================================

FUNCTION mccsec_set_cs_init_info

DESCRIPTION
  Sets the flag indicating that crypto sync just got initialized

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void mccsec_set_cs_init_info(

  boolean cs_init_flag

  );





/*===========================================================================

FUNCTION MCCSEC_LOG_DATA

DESCRIPTION
  This function logs arbitrary byte arrays.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mccsec_log_data
(
  const uint8*                          pdata,
  uint16                          plen
);



/* provide an access function for other modules to use */
/*===========================================================================

FUNCTION get_sseq_validation_status

DESCRIPTION
  This function returns the SSEQ validation status

DEPENDENCIES
  None

RETURN VALUE
  an integer value of SSEQ validation status

SIDE EFFECTS
  buf_pos updated.

===========================================================================*/

mccsec_sseq_validation_status_type get_sseq_validation_status(void);

#endif /* defined(FEATURE_IS2000_REL_A_REL_A_AES) */

#endif /* MCCSEC_H */
