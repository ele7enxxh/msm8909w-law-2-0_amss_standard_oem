#ifndef HDRMCI_H
#define HDRMCI_H

/*===========================================================================

        I N T E R N A L   H D R   S U B T A S K   D E F I N I T I O N S

DESCRIPTION
  This contains the internal declarations for the HDRMC Task.

Copyright (c) 2000 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdrmci.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/19/12   cnx     Resolved TCB compiler errors for Dime. 
07/27/11   cnx     Merged Optimized Handoff changes.
04/27/11   cnx     Supported callback registration.
09/18/09   wsh     CMI: support changes from modem DATA
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
01/08/07   yll     Added support for CSNA.
10/04/06   pba     Added logging module command support.
09/08/06   spn     Added a new command type for reservation module.
06/12/06   mpa     Updates for 7500 RPCing of HDR rev0
04/28/05   etv     Added HDRMC_BLOCKING_NV_WRITE_SIG.
08/28/05   mpa     Added BCMCS command support.
05/09/05   ar/pba  Integrated Data Over Signaling protocol code
03/07/05   pba     Modified RUP timers to use callback functions instead of 
                   signals.
09/09/04   mpa     Added PAC MC command support.
07/02/04   mpa     Removed BCMCS-related signal.
05/05/04   kss     Added HDR BCMCS-related signal.
03/05/03   mpa     Removed HDRMC_INIT_SYNC_MSG_TIMER_SIG
03/05/03   mpa     Removed HDRMC_UATI_ASSIGN_TIMER_SIG
08/15/02   kss     Added HDRMC_MAC_DEACTIVATE_COMPLETE_SIG.
07/17/02   dna     Queue timer expiration events on hdrmc.event_q because timer
                   callback runs as a separate thread and causes problems.
06/05/02   dna     Queue NV writes and write the items one at a time.
04/03/02   mpa     Removed HDRAMP_DUAL_ADDR_TIMER_SIG
03/29/02   dna     Changed timers to use REX callback instead of indications
03/15/02   dna     Removed INIT acquisition timer, so the signal is now unused.
01/21/02   sh      Removed HDRMC_SD_TIMER_SIG
11/30/01   om      Added security layer support
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
07/10/01   dna     Code cleanup changes
                   Removed LUP config response timer signal
06/21/01   vas     Removed hmp delay timer signal
06/16/01   vas     Added function hdrmc_get_exit_reason
05/01/01   dna     Add hdrlup_cmd to union, many signal changes for hdrmc task
04/05/01   vas     hdrmc_protocol_cmd_union_type define hmp for both X4 & IS856
03/29/01   dna     Re-arranged signals, added LUP config timer signal
03/19/01   dna     Added support for IS-856 compliant session layer
03/10/01   sh      Added HDRMC_IDLE_CONFIG_RES_TIMER_SIG
02/07/01   dna     Renamed HDRMC_TEST_1_SIG to 
                   HDRMC_HMP_DELAY_AFTER_ACQ_TIMER_SIG because it looks like
                   its use may be permanent.
09/19/00   sh      added Idle State related signals
02/02/00   dna     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "hdr_variation.h"
#include "comdef.h"
#include "hdrbuf.h"
#include "mc.h"

#ifdef FEATURE_CMI
#include "mc_v.h"
#endif

#include "hdrmc_v.h"
#include "hdralmp.h"
#include "hdrovhdi.h"
#include "hdralmpi.h"
#include "hdrhmpi.h"
#include "hdriniti.h"

#include "hdrstream.h"
#include "hdrstreami.h"

#include "hdrrupi.h"
#include "hdridlei.h"

#include "hdrcon.h"
#include "hdrconi.h"
#include "hdrmc.h"

#include "hdrsmp.h"
#include "hdrsmpi.h"
#include "hdramp.h"
#include "hdrampi.h"
#include "hdrscp.h"
#include "hdrscpi.h"
#include "hdrlupi.h"
#include "hdrpacc.h"
#include "hdrdosi.h"
#include "hdrfcp.h"
#include "hdrlog.h"

#ifdef FEATURE_HDR_BCMCS
#include "hdrbccpi.h"
#endif /* FEATURE_HDR_BCMCS */

#ifdef FEATURE_HDR_QOS
#include "hdrmrlp_api.h"
#endif /* FEATURE_HDR_QOS */

#include "hdrcsna.h"

#ifdef FEATURE_LTE_TO_HDR_OH
#include "hdrsap.h"
#endif /* FEATURE_LTE_TO_HDR_OH */

/* <EJECT> */ 
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/


/* <EJECT> */ 
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* Address of the TCB for the HDRMC task.  This #define is only to be used
 * in routines that are not part of a specified HDR protocol.  Each protocol
 * has its own <protocol_tag>_TASK_PTR define to specify what task TCB the 
 * individual protocol uses.
 */
#define HDRMC_TASK_PTR hdrmc_tcb_ptr

/*--------------------------------------------------------------------------
                             Signal Masks
--------------------------------------------------------------------------*/
/* This signal is set when a command is put on the hdrmc_cmd_q. */
#define  HDRMC_CMD_Q_SIG                  0x0001

/* This signal is set when NV completes a command */
#define  HDRMC_NV_CMD_SIG                 0x0002

/* This signal is set when the mc_rpt_timer expires     */
#define  HDRMC_RPT_TIMER_SIG               0x0004
  
/* This signal is set when a command is put on the hdrmc_event_q. */
#define  HDRMC_EVENT_Q_SIG                0x0008

/* This signal is set when a decision is made to exit HDR mode */
#define  HDRMC_EXIT_SIG                   0x0010

/* This signal is set when QuickConfig supervision timer expires */
#define HDRMC_OVHD_QC_TIMER_SIG           0x0020

/* This signal is set when SectorParameters supervision timer expires */
#define HDRMC_OVHD_SP_TIMER_SIG           0x0040

/* This signal is set when AccessParameters supervision timer expires */
#define HDRMC_OVHD_AP_TIMER_SIG           0x0080

#define HDRMC_NV_WRITE_SIG                0x0100

/* This signal is set when a MAC layer deactivation is completed. */
#define HDRMC_MAC_DEACTIVATE_COMPLETE_SIG 0x0200

/* This signal is set when a blocking NV write completes. */
#define HDRMC_BLOCKING_NV_WRITE_SIG       0x0400

/* Signal 0x0800 is available */

/* This signal is set when idle completes the monitor state entering processing */
#define HDRMC_IDLE_MONITOR_ENTERED_SIG    0x1000

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Signals 0x2000 thru 0x8000 are system wide and defined in task.h        */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Signal 0x00010000 is available */

/* Signal 0x00020000 is available */

/* Signal 0x00040000 is available */

/* This signal is set when the Keep Alive Timer expires */
#define HDRMC_SMP_KA_TIMER_SIG           0x00080000

/* Signal 0x00100000 is available */

/* Signal 0x00200000 is available */

/* Signal 0x00400000 is available */

/* This signal is set when the SCP Configuration Response Timer expires */
#define HDRMC_SCP_CONFIG_RSP_TIMER_SIG   0x00800000

/* This signal is set when the AN Init State Timer expires */
#define HDRMC_SCP_AN_INIT_STATE_TIMER_SIG 0x01000000

/* This signal is set when the SEC task completes its operation */
#define HDRMC_KEP_KEYGEN_DONE_SIG         0x02000000

/* This signal is set when the STREAM config rsp timer expires */
#define HDRMC_STREAM_CONFIG_RSP_TIMER_SIG 0x04000000

/*--------------------------------------------------------------------------
                    Commands, Indications, and Messages for 
                    protocols running in the MC task context
--------------------------------------------------------------------------*/

/* Because each task handles a disjoint set of commands, it makes sense
 * to have the data type used for command buffers be unique to the task level.
 * Indications are common to all protocols, so the data type used for 
 * indication buffers is shared by all tasks that contain HDR protocols.  
 * Similarly, because DSM items are used for messages, the data type used 
 * for message buffers is shared by all tasks that contain HDR protocols.
 * See hdrbuf.h for more details.
 */
 
typedef union
{
  hdralmp_cmd_type almp;          /* Air Link Management Protocol Command */
  hdrinit_cmd_type init;          /* Initialization State Protocol Command */
  hdridle_cmd_type idle;          /* Idle State Protocol Command */
  hdrcon_cmd_type connected;      /* Connected State Protocol Command */
  hdrrup_cmd_type route;          /* Route Update Protocol Command */
  hdrovhd_cmd_type ovhd;          /* Overhead Messages Protocol Command */
  hdrsmp_cmd_type smp;            /* Session Management Protocol Command */
  hdramp_cmd_type amp;            /* Address Management Protocol Command */
  hdrscp_cmd_type scp;            /* Session Configuration Protocol Command */
  hdrstream_cmd_type stream;      /* STREAM Protocol Command */
  hdrhmp_cmd_type hmp;            /* HDR Messaging Protocol Command */
  hdrlup_cmd_type lup;            /* Location Update Protocol Command */
  hdrdos_cmd_type dos;            /* Data Over Signalling Protocol Command */
  hdrpacc_cmd_type pac;           /* Packet Application Controller Command */
  hdrfcp_cmd_type fcp;            /* Flow Control Protocol Command */
#ifdef FEATURE_HDR_BCMCS
  hdrbccp_cmd_type bcmcs;         /* Broadcast Protocol Command */
#endif /* FEATURE_HDR_BCMCS */
#ifdef FEATURE_HDR_QOS
  hdrmrlpresv_cmd_type resv;      /* HDR MRLP RESV Protocol Command */
#endif /*FEATURE_HDR_QOS */
  hdrlog_cmd_type log;            /* HDR Logging Module Command */
#ifdef FEATURE_HDR_CSNA
  hdrcsna_cmd_type  csna;         /* Circuit Services Notification App command */
#endif /* FEATURE_HDR_CSNA */
#ifdef FEATURE_LTE_TO_HDR_OH
  hdrsap_cmd_type sap;            /* Signaling Adaptation Protocol command */
#endif /* FEATURE_LTE_TO_HDR_OH */
} hdrmc_protocol_cmd_union_type;

typedef struct
{
  hdrbuf_hdr_type hdr;      /* header */
  union
  {
    hdrmc_protocol_cmd_union_type cmd;    /* <Task> = hdrmc, hdrrx, or hdrtx */
    hdrbuf_ind_type ind; /* All tasks use the same structure for indications */
    dsm_item_type *item_ptr;       /* The message is just a DSM item pointer */
    uint32 timer_id;                         /* ID of the timer that expired */
  } item;
} hdrmc_buf_type;

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRMC_QUEUE_CMD

DESCRIPTION
  This function queues a command on the hdrmc_event_q and sets the signal.
  The command data is copied into the command buffer before it is queued.  
  This function is called in the task context of whoever is giving the command.

DEPENDENCIES
  This routine must be multi-thread safe, as two different tasks may give
  commands to protocols in the MC task at the same time.

  cmd_size must not be larger than hdrmc_protocol_cmd_union_type, because 
  this is the data type the command gets copied into.

PARAMETERS
  protocol_name - Who the command is for
  input_cmd_ptr - Pointer to a union of all protocols in HDRMC task
  cmd_size - Size of the command being given

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void hdrmc_queue_cmd( hdrhai_protocol_name_enum_type protocol_name,
                          void * input_cmd_ptr, uint32 cmd_size );

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRMC_QUEUE_IND

DESCRIPTION
  This function queues an indication on the hdrmc_event_q and sets the 
  signal.  If there is any data associated with the indication, it is copied 
  into the buffer before it is queued.  This function is called in the task 
  context of whoever is giving the indication.

DEPENDENCIES
  This routine must be multi-thread safe, as two different tasks may give
  an indication to protocols in the MC task at the same time.

  ind_data_size must not be larger than hrgivei_ind_data_union_type, because 
  this is the data type the command gets copied into.

PARAMETERS
  protocol_name - Who the indication is for
  ind_name - Global enumeration of indications
  input_ind_ptr - Pointer to the indication data

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void hdrmc_queue_ind( hdrhai_protocol_name_enum_type protocol_name,
          hdrind_ind_name_enum_type ind_name,
          void *input_ind_data_ptr
   );

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRMC_QUEUE_MSG

DESCRIPTION
  This function queues a message on the hdrmc.event_q and sets the signal.
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

void hdrmc_queue_msg( hdrhai_protocol_name_enum_type protocol_name,
                      hdrhai_protocol_instance_enum_type protocol_instance,
                      dsm_item_type * item_ptr );

/*===========================================================================

FUNCTION HDRMC_QUEUE_TIMER

DESCRIPTION
  This function queues a timer event on the hdrmc.event_q and sets the
  signal.  

DEPENDENCIES
  None

PARAMETERS
  protocol_name - Who the indication is for
  timer_id - The ID of the timer that has expired

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrmc_queue_timer
( 
  hdrhai_protocol_name_enum_type protocol_name,
  uint32 timer_id
);
 
 
/*===========================================================================

FUNCTION HDRMC_EXECUTE_CB

DESCRIPTION
  This function looks for registered clients according to the events passed in
  and execute the callback functions if a registered client is found.

DEPENDENCIES
  None
 
PARAMETERS 
  cb_event - the event type that triggers the callback
  params - parameters of callback function
 
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrmc_execute_cb
(
  hdrmc_cb_params_rsp_union_type *params
); 


#endif               /* HDRMCI_H */
