#ifndef DSGCSDI_H
#define DSGCSDI_H
/*===========================================================================

              G C S D   P R O T O C O L   S T A C K   I N T E R N A L   
                             H E A D E R   F I L E

DESCRIPTION
  This header file contains definitions used internally by the GCSD protcol
  stack.   It provides internal interface to the dsgcsd*.c files. 
  It should NOT be included by any source file outside the GCSD protocol
  stack subsystem.

  
Copyright (c) 2002 - 2012 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/
/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/GSM/GCSD/vcs/dsgcsdi.h_v   1.10   25 Jun 2002 13:11:16   trebman  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dsgcsdi.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/22/12   ND      Fixing compilation error due to TCB variables undeclared.
04/05/11   SS      Q6 free floating support.
04/01/11   TTV     Support for Q6 free floating.
03/04/09   SA      AU level CMI modifications.
06/06/08   SA      Migrated INTLOCK in SIO to Rex Critical section.
03/25/08   SA      Added internal functions from dsgcsd_task.h.
09/01/06   AR      Add support for multiprocessor build.
11/21/05   AR      Add support for multiple call instances.
08/05/03   TMR     Updated watermark callbacks for DSM 2
01/10/02   TMR     Initial release
02/26/02   TMR     Added DS_XID_NEG_COMPL

===========================================================================*/

/*===========================================================================
Header files
===========================================================================*/

#include "datamodem_variation.h"
#include "cm.h"
#include "dsm.h"
#include "dsucsdi.h"
#include "dsgcsd_task_v.h"
#include "dsucsdbc.h"
#include "dsucsdnti.h"
#include "rex.h"


/* Watermark levels */
#define GCSD_TPS_SIO_LO_WM_LEVEL MAX_BUFFERED_USER_BYTES
#define GCSD_TPS_SIO_HI_WM_LEVEL (2*MAX_BUFFERED_USER_BYTES)

/* This level needs to be AT LEAST 1000 bytes greater than the
** high watermark level because of the size of data buffers
** that are received over USB
*/
#define GCSD_TPS_SIO_DONT_EXCEED_LEVEL (GCSD_TPS_SIO_HI_WM_LEVEL+1500)

/* Macro for reporting data traffic interval to avoid excessive updates */
#define GCSD_TRAFFIC_REPORTING_INTERVAL  50  


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Global variable data types
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Current call mode for multi mode calls (not currently supported) */
typedef enum
{
   MODE_1,
   MODE_2
} current_call_mode_T;


/*definition of synchronous user data from RA0 to RA1'*/
typedef uint32 sync_tx_data_T;

/*definition of synchronous user data from RA1' to RA0*/
typedef uint32 sync_rx_data_T;

/* Raw control bit info from network */
typedef struct
{
   uint8               X1_status_rx;
   uint8               X2_status_rx;
   uint8               S1_status_rx;
   uint8               S3_status_rx;
   uint8               S6_status_rx;
   uint8               S8_status_rx;
   uint8               S4_status_rx;
   uint8               S9_status_rx;
   uint8               E4_status_rx;
   uint8               E5_status_rx;
   uint8               E6_status_rx;
   uint8               E7_status_rx;
} rx_status_T;


/*types for data transfer*/

typedef struct
{
   uint8                             SA_status_tx;
   uint8                             SB_status_tx;
} tx_status_T;

/* Data rates for logging data */
typedef enum
{
   GCSD_RATE_NONE       = 0x00,
   GCSD_RATE_300        = 0x01,
   GCSD_RATE_600        = 0x02,
   GCSD_RATE_1200       = 0x03,
   GCSD_RATE_1200_300   = 0x04,
   GCSD_RATE_2400       = 0x05,
   GCSD_RATE_4800       = 0x06,
   GCSD_RATE_7200       = 0x07,
   GCSD_RATE_9600       = 0x08,
   GCSD_RATE_12000      = 0x09,
   GCSD_RATE_14400      = 0x0A
} data_rate_T;


/* Define macro for byte reflection table */
#define GCSD_REFLECT_BYTE(x) ds_Hardware_access_byte_reflection_table(x)


/****************************************************************/
/* Function prototypes for the GCSD Protocol stack subsystem    */
/****************************************************************/
void ds_gcsd_UpdateTimers     (void);


void
ds_gcsd_transparent_data_services_initialise
(
  cm_call_id_type                    call_id,
  const cm_bearer_capability_s_type *bearer_cap_ptr,
  ds_ucsd_data_rate_T                data_rate,
  sys_sys_mode_e_type                curr_mode,
  ds_ucsd_set_data_counters_cb_T     set_data_counters_cb      
);


void 
ds_gcsd_control_transparent_data_services 
(
   DSI_Event_T               Event,
   uint8                     cm_call_id,
   boolean                   process_uplink,
   boolean                   process_downlink
);



void
ds_gcsd_send_event_to_DS 
(
  ds_ucsd_ind_T    indication,
  cm_call_id_type  call_id
);



word 
ds_calculate_words_per_block
(
   byte  user_rate,
   word  character_length
);

void
ds_gcsd_init_RA0
(
  const cm_bearer_capability_s_type *bearer_cap_ptr,
  ds_ucsd_data_rate_T                data_rate 
);

uint32 
ds_gcsd_RA0_rx_data
(
   uint32 *rx_data
);

uint32 
ds_gcsd_RA0_tx_data
(
   uint32 *rx_data
);

boolean
ds_gcsd_tx_transparent_data_read_from_serial_port (void);


void
ds_gcsd_RA1d_adaption_tx
(
   uint32        *sync_tx_data,
   tx_status_T   tx_status,
   boolean       e_send_sync_pattern,
   uint8         *radio_tx_data
);

void ds_gcsd_init_RA1d_adaption_tx (void);
void ds_gcsd_init_RA1d_adaption_rx (void);

boolean 
ds_gcsd_RA1d_adaption_rx
(
   boolean         e_receive_sync_pattern,
   sync_rx_data_T  *sync_rx_data,
   rx_status_T     *rx_status_bits,
   boolean         *rx_error
);


void 
ds_gcsd_V24_reset(void);


void
ds_gcsd_suspend_data_to_sio 
(
   boolean suspend
);


void ds_gcsd_set_cts_state  
(
   boolean on_off
);

void ds_gcsd_CheckModemControlLines (void);

/* Functions that should be called when high/low watermarks are hit. */
void ds_gcsd_sio_high_wm_handler
#ifdef FEATURE_DSM_WM_CB
(
   dsm_watermark_type  *wm_ptr, 
   void                *user_data_ptr
);
#else
(
   void
);
#endif

void ds_gcsd_sio_low_wm_handler 
#ifdef FEATURE_DSM_WM_CB
(
   dsm_watermark_type  *wm_ptr, 
   void                *user_data_ptr
);
#else
(
   void
);
#endif


/* Checks to see if OTA RX buffer is full */
boolean ds_gcsd_ota_rx_buffer_full(void);

/* TODO: temporary */

boolean
ds_gcsd_get_tx_buffer 
(
   uint8 **radio_tx_data
);

byte 
ds_gcsd_read_from_channel_decoder
(
   uint8 **radio_rx_data
);

byte 
ds_gcsd_write_to_channel_coder
(
   uint8 *radio_tx_data
);


void ds_gcsd_l1_init (void);


boolean 
ds_gcsd_read_from_serial_port
(
   uint8                   *data
);

boolean 
ds_gcsd_write_to_serial_port
(
   uint8                   output_data,
   boolean                 output_data_valid, 
   boolean                 flush_data
);

void 
ds_gcsd_setup_online_modem_control (void);


boolean
ds_gcsd_update_serial_status 
(
   ds_ucsd_sio_status_T *status
);

/* FAX functions */
void ds_gcsd_fax_process_event
(
   ds_gcsd_cmd_enum_type cmd
);

void 
ds_gcsd_report_fax_connect
(
  cm_call_id_type   call_id
);


/*===========================================================================

FUNCTIONS get_gcsd_tcb_ptr

DESCRIPTION:
    This callback function is called to get gcsd task pointer.      

DEPENDENCIES
  None

RETURN VALUE
  Returns pointer to gcsd tcb (rex_tcb_type*)

SIDE EFFECTS
  None

===========================================================================*/
rex_tcb_type* get_gcsd_tcb_ptr( void );


/*===========================================================================

FUNCTION DS_GCSD_SIO_INIT

DESCRIPTION
  This function initializes the task critical section for V.24 modem 
  control. It is called once during powerup.

DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
void ds_gcsd_sio_init( void );

/*===========================================================================

FUNCTION DS_GCSD_INIT_CALL_DATA

DESCRIPTION
  This function initializes all data used within the GCSD task on a 
  per call basis.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Sets GCSD global data

===========================================================================*/
extern void  
ds_gcsd_init_call_data
(
   cm_call_id_type                   call_id,
   boolean                           call_is_MO,
   sys_sys_mode_e_type               curr_mode,
   ds_ucsd_data_rate_T               data_rate,
   ds_ucsd_frame_size_T              bytes_per_sdu,                           
   uint32                            sdus_per_tti,
   const cm_bearer_capability_s_type *bearer_cap_ptr,
   const ds_ucsd_nt_rlp_parameters_T *rlp_params_ptr,
   ds_ucsd_set_data_counters_cb_T    set_data_counters_cb      
);

/*===========================================================================

FUNCTION DS_GCSD_INIT_S30_TIMER_VALUE

DESCRIPTION
  This function initializes the S30 timer value store that is global to GCSD.

DEPENDENCIES
  S30 value is provided by ATCOP.

RETURN VALUE
  None

SIDE EFFECTS
  Sets GCSD global data

===========================================================================*/

extern void ds_gcsd_init_s30_timer_value(void);


/*===========================================================================

FUNCTION DS_GCSD_READ_S30_TIMER_VALUE

DESCRIPTION
  This function reads S30 timer value from a store that is global to GCSD.

DEPENDENCIES
  None.

RETURN VALUE
  S30 register value.

SIDE EFFECTS
  None

===========================================================================*/

extern uint8 ds_gcsd_read_s30_timer_value(void);


/*===========================================================================

FUNCTION DS_GCSD_S30_TIMER_CONTROL

DESCRIPTION
  This function handles S30 data inactivity timer start, stop, and expiry. 

  Start Timer: if zero UL or DL user data byte count is detected.
  Stop Timer : if non-zero UL or DL user data byte count is detected.
  On timer Expiry : Terminate call.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  CSD call is terminated on data inactivity timer expiry.

===========================================================================*/

extern void ds_gcsd_s30_timer_control(uint32 data_byte_count);

/*===========================================================================

FUNCTION DS_GCSD_SET_CURR_RX_DSM_LENGTH

DESCRIPTION
   This function sets the Curr_rx_dsm_length.
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
void 
ds_gcsd_set_Curr_rx_dsm_length
(
  word curr_rx_dsm_lnth
);

/*===========================================================================

FUNCTION DS_GCSD_SET_SIO_HIGH_WM_STATE

DESCRIPTION
   This function returns the SIO_high_WM_state.
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
void 
ds_gcsd_set_SIO_high_WM_state
(
  boolean sio_high_WM_state
);

/*===========================================================================

FUNCTION DS_GCSD_SET_OTA_RX_BUFFER_CNT

DESCRIPTION
   This function returns the OTA_rx_buffer_cnt.
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
void 
ds_gcsd_set_OTA_rx_buffer_cnt
(
  uint32 ota_rx_buffer_cnt
);

/*===========================================================================

FUNCTION DS_GCSD_SET_SUSPEND_DATA_TO_SIO

DESCRIPTION
   This function returns the Suspend_data_to_sio.
         
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void 
ds_gcsd_set_Suspend_data_to_sio
(
  boolean suspend_data_to_sio
);

/*===========================================================================

FUNCTION DS_GCSD_GET_CURR_RX_DSM_LENGTH

DESCRIPTION
   This function returns the Curr_rx_dsm_length.
         
DEPENDENCIES
  None
  
RETURN VALUE
  word - value of Curr_rx_dsm_length
    
SIDE EFFECTS
  None

===========================================================================*/

word
ds_gcsd_get_Curr_rx_dsm_length(void);

/*===========================================================================

FUNCTION DS_GCSD_CLEAN_CURR_DSM_ITEM

DESCRIPTION
   This function cleans Curr_rx_dsm_item and Curr_tx_dsm_item.
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/

void ds_gcsd_clean_Curr_dsm_item(void);

/*===========================================================================

FUNCTION DS_GCSD_GET_SIO_HIGH_WM_STATE

DESCRIPTION
   This function returns the SIO_high_WM_state.
         
DEPENDENCIES
  None
  
RETURN VALUE
  booelan - value of SIO_high_WM_state
    
SIDE EFFECTS
  None

===========================================================================*/
boolean 
ds_gcsd_get_SIO_high_WM_state(void);

/*===========================================================================

FUNCTION DS_GCSD_GET_SUSPEND_DATA_TO_SIO

DESCRIPTION
   This function returns the Suspend_data_to_sio.
         
DEPENDENCIES
  None
  
RETURN VALUE
  booelan - value of Suspend_data_to_sio
    
SIDE EFFECTS
  None

===========================================================================*/
boolean 
ds_gcsd_get_Suspend_data_to_sio(void);

/*===========================================================================

FUNCTION DS_GCSD_SET_FILTERED_RX_STATUS

DESCRIPTION
  This function turns the value of the global variable
  Filtered_rx_status.

DEPENDENCIES
  None

RETURN VALUE
  filtered_rx_status_T* - Pointer of Filtered_rx_status variable

SIDE EFFECTS
  None

===========================================================================*/
filtered_rx_status_T* 
ds_gcsd_get_Filtered_rx_status(void);

#endif /* DSGCSDI_H */
