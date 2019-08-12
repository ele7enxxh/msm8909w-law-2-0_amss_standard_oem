#ifndef _UI_BASE_H
#define _UI_BASE_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


           U S E R   I N T E R F A C E   M A I N  M O D U L E

GENERAL DESCRIPTION
  This is the minimal "user interface".  It is responsible for ensuring
  the UI task starts and handles the phone events not related to the
  user.  This code will exist to ensure the ui_task runs properly on
  a phone without a UI or with a UI on a different processor.  If the
  phone has a UI on this processor.

EXTERNALIZED FUNCTIONS

  ui_task
    The main user interface task.  Handle events

  ui_kick_dog
    Kicks the watchdog.

  Defines uisleep & uisleepold global variables.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  ui_task should be created by MC before anyone calls ui_cmd.
  NV, HS, SND, and DIAG should be started before ui_task.
  All other functions are internal to UI.

  Copyright (c) 1999 through 2006 by Qualcomm Technologies INCORPORATED.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/coreapp/src/ui_base.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/26/12   mg      Set ctm bit from thin UI if auto answer is enabled.
9/29/11    kvk     Initial revision
===========================================================================*/
#include "mmcp_variation.h"
#include "cm.h"
#include "rex.h"
#include "nv.h"

#ifdef FEATURE_COREAPP_CSG
#include "ui_base_msgr_msg.h"
#include "ui_base_msgr_if.h"
#endif

#ifndef CM_UNIT_TEST
  #include "msg.h"
#endif

/* Signals for the UI task */
/* avoid common signals listed in task.h */
#define UI_KEY_SIG         0x0002  /* Key from HS task */
#define UI_RPT_TIMER_SIG   0x0004  /* Time to kick watchdog */
#define UI_NV_SIG          0x0008  /* Return from NV */
#define UI_AUTOANSWER_SIG  0x0010  /* autoanswer timer expired */
#define UI_OEM_SIG         0x0020  /* APP COM FUNC signal to block UI task. */
#define UI_ICARD_SIG       0x0040  /* Signal Used for ICARD related events */
#define AEE_MEDIA_SIG      0x0080  /* Signal for media-related syncronization */
#define UI_IPC_SIG         0x0100
#define AEE_SIO_SIG        0x0200
#define UI_RUIM_SIG        0x0400
#define AEE_NET_SIG        0x0800 /* Used to help BREW process Network signals */
#define AEE_APP_SIG        0x1000
//From task.h, avoid overlap
//define  TASK_START_SIG   0x8000
//define  TASK_STOP_SIG    0x4000
//define  TASK_OFFLINE_SIG 0x2000

#define AEE_USBDRIVER_SIG  0x20000
#define AEE_SLEEP_SIG      0x80000 /* AEE_SLEEP_SIG for Brew 3.1 Sleep. */

#define UI_BAC_SIG         0x100000 /*Used for BREW App Coordination with 
                                      alternate environment applications */
#define UI_TS_EVENT_SIG    0x200000 
/*message router signal*/
#define UI_BASE_TASK_MSGR_SIGNAL 0x400000 
/*Internal timer signal*/
#define UI_BASE_TASK_TIMER_SIGNAL 0x800000
/*Internal command posting signal*/
#define UI_BASE_TASK_CMD_Q_SIGNAL 0x1000000
/*--------------------------------------------------------------------------
  Pointer to qmi_mmode_task_tcb
---------------------------------------------------------------------------*/
extern rex_tcb_type *ui_tcb_ptr;

#define UI_BASE_SET_SIGNAL( sig )                                               \
  (void)rex_set_sigs( ui_tcb_ptr, (rex_sigs_type) sig )

#define UI_BASE_CLR_SIGNAL( sig )                                               \
  (void)rex_clr_sigs( ui_tcb_ptr, (rex_sigs_type) sig )

#define UI_BASE_OFFSETOF(type,member) (((char*)(&((type*)1)->member))-((char*)1))


/*New ThinUI (CoreApp) MSG macros. */
#define TUI_MSG_FATAL(x_fmt) \
    MSG (MSG_SSID_DFLT, MSG_LEGACY_FATAL, x_fmt)

#define TUI_MSG_ERROR(x_fmt) \
    MSG (MSG_SSID_DFLT, MSG_LEGACY_ERROR, x_fmt)

#define TUI_MSG_HIGH(x_fmt) \
    MSG (MSG_SSID_DFLT, MSG_LEGACY_HIGH, x_fmt)

#define TUI_MSG_MED(x_fmt) \
    MSG (MSG_SSID_DFLT, MSG_LEGACY_MED, x_fmt)

#define TUI_MSG_LOW(x_fmt) \
    MSG (MSG_SSID_DFLT, MSG_LEGACY_LOW, x_fmt)

#define TUI_MSG_FATAL_1(x_fmt, a) \
    MSG_1 (MSG_SSID_DFLT, MSG_LEGACY_FATAL, x_fmt, a)

#define TUI_MSG_ERROR_1(x_fmt, a) \
    MSG_1 (MSG_SSID_DFLT, MSG_LEGACY_ERROR, x_fmt, a)

#define TUI_MSG_HIGH_1(x_fmt, a) \
    MSG_1 (MSG_SSID_DFLT, MSG_LEGACY_HIGH, x_fmt, a)

#define TUI_MSG_MED_1(x_fmt, a) \
    MSG_1 (MSG_SSID_DFLT, MSG_LEGACY_MED, x_fmt, a)

#define TUI_MSG_LOW_1(x_fmt, a) \
    MSG_1 (MSG_SSID_DFLT, MSG_LEGACY_LOW, x_fmt, a)

#define TUI_MSG_FATAL_2(x_fmt, a, b) \
    MSG_2 (MSG_SSID_DFLT, MSG_LEGACY_FATAL, x_fmt, a, b)

#define TUI_MSG_ERROR_2(x_fmt, a, b) \
    MSG_2 (MSG_SSID_DFLT, MSG_LEGACY_ERROR, x_fmt, a, b)

#define TUI_MSG_HIGH_2(x_fmt, a, b) \
    MSG_2 (MSG_SSID_DFLT, MSG_LEGACY_HIGH, x_fmt, a, b)

#define TUI_MSG_MED_2(x_fmt, a, b) \
    MSG_2 (MSG_SSID_DFLT, MSG_LEGACY_MED, x_fmt, a, b)

#define TUI_MSG_LOW_2(x_fmt, a, b) \
    MSG_2 (MSG_SSID_DFLT, MSG_LEGACY_LOW, x_fmt, a, b)

#define TUI_MSG_FATAL_3(x_fmt, a, b, c) \
    MSG_3 (MSG_SSID_DFLT, MSG_LEGACY_FATAL, x_fmt, a, b, c)

#define TUI_MSG_ERROR_3(x_fmt, a, b, c) \
    MSG_3 (MSG_SSID_DFLT, MSG_LEGACY_ERROR, x_fmt, a, b, c)

#define TUI_MSG_HIGH_3(x_fmt, a, b, c) \
    MSG_3 (MSG_SSID_DFLT, MSG_LEGACY_HIGH, x_fmt, a, b, c)

#define TUI_MSG_MED_3(x_fmt, a, b, c) \
    MSG_3 (MSG_SSID_DFLT, MSG_LEGACY_MED, x_fmt, a, b, c)

#define TUI_MSG_LOW_3(x_fmt, a, b, c) \
    MSG_3 (MSG_SSID_DFLT, MSG_LEGACY_LOW, x_fmt, a, b, c)

/* TTY modes */
typedef enum
{
  UI_TTY_MODE_FULL,  /* TTY Full */
  UI_TTY_MODE_VCO,   /* TTY Voice Carryover */
  UI_TTY_MODE_HCO,   /* TTY Hearing Carryover */
  UI_TTY_MODE_OFF    /* TTY Off */
} ui_tty_mode_e_type;

/*===========================================================================*/

/*===========================================================================*/

extern void ui_task (
  dword dummy                     /*lint -esym(715,dummy) */
                                  /*ignore fact that this isn't used */
);

/*===========================================================================
FUNCTION ui_kick_dog

DESCRIPTION
  This function reports to dog task that UI is alive and kicking!

RETURN VALUE
  None

DEPENDENCIES
  Dog Task is running.
===========================================================================*/
extern void ui_kick_dog( void );

/*===========================================================================
FUNCTION ui_answer_call

DESCRIPTION
  This function answers an incoming call.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void ui_answer_call (void);
/*===========================================================================*/

/*===========================================================================*/
extern void HandleSignals (
    rex_sigs_type sigs
);

/*===========================================================================*/

/*===========================================================================*/

/*===========================================================================*/

/*===========================================================================*/
extern void ui_call_err_cb(void *data_ptr, cm_call_cmd_e_type call_cmd,
                    cm_call_cmd_err_e_type call_cmd_err);
/*===========================================================================*/

/*===========================================================================*/
extern nv_stat_enum_type ui_get_nv(
  nv_items_enum_type item,        /* which item */
  nv_item_type *data_ptr          /* pointer to space for item */
);
/*===========================================================================*/


/*===========================================================================*/
extern void ui_subscription_cb(void *data_block, cm_ph_cmd_e_type ph_cmd,
    cm_ph_cmd_err_e_type ph_cmd_err);
/*===========================================================================*/

/*===========================================================================*/
extern void HandleStopSig(void);
/*===========================================================================*/

/*===========================================================================*/
extern void HandleOfflineSig(void);
/*===========================================================================*/
#ifdef FEATURE_COREAPP_CSG
/*===========================================================================*/
void ui_base_msgr_init( void );
/*===========================================================================*/

int ui_base_get_cm_client_id(void);
#endif
#endif /* #define _UI_BASE_H*/