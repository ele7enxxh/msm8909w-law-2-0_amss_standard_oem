#ifndef TRMLOG_H
#define TRMLOG_H
/*===========================================================================

                   T R M    L O G   H E A D E R    F I L E

DESCRIPTION
   This file contains log types used internally by the TRM.
   It should NOT be included by any source file outside the TRM!

  Copyright (c) 2013-2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

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

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/trm/src/trmlog.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/18/15   sk      Update log v8 for eMBMS logging(796676)
02/08/15   sk      QSH logging format change for eMBMS(792847)
01/23/15   jm      Adding page request attempts to TRM metrics (CR: 778020)
01/16/15   jm      Adding TRM metric query support (CR: 778020)
12/15/14   mn      TRM QSH Handling to print debug information (CR: 771014).
12/12/14   sk      Logging Enhancement(737879)
11/12/14   ag      Added support for new circular buffer 
08/11/14   mn      TRM does not fill in logging information in Band 
                   tune APIs. (CR: 705717).
06/18/14   mn      Changes for DR-DSDS (CR: 651800).
03/03/14   mn      Adding support for connected mode WTR hopping.
05/10/13   mn      Adding support for TSTS.
12/17/12   rj      Logging change to log Set Client Prio.
03/22/12   sr      Logging change to log RF Capability and NV data.
07/26/11   sg      Added TRM V2 logging
10/18/10   ag      Merged DSDS functionality.
07/31/09   ag      Fixed lint warnings 
08/28/08   adw     Added FEATURE_MCS_TRM to featurize TRM for ULC.
06/16/05   ajn     Re-implemented logging as on "ostream" look-a-like.
05/26/05   ajn     Code review comment changes
05/06/05   ajn     Added official LOG_TRANSCEIVER_RESOURCE_MGR_C log code.
02/23/05   ajn     TRMLogClient now takes client id
02/22/05   ajn     Implemented logging structures using inheritance.
02/18/05   ajn     Added more log structures
02/11/05   ajn     Initial revision.

===========================================================================*/

#include "customer.h"

extern "C"
{
  #include "trm.h"
}

#include "trmi.h"
#include <stddef.h>
#include "trm_client.h"


/*===========================================================================

                              HEADER CONSTANTS

  Numeric values for the identifiers in this section should not be changed.

===========================================================================*/

#define TRM_GET_METRICS_MASK_DENIAL(lose_clnt, lose_reason, win_clnt, win_reason) \
                              (((lose_clnt) << 24) | \
                               ((lose_reason) << 16) | \
                               ((win_clnt) << 8) | \
                               (win_reason))

#define TRM_GET_METRICS_MASK_ATTEMPT(req_clnt, req_reason) \
                              (((req_clnt) << 8) | \
                               (req_reason))

/*-------------------------------------------------------------------------
  Version IDs 
-------------------------------------------------------------------------*/

#define TRMLOG_VERSION_1              1
#define TRMLOG_VERSION_2              2
#define TRMLOG_VERSION_3              3
#define TRMLOG_VERSION_4              4
#define TRMLOG_VERSION_5              5
#define TRMLOG_VERSION_6              6
#define TRMLOG_VERSION_7              7
#define TRMLOG_VERSION_8              8

#define TRMLOG_VERSION_CURRENT            TRMLOG_VERSION_8

#define TRM_LOG_GROUP_ASSIGNMENT_LOG_MAX  20

/*-------------------------------------------------------------------------
  Packet Type IDs 
-------------------------------------------------------------------------*/

typedef enum
{                 
  TRMLOG_ID_GRANT                     = 0,
  TRMLOG_ID_RESERVE_AT                = 1,
  TRMLOG_ID_REQUEST_N_NOTIFY          = 2,
  TRMLOG_ID_REQUEST                   = 3,
  TRMLOG_ID_CANCEL_REQUEST            = 4,
  TRMLOG_ID_RELEASE                   = 5,
  TRMLOG_ID_RETAIN_LOCK               = 6,
  TRMLOG_ID_CHANGE_PRIORITY           = 7,
  TRMLOG_ID_EXTEND_DURATION           = 8,
  TRMLOG_ID_CHANGE_DURATION           = 9,
  TRMLOG_ID_EXCHANGE                  = 10,
  TRMLOG_ID_UNLOCK_NOTIFY             = 11,
  TRMLOG_ID_REQUEST_N_NOTIFY_ENHANCED = 12,
  TRMLOG_ID_GRANT_ENHANCED            = 13,
  TRMLOG_ID_INIT                      = 14,
  TRMLOG_ID_REQUEST_BAND_TUNE         = 15,
  TRMLOG_ID_CONFIRM_BAND_TUNE         = 16,
  TRMLOG_ID_CANCEL_BAND_TUNE          = 17,
  TRMLOG_ID_BAND_GRANT                = 18,
  TRMLOG_ID_CHANGE_REASON             = 19,
  TRMLOG_ID_SET_CLIENT_PRIO           = 20,
  TRMLOG_ID_RETAIN_LOCK_ADVANCED      = 21,
  TRMLOG_ID_UNLOCK_NOTIFY_ADVANCED    = 22,
  TRMLOG_ID_ASYNC_EVENT_CALLBACK      = 23,
  TRMLOG_ID_ASYNC_RESPONSE            = 24
}
trmlog_id_type;

/*----------------------------------------------------------------------------
  TRM Metrics Type
----------------------------------------------------------------------------*/
typedef enum
{
  /* Indicates type of metric is based off of denials */
  TRM_METRIC_TYPE_DENIAL,

  /* Indicates type of metric is based off of requests */
  TRM_METRIC_TYPE_ATTEMPT,

  TRM_METRIC_TYPE_MAX
}trm_metric_type_t;



/*----------------------------------------------------------------------------
  Log Support Data
    This structure is packed so that TRM log does not have to
    account for any padding and to make it ABI independent.
----------------------------------------------------------------------------*/

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

typedef PACK( struct )
{
  /* SHDR / Hybrid mode indication */
  uint32                      rf_type;

  /* RF chain information          */
  uint32                      rf_mask;

  /* PAM/SVDO/SVLTE information    */
  uint32                      trm_config;

  /* RF Capability */
  uint32                      rf_capability;
 
  /* NV capabilities */
  uint32                      nv_rf_type;

  uint32                      nv_config_mask;

  /* Dual Receive Mask          */
  uint32                      dr_mask;

  /* Dual Receive State */
  uint32                      dr_state;
} trm_log_struct_type;

/*----------------------------------------------------------------------------
  Band State Info
    This structure is packed so that TRM log does not have to
    account for any padding and to make it ABI independent.
----------------------------------------------------------------------------*/
typedef PACK( struct )
{
  trm_log_band_info_type              bc_current;    /* Current Band          */
  trm_log_band_info_type              bc_requested;  /* Requested Band        */
  trm_log_band_info_type              bc_previous;   /* Previous Band         */
  uint8                               bc_state;      /* Band Class State Enum */
  uint8                               bc_waiting_on; /* Client waiting on ... */
  boolean                             bc_registered; /* Is band registered ?  */
  boolean                             reserved;      /* Urgent release flag */
} trm_log_band_state_info_type;

/*----------------------------------------------------------------------------
  TRM Group Assignment log entry.
----------------------------------------------------------------------------*/
typedef PACK( struct )
{
  uint8                               client_id;            /* Current client's Id     */
  uint8                               client_reason;        /* Current client's reason */
  int8                                requested_group;      /* Requested group value   */
  int8                                group;                /* Group value             */
  uint8                               winning_client;       /* Winning client id       */
  uint8                               winning_reason;       /* Reason used by winning 
                                                               client                  */
  uint8                               group_assingment_val; /* The group assignment value.
                                                               This tells the reason why
                                                               the client was denied   */
  uint8                               reserved;             /* Reserved field */
} trm_group_assignment_log_entry_type;

/*----------------------------------------------------------------------------
  TRM Group Assignment log type.
----------------------------------------------------------------------------*/
typedef PACK( struct )
{
  uint32                              num_entries; 
  trm_group_assignment_log_entry_type entries[TRM_LOG_GROUP_ASSIGNMENT_LOG_MAX];
} trm_group_assignment_log_info_type;

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

/*============================================================================

Class: TRMLog

Description:
  Allocates a buffer from diag's log pool, using TRM's log code, and
  accumulates data into the buffer.  Automatically commits the log buffer
  when the TRMLog object goes out of scope.

  If logging of TRM packets is not enabled, or if there is no availiable 
  memory for the buffer, logging requests using this object are silently
  discarded.
  
============================================================================*/

class TRMLog
{
  /*------------------------------------------------------------------------
    Private data members
  ------------------------------------------------------------------------*/
  private:

    /* Pointer to log buffer - if one can be allocated */
    uint8 * packet;

    /* Size of the log buffer */
    uint32  size;

    /* Amount of log buffer that has been used */
    uint32  used;


  /*------------------------------------------------------------------------
    Private Member functions
  ------------------------------------------------------------------------*/

    /* Allocate memory from diag's log buffer */
    void allocate( uint32 length );


  /*------------------------------------------------------------------------
    Public Member functions
  ------------------------------------------------------------------------*/
  public:

    /* Constructor.  Begin a log packet with the given log id and version 
       This is a Version 3 type log packet */
    TRMLog( trmlog_id_type id, trm_log_struct_type *trm_cfg_info, 
               uint8 ver = TRMLOG_VERSION_CURRENT, uint32 length = 0);

    /* Constructor.  length=0 --> uses default length */
    TRMLog( uint32 length=0 );

    /* Destructor.  Will commit the log packet, if necessary. */
    ~TRMLog();

    /* Commit the log packet */
    void commit();

    /* Allocate memory inside the log packet */
    void *alloc( uint32 length );

    /* Allocate memory inside the log packet, and copy in the given data */
    void log( const void *data, uint32 length );

    /* Set log packet id */
    void update_log_id(trmlog_id_type id);

    /* Set current band */
    void update_current_band (trm_rf_mode_map_type *rf_map);
    
    /* Log the async event callback */
    void log_async_event_cb (trm_async_event_cb_data_type* data);
    
    /* Log the async response*/
    void log_async_response (trm_async_event_response_data_type* data);

    /* Log the wakeup identifier */
    void log_wakeup_identifier(uint64 data);

    /* Returns "true" if the log packet can accept more data */
    operator int () const { return size > used; }


  /*------------------------------------------------------------------------
    "ostream" style logging functions.
  ------------------------------------------------------------------------*/

    TRMLog& operator << (uint8  u8);
    TRMLog& operator << (uint16 u16);
    TRMLog& operator << (uint32 u32);

    TRMLog& operator << (int8   i8);
    TRMLog& operator << (int16  i16);
    TRMLog& operator << (int32  i32);

    TRMLog& operator << (trm_client_enum_t client_id);
    TRMLog& operator << (const trm_client_state &state);
    TRMLog& operator << (const trm_bc_info_type &bcstate);
    TRMLog& operator << (const trm_client_info_t &info);
    TRMLog& operator << (const trm_pbr_info_type &info);

  /*------------------------------------------------------------------------
    Additional stream output operators can be declared outside the class,
    via: "TRMLog& operator << (TRMLog& log, sometype& t);"
  ------------------------------------------------------------------------*/
};

#define TRM_HIST_GET_CURR_REC_PTR() (&trm_time_history_buffer[trm_time_history_index])

/* Do not use this inside a condition */
#define TRM_HIST_INCR_INDEX() trm_time_history_index = ((trm_time_history_index + 1) % TRM_HIST_RECS)

typedef struct
{
   uint64   wakeup_identifier;
  
   /* Time information */
   uint32   duration;
   uint32   when;
   
   /* Frequency information */
   uint8    bands[MAX_BAND_INFO];
   uint8    dev_allocated[MAX_BAND_INFO];  
   uint8    num_bands;
	
   /* Enums */
   uint8    resource;
   uint8    reason;
   uint8    result; /* can be TRUE/FALSE or the Grant Enum */
   uint8    tune_info;
   
   /* Other 8 bit fields */  
   union trm_hist_specific_fields_type
   {
     uint8    num_pending_cbs_for_rat;
     uint8    sub_prio; 
     uint8    band_tune_action;
     uint8    other_client; /* used only for exchange */   
   }sf_u;
   
}trm_hist_rec_data_type;

/* History buffer record header structure */ 
typedef struct
{
  trm_timestamp_t  time;

  /* Log code: for QTA, INIT is used. */
  trmlog_id_type  subcode;  

  uint8  client;

  /* type of async cb if the subcode points to async callback record */
  trm_async_callback_type_enum_t  async_cb_type;  

  /* type of modify change event if async cb type is mod_chain_state */
  trm_modify_state_enum_t  mod_chg_type; 
  
}trm_hist_rec_hdr_type;

typedef struct
{
  trm_hist_rec_hdr_type  hdr;
  
  union trm_hist_rec_union_type
  {  
    /* For all the other subcodes except unlock, qta, connected hopping */
    trm_hist_rec_data_type details;

    /* For unlock subcode record */
    trm_unlock_callback_data unlock_data;

    /* For connected mode hop record */
    trm_connected_mode_hop_callback_data hop_data;

    /* For QTA i.e INIT subcode record */
    trm_qta_data_type  qta_data;
  }rec_u;
} trm_hist_rec_type;

#define TRM_HIST_RECS  60

extern trm_hist_rec_type trm_time_history_buffer[TRM_HIST_RECS];
extern int trm_time_history_index;

void trm_history_update_input_freq
(
  /* Arguments specific to each api */
  trm_hist_rec_data_type* details,
  
  /* Frequency info to be logged */
  trm_freq_input_type* freq_info
);

void trm_history_update_output_freq
(
  /* Arguments specific to each api */
  trm_hist_rec_data_type* details,
  
  /* Frequency info to be logged */
  trm_band_dev_alloc_type* band_alloc,
  
  /* Number of bands to copy */
  uint8   num_bands 
);

/*============================================================================

FUNCTION trm_history_insert_request

DESCRIPTION
  API to insert trm_request and req&not information in the history buffer
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void trm_history_insert_request
(
  /* Indicator of API that has been invoked */ 
  trmlog_id_type api_ind,
  trm_request_input_info* input,
  trm_request_return_data* output,
  /* Current 64 bit time */
  trm_timestamp_t  time
);

/*============================================================================

FUNCTION trm_history_insert_reserve_at

DESCRIPTION
  API to insert reserve_at information in the history buffer
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void trm_history_insert_reserve_at
(
  trm_reserve_at_input_info* input,
  /* Current 64 bit time */
  trm_timestamp_t  time
);

/*============================================================================

FUNCTION TRM_GROUP_ASSIGNMENT_LOG_RESET

DESCRIPTION
  API to reset the information in the group assignment log.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void trm_group_assignment_log_reset( void );

/*============================================================================

FUNCTION TRM_GROUP_ASSIGNMENT_LOG_ADD_ENTRY

DESCRIPTION
  API to add an entry to the group assignment log.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void trm_group_assignment_log_add_entry
( 
  const TRMClient* client_ptr 
);

/*============================================================================

FUNCTION TRM_GROUP_ASSIGNMENT_LOG_FLUSH

DESCRIPTION
  API to flush the group assignment log
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void trm_group_assignment_log_flush(TRMLog* log_ptr);

/*============================================================================

FUNCTION trm_metrics_increment_count

DESCRIPTION
  Inrements the metrics counter for a given
  winning/losing client/reason combination
 
  combo_mask is decoded pending on the metric_type:
     Denials:
     - 8 MSB for Losing Client (trm_client_enum_t)
     - 8 bits for Losing Client Reason (trm_reason_enum_t)
     - 8 bits for Winning Client (trm_client_enum_t) 
     - 8 LSB for Winning Client Reason (trm_reason_enum_t)
   
     Requests:
     - 8 bits for requesting client (trm_client_enum_t)
     - 8 LSB for requesting client reason (trm_reason_enum_t)
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void trm_metrics_increment_count(uint32 combo_mask, trm_metric_type_t metric_type);

/*============================================================================

FUNCTION trm_metrics_query_count

DESCRIPTION
  Retrieves the counter for a given
  winning/losing client/reason combination
 
  combo_mask is decoded pending on the metric_type:
     Denials:
     - 8 MSB for Losing Client (trm_client_enum_t)
     - 8 bits for Losing Client Reason (trm_reason_enum_t)
     - 8 bits for Winning Client (trm_client_enum_t) 
     - 8 LSB for Winning Client Reason (trm_reason_enum_t)
   
     Requests:
     - 8 bits for requesting client (trm_client_enum_t)
     - 8 LSB for requesting client reason (trm_reason_enum_t)
 
DEPENDENCIES
  None

RETURN VALUE
  uint16

SIDE EFFECTS
  None

============================================================================*/
uint16 trm_metrics_query_count(uint32 combo_mask, trm_metric_type_t metric_type);

/*============================================================================

FUNCTION TRM_LOG_INIT

DESCRIPTION
  Initializes any logging related information.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void trm_log_init( void );

#endif /* TRMLOG_H */

