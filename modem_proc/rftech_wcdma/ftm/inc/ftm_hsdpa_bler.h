/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2002 by Qualcomm Technologies INCORPORATED.
  Copyright (c) 2013 by Qualcomm Technologies INCORPORATED.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/ftm/inc/ftm_hsdpa_bler.h#1 $
  $DateTime: 2016/12/13 07:59:54 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/21/12   kai     Add internal definition of the max number of HARQ processes, remove the featurization
03/28/12   kai     Changed the max number of HARQ processes is 16
03/26/12   kai     MIMO feature related change in the structure ftm_hsdpa_bler_harq_process_info_type 
02/13/12   swb     Added API to update L1 parameters for HSDPA reconfigure during HO 
05/05/11   dw      CMI violation fix 
10/21/10   ems     Factory features Rx Lvl, non-signaling events and SE/BER
02/03/10   kguo    Changed PACKED struct to PACK(struct) 
07/20/05   dp      Add ability to request stop/start of HS config, before and
                   after a HHO.
06/08/05   dp      Add status byte in FTM HSDPA Log.
05/19/05   dp      Add reconfig indication log.
03-21-05   dp      Created
===========================================================================*/
#ifndef FTM_HSDPA_BLER_H
#define FTM_HSDPA_BLER_H

/*===========================================================================

          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "rfa_variation.h"
#include "target.h"
#include "customer.h"
#include "ftm.h"
#include "ftm_wcdma_ber.h"
#include "ftmdiag.h"
#include "queue.h"
#include "rrccmd.h"
#include "ftm_log.h"


#ifdef FEATURE_FACTORY_TESTMODE
#ifdef FEATURE_FTM_WCDMA_BER
#ifdef FEATURE_HSDPA
/*===========================================================================

                         EXTERNAL GLOBAL VARIABLES

===========================================================================*/

/*===========================================================================

                    LOCAL GLOBAL VARIABLES AND CONSTANTS

===========================================================================*/

/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/

typedef enum
{
    FTM_HSDPA_BLER_HS_CHAN_STATE_INACTIVE,
    FTM_HSDPA_BLER_HS_CHAN_STATE_ACTIVE

} ftm_hsdpa_bler_hs_chan_state_type;

typedef enum
{
    FTM_HSDPA_BLER_HS_FORCE_STOP__NO_ERROR = 0,
    FTM_HSDPA_BLER_HS_FORCE_STOP__ERROR_OP_IN_PROGRESS,
    FTM_HSDPA_BLER_HS_FORCE_STOP__ERROR_HS_INACTIVE

} ftm_hsdpa_bler_hs_force_stop_error_type;



typedef struct
{

    boolean                            isInitialized;
    ftm_hsdpa_bler_hs_chan_state_type  current_state;
    hsdpa_setup_ops_enum_type          current_op;
    boolean                            hs_config_to_resume_after_recfg;
    l1_hsdpa_info_struct_type          *ftm_hsdpa_info;

} ftm_hsdpa_bler_db_type;

/* This type specifies the FTM HS-DPCCH configuration parameters */

typedef PACK(struct)
{

    int8         cpich_power_offset;
    uint8        cqi_feedback_cycle;
    uint8        cqi_repetition_factor;
    uint8        delta_cqi;
    uint8        delta_ack;
    uint8        delta_nack;
    uint8        ack_nack_repetition_factor;

} ftm_hsdpa_bler_hs_dpcch_config_type;

/* This type specifies the FTM HS-PDSCH configuration parameters */

typedef PACK(struct)
{

    uint16       hs_pdsch_radio_link_psc;

} ftm_hsdpa_bler_hs_pdsch_config_type;

/* This type specifies the FTM HS-SCCH configuration parameters */

typedef PACK(struct)
{

    uint8        num_channelisation_codes;   /* 1 to 4 */
    uint8        channelisation_codes[1];    /* placeholder */

} ftm_hsdpa_bler_hs_scch_config_type;

/* Maximum number of HARQ processes */
#define L1_DL_HS_DSCH_HARQ_MAX_PROC_INTERNAL 16

typedef PACK(struct)
{

    uint8                                      num_harq_processes;
    boolean                                    mem_partition_valid;

    l1_dl_hs_dsch_harq_mem_sz_enum_type        mem_size[L1_DL_HS_DSCH_HARQ_MAX_PROC_INTERNAL];          /* max number of HARQ processes is internally defined to 16 */
} ftm_hsdpa_bler_harq_process_info_type;

/* This type specifies the FTM HS-DSCH configuration parameters */

typedef PACK(struct)
{

    uint16                                      h_rnti;
    ftm_hsdpa_bler_harq_process_info_type       harq_info;

} ftm_hsdpa_bler_hs_dsch_config_type;




/* Interface to PC */

typedef PACK(struct)
{

    diagpkt_subsys_header_type   header;
    ftm_cmd_header_type          ftm_cmd_header;
    byte                         data[1];

} ftm_hsdpa_bler_pkt_type;


/* This type specifies the interface for the Start HS channel command */

typedef PACK(struct)
{

    diagpkt_subsys_header_type   header;
    ftm_cmd_header_type          ftm_cmd_header;

} ftm_hsdpa_bler_start_hs_cmd_type;


/* This type specifies the interface for the Stop HS channel command */

typedef ftm_hsdpa_bler_start_hs_cmd_type   ftm_hsdpa_bler_stop_hs_cmd_type;


/* This type specifies the interface for the Reconfig HS channel command */

typedef PACK(struct)
{

    diagpkt_subsys_header_type   header;
    ftm_cmd_header_type          ftm_cmd_header;
    uint8                        req_mask;

} ftm_hsdpa_bler_reconfig_hs_cmd_type;

/* This type specifies the interface for the Config HS-PDSCH command */

typedef PACK(struct)
{

    diagpkt_subsys_header_type            header;
    ftm_cmd_header_type                   ftm_cmd_header;
    ftm_hsdpa_bler_hs_pdsch_config_type   config;

} ftm_hsdpa_bler_config_hs_pdsch_cmd_type;


/* This type specifies the interface for the Config HS-SCCH command */

typedef PACK(struct)
{

    diagpkt_subsys_header_type              header;
    ftm_cmd_header_type                     ftm_cmd_header;
    ftm_hsdpa_bler_hs_scch_config_type      config;

} ftm_hsdpa_bler_config_hs_scch_cmd_type;

/* This type specifies the interface for the Config HS-DSCH command */

typedef PACK(struct)
{

    diagpkt_subsys_header_type            header;
    ftm_cmd_header_type                   ftm_cmd_header;
    ftm_hsdpa_bler_hs_dsch_config_type    config;

} ftm_hsdpa_bler_config_hs_dsch_cmd_type;

/* This type specifies the interface for the Config HS-DPCCH command */

typedef PACK(struct)
{

    diagpkt_subsys_header_type            header;
    ftm_cmd_header_type                   ftm_cmd_header;
    ftm_hsdpa_bler_hs_dpcch_config_type   config;

} ftm_hsdpa_bler_config_hs_dpcch_cmd_type;

/* This type specifies the request structure for FTM_HSDPA_BLER_EVENT_STATUS_REQ */

typedef PACK(struct)
{
    diagpkt_subsys_header_type        header;
    ftm_cmd_header_type               ftm_cmd_header;
    int8                              event_type;
    uint32                            timeout_ms;

} ftm_hsdpa_bler_event_status_req_type;

enum
{
    FTM_HSDPA_BLER_GEN_RESPONSE = 0x1003
};

/* This type specifies the FTM WCDMA BER response header packet */
typedef PACK(struct)
{
    uint16                     rsp_id;
    uint16                     rsp_cid;
    uint16                     rsp_scid;

} ftm_hsdpa_bler_rsp_header_type;

/* This type specifies the FTM WCDMA BER response packet */
typedef PACK(struct)
{

    diagpkt_subsys_header_type      header;
    ftm_hsdpa_bler_rsp_header_type  ftm_rsp_header;
    uint16                          status;

} ftm_hsdpa_bler_gen_rsp_pkt_type;

/* Command codes for the FTM HSDPA BER feature */

enum
{

    FTM_HSDPA_BLER_CMD_START_HS                  = FTM_WCDMA_BER_HSDPA_CMD_BASE,
    FTM_HSDPA_BLER_CMD_STOP_HS,
    FTM_HSDPA_BLER_CMD_RECONFIG_HS,
    FTM_HSDPA_BLER_CMD_CONFIG_HS_PDSCH,
    FTM_HSDPA_BLER_CMD_CONFIG_HS_SCCH,
    FTM_HSDPA_BLER_CMD_CONFIG_HS_DSCH,
   FTM_HSDPA_BLER_CMD_CONFIG_HS_DPCCH,
   FTM_HSDPA_BLER_EVENT_STATUS_CLEAR_REQ,
   FTM_HSDPA_BLER_EVENT_STATUS_REQ

};



/* FTM WCDMA BER Asynchronous Logging Interface */

/* Asynchronous logging interface */

/* This type specifies the possible asynchronous event packet types */
typedef enum
{

    FTM_LOG_HSDPA_HS_CHANNEL_START_IND = FTM_LOG_HSDPA_LOG_ID_BASE,
    FTM_LOG_HSDPA_HS_CHANNEL_STOP_IND,
    FTM_LOG_HSDPA_HS_CHANNEL_RECFG_IND


} ftm_log_hsdpa_bler_id_type;

/* This type specifies the possible synchronous status EVENT_ID types for the 
   FTM_HSDPA_BLER_EVENT_STATUS_REQ command
*/
typedef enum
{
    FTM_HSDPA_BLER_HS_CHANNEL_START_SYNC_STATUS = 0,
    FTM_HSDPA_BLER_HS_CHANNEL_STOP_SYNC_STATUS  = 1,
    FTM_HSDPA_BLER_HS_CHANNEL_RECFG_SYNC_STATUS = 2,

	// Insert new values aboive this one
	FTM_HSDPA_BLER_SYNC_STATUS_MAX

} ftm_log_hsdpa_bler_sync_event_id_type;

/* This type specifies the synchronous status types reported using  
   the FTM_HSDPA_BLER_EVENT_STATUS_REQ command 
*/
typedef enum
{

    FTM_HSDPA_BLER_SYNC_STATUS_SUCCESS             = 0,
    FTM_HSDPA_BLER_SYNC_STATUS_GEN_FAILURE         = 1,
    FTM_HSDPA_BLER_SYNC_STATUS_TIMEOUT_EXPIRED     = 2,
    FTM_HSDPA_BLER_SYNC_STATUS_EVENT_STATUS_FAILED = 3

} ftm_hsdpa_bler_sync_status_type;

/* This type specifies the FTM HSDPA BER generic confirmation event packet */
typedef PACK(struct)
{

    ftm_log_gen_type            header;
    ftm_log_hsdpa_bler_id_type  log_id;

} ftm_log_hsdpa_bler_gen_cnf;

/* This type specifies the FTM WCDMA BER generic confirmation event with status packet */
typedef PACK(struct)
{

    ftm_log_gen_type            header;
    ftm_log_hsdpa_bler_id_type  log_id;
    uint8                       status;

} ftm_log_hsdpa_bler_gen_cnf_with_status;



ftm_rsp_pkt_type ftm_hsdpa_bler_dispatch( ftm_hsdpa_bler_pkt_type *pkt);


void ftm_hsdpa_bler_init( void);

void ftm_hsdpa_bler_process_l1_messages( rrc_cmd_type *msg);
ftm_hsdpa_bler_hs_force_stop_error_type ftm_hsdpa_bler_request_stop_hs_channel( l1_setup_cmd_type *cmd);
void ftm_hsdpa_bler_request_resume_hs_channel( void);
void ftm_hsdpa_bler_update_reconfig_hs_channel_parameters (l1_setup_cmd_type *cmd);

#endif /* FEATURE_HSDPA */
#endif /* FEATURE_FTM_WCDMA_BER */
#endif /* FEATURE_FACTORY_TESTMODE */
#endif /* FTM_WCDMA_BER_H */
