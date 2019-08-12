#ifndef DSUCSDNT_H
#define DSUCSDNT_H
/*===========================================================================

            U M T S   N O N - T R A N S P A R E N T   C I R C U I T
                          S W I T C H E D   D A T A      
                            H E A D E R   F I L E

DESCRIPTION
   This header file contains the interface the the GSM/WCDMA 
   Non-transparent Circuit Switched data modeule. 
  
Copyright (c) 2003 - 2011 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dsucsdnt.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/05/11   SS      Q6 free floating support.
06/27/07   AR      Add multiple client support for unified API.
09/01/06   AR      Added support for multiprocessor build.
02/06/06   AR      Lint corrections.
11/21/05   AR      Add support for multiple call instances.
04/26/05   AR      Add rlp_connect_state NV item for CONNECT result timing.
10/19/04   AR      Add rlp_adm_idle timer parameter stored in NVRAM.
09/08/04   AR      Correct ordering of v42_comp_direction_T values.
06/07/04   AR      Added storage of RLP recovery mode to parameter set.
04/06/04   AR      Added storage of RLP version for each mode.
07/15/03   TMR     Added support for RLP Version 2
04/17/03   AR      Added prototype for ds_ucsd_nt_pwrup_init().
02/17/03   TMR     Created

===========================================================================*/


/*===========================================================================
Header files
===========================================================================*/
#include "dsm.h"
#include "sys.h"
#include "cm.h"
#include "dsucsdi.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/* Types of PDU in Non-transparent data services */
typedef enum PDU_T
{
  PDU_TYPE_INVALID = -1,
  PDU_TYPE_SMALL,
  PDU_TYPE_LARGE
} PDU_Type;

/* Default RLP parameter values */
#define DEF_RLP_IWS           480
#define DEF_RLP_MWS           480
#define DEF_RLP_T1            52
#define DEF_RLP_N2            6

#define DEF_RLP_Pt            0
#define DEF_RLP_P0            0
#define DEF_RLP_COMP_NEG      0
#define DEF_RLP_P1            2048
#define DEF_RLP_P2            6
#define DEF_RLP_T4            50

#define DEF_RLP_PDU_TYPE      PDU_TYPE_SMALL

/* Supported RLP versions */
#define XIDMIN_VERSION           0
#define XIDMAX_VERSION           2

#define XIDDEFAULT_VERSION_0_1   1
#define XIDDEFAULT_VERSION_2     2

/* NT connection states */
#define NT_CONNECT_STATE_L2R     0
#define NT_CONNECT_STATE_RLP     1
#define NT_CONNECT_STATE_MAX     NT_CONNECT_STATE_RLP

/* This MUST be an even power of 2!!*/
#define QUEUE_SIZE         8

/*===========================================================================
typedef the indication report call-back function type
===========================================================================*/

typedef void (*ds_ucsd_nt_ind_report_cb_T)
(
  ds_ucsd_ind_T    indication,
  cm_call_id_type  call_id
);


/*===========================================================================
typedef the callback that will indicate when no TX frames are sent
in WCDMA mode due to duplicate frame suppression
===========================================================================*/
typedef void (*ds_ucsd_nt_no_tx_frames_sent_cb_T) (void);


/*===========================================================================
typedef the SIO related call-back functions
===========================================================================*/

   /* Function that will be used by the NT data stack to 
   ** read data from sio.  Data read from SIO is returned
   ** in the rx_data, and TRUE is returned if data was successfully
   ** read, FALSE if not (no data available)
   */
typedef boolean (*ds_ucsd_nt_rx_from_sio_cb_T) 
(
   uint8 *rx_data
);

   
   /* Function to be used by NT data to transmit data to SIO.
   ** The data parameter is the data item to be written to SIO,
   ** the data_valid parameter is used to indicate if there is
   ** valid information in the data parameter, and the write_data
   ** is used to tell write any queued tata to SIO
   */
typedef boolean (*ds_ucsd_nt_tx_to_sio_cb_T)
(
   uint8    data,
   boolean  data_valid, 
   boolean  write_data
);
   
   /* This function queries the state of the SIO modem status
   ** signals (DTR, DSR).
   */
typedef boolean (*ds_ucsd_nt_get_sio_status_cb_T) 
(
   ds_ucsd_sio_status_T  *status
);
                                             
   /* This function is used by NT data to indicate how it would like CTS
   ** to the TE to be set.  The actual setting will be based both on this
   ** value as well as on the state of data queued in the watermarks and the
   ** watermark values
   */                                         
typedef void (*ds_ucsd_nt_set_sio_cts_state_cb_T)  
(
   boolean on_off
);
   
   /* Function used to find out if the SIO TX (to TE) queue is getting full.
   ** If the queue is getting full, NT will send a CTS down indication to the
   ** far end L2R entity to shut off the flow of data to the mobile.  Note 
   ** that it will take a while to get the data shut off, so when this 
   ** function returns TRUE, it still needs to provide room to store data...
   ** the amount of which is dependent on the async data rate.
   */                                          
typedef boolean (*ds_ucsd_nt_is_sio_tx_q_full_cb_T) 
(
   void
);


   /* Function used to suspend/resume sio TX (to TE) data flow.
   ** If function is called with TRUE, data flow to TE will be suspended,
   ** If called with FALSE, data flow to TE will resume.
   */                                          
typedef void (*ds_ucsd_nt_suspend_sio_tx_cb_T) 
(
   boolean on_off
);

   /* Function used to initialize the SIO subsystem.
   */                                          
typedef void (*ds_ucsd_nt_init_sio_cb_T) 
(
   void
);

/*===========================================================================
typdef the OTA call-back functions.
===========================================================================*/

   /* Function used to read rx OTA data from lower layers.  
   ** dl_data_ptr parameter is set by the callback function to 
   ** point to the block of rx data.  Boolean value returned
   ** indicates whether or not rx data was successfully read
   ** or not.  frame_size indicates the amount of data to be read.
   */
typedef boolean (*ds_ucsd_nt_read_dl_ota_data_cb_T) 
(
   uint8                      **dl_data_ptr
);

   /* Function used to write tx data to the lower layers.
   ** ul_data_ptr points to the data block to be written, frame_size
   ** indicates how much data there is to be written.  Function is
   ** to return a boolean indicating whether or not write operation
   ** was successful.
   */
typedef boolean (*ds_ucsd_nt_write_ul_ota_data_cb_T) 
(
   uint8                      *ul_data_ptr
);

   /* This function is used to return a buffer to be used to 
   ** for tx data prior to it being written via the above
   ** function call.  Function returns the tx data buffer
   ** via the ul_data_ptr parameter, and the size of the buffer
   ** wanted is indicated by the frame_size parameter.  Function
   ** is to return a boolean indicating whether or not the 
   ** operation was successful.
   */
typedef boolean (*ds_ucsd_nt_get_ul_ota_buf_cb_T) 
(
   uint8                      **ul_data_ptr
);

   /* This function is used to initialize the OTA subsystem prior
   ** to starting up a call.  In the below function used to register
   ** this callback function, this function pointer may be set to 
   ** NULL if no initialization function is needed
   */
typedef void (*ds_ucsd_nt_init_ota_cb_T) (void); 

typedef struct 
{
   DSI_Event_T Event[QUEUE_SIZE];
   short       Read;
   short       Write;
} DSISR_EventQueue_T;

typedef struct
{
  ds_ucsd_nt_is_sio_tx_q_full_cb_T   is_sio_tx_q_full_cb;
  ds_ucsd_nt_read_dl_ota_data_cb_T   read_dl_ota_data_cb;
  ds_ucsd_nt_write_ul_ota_data_cb_T  write_ul_ota_data_cb;
  ds_ucsd_nt_get_ul_ota_buf_cb_T     get_ul_ota_buf_cb;
} ds_ucsd_nt_cb_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION ds_ucsd_nt_register_ind_report_func

DESCRIPTION
  This function registers the function that will be used by the UMTS
  non-transparent data stack to report various indications.
  
DEPENDENCIES
  This function MUST be called prior to calling
  ds_ucsd_nt_init_call_data()

RETURN VALUE
  None

SIDE EFFECTS
  Sets the internally kept NT data stack indicator report callback function

===========================================================================*/
extern void  
ds_ucsd_nt_register_ind_report_func
(
   ds_ucsd_nt_ind_report_cb_T    ind_report_cb   
);


/*===========================================================================

FUNCTION ds_ucsd_nt_register_no_tx_frames_sent_func

DESCRIPTION
  This function registers the function that will be used by the UMTS
  non-transparent data stack to report when no tx data frames were
  sent in WCDMA mode due to duplicate frame suppression being enabled,
  and tx data being all duplicate frames
  
DEPENDENCIES
  This function MUST be called prior to calling
  ds_ucsd_nt_init_call_data()

RETURN VALUE
  None

SIDE EFFECTS
  Sets the internally kept NT data stack indicator report callback function

===========================================================================*/
extern void
ds_ucsd_nt_register_no_tx_frames_sent_func
(
   ds_ucsd_nt_no_tx_frames_sent_cb_T    no_tx_frames_sent_cb   
);


/*===========================================================================

FUNCTION ds_ucsd_nt_register_sio_funcs

DESCRIPTION
  This function is used to register with the UMTS NT data stack the 
  various callback functions related to the SIO interface.  Note that
  valid callback function pointers MUST be supplied for all functions
  except for the initialization (ini_sio_cb) function which may be
  NULL
  
DEPENDENCIES
  This function MUST be called prior to calling
  ds_ucsd_nt_init_call_data()

RETURN VALUE
  None

SIDE EFFECTS
  Sets the internally kept NT data stack SIO callback function pointers

===========================================================================*/
extern void
ds_ucsd_nt_register_sio_funcs
(
   ds_ucsd_nt_rx_from_sio_cb_T        rx_from_sio_cb,
   ds_ucsd_nt_tx_to_sio_cb_T          tx_to_sio_cb,
   ds_ucsd_nt_get_sio_status_cb_T     get_sio_status_cb,
   ds_ucsd_nt_set_sio_cts_state_cb_T  set_sio_cts_state_cb,
   ds_ucsd_nt_is_sio_tx_q_full_cb_T   is_sio_tx_q_full_cb,
   ds_ucsd_nt_suspend_sio_tx_cb_T     suspend_sio_tx_cb,
   ds_ucsd_nt_init_sio_cb_T           init_sio_cb
);


/*===========================================================================

FUNCTION ds_ucsd_nt_register_ota_funcs

DESCRIPTION
  This function is used to register with the UMTS NT data stack the 
  various callback functions related the OTA interface.  Note that
  valid callback function pointers MUST be supplied for all functions
  except for the initialization (ini_ota_cb) function which may be
  NULL.
  
DEPENDENCIES
  This function MUST be called prior to calling
  ds_ucsd_nt_init_call_data() 

RETURN VALUE
  None

SIDE EFFECTS
  Sets the internally kept NT data stack OTA callback function pointers

===========================================================================*/
extern void
ds_ucsd_nt_register_ota_funcs
(
   ds_ucsd_nt_read_dl_ota_data_cb_T   read_dl_ota_data_cb,
   ds_ucsd_nt_write_ul_ota_data_cb_T  write_ul_ota_data_cb,
   ds_ucsd_nt_get_ul_ota_buf_cb_T     get_ul_ota_buf_cb,
   ds_ucsd_nt_init_ota_cb_T           init_ota_cb
);




/*===========================================================================

FUNCTION ds_ucsd_nt_init_call_data

DESCRIPTION
  This function is used to initialize data for every call.  It 
  should be called when the call is connected.  Note that the
  suppress_wcdma_duplicate_tx_data parameter should be set to TRUE
  to stop duplicate TX data from being transmitted in WCDMA mode.
  
DEPENDENCIES
  ds_ucsd_nt_register_ota_funcs() and ds_ucsd_nt_register_sio_funcs(),
  and ds_ucsd_nt_register_ind_report_func() MUST be called prior to 
  calling this function.

RETURN VALUE
  None

SIDE EFFECTS
  Sets the internally kept NT data stack state information.

===========================================================================*/
extern void
ds_ucsd_nt_init_call_data
(
  const cm_call_id_type                  call_id,
  const sys_sys_mode_e_type              curr_mode,
  const ds_ucsd_frame_size_T             bytes_per_sdu,                           
  const uint32                           sdus_per_tti,
  const ds_ucsd_nt_rlp_parameters_T     *rlp_params_ptr,
  const ds_ucsd_data_rate_T              data_rate,
  const boolean                          bc_compression_enabled,
  const boolean                          suppress_wcdma_duplicate_tx_data,
  ds_ucsd_set_data_counters_cb_T         set_data_counters_cb      
);   


/* <EJECT> */
/*===========================================================================

FUNCTION ds_ucsd_nt_pwrup_init

DESCRIPTION
  This function is used to initialize NT at powerup. Operations that occur
  only during task startup are invoked here.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/ 
void
ds_ucsd_nt_pwrup_init ( void );


/* <EJECT> */
/*===========================================================================

FUNCTION  ds_control_non_transparent_data_services

DESCRIPTION
   This function is used to drive the Non-Transparent data processing
   and state machine.  It should be called only after the above initialization
   functions are called.  This function will be called each time one
   of the relevent events is to be processed and/or for each transmit and
   receive TTI.  Each TTI time, the process_tx or process_rx_data 
   parameters must be set.  Although in theory, both the process_tx_data and
   process_rx_data can both be set when calling this function, this has not 
   been tested in the Qualcomm GSM environment.
         
DEPENDENCIES
  ds_ucsd_nt_register_ota_funcs(), ds_ucsd_nt_register_sio_funcs() and
  ds_ucsd_nt_init_call_data() must be called first
  
RETURN VALUE
  None
    
SIDE EFFECTS
  Lots.

===========================================================================*/
extern void 
ds_ucsd_nt_process_data
(                                               
  ds_ucsd_event_T event,
  uint8           call_id,  
  boolean         process_tx_data,
  boolean         process_rx_data
);

/*===========================================================================

FUNCTION ds_ucsd_nt_get_Ds_FillPattern

DESCRIPTION
  This function is used to access the Ds_FillPattern array

DEPENDENCIES
  None

RETURN VALUE
  uint8 *

SIDE EFFECTS
  None

===========================================================================*/ 
uint8 * ds_ucsd_nt_get_Ds_FillPattern(void);

/*===========================================================================

FUNCTION ds_ucsd_nt_get_Evq_To_L2R

DESCRIPTION
  This function is used to access Evq_To_L2R

DEPENDENCIES
  None

RETURN VALUE
  DSISR_EventQueue_T *

SIDE EFFECTS
  None

===========================================================================*/
DSISR_EventQueue_T * ds_ucsd_nt_get_Evq_To_L2R(void);

/*===========================================================================

FUNCTION ds_ucsd_nt_get_Evq_To_RLP

DESCRIPTION
  This function is used to access Evq_To_RLP

DEPENDENCIES
  None

RETURN VALUE
  DSISR_EventQueue_T *

SIDE EFFECTS
  None

===========================================================================*/
DSISR_EventQueue_T * ds_ucsd_nt_get_Evq_To_RLP(void);

/*===========================================================================

FUNCTION ds_ucsd_nt_get_curr_mode

DESCRIPTION
  This function is used to access Ds_ucsd_nt_curr_mode

DEPENDENCIES
  None

RETURN VALUE
  sys_sys_mode_e_type

SIDE EFFECTS
  None

===========================================================================*/
sys_sys_mode_e_type ds_ucsd_nt_get_curr_mode(void);

/*===========================================================================

FUNCTION ds_ucsd_nt_get_bytes_per_sdu

DESCRIPTION
  This function is used to access Ds_ucsd_nt_bytes_per_sdu

DEPENDENCIES
  None

RETURN VALUE
  ds_ucsd_frame_size_T

SIDE EFFECTS
  None

===========================================================================*/
ds_ucsd_frame_size_T ds_ucsd_nt_get_bytes_per_sdu(void);

/*===========================================================================

FUNCTION ds_ucsd_nt_get_data_rate

DESCRIPTION
  This function is used to access Ds_ucsd_nt_data_rate

DEPENDENCIES
  None

RETURN VALUE
  ds_ucsd_data_rate_T

SIDE EFFECTS
  None

===========================================================================*/
ds_ucsd_data_rate_T ds_ucsd_nt_get_data_rate(void);

/*===========================================================================

FUNCTION ds_ucsd_nt_get_suppress_duplicate_tx_frames

DESCRIPTION
  This function is used to access Ds_ucsd_nt_suppress_duplicate_tx_frames

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_ucsd_nt_get_suppress_duplicate_tx_frames(void);

/*===========================================================================

FUNCTION ds_ucsd_nt_set_tx_frame_sent

DESCRIPTION
  This function is used to set Ds_ucsd_nt_tx_frame_sent

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_ucsd_nt_set_tx_frame_sent
(
  boolean value
);

/*===========================================================================

FUNCTION ds_ucsd_nt_set_frame_suppressed

DESCRIPTION
  This function is used to set Ds_ucsd_frame_suppressed

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_ucsd_nt_set_frame_suppressed
(
  boolean value
);

/*===========================================================================

FUNCTION ds_ucsd_nt_get_DSM_Level_Low

DESCRIPTION
  This function is used to get DSM_Level_Low

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_ucsd_nt_get_DSM_Level_Low(void);

/*===========================================================================

FUNCTION ds_ucsd_nt_set_DSM_Level_Low

DESCRIPTION
  This function is used to set DSM_Level_Low

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_ucsd_nt_set_DSM_Level_Low
(
  boolean value
);

/*===========================================================================

FUNCTION ds_ucsd_nt_get_rlp_P0

DESCRIPTION
  This function is used to get Ds_ucsd_nt_rlp P0 param

DEPENDENCIES
  None

RETURN VALUE
  byte

SIDE EFFECTS
  None

===========================================================================*/
byte ds_ucsd_nt_get_rlp_P0(void);

/*===========================================================================

FUNCTION ds_ucsd_nt_get_cb_info

DESCRIPTION
  This function is used to access ds_ucsd_nt_cb_info

DEPENDENCIES
  None

RETURN VALUE
  ds_ucsd_nt_cb_type *

SIDE EFFECTS
  None

===========================================================================*/
ds_ucsd_nt_cb_type * ds_ucsd_nt_get_cb_info(void);

#endif /* DSUCSDNT_H */
