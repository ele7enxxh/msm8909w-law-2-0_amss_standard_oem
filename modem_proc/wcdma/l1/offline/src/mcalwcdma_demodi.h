#ifndef MCALWCDMA_DEMODI_H
#define MCALWCDMA_DEMODI_H

/*============================================================================*/
/** @file  
 * This module has definition and declaration related to MCAL WCDMA DEMOD
 * (internal only) module APIs
 */
/*============================================================================*/

/*============================================================================
Copyright (c) 2007 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/mcalwcdma_demodi.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
07/16/09    hk      Reorganized the code to remove INC/SRC violations
04/17/09    rgn     Fixed compiler warnings
11/03/08    rgn     Decreased status dump wait time before polling to 4000us
10/02/08    yh      Added more sanity checks
07/18/07    yh/mmr  Initial revision.

===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "comdef.h"
#include "mdspsync.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
/** Check for finger out-of-bounds */
#define MCALWCDMA_FING_BOUND_CHECK( finger )         \
  if(!(finger < MCALWCDMA_MAX_NUM_FINGERS))    \
  {                                                  \
    ERR_FATAL("Fing index out of bounds", 0, 0, 0 ); \
  }

/** Check for cell out-of-bounds */
#define MCALWCDMA_CELL_BOUND_CHECK( cell )           \
  if(!(cell < MCALWCDMA_MAX_NUM_CELLS))        \
  {                                                  \
    ERR_FATAL("Cell index out of bounds", 0, 0, 0 ); \
  }                                                  \

/** Check for NULL pointers within MCAL DEMOD structs   */
#define MCALWCDMA_DEMOD_CHECK_PTR( ptr )             \
  if( !ptr )                                         \
  {                                                  \
    ERR_FATAL("MCAL DEMOD: Null PTR", 0, 0, 0 );     \
  }                                                  \

/** Check for STTD   */
#define MCALWCDMA_DEMOD_GET_TXDIV_STATUS( divval )   \
   (divval == WFW_DEMOD_CELL_DIV_MODE_TD)

/** Number of microseconds required for status dump to
 *  complete */
#define MCALWCDMA_DEMOD_STATUS_DUMP_MICROSECONDS_NEEDED    4000
/** Number of attempts to poll status dump status */
#define MCALWCDMA_DEMOD_STATUS_DUMP_POLLING_RETRIALS       10
/** Number of bits representing cx8 units for refcnt and tst in
 *  dsd */
#define MCALWCDMA_DEMOD_STATUS_DUMP_CNT_IN_FRAME_BMSK 0x7FFFF
#define MCALWCDMA_DEMOD_STATUS_DUMP_FRAME_CNT_BMSK    0xFFF80000

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* Call back function type declaration */
/* ----------------------------------- */

/* Struct type declaration */
/* ----------------------- */
/** Make alias for the FW/ARM memory */
typedef WfwDemodFingerInfoStruct mcalwcdma_demod_fw_fing_config_struct_type;
typedef WfwDemodCellInfoStruct   mcalwcdma_demod_fw_cell_config_struct_type;
typedef WfwDemodStatusDumpStruct mcalwcdma_demod_fw_status_dump_struct_type;

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===========================================================================*/
/**
 * This function is a precallback for mdsp_sync. This is called right before
 * sending the mdsp demod command to FW. This function will mark the cell
 * and finger as accepted by MDSP.
 *
 */
/*==========================================================================*/
extern void mcalwcdma_demod_process_finger_cell_cfg(void);

/*===========================================================================*/
/**
 * This function is a postcallback for mdsp_sync. This is called upon 
 * receiving the mdsp done interrupt. Reception of this call means FW 
 * accepted the command sent and it will start processing. This does not 
 * mean that FW finished processing the command. In this function, the 
 * status for the command is checked. If status is zero (successful cmd), 
 * this function does nothing. On the other hand, a nonzero value will trigger 
 * a call to WHL1, notifying the failure of this command.
 * NOTE: MDSP will send status failure if a finger or cell has violated 
 * the 3 slots delay requirement.
 *
 */
/*=========================================================================*/
extern void mcalwcdma_demod_finger_cell_cfg_done(
  /** Status of cmd sent to mDSP */
  uint16 status);

/*===========================================================================*/
/**
 * This function is a handler for the DEMOD done interrupt sent by MDSP.
 * It checks for the finger and cell that has finished processing and 
 * notifies WHL1 about them.
 *
 */
/*=========================================================================*/
extern void mcalwcdma_demod_finger_cell_cfg_wrapup(void);

/*===========================================================================*/
/**
 * This function is a wrap up function invoked after the MDSP has completed
 * the status dump. It populates the status dump structure within MCAL.
 */
/*==========================================================================*/
extern void mcalwcdma_demod_status_dump_wrapup(
  /** Status of cmd sent to mDSP */
  uint16 status);

#endif /* MCALWCDMA_DEMODI_H */
