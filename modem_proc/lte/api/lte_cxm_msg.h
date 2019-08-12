/*!
  @file   lte_ml1_coex_msg.h

  @brief  Defines the messages used to interface with CxM.

  @detail

  This file describes the interface betwen CxM and ML1

*/
/*===========================================================================

  Copyright (c) 2006 - 2011 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/api/lte_cxm_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/10/12   ns     Add support to specify bandwidth in Mhz
05/31/11   aa     Initial release

===========================================================================*/
#ifndef LTE_ML1_CXM_MSG_H
#define LTE_ML1_CXM_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <msgr_lte.h>
#include <msgr.h>
#include <lte.h>
#include <lte_l1_types.h>
#include <lte_as.h>
#include <lte_app_msg.h>
#include <intf_sys_msg.h>
/*===========================================================================

                      DEFINITIONS

===========================================================================*/
enum
{
 LTE_ML1_CXM_STATE_UPDATE_REQ_ID =0,
 LTE_ML1_CXM_STATE_IND_ID =1,
 LTE_ML1_CXM_ACTIVATION_REQ_ID =2,
 LTE_ML1_CXM_DEACTIVATION_REQ_ID = 3,
 LTE_ML1_CXM_NOTIFY_EVENT_IND_ID =4,
 LTE_ML1_CXM_FRAME_TIMING_IND_ID =5,
 LTE_ML1_CXM_READING_COUNTER_REQ_ID =6,
 LTE_ML1_CXM_READING_COUNTER_CNF_ID =7,
 LTE_ML1_CXM_RC_COMPLETE_IND_ID =8,
 LTE_ML1_CXM_PHR_LESS_BACKOFF_REQ_ID =9,
 LTE_ML1_CXM_PHR_BACKOFF_REQ_ID =10,
 LTE_ML1_CXM_QMI_SMD_RX_IND_ID=11,
 LTE_ML1_CXM_BAND_FILTER_IND_ID=12,
 LTE_ML1_CXM_STATE_INIT_IND_ID=13,
};

extern msgr_umid_type lte_ml1_cxm_umid_list[];

typedef lte_LL1_sys_cxm_boot_params_req_struct lte_ml1_sys_cxm_boot_params_req_struct; 
typedef lte_LL1_sys_cxm_active_req_struct      lte_ml1_sys_cxm_active_params_req_struct; 
typedef lte_LL1_sys_cxm_active_req_msg_struct  lte_ml1_sys_cxm_active_req_msg_struct;
typedef lte_LL1_sys_cxm_active_cnf_msg_struct  lte_ml1_sys_cxm_active_cnf_msg_struct;
typedef lte_LL1_sys_cxm_wcn_threshold_exceeded_ind_struct lte_ml1_sys_cxm_wcn_report_struct;

/*------------------------------------------------------------------------- 
  Co-existense database field  mask
----------------------------------------------------------------------------*/

 #define  LTE_ML1_COEX_ML1_NO_MASK           0x00000000   
 #define  LTE_ML1_COEX_ML1_HP_EVENT          0x00000001 
 #define  LTE_ML1_COEX_ML1_EVENT_START_TIME  0x00000002
 #define  LTE_ML1_COEX_ML1_EVENT_END_TIME    0x00000004
 #define  LTE_ML1_COEX_DL_EARFCN             0x00000008
 #define  LTE_ML1_COEX_UL_EARFCN             0x00000010
 #define  LTE_ML1_COEX_DL_BANDWIDTH          0x00000020
 #define  LTE_ML1_COEX_UL_BANDWIDTH          0x00000040
 #define  LTE_ML1_COEX_FRAME_STRUCT          0x00000080
 #define  LTE_ML1_COEX_TDD_CONFIG            0x00000100
 #define  LTE_ML1_COEX_TDD_SSP               0x00000200
 #define  LTE_ML1_COEX_TDD_DL_CP             0x00000400
 #define  LTE_ML1_COEX_TDD_UL_CP             0x00000800
 #define  LTE_ML1_COEX_ML1_INT_EVT_RPT       0x00001000               
 #define  LTE_ML1_COEX_FRAME_OFFSET          0x00002000
 #define  LTE_ML1_COEX_OFF_DURATION          0x00004000
 #define  LTE_ML1_COEX_CC_ID                 0x00008000
 #define  LTE_ML1_COEX_MASK_NUM              0x00010000

/*------------------------------------------------------------------------- 
  Co-existense defines
----------------------------------------------------------------------------*/
 #define LTE_ML1_COEX_FILTER_MAX              32

/*===========================================================================

                      TYPEDEF USED IN THE CXM INTERFACE

===========================================================================*/


/****************************************************/
/*! @brief structure def.  to request state info from ML1*/
/***************************************************/

typedef struct
{
  /*!MSG router header */
  msgr_hdr_struct_type                msg_hdr;

}lte_ml1_cxm_state_update_req_s;


/*****************************************************/
/*! Structures used by Coex. module to report state info to CxM*/
/*****************************************************/

typedef struct
{

  /*!Connected mode flag */
  boolean                                 connected_mode;

  /*!High priority flag */
  boolean                                 high_priority;

  /*!Co-existence info validity mask for the fields below*/
  uint16                                  mask;

  /*!Event Start time*/
  uint32                                  start_time;

  /*!Event End time*/
  uint32                                  end_time;

  /*!DL EARFCN */
  uint32                                  dl_earfcn;

  /*!UL EARFCN */
  uint32                                  ul_earfcn;

  /*!DL Bandwidth */
  lte_bandwidth_idx_e                     dl_bandwidth;

  /*!UL Bandwidth */
  lte_bandwidth_idx_e                     ul_bandwidth;

  /*!DL Bandwidth in Mhz */
  uint32                                  dl_bandwidth_mhz;

  /*!UL Bandwidth in Mhz */
  uint32                                  ul_bandwidth_mhz;

  /*!Frame Structure FDD or TDD as per 36.201 Section 4.2.1*/
  lte_l1_frame_struct_e                   frame_structure;

  /*!TDD configuration as per 36.211, table 4.2.2, valid only for TDD frame structure */
  lte_l1_tdd_ul_dl_cfg_index_e            tdd_config;

  /*Special subframe pattern*/
  lte_l1_tdd_special_subframe_pattern_e   ssp;

  /*DL CP*/
  lte_l1_cp_e                             dl_cp;

  /*UL CP*/
  lte_cphy_ul_cyclic_prefix_length_e      ul_cp;

  /*DL freq in MHz*/
   uint16                                 dl_freq;
      
  /*UL freq in MHz*/
   uint16                                 ul_freq;

  /* frame offset in us */
   uint16                                 frame_offset;

  /*off duration in ms*/
   uint32                                 off_duration;

}lte_ml1_cxm_ext_info_s;

typedef struct
{

    /*!MSG router header */
  msgr_hdr_struct_type                      msg_hdr;

    /*!ML1 info */
  lte_ml1_cxm_ext_info_s                    ml1_ext_state;

} lte_ml1_cxm_state_ind_s;

/*********************************************************************/
/* Structure for requesting Co-existence protocol activation*/
/*********************************************************************/

typedef struct
{
  /*!MSG router header */
  msgr_hdr_struct_type                      msg_hdr;

  /*!Data plane messaging enable */
  boolean                                   data_plane_enable;

} lte_ml1_cxm_activation_req_s;


/*********************************************************************/
/*Structure used by Coex. module to de-activate coexistence*/
/*********************************************************************/

typedef struct
{
  /*!MSG router header */
  msgr_hdr_struct_type                      msg_hdr;

} lte_ml1_cxm_deactivation_req_s;

/***********************************************************************/
/* Structures used by ML1 to notify CxM  about the upcoming Tx*/
/**********************************************************************/

typedef enum
{
  LTE_ML1_COEX_TX_PRIORITY_0 =0,
  LTE_ML1_COEX_TX_PRIORITY_1 =1,
  LTE_ML1_COEX_TX_PRIORITY_2 =2,
  LTE_ML1_COEX_TX_PRIORITY_3 =3,
  LTE_ML1_COEX_TX_PRIORITY_4 =4,
  LTE_ML1_COEX_TX_PRIORITY_5 =5,
  LTE_ML1_COEX_TX_PRIORITY_6 =6,
  LTE_ML1_COEX_TX_PRIORITY_7 =7,
  LTE_ML1_COEX_NUM_PRIORITY =8
}lte_ml1_coex_tx_priority_e;


typedef enum
{
  LTE_ML1_COEX_RESERVED =0x0,
  LTE_ML1_COEX_PUCCH =0x1,
  LTE_ML1_COEX_PUSCH =0x2,
  LTE_ML1_COEX_PRACH =0x3,
  LTE_ML1_COEX_SRS=0x4,
  LTE_ML1_COEX_SRS_PUCCH =0x5,
  LTE_ML1_COEX_SRS_PUSCH =0x6,
  LTE_ML1_COEX_NUM_CH_TYPE =0x7
}lte_ml1_coex_channel_e;

typedef int8 lte_ml1_coex_tx_pwr_type;

typedef struct
{

 /*!Index of first RB */
 uint8 first_rb;

 /*!Index of last RB */
 uint8 last_rb;
}lte_ml1_coex_rb_alloc_s;

typedef struct
 {

   /*!SNR validity*/
   boolean                             valid;

   /*! Sub frame number*/
   uint8                               subframe;

   /*!  SNR in 16S8 format  */
   int32                               snr;
 }lte_ml1_coex_snr_info_s;


typedef struct
{
  /*!Subframe number (0-9) */
  lte_l1_subfn_t                        sub_frame;

  /*!If Tx scheduled in subframe */
  boolean                               tx_flag;

  /*!Channel type*/
  lte_ml1_coex_channel_e                channel_type;

  /*!Tx Priority */
  lte_ml1_coex_tx_priority_e            priority;

  /*!Tx Power in slot 1 */
  int8                                  slot0_power;

  /*!PRB allocation in slot 1 */
  lte_ml1_coex_rb_alloc_s               slot0_rb;

  /*!Tx Power in slot 2 */
  int8                                  slot1_power;

  /*!PRB allocation in slot 2 */
  lte_ml1_coex_rb_alloc_s               slot1_rb;

  /*!SRS RB */
  lte_ml1_coex_rb_alloc_s               srs_rb;

  /*!SRS power */
  int8                                  srs_power;

}lte_ml1_coex_tx_event_s;


typedef struct
{

  /*!MSG router header */
  msgr_hdr_struct_type                  msg_hdr;

  /*!Tx Event info*/
  lte_ml1_coex_tx_event_s               tx_event_info;

  /*!SNR info*/
  lte_ml1_coex_snr_info_s               snr_info;

} lte_ml1_cxm_notify_event_ind_s;

/***************************************************************************/
/*Structures/enums/defines  used by ML1 to notify CxM  about the UL/DL frame timing*/
/***************************************************************************/

/*! Max. number of pending frame reports  */
#define    LTE_ML1_NUM_FRAME_REPORTS    6

typedef enum
{
  /*Periodic Report*/
  LTE_ML1_COEX_PERIODIC_REPORT =0,
    /*HO report*/
  LTE_ML1_COEX_HO_REPORT=1,
    /*TA report*/
  LTE_ML1_COEX_TA_REPORT=2,
 /*Total no. of tasks*/
  LTE_ML1_COEX_NUM_REPORTS=3
}lte_ml1_coex_report_type_e;

typedef uint32 lte_ml1_coex_frame_time_type;


typedef struct
{
  /*!DL subframe number*/
  lte_l1_subfn_t                        dl_sub_frame;

  /*!DL frame timing in terms of UNIV STMR*/
  lte_ml1_coex_frame_time_type          dl_frame_time;

  /*!UL subframe number*/
  lte_l1_subfn_t                        ul_sub_frame;

  /*!UL frame timing in terms of UNIV STMR*/
  lte_ml1_coex_frame_time_type          ul_frame_time;

}lte_ml1_coex_ustmr_s;



typedef struct
{

  /*!Report Type*/
  lte_ml1_coex_report_type_e           report_type;

  lte_ml1_coex_ustmr_s                 timing;

} lte_ml1_coex_frame_timing_s;


typedef struct
{
  /*!MSG router header */
  msgr_hdr_struct_type                 msg_hdr;

  /*!No. of reports*/
  uint8                                num_of_reports;

 /*!Reports*/
 lte_ml1_coex_frame_timing_s           report[LTE_ML1_NUM_FRAME_REPORTS];

} lte_ml1_cxm_frame_timing_ind_s;

/***************************************************************************/
/* Structure for requesting UNIV. STMR read service*/
/***************************************************************************/

typedef struct
{
  /*!MSG router header */
  msgr_hdr_struct_type                 msg_hdr;

} lte_ml1_cxm_reading_count_req_s;

/***************************************************************************/
/* Structure for confirming UNIV. STMR read service*/
/***************************************************************************/
typedef struct
{
  /*!MSG router header */
  msgr_hdr_struct_type                 msg_hdr;

} lte_ml1_cxm_reading_count_cnf_s;
/***************************************************************************/
/*Structure for indicating completion of  UNIV. STMR read service*/
/***************************************************************************/

typedef struct
{
  /*!MSG router header */
  msgr_hdr_struct_type                  msg_hdr;

} lte_ml1_cxm_rc_complete_ind_s;

/***************************************************************************/
/*Structure for message from CxM requesting PHR less backoff */
/***************************************************************************/

typedef struct
{
  /*!MSG router header */
  msgr_hdr_struct_type                  msg_hdr;

  /*!Backoff max. transmit power limit */
  int8                                  backoff_mtpl;

  /*!Threshold at/below which  backoff will be applied*/
  lte_ml1_coex_tx_priority_e            priority_threshold;

  /*!Time in terms of Univ. STMR from which the backoff will be applied*/
  uint32                                starting_time;

  /*!No. of subframes from starting time for which this request is valid*/
  uint8                                 num_of_subframes;

} lte_ml1_cxm_phr_less_backoff_req_s;

/***************************************************************************/
/* Structure for message from CxM requesting PHR backoff */
/***************************************************************************/

typedef struct
{
  /*!MSG router header */
  msgr_hdr_struct_type                  msg_hdr;

  /*!Backoff max. transmit power limit */
  int8                                  backoff_mtpl;

  /*!Threshold at/below which  backoff will be applied*/
  lte_ml1_coex_tx_priority_e            priority_threshold;

} lte_ml1_cxm_phr_backoff_req_s;

/***************************************************************************/
/* Structure for message from CxM requesting PHR backoff */
/***************************************************************************/

typedef struct
{
  /*!MSG router header */
  msgr_hdr_struct_type                  msg_hdr;

} lte_ml1_cxm_qmi_smd_rx_ind_s;

/***************************************************************************/
/* Structure to indicate that Ml1 has entered init state */
/***************************************************************************/
typedef struct
{
  /*!MSG router header */
  msgr_hdr_struct_type                 msg_hdr;

  boolean                              initialized;

} lte_ml1_cxm_state_init_ind_s;

/***************************************************************************/
/* Structure/typedefs for message from CxM indicating params for coex. */
/***************************************************************************/
typedef enum
{
  LTE_ML1_COEX_PROTOCOL_VERSION_1 =1,/*On chip legacy for ROW*/
  LTE_ML1_COEX_PROTOCOL_VERSION_2 =2,/*Off chip legacy for ROW*/
  LTE_ML1_COEX_PROTOCOL_VERSION_3 =3, 
  LTE_ML1_COEX_PROTOCOL_VERSION_4 =4, 
  LTE_ML1_COEX_PROTOCOL_VERSION_5 =5,
  LTE_ML1_COEX_NUM_PROTOCOLS=5
}lte_ml1_coex_protocol_e;

/*********************************************************************/
/* Structure to store band info */
/*********************************************************************/
typedef struct
{
  /*!   Center frequency in MHz */
  uint32                                    freq;
  
  /*!   Bandwidth in MHz */
  uint32                                    bandwidth;
}lte_ml1_cxm_band_type_s;


/*********************************************************************/
/* Structure to store UL/DL channel info */
/*********************************************************************/
typedef struct
{
  /*! UL Band Info */
  lte_ml1_cxm_band_type_s                   ul_band;

  /*! DL Band Info */
  lte_ml1_cxm_band_type_s                   dl_band;

}lte_ml1_cxm_channel_type_s;

/*********************************************************************/
/* Structure to store  channel info */
/*********************************************************************/
typedef struct
{
  /*! UL Band Info */
  lte_ml1_cxm_channel_type_s                band_info;

  /*! filter UL/DL */
  uint64                                    band_mask; 

}lte_ml1_cxm_chan_info_type_s;

/*********************************************************************/
/* Message from CxM to ML1 to indicate bands to filter */
/*********************************************************************/

typedef struct
{
  /*! MSG router header */
  msgr_hdr_struct_type                      msg_hdr;

  /*! Must be set to # of elements in bands */
  uint32                                    bands_len; 

  /*! channel Info */
  lte_ml1_cxm_chan_info_type_s              band_info[LTE_ML1_COEX_FILTER_MAX];  
}
lte_ml1_cxm_band_filter_ind_s;

/*=========================================================================*/
/*                                          UMIDS                          */
/*=========================================================================*/

enum
{
  MSGR_DEFINE_UMID( LTE, ML1_CXM, REQ, STATE_UPDATE , LTE_ML1_CXM_STATE_UPDATE_REQ_ID, lte_ml1_cxm_state_update_req_s),
  MSGR_DEFINE_UMID( LTE, ML1_CXM, IND, STATE , LTE_ML1_CXM_STATE_IND_ID, lte_ml1_cxm_state_ind_s),
  MSGR_DEFINE_UMID( LTE, ML1_CXM, REQ, ACTIVATION, LTE_ML1_CXM_ACTIVATION_REQ_ID, lte_ml1_cxm_activation_req_s),
  MSGR_DEFINE_UMID( LTE, ML1_CXM, REQ, DEACTIVATION , LTE_ML1_CXM_DEACTIVATION_REQ_ID, lte_ml1_cxm_deactivation_req_s),
  MSGR_DEFINE_UMID( LTE, ML1_CXM, IND, NOTIFY_EVENT , LTE_ML1_CXM_NOTIFY_EVENT_IND_ID, lte_ml1_cxm_notify_event_ind_s),
  MSGR_DEFINE_UMID( LTE, ML1_CXM, IND, FRAME_TIMING , LTE_ML1_CXM_FRAME_TIMING_IND_ID, lte_ml1_cxm_frame_timing_ind_s),
  MSGR_DEFINE_UMID( LTE, ML1_CXM, REQ, READING_COUNTER, LTE_ML1_CXM_READING_COUNTER_REQ_ID, lte_ml1_cxm_reading_count_req_s),
  MSGR_DEFINE_UMID( LTE, ML1_CXM, CNF, READING_COUNTER ,LTE_ML1_CXM_READING_COUNTER_CNF_ID, lte_ml1_cxm_reading_count_cnf_s),
  MSGR_DEFINE_UMID( LTE, ML1_CXM, IND, RC_COMPLETE , LTE_ML1_CXM_RC_COMPLETE_IND_ID, lte_ml1_cxm_rc_complete_ind_s),
  MSGR_DEFINE_UMID( LTE, ML1_CXM, REQ, PHR_LESS_BACKOFF , LTE_ML1_CXM_PHR_LESS_BACKOFF_REQ_ID,  lte_ml1_cxm_phr_less_backoff_req_s),
  MSGR_DEFINE_UMID( LTE, ML1_CXM, REQ, PHR_BACKOFF ,  LTE_ML1_CXM_PHR_BACKOFF_REQ_ID,  lte_ml1_cxm_phr_backoff_req_s),
  MSGR_DEFINE_UMID( LTE, ML1_CXM, IND, QMI_SMD_RX ,  LTE_ML1_CXM_QMI_SMD_RX_IND_ID,  lte_ml1_cxm_qmi_smd_rx_ind_s),
  MSGR_DEFINE_UMID( LTE, ML1_CXM, IND, BAND_FILTER, LTE_ML1_CXM_BAND_FILTER_IND_ID, lte_ml1_cxm_band_filter_ind_s),
  MSGR_DEFINE_UMID( LTE, ML1_CXM, IND, STATE_INIT ,  LTE_ML1_CXM_STATE_INIT_IND_ID,  lte_ml1_cxm_state_init_ind_s),
};

enum
{
  MSGR_DEFINE_UMID(LTE, ML1_CXM, SPR, LOOPBACK, MSGR_ID_LOOPBACK, msgr_spr_loopback_s),
  MSGR_DEFINE_UMID(LTE, ML1_CXM, SPR, LOOPBACK_REPLY, MSGR_ID_LOOPBACK_REPLY,msgr_spr_loopback_reply_s),
  MSGR_DEFINE_UMID(LTE, ML1_CXM, SPR, THREAD_READY, LTE_APP_ID_THREAD_READY, none),
  MSGR_DEFINE_UMID(LTE, ML1_CXM, SPR, THREAD_KILL, LTE_APP_ID_THREAD_KILL,none)
};

#endif /* LTE_ML1_CXM_MSG_H */
