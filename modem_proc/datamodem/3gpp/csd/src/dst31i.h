#ifndef DST31I_H
#define DST31I_H

/*=============================================================================
**
**
**                        D A T A   S E R V I C E S
**
**           G S M  C I R C U I T - S W I T C H E D  D A T A
**
**                   F A X  P R O T O C O L  S T A C K
**
**                     T 3 1  F A X  S E R V I C E S
**
**                I N T E R N A L   H E A D E R   F I L E
**
** Description:
**
**   This file is the header file for the GCSD-FPS T31 Fax Services layer.
**   It contains the definitions of data structures, defined and
**   enumerated constants, and function prototypes used internally by T31.
**

 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Copyright (c) 2002 - 2009 Qualcomm Technologies Incorporated. All Rights Reserved
                            Qualcomm Proprietary

  This software is controlled by the United States Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dst31i.h#1 $

when      who   what, where, why
--------  ---   ---------------------------------------------------------------
04/01/11  TTV   Support for Q6 free floating.
03/04/09  SA    AU level CMI modifications.
03/13/08  SA    Fixing Klocwork issues.
04/06/06  AR    Replace use of ATCOP globals with control block elements.
03/01/06  AR    Add CM call ID to control block structure.
07/15/05  AR    Adjust DST31_TIMER* macros to report error conditions.
06/07/04  RLP   Modified to terminate the session if the time it takes to setup 
                  the T.30 session takes too long and the local fax appl aborts 
                  an implied FTH/FRH command.  In this case the fax appl will 
                  not send any further AT commands and this was causing the 
                  downlink watermark to fill up.
05/30/04  AR    Lint corrections.
09/05/03  RLP   Added new field to the control block structure used when 
                  sending the final AT response to the local DTE. 
                Removed and unused parameter from a function prototype.                 
06/04/03  RLP   Removed sprintf()s from the DEBUG message macros as they were
                causing some performance issues and are not needed.
04/25/03  TMR   Changes for new UMTS CS data architecture
03/19/03  RLP   GCF certification changes to fix MT fax issue.
02/10/03  RLP   Spain IOT changes.
                  - Added DST31_FRH_MAX_BYTES_PER_20MS define to limit the
                    amount of +FRH data to process per 20ms interval.
01/14/03  RLP   Combined +FTH/+FTM SIO/TPS watermark sizings so now the UL
                  watermarks uses the sames settings for both states.
                Code review updates.
12/20/02  RLP   Changed SIO flow control macros to use new MM functions.
                Added +FRM data limit define.  Lowered +FTM limit.
11/15/02  RLP   Changes for GCF certification support.
11/04/02  RLP   Nortel IOT updates from Stage 2 testing.
09/11/02  RLP   MMO2 IOT changes.
                  - Changed flow control levels.
                  - Added carrier_detect to control block structure.
07/19/02  RLP   Remove frh_data_pending, dsat_result, dsat_result_type fields 
                  from control block structure.  These are no longer used.
07/12/02  RLP   Changes to maintain TREX compatibilty.
06/25/02  RLP   Added extern references for new dst31i_dsm_pushdown wrappers.
06/21/02  RLP   First pass changes to get fax working with Surf/Racal.
03/14/02  RLP   Initial release.
=============================================================================*/

/*=============================================================================
** INCLUDES
**===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stringl/stringl.h>

#ifdef FEATURE_DATA_GCSD_FAX
#include "stdio.h"
#include "rex.h"
#include "dsm.h"
#include "queue.h"
#include "ds3gmgr.h"
#include "dsgcsdfa.h"
#include "dsfps.h"
#include "dsgcsd_task_v.h"
#include "ds3gsiolib_ex.h"

#include "ds3gsiolib.h"


/*=============================================================================
** DEFINES
**===========================================================================*/

/* Call types */
#define DST31_MO_FAX_CALL                       1  /* Mobile Originated */
#define DST31_MT_FAX_CALL                       2  /* Mobile Terminted */

/* HDLC control definitions */
#define DST31_HDLC_START_FLAG_OFFSET            0
#define DST31_HDLC_ADDRESS_OFFSET               1
#define DST31_HDLC_CONTROL_FIELD_OFFSET         2
#define DST31_HDLC_DATA_FIELD_OFFSET            3

#define DST31_HDLC_FLAG                         0x7E   /* HDLC Flag */
#define DST31_HDLC_FAX_ADDRESS                  0xFF   /* Static fax address */
#define DST31_HDLC_CONTROL_PF0                  0x03   /* Sent msb first */
#define DST31_HDLC_CONTROL_PF1                  0x13   /* Sent msb first */
#define DST31_HDLC_VALID_FCS                    0x1D0F /* Valid T.30 HDLC FCS */
#define DST31_HDLC_FINAL_FRAME                  DST31_HDLC_CONTROL_PF1

#define DST31_HDLC_FRAME_END                    DST31_HDLC_FLAG

#define DST31_HDLC_MIN_FRAME_SZ                 5

/* T.30 Protocol Frame Identifier definitions */
#define DST31_T30_FCF_MASK                      0xFE

#define DST31_T30_DIS_FRAME_ID                  0x01
#define DST31_T30_CSI_FRAME_ID                  0x02
#define DST31_T30_NSF_FRAME_ID                  0x04

#define DST31_T30_CTS_FRAME_ID_MASK             0x80
#define DST31_T30_DTC_FRAME_ID                  0x81
#define DST31_T30_CIG_FRAME_ID                  0x82
#define DST31_T30_SPWD_FRAME_ID                 0x83
#define DST31_T30_NSC_FRAME_ID                  0x84
#define DST31_T30_SEP_FRAME_ID                  0x85

#define DST31_T30_CTR_FRAME_ID_MASK             0x40
#define DST31_T30_DCS_FRAME_ID                  0x82
#define DST31_T30_TSI_FRAME_ID                  0x42
#define DST31_T30_SUB_FRAME_ID                  0x43
#define DST31_T30_NSS_FRAME_ID                  0x44
#define DST31_T30_RPWD_FRAME_ID                 0x45
#define DST31_T30_TCF_FRAME_ID                  0x00  /* NO HDLC FRAME USED */

#define DST31_T30_FTT_FRAME_ID                  0x21
#define DST31_T30_CTR_FRAME_ID                  0x22

#define DST31_T30_EOM_FRAME_ID                  0x8E
#define DST31_T30_MPS_FRAME_ID                  0x4E
#define DST31_T30_EOP_FRAME_ID                  0x2E
#define DST31_T30_PRI_EOM_FRAME_ID              0x9E
#define DST31_T30_PRI_MPS_FRAME_ID              0x5E
#define DST31_T30_PRI_EOP_FRAME_ID              0x3E

#define DST31_T30_MCF_FRAME_ID                  0x8C
#define DST31_T30_RTP_FRAME_ID                  0xCC
#define DST31_T30_PIP_FRAME_ID                  0xAC
#define DST31_T30_RTN_FRAME_ID                  0x4C
#define DST31_T30_PIN_FRAME_ID                  0x2C

#define DST31_T30_DCN_FRAME_ID                  0xFA
#define DST31_T30_CRP_FRAME_ID                  0x1A
/* End of T.30 Protocol Frame Identifiers */

/* Frame and message control definitions */
#define DST31_DLE_CHAR                          0x10
#define DST31_ETX_CHAR                          0x03
#define DST31_SUB_CHAR                          0x1A

#define DST31_DLE_CHAR_REV                      0x08  /* Bit reversed */
#define DST31_ETX_CHAR_REV                      0xC0  /* Bit reversed */
#define DST31_SUB_CHAR_REV                      0x58  /* Bit reversed */

#define DST31_FCS_CALC_PENDING                  0
#define DST31_FCS_CALC_DONE_FAIL                1
#define DST31_FCS_CALC_DONE_PASS                2

#define DST31_FTH_XMIT_FLAGS                    0
#define DST31_FTH_XMIT_DATA                     1

#define DST31_FTH_MAX_OPENING_FLAGS             2
#define DST31_FTH_MAX_PREAMBLE_SIZE             32
#define DST31_FTH_MAX_BYTES_PER_20MS            8
#define DST31_FRH_MAX_BYTES_PER_20MS            8
#define DST31_FTM_MAX_BYTES_PER_20MS            75
#define DST31_FRM_MAX_BYTES_PER_20MS            75

/* Tx and Rx buffer sizes - CHANGE AT YOUR OWN RISK! 
** Better Yet - DON'T CHANGE THESE 
*/
#define DST31_MAX_BUF_SZ                        128
#define DST31_MAX_IO_DATA_SZ                   (DST31_MAX_BUF_SZ * 10)

#define DST31_MAX_SIO_RX_BUFS                   10
#define DST31_MAX_SIO_TX_BUFS                   7

#define DST31_MAX_TPS_RX_BUFS                   10
#define DST31_MAX_TPS_TX_BUFS                   7

#define DST31_MAX_SIO_RX_DATA_SZ \
  (DST31_MAX_BUF_SZ * DST31_MAX_SIO_RX_BUFS)
#define DST31_MAX_SIO_TX_DATA_SZ \
  (DST31_MAX_BUF_SZ * DST31_MAX_SIO_TX_BUFS)

#define DST31_MAX_TPS_RX_DATA_SZ \
  (DST31_MAX_BUF_SZ * DST31_MAX_TPS_RX_BUFS)
#define DST31_MAX_TPS_TX_DATA_SZ \
  (DST31_MAX_BUF_SZ * DST31_MAX_TPS_TX_BUFS)

/* SIO and TPS watermark control threshold definitions */
#define DST31_SIO_DL_LO_WM_THRESHOLD           0
#define DST31_SIO_DL_HI_WM_THRESHOLD           0
#define DST31_SIO_DL_DONT_EXCEED_WM_THRESHOLD  \
  (DST31_MAX_SIO_RX_DATA_SZ * 2)

#define DST31_SIO_UL_LO_WM_THRESHOLD       \
  (DST31_MAX_SIO_TX_DATA_SZ / 5)
#define DST31_SIO_UL_HI_WM_THRESHOLD       \
  DST31_MAX_SIO_TX_DATA_SZ
#define DST31_SIO_UL_DONT_EXCEED_WM_THRESHOLD  \
  (DST31_MAX_SIO_TX_DATA_SZ * 2)

#define DST31_TPS_DL_LO_WM_THRESHOLD           0
#define DST31_TPS_DL_HI_WM_THRESHOLD           0
#define DST31_TPS_DL_DONT_EXCEED_WM_THRESHOLD  \
  (DST31_MAX_TPS_RX_DATA_SZ * 2)

#define DST31_TPS_UL_LO_WM_THRESHOLD       \
  (DST31_MAX_TPS_TX_DATA_SZ / 4)
#define DST31_TPS_UL_HI_WM_THRESHOLD       \
  DST31_MAX_TPS_TX_DATA_SZ
#define DST31_TPS_UL_DONT_EXCEED_WM_THRESHOLD  \
  (DST31_MAX_TPS_TX_DATA_SZ * 2)

/* Default T.31 timeout values for WAIT_FOR_ORDER state */
#define DST31_WFO_DEFAULT_TIMEOUT               10000  /* in milliseconds */
#define DST31_DCN_DEFAULT_TIMEOUT               1500   /* in milliseconds */

/* Default T.31 timeout values for +FCL and +FIT if not set by AT commands */
#define DST31_FCL_DEFAULT_TIMEOUT               7000   /* in milliseconds */
#define DST31_FIT_DEFAULT_TIMEOUT               7000   /* in milliseconds */

/* Timeout value used to send the +FRH/+FRM CONNECT response to the local
** TE in the event that 'carrier' is not detected from the remote end.  By
** going ahead and sending the CONNECT, it gives the local TE additional
** delay time to await for the data to arrive.
*/
#define DST31_CD_TIMEOUT                        2000   /* in milliseconds */

/*=============================================================================
** MACROS
**===========================================================================*/

#ifndef ENABLE_GSM_TREX_NETWORK_INTERFACE

#if 0
  #define T31_MSG_DETAIL(str, _a, _b, _c)  \
    MSG_LOW(str, _a, _b, _c)
#else
  #define T31_MSG_DETAIL(str, _a, _b, _c)
#endif

#define T31_MSG_LOW(str, _a, _b, _c)  \
    MSG_LOW(str, _a, _b, _c)
#define T31_MSG_MED(str, _a, _b, _c)  \
    MSG_MED(str, _a, _b, _c)
#define T31_MSG_HIGH(str, _a, _b, _c)  \
    MSG_HIGH(str, _a, _b, _c)
#define T31_ERR(str, _a, _b, _c)  \
    ERR(str, _a, _b, _c)
#define T31_MSG_STATE_CHANGE(_a, _b, _c, _d)  \
    MSG_HIGH("State change: S%d -> S%d",_b, _d, 0); 

#else

#if 0
  #define T31_MSG_DETAIL(str, _a, _b, _c)  \
  { char buf[120]; \
    (void) sprintf(buf, "%s: %s", t31_ctrl_blk->i_name, str); \
    MSG_LOW(buf, _a, _b, _c); } 
#else
  #define T31_MSG_DETAIL(str, _a, _b, _c)
#endif

/* NOTE: these sprintfs are a bit CPU intensive and can cause
** performance issues given we are already tight on CPU usage
** during a fax call.  These macros were mainly used during
** the TREX development phase and were carried over to the target
** builds.  They are no longer used in target builds.
*/
#define MAX_BUF_SIZ    120
#define T31_MSG_LOW(str, _a, _b, _c)  \
  { static char buf[MAX_BUF_SIZ]; \
    (void) snprintf(buf,MAX_BUF_SIZ, "%s: %s", t31_ctrl_blk->i_name, (str)); \
    MSG_LOW(str, _a, _b, _c); }
#define T31_MSG_MED(str, _a, _b, _c)  \
  { static char buf[MAX_BUF_SIZ]; \
    (void) snprintf(buf,MAX_BUF_SIZ, "%s: %s", t31_ctrl_blk->i_name, (str)); \
    MSG_MED(buf, _a, _b, _c); }
#define T31_MSG_HIGH(str, _a, _b, _c)  \
  { static char buf[MAX_BUF_SIZ]; \
    (void) snprintf(buf,MAX_BUF_SIZ, "%s: %s", t31_ctrl_blk->i_name, (str)); \
    MSG_HIGH(buf, _a, _b, _c); }
#define T31_ERR(str, _a, _b, _c)  \
  { static char buf[MAX_BUF_SIZ]; \
    (void) snprintf(buf,MAX_BUF_SIZ, "%s: %s", t31_ctrl_blk->i_name, str); \
    ERR(buf, _a, _b, _c); }
#define T31_MSG_STATE_CHANGE(_a, _b, _c, _d)  \
  { static char buf[MAX_BUF_SIZ]; \
    (void) snprintf(buf,MAX_BUF_SIZ, "%s: State change %s(%d)->%s(%d)", \
             t31_ctrl_blk->i_name, (_a), (_b), (_c), (_d)); \
    MSG_HIGH(buf, 0, 0, 0); }

#endif

/* Macros to assert/deassert SIO flow control */


#define DST31_SIO_FLOWCTL_ASSERT() \
  if (t31_ctrl_blk->flags.sio_direct_flowctl_asserted == FALSE) \
  { \
    T31_MSG_HIGH("SIO-UL:Manual flowctl ASSERTED",0,0,0); \
    ds3g_siolib_ex_set_inbound_flow(DS_FLOW_GCSD_MASK, DS_FLOW_DISABLE,\
                      ds3g_siolib_ex_get_csd_port()   ); \
    t31_ctrl_blk->flags.sio_direct_flowctl_asserted = TRUE; \
  }
#define DST31_SIO_FLOWCTL_DEASSERT() \
  if (t31_ctrl_blk->flags.sio_direct_flowctl_asserted == TRUE) \
  { \
    T31_MSG_HIGH("SIO-UL:Manual flowctl DEASSERTED",0,0,0); \
    ds3g_siolib_ex_set_inbound_flow(DS_FLOW_GCSD_MASK, DS_FLOW_ENABLE, \
                           ds3g_siolib_ex_get_csd_port()); \
    t31_ctrl_blk->flags.sio_direct_flowctl_asserted = FALSE; \
  }



/* Macros to detect if the fax session is in the training phase */
#define DST31_T30_DCS_FRAME(frame_ptr) \
  ((frame_ptr[2] & DST31_T30_FCF_MASK) == DST31_T30_DCS_FRAME_ID)

#define DST31_T30_SET_TRAINING_PHASE_ACTIVE() \
  if (t31_ctrl_blk->flags.training_phase_active == FALSE) \
  { \
    T31_MSG_HIGH("Training Phase started",0,0,0); \
    t31_ctrl_blk->flags.training_phase_active = TRUE; \
  }

#define DST31_T30_SET_TRAINING_PHASE_INACTIVE() \
  if (t31_ctrl_blk->flags.training_phase_active == TRUE) \
  { \
    T31_MSG_HIGH("Training Phase completed",0,0,0); \
    t31_ctrl_blk->flags.training_phase_active = FALSE; \
  }

#define DST31_T30_TRAINING_PHASE_ACTIVE() \
  (t31_ctrl_blk->flags.training_phase_active == TRUE)
   
/* Macro to reset the internal IO data buffer used to build
** frames from data received via SIO or from the TPS.
*/
#define DST31_RESET_IO_BUFFER() \
  t31_ctrl_blk->io_data.cnt = 0; \
  t31_ctrl_blk->io_data.fcs = 0; \
  t31_ctrl_blk->io_data.last_ftm_tx_char = 0; \
  t31_ctrl_blk->io_data.dle_data_state = DST31_FIND_DLE_STATE;


#define DST31_CLR_IO_BUFFER() \
  t31_ctrl_blk->io_data.cnt = 0;


/* Macros to control switching SIO between online-command and online-
** data modes.
*/
#define DST31_SAVE_SIO_PROCESSING_MODE(new_sio_mode) \
  t31_ctrl_blk->sio_mode = new_sio_mode;

#define DST31_SIO_IN_ONLINE_DATA_MODE()       \
  (t31_ctrl_blk->sio_mode == DST31_SIO_ONLINE_DATA_MODE)

#define DST31_SIO_IN_ONLINE_CMD_MODE()        \
  (t31_ctrl_blk->sio_mode == DST31_SIO_ONLINE_CMD_MODE)

#define DST31_SIO_MODE_CHANGE_PENDING()   \
  (t31_ctrl_blk->sio_mode == DST31_SIO_WAITING_FOR_MODE_CHANGE)

/* Macros for state validation */
#define DST31_IN_VALID_UL_STATE(__state)  \
  ((__state == DST31_FTH_STATE)   ||      \
   (__state == DST31_FTM_STATE)   ||      \
   (__state == DST31_FTS_STATE)   ||      \
   (__state == DST31_SYNC_STATE)  ||      \
   (__state == DST31_WAIT_FOR_ORDER_STATE))

#define DST31_IN_VALID_TX_STATE(__state)  \
  ((__state == DST31_FTH_STATE)   ||      \
   (__state == DST31_FTM_STATE)   ||      \
   (__state == DST31_FTS_STATE))

#define DST31_IN_VALID_DL_STATE(__state)  \
  ((__state == DST31_FRH_STATE)   ||      \
   (__state == DST31_FRM_STATE)   ||      \
   (__state == DST31_FRS_STATE)   ||      \
   (__state == DST31_SYNC_STATE)  ||      \
   (__state == DST31_WAIT_FOR_ORDER_STATE))

#define DST31_IN_VALID_RX_STATE(__state)  \
  ((__state == DST31_FRH_STATE)   ||      \
   (__state == DST31_FRM_STATE)   ||      \
   (__state == DST31_FRS_STATE))

#define DST31_IN_IDLE_STATE(__state)  (__state == DST31_IDLE_STATE) 

/* Macro to set a T31 timer */
#define DST31_TIMER_SET(_label, _timer_ptr, _timer_cnt, _timer_type) \
  if ((_timer_cnt != 0) && (_timer_ptr != NULL)) \
  { \
    if (t31_ctrl_blk->timer_type != DST31_NO_TIMER_TYPE) \
    { \
      T31_MSG_HIGH("Overriding timer, curr=%d next=%d", \
        t31_ctrl_blk->timer_type,_timer_type,0); \
    } \
    T31_MSG_DETAIL("%s timer set for %dms",_label, _timer_cnt, 0); \
    t31_ctrl_blk->timer_type = _timer_type; \
    (void) rex_set_timer(_timer_ptr, _timer_cnt); \
  } \
  else \
  { \
    T31_ERR("No timeout value for timer=%d 0x%p", _timer_type,_timer_ptr,0); \
  }

/* Macro to reset a T31 timer */
#define DST31_TIMER_RESET(_label, _timer_ptr, _timer_cnt, _timer_type) \
  t31_ctrl_blk->timer_type = DST31_NO_TIMER_TYPE; \
  DST31_TIMER_SET(_label, _timer_ptr, _timer_cnt, _timer_type)

/* Macro to clear a T31 timer */
#define DST31_TIMER_CLR(_timer_ptr) \
  if (_timer_ptr) \
  { \
    (void) rex_clr_timer(_timer_ptr); \
    (void) rex_clr_sigs(rex_self(), DS_GCSD_T31_TIMEOUT_SIG); \
    t31_ctrl_blk->timer_type = DST31_NO_TIMER_TYPE; \
    t31_ctrl_blk->flags.timeout = FALSE; \
  } \
  else \
  { \
    T31_ERR("Attempt to clear NULL T31 timer",0,0,0); \
  }

/*=============================================================================
** ENUMERATIONS
**===========================================================================*/

/* T31 State Definitions */
typedef enum
{
  DST31_IDLE_STATE,
  DST31_SYNC_STATE,
  DST31_WAIT_FOR_ORDER_STATE,
  DST31_FTH_STATE,
  DST31_FRH_STATE,
  DST31_FTM_STATE,
  DST31_FRM_STATE,
  DST31_FTS_STATE,
  DST31_FRS_STATE,
  DST31_MAX_STATES
} dst31_state_enum_type;

/* Frame processing status indicators */
typedef enum
{
  DST31_FRAME_EMPTY =      0x1,   /* No frame available */
  DST31_FRAME_READY =      0x2,   /* Frame available but is not last frame */
  DST31_FRAME_COMPLETE =   0x4,   /* Frame available and is last frame */
  DST31_FRAME_INCOMPLETE = 0x8,   /* Partial frame */
  DST31_FRAME_CORRUPT =    0x10   /* Corrupted frame, discard the data */
} dst31_frame_status_enum_type;

/* SIO Data Modes */
typedef enum
{
  DST31_SIO_CMD_MODE,
  DST31_SIO_ONLINE_CMD_MODE,
  DST31_SIO_ONLINE_DATA_MODE,
  DST31_SIO_WAITING_FOR_MODE_CHANGE,
  DST31_MAX_SIO_MODES
} dst31_sio_mode_enum_type;

/* OTA link processing directions */
typedef enum
{
  DST31_OTA_UPLINK,
  DST31_OTA_DOWNLINK
} dst31_ota_path_enum_type;

/* +FRH Data processing states */
typedef enum
{
  DST31_FRH_HDLC_FIND_FLAG_STATE,
  DST31_FRH_HDLC_FLAG_START_STATE,
  DST31_FRH_HDLC_DATA_STATE,
  DST31_FRH_HDLC_FLAG_STOP_STATE
} dst31_frh_data_states_enum_type;

/* DLE filtering processing states */
typedef enum
{
  DST31_FIND_DLE_STATE,
  DST31_DLE_START_STATE,
  DST31_DLE_STOP_STATE
} dst31_dle_states_enum_type;

/* DSAT Result Code Types */
typedef enum
{
  DST31_DSAT_RESULT_INTERMEDIATE,
  DST31_DSAT_RESULT_FINAL
} dst31_dsat_result_enum_type;

/* Data device identifiers */
typedef enum
{
  DST31_DATA_ON_NULL_DEV,
  DST31_DATA_ON_UART_DEV,
  DST31_DATA_ON_USB_DEV
} dst31_data_dev_id_enum_type;

/* Timer Types */
typedef enum
{
  DST31_NO_TIMER_TYPE,
  DST31_FIT_TIMER_TYPE,
  DST31_FCL_TIMER_TYPE,
  DST31_DCN_TIMER_TYPE,
  DST31_WFO_TIMER_TYPE,
  DST31_FTS_TIMER_TYPE,
  DST31_FRS_TIMER_TYPE,
  DST31_CD_TIMER_TYPE
} dst31_timer_type_enum_type;

/*=============================================================================
** STRUCTURES
**===========================================================================*/

/* T31 Control Block - primary data structure used by T31 to maintain
** processing context.
*/
typedef struct dst31_cb_s
{
  /* Bit-field used to maintain T31 boolean flags to conserve memory */
  struct flag_s {
    /* Set to TRUE when the interface layer has been initialized. */
    unsigned int              t31_init                    : 1;

    /* Set to TRUE when a state transition occurs (start of procedure). */
    unsigned int              sop                         : 1;

    /* Set to TRUE when TPS has asserted flow control. */
    unsigned int              tps_flowctl_asserted        : 1;

    /* Set to TRUE when SIO has asserted flow control. */
    unsigned int              sio_flowctl_asserted        : 1;

    /* Set to TRUE when T31 has asserted SIO flow control directly. */
    unsigned int              sio_direct_flowctl_asserted : 1;

    /* Set to TRUE when a T31 protocol timer has expired. */
    unsigned int              timeout                     : 1;

    /* Set to TRUE when T.30 trainining phase is active. */
    unsigned int              training_phase_active       : 1;

    /* Set to TRUE when a request to change SIO modes has been sent
    ** to DSMGR
    */
    unsigned int              sio_mode_change_pending     : 1;

    /* Set to TRUE when ATCOP has detected a command abort indication. */
    unsigned int              at_cmd_abort                : 1;

    /* Set to TRUE when it is ok for a SOP function to send a DSAT_CONNECT
    ** response to the DTE when entering an FTH/FRH state via an AT command
    ** request.  The initial entry to these states is implied at the start
    ** of the call and the DSAT_CONNECT is sent from ATCOP not T31.
    */
    unsigned int              first_dsat_connect          : 1;

    /* Set to TRUE when it is ok for a SOP function to send a DSAT_CONNECT
    ** response to the DTE.
    */
    unsigned int              send_connect                : 1;

    /* Set to TRUE whenever an SIO TX flush is in progress.  During this
    ** period T31 can not send any data to SIO.
    */
    unsigned int              wait_sio_flush_tx           : 1;

    /* Set to TRUE if the FA needs to be set to the IDLE state */
    unsigned int              set_fa_to_idle              : 1;

    /* Set to FALSE if an +FIT timer expires and the action is set to 1.
    ** This condition prohibits the DCE from setting FCLASS=0 under certain 
    ** fax related errors.
    */
    unsigned int              allow_fclass_0              : 1;

    /* Set to TRUE when T31 enters the +FTH or +FTM state and is awaiting
    ** the first data byte to be received from the DTE.  Special processing
    ** is done in each of these states while T31 waits for the DTE to respond.
    */
    unsigned int              wait_for_first_data_byte    : 1;

    /* This flag is set to TRUE when T31 receives the last frame of an
    ** HDLC sequence during +FRH processing.  It is used to allow any
    ** trailing HDLC flags to be pulled up and removed from the data
    ** stream before exiting the FRH state.
    */
    unsigned int              frh_wait_for_fa_idle        : 1;

    /* This flag is set to TRUE when T31 sends the +FTH HDLC preamble
    ** to the remote FA.
    */
    unsigned int              fth_preamble_active         : 1;

    /* This flag is set to TRUE when T31 has a complete HDLC frame
    ** ready to be transmitted to the remote FA.
    */
    unsigned int              fth_frame_complete          : 1;

    /* This flag is set to TRUE when T31 has found the last HDLC
    ** frame in a sequence of frames sent by the local TE.
    */
    unsigned int              fth_last_frame              : 1;

    /* Set to TRUE if carrier is not detected within the specified time */
    unsigned int              cd_timeout                  : 1;
  } flags;

  /* This is an embedded structure which is used to store HDLC and MSG
  ** data as frames are being built for RX/TX transmission
  */
  struct dst31_io_data_s {
    /* Indicates the current total number of bytes in io_data_buf */ 
    uint16                    cnt;

    /* Buffer to store HDLC and MSG data while building RX/TX frames */
    uint8                     buf[DST31_MAX_IO_DATA_SZ];

    /* Buffer to store HDLC zero-bit-stuffed data during stuffing/unstuffing */
    uint8                     zbuf[DST31_MAX_IO_DATA_SZ];

    /* Used to calculate Frame Check Sum for received frames */
    uint16                    fcs;

    /* Holds the value of the last character transmitted after each iteration
    ** of the +FTM state.
    */
    uint8                     last_ftm_tx_char;

    /* DLE filtering state when processing +FTH and +FTM data */
    dst31_dle_states_enum_type dle_data_state;
  } io_data;

  /* Struct to store the TX/RX byte counts per fax data call */
  struct dst31_byte_cnts_s {
    uint16                    total_tx_hdlc_bytes;
    uint16                    total_rx_hdlc_bytes;
    uint32                    total_tx_msg_bytes;
    uint32                    total_rx_msg_bytes;
    uint32                    dle_bytes;
    uint32                    fa_page_bytes;
    uint32                    sio_page_bytes;
    uint32                    other_bytes;
    uint16                    page_cnt;
    uint16                    fth_byte_cnt;
    uint16                    fth_preamble_cnt;
  } byte_cnts;

  /* Set to TRUE when 'carrier' is detected when a state
  ** change to +FRH occurs.
  ** NOTE: this field is passed by address to a function thus it can
  ** not be part of the bit field flag structure above.  I didn't want
  ** to pass the 'flags' field as a whole so I just added a separate
  ** boolean.
  */
  boolean                     carrier_detected;

  /* Indicates which device data is being received on from the local TE */
  dst31_data_dev_id_enum_type data_dev_id;

  /* DSM item pointer used to hold any pending data after processing a 
  ** +FRH or +FRM state
  */
  dsm_item_type *             pending_dl_data_ptr;

  /* DSM item pointer used to hold any pending data after processing a 
  ** +FTH or +FTM state
  */
  dsm_item_type *             pending_ul_data_ptr;

  /* DSM item pointer used to current uplink data */
  dsm_item_type *             current_ul_data_ptr;

  /* Timeout value for +FCL (carrier loss) */
  dsat_num_item_type          fcl_timeout_val;

  /* Timeout value for +FIT (DTE timeout) */
  dsat_num_item_type          fit_timeout_val;

  /* Last T31 state processed. */
  dst31_state_enum_type       last_state;
  
  /* Current T31 state being processed. */
  dst31_state_enum_type       curr_state;

  /* SIO mode (COMMAND | ONLINE-COMMAND | ONLINE-DATA MODE) */
  dst31_sio_mode_enum_type    sio_mode;

  /* OTA path (Indicates uplink or downlink processing direction).*/
  dst31_ota_path_enum_type    ota_path;

  /* Current AT command result code to send to the local DTE via ATCOP */
  dsat_result_enum_type       dsat_result;

  /* Callback function used by T31 to notify ATCOP when an action has
  ** completed.
  */
  void (*dsat_callback)(dsat_result_enum_type result_code);

  /* FPS parameters from ATCOP. */
  dsfps_params_type           at_params;
  
  /* Call type - Mobile originated or Mobile Terminated */
  uint8                       call_type;

  /* Call ID - from Call Manager */
  cm_call_id_type             call_id;
  
  /* Pointer to REX timer used to process T31 protocol timers. */
  rex_timer_type *            timer_ptr;
  rex_timer_type              timer;
  rex_sigs_type               timer_sig;
  dst31_timer_type_enum_type  timer_type;

  /* Short cut watermark pointers */
  dsm_watermark_type *        sio_ul_wm_ptr;
  dsm_watermark_type *        sio_dl_wm_ptr;
  dsm_watermark_type *        tps_ul_wm_ptr;
  dsm_watermark_type *        tps_dl_wm_ptr;

  /* Name assigned to the T31 instance */
  char *                      i_name;
} dst31_ctrl_blk_struct_type;

/* State handler table definition - contains pointers to the SOP and 
** main state processing functions.
*/
typedef struct
{
  /* Pointer to the state start of procedure initialization routine */
  void (*state_sop)(void);

  /* Pointer to the main state handler processing routine */
  void (*state_handler)(void);
} t31_state_handler_tbl_struct_type;

/*=============================================================================
** EXTERNAL FUNCTIONS USED SOLELY BY T31
=============================================================================*/

/* State start of procedure initialization functions */
extern void dst31i_sop_idle_state(void);
extern void dst31i_sop_wait_for_order_state(void);
extern void dst31i_sop_fth_state(void);
extern void dst31i_sop_frh_state(void);
extern void dst31i_sop_ftm_state(void);
extern void dst31i_sop_frm_state(void);
extern void dst31i_sop_fts_state(void);
extern void dst31i_sop_frs_state(void);

/* State handler function */
extern void dst31i_process_idle_state(void);
extern void dst31i_process_sync_state(void);
extern void dst31i_process_wait_for_order_state(void);
extern void dst31i_process_fth_state(void);
extern void dst31i_process_frh_state(void);
extern void dst31i_process_ftm_state(void);
extern void dst31i_process_frm_state(void);
extern void dst31i_process_fts_state(void);
extern void dst31i_process_frs_state(void);

/* Utility and helper functions primarily used by the state handler routines */
extern void dst31i_check_fa_status
(
  ds_fa_status_enum_type
);

extern uint16 dst31i_dump_dsm_item
(
  char *,
  dsm_item_type *
);

extern dst31_frame_status_enum_type dst31i_build_ul_hdlc_frame
(
  dsm_item_type **,
  uint16
);

extern dst31_frame_status_enum_type dst31i_build_dl_hdlc_frame
(
  dsm_item_type **,
  uint16,
  dsm_item_type **,
  boolean *,
  boolean *
);

extern dst31_frame_status_enum_type dst31i_build_ul_msg_frame
(
  dsm_item_type **,
  uint16,
  dsm_item_type **
);

extern dst31_frame_status_enum_type dst31i_build_dl_msg_frame
(
  dsm_item_type **,
  uint16,
  dsm_item_type **
);

extern void dst31i_change_state
(
  dst31_state_enum_type,
  boolean
);

extern void dst31i_terminate_state
(
  dsat_result_enum_type,
  boolean
);

extern void dst31i_terminate_session
(
  dst31_state_enum_type,
  dsat_result_enum_type
);

extern void dst31i_ds_send_cmd
(
  ds_cmd_enum_type
);

extern void dst31i_request_sio_mode_change
(
  dst31_sio_mode_enum_type
);

extern void dst31i_send_dsat_result
(
  dsat_result_enum_type,
  dst31_dsat_result_enum_type
);

extern boolean dst31i_dsm_pushdown
(
  dsm_item_type **,
  void *,
  uint16,
  dsm_mempool_id_enum_type
);

extern boolean dst31i_dsm_pushdown_tail
(
  dsm_item_type **,
  void *,
  uint16,
  dsm_mempool_id_enum_type
);

extern dst31_state_enum_type dst31i_curr_state
(
  void
);

/*===========================================================================
** FUNCTION: DST31_GET_T31_CTRL_BLK
**
** DESCRIPTION:
**   This function is used to return the address of t31_ctrl_blk variable.
**
** PARAMETERS:
**   None
**
** RETURNS:
**   dst31_ctrl_blk_struct_type* - address of variable t31_ctrl_blk
**
** SIDE EFFECTS:
**   None.
**=========================================================================*/
dst31_ctrl_blk_struct_type* dst31_get_t31_ctrl_blk(void);

/*=============================================================================
** FUNCTION: DST31I_GET_DST31_STATE_NAME
**
** DESCRIPTION:
**   This function is used to retrieve the dst31_state_name of particular index.
**
** PARAMETERS:
**   index - index to the array dst31_state_names
**
** RETURNS:
**   char* - the state name
**
** SIDE EFFECTS:
**   None
**===========================================================================*/
char* dst31i_get_dst31_state_name
(
  uint8 index
);

#endif /* FEATURE_DATA_GCSD_FAX */
#endif /* DST31I_H */
