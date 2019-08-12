#ifndef SRCHLOG_H
#define SRCHLOG_H

/*===========================================================================
                              S R C H L O G . H

DESCRIPTION
This file contains or includes files that contain Layer 1 searcher log packet
structure definitions, prototypes for Layer 1 seacher logging functions, any
constant definitions that are needed, and any external variable declarations
needed for Layer 1 searcher logging.


    Copyright (c) 1999-2010 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/srchlog.h_v   1.17   18 Jun 2002 15:55:40   yshi  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/wsrchlog.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
10/20/15    vn     Changes for Step1 Peak Pruner algorithm.
01/05/14    vn      Log packet changes for step1, NASTT and PN searches for Jolokia.
11/19/14    gsk     FACH FMO log packet and adding support for 0x4027 for FE-FACH
11/14/14    jd      Changes to include rf tune info for step1 and nastt log packet.
10/22/14    jd      Changes to include uarfcn in pn search log packet.
08/12/14    vn      Defining 0x41BC log packet for Narrow Band Rejection algorithm
06/24/14    mk      Support for changing EARFCN variable type from uint16 to uint32.
05/05/14    rs      FR-3672 CTCH log packet changes. 
02/14/13    km      Featurize FEATURE_FATPATH_IN_DCH related variables correctly
02/14/14    bj      Reverting UARFCN print changes in 0x4179 WCDMA PN Search log packet version-2
02/07/14    bj      Added uarfcn info in SRCH_PN_LOG_PACKET version-2
01/22/14    gsk     Ngbr srch opt log packet changes
12/17/13    gj      3C searcher logging changes
09/12/13    jd      Searcher redesign
07/23/13    jd      parallel step1 changes
05/23/13    geg     WSW decoupling (except for component of QCHAT)
03/27/13    vn      Defining 0x41B0 log packet to support frequency scan. 
02/19/13    ar      Defining 0x41AE (CSG ASF Meas Req) and 0x41AF (CSG cell resel) log packets
09/24/12    zr      Defining 0x41AC log packet to add dual carrier support and replace 0x4110
08/14/12    zr      Adding log packets to reduce F3 logging from searcher
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
07/02/12    cc      A new log packet 0x4722 for W2G FACH measurement results 
03/14/12    zr      Added PN bypass peak info log packet
12/21/11    zr      Deprecated log packet 0x4005 and added its fields to 0x4027 when SIB 19 is present
06/17/11    sa      Added code to support list search logging for all cells
                    after every 1 sec.
04/28/11    sj      upmerge from offline/main
04/06/11    mr      Added support for absolute priority cell Reselection log packet (0x4027)
08/30/10    sv      Support for PN LSW and filter RSCP logging.
02/26/10    rvs     Fixes for Mustang.
02/12/10    sv      Logging support for PN search posLSW per task.
02/09/10    sv      Added support for 0x4178 NASTT log packet.
02/08/10    sv      Fixed mismatched #if/#endif
02/06/10    sv      Mainlined FEATURE_WCDMA_OFFLINE_SEARCHER
01/05/10    mr      Updated "num_peaks_req" field in Step1 log packet 
11/23/09    nd      Removed the SRCH_PN_LOG_PACKET definition
11/23/09    hk      PN Srch Log packet changes for DC
09/22/09    sup     Removed log code assignments and placed them in 
                    log_codes_wcdma_int_v.h
10/03/08    hk      Bringing in 7k Mailine fixes
09/17/08    sv      Fixed PN packet logging when in RxD and shoulder searches.
09/05/08    sv      Fixed the incorrect computation of PN search log length.
06/10/08    ms      Added PACKED_POST attribute for structures to make it
                    compatible for GNU compiler.
10/26/07    sv      Added support for searcher MBMS list search logging under 
                    FEATURE_SRCH_MBMS_LIST_LOG_4170.
03/07/07    nd      Added a new log packet PSC scanner Pilot Info 
11/29/06    nd      Added multiple layer3 filter support for intra freq measurement
                    under FEATURE_WCDMA_MULTIPLE_L3_FILTERS_SUPPORTED
08/08/06    sv      Renamed feature FEATURE_WCDMA_SRCH_RXDIV_LOGGING to 
                    FEATURE_SRCH_ANT_BIT_LOGGING.
06/30/06    sv      Redefined log codes for list and Step1,2,3 searches when 
                    RxD feature is defined.
05/05/06    sv      Replace previous 4 RxDiv logging feature defines with 1.
                    Also fixed incorrect lengths for new log packets.
04/17/06    sv      Changes to support Rx Diversity search filtering.
07/11/05    sk      Put new list search log pkt under FEATURE_LIST_LOG_413F
06/15/05    sk      Redefined List search log packet to 0x413F
05/27/05    sk      Removed and mainlined FEATURE_CELL_RESELECTION_DUAL_SYS_LOG
01/25/05    yiz     Fix feature name around FACH_INTER_RAT_MEAS_LOG struct.
12/03/04    yiz     Add FACH meas oksn info and inter-RAT meas log packets.
                    Use existing log_hdr_type.
07/09/03    yiz     Modified CR rank log packet definition with new code 0x4005
                    to have WCDMA and GSM cells simultaneously in the packet.
02/26/03    sh      Added log packet definition for cell reselection rank.
08/12/02    cpe     Completetly revamped search packets based on the new 
                    ICD document. 
06/18/02   yus/cpe Added filt_eng in srchlog_list_search_task_type.
06/12/02    cpe    Changed the field size of eng in srchlog_list_search_task_type
		               from uint16 to uint32
05/29/02    m      Added support for LOD of ASET and NSET by introducing new
                   types to save a snapshot of the ASET and NSET data
05/17/02    cpe    Updated the new list search log packet number
02/14/02    cpe    Merged the 0x412E (WCDMA reference position) packet into
                   ASET and NSET packets and removed 0x41E packet.
09/10/01    cpe    Removed the field ecio from the NSET log packet.
08/08/01    cpe    Added log.h to the includes
06/27/01    cpe    Reduced size of psc_pos from uint32 to uint16 in step2 packet
                   Modified the macro to calculate packet size accordingly
06/12/01    cpe    Added Nc (coherent) and Nn (non-coherent) integration
                   length fields to step1 and step2 packets
                   Added macros to calculate size of each of these packets     
04/22/2001  cpe    added srchdrv.h to the list of included files
04/20/2001  cpe    modified nset update packet to include num_freq 
                   as well as num of cells
                   Also some changes to adhere to variable naming 
                   convention.
                   Changed LOG_WCDMA_L1_BASE_C to LOG_WCDMA_BASE_C

04/11/2001  cpe    Added definitions for aset update and nset update 
                   packets

01/24/2001  cpe    initial cut                         


===========================================================================*/

#include "wcdma_variation.h"
#include "log.h"
#include "log_codes.h"
#include "log_codes_wcdma_int.h"
#include "srchparm.h"
#ifdef FEATURE_WCDMA_L1_PSC_SCANNER
#include "wsrchmobileview.h"
#endif

/* LOG_WCDMA_BASE_C has been defined in log_codes.h */

/*--------------------------------------------------------------------------
                 LOG PACKET: SRCH_STEP1_LOG_PACKET
                     
This structure is use to log search step 1 information. These packets occur
upon every initial acquisition and upon receiving a new neighbor list.          
--------------------------------------------------------------------------*/

/* DC_HSDPA carrier info */
#ifdef FEATURE_DC_HSDPA_LOGGING
typedef PACKED struct PACKED_POST {
    
    /* Instantaneous Rx AGC*/
    int16 rx_CARR0_agc_0;
    int16 rx_CARR0_agc_1;
    int16 rx_CARR1_agc_0; 
    int16 rx_CARR1_agc_1; 
   //3C logging
#ifdef FEATURE_WCDMA_3C_HSDPA
    int16 rx_CARR2_agc_0;
    int16 rx_CARR2_agc_1;
#else
#ifdef FEATURE_WCDMA_PN_PACKET_VER4
    int16 dummy_rx_CARR2_agc_0;
    int16 dummy_rx_CARR2_agc_1;
#endif
#endif /* FEATURE_WCDMA_3C_HSDPA */
   /* Internal use only 
    * Num_freq_hyp. Bits [2-0] 
    * ss_freeze_info. Bits [5-3]
    * ss_use_freeze_info. Bits [7-6]
    */
   uint8 ss_info;
    
} srchlog_dc_hsdpa_carr_info_struct_type;
#endif

/* the log code for the seach step 1 packet */
#ifndef SRCH_STEP1_LOG_PACKET
#if defined(FEATURE_DC_HSDPA_LOGGING)
#define SRCH_STEP1_LOG_PACKET SRCH_STEP1_0x4177_LOG_PACKET
#define SRCH_PARALLEL_STEP1_LOG_PACKET SRCH_STEP1_0x4177_LOG_PACKET
#else 
#define SRCH_STEP1_LOG_PACKET SRCH_STEP1_0x4183_LOG_PACKET
#endif
#else
#error redefinition of LOG command code: SRCH_STEP1_LOG_PACKET
#endif

/* Maximum number of rows in the step 1 log packet table */
#define SRCH_STEP1_LOG_PACKET_MAX_LEN 16

/* information for each peak returned */
typedef PACKED struct PACKED_POST {
    /* Primary Scrambling code PSC position in chipx8 resolution. 
       Range :  0....(8x2560 - 1)          */
    uint16 pos ;

    /* energy level for the peak */
    uint16 eng ;
    /* Frequency hypothesis index used for this peak */
    uint8 freq_hypo_index ;
} srchlog_psc_peak_struct_type;
 
/* information for each peak returned */
typedef PACKED struct PACKED_POST {
    /* Primary Scrambling code PSC position in chipx8 resolution. 
       Range :  0....(8x2560 - 1)          */
    uint16 pos ;

    /* energy level for the peak */
    uint16 eng ;
    /* Frequency hypothesis index used for this peak */
    uint8 freq_hypo_index ;
	/* Carrier index of the peak */
	uint8 carr_idx;

#ifdef FEATURE_WCDMA_STEP1_PEAK_PRUNER
    /* boolean indicating whether this peak will be pruned or not */
    boolean to_be_pruned;
#endif
} srchlog_psc_peak_parallel_step1_struct_type;

 
#if defined(FEATURE_DC_HSDPA_LOGGING)
LOG_RECORD_DEFINE(SRCH_STEP1_LOG_PACKET)
 /* version field */
  uint8 version;
  
  uint8 carrier_type;
  srchlog_dc_hsdpa_carr_info_struct_type rx_ss_freeze_info;

  /* Frequency hypothesis */
  int16 freq_offset[MCALWCDMA_SRCH_STEP1_NUM_FREQ_HYPO];

 /* Non-coherent integration length*/
  uint8 nn;

 /* Num of peaks */
  uint8 num_peaks_req;  
  
  /* 
   * ANTENNA_RXD_INFO	1	This field has the rxd mode and the antenna used information.  Bits [1-0]
   * 0 - Primary antenna and rxd_mode 0
   * 1 - Secondary Antenna and rxd_mode 0
   * 2 - Both antennas in rxd add mode
   * 3 - Both antennas in rxd separate mode
   * PD_MODE
   * ( Reserved ) Peak Detect Mode. Bits [3-2]
   * 0 - Normal Mode
   * 1 - Bypass
   * 2 - Shoulder  
   * 7:4 - Reserved
   */
  uint8 step1_config;

  /* Energy Threshold for step1 peaks */
  uint16 eng_thr; 
  
  /* Num of peaks */
  uint8 num_peaks_primary;

  /* Num of peaks on secondary antenna */
  uint8 num_peaks_sec;
  
  /* the actual peak log records */
  srchlog_psc_peak_struct_type srchlog_psc_peak_info[SRCH_MAX_PSC_PEAKS];

LOG_RECORD_END
#else
LOG_RECORD_DEFINE(SRCH_STEP1_LOG_PACKET)

  /* Instantaneous Rx AGC on Primary Antenna*/
  int16 rx_agc_0;

  /* Instantaneous Rx AGC on Secondary Antenna */
  int16 rx_agc_1; 
  
  /* Non-coherent integration length*/
  uint8 nn;
  
  /* Num of peaks */
  uint8 num_peaks_req;   

  /* 
   * ANTENNA_RXD_INFO	1	This field has the rxd mode and the antenna used information.  Bits [1-0]
   * 0 - Primary antenna and rxd_mode 0
   * 1 - Secondary Antenna and rxd_mode 0
   * 2 - Both antennas in rxd add mode
   * 3 - Both antennas in rxd separate mode
   * PD_MODE
   * ( Reserved ) Peak Detect Mode. Bits [3-2]
   * 0 - Normal Mode
   * 1 - Bypass
   * 2 - Shoulder  
   * FA_ENABLE
   * ( Reserved ) False Alarm reduction. Bits[4]
   * 0 - Disabled
   * 1 - Enabled
   * 2 - Shoulder  
   * STTD_MODE
   * ( Reserved ) STTD Mode. Bits[5]
   * 0 - Disabled
   * 1 - Enabled  
   */
  uint8 step1_config;

  /* Energy Threshold for step1 peaks */
  uint16 eng_thr;   

  /* Frequency hypothesis */
  int16 freq_offset[MCALWCDMA_SRCH_STEP1_NUM_FREQ_HYPO] ;

  /* Num of peaks */
  uint8 num_peaks;

  /* Num of peaks on secondary antenna */
  uint8 num_peaks_sec;
  
  /* the actual peak log records */
  srchlog_psc_peak_struct_type srchlog_psc_peak_info[SRCH_MAX_PSC_PEAKS];

LOG_RECORD_END
#endif

/**************************************************************************
LOG PACKET 0x4177 VERSION-3 FOR PARALLEL STEP1 SEARCH
**************************************************************************/

#if defined(FEATURE_DC_HSDPA_LOGGING)
  LOG_RECORD_DEFINE(SRCH_PARALLEL_STEP1_LOG_PACKET)
  /* version field */
  uint8 version;

  /*Ecio Conversion Table version. 0 is the default value for pre JO targets*/
  uint8 ecio_conv_tbl_ver;

  uint8 carrier_type;

#ifdef FEATURE_WCDMA_PN_PACKET_VER4
   /* RF antenna tune information*/
  uint8 rf_tune_info;
#endif

  srchlog_dc_hsdpa_carr_info_struct_type rx_ss_freeze_info;
	
  /* Frequency hypothesis */
  int16 freq_offset[MCALWCDMA_SRCH_STEP1_NUM_FREQ_HYPO];
	
  /* Non-coherent integration length*/
  uint8 nn;
	
  /* Num of peaks */
  uint8 num_peaks_req;	
	  
  /* 
     * ANTENNA_RXD_INFO	1	This field has the rxd mode and the antenna used information.  Bits [1-0]
     * 0 - Primary antenna and rxd_mode 0
     * 1 - Secondary Antenna and rxd_mode 0
     * 2 - Both antennas in rxd add mode
     * 3 - Both antennas in rxd separate mode
     * PD_MODE
     * ( Reserved ) Peak Detect Mode. Bits [3-2]
     * 0 - Normal Mode
	   * 1 - Bypass
	   * 2 - Shoulder  
	   * 7:4 - Reserved
	   */
   uint8 step1_config;
	
   /* Energy Threshold for step1 peaks */
   uint16 eng_thr; 
	  
   /* Num of peaks */
   uint8 num_peaks_primary;
	
   /* Num of peaks on secondary antenna */
   uint8 num_peaks_sec;
	  
#ifdef FEATURE_WCDMA_STEP1_PEAK_PRUNER
   /* Step1 peak pruner enabled */
   boolean s1_pper_en;

   /* NAP enabled */
   boolean nastt_skip_nap;

   /* Full search period on C0 */
   uint16 fs_period;
#endif	  
   /* the actual peak log records */
   srchlog_psc_peak_parallel_step1_struct_type srchlog_psc_peak_info[SRCH_MAX_PSC_PEAKS];
	
LOG_RECORD_END
#endif

/*--------------------------------------------------------------------------
                 LOG PACKET: SRCH_NAS_TT_LOG_PACKET
                     
This structure is use to log search nas tt information. These packets occur
upon every initial acquisition and upon receiving a new neighbor list.          
--------------------------------------------------------------------------*/

/* the log code for the seach NASTT packet */
#ifndef SRCH_NAS_TT_LOG_PACKET
#if defined(FEATURE_DC_HSDPA_LOGGING)
#define SRCH_NAS_TT_LOG_PACKET SRCH_NASTT_0x4178_LOG_PACKET
#define SRCH_PARALLEL_NAS_TT_LOG_PACKET SRCH_NASTT_0x4178_LOG_PACKET
#else 
#define SRCH_NAS_TT_LOG_PACKET SRCH_NASTT_0x4184_LOG_PACKET
#endif
#else
#error redefinition of LOG command code: SRCH_NAS_TT_LOG_PACKET
#endif

/* information for each peak returned */
typedef PACKED struct PACKED_POST 
{

  /* Scrambling code index */
  uint16 psc;

  /* peak position returned by NAS-TT in cx8 */
  uint32 pos;

  /* non-coherent accumulation energy, if the energy is 0, it's an invalid peak */
  uint16 eng;

} srchlog_nastt_rslt_struct_type;

#if defined(FEATURE_DC_HSDPA_LOGGING)
LOG_RECORD_DEFINE(SRCH_NAS_TT_LOG_PACKET)
 /* version field */
  uint8 version;
  
  uint8 carrier_type;
  srchlog_dc_hsdpa_carr_info_struct_type rx_ss_freeze_info;

  /* Frequency hypothesis */
  int16 freq_offset;

  /* Non-coherent integration length*/
  uint8 nn;

  /* Coherent integration length*/
  uint16 nc;

  /* 
   * ANTENNA_RXD_INFO This field has the rxd mode and the antenna used information.  Bits [1-0]
   * 0 – Primary antenna and rxd_mode 0
   * 1 – Secondary Antenna and rxd_mode 0
   * 2 – Both antennas in rxd add mode
   * 3 – Both antennas in rxd separate mode
   * ASYNC		Type of NAS-TT search. Bits[2]
   * 0 –  Unmonitored Set search
   * 1 – Async search
   * PD_MODE
   * Peak Detect Mode. Bits [4-3]
   * 0 – Normal Mode
   * 1 – Bypass
   * 2 – Shoulder  
   * STTD_MODE
   * STTD Mode. Bits[5]
   * 0 – Disabled
   * 1 – Enabled   
   */
  uint8 nastt_config;

  /* Energy Threshold */
  uint16 eng_thr;

  /* Num of peaks */
  uint8 num_input_tasks;

  /* Step1 peak position */
  uint16 peak_pos[12];

  /* Num of peaks */
  uint8 num_tasks_finished;

  /* Num of peaks */
  uint8 num_results_per_tasks;

  srchlog_nastt_rslt_struct_type rslt[12*4];   

LOG_RECORD_END
#else
LOG_RECORD_DEFINE(SRCH_NAS_TT_LOG_PACKET)
 
  /* Instantaneous Rx AGC on Primary Antenna*/
  int16 rx_agc_0; 

  /* Instantaneous Rx AGC on Secondary Antenna */
  int16 rx_agc_1; 

  /* Non-coherent integration length*/
  uint8 nn;

  /* Coherent integration length*/
  uint16 nc;

  /* 
   * ANTENNA_RXD_INFO This field has the rxd mode and the antenna used information.  Bits [1-0]
   * 0 – Primary antenna and rxd_mode 0
   * 1 – Secondary Antenna and rxd_mode 0
   * 2 – Both antennas in rxd add mode
   * 3 – Both antennas in rxd separate mode
   * ASYNC		Type of NAS-TT search. Bits[2]
   * 0 –  Unmonitored Set search
   * 1 – Async search
   * PD_MODE
   * Peak Detect Mode. Bits [4-3]
   * 0 – Normal Mode
   * 1 – Bypass
   * 2 – Shoulder  
   * STTD_MODE
   * STTD Mode. Bits[5]
   * 0 – Disabled
   * 1 – Enabled   
   */
  uint8 nastt_config;

  /* Energy Threshold */
  uint16 eng_thr;

  /* Number of scrambling codes used for this NAS-TT search */
  uint8 num_sc_index_req;
  
  /* The scrambling codes used for this NAS-TT search */
  uint16 SC_INDEX[32]; 
  
  /* Channelization code index */
  uint8 CHAN_CODE[32]; 
  
  /* Num of peaks */
  uint8 num_task;
  
  /* Step1 peak position */
  uint16 peak_pos[12];

  srchlog_nastt_rslt_struct_type rslt[12*4];   

LOG_RECORD_END
#endif

/*-------------------------------------------------------------------------
  Parallel nastt log packet
-------------------------------------------------------------------------*/

#if defined(FEATURE_DC_HSDPA_LOGGING)
LOG_RECORD_DEFINE(SRCH_PARALLEL_NAS_TT_LOG_PACKET)
 /* version field */
  uint8 version;
    /*Ecio Conversion Table version. 0 is the default value for pre JO targets*/
  uint8 ecio_conv_tbl_ver;
  uint8 carrier_type;

#ifdef FEATURE_WCDMA_PN_PACKET_VER4
   /* RF antenna tune information*/
  uint8 rf_tune_info;
#endif

  srchlog_dc_hsdpa_carr_info_struct_type rx_ss_freeze_info;

  /* Frequency hypothesis */
  int16 freq_offset;

  /* Non-coherent integration length*/
  uint8 nn;

  /* Coherent integration length*/
  uint16 nc;

  /* 
   * ANTENNA_RXD_INFO This field has the rxd mode and the antenna used information.  Bits [1-0]
   * 0 – Primary antenna and rxd_mode 0
   * 1 – Secondary Antenna and rxd_mode 0
   * 2 – Both antennas in rxd add mode
   * 3 – Both antennas in rxd separate mode
   * ASYNC		Type of NAS-TT search. Bits[2]
   * 0 –  Unmonitored Set search
   * 1 – Async search
   * PD_MODE
   * Peak Detect Mode. Bits [4-3]
   * 0 – Normal Mode
   * 1 – Bypass
   * 2 – Shoulder  
   * STTD_MODE
   * STTD Mode. Bits[5]
   * 0 – Disabled
   * 1 – Enabled   
   */
  uint8 nastt_config;

  /* Energy Threshold */
  uint16 eng_thr;

  /* Num of peaks */
  uint8 num_input_tasks;

  /* Step1 peak position */
  uint16 peak_pos[12];

  /* Num of peaks */
  uint8 num_tasks_finished;

  /* Num of peaks */
  uint8 num_results_per_tasks;

  /* Carrier index of each task */
  uint8 carr_idx[6];
  
  srchlog_nastt_rslt_struct_type rslt[12*4];   


LOG_RECORD_END
#endif

/*--------------------------------------------------------------------------
                 LOG PACKET: SRCH_PN_LOG_PACKET
                     
This structure is use to log search nas tt information. These packets occur
upon every initial acquisition and upon receiving a new neighbor list.          
--------------------------------------------------------------------------*/

/* the log code for the seach NASTT packet */
#ifndef SRCH_PN_LOG_PACKET
#if defined(FEATURE_DC_HSDPA_LOGGING)
    #define SRCH_PN_BYPASS_PEAK_LOG_PACKET SRCH_PN_BYPASS_INFO_LOG_PACKET
#define SRCH_PN_LOG_PACKET SRCH_PN_0x4179_LOG_PACKET
#else 
#define SRCH_PN_LOG_PACKET SRCH_PN_0x4185_LOG_PACKET
#endif
#else
#error redefinition of LOG command code: SRCH_PN_LOG_PACKET
#endif
/* log pkt to print all searches cells after every 1 sec */
#ifndef SRCH_LIST_ALL_CELLS_LOG_PACKET
#define SRCH_LIST_ALL_CELLS_LOG_PACKET  SRCH_ALL_CELLS_PN_0x417A_LOG_PACKET
#else
#error redefinition of LOG command code: SRCH_LIST_ALL_CELLS_LOG_PACKET
#endif

#ifdef FEATURE_DC_HSDPA_LOGGING

/* information for each input task */
typedef PACKED struct PACKED_POST
{
  /* Coherent integration duration */
  uint8 nc; 
  
  /* Noncoherent integration duration */
  uint8 nn; 

  /*  PN Config byte
     ================
     CARRIER_INFO: Bit 0
      0 - Primary carrier 
      1 - Secondary carrier
   
     ANTENNA_RXD_INFO: Bits [2:1]
      0 (2'b00) - Primary antenna on, no RxD
      1 (2'b01) - Secondary antenna on, no RxD 
      2 (2'b10) - Both antennae in RxD add mode 
      3 (2'b11) - Both antennae in RxD separate mode
   
     Bits [7:3] are reserved for future use
   */
  uint8 pn_config;
  
  /* Bits [12:0] the scrambling code used for this PN search, 0-8176 
     Bits [15:13} are reserved for future use */
  uint16 scr_code; 
} srchlog_pn_bypass_peak_input_struct_type;

/* Structure that holds a set of 12 peaks per result */
typedef PACKED struct PACKED_POST
{
  uint32 peak_pos[SRCH_PN_NUM_RESULT_PER_TASK_FAT_PATH];
  uint32 peak_eng[SRCH_PN_NUM_RESULT_PER_TASK_FAT_PATH];
} srchlog_pn_bypass_peak_set_struct_type;

/* Union of all result tasks */
typedef PACKED union PACKED_POST
{
  srchlog_pn_bypass_peak_set_struct_type no_rxd;
  srchlog_pn_bypass_peak_set_struct_type rxd[2];
} srchlog_pn_bypass_peak_rslt_union_type;

/* Log packet structure */
LOG_RECORD_DEFINE(SRCH_PN_BYPASS_PEAK_LOG_PACKET)
  uint8 version;

  /* Number of specified tasks */
  uint8 num_task_spec;
  
  /* Per-task data */
  srchlog_pn_bypass_peak_input_struct_type task[SRCH_NORMAL_MODE_MAX_PN_TASKS];

  /* Per-result data, one array element per task */
  srchlog_pn_bypass_peak_rslt_union_type   rslt[SRCH_NORMAL_MODE_MAX_PN_TASKS];
LOG_RECORD_END


#endif /* FEATURE_DC_HSDPA_LOGGING */

/* information for each input task */
typedef PACKED struct PACKED_POST
{

  /* Coherent integration duration */
  uint8 nc; 
  
  /* Noncoherent integration duration */
  uint8 nn; 

  /* 
   * ANTENNA_RXD_INFO		Description: Bit [5-3]
   * 0 - Primary antenna and rxd_mode 0
   * 1 - Secondary Antenna  rxd_mode 0
   * 2 - Both antennas in rxd add mode
   * 3 - Both antennas in rxd separate mode
   *
   * PD_MODE Peak detect mode. Bit [2-1]
   *    
   * STTD  1  Sttd enabled. Bit 0  
   */
  uint8 pn_config;
  
  /* Channelization code index */
  uint8 ovsf;
  
  /* Window starting position for PN search. This values in chipx1. The field PN_POS_CX8 and this field should be used to print the pn_pos in cx8. PN_POS << 3 +  PN_POS_CX8 */
  uint16 pn_pos;
  
  /* Bits [12- 0] the scrambling code used for this PN search, 0-8176 */
  uint16 src_code; 
  
  /* the window size for PN search to search for the peak */
  uint16 win_size;
  
  /*
   *
   * Bits [2-0] 
   *   000 - ASET
   *   010 - Intra Freq Monitored set
   *   100 - Intra Freq Unlisted set
   *   001 - Inter Freq Monitored Set
   *   101 - Inter Freq Virtual Active Set
   *   
   * Bits [4-3] 
   *   (Bit4,Bit3): Indicate MBMS Set. 
   *        0    0   : NO MBMS 
   *        0    1   : MBMS ASET
   *        1    0   : MBMS SYNC
   * Bit [5] : Antenna Select
   *      0  : Antenna 0
   *      1  : Antenna 1
   *        
   */
  uint8 set;

#ifdef FEATURE_WCDMA_PN_PACKET_VER2
  uint8 lsw_pos;
#endif

#ifndef FEATURE_DC_HSDPA_LOGGING
  uint32 filt_eng[3];
#endif
} srchlog_pn_input_struct_type;

/* Structure that holds a set of 6 peaks per result */
typedef PACKED struct PACKED_POST
{
  #ifdef FEATURE_DC_HSDPA_LOGGING
  /* sequence is different */
  uint32 peak_pos[6];
  uint32 peak_eng[6];
  #else
  uint32 peak_eng[6];
  uint32 peak_pos[6];
  #endif
} srchlog_pn_peak_set_struct_type;

/* Filtered energy per result task */
typedef PACKED struct PACKED_POST
{  
  uint32 eng[3];
#ifdef FEATURE_WCDMA_PN_PACKET_VER2
  uint8   rscp[3];
#endif
} srchlog_filt_eng_per_task_struct_type;


/* Logging result structure for non-RxD normal searches */
typedef PACKED struct PACKED_POST
{
  srchlog_filt_eng_per_task_struct_type filt_eng;
  srchlog_pn_peak_set_struct_type       result;  
} srchlog_pn_rslt_no_rxd_normal_struct_type;

/* Logging result structure for RxD normal searches */
typedef PACKED struct PACKED_POST
{
  srchlog_filt_eng_per_task_struct_type filt_eng;
  srchlog_pn_peak_set_struct_type       result[2];  
} srchlog_pn_rslt_rxd_normal_struct_type;

/* Logging result structure for non-RxD shoulder searches */
typedef PACKED struct PACKED_POST
{
  srchlog_filt_eng_per_task_struct_type filt_eng;
  srchlog_pn_peak_set_struct_type       result[3];  
} srchlog_pn_rslt_no_rxd_shoulder_struct_type;

/* Logging result structure for RxD shoulder searches */
typedef PACKED struct PACKED_POST
{  
  srchlog_filt_eng_per_task_struct_type filt_eng;
  srchlog_pn_peak_set_struct_type       result[6];  
} srchlog_pn_rslt_rxd_shoulder_struct_type;

/* Union of all result tasks */
typedef PACKED union PACKED_POST
{
  srchlog_pn_rslt_no_rxd_normal_struct_type    no_rxd_normal;
  srchlog_pn_rslt_no_rxd_shoulder_struct_type  no_rxd_shoulder; 
  srchlog_pn_rslt_rxd_normal_struct_type       rxd_normal;  
  srchlog_pn_rslt_rxd_shoulder_struct_type     rxd_shoulder;  
} srchlog_pn_rslt_union_type;

LOG_RECORD_DEFINE(SRCH_PN_LOG_PACKET)

#ifdef FEATURE_DC_HSDPA_LOGGING
  
  uint8 version;

#ifdef FEATURE_WCDMA_PN_PACKET_VER4
   /*Ecio Conversion Table version. 0 is the default value for pre JO targets*/
   uint8 ecio_conv_tbl_ver;

   /* RF antenna tune information*/
  uint8 rf_tune_info;
#endif

  srchlog_dc_hsdpa_carr_info_struct_type rx_ss_freeze_info;

#ifdef FEATURE_WCDMA_PN_PACKET_VER4
    /* UARFCN(Downlink) of a carrier.
    UARFCN[0]  : Primary carrier UARFCN 
    UARFCN[1]  : Secondary carrier UARFCN 
    UARFCN[2]  : Tertiary carrier UARFCN
    UARFCN[3]  : Quaternary carrier UARFCN
  */
  uint16 uarfcn[MAX_CARR];
#endif

  /* Frequency hypothesis */
  int16 freq_offset;
   
#else
 
  /* Instantaneous Rx AGC on Primary Antenna*/
  int16 rx_agc_0;

  /* Instantaneous Rx AGC on Secondary Antenna */
  int16 rx_agc_1;
#endif
  /* Number of specified tasks */
  uint8 num_task_spec;
  
  /* Window starting position of cx8 part */
  uint8 pn_pos_lsb;

  srchlog_pn_input_struct_type task[SRCH_NORMAL_MODE_MAX_PN_TASKS];

  srchlog_pn_rslt_union_type   rslt[SRCH_NORMAL_MODE_MAX_PN_TASKS];
LOG_RECORD_END

LOG_RECORD_DEFINE(SRCH_LIST_ALL_CELLS_LOG_PACKET)

#ifdef FEATURE_DC_HSDPA_LOGGING
  
  uint8 version;
  srchlog_dc_hsdpa_carr_info_struct_type rx_ss_freeze_info;

  /* Frequency hypothesis */
  int16 freq_offset;
   
#else
 
  /* Instantaneous Rx AGC on Primary Antenna*/
  int16 rx_agc_0;

  /* Instantaneous Rx AGC on Secondary Antenna */
  int16 rx_agc_1;
#endif
  /* Number of specified tasks */
  uint8 num_task_spec;
  
  /* Window starting position of cx8 part */
  uint8 pn_pos_lsb;

  srchlog_pn_input_struct_type task[SRCH_NORMAL_MODE_MAX_PN_TASKS];

  srchlog_pn_rslt_union_type   rslt[SRCH_NORMAL_MODE_MAX_PN_TASKS];
LOG_RECORD_END


#define MAX_NUM_PSC_TST 6
#define MAX_NUM_CODES_PER_GROUP 8

/* information for each code group tested  */
typedef PACKED struct PACKED_POST {
    /* code group id. range: 0...63 */
    uint8 code_grp ;

    /* pn position  0.....(38400x8 -1) */
    uint32 pos ;
    
    /* srch energy */
    uint16 eng ;

} srchlog_ssc_peak_struct_type ;


#define SRCH_LIST_SRCH_MAX_TASKS (SRCH_NORMAL_MODE_MAX_PN_TASKS)

/*--------------------------------------------------------------------------
                 LOG PACKET: ASET_LOG_PACKET
                     
This structure is use to log active set informations. Logged every time L1
receives active set update message from RRC
--------------------------------------------------------------------------*/

/* information for each cell */
typedef PACKED struct PACKED_POST {

 /*  Is this the ref cell ? 0-no 1-yes  */
 uint8  is_ref_cell;

 /* Primary Scrambling Code */
 uint16 pri_scr_code ;

 /* Cell TPC  1...5 */
 uint8 cell_tpc ;

 /* CPICH diversity  00 - no diversity 
    01 - STTD     10 - TSTD      11 - SSTD     */
 uint8 cell_diversity ;

 /* Is there a secondary CPICH present? 0 - no 1 - yes */
 uint8 scpich_exists ;

 /* Secondary Srambling Code 1....15 (exists only if 
    Secondary CPICH is present */
 uint8 sec_scr_code ;  

 /* OVSF code for S-CPICH, if S-CPICH exists */
 uint8 scpich_ovsf ;

 /*  cell position in chipx8   */
 uint32 cell_pos;

} srchlog_cell_info_type ;

LOG_RECORD_DEFINE(ASET_LOG_PACKET)

 /* Num of cells being tracked */
 uint8 aset_size ;

 /* WCDMA frequency  */
 uint16 freq ;

 /* For each cell */
 srchlog_cell_info_type srchlog_cell[L1_ASET_MAX] ;

LOG_RECORD_END

#ifdef FEATURE_L1_LOG_ON_DEMAND
typedef struct
{
  /* snapshot of common dl trCH setup parameters */
  ASET_LOG_PACKET_type   channel_log;

  /* flag to determine snapshot has atleast one valid value */
  boolean                valid;
} aset_log_on_demand_struct_type;
#endif

/*--------------------------------------------------------------------------
                 LOG PACKET: ASET_LOG_PACKET_V2
                     
This structure is use to log active set informations. Logged every time L1
receives active set update message from RRC
--------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(ASET_LOG_PACKET_V2)

 /* Version Number of log packet */
 uint8 version;

 /* Num of cells being tracked */
 uint8 aset_size ;

 /* WCDMA frequency  */
 uint16 freq ;

 #ifdef FEATURE_WCDMA_DC_HSDPA
 /* Num of cells being tracked on the secondary carrier */
 uint8 dc_a_set_size;

 /* WCDMA frequency on secondary carrier */
 uint16 freq_dc;

 /* For each cell */
 srchlog_cell_info_type srchlog_cell[L1_ASET_MAX + L1_DC_A_SET_MAX];
 #else

 /* For each cell */
 srchlog_cell_info_type srchlog_cell[L1_ASET_MAX];

 #endif /* FEATURE_WCDMA_DC_HSDPA */

LOG_RECORD_END


/*--------------------------------------------------------------------------
                 LOG PACKET: NSET_LOG_PACKET
                     
This structure is use to log neighbor set informations. Logged every time L1
receives neighbor set update message from RRC
--------------------------------------------------------------------------*/

#define  MAX_NSET_FREQ 3 
#define  MAX_NSET_CELLS_PER_FREQ 32

/* information for each cell */
typedef PACKED struct PACKED_POST {
 /* Primary Srambling Code */
 uint16  pri_scr_code ;

 /* Diversity indicator 0 - No  1 - yes */
 uint8 cell_diversity ;

 /* cell position, in chipx8   */
 uint32  cell_pos;
} nset_cell_info_type ;

/* information for each freq */
typedef PACKED struct PACKED_POST {
 /* Frequency */
 uint16 freq ;

 /* num of cells */
 uint8 num_cells ;

 /* per freq info*/
 nset_cell_info_type cell_info[MAX_NSET_CELLS_PER_FREQ] ;
} srchlog_per_freq_info_type ; 

LOG_RECORD_DEFINE(NSET_LOG_PACKET)

 /* num of freq */
 uint8 num_freq ;

 srchlog_per_freq_info_type srchlog_freq[MAX_NSET_FREQ] ;
LOG_RECORD_END

#ifdef FEATURE_L1_LOG_ON_DEMAND
typedef struct
{
  /* snapshot of common dl trCH setup parameters */
  NSET_LOG_PACKET_type   channel_log;

  /* flag to determine snapshot has atleast one valid value */
  boolean                valid;
} nset_log_on_demand_struct_type;
#endif


/*--------------------------------------------------------------------------
              LOG PACKET: SRCH_CELL_RESELECTION_RANK_LOG_PKT
                     
  This structure is used to log neighbor cell rank information during
  cell reselection evaluation. This is logged every time a neighbor list
  search is done for cell reselection evaluation.
--------------------------------------------------------------------------*/

/* Maximum number of cells to rank */
#define SRCH_CELL_RESEL_RANK_LOG_NUM_CELLS_MAX 32

/* WCDMA cell reselection rank log info */
typedef PACKED struct PACKED_POST
{
  /* WCDMA RF frequency, range 10562..10838 */
  uint16 freq;

  /* primary scrambling code */
  uint16 psc;
  
  /* RSCP, log_val = actual_val + 21, 0xFF -> not used */  
  int8 rscp;

  /* RSCP rank, range [-200, 200], 0xFF -> not used */
  int16 rank_rscp;

  /* ECIO, range 0 .. -63, 0xFF -> not used */
  int8 ecio;

  /* ECIO rank, range [-200, 200], 0xFF -> not used */
  int16 rank_ecio;
  
} srch_cell_resel_wcdma_rank_log_struct_type;

/* GSM cell reselection log info */
typedef PACKED struct PACKED_POST
{
  
  /* ARFCN, range 0..1023 */
  uint16 arfcn;

  /* BSIC: bcc in LSB, ncc in MSB */
  uint16 bsic;

  /* RSSI: 0 .. -120 */
  int8 rssi;
  
  /* rank of cell, range [-200, 200] */
  int16 rank;
  
} srch_cell_resel_gsm_rank_log_struct_type;

/* Cell Reselection rank log info */
LOG_RECORD_DEFINE(SRCH_CELL_RESEL_RANK_LOG_PKT)
   
  /* Number of ranked WCDMA cells, up to 64 when supporting inter-F */
  uint8 num_wcdma_cells;

  /* Number of ranked GSM cells, up to 32 */
  uint8 num_gsm_cells;

  /* ranked WCDMA cells, need to double size to include interf-F */
  srch_cell_resel_wcdma_rank_log_struct_type 
      wcdma[SRCH_CELL_RESEL_RANK_LOG_NUM_CELLS_MAX];

  /* ranked GSM cells */
  srch_cell_resel_gsm_rank_log_struct_type   
      gsm[SRCH_CELL_RESEL_RANK_LOG_NUM_CELLS_MAX];
  
LOG_RECORD_END


/*--------------------------------------------------------------------------
       LOG PACKET (0x4027): Absolute_Priority_Cell_Reselection_Log_Packet  
                     
  This structure is used to log neighbor cell information during cell reselection 
  evaluation when SIB19/Priority based reslection is used. 
  This is logged when cell reselection evaluation is performed in IDLE/DRX mode.
--------------------------------------------------------------------------*/

#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY

/* WCDMA cell reselection rank log info */
typedef PACKED struct PACKED_POST
{
  /* WCDMA RF frequency, range 10562..10838 */
  uint16 freq;

  /* primary scrambling code */
  uint16 psc;

  /* Priority of this cell, 0xFF -> not used */
  uint8 priority;

  /* ECIO, range 0 .. -63, 0xFF -> not used */
  int8 ecio;
  
  /* RSCP, log_val = actual_val + 21, 0xFF -> not used */  
  int16 rscp;
  /* Cell Selection RX Level Value */
  int16 s_rxlev;

  /* Cell selection quality level value */
  int16 s_qual;
  
  /* RSCP rank, range [-200, 200], 0xFF -> not used */
  int16 rank_rscp;

  /* ECIO rank, range [-200, 200], 0xFF -> not used */
  int16 rank_ecio;

  /* A Srxlev based threshold used by the UE for
   * cell reselection towards a higher absolute
   * priority layer when absolute priorities are applied.
   */
  uint8 threshx_high;
  /* A Srxlev based threshold used by the UE for
   * cell reselection towards an equal or lower
   * absolute priority layer when absolute
   *  priorities are applied.
   */
  uint8 threshx_low;

  /* Notifies if absolute criteria for the cell satisfied */
  boolean absolute_criteria_pass;

} srch_abs_prio_cell_resel_wcdma_rank_log_struct_type;

/* GSM cell reselection log info */
typedef PACKED struct PACKED_POST
{
  /* ARFCN, range 0..1023 */
  uint16 arfcn;

  /* BSIC: bcc in LSB, ncc in MSB */
  uint16 bsic;

  /* Priority of this cell, 0xFF -> not used */
  uint8 priority;

  /* GSM band */
  int8 band;

  /* RSSI: 0 .. -120 */
  int16 filt_rssi;

  /* Cell Selection RX Level Value */
  int16 s_rxlev;
 
  /* rank of cell, range [-200, 200] */
  int16 rank;

  /* A Srxlev based threshold used by the UE for
   * cell reselection towards a higher absolute
   * priority layer when absolute priorities are applied.
   */
  uint8 threshx_high;
  /* A Srxlev based threshold used by the UE for
   * cell reselection towards an equal or lower
   * absolute priority layer when absolute
   *  priorities are applied.
   */
  uint8 threshx_low;

  /* Notifies if absolute criteria for the cell satisfies */
  boolean absolute_criteria_pass;
 
} srch_abs_prio_cell_resel_gsm_rank_log_struct_type;

#ifdef FEATURE_WCDMA_TO_LTE
/* LTE cell reselection log info */
typedef PACKED struct PACKED_POST
{
  /* E-ARFCN */
  uint32 earfcn;

  /* The physical cell id of the detected cell */
  uint16 cell_id;

  /* Priority of this cell */
  uint8 priority;

  /* Max RSRP comb.value combined across Tx-Rx pairs. In linear scale */
  int16 rsrp;

  /* Max RSRQ value combined across Tx-Rx pairs. In linear scale */
  int8 rsrq;
  /* Priority of this cell */
  //uint8 priority;

  /* Cell Selection RX Level Value */
  int16 s_rxlev;
 
  /* Cell Selection quality value (dB) */
  int16 s_qual;

  /* A Srxlev based threshold used by the UE for
   * cell reselection towards a higher absolute
   * priority layer when absolute priorities are applied.
   */
  uint8 threshx_high;
  /* A Srxlev based threshold used by the UE for
   * cell reselection towards an equal or lower
   * absolute priority layer when absolute
   *  priorities are applied.
   */
  uint8 threshx_low;

  /* A Squal based threshold used by the UE for
   * cell reselection towards a higher absolute
   * priority layer when absolute priorities are applied.
   */
  uint8 threshx_high2;
  /* A Squal based threshold used by the UE for
   * cell reselection towards an equal or lower
   * absolute priority layer when absolute
   *  priorities are applied.
   */
  uint8 threshx_low2;

  /* Notifies if absolute criteria for the cell satisfies */
  boolean absolute_criteria_pass;

} srch_abs_prio_cell_resel_lte_rank_log_struct_type;
#endif

/* Cell Reselection rank log info */
LOG_RECORD_DEFINE(SRCH_ABS_PRIO_CELL_RESEL_RANK_LOG_PKT)

  /* Version */
  uint8 version;

  /* Number of ranked WCDMA cells, up to 64 when supporting inter-F */
  uint8 num_wcdma_cells_searched;

  /* Number of ranked GSM cells, up to 32 */
  uint8 num_gsm_cells_searched;

  #ifdef FEATURE_WCDMA_TO_LTE
   /* Number of ranked LTE cells, up to 32 */
  uint8 num_lte_cells_searched;
  #endif

   /* High / ALL priority scheduling mode */
  uint8 scheduling_mode;

  /*Range: (0 to 31)*/
  uint8 thresh_serving_low;

  /*Range: (0 to 31)*/
  uint8 thresh_serving_low2;

  /* ranked WCDMA cells, need to double size to include interf-F */
  srch_abs_prio_cell_resel_wcdma_rank_log_struct_type 
      wcdma[SRCH_CELL_RESEL_RANK_LOG_NUM_CELLS_MAX];

  /* ranked GSM cells */
  srch_abs_prio_cell_resel_gsm_rank_log_struct_type   
      gsm[SRCH_CELL_RESEL_RANK_LOG_NUM_CELLS_MAX];

  #ifdef FEATURE_WCDMA_TO_LTE
  /* ranked LTE cells */
  srch_abs_prio_cell_resel_lte_rank_log_struct_type   
      lte[SRCH_CELL_RESEL_RANK_LOG_NUM_CELLS_MAX];
  #endif
LOG_RECORD_END
#endif /* defined FEATURE_WCDMA_ABSOLUTE_PRIORITY */

typedef enum
{
  SRCH_FACH_INVALID,
  SRCH_FACH_WCDMA_STEP1,
  SRCH_FACH_WCMDA_PSC_ACQ,
  SRCH_FACH_WCDMA_PSC_RECONF,
  SRCH_FACH_GSM_BSIC_ID,
  SRCH_FACH_GSM_BSIC_RC,
  SRCH_FACH_GSM_RSSI,
  SRCH_FACH_LTE_GAP
} srch_fach_fmo_oksn_use_enum_type;


typedef enum
{
  SRCH_FACH_MEAS_OKSN_TYPE_SIB_IND_FALSE,
  SRCH_FACH_MEAS_OKSN_TYPE_SIB_IND_TRUE,
  SRCH_FACH_MEAS_OKSN_TYPE_NOT_SUPPORTED
} srch_fach_meas_oksn_type_info_enum;

typedef PACKED struct PACKED_POST {
  /* Notifies the start SFN of a FMO*/
  uint16   fmo_reserv_start_sfn; 

  /* Notifies the scheduling mode in case of absolute priority */
  uint8   scheduling_mode; 

  /* Notifies the measurement rules used to perform the FMO*/
  uint8   curr_meas_rules;
  uint8   pruned_meas_rules;

  /* Type of FMO that is attempted */
  srch_fach_fmo_oksn_use_enum_type   fmo_srch_type;

  /* Frequency to which FMO is tuned to */
  uint32  freq_tuned_during_fmo;

  /* Notifies if the reservation was succesful for a FMO */
  boolean fmo_reserv_status;

  /* Notifies the FAIL cause for a FMO */
  uint8   fmo_fail_cause;

  /* Notfies the FMO gap length in slots */
  uint8   fmo_reserv_length_in_slots;
} srch_fach_meas_oksn_info_type; /* 13 bytes */

LOG_RECORD_DEFINE(SRCH_FACH_MEAS_OKSN_INFO_LOG_PKT)
  uint8    version;
  uint8    m_rep; /* 2^k, where k ranges 1..6 */
  uint16   c_rnti;
  uint8    n_tti; /* 1, 2, 4, or 8 */
  boolean  w_if_fmo_indicator;
  boolean  irat_fmo_indicator;
  uint8    ue_cap_and_sib19_ind;
  boolean  if_prior_info_present;
  boolean  gsm_prior_info_present;
  boolean  lte_prior_info_present;
  uint8    table_idx;
  srch_fach_meas_oksn_info_type fmo_info_table[20];
LOG_RECORD_END  /* 20*13 + 11 = 271 bytes */

#define SRCH_FACH_MEAS_OKSN_INFO_LOG_PKT_LEN() \
    (sizeof(SRCH_FACH_MEAS_OKSN_INFO_LOG_PKT_type))
    
typedef PACKED struct PACKED_POST {
  uint8    last_cmd_schedule_gfn;     /* GFN when last cmd was scheduled */
  uint8    cm_startup_cmd_gfn;
  uint16   cm_startup_cmd_offset_qs;
  uint8    cm_cleanup_cmd_gfn;
  uint16   cm_cleanup_cmd_offset_qs;
} srch_fach_gsm_cm_gap_cmds_struct_type; /* 7 bytes */

typedef PACKED struct PACKED_POST {
  uint16  arfcn;
  int8    rslt_rssi;       /* meas result RSSI */

  /* The following are internal */
  uint8   cmd_gfn;         /* meas cmd GFN */
  uint16  burst_offset_qs; /* burst offset */
  int8    esti_rssi;       /* estimated RSSI for burst */
  uint8   rslt_gfn;        /* GFN when reading result */
} srch_fach_gsm_rssi_burst_struct_type; /* 8 bytes */

#define SRCH_FACH_GSM_RSSI_LOG_MAX_BURSTS 18

typedef PACKED struct PACKED_POST {
  uint8  num_rssi_bursts;

  /* The following are internal */
  srch_fach_gsm_cm_gap_cmds_struct_type  cm_gap_cmds;

  /* If less than 3 bursts, log all bursts; 
   * otherwise, log the first, second, and last bursts in array */
  srch_fach_gsm_rssi_burst_struct_type  
    rssi_bursts[SRCH_FACH_GSM_RSSI_LOG_MAX_BURSTS];
} srch_fach_gsm_rssi_log_struct_type; /* 16 ~ 32 bytes */

#define SRCH_FACH_GSM_RSSI_LOG_FIRST_BURST  0
#define SRCH_FACH_GSM_RSSI_LOG_SECOND_BURST 1
#define SRCH_FACH_GSM_RSSI_LOG_LAST_BURST   2

#define SRCH_FACH_GSM_BSIC_OP_SUCCESS_LOG_MSK  0x8000  /* MSB 15 is 1 */

typedef PACKED struct PACKED_POST {
  uint16  arfcn;
  uint16  bsic_id_timer;   /* in unit of WCDMA frames; MSB: SUCCESS/FAIL */
  uint16  t_identify_gsm;  /* in unit of WCDMA frames */

  /* These are valid only if ACQ success */
  uint8   ncc;  /* Network Color Code */
  uint8   bcc;  /* Base station color code */
  uint8   nbr_gfn_delta; /* neighbor GFN delta relative to GSTMR */
  uint8   nbr_sch_gfn;       /* Decoded neighbor SCH GFN */
  uint16  nbr_sch_offset_qs;      /* neighbor SCH offset relative to GFN frame */
  
  /* The following are all internal */
  srch_fach_gsm_cm_gap_cmds_struct_type  cm_gap_cmds;
  uint8   acq_start_gfn;
  uint16  acq_start_offset_qs;
  uint16  acq_width_qs;
  int8    esti_rssi;       /* estimated RSSI */
  uint8   acq_stop_gfn;
  uint16  acq_stop_offset_qs;
  uint8   first_rslt_poll_gfn;
  uint8   last_rslt_poll_gfn;
} srch_fach_gsm_bsic_id_acq_log_struct_type; /* 30 bytes */

typedef PACKED struct PACKED_POST {
  uint16  arfcn;
  uint16  bsic_id_timer;   /* in unit of WCDMA frames; MSB: SUCCESS/FAIL */
  uint16  t_identify_gsm;  /* in unit of WCDMA frames */

  /* These are valid only if tone detection success */
  uint8   nbr_gfn_delta; /* Assume tone detected in neighbor GFN 0 */
  uint16  nbr_sch_offset_qs;      /* Estimated neighbor SCH offset relative to GFN frame */
  
  /* The following are all internal */
  srch_fach_gsm_cm_gap_cmds_struct_type  cm_gap_cmds;
  uint8   acq_start_gfn;
  uint16  acq_start_offset_qs;
  uint32  acq_width_qs;
  int8    esti_rssi;       /* estimated RSSI */
  uint8   acq_stop_gfn;
  uint16  acq_stop_offset_qs;
  uint8   acq_cleanup_gfn; /* second part of AcqStop for tone detection */
  uint8   first_rslt_poll_gfn;
  uint8   last_rslt_poll_gfn;
} srch_fach_gsm_bsic_id_tone_log_struct_type; /* 30 bytes */

typedef PACKED struct PACKED_POST {
  uint16  arfcn;
  uint16  bsic_id_timer;   /* in unit of WCDMA frames; MSB: SUCCESS/FAIL */
  uint16  t_identify_gsm;  /* in unit of WCDMA frames */

  /* These are valid only if SCH decoding success */
  uint8   ncc;  /* Network Color Code */
  uint8   bcc;  /* Base station color code */
  uint8   nbr_gfn_delta;     /* neighbor GFN delta relative to GSTMR */
  uint8   nbr_sch_gfn;       /* Decoded neighbor SCH GFN */
  uint16  nbr_sch_offset_qs; /* neighbor SCH offset relative to GFN frame */
  
  /* The following are all internal */
  srch_fach_gsm_cm_gap_cmds_struct_type  cm_gap_cmds;
  uint8   sch_dec_gfn;
  int32   sch_dec_start_offset_qs;
  uint16  sch_dec_ctr_offset_qs;
  int8    esti_rssi;       /* estimated RSSI */
  uint8   burst_metric_gfn;
  uint8   sch_burst_gfn;
  uint8   rslt_poll_gfn;
} srch_fach_gsm_bsic_id_sch_log_struct_type;  /* 30 bytes */

typedef PACKED struct PACKED_POST {
  uint16  arfcn;
  uint16  bsic_rc_timer;   /* in unit of WCDMA frames; MSB: SUCCESS/FAIL */
  uint16  t_reconfirm_gsm; /* in unit of WCDMA frames */

  /* These are valid only if SCH decoding success */
  uint8   ncc;  /* Network Color Code */
  uint8   bcc;  /* Base station Color Code */
  uint8   nbr_gfn_delta;     /* neighbor GFN delta relative to GSTMR */
  uint16  nbr_sch_offset_qs; /* neighbor SCH offset relative to GFN frame */

  /* The following are all internal */
  srch_fach_gsm_cm_gap_cmds_struct_type  cm_gap_cmds;
  uint8   sch_dec_gfn;
  int32   sch_dec_start_offset_qs;
  uint16  sch_dec_ctr_offset_qs;
  int8    esti_rssi;       /* estimated RSSI */
  uint8   burst_metric_gfn;
  uint8   sch_burst_gfn;
  uint8   rslt_poll_gfn;
} srch_fach_gsm_bsic_rc_log_struct_type;  /* 29 bytes */

typedef PACKED union PACKED_POST
{
  srch_fach_gsm_rssi_log_struct_type           rssi_meas;    
  srch_fach_gsm_bsic_id_acq_log_struct_type    bsic_id_acq_meas;  
  srch_fach_gsm_bsic_id_tone_log_struct_type   bsic_id_tone_meas; 
  srch_fach_gsm_bsic_id_sch_log_struct_type    bsic_id_sch_meas;  
  srch_fach_gsm_bsic_rc_log_struct_type        bsic_rc_meas;      
} srch_fach_gsm_meas_log_union_type;

typedef PACKED struct PACKED_POST {
  uint8    oksn_start_gfn;
  uint16   oksn_start_offset_qs;
  uint8    oksn_end_gfn;
  uint16   oksn_end_offset_qs;
} srch_fach_gsm_oksn_info_struct_type; /* 6 bytes */

typedef enum
{
  SRCH_FACH_GSM_OKSN_RSSI,
  SRCH_FACH_GSM_OKSN_BSIC_ID_ACQ,    /* FCCH plus SCH */
  SRCH_FACH_GSM_OKSN_BSIC_ID_TONE,   /* FCCH only */
  SRCH_FACH_GSM_OKSN_BSIC_ID_SCH,    /* SCH decoding for BSIC ID */
  SRCH_FACH_GSM_OKSN_BSIC_RC,
  SRCH_FACH_GSM_OKSN_BSIC_ID_NO_SCH, /* no SCH falling in oksn */
  SRCH_FACH_GSM_OKSN_SW_MISSED,      /* SW task context too slow for scheduling */
  SRCH_FACH_GSM_OKSN_BLOCKED         /* oksn blocked by RACH or intra-freq */  
} srch_fach_gsm_oksn_use_enum_type;

LOG_RECORD_DEFINE(SRCH_FACH_INTER_RAT_MEAS_LOG_PKT)
  uint8   pkt_ver;
  uint16   oksn_sccpch_sfn; 
  srch_fach_gsm_oksn_use_enum_type    oksn_usage; /* 1 byte */
  srch_fach_gsm_oksn_info_struct_type oksn_info;  /* 6 bytes */
  srch_fach_gsm_meas_log_union_type   meas_info;  /* 16 ~ 32 bytes */
LOG_RECORD_END


#ifdef FEATURE_WCDMA_L1_PSC_SCANNER

/* Structure to store the results of the PSC scanner search */
typedef PACKED struct PACKED_POST {
    /* primary scrambling code 0..512 */
    uint16 psc ;
   /* Ec/Io in 0.5dB units */
    uint8 ecio;
   /* RSCP in dBm */
    uint8 rscp;

} srchmview_result_struct_type ;

/* Define the log packet for the PSC scan search */
LOG_RECORD_DEFINE(SRCH_PSC_SCAN_PILOT_INFO_LOG_PKT)
    /* DL UARFCN */
    uint16 freq ;
    /* number of peaks found during step3 search */
    uint8 num_task;
    /* Details of each peak */
    srchmview_result_struct_type srchmview_psc_scan_peak_info[MAX_NUM_PSC_SCAN_RSLT] ;

LOG_RECORD_END

#endif /* FEATURE_WCDMA_L1_PSC_SCANNER */

/* Define the log packet for search history */
LOG_RECORD_DEFINE(SEARCH_HIST_INFO_LOG_PACKET)
    /* Version number */
    uint8 ver_num ;
    /* number of search results */
    uint8 num_srch;
    /* Details of each search result */
    wl1_circ_buff_srch_hist_struct_type srch_hist_circ_buff[WL1_CIRC_BUFF_HISTORY_LENGTH];

LOG_RECORD_END

/* Define the log packet for nastt history */
LOG_RECORD_DEFINE(NASTT_HIST_INFO_LOG_PACKET)
    /* Version number */
    uint8 ver_num ;
    /* number of search results */
    uint8 num_srch;
    /* Details of each search result */
    wl1_circ_buff_nastt_hist_struct_type nastt_hist_circ_buff[WL1_CIRC_BUFF_HISTORY_LENGTH];

LOG_RECORD_END

/* Define the log packet for nastt detect history */
LOG_RECORD_DEFINE(NASTT_DETECT_HIST_INFO_LOG_PACKET)
    /* Version number */
    uint8 ver_num ;
    /* number of search results */
    uint8 num_srch;
    /* Details of each search result */
    wl1_circ_buff_nastt_detect_hist_struct_type nastt_detect_hist_circ_buff[WL1_CIRC_BUFF_HISTORY_LENGTH];

LOG_RECORD_END

/* Define the log packet for cell history */
LOG_RECORD_DEFINE(CELL_HIST_INFO_LOG_PACKET)
    /* Version number */
    uint8 ver_num ;
    /* number of search results */
    uint8 num_srch;
    /* Details of each search result */
    wl1_circ_buff_cell_hist_struct_type cell_hist_circ_buff[WL1_CIRC_BUFF_HISTORY_LENGTH];

LOG_RECORD_END

/* Define the log packet for aset filt history */
LOG_RECORD_DEFINE(ASET_FILT_HIST_INFO_LOG_PACKET)
    /* Version number */
    uint8 ver_num ;
    /* number of search results */
    uint8 num_srch;
    /* Details of each search result */
    wl1_circ_buff_aset_filt_hist_struct_type aset_filt_hist_circ_buff[WL1_CIRC_BUFF_HISTORY_LENGTH];

LOG_RECORD_END

/* Define the log packet for aset filt history */
LOG_RECORD_DEFINE(SIRT_FILT_HIST_INFO_LOG_PACKET)
    /* Version number */
    uint8 ver_num ;
    /* number of search results */
    uint8 num_srch;
    /* Details of each search result */
    wl1_circ_buff_sirt_filt_hist_struct_type sirt_filt_hist_circ_buff[WL1_CIRC_BUFF_HISTORY_LENGTH];

LOG_RECORD_END

/* Define the log packet for filt ecio history */
LOG_RECORD_DEFINE(FILT_ECIO_HIST_INFO_LOG_PACKET)
    /* Version number */
    uint8 ver_num ;
    /* number of search results */
    uint8 num_srch;
    /* Details of each search result */
    wl1_circ_buff_filt_ecio_hist_struct_type filt_ecio_hist_circ_buff[WL1_CIRC_BUFF_HISTORY_LENGTH];

LOG_RECORD_END


/* Define the log packet for rxd filt history */
LOG_RECORD_DEFINE(RXD_FILT_HIST_INFO_LOG_PACKET)
    /* Version number */
    uint8 ver_num ;
    /* number of search results */
    uint8 num_srch;
    /* Details of each search result */
    wl1_circ_buff_rxd_filt_hist_struct_type rxd_filt_hist_circ_buff[WL1_CIRC_BUFF_HISTORY_LENGTH];

LOG_RECORD_END

/* Structure to hold RSSI info against the ARFCN. Used in freq scan result log pkt*/
typedef PACKED struct PACKED_POST {
    
    /* Instantaneous signal strength*/
    int16 rssi;
    /*corresponding UARFCN*/
    uint16 uarfcn;

}srchlog_freq_scan_info_struct_type;

/* This is the Freq scan result log packet structure. This log packet captures
information regarding different types of freq scan (Raw/fine/additiona ch).
The UARFCNs captured here are the ones that satisfy the threshold and are
reported to RRC */
LOG_RECORD_DEFINE(SRCH_FREQ_SCAN_LOG_PACKET)

  /* Version number */
  uint8 ver_num ;

  /*Indicates freq scan type raw/fine/additional */
  uint8 freq_scan_type_info;

  /*AGC thresholds */
  int16 scan_thresh;
  
  /*Num of ARFCNs that are better than threshold and reported to RRC*/
  uint8 num_arfcn;

  srchlog_freq_scan_info_struct_type result[L1_MAX_SCAN_FREQ];
  
LOG_RECORD_END


#ifdef FEATURE_FEMTO_CSG

/* WCDMA CSG ASF Meas Req log info */
typedef PACKED struct PACKED_POST
{
  /* WCDMA RF frequency, range 10562..10838 */
  uint16 freq;

  /* primary scrambling code */
  uint16 psc;

  /*CSG ID. INVALID ID -> 0xFFFFFFFF*/
  uint32 csg_id;

  /*ACQ PN POS*/
  uint32 acq_pn_pos;

  /*ACQ ENG*/
  uint32 acq_tot_eng;

  /*ACQ RX ACG*/
  int16 acq_rx_agc;

  /* ECIO, range 0 .. -63 */
  int16 acq_ecio;
  
  /* RSCP*/  
  int16 acq_rscp;
  
}srch_csg_asf_meas_req_log_struct_type;

LOG_RECORD_DEFINE(SRCH_CSG_ASF_MEAS_REQ_LOG_PKT)
  uint8 version;                            /*Version Info*/ 
  uint8 num_wcdma_intra_freq_cells_rxed;    /*No.of WCDMA intra freq cells received in ASF meas req*/
  uint8 num_wcdma_inter_freq_cells_rxed;    /*No.of WCDMA inter freq cells received in ASF meas req*/
  /*Array WCDMA cell info received as part of ASF meas request*/
  srch_csg_asf_meas_req_log_struct_type wcdma[SRCH_CELL_RESEL_RANK_LOG_NUM_CELLS_MAX];

LOG_RECORD_END

typedef PACKED struct PACKED_POST
{
  
  /* WCDMA RF frequency, range 10562..10838 */
  uint16 freq;

  /* primary scrambling code */
  uint16 psc;

  /*CSG ID. INVALID ID -> 0xFFFFFFFF*/
  uint32 csg_id;

  /*Min cell quality level for selection*/
  int8 q_qualmin;

  /*Min Rx level for Cell selection */
  int8 q_rxlevmin;

  /* Cell Selection quality value (dB) */
  int16 s_qual;

  /* Cell Selection RX Level Value */
  int16 s_rxlev;

  /* ECIO, range 0 .. -63 */
  int16 ecio;

  /* Received code power*/  
  int16 rscp;

  /* ECIO rank, range [-200, 200], 0xFF -> not used */
  int16 rank_ecio;

  /* RSCP rank, range [-200, 200], 0xFF -> not used */
  int16 rank_rscp;

  /*HCS priority for IntraF cells, 0xFFFF -> NA*/
  uint16 hcs_prio;

  /*HCS Rank*/
  int16 rank_hcs;

  /*Bool to indicate if current cell is a CSG Resel candidate*/
  boolean is_csg_resel_candidate;
  
}srch_csg_cell_resel_log_struct_type;

LOG_RECORD_DEFINE(SRCH_CSG_CELL_RESEL_LOG_PKT)

  uint8 version;                            /*Version Info*/ 
  uint8 num_wcdma_intra_freq_cells_searched;    /*No.of WCDMA intra freq cells searched in this DRX*/
  uint8 num_wcdma_inter_freq_cells_searched;    /*No.of WCDMA inter freq cells searched in this DRX*/

  uint16 reselected_csg_freq; /*Reselected CSG Freq. Value of 65535 implies NA*/
  uint16 reselected_csg_psc;  /*Reselected CSG cell PSC. Value of 4095 implies NA*/
  uint32 reselected_csg_id;  /*Reselected CSG cell ID. Value of 0xFFFFFFFF implies NA*/

  /*WCDMA CSG Cell Info*/
  srch_csg_cell_resel_log_struct_type wcdma[SRCH_CELL_RESEL_RANK_LOG_NUM_CELLS_MAX];

LOG_RECORD_END

#define CTCH_LOG_TABLE_SIZE 256

/* WCDMA CTCH enhancement log packet 0x41B9 */
typedef PACKED struct PACKED_POST
{
  /* Bit 0:        If CTCH is skipped
     Bits 1 and 2: Indicate if its PICH, Schedule, Data or Invalid sfn           :
     Bits 3 and 4: Provides info if Inter-frequency search could be fit in the usable gap
     Bits 5 and 6: Provides info if IRAT search could be fit in the usable gap
     Bit 7:        Unused    */  
  uint8 ctch_info; 

  /* CTCH SFN number */
  uint16 ctch_sfn;

  /* number of gaps that can be used for searches */
  uint16 usable_gaps;

}ctch_enh_log_struct_type;

LOG_RECORD_DEFINE(WCDMA_DRX_CTCH_ENH_LOG_PKT)

  uint8 version;                  /* Version Info */ 
  uint16 period;                  /* CTCH alocation periodicity*/
  uint16 offset;                  /* First CTCH offset within each SFN cycle*/
  uint8 ctch_mtti;                /* Number of frames for which each CTCH block will be transmitted */
  uint16 sched_table_start[2];   /* each index indicates the first CTCH sfn in a particular scheduling period */
  uint16 sched_table_end[2];     /* each index indicates the last CTCH sfn in a particular scheduling period */
  uint8 num_frames_ifreq;         /* number of frames required to perform inter frequency searches */
  uint8 num_frames_irat;          /* number of frames required to perform IRAT searches */
  boolean prior_srch_over_ctch;   /* Indicates whether searches needs to be prioritized */
  boolean srch_skipped_last_drx;  /* Indicates if any search was skipped in the previous DRX cycle */
  uint16 num_ctch_oksn;           /* Gives the number of CTCH occasions between two PICH SFN’s */

  /* CTCH logging array */
  ctch_enh_log_struct_type ctch_table_info[CTCH_LOG_TABLE_SIZE];

LOG_RECORD_END

/*WCDMA neighbor search optimization log info*/
typedef PACKED struct PACKED_POST
{
  /* WCDMA RF frequency, range 10562..10838 */
  uint16 freq;

  /*Priority value for W layer*/
  uint8 priority;

  /*Minimum value of Qualmin on a frequency*/
  int16 qualmin_min_fi;

  /*Minimum value of Qrxlev on a frequency*/
  int16 qrxlev_min_fi;

  /*Minimum value of Qoffset1 on a frequency*/
  int16 qoffset1_min_fi;

  /*Minimum vlaue of Qoffset2 on a frequency*/
  int16 qoffset2_min_fi;

  /*EcIo of Candidate neighbor */
  uint8 cand_nbr_ecio;

  /*RSCP of Candidate neighbor*/
  int16 cand_nbr_rscp;

  /*cell quality level for candidate neighbor*/
  int16 squal_cand_nbr;

  /*Rx level for candidate neighbor*/
  int16 srxlev_cand_nbr;

  /*serving cell rank based on EcIo */
  int16 rank_s_ecio;

  /*serving cell rank based on RSCP*/
  int16 rank_s_rscp;

  /*Candidate neighbor cell rank based on EcIo*/
  int16 rank_cand_nbr_ecio;

  /*Candidate neighbor cell rank based on RSCP*/
  int16 rank_cand_nbr_rscp;

  /*Flag to indicate cell quality level for candidate neighbor cell <0 or not
     * 1:TRUE, 0:False */
  boolean cond3;

  /*Flag to indicate Rx level for candidate neighbor cell <0 or not
     * 1:TRUE, 0:False */
  boolean cond4;

  /*Flag to indicate RSCP rank for serving cell better than neighbor cell
     * 1:TRUE, 0:False */
  boolean cond5;

  /*Flag to indicate EcIo rank for serving cell better than neighbor cell
      * 1:TRUE, 0:False */
  boolean cond6;

  /*Flag to indicate whether Srxlev for candidate neighbor less than threshx_high
     *1:TRUE, 0:False*/
  boolean cond7;

  /*Flag to indicate whetherSrxlev for candidate neighbor less than threshx_low
      * 1:TRUE, 0:False*/
  boolean cond8;

  /*Flag to indicate whether to skip full search */
  boolean skip_full_srch;

  /*Linear energy to eliminate step1 peaks */
  uint32 step1_energy_thresh_linear;

}srch_ngbr_srch_opt_log_struct_type;


LOG_RECORD_DEFINE(SRCH_NGBR_SRCH_OPT_LOG_PKT)

  uint8 version;                    /*Version Info*/
  uint8 num_freq;                  /*number of frequencies present in log packet*/
  uint8 quan_type;                 /*Measurement Quantity type*/
  uint8 detmargin_ecio;            /*Offset to calculate EcIo rank for Candidate neighbor*/
  uint8 detmargin_rscp;            /*Offset to calculate RSCP rank for Candidate neighbor*/
  uint8 prediction_margin;         /*Offset to calculate step1 energy threshold*/
  boolean mobility_state;          /*Flag to indicate the mobility of UE*/
  uint8 standard_deviation_value;  /*Standard deviation in RSCP values computed over the last 30sec interval*/
  uint8 serving_cell_min_ecio;     /*Minimum EcIo of the serving cell over last 30secs interval*/
  srch_ngbr_srch_opt_log_struct_type log_cell_ptr[L1_MAX_NUM_FREQ];

LOG_RECORD_END


/* Structure to input parms of NBR*/
typedef PACKED struct PACKED_POST {
    /* rotator frequency to be programmed to RXFE_NB_CSR_FREQ_OFFSET_NB0 */
    uint32 rotatorFreq;
    /*! num of bpgs to accumulate for NBEE */
    uint8 accumBpg;
    /* threshold1 - G-G */
    int8 threshold1;
    /* threshold2 - W-G */
    int8 threshold2;
 }wsrch_nbrej_log_input_struct_type;

/* Structure to hold output parms of NBR*/
typedef PACKED struct PACKED_POST {
  /* TRUE indicates non WCDMA signal*/
  boolean result;
  /* maximum NBEE power */
  uint32 max_g;
  /* minimum NBEE power */
  uint32 min_g;
  /* Difference between maximum NBEE power and minimum*/
  uint32 g_g_diff;
  /* Difference between maximum NBEE power and W power*/
  uint32 w_g_diff;
  /*RxAGC*/
  uint32 rx_agc;
  /*W accumulated power*/
  uint32 w_accum_power;
  /* narrow band accumulated power for all NBEE*/
  uint32 nb_accum_power[WSRCH_NB_REJ_NBEE_NUM_LOG];
}wsrch_nbrej_log_output_struct_type;


/* Structure to hold per bpg output parms of NBR*/
typedef PACKED struct PACKED_POST {
 /*per BPG power of W*/
 uint32 w_power_bpg;
 /*per BPG power of narrow band for each NBEE*/
 uint32 nb_power_bpg[WSRCH_NB_REJ_NBEE_NUM_LOG];
}wsrch_nbrej_log_output_per_bpg_struct_type;

LOG_RECORD_DEFINE(WSRCH_NB_REJ_LOG_PACKET)

  /* Version number */
  uint8 ver_num ;
  /*UARFCN on which NBREJ was run*/
  uint16 uarfcn;
  wsrch_nbrej_log_input_struct_type input;
  wsrch_nbrej_log_output_struct_type output;
  wsrch_nbrej_log_output_per_bpg_struct_type output_bpg[WSRCH_NB_REJ_ACCUM_BPG_LOG];
LOG_RECORD_END


#endif

/* Determine the sizes of Step1 log packet */
#define STEP1_PACKET_LEN(step1_peaks)  ( \
    FPOS(SRCH_STEP1_LOG_PACKET_type, srchlog_psc_peak_info)+  \
    ( step1_peaks * sizeof(srchlog_psc_peak_struct_type)))

/* Determine the sizes of Step1 log packet */
#define PARALLEL_STEP1_PACKET_LEN(step1_peaks)  ( \
    FPOS(SRCH_PARALLEL_STEP1_LOG_PACKET_type, srchlog_psc_peak_info)+  \
    ( step1_peaks * sizeof(srchlog_psc_peak_parallel_step1_struct_type)))

/* Determine the sizes of NASTT log packet */
#define NAS_TT_PACKET_LEN(nas_tt_peaks) ( \
    FPOS(SRCH_NAS_TT_LOG_PACKET_type, rslt)+  \
    ( nas_tt_peaks * sizeof(srchlog_nastt_rslt_struct_type)))

/* Determine the sizes of parallel NASTT log packet */
#define PARALLEL_NAS_TT_PACKET_LEN(nas_tt_peaks) ( \
		FPOS(SRCH_PARALLEL_NAS_TT_LOG_PACKET_type, rslt)+  \
		( nas_tt_peaks * sizeof(srchlog_nastt_rslt_struct_type)))


/* Determine the sizes of PN log packet */
#define PN_PACKET_LEN(num_tasks, num_results_per_task)  ( \
    FPOS(SRCH_PN_LOG_PACKET_type, task)+  \
     num_tasks * (sizeof(srchlog_pn_input_struct_type) + \
                  sizeof(srchlog_filt_eng_per_task_struct_type) + \
                  num_results_per_task*sizeof(srchlog_pn_peak_set_struct_type)))

/* Determine the size of the PN bypass peak log packet */
#define PN_BYPASS_PEAK_PACKET_LEN(num_tasks, num_results_per_task) (    \
        FPOS(SRCH_PN_BYPASS_PEAK_LOG_PACKET_type, task)               + \
        num_tasks * (sizeof(srchlog_pn_bypass_peak_input_struct_type) + \
                     num_results_per_task * sizeof(srchlog_pn_bypass_peak_set_struct_type)))

#define SRCH_ALL_CELLS_PN_PACKET_LEN(num_tasks, num_results_per_task)  ( \
    FPOS(SRCH_LIST_ALL_CELLS_LOG_PACKET_type, task)+  \
     num_tasks * (sizeof(srchlog_pn_input_struct_type) + \
                  sizeof(srchlog_filt_eng_per_task_struct_type) + \
                  num_results_per_task*sizeof(srchlog_pn_peak_set_struct_type)))

/* ASET search with DC information*/
#define ASET_PACKET_LEN_V2(n_cells, n_dc_cells)  ( \
       FPOS(ASET_LOG_PACKET_V2_type , srchlog_cell) + \
       ((n_cells + n_dc_cells) * sizeof(srchlog_cell_info_type)))
    
/* NSET search */
#define NSET_PACKET_LEN(n_freq, n_cell)  (\
    FPOS(NSET_LOG_PACKET_type, srchlog_freq) + \
    ( n_freq* ( sizeof(uint16) + sizeof(uint8) + \
      (n_cell*sizeof(nset_cell_info_type)) )  \
    ) )

/* Computing the log packet length for SEARCH_HIST_INFO_LOG_PACKET */
#define SEARCH_HIST_INFO_LOG_PACKET_LEN(len) \
  FPOS(SEARCH_HIST_INFO_LOG_PACKET_type, srch_hist_circ_buff) + \
  len * sizeof(wl1_circ_buff_srch_hist_struct_type)

/* Computing the log packet length for NASTT_HIST_INFO_LOG_PACKET */
#define NASTT_HIST_INFO_LOG_PACKET_LEN(len) \
  FPOS(NASTT_HIST_INFO_LOG_PACKET_type, nastt_hist_circ_buff) + \
  len * sizeof(wl1_circ_buff_nastt_hist_struct_type)

/* Computing the log packet length for NASTT_DETECT_HIST_INFO_LOG_PACKET */
#define NASTT_DETECT_HIST_INFO_LOG_PACKET_LEN(len) \
  FPOS(NASTT_DETECT_HIST_INFO_LOG_PACKET_type, nastt_detect_hist_circ_buff) + \
  len * sizeof(wl1_circ_buff_nastt_detect_hist_struct_type)

/* Computing the log packet length for CELL_HIST_INFO_LOG_PACKET */
#define CELL_HIST_INFO_LOG_PACKET_LEN(len) \
  FPOS(CELL_HIST_INFO_LOG_PACKET_type, cell_hist_circ_buff) + \
  len * sizeof(wl1_circ_buff_cell_hist_struct_type)

/* Computing the log packet length for ASET_FILT_HIST_INFO_LOG_PACKET */
#define ASET_FILT_HIST_INFO_LOG_PACKET_LEN(len) \
  FPOS(ASET_FILT_HIST_INFO_LOG_PACKET_type, aset_filt_hist_circ_buff) + \
  len * sizeof(wl1_circ_buff_aset_filt_hist_struct_type)

/* Computing the log packet length for SIRT_FILT_HIST_INFO_LOG_PACKET */
#define SIRT_FILT_HIST_INFO_LOG_PACKET_LEN(len) \
  FPOS(SIRT_FILT_HIST_INFO_LOG_PACKET_type, sirt_filt_hist_circ_buff) + \
  len * sizeof(wl1_circ_buff_sirt_filt_hist_struct_type)

/* Computing the log packet length for FILT_ECIO_HIST_INFO_LOG_PACKET */ 
#define FILT_ECIO_HIST_INFO_LOG_PACKET_LEN(len) \
  FPOS(FILT_ECIO_HIST_INFO_LOG_PACKET_type, filt_ecio_hist_circ_buff) + \
  len * sizeof(wl1_circ_buff_filt_ecio_hist_struct_type)

/* Computing the log packet length for RXD_FILT_HIST_INFO_LOG_PACKET */
#define RXD_FILT_HIST_INFO_LOG_PACKET_LEN(len) \
  FPOS(RXD_FILT_HIST_INFO_LOG_PACKET_type, rxd_filt_hist_circ_buff) + \
  len * sizeof(wl1_circ_buff_rxd_filt_hist_struct_type)

/* Cell Reselection Rank */
typedef log_hdr_type l1_srch_log_hdr_struct_type;

#ifdef FEATURE_WCDMA_L1_PSC_SCANNER
/* PSC Scanner Pilot Info */
#define PSC_SCAN_PACKET_LEN(num_peaks)  ( \
    FPOS(SRCH_PSC_SCAN_PILOT_INFO_LOG_PKT_type, srchmview_psc_scan_peak_info)+  \
    ( num_peaks * sizeof(srchmview_result_struct_type)) )
#endif

/* Determine the sizes of FREQ SCAN RESULT log packet */
#define FREQ_SCAN_PACKET_LEN(num_uarfcn)  ( \
    FPOS(SRCH_FREQ_SCAN_LOG_PACKET_type, result)+  \
    ( num_uarfcn * sizeof(srchlog_freq_scan_info_struct_type)))

/* Determine the sizes of NBREJ log packet */
#define WSRCH_NB_REJ_LOG_PACKET_LEN(num_bpg)  ( \
    FPOS(WSRCH_NB_REJ_LOG_PACKET_type,output_bpg)+ \
    ( num_bpg * sizeof(wsrch_nbrej_log_output_per_bpg_struct_type)))


#endif /* SRCHLOG_H */
