#ifndef A2_DL_PHY_DO_H
#define A2_DL_PHY_DO_H

/*!
  @file a2_dl_phy_do.h

  @brief
  The interface to the downlink phy portion of the a2 driver for 1x/EvDo.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/api/a2/a2_dl_phy_do.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/06/10   sm      Removed declaration a2_dl_phy_do_set_technology() and added 
                   a2_dl_phy_set_technology()
06/23/10   ar      Initial version


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

                         TYPE DEFINITIONS

==============================================================================*/

/*!
  @brief
*/
typedef void (*a2_dl_phy_do_dma_from_dob_cb)
(
  uint32,         /*! user_data provided via a2_dl_phy_do_dma_from_dob() */
  dsm_item_type * /*! dsm pointer to first item of the dsm chain which contains
                      transport block from dob */
);

typedef void (*a2_dl_phy_do_dma_to_dob_cb)
(
  uint32  /*! user_data provided via a2_dl_phy_do_dma_to_dob() */
);

/*==============================================================================

                       STRUCTURES AND UNIONS

==============================================================================*/

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/
/*==============================================================================

  FUNCTION:  a2_dl_phy_set_technology

==============================================================================*/
/*!
  @brief
  Set the technology that is going to use A2 DL PHY HW and perform necessary
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
void a2_dl_phy_set_technology( a2_technology_e technology );

/*==============================================================================

  FUNCTION:  a2_dl_phy_do_register_dma_from_dob_cb

==============================================================================*/
/*!
  @brief
  Registers a callback function to be called whenever do_dma_from_dob
  tag status is processed in the DL PHY status queue.

  @note
  Registered callback can be used to send a signal to a given task to process
  transfer done associated with the *tag* returned as part of the call back
  function argument.
  Processing can be done in call back function context as well with the
  necessary mutex protection (if needed).
*/
/*============================================================================*/
void a2_dl_phy_do_register_dma_from_dob_cb
(
  /*! The callback to be called whenever dma_from_dob_and_decipher tag status
      is received */
  a2_dl_phy_do_dma_from_dob_cb dma_from_dob_cb_fn_ptr
);


/*==============================================================================

  FUNCTION:  a2_dl_phy_do_dma_from_dob

==============================================================================*/
/*!
    @brief
    This function moves the DOB contents as per the arguments to DSM items
    and return to caller via registerd call back function with the chain of dsm
    item containing the transport block.

    @note
    This function takes care of necessary cache flushing/invalidation of source/
    destination.
    This functions assumes that no virtual to physical mapping is needed for
    the address provided as part of the arguments i.e. virtual and physical
    memory address is same.

    @return
    None
*/
/*============================================================================*/
void a2_dl_phy_do_dma_from_dob
(
  /*! DecOB source address of the data to be moved from DecOB. This can be
      relative address (start of DOB addr as address 0) or physical address.
      This is assumed to be byte aligned address location. */
  uint32                  src_dob_addr,
  /*! source length in bytes */
  uint16                  src_len_in_bytes,
  /*! user data that needs to be returned to the caller */
  uint32                  usr_data
);

/*==============================================================================

  FUNCTION:  a2_dl_phy_do_commit

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

    @return
    None
*/
/*============================================================================*/
void a2_dl_phy_do_commit(boolean interrupt_required);

/*==============================================================================

  FUNCTION:  a2_dl_phy_do_register_dma_to_dob_cb

==============================================================================*/
/*!
  @brief
  Registers a callback function to be called whenever do_dma_from_dob
  tag status is processed in the DL PHY status queue.

  @note
  Registered callback can be used to send a signal to a given task to process
  transfer done associated with the *tag* returned as part of the call back
  function argument.
  Processing can be done in call back function context as well with the
  necessary mutex protection (if needed).

  @usage
  test module

*/
/*============================================================================*/
void a2_dl_phy_do_register_dma_to_dob_cb
(
  /*! The callback to be called whenever do_dma_from_dob tag status
      is received */
  a2_dl_phy_do_dma_to_dob_cb dob_tag_cb_fn_ptr
);

/*==============================================================================

  FUNCTION:  a2_dl_phy_do_dma_to_dob

==============================================================================*/
/*!
    @brief
    Adds following tasks to A2 DL PHY TASK Q:
     - DMA TASK (one source  <external memory> and one destination <dob>)
       - dest_addr is byte aligned address in DOB

    @note
    This function takes care of necessary cache flushing/invalidation of source/
    destination.
    This functions assumes that no virtual to physical mapping is needed for
    the address provided as part of the arguments i.e. virtual and physical
    memory address is same.

    @usage
    test module

    @return
    None
*/
/*============================================================================*/
void a2_dl_phy_do_dma_to_dob
(
  /*! source address (external memory) of the data to be moved to DecOB. */
  uint32                  src_addr,
  /*! source length in bytes */
  uint16                  src_len_in_bytes,
  /*! destination address (DecOB) where deciphered data needs to be copied.
      Length to be copied to destination is assumed to be same as
      "src_len_in_bytes" . This can be relative address (start of DOB addr as
      address 0) or physical address */
  uint32                  dest_dob_addr,
  /*! user data that needs to be returned to the caller */
  uint32                  user_data
);


#endif /* A2_DL_PHY_DO_H */

