#ifndef A2_DIAG_H
  #define A2_DIAG_H
/*!
  @file a2_diag.h

  @brief
   A2 diag parameters.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/api/a2/a2_diag.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
11/02/15   am      CR924224 Add diag support for dup factor in per loopback
01/27/15   vd      CR735568: Requirement to corrupt CRC on UL PHY for a harq id
12/29/14   mm      Removing include for a2_sio_ext.h
12/16/14   mm      CR 772110: Adding diag interface for L2
12/05/14   sn      CR767131: QSH initial version
07/18/14   vd      CR630063: Integrating QSH feature
08/21/13   yjz     CR527081, Added A2_DIAG_CMD_CODE_REQUEST_ITEM_AUTO_WAKE_UP
02/05/13   bn      Added debug command to disable ul per flow control
12/17/12   ar      added support to print a2 pc stats
10/02/12   yuw     Enabled diag to set the destination of eMBMS data as discard
07/25/12   rp      Added the A2 log task and added stats collection & display 
07/13/12   rp      CR371608. Enable/Disable A2 DL_PER bridging via A2 diag
07/06/12   rp      CR371608 Added api's to enable/disable A2 peak data rate mode
06/05/12   rp      CR367832 Created the file to support diag commands for A2 and
                   add support for variable modification in A2.
==============================================================================*/


/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#include <comdef.h>
#include <msg.h>
#include <log.h>
#include <dsm.h>
#include <a2_common.h>
#include <diagpkt.h>
#include <diagcmd.h>
#include <diagdiag.h>

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*! @brief
     A2 Diag "RequestItem" request payload structure
*/
typedef PACK(struct)
{
  /*! Command code requested via RequestItem(enum: a2_diag_cmd_reqitem_code_e)*/
  uint16 cmd_code;      
  /*! CMD data can be used for different purposes depending on the cmd_code*/
  uint32 cmd_data_1;
  uint32 cmd_data_2;
  uint32 cmd_data_3;
} a2_diag_req_reqitem_s;

/*! @brief Prototype for callback registered by L2 */
typedef void (*a2_diag_cb_t)(a2_diag_req_reqitem_s *);

/*! @brief
     A2 Diag cmd codes currently supported
*/
enum a2_diag_cmd_reqitem_code_e
{
  A2_DIAG_CMD_CODE_REQUEST_ITEM_MIN = 0,
  A2_DIAG_CMD_CODE_REQUEST_ITEM_PEAK_DATA_RATE = 0x1000,
  A2_DIAG_CMD_CODE_REQUEST_ITEM_DUMP_DBG_REGS_SAVE = 0x2000,
  A2_DIAG_CMD_CODE_REQUEST_ITEM_A2_DL_PER_BRIDGE = 0x3000,
  A2_DIAG_CMD_CODE_REQUEST_ITEM_A2_LOG_STATS = 0x4000,
  A2_DIAG_CMD_CODE_REQUEST_ITEM_A2_LOG_DPL = 0x5000,
  A2_DIAG_CMD_CODE_REQUEST_ITEM_A2_DBG_MASK = 0x6000,
  A2_DIAG_CMD_CODE_REQUEST_ITEM_A2_ASSERT_MASK = 0x7000,
  A2_DIAG_CMD_CODE_REQUEST_ITEM_A2_EMBMS_MASK = 0x8000,
  A2_DIAG_CMD_CODE_REQUEST_ITEM_PRINT_A2_PC_STATS = 0x9000,
  A2_DIAG_CMD_CODE_REQUEST_ITEM_UL_FLOW_CONTROL_FLAG = 0xA000,
  A2_DIAG_CMD_CODE_REQUEST_ITEM_PRINT_A2_LIB_VER = 0xB000,
  A2_DIAG_CMD_CODE_REQEST_ITEM_RANDOM_DL_BAM_WAKEUP = 0xC000,
  A2_DIAG_CMD_CODE_REQUEST_ITEM_INVOKE_QSH = 0xD000,
  A2_DIAG_CMD_CODE_REQUEST_ITEM_ASSERT = 0xDEAD,
  A2_DIAG_CMD_CODE_REQUEST_ITEM_M_READ = 0xDEED,
  A2_DIAG_CMD_CODE_REQUEST_ITEM_PRINT_QSH_CFG = 0xE000,
  A2_DIAG_CMD_CODE_REQUEST_ITEM_CHANGE_QSH_CFG = 0xF000,
  A2_DIAG_CMD_CODE_REQUEST_ITEM_INVOKE_QSH_RESET_STATS = 0xF001,
  A2_DIAG_CMD_CODE_REQUEST_ITEM_PRINT_QSH_STATS = 0xF002,
  A2_DIAG_CMD_CODE_REQUEST_ITEM_PRINT_CORRUPT_CRC = 0xF003,
  A2_DIAG_CMD_CODE_REQUEST_ITEM_DUP_LB = 0xF004,
  A2_DIAG_CMD_CODE_REQUEST_ITEM_LTE_L2_CMD_MIN = 0xFF00,
  A2_DIAG_CMD_CODE_REQUEST_ITEM_LTE_L2_CMD_MAX = 0xFF0F, /* inclusive */
  A2_DIAG_CMD_CODE_REQUEST_ITEM_M_WRITE = 0xFEED,
  A2_DIAG_CMD_CODE_REQUEST_ITEM_MAX = 0xFFFF
};

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*==============================================================================

  FUNCTION:  a2_diag_init

==============================================================================*/
/*!
  @brief
  init function for a2_diag.c file. Should not be called by clients.

  @detail
  Function registers the diagnostic call back functions for A2

  @caller
  a2_task_main()

*/
/*============================================================================*/
void a2_diag_init
(
  void
);
/* a2_diag_init() */

/*==============================================================================

  FUNCTION:  a2_diag_lte_l2_cmd_cb_register

==============================================================================*/
/*!
  @brief
  Registers a callback function to handle commands between 
  A2_DIAG_CMD_CODE_REQUEST_ITEM_LTE_L2_CMD_MIN and 
  A2_DIAG_CMD_CODE_REQUEST_ITEM_LTE_L2_CMD_MAX.
*/
/*============================================================================*/
void a2_diag_lte_l2_cmd_cb_register
(
  a2_diag_cb_t  cb
);
/* a2_diag_lte_l2_cmd_cb_register() */

#endif /* A2_DIAG_H */
