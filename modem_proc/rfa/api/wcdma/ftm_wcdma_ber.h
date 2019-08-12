/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2002 - 2012 by Qualcomm Technologies INCORPORATED.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
 
                           EDIT HISTORY FOR MODULE
                           
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/wcdma/ftm_wcdma_ber.h#1 $ 
  $DateTime: 2016/12/13 07:59:52 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/07/13   ck      Add prototype ftm_wcdma_ber_send_CPHY_STOP_WCDMA_MODE_REQ()
04/19/12   kai     Removed non api header file l1ulcfg.h 
03/13/12   gh      Add api to clear RRC cmd queue
02/17/12   swb     Add support for RSCP 
02/13/12   swb     Moved function declarations from .c file 
01/12/12   dw      Add ftm_wcdma_ber_set_state()
10/28/11   gh      Add APIs to check if RRC cmd is START/STOP_WCDMA_MODE_CNF 
04/08/11   sar     Removed condition #def's for CMI-4.
10/21/10   ems     Factory features Rx Lvl, non-signaling events and SE/BER
02/03/10   kguo    Changed PACKED struct to PACK(struct) 
06-06-06   dp      Add TFCS config command.
10-27-05   dp      Add symmetric configurations.
10-17-05   dp      Add "ftm_umts_ber.h" include.
07-20-05   dp      Add V2 RMC DCH Setup command to support more parameters,
                   and ability to do HHO.
05-19-05   dp      Add HSDPA support (featurized)
11-18-04   dp      Added ftm_wcdma_ber_cleanup function
11-8-04    scm     Command confirmation support from WL1 back to FTM.
12-17-03   dp      Created
===========================================================================*/
#ifndef FTM_WCDMA_BER_H
#define FTM_WCDMA_BER_H

/*===========================================================================

          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"
#include "customer.h"
#include "ftm.h"
#include "ftmdiag.h"
#include "queue.h"
#include "l1rrcif.h"
#include "rrccmd.h"

#include "ftm_log.h"
#include "ftm_umts_ber.h"



/*===========================================================================
 
                         EXTERNAL GLOBAL VARIABLES

===========================================================================*/

/*===========================================================================
 
                    LOCAL GLOBAL VARIABLES AND CONSTANTS

===========================================================================*/
/* FTM WCDMA BER feature only uses RMC channels, with 2 logical channels
 * and 4 distict TFCI values 
 */

#define FTM_WCDMA_BER_MAX_NUM_LOGICAL_CHANNELS     2
#define FTM_WCDMA_BER_NUM_TRANSPORT_FORMAT_COMBINATIONS 4    
/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/
typedef uint16 ftm_wcdma_arfcn;


typedef struct
{
    l1_dl_cctrch_info_struct_type    *dl_cctrch_info;
    l1_dl_ctfc_info_struct_type      *dl_ctfc_info;
    l1_dl_phychan_db_struct_type     *add_dl_phychan_db;
    uint16                           current_rf_chan;
    boolean                          current_rf_chan_active;
    boolean                          isInitialized;

} ftm_wcdma_ber_dl_db_type;


typedef struct
{
    l1_ul_cctrch_cfg_struct_type      *ul_cctrch_info;
    l1_ul_phychan_cfg_struct_type     *add_ul_phychan_db;
    mac_ul_tfcs_type                  *tfcs_ptr;
    boolean                           isInitialized;
} ftm_wcdma_ber_ul_db_type;




/* Interface to PC */

/* Command codes for the FTM WCDMA BER feature */

enum
{

    FTM_WCDMA_BER_START_WCDMA_MODE_REQ = 0,
    FTM_WCDMA_BER_STOP_WCDMA_MODE_REQ  = 1,
    FTM_WCDMA_BER_IDLE_REQ             = 2,
    FTM_WCDMA_BER_ACQUIRE_REQ          = 3,
    FTM_WCDMA_BER_RMC_DCH_SETUP        = 4,
    FTM_WCDMA_BER_RMC_DCH_SETUP_V2           = 5,
    FTM_WCDMA_BER_RMC_DCH_TFCS_CONFIG_REQ    = 6,
    FTM_WCDMA_BER_HSDPA_CMD_BASE       = 0x100

};

/* This type specifies the interface for the Start WCDMA mode command */

typedef PACK(struct)
{

    diagpkt_subsys_header_type   header;
    ftm_cmd_header_type          ftm_cmd_header;

} ftm_wcdma_ber_start_wcdma_mode_req_type;

/* This type specifies the interface for the Stop WCDMA mode command */

typedef ftm_wcdma_ber_start_wcdma_mode_req_type ftm_wcdma_ber_stop_wcdma_mode_req_type;

/* This type specifies the interface for the Idle Request Command */

typedef ftm_wcdma_ber_start_wcdma_mode_req_type ftm_wcdma_ber_idle_req_type;

/* This type specifies the Acquisition configuration type */

typedef PACK(struct)
{
    
    l1_wcdma_acq_mech_type       acq_type;
    l1_wcdma_acq_mode_type       acq_mode;
    uint16                       freq;
    uint16                       scr_code;
    uint32                       pn;


} ftm_wcdma_ber_acquire_config_type;

/* This type specifies the interface for the FTM WCDMA Acquire Command */

typedef PACK(struct)
{

    diagpkt_subsys_header_type           header;
    ftm_cmd_header_type                  ftm_cmd_header;
    ftm_wcdma_ber_acquire_config_type    acq_config;

} ftm_wcdma_ber_acquire_req_type;


typedef enum
{
    FTM_WCDMA_BER_RMC_TYPE_12_2            = 0,
    FTM_WCDMA_BER_RMC_TYPE_64              = 1,
    FTM_WCDMA_BER_RMC_TYPE_384             = 2,
    FTM_WCDMA_BER_RMC_TYPE_12_2_SYMMETRIC  = 3,
    FTM_WCDMA_BER_RMC_TYPE_64_SYMMETRIC    = 4,
    FTM_WCDMA_BER_RMC_TYPE_384_SYMMETRIC   = 5

} ftm_wcdma_ber_rmc_setup_type;

/* This type specifies the RMC DCH setup configuration */

typedef PACK(struct)
{
    ftm_wcdma_ber_rmc_setup_type      rmc_setup;
    int8                              ul_max_tx_pwr;
    uint32                            ul_dpch_scr_code;
    l1_ul_pca_enum_type               ul_pca;
    l1_ul_tpc_step_enum_type          ul_tpc_step;
    uint16                            dl_pri_cpich_scr_code;
    uint16                            dl_chan_code;

} ftm_wcdma_ber_rmc_dch_setup_config_type;

typedef enum
{
    FTM_WCDMA_BER_N312_s1    = 0,
    FTM_WCDMA_BER_N312_s50   = 1,
    FTM_WCDMA_BER_N312_s100  = 2,
    FTM_WCDMA_BER_N312_s200  = 3,
    FTM_WCDMA_BER_N312_s400  = 4,
    FTM_WCDMA_BER_N312_s600  = 5,
    FTM_WCDMA_BER_N312_s800  = 6,
    FTM_WCDMA_BER_N312_s1000 = 7,
  
} ftm_wcdma_ber_n312_type;


typedef enum
{
    
    FTM_WCDMA_BER_N313_s1   = 0,
    FTM_WCDMA_BER_N313_s2   = 1,
    FTM_WCDMA_BER_N313_s4   = 2,
    FTM_WCDMA_BER_N313_s10  = 3,
    FTM_WCDMA_BER_N313_s20  = 4,
    FTM_WCDMA_BER_N313_s50  = 5,
    FTM_WCDMA_BER_N313_s100 = 6,
    FTM_WCDMA_BER_N313_s200 = 7

} ftm_wcdma_ber_n313_type;

typedef enum
{
    
    FTM_WCDMA_BER_N315_s1   = 0,
    FTM_WCDMA_BER_N315_s50  = 1,
    FTM_WCDMA_BER_N315_s100 = 2,
    FTM_WCDMA_BER_N315_s200 = 3,
    FTM_WCDMA_BER_N315_s400 = 4,
    FTM_WCDMA_BER_N315_s600 = 5,
    FTM_WCDMA_BER_N315_s800 = 6,
    FTM_WCDMA_BER_N315_s1000 = 7

} ftm_wcdma_ber_n315_type;

typedef enum
{

    FTM_WCDMA_BER_UL_TFCS_CONFIG = 0,
    FTM_WCDMA_BER_DL_TFCS_CONFIG,

} ftm_wcdma_ber_tfcs_config_type;

typedef PACK(struct)
{

    diagpkt_subsys_header_type           header;
    ftm_cmd_header_type                  ftm_cmd_header;
    ftm_wcdma_ber_tfcs_config_type       tfcs_select;
    uint8                                tfcs_config[ FTM_WCDMA_BER_NUM_TRANSPORT_FORMAT_COMBINATIONS];
} ftm_wcdma_ber_rmc_dch_tfcs_config_req_type;

typedef enum
{
    PRIMARY_RX_CHAIN = 0,
    SECONDARY_RX_CHAIN,
}ftm_wcdma_ber_rx_chain_type;

typedef PACK(struct)
{
    diagpkt_subsys_header_type           header;
    ftm_cmd_header_type                  ftm_cmd_header;
    ftm_wcdma_ber_rx_chain_type          rx_chain;
} ftm_wcdma_ber_rx_level_req_type;


/* This type specifies the RMC DCH setup Version 2 configuration */

typedef PACK(struct)
{
    ftm_wcdma_ber_rmc_setup_type      rmc_setup;
    int8                              ul_max_tx_pwr;
    uint32                            ul_dpch_scr_code;
    l1_ul_pca_enum_type               ul_pca;
    l1_ul_tpc_step_enum_type          ul_tpc_step;
    uint16                            dl_pri_cpich_scr_code;
    uint16                            dl_chan_code;
    uint8                             beta_c;
    uint8                             beta_d;
    int16                             dpcch_power_offset;
    ftm_wcdma_ber_n312_type           n_312;
    ftm_wcdma_ber_n313_type           n_313;
    ftm_wcdma_ber_n315_type           n_315;
    uint8                             t_312;
    uint8                             t_313;
    uint16                            rf_chan;

} ftm_wcdma_ber_rmc_dch_setup_v2_config_type;


/* This contains identical elements as DCH_SETUP_V2 command packet, plus
 * additional entries needed after V2 command packet was created.
 */

typedef struct
{
    ftm_wcdma_ber_rmc_setup_type      rmc_setup;
    int8                              ul_max_tx_pwr;
    uint32                            ul_dpch_scr_code;
    l1_ul_pca_enum_type               ul_pca;
    l1_ul_tpc_step_enum_type          ul_tpc_step;
    uint16                            dl_pri_cpich_scr_code;
    uint16                            dl_chan_code;
    uint8                             beta_c;
    uint8                             beta_d;
    int16                             dpcch_power_offset;
    ftm_wcdma_ber_n312_type           n_312;
    ftm_wcdma_ber_n313_type           n_313;
    ftm_wcdma_ber_n315_type           n_315;
    uint8                             t_312;
    uint8                             t_313;
    uint16                            rf_chan;
    uint8                             dl_tfcs_config[ FTM_WCDMA_BER_NUM_TRANSPORT_FORMAT_COMBINATIONS];
    uint8                             ul_tfcs_config[ FTM_WCDMA_BER_NUM_TRANSPORT_FORMAT_COMBINATIONS];
} ftm_wcdma_ber_dch_config_type;

/* This type specifies the interface for the FTM WCDMA RMC DCH Setup Command */

typedef PACK(struct)
{

    diagpkt_subsys_header_type               header;
    ftm_cmd_header_type                      ftm_cmd_header;
    ftm_wcdma_ber_rmc_dch_setup_config_type  rmc_dch_config;

} ftm_wcdma_ber_rmc_dch_setup_type;

typedef PACK(struct)
{

    diagpkt_subsys_header_type                  header;
    ftm_cmd_header_type                         ftm_cmd_header;
    ftm_wcdma_ber_rmc_dch_setup_v2_config_type  rmc_dch_config;

} ftm_wcdma_ber_rmc_dch_setup_v2_type;


/* This type specifies the command id's for internal (FTM->FTM) WCDMA BER commands */
typedef enum
{
    FTM_WCDMA_BER_INT_CMD_RX_DATA = 0
} ftm_wcdma_ber_int_cmd_id_type;


/* This type specifies the header structure for internal (FTM->FTM) WCDMA BER commands */
typedef struct
{
    q_link_type           link;
    ftm_wcdma_ber_int_cmd_id_type       cmd_id;
} ftm_wcdma_ber_int_cmd_hdr_type;

/* This type specifies the Rx data received command structure */
typedef struct
{
    uint8                  wm_id;

} ftm_wcdma_ber_int_cmd_rx_data_type;

/* This type specifies the message field for internal (FTM->FTM) WCDMA BER commands */
typedef union
{
      ftm_wcdma_ber_int_cmd_rx_data_type     rx_data_stream_cmd;
} ftm_wcdma_ber_int_cmd_msg_type;

/* This type specifies the internal (FTM->FTM) command structure */
typedef struct
{
    ftm_wcdma_ber_int_cmd_hdr_type           cmd_hdr;
    ftm_wcdma_ber_int_cmd_msg_type           msg;
} ftm_wcdma_ber_int_cmd_type;

/* This type specifies the request structure for FTM_WCDMA_BER_EVENT_STATUS_REQ */

typedef PACK(struct)
{
    diagpkt_subsys_header_type        header;
    ftm_cmd_header_type               ftm_cmd_header;
    int8                              event_type;
    uint32                            timeout_ms;

} ftm_wcdma_ber_event_status_req_type;

typedef PACK(struct)
{
    diagpkt_subsys_header_type        header;
    ftm_cmd_header_type               ftm_cmd_header;
    byte                              override_enable;
    int2                              tx_power_10th_dbm;
} ftm_wcdma_ber_set_ul_pwr_type;

/* FTM WCDMA BER Asynchronous Logging Interface */

/* Asynchronous logging interface */

/* This type specifies the possible asynchronous event packet types */
typedef enum
{

    FTM_LOG_WCDMA_BER_START_WCDMA_MODE_CNF  = 0,
    FTM_LOG_WCDMA_BER_STOP_WCDMA_MODE_CNF   = 1,
    FTM_LOG_WCDMA_BER_IDLE_MODE_CNF         = 2,
    FTM_LOG_WCDMA_BER_ACQ_CNF               = 3,
    FTM_LOG_WCDMA_BER_CPHY_SETUP_CNF        = 4,
    FTM_LOG_WCDMA_PHYCHAN_ESTABLISHED_IND   = 5,
    FTM_LOG_WCDMA_CPHY_ERROR_IND            = 6,
    FTM_LOG_WCDMA_RL_FAILURE_IND            = 7,
    FTM_LOG_HSDPA_LOG_ID_BASE              = 0x40

} ftm_log_wcdma_ber_id_type;

/* This type specifies the possible synchronous status EVENT_ID types for the 
   FTM_WCDMA_BER_EVENT_STATUS_REQ command
*/
typedef enum
{

    FTM_WCDMA_BER_START_WCDMA_MODE_SYNC_STATUS  = 0,
    FTM_WCDMA_BER_STOP_WCDMA_MODE_SYNC_STATUS   = 1,
    FTM_WCDMA_BER_IDLE_MODE_SYNC_STATUS         = 2,
    FTM_WCDMA_BER_ACQ_SYNC_STATUS               = 3,
    FTM_WCDMA_BER_CPHY_SETUP_SYNC_STATUS        = 4,
    FTM_WCDMA_PHYCHAN_ESTABLISHED_SYNC_STATUS   = 5,
    FTM_WCDMA_BER_EVENT_STATUS_CLEAR_REQ        = 7,
    FTM_WCDMA_BER_EVENT_STATUS_REQ              = 8,
    FTM_WCDMA_BER_SET_UL_POWER                  = 9,
    FTM_WCDMA_BER_RX_LEVEL_REQ                  = 10,
    FTM_WCDMA_BER_RSCP_REQ                      = 11,

	// Insert new values above this one
	FTM_WCDMA_BER_SYNC_STATUS_MAX

} ftm_log_wcdma_ber_sync_event_id_type;


typedef enum
{
    FTM_WCDMA_BER_STATUS_GEN_SUCCESS = 0,
    FTM_WCDMA_BER_STATUS_GEN_FAILURE,
    FTM_WCDMA_BER_STATUS_INVALID_STATE,
    FTM_WCDMA_BER_STATUS_INIT_DCH_SUCCESS,
    FTM_WCDMA_BER_STATUS_DCH_RECFG_SUCCESS,
    FTM_WCDMA_BER_STATUS_ERROR_RECOVERY
} ftm_wcdma_ber_status_type;

/* This type specifies the synchronous status types reported using  
   the FTM_WCDMA_BER_EVENT_STATUS_REQ command 
*/
typedef enum
{

    FTM_WCDMA_BER_SYNC_STATUS_SUCCESS                     = 0,
    FTM_WCDMA_BER_SYNC_STATUS_GEN_FAILURE                 = 1,
    FTM_WCDMA_BER_SYNC_STATUS_TIMEOUT_EXPIRED             = 2,
    FTM_WCDMA_BER_SYNC_STATUS_EVENT_STATUS_FAILED         = 3,
    FTM_WCDMA_BER_SYNC_STATUS_UNKNOWN_CMD                 = 4,
    FTM_WCDMA_BER_SYNC_STATUS_PWR_SETTING_OUT_OF_RANGE    = 5,
    FTM_WCDMA_BER_SYNC_STATUS_OVERRIDE_VALUE_UNSUPPORTED  = 6


} ftm_wcdma_ber_sync_status_type;

typedef enum
{
    FTM_WCDMA_TX_POWER_CONTROL_TYPE_AGC_SETPOINT,
    FTM_WCDMA_TX_POWER_CONTROL_TYPE_RPC_ENABLE,
    FTM_WCDMA_TX_POWER_CONTROL_TYPE_POWER_UP_BITS,
    FTM_WCDMA_TX_POWER_CONTROL_TYPE_MAX
} ftm_wcdma_ber_power_control_type;

/* This type specifies the FTM WCDMA BER generic confirmation event packet */
typedef PACK(struct)
{

    ftm_log_gen_type           header;
    ftm_log_wcdma_ber_id_type  log_id;

} ftm_log_wcdma_ber_gen_cnf;

/* This type specifies the FTM WCDMA BER generic confirmation event with status packet */
typedef PACK(struct)
{

    ftm_log_gen_type           header;
    ftm_log_wcdma_ber_id_type  log_id;
    uint8                      status;

} ftm_log_wcdma_ber_gen_cnf_with_status;


 
/* This type specifies the FTM GSM BER select specific bcch confirmation event */
typedef PACK(struct)
{

    ftm_log_gen_type           header;
    ftm_log_wcdma_ber_id_type  log_id;
    boolean                    status;
    uint16                     freq;
    uint16                     scr_code;

} ftm_log_wcdma_ber_acq_cnf_type;


enum
{
    FTM_WCDMA_BER_GEN_RESPONSE = 0x1002
};

/* This type specifies the FTM WCDMA BER response header packet */
typedef PACK(struct)
{
    uint16                     rsp_id;
    uint16                     rsp_cid;
    uint16                     rsp_scid;

} ftm_wcdma_ber_rsp_header_type;

/* This type specifies the FTM WCDMA BER response packet */
typedef PACK(struct)
{

    diagpkt_subsys_header_type     header;
    ftm_wcdma_ber_rsp_header_type  ftm_rsp_header;
    uint16                         status;

} ftm_wcdma_ber_gen_rsp_pkt_type;


typedef PACK(struct)
{
	int16                        rssi;
	int16                        rx_agc_dbm10;
}ftm_wcdma_ber_rx_level_rpt_type;

typedef PACK(struct)
{

    diagpkt_subsys_header_type     header;
    ftm_wcdma_ber_rsp_header_type  ftm_rsp_header;
    uint16                         status;
    ftm_wcdma_ber_rx_level_rpt_type rx_lev_rpt;
} ftm_wcdma_ber_rx_lev_rsp_pkt_type;

typedef PACK(struct)
{
  int32 ecio_rx0;
  int32 rscp_rx0; 
  int32 ecio_rx1; 
  int32 rscp_rx1;
}ftm_wcdma_ber_rscp_rpt_type;

typedef PACK(struct)
{
  diagpkt_subsys_header_type     header;
  ftm_wcdma_ber_rsp_header_type  ftm_rsp_header;
  uint16                         status;
  ftm_wcdma_ber_rscp_rpt_type    rscp_rpt;
} ftm_wcdma_ber_rscp_rsp_pkt_type;

/*===========================================================================
FUNCTION     FTM_PUT_WL1_CMD

DESCRIPTION
  This function is called by WCDMA L1 to send a message to FTM.  In normal
  phone operation these messages go to RRC.  But in Factory Test Mode,
  FTM is commanding WL1, so FTM gets WL1 messages.

DEPENDENCIES
  None.

PARAMETERS
  rrc_cmd_type *cmd_buf - Pointer to command structure.

RETURN VALUE
  None.

SIDE EFFECTS
  Posts a WL1 message to FTM.
===========================================================================*/
extern void ftm_put_wl1_cmd(rrc_cmd_type *cmd_ptr);

extern void ftm_wcdma_ber_cleanup( void);
extern void ftm_wcdma_ber_init( void);

extern ftm_wcdma_ber_state_type ftm_wcdma_ber_get_state( void);
extern void ftm_wcdma_ber_send_CPHY_SETUP_REQ( l1_setup_cmd_type *cmd);
extern void ftm_wcdma_ber_init_cphy_setup_cmd( l1_setup_cmd_type *cmd, boolean signal_mac);
extern boolean ftm_wcdma_ber_is_rrc_cmd_start_wcdma_mode(void);
extern boolean ftm_wcdma_ber_is_rrc_cmd_stop_wcdma_mode(void);
extern void ftm_wcdma_ber_set_state(ftm_wcdma_ber_state_type state);
void ftm_wcdma_ber_send_CPHY_STOP_WCDMA_MODE_REQ(void);
void ftm_wcdma_ber_assign_cctrch_id(void);
void ftm_wcdma_ber_add_dl_cphy_setup_cmd( l1_setup_cmd_type *cmd);
void ftm_wcdma_ber_add_ul_cphy_setup_cmd( l1_setup_cmd_type *cmd);
void ftm_wcdma_ber_drop_dl_cphy_setup_cmd( l1_setup_cmd_type *cmd);
void ftm_wcdma_ber_drop_ul_cphy_setup_cmd( l1_setup_cmd_type *cmd);
void ftm_wcdma_ber_clear_rrc_cmd_q(void);

#endif /* FTM_WCDMA_BER_H */
