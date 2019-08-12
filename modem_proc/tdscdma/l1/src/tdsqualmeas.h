#ifndef L1QUALMEAS_H
#define L1QUALMEAS_H
/*==========================================================================

             L1  QUALITY MEASUREMENTS Code

DESCRIPTION
  This module handles quality measurements requests from RRC.

  Copyright (c) 2005 - 2010 by Qualcomm Technologies Incorporated.
  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdsqualmeas.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------------------------------------------
05/03/10   rc      Added function prototype for tdsqualmeas_add_transport_channel_measurement_index()
05/08/09   sup     Changing the includes for the renaming of header files  dec.h, enci.h, l1m.h, srch.h, srchi.h, 
                   srchlog.h to wdec.h, wenci.h, wl1m.h, wsrch.h, wsrchi.h, wsrchlog.h
09/12/05   gs/rc   Updated the l1qualmeas_update_quality_meas_info() function 
                   srchlog.h to tdsdec.h, tdsenci.h, tdsl1m.h, tdssrch.h, tdssrchi.h, tdssrchlog.h
09/12/05   gs/rc   Updated the tdsqualmeas_update_quality_meas_info() function 
                   declaration.
06/06/05   kps     Lint fix
04/14/05   rc      Modified the function declaration for 
                   tdsqualmeas_send_meas_report() function.
02/08/05   rc      Created the file. 

                           
                           
===========================================================================*/
                           


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "tdscdma_variation.h"
#ifdef FEATURE_TDSCDMA_QUALITY_MEAS

#include "tdsl1rrcif.h"
#include "tdsl1m.h"
#include "tdsl1def.h"
#include "tdsl1std.h"
#ifndef  FEATURE_TDSCDMA_PLT
#include "tdsrrccmd_v.h" 
#else
/*#include "wpltrrc.h"*/
#endif

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

//#define TDSL1_QUALMEAS_MAX_QUALITY_MEAS (0xFF)

#define TDSQUALMEAS_NO_TRANSPORT_CHANNEL_ID (0xFF)

#define TDSQUALMEAS_NO_QUALITY_MEAS_ID (0xFF)

#define TDSQUALMEAS_NO_BLER (0xFF)


#define TDSQUALMEAS_MAX_CRC_WINDOW_SIZE 512

extern uint16 tdsqualmeas_num_quality_meas_in_progress;

extern boolean tdsqualmeas_quality_periodic_rpts_requested;

/*************************************************************************

    D A T A   S T R U C T U R E S
   
************************************************************************/ 

typedef struct
{
uint8 no_ts;
uint8 sir[L1_MAX_TS];
}tdsqualmeas_sir_value_struct_type;


/* Transport channel structure */

typedef struct
{
  
  uint16 tr_ch_id;
 
  /* Measurement id this transport channel belongs to */
  uint16 meas_idx;
  
  uint8 bler;
  /* Keeps track how many CRC was received */
  uint32 no_of_crc_rcvd;
 /* Keeps track how many CRC with error were received */
  uint32 no_of_crc_w_err_rcvd;

  uint16 total_crc; /* Total CRC results reported (0..512) */
  uint16 bad_crc;

  /* The following are needed for event reporting */
  boolean total_crc_count_reached;

  /* Trigger pending flag */
  boolean pending_time_after_trigger;

  uint16 pending_after_tigger_crc_thresh;
  uint16 pending_after_trigger_crc_cnt;
  
 /* CRC result with CRC failed history */
  uint8 num_tb_w_crc_err[TDSQUALMEAS_MAX_CRC_WINDOW_SIZE];
  uint8 num_tb_w_crc[TDSQUALMEAS_MAX_CRC_WINDOW_SIZE];
  uint32 tb_crc_result[TDSQUALMEAS_MAX_CRC_WINDOW_SIZE];
  uint8 num_tb_in_window[TDSQUALMEAS_MAX_CRC_WINDOW_SIZE];
  uint16 crc_info_head;
  uint16 crc_info_tail; 

} tdsqualmeas_transport_channel_struct_type;

extern uint8 tdsl1_srch_bler_max_value;


 /* Quality Measurements parameters info */

typedef struct
{
  uint16 meas_id;

  tdsl1_meas_report_mode_enum_type rpt_mode;
  
  tdsl1_meas_report_trans_mode_enum_type rpt_trans_mode;
  
  boolean rpt_quan_included;

  boolean rpt_all_transport_ch;

  uint16 no_of_tr_ch;

  uint8 reported_transport_channels[TDSL1_MAX_TRANSPORT_CHANNELS];
  
  uint8 no_of_cctrch;
  uint8 tcfs_id[L1_MAX_CCTRCH];
  
  struct 
  {
    uint16 periodic_rpt_cnt;

    uint16 ts_cnt;

    uint16 num_reports_req;

    uint16 reporting_interval;

  } periodic_crit;

  tdsl1_qual_event_crit_struct_type event_crit;
  
} tdsqualmeas_param_info_struct_type;


/* Tranpsort channel lookup table */

typedef struct
{
  /* State of this transport channel */
  boolean trch_enable_state;
  
  /* can't allow more than 2 measurements per transport channel */
  uint8 trch_count;
} tdsqualmeas_trch_lookup_table_struct_type;


/* Parameters sent in the local command */
typedef struct
{
  tdsl1def_l1_local_cmd_hdr_type hdr;
    // generic command header 
  uint16 trch_id;
  uint16 crc_results;
  uint16 crc_errors;
  uint32 tb_crc_result;
} tdsqualmeas_crc_update_cmd_type;  



extern tdsqualmeas_param_info_struct_type tdsqualmeas_meas_db[TDSL1_QUALMEAS_MAX_QUALITY_MEAS];

         
/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================
FUNCTION     tdsqualmeas_msmt_ctrl_msg_quality_meas_processing

DESCRIPTION
  Handle Measurement Control Message Quality Measurement Related Processing
  
DEPENDENCIES
  None

RETURN VALUE
  FALSE if measurement control message processing identified errors in the message

SIDE EFFECTS
  None
===========================================================================*/
extern boolean tdsqualmeas_msmt_ctrl_msg_quality_meas_processing( tdsl1_meas_cmd_type *cmd_ptr );



/*===========================================================================
FUNCTION     tdsqualmeas_init

DESCRIPTION
  This function initializes the l1qualmeas subsystem

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdsqualmeas_init( void );



/*===========================================================================
FUNCTION     tdsqualmeas_find_index

DESCRIPTION
  Locate the index specified in the meas index database. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern uint16 tdsqualmeas_find_index(uint16 measid );
       
                           


/*===========================================================================
FUNCTION     tdsqualmeas_send_periodic_report

DESCRIPTION
  This function performs periodic event processing at the time the
  periodic timer expires.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdsqualmeas_send_periodic_report(uint16 meas_id );




/*===========================================================================
FUNCTION     tdsqualmeas_update_quality_meas_info

DESCRIPTION
  Receives the CRC info and sets a local command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdsqualmeas_update_quality_meas_info(uint8 trch_id, uint8 crc_results, uint8 crc_errors, uint32 tb_crc_result);



/*===========================================================================
FUNCTION     L1QUALMEAS_ACTIVE_TR_CH

DESCRIPTION
  Receive the list of active tranposrt channels from decoder.

DEPENDENCIES
  None

RETURN VALUE
  No of active channels

SIDE EFFECTS
  None
===========================================================================*/


extern void tdsqualmeas_update_active_tr_ch(uint8 no_active_tr_ch, uint8 tr_ch_id []); 


/*===========================================================================
FUNCTION     tdsqualmeas_update_quality_meas_info

DESCRIPTION
  Update the CRC status, sliding window and the flags.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdsqualmeas_process_crc_results(tdsqualmeas_crc_update_cmd_type* cmd_ptr);

                           
/*===========================================================================
FUNCTION     tdsqualmeas_send_meas_report

DESCRIPTION
  This function sends an event report when triggering criteria is met.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdsqualmeas_send_meas_report(uint16 meas_id, uint8 trch_id );



/*===========================================================================
FUNCTION     tdsqualmeas_transition_out_of_dch

DESCRIPTION
  This function cleans the databases and resets the flags when we exit DCH state.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdsqualmeas_transition_out_of_dch(void);

                           
/*======================================================================
FUNCTION L1QUALLMEAS_GET_MEAS_IDX_BASED_ON_ID

DESCRIPTION
  Returns the meas index into the measurements DB table based on
  the measurement id
    
DEPENDENCIES
  None

RETURN VALUE
  uint16

SIDE EFFECTS
  None

=======================================================================*/
uint16 tdsqualmeas_get_meas_idx_based_on_id(uint16 meas_id);


/*===========================================================================
FUNCTION     tdsqualmeas_remove_quality_measurement_index

DESCRIPTION
  Deletes the meas_idx entry from the quality measurement database.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The variable tdsqualmeas_num_quality_meas_in_progress may be changed.
===========================================================================*/
void tdsqualmeas_remove_quality_measurement_index( uint16 meas_idx );

/*===========================================================================
FUNCTION     tdsqualmeas_reset_meas_dbase

DESCRIPTION
  This function resets the measurement dbase.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsqualmeas_reset_meas_dbase(void);


/*===========================================================================
FUNCTION     tdsqualmeas_reset_trch_dbase

DESCRIPTION
  This function resets the transport channel database.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsqualmeas_reset_trch_dbase(void);

/*===========================================================================

FUNCTION tdsqualmeas_validate_quality_meas_ctrl_msg

DESCRIPTION
  This function validates the quality measurement control message from RRC

DEPENDENCIES
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  The quality measurement data structures may be altered.

===========================================================================*/          
boolean tdsqualmeas_validate_quality_meas_ctrl_msg(
                                       tdsl1_meas_ctrl_parm_struct_type* ctrl_ptr);

/*===========================================================================
FUNCTION     L1QUALMEAS_REMOVE_TRANSPORT_CHANNEL_INDEX

DESCRIPTION
  Deletes the trch_idx entry from the transport channel database.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The variable tdsqualmeas_num_transport_channel_in_trch_db may be changed.
===========================================================================*/
void tdsqualmeas_remove_transport_channel_measurement_index( uint16 trch_idx );

/*===========================================================================
FUNCTION     tdsqualmeas_remove_quality_measurements

DESCRIPTION
  This function will remove the measurement specified from the
  quality measurement list.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsqualmeas_remove_quality_measurements(uint16 meas_id );


/*===========================================================================
FUNCTION     L1QUALMEAS_ADD_TRANSPORT_CHANNEL_INDEX

DESCRIPTION
  Adds the trch_idx entry to the transport channel database if needed.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The variable tdsqualmeas_num_transport_channel_in_trch_db may be changed.
===========================================================================*/
void tdsqualmeas_add_transport_channel_measurement_index( uint16 trch_id );


#endif /* FEATURE_TDSCDMA_QUALITY_MEAS */

#endif /* L1QUALMEAS_H */
                           
