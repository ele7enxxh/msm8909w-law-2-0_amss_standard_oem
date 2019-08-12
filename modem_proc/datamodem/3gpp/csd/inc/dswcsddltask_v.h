#ifndef DSWCSDDLTASK_V_H
#define DSWCSDDLTASK_V_H
/*===========================================================================

             W C D M A   C I R C U I T - S W I T C H E D   D A T A

                         D O W N L I N K   T A S K
                         
                          H E A D E R   F I L E

DESCRIPTION
  This file contains data declarations and function prototypes for WCDMA 
  Circuit-Switched Data Downlink Task. 

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

  $PVCSPath:  L:/src/asw/MSM5200/data/vcs/dswcsddltask.h_v   1.2.1.0   07 Nov 2001 17:16:36   sparikh  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/inc/dswcsddltask_v.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/04/09   sa      AU level CMI modifications.
08/05/03   tmr     Updated watermark callbacks for DSM 2
04/15/03   dgy     Updated the copyright.
09/20/01   dgy     Added the callback funtion for the DL WM,
                   dswcsd_set_dl_tx_data_sig(), and the signal,
                   DSWCSD_DL_TX_DATA_SIG.
08/29/01   dgy     Changed FEATURE_WCDMA_CS_DATA to FEATURE_DATA_WCDMA_CS.
08/17/01   dgy     Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODDLE

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
  WCDMA CS Data Downlink Task Signal Definitions
---------------------------------------------------------------------------*/

#define DSWCSD_DL_TASK_START_SIG    0x8000               /* TAST_START_SIG */
#define DSWCSD_DL_TASK_STOP_SIG     0x4000                /* TASK_STOP_SIG */
#define DSWCSD_DL_TASK_OFFLINE_SIG  0x2000             /* TASK_OFFLINE_SIG */
#define DSWCSD_DL_CMD_Q_SIG         0x1000         /* Command Queue signal */
#define DSWCSD_DL_DOG_HB_REPORT_SIG 0x0800                /* Dog HB Report */
#define DSWCSD_DL_TX_DATA_SIG       0x0400               /* TX data signal */
#define DSWCSD_DL_TIMER_SIG         0x0200                 /* Timer signal */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION DSWCSD_SET_DL_TX_DATA_SIG

DESCRIPTION
  This function sets the DL TX_DATA signal for the DL task, which tells the 
  DL task to send data to SIO.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void dswcsd_set_dl_tx_data_sig
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

FUNCTION DSWCSD_DL_GET_CMD_BUF

DESCRIPTION
  This function allocates a command buffer from the WCDMA CS Data Downlink Task. 
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

extern dswcsd_cmd_type *dswcsd_dl_get_cmd_buf( void );


/*===========================================================================

FUNCTION DSWCSD_DL_PUT_CMD

DESCRIPTION
  This function puts a command on the WCDMA CS DL Task's command queue, and
  sets the DSWCSD_DL_CMD_Q_SIG.

DEPENDENCIES
  The caller of this function should have previously allocated a command
  buffer by calling dswcsd_dl_get_cmd_buf().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void  dswcsd_dl_put_cmd
(
  dswcsd_cmd_type *cmd_ptr              /* Pointer to a command to be sent */
);

#endif /* FEATURE_DATA_WCDMA_CS */

#endif /* FEATURE_WCDMA || FEATURE_TDSCDMA */


#endif /* DSWCSDDLTASK_V_H */
