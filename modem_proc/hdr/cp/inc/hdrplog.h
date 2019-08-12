#ifndef HDRPLOG_H
#define HDRPLOG_H
/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               H D R   P R O T O C O L    L O G    F U N C T I O N S

GENERAL DESCRIPTION

   This file contains the databases, typedefs, enums and defines used in
   the HDR Protocol logging functions
   
EXTERNALIZED FUNCTIONS

  hdrplog_log_conn_term_stats
  hdrplog_conn_term_event
  hdrplog_conn_start_event
  hdrplog_inc_conn_term_stats_counter
  hdrplog_reset_conn_term_stats                
  hdrplog_log_conn_state_stats
  hdrplog_inc_conn_state_stats_counter
  hdrplog_reset_conn_state_stats

REGIONAL FUNCTIONS 

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2001 - 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

============================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrplog.h_v   1.7   29 Oct 2002 16:38:08   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/inc/hdrplog.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ------------------------------------------------------- 
01/03/12   sju     Added support to enable verbose debug f3 at runtime. 
05/31/11   cnx     Added hdrplog_get_subpkt_protocol_data().
11/09/10   pba     Added connection abort reason HDRPLOG_CONN_ATT_UNSPECIFIED.
11/09/10   pba     Fixed issue of DO CP not reporting call failure to CM during
                   BSR attempt.
02/24/09   wsh     CMI: updated included headers due to header split
10/10/06   hal     Added log packet support for RouteUpdateTriggerCode list
04/04/05   pba     Removed Lint errors
11/29/04   grl     Added support for connected state stat logging.
11/09/04   dna     Add new reasons for ConnectionRequest to be aborted.
11/03/04   mpa     Removed dependency on std.h (merged with sys.h)
10/28/02   mpa     Added hdrplog_log_rcv_ati_list()
                   Log full uati in uati_rcvd and powerup_update_session_data
12/13/01   mpa     Added hdrplog_powerup_update_session_data() for hdramp 
                   process powerup init.  Added prototype for hdrplog_log_
                   session_attempt and hdrplog_log_dm_sector_info
10/29/01   mpa     Added hdrplog_log_dm_page_msg()
10/04/01   vas     Added hdrplog_log_tca_info()
08/23/01   sh      added new params for hdrplog_uati_rcvd
07/26/01   sh      Created module
==========================================================================*/


/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "hdr_variation.h"
#include "sys.h"

#ifdef FEATURE_CMI
#include "sys_v.h"
#endif

#include "dsm.h"
#include "hdraddr.h"
#include "hdrhai.h"
#include "hdrlogi.h"
#include "hdrlog.h"



/*==========================================================================


                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

==========================================================================*/




typedef enum
{
  HDRPLOG_CONN_ATT_CD_GENERAL    =0,
  HDRPLOG_CONN_ATT_CD_NETBUSY    =1,
  HDRPLOG_CONN_ATT_CD_AUTH       =2,
  HDRPLOG_CONN_ATT_MAX_PROBES    =3,
  HDRPLOG_CONN_ATT_SYS_LOST      =4,
  HDRPLOG_CONN_ATT_NOT_PREFERRED =5,
  HDRPLOG_CONN_ATT_REDIRECT      =6,
  HDRPLOG_CONN_ATT_SETUP_TIMEOUT =7,
  HDRPLOG_CONN_ATT_POWERDOWN     =8,
  HDRPLOG_CONN_ATT_OFFLINE       =9,
  HDRPLOG_CONN_ATT_NAM_CHANGE    =0x0a,
  HDRPLOG_CONN_ATT_USER_ABORT    =0x0b,  
  HDRPLOG_CONN_ATT_ACCESS_HO     =0x0c,  
  HDRPLOG_CONN_ATT_ACQ_OTHERS    =0x0d,  
  HDRPLOG_CONN_ATT_UNSPECIFIED   =0x0e,  
  HDRPLOG_CONN_ATT_SUCCESS       =0x0f 
  /* additional failure reasons can not be added. */
} hdrplog_conn_attempt_result_enum_type;


typedef enum
{
  HDRPLOG_CONN_REL_AN            =0,
  HDRPLOG_CONN_REL_AT            =1,
  HDRPLOG_CONN_REL_SYS_LOST      =2,
  HDRPLOG_CONN_REL_NOT_PREFERRED =3,
  HDRPLOG_CONN_REL_REDIRECT      =4,
  HDRPLOG_CONN_REL_POWERDOWN     =5,
  HDRPLOG_CONN_REL_OFFLINE       =6,
  HDRPLOG_CONN_REL_NAM_CHANGE    =7,
  HDRPLOG_CONN_REL_CDMA_PAGE     =8,
  HDRPLOG_CONN_REL_UNSPECIFIED   =9
} hdrplog_conn_rel_enum_type;
                                                      
/* Which statistics that are being tracked to clear for logging purposes. */
typedef enum 
{
  HDRPLOG_ALL_STATS,
    /* Clear all of the statistics tracked in this log packet. */

  HDRPLOG_LAST_CALL_STATS,
    /* Clear only the statistics tracked related to the last call. */
                                                      
} hdrplog_reset_stats_type;
                                                         
#define HDRPLOG_HDRCP_VERBOSE_DEBUG "/nv/item_files/modem/hdr/cp/plog/verbosedebug"

/* Bit mask for enabling verbose debug f3 */
#define HDRPLOG_VERBOSE_DEBUG_SLP 0x01
#define HDRPLOG_VERBOSE_DEBUG_PCP 0x02
#define HDRPLOG_VERBOSE_DEBUG_SAP 0x04
                                                      
/*==========================================================================

                    FUNCTION PROTOTYPES USED IN THE MODULE

This section contains function prototypes for the functions used in this
module.

==========================================================================*/
 
/*===========================================================================

FUNCTION HDRPLOG_START_CONN_ATTEMPT

DESCRIPTION
  This function report the start of a connection attempt
    
DEPENDENCIES
  None
    
PARAMETERS
  trans_id - transaction id of the connection request message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_start_conn_attempt 
( 
  uint8 trans_id 
);

/*===========================================================================

FUNCTION HDRPLOG_LOG_TCA_INFO

DESCRIPTION
  This function logs information when a Traffic Channel Assignment message
  is received.
    
DEPENDENCIES
  None
    
PARAMETERS
  sho_cell_cnt - Number of pilots received in TCA message
  drc_len - DRC Length
  drc_gain - DRC Channel Gain
  ack_gain - ACK Channel Gain
  frame_offset - Frame Offset

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_log_tca_info 
( 
  uint8   sho_cell_cnt,
  uint8   drc_len,
  int8    drc_gain,
  int8    ack_gain,
  uint8   frame_offset
);

/*===========================================================================

FUNCTION HDRPLOG_TCA_RCVD

DESCRIPTION
  This function report the reception of a traffic channel assignment
  message
    
DEPENDENCIES
  None
    
PARAMETERS
  msg_seq - message sequence number of the TCA message
  item_ptr - pointer of the TCA message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_tca_rcvd 
( 
  uint8         msg_seq, 
  dsm_item_type *item_ptr
);

/*===========================================================================

FUNCTION HDRPLOG_CONN_DENY_RCVD

DESCRIPTION
  This function report the reception of a connection deny message  
  
    
DEPENDENCIES
  None
    
PARAMETERS
  result   - connection deny reason
  item_ptr - pointer of the Conenction Deny message


RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_conn_deny_rcvd 
( 
  hdrplog_conn_attempt_result_enum_type  reason,
  dsm_item_type *item_ptr
);

/*===========================================================================

FUNCTION HDRPLOG_LOG_HO
DESCRIPTION
  This function report the handoff during the connection attempt 
  
    
DEPENDENCIES
  None
    
PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_log_ho ( void );

/*===========================================================================

FUNCTION HDRPLOG_END_CONN_ATTEMPT

DESCRIPTION
  This function reports the end of a connection attempt
    
                        
DEPENDENCIES
  None
    
PARAMETERS
  result - result of the connection attempt

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_end_conn_attempt 
( 
  hdrplog_conn_attempt_result_enum_type result 
);

/*===========================================================================

FUNCTION HDRPLOG_LOG_CONN_REL

DESCRIPTION
  This function logs the connection release record  
    
DEPENDENCIES
  None
    
PARAMETERS
  rel_reason - connection release reason

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_log_conn_rel
(
  hdrplog_conn_rel_enum_type rel_reason  
);

/*===========================================================================

FUNCTION HDRPLOG_LOG_DM_PAGE_MSG

DESCRIPTION
  This function logs the received page message to the DM.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_log_dm_page_msg ( void );

/*===========================================================================

FUNCTION HDRPLOG_LOG_PAGE_MSG

DESCRIPTION
  This function logs the received page message  
    
DEPENDENCIES
  None
    
PARAMETERS
   item_ptr - pointer to the dsm item

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_log_page_msg
(
  dsm_item_type *item_ptr 
);

/*===========================================================================

FUNCTION HDRPLOG_LOG_SESSION_ATTEMPT

DESCRIPTION
  This function logs session attempts info
    
DEPENDENCIES
  None
    
PARAMETERS
  None   
 
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_log_session_attempt ( void );

/*===========================================================================

FUNCTION HDRPLOG_POWERUP_UPDATE_SESSION_DATA

DESCRIPTION
  This function updates and reports the session attempt log data upon 
  power up.  The data is retrieved from NV memory upon start up.  
  This is logged only if a session is already open.
    
DEPENDENCIES
  None
    
PARAMETERS
  rati       - RATI used for opening that session (restored from NV)
  full_uati  - Full uati restored from NV
  color_code - color code from the uati (restored from NV)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_powerup_update_session_data
( 
  hdraddr_type full_uati,
  uint32  rati,
  uint8   color_code
);

/*===========================================================================

FUNCTION HDRPLOG_LOG_SECTOR_INFO

DESCRIPTION
  This function updates and logs the sector information
    
DEPENDENCIES
  hdrplog_log_dm_sector_info
    
PARAMETERS
  sector_id    - 128 bit sector id 
  subnet_mask  - sector subnet mask
  color_code   - color code of the sector
  pn_offset    - sector pn offset
    
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_log_sector_info
(
  hdraddr_type  sector_id,      
  uint8         subnet_mask,    
  uint8         color_code,
  uint16        pn_offset
);

/*===========================================================================

FUNCTION HDRPLOG_LOG_DM_SECTOR_INFO

DESCRIPTION
  This function logs the sector information to the DM
    
DEPENDENCIES
  None
    
PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_log_dm_sector_info ( void );

/*===========================================================================

FUNCTION HDRPLOG_GET_SECTOR_ID_N_COLOR_CODE

DESCRIPTION
  This function get teh sector id and color code
  from the OVHD

DEPENDENCIES
  None
    
PARAMETERS
  protocol name - calling protocol name
  sector_id_ptr - pointer to sector id
  color_code_ptr - pointer to color code 

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_get_sector_id_n_color_code 
( 
  hdrhai_protocol_name_enum_type protocl_name,
  uint32 * sector_id_ptr,
  uint8  * color_code_ptr
);

/*===========================================================================

FUNCTION HDRPLOG_GET_PN
DESCRIPTION
  This function get the PN offset from the dsm item 
    
DEPENDENCIES
  None
    
PARAMETERS
   item_ptr - pointer to the dsm item

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uint16 hdrplog_get_pn
(
  dsm_item_type *item_ptr 
);

/*===========================================================================

FUNCTION HDRPLOG_UATI_RCVD

DESCRIPTION
  This function report the reception of a UATI assignment
  message
    
DEPENDENCIES
  None
    
PARAMETERS
  item_ptr - pointer to the dsm item
  full_uati - full uati just assigned
  color_code - color code 

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_uati_rcvd 
( 
  dsm_item_type *item_ptr,
  hdraddr_type  full_uati,
  uint8         color_code
 );
/*===========================================================================

FUNCTION HDRPLOG_END_SESSION_ATTEMPT

DESCRIPTION
  This function report the end of a session attempt
    
DEPENDENCIES
  None
    
PARAMETERS
  result - TRUE session attempt is successful

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_end_session_attempt 
( 
  boolean result 
);
/*===========================================================================

FUNCTION HDRPLOG_UPDATE_LOG_STATE_INFO

DESCRIPTION
  This function updates the state information for logging
    
DEPENDENCIES
  None
    
PARAMETERS
 

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_update_log_state_info( LOG_HDR_STATE_INFO_C_type *log_ptr ) ;

/*===========================================================================

FUNCTION HDRPLOG_LOG_STATE_INFO

DESCRIPTION
  This function logs the state information  
    
DEPENDENCIES
  None
    
PARAMETERS
 

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_log_state_info( void ) ;


/*===========================================================================

FUNCTION HDRPLOG_TRANS_ABORT_REASON_FOR_REL

DESCRIPTION
   This function convert HAI's abort reason to appropriate connection
   release reason
  

DEPENDENCIES
  None

PARAMETERS
  abort_reason - Conenction abort reason 
  
RETURN VALUE
  reason for connection release

SIDE EFFECTS
  None

===========================================================================*/

hdrplog_conn_rel_enum_type hdrplog_trans_abort_reason_for_rel
( 
  hdrhai_abort_reason_enum_type abort_reason 
);

/*===========================================================================

FUNCTION HDRPLOG_POWERUP_INIT

DESCRIPTION
  This function reset all protocol related counters and statistics   

DEPENDENCIES
  None
    
PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_powerup_init ( void );


/*===========================================================================

FUNCTION HDRPLOG_ACCESS_MODE

DESCRIPTION
  This function set the access mode status  

DEPENDENCIES
  None
    
PARAMETERS
  access_mode_status - TRUE in access mode
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_access_mode
( 
  boolean access_mode_status 
);
/*===========================================================================

FUNCTION HDRPLOG_SET_NORAML_CONN_REL_REASON

DESCRIPTION
  This function set the normal connection release reason 
  to either AT or AN release 
    
DEPENDENCIES
  None
    
PARAMETERS
  rel_reason - normal connection release reason. this shouldbe
               either AT or AN release

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_set_normal_conn_rel_reason
(
  hdrplog_conn_rel_enum_type rel_reason  
);
/*===========================================================================

FUNCTION HDRPLOG_START_SESSION_ATTEMPT

DESCRIPTION
  This function report the start of a session attempt
    
DEPENDENCIES
  None
    
PARAMETERS
  trans_id - transaction id of the UATI request message
  rati     - RATI used for opening a session

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_start_session_attempt 
( uint8 trans_id, 
  uint32 rati 
);


/* <EJECT> */
/*===========================================================================

FUNCTION HDRPLOG_RESET_CONN_ATTEMPT

DESCRIPTION
  This function reset connection attempt statistics   

DEPENDENCIES
  None
    
PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_reset_conn_attempt ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION HDRPLOG_RESET_PAGE_COUNT

DESCRIPTION
  This function reset page count  

DEPENDENCIES
  None
    
PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_reset_page_count ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION HDRPLOG_RESET_SESSION_ATTEMPT

DESCRIPTION
  This function reset session attempt statistics   

DEPENDENCIES
  None
    
PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_reset_session_attempt ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION HDRPLOG_LOG_RCV_ATI_LIST

DESCRIPTION
  This function logs the list of currently valid UATIs

DEPENDENCIES
  None
    
PARAMETERS
  rcv_list_ptr - pointer to the list of valid UATIs
  ati_cnt - number of valid UATIs in the list

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrplog_log_rcv_ati_list(uint32* rcv_list_ptr, uint8 ati_cnt);

/* <EJECT> */
/*=========================================================================

FUNCTION HDRPLOG_CONN_TERM_STATS

DESCRIPTION  
  Logs the connection termination info. It gets a log record
  from the diag log buffer, fills it with the latest 
  connection termination info and commits the log record to
  the diag for delivery.

DEPENDENCIES 
  None

PARAMETERS
  None

RETURN VALUE 
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrplog_log_conn_term_stats( void );

/* <EJECT> */
/*=========================================================================

FUNCTION HDRPLOG_CONN_TERM_EVENT

DESCRIPTION  
  This function records the timestamp of a call termination for call
  statistics tracking purposes.
  
DEPENDENCIES 
  None

PARAMETERS
  rel_reason - The Layer 3 reason for the call termination.

RETURN VALUE 
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrplog_conn_term_event
(
  hdrplog_conn_rel_enum_type rel_reason  
);

/* <EJECT> */
/*=========================================================================

FUNCTION HDRPLOG_CONN_START_EVENT

DESCRIPTION 
  This function notifies the logging module of the start of a new HDR 
  connection for logging statistic tracking purposes. 

DEPENDENCIES 
  None

PARAMETERS
  None

RETURN VALUE 
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrplog_conn_start_event( void );

/* <EJECT> */
/*=========================================================================

FUNCTION HDRPLOG_INC_CONN_TERM_STATS_COUNTER

DESCRIPTION 
  This function increments the appropriate connection termination counter
  by the given amount.

DEPENDENCIES 
  None

PARAMETERS
  counter - The counter to increment.
  inc_count - How much to increment the counter by.

RETURN VALUE 
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrplog_inc_conn_term_stats_counter
( 
  hdrplog_conn_term_stats_count_type counter,
  uint16 inc_count
);

/* <EJECT> */
/*=========================================================================

FUNCTION HDRPLOG_RESET_CONN_TERM_STATS

DESCRIPTION  
  This function resets the connection termination stats as
  specified by the stats reset parameter.

DEPENDENCIES 
  None

PARAMETERS
  stats_reset - Which statistics we should clear (all or just the last 
                call stats).

RETURN VALUE 
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrplog_reset_conn_term_stats
( 
  hdrplog_reset_stats_type stats_reset
);

/* <EJECT> */
/*=========================================================================

FUNCTION HDRPLOG_LOG_CONN_STATE_STATS

DESCRIPTION  
  Logs the connected state stats. It gets a log record
  from the diag log buffer, fills it with the latest 
  connected state info and commits the log record to
  the diag for delivery.

DEPENDENCIES 
  None

PARAMETERS
  None

RETURN VALUE 
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrplog_log_conn_state_stats( void );

/* <EJECT> */
/*=========================================================================

FUNCTION HDRPLOG_INC_CONN_STATE_STATS_COUNTER

DESCRIPTION  
  This function increments the requested connected state 
  counter by the given amount.

DEPENDENCIES 
  None

PARAMETERS
  counter - The counter to increment.
  inc_count - How much to increment the counter by.

RETURN VALUE 
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrplog_inc_conn_state_stats_counter
( 
  hdrplog_conn_state_stats_count_type counter,
  uint16                              inc_count
);

/* <EJECT> */
/*=========================================================================

FUNCTION HDRPLOG_RESET_CONN_STATE_STATS

DESCRIPTION 
  This function resets the connected state stats as
  specified by the stats reset parameter.

DEPENDENCIES 
  None

PARAMETERS
  stats_reset - Which statistics we should clear (all or just the last 
                call stats).

RETURN VALUE 
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrplog_reset_conn_state_stats
( 
  hdrplog_reset_stats_type stats_reset
);

/*===========================================================================

FUNCTION HDRPLOG_RUTC_CLEAR_LIST

DESCRIPTION
  This function clears the RUTC log packet data and sets the list size.

DEPENDENCIES
  None.

PARAMETERS
  route_update_trigger_code_list_size - The negotiated size of the RUTC list

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrplog_rutc_clear_list
(
  int route_update_trigger_code_list_size
);

/*===========================================================================

FUNCTION HDRPLOG_RUTC_ADD_ENTRY

DESCRIPTION
  This function adds an RUTC entry to the RUTC list log packet.

DEPENDENCIES
  None.

PARAMETERS
  subnet_id - The subnet ID of the RUTC list entry
  subnet_mask - The subnet mask of the RUTC list entry
  route_update_trigger_code - The trigger code of the RUTC list entry
  timestamp - The timestamp of the RUTC list entry
  rup_was_acked - The ACK status of the RUTC list entry

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrplog_rutc_add_entry
(
  hdraddr_type subnet_id,
  uint8 subnet_mask,
  uint16 route_update_trigger_code,
  uint32 timestamp,
  boolean rup_was_acked
);

/*===========================================================================

FUNCTION HDRPLOG_RUTC_COMMIT

DESCRIPTION
  This function commits the RUTC list log packet.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrplog_rutc_commit( void );

/*=========================================================================

FUNCTION HDRPLOG_GET_SUBPKT_PROTOCOL_DATA

DESCRIPTION
  This function gets value for the fields, which are related to HDRCP, in
  subpacket Protocol Data. 

DEPENDENCIES
  None

PARAMETERS
  subpkt_ptr - pointer to the subpacket to be set

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrplog_get_subpkt_protocol_data
(
  hdrdiag_hdr_protocol_data_subpkt_protocol_data_type *subpkt_ptr
);

/* <EJECT> */
#endif /* HDRPLOG_H */
