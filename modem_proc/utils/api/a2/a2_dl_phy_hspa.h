#ifndef A2_DL_PHY_HSPA_H
#define A2_DL_PHY_HSPA_H

/*!
  @file a2_dl_phy_hspa.h

  @brief
  The interface to the downlink phy portion of the a2 driver for HSPA.

  -# Initialization
    -# a2_dl_phy_hspa_init()
      -# Initializes a2 dl phy hspa related variables
      -# One time initialization (can be done at task bring up)
    -# a2_dl_phy_hspa_register_dma_from_dob_and_decipher_cb()
      -# Initialize call back function to be called once data is moved from
         DOB to external memory
      -# One time registration (can be done at task bring up)
    -# a2_dl_phy_hspa_register_gather_and_decipher_cb():
      -# Initialize call back function to be called once data is moved from
         external memory to external memory (Partial PDU handling)
      -# One time registration (can be done at task bring up)
    -# a2_dl_phy_hspa_set_technology()
      -# Needs to be called whenever UE enters UMTS or HSPA


  -# Reading data from DOB
    -# Expected Function call sequence in header read ISR
      -# Step 1..n: a2_dl_phy_hspa_dma_from_dob_and_decipher()
        -# Moves the data from one DOB location to one external memory location
           and optionally perform ciphering based on the ciphering algo argument
        -# This function call needs to be repeated for all complete/partial RLC
           PDUs sitting in DOB
      -# Step n+1: a2_dl_phy_hspa_notify_gather_and_decipher()
        -# Insert a tag to indicate this transaction
        -# Indicated tag/transaction_id will be returned as part of the call
           back function a2_dl_phy_hspa_dma_from_dob_and_decipher_cb() which
           was registered during initialization
      -# Step n+2: a2_dl_phy_hspa_commit(boolean)
        -# Triggers A2 HW to start DMA and deciphering
        -# This function also indicates A2 HW whether interrupt is needed or not
           after all the DMA tasks are done.

  -# Expected Partial PDU handling
    -# Function call sequence for partial pdu handling
      -# Step 1: a2_dl_phy_hspa_gather_and_decipher()
        -# Moves data from multiple source to one destination and optionally
           perform ciphering based on the ciphering algo argument
      -# Step 2: a2_dl_phy_hspa_notify_gather_and_decipher()
        -# Indicated tag/transaction_id will be returned as part of the call
           back function a2_dl_phy_hspa_gather_and_decipher_cb() which was
           registered during initialization
      -# Step 3: a2_dl_phy_hspa_commit(boolean)
        -# Triggers A2 HW to start DMA and deciphering
        -# This function also indicates A2 HW whether interrupt is needed or not
           after all the DMA tasks are done.
      Note: If needed MAC/L1 can always set one tag for multiple p-to-c's

  -# Miscellaneous APIs
    -# a2_dl_phy_hspa_get_status()
      -# In case MAC/L1 wants to avoid interrupt then this function can be used
         to read the status queue as part of next TB decode done interrupt or
         after some timer expiry

  @ingroup a2_ext_interface
*/

/*==============================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/api/a2/a2_dl_phy_hspa.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/06/11   ar      added a2 power collapse support
09/09/11   ar      revert CR 295754 fix as its breaking basic data calls
07/12/11   ar      CR 295754 fix: process all individual tech tbs in individual 
                   tech ISRs
12/11/09   sm      Updated A2_DL_PHY_HSPA_MAX_SRC_IOVEC_COUNT from 50 to 70
11/17/09   ar      fixed Q6 compiler warnings
11/02/09   ar      removed a2_dl_phy_hspa_set_technology()
10/04/09   sm      Removed feature FEATURE_A2_DL_PHY_HSPA_TEST
04/26/09   sm      Initial version


==============================================================================*/


/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <dsm.h>
#include <a2_common.h>
#include <comdef.h>
#include <a2_dl_phy.h>

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*==============================================================================

                             MACROS

==============================================================================*/

/*!
  Maximum number of source iovec (address+length) count per DMA chain
  */
#define A2_DL_PHY_HSPA_MAX_SRC_IOVEC_COUNT A2_MAX_SRC_IOVEC_COUNT_PER_DMA_CHAIN

typedef void (*a2_dl_phy_hspa_dma_from_dob_and_decipher_cb)(uint32);

typedef void (*a2_dl_phy_hspa_gather_and_decipher_cb)(uint32);

typedef void (*a2_dl_phy_hspa_dma_to_dob_and_cipher_cb)(uint32);

/*==============================================================================

                       STRUCTURES AND UNIONS

==============================================================================*/

/*!
   @brief
  structure to hold ciphering information

  @note
  If cipher_algo is set to A2_CIPHER_ALGO_NONE then remaining fields are dont
  care.
*/
typedef struct
{
  /*! The type of ciphering algorithm to use. If set to A2_CIPHER_ALGO_NONE
      then data will not be ciphered and remaining fields will be ignored */
  a2_cipher_algo_e algo;
  /*! Ciphering key index. Valid values: 0..5 */
  uint8            key_index;
  /*! radio_bearer_id minus 1. Valid values: 0..31 */
  uint8            bearer_id;
  /*! The count-c to be used for ciphering */
  uint32           count_c;
} a2_dl_phy_hspa_cipher_info_t;


/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/* #ifdef FEATURE_A2_DL_PHY_HSPA_TEST */

/*==============================================================================

  FUNCTION:  a2_dl_phy_hspa_register_dma_to_dob_and_cipher_cb

==============================================================================*/
/*!
  @brief
  Registers a callback function to be called whenever dma_from_dob_and_decipher
  tag status is processed in the DL PHY status queue.

  @note
  Registered callback can be used to send a signal to a given task to process
  transfer done associated with the *tag* returned as part of the call back
  function argument.
  Processing can be done in call back function context as well with the
  necessary mutex protection (if needed).

  @caller
  WL1 (during task initialization)

*/
/*============================================================================*/
void a2_dl_phy_hspa_register_dma_to_dob_and_cipher_cb
(
  /*! The callback to be called whenever dma_from_dob_and_decipher tag status
      is received */
  a2_dl_phy_hspa_dma_to_dob_and_cipher_cb dob_tag_cb_fn_ptr
);

/*==============================================================================

  FUNCTION:  a2_dl_phy_hspa_dma_to_dob_and_cipher

==============================================================================*/
/*!
    @brief
    Adds following tasks to A2 DL PHY TASK Q:
     - CIPHER TASK (if cipher_algo is not set to A2_CIPHER_ALGO_NONE)
     - DMA TASK (one source  <DecOB> and one destination <external memory>)
       - src_addr and src_offset makes a bit aligned DecOB source address

    @note
    This function takes care of necessary cache flushing/invalidation of source/
    destination.
    This functions assumes that no virtual to physical mapping is needed for
    the address provided as part of the arguments i.e. virtual and physical
    memory address is same.

    @caller
    HSPA Header Read ISR

    @return
    None
*/
/*============================================================================*/
void a2_dl_phy_hspa_dma_to_dob_and_cipher
(
  /*! pointer to ciphering info */
  a2_dl_phy_hspa_cipher_info_t *cipher_info_ptr,
  /*! source address (external memory) of the data to be moved to DecOB. */
  uint32                  src_addr,
  /*! source length in bytes */
  uint16                  src_len_in_bytes,
  /*! destination address (DecOB) where deciphered data needs to be copied.
      Length to be copied to destination is assumed to be same as
      "src_len_in_bytes" . This can be relative address (start of DOB addr as
      address 0) or physical address */
  uint32                  dest_dob_addr,
  /*! offset in bits (0..7) from dest_dob_addr. Offset in bits within the start
      Byte. Offset is measured from MSBit */
  uint16                  dest_offset_in_bits
);

/*==============================================================================

  FUNCTION:  a2_dl_phy_hspa_notify_dma_to_dob_and_cipher

==============================================================================*/
/*!
    @brief
    Write TAG task to A2 DL PHY with argument "user_data".
    Once "user_data" is received in the status queue then call back function
    pointer a2_dl_phy_hspa_dma_to_dob_and_cipher_tag_cb() is called with
    argument"user_data".

    @return
    None
*/
/*============================================================================*/
void a2_dl_phy_hspa_notify_dma_to_dob_and_cipher(uint32 user_data);

/* #endif #ifdef FEATURE_A2_DL_PHY_HSPA_TEST */

/*==============================================================================

  FUNCTION:  a2_dl_phy_hspa_init

==============================================================================*/
/*!
    @brief
    Initializes variables associated with hspa init block

    @caller
    A2 Init (during A2 driver initialization)

    @return
    None
*/
/*============================================================================*/
void a2_dl_phy_hspa_init ( void );

/*==============================================================================

  FUNCTION:  a2_dl_phy_hspa_register_dma_from_dob_and_decipher_cb

==============================================================================*/
/*!
  @brief
  Registers a callback function to be called whenever dma_from_dob_and_decipher
  tag status is processed in the DL PHY status queue.

  @note
  Registered callback can be used to send a signal to a given task to process
  transfer done associated with the *tag* returned as part of the call back
  function argument.
  Processing can be done in call back function context as well with the
  necessary mutex protection (if needed).

  @caller
  WL1 (during task initialization)

*/
/*============================================================================*/
void a2_dl_phy_hspa_register_dma_from_dob_and_decipher_cb
(
  /*! The callback to be called whenever dma_from_dob_and_decipher tag status
      is received */
  a2_dl_phy_hspa_dma_from_dob_and_decipher_cb dma_from_dob_cb_fn_ptr
);

/*==============================================================================

  FUNCTION:  a2_dl_phy_hspa_register_gather_and_decipher_cb

==============================================================================*/
/*!
  @brief
  Registers a callback function to be called whenever gather_and_decipher tag
  status is processed in the DL PHY status queue.

  @note
  This callback can be used to send a signal to a given task to process transfer
  done associated with the *tag* returned as part of the call back function
  argument.
  Processing can be done in interrupt context as well with the necessary mutex
  protection.

  @caller
  WL1 (during task initialization)
*/
/*============================================================================*/
void a2_dl_phy_hspa_register_gather_and_decipher_cb
(
  /*! The callback to be called whenever gather_and_decipher tag status
      is received */
  a2_dl_phy_hspa_gather_and_decipher_cb gather_cb_fn_ptr
);


/*==============================================================================

  FUNCTION:  a2_dl_phy_hspa_dma_from_dob_and_decipher

==============================================================================*/
/*!
    @brief
    Adds following tasks to A2 DL PHY TASK Q:
     - CIPHER TASK (if cipher_algo is not set to A2_CIPHER_ALGO_NONE)
     - DMA TASK (one source  <DecOB> and one destination <external memory>)
       - src_addr and src_offset makes a bit aligned DecOB source address

    @note
    This function takes care of necessary cache flushing/invalidation of source/
    destination.
    This functions assumes that no virtual to physical mapping is needed for
    the address provided as part of the arguments i.e. virtual and physical
    memory address is same.

    @caller
    HSPA Header Read ISR

    @return
    None
*/
/*============================================================================*/
void a2_dl_phy_hspa_dma_from_dob_and_decipher
(
  /*! pointer to ciphering info */
  a2_dl_phy_hspa_cipher_info_t *cipher_info_ptr,
  /*! DecOB source address of the data to be moved from DecOB. This can be
      relative address (start of DOB addr as address 0) or physical address */
  uint32                  src_dob_addr,
  /*! offset in bits (0..7) from src_addr. Offset in bits within the start
      Byte. Offset is measured from MSBit */
  uint16                  src_offset_in_bits,
  /*! source length in bytes */
  uint16                  src_len_in_bytes,
  /*! destination address (external memory/dsm item memory) where deciphered
      data needs to be copied. Length to be copied to destination is assumed
      to be same as "src_len_in_bytes" */
  uint32                  dest_addr
);


/*==============================================================================

  FUNCTION:  a2_dl_phy_hspa_gather_and_decipher

==============================================================================*/
/*!
    @brief
    Gathers data from multiple location to one location and if needed performs
    deciphering.

    @detail
    Add following tasks to A2 DL PHY TASK Q:
     - CIPHER TASK (only if cipher_algo is not set to A2_CIPHER_ALGO_NONE)
     - DMA TASK (to gather from multiple sources <external memory> to one
       destination <external memory>)

    @note
    This function takes care of necessary cache flushing/invalidation of
    source(s) and destination.
    This functions assumes that no virtual to physical mapping is needed for
    the address provided as part of the arguments i.e. virtual and physical
    memory address is same.

    @caller
    Either "HSPA Header Read ISR" or "MAC DL HS"

    @return
    None
*/
/*============================================================================*/
void a2_dl_phy_hspa_gather_and_decipher
(
  /*! pointer to ciphering info */
  a2_dl_phy_hspa_cipher_info_t *cipher_info_ptr,
  /*! number of source iovec's valid in src_iovec_list[] from which data needs
      to be gathered. 0 is invalid value. */
  uint16                 num_src_iovec,
  /*! source iovec list. Address shall be not NULL and length shall be non zero
      in first "num_src_iovec" array elements */
  a2_iovec_t             src_iovec_list[A2_DL_PHY_HSPA_MAX_SRC_IOVEC_COUNT],
  /*! destination address (external memory/dsm item memory) where deciphered
      data needs to be copied. Length to be copied to destination is assumed
      to be sum of "length_in_bytes" in valid "src_iovec_list" entries.
      "dest_addr" shall not be set to NULL */
  uint32                 dest_addr,
  /*! destination length in bytes */
  uint32                 dest_length_in_bytes
);

/*==============================================================================

  FUNCTION:  a2_dl_phy_hspa_notify_gather_and_decipher

==============================================================================*/
/*!
    @brief
    Write TAG task to A2 DL PHY with argument "user_data".
    Once "user_data" is received in the status queue then call back function
    pointer a2_dl_phy_hspa_gather_and_decipher_tag_cb() is called with argument
    "user_data".

    @caller
    Either "HSPA Header Read ISR" or "MAC DL HS"

    @return
    None
*/
/*============================================================================*/
void a2_dl_phy_hspa_notify_gather_and_decipher(uint32 user_data);

/*==============================================================================

  FUNCTION:  a2_dl_phy_hspa_notify_dma_from_dob_and_decipher

==============================================================================*/
/*!
    @brief
    Write TAG task to A2 DL PHY with argument "user_data".
    Once "user_data" is received in the status queue then call back function
    pointer a2_dl_phy_hspa_dma_from_dob_and_decipher_tag_cb() is called with
    argument "user_data".

    @caller
    HSPA Header Read ISR

    @return
    None
*/
/*============================================================================*/
void a2_dl_phy_hspa_notify_dma_from_dob_and_decipher(uint32 user_data);

/*==============================================================================

  FUNCTION:  a2_dl_phy_hspa_commit

==============================================================================*/
/*!
    @brief
    Commits the write pointer of a2_dl_phy task queue.

    If "interrupt_required" argument is set to TRUE then A2 DL PHY is indciated
    to interrupt SW whenever all the tasks written so far is executed.
    If "interrupt_required" argument is set to FALSE then it means that there is
    no need to receive an interrupt from A2 DL PHY.

    @note
    Takes care of necessary cache flushing of task queue before updating
    write pointer of the A2 DL PHY task queue

    @caller
    "HSPA Header Read ISR" and/or "MAC DL HS"

    @return
    None
*/
/*============================================================================*/
void a2_dl_phy_hspa_commit(boolean interrupt_required);

/*==============================================================================

  FUNCTION:  a2_dl_phy_hspa_get_status

==============================================================================*/
/*!
    @brief
    Reads the status from the status queue. Calls appropriate call back
    functions based on the status/tag received.

    @note
    Takes care of necessary cache invaldiation before processing status queue.

    @caller
    "HSPA Header Read ISR" (primary) and/or "MAC DL HS"

    @return
    boolean: TRUE if any status is read from the status queue. FALSE if status
             is not yet updated.
*/
/*============================================================================*/
boolean a2_dl_phy_hspa_get_status(void);

/*==============================================================================

  FUNCTION:  a2_dl_phy_hspa_shut_down

==============================================================================*/
/*!
    @brief
    cleans up DL PHY HSPA state during a2 power collapse

    @return
    TRUE if shut down was successful
    FALSE otherwise
*/
/*============================================================================*/
boolean a2_dl_phy_hspa_shut_down(void);

#endif /* A2_DL_PHY_HSPA_H */

