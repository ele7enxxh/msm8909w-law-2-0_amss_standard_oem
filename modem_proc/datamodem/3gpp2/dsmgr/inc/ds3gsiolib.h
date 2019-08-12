#ifndef DS3GSIOLIB_H
#define DS3GSIOLIB_H
/*===========================================================================

               3 G   D A T A   S E R V I C E S   M A N A G E R

                S I O   I N T E R F A C E   F U N C T I O N S

                           H E A D E R    F I L E

DESCRIPTION
  This file defines services provided by SIOLIB to other Data Services
  software units, to perform serial port operations on the port assigned to
  Data Services. This includes setting and/or checking the state of various
  serial port control signals, configuring and performing flow control,
  configuring the serial baud rate and setting up data transmission.

  Note that Data Services software units other than SIOLIB should NOT use
  the SIO API directly, but should use the functions provided here instead.
  Serial port operations not defined here, such as opening or closing a port,
  are performed only by SIOLIB.

  Copyright (c) 2001 - 2016 by Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds3gsiolib.h_v   1.4   19 Dec 2002 17:36:26   ajithp  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/inc/ds3gsiolib.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/18/16   skc     Fixed issue related to cd assert for DUN call.
08/03/16   skc     Adding support for ESC SEQUENCE for PS Data call.
07/26/16   skc     Fixed issue with ATO command.
06/27/14   tk      Added support for Dynamic ATCoP.
02/13/12   sb      Sending URC's only when DTR for USB_CDC, A2_DUN ports is HIGH.
02/08/12   sk      Feature cleanup.
01/02/12   msh     Coan: Feature cleanup
05/26/11   dvk     Global Variables Cleanup
02/11/11   vs      DUN changes for CSFB Fusion
08/22/10   vk      Use a single signal for all forwarding ports
06/25/10   vs      Added Forwarding Port manager and other misc changes 
                   for Fusion.
06/03/10   sn      Added FEATURE_BT_MODEM featurization for BT specific 
                   functionality. This feature will be enabled in MODEM proc 
                   on multi-proc targets.
11/16/09   ca      Added support for the MD5 message entry mode.
09/04/09   ss      CMI SU level modifications.
06/17/09   sn      Included "dstask.h".
04/20/09   bs/sa   Added support for Modem Bridge Architecture.
03/04/09   sa      AU level CMI modifications.
07/16/08   ua      Added support for external client support.
                   Optimized received  SIO signals usage. This is feature wrapped in
                   FEATURE_DSAT_EXT_CLIENT_SUPPORT.
05/05/08   sa      Added ds3g_siolib_get_at_state() function.
10/26/07   ua      Added support for AT&S command. 
10/8/07    ar      Added DS3G_SIOLIB_SMD_DATA11_PORT.
08/09/07   ar      Fix behavior for mix of auto and manual answer scenarios.
04/12/07   ua      Added function ds3g_siolib_get_answer_port.
02/16/07   ar      Replace ds3g_siolib_incoming_call with per-call dir indication.
02/14/07   vd      Lint error fixes.
01/03/07   sq/vp   Add ds3g_siolib_incoming_cs_call_reset function
11/28/06   ar      Add support for explicit modem port selection.
07/23/06   ar      Add ds3g_siolib_current_ring_type export
06/13/06   sl      Add port_id to ds3g_siolib_set_flow_ctl_method() interface
05/10/06   vp      Declaration of ds3g_siolib_get_modem_port()
05/04/06   vp      Featurized ds3g_siolib_update_sio_conn_status under 
                   FEATURE_BT
05/01/06   vp      Changes to enable multiple data ports (shared memory data 
                   ports) for multiple processor builds.
01/19/06   sl      Serializer merged with SIOLIB.
09/20/04   ar      Add port allocation/release semaphore functionality.
12/19/02   atp     Removed deprecated functions inbound_flow_enable/disable()
05/08/02   rsl     Added siolib functionality.  
02/19/02   rlp     Code review changes.
10/04/01   smp     Added ds3g_siolib_is_rts_asserted() function.
06/07/01   smp     Initial version.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"


#include "sio.h"
#include "dsm.h"
#include "ds_flow_control.h"
#include "dsat_v.h"
#include "rdevmap.h"
#include "dstask.h"
#include "dstask_v.h"
#include "dsmgr_api.h"
#include "dsatclient.h"

#if defined(FEATURE_BT) || defined(FEATURE_BT_MODEM)
#include "bt.h"
#endif /* FEATURE_BT || FEATURE_BT_MODEM */

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Convenience macros for message logging
---------------------------------------------------------------------------*/

#define DS_SIO_MSG3_LOW(fmtstr, arg1, arg2, arg3) \
          DATA_MSG3(MSG_LEGACY_LOW, fmtstr, arg1, arg2, arg3)

#define DS_SIO_MSG3_MED(fmtstr, arg1, arg2, arg3) \
          DATA_MSG3(MSG_LEGACY_MED, fmtstr, arg1, arg2, arg3)

#define DS_SIO_MSG3_HIGH(fmtstr, arg1, arg2, arg3) \
          DATA_MSG3(MSG_LEGACY_HIGH, fmtstr, arg1, arg2, arg3)

#define DS_SIO_MSG3_ERROR(fmtstr, arg1, arg2, arg3) \
          DATA_MSG3(MSG_LEGACY_ERROR, fmtstr, arg1, arg2, arg3)


#define DS_SIO_MSG2_LOW(fmtstr, arg1, arg2) \
          DATA_MSG2(MSG_LEGACY_LOW, fmtstr, arg1, arg2)

#define DS_SIO_MSG2_MED(fmtstr, arg1, arg2) \
          DATA_MSG2(MSG_LEGACY_MED, fmtstr, arg1, arg2)

#define DS_SIO_MSG2_HIGH(fmtstr, arg1, arg2) \
          DATA_MSG2(MSG_LEGACY_HIGH, fmtstr, arg1, arg2)

#define DS_SIO_MSG2_ERROR(fmtstr, arg1, arg2) \
          DATA_MSG2(MSG_LEGACY_ERROR, fmtstr, arg1, arg2)


#define DS_SIO_MSG1_LOW(fmtstr, arg1) \
          DATA_MSG1(MSG_LEGACY_LOW, fmtstr, arg1)

#define DS_SIO_MSG1_MED(fmtstr, arg1) \
          DATA_MSG1(MSG_LEGACY_MED, fmtstr, arg1)

#define DS_SIO_MSG1_HIGH(fmtstr, arg1) \
          DATA_MSG1(MSG_LEGACY_HIGH, fmtstr, arg1)

#define DS_SIO_MSG1_ERROR(fmtstr, arg1) \
          DATA_MSG1(MSG_LEGACY_ERROR, fmtstr, arg1)


#define DS_SIO_MSG0_LOW(fmtstr) \
          DATA_MSG0(MSG_LEGACY_LOW, fmtstr)

#define DS_SIO_MSG0_MED(fmtstr) \
          DATA_MSG0(MSG_LEGACY_MED, fmtstr)

#define DS_SIO_MSG0_HIGH(fmtstr) \
          DATA_MSG0(MSG_LEGACY_HIGH, fmtstr)

#define DS_SIO_MSG0_ERROR(fmtstr) \
          DATA_MSG0(MSG_LEGACY_ERROR, fmtstr)

/*---------------------------------------------------------------------------
  DS3GSIOLIB error codes
---------------------------------------------------------------------------*/
typedef enum
{
  DS3G_SIOLIB_ERR_NONE               = -1,
  DS3G_SIOLIB_ERR_INVALID_DTR_EVENT  =  0,
  DS3G_SIOLIB_ERR_INVALID_DTR_STATUS =  1,
  DS3G_SIOLIB_ERR_DTR_SIG_CNTR_MAX   =  2,
  DS3G_SIOLIB_ERR_DTR_SIG_CNTR_MIN   =  3
} ds3g_siolib_err_code_e_type;

/*---------------------------------------------------------------------------
  Serial port status returned by the functions exported in this file.
---------------------------------------------------------------------------*/  
typedef enum
{
  DS3G_SIOLIB_OK,                            /* Serial port is currently open */
  DS3G_SIOLIB_NO_PORT_OPEN                   /* No serial port currently open */
} ds3g_siolib_status_e_type;

/*---------------------------------------------------------------------------
  Events that impact the Carrier Detect (CD) state.
---------------------------------------------------------------------------*/
typedef enum
{
  DS3G_SIOLIB_REMOTE_CARRIER_ON,                      /* Remote carrier is ON */
  DS3G_SIOLIB_REMOTE_CARRIER_OFF,                    /* Remote carrier is OFF */
  DS3G_SIOLIB_CD_WINKING_END,                   /* End of CD winking interval */
  DS3G_SIOLIB_SMS_MSG_START,                    /* Start of SMS message entry */
  DS3G_SIOLIB_SMS_MSG_END,                        /* End of SMS message entry */
  DS3G_SIOLIB_MD5_MSG_START,                    /* Start of MD5 message entry */
  DS3G_SIOLIB_MD5_MSG_END,                        /* End of MD5 message entry */
  DS3G_SIOLIB_EVENT_NONE                                          /* No event */
} ds3g_siolib_dcd_event_e_type;

/*---------------------------------------------------------------------------
  Serial Port States. Note that any non-null state implies that a
  serial port is assigned to Data Services.
---------------------------------------------------------------------------*/ 
typedef enum
{
  DS3G_SIOLIB_NULL_SERIAL_STATE,      /* No serial port assigned to data services */
  DS3G_SIOLIB_AUTODETECT_SERIAL_STATE,          /* Serial port in Autodetect Mode */
  DS3G_SIOLIB_RAWDATA_SERIAL_STATE,                /* Serial port in Rawdata Mode */
  DS3G_SIOLIB_PACKET_SERIAL_STATE,                  /* Serial port in Packet Mode */
  DS3G_SIOLIB_WAIT_TX_FLUSH_SERIAL_STATE,      /* Wait for TX buffer to be flushed */
  DS3G_SIOLIB_NO_WAIT_TX_FLUSH_SERIAL_STATE      /* No Wait for TX buffer to be flushed */
} ds3g_siolib_serial_state_e_type;


/*---------------------------------------------------------------------------
  SIOLIB AT Command Processing States
---------------------------------------------------------------------------*/ 
typedef enum
{
  DS3G_SIOLIB_NULL_AT_STATE,     /* ATCoP not active i.e. no serial port assigned */
  DS3G_SIOLIB_COMMAND_AT_STATE,    /* No data call active, listen for AT commands */
  DS3G_SIOLIB_ONLINE_DATA_AT_STATE,/* Data call active, rx'ed bytes are user data */
  DS3G_SIOLIB_ONLINE_CMD_AT_STATE     /* Data call active, listen for AT commands */
} ds3g_siolib_at_state_e_type;

/*---------------------------------------------------------------------------
  Serial port identifiers for semaphore allocation/release
---------------------------------------------------------------------------*/  
typedef enum
{
  DS3G_SIOLIB_PORTID_NONE  = -1,              /* Serial port not allocated */
  DS3G_SIOLIB_PORTID_0,                                  /* Serial port #0 */
  DS3G_SIOLIB_PORTID_1,
  DS3G_SIOLIB_PORTID_MAX                              /* Internal use only */
} ds3g_siolib_portid_e_type;

/*---------------------------------------------------------------------------
  Type for SIOLIB serial port related information
---------------------------------------------------------------------------*/
typedef struct
{
  ds3g_siolib_serial_state_e_type    serial_state;
  sio_stream_id_type           stream_id;   /* SIO stream id */
  sio_port_id_type             port_id;     /* SIO port id */
  dsm_watermark_type           rx_wm_ptr;   /* Autodetect SIO RX watermark */
  q_type                       rx_wm_q;     /* Autodetect SIO RX Queue */ 
  dsm_watermark_type           tx_wm_ptr;   /* Autodetect SIO TX watermark */
  q_type                       tx_wm_q;     /* Autodetect SIO TX Queue */ 
  boolean                      ri_asserted; /* Ring Indicator (RI) state */
  boolean                      rx_sig;      /* SIO Rx SIG received */
  dword                        rx_pending_bit;  /* Pending RX data bit */
  dword                        rx_sig_val;  /* Signal value, used for sig muxing */
  boolean                      close_in_progress; /* sio_close() in progress */
  uint32                       flow_ctl_mask;  /* Keeps track of flow ctl state */
} ds3g_siolib_serial_info_type;

/*---------------------------------------------------------------------------
  SIOLIB PORTS for SERIALIZER 
---------------------------------------------------------------------------*/ 
typedef enum
{
  DS3G_SIOLIB_PORT_NONE  = -1,
  DS3G_SIOLIB_DATA_PORT,
  DS3G_SIOLIB_UART1_PORT,
  DS3G_SIOLIB_USB_CDC_PORT,
  DS3G_SIOLIB_BT_NA_PORT,
  DS3G_SIOLIB_IRCOMM_PORT,
  DS3G_SIOLIB_BT_SPP_PORT,
#if defined(FEATURE_MULTIPROCESSOR) && defined(FEATURE_SMD)
  DS3G_SIOLIB_SMD_DS_PORT,
  DS3G_SIOLIB_SMD_DATA1_PORT,
  DS3G_SIOLIB_SMD_DATA2_PORT,
  DS3G_SIOLIB_SMD_DATA3_PORT,
  DS3G_SIOLIB_SMD_DATA4_PORT,
  DS3G_SIOLIB_SMD_DATA11_PORT,
#endif/* defined(FEATURE_MULTIPROCESSOR) && defined(FEATURE_SMD) */
#ifdef FEATURE_DSAT_EXT_CLIENT_SUPPORT
  DS3G_SIOLIB_CLIENT_VSP_PORT,
#endif /* FEATURE_DSAT_EXT_CLIENT_SUPPORT */
#if defined(FEATURE_DSAT_MDM_FUSION) || defined(FEATURE_DSAT_MSM_FUSION)
  DS3G_SIOLIB_HSUART1_PORT,
  DS3G_SIOLIB_HSUART2_PORT,
  DS3G_SIOLIB_HSUART3_PORT,
#endif

#if defined(FEATURE_DATA_A2) && !defined(FEATURE_DATA_SDIO_MUX_A2_DUN)
  DS3G_SIOLIB_A2_MDM_PORT,
#endif /* FEATURE_DATA_A2 && !FEATURE_DATA_SDIO_MUX_A2_DUN*/
#ifdef FEATURE_DATA_SDIO_MUX_A2_DUN
  DS3G_SIOLIB_SDIO_MUX_A2_DUN_0_PORT,
#endif /* FEATURE_DATA_SDIO_MUX_A2_DUN */
#ifdef FEATURE_DATA_MUX  
  DS3G_SIOLIB_DATA_MUX_1_PORT,
  DS3G_SIOLIB_DATA_MUX_2_PORT,
  DS3G_SIOLIB_DATA_MUX_3_PORT,
#endif /* FEATURE_DATA_MUX */

  DS3G_SIOLIB_PORT_MAX
}ds3g_siolib_port_e_type;

/*---------------------------------------------------------------------------
  SIOLIB PORT BIT for SERIALIZER
---------------------------------------------------------------------------*/ 

#define DS3G_SIOLIB_PORT_UART1_BIT      0x0001
#define DS3G_SIOLIB_PORT_USB_CDC_BIT    0x0002
#define DS3G_SIOLIB_PORT_BT_NA_BIT      0x0004
#define DS3G_SIOLIB_PORT_IRCOMM_BIT     0x0008
#define DS3G_SIOLIB_PORT_BT_SPP_BIT     0x0010
#if defined(FEATURE_MULTIPROCESSOR) && defined(FEATURE_SMD)
#define DS3G_SIOLIB_PORT_SMD_DS_BIT     0x0020
#define DS3G_SIOLIB_PORT_SMD_DATA1_BIT  0x0040
#define DS3G_SIOLIB_PORT_SMD_DATA2_BIT  0x0080
#define DS3G_SIOLIB_PORT_SMD_DATA3_BIT  0x0100
#define DS3G_SIOLIB_PORT_SMD_DATA4_BIT  0x0200
#define DS3G_SIOLIB_PORT_SMD_DATA11_BIT 0x0400
#endif /* defined(FEATURE_MULTIPROCESSOR) && defined(FEATURE_SMD) */
#ifdef FEATURE_DSAT_EXT_CLIENT_SUPPORT
  #define DS3G_SIOLIB_PORT_CLIENT_VSP_BIT 0x0800
#endif /* FEATURE_DSAT_EXT_CLIENT_SUPPORT */
#if defined(FEATURE_DSAT_MDM_FUSION) || defined(FEATURE_DSAT_MSM_FUSION)
#define DS3G_SIOLIB_PORT_HSUART1_BIT    0x1000
#define DS3G_SIOLIB_PORT_HSUART2_BIT    0x2000
#define DS3G_SIOLIB_PORT_HSUART3_BIT    0x4000
#endif
#if defined(FEATURE_DATA_SDIO_MUX_A2_DUN)
  #define DS3G_SIOLIB_PORT_SDIO_MUX_A2_DUN_0_BIT 0x8000
#elif defined(FEATURE_DATA_A2)
#define DS3G_SIOLIB_PORT_A2_MDM_BIT     0x8000
#endif 
#ifdef FEATURE_DATA_MUX
#define DS3G_SIOLIB_PORT_MUX_DATA1_BIT  0x10000
#define DS3G_SIOLIB_PORT_MUX_DATA2_BIT  0x20000
#define DS3G_SIOLIB_PORT_MUX_DATA3_BIT  0x40000
#endif /* FEATURE_DATA_MUX */
/* DSR Management for &S Command */
#define DS3G_DSR_ALWAYS_ON 0
#define DS3G_DSR_CONNECT_ON 1

/*---------------------------------------------------------------------------
  The time interval in ms for which DCD is "winked" (i.e DCD is de-asserted)
  at the end of the call, if &C=2.
---------------------------------------------------------------------------*/
#define DS3G_SIOLIB_DCD_WINK_INTERVAL    1000

#if defined(FEATURE_HS_USB)
#define DS3G_SIOLIB_USB_BUFFER_SIZE  0x1000
#endif
/*---------------------------------------------------------------------------
  SIOLIB Modem Port Allocation.
---------------------------------------------------------------------------*/ 
typedef enum ds3g_siolib_port_alloc_mode
{
  DS3G_SIOLIB_PORT_ALLOC_MODE_AUTOMATIC, /* Active port assigns modem port */
  DS3G_SIOLIB_PORT_ALLOC_MODE_MANUAL,    /* Assign modem port as specified */
  DS3G_SIOLIB_PORT_ALLOC_MODE_MAX        /* Internal use only              */
} ds3g_siolib_port_alloc_mode_e_type;

typedef enum ds3g_siolib_port_call_dir
{
  DS3G_SIOLIB_PORT_CALL_DIR_ORIGINATED,  /* Mobile originated call */
  DS3G_SIOLIB_PORT_CALL_DIR_TERMINATED,  /* Mobile terminated call */
  DS3G_SIOLIB_PORT_CALL_DIR_MAX          /* Internal use only      */
} ds3g_siolib_port_call_dir_e_type;

typedef struct ds3g_siolib_port_alloc_info
{
  ds3g_siolib_port_alloc_mode_e_type  mode;
  ds3g_siolib_port_e_type             manual_port;
  ds3g_siolib_port_call_dir_e_type    call_dir;
} ds3g_siolib_port_alloc_info_s_type;

/*---------------------------------------------------------------------------
  SIOLIB PORT CLIENT TYPE for SERIALIZER
---------------------------------------------------------------------------*/ 
typedef enum
{
    DS3G_SIOLIB_CLIENT_NONE = -1,
    DS3G_SIOLIB_CLIENT_DS,             /* data from PS/CS protocol stack */
    DS3G_SIOLIB_CLIENT_ATCOP,           /* data from ATCOP                */
    DS3G_SIOLIB_CLIENT_MAX
}ds3g_siolib_client_e_type;

/*---------------------------------------------------------------------------
  SIOLIB PORT TX TYPE for SERIALIZER
---------------------------------------------------------------------------*/ 
typedef enum
{
    DS3G_SIOLIB_UNSOLICTED_NONE = 0,
    DS3G_SIOLIB_UNSOLICTED_AT_ERROR_CODE,  /* ERROR code                  */
    DS3G_SIOLIB_UNSOLICTED_DS_RESULT_CODE, /* such as NO CARRIER, CONNECT */
    DS3G_SIOLIB_UNSOLICTED_SMS,            /* SMS message                 */
    DS3G_SIOLIB_UNSOLICTED_CM,             /* Supplementary Services Data */
    DS3G_SIOLIB_UNSOLICTED_ACC,            /* Accessories Data            */
    DS3G_SIOLIB_UNSOLICTED_FAX_RESULT_CODE /* Fax result codes            */
}ds3g_siolib_tx_e_type;

/*---------------------------------------------------------------------------
  SIOLIB AT Call Answer Mode States
---------------------------------------------------------------------------*/ 
typedef enum
{
  DS3G_SIOLIB_ANSWER_MODE_NULL   = 0x0,/* Internal value                   */
  DS3G_SIOLIB_ANSWER_MODE_AUTO   = 0x1,/* Set for call auto-answer         */
  DS3G_SIOLIB_ANSWER_MODE_MANUAL = 0x2,/* Set for manual answer of call    */
  DS3G_SIOLIB_ANSWER_PORT_MAX    = 0x8 /* Internal value                   */
} ds3g_siolib_answer_mode_e_type;

#ifdef FEATURE_DSAT_MDM_FUSION
/*---------------------------------------------------------------------------
  Enumeration of the different states of forwarding port
---------------------------------------------------------------------------*/ 
typedef enum 
{
  DS3G_SIOLIB_FORWARDING_PORT_INVALID_STATE = -1,
  DS3G_SIOLIB_FORWARDING_PORT_AVAILABLE_STATE,
  DS3G_SIOLIB_FORWARDING_PORT_AT_STATE,
  DS3G_SIOLIB_FORWARDING_PORT_PKT_STATE
} ds3g_siolib_forwarding_port_state_type;

#endif /*FEATURE_DSAT_MDM_FUSION*/

/*---------------------------------------------------------------------------
  Each mode-specific handler maintains such a table, and registers this
  table with SIOLIB.
---------------------------------------------------------------------------*/
typedef struct
{
/*-------------------------------------------------------------------------
    This handler is invoked when the AT escape sequence is detected, in
    online data mode. The return value indicates whether online command mode
    should be entered (in this case, data transfer over the air should be
    suspended), or if we should stay in online data mode.
  -------------------------------------------------------------------------*/
  dsat_mode_enum_type (*at_escape_sequence_handler)( void );

  /*-------------------------------------------------------------------------
    This handler is invoked when the DTR signal gets asserted or de-asserted.
  -------------------------------------------------------------------------*/
  void (*dtr_changed_sig_handler)( boolean dtr_status );

  /*-------------------------------------------------------------------------
    This handler is invoked when the DTR signal gets asserted or de-asserted
        while AT&D = 0.
  -------------------------------------------------------------------------*/
  void (*dtr_changed_sig_handler_ampd0)( boolean dtr_status );

#ifdef FEATURE_DSAT_MDM_FUSION
  /*-------------------------------------------------------------------------
    This handler is invoked when the CD signal gets asserted or de-asserted
  -------------------------------------------------------------------------*/
  void (*cd_changed_sig_handler)( boolean cd_status );  
#endif /* FEATURE_DSAT_MDM_FUSION */

  void (*at_return_to_online_data_handler)(
                         dsm_watermark_type     **from_sio_wm_ptr,
                         dsm_watermark_type     **to_sio_wm_ptr,
                         sio_rx_func_ptr_type    *sio_rx_func_ptr);

  /*-------------------------------------------------------------------------
    This handler is invoked when the ATO command (return to online data mode)
    is rx'ed in online command mode. The mode-specific handler should resume
    data transfer over the air.
  -------------------------------------------------------------------------*/
  void (*at_return_to_online_data_complete_handler)( void );

  /*-------------------------------------------------------------------------
    This handler is invoked when the DS_ENTER_ONLINE_CMD_* mode transition
    completes (i.e. SIO has been placed in online-command mode).  The mode
    specific handler can be used to notify the initiating task that the
    mode transition has completed.  The handler is only called when exiting
    online-data mode and returning to online-command mode.
  -------------------------------------------------------------------------*/
  void (*return_to_online_cmd_mode_complete_handler)( void );

  /*-------------------------------------------------------------------------
    This handler will be called when auto-answering is to be performed based
    on the value of ATS0.
  -------------------------------------------------------------------------*/
  void (*auto_answer_handler)( void );

  dsat_mode_enum_type (*at_escape_sequence_ex_handler)( void *cb_data );

  void (*dtr_changed_sig_ex_handler)( boolean dtr_status, void *cb_data );

  void (*dtr_changed_sig_ex_handler_ampd0)( boolean dtr_status, void *cb_data );

} ds3g_siolib_func_tbl_type;

/*---------------------------------------------------------------------------
  Indicates the port on which we received an ATS0
---------------------------------------------------------------------------*/
typedef struct 
{
  uint8                           mode;
  ds3g_siolib_port_e_type         port;
  ds3g_siolib_port_e_type         manual_port;
  ds3g_siolib_func_tbl_type      *func_tbl_ptr;
}ds3g_siolib_answer_info_type;

/* Macros to set,clear & test answer_info.mode bitfield */
#define SET_ANSWERMODE(answermode) \
        ds3g_siolib_answer_info.mode |= (uint8)answermode;
#define CLEAR_ANSWERMODE(answermode) \
        ds3g_siolib_answer_info.mode &= ~(uint8)answermode;
#define IS_ANSWERMODE(answermode) \
        (0 != (ds3g_siolib_answer_info.mode & (uint8)answermode))

extern ds3g_siolib_answer_info_type ds3g_siolib_answer_info;


/*---------------------------------------------------------------------------
  Type for SIOLIB flow control status between RM watermark and USB
---------------------------------------------------------------------------*/
typedef enum
{
  DS3G_SIOLIB_FLOW_STATUS_NONE,
  DS3G_SIOLIB_FLOW_ENABLED,
  DS3G_SIOLIB_FLOW_DISABLED
} ds3g_siolib_flow_control_status_type;

/*---------------------------------------------------------------------------
  SIOLIB state type.
---------------------------------------------------------------------------*/ 

typedef struct
{
  ds3g_siolib_serial_info_type  serial_info;   /* Serial port related info */
  ds3g_siolib_at_state_e_type   at_state;      /* AT command processing state */
  ds3g_siolib_func_tbl_type     *func_tbl;
  boolean                       call_coming_up;
  dsat_result_enum_type         ring_type;     /* Ring result code to send */
  uint8                         ring_counter;  /* Counts the number of rings */
  ds3g_siolib_flow_control_status_type   flow_status; /* If enabled then set to TRUE */
  boolean                       is_dtr_high;
#ifdef FEATURE_DSAT_MDM_FUSION
  ds3g_siolib_forwarding_port_state_type forwarding_port_state;
#endif /* FEATURE_DSAT_MDM_FUSION */
  rex_crit_sect_type            port_info_crit_sec;
  boolean                       dtr_handling_pending_flag; 
                                               /* Flag that indicates whether
                                                  or not DTR handling is
                                                  pending. */

} ds3g_siolib_state_info_type;

/*---------------------------------------------------------------------------
  Structure for DS_CMD_SIO_DTR_CHANGED
---------------------------------------------------------------------------*/
typedef struct
{
  int port_id;            /* Port on which the DTR has changed */
} ds3g_siolib_dtr_chg_cmd_type;

/*---------------------------------------------------------------------------
  Structure for DS_CMD_SIO_CD_CHANGED
---------------------------------------------------------------------------*/
#ifdef FEATURE_DSAT_MDM_FUSION
typedef struct
{
  int port_id;            /* Port on which the CD has changed */
} ds3g_siolib_cd_chg_cmd_type;
#endif

/*---------------------------------------------------------------------------
  Structure for DS_CMD_SIO_DSM_MEM_CTRL
---------------------------------------------------------------------------*/
typedef struct
{
  dsm_mempool_id_enum_type  pool_id;
  dsm_mem_level_enum_type   event;
  dsm_mem_op_enum_type      op;
} ds3g_siolib_dsm_mem_ctrl_type;

extern ds_call_type_e_type current_ds_call_type;

/*---------------------------------------------------------------------------
  SIO port allocation semaphore flags
---------------------------------------------------------------------------*/
extern ds3g_siolib_port_e_type ds3g_siolib_port_allocated[DS3G_SIOLIB_PORTID_MAX];

/*---------------------------------------------------------------------------
  SIO port allocation semaphore flags
---------------------------------------------------------------------------*/
extern ds3g_siolib_port_e_type ds3g_siolib_last_port_allocated[DS3G_SIOLIB_PORTID_MAX];

/*---------------------------------------------------------------------------
  SIO port Signal Status
---------------------------------------------------------------------------*/

typedef struct
{ 
  boolean			      remote_carrier_state;
  boolean			      dcd_winking;
  boolean			      sms_entry;
  boolean			      CD_asserted_already;
}ds3g_siolib_port_signal_status_type;

extern ds3g_siolib_port_signal_status_type ds3g_siolib_port_signal_status[DS3G_SIOLIB_PORTID_MAX + 1];

/* DTR Signal Counter Limit */
#define DS3G_SIOLIB_DTR_SIG_CNTR_MAX (DS3G_SIOLIB_PORT_MAX - DS3G_SIOLIB_DATA_PORT)

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION DS3G_SIOLIB_INIT
   
DESCRIPTION
  This function performs power-up initialization of SIOLIB.  This includes
  initialization of some state variables and watermark pointers.
  
  This function must be called once at data services task startup.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

rex_sigs_type ds3g_siolib_init ( void );

/*===========================================================================

FUNCTION DS3G_SIOLIB_OPEN

DESCRIPTION
  This function opens the specified serial port. Received data bytes will be
  stored in the RX watermark specified by the caller. Data bytes pending
  transmission will be stored in the TX watermark specified by the caller.

  The port is opened in Autodetect mode, and the baud rate is set according
  to the +IPR setting. Callback functions for DTR state changes and packet
  detection are registered with SIO. RS-232 control signals are initialized
  as follows:
    CD  - Initialized per the &C setting
    CTS - Asserted
    RI  - De-asserted
    DSR - Asserted

DEPENDENCIES
  None

RETURN VALUE
  Returns DS3G_SIOLIB_OK if the open was successful, or DS3G_SIOLIB_NO_PORT_OPEN
  if the open was unsuccessful.

SIDE EFFECTS
  None

===========================================================================*/

extern ds3g_siolib_status_e_type  ds3g_siolib_open
(
  sio_port_id_type      port_id,       /* Port id of the port to be opened */
  dsm_watermark_type   *rx_wm_ptr,       /* Watermark for rx data from SIO */
  dsm_watermark_type   *tx_wm_ptr          /* Watermark for tx data to SIO */
);


/*===========================================================================

FUNCTION DS3G_SIOLIB_CLOSE

DESCRIPTION
  This function closes the currently open serial port.

DEPENDENCIES
  No action is taken if there is no port currently open.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void  ds3g_siolib_close
(
  ds3g_siolib_port_e_type port_id
);
/*===========================================================================

FUNCTION DS3G_SIOLIB_IOCTL

DESCRIPTION
  This function allows control of open stream.

DEPENDENCIES
  No action is taken if there is no port currently open.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void  ds3g_siolib_ioctl
( 
  sio_ioctl_cmd_type cmd,
  sio_ioctl_param_type *param 
);

/*===========================================================================

FUNCTION DS3G_SIOLIB_IS_BAUD_VALID                             EXTERNALIZED FUNCTION

DESCRIPTION
  This routine checks if the specified baud rate is valid under the
  current feature set.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the baud rate is valid under the current feature set.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/

extern boolean ds3g_siolib_is_baud_valid
(
  sio_bitrate_type   check_baud              /* SIO baud rate */
);

/*===========================================================================

FUNCTION DS3G_SIOLIB_CHANGE_MODE

DESCRIPTION
  This function changes the mode of the currently open serial port to the
  specified mode. After the mode change, received data bytes will be stored
  in the RX watermark specified by the caller, and the rx_func will be called
  each time data is enqueued to the watermark. One of these (rx_wm_ptr or
  rx_func_ptr) may be NULL. Also, data bytes pending transmission will be
  stored in the TX watermark specified by the caller.

  Depending on the new serial mode, callback functions are registered for
  serial events of interest, namely:
    Autodetect Mode - Packet detection callback
    Rawdata Mode    - Escape signal detection callback
    Packet Mode     - None

DEPENDENCIES
  No action is taken if there is no port currently open.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

extern ds3g_siolib_status_e_type  ds3g_siolib_change_mode
(
  sio_stream_mode_type  stream_mode,                    /* New stream mode */
  dsm_watermark_type   *rx_wm_ptr,       /* Watermark for rx data from SIO */
  dsm_watermark_type   *tx_wm_ptr,         /* Watermark for tx data to SIO */
  sio_rx_func_ptr_type  rx_func_ptr   /* Called each time data bytes rx'ed */
);


/*===========================================================================

FUNCTION DS3G_SIOLIB_CD_ASSERT

DESCRIPTION
  Asserts the Carrier Detect (CD) signal.

DEPENDENCIES
  No action is taken if there is no port currently open.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

extern ds3g_siolib_status_e_type  ds3g_siolib_cd_assert
( 
  ds3g_siolib_port_e_type port_id
);


/*===========================================================================

FUNCTION DS3G_SIOLIB_CD_DEASSERT

DESCRIPTION
  De-asserts the Carrier Detect (CD) signal.

DEPENDENCIES
  No action is taken if there is no port currently open.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

extern ds3g_siolib_status_e_type  ds3g_siolib_cd_deassert
( 
  ds3g_siolib_port_e_type port_id
);


/*===========================================================================

FUNCTION DS3G_SIOLIB_RI_ASSERT

DESCRIPTION
  Asserts the Ring Indicator (RI) signal.

DEPENDENCIES
  No action is taken if there is no port currently open.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

extern ds3g_siolib_status_e_type  ds3g_siolib_ri_assert
( 
  ds3g_siolib_port_e_type port_id
);


/*===========================================================================

FUNCTION DS3G_SIOLIB_RI_DEASSERT

DESCRIPTION
  De-asserts the Ring Indicator (RI) signal.

DEPENDENCIES
  No action is taken if there is no port currently open.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

extern ds3g_siolib_status_e_type  ds3g_siolib_ri_deassert
( 
  ds3g_siolib_port_e_type port_id
);

/*===========================================================================
FUNCTION DS3G_SIOLIB_SET_CALL_COMING_UP

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/

extern void  ds3g_siolib_set_call_coming_up
(
    boolean call_coming_up
);

/*===========================================================================
FUNCTION DS3G_SIOLIB_REGISTER_CALLBACK_FUNC

DESCRIPTION
  This function registers a mode specific call back function table. 

  Caller should check the return value of this function to see if the
  registration succeeded.
  
DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if a func_tbl is not registered with SIOLIB already and
  if the passed in function pointer is not NULL, otherwise returns FALSE;

SIDE EFFECTS
  If a mode-specific handler function table was previously registered, 
  it will be over-written.
===========================================================================*/

extern boolean  ds3g_siolib_register_callback_func
(
  ds3g_siolib_func_tbl_type  *func_tbl_ptr        /* Handler function table */
);

/*===========================================================================
FUNCTION DS3G_SIOLIB_DEREGISTER_CALLBACK_FUNC

DESCRIPTION
  This function deregisters a mode-specific handler function table.

DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if deregister happened successfully. 
  Returns FALSE if func ptr passed in does not match siolib table value.

SIDE EFFECTS
  Resets the callback function table pointer to null. 
===========================================================================*/

extern boolean ds3g_siolib_deregister_callback_func 
( 
  ds3g_siolib_func_tbl_type *func_tbl_ptr
);



/*===========================================================================

FUNCTION DS3G_SIOLIB_DSR_ASSERT

DESCRIPTION
  Asserts the Data Set Ready (DSR) signal.

DEPENDENCIES
  No action is taken if there is no port currently open.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

extern ds3g_siolib_status_e_type  ds3g_siolib_dsr_assert
( 
  ds3g_siolib_port_e_type port_id
);


/*===========================================================================

FUNCTION DS3G_SIOLIB_DSR_DEASSERT

DESCRIPTION
  De-asserts the Data Set Ready (DSR) signal.

DEPENDENCIES
  No action is taken if there is no port currently open.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

extern ds3g_siolib_status_e_type  ds3g_siolib_dsr_deassert
( 
  ds3g_siolib_port_e_type port_id
);


/*===========================================================================

FUNCTION DS3G_SIOLIB_IS_DTR_ASSERTED

DESCRIPTION
  Checks the state of the Data Terminal Ready (DTR) signal, and returns a
  value (by means of a passed-by-reference parameter) indicating whether or
  not DTR is asserted.

DEPENDENCIES
  If there is no port currently open, the dtr_status value has no meaning.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

extern ds3g_siolib_status_e_type  ds3g_siolib_is_dtr_asserted
(
  boolean  *dtr_status     /* Set to TRUE if DTR asserted, or FALSE if not */
);


/*===========================================================================

FUNCTION DS3G_SIOLIB_IS_RTS_ASSERTED

DESCRIPTION
  Checks the state of the Request To Send (RTS) signal, and returns a
  value (by means of a passed-by-reference parameter) indicating whether or
  not RTS is asserted.

DEPENDENCIES
  If there is no port currently open, the rts_status value has no meaning.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

extern ds3g_siolib_status_e_type  ds3g_siolib_is_rts_asserted
(
  boolean  *rts_status     /* Set to TRUE if RTS asserted, or FALSE if not */
);


/*===========================================================================

FUNCTION DS3G_SIOLIB_SET_FLOW_CTL_METHOD

DESCRIPTION
  Configures the flow control method for the currently open serial port.

DEPENDENCIES
  No action is taken if there is no port currently open.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

extern ds3g_siolib_status_e_type  ds3g_siolib_set_flow_ctl_method
(
  ds3g_siolib_port_e_type  port_id,              /* SIOLIB port id */
  sio_flow_ctl_type  tx_flow,                    /* TX flow control method */
  sio_flow_ctl_type  rx_flow                     /* RX flow control method */
);


/*===========================================================================

FUNCTION DS3G_SIOLIB_SETUP_TX

DESCRIPTION
  This function queues data for transmission over the serial interface.
  Additionally, Autodetect Mode is re-armed, if specified by the caller.
  Typically, the re-arm feature is used only by ATCoP.

DEPENDENCIES
  No action is taken if there is no port currently open.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

extern ds3g_siolib_status_e_type  ds3g_siolib_setup_tx
(
  dsm_item_type  *tx_ptr,         /* DSM item containing data to transmit */
  boolean         rearm_autodetect       /* TRUE = Re-arm Autodetect Mode */
);



/*===========================================================================

FUNCTION DS3G_SIOLIB_CHANGE_BAUD_RATE

DESCRIPTION
  Sets the baud rate of the currently open serial port to the specified
  value.  Change may be scheduled or immediate based on caller.

DEPENDENCIES
  No action is taken if there is no port currently open.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

extern ds3g_siolib_status_e_type ds3g_siolib_change_baud_rate
(
  sio_bitrate_type  baud_rate,             /* New baud rate                */
  boolean           chg_now                /* Flag for immediate change    */  
);



/*===========================================================================

FUNCTION DS3G_SIOLIB_SET_CD_STATE

DESCRIPTION
  This function sets the state of the Carrier Detect (CD) signal, based on
  the following:
  1) &C setting
       &C0 - CD is always ON
       &C1 - CD is ON when the remote carrier is ON
       &C2 - CD is always ON except for 1sec 'wink' when a data call is
             disconnected
  2) Remote carrier state (only if &C1)
  3) End-of-call winking (only if &C2)
  4) SMS message entry
       CD is ON during SMS message entry (overrides the &C setting)

  This function should be called when a serial port is opened, and whenever
  any event occurs that impacts the CD state. Use the NO_EVENT event when
  a serial port is opened, or when the &C parameter is changed.

DEPENDENCIES
  If there is no port currently open, the state of the event passed in (if
  any) will be remembered, but the CD state will not be changed.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

extern ds3g_siolib_status_e_type  ds3g_siolib_set_cd_state
(
  ds3g_siolib_dcd_event_e_type  dcd_event              /* Event that occurred */
);



/*===========================================================================

FUNCTION DS3G_SIOLIB_REARM_AUTODETECT

DESCRIPTION
  This function re-arms SIO Autodetect Mode. Typically, the re-arm
  feature is used only by ATCoP.

DEPENDENCIES
  No action is taken if there is no port currently open.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

extern ds3g_siolib_status_e_type  ds3g_siolib_rearm_autodetect
( 
  ds3g_siolib_port_e_type port
);



/*===========================================================================

FUNCTION DS3G_SIOLIB_FLUSH_TX

DESCRIPTION
  This function is used to schedule the SIO TX queue to be flushed when
  changed to and from online-command and online-data modes.

  The flush_func_ptr_cb parameter is used to pass a callback function to
  SIO that is called when the flush operation has completed.  The callback
  should be used to notify the task which requested the flush operation
  that the TX queue is now empty.

DEPENDENCIES
  No action is taken if there is no port currently open.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

extern ds3g_siolib_status_e_type  ds3g_siolib_flush_tx
( 
  void (*flush_func_ptr_cb)(void)
);



/*===========================================================================

FUNCTION DS3G_SIOLIB_RDM_INIT

DESCRIPTION
  This function performs registration with RDM to use port mapping 
  services.  It registers open and close serial device functions with
  RDM.  It also initializes SIOLIB state information and sets up
  autodetect watermark items, both of which are required by the open
  and close serial device functions.

  This function must be called once at data services task startup.

DEPENDENCIES
  Must be called before TMC function to start task.

RETURN VALUE
  None

SIDE EFFECTS
  Initializes state variables required by RDM serial device functions.

===========================================================================*/

extern void ds3g_siolib_rdm_init( void );



/*===========================================================================

FUNCTION DS3G_SIOLIB_PROCESS_SIGNALS

DESCRIPTION
  This function processes SIOLIB signals. The appropriate processing is
  performed based on the signal(s) that are set in the signal mask.

  This function should be called by the top-level task dispatcher whenever
  any of the SIOLIB signals are set.

DEPENDENCIES
  SIOLIB should have been initialized prior to calling this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void  ds3g_siolib_process_signals
(
  rex_sigs_type  set_sigs         /* Mask containing signals that were set */
);



/*===========================================================================

FUNCTION DS3G_SIOLIB_PROCESS_CMDS

DESCRIPTION
  This function processes SIOLIB commands. A command processing function is
  called based on the type of command received.

  This function should be called by the top-level task dispatcher whenever
  any SIOLIB command is received.

DEPENDENCIES
  SIOLIB should have been initialized prior to calling this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void  ds3g_siolib_process_cmds
(
  ds_cmd_type  *cmd_ptr                         /* Command to be processed */
);



/*===========================================================================

FUNCTION DS3G_SIOLIB_CLEANUP_AUTODETECT_RX_WATERMARK

DESCRIPTION
  This function cleans up the Autodetect watermark used for RX data from SIO.
  Any data stored in the watermark is discarded.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void  ds3g_siolib_cleanup_autodetect_rx_watermark( void );


/*===========================================================================

FUNCTION DS3GI_CONVERT_SERIAL_MODE

DESCRIPTION
  This function converts the specified SIO serial mode into the corresponding
  SIOLIB serial state.

DEPENDENCIES
  None

RETURN VALUE
  Returns the SIOLIB serial state corresponding to the specified SIO serial
  mode.

SIDE EFFECTS
  None

===========================================================================*/

extern ds3g_siolib_serial_state_e_type  ds3gi_convert_serial_mode
(
  sio_stream_mode_type  sio_serial_mode                 /* SIO serial mode */
);

/*===========================================================================

FUNCTION DS3G_SIOLIB_ALERT_USER

DESCRIPTION
  This function rings the serial port to alert the user/TE of an incoming
  data or voice call. The RI signal is toggled, and the RING result code is
  sent to the TE. Auto-answer is also performed, if requested by the caller
  (and if enabled by the S0 s-register).

  This function should be called to start alerting, and subsequently, each
  time the ring timer expires. This function may be invoked either by 3G
  Dsmgr (for data calls) or ATCoP (for voice calls).

  Note that the auto-answer facility is intended to be used for data calls
  only.

DEPENDENCIES
  This function does nothing if there is no serial port currently open.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void  ds3g_siolib_alert_user
(
  dsat_result_enum_type  ring_type,     /* Ring result code to send to TE */
  boolean                auto_answer       /* TRUE => Perform auto-answer */
);



/*===========================================================================

FUNCTION DS3G_SIOLIB_STOP_ALERTING

DESCRIPTION
  This function stops alerting the user i.e. stops ringing the serial port.
  The RI signal is de-asserted and the ring timer is stopped.

  This function should be called when the user answers the call or if the
  call is released in the RINGING call state.

DEPENDENCIES
  This function does nothing if there is no serial port currently open.

RETURN VALUE
  None

SIDE EFFECTS
  Updates 3G Dsmgr state variables (resets the ring_counter to zero).

===========================================================================*/

extern void  ds3g_siolib_stop_alerting( void );



/*===========================================================================

FUNCTION DS3G_SIOLIB_SIO_CLOSE_CB

DESCRIPTION
  This function sends the SIO_CLOSE_COMPLETE_CMD to DS.

DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

extern void  ds3g_siolib_sio_close_cb( void );



/*===========================================================================

FUNCTION DS3G_SIOLIB_ALLOCATE_PORT

DESCRIPTION
  This function attempts to allocate an SIO port for an external data
  call.  It implements a simple semaphore flag for the port resource
  to ensure only one data call per SIO port.  A port must be
  unallocated and in AUTODETECT state for it to be available.  On
  successful allocation, a valid port identifier is returned.
  Otherwise, the NONE identifier is returned.  It is the calling
  routines responsibility to honor the resource semaphore.

  The current assumption is that only one port can host an external call
  at any given time.  This port is referred to as the modem port.
                
DEPENDENCIES
  None

RETURN VALUE
  DS3G_SIOLIB_PORTID_NONE on error condition, valid port identifier otherwise.

SIDE EFFECTS
  None

===========================================================================*/

extern ds3g_siolib_portid_e_type ds3g_siolib_allocate_port
(
  ds3g_siolib_port_alloc_info_s_type  *alloc_info_ptr
);



/*===========================================================================

FUNCTION DS3G_SIOLIB_RELEASE_PORT

DESCRIPTION
  This function releases a previously allocated SIO port, clearing the
  semaphore flag.  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void ds3g_siolib_release_port
(
  ds3g_siolib_portid_e_type port_index
);



/*===========================================================================

FUNCTION DS3G_SIOLIB_SET_CLIENT_TYPE

DESCRIPTION
  This function sets up the client_type for response routing.
  Mainly used when on ONLINE DATA state. While on ONLINE DATA state,
  distinction has to be made on where to route the data. If the data
  is from ATCOP, it cannot be routed to MODEM port. If any virtual port
  is active, it should be routed to that port. Active port will receive
  the data while on ONLINE COMMAND mode and COMMAND mode.

DEPENDENCIES
  ds3g_siolib_setup_tx() will use client_type and resets the state
  variable.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

extern void ds3g_siolib_set_client_type
(
  ds3g_siolib_client_e_type
);

/*===========================================================================

FUNCTION DS3G_SIOLIB_GET_CLIENT_TYPE

DESCRIPTION
  This function gets the client_type for response routing.
  Mainly used when on ONLINE DATA state. While on ONLINE DATA state,
  distinction has to be made on where to route the data. If the data
  is from ATCOP, it cannot be routed to MODEM port. If any virtual port
  is active, it should be routed to that port. Active port will receive
  the data while on ONLINE COMMAND mode and COMMAND mode.

DEPENDENCIES
  ds3g_siolib_setup_tx() will use client_type and resets the state
  variable.

RETURN VALUE
  Client Type

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_client_e_type ds3g_siolib_get_client_type
(
  void
);

#ifdef FEATURE_DSAT_EXT_CLIENT_SUPPORT
/*===========================================================================

FUNCTION DS3G_SIOLIB_SET_ACTIVE_EXT_CLIENT

DESCRIPTION
  Sets the current active external client

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void ds3g_siolib_set_active_ext_client 
(  
  dsat_ext_client_id_type client_id    /* External client id */
);
#endif /* FEATURE_DSAT_EXT_CLIENT_SUPPORT */

/*===========================================================================

FUNCTION DS3G_SIOLIB_SET_TX_TYPE

DESCRIPTION
  This function sets up the tx_type for unsolicited result code.
  the tx_type is used in ds3g_siolib_setup_tx() while doing tx to SIO.
  We use tx_type to process unsolicited data such as RSSI, SMS...etc.


DEPENDENCIES
  ds3g_siolib_setup_tx() will use tx_type to distribute data.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

extern void ds3g_siolib_set_tx_type
(
  ds3g_siolib_tx_e_type     tx_type,      /* unsolicited result code type */
  ds3g_siolib_port_e_type   desired_port  /* port type                    */
);


#ifdef FEATURE_DSAT_MDM_FUSION
/*===========================================================================

FUNCTION DS3G_SIOLIB_GET_PROXY_PORT

DESCRIPTION
  Returns the current proxy port

DEPENDENCIES
  None

RETURN VALUE
  DS3G_SIOLIB_ACTIVE_PORT

SIDE EFFECTS
  None

===========================================================================*/

extern ds3g_siolib_port_e_type ds3g_siolib_get_proxy_port ( void );

/*===========================================================================

FUNCTION DS3G_SIOLIB_SET_PROXY_PORT

DESCRIPTION
  Sets the current proxy port

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None

===========================================================================*/

extern void ds3g_siolib_set_proxy_port ( ds3g_siolib_port_e_type port_id );

#endif /* FEATURE_DSAT_MDM_FUSION */

#ifdef FEATURE_DSAT_EXT_CLIENT_SUPPORT
/*===========================================================================

FUNCTION DS3G_SIOLIB_GET_ACTIVE_EXT_CLIENT

DESCRIPTION
  Returns the current active client

DEPENDENCIES
  None

RETURN VALUE
  DS3G_SIOLIB_ACTIVE_PORT

SIDE EFFECTS
  None

===========================================================================*/

extern dsat_ext_client_id_type ds3g_siolib_get_active_ext_client ( void );
#endif /* FEATURE_DSAT_EXT_CLIENT_SUPPORT */

/*===========================================================================

FUNCTION DS3G_SIOLIB_GET_AT_STATE

DESCRIPTION
  Returns the current AT command state of the input port..

DEPENDENCIES
  None

RETURN VALUE
  ds3g_siolib_at_state_e_type 

SIDE EFFECTS
  None

===========================================================================*/

extern ds3g_siolib_at_state_e_type ds3g_siolib_get_at_state 
(
  ds3g_siolib_port_e_type port      /* port_id */
);

/*===========================================================================

FUNCTION DS3G_SIOLIB_SET_ANSWER_PORT

DESCRIPTION
  Assigns the active port depending on the answer mode.  For auto-
  answer mode, the active port is preserved in answer_port.  This 
  is used to assign the modem_port when auto-answer is invoked. For 
  manual answer mode, the active port is assigned directly to the 
  modem_port.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void ds3g_siolib_set_answer_port
(
  ds3g_siolib_answer_mode_e_type answer_mode  /* Call answer mode */
);



/*===========================================================================

FUNCTION DS3G_SIOLIB_GET_ANSWER_PORT

DESCRIPTION
  Returns the cached port for call answer when auto-answer enabled.

DEPENDENCIES
  None

RETURN VALUE
  Valid port when auto-answer enabled, DS3G_SIOLIB_PORT_NONE otherwise.

SIDE EFFECTS
  None

===========================================================================*/

extern ds3g_siolib_port_e_type ds3g_siolib_get_answer_port( void );



/*===========================================================================

FUNCTION DS3G_SIOLIB_SET_ANSWER_CALLBACKS

DESCRIPTION
  Assigns the callback function table for auto answer functionality.
  Parameter may be NULL to clear the registration.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void ds3g_siolib_set_answer_callbacks
(
  ds3g_siolib_func_tbl_type  *func_tbl_ptr               /* Callback table */
);



/*===========================================================================

FUNCTION DS3G_SIOLIB_SETUP_AT_TX

DESCRIPTION
  This function queues AT command response for transmission over the 
  serial interface.
  
  Data will be distributed to the active port, if tx_type is not equal to 
  DS3G_SIOLIB_UNSOLICTED_NONE. 

DEPENDENCIES
  For unsolicited result codes, tx_type should be set first. 

  
RETURN VALUE
  Status enum. 
  
SIDE EFFECTS
  tx_type will be reset to DS3G_SIOLIB_UNSOLICTED_NONE after this function call.

  We will check rearm_autodetect to verify whether this AT command has been 
  finished processing. If yes, then we will call ds3g_siolib_service_ports() to 
  process another port.

===========================================================================*/

extern ds3g_siolib_status_e_type  ds3g_siolib_setup_at_tx
(
  dsm_item_type  *tx_ptr,           /* DSM item containing data to transmit */
  boolean         rearm_autodetect  /* TRUE = Re-arm Autodetect Mode        */
);



/*===========================================================================

FUNCTION DS3G_SIOLIB_MAP_SIO_PORT

DESCRIPTION
  This port reads the SIO port passed to it and returns the corresponding
  DSMGR port (of type ds3g_siolib_port_e_type). 

DEPENDENCIES
  None
  
RETURN VALUE
  DSMGR port of type ds3g_siolib_port_e_type
  
SIDE EFFECTS
  None

===========================================================================*/

extern ds3g_siolib_port_e_type ds3g_siolib_map_sio_port
(
  sio_port_id_type port_id
);



/*===========================================================================

FUNCTION DS3G_SIOLIB_MAP_RDM_PORT

DESCRIPTION
  This port reads the RDM port passed to it and returns the corresponding
  DSMGR port (of type ds3g_siolib_port_e_type). 

DEPENDENCIES
  None
  
RETURN VALUE
  DSMGR port of type ds3g_siolib_port_e_type
  
SIDE EFFECTS
  None

===========================================================================*/

extern ds3g_siolib_port_e_type ds3g_siolib_map_rdm_port
(
  rdm_device_enum_type port_id
);



/*===========================================================================

FUNCTION DS3G_SIOLIB_MAP_DS_PORT

DESCRIPTION
  This port reads the DSMGR port passed to it and returns the corresponding
  SIO port (of type sio_port_id_type). 

DEPENDENCIES
  None
  
RETURN VALUE
  SIO port of type sio_port_id_type
  
SIDE EFFECTS
  None
  
===========================================================================*/

extern sio_port_id_type ds3g_siolib_map_ds_port
( 
  ds3g_siolib_port_e_type port_id
);


/*===========================================================================

FUNCTION DS3G_SIOLIB_GET_SIO_PORT

DESCRIPTION
  This routine determines the SIO port from the specified modem port.

DEPENDENCIES
  None
  
RETURN VALUE
  SIO port of type sio_port_id_type
  
SIDE EFFECTS
  None
  
===========================================================================*/

extern sio_port_id_type ds3g_siolib_get_sio_port
( 
  ds3g_siolib_port_e_type modem_port_id
);


/*===========================================================================

FUNCTION DS3G_SIOLIB_CONVERT_DS_TO_RDM_PORT

DESCRIPTION
  This routine determines the RDM port from the specified DS modem port.

DEPENDENCIES
  None
  
RETURN VALUE
  RDM port of type rdm_device_enum_type
  
SIDE EFFECTS
  None
  
===========================================================================*/
extern rdm_device_enum_type ds3g_siolib_convert_ds_to_rdm_port
( 
  ds3g_siolib_port_e_type port_id
);

#if defined(FEATURE_BT) || defined(FEATURE_BT_MODEM)
/*===========================================================================

FUNCTION DS3G_SIOLIB_UPDATE_SIO_CONN_STATUS

DESCRIPTION
  Callback function handling SPP status change for BT.  Called by BT driver.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void ds3g_siolib_update_sio_conn_status
( 
  bt_spp_status_type* spp_status_ptr 
);
#endif /* FEATURE_BT || FEATURE_BT_MODEM */


/*===========================================================================

FUNCTION DS3G_SIOLIB_BLOCK_TASK

DESCRIPTION
  Suspends DS task until one of the relevant signals are set which call
  ds3g_siolib_release.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void ds3g_siolib_block_task (void);



/*===========================================================================

FUNCTION DS3G_SIOLIB_RELEASE_TASK

DESCRIPTION
  Unblocks the DS task.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void ds3g_siolib_release_task (void);



/*===========================================================================

FUNCTION DS3G_SIOLIB_INIT_CD_STATE

DESCRIPTION
  This function inits the state of the Carrier Detect (CD) signal, based on
  the following:
  1) &C setting
     &C0, &C2 - CD is always ON
     &C1 - CD is OFF

  This function should be called when a serial port is opened.

DEPENDENCIES
  None

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

extern ds3g_siolib_status_e_type ds3g_siolib_init_cd_state
(
  ds3g_siolib_port_e_type port_id
);


/*===========================================================================
FUNCTION DS3G_SIOLIB_SET_PORT_INBOUND_FLOW

DESCRIPTION
  This function controls the enabling or diabling of inbound flow ctl on a 
  specified port.
  This function was required because inbound flow ctl. can be changed
  from several places for different reasons, and we need an arbitrator
  function to handle those changes.

  Basically, the function enables inbound flow, when no caller wants it
  disabled. If even one caller wants the flow disabled, the inbound flow
  is disabled.


DEPENDENCIES
  This is the only function that should be used to toggle the inbound flow.
  This is important because the function keeps track of the state of inbound
  flow, and if the state of inbound flow is changed


RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/

extern void ds3g_siolib_set_port_inbound_flow
( 
  ds3g_siolib_port_e_type  port_id,   /* DS3G_SIOLIB port id                */
  uint32 in_mask,          /* input mask of calling function               */
  ds3g_flow_e_type flow  /* specifies if flow is to be enabled or disabled */
);


/*===========================================================================

FUNCTION DS3G_SIOLIB_SERVICE_PORTS

DESCRIPTION
  This function checks the ds3g_port_waiting_mask to see which port we
  will process next. We use Round Robin Algorithm here. The first port to be 
  processed is the one next to the port just processed. This ensures fairness
  by not allowing a single port to hog the ATCOP engine.
        ^                       |
        |                       |
        <-------------------- <-

DEPENDENCIES
  It depends on current active port which is ds3g_active_port. 

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void ds3g_siolib_service_ports(void);


/*===========================================================================

FUNCTION DS3G_SIOLIB_IS_EXTERNAL_PORT_FREE

DESCRIPTION
  Reports state of external modem port availablilty.  Should be called by
  mode-specific handler before alerting TE for terminating calls.
  
DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if modem port is unassigned, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

extern boolean ds3g_siolib_is_external_port_free( void );



/*===========================================================================

FUNCTION DS3G_SIOLIB_HANDLE_UNSOLICITED_RESULT

DESCRIPTION
  This function process the tx_type for unsolicited result code and sends the
  response to the specified port. In the event the specified port is 
  DS3G_SIOLIB_DATA_PORT, the response is broadcasted to all serial ports
  currently open.

DEPENDENCIES
  It depends on ds3g_siolib_tx_type set by ds3g_siolib_set_tx_type().

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void ds3g_siolib_handle_unsolicited_result
(
  ds3g_siolib_status_e_type *ret_val, /* SIO return value */
  dsm_item_type  *tx_ptr,        /* dsm item ptr     */
  boolean       rearm_autodetect,/* Done with AT command line?  */ 
  ds3g_siolib_tx_e_type tx_type,  /* unsolicited result code type*/
  ds3g_siolib_port_e_type tx_port
);


/*===========================================================================

FUNCTION DS3G_SIOLIB_SEND_DATA_TO_PORT

DESCRIPTION
  This function does tx based on active port.
  Also, check whether there is left over at rx watermark. If true, then we 
  turn on the bit on ds3g_port_waiting_mask.

DEPENDENCIES
  Called from ds3g_siolib_setup_tx or ds3g_siolib_handle_unsolicited_result.

RETURN VALUE
  None.

SIDE EFFECTS
  Data will be discarded if the active port is not defined. 

===========================================================================*/

extern void ds3g_siolib_send_data_to_port
(
  ds3g_siolib_status_e_type *ret_val,
  dsm_item_type  *tx_ptr,        /* DSM item ptr                  */
  boolean       rearm_autodetect,/* Done with one AT Command Line?*/
  ds3g_siolib_port_e_type port   /* Current active port           */
);


/*===========================================================================

FUNCTION DS3G_SIOLIB_CHANGE_SIO_PARAMS

DESCRIPTION
  Sets the baud rate and flow control of the specified SIO port.
  Change of baud rate may be scheduled or immediate based on caller.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void ds3g_siolib_change_sio_params
( 
  sio_port_id_type port_id,                      /* SIO port_id            */
  sio_flow_ctl_type tx_flow,                    /* TX flow control method */
  sio_flow_ctl_type rx_flow,                    /* RX flow control method */
  sio_bitrate_type new_baud_rate,               /* Requested baud rate    */
  boolean change_baud_now                       /* Immediate or post ack  */
);

#ifdef FEATURE_DSAT_EXT_CLIENT_SUPPORT
/*===========================================================================

FUNCTION DS3G_SIOLIB_SERVICE_EXT_CLIENTS

DESCRIPTION
  This function is a wrapper function for the actual service clients
  which is dsatclient_service_ext_clients. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void ds3g_siolib_service_ext_clients(void);
#endif /* FEATURE_DSAT_EXT_CLIENT_SUPPORT */



/*===========================================================================

FUNCTION DS3G_SIOLIB_GET_INDEX_FROM_ALLOCATED_PORT

DESCRIPTION
  This functions return an index into the Allocated Port array

DEPENDENCIES
  None

PARAMETERS
  Port Id

RETURN VALUE
  Index into the Allocated port array.
  DS3G_SIOLIB_PORTID_NONE, if port if the not allocated.

SIDE EFFECTS
  None

===========================================================================*/
ds3g_siolib_portid_e_type ds3g_siolib_get_index_from_allocated_port
(
  ds3g_siolib_port_e_type port_id
);

/*===========================================================================

FUNCTION DS3G_SIOLIB_GET_INDEX_FROM_LAST_ALLOCATED_PORT

DESCRIPTION
  This functions return an index into the Last Allocated Port array.

DEPENDENCIES
  None

PARAMETERS
  Port Id

RETURN VALUE
  Index into the Last allocated port array.
  DS3G_SIOLIB_PORTID_NONE, if port is not found in
  the last allocated array.

SIDE EFFECTS
  None

===========================================================================*/
ds3g_siolib_portid_e_type ds3g_siolib_get_index_from_last_allocated_port
(
  ds3g_siolib_port_e_type port_id
);


/*===========================================================================

FUNCTION DS3G_SIOLIB_ESCAPE_DETECTED_CB

DESCRIPTION
  Callback function registered with SIO, used to notify data services when
  the escape code (+++) is detected (in Rawdata Mode).

  This function simply sets the DS_SIO_ESCAPE_SIG.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_siolib_escape_detected_cb( void );

/*===========================================================================

FUNCTION DS3G_SIOLIB_RESET_MODEM_PORT

DESCRIPTION
  Reset the current modem port to be DS3G_SIOLIB_PORT_NONE.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_siolib_reset_modem_port
( 
  void 
);

/*===========================================================================

FUNCTION DS3G_SIOLIB_GET_MODEM_PORT

DESCRIPTION
  Returns the current modem port

DEPENDENCIES
  None

RETURN VALUE
  DS3G_SIOLIB_MODEM_PORT

SIDE EFFECTS
  None

===========================================================================*/
ds3g_siolib_port_e_type ds3g_siolib_get_modem_port 
( 
  void 
);

/*===========================================================================

FUNCTION DS3G_SIOLIB_SET_MODEM_PORT

DESCRIPTION
  Sets the modem port to be the current active port or for an incoming call
  to the appropriate answer port.
  
DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_siolib_set_modem_port 
( 
  ds3g_siolib_port_e_type port_id
);

/*===========================================================================

FUNCTION DS3G_SIOLIB_SETUP_WATERMARKS

DESCRIPTION
  This function initializes the watermarks used for RX and TX data
  from/to SIO. The lowater, hiwater and don't exceed counts are configured,
  and the watermark count (amount of data stored in the watermark) is set to
  zero.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void ds3g_siolib_setup_watermarks
( 
  ds3g_siolib_port_e_type port_id 
);

/*===========================================================================

FUNCTION DS3G_SIOLIB_ESCAPE_DETECTED_CB

DESCRIPTION
  Callback function registered with SIO, used to notify data services when
  the escape code (+++) is detected (in Rawdata Mode).

  This function simply sets the DS_SIO_ESCAPE_SIG.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_siolib_escape_detected_cb
(
  void
);

#ifdef FEATURE_DSAT_MDM_FUSION
/*===========================================================================

FUNCTION DS3G_SIOLIB_HSUART_CD_CHANGED_CB

DESCRIPTION
  Callback function registered with HSUART, used to notify RmSm proxy when
  a change in the Carrier Detect (CD) signal occurs.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_siolib_hsuart_cd_changed_cb 
(
  void *user_data_ptr
);

/*===========================================================================
FUNCTION DS3G_SIOLIB_ALLOCATE_FORWARDING_PORT

DESCRIPTION This function accepts a physical port as parameter 
            (i.e. the port on which an AT command is received).
			It allocates and returns the forwarding port to the calling 
			function, if available. Initially the forwarding port state is
			DS3G_SIOLIB_FORWARDING_PORT_AVAILABLE_STATE. If the allocation 
			of the forwarding port is successful, the state is changed to 
			DS3G_SIOLIB_FORWARDING_PORT_AT_STATE.

DEPENDENCIES

RETURN VALUE
  Forwarding Port on SUCCESS
  DS3G_SIOLIB_PORT_NONE on FAILURE

SIDE EFFECTS
  An entry is made in the forwarding port table, which maps the physical port
  to the forwarding port.
===========================================================================*/
ds3g_siolib_port_e_type ds3g_siolib_allocate_forwarding_port
(
  ds3g_siolib_port_e_type phys_port
);

/*===========================================================================
FUNCTION DS3G_SIOLIB_GET_FORWARDING_PORT

DESCRIPTION This function accepts a physical port as parameter 
            (i.e. the port on which an AT command is received).
			It looks up the forwarding port table and returns the previously
			allocated forwarding port to the calling function, if available. 
DEPENDENCIES

RETURN VALUE
  Forwarding Port on SUCCESS
  DS3G_SIOLIB_PORT_NONE on FAILURE

SIDE EFFECTS

===========================================================================*/
ds3g_siolib_port_e_type ds3g_siolib_get_forwarding_port
(
  ds3g_siolib_port_e_type phys_port
);

/*===========================================================================
FUNCTION DS3G_SIOLIB_CHANGE_FORWARDING_PORT_STATE

DESCRIPTION  This function changes the state of the forwarding port to the 
             specifed value.
DEPENDENCIES

RETURN VALUE None

SIDE EFFECTS

===========================================================================*/
void ds3g_siolib_change_forwarding_port_state
(
  ds3g_siolib_port_e_type                forwarding_port,
  ds3g_siolib_forwarding_port_state_type state
);

/*===========================================================================
FUNCTION DS3G_SIOLIB_RELEASE_FORWARDING_PORT

DESCRIPTION This function accepts forwarding port as the parameter. 
            It releases the forwarding port and changes its state back to
			DS3G_SIOLIB_FORWARDING_PORT_AVAILABLE_STATE. 
            
DEPENDENCIES

RETURN VALUE None

SIDE EFFECTS
  The entry in forwarding port table which maps the physical port
  to the forwarding port is now removed.
===========================================================================*/
void ds3g_siolib_release_forwarding_port
(
  ds3g_siolib_port_e_type forwarding_port
);

/*===========================================================================
FUNCTION       DS3G_SIOLIB_SET_DS3G_SIOLIB_PROXY_PORT

DESCRIPTION    This function sets the ds3g_siolib_proxy_port

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None

===========================================================================*/
void ds3g_siolib_set_ds3g_siolib_proxy_port
(
  ds3g_siolib_port_e_type proxy_port
);

/*===========================================================================
FUNCTION       DS3G_SIOLIB_GET_DS3G_SIOLIB_PROXY_PORT

DESCRIPTION    This function returns the ds3g_siolib_proxy_port.

DEPENDENCIES   None.

RETURN VALUE   ds3g_siolib_port_e_type - proxy port

SIDE EFFECTS   None

===========================================================================*/
ds3g_siolib_port_e_type ds3g_siolib_get_ds3g_siolib_proxy_port 
(
  void
);

#endif /* FEATURE_DSAT_MDM_FUSION */

/*===========================================================================
FUNCTION       DS3G_SIOLIB_SET_DS3G_SIOLIB_ACTIVE_PORT

DESCRIPTION    This function sets the ds3g_siolib_active_port

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None

===========================================================================*/
void ds3g_siolib_set_ds3g_siolib_active_port
(
  ds3g_siolib_port_e_type active_port
);

/*===========================================================================
FUNCTION       DS3G_SIOLIB_GET_DS3G_SIOLIB_ACTIVE_PORT

DESCRIPTION    This function returns the ds3g_siolib_active_port.

DEPENDENCIES   None.

RETURN VALUE   ds3g_siolib_port_e_type - active port

SIDE EFFECTS   None

===========================================================================*/
ds3g_siolib_port_e_type ds3g_siolib_get_ds3g_siolib_active_port 
(
  void
);

/*===========================================================================
FUNCTION       DS3G_SIOLIB_SET_DS3G_SIOLIB_MODEM_PORT

DESCRIPTION    This function sets the ds3g_siolib_modem_port

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None

===========================================================================*/
void ds3g_siolib_set_ds3g_siolib_modem_port
(
  ds3g_siolib_port_e_type modem_port
);

/*===========================================================================
FUNCTION       DS3G_SIOLIB_GET_DS3G_SIOLIB_MODEM_PORT

DESCRIPTION    This function returns the current port in data call.

DEPENDENCIES   None.

RETURN VALUE   ds3g_siolib_port_e_type - current port

SIDE EFFECTS   None

===========================================================================*/
ds3g_siolib_port_e_type ds3g_siolib_get_ds3g_siolib_modem_port 
(
  void
);

/*===========================================================================
FUNCTION       DS3G_SIOLIB_SET_DS3G_SIOLIB_PENDING_CLOSE_PORT

DESCRIPTION    This function sets the port pending sio_close callback

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None

===========================================================================*/
void ds3g_siolib_set_ds3g_siolib_pending_close_port
(
  ds3g_siolib_port_e_type pending_close_port
);

/*===========================================================================
FUNCTION       DS3G_SIOLIB_GET_DS3G_SIOLIB_PENDING_CLOSE_PORT

DESCRIPTION    This function returns the port pending sio_close callback

DEPENDENCIES   None.

RETURN VALUE   ds3g_siolib_port_e_type - pending close port

SIDE EFFECTS   None

===========================================================================*/
ds3g_siolib_port_e_type ds3g_siolib_get_ds3g_siolib_pending_close_port 
(
  void
);

/*===========================================================================
FUNCTION       DS3G_SIOLIB_SET_DS3G_SIOLIB_CURRENT_RING_TYPE

DESCRIPTION    This function sets the current ring type

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None

===========================================================================*/
void ds3g_siolib_set_ds3g_siolib_current_ring_type
(
  dsat_result_enum_type current_ring_type
);

/*===========================================================================
FUNCTION       DS3G_SIOLIB_GET_DS3G_SIOLIB_CURRENT_RING_TYPE

DESCRIPTION    This function returns the current ring type

DEPENDENCIES   None.

RETURN VALUE   dsat_result_enum_type - current ring type

SIDE EFFECTS   None

===========================================================================*/
dsat_result_enum_type ds3g_siolib_get_ds3g_siolib_current_ring_type 
(
  void
);

/*===========================================================================
FUNCTION       DS3G_SIOLIB_GET_DS3G_SIOLIB_STATE

DESCRIPTION    This function returns the port state

DEPENDENCIES   None.

RETURN VALUE   ds3g_siolib_state_info_type* - port state info

SIDE EFFECTS   None

===========================================================================*/
ds3g_siolib_state_info_type* ds3g_siolib_get_ds3g_siolib_state 
(
  ds3g_siolib_port_e_type port_id
);
#ifdef FEATURE_DATA_MUX
/*===========================================================================
FUNCTION       DS3G_SIOLIB_IS_CLIENT_MUX_PORT

DESCRIPTION    This function checks if the input port is a client side MUX port.

DEPENDENCIES   None.

RETURN VALUE    
  TRUE if the input port is a client side MUX port.
  FALSE otherwise.

SIDE EFFECTS   None

===========================================================================*/

boolean ds3g_siolib_is_client_mux_port
(
  ds3g_siolib_port_e_type port_id
);
#endif /* FEATURE_DATA_MUX */
#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */


/*===========================================================================
FUNCTION DS3G_SIOLIB_MEM_EVENT_CTRL_EV_CB

DESCRIPTION
  This handler is registered with DSM for memory use events

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds3g_siolib_mem_ctrl_ev_cb
(
  dsm_mempool_id_enum_type pool_id,            /* memory pool ID */
  dsm_mem_level_enum_type  event,              /* type of memory event */
  dsm_mem_op_enum_type     op
);

/*===========================================================================

FUNCTION DS3G_SIOLIB_PORT_IS_IN_ACTIVE_CALL

DESCRIPTION
  This function checks whether the given port is in an active Data call.

DEPENDENCIES
  None

RETURN VALUE
  TRUE, if port is already being used for a Data call
  FALSE, otherwise

SIDE EFFECTS
  None

===========================================================================*/

boolean ds3g_siolib_port_is_in_active_call
(
  ds3g_siolib_port_e_type port_id
);

/*===========================================================================

FUNCTION DS3G_SIOLIB_GET_ACTIVE PORT

DESCRIPTION
  This functions return the Active port

DEPENDENCIES
  None

PARAMETERS
 None

RETURN VALUE
  Active port

SIDE EFFECTS
  None

===========================================================================*/
ds3g_siolib_port_e_type ds3g_siolib_get_active_port 
(
  void
);


/*===========================================================================

FUNCTION DS3G_SIOLIBI_RX_SIG_CRIT_SECT_ENTER

DESCRIPTION
  This functions enters ds3g_siolib_rx_sig_crit_sec critical section

DEPENDENCIES
  None

PARAMETERS
 None

RETURN VALUE
  Active port

SIDE EFFECTS
  None

===========================================================================*/

void ds3g_siolibi_rx_sig_crit_sect_enter(void);
/*===========================================================================

FUNCTION DS3G_SIOLIBI_RX_SIG_CRIT_SECT_LEAVE

DESCRIPTION
  This functions leaves ds3g_siolib_rx_sig_crit_sec critical section

DEPENDENCIES
  None

PARAMETERS
 None

RETURN VALUE
  Active port

SIDE EFFECTS
  None

===========================================================================*/

void ds3g_siolibi_rx_sig_crit_sect_leave(void);
#endif /* DS3GSIOLIB_H */
