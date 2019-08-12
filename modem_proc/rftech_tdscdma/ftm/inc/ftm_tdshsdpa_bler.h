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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/ftm/inc/ftm_tdshsdpa_bler.h#1 $
  $DateTime: 2016/12/13 07:59:50 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/23/12   hy      Added support for TDSCDMA HSDPA Non-signaling
===========================================================================*/
#ifndef FTM_TDSHSDPA_BLER_H
#define FTM_TDSHSDPA_BLER_H

/*===========================================================================

          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "rfa_variation.h"
#include "target.h"
#include "customer.h"
#include "ftm.h"
#include "ftm_tdscdma_ber.h"
#include "ftmdiag.h"
#include "queue.h"
#include "tdsrrccmd.h"
#include "ftm_log.h"


#ifdef FEATURE_FACTORY_TESTMODE
#ifdef FTM_HAS_TDSCDMA_BER
#ifdef FTM_HAS_TDS_HSDPA
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
    FTM_TDSHSDPA_BLER_HS_CHAN_STATE_INACTIVE,
    FTM_TDSHSDPA_BLER_HS_CHAN_STATE_ACTIVE

} ftm_tdshsdpa_bler_hs_chan_state_type;

typedef enum
{
    FTM_TDSHSDPA_BLER_HS_FORCE_STOP__NO_ERROR = 0,
    FTM_TDSHSDPA_BLER_HS_FORCE_STOP__ERROR_OP_IN_PROGRESS,
    FTM_TDSHSDPA_BLER_HS_FORCE_STOP__ERROR_HS_INACTIVE

} ftm_tdshsdpa_bler_hs_force_stop_error_type;



typedef struct
{

    boolean                            isInitialized;
    ftm_tdshsdpa_bler_hs_chan_state_type  current_state;
    tdsl1_hsdpa_setup_ops_enum_type          current_op;
    boolean                            hs_config_to_resume_after_recfg;
    tdsl1_hs_info_struct_type          *ftm_tdshsdpa_info;

} ftm_tdshsdpa_bler_db_type;

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

} ftm_tdshsdpa_bler_hs_dpcch_config_type;

/* This type specifies the FTM HS-PDSCH configuration parameters */

typedef PACK(struct)
{

    uint16       hs_pdsch_radio_link_psc;

} ftm_tdshsdpa_bler_hs_pdsch_config_type;

/* This type specifies the FTM HS-SCCH configuration parameters */

typedef PACK(struct)
{

    uint8        num_channelisation_codes;   /* 1 to 4 */
    uint8        channelisation_codes[TDS_MAX_HS_SCCH_SICH_COUNT];    /* placeholder */

} ftm_tdshsdpa_bler_hs_scch_config_type;


typedef PACK(struct)
{

    uint8                                      num_harq_processes;
    boolean                                    mem_partition_valid;
    tdsl1_dl_hs_dsch_harq_mem_sz_enum_type        mem_size[TDSL1_DL_CCTRCHCFG_HS_DSCH_HARQ_MAX_PROC];          /* max number of HARQ processes is 8 */

} ftm_tdshsdpa_bler_harq_process_info_type;

/* This type specifies the FTM HS-DSCH configuration parameters */

typedef PACK(struct)
{

    uint16                                      h_rnti;
    ftm_tdshsdpa_bler_harq_process_info_type       harq_info;

} ftm_tdshsdpa_bler_hs_dsch_config_type;




/* Interface to PC */

typedef PACK(struct)
{

    diagpkt_subsys_header_type   header;
    ftm_cmd_header_type          ftm_cmd_header;
    byte                         data[1];

} ftm_tdshsdpa_bler_pkt_type;


/* This type specifies the interface for the Start HS channel command */

typedef PACK(struct)
{

    diagpkt_subsys_header_type   header;
    ftm_cmd_header_type          ftm_cmd_header;

} ftm_tdshsdpa_bler_start_hs_cmd_type;


/* This type specifies the interface for the Stop HS channel command */

typedef ftm_tdshsdpa_bler_start_hs_cmd_type   ftm_tdshsdpa_bler_stop_hs_cmd_type;


/* This type specifies the interface for the Reconfig HS channel command */

typedef PACK(struct)
{

    diagpkt_subsys_header_type   header;
    ftm_cmd_header_type          ftm_cmd_header;
    uint8                        req_mask;

} ftm_tdshsdpa_bler_reconfig_hs_cmd_type;

/* This type specifies the interface for the Config HS-SCCH command */

typedef PACK(struct)
{

    diagpkt_subsys_header_type              header;
    ftm_cmd_header_type                     ftm_cmd_header;
    ftm_tdshsdpa_bler_hs_scch_config_type      config;

} ftm_tdshsdpa_bler_config_hs_scch_cmd_type;

/* This type specifies the interface for the Config HS-DSCH command */

typedef PACK(struct)
{

    diagpkt_subsys_header_type            header;
    ftm_cmd_header_type                   ftm_cmd_header;
    ftm_tdshsdpa_bler_hs_dsch_config_type    config;

} ftm_tdshsdpa_bler_config_hs_dsch_cmd_type;

/* This type specifies the interface for the Config HS-PDSCH command */

typedef PACK(struct)
{

    diagpkt_subsys_header_type            header;
    ftm_cmd_header_type                   ftm_cmd_header;
    ftm_tdshsdpa_bler_hs_pdsch_config_type   config;

} ftm_tdshsdpa_bler_config_hs_pdsch_cmd_type;


/* This type specifies the request structure for FTM_TDSHSDPA_BLER_EVENT_STATUS_REQ */

typedef PACK(struct)
{
    diagpkt_subsys_header_type        header;
    ftm_cmd_header_type               ftm_cmd_header;
    int8                              event_type;
    uint32                            timeout_ms;

} ftm_tdshsdpa_bler_event_status_req_type;

enum
{
    FTM_TDSHSDPA_BLER_GEN_RESPONSE = 0x1003
};

/* This type specifies the FTM TDSCDMA BER response header packet */
typedef PACK(struct)
{
    uint16                     rsp_id;
    uint16                     rsp_cid;
    uint16                     rsp_scid;

} ftm_tdshsdpa_bler_rsp_header_type;

/* This type specifies the FTM TDSCDMA BER response packet */
typedef PACK(struct)
{

    diagpkt_subsys_header_type      header;
    ftm_tdshsdpa_bler_rsp_header_type  ftm_rsp_header;
    uint16                          status;

} ftm_tdshsdpa_bler_gen_rsp_pkt_type;

/* Command codes for the FTM HSDPA BER feature */

enum
{

    FTM_TDSHSDPA_BLER_CMD_START_HS                  = FTM_TDSCDMA_BER_HSDPA_CMD_BASE,
    FTM_TDSHSDPA_BLER_CMD_STOP_HS,
    FTM_TDSHSDPA_BLER_CMD_RECONFIG_HS,
    FTM_TDSHSDPA_BLER_CMD_CONFIG_HS_PDSCH,
    FTM_TDSHSDPA_BLER_CMD_CONFIG_HS_SCCH,
    FTM_TDSHSDPA_BLER_CMD_CONFIG_HS_DSCH,
   FTM_TDSHSDPA_BLER_EVENT_STATUS_CLEAR_REQ,
   FTM_TDSHSDPA_BLER_EVENT_STATUS_REQ

};



/* FTM TDSCDMA BER Asynchronous Logging Interface */

/* Asynchronous logging interface */

/* This type specifies the possible asynchronous event packet types */
typedef enum
{

    FTM_LOG_TDSHSDPA_HS_CHANNEL_START_IND = FTM_LOG_TDSHSDPA_LOG_ID_BASE,
    FTM_LOG_TDSHSDPA_HS_CHANNEL_STOP_IND,
    FTM_LOG_TDSHSDPA_HS_CHANNEL_RECFG_IND


} ftm_log_tdshsdpa_bler_id_type;

/* This type specifies the possible synchronous status EVENT_ID types for the 
   FTM_TDSHSDPA_BLER_EVENT_STATUS_REQ command
*/
typedef enum
{
    FTM_TDSHSDPA_BLER_HS_CHANNEL_START_SYNC_STATUS = 0,
    FTM_TDSHSDPA_BLER_HS_CHANNEL_STOP_SYNC_STATUS  = 1,
    FTM_TDSHSDPA_BLER_HS_CHANNEL_RECFG_SYNC_STATUS = 2,

	// Insert new values aboive this one
	FTM_TDSHSDPA_BLER_SYNC_STATUS_MAX

} ftm_log_tdshsdpa_bler_sync_event_id_type;

/* This type specifies the synchronous status types reported using  
   the FTM_TDSHSDPA_BLER_EVENT_STATUS_REQ command 
*/
typedef enum
{

    FTM_TDSHSDPA_BLER_SYNC_STATUS_SUCCESS             = 0,
    FTM_TDSHSDPA_BLER_SYNC_STATUS_GEN_FAILURE         = 1,
    FTM_TDSHSDPA_BLER_SYNC_STATUS_TIMEOUT_EXPIRED     = 2,
    FTM_TDSHSDPA_BLER_SYNC_STATUS_EVENT_STATUS_FAILED = 3

} ftm_tdshsdpa_bler_sync_status_type;

/* This type specifies the FTM HSDPA BER generic confirmation event packet */
typedef PACK(struct)
{

    ftm_log_gen_type            header;
    ftm_log_tdshsdpa_bler_id_type  log_id;

} ftm_log_tdshsdpa_bler_gen_cnf;

/* This type specifies the FTM TDSCDMA BER generic confirmation event with status packet */
typedef PACK(struct)
{

    ftm_log_gen_type            header;
    ftm_log_tdshsdpa_bler_id_type  log_id;
    uint8                       status;

} ftm_log_tdshsdpa_bler_gen_cnf_with_status;

/* Structure for events that will be reported synchronously */
typedef struct 
{
	uint8 has_updated;
	ftm_tdshsdpa_bler_sync_status_type status;
} ftm_tdshsdpa_bler_sync_status_struct;


ftm_rsp_pkt_type ftm_tdshsdpa_bler_dispatch( ftm_tdshsdpa_bler_pkt_type *pkt);


void ftm_tdshsdpa_bler_init( void);

void ftm_tdshsdpa_bler_process_l1_messages( tdsrrc_cmd_type *msg);
ftm_tdshsdpa_bler_hs_force_stop_error_type ftm_tdshsdpa_bler_request_stop_hs_channel( tdsl1_setup_cmd_type *cmd);
void ftm_tdshsdpa_bler_request_resume_hs_channel( void);
void ftm_tdshsdpa_bler_result_update(uint32 txRcv, uint32 newtxRcv, uint32 txCRCpass, uint32 newtxCRCpass, uint16 bitRate/* in Kbps*/);

#endif /* FTM_HAS_TDS_HSDPA */
#endif /* FTM_HAS_TDSCDMA_BER */
#endif /* FEATURE_FACTORY_TESTMODE */
#endif /* FTM_TDSCDMA_BER_H */
