/*!
  @file
  RF Dispatch Documentation

  @brief
  This module contains Documentation Manual for RF Tasks. This module 
  <b>SHOULD NOT</b> be included as header file in any part the embedded code.
*/

/*==============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/task/common/etc/rf_dispatch_documentation.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
01/16/13   aro     Doc for handle init
01/16/13   aro     Added callflow
01/15/13   aro     Added registration callflow
01/15/13   aro     Updated registration interface to pass dispatch handle
01/10/13   aro     Added off-target test cases to doxygen
07/09/13   aro     Initial Revision
 
==============================================================================*/


/* Hault Compile if this Header File is included in any source file */
#error "Do not include this file.  For documentation purposes only."

/*! 
  @mainpage RF Dispatch
 
  @details
  This module provides a common framework to perform generic command
  dispatching for tasks used by RF Software. This module also provides an
  feature to perform secondary dispatching after main dispatch. Secondary
  Dispatching allows introducing command specific dispatch feature. For
  instance, a sequence number based secondary dispatch can be used by any
  command to perform synchronization of messaging between RF and FW.
 
  <b> RF Dispatch Module does not hold any data pertaining to each task. It
  will be the responsibility of the task to store the dispatch data. This
  dispatch data will be pushed to the RF Dispatch module as dispatch handler,
  on which the RF dispatch module will perform the dispatching algorithm.</b>
 
  @image html rf_dispatch_implementation.jpg

*/

/*! 
  @defgroup RF_DISPATCH Primary Dispatch
  @details
  This module handles the first level of command dispatching. All
  registsred commands are grouped together based on the tech (8 MSB of command
  ID) and stored in several nodes of a linked list; where each each corresponds
  to a tech. Each registered command is allocated a opaque data container to
  store data pertaining to secondary dispatch. This allows to have different
  kinds of second level dispatching for each command in same linked list 
  node.
  @image html rf_dispatch_implementation.jpg
  @ingroup RF_TASK
 
    @defgroup RF_DISPATCH_INIT Handle Initialization
    @ingroup RF_DISPATCH
 
    @defgroup RF_DISPATCH_REG_DREG Registration Interface
    @ingroup RF_DISPATCH
 
    @defgroup RF_DISPATCH_QUERY Query Interface
    @ingroup RF_DISPATCH
 
    @defgroup RF_DISPATCH_MAIN_DISPATCH Dispatch
    @ingroup RF_DISPATCH
  
    @defgroup RF_DISPATCH_MAIN_DISPATCH_LL Linked List Implementation
    @ingroup RF_DISPATCH
 
    @defgroup RF_DISPATCH_MAIN_DISPATCH_REX Rex-MSGR
    @ingroup RF_DISPATCH
 
    @defgroup RF_DISPATCH_REG_CALLFLOW Registration Callflow
    @ingroup RF_DISPATCH
    
    @defgroup RF_DISPATCH_MAIN_DISPATCH_OFT Off-target Test case
    @ingroup RF_DISPATCH
 
*/

/*! 
  @defgroup RF_DISPATCH_SECONDARY Secondary Dispatch
  @details
  This module handles secondary dispatch implementation. The sub modules
  under this hold several feature specific secondary dispatching which is
  linked to the main dispatch   
  @image html rf_dispatch_implementation.jpg
  @ingroup RF_TASK
 
    @defgroup RF_DISPATCH_SECONDARY_SNUM Sequence Number
    @brief
    This module pertains to secondary dispatching based on sequence number.
    The priamry use of this is for RF and FW message synchronization. The basic
    working principal is that RF will generate a unique sequence number and send
    it to FW embedded in the message. RF will register a callback function
    with the dispatcher based on the same sequence number. FW loops back the
    sequence number. Once the command from FW is received by the dispatcher, it
    get get routed to the sequence number based callback via main dispatch
    and sequence number based secondary dispatcher. The callback function
    for the sequence number can be registered as dynamic registration 
    (registered everytime before sending command to FW) or static registration
    (register once).
    @ingroup RF_DISPATCH_SECONDARY
 
      @defgroup RF_DISPATCH_SECONDARY_SNUM_REG Registration Interface
      @ingroup RF_DISPATCH_SECONDARY_SNUM
 
      @defgroup RF_DISPATCH_SECONDARY_SNUM_DISPATCH Dispatch
      @ingroup RF_DISPATCH_SECONDARY_SNUM
 
      @defgroup RF_DISPATCH_SECONDARY_SNUM_POOL SeqNum Pool
      @ingroup RF_DISPATCH_SECONDARY_SNUM
 
      @defgroup RF_DISPATCH_SECONDARY_SNUM_HELPER Helper functions
      @ingroup RF_DISPATCH_SECONDARY_SNUM
 
      @defgroup RF_DISPATCH_SECONDARY_SNUM_UC_I Use Case I
      @brief
      Synchronization between RX_START and RX_START_RSP where there is no
      POST-BEFORE-WAIT
      @ingroup RF_DISPATCH_SECONDARY_SNUM
 
      @defgroup RF_DISPATCH_SECONDARY_SNUM_UC_II Use Case II
      @brief
      Synchronization between RX_START and RX_START_RSP where there
      is POST-BEFORE-WAIT
      @image html rf_dispatch_snum_uc2.jpg
      @ingroup RF_DISPATCH_SECONDARY_SNUM
 
      @defgroup RF_DISPATCH_SECONDARY_SNUM_OFT Off-target Test case
      @ingroup RF_DISPATCH_SECONDARY_SNUM 
*/

/*============================================================================*/
/*!
  @addtogroup RF_DISPATCH_REG_CALLFLOW
 
  @brief
  Call flow depicting the command registration, deregistration,
  And dispatching using command dispatch module 
 
  @image html rf_dispatch_reg_callflow.jpg
*/


/*============================================================================*/
/*!
  @addtogroup RF_DISPATCH_SECONDARY_SNUM_UC_I
 
  @brief
  Synchronization between RX_START and RX_START_RSP where there
  is POST-BEFORE-WAIT
 
  @image html rf_dispatch_snum_uc1.jpg
*/

/*============================================================================*/
/*!
  @addtogroup RF_DISPATCH_SECONDARY_SNUM_UC_II
 
  @brief
  Synchronization between RX_START and RX_START_RSP where there is no
  POST-BEFORE-WAIT

  @image html rf_dispatch_snum_uc2.jpg
*/

/*
  ------------------------------------------------------------------------------
  How to Generate Doxygen Documenation for this module?
  ------------------------------------------------------------------------------

  [1] Go to "doc" folder in CommandPrompt

  [2] Run the following Command
      >> doxygen doxyfile_rf_task

  [3] Open doc/html/index.html in your Browser
*/

