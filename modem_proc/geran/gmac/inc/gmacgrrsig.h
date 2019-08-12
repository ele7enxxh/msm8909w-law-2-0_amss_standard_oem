/*****************************************************************************
***
*** TITLE
***
***  GPRS MAC GRR to MAC SIGNAL DEFINITION
***
***
*** DESCRIPTION
***
***  Signal definition for use with access function GPRS_MAC_GRR_ACC_FUNC()
***
***  Defines the ENUM for all the signals used from GRR to MAC  .
***
***  Defines the Union that holds all the signal struct definitions
***  used from GRR to MAC
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/inc/gmacgrrsig.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 09/11/10   sjw     Updated for Dual SIM support
*** 10-04-01   tjw     Add customer.h and comdef.h to all SU API files
*** 06/26/01   tlx     Created
*****************************************************************************/

#ifndef INC_GMACGRRSIG_H
#define INC_GMACGRRSIG_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif


#include "queue.h"
#include "csn.h"
#include "rr_l1.h"
#include "rr_l1_g.h"

#ifdef FEATURE_GSM_DTM
#include "gprsdef.h"
#include "gprsdef_g.h"
#endif /*FEATURE_GSM_DTM*/
#include "geran_multi_sim.h"

/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/

/**************************************************/
/* The enum values below are assigned for logging */
/* purposes - please do not change them!          */
/* If a signal is removed, it's number should be  */
/* unassigned, not re-used                        */
/**************************************************/
typedef enum
{
   /*GRR_MAC_CELL_RESEL_IND           = 0x00 Obsolete for IPA
    ,GRR_MAC_CAMPED_ON_CELL           = 0x01 Obsolete for IPA
    ,GRR_MAC_PCCO_CAMPED_ON_CELL      = 0x02 Obsolete for IPA
    ,GRR_MAC_T3174_EXPIRED            = 0x03 Obsolete for IPA
    ,GRR_MAC_T3176_EXPIRED            = 0x04 Obsolete for IPA */
     GRR_MAC_SEND_MEAS_RPT            = 0x05
  /*,GRR_MAC_GPRS_SUSPENSION          = 0x06 Obsolete for IPA */
    ,GRR_MAC_SEND_PCCF                = 0x07
    ,GRR_MAC_DL_TBF_EST               = 0x08
    ,GRR_MAC_DL_SB_EST                = 0x09
    ,GRR_MAC_UL_TBF_EST               = 0x0a
    ,GRR_MAC_UL_SB_EST                = 0x0b
  /*,GRR_MAC_UL_TBF_EST_REJ           = 0x0c Obsolete for IPA
    ,GRR_MAC_PACKET_PAGE_IND          = 0x0d Obsolete for IPA
    ,GRR_MAC_PDCH_ASSIGNMENT          = 0x0e Obsolete for IPA
    ,GRR_MAC_NO_SERVICE               = 0x0f Obsolete for IPA
    ,GRR_MAC_CS_PAGING_ACCESS_REJ     = 0x10 Obsolete for IPA
    ,GRR_MAC_ASSIGN_REQ               = 0x11 Obsolete for IPA */
    ,GRR_MAC_RACH_REQ                 = 0x12
    ,GRR_MAC_SEND_PCCN_REQ            = 0x13
    ,GRR_MAC_NACC_RELEASE_REQ         = 0x14
  /*,GRR_MAC_SEND_PPS_REQ             = 0x15 Obsolete - PBCCH */
    ,GRR_MAC_SEND_PSS_REQ             = 0x16
    ,GRR_MAC_RESET_REQ                = 0x17
#ifdef FEATURE_GSM_DTM
    ,GRR_MAC_DTM_CHANNEL_ASSIGN_REQ    = 0x18
    ,GRR_MAC_DTM_CHANNEL_RELEASE_IND   = 0x19
    ,GRR_MAC_DTM_UL_DCCH_IND           = 0x1a
    ,GRR_MAC_DTM_UL_DCCH_CNF           = 0x1b
#ifdef FEATURE_GSM_EDTM
    ,GRR_MAC_EDTM_PKT_CS_EST_REQ       = 0x1c
#endif /*FEATURE_GSM_EDTM*/
#endif /*FEATURE_GSM_DTM*/
/********************************************************
*                  IPA NEW SIGNALS                      *
********************************************************/
    ,GRR_MAC_NO_PS_ACCESS_IND           = 0x1d
    ,GRR_MAC_PS_ACCESS_REQ              = 0x1e
#ifdef FEATURE_GPRS_PS_HANDOVER
    ,GRR_MAC_PSHO_REQ                  = 0x1f
    ,GRR_MAC_PSHO_COMPLETE_IND         = 0x20
#endif

} grr_mac_sig_id_t;

/*************************************************************************
*   "GRR_MAC_SEND_MEAS_RPT" GRR to MAC message structure
*************************************************************************/
#define NC_NON_DRX_PERIOD_0     0
#define NC_NON_DRX_PERIOD_24    1
#define NC_NON_DRX_PERIOD_48    2
#define NC_NON_DRX_PERIOD_72    3
#define NC_NON_DRX_PERIOD_96    4
#define NC_NON_DRX_PERIOD_120   5
#define NC_NON_DRX_PERIOD_144   6
#define NC_NON_DRX_PERIOD_192   7

typedef enum
{
  REPORT_TYPE_PMR,
  REPORT_TYPE_PEMR
} pmr_or_pemr_e;

typedef union
{
  packet_measurement_report_t pmr;
  packet_enhanced_measurement_report_t pemr;
} pmr_or_pemr_u;

typedef struct
{
  pmr_or_pemr_e msg_type;
  pmr_or_pemr_u msg;
  uint8 nco;
  uint8 nc_non_drx_period;
  gas_id_t gas_id;
} gm_send_meas_rpt_t;


/*************************************************************************
*   "GRR_MAC_SEND_PCCF" GRR to MAC message structure
*************************************************************************/
#define PCCF_FREQ_NOT_IMPLEMENTED        0x0
#define PCCF_NO_RESPONSE_ON_TARGET_CELL  0x1
#define PCCF_UL_ACCESS_REJECT            0x2
#define PCCF_ONGOING_CS_CONNECTION       0x3
#define PCCF_MS_IN_STANDBY_STATE         0x5
#define PCCF_FORCED_TO_STANDBY_STATE     0x6
#define PCCF_NOT_REQUIRED                0xF
#define REASON_NOT_REQUIRED             3   /* PDU deleted, retry allowed */
#define GPRS_SUSPENSION_IMMINENT        7   /* PDU deleted, no retry until CAMPED_ON_CELL */
typedef struct
{
  packet_cell_change_failure_t pccf;
  gas_id_t gas_id;
} gm_send_pccf_t;

/*************************************************************************
*   "GRR_MAC_DL_TBF_EST" GRR to MAC message structure
*************************************************************************/

/* Used for the hopping encoding in UL/DL SB and TBF definitions below */
#define DIRECT 0
#define INDIRECT 1

typedef struct
{
  byte tn;
  byte tsc;
  boolean hopping_flag;
  byte maio;
  byte hsn;
  frequency_list_T frequency_list;

  byte tfi_assignment;
  byte rlc_mode;
  option_status_e alpha_flag;
  byte alpha;
  byte gamma;
  byte polling_bit;
  byte ta_valid;

  option_status_e timing_advance_idx_flag;
  byte timing_advance_idx;

  option_status_e tbf_starting_time_flag;
  word tbf_starting_time;

  option_status_e dl_pwr_ctrl_flag;
  dwnlk_pwr_ctrl_t dl_pwr_ctrl;

  byte timing_advance;
  uint32 tlli;


  boolean egprs_alloc;
  uint8   egprs_dl_ws;
  uint8   egprs_lq_meas_mode;
  boolean egprs_bep_period2_present;
  uint8   egprs_bep_period2;

  gas_id_t gas_id;
} gm_dl_tbf_est_t;

/*************************************************************************
*   "GRR_MAC_DL_SB_EST" GRR to MAC message structure
*************************************************************************/
typedef struct
{
  byte tn;
  byte tsc;
  boolean hopping_flag;
  byte maio;
  byte hsn;
  frequency_list_T frequency_list;

  byte timing_advance;

  word tbf_starting_time;
  uint32 tlli;
  gas_id_t gas_id;
} gm_dl_sb_est_t;

/*************************************************************************
*   "GRR_MAC_UL_TBF_EST" GRR to MAC message structure
*************************************************************************/

typedef struct
{
  boolean egprs_alloc;
  uint8   access_tech_type_count;
  uint8   access_tech_type[MAX_ACCESS_TECH_TYPE];
  uint8   egprs_channel_coding_command;
  boolean bep_period2_present;
  uint8   bep_period2;
  uint8   resegment;
  uint8   egprs_window_size; /* 5-bit encoded, see 04.60 sect 12.5.2 */
} gm_egprs_ul_tbf_t;

typedef struct
{
  byte tn;
  byte tsc;
  boolean hopping_flag;
  byte maio;
  byte hsn;
  frequency_list_T frequency_list;
  byte tfi;
  byte polling_bit;

  alloc_flag_e alloc_flag;
  union {
      ia_rest_octets_dynamic_alloc_t dynamic_alloc;
      ia_rest_octets_fixed_alloc_t   fixed_alloc;
  } alloc;

  gm_egprs_ul_tbf_t egprs_ul_tbf;
  byte    ch_block_coding_cmd;
  byte    tlli_block_ch_coding;
  byte    alpha;
  byte    gamma;
  option_status_e timing_advance_index_flag;
  byte    timing_advance_index;
  option_status_e tbf_starting_time_flag;
  word    tbf_starting_time;
  byte    timing_advance;

  uint32  tlli;
  uint8   retry_bit;

  gas_id_t gas_id;
} gm_ul_tbf_est_t;

/*************************************************************************
*   "GRR_MAC_UL_SB_EST" GRR to MAC message structure
*************************************************************************/

typedef struct
{
  boolean egprs_alloc;
  uint8   access_tech_type_count;
  uint8   access_tech_type[MAX_ACCESS_TECH_TYPE];
  uint8   egprs_num_radio_blocks;
} gm_egprs_ul_sb_est_t;

typedef struct
{
  byte tn;
  byte tsc;
  boolean hopping_flag;
  byte maio;
  byte hsn;
  frequency_list_T frequency_list;

  option_status_e alpha_flag;
  byte alpha;
  byte gamma;
  word starting_time;
  option_status_e dl_pwr_ctrl_flag;
  dwnlk_pwr_ctrl_t dl_pwr_ctrl;
  gm_egprs_ul_sb_est_t egprs_ul_sb_est;

  byte timing_advance;

  gas_id_t gas_id;
} gm_ul_sb_est_t;

typedef struct
{
  uint8 page_type;

  gas_id_t gas_id;
} gm_packet_page_ind_t;

/*************************************************************************
*   "GRR_MAC_PDCH_ASSIGNMENT" GRR to MAC message structure
*************************************************************************/
typedef struct
{
  uint8 dummy_data;

  gas_id_t gas_id;
} gm_pdch_assignment_t;

/*************************************************************************
*   "GRR_MAC_CS_PAGING_ACCESS_REJ" GRR to MAC message structure
*************************************************************************/
typedef struct
{
  uint8 dummy_data;

  gas_id_t gas_id;
} gm_cs_paging_access_rej_t;

/*************************************************************************
*   "GRR_MAC_ASSIGN_REQ" GRR to MAC message structure
*************************************************************************/
/*
contents_description:

76543210
xxxxxx00  TMSI not changed; not valid / deallocated
xxxxxx01  TMSI not changed; valid
xxxxxx10  TMSI changed; not valid / decallocated
xxxxxx11  TMSI changed; valid
xxxx00xx  P-TMSI / TLLI not changed; not valid / deallocated
xxxx01xx  P-TMSI / TLLI not changed; valid
xxxx10xx  P-TMSI / TLLI changed; not valid / decallocated
xxxx11xx  P-TMSI / TLLI changed; valid
*/
#define TMSI_VALID_CHECK            0x01
#define TMSI_CHANGED_CHECK          0x02
#define PTMSI_VALID_CHECK           0x04
#define PTMSI_CHANGED_CHECK         0x08

typedef struct
{
  uint8           contents_description;
  uint32          tlli;
  uint32          ptmsi;
  uint32          tmsi;
  boolean         ready_timer_force_to_standby;

  gas_id_t        gas_id;
} gm_assign_req_t;

/*************************************************************************
*   "GRR_MAC_RACH_REQ" GRR to MAC message structure
*************************************************************************/
typedef struct
{
  uint8 dummy_data;
  gas_id_t gas_id;
} gm_rach_req_t;

#define MAX_NUMBER_OF_NACC_NCELLS       4
#define SERVING_CELL_CONTAINER MAX_NUMBER_OF_NACC_NCELLS

/*************************************************************************
*   "GRR_MAC_SEND_PCCN_REQ" GRR to MAC message structure
*************************************************************************/
typedef struct
{
  packet_cell_change_notification_t pccn_msg;
  gas_id_t gas_id;
} gm_send_pccn_req_t;

/*************************************************************************
*   "GRR_MAC_NACC_RELEASE_REQ" GRR to MAC message structure
*************************************************************************/
#define NACC_RELEASE_ONE 0x01
#define NACC_RELEASE_ALL 0x02

typedef struct
{
  uint8   container_id;
  uint8   release_flags;
  gas_id_t gas_id;
} gm_nacc_release_req_t;

/*************************************************************************
*   "GRR_MAC_SEND_PSS_REQ" GRR to MAC message structure
*************************************************************************/
typedef struct
{
  packet_si_status_t pss_msg;
  gas_id_t gas_id;
} gm_send_pss_req_t;

/*************************************************************************
*   "GRR_MAC_RESET_REQ" GRR to MAC message structure
*************************************************************************/
typedef struct
{
  uint8 dummy_data;
  gas_id_t gas_id;
} gm_reset_req_t;


#ifdef FEATURE_GSM_DTM

/*************************************************************************
*   "GRR_MAC_DTM_CHANNEL_ASSIGN_REQ" GRR to MAC message structure
*************************************************************************/

typedef struct
{
  uint8                              current_cs_timeslot; /*used by MAC to perform*/
                                                          /*multislot checking    */
  dtm_tbf_type_t                     dtm_tbf_type;
  gprs_l1_cell_opt_t                 gprs_l1_cell_opt;

  /****************      PS achannel discription **********************************/
  rr_packet_uplink_assignment_t      gm_dtm_ul_tbf_ass;    /*type defined in csn.h*/
  rr_packet_downlink_assignment_t    gm_dtm_dl_tbf_ass;    /*type defined in csn.h*/

  /******************   frquency discription     **********************************/
  boolean                            dtm_freq_present;
  frequency_information_T            dtm_freq_info;

  /****************** CS channel discription    ***********************************/
  boolean                            cs_present;   /*True => "cs_desc is" present */
  cs_chan_desc_t                     cs_desc;

  gas_id_t                           gas_id;
} gm_dtm_channel_assign_req_t;

/*************************************************************************
*   "GRR_MAC_DTM_CHANNEL_RELEASE_IND" GRR to MAC message structure
*************************************************************************/
typedef enum
{
  HANDOVER_REL,
  NORMAL_REL
} dtm_ps_rel_cause_t;

typedef struct
{
  boolean                   dtm_ps_release;    /* True  = Release all PS TBFs */
                                               /* False = EDTM: maintain TBFs */
  dtm_ps_rel_cause_t        dtm_ps_rel_cause;

  gas_id_t                  gas_id;
} gm_dtm_channel_release_ind_t;

/*************************************************************************
*   "GRR_MAC_DTM_UL_DCCH_IND" GRR to MAC message structure
*************************************************************************/
typedef struct
{
  uint8 dummy_data;
  gas_id_t gas_id;
} gm_dtm_ul_dcch_ind_t;
/*************************************************************************
*   "GRR_MAC_DTM_UL_DCCH_CNF" GRR to MAC message structure
*************************************************************************/
typedef struct
{
  uint8 dummy_data;
  gas_id_t gas_id;
} gm_dtm_ul_dcch_cnf_t;
#endif /*FEATURE_GSM_DTM*/

#ifdef FEATURE_GSM_EDTM
/*************************************************************************
*   "GRR_MAC_EDTM_PKT_CS_EST_REQ" GRR to MAC message structure
*************************************************************************/
typedef struct
{
  uint8 rr_est_cause;
  gas_id_t gas_id;
} gm_edtm_pkt_cs_est_req_t;
#endif /*FEATURE_GSM_EDTM*/

/*********************************************
*      IPA NEW SIGNAL STRUCTURES             *
**********************************************/
/*************************************************************************
*   "GRR_MAC_PS_ACCESS_REQ" GRR to MAC message structure
*************************************************************************/
typedef struct
{
  boolean                   pcco_in_progress;
  action_on_ps_access_t     action;
  gas_id_t                  gas_id;
} gm_ps_access_req_t;

/*************************************************************************
*   "GRR_MAC_NO_PS_ACCESS_IND" GRR to MAC message structure
*************************************************************************/
typedef struct
{
  delete_pdu_cause_t delete_pdu;
  gas_id_t           gas_id;
} gm_no_ps_access_ind_t;

#ifdef FEATURE_GPRS_PS_HANDOVER
typedef struct
{
  psho_type_t                  psho_type;
  ps_handover_command_t        psho_cmd;
  gas_id_t                     gas_id;
} gm_psho_req_t;

typedef struct
{
  psho_status_t             psho_status;
  gas_id_t                  gas_id;
} gm_psho_complete_ind_t;
#endif /*FEATURE_GPRS_PS_HANDOVER*/
/*************************************************************************
*   Union of all GRR to MAC message structure
*************************************************************************/
typedef union
{
  gm_send_meas_rpt_t              gm_send_meas_rpt;
  gm_send_pccf_t                    gm_send_pccf;
  gm_dl_tbf_est_t                   gm_dl_tbf_est;
  gm_dl_sb_est_t                    gm_dl_sb_est;
  gm_ul_tbf_est_t                   gm_ul_tbf_est;
  gm_ul_sb_est_t                    gm_ul_sb_est;
  gm_packet_page_ind_t        gm_packet_page_ind;
  gm_pdch_assignment_t        gm_pdch_assignment;
  gm_cs_paging_access_rej_t gm_cs_paging_access_rej;
  gm_assign_req_t           gm_assign_req;
  gm_rach_req_t             gm_rach_req;
  gm_send_pccn_req_t        gm_send_pccn_req;
  gm_nacc_release_req_t     gm_nacc_release_req;
  gm_send_pss_req_t         gm_send_pss_req;
  gm_reset_req_t            gm_reset_req;
#ifdef FEATURE_GSM_DTM
  gm_dtm_channel_assign_req_t      gm_dtm_channel_assign_req;
  gm_dtm_channel_release_ind_t     gm_dtm_channel_release_ind;
  gm_dtm_ul_dcch_ind_t                 gm_dtm_ul_dcch_ind;
  gm_dtm_ul_dcch_cnf_t                 gm_dtm_ul_dcch_cnf;
#endif /*FEATURE_GSM_DTM*/
#ifdef FEATURE_GSM_EDTM
  gm_edtm_pkt_cs_est_req_t         gm_edtm_pkt_cs_est_req;
#endif /*FEATURE_GSM_EDTM*/
/********************************************************
*        IPA NEW STRUCTURES                             *
*********************************************************/
  gm_no_ps_access_ind_t            gm_no_ps_access_ind;
  gm_ps_access_req_t               gm_ps_access_req;
#ifdef FEATURE_GPRS_PS_HANDOVER
  gm_psho_req_t                    gm_psho_req;
  gm_psho_complete_ind_t           gm_psho_complete_ind;
#endif /*FEATURE_GPRS_PS_HANDOVER*/
} grr_mac_msg_u;

/*************************************************************************
*   GRR to MAC signal defintion
*************************************************************************/
typedef struct
{
   q_link_type        link;
   byte               task_id;
   grr_mac_sig_id_t   sig_id;

   grr_mac_msg_u      msg;
} grr_mac_sig_t;

#endif /* INC_GMACGRRSIG_H */
/*** EOF: don't remove! ***/
