/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2011 by Qualcomm Technologies INCORPORATED.  
  Copyright (c) 2012 by Qualcomm Technologies INCORPORATED.  
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
 
                           EDIT HISTORY FOR MODULE
                           
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/tdscdma/ftm_tdscdma_ber.h#1 $ 
  $DateTime: 2016/12/13 07:59:52 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/30/14   qzh     Enable FTM NS new intf and remove the dependency
11/20/13   qzh     Featurized FTM NS new interface to disable it on BO 0.1 
07/06/13   qzh     Update Non-signaling R4&HSUPA code with new TL1-RFSW interface and function partition
03/01/13   qzh     Add command FTM_TDSCDMA_BER_TPC_CONTROL_REQ to support TPC override feature
12/06/12   ms      HSUPA support (Nikel merge) 
07/16/12   hy/sk Fixed DCH ReConfig (HHO) failure
                           Changed state machine to force Stop TDS after drop all                           
                           Implement RxLevel Request command
03/22/12   hy      NS SE BER for PN9 and PN15 data pattern(PN15 requires invert PN stream)
03/21/12   hy      NS API clean-up and redesign.
03/16/12   hy/sk   NS BER/TxLoopback/HHO/Release
02/16/12   hy      Added support for TDSCDMA Non-signaling
05-19-11   hy      Created
===========================================================================*/
#ifndef FTM_TDSCDMA_BER_H
#define FTM_TDSCDMA_BER_H

/*===========================================================================

          INCLUDE FILES FOR MODULE

===========================================================================*/
#ifndef FEATURE_RF_TL1_FTM_NEW_INTF
#define FEATURE_RF_TL1_FTM_NEW_INTF
#endif

#include "target.h"
#include "customer.h"
#include "ftm.h"
#include "ftmdiag.h"
#include "ftm_log.h"
#include "queue.h"

#ifdef FEATURE_RF_TL1_FTM_NEW_INTF
#include "tds_l1_api.h"
#else /*FEATURE_RF_TL1_FTM_NEW_INTF*/
#include "tdsl1rrcif.h"
#include "tdsrrccmd.h"
#include "tdsrrccmd_v.h"

#include "tdseulsg.h"
#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/

/*===========================================================================
 
                         EXTERNAL GLOBAL VARIABLES

===========================================================================*/

/*===========================================================================
 
                    LOCAL GLOBAL VARIABLES AND CONSTANTS

===========================================================================*/
/* FTM TDSCDMA BER feature only uses RMC channels, with 2 logical channels
 * and 4 distict TFCI values 
 */

#define FTM_TDSCDMA_BER_MAX_NUM_LOGICAL_CHANNELS     2
#define FTM_TDSCDMA_BER_NUM_TRANSPORT_FORMAT_COMBINATIONS 4    

/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/

//copied from ftm_umts_ber.h so that we don't need to include that file.
typedef enum
{
    FTM_TDSCDMA_BER_STATE_NULL = 0,//inactive
    FTM_TDSCDMA_BER_STATE_IDLE = 1, //state after start TDS
    FTM_TDSCDMA_BER_STATE_ACQ = 2,//state after acq
    FTM_TDSCDMA_BER_STATE_DCH_PENDING = 3,//state before DCH
    FTM_TDSCDMA_BER_STATE_DCH = 4,//DCH
    FTM_TDSCDMA_BER_STATE_DCH_RECONFIG_PENDING = 5,//state before DCH reconfig
    FTM_TDSCDMA_BER_STATE_ERROR_OCCURRED_IDLE_PENDING = 6,//error happend, must call stop TDS to exist this state.
    FTM_TDSCDMA_BER_STATE_DROPALL_PENDING = 7, //state before drop all   
    FTM_TDSCDMA_BER_STATE_DROPALL_COMPLETE = 8,//must call stop TDS to exit this state.
    FTM_TDSCDMA_BER_STATE_MAX

} ftm_tdscdma_ber_state_type;

#ifndef FEATURE_RF_TL1_FTM_NEW_INTF
ftm_tdscdma_ber_state_type ftm_tdscdma_ber_get_state( void);
#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/

#ifdef FEATURE_RF_TL1_FTM_NEW_INTF
typedef struct
{

    uint16                           current_rf_chan;
    boolean                          current_rf_chan_active;
} ftm_tdscdma_ber_dl_db_type;
#else /*FEATURE_RF_TL1_FTM_NEW_INTF*/
typedef struct
{
    tdsl1dlcctrchcfg_cctrch_info_struct_type    *dl_cctrch_info;
    tdsl1_dl_cctrchcfg_ctfc_info_struct_type    *dl_ctfc_info;
    tdsl1_dl_phycfg_phychan_db_struct_type      *dl_phychan_db;    
    uint16                           current_rf_chan;
    boolean                          current_rf_chan_active;
    boolean                          isInitialized;

} ftm_tdscdma_ber_dl_db_type;
#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/


#ifndef FEATURE_RF_TL1_FTM_NEW_INTF
typedef struct
{
    tdsl1_ul_cctrch_cfg_struct_type  *ul_cctrch_info;
    tdsl1_ul_phych_cfg_struct_type   *ul_phychan_db;

    tdsmac_ul_tfcs_type              *tfcs_ptr;
    boolean                           isInitialized;
} ftm_tdscdma_ber_ul_db_type;


#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/
/* Interface to PC */

/* Command codes for the FTM TDSCDMA BER feature */

enum
{
    FTM_TDSCDMA_BER_START_TDSCDMA_MODE_REQ         = 0,
    FTM_TDSCDMA_BER_STOP_TDSCDMA_MODE_REQ          = 1,
    FTM_TDSCDMA_BER_IDLE_REQ                       = 2,
    FTM_TDSCDMA_BER_ACQUIRE_REQ                    = 3,
    FTM_TDSCDMA_BER_RMC_DCH_SETUP_REQ              = 4,
    FTM_TDSCDMA_BER_DROP_ALL_REQ                   = 5,/*note: diff from wcdma*/
    FTM_TDSCDMA_BER_RMC_DCH_TFCS_CONFIG_REQ        = 6,
    FTM_TDSCDMA_BER_EVENT_STATUS_CLEAR_REQ         = 7,
    FTM_TDSCDMA_BER_EVENT_STATUS_REQ               = 8,
    FTM_TDSCDMA_BER_SET_UL_POWER_REQ               = 9,
    FTM_TDSCDMA_BER_RX_LEVEL_REQ                   = 10,
    FTM_TDSCDMA_BER_TPC_CONTROL_REQ                = 11,
    FTM_TDSCDMA_BER_HSDPA_CMD_BASE                 = 0x100,
    FTM_TDSCDMA_HSUPA_CMD_BASE                     = 0x2000,

};

/* This type specifies the interface for the Start TDSCDMA mode command */

typedef PACK(struct)
{

    diagpkt_subsys_header_type   header;
    ftm_cmd_header_type          ftm_cmd_header;

} ftm_tdscdma_ber_start_tdscdma_mode_req_type;

/* This type specifies the interface for the Stop TDSCDMA mode command */

typedef ftm_tdscdma_ber_start_tdscdma_mode_req_type ftm_tdscdma_ber_stop_tdscdma_mode_req_type;

/* This type specifies the interface for the Idle Request Command */

typedef ftm_tdscdma_ber_start_tdscdma_mode_req_type ftm_tdscdma_ber_idle_req_type;

/* This type specifies the Acquisition configuration type */

typedef PACK(struct)
{
    uint16                       freq;
} ftm_tdscdma_ber_acquire_config_type;

/* This type specifies the interface for the FTM TDSCDMA Acquire Command */

typedef PACK(struct)
{
    diagpkt_subsys_header_type           header;
    ftm_cmd_header_type                  ftm_cmd_header;
    ftm_tdscdma_ber_acquire_config_type    acq_config;
} ftm_tdscdma_ber_acquire_req_type;

#ifndef FEATURE_RF_TL1_FTM_NEW_INTF
typedef enum
{
    FTM_TDSCDMA_BER_RMC_TYPE_12_2_SC      = 0,
    FTM_TDSCDMA_BER_RMC_TYPE_12_2_MC      = 1,
    FTM_TDSCDMA_BER_RMC_TYPE_64           = 2,    
    FTM_TDSCDMA_BER_RMC_TYPE_144          = 3,
    FTM_TDSCDMA_BER_RMC_TYPE_384          = 4,    
    FTM_TDSCDMA_BER_RMC_TYPE_MAX
} ftm_tdscdma_ber_rmc_setup_type;

#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/
/* This type specifies the RMC DCH setup configuration */

#ifdef FEATURE_RF_TL1_FTM_NEW_INTF
typedef PACK(struct)
{
    tdsl1_ftm_ns_rmc_setup_type    rmc_setup;
    uint8 dl_slot;
    uint8 ul_slot;
    int8                              ul_max_tx_pwr;
    tdsl1_ftm_tpc_step_size_enum_type   ul_tpc_step;
    uint16                            rf_chan;
    uint8 reserved[10];
} ftm_tdscdma_ber_rmc_dch_setup_config_type;
#else /*FEATURE_RF_TL1_FTM_NEW_INTF*/
typedef PACK(struct)
{
    ftm_tdscdma_ber_rmc_setup_type    rmc_setup;
    uint8 dl_slot;
    uint8 ul_slot;
    int8                              ul_max_tx_pwr;
    tdsl1_tpc_step_size_enum_type          ul_tpc_step;
    uint16                            rf_chan;
    uint8 reserved[10];
} ftm_tdscdma_ber_rmc_dch_setup_config_type;
#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/


typedef enum
{
    FTM_TDSCDMA_BER_N312_s1    = 0,
    FTM_TDSCDMA_BER_N312_s50   = 1,
    FTM_TDSCDMA_BER_N312_s100  = 2,
    FTM_TDSCDMA_BER_N312_s200  = 3,
    FTM_TDSCDMA_BER_N312_s400  = 4,
    FTM_TDSCDMA_BER_N312_s600  = 5,
    FTM_TDSCDMA_BER_N312_s800  = 6,
    FTM_TDSCDMA_BER_N312_s1000 = 7,
  
} ftm_tdscdma_ber_n312_type;


typedef enum
{
    
    FTM_TDSCDMA_BER_N313_s1   = 0,
    FTM_TDSCDMA_BER_N313_s2   = 1,
    FTM_TDSCDMA_BER_N313_s4   = 2,
    FTM_TDSCDMA_BER_N313_s10  = 3,
    FTM_TDSCDMA_BER_N313_s20  = 4,
    FTM_TDSCDMA_BER_N313_s50  = 5,
    FTM_TDSCDMA_BER_N313_s100 = 6,
    FTM_TDSCDMA_BER_N313_s200 = 7

} ftm_tdscdma_ber_n313_type;

typedef enum
{
    
    FTM_TDSCDMA_BER_N315_s1   = 0,
    FTM_TDSCDMA_BER_N315_s50  = 1,
    FTM_TDSCDMA_BER_N315_s100 = 2,
    FTM_TDSCDMA_BER_N315_s200 = 3,
    FTM_TDSCDMA_BER_N315_s400 = 4,
    FTM_TDSCDMA_BER_N315_s600 = 5,
    FTM_TDSCDMA_BER_N315_s800 = 6,
    FTM_TDSCDMA_BER_N315_s1000 = 7

} ftm_tdscdma_ber_n315_type;

typedef enum
{

    FTM_TDSCDMA_BER_UL_TFCS_CONFIG = 0,
    FTM_TDSCDMA_BER_DL_TFCS_CONFIG,

} ftm_tdscdma_ber_tfcs_config_type;

typedef PACK(struct)
{
    diagpkt_subsys_header_type           header;
    ftm_cmd_header_type                  ftm_cmd_header;
    ftm_tdscdma_ber_tfcs_config_type       tfcs_select;
    uint8                                tfcs_config[ FTM_TDSCDMA_BER_NUM_TRANSPORT_FORMAT_COMBINATIONS];
} ftm_tdscdma_ber_rmc_dch_tfcs_config_req_type;

typedef enum
{
    FTM_TDSCDMA_PRIMARY_RX_CHAIN = 0,
    FTM_TDSCDMA_SECONDARY_RX_CHAIN,
}ftm_tdscdma_ber_rx_chain_type;

typedef PACK(struct)
{
    diagpkt_subsys_header_type           header;
    ftm_cmd_header_type                  ftm_cmd_header;
    ftm_tdscdma_ber_rx_chain_type          rx_chain;
} ftm_tdscdma_ber_rx_level_req_type;

/* This contains identical elements as DCH_SETUP_V2 command packet, plus
 * additional entries needed after V2 command packet was created.
 */

#ifdef FEATURE_RF_TL1_FTM_NEW_INTF
typedef struct
{
   //passed from ftm cmd.
   tdsl1_ftm_ns_rmc_setup_type    rmc_setup;
   uint8 dl_slot;
   uint8 ul_slot;
   /* Maximum allowed Tx power in dBm (-50..33)
    * See 25.331 v9.1.0 10.3.6.39*/
   int8                              ul_max_tx_pwr;
  
   tdsl1_ftm_tpc_step_size_enum_type     ul_tpc_step;
  
   uint16                            rf_chan;
   uint8 reserved[10];

   //internal
   uint16  current_tb_size;

   uint16  req_mask;
   
   uint16  ho_setup_mask;

   uint8   call_type_mask;

   boolean is_dch_refcg;
   /*Primary frequency - cell's primary frequency. See  25.331:10.3.6.36 and 25.102*/
   uint16  primary_freq; 

   /* TRUE - working frequency included*/
   boolean working_freq_incl;

   /*Working frequency. If working frequency is included, cell shall work at the frequency.*/
   uint16  working_freq; 

   /*Cell parameters ID (see 25.223:7.3)*/
   uint8   cell_parm_id;

   boolean seamless_recfg_ind;

    boolean freq_incl;
    
    uint16                            dl_pri_cpich_scr_code;
    uint16                            dl_chan_code;
    uint8                             beta_d;

    ftm_tdscdma_ber_n312_type         n_312;
    ftm_tdscdma_ber_n313_type         n_313;
    ftm_tdscdma_ber_n315_type         n_315;
    uint8                             t_312;
    uint8                             t_313;

    uint8                             dl_tfcs_config[ FTM_TDSCDMA_BER_NUM_TRANSPORT_FORMAT_COMBINATIONS];
    uint8                             ul_tfcs_config[ FTM_TDSCDMA_BER_NUM_TRANSPORT_FORMAT_COMBINATIONS];
} ftm_tdscdma_ber_dch_config_type;

#else /*FEATURE_RF_TL1_FTM_NEW_INTF*/
typedef struct
{
   //passed from ftm cmd.
   ftm_tdscdma_ber_rmc_setup_type    rmc_setup;
   uint8 dl_slot;
   uint8 ul_slot;
   /* Maximum allowed Tx power in dBm (-50..33)
    * See 25.331 v9.1.0 10.3.6.39*/
   int8                              ul_max_tx_pwr;
   tdsl1_tpc_step_size_enum_type          ul_tpc_step;
   uint16                            rf_chan;
   uint8 reserved[10];

   //internal
   uint16  current_tb_size;

   uint16  req_mask;
   
   uint16  ho_setup_mask;

   uint8   call_type_mask;

   boolean is_dch_refcg;
   /*Primary frequency - cell's primary frequency. See  25.331:10.3.6.36 and 25.102*/
   uint16  primary_freq; 

   /* TRUE - working frequency included*/
   boolean working_freq_incl;

   /*Working frequency. If working frequency is included, cell shall work at the frequency.*/
   uint16  working_freq; 

   /*Cell parameters ID (see 25.223:7.3)*/
   uint8   cell_parm_id;

   boolean seamless_recfg_ind;

   /* uplink physical channel type */
   tdsl1_ul_phychan_enum_type cfg_ul_phychan;
   tdsl1_ul_phychan_enum_type add_ul_phychan;
   tdsl1_ul_phychan_enum_type drop_ul_phychan;

   /* downlink physical channel type */
   tdsl1_dl_phychan_enum_type cfg_dl_phychan;
   tdsl1_dl_phychan_enum_type add_dl_phychan;
   tdsl1_dl_phychan_enum_type drop_dl_phychan;

    boolean freq_incl;
    
    uint16                            dl_pri_cpich_scr_code;
    uint16                            dl_chan_code;
    uint8                             beta_d;

    ftm_tdscdma_ber_n312_type         n_312;
    ftm_tdscdma_ber_n313_type         n_313;
    ftm_tdscdma_ber_n315_type         n_315;
    uint8                             t_312;
    uint8                             t_313;

    uint8                             dl_tfcs_config[ FTM_TDSCDMA_BER_NUM_TRANSPORT_FORMAT_COMBINATIONS];
    uint8                             ul_tfcs_config[ FTM_TDSCDMA_BER_NUM_TRANSPORT_FORMAT_COMBINATIONS];
} ftm_tdscdma_ber_dch_config_type;

#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/

#ifndef FEATURE_RF_TL1_FTM_NEW_INTF
//#define FTM_TDSCDMA_BER_UL_MAX_TB_SIZE       640
#define FTM_TDSCDMA_BER_UL_MAC_MAX_HDR_SIZE  6
#define FTM_TDSCDMA_BER_UL_MAX_NUM_TB        12
#define FTM_TDSCDMA_BER_UL_MAX_TRCH          4

typedef struct {
  /* pointer to transport block data */
  dsm_item_type *tb_data;
  /* MAC header data */
  uint8          mac_hdr[FTM_TDSCDMA_BER_UL_MAC_MAX_HDR_SIZE];

  /* size of MAC header in bits (0..39)*/
  uint8          mac_hdr_size;
    /* ciphering flag for this block */
  boolean        ciphering_on;
  /* TM ciphering state */
  tdsl1_tb_tm_cipher_state_enum_type tm_cipher_state;
    /* radio bearer id */
  uint8          radio_bearer_id;
    /* ciphering key */
  uint8          ciphering_key_id;
    /* combination of hyper frame num and RLC seq num */
  uint32         count;
    /* offset in transport block from where ciphering should start */
  uint8          offset;
 /* Ciphering Algorithm: Kasumi or Snow3G*/
  uint32 cipher_algo;

} ftm_tdscdma_ber_ul_tb_data_type;

/* transport channel data */
typedef struct {
  tdsl1_tti_enum_type tti;
    /* number of transport blocks - also indicates no TrCH if zero */
  uint8              num_tb;
    /* transport block size in bits (0..5120) */
  uint16             tb_size;
    /* transport channel data */
  ftm_tdscdma_ber_ul_tb_data_type trch_data[FTM_TDSCDMA_BER_UL_MAX_NUM_TB];
} ftm_tdscdma_ber_ul_trch_data_type;

/* Tx data (for a 10ms frame) + control information */
typedef struct {
    /* number of transport channels */
  uint8                num_trch;
    /* flag to indicate if MAC is busy writing this buffer */
  boolean              mac_busy;
    /* TFCI used for this 10ms frame of Tx data */
  uint16               TFCI;
    /* data of all transport channels in the 10ms frame */
  ftm_tdscdma_ber_ul_trch_data_type tx_data[FTM_TDSCDMA_BER_UL_MAX_TRCH];
    /* ASC index valid for PRACH only */
  uint8                asc_num;
} ftm_tdscdma_ber_ul_tx_data_type;
#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/

/* This type specifies the interface for the FTM TDSCDMA RMC DCH Setup Command */

typedef PACK(struct)
{
    diagpkt_subsys_header_type               header;
    ftm_cmd_header_type                      ftm_cmd_header;
    ftm_tdscdma_ber_rmc_dch_setup_config_type  rmc_dch_config;
} ftm_tdscdma_ber_rmc_dch_setup_type;


/* This type specifies the command id's for internal (FTM->FTM) TDSCDMA BER commands */
typedef enum
{
    FTM_TDSCDMA_BER_INT_CMD_RX_DATA = 0
} ftm_tdscdma_ber_int_cmd_id_type;


/* This type specifies the header structure for internal (FTM->FTM) TDSCDMA BER commands */
typedef struct
{
    q_link_type           link;
    ftm_tdscdma_ber_int_cmd_id_type       cmd_id;
} ftm_tdscdma_ber_int_cmd_hdr_type;

/* This type specifies the Rx data received command structure */
typedef struct
{
    uint8*                  rx_data; /*note: different from wcdma. we are not using watermark.*/
    uint16                  trb_size;
    boolean                 delete_payload;/*indicate if the handler should delete the payload after use*/
} ftm_tdscdma_ber_int_cmd_rx_data_type;

/* This type specifies the message field for internal (FTM->FTM) TDSCDMA BER commands */
typedef union
{
      ftm_tdscdma_ber_int_cmd_rx_data_type     rx_data_stream_cmd;
} ftm_tdscdma_ber_int_cmd_msg_type;

/* This type specifies the internal (FTM->FTM) command structure */
typedef struct
{
    ftm_tdscdma_ber_int_cmd_hdr_type           cmd_hdr;
    ftm_tdscdma_ber_int_cmd_msg_type           msg;
} ftm_tdscdma_ber_int_cmd_type;

/* This type specifies the request structure for FTM_TDSCDMA_BER_EVENT_STATUS_REQ */

typedef PACK(struct)
{
    diagpkt_subsys_header_type        header;
    ftm_cmd_header_type               ftm_cmd_header;
    int8                              event_type;
    uint32                            timeout_ms;
} ftm_tdscdma_ber_event_status_req_type;

typedef PACK(struct)
{
    diagpkt_subsys_header_type        header;
    ftm_cmd_header_type               ftm_cmd_header;
    byte                              override_enable;
    int2                              tx_power_10th_dbm;
} ftm_tdscdma_ber_set_ul_pwr_type;

typedef PACK(struct)
{
    diagpkt_subsys_header_type        header;
    ftm_cmd_header_type               ftm_cmd_header;
    int2                              override_enable;
    uint32                            tpc_pattern;
} ftm_tdscdma_ber_tpc_control_req_type;

/* FTM TDSCDMA BER Asynchronous Logging Interface */

/* Asynchronous logging interface */

/* This type specifies the possible asynchronous event packet types */
typedef enum
{
    FTM_LOG_TDSCDMA_BER_START_TDSCDMA_MODE_CNF  = 0,
    FTM_LOG_TDSCDMA_BER_STOP_TDSCDMA_MODE_CNF   = 1,
    FTM_LOG_TDSCDMA_BER_IDLE_MODE_CNF         = 2,
    FTM_LOG_TDSCDMA_BER_ACQ_CNF               = 3,
    FTM_LOG_TDSCDMA_BER_CPHY_SETUP_CNF        = 4,
    FTM_LOG_TDSCDMA_PHYCHAN_ESTABLISHED_IND   = 5,
    FTM_LOG_TDSCDMA_CPHY_ERROR_IND            = 6,
    FTM_LOG_TDSCDMA_RL_FAILURE_IND            = 7,
    FTM_LOG_TDSHSDPA_LOG_ID_BASE              = 0x40
} ftm_log_tdscdma_ber_id_type;

/* This type specifies the possible synchronous status EVENT_ID types for the 
   FTM_TDSCDMA_BER_EVENT_STATUS_REQ command
*/
typedef enum
{
    FTM_TDSCDMA_BER_START_TDSCDMA_MODE_SYNC_STATUS  = 0,
    FTM_TDSCDMA_BER_STOP_TDSCDMA_MODE_SYNC_STATUS   = 1,
    FTM_TDSCDMA_BER_IDLE_MODE_SYNC_STATUS         = 2,
    FTM_TDSCDMA_BER_ACQ_SYNC_STATUS               = 3,
    FTM_TDSCDMA_BER_CPHY_SETUP_SYNC_STATUS        = 4,
    FTM_TDSCDMA_PHYCHAN_ESTABLISHED_SYNC_STATUS   = 5,
 

	// Insert new values above this one
	FTM_TDSCDMA_BER_SYNC_STATUS_MAX

} ftm_log_tdscdma_ber_sync_event_id_type;


typedef enum
{
    FTM_TDSCDMA_BER_STATUS_GEN_SUCCESS = 0,
    FTM_TDSCDMA_BER_STATUS_GEN_FAILURE,
    FTM_TDSCDMA_BER_STATUS_INVALID_STATE,
    FTM_TDSCDMA_BER_STATUS_INIT_DCH_SUCCESS,
    FTM_TDSCDMA_BER_STATUS_DCH_RECFG_SUCCESS,
    FTM_TDSCDMA_BER_STATUS_ERROR_RECOVERY
} ftm_tdscdma_ber_status_type;

/* This type specifies the synchronous status types reported using  
   the FTM_TDSCDMA_BER_EVENT_STATUS_REQ command 
*/
typedef enum
{
    FTM_TDSCDMA_BER_SYNC_STATUS_SUCCESS                     = 0,
    FTM_TDSCDMA_BER_SYNC_STATUS_GEN_FAILURE                 = 1,
    FTM_TDSCDMA_BER_SYNC_STATUS_TIMEOUT_EXPIRED             = 2,
    FTM_TDSCDMA_BER_SYNC_STATUS_EVENT_STATUS_FAILED         = 3,
    FTM_TDSCDMA_BER_SYNC_STATUS_UNKNOWN_CMD                 = 4,
    FTM_TDSCDMA_BER_SYNC_STATUS_PWR_SETTING_OUT_OF_RANGE    = 5,
    FTM_TDSCDMA_BER_SYNC_STATUS_OVERRIDE_VALUE_UNSUPPORTED  = 6
} ftm_tdscdma_ber_sync_status_type;

typedef enum
{
    FTM_TDSCDMA_TX_POWER_CONTROL_TYPE_AGC_SETPOINT,
    FTM_TDSCDMA_TX_POWER_CONTROL_TYPE_RPC_ENABLE,
    FTM_TDSCDMA_TX_POWER_CONTROL_TYPE_POWER_UP_BITS,
    FTM_TDSCDMA_TX_POWER_CONTROL_TYPE_MAX
} ftm_tdscdma_ber_power_control_type;

/* This type specifies the FTM TDSCDMA BER generic confirmation event packet */
typedef PACK(struct)
{
    ftm_log_gen_type           header;
    ftm_log_tdscdma_ber_id_type  log_id;
} ftm_log_tdscdma_ber_gen_cnf;

/* This type specifies the FTM TDSCDMA BER generic confirmation event with status packet */
typedef PACK(struct)
{
    ftm_log_gen_type           header;
    ftm_log_tdscdma_ber_id_type  log_id;
    uint8                      status;
} ftm_log_tdscdma_ber_gen_cnf_with_status;


 
/* This type specifies the FTM GSM BER select specific bcch confirmation event */
typedef PACK(struct)
{
    ftm_log_gen_type           header;
    ftm_log_tdscdma_ber_id_type  log_id;
    boolean                    status;
    uint16                     freq;
} ftm_log_tdscdma_ber_acq_cnf_type;


enum
{
    FTM_TDSCDMA_BER_GEN_RESPONSE = 0x1002
};

/* This type specifies the FTM TDSCDMA BER response header packet */
typedef PACK(struct)
{
    uint16                     rsp_id;
    uint16                     rsp_cid;
    uint16                     rsp_scid;
} ftm_tdscdma_ber_rsp_header_type;

/* This type specifies the FTM TDSCDMA BER response packet */
typedef PACK(struct)
{
    diagpkt_subsys_header_type     header;
    ftm_tdscdma_ber_rsp_header_type  ftm_rsp_header;
    uint16                         status;
} ftm_tdscdma_ber_gen_rsp_pkt_type;


typedef PACK(struct)
{
	int16                        rssi;
	int16                        rx_agc_dbm10;
}ftm_tdscdma_ber_rx_level_rpt_type;

typedef PACK(struct)
{
    diagpkt_subsys_header_type     header;
    ftm_tdscdma_ber_rsp_header_type  ftm_rsp_header;
    uint16                         status;
    ftm_tdscdma_ber_rx_level_rpt_type rx_lev_rpt;
} ftm_tdscdma_ber_rx_lev_rsp_pkt_type;

#ifndef FEATURE_RF_TL1_FTM_NEW_INTF
/*===========================================================================
FUNCTION     FTM_PUT_TDSL1_CMD

DESCRIPTION
  This function is called by TDSCDMA L1 to send a message to FTM.  In normal
  phone operation these messages go to RRC.  But in Factory Test Mode,
  FTM is commanding TDSL1, so FTM gets TDSL1 messages.

DEPENDENCIES
  None.

PARAMETERS
  rrc_cmd_type *cmd_buf - Pointer to command structure.

RETURN VALUE
  None.

SIDE EFFECTS
  Posts a TDSL1 message to FTM.
===========================================================================*/
extern void ftm_put_tdsl1_cmd(tdsrrc_cmd_type *cmd_ptr);

#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/
extern void ftm_tdscdma_ber_cleanup( void);
extern void ftm_tdscdma_ber_init( void);

extern ftm_tdscdma_ber_state_type ftm_tdscdma_ber_get_state( void);
#ifndef FEATURE_RF_TL1_FTM_NEW_INTF
extern void ftm_tdscdma_ber_send_CPHY_SETUP_REQ( tdsl1_setup_cmd_type *cmd);
extern void ftm_tdscdma_ber_init_cphy_setup_cmd( tdsl1_setup_cmd_type *cmd, boolean signal_mac);

#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/
extern void ftm_tdscdma_ber_process_msgs( void);
extern void ftm_tdscdma_ber_process_int_msgs( void);


void ftm_tdscdma_ber_set_state(ftm_tdscdma_ber_state_type s);
void ftm_tdscdma_ber_assign_cctrch_id( void);
#ifndef FEATURE_RF_TL1_FTM_NEW_INTF
void ftm_tdscdma_ber_dl_cphy_setup_cmd( tdsl1_setup_cmd_type *cmd);
void ftm_tdscdma_ber_ul_cphy_setup_cmd( tdsl1_setup_cmd_type *cmd);
void ftm_tdscdma_ber_send_CPHY_DROP_ALL_REQ(void);

extern void ftm_tdscdma_ber_ul_build_frame_ind
(
  tdsl1_tti_enum_type     tti,
  boolean                 ul_tx_disabled,
  tdsl1_ul_tx_data_type  *tx_buf_ptr
);
#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/
#endif /* FTM_TDSCDMA_BER_H */
