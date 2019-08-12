#ifndef HDRAMAC_H
#define HDRAMAC_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 H D R   A C C E S S   C H A N N E L   M A C   
                            
                             P R O T O C O L
                                    
                   E X T E R N A L   H E A D E R   F I L E       

GENERAL DESCRIPTION
  This module contains the external interface of the HDR Access Channel 
  Media Access Control (MAC) Protocol.

EXTERNALIZED FUNCTIONS
  hdramac_init
    Initialize the Access Channel MAC protocol.
    
  hdramac_activate
    Activate the Access Channel MAC protocol.

  hdramac_deactivate
    Deactivate the Access Channel MAC protocol.
  
  hdramac_is_active
    Indicate whether protocol is active.
    
  hdramac_is_in_attempt
    Determine if protocol is currently in an access attempt.

  hdramac_start_attempt
    Initiate an access attempt.         
        
  hdramac_get_last_tx_pwr
    Return transmit power level used for the most recent access probe.         

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The protocol must be initialized after system startup prior to activating 
  the protocol.  The procotol must be activated before an access probe 
  sequence can be sent.

  Copyright (c) 2000-2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* EJECT */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: O:/src/asw/COMMON/vcs/hdramac.h_v   1.10   10 Sep 2001 11:38:56   kevins  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/mac/api/hdramac.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/21/14   sat     Mainlined hybrid_during_overhead to always TRUE
03/14/14   dsp     FR19338:Reduce call setup time by skipping Tx RF shutdown.
01/27/14   sat     Mainlined FEATURE_HYBRID_ACCESS_DURING_OVERHEAD.
03/16/12   arm     Resolve TCB errors for Dime. 
12/09/11   rmg     Added support for calling tx stop confirmation callback 
                   after processing stop access command for tuneaway.
09/21/07   kss     Included definitions from hdramaci.h for cleanup.
04/27/07   kss     Added support for PN Randomization Delay.
03/28/07   rmg     Added hdramac_override_config().
08/23/06   hal     Added support to reset the current access attempt.
06/07/06   hrk     Support for tune away during access.
03/20/06   hrk     Support for fixing access probe rate.
01/13/06   kss     Added hdramac_update_persist_index() (needed for RUIM).
12/06/05   etv     Added support for Session configuration using SCM.
10/25/05   jyw     Fixed the featurization.
10/06/05   dna     Inform SD if PSIST value makes HDR unusable
09/14/05   jyw     Fixed the IS890A featurization.
07/08/05   hrk     Rev A changes for Enhanced Access Channel MAC.
09/07/01   kss     Added hdramac_set_silence_params() function.
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
12/13/00   kss     Added hdramac_start_attempt().
11/03/00   dna     Added HDR event logging
03/30/00   st      Created.

===========================================================================*/

/* EJECT */
/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"
#include "dsm.h"
#include "rex.h"
#include "qw.h"
#include "hdrhai.h"
#include "hdrind.h"
#include "hdrovhd.h"
#include "hdrscmamac.h"

/* EJECT */
/*===========================================================================
                   
                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions and declarations for constants, macros,
types, variables and other items needed by this module.

===========================================================================*/

/* Nominal gain for 9.6 kbps data channel */
#define HDRAMAC_DATA_NOM_GAIN_9K6 15    /* 3.75 dB */

/* Nominal gain for 19.2 kbps data channel */
#define HDRAMAC_DATA_NOM_GAIN_19K2 27   /* 6.75 dB */
/* Nominal gain for 38.4 kbps data channel */
#define HDRAMAC_DATA_NOM_GAIN_38K4 39   /* 9.75 dB */

/* Minimum value of the power ratio of ACK channel to Pilot channel */
#define HDRAMAC_MIN_ACK_CHAN_GAIN  -192   /* -48 dB */
/* Minimum value of the power ratio of DRC channel to Pilot channel */
#define HDRAMAC_MIN_DRC_CHAN_GAIN  -192   /* -48 dB */

/* preamble length = 16 slots */
#define HDRAMAC_PREAMBLE_LENGTH_16_SLOTS  16
/* preamble length = 4 slots  */
#define HDRAMAC_PREAMBLE_LENGTH_4_SLOTS   4

/* Upper 32 I bits. */
#define HDRAMAC_I_MASK_UPPER_BITS      0x00000300

/* Access Cycle length that the I & Q masks are based on. */
#define HDRAMAC_PN_CYCLE_NUM_MOD       256

/* Task context block pointer. */
#define HDRAMAC_TASK_PTR              HDRTX_TASK_PTR

/* Transaction timer defined in IS-856 */
#define HDRAMAC_T_ACMP_TRANSACTION_MS 1000

#ifdef FEATURE_HDR_HYBRID_ACCESS
/* Buffer between transaction time end and when search should re-enable
   tune-away. */
#define HDRAMAC_TUNEAWAY_BUFFER_MS 30

/* Buffer for accepting tuneaway time -  don't accept tuneaway
   time in past or within 10 ms */
#define HDRAMAC_TUNEAWAY_TIME_BUFFER_IN_SLOTS 6

/* Per probe AcAck timeout duration, in slots */
#define HDRAMAC_ACACK_TIMEOUT_PERIOD  280

/* Access packet CRC and trailer/pad field length */
#define HDRAMAC_PKT_CRC_TRAILER_LEN 3

#endif /* FEATURE_HDR_HYBRID_ACCESS */

/* number of slots in 5.12s cycle */
#define HDRAMAC_SLOTS_PER_PAGE_CYCLE  3072

/* Slot (relative to frame boundary) to handle frame interrupt 
   to load encoder */
#define HDRAMAC_FRAME_INTERRUPT_ENC_SLOT  12

/* Slot (relative to frame boundary) to start probe */
#define HDRAMAC_FRAME_INTERRUPT_START_SLOT  15


/*---------------------------------------------------------------------------
                             PROTOCOL CONSTANTS                        
---------------------------------------------------------------------------*/

#define N_ACMP_APERSIST        4       /* Persistence values.     */
#define T_ACMP_CYCLE_LEN      16       /* 256 slots = 16 frames.  */
#define T_ACMP_PROBE_TIMEOUT   8       /* 128 slots = 8 frames.   */
#define T_ACMP_TRANSACTION    37       /* 1 second = 37.5 frames. */

#ifdef FEATURE_HDR_HYBRID_ACCESS
/* This value is used to calculate the time for AT to wait until after 
   transmitting an access probe in the access attempt before returning a 
   TransmissionFailed indication */
  #define T_ACMP_MAX_DELAY_PREV_PROBE  300
#endif /* FEATURE_HDR_HYBRID_ACCESS */

/* Packet size definitions. */

#ifdef FEATURE_HDR_REVA_L1
  #define HDRAMAC_PKT_9K6_FRAME_DATA_LEN   29
  #define HDRAMAC_PKT_19K2_FRAME_DATA_LEN  61
  #define HDRAMAC_PKT_38K4_FRAME_DATA_LEN  125 
#endif /* FEATURE_HDR_REVA_L1 */

/* ---------------- Access terminal persistence constants ---------------- */

/* Persistence vector index (0,1,2, or 3). 0 = non-emergency value. */

#define HDRAMAC_PSIST_INDEX                  0   

/* Persistence value corresponding to 100 % access probability. */
 
#define HDRAMAC_PSIST_MAX_PROB             0x0 

/* Probabilility fraction maximum value (unsigned binary fraction). */

#define HDRAMAC_PSIST_PROB_ONE_VAL  0xffffffffUL

/* Probability fraction 1/2 value. */

#define HDRAMAC_PSIST_PROB_HALF_VAL 0x80000000UL



/*---------------------------------------------------------------------------
                              PROTOCOL COMMANDS
                              
 The following codes are for use with the 'cmd' field of the protocol command
 header type defined below, to define the operation requested by the command.                              
---------------------------------------------------------------------------*/
/* 
   Note: The cmd enum values are fixed and they are used by the QXDM database
   New enum tag should be added after the current enum tag and also
   assign a fixed value
*/  

typedef enum 
{
  HDRAMAC_DEACTIVATE_CMD=0,                      
  HDRAMAC_ACTIVATE_CMD=1,
  HDRAMAC_STOP_CMD=2,
  HDRAMAC_START_ATTEMPT_CMD=3,
  HDRAMAC_RESET_ATTEMPT_CMD=4
#ifdef FEATURE_HDR_HYBRID_ACCESS
,
  HDRAMAC_TUNE_AWAY_TIME_CMD = 5,
  HDRAMAC_TUNE_AWAY_STOP_TX_CMD = 6
#endif /* FEATURE_HDR_HYBRID_ACCESS */
} 
hdramac_cmd_enum_type;

/*--------------------------------------------------------------------------
                           COMMAND HEADER TYPE

 This header type is contained in every protocol command. It contains info
 common to all commands.
--------------------------------------------------------------------------*/

typedef struct 
{
  hdramac_cmd_enum_type   cmd;      /* Specifies which command to perform */
  hdrhai_protocol_name_enum_type sender; /* protocol giving command, for debug */
  boolean skip_tx_disable;         /* Specifies if CP wants to skip Tx disable */
} 
hdramac_cmd_hdr_type;


/*--------------------------------------------------------------------------
                             ACTIVATE COMMAND

 This command is given to activate the protocol.
--------------------------------------------------------------------------*/

typedef struct 
{
  hdramac_cmd_hdr_type   hdr;       /* Common header */
} 
hdramac_act_cmd_type;

/*--------------------------------------------------------------------------
                           DEACTIVATE COMMAND

 This command is given to activate the protocol.
--------------------------------------------------------------------------*/

typedef struct 
{
  hdramac_cmd_hdr_type   hdr;       /* Common header */
} 
hdramac_deact_cmd_type;

#ifdef FEATURE_HDR_HYBRID_ACCESS
/* Tune away time command structure */
typedef struct 
{
  hdramac_cmd_hdr_type   hdr;           /* Common header */
  boolean                no_tune_away;  /* Whether tune away or not */
  qword                  ta_time_slot;  /* Tune away time in slot */
}
hdramac_ta_time_cmd_type;


/* Callback function prototype for tuneaway stop tx command */
typedef void (*hdramac_ta_stop_tx_cb_type) ( void );

/* Tune away stop tx command structure */
typedef struct
{
  hdramac_cmd_hdr_type           hdr;   /* Common header */
  hdramac_ta_stop_tx_cb_type     cb;    /* Tune away stop tx
                                           callback function */
} 
hdramac_ta_stop_tx_cmd_type;

#endif /* FEATURE_HDR_HYBRID_ACCESS */

/*--------------------------------------------------------------------------
                        UNION OF PROTOCOL COMMANDS
                      
 This union type includes all command types, as well as the common header
 type, to define a type that can contain any of the command types.
--------------------------------------------------------------------------*/

typedef union 
{
  hdramac_cmd_hdr_type   hdr;           
  hdramac_act_cmd_type   act;
  hdramac_deact_cmd_type deact;
#ifdef FEATURE_HDR_HYBRID_ACCESS
  hdramac_ta_time_cmd_type ta_time;
  hdramac_ta_stop_tx_cmd_type stop_tx;
#endif /* FEATURE_HDR_HYBRID_ACCESS */
} 
hdramac_cmd_union_type;


/* Access MAC InitialConfiguration Attribute */
typedef struct
{
  uint8 probe_sequence_max;
  uint8 probe_backoff;
  uint8 probe_sequence_backoff;
} hdramac_initial_config_struct_type;

/* Access MAC PowerParameters Attribute */
typedef struct
{
  int8 data_offset_nom;
  int8 data_offset_9k6;
#ifdef FEATURE_HDR_REVA_L1
  int8 data_offset_19k2;
  int8 data_offset_38k4;
#endif /* FEATURE_HDR_REVA_L1 */
} hdramac_power_params_struct_type;

/* Structure for all configurable attributes */
typedef struct 
{
  hdramac_initial_config_struct_type initial;
  hdramac_power_params_struct_type   power_params;
#ifdef FEATURE_HDR_REVA_L1
  uint8  pn_delay_supported;             /* PNRandomizationDelaySupported */
  uint8  access_terminal_class_override; /* AccessTerminalClassOverride */
  uint8  apersistence_override;          /* APersistenceOverride     */
  uint8  terminal_access_rate_max;       /* Max Terminal Access Rate */
#endif /* FEATURE_HDR_REVA_L1 */
} hdramac_config_struct_type;

/* Override config structure*/
typedef struct
{
  hdrovhd_ap_msg_type ap_msg_override; 
                                /* ovhd_ap_msg_override */
  
  uint8  test_access_rate;      /* = 0  => Variable Access Rate
                                 * else => Fixed Access Rate
                                 */
  uint16 data_bytes;            /* Payload data byte count */

} hdramac_override_config_struct_type;

/*--------------------------------------------------------------------------
                             PROTOCOL REVISIONS

 This enum type defines the revisions of the AC MAC protocol.
--------------------------------------------------------------------------*/

/* Enum for various sub-types of Access MAC protocol */
typedef enum
{
  HDRAMAC_SUBTYPE_DEFAULT = 0,
    /* Default Access MAC */

  HDRAMAC_SUBTYPE_ENHANCED
    /* Enhanced Acess MAC */
} hdramac_subtypes_type;

/* ----------------------- Access attempt substates ---------------------- */

typedef enum
{
  HDRAMAC_ACC_START_SUBSTATE,      /* Access attempt start substate.       */
  HDRAMAC_ACC_AP_MSG_SUBSTATE,     /* Wait for Access Parameters substate. */
  HDRAMAC_ACC_SCHEDULE_SUBSTATE,   /* Schedule sequence substate.          */
  HDRAMAC_ACC_PERSIST_SUBSTATE,    /* Pre-sequence persistence substate.   */
#ifdef FEATURE_HDR_HYBRID_ACCESS
  HDRAMAC_ACC_TUNE_AWAY_SUBSTATE,  /* Tune away substate                   */
#endif /* FEATURE_HDR_HYBRID_ACCESS */
  HDRAMAC_ACC_TX_SETUP_SUBSTATE,   /* Probe TX power setup substate.       */
  HDRAMAC_ACC_TX_PRE_SUBSTATE,     /* Probe preamble substate.             */
  HDRAMAC_ACC_TX_DATA_SUBSTATE,    /* Probe data substate.                 */
  HDRAMAC_ACC_TX_DONE_SUBSTATE,    /* Probe done substate.                 */
  HDRAMAC_ACC_PRB_BKOFF_SUBSTATE,  /* Probe backoff setup substate.        */
  HDRAMAC_ACC_PRB_BKOFF2_SUBSTATE, /* Probe backoff delay substate.        */
  HDRAMAC_ACC_SEQ_BKOFF_SUBSTATE,  /* Sequence backoff setup substate.     */
  HDRAMAC_ACC_SEQ_BKOFF2_SUBSTATE, /* Sequence backoff delay substate.     */
  HDRAMAC_ACC_RX_ACK_SUBSTATE,     /* ACAck timeout setup substate.        */
  HDRAMAC_ACC_RX_ACK2_SUBSTATE,    /* ACAck timeout delay substate.        */
  HDRAMAC_ACC_END_SUBSTATE,        /* Access attempt end setup substate.   */
  HDRAMAC_ACC_END2_SUBSTATE        /* Access attempt end delay substate.   */
}
hdramac_substate_enum_type;


/* ------------------------- Access attempt data ------------------------- */

typedef struct
{
  hdramac_substate_enum_type  substate;   /* Access attempt substate.      */
  uint32  countdown;          /* Countdown counter, in 26.67ms frames.     */
   int16  frame_num;          /* Frame number counter, in 26.67ms frames.  */

  uint16  probe_cnt;          /* Current probe count.                      */
  uint16  seq_cnt;            /* Current probe seqeunce count.             */
  uint8   accolc;             /* ACCOLC value */
  uint8   persist_index;      /* Persistance index                         */
  uint32  persist_cnt;        /* Persistence counter, in AC cycles.        */
  uint32  persist_limit;      /* Max number of persistence tests.          */
  uint16  persist_value;      /* Persistance value from AP msg.            */
  uint32  persist_prob;       /* Persistence probability, binary fraction. */
  uint16  backoff_cycles;     /* Number of backoff cycles (probe/seq)      */
  boolean abort_pending;      /* Abort pending flag.                       */
  boolean attempt_in_progress;/* Probe in progess flag                     */

   int16  probe_init_adjust;  /* Probe initial adjust, from AP msg.        */
   int16  tx_pwr;             /* Probe current Tx power, 0.5 dB / lsb.     */
   int16  last_pwr;           /* Most recent probe Tx power, 0.5 dB / lsb. */

  uint16  data_bytes;         /* Payload data byte count.                  */
  boolean override_data_bytes;/* data_bytes override flag - used by HIT    */
#ifdef FEATURE_HDR_REVA_L1
  uint16  data_sent;          /* Payload transmitted byte count.           */
#endif /* FEATURE_HDR_REVA_L1 */
  boolean is_size_incl;       /* Packet size incl status, TRUE=Format B    */
  boolean is_secured;         /* Packet security status, TRUE=encrypted    */
  dsm_item_type* pkt_ptr;     /* Packet dsm item chain pointer.            */
  hdrhai_ati_record_type ati; /* Access terminal identifier (ATI) record.  */
  uint32  fcs;                /* The packet's FCS                          */
  uint16  num_encoder_pkts;   /* Number of packets the AC msg will span    */
  uint16  probe_len;          /* Access probe length, (frames).            */

  uint8   pn_cycle_num;       /* PN Cycle number for computing I and Q     */
  uint32  sector_id;          /* Sector ID of target sector.               */
  uint8   color_code;         /* Color code for computing I and Q          */
  qword   i_mask;             /* I mask for probe                          */
  qword   q_mask;             /* Q mask for probe                          */

  uint16  start_slot_time;    /* Slot time at ACMAC activation             */
  uint32  start_page_cycle;   /* Page cycle at ACMAC activation            */
  uint16  elapsed_frames;     /* Access attempt elapsed time, in frames.   */

  qword   scheduled_time;     /* Time next event is to occur               */
  boolean event_scheduled;    /* Whether a probe event is scheduled        */
  uint8   tx_offset;          /* Slot offset for Tx                        */
  uint8   preamble_len;       /* preamble length in slots                  */
  qword   silence_begin_time; /* Time of next reverse silence inverval     */
  uint32  silence_period;     /* Period of silence interval (frames)       */
  uint32  silence_duration;   /* Length of silence interval (frames)       */

#ifdef FEATURE_HDR_REVA_L1
  uint8   preamble_isr_cnt;   /* Number of ISR's needed for preamble TX   */
  hdramac_subtypes_type current_subtype;  /* Enhanced/Default AC MAC      */
  uint8   test_access_rate;   /* Fixed Access Rate                       */
#endif /* FEATURE_HDR_REVA_L1 */

  hdramac_config_struct_type cfg;
    /* Instance of AMAC Configuration struct */

  hdrscmamac_config_struct_type scm_cfg;
    /* Instance of SCMAMAC's definition of Config struct */

  hdramac_config_struct_type *config;
    /* Configuration parameters. */

#ifdef FEATURE_HDR_REVA_L1 

  uint8 gaup_attrib_verno[HDRSCMAMAC_NUM_GAUPABLE_ATTRIBS];
    /* Version numbers of GAUPable attributes */

  uint8* gaup_attribs[HDRSCMAMAC_NUM_GAUPABLE_ATTRIBS];
    /* Pointer to each of the GAUPable attrib in the config struct instance.
       Since all the GAUPable attribs are simple, uint8* is generic enough. */

#endif /* FEATURE_HDR_REVA_L1 */

#ifdef FEATURE_HDR_HYBRID_ACCESS
  boolean tuneaway_received;   /* Whether tuneaway command received */
  boolean no_tune_away;        /* Indicate whether there is tune away */
  qword   tuneaway_time;       /* Tune away time in slot */
  boolean tuneaway_time_ok;    /* Whether tune away time is ok */
  uint16  max_inter_probe_int; /* max inter probe interval in frames */
  boolean reject_tuneaway;
  boolean tuneaway_stopped;    /* Whether AC has been stopped for tuneaway */
#endif /* FEATURE_HDR_HYBRID_ACCESS */

  boolean tx_enabled;          /* Tracks whether ACMAC has requested Tx */
  boolean probe_delayed;       /* if the probe was delayed due to tune away */

  uint16  max_pn_randomization_delay; /* PN Randomization delay. */

  uint8   tx_frame_start_offset;      /* offset at which data frames begin. */

  boolean skip_tx_disable;    /* Tracks if CP has requested skip tx disable */
}
hdramac_struct_type;

/* ------------------ Access attempt logging statisics ------------------- */

typedef PACK(struct)
{
  uint8  transaction_id;
  uint8  probe_num_step;
  uint8  max_probe_sequence;
  uint8  probe_sequence_count;
  uint8  result;
  uint16 duration;
  uint32 success_count;
  uint32 failure_count;
  uint32 attempts_count;
  uint16 pn;
  uint16 sector_id_lsw;        /* Lower 16 bits of sector id */
  uint8  sector_id_usw;        /* Upper 8 bits of sector id */
  uint8  color_code;
  uint8  probe_count[16];      /* Probes per sequence */
} hdramac_attempt_log_type;


/* Persistence value corresponding to 0 % access probability (no access). */
#define HDRAMAC_PSIST_NO_ACCESS            0x3f 
 
#define HDRAMAC_RETAP_EACH_RATE_MODE_DISABLED 0xFF



/* EJECT */
/*===========================================================================
FUNCTION HDRAMAC_PROCESS_CMD

DESCRIPTION
  This function processes the commands for this protocol that were queued
  on the task command queue from a different task context.
  
DEPENDENCIES
  None.

PARAMETERS
  cmd_ptr - address of command structure.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdramac_process_cmd 
( 
  hdramac_cmd_union_type *cmd_ptr
);

/* EJECT */
/*===========================================================================
FUNCTION HDRAMAC_PROCESS_SIG

DESCRIPTION
  This function handles the task signals assigned to this protocol.
  
DEPENDENCIES
  None.

PARAMETERS
  signal - task signal to be processed.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdramac_process_sig 
( 
  rex_sigs_type signal                    
);

/* EJECT */
/*===========================================================================

FUNCTION HDRAMAC_PROCESS_MSG

DESCRIPTION
  This function processes incoming messages for this protocol.  The message
  ID field is extracted from the message and if successful the corresponding
  message processing function is dispatched.
  
DEPENDENCIES
  None.

PARAMETERS
  msg_ptr - address of dsm item chain containing message.
  protocol_instance - Instance of the protocol the message is for
  
RETURN VALUE
  None.

SIDE EFFECTS
  The message buffer memory is freed after the message has been processed.
  
===========================================================================*/
extern void hdramac_process_msg 
( 
  dsm_item_type *msg_ptr,
  hdrhai_protocol_instance_enum_type protocol_instance
);


/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRAMAC_RESET_ATTEMPT

DESCRIPTION
  This function queues a command for AMAC to reset the current access attempt.

DEPENDENCIES
  None.

PARAMETERS
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdramac_reset_attempt( hdrhai_protocol_name_enum_type caller );

/* EJECT */
/*===========================================================================
FUNCTION HDRAMAC_INIT

DESCRIPTION
  This function initializes the protocol prior to use.  This function should 
  be called once upon system startup.  The protocol configuration is 
  initialized and the protocol state is set to Inactive. 
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Protocol configuration is initialized and state is assigned.
===========================================================================*/
extern void hdramac_powerup_init ( void );


/* EJECT */
/*===========================================================================

FUNCTION HDRAMAC_UPDATE_PERSIST_INDEX

DESCRIPTION
  This function sets up the HDR AC MAC persistence index based on the 1x
  accolc NV value.

DEPENDENCIES
  THIS FUNCTION IS RUN IN HDRMC CONTEXT.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void hdramac_update_persist_index ( void );


/* EJECT */
/*===========================================================================
FUNCTION HDRAMAC_IS_ACTIVE

DESCRIPTION 
  This function queries the current state of the protocol.
  
DEPENDENCIES
  None.

PARAMETERS 
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  TRUE  = protocol is Active.
  FALSE = protocol is Inactive.

SIDE EFFECTS 
  None.
===========================================================================*/
extern boolean hdramac_is_active ( hdrhai_protocol_name_enum_type caller );

/* EJECT */
/*===========================================================================
FUNCTION HDRAMAC_IS_IN_ATTEMPT

DESCRIPTION 
  This function returns whether the access channel is currently sending 
  a probe (in an access attempt).
  
DEPENDENCIES
  None.

PARAMETERS 
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  TRUE  = the protocol is currently in an access attempt.
  FALSE = the protocol is not curently in an access attempt.

SIDE EFFECTS 
  None.
===========================================================================*/
extern boolean hdramac_is_in_attempt( hdrhai_protocol_name_enum_type caller );

/* EJECT */
/*===========================================================================
FUNCTION HDRAMAC_IND_CB

DESCRIPTION
  This callback function queues incoming indications for this protocol.  
  
DEPENDENCIES
  None.

PARAMETERS
  ind_name - Indication to process, is a global enum for all
                    possible indications in HDR signaling
  ind_data - Optional data pointer for indication specific data
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdramac_ind_cb 
( 
  hdrind_ind_name_enum_type ind_name, 
  void *ind_data
);

/* EJECT */
/*===========================================================================
FUNCTION HDRAMAC_MSG_CB

DESCRIPTION
  This callback function queues incoming messages for this protocol.  The
  AccessParameters message is forwarded to the Overhead Messages Protocol to
  be managed along with the other overhead messages.
  
DEPENDENCIES
  None.

PARAMETERS
  msg_ptr - address of dsm item chain containing message.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdramac_msg_cb 
( 
  dsm_item_type *msg_ptr  
);

/* EJECT */
/*===========================================================================
FUNCTION HDRAMAC_CONFIG_MSG_CB

DESCRIPTION
  This callback function queues incoming messages for the in config instance
  of this protocol.  The AccessParameters message is forwarded to the 
  Overhead Messages Protocol to be managed along with the other overhead 
  messages.
  
DEPENDENCIES
  None.

PARAMETERS
  msg_ptr - address of dsm item chain containing message.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdramac_config_msg_cb 
( 
  dsm_item_type *msg_ptr  
);

/* EJECT */
/*===========================================================================
FUNCTION HDRAMAC_ACTIVATE

DESCRIPTION
  This command transitions the protocol to the Active state.  If the protocol 
  is already in the Active state then no action is taken.
  
DEPENDENCIES
  The protocol must be initialized by calling the hdramac_init function prior
  to activation.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdramac_activate ( hdrhai_protocol_name_enum_type sender );

/* EJECT */
/*===========================================================================
FUNCTION HDRAMAC_DEACTIVATE

DESCRIPTION
  This command transitions the protocol to the Inactive state and clears the 
  supervision timer.  If the protocol is already in the Inactive state then 
  no action is taken.
  
DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)
  skip_tx_disable - Specifies if CP wants to skip disabling Tx,
                    If true, then we skip disabling Tx RF and MCPM. 

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdramac_deactivate 
( 
   hdrhai_protocol_name_enum_type sender,
   boolean skip_tx_disable 
);

/* EJECT */
/*===========================================================================
FUNCTION HDRAMAC_STOP

DESCRIPTION
  This command causes the ACMAC to finish its current probe as if it were
  the last.  If no response is received ACMAC returns 
  HDRIND_ACMAC_TRANSMISSION_STOPPED indication rather  than a failure.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdramac_stop ( hdrhai_protocol_name_enum_type sender );

/* EJECT */
/*===========================================================================
FUNCTION HDRAMAC_START_ATTEMPT

DESCRIPTION
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdramac_start_attempt ( hdrhai_protocol_name_enum_type sender );

/* EJECT */
/*===========================================================================
FUNCTION HDRAMAC_GET_LAST_TX_PWR

DESCRIPTION
  This function returns the last power level that was used to send a complete
  access probe.
  
DEPENDENCIES
  At lease one access probe must have been sent before this function is
  called.

PARAMETERS
  caller - The protocol calling this externalized function (for sw trace)
  
RETURN VALUE
  Power level - scaling TBD.

SIDE EFFECTS
  None.
===========================================================================*/
extern uint16 hdramac_get_last_tx_pwr ( hdrhai_protocol_name_enum_type caller );

/*===========================================================================

FUNCTION HDRAMAC_SET_SILENCE_DURATION                               EXTERNAL

DESCRIPTION
  This function sets the AMAC silence duration and offset.

DEPENDENCIES
  The parameter must not be NULL.
  
PARAMETERS
  silence_duration 
  silence_period 

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
extern void hdramac_set_silence_params
( 
  uint8 silence_duration,
  uint8 silence_period 
);

/*===========================================================================

FUNCTION HDRAMAC_PROCESS_IND

DESCRIPTION
  This function processes indications given to AMAC.

DEPENDENCIES
  None.

PARAMETERS
  ind_name - Name of the indication for AMAC to process
  ind_data_ptr - pointer to data associated with the indication

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdramac_process_ind
(
  hdrind_ind_name_enum_type ind_name,
  hdrind_ind_data_union_type * ind_data_ptr
);

/*===========================================================================

FUNCTION HDRAMAC_RESET_ACCESS_STATS

DESCRIPTION
  This function resets access attempt counts
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdramac_reset_access_stats( void );


/*===========================================================================

FUNCTION HDRAMAC_GET_PERSIST_INDEX

DESCRIPTION
  This function returns the current index in the apersistence array of the 
  AccessParameters message to use for calculating persistence backoff.

DEPENDENCIES
  None.

RETURN VALUE
  apersistence index, based on 1x access overload class

SIDE EFFECTS
  None.

===========================================================================*/
extern uint8 hdramac_get_persist_index( void );

#ifdef FEATURE_HDR_IS890A
/*===========================================================================
FUNCTION HDRAMAC_SET_EACH_RATE

DESCRIPTION
  This function specifies the enhanced access channel rate to be used in
  the next access attempt.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  Rate of EAC frame.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdramac_set_each_rate 
( 
  uint8 each_rate
);

#endif /* FEATURE_HDR_IS890A */

#ifdef FEATURE_HDR_HYBRID_ACCESS
/*===========================================================================

FUNCTION HDRAMAC_TA_TIME

DESCRIPTION
  Notify ACMAC tune away time.

DEPENDENCIES
  The ACMAC must be in the active state.

PARAMETERS
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdramac_ta_time
(
  hdrhai_protocol_name_enum_type  caller,
  boolean                         no_tune_away,
  qword                           ta_time_slot
);

/* EJECT */
/*===========================================================================

FUNCTION HDRAMAC_TA_STOP_ACCESS_CMD

DESCRIPTION
  Notify ACMAC to stop access attempt due to tuneaway.

DEPENDENCIES
  The ACMAC must be in the active state.

PARAMETERS
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdramac_ta_stop_access_cmd
(
  hdrhai_protocol_name_enum_type  caller,
  hdramac_ta_stop_tx_cb_type      cb
);

#endif /* FEATURE_HDR_HYBRID_ACCESS */

/* EJECT */
/*===========================================================================
FUNCTION HDRAMAC_OVERRIDE_CONFIG

DESCRIPTION
  This function overrides amac configuration with the passed values

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Values stored in hdramac_ap_msg and hdramac are modified
===========================================================================*/

extern void hdramac_override_config
(
  hdramac_override_config_struct_type * param_ptr
);


/*===========================================================================
FUNCTION HDRAMAC_CONFIGURE                                   EXTERNAL

DESCRIPTION
  This function does the AT-init configuration of Enh. AMAC attribute. 

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE - If the configuration was needed.
  FALSE - Otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean hdramac_configure
(
  hdrhai_protocol_name_enum_type sender
);

#endif /* HDRAMAC_H */

