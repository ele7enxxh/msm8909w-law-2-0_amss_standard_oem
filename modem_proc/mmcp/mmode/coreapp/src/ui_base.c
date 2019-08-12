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
  NV, and DIAG should be started before ui_task.
  All other functions are internal to UI.

  Copyright (c) 1999 through 2012 by Qualcomm Technologies INCORPORATED.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/coreapp/src/ui_base.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/03/13   dk      KW error fixed in ui_answer_call
06/25/13   dk      CR470025 stack reduction effort
01/23/13   skk     Adding support to enable ThinUI setup respose configurable
                   with EFS file(/mmode/thinui_setup_resp_req.txt).
10/18/12   gm      Thin UI should put modem online by default in 9x25
09/27/12   vm      Removing DIME featurization and cleanup
9/25/12    gm      Removing references to TCB object for UI_BASE
9/18/12    vm      Integrating: Fixing issue with call_id checked again call_type
08/08/12   vm      Removing processing for rf initialization as cm handles it
2/2/11     KVK     Added support for TD-SCDMA
10/7/11    KVK     Fixed calling tmc_task_start() twice
9/30/11    KVK     Fixed featurization
9/29/11    KVK     Initial version
3/08/12    gm      RCInit Integration changes
7/30/12    vm      Reading sys_mode from incoming call ptr, handling LTE and
                           handling call end for voice calls.
10/26/12   mg      Set ctm bit from thin UI if auto answer is enabled.
===========================================================================*/

#include "mmcp_variation.h"
#include "target.h"
#include "customer.h"
#include "comdef.h"
#include "msg.h"
#include "err.h"
#include "rex.h"

#include <string.h>

#include "nv.h"
#include "nv_items.h"
#include "dog_hb_rex.h"    /*Interface for the heartbeat watchdog services*/
#include "cm.h"
#include "sd.h"

#include "mcfg_fs.h"   /* Interface to MCFG efs wrappers */

#ifdef FEATURE_COREAPP_CSG
#include "ui_base_timer.h"
#include "ui_base_msgr_if.h"
#include "csg_app_cm_if.h"
#endif

#include "rcinit.h"
#include "rfm.h"
#include "rcevt_rex.h"
#include "modem_mem.h"
#ifdef CM_UNIT_TEST
#error code not present
#endif
#include "msgr_types.h"
#include "msgr.h"
#include "msgr_rex.h"
#include "ui_base.h"


static boolean bAutoAnswer = FALSE;
static boolean bThinUI_setup_resp_req = FALSE;

static cm_call_type_e_type ui_incomCall_type;
static cm_client_id_type cm_client_id;

/* -----------------------------------
** Define watchdog report handle
** -----------------------------------*/
static dog_report_type   ui_dog_rpt = 0;       /*Initial Safety*/

static uint8 numberOfVoiceCalls = 0;
static cm_call_id_type ui_incomCall_id = CM_CALL_ID_UNASSIGNED;
static cm_call_type_e_type ui_incomCall_type;
static sys_sys_mode_e_type ui_incomCall_sys_mode;
static cm_call_mode_info_e_type ui_incomCall_mode_info;
static ui_tty_mode_e_type ui_tty_mode = UI_TTY_MODE_OFF;
rex_tcb_type *ui_tcb_ptr;

LOCAL rex_timer_type  ui_autoanswer_timer; /* Timer for autoanswer. */
#define UI_AUTOANSWER_TIME          3000   // millisecond timer for autoanswer

#ifdef FEATURE_NEW_SLEEP_API
  //sleep_okts_handle gNewSleepHandle;
#endif /* FEATURE_NEW_SLEEP_API */

typedef enum {
    AMSS_MODE,
    DMSS_MODE = AMSS_MODE,
    FTM_MODE
} ftm_mode_type;

#define DUAL_RATE_FULL_RATE_PREFERRED        3
#define ITC_SPEECH                           0  /* Speech */
#define GSM                                  0  /* coding standard */
#define CIRCUIT                              0  /* transfer mode */

#ifdef FEATURE_COREAPP_CSG
msgr_client_t  ui_base_msgr_client;
#endif


void ui_call_event_cb( cm_call_event_e_type call_event, /* call event */
                       const cm_mm_call_info_s_type *call_info_ptr);
					   
/*===========================================================================
FUNCTION   static void uibase_create_thinui_config

DESCRIPTION
  create conf file for thin ui if it does not exist

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
static void uibase_create_thinui_config(void)
{
  /* thin_ui_config is config file ., points to
   enable_thin_ui_cfg is the efs item file */

  struct fs_stat file_stat;
  int conf_fd = -1;
  int size_committed;
  char *thin_ui_config_list = "/nv/item_files/Thin_UI/enable_thin_ui_cfg";

  /* write into the CONF file as a 'regular' file */
  if(  (mcfg_fs_stat("/nv/item_files/conf/thin_ui_config.conf", &file_stat, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_0) 
        != MCFG_FS_STATUS_OK)
     && mcfg_fs_errno(MCFG_FS_TYPE_EFS) == ENOENT  )

   {
    TUI_MSG_HIGH("thin_ui_config.conf absent., creating it");

    /* create the directories */
    (void)mcfg_mkdir("/nv",0777,MCFG_FS_TYPE_EFS);
    TUI_MSG_MED_1("create Error for /nv: %d", mcfg_fs_errno(MCFG_FS_TYPE_EFS));

    (void)mcfg_mkdir("/nv/item_files",0777,MCFG_FS_TYPE_EFS);
    TUI_MSG_MED_1("create Error for /item_files: %d", mcfg_fs_errno(MCFG_FS_TYPE_EFS));

    (void)mcfg_mkdir("/nv/item_files/conf",0777,MCFG_FS_TYPE_EFS);
    TUI_MSG_MED_1("create Error for /conf: %d", mcfg_fs_errno(MCFG_FS_TYPE_EFS));

    (void)mcfg_mkdir("/nv/item_files/Thin_UI",0777,MCFG_FS_TYPE_EFS);
    TUI_MSG_MED_1("create Error for /Thin_UI: %d", mcfg_fs_errno(MCFG_FS_TYPE_EFS));

    conf_fd = mcfg_fopen ("/nv/item_files/conf/thin_ui_config.conf", 
                          MCFG_FS_O_WRONLY|MCFG_FS_O_CREAT|MCFG_FS_O_TRUNC, 
                          MCFG_FS_ALLPERMS, 
                          MCFG_FS_TYPE_EFS, 
                          MCFG_FS_SUBID_0);

    if (conf_fd >= 0)
    {
      size_committed = mcfg_fwrite(conf_fd,
                                   thin_ui_config_list,
                                   strlen(thin_ui_config_list),
                                   MCFG_FS_TYPE_EFS);
      if (size_committed < 0)
      {
        TUI_MSG_ERROR("mcfg_fwrite error in thin_ui_config.conf");
        return;
      }
    }else
    {
      return;
    }
    (void)mcfg_fclose(conf_fd, MCFG_FS_TYPE_EFS); /* close CONF file */
    TUI_MSG_HIGH_1("close thin_ui_config.conf %d", conf_fd);
   }
   else
   {
    return;
   }

}/* uibase_create_thinui_config() */




/*===========================================================================
FUNCTION   static boolean uibase_read_thinui_efs

DESCRIPTION
  read the thinui efs item file pointed by the conf file

DEPENDENCIES
  None

RETURNS
  TRUE/ FALSE

SIDE EFFECTS
  None
===========================================================================*/
static boolean uibase_read_thinui_efs (void)
{
  int read_size = sizeof(boolean);
  boolean isthinui = FALSE;
  mcfg_fs_status_e_type status;

  status = mcfg_fs_read("/nv/item_files/Thin_UI/enable_thin_ui_cfg", 
                        (byte*)&isthinui, 
                        read_size, 
                        MCFG_FS_TYPE_EFS, 
                        MCFG_FS_SUBID_0 );

  if(status != MCFG_FS_STATUS_OK)
  {
    TUI_MSG_HIGH_2("efs read error %d, error no. %d", status, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    return FALSE;
}
  return isthinui;
} /* uibase_read_thinui_efs */




/*===========================================================================
FUNCTION   ui_init

DESCRIPTION
  Initialize the ui task.

DEPENDENCIES
  None

RETURNSui
  None

SIDE EFFECTS
  None
===========================================================================*/
void ui_init( void )
{
  nv_item_type nvi;

  ui_tcb_ptr = rex_self();

  /* Register with Watchdog Heartbeat Services */
#ifndef CM_UNIT_TEST
  ui_dog_rpt = dog_hb_register_rex((rex_sigs_type) UI_RPT_TIMER_SIG);
#endif
  (void) rex_clr_sigs( rex_self(), TASK_START_SIG );

  /* Set the watchdog report timer signal so that we'll
  ** reset the watchdog as soon as we start checking for events
  ** and start our first tick timer */
  (void) rex_set_sigs( ui_tcb_ptr, UI_RPT_TIMER_SIG );

  cm_client_init(CM_CLIENT_TYPE_UI, &cm_client_id);


#ifdef FEATURE_NEW_SLEEP_API
  //gNewSleepHandle = sleep_register("UI_TASK", FALSE);
#endif /* FEATURE_NEW_SLEEP_API */

  /* Initialize timers */
  rex_def_timer( &ui_autoanswer_timer, ui_tcb_ptr, UI_AUTOANSWER_SIG );

  cm_mm_client_call_reg(cm_client_id, ui_call_event_cb, CM_CLIENT_EVENT_REG,
                        CM_CALL_EVENT_ORIG, CM_CALL_EVENT_EXIT_TC,
                        NULL);

  #ifdef FEATURE_COREAPP_CSG
  csg_app_cm_if_init();
  #else
  /*Client activation takes place inside csg init if enabled*/
  cm_client_act(cm_client_id);
  #endif

  //load the auto answer option from nv
  if(ui_get_nv(NV_AUTO_ANSWER_I, &nvi) == NV_DONE_S)
  {
    bAutoAnswer = nvi.auto_answer.enable;
  }
  /* Read the TTY mode from NV */
  if(ui_get_nv(NV_TTY_I, &nvi) == NV_DONE_S)
  {
    ui_tty_mode = nvi.tty;
  }
  TUI_MSG_HIGH_2("Auto answer: %d, TTY mode: %d", bAutoAnswer, ui_tty_mode);

  //Default is same as auto answer
  bThinUI_setup_resp_req = bAutoAnswer;

  //Read "ThinUI setup_resp required"? from efs /mmode/thinui_setup_resp_req.txt
  {
     char aData[2] = {0};
     int nSize = 0, fd = 0;

     fd = mcfg_fopen( "/mmode/thinui_setup_resp_req.txt",
                      MCFG_FS_O_RDONLY,
                      MCFG_FS_ALLPERMS,
                      MCFG_FS_TYPE_EFS,
                      MCFG_FS_SUBID_0);
     if( fd > 0 )
     {
        /* Read data from file. */
        nSize = mcfg_fread( fd, aData, sizeof(char), MCFG_FS_TYPE_EFS );
        TUI_MSG_HIGH_2("nSize: %d, data=%c", nSize, aData[0]);
        if(nSize  > 0 )
        {
          if(aData[0] == '1')
            bThinUI_setup_resp_req = TRUE;
        }
        /* Close the file */
        (void) mcfg_fclose(fd, MCFG_FS_TYPE_EFS);
     }
     else
     {
        TUI_MSG_HIGH_1("thinui_setup_resp_req efs file open failed : %d", fd);
     }
  }
} /* end of ui_init */

/*===========================================================================

FUNCTION ui_wait

DESCRIPTION
  Main Wait routine for UI task. This routine performs the following functions:
    - Waits for a signal in the requested mask to be set.
    - Handles Watchdog report signal
    - Handles Key release to finish the keybeep if Core UI is built in

DEPENDENCIES
  None

RETURN VALUE
  A rex_sigs_type value indicating all signals set (the current state
  of all signals) The calling unit can then check for the reason for
  exiting the wait.

SIDE EFFECTS
  None

===========================================================================*/
rex_sigs_type ui_wait
(
   rex_sigs_type requested_mask    /* Mask of signals to wait for */
)
{
   rex_sigs_type always_handle_mask; /* Mask of signals to handle while waiting */
   rex_sigs_type rex_signals_mask;   /* Mask of signals returned by REX     */



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   /* Always handle the UI_RPT_TIMER_SIG. This tells UI task to kick the watchdog. */
   always_handle_mask = UI_RPT_TIMER_SIG;

   /* Get current state of rex signals */
   rex_signals_mask = rex_get_sigs( ui_tcb_ptr );

   /* --------------------------------------------------------------------
   ** Loop until the current signals from REX (including the queue signals
   ** faked due to an item on a queue, if any) match with one or more of
   ** the requested signals.
   ** -------------------------------------------------------------------- */
   do
   {

      /* Perform the 'real' wait (if needed) */
      if ((rex_signals_mask & requested_mask) == 0)
      {
         rex_signals_mask = rex_wait( requested_mask | always_handle_mask);
      }


      /* Need to kick the watchdog */
      if( rex_signals_mask & UI_RPT_TIMER_SIG )
      {
        (void)rex_clr_sigs( rex_self(), UI_RPT_TIMER_SIG );
        ui_kick_dog();
      }

   /* do while mask requested does not match current mask */
   } while ((rex_signals_mask & requested_mask) == 0);

   return (rex_signals_mask);

} /* end ui_wait() */

/*===========================================================================
FUNCTION ui_kick_dog

DESCRIPTION
  This function reports to dog task that UI is alive and kicking!

RETURN VALUE
  None

DEPENDENCIES
  Dog Task is running.
===========================================================================*/
void ui_kick_dog( void )
{
   #ifndef CM_UNIT_TEST
   dog_hb_report( ui_dog_rpt );
   #endif
} /* End of ui_kick_dog */

/*===========================================================================
FUNCTION ui_oprt_mode_cb

DESCRIPTION
  This function is called at the completion of oprt mode cmd processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ui_oprt_mode_cb(void *data_block, cm_ph_cmd_e_type ph_cmd,
                     cm_ph_cmd_err_e_type ph_cmd_err)
{
  if (ph_cmd_err != CM_PH_CMD_ERR_NOERR)
  {
    TUI_MSG_HIGH_2("CM oprt cmd %d, rsp %d", ph_cmd, ph_cmd_err);
  }
}


/*===========================================================================
FUNCTION UI_TASK

DESCRIPTION
  The main user interface function.  This is the big loop outside of the
  state machine which handles signals and all sorts of inputs.  It mostly
  serves to convert inputs into events for the state machine, but it
  also handles keypad incidentals - backlighting and key beeps.

DEPENDENCIES
  REX is running.  NV and DIAG and HS are running.

RETURN VALUE
  Never exits.

SIDE EFFECTS
  Plays with variable ui.

===========================================================================*/

void ui_task (
  dword dummy                     /*lint -esym(715,dummy) */
                                  /*ignore fact that this isn't used */
)
{
  rex_sigs_type waitMask;         /* hold signals */

  RCEVT_SIGEX_SIGREX term_sig;
  RCINIT_GROUP ui_group = RCINIT_GROUP_NONE;
  nv_item_type nvi;

  boolean nv_thin_ui = FALSE;
  int nv_size = sizeof(boolean);
  mcfg_fs_status_e_type status;

  ui_tcb_ptr = rex_self();

  rcinit_handshake_startup();

  uibase_create_thinui_config();

  status = mcfg_fs_read ("/nv/item_files/Thin_UI/enable_thin_ui_cfg", 
                         (byte*)&nv_thin_ui, 
                         nv_size, 
                         MCFG_FS_TYPE_EFS, 
                         MCFG_FS_SUBID_0 );

  nv_thin_ui = uibase_read_thinui_efs();

  //ACK the TMC task
  ui_init();                      /* initialize task. */

  term_sig.signal = ui_tcb_ptr;
  term_sig.mask = TASK_STOP_SIG;

  /* Lookup for sequence group */
  ui_group = rcinit_lookup_group_rextask(ui_tcb_ptr);

  /* Registering for TASK_STOP_SIG with rcinit */
  if(ui_group != RCINIT_GROUP_NONE ){
#ifndef CM_UNIT_TEST
    rcinit_register_term_group(ui_group,
                             RCEVT_SIGEX_TYPE_SIGREX,
                             &term_sig);
#endif
  }
  else {
    ERR( "Invalid Sequence group rcinit_lookup_group_rextask failed ", 0, 0, 0);
  }


  /* Initialize WaitMask */
  waitMask = (UI_RPT_TIMER_SIG | TASK_OFFLINE_SIG | TASK_STOP_SIG);

  waitMask |= UI_AUTOANSWER_SIG ;

  #ifdef FEATURE_COREAPP_CSG
  waitMask |= UI_BASE_TASK_MSGR_SIGNAL ;
  waitMask |= UI_BASE_TASK_TIMER_SIGNAL ;
  #endif

  TUI_MSG_HIGH_2("EFS enable_thin_ui_cfg status %d,value=%d", status, nv_thin_ui);

  if(nv_thin_ui == TRUE && status == MCFG_FS_STATUS_OK)
  {
    if(ui_get_nv(NV_FTM_MODE_I, &nvi) != NV_DONE_S)
    {
      TUI_MSG_HIGH("FTM NV is not valid, ThinUI makes modem ONLINE");
      cm_ph_cmd_oprt_mode(ui_oprt_mode_cb, NULL, cm_client_id,
                          SYS_OPRT_MODE_ONLINE );
    }
    else if(nvi.ftm_mode != FTM_MODE)
    {
      TUI_MSG_HIGH("FTM NV is 0, ThinUI makes modem ONLINE");
      cm_ph_cmd_oprt_mode(ui_oprt_mode_cb, NULL, cm_client_id,
                          SYS_OPRT_MODE_ONLINE );
    }
  }

#ifdef FEATURE_COREAPP_CSG
ui_base_msgr_init();
ui_base_timer_init();
#endif

#ifndef CM_UNIT_TEST
  for( ;; ) {                 /* Wait on REX signals, repeat forever */
     rex_sigs_type sigs;      /* hold signals */

     sigs = rex_wait(waitMask);

    /* Process the signals */

    if (sigs & TASK_STOP_SIG){
      waitMask &= ~TASK_STOP_SIG;
    }
    HandleSignals( sigs );

  }
#endif

} /* ui_task() */


  /*===========================================================================
FUNCTION HandleSignals

DESCRIPTION
  This function handles all signals received by UI task. This function is to
  be invoked for every signal UI recives. It calls the appropriate functions
  to handle each signal.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void HandleSignals (
    rex_sigs_type sigs
)
{

  /* Stop task - nothing special to do */
  if ( ( sigs & TASK_STOP_SIG ) != 0 )
  {
    HandleStopSig();
  }

  /* Go offline signal */
  if( ( sigs & TASK_OFFLINE_SIG ) != 0 ) {
    HandleOfflineSig();

  }

  /* Report back to the watchdog.  */
  if( sigs & UI_RPT_TIMER_SIG )
  {
    (void)rex_clr_sigs( rex_self(), UI_RPT_TIMER_SIG );
    ui_kick_dog();
  }

  if ( sigs & UI_AUTOANSWER_SIG )
  {
    (void) rex_clr_sigs( ui_tcb_ptr, UI_AUTOANSWER_SIG );
    //The autoanswer timer expired, so answer the call.
    ui_answer_call();
  }

  #ifdef FEATURE_COREAPP_CSG
  if ( sigs & UI_BASE_TASK_MSGR_SIGNAL )
  {
    UI_BASE_CLR_SIGNAL( UI_BASE_TASK_MSGR_SIGNAL );
    //The Msgr router event recieved.
    ui_base_msgr_if_process_msgr();
  }

  if ( sigs & UI_BASE_TASK_TIMER_SIGNAL )
  {
    UI_BASE_CLR_SIGNAL( UI_BASE_TASK_TIMER_SIGNAL );
    //The csg timer expired.
    ui_base_timer_expiry_hdlr(TRUE);
  }
  #endif
  
}

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
void ui_answer_call (void)
{
  cm_ans_params_s_type    *answer_params
                                     = (cm_ans_params_s_type*)
                                        modem_mem_alloc (sizeof(cm_ans_params_s_type), MODEM_MEM_CLIENT_MMODE);
  sys_sys_mode_e_type sys_mode = ui_incomCall_sys_mode;
  cm_call_mode_info_e_type mode_info=ui_incomCall_mode_info;
  
  if(answer_params == NULL) {
    TUI_MSG_ERROR("return from ui answer call, answer_params = NULL");
    return;
  }

  memset( answer_params, CM_CALL_CMD_PARAM_DEFAULT_VALUE, sizeof( cm_ans_params_s_type));

  TUI_MSG_HIGH_2("Thin UI Answering Call sys_mode=%d mode_info=%d",sys_mode,mode_info);

  switch (sys_mode) {
  case SYS_SYS_MODE_CDMA:
  case SYS_SYS_MODE_HDR:
    answer_params->info_type = CM_CALL_MODE_INFO_CDMA;
    answer_params->ans_params.cdma_ans.call_type = ui_incomCall_type;
    break;


  case SYS_SYS_MODE_GSM:
  case SYS_SYS_MODE_WCDMA:
  case SYS_SYS_MODE_GW:
#ifdef FEATURE_TDSCDMA
  case SYS_SYS_MODE_TDS:
#endif
#if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) || defined(FEATURE_TDSCDMA)
    answer_params->info_type = CM_CALL_MODE_INFO_GW_CS;
    answer_params->ans_params.gw_cs_ans.accept = TRUE;
    answer_params->ans_params.gw_cs_ans.call_type = ui_incomCall_type;
#endif /* (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */
    break;
  case SYS_SYS_MODE_LTE:
  #if defined (FEATURE_IP_CALL)
    if(mode_info == CM_CALL_MODE_INFO_IP || mode_info == CM_CALL_MODE_INFO_LTE)
    {
    answer_params->ans_params.ip_ans.call_type = ui_incomCall_type;
    answer_params->ans_params.ip_ans.accept = TRUE;
    answer_params->info_type = CM_CALL_MODE_INFO_IP;
    }
    else if (mode_info == CM_CALL_MODE_INFO_GW_CS)
    {
      answer_params->info_type = CM_CALL_MODE_INFO_GW_CS;
      answer_params->ans_params.gw_cs_ans.accept = TRUE;
      answer_params->ans_params.gw_cs_ans.call_type = ui_incomCall_type;
    }
    else
    {
      TUI_MSG_ERROR("Unhandled Mode info type for LTE");
    }
  #else
    TUI_MSG_HIGH("feature_ip_call not defined");
  #endif

    break;
  default:
    ERR("Invalid System Mode %d", sys_mode, 0, 0);
      /* Free allocated memory to heap */
     modem_mem_free(answer_params, MODEM_MEM_CLIENT_MMODE);
    return;
  }

  if (!cm_mm_call_cmd_answer(ui_call_err_cb, NULL, cm_client_id, ui_incomCall_id, answer_params)) {
    TUI_MSG_HIGH("Could not auto answer incoming call");
  }

  /* Free allocated memory to heap */
  modem_mem_free(answer_params, MODEM_MEM_CLIENT_MMODE);
}

/*===========================================================================
FUNCTION   ui_call_event_cb

DESCRIPTION
  When a call event occurs, this callback informs the user that it
  occurred.  In the case of the setup message, this handler will send
  CM the setup response with a default set of bearer capabilities.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Allows the MT call to continue.
===========================================================================*/
void ui_call_event_cb( cm_call_event_e_type call_event, /* call event */
                       const cm_mm_call_info_s_type *call_info_ptr
                                            /* call state information */
)
{
  /* The default bearer capability to use on incoming voice calls. */
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
  static cm_bearer_capability_s_type default_bearer_capability =
  {
     TRUE,                  /* validity */

     /* octet 3 */
  #ifdef FEATURE_GSM_HALF_RATE
     DUAL_RATE_FULL_RATE_PREFERRED,
  #else
     FULL_RATE_ONLY,
  #endif /* FEATURE_GSM_HALF_RATE */

     GSM,                   /* coding_standard : GSM */
     CIRCUIT,               /* transfert mode : circuit mode */
     ITC_SPEECH,            /* information transfer capability : speech */

  #ifdef FEATURE_TTY
     CTM_NOT_SUPPORTED,     /*ctm*/
  #endif /* FEATURE_TTY */

     /*  support for 3 speech versions: EFR, FR and AMR  */
  #ifdef FEATURE_GSM_AMR
     /* octet 3a */
     GSM_FR_SPEECH_VER_3,  /* GSM AMR Full Rate */

     /* octet 3c */
     GSM_FR_SPEECH_VER_2,  /* GSM Enhanced Full Rate  */

     /* octet 3b */
     GSM_FR_SPEECH_VER_1,  /* GSM FR */
  #else
     /* octet 3a */
     GSM_FR_SPEECH_VER_2,

     /* octet 3b */
     GSM_FR_SPEECH_VER_1,

     /* octet 3c */
  #ifdef FEATURE_GSM_HALF_RATE
     GSM_HR_SPEECH_VER_1, /* GSM Half Rate Version 0 or 1 */
  #else
     GSM_INVALID_SPEECH_VER,
  #endif /* FEATURE_GSM_HALF_RATE */
  #endif /* FEATURE_GSM_AMR */

  #ifdef FEATURE_GSM_EXT_SPEECH_PREF_LIST
  #ifdef FEATURE_GSM_AMR
     /* octet 3d */
     GSM_HR_SPEECH_VER_3,

     /* octet 3e */
     GSM_HR_SPEECH_VER_1,
  #else
     /* octet 3d */
     GSM_INVALID_SPEECH_VER,

     /* octet 3e */
     GSM_INVALID_SPEECH_VER,
  #endif /* FEATURE_GSM_AMR */
  #endif /* FEATURE_GSM_EXT_SPEECH_PREF_LIST */

     /* octet 4 */
     0x00,                  /* compression */
     0x00,                  /* structure */
     0x01,                  /* duplex mode : full duplex */
     0x00,                  /* configuration */
     0x00,                  /* NIRR */
     0x00,                  /* establishment */

     /* octet 5 */
     0x00,                  /* access id */
     0x00,                  /* rate adaption : no rate adaption */
     0x00,                  /* signalling access control */

     /* octet 5a */
     0x00,                  /* other_ITC */
     0x00,                  /* other_rate_adaption */

     /* octet 5b */
     0x00,                  /* rate_adaption_header */
     0x00,                  /* multiple_frame_establishment */
     0x00,                  /* mode_of_operation */
     0x00,                  /* logical_link_identifier_negotiation */
     0x00,                  /* assignor_assignee */
     0x00,                  /* inband_outband_negotiation */

     /* octet 6 */
     0x01,                  /* layer1 id */
     0x00,                  /* user info layer 1 protocol */
     0x00,                  /* sync - async */

     /* octet 6a */
     0x00,                  /* number of stop bits */
     0x00,                  /* negotiation */
     0x00,                  /* number of data bits */
     0x00,                  /* user rate */

     /* octet 6b */
     0x00,                  /* intermediate rate */
     0x00,                  /* NIC on Tx */
     0x00,                  /* NIC on Rx */
     0x00,                  /* parity */

     /* octet 6c */
     0x00,                  /* connection element */
     0x00,                  /* modem type */

     /* These values are for GSM Phase2+/WCDMA */
     /* octet 6d */
     OMT_UNDEFINED,         /* Other Modem type */
     FNUR_UNDEFINED,        /* Fixed Network User Rate */

     /* octet 6e */
     ACC_NONE,                    /* Acceptable Channel Codings */
     MAX_NUMBER_OF_TCH_UNDEFINED, /* Max Number of TCHs */

     /* octet 6f */
     UIMI_UNDEFINED,              /* User Initiated Modifiaction Indication */
     WAIUR_UNDEFINED,             /* Wanted Air User Rate */

     /* octet 6g */
     0x00,                        /* acceptable_channel_coding_extended */
     0x00,                        /* asymmetry_indication - EDGE channel coding */

     /* octet 7 */
     0x02,                        /* layer 2 ID */
     0x00,                        /* user info layer 2 protocol */

     /* For logging purpose */
     0                            /* MT fall-back rate */
  };
#else
  static cm_bearer_capability_s_type default_bearer_capability = {0};
#endif //defined (FEATURE_GSM) || defined (FEATURE_WCDMA)

  /* Inform the user via QXDM. */
  TUI_MSG_HIGH_1("Received a call event %d", call_event);

  switch (call_event)
  {
    case CM_CALL_EVENT_SETUP_IND:
      {
        cm_setup_res_params_s_type setup_params;

        if (call_info_ptr->call_type != CM_CALL_TYPE_VOICE)
          return;

        TUI_MSG_HIGH_3("ThinUI setup_resp req = %d, bAutoAnswer = %d, ui_tty_mode = %d", bThinUI_setup_resp_req, bAutoAnswer, ui_tty_mode);
        if( bThinUI_setup_resp_req )
        {
          memset(&setup_params, CM_CALL_CMD_PARAM_DEFAULT_VALUE,
                 sizeof(setup_params));
          setup_params.accept = TRUE;

          setup_params.bearer_capability_1 = default_bearer_capability;

          if ( ui_tty_mode != UI_TTY_MODE_OFF )
          {
            setup_params.bearer_capability_1.ctm = CTM_SUPPORTED;
          }
          cm_mm_call_cmd_setup_res(ui_call_err_cb, NULL, cm_client_id,
                                 call_info_ptr->call_id, &setup_params);
        }
        else
        {
          return;
    }
      }
      break;

    case CM_CALL_EVENT_ORIG:
        TUI_MSG_HIGH_1("CM_CALL_EVENT_ORIG Number voice calls %d", numberOfVoiceCalls);
      break;

    // Incoming call, start ringing and autoanswer timer
    case CM_CALL_EVENT_INCOM:
      {
        //bAutoAnswer is updated in ui_init.  If the auto answer option is modified in the nv,
        //reset of the phone is required to update the bAutoAnswer value.  Getting the auto answer
        //from nv here directly is not recommending because it will cause unacceptable delay.
        if (bAutoAnswer) {
          /* Check if any call exists */
          if( ui_incomCall_id != CM_CALL_ID_UNASSIGNED ){

          /* Process on going call first then attend the incoming call */
          (void) rex_clr_timer( &ui_autoanswer_timer );
          ui_answer_call();

          }
          //auto answer is enabled.  So, answer it after some time.
          (void) rex_set_timer( &ui_autoanswer_timer, UI_AUTOANSWER_TIME );
        }
        ui_incomCall_id = call_info_ptr->call_id;
        ui_incomCall_type = call_info_ptr->call_type;
        ui_incomCall_sys_mode = call_info_ptr->sys_mode;
        ui_incomCall_mode_info = call_info_ptr->mode_info.info_type;
      }
      break;

    case CM_CALL_EVENT_ANSWER:
      (void) rex_clr_timer( &ui_autoanswer_timer );
      break;

    case CM_CALL_EVENT_END:
    case CM_CALL_EVENT_END_REQ:
      /* Clear autoanswer timer for existing call */
      if( call_info_ptr->call_id == ui_incomCall_id ){
        (void) rex_clr_timer( &ui_autoanswer_timer );
        /* Clear incoming call id */
        ui_incomCall_id = CM_CALL_ID_UNASSIGNED;
      }
      break;


    default:
      break;
  }
}
/*===========================================================================
FUNCTION   ui_call_err_cb

DESCRIPTION
  This command is called after a call command is issued.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
void ui_call_err_cb(void *data_ptr, cm_call_cmd_e_type call_cmd,
                    cm_call_cmd_err_e_type call_cmd_err)
{
  TUI_MSG_HIGH_2("Call Command Callback returned %d for command %d", call_cmd_err, call_cmd);
}

/*===========================================================================
FUNCTION UI_GET_NV

DESCRIPTION
  Get an item from the nonvolatile memory.  Handles nonactive items by
  providing a default value.

RETURN VALUE
  The NV return code, except for NV_NOTACTIVE_S, which is handled
  internally.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from the UI task.
===========================================================================*/
nv_stat_enum_type ui_get_nv(
  nv_items_enum_type item,        /* which item */
  nv_item_type *data_ptr          /* pointer to space for item */
)
{
  static nv_cmd_type nvi;           /* nv command buffer */

  nvi.tcb_ptr = ui_tcb_ptr;          /* notify this task when done */
  nvi.sigs = UI_NV_SIG;
  nvi.done_q_ptr = NULL;             /* command goes on no queue when done */

  nvi.item = item;                 /* item to read */
  nvi.cmd = NV_READ_F;

  if ( item == NV_DIAL_I ) {       /* check for speed dial */
    data_ptr->dial.address -= 1;   /* update to real nv address */
  }

  /* Set up NV so that it will read the data into the correct location */
  nvi.data_ptr = data_ptr;


  /* Clear the return signal, call NV, and wait for a response */
  (void) rex_clr_sigs( ui_tcb_ptr, UI_NV_SIG );
  nv_cmd( &nvi );
#ifndef CM_UNIT_TEST
  ui_wait(UI_NV_SIG);
#endif
  if (nvi.status == NV_NOTACTIVE_S) {
     TUI_MSG_HIGH_1("Nonactive NV item 0x%x", nvi.item);
  } else {
    if( nvi.status != NV_DONE_S ) {
      ERR( "NV Read Failed Item %d Code %d", nvi.item, nvi.status, 0 );
    }
  }

  if ( item == NV_DIAL_I ) {       /* check for speed dial */
    data_ptr->dial.address += 1;   /* restore to user address */
  }

  return( nvi.status );

}

/*===========================================================================
FUNCTION ui_subscription_cb

DESCRIPTION
  This function is called at the completion of the subscription available
  and not available commands.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ui_subscription_cb(void *data_block, cm_ph_cmd_e_type ph_cmd,
    cm_ph_cmd_err_e_type ph_cmd_err)
{
  if (ph_cmd_err != CM_PH_CMD_ERR_NOERR)
    ERR("Error %d to subscription command %d", ph_cmd_err, ph_cmd, 0);
}

/*===========================================================================
FUNCTION HandleStopSig

DESCRIPTION
  This function handles offline signals received by UI task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void HandleStopSig(void)
{

  RCEVT_SIGEX_SIGREX term_sig;
  RCINIT_GROUP ui_group = RCINIT_GROUP_NONE;

  term_sig.signal = ui_tcb_ptr;
  term_sig.mask = TASK_STOP_SIG;

  /* Lookup for sequence group */
  ui_group = rcinit_lookup_group_rextask(ui_tcb_ptr);

  TUI_MSG_MED("UI received stop sig ");
  /* Shutdown runtime registration */
#ifndef CM_UNIT_TEST
  rcinit_unregister_term_group(ui_group, RCEVT_SIGEX_TYPE_SIGREX, &term_sig);
#endif
  rcinit_handshake_term();
  return;
} /* HandleStopSig */

/*===========================================================================
FUNCTION HandleOfflineSig

DESCRIPTION
  This function handles offline signals received by UI task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void HandleOfflineSig(void)
{
  (void) rex_clr_sigs( rex_self(), TASK_OFFLINE_SIG );
  TUI_MSG_MED("UI got offline sig");
  return;
} /* HandleOfflineSig */


#ifdef FEATURE_COREAPP_CSG
/*===========================================================================*/
void ui_base_msgr_init( void )
{
  errno_enum_type msgr_err;

  /*-------------------------------------------------------------------------
    Initialize the msgr queues and link items onto the command free queue
    Also perform msgr init and registration
  -------------------------------------------------------------------------*/
  (void) q_init( &ui_base_msgr_q );

  msgr_err = msgr_client_create( &ui_base_msgr_client );
  if(msgr_err != E_SUCCESS)
  {
    TUI_MSG_HIGH_1("msgr_client_create ret %d", msgr_err);
  }
#ifndef CM_UNIT_TEST
  ASSERT( msgr_err == E_SUCCESS );
#endif

  msgr_err = msgr_client_set_rex_q( &ui_base_msgr_client,
                                    ui_tcb_ptr,
                                    (rex_sigs_type)(UI_BASE_TASK_MSGR_SIGNAL),
                                    &ui_base_msgr_q,
                                    MSGR_NO_QUEUE,
                                    UI_BASE_OFFSETOF(ui_base_msgr_type, cmd),
                                    MSGR_NO_OFFSET,
                                    sizeof( ui_base_msgr_type ) );
  if(msgr_err != E_SUCCESS)
  {
    TUI_MSG_MED_1("TUI msgr_client_set_rex_q ret %d", msgr_err);
  }
  #ifndef CM_UNIT_TEST
    ASSERT( msgr_err == E_SUCCESS );
  #endif
  ui_base_msgr_register( &ui_base_msgr_client );
}


/*===========================================================================*/
/*================Return client id with which UI base registered to============================*/
/*===========================================================================*/


int ui_base_get_cm_client_id(void)
{
  TUI_MSG_HIGH_1("ui_base_get_cm_client_id ret %d", cm_client_id);
  return (cm_client_id);
}
#endif
