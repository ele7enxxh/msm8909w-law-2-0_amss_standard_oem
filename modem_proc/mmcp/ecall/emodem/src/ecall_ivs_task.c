/*===========================================================================

                           E C A L L _ T A S K . C

DESCRIPTION
  eCall_ivs task would wait for signals and it does the processing of the commands
  received from application as well the vocoder frames received. 

REFERENCES
  

EXTERNALIZED FUNCTIONS
       - ecall_ivs_task(dword ignored)
       - void ecall_put_cmd( ecall_cmd_type  *cmd_ptr)
       - ecall_cmd_type *ecall_get_cmd_buf(void)

INITIALIZATION AND SEQUENCING REQUIREMENTS

 
Copyright (c) 2009-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary. 

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/ecall/emodem/src/ecall_ivs_task.c#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $ 
===========================================================================*/

/*===========================================================================

                         I N C L U D E   F I L E S

===========================================================================*/

#include "customer.h"

#ifdef FEATURE_ECALL_IVS
#include "comdef.h"
#include "rex.h"
#include "err.h"
#include "msg.h"
#include "queue.h"
#include "fs_public.h"
#include "string.h"

#include "ecall_ivs_task.h"
#include "ecall_modem_ivs.h" 
#include "ecall_sim.h"
#include "ecall_efs.h"

#ifndef FEATURE_ECALL_LCU45
#include "npa.h"
#endif /* FEATURE_ECALL_LCU45 */

#ifdef FEATURE_ECALL_HAS_V1_HEADER
#include "task.h"
#else /* Nikel or Dime */
#include "aprv2_packet.h"
#include "apr_errcodes.h"
#include "aprv2_api_inline.h"
#endif /* FEATURE_ECALL_HAS_V1_HEADER */

#ifdef FEATURE_MODEM_RCINIT
#include "rcinit.h"
#endif
#ifndef FEATURE_MODEM_RCINIT_PHASE2
#include "tmc.h"
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */

#ifdef FEATURE_ECALL_HAS_DOG_HB
#include "dog_hb_rex.h"
#else /* Legacy Dog */
#include "dog.h"
#endif /* FEATURE_ECALL_HAS_DOG_HB */

/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/

#define ECALL_CMD_BUF_COUNT     8
#define ECALL_IVS_PERMISSIONS   0777

/*===========================================================================

                         I N T E R N A L    D A T A

===========================================================================*/

/* queue uses static memory than dynamic memory as HLAP spec
   says we need to originate call
 
   Ref TC 278 WI 002278243.091009 section 6
   NOTE:	Should the MSD not be sent or received for any reason then the eCall
   continues as a normal 112/E112 emergency call and is afforded the same
   protection and priority as a Teleservice 12 [Ref.13] emergency voice call. 
*/

static ecall_cmd_type   ecall_cmd_buf[ECALL_CMD_BUF_COUNT];
static q_type           ecall_cmd_free_q;
static q_type           ecall_cmd_q;

#ifdef FEATURE_ECALL_HAS_MMOC
#include "msgr_rex.h"
#define ECALL_IVS_OFFSETOF(type,member)    (((char*)(&((type*)1)->member))-((char*)1))
#define ECALL_IVS_MSGR_Q_SIZE    20
static q_type                    ecall_ivs_msgr_q;
static q_type                    ecall_ivs_msgr_free_q;
static ecall_ivs_msgr_cmd_s_type ecall_ivs_msgr_bufs[ECALL_IVS_MSGR_Q_SIZE];
static msgr_client_t             ecall_ivs_msgr_client;
static msgr_id_t                 ecall_ivs_msgr_rex_q_id;
#endif /* FEATURE_ECALL_HAS_MMOC */

/* Define a variable to hold the ECALL_IVS task TCB.  This is done for
   a several reasons:
    1) The return value of rex_self() never changes at runtime, so
       there is nothing gained by repeatedly fetching it.
    2) Directly using ecall_ivs_tcb requires that 'task.h' be included.
       It is now prohibited to include 'task.h'.
    3) The use of rex_self() cannot be used in any context where
       the ecall_ivs_tcb is specifically required (such as when a command
       is placed into a queue and the task signal is set).
    4) A task's tcb is not available if the task is not started.
       Worse, signaling a task that isn't running causes an exception.
    5) The variable allows access to the correct tcb in case 3 and
       provides state to use in determining if the task is running.
*/
/* To be initialized to rex_self() when task is running */
static rex_tcb_type *ecall_ivs_myself = NULL;

/* ----------------------------
** Define watchdog report handle
** ---------------------------- */
#ifdef FEATURE_ECALL_HAS_DOG_HB
#undef   DOG_ECALL_IVS_RPT
#define  DOG_ECALL_IVS_RPT        ecall_ivs_dog_rpt_var
static   dog_report_type          ecall_ivs_dog_rpt_var  = 0;     /* Initial Safety */
#else /* Legacy Dog */
/* Dynamic Dog Interface */
#ifdef DOG_DYNAMIC_API   
  #undef  DOG_ECALL_IVS_RPT
  #define DOG_ECALL_IVS_RPT       ecall_ivs_dog_rpt_var
  static dog_report_type          ecall_ivs_dog_rpt_var  = 0;     /* Initial Safety */

  #undef  DOG_ECALL_IVS_RPT_TIME
  #define DOG_ECALL_IVS_RPT_TIME  ecall_ivs_dog_rpt_time_var
  static uint32                   ecall_ivs_dog_rpt_time_var   = 0xffff; /* Initial Safety */
#endif   /* DOG_DYNAMIC_API */
#endif /*FEATURE_ECALL_HAS_DOG_HB */

LOCAL struct
{
  #ifndef FEATURE_ECALL_HAS_DOG_HB /* Legacy Dog */
  rex_timer_type     ecall_ivs_rpt_timer;        /* Watchdog timer   */
  #endif /* Legacy Dog */
  rex_timer_type     ecall_ivs_voc_timer;        /* Vocoder timer    */
  boolean            ecall_nv_enable;            /* NV item EFS file */
  #ifndef FEATURE_ECALL_LCU45
  npa_client_handle  ecall_cpu_hdl;              /* Vote for Mips    */
  #endif /* FEATURE_ECALL_LCU45 */
} ecall_ivs;


/*===========================================================================

                          INTERNAL FUNCTION DEFINITIONS

===========================================================================*/


/*==========================================================================

  FUNCTION    ECALL_PROCESS_CMD

  DESCRIPTION
    Processes the received commands from the APP task and the vocoder
    and returns the processed command buffers to the free queue.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static void ecall_process_cmd
(
  void
)
{
  ecall_cmd_type *cmd_ptr = NULL;
  uint8 num_rx_pcm_cmd_processed = 0;

  while((cmd_ptr = (ecall_cmd_type *) q_get(&ecall_cmd_q)) != NULL)
  {
    switch(cmd_ptr->cmd_id)
      {
        case ECALL_CMD_PROCESS_RX_PCM:
             ECALL_MSG_0( ECALL_MED, "started RX PCM Process" );
             /* Processes the PCM data received every 20ms and puts TX pcm in the ecall_pcm_cmd_q */
             ecall_ivs_process ( cmd_ptr->data.pcm, sizeof(cmd_ptr->data.pcm) );
             ECALL_MSG_0( ECALL_MED, "Finished RX PCM Process" );

             /* 2+ frames per visit may indicate a loss of synch between the Rx & Tx timing. */
             num_rx_pcm_cmd_processed++;
             break;

        case ECALL_CMD_MODEM_START_TX_MSD:
        
             #ifndef FEATURE_ECALL_LCU45
             if(ecall_ivs.ecall_cpu_hdl != NULL)
             {
               /* Issue a work request of MAX MIPS to the CPU */
               npa_issue_required_request( ecall_ivs.ecall_cpu_hdl, NPA_MAX_STATE );
             }
             else
             {
               ECALL_MSG_0( ECALL_ERROR, "couldn't request for ECALL_IVS mips " );
             }
             #endif /* FEATURE_ECALL_LCU45 */

             /* Start the ecall_modem by registering the rx_output to listed fo DL frames */
             ECALL_MSG_0( ECALL_HIGH, "[IVS] Sending cmd_ptr->tx_mode = %d to process_start " );
             ecall_ivs_process_start(cmd_ptr->data.ecall_msd); 
             break;

        case ECALL_CMD_MODEM_ABORT_TX:
             /* Abort the UL and continue to listen for DL frames, APP is responsible for
                to keep the msd or discard it*/
             ECALL_MSG_1( ECALL_HIGH, "ECALL_CMD_MODEM_ABORT_TX  discard msd %d",
                                       cmd_ptr->data.discard_msd );
             ecall_ivs_process_abort_tx(cmd_ptr->data.discard_msd);
             break;

        case ECALL_CMD_MODEM_ABORT_ALL:
             #ifndef FEATURE_ECALL_LCU45
             if(ecall_ivs.ecall_cpu_hdl != NULL)
             {
               /* cancel work request */
               npa_cancel_request( ecall_ivs.ecall_cpu_hdl ); 
             }
             #endif /* FEATURE_ECALL_LCU45 */
             /* Abort the UL data as well as listening for DL messages. Also discards the MSD*/
             ECALL_MSG_0( ECALL_HIGH, "ECALL_CMD_MODEM_ABORT_ALL  " );
             ecall_ivs_process_abort_all();
             break;

        case ECALL_CMD_SIM_CONFIG_UPDATE:
             ecall_process_sim_config_update(cmd_ptr->data.ecall_operating_mode);
             break;

        case ECALL_CMD_SIM_CLIENT_ID_REG_SUCCESS:
             ecall_process_sim_client_id_reg_success(cmd_ptr->data.gsdi_client_id);
             break;

        case ECALL_CMD_SIM_CLIENT_SESSION_EVENTS:
             ecall_process_sim_events(cmd_ptr->data.sim_data.event);
             break;

        case ECALL_CMD_SIM_READ_SUCCESS:
             ecall_process_sim_read_success( cmd_ptr->data.sim_data.bit89_value,
                                             cmd_ptr->data.sim_data.bit2_value,
                                             cmd_ptr->data.sim_data.bit4_value,
                                             cmd_ptr->data.sim_data.appInfo_ptr );
             break;

        case ECALL_CMD_CLIENT_REG:
             /* Register the clients for modem status notification */ 
             ecall_process_client_register(cmd_ptr->data.client_ptr);
             break;

        case ECALL_CMD_CLIENT_DEREG:
             /* de-register the client */ 
             ecall_process_client_deregister(cmd_ptr->data.clientid);
             break;

        case ECALL_CMD_CLIENT_NOTIFY:
             /* de-register the client */                                
             ecall_process_client_notify(cmd_ptr->data.ivs_status);
             break;

        case ECALL_CMD_SIM_READ_FAILED:
             ecall_process_sim_read_failed( cmd_ptr->data.sim_data.appInfo_ptr );  
             break;

        case ECALL_CMD_SET_EST_FDN_ENABLED:
             ecall_sim_set_est_fdn_enabled(cmd_ptr->data.sim_data.est_fdn_enabled,
                                           cmd_ptr->data.sim_data.appInfo_ptr);
             break;

        case ECALL_CMD_SIM_SESSION_OPEN_SUCCESS:
             ecall_sim_process_session_open_success(cmd_ptr->data.mmgsdi_session_open_data.session_type,
                                                    cmd_ptr->data.mmgsdi_session_open_data.session_id, 
                                                    cmd_ptr->data.mmgsdi_session_open_data.slot_id,
                                                    cmd_ptr->data.mmgsdi_session_open_data.app_type);
             break;

        case ECALL_CMD_SET_CPU_HDL:
          #ifndef FEATURE_ECALL_LCU45
          ecall_ivs.ecall_cpu_hdl = cmd_ptr->data.cpu_hdl;
          if(ecall_ivs.ecall_cpu_hdl  == NULL)
          {
            ECALL_MSG_0( ECALL_ERROR, "ECALL_IVS NPA create client failed" );
          }
          #endif /* FEATURE_ECALL_LCU45 */
          break;
          
        case ECALL_CMD_APR_PACKET:
          #if ( (defined FEATURE_ECALL_HAS_VOICE_A) || (defined FEATURE_ECALL_HAS_VOICE_B) )
          ecall_process_apr_packet(cmd_ptr->data.apr_packet);
          #else
          ECALL_MSG_0( ECALL_ERROR, "VOICE_A or VOICE_B not defined, but receives ECALL_CMD_APR_PACKET" );
          #endif /* FEATURE_ECALL_HAS_VOICE_A || EFATURE_ECALL_HAS_VOICE_B */
          break;

        case ECALL_CMD_SET_AND_SEND_OPERATING_MODE:
          ECALL_MSG_1( ECALL_HIGH, "Rx ECALL_CMD_SET_AND_SEND_OPERATING_MODE with as_id %d",
                                    cmd_ptr->data.as_id );
          ecall_sim_process_set_and_send_operating_mode(cmd_ptr->data.as_id);
          break;

        case ECALL_CMD_UPDATE_CM_SUBS_INFO:
          ECALL_MSG_0( ECALL_HIGH, "Rx ECALL_CMD_UPDATE_CM_SUBS_INFO" );
          ecall_sim_process_update_cm_subs_info();
          break;

        default:
          ECALL_MSG_1( ECALL_ERROR, "Unknown command %d passed to eCall task",
                                     cmd_ptr->cmd_id );
          break;
      }

    /* Free the command queue*/ 
    q_put(&ecall_cmd_free_q, &cmd_ptr->link);
  }

  if(num_rx_pcm_cmd_processed > 1)
  {
    ECALL_MSG_1( ECALL_ERROR, "[bd] WARNING: processed > 1 command (%d), might lose sync!",
                               num_rx_pcm_cmd_processed );
  }

} /* ECALL_PROCESS_CMD() */



/*===========================================================================

FUNCTION ECALL_IVS_WAIT

DESCRIPTION
  This function suspends execution of the ECALL_IVS_WAIT task until one (or
  more) signals from the requested signal mask is set.

  Watchdog kicking is performed in this function.

DEPENDENCIES
  None

RETURN VALUE
  The signal mask returned by rex_wait().

SIDE EFFECTS
  None

===========================================================================*/
rex_sigs_type  ecall_ivs_wait
(
   rex_sigs_type  requested_signal_mask       /* Signal mask to suspend on */
)
{
  rex_sigs_type  set_signals;     /* Signal mask upon return from rex wait */

  /*-------------------------------------------------------------------------
    Wait for a signal to be set.
  -------------------------------------------------------------------------*/
  do
  {
    /*lint --e{641}*/
    set_signals = rex_wait( requested_signal_mask | ECALL_IVS_DOG_RPT_TIMER_SIG );

    if( (set_signals & ECALL_IVS_DOG_RPT_TIMER_SIG) != 0 )
    {
      #ifdef FEATURE_ECALL_HAS_DOG_HB
      /*---------------------------------------------------------------------
      The watchdog needs a kick. Clear the signal and report back 
      to the watchdog by Dog HB.
      ---------------------------------------------------------------------*/
      (void)rex_clr_sigs( ecall_ivs_myself, ECALL_IVS_DOG_RPT_TIMER_SIG );
      /* Report to the Dog HB */
      dog_hb_report(DOG_ECALL_IVS_RPT);

      #else /* Legacy Dog */
      /*---------------------------------------------------------------------
      The watchdog needs a kick. Report back to the watchdog, and then
      restart the timer.
      ---------------------------------------------------------------------*/
      #ifdef DOG_DYNAMIC_API
      if(ecall_ivs_dog_rpt_time_var > 0)
      #endif
      {
        (void)rex_clr_sigs( ecall_ivs_myself, ECALL_IVS_DOG_RPT_TIMER_SIG );  
        dog_report( DOG_ECALL_IVS_RPT );
        (void)rex_set_timer( &ecall_ivs.ecall_ivs_rpt_timer, DOG_ECALL_IVS_RPT_TIME ); 
      } 
      #endif /* FEATURE_ECALL_HAS_DOG_HB */

      /*---------------------------------------------------------------------
      Processed the DOG_RPT_TIMER_SIG so clear it off in set_signals. 
      ---------------------------------------------------------------------*/
      set_signals &= (~ECALL_IVS_DOG_RPT_TIMER_SIG);
    }
  } while( (set_signals & requested_signal_mask) == 0 );

  return( set_signals );

} /* ecall_ivs_wait() */



/*==========================================================================

  FUNCTION ECALL_GET_CMD_BUF

  DESCRIPTION
    Returns a command pointer off of the free queue, or NULL on failure.

  DEPENDENCIES
    None.

  RETURN VALUE
    Command pointer or NULL

  SIDE EFFECTS
    None

===========================================================================*/
ecall_cmd_type *ecall_get_cmd_buf
(
  void
)
{
  ecall_cmd_type *cmd_ptr = NULL;

  if ((cmd_ptr = q_get(&ecall_cmd_free_q)) == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "No items on eCall task free cmd q" );
  }

  return cmd_ptr;

} /* ecall_get_cmd_buf() */


/*==========================================================================
  FUNCTION ECALL_PUT_CMD

  DESCRIPTION
    Adds a command pointer to the command queue and signals the eCall task
    to process it.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    eCall task is signaled that there is a pending command

===========================================================================*/
void ecall_put_cmd
(
  ecall_cmd_type  *cmd_ptr
)
{

  q_put(&ecall_cmd_q, &cmd_ptr->link);
  (void) rex_set_sigs( ecall_ivs_myself, ECALL_IVS_CMD_Q_SIG );

} /* ecall_put_cmd() */

#ifndef FEATURE_ECALL_LCU45
/*==========================================================================
  FUNCTION    ECALL_IVS_NPA_CB   

  DESCRIPTION
    callback function called by npa

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    

===========================================================================*/
/*lint -save -esym(715,context,event_type,data,data_size) -e818*/
void ecall_ivs_npa_cb
(
  void          *context, 
  unsigned int  event_type, 
  void          *data,  
  unsigned int  data_size
)
{
  ecall_cmd_type *cmd_ptr = NULL;

  cmd_ptr = ecall_get_cmd_buf();
  if(cmd_ptr == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "ecall_get_cmd_buf ran out of buf in ecall_ivs_npa_cb()" );
  }
  else
  {
    /* create a handle to the /core/cpu resource */
    cmd_ptr->data.cpu_hdl = npa_create_sync_client("/core/cpu", "ECALL_IVS", NPA_CLIENT_REQUIRED );
    cmd_ptr->cmd_id = ECALL_CMD_SET_CPU_HDL;
    ecall_put_cmd(cmd_ptr);
  }
}
#endif /* FEATURE_ECALL_LCU45 */

/*lint -restore*/
/*==========================================================================
  FUNCTION    ECALL_IVS_INIT_NV_ITEMS   

  DESCRIPTION
    initialise the ecall.conf file to store paths to ecall NV items.
    ecall.conf is used by diag tools to read nv paths

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
     None

===========================================================================*/
void ecall_ivs_init_nv_items
(
  void
)
{
  const char *ecall_conf_fpath = "/nv/item_files/conf/ecall.conf";
  const char *ecall_config_items = "/nv/item_files/modem/ecall/ecall_mode\n";
  const char *ecall_mode_nv = "/nv/item_files/modem/ecall/ecall_mode";


  struct fs_stat file_stat;
  int conf_fd = -1;

  /* create the directory if it doesn't exist */
  if (efs_stat("/nv/item_files/conf", &file_stat) == -1 )
  {
    if (efs_stat("/nv/item_files", &file_stat) == -1 )
    {
      if (efs_stat("/nv", &file_stat) == -1 )
      {
        (void)efs_mkdir("/nv", ECALL_IVS_PERMISSIONS);
      }
      (void)efs_mkdir("/nv/item_files", ECALL_IVS_PERMISSIONS);
    }
    (void)efs_mkdir("/nv/item_files/conf", ECALL_IVS_PERMISSIONS);
  }

  if (efs_stat("/nv/item_files/modem/ecall", &file_stat) == -1 )
  {
    if (efs_stat("/nv/item_files/modem", &file_stat) == -1 )
    {
      (void)efs_mkdir("/nv/item_files/modem", ECALL_IVS_PERMISSIONS);
    }
    (void)efs_mkdir("/nv/item_files/modem/ecall", ECALL_IVS_PERMISSIONS); 
  }

  /* Create ecall.conf file for QPST to copy the NV item to QCN */
  if( efs_stat(ecall_conf_fpath, &file_stat) != 0 )
  {
    ECALL_MSG_0( ECALL_HIGH, "ecall.conf not present yet. Creating it now");

    /* Open /nv/item_files/conf/ecall.conf file */
    conf_fd =  efs_creat( ecall_conf_fpath, ECALL_IVS_PERMISSIONS );

    if ( conf_fd >= 0 )
    {
      (void)efs_write(conf_fd, ecall_config_items, strlen(ecall_config_items));
    }
    else
    {
      ECALL_MSG_2( ECALL_HIGH, "Failed to open ecall.conf efs_err=%d errno %d",
                                conf_fd,
                                efs_errno );
    }
    (void)efs_close(conf_fd);
    ECALL_MSG_1( ECALL_HIGH, "ecall.conf file fd %d", conf_fd );
  }

  /* Open/Read file */
  ECALL_MSG_1( ECALL_HIGH, "Open/Read file=%s",
                            ecall_mode_nv );

  if ( sizeof(boolean) != (uint32)efs_get( ecall_mode_nv,
                                           &ecall_ivs.ecall_nv_enable,
                                           sizeof(ecall_ivs.ecall_nv_enable) ) )
  {
    ECALL_MSG_1( ECALL_HIGH, "efs_get error %d",
                              efs_errno );

    ecall_ivs.ecall_nv_enable = FALSE;

    /* Open/Write file */
    if( -1 ==  efs_put( ecall_mode_nv,
                        &ecall_ivs.ecall_nv_enable,
                        sizeof(ecall_ivs.ecall_nv_enable),
                        O_CREAT | O_AUTODIR,
                        ALLPERMS) )
    {
      ECALL_MSG_1( ECALL_HIGH, "efs_put error = %d",
                                efs_errno );
    }
  }

  ECALL_MSG_1( ECALL_HIGH, "ecall_ivs.ecall_nv_enable = %d",
                            ecall_ivs.ecall_nv_enable );



}

/*===========================================================================

FUNCTION ecall_ivs_task_offline

DESCRIPTION
  This procedure performs the transition to "offline" for the ECALL_IVS task.

DEPENDENCIES
  

RETURN VALUE
  None

SIDE EFFECTS
  ecall_ivs_tcb

===========================================================================*/
LOCAL void ecall_ivs_task_offline (void)
{

#ifndef FEATURE_MODEM_RCINIT_PHASE2
  /* Process task offline procedure from task controller. */
  tmc_task_offline();
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */

} /* end vs_task_offline */


/*===========================================================================

FUNCTION ecall_ivs_task_shutdown

DESCRIPTION
  This procedure performs shutdown processing for the ECALL_IVS task.

DEPENDENCIES
  FEATURE_ECALL_IVS_TASK is defined.

RETURN VALUE
  None

SIDE EFFECTS
  ecall_ivs_tcb

===========================================================================*/
LOCAL void ecall_ivs_task_shutdown (void)
{

#ifndef FEATURE_MODEM_RCINIT_PHASE2
  /* Process task stop procedure from task controller. */
  tmc_task_stop();
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */

} /* end ecall_ivs_task_shutdown */

/*===========================================================================

  FUNCTION ECALL_IVS_SET_VOC_TIMER

  DESCRIPTION
    This function will set the vocoder timer with the input time duration to
    identify missed rx_pcm().
      
  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_ivs_set_voc_timer
(
  uint32 duration
)
{
  ECALL_MSG_1( ECALL_MED, "Set voc_timer %d ms",
                           duration );
  (void) rex_set_timer( &ecall_ivs.ecall_ivs_voc_timer, duration );
} /* ECALL_IVS_SET_VOC_TIMER */

/*===========================================================================

  FUNCTION ECALL_IVS_CLR_VOC_TIMER

  DESCRIPTION
    This function will clear the vocoder timer.
      
  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_ivs_clr_voc_timer
(
  void
)
{
  (void) rex_clr_timer( &ecall_ivs.ecall_ivs_voc_timer );
} /* ECALL_IVS_CLR_VOC_TIMER */

#ifdef FEATURE_ECALL_HAS_MMOC
/*==========================================================================

  FUNCTION ECALL_IVS_MSGR_INIT

  DESCRIPTION
    Initializes ECALL_IVS message router.

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
 
===========================================================================*/
static void ecall_ivs_msgr_init(void)
{
  /* Message Router */
  errno_enum_type       msgr_error;
  byte                  count = 0;

  ECALL_MSG_0( ECALL_HIGH, "ecall_ivs_msgr_init()" );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize MSGR Client and register for messages */
  (void)q_init(&ecall_ivs_msgr_free_q);
  (void)q_init(&ecall_ivs_msgr_q);

  /*( Initialize the free Q with cmd type structures*/
  for(count = 0; count < ECALL_IVS_MSGR_Q_SIZE; count++)
  {
    q_put(&ecall_ivs_msgr_free_q, q_link(&ecall_ivs_msgr_bufs[count], &ecall_ivs_msgr_bufs[count].link));
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /* Create ecall_ivs_msgr_client*/
  msgr_error = msgr_client_create(&ecall_ivs_msgr_client);
  if (msgr_error != E_SUCCESS) {
      ECALL_MSG_1( ECALL_ERROR, "ecall_ivs_msgr_create() faild with error = %d",
                                 msgr_error );
      return;
  }

  msgr_error = msgr_client_add_rex_q(  &ecall_ivs_msgr_client,
                                       ecall_ivs_myself,
                                       ECALL_IVS_MSGR_Q_SIG,
                                       &ecall_ivs_msgr_q,
                                       &ecall_ivs_msgr_free_q,
                                       (uint16)ECALL_IVS_OFFSETOF( ecall_ivs_msgr_cmd_s_type, cmd ),    
                                       MSGR_NO_OFFSET,   
                                       sizeof(ecall_ivs_msgr_cmd_s_type),
                                       &ecall_ivs_msgr_rex_q_id        
                                       );

  if (msgr_error != E_SUCCESS) {
      ECALL_MSG_1( ECALL_ERROR, "msgr_client_add_rex_q() failed with error = %d",
                                 msgr_error );
      return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   
  /* Register with MSG ROUTER for OFFLINE notification */
  msgr_error = msgr_register(MSGR_ECALL_IVS, &ecall_ivs_msgr_client, ecall_ivs_msgr_rex_q_id, MM_MMOC_OFFLINE_IND);
  if (msgr_error != E_SUCCESS) {
      ECALL_MSG_1( ECALL_ERROR, "msgr_register() failed with msgr error = %d",
                                 msgr_error );
  }
   
  return; 

} /* ECALL_IVS_MSGR_INIT() */

/*==========================================================================

  FUNCTION ECALL_IVS_MSGR_PROC

  DESCRIPTION
    This function will process the eCall IVS msgr notification.

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
 
===========================================================================*/
void ecall_ivs_msgr_proc(void)
{
  ecall_ivs_msgr_cmd_s_type *rpt_ptr = NULL;
  
  /* Get the command from the ecall_ivs_msgr_q and process it */
  while ((rpt_ptr = q_get(&ecall_ivs_msgr_q)) != NULL)
  {
    if (MM_MMOC_OFFLINE_IND == rpt_ptr->cmd.offline_ind.hdr.id)
    {
      ECALL_MSG_0( ECALL_HIGH, "Received OFFLINE notification" );
      /* Process task offline procedure.*/
    }
    else
    {
      ECALL_MSG_1( ECALL_ERROR, "Receive unexpected msg id %d",
                                 rpt_ptr->cmd.offline_ind.hdr.id );
    }
  
    //Return the buffer back to free Q
    q_put (&ecall_ivs_msgr_free_q, &(rpt_ptr->link));
  }  

} /* ECALL_IVS_MSGR_PROC() */
#endif /* FEATURE_ECALL_HAS_MMOC */

/*==========================================================================

  FUNCTION    ECALL_IVS_TASK_INIT_BEFORE_TASK_START

  DESCRIPTION
     Initializes queues and defualt values for the global data.

  DEPENDENCIES
    None.

  RETURN VALUE
  
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_ivs_task_init_before_task_start
(
  void
)
{
  uint8 i=0;

  ECALL_MSG_0( ECALL_HIGH, "RCINIT_DBG: ecall_modem_ivs_init_before_task_start() entered" );

  /* Initialize command queues */
  (void) q_init(&ecall_cmd_q);
  (void) q_init(&ecall_cmd_free_q);

  for (i = 0; i < ECALL_CMD_BUF_COUNT; i++)
  {
     (void) q_link(&ecall_cmd_buf[i], &ecall_cmd_buf[i].link);
     q_put(&ecall_cmd_free_q, &ecall_cmd_buf[i].link);
  }

  /*-------------------------------------------------------------------------
      Initialise the pcm command queues for TX PCM transmission
  -------------------------------------------------------------------------*/
  ecall_modem_ivs_init();

  ECALL_MSG_0( ECALL_HIGH, "RCINIT_DBG: ecall_modem_ivs_init_before_task_start() returning" );
} /* ECALL_IVS_TASK_INIT_BEFORE_TASK_START */

/*==========================================================================

  FUNCTION    ECALL_IVS_TASK_INIT

  DESCRIPTION
     Initializes queues and defualt values for the global data.

  DEPENDENCIES
    None.

  RETURN VALUE
  
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_ivs_task_init
(
  void
)
{
  ECALL_MSG_0( ECALL_HIGH, "RCINIT_DBG: ecall_ivs_task_init() entered" );

  /*-------------------------------------------------------------------------
    Wait for the task start signal from task controller.
  -------------------------------------------------------------------------*/
  #ifdef FEATURE_MODEM_RCINIT
    /* Inform RCInit that ECALL_IVS initialization phase is complete */
    ECALL_MSG_0( ECALL_HIGH, "RCINIT_DBG: Calling rcinit_handshake_startup()" );
    rcinit_handshake_startup();
  #else /* FEATURE_MODEM_RCINIT */
    /* If not using RCInit framework, initialize ECALL_IVS queue and clients. */
    /* Otherwise, RCInit has already called this function in the init step. */
    ECALL_MSG_0( ECALL_HIGH, "RCINIT_DBG: Calling ecall_ivs_task_init_before_task_start() from ecall_ivs_task_init" );
    ecall_ivs_task_init_before_task_start();

  #ifndef FEATURE_MODEM_RCINIT_PHASE2
    /* Inform Main Control task that ECALL_IVS task has finished initialization */
    tmc_task_start();
  #endif /* FEATURE_MODEM_RCINIT_PHASE2 */
  #endif /* FEATURE_MODEM_RCINIT */

  #ifdef FEATURE_ECALL_HAS_DOG_HB
    /* Register with DOG HB new API */
    ecall_ivs_dog_rpt_var = dog_hb_register_rex((rex_sigs_type) ECALL_IVS_DOG_RPT_TIMER_SIG);
  #else /* Legacy Dog */
  #ifdef DOG_DYNAMIC_API
    ecall_ivs_dog_rpt_var      = dog_register( ecall_ivs_myself, \
                                               DOG_DEFAULT_TIMEOUT,
                                               DOG_DEADLKCHK_ENABLE);
    ecall_ivs_dog_rpt_time_var = dog_get_report_period( ecall_ivs_dog_rpt_var );
  
  /* If the return value from dog_get_report_period() was zero then we MUST
   ** NOT report to dog directly, as DOG will monitor us in a different manner.
   ** Hence we won't initialize or start any timer. */
  if( ecall_ivs_dog_rpt_time_var > 0 )
  #endif /* ! DOG_DYNAMIC_API */
  {
    /* Initialize taskname watchdog report timer */
    rex_def_timer( &ecall_ivs.ecall_ivs_rpt_timer, ecall_ivs_myself, ECALL_IVS_DOG_RPT_TIMER_SIG );
    (void)rex_set_timer( &ecall_ivs.ecall_ivs_rpt_timer, DOG_ECALL_IVS_RPT_TIME );
  }
  #endif /* FEATURE_ECALL_HAS_DOG_HB */

  /* Initialize timers */
  rex_def_timer( &ecall_ivs.ecall_ivs_voc_timer, ecall_ivs_myself, ECALL_IVS_VOC_TIMER_SIG );

  (void) rex_clr_sigs( ecall_ivs_myself, (rex_sigs_type)~0 );

  /* initialise the ecall.conf file and read the ecall nv items */
  ecall_ivs_init_nv_items();

  /* Initialize eCall provisioning app info */
  ecall_sim_cfg_init();

  if(ecall_ivs.ecall_nv_enable == TRUE) 
  {
    #ifdef FEATURE_NAS_ECALL
      ECALL_MSG_0( ECALL_HIGH, "IVS Task: ECALL enabled thru NV, start EIP" );
      ecall_sim_mmgsdi_init();
      
    #ifdef FEATURE_DUAL_SIM
      /* For Dual SIM case with as_id unknown at this stage. *
       * Do not send operating mode to NAS at init stage.     */
      ECALL_MSG_0( ECALL_HIGH, "Do not send operating mode to NAS at init stage for dual SIM case since as_id is unknown");
    #else /* Single SIM case */
      ecall_send_modem_config_to_nas(SYS_MODEM_AS_ID_1);
    #endif /* FEATURE_DUAL_SIM */
    #else /* FEATURE_NAS_ECALL not defined */
      ECALL_MSG_0( ECALL_HIGH, "IVS Task: FEATURE_NAS_ECALL not Enabled, Skip EIP" );
    #endif /* FEATURE_NAS_ECALL */
  }
  else
  {
    ECALL_MSG_0( ECALL_HIGH, "IVS Task: ECALL Disabled thru NV, SKip EIP" );
  }
  
  #ifndef FEATURE_ECALL_LCU45
  npa_resource_available_cb( "/core/cpu", ecall_ivs_npa_cb, NULL );
  #endif /* FEATURE_ECALL_LCU45 */

  /* Initialize the queue in ecall_modem_clients */
  ecall_emodem_clients_initialise();

  #ifdef FEATURE_ECALL_HAS_MMOC
  ecall_ivs_msgr_init();
  #endif

  ECALL_MSG_0( ECALL_HIGH, "RCINIT_DBG: ecall_ivs_task_init() returning" );
} /* ECALL_IVS_TASK_INIT */




/*===========================================================================

                          EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*==========================================================================

  FUNCTION ECALL_IVS_TASK

  DESCRIPTION
    Starts up the eCall task

  DEPENDENCIES
    FEATURE_ECALL_IVS_TASK is defined.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
/*lint -esym(715,ignored) */
void ecall_ivs_task
(
   dword ignored
)
{
   /*lint --e{641}*/
   rex_sigs_type   set_sig;
   rex_sigs_type   requested_sigs;   /* Signals to wait for  */

   ecall_ivs_myself = rex_self();    /* Will never change after this */

  /*-------------------------------------------------------------------------
   Do task initialization. The init function performs all the task-level
   initialization, e.g. define message queues.
  -------------------------------------------------------------------------*/
  ecall_ivs_task_init();

  /* Start the queue processing */
  (void) rex_set_sigs( ecall_ivs_myself, ECALL_IVS_CMD_Q_SIG );


  ECALL_MSG_0( ECALL_HIGH, "Entered ecall_ivs_task()" );

  /*-------------------------------------------------------------------------
     Infinite loop for the task to be alive
  -------------------------------------------------------------------------*/
  /*lint -e655*/
  requested_sigs = 
                  (ECALL_IVS_CMD_Q_SIG )        |
                  (ECALL_IVS_TASK_OFFLINE_SIG ) | 
                  (ECALL_IVS_TASK_STOP_SIG)     |
                  (ECALL_IVS_VOC_TIMER_SIG)     |
                  (ECALL_IVS_MSGR_Q_SIG);
  /*lint +e655*/
  for( ; ; )
  {
    
    /*Wait for the signal and process the command*/
    set_sig = ecall_ivs_wait( requested_sigs); 

    if((set_sig & ECALL_IVS_CMD_Q_SIG )!= 0 )
    {
      (void) rex_clr_sigs( ecall_ivs_myself, ECALL_IVS_CMD_Q_SIG ); 
      /* Process the received command*/ 
      ecall_process_cmd();
    }

    if ((set_sig & ECALL_IVS_TASK_OFFLINE_SIG) != 0) 
    {
      (void) rex_clr_sigs( ecall_ivs_myself, ECALL_IVS_TASK_OFFLINE_SIG );
      ecall_ivs_task_offline ();
    }

    if ((set_sig & ECALL_IVS_TASK_STOP_SIG) != 0) 
    {
      (void) rex_clr_sigs( ecall_ivs_myself, ECALL_IVS_TASK_STOP_SIG );
      ecall_ivs_task_shutdown ();
    }

    if ((set_sig & ECALL_IVS_VOC_TIMER_SIG) != 0) 
    {
      /* Vocoder did not call ecall_rx_pcm() within this period of time.
         Clear the signal and call the handle function */
      (void) rex_clr_sigs ( ecall_ivs_myself, ECALL_IVS_VOC_TIMER_SIG );
      ecall_modem_handle_voc_timer_expire();
    }

    if ((set_sig & ECALL_IVS_MSGR_Q_SIG)!=0 )
    {
      /*-------------------------------------------------------------------------
                        Clear the Signal
      -------------------------------------------------------------------------*/
      (void) rex_clr_sigs( ecall_ivs_myself, ECALL_IVS_MSGR_Q_SIG );
      ECALL_MSG_0( ECALL_HIGH, "ECALL_IVS receives ECALL_IVS_MSGR_Q_SIG" );
      #ifdef FEATURE_ECALL_HAS_MMOC
      ecall_ivs_msgr_proc();
      #endif /* FEATURE_ECALL_HAS_MMOC */
    } /* ECALL_IVS_MSGR_Q_SIG */

  }
}
/*lint +esym(715,ignored) */

#else

  void ecall_ivs_task_dummy_func(void);

#endif /* end FEATURE_ECALL_IVS */

