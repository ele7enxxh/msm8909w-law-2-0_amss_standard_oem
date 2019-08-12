/*!
  @file
  qsh_ss.h

  @brief
  Exports functionality to allocate memory for clients and trigger test 
  routines.
  
  @note
  This file is intended to have exactly one user. Behavior for multiple users
  is undefined.
*/

/*==============================================================================

  Copyright (c) 2015 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/api/qsh/backend/qsh_ss.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
08/28/15   ca      CR 898746: QSH rule management feature.
02/28/15   mm      Initial check-in                 
==============================================================================*/

#ifndef QSH_SS_H
#define QSH_SS_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <stdint.h>
#include <qsh.h>

/*==============================================================================

                         EXTERNAL VARIABLES

==============================================================================*/

/*! ULog handle */
extern ULogHandle qsh_ss_ulog_handle;

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*==============================================================================

  FUNCTION:  qsh_ss_init

==============================================================================*/
/*!
  @brief
  Initializes required resources.
*/
/*============================================================================*/
void qsh_ss_init
(
  void
);

/*==============================================================================

  FUNCTION:  qsh_ss_mem_current_alloc

==============================================================================*/
/*!
  @brief
  Gets heap space currently allocated in bytes.
  
  @note
  This is recommended to check whether or not to call mem_alloc()/mem_free().
  
  @return
  Size of heap memory currently allocated, or 0 if there is no space currently
  allocated.
*/
/*============================================================================*/
size_t qsh_ss_mem_current_alloc
(
  void
);

/*==============================================================================

  FUNCTION:  qsh_ss_mem_alloc

==============================================================================*/
/*!
  @brief
  Allocates memory for all clients currently registered.
  
  @return
  None
*/
/*============================================================================*/
void qsh_ss_mem_alloc
(
  void
);

/*==============================================================================

  FUNCTION:  qsh_ss_mem_free

==============================================================================*/
/*!
  @brief
  Frees memory previous allocated.
  
  @return
  None
*/
/*============================================================================*/
void qsh_ss_mem_free
(
  void
);

/*==============================================================================

  FUNCTION:  qsh_ss_dump

==============================================================================*/
/*!
  @brief
  Executes dump functionality for all clients using tag mask 
  QSH_DUMP_TAG_MASK_ALL.
*/
/*============================================================================*/
void qsh_ss_dump
(
  void
);

/*==============================================================================

  FUNCTION:  qsh_ss_metric_check_stop

==============================================================================*/
/*!
  @brief
  Increments timer count and returns whether or not the limit has been reached.
*/
/*============================================================================*/
boolean qsh_ss_metric_check_stop
(
  void
);

/*==============================================================================

  FUNCTION:  qsh_ss_metric_check_restart

==============================================================================*/
/*!
  @brief
  Returns whether or not the test should be re-run.
*/
/*============================================================================*/
boolean qsh_ss_metric_check_restart
(
  void
);

/*==============================================================================

  FUNCTION:  qsh_ss_metric_start

==============================================================================*/
/*!
  @brief
  Sends START for metrics.
*/
/*============================================================================*/
void qsh_ss_metric_start
(
  void
);

/*==============================================================================

  FUNCTION:  qsh_ss_metric_stop

==============================================================================*/
/*!
  @brief
  Sends STOP for metrics.
*/
/*============================================================================*/
void qsh_ss_metric_stop
(
  void
);

#endif /* QSH_SS_H */
