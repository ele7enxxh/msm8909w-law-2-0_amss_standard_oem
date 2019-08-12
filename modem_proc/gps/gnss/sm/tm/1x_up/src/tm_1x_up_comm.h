#ifndef TM_1X_UP_COMM_H
#define TM_1X_UP_COMM_H

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                  TM_1X_UP Comm Header File

DESCRIPTION
  This header file contains definitions of data structure necessary for TM 1X UP Comm  module

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/1x_up/src/tm_1x_up_comm.h#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/15/06    ank     Added wrapper for Force_Dormancy, Unforce_Dormancy
10/25/06    ank     Initial Release
===========================================================================*/

/*=============================================================================

                                INCLUDE FILES

=============================================================================*/
#include "pd_comms_api.h"
#include "tm_is801.h"
#include "tm_common.h"

/* For IS-801 Header */
#define IS801_LENGTH_HDR_SIZE 1

#define TM_1X_UP_TRUSTED_CUSTOM_IP_HEADER_SIZE 24                  /* Custom header size */

#define TM_1X_UP_JGPS_HEADER_SIZE 6 /* KDDI specific header: ServID 2+ProtVer 1 + Len 2+ cmd id 1 */

#define TM_1X_UP_COMM_DATA_SIZE (TM_MAX_BUFFSIZE+\
                                 TM_1X_UP_TRUSTED_CUSTOM_IP_HEADER_SIZE+\
                                 TM_1X_UP_JGPS_HEADER_SIZE)


#define MSID_HDR_START 2

#define TM_1X_UP_CUSTOM_IP_RESERVED_VAL  0x00
#define RESERVED_SIZE 1
#define LENGTH_HDR_SIZE 1
typedef struct
{
    uint16      num_bytes_to_write;
    uint16      num_bytes_written;
    uint16      cust_hdr_size;
    byte        data[TM_1X_UP_COMM_DATA_SIZE];
} tm_1x_up_comm_output_buffer;

typedef enum 
{
  TM_1X_UP_COMM_RECV_HEADER=0,
  TM_1X_UP_COMM_RECV_PAYLOAD
} tm_1x_up_comm_recv_state;

/* Structure for PD COMM transaction id info */
typedef struct {
  uint32  base;  
  uint32  range;    
  uint32  curr_id;    
} tm_1x_up_pdcomm_tr_id_s_type;

typedef struct tm_1x_up_pdcomm_info {
    pd_comms_handle_type            handle;
    boolean                         handle_valid;
    tm_1x_up_pdcomm_tr_id_s_type    tr_id_info;                                                
    pd_comms_state_type             curr_state;
    boolean                         cust_hdr_sent;
    tm_1x_up_comm_output_buffer     buffer;
    
    uint8                           pde_version;
    /* number of 801-1 rev link pddm to send, and the raw data */
    int                             num_rev_pddm_801_1;
    is801_rev_link_pddm_info_s_type rev_pddm; 
    /* number of 801-B rev link pddm to send, and the raw data */
    int                             num_rev_pddm_801_b;
    byte                            rev_pddm_is801b[MAX_IS801_PDDM_SIZE];

    /* this pointer will point to the last pddm to be sent by 1x_up_comm.
     * This data will be used for logging the pddm once its confirmed to be
     * sent */
    byte*                           last_pddm_sent;
    uint16                          last_pddm_len;
    uint8                           last_pddm_type;

    tm_1x_up_comm_recv_state        recv_state;
    uint16                          num_bytes_to_read;
    boolean                         wait_for_sess_end_ack;
    boolean                         abort;
    uint32                          connect_tr_id;
    uint32                          disconnect_tr_id;
} tm_1x_up_pdcomm_info_s_type;

/*===========================================================================

FUNCTION tm_1x_up_comm_GET_PDE_ADDRESS

DESCRIPTION
  This function is used to obtain PDE address from PDSM

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/

extern dword tm_1x_up_comm_get_pde_address( void );

/*===========================================================================

FUNCTION PDSM_PDE_PORT_ID

DESCRIPTION
  This function is used to obtain PDE connecting Port id.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/

extern dword tm_1x_up_comm_get_pde_port_id( void );

/*===========================================================================

FUNCTION PDE_CLOSE_CONNECTION

DESCRIPTION
  This function is used to disconnect with PDE.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/

extern void tm_1x_up_comm_close_connection(void);

/*===========================================================================

FUNCTION tm_1x_up_comm_abort_connection

DESCRIPTION
  This function is used to abort connection with PDE.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/

extern void tm_1x_up_comm_abort_connection( void );


/*===========================================================================

FUNCTION tm_1x_up_comm_write

DESCRIPTION
  This function is used to write data to socket.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
extern boolean tm_1x_up_comm_write
(
    byte *data, 
    uint16 size
);

/*===========================================================================

FUNCTION tm_1x_up_comm_read

DESCRIPTION
  This function is used to read data.

RETURN VALUE
  Boolean

DEPENDENCIES      
There has to a successful pd_comms_app_connect() call which returned this handle before this function can be called. 
A pd_comms_app_read() call is not allowed when a previous pd_comms_app_read() has still not returned any data. 
In an attempt to do so, a PDCOMM_RESULT_ERROR_PREVIOUS_READ_PENDING will be returned.

===========================================================================*/
extern boolean tm_1x_up_comm_read
(
  uint16 length,
  boolean wait_all 
);

/*===========================================================================

FUNCTION tm_1x_up_comm_disconnect

DESCRIPTION
  This function is used to close phyiscal path with PDE.

RETURN VALUE
  pd_comms_return_type

DEPENDENCIES

===========================================================================*/
extern pd_comms_return_type tm_1x_up_comm_disconnect
(
  boolean abort 
);


/*===========================================================================

FUNCTION tm_1x_up_comm_send

DESCRIPTION
  This function sends a IS801 output buffer passed in by IS801 module. The type
  of the pddm being sent is provided in pddm_type.

DEPENDENCIES
  None

RETURN VALUE
  TRUE 
  FALSE

SIDE EFFECTS
  None

===========================================================================*/

extern boolean tm_1x_up_comm_send
(
    byte*  start,
    uint16 length,
    uint8  pddm_type
);

/*===========================================================================

FUNCTION tm_1x_up_comm_connect_with_pde

DESCRIPTION
  This function is used to connect with PDE. It gets invoked when client requests
  for a Position location.
  It invokes PDCOMM functions to open and then connect to PDE. The result of connection
  request is available through asynchronous callbacks.

RETURN VALUE
  PDCOMM state

DEPENDENCIES
  This function makes calls to asynchronous PDCOMM functions.

===========================================================================*/

extern boolean tm_1x_up_comm_connect_with_pde(void);

/*===========================================================================

FUNCTION tm_1x_up_handle_comm_msg

DESCRIPTION
  This function is used to handle messages from PD COMM.

RETURN VALUE
  boolean

DEPENDENCIES

===========================================================================*/
extern boolean tm_1x_up_handle_comm_msg(
    pd_comms_client_ipc_message_type  msg_type,
    void                              *msg_data_ptr);

/*===========================================================================

FUNCTION tm_1x_up_comm_get_handle

DESCRIPTION
  This function is used to get a new pdcomm handle. For V1/V2, connection to MPC and connection to
  PDE will be indetified by separate handles.

RETURN VALUE
  boolean

DEPENDENCIES
  Called by registered clients.

===========================================================================*/
extern boolean tm_1x_up_comm_get_handle( void );

/*===========================================================================

FUNCTION tm_1x_up_comm_init

DESCRIPTION

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
extern void tm_1x_up_comm_init(void);

/*===========================================================================

FUNCTION tm_1x_up_comm_reset

DESCRIPTION

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
extern void tm_1x_up_comm_reset(void);


/*===========================================================================

FUNCTION tm_1x_up_comm_app_ioctl

DESCRIPTION
  This function is to send the ioctl to PDCOMM

RETURN VALUE
  Boolean

DEPENDENCIES      

===========================================================================*/
extern boolean tm_1x_up_comm_app_ioctl(pd_comms_ioctl_cmd_type ioctl_cmd);


/*===========================================================================

FUNCTION tm_1x_up_force_dormancy

DESCRIPTION
  This function is used to force dormancy.

RETURN VALUE
  None

DEPENDENCIES      

===========================================================================*/
extern boolean tm_1x_up_comm_force_dormancy(void);

/*===========================================================================

FUNCTION tm_1x_up_unforce_dormancy

DESCRIPTION
  This function is used to unforce dormancy.

RETURN VALUE
  None

DEPENDENCIES      

===========================================================================*/
extern boolean tm_1x_up_comm_unforce_dormancy(void);

/*===========================================================================

FUNCTION tm_1x_up_comm_get_state

DESCRIPTION
 Get the current state of PDCOMM

RETURN VALUE
  Enum representing the current PDCOMM state

DEPENDENCIES

===========================================================================*/
extern pd_comms_state_type tm_1x_up_comm_get_state(void);

/*===========================================================================

FUNCTION tm_1x_up_handle_comm_nofification

DESCRIPTION
  This function is used to handle WRITE ACK from PD COMM.

RETURN VALUE
  TRUE: No error
  FALSE: Error

DEPENDENCIES

===========================================================================*/
extern boolean tm_1x_up_handle_comm_write_ack(void *msg_data_ptr);


/*===========================================================================

FUNCTION tm_1x_up_handle_comm_nofification

DESCRIPTION
  This function is used to handle IOCTL ACK from PD COMM.

RETURN VALUE
  TRUE: No error
  FALSE: Error

DEPENDENCIES

===========================================================================*/
extern boolean tm_1x_up_handle_comm_ioctl_ack(void *msg_data_ptr);


#ifdef __cplusplus
}
#endif

#endif /* TM_1X_UP_H */
