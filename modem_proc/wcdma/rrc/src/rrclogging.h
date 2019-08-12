#ifndef RRCLOGGING_H
#define RRCLOGGING_H
/*===========================================================================
                          R R C L O G G I N G

DESCRIPTION
  This file defines the logging interface between RRC and the tools.

Copyright (c) 2001-2003, 2005, 2007-2008 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrclogging.h_v   1.4   18 Sep 2001 09:38:22   bumatt  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrclogging.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/10/16   sr      FR 38167: Changes to correct WTOL RESELECTION START event 
06/07/16   nr      Adding log packet for standalone HRB db
05/12/16   vs      Adding log packet for measurement related logging
05/09/16   nr      Adding log packet for hanging RB 
01/14/15   sas     Made changes to log events for cfsb call opt feature
10/01/14   sp      Made changes to use bit mask for fields in EVENT_WCDMA_CELL_EFACH_EPCH_ERACH_SUPPORT 
09/05/14   as      Made changes to log events for inter freq HHO status
07/17/14   vi      Made changes to add WtoW and WtoG Redirection events
06/09/14   db      RRC Logging Changes for CMAC and CRLC
05/20/14   rmn     Made changes to log start event for blind redirection to LTE.
24/01/13   sg      Made changes to log CU and UU status as external event
08/14/13   vg      Made changes to implement a new diag event EVENT_WCDMA_RRC_FAILURE
03/08/13   sr      Added changes to introduce events for CSFB redirection with SI container
02/28/13   sn      New log packet for CSG finger printing information
12/07/12   sn      CSG feature changes
11/19/12   sn      Added new fields to WCDMA CELL ID log packet
02/15/12   gv      Added support to log new event when complete SIB is received
05/10/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.97.50
05/03/11   as      Made changes to support CRNTI and URNTI events
04/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.94.52
04/05/11   as      Added code to support E-FACH events
02/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.50
01/27/11   rm      Added code to support WCDMA to LTE events
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/05/11   su      Corrected macro for logging etws primary notification with
                   security.
01/04/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.87.50
12/10/10   su      Added code to support FEATURE_WCDMA_ETWS.
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
11/24/10   ss      Added code to support logging for System Information Container
12/22/10   su      Made code changes to support FEATURE_WCDMA_ETWS.
11/24/10   ss      Added code to support logging for System Information Container
11/09/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.84.50
10/18/10   rm      Added code to support EOOS Events
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
08/18/10   ss      Added SIB19 logging code.
05/20/10   gkg     Made changes to log RRC state in SMEM area.
09/22/09   sup     Removed log code assignments and placed them in 
                   log_codes_wcdma_int.h
05/08/09   ss      Updated Copyright Information
12/29/08   rm      Added support for new Events
23/10/08   gkg     Added support for events EVENT_WCDMA_CONN_REQ_CAUSE and 
                   EVENT_WCDMA_CONN_REL_CAUSE.
09/12/08   ps      Added code for REl7 under FEATURE_WCDMA_REL7
03/06/08   da      Changed FEATURE_MBMS to FEATURE_MODEM_MBMS
12/21/07   da      Support for FEATURE_MBMS
06/24/05   svk     Added new_rate to rrc_event_state_change_info_type struct.
08/07/03   kc      Added code for GtoW Handover Events
05/23/03   svk     Added enum rrc_rl_failure_cause_e_type
05/05/03   bu      Added rrclog_wcdma_cell_selected_type for reporting
                   EVENT_WCDMA_CELL_SELECTED.
04/30/01   vk      Added rrc_event_paging_info_type, rrc_event_state_change_info_type
                   for sending paging and state change related events.
04/11/03   bu      Added definitions for WTOG re-selection start, end and
                   handover start event types for reporting it in QXDM.
01/13/03   ram     Updated copyright
12/12/02   jh      Added definitions for signalling message types for use in
                   QXDM event reporting.
9/18/01    bu      Modified the log codes for log packets match the
                   DIAG interface.
7/30/01    rj      RRC Logging codes are modifed to use LOG_WCDMA_BASE_C so
                   that there won't be any mismatches with DIAG interface.
6/11/01    rj      Fixed a typo in WCDMA_SIGNALLING_MESSAGE record
                   declaration.
5/15/01    jrs     Removed RRCLOG_SIG_DL_DTCH and RRCLOG_SIG_UL_DTCH from
                   the signalling message packet since there aren't any
                   signalling messages on the DTCH.
4/3/01     jrs     Created file.

===========================================================================*/


/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "comdef.h"
#include "log.h"
#include "rrcmmif.h"
#include "event.h"
#include "log_codes_wcdma_int.h"

#ifdef FEATURE_WCDMA_LOG_RRC_STATES
#include "smem_log.h"
#endif /*FEATURE_WCDMA_LOG_RRC_STATES*/

/* Set one-byte structure packing for Windows targets. The packing for ARM
 * targets is specified by use of the PACKED directive on structure
 * definitions.
 */
#ifdef T_WINNT
  #error code not present
#endif


/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* Define the RRC states. These defines are used in log packets 0x4125 and
 * 0x4126 to set the rrc_state field.
 */
#define RRCLOG_STATE_DISCONNECTED  0      /* Disconnected State (Idle Mode) */
#define RRCLOG_STATE_CONNECTING    1      /* Connecting State (Idle Mode) */
#define RRCLOG_STATE_CELL_FACH     2      /* CELL_FACH State (Connected Mode) */
#define RRCLOG_STATE_CELL_DCH      3      /* CELL_DCH State (Connected Mode) */
#define RRCLOG_STATE_CELL_PCH      4      /* CELL_PCH State (Connected Mode) */
#define RRCLOG_STATE_URA_PCH       5      /* URA_PCH State (Connected Mode) */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

typedef enum{
RRC_CU_UU_STATUS_START,
RRC_CU_UU_STATUS_RESEND,
RRC_CU_UU_STATUS_SUCCESS,
RRC_CU_UU_STATUS_FAILURE
}rrc_cu_uu_status_e_type;

typedef enum{
RRC_LOG_INTER_FREQ_HHO_STARTED,
RRC_LOG_INTER_FREQ_HHO_SUCCESS,
RRC_LOG_INTER_FREQ_HHO_FAILURE
}rrc_inter_freq_hho_status_e_type;

typedef enum
{
  RRC_CU_UU_CAUSE_NONE,                      /*CU cause none*/
  RRC_CU_CELL_RESELECTION,                   /* Cell Reselection */
  RRC_CU_PERIODIC_CELL_UPDATE,               /* Periodic Cell Update */
  RRC_CU_UL_DATA_TRANSMISSION,               /* UL Data Transmission */
  RRC_CU_PAGING_RESPONSE,                    /* Paging Response */
  RRC_CU_RADIO_LINK_FAILURE,                 /* Radio Link Failure */
  RRC_CU_REENTERED_SERVICE_AREA,             /* Re-entered Service area */
  RRC_CU_UNRECOVERABLE_ERROR,                 /* Unrecoverable error */
  RRC_UU_URA_RESELECTION,                      /* Change of URA id */
  RRC_UU_PERIODIC_URA_UPDATE                 /* Periodic URA update */
} rrc_cu_uu_cause_e_type;

typedef enum
{
  RRC_CU_UU_FAILURE_CAUSE_NONE,
  RRC_CU_UU_UNSUPPORTED_CONFIG,
  RRC_CU_UU_INVALID_CONFIG,
  RRC_CU_UU_INCOMPATIBLE_SIMULTANEOUS_RECFG,
  RRC_CU_UU_PROTOCOL_ERROR,
  RRC_CU_UU_T302_EXPIRED,
  RRC_CU_UU_ANOTHER_CU_INITIATED,
  RRC_CU_UU_TX_TO_IDLE
} rrc_cu_uu_failure_cause_e_type;

typedef enum{
  RL_FAILURE_DUE_TO_L1=0,
  RL_FAILURE_DUE_TO_RRC
}rrc_rl_failure_cause_e_type;

typedef struct
{
  /* Current RRC state */
  uint8 current_rrc_state;
  /* The next state to which RRC would transition to */
  uint8 next_rrc_state;

  /* Indicates the rate */
  uint8 new_rate;

}rrc_event_state_change_info_type;

typedef enum
{
  /* Paging type 1 */
  RRCLOG_PAGING_TYPE_1,
  /* Paging type 2 */
  RRCLOG_PAGING_TYPE_2
}rrc_event_paging_e_type;

typedef struct
{
  /* Paging type */
  rrc_event_paging_e_type           paging_type;
  /* Paging cause */
  rrc_paging_cause_e_type       paging_cause;
}rrc_event_paging_info_type;
typedef enum
{  
  RRCLOG_PROTOCOL_ERROR_ViolationOrEncodingError = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError,
  RRCLOG_PROTOCOL_ERROR_messageTypeNonexistent = rrc_ProtocolErrorCause_messageTypeNonexistent,
  RRCLOG_PROTOCOL_ERROR_messageNotCompatibleWithReceiverState = rrc_ProtocolErrorCause_messageNotCompatibleWithReceiverState,
  RRCLOG_PROTOCOL_ERROR_ie_ValueNotComprehended = rrc_ProtocolErrorCause_ie_ValueNotComprehended,
  RRCLOG_PROTOCOL_ERROR_informationElementMissing = rrc_ProtocolErrorCause_informationElementMissing,
  RRCLOG_PROTOCOL_ERROR_messageExtensionNotComprehended = rrc_ProtocolErrorCause_messageExtensionNotComprehended,
  RRCLOG_PROTOCOL_ERROR_spare2 = rrc_ProtocolErrorCause_spare2,
  RRCLOG_PROTOCOL_ERROR_spare1 = rrc_ProtocolErrorCause_spare1,
  RRCLOG_PROTOCOL_ERROR_NA=8  /* Not from the spec so choosing such value which is above the range of the error causes in the spec.
                                 If Failure cause  is not RRCLOG_failureCause_protocolError then this value will be used in the payload.*/
} rrclog_wcdma_rrc_protocol_error_cause_e_type;

typedef enum
{  
  RRCLOG_failureCause_configurationUnsupported = T_rrc_FailureCauseWithProtErr_configurationUnsupported,
  RRCLOG_failureCause_physicalChannelFailure = T_rrc_FailureCauseWithProtErr_physicalChannelFailure,
  RRCLOG_failureCause_incompatibleSimultaneousReconfiguration = T_rrc_FailureCauseWithProtErr_incompatibleSimultaneousReconfiguration,
  RRCLOG_failureCause_compressedModeRuntimeError = T_rrc_FailureCauseWithProtErr_compressedModeRuntimeError,
  RRCLOG_failureCause_protocolError = T_rrc_FailureCauseWithProtErr_protocolError,
  RRCLOG_failureCause_cellUpdateOccurred = T_rrc_FailureCauseWithProtErr_cellUpdateOccurred,
  RRCLOG_failureCause_invalidConfiguration = T_rrc_FailureCauseWithProtErr_invalidConfiguration,
  RRCLOG_failureCause_configurationIncomplete = T_rrc_FailureCauseWithProtErr_configurationIncomplete,
  RRCLOG_failureCause_unsupportedMeasurement = T_rrc_FailureCauseWithProtErr_unsupportedMeasurement,
  RRCLOG_RachFailure = 17,  /* Not from the spec so choosing such value which is above the range of the failure causes in the spec. */
  RRCLOG_releaseCauseUnspecified=18,       /* Not from the spec so choosing such value which is above the range of the failure causes in the spec. */ 
  /* Below are Inter RAT failure causes. These are from the spec but translating here to new values 
     as two choice struct(FailureCauseWithProtErr and InterRAT-HO-FailureCause) 
     in the spec are being combined in to single ENUM here.*/
  RRCLOG_InterRatfailureCause_configurationUnacceptable = 19,
  RRCLOG_InterRatfailureCause_physicalChannelFailure = 20,
  RRCLOG_InterRatfailureCause_protocolError = 21,
  RRCLOG_InterRatfailureCause_unspecified = 22
    
} rrclog_wcdma_rrc_failure_cause_e_type;

typedef struct 
{
  rrc_state_e_type rrc_state; 
  rrc_proc_e_type  rrc_proc; /* Exists in file rrc_data.h. From this failure message name can be derived */
  rrclog_wcdma_rrc_failure_cause_e_type  rrc_failure_cause;
  rrclog_wcdma_rrc_protocol_error_cause_e_type protocol_error_cause;
} rrclog_wcdma_rrc_failure_event_type;



/*--------------------------------------------------------------------------
                   LOG PACKET: WCDMA_RRC_MODES_AND_STATES
--------------------------------------------------------------------------*/
LOG_RECORD_DEFINE(WCDMA_RRC_MODES_AND_STATES)

  /* The RRC State is the only field in this log packet
   */
  uint8 rrc_state;                /* The RRC state */

LOG_RECORD_END

/*--------------------------------------------------------------------------
                   LOG PACKET: WCDMA_RRC_PROTOCOL_ERRORS
--------------------------------------------------------------------------*/
/* Define the RRC procedures. These defines are used to set the rrc_procedure
 * field.
 */
#define RRCLOG_PROCEDURE_CSP    0   /* Cell Selection */
#define RRCLOG_PROCEDURE_SIB    1   /* SIB Processing */
#define RRCLOG_PROCEDURE_PG2    2   /* Paging Type 2 */
#define RRCLOG_PROCEDURE_MCMR   3   /* Measurement Control Reporting */
#define RRCLOG_PROCEDURE_RCE    4   /* RRC Connection Establishment */
#define RRCLOG_PROCEDURE_RCR    5   /* RRC Connection Release */
#define RRCLOG_PROCEDURE_UECI   6   /* UE Capability Information */
#define RRCLOG_PROCEDURE_UECE   7   /* UE Capability Enquiry */
#define RRCLOG_PROCEDURE_IDT    8   /* Initial Direct Transfer */
#define RRCLOG_PROCEDURE_UDT    9   /* Uplink Direct Transfer */
#define RRCLOG_PROCEDURE_DDT    10  /* Downlink Direct Transfer */
#define RRCLOG_PROCEDURE_SCR    11  /* Signalling Connection Release */
#define RRCLOG_PROCEDURE_SCRR   12  /* Signalling Connection Release Request */
#define RRCLOG_PROCEDURE_CC     13  /* Counter Check */
#define RRCLOG_PROCEDURE_RBE    14  /* Radio Bearer Establishment */
#define RRCLOG_PROCEDURE_RBRC   15  /* Radio Bearer Re-configuration */
#define RRCLOG_PROCEDURE_RBR    16  /* Radio Bearer Release */
#define RRCLOG_PROCEDURE_TCR    17  /* Transport Channel Re-configuration */
#define RRCLOG_PROCEDURE_PCR    18  /* Physical Channel Re-configuration */
#define RRCLOG_PROCEDURE_TFC    19  /* Transport Format Combination Control */
#define RRCLOG_PROCEDURE_CU     20  /* Cell Update */
#define RRCLOG_PROCEDURE_UU     21  /* URA Update */
#define RRCLOG_PROCEDURE_UMI    22  /* UTRAN Mobility Information */
#define RRCLOG_PROCEDURE_ASU    23  /* Active Set Update */
#define RRCLOG_PROCEDURE_ISHFU  24  /* Inter-System Handover from UTRAN */
#define RRCLOG_PROCEDURE_ISHTU  25  /* Inter-System Handover to UTRAN */
#define RRCLOG_PROCEDURE_ISCFU  26  /* Inter-System Cell Reselection fm UTRAN */
#define RRCLOG_PROCEDURE_ISCTU  27  /* Inter-System Cell Reselection to UTRAN */
#define RRCLOG_PROCEDURE_PG1    28  /* Page Type 1 */
#define RRCLOG_PROCEDURE_SMC    29  /* Security Mode Control */

/* Define the failure causes. These defines are used to set the
 * failure_cause field.
 */
#define RRCLOG_CONFIG_UNSUP           0 /* Configuration unsupported */
#define RRCLOG_PHY_CHAN_FAIL          1 /* Physical channel failure */
#define RRCLOG_INCOMPAT_SIM_RECONFIG  2 /* Incompatible simultaneous reconfig */
#define RRCLOG_PROTOCOL_ERROR         3 /* Protocol error */
#define RRCLOG_COMPRESSED_RT_ERR      4 /* Compressed mode runtime error */
#define RRCLOG_CELL_RESEL             5 /* Cell reselection */
#define RRCLOG_INVALID_CONFIG         6 /* Invalid configuration */
#define RRCLOG_CONFIG_INCOMPLETE      7 /* Configuration incomplete */
#define RRCLOG_UNSUP_MEAS             8 /* Unsupported measurement */

/* Define the protocol errors. These defines are used to set the
 * protocol_error_cause field.
 */
#define RRCLOG_ASN1_ENC_ERR      0 /* ASN.1 violation or encoding error */
#define RRCLOG_BAD_MSG_TYPE      1 /* Msg type nonexistent or not implemented */
#define RRCLOG_INCOMPAT_RX_STATE 2 /* Msg not compatible with receiver state */
#define RRCLOG_BAD_IE            3 /* IE value not comprehended */
#define RRCLOG_COND_IE_ERROR     4 /* Conditional IE error */
#define RRCLOG_BAD_MSG_EXTENSION 5 /* Message extension not comprehended */


LOG_RECORD_DEFINE(WCDMA_RRC_PROTOCOL_ERRORS)

  uint8 rrc_state;                /* The RRC state */
  uint8 rrc_procedure;            /* The RRC procedure */
  uint8 failure_cause;            /* The failure cause */
  uint8 protocol_error_cause;     /* The protocol error cause */

LOG_RECORD_END

/*--------------------------------------------------------------------------
                   LOG PACKET: WCDMA_CELL_ID
--------------------------------------------------------------------------*/
/* Define the cell access restrictions. These defines are used to set the
 * cell_access_rest field.
 *
 * Bit 0: 0 = Cell barred
 *        1 = Cell not barred
 *
 * Bit 1: 0 = Cell Reserved
 *        1 = Cell not reserved
 *
 * Bit 2: 0 = Cell reserved for SOLSA
 *        1 = Cell not reserved for SOLSA
 *
 * Bit 3: 0 = UE camped on cell
 *        1 = UE not camped on cell
 */
#define RRCLOG_CELL_BARRED             0
#define RRCLOG_CELL_NOT_BARRED         1

#define RRCLOG_CELL_RESERVED           0
#define RRCLOG_CELL_NOT_RESERVED       2

#define RRCLOG_CELL_RESERVED_SOLSA     0
#define RRCLOG_CELL_NOT_RESERVED_SOLSA 4

#define RRCLOG_UE_CAMPED               0
#define RRCLOG_UE_NOT_CAMPED           8

/* Define the call access. These defines are used to set the call_access field.
 *
 * Bit definitions for the call_access field:
 *
 * Bit 0: 0 = All calls
 *        1 = Emergency calls only
 */
#define RRCLOG_ALL_CALLS               0
#define RRCLOG_EMERGENCY_CALLS         1

LOG_RECORD_DEFINE(WCDMA_CELL_ID)

  uint32 ul_uarfcn;               /* The uplink UARFCN */
  uint32 dl_uarfcn;               /* The downlink UARFCN */
  uint32 cell_id;                 /* The cell ID */
  uint16 ura_id;                  /* The URA ID */
  uint8 cell_access_rest;         /* Cell access restrictions */
  uint8 call_access;              /* Allowed access */
  uint16 psc;
  uint8 mcc[3];
  uint8 mnc[3];
  uint32 lac;
  uint32 rac;

LOG_RECORD_END

#ifdef FEATURE_FEMTO_CSG

#define W_ASF_RAT_LTE 1

#define W_ASF_RAT_WCDMA 0

/* WCDMA FPed CSG cell info */
typedef PACKED struct PACKED_POST
{
  /* CSG PLMN MCC */
  uint8 mcc[3];

  /* CSG PLMN MNC */
  uint8 mnc[3];

  /* Frequency */
  uint32 uarfcn;

  /* Cell ID */
  uint32 cell_id;

  /* CSG ID */
  uint32 csg_id;

  /* Scrambling code */
  uint16 psc;

}wcdma_csg_cell_info_log_struct_type;

LOG_RECORD_DEFINE(WCDMA_FP_INFO)
  
  /* Version info */
  uint8 version;   

  /* RAT */   
  uint8 rat;
                     
  /* macro PLMN MCC */
  uint8 mcc[3];

  /* macro PLMN MNC */
  uint8 mnc[3];

  /* Frequency */
  uint32 uarfcn;

  /* Cell ID */
  uint32 cell_id;

  /* Scrambling code */
  uint16 psc;

  /*Number of CSG entries*/
  uint32 num_csg_entries;

  wcdma_csg_cell_info_log_struct_type csg_cells[MAX_ACQ_DB_CSG_ENTRIES];

LOG_RECORD_END

#endif
/*--------------------------------------------------------------------------
                   LOG PACKET: WCDMA_HANGING_RB_INFO
--------------------------------------------------------------------------*/

typedef PACKED struct PACKED_POST
{
  uint8 ul_mapping_type;
  uint8 ul_trch_id; 
  uint8 dl_mapping_type;
  uint8 dl_trch_id;
} rrclog_hrb_mapping_info_type;


#define MAX_HRB_MAPPING_COMBO 8
#define MAX_HRB_MAPPING_INFO 13

typedef PACKED struct PACKED_POST
{
  uint8 rb_id;
  uint8 lc_id;
  uint8 mapping_combo_per_rb;
  rrclog_hrb_mapping_info_type hrb_mapping_info[MAX_HRB_MAPPING_COMBO];
} rrclog_hrb_info_type;

LOG_RECORD_DEFINE(WCDMA_HANGING_RB_INFO)
  uint8 version;
  uint8 num_rb_in_use;
  rrclog_hrb_info_type  rrclog_hrb_info[MAX_HRB_MAPPING_INFO];

LOG_RECORD_END

#define SIZE_RRCLOG_HRB_INFO_TYPE(num_mapping_combos) \
  (FPOS(rrclog_hrb_info_type,hrb_mapping_info) +             \
   num_mapping_combos * sizeof(rrclog_hrb_mapping_info_type))


/*--------------------------------------------------------------------------
                   LOG PACKET: WCDMA_HANGING_RB_MAPPING_INFO
--------------------------------------------------------------------------*/

typedef PACKED struct PACKED_POST
{
  uint8 mapping_type;
  uint8 trch_id; 
  uint8 log_channel_id;
} rrclog_hrb2_mapping_info_type;

typedef PACKED struct PACKED_POST
{
  uint8 rb_index;
  uint8 rb_id;
  uint8 lc_id;
  uint8 num_mappings_per_rb;
  rrclog_hrb2_mapping_info_type hrb2_mapping_info[MAX_HRB_MAPPING_COMBO];
} rrclog_hrb2_info_type;

LOG_RECORD_DEFINE(WCDMA_HANGING_RB_MAPPING_INFO)
  uint8 version;
  uint8 num_rb_in_use;
  rrclog_hrb2_info_type  rrclog_hrb2_info[MAX_HRB_MAPPING_INFO];
LOG_RECORD_END

#define SIZE_RRCLOG_HRB2_INFO_TYPE(num_mappings_per_rb) \
  (FPOS(rrclog_hrb2_info_type,hrb2_mapping_info) +             \
   num_mappings_per_rb * sizeof(rrclog_hrb2_mapping_info_type))



/*--------------------------------------------------------------------------
                   LOG PACKET: WCDMA_UL_CHANNEL_MAPPING
--------------------------------------------------------------------------*/
/* Define the uplink transport channels. These defines are used to set the
 * ul_transport_channel_type field.
 */
#define RRCLOG_UL_DCH  0          /* UL DCH transport channel */
#define RRCLOG_UL_RACH 1          /* UL RACH transport channel */

/* Define the uplink physical channels. These defines are used to set the
 * ul_physical_channel_type field.
 */
#define RRCLOG_UL_DPCH  0         /* DPCH physical channel */
#define RRCLOG_UL_PRACH 1         /* PRACH physical channel */

/* Define the RLC modes. These defines are used to set the rlc_mode field.
 */
#define RRCLOG_UL_RLC_MODE_TM  0  /* RLC transparent mode */
#define RRCLOG_UL_RLC_MODE_AM  1  /* RLC acknowledged mode */
#define RRCLOG_UL_RLC_MODE_UM  2  /* RLC unacknowledged mode */

/* Define the logical channel types. These defines are used to set the
 * logical_channel_type field.
 */
#define RRCLOG_UL_CCCH 0          /* UL CCCH logical channel */
#define RRCLOG_UL_DCCH 1          /* UL DCCH logical channel */
#define RRCLOG_UL_DTCH 2          /* UL DTCH logical channel */


/* At present keeping this under R7 feature should be __GNUC__ actually. But lint cribs */

/* Define a structure to contain information about each logical channel
 */

typedef PACKED struct PACKED_POST
{
  uint8 rb_id;                    /* Radio bearer ID */
  uint8 rlc_id;                   /* RLC ID */
  uint8 rlc_mode;                 /* RLC mode */
  uint8 logical_channel_id;       /* Logical channel ID */
  uint8 logical_channel_type;     /* Logical channel type */
} rrclog_ul_logical_channel_e_type;
/* Define a structure to hold information about each transport channel
 */
typedef PACKED struct PACKED_POST
{
  uint8 ul_transport_channel_id;    /* Uplink transport channel ID */
  uint8 ul_transport_channel_type;  /* Uplink transport channel type */
  uint8 ul_physical_channel_type;   /* Uplink physical channel type */
  uint8 num_logical_channels;       /* Number of logical channels */
  rrclog_ul_logical_channel_e_type logical_channel[32];  /* Logical channels */
} rrclog_ul_transport_channel_e_type;

LOG_RECORD_DEFINE(WCDMA_UL_CHANNEL_MAPPING)

  uint8 num_ul_tr_chan;         /* Number of UL transport channels */
  uint8 cctrch_id;              /* CCTrCH ID */

LOG_RECORD_END

/*--------------------------------------------------------------------------
                   LOG PACKET: WCDMA_DL_CHANNEL_MAPPING
--------------------------------------------------------------------------*/
/* Define the downlink transport channels. These defines are used to set the
 * dl_transport_channel_type field.
 */
#define RRCLOG_DL_SBCH  0         /* DL serving BCH transport channel */
#define RRCLOG_DL_NBCH  1         /* DL neighbor BCH */
#define RRCLOG_DL_DCH   2         /* DL DCH transport channel */
#define RRCLOG_DL_DSCH  3         /* DL DSCH transport channel */
#define RRCLOG_DL_PCH   4         /* DL PCH transport channel */
#define RRCLOG_DL_FACH  5         /* DL FACH transport channel */

/* Define the downlink physical channels. These defines are used to set the
 * dl_physical_channel_type field.
 */
#define RRCLOG_DL_PCCPCH 0        /* DL PCCPCH physical channel */
#define RRCLOG_DL_SCCPCH 1        /* DL SCCPCH physical channel */
#define RRCLOG_DL_AICH   2        /* DL AICH physical channel */
#define RRCLOG_DL_PICH   3        /* DL PICH physical channel */
#define RRCLOG_DL_PDSCH  4        /* DL PDSCH physical channel */
#define RRCLOG_DL_DPCH   5        /* DL DPCH physical channel */

/* Define the RLC modes. These defines are used to set the rlc_mode field.
 */
#define RRCLOG_DL_RLC_MODE_TM  0  /* RLC transparent mode */
#define RRCLOG_DL_RLC_MODE_AM  1  /* RLC acknowledged mode */
#define RRCLOG_DL_RLC_MODE_UM  2  /* RLC unacknowledged mode */

/* Define the logical channel types. These defines are used to set the
 * logical_channel_type field.
 */
#define RRCLOG_DL_BCCH  0         /* DL BCCH logical channel */
#define RRCLOG_DL_PCCH  1         /* DL PCCH logical channel */
#define RRCLOG_DL_CCCH  2         /* DL CCCH logical channel */
#define RRCLOG_DL_CTCH  3         /* DL CTCH logical channel */
#define RRCLOG_DL_DCCH  4         /* DL DCCH logical channel */
#define RRCLOG_DL_DTCH  5         /* DL DTCH logical channel */



/* At present keeping this under R7 feature should be __GNUC__ actually. But lint cribs */

/* Define a structure to contain information about each logical channel
 */
typedef PACKED struct PACKED_POST
{
  uint16 rb_id;                    /* Radio bearer ID */
  uint8 rlc_id;                   /* RLC ID */
  uint8 mac_id;       /* Logical channel ID */
  uint8 rlc_mode;                 /* RLC mode */
  uint8 logical_channel_type;     /* Logical channel type */
} rrclog_dl_logical_channel_e_type;

/* Define a structure to hold information about each transport channel
 */
typedef PACKED struct PACKED_POST
{
  uint8 dl_transport_channel_id;    /* Downlink transport channel ID */
  uint8 dl_transport_channel_type;  /* Downlink transport channel type */
  uint8 cctrch_id;                  /* Downlink CCTrCH ID */
  uint8 dl_physical_channel_type;   /* Downlink physical channel type */
  uint8 num_logical_channels;       /* Number of logical channels */
  rrclog_dl_logical_channel_e_type logical_channel[32];  /* Logical channels */
} rrclog_dl_transport_channel_e_type;

LOG_RECORD_DEFINE(WCDMA_DL_CHANNEL_MAPPING)

  uint8 num_dl_tr_chan;         /* Number of DL transport channels */

LOG_RECORD_END

/*CMAC DL Logging Structures for R99 Channels*/
typedef PACKED struct PACKED_POST
{
  uint8      cctrch_id;
  uint8      rlc_id;
}rrclog_mac_dl_bch_pch_type;

typedef PACKED struct PACKED_POST
{
  uint8              trch_id;
  uint8              cctrch_id;
  boolean            bcch_enable;
  uint8              bcch_id;
  boolean            ccch_enable;
  uint8              ccch_id;
  boolean            ctch_enable;
  uint8              ctch_id;
  uint8              ndlchan;
  rrclog_dl_logical_channel_e_type  dlchan_info[MAX_DED_LOGCHAN_PER_TRCH];

}rrclog_mac_dl_fach_type;

typedef PACKED struct PACKED_POST
{
  uint8              trch_id;
  uint8              cctrch_id;
  uint8              ndlchan;
  rrclog_dl_logical_channel_e_type  dlchan_info[MAX_DED_LOGCHAN_PER_TRCH];

}rrclog_mac_dl_dch_type;

LOG_RECORD_DEFINE(WCDMA_CMAC_DL_R99)

  uint8   version;
  boolean wait_for_l1_sig;
  uint8   num_bchs;
  uint8   num_pchs;
  uint8   num_fachs;
  uint8   num_dchs;
  uint8   rnti_type;
  uint16   c_rnti;
  uint32   u_rnti;

  rrclog_mac_dl_bch_pch_type bch_info[MAC_DL_MAX_CCTRCH];
  rrclog_mac_dl_bch_pch_type pch_info[MAC_DL_MAX_CCTRCH];
  rrclog_mac_dl_fach_type    fach_info[UE_MAX_TRCH];
  rrclog_mac_dl_dch_type     dch_info[UE_MAX_TRCH];

LOG_RECORD_END

/*CRLC AM Channel Logging Structures*/
typedef PACKED struct PACKED_POST
{
  uint8 direction;
  uint8 control_data_id;
  uint32 hfn;
  uint32 rlc_size;
  uint8 pdu_type;
  uint8 li_size;
  uint8 special_value_he_config;
  uint16 min_flex_pdu_size;
  uint16 max_flex_pdu_size;
  
} rrclog_rlc_ul_dl_info_type;

typedef PACKED struct PACKED_POST
{

  uint8 rb_id;
  uint8 lc_type;
  uint8 reestab_entity;
  uint8 action;
  uint16 dl_cfg_mask;
  uint16 ul_cfg_mask;
  uint8 ul_nchan;
  uint8 dl_nchan;

  rrclog_rlc_ul_dl_info_type ul_dl_info[2];

} rrclog_rlc_am_entity_type;


LOG_RECORD_DEFINE(WCDMA_RLC_AM_LOGGING)

  uint8 version;
  uint8 nentity;
  rrclog_rlc_am_entity_type rlc_am_channel[UE_MAX_AM_ENTITY];

LOG_RECORD_END

/*CMAC UL Logging Structures for R99 Channels*/
typedef PACKED struct PACKED_POST
{
  uint8 trch_id;
  uint8 ndlchan;
  uint8 cctrch_id;
  uint8 ccch_enable;
  uint8 ccch_id;
} rrclog_ul_trch_r99_type;

typedef PACKED struct PACKED_POST
{
  uint8 rb_id;
  uint8 rlc_id;
  uint8 mac_id;
  uint8 trch_id;
  uint8 rlc_mode;
  uint8 chan_type;
  uint8 priority;
  boolean all_tfi_valid;
  uint8 ntfi_per_lc;

} rrclog_ul_r99_logical_channel_type;


LOG_RECORD_DEFINE(WCDMA_CMAC_UL_R99_LOGGING)

  uint8 version;
  boolean mac_config_changed;
  boolean wait_for_l1_sig;
  boolean new_tfcs_list_present;
  uint8 rnti_type;
  uint16 c_rnti;
  uint32 u_rnti;
  uint8 cctrch_type;

  uint8 num_trch;
  rrclog_ul_trch_r99_type trch_info[UE_MAX_TRCH];

  uint8 ndlchan;
  rrclog_ul_r99_logical_channel_type logical_chan_info[UE_MAX_TRCH * MAX_DED_LOGCHAN_PER_TRCH];

LOG_RECORD_END

/*CMAC DL Logging Structures for HS Channels*/
typedef PACKED struct PACKED_POST
{
  uint8 rb_id;
  uint8 rlc_id;
  uint8 mac_id;
  uint8 rlc_mode;
  uint8 chan_type;
  uint8 queue_flow_id;
} rrclog_dl_hs_logical_chan_type;

typedef PACKED struct PACKED_POST
{
  uint8 queue_id;
  uint8 cctrch_id;
  uint8 window_size;
  uint16 release_timer;
  uint8 reset_timer;
  uint8 mac_d_flow_id;
  uint8 num_pdu_size;
} rrclog_dl_hs_queue_type;

typedef PACKED struct PACKED_POST
{
  uint8 flow_id;
  uint8 cctrch_id;
  uint8 num_queue;
  uint8 num_channel;
} rrclog_dl_hs_flow_type;


LOG_RECORD_DEFINE(WCDMA_CMAC_DL_HS_LOGGING)

  uint8                      version;
  uint8                      mac_dl_action;
  uint8                      wait_for_l1_sig;
  boolean                    hs_status_in_e_fach;
  uint8                      mac_hsdpa_action;
  boolean                    mac_hs_reset_indicator;
  boolean                    mac_hs_restore_tsns;
  uint8                      mac_hs_or_ehs;
  uint8                      mac_hs_transition_type;
  uint8                      reset_timer_valid;

  uint8                      ndlchan_mac_ehs_hs;
  rrclog_dl_hs_logical_chan_type hs_chanel_info[UE_MAX_DL_LOGICAL_CHANNEL];

  uint8                      num_queues;
  rrclog_dl_hs_queue_type    queue_info[UE_MAX_HS_QUEUE];

  uint8                      num_flows;
  rrclog_dl_hs_flow_type     macd_flow_info[UE_MAX_MAC_D_FLOW];

LOG_RECORD_END

typedef PACKED struct PACKED_POST
{
  uint16                                             psc;
  uint8                                              cell_position;
  boolean                                            measure_in_idle_pch_fach;
  int8                                               cell_offset;
  boolean                                            rd_sfn_ind;
  int8                                               pri_cpich_txpwr;
  boolean                                            tx_diver_ind;
}rrclog_intraf_meas_type;

typedef PACKED struct PACKED_POST
{
  uint8                                              event_id;
}intraf_event_info_type;

LOG_RECORD_DEFINE(WCDMA_MEAS_LOGGING_INTRAFREQ)
  uint8                                              version;
  uint8                                              rpt_cell_stat;
  uint8                                              rpt_cell_type;
  uint8                                              maxrptcells;
  uint8                                              num_events;
  uint8                                              num_cells_added;
  intraf_event_info_type                             intraf_event_info[L1_MAX_MEAS_EVENT];
  rrclog_intraf_meas_type                            rrc_intraf_meas[L1_MAX_CELL_MEAS];
LOG_RECORD_END

typedef PACKED struct PACKED_POST
{
  uint16                                             psc;
  uint8                                              cell_position;
  boolean                                            measure_in_idle_pch_fach;
  int8                                               cell_offset;
  boolean                                            rd_sfn_ind;
  int8                                               pri_cpich_txpwr;
  boolean                                            tx_diver_ind;
  uint16                                             uarfcn_dl;
}rrclog_interf_meas_type;

typedef PACKED struct PACKED_POST
{
  uint8                                              event_id;
}interf_event_info_type;

LOG_RECORD_DEFINE(WCDMA_MEAS_LOGGING_INTERFREQ)
  uint8                                              version;
  uint8                                              rpt_cell_stat;
  uint8                                              rpt_cell_type;
  uint8                                              maxrptcells;
  uint8                                              num_events;
  uint8                                              num_cells_added;
  interf_event_info_type                             interf_event_info[L1_MAX_MEAS_EVENT];
  rrclog_interf_meas_type                            rrc_interf_meas[L1_MAX_CELL_MEAS];
LOG_RECORD_END

typedef PACKED struct PACKED_POST
{
  uint8                                              cell_position;
  boolean                                            measure_in_idle_pch_fach;
}rrclog_inters_meas_type;

typedef PACKED struct PACKED_POST
{
  uint8                                              event_id;
}inters_event_info_type;

LOG_RECORD_DEFINE(WCDMA_MEAS_LOGGING_IRAT)
  uint8                                              version;
  uint8                                              rpt_cell_stat;
  uint8                                              rpt_cell_type;
  uint8                                              maxrptcells;
  uint8                                              num_events;
  uint8                                              num_cells_added;
  inters_event_info_type                             inters_event_info[L1_MAX_MEAS_EVENT];
  rrclog_inters_meas_type                            rrc_inters_meas[L1_MAX_CELL_MEAS];
LOG_RECORD_END

/*CMAC UL Logging Structures for HS Channels*/
typedef PACKED struct PACKED_POST
{
  uint8 e_tti;
  uint16 happy_bit_delay_in_ms;
  uint8 e_tfci_table_index;
  uint8 e_tfci_min_set;
  uint16 no_grant_periodicity_of_si;
  uint16 grant_periodicity_of_si;
  uint8 power_offset_of_si;
  uint8 sched_tx_grant_harq_allocation_2ms;

} rrclog_edch_params_type;

typedef PACKED struct PACKED_POST
{
  uint8 hs_rach_to_eul_trans;
  uint8 mac_hsrach_e_reset_indicator;
  boolean primary_e_rnti_present;
  uint16 primary_e_rnti;
  uint8 l1_e_dch_tx_cont_backoff;
  uint8 add_e_dch_tx_backoff;
  uint8  max_e_dch_res_alloc_ccch;
  uint8  ccch_asc;
  uint8  max_collision_resol;

} rrclog_hsrach_params_type;

typedef PACKED struct PACKED_POST
{
  uint8                                  e_mac_d_flow_id;
  uint8                                  num_logch; 
  uint8                                  e_mac_d_flow_power_offset;
  uint8                                  e_mac_d_flow_max_num_of_retx;
  uint8                                  e_mac_d_flow_mux_list;
  uint8                                  e_tx_grant_info;
  uint16                                 max_mac_e_pdu_size_in_bits;
  uint8                                  non_sched_tx_grant_harq_allocation_2ms;

} rrclog_mac_d_flow_params_type;

typedef PACKED struct PACKED_POST
{
  uint8 rb_id;
  uint8 rlc_id;
  uint8 lc_id;
  uint8 rlc_mode;
  uint8 rlc_pdu_type;
  uint8 ul_li_size;
  uint8 chan_type;
  uint8 priority;
  uint16 ul_flex_min_pdu_size; 
  uint16 ul_flex_max_pdu_size;

} rrclog_ul_hs_logical_chan_type;


LOG_RECORD_DEFINE(WCDMA_CMAC_UL_HS_LOGGING)

  uint8 version;
  uint8 mac_config_changed;
  uint8 wait_for_l1_sig;
  uint8 ul_pdu_type;
  uint8 ul_rlc_action;
  uint8 mac_eul_action;
  uint8 mac_e_tsn_action;
  boolean mac_e_reset_indicator;

  /*EDCH Parameters*/
  rrclog_edch_params_type edch_params;

  /*HSRACH Parameters*/
  uint8 hs_rach_action;
  rrclog_hsrach_params_type hsrach_params;

  /*Mac D Flow parameters*/
  uint8 num_e_mac_d_flow;
  rrclog_mac_d_flow_params_type mac_d_flow[MAX_E_DCH_MAC_D_FLOW];

  uint8 num_logch;
  rrclog_ul_hs_logical_chan_type logical_channel_info[UE_MAX_DL_LOGICAL_CHANNEL];

LOG_RECORD_END


#define SIZE_RRCLOG_MAC_DL_FACH_TYPE(num_logical_channels) \
  (FPOS(rrclog_mac_dl_fach_type,dlchan_info) +             \
   num_logical_channels * sizeof(rrclog_dl_logical_channel_e_type))

#define SIZE_RRCLOG_MAC_DL_DCH_TYPE(num_logical_channels) \
  (FPOS(rrclog_mac_dl_dch_type,dlchan_info) +             \
   num_logical_channels * sizeof(rrclog_dl_logical_channel_e_type))


/*--------------------------------------------------------------------------
                   LOG PACKET: WCDMA_SIB
--------------------------------------------------------------------------*/
/* Define the SIBs. These defines are used to set the sib_id field.
 */
#define RRCLOG_MIB       0
#define RRCLOG_SIB_1     1
#define RRCLOG_SIB_2     2
#define RRCLOG_SIB_3     3
#define RRCLOG_SIB_4     4
#define RRCLOG_SIB_5     5
#define RRCLOG_SIB_6     6
#define RRCLOG_SIB_7     7
#define RRCLOG_SIB_8     8
#define RRCLOG_SIB_9     9
#define RRCLOG_SIB_10    10
#define RRCLOG_SIB_11    11
#define RRCLOG_SIB_12    12
#define RRCLOG_SIB_13    13
#define RRCLOG_SIB_14    14
#define RRCLOG_SIB_15    15
#define RRCLOG_SIB_16    16
#define RRCLOG_SIB_13_1  17
#define RRCLOG_SIB_13_2  18
#define RRCLOG_SIB_13_3  19
#define RRCLOG_SIB_13_4  20
#define RRCLOG_SIB_15_1  21
#define RRCLOG_SIB_15_2  22
#define RRCLOG_SIB_15_3  23
#define RRCLOG_SIB_SB1   24
#define RRCLOG_SIB_SB2   25

LOG_RECORD_DEFINE(WCDMA_SIB)

  uint8 sib_id;                   /* SIB identity */

LOG_RECORD_END

/*--------------------------------------------------------------------------
                   LOG PACKET: WCDMA_SIGNALLING_MESSAGE
--------------------------------------------------------------------------*/
/* Define the maximum size of a signalling message. This define is used to size
 * the signalling_message field.
 */
#define MAX_SIG_MSG_SIZE 2048

/* Define the logical channel types. These defines are used to set the
 * channel_type field.
 */
#define RRCLOG_SIG_UL_CCCH      0   /* Uplink CCCH logical channel */
#define RRCLOG_SIG_UL_DCCH      1   /* Uplink DCCH logical channel */
#define RRCLOG_SIG_DL_CCCH      2   /* Downlink CCCH logical channel */
#define RRCLOG_SIG_DL_DCCH      3   /* Downlink DCCH logical channel */
#define RRCLOG_SIG_DL_BCCH_BCH  4   /* Downlink BCCH:BCH logical channel */
#define RRCLOG_SIG_DL_BCCH_FACH 5   /* Downlink BCCH:FACH logical channel */
#define RRCLOG_SIG_DL_PCCH      6   /* Downlink PCCH logical channel */
#define RRCLOG_EXTENSION_SIB    9
#ifdef FEATURE_3GPP_CSFB
#define RRCLOG_SIB_CONTAINER    10 
#endif

LOG_RECORD_DEFINE(WCDMA_SIGNALLING_MESSAGE)

  uint8 channel_type;         /* Logical channel type */
  uint8 rb_id;                /* Radio Bearer ID */
  uint16 length;              /* Signalling message length in bytes */
  uint8 signalling_message[MAX_SIG_MSG_SIZE]; /* Signalling message */

LOG_RECORD_END

/* Define the signalling message types.  These defines are used to report
 * events when signalling messages are sent or received.
 */

#define RRCLOG_ACTIVE_SET_UPDATE_MSG                            0
#define RRCLOG_ACTIVE_SET_UPDATE_COMPLETE_MSG                   1
#define RRCLOG_ACTIVE_SET_UPDATE_FAILURE_MSG                    2
#define RRCLOG_ASSISTANCE_DATA_DELIVERY_MSG                     3
#define RRCLOG_CELL_CHANGE_ORDER_FROM_UTRAN_MSG                 4
#define RRCLOG_CELL_CHANGE_ORDER_FROM_UTRAN_FAILURE_MSG         5
#define RRCLOG_CELL_UPDATE_MSG                                  6
#define RRCLOG_CELL_UPDATE_CONFIRM_MSG                          7
#define RRCLOG_COUNTER_CHECK_MSG                                8
#define RRCLOG_COUNTER_CHECK_RESPONSE_MSG                       9
#define RRCLOG_DOWNLINK_DIRECT_TRANSFER_MSG                     10
#define RRCLOG_HANDOVER_FROM_UTRAN_COMMAND_MSG                  11
#define RRCLOG_HANDOVER_FROM_UTRAN_FAILURE_MSG                  12
#define RRCLOG_HANDOVER_TO_UTRAN_COMMAND_MSG                    13
#define RRCLOG_HANDOVER_TO_UTRAN_COMPLETE_MSG                   14
#define RRCLOG_INITIAL_DIRECT_TRANSFER_MSG                      15
#define RRCLOG_INTER_RAT_HANDOVER_INFO_MSG                      16
#define RRCLOG_MEASUREMENT_CONTROL_MSG                          17
#define RRCLOG_MEASUREMENT_CONTROL_FAILURE_MSG                  18
#define RRCLOG_MEASUREMENT_REPORT_MSG                           19
#define RRCLOG_PAGING_TYPE_1_MSG                                20
#define RRCLOG_PAGING_TYPE_2_MSG                                21
#define RRCLOG_PHYSICAL_CHANNEL_RECONFIGURATION_MSG             22
#define RRCLOG_PHYSICAL_CHANNEL_RECONFIGURATION_COMPLETE_MSG    23
#define RRCLOG_PHYSICAL_CHANNEL_RECONFIGURATION_FAILURE_MSG     24
#define RRCLOG_RADIO_BEARER_RECONFIGURATION_MSG                 25
#define RRCLOG_RADIO_BEARER_RECONFIGURATION_COMPLETE_MSG        26
#define RRCLOG_RADIO_BEARER_RECONFIGURATION_FAILURE_MSG         27
#define RRCLOG_RADIO_BEARER_RELEASE_MSG                         28
#define RRCLOG_RADIO_BEARER_RELEASE_COMPLETE_MSG                29
#define RRCLOG_RADIO_BEARER_RELEASE_FAILURE_MSG                 30
#define RRCLOG_RADIO_BEARER_SETUP_MSG                           31
#define RRCLOG_RADIO_BEARER_SETUP_COMPLETE_MSG                  32
#define RRCLOG_RADIO_BEARER_SETUP_FAILURE_MSG                   33
#define RRCLOG_RRC_CONNECTION_REJECT_MSG                        34
#define RRCLOG_RRC_CONNECTION_RELEASE_MSG                       35
#define RRCLOG_RRC_CONNECTION_RELEASE_COMPLETE_MSG              36
#define RRCLOG_RRC_CONNECTION_REQUEST_MSG                       37
#define RRCLOG_RRC_CONNECTION_SETUP_MSG                         38
#define RRCLOG_RRC_CONNECTION_SETUP_COMPLETE_MSG                39
#define RRCLOG_RRC_FAILURE_INFO_MSG                             40
#define RRCLOG_RRC_STATUS_MSG                                   41
#define RRCLOG_SECURITY_MODE_COMMAND_MSG                        42
#define RRCLOG_SECURITY_MODE_COMPLETE_MSG                       43
#define RRCLOG_SECURITY_MODE_FAILURE_MSG                        44
#define RRCLOG_SIGNALLING_CONNECTION_RELEASE_MSG                45
#define RRCLOG_SIGNALLING_CONNECTION_RELEASE_INDICATION_MSG     46
#define RRCLOG_SYSTEM_INFORMATION_MSG                           47
#define RRCLOG_SYSTEM_INFORMATION_CHANGE_INDICATION_MSG         48
#define RRCLOG_TRANSPORT_CHANNEL_RECONFIGURATION_MSG            49
#define RRCLOG_TRANSPORT_CHANNEL_RECONFIGURATION_COMPLETE_MSG   50
#define RRCLOG_TRANSPORT_CHANNEL_RECONFIGURATION_FAILURE_MSG    51
#define RRCLOG_TRANSPORT_FORMAT_COMBINATION_CONTROL_MSG         52
#define RRCLOG_TRANSPORT_FORMAT_COMBINATION_CONTROL_FAILURE_MSG 53
#define RRCLOG_UE_CAPABILITY_ENQUIRY_MSG                        54
#define RRCLOG_UE_CAPABILITY_INFORMATION_MSG                    55
#define RRCLOG_UE_CAPABILITY_INFORMATION_CONFIRM_MSG            56
#define RRCLOG_UPLINK_DIRECT_TRANSFER_MSG                       57
#define RRCLOG_UPLINK_PHYSICAL_CHANNEL_CONTROL_MSG              58
#define RRCLOG_URA_UPDATE_MSG                                   59
#define RRCLOG_URA_UPDATE_CONFIRM_MSG                           60
#define RRCLOG_UTRAN_MOBILITY_INFORMATION_MSG                   61
#define RRCLOG_UTRAN_MOBILITY_INFORMATION_CONFIRM_MSG           62
#define RRCLOG_UTRAN_MOBILITY_INFORMATION_FAILURE_MSG           63
#define RRCLOG_SIB11bis_MSG                                    66
#define RRCLOG_SIB19_MSG                                       67
#ifdef FEATURE_3GPP_CSFB
#define RRCLOG_SIB_CONTAINER_COMPLETE_SIB                       68
#endif
#ifdef FEATURE_WCDMA_ETWS
#define RRCLOG_ETWS_PRIMARY_NOTIFICATION_WITH_SECURITY_MSG      69
#endif

/*Definitions*/

#define T_314_TIMER 0
#define T_315_TIMER 1
#define BPLMN_SRCH_TIMER 2
#define UNIFORM_OOS_SEARCH_TIMER 3

#define RRC_RAT_FDD 0
#define RRC_RAT_GSM 1

#define UOOS_STOP_ACQ_SUCCEED 0
#define UOOS_STOP_UPPER_LAYER 1
#define UOOS_STOP_PLMN_AVAILABLE 2

#define RRC_CS_CELL_UNKNOWN 0 
#define RRC_CS_SIB_READ_FAIL 1 
#define RRC_CS_CELL_BARRED 2
#define RRC_CS_CELL_NO_ACCESS 3
#define RRC_CS_CELL_BELOW_EXPECTED 4
#define RRC_CS_CELL_DIFFERENT_PLMN 5
#define RRC_CS_CELL_FORBIDDEN_LA 6
#define RRC_CS_S_FAIL 7

#define RRC_BPLMN_COMPLETED 0
#define RRC_BPLMN_TIMEOUT 1
#define RRC_BPLMN_ABORTED 2

#define RRC_WTOW_BPLMN 0
#define RRC_WTOG_BPLMN 1
#define RRC_GTOW_BPLMN 2

#define ACQ_DB_SCAN 0
#define FULL_FREQ_SCAN 1

#define MASTER_INFORMATION_BLOCK                  0
#define SYSTEM_INFORMATION_BLOCK_TYPE1            1
#define SYSTEM_INFORMATION_BLOCK_TYPE2            2
#define SYSTEM_INFORMATION_BLOCK_TYPE3            3
#define SYSTEM_INFORMATION_BLOCK_TYPE4            4
#define SYSTEM_INFORMATION_BLOCK_TYPE5            5
#define SYSTEM_INFORMATION_BLOCK_TYPE6            6
#define SYSTEM_INFORMATION_BLOCK_TYPE7            7
#define SYSTEM_INFORMATION_BLOCK_TYPE11           8
#define SYSTEM_INFORMATION_BLOCK_TYPE12           9
#define SYSTEM_INFORMATION_BLOCK_TYPE18           10
#define SCHEDULING_BLOCK_TYPE1                    11
#define SCHEDULING_BLOCK_TYPE2                    12
#define SYSTEM_INFORMATION_BLOCK_TYPE5BIS         13
#define SYSTEM_INFORMATION_BLOCK_TYPE11BIS        14
#define SYSTEM_INFORMATION_BLOCK_TYPE19           15
#define EXTENSION_SIB_TYPE_UNSUPPORTED            16
#define SIB_TYPE_UNSUPPORTED                      17


#ifdef FEATURE_WCDMA_LOG_RRC_STATES
#define SMEM_LOG_RRC_FACH_STATE              (SMEM_LOG_RRC_EVENT_BASE + 1)
#define SMEM_LOG_RRC_DCH_STATE               (SMEM_LOG_RRC_EVENT_BASE + 2)
#define SMEM_LOG_RRC_DCH_STATE_HS_ON         (SMEM_LOG_RRC_EVENT_BASE + 3)
#define SMEM_LOG_RRC_PCH_STATE               (SMEM_LOG_RRC_EVENT_BASE + 4)
#define SMEM_LOG_RRC_OOS_STATE               (SMEM_LOG_RRC_EVENT_BASE + 5)
#define SMEM_LOG_RRC_IDLE_STATE              (SMEM_LOG_RRC_EVENT_BASE + 6)
#endif /*FEATURE_WCDMA_LOG_RRC_STATES*/

#ifdef FEATURE_WCDMA_TO_LTE
#define WTOL_SUCCESS 0
#define WTOL_FAILURE 1
#define WTOL_ABORTED 2
#define WTOL_REDIR_CONN_RELEASE 0
#define WTOL_REDIR_CONN_REJECT  1
#define WTOL_REDIR_NONE         2
#define WTOL_REDIR_BLIND        3
#endif

/*Redirection status*/
#define WTOW_SUCCESS 0
#define WTOW_FAILURE 1
#define WTOW_ABORTED 2

/*Redirection cause*/
#define WTOW_REDIR_CONN_RELEASE 0
#define WTOW_REDIR_CONN_REJECT  1
#define WTOW_REDIR_NONE         2

/*Redirection status*/
#define WTOG_SUCCESS 0
#define WTOG_FAILURE 1
#define WTOG_ABORTED 2

/*Redirection cause*/
#define WTOG_REDIR_CONN_RELEASE 0
#define WTOG_REDIR_CONN_REJECT  1
#define WTOG_REDIR_NONE         2



#define RRC_SIB_CONTAINER_VALID                                0
#define RRC_SIB_CONTAINER_NOT_USED_ACQ_FAILURE                 1
#define RRC_SIB_CONTAINER_INCORRECT_INVALID_SIB1               2
#define RRC_SIB_CONTAINER_INCORRECT_ASN_DECODE_FAILURE         3
#define RRC_SIB_CONTAINER_INCORRECT_SIBDB_INDEX_NOT_FOUND      4
#define RRC_SIB_CONTAINER_INCORRECT_PLMN_NOT_GSM_MAP           5
#define RRC_SIB_CONTAINER_INCORRECT_MANDATORY_SIBS_NOT_PRESENT 6


#ifdef FEATURE_WCDMA_CSFB_CALL_OPT
#define RRC_BCCH_SETUP_SKIPPED            1
#define RRC_S_CRITERION_API_CHECK_PASSED  2
#define RRC_S_CRITERION_API_CHECK_FAILED  3
#define RRC_PCH_SETUP_SKIPPED             4
#endif

typedef PACKED struct {
rrc_cu_uu_status_e_type status;
rrc_cu_uu_cause_e_type cu_cause;
rrc_cu_uu_failure_cause_e_type failure_cause;
}rrc_cu_uu_status_event_type;

typedef PACKED struct {
rrc_inter_freq_hho_status_e_type status;
}rrc_log_inter_freq_hho_status_event_type;

#ifdef FEATURE_WCDMA_HS_FACH

typedef PACKED struct {
  uint8 rrc_efach_hsrach_event;
} rrclog_efach_epch_erachtype;

#endif

typedef PACKED struct {
  uint32 crnti_value;
} rrclog_crnti_event;

typedef PACKED struct {
  uint32 urnti_value;
} rrclog_urnti_event;

typedef PACKED struct {
  uint8 sib_type;
}rrclog_complete_sib_rcvd_event_type;

typedef PACKED struct {
  uint8 sib_container_sts;
}rrc_csfb_sib_container_event_type;

#ifdef FEATURE_WCDMA_CSFB_CALL_OPT

typedef PACKED struct PACKED_POST{
  uint8 status;
}rrclog_wcdma_csfb_call_opt_event_type;

#endif


/* At present keeping this under R7 feature should be __GNUC__ actually. But lint cribs */

typedef PACKED struct PACKED_POST
{
  uint8 channel_type;
  uint8 message_type;
} rrclog_sig_msg_event_type;

typedef PACKED struct PACKED_POST
{
  uint16 arfcn;
  uint8  ncc;
  uint8  bcc;
} rrclog_wcdma_to_gsm_reselection_start_event_type;

typedef PACKED struct PACKED_POST
{  
  uint8  status;
  uint8  failure_reason;
} rrclog_wcdma_to_gsm_reselection_end_event_type;


typedef PACKED struct PACKED_POST
{
  uint8  wcdma_handover_type;
  uint8  activation;
  uint8  activation_cfn;
  uint8  receive_cfn;
} rrclog_wcdma_inter_rat_handover_start_event_type;

typedef PACKED struct PACKED_POST
{
  uint8 handover_type;
  uint32 failure_cause;
  uint8 handover_status;
} rrclog_wcdma_inter_rat_handover_end_event_type;

typedef PACKED struct PACKED_POST
{
  uint16 freq;
  uint16 scr_code;
  uint32 csg_id;
} rrclog_wcdma_cell_selected_event_type;

typedef PACKED struct PACKED_POST
{
  uint8 mode;
} rrclog_wcdma_mode_change_event_type;

typedef PACKED struct PACKED_POST
{
  uint16 uarfcn;
  uint16 scr_code;
} rrclog_wcdma_handover_to_utran_start_event_type;

typedef PACKED struct PACKED_POST
{
  uint8 phychan_cfg_status;
} rrclog_phychan_cfg_status_event_type;

typedef PACKED struct PACKED_POST
{
  uint8 conn_rel_cause;
} rrclog_wcdma_conn_rel_cause_event_type;

typedef PACKED struct PACKED_POST
{
  uint8 conn_req_cause;
} rrclog_wcdma_conn_req_cause_event_type;

typedef PACKED struct PACKED_POST
{
  uint8 rrc_timer_id;
} rrclog_rrc_timer_expired_event_type;

typedef PACKED struct PACKED_POST
{
  uint16 wcdma_scan_time;
  uint16 gsm_scan_time;
} rrclog_UOOS_timer_used_event_type;

typedef PACKED struct PACKED_POST
{
  uint8 rat;
} rrclog_UOOS_timer_start_event_type;

typedef PACKED struct PACKED_POST
{
  uint8 reason;
} rrclog_UOOS_timer_stop_event_type;

typedef PACKED struct PACKED_POST
{
  uint16 rem_awake_time;
  uint16 rem_gsm_awake_time;
  uint16 rem_sleep_time;
} rrclog_wcdma_UOOS_time_remaining_event_type;

typedef PACKED struct PACKED_POST
{
  uint16 uarfcn;
  uint16 scr_code;
  uint8 sttd;
} rrclog_wcdma_acq_success_event_type;

typedef PACKED struct PACKED_POST
{
  uint8 reason;
} rrclog_wcdma_cell_selection_fail_event_type;

typedef PACKED struct PACKED_POST
{
  uint8 network_select_mode; /* Same as the one that comes in service request from MM*/
  uint8 search_type;
} rrclog_wcdma_bplmn_start_event_type;

typedef PACKED struct PACKED_POST
{
  uint8 bplmn_end_reason;
} rrclog_wcdma_bplmn_end_event_type;

typedef PACKED struct PACKED_POST
{
  uint8 scan_type;
} rrclog_wcdma_bplmn_scan_start_event_type;

typedef PACKED struct PACKED_POST
{
  uint8 scan_complete;
} rrclog_wcdma_bplmn_scan_end_event_type;

typedef PACKED struct PACKED_POST
{
  uint8 scan_type;
} rrclog_wcdma_rrccsp_scan_start_event_type;

typedef PACKED struct PACKED_POST
{
  uint8 timer_in_secs;
} rrclog_wcdma_eoos_skip_full_scan_timer_start_event_type;

#ifdef FEATURE_WCDMA_TO_LTE
typedef PACKED struct PACKED_POST {
  uint8 status;
  uint8 failure_cause;
} rrclog_wcdma_to_lte_resel_end_event_type;

typedef PACKED struct PACKED_POST {
  uint32 earfcn;
  uint16 cell_id;
  uint8 MCC[3];
  uint8 MNC[3];
} rrclog_wcdma_to_lte_resel_start_event_type;

typedef PACKED struct PACKED_POST{
uint8 status;
} rrclog_wcdma_to_lte_redir_end_event_type;

typedef PACKED struct PACKED_POST{
uint8 redir_cause;
uint8 MCC[3];
uint8 MNC[3];
uint8 wait_timer_in_secs;
} rrclog_wcdma_to_lte_redir_start_event_type;
#endif


typedef PACKED struct PACKED_POST{
uint8 status;
} rrclog_wcdma_to_gsm_redir_end_event_type;

typedef PACKED struct PACKED_POST{
uint8 redir_cause;
uint8 MCC[3];
uint8 MNC[3];
uint8 wait_timer_in_secs;
} rrclog_wcdma_to_gsm_redir_start_event_type;


typedef PACKED struct PACKED_POST{
uint8 status;
} rrclog_wcdma_to_wcdma_redir_end_event_type;

typedef PACKED struct PACKED_POST{
uint8 redir_cause;
uint16 target_freq;
} rrclog_wcdma_to_wcdma_redir_start_event_type;



#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY
typedef PACKED struct PACKED_POST{
  uint64 validity_timer_in_ms;
} rrclog_wcdma_ded_pri_val_tmr_start_event_type;
#endif

#ifdef FEATURE_FEMTO_CSG
typedef PACKED struct PACKED_POST{
  uint32 timer_value;
  uint8 timer_state;
} rrclog_wcdma_asf_timer_status_event_type;

typedef PACKED struct PACKED_POST{
  uint8 mcc[3];
  uint8 mnc[3];
  uint16 freq;
  uint16 psc;
  uint32 csg_id;
} rrclog_wcdma_csg_cell_detected_event_type;

#endif

#ifdef T_WINNT
  #error code not present
#endif
/*====================================================================
FUNCTION: rrc_log_event_inter_freq_hho_update_status

DESCRIPTION:
  This function logs the status of inter frequency hard hand off.

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void rrc_log_event_inter_freq_hho_update_status(rrc_inter_freq_hho_status_e_type status);

#endif /* RRCLOGGING_H */
