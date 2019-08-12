#ifndef RF_HAL_CCS_MSM_H
#define RF_HAL_CCS_MSM_H

/*!
   @file
   rf_hal_ccs.h

   @brief

   @details

*/

/*==============================================================================

  Copyright (c) 2013 - 2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfmodem_jolokia/api/ccs/rf_hal_ccs.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
10/10/19   sbo     Support for dump of user requested registers during error handling
10/06/14   sbo     Increased the RFLM_CCS priority to 26
01/02/14   rca     CCS. Added support for CCS M3 Halt.
12/10/13   Saul    CCS. Command Port Payload Buffer Size Fix.
12/06/13   Saul    CCS. Command Port Buffer Overflow Fix
12/02/13   pl     remove obsolete defines, move definition to CCS header file
11/25/13   Saul    CCS. New CSS Data Buffer Allocation For All Techs.
11/05/13   Saul    CCS. Fixed Issue Seq
10/30/13   Saul    CCS. Device script support for RFFE WRITE EXT.
10/24/13   ra      Modified for FW_LIB build
10/04/13   ra      Moved modem specific CCS HAL support from RFA
07/14/13   sr      Initial version.

==============================================================================*/

#include "comdef.h"
#include "rflm_ccs_rf_event_intf.h"
#include "rflm_rf_event_intf.h"
#include "rf_hal_bus_types.h"
#if (RFLM_FEATURE_BUILD_MODE==RFLM_FEATURE_BUILD_NORMAL)
#include "rex.h"
#endif




#ifdef __cplusplus
extern "C" {
#endif

/*! @brief Enum for specifying task queue pairs */
typedef enum
{
  RF_HAL_RFCMD_TQ_NONE      = 0,  /*! No TQ */
  RF_HAL_RFCMD_TQ_PAIR0     = 1,  /*! TQ Pair # 1*/
  RF_HAL_RFCMD_TQ_PAIR1     = 2,  /*! TQ Pair # 2*/
  RF_HAL_RFCMD_TQ_ANY       = 5,  /*! Next free TQ */
  RF_HAL_RFCMD_TQ_ERR       = -1, /*! No TQ and error */
} rf_hal_rfcmd_tq_enum;


/*! First TQ pair enum */
#define RF_HAL_RFCMD_TQ_FIRST_PAIR  RF_HAL_RFCMD_TQ_PAIR0
/*! Last TQ pair enum */
#define RF_HAL_RFCMD_TQ_LAST_PAIR   RF_HAL_RFCMD_TQ_PAIR1
/*! Number of actual TQs. This should match CCS definitions */
#define RF_HAL_RFCMD_TQ_NUM         5
/*! Index of the dedicated TQ (command interface). This should match the RFCMD app interface */
#define RF_HAL_RFCMD_TQ_DEDICATED   0

/*! @brief number of temp buffers to hold the event data for dedicated cmd intf 
  Represents the number of concurrent threads at any one given time that are
  trying to access the command interface */
#define RF_CCS_CMD_INTF_EVENT_NUM_BUFS 4

#define RFCMD_APP_MAX_EXPECTED_CLIENTS 10  /*! Assume we never have more than 10 clients */


#define RFLM_CCS_THREAD_PRI              26 /*! IST priority */

typedef enum 
{
  RF_HAL_CLIENT_COMMON    =  0,
  RF_HAL_CLIENT_FTM       =  1,
  RF_HAL_CLIENT_GSM       =  4,
  RF_HAL_CLIENT_WCDMA     =  5,
  RF_HAL_CLIENT_1X        =  6,
  RF_HAL_CLIENT_EVDO      =  7,
  RF_HAL_CLIENT_LTE       =  8,
  RF_HAL_CLIENT_TDSCDMA   =  9,
  RF_HAL_CLIENT_GPS       = 10,
  RF_HAL_CLIENT_GSM2      = 11, /* Used for DSDS/DSDA operation */
  RF_HAL_CLIENT_INTERNAL  = 15, /* Reserved ID */
  RF_HAL_CLIENT_MAX       = 16,
} rf_hal_client_enum;

typedef enum
{
  RF_HAL_BUS_NON_MISSION_MODE = 0, /*! non mission mode like FTM, or bootup */
  RF_HAL_BUS_MISSION_MODE     = 1, /*! mission mode like WWAN or GPS online mode */
  RF_HAL_BUS_NUM_MODES        = 2  /*! number of modes */
} rf_hal_bus_mode_enum;

/*! @brief Struct holding client information */
typedef struct _rf_hal_bus_client_handle
{
  #define RF_HAL_BUS_CLIENT_NAME_SIZE 16       /*!< Number of characters in client name, including NULL terminator */
  rf_hal_client_enum client_enum;              /*!< Type of client */
  uint8 count;                                 /*!< Number of votes using this client */
  uint8 index;                                 /*!< The location of this client in the client array */
  rf_hal_bus_mode_enum mode;           /*!< Mission mode is essentially online mode */
} rf_hal_bus_client_handle_type;

/*! @brief Structure to hold useful info about state of RF CCS command interface
*/
typedef struct
{
  struct
  { 
    boolean fws_app_enabled;            /*! Indicates whether the RFCMD app is enabled */
    rf_hal_bus_client_handle_type * current_clients[RFCMD_APP_MAX_EXPECTED_CLIENTS]; /*! Array of client handles */
    uint8 num_clients[RF_HAL_BUS_NUM_MODES];  /*! Number of existing clients holding a vote, mission mode and non-mission mode */
    uint16 num_clients_total;                 /*! Total number of existing clients */
    uint32 mission_mode_mask;
  } rfcmd;
  struct
  {
    boolean is_allocated;             /*! Whether the TQ is allocated to RF */
    uint32 vote_mask;                 /*! Who has this reserved */
    ccs_rf_task_status_t last_error;  /*! Last non-zero error from CCS */
    uint32 num_errors;                /*! Number of errors from CCS task */
    uint32 internal_errors;           /*! Number of internal errors, e.g. no response*/
    void* curr_buf;                   /*! Pointer to the current event buffer */
    uint32 max_wait;                  /*! Max wait seen on this TQ for response (in microseconds) */
#if (RFLM_FEATURE_BUILD_MODE==RFLM_FEATURE_BUILD_NORMAL)
    rex_crit_sect_type lock;          /*! MUTEX for using this TQ */
#endif 
  } tq[RF_HAL_RFCMD_TQ_NUM];          /* Num best effort task queue pairs*2 + 1 dedicated task queue */
#if (RFLM_FEATURE_BUILD_MODE==RFLM_FEATURE_BUILD_NORMAL)
  struct
  {
    rex_crit_sect_type cmd_app_vote_lock;    /*! MUTEX for RFCMD app voting */
    rex_crit_sect_type cmd_tq_vote_lock;     /*! MUTEX for RFCMD TQ voting */
    rex_crit_sect_type cmd_event_buf_lock[RF_CCS_CMD_INTF_EVENT_NUM_BUFS]; /*! MUTEXes for internal event buffers (equivalent of SM event buffers but for RF use only */
  } locks;
#endif 
  #ifdef RF_CSS_CMD_INTF_DEBUG
  struct
  {
    void* mss_clk_ptr;               /*! Pointer to MSS_CLK register */
    void* mss_clamp_ptr;             /*! Pointer to MSS_CLAMP_IO register */
    void* ccs_pic4sts_ptr;           /*! Pointer to CCS_PIC4STS register. Tells us status of the SSBI/RFFE events */
  } debug;
  #endif
} rf_hal_bus_ccs_struct; 

/*! 
  @brief CCS command inferface payload size in words.
  @details
  The maximum number of 32-bit words that can be contained in the payload array.
  When fw vs rf includes discrepancy is fixed replace first line of this #def with CCS_RF_TASKQ_DATA_BUF_DEDICATED_NUM_WORDS
  */
#define RF_BUS_CCS_CMD_INTF_PAYLOAD_SIZE_WORDS ( CCS_RF_MAX_NUM_TRANSACTIONS*2*CCS_RF_CMD_SSBI_WORDS_PER_TRANS )
/*! 
  @brief CCS command interface event type
  */
typedef struct
{
  rflm_rf_ccs_event_description_buffer_header_type     header;
  rf_ccs_event_task_description_type              tasks[RFLM_CCS_RF_MAX_NUM_CCS_TASKS];
  uint32                                          payload[RF_BUS_CCS_CMD_INTF_PAYLOAD_SIZE_WORDS];
} rf_bus_ccs_cmd_intf_event_type;

typedef struct
{
  rf_hal_rfcmd_tq_enum  tq_pair;
  void*                 tq_sm_ptr[2];
  uint16                tq_sm_size[2];
} rf_hal_rfcmd_tq_type;


/*! 
  @brief RF Register Dump Table Entry Attributes
  */
#define MAX_RF_REGISTER_DUMP_TABLE_LIST_ENTRIES 50

typedef enum
{
  RF_HAL_REG_DUMP_BUS_RESOURCE_RFFE = 0x01,
  RF_HAL_REG_DUMP_BUS_RESOURCE_SSBI = 0x02,
  RF_HAL_REG_DUMP_BUS_RESOURCE_GRFC = 0x03,
} rf_hal_rf_register_dump_type_enum;

typedef struct
{
  char *description;
  rf_hal_rf_register_dump_type_enum type;
  uint8 channel;
  uint8 slave_id;
  boolean rd_delay;  // Set to TRUE if requesting one symbol delay at start of read RFFE frame
  boolean half_rate; // Set to TRUE for requesting half speed during read RFFE frame
  uint8 num_bytes;
  uint16 addr;
  uint32 data;
  boolean valid;
} rf_hal_rf_register_dump_table_entry_type;


/*----------------------------------------------------------------------------*/
void*
rf_hal_ccs_get_irat_script_addr
(
  uint8 buffer_index
);

/*----------------------------------------------------------------------------*/
void
rf_hal_ccs_fws_ipc_send
(
  void
);

/*----------------------------------------------------------------------------*/
void
rf_hal_ccs_enable_rfcmd_app
(
  void
);

/*----------------------------------------------------------------------------*/
void
rf_hal_ccs_disable_rfcmd_app
(
  void
);

/*----------------------------------------------------------------------------*/
void
rf_hal_ccs_set_grfc_mask
(
  boolean level_high,
  uint32 bit_mask
);

/*----------------------------------------------------------------------------*/
void
rf_hal_ccs_halt
(
   void
);
/*----------------------------------------------------------------------------*/
boolean rf_request_rfi_handle
(
  boolean enable,  
  uint32 tq,       
  uint32 client    
);
/*----------------------------------------------------------------------------*/

void rf_hal_bus_rfi_enable_app
(
   void
);

/*----------------------------------------------------------------------------*/
void rf_hal_bus_rfi_deint
(
   void
);

/*----------------------------------------------------------------------------*/
void rf_hal_bus_rfi_bootup
(
   void
);

/*----------------------------------------------------------------------------*/
rf_hal_rfcmd_tq_type rf_hal_bus_rfi_reserve_tq_pair
(  
   boolean enable, 
   rf_hal_rfcmd_tq_enum tq_pair,
   uint32 client
);

/*----------------------------------------------------------------------------*/
rf_hal_bus_result_type rf_hal_bus_rfi_event_generic
(
  rf_hal_rfcmd_tq_enum which_pair, 
  boolean force_immediate_timing 
);
rf_hal_bus_result_type rf_bus_ccs_command_interface_execute
(
   rf_bus_ccs_cmd_intf_event_type* cmd
);


/*----------------------------------------------------------------------------*/
void rf_hal_bus_debug
(
    rf_hal_bus_ccs_struct *
);

/*----------------------------------------------------------------------------*/
boolean rf_hal_rfi_init
(
   void
);

/*----------------------------------------------------------------------------*/
boolean 
event_builder_translate_payload
( 
  rf_ccs_event_task_description_type* task_block_ptr,
  boolean is_issue_seq
);

/*----------------------------------------------------------------------------*/
int rflm_ccs_post_mortem_read_debug_file(void);
#ifdef __cplusplus
}
#endif


#endif /* RF_HAL_CCS_MSM_H */
