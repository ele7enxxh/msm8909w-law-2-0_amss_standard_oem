#ifndef DSWCSDULTASK_V_H
#define DSWCSDULTASK_V_H
/*===========================================================================

             W C D M A   C I R C U I T - S W I T C H E D   D A T A
                           
                          U P L I N K   T A S K

                          H E A D E R   F I L E

DESCRIPTION
  This file contains data declarations and function prototypes for WCDMA 
  Circuit-Switched Data Uplink Task. 

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001 - 2009 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM5200/data/vcs/dswcsdultask.h_v   1.6   06 May 2002 14:55:30   degangy  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/inc/dswcsdultask_v.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/04/09   sa      AU level CMI modifications.
08/05/03   tmr     Updated watermark callbacks for DSM 2.
04/15/03   dgy     Updated the copyright.
11/18/02   dgy     Moved dswcsd_sdus_per_tti to dswcsdhdlr.c.
05/06/02   dgy     Changed a variable name.
03/15/02   dgy     Moved an extern to dswcsd.h.
02/21/02   dgy     Added Synchronous CS Data and a run-time switch to switch 
                   between Asynchronous and Synchronous CS Data.
09/20/01   dgy     Built SDUs in WCDMA CS UL task context instead of RLC 
                   task context. 
08/29/01   dgy     Changed FEATURE_WCDMA_CS_DATA to FEATURE_DATA_WCDMA_CS.
08/17/01   dgy     Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"


#if defined (FEATURE_WCDMA) || defined (FEATURE_TDSCDMA)

#ifdef FEATURE_DATA_WCDMA_CS
#include "dswcsd.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  WCDMA CS Data Uplink Task Signal Definitions
---------------------------------------------------------------------------*/

#define DSWCSD_UL_TASK_START_SIG    0x8000               /* TAST_START_SIG */
#define DSWCSD_UL_TASK_STOP_SIG     0x4000                /* TASK_STOP_SIG */
#define DSWCSD_UL_TASK_OFFLINE_SIG  0x2000             /* TASK_OFFLINE_SIG */
#define DSWCSD_UL_CMD_Q_SIG         0x1000         /* Command Queue signal */
#define DSWCSD_UL_DOG_HB_REPORT_SIG 0x0800                /* Dog HB Report */
#define DSWCSD_UL_TX_DATA_SIG       0x0400               /* TX data signal */
#define DSWCSD_UL_TIMER_SIG         0x0200                 /* Timer signal */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION DSWCSD_SET_UL_TX_DATA_SIG

DESCRIPTION
  This function sets the UL TX_DATA signal for the UL task, which tells the 
  UL task to send data to RLC.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void dswcsd_set_ul_tx_data_sig
#ifdef FEATURE_DSM_WM_CB
(
   dsm_watermark_type  *wm_ptr, 
   void                *user_data_ptr
);
#else
(
   void
);
#endif



/*===========================================================================

FUNCTION DSWCSD_UL_GET_CMD_BUF

DESCRIPTION
  This function allocates a command buffer from the WCDMA CS Data Uplink Task. 
  The caller of this function must check for a NULL return value, and either
  perform some error recovery or ERR_FATAL, as appropriate.

DEPENDENCIES
  None

RETURN VALUE
  Returns a pointer to a command buffer, if available, or NULL if no command
  buffers are available.

SIDE EFFECTS
  None

===========================================================================*/

extern dswcsd_cmd_type *dswcsd_ul_get_cmd_buf( void );


/*===========================================================================

FUNCTION DSWCSD_UL_PUT_CMD

DESCRIPTION
  This function puts a command on the WCDMA CS UL Task's command queue, and
  sets the DSWCSD_UL_CMD_Q_SIG.

DEPENDENCIES
  The caller of this function should have previously allocated a command
  buffer by calling dswcsd_ul_get_cmd_buf().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void  dswcsd_ul_put_cmd
(
  dswcsd_cmd_type *cmd_ptr              /* Pointer to a command to be sent */
);

#endif /* FEATURE_DATA_WCDMA_CS */

#endif /* FEATURE_WCDMA || FEATURE_TDSCDMA */


#endif /* DSWCSDULTASK_V_H */
