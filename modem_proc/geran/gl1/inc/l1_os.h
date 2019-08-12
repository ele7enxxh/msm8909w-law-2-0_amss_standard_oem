#ifndef L1_OS_H
#define L1_OS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  L1   OPERATING SYSTEM SIGNALS
                       H E A D E R   F I L E

DESCRIPTION
   Layer 1 Rex Signal definitions

EXTERNALIZED FUNCTIONS

Copyright (c) 2002-2015 Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl1/inc/l1_os.h#1 $
$DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who      what, where, why
--------   -------- ---------------------------------------------
21/04/15   cs        CR808438 Ensure any pending TT requests are sent before cell
                     information is updated
07/04/14   cja       CR644445 Mainline feature FEATURE_GSM_ASYNC_FW_INTF
28/11/13   cja       CR521061 Remove unused RF API and mainline FEATURE_GSM_RFA_TASK.
16/09/13   cs        CR503042 Updated XO RGS handling for Triton
14/09/13   ap/aga    CR545423 - G2W Tune Away Feature
21/06/13   cs        Major Triton TSTS Syncup
25/03/13   mc        CR464496 : COEX Support - GERAN arbitrator SW CXM integration
19/06/12   cs        CR370453 Use the correct wait sig for the TRM/TCXO Manager timeouts
25/05/12   pg        Add support for RF Task
18/05/12   ky        Async message router Interface Changes between SW & FW
10/02/12   ws        CR 36194 Enable TD-SCDMA IRAT for GERAN
21/01/11   cs        Remove NCELL power monitors signal
29/11/10   og        Adding G2L support.
28/04/10   nt        CR234959 Optimisation to NCELL power monitors in the idle mode
10/05/07   cs        Frequency error update changes for Aries GPS
06/01/07   cs        Remove common signal definitions
10/11/06   agv       Update for 7200A TRM and TCXO Mgr3.0
26/09/06   agv       Initial Version.

===========================================================================*/


/*===========================================================================

                DEFINITIONS AND DECLARATIONS

===========================================================================*/

#include "geran_variation.h"

/*===========================================================================

                         REX SIGNALS

===========================================================================*/

/* A command is available in the queue. */
#define L1_CMD_Q_SIG                 0x00000001

/* Used to signal that  WD rpt is reqd. */
#define L1_RPT_TIMER_SIG             0x00000002

/* Used by the HW module to signal rdy. */
#define L1_WAIT_HW_SIG               0x00000004

/* MDSP download timeout signal */
#define L1_TIMEOUT_HW_SIG            0x00000008

#define L1_ISR_WATCHDOG_TIMER_SIG    0x00000010

#ifdef FEATURE_GSM_COEX_SW_CXM
#define GL1_CXM_MSGR_SIG             0x00000020
#endif /* FEATURE_GSM_COEX_SW_CXM */

#define GL1_GFW_ASYNC_INTF_MSG_ROUTER_SIG \
                                     0x00000040

#define GL1_RF_TASK_MSGR_SIG         0x00000080

/*  Used by isr to signal task  */
#define SERV_MEAS_ISR_SIG            0x00000100
#define GPL1_WAIT_FOR_ISR_TO_GET_FN  0x00000200

/* This is in gl1rlc.h so be careful        */
/* #define RLC_PH_DATA_REQ_SIG       0x00000400 */

#define GL1_WAIT_FOR_DPC_TO_FINISH   0x00000800

#if defined(FEATURE_GSM_TO_LTE) || defined(FEATURE_GSM_TDS)
#define GL1_MSG_ROUTER_SIG           0x00008000
#endif

/* CM task signals that it's running
 * (so gsm_diag fn that calls it can safely be registered)
 */
#define CM_TASK_STARTED_SIG          0x00010000

/*===========================================================================

                EXTERN DEFINITIONS AND DECLARATIONS

===========================================================================*/

/* Used when waiting for confirmations from TRM */
extern rex_timer_type  gsm_l1_timeout_timer[];

#endif /* L1_OS_H */
