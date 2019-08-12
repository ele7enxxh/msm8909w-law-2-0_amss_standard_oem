#ifndef MACINTERNAL_H
#define MACINTERNAL_H

/*==================================================================

   MAC LAYER - INTERNAL DATA H E A D E R    F I L E

DESCRIPTION
  This file contains the definition of the data types that are
  internally used used by MAC subsystem software.

  This file is included only in MAC subsystem modules.

  Copyright (c) 2001 by Qualcomm Technologies, Inc.
  All Rights Reserved.
  Copyright (c) 2002-2005, 2007-2009 by Qualcomm Technologies, Inc.All Rights Reserved.

====================================================================*/

/*=====================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mac/vcs/macinternal.h_v   1.21   10 Jul 2002 12:50:28   skrishna  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/mac/inc/macinternal.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------
05/28/13   kc      Change for MRM-ID extension for Rel-9
10/12/12   ts      Reordering of structure elements for memory optimization
07/13/12   grk     MBMS feature cleanup at L2(MBMS_DEBUG_1, FEATURE_MODEM_MBMS)
08/13/12   mrg     HS-RACH TVM issues (Multiple TVMs sent , 4A/4B back to backcontinous reported)
                   1)Multiple unnecessary Events 4A-4B triggered are avoidede now
                   2)When Prohibit timer is running Timer_To_Trigger is not running – So it will delay TVM event triggering by PTT+TTT -fixed
                   3)During DCH<->RACH/FACH transition TRCH are added with our checking for TRCH type 
01/02/12   grk     Feature cleanup
12/15/10   vp      Fixed compiler warning
10/01/10   mrg     TVM event and periodic measurements handled with different timers
09/16/10   mrg     Moved auido(MVS) dependency under the feature flag FEATURE_MVS
10/07/09   kvk     Fixed featurization for WPLT MAC compilation
06/18/09   mrg     lowest_amr_rate_tfci variable is added to mac_amr_mode_info_type 
                   to store the TFCI index for which lowest possible AMR rate allowed 
05/29/09   ssg     Added support for SNOW_3G algo.
05/07/09   ssg     Updated Copyright Information
04/03/08   sk      Renamed FEATURE_MBMS to FEATURE_MODEM_MBMS
03/24/08   pj      Added mac_id to structure dlc_dch_config_info_type.
01/29/08   sk      Support for MBMS under FEATURE_MODEM_MBMS
09/05/07   sk      Added the pdu_size in the internal data structure under FEATURE_RLC_ENH_BUFFER_STATUS
07/19/05   sk      Added structure mac_amr_mode_info_type to store the lowest_amr_rate,the
                   highest amr rate that is available based on the TFCS table.Added
                   mac_amr_rate_e_type enum to indicate if it is a single rate, multi rate AMR call.
02/15/05   gsc     Added HSDPA support under FEATURE_HSDPA feature flag.
12/30/04   sk      Fixed lint errors
09/28/04   sk      Added marked to field.
09/19/04   sk      Added mac_ciphering_table which will maintain for each cfn the key, the
                   cipher_enable flag and the algorithm
07/24/04   sk      Added evt_not_first_meas to maci_ul_traffic_trch_event_param. This variable
                   will be associated with each particular transport channel.
07/16/04   sk      Added tfcs_control_ptr_in_use_e_type enum under FEATURE_TFCS_CONTROL_ENABLE
04/14/04   sk      Added a structure maci_ul_trch_tf_type.
12/26/03   sk      Added support for TFCS selection based on logical channel prioritites
                   under FEATURE_TFCS_LC_PRIORITIES
04/29/03   sk      Added uecomdef_logchan_e_type. This will save the channel type for DCH. Its under
                   FEATURE_TC_CONSTANT_DELAY.
02/13/03   sk      Changed num_events to num_events[UE_MAX_TRCH] in maci_ul_traffic_cfg_param.
12/31/02   sk      Added power_state, tblock to ul_tfcs_info_type under FEATURE_PWR_TFC_ELMN
12/24/02   sk      Added tfc_sig_bo and tfc_data_bo to ul_tfcs_info_type.
11/15/02   sk      Removed cfg_ind from maci_ul_traffic_cfg_param struct
09/26/02   sk      Added suspend_meas to maci_ul_traffic_cfg_index_type.
09/18/02   sk      Added RACH_WAITING_FOR_RIGHT_TTI to rach_state_e_type
09/04/02   sk      * ul_trch_info_type structure: Added boolean cs_or_ps_domain to indicate
                   if this transport channel belongs to CS or the PS domain.
                   * dl_dch_config_info_type structure : Added boolean cs_or_ps_domain to indicate
                   if this transport channel belongs to CS or the PS domain.
07/10/02   sk      Changed the size of report_threshold from uint16 to uint32
06/08/02   sk      Removed FEATURE_TRAFFIC_VOLUME_MEAS.
05/20/02   sk      Added enum traffic_meas_cfg_e_type for traffic volume measurements.
05/10/02   sk      Added data structures maci_ul_traffic_cfg_index_type,
                   maci_ul_traffic_cfg_param, maci_ul_traffic_trch_event_param
                   to support traffic volume measurements. The additions
                   are made under FEATURE_TRAFFIC_VOLUME_MEAS
11/03/01   ps      Added "highest_tti" field to "int_ul_tfcs_info" structure
09/19/01   ps      Added "ntb" field to "ul_trch_info_type"

=====================================================================*/

#include "wcdma_variation.h"
#include        "uecomdef.h"
#include        "macrlcif.h"
#ifdef FEATURE_WCDMA_MULTIRATE_AMR
#ifdef FEATURE_MVS
#include "mvs.h"
#endif  /* FEATURE_MVS */
#endif

/*===================================================================

                     CONSTATNT DATA DEFINITIONS

====================================================================*/

/* Minimum value of DL CCTrCh ID */
#define     MAC_DL_MIN_CCTRCH_ID        0

/* Maximum value of DL CCTrCh ID */
#define     MAC_DL_MAX_CCTRCH_ID        (UE_MAX_CCTRCH + MAC_DL_MIN_CCTRCH_ID - 1)

/* Minimum value of DL TrCh ID */
#define     MAC_DL_MIN_TRCH_ID          1

/* Maximum value of DL TrCh ID */
#define     MAC_DL_MAX_TRCH_ID          32

/* Minimum value of UL TrCh ID */
#define     MAC_UL_MIN_TRCH_ID          1

/* Maximum value of UL TrCh ID */
#define     MAC_UL_MAX_TRCH_ID          32

/* Minimum value of DL RLC ID */
#define     MAC_DL_MIN_RLC_ID           0

/* Maximum value of DL RLC ID */
#define     MAC_DL_MAX_RLC_ID           (UE_MAX_DL_LOGICAL_CHANNEL + MAC_DL_MIN_RLC_ID - 1)

/* Minimum value of UL RLC ID */
#define     MAC_UL_MIN_RLC_ID           0

/* Maximum value of UL RLC ID */
#define     MAC_UL_MAX_RLC_ID           (UE_MAX_UL_LOGICAL_CHANNEL + MAC_UL_MIN_RLC_ID - 1)

/* Minimum value of DL Radio Bearer ID */
#define     MAC_DL_MIN_RB_ID            0

/* Maximum value of DL Radio Bearer ID */
#define     MAC_DL_MAX_RB_ID            (MAX_RB + MAC_DL_MIN_RB_ID - 1)

/* Minimum value of UL Radio Bearer ID */
#define     MAC_UL_MIN_RB_ID            0

/* Maximum value of UL Radio Bearer ID */
#define     MAC_UL_MAX_RB_ID            (MAX_RB + MAC_UL_MIN_RB_ID - 1)

/* Minimum value of MAC ID of DCCH/DTCH */
#define         MAC_MIN_MAC_ID          1

/* Maximum value of MAC ID of DCCH/DTCH */
#define         MAC_MAX_MAC_ID          15

/* Minimum value of UL Access Service Class ID */
#define         MAC_UL_MIN_ASC_ID       0

/* MAximum value of UL Access Service Class ID */
#define         MAC_UL_MAX_ASC_ID       7

/* Minimum value of UL logical channel priotity */
#define         MAC_UL_MIN_MLP_ID       1

/* MAximum value of UL logical channel priority */
#define         MAC_UL_MAX_MLP_ID       8

/* Maximum number of MAC logical channel priorities */
#define         MAC_UL_MAX_MLPS         (MAC_UL_MAX_MLP_ID - MAC_UL_MIN_MLP_ID + 1)

/* Maximum size of uplink MAC PDU header */
#define         MAC_UL_MAX_HDR_SIZE     5

/* Minimum value of DL transport block ID */
#define         MAC_DL_MIN_TB_ID        0

/* Maximum value of DL transport block ID */
#define         MAC_DL_MAX_TB_ID        (UE_MAX_DL_RLC_PDUS + MAC_DL_MIN_TB_ID - 1)

/* Maximum number of traffic volume measurements */
#define     MAC_UL_MAX_MEAS_ID          32

/* Minimum measurement identity that a traffic volume measurement can have */
#define     MAC_UL_MIN_MEAS_ID          1

#define     MAC_UL_TRAFFIC_VOLUME_TMR_BASE     10

/* Number of traffic volume measurements that MAC supports */
#define     MAC_UL_NUM_TRAFFIC_MEAS     6

/* Number of traffic volume measurements that can be present at a time */
#define     UE_NUM_TRAFFIC_MEAS                (MAC_UL_MAX_MEAS_ID-MAC_UL_MIN_MEAS_ID+1 )


/*The periodic timer reset value can be set using this multiplication factor */
#ifdef FEATURE_WCDMA_TVM_PERIODIC_TIMER_ENH
#error code not present
#else
#define     MAC_UL_PERIODIC_TVM_TIMER_SCALE        1
#endif

/*===================================================================

                     EXTERN DATA DECLARATIONS

====================================================================*/
/* Once the ciphering_grabled message is set to TRUE the debug print messages
would eb printed every 500 ms. */
extern uint8 mac_dl_debug_print_interval;

/*============================================================================================

                     ENUMERATED TYPE DATA DEFINITIONS

=============================================================================================*/

/*--------------------------------------------------------------------------------------------
 RACH State
--------------------------------------------------------------------------------------------*/
typedef enum
{
  RACH_INVALID_STATE= 0,
  RACH_IN_IDLE,
  RACH_WAITING_FOR_STATUS,
  RACH_RCVD_NOACK,
  RACH_IN_BACKOFF,
  RACH_PERSISTENCE_FAILED,
  RACH_WAITING_FOR_RIGHT_TTI
} rach_state_e_type;


typedef enum
{
  CFG_INVALID=0,
  CFG_COMPLETE,
  CFG_INCOMPLETE
} traffic_meas_cfg_e_type;

#ifdef FEATURE_WCDMA_PWR_TFC_ELMN

typedef enum
{
  SUPPORTED_STATE=0,
  EXCESS_PWR_STATE,
  BLOCKED_STATE
} tfc_power_state_e_type;

#endif


typedef enum
{
  FULL_TFCS_TABLE=0,
  DEFAULT_TFCC_TABLE,
  CURRENT_TFCC_TABLE
}tfcs_control_ptr_in_use_e_type;


#ifdef FEATURE_WCDMA_MULTIRATE_AMR

typedef enum
{
  SINGLE_RATE_AMR=0,
  MULTI_RATE_AMR,
  AMR_RATE_UNDEFINED
}mac_amr_rate_e_type;

#endif

/*============================================================================================

                     STRUCTURE TYPE DATA DEFINITIONS

============================================================================================*/
/*---------------------------------------------------------------------
 DL Transport Block Information

 During pass 1 (header decoding), MAC determnines and saves the RLC ID
 of each transport block in this table, Uses the table information
 during pass 2 (data indication)
----------------------------------------------------------------------*/
typedef struct
{
  /* TRUE indicates the TB as valid */
  /* FALSE otherwise */
  boolean                 valid;

  /* DL RLC logical channel ID */
  rlc_lc_id_type  rlc_id;

}dl_tb_info_type;

/*---------------------------------------------------------------------
 Transport Channel ID to Transport Channel Index Table
 ----------------------------------------------------------------------*/
typedef struct
{
  /* TRUE     indicates that this transport channel is valid */
  boolean                     valid;

  /* Transport Channel Index */
  uint8                       idx;

}trch_id_to_idx_map_type;

/*---------------------------------------------------------------------
 BCH CONFIG TABLE

 The CCTrCh config info received from RRC is validated and stored in
 this structure. The information is used during header decoding
----------------------------------------------------------------------*/
typedef struct
{
  /* TRUE             if transport channel is enabled */
  /* FALSE    otherwise */
  boolean                     bch_enable;

  /* RLC ID of the logical channel to which this TrCh is mapped */
  rlc_lc_id_type      bcch_id;

}bch_config_info_type;

/*---------------------------------------------------------------------
 PCH CONFIG TABLE

 The CCTrCh config info received from RRC is validated and stored in
 this structure. This information is used during header decoding.
----------------------------------------------------------------------*/
typedef struct
{
  /* TRUE if the transport channel is enabled */
  /* FALSE otherwise */
  boolean                     pch_enable;

  /* RLC ID of the logical channel to which this TrCh is mapped */
  rlc_lc_id_type      pcch_id;

} pch_config_info_type;

/*---------------------------------------------------------------------
 Dedicated Logical Channel mapped to DSCH

 The info of DSCH-Dedicated Logical Channels received from RRC is
 validated and stored in this structure.
----------------------------------------------------------------------*/
typedef struct
{
  /* DSCH Enable Flag */
  boolean                     enable;

  /* RLC ID */
  rlc_lc_id_type      rlc_id;

  /* Radio Bearer ID */
  rb_id_type          rb_id;

  /* RLC Mode */
  uecomdef_logch_mode_e_type          rlc_mode;

}dlc_dsch_config_info_type;

/*---------------------------------------------------------------------
 DSCH CONFIG TABLE

 The Transport Channel config info received from RRC is validated and
 stored in this structure.
----------------------------------------------------------------------*/
typedef struct
{
  /* CCTrCh ID */
  cctrch_id_type       cctrch_id;

  /* Transport channel ID */
  tr_ch_id_type   trch_id;

  /* Number of dedicated logical channels mapped */
  uint8                        num_dlc;

  /* Dedicated logical channel configuration */
  dlc_dsch_config_info_type    dlcinfo[MAX_DED_LOGCHAN_PER_TRCH];

}dsch_config_info_type;

/*---------------------------------------------------------------------
 Config info of dedicated logical channel mapped to DL DCH
----------------------------------------------------------------------*/

typedef struct
{
  /* TRUE         if the DLC is enabled */
  /* FALSE        otherwise */
  boolean    enable;

  /* logical channel id received from RRC/NW <1..15>
   - valid for HS only */
  uint8      mac_id;

  /* RLC ID */
  rlc_lc_id_type      rlc_id;

  /* RAdio bearer ID */
  rb_id_type          rb_id;

  /* RLC mode */
  uecomdef_logch_mode_e_type  rlc_mode;

//#ifdef FEATURE_TC_CONSTANT_DELAY
  /* DCCH or DTCH */
  uecomdef_logchan_e_type chan_type;
//#endif

}dlc_dch_config_info_type;

/*---------------------------------------------------------------------
 DCH CONFIG TABLE

 The Transport Channel config info received from RRC is validated and
 stored in this structure.
----------------------------------------------------------------------*/
typedef struct
{
  /* CCTrCh ID */
  cctrch_id_type       cctrch_id;

  /* Transport channel ID */
  tr_ch_id_type   trch_id;

  uint8   mac_d_flow_id;

  /* Number of DCCH/DTCHs */
  uint8                        num_dlc;

  /* Dedicated logical channel configuration */
  dlc_dch_config_info_type    dlcinfo[MAX_DED_LOGCHAN_PER_TRCH];

  /* 0(False) indicates it is CS domain, 1(TRUE) indicates it is PS domain */
  boolean    cs_or_ps_domain;

}dl_dch_config_info_type;
/*---------------------------------------------------------------------
 Config info of dedicated logical channel mapped to FACH
----------------------------------------------------------------------*/
typedef struct
{
  /* TRUE if dedicated logical channel is enabled */
  /* FALSE otherwise */
  boolean              enable;

  /* Dedicated logical channel ID */
  rlc_lc_id_type               rlc_id;

}dlc_fach_config_info_type;

/*---------------------------------------------------------------------
 FACH CONFIG TABLE

 The Transport channel config info received from RRC is validated and
 stored in this structure.
----------------------------------------------------------------------*/
typedef struct
{
  /* ID of the CCTrCH to which this transport channel mapped */
  cctrch_id_type       cctrch_id;

  /* Transport channel ID */
  tr_ch_id_type   trch_id;

  /* TRUE if BCCH is enabled */
  /* FALSE otherwise */
  boolean                      bcch_enable;

  /* BCCH ID */
  rlc_lc_id_type       bcch_id;

  /* TRUE if CCCH is enabled */
  /* FALSE otherwise */
  boolean                      ccch_enable;

  /* CCCH ID */
  rlc_lc_id_type       ccch_id;

  /* TRUE if CTCH is enabled */
  /* FALSE otherwise */
  boolean                      ctch_enable;

  /* CTCH ID */
  rlc_lc_id_type       ctch_id;

  /* TRUE if there is at least one DCCH/DTCH */
  /* FALSE otherwise */
  boolean                      dlc_enable;

  /* Number of DCCH/DTCHs */
  uint8                        num_dlc;

  /* Dedicated logical channel info */
  dlc_fach_config_info_type     dlcinfo[MAX_DED_LOGCHAN_PER_TRCH];

}fach_config_info_type;

/*----------------------------------------------------------------------------------------*/
/* UL TFCS information used internally by MAC                                                                                     */
/*----------------------------------------------------------------------------------------*/
typedef struct
{
  

  /* TFC Buffer Occupancy */
  uint32                    tfc_bo;

  /* TFC buffer occupancy contributed by data on CCCH & DCCH */
  uint32                    tfc_sig_bo;

  /* TFC buffer occupancy contributed by data on DTCH */
  uint32                    tfc_data_bo;

  /* This TFC was */
  boolean         marked[UE_MAX_TRCH];

  /* TFC bo at this priority level */
  uint32          tfc_priority_bo[MAC_UL_MAX_MLPS];

  /* This denotes the number of bits avaialble for this TFC */
  uint16          tfc_priority_ntb[MAC_UL_MAX_MLPS];

  /* This denotes the still available space for each transport channel
  in this TFC */
  uint16          trch_available_ntb[UE_MAX_TRCH];

#ifdef FEATURE_WCDMA_PWR_TFC_ELMN
  /*Timer in MAC will be based on the build frame indication that MAC gets from L1.
  This timer will keep count of the time this TFC is in Excess power state and will
  be used to transition to Blocked state if needed. */
  uint16                          tblock;
   /*Indicates if the TFC is in Supported, Excess or Blocked state.*/
  tfc_power_state_e_type          power_state;

#endif
/* TFC Control Flag -- TFC_ALLOWED or TFC_NOT_ALLOWED */
  mac_ul_tfc_control_e_type control_flag;
  /* Highest TTI in this TFC */
  l1_tti_enum_type          highest_tti;

}ul_tfcs_info_type;

/*--------------------------------------------------------------------------------
For each transpport channel, this structure would indicate the allowed tf's for this
channel. and the number of pdus that can be built. This will later be used to
eliminate tfc's when npdus is les than the tb_size for a particular transport
channel adn a particular transport format.
---------------------------------------------------------------------------------*/
typedef struct
{
  uint16  ntblks;
  uint16  tb_size;
  uint16  npdus;
  boolean allowed;

}maci_ul_trch_tf_type;

/*----------------------------------------------------------------------------------------*/
/* UL Transport Channel information                                                                                                               */
/*----------------------------------------------------------------------------------------*/
typedef struct
{
  /* Number of logical channels */
  /* This value is set on receiving CMAC_UL_CONFIG_REQ */
  uint8                           nlchan;

  /* Transport channel ID */
  tr_ch_id_type           trch_id;

  /* Logical Channel IDs */
  /* This value is set on receiving CMAC_UL_CONFIG_REQ */
  rlc_lc_id_type          rlc_id[UE_MAX_UL_LOGICAL_CHANNEL];

  /* TFI of the previous TTI boundary */
  /* This value is cleared on receiving CMAC_UL_CONFIG_REQ */
  uint8                           prev_tfi;

  /* Flag indicating whether data was transmitted on previous TTI boundary or not */
  boolean                         prev_tx_flag;

  /* Number of actual Transport Blocks in TTI */
  uint8               ntb;

  /* Indicates if the domain is CS or PS */
  boolean   cs_or_ps_domain;

}ul_trch_info_type;

/*----------------------------------------------------------------------------------------*/
/* UL Coded Composite Transport Channel information                                                                               */
/*----------------------------------------------------------------------------------------*/
typedef struct
{
  /* CCTrCh Type */
  mac_ul_cctrch_e_type    cctrch_type;

  /* Number of Transport Channels */
  /* This value =1 if the CCTrCh type = RACH */
  /* This value can be any value between 1 and Maximum Number of Transport Channels */
  uint8   ntrch;

  /* Selected TFCI */
  uint16  selected_tfci;

}ul_cctrch_info_type;

/*------------------------------------------------------------------*/
/* UL Logical channel information                                   */
/*------------------------------------------------------------------*/
typedef struct
{

  /* TRUE indicates logical channel is enabled */
  boolean                                         enable;

  /* Logical channel type */
  uecomdef_logchan_e_type         lc_type;

  /* MAC Header size */
  uint8                                           mac_hdr_size;

  /* MAC header value */
  uint8                                           mac_hdr_value[MAC_UL_MAX_HDR_SIZE];

  /* RLC mode */
  uecomdef_logch_mode_e_type      rlc_mode;

  /* Radio bearer identity */
  rb_id_type                                      rb_id;

  /* Transport Channel Index */
  uint8                                           trch_idx;

  /* Access Service Class (in case of CCCH) or */
  /* MAC Logical Channel Priority (in case of DCCH/DTCH) */
  uint8                                           asc_or_mlp;

  /* RLC Header Length in bits */
  uint8                                           rlc_hdr_length;

  /* Lists the rlc sizes that are valid for this logical channel */
  mac_ul_rlc_size_info_type                      rlc_size_info;

  uint32                                         pdu_size;

}ul_lc_info_type;

typedef struct
{
  /* the event could be either event_4a or event_4b */
  mac_ul_traffic_event_id_e_type  event_id;

  boolean   evt_not_first_meas;
  tr_ch_id_type   first_event_trch_id;

  /* The previous value of buffer occupancy */
  uint32    prev_bo;

  /* threshold in bytes (8,16,32,64,...) */
  uint32    report_threshold;

  /* Indicates the period of time in ms during which the event has to be satisfied for*/
  /* MAC to trigger a report */
  uint16    time_to_trigger;

  /* Indicates the period of time during which it is forbidden to send any new report */
  /* with the same traffic volume event identity even if the triggering condition is  */
  /* fulfilled */
  uint16    trigger_prohibit_tmr;

  /* Indicates how long the UE shall block DTCH transmissions on the RACH after a   */
  /* measurement report is triggered */
  uint16    tx_interrupt_tmr;

  /* Counter to keep track of Time to trigger*/
  uint16    trigger_tmr_cnt;

  /*Counter to keep track of Trigger prohibit timer */
  uint16    prohibit_tmr_cnt;

}maci_ul_traffic_trch_event_param;

/*-------------------------------------------------------------------------------*/
/* MAC stores the config information corresponding to each measurement id        */
/*-------------------------------------------------------------------------------*/

typedef struct
{
  /* False indicates apply measurement for all uplink transport channels */
  /* True indicates that apply measurement only for those specified in   */
  /* the object list */
  boolean                                       meas_obj_list_incl;

  /* Number of transport channels */
  uint16                                        num_trch;

  /* Transport channel id and type information for each of these transport channels */
  mac_ul_traffic_trch_info_type                 trch_info[UE_MAX_TRCH];

  /* If set to true include raw rlc payload measurement information in the report*/
  boolean                                       rlc_payload;

  /* If set to true include average payload measurement information in the report*/
  boolean                                       avg_payload;

  /* If set to true include variance payload measurement information in the report*/
  boolean                                       var_payload;

  /* If the reporting quantity is avergae or variance then the interval to take the average
  or variance is given by meas_interval */
  uint16                                        nsamples;

  /* Contains the report mode-indicates if it is periodic/event_triggered*/
  mac_ul_traffic_report_mode_e_type             report_mode;

  /* Contains the report transfer mode*/
  mac_ul_traffic_report_transfer_mode_e_type    transfer_mode;

  /* first meas is set to true initially. TRue indicates this is the
  first time a measurement report is going to be posted for this meas id */
  boolean                                       first_meas;

  /*-----------------------------------------------------------------------------------
      The following parameters are used when the mode is periodic
  -----------------------------------------------------------------------------------*/
  /* True indicates there is no limit on the number of reports that we send to RRC    */
  /* False indicates,send only the number of reports that are specified in num_reports*/
  boolean                                       report_infinity;

  /* The number of reports that can be sent */
  uint16                                        num_reports;

  /* Indicates in milliseconds the interval of periodic report */
  uint16                                        report_interval;


  /* In the case of periodic if there is a limit on number of reports that we   */
  /* need to send then num_reports_sent will tell us how many reports have been */
  /* sent so far                                                                */
  uint16                                        nreports_sent;

  /* This counter will keep track if the timer has expired */
  uint32                                        periodic_tmr_cnt;

  /*-----------------------------------------------------------------------------------
      The following parameters are used when the mode is event_triggered.
  -----------------------------------------------------------------------------------*/
  boolean                                       event_trch_incl;

  uint16                                        event_num_trch;

  /* Transport channel id and type information for each of these transport channels */
  mac_ul_traffic_trch_info_type                 event_trch_info[UE_MAX_TRCH];

  /* Number of events for each transport channel that is specified in event_trch_info*/
  uint16                                        num_events[UE_MAX_TRCH];

  /* Event paramaeters for each of these transport channels */
  maci_ul_traffic_trch_event_param              trch_event_param[UE_MAX_TRCH][2];

  /* Stores the event_id and the transport channel on which the event occurred */
  mac_ul_traffic_event_result                   event_result;

  /* Stores the index from which samples have to be taken for computing the raw, avg, variance*/
  uint16                                        current_idx;

}maci_ul_traffic_cfg_param;

typedef struct
{
  /* True indicates if this entry has valid parameters in it */
  boolean                     valid;

  /* TRUE indicates that the measurements are temporarily suspended.
  FALSE indicates there is no such restraint on this measurement id */
  boolean                     suspend_meas;

  /* Pointer to the config information */
  maci_ul_traffic_cfg_param   *traffic_cfg_ptr;

}maci_ul_traffic_cfg_index_type;


typedef struct
{

  traffic_meas_cfg_e_type  cfg_e_type;

  /* Number of transport channels */
  uint16                            ntrch;

  /* Transport channel type and id for each of these transport channels*/
  mac_ul_traffic_trch_info_type     trch_info[UE_MAX_TRCH];

}maci_ul_traffic_obj_list_cfg_type;

/*-----------------------------------------------------------------------------
For a particular transport channel with a particular priority, this
structure gives the list of the logical channels mapped to this transport
channel, the rlc size buffer occupancy and the total buffer occupancy
information is stored.
-----------------------------------------------------------------------------*/

typedef struct
{
  uint8           num_lc;
  rlc_lc_id_type  lc_id[UE_MAX_UL_LOGICAL_CHANNEL];
  uint32          rlc_size_bo[UE_MAX_TF];
  uint32          total_bo;
  /* This flag indicates if pdus were picked up from this logical
  channel */
  uint8           start_idx;

}maci_ul_trch_priority_info_type;

typedef struct
{
  /* The ciphering algorithm can be either UE_0 or UE_1 */
  uecomdef_wcdma_cipher_algo_e_type     cipher_algo;

  /* Cipher Enable flag */
  /* TRUE if ciphering is enabled */
  /* FALSE otherwise */
  boolean     cipher_enable;

  /* Ciphering key index */
  uint8       key_idx;

}mac_ciphering_table;

#ifdef FEATURE_WCDMA_MULTIRATE_AMR

typedef struct
{
  /* This is the rate that MVS will be configured with */
  mvs_amr_mode_type  amr_rate;

  /* Among the tfci's that are given this is the lowest possible rate. In case
  of multirate amr call this rate will be put in the minimum set */
  mvs_amr_mode_type  lowest_amr_rate;

  /* TFCI index for which lowest possible AMR rate allowed */
  uint16    lowest_amr_rate_tfci;
  
  /* Enum value indicates if it is SINGLE_RATE, MULTI_RATE or UNDEFINED. */
  mac_amr_rate_e_type   amr_rate_type;

}mac_amr_mode_info_type;

#endif

#endif // macinternal_h


