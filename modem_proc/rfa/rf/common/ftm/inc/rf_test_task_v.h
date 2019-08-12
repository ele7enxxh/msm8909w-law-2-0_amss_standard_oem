#ifndef RF_TEST_TASK_V_H
#define RF_TEST_TASK_V_H
/*===========================================================================

           F T M    S E R V I C E S    H E A D E R    F I L E

DESCRIPTION
  This file contains declarations associated with the FTM Services module.

REFERENCES
  None

Copyright (c) 2003 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/rf_test_task_v.h#1 $
   
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/29/15   rmb     Creating new FTM WCDMA Signal for Temp comp in FTM RF mode. 
07/25/14   par     Creating new FTM BER signal for GSM NS 
02/15/13   tws     Back out previous change.
02/17/13   tws     Handling of FTM activate and deactivate CMD from MMOC task has been added
05/09/12   aki     Mainlined ftm_kick_watchdog function
04/19/12   ggs     Added FTM_RF_GSM_CMD_Q_SIG for handling FTM GSM MSGR 
                   response messages from RF task 
02/16/12   ans/ars Changed messages to MSG_# interface to capture logs in crash 
                   dumps.  
02/14/12   sb      Changes to support FTM logging for GSM  
02/14/12   sar     Reinstated TDSCDMA features.
01/28/12   sar     Feature reduction changes.
12/28/11   ars     Added ftm_sig_clk_rex_wait_no_clear()  
11/30/11   aki     Added ftm_kick_watchdog function
09/14/11   can     Merge MDM9K tips to NikeL.
07/11/11   hy      Including TD-SCDMA support.
07/07/11   sar     Removed deprecated file.
04/29/11    av     Change in ftm_gsm_rx_disable(). The wait is now on the task that 
                    is running: FTM or RF task. Takes care of crash between G to L FTM
                    transition 
02/24/11   sty     Added C++ protection
12/09/10   aro     Removed unnecessary includes
12/08/10   aro     Fixed include filename
12/07/10   aro     Added Support for SV Enabled API
09/10/10   ems     Added FTM_SEQ_STATUS_REQ_SIG for async status reporting 
09/10/10   ems     Support for FTM Sequencer 
01/08/10   sar     Fixed copile error by changing FTM_TASK_H to RF_TEST_TASK_V_H. 
12/15/09   sar     Renamed file to rf_test_task_v.h from ftm_task.h.
11/14/09   sar     Removed BT Support from rfa ftm.
09/14/09   sr      Added #define for FTM_RF_ASYNC_SIG
09/09/09   aro     Added a check to see if priority val is valid
09/09/09   aro     Removed DateTime and Author + Extra Space Removal
09/02/09   bmg     Added comdef.h (and target.h for unfortunate comdef.h
                   MSVC featurization) to insure FEATURE_FACTORY_TESTMODE is
                   defined.
09/01/09   ad      Featurized this entire file since ftm_task.c is also featurized
08/13/09   pl      removed uneccessary __inline of FTM_LEDS
08/11/09   mkv     Merge MODEM_LTE_DEV into rfa2
07/22/09   mkv	   Fixing MODEM_LTE_DEV compiler warnings.
07/22/09   mkv	   Fixing compiler warnings.
07/22/09   aro     Added Feature to manage changing of FTM Task Priority
04/06/09   aro     Added a New Signal to communicate with RFM Async APIs
12/17/07   ad      Dspdiag fns have changed parameter types - now event*
                   Included ftm.h to bring in the ftm_event_type struct def.
05/18/06   tm      Initial release for dsp diagnostics driver - new FTM subsystem.
                   Event mechanism to conserve signal defs, and support for delayed
                   response.
06/16/05   dp      Add support for FTM EGPRS BER feature.
03/15/05   dp      Change FTM WCDMA/GSM BER signal definitions.
01/28/05   jfc     Added no_clear versions to ftm_sig_rex_wait(_cb)
11/12/04   ra      Changed msg mask from MSG_SSID_DFLT to MSG_SSID_FTM.
10/07/04   wd      Removed redundant functions ftm_gsm_wait and ftm_wait.
                   Added ftm_sig_rex_wait_cb function which can be passed
                   to drivers that will need to wait while executing on ftm 
                   task context.
01/31/03   xw      created
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/
#include "rfa_variation.h"
#include "comdef.h"
#include "target.h"

#ifdef FEATURE_FACTORY_TESTMODE
#include "queue.h"
#include "prot.h"
#include "ftmicap.h"
#include "msg.h"
#include "ftm.h"
#include "ftm_sequencer_definitions.h"
#ifdef __cplusplus
extern "C" {
#endif


/*===========================================================================
 
                   LOCAL GLOBAL VARIABLES and CONSTANTS

===========================================================================*/
typedef enum
{
  FTM_MSG_LVL_FATAL,
  FTM_MSG_LVL_ERROR,
  FTM_MSG_LVL_HIGH,
  FTM_MSG_LVL_MED,
  FTM_MSG_LVL_LOW,
  FTM_MSG_LVL_MAX
} ftm_msg_lvl_type;


/* FTM signals */
#define FTM_CMD_Q_SIG          0x0001    /* A command is available in the queue. */
#define FTM_RPT_TIMER_SIG      0x0002    /* Used to signal a watchdog report.    */ 
#define FTM_HS_CMD_SIG         0x0004    /* This signal is used by the test      */
#define FTM_ACK_SIG            0x0008    /* State change ack signal              */
#define FTM_CMD_RSP_SIG        0x0010    /* Command response signal              */
#define FTM_CLK_WAIT_SIG       0X0020    /* Used generic for ftm_clk_rex_wait    */ 
#define FTM_SIG_WAIT_SIG       0X0040    /* Used generic for ftm_sig_rex_wait    */ 
#define FTM_SIG_WAIT_SIG2      0x00010000    /* Used generic for ftm_sig_rex_wait    */
#define FTM_ASYNC_PKT_SIG      0x0080    /* Used to process asyn packets         */
#ifdef FEATURE_CDMA1X
#define FTM_ENABLE_PROT_SIG    0x0200    /* FTM enable protocol signal           */
#define FTM_MC_READY_SIG       0x0400    /* MC FTM ready siganl                  */
#endif
#define FTM_SEARCH_SIG         0x1000
#define FTM_RF_ASYNC_SIG       0x2000    /* Used by FTM Callbacks in RF          */

#ifdef FEATURE_GSM
    #define FTM_GSM_Q_SIG   0x4000000    /* FTM GSM logging Q */
  #ifdef FEATURE_FTM_GSM_BER 
    #define FTM_GSM_BER_CMD_Q_SIG  0x00020000  /* FTM GSM BER Cmd Q */
    #define FTM_GSM_BER_EXIT_CMD_Q_SIG  0x08000000  /* FTM GSM BER Exit Cmd Q */
    
    #if defined(FEATURE_GSM_EGPRS) && defined(FEATURE_FTM_EGPRS_BER) 
        #define FTM_EGPRS_BER_CMD_Q_SIG 0x00100000
    #endif
  #endif
#define FTM_RF_GSM_CMD_Q_SIG      0x40000000   /* FTM RF GSM CMD Q. Used by RF to signal FTM task */
#endif

#ifdef FTM_HAS_UMTS
  #ifdef FEATURE_FTM_WCDMA_BER
    #define FTM_WCDMA_BER_CMD_Q_SIG     0x00040000  /* FTM WCDMA BER External Cmd Q */
    #define FTM_WCDMA_BER_INT_CMD_Q_SIG 0x00080000  /* FTM WCDMA BER Internal Cmd Q */
  #endif

  #ifdef FEATURE_WCDMA
  #define FTM_WCDMA_EXECUTE_TEMP_COMP_SIG 0x10000000 /* FTM WCDMA Temp Comp enable signal*/
  #endif
#endif

#ifdef FEATURE_LTE
#define FTM_LTE_CMD_Q_SIG      0x00200000   /* LTE FTM Signal                       */
#endif

#define FTM_SEQ_SIG            0x00400000   /* FTM Sequencer Signal                 */
#define FTM_SEQ_STATUS_REQ_SIG 0x00800000   /* FTM Sequencer Async status reporting */

#ifdef FTM_HAS_TDSCDMA  
  #ifdef FTM_HAS_TDSCDMA_BER
    #define FTM_TDSCDMA_BER_CMD_Q_SIG     0x01000000  /* FTM TDSCDMA BER External Cmd Q */
    #define FTM_TDSCDMA_BER_INT_CMD_Q_SIG 0x02000000  /* FTM TDSCDMA BER Internal Cmd Q */
  #endif /*FTM_HAS_TDSCDMA_BER*/
#endif /*FTM_HAS_TDSCDMA*/

#define FTM_NUM_CMDS           5


/* defines to use with ftm_sig_clk_rex_wait */
#define FTM_WAIT_FOREVER        0 
#define FTM_WAIT_TIMEOUT        FTM_CLK_WAIT_SIG
/*===========================================================================

                                  MACROS

===========================================================================*/
#define FTM_MSG_FATAL(x_fmt, a, b, c) \
    MSG_3 (MSG_SSID_FTM, MSG_LEGACY_FATAL, x_fmt, a, b, c)

#define FTM_MSG_ERROR(x_fmt, a, b, c) \
    MSG_3 (MSG_SSID_FTM, MSG_LEGACY_ERROR, x_fmt, a, b, c)

#define FTM_MSG_HIGH(x_fmt, a, b, c) \
    MSG_3 (MSG_SSID_FTM, MSG_LEGACY_HIGH, x_fmt, a, b, c)

#define FTM_MSG_MED(x_fmt, a, b, c) \
    MSG_3 (MSG_SSID_FTM, MSG_LEGACY_MED, x_fmt, a, b, c)

#define FTM_MSG_LOW(x_fmt, a, b, c) \
    MSG_3 (MSG_SSID_FTM, MSG_LEGACY_LOW, x_fmt, a, b, c)


/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/
LOCAL void ftm_req (void);

LOCAL FTM_Sequencer_ErrorCode ftm_dispatch( FTM_Sequencer_ContextID        iContextID,
			                                unsigned short                 iRequestPacketSize,
			                                unsigned char*                 pRequestBytes,
			                                unsigned short*                piResponsePacketSize,
                                            FTM_Sequencer_ResponsePacket*  pResponseBytes
                                          );
  
/*===========================================================================

FUNCTION FTM_PROCESS_EVENTS

DESCRIPTION
   This function handles delayed rsp requests from Diag + events from other 
   susbsystems. Dispatches the appropriate functions and or macros.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/

LOCAL void ftm_process_events (void);


/*===========================================================================

FUNCTION FTM_DIAG_EVT_PROCESSOR

DESCRIPTION
   This function handles delayed rsp requests from Diag 

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/

void ftm_diag_evt_processor(ftm_event_type *evt_ptr);

/*===========================================================================

FUNCTION FTM_DIAG_MSG_CONFIG_CMD_PROCESSOR

DESCRIPTION
   This function handles 125 requests from Diag 

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/

void ftm_diag_msg_config_cmd_processor(ftm_event_type *evt_ptr);

/*===========================================================================

FUNCTION FTM_DIAG_DELAYED_RESPONSE_CMD_PROCESSOR

DESCRIPTION
   This function handles delayed rsp requests from Diag 

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/

void ftm_diag_delayed_rsp_cmd_processor(ftm_event_type *evt_ptr);

/*===========================================================================

FUNCTION ftm_is_task_running 

===========================================================================*/

#if defined (FEATURE_LTE) || defined (FEATURE_GSM)
boolean ftm_is_task_running ( void );
#endif /* ( FEATURE_LTE || FEATURE_GSM ) */

/* ==================================================================
FUNCTION FTM_MALLOC
DESCRIPTION
   Dynamically allocates a specified number of bytes and returns a
   pointer to this chunk of memory.
===================================================================== */
void *ftm_malloc (unsigned long num_bytes);


/* ==================================================================
FUNCTION FTM_FREE
DESCRIPTION
   Frees a chunk of memory allocated using ftm_malloc.
===================================================================== */
void ftm_free (void *mem_ptr);

/* ==================================================================
FUNCTION FTM_KICK_WATCHDOG

DESCRIPTION
   Kicks the watchdog timer and sets a timer to wake it again.
===================================================================== */
void ftm_kick_watchdog(void);

/* ==================================================================
FUNCTION FTM_ACTIVATE_PROTOCOL_OFFTARGET_TEST
DESCRIPTION
   The function is responsible for getting MB ready to FTM mode.
   This is called only during offtarget testing.
===================================================================== */
void ftm_activate_protocol_offtarget_test ( prot_act_e_type act_reason);

/* ==================================================================
FUNCTION FTM_ACTIVATE_PROTOCOL
DESCRIPTION
   The function is responsible for getting MB ready to FTM mode.
===================================================================== */
void ftm_activate_protocol( rex_tcb_type * caller_tcb_ptr,
                            rex_sigs_type task_nv_wait_sig,
                            void(* task_wait_func_ptr)(rex_sigs_type),
                            prot_act_e_type act_reason,
                            prot_trans_type trans_id );

/* ==================================================================
FUNCTION FTM_ACTIVATE_PROTOCOL
DESCRIPTION
   The function is responsible for release any resource used by FTM mode.  
 (for example, deregister isr handler from gstmr_handlers[],etc)
===================================================================== */
void ftm_deactivate_protocol( rex_tcb_type * caller_tcb_ptr,
                              rex_sigs_type task_nv_wait_sig,
                              void(* task_wait_func_ptr)(rex_sigs_type),
                              prot_deact_e_type deact_reason,
                              prot_trans_type trans_id);


/*===========================================================================
FUNCTION FTM_SIG_REX_WAIT
DESCRIPTION
  This procedure waits for specified signals to be set.  While it waits,
  this procedure continues to set the Watchdog report signal and reset
  the mc_rpt_timer.
===========================================================================*/
rex_sigs_type ftm_sig_rex_wait ( rex_sigs_type mask );

/*===========================================================================
FUNCTION FTM_SIG_REX_WAIT_NO_CLEAR
DESCRIPTION
  This procedure waits for specified signals to be set.  While it waits,
  this procedure continues to set the Watchdog report signal and reset
  the mc_rpt_timer. At end of function, signals are not cleared.
===========================================================================*/
rex_sigs_type ftm_sig_rex_wait_no_clear (rex_sigs_type mask );

/*===========================================================================
FUNCTION RF_FTM_SIG_REX_WAIT
DESCRIPTION
  This procedure waits for specified signals to be set.  While it waits,
  this procedure continues to set the Watchdog report signal and reset
  the timer. The wait is on the task that is currently running.
===========================================================================*/
void rf_ftm_sig_rex_wait(uint32 msec );

/*===========================================================================
FUNCTION FTM_SIG_REX_WAIT_CB
DESCRIPTION
  This function is a stub to be able to call ftm_sig_rex_wait where a
  [ void func (uint32) ] function signature is needed.
===========================================================================*/
void ftm_sig_rex_wait_cb ( rex_sigs_type mask );

/*===========================================================================
FUNCTION FTM_SIG_REX_WAIT__NO_CLEAR_CB
DESCRIPTION
  This function is a stub to be able to call ftm_sig_rex_wait_no_clear where 
  a [ void func (uint32) ] function signature is needed.
===========================================================================*/
void ftm_sig_rex_wait_no_clear_cb (rex_sigs_type mask);

/*===========================================================================
FUNCTION FTM_SIG_REX_SET
DESCRIPTION
  This procedure sets a signal for ftm_task to handle.
===========================================================================*/
void ftm_sig_rex_set ( rex_sigs_type mask );

/*===========================================================================
FUNCTION FTM_CLK_REX_WAIT
DESCRIPTION
   This function does a wait in ms, but does not hug the cpu. instead it lets
   the OS (REX) run and other tasks run.            
===========================================================================*/
void ftm_clk_rex_wait (uint32 msec);

/*===========================================================================
FUNCTION FTM_SIG_CLK_REX_WAIT
DESCRIPTION
  This procedure waits for specified signals to be set.  While it waits,
  this procedure continues to set the Watchdog report signal and reset
  the mc_rpt_timer. The clk addition means that this function is capable
  of waiting for a signal with a time out, instead of waiting forever.
===========================================================================*/
rex_sigs_type ftm_sig_clk_rex_wait
(
  rex_sigs_type   mask,        /* Mask of signals to wait for */
  uint32          msec         /* Timeout value in msecs */
);

/*=========================================================================== 
FUNCTION FTM_SIG_CLK_REX_WAIT_NO_CLEAR
DESCRIPTION
  This procedure waits for specified signals to be set.  While it waits,
  this procedure continues to set the Watchdog report signal and reset
  the mc_rpt_timer. The clk addition means that this function is capable
  of waiting for a signal with a time out, instead of waiting forever.
  It does not clear the signals it is waiting for.
===========================================================================*/
rex_sigs_type ftm_sig_clk_rex_wait_no_clear
(
  rex_sigs_type   mask,        /* Mask of signals to wait for */
  uint32          msec         /* Timeout value in msecs */
);

/*===========================================================================
FUNCTION FTM_LEDS
DESCRIPTION  
  This procedure writes to the LEDs port shifting and masking the data.
===========================================================================*/
#ifdef FEATURE_FTM_LEDS    
#error code not present
#endif /* FEATURE_FTM_LEDS */

/*===========================================================================

FUNCTION       FTM_PRINTF
DESCRIPTION
  This function is a printf clone, it will work just as printf does, but 
  will output to string to TBD. 
===========================================================================*/
void ftm_printf(ftm_msg_lvl_type msg_lvl_enum, const char *fmt, ... );

/*----------------------------------------------------------------------------*/
boolean
ftm_set_task_priority
(
  rex_priority_type p_pri       /* the new priority */
);

/*----------------------------------------------------------------------------*/
rex_priority_type
ftm_get_task_priority
(
  void
);

#ifdef __cplusplus
}
#endif


#endif /* FEATURE_FACTORY_TESTMODE */
#endif /* #ifndef RF_TEST_TASK_V_H */
