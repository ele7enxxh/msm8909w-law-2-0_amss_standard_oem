/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

       F T M  H W T C  D V T  S T R I N G  C O M M A N D S  M O D U L E

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2002 by Qualcomm Technologies INCORPORATED.
  Copyright (c) 2003 by Qualcomm Technologies INCORPORATED.
  Copyright (c) 2013 by Qualcomm Technologies INCORPORATED.  
  Copyright (c) 2014 by Qualcomm Technologies INCORPORATED.  
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
 
                           EDIT HISTORY FOR MODULE
                           
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/gsm/ftm_gsm_ber.h#1 $ 
  $DateTime: 2016/12/13 07:59:52 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/25/14   par     New API to set the FTM GSM NS exit signal which 
                   will be used by GL1         
03/31/14   tsr     Add support for GSM RX Diversity
05/07/13   ck      Change return type of ftm_gsm_ber_cleanup
04/03/13   ec	   Remove redundant function ftm_gsm_ber_set_state
01/29/13   ec      Add framework for Non Signalling on multiple RF Devices 
11/21/12   tws     Remove legacy GERAN API includes.
06/12/12   lp      Removed rfa_variations.h include file 
01/05/12   sr      geran api changes for non-signaling.
11/23/11   sr      removed GSDI related include files.
04/08/11   sar     Removed condition #def's for CMI-4.
04/05/11   sar     Removed conditional #def's to comply with CMI.
11/01/10   ems     Support for FTM Sequencer 
03/19/10   lcl     Pull in fixes from QSC6295
02/04/10   sr      Lint & kw fixes 
01/20/10   tws     Use PACK(...) instead of PACKED for qdsp6 compatibility.
10/22/07   ad      SE BER support From 6260: Increase Rx data buffer queue size 
                   for SE BER data indications from GL1
06/25/07   ad      Added support for single ended BER
09/18/06   dp      Handle loopback configuration properly across handover, channel
                   release, and GSM Stop.
10-17-05   dp      Add ftm_umts_ber.h include file.
07-20-05   dp      Add Version 2 Channel Assign request packet for better TCH
                   param control.
11-18-04   dp      Added ftm_gsm_ber_cleanup function
10/18/04   fpe     Removed FTM_HAS_ASYNC_PKT
12-17-03   dp      Created
===========================================================================*/
#ifndef FTM_GSM_BER_H
#define FTM_GSM_BER_H

/*===========================================================================

          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"
#include "customer.h"
#include "ftm.h"
#include "ftmdiag.h"
#include "queue.h"

#include "gl1_defs.h"
#include "gs.h"
#include "rr_l1.h"
#include "l2_l1.h"
#include "sys_type.h"

#include "log.h"
#include "ftm_log.h"
#include "ftm_umts_ber.h"

/*===========================================================================
 
                         EXTERNAL GLOBAL VARIABLES

===========================================================================*/

/*===========================================================================
 
                    LOCAL GLOBAL VARIABLES AND CONSTANTS

===========================================================================*/

/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/

/* This type specifies the possible FTM GSM TCH slots */
typedef enum
{
    FTM_TCH_SLOT0 = 0,
    FTM_TCH_SLOT1,
    FTM_TCH_SLOT2,
    FTM_TCH_SLOT3,
    FTM_TCH_SLOT4,
    FTM_TCH_SLOT5,
    FTM_TCH_SLOT6,
    FTM_TCH_SLOT7,
    FTM_TCH_SLOT_INVALID
} ftm_gsm_ber_tch_slot_type;

/* This type specifies the allowed TCH modes */
typedef enum
{

    FTM_GSM_BER_TCH_FR   = 0,
    FTM_GSM_BER_TCH_HR   = 1,
    FTM_GSM_BER_TCH_EFR  = 2,
    FTM_GSM_BER_TCH_AMR  = 3

} ftm_gsm_ber_tch_mode_type;

/* This type specifies the possible loopback modes */
typedef enum
{
    FTM_GSM_LOOPBACK_TYPE_NULL    = GL1_DEFS_NO_LOOPBACK,
    FTM_GSM_LOOPBACK_TYPE_A       = GL1_DEFS_LOOPBACK_TYPE_A,
    FTM_GSM_LOOPBACK_TYPE_B       = GL1_DEFS_LOOPBACK_TYPE_B,
    FTM_GSM_LOOPBACK_TYPE_C       = GL1_DEFS_LOOPBACK_TYPE_C, 
    FTM_GSM_LOOPBACK_TYPE_D       = GL1_DEFS_LOOPBACK_TYPE_D,
    FTM_GSM_LOOPBACK_TYPE_E       = GL1_DEFS_LOOPBACK_TYPE_E,
    FTM_GSM_LOOPBACK_TYPE_F       = GL1_DEFS_LOOPBACK_TYPE_F, 
    FTM_GSM_NUM_LOOPBACK_TYPES
} ftm_gsm_ber_loopback_type;

typedef ARFCN_T ftm_gsm_ber_arfcn_type;


/* 
 * This type specifies a buffer to hold network parameters used during 
 * the FTM GSM BER tests
 *
 */
typedef struct
{
    
    BSIC_T                               bsic;
} ftm_gsm_ber_network_parms_type;

/* 
 * This type specifies a buffer to hold the current state of the FTM GSM BER 
 * module
 *
 */
typedef struct
{

    ftm_gsm_ber_state_type          state;
    ftm_gsm_ber_arfcn_type          arfcn; 
    uint8                           tsc;
    ftm_gsm_ber_arfcn_type          tch_arfcn;
    ftm_gsm_ber_tch_slot_type       tch_slot;
    ftm_gsm_ber_tch_mode_type       tch_mode;
    ftm_gsm_ber_loopback_type       loopback_type;
    ftm_gsm_ber_loopback_type       saved_loopback_type;
    ftm_gsm_ber_network_parms_type  network_parms;
    uint8                           tch_ordered_power_level;

} ftm_gsm_ber_status_type;


/* 
 * This type specifies the FTM<->L1 primitives.  These mimic the
 * RR<->L1 interface
 *
 */ 
typedef enum
{

    FTM_START_GSM_MODE_REQ       = MPH_START_GSM_MODE_REQ,
    FTM_STOP_GSM_MODE_REQ        = MPH_STOP_GSM_MODE_REQ,
    FTM_SELECT_SPECIFIC_BCCH_REQ = MPH_SELECT_SPECIFIC_BCCH_REQ,
    FTM_START_IDLE_REQ           = MPH_START_IDLE_REQ,
    FTM_POWER_SCAN_REQ           = MPH_POWER_SCAN_REQ,
    FTM_CHANNEL_ASSIGN_REQ       = MPH_CHANNEL_ASSIGN_REQ,
    FTM_CHANNEL_RELEASE_REQ      = MPH_CHANNEL_RELEASE_REQ,
    FTM_DEDICATED_UPDATE_REQ     = MPH_DEDICATED_UPDATE_REQ,
    FTM_DEDICATED_WCDMA_CELL_UPDATE_LIST_REQ = MPH_DEDICATED_WCDMA_CELL_UPDATE_LIST_REQ,
    FTM_SELECT_SPECIFIC_BCCH_CNF = MPH_SELECT_SPECIFIC_BCCH_CNF,
    FTM_START_GSM_MODE_CNF       = MPH_START_GSM_MODE_CNF,
    FTM_STOP_GSM_MODE_CNF        = MPH_STOP_GSM_MODE_CNF,
    FTM_CHANNEL_ASSIGN_CNF       = MPH_CHANNEL_ASSIGN_CNF,
    FTM_CHANNEL_RELEASE_CNF      = MPH_CHANNEL_RELEASE_CNF,
    FTM_POWER_SCAN_CNF           = MPH_POWER_SCAN_CNF,
    FTM_START_IDLE_MODE_CNF      = MPH_START_IDLE_MODE_CNF,
    FTM_SERVING_DED_MEAS_IND     = MPH_SERVING_DED_MEAS_IND,
    FTM_SURROUND_MEAS_IND        = MPH_SURROUND_MEAS_IND
} ftm_gsm_l1_msg_id_type;


/* 
 * This type specifies the 2nd set of FTM<->L1 primitives.  These mimic the
 * L2<->L1 interface
 *
 */ 
typedef enum
{
   FTM_PH_DATA_REQ                       = PH_DATA_REQ,
   FTM_PH_EMPTY_FRAME_REQ                = PH_EMPTY_FRAME_REQ,
   FTM_PH_RANDOM_ACCESS_REQ              = PH_RANDOM_ACCESS_REQ,
                                    
   FTM_PH_DATA_IND                       = PH_DATA_IND,
   FTM_PH_CONNECT_IND                    = PH_CONNECT_IND,
   FTM_PH_READY_TO_SEND_IND              = PH_READY_TO_SEND_IND,
   FTM_PH_RANDOM_ACCESS_CNF              = PH_RANDOM_ACCESS_CNF,
   FTM_PH_STOP_SACCH_MEASUREMENT_REPORT  = PH_STOP_SACCH_MEASUREMENT_REPORT
} ftm_gsm_l2_l1_msg_id_type;

/* This type specifies the possible L1->FTM messages (L1->L2 like)*/
typedef union
{

  IMH_T                   message_header;
  ph_data_ind_T           ph_data_ind;
  ph_connect_ind_T        ph_connect_ind;
  ph_ready_to_send_ind_T  ph_ready_to_send_ind;
  ph_random_access_cnf_T  ph_random_access_cnf;


} ftm_pseudo_l2_msg_type;

/* This type specifies the possible L1->FTM messages (L1->RR like) */
typedef union
{
  IMH_T                                  message_header;
  mph_serving_ded_meas_ind_T             mph_serving_ded_meas_ind;
  mph_serving_meas_ind_t                 mph_serving_meas_ind;
  mph_channel_assign_cnf_T               mph_channel_assign_cnf;
  mph_channel_release_cnf_T              mph_channel_release_cnf;
  mph_select_specific_bcch_cnf_T         mph_select_specific_bcch_cnf;
  /* mph_geran_to_ftm_padding_T is not used for messages but for adding to 
     this union to make it at least as big as the RR -> L1 union */
  mph_geran_to_ftm_padding_T             mph_geran_to_ftm_padding;

} ftm_pseudo_rr_msg_type;


/* This type specifies the possible messages delivered to the FTM BER task */
typedef union
{
        ftm_pseudo_rr_msg_type       ftm_pseudo_rr_msg;
        ftm_pseudo_l2_msg_type       ftm_pseudo_l2_msg;
} ftm_gsm_ber_msg;

/* This type specifies the possible queued messages delivered to the FTM BER 
 * task queue 
 */
typedef struct
{
    q_link_type            link;
    ftm_gsm_ber_msg        msg;
} ftm_gsm_ber_queue_type;

#define FTM_GSM_BER_RX_DATA_PAYLOAD_MAX_SIZE     44     /* covers GSM and EGPRS SRB => 348 bits */ 
#define FTM_GSM_BER_NUM_RX_DATA_MSG_BUFFERS      20
typedef struct
{
  uint32         fn;
  uint8          slot_num;
  boolean        bfi;
  uint16         num_bits;
  uint8          rx_data[FTM_GSM_BER_RX_DATA_PAYLOAD_MAX_SIZE];
} ftm_gsm_ber_rx_data_payload;

typedef struct
{
  q_link_type                   link;
  ftm_gsm_ber_rx_data_payload   msg;
} ftm_gsm_ber_rx_data_payload_msg_type;



#define MS_FTM_P_RR_L1   MS_RR_L1
#define MS_FTM_P_L2_L1   MS_L2_L1


typedef rr_l1_pscan_results_T   ftm_l1_pscan_results_T;


typedef mph_add_channel_assign_cnf_T               ftm_gsm_add_channel_assign_cnf_type;           
typedef mph_alternate_bcch_monitor_cnf_T           ftm_gsm_alternate_bcch_monitor_cnf_type;       

/* FTM GSM BER API definition (interface to PC) */
enum
{
    FTM_GSM_BER_START_GSM_MODE_REQ         = 0,
    FTM_GSM_BER_SELECT_SPECIFIC_BCCH_REQ     = 1,
    FTM_GSM_BER_START_IDLE_MODE_REQ          = 2,
    FTM_GSM_BER_CONFIG_LOOPBACK_TYPE_REQ     = 3,
    FTM_GSM_BER_CHANNEL_ASSIGN_REQ           = 4,
    FTM_GSM_BER_CHANNEL_RELEASE_REQ          = 5,
    FTM_GSM_BER_STOP_GSM_MODE_REQ            = 6,
    FTM_GSM_BER_CHANNEL_ASSIGN_V2_REQ        = 7,
    FTM_GSM_BER_EVENT_STATUS_CLEAR_REQ       = 8,
    FTM_GSM_BER_EVENT_STATUS_REQ             = 9,
    FTM_LOG_GSM_BER_RX_LEVEL_REQ       = 10,
    FTM_GSM_BER_INVALID_REQ

};


/* This type specifies the interface for the START_GSM_MODE_REQ */
typedef PACK(struct)
{
    diagpkt_subsys_header_type   header;
    ftm_cmd_header_type          ftm_cmd_header;

} ftm_gsm_ber_start_gsm_mode_req_type;

/* This type specifies the interface for the START_IDLE_MODE_REQ */
typedef ftm_gsm_ber_start_gsm_mode_req_type ftm_gsm_ber_start_idle_mode_req_type;

/* This type specifies the interface for the CHANNEL_RELEASE_REQ */
typedef ftm_gsm_ber_start_gsm_mode_req_type ftm_gsm_ber_channel_release_req_type;

/* This type specifies the interface for the STOP_GSM_MODE_REQ */
typedef ftm_gsm_ber_start_gsm_mode_req_type ftm_gsm_ber_stop_gsm_mode_req_type;

/* This type specifies the interface for the SELECT_SPECIFIC_BCCH_REQ */
typedef PACK(struct)
{

    diagpkt_subsys_header_type   header;
    ftm_cmd_header_type          ftm_cmd_header;
    uint16                       channel;
    uint8                        band;

} ftm_gsm_ber_select_specific_bcch_req_type;

/* This type specifies the interface for the CONFIG_LOOPBACK_REQ */
typedef PACK(struct)
{

    diagpkt_subsys_header_type   header;
    ftm_cmd_header_type          ftm_cmd_header;
    uint8                        loopback_type;

} ftm_gsm_ber_config_loopback_req_type;

/* This type specifies the interface for the CHANNEL_ASSIGN_REQ */
typedef PACK(struct)
{

    diagpkt_subsys_header_type   header;
    ftm_cmd_header_type          ftm_cmd_header;
    uint8                        slot;
    uint8                        tch_mode;

} ftm_gsm_ber_channel_assign_req_type;


/* This type specifies the interface for the CHANNEL_ASSIGN_V2_REQ */

typedef PACK(struct)
{

    diagpkt_subsys_header_type   header;
    ftm_cmd_header_type          ftm_cmd_header;
    uint16                       channel;
    uint8                        band;
    uint8                        tsc;
    uint8                        slot;
    uint8                        tch_mode;
    uint8                        power_level;

} ftm_gsm_ber_channel_assign_req_v2_type;





/* Asynchronous logging interface */

/* This type specifies the possible asynchronous event packet types */
typedef enum
{

    FTM_LOG_GSM_BER_START_GSM_MODE_CNF        = 0,
    FTM_LOG_GSM_BER_SELECT_SPECIFIC_BCCH_CNF  = 1,
    FTM_LOG_GSM_BER_START_IDLE_MODE_CNF       = 2,
    FTM_LOG_GSM_BER_CHANNEL_ASSIGN_CNF        = 3,
    FTM_LOG_GSM_BER_CHANNEL_RELEASE_CNF       = 4,
    FTM_LOG_GSM_BER_STOP_GSM_MODE_CNF         = 5,
    FTM_LOG_GSM_BER_PH_DATA_IND               = 6,
    FTM_LOG_GSM_BER_DED_RPT                   = 7

} ftm_log_gsm_ber_id_type;

/* This type specifies the possible synchronous status EVENT_ID types for the 
   FTM_GSM_BER_EVENT_STATUS_REQ command
*/
typedef enum
{

   FTM_GSM_BER_START_GSM_MODE_SYNC_STATUS             = 0,
   FTM_GSM_BER_SELECT_SPECIFIC_BCCH_CNF_SYNC_STATUS   = 1,
   FTM_GSM_BER_START_IDLE_MODE_SYNC_STATUS            = 2,
   FTM_GSM_BER_CHANNEL_ASSIGN_SYNC_STATUS             = 3,
   FTM_GSM_BER_CHANNEL_RELEASE_SYNC_STATUS            = 4,
   FTM_GSM_BER_STOP_GSM_MODE_SYNC_STATUS              = 5,
   FTM_GSM_BER_SACCH_SYNC_STATUS                      = 6,

  // Insert new values aboive this one
  FTM_GSM_BER_SYNC_STATUS_MAX

} ftm_log_gsm_ber_sync_event_id_type;

/* This type specifies the synchronous status types reported using  
   the FTM_GSM_BER_EVENT_STATUS_REQ command 
*/
typedef enum
{

    FTM_GSM_BER_SYNC_STATUS_SUCCESS             = 0,
    FTM_GSM_BER_SYNC_STATUS_GEN_FAILURE         = 1,
    FTM_GSM_BER_SYNC_STATUS_TIMEOUT_EXPIRED     = 2,
    FTM_GSM_BER_SYNC_STATUS_EVENT_STATUS_FAILED = 3

} ftm_gsm_ber_sync_status_type;

/* This type specifies the FTM GSM BER generic confirmation event packet */
typedef PACK(struct)
{

    ftm_log_gen_type           header;
    ftm_log_gsm_ber_id_type    log_id;

} ftm_log_gsm_ber_gen_cnf;

/* This type specifies the FTM GSM BER generic confirmation event with status packet */
typedef PACK(struct)
{

    ftm_log_gen_type           header;
    ftm_log_gsm_ber_id_type    log_id;
    uint8                      status;

} ftm_log_gsm_ber_gen_cnf_with_status;

/* This type specifies the FTM GSM BER select specific bcch confirmation event */
typedef PACK(struct)
{

    ftm_log_gen_type           header;
    ftm_log_gsm_ber_id_type    log_id;
    boolean                    status;
    byte                       sch_data[SCH_BLOCK_SIZE];

} ftm_log_gsm_ber_select_specific_bcch_cnf_type;

/* This type specifies the request structure for FTM_GSM_BER_EVENT_STATUS_REQ */
typedef PACK(struct)
{
    diagpkt_subsys_header_type        header;
    ftm_cmd_header_type               ftm_cmd_header;
    int8                              event_type;
    uint32                            timeout_ms;

} ftm_gsm_ber_event_status_req_type;

/* This type specifies the FTM GSM BER ph data indication event */
typedef PACK(struct)
{

    ftm_log_gen_type           header;
    ftm_log_gsm_ber_id_type    log_id;
    l2_channel_type_T          channel_type;
    boolean                    crc_pass;

} ftm_log_gsm_ber_ph_data_ind_type;

/* This type specifies the FTM GSM BER dedicated mode report event */
typedef PACK(struct)
{
    ftm_log_gen_type           header;
    ftm_log_gsm_ber_id_type    log_id;
    uint8                      RxLev_avg;
    uint8                      RxQual_avg;
   
} ftm_log_gsm_ber_ded_rpt_type;

enum
{
    FTM_GSM_BER_GEN_RESPONSE = 0x1001
};

/* This type specifies the FTM GSM BER response header packet */
typedef PACK(struct)
{
    uint16                     rsp_id;
    uint16                     rsp_cid;
    uint16                     rsp_scid;

} ftm_gsm_ber_rsp_header_type;

/* This type specifies the FTM GSM BER response packet */
typedef PACK(struct)
{

    diagpkt_subsys_header_type   header;
    ftm_gsm_ber_rsp_header_type  ftm_rsp_header;
    uint16                       status;

} ftm_gsm_ber_gen_rsp_pkt_type;


typedef PACK(struct)
{
  uint32                       rssi;
  int16                        rx_agc_dbm16;
  uint32                       rssi_drx;
  int16                        rx_agc_dbm16_drx;
}ftm_gsm_ber_rx_level_rpt_type;

/* This type specifies the FTM GSM BER Rx Level response packet */
typedef PACK(struct)
{
    diagpkt_subsys_header_type     header;
    ftm_gsm_ber_rsp_header_type    ftm_rsp_header;
    uint16                         status;
    ftm_gsm_ber_rx_level_rpt_type  rx_lev_rpt;
}ftm_gsm_ber_rx_lev_rsp_pkt_type;

typedef struct
{
  uint32   fn;
  uint8    slot_num;
  boolean  bfi;
  uint16   num_bits;
  uint8    *data;
} ftm_gsm_ber_rx_data_ind_type;



boolean ftm_gsm_ber_cleanup( rfm_device_enum_type rfm_dev );

void ftm_gsm_ber_init( void);

void ftm_gsm_ber_set_dev_state (rfm_device_enum_type rfm_dev, ftm_gsm_ber_state_type new_state);

ftm_gsm_ber_state_type ftm_gsm_ber_get_dev_state( rfm_device_enum_type rfm_dev );


/*===========================================================================

FUNCTION        ftm_gsm_ber_handle_rx_data_indications

DESCRIPTION     Handles raw data sent by L1 for use with single ended BER
                subsystem.
.

DEPENDENCIES    ftm_gsm_ber_init() called.
 

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_gsm_ber_handle_rx_data_indications(void );

/*===========================================================================

FUNCTION        FTM_EGPRS_BER_SEND_MSG_TO_FTM

DESCRIPTION     Takes a populated message and sends it to the FTM task via
                the q_xxxx() interfaces (uses command Q).  Sets signal for FTM task to 
                handle the message.
.

DEPENDENCIES    buffer has been previously "allocated" using 
                ftm_egprs_ber_get_cmd_buf()
   

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_gsm_ber_send_rx_data_to_ftm( const ftm_gsm_ber_rx_data_ind_type *rx_data_ind);

/*===========================================================================

FUNCTION       ftm_gsm_set_exit_cmd_sig

DESCRIPTION    This function sets the FTM_GSM_BER_EXIT_CMD_Q_SIG
               signal which is called by GL1 and when complete
               GSM tech exit is done
.

DEPENDENCIES    
   None.
   

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/

void ftm_gsm_set_exit_cmd_sig( void );


#endif /* FTM_GSM_BER_H */
