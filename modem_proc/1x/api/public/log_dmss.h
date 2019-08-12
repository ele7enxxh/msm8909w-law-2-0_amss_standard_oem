#ifndef LOG_DMSS_H
#define LOG_DMSS_H

/*===========================================================================

                      Log Services Header File

General Description
  This file contains declarations for packets to be logged.

  Copyright (c) 1991 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                             Edit History

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/public/log_dmss.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/16/09   jtm     Modified 1xA log packet to support frame aligned PCGs.
11/24/09   jtm     Added additional information to the 1xA log packet.
10/09/09   jtm     Added 1xA logging support
09/22/09   adw     Removed unnecessary cdma2kdiag.h include.
08/31/09   adw     Removed unnecessary target.h include.
05/18/09   adw     Removed featurization as part of CMI phase-II cleanup.
12/15/06   as      Fixed lint errors.
11/19/04   as      Removed srch task dependencies.
02/26/04   rng     Made log_enh_fing_type and log_enh_fing_info_type available
                   without FEATURE_CDMA_RX_DIVERSITY
11/05/03   fu/aaj  Corrected rssi date type from byte to unit16
11/05/03   fu/aaj  Support for enh fing lock pkt for CDMA_RX_DIVERSITY feature
09/12/03   wd      Added FTM generic log paket
08/26/03   as      Removed FEATURE_SPECIAL_MDR
03/30/03   bt      moved QPCH log packet to srchlog.h.
10/24/02   lad     Backed out usage of LOG_RECORD_DEFINE.
05/03/02   gs      Added LOG_CFSET_M define for Candidate frequency search
                   logging.
03/21/02   br      Removed all gpsOne log definitions.
03/13/02   abh     Changes in GPS multipeak logs to log 32 bit eng sums for
                   MSM6050.
03/08/02   bn      Added interference-correction field into the access probe log packet
12/02/01   abh     Changes in GPS multipeak logs to log 16 bit floating point
                   energy sums. Added Local Oscillator Bias logging.
08/31/01   jal     Remove check for FEATURE_DIAG_COUPLED_ARCHITECTURE/
                   FEATURE_DIAG_PACKET_COUPLING (obsoleted by change in
                   log_codes.h.)
08/24/01   bn      Put the new DCCH FTM logging data fields at the end of the
                   for backwards compatible.
08/22/01   bn      Support DCCH in FTM mode
05/14/01   jal     Stripped out log code definitions, now including log_codes.h
                   from the MSM_DIAG_CORE_DEFS VU.  One place to change both
                   the new and old Diag VUs.  MUST #define
                   FEATURE_DIAG_COUPLED_ARCHITECTURE to work right!
05/09/01   jal     Featurized including srch.h -- Some unit testers need
                   Diag, but not the searcher.
05/08/01   jal     Synced log codes with MSM_DIAG_CORE_DEFS VU
04/10/01   jq      Moved some diag functionalities to MC.
03/26/01   br      Added more SRCH PPM log definitions.
03/07/01   jq      Added definition for ASET_INFO log.
01/28/01   abh     Added version id and Trk Lo to GPS Multipeak structure.
12/08/00   aaj     Added session_tag to GPS SRCH MULTIPEAK structure. Changed
                   couple of temporary variables with real names
12/05/00   lad     Created file from DMSS log.h.  Logging content only.
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/

/* Common */
#include "comdef.h"

/* Cp */
#include "cai.h"

/* Other */
#include "log.h"
#include "log_codes.h"
#include "nv.h"


/* -------------------------------------------------------------------------
** Data Declarations
** ------------------------------------------------------------------------- */

/* This is the array of "nice" values for message levels.  These values are
** used in a similar fashion as the UNIX "nice" command.  The higher the
** number, the less priority the item has.
*/
extern int16 log_nice[];

/*---------------------------------------------------------------------------
              SPARSELY-SAMPLED AGC AND POWER CONTROL ENTRY TYPE
                            LOG_S_AGC_PCTL_C
---------------------------------------------------------------------------*/
#define LOG_AGC_PCTL_CNT  100


typedef PACKED struct
{
  log_hdr_type  hdr;

  byte          adc_therm;
    /* Calibrated thermistor ADC sample value. */

  byte          batt_volt;
    /* Calibrated battery voltage */

  byte          tx_pwr_limit;
    /* CDMA TX power limit (hi 8-bits) */

  byte          agc_vals[ LOG_AGC_PCTL_CNT ];
    /* CAGC values sampled at 20 ms intevals. */

  byte          pctl_vals[ LOG_AGC_PCTL_CNT ];
    /* Power control values sampled at 20 ms intevals. */

  byte          tx_pwr_vals[ LOG_AGC_PCTL_CNT ];
    /* CAGC TX power estimate values (hi 8-bits) sampled at 20 ms intervals. */

}
log_s_agc_pctl_type;


/*---------------------------------------------------------------------------
                  REVERSE LINK FRAME STATS ENTRY TYPE
---------------------------------------------------------------------------*/

typedef PACKED struct
{
  log_hdr_type  hdr;

#define               LOG_R_MUX1_CNT  100
  byte          vals[ LOG_R_MUX1_CNT ];
    /* Mixed mode bits for sequential reverse link traffic frames. */

}
log_r_mux1_type;


/*---------------------------------------------------------------------------
                    ACCESS CHANNEL MESSAGE ENTRY TYPE
---------------------------------------------------------------------------*/

/* LOG_AC_MSG_C */
typedef PACKED struct
{
  log_hdr_type  hdr;

  byte          msg[ CAI_AC_MAX_MSG_BYTES ];
    /* Actual access channel message bits. */

}
log_ac_msg_type;

/*---------------------------------------------------------------------------
                    REVERSE TRAFFIC CHANNEL ENTRY TYPE
---------------------------------------------------------------------------*/

/* LOG_R_TC_MSG_C */
typedef PACKED struct
{
  log_hdr_type  hdr;

  byte          msg[ CAI_REV_TC_MAX_MSG_BYTES ];
    /* Actual reverse link traffic channel message bits. */

}
log_r_tc_msg_type;

/*---------------------------------------------------------------------------
                     SYNC CHANNEL MESSAGE ENTRY TYPE
---------------------------------------------------------------------------*/

/* LOG_SC_MSG_C */
typedef PACKED struct
{
  log_hdr_type hdr;

  byte          msg[ CAI_SC_MAX_MSG_BYTES ];
    /* Actual sync channel message bits. */

}
log_sc_msg_type;

/*---------------------------------------------------------------------------
                    PAGING CHANNEL MESSAGE ENTRY TYPE
---------------------------------------------------------------------------*/

/* LOG_PC_MSG_C */
typedef PACKED struct
{
  log_hdr_type hdr;

  byte          msg[ CAI_PC_MAX_MSG_BYTES ];
    /* Actual paging channel message bits. */

}
log_pc_msg_type;

/* FORWARD TRAFFIC CHANNEL MESSAGE ENTRY TYPE
*/
/* LOG_F_TC_MSG_C */
typedef PACKED struct
{
  log_hdr_type hdr;

  byte          msg[ CAI_FWD_TC_MAX_MSG_BYTES ];
    /* Actual forward link traffic channel message bits. */

}
log_f_tc_msg_type;

/* Vocoder Data Entry Type
*/
/* LOG_VOC_FOR_C */
typedef PACKED struct
{
  log_hdr_type hdr;

  byte  data[36];                 /* vocoder data */
  byte  rate;                     /* rate */
  boolean revlink;                /* is reverse link? */

}
log_voc_type;

/*---------------------------------------------------------------------------
                           FINGER ENTRY TYPE
 Note: similar info to that contained in the Temporal Analyzer log entry,
       but not including searcher data.
---------------------------------------------------------------------------*/
#define  MAX_FINGS_MSM22          3
  /* Max number of fingers for MSM2.2 */


/* LOG_FING_C */
typedef PACKED struct
{
  log_hdr_type hdr;

  word          fing_pilot[ MAX_FINGS_MSM22 ];
    /* For each finger, indicates which pilot offset is in use. */

  word          fing_x [ MAX_FINGS_MSM22 ];
    /* For each finger, gives low order 16 bits of absolute offset in
       1/8 chip units. */

  byte          fing_y[ MAX_FINGS_MSM22 ];
    /* For each finger, indicates the current RSSI value. If zero,
       indicates finger is not locked. */

}
log_fing_type;

/*---------------------------------------------------------------------------
                        SEARCHER PATH PACKET TYPE ( TYPE D )

 Note: An abstraction of the Temporal Analyzer log entry, but logs only
       three largest paths of each search to log every search.

 Basic usage:
       This is designed to be used running Async 115.2 Kbps, or HDLC 57K.
       It is designed to fit within the payload data of the log_srch2_type.
       Mobile packages up each search and sends this packet to the DM.
       This is designed to be logged when in TA mode and file logging enabled.
       The mobile MUST take care to carefully meter other packets or
       some of these will get lost.

       Example for max data rate calc from Mobile:

         Search rate:   Fastest  - 150 searches/sec (throttled value)
                        Moderate - 100 searches/sec (40-chip windows)
                        Slow     - 25 searches/sec (160-chip windows)

         Bytes/pkt:     B = (16 srchs/pkt) * sizeof(log_path_type) + [ovhd]
                          = 832 bytes + [24+?] = appx 860 (for example)

         Max rate:      R = 860 * 8 * 9.37 pks/sec = 64 kbits/sec (fastest)
                        R = 860 * 8 * 6.24 pks/sec = 43 kbits/sec (40-chip win)
                        R = 860 * 8 * 1.56 pks/sec = 11 kbits/sec (160-chip win)
                        Note: Anything above around 50 kbits/sec misses searches
---------------------------------------------------------------------------*/
#define MAX_PATHLOG_PATHS  3
#define MAX_PATHLOG_SRCHS 16   /* Data MUST fit log_srch2_type payload */
                               /* NOTE: 17 actually fit, but we have ovhd */

typedef PACKED struct
{
  int2          pilot;                  /* Pilot offset of search */
  byte          sect;                   /* Index of set membership */
  byte          set;                    /* Set membership */
  int2          win_siz;                /* Search window size in chips */
  int4          win_pos;                /* Absolute window position (chipx8) */
  word          tdiff;                  /* Time since pack start (1.25ms) */
  byte          frame_erase;            /* Count of frames/erasures */
  byte          filt_len;               /* Length of energy filter */
  word          filt_eng;               /* Filtered total energy of this PN */
  word          inx[MAX_PATHLOG_PATHS]; /* Position of path relative to window */
  word          eng[MAX_PATHLOG_PATHS]; /* Instantaneous energy of path */
  /* 14 words total */

  int2          f_pilot[MAX_FINGS_MSM22];  /* Finger's pilot assignment */
  byte          f_sect[MAX_FINGS_MSM22];   /* Finger's sector in Aset assignment */
  byte          f_rssi[MAX_FINGS_MSM22];   /* Finger RSSI */
  int4          f_pos[MAX_FINGS_MSM22];    /* Absolute finger positions */
  /* 12 words total */

  /* Note that we also pack the RX/TX/ADJ values into the top bytes
     of the finger positions, and the counts of frames and frame errors
     in the top byte of the searcher window positions. */
}
log_path_type;

typedef PACKED struct
{
  byte   pkt_seq_num;        /* Pkt seq num to keep track of skips */
  word   srch_seq_num;       /* Can reveal lost seaches if full pkt */
  byte   srch_cnt;           /* Number of searches in search array */
  qword  start_time;         /* Time started packing message */
  /* 6 words */

  /* Search-related parms */
  byte   state;
  byte   aset_cnt;
  byte   cset_cnt;
  byte   nset_cnt;
  byte   freq_est;
  byte   search_rate;
  /* 3 words */

  byte   adc_therm;    /* Calibrated thermistor ADC sample value. */
  byte   batt_volt;    /* Calibrated battery voltage */
  byte   tx_pwr_limit; /* CDMA TX power limit (hi 8-bits) */
  byte   rx_agc;       /* CAGC value */
  byte   tx_gain_adj;  /* Power control value */
  byte   tx_agc;       /* CAGC TX power estimate values (hi 8-bits) */
  /* 3 words */

  log_path_type srch[MAX_PATHLOG_SRCHS]; /* Paths for each search */
}
log_srch_type;


/*---------------------------------------------------------------------------
                        SEARCHER PATH PACKET TYPE ( TYPE A1 )

 Note: An abstraction of the Temporal Analyzer log entry, but logs only
       four largest paths of each search to log every search. This packet is
       based on the Type D packet and used to log the fourth finger in MSM2.3.

 Basic usage:
       This is designed to be used running Async 115.2 Kbps, or HDLC 57K.
       It is designed to fit within the payload data of the log_srch2_type.
       Mobile packages up each search and sends this packet to the DM.
       This is designed to be logged when in TA mode and file logging enabled.
       The mobile MUST take care to carefully meter other packets or
       some of these will get lost.

---------------------------------------------------------------------------*/
#define  MAX_FINGS_MSM23          4
  /* Max number of fingers for MSM2.3 */

#define  MAX_PATHS_MSM23          4
  /* Max number of paths for MSM2.3. MSM2.2 has three */

#define  LOG_PATH_MAX_DATA_CNT    14
  /* Max number of searches logged per packet. Data MUST fit log_srch2_type
     payload */

typedef PACKED struct
{
  int2   pilot;                       /* Pilot offset of search */
  byte   sect;                        /* Index of set membership */
  byte   set;                         /* Set membership */
  int2   win_siz;                     /* Search window size in chips */
  int4   win_pos;                     /* Absolute window position (chipx8) */
  word   tdiff;                       /* Time since pack start (1.25ms) */
  byte   frame_erase;                 /* Count of frames/erasures */
  word   filt_eng;                    /* Filtered total energy of this PN */
  word   inx[MAX_PATHS_MSM23];        /* Position of path relative to window */
  word   eng[MAX_PATHS_MSM23];        /* Instantaneous energy of path */
  /* 31 bytes total */

  int2   f_pilot[MAX_FINGS_MSM23];    /* Finger's pilot assignment */
  byte   f_sect[MAX_FINGS_MSM23];     /* Finger's sector in Aset assignment */
  byte   f_rssi[MAX_FINGS_MSM23];     /* Finger RSSI */
  int4   f_pos[MAX_FINGS_MSM23];      /* Absolute finger positions */
  /* 32 bytes total */

  /* Note that we also pack the RX/TX/ADJ values and the length of the
     energy filter into the top bytes of the finger positions, and
     the LNA state into the top byte of the searcher window position. */
}
log_path_data_type;

typedef PACKED struct
{
  byte   pkt_seq_num;        /* Pkt seq num to keep track of skips */
  word   srch_seq_num;       /* Can reveal lost seaches if full pkt */
  byte   srch_cnt;           /* Number of searches in search array */
  qword  start_time;         /* Time started packing message */
  /* 6 words */

  /* Search-related parms */
  byte   state;
  byte   aset_cnt;
  byte   cset_cnt;
  byte   nset_cnt;
  byte   freq_est;
  byte   search_rate;
  /* 3 words */

  byte   adc_therm;    /* Calibrated thermistor ADC sample value. */
  byte   batt_volt;    /* Calibrated battery voltage */
  byte   tx_pwr_limit; /* CDMA TX power limit (hi 8-bits) */
  byte   rx_agc;       /* CAGC value */
  byte   tx_gain_adj;  /* Power control value */
  byte   tx_agc;       /* CAGC TX power estimate values (hi 8-bits) */
  /* 3 words */

  log_path_data_type srch[ LOG_PATH_MAX_DATA_CNT ]; /* Paths for each search */
}
log_path_srch2_A1_type;


/*---------------------------------------------------------------------------
                   GENERAL TEMPORAL ANALYZER ENTRY TYPE
---------------------------------------------------------------------------*/

/* LOG_GENRL_TA_C */
typedef PACKED struct
{
  log_hdr_type hdr;

  byte            pregain;
    /* Pregain setting used for search: 0 = 2, 1 = 4, 2 = 8, 3 = 32. */

  byte            integ;
    /* Integration period used for search in 4 chip units. */

  byte            non_coh;
    /* Number of non-coherent integrations accumulated. Value is
       one less than actual value, e.g., 0 implies 1. */

  /* Following masks are OR'd into the pilot_off field. */

#define  LOG_ASET_M  0x8000  /* pilot belongs to active set. */
#define  LOG_CSET_M  0x4000  /* pilot belongs to candidate set. */
#define  LOG_NSET_M  0x2000  /* pilot belongs to neighbor set. */
#define  LOG_RSET_M  0x1000  /* pilot belongs to remaining set. */
#define  LOG_PCSET_M 0x0800  /* pilot belongs to the pre-candidate set. */
#define  LOG_NOCEN_M 0x0400  /* Suppress DM TA window recentering */
#define  LOG_CFSET_M 0x0200  /* pilot belongs to candidate frequency neighbor
                                set */
  unsigned short  pilot_off;
    /* Pilot offset used during search. */

  int4            win_pos;
    /* Absolute window ending position in 1/8 chip units. */

  unsigned short  win_cen;
    /* Specifies center position of searcher window space. */

  int2            tcxo_drift;
    /* Specifies tcxo error in 1/(8*256) chip units. Note number is
       signed. */

  unsigned short  path_pos[ MAX_PATHS_MSM23 ];
    /* Position of path relative to window */

  unsigned short  path_eng[ MAX_PATHS_MSM23 ];
    /* Instantaneous energy of path */

  unsigned short  f_pilot[ MAX_FINGS_MSM23 ];
    /* Finger's pilot assignment */

  byte            f_sect[ MAX_FINGS_MSM23 ];
    /* Finger's sector in Aset assignment */

  byte            f_rssi[ MAX_FINGS_MSM23 ];
    /* For each finger, indicates the current RSSI value. If zero,
       indicates finger is not locked. */

  int4            f_pos[ MAX_FINGS_MSM23 ];
    /* Absolute finger positions */

  byte            rx_agc;
    /* CAGC value */

  byte            tx_agc;
    /* CAGC TX power estimate values (hi 8 bits) */

  byte            tx_gain_adj;
    /* Power control value */

  byte            tx_pwr_limit;
    /* CDMA TX power limit (hi 8 bits) */

  byte            srch_state;
    /* Searcher state */

}
log_gen_ta_type;

/*---------------------------------------------------------------------------
        GENERAL TEMPORAL ANALYZER ENTRY WITH SUPPLEMENTAL CHANNELS TYPE
        NOTE: Same as log_gen_ta_type with sup channels added.
---------------------------------------------------------------------------*/

/* LOG_GENRL_TA_SUP_CH_C */
typedef PACKED struct
{
  log_hdr_type hdr;

  byte            pregain;
    /* Pregain setting used for search: 0 = 2, 1 = 4, 2 = 8, 3 = 32. */

  byte            integ;
    /* Integration period used for search in 4 chip units. */

  byte            non_coh;
    /* Number of non-coherent integrations accumulated. Value is
       one less than actual value, e.g., 0 implies 1. */

  unsigned short  pilot_off;
    /* Pilot offset used during search. */

  int4            win_pos;
    /* Absolute window ending position in 1/8 chip units. */

  unsigned short  win_cen;
    /* Specifies center position of searcher window space. */

  int2            tcxo_drift;
    /* Specifies tcxo error in 1/(8*256) chip units. Note number is
       signed. */

  unsigned short  path_pos[ MAX_PATHS_MSM23 ];
    /* Position of path relative to window */

  unsigned short  path_eng[ MAX_PATHS_MSM23 ];
    /* Instantaneous energy of path */

  unsigned short  f_pilot[ MAX_FINGS_MSM23 ];
    /* Finger's pilot assignment */

  byte            f_sect[ MAX_FINGS_MSM23 ];
    /* Finger's sector in Aset assignment */

  byte            f_rssi[ MAX_FINGS_MSM23 ];
    /* For each finger, indicates the current RSSI value. If zero,
       indicates finger is not locked. */

  int4            f_pos[ MAX_FINGS_MSM23 ];
    /* Absolute finger positions */

  byte            rx_agc;
    /* CAGC value */

  byte            tx_agc;
    /* CAGC TX power estimate values (hi 8 bits) */

  byte            tx_gain_adj;
    /* Power control value */

  byte            tx_pwr_limit;
    /* CDMA TX power limit (hi 8 bits) */

  byte            srch_state;
    /* Searcher state */

  byte              num_sup_chan;
    /* Number of supplemental channels */

  word walsh[ CAI_MAX_NUM_SUP + 1 ];
     /* Walsh Codes */

}
log_gen_ta_sup_ch_type;


/*---------------------------------------------------------------------------
                          ENVIRONMENT ENTRY TYPE
---------------------------------------------------------------------------*/
#define  LOG_ENVRN_MAX_DATA_CNT  37

/* Define a structure for environment data */
typedef PACKED struct
{
  unsigned short  pilot_off;
    /* Pilot offset used during search. */

  int4            win_pos;
    /* Absolute window position */

  unsigned short  path_pos[ MAX_PATHS_MSM23 ];
    /* Position of path relative to window */

  unsigned short  path_eng[ MAX_PATHS_MSM23 ];
    /* Instantaneous energy of path */

  unsigned short  tdiff;
    /* Time since packing starts (in 1.25ms) */

  /* Note: we load LNA state into the MSBit of the tdiff field and RX AGC value
           into the upper byte of the win_pos field. */
}
log_envrn_data_type;

typedef PACKED struct
{
  byte                 srch_cnt;
    /* Number of searches done for the packet */

  qword                start_time;
    /* Time we started packing */

  byte                 pregain;
    /* Pregain setting used for search: 0 = 2, 1 = 4, 2 = 8, 3 = 32. */

  byte                 integ;
    /* Integration period used for search in 4 chip units. */

  byte                 non_coh;
    /* Number of non-coherent integrations accumulated. Value is
       one less than actual value, e.g., 0 implies 1. */

  log_envrn_data_type  data[ LOG_ENVRN_MAX_DATA_CNT ];
    /* Data portion of the packet */
}
log_envrn_srch2_A0_type;


/*---------------------------------------------------------------------------
                         MARKOV FRAME STATISTICS
---------------------------------------------------------------------------*/

/* LOG_MAR_C */
typedef PACKED struct
{
  log_hdr_type  hdr;

  byte          count;
    /* number of entries in entry array */

#define                   LOG_MAR_CNT 100
  PACKED struct
  {
    byte          mar_inx;
    /* Decision indices for sequential frames. Each index value is computed
       according to the following formula:

         mar_inx = (EXPECTED_MUX1 * 16) + ACTUAL_MUX1

       where EXPECTED_MUX1(i) is the MUX1 index (1 to 14, a la CAI) predicted
       by the Markov process for frame i, and ACTUAL_MUX1 is the MUX1 index
       actually decided upon by the mobile's rate determination algorithm for
       frame i. */

    byte          bit_errs;
      /* Bit error counts for sequential frames. */
  }entry[ LOG_MAR_CNT];

}
log_mar_type;

/*---------------------------------------------------------------------------
                        SEARCHER 2 ENTRY TYPE
 Note: Supersedes previous Temporal Analyzer log entry type. Allows for
       variable window size.
---------------------------------------------------------------------------*/

/* LOG_SRCH2_C, LOG_SRCH_C */
typedef PACKED struct
{
  log_hdr_type hdr;

  byte          pregain;
    /* Pregain setting used for search: 0 = 2, 1 = 4, 2 = 8, 3 = 32. */

  byte          integ;
    /* Integration period used for search in 4 chip units. */

  byte          non_coh;
    /* Number of non-coherent integrations accumulated. Value is
       one less than actual value, e.g., 0 implies 1. */

  /* Following masks are OR'd into the pilot_off field. */

  word          pilot_off;
    /* Pilot offset used during search. */

  word          win_pos;
    /* Low order 16 bits of absolute window ending position in 1/8 chip
       units. */

  word          win_cen;
    /* Specifies center position of searcher window space. */

  int2          tcxo_drift;
    /* Specifies tcxo error in 1/(8*256) chip units. Note number is
       signed. */

  word          num_entries;
    /* Number of entries in energies array. */

#define                   LOG_TA2_CNT  (228*2)
  word          energies[ LOG_TA2_CNT ];
    /* Energies versus half chip time offset. */

}
log_srch2_type;

/*---------------------------------------------------------------------------
                     GENERAL ANALOG INFORMATION ENTRY TYPE
---------------------------------------------------------------------------*/
/* LOG_ANALOG_INFO_C */
typedef PACKED struct
{
  log_hdr_type hdr;

  word            sid;        /* SID            */
  word            channel;    /* Analog Channel */
  byte            scc;        /* SCC            */
  byte            rssi;       /* RSSI           */
  byte            power;      /* Tx Power       */
}
LOG_ANALOG_INFO_C_type;

/*---------------------------------------------------------------------------
                      ANALOG FOCC Word Sync Count ENTRY TYPE
---------------------------------------------------------------------------*/
/* LOG_ANALOG_WS_COUNT_C */
typedef PACKED struct
{
  log_hdr_type hdr;

  dword ws_count;            /* Word Sync Count */

}
log_analog_ws_count_type;

/*---------------------------------------------------------------------------
                     ANALOG HANDOFF ENTRY TYPE
---------------------------------------------------------------------------*/
typedef enum           /* Handoff Type */
{
  CDMA_TO_ANALOG_HANDOFF_LOG = 0,
  ANALOG_HANDOFF_LOG
}
analog_handoff_type;

typedef enum        /* Indicates reason for Handoff    */
{
  HANDOFF_MASK_NA = 0x0,   /* Not Applicable(CDMA-ANALOG HO ) */
  VCHAN_CHANGE_MASK = 0x1, /* Change in Voice Channel         */
  SCC_CHANGE_MASK = 0x2,   /* Change in SCC                   */
  VMAC_CHANGE_MASK = 0x4   /* Change in VMAC                  */
}
analog_handoff_mask_type;


/* LOG_ANALOG_HANDOFF_C */
typedef PACKED struct
{
  log_hdr_type hdr;

  byte               handoff_type;     /* 0 - Cdma to Analog
                                          1 - Analog Handoff         */
  byte               change_mask;      /* Reason for Handoff         */
  word               amps_vc;          /* AMPS Voice Channel Number  */
  byte               scc;              /* SCC                        */
  byte               pwr_level;        /* Power Level                */
  word               system_id;        /* System Identification Code */
}
log_analog_handoff_type;

/*---------------------------------------------------------------------------
                      ANALOG CHANNEL(FORW/REVS) ENTRY TYPE
---------------------------------------------------------------------------*/
/* Max Channel Log Buffer Size */
#define        CHANNEL_LOG_MAX         200
typedef enum  /* analog channel type */
{
   ANALOG_FOCC = 0,
   ANALOG_FVC,
   ANALOG_RECC,
   ANALOG_RVC
}
analog_channel_type;


/* LOG_ANALOG_FORW_C, LOG_ANALOG_REVS_C */
typedef PACKED struct
{
  log_hdr_type hdr;

  byte msg[CHANNEL_LOG_MAX];  /* Buffer to hold channel data */

}
log_analog_channel_type;


/* ANALOG FM_SLOT Statistics ENTRY TYPE
*/
/* LOG_ANALOG_FMSLOT_C */
typedef PACKED struct
{
  log_hdr_type hdr;
  dword invalid_words;           /* invalid words   */
  dword good_words;              /* good words      */
  dword single_errs;             /* single errors   */
  dword multiple_errs;           /* multiple errors */

  dword total_rx_isr;            /* total recvd frames                   */
  dword no_free_buffs;           /* count of *out of free buf* condition */
  dword skip_words;              /* #words skipped after a DFM reset     */

  dword slot_count_1;            /* #words decoded after 1 slot  */
  dword slot_count_2;            /* #words decoded after 2 slot  */
  dword slot_count_3;            /* #words decoded after 3 slot  */
  dword slot_count_4;            /* #words decoded after 4 slot  */
  dword slot_count_5;            /* #words decoded after 5 slot  */

  dword error_slot_count_1;      /* #errors in 1 slot decoded words */
  dword error_slot_count_2;      /* #errors in 2 slot decoded words */
  dword error_slot_count_3;      /* #errors in 3 slot decoded words */
  dword error_slot_count_4;      /* #errors in 4 slot decoded words */
  dword error_slot_count_5;      /* #errors in 5 slot decoded words */

  byte  new_sync_erase_value;    /* new status of sync erasure  */
  byte  old_sync_erase_value;    /* old status of sync erasure  */
  dword sync_erase_count;        /* #sync erasures              */
  dword sync_erase_error_count;  /* #errors during sync erasure */
  dword out_of_sync_frames;      /* #out of sync frames         */
  dword out_of_sync_count;       /* #out of sync                */

  dword new_time_value;          /* current time                */
  dword old_time_value;          /* old time                    */
}
log_analog_fmslot_type;

/* Band Class Change Type
** Note: This is not in use for QCT builds as of 10/26/00
*/
/* LOG_BAND_CHANGE_C */
typedef PACKED struct
{
  log_hdr_type hdr;

  byte         band_class;
  word         channel;

}
LOG_BAND_CHANGE_C_type;

/* EFS Information Type
*/
/* LOG_EFS_INFO_C */
typedef PACKED struct
{
  log_hdr_type hdr;

  byte efs_info[CHANNEL_LOG_MAX];       /* Buffer to EFS Log data */

}
LOG_EFS_INFO_C_type;

/* Access Probe Information Type
*/
/* LOG_ACC_INFO_C */
typedef PACKED struct
{
  log_hdr_type hdr;

  byte seq_num;
    /* Access probe sequence number */
  byte probe_num;
    /* Access probe number */
  byte rx_agc;
    /* Receive AGC */
  byte tx_adj;
    /* TX gain adjust */
  byte psist;
    /* number of persistence tests performed */
  byte ra;
    /* access channel number */
  byte rn;
    /* PN randomization delay */
  byte rs;
    /* Sequence backoff */
  byte rt;
    /* probe backoff */
  byte int_corr;
    /* interference-correction  */

}
log_acc_info_type;


/*---------------------------------------------------------------------------
                       RLP PARTIAL FRAMES LOG TYPE
---------------------------------------------------------------------------*/
/* This is a high volume accumulated log.  Many fields are bit stuffed. */

#define LOG_MAX_RLP_PKT 300

typedef enum {
  LOG_RLP_FCH    = 0,  /* Fundamental Channel          */
  LOG_RLP_DCCH   = 1,  /* Dedicated Control Channel    */
  LOG_RLP_SCH_0  = 2,  /* Supplemental Channel 0       */
  LOG_RLP_SCH_1  = 3,  /* Supplemental Channel 1       */
  LOG_RLP_SCCH_0 = 4,  /* IS95B Supplemental Channel 0 */
  LOG_RLP_SCCH_1 = 5,  /* IS95B Supplemental Channel 1 */
  LOG_RLP_SCCH_2 = 6,  /* IS95B Supplemental Channel 2 */
  LOG_RLP_SCCH_3 = 7,  /* IS95B Supplemental Channel 3 */
  LOG_RLP_SCCH_4 = 8,  /* IS95B Supplemental Channel 4 */
  LOG_RLP_SCCH_5 = 9,  /* IS95B Supplemental Channel 5 */
  LOG_RLP_SCCH_6 = 10, /* IS95B Supplemental Channel 6 */
  LOG_RLP_SCCH_7 = 11, /* IS95B Supplemental Channel 7 */
  LOG_RLP_PDCH   = 12, /* EVDV Pkt Data Channel        */

  LOG_RLP_MAX    = 15  /* Values 13-15 reserved        */
} log_rlp_phy_channel_enum_type;

/* Channel frame entry for partial RLP frame.
*/
/* Plans exist to change this format slightly! */
typedef PACKED struct {
  PACKED struct {
    byte phy_chan : 4;       /* log_rlp_phy_channel_enum_type  */
    byte mux_pdu  : 3;       /* Use mux_pdu_type 0-7           */
                             /* 0 == "mux_pdu1"                */
                             /* 1 == "mux_pdu2"                */
    byte rsvd     : 1;
  } mux_chan;

  PACKED struct {
    word phy_chan_rate : 14; /* Physical Channel Rate / 100    */
    word time_delta    : 1;  /* 0 if same frame,
                                1 if next frame                */
    word rsvd          : 1;
  } time_rate;

  word frame_len;            /* Length, in bits, of this frame. */
                             /* 0 for blank, 0xFFFF for erasure.*/

  PACKED struct {
    byte tail :2;            /* Number of tail payload bytes    */
    byte head :6;            /* Number of head payload bytes    */

  } payload_len;

  byte payload[1];           /* Head and tail payload bytes,
                                length given above.             */
} log_rlp_pkt_type;

/* Partial RLP frames log record.
*/
/* LOG_RLP_RX_FRAMES_C */
typedef PACKED struct
{
  log_hdr_type hdr;

  byte         service_ID; /* Service ID                      */
  byte         count;      /* Count of partial RLP packets    */

  PACKED union {

    log_rlp_pkt_type pkt[1];
    byte             buf_ref[LOG_MAX_RLP_PKT];
                           /* Accumulated partial RLP packets
                              defined in log_rlp_pkt_type     */
  } var_len_buf;

}
log_rlp_frames_type;

typedef log_rlp_frames_type LOG_RLP_RX_FRAMES_C_type;
typedef log_rlp_frames_type LOG_RLP_TX_FRAMES_C_type;

#define LOG_MAX_RLP_PAYLOAD_LEN 34

#define LOG_MAX_RLP_FRM_SIZE \
  (FPOS(log_rlp_pkt_type, payload) + LOG_MAX_RLP_PAYLOAD_LEN)

/* Threshold to determine when the buf is full and the log should be sent. */
#define LOG_RLP_FRM_BUF_THRESHOLD \
  (sizeof(log_rlp_frames_type) - \
    (FPOS(log_rlp_pkt_type, payload) + LOG_MAX_RLP_PAYLOAD_LEN))

/*---------------------------------------------------------------------------
                         RLP STATISTICS LOG TYPE
---------------------------------------------------------------------------*/
#define LOG_RLP_MAX_NAK_ROUNDS  3

/*---------------------------------------------------------------------------
  RLP statistics information definition. This structure contains all of the
  RLP statistical information for the currently active RLP session.
---------------------------------------------------------------------------*/
/* LOG_RLP_STAT_C */
typedef PACKED struct
{
  log_hdr_type hdr;

  uint8        service_ID; /* Service ID                          */
  qword        reset_time; /* Timestamp for last statistic reset. */

  uint32 naks_received ;           /* total # of nak frames received        */
  uint32 re_xmits_not_found;       /* # of Rexmits not found                */
  uint16 largest_contig_erasure;   /* largest block of consecutive erasures */
  uint8  resets;                   /* # of resets                           */
  uint8  aborts;                   /* # of aborts    (nak aborts)           */
  uint32 last_rtt;                 /* round trip time                       */
  uint8  rlp_blob_used;            /* 0-no BLOB; 1-NAK; 2-RTT; 3-NAK+RTT    */

  uint32 rx_rexmit_fr_cnt;         /* # of received re-transmitted frames   */
  uint32 rx_idle_fr_cnt;           /* # of received idle frames             */
  uint32 rx_fill_fr_cnt;           /* # of received fill frames             */
  uint32 rx_blank_fr_cnt;          /* # of blank frames received            */
  uint32 rx_null_fr_cnt;           /* # of null frames received             */
  uint32 rx_new_data_fr_cnt;       /* total # of received new data frames   */
  uint32 rx_20ms_fr_cnt;           /* # of received Fundamental data frames */
  uint32 rx_total_bytes;           /* total # of bytes received             */
  uint32 rx_rlp_erasures;          /* # of rlp erasures received            */
  uint32 rx_mux_erasures;          /* # of mux erasures received            */

  uint32 tx_rexmit_fr_cnt;         /* # of transmitted re-transmitted frames*/
  uint32 tx_idle_fr_cnt;           /* # of transmitted idle frames          */
  uint32 tx_new_data_fr_cnt;       /* total # of new transmitted data frames*/
  uint32 tx_20ms_fr_cnt;           /* # of txed Fundamental data frames     */
  uint32 tx_total_bytes;           /* total # of bytes transmitted          */
  uint8  tx_naks_cnt;              /* Number of tx_naks[] entries to follow */
  uint32 tx_naks[LOG_RLP_MAX_NAK_ROUNDS];
                                   /* single, double etc NAKs sent array    */

}
log_rlp_stats_type;


/*---------------------------------------------------------------------------
                     FORWARD FRAME TYPES LOG TYPE
---------------------------------------------------------------------------*/
/* Dedicated forward channel set mask.
*/
typedef PACKED union {

  PACKED struct {
    byte f_fch      : 1; /* F-FCH  active      */
    byte f_sch0     : 1; /* F-SCH1 active      */
    byte f_sch1     : 1; /* F-SCH0 active      */
    byte f_dcch     : 1; /* F-DCCH active      */
    byte f_scch_num : 3; /* # of F-SCCH active */
    byte reserved   : 1; /* unused/reserved    */

  } chan_set;

  byte mask; /* Used to init the entire field */

} log_ded_fwd_chan_set_type;

/* Dedicated reverse channel set mask.
*/
typedef PACKED union {

  PACKED struct {
    byte r_fch     : 1; /* R-FCH  active     */
    byte r_sch0    : 1; /* R-SCH1 active     */
    byte r_sch1    : 1; /* R-SCH0 active     */
    byte r_dcch    : 1; /* R-DCCH active     */
    byte reserved  : 4; /* Unused/reserved   */

  } chan_set;

  byte mask; /* Used to init the entire field */

} log_ded_rev_chan_set_type;

typedef PACKED struct {
  uint16 f_fch_rc  :4; /* Forward Fundamental Channel Radio Config       */
  uint16 f_sch0_rc :4; /* Forward Supplemental Channel 0 Radio Config    */
  uint16 f_sch1_rc :4; /* Forward Supplemental Channel 1 Radio Config    */
  uint16 f_dcch_rc :4; /* Forward Dedicated Control Channel Radio Config */

} log_fwd_ch_rc_type;

typedef PACKED struct {
  uint16 r_fch_rc  :4; /* Reverse Fundamental Channel Radio Config       */
  uint16 r_sch0_rc :4; /* Reverse Supplemental Channel 0 Radio Config    */
  uint16 r_sch1_rc :4; /* Reverse Supplemental Channel 1 Radio Config    */
  uint16 r_dcch_rc :4; /* Reverse Dedicated Control Channel Radio Config */

} log_rev_ch_rc_type;

/* Enum for FCH frame type */
typedef enum {
  LOG_FCH_TYPE_PAYLOAD_ONLY = 0,
  LOG_FCH_TYPE_DIM_BURST    = 1,
  LOG_FCH_TYPE_BLANK_BURST  = 2

} log_fch_type_enum_type;

/* Enum for F-FCH rates */
typedef enum {
  LOG_FFCH_ERASURE_RATE = 0, /* Indicates erasure       */
  LOG_FFCH_EIGHTH_RATE  = 1, /* Indicates rate 1/8 data */
  LOG_FFCH_QUARTER_RATE = 2, /* Indicates rate 1/4 data */
  LOG_FFCH_HALF_RATE    = 3, /* Indicates rate 1/2 data */
  LOG_FFCH_FULL_RATE    = 4  /* Indicates rate 1   data */

} log_f_fch_rate_enum_type;

/* Enum for R-FCH rates */
typedef enum {
  LOG_RFCH_FULL_RATE    = 0, /* Indicates rate 1   data */
  LOG_RFCH_HALF_RATE    = 1, /* Indicates rate 1/2 data */
  LOG_RFCH_QUARTER_RATE = 2, /* Indicates rate 1/4 data */
  LOG_RFCH_EIGHTH_RATE  = 3  /* Indicates rate 1/8 data */

} log_r_fch_rate_enum_type;

/* Enum for SCH and SCCH channels */
typedef enum {
  LOG_SCH_FULL_RATE   = 0,
  LOG_SCH_BLANK_FRAME = 1,
  LOG_SCH_ERASUE      = 2,
  LOG_SCH_NOT_ACTIVE  = 3  /* Valid for SCH only */

} log_sch_rate_enum_type; /* All SCH and SCCH channels */


/* Slowest case: 25bytes/1byte(per 20ms frame) --> 0.5 sec per log
** Fastest case: 25bytes/2bytes(per 20ms frame) --> 0.25 sec per log
*/
#define LOG_MAX_FRAME_TYPES_SIZE (25 + 4)

/* Forward Frame Types frame record.  This structure defines each frame entry
** in the sub-record.
*/
typedef PACKED union {

  PACKED struct {
    byte rate : 4; /* Use log_f_fch_rate_enum_type */
    byte type : 4; /* Use log_fch_type_enum_type   */
  } f_fch;

  byte f_sch0; /* Use log_sch_rate_enum_type */
  byte f_sch1; /* Use log_sch_rate_enum_type */

  byte f_dcch; /* DCCH MUX type */

  PACKED struct {
    byte f_scch1 : 2; /* Use log_sch_rate_enum_type */
    byte f_scch2 : 2; /* Use log_sch_rate_enum_type */
    byte f_scch3 : 2; /* Use log_sch_rate_enum_type */
    byte f_scch4 : 2; /* Use log_sch_rate_enum_type */

  } f_scch1_4; /* F-SCCH 1-4 */

  PACKED struct {
    byte f_scch5  : 2; /* Use log_sch_rate_enum_type */
    byte f_scch6  : 2; /* Use log_sch_rate_enum_type */
    byte f_scch7  : 2; /* Use log_sch_rate_enum_type */
    byte reserved : 2;

  } f_scch5_7; /* F-SCCH 5-7 */
} log_fwd_frame_types_frm_type;

/* Forward Frame Types sub-record.  Used to log multiple frames
** with the same active channel set.  If the active channel set
** changes, a new sub-record is started.
*/
typedef PACKED struct {
  log_ded_fwd_chan_set_type ch_set_mask; /* Set of active channels */

  word frame_cnt; /* # of frames represented in this sub-record */

  byte len_per_frame; /* Length, in bytes, of each frame entry */

  /* Array of frame type indicators. One per channel per frame.
  ** Note: There will be an entry for each member that is associated with an
  ** active channel in the active channel mask.  Not all members will exist
  ** in each frame entry.  Entries appear in order of active LSB to MSB in
  ** the channel set mask (FCH first).
  */
  PACKED union {
    /* Array of multiple frame entries.
    ** Up to 4 entries (4 ch or FCH + SCCH) per frame
    */
    log_fwd_frame_types_frm_type frame_type_ind[4];

    /* Member for referenceing offsets in buf */
    byte buf_ref[LOG_MAX_FRAME_TYPES_SIZE - 4];

  } var_len_buf; /* sizeof(var_len_buf) gives max length */

} log_fwd_frame_types_subrecord_type; /* FTSS */

/* Forward Frame Types Log Record
*/
/* LOG_FWD_FRAME_TYPES_C */
typedef PACKED struct
{
  log_hdr_type hdr;

  byte frame_offset; /* Frame offset in 1.25 ms units. */

  word so;           /* Service option */

  log_fwd_ch_rc_type fwd_ch_rc;
                     /* Forward channel Radio Config Info */

  byte sr_cnt;       /* Count of sub-records */

  /* Series of var length log_fwd_frame_types_subrecord_type */
  PACKED union {
    /* Buffer will contain at least 1 subrecord */
    log_fwd_frame_types_subrecord_type sr[1];

    /* Member for referenceing offsets in buf */
    byte buf_ref[LOG_MAX_FRAME_TYPES_SIZE];

  } var_len_buf; /* sizeof(var_len_buf) gives max length */

}
log_fwd_frame_types_type;

/* Threshold to determine when the buf is full and the log should be sent.
*/
#define LOG_FWD_FRAME_TYPES_BUF_THRESHOLD \
  (sizeof(log_fwd_frame_types_type) -     \
   FPOS(log_fwd_frame_types_subrecord_type, var_len_buf.frame_type_ind[4]))


/*---------------------------------------------------------------------------
                     REVERSE FRAME TYPES LOG TYPE
---------------------------------------------------------------------------*/

/* Reverse Frame Types frame record.  This structure defines each frame entry
** in the sub-record.
*/
typedef PACKED union {

  PACKED struct {
    byte rate : 4; /* Use log_r_fch_rate_enum_type */
    byte type : 4; /* Use log_fch_type_enum_type   */
  } r_fch;

  byte r_sch0; /* Use log_sch_rate_enum_type */
  byte r_sch1; /* Use log_sch_rate_enum_type */

  byte r_dcch; /* DCCH frame category */

} log_rev_frame_types_frm_type;

/* Reverse Frame Types sub-record.  Used to log multiple frames with the same
** active channel set.  If the active channel set changes, a new
** sub-record is started.
*/
typedef PACKED struct {
  log_ded_rev_chan_set_type ch_set_mask; /* Set of active channels */

  word frame_cnt; /* # of frames represented in this sub-record */

  byte len_per_frame; /* Length, in bytes, of each frame entry */

  /* Array of frame type indicators. One per channel per frame.
  ** Note: There will be an entry for each member that is associated with an
  ** active channel in the active channel mask.  Not all members will exist
  ** in each frame entry.  Entries appear in order of active LSB to MSB in
  ** the channel set mask (FCH first).
  */
  PACKED union {
    /* Array of multiple frame entries.
    ** Up to 4 entries (4 ch) per frame
    */
    log_rev_frame_types_frm_type frame_type_ind[4]; /* 4 is the max per frame */

    /* Member for referenceing offsets in buf */
    byte buf_ref[LOG_MAX_FRAME_TYPES_SIZE - 4];

  } var_len_buf; /* sizeof(var_len_buf) gives max length */

} log_rev_frame_types_subrecord_type; /* FTSS */

/* Reverse Frame Types Log Record
*/
/* LOG_REV_FRAME_TYPES_C */
typedef PACKED struct
{
  log_hdr_type hdr;

  byte frame_offset; /* Frame offset in 1.25 ms units. */

  word so;           /* Service option */

  log_rev_ch_rc_type rev_ch_rc;
                     /* Reverse channel Radio Config Info */

  byte sr_cnt;       /* Count of sub-records */

  /* Series of var length log_rev_frame_types_subrecord_type */
  PACKED union {
    /* Buffer will contain at least 1 subrecord */
    log_rev_frame_types_subrecord_type sr[1];

    /* Member for referenceing offsets in buf */
    byte buf_ref[LOG_MAX_FRAME_TYPES_SIZE];

  } var_len_buf; /* sizeof(var_len_buf) gives max length */

}
log_rev_frame_types_type;

/* Threshold to determine when the buf is full and the log should be sent.
*/
#define LOG_REV_FRAME_TYPES_BUF_THRESHOLD \
  (sizeof(log_fwd_frame_types_type) -     \
   FPOS(log_rev_frame_types_subrecord_type, var_len_buf.frame_type_ind[4]))


/*---------------------------------------------------------------------------
                   FAST FORWARD POWER CONTROL LOG TYPE
---------------------------------------------------------------------------*/
/* Slowest case: 75bytes/3bytes(per 20ms frame) --> 0.5 sec per log
** Fastest case: 75bytes/5bytes(per 20ms frame) --> 0.3 sec per log
*/
#define LOG_MAX_FFPC_SIZE (75 + 4) /* 75 frame entries + 4 per sr */

/* Fast Forward Power Control frame record.  This structure defines each
** frame entry in the sub-record.
*/
typedef PACKED struct {
  word dec_history; /* Decision History: 16 1-bit decisions per frame */

  /* List of ffpc setpoint indicators that are active in the channel set mask.
  ** Note: There will be an entry for each member that is associated with an
  ** active channel in the active channel mask.  Not all members will exist
  ** in each frame entry.  Entries appear in order of active LSB to MSB in
  ** the channel set mask (FCH first).
  */
  PACKED union {
    uint8 f_fch;       /* F-FCH setpoint in 0.125 bB units       */
    uint8 f_sch0;      /* F-SCH0 setpoint in 0.125 bB units      */
    uint8 f_sch1;      /* F-SCH1 setpoint in 0.125 bB units      */
    uint8 f_dcch;      /* F-DCCH setpoint in 0.125 bB units      */
    int8  f_sch_delta; /* F-SCH delta setpoint in 0.125 dB units */

  } setpts[5]; /* Up to five entries based on channel set mask. */

} log_ffwd_pctrl_frame_type;

/* Fast Forward Power Control sub-record.  Used to log multiple frames with
** the same active channel set.  If the active channel set changes, a new
** sub-record is started.
*/
typedef PACKED struct {
  log_ded_fwd_chan_set_type ch_set_mask; /* Set of active channels */

  word frame_cnt; /* # of frames represented in this sub-record */

  byte len_per_frame; /* Length, in bytes, of each frame entry */

  /* Variable length buffer containing 'frame_cnt'
  ** entries of log_ffwd_pctrl_frame_type
  */
  PACKED union {

    /* Each frame entry is variable length based on channel set mask */
    log_ffwd_pctrl_frame_type frame_vals[1];

    /* Member for referenceing offsets in buf */
    byte buf_ref[LOG_MAX_FFPC_SIZE - 4];

  } var_len_buf; /* sizeof(var_len_buf) gives max length */

} log_ffwd_pctrl_subrecord_type;

/* Fast Forward Power Control Log Record
*/
/* LOG_FFWD_PCTRL_C */
typedef PACKED struct
{
  log_hdr_type hdr;

  byte frame_offset; /* Frame offset in 1.25 ms units. */

  word so;               /* Service Option                 */
  byte fpc_mode;         /* FPC Mode                       */
  byte fpc_pri_chan;     /* Sub-channel Indicator          */
  byte fpc_subchan_gain; /* Sub-channel gain 0.25dB units  */

  byte sr_cnt;       /* Count of sub-records */

  /* Series of var length log_ffwd_pctrl_subrecord_type */
  PACKED union {
    /* Buffer will contain at least 1 subrecord */
    log_ffwd_pctrl_subrecord_type sr[1];

    /* Member for referenceing offsets in buf */
    byte buf_ref[LOG_MAX_FFPC_SIZE];

  } var_len_buf; /* sizeof(var_len_buf) gives max length */

}
log_ffwd_pctrl_type;

/* Threshold to determine when the buf is full and the log should be sent.
*/
#define LOG_FFPC_BUF_THRESHOLD                          \
  (sizeof(log_ffwd_pctrl_type) -                        \
    (FPOS(log_ffwd_pctrl_subrecord_type, var_len_buf) + \
     sizeof(log_ffwd_pctrl_frame_type)))


/*---------------------------------------------------------------------------
                 REVERSE POWER CONTROL LOG TYPE
---------------------------------------------------------------------------*/
/* Slowest case: 175bytes/7bytes(per 20ms frame) --> 0.5 sec per log
** Fastest case: 175bytes/9bytes(per 20ms frame) --> 0.38 sec per log
*/
#define LOG_MAX_REV_PCTRL_SIZE 175

/* Reverse Power Control frame record.  This structure defines each
** frame entry in the sub-record.
*/
typedef PACKED struct {

  uint16 dec_history; /* Power control decision history */

  uint8  rx_agc;       /* RX AGC vals            */
  uint8  tx_pwr;       /* TX CAGC vals           */
  uint8  tx_gain_adj;  /* Gain adjust pctrl vals */

  /* List rpctrl setpoint indicators that are active in chan_mask.
  ** Note: There will be an entry for each member that is associated with an
  ** active channel in the active channel mask.  Not all members will exist
  ** in each frame entry.  Entries appear in order of active LSB to MSB in
  ** the channel set mask (FCH first).
  */
  PACKED union {
    int16 r_fch_pich;  /* R-FCH TX power in 0.125 bB units  */
    int16 r_sch0_pich; /* R-SCH0 TX power in 0.125 bB units */
    int16 r_sch1_pich; /* R-SCH1 TX power in 0.125 bB units */
    int16 r_dcch_pich; /* R-DCCH TX power in 0.125 bB units */

  } tx_ch_pwr[4]; /* array of channel pwr vals. Up to 4 per frame. */
} log_rev_pctrl_frame_type;

/* Reverse Power Control sub-record.  Used to log multiple frames with
** the same active channel set.  If the active channel set changes, a new
** sub-record is started.
*/
typedef PACKED struct {
  log_ded_rev_chan_set_type ch_set_mask; /* Set of active channels */

  word frame_cnt;   /* # of frames represented in this sub-record */

  byte len_per_frame; /* Length, in bytes, of each frame entry */

  /* Variable length buffer containing 'frame_cnt'
  ** entries of log_rev_pctrl_frame_type
  */
  PACKED union {

    /* Each frame entry is variable length based on channel set mask */
    log_rev_pctrl_frame_type frame_vals[1];

    /* Member for referenceing offsets in buf */
    byte buf_ref[LOG_MAX_REV_PCTRL_SIZE - 3];

  } var_len_buf; /* sizeof(var_len_buf) gives max length */

} log_rev_pctrl_subrecord_type;

/* Reverse Power Control Log Record
*/
/* LOG_REV_PCTRL_C */
typedef PACKED struct
{
  log_hdr_type hdr;

  byte frame_offset; /* Frame offset in 1.25 ms units. */

  byte band_class;   /* Band Class */

  log_rev_ch_rc_type rev_ch_rc;
                     /* Reverse channel Radio Config Info */

  byte pilot_gating_rate; /* Pilot gating rate       */
  byte step_size;         /* Power control step size */

  byte sr_cnt;       /* Count of sub-records */

  /* Series of var length log_rev_pctrl_subrecord_type */
  PACKED union {

    /* Buffer will contain at least 1 subrecord */
    log_rev_pctrl_subrecord_type sr[1];

    /* Member for referenceing offsets in buf */
    byte buf_ref[LOG_MAX_REV_PCTRL_SIZE];

  } var_len_buf; /* sizeof(var_len_buf) gives max length */

}
log_rev_pctrl_type;

/* Threshold to determine when the buf is full and the log should be sent.
*/
#define LOG_REV_PCTRL_BUF_THRESHOLD \
  (sizeof(log_rev_pctrl_type) - \
    (FPOS(log_rev_pctrl_subrecord_type, var_len_buf) + \
     sizeof(log_rev_pctrl_frame_type)))


/*---------------------------------------------------------------------------
                 SEARCHER AND FINGER INFORMATION LOG TYPE
---------------------------------------------------------------------------*/
typedef PACKED struct {
    uint2 path_pos;  /* Position of path relative to window */

    uint2 path_eng;  /* Instantaneous energy of path        */

} log_srch_path_info_type; /* 'num_peaks' items in array */

typedef PACKED union {
  PACKED struct {
    word enabled   : 1;
    word assgn     : 1; /* Is the finger assigned?                      */
    word comb_lock : 1; /* Is the finger locked in the symbol combiner? */
    word pc_lock   : 1; /* The rest (below) are not yet used            */
    word aux_pilot : 1;
    word otd_pilot : 1;
    word sts_pilot : 1;
    word div_chan  : 1;
    word sr3       : 1;
    word non_pilot : 1;
    word reserved  : 6;
  } status;

  word mask; /* Use to init entire mask. */

} log_srch_fing_status_type;

typedef PACKED struct {
    uint2 f_pilot; /* Finger's pilot assignment                          */

    byte  f_sect;  /* Finger's sector in Aset assignment                 */

    byte  f_rssi;  /* For each finger, indicates the current RSSI value.
                      If zero, indicates finger is not locked.           */

    int4  f_pos;   /* Absolute finger position                           */


    log_srch_fing_status_type status_mask; /* Finger status */

} log_srch_fing_info_type;

/* LOG_SRCH_FING_INFO_C */
typedef PACKED struct
{
  log_hdr_type hdr;

  PACKED struct {
    byte td_entry   : 1; /* 1 == TX diversity pilot in this record */
    byte td         : 1; /* 1 == TX diversity enabled              */
    byte pilot_type : 1; /* 0 == Common, 1 == Auxiliary            */
    byte reserved   : 5;
  } pilot_type_info;

  byte band_class; /* Band Class */

  byte pregain;    /* Pregain setting used for
                      search: 0 = 2, 1 = 4, 2 = 8, 3 = 32.               */

  byte integ;      /* Integration period used for search
                      in 4 chip units.                                   */

  byte non_coh;    /* Number of non-coherent integrations accumulated.
                      Value is one less than actual value,
                      e.g., 0 implies 1.                                 */

  uint2 pilot_off; /* Pilot offset used during search.                   */

  int4  win_pos;   /* Absolute window ending position in 1/8 chip units. */

  uint2 win_size;  /* The sweep window size to use when searching for this sector.
                      Units are 1/8 chips to make slew math clear and simple. */


  byte rx_agc;       /* CAGC value                                */

  byte tx_agc;       /* CAGC TX power estimate values (hi 8 bits) */

  byte tx_gain_adj;  /* Power control value                       */

  byte tx_pwr_limit; /* CDMA TX power limit (hi 8 bits)           */


  byte srch_state;   /* Searcher state             */

  log_fwd_ch_rc_type fwd_ch_rc;
                     /* Forward channel Radio Configurations */

  log_ded_fwd_chan_set_type active_channels;
                     /* Active forward channel set */

  byte num_peaks;    /* Number of peaks            */

  byte num_fingers;  /* Number of fingers          */

  PACKED union {
    PACKED struct {
      log_srch_path_info_type peaks[4];  /* Up to 4 peaks, based on 'num_peaks'.
                                            Note: this is a variable length member.
                                            Not all entries will exist in the record. */
      log_srch_fing_info_type fings[12]; /* Up to 12 fingers based on 'num_fingers'.
                                            Note: this is a variable length member.
                                            Not all entries will exist in the record. */
    } max_buf_content;

    byte buf_ref[1]; /* Member used to reference offsets in buffer. */

  } var_len_buf;

}
log_srch_fing_type;


/*---------------------------------------------------------------------------
                ENHANCED FINGER INFORMATION LOG TYPE
---------------------------------------------------------------------------*/

/* The diversity combining algorithm used. */
typedef enum {
  LOG_DIV_ALG_MMSE,   /* Use MMSE algorithm */
  LOG_DIV_ALG_SPW     /* Simple pilot weighting */

} log_div_comb_alg_enum_type;


/* The coefficients of the MMSE correlation matrix */
typedef PACKED struct {
    int2  ul_real;   /* the upper left element. This is real only */

    int2  ur_real;   /* The real part of upper right element */

    int2  ur_img;    /* The img part of upper right element */

    int2  lr_real;   /* The lower right element. This is real only */

} log_mmse_corr_coefs_type;

/* The individual finger information */
typedef PACKED struct {
    uint2 f_pilot;     /* Finger's pilot assignment                       */

    byte  f_sect;      /* Finger's sector in Aset assignment              */

    uint2 f_rssi;      /* For each finger, indicates the current RSSI value.
                       If diversity used this is the combined RSSI        */

    uint2 f_rx0_rssi;  /* In diversity mode, indicates the current RSSI value.
                       for Rx0 only. Valid only for diversity            */

    uint2 f_rx1_rssi;  /* In diversity mode, indicates the current RSSI value.
                       for Rx1 only. Valid only for diversity            */

    int4  f_pos;       /* Absolute finger position                       */

    log_srch_fing_status_type status_mask; /* Finger status */

    byte  reserved1;
    byte  reserved2;

} log_enh_fing_info_type;

/* LOG_ENH_FING_INFO_C */
typedef PACKED struct
{
  log_hdr_type hdr;

  byte srch_state;   /* Searcher state             */

  uint2 channel_num; /* The current channel number */

  byte band_class;   /* Band Class */

  byte ant_conf;     /* use srchdiv_rx_div_enum_type*/

  byte div_comb_alg; /* use srchdiv_rx_div_algo_enum_type */

  byte rx0_agc;       /* CAGC value  for Rx0           */

  byte rx1_agc;       /* CAGC value  for Rx1           */


  log_fwd_ch_rc_type fwd_ch_rc;
                     /* Forward channel Radio Configurations */

  log_ded_fwd_chan_set_type active_channels;
                     /* Active forward channel set */

  log_mmse_corr_coefs_type corr_coefs;
                     /* The MMSE correlation coefficients */

  int2  reserved1;
  int2  reserved2;

  byte num_fingers;  /* Number of fingers          */


  PACKED union {
    PACKED struct {
      log_enh_fing_info_type fings[15];  /* Up to 15 fingers based on 'num_fingers'.
                                            Note: this is a variable length member.
                                            Not all entries will exist in the record. */
    } max_buf_content;

    byte buf_ref[1]; /* Member used to reference offsets in buffer. */

  } var_len_buf;

}
log_enh_fing_type;

/*---------------------------------------------------------------------------
  Service Configuration Log Type
---------------------------------------------------------------------------*/
typedef PACKED struct {
  PACKED struct {
    byte sch_id   : 3; /* SCH ID             */
    byte fwd_rev  : 1; /* 0 == fwd, 1 == rev */
    byte reserved : 4;

  } identifier;

  int16 mux_opt;     /* SCH Mulitplex option */
  byte  coding_type; /* SCH Coding type      */

} log_svc_config_sch_info_type;

/* LOG_SVC_CONFIG_C */
typedef PACKED struct
{
  log_hdr_type hdr;

  uint16 so;         /* Service Option */

  /* Radio Configurations */
  log_fwd_ch_rc_type fwd_ch_rc;
                     /* Forward channel Radio Config Info */
  log_rev_ch_rc_type rev_ch_rc;
                     /* Reverse channel Radio Config Info */

  byte fch_frame_size;   /* FCH frame size  */
  byte dcch_frame_size;  /* DCCH frame size */

  byte fwd_traffic_type; /* Forward link traffic type */
  byte rev_traffic_type; /* Reverse link traffic type */

  int16 fwd_mux_opt;     /* Forward Link Multiplex option */
  int16 rev_mux_opt;     /* Reverse Link Multiplex option */

  byte fwd_rate;         /* Forward transmission rate */
  byte rev_rate;         /* Reverse transmission rate */

  byte num_for_sch;      /* Number of forward supplemental channels */
  byte num_rev_sch;      /* Number of reverse supplemental channels */

  /* The following entries will be ordered in order of active channels:
  ** F-SCH0, F-SCH1, R-SCH0, R-SCH1.  Inactive channels will not be
  ** included in the record.
  */
  log_svc_config_sch_info_type sch_info[4];

}
log_svc_config_type;

/*---------------------------------------------------------------------------
                ACTIVE SET CONFIGURATION LOG TYPE
---------------------------------------------------------------------------*/
/* Per pilot aset configuration information
*/

/* Structure describing per channel information */
typedef PACKED struct {
  uint16 code_chan;   /* Walse Code Channel                */
  byte   qof_mask_id; /* Quasi-Orthagonal Function Mask ID */

} log_aset_config_ch_type;

/* Variable length structure describing per pilot information. */
typedef PACKED struct {
  PACKED struct{
    uint16 pilot_pn;       /* Pilot PN Offset for this pilot */
    byte   pilot_rec_type; /* Pilot record type              */
    byte   reserved;       /* Reserved for future expansion  */

  } pilot_pn_info;

  log_ded_fwd_chan_set_type ch_set_mask; /* Set of active channels */

  byte ch_cfg_cnt; /* Number of entries in the following array. */

  /* Active Set Configuraiton info per channel.
  ** Up to 4 for IS2000, up to 8 for IS95B.
  ** The entries appear in the order of bits in the active
  ** channel mask, from LSB to MSB.
  */
  log_aset_config_ch_type ch_cfg[CAI_MAX_NUM_SUP+1];

} log_aset_config_pilot_type;

/* Rate information for the SCH channels.  This is *not* per pilot.
*/
typedef PACKED struct {
  byte rate       : 4;
  byte r_walsh_id : 1; /* 1 if reverse SCH is covered by walsh code */
  byte reserved   : 3;

} log_aset_config_sch_type;

/* LOG_ASET_CONFIG_C */
typedef PACKED struct
{
  log_hdr_type hdr;

  byte band_class;   /* CDMA band class */
  byte cdma_chan;    /* CDMA channel    */

  byte num_for_sch;  /* # of forward SCH channels in the active set */
  byte num_rev_sch;  /* # of reverse SCH channels in the active set */

  byte num_pilots;   /* Number of pilots in the active set */

  /* Variable length per pilot data for the rest of the packet.
  */
  PACKED union {

    /* Variable length based on num_sch.
    */
    PACKED struct {
      /* Variable length array of SCH rates.  'num_sch' gives # of entries.
      ** The entries appear in the following order:
      ** F-SCH0, F-SCH1, R-SCH0, R-SCH1.
      */
      log_aset_config_sch_type sch_rate[CAI_MAX_NUM_FOR_SCH +
                                        CAI_MAX_NUM_REV_SCH];

      /* Variable length based on 'num_pilots'.
      */
      log_aset_config_pilot_type pilot[6]; /* 6 is the max # of
                                              pilots for now. */
    } sch_and_pilot_buf;

    /* Variable length based on 'num_pilots'.
    ** Note: Each pilot entry is also variable length.
    */
    log_aset_config_pilot_type pilot[6]; /* 6 is the max # of
                                            pilots for now. */

    byte buf_ref[1]; /* Used to reference positions */

  } var_len_buf;

}
log_aset_config_type;

/* FTM Log Type
*/
/* LOG_FTM_C */
typedef PACKED struct
{
  log_hdr_type hdr;

  dword fch_total_frames;       /* total count FCH frames */
  dword fch_fer;                /* total FCH frame error count */
  dword sch_total_frames;       /* total count of SCH frames */
  dword sch_fer;                /* total SCH frame error count */
  dword sch_num_dtx;            /* total SCH DTX count */
  /* Support FEATURE_FTM_DCCH */
  dword dcch_total_frames;      /* total count DCCH frames */
  dword dcch_fer;               /* total DCCH frame error count */
  dword dcch_num_dtx;           /* total DCCH DTX count */
  /* End of FEATURE_FTM_DCCH */
}
log_ftm_type;

/* LOG_FTM_GEN_C */
typedef PACKED struct
{
  log_hdr_type hdr;

  word         ftm_log_id;      /* FTM log id */
  byte         data[1];         /* Variable length payload,
                                   look at FTM log id for contents */
}
log_ftm_gen_type;


/*---------------------------------------------------------------------------
  Test Data Service Option (TDSO) Log Type

  Refer to the following tables in the TDSO standard for more info:
  Table 13, Table 14, Table 15, Table 16, Table 17
---------------------------------------------------------------------------*/
/* LOG_FTDSO_C */
typedef PACKED struct
{
  log_hdr_type hdr;
  uint16 for_mux_opt; /* Forward MUX option */
  uint16 rev_mux_opt; /* REverse MUX option */

  /* Transmit FCH frame counters */
  uint32 tdso_e1_t1;      /* Rate 1 with no signaling                */
  uint32 tdso_e1_td;      /* Rate 1 with dim-and-burst signaling     */
  uint32 tdso_e1_tb;      /* Rate 1 with blank-and-burst signaling   */
  uint32 tdso_eb_tb;      /* Blank                                   */
  uint32 tdso_eb_to;      /* Frame Category other than blank         */

  /* Transmit SCH0 frame counters */
  uint32 tdso_e1x_t1x;   /* 1 x (9.6 or 14.4), depending on SCH MUX option  */
  uint32 tdso_e2x_t2x;   /* 2 x (9.6 or 14.4), depending on SCH MUX option  */
  uint32 tdso_e4x_t4x;   /* 4 x (9.6 or 14.4), depending on SCH MUX option  */
  uint32 tdso_e8x_t8x;   /* 8 x (9.6 or 14.4), depending on SCH MUX option  */
  uint32 tdso_e16x_t16x; /* 16 x (9.6 or 14.4), depending on SCH MUX option */
  uint32 tdso_e1x_tb;    /* 1 x (9.6 or 14.4) expected, blank tx'd instead  */
  uint32 tdso_e2x_tb;    /* 2 x (9.6 or 14.4) expected, blank tx'd instead  */
  uint32 tdso_e4x_tb;    /* 4 x (9.6 or 14.4) expected, blank tx'd instead  */
  uint32 tdso_e8x_tb;    /* 8 x (9.6 or 14.4) expected, blank tx'd instead  */
  uint32 tdso_e16x_tb;   /* 16 x (9.6 or 14.4) expected, blank tx'd instead */
  uint32 tdso_esb_tb;     /* Blank   */

  /* Received Frame counters for Fundicated channel FCH/DCCH              */
  uint32 tdso_e1_r1;    /* Error-free Rate 1 frame with no dim-and-burst  */
  uint32 tdso_e1_rerr;  /* Rate 1 with bit errors                         */
  uint32 tdso_e1_rd;    /* Dim-and-burst frame                            */
  uint32 tdso_e1_ro;    /* Other rate Frame                               */
  uint32 tdso_e1_rb;    /* Blank-and-burst                                */
  uint32 tdso_e1_rfl;   /* Rate 1 physical layer frame with insufficient
                           physical layer frame quality                   */
  uint32 tdso_e1_re;    /* Insufficient frame quality (erasure)           */
  uint32 tdso_en_rn;    /* Null  */
  uint32 tdso_en_rb;    /* Blank */
  uint32 tdso_en_ro;    /* Other */

  /* Received SCH0 frame counters */
  uint32 tdso_e1x_r1x;   /* Error-free 1 x (9.6 or 14.4) frame          */
  uint32 tdso_e2x_r2x;   /* Error-free 2 x (9.6 or 14.4) frame          */
  uint32 tdso_e4x_r4x;   /* Error-free 4 x (9.6 or 14.4) frame          */
  uint32 tdso_e8x_r8x;   /* Error-free 8 x (9.6 or 14.4) frame          */
  uint32 tdso_e16x_r16x; /* Error-free 16 x (9.6 or 14.4) frame         */
  uint32 tdso_e1x_rerr;  /* 1 x (9.6 or 14.4) frame with bit errors     */
  uint32 tdso_e2x_rerr;  /* 2 x (9.6 or 14.4) frame with bit errors     */
  uint32 tdso_e4x_rerr;  /* 4 x (9.6 or 14.4) frame with bit errors     */
  uint32 tdso_e8x_rerr;  /* 8 x (9.6 or 14.4) frame with bit errors     */
  uint32 tdso_e16x_rerr; /* 16 x (9.6 or 14.4) frame with bit errors    */
  uint32 tdso_e1x_re;    /* Expected 1 x (9.6 or 14.4), erasure         */
  uint32 tdso_e2x_re;    /* Expected 2 x (9.6 or 14.4), erasure         */
  uint32 tdso_e4x_re;    /* Expected 4 x (9.6 or 14.4), erasure         */
  uint32 tdso_e8x_re;    /* Expected 8 x (9.6 or 14.4), erasure         */
  uint32 tdso_e16x_re;   /* Expected 16 x (9.6 or 14.4), erasure        */
  uint32 tdso_e1x_rb;    /* 1 x (9.6 or 14.4) expected, blank received  */
  uint32 tdso_e2x_rb;    /* 2 x (9.6 or 14.4) expected, blank received  */
  uint32 tdso_e4x_rb;    /* 4 x (9.6 or 14.4) expected, blank received  */
  uint32 tdso_e8x_rb;    /* 8 x (9.6 or 14.4) expected, blank received  */
  uint32 tdso_e16x_rb;   /* 16 x (9.6 or 14.4) expected, blank received */
  uint32 tdso_esb_rb;     /* Blank                                       */
  uint32 tdso_esb_ro;     /* Frame category other than blank             */


  /* Received PDU counters for the Supplemental Channels                */
  uint32 tdso_e3_r3;    /* Error-free Rate 3 MuxPDU                     */
  uint32 tdso_e3_rerr;  /* Rate 3 MuxPDU with Errors                    */
  uint32 tdso_e3_re;    /* Insufficient Frame Quality, Erasures         */
  uint32 tdso_e2_r2;    /* Error-free Rate 2 MuxPDU                     */
  uint32 tdso_e2_rerr;  /* Rate 2 MuxPDU with Errors                    */
  uint32 tdso_e2_re;    /* Insufficient Frame Quality, Erasures         */
  uint32 tdso_e1a_r1a;   /* Error-free Rate 1a MuxPDU                    */
  uint32 tdso_e1a_rerr; /* Rate 1a MuxPDU with Errors                   */
  uint32 tdso_e1a_re;   /* Insufficient Frame Quality, Erasures         */
  uint32 tdso_e1b_r1b;   /* Error-free Rate 1b MuxPDU                    */
  uint32 tdso_e1b_rerr; /* Rate 1a MuxPDU with Errors                   */
  uint32 tdso_e1b_re;   /* Insufficient Frame Quality, Erasures         */

}
log_ftdso_type;

/*---------------------------------------------------------------------------
  Simple Test Data Service Option (TDSO) Log Type
---------------------------------------------------------------------------*/
/* LOG_STDSO_C */
typedef PACKED struct
{
  log_hdr_type hdr;
  uint16 for_mux_opt; /* Forward MUX option */
  uint16 rev_mux_opt; /* REverse MUX option */

  /* Transmit FCH frame counters */
  uint32 e1_t1;      /* Rate 1 with no signaling                */
  uint32 e2_t2;      /* Rate 1/2 with no signaling              */
  uint32 e4_t4;      /* Rate 1/4 with no signaling              */
  uint32 e8_t8;      /* Rate 1/8 with no signaling              */
  uint32 e1_td;      /* Rate 1 with dim-and-burst signaling     */
  uint32 e2_td;      /* Rate 1/2 with dim-and-burst signaling   */
  uint32 e4_td;      /* Rate 1/4 with dim-and-burst signaling   */
  uint32 e8_td;      /* Rate 1/8 with dim-and-burst signaling   */
  uint32 e1_tb;      /* Rate 1 with blank-and-burst signaling   */
  uint32 e2_tb;      /* Rate 1/2 with blank-and-burst signaling */
  uint32 e4_tb;      /* Rate 1/4 with blank-and-burst signaling */
  uint32 e8_tb;      /* Rate 1/8 with blank-and-burst signaling */
  uint32 eb_tb;      /* Blank                                   */
  uint32 eb_to;      /* Frame category other than blank         */

  /* Transmit SCH0 frame counters */
  uint32 es1x_t1x;   /* 1 x (9.6 or 14.4), depending on SCH MUX option  */
  uint32 es2x_t2x;   /* 2 x (9.6 or 14.4), depending on SCH MUX option  */
  uint32 es4x_t4x;   /* 4 x (9.6 or 14.4), depending on SCH MUX option  */
  uint32 es8x_t8x;   /* 8 x (9.6 or 14.4), depending on SCH MUX option  */
  uint32 es16x_t16x; /* 16 x (9.6 or 14.4), depending on SCH MUX option */
  uint32 es1x_tb;    /* 1 x (9.6 or 14.4) expected, blank tx'd instead  */
  uint32 es2x_tb;    /* 2 x (9.6 or 14.4) expected, blank tx'd instead  */
  uint32 es4x_tb;    /* 4 x (9.6 or 14.4) expected, blank tx'd instead  */
  uint32 es8x_tb;    /* 8 x (9.6 or 14.4) expected, blank tx'd instead  */
  uint32 es16x_tb;   /* 16 x (9.6 or 14.4) expected, blank tx'd instead */
  uint32 esb_tb;     /* Blank                                           */

  /* Received FCH frame counters */
  uint32 e1_r1;      /* Error-free Rate 1 frame with no dim-and-burst         */
  uint32 e1_rerr;    /* Rate 1 with bit errors detected by the service option */
  uint32 e1_rd;      /* Dim-and-burst frame                                   */
  uint32 e1_rb;      /* Blank-and-burst                                       */
  uint32 e1_r2;      /* Rate 1/2 frame with no dim-and-burst                  */
  uint32 e1_r4;      /* Rate 1/4 frame with no dim-and-burst                  */
  uint32 e1_r8;      /* Rate 1/8 frame with no dim-and-burst                  */
  uint32 e1_rfl;     /* Rate 1 physical layer frame with insufficient
                        physical layer frame quality                          */
  uint32 e1_re;      /* Insufficient frame quality (erasure)                  */
  uint32 e2_r2;      /* Error-free Rate 1/2 frame with no dim-and-burst       */
  uint32 e2_rerr;    /* Rate 1 with bit errors detected by the service option */
  uint32 e2_rd;      /* Dim-and-burst frame                                   */
  uint32 e2_rb;      /* Blank-and-burst                                       */
  uint32 e2_r1;      /* Rate 1 frame with no dim-and-burst                    */
  uint32 e2_r4;      /* Rate 1/4 frame with no dim-and-burst                  */
  uint32 e2_r8;      /* Rate 1/8 frame with no dim-and-burst                  */
  uint32 e2_rfl;     /* Rate 1 physical layer frame with insufficient
                        physical layer frame quality                          */
  uint32 e2_re;      /* Insufficient frame quality (erasure)                  */
  uint32 e4_r4;      /* Error-free Rate 1/4 frame with no dim-and-burst       */
  uint32 e4_rerr;    /* Rate 1 with bit errors detected by the service option */
  uint32 e4_rd;      /* Dim-and-burst frame                                   */
  uint32 e4_rb;      /* Blank-and-burst                                       */
  uint32 e4_r1;      /* Rate 1 frame with no dim-and-burst                    */
  uint32 e4_r2;      /* Rate 1/2 frame with no dim-and-burst                  */
  uint32 e4_r8;      /* Rate 1/8 frame with no dim-and-burst                  */
  uint32 e4_rfl;     /* Rate 1 physical layer frame with insufficient
                        physical layer frame quality                          */
  uint32 e4_re;      /* Insufficient frame quality (erasure)                  */
  uint32 e8_r8;      /* Error-free Rate 1/8 frame with no dim-and-burst       */
  uint32 e8_rerr;    /* Rate 1 with bit errors detected by the service option */
  uint32 e8_rd;      /* Dim-and-burst frame                                   */
  uint32 e8_rb;      /* Blank-and-burst                                       */
  uint32 e8_r1;      /* Rate 1 frame with no dim-and-burst                    */
  uint32 e8_r2;      /* Rate 1/2 frame with no dim-and-burst                  */
  uint32 e8_r4;      /* Rate 1/4 frame with no dim-and-burst                  */
  uint32 e8_rfl;     /* Rate 1 physical layer frame with insufficient
                        physical layer frame quality                          */
  uint32 e8_re;      /* Insufficient frame quality (erasure)                  */
  uint32 en_rn;      /* Null  */
  uint32 en_rb;      /* Blank */
  uint32 en_ro;      /* Other */

  /* Received SCH0 frame counters */
  uint32 es1x_r1x;   /* Error-free 1 x (9.6 or 14.4) frame          */
  uint32 es2x_r2x;   /* Error-free 2 x (9.6 or 14.4) frame          */
  uint32 es4x_r4x;   /* Error-free 4 x (9.6 or 14.4) frame          */
  uint32 es8x_r8x;   /* Error-free 8 x (9.6 or 14.4) frame          */
  uint32 es16x_r16x; /* Error-free 16 x (9.6 or 14.4) frame         */
  uint32 es1x_rerr;  /* 1 x (9.6 or 14.4) frame with bit errors     */
  uint32 es2x_rerr;  /* 2 x (9.6 or 14.4) frame with bit errors     */
  uint32 es4x_rerr;  /* 4 x (9.6 or 14.4) frame with bit errors     */
  uint32 es8x_rerr;  /* 8 x (9.6 or 14.4) frame with bit errors     */
  uint32 es16x_rerr; /* 16 x (9.6 or 14.4) frame with bit errors    */
  uint32 es1x_re;    /* Expected 1 x (9.6 or 14.4), erasure         */
  uint32 es2x_re;    /* Expected 2 x (9.6 or 14.4), erasure         */
  uint32 es4x_re;    /* Expected 4 x (9.6 or 14.4), erasure         */
  uint32 es8x_re;    /* Expected 8 x (9.6 or 14.4), erasure         */
  uint32 es16x_re;   /* Expected 16 x (9.6 or 14.4), erasure        */
  uint32 es1x_rb;    /* 1 x (9.6 or 14.4) expected, blank received  */
  uint32 es2x_rb;    /* 2 x (9.6 or 14.4) expected, blank received  */
  uint32 es4x_rb;    /* 4 x (9.6 or 14.4) expected, blank received  */
  uint32 es8x_rb;    /* 8 x (9.6 or 14.4) expected, blank received  */
  uint32 es16x_rb;   /* 16 x (9.6 or 14.4) expected, blank received */
  uint32 esb_rb;     /* Blank                                       */
  uint32 esb_ro;     /* Frame category other than blank             */
}
log_stdso_type;

/*---------------------------------------------------------------------------
                 1X ADVANCED INFORMATION LOG TYPE
---------------------------------------------------------------------------*/
typedef PACKED struct
{
  uint32 buff[192];
} log_1xa_mdsp_info_type;

/* LOG_1X_ADV_INFO_C */
typedef PACKED struct
{
  log_hdr_type hdr;

  uint32  frame_num_with_offset;
  uint8   band_class;
  uint16  channel;

  /*RCPM DATA*/
  PACKED struct
  {
    uint16 for_fch_ack_mask_rl_blanking;
    uint16 for_fch_ack_mask_no_rl_blanking;
    uint16 rev_fch_ack_mask;
    uint16 for_sch_ack_mask_rl_blanking;
    uint16 for_sch_ack_mask_no_rl_blanking;
    uint16 rev_sch_ack_mask;
    uint8  for_n2m_ind;
    uint8  power_control_mode;
    uint8  power_control_step;
    uint8  for_fch_blanking_dutycycle;
    uint8  rev_fch_blanking_dutycycle;
    uint8  rev_gain_adj_acs1;
    uint8  rev_gain_adj_acs2plus;
  } rcpm;

  /*TX STATS*/
  PACKED struct
  {
    uint8 dtx_non_crit_fch_frame;
    uint8 pwr_ctrl_step_size;
    uint8 pwr_ctrl_mode;
    uint8 rate;
  }rc8_data;

  /*MDSP LOG*/
  uint16 num_entries;
  log_1xa_mdsp_info_type mdsp_buff;
}
log_1x_adv_info_type;

/*-------------------------------------------------------------------------*/

/* Union of All Log Entry Types
*/
typedef PACKED union
{
  log_hdr_type             hdr;       /* Generic header.                     */
}
log_type;

#endif /* LOG_DMSS_H */
