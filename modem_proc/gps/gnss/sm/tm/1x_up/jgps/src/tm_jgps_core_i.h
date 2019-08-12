#ifndef TM_JGPS_CORE_I_H
#define TM_JGPS_CORE_I_H

#include "gps_variation.h"
#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                  TM JGPS Internal Header File

DESCRIPTION
  This header file contains definitions of data structure necessary for Japan 
  GPS specification for KDDI.

  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/1x_up/jgps/src/tm_jgps_core_i.h#1 $  
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/06/06   rw      Initial Release
===========================================================================*/

/*=============================================================================

                                Include Files

=============================================================================*/
#ifdef FEATURE_JCDMA_GPSONE
#include "tm_common.h"
#include "pdapi.h"

#include "pd_comms_api.h"
#include "tm_jgps_core.h"
  
/*=============================================================================
===============================================================================

                                Internal Functions
                                
===============================================================================
=============================================================================*/


/*===========================================================================
FUNCTION: tm_jgps_core_sess_init

DESCRIPTION:

 
PARAMETERS:

===========================================================================*/
void tm_jgps_core_sess_init( void );

  
/*===========================================================================
FUNCTION: tm_jgps_core_app_info_init

DESCRIPTION:

 
PARAMETERS:

===========================================================================*/
void tm_jgps_core_app_info_init( void );


/*===========================================================================
FUNCTION: tm_jgps_core_comm_info_init

DESCRIPTION:

 
PARAMETERS:

===========================================================================*/
void tm_jgps_core_comm_info_init( void );


/*===========================================================================
FUNCTION: tm_jgps_core_read_nv

DESCRIPTION:

PARAMETERS:

===========================================================================*/
void tm_jgps_core_read_nv( void );


/*===========================================================================

FUNCTION   tm_jgps_core_update_sess_state

DESCRIPTION
  This function is used to save JGPSONE specific info for use.

RETURN VALUE
  TRUE - JGPS core grants the session start request
  FALSE - otherwise
============================================================================*/
boolean tm_jgps_core_update_sess_state
(
  tm_jgps_sess_state_e_type new_state
);

/*===========================================================================

FUNCTION   tm_jgps_core_restart_session

DESCRIPTION
  This function will kick-start a new session and wake-up TM-Core. Previous, 
  TM-Core request was placed in wait state because JGPS was in the process
  of closing the TCP connection.
  
RETURN VALUE
  None

============================================================================*/
boolean tm_jgps_core_restart_session( void );


/*===========================================================================

FUNCTION   tm_jgps_core_abort_recover

DESCRIPTION
  This function is used to abort the current sesssion and notify the 
  proper sub-modules (i.e. IS-801 driver), TM-Core, and client

RETURN VALUE
  None

============================================================================*/
void tm_jgps_core_abort_recover
(
  tm_sess_stop_reason_e_type  abort_reason,
  prtl_sess_stop_type         stop_type
);


/*===========================================================================

FUNCTION tm_jgps_core_abort_scu_resp

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
tm_sess_stop_reason_e_type tm_jgps_core_abort_scu_resp
(
  byte  cause
);


/*===========================================================================

FUNCTION tm_jgps_core_abort_stop_scu

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
tm_sess_stop_reason_e_type tm_jgps_core_abort_stop_scu
(
  byte  cause
);


/*===========================================================================

FUNCTION tm_jgps_core_abort_pos_comp

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

tm_sess_stop_reason_e_type tm_jgps_core_abort_pos_comp
(
  byte   cause
);


/*===========================================================================

FUNCTION tm_jgps_core_abort_trans_scu_error

DESCRIPTION
   This function calls functions to translate the error codes to JGPS
   errors, and eventually calls tm_jgps_core_abort_recover() to 
   send the error back to JGPS clients and abort the current IS-801 session

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_jgps_core_abort_trans_scu_error
(
  byte   cause
);


/*===========================================================================

FUNCTION tm_jgps_core_t02_timer_handler

DESCRIPTION
  This function handles the T02 timer expiration
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
void tm_jgps_core_t02_timer_handler( void );


/*===========================================================================

FUNCTION tm_jgps_cpre_t04_timer_handler

DESCRIPTION
  This function handles the T04 timer expiration
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
void tm_jgps_core_t04_timer_handler( void );


/*===========================================================================

FUNCTION tm_jgps_core_info_req_timer_handler

DESCRIPTION
  This function handles the RF info request  or MS IP address 
  info request timer expiration
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
void tm_jgps_core_info_req_timer_handler( void );


/*===========================================================================

FUNCTION tm_jgps_core_req_info_dispatch

DESCRIPTION
  This function is used to handle IOCTL ACK from PD COMM.

RETURN VALUE
  TRUE: No error
  FALSE: Error

DEPENDENCIES

===========================================================================*/
void tm_jgps_core_req_info_dispatch( void );


/*===========================================================================

FUNCTION tm_jgps_core_req_ms_ip_info

DESCRIPTION
  This function requests MS IP address for KDDI POS req or ACQ req 
  packets.  Ask TM-Core to query LM which asks MGP to return a RF info
  data structure. 
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
boolean tm_jgps_core_req_ms_ip_info( void );


/*===========================================================================

FUNCTION tm_jgps_core_trans_ms_ip_addr

DESCRIPTION
  This function is used to handle IOCTL ACK from PD COMM.

RETURN VALUE
  TRUE: No error
  FALSE: Error

DEPENDENCIES

===========================================================================*/
boolean tm_jgps_core_trans_ms_ip_addr
(
  byte     *ms_ip_ptr,
  uint32   ms_ip_len
);


/*===========================================================================

FUNCTION tm_jgps_core_req_rf_info

DESCRIPTION
  This function requests RF info specific for KDDI POS req or ACQ req 
  packets.  Ask TM-Core to query LM which asks MGP to return a RF info
  data structure. 
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
boolean tm_jgps_core_req_rf_info( void );


/*===========================================================================

FUNCTION   tm_jgps_core_trans_rf_info

DESCRIPTION
  This function handles the session information data returned by TM-Core

RETURN VALUE
  None

============================================================================*/
boolean tm_jgps_core_trans_rf_info
(
  sm_ReportRfInfoStructType   *rf_info
);

      
/*===========================================================================

FUNCTION tm_jgps_core_calc_timestamp

DESCRIPTION
  This function calculates Time in JST format.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
void tm_jgps_core_calc_timestamp( void );


/*===========================================================================

FUNCTION tm_jgps_core_calc_latitude

DESCRIPTION
  This function calculates Lattitude in WGS-84 format, [degree].
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
double tm_jgps_core_calc_latitude
(
  sint31 latitude
);

/*===========================================================================

FUNCTION tm_jgps_core_calc_longitude

DESCRIPTION
  This function calculates Longitude in WGS-84 format, [degree].
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
double tm_jgps_core_calc_longitude 
(
  sint31 longitude
);


/*===========================================================================

FUNCTION tm_jgps_core_calc_majaa

DESCRIPTION
  This function calculates Majaa    Major Axis angle from true north ( 0 .. 180 [Degree] )  
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
int tm_jgps_core_calc_majaa
(
  byte loc_uncrtnty_ang
);


/*===========================================================================

FUNCTION tm_jgps_core_calc_smaj

DESCRIPTION
   This function calculates Smaj Long axis error ( Range: 0 .. 9999 [m] )
 
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
int tm_jgps_core_calc_smaj 
(
  byte loc_uncrtnty_a
);


/*===========================================================================

FUNCTION tm_jgps_core_calc_smin

DESCRIPTION
   This function calculates Smin Short axis error ( Range: 0 .. 9999 [m] )
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
int tm_jgps_core_calc_smin
(
  byte loc_uncrtnty_p
);


/*===========================================================================

FUNCTION tm_jgps_core_calc_altitude

DESCRIPTION
  This function calculates  altitude    Range: -999..+9999[m]
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
int tm_jgps_core_calc_altitude 
(
  sint15 alt
);

/*===========================================================================

FUNCTION tm_jgps_core_calc_vert_err

DESCRIPTION
  This function calculates Vert ( Altitude error )   ( Range: 0 .. 9999 [m] ) 
    
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
int tm_jgps_core_calc_vert_err 
(
  byte loc_uncrtnty_v
);


/*===========================================================================

FUNCTION tm_jgps_core_convert_lr_data

DESCRIPTION
  This function converts LR data from IS801.1 format to KDDI compliant format
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
void tm_jgps_core_convert_lr_data 
( 
  pdsm_pd_info_s_type  *pd_info_ptr
);


/*===========================================================================

FUNCTION tm_jgps_core_get_fixdata

DESCRIPTION
    
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

void tm_jgps_core_get_fixdata
(
  char *data,
  int  len,
  char *key,
  char *buf,
  int  size
);


/*===========================================================================

FUNCTION tm_jgps_core_set_pd_data

DESCRIPTION
    
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

void tm_jgps_core_set_pd_data
(
  char *data,
  char *buf,
  int  size
);


/*===========================================================================

FUNCTION tm_jgps_core_convert_scu_fix_data

DESCRIPTION
    This function translates the POS response data into PDAPI
    format for all JGPS clients.
    
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_jgps_core_convert_scu_fix_data
(
  byte flength,
  char *fixdata
);


/*===========================================================================

FUNCTION tm_jgps_core_save_lr_data_msa

DESCRIPTION
  This function converts LR data from IS801.1 format to KDDI compliant format
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
boolean tm_jgps_core_save_lr_data
(
  pdsm_pd_info_s_type  *pd_info_ptr
);


/*===========================================================================

FUNCTION tm_jgps_core_send_pos_acq_req

DESCRIPTION
  This function sends the POS req or an ACQ after successfully requesting
  and recieving 
  
  1. RF info
  2. MS IP address
  3. URL data.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
boolean tm_jgps_core_send_pos_acq_req( void );


/*===========================================================================

FUNCTION tm_jgps_core_start_pkt_data_req

DESCRIPTION
  This function starts the process of sending either a POS req or an ACQ
  req to SCU.  First, JPGS module must get RF info from TM-Core (async call)
  MS IP address from PDCOMM (async call), and URL data (sync).  
  JGPS must wait for all data to return before sending out a request to SCU
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
boolean tm_jgps_core_start_pkt_data_req( void );


/*===========================================================================

FUNCTION   tm_jgps_core_make_msb_def_url

DESCRIPTION
  This function is used to set default URL data for MSBased sessions.

RETURN VALUE
  None

============================================================================*/
static boolean tm_jgps_core_make_msb_def_url( void );


/*===========================================================================

FUNCTION   tm_jgps_core_make_msa_def_url

DESCRIPTION
  This function is used to set default URL data for MSAssisted sessions.

RETURN VALUE
  None

============================================================================*/
static boolean tm_jgps_core_make_msa_def_url( void );


/*===========================================================================

FUNCTION   tm_jgps_make_url

DESCRIPTION
  This function is used to set URL data

RETURN VALUE
  None

============================================================================*/
boolean tm_jgps_make_url( void );


/*===========================================================================

FUNCTION tm_jgps_core_add_pkt_hdr_and_send

DESCRIPTION
  This function pre-pends the KDDI specific header to the data and sends 
  out the data through the IS-801 Driver
  
DEPENDENCIES

RETURN VALUE
   Number of bytes sent
   
SIDE EFFECTS
  None 

===========================================================================*/

int tm_jgps_core_add_pkt_hdr_and_send
(
    tm_jgps_scu_cmd_id_type cmd_id,
    byte*  start,
    uint16 length
);


/*===========================================================================

FUNCTION tm_jpgs_core_send_msg

DESCRIPTION
  This function sends a message to SCU for POS req, ACQ req, POS cancel, 
  ACQ cancel, ACQ complete.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
boolean tm_jgps_core_send_msg
(
  tm_jgps_scu_cmd_id_type   cmd_id, 
  void                      *data_ptr
);


/*===========================================================================

FUNCTION tm_jgps_core_send_message_cancel

DESCRIPTION
   This function send the correct cancel message to SCU.  Determines if this
   cancel message is for MS-A (POS cancel) or MS-B dload (ACQ cancel)

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_jgps_core_send_msg_cancel
(
  byte cancel_reason
);


/*===========================================================================

FUNCTION tm_jgps_core_send_msg_cancel_others

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_jgps_core_send_msg_cancel_others
(
  pdsm_pd_end_e_type error
);


/*===========================================================================

FUNCTION tm_jgps_core_send_msg_cancel_rej

DESCRIPTION
    Determine the cause for the cancellation from MS->SCU. Send the
    cancellation message.
    
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_jgps_core_send_msg_cancel_rej
(
  pdsm_pd_end_e_type            error,
  pdsm_pd_dload_data_s_type     *dload_ptr
);


/*===========================================================================

FUNCTION tm_jgps_core_send_msg_cancel_timeout

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_jgps_core_send_msg_cancel_timeout(void);


/*===========================================================================

FUNCTION tm_jgps_core_scu_msg_hdr_handler

DESCRIPTION
  This function is used handle the SCU->MS data packet header

RETURN VALUE
  TRUE: No error
  FALSE: Error

DEPENDENCIES

===========================================================================*/
boolean tm_jgps_core_scu_msg_hdr_handler
(
  byte     *hdr_ptr,
  uint16   hdr_ptr_len
);


/*===========================================================================

FUNCTION tm_jgps_core_scu_msg_payload_handler

DESCRIPTION
  This function is used handle the SCU->MS data packet

RETURN VALUE
  TRUE: No error
  FALSE: Error

DEPENDENCIES

===========================================================================*/
boolean tm_jgps_core_scu_msg_payload_handler
(
  byte     *data_ptr,
  uint16   data_ptr_len
);


/*===========================================================================

FUNCTION tm_jgps_core_comm_notify_handler

DESCRIPTION
  This function is used to handle notifications from PD COMM.

RETURN VALUE
  TRUE: No error
  FALSE: Error

DEPENDENCIES

===========================================================================*/
boolean tm_jgps_core_comm_notify_handler
(
  pd_comms_client_ipc_message_type  msg_type,
  void                              *msg_data_ptr
);


/*===========================================================================

FUNCTION tm_jgps_core_comm_write_ack_handler

DESCRIPTION
  This function is used to handle write ack notification from PDCOMM.

RETURN VALUE
  TRUE: No error
  FALSE: Error

DEPENDENCIES

===========================================================================*/
boolean tm_jgps_core_comm_write_ack_handler
(
  pd_comms_client_ipc_message_type  msg_type,
  void                              *msg_data_ptr
);


/*===========================================================================

FUNCTION tm_jgps_core_comm_ioctl_ack_handler

DESCRIPTION
  This function is used to handle IOCTL ACK from PD COMM.

RETURN VALUE
  TRUE: No error
  FALSE: Error

DEPENDENCIES

===========================================================================*/
boolean tm_jgps_core_comm_ioctl_ack_handler
(
  pd_comms_client_ipc_message_type  msg_type,
  void                              *msg_data_ptr
);


/*===========================================================================

FUNCTION tm_jgps_core_comm_read_data_handler

DESCRIPTION
  This function is used to handle READ DATA from PD COMM.

RETURN VALUE
  TRUE: No error
  FALSE: Error

DEPENDENCIES

===========================================================================*/
boolean tm_jgps_core_comm_read_data_handler(void *msg_data_ptr);


/*===========================================================================

FUNCTION tm_jgps_core_set_mode_info

DESCRIPTION
   Set the PPM and PRM mode info, support monitor mode
   
RETURN VALUE
  
DEPENDENCIES

===========================================================================*/
void tm_jgps_core_set_mode_info
(
  pdsm_pd_option_s_type    *option_ptr,
  pdsm_client_type_e_type  client_type
);


/*===========================================================================

FUNCTION tm_jgps_core_mode_is_not_normal

DESCRIPTION

RETURN VALUE
  True if not normal (GPS/AFLT/TEST). False otherwise. (HYBRID)

DEPENDENCIES

===========================================================================*/
boolean tm_jgps_core_mode_is_not_normal( void );


/*===========================================================================

FUNCTION tm_jgps_core_arrange_ppm_data

DESCRIPTION
 The GPS mode, the AFLT mode and TEST mode restrict the number of 
 Pilots and SVs to be used by the calculation of MS Assisted. 
 Also, TEST mode is to select the pilots and the SVs by ascending 
 or descending order. The PPM data is changed here depending on 
 PD mode setting.

RETURN VALUE
  None.

DEPENDENCIES

===========================================================================*/ 
void tm_jgps_core_arrange_ppm_data
(
  srch_ppm_rpt_type *ppm_ptr
);


/*===========================================================================

FUNCTION tm_jgps_core_arrange_prm_data

DESCRIPTION
 The GPS mode, the AFLT mode and TEST mode restrict the number of 
 Pilots and SVs to be used by the calculation of MS Assisted. 
 Also, TEST mode is to select the pilots and the SVs by ascending 
 or descending order. The PRM data is changed here depending 
 on PD mode setting.

RETURN VALUE
  None.

DEPENDENCIES

===========================================================================*/
void tm_jgps_core_arrange_prm_data
(
  sm_GpsMeasRptStructType *prm_ptr
);


/*===========================================================================

FUNCTION tm_jgps_core_reset_prm_satnum

DESCRIPTION
  This function is used to reset PRM info if PRM num is set to 
  TM_JGPS_PRM_PRN_MIN.

RETURN VALUE
  None.

DEPENDENCIES

===========================================================================*/
void tm_jgps_core_reset_prm_satnum
(
  sm_GpsMeasRptStructType *prm_ptr
);

#endif /* FEATURE_JCDMA_GPSONE */
  
#ifdef __cplusplus
}
#endif

#endif /* TM_JGPS_CORE_I_H */

