/*===========================================================================

                        QM_TIMER.C 

DESCRIPTION

  QMI_MMODE source file for Timer module.

Copyright (c) 2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header:
  $DateTime:
  $Author:
  

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/04/13    sk     Initial version
===========================================================================*/
#include "comdef.h"
#include "modem_mem.h"
#include "qm_timer.h"
#include "msg.h"
#include "mcfg_fs.h"
#include "qmi_voice_call_list.h"
#include "qmi_mmode_task_cmd.h"

#define INCOM_EVT_RESEND_EFS_FILE "/nv/item_files/modem/mmode/qmi/incom_evt_resend"
#define INVALID_INCOM_EVT_RETRY_CNT 0xFF

struct qm_timer_evt_cb_s
{
  uint32  timer_id;
};

void qm_timer_cb
( 
  timer_cb_data_type  user_data 
);

void qm_timer_cb_handler
(
  qmi_mmode_cmd_enum_type cmd_type, 
  void *user_data 
);

struct qm_timer_global_s
{
  struct
  {
    boolean       is_efs_read;
    boolean       is_valid;
    timer_type    timer;
    uint16        interval; //in millisec
    uint8         max_retry;
    uint8         retry_cnt;
    enum qm_subs_e  subs;
  } incom_evt_resend;
};

//===========================================================================
// GLOBALS
//===========================================================================
static struct qm_timer_global_s timer_global;


//===========================================================================
// Function declarations
//===========================================================================

/*===========================================================================
  FUNCTION QM_TIMER_INIT()

  DESCRIPTION
    This function initializes the timer module

  PARAMETERS

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_timer_init( void )
{

  /* Common init */
  memset(&timer_global, 0, sizeof(timer_global));
  qmi_mmode_set_cmd_handler( QMI_MMODE_TIMER_EVT_CB,  qm_timer_cb_handler);

  /* Init the INCOM EVT RESEND data */
  timer_global.incom_evt_resend.retry_cnt = INVALID_INCOM_EVT_RETRY_CNT;
  timer_global.incom_evt_resend.subs = QM_SUBS_PRI;
}


/*===========================================================================
  FUNCTION QM_TIMER_CHECK_AND_START_INCOM_EVENT_RESEND()

  DESCRIPTION
    This function checks if the iNCOMING EVT RESEND TIMER is enabled and then starts it.
    
  PARAMETERS
    NONE

  RETURN VALUE
    void
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qm_timer_check_and_start_incom_event_resend
(
  enum qm_subs_e subs
)
{
  boolean incom_call_present = TRUE;

  if ( timer_global.incom_evt_resend.is_valid )
  {
    if( !qmi_voice_call_list_is_pure_incom_call_present((uint8)subs) )
    {
      incom_call_present = FALSE;
      QM_MSG_HIGH("No incoming calls. Timer will not start");
      
      /* If the timer is already running, Stop it as there is no incoming call anymore */
      if( qm_is_timer_running(QM_TIMER_ID_INCOM_EVT_RESEND) )
      {
        qm_timer_stop(QM_TIMER_ID_INCOM_EVT_RESEND);
      }
    }
    
    if( incom_call_present ) 
    {
      if( timer_global.incom_evt_resend.retry_cnt == INVALID_INCOM_EVT_RETRY_CNT )
      {
        /* Prepare the retry counter to start the retransmission */ 
        timer_global.incom_evt_resend.retry_cnt = 0;
      }
      else
      {
        timer_global.incom_evt_resend.retry_cnt++;
      }
      
      if(timer_global.incom_evt_resend.retry_cnt < timer_global.incom_evt_resend.max_retry)
      {
        timer_set( &timer_global.incom_evt_resend.timer, timer_global.incom_evt_resend.interval, 0, T_MSEC);
        timer_global.incom_evt_resend.subs = subs;

        QM_MSG_HIGH_3("Started INCOM EVT RESED timer %d ms,cnt %d, max %d", timer_global.incom_evt_resend.interval, (timer_global.incom_evt_resend.retry_cnt+1), timer_global.incom_evt_resend.max_retry);
      }
      else
      {
        /* Exhausted the retries. Reset the retry counter */
        QM_MSG_HIGH("Retries completed for incoming event transmit");
        timer_global.incom_evt_resend.retry_cnt = INVALID_INCOM_EVT_RETRY_CNT;
        timer_global.incom_evt_resend.subs = QM_SUBS_PRI;
      }
    }
  }

}/* qm_timer_start_incom_event_resend */


/*===========================================================================
  FUNCTION QM_TIMER_IS_INCOM_EVT_RESEND_ENABLED()

  DESCRIPTION
    This function checks if the iNCOMING EVT RESEND TIMER setting is enabled.
    
  PARAMETERS
    NONE

  RETURN VALUE
    TRUE if timer is initialized with valid interval, otherwise FALSE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qm_timer_is_incom_evt_resend_enabled(void)
{
  mcfg_fs_status_e_type status = MCFG_FS_STATUS_OK;
  uint8 buf[3];

  if( !timer_global.incom_evt_resend.is_efs_read ) // we haven't read the timer interval yet, read EFS file
  {
    // read efs file and mark is_efs_read as TRUE so that EFS file is not attempted to read next time
    status = mcfg_fs_read(INCOM_EVT_RESEND_EFS_FILE, (byte *)buf, sizeof(buf), MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_0 );
    timer_global.incom_evt_resend.is_efs_read = TRUE;
 
    if( status == MCFG_FS_STATUS_OK )
    {
      memscpy(&timer_global.incom_evt_resend.interval, 
              sizeof(timer_global.incom_evt_resend.interval),
              buf, sizeof(buf));
      QM_MSG_HIGH_1("efs_get Incom evt timer read success interval %d secs", timer_global.incom_evt_resend.interval);
 
      if( (timer_global.incom_evt_resend.interval > 0) && (timer_global.incom_evt_resend.interval != 0xFFFF) ) 
      {
        timer_global.incom_evt_resend.max_retry = buf[2];
        if((timer_global.incom_evt_resend.max_retry > 0) && (timer_global.incom_evt_resend.max_retry != 0xFF))
        {
          timer_def( &timer_global.incom_evt_resend.timer,  // timer object
                     NULL,                                  // timer group (needed if timer is critical and needs to expire while device is in sleep)
                     NULL,                                  // task to be called when timer expires (needed when signal parameter is provided, not needed with callback function)
                     0,                                     // signal to be generated when timer expires
                     qm_timer_cb,                                  //call back function
                     (timer_cb_data_type)QM_TIMER_ID_INCOM_EVT_RESEND   // timer id
                   );

          QM_MSG_HIGH("incom_evt_resend timer defined");
          timer_global.incom_evt_resend.is_valid = TRUE;
        }
      }
    }
    else
    {
      QM_MSG_ERROR_1("msfg_fs_read timer read failed status %d", status);
    }
  }

  return timer_global.incom_evt_resend.is_valid;

}



/*===========================================================================
  FUNCTION QM_TIMER_PROCESS_INCOM_EVT_RESEND()

  DESCRIPTION
    This function first checks whether or not the incoming call is not answered. If the call is unanswered, the indication
    is sent again and timer is re-started.
    
  PARAMETERS
    NONE

  RETURN VALUE
    NONE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qm_timer_process_incom_evt_resend(void)
{
  /* Check if the timer is forcibly stopped already. If not then resend the indication */
  if(timer_global.incom_evt_resend.retry_cnt != INVALID_INCOM_EVT_RETRY_CNT)
  {
    if(!qmi_voice_call_list_is_pure_incom_call_present((uint8)timer_global.incom_evt_resend.subs) )
    {
      QM_MSG_HIGH("Incoming event answered or ended");
      /* Clear the retries as there is no need of retransmitting the incoming event */
      timer_global.incom_evt_resend.retry_cnt = INVALID_INCOM_EVT_RETRY_CNT;
      timer_global.incom_evt_resend.subs = QM_SUBS_PRI;
    }
    else
    {
      qmi_voice_call_list_report_all_call_status_changed(timer_global.incom_evt_resend.subs);
      /* Timer will be started while sending the indication */
    }
  }

}/* qm_timer_process_incom_evt_resend */


/*===========================================================================
  FUNCTION QM_TIMER_IS_ENABLED()

  DESCRIPTION
    This function checks if the timer is enabled.
    
  PARAMETERS
    NONE

  RETURN VALUE
    TRUE if timer is initialized with valid interval, otherwise FALSE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qm_timer_is_enabled( enum qm_timer_id_e timer_id)
{
  boolean ret = FALSE;

  switch(timer_id)
  {
    case QM_TIMER_ID_INCOM_EVT_RESEND:
     ret = qm_timer_is_incom_evt_resend_enabled();
      break;

    default:
      QM_MSG_ERROR_1("Invalid timer id recv %d", timer_id);
      break;
  }
  
 return ret;
}

/*===========================================================================
  FUNCTION QM_TIMER_CB_HANDLER()

  DESCRIPTION
    Callback function handler when timer expires.
    
  PARAMETERS
    NONE

  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_timer_cb_handler
(
  qmi_mmode_cmd_enum_type cmd_type, 
  void *user_data 
)
{
  struct qm_timer_evt_cb_s *timer_data = (struct qm_timer_evt_cb_s *)user_data;

  if(timer_data != NULL)
  {
    switch (timer_data->timer_id)
    {
      case QM_TIMER_ID_INCOM_EVT_RESEND:
        qm_timer_process_incom_evt_resend();
        break;

      default:
        QM_MSG_ERROR_1("unhandled timer id %d", timer_data->timer_id);
        break;
    }
  }
  else
  {
    QM_MSG_ERROR("qm_timer_cb_handler processing timer_data is NULL");
  }
}/* qm_timer_cb_handler */


/*===========================================================================
  FUNCTION QM_TIMER_CB()

  DESCRIPTION
    Callback function when timer expires.
    
  PARAMETERS
    NONE

  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_timer_cb
( 
  timer_cb_data_type  user_data 
)
{

  struct qm_timer_evt_cb_s  *ptr = NULL;

  ptr = modem_mem_alloc( sizeof(*ptr), MODEM_MEM_CLIENT_QMI_MMODE );

  if (ptr) 
  {
    ptr->timer_id = (uint32)user_data;         // user data contains the timer event type, so convert to uint32 and assign to evt_type

    qmi_mmode_send_cmd( QMI_MMODE_TIMER_EVT_CB, ptr);

    QM_MSG_HIGH("QMI_MMODE_TIMER_EVT_CB to QMI MMODE task");
  }
  else 
  {
    QM_MSG_ERROR("qm_timer_cb() modem_mem_alloc failed");
  }

}


/*===========================================================================
  FUNCTION QM_TIMER_START()

  DESCRIPTION
     This functions starts running a timer if it is enabled.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_timer_start (enum qm_timer_id_e timer_id, enum qm_subs_e subs)
{

  if(qm_timer_is_enabled(timer_id))
  {
    switch(timer_id)
    {
      case QM_TIMER_ID_INCOM_EVT_RESEND:
        qm_timer_check_and_start_incom_event_resend(subs);
        break;

      default:
        break;
    }
  }
}



/*===========================================================================
  FUNCTION QM_TIMER_STOP()

  DESCRIPTION
    This functions stops the running timer  if it is enabled already.
    
  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_timer_stop (enum qm_timer_id_e timer_id)
{

  if(qm_is_timer_running(timer_id))
  {
    switch(timer_id)
    {
      case QM_TIMER_ID_INCOM_EVT_RESEND:
        timer_clr( &timer_global.incom_evt_resend.timer, T_MSEC); // stops the active timer
        timer_global.incom_evt_resend.retry_cnt = INVALID_INCOM_EVT_RETRY_CNT;
        timer_global.incom_evt_resend.subs = QM_SUBS_PRI;
        QM_MSG_HIGH("Stopped INCOM EVT RESED timer");
        break;
      default:
        break;
    }
  }
  else
  {
    QM_MSG_HIGH_1("Timer %d not running.", timer_id);
  }

}


/*===========================================================================
  FUNCTION QM_IS_TIMER_RUNNING()

  DESCRIPTION
    This functions determines if the timer is running.

  PARAMETERS
    None

  RETURN VALUE
    TRUE if timer is active, FALSE otherwise (even if timer is paused)

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qm_is_timer_running(enum qm_timer_id_e timer_id)
{
  boolean run_status = FALSE;

  switch(timer_id)
  {
    case QM_TIMER_ID_INCOM_EVT_RESEND:
      if(timer_global.incom_evt_resend.is_valid)
      {
        run_status = timer_is_active( &timer_global.incom_evt_resend.timer );
      }
      break;

    default:
      QM_MSG_ERROR_1("Invalid timer id recv %d", timer_id);
      break;
  }

  return run_status;
}


