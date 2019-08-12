#ifndef A2_UL_PHY_DO_H
#define A2_UL_PHY_DO_H
/*!
  @file a2_ul_phy_do.h

  @brief
  The interface to the uplink phy(reverse link) portion of the a2 driver for 
  1x/EvDo.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/api/a2/a2_ul_phy_do.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/30/13   am      CR405543 Crash due to freed DSM when chain length > 80
10/19/11   ar      added UL PHY support for DO
06/23/10   ar      Initial version

==============================================================================*/


/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <dsm.h>
#include <a2_common.h>

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*! Callback type to be register with a2_ul_phy_do_register_dma_to_eram_cb() */
typedef void (*a2_ul_phy_do_dma_to_eram_cb)
(
  uint32 /*! user_data provided via a2_ul_phy_do_dma_to_eram() */
);

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*==============================================================================

  FUNCTION:  a2_ul_phy_do_init

==============================================================================*/
/*!
  @brief
  Initialize the ul phy block

  @caller
  From A2 driver initialization function during driver bring up

*/
/*============================================================================*/
void a2_ul_phy_do_init
(
  void
);

/*==============================================================================

  FUNCTION:  a2_ul_phy_do_register_dma_to_eram_cb

==============================================================================*/
/*!
  @brief
  Registers a callback function to be called whenever do_write_transport_block
  tag status is processed in the UL PHY status queue.

  @note
  Registered callback can be used to send a signal to a given task to process
  transfer done associated with the *tag* returned as part of the call back
  function argument.
  Processing can be done in call back function context as well with the
  necessary mutex protection (if needed).
*/
/*============================================================================*/
void a2_ul_phy_do_register_dma_to_eram_cb
(
  /*! The callback to be called whenever a2_ul_phy_do_write_transport_block tag
      status is received/processed */
  a2_ul_phy_do_dma_to_eram_cb dma_to_eram_cb_fn_ptr
);

/*==============================================================================

  FUNCTION:  a2_ul_phy_do_dma_to_eram

==============================================================================*/
/*!
  @brief
  DMAs DO transport block to ERAM

  Data is pointed by src_dsm_ptr. this function goes thru the complete chain of
  dsm item till end of dsm chain (pkt_ptr set to NULL) is reached and move the
  contents of the same to indicated eram address.

  @note
  Please note that caller is responsible to make sure that dsm pointer cannot be
  released until data is moved to eram. User can request for the interrupt to
  get the dsm pointer to be released or can delay the release to later point.

*/
/*============================================================================*/
void a2_ul_phy_do_dma_to_eram
(

  /*! ERAM Destination address where data needs to be copied. This has to be a 
  relative address and not the physical ERAM address. This address is assumed to
  be byte aligned address. */
  uint32         dest_eram_addr,
  /*! source dsm pointer */
  dsm_item_type** src_dsm_ptr,
  /*!< The length of the data in bytes to be copied from source_address */
  uint16         src_len_in_bytes
);

/*==============================================================================

  FUNCTION:  a2_ul_phy_do_dma_to_32bit_reg

==============================================================================*/
/*!
  @brief
  Writes a DMA fill task to copy the passed value into the register address

  This api can be used to program the registers that need to be written once
  the transport block is copied to ERAM. Writing these registers through A2 
  taskq eliminates the need for interrupts for each dma to eram.
  
  @return
  none

*/
/*============================================================================*/
void a2_ul_phy_do_dma_to_32bit_reg
(
  /*! Destination address of the register */
  uint32 reg_addr,
  /*! value that needs to be written to register address */
  uint32 value
);

/*==============================================================================

  FUNCTION:  a2_ul_phy_do_commit

==============================================================================*/
/*!
    @brief
    Commits the write pointer of a2_ul_phy task queue.

    If "interrupt_required" argument is set to TRUE then tag task with user data
    is written and A2 UL PHY is indciated to interrupt SW whenever all the tasks
    written so far is executed. If "interrupt_required" argument is set to FALSE
    then it means that there is no need to receive an interrupt from A2 UL PHY.

    @note
    Takes care of necessary cache flushing of task queue before updating
    write pointer of the A2 UL PHY task queue

    @return
    None
*/
/*============================================================================*/
void a2_ul_phy_do_commit
(
  /*! user data that needs to be returned to the caller if interrupt is
      requested (see argument 'interrupt_reqd') */
  uint32         user_data,
  /*! indicate whether interrupt is required or not after data is moved to dest
      eram address */
  boolean        interrupt_reqd
);

#endif /* A2_UL_PHY_DO_H */

