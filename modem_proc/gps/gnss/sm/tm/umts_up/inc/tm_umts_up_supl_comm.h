/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

   TM UMTS_UP SUPL module interface with PDCOMM.


GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2000 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/umts_up/inc/tm_umts_up_supl_comm.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

  when       who     what, where, why
 --------   ---     ----------------------------------------------------------
 03/10/15    lt     Proper handling of TLS version in SUPL messages for areaEventTriggered sessions. 
 05/14/13    ssh    Handle TCP connections in back to back SUPL sessions
===========================================================================*/

#ifndef TM_SUPL_COMM_H
#define TM_SUPL_COMM_H

#include "gps_variation.h"
#ifdef __cplusplus
extern "C" {
#endif

#ifdef PDCOMM_INFO_DECLARED
#define extern
#endif

#define SUPL_LENGTH_HDR_SIZE     2
#define TM_SUPL_MAX_BUFFSIZE   200  /* Maximum SUPL size */

typedef enum 
{
  TM_SUPL_COMM_RECV_HEADER=0,
  TM_SUPL_COMM_RECV_PAYLOAD
} tm_supl_comm_recv_state;

typedef struct
{
    uint16      num_bytes_to_write;
    uint16      num_bytes_written;
    byte        data[TM_SUPL_MAX_BUFFSIZE];
} tm_supl_comm_output_buffer;


typedef struct tm_supl_pdcomm_info {
    pd_comms_state_type             curr_state;
    tm_supl_comm_output_buffer      tx_buffer;
    tm_supl_comm_recv_state         recv_state;
    uint16                          num_bytes_to_read;
    boolean                         tls_session_authentication_failure;   
    boolean                         used_auto_configured_hslp_address;
    uint32                          disconnect_transaction_id;    
    /*Whether this connection is for Emergency SUPL Session*/
    boolean                         emergency_connection;
} tm_supl_pdcomm_info_s_type;


extern tm_supl_pdcomm_info_s_type   z_pdcomm_tcp_info;  
extern tm_supl_pdcomm_info_s_type   z_pdcomm_sms_info;
extern tm_supl_pdcomm_info_s_type   z_pdcomm_udp_info;  
extern pd_comms_handle_type         z_pdcomm_handle_tcp;
extern pd_comms_handle_type         z_pdcomm_handle_sms;
extern pd_comms_handle_type         z_pdcomm_handle_udp;


extern boolean                      pdcomm_tcp_handle_get_done;



/*===========================================================================

FUNCTION tm_umts_up_supl_comm_handle_notification

DESCRIPTION
  This function handles PDCOMM notifications. 
  
DEPENDENCIES: none.

RETURN VALUE: TRUE/FALSE.

SIDE EFFECTS: none.

===========================================================================*/
void tm_umts_up_supl_comm_handle_notification ( void *msg_data_ptr );



/*===========================================================================

FUNCTION tm_umts_up_supl_comm_tcp_connect

DESCRIPTION
  This function is used to establish a TCP link to SLP.

RETURN VALUE
  pd_comms_return_type

DEPENDENCIES

===========================================================================*/
pd_comms_return_type tm_umts_up_supl_comm_tcp_connect( uint8 u_supl_version_main );


/*===========================================================================

FUNCTION tm_umts_up_supl_comm_udp_bind

DESCRIPTION
  This function is used to bind a UDP socket.

RETURN VALUE
  void

DEPENDENCIES

===========================================================================*/
void tm_umts_up_supl_comm_udp_bind( boolean srv_system_changed );

/*===========================================================================

FUNCTION tm_umts_up_supl_comm_abort_udp_bind_retries

DESCRIPTION
  This function is used to abort any UDP ding retries.

RETURN VALUE
  void

DEPENDENCIES

===========================================================================*/
void tm_umts_up_supl_comm_abort_udp_bind_retries(void);

/*===========================================================================

FUNCTION tm_umts_up_supl_comm_get_tcp_state

DESCRIPTION
 Get the current Tcp state of PDCOMM

RETURN VALUE
  Enum representing the current Tcp PDCOMM state

DEPENDENCIES

===========================================================================*/ 
pd_comms_state_type tm_umts_up_supl_comm_get_tcp_state(void);


/*===========================================================================

FUNCTION tm_supl_comm_get_handle

DESCRIPTION
  This function is used to initialize pdcomm for SUPL.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/

boolean tm_umts_up_supl_comm_init ( void );

/*===========================================================================

FUNCTION tm_umts_up_supl_comm_write

DESCRIPTION
  This function is used to write data to socket.

RETURN VALUE
  pd_comms_return_type

DEPENDENCIES

===========================================================================*/
pd_comms_return_type tm_umts_up_supl_comm_write ( uint8 *p_data, uint16 w_size );


/*===========================================================================

FUNCTION PDE_CLOSE_CONNECTION

DESCRIPTION
  This function is used to disconnect with PDE.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/

void tm_umts_up_supl_comm_close_connection( void );

/*===========================================================================

FUNCTION tm_supl_handle_comm_nofification

DESCRIPTION
  This function is used to handle READ DATA from PD COMM.

RETURN VALUE
  TRUE: No error
  FALSE: Error

DEPENDENCIES

===========================================================================*/
boolean tm_umts_up_supl_comm_handle_read_data(void *msg_data_ptr);


#if 0
/*===========================================================================

FUNCTION tm_supl_comm_handle_write_ack

DESCRIPTION
  This function is used to handle WRITE ACK from PD COMM.

RETURN VALUE
  TRUE: No error
  FALSE: Error

DEPENDENCIES

===========================================================================*/
void tm_supl_comm_handle_write_ack(void *msg_data_ptr);
#endif

/*===========================================================================

FUNCTION tm_umts_up_supl_comm_handle_hash_resp

DESCRIPTION
  This function handles hash response from PDCOMM 
  
DEPENDENCIES: none.

SIDE EFFECTS: none.

===========================================================================*/

void tm_umts_up_supl_comm_handle_hash_resp (void *msg_data_ptr); 

/********************************************************************
*
* tm_umts_up_supl_comm_pass_root_cert_dir
*
* Function description:
*  This function passes a root certificate directory to PDCOMM. This
*  is the directory that will contain all the SUPL SSL certificates. 
* 
* Parameters: 
* efs_data - Pointer to the EFS data sent from PDAPI

* Return:
*  TRUE/FALSE
*  
*********************************************************************
*/
boolean tm_umts_up_supl_comm_pass_root_cert_dir(void);

/********************************************************************
*
* tm_umts_up_supl_comm_root_certs_updated
*
* Function description:
*  This function lets PDCOMM know that root certs dir has been updated.
* 
* Parameters: 

* Return:
*  
*********************************************************************
*/
void tm_umts_up_supl_comm_root_certs_updated(void);

/********************************************************************
*
* tm_umts_up_supl_comm_retrieve_imsi
*
* Function description:
*  Retrieve the IMSI value from the the SIM and store a copy 
*  in the Ram (z_umts_nv_item_ram_copy).
*
* Parameters: 
*
* Return:
*  TRUE/FALSE
*  
*********************************************************************
*/
boolean tm_umts_up_supl_comm_retrieve_imsi(void);

/*===========================================================================
FUNCTION tm_1x_up_comm_app_ioctl

DESCRIPTION
  This function is to send the ioctl to PDCOMM

RETURN VALUE
  Boolean

DEPENDENCIES      

===========================================================================*/
boolean  tm_umts_up_supl_comm_app_ioctl(pd_comms_ioctl_cmd_type ioctl_cmd);

/*===========================================================================
FUNCTION tm_umts_supl_comm_sms_read_state_init

DESCRIPTION
  This function is used to initialize sms read state.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_umts_supl_comm_sms_read_state_init(void);


/*===========================================================================
FUNCTION tm_umts_up_supl_comm_free_pdcomm_rx_buff

DESCRIPTION
  This function frees the buffer allocated for pdcomm_rx.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_umts_up_supl_comm_free_pdcomm_rx_buff(void);


/*===========================================================================
FUNCTION tm_umts_up_supl_comm_ni_sms

DESCRIPTION
  This function will handle the NI SMS injected from PDAPI

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_umts_up_supl_comm_ni_sms(uint32 length, uint8 *data);


#ifdef extern
#undef extern
#endif


#endif 

