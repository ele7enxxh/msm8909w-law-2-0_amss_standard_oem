/*!
  @file
  a2_ul_sec.h

  @brief
  The interface to the A2 UL SEC Driver Module

  @author
  araina
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/api/a2/a2_ul_sec.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/30/13   am      CR405543 Crash due to freed DSM when chain length > 80
07/11/13   ars     Add a2_ul_sec_commit_tag_and_irq
12/18/11   ar      CR 324348 fix: memcpy the keys directly into a2 internal 
                   memory instead of UL SEC taskq to avoid delays
06/07/11   ar      added a2_ul_sec_shut_down() and a2_ul_sec_hw_init() apis
08/31/10   ar      added a2_ul_sec_register_dma_dsm_chain_cb() api
05/19/10   ar      changed a2_ul_sec_dest_info_s type to a2_iovec_t
04/29/10   ar      added a2_ul_sec_hspa_gather_and_decipher_with_intr_on_demand
                   api with support for on demand interrupt
12/07/09   ar      added deinit function for the ul sec block
10/26/09   sm      Modified argument type in function a2_ul_sec_lte_gather_-
                   -decipher_and_integrity()
10/12/09   sm      Added comment in a2_ul_sec_lte_compute_integrity()
10/04/09   sm      Removed feature FEATURE_A2_UL_SEC_TEST
07/21/09   ar      added a2_ul_sec_config() function to write config task during
                   UL SEC initialization
06/17/09   ar      converted uint32 integrity value to byte array
05/15/09   ar      initial version
==============================================================================*/

#ifndef A2_UL_SEC_H
#define A2_UL_SEC_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <dsm_item.h>
#include <a2_common.h>
#include <a2_dl_phy_hspa.h>

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*!
   @brief
   Ciphering/Integrity key size (in bytes)
*/
#define A2_UL_SEC_KEY_SIZE                  16

/*!
   @brief
   Integrity value size in bytes
*/
#define A2_UL_SEC_INTEGRITY_VALUE_SIZE      4

/*!
   @brief
   Maximum number of source iovec (address+length) count
 */
#define A2_UL_SEC_MAX_SRC_IOVEC_COUNT       A2_DL_PHY_HSPA_MAX_SRC_IOVEC_COUNT

/*!
   @brief
   LTE PDCPDL callback function ptr type for lte_gather_decipher_and_integrity
   task
*/
typedef void (*a2_ul_sec_lte_gather_decipher_and_integrity_cb)
(
  uint8,           /*!< user data (rb_cfg_idx) */
  dsm_item_type*,  /*!< user data (source dsm item address) */
  dsm_item_type*,  /*!< destination address */
  uint8*           /*!< pointer to computed integrity value (big endian) */
);

/*!
   @brief
   LTE PDCPUL callback function ptr type for lte_compute_integrity task
*/
typedef void (*a2_ul_sec_lte_compute_integrity_cb)
(
  uint8,           /*!< user data (rb_cfg_idx) */
  dsm_item_type*,  /*!< user data (source dsm item address) */
  uint8*           /*!< pointer to computed integrity value (big endian) */
);

/*!
   @brief
   callback function ptr type for a2_ul_sec_dma_src_dsm_chain_to_dest_dsm_chain
   task
*/
typedef void (*a2_ul_sec_dma_src_dsm_to_dest_dsm_cb)
(
  uint32           /*!< user data */
);

/*!
   @brief
   callback function ptr type for hspa_gather_and_decipher task
*/
typedef
    a2_dl_phy_hspa_gather_and_decipher_cb a2_ul_sec_hspa_gather_and_decipher_cb;

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*==============================================================================

  FUNCTION:  a2_ul_sec_init

==============================================================================*/
/*!
    @brief
    Initializes SW structures associated with a2 ul sec block

    @return
    None
*/
/*============================================================================*/
extern void a2_ul_sec_init
(
  void
);
/* a2_ul_sec_init() */

/*==============================================================================

  FUNCTION:  a2_ul_sec_deinit

==============================================================================*/
/*!
    @brief
    De-inits variables associated with a2 ul sec block

    @return
    None
*/
/*============================================================================*/
extern void a2_ul_sec_deinit
(
  void
);
/* a2_ul_sec_deinit() */

/*==============================================================================

  FUNCTION:  a2_ul_sec_hspa_register_gather_and_decipher_cb

==============================================================================*/
/*!
    @brief
    Registers a callback function to be called whenever gather_and_decipher
    tag status is processed in the UL SEC status queue.

    @note
    Registered callback can be used to send a signal to a given task to process
    transfer done associated with the *tag* returned as part of the call back
    function argument.
    Processing can be done in call back function context as well with the
    necessary mutex protection (if needed).

    @caller
    HSPA: MAC/L1
    LTE: none
*/
/*============================================================================*/
extern void a2_ul_sec_hspa_register_gather_and_decipher_cb
(
  /*! The callback to be called whenever A2_UL_SEC_TAG_HSPA_GATHER_AND_DECIPHER
   * tag status is received */
  a2_ul_sec_hspa_gather_and_decipher_cb cb_fn_ptr
);
/* a2_ul_sec_hspa_register_gather_and_decipher_cb() */

/*==============================================================================

  FUNCTION:  a2_ul_sec_register_dma_dsm_chain_cb

==============================================================================*/
/*!
    @brief
    Registers a callback function to be called whenever
    A2_UL_SEC_TAG_DSM_SRC_TO_DSM_DEST_GATHER tag status is processed in the
    UL SEC status queue.

    @note
    Registered callback can be used to send a signal to a given task to process
    transfer done associated with the user data returned as part of the call
    back function argument.
    Processing can be done in call back function context as well with the
    necessary mutex protection (if needed).

    @caller
    HSPA: L1
    LTE: none
*/
/*============================================================================*/
EXTERN void a2_ul_sec_register_dma_dsm_chain_cb
(
  /*! The callback to be called whenever
   * A2_UL_SEC_TAG_DSM_SRC_TO_DSM_DEST_GATHER tag status is received */
  a2_ul_sec_dma_src_dsm_to_dest_dsm_cb cb_fn_ptr
);
/* a2_ul_sec_register_dma_dsm_chain_cb() */

/*==============================================================================

  FUNCTION:  a2_ul_sec_lte_register_gather_decipher_and_integrity_cb

==============================================================================*/
/*!
    @brief
    Registers a callback function to be called whenever
    gather_decipher_and_integrity tag status is processed in the UL SEC status Q

    @note
    Registered callback can be used to send a signal to a given task to process
    transfer done associated with the *tag* returned as part of the call back
    function argument.
    Processing can be done in call back function context as well with the
    necessary mutex protection (if needed).

    @caller
    LTE: PDCPDL
    HSPA: none
*/
/*============================================================================*/
extern void a2_ul_sec_lte_register_gather_decipher_and_integrity_cb
(
  /*! callback to be called whenever
   * A2_UL_SEC_TAG_LTE_GATHER_DECIPHER_AND_INTEGRITY tag status is received */
  a2_ul_sec_lte_gather_decipher_and_integrity_cb cb_fn_ptr
);
/* a2_ul_sec_lte_register_gather_decipher_and_integrity_cb() */

/*==============================================================================

  FUNCTION:  a2_ul_sec_lte_register_compute_integrity_cb

==============================================================================*/
/*!
    @brief
    Registers a callback function to be called whenever
    A2_UL_SEC_TAG_LTE_COMPUTE_INTEGRITY tag status is processed in the UL SEC
    status Q

    @note
    Registered callback can be used to send a signal to a given task to process
    transfer done associated with the *tag* returned as part of the call back
    function argument.
    Processing can be done in call back function context as well with the
    necessary mutex protection (if needed).

    @caller
    LTE: PDCPDL
    HSPA: none
*/
/*============================================================================*/
extern void a2_ul_sec_lte_register_compute_integrity_cb
(
  /*! callback to be called whenever A2_UL_SEC_TAG_LTE_COMPUTE_INTEGRITY tag
   * status is received */
  a2_ul_sec_lte_compute_integrity_cb cb_fn_ptr
);
/* a2_ul_sec_lte_register_compute_integrity_cb() */

/*==============================================================================

  FUNCTION:  a2_ul_sec_hspa_gather_and_decipher

==============================================================================*/
/*!
    @brief
    Gathers data from multiple location to one location and if needed performs
    deciphering.

    @detail
    Add following tasks to A2 UL SEC TASK Q:
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
    - HSPA: MAC/L1
    - LTE: none

    @return
    None
*/
/*============================================================================*/
extern void a2_ul_sec_hspa_gather_and_decipher
(
  /*! pointer to ciphering info */
  a2_cipher_info_s      *cipher_info_ptr,
  /*! number of source iovec's valid in src_iovec_list[] from which data needs
      to be gathered. 0 is invalid value. */
  uint16                num_src_iovec,
  /*! source iovec list. Address shall be not NULL and length shall be non zero
      in first "num_src_iovec" array elements */
  a2_iovec_t            src_iovec_list[A2_UL_SEC_MAX_SRC_IOVEC_COUNT],
  /*! destination address (external memory/dsm item memory) where deciphered
      data needs to be copied. Length to be copied to destination is assumed
      to be sum of "length_in_bytes" in valid "src_iovec_list" entries.
      "dest_addr" shall not be set to NULL */
  uint32            dest_addr,
  /*! destination length in bytes */
  uint32            dest_length_in_bytes,
  /*! user data */
  uint32            user_data
);
/* a2_ul_sec_hspa_gather_and_decipher() */

/*==============================================================================

  FUNCTION:  a2_ul_sec_hspa_gather_and_decipher_with_intr_on_demand

==============================================================================*/
/*!
    @brief
    Gathers data from multiple location to one location and performs deciphering
    if needed.

    @detail
    Add following tasks to A2 UL SEC TASK Q:
     - CIPHER TASK (only if cipher_algo is not set to A2_CIPHER_ALGO_NONE)
     - DMA TASK (to gather from multiple sources <external memory> to one
       destination <external memory>)
     - DMA TAG and IRQ TASK (if interrupt_reqd param is set to TRUE)

    @note
    This function takes care of necessary cache flushing/invalidation of
    source(s) and destination.
    This functions assumes that no virtual to physical mapping is needed for
    the address provided as part of the arguments i.e. virtual and physical
    memory address is same.

    @caller
    - HSPA: MAC/L1
    - LTE: none

    @return
    None
*/
/*============================================================================*/
EXTERN void a2_ul_sec_hspa_gather_and_decipher_with_intr_on_demand
(
  /*! pointer to ciphering info */
  a2_cipher_info_s      *cipher_info_ptr,
  /*! number of source iovec's valid in src_iovec_list[] from which data needs
      to be gathered. 0 is invalid value. */
  uint16                num_src_iovec,
  /*! source iovec list. Address shall be not NULL and length shall be non zero
      in first "num_src_iovec" array elements */
  a2_iovec_t            src_iovec_list[A2_UL_SEC_MAX_SRC_IOVEC_COUNT],
  /*! destination address and length info where deciphered data needs to be
      copied. Length to be copied to destination is assumed to be sum of
      "length_in_bytes" in valid "src_iovec_list" entries. "dest_addr" shall not
      be set to NULL */
  a2_iovec_t            *dest_info_ptr,
  /*! user data */
  uint32                user_data
);

/*==============================================================================

  FUNCTION:  a2_ul_sec_commit_tag_and_irq

==============================================================================*/
/*!
    @brief
    Function writes the tag task and irq task and commits the write pointer to
    HW

    @caller
    - HSPA: L1
    - LTE: none

    @return
    None
*/
/*============================================================================*/
EXTERN void a2_ul_sec_commit_tag_and_irq
(
  /*! user data */
  uint32        user_data
);

/*==============================================================================

  FUNCTION:  a2_ul_sec_dma_src_dsm_chain_to_dest_dsm_chain

==============================================================================*/
/*!
    @brief
    Gathers data from source dsm item chain to destination dsm item chain

    @detail
    Input arguments:

    src_dsm_chain_ptr - dsm item chain ptr to copy data from. It can either be
    a single dsm item or a dsm item chain

    src_length_in_bytes - length in bytes of the src dsm item chain

    dest_dsm_chain_ptr - dsm item chain ptr where data needs to be copied to. It
    can either be a single dsm item or a dsm item chain
    NOTE: The caller needs to allocate and chain the dsm item together. The used
    field of the newly allocated dsm items should be set to 0. Api behavior is
    undefined if the used field of the newly allocated dsm items is non-zero.

    dest_length_in_bytes - total length in bytes that need to be copied to the
    dest dsm address. It can be less than OR equal OR or greater than the source
    dsm length. In case its greater, the source data would be repeated.


    @note
    This function takes care of necessary cache flushing/invalidation of
    source(s) and destination.
    This functions assumes that no virtual to physical mapping is needed for
    the address provided as part of the arguments i.e. virtual and physical
    memory address is same.

    @caller
    - HSPA: L1
    - LTE: none

    @return
    None
*/
/*============================================================================*/
EXTERN void a2_ul_sec_dma_src_dsm_chain_to_dest_dsm_chain
(
  /*! pointer to source dsm item chain */
  dsm_item_type **src_dsm_chain_ptr,
  /*! size in bytes of the src dsm chain */
  uint16        src_length_in_bytes,
  /*! pointer to dest dsm item chain, see api comments for more info */
  dsm_item_type *dest_dsm_chain_ptr,
  /*! destination length in bytes */
  uint32        dest_length_in_bytes,
  /*! user data */
  uint32        user_data
);
/* a2_ul_sec_dma_src_dsm_chain_to_dest_dsm_chain() */

/*==============================================================================

  FUNCTION:  a2_ul_sec_lte_gather_decipher_and_integrity

==============================================================================*/
/*!
    @brief
    Gathers data from source address and if needed performs deciphering and
    integrity.

    @detail
    Add following tasks to A2 UL SEC TASK Q:
     - CIPHER TASK (only if cipher algo is not set to A2_CIPHER_ALGO_NONE)
     - DMA TASK (to gather from multiple sources <external memory> to one
       destination <external memory>)
     - INTEGRITY TASK (computes integrity value of the deciphered dma data only
       if integrity algo is not set to A2_INTEGRITY_ALGO_NONE)

    For PDCPDL packet, the PDCP header are the first (1 or 2) bytes in the
    beginning of the source packet dsm chain.

    This API doesn't take the destination address as an input argument due to
    possibility of source data length being greater than DSM_DS_LARGE_ITEM_SIZ.
    This API internally allocates dsm items on demand. If source data is greater
    than DSM_DS_LARGE_ITEM_SIZ (i.e. 1536 bytes), this function splits the
    source data into multiple DMA tasks of size DSM_DS_LARGE_ITEM_SIZ or less.
    It chains the dsm items containing deciphered data and returns the pointer
    to the caller.

    The passed source pointer address variable (src_pkt_ptr) may be changed.

    @note
    This function takes care of necessary cache flushing/invalidation of
    source(s) and destination.
    This functions assumes that no virtual to physical mapping is needed for
    the address provided as part of the arguments i.e. virtual and physical
    memory address is same.

    @caller
    - LTE: PDCPDL
    - HSPA: none

    @return
    None
*/
/*============================================================================*/
extern void a2_ul_sec_lte_gather_decipher_and_integrity
(
  a2_cipher_info_s     *cipher_info_ptr,    /*!< ptr to ciphering info*/
  a2_integrity_info_s  *integrity_info_ptr, /*!< ptr to integrity info*/
  dsm_item_type        *src_pkt_ptr,        /*!< src dsm item addr */
  uint16               src_length_in_bytes, /*!< src dsm length */
  uint16               pdcp_hdr_len,        /*!< pdcp header length in bytes */
  uint8                user_data            /*!< user data (rb_cfg_idx)*/
);
 /* a2_ul_sec_lte_gather_decipher_and_integrity() */

/*==============================================================================

  FUNCTION:  a2_ul_sec_lte_compute_integrity

==============================================================================*/
/*!
    @brief
    Gathers data from source address computes the MAC-I.

    @detail
    Add following tasks to A2 UL SEC TASK Q:
     - DMA TASK (to gather from multiple sources <external memory> to one
       destination <external memory>)
     - INTEGRITY TASK (computes integrity value of the dma data only
       if integrity algo is not set to A2_INTEGRITY_ALGO_NONE)

    For PDCPUL packet, the PDCP header is present in the structure pointed to
    by the app_ptr of the source packet dsm chain.

    @note
    This function takes care of necessary cache flushing/invalidation of
    source(s) and destination.
    This functions assumes that no virtual to physical mapping is needed for
    the address provided as part of the arguments i.e. virtual and physical
    memory address is same.

    @caller
    - LTE: PDCPUL
    - HSPA: none

    @return
    None
*/
/*============================================================================*/
extern void a2_ul_sec_lte_compute_integrity
(
  a2_integrity_info_s  *integrity_info_ptr, /*!< ptr to integrity info*/
  dsm_item_type        *src_pkt_ptr,        /*!< src dsm item addr */
  uint16               src_length_in_bytes, /*!< src length in bytes (includes
                                                 payload and header length) */
  uint8                *pdcp_hdr_ptr,       /*!< ptr to pdcp header, pdcpul hdr
                                                 resides in the src pkt dsm item
                                                 app_ptr */
  uint32               pdcp_hdr_len,        /*!< pdcp header length */
  uint8                user_data            /*!< user data (rb_cfg_idx)*/
);
/* a2_ul_sec_lte_compute_integrity() */

/*==============================================================================

  FUNCTION:  a2_ul_sec_write_keys

==============================================================================*/
/*!
    @brief
    Function writes multiple key values (atleast 1, atmost 3) to A2 internal
    memory

    @detail
    It also stores the key values to local struct which will be needed for
    register interface based ciphering.
    Caller is expected to set xxx_key_index to 0 and xxx_key value to NULL for
    unused index and key value arguments

    For non-LTE techs, messaging to DL PER is needed to ensure A2 is woken up
    before UL SEC writes keys. In case of HSPA, RRC wakes up A2 and immediately
    writes the security keys.

    @caller
    LTE: RRC
    HSPA: RRC
*/
/*============================================================================*/
extern void a2_ul_sec_write_keys
(
  uint8  first_key_index,                /*!< first key index: range 0-15 */
  uint8  first_key[A2_UL_SEC_KEY_SIZE],  /*!< 16 byte key value > */
  uint8  second_key_index,               /*!< second key index: range 0-15 */
  uint8  second_key[A2_UL_SEC_KEY_SIZE], /*!< 16 byte key value > */
  uint8  third_key_index,                /*!< third key index: range 0-15 */
  uint8  third_key[A2_UL_SEC_KEY_SIZE]   /*!< 16 byte key value > */
);
/* a2_ul_sec_write_keys() */

/*==============================================================================

  FUNCTION:  a2_ul_sec_process_write_keys_req

==============================================================================*/
/*!
  @brief
    Function copies multiple key values (atleast 1, atmost 3) to A2 internal
    memory

    @detail
    It also stores the key values to local struct which will be needed for
    register interface based ciphering.
    Caller is expected to set xxx_key_index to 0 and xxx_key value to NULL for
    unused index and key value arguments
*/
/*============================================================================*/
EXTERN void a2_ul_sec_process_write_keys_req
(
  uint8  first_key_index,                /*!< first key index: range 0-15 */
  uint8  first_key[A2_UL_SEC_KEY_SIZE],  /*!< 16 byte key value > */
  uint8  second_key_index,               /*!< second key index: range 0-15 */
  uint8  second_key[A2_UL_SEC_KEY_SIZE], /*!< 16 byte key value > */
  uint8  third_key_index,                /*!< third key index: range 0-15 */
  uint8  third_key[A2_UL_SEC_KEY_SIZE]   /*!< 16 byte key value > */
);
/* a2_ul_sec_process_write_keys_req() */

/*==============================================================================

  FUNCTION:  a2_ul_sec_ist_cb

==============================================================================*/
/*!
  @brief
  Interrupt handler for A2 UL SEC module.

*/
/*============================================================================*/
extern int a2_ul_sec_ist_cb
(
  void* arg
);
/* a2_ul_sec_ist_cb() */

/* #ifdef FEATURE_A2_UL_SEC_TEST */

/*==============================================================================

  FUNCTION:  a2_ul_sec_dma_from_a2_internal_mem

==============================================================================*/
/*!
  @brief
  Function writes a dma task for gathering keys from A2 internal memory

  @detail
  none

*/
/*============================================================================*/
extern void a2_ul_sec_dma_from_a2_internal_mem
(
  uint32 src_addr,         /*!< source a2 internal memory address */
  uint16 src_len_in_bytes, /*!< source length in bytes */
  uint32 dest_addr         /*!< address where dma-ed data is stored */
);
/* a2_ul_sec_dma_from_a2_internal_mem() */

/* #endif FEATURE_A2_UL_SEC_TEST */

/*==============================================================================

  FUNCTION:  a2_ul_sec_shut_down

==============================================================================*/
/*!
    @brief
    cleans up UL SEC state during a2 power collapse

    @return
    TRUE if shut down was successful
    FALSE otherwise
*/
/*============================================================================*/
extern boolean a2_ul_sec_shut_down
(
  void
);
/* a2_ul_sec_shut_down() */

/*==============================================================================

  FUNCTION:  a2_ul_sec_hw_init

==============================================================================*/
/*!
    @brief
    initializes the HW related registers for UL SEC block

    @return
    None
*/
/*============================================================================*/
extern void a2_ul_sec_hw_init
(
  void
);
/* a2_ul_sec_hw_init() */

/*This API will return the actual keys for the passed key index*/

EXTERN uint8* a2_ul_sec_get_key_ptr
(
uint8 key_index
);


#endif /* A2_UL_SEC_H */
