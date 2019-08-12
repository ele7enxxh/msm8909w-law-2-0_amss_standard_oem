#ifndef TDSMAC_UL_HS_H
#define TDSMAC_UL_HS_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              U P L I N K   M A C   H S U P A   H E A D E R    F I L E

DESCRIPTION
   MAC HSUPA specific type declarations.

Copyright (c) 2011-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.


===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/mac/inc/tdsmaculhs.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when       who      what, where, why
--------   -------- --------------------------------------------------------
11/06/11   jz       Add time_slot_num as part of build pdu info type 
03/09/11   cyx      Add HUPA TVM func
02/02/11   ps       Initial release Creation of file for TDS MAC HSUPA
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#ifdef CUST_H
#include "customer.h"
#endif

#include "tdsmacrrcif.h"
#include "tdsmacinternal.h"
#include "tdsl1macdata.h"



/*===========================================================================

                     DEFINES AND MACROS FOR MODULE

===========================================================================*/
/*--------------------------------------------------------------------------
Number of MAC-d flows configured
--------------------------------------------------------------------------*/
#define TDSMAC_HS_UL_MIN_MAC_D_FLOWS                  1
#define TDSMAC_HS_UL_MAX_MAC_D_FLOWS                  TDSMAX_E_DCH_MAC_D_FLOW

/*--------------------------------------------------------------------------
Ranges for MAC-d flow Id
--------------------------------------------------------------------------*/
#define TDSMAC_HS_UL_MIN_MAC_D_FLOW_ID                0
#define TDSMAC_HS_UL_MAX_MAC_D_FLOW_ID                (TDSMAX_E_DCH_MAC_D_FLOW - 1)

/*--------------------------------------------------------------------------
Max TSN  value
--------------------------------------------------------------------------*/
#define TDSMAC_HS_UL_MAX_TSN                          63

/*--------------------------------------------------------------------------
E-DCH MAC-d flow mpower offset range
--------------------------------------------------------------------------*/
#define TDSMAC_HS_UL_MIN_POWER_OFFSET      0
#define TDSMAC_HS_UL_MAX_POWER_OFFSET      6

/*--------------------------------------------------------------------------
E-DCH MAC-d flow maximum number of  retransmissions
--------------------------------------------------------------------------*/
#define TDSMAC_HS_UL_MAC_D_FLOW_MAX_NUM_RETX          15

/*--------------------------------------------------------------------------
MAX logical channels configured with HSUPA
--------------------------------------------------------------------------*/

#define TDSMAX_HSUPA_LOGCHAN   15

#define TDS_DDI_OR_N_LEN 6
/*--------------------------------------------------------------------------
Since there is no header for MAC-D pdus in EUL, so rlc_pdu_size is
same as mac_d_pdu_size. The pdu size should be between 16 and 5000 bits
--------------------------------------------------------------------------*/
#define TDSMAC_HS_UL_MIN_MAC_D_PDU_SIZE              16
#define TDSMAC_HS_UL_MAX_MAC_D_PDU_SIZE              5000

/*--------------------------------------------------------------------------
Data Description Identifier (6 bits), it identifies the logical channel,
MAC-d flow and size of the MAC-d PDUs concatenated into the associated
MAC-es PDU. 0-62 is the valid range. 63 is reserved
--------------------------------------------------------------------------*/
#define TDSMAC_HS_UL_MAX_DDI_VALUE                   63

/*--------------------------------------------------------------------------
Number of valid fields in mapping of rlc pdu size to ddi values. 32 is the
max
--------------------------------------------------------------------------*/
#define TDSMAC_HS_UL_NUM_RLC_PDU_SIZE_DDI            32

/*--------------------------------------------------------------------------
Maximum Number of RLC PDUS allowed in one TTI
--------------------------------------------------------------------------*/

#define TDSMAC_HS_UL_MAX_NUM_RLC_PDUS_PER_LC               63

/*--------------------------------------------------------------------------
There are 32 entries in the TEBS value table.
--------------------------------------------------------------------------*/
#define TDSMAC_HS_UL_MAX_TEBS_VALUES                 32

/*--------------------------------------------------------------------------
There are 15 entries in the HLBS value table.
--------------------------------------------------------------------------*/
#define TDSMAC_HS_UL_MAX_HLBS_VALUES                 16

/*--------------------------------------------------------------------------
MAximum value for the HARQ_ID
--------------------------------------------------------------------------*/
#define TDSMAC_HS_UL_MAX_HARQ_ID                     7
/*--------------------------------------------------------------------------
Is there is room after E-TFCI selection adn as a result of quantization,
we can attach a special header consisting of all 1s, as in 111111
--------------------------------------------------------------------------*/
#define TDSMAC_HS_UL_SI_SPL_HEADER                   0x3F

/*--------------------------------------------------------------------------
Each uint32 can contain the following
DDI1*N1*DDI2*N2*DDI3*N3-partial
Hence it can completely contain 2 sets of DDI,N and one complete ddi
--------------------------------------------------------------------------*/
#define TDSMAC_HS_UL_MAX_HDR_SIZE                    32

/*--------------------------------------------------------------------------
Retransmit times when rcv abort from L1 when using e-rucch
--------------------------------------------------------------------------*/
#define TDSMAC_MAX_ERUCCH_ABORT_CNT                  2

/* The ERUCCH data is of lenght 23 bits + 16 = 39 bis, hence requires 5 bytes to fit in a uint8 array*/
#define TDSMAC_ULHS_ERUCCH_DATA_LEN_IN_BYTES 5

#define TDSMAC_E_HDR_SIZE 12

/*TSN size*/
#define TDSMAC_ES_HDR_SIZE 6

#define TDS_MACULHS_SI_SIZE 23

#define TDS_MACULHS_SPECIAL_SI_SIZE 6

#define TDS_MACULHS_SPECIAL_SI_VAL 0x3F
/*Passed by l1 when Build Frame Ind is called for E-RUCCH*/
#define ERUCCH_TX_ETFCI 0xFE


#define TDS_HSUPA_MAC_HDR_SIZE (TDSMAC_E_HDR_SIZE + TDSMAC_ES_HDR_SIZE)



/* edch id */
#define TDSEDCH_ID 32


/*For HSUPA, HArq ID 0--3 is reseved for Scheduled flow 
   and 3--7 is used for non-scheduled flow 
   Hence MAX harq id for schedule flow is 3
  */
#define TDSMAC_ULS_HS_MAX_SCHED_HARQ_ID 3

/* Table 9.2.5.3.2.1 TEBS values (ceil in bits) */

#define TDSMAC_E_TEBS_INDEX_0 0

#define TDSMAC_E_TEBS_INDEX_1 10<<3

#define TDSMAC_E_TEBS_INDEX_2 14<<3

#define TDSMAC_E_TEBS_INDEX_3 18<<3

#define TDSMAC_E_TEBS_INDEX_4 24<<3

#define TDSMAC_E_TEBS_INDEX_5 32<<3

#define TDSMAC_E_TEBS_INDEX_6 42<<3

#define TDSMAC_E_TEBS_INDEX_7 55<<3

#define TDSMAC_E_TEBS_INDEX_8 73<<3

#define TDSMAC_E_TEBS_INDEX_9 97<<3

#define TDSMAC_E_TEBS_INDEX_10 129<<3

#define TDSMAC_E_TEBS_INDEX_11 171<<3

#define TDSMAC_E_TEBS_INDEX_12 228<<3

#define TDSMAC_E_TEBS_INDEX_13 302<<3

#define TDSMAC_E_TEBS_INDEX_14 401<<3

#define TDSMAC_E_TEBS_INDEX_15 533<<3

#define TDSMAC_E_TEBS_INDEX_16 708<<3

#define TDSMAC_E_TEBS_INDEX_17 940<<3

#define TDSMAC_E_TEBS_INDEX_18 1248<<3

#define TDSMAC_E_TEBS_INDEX_19 1658<<3

#define TDSMAC_E_TEBS_INDEX_20 2202<<3

#define TDSMAC_E_TEBS_INDEX_21 2925<<3

#define TDSMAC_E_TEBS_INDEX_22 3884<<3

#define TDSMAC_E_TEBS_INDEX_23 5160<<3

#define TDSMAC_E_TEBS_INDEX_24 6853<<3

#define TDSMAC_E_TEBS_INDEX_25 9103<<3

#define TDSMAC_E_TEBS_INDEX_26 12092<<3

#define TDSMAC_E_TEBS_INDEX_27 16062<<3

#define TDSMAC_E_TEBS_INDEX_28 21335<<3

#define TDSMAC_E_TEBS_INDEX_29 28339<<3

#define TDSMAC_E_TEBS_INDEX_30 37642<<3

#define TDSMAC_E_TEBS_INDEX_31 50000<<3




/*===========================================================================

                      MACROS

===========================================================================*/



/*===========================================================================

                     ENUMS

===========================================================================*/
/*MAC UL HS state */

typedef enum
{
  TDSMAC_ULHS_INACTIVE,
  TDSMAC_ULHS_ACTIVE
}tdsmac_ulhs_state_e_type;

/*E-RUCCH States */
typedef enum
{
  E_RUCCH_IDLE,
  E_RUCCH_PERSISTENCE_FAILED,
  E_RUCCH_WAIT_FOR_STATUS,
  E_RUCCH_RCV_NOACK,
  E_RUCCH_WAIT_FOR_MSG_SENT_CNF,
  E_RUCCH_WAIT_FOR_GRANT,
  E_RUCCH_RETX_SI,
  E_RUCCH_HYST_TMR_RUNNING
}tdsmac_ulhs_erucch_state_e_type;


/*E-RUCCH event types */
typedef enum
{
  MAC_UL_HS_ERUCCH_BLD_SI_ON_ERUCCH,
  MAC_UL_HS_ERUCCH_TRIGGER_SI,
  MAC_UL_HS_ERUCCH_T_RUCCH_TIMER_EXP,
  MAC_UL_HS_ERUCCH_T_RUCCH_HYS_TIMER_EXP,
  MAC_UL_HS_ERUCCH_PROC_L1_STATUS,
  MAC_UL_HS_ERUCCH_GRANT_RCV
}tdsmac_ulhs_erucch_event_e_type;


/*SI trigger reason */
typedef enum
{
  SI_INVAID_REASON,
  SI_TEBS_NOW_ABVE_ZERO,
  SI_NO_GRANT_TEBS_ABOVE_ZERO,
  SI_SRV_CELL_CHANGE,
  SI_ERNTI_CHANGE,
  SI_PERIODIC_TMR_EXP,
  SI_HIGH_PRIO_LOG_CHAN,
  SI_HARQ_FAIL_RETRANS,
  SI_QUANTISATION_OF_DATA
}tdsmac_ulhs_si_reason_e_type;

/*Timer status */
typedef enum
{
  MAC_ULHS_TIMER_NOT_RUNNING,
  MAC_ULHS_TIMER_STARTED,
  MAC_ULHS_TIMER_EXPIRED
}tdsmac_ulhs_timer_status_e_type;


/*===========================================================================

                     TYPE DEFINITIONS FOR MODULE

===========================================================================*/
typedef struct
{
  /* TRUE is valid & FALSE is invalid */
  boolean					  valid;
  /* E-DCH MAC-d flow power offset	0..6 (unit in dB)*/
  uint8 power_offset;	 
  /* E-DCH MAC-d flow maximum number of  retransmissions  0..15*/
  uint8 max_num_retran; 
  /* E-DCH MAC-d flow retransmission timer */
  uint16 retrans_tmr;
  
  /* E-DCH MAC-d flow multiplexing list:
	Indicates, if this is the first MAC-d flow for which PDU's are placed in
	the MAC-e PDU, the other	MAC-d flows from which MAC-d PDU's are
	allowed to be included in the same MAC-e PDU.
  Bit 0 is for MAC-d flow 0, Bit 1 is for MAC-d flow 1, …
  Value '1' for a bit means multiplexing is allowed.
  Bit 0 is Most Significant Bit(Left most bit) in the bitstring
	   NOTE: The bit that corresponds to the MAC-d flow itself is ignored  */
  uint8 mux_list;
  /* transmission grant type */
  tdsmac_e_tx_grant_info_e_type 			tx_grant_info;
}tdsmac_ulhs_mac_d_flow_info_type ;



typedef struct
{
  /* TRUE is valid & FALSE is invalid */
  boolean                     valid;
  
  /*RLC Logical channel as allocated by RRC for RLC*/
  uint8 rlc_id;

  /* E-DCH MAC-d flow identified    0..7*/
  uint8 macd_flow_id;
  /* Radio Bearer Identifier (0-31) */
  rb_id_type rb_id;
  /* RLC mode (UM or AM */
  uecomdef_logch_mode_e_type             rlc_mode;
  /* Logical channel type (DCCH or DTCH)*/
  uecomdef_logchan_e_type                 chan_type;
  /* Indicates if this channel has to be looked into for computing  scheduling info */
  boolean include_in_sched_info;
  /*Logical channel priority*/
  uint8 priority;
  
  /* RLC Hdr size = 8 bits for UM and 16 bits for AM */
  uint8 rlc_hdr_length;
  /* number of valid fields in rlc_size_ddi_info[]   0..32,
   0 is valid only when action is NOOP or STOP */
  uint8 num_rlc_size_ddi_info;
  tdsmac_e_rlc_size_ddi_info_s_type         rlc_size_ddi_info[MAX_RLC_PDU_SIZE_PER_LOGCHAN];
 
}tdsmac_ulhs_logical_ch_mac_d_flow_info_type ;


typedef struct
{
  /* This structure gives all the information about a MAC-d flow ID. 
        referenced using the Logical channel id provided by the N\W */
  tdsmac_ulhs_mac_d_flow_info_type  maculhs_mac_dflow_info[TDSMAX_E_DCH_MAC_D_FLOW];
  /*This structure gives all the information about Logical channel ID.
    referenced using the Logical channel id provided by the N\W */
  tdsmac_ulhs_logical_ch_mac_d_flow_info_type  maculhs_ul_logical_ch_mac_d_flow_mapping[TDSMAX_HSUPA_LOGCHAN];  
  /* E-RUCCH info received from RRC */
  tdsmac_eul_erucch_info_s_type  erucch_info;
  /* Si info received form RRC */
  tdsmac_eul_si_info_s_type  si_info;

  
}tdsmac_ulhs_config_info_type ;


typedef struct
  {
  /* Set to true if higher priority data arrives */
  boolean new_high_prio_chan;
  /* Set to true if a previous Harq process containing SI fails */
  boolean prev_grant_harq_fail;
  /* Set to True if there is a serving cell/working freq change */
  boolean srv_cell_change;
  /* Set to True if ERNTI is changed */
  boolean ernti_change;

  boolean si_on_erucch_not_sent;
  boolean si_on_edch_after_erucch;
}tdsmac_ulhs_si_trigger_info_s_type;


typedef struct
{

  /* True if a Grant type SI was sent on this Harq process.*/
  boolean grant_type_si_inc;
}tdsmac_ulhs_int_si_harq_status_s_type;


typedef struct
{
  /*Updated at every ETFCI selction. Indictaes if SI needs to be sent */
  boolean si_to_be_included;
  /*Indicates if Si needs to be sent on E-RUCCH */
  boolean si_on_erucch_to_be_sent;
  /*Indicates the reason for last SI trigger */
  tdsmac_ulhs_si_reason_e_type   current_si_trigger_reason;
  /* Stores the status of the trigger conditions of SI. If any of them are set to TRUE SI needs to be triggered */
  tdsmac_ulhs_si_trigger_info_s_type  si_trigger_info;

  uint8 latest_harq_id_with_si_sent;
  /* Contains the status if grant type is SI is sent on any of the Harq process. Needed to track as if that Harq fails, SI needs to be resent.*/
  boolean  harq_has_grant_si_info[TDSMAX_HARQ_PROCESS];
 } tdsmac_ulhs_si_status_info_type;

typedef struct
{
  /* Indicates the DDI that has to be attached to the MAC header */
  uint8 mac_e_hdr_info[2* TDSMAX_HSUPA_LOGCHAN];
  /* Contains the RLC PDU info i.e the number of RLC PDU per logical channel to be built, and the MAC-E S header . Sent to RLC
  */
  tdsrlc_ul_frame_format_type  rlc_pdu_info;
}tdsmac_ulhs_ddi_info_s_type;


typedef struct
{
  /* Number of logical channels at this priority level */
  uint8                     num_logical_channels;

  /* Start index for the round robin scheduling*/
  uint8                     start_idx;

  /* RLC logical channel buffer identifier (0-20) */
  rlc_lc_id_type            rlc_id[MAX_DED_LOGCHAN_PER_TRCH];

}tdsmac_hs_ul_priority_lc_mapping_info_type;



typedef struct
{
  /* Calculted at config. Contains the MAX power offset mapped to a scheduled flow */
     uint8 pwr_offset_sched_flow;
  /* Calculted at config. Contains the MAX power offset mapped to a non-scheduled flow */
     uint8 pwr_offset_non_sched_flow;
  /* Indicates if currently the buffer status is non-zero. Used to trigger SI */
    uint8 curr_buffer_status_non_zero;
  /* Indicates if previously the buffer status is non-zero. Used to track the if T_WAIT needs to start or extended estimation  window */
    uint8 prev_buffer_status_non_zero;
  
  /* Stores the current Highest logical channel containing data. Used to track to trigger SI */
    uint8 curr_high_prio_chan_having_data;
  /* Stores the previous Highest logical channel containing data. Used to track to trigger SI */
    uint8 prev_high_prio_chan_having_data;
  /* Total Scheduled Buffer Status*/
  uint32                    sched_buf_status;
  /* Total Non-Scheduled Buffer Status*/
  uint32                    non_sched_buf_status;

  /* last sub frame status */
  tdsl1_eul_tx_status_enum_type      prev_grant_status;

  boolean non_sched_flow_only_configured;
  /*Inidcates if the same priority is used for multiple logical channels. 
   If yes MAC will use round robin scheduling to schedule to them*/
  boolean same_prio_used_for_diff_lc_id;
  
  tdsmac_hs_ul_priority_lc_mapping_info_type   tdsmac_hs_ul_priority_lc_mapping[TDSMAC_UL_MAX_MLP_ID +1 ];
} tdsmac_ulhs_int_info_s_type;


typedef struct
{
  /* Loop start index when looping over priority */
  uint16 min_mlp;
  /* Loop end index when looping over priority */
  uint16 max_mlp;
  /* Loop start index when looping over logical channels */
  uint16 min_rlc_id;
  /*Loop end index when looping over logical channels*/
  uint16 max_rlc_id;
} tdsmac_ulhs_loop_limits_cfg_s_type  ;


typedef struct
{
  /* Counter to trak the extended estimation window. Since it is a small value(2-5 TTI), can be updated at each ETFCI selction called by L1. Which is called every 5 ms TTI.*/
  uint8 mac_ul_hs_extended_window_est;
  /* Tracks the number of E-RUCCH already sent for the same SI trigger request */
  uint8  mac_ul_hs_n_rucch_sent;
  /* Tracks the number of abort get from E-RUCCH status ind */
  uint8  erucch_abort_cnt;
  /* Tracks the number of sync attempts done by L1 for same E-RUCCH request */
  uint8 mac_ul_hs_num_sync_attempts;

  uint8 selected_ASC;
  tdsmac_ulhs_loop_limits_cfg_s_type       log_chan_limit_cng;
}tdsmac_ulhs_counters_s_type;


typedef struct
{
  tdsmac_ulhs_timer_status_e_type     t_wait_tmr_status;
  tdsmac_ulhs_timer_status_e_type     si_periodicity_tmr_status;
  tdsmac_ulhs_timer_status_e_type     non_sched_si_periodicity_tmr_status;
  tdsmac_ulhs_timer_status_e_type     t_erucch_tmr_status;
  tdsmac_ulhs_timer_status_e_type     t_erucch_hyst_tmr_status;
  
  rex_timer_type                        tds_ulhs_t_wait_tmr_blk;
  rex_timer_type                        si_periodicity_tmr_blk;
  rex_timer_type                        non_sched_si_periodicity_tmr_blk;
  rex_timer_type                        t_erucch_tmr_blk;
  rex_timer_type                        t_erucch_hyst_tmr_blk;
  
}tdsmac_ulhs_int_tmr_status_s_type;


/*MAC ULHS timer types*/
typedef enum
{
  TDSMAC_ULHS_INVALID_TMR, /*Invlaid*/
  
  TDSMAC_ULHS_T_WAIT_TMR,/*T-Wait timer*/
  
  TDSMAC_ULHS_T_ERUCCH_TMR,/*T-RUCCH timer*/
  
  TDSMAC_ULHS_T_ERUCCH_HYST_TMR,/*T-RUCCH Hysteresis timer*/
  
  TDSMAC_ULHS_PERIODIC_SI_TMR,/*Periodic SI  timer*/
  
  TDSMAC_ULHS_NON_SCHED_PERIODIC_SI_TMR,/*Periodic SI timer when only Non-Scheduled flow configured  timer*/
  
  TDSMAC_ULHS_ALL_TMR /*All the MAC ULHS timers*/
}tdsmac_ulhs_timers_e_type;







typedef struct
{
  /* Contains the TEBS table */
  uint32 ul_tebs_table_values[TDSMAC_HS_UL_MAX_TEBS_VALUES];
  /* Contains the HLBS table */
  uint32 ul_hlbs_table_values[TDSMAC_HS_UL_MAX_HLBS_VALUES];

  uint8 current_uph;
  
  uint8 snpl;

}tdsmac_ulhs_int_table_val_type;





typedef struct
{
  uint32 current_tsn_lst[TDSMAX_HSUPA_LOGCHAN];
  uint32 backup_tsn_lst[TDSMAX_HSUPA_LOGCHAN];
} tdsmac_ulhs_tsn_info_type;




typedef struct
{
  uint32 padding;
  uint16 max_allowed_bits;
  uint16 total_calc_blk_size ;
  uint16 rlc_pdu_size ;
  uint16 max_retran_tmr_sel;
  uint8 max_retran_num_sel;
  uint8 max_etfci_idx_sel;
  uint8 min_etfci_idx_sel;
#ifdef FEATURE_TDSCDMA_HSUPA_PADDING_MAXIMUM
  uint8 time_slot_num;
#endif
  boolean spl_hdr_incl;
  boolean si_included;
  boolean highest_prio_mac_dflow_sel;

}tdsmac_ulhs_bld_pdu_info_type; 

typedef struct
{
  /*Logical channel provided by the N\W*/
  uint8 lc_id;
  /*MAC-D flow id*/
  uint8 mac_dflow_id;
}tdsmac_ulhs_lcid_macd_mapping_type;


/*===========================================================================

      Declaration

===========================================================================*/
extern tdsmac_ulhs_si_status_info_type                  tdsmac_ulhs_si_info;
extern tdsmac_ulhs_bld_pdu_info_type                    tdsmac_ulhs_int_bld_mace_pdu_info;
extern tdsmac_ulhs_int_info_s_type                      tdsmac_ulhs_int_status_info;
extern tdsmac_ulhs_int_tmr_status_s_type                tdsmac_ulhs_int_tmr_status;
extern tdsrlc_ul_lc_info_type                           tdsmac_hs_ul_lc_info;
extern tdsmac_ulhs_lcid_macd_mapping_type  
          tdsmac_ulhs_rlc_id_mac_dflow_lc_id_mapping[TDSUE_MAX_UL_LOGICAL_CHANNEL];
extern tdsrlc_ul_buf_status_type                        tdsmac_hs_ul_lc_buf_status;
extern uint8                                            tdsmac_hs_ul_cur_cfn;
extern tdsmac_ulhs_ddi_info_s_type                      tdsmacul_hs_ddi_info;
extern tdsmac_ulhs_config_info_type                     tdsmaculhs_config_info;


/*===========================================================================

                         GLOBAL FUNCTIONS

===========================================================================*/

#ifdef FEATURE_TDSCDMA_CRITICAL_SECTION_CLEANUP
/*===========================================================================
FUNCTION    tdsmac_ulhs_crit_init

DESCRIPTION
     Init MAC critical sections for HSUPA.
      

DEPENDENCIES
    None
RETURN VALUE
    None
SIDE EFFECTS
    None
===========================================================================*/
void tdsmac_ulhs_crit_sect_init(void);
#endif

/*===========================================================================
FUNCTION	tdsmac_ulhs_init_parameter
	
DESCRIPTION
	Init MAC globel varible for HSUPA.
	Be called when UL L2 task routine start 
	
DEPENDENCIES
	None
RETURN VALUE
	None
SIDE EFFECTS
	None
===========================================================================*/
void tdsmac_ulhs_init
(
  boolean is_pwr_up
);
/*===========================================================================
FUNCTION    tdsmac_ulhs_config_handler

DESCRIPTION
    The function save HSUPA config into MAC globel varible.
    HSUPA config is comanyed with other UL config.
    This func is invoked from tdsconfig_ul_mac.

DEPENDENCIES
    None
RETURN VALUE
    None
SIDE EFFECTS
    None
===========================================================================*/
void tdsmac_ulhs_config_handler (  
  /* HSUPA related information */
  tdsmac_e_config_s_type      *mac_e_cfg_ptr,

  /* indciates whether we need to back-up TSN or revert back to stored TSN */
  tdsmac_e_tsn_action_e_type   mac_e_tsn_action,

  /* Specifies if the harq related information has to be flushed or not */
  boolean                        mac_e_reset_indicator,

  /*Specifies the action for this config */
  tdsmac_eul_action_e_type        mac_eul_action
);


/*===========================================================================
FUNCTION    tdsmac_ulhs_update_total_bytes_for_eul_lc

DESCRIPTION


This function updates the totoal number of bytes available for each EUL LC .
This can be used later for buffer status reporting or builidng PDU 

DEPENDENCIES

RETURN VALUE
None

SIDE EFFECTS

===========================================================================*/

void  tdsmac_ulhs_update_total_bytes_for_eul_lc
(
  tdsrlc_ul_lc_buf_status_type                *buf_status_ptr,
  uint8 lc_id
  
);


/*===========================================================================
FUNCTION    tdsmac_ulhs_suspend

DESCRIPTION


This function suspend MAC HSUPA.  Currently used in handover case.

DEPENDENCIES

RETURN VALUE
None

SIDE EFFECTS

===========================================================================*/

void  tdsmac_ulhs_suspend
(
  void
);


#endif /* MAC_HS_UL_H */

