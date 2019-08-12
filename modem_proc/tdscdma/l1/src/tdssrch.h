#ifndef TDSSRCH_H
#define TDSSRCH_H
/*===========================================================================
                S E A R C H      H E A D E R      F I L E

GENERAL DESCRIPTION:
  This file contains basic definitions used all other srch files or 
  external modules.

EXTERNALIZED FUNCTIONS

 srch_get_cpich_rscp
  This function retruns serving cell's RSCP.

 tdssrch_get_curr_freq
  This function returns current frequency on which UE is camped.

 srch_get_filtered_rscp
  This function returns an unsigned value representing the current,
  filtered RSCP value in P(AGC) + Ec/Io form for all ASET cells. 
  
  The returned can then be arbitrarily mapped to UI display space 
  using an equation, table, or case statement.  
  
  The RSCP is filtered using an (N-1)/N IIR filter for a smoother signal
  strength display. 

 srch_compute_rscp
  This function calculates the RSCP given the ecIo and the RxAGC..
  The calculated RSCP value will be returned in uint16 units. The actual RSCP
  is -ve of the returned value.

 srch_async_parm_init
  This function initializes searcher asynchronous parameters depending
	on the current state.

 srch_event_timer_elapsed
  This function returns the time elapsed from start_ts passed to it.

 nset_process_log_on_demand
  This funtion creates a NSET log packet and submits it to QXDM

 aset_process_log_on_demand
  This funtion creates a ASET log packet and submits it to QXDM

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS  
  

Copyright(c) 2000-2011 by Qualcomm Technologies Incorporated.
All Rights Reserved.
      
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "tdscdma_variation.h"
#include "comdef.h"
#include "tdssrchset.h"
#include "tfw_sw_intf_cell.h"
#include "tfw_sw_intf_srch.h"
#include "tfw_sw_intf_msg.h"
#include "tdsl1_nv.h"

#ifdef FEATURE_QSH_EVENT_METRIC
#include "tdsl1_qsh_ext.h"
#endif

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#ifdef FEATURE_TDSCDMA_REPEAT_MR 
typedef struct
{
  boolean flag;     //tmrpt valid flag
  boolean expired;  //tmrpt expired flag
  uint16 counter;  //tmrpt timer counter for check whether expired
  uint8  rpt_times; //repet send times
}tdssrchdch_mrpt_timer_t;

extern tdssrchdch_mrpt_timer_t tdssrchdch_1g_repeat_send_timer[TDSSRCHDCH_MAX_INTRAF_EVT];
extern tdssrchdch_mrpt_timer_t tdssrchdch_2a_repeat_send_timer[TDSSRCHDCH_MAX_INTERF_EVT];
#endif

extern tdsl1_nv_fast_ho_control_struct_type2 tdsl1_fast_ho_control_struct_type2;


#define TDSSRCHDCH_MRDELAY_TIMER         tdsl1_fast_ho_control_struct_type2.tdssrchdch_evt_1G2A_Delay_after_3A
#define TDSSRCHDCH_MRPRT_1G_TIMER        tdsl1_fast_ho_control_struct_type2.tdssrchdch_evt_1G_MR_Repeat_Interval
#define TDSSRCHDCH_MRPRT_2A_TIMER        tdsl1_fast_ho_control_struct_type2.tdssrchdch_evt_2A_MR_Repeat_Internal
#define TDSSRCHDCH_1G_MAX_REPEAT_TIMES   tdsl1_fast_ho_control_struct_type2.tdssrchdch_evt_1G_MR_Repeat_Number
#define TDSSRCHDCH_2A_MAX_REPEAT_TIMES   tdsl1_fast_ho_control_struct_type2.tdssrchdch_evt_2A_MR_Repeat_Number
#define TDSSRCHDCH_MIN_RSCP_X256         (256 * tdsl1_fast_ho_control_struct_type2.tdssrchdch_evt_1G2A_RSCP_Min_Threshold)
#define TDSSRCHDCH_MIN_RSCP_X4           (4 * tdsl1_fast_ho_control_struct_type2.tdssrchdch_evt_1G2A_RSCP_Min_Threshold)
#define TDSSRCHDCH_MIN_RSCP              tdsl1_fast_ho_control_struct_type2.tdssrchdch_evt_1G2A_RSCP_Min_Threshold
#define TDSSRCHDCH_DELAY_AFTER_SETUP_RECONFIG tdsl1_fast_ho_control_struct_type2.tdssrchdch_evt_Delay_Fast_HO_After_Setup_Reconfig




extern uint16 tdssrch_default_pathloss;

#ifdef FEATURE_JDS_OPT
#ifdef FEATURE_TDSCDMA_NON_TS0_CELL_PRUNING

/* Provide a debug way to hack GP_N0 power. 
 * Since GP_N0 is high in FT, currently system suggest hack to -110dBm. System 
 * will continue to work out better solution.
 */
extern int16 tdssrch_gp_n0_pwr;

#endif /* FEATURE_TDSCDMA_NON_TS0_CELL_PRUNING */
#endif /* FEATURE_JDS_OPT */

typedef struct
{
  union
  {
    /** sub frame count and chip x8 count */
    struct
    {
      uint32  ts0StartPosition : 16;
      uint32  subFrmCnt : 13;
      uint32  : 3;
    };
    /*! \brief Offset counter including both subframe and chipx8 count */
    uint32  wallTimeOfs;
  };

  int32  totalFreqError;
  int32  dwptsFiltFreqError;
  int32  midFiltFreqError;  
} tdssrch_cell_tracking_info_structure_type;
extern tdssrch_cell_tracking_info_structure_type  tdssrch_cell_tracking_info;

#ifdef FEATURE_TDSCDMA_LTE_MEAS_TIMING_IMPROVEMENT
#define TDSSRCH_GET_SERVING_CELL_RX_TIME() (tdssrch_cell_tracking_info.wallTimeOfs)
#endif

typedef enum
{
  /* When we camp on a cell (cell selection) and during cell transition */
  TDSSRCHCMD_ASET_UPD_CELL_CAMP,
  /* Hard handover */
  TDSSRCHCMD_ASET_UPD_CELL_HHO,
  /* BHO or HHO failure, then recover to old cell */
  TDSSRCHCMD_ASET_UPD_CELL_HO_RECOVERY,
  /* Baton handover with 80ms gap for uplink cell */
  TDSSRCHCMD_ASET_UPD_UL_CELL_80MS_BHO,
  /* Baton handover with 0ms gap for uplink cell */
  TDSSRCHCMD_ASET_UPD_UL_CELL_0MS_BHO,
  /* Baton handover with 80ms gap for uplink cell */
  TDSSRCHCMD_ASET_UPD_DL_CELL_80MS_BHO,
  /* Baton handover with 0ms gap for uplink cell */
  TDSSRCHCMD_ASET_UPD_DL_CELL_0MS_BHO,
  /* Serving cell doesn't change, but working frequency changes */
  TDSSRCHCMD_ASET_UPD_WORKING_FREQ_CHANGED,
  /* In DRX mode,go to neighbor cell */
  TDSSRCHCMD_ASET_UPD_TO_NEIGHBOR_CELL_DRX,
  /* In DRX mode,come back to serving cell */
  TDSSRCHCMD_ASET_UPD_BACK_SERVING_CELL_DRX,
  /* For resume skip reacq, force SRCH to do ASET UPDATE */
  TDSSRCHCMD_ASET_UPD_FORCE_TO_DO
} tdssrchcmd_aset_update_enum_type;

typedef struct
{
  tdssrchcmd_aset_update_enum_type upd_type;
  uint16 primary_freq;
  uint16 working_freq;
  uint8 cpi;
  /* Subframe time when aset update begins */
  uint16  startSubframe;
} tdssrchcmd_aset_update_cmd_type;

extern tdssrchcmd_aset_update_cmd_type pending_aset_update_cmd;
extern tdssrchcmd_aset_update_cmd_type *pending_aset_update_cmd_ptr;



/*--------------------------------------------------------------------------
                         SEARCHER OPERATION TYPE
--------------------------------------------------------------------------*/

typedef struct
{
  boolean periodic_meas_rpt; /* Periodic meas report */
  uint16  meas_id;      /* Meas id of the periodic report */
}
tdssrch_ops_code_struct_type;

/*--------------------------------------------------------------------------
                       SEARCHER LOST_SRCH STRUCT TYPE
--------------------------------------------------------------------------*/

typedef struct
{
  uint32  entity_type;
  uint16  freq;  
  uint8   cpid;
  boolean status;
} tdssrch_wakeup_set_struct_type;
/*--------------------------------------------------------------------------
                         SEARCH STEP ENUMERATION

--------------------------------------------------------------------------*/
typedef enum
{
  TDSSRCH_STEP_NONE,     /* 0 means no search is active */
  TDSSRCH_STEP1,              /* Step_1 search */
  TDSSRCH_STEP_NASTT,    /* NASTT search */
  TDSSRCH_STEP_PN           /* PN search */
} tdssrch_step_enum_type;

typedef enum
{
  TDSSRCH_RSCP,     /* RSCP */
  TDSSRCH_ISCP,      /* ISCP */
  TDSSRCH_TxPower,      /* TxPower */
  TDSSRCH_MEAS_NONE
}tdssrch_meas_quan_enum_type;


/* type of search - intra OR inter */
typedef enum
{
  TDS_INTRA_F,
  TDS_INTER_F,
} tdssrch_cell_type_enum_type;

/*Below Changed Xiaoning*/

/*------------------------------------------------------------------------------
   structure for dpch ul to update measurement module the new configuration
-------------------------------------------------------------------------------*/

typedef struct
{
 /*frame based, if new config starts at subframe N, 
    needs to pass to measurement before the boundary of subframe N-1, 
    convert cfn to sfn before pass to measruement module*/
  uint16 idleslot_config_time; 
  
 /*Timeslot bitmap.  From LSB, Bit 15-14 for TS#0, Bit 13-12 for TS#1, 
    бн, Bit 3-2 for TS#6, and 00->idle, 01->UL, 10->DL */
  uint16 ts_bitmap;
    
}tdssrch_meas_dpchul_config_struct_type;

/*------------------------------------------------------------------------------
   structure for dpch dl to update measurement module the new configuration
-------------------------------------------------------------------------------*/

typedef struct
{
  /*frame based, if new config starts at subframe N, 
    needs to pass to measurement before the boundary of subframe N-1, 
    convert cfn to sfn before pass to measruement module*/
  uint16 idleslot_config_time;
  
  /*Timeslot bitmap.  From LSB, Bit 15-14 for TS#0, Bit 13-12 for TS#1, 
    бн, Bit 3-2 for TS#6, and 00->idle, 01->UL, 10->DL */
  uint16 ts_bitmap;
   
  /*Integer(1, 2,4,8,16,32,64). Default is continuous allocation. 
    Value 1 indicate continuous. (10.3.6.10), frame based*/
  uint8 repetition_period;
  
  /*Bit[0..repetition_period-1] with each bit indicates if Physical channel is present in
  the corresponding frame. 1: phy channel is present. 0: not present. Bit[chanOnRepPeriod..63] 
  shall be set to 0 when chanOnRepPeriod is less than 64*/
  uint64  chanPresentMask;

}tdssrch_meas_dpchdl_config_struct_type;

/*------------------------------------------------------------------------------
   structure for sccpch to update measurement module the new configuration
-------------------------------------------------------------------------------*/
typedef struct
{
  /*frame based, if new config starts at subframe N, 
    needs to pass to measurement before the boundary of subframe N-1, 
    convert cfn to sfn before pass to measruement module*/
  uint16 idleslot_config_time;
 
  /*Timeslot bitmap.  From LSB, Bit 15-14 for TS#0, Bit 13-12 for TS#1, 
    бн, Bit 3-2 for TS#6, and 00->idle, 01->UL, 10->DL */ 
  uint16 ts_bitmap;
  
 /*Integer(1, 2,4,8,16,32,64). Default is continuous allocation. 
    Value 1 indicate continuous. (10.3.6.10), frame based*/
  uint8 repetition_period;
  
  /*Bit[0..repetition_period-1] with each bit indicates if Physical channel is present in
  the corresponding frame. 1: phy channel is present. 0: not present. Bit[chanOnRepPeriod..63] 
  shall be set to 0 when chanOnRepPeriod is less than 64*/
  uint64  chanPresentMask;
  
}tdssrch_meas_sccpch_config_struct_type;

/*------------------------------------------------------------------------------
   structure for PICH to update measurement module the new configuration
-------------------------------------------------------------------------------*/
typedef struct
{
  /*subframe based, if new config starts at subframe N, 
    needs to pass to measurement before the boundary of subframe N-1, 
    convert cfn to sfn before pass to measruement module*/
  uint16 idleslot_config_time;
 
  /*Timeslot bitmap.  From LSB, Bit 15-14 for TS#0, Bit 13-12 for TS#1, 
    бн, Bit 3-2 for TS#6, and 00->idle, 01->UL, 10->DL */ 
  uint16 ts_bitmap;  
}tdssrch_meas_pich_config_struct_type;

/*------------------------------------------------------------------------------
   structure for hsscch to update measurement module the new configuration
-------------------------------------------------------------------------------*/

typedef struct
{
  /*frame based, if new config starts at subframe N, 
    needs to pass to measurement before the boundary of subframe N-1, 
    convert cfn to sfn before pass to measruement module*/
  uint16 idleslot_config_time;
  
  /*Timeslot bitmap.  From LSB, Bit 15-14 for TS#0, Bit 13-12 for TS#1, 
    бн, Bit 3-2 for TS#6, and 00->idle, 01->UL, 10->DL */ 
  uint16 ts_bitmap;
  
}tdssrch_meas_hsscch_config_struct_type;

/*------------------------------------------------------------------------------
   structure for eagch to update measurement module the new configuration
-------------------------------------------------------------------------------*/
typedef struct
{
  /*subframe based, if new config starts at subframe N, 
    needs to pass to measurement before the boundary of subframe N-1, 
    convert cfn to sfn before pass to measruement module*/
  uint16 idleslot_config_time;

  /*Timeslot bitmap.  From LSB, Bit 15-14 for TS#0, Bit 13-12 for TS#1, 
    бн, Bit 3-2 for TS#6, and 00->idle, 01->UL, 10->DL */ 
  uint16 ts_bitmap;
  
}tdssrch_meas_eagch_config_struct_type;

/*------------------------------------------------------------------------------
   structure for uppch&fpach to update measurement module the new configuration
-------------------------------------------------------------------------------*/
typedef struct
{
  /*subframe based, if new config starts at subframe N, 
    needs to pass to measurement before the boundary of subframe N-1, 
    convert cfn to sfn before pass to measruement module*/
  uint16 upsync_start_time;

  /*True = uppch transmitting, false = uppch not transmitting*/
  boolean upsync_on_uppch;

  /*Timeslot bitmap.  From LSB, Bit 15-14 for TS#0, Bit 13-12 for TS#1, 
   бн, Bit 3-2 for TS#6, and 00->idle, 01->UL, 10->DL
   this is for UpPCH slot usage, 0 = uppts without shift, ~0 = with shift */
  uint16 ts_bitmap_uppch;

  /*Timeslot bitmap.  From LSB, Bit 15-14 for TS#0, Bit 13-12 for TS#1, 
    бн, Bit 3-2 for TS#6, and 00->idle, 01->UL, 10->DL*/
  uint16 ts_bitmap_fpach; 
  
  /*waiting time for FPACH in subframe */
  uint16 fpach_wt;
  
  /*0,1,2,3,бнup to 255*/
  uint8 seq_no;
  
}tdssrch_meas_uppch_fpach_config_struct_type;

/*------------------------------------------------------------------------------
   structure for prach to update measurement module the new configuration
-------------------------------------------------------------------------------*/
typedef struct
{
  /*frame based, if new config starts at subframe N, 
    needs to pass to measurement before the boundary of subframe N-1, 
    convert cfn to sfn before pass to measruement module*/
  uint16 idleslot_config_time;
  
  /*Timeslot bitmap.  From LSB, Bit 15-14 for TS#0, Bit 13-12 for TS#1, 
    бн, Bit 3-2 for TS#6, and 00->idle, 01->UL, 10->DL*/
  uint16 ts_bitmap;
  
}tdssrch_meas_prach_config_struct_type;

/*------------------------------------------------------------------------------
   structure for pdsch to update measurement module the new configuration
-------------------------------------------------------------------------------*/
typedef struct
{
  /*subframe based, arrive asap*/
  uint16 idleslot_config_time;
  
  uint16 ts_bitmap;
  /*Timeslot bitmap.  From LSB, Bit 15-14 for TS#0, Bit 13-12 for TS#1, 
    бн, Bit 3-2 for TS#6, and 00->idle, 01->UL, 10->DL*/
}tdssrch_meas_pdsch_config_struct_type;

/*------------------------------------------------------------------------------
   structure for epuch to update measurement module the new configuration
-------------------------------------------------------------------------------*/
typedef struct
{
  /*subframe based, arrive asap*/
  uint16 idleslot_config_time;

  /*Timeslot bitmap.  From LSB, Bit 15-14 for TS#0, Bit 13-12 for TS#1, 
    бн, Bit 3-2 for TS#6, and 00->idle, 01->UL, 10->DL*/
  uint16 ts_bitmap;
  
}tdssrch_meas_epuch_config_struct_type;

/*------------------------------------------------------------------------------
   structure for ehich to update measurement module the new configuration
-------------------------------------------------------------------------------*/
typedef struct
{
  /*subframe based, arrive asap*/
  uint16 idleslot_config_time;

  /*Timeslot bitmap.  From LSB, Bit 15-14 for TS#0, Bit 13-12 for TS#1, 
    бн, Bit 3-2 for TS#6, and 00->idle, 01->UL, 10->DL*/
  uint16 ts_bitmap;
  
}tdssrch_meas_ehich_config_struct_type;

/*------------------------------------------------------------------------------
   structure for pccpch to update measurement module the new configuration
-------------------------------------------------------------------------------*/
typedef struct
{
  /*frame based, if new config starts at subframe N, 
    needs to pass to measurement before the boundary of subframe N-1, 
    convert cfn to sfn before pass to measruement module*/
  uint16 idleslot_config_time;
 
  /*Timeslot bitmap.  From LSB, Bit 15-14 for TS#0, Bit 13-12 for TS#1, 
    бн, Bit 3-2 for TS#6, and 00->idle, 01->UL, 10->DL */ 
  uint16 ts_bitmap;
  
 /*Integer(1, 2,4,8,16,32,64). */
  uint8 repetition_period;
  
  /*Bit[0..repetition_period-1] with each bit indicates if Physical channel is present in
  the corresponding frame. 1: phy channel is present. 0: not present. Bit[chanOnRepPeriod..63] 
  shall be set to 0 when chanOnRepPeriod is less than 64*/
  uint64  chanPresentMask;
  
}tdssrch_meas_pccpch_config_struct_type;

/*------------------------------------------------------------------------------
   structure for fmo to update measurement module the new configuration
-------------------------------------------------------------------------------*/
typedef struct
{
  /*frame based, if new config starts at subframe N, 
    needs to pass to measurement before the boundary of subframe N-1, 
    convert cfn to sfn before pass to measruement module*/
  uint16 idleslot_config_time;
  
  /*n_tti*/
  uint8 n_tti;
  
  /*t_meas_rep*/
  uint32 t_meas_rep;
  
}tdssrch_meas_fmo_config_struct_type;

/*Above changed by Xiaoning*/

typedef struct
{
  uint8 cpid;
  int16 rscp;
}tdssrch_cell_rscp_type;

#ifdef FEATURE_TDSCDMA_DSDA
typedef struct {
  /*Timeslot bitmap indicate DL channels in current subframe.  
    From LSB, Bit 15-14 for TS#0, Bit 13-12 for TS#1, 
    ..., Bit 3-2 for TS#6, and 00->idle, 01->UL, 10->DL */
  uint16 channel_bitmap;  

  /*TS-RSP on TS0 from each TS0-rsp*/
  int16 ts0_min_tsrsp; 
  
  /*minimum TS-RSP on all non-TS0 from each NonTS0-rsp*/
  int16 non_ts0_min_tsrsp; 
} tdssrch_rx_power_report_t;

#endif

typedef struct {
    boolean resume_state_cfg;
    boolean resume_rx_cfg_primary_freq;
    boolean resume_rx_cfg_working_freq;
    boolean resume_reacq_cfg;
    boolean suspend_state_cfg;
} lna_lta_ind_type;
extern lna_lta_ind_type LNA_LTA_ind;

/*===========================================================================
                          FUNCTION PROTOTYPES
                          
The followings are prototypes of functions exported to external modules.
===========================================================================*/

/*===========================================================================

FUNCTION tdssrchacq_rpt

DESCRIPTION
  Reports Acquisition status to CPHY setup procedure

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void tdssrchacq_rpt (boolean acq_status);

/*===========================================================================

FUNCTION SRCH_GET_FILTERED_RSCP

DESCRIPTION    
  This function returns an unsigned value representing the current,
  filtered RSCP value in P(AGC) + Ec/Io form for all ASET cells. 
  
  The returned can then be arbitrarily mapped to UI display space 
  using an equation, table, or case statement.  
  
  The RSCP is filtered using an (N-1)/N IIR filter for a smoother signal
  strength display. 

DEPENDENCIES
  
    
RETURN VALUE
  An unsigned filtered RSCP value, in 0.5 dBm steps. The actual RSCP is
  -(Returned-unsigned-RSCP-value/2)dBm.

SIDE EFFECTS
  Filtered RSCP is updated.

===========================================================================*/
extern uint16 tdssrch_get_filtered_rscp(void);

/*===========================================================================

FUNCTION SRCH_COMPUTE_RSCP

DESCRIPTION
  This function calculates the RSCP given the ecIo and the RxAGC..
  The calculated RSCP value will be returned in uint16 units. The actual RSCP
  is -ve of the returned value.

DEPENDENCIES
  The cell's ecio have been computed.

RETURN VALUE
  uint16.

SIDE EFFECTS
  None.

===========================================================================*/
extern uint16 tdssrch_compute_rscp( uint16 ecio, int16 rx_agc);


/*===========================================================================

FUNCTION SRCH_EVENT_TIMER_ELAPSED

DESCRIPTION
  This function returns the time elapsed from start_ts passed to it.

DEPENDENCIES
  start_ts should be initialized and updated correctly.

RETURN VALUE
  Time elapsed from start_ts.

SIDE EFFECTS
  None.

===========================================================================*/
extern uint16 tdssrch_event_timer_elapsed( uint16 start_ts );

#ifdef FEATURE_L1_LOG_ON_DEMAND
/*===========================================================================
FUNCTION        NSET_PROCESS_LOG_ON_DEMAND

FILE NAME       srch.c

DESCRIPTION     This funtion creates a NSET log packet and submits it
                to QXDM

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
void tds_nset_process_log_on_demand(void);
#endif

#ifdef FEATURE_L1_LOG_ON_DEMAND
/*===========================================================================
FUNCTION        ASET_PROCESS_LOG_ON_DEMAND

FILE NAME       srch.c

DESCRIPTION     This funtion creates a ASET log packet and submits it
                to QXDM

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
void tds_aset_process_log_on_demand(void);
#endif

/*===========================================================================

FUNCTION SRCH_WAKEUP_CELL

DESCRIPTION:
  API called by DRX manager to wakeup cells. This API can be called in any
  L1 state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Wakeup list search may get triggered in certain states
===========================================================================*/
extern void tdssrch_wakeup_cell(tdssrch_wakeup_set_struct_type *wakeup_set_ptr,
                       boolean ue_wakeup);


/*===========================================================================

FUNCTION TDSSRCH_WAKEUP_CELL_DONE

DESCRIPTION:
  API called by SRCH to respond to wakeup request from DRX
  L1 state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

extern void tdssrch_wakeup_cell_done(tdssrch_wakeup_set_struct_type *wakeup_set_ptr);
extern boolean tdssrch_send_meas_cmd(uint16 cell_mask);
extern uint8  tdssrch_rscp_meas_done(uint8 message_id);

/*===========================================================================

FUNCTION SRCH_NOTIFY_ASET_UPD_STATUS_TO_L1M

DESCRIPTION:
  send a TDS_PHYCHAN_ASET_DONE_CMD local cmd to L1M 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
//extern void tdssrch_notify_aset_upd_status_to_l1m(boolean status);

//extern int16 tdssrch_get_freq_offset(void);

/*===========================================================================
FUNCTION        tdsl1_get_rssi

FILE NAME       wsrch.c

DESCRIPTION     This function returns the Received Signal Strength Indication
                for use by the UI and Call Manager subsystems

DEPENDENCIES    The SRCH module of Layer 1

RETURN VALUE    The measured signal level shall be sent with no sign

SIDE EFFECTS    None
===========================================================================*/
extern void tdsl1_get_rssi (tdsl1_rssi_info_struct_type *rssi_data);

/*=========================================================================

 FUNCTION     :TDSSRCH_GET_PRIMARY_FREQ
 
 DESCRIPTION  : get the current serving cell's primary frequency.
              
 DEPENDENCIES : 
 
 RETURN VALUE : primary frequency information. 
       
 SIDE EFFECTS : 
==========================================================================*/
extern uint16 tdssrch_get_primary_freq(void);

/*=========================================================================

 FUNCTION     :TDSSRCH_GET_WORKING_FREQ
 
 DESCRIPTION  : get the current serving cell's working/intra frequency.
              
 DEPENDENCIES : 
 
 RETURN VALUE : working frequency information. 
       
 SIDE EFFECTS : 
==========================================================================*/
extern uint16 tdssrch_get_working_freq(void);

/*=========================================================================

FUNCTION     :TDSSRCH_GET_HO_CELL_CPI

 DESCRIPTION  : The function is used to get handover cell's cpi. 
              
 DEPENDENCIES : 
 
 RETURN VALUE : 
  uint8: cell parameter id information
   
 SIDE EFFECTS : None
==========================================================================*/

extern uint8 tdssrch_get_ho_cell_cpi(void);

/*=========================================================================

 FUNCTION     :TDSSRCH_GET_SERVING_CELL_CPI
 
 DESCRIPTION  : The function is used to get serving cell's cpi. 
              
 DEPENDENCIES : 
 
 RETURN VALUE : 
  uint8: cell parameter id information
   
 SIDE EFFECTS : None
==========================================================================*/
extern uint8 tdssrch_get_serving_cell_cpi(void);

/*===========================================================================

FUNCTION TDSSRCH_GET_SERVING_CELL_PATHLOSS

DESCRIPTION
  This function provides current serving cell's pathloss value for another modules.

DEPENDENCIES
  None.

RETURN VALUE
  uint16: pathloss value.

SIDE EFFECTS

===========================================================================*/
extern uint16 tdssrch_get_serving_cell_pathloss(void );

/*===========================================================================

FUNCTION TDSSRCH_ASET_UPDATE

DESCRIPTION
  This function processes serving cell update command from L1M. 
  
 INPUT PARAMETERS: 
  tdssrchcmd_aset_update_cmd_type *cmd_ptr: the pointer points to an aset_update_cmd, which including
  primary frequency, working frequency, cpi and updating type.
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
//extern void tdssrch_aset_update( tdssrchcmd_aset_update_cmd_type *cmd_ptr );

/*=========================================================================

 FUNCTION     :TDSSRCH_GET_SERVING_CELL_REF_POS
 
 DESCRIPTION  : The function is used to get serving cell's reference position(Timeslot 0 Start Position). 
              
 DEPENDENCIES : 
 
 RETURN VALUE : 
  uint16: Timeslot 0 Start Position
   
 SIDE EFFECTS : None
==========================================================================*/
extern uint16 tdssrch_get_serving_cell_ref_pos(void);

/*=========================================================================

 FUNCTION     :TDSSRCH_GET_TARGET_CELL_OTDTYPE2
 
 DESCRIPTION  : The function is used to get appointed cell's otd type2 value. 
              
 DEPENDENCIES : 

 INPUT VALUE:
  uint16 primary_freq, 
  uint8 cpid   
  
 RETURN VALUE : 
   int32  
   
 SIDE EFFECTS : None
==========================================================================*/
extern int32 tdssrch_get_target_cell_otdtype2(uint16 primary_freq, uint8 cpid);

/*=========================================================================

 FUNCTION     :tdssrch_check_if_cell_measured_before
 
 DESCRIPTION  : The function is used to check if HO target cell is in SRCH measurement cell_list. 
              
 DEPENDENCIES : 

 INPUT VALUE:
  uint16 primary_freq, 
  uint8 cpid   
  
 RETURN VALUE : 
   boolean  
   
 SIDE EFFECTS : None
==========================================================================*/
extern boolean tdssrch_check_if_cell_measured_before(uint16 primary_freq, uint8 cpid);

/*=========================================================================

 FUNCTION     :TDSSRCH_GET_TARGET_CELL_OTDTYPE2
 
 DESCRIPTION  : The function is used to get appointed cell's otd type2 value. 
              
 DEPENDENCIES : 

 INPUT VALUE:
  
 RETURN VALUE : 
   int32  
   
 SIDE EFFECTS : None
==========================================================================*/
extern int16 tdssrch_get_scell_tadv(void);


/*=========================================================================

 FUNCTION     :TDSSRCH_GET_PRI_FREQ_RSSI
 
 DESCRIPTION  : The function is used to get primary frequency's rssi 
              
 DEPENDENCIES : 
 
 RETURN VALUE : 
  uint8: (0,76)
   
 SIDE EFFECTS : None
==========================================================================*/
extern uint8 tdssrch_get_pri_freq_rssi(void);

/*=========================================================================

 FUNCTION     :TDSSRCHSET_UPDATE_SERVING_CELL_FREQ_OFFSET
 
 DESCRIPTION  : During blind P-CCPCH decoding, DL module calls the function to update and get final 
                         frequency-offset value.
              
 DEPENDENCIES : 

 INPUT PARAMETERS :
   int16 delta_freqoffset : a delta frequency-offset value from DL module.

 RETURN VALUE : 
   int16: the current cell's final frequency-offset value.
       
 SIDE EFFECTS : 
==========================================================================*/
extern int16 tdssrch_update_serving_cell_freq_offset(int16 delta_freqoffset);


/*=========================================================================

 FUNCTION     :TDSSRCH_CHECK_WALLTIMEOFS
 
 DESCRIPTION  : This function is used to check whether walltime offset is valid.
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 
   uint32 walltimeofs
 
 RETURN VALUE : None. 
       
 SIDE EFFECTS : 
==========================================================================*/
extern boolean tdssrch_check_walltimeofs(uint32 walltimeofs);


/*=========================================================================

 FUNCTION     :TDSSRCH_PROC_CELL_INFO_IND
 
 DESCRIPTION  : This function is used to handle TDSCDMA_FW_CELL_INFO_IND from FW.
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 
   tfw_cell_info_t *cell_info
 
 RETURN VALUE : None. 
       
 SIDE EFFECTS : 
==========================================================================*/
extern void tdssrch_proc_cell_info_ind(tfw_cell_info_t *cell_info);

/*Xiaoning Changed*/
/*===========================================================================
FUNCTION        TDSSRCHMEAS_UPDATE_DPCHUL_SLOTUSAGE

DESCRIPTION     This function updates the slotusage for upch ul.


DEPENDENCIES    

RETURN VALUE    None

SIDE EFFECTS    
===========================================================================*/
extern void tdssrchmeas_update_dpchul_slotusage(tdssrch_meas_dpchul_config_struct_type
                                             *slot_usage_ptr);

/*===========================================================================
FUNCTION        TDSSRCHMEAS_UPDATE_DPCHDL_SLOTUSAGE

DESCRIPTION     This function updates the slotusage for upch dl.


DEPENDENCIES    

RETURN VALUE    None

SIDE EFFECTS    
===========================================================================*/
extern void tdssrchmeas_update_dpchdl_slotusage(tdssrch_meas_dpchdl_config_struct_type 
                                             *slot_usage_ptr);

/*===========================================================================
FUNCTION        TDSSRCHMEAS_UPDATE_SCCPCH_SLOTUSAGE

DESCRIPTION     This function updates the slotusage for SCCPCH.


DEPENDENCIES    

RETURN VALUE    None

SIDE EFFECTS    
===========================================================================*/
extern void tdssrchmeas_update_sccpch_slotusage(tdssrch_meas_sccpch_config_struct_type 
                                             *slot_usage_ptr);
/*===========================================================================
FUNCTION        TDSSRCHMEAS_UPDATE_PICH_SLOTUSAGE

DESCRIPTION     This function updates the slotusage for PICH.


DEPENDENCIES    

RETURN VALUE    None

SIDE EFFECTS    
===========================================================================*/
extern void tdssrchmeas_update_pich_slotusage(tdssrch_meas_pich_config_struct_type 
                                             *slot_usage_ptr);
/*===========================================================================
FUNCTION        TDSSRCHMEAS_UPDATE_HSSCCH_SLOTUSAGE

DESCRIPTION     This function updates the slotusage for hs-scch.


DEPENDENCIES    

RETURN VALUE    None

SIDE EFFECTS    
===========================================================================*/
extern void tdssrchmeas_update_hsscch_slotusage(tdssrch_meas_hsscch_config_struct_type 
                                             *slot_usage_ptr);

/*===========================================================================
FUNCTION        TDSSRCHMEAS_UPDATE_EAGCH_SLOTUSAGE

DESCRIPTION     This function updates the slotusage for eagch.


DEPENDENCIES    

RETURN VALUE    None

SIDE EFFECTS    
===========================================================================*/
extern void tdssrchmeas_update_eagch_slotusage(tdssrch_meas_eagch_config_struct_type 
                                            *slot_usage_ptr);

/*===========================================================================
FUNCTION        TDSSRCHMEAS_UPDATE_UPPCH_FPACH_SLOTUSAGE

DESCRIPTION     This function updates the slotusage for uppch&fpach.


DEPENDENCIES    

RETURN VALUE    None

SIDE EFFECTS    
===========================================================================*/
extern void tdssrchmeas_update_uppch_fpach_slotusage(
  tdssrch_meas_uppch_fpach_config_struct_type *slot_usage_ptr);

/*===========================================================================
FUNCTION        TDSSRCHMEAS_STOP_UPPCH_FPACH

DESCRIPTION     This function clean-up the slotusage for uppch&fpach.


DEPENDENCIES    

RETURN VALUE    None

SIDE EFFECTS    
===========================================================================*/
extern void tdssrchmeas_stop_uppch_fpach(void);

/*===========================================================================
FUNCTION        TDSSRCHMEAS_UPDATE_PRACH_SLOTUSAGE

DESCRIPTION     This function updates the slotusage for prach.


DEPENDENCIES    

RETURN VALUE    None

SIDE EFFECTS    
===========================================================================*/
extern void tdssrchmeas_update_prach_slotusage(tdssrch_meas_prach_config_struct_type 
                                             *slot_usage_ptr);

/*===========================================================================
FUNCTION        TDSSRCHMEAS_UPDATE_PDSCH_SLOTUSAGE

DESCRIPTION     This function updates the slotusage for pdsch.


DEPENDENCIES    

RETURN VALUE    None

SIDE EFFECTS    
===========================================================================*/
extern void tdssrchmeas_update_pdsch_slotusage(tdssrch_meas_pdsch_config_struct_type 
                                             *slot_usage_ptr);

/*===========================================================================
FUNCTION        TDSSRCHMEAS_UPDATE_EPUCH_SLOTUSAGE

DESCRIPTION     This function updates the slotusage for epuch.


DEPENDENCIES    

RETURN VALUE    None

SIDE EFFECTS    
===========================================================================*/
extern void tdssrchmeas_update_epuch_slotusage(tdssrch_meas_epuch_config_struct_type 
                                             *slot_usage_ptr);

/*===========================================================================
FUNCTION        TDSSRCHMEAS_UPDATE_NONSCHE_EPUCH_SLOTUSAGE

DESCRIPTION     This function updates the slotusage for non scheduled epuch.


DEPENDENCIES    

RETURN VALUE    None

SIDE EFFECTS    
===========================================================================*/
extern void tdssrchmeas_update_nonsche_epuch_slotusage(tdssrch_meas_epuch_config_struct_type 
                                             *slot_usage_ptr);

/*===========================================================================
FUNCTION        TDSSRCHMEAS_UPDATE_EHICH_SLOTUSAGE

DESCRIPTION     This function updates the slotusage for scheduled ehich.


DEPENDENCIES    

RETURN VALUE    None

SIDE EFFECTS    
===========================================================================*/
extern void tdssrchmeas_update_ehich_slotusage(tdssrch_meas_ehich_config_struct_type 
                                             *slot_usage_ptr);

/*===========================================================================
FUNCTION        TDSSRCHMEAS_UPDATE_NONSCHE_EHICH_SLOTUSAGE

DESCRIPTION     This function updates the slotusage for non scheduled ehich.


DEPENDENCIES    

RETURN VALUE    None

SIDE EFFECTS    
===========================================================================*/
extern void tdssrchmeas_update_nonsche_ehich_slotusage(tdssrch_meas_ehich_config_struct_type 
                                             *slot_usage_ptr);


/*===========================================================================
FUNCTION        TDSSRCHMEAS_UPDATE_PCCPCH_SLOTUSAGE

DESCRIPTION     This function updates the slotusage for pccpch.


DEPENDENCIES    

RETURN VALUE    None

SIDE EFFECTS    
===========================================================================*/
extern void tdssrchmeas_update_pccpch_slotusage(tdssrch_meas_pccpch_config_struct_type 
                                             *slot_usage_ptr);

/*===========================================================================
FUNCTION        TDSSRCHMEAS_UPDATE_FMO_INFO

DESCRIPTION     This function updates the fmo info.


DEPENDENCIES    

RETURN VALUE    None

SIDE EFFECTS    
===========================================================================*/
extern void tdssrchmeas_update_fmo_info(tdssrch_meas_fmo_config_struct_type 
                                             *slot_usage_ptr);

/*======================================================================
FUNCTION TDSSRCHMEAS_RESET_IDLESLOT_DBASE

DESCRIPTION
    Resets the flags for the idle slot configuration
     
RETURN VALUE
    void

=======================================================================*/
extern void tdssrchmeas_reset_idleslot_dbase(void);

extern void tdssrchdch_cell_eval_1g( tdssrchset_cell_struct_type_tmp *cell_ptr );
extern void tdssrchdch_cell_eval_1h1i( tdssrchset_cell_struct_type_tmp *cell_ptr );
extern void tdssrchdch_send_meas_report ( void );
extern void tdssrchinterf_evaluate_interfreq_events(void);
extern void tdssrchinterf_evaluate_events_2d_2f(void);

extern void tdssrchacq_tune_to_chan(uint16 chan_num);
extern void tdssrchacq_proc_step0_rsp(tfw_srch_gap_detect_rsp_t *gap_detect_rsp);
extern void tdssrchacq_proc_step1_rsp(tfw_srch_syncdl_midamble_detect_rsp_t *syncdl_midamble_detect_rsp);
extern void tdssrchacq_proc_state_cfg_rsp(tfw_state_t state);

extern void tdssrchsync_cell_update(tdsl1_acq_cmd_type *cmd);
extern void tdssrch_proc_rscp_rsp(tfw_measure_rscp_rsp_t *msg_rscp_rsp);
extern void tdssrch_proc_iscp_rsp(tfw_measure_iscp_rsp_t *msg_iscp_rsp);

#ifdef FEATURE_JDS_OPT
extern void tdssrch_proc_ts0_cell_rsp(tfw_ts0_cell_report_t *ts0_cell_rsp);
extern void tdssrch_proc_non_ts0_cell_rsp(tfw_nonts0_cell_report_t *non_ts0_cell_rsp);
#endif

/*=========================================================================
 
 FUNCTION     : tdssrch_stopped_init
 
 DESCRIPTION  : This function initialize some variables in stop state

              
 DEPENDENCIES : 
 
 RETURN VALUE : 
                
 SIDE EFFECTS : 
 
 =========================================================================*/

extern void tdssrch_stopped_init(void);


/*=========================================================================

FUNCTION     : tdssrch_meas_appointed_cell

DESCRIPTION  : 
This function will put appointed cell and its TDCS to tdssrch_send_rscp_cmd 

DEPENDENCIES : None

RETURN VALUE : None.

SIDE EFFECTS :

=========================================================================*/
extern void tdssrch_meas_appointed_cell( tdssrchset_cell_struct_type_tmp *cell_ptr);

/*===========================================================================

FUNCTION TDSSRCH_RSCP_MEAS_CMD

DESCRIPTION:
  - fill all the parameters to tdssrch_rscp_sync_parms
  - call the MCAL api to issue rscp srch
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean tdssrch_rscp_meas_cmd(uint8 message_id);


extern void tdssrch_filter_meas_results
	 (int16 *filt_value, int16 last_meas_value, uint16 last_meas_gap, uint16 value_k,
	  tdssrch_meas_quan_enum_type meas_type,boolean first_meas);


/*===========================================================================

FUNCTION TDSSRCH_SORT_ALL_RSCP

DESCRIPTION:
  sort the cells from high to low RSCP value
  - rscp: measure all returned rscp result array
  - cell: array provided by the caller to store the sorting result.
  - num_cells: total number of cells to sort, should not exceed the array size of rscp[] and cell[]
  

DEPENDENCIES
  None.

RETURN VALUE
  num: total number of cells in the sorted cell[] result.

SIDE EFFECTS
  None
===========================================================================*/
uint8 tdssrch_sort_all_rscp(const int16* rscp, tdssrch_cell_rscp_type* cell, uint8 num_cells);

int8 tdssrch_comp_cell_rscp_func(const void* parm1, const void* parm2);

/*===========================================================================

FUNCTION TDSSRCH_ASET_UPDATE

DESCRIPTION
  This function processes serving cell update command from L1M. 
  
 INPUT PARAMETERS: 
  tdssrchcmd_aset_update_cmd_type *cmd_ptr: the pointer points to an aset_update_cmd, which including
  primary frequency, working frequency, cpi and updating type.
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void tdssrch_aset_update( tdssrchcmd_aset_update_cmd_type *cmd_ptr);

/*=========================================================================

 FUNCTION     :TDSSRCH_CURRENT_FW_SERVING_CELL_CPID
 
 DESCRIPTION  : get the current serving cell's cpid in FW side.
              
 DEPENDENCIES : 
 
 RETURN VALUE : Cell parameter id. 
       
 SIDE EFFECTS : 
==========================================================================*/
extern uint16 tdssrch_current_fw_serving_cell_cpid(void);

extern uint64 tdssrch_exp_2(int32 x_q24x8);
extern uint64 tdssrch_db_2_lin( int16 dB_q8x8 );
extern int16 tdssrch_lin_2_db( uint16 linear_q16x8 );

extern void tdssrchset_update_rf_script_after_ho(void);

#ifdef FEATURE_TDSCDMA_L1_LOGGING_ENABLED
extern void tdssrch_log_packet_scell_tracking_status(int32 freq_err, uint16 ts0startPosition );
#endif

/*=========================================================================
 
 FUNCTION     : mcaltds_srch_current_fw_serving_cell_freq
 
 DESCRIPTION  : 

              
 DEPENDENCIES : 
 
 RETURN VALUE : 
                
 SIDE EFFECTS : 
 
 =========================================================================*/
 extern uint16 mcaltds_srch_current_fw_serving_cell_freq(void);

#ifdef FEATURE_TDSCDMA_ANTENNA_SWITCH_DIVERSITY
/*=========================================================================

 FUNCTION     :TDSSRCH_ASDIV_CHECK_STATE_CHANGE
 
 DESCRIPTION  : This function checks if there is a ASDiv state change (SLEEP <-> RX <-> TX <-> SLEEP). If true, 
 inform the config manager and set up the callback functions.
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 

 RETURN VALUE : 
       
 SIDE EFFECTS : 
==========================================================================*/
extern void tdssrch_asdiv_check_state_change(void);

/*=========================================================================

 FUNCTION     :TDSSRCH_ASDIV_SET_MODE_CB
 
 DESCRIPTION  : This is the callback function for TRM to confirm TDS has set ASDiv mode
              
 DEPENDENCIES : 

 INPUT PARAMETERS: trm_ant_switch_set_mode_return_type set_mode_status

 RETURN VALUE : 
       
 SIDE EFFECTS : 
==========================================================================*/
extern void tdssrch_asdiv_set_mode_cb(trm_ant_switch_set_mode_type set_mode_status);

/*=========================================================================

 FUNCTION     :TDSSRCH_ASDIV_ANT_SWITCH_CB
 
 DESCRIPTION  : This is the callback function for TRM to inform TDS to switch antenna
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 
 	trm_ant_switch_cb_info_type switch_action

 RETURN VALUE : 
       
 SIDE EFFECTS : 
==========================================================================*/
extern void tdssrch_asdiv_ant_switch_cb(trm_ant_switch_cb_info_type switch_action);

/*=========================================================================

 FUNCTION     :TDSSRCH_ASDIV_SET_MODE_AND_CONFIG_CB
 
 DESCRIPTION  : This is the callback function used by TRM to inform TDS the 
 				the grant result of the trm_asd_set_mode_and_config()
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 
 	trm_asd_set_mode_and_config_type set_mode_status

 RETURN VALUE : 
       
 SIDE EFFECTS : 
==========================================================================*/
extern void tdssrch_asdiv_set_mode_and_config_cb(trm_asd_set_mode_and_config_type set_mode_status);

/*=========================================================================

 FUNCTION     :TDSSRCH_ASDIV_CHECK_SAFE_CONDITION
 
 DESCRIPTION  : This function checks the safe condition to allow other tech to switch in DSDA idle/traffic scenario.
 Then send the result to TRM to update status.
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 

 RETURN VALUE : 
       
 SIDE EFFECTS : 
==========================================================================*/
void tdssrch_asdiv_check_safe_condition(void);

/*=========================================================================

 FUNCTION     :TDSSRCH_ASDIV_BLOCK_OTHER_TECH_SWITCH
 
 DESCRIPTION  : This function informs TRM to stop/resume other tech from switching due to certain reasons 
 (TDS is doing QTA, IRAT, etc.)
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 
	boolean								lock
	trm_asdiv_switch_state_update_reason_t	reason
 RETURN VALUE : 
       
 SIDE EFFECTS : 
==========================================================================*/
void tdssrch_asdiv_block_other_tech_switch(boolean lock, trm_asdiv_switch_state_update_reason_t reason);

/*=========================================================================

 FUNCTION     :TDSSRCH_ASDIV_INIT_ANTENNA_POSITION
 
 DESCRIPTION  : This function initializes antenna position during idle wake up or enter mode
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 

 RETURN VALUE : 
       
 SIDE EFFECTS : 
==========================================================================*/
void tdssrch_asdiv_init_antenna_position(void);

/*=========================================================================

 FUNCTION     :TDSSRCH_ASDIV_SET_PREFERRED_ANTENNA_BEFORE_SLEEP
 
 DESCRIPTION  : This function sets the preferred antenna and inform TRM before going to sleep
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 

 RETURN VALUE : 
       
 SIDE EFFECTS : 
==========================================================================*/
void tdssrch_asdiv_set_preferred_antenna_before_sleep(void);

#endif

/*=========================================================================

 FUNCTION     :TDSSRCH_MUST_WAIT_DEMOD_rsp
 
 DESCRIPTION  : This is a function for DL to ask SRCH to wait all demod
                rsps before sending cleanup done to L1M.
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 

 RETURN VALUE : 
       
 SIDE EFFECTS : 
==========================================================================*/
extern void tdssrch_must_wait_demod_rsp(void);

/*=========================================================================

 FUNCTION     :TDSSRCH_RESET_WAIT_DEMOD_RSP_FLAG
 
 DESCRIPTION  : This is a function for L1M to reset 
                'tdssrch_need_wait_ts0_demod_rsp'.
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 

 RETURN VALUE : 
       
 SIDE EFFECTS : 
==========================================================================*/
extern void tdssrch_reset_wait_demod_rsp_flag(void);

/*=========================================================================

 FUNCTION     :TDSSRCH_UPDATE_INTERF_RF_SCRIPTS
 
 DESCRIPTION  : This is a function for rfsw to ask SRCH to update inter-freq 
 rf scripts.
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 

 RETURN VALUE : 
       
 SIDE EFFECTS : 
==========================================================================*/
extern void tdssrch_update_interf_rf_scripts(void);

/*=========================================================================
 
 FUNCTION     : 
 
 DESCRIPTION  : 

              
 DEPENDENCIES : 
 
 RETURN VALUE : 
                
 SIDE EFFECTS : 
 
 =========================================================================*/

extern __inline int16 TDSSRCHDCH_FHO_1G_REPEAT_NUMBER(void);

extern __inline int16 TDSSRCHDCH_FHO_2A_REPEAT_NUMBER(void);

extern __inline boolean TDSSRCHDCH_FHO_1G_RSCP_SWITCH(void);

extern __inline boolean TDSSRCHDCH_FHO_1G_TS_RSP_SWITCH(void);

extern __inline boolean TDSSRCHDCH_FHO_1G_SIR_SWITCH(void);

extern __inline boolean TDSSRCHDCH_FHO_1G_UL_POWER_SWITCH(void);

extern __inline boolean TDSSRCHDCH_FHO_2A_RSCP_SWITCH(void);

extern __inline boolean TDSSRCHDCH_FHO_2A_TS_RSP_SWITCH(void);

extern __inline boolean TDSSRCHDCH_FHO_2A_SIR_SWITCH(void);

extern __inline boolean TDSSRCHDCH_FHO_2A_UL_POWER_SWITCH(void);

extern __inline boolean TDSSRCHDCH_FHO_3A_RSCP_SWITCH(void);

extern __inline boolean TDSSRCHDCH_FHO_3A_TS_RSP_SWITCH(void);

extern __inline boolean TDSSRCHDCH_FHO_3A_SIR_SWITCH(void);

extern __inline boolean TDSSRCHDCH_FHO_3A_UL_POWER_SWITCH(void);

extern __inline boolean TDSSRCHDCH_FHO_1G2A_NEW_TIMER_SWITCH(void);

#ifdef FEATURE_TDSCDMA_REPEAT_MR
extern void tdssrchdch_tmrpt_init(void);
 
extern void tdssrchdch_check_reset_tmrpt_timer(tdssrchdch_mrpt_timer_t *tds_mrpt_timer, uint16 counter, uint8 max_repeat_times, boolean force_reset);

extern void tdssrchdch_check_start_1g2a_repeat_send_timer(tdssrchdch_mrpt_timer_t *tds_mrpt_timer, uint8 meas_index, uint16 ts_cnt);

extern boolean tdssrchdch_check_T_mrpt_expired(tdssrchdch_mrpt_timer_t *tds_mrpt_timer, uint8 meas_index, uint16 mrpt_timer_counter);
#endif

#ifdef FEATURE_TDSCDMA_REPEAT_TO_SECOND_BEST 
extern __inline uint8 TDSSRCHDCH_FHO_1G_REPEAT_TIMES(void);
extern __inline uint8 TDSSRCHDCH_FHO_2A_REPEAT_TIMES(void);
extern __inline uint8 TDSSRCHDCH_FHO_1G_REPEAT_CELL_NUM(void);
extern __inline uint8 TDSSRCHDCH_FHO_2A_REPEAT_CELL_NUM(void);
#endif
#if defined (FEATURE_TDSCDMA_E3A_THRESHOLD_OPT)
extern __inline int8 TDSSRCHDCH_E3A_THRESHOLD_OPT_TD(void);
extern __inline int8 TDSSRCHDCH_E3A_THRESHOLD_OPT_GSM(void);
extern __inline int8 TDSSRCHDCH_E3A_THRESHOLD_OPT_TD_FOR_CS_ONLY(void);
#endif

#ifdef FEATURE_TDSCDMA_ANTENNA_SWITCH_DIVERSITY
void tdssrch_asdiv_init(void);
void tdssrch_asdiv_dch_init(void);
void tdssrch_asdiv_rscp_filtering(void);
void tdssrch_asdiv_state_update(void);
void tdssrch_asdiv_complete_switch(void);
void tdssrch_asdiv_call_fw_switch(boolean is_rf_ready);
void tdssrch_asdiv_rf_callback(void * cb_data);
void tdssrch_asdiv_init_antenna_position_when_asdiv_is_disabled(void);
void tdssrch_asdiv_test_mode_update(void);
void tdssrch_asdiv_stay_on_current_antenna_before_sleep(void);


void tdssrch_asdiv_init_type1_algo(void);
void tdssrch_asdiv_rscp_filtering_type1_algo(void);
void tdssrch_asdiv_state_update_type1_algo(void);
void tdssrch_asdiv_complete_switch_type1_algo(void);
void tdssrch_asdiv_call_fw_switch_type1_algo(boolean is_rf_ready);
boolean tdssrch_asdiv_choose_type1_algorithm(void);
#endif

/*=========================================================================

 FUNCTION     :TDSSRCH_GET_SCELL_TS0_JDS_NUM
 
 DESCRIPTION  : The function will return TS0 iterative cell(JDS) number for 
 primary frequency.
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 

 RETURN VALUE : 
       
 SIDE EFFECTS : 
==========================================================================*/
extern uint8 tdssrch_get_scell_ts0_jds_num(void);

/*=========================================================================

 FUNCTION     :TDSSRCH_GET_SCELL_NONTS0_JDS_NUM
 
 DESCRIPTION  : The function will return NONTS0 iterative cell(JDS) number for 
 primary frequency.
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 

 RETURN VALUE : 
       
 SIDE EFFECTS : 
==========================================================================*/
extern uint8 tdssrch_get_scell_nonts0_jds_num(void);

/*=========================================================================

 FUNCTION     :TDSSRCH_GET_MEAS_CELLS_NUM
 
 DESCRIPTION  : The function will return total measurement cells num over 20ms.
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 

 RETURN VALUE : 
       
 SIDE EFFECTS : 
==========================================================================*/
extern uint8 tdssrch_get_meas_cells_num(void);

/*=========================================================================

 FUNCTION     :TDSSRCH_GET_ACCUMULATE_UL_TS_NUM
 
 DESCRIPTION  : The function will return total number(accumulated) of active
 UL ts over last 20ms.
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 

 RETURN VALUE : 
       
 SIDE EFFECTS : 
==========================================================================*/
extern uint8 tdssrch_get_accumulate_ul_ts_num(void);

/*=========================================================================

 FUNCTION     :TDSSRCH_GET_ACCUMULATE_DL_TS_NUM
 
 DESCRIPTION  : The function will return total number(accumulated) of active
 DL ts over last 20ms.
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 

 RETURN VALUE : 
       
 SIDE EFFECTS : 
==========================================================================*/
extern uint8 tdssrch_get_accumulate_dl_ts_num(void);


/*=========================================================================

 FUNCTION     : tdssrch_update_LNA_info()
 
 DESCRIPTION  : update LNA info into freq_set after receiving ts0 update response
                from FW. 
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 

 RETURN VALUE : 
       
 SIDE EFFECTS : 
==========================================================================*/
void tdssrch_update_LNA_info(uint8 freq_index, tfw_ts0_cell_report_t *msg_ts0_cell_rsp);


#ifdef FEATURE_QSH_EVENT_METRIC
/*=========================================================================

 FUNCTION     :TDSSRCH_GET_QSH_METRIC
 
 DESCRIPTION  : This is a function to share SRCH parameter for QSH
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 

 RETURN VALUE : 
       
 SIDE EFFECTS : 
==========================================================================*/
boolean tdssrch_get_qsh_rf_metirc(tdsl1_qsh_metric_serv_cell_rf_s *serv_cell_rf_ptr);

boolean tdssrch_get_qsh_cir_snr_metirc(tdsl1_qsh_metric_cir_snr_s *cir_snr_ptr);

boolean tdssrch_get_qsh_pwr_metirc(tdsl1_qsh_metric_serv_cell_pwr_s *serv_cell_pwr_ptr);

#endif

#endif /* TDSSRCH_H */



