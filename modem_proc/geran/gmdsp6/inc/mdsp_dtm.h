#ifndef MDSP_DTM_H
#define MDSP_DTM_H
/*===========================================================================
                          M D S P _ D T M . H


GENERAL DESCRIPTION
  This header file contains DTM definitions.  It should only be included
  via mdsp_gsm.h by external clients.


Copyright (c) 2005-2014 Qualcomm Technologies, Inc.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmdsp6/inc/mdsp_dtm.h#1 $
$DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when         who     what, where, why
----------  -------- ------------------------------------------------------
17/11/14     cs      CR701894 Enhanced Page Detection Support
07/11/14     pjr     CR752564 Calculate correct buffer index when retrieving AMR-PD metrics
27/10/14     am      CR646584 WLAN coex
19/08/14     pjr     CR711506 Support for AMR Partial Decode and DTX detection FR19667
01/10/14     df      CR611845 SBD interface updates for missed wakeup
09/09/14     jk      CR697308:GL1 changes to support HLin LLin sawless feature
12/09/14     nm      CR723570 GL1 changes for FR21036 - Read own PCH in transfer to BO2.0(to be leveraged for TH/JO)
29/08/14     cos     CR 716812 - Adding missing changes for imrd framework
07/08/14     cja     CR643648 Desense for channel adjacent to spur
11/02/14     jk      CR614254:GL1 changes to support IP2 calibration
11/07/14     pjr     CR 692177 Added support for RFACCH 2.0
18/06/14     cos     CR682031 - GL1 control algorithm and changes for supporting AEQ PHASE 4
07/04/14     cja     CR644445 Mainline feature FEATURE_GSM_ASYNC_FW_INTF
03/03/14     npt     CR622060 Rework wake-up
03/02/14     cos     CR475069 GL1 to indicate rxlev to firmware; merged on Bolt
19/12/13     jk      CR503359 - GL1 changes to support Rx diversity
22/11/13     cja     CR455527 Remove featurisation from files in API directory
17/09/13     pjr     CR538893 Changes to enable CXM logging
13/09/13     jk      CR413653:Set flag spurChanFlag for desense channel
29/07/13     sk      CR519663 Partial QBTA bringup changes
17/05/13     kb/npt  CR385548 - Decode PCH after sleep wakeup error recovery.
25/04/13     pjr     CR480105 dual dataspaced missing variables
13/03/13     mc      CR459487 : COEX Support - GERAN arbitrator framework
05/12/12     pg      CR427361: GFW/RF shared memory interface cleanup
18/05/12     ky      Async message router Interface Changes between SW & FW
26/03/12     jj      CR344695 rename feature name FEATURE_GSM_GPRS_MSC34 to FEATURE_GSM_GPRS_MSC33
26/03/12     jj      CR344695 rename feature name FEATURE_GSM_GPRS_MSC34 to FEATURE_GSM_GPRS_MSC33
29/09/11     jj      CR308254 GL1 Changes for GSM DC spur removal
08/09/11     jj      CR302378  Added GL1 changes for RSB along with spectral inversion
28/04/11     cja     Add Nikel freq correction by GFW
20/04/11     cja     Add spectral inversion bit (for Nikel)
27/01/11     cja     Changes for Nikel
19/10/09     ky      Removed Q6 based compiler warnings
05/08/09     ws      Removed FEATURE_GSM_MDSP_DTM it's now mainlined
29/05/09     ws      FEATURE_GSM_MDSP_AEQ phase 1 for all GMSK bursts
27/03/09     cs      MOB build Lint fixes
26/03/09     ws      Added PACKED_POST for QDSP6 targets
24/11/08     og      FEATURE_GSM_GPRS_MSC33 support.
30/10/08     ws      CR 162112 - ESAIC Only reset IIR filter on PCH and NCELL BCCH
26/08/08     og      Mustang GPLT changes.
19/03/08     agv     LCU RF Spur Interface Support.
24/12/07     nt      Added the GCF flag to the mDSP interface for async/sync_rx cmds
13/12/07     agv     CR 131982 Fix for EGPRS DM causing sleep nacks.
09/11/07     nf      Added FEATURE_GSM_R_SACCH
27/03/07     dv      Added FEATURE_GSM_R_FACCH
23/01/07     agv     Switch SAIC off if Idle Mode to reduce current draw.
02/11/06     agv     CR98176 - Added csprng seed for security util.
21/09/06     og      Reset only the cs part of lower layers when connecting DTM
                     assignment in packet transfer (EDTM route).
31/07/06     agv     CR85810 NB AB TSC, and TA fix.
03/23/06     ws      Added Support for SDA algorithm SAIC Disable Algorithm
16/03/06     og      Take timeslot gaps into account when deciding on Tta/Trb
                     or Ttb/Tra. Resolves CR 88935 and CR 88975.
09/30/05     gfr     Move ACI detection control to mDSP driver
08/05/05     gfr     Add extern for mdsp_aci_detection
04/21/05     gfr     Initial version
===========================================================================*/

#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif


#include "sys_type.h" /* dBx16_T */

#include "mdsp_gprs.h"

#include "gfw_sw_intf.h"
#include "msgr.h"
#include "geran_msg.h"

#ifdef FEATURE_GSM_RX_DIVERSITY
#include "gl1_hw.h"
#endif /*FEATURE_GSM_RX_DIVERSITY*/
/*===========================================================================

                     DEFINITIONS FOR MODULE

===========================================================================*/



#define MDSP_CHAN_MASK          0x00FF



typedef PACKED struct PACKED_POST
{
   uint16   len;
   uint16   seq_num;
   uint16   control[2];
   uint16   rx_qual_hard;
   uint16   rx_qual_soft;
   uint16   data_1[MAX_RX_DTM_DATA_PER_FRAME];
#ifdef FEATURE_GSM_MDSP_EGPRS
   uint16   egprs_hdr[MDSP_RX_EGPRS_HDR_SIZE];
   uint16   data_2[MAX_RX_DTM_DATA_PER_FRAME];
   uint16   reserved[5];
#endif
} mdsp_dtm_rx_struct;


typedef PACKED struct PACKED_POST
{
  uint16 len;
  uint16 control;
  uint16 data[MAX_TX_DTM_DATA_PER_FRAME];
} mdsp_dtm_tx_gsm_gprs_struct;

#ifdef FEATURE_GSM_MDSP_EGPRS
typedef PACKED struct PACKED_POST
{
  uint16 len;
  uint16 control;
  uint16 hdr[3];
  uint16 data_1[MAX_TX_DTM_DATA_PER_FRAME];
  uint16 data_2[MAX_TX_DTM_DATA_PER_FRAME];
} mdsp_dtm_tx_egprs_struct;
#endif

typedef union
{
#ifdef FEATURE_GSM_MDSP_EGPRS
   mdsp_dtm_tx_egprs_struct      egprs;
#endif
   mdsp_dtm_tx_gsm_gprs_struct   gsm_gprs;
} mdsp_dtm_tx_struct;



/* Transmit data structure for GSM packets */
typedef struct
{
   uint16  len[2];
   uint16 *data[2];
} mdsp_gsm_tx_data_type;

/* Structure used to pass data to write_tx_data function */
typedef union
{
   mdsp_gsm_tx_data_type   gsm;
   mdsp_gprs_tx_data_type  gprs;
#ifdef FEATURE_GSM_MDSP_EGPRS
   mdsp_egprs_tx_data_type egprs;
#endif
} mdsp_dtm_tx_msg_type;



/*===========================================================================

                             MACROS

===========================================================================*/

/* Macro to set the RX command control field values */
#define MDSP_DTM_RX_CTL_FIELD(field,value) \
   (uint16)((((uint16)(value)) << gsmSyncRxCmd##field##_POS) & gsmSyncRxCmd##field##_MASK)

/* Macros for parsing the control fields */
#define MDSP_PARSE_RX_HDR(hdr,field)       \
   (((hdr)->control[gsmSyncRxCmd##field##_IDX] & gsmSyncRxCmd##field##_MASK) >> gsmSyncRxCmd##field##_POS)
#define MDSP_GPRS_PARSE_RX_HDR(hdr,field)  MDSP_PARSE_RX_HDR(hdr,field)

/* Invalid tag value */
#define MDSP_INVALID_TAG (0xFFFF)



/*===========================================================================

                           GLOBAL VARIABLES

===========================================================================*/



/*===========================================================================

                           FUNCTION PROTOTYPES

===========================================================================*/
typedef struct
{
   uint32  coex_priority;
   uint32  coex_desense_id;
} mdsp_dtm_coex_params_type;

/* ---------- Receive control functions ---------- */
typedef struct
{
   mdsp_channel_type         channel_type;
   mdsp_timeslot_type        timeslot;
   boolean                   decode;
   boolean                   early_decoding;
   uint8                     burst_num;
   boolean                   force_facch;
   boolean                   look_for_facch;
   uint16                    arfcn;
#ifdef FEATURE_GSM_AMR
   uint8                     amr_rx_phase;
#endif
   boolean                   look_for_rfacch;
   uint8                     rfacch_fw_buffer_index;
   boolean                   look_for_rsacch;
   boolean                   sbd_enabled;
   boolean                   epd_allowed;
   mdsp_dtm_coex_params_type coex_params;
#ifdef FEATURE_GSM_RX_DIVERSITY
   GfwRxdControlType         rxdiv_control;
#ifndef FEATURE_GSM_RX_DIVERSITY_IDLE
   boolean                   is_cs_channel;
#else
   gl1_rxd_channel_type      is_cs_channel;
#endif /* FEATURE_GSM_RX_DIVERSITY_IDLE */
#endif

#ifdef FEATURE_GSM_AMR_PD_DTX_DETECT
   boolean                   amr_partial_decode;
   boolean                   dtx_detected;
#endif
} mdsp_dtm_rx_slot_params_type;


typedef struct
{
   uint8                         range;   /* SDA RSSI changes */
   dBx16_T                      valx16;   /* SDA RSSI changes */
} mdsp_rf_gain_type;                      /* SDA RSSI changes */



#define mdsp_rf_desense_chan_type GfwRfDesenseChanType

typedef struct
{
   boolean                       saic;            /* SAIC On/Off      */
   boolean                       idle;            /* IDLE MODE TRUE/FALSE */
   boolean                       dedicated;       /* Dedicated Mode True/false */
   uint8                         tsc;
   mdsp_rf_gain_type             rf_gain;         /* SDA RSSI changes */
#ifdef FEATURE_GSM_RX_DIVERSITY
   mdsp_rf_gain_type       rf_gain_divrx; /* SDA RSSI changes */
#endif
   int16                         rx_lev_dBm;
   uint8                         num_slots;
   uint8                         num_of_page_bursts_missed;

#ifdef  FEATURE_GSM_GPRS_MSC33
   mdsp_dtm_rx_slot_params_type  slot_params[MDSP_MAX_DL_TS];
#else
   mdsp_dtm_rx_slot_params_type  slot_params[4];
#endif
   boolean                       gl1_hw_gprs_anite_gcf;
   mdsp_rf_desense_chan_type     DesenseArfcn;
#ifdef FEATURE_AEQ_PHASE4
   boolean                        enableAeq4;
#endif /*FEATURE_AEQ_PHASE4*/
} mdsp_dtm_rx_params_type;

typedef struct
{
#ifdef  FEATURE_GSM_GPRS_MSC33
   uint16      data_tag[MDSP_MAX_DL_TS];
   uint16      metrics_tag[MDSP_MAX_DL_TS];
#else
   uint16      data_tag[4];
   uint16      metrics_tag[4];
#endif
} mdsp_dtm_rx_tags_type;

void mdsp_issue_async_msg(GfwAsyncCmd cmdType, gas_id_t gas_id);
void mdsp_wakeup_issue_async_msg(GfwAsyncCmd cmdType, gas_id_t gas_id, uint16 fn);

#ifdef FEATURE_GSM_MDSP_RAND_GEN
/*===========================================================================

FUNCTION mdsp_random_timer_cmd

DESCRIPTION
  This function tells the mDSP every 4 seconds to generate the random seed,
  and sets bit 15 of the mDSP control field.

DEPENDENCIES
  None

RETURN VALUE
  uint16         Switches bit 15 on /off.

SIDE EFFECTS
  None
===========================================================================*/
uint16 mdsp_random_timer_cmd( gas_id_t gas_id );

/*===========================================================================

FUNCTION mdsp_process_csprng_rnd_data

DESCRIPTION
  This function reads the 64bit Random seed from the mDSP and writes it to
  the security utility for it's processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mdsp_process_csprng_rnd_data(void);

#endif




void mdsp_dtm_rx_qdsp6
(
   mdsp_dtm_rx_params_type *params,
   mdsp_dtm_rx_tags_type *tags,
   int32 phase_incrmt,
   ARFCN_T arfcn,
   gas_id_t gas_id
);

boolean mdsp_dtm_get_rx_data
 (
   uint16                  tag,
   GfwDecodedDataStruct      *hdr,
   uint16                  *data[],
   boolean                 last_attempt
   , gas_id_t gas_id
 );

boolean mdsp_dtm_get_rx_burst_metrics
 (
   uint16                    tag,
   GfwBurstMetricsStruct *metrics,
   boolean                   get_hard_decisions
   , gas_id_t gas_id
 );

#ifdef FEATURE_GSM_AMR_PD_DTX_DETECT
boolean mdsp_gprs_get_dtx_amr_pd_metrics
 (
   uint16             tag,
   GfwDtxAmrPdMetrics *data,
   gas_id_t           gas_id
 );
#endif

void mdsp_dtm_abort_rx (uint16 data_tag, gas_id_t gas_id);
void mdsp_dtm_reset_rx (gas_id_t gas_id);

#ifdef  FEATURE_GSM_DTM
#ifdef  FEATURE_GSM_EDTM

void  mdsp_dtm_reset_rx_cs_only(gas_id_t gas_id);

#endif
#endif


#ifdef FEATURE_GSM_MDSP_IR
/* Internal debug function */
void mdsp_check_IR_data (GfwDecodedDataStruct *hdr);
#endif


/* ---------- Transmit control functions ---------- */
boolean mdsp_dtm_write_tx_data
 (
   mdsp_channel_type         chan,
   mdsp_dtm_tx_msg_type      *msg,
   mdsp_specific_msg_type    msg_type,
   mdsp_timeslot_type        timeslot,
   boolean                   auto_clear,
   uint16                    *tag
   , gas_id_t gas_id
 );

/* Parameters for a transmit command */
typedef struct
{
   uint8                     tsc;              /* training sequence  */
   uint8                     ab_tsc;           /* AB 41bit sequence  */
   uint16                    timing_advance;   /* timing in symbols  */
   uint16                    tx_alpha;         /* tx alpha value     */
   uint8                     num_msgs;         /* number of messages */
   uint16                    data_tags[MDSP_MAX_UL_TS]; /* data tags */
   uint8                     num_slots;        /* num of bursts      */
   mdsp_timeslot_type        timeslots[MDSP_MAX_UL_TS]; /* timeslots */
   boolean                   high_prio_ctrl[MDSP_MAX_UL_TS]; // for rrbp_hi messages.
#ifdef FEATURE_GSM_MDSP_EGPRS
   boolean                   loopback;         /* srb loopback       */
#endif
   uint8                     num_ul_timeslots;
   int32                     f_hz_q6;
   uint32                    inv_f_dl_lo;
   uint8                     burst_num;

   mdsp_dtm_coex_params_type coex_params[MDSP_MAX_UL_TS]; /* coex params */

} mdsp_dtm_tx_params_type;

void mdsp_dtm_tx (mdsp_dtm_tx_params_type *params, ARFCN_T arfcn, uint16 pa_index[], gas_id_t gas_id );
boolean mdsp_dtm_get_tx_schedule (uint16 tag, boolean discard_untxed , gas_id_t gas_id );
boolean mdsp_dtm_abort_tx (uint16 tag , gas_id_t gas_id );

void mdsp_log_dtm_tx_trace_buf
 (
   mdsp_channel_type      chan,
   mdsp_dtm_tx_msg_type  *msg,
   mdsp_specific_msg_type msg_type,
   mdsp_timeslot_type     pdch_id
   , gas_id_t gas_id
 );


/* ---------- Miscellaneous functions ---------- */
void mdsp_dtm_init ( gas_id_t gas_id );
void mdsp_dtm_process_frame_tick ( gas_id_t gas_id );

#ifdef GERAN_L1_IP2CAL
void mdsp_schedule_ip2cal(uint16 Offset,uint8 numIp2CalMeas,uint32 ip2CalStepSizeQS,
                          gas_id_t gas_id);
#endif /*GERAN_L1_IP2CAL*/

#endif  /* MDSP_DTM_H */

