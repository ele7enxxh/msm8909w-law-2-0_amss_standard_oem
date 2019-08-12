#ifndef DSUCSDI_H
#define DSUCSDI_H
/*===========================================================================

            U M T S  C I R C U I T   S W I T C H E D   D A T A      
                 I N T E R N A L   H E A D E R   F I L E

DESCRIPTION
   This header file contains definitions used internally by the GSM/WCDMA 
   Circuit Switched data modules. 
  
   Copyright (c) 2003 - 2012 by Qualcomm Technologies INCORPORATED.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/inc/dsucsdi.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/09/12   ND      Memory reduction effort in 3gpp.
11/11/11   ND      Feature cleanup.
09/29/11   SA      Added support for TD-SCDMA.
04/05/11   SS      Q6 free floating support.
11/18/10   TTV     Added support for DSDS.
02/15/10   SA      Added ds_ucsd_is_atcop_call().
07/09/09   BS      Fixed offtarget lint medium and low's.
09/01/08   SA      Added modifications for CMI defeaturization.
08/25/08   SA      Added a macro to copy Speech preferences.
03/25/08   SA      Moved dsucsd_call_descriptor_type to dsucsd.h
03/19/08   SA      Moved ds_ucsd_nt_rlp_parameters_T to public header file.
11/15/07   AR      Added safety macros for watermark operations.
03/28/07   AR      Added FEATURE_UUS wrapping for EDGE only build.
02/06/07   AR      Migrate from SIO to SMD interface.
11/30/06   AR      Added HTORPC annotations for remoting API.
11/09/06   AR      Add call control and datapath interfaces to instance.
09/05/06   AR      Relocate REJECT_CALL & SET_CALL_DESCRIPTOR macros to 
                   common header. Added support for multiprocessor build.
06/02/06   AR      Add DS_UCSD_CALL_STATE_ORIG to call state emun.
02/15/06   AR      Remove obsolete call orig callback.
                   Add incall modify state tracking.
11/18/05   AR      Add support for multiple call instances.
09/27/05   AR      Added call instance state tracking.
09/15/05   AR      Add support for service change & fallback.
05/20/05   AR      Added supprot for call_progress_cb.
04/28/05   AR      Add support for exported ATCOP +CV120 parameters.
11/03/04   dwp     Add support for sending reject cause for incoming call.
08/16/04   AR      Add macro MAX_USER_BYTES.
05/05/04   AR      Add WCDMA constants for V.42bis compresion.
03/24/04   AR      Relocate ds_ucsd_call_tbl to common header file.
08/13/03   dwp     Add support for Call Confirmation callback for VT.
07/16/03   TMR     Modified for RLP version 2 changes
06/20/03   dgy     Added support for UMTS CS Data API and call table.
02/25/03   TMR     Created

===========================================================================*/


/*===========================================================================
Header files
===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"


#if ( defined( FEATURE_DATA_GCSD ) || defined( FEATURE_DATA_WCDMA_CS ) )

#include "msg.h"
#include "err.h"
#include "cm.h"

#include "ds3gsiolib.h"
#include "dsucsd.h"
#include "dsucsdtimer.h"
#include "dsucsdappif.h"

/*===========================================================================

                      #defines

===========================================================================*/
/* Global Debugging control... For individual files, debug can be turned
** on or off by defining DS_GCSD_FILE_DEBUG_ON or DS_GCSD_FILE_DEBUG_OFF 
** respectively prior to including this file.  The DS_UCSD_DEBUG flag will 
** indicate whether or not debugging is on or off in any particular source 
** file 
*/
#define DS_UCSD_DEBUG_GLOBAL

/* Set Debug message level */
#define DS_UCSD_DEBUG_MSG_LVL MSG_HIGH


/* Define the message macros based on debugging defines */
#if (defined (DS_UCSD_DEBUG_GLOBAL) && !defined (DS_GCSD_FILE_DEBUG_OFF)) || \
    defined (DS_GCSD_FILE_DEBUG_ON)

#define DS_UCSD_DEBUG

#define DS_UCSD_DEBUG_MSG(a) DS_UCSD_DEBUG_MSG_LVL(a,0,0,0)
#define DS_UCSD_DEBUG_MSG_1(a,b) DS_UCSD_DEBUG_MSG_LVL(a,b,0,0)
#define DS_UCSD_DEBUG_MSG_2(a,b,c) DS_UCSD_DEBUG_MSG_LVL(a,b,c,0)
#define DS_UCSD_DEBUG_MSG_3(a,b,c,d) DS_UCSD_DEBUG_MSG_LVL(a,b,c,d)

#else /* Debugging should be turned off */

#undef DS_UCSD_DEBUG

#define DS_UCSD_DEBUG_MSG(a)
#define DS_UCSD_DEBUG_MSG_1(a,b)
#define DS_UCSD_DEBUG_MSG_2(a,b,c)
#define DS_UCSD_DEBUG_MSG_3(a,b,c,d) 

#endif /* DS_UCSD_DEBUG_GLOBAL && !DS_GCSD_FILE_DEBUG_OFF || DS_GCSD_FILE_DEBUG_ON */


/* Define LOCAL macro if it hasn't already been defined */
#ifndef LOCAL
#define LOCAL static
#endif

#ifdef FEATURE_TDSCDMA
#define DS_UCSD_IS_WCDMA_TDS_MODE( mode ) \
    ( (SYS_SYS_MODE_WCDMA == mode) || (SYS_SYS_MODE_TDS == mode) )
#else
#define DS_UCSD_IS_WCDMA_TDS_MODE( mode ) \
    (SYS_SYS_MODE_WCDMA == mode)
#endif /* FEATURE_TDSCDMA */

/* Maximum number of CS domain Radio Access Bearers supported. */
#define SYS_MAX_CS_RABS      (1)

/* Macro to test for valid call instance ID range (TRUE indicates error) */
#define VALIDATE_INST_ID( inst_id ) \
      ((SYS_MAX_UCSD_CALLS <= inst_id) || \
       (DSUCSD_INVALID_INST_ID == inst_id))

/* Definitions for V.24 control signals */
#define V24_CONTROL_OFF           1
#define V24_CONTROL_ON            0

#define NUM_BITS_PER_BYTE            8   /* Number of bits in a byte      */

#define SDU_SIZE_FOR_64K            80   /* SDU size for 64kbps, in bytes */

/*number of user data bits per 20ms block*/
#define BITS_PER_9600_FRAME         60
#define FRAMES_PER_9600_20_MSEC      4
#define BYTES_PER_9600_20_MSEC      30   /* 4 X 60 bit frames = 240 bits = 30 bytes */


#define BITS_PER_14400_FRAME        36
#define FRAMES_PER_14400_20_MSEC     8
#define BITS_PER_14400_20_MSEC     288   /* 8 X 36 bit frames = 288 bits */
#define BYTES_PER_14400_20_MSEC     36   /* 288 bits = 36 bytes */

#define BYTES_PER_57600_10_MSEC     72   /* 576 bits = 72 bytes */

#define USER_BITS_PER_20MS       (8*36)  /* was (4 * 48)*/
#define BITS_PER_BLOCK_14400       288
#define BITS_PER_BLOCK_9600        192

/* Define the maximum number of user bytes per 20 msec that will ever
** pass through the protocol stack.  Currently, this is the number of
** bytes in a 14400 Transparent frame in which all bits are user bits.
** Similiarily, for WCDMA 10 msec TTI.
*/
#define MAX_USER_BYTES_PER_20_MSEC  BYTES_PER_14400_20_MSEC
#define MAX_USER_BYTES_PER_10_MSEC  BYTES_PER_57600_10_MSEC
#define MAX_USER_BYTES              MAX_USER_BYTES_PER_10_MSEC

/* Define the maximum number of Non-Transparent user bytes that will
** be buffered up in the CCWW tx/rx buffers.  We want at least 4X the
** maximum number that will be transmitted to account for the case
** when doing V42bis compression which has a maximum compression
** of 1 to 4.  We'll make it 10X to be safe because the processing
** functions will quit after buffers are half full.
** For WCDMA, use a factor of 5X as this uses same buffer size.
*/
#define MAX_BUFFERED_USER_BYTES        (10*MAX_USER_BYTES_PER_20_MSEC)
#define MAX_BUFFERED_USER_BYTES_WCDMA   (5*MAX_USER_BYTES_PER_10_MSEC)


#if (defined(FEATURE_DATA_GCSD_FAX) && defined(FEATURE_SMD))
/* Maximum SIO driver buffer size (in bytes) for GSM fax calls */
#define DSUCSD_MAX_SIO_FAX_BUFFER  0x600
#endif /* FEATURE_DATA_GCSD_FAX && FEATURE_SMD */


/* Definitions for OTA M1 and M2 bit positions in OTA RX and TX data 
** as well as macros for setting and retrieving these bits.  Note
** that this is currently only used for GSM 14.4 data
*/
#define OTA_M1_BIT 0x8000
#define OTA_M2_BIT 0x4000

/* The set M1 & M2 macro sets the bits in the first 16 bits
** of memory pointed to by the data_ptr, and then increments
** the data pointer past these first 16 bits
*/
#define DS_UCSD_SET_M1_M2_BITS(data_ptr,M1,M2)           \
   {                                                     \
      *((uint16 *) data_ptr) = 0x0000;                   \
      /*lint -e506 -e774 */                              \
      if (M1) *((uint16 *) data_ptr) |= OTA_M1_BIT;      \
      if (M2) *((uint16 *) data_ptr) |= OTA_M2_BIT;      \
      /*lint +e506 +e774 */                              \
      data_ptr = ((uint8 *) data_ptr) + sizeof (uint16); \
   }
 
/* The get M1 & M2 macro retrieves the bits in the first 16 bits
** of memory pointed to by the data_ptr, and then increments
** the data pointer past these first 16 bits
*/  
/*
#define DS_UCSD_GET_M1_M2_BITS(data_ptr,M1,M2)                    \
   {                                                              \
      M1 = (*((uint16 *) data_ptr) & OTA_M1_BIT) ? 0x01 : 0x00;   \
      M2 = (*((uint16 *) data_ptr) & OTA_M2_BIT) ? 0x01 : 0x00;   \
      data_ptr = ((uint8 *) data_ptr) + sizeof (uint16);          \
   }
*/
#define DS_UCSD_GET_M1_M2_BITS(data_ptr,M1)                    \
   {                                                              \
      M1 = (*((uint16 *) data_ptr) & OTA_M1_BIT) ? 0x01 : 0x00;   \
      data_ptr = ((uint8 *) data_ptr) + sizeof (uint16);          \
   }
   
   
/* +CBST attributes for 64k Multimedia call */
#define SPEED_64K_MULTIMEDIA_VAL      134
#ifdef FEATURE_MULTIMEDIA_FALLBACK 
  #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */


/* Mask to test for true data call types */
#define DS_UCSD_DATA_CALL_TYPE_MASK \
  ( DS_UCSD_ASYNC_CSD_CALL | \
    DS_UCSD_SYNC_CSD_CALL | \
    DS_UCSD_FAX_CALL | \
    DS_UCSD_VIDEO_TELEPHONY_CALL )

#define IS_CALL_TYPE( call_type, mask ) \
  ( (uint8)DS_UCSD_UNKNOWN_CALL != (uint8)((uint8)(call_type) & (uint8)(mask)) )

#define IS_VIDEO_CALL( call_type ) \
  IS_CALL_TYPE( call_type, DS_UCSD_VIDEO_TELEPHONY_CALL )   
#define IS_VOICE_CALL( call_type ) IS_CALL_TYPE( call_type, DS_UCSD_VOICE_CALL )


/* Events to the Transparent and Non-transparent 
** Async CS data protocol stacks 
*/
typedef enum
{
   DS_UMTS_CSD_NULL_EVENT,
   DS_UMTS_CSD_CONNECT_EVENT,
   DS_UMTS_CSD_DISCONNECT_EVENT,
   DS_UMTS_CSD_DISCONNECT_AFTER_FLUSH_EVENT,
   DS_UMTS_CSD_ONLINE_CMD_MODE_EVENT,
   DS_UMTS_CSD_ONLINE_DATA_MODE_EVENT,
   DS_UMTS_CSD_NUM_EVENTS                      /* Must remain last */
} ds_ucsd_event_T;

 
/* Indications from the Transparent and Non-transparent
** Async CS data protocol stacks 
*/
typedef enum
{
   DS_UMTS_CSD_CONNENCT_COMPLETE_IND,          /* Connect completed */
   DS_UMTS_CSD_DISCONNECT_COMPLETE_IND,        /* Disconnect completed */
   DS_UMTS_CSD_TERMINATE_CALL_IND,             /* Call should be terminated */
   DS_UMTS_CSD_NUM_INDS                        /* Must remain last */
} ds_ucsd_ind_T;

/* Modem control/status structures */
typedef struct
{
   uint8          RTS;
   uint8          DTR;
} ds_ucsd_sio_status_T;

/*filtered RS232 control bits*/
typedef struct filtered_rx_status_s
{
   uint8               X_status_rx;
   uint8               SA_status_rx;
   uint8               SB_status_rx;
} filtered_rx_status_T;

/* Typdef the event type */
typedef uint32 DSI_Event_T;

/* Bearer service type information, which is specified in the +CBST, +CHSN,
** or +CV120 command, or in the CS Data application call origination API.
*/
typedef struct ds_ucsd_cbst_param_s
{
   uint8 speed;                           /* Speed value                   */
   uint8 name;                            /* Synchronous or Asynchronous   */
   uint8 connection_element;              /* Transparent or Non-Transparent*/
}ds_ucsd_cbst_param_T;

typedef struct ds_ucsd_chsn_param_s
{
   uint8 waiur;                           /* Wanted air interface rate     */
}ds_ucsd_chsn_param_T;

typedef struct ds_ucsd_v120_param_s
{
  uint8   ra_header;                    /* Rate Adaption Header */
  uint8   multi_frame_est;              /* Multiple Frame Establishment */
  uint8   oper_mode;                    /* Mode of Operation */
  uint8   llink_id_neg;                 /* Logical Link Identifier Negotiation */
  uint8   assignor;                     /* Assignor / Assignee */
  uint8   band_neg;                     /* Inband / Outband Negotiation */
} ds_ucsd_v120_param_T;

typedef struct ds_ucsd_bearer_service_s
{
  ds_ucsd_cbst_param_T  cbst_info;      /* Bearer Configuration parameters */
  ds_ucsd_chsn_param_T  chsn_info;      /* Call Configuration parameters */
   ds_ucsd_v120_param_T  v120_info;       /* V.120 Rate Adaption parameters */
}ds_ucsd_bearer_service_type;

typedef struct ds_ucsd_chosen_bc_s
{
  uint8     mode;
  uint8     user_rate;
  uint8     fnur;                      /* Fixed network user rate         */
  uint8     waiur;                     /* Wanted air interface user rate  */
  uint8     modem_type;
  uint8     other_modem_type;
  uint8     itc;                       /* Information transfer capability */
  uint8     sync_async;
  uint8     connection_element;
  uint8     rate_adaption;
  uint8     other_rate_adaption;
  ds_ucsd_v120_param_T  v120_info;      /* V.120 Rate Adaption parameters */
} ds_ucsd_chosen_bc_T;

typedef struct ds_ucsd_chosen_llc_s
{
  boolean   present;                    /* Is LLC seeded into OTA message */
  uint8     user_rate;
  uint8     modem_type;
  uint8     itc;                        /* Information transfer capability */
  uint8     sync_async;
} ds_ucsd_chosen_llc_T;

typedef struct ds_ucsd_chosen_capability_s
{
   ds_ucsd_chosen_bc_T  chosen_bc;
   ds_ucsd_chosen_llc_T chosen_llc;
} ds_ucsd_chosen_capability_T;

/*---------------------------------------------------------------------------
  Bearer configuation and capability structures
---------------------------------------------------------------------------*/
typedef struct ds_ucsd_low_layer_compatibility_s
{
   uint8                   IEI;
   uint8                   coding_standard;
   uint8                   information_transfer_capability;
   uint8                   negotiation_indicator;                /*not used*/
   uint8                   transfer_mode;
   uint8                   information_transfer_rate;
   uint8                   structure;                            /*not used*/
   uint8                   configuration;                        /*not used*/
   uint8                   establishment;                        /*not used*/
   uint8                   symmetry;                             /*not used*/
   uint8                   information_transfer_rate_d_to_o;     /*not used*/
   uint8                   user_information_layer_1_protocol;
   uint8                   sync_async;
   uint8                   negotiation;
   uint8                   user_rate;
   uint8                   intermediate_rate;                    /*V.110 only*/
   uint8                   NIC_on_Tx;                            /*V.110 only*/
   uint8                   NIC_on_Rx;                            /*V.110 only*/
   uint8                   flow_control_on_Tx;                   /*V.110 only*/
   uint8                   flow_control_on_Rx;                   /*V.110 only*/
   uint8                   rate_adaption_header;                 /*V.120 only*/
   uint8                   multiple_frame_establishment_support; /*V.120 only*/
   uint8                   mode_of_operation;                    /*V.120 only*/
   uint8                   logical_link_identifier_negotiation;  /*V.120 only*/
   uint8                   assignor_assignee;                    /*V.120 only*/
   uint8                   in_band_out_band_negotiation;         /*V.120 only*/
   uint8                   number_of_stop_bits;
   uint8                   number_of_data_bits;
   uint8                   parity;
   uint8                   duplex_mode;
   uint8                   modem_type;
   uint8                   user_information_layer_2_protocol;
   uint8                   optional_layer_2_protocol_information;
   uint8                   user_information_layer_3_protocol;
   uint8                   optional_layer_3_protocol_information;
} ds_ucsd_low_layer_compatibility_T;

typedef struct ds_ucsd_negotiated_capability_s
{
  boolean                             present;
  cm_repeat_indicator_s_type          bc_repeat_indicator;
  cm_bearer_capability_s_type         bearer_capability_1;
  cm_bearer_capability_s_type         bearer_capability_2;
  cm_repeat_indicator_s_type          llc_repeat_indicator;
  cm_low_layer_compatibility_s_type   low_layer_compatibility_1;
  cm_low_layer_compatibility_s_type   low_layer_compatibility_2;
  cm_repeat_indicator_s_type          hlc_repeat_indicator;
  cm_high_layer_compatibility_s_type  high_layer_compatibility_1;
  cm_high_layer_compatibility_s_type  high_layer_compatibility_2;
} ds_ucsd_negotiated_capability_T;

#ifdef FEATURE_MULTIMEDIA_FALLBACK
#error code not present
#else
#define COPY_BEARER_CONFIG( to, from ) \
   (to)->bc_repeat_indicator        = (from)->bc_repeat_indicator; \
   (to)->bearer_capability_1        = (from)->bearer_capability_1; \
   (to)->bearer_capability_2        = (from)->bearer_capability_2;
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

#ifdef FEATURE_MULTIMEDIA_FALLBACK
#error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

/* Macro to configure for call rejection */
#define REJECT_CALL( params_ptr, reject_cause ) \
   params_ptr->accept = FALSE; \
   params_ptr->bc_repeat_indicator.present = FALSE; \
   params_ptr->bearer_capability_1.present = FALSE; \
   params_ptr->bearer_capability_2.present = FALSE; \
   \
   /* Fill in the reject cause data */ \
   params_ptr->cc_cause.present = TRUE; \
   params_ptr->cc_cause.coding_standard = STD_CODESTANDARD; \
   params_ptr->cc_cause.location = STD_LOCATION; \
   params_ptr->cc_cause.recommendation_present = FALSE; \
   params_ptr->cc_cause.diagnostic_length = 0; \
   params_ptr->cc_cause.cause_value = reject_cause; 

/* Macro to populate the call descriptor from call instance */
#define SET_CALL_DESCRIPTOR( desc_ptr, inst_ptr, bearer_ptr ) \
   desc_ptr->cm_call_id     = inst_ptr->cm_call_id; \
   desc_ptr->call_type      = inst_ptr->call_type; \
   desc_ptr->network_mode   = inst_ptr->network_mode; \
   desc_ptr->call_dir       = inst_ptr->call_dir; \
   desc_ptr->bearer_cap     = *bearer_ptr; \
   desc_ptr->rlp_parameters = inst_ptr->rlp_parameters; \
   desc_ptr->user_initiated_termination = inst_ptr->user_initiated_termination;

/* Define macros to protect against NULL watermarks on queue operations */
#define DSM_ENQUEUE(wm_ptr,item)                                    \
   if(wm_ptr) {                                                     \
     dsm_enqueue( wm_ptr, item );                                   \
   } else {                                                         \
     MSG_ERROR("Attempt to dsm_enqueue on NULL watermark",0,0,0);   \
   } 

#define DSM_DEQUEUE(wm_ptr)                                         \
   ( (wm_ptr)? dsm_dequeue( wm_ptr ) : NULL )

#define EMPTY_WATERMARK(wm_ptr)                                     \
  if(wm_ptr) {                                                      \
     dsm_empty_queue( wm_ptr );                                     \
   } else {                                                         \
     MSG_LOW("Attempt to dsm_empty_queue on NULL watermark",0,0,0); \
   } 

/* Enum for internal call state tracking */
typedef enum ds_ucsd_call_state
{
  DS_UCSD_CALL_STATE_NONE,
  DS_UCSD_CALL_STATE_ORIG,
  DS_UCSD_CALL_STATE_SETUP,
  DS_UCSD_CALL_STATE_ALERTING,
  DS_UCSD_CALL_STATE_ANSWERED,
  
  DS_UCSD_CALL_STATE_PRE_CONNECT,  /* Pre-connect demarcation line
                                      (for sublayer disconnect processing) */
  DS_UCSD_CALL_STATE_CONNECT_LL,
  DS_UCSD_CALL_STATE_CONNECT,
  DS_UCSD_CALL_STATE_MAX
} ds_ucsd_call_state_e_type;

/* Enum for in-call modify state tracking */
typedef enum ds_ucsd_modify_state
{
  DS_UCSD_MODIFY_STATE_IDLE,
  DS_UCSD_MODIFY_STATE_PENDING,
  DS_UCSD_MODIFY_STATE_MAX
} ds_ucsd_modify_state_e_type;

/* Enum for call control/datapath interface (SIO/AT Command or API) */
typedef enum ds_ucsd_interface
{
  DS_UCSD_IFACE_NONE,
  DS_UCSD_IFACE_SIO,
  DS_UCSD_IFACE_API,
  DS_UCSD_IFACE_MAX
} ds_ucsd_interface_e_type;

/* Max number of entries in the UMTS CS Data call table */ 
#define MAX_UCSD_CALL_TBL_ENTRIES  (SYS_MAX_UCSD_CALLS)

/* Structure to hold the information associated with a CS Data call */
typedef struct ds_ucsd_call_tbl_entry_s
{
  uint8                       inst_id;           /* CSData call instance  */
  cm_call_id_type             cm_call_id;        /* CM call ID            */
#ifdef FEATURE_DUAL_SIM
  sys_modem_as_id_e_type      subs_id;           /* Subscription id       */
#endif /* FEATURE_DUAL_SIM */
  sys_sys_mode_e_type         network_mode;      /* WCDMA or GSM          */
  ds_ucsd_call_type           call_type;         /* Type of call          */
  uint8                       call_mode;         /* Single/Dual mode      */
  ds_ucsd_client_id_type      client_id;         /* API client owning call*/
  ds_ucsd_call_state_e_type   state;             /* CSD state of call     */
  ds_ucsd_call_dir_e_type     call_dir;          /* Call direction        */
  ds_ucsd_interface_e_type    call_ctrl_iface;   /* Call Control interface*/
  ds_ucsd_interface_e_type    datapath_iface;    /* Datapath interface    */
  boolean                     was_user_answered;
  boolean                     user_initiated_termination;
  boolean                     protocol_stack_termination;
  boolean                     modify_allowed;
  ds_ucsd_bearer_service_type bearer_params;     /* Bearer Service info   */
  ds_ucsd_ota_channel_params_T chan_info;        /* Access stratum info   */
  ds_ucsd_data_rate_T         ota_rate;          /* OTA data rate,        **
                                                 ** equivalent to WAIUR   **
                                                 ** for NT or FNUR for    **
                                                 ** Transparent calls     */
  ds3g_siolib_port_e_type   sio_port_id;       /* Rm iface SIOLIB port  */
  ds_ucsd_negotiated_capability_T  neg_cap;      /* Negotiated Capability */
  ds_ucsd_dpath_info_type     dpath_params;      /* API/SIO data path info*/
  cm_setup_res_params_s_type  setup_params;      /* Setup paramater info  */
  ds_ucsd_answer_param_type   answer_params;     /* Answer parameter info */
  ds_ucsd_modify_state_e_type modify_state;      /* Incall modify state   */
  ds_ucsd_dial_modifier_info_type
                              dial_string_modifiers;
#ifdef FEATURE_UUS
  ds_ucsd_uus_info_type       uus_info;          /* UUIE for MO call      */
  cm_call_event_user_data_s_type
                              uus1_data;         /* UUIE for UUS1 MT call */
#endif /* FEATURE_UUS */
  uint32                      tx_data_counter;   /* UL data, in bytes     */
  uint32                      rx_data_counter;   /* DL data, in bytes     */
  cm_calling_party_bcd_no_T   calling_party_number; /* Incoming phone#    */
  ds_ucsd_nt_rlp_parameters_T rlp_parameters;    /* AsyncNT call RLP info */
}ds_ucsd_call_tbl_entry_type;

/* Structure to describe last connected call configuraiton. */
typedef struct ds_ucsd_last_call_info_s {
  boolean          present;
  uint8            inst_id;
  cm_call_id_type  cm_call_id;
  uint32           tx_data_counter;
  uint32           rx_data_counter;
  byte             sync_async;
} ds_ucsd_last_call_info_type;

/*===========================================================================
typedef the data traffic counter update callback prototype
===========================================================================*/
typedef boolean (*ds_ucsd_set_data_counters_cb_T)
(
  cm_call_id_type    call_id,
  boolean            incremental,
  uint32             rx_from_network_byte_cnt, 
  uint32             tx_to_network_byte_cnt
);

/*===========================================================================

FUNCTION DS_UCSD_GET_CS_INSTID

DESCRIPTION
   This function is to allocate and return a CS Data internal instance ID. It 
   returns NO_CALL_ID if all the entries in the call table, ds_ucsd_call_tbl[],
   are being used. 
        
DEPENDENCIES
   None
  
RETURN VALUE
   CS Data internal call instance ID or NO_CALL_ID.
    
SIDE EFFECTS
   Increment ds_ucsd_last_allocated_cs_inst_id if a call ID is allocated. Wrap
   it at MAX_INST_ID. 
   
   Mark the call table entry used. Set the call type in the entry to
   UNKNOWN_CALL_TYPE and CM call ID to NO_CALL_ID.
   
   The code that looks for an unused call table entry and subsequently marks
   the entry used is task-locked.
   
===========================================================================*/
extern uint8 ds_ucsd_get_instid
(
  uint8 *index_ptr
); 

/*===========================================================================

FUNCTION ds_ucsd_get_chosen_cap

DESCRIPTION
  This function fills in the passed in chosenCapability parameter
  with bearer capability and low-level compatibility information 
  which is derived from the bearer service type sent to the mobile. 
         
DEPENDENCIES
  ds_ucsd_store_bearer_service_type() must be called before this function 
  is called.
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
extern boolean
ds_ucsd_get_chosen_cap 
(
  const ds_ucsd_bearer_service_type  *service_ptr,
  ds_ucsd_chosen_capability_T        *chosen_Capability
);



/*===========================================================================

FUNCTION ds_ucsd_set_data_counters

DESCRIPTION
  This function sets the Tx/Rx data couters for a specified call ID.

DEPENDENCIES
  None
  
RETURN VALUE
  boolean:  The return value indicates if data call call updated.
    
SIDE EFFECTS
  None

===========================================================================*/
extern boolean 
ds_ucsd_set_data_counters
(
   cm_call_id_type    call_id,
   boolean            incremental,
   uint32             rx_from_network_byte_cnt, 
   uint32             tx_to_network_byte_cnt
);


#if defined(FEATURE_DATA_WCDMA_CS) && defined(FEATURE_DATA_UCSD_HANDOVER)

/*===========================================================================

FUNCTION ds_ucsd_handover_notify

DESCRIPTION
  This function is a callback routine for NAS/MN to notify DS of
  RAT handover events. The handover direction (reason) and new
  channel RAB identifier are passed from MN.

DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  Command is enqueued to DS task.

===========================================================================*/
void
ds_ucsd_handover_notify
(
  sync_enum_T direction,
  uint8       rab_id
);

#endif /* FEATURE_DATA_WCDMA_CS && FEATURE_DATA_UCSD_HANDOVER */

/*===========================================================================
FUNCTION ds_ucsd_is_atcop_call

DESCRIPTION
   This function is called by Ds3g to notify whether the call is originated
   or answered by ATCoP.

DEPENDENCIES
   None
  
RETURN VALUE
   TRUE  -- if it is an ATCoP call
   FALSE -- if it is a non-ATCoP call
    
SIDE EFFECTS
   None

===========================================================================*/
boolean ds_ucsd_is_atcop_call
(
  cm_call_id_type                 cm_call_id        /* call id of call    */
);

#ifdef FEATURE_DUAL_SIM
/*===========================================================================
FUNCTION ds_ucsd_set_call_mode_and_subs_id

DESCRIPTION
   If the call type is video call then the subs id set to the WCDMA subs id 
   and call mode to WCDMA.

DEPENDENCIES
   None
  
RETURN VALUE
   TRUE  -- If caluclated subs id not equal to -1.
   FALSE -- If caluclated subs id equal to -1.

SIDE EFFECTS
   None

===========================================================================*/
boolean ds_ucsd_set_call_mode_and_subs_id
(
  ds_ucsd_call_type      call_type,
  sys_sys_mode_e_type    *csd_mode,
  sys_modem_as_id_e_type *csd_subs_id
);
#endif /* FEATURE_DUAL_SIM */

#endif /* FEATURE_DATA_GCSD || FEATURE_DATA_WCDMA_CS */

#endif /* DSUCSDI_H */
