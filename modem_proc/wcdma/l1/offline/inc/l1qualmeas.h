#ifndef L1QUALMEAS_H
#define L1QUALMEAS_H
/*==========================================================================

             L1  QUALITY MEASUREMENTS Code

DESCRIPTION
  This module handles quality measurements requests from RRC.

  Copyright (c) 2005 - 2010 by Qualcomm Technologies, Inc.
  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/l1qualmeas.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------------------------------------------
01/18/13   vn      Declared additional measurement variables to enhance quality 
                   measurement reports.
01/26/12   vs      Nikel feature cleanup.mainline the feature FEATURE_WCDMA_QUALITY_MEAS.
05/03/10   rc      Added function prototype for l1qualmeas_add_transport_channel_measurement_index()
05/08/09   sup     Changing the includes for the renaming of header files  dec.h, enci.h, l1m.h, srch.h, srchi.h, 
                   srchlog.h to wdec.h, wenci.h, wl1m_v.h, wsrch.h, wsrchi.h, wsrchlog.h
09/12/05   gs/rc   Updated the l1qualmeas_update_quality_meas_info() function 
                   declaration.
06/06/05   kps     Lint fix
04/14/05   rc      Modified the function declaration for 
                   l1qualmeas_send_meas_report() function.
02/08/05   rc      Created the file. 

                           
                           
===========================================================================*/
                           


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/


#include "wcdma_variation.h"
#include "l1rrcif.h"
#include "wl1m.h"
#include "l1def.h"
#ifndef FEATURE_WCDMA_PLT
#include "rrccmd_v.h" 
#else
#error code not present
#endif

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* UE need not support more than 16 reporting criteria in total. (25.133) */
#define L1_MAX_QUALITY_MEAS 16

#define L1_NO_QUALITY_MEAS_INDEX (0xFF)

#define L1_NO_TRANSPORT_CHANNEL_ID (0xFF)

#define L1_NO_QUALITY_MEAS_ID (0xFF)

#define L1QUALMEAS_NO_BLER (0xFF)


/* We can get upto 32 transport channel in an MCM and can support two measurements
   per transport channel, so need to support 64 channels at any given time. But since
   currently we support 8 transport channels in DCH state, this is defined as 16 */

/* Maximum number of tranposrt channels in the transport channels database */
#define L1_MAX_TRANSPORT_CHANNEL_INDEX 16

/* UE should not support more than 2 measurements per transport channel. (25.133) */
#define L1_MAX_MEAS_PER_TRANSPORT_CHANNEL 2

#define L1_MAX_CRC_WINDOW_SIZE 512

extern uint16 l1_num_quality_meas_in_progress;

extern boolean l1_quality_periodic_rpts_requested;

/*************************************************************************

    D A T A   S T R U C T U R E S
   
************************************************************************/ 

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
  uint8 num_tb_w_crc_err[L1_MAX_CRC_WINDOW_SIZE];
  uint8 num_tb_w_crc[L1_MAX_CRC_WINDOW_SIZE];
  uint32 tb_crc_result[L1_MAX_CRC_WINDOW_SIZE];
  uint8 num_tb_in_window[L1_MAX_CRC_WINDOW_SIZE];
  uint16 crc_info_head;
  uint16 crc_info_tail; 

} l1_qual_meas_transport_channel_struct_type;


 /* Quality Measurements parameters info */

typedef struct
{
  uint16 meas_id;

  l1_meas_report_mode_enum_type rpt_mode;
  
  l1_meas_report_trans_mode_enum_type rpt_trans_mode;
  
  boolean rpt_quan_included;

  boolean rpt_all_transport_ch;

  uint16 no_of_tr_ch;

  uint8 reported_transport_channels[L1_MAX_TRANSPORT_CHANNELS];

  struct 
  {
    uint16 periodic_rpt_cnt;

    uint16 ts_cnt;

    uint16 num_reports_req;

    uint16 reporting_interval;

  } periodic_crit;

  l1_qual_event_crit_struct_type event_crit;
  
  uint16  num_addl_meas;
    /* Number of additional measurements */
  uint16  addl_meas_id[ L1_MAX_CELL_MEAS ];
    /* 10.3.7.48 Measurement Identity for each addl measurement */
  l1_meas_type_enum_type addl_meas_type[L1_MAX_CELL_MEAS];

  
} l1_qual_meas_param_info_struct_type;


/* Tranpsort channel lookup table */

typedef struct
{
  /* State of this transport channel */
  boolean trch_enable_state;
  
  /* can't allow more than 2 measurements per transport channel */
  uint8 trch_count;
} l1_qual_meas_trch_lookup_table_struct_type;


/* Parameters sent in the local command */
typedef struct
{
  l1_local_cmd_hdr_type hdr;
    // generic command header 
  uint16 trch_id;
  uint16 crc_results;
  uint16 crc_errors;
  uint32 tb_crc_result;
} l1qualmeas_crc_update_cmd_type;  



extern l1_qual_meas_param_info_struct_type l1_quality_meas_db[L1_MAX_QUALITY_MEAS];

         
/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================
FUNCTION     L1QUALMEAS_MSMT_CTRL_MSG_QUALITY_MEAS_PROCESSING

DESCRIPTION
  Handle Measurement Control Message Quality Measurement Related Processing
  
DEPENDENCIES
  None

RETURN VALUE
  FALSE if measurement control message processing identified errors in the message

SIDE EFFECTS
  None
===========================================================================*/
extern boolean l1qualmeas_msmt_ctrl_msg_quality_meas_processing( l1_meas_cmd_type *cmd_ptr );



/*===========================================================================
FUNCTION     L1QUALMEAS_INIT

DESCRIPTION
  This function initializes the l1qualmeas subsystem

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1qualmeas_init( void );



/*===========================================================================
FUNCTION     L1QUALMEAS_FIND_INDEX

DESCRIPTION
  Locate the index specified in the meas index database. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern uint16 l1qualmeas_find_index(uint16 measid );
       
                           


/*===========================================================================
FUNCTION     L1QUALMEAS_SEND_PERIODIC_REPORT

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

extern void l1qualmeas_send_periodic_report(uint16 meas_id );




/*===========================================================================
FUNCTION     L1QUALMEAS_UPDATE_QUALITY_MEAS_INFO

DESCRIPTION
  Receives the CRC info and sets a local command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void l1qualmeas_update_quality_meas_info(uint8 trch_id, uint8 crc_results, uint8 crc_errors, uint32 tb_crc_result);



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


extern void l1qualmeas_update_active_tr_ch(uint8 no_active_tr_ch, uint8 tr_ch_id []); 


/*===========================================================================
FUNCTION     L1QUALMEAS_UPDATE_QUALITY_MEAS_INFO

DESCRIPTION
  Update the CRC status, sliding window and the flags.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void l1qualmeas_process_crc_results(l1qualmeas_crc_update_cmd_type* cmd_ptr);

                           
/*===========================================================================
FUNCTION     L1QUALMEAS_SEND_MEAS_REPORT

DESCRIPTION
  This function sends an event report when triggering criteria is met.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void l1qualmeas_send_meas_report(uint16 meas_id, uint8 trch_id );



/*===========================================================================
FUNCTION     L1QUALMEAS_TRANSITION_OUT_OF_DCH

DESCRIPTION
  This function cleans the databases and resets the flags when we exit DCH state.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void l1qualmeas_transition_out_of_dch(void);

                           
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
uint16 l1qualmeas_get_meas_idx_based_on_id(uint16 meas_id);


/*===========================================================================
FUNCTION     L1QUALMEAS_REMOVE_QUALITY_MEASUREMENT_INDEX

DESCRIPTION
  Deletes the meas_idx entry from the quality measurement database.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The variable l1_num_quality_meas_in_progress may be changed.
===========================================================================*/
void l1qualmeas_remove_quality_measurement_index( uint16 meas_idx );

/*===========================================================================
FUNCTION     L1QUALMEAS_RESET_MEAS_DBASE

DESCRIPTION
  This function resets the measurement dbase.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void l1qualmeas_reset_meas_dbase(void);


/*===========================================================================
FUNCTION     L1QUALMEAS_RESET_TRCH_DBASE

DESCRIPTION
  This function resets the transport channel database.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void l1qualmeas_reset_trch_dbase(void);

/*===========================================================================

FUNCTION L1QUALMEAS_VALIDATE_QUALITY_MEAS_CTRL_MSG

DESCRIPTION
  This function validates the quality measurement control message from RRC

DEPENDENCIES
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  The quality measurement data structures may be altered.

===========================================================================*/          
boolean l1qualmeas_validate_quality_meas_ctrl_msg(
                                       l1_meas_ctrl_parm_struct_type* ctrl_ptr);

/*===========================================================================
FUNCTION     L1QUALMEAS_REMOVE_TRANSPORT_CHANNEL_INDEX

DESCRIPTION
  Deletes the trch_idx entry from the transport channel database.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The variable l1_num_transport_channel_in_trch_db may be changed.
===========================================================================*/
void l1qualmeas_remove_transport_channel_measurement_index( uint16 trch_idx );

/*===========================================================================
FUNCTION     L1QUALMEAS_REMOVE_QUALITY_MEASUREMENTS

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
void l1qualmeas_remove_quality_measurements(uint16 meas_id );


/*===========================================================================
FUNCTION     L1QUALMEAS_ADD_TRANSPORT_CHANNEL_INDEX

DESCRIPTION
  Adds the trch_idx entry to the transport channel database if needed.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The variable l1_num_transport_channel_in_trch_db may be changed.
===========================================================================*/
void l1qualmeas_add_transport_channel_measurement_index( uint16 trch_id );



#endif /* L1QUALMEAS_H */
                           
