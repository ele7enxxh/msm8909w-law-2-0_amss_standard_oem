#ifndef HDRTX_H
#define HDRTX_H
/*===========================================================================

                     H D R   T R A N S M I T   T A S K

GENERAL DESCRIPTION
  The HDR Transmit task runs the following protocols:
   - Radio Link Protocol (RLP),
   - Signalinng Link Protocol (SLP),
   - Packet Consolidatio Protocol (PCP),
   - Access Channel MAC Protocol (ACMAC),
   - Reverse Traffic Channel MAC Protocol (RTCMAC).
   
EXTERNALIZED FUNCTIONS
  hdrtx_task
    Entry point to the HDR Transmit task.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The hdrtx_task function contains the HDR Transmit task.  It must be
  specified in a call to rex_def_task and started by the Main Control task.

Copyright (c) 2000 - 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrtx.h_v   1.7   06 May 2002 19:28:48   omichael  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/mac/api/hdrtx.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/11/15   kss     Added support for modem tx power stats.
02/03/15   mbs     Forced 2HS interval between delivery of 2 FW cmds
07/29/14   tnp     FR22375: Fix Medium compiler warnings in HDR
03/14/14   dsp     FR19338: Reduce call setup time by skipping Tx RF shutdown. 
06/26/13   wsh     Added Selective TxD feature
10/25/12   wsh     Do not flood NAS during TAU/SRB2 unavailability 
08/23/12   ukl     Added one more update for SU API cleanup.
03/20/12   cnx     Resolved TCB compiler errors for Dime. 
03/16/12   arm     Resolve TCB errors for Dime. 
01/04/12   rmg     Added hdrtx_wait_for_msg().
07/27/11   cnx     Merged Optimized Handoff changes.
06/21/11   rmg     Added hdrtx_task_wait().
05/02/11   kss     Restored signal for timer use.
06/15/10   ms/rmg  Added signal for MRLP stats calculation.
09/17/09   etv     Added support for RMAC0 Q6 interface changes.
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
09/12/08   etv     Cleaned up RF Powering up call sequence.
07/30/07   cc      Added protocol timer support (for rmac4).
01/23/07   spn     Changed the bitmastk for the SPD signal. L4 can handle
                   32bit signals now. 
12/21/06   spn     Added a signal for Stale Packet Dropping (SPD)
08/02/05   kss     Added hdrtx_rf_status_enum_type.
06/30/05   jyw     Replaced HDRTX_NV_CMD_SIG with HDRTX_IS890_RTAP_PKTGEN_SIG
10/13/03   kss     Added prototype for hdrtx_report_agc() function.
05/06/02   om      Added HDRTX_IS890_FTAP_LO_INIT_SIG signal
10/08/01   om      Added HDRTX_IS890_[F|r]TAP_MSG_SIG signals
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
06/15/01   kss     Added HDR enter/exit signals.
05/24/00   om      Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "hdr_variation.h"
#include "dsm.h"
#include "rcinit.h"
#include "hdrhai.h"
#include "hdrind.h"
#include "hdrl1_api.h"
#include "hdrfw_msg.h"

#ifndef FEATURE_CMI
#include "hdrtask.h"
#endif

#ifdef FEATURE_LTE_TO_HDR_OH
#include "emm_irat_if_msg.h"
#endif

/* <EJECT> */
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/** Address of the TCB for the task in which this protocol runs */
extern rex_tcb_type*    hdrtx_tcb_ptr;
/** Pointer to HDRDEC task TCB */
#define HDRTX_TASK_PTR hdrtx_tcb_ptr

/*---------------------------------------------------------------------------
                             Signal Masks
---------------------------------------------------------------------------*/
#define  HDRTX_RPT_TIMER_SIG           0x0001 /* Sig for watchdog timer    */
#define  HDRTX_EVENT_Q_SIG             0x0002 /* Sig for external queue    */
#define  HDRTX_AMAC_ISR_SIG            0x0004 /* Sig for ACMAC transmit    */
#define  HDRTX_RMAC_ISR_SIG            0x0008 /* Sig for RTCMAC transmit   */
#define  HDRTX_TIMER_SIG               0x0010 /* Sig for Tx start timer    */
#define  HDRTX_RMAC_RATELIM_TIMER_SIG  0x0020 /* rmac rate limit timer     */
#define  HDRTX_SLP_CB_SIG              0x0040 /* SLP sig to process cb's   */
#define  HDRTX_ENTER_HDR_SIG           0x0080 /* Sig for HDR entry         */
#define  HDRTX_EXIT_HDR_SIG            0x0100 /* Sig for HDR exit          */
#define  HDRTX_IS890_RTAP_PKTGEN_SIG   0x0200 /* Initialize the lo pkt buf */
#define  HDRTX_IS890_FTAP_MSG_SIG      0x0400 /* Fwd test app msg process. */
#define  HDRTX_IS890_RTAP_MSG_SIG      0x0800 /* Rev test app msg process. */
#define  HDRTX_IS890_FTAP_LO_INIT_SIG  0x1000 /* Initialize the lo pkt buf */

#ifdef FEATURE_HDR_SELECTIVE_TXD
#if 0
/* Revisit: may not be needed */
#define  HDRTX_TXD_TX_SHUTDOWN_SIG     0x2000 /* TxD exiting TX mode       */
#endif
#endif /* FEATURE_HDR_SELECTIVE_TXD */

/* The following three patterns are common across tasks and should not be 
   used for HDRTX task specific signal definitions 

   TASK_OFFLINE_SIG  = 0x00002000
   TASK_STOP_SIG     = 0x00004000
   TASK_START_SIG    = 0x00008000                                          */

#define  HDRTX_MOD_TASK_DONE_SIG       0x00080000 /* Signal for queued 
                               modulator tasks ( tx prefill and tx start ) */
#define  HDRTX_MSG_SIG                 0x00100000 /* Message signal */


/* RF Tx status type (used by enable_tx/disable_tx functions) */
typedef enum
{
  TX_POWERING_UP,
  TX_ON,
  TX_OFF,
  TX_OFF_RF_ON
} hdrtx_rf_status_enum_type;


/* Message encapsulation that can be queued from FW to HDRTX task */

typedef union
{
  msgr_hdr_struct_type              hdr;
  hdrfw_sw_rcv_msg_u                fw_msg;

#ifdef FEATURE_LTE_TO_HDR_OH
  emm_irat_failure_ind_type         irat_failure_ind;
#ifdef FEATURE_IRAT_DSM_SUPPORT
  emm_irat_3gpp2_ul_msg_cnf_type    irat_ul_cnf;
#else
  emm_irat_ul_msg_cnf_type          irat_ul_cnf;
#endif /* FEATURE_IRAT_DSM_SUPPORT */
#endif  /*FEATURE_LTE_TO_HDR_OH */

} hdrtx_msg_union_type;

typedef struct
{
  q_link_type           qlink;
  hdrtx_msg_union_type  msg;

} hdrtx_msg_s_type;

/* Message queue size */
#define HDRTX_MSG_QUEUE_SIZE            15

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRTX_QUEUE_CMD

DESCRIPTION
  This function queues a command on the hdrtx_event_q and sets the signal.
  The command data is copied into the command buffer before it is queued.  
  This function is called in the task context of whoever is giving the command.

DEPENDENCIES
  This routine must be multi-thread safe, as two different tasks may give
  commands to protocols in the TX task at the same time.

  cmd_size must not be larger than hdrtx_protocol_cmd_union_type, because 
  this is the data type the command gets copied into.

PARAMETERS
  protocol_name - Who the command is for
  input_cmd_ptr - Pointer to a union of all protocols in HDRTX subtask
  cmd_size      - Size of the command being given

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrtx_queue_cmd
(
  hdrhai_protocol_name_enum_type protocol_name,
  void * input_cmd_ptr, 
  uint32 cmd_size 
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRTX_QUEUE_IND

DESCRIPTION
  This function queues an indication on the hdrtx_event_q and sets the 
  signal.  If there is any data associated with the indication, it is copied 
  into the buffer before it is queued.  This function is called in the task 
  context of whoever is giving the indication.

DEPENDENCIES
  This routine must be multi-thread safe, as two different tasks may give
  an indication to protocols in the TX task at the same time.

  ind_data_size must not be larger than hdrind_ind_data_union_type, because 
  this is the data type the command gets copied into.

PARAMETERS
  protocol_name - Who the indication is for
  ind_name      - Global enumeration of indications
  input_ind_ptr - Pointer to the indication data
  ind_data_size - The size of the data associated with this indication.
                  This value is undefined if the input_ind_ptr
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrtx_queue_ind
(
  hdrhai_protocol_name_enum_type protocol_name,
  hdrind_ind_name_enum_type ind_name,
  void *input_ind_data_ptr
);


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRTX_QUEUE_MSG

DESCRIPTION
  This function queues a message on the hdrtx_event_q and sets the signal.
  The queued buffer simply holds a pointer to the DSM item chain that is
  the message.  This function is called in the task context of whoever is 
  giving the message, which is always the task in which SLP runs.

DEPENDENCIES
  None

PARAMETERS
  protocol_name - Who the message is for
  protocol_instance - Instance of the protocol the message is for
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrtx_queue_msg
( 
  hdrhai_protocol_name_enum_type  protocol_name,
  hdrhai_protocol_instance_enum_type protocol_instance,
  dsm_item_type               *item_ptr 
);


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRTX_QUEUE_TIMER

DESCRIPTION
  This function queues a timer event on the hdrtx_event_q and sets the
  signal.

DEPENDENCIES
  None

PARAMETERS
  protocol_name - Who the timer is for
  timer_id - The ID of the timer that has expired

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrtx_queue_timer
(
  hdrhai_protocol_name_enum_type protocol_name,
  uint32 timer_id
);


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRTX_REPORT_AGC

DESCRIPTION
  This function reports Tx agc values.  No more, no less.  The parameter
  is not used (pass 0).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrtx_report_agc( 
  int32 ms 
);


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRTX_ENABLE_TX

DESCRIPTION
  This function takes care of enabling TX RF hardware. The function returns 
  immediately.  An optional callback function is called when the warmup is 
  complete.

DEPENDENCIES
  Should be called from Tx task context.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern hdrtx_rf_status_enum_type hdrtx_enable_tx
(
  void (*callback_ptr)(hdrtx_rf_status_enum_type)
    /* Function to be called when RF is ready */ 
);


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRTX_DISABLE_TX

DESCRIPTION
  This function takes care are disabling TX hardware.  Unlike the enable,
  no wait is involved.

DEPENDENCIES
  None.

PARAMETERS
  skip_tx_disable - Specifies if CP wants to skip disabling Tx, 
                    If true, then we skip disabling Tx RF and MCPM. 

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern hdrtx_rf_status_enum_type hdrtx_disable_tx
( 
   boolean skip_disable_tx 
);


/*===========================================================================

FUNCTION HDRTX_TASK_WAIT

DESCRIPTION
  This function is intended to be used from entities running within hdrtx 
  task context to block hdrtx task from running until a particular signal
  is set.

DEPENDENCIES
  None.

PARAMETERS
  Signal to wait for.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrtx_task_wait
(
  uint32 sig
);


/*===========================================================================

FUNCTION HDRTX_WAIT_FOR_MSG

DESCRIPTION
  This function is intended to be used by entities running within hdrtx 
  task context to block hdrtx task from running until a particular msg
  is received via msg router.

DEPENDENCIES
  None.

PARAMETERS
  Message to wait for.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrtx_wait_for_msg
( 
  uint32   req_msg
);

#ifdef FEATURE_HDR_QTA
/*===========================================================================

FUNCTION HDRTX_PSEUDO_ENABLE_TX

DESCRIPTION
  This function fakes enabling TX RF hardware as it is already enabled.
  The function returns immediately.  An optional callback function is
  called when the warmup is complete.

DEPENDENCIES
  Must be called from the HDR Tx task.
  
RETURN VALUE
  RF status.

SIDE EFFECTS
  None.

===========================================================================*/
hdrtx_rf_status_enum_type hdrtx_pseudo_enable_tx
(
  void (*callback_ptr)(hdrtx_rf_status_enum_type)
    /* Function to be called when RF is ready */ 
);
#endif /* FEATURE_HDR_QTA */

/*===========================================================================
FUNCTION HDRTX_PAUSE_FW_CMD_DELIVERY

DESCRIPTION
  This function spaces apart delivery of RMAC_ENABLE_CARRIER_CMD to FW
  such that two successive cmds are at least 2 half slots apart. This
  is needed to avoid inconsistent handling of two successive cmds in FW

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  May cause slight delay in TX establishment or teardown procedure
===========================================================================*/
void hdrtx_pause_fw_cmd_delivery();

/*===========================================================================
FUNCTION HDRTX_UPDATE_TX_POWER_DURATION_STATS

DESCRIPTION
  
 
DEPENDENCIES
  None

PARAMETERS
  

RETURN VALUE
  None

SIDE EFFECTS
  None
 ===========================================================================*/
void hdrtx_update_tx_power_stats_with_duration
(
   int interval
     /* In slots */
);

/*===========================================================================
FUNCTION HDRTX_UPDATE_TX_POWER_DURATION_STATS

DESCRIPTION
  
 
DEPENDENCIES
  None

PARAMETERS
  

RETURN VALUE
  None

SIDE EFFECTS
  None
 ===========================================================================*/
void hdrtx_update_tx_power_stats
(
   int interval,
   int tx_power
);



/*===========================================================================
FUNCTION HDRTX_CONFIG_MODEM_TX_POWER_STATS

DESCRIPTION
  This function enables or disables tx duration logging.
 
DEPENDENCIES
  None

PARAMETERS
  enable - Whether to start or stop logging

RETURN VALUE
  None

SIDE EFFECTS
  None
 ===========================================================================*/
void hdrtx_config_modem_tx_power_stats
(
  boolean enable
        /* Enable/disable statistic collection */   
);

/*===========================================================================
FUNCTION HDRTX_GET_MODEM_TX_POWER_STATS

DESCRIPTION
  This function fills Tx duration values into the provided table.
 
DEPENDENCIES
  None

PARAMETERS
  tx_power_bins - array of max_bins items, to be filled with duration stats. 

RETURN VALUE
  None

SIDE EFFECTS
  None
 ===========================================================================*/
void hdrtx_get_modem_tx_power_stats
(
  uint32 tx_power_bins[]
    /* Pointer to the bin array */
);

#endif
