#ifndef SRCHGSMDRV_H
#define SRCHGSMDRV_H

/*============================================================================
                            S R C H C M D R V . H
DESCRIPTION

Copyright (c) 2000 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/srchgsmdrv.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
02/24/15    ap      Initial check-in for WL1 QSH framework 
05/18/16    mk      GSM Init needs only 2 W frames
02/22/16    ks      Skip W2G gap if there is not enough time to prebuild RF scripts and send X2G startup
09/20/15    wz/ar   Add prototypes to query if W2G/W2L CM/FMO searches are ongoing
08/23/14    gv      Featurization to support W compiled out
06/10/14    ar      Change prototype for init pending cb since all func's registered takes param
05/29/14    mm      Update IRAT/CM driver initialization and RxLM buffer allocation call flow
05/21/14    db      Remove race condition between WL1 reading MDSP status and GL1 updating it during FACH cleanup
04/02/14    mm      3C IRAT/CM support
02/21/14    jkb     Reverting changes for the Unified GERAN APIs
01/29/14    mm      Cleanup function callback prototype changed
12/18/13    jkb     Changes for the Unified GERAN APIs
12/11/13    mm      3C CM/IRAT support
11/20/13    gsk     DSDS:Account for time taken for GSM initializations when triggering GSM search in W.
10/24/13    cc      Remove RxD_is_active API from driver layer
09/30/13    cc      DBDC IRAT support check in
09/29/13    as      Continuing port of Dual Sim from Triton to Dime.
08/01/13    cc      Add new cb system to avoid FACH cleanup race condition
01/11/13    mm      Compiler error fixes for Triton DSDA
12/17/12    mm      Interface changes for AS ID passed to GL1 functions for DSDS
11/16/12    cc      IRAT feature for Dime
10/15/12    dp      Cleaned up code relating to unit test
08/28/12    cc      Add a function to handle the critical sec in sending a cmd to FW  
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
07/10/12    dp      Made it so we don't hold mutex lock across gl1_hw_cm_gsm_init()
05/23/12    dp      Parallelized the gl1 init process when wl1 boots up
04/12/12    yh      Add support for resetting edge_core
02/03/12    dp      Get RSSI Threshold from common external #define
01/26/12    zr      Mainlined FEATURE_WCDMA_GENESIS_SW, removed FEATURE_WCDMA_TAXIS_SW
09/12/11    yh      Increase number of pwr meas to 10 per GSM frame
07/08/11    mk      Fixed compilation error.
04/28/11    mr      Added support for unit test framework under FEATURE_CM_UNIT_TEST
1/17/11     mm      Featurized srchgsm_drv_check_validity()
1/7/11      mm      Prototype for API srchgsm_drv_check_validity
12/21/10    dm      Moved mutex initializations to wl1m while doing 
                    task initialization.
12/08/10    yh      Add cleanup done status and API to check if GSM mDSP is awake
08/30/10    yh      Externed srchgsm_gfn_update_in_odd_frame
08/23/10    ks      Removed inclusion of mdsp header for Genesis (featurized)
06/03/10    yh      Deleted dependency on mcalwcdma_cm.h
05/07/10    yh      Add support for IRAT RF buffer control
07/31/09    yh      Remove references to FEATURE_WCDMA_OFFLINE_MODEM_IRAT_INITIAL_DEV
06/17/09    uk      Prevent aset/nset meas during a FMO GSM gap
06/15/09    uk      Support for sending START_FACH_MEAS cmd to fw before 
                    starting a FMO gap for GSM meas
05/01/09    yh      Put GSM pending commands cleanup inside GSTMR context
04/17/09    yh/uk   Add W2G mutex into W2G drivers code
03/12/09    uk      Externed srchgsm_clks_set_to_gsm flag
01/27/09    stk/yh  Added IRAT bring-up code under FEATURE_WCDMA_OFFLINE_MODEM_IRAT_INITIAL_DEV
01/22/08    yh      Added debug code to catch cmds being scheduled too soon in G-timeline.
04/05/06    sk/bbd  Added frequency offset to SCH dec struct and ACQ start struct.
02/07/06    bbd     Moved GSM clock variables to l1utils and added support for
                    GSM clock shutdown voting mechanism.
04/28/05    bbd     Fixes for passing GL1 HW tags parameters during sending of GSM commands
                    and retrieval of results.
04/20/05    bbd     Changed FEATURE_TWO_PARAM_ASYNC_RX_METRICS to
                    FEATURE_USE_LATEST_GL1_HW_API.
                    Added support for GL1 HW abstraction to pass parameters needed
                    for RF driver. These parameters are encapsulated in the "tags" field.
08/23/08    sh      Increased SRCH_GSM_MAX_FR_NUM_DIFF to 18.
03/11/04    gs      Externalized variable srchcm_gsm_clock_enable_state
                    Externalized function srchcm_gsm_query_gsm_clock_state
12/17/03    yiz     Moved srchgsmdrv.h out to srchset.h
12/16/03    gs      Support for GSM Quadband
12/10/03    yiz     Add FEATURE_INTER_RAT_CELL_FACH_RE_SELECTION
11/04/03    gs      Externalized function srchgsm_drv_init_on_resume
08/15/03    gs      Added element do_cleanup in strcuture
                    srchcm_gsm_stop_acq_cmd_param_struct_type.
                    Updated declaration of function srchgsm_read_pwr_meas_result
                    as per changes in source file.
                    Externalized function srchgsm_send_acq_cleanup_req
07/11/03    gs      Created macro SRCHCM_GSM_FRAME_NUM_INVALID
                    Externalized function srchcm_gsm_update_gsm_clock_state
01/30/03    gs      Defined function type SRCHGSM_CMD_RESULT_POLL_CB_FUNC_TYPE
                    Defined function type SRCH_GSM_CMD_SENT_POST_CB_FUNC_TYPE
                    Added member cmd_sent_post_cb_func to structure
                    srchgsm_mdsp_cmd_struct_type
                    Updated function prototype as per changes in srchgsmdrv.c
                    and externalized new functions.
10/21/02    gs      Changed the uncertainty window type to uint16 (same as
                    GSM gl1 and mdsp driver)
                    Externalized the function to set SCH decode search width
08/09/02    gs      Created this file
===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "comdef.h"
#include "gl1_hw.h"
#include "l1rrcif.h"

#include "wl1rfmif.h"
#include "wl1api.h"
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* GSM IRQ config field related macro */
#define GSTMR_MICRO_IRQ_MSB_IGNORE_EVEN_ODD_BIT 0x0002

/* GSM 51 multiframe number */
#define SRCH_GSM_MULTI_FRAME_COUNT 51
/* GSM Quater symbols per GSM frame */
#define SRCH_GSM_NUM_QS_PER_FRAME 5000

#define SRCH_GSM_MAX_MDSP_CMD_BUF_SIZE 20

/* The number of GSM frame by which cmd frame number can lead the
   GSM frame number. Other wise it is treated as missed cmd */
#define SRCH_GSM_MAX_FR_NUM_DIFF       18

/* GSM mDSP command related macros */
#define SRCHCM_MAX_PWR_MEAS_BURST_PER_GSM_FR 10

/* invalid GSM frame number for WCDMA gap */
#define SRCHCM_GSM_FRAME_NUM_INVALID 0xFF

/* Number of GSM RSSI Samples per GSM carrier */
#define SRCHFACH_NUM_GSM_RSSI_PER_CARRIER  3

#define SRCHGSM_MIN_ACCEPTABLE_RX_LVL_dBm_x16 GL1_HW_MIN_ACCEPTABLE_RX_LVL_dBm_x16

/* RSSI in SCH: Allow only 6 cells to be measured in SCH gap */
#define SRCHCM_MAX_NUM_RSSI_BURSTS_IN_SCH_GAP 6

/* RSSI in SCH: Num of remaining rssi bursts can be used */
#define SRCHCM_NUM_REM_RSSI_BURSTS_IN_SCH_GAP (SRCHCM_MAX_PWR_MEAS_BURST_PER_GSM_FR) - \
                                              (SRCHCM_MAX_NUM_RSSI_BURSTS_IN_SCH_GAP)

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */


/* CB function for if GL1 initialization is pending */
typedef void SRCHGSM_GL1_INIT_PENDING_CB_FUNC_TYPE(uint8);
/* GSM command result poll call back function type */
typedef void SRCHGSM_CMD_RESULT_POLL_CB_FUNC_TYPE(uint8);
/* call back fucntion for command cleanup */
/* GSM command cleanup pre call back function type */
typedef void SRCHGSM_CMD_CLEANUP_PRE_CB_FUNC_TYPE(void);
/* GSM command result poll call back function type */
typedef void SRCHGSM_CMD_CLEANUP_POLL_CB_FUNC_TYPE(uint8);

/* GSM related typedefinition */
/* -------------------------- */
typedef enum
{
  SRCHCM_GSM_MEAS_STATE_IDLE,
  SRCHCM_GSM_MEAS_STATE_READY,
  SRCHCM_GSM_MEAS_STATE_MEAS,
  SRCHCM_GSM_MEAS_STATE_DONE,
  SRCHCM_GSM_MEAS_STATE_RES_PROC,
  SRCHCM_GSM_MEAS_STATE_READY_PENDING,
  SRCHCM_GSM_MEAS_STATE_MEAS_PENDING,
  SRCHCM_GSM_MEAS_STATE_DONE_PENDING,
  SRCHCM_GSM_MEAS_STATE_RES_PROC_PENDING
} srchcm_gsm_meas_state_enum_type;

/* GSM mDSP command enum type definiiton */
typedef enum
{
  SRCHGSM_CM_STARTUP_MDSP_CMD,
  SRCHGSM_CM_CLEANUP_MDSP_CMD,
  SRCHGSM_PWR_MEAS_MDSP_CMD,
  SRCHGSM_ACQ_START_MDSP_CMD,
  SRCHGSM_ACQ_STOP_MDSP_CMD,
  SRCHGSM_ASYNC_SCH_RECE_MDSP_CMD,
  SRCHGSM_NUM_MDSP_CMD
} srchgsm_cmd_type_enum_type;

/* GAP parameters in GSM time unit structure definition */
typedef struct
{
  /* CM gap start frame number */
  uint8 frame_num;
  /* CM gap start QS offset */
  uint16 offset_qs;
  /* CM gap length in GSP QS */
  uint16 gap_len_qs;
} schcm_cm_gap_param_gsm_struct_type;

/* Common minimum GSM command type structure definition */
typedef struct
{
  /* GSM copmmand frame number. This is always the first element in
     every GSM command structure */
  uint8 frame_num;
} srchcm_gsm_cmd_param_struct_type;

/* GSM CM startup command structure definition */
typedef struct
{
  /* Command send frame number */
  uint8 frame_num;
  /* CM startup QS offset */
  uint16 offset_qs_to_start;
  /* RF buffer info */
  void *rf_buf;
  /* RxD enabled/disabled */
  boolean rxd_en;
  /* Num RF header source items */
  uint8 num_source_item;
  #ifdef FEATURE_WCDMA
  /* RF source params */
  wl1_rf_source_params_struct_type source_params[WL1_RF_HEADER_DB_NUM_SOURCE];
  #endif
  #ifdef FEATURE_DUAL_SIM
  /* as_id received from RRC. Passed to GSM L1 for DSDS */
  sys_modem_as_id_e_type as_id;
  #endif
} srchcm_gsm_startup_cmd_param_struct_type;

/* GSM CM cleanup command structure definition */
typedef struct
{
  /* Command send frame number */
  uint8 frame_num;
  /* CM cleanup QS offset */
  uint16 offset_qs_to_end;
  #ifdef FEATURE_DUAL_SIM
  /* as_id received from RRC. Passed to GSM L1 for DSDS */
  sys_modem_as_id_e_type as_id;
  #endif
} srchcm_gsm_cleanup_cmd_param_struct_type;

/* GSM RSSI power meas command structure definition */
typedef struct
{
  /* Command send frame number */
  uint8 frame_num;
  /* number RSSI scan burst */
  uint8 num_bursts;
  /* ARFCN of the carrier for which RSSI scan to be done */
  W_ARFCN_T carrier_arfcn[SRCHCM_MAX_PWR_MEAS_BURST_PER_GSM_FR];
  /* starting offset of start of the burst */
  uint16 offset_to_start[SRCHCM_MAX_PWR_MEAS_BURST_PER_GSM_FR];
  /* expected RSSI strength fo rLNA range selection */
  int16 signal_strength_dbx16[SRCHCM_MAX_PWR_MEAS_BURST_PER_GSM_FR];
  /* Burst position in the GAP */
  gl1_hw_burst_position_type burst_position[SRCHCM_MAX_PWR_MEAS_BURST_PER_GSM_FR];
  /* GL1 HW abstraction to allow RF parameters to be passed back to GL1 HW driver
     This is transparent to WL1. */
  gl1_hw_rx_tags_type *tags;
  #ifdef FEATURE_DUAL_SIM
  /* as_id received from RRC. Passed to GSM L1 for DSDS */
  sys_modem_as_id_e_type as_id;
  #endif
} srchcm_gsm_pwr_scan_cmd_param_struct_type;

/* GSM FCCH ACQ start command structure definition */
typedef struct
{
  /* Command send frame number */
  uint8 frame_num;
  /* GSM cell ARFCN */
  W_ARFCN_T arfcn;
  /* RSSI of the GSM cell */
  int16 signal_strength;
  /* ACQ start QS offset */
  uint16 start_offset;
  /* SCH to be decode if the tone is detected */
  /* Frequency offset for this GSM cell */
  int16 frequency_offset;
  boolean sch_decode;
  /* GL1 HW abstraction to allow RF parameters to be passed back to GL1 HW driver
     This is transparent to WL1. */
  gl1_hw_rx_tags_type tags;
  #ifdef FEATURE_DUAL_SIM
  /* as_id received from RRC. Passed to GSM L1 for DSDS */
  sys_modem_as_id_e_type as_id;
  #endif
} srchcm_gsm_start_acq_cmd_param_struct_type;

/* GSM FCCH ACQ stop command structure definition */
typedef struct
{
  /* Command send frame number */
  uint8 frame_num;
  /* GSM cell ARFCN */
  W_ARFCN_T arfcn;
  /* ACQ stop QS offset */
  uint16 stop_offset;
  /* indicate to perform cleanup for RF unwind along with stop ACQ */
  boolean do_cleanup;
  #ifdef FEATURE_DUAL_SIM
  /* as_id received from RRC. Passed to GSM L1 for DSDS */
  sys_modem_as_id_e_type as_id;
  #endif
} srchcm_gsm_stop_acq_cmd_param_struct_type;

/* GSM SCH decode command structure definition */
typedef struct
{
  /* Command send frame number */
  uint8 frame_num;
  /* GSM cell ARFCN */
  W_ARFCN_T arfcn;
  /* RSSI of the GSM cell */
  int16 signal_strength;
  /* SCH decode uncertaintity window */
  uint16 uncertainty_win_size;
  /* Offset of SCH burst center from frame boundary */
  uint16 offset_to_center_of_burst;
  /* Frequency offset for this GSM cell */
  int16 frequency_offset;
  /* GL1 HW abstraction to allow RF parameters to be passed back to GL1 HW driver
     This is transparent to WL1. */
  gl1_hw_rx_tags_type *tags;
  #ifdef FEATURE_DUAL_SIM
  /* as_id received from RRC. Passed to GSM L1 for DSDS */
  sys_modem_as_id_e_type as_id;
  #endif
} srchcm_gsm_sch_decode_cmd_param_struct_type;

typedef enum
{
  SRCHGSM_GL1_ON_PENDING,
  SRCHGSM_GL1_ON,
  SRCHGSM_GL1_OFF
} srchgsm_gl1_init_state_enum_type;

typedef void SRCH_GSM_CMD_SENT_POST_CB_FUNC_TYPE(void);

/* GSM command structure definition */
typedef struct
{
  /* command type */
  srchgsm_cmd_type_enum_type cmd_type;
  /* frame number at which the command to be sent */
  uint8 frame_num;
  /* command pointer */
  void* cmd_param_ptr;
  /* cmd sent post call back function */
  SRCH_GSM_CMD_SENT_POST_CB_FUNC_TYPE* cmd_sent_post_cb_func;
} srchgsm_mdsp_cmd_struct_type;

/* GSM Command List*/
typedef struct
{
  /* GSM Command type */
  srchgsm_cmd_type_enum_type cmd_type;
  /* GSM command pointer */
  void* gsm_mdsp_cmd_ptr;
  /* call back function after command has been sent */
  SRCH_GSM_CMD_SENT_POST_CB_FUNC_TYPE* cmd_sent_post_cb_func;
} srchcm_gsm_send_cmd_seq_type;

/* GSM frame number that counts 51 multiframe modulo count */
extern uint8 srchgsm_gsm_frame_num;
/* indicate the last GSM frame number update in EVEN/ODD frame */
extern boolean srchgsm_gfn_update_in_odd_frame;
extern rex_crit_sect_type w2g_crit_sect;
#define W2G_INTLOCK()  REX_ISR_LOCK(&w2g_crit_sect)
#define W2G_INTFREE()  REX_ISR_UNLOCK(&w2g_crit_sect)

extern rex_crit_sect_type w2g_gl1_init_crit_sect;
#define W2G_GSM_STATE_INTLOCK() rex_enter_crit_sect(&w2g_gl1_init_crit_sect)
#define W2G_GSM_STATE_INTFREE() rex_leave_crit_sect(&w2g_gl1_init_crit_sect)

/* GSM init is done every time before triggering GSM search for DSDS.
 * So account for time taken for GSM initializations when triggering GSM search.
 */

#define SRCHGSM_GSM_INIT_DELAY(val) \
  do {  \
    if((WL1_IN_DS_MODE()) && \
       (srchgsm_gl1_init_state != SRCHGSM_GL1_ON) \
      )	   \
    {  \
      val = 2;\
    } \
    else \
    {  \
      val = 0; \
    } \
  } while(0)
  

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===========================================================================
FUNCTION srchgsm_drv_init
         srchgsm_drv_init_on_resume

DESCRIPTION
  This function initializes the variables related to this module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchgsm_drv_init_on_resume(void);
extern void srchgsm_drv_init(void);

#ifndef FEATURE_CM_UNIT_TEST
/*===========================================================================
FUNCTION srchgsm_drv_check_validity 

DESCRIPTION
  This function checks the validity of the RF constants being passed for
  W2G timeline according to specs.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchgsm_drv_check_validity(void);
#endif
/*===========================================================================
FUNCTION srhcm_gsm_frame_tick_handler

DESCRIPTION
  This function implements the GSM frame tick hanldler. This function must
  be register to GSM timer module to be called back every GSM frame. It do
  the following actions:
  - Get the GSM time QS and frame number
  - Sequence the command that can be sent in this frame
  - Call the result poll call back function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srhcm_gsm_frame_tick_handler(
   #ifdef FEATURE_DUAL_SIM
   sys_modem_as_id_e_type as_id
   #else
   void
   #endif
   );

/*===========================================================================
FUNCTION srchcm_gsm_get_gfn

DESCRIPTION
  This function is a utility function to return current GFN.

DEPENDENCIES
  None

RETURN VALUE
  Current GSM frame number.

SIDE EFFECTS
  None
===========================================================================*/

extern uint8 srchcm_gsm_get_gfn(void);

/*===========================================================================
FUNCTION srchcm_gsm_update_gap_params_to_gsm_tl

DESCRIPTION
  This function convert the WCDMA compressed gap parameters to GSM timeline.
  The GSM gap is the GAP in which measurement can be done. It excludes the UL
  transmission time and DL pilot time in the start and end of gap respectively.
  
  See description in function comments.

DEPENDENCIES
  None

RETURN VALUE
  Compressed mode GAP in GSM time info structure pointer.

SIDE EFFECTS
  None
===========================================================================*/

extern schcm_cm_gap_param_gsm_struct_type* srchcm_gsm_update_gap_params_to_gsm_tl(
  /* WCDMA frame offset from the GAP. If 0 indicate that GAP is in the next frame */
  uint8 wcdma_frame_offset,
  /* GAP start inf slot number in WCDMA slot unit */
  uint8 gap_start_slot_num,
  /* GAP length in WCDMA slot units */
  uint8 gap_length);

/*===========================================================================
FUNCTION srchgsm_send_cmd

DESCRIPTION
  This function sends the GSM command. It checks for the GSM frame number of
  the command to be sent. If this is the frame in which the command can be sent
  then immediately send it other wise it queues the command in comand Q.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchgsm_send_cmd(
  /* GSM Command type */
  srchgsm_cmd_type_enum_type cmd_type,
  /* GSM command pointer */
  void* gsm_mdsp_cmd_ptr,
  /* call back function after command has been sent */
  SRCH_GSM_CMD_SENT_POST_CB_FUNC_TYPE* cmd_sent_post_cb_func);

/*===========================================================================
FUNCTION srchgsm_send_cmd

DESCRIPTION
  This function sends the GSM command immediately. It calls appropriate
  forwarding function in this module to send the command to GSM mDSP drivers.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchgsm_send_cmd_now(
  /* GSM Command type */
  srchgsm_cmd_type_enum_type cmd_type,
  /* GSM command pointer */
  void* gsm_mdsp_cmd_ptr,
  /* call back function after command has been sent */
  SRCH_GSM_CMD_SENT_POST_CB_FUNC_TYPE* cmd_sent_post_cb_func);

/*===========================================================================
FUNCTION srchgsm_send_gsm_cm_startup_cmd

DESCRIPTION
  This function sends GSM CM startup command.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchgsm_send_gsm_cm_startup_cmd(
  /* startup command parameters */
  srchcm_gsm_startup_cmd_param_struct_type *cm_startup_cmd_ptr);

/*===========================================================================
FUNCTION srchgsm_send_gsm_cm_cleanup_cmd

DESCRIPTION
  This function sends GSM CM cleanup command.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchgsm_send_gsm_cm_cleanup_cmd(
  /* Cleanup command parameters */
  srchcm_gsm_cleanup_cmd_param_struct_type *cm_cleanup_cmd_ptr);

/*===========================================================================
FUNCTION srchgsm_send_pwr_meas_cmd

DESCRIPTION
  This function sends GSM RSSI measure command.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchgsm_send_pwr_meas_cmd(
  /* RSSI meas command parameters */
  srchcm_gsm_pwr_scan_cmd_param_struct_type *pwr_meas_cmd_ptr);

/*===========================================================================
FUNCTION srchgsm_set_pwr_meas_result_poll_cb_func

DESCRIPTION
  This function set the RSSI scan result poll call back function
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchgsm_set_pwr_meas_result_poll_cb_func(
  /* RSSI scan result poll call back function */
  SRCHGSM_CMD_RESULT_POLL_CB_FUNC_TYPE *cb_func);

/*===========================================================================
FUNCTION srchgsm_read_pwr_meas_result

DESCRIPTION
  This function reads the RSSI meas result from GSM driver
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchgsm_read_pwr_meas_result(
  /* number of RSSI scan burst */
  uint8 num_bursts,
  /* RSSI measured values */
  int16 *rssi_meas_buffer,
  /* lna state values */
  uint8 *lna_state_buffer,
  /* GSM HW frame layer tags */
  gl1_hw_rx_tags_type *tags
  #ifdef FEATURE_DUAL_SIM
  /* as_id received from RRC. Passed to GSM L1 for DSDS */
  , sys_modem_as_id_e_type as_id
  #endif
);

/*===========================================================================
FUNCTION srchgsm_send_acq_start_cmd

DESCRIPTION
  This function sends ACQ start command.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchgsm_send_acq_start_cmd(
  /* FCCH ACQ start command parameters */
  srchcm_gsm_start_acq_cmd_param_struct_type *acq_start_cmd_ptr);

/*===========================================================================
FUNCTION srchgsm_send_acq_stop_cmd

DESCRIPTION
  This function sends ACQ stop command.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchgsm_send_acq_stop_cmd(
  /* FCCH ACQ stop command parameters */
  srchcm_gsm_stop_acq_cmd_param_struct_type *acq_stop_cmd_ptr);

/*===========================================================================
FUNCTION srchgsm_send_acq_cleanup_req

DESCRIPTION
  This function sends cleanup req for RF driver POP for ACQ start
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchgsm_send_acq_cleanup_req(void);

/*===========================================================================
FUNCTION srchgsm_set_acq_result_poll_cb_func

DESCRIPTION
  This function sets the FCCH tone detect result poll call back function.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchgsm_set_acq_result_poll_cb_func(
  /* FCCH tone detect result poll call back function */
  SRCHGSM_CMD_RESULT_POLL_CB_FUNC_TYPE *cb_func);

/*===========================================================================
FUNCTION srchgsm_send_sch_decode_cmd

DESCRIPTION
  This function send GSM SCH decode async read command.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchgsm_send_sch_decode_cmd(
  /* SCH decode command parameter */
  srchcm_gsm_sch_decode_cmd_param_struct_type *sch_decode_cmd_ptr);

/*===========================================================================
FUNCTION srchgsm_set_sch_decode_search_width

DESCRIPTION
  This function sets the SCH decode search width. This function should be
  called after calling srchgsm_send_sch_decode_cmd.
  Function srchgsm_send_sch_decode_cmd calls gl1_hw_rx_async_sch_burst which
  sets the default SCH search width.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchgsm_set_sch_decode_search_width(
  /* number of Symbols for SCH to decode */
  uint16 search_width_qs
  #ifdef FEATURE_DUAL_SIM
  /* as_id to be passed to GL1 for DSDS */
  , sys_modem_as_id_e_type as_id
  #endif
  );

/*===========================================================================
FUNCTION srchgsm_set_acq_result_poll_cb_func

DESCRIPTION
  This function sets the SCH decode result poll call back function.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchgsm_set_sch_decode_result_poll_cb_func(
  /* SCH decode result poll call back function */
  SRCHGSM_CMD_RESULT_POLL_CB_FUNC_TYPE *cb_func);

/*===========================================================================
FUNCTION srchgsm_set_result_poll_cb_func

DESCRIPTION
  This function set the results call back function to call all the search result poll function
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchgsm_set_result_poll_cb_func(
  /* Call back function to call poll result function*/
  SRCHGSM_CMD_RESULT_POLL_CB_FUNC_TYPE *cb_func);

/*===========================================================================
FUNCTION srchgsm_check_mdsp_status

DESCRIPTION
  This function returns the sleep status of GSM mDSP.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE if mDSP is awake
  FALSE otherwise

SIDE EFFECTS
  None
===========================================================================*/
extern boolean srchgsm_check_mdsp_status( void );

/*===========================================================================
FUNCTION srchgsm_check_mdsp_suspend_after_cleanup

DESCRIPTION
  This function checks if GSM mDSP is already sent to suspend. If not, it 
  sets a callback to keep polling until GSM mDSP is finished with suspend cmd. 
  This is only used during IDLE W2G. 
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE of FALSE based on mDSP suspend

SIDE EFFECTS
  None
===========================================================================*/
extern boolean srchgsm_check_mdsp_suspend_after_cleanup(
  /* Cmd cleanup poll call back function */
  SRCHGSM_CMD_CLEANUP_POLL_CB_FUNC_TYPE *cb_func);

/*===========================================================================
FUNCTION srchgsm_set_cleanup_poll_cb_func

DESCRIPTION
  This function sets the a callback to keep polling until GSM mDSP is 
  finished with suspend cmd. This is used during FACH W2G module cleanup. 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchgsm_set_cleanup_poll_cb_func(
  /* Cmd cleanup poll call back function */
  SRCHGSM_CMD_CLEANUP_POLL_CB_FUNC_TYPE *cb_func);

/*===========================================================================
FUNCTION srchgsm_check_pending_cmd_q

DESCRIPTION
  This function checks cmd Q for any pending cmd that need not be sent
  as the part of cleanup.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE of FALSE based on remaining pending commands

SIDE EFFECTS
  None
===========================================================================*/

extern boolean srchgsm_check_pending_cmd_q(
  /* Cmd cleanup poll call back function */
  SRCHGSM_CMD_CLEANUP_POLL_CB_FUNC_TYPE *cb_func);

/*===========================================================================
FUNCTION srchgsm_clean_pending_cmd_q

DESCRIPTION
  This function cleans cmd Q for any pending cmd that need not be sent
  as the part of cleanup.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE of FALSE based on remaining pending commands

SIDE EFFECTS
  None
===========================================================================*/
extern void srchgsm_clean_pending_cmd_q(void);

/*===========================================================================
FUNCTION srchgsm_set_cmd_cleanup_pre_cb

DESCRIPTION
  This function sets the callback function for srchgsm_cmd_cleanup_pre_cb_func.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE of FALSE based on remaining pending commands

SIDE EFFECTS
  None
===========================================================================*/
extern void srchgsm_set_cmd_cleanup_pre_cb(
  /* Cmd cleanup pre call back function */
  SRCHGSM_CMD_CLEANUP_PRE_CB_FUNC_TYPE *cb_func);

/*===========================================================================
FUNCTION srchgsm_init_gsm

DESCRIPTION
  This function initializes GSM for W2G functionality.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchgsm_init_gsm(
  /* Ptr to task waiting for this to be done */
  rex_tcb_type*  task_ptr,
  /* Signal handler */
  void           (* task_wait_handler)(rex_sigs_type),
  /* Timeout signal 1 */
  rex_sigs_type  task_wait_sig,
  /* Timeout signal 2 */
  rex_sigs_type  task_wait_sig_1);

/*===========================================================================
FUNCTION srchgsm_get_error_status

DESCRIPTION
  This function returns the error status of GSM RxFE.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE means it's bad
  FALSE means it's good

SIDE EFFECTS
  None
===========================================================================*/
extern boolean srchgsm_get_error_status(void);

/*===========================================================================
FUNCTION srchgsm_set_error_status

DESCRIPTION
    This function sets the error status of GSM RxFE.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchgsm_set_error_status(boolean status);

/*===========================================================================
FUNCTION srchgsm_edge_block_cb

DESCRIPTION
    This function is a callback from MCPM when a GSM EDGE core reset happens.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchgsm_edge_block_cb(void);

/*===========================================================================
FUNCTION srchgsm_init_gl1

DESCRIPTION
  This function starts off a delayed/parallelized version of the gsm 
  initializations needed to handle GSM measurements while in WCDMA.  This 
  should save ~20 ms from wl1m_init, which is where a blocking call used to 
  happen.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/
extern void srchgsm_init_gl1(void);


/*===========================================================================
FUNCTION srchgsm_gl1_hw_cm_gsm_init

DESCRIPTION
  This function starts of the gsm 
  initializations needed to handle GSM measurements while in WCDMA.  
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/
extern void srchgsm_gl1_hw_cm_gsm_init(void);

/*===========================================================================
FUNCTION srchgsm_process_gl1_init

DESCRIPTION
  This function sets GL1 init state to ON. W2G search commands can be issued now.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/
extern void srchgsm_process_gl1_init(void);

/*===========================================================================
FUNCTION srchgsm_deinit_gl1

DESCRIPTION
  Used to de-initialize the gl1 functions needed by WCDMA for W2G searches
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/
extern void srchgsm_deinit_gl1(void);

/*===========================================================================
FUNCTION srchgsm_init_gl1_now

DESCRIPTION
  This function initializes GL1 SW so WCDMA can make use of GSM function calls 
  for GSM searches.  GSM must have been initialized before anything else can 
  take place for W2G, so think of calling this as a pre-requisite.

  Currently this is called shortly after wl1m_init() in ISR context.
  
DEPENDENCIES
  Must be called only from seq.c's seq_generic_event_handler() function.  
  Cannot be called generically.

RETURN VALUE
  None

SIDE EFFECTS
  Initializes GL1 sw so WCDMA can search for GSM cells.
===========================================================================*/
extern void srchgsm_init_gl1_now(void);

/*===========================================================================
FUNCTION srchgsm_query_ready_for_cmds

DESCRIPTION
  This function lets parent-modules poll whether srchgsmdrv is ready to accept 
  commands to be sent to GL1.  Parent-modules should check this function before
  sending any commands.  Otherwise srchgsmdrv may crash if it is sent a command
  before it is ready.
  
DEPENDENCIES
  Should only be called by code that runs between wl1m_init and wl1m_deinit.  
  Additionally the code that sends commands must only be active during this 
  time.  Otherwise it's possible to have a scenario where this is checked just 
  before wl1m_deinit, and then the command can come after the wl1m_deinit.  
  This will cause srchgsmdrv to crash.

RETURN VALUE
  Returns TRUE if srchgsmdrv is ready for commands to gsm
  Returns FALSE if srchgsmdrv is NOT ready for commands yet

SIDE EFFECTS
  None
===========================================================================*/
extern boolean srchgsm_query_ready_for_cmds(void);

extern boolean srchgsm_gl1_init_pending(SRCHGSM_GL1_INIT_PENDING_CB_FUNC_TYPE *cb_func);


/*===========================================================================
FUNCTION SRCHGSM_SEND_CMD_HANDLER

DESCRIPTION
  This function will handle the timing when we send cmd to FW. If frame num of 
  the cmd is not equal to curr GFN, we have to set a critical section before 
  sending the cmd to a queue for ISR. While frame number of the cmd now is at 
  curr GFN, we need to send it right away to FW without locking it. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchgsm_send_cmd_handler(
  /* Num of cmds in the list */
  uint8 num_cmd, 
  /* List of cmd seqeunce */
  srchcm_gsm_send_cmd_seq_type *cmd_seq);

/*===========================================================================
FUNCTION srchcm_get_cur_gsm_rssi_meas_state

DESCRIPTION
  This function returns the current RSSI measurement state.

DEPENDENCIES
  None

RETURN VALUE
  RSSI measuremnt state.

SIDE EFFECTS
  None
===========================================================================*/
extern srchcm_gsm_meas_state_enum_type srchcm_get_cur_gsm_rssi_meas_state(void);

/*===========================================================================
FUNCTION srchcm_set_gsm_rssi_meas_state

DESCRIPTION
  This function sets the RSSI meas state

DEPENDENCIES
  None

RETURN VALUE
  RSSI measuremnt state.

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcm_set_gsm_rssi_meas_state(
  /* RSSI meas state */
  srchcm_gsm_meas_state_enum_type state);

/*===========================================================================
FUNCTION srchcm_get_cur_gsm_bsic_id_state

DESCRIPTION
  This function returns the current BSIC Id measurement state.

DEPENDENCIES
  None

RETURN VALUE
  BSIC Id measuremnt state.

SIDE EFFECTS
  None
===========================================================================*/
extern srchcm_gsm_meas_state_enum_type srchcm_get_cur_gsm_bsic_id_state(void);

/*===========================================================================
FUNCTION srchcm_set_gsm_bsic_id_meas_state

DESCRIPTION
  This function sets the BSIC Id measurement state.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcm_set_gsm_bsic_id_meas_state(
  /* BSIC Id measurement state */
  srchcm_gsm_meas_state_enum_type state);

/*===========================================================================
FUNCTION srchcm_get_cur_gsm_bsic_reconf_state

DESCRIPTION
  This function returns the current BSIC reconf measurement state.

DEPENDENCIES
  None

RETURN VALUE
  BSIC reconf measuremnt state.

SIDE EFFECTS
  None
===========================================================================*/
extern srchcm_gsm_meas_state_enum_type srchcm_get_cur_gsm_bsic_reconf_state(void);

/*===========================================================================
FUNCTION srchcm_set_gsm_bsic_reconf_meas_state

DESCRIPTION
  This function sets the BSIC reconf measurement state.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcm_set_gsm_bsic_reconf_meas_state(
  /* BSIC Id reconf state */
  srchcm_gsm_meas_state_enum_type state);

/*===========================================================================
FUNCTION srchgsm_query_w2g_dch_fach_meas_ongoing

DESCRIPTION
  This function is to determine if W2G DCH or FACH meas is in the progress.
  
RETURN VALUE
  NONE
  
SIDE EFFECTS
=============================================================================*/
boolean srchgsm_query_w2g_dch_fach_meas_ongoing(void);

#ifdef FEATURE_QSH_DUMP
/*===========================================================================
FUNCTION srchgsm_get_gsmdrv_state_info

DESCRIPTION
  This function is to collect gsm meas driver state info for debugging.  

DEPENDENCIES  
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchgsm_get_gsmdrv_state_info(void *debug_buf);
#endif /* FEATURE_QSH_DUMP */

/*===========================================================================

FUNCTION SRCHGSM_TOO_CLOSE_TO_FRAME

DESCRIPTION
  This function returns TRUE if we are less than 3ms from the GFN boundary
  when the startup GFN is the current GFN.

DEPENDENCIES
  
RETURN VALUE
  TRUE: not enough time for prebuild
  FALSE: enough time for prebuild

SIDE EFFECTS
  None.
===========================================================================*/
boolean srchgsm_too_close_to_frame(
uint8 start_frame_num
#ifdef FEATURE_DUAL_SIM
/* as_id received from RRC. Passed to GSM L1 for DSDS */
, sys_modem_as_id_e_type as_id
#endif
);


#endif /* SRCHGSMDRV_H */
