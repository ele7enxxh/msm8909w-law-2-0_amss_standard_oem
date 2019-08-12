#ifndef MUX1XDIAG_H
#define MUX1XDIAG_H
/*==========================================================================

                      1x MUX DIAGnostic services header

  Description:  Interfaces for external subsystems

  Copyright (c) 2003 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                            Edit History

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/mux/inc/mux1xdiag.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/03/10   trc     Rewrote this file to only export external interfaces
04/01/09   jtm     Added customer.h
10/21/03   bn      Added support for retrieving paging statistic counters via DIAG
05/22/03   vlc     Added extern prototype for mux1xdiag_init().
04/15/03   bn      Created new file for DIAG MUX1X sub-system access.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
//common
#include "comdef.h"
#include "customer.h"

// NOT WANTED HERE, but cdma2kdiag.c and mclog.c depend upon this #include
// being done for them indirectly via this file.
#include "diagcmd.h"
// Get rid of this at the earliest convenience!!!

/*===========================================================================

                EXPORTED FUNCTIONS

===========================================================================*/

/*--------------------------------------------------------------------------
  MUX1XDIAG_INIT() - Perform initial registration of DIAG packets
--------------------------------------------------------------------------*/
extern void mux1xdiag_init (void);


/*===========================================================================

              EXPORTED DEFINITIONS AND DECLARATIONS

This section contains definitions for constants, macros, types, variables
and other items exported by this module.

===========================================================================*/

/* Data structure for Frame Error Rate
*/
typedef struct
{

  uint16    meas_frames;
    /* Indicate # of measured frames depended on PWR_REP_FRAMES */

  uint16    err_frames;
    /* Indicate # of error frames during measured frames */

} mux1xdiag_fer_s_type;

#endif /* MUX1XDIAG_H */
