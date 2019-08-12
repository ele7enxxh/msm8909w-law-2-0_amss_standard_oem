#ifndef GL1_DEFS_H
#define GL1_DEFS_H

/*========================================================================
                 GSM GPRS NPL1 DEFINITIONS FILE
DESCRIPTION
   This file contains definitions used by both the GSM/GPRS NPL1
   Message Layer and the Frame layer.

Copyright (c) 2001-2014 Qualcomm Technologies, Inc.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
========================================================================== */

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/gl1_defs.h#1 $
$DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
16/10/15    pg      CR924813 Enable LIF in FTM Cal for Rx only devices
09/10/15    jk      CR89990:Enable LIF by default for FTM Cal mode
17/03/15    xz      CR807490 solve the address word aligment issue for scheduled_status of SCH
16/12/14    jk      CR753423:LIF for Idle,voice and data
17/11/14    cs      CR701894 Enhanced Page Detection Support
19/08/14    pjr     CR711506 Support for AMR Partial Decode and DTX detection FR19667
01/10/14    df      CR611845 SBD interface updates for missed wakeup
06/06/14    jk      CR668058:Adding EPD Log pakcet
09/09/14    jk      CR697308:GL1 changes to support HLin LLin sawless feature
23/07/14    pjr     CR682764 TP IR Feature - implement IR mem Full handling from GFW to GRLC
11/07/14    pjr     CR 692177 Added support for RFACCH 2.0
24/07/14    jk      CR699271:GL1 LIF changes for RxD
10/07/14    jk      CR688587:GL1 changes to support LIF feature
02/01/14    jk      CR595130: GL1 changes to support VAMOS-II
03/01/14    jk      CR572347 - GL1 FTM API changes to support Rx diversity
14/10/13    pa      CR539505:Bolt: Added support for Dynamic Power Scheduling
24/09/13    sp      CR482765: Enhance Edge log packet reporting
24/09/13    sp      CR449651 - Report brstCancel information in Burst Metrics Logging.
15/08/13    sk      CR524039 GBTA bringup changes
24/07/13    jk      CR464982:Expand support of ciphering
06/06/13    mc      CR492352 : COEX Support : Per-Slot Channel Indication (for RRBP) + COEX params for monitors
17/05/13    kb/npt  CR385548 - Decode PCH after sleep wakeup error recovery.
28/03/13    jk      CR452710: Changes to support continuous IQ logging and GSM XO calibration
13/03/13    mc      CR.459487 : COEX Support - GERAN arbitrator framework
16/10/12    ws      CR409719 - DIME/TRITON FTM API compatability support
10-04-01   tjw      Add customer.h and comdef.h to all SU API files
29/05/09    ws      Removed Featurisation from GL1 API for CMI compatability
24/11/08    og      FEATURE MSC34 changes.
15/01/07    ws      Added suport for ESIAC logging and SAIC AFC in PTM
09/11/07    nf      Added FEATURE_GSM_R_SACCH
30/10/07    og      Added support for FTM BER calculation in loopback A, B, C
                    and SRB.
03/08/07    agv     CR123023 - Combined fix for AFC/TT/AGC.
19/07/07   og       UOOS changes, reduce number of measured samples.CR119147.
03/04/07    dv      Added FEATURE_GSM_R_FACCH
31/07/06    agv     CR85810 NB AB TSC, and TA fix.
21/07/06    agv     Removed SAIC_GMSK from gl1_defs_modulation_type.
17/05/06    ws      Corrected gl1_defs_modulation_type structure
06/04/06    agv     Feature SAIC MIMO elements added to the burst metrics.
02/17/06    gfr     Added modulation for power backoff to RF Driver.
26/01/06    og      Additions to allow TA to be included in NCELL ACQ offset
                    calculations.
10/07/05    gfr     Add ACI detection metrics to the burst metrics
08/31/05    gfr     Replace gl1_msg_metrics_rpt with gl1_defs_metrics_rpt
07/29/05    gfr     DTM support
06/21/05    gfr     Add camp_tn to the GPRS metrics and data callback structs
06/01/05    gfr     Add support for narrow ACI filter
04/26/05    gfr     Handle invalid rx metrics gracefully
04/11/05    gfr     Remove obselete comments
03/09/05    yh      Add bep in header struct.
11/08/04    gfr     BEP is unsigned.
11/17/04    yhong   Add offset_to_sob and offset_to_burst into data and metric
09/02/04    gfr     Added ACI fields to burst metrics.
08/18/04    gfr     Fixed puncturing scheme mismatch.
08/17/04    gsc     Added support for Loopback I.
07/22/04    gfr     EGPRS support.
05/20/04    gfr     Mainlined FEATURE_COMMON_PWR_HANDLER
02/17/04    gfr     Timing offset is signed.
01/12/04    gfr     Support for 6 monitors per frame.
01/07/04    bm      Mainlined RF_MULTIMODE
11/05/03    gfr     Support for quad-band.
08/11/03    gsc     Added c_over_i to gl1_defs_metrics_rpt under FEATURE_GSM_AMR.
05/13/03    gfr     Common power handler support.
05/07/03    gsc     Added AMR support
02/26/03    dp      Added half rate support featurized under
                    FEATURE_GSM_HALF_RATE
02/24/03    gfr     Lint cleanup.
01/13/03    mk      Added soft rx qual to gl1_defs_rx_hdr_struct.
11/15/02    gfr     Store the arfcn, gain range and power status in the GPRS
                    metrics structure.
10/11/02     ih     Removed circular dependency between HW and RF
09/12/02     sd     Added support for RF driver multimode API that includes
                    GSM drivers.
09/09/02     gr     Added coding scheme based msg length lut.
07/11/02     gw     Changed field power_dBm_x16 in metrics report to
                    pwr_dBm_x16.
06/28/02     bk     Changed the signature of GPRSlog callback
06/28/02    thh     Featurize this whole module under FEATURE_GSM.  This is
                    required to allow dualmode build to make single mode
                    (WCDMA) executable.
04/19/02     JC     Added rx_gain_range to gl1_defs_metrics_rpt.
04/16/02     bk     Added enum for GPRS med. acc. mode
03/29/02     dp     Merged Gabe's BFI changes
03/14/02     bk     Changed GPRS monitor struct definitions.
03/05/02     JC     Added dBx16_T and rf_measurement_T to
                    gl1_defs_metrics_rpt for AGC on normal burst.
02/15/02     mk     Afc/tt update.
02/13/02     jc     Reversed order of enums in gl1_defs_cipher_mode to match RR
02/09/02     bk     Changed format of gprs rssi and moved gprs meas struct
02/01/02     mk     Added snr to rx metrics struct.
01/15/02     bk     Reversed order of enums in gl1_defs_coding_scheme_type
12/18/01     dp     Reversed order of enums in gl1_defs_cipher_mode
12/12/01     bk     Added se num to GPRS metrics info
11/29/01     bk     Reordered the GPRS msg layer definitions
11/19/01     jc     Added loopback definitions.
11/9/01      bk     Added GPRS support
10/31/01     dp     Moved definition of rx_hdr_struct here since its common to msg and
                    frame layers
09/25/01     gw     Added metrics report.
08/31/01     jc     Initial version.
===========================================================================*/
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif


#include "sys_type.h"
#include "rfcom.h"
#include "gfw_sw_intf.h"
#include "gprsdef.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#define GL1_DEFS_SLOTS_IN_FRAME       8

#define GL1_DEFS_FRAMES_IN_BLK_PERIOD 4

/* GL1_DEFS_MAX_DL_MSGS may be 4 or 5, define the maximum for GERAN clients
   to use for array sizing */
#define GERAN_API_MAX_GL1_DEFS_MAX_DL_MSGS  5
#define GERAN_API_MAX_GL1_DEFS_MAX_UL_MSGS  4
/* and an access function to give the value in use */
extern uint32 geran_api_GL1_DEFS_MAX_DL_MSGS(void);
#define GL1_DEFS_MAX_PWR_MEAS       120
#define GL1_DEFS_MAX_PWR_MEAS_GPRS   32

#define GL1_DEFS_ACCESS_BYTES     1
#define GL1_DEFS_EXT_ACCESS_BYTES 2
#define GL1_DEFS_CS1_BYTES       23
#define GL1_DEFS_CS2_BYTES       34
#define GL1_DEFS_CS3_BYTES       40
#define GL1_DEFS_CS4_BYTES       54
#define GL1_DEFS_MCS1_BYTES      23  /* 178 bits */
#define GL1_DEFS_MCS2_BYTES      29  /* 226 bits */
#define GL1_DEFS_MCS3_BYTES      38  /* 298 bits */
#define GL1_DEFS_MCS4_BYTES      45  /* 354 bits */
#define GL1_DEFS_MCS5_BYTES      57  /* 450 bits */
#define GL1_DEFS_MCS6_BYTES      75  /* 594 bits */
#define GL1_DEFS_MCS7_BYTES      57  /* 450 bits */
#define GL1_DEFS_MCS8_BYTES      69  /* 546 bits */
#define GL1_DEFS_MCS9_BYTES      75  /* 594 bits */
#define GL1_DEFS_MAX_DATA        GL1_DEFS_MCS9_BYTES


/* Basic GSM definitions */
#define QS_PER_SLOT   625
#define QS_PER_FRAME 5000

/* Access Burst Sync Seq Bits format as per 3GPP45.002, Clause 5.2.7.*/
#define TS0           0
/* -----------------------------------------------------------------------
**  GPRS Type Declarations
** ----------------------------------------------------------------------- */

typedef enum
{
   GL1_HW_INIT_INACTIVE = 0,
   GL1_HW_INIT_RESUME,
   GL1_HW_INIT_ACTIVE_RAT,
   /* The following are for FTM use only */
   GL1_HW_INIT_RX_ONLY,    // Rx device with no associated Tx device
   GL1_HW_INIT_RX_TX,      // Rx device with an associated Tx device
   GL1_HW_INIT_NON_SIGNALLING // to be deprecated - do not use!!
} gl1_init_rflm_type;

 typedef enum {
  GL1_MSG_DCCH                 = 0,
  GL1_MSG_BCCH                 = 1,
  GL1_MSG_RACH                 = 2,
  GL1_MSG_CCCH                 = 3,
  GL1_MSG_SACCH                = 4,
  GL1_MSG_SDCCH                = 5,
  GL1_MSG_FACCH                = 6,
  GL1_MSG_TCH                  = 7,
  GL1_MSG_TCH_F96              = 8,
  GL1_MSG_TCH_F144             = 9,
  GL1_MSG_TCH_HS               = 10,
  GL1_MSG_TCH_AFS              = 11,
  GL1_MSG_TCH_AHS              = 12,
  GL1_MSG_RATSCCH_FS           = 13,
  GL1_MSG_RATSCCH_HS           = 14,
  GL1_MSG_SID_FIRST_FS         = 15,
  GL1_MSG_SID_UPDATE_FS        = 16,
  GL1_MSG_SID_FIRST_HS         = 17,
  GL1_MSG_SID_UPDATE_HS        = 18,
  GL1_MSG_FACCH_HS             = 19,
  GL1_MSG_FACCH_AFS            = 20,
  GL1_MSG_FACCH_AHS            = 21,
  GL1_MSG_SID_FIRST_INH        = 22,
  GL1_MSG_SID_UPDATE_INH       = 23,
  GL1_MSG_RATSCCH_MARKER       = 24
  ,GL1_MSG_CBCH                = 25
  ,GL1_MSG_ONSET               = 26
  ,GL1_MSG_TCH_WFS             = 27,
  GL1_MSG_FACCH_WFS            = 28,
  GL1_MSG_RATSCCH_WFS          = 29
  ,GL1_INVALID                 = 255
}  gl1_msg_chan_type;

typedef enum
{
   GL1_HW_TCH_FS,
   GL1_HW_TCH_EFS,
   GL1_HW_TCH_F96,
   GL1_HW_TCH_F144,
   GL1_HW_SACCH,
   GL1_HW_SDCCH,
   GL1_HW_BCCH,
   GL1_HW_PCH,
   GL1_HW_FACCH,
   GL1_HW_SCH,
   GL1_HW_RACH,
   GL1_HW_TCH_F96_AND_FACCH,
   GL1_HW_TCH_F144_AND_FACCH,
   GL1_HW_TCH_HS,
   GL1_HW_FACCH_HS,
   GL1_HW_TCH_AFS,
   GL1_HW_TCH_AHS,
   GL1_HW_FACCH_AFS,
   GL1_HW_FACCH_AHS,
   GL1_HW_TCH_WFS,
   GL1_HW_FACCH_WFS,
   GL1_HW_PTCCHD,
   GL1_HW_PRACH_ACC,
   GL1_HW_PTCCHU_ACC,
   GL1_HW_PDTCHU_CS1,
   GL1_HW_PDTCHU_CS2,
   GL1_HW_PDTCHU_CS3,
   GL1_HW_PDTCHU_CS4,
   GL1_HW_PDTCHD,
   GL1_HW_PACCHU,
   GL1_HW_PRACH_EXT,
   GL1_HW_PTCCHU_EXT,
   GL1_HW_PACCHU_EXT,
   GL1_HW_PBCCH,
   GL1_HW_PCCCH,
   GL1_HW_PDTCHU_MCS1,
   GL1_HW_PDTCHU_MCS2,
   GL1_HW_PDTCHU_MCS3,
   GL1_HW_PDTCHU_MCS4,
   GL1_HW_PDTCHU_MCS5,
   GL1_HW_PDTCHU_MCS6,
   GL1_HW_PDTCHU_MCS7,
   GL1_HW_PDTCHU_MCS8,
   GL1_HW_PDTCHU_MCS9,
   GL1_HW_RRBP_HI,
   GL1_HW_RRBP_LO,
   GL1_HW_NUM_CHANNEL_TYPES
} gl1_hw_channel_type;

/* enum specifying the allowable timeslots */
typedef enum
{
    GL1_DEFS_TN_0,
    GL1_DEFS_TN_1,
    GL1_DEFS_TN_2,
    GL1_DEFS_TN_3,
    GL1_DEFS_TN_4,
    GL1_DEFS_TN_5,
    GL1_DEFS_TN_6,
    GL1_DEFS_TN_7
} gl1_defs_tn_type;

/* enum specifyingthe allowable USFs to be used with PACCH/PDTCH */
typedef enum
{
   GL1_DEFS_USF_0,
   GL1_DEFS_USF_1,
   GL1_DEFS_USF_2,
   GL1_DEFS_USF_3,
   GL1_DEFS_USF_4,
   GL1_DEFS_USF_5,
   GL1_DEFS_USF_6,
   GL1_DEFS_USF_7,
   GL1_DEFS_USF_FREE = GL1_DEFS_USF_7,
   GL1_DEFS_PDCH_ALLOCATED,
   GL1_DEFS_PDCH_UNALLOCATED
} gl1_defs_usf_type;

/* enum specifying the medium access mode */

/* Do not change the order or values of the enums */
typedef enum
{
  GL1_DEFS_FIXED_ALLOC       = 0,
  GL1_DEFS_DYNAMIC_ALLOC     = 1,
  GL1_DEFS_EXT_DYNAMIC_ALLOC = 2
} gl1_defs_access_mode_type;


/* enum sepcifying the allowable coding schemes */
/* IMPORTANT - Do not change the order of these enums */
typedef enum
{
   GL1_DEFS_CS1_CODING,
   GL1_DEFS_CS2_CODING,
   GL1_DEFS_CS3_CODING,
   GL1_DEFS_CS4_CODING,
   GL1_DEFS_ACCESS_CODING,
   GL1_DEFS_EXT_ACCESS_CODING,
   GL1_DEFS_MCS1_CODING,
   GL1_DEFS_MCS2_CODING,
   GL1_DEFS_MCS3_CODING,
   GL1_DEFS_MCS4_CODING,
   GL1_DEFS_MCS5_CODING,
   GL1_DEFS_MCS6_CODING,
   GL1_DEFS_MCS7_CODING,
   GL1_DEFS_MCS8_CODING,
   GL1_DEFS_MCS9_CODING
} gl1_defs_coding_type;


typedef enum
{
   GL1_DEFS_MODULATION_GMSK,
   GL1_DEFS_MODULATION_8PSK,
   /* The UNKNOWN def is used for certain
      uplink cases when we cannot determine
      the modulation for a particular timeslot
      until USF is decoded.
   */
   GL1_DEFS_MODULATION_UNKNOWN

} gl1_defs_modulation_type;


/* Adjacent channel interference filters */
typedef enum
{
   GL1_DEFS_ACI_FILTER_CENTER,
   GL1_DEFS_ACI_FILTER_HIGH,
   GL1_DEFS_ACI_FILTER_LOW,
   GL1_DEFS_ACI_FILTER_NARROW
} gl1_defs_aci_filter_type;


/* The puncturing scheme used for uplink */
typedef enum
{
  GL1_DEFS_PUNCTURING_1,
  GL1_DEFS_PUNCTURING_2,
  GL1_DEFS_PUNCTURING_3
} gl1_defs_puncturing_type;

/* Enumerated type for acq afc, needs to align with the mdsp_acq_afc_type */
typedef enum
{
   GL1_HW_ACQ_AFC_DISABLED,
   GL1_HW_ACQ_AFC_ON_PDM,
   GL1_HW_ACQ_AFC_ON_ROTATOR,
   GL1_HW_ACQ_GSM_XOCAL
} gl1_hw_acq_afc_type;

typedef enum
{
    GL1_HW_USF,
    GL1_HW_PDCH,
    GL1_HW_LOOPBACK
} gl1_hw_pkt_msg_type;

typedef struct
{
   rf_gain_T gain;
   uint16    data_tag;
   uint16    metrics_tag;
   /* This field is introduced for holding the gain for diversity chain */
   rf_gain_T gain_divrx;
   /*This filed holds the HLin/Llin LNA state used for receiving the burst*/
   uint16    sawless_lnastate;
} gl1_hw_rx_tags_type;

/* Tags for tracking data between msg and hw layers */
typedef struct
{
   uint16   data_tag;
} gl1_hw_tx_tags_type;


typedef struct
{
   ARFCN_T              arfcn;
   gl1_hw_channel_type  channel_type;
   uint8                tsc;
   /* Added for AB TSC sync sequence bits. */
   uint8                ab_tsc;
   uint8                bsic;
   uint8                timing_advance;
   boolean              encode_flag;
   uint8                burst_num;
} gl1_hw_tx_burst_type;


typedef struct
{
   ARFCN_T               arfcn;
   gl1_hw_channel_type   channel_type;
   uint8                 tsc;
   /* Added for AB TSC Sync Sequence Bits. */
   uint8                 ab_tsc;
   uint8                 bsic;
   uint8                 timing_advance;
   boolean               encode_flag;
   boolean               loopback_flag;
   uint8                 burst_num;
} gl1_hw_tx_pkt_burst_type;

/* Macro to check if a coding scheme is EGPRS or not */
#define IS_EGPRS_CODING(c)    (c >= GL1_DEFS_MCS1_CODING && c <= GL1_DEFS_MCS9_CODING)

/* Macro to check if a coding scheme is 8PSK modulation or not */
#define IS_8PSK_MODULATION(c) (c >= GL1_DEFS_MCS5_CODING && c <= GL1_DEFS_MCS9_CODING)

#define GL1_DEFS_MSG_PER_SLOT 2

typedef struct
{
  boolean hdr_crc_passed;   /* Header CRC */
  uint16  hdr[3];           /* Three words of the header */
  uint16  bep[4];           /* BEP for the four bursts (in Q16) */
  boolean IR_used[GL1_DEFS_MSG_PER_SLOT];    /* If incremental redundancy used for decoding */
  boolean IR_written[GL1_DEFS_MSG_PER_SLOT]; /* If IR data was written for the msgs */
  boolean IR_mem_full[GL1_DEFS_MSG_PER_SLOT]; /* if IR memory is full in GFW*/
  GfwEnhancedEdgeLogPacket enhEdgeLogPkt;
} gl1_defs_rx_pkt_egprs_type;

/* Structure contain per-msg data */
typedef struct
{
  uint8        msg_len;         /* Message length */
  boolean      crc_passed;      /* If the CRC passed */
  boolean      firecode_used;   /* If firecode was used to correct errors */
  uint16       rx_qual;         /* RX quality report */
  uint8        data[GL1_DEFS_MAX_DATA];
} gl1_defs_rx_pkt_msg_type;


/* Structure specifying the format of the rx pkt msg header and decoded data */
typedef struct
{
  uint8                      offset_to_sob;
  gl1_defs_tn_type           tn;             /* Timeslot msg was rxed on */
  gl1_defs_tn_type           camp_tn;        /* Camped timeslot during rx */
  gl1_defs_coding_type       coding_scheme;  /* Coding scheme */
  gl1_defs_rx_pkt_msg_type   msg[GL1_DEFS_MSG_PER_SLOT];
  gl1_defs_rx_pkt_egprs_type egprs;          /* EGPRS specific information */
} gl1_defs_rx_pkt_data_type;

typedef struct
{
   ARFCN_T               arfcn;
   uint8                 tsc;
   gl1_hw_channel_type   channel_type;
   boolean               decode_flag;
   uint8                 burst_num;
   uint16                search_width;
} gl1_hw_rx_pkt_burst_type;

#define GL1_DEFS_MAX_HARD_DECISIONS 22
/* structure specifying the format of the rx pkt metrics for a timeslot */
typedef struct
{
    ARFCN_T                  arfcn;
    gl1_defs_tn_type         tn;
    gl1_defs_tn_type         camp_tn;
    uint8                    offset_to_burst;
    uint16                   schedule_status;
    gl1_defs_usf_type        usf;
    boolean                  valid;

    /* The following entries are only set if the valid flag is TRUE */
    dBx16_T                  pwr_dBm_x16;
    uint32                   rssi;
    int16                    dc_offset_i;
    int16                    dc_offset_q;
    int16                    freq_offset;
    int16                    timing_offset;
    uint16                   snr;
    rf_measurement_T         power_status;
    rf_gain_range_T          gain_range;
    gl1_defs_modulation_type modulation;
    uint16                   aci_pwr_low;
    uint16                   aci_pwr_center;
    uint16                   aci_pwr_high;
    gl1_defs_aci_filter_type aci_filter_used;
    uint16                   brstCancel;
    uint32                   reliabFactorQ16;

    /* Hard decisions are used internally for SRB loopback */
    uint16                   hard_decisions[GL1_DEFS_MAX_HARD_DECISIONS];
    /* SAIC MIMO metrics additions. */
    uint16                   pre_mimo_trained_complete_snr;
    uint16                   post_mimo_trained_complete_snr;
    uint16                   misc_flags;
    /* Metrics for diversity chain */
    uint32                   rssi_divrx;
    dBx16_T                  pwr_dBm_x16_divrx;
    rf_measurement_T         powerStatus_divrx;
    rf_gain_range_T          rx_gain_range_divrx;
    int16                    dc_offset_i_divrx;
    int16                    dc_offset_q_divrx;
    /*GL1 gets the jammer detector from FW and HLin/LLin state used for receiving
     *the burst from RF and GL1 uses these for logging purposes*/
    uint16                   jdet_value;
    uint16                   SawlessHLLLState;
    uint32                   WBEE;
    uint32                   WBEE_DRx;
    int32                    lif_offset_freq;
    int32                    curLifFreqKhz;
    boolean                  aci400Detected;
    int32                    accISqr;
    int32                    accQSqr;
    int32                    accIQ;
    int32                    wbdcEstI_PRx; // Wbdc Estimation I Coeff
    int32                    wbdcEstQ_PRx; // Wbdc Estimation Q Coeff
    int32                    wbdcEstI_DRx; // Wbdc Estimation I Coeff
    int32                    wbdcEstQ_DRx; // Wbdc Estimation Q Coeff
    int32                    rsbImgAciMetric;
} gl1_defs_rx_pkt_metrics_type;

/* The schedule status value that indicates success.  Any other value
   indicates the reason the burst was not scheduled */
#define GL1_DEFS_RX_SCHEDULED  0x8000

/* Transmit data structure for EGPRS packets */
typedef struct
{
   uint8                    *hdr;
   uint8                    *data[2];
   gl1_defs_puncturing_type puncturing[2];
} gl1_defs_tx_egprs_data_type;

/* Transmit data structure for GPRS packets */
typedef struct
{
   uint8 *data;
} gl1_defs_tx_gprs_data_type;

/* Structure used to pass data to write_tx_data function */
typedef union
{
  gl1_defs_tx_gprs_data_type  gprs;
  gl1_defs_tx_egprs_data_type egprs;
} gl1_defs_tx_pkt_data_type;

/* Struct specifying the USF information passed */
typedef struct
{
   gl1_defs_usf_type  usf;
   gl1_defs_tn_type   tn;
} gl1_defs_rx_pkt_alloc_type;

/* structure containing data, metrics and log memory info */
typedef struct
{
   gl1_defs_rx_pkt_data_type    *msg_data_mem;
   gl1_defs_rx_pkt_metrics_type *metrics_data_mem[ GL1_DEFS_FRAMES_IN_BLK_PERIOD ];
} gl1_defs_rx_pkt_mem_type;

/* This structure is used to define the type of timeslot passed.
 * i.e. a receive or transmit.
 */
typedef struct
{
  gl1_defs_tn_type  tn;
  boolean           transmit;
} gl1_defs_tn_struct;

typedef struct
{
   gl1_defs_tn_type     tn;
   int16                tx_power_index;
   /* Modulation needed for power backoff. */
   gl1_defs_modulation_type modulation;
   gl1_hw_channel_type  channel_type;
   geran_pdu_priority_t priority;
} gl1_hw_tx_signal_type;

typedef struct
{
   gl1_defs_tn_type     tn;
   int16                rx_signal_strength;
   boolean              valid;
   uint8                lna_range;  /* Only use in FTM mode or calibration */
   gl1_hw_channel_type  channel_type;
   /* Metrics for diversity chain */
   int16                rx_signal_strength_divrx;
   uint8                lna_range_divrx;  /* Only use in FTM mode or calibration for diversity Rx*/
} gl1_hw_rx_signal_type;
/* -----------------------------------------------------------------------
**  GSM Type Declarations
** ----------------------------------------------------------------------- */



typedef struct
{
   boolean       good_data;
   boolean       firecode_used;
   uint16        len;
   uint16        chan;  /* gl1_msg_chan_type */
   uint16        sid;
   uint16        voicing_mode;
   uint16        rx_qual;   /* hard rx qual */
   uint16        rx_qual_s; /* soft rx qual */
   uint16        bfi;       /* bad frame indicator */
   uint16        ufi;
   uint16        sfi;       /* speech frame indicator */
   uint16        onset;
   uint16        cmi;
   uint16        cmc;
   uint16        phase;
   uint16        bep[GL1_DEFS_FRAMES_IN_BLK_PERIOD];
   boolean       rfacch_store_status;
   boolean       rfacch_comb_status;
   boolean       rsacch_store_status;
   boolean       rsacch_comb_status;
   uint16        scheduled_status;
   boolean       vamospaired;
   GfwRfacchLogType rfacchLogBuff;
   GfwEpdLogPacket epdLogPacket;

   /* Metrics used for FEATURE_GSM_AMR_PD_DTX_DETECT */
   boolean       gfw_dtx_ind_dec;
   boolean       gfw_amr_pd_ind_dec;
   boolean       gfw_gl1_disable_amr_pd;
} gl1_defs_rx_hdr_struct;

typedef struct
{
   gl1_hw_channel_type  channel_type;
   ARFCN_T              arfcn;
   uint8                tsc;
   boolean              decode_flag;
   boolean              early_decoding;
   uint8                burst_num;
   uint16               search_width;
   boolean              force_facch;
   boolean              look_for_facch;
   uint8                amr_phase;
   boolean              look_for_rfacch;
   boolean              look_for_rsacch;
   uint8                rfacch_fw_buffer_index;
   uint8                num_of_page_bursts_missed;
   boolean              sbd_enabled;
   boolean              epd_allowed;

   /* Variables used for FEATURE_GSM_AMR_PD_DTX_DETECT */
   boolean              amr_partial_decode;
   boolean              dtx_detected;
} gl1_hw_rx_burst_type;

typedef enum
{
    A5_1,
    A5_2,
    A5_3,
    A5_4
} gl1_defs_cipher_mode;


typedef struct
{
   /* NPL1 metrics */
   boolean             valid;
   uint32              time_stamp;
   uint16              offset_to_sob;
   ARFCN_T             arfcn;
   boolean             own_paging_block;

   /* mDSP/RF metrics - only valid if the valid flag is set above */
   uint32              rssi;
   dBx16_T             pwr_dBm_x16;
   rf_measurement_T    powerStatus;
   rf_gain_range_T     rx_gain_range;
   int16               dc_offset_i;
   int16               dc_offset_q;
   int16               freq_offset;
   int16               timing_offset;
   uint16              snr;
   uint16              c_over_i;
   uint16                   aci_pwr_low;
   uint16                   aci_pwr_center;
   uint16                   aci_pwr_high;
   gl1_defs_aci_filter_type aci_filter_used;
   uint16                  pre_mimo_trained_complete_snr;
   uint16                  post_mimo_trained_complete_snr;
   uint16                  misc_flags;
   uint16                  brstCancel;
   uint32                  reliabFactorQ16;

//ifdef  F EATURE_FTM_GSM_BER
   /* This array is needed to supply FTM with the hard decision bits during
    * loopback C.
    * SRB loopback relies on the pkt metrics structure.
    * Loopback A and B use a different structure.
    */
   uint16  hard_decisions[GL1_DEFS_MAX_HARD_DECISIONS];
//endif
   /* Metrics for diversity chain */
   uint32                    rssi_divrx;
   dBx16_T                   pwr_dBm_x16_divrx;
   rf_measurement_T          powerStatus_divrx;
   rf_gain_range_T           rx_gain_range_divrx;
   int16                     dc_offset_i_divrx;
   int16                     dc_offset_q_divrx;
   uint32                    WBEE;
   uint32                    WBEE_DRx;
   int32                     lif_offset_freq;
   int32                     curLifFreqKhz;
   boolean                   aci400Detected;
   int32                     accISqr;
   int32                     accQSqr;
   int32                     accIQ;
   int32                     wbdcEstI_PRx; // Wbdc Estimation I Coeff
   int32                     wbdcEstQ_PRx; // Wbdc Estimation Q Coeff
   int32                     wbdcEstI_DRx; // Wbdc Estimation I Coeff
   int32                     wbdcEstQ_DRx; // Wbdc Estimation Q Coeff
   int32                     rsbImgAciMetric;
   uint16                    jdet_value;
   uint16                    SawlessHLLLState;
   boolean                   aqpsk_detected;

   /* Metrics used for FEATURE_GSM_AMR_PD_DTX_DETECT */
   uint8                     esaic_metrics;
   boolean                   dtx_cxm_reg;
   boolean                   amr_pd_cxm_reg;
   boolean                   dtx_rx_cancel;
   boolean                   amr_pd_rx_cancel;
   boolean                   amr_pd_ind;
   boolean                   amr_pd_desense;
} gl1_defs_metrics_rpt;

/* Obselete metrics type still used by PL1 */
typedef gl1_defs_metrics_rpt gl1_msg_metrics_rpt;

typedef enum
{
   GL1_DEFS_NO_LOOPBACK,
   GL1_DEFS_LOOPBACK_TYPE_A,
   GL1_DEFS_LOOPBACK_TYPE_B,
   GL1_DEFS_LOOPBACK_TYPE_C,
   GL1_DEFS_LOOPBACK_TYPE_D,
   GL1_DEFS_LOOPBACK_TYPE_E,
   GL1_DEFS_LOOPBACK_TYPE_F,
   GL1_DEFS_LOOPBACK_TYPE_I
}
gl1_defs_loopback_type;

typedef enum
{
   GL1_DEFS_FIXED_PDTCH,
   GL1_DEFS_FIXED_PACCH,
   GL1_DEFS_FIXED_RRBP_HI,
   GL1_DEFS_FIXED_RRBP_LO
} gl1_defs_fixed_block_type;

#endif
