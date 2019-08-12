#ifndef MDSP_GPRS_H
#define MDSP_GPRS_H
/*===========================================================================
                          M D S P _ G P R S . H


GENERAL DESCRIPTION
  This header file contains macros and funtion prototypes for general
  utility functionality for the mDSP gprs driver

  Copyright (c) 2000-2013 Qualcomm Technologies, Inc.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmdsp6/inc/mdsp_gprs.h#1 $
$DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when         who     what, where, why
----------  -------- ------------------------------------------------------
13/01/15      am     CR775652 Mismatch in monitor reading
09/09/14      jk     CR697308:GL1 changes to support HLin LLin sawless feature
28/07/14      pjr    CR700772 Changed mdsp_notify_power_mon_info to support two no_zones
20/06/14      pjr    DATA+MMS feature - USF utilisation calculation
19/06/14      ws     DATA+MMS feature - command FW to schedule monitors if GL1 can't find a space
14/11/13      js     CR549415 - Do not do reselection meas on yielded arfcns
27/11/13      cja    CR521061 Remove unused RF API and mainline FEATURE_GSM_RFA_TASK.
22/11/13      cja    CR455527 Remove featurisation from files in API directory
17/09/13      pjr    CR538893 Changes to enable CXM logging
21/08/13      npt    CR531633 - Remove FEATURE_DIME_MODEM dependencies
29/07/13      sk     CR519663 Partial QBTA bringup changes
20/06/13      cs     Major Triton TSTS Syncup
16/05/13      ab     CR487396 - Support EGPRS IR Reduced Window in GFW
05/12/12      pg     CR427361: GFW/RF shared memory interface cleanup
31/07/12      ky     Fixed the compilation issues after removing the redundant strcutures
                     in SW INTF.h file
29/06/12      jj     CR374950 GL1 changes for 38 symbol HMSC monitor
26/03/12      jj     CR344695 rename feature name FEATURE_GSM_GPRS_MSC34 to FEATURE_GSM_GPRS_MSC33
08/09/11      jj     CR302378  Added GL1 changes for RSB along with spectral inversion
16/08/11      jj     CR 299789 Reduced the pwr meas length to 48 symbol
27/01/11      cja    Changes for Nikel
08/02/10      ws     Merged QDSP6 changes from dev branch
05/08/09      ws     Removed FEATURE_GSM_MDSP_DTM it's now mainlined
06/06/09      ab     Feature FEATURE_GSM_EGPRS_IR_OPT for LCU
26/03/09      ws     Added PACKED_POST for QDSP6 targets
24/11/08      og     FEATURE_GSM_GPRS_MSC33 support.
16/10/08      cs     Update RX Sweep to use a full slot for each measurement
09/10/08      cs     Allow the RX Sweep to schedule shorter monitor duration
14/01/08      agv    New feature for Switched Mode Power Supply which will
                     provide better talk time.
09/23/05      gfr    Optimize DM active period to just during PDTCH decoding
08/05/05      gfr    PTCCHD has its own channel type
07/29/05      gfr    DTM support
06/10/05      gfr    Remove unused power measure code
05/18/05      gfr    Increase interference measure length to 156
05/18/05      gfr    Rename mdsp_pdch_id to mdsp_timeslot_type
05/03/05      gfr    Mainline FEATURE_GSM_MDSP_EER
04/08/05      gfr    Allow caller to specify whether to discard untxed data
03/22/05      gfr    Use tx tag to keep track of encoded data.
03/07/05      gfr    Add mdsp_gprs_IR_set_tfi
03/02/05      gfr    Move field parameters into mdsp_gprs.c
02/09/05      gfr    Reduce shared memory required for PTCCH bursts
01/31/05      gfr    Finalized support for incremental redundancy
01/25/05      gfr    Use PA transition profile provided by RF driver
01/24/05      gfr    General cleanup and restructure.
12/16/04      kt     Support for FEATURE_GSM_MDSP_ACI_DETECTION
12/06/04      yhong  Support for FEATURE_GSM_MDSP_EGPRS
12/06/04      gfr    Featurized polar RF
11/30/04      gfr    Support for polar RF and EER programming
09/13/04      gfr    Fixed puncturing scheme 3 value.
08/30/04      gfr    Lint cleanup.
08/26/04      gfr    Added first_burst parameter to rx command.
07/22/04      gfr    EGPRS support.
04/09/04      gfr    Minor code cleanup.
03/05/04      gfr    Lint cleanup.
01/30/04      gfr    Support for firecode bit.
01/12/04      gfr    Support for 6 monitors per frame.
06/03/03      gfr    Clean up driver if rx message is never available.
05/21/03      gfr    Take offsets instead of timeslots for scheduling power
                     measures.
05/13/03      gfr    Common power handler support.
05/12/03      gfr    Host scheduling of all power measures.
05/12/03      gfr    Added tx_alpha parameter to transmit command.
01/02/03      gfr    New mdsp interfaces.
09/06/02      gr     New uplink power control interface.
04/16/02      bk     Changed abort functionality i/f
04/11/02      bk/jc  Added mdsp_access_mode_type
03/14/02      bk/jc  Added support for GPRS power measure command.
12/12/01      jc     Add prototypes for _power_measure(), _power_measure_auto.
11/28/01      jc     Force mdsp_pdch_id to be 16 bits.
10/25/01      jc     Initial version.
===========================================================================*/
#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "gfw_sw_intf.h"
#include "mdsp_intf.h"

#define QS_PER_TS   625         /* number of quarter symbols in 1 timeslot */

/* Definitions for TX SCHEDULE failures */
#define MDSP_GPRS_TX_TRB_TTB_COMPLIANCE     1
#define MDSP_GPRS_TX_PREVIOUSLY_SCHEDULED   2

/* Value in usf list to indicate unallocated timeslot */
#define MDSP_GPRS_UNALLOCATED_USF    0xFFFF

/* Defines for the reason field of the rx burst schedule */
#define MDSP_GPRS_RX_BURST_SCHED_OK  0x8000

/* Decoded USF field constants */
#define MDSP_USF_DEC_VALID_POS        15
#define MDSP_USF_DEC_VALID_MASK       0x8000
#define MDSP_USF_DEC_USF_VAL_MASK     0x7

/* Power measure field constants */
#define MDSP_PWR_MEAS_READY_MASK      0x8000
#define MDSP_PWR_MEAS_LENGTH_MASK     0x7FFF



/* Macros to determine if a given channel type is GPRS or EGPRS */
#define IS_MDSP_GPRS_CHANNEL(chan)  \
   ((chan >= MDSP_PDTCHU_CS1 && chan <= MDSP_PTCCHU_EXT) || chan == MDSP_PTCCHD)

#ifdef FEATURE_GSM_MDSP_EGPRS
#define IS_MDSP_EGPRS_CHANNEL(chan)  \
   (chan >= MDSP_PDTCHU_MCS1 && chan <= MDSP_PDTCHU_MCS9)
#endif


/* Lengths of interference and power measurements in symbols */
#define MDSP_INT_MEAS_LEN  (156)

#define MDSP_PWR_MEAS_LEN  (48)


#ifdef FEATURE_GSM_MDSP_IR
/* In the worst case if a decode is issued and then IR is disabled in the
   same frame, it will be 6 frames before we are guaranteed the mDSP is
   done decoding the data and using DM for IR */
#define MDSP_MAX_FRAMES_TO_DM_DONE 6
#endif



/****************************************************************************
 ****************************************************************************
                            mDSP   GPRS  STRUCTURES


      Various structure definitions used by the mdsp driver for gprs.


 ****************************************************************************
 ***************************************************************************/


/* GPRS message type */
typedef enum
{
   MDSP_USF_SPECIFIC,
   MDSP_PDCH_SPECIFIC,
#ifdef FEATURE_GSM_MDSP_EGPRS
   MDSP_LOOPBACK_SPECIFIC
#endif
} mdsp_specific_msg_type;


/* Allocation types */
typedef enum
{
   MDSP_FIXED_ALLOCATION,
   MDSP_DYNAMIC_ALLOCATION,
   MDSP_EXT_DYNAMIC_ALLOCATION
} mdsp_access_mode_type;


/* Timeslot numbers */
typedef enum
{
   MDSP_TN_0 = 0,
   MDSP_TN_1,
   MDSP_TN_2,
   MDSP_TN_3,
   MDSP_TN_4,
   MDSP_TN_5,
   MDSP_TN_6,
   MDSP_TN_7
} mdsp_timeslot_type;


/* Coding scheme */
typedef enum
{
   MDSP_CODING_CS1 = 0,
   MDSP_CODING_CS2,
   MDSP_CODING_CS3,
   MDSP_CODING_CS4,
#ifdef FEATURE_GSM_MDSP_EGPRS
   MDSP_CODING_MCS1,
   MDSP_CODING_MCS2,
   MDSP_CODING_MCS3,
   MDSP_CODING_MCS4,
   MDSP_CODING_MCS5,
   MDSP_CODING_MCS6,
   MDSP_CODING_MCS7,
   MDSP_CODING_MCS8,
   MDSP_CODING_MCS9
#endif
} mdsp_coding_type;

#ifdef FEATURE_GSM_MDSP_EGPRS
/* Puncturing schemes */
typedef enum
{
  MDSP_PUNCTURING_1 = 0x0,
  MDSP_PUNCTURING_2 = 0x1,
  MDSP_PUNCTURING_3 = 0x2
} mdsp_puncturing_type;

/* Transmit data structure for EGPRS packets */
typedef struct
{
   uint16               *hdr;
   uint16                len;
   uint16               *data[2];
   mdsp_puncturing_type puncturing[2];
} mdsp_egprs_tx_data_type;
#endif

/* Transmit data structure for GPRS packets */
typedef struct
{
   uint16  len;
   uint16 *data;
} mdsp_gprs_tx_data_type;

/* Structure of Burst Scheduler Results  - used with gprs_sync_rx cmd*/
typedef PACKED struct PACKED_POST
{
  uint16                    len;          /* len of packet in words */
  uint16                    seq_num;      /* seq num issued in command */
  uint16                    reason[8];    /* bit 15= True for successful schedule */
                                          /*       = FALSE otherwise,             */
                                          /* failure code in bits 0-7             */
} mdsp_burst_sched_struct;

/* Structure of decoded usfs - used with gprs_sync_rx cmd */
typedef PACKED struct PACKED_POST
{
  uint16                    len;          /* len of packet in words      */
  uint16                    seq_num;      /* seq num issued in command   */
  uint16                    usf[8];       /* bit 15= True for valid usf  */
                                          /*    0-2 = usf value          */
} mdsp_decoded_usf_struct;


/* Burst type to use for each of the four possible monitors in a frame */
extern const mdsp_burst_type mdsp_gprs_mon_metrics_burst_types[];


#ifdef FEATURE_GSM_MDSP_IR
/* Format of the incremental redundancy memory - this is external memory used
   by the firmware to store the soft decisions.  The mDSP reads and writes it
   using Data Mover.  ARM should never read it (except for debugging) and needs
   to be careful to flush the cache before doing so. */

#define MDSP_IR_DATA_SOFT_DECISION_SIZE 306 /* No change needed here for MSC34 */

typedef PACKED struct PACKED_POST
{
   uint32   coding_scheme;
   uint32   puncturing_scheme;
   uint32   bsn;
   uint32   seq_num;
   uint32   rescale_factor;
   uint32   checksum;
   uint32   soft_decisions[MDSP_IR_DATA_SOFT_DECISION_SIZE];
} mdsp_IR_data_struct;

/* Max EGPRS window size is 512, so that's how much space we need - yes this
   is a very large amount of memory, but it is needed for IR */
#ifdef  FEATURE_GSM_GPRS_MSC33
#define MDSP_IR_DATA_SIZE  640
#else
#define MDSP_IR_DATA_SIZE 512
#endif

  extern mdsp_IR_data_struct mdsp_IR_data[MDSP_IR_DATA_SIZE];

/* The modulo for wrapping the BSN number, max BSN is 2047 */
#define MDSP_IR_BSN_MOD 2048

#endif

/* Tag coex params against scheduled monitors */
typedef struct
{
   uint32 coex_desense_id;
   uint32 coex_priority;
   uint32 coex_skip;
} mdsp_gprs_monitor_coex_params;

/****************************************************************************
 ****************************************************************************
                       mDSP   GPRS  COMMAND   STRUCTURES


 These strutures are used to communicate individual commands from the
 host to the mDSP.  Multiple commands can be stacked back to back.


 ****************************************************************************
 ***************************************************************************/

/* GPRS Power Measure command substruct */
typedef PACKED struct PACKED_POST
{
  uint16                    pdch;           /* pdch ID */
  uint16                    offset;         /* time offset to start of burst in qs   */
  uint16                    grfc_buffer;    /* ptr to buffer of grfc cmds to execute */
  uint16                    sbi_buffer;     /* ptr to buffer of sbi cmds to execute  */
} gprs_power_measure_cmd_substruct;


/* GPRS Power Measure command struct */
typedef PACKED struct PACKED_POST
{
  uint16                    cmd;            /* command code                   */
  uint16                    control_field;  /* dsp or host scheduled, length  */
  uint16                    metrics_buffer; /* ptr to buffer to store results */
  uint16                    nbursts;        /* number of bursts to perform measurement over */
  gprs_power_measure_cmd_substruct ctrl[MAX_GPRS_RSSI_MEASUREMENTS]; /* control structure for the bursts */
} gprs_power_measure_cmd_struct;



/****************************************************************************
 ****************************************************************************
                            FUNCTION PROTOTYPES
 ****************************************************************************
 ***************************************************************************/
boolean mdsp_gprs_get_rx_burst_schedule (GfwSchedulerResultStruct *data , gas_id_t gas_id );
boolean mdsp_gprs_get_rx_usfs (GfwUsfBufStruct *data , gas_id_t gas_id );
uint16  mdsp_gprs_get_tx_burst_schedule ( gas_id_t gas_id );

void mdsp_gprs_set_usf_list(uint16  list[8] , gas_id_t gas_id );
boolean mdsp_gprs_get_tx_schedule (uint16 tag, boolean discard_untxed, gas_id_t gas_id );
void mdsp_gprs_convert_usf_to_pdch(uint16 tag, mdsp_timeslot_type pdch , gas_id_t gas_id );

void mdsp_notify_power_mon_info
(
   boolean data_valid,
   uint16 nozone_0_start,
   uint16 nozone_0_end,
   uint16 nozone_1_start,
   uint16 nozone_1_end,
   gas_id_t gas_id
);

void mdsp_gprs_power_measure
 (
   uint16       num,        /* number of measurements */
   uint16       offsets[],   /* which bursts to do     */
   mdsp_gprs_monitor_coex_params coex_params[], /* coex params */
   boolean reduced_length,
   ARFCN_T      *arfcns,
   gas_id_t     gas_id
);


void mdsp_gprs_interference_measure
 (
   uint16              num,       /* number of measurements */
   mdsp_timeslot_type  pdch_id[],  /* which pdchs */
   mdsp_gprs_monitor_coex_params coex_params[],
   ARFCN_T             *arfcns,
   gas_id_t             gas_id
);

void mdsp_gprs_power_measure_auto
 (
   uint16   num,      /* number of measurements */
   uint16   offset,    /* scheduling gap offset for mDSP */
   mdsp_gprs_monitor_coex_params coex_params[],
   ARFCN_T  *arfcns,
   gas_id_t gas_id
);

boolean mdsp_gprs_get_power_measure_results
 (
   uint32 *num_rssi,
   uint32 *rssi_values,
   boolean * yield_status,
   boolean last_attempt,
#ifdef GERAN_L1_HLLL_LNASTATE 
   uint16  *jdetvalues,
#endif /*GERAN_L1_HLLL_LNASTATE*/
   gas_id_t gas_id
 );
void mdsp_gprs_abort_power_measure ( gas_id_t gas_id );


void mdsp_gprs_set_medium_access_mode(mdsp_access_mode_type mode , gas_id_t gas_id );

boolean mdsp_gprs_is_power_measure_results_available ( gas_id_t gas_id );

#ifdef FEATURE_GSM_MDSP_EGPRS
/* Enable EGPRS mode */
void mdsp_gprs_set_egprs_mode(boolean egprs , gas_id_t gas_id );

#ifdef FEATURE_GSM_EGPRS_IR_WS_REDUCTION
uint16 mdsp_gprs_get_oper_win_size(gas_id_t gas_id);
#endif /* FEATURE_GSM_EGPRS_IR_WS_REDUCTION */

#ifdef FEATURE_GSM_MDSP_IR
/* Incremental Redundancy */
void mdsp_gprs_IR_enable(boolean enable , gas_id_t gas_id );
void mdsp_gprs_IR_clear_memory (uint16 starting_bsn, uint16 ending_bsn , gas_id_t gas_id );
void mdsp_gprs_IR_set_active_window(uint16 starting_bsn, uint16 ending_bsn , gas_id_t gas_id );
void mdsp_gprs_IR_set_tfi (uint16 tfi , gas_id_t gas_id );

#endif
#endif

/* enable ACI CCI detection in mdsp */
void mdsp_gprs_set_aci_detection(boolean enable , gas_id_t gas_id );

void mdsp_gprs_reset_rx_seq_nums( gas_id_t gas_id );
void mdsp_gprs_init( gas_id_t gas_id );
void mdsp_gprs_inact( gas_id_t gas_id );
void mdsp_gprs_process_frame_tick( gas_id_t gas_id );

/*===========================================================================
  MULTI-DATASPACED PUBLIC DATA
===========================================================================*/

typedef struct
{
#if defined (FEATURE_GSM_MDSP_IR) && !defined (FEATURE_GSM_QDSP6_FW_HAS_IR_MEM)

  /* Incremental redundancy data memory for Q4 MDSP targets*/
  mdsp_IR_data_struct mdsp_IR_data[MDSP_IR_DATA_SIZE];

  int16 mdsp_dm_pdtch_decodes;

#endif /* (FEATURE_GSM_MDSP_IR) && !defined (FEATURE_GSM_QDSP6_FW_HAS_IR_MEM)*/

#ifdef FEATURE_GSM_MDSP_EGPRS
  /* Flag to store EGPRS mode */
  boolean mdsp_egprs_mode;
#endif

  /* Flag indicating if ACI detection is enabled */
  boolean mdsp_aci_detection;

  /* USF list table */
  uint32 mdsp_usf_list[8];

} mdsp_gprs_public_data_t;

mdsp_gprs_public_data_t* get_mdsp_gprs_public_dataspace_ptr(const gas_id_t gas_id );

#endif /* MDSP_GPRS_H */
