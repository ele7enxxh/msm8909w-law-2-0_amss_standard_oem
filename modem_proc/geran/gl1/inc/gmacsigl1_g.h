/*****************************************************************************
***
*** TITLE
***
***  GPRS MAC L1 to MAC SIGNAL DEFINITION (only for test purposes)
***
***
*** DESCRIPTION
***
***  Signal definition for use with access function GPRS_l1_MAC_ACC_FUNC()
***
***  Defines the ENUM for all the signals used from MAC to L1  .
***
***  Defines the Union that holds all the signal struct definitions
***  used from MAC to L1
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl1/inc/gmacsigl1_g.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 06/05/10   dv      CR237642 - Add ASRB funtionality
*** 10-04-01   tjw     Add customer.h and comdef.h to all SU API files
*** 10-03-30   tjw     Explicitly include dependent headers to make SU API files
***                    stand alone
*** 07/03/08   ip      Added PSHO changes under FEATURE_GPRS_PS_HANDOVER
*** 06/11/06   og      UL ctrl message handling updates provided by rnorthcot.
*** 16/10/06   og      EDTM enhanced release updates.
*** 21/3/05    og      Lint error removal.
*** 2/11/04    og      Addition of egprs srb test mode capability.
*** 30/07/04   ws      Added EGPRS signal definitions
*** 15/08/03   ws      Added reuse_sb_frq_alloc flag to MAC_L1_FREQ_CONFIG
*** 03/04/03   pjr     Added PRACH abort message.
*** 01/11/02   pjr     Moved L1_GPRS_INVALID_FN into here so common to MAC and PL1
*** 09/18/02   sb      Add PCA_DL_FAI_MSG to rrbp_msg_t for RRBP
*** 09/11/02   pjr     Changed PDAN_WITH_FAI_OR_CRD into two emuns
                       PDAN_WITH_FAI and PDAN_WITH_CRD
    07/16/02   DLH     Removed frequency_list_T and frequency_information_T
                       and put them into rr_l1.h
*** 06/26/01   tlx     Created

*****************************************************************************/

#ifndef INC_GMACSIGL1_G_H
#define INC_GMACSIGL1_G_H


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

#include "gprsdef.h"
#include "gprsdef_g.h"
#include "nas_exp.h"
#include "rr_l2_l1.h"
#include "rr_l1.h"
#include "rr_l1_g.h" // Added for L1/MAC merge WS.
#include "csn_apt.h"

#include "geran_multi_sim.h"
/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/

/* defines number of frequencies in Mobile allocation list
 */
//#define MAX_MA_CHANNEL    removed as it is defined in rr_l1.h

/* defined the maximum length of the fixed allocation bitmap
 */
#define MAX_BITMAP_LEN     16

/* defines the timeslot allocation. If present is not
 * set then the timeslot allocation can be ignored
 * Bit 0 = TN7  Bit7 = TN0 as defined in GSM 04.60
 */
typedef struct
{
  boolean     present;
  uint8       tn;
} timeslot_allocation_T;


typedef struct
{
  uint8 alpha;
  uint8 t_avg_w;
  uint8 t_avg_t;
  uint8 pb;
  uint8 pc_meas_chan;
  uint8 int_meas_channel_list_avail;
  uint8 n_avg_i;
} global_pwr_ctrl_param_T;

typedef struct
{
  timing_advance_params_T ta_params;
} mac_l1_pkt_ta_T;

/*
 * repeat allocation for uplink fixed TBF. Received in PUAN
 */
typedef struct
{
  boolean present;
  uint8   ts_override;
} mac_fixed_repeat_alloc_T;
/* Defines parameters to describe the fixed allocation
 * description
 */
typedef struct
{
  mac_fixed_repeat_alloc_T      mac_fixed_repeat_alloc;
  mac_fixed_allocation_bitmap_T mac_fixed_allocation_bitmap;

  mac_fixed_control_tn_T        control_tn;
  downlink_pwr_t                downlink_pwr;

  timeslot_allocation_T         timeslot_allocation;

  power_control_params_T        power_control_params;
  timing_advance_params_T       timing_advance_params;

  uint32                        starting_time;
} mac_fixed_tbf_information_T;


/*************************************************************************
* "MAC_L1_FIXED_TBF_CONFIG" MAC to l1 message structure
*************************************************************************/
typedef struct
{
  IMH_T                           message_header;
  mac_fixed_tbf_information_T     fixed_tbf;

  gas_id_t                        gas_id;

} mac_l1_fixed_tbf_config_t;


/*************************************************************************
* "MAC_L1_SINGLE_BLOCK_CONFIG " MAC to l1 message structure
*************************************************************************/
typedef struct
{
  uint8                   nco;
  uint8                   nc_non_drx_period;
  boolean                 meas_report;
  boolean                 two_phase_access;
  uint8                   ts_number;
  boolean                 alpha_present;
  uint8                   alpha;
  uint8                   gamma_tn;
  downlink_pwr_t          downlink_pwr;
  uint32                  starting_time;
  timing_advance_params_T timing_advance_params;

  /* new parameter EGPRS */

  bep_period2_T           bep_period2;
  tbf_mode_T              tbf_mode;
  uint8                   num_rb_alloc;

} mac_sb_tbf_information_T;


typedef struct
{
  IMH_T                      message_header;

  mac_sb_tbf_information_T   sb_tbf;

  gas_id_t                   gas_id;

} mac_l1_single_block_config_t;


/*************************************************************************
* "MAC_L1_DL_SINGLE_BLOCK " MAC to l1 message structure
*************************************************************************/
typedef struct
{
  IMH_T   message_header;
  uint8   tn;
  uint32  tbf_starting_time;
  gas_id_t gas_id;
} mac_l1_dl_single_block_t;

/*************************************************************************
* "MAC_L1_PACCH_RRBP" LMAC to l1 message structure
*************************************************************************/
typedef enum
{
  AB_8_BIT,
  AB_11_BIT,
  NB
} pcr_format_t;

typedef enum
{
  PRR_MSG,                    //used for ul re-est via ul release process
  PCCF_MSG,
  PMR_MSG,
  PCA_UL_MSG,                 //PCA for uplink
  PCA_UL_FAI_MSG,             // PCA for uplink when FAI is set
  PCA_DL_MSG,                 //PCA for downlink
  PCA_51_MSG,                 //PCA for polling on 51 MF access procedure
  PCA_DL_FAI_MSG,             //PCA for DL unack mode where no PDAN with FAI is transmitted
  PCA_SRB,                    // PCA for the Packet TBF Release message when in EGPRS SRB mode.
  PDAN_MSG,                   //packet dl ack nak
  PDAN_MSG_WITH_FAI,          //PDAN contains Final Ack Indicator set to 1
  PDAN_MSG_WITH_CRD           //PDAN contains Channel Req Desc for UL
} rrbp_msg_t;

typedef struct
{
  IMH_T         message_header;
  uint32        absolute_fn;          // frame  number on which the pacch is to be sent
  uint8         ts_of_dl_msg;         // ts dl msg was received on
  pcr_format_t  msg_burst_type;       // the burst type being sent on the pacch
  rrbp_msg_t    rrbp_msg;             // what type of ul msg is in rrbp buffer
#ifdef PL1_FW_SIM
  uint8         pacch_ab_data[SIZE_OF_CONTROL_RADIO_BLOCK + 1];    // data to be sent on pacch
#else
  uint8         pacch_ab_data[SIZE_OF_CONTROL_RADIO_BLOCK];    // data to be sent on pacch
#endif

#ifdef FEATURE_GSM_EDTM
  uint8         message_type;
#endif /*FEATURE_GSM_EDTM*/
  gas_id_t      gas_id;
} mac_l1_pacch_rrbp_t;

/*************************************************************************
* "MAC_L1_CS_RECONFIG " MAC to l1 message structure
*************************************************************************/
typedef struct
{
  IMH_T     message_header;
  byte      dummy_data;
  gas_id_t  gas_id;
} mac_l1_cs_reconfig_t;

/*************************************************************************
* "MAC_L1_POWER_CONTROL " MAC to L1 message structure
*************************************************************************/
typedef struct
{
  IMH_T                   message_header;
  boolean                 global_packet_ta_present;
  global_packet_ta_T      global_packet_ta;
  power_control_params_T  power_control;
  gas_id_t                gas_id;
}mac_l1_pwr_ctrl_t;

/*************************************************************************
* "MAC_L1_PAGE_MODE " MAC to l1 message structure
*************************************************************************/

typedef struct
{
  IMH_T         message_header;
  uint8         current_page_mode;
  gas_id_t      gas_id;
} mac_l1_page_mode_t;

/*************************************************************************
* "MAC_L1_PDCH_REL_TS " MAC to l1 message structure
*************************************************************************/
typedef struct
{
  IMH_T     message_header;
  byte      ts_allocation;
  gas_id_t  gas_id;
}mac_l1_pdch_rel_ts_t ;

/*************************************************************************
* "MAC_L1_DUMMY_CTRL_BLOCK " MAC to l1 message structure
*************************************************************************/
typedef struct
{
  IMH_T   message_header;

#ifdef PL1_FW_SIM
  /* even bytes required for PL1 FW simulation */
  uint8   dummy_ctrl_block[SIZE_OF_CONTROL_RADIO_BLOCK + 1];
#else
  uint8   dummy_ctrl_block[SIZE_OF_CONTROL_RADIO_BLOCK];
#endif
  gas_id_t gas_id;
} mac_l1_dummy_ctrl_block_t;

/*************************************************************************
* " MAC_L1_QUALITY_RPT " MAC to l1 message structure
*************************************************************************/
/*  L1_GPRS_INVALID_FN defined as 0xffffff and used in PL1 */
#define L1_GPRS_INVALID_FN  (uint32)0xffffff
#define MAC_L1_RRBP_VALUE_NOT_VALID  L1_GPRS_INVALID_FN


typedef struct
{
  IMH_T       message_header;
  boolean     i_level_ind;
  uint32      rrbp_value;
  enum
  {
    GPRS_RPT,
    EGPRS_RPT
  } qual_rpt_type;
  gas_id_t    gas_id;
}mac_l1_quality_rpt_t;

/*************************************************************************
* "MAC_L1_CONTENTION_RES" MAC to L1 message structure
*************************************************************************/
typedef struct
{
  IMH_T message_header;
  gas_id_t gas_id;
} mac_l1_contention_res_t;

/*************************************************************************
* "MAC_L1_STOP_RLC_TX" MAC to L1 message structure
*************************************************************************/
typedef struct
{
  IMH_T message_header;
  gas_id_t gas_id;
} mac_l1_stop_rlc_tx_t;

#if  defined (FEATURE_GSM_DTM) || defined (FEATURE_GPRS_PS_HANDOVER)

typedef struct
{
  uint8                   mac_mode;
  uint8                   ts_allocation;
  downlink_pwr_t          downlink_pwr;
  boolean                 tbf_start_time_valid;
  uint32                  tbf_start_time;
  timing_advance_params_T ta_params;
  power_control_params_T  power_control;
  boolean                 dl_tfi_present;
  uint8                   dl_tfi;

  bep_period2_T           bep_period2;
  tbf_mode_T              tbf_mode;
  uint8                   link_qual_meas_mode;
  mac_rlc_mode_T          rlc_mode;
} mac_downlink_tbf_t;

#endif /* defined (FEATURE_GSM_DTM) || defined (FEATURE_GPRS_PS_HANDOVER) */

#if  defined (FEATURE_GSM_EDTM) || defined (FEATURE_GPRS_PS_HANDOVER)

typedef struct
{
  boolean pkt_ext_ta_present;
  uint8   pkt_ext_ta;
} mac_l1_pkt_ext_ta_t;

#endif /* defined (FEATURE_GSM_EDTM) || defined (FEATURE_GPRS_PS_HANDOVER) */

#ifdef FEATURE_GSM_DTM
/*************************************************************************
* "MAC_L1_DTM_CHANNEL_ASSIGN_REQ" MAC to L1 message structure
*************************************************************************/

typedef struct
{
  IMH_T                           message_header;
  dtm_tbf_type_t                  dtm_tbf_type;
  gprs_l1_cell_opt_t              gprs_l1_cell_opt;
  mac_dynamic_tbf_information_T   ul_tbf;
  mac_downlink_tbf_t              dl_tbf;
  boolean                         cs_present;
  cs_chan_desc_t                  cs_desc;
  gas_id_t                        gas_id;
}mac_l1_dtm_channel_assign_req_t;


#ifdef FEATURE_GSM_EDTM
/*************************************************************************
* "MAC_L1_EDTM_CS_REL_TBF_ALLOC" MAC to L1 message structure
*************************************************************************/

typedef struct
{
  IMH_T                           message_header;
  dtm_tbf_type_t                  tbf_type;
  mac_dynamic_tbf_information_T   ul_tbf;
  mac_downlink_tbf_t              dl_tbf;
  mac_l1_pkt_ext_ta_t             pkt_ext_ta;
  gas_id_t                        gas_id;
}mac_l1_edtm_cs_rel_tbf_alloc_t;

/*************************************************************************
* "MAC_L1_DELETE_UL_CTRL_MSG" MAC to L1 message structure
*************************************************************************/
typedef struct
{
  IMH_T message_header;
  uint8 message_type;
  gas_id_t gas_id;
} mac_l1_delete_ul_ctrl_msg_t;

#endif /*FEATURE_GSM_EDTM*/

#endif /*FEATURE_GSM_DTM*/

/*************************************************************************
* "MAC_PH_RANDOM_ACCESS_REQ " MAC to l1 message structure
*************************************************************************/
typedef struct
{
  IMH_T            message_header;
  boolean          tx_access_burst;
  uint16           random_access_content;
  uint16           random_access_delay;
  uint8            access_burst_type;

  tbf_mode_T   tbf_mode;
  uint8            tsc;

  uint8            prach_ind;             /* can have a value of 0 or 1  */
                                          /* and is used to pair up      */
                                          /* MAC_PH_RANDOM_ACCESS_REQ and*/
                                          /* MAC_PH_RANDOM_ACCESS_CONF   */
  gas_id_t         gas_id;
} mac_ph_random_access_req_t ;

/*************************************************************************
* "MAC_RANDOM_ACCESS_ABORT " MAC to l1 message structure
*************************************************************************/
typedef struct
{
  IMH_T            message_header;
  gas_id_t         gas_id;
} mac_l1_random_access_abort_t ;

/*************************************************************************
* "MAC_PH_DATA_REQ " MAC to l1 message structure
*************************************************************************/

typedef struct
{
  IMH_T message_header;
#ifdef PL1_FW_SIM
  /* Even bytes required for PL1 firmware simulator */
  uint8 dl_ctrl_block[SIZE_OF_CONTROL_RADIO_BLOCK + 1];
#else
  uint8 dl_ctrl_block[SIZE_OF_CONTROL_RADIO_BLOCK];
#endif

#ifdef FEATURE_GSM_EDTM
  uint8 message_type;
#endif /*FEATURE_GSM_EDTM*/
  gas_id_t gas_id;
} mac_ph_data_req_t;

#ifdef FEATURE_GPRS_PS_HANDOVER
typedef enum
{
  PSHO_NO_TBF,
  PSHO_UL_TBF,
  PSHO_UL_DL_TBF
} psho_tbf_type_t ;

typedef enum
{
 PSHO_NON_SYNC,
 PSHO_SYNC,
 PSHO_PRE_SYNC,
 PSHO_NO_SYNC
} psho_sync_type_t;

typedef struct
{
  IMH_T                            message_header;
  psho_type_t                      psho_type;/* This is the type of PSHO handover*/
  psho_sync_type_t                 psho_sync_type;/* This is type of SYNC should
                                                     be used in PSHO*/
  psho_tbf_type_t                  psho_tbf_type; /* This is the tbf type assigned in
                                                     PSHO*/
  CSN_ARFCN_TYPE                   psho_arfcn;/* This is the PSHO ARFCN*/
  uint8                            psho_bsic;/* This is the BSIC of the target cell*/
  uint8                            psho_access_burst_type;/*0 means 8 bit and 1 means 11 bit access
                                                            burst */
  uint8                            normal_cell_indication;
  boolean                          handover_reference_present;
  uint8                            handover_reference;
  mac_dynamic_tbf_information_T    ul_tbf;/* ul tbf resources */
  mac_downlink_tbf_t               dl_tbf;/* dl tbf resources */
  mac_l1_pkt_ext_ta_t              pkt_ext_ta;
  gas_id_t                         gas_id;
}mac_l1_psho_req_t;

typedef struct
{
  IMH_T                                            message_header;
  psho_status_t                                   psho_status;
  gas_id_t       gas_id;
}mac_l1_psho_status_t;

typedef struct
{
  IMH_T                      message_header;
  gas_id_t gas_id;
}mac_l1_psho_cancel_pending_config_t;
#endif /*FEATURE_GPRS_PS_HANDOVER*/

/*************************************************************************
* " Union of all L1 to MAC message structure
*************************************************************************/

typedef union
{

  /*application dependent signals*/
  mac_l1_freq_config_t          mac_l1_freq_config;
  mac_l1_dynamic_tbf_config_t   mac_l1_dynamic_tbf_config;
  mac_l1_fixed_tbf_config_t     mac_l1_fixed_tbf_config;
  mac_l1_single_block_config_t  mac_l1_single_block_config;
  mac_l1_dl_single_block_t      mac_l1_dl_single_block;
  mac_l1_dl_tbf_config_t        mac_l1_dl_tbf_config;
  mac_l1_pacch_rrbp_t           mac_l1_pacch_rrbp;
  mac_l1_cs_reconfig_t          mac_l1_cs_reconfig;
  mac_l1_pwr_ctrl_t             mac_l1_power_control;
  mac_l1_page_mode_t            mac_l1_page_mode;
  mac_l1_pdch_rel_ts_t          mac_l1_pdch_rel_ts;
  mac_l1_dummy_ctrl_block_t     mac_l1_dummy_ctrl_block;
  mac_l1_release_t              mac_l1_release;
  mac_l1_quality_rpt_t          mac_l1_quality_rpt;
  mac_l1_ts_reconfig_t          mac_l1_ts_reconfig;
  mac_l1_contention_res_t       mac_l1_contention_res;
  mac_l1_stop_rlc_tx_t          mac_l1_stop_rlc_tx;


#ifdef FEATURE_GSM_DTM
  mac_l1_dtm_channel_assign_req_t mac_l1_dtm_channel_assign_req;

#ifdef FEATURE_GSM_EDTM
  mac_l1_edtm_cs_rel_tbf_alloc_t  mac_l1_edtm_cs_rel_tbf_alloc;
  mac_l1_delete_ul_ctrl_msg_t     mac_l1_delete_ul_ctrl_msg;
#endif /*FEATURE_GSM_EDTM*/

#endif /*FEATURE_GSM_DTM*/


#ifdef FEATURE_GPRS_PS_HANDOVER
  mac_l1_psho_req_t               mac_l1_psho_req;
  mac_l1_psho_status_t            mac_l1_psho_status;
  mac_l1_psho_cancel_pending_config_t mac_l1_psho_cancel_pending_config;
#endif /*FEATURE_GPRS_PS_HANDOVER*/

  /*GPRS specification primatives */
  mac_ph_random_access_req_t    mac_ph_random_access_req;
  mac_ph_data_req_t             mac_ph_data_req;
  mac_l1_random_access_abort_t  mac_l1_random_access_abort;
} mac_l1_sig_t;

#endif /* INC_GMACSIGL1_H */
/*** EOF: don't remove! ***/
