/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

       F T M  H W T C  D V T  S T R I N G  C O M M A N D S  M O D U L E

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2002 by Qualcomm Technologies INCORPORATED.
  Copyright (c) 2003 by Qualcomm Technologies INCORPORATED.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
 
                           EDIT HISTORY FOR MODULE
                           
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/gsm/ftm_egprs_ber.h#1 $ 
  $DateTime: 2016/12/13 07:59:52 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/29/13   ec      Add framework for Non Signalling on multiple RF Devices 
11/21/12   tws     Remove legacy GERAN API includes.
11/23/11   sr      removed GSDI related include files.
11/07/11   ars     Fix for EGPRS Rx Level and SE/BER 
04/05/11   sar     Removed conditional #def's to comply with CMI. 
11/01/10   ems     Support for FTM Sequencer 
01/20/10   tws     Use PACK(...) instead of PACKED for qdsp6 compatibility.
10/22/07   ad      SE BER support from 6260: Add V2 command for SRB 
                   configuration (more UL/DL slots)
11-02-05   dp      Add gamma as a parameter for each uplink slot.
06-08-05   dp      Created
===========================================================================*/
#ifndef FTM_EGPRS_BER_H
#define FTM_EGPRS_BER_H


/*===========================================================================

          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"
#include "customer.h"

#include "ftm.h"
#include "ftmdiag.h"

#include "log.h"
#include "ftm_log.h"
#include "geran_test_mode_api.h"


#define FTM_EGPRS_BER_MAX_DL_UL_SLOTS      2
#define FTM_EGPRS_BER_MAX_DL_UL_SLOTS_V2   8       /* open up interface to allow 8 down/8 up.  User must obey class 
                                                    * limitations of the device (example class 10 = 4 down/2up/5total)
                                                    */
#define FTM_EGPRS_BER_NUM_CMD_BUFFERS   10


/* This tpe specifies the states of the FTM EGPRS SRB loopback scenario */

typedef enum
{

    FTM_EGPRS_BER_STATE_NULL,                   /* No (E)GPRS specific operation in progress */
    FTM_EGPRS_BER_STATE_GPRS_IDLE_PENDING,      /* Sent Start GPRS IDLE command, waiting confirmation */
    FTM_EGPRS_BER_STATE_GPRS_IDLE,              /* In GPRS IDLE mode (decoding CCCH) */
    FTM_EGPRS_BER_STATE_DL_TBF_ESTABLISHING,    /* Establishing DL TBF...no uplink */
    FTM_EGPRS_BER_STATE_DL_TBF_ESTABLISHED,     /* Established DL TBF...no uplink */
    FTM_EGPRS_BER_STATE_UL_TBF_ESTABLISHING,    /* Established DL TBF...establishing UL TBF */
    FTM_EGPRS_BER_STATE_UL_TBF_ESTABLISHED,     /* Established DL TBF...established UL TBF */
    FTM_EGPRS_BER_STATE_DL_TBF_RELEASING,       /* Releasing prev. established DL TBF...no uplink */
    FTM_EGPRS_BER_STATE_UL_DL_TBF_RELEASING,    /* Releasing prev. established UL and DL TBF */
    FTM_EGPRS_BER_NUM_STATES

} ftm_egprs_ber_state_type;


/* This type specifies the reconfig ("handover") state for one-shot reconfiguration of TBFs */
/* Note these states are not currently used! */
typedef enum
{

    FTM_EGPRS_BER_RECONFIG_STATE_NULL,
    FTM_EGPRS_BER_RECONFIG_STATE_RELEASING,
    FTM_EGPRS_BER_RECONFIG_STATE_ESTABLISHING

} ftm_egprs_ber_reconfig_state_type;

/* This type specifies the ARFCN used to setup the Rx/Tx frequencies during SRB loop */

typedef PACK(struct)
{
    uint16      channel;
    uint8       band;

} ftm_egprs_ber_arfcn_type;

/* This type specifies a slot configuration, containing a timeslot (0-7) and an
 * active flag */

typedef PACK(struct)
{

    boolean     slot_active;
    uint8       slot_number;

} ftm_egprs_ber_dl_slot_config_type;


typedef PACK(struct)
{

    uint8       ul_gamma;
    boolean     slot_active;
    uint8       slot_number;

} ftm_egprs_ber_ul_slot_config_type;

/* This type specifies a downlink config buffer structure */

typedef struct
{
    uint8                             dl_tsc;   
    uint8                             dl_tfi;
    ftm_egprs_ber_arfcn_type          dl_arfcn;
    ftm_egprs_ber_dl_slot_config_type dl_slot_config[ FTM_EGPRS_BER_MAX_DL_UL_SLOTS_V2];  //allow configuration of up to 2 slots

} ftm_egprs_ber_dl_tbf_status_type;

/* This type specifies an uplink config buffer structure */

typedef struct
{

    uint8                              ul_tsc;
    uint8                              usf;
    ftm_egprs_ber_ul_slot_config_type  ul_slot_config[ FTM_EGPRS_BER_MAX_DL_UL_SLOTS_V2]; //allow configuration of up to 2 slots


} ftm_egprs_ber_ul_tbf_status_type;

typedef struct
{

    /* current state of FTM EGPRS scenario */

    ftm_egprs_ber_state_type           current_state;
    ftm_egprs_ber_reconfig_state_type  current_reconfig_state;

    /* Queues and buffers for passing messages from L1 to FTM (rather than MAC) */

    q_type                             ftm_egprs_ber_cmd_q;
    q_type                             ftm_egprs_ber_cmd_free_q;
    geran_test_mode_api_l1_mac_sig_t   ftm_egprs_ber_l1_mac_cmd_buffers[ FTM_EGPRS_BER_NUM_CMD_BUFFERS];
    
    /* Buffer for DL and UL TBF configuration parameters that are controllable via FTM interface */
    
    ftm_egprs_ber_dl_tbf_status_type   current_dl_tbf_info;
    ftm_egprs_ber_ul_tbf_status_type   current_ul_tbf_info;

    /* Buffers the DL and UL TBF configurations that get sent to L1 */

    mac_dynamic_tbf_information_T      current_l1_dynamic_tbf_info;
    mac_l1_dl_tbf_config_t             current_l1_dl_tbf_info;


} ftm_egprs_ber_status_info_type;



/* Interface to PC */

typedef PACK(struct)
{

    diagpkt_subsys_header_type   header;
    ftm_cmd_header_type          ftm_cmd_header;
    byte                         data[1];

} ftm_egprs_ber_pkt_type;


/* FTM EGPRS BER API definition (interface to PC) */

enum
{
    FTM_EGPRS_BER_CMD_BASE                 = 256,
    FTM_EGPRS_BER_START_GPRS_IDLE_MODE_REQ = FTM_EGPRS_BER_CMD_BASE,
    FTM_EGPRS_BER_ESTABLISH_SRB_LOOPBACK,
    FTM_EGPRS_BER_CONFIGURE_DL_TBF,
    FTM_EGPRS_BER_CONFIGURE_UL_TBF,
    FTM_EGPRS_BER_RELEASE_ALL_TBF,
    FTM_EGPRS_BER_RELEASE_UL_TBF,
    FTM_EGPRS_BER_CONFIGURE_DL_TBF_V2,      /* expanded num of allowed slots */   
    FTM_EGPRS_BER_CONFIGURE_UL_TBF_V2, 
    FTM_EGPRS_BER_RESERVED_1,               /* Allow for two obsolete commands */
    FTM_EGPRS_BER_RESERVED_2,       
    FTM_EGPRS_BER_EVENT_STATUS_CLEAR_REQ,
    FTM_EGPRS_BER_EVENT_STATUS_REQ

};


/* This type specifies the interface for the START_GPRS_IDLE_MODE_REQ */

typedef PACK(struct)
{
    diagpkt_subsys_header_type   header;
    ftm_cmd_header_type          ftm_cmd_header;

} ftm_egprs_ber_start_gprs_idle_mode_req_type;


/* This type specifies the interface for the CONFIGURE_DL_TBF command */

typedef PACK(struct)
{

    diagpkt_subsys_header_type        header;
    ftm_cmd_header_type               ftm_cmd_header;
    uint8                             dl_tsc;
    uint8                             dl_tfi;
    ftm_egprs_ber_arfcn_type          dl_arfcn;
    ftm_egprs_ber_dl_slot_config_type dl_slot_config[ FTM_EGPRS_BER_MAX_DL_UL_SLOTS_V2];

} ftm_egprs_ber_configure_dl_tbf_type;

/* This type specifies the interface for the CONFIGURE_UL_TBF command */

typedef PACK(struct)
{

    diagpkt_subsys_header_type         header;
    ftm_cmd_header_type                ftm_cmd_header;
    uint8                              ul_tsc;
    uint8                              usf;
    ftm_egprs_ber_ul_slot_config_type  ul_slot_config[ FTM_EGPRS_BER_MAX_DL_UL_SLOTS_V2];

} ftm_egprs_ber_configure_ul_tbf_type;

/* This type specifies the request structure for FTM_EGPRS_BER_EVENT_STATUS_REQ */
typedef PACK(struct)
{
    diagpkt_subsys_header_type        header;
    ftm_cmd_header_type               ftm_cmd_header;
    int8                              event_type;
    uint32                            timeout_ms;

} ftm_egprs_ber_event_status_req_type;




/* Response API  */


enum
{
    FTM_EGPRS_BER_GEN_RESPONSE = 0x1004
};

/* This type specifies the FTM EGPRS BER response header packet */

typedef PACK(struct)
{
    uint16                     rsp_id;
    uint16                     rsp_cid;
    uint16                     rsp_scid;

} ftm_egprs_ber_rsp_header_type;

/* This type specifies the FTM EGPRS BER response packet */

typedef PACK(struct)
{

    diagpkt_subsys_header_type     header;
    ftm_egprs_ber_rsp_header_type  ftm_rsp_header;
    uint16                         status;

} ftm_egprs_ber_gen_rsp_pkt_type;


/* Logging API */

/* This type specifies the log events associated with a TBF */

typedef enum
{
    
    FTM_LOG_EGPRS_TBF_EVENT_ESTABLISHED,
    FTM_LOG_EGPRS_TBF_EVENT_RELEASED,
    FTM_LOG_EGPRS_TBF_EVENT_ERROR

} ftm_log_egprs_tbf_event_type;

/* This type specifies the types of TBFs that we generate events for */

typedef enum
{
    FTM_LOG_EGPRS_BER_TBF_DL,
    FTM_LOG_EGPRS_BER_TBF_UL

} ftm_log_egprs_tbf_type;

/* This type specifies the logging codes used for asynchronous FTM Logs */

typedef enum
{

    FTM_LOG_EGPRS_BER_TBF_IND           = 0x40,
    FTM_LOG_EGPRS_BER_MAC_PH_DATA_IND,
    FTM_LOG_EGPRS_BER_SERVING_MEAS_IND,

    FTM_LOG_EGPRS_BER_INVALID_LOG_CODE

} ftm_log_egprs_ber_id_type;

/* This type specifies the possible synchronous status EVENT_ID types for the 
   FTM_EGPRS_BER_EVENT_STATUS_REQ command
*/
typedef enum
{

   FTM_EGPRS_BER_UL_TBF_ESTABLISHED_SYNC_STATUS = 0,
   FTM_EGPRS_BER_UL_TBF_RELEASED_SYNC_STATUS    = 1,
   FTM_EGPRS_BER_DL_TBF_ESTABLISHED_SYNC_STATUS = 2,
   FTM_EGPRS_BER_DL_TBF_RELEASED_SYNC_STATUS    = 3,

  // Insert new values aboive this one
  FTM_EGPRS_BER_SYNC_STATUS_MAX

} ftm_log_egprs_ber_sync_event_id_type;

/* This type specifies the synchronous status types reported using  
   the FTM_GSM_BER_EVENT_STATUS_REQ command 
*/
typedef enum
{

    FTM_EGPRS_BER_SYNC_STATUS_SUCCESS             = 0,
    FTM_EGPRS_BER_SYNC_STATUS_GEN_FAILURE         = 1,
    FTM_EGPRS_BER_SYNC_STATUS_TIMEOUT_EXPIRED     = 2,
    FTM_EGPRS_BER_SYNC_STATUS_EVENT_STATUS_FAILED = 3

} ftm_egprs_ber_sync_status_type;

/* This type specifies the FTM EGPRS BER generic confirmation event packet */

typedef PACK(struct)
{

    ftm_log_gen_type             header;
    ftm_log_egprs_ber_id_type    log_id;

} ftm_log_egprs_ber_gen_cnf;

/* This type specifies the FTM EGPRS BER generic confirmation event with status packet */

typedef PACK(struct)
{

    ftm_log_gen_type           header;
    ftm_log_egprs_ber_id_type  log_id;
    uint8                      status;

} ftm_log_egprs_ber_gen_cnf_with_status;

/* This type specifies the FTM EGPRS BER tbf indication */

typedef PACK(struct)
{

    ftm_log_gen_type               header;
    ftm_log_egprs_ber_id_type      log_id;
    ftm_log_egprs_tbf_type         tbf_type;
    ftm_log_egprs_tbf_event_type   tbf_event;

} ftm_log_egprs_ber_tbf_ind_type;

/* This type specifies the FTM EGPRS BER serving meas indication */

typedef PACK(struct)
{

    ftm_log_gen_type               header;
    ftm_log_egprs_ber_id_type      log_id;
    uint8                          rxlev_average;

} ftm_log_egprs_ber_serving_meas_ind_type;


/*===========================================================================
 
                         EXTERNAL GLOBAL VARIABLES

===========================================================================*/

/*===========================================================================
 
                    LOCAL GLOBAL VARIABLES AND CONSTANTS

===========================================================================*/

/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION       FTM_EGPRS_BER_GET_STATE

DESCRIPTION    Returns current state of FTM EGPRS BER operations.


DEPENDENCIES
  none

===========================================================================*/
ftm_egprs_ber_state_type ftm_egprs_ber_get_state(void);

/*===========================================================================

FUNCTION       FTM_EGPRS_BER_GET_DEV_STATE

DESCRIPTION    Returns current state of FTM EGPRS BER operations for a specific
				device.


DEPENDENCIES
  none

===========================================================================*/
ftm_egprs_ber_state_type ftm_egprs_ber_get_dev_state( rfm_device_enum_type rfm_dev );



/* Message sending/handling */

void ftm_egprs_ber_handle_msgs_from_gl1( void *msg);
void ftm_egprs_ber_handle_msgs_from_gpl1( void);
void *ftm_egprs_ber_get_cmd_buf( void);
void ftm_egprs_ber_send_msg_to_ftm( void *);
void ftm_egprs_ber_init( void);

/* Dispatch prototype - takes rfm_dev type  */
ftm_rsp_pkt_type ftm_egprs_ber_dispatch( ftm_egprs_ber_pkt_type *pkt, rfm_device_enum_type rfm_dev );

#endif /* FTM_EGPRS_BER_H */
