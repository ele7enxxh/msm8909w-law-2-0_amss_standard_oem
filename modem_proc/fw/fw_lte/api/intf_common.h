/*!
  @file
  intf_common.h

  @brief
  This file contains lte L1 common interface parameter structures

  @detail
  description of functions in this file
 
*/

/*===========================================================================

  Copyright (c) 2008-09 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_lte/api/intf_common.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/31/11   isalman Don't use q6 headers when compiling for UMPLT.

===========================================================================*/
#ifndef INTF_COMMON_H
#define INTF_COMMON_H

#include "lte_l1_types.h"
#include "msgr.h"
// #include "lte_global.h"

/*===========================================================================

      Constants

===========================================================================*/

/*! @brief Constant definitions for cell related information for initial search
*/
#define INIT_ACQ_SRCH_MAX_SSS_PKS_PER_PSS_MAX   5
#define INIT_ACQ_SRCH_MAX_SSS_PKS_PER_PSS_MIN   1
#define MAX_NUM_PSS_INIT_CELLS          8

/*! @brief Definition for max number of detected init cells and max number of HFs
*/
#define MAX_NUM_DETECTED_INIT_CELLS     8
#define SRCH_INIT_ACQ_MAX_NUM_HF        8

/*! @brief Definition for limits for initial acquisition number of detected cells
*/
#define INIT_ACQ_MAX_NUM_DET_CELLS_MAX   MAX_NUM_DETECTED_INIT_CELLS
#define INIT_ACQ_MAX_NUM_DET_CELLS_MIN   1

/*! @brief structure for cell related information for neighbor search
*/
// XXX - this has to be changed to 16
#define MAX_NUM_DETECTED_NEIGHBOR_CELLS 8
#define MAX_NUM_PSS_NCELLS              8
#define MAX_MAX_NUM_PSS_INIT_ACQ_NCELLS  (MAX_NUM_PSS_NCELLS)

/*! @brief Maximum number of LNA gain stages that can be used for TDD
*/
#define TDD_MAX_NUM_LNA_GAIN_STATES  6

/*! @brief Maximum number of black list structures for neighbor search
    FW will keep a copy of these per each channel (there are up to 
    4 different frequency allocations
*/
#define  MAX_NUM_BLACK_LIST_ELEMENTS 32

/*! @brief LIMITS for neighbor search message values
*/
#define NCELL_SRCH_NOT_ABORTED  0
#define NCELL_SRCH_ABORTED      1
#define NCELL_SRCH_NUM_HF_MAX   8
#define NCELL_SRCH_NUM_HF_MIN   1

#define NCELL_SRCH_MAX_SSS_PKS_PER_PSS_MAX   5
#define NCELL_SRCH_MAX_SSS_PKS_PER_PSS_MIN   1

/*! @brief Definition for limits for ncell search number of detected cells
*/
#define NCELL_SRCH_MAX_NUM_DET_CELLS_MAX   MAX_NUM_DETECTED_NEIGHBOR_CELLS
#define NCELL_SRCH_MAX_NUM_DET_CELLS_MIN   1

/*! @brief MAX number of carriers supported in CA 
*/
#define LTE_LL1_DL_NUM_CARRIERS ((uint32)LTE_LL1_CARRIER_COUNT)

/*! @brief MAX number of RX CXM channel ids
*/
#define LTE_MAX_CXM_CH_IDS 2
/*! @brief MAX number of CRS IC cells supported per carrier
*/
#define LTE_MAX_NUM_CRS_IC_CELLS     3

/*! @brief MAX number of LQICE IC cells supported per carrier
*/
#define LTE_MAX_NUM_LQICE_IC_CELLS    2

/*===========================================================================

      Macros

===========================================================================*/

#define LTE_LL1_INTF_MSG( MSG_NAME )                  \
typedef struct {                                      \
  msgr_hdr_struct_type              msg_hdr;          \
  lte_LL1_##MSG_NAME##_struct       msg_payload;      \
} lte_LL1_##MSG_NAME##_msg_struct

/*===========================================================================

      Typedefs

===========================================================================*/
/*! @brief Enum for carriers to be supported for CA 
*/
#if 0
typedef enum
{
  LTE_LL1_CARRIER_PCC= 0,
  LTE_LL1_CARRIER_SCC_0,
  LTE_LL1_CARRIER_SCC_1,
// #ifndef BOLT_PLUS_VERIFY_VPE3
  LTE_LL1_CARRIER_COUNT,
// #else
//   LTE_LL1_CARRIER_COUNT = 2,
// #endif
}lte_LL1_carrier_type_enum;
#endif

typedef uint8 lte_LL1_carrier_type_enum;
#define LTE_LL1_CARRIER_PCC    0
#define LTE_LL1_CARRIER_SCC_0  1
#define LTE_LL1_CARRIER_SCC_1  2
#define LTE_LL1_CARRIER_COUNT  2  // Keeping the carrier count = 2 in JOLOKIA



/*! @brief Enum for NB searches to be supported */
typedef enum
{
   LTE_LL1_NB_ID_0 = 0,
   LTE_LL1_NB_ID_1,
   LTE_LL1_NB_ID_COUNT
}lte_LL1_nb_id_enum;

/*! @brief LTE enum type for neighbor black list index
*/
typedef enum
{
  LTE_NSRCH_BLACK_LIST_INDEX0=0,   ///<neighbor black list index0
  LTE_NSRCH_BLACK_LIST_INDEX1,     ///<neighbor black list index1
  LTE_NSRCH_BLACK_LIST_INDEX2,     ///<neighbor black list index2
  LTE_NSRCH_BLACK_LIST_INDEX3,     ///<neighbor black list index3
  LTE_NSRCH_BLACK_LIST_INDEX_NONE, ///<no neighbor black list
  LTE_NSRCH_NO_BLACK_LIST_INDEX_LAST=0xff     
                                  ///<last enum element to make it uint8
} lte_LL1_nsrch_black_list_index_enum_t;

/*! @brief LTE enum type to Do/Do not run pruning algorithm
    during init cell search and neighbor search
*/
typedef enum
{
  LTE_SRCH_DO_NOT_PRUNE=0,    ///<LTE_SRCH_DO_NOT_PRUNE
  LTE_SRCH_DO_PRUNE,          ///<LTE_SRCH_DO_PRUNE
  LTE_SRCH_PRUNING_LAST=0xff  ///<last enum element to make it uint8
} lte_LL1_srch_pruning_enum_t;

/*! @brief LTE enum type to distinguish between SYNC/ASYNC system
*/
typedef enum
{
  LTE_SRCH_ASYNC_SYS=0,        ///<LTE_SRCH_ASYNC_SYS
  LTE_SRCH_SYNC_SYS,           ///<LTE_SRCH_SYNC_SYS
  LTE_SRCH_SYNC_LAST=0xff      ///<last enum element to make it uint8
} lte_LL1_srch_sys_sync_enum_t;

/*! @brief structure type for black cells for Neighbor search
*/
typedef struct
{
  uint16 black_cell_id;     ///<Black Cell ID
  uint16 range;             ///<Num of consecutive cell ID
} lte_LL1_srch_black_cell_struct_t;

/*! @brief LTE supported bandwidths 
*/
typedef enum
{
  LTE_SYS_BW_1_4_MHZ =0,     ///< 1.4 MHz
  LTE_SYS_BW_3_MHZ   =1,     ///< 3 MHz
  LTE_SYS_BW_5_MHZ   =2,    ///< 5 MHz
  LTE_SYS_BW_10_MHZ  =3,    ///< 10 MHz
  LTE_SYS_BW_15_MHZ  =4,    ///< 15 MHz
  LTE_SYS_BW_20_MHZ  =5,    ///< 20 MHz
  LTE_NUM_BANDWIDTHS
} lte_bandwidth_enum;

typedef enum
{
  LTE_NUM_RB_IDX_6   =0,     ///< 1.4 MHz
  LTE_NUM_RB_IDX_15  =1,     ///< 3 MHz
  LTE_NUM_RB_IDX_25  =2,    ///< 5 MHz
  LTE_NUM_RB_IDX_50  =3,    ///< 10 MHz
  LTE_NUM_RB_IDX_75  =4,    ///< 15 MHz
  LTE_NUM_RB_IDX_100 =5,    ///< 20 MHz
  LTE_NUM_RB_IDX
} lte_num_rb_idx_enum;

typedef enum
{
  LTE_NUM_RB_1_4_MHZ   =6,     ///< 1.4 MHz
  LTE_NUM_RB_3_MHZ     =15,     ///< 3 MHz
  LTE_NUM_RB_5_MHZ     =25,    ///< 5 MHz
  LTE_NUM_RB_10_MHZ    =50,    ///< 10 MHz
  LTE_NUM_RB_15_MHZ    =75,    ///< 15 MHz
  LTE_NUM_RB_20_MHZ    =100,    ///< 20 MHz
  LTE_NUM_RB
} lte_num_rb_enum;

// General Rx ant related enumerations
#if 0
typedef enum
{
  LTE_LL1_UE_RX_ANT_0 = 0,
  LTE_LL1_UE_RX_ANT_1 = 1,
  LTE_LL1_MAX_NUM_UE_RX_ANT,
  LTE_LL1_UE_RX_ANT_INVALID = LTE_LL1_MAX_NUM_UE_RX_ANT,
} lte_LL1_ue_rx_ant_enum;
#endif
typedef int8 lte_LL1_ue_rx_ant_enum;
#define LTE_LL1_UE_RX_ANT_0       0
#define LTE_LL1_UE_RX_ANT_1       1
#define LTE_LL1_MAX_NUM_UE_RX_ANT 2
#define LTE_LL1_UE_RX_ANT_INVALID LTE_LL1_MAX_NUM_UE_RX_ANT


/*! @brief MAX number of Rx antennas supported per carrier
*/
#define LTE_MAX_NUM_UE_RX_ANT           (LTE_LL1_MAX_NUM_UE_RX_ANT)
#define LTE_MAX_NUM_UE_RX_ANT_PER_CAR   (LTE_LL1_MAX_NUM_UE_RX_ANT)

// Macrosleep status related enumerations
typedef enum
{
  LTE_LL1_MACROSLEEP_SUCCESS = 0,
  LTE_LL1_MACROSLEEP_SUCCESS_NB_PENDING = 1,
  LTE_LL1_MACROSLEEP_CANCELLED_PDCCH_MATCH = 2,
  LTE_LL1_MACROSLEEP_CANCELLED_MS_WAKEUP = 3,
  LTE_LL1_MACROSLEEP_CANCELLED_CONFIG_APP = 4,
  LTE_LL1_MACROSLEEP_CANCELLED_RNTI_DISABLED = 5,
  LTE_LL1_MACROSLEEP_CANCELLED_EXCESS_MESSAGE = 6,
  LTE_LL1_MACROSLEEP_CNF_INVALID = 255,
} lte_LL1_macrosleep_status_enum;

typedef enum
{
  LTE_LL1_MACROSLEEP_WAKEUP_SUCCESS = 0,
  LTE_LL1_MACROSLEEP_WAKEUP_ALREADY_AWAKE = 1,
  LTE_LL1_MACROSLEEP_WAKEUP_CANCELLED_EXCESS_MESSAGE = 2,
  LTE_LL1_MACROSLEEP_WAKEUP_CNF_INVALID = 255,
}lte_LL1_macrosleep_wakeup_status_enum;
// ----------------------------------------------------------------------------
//
// common typedefs used in REQ/CNF/IND ML-LL messages
//
// ----------------------------------------------------------------------------
/*! @brief system time struct
*/
typedef union {
  struct {                    
    uint16 subframe_num:4;                  ///< 0 ~9
    uint16 frame_num:12;                    ///< radio frame number
  };
  /* GCC doesn't include symbol info for anonymous structs, so include named
     struct also for debugging. */
  struct {
    uint16 subframe_num:4;                  ///< 0 ~9
    uint16 frame_num:12;                    ///< radio frame number
  } seg;
  int16 value;
} lte_LL1_sys_time_struct;


/*! @brief Missed timing action enum
*/
typedef enum
{
  LTE_LL1_REQ_NO_ACTION_TIME = 0,          ///< no action time specified
  LTE_LL1_REQ_HALT_ON_MISSED_ACT_TIME,     ///< halt if subframe action time of request is missed
  LTE_LL1_REQ_IGNORE_ON_MISSED_ACT_TIME,   ///< ignore request if subframe action time is missed
  LTE_LL1_REQ_IGNORE_MISSED_ACT_TIME,      ///< execute request even if subframe action time is missed  
  LTE_LL1_REQ_HALT_ON_MISSED_FULL_ACT_TIME ///< halt if action time (subframe & frame) of request is missed
} lte_LL1_req_handling_enum;


/*! @brief Request confirmation status enum
*/
typedef enum
{
  LTE_LL1_CONF_REQ_COMPLETED = 0,                   ///< request was completed
  LTE_LL1_CONF_REQ_HALT,                            ///< late action time resulted in halt
  LTE_LL1_CONF_REQ_IGNORED,                         ///< request was ignored
  LTE_LL1_CONF_REQ_COMPLETED_WITH_IGNORED_ACT_TIME  ///< request was completed but
                                                    ///<  its action time was missed  
} lte_LL1_conf_status_enum;



// ----------------------------------------------------------------------------
//
// Common message structures
//
// ----------------------------------------------------------------------------

/*! @brief Request message header (this goes in message body of each request message)
*/
typedef struct {                    
  
  uint8                      seq_id;                       ///< sequence id
  
  lte_LL1_req_handling_enum  action_time_handling;         ///< action time handling
  
  lte_LL1_sys_time_struct    ota_action_time;              ///< OTA action time of timed request 
      
} lte_LL1_req_header_struct;


/*! @brief Confirmation message header (this goes in message body of each confirmation message)
*/
typedef struct {                    
  
  uint8                      seq_id;                       ///< sequence id
  
  lte_LL1_conf_status_enum   status;                       ///< confirmation status
  
} lte_LL1_cnf_header_struct;


/*! @brief Indication message header (this goes in message body of each indication message)
*/
typedef struct {                    
  
  int16  sfn;
  int8   subframe_index;
  
} lte_LL1_ind_header_struct;


/*! @brief Configuration element structure.  Req message can contain an array of 
           these structures for configuration LL1 modules
*/
typedef struct
{
  int16 index;      ///< index for specifying configuration parameter
  int   value;      ///< new value of configuration parameter
} lte_LL1_config_element_struct;


/*! @brief Generic confirmation structure
*/
typedef struct 
{ 
  lte_LL1_cnf_header_struct   cnf_hdr;      ///< Common confirmation parameters  
} lte_LL1_generic_cnf_struct;

LTE_LL1_INTF_MSG( generic_cnf );



// ----------------------------------------------------------------------------
//
// typedefs used in ML-LL message structures and else where in LL1 FW
//
// ----------------------------------------------------------------------------

typedef enum
{
   LTE_DSP_ACK_NAK_SOURCE=0,
   LTE_HW_ACK_NAK_SOURCE=1
} lte_LL_ul_ack_nak_source_enum;

#ifdef FEATURE_MODEM_ANTENNA_SWITCH_DIVERSITY 
// --------------------------------------------------------------------------
//
// Antenna Diversity enum, used in sys ant switch and config app messages
// 
// --------------------------------------------------------------------------
/*! @brief Antenna diversity enum for antenna setting 
*/
typedef enum{
    LTE_LL1_DEFAULT_ANT_CONNECTION=0,  ///< ANT0->PRX, ANT1->DRX
    LTE_LL1_CROSS_ANT_CONNECTION=1,    ///< ANT1->PRX, ANT0->DRX
    LTE_LL1_INVALID_ANT_CONNECTION=2   ///< Invalid connection
}lte_LL1_ant_setting_enum_t;

typedef enum{
    LTE_LL1_ANT_SWITCH_TYPE1=0,        ///< ANT SWITCH TYPE1: swap ant0 and ant1
    LTE_LL1_ANT_SWITCH_TYPE2=1,        ///< ANT SWITCH TYPE2: fix ant2, choose between ant0/1
    LTE_LL1_INVALID_ANT_SWITCH_TYPE=2  ///< Invalid ANT SWITCH TYPE
}lte_LL1_ant_switch_type_enum_t;

#endif

/*! @brief Enum for QTA conflict type


typedef enum{
    LTE_LL1_QTA_CONFLICT_START=0,    ///< CXM conflict start
    LTE_LL1_QTA_CONFLICT_END          ///< CXM conflict end
}lte_LL1_qta_conflict_type_enum_t;
*/


/*! @brief Enum for conflict handling mode
*/

typedef enum {
 LTE_LL1_CONFLICT_HANDLING_BLANKING=0,
 LTE_LL1_CONFLICT_HANDLING_TUNE_AWAY
}lte_LL1_conflict_handling_mode_t;



/*! @brief Struct for DL carrier info for which conflicts are to be checked 
*/

typedef struct{
  uint32 frequency_id;              ///< Frequency ID of the carrrier
  uint8 enable_conflict_handling;      ///< Enable conflict checks
  uint8 disable_conflict_handling;     ///< disable conflict checks
  uint8 rxlm_buf_id_ant0;           ///< RXLM buffer index for antenna 0
  uint8 rxlm_buf_id_ant1;           ///< RXLM buffer index for antenna 1
  uint8 cxm_channel_id[LTE_MAX_CXM_CH_IDS]; ///< channel_id representing the RF path for LTE activity
  uint8 num_cxm_channels;           ///< Number of valid RX CXM channel id's 
  lte_LL1_conflict_handling_mode_t conflict_handling_mode; ///< Conflict handling mode 
}lte_LL1_qta_dl_carrier_info_struct;


/*! @brief Struct for DL carrier info for which conflicts are to be checked 
*/
typedef struct{
  uint32 frequency_id;              ///< Frequency ID of the carrrier
  uint8 enable_conflict_handling;      ///< Enable/disable conflict check
  uint8 disable_conflict_handling;     ///< disable conflict checks
  uint8 cxm_channel_id[LTE_MAX_CXM_CH_IDS]; ///< channel_id representing the RF path for LTE activity
  uint8 num_cxm_channels;           ///< Number of valid RX CXM channel id's 
  lte_LL1_conflict_handling_mode_t conflict_handling_mode; ///< Conflict handling mode 
}lte_LL1_qta_ul_carrier_info_struct;

typedef struct{
  /*! The value for the higher priority */
  uint32 prio_value;

  /*! The value for the freq id */
  uint32 freq_id[LTE_LL1_DL_NUM_CARRIERS];

  /*! num carriers */
  uint8 num_carriers;
}lte_LL1_change_cxm_params_struct;

/*! @brief DL Interferer types
*/ 
typedef enum
{
  LTE_LL1_DL_INTERF_NONE                 = 0,  /*!< No DL interference */
  LTE_LL1_DL_INTERF_DSDA_AGGRESSOR       = 1,  /*!< DSDA interference SF */
  LTE_LL1_DL_INTERF_SLTE_RX_ANT_MODE_CHG = 2,  /*!< SLTE transition SF interference caused due to 2Rx<->1Rx 
                                                    antenna mode change*/
  LTE_LL1_DL_INTERF_NUM_MAX_TYPES,             /*!< Add other interference types above this */
  LTE_LL1_DL_INTERF_INVALID              = 0xFFFFFFFF
}lte_LL1_dl_interf_enum;

#define LTE_LL1_DUAL_DEMOD_NUM_ANT 4
#define LTE_LL1_DUAL_DEMOD_NUM_ANT_CORRELATION 6
typedef enum
{
  LTE_LL1_DUAL_DEMOD_RX_INDEX_0         =0,  
  LTE_LL1_DUAL_DEMOD_RX_INDEX_1         =1,         
  LTE_LL1_DUAL_DEMOD_RX_INDEX_2         =2,         
  LTE_LL1_DUAL_DEMOD_RX_INDEX_3         =3,         
  LTE_LL1_DUAL_DEMOD_MAX_RX_INDEX       =4      
}lte_LL1_dual_demod_rx_index_e;

typedef enum
{
  LTE_LL1_DUAL_DEMOD_CSF_SUM_SPEF =0,
  LTE_LL1_DUAL_DEMOD_CSF_MAX_SPEF =1,
  LTE_LL1_DUAL_DEMOD_CSF_VPE1_SPEF=2
}lte_LL1_dual_demod_csf_type_e;

typedef struct {
  /*revse mapping table based on vpe index*/
  lte_LL1_dual_demod_rx_index_e rx_index[LTE_LL1_DUAL_DEMOD_NUM_ANT];                  
} lte_LL1_dual_demod_map_struct;

#endif /* INTF_COMMON_H */
