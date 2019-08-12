/*!
  @file
  rfca_task.h
 
  @brief
  Defines RFCA task signals and main task function.

*/


/*==============================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/rfca/inc/rfca_task.h#1 $
   
when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
06/10/13   aki     Mainline RFCA 
10/28/11   aki     Initial revision

==============================================================================*/



#ifndef _RFCA_TASK_H_
#define _RFCA_TASK_H_

#include "rfa_variation.h"
#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif


  /** Identifies the major version number of this API file. */
#define VER_RFCA_TASK_H_MAJOR     1
/** Identifies the minor version number of this API file. */
#define VER_RFCA_TASK_H_MINOR     1


#define RFCA_CMD_Q_SIG            0x0001    /* A command is available in the queue.       */
#define RFCA_CMD_RSP_SIG          0x0002    /* Command response signal                    */
#define RFCA_FTM_ASYNC_RESP_SIG   0x0004    /* Used to process async packets from FTM     */
#define RFCA_FTM_SEQ_LOG_MSG_SIG  0x0008    /* Used to process FTM Sequencer log messages */
#define RFCA_DIAG_CB_REDY_SIG     0x0010    /* Used to indicate diag callback is done     */
/**
 *
 * The RFCA task.
 *
 *
 * @param ignored
 *   parameter from rex that is ignored.
 *
 * This function never returns.
 *
 **************************************************************************/
void rfca_task (dword ignored);

#ifdef __cplusplus
}
#endif

#endif /* _RFCA_TASK_H_ */
