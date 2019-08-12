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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/gmacsigl1.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 11-06-06   tjw     Split API content from original file

*****************************************************************************/
#ifndef INC_GMACSIGL1_H
#define INC_GMACSIGL1_H

#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

/* Forward declaration of struct/union required in header files */
struct mac_l1_freq_config_tag;
struct mac_l1_dynamic_tbf_config_tag;
struct mac_l1_dl_tbf_config_tag;

#include "sys_cnst.h"
#include "rr_l1.h"
#include "gprsdef.h"
#include "geran_dual_sim.h"

/* defined the maximum length of the fixed allocation bitmap
 */
#define MAX_BITMAP_LEN     16

/* defines the different MAC_MODE settings */
#define MAC_L1_MAC_MODE_DYNAMIC               0
#define MAC_L1_MAC_MODE_EXTENDED_DYNAMIC      1  /* not supported */
#define MAC_L1_MAC_MODE_FIXED_NOT_HALF_DUPLEX 2
#define MAC_L1_MAC_MODE_FIXED_HALF_DUPLEX     3  /* not supported */

/* Defines enum to define the RLC mode of operation */

typedef enum
{
  RLC_ACK,
  RLC_UNACK,
  RLC_MODE_INVALID
} mac_rlc_mode_T;

/* Defines structure for storing BEP_PERIOD2 */

typedef struct
{
  boolean   present;
  uint8     bep_period2;
} bep_period2_T;

typedef struct
{
  boolean   present;
  uint8   gamma;
} power_control_gamma_T;

typedef struct
{
  boolean               present;
  uint8                 alpha;
  power_control_gamma_T gamma_TN0;
  power_control_gamma_T gamma_TN1;
  power_control_gamma_T gamma_TN2;
  power_control_gamma_T gamma_TN3;
  power_control_gamma_T gamma_TN4;
  power_control_gamma_T gamma_TN5;
  power_control_gamma_T gamma_TN6;
  power_control_gamma_T gamma_TN7;
} power_control_params_T;

typedef struct
{
  boolean   present;
  uint8     ta_value;
} timing_advance_value_T;
/* defines the timing advance index and the timming
 * advance timeslot. if present is not set then tai
 * and ta_timeslot is ignored.
 */
typedef struct
{
  boolean   present;
  uint8     tai;
  uint8     ta_timeslot;
} cont_ta_information_T;

typedef struct
{
  timing_advance_value_T  tav;
  cont_ta_information_T   uplink_tai;
  cont_ta_information_T   downlink_tai;
} global_packet_ta_T;

/* Defines the packet timing adavance parameters
 * defined in GSM 04.60 section 12.12
 */
typedef struct
{
  timing_advance_value_T    ta_value_ie;
  cont_ta_information_T   cont_ta_ie;
} timing_advance_params_T;




typedef struct
{
  boolean params_valid;
  uint8   po;
  uint8   bts_pwr_ctrl_mode;
  uint8   pr_mode;

} downlink_pwr_t;

typedef struct
{
  boolean params_valid;
  uint8   po;
  uint8   pr_mode;
}dl_ctrl_param_t;

/*
 * downlink control timeslot used for uplink TBF
 */
typedef struct
{
  boolean control_tn_present;
  uint8   control_tn;
} mac_fixed_control_tn_T;

/*
 * enum for fixed allocation bitmap encoding type
 */
typedef enum
{
  BLOCKS,
  BLOCK_PERIODS
} blocks_blockperiods_T;

/*  Defines paramaters to decribe the fixed allocation
 *  bitmap
 */
typedef struct
{
  boolean                 present;
  blocks_blockperiods_T   blocks_or_block_periods;
  uint8                   allocation_bitmap[MAX_BITMAP_LEN];
  uint8                   allocation_length;
} mac_fixed_allocation_bitmap_T;

/* defines the usf parameters used in the Dynamic
 * allocation description
 */
typedef struct
{
  boolean   present;
  uint8     usf_value;

} usf_value_T;


/* Defines parameters to describe the Dynamic allocation
 * description
 */
typedef struct
{
  /*
   * mapping = element 0 = usf value for TN0
   *           element 7 = usf value for TN7
   */
  usf_value_T             usf[8];
  power_control_params_T  power_control_params;
  timing_advance_params_T timing_advance_params;
  boolean                 usf_granularity;
  boolean                 tbf_start_time_valid;
  uint32                  starting_time;
  dl_ctrl_param_t         dl_ctrl_param;

//ifdef F EATURE_GSM_EGPRS
  bep_period2_T           bep_period2;
  tbf_mode_T              tbf_mode;
  mac_rlc_mode_T          rlc_mode;
  boolean                 srb_mode;
//endif
  test_mode_T             test_mode;
  uint8                   ts_offset;
//ifdef F EATURE_GSM_DTM
  uint8                   mac_mode;
//endif

} mac_dynamic_tbf_information_T;

/*****************************************************************************
***
***     Signal Type Definitions
***
*****************************************************************************/
typedef enum
{

  MAC_L1_FREQ_CONFIG          = 0x80,
  MAC_L1_DYNAMIC_TBF_CONFIG   = 0x81,
  MAC_L1_FIXED_TBF_CONFIG     = 0x82,
  MAC_L1_SINGLE_BLOCK_CONFIG  = 0x83,
  MAC_L1_DL_SINGLE_BLOCK      = 0x84,
  MAC_L1_DL_TBF_CONFIG        = 0x85,
  MAC_L1_PACCH_RRBP           = 0x86,
  MAC_L1_CS_RECONFIG          = 0x87,
  MAC_L1_POWER_CONTROL        = 0x88,
  MAC_L1_PAGE_MODE            = 0x89,
  MAC_L1_PDCH_REL_TS          = 0x8a,
  MAC_L1_DUMMY_CTRL_BLOCK     = 0x8b,
  MAC_L1_RELEASE              = 0x8c,
  MAC_L1_QUALITY_RPT          = 0x8d,
  MAC_L1_TS_RECONFIG          = 0x8e,
  MAC_L1_CONTENTION_RES       = 0x8f,
  MAC_L1_STOP_RLC_TX          = 0x90,
  MAC_L1_DTM_CHANNEL_ASSIGN_REQ = 0x91,
  MAC_L1_EDTM_CS_REL_TBF_ALLOC = 0x92,
  MAC_L1_DELETE_UL_CTRL_MSG    = 0x93,
  MAC_L1_PSHO_REQ              = 0x94,
  MAC_L1_PSHO_STATUS           = 0x95,
  MAC_L1_PSHO_CANCEL_PENDING_CONFIG = 0x96,
/**************************************************************************
* L1 to MAC GPRS spec dependent messages  (GSM 4.04)
**************************************************************************/

  MAC_PH_RANDOM_ACCESS_REQ    = 0xc0,
  MAC_PH_DATA_REQ             = 0xc1,
  MAC_L1_RANDOM_ACCESS_ABORT  = 0xc2

} mac_l1_sig_id_t;

/*************************************************************************
* "MAC_L1_FREQ_CONFIG" MAC to l1 message structure
*************************************************************************/


typedef enum
{
 UL_FREQ_CONFIG,
 DL_FREQ_CONFIG,
 UL_DL_FREQ_CONFIG
} tbf_freq_config_t;


typedef struct mac_l1_freq_config_tag
{
  IMH_T                     message_header;
  frequency_information_T   frequency_information;
  boolean                   reuse_sb_frq_alloc;
  tbf_freq_config_t         tbf_freq_cnf;
//ifdef F EATURE_GSM_DTM
  boolean                   dtm_freq_param_present;
//endif
  gas_id_t                  gas_id;
} mac_l1_freq_config_t;

/*************************************************************************
* "MAC_L1_DYNAMIC_TBF_CONFIG" MAC to l1 message structure
*************************************************************************/
typedef struct mac_l1_dynamic_tbf_config_tag
{
  IMH_T                           message_header;
  mac_dynamic_tbf_information_T   dynamic_tbf;
  gas_id_t                        gas_id;
} mac_l1_dynamic_tbf_config_t;


/*************************************************************************
* " MAC_L1_DL_TBF_CONFIG" MAC to l1 message structure
*************************************************************************/
typedef struct mac_l1_dl_tbf_config_tag
{
  IMH_T                   message_header;
  uint8                   mac_mode;
  uint8                   ts_allocation;
  downlink_pwr_t          downlink_pwr;
  boolean                 tbf_start_time_valid;
  uint32                  tbf_start_time;
  timing_advance_params_T ta_params;
  power_control_params_T  power_control;
  boolean                 dl_tfi_present;
  uint8                   dl_tfi;

//ifdef F EATURE_GSM_EGPRS
  bep_period2_T           bep_period2;
  tbf_mode_T          tbf_mode;
  uint8                   link_qual_meas_mode;
  mac_rlc_mode_T          rlc_mode;
//endif
  gas_id_t                gas_id;
} mac_l1_dl_tbf_config_t;

/*************************************************************************
* "MAC_L1_RELEASE " MAC to l1 message structure
*************************************************************************/

typedef enum
{
UL_TBF_NORMAL,
DL_TBF_NORMAL,
UL_AND_DL_TBF_NORMAL,
UL_TBF_ABNORMAL,
DL_TBF_ABNORMAL,
UL_AND_DL_TBF_ABNORMAL
} release_tbf_t;


typedef struct
{
  IMH_T           message_header;
  release_tbf_t   release_tbf;
  gas_id_t        gas_id;
} mac_l1_release_t;

/*************************************************************************
* "MAC_L1_TS_RECONFIG " MAC to l1 message structure
*************************************************************************/
typedef struct
{
  IMH_T message_header;
  uint8                         uplink_tbf_type;
  global_packet_ta_T            global_packet_ta;

  /* uplink TBF data (for fixed and dynamic) */
  power_control_params_T        power_control_params;
  uint8                         starting_time_present;
  uint32                        starting_time;

  /* fixed uplink TBF data */
  boolean                       uplink_ts_alloc_present;
  uint8                         uplink_ts_alloc;
  mac_fixed_control_tn_T        dl_control_ts;
  downlink_pwr_t                dl_pwr_params;
  mac_fixed_allocation_bitmap_T mac_fixed_allocation_bitmap;

  /* dynamic uplink TBF data */
  dl_ctrl_param_t               dl_ctrl_param;
  boolean                       usf_granularity;
  usf_value_T                   usf[8];


  /* downlink TBF data */
  uint8                         dl_tfi_present;
  uint8                         dl_tfi;
  uint8                         dl_ts_allocation;

//ifdef F EATURE_GSM_EGPRS
  tbf_mode_T                tbf_mode;
  uint8                         link_qual_meas_mode;
  mac_rlc_mode_T                rlc_mode;
//endif
  gas_id_t                      gas_id;
} mac_l1_ts_reconfig_t;

#endif
