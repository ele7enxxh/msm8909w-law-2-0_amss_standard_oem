#ifndef DSGCSD_TASK_V_H
#define DSGCSD_TASK_V_H
/*===========================================================================

              G C S D   S H A R E D   D E F I N I T I O N      
                             H E A D E R   F I L E

DESCRIPTION
  This header file contains definitions that are shared between GCSD
  call control and the GCSD protocol stack.

  
Copyright (c) 2001 - 2011 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/GSM/GCSD/vcs/dsgcsd.h_v   1.3   21 Mar 2002 10:54:04   trebman  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/inc/dsgcsd_task_v.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/11   TTV     Support for Q6 free floating.
01/06/11   TTV     Changes made to start GCSD general purpose timer after call 
                   origination.
03/04/09   SA      AU level CMI modifications.
03/25/08   SA      Moved all internal functions to dsgcsdi.h.
03/19/08   SA      Removed the dependency on internal header.
07/08/07   DA      Added user data inactivity timer functionality.
09/01/06   AR      Add support for multiprocessor build.
07/10/06   AR      Add Gcsd_curr_call_state to track call processing state.
11/21/05   AR      Add supprot for multiple call instances.
09/16/03   AR      Added support for suspend/resume data transfer commands.
09/28/01   TMR     Created

===========================================================================*/


/*===========================================================================
                         Header files
===========================================================================*/
#include "datamodem_variation.h"
#include "queue.h"
#include "rex.h"
#include "dsm.h"
#include "cm.h"
#include "dsat_v.h"
#include "dsgcsd_task.h"
#include "dsucsdi.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Data Services Task Signal Definitions
---------------------------------------------------------------------------*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Task top-level signals
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#define DS_GCSD_TASK_START_SIG       0x8000              /* TAST_START_SIG */
#define DS_GCSD_TASK_STOP_SIG        0x4000               /* TASK_STOP_SIG */
#define DS_GCSD_TASK_OFFLINE_SIG     0x2000            /* TASK_OFFLINE_SIG */
#define DS_GCSD_CMD_Q_SIG            0x1000        /* Command Queue signal */
#define DS_GCSD_DOG_HB_REPORT_SIG    0x0800        /* Dog HB Report signal */ 
#define DS_GCSD_TIMER_SIG            0x0400                /* Timer signal */
#define DS_GCSD_PROC_UL_SIG          0x0200       /* Process uplink signal */
#define DS_GCSD_PROC_DL_SIG          0x0100     /* Process downlink signal */

#ifdef FEATURE_DATA_GCSD_FAX
#define DS_GCSD_T31_TIMEOUT_SIG      0x0080   /* T31 Protocol Timer signal */
#define DS_GCSD_FA_TIMEOUT_SIG       0x0040   /*  FA Protocol Timer signal */
#endif

#define DS_GCSD_INVALID_SREG_VALUE    -1
#define DS_GCSD_S30_REG              30


/*---------------------------------------------------------------------------
  Data Services Task Command Definitions
---------------------------------------------------------------------------*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Command Header type
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
typedef struct
{
   q_link_type            link;      /* Queue link type        */
   ds_gcsd_cmd_enum_type  cmd_id;    /* Identifies the command */
} ds_gcsd_cmd_hdr_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Command Data types -- none currently
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Command type: The command header identifies the command. If the command has
  any parameters, they are specified in the union.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
typedef struct
{
   ds_gcsd_cmd_hdr_type    hdr;                   /* Command header */
   union
   {
     void (*dsat_callback) (dsat_result_enum_type result);

     /* Call Manager call ID */
     cm_call_id_type  call_id;
   } cmd;
} ds_gcsd_cmd_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Call processing state: Track state if current call processing.  Assumes
  GCSD supports single call instance.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
typedef enum gcsd_call_state
{
  GCSD_CALL_STATE_IDLE,         /* Call processing idle */
  GCSD_CALL_STATE_CONNECT,      /* Call active */
  GCSD_CALL_STATE_DISCONNECT,   /* Call teardown in progress */
  GCSD_CALL_STATE_MAX           /* Internal use only */
} ds_gcsd_call_state_type;



/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION DS_GCSD_GET_CMD_BUF

DESCRIPTION
  This function allocates a command buffer from theData Services GCSD Protocol 
  Stack Task.
  The caller of this function must check for a NULL return value, and either
  perform some error recovery or ERR_FATAL, as appropriate.

DEPENDENCIES
  None

RETURN VALUE
  Returns a pointer to a command buffer, if available, or NULL if no command
  buffers are available.

SIDE EFFECTS
  None

===========================================================================*/

extern ds_gcsd_cmd_type*
ds_gcsd_get_cmd_buf( void );


/*===========================================================================

FUNCTION DS_GCSD_PUT_CMD

DESCRIPTION
  This function puts a command on the Data Services GCSD Protocol Stack Task's 
  command queue, and sets the DS_GCSD_CMD_Q_SIG.

DEPENDENCIES
  The caller of this function should have previously allocated a command
  buffer by calling ds_gcsd_get_cmd_buf().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void  
ds_gcsd_put_cmd
(
   ds_gcsd_cmd_type    *cmd_ptr           /* Pointer to command to be sent */
);


/*===========================================================================

FUNCTION DS_GCSD_INIT_WATERMARKS

DESCRIPTION
  This function specifies and initializes the watermarks which are to
  be used for GCSD call.  Parameters may be NULL to deregister watermarks.

DEPENDENCIES
  Must be called after ds_gcsd_init_call_data().

RETURN VALUE
  None

SIDE EFFECTS
  Sets GCSD global data

===========================================================================*/
extern void  
ds_gcsd_init_watermarks
(
  const uint32                        call_id,
  const ds_ucsd_call_type             call_type,
  dsm_watermark_type                 *gcsd_dl_wm_ptr,
  dsm_watermark_type                 *gcsd_ul_wm_ptr,
  dsm_watermark_type                 *fax_sio_dl_wm_ptr,
  dsm_watermark_type                 *fax_sio_ul_wm_ptr   
);

/*===========================================================================

FUNCTION DS_GCSD_START_GENERAL_PURPOSE_TIMER

DESCRIPTION
  This function starts the Gcsd_general_purpose_timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_gcsd_start_general_purpose_timer (void);

/*===========================================================================

FUNCTION DS_GCSD_STOP_GENERAL_PURPOSE_TIMER

DESCRIPTION
  This function stops the Gcsd_general_purpose_timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_gcsd_stop_general_purpose_timer (void);

/*===========================================================================

FUNCTION DS_GCSD_GET_GCSD_CURR_DATA_RATE

DESCRIPTION
  This function turns the value of the global variable
  Gcsd_curr_data_rate.

DEPENDENCIES
  None

RETURN VALUE
  ds_ucsd_data_rate_T - value of the Gcsd_curr_data_rate variable.

SIDE EFFECTS
  None

===========================================================================*/
ds_ucsd_data_rate_T ds_gcsd_get_Gcsd_curr_data_rate(void);

/*===========================================================================

FUNCTION DS_GCSD_GET_GCSD_CURR_CALL_IS_FAX

DESCRIPTION
  This function turns the value of the global variable
  Gcsd_curr_call_is_fax.

DEPENDENCIES
  None

RETURN VALUE
  boolean - value of the Gcsd_curr_call_is_fax variable.

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_gcsd_get_Gcsd_curr_call_is_fax(void);

/*===========================================================================

FUNCTION DS_GCSD_GET_GCSD_CURR_CALL_STATE

DESCRIPTION
  This function turns the value of the global variable
  Gcsd_curr_call_state.

DEPENDENCIES
  None

RETURN VALUE
  ds_gcsd_call_state_type - value of the Gcsd_curr_call_state variable.

SIDE EFFECTS
  None

===========================================================================*/
ds_gcsd_call_state_type ds_gcsd_get_Gcsd_curr_call_state(void);

/*===========================================================================

FUNCTION DS_GCSD_GET_GCSD_DL_WM_PTR

DESCRIPTION
  This function turns the value of the global variable
  Gcsd_dl_wm_ptr.

DEPENDENCIES
  None

RETURN VALUE
  dsm_watermark_type* - value of the Gcsd_dl_wm_ptr variable.

SIDE EFFECTS
  None

===========================================================================*/
dsm_watermark_type* ds_gcsd_get_Gcsd_dl_wm_ptr(void);

/*===========================================================================

FUNCTION DS_GCSD_GET_GCSD_UL_WM_PTR

DESCRIPTION
  This function turns the value of the global variable
  Gcsd_ul_wm_ptr.

DEPENDENCIES
  None

RETURN VALUE
  dsm_watermark_type* - value of the Gcsd_ul_wm_ptr variable.

SIDE EFFECTS
  None

===========================================================================*/
dsm_watermark_type* ds_gcsd_get_Gcsd_ul_wm_ptr(void);

/*===========================================================================

FUNCTION DS_GCSD_GET_GCSD_FAX_SIO_DL_WM_PTR

DESCRIPTION
  This function turns the value of the global variable
  Gcsd_fax_sio_dl_wm_ptr.

DEPENDENCIES
  None

RETURN VALUE
  dsm_watermark_type* - value of the Gcsd_fax_sio_dl_wm_ptr variable.

SIDE EFFECTS
  None

===========================================================================*/
dsm_watermark_type* ds_gcsd_get_Gcsd_fax_sio_dl_wm_ptr(void);

/*===========================================================================

FUNCTION DS_GCSD_GET_GCSD_FAX_SIO_UL_WM_PTR

DESCRIPTION
  This function turns the value of the global variable
  Gcsd_fax_sio_ul_wm_ptr.

DEPENDENCIES
  None

RETURN VALUE
  dsm_watermark_type* - value of the Gcsd_fax_sio_ul_wm_ptr variable.

SIDE EFFECTS
  None

===========================================================================*/
dsm_watermark_type* ds_gcsd_get_Gcsd_fax_sio_ul_wm_ptr(void);

/*===========================================================================

FUNCTION DS_GCSD_GET_GCSD_WM_CONTEXT

DESCRIPTION
  This function turns the value of the global variable
  Gcsd_wm_context.

DEPENDENCIES
  None

RETURN VALUE
  uint32 - value of the Gcsd_wm_context variable.

SIDE EFFECTS
  None

===========================================================================*/
uint32 ds_gcsd_get_Gcsd_wm_context(void);

/*===========================================================================

FUNCTION DS_GCSD_GET_CURRENT_MODEM_CONTROL_LINES

DESCRIPTION
  This function turns the value of the global variable
  CurrentModemControlLines.

DEPENDENCIES
  None

RETURN VALUE
  ds_ucsd_sio_status_T* - Pointer of CurrentModemControlLines variable

SIDE EFFECTS
  None

===========================================================================*/

ds_ucsd_sio_status_T* ds_gcsd_get_CurrentModemControlLines(void);

/*===========================================================================

FUNCTION DS_GCSD_SET_OLD_MODEM_CONTROL_LINES

DESCRIPTION
  This function turns the value of the global variable
  OldModemControlLines.

DEPENDENCIES
  None

RETURN VALUE
  ds_ucsd_sio_status_T* - Pointer of OldModemControlLines variable

SIDE EFFECTS
  None

===========================================================================*/

ds_ucsd_sio_status_T* ds_gcsd_get_OldModemControlLines(void);

/*===========================================================================

FUNCTION DS_GCSD_SET_CURRENTMODEMCONTROLLINES

DESCRIPTION
  This function turns the value of the global variable
  CurrentModemControlLines.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void ds_gcsd_set_CurrentModemControlLines
(
  ds_ucsd_sio_status_T   *CurrentModemControlLines_ptr
);

/*===========================================================================

FUNCTION DS_GCSD_SET_OLD_MODEM_CONTROL_LINES

DESCRIPTION
  This function turns the value of the global variable
  OldModemControlLines.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void ds_gcsd_set_OldModemControlLines
(
  ds_ucsd_sio_status_T   *OldModemControlLines_ptr
);

#endif /* DSGCSD_TASK_V_H */
