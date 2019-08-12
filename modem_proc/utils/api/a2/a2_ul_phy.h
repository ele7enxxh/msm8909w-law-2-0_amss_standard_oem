#ifndef A2_UL_PHY_H
#define A2_UL_PHY_H
/*!
  @file a2_ul_phy.h

  All data is passed as numbers of bits, any parameters which are in bytes
  can be converted to bytes via the A2_BYTES_TO_BITS() macro.

  -# Initialization: The function calls used to initialize a transport block
     These APIs must be called in the given order.
    -# a2_ul_phy_write_crc_init_task()
    -# a2_ul_phy_reserve_first_gather_task() or
       a2_ul_phy_reserve_first_fill_task()
  -# Data Preparation: Used to cipher and copy data to encoder ram
    -# a2_ul_phy_write_fill_task() -- Optional
    -# a2_ul_phy_reserve_gather_task()
    -# a2_ul_phy_write_gather_task_to_specified_addr()
    -# a2_ul_phy_write_cipher_task()
    -# a2_ul_phy_write_dsm_gather_task()
    -# a2_ul_phy_write_gather_task()
    -# a2_ul_phy_write_first_fill_task_to_specified_addr()
  -# Completion: These functions used to complete a transport block
     and should be called in the following order.
    -# a2_ul_phy_write_first_gather_task_to_specified_addr()
    -# a2_ul_phy_write_crc_task()
    -# a2_ul_phy_write_done_task()
    -# a2_ul_phy_commit_write_ptr()

  @ingroup a2_ext_interface
*/

/*===========================================================================

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/api/a2/a2_ul_phy.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/28/15   mm      CR779307: Increasing maximum MAC padding
01/27/15   vd      CR735568: Requirement to corrupt CRC on UL PHY for a harq id
03/22/13   bn      CR466873, new a2_ul_phy_process_harq_retx input
02/05/13   bn      Delete a2_ul_phy_notify_actual_hw_process_time func
08/08/12   ars     DIME PHY changes after RUMI validation 
06/07/11   ar      added a2_ul_phy_shut_down() and a2_ul_phy_hw_init() apis
04/29/11   ar      passing void to a2_ul_phy_taskq_find_space_remaining()
04/19/11   ar      removed FEATURE_A2_NIKEL for a2_ul_phy_commit_write_ptr()
04/19/11   ar      added FEATURE_A2_NIKEL for a2_ul_phy_commit_write_ptr()
04/12/11   ar      Nikel harq reduction changes
11/02/09   ar      added a2_ul_phy_set_technology()
05/22/09   sk      Added function a2_ul_phy_harq_init to reset the UMTS
                   done flag at the beginning of the processing of the
                   task session
05/19/09   sah     Add in a2_ul_phy_notify_upon_completion() functions.
05/14/09   sah     Add in some new espresso initialization, featurization,
                   and split init.
05/10/09   sk      Added a2_ul_phy_dma_from_eram function prototypeto support
                   moving data from eram to external memory
04/21/09   sm      Removed dependency on lte_as.h
07/11/08   sah     Initial Checkin

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/


#include <dsm.h>
#include <a2_common.h>




/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! Callback type to be register with a2_ul_phy_set_notify_cb() */
typedef void (*a2_ul_phy_notify_cb_t)(uint32);

/*! Definition for the struct to indicate crc corruption */
typedef struct
{
  uint8 harq_id;/*!< harq id for which tranmission is reqd */
  boolean  corrupt_crc; /*!< TRUE => corrupt CRC */
} a2_ul_phy_corrupt_crc_info_s;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================

  FUNCTION:  a2_ul_phy_init

===========================================================================*/
/*!
  @brief
  Initialize the SW structures for ul phy block

*/
/*=========================================================================*/
void a2_ul_phy_init
(
  void
);

/*===========================================================================

  FUNCTION:  a2_ul_phy_write_crc_init_task

===========================================================================*/
/*!
  @brief
  This function will reset the CRC computation.

  This function must be called first whenever a transport block is being
  built by the A2 driver.  The CRC calculation will be performed based upon
  all Fill/Gather tasks between this call and a2_ul_phy_write_cipher_task.

*/
/*=========================================================================*/
void a2_ul_phy_write_crc_init_task
(
  void
);


/*===========================================================================

  FUNCTION:  a2_ul_phy_reserve_gather_task

===========================================================================*/
/*!
  @brief
  Reserves space to add scatter gather task in UL_PHY TASK Q.

  The next task in UL_PHY TASK Q will be placed after this reserved space.
  The caller must write to this reserved task memory via
  a2_ul_phy_write_gather_task prior to a call to a2_ul_phy_commit_write_ptr.

  @return A pointer to the gather task to be filled. The caller is expected
          to use this reserved address to write the gather task (for MAC
          header) by providing this address to
          a2_ul_phy_write_gather_task_to_specified_addr().
*/
/*=========================================================================*/
void *a2_ul_phy_reserve_gather_task
(
  void
);

/*===========================================================================

  FUNCTION:  a2_ul_phy_reserve_first_gather_task

===========================================================================*/
/*!
  @brief
  Reserves space to add scatter gather task in UL_PHY TASK Q.

  The next task in UL_PHY TASK Q will be placed after this reserved space.
  The caller must write to this reserved task memory via
  a2_ul_phy_write_first_gather_task prior to a call to
  a2_ul_phy_commit_write_ptr.

  @return A pointer to the gather task to be filled. The caller is expected
          to use this reserved address to write the gather task (for MAC
          header) by providing this address to
          a2_ul_phy_write_first_gather_task_to_specified_addr().
*/
/*=========================================================================*/
void *a2_ul_phy_reserve_first_gather_task
(
  void
);

/*===========================================================================

  FUNCTION:  a2_ul_phy_write_gather_task_to_specified_addr

===========================================================================*/
/*!
  @brief
  Write GATHER TASK to specified address in UL_PHY TASK Q which was reserved
  using a2_ul_phy_reserve_scatter_gather_task().

  This function is expected to be successful, upon failure it will throw an
  error fatal (which can be used to fine tune TASK Q size).

*/
/*=========================================================================*/
void a2_ul_phy_write_gather_task_to_specified_addr
(
  void   *ul_phy_task_q_addr, /*!< The address where GATHER TASK needs to be
                                written.  This must be previously allocated
                                and returned by a2_ul_phy_reserve_gather_task.
                               */
  void   *source_address,     /*!< Source address from which gather needs to
                                be performed */
  uint16 length_in_bits      /*!< The length of the data pointed to by
                                source_address */
);

/*===========================================================================

  FUNCTION:  a2_ul_phy_write_first_gather_task_to_specified_addr

===========================================================================*/
/*!
  @brief
  Write GATHER TASK to specified address in UL_PHY TASK Q which was reserved
  using a2_ul_phy_reserve_first_scatter_gather_task().

  This task also contains the destination location that the subsequent gather
  tasks will copy data to.

  This function is expected to be successful, upon failure it will throw an
  error fatal (which can be used to fine tune TASK Q size).

*/
/*=========================================================================*/
void a2_ul_phy_write_first_gather_task_to_specified_addr
(
  void   *ul_phy_task_q_addr, /*!< The address where GATHER TASK needs to be
                                written.  This must be previously allocated
                                and returned by a2_ul_phy_reserve_gather_task.
                               */
  void   *source_address,     /*!< Source address from which gather needs to
                                be performed */
  void   *dest_address,     /*!< Destination address where data programmed
                              in gather tasks will be copied to */
  a2_mem_type_e dst_mem_type,    /*!< Whether the destination is either
                                   internal or external memory */
  uint16 length_in_bits      /*!< The length of the data pointed to by
                                source_address */
);

/*===========================================================================

  FUNCTION:  a2_ul_phy_reserve_fill_task

===========================================================================*/
/*!
  @brief
  Reserves space to add fill task in UL_PHY TASK Q.

  The next task in UL_PHY TASK Q will be placed after this reserved space.
  The caller must write to this reserved task memory via
  a2_ul_phy_write_fill_task prior to a call to a2_ul_phy_commit_write_ptr.

  @return A pointer to the gather task to be filled. The caller is expected
          to use this reserved address to write the fill task by providing this
          address to a2_ul_phy_write_fill_task_to_specified_addr().
*/
/*=========================================================================*/
void *a2_ul_phy_reserve_fill_task
(
  void
);

/*===========================================================================

  FUNCTION:  a2_ul_phy_write_fill_task_to_specified_addr

===========================================================================*/
/*!
  @brief
  Write GATHER TASK to specified address in UL_PHY TASK Q which was reserved
  using a2_ul_phy_reserve_fill_task().

  This function is expected to be successful, upon failure it will throw an
  error fatal (which can be used to fine tune TASK Q size).

*/
/*=========================================================================*/
void a2_ul_phy_write_fill_task_to_specified_addr
(
  void   *ul_phy_task_q_addr, /*!< The address where FILL TASK needs to be
                                written.  This must be previously allocated
                                and returned by a2_ul_phy_reserve_fill_task.
                               */
  void   *source_address,     /*!< Source address from which gather needs to
                                be performed */
  uint16 length_in_bits      /*!< The length of the data pointed to by
                                source_address( MUST not exceed 16 bytes) */
);

/*===========================================================================

  FUNCTION:  a2_ul_phy_reserve_first_fill_task

===========================================================================*/
/*!
  @brief
  Reserves space to add the first fill task in UL_PHY TASK Q.

  This function or a2_ul_phy_reserve_first_scatter_gather_task() must be the
  first dma-related task in the transport block.

  The next task in UL_PHY TASK Q will be placed after this reserved space.
  The caller must write to this reserved task memory via
  a2_ul_phy_write_first_fill_task_to_specified_addr() prior to a call to
  a2_ul_phy_commit_write_ptr().

  @return A pointer to the gather task to be filled. The caller is expected
          to use this reserved address to write the fill task by providing this
          address to a2_ul_phy_write_first_fill_task_to_specified_addr().
*/
/*=========================================================================*/
void *a2_ul_phy_reserve_first_fill_task
(
  void
);

/*===========================================================================

  FUNCTION:  a2_ul_phy_write_first_fill_task_to_specified_addr

===========================================================================*/
/*!
  @brief
  Write GATHER TASK to specified address in UL_PHY TASK Q which was reserved
  using a2_ul_phy_reserve_first_fill_task().

  This task also contains the destination location that the subsequent gather
  tasks will copy data to.

  This function is expected to be successful, upon failure it will throw an
  error fatal (which can be used to fine tune TASK Q size).

*/
/*=========================================================================*/
void a2_ul_phy_write_first_fill_task_to_specified_addr
(
  void   *ul_phy_task_q_addr, /*!< The address where FILL TASK needs to be
                                written.  This must be previously allocated
                                and returned by a2_ul_phy_reserve_fill_task.
                               */
  void   *source_address,     /*!< Source address from which gather needs to
                                be performed */
  void   *dest_address,       /*!< Destination address where data programmed
                                   in subsequent gather tasks will be copied
                                   to */
  a2_mem_type_e dst_mem_type,    /*!< Whether the destination is either
                                   internal or external memory */
  uint16 length_in_bits      /*!< The length of the data pointed to by
                                source_address( MUST be less than 128 bits)
                              */
);

/*===========================================================================

  FUNCTION:  a2_ul_phy_write_cipher_task

===========================================================================*/
/*!
  @brief
  Builds and writes a cipher task using provided input parameters to UL_PHY
  TASK Q.

  A2 is expected to apply ciphering parameters indicated in CIPHER TASK on next
  TASK (SCATTER-GATHER/GATHER/FILL).

  This function is expected to be successful otherwise it will throw error
  fatal (which can be used to fine tune TASK Q size).  It is assumed that this
  task is to be used only for uplink data.

  @caller
    - LTE: RLC (to provide ciphering parameters for next task);
    - HSPA: RLC (to provide ciphering parameters for next task)
*/
/*=========================================================================*/
void a2_ul_phy_write_cipher_task
(
  a2_cipher_algo_e cipher_algo,  /*!< The type of ciphering algorithm to use */
  uint8 key_index,  /*!< The CK index (valid range 0..15) */
  uint32 count_c,   /*!< The count-c to be used for ciphering */
  uint16 key_stream_offset, /*!< The offset into the keystream in bytes from the
                              start of the IP Frame/RLC PDU which has been
                             ciphered in the previous task */
  uint8 bearer_id,          /*!< rb_id - 1 (valid range 0..31) */
  uint16 cipher_length      /*!< @todo need to remove this */
);

/*===========================================================================

  FUNCTION:  a2_ul_phy_harq_init

===========================================================================*/
/*===========================================================================*/
/*!
  @brief

  This function resets the UMTS done flag for a particular HARQ

  This function must be called first whenever a transport block is being
  built by the A2 driver.  This should be called before calling the CRC_INIT
  task

*/
/*=========================================================================*/
void a2_ul_phy_harq_init
(
  uint8 harq_id,/*!< harq id for which tranmission is reqd */
  uint32  univ_stmr_val /*!< Sched. time at which the harq should be moved to ERAM */
);

/*===========================================================================

  FUNCTION:  a2_ul_phy_corrupt_crc

===========================================================================*/
/*===========================================================================*/
/*!
  @brief

  This function indicates if the CRC needs to be corrupted for a particular
  HARQ

  If the corrupt crc flag is set for a particular harq it will remain that 
  way even for future transmissions/re-transmission on the same harq id
  unless explicitly the corrupt crc flag is set to false by calling this
  function again

*/
/*=========================================================================*/
void a2_ul_phy_corrupt_crc
(
  a2_ul_phy_corrupt_crc_info_s corrupt_crc
);

/*===========================================================================

  FUNCTION:  a2_ul_phy_write_gather_task

===========================================================================*/
/*!
  @brief
  Writes GATHER TASK to UL_PHY TASK Q.

  A gather task is used to indicates A2 to copy the data provided to the
  next spot in the encoder ram.  To write multiple addresses to the
  gather task use a2_ul_phy_append_gather_task() to add additional addresses.

  This function is expected to be successful otherwise it will throw an
  error fatal (which can be used to fine tune TASK Q size).

  @caller
    - LTE: MAC and RLC (to move the data from external memory to ERAM);
    - HSPA: RLC (optional) (to move the data from external memory to ERAM)
*/
/*=========================================================================*/
void a2_ul_phy_write_gather_task
(
  uint32 *source_address, /*!< A pointer to the data to be copied to encoder
                            RAM */
  uint16 length_in_bits  /*!< The length of the data to be copied into
                            encoder RAM */
);




/*===========================================================================

  FUNCTION:  a2_ul_phy_write_dsm_scatter_gather_task

===========================================================================*/
/*!
  @brief
  Builds scatter-gather task to copy dsm chain to encoder ram.

  Builds and writes SCATTER-GATHER TASK based upon the DSM chain provided as
  the input parameters to UL_PHY TASK Q.

  This function is expected to be successful otherwise it will throw error
  fatal (which can be used to fine tune TASK Q size).

*/
/*=========================================================================*/
void a2_ul_phy_write_dsm_scatter_gather_task
(
  dsm_item_type *pkt_ptr, /*!< A pointer to the dsm chain*/
  uint16 offset, /*!< The offset into the dsm item to be copied */
  uint16 length, /*!< The length of the the data to be extracted from the dsm
                   item */
  boolean interrupt_reqd /*!< Whether or not an interrupt is required upon
                            movement of this item, (may be removed) */
);


/*===========================================================================

  FUNCTION:  a2_ul_phy_write_fill_task

===========================================================================*/
/*!
  @brief
  Writes a fill task, which is used to efficently copy small amounts of data.

  Copies the data using the source address and length to build FILL TASK
  causing data to be copied to encoder RAM.

  This function is expected to be successful otherwise it will throw error
  fatal (in the beginning to fine tune TASK Q size).
*/
/*=========================================================================*/
void a2_ul_phy_write_fill_task
(
  void *source_address, /*!< source address of the data to be copied */
  uint16 length_in_bits /*!< length in bytes from specified source address
                           to be copied (must be less than 128) */
);

/*===========================================================================

  FUNCTION:  a2_ul_phy_write_hdr_task

===========================================================================*/
/*!
  @brief
  Writes a dma header task, which is used to efficently copy even smaller
  amounts of data than the fill task.

  Copies the data using the source address and length to build FILL TASK
  causing data to be copied to encoder RAM.

  This function is expected to be successful otherwise it will throw error
  fatal (in the beginning to fine tune TASK Q size).
*/
/*=========================================================================*/
void a2_ul_phy_write_hdr_task
(
  void *source_address, /*!< source address of the data to be copied */
  uint16 length_in_bits /*!< length in bytes from specified source address
                           to be copied (must be less than 32) */
);


/*===========================================================================

  FUNCTION:  a2_ul_phy_write_crc_task

===========================================================================*/
/*!
  @brief
  Write CRC TASK to UL_PHY TASK Q.

  The CRC TASK indicates A2 to write the CRC to destination address based upon
  the data programmed between a2_ul_phy_init_transport_block() and this
  function.

  This function is expected to be successful otherwise it will throw error
  fatal (which can be used to fine tune TASK Q size).

  @caller
    - LTE: MAC (for indicating A2 to generate and append CRC);
    - HSPA: MAC or L1 (for indicating A2 to generate and append CRC)
*/
/*=========================================================================*/
void a2_ul_phy_write_crc_task
(
  void
);

/*===========================================================================

  FUNCTION:  a2_ul_phy_write_done_task

===========================================================================*/
/*!
  @brief
  Write DONE TASK to UL_PHY TASK Q.

  This task updates the "done" flag associated with a given ERAM to indicate
  data is ready to be transferred on a given ERAM.

  This function is expected to be successful otherwise it will throw error
  fatal (which can be used to fine tune TASK Q size)

  @note who will clear the flag? HW/FW/SW? HW seems to be the right choice here
        if there is no FW involvement

  @todo combine with crc

  @caller
    - LTE: MAC (for indicating data is ready to be ciphered,
      concatenated and CRCed for transmission on a given ERAM Bank);
    - HSPA: MAC or L1 (for indicating data is ready ciphered,
      concatenated and CRCed for transmission on a given ERAM Bank)
*/
/*=========================================================================*/
void a2_ul_phy_write_done_task
(
  uint8 harq_id,  /*!< HARQ Identifier. This is expected to be same as
                    ERAM Bank Index on which transport block is just written
                    is this needed? */
  uint16 tb_size_in_bits, /*!< For error handling purpose. */
  boolean interrupt_reqd /*!< caller is expected to set it to true for
                           profiling purpose [This can be made as in internal
                           feature of A2 as well TBD] */
);



/*===========================================================================

  FUNCTION:  a2_ul_phy_commit_write_ptr

===========================================================================*/
/*!
  @brief
  Triggers the programmed tasks programmed by the driver to be worked on by
  hardware.

  This function will write the newly updated write pointer to write pointer
  register of ULPHY TASK Q.  This function will also take care of necessary
  cache flushing/invalidation needed for all the SCATTER GATHER TASKs written
  since last update of write pointer, and also flushing of UL TASK Q
  appropriately.

  @caller
    - LTE: MAC (to trigger A2 to start working on the TASK Q);
    - HSPA: MAC or L1 (to trigger A2 to start working on the TASK Q)
*/
/*=========================================================================*/
void a2_ul_phy_commit_write_ptr
(
  uint8 harq_id /*!< HARQ Identifier. This is expected to be same as
                   ERAM Bank Index on which transport block is just written */
);

/*===========================================================================

  FUNCTION:  a2_ul_phy_write_dsm_gather_task

===========================================================================*/
/*!
  @brief
  Copies the data to encoder ram from the provided DSM chain, using the A2
  GATHER TASK.

  The GATHER TASK indicates A2 to pick up destination address from the ones
  specified in last SET_DESTINATION_TASK and length specified in in-between
  TASKs.

  This function is expected to be successful otherwise it will throw error
  fatal (in order to fine tune TASK Q size).

*/
/*=========================================================================*/

void a2_ul_phy_write_dsm_gather_task
(
  dsm_item_type *pkt_ptr, /*!< chain of DSM pointer(s) which needs to be
                            gathered */
  uint16 offset_in_bytes, /*!< offset in bytes from where data needs to be
                            gathered in the pkt_ptr. (Expected to be zero
                            in most or may be all the cases. Do we need this?).
                           */
  uint16 length_in_bytes /*!< The length of the data to be copied from pkt_ptr
                           */
);



/*===========================================================================

  FUNCTION:  a2_ul_phy_write_padding_bits

===========================================================================*/
/*!
  @brief
  Writes padding bits (all 0's) to ERAM

  @caller
    - LTE: MAC (For adding padding bits to the transport block);
    - HSPA: RLC
       -# For adding padding bits to the transport block
       -# For adding padding bits to the RLC PDU)


*/
/*=========================================================================*/
void a2_ul_phy_write_padding_bits
(
  uint32 num_pad_bits /*!< number of bits to be padded by A2 from destination
                        address as specified in SET_DESTINATION Task */
);


/*==============================================================================

  FUNCTION:  a2_ul_phy_taskq_find_space_remaining

==============================================================================*/
/*!
  @brief
  Checks to see how much space is left in the task queue
  Needed for unit testing.

  @return The number of blocks remaining

*/
/*============================================================================*/
uint32 a2_ul_phy_taskq_find_space_remaining
(
  void
);

/*===========================================================================

  FUNCTION:  a2_ul_phy_dma_from_eram

===========================================================================*/
/*!
  @brief
  Write DMA for copying data from ERAM to external memory

  This function is expected to be successful, upon failure it will throw an
  error fatal (which can be used to fine tune TASK Q size).

*/
/*=========================================================================*/
void a2_ul_phy_dma_from_eram
(
  const void *source_address, /*!< Source address from where the data
                                   needs to be copied. This will be private
                                   memory and ERAM address*/
  void       *dest_address,   /*!< This will be external memory address */
  uint32     length_in_bytes  /*!< The length of the data pointed to by
                                source_address */
);

/*==============================================================================

  FUNCTION:  a2_ul_phy_set_notify_cb

==============================================================================*/
/*!
  @brief
  Sets the notification callback.

  The notification callback is called after all tasks programmed
  prior to a2_ul_phy_notify_upon_completion() are processed.
*/
/*============================================================================*/
void a2_ul_phy_set_notify_cb
(
 /*! The callback to be called when the notification is triggered. */
 a2_ul_phy_notify_cb_t cb
);

/*==============================================================================

  FUNCTION:  a2_ul_phy_notify_upon_completion

==============================================================================*/
/*!
  @brief
  Notifies the caller when all the currently programmed tasks have been
  processed.

  The A2 driver will call the callback specified in a2_ul_phy_set_notify_cb()
  after the tasks have been processed.

*/
/*============================================================================*/
void a2_ul_phy_notify_upon_completion
(
  /*! The user data, this will be passed in the callback when the currently
   programmed task are processed.
   */
  uint32 user_data
);

/*===========================================================================

  FUNCTION:  a2_ul_phy_ist_cb

===========================================================================*/
/*!
  @brief
  Interrupt handler for A2 UL PHY module.

*/
/*=========================================================================*/
int a2_ul_phy_ist_cb ( void* arg );


/*==============================================================================

  FUNCTION:  a2_ul_phy_set_technology

==============================================================================*/
/*!
  @brief
  Set the technology that is going to use A2 UL PHY HW and perform necessary
  HW initialization.

  @caller
  HSPA: whenever handover to HSPA is initiated and/or whenever WCDMA cell search
        starts
  LTE: whenever UE starts LTE cell search

  @todo - need to see from when and where to call this function. Critical from
   HSPA<->LTE hand-off. Need to make sure that task bring up doesn't happen in
   parallel. If yes then there is a need to sync up between LTE and HSPA.
   Probable mutex to guarantee mutual exclusion.
*/
/*============================================================================*/
void a2_ul_phy_set_technology( a2_technology_e technology );

/*==============================================================================

  FUNCTION:  a2_ul_phy_process_harq_retx

==============================================================================*/
/*!
  @brief
  Function for re-programming the UL PHY taskq for harq retransmission

  @details
  In Nikel modem, due to reduction in harq memory, A2 needs to remember the
  taskq programming for a particular harq and in case of harq retx, reprogram
  the a2 tasks for that harq.

  If the A2 tasks for that harq are intact in the UL PHY taskq, this function
  simply copies them again into the taskq. If the tasks have been backed up into
  a dsm item (in case of DTX), it will copy the task programming from dsm memory
  to the UL PHY taskq.

  @caller
  MAC UL task

*/
/*============================================================================*/
void a2_ul_phy_process_harq_retx
(
  uint8 harq_id, /*!< harq id on which retx is scheduled */
  uint32  univ_stmr_val,/*!< Sched. time at which the harq should be moved to ERAM */
  uint32  enc_addr      /*!< encoder address */
);
/* a2_ul_phy_process_harq_retx() */

/*==============================================================================

  FUNCTION:  a2_ul_phy_process_harq_dtx

==============================================================================*/
/*!
  @brief
  Function stores the UL PHY taskq programming for current harq into a dsm item
  in case of DTX (discontinuous transmission)

  @details
  In case of DTX, eNB doesn't want the UE to retx the harq data. In that case
  A2 needs to back up the taskq programming into a dsm item as it may get over-
  written.

  @caller
  MAC UL task

*/
/*============================================================================*/
void a2_ul_phy_process_harq_dtx
(
  uint8 harq_id /*!< harq id on which neither tx/retx is scheduled */
);
/* a2_ul_phy_process_harq_dtx() */

/*==============================================================================

  FUNCTION:  a2_ul_phy_shut_down

==============================================================================*/
/*!
    @brief
    cleans up UL PHY state during a2 power collapse

    @return
    TRUE if shut down was successful
    FALSE otherwise
*/
/*============================================================================*/
extern boolean a2_ul_phy_shut_down
(
  void
);
/* a2_ul_phy_shut_down ()*/

/*===========================================================================

  FUNCTION:  a2_ul_phy_hw_init

===========================================================================*/
/*!
    @brief
    initializes the HW related registers for UL PHY block

    @return
    None
*/
/*=========================================================================*/
void a2_ul_phy_hw_init
(
  void
);
/* a2_ul_phy_hw_init() */

/*==============================================================================

  FUNCTION:  a2_ul_phy_dma_write_iovec_list

==============================================================================*/
/*!
  @brief
  write DMA tasks based on the input IOV list 
*/
/*============================================================================*/
void a2_ul_phy_dma_write_iovec_list
(
  /*! number of source iovecs */
  uint32              num_src_iovecs,
  /*! source iovec list */
  a2_iovec_t          *src_iovec_list
);
/* a2_ul_phy_dma_write_iovec_list */

#endif /* A2_UL_PHY_H */

