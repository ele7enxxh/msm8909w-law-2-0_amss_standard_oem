#ifndef LOOPBACK_H
#define LOOPBACK_H
/*===========================================================================

               L O O P B A C K    H E A D E R    F I L E

DESCRIPTION
  This file contains declarations associated with the Mobile Station
  Loopback service option.

  Copyright (c) 1994 - 2013 Qualcomm Technologies, Inc. 
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

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/loopback.h_v   1.0.2.0   30 Nov 2001 16:56:48   fchan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/loopback.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/07/11   trc     Changed interface of loopback_rx()
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
10/08/10   ag      Renamed Markov, Loopback related counters.
06/15/10   ag      Added support for enhanced loopback service option.
08/14/95   gb      Added new Loopback_13K service option support.
03/15/94   jca      Created file.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "1x_variation.h"
#include "comdef.h"
#include "cai.h"
#include "cai_v.h"
#include "cai_i.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#ifdef FEATURE_IS2000_1X_ADV
/*
 * Type of Loopback Testing
 */
 typedef enum
{
  LB_FCH_NORMAL_TEST,
  LB_RFCH_BLANKING_TEST
} loopback_test_type;

/*
 * CTL_REC_TYPE Message Field  definitions
 */
#define LB_CTL_TYP      0     /* byte index for ctl type field */
#define LB_FCH_DIR      0     /* Control Directive for FCH testig */
#define LB_R_FCH_0_DIR  1     /* Control Directive for R-FCH 0 bps testing */
#define LB_RACKCH1_DIR  2     /* Control Directive for R-ACKCH1 */
#define LB_FACKCH_DIR   3     /* Control Directive for F-ACKCH */
#define LB_CTR_RET_DIR  4     /* Counter Retrieval Directive */
#define LB_RACKCH2_DIR  5     /* Control Directive for R-ACKCH2 */

#define LB_VECT         1     /* byte index for counters retrieve vector id */

/* Structure for Loopback Service Option frame statistics */
typedef struct
{
dword lso_fackch_eack_rack; /* F-ACKCH expected ACK received ACK */
dword lso_fackch_eack_rnak; /* F-ACKCH expected ACK received NAK */
dword lso_fackch_enak_rack; /* F-ACKCH expected NAK received ACK */
dword lso_fackch_enak_rnak; /* F-ACKCH expected NAK received NAK */
} lso_rx_ackch_type;
#endif /* FEATURE_IS2000_1X_ADV */

/* <EJECT> */
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION LOOPBACK_RX

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

extern void loopback_rx
(
  byte *bit_ptr,
  word bit_position,
  word bit_len,
  word frame_category
);

/* <EJECT> */
/*===========================================================================

FUNCTION LOOPBACK_TX

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  The frame rate to use is returned.

SIDE EFFECTS

===========================================================================*/

extern cai_data_rate_type loopback_tx
(
  cai_data_rate_type max_rate,
    /* Max rate for this frame */
  cai_frame_type     *frame_data
    /* Pointer to loopback data bits */
);

/* <EJECT> */
/*===========================================================================

FUNCTION LOOPBACK_INIT

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

extern void loopback_init( int rate_set );

#ifdef FEATURE_IS2000_1X_ADV
/*===========================================================================

FUNCTION LOOPBACK_LOG_RX

DESCRIPTION
  This function counts up the Loopback SO RX statistics.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void loopback_log_rx(word mux);

/*===========================================================================
FUNCTION LOOPBACK_GET_VECT

DESCRIPTION
  This function updates a given storage with the appropriate loopback stats.

DEPENDENCIES
  None.

RETURN VALUE
  Length of vector (vector elements are dwords here).

SIDE EFFECTS
  None.

===========================================================================*/

extern byte loopback_get_vect
(
  byte vect_counter_id,    /* id of vector of counters */
  dword *tsf               /* address of storage for vector */
);

/*===========================================================================

FUNCTION LOOPBACK_PROC_CTL_DIR

DESCRIPTION
  Processes the service option control message directives for
  loopback service option.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void loopback_proc_ctl_dir
(
  const byte tsf[]
);
#endif /* FEATURE_IS2000_1X_ADV */

#endif /* LOOPBACK_H */
