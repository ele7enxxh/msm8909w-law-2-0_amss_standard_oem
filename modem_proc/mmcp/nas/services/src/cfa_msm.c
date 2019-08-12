/*===========================================================================
 
                    Callflow Analysis Logging Source File
 
DESCRIPTION
   This  file includes the appropriate source for CFA logging
   needed for the TARGET (MSM).
 
Copyright (c) 2000, 2001 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
 
                      EDIT HISTORY FOR FILE
 
$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/cfa_msm.c_v   1.6   15 May 2002 16:49:04   amcintos  $ 
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/services/src/cfa_msm.c#1 $  $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/28/12   ps      Fixed compilation warnings  
09/12/11   HC      Replacing the #include "gs.h" with #include "gs_v.h"
                   as a part of CMI activit 
07/11/06   UP      Updated to resolve compilation issue.
03/24/05   DA      Added GRR timers to cfa_log_timer_expired() 
07/03/03   AB      Added NAS messaget to GSM_MESSATE_SENT/RECEIVED payload
04/11/03   ATM     Fixed descrepancy in L3 location for ANALYZE logging
12/18/02   ATM     Updated high-volume WCDMA message list
11/07/02   ATM     Added GSM ANALYZE interface functions
10/10/02   ATM     Added GSM analyze events, log-code filtering, and removed
                   dependancy on log_config
09/19/02   ATM     Fixed misleading ERROR message
09/15/02   ATM     Incorporated GPRS1.0 improvements
09/05/02   ADG     Improved GPRS filtering granularity
08/12/02   ATM     Added RRC_RR to default logging list
07/16/02   ATM     Added GPRS filtering
05/15/02   ATM     Added some RRC messages to HIGH_VOL list
05/14/02   ATM     Added WCDMA filters
05/13/02   ATM     Fixed MSGSIZE problem --  off my IMH_T 
05/10/02   ATM     Added cfa_log_ex function 
                   and made debug statistics optional
04/10/02   ATM     Added Front-End Filter to CFA
02/12/02   ATM     Increased MIN/MAX truncation to 250 bytes (per rodgerc)
                   Also renamed packet constant
11/21/01   ATM     Reintroduced Generic Services (gs) as abstraction layer
08/24/01   ATM     Initial Revision
===========================================================================*/

/* ----------------------------------------------------- */
/*    INCLUDES                                           */  
/* ----------------------------------------------------- */
#include "mmcp_variation.h"
#include "cfa_v.h"               /* This header  */
#include "memory.h"            /* memcpy       */
#include "log.h"               /* log_commit   */
#include "msg.h"               /* MSG_MED      */
#include "cmd.h"               /* cmd_hdr_type */
#include "log_codes_umts.h"    /* for CFA code */
#ifdef DEBUG_USE_OLD_DIAG /* eg: UMTS_DIAG.00.00.17 */
  #error code not present
#else /* VU: MSMSHARED_UMTS_DIAG.xx.yy.zz */
  #include "umtsdiag.h"          /* for CFA CONFIG cmd */
  #include "umtsdiag_v.h"
#endif
#include "diagcmd.h"           /* for DIAG_SUBSYS_UMTS enum */

#include "ms.h"                /* for MSG_SETs */
#include "gs_v.h"                /* for QUEUE_IDs */

#ifdef FEATURE_GSM
#include "event.h"             /* for EVENT logging     */
#include "com_iei_v.h"           /* for PD     values     */
#include "ms_timer_v.h"          /* for TIMER EXPIRED EVENT logging */
#include "rr_l2_l1.h"             /* for L3 locations in RR_L2 logging */
#include "rr_l2.h"             /* for L3 locations in RR_L2 logging */
#endif /* FEATURE_GSM */
#include <stringl/stringl.h>
#include "nasutils.h"
/* ----------------------------------------------------- */
/*    TYPE DEFINITIONS/VARAIBLES                         */  
/* ----------------------------------------------------- */
#ifdef FEATURE_GSM
/* *************************************************************
 *  KEY EVENT IDs 
 * ************************************************************/
#define CFA_CHANNEL_FIELD  (5)

/* ========= Messages IDs for RR -- from rr_defs.h ========= */
/******************************************/
/* RR Timer and Timeout Value Definitions */
/******************************************/
#define CFA_T3110                             0x02
#define CFA_T3126                             0x03
#define CFA_T3122                             0x04
#define CFA_T3124                             0x05
#define CFA_T3178                             0x60
#define CFA_T3158                             0x61
#define CFA_T3174                             0x62
#define CFA_T3176                             0x63
#define CFA_T3142                             0x67
#define CFA_T3146                             0x68

/**************************************/
/* definition of the RR message types */
/* according to 04.08 10.4            */
/**************************************/
#define CFA_ADDITIONAL_ASSIGNMENT             0x3B
#define CFA_IMMEDIATE_ASSIGNMENT              0x3F
#define CFA_IMMEDIATE_ASSIGNMENT_EXTENDED     0x39
#define CFA_IMMEDIATE_ASSIGNMENT_REJECT       0x3A
#define CFA_CIPHERING_MODE_COMMAND            0x35
#define CFA_CIPHERING_MODE_COMPLETE           0x32
#define CFA_ASSIGNMENT_COMMAND                0x2E
#define CFA_ASSIGNMENT_COMPLETE               0x29
#define CFA_ASSIGNMENT_FAILURE                0x2F
#define CFA_HANDOVER_COMMAND                  0x2B
#define CFA_HANDOVER_COMPLETE                 0x2C
#define CFA_HANDOVER_FAILURE                  0x28
#define CFA_PHYSICAL_INFORMATION              0x2D
#define CFA_CHANNEL_RELEASE                   0x0D
#define CFA_PARTIAL_RELEASE                   0x0A
#define CFA_PARTIAL_RELEASE_COMPLETE          0x0F
#define CFA_PAGING_RESPONSE                   0x27
#define CFA_CHANNEL_MODE_MODIFY               0x10
#define CFA_RR_STATUS                         0x12
#define CFA_CHANNEL_MODE_MODIFY_ACKNOWLEDGE   0x17
#define CFA_FREQUENCY_REDEFINITION            0x14
#define CFA_MEASUREMENT_REPORT                0x15
#define CFA_CLASSMARK_CHANGE                  0x16
#define CFA_CLASSMARK_ENQUIRY                 0x13

/* ========= Messages IDs for MM -- from mm_mm.h ========= */
/* 
 * Message Types for mm to mm peer messages - These are as defined in
 * GSM 04.08
 */
#define CFA_IMSI_DETACH_INDICATION      0x01
#define CFA_LOCATION_UPDATING_ACCEPT    0x02
#define CFA_LOCATION_UPDATING_REJECT    0x04
#define CFA_LOCATION_UPDATING_REQUEST   0x08
#define CFA_AUTHENTICATION_REJECT       0x11
#define CFA_AUTHENTICATION_REQUEST      0x12
#define CFA_AUTHENTICATION_RESPONSE     0x14
#define CFA_IDENTITY_REQUEST            0x18
#define CFA_IDENTITY_RESPONSE           0x19
#define CFA_TMSI_REALLOCATION_COMMAND   0x1a
#define CFA_TMSI_REALLOCATION_COMPLETE  0x1b
#define CFA_CM_SERVICE_ACCEPT           0x21
#define CFA_CM_SERVICE_REJECT           0x22
#define CFA_CM_SERVICE_ABORT            0x23
#define CFA_CM_SERVICE_REQUEST          0x24
#define CFA_CM_REESTABLISHMENT_REQUEST  0x28
#define CFA_ABORT                       0x29
#define CFA_MM_STATUS                   0x31
#define CFA_AUTHENTICATION_FAILURE      0x1C
#define CFA_PAGING_RESPONSE             0x27


/* 
 * Message Types for GMM to mm peer messages - These are as defined in
 * GSM 04.08
 */
#define CFA_GMM_ATTACH_REQUEST                         0x01
#define CFA_GMM_ATTACH_ACCEPT                          0x02
#define CFA_GMM_ATTACH_COMPLETE                        0x03
#define CFA_GMM_ATTACH_REJECT                          0x04
#define CFA_GMM_DETACH_REQUEST                         0x05
#define CFA_GMM_DETACH_ACCEPT                          0x06
#define CFA_GMM_ROUTING_AREA_UPDATE_REQUEST            0x08
#define CFA_GMM_ROUTING_AREA_UPDATE_ACCEPT             0x09
#define CFA_GMM_ROUTING_AREA_UPDATE_COMPLETE           0x0A
#define CFA_GMM_ROUTING_AREA_UPDATE_REJECT             0x0B
#define CFA_GMM_SERVICE_REQUEST                        0x0C
#define CFA_GMM_SERVICE_ACCEPT                         0x0D
#define CFA_GMM_SERVICE_REJECT                         0x0E
#define CFA_GMM_PTMSI_REALLOCATION_COMMAND             0x10
#define CFA_GMM_PTMSI_REALLOCATION_COMPLETE            0x11
#define CFA_GMM_AUTHENTICATION_AND_CIPHERING_REQUEST   0x12
#define CFA_GMM_AUTHENTICATION_AND_CIPHERING_RESPONSE  0x13
#define CFA_GMM_AUTHENTICATION_AND_CIPHERING_REJECT    0x14
#define CFA_GMM_AUTHENTICATION_AND_CIPHERING_FAILURE   0x1C
#define CFA_GMM_IDENTITY_REQUEST                       0x15
#define CFA_GMM_IDENTITY_RESPONSE                      0x16
#define CFA_GMM_STATUS                                 0x20
#define CFA_GMM_INFORMATION                            0x21

/* ========= Messages IDs for CC -- from cc_cc.h ========= */
/* 
 * Message Types for cc to cc peer messages - These are as defined in
 * GSM 04.08
 */
#define CFA_ALERTING           0x01
#define CFA_CALL_CONFIRMED     0x08
#define CFA_CALL_PROCEEDING    0x02
#define CFA_CONNECT            0x07
#define CFA_CONNECT_ACK        0x0f
#define CFA_EMERGENCY_SETUP    0x0e
#define CFA_PROGRESS           0x03
#define CFA_SETUP              0x05
#define CFA_MODIFY             0x17
#define CFA_MODIFY_COMPLETE    0x1f
#define CFA_MODIFY_REJECT      0x13
#define CFA_USER_INFORMATION   0x10
#define CFA_HOLD               0x18
#define CFA_HOLD_ACK           0x19
#define CFA_HOLD_REJECT        0x1a
#define CFA_RETRIEVE           0x1c
#define CFA_RETRIEVE_ACK       0x1d
#define CFA_RETRIEVE_REJ       0x1e
#define CFA_DISCONNECT         0x25
#define CFA_RELEASE            0x2d
#define CFA_RELEASE_COMPLETE   0x2a
#define CFA_CONGESTION_CONTROL 0x39
#define CFA_NOTIFY             0x3e
#define CFA_STATUS_MSG         0x3d
#define CFA_STATUS_ENQUIRY     0x34
#define CFA_START_DTMF         0x35
#define CFA_STOP_DTMF          0x31
#define CFA_STOP_DTMF_ACK      0x32
#define CFA_START_DTMF_ACK     0x36
#define CFA_START_DTMF_REJ     0x37
#define CFA_FACILITY           0x3a
#define CFA_REGISTER           0x3b
/*
 * SMS Messages
 */
#define CFA_CP_DATA            0x01
#define CFA_CP_ACK             0x04
#define CFA_CP_ERROR           0x10

/* This generic two-byte identity can be used as */
/* payload for all 4 events defined              */
/* NOTE: I am packing the bytes backwards so it  *
 *       can be read in as a word16 by the event *
 *      database                                 */
typedef PACKED struct PACKED_POST{
   byte    id;          /* Message ID or Timer ID          */
   byte    set;         /* Protocol Descriminator or Queue */
   byte    channel;     /* channel */
} cfa_generic_identity_type;
#endif /* FEATURE_GSM */

/* *************************************************************
 *  CFA PACKET DEFINITION
 * ************************************************************/
#define CFA_MAX_PACKET_LENGTH (255)
LOG_RECORD_DEFINE(LOG_UMTS_CALL_FLOW_ANALYSIS_C)
  byte   Version;
  byte   Sequence;
  byte   Direction; 
  byte   Task;
  byte   Queue;
  byte   MsgSize;                          /* Size sent in packet */
  byte   MsgSet;                           /* Mesasge Set         */
  byte   MsgId;                            /* Message ID          */
  word   MsgLen;                           /* Actual size of message */
  byte   Message[CFA_MAX_PACKET_LENGTH-4]; /* First MsgSize bytes of data */
LOG_RECORD_END

/* *************************************************************
 *  CFA LOGGING FILTER -- now performed by logcodes
 * ************************************************************/
#define CFA_HIGH_VOLUME_C   LOG_UMTS_CFA_FIRST_FILTER_C

/* *************************************************************
 *  CFA LOG PACKET TRIMMING
 * ************************************************************/
#define MAX_PAYLOAD  250      /* Largest payload allowed before truncation   */
#define MIN_PAYLOAD  250      /* Value we truncate to (make this >=6 bytes)  */
/* Default settings */
static byte cfa_trim_if_over_len   = MAX_PAYLOAD;
static byte cfa_trim_to_len        = MIN_PAYLOAD;

/* *************************************************************
 *  CFA LOGGING STATS
 * ************************************************************/
#undef DEBUG_CFA_STATS   /* Define this to record statistics */



#ifdef DEBUG_CFA_STATS

static rex_crit_sect_type  cfa_stats_crit_sect;
 #define STATS(cmd) cmd
 enum {
    CFA_FUNCTION_CALLS,
    CFA_BYTES_TO_LOG,
    CFA_ALLOCS,
    CFA_BYTES_LOGGED,
    BREAK_IN_SEQ,
    LAST_SEQ,
    CFA_STAT_SIZE
 };

 static long cfa_stats[CFA_STAT_SIZE] = {0,0,0,0,0,0};

 /* KLUDGE - LIMIT to the first 36 message sets */
 #define CFA_MAX_MSGSET_STATS 36
 static long msgset_stats[CFA_MAX_MSGSET_STATS] = { 
                    0,0,0,0,0,0,0,0,0,
                    0,0,0,0,0,0,0,0,0,
                    0,0,0,0,0,0,0,0,0,
                    0,0,0,0,0,0,0,0,0 };

#else
 /* DEFINE as an empty function */
 #define STATS(cmd)
#endif 

/* *************************************************************
 *  CFA DIAG CONFIG SETTINGS
 * ************************************************************/
/* OPERATIONS */
enum {
  CFA_SET_MSG_FILTER,     /* Now obsolete */
  CFA_GET_CATEGORY_NAME,  /* Now obsolete */
  CFA_GET_TRIM_PARAMS,
  CFA_SET_TRIM_PARAMS,
  CFA_GET_MSGSET_STATS,
  CFA_GET_CFA_STATS
};

/* OPERATION 0 - SET MSG FILTER       - Now obsolete */
/* OPERATION 1 - GET CATEGORY NAME    - Now obsolete */

/* OPERATION 2/3 - GET/SET TRIM PARAMS */
typedef PACKED struct PACKED_POST{
  byte trim_at;
  byte trim_to;
} cfa_trim_params_type;

#ifdef DEBUG_CFA_STATS
/* OPERATION 4/5 - GET MSGSET/CFA STATS */
typedef PACKED struct PACKED_POST{
  long last;
  long stat[CFA_MAX_MSGSET_STATS];
} cfa_get_stats_rsp_type;
#endif

/* UNION OF ALL REQUEST OPERATIONS */
typedef PACKED union PACKED_POST{
  cfa_trim_params_type           trim_params; 
} cfa_diag_config_op_req_type;

/* UNION OF ALL RESPONSE OPERATIONS */
typedef PACKED union PACKED_POST{
  cfa_trim_params_type           trim_params; 
  STATS( cfa_get_stats_rsp_type         stats; )
} cfa_diag_config_op_rsp_type;

/* ----------------------------------------------------- */
/*    FUNCTION PROTOTYPES                                */  
/* ----------------------------------------------------- */
#ifdef FEATURE_GSM
static void cfa_send_event_if_neccessary( byte queue, byte* msg_ptr );
#endif /* FEATURE_GSM */

/* ----------------------------------------------------- */
/*    FUNCTIONS                                          */  
/* ----------------------------------------------------- */
/*===========================================================================

FUNCTION  cfa_allow_msgset

DESCRIPTION
  HELPER FUNCTION that governs whether message gets logged or not 
  
RETURN VALUE
  logcode - 0 means not allowed

===========================================================================*/
static word cfa_allow_msgset( byte msgSet, byte msgId )
{
    word logcode = LOG_UMTS_CFA_FIRST_FILTER_C + msgSet;
    STATS( ++msgset_stats[msgSet%(CFA_MAX_MSGSET_STATS-1)]; )
    /* Step 1. If 0x7001, then provisionally allow this message */
    if( TRUE == log_status( LOG_UMTS_CALL_FLOW_ANALYSIS_C ) )
    {
        logcode = LOG_UMTS_CALL_FLOW_ANALYSIS_C;
    }
    /* Step 2. Else check message set and see if it is allowed */
    else if( FALSE == log_status( logcode ) )
    {
        return 0;
    }

    /* Step 3. Finally, check if msgID is a high-volume message and if HV is allowed. */
    /* Step 3a. If high-volume messages are allowed, then return true. */
    if( TRUE == log_status( CFA_HIGH_VOLUME_C ) )
    {
        return logcode;
    }
    /* Step 3b. Else check if this is a HighVolume message (return false if yes). */
    switch (msgSet)
    {
    /* ********************************
     * GSM LOWER_LAYER FILTERS
     * ********************************/
    case MS_RR_L1:        
        if ( 0x82 == msgId  || /* MPH_BLOCK_QUALITY_IND     */
             0x8E == msgId  || /* MPH_SERVING_DED_MEAS_IND  */
             0x8F == msgId  || /* MPH_SERVING_IDLE_MEAS_IND */
             0x92 == msgId   ) /* MPH_SURROUND_MEAS_IND     */
        {
            return 0;
        }     

#if defined(FEATURE_GSM_GPRS)
        if ( 0x36 == msgId ||   /* MPH_NC_MEASUREMENTS_REQ */
             0x37 == msgId ||   /* MPH_STOP_NC_MEASUREMENTS_REQ */
             0x38 == msgId ||   /* MPH_EXT_MEASUREMENTS_REQ */
             0x39 == msgId ||   /* MPH_STOP_EXT_MEASUREMENTS_REQ */
             0x82 == msgId ||   /* MPH_BLOCK_QUALITY_IND */
             0xB1 == msgId ||   /* MPH_NC_MEASUREMENT_IND */
             0xB2 == msgId  )   /* MPH_EXT_MEASUREMENT_IND */
        {
            return 0;
        }
#endif /* FEATURE_GSM_GPRS */

        break;
    case MS_L2_L1:        
        if ( 0x00 == msgId  || /* PH_DATA_REQ          */
             0x03 == msgId  || /* PH_DATA_IND          */
             0x05 == msgId   ) /* PH_READY_TO_SEND_IND */
        {
            return 0;
        }     
        break;
    case MS_L1_L1:        
        if ( 0x05 == msgId  || /* L1_REP_PERIOD_IND      */
             0x07 == msgId   ) /* L1_SC_CALL_IN_TASK_CNF */
        {
            return 0;
        }
        break;
    /* ********************************
     * WCDMA LOWER_LAYER FILTERS
     * ********************************/
    case MS_WCDMA_L1_LOCAL:        
        if ( 0x00 == msgId  || /* SRCH_DONE_CMD */
             0x01 == msgId  || /* SRCH_OPS_CMD */
             0x1E == msgId  || /* RF_DO_TX_PWR_LIM_CMD */
             0x20 == msgId  || /* DL_PHYCHAN_CFG_CMD */
             0x21 == msgId  || /* DL_PHYCHAN_FRM_BDRY_CMD */
             0x29 == msgId  || /* DL_TRBLK_CRC_STATUS_CMD */
             0x2E == msgId  || /* UL_TRCH_CODING_LOG_DONE_CMD */
             0x2F == msgId  || /* DRX_LOG_DONE_CMD */
             0x30 == msgId   ) /* DRX_SLEEP_CMD */
        {
            return 0;
        }
        break;
    case MS_WCDMA_L1_EXT:        
        if ( 0x01 == msgId  || /* CPHY_IDLE_REQ */
             0x08 == msgId   ) /* CPHY_ACQ_REQ */
        {
            return 0;
        }
        break;
    case MS_WCDMA_RRC_INT:
        if ( 0x01 == msgId  || /* RRC_DOWNLINK_SDU_IND */
             0x04 == msgId  || /* RRC_RESET_L1_REQ */
             0x1A == msgId  || /* RRC_STATE_CHANGE_IND */
             0x20 == msgId  || /* RRC_LLC_CMD_REQ */
             0x21 == msgId   ) /* RRC_LLC_CMD_CNF */
        {
            return 0;
        }
        break;
    case MS_WCDMA_RRC_L1:
        if ( 0x01 == msgId  || /* RRC_CPHY_ACQ_CNF */
             0x04 == msgId   ) /* RRC_CPHY_IDLE_CNF */
        {
            return 0;
        }
        break;
#if defined(FEATURE_GSM_GPRS)
    /* ********************************
     * GPRS FILTERS
     * ********************************/
    case MS_MAC_L1:
        if ( 0x01 == msgId  ||   /* L1_MAC_USF_DETECTED */
             0x02 == msgId  ||   /* L1_MAC_QUAL_MEASURMENTS */
             0x41 == msgId  ||   /* MAC_PH_DATA_IND */
             0x42 == msgId  ||   /* MAC_PH_RANDOM_ACCESS_CONF */
             0x86 == msgId  ||   /* MAC_L1_PACCH_RRBP */
             0x8D == msgId  ||   /* MAC_L1_QUALITY_RPT */
             0xC1 == msgId   )   /* MAC_PH_DATA_REQ */
        {
            return 0;
        }
        break;
    case MS_MAC_RR:
        if ( 0x05 == msgId  ||   /* GRR_MAC_SEND_MEAS_RPT */
             0x0B == msgId  ||   /* GRR_MAC_UL_SB_EST */
             0x80 == msgId  ||   /* MAC_GRR_DATA_IND */
             0x85 == msgId  )   /* MAC_GRR_MEAS_RPT_CNF */
        {
            return 0;
        }
        break;
    case MS_MAC_RLC:
        if ( 0x40 == msgId  ||   /* RM_DL_PKT_ACKNACK_REQ */
             0x83 == msgId   )   /* MR_UL_PKT_ACKNACK_IND */
        {
            return 0;
        }
        break;
    default:
        break;
    /* GPRS messages under case MS_RR_L1 included with GSM messages above */
    
#endif /* FEATURE_GSM_GPRS */
    }/*end switch(msgSet)*/

    /* Step 3c. Else not a HighVolume message (return true). */
    return logcode;
}


/*===========================================================================
FUNCTION:  cfa_log_packet
===========================================================================*/
/*******************************************************************************
 *
 *  FORMAT:   PS d t q L mmmm...
 *
 *   P       is CFA log format version
 *   S       is log sequence number (incremented after every call)
 *   d       is Direction code -- now obsolete
 *   t       is TaskId code    -- now obsolete
 *   q       is QueueId code   used to determine direction/task
 *   L       is Message Length (0..255)
 *   mmmm... is Message 
 *            Message is assumed to have IMH_T header:
 *                       byte MsgSet
 *                       byte MsgId
 *                       word MsgLen
 *                       byte contents[MsgLen]
 *
 ******************************************************************************/
void  cfa_log_packet( byte   queue,
                      word   length,
                      byte  *msg_ptr )
{
#ifdef FEATURE_GSM
  cfa_send_event_if_neccessary( queue, msg_ptr );
#endif /* FEATURE_GSM */
  cfa_log_packet_ex( queue,
                     msg_ptr[0],
                     msg_ptr[1],
                     length -sizeof(IMH_T),
                     msg_ptr+sizeof(IMH_T) );
}


/*===========================================================================
FUNCTION:  cfa_log_packet_ex
===========================================================================*/
void  cfa_log_packet_ex( byte  queue,
                         byte  msg_set,
                         byte  msg_id,
                         word  length,
                         byte *msg_ptr )
{
#define CFA_VERSION  0xA1

   LOG_UMTS_CALL_FLOW_ANALYSIS_C_type    *log_ptr; /* pointer to log packet */
   static byte                       sequence = 0;
          word                       msg_size = length;
          word                       log_code = LOG_UMTS_CALL_FLOW_ANALYSIS_C;
          int                        packet_size;

   STATS(rex_enter_crit_sect(&cfa_stats_crit_sect);)
   STATS( ++cfa_stats[CFA_FUNCTION_CALLS]; )
   STATS(rex_leave_crit_sect(&cfa_stats_crit_sect);)

   if ( (msg_size != 0) && !msg_ptr )
   {
     MSG_ERROR_0("cfa_log_packet : bad msg_ptr!");
     return;
   }
   else if ( 0 == (log_code = cfa_allow_msgset(msg_set, msg_id)) )
   {
     return;
   }
   else
   {
     STATS(rex_enter_crit_sect(&cfa_stats_crit_sect);)
     STATS( if( cfa_stats[CFA_ALLOCS] && (sequence-cfa_stats[LAST_SEQ]) > 1 ){++cfa_stats[BREAK_IN_SEQ];} )
     STATS(rex_leave_crit_sect(&cfa_stats_crit_sect);)
     ++sequence;
     
     if ( msg_size > cfa_trim_if_over_len )
     {
       msg_size = cfa_trim_to_len;
     }

     packet_size = (int) ( FPOS(LOG_UMTS_CALL_FLOW_ANALYSIS_C_type, Message) + msg_size );
     STATS(rex_enter_crit_sect(&cfa_stats_crit_sect);)
     STATS( cfa_stats[CFA_BYTES_TO_LOG]+=packet_size; )
     STATS(rex_leave_crit_sect(&cfa_stats_crit_sect);)

     log_ptr = (LOG_UMTS_CALL_FLOW_ANALYSIS_C_type    *)log_alloc_ex(log_code, (unsigned int)packet_size);
     
     if (log_ptr != NULL)
     {
         STATS(rex_enter_crit_sect(&cfa_stats_crit_sect);)
         STATS( ++cfa_stats[CFA_ALLOCS]; )
         STATS( cfa_stats[CFA_BYTES_LOGGED]+=packet_size; )
         STATS( cfa_stats[LAST_SEQ]=sequence; )
         STATS(rex_leave_crit_sect(&cfa_stats_crit_sect);)

         /* Set the log code back to the standard code: 0x7001 */
         log_set_code ( log_ptr, LOG_UMTS_CALL_FLOW_ANALYSIS_C );


         /* Fill in first 2 bytes w/Version,Sequence info */
         log_ptr->Version   = (byte)CFA_VERSION;
         log_ptr->Sequence  = sequence;

         /* Fill in next 4 bytes w/dir,task,queue,msglen */
         log_ptr->Direction =       1;           /* Obsolete */
         log_ptr->Task      =       1;           /* Obsolete */
         log_ptr->Queue     =       queue;
         log_ptr->MsgSize   = (byte)msg_size+sizeof(IMH_T);

         /* Fill remaining bytes w/msg */
         log_ptr->MsgSet    =       msg_set;
         log_ptr->MsgId     =       msg_id;
         log_ptr->MsgLen    =       length;
         if (msg_ptr)
            (void)memscpy((byte*)log_ptr->Message, msg_size, msg_ptr, msg_size);

         /* Write packet to log */
         log_commit((void *)log_ptr);
     }
     else
     {
        /* no log buffer or logging not enabled, so do nothing */
        return;
     }
   }
}


/*===========================================================================

FUNCTION: cfa_diag_config_pkt

DESCRIPTION
  This procedure processes a gsm diag configuration request -- it is modelled
  after LOG_PROCESS_CONFIG_PKT because it may fold into this packet someday.
  
  The packet contains a opcode, so many different operations can be 
  implemented here.

RETURN VALUE
  Pointer to response packet.

  This command effects the diagnostic configuration.  
===========================================================================*/
PACKED void * cfa_diag_config_pkt (
  PACKED void *req_pkt_ptr, /* pointer to request packet */
  uint16 pkt_len            /* length of request packet  */
)
{
  DIAG_SUBSYS_UMTS_UMTS_CFA_CONFIG_F_req_type *req_ptr = (DIAG_SUBSYS_UMTS_UMTS_CFA_CONFIG_F_req_type *)req_pkt_ptr;
  DIAG_SUBSYS_UMTS_UMTS_CFA_CONFIG_F_rsp_type *rsp_ptr;
  STATS( int x; )
  /* Set minimum rsp_len */
  unsigned int rsp_len = FPOS(DIAG_SUBSYS_UMTS_UMTS_CFA_CONFIG_F_rsp_type, op_data);

  /* Pointers to unions for operation specific data */
  cfa_diag_config_op_req_type *req_op_ptr;
  cfa_diag_config_op_rsp_type *rsp_op_ptr;

  /* we'll reuse this enum from log.h */
  int status = 0;  /* SUCCESS */

  /* Local working variables */
  unsigned int operation = req_ptr->operation;

  /* First, determine the length of the response.
   * Second, allocate the response.
   * Last, fill in the response. */

  req_op_ptr = (cfa_diag_config_op_req_type *) req_ptr->op_data;

  /* This switch statement computes rsp_len for the operation */
  switch (operation) {
  case CFA_GET_TRIM_PARAMS:
      rsp_len += sizeof(cfa_trim_params_type);
      break;

  case CFA_SET_TRIM_PARAMS:
      rsp_len += sizeof(cfa_trim_params_type);
      break;

#ifdef DEBUG_CFA_STATS
  case CFA_GET_MSGSET_STATS:
      STATS( rsp_len += sizeof(long) * (CFA_MAX_MSGSET_STATS+1); )
      break;

  case CFA_GET_CFA_STATS:
      STATS( rsp_len += sizeof(long) * (CFA_STAT_SIZE+1); )
      break;
#endif
  default:
      status = 2; /* BAD OPERATION */
  } /* end switch */

  /* Now that we know the size, allocate rsp */
  rsp_ptr = (DIAG_SUBSYS_UMTS_UMTS_CFA_CONFIG_F_rsp_type *)diagpkt_subsys_alloc((int)DIAG_SUBSYS_UMTS, UMTS_CFA_CONFIG_F, rsp_len );

  if(rsp_ptr)
  {   /* FILL IN RSP */
      rsp_ptr->operation = operation;

      rsp_op_ptr = (cfa_diag_config_op_rsp_type *) rsp_ptr->op_data;

      /* This switch statement actually performs the operation */
      switch (operation) {
      case CFA_SET_TRIM_PARAMS:
          cfa_trim_if_over_len = req_op_ptr->trim_params.trim_at;
          cfa_trim_to_len      = req_op_ptr->trim_params.trim_to;
          /*Fall through and report back new values */
      case CFA_GET_TRIM_PARAMS:
          rsp_op_ptr->trim_params.trim_at = cfa_trim_if_over_len;
          rsp_op_ptr->trim_params.trim_to = cfa_trim_to_len;
          break;

#ifdef DEBUG_CFA_STATS
      case CFA_GET_MSGSET_STATS:
          rsp_op_ptr->stats.last = CFA_MAX_MSGSET_STATS;
          for( x=0; x<CFA_MAX_MSGSET_STATS; ++x)
          {
              rsp_op_ptr->stats.stat[x] = msgset_stats[x];
          }
          break;

      case CFA_GET_CFA_STATS:
          rsp_op_ptr->stats.last = CFA_STAT_SIZE;
          STATS(rex_enter_crit_sect(&cfa_stats_crit_sect);)
          for( x=0; x<CFA_STAT_SIZE; ++x)
          {
              rsp_op_ptr->stats.stat[x] = cfa_stats[x];
          }
          STATS(rex_leave_crit_sect(&cfa_stats_crit_sect);)
          break;
      default:
          break;
#endif
      } /* end switch */

      rsp_ptr->status = (unsigned long) status;

  }

  return rsp_ptr;

} /* cfa_diag_config_pkt */

#ifdef FEATURE_GSM

/*===========================================================================

FUNCTION  cfa_send_event_if_neccessary

DESCRIPTION
  HELPER FUNCTION that governs whether message should issue an event:
    EVENT_GSM_MESSAGE_SENT
    EVENT_GSM_MESSAGE_RECEIVED
    EVENT_GSM_TIMER_EXPIRED
    EVENT_GSM_COUNTER_EXPIRED

RETURN VALUE
  none

===========================================================================*/
static void cfa_send_event_if_neccessary( byte queue, byte* msg_ptr )
{
    IMH_T*  msg = (IMH_T*)msg_ptr;
    byte*   l3msg = NULL ;
    boolean isTx = FALSE;
    

    switch (msg->message_set)
    {
    /* ********************************
     * KEY GSM OTA MESSAGES
     * ********************************/
#ifdef FEATURE_GSM
    case MS_RR_L2:        
        switch( msg->message_id ) {
        /* Messages containing OTA to send */
        case DL_DATA_REQ:
        {
            l3msg = msg_ptr+FPOS(dl_data_req_T, layer3_message[0]);;
            isTx  = TRUE;
            break;
        }
        case DL_RECONNECT_REQ:
        {
            l3msg = msg_ptr+FPOS(dl_reconnect_req_T, layer3_message[0]);;
            isTx  = TRUE;
            break;
        }
        case DL_RESUME_REQ:
        {
            l3msg = msg_ptr+FPOS(dl_resume_req_T, layer3_message[0]);;
            isTx  = TRUE;
            break;
        }
        case DL_UNIT_DATA_REQ:
        {
            l3msg = msg_ptr+FPOS(dl_unit_data_req_T, layer3_message[0]);;
            isTx  = TRUE;
            break;
        }
        case DL_ESTABLISH_REQ:
        {
            l3msg = msg_ptr+FPOS(dl_establish_req_T, layer3_message[0]);;
            isTx  = TRUE;
            break;
        }

        /* Messages containing OTA received */
        case DL_DATA_IND:
        {
            l3msg = msg_ptr+FPOS(dl_data_ind_T, layer3_message[0]);;
            isTx  = FALSE;
            break;
        }
        case DL_UNIT_DATA_IND:
        {
            l3msg = msg_ptr+FPOS(dl_unit_data_ind_T, layer3_message[0]);;
            isTx  = FALSE;
            break;
        }
        default:
            break;
        }/* end msgID */
        if( l3msg )
        {
            l2_channel_type_T channel_type = ((dl_data_ind_T *) msg_ptr)->l2_channel_type;
            if( DCCH  != channel_type  &&
                SACCH != channel_type  &&
                SDCCH != channel_type  &&
                FACCH_F != channel_type &&
                FACCH_H != channel_type)
            {   /* Compensate for pseudo-length field */
                ++l3msg;
            }
            cfa_log_ota_msg ( isTx, channel_type, l3msg[0], l3msg[1] );
        }
        else
        {
            return;
        }

        break;
#endif /* FEATURE_GSM */

    /* ********************************
     * KEY TIMEOUTS
     * ********************************/
    case MS_TIMER:
        if( MID_TIMER_EXPIRY == msg->message_id )
        {
            timer_expiry_T* expiry = (timer_expiry_T*) msg_ptr;
            cfa_log_timer_expired( queue, expiry->timer_id );
        }
        break;
    /* ********************************
    * KEY COUNTER EXPIRES -- TBD
    * ********************************/
    default:
        break;
    }
}

/*===========================================================================
FUNCTION:  cfa_log_ota_msg
===========================================================================*/
void cfa_log_ota_msg ( boolean isUplink, 
                       byte    channel, 
                       byte    pd, 
                       byte    msgid )
{
    cfa_generic_identity_type id;
    boolean log_event = FALSE;
    
    id.set     = GET_PD(pd);
    id.id      = msgid;
    id.channel = channel;

    switch (id.set) {
    case PD_CC:    
      id.id &= 0x3F; /* MASK off direction bit */
      switch ( id.id ) {
      case CFA_ALERTING:
      case CFA_CALL_CONFIRMED:
      case CFA_CALL_PROCEEDING:
      case CFA_CONNECT:
      case CFA_CONNECT_ACK:
      case CFA_EMERGENCY_SETUP:
      case CFA_PROGRESS:
      case CFA_SETUP:
      case CFA_MODIFY:
      case CFA_MODIFY_COMPLETE:
      case CFA_MODIFY_REJECT:
      case CFA_USER_INFORMATION:
      case CFA_HOLD:
      case CFA_HOLD_ACK:
      case CFA_HOLD_REJECT:
      case CFA_RETRIEVE:
      case CFA_RETRIEVE_ACK:
      case CFA_RETRIEVE_REJ:
      case CFA_DISCONNECT:
      case CFA_RELEASE:
      case CFA_RELEASE_COMPLETE:
      case CFA_CONGESTION_CONTROL:
      case CFA_NOTIFY:
      case CFA_STATUS_MSG:
      case CFA_STATUS_ENQUIRY:
      case CFA_START_DTMF:
      case CFA_STOP_DTMF:
      case CFA_STOP_DTMF_ACK:
      case CFA_START_DTMF_ACK:
      case CFA_START_DTMF_REJ:
      case CFA_FACILITY:
          log_event = TRUE;
          break;
      default:
          break;
      }
      break;
    case PD_SMS:
      switch ( id.id ) {
      case CFA_CP_DATA:
      case CFA_CP_ACK:
      case CFA_CP_ERROR:
      default:
           break;
      }
      break;
    case PD_SS:
        switch ( id.id ) {
        case CFA_FACILITY:
        case CFA_REGISTER:
        case CFA_RELEASE_COMPLETE:
        default:
           break;
         }
         break;

    case PD_MM:
        id.id &= 0x3F; /* MASK off direction bit */
        switch ( id.id ) {
        case CFA_IMSI_DETACH_INDICATION:
        case CFA_LOCATION_UPDATING_ACCEPT:
        case CFA_LOCATION_UPDATING_REJECT:
        case CFA_LOCATION_UPDATING_REQUEST:
        case CFA_AUTHENTICATION_REJECT:
        case CFA_AUTHENTICATION_REQUEST:
        case CFA_AUTHENTICATION_RESPONSE:
        case CFA_IDENTITY_REQUEST:
        case CFA_IDENTITY_RESPONSE:
        case CFA_TMSI_REALLOCATION_COMMAND:
        case CFA_TMSI_REALLOCATION_COMPLETE:
        case CFA_CM_SERVICE_ACCEPT:
        case CFA_CM_SERVICE_REJECT:
        case CFA_CM_SERVICE_ABORT:
        case CFA_CM_SERVICE_REQUEST:
        case CFA_CM_REESTABLISHMENT_REQUEST:
        case CFA_ABORT:
        case CFA_MM_STATUS:
            log_event = TRUE;
            break;
        default: 
        break;
         }
         break;

     case PD_GMM:
        id.id &= 0x3F; /* MASK off direction bit */
        switch ( id.id ) {
        case CFA_GMM_ATTACH_REQUEST:
        case CFA_GMM_ATTACH_ACCEPT:
        case CFA_GMM_ATTACH_COMPLETE:
        case CFA_GMM_ATTACH_REJECT:
        case CFA_GMM_DETACH_REQUEST:
        case CFA_GMM_DETACH_ACCEPT:
        case CFA_GMM_ROUTING_AREA_UPDATE_REQUEST:
        case CFA_GMM_ROUTING_AREA_UPDATE_ACCEPT:
        case CFA_GMM_ROUTING_AREA_UPDATE_COMPLETE:
        case CFA_GMM_ROUTING_AREA_UPDATE_REJECT:
        case CFA_GMM_SERVICE_REQUEST:
        case CFA_GMM_SERVICE_ACCEPT:
        case CFA_GMM_SERVICE_REJECT:
        case CFA_GMM_PTMSI_REALLOCATION_COMMAND:
        case CFA_GMM_PTMSI_REALLOCATION_COMPLETE:
        case CFA_GMM_AUTHENTICATION_AND_CIPHERING_REQUEST:
        case CFA_GMM_AUTHENTICATION_AND_CIPHERING_RESPONSE:
        case CFA_GMM_AUTHENTICATION_AND_CIPHERING_REJECT:
        case CFA_GMM_AUTHENTICATION_AND_CIPHERING_FAILURE:
        case CFA_GMM_IDENTITY_REQUEST:
        case CFA_GMM_IDENTITY_RESPONSE:
        case CFA_GMM_STATUS:
        case CFA_GMM_INFORMATION:
           log_event = TRUE;
           break;
        default: 
       break;
        }
        break;

    case PD_RR:
        switch ( id.id ) {
        case CFA_ADDITIONAL_ASSIGNMENT:
        case CFA_IMMEDIATE_ASSIGNMENT:
        case CFA_IMMEDIATE_ASSIGNMENT_EXTENDED:
        case CFA_IMMEDIATE_ASSIGNMENT_REJECT:
        case CFA_CIPHERING_MODE_COMMAND:
        case CFA_CIPHERING_MODE_COMPLETE:
        case CFA_ASSIGNMENT_COMMAND:
        case CFA_ASSIGNMENT_COMPLETE:
        case CFA_ASSIGNMENT_FAILURE:
        case CFA_HANDOVER_COMMAND:
        case CFA_HANDOVER_COMPLETE:
        case CFA_HANDOVER_FAILURE:
        //case CFA_PHYSICAL_INFORMATION:
        case CFA_CHANNEL_RELEASE:
        case CFA_PARTIAL_RELEASE:
        case CFA_PARTIAL_RELEASE_COMPLETE:
        //case CFA_PAGING_REQUEST_TYPE_1:
        //case CFA_PAGING_REQUEST_TYPE_2:
        //case CFA_PAGING_REQUEST_TYPE_3:
        case CFA_PAGING_RESPONSE:
        //case CFA_SYSTEM_INFORMATION_TYPE_8:
        //case CFA_SYSTEM_INFORMATION_TYPE_1:
        //case CFA_SYSTEM_INFORMATION_TYPE_2:
        //case CFA_SYSTEM_INFORMATION_TYPE_3:
        //case CFA_SYSTEM_INFORMATION_TYPE_4:
        //case CFA_SYSTEM_INFORMATION_TYPE_5:
        //case CFA_SYSTEM_INFORMATION_TYPE_6:
        //case CFA_SYSTEM_INFORMATION_TYPE_7:
        //case CFA_SYSTEM_INFORMATION_TYPE_2_BIS:
        //case CFA_SYSTEM_INFORMATION_TYPE_5_BIS:
        //case CFA_SYSTEM_INFORMATION_TYPE_2_TER:
        //case CFA_SYSTEM_INFORMATION_TYPE_5_TER:
        case CFA_CHANNEL_MODE_MODIFY:
        case CFA_RR_STATUS:
        case CFA_CHANNEL_MODE_MODIFY_ACKNOWLEDGE:
        case CFA_FREQUENCY_REDEFINITION:
        //case CFA_MEASUREMENT_REPORT:
        case CFA_CLASSMARK_CHANGE:
        case CFA_CLASSMARK_ENQUIRY:
            log_event = TRUE;
            break;
        default: 
            break;
         }
         break; 
    default:
        log_event = FALSE;
        break;
    }/* end switch(set) */
    if ( TRUE == log_event ) {
        if ( isUplink ) {
            event_report_payload(EVENT_GSM_MESSAGE_SENT, 3,(void *)&id);
        }
        else {
            event_report_payload(EVENT_GSM_MESSAGE_RECEIVED, 3,(void *)&id);
        }
    }
}

/*===========================================================================
FUNCTION:  cfa_log_timer_expired
===========================================================================*/
void cfa_log_timer_expired( byte queue, byte timer )
{
    cfa_generic_identity_type id;
    boolean log_event = FALSE;
    
    id.set     = queue;
    id.id      = timer;

    switch (id.set) {
    case GS_QUEUE_RR:
      switch ( id.id ) {
      case CFA_T3110:
      case CFA_T3126:
      case CFA_T3122:
      case CFA_T3124:
      case CFA_T3178:
      case CFA_T3158:
      case CFA_T3174:
      case CFA_T3176:
      case CFA_T3142:
      case CFA_T3146:    
        log_event = TRUE;
        break;
      default:
        break;
      }
      break;
    case GS_QUEUE_MM:
      switch ( id.id ) {
      case TIMER_T3210:
      case TIMER_T3211:
      case TIMER_T3212:
      case TIMER_T3213:
      case TIMER_T3220:
      case TIMER_T3230:
      case TIMER_T3240:
        log_event = TRUE;
        break;
      default:
        break;
      }
      break;
    case GS_QUEUE_REG:
    case GS_QUEUE_LAYER_2:
    default:
      break;
    }/* end switch(set) */
    if ( TRUE == log_event ) {
        event_report_payload(EVENT_GSM_TIMER_EXPIRED, 2,(void *)&id);
    }
}


/*===========================================================================
FUNCTION:  cfa_log_counter_expired
===========================================================================*/
void cfa_log_counter_expired( byte queue, byte counter )
{
}


#endif /* FEATURE_GSM */

