/*===========================================================================
           COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cnm_mntask_main.c_v   1.16   14 Jun 2002 14:47:42   rgurumoo  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/src/cnm_mntask_main.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
8-Jun-2001   VT    Moved tmc_task_start() outside FOREVER
13-Jun-2001  VT    Cleaned up the logging on receiving a CNM_MN (Task) Msg
20-Jun-2001  VT    Removed unwanted reference(s) to m20000_debug_string

7/13/01      CD    Replaced MS_MN_MMI by MS_MN_CM
                   Added MS_MN_UASMS
7/31/01      JC    Add watch dog reporting         

8/8/01       CD    Deleted call to gs_log_message
                   Deleted MSG_MED to report the start of a CNM transaction

08/14/01     AB    Renamed connection manager references to CNM from CM.

08/31/01     CD    Added void return type to mn_cnm_cmd_q_free
09/05/01    sbk    Moved the message buffer into global space

4-Oct-2001   VT    Split mn_cnm_main() to form the new function,
                     mn_cnm_process_messages().

20-Feb-2002  AB    Fixed the routing of MN_CC_TIMER in 
                                 mn_cnm_process_messages().

12-Mar-2002  VT    Removed Lint errors.

03/26/02     CD    Merged in branch 1.11.1.0 for GS unification

03/26/02     CD    Merged in branch 1.11.1.0 for GS unification

06/11/02     RG    Fixed the bug found during testing the TR2M timeout (CR 22313).
                   In line 366 the timeerid range check should be done till 
                   MAXNO_SMS_TRANSACTIONS instead of MAXNO_SMS_MO_TRANSACTIONS. TR2M 
                   timer id value of "0xf8" should be handled by the MN_handle_MN_messaging() 
                   routine. (Before this change, the TR2M timeout was handled
                   by the CNM timeout handler which dropped it with an "unknown timer 
                   expiry" message in the log).

06/14/02     RG    Used FEATURE_CNM_UNIT_TEST to exclude compilation of code related to 
                   watch dog report timer. This is done to reduce the dependencies of the 
                   MN-CNM unit when built for unit testing.

07/29/02     AB    Updated the mn_cnm_main_task(void) prototype to be more 
                   ANSI C compliance and more consistence other WCDMA tasks.

02/12/02     AB    Added checking for function with return value per Lint.

                   Added routing for new message set, MS_CC_MM_PS, to support 
                   SMS prefer CS+PS domain setting on a per transaction basis.

05/25/05     HS    Initialize message buffer every time to prevent unexpected
                   errors

02/11/06     NR    Removing lint errors
12/14/11     jbk    Free floating changes for gscvoice library
===========================================================================*/
#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "queue.h"
#include <string.h>
#include "msg.h"
#include "err.h"
#ifndef FEATURE_CNM_UNIT_TEST
#include "dog_hb_rex.h"
#endif
#include "environ.h"
#include "sys_type.h"
#include "sys_cnst_v.h"
#include "sys_stru_v.h"
#include "ms.h"
#include "timers_v.h"
#include "gs_v.h"
#include "mn_cnmtask.h"
#include "mn_cnmtask_int.h"
/* ----------------------------------------------------------------------- 
** REMOVE compiler warning result from inclusion of memheap.h 
** ----------------------------------------------------------------------- */
#ifdef FEATURE_MEM_DEBUG
#undef FEATURE_MEM_DEBUG
#endif
#ifndef FEATURE_MODEM_RCINIT_PHASE2
#include "tmc.h"
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */
#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#endif
#ifdef FEATURE_MODEM_RCINIT
#include "rcinit.h"
#endif

#include "nasutils.h"
#include "mnglobal.h"
#include "cnm.h"
#include <stringl/stringl.h>

/*
 * Function prototypes
 */

VOID_FUNC CNM_initialise_CNM_data( void );
VOID_FUNC MN_initialise_MN_data( void );
VOID_FUNC CNM_route_CNM_messages( IMH_T * );
VOID_FUNC MN_handle_MN_messaging( IMH_T * );
gs_status_T cnm_send_routine( void *msg_p );

static mn_cnm_cmd_type *mn_cnm_cmd_q_get(void);
static mn_cnm_cmd_type *mn_cnm_cmd_internal_q_get(void);
static void mn_cnm_cmd_q_free(mn_cnm_cmd_type *cmd_ptr);
#if defined (FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
static boolean mn_cnm_validate_and_set_as_id(IMH_T *);
static boolean mn_cnm_validate_as_id(sys_modem_as_id_e_type local_as_id);
#endif

void mn_cnm_process_external_messages(void);
void mn_cnm_process_internal_messages(void);

#ifndef FEATURE_CNM_UNIT_TEST
/* ----------------------------
** Define watchdog report handle
** ---------------------------- */
  #undef  DOG_MN_CNM_RPT
  #define DOG_MN_CNM_RPT       mn_cnm_dog_rpt_var
  static dog_report_type       mn_cnm_dog_rpt_var  = 0;      /* Initial Safety */

#endif

/* This buffer should not be declared on the stack due to its size */
#ifndef FEATURE_MODEM_HEAP
byte message[sizeof( mn_cnm_cmd_type )];
#endif

extern void ghdi_mvs_init(void);

rex_crit_sect_type        cm_conn_ids_crit_sect;  /* to be used for critical sections */
rex_tcb_type* mn_tcb_ptr;
#ifdef FEATURE_NAS_MN_CB_SERVICES 
LOCAL  rex_crit_sect_type        mn_cb_crit_sect;


/*
 * Function name :  cm_init_conn_ids_crit_sect
 * -------------
 *
 * Description : 
 * -----------     
 *
 *  Initialize the semaphore that is used by the 
 *  the CM or UI task to modify MN global data inorder
 *  to allocate or deallocate the connection ID.
 *
 * Parameters: None
 * ----------  
 *
 * Returns: Reserved connection_id, if available. Else, 
 * --------   UNKNOWN_CONNECTION_ID.
 *
 */

void mn_cb_init_crit_sect(void)
{
   /* Initialize the critical section */
   (void)gs_attach_lock(&mn_cb_crit_sect);
}


/*
 * Function name :  MN_register_cb_services()
 * -------------
 *
 * Description :  
 * -----------     
 *   These CB are to be register on a call basis by the respective Client 
 *   or after the CNM_TASK had been initialized.  
 *
 *   Deregister the service by NULL(tify) the CB function pointer. 
 *   
 * Parameters: message_header
 * ----------  
 *
 * Returns:
 * --------
 *
 */
boolean MN_register_cb_services(mn_client_T client, mn_client_service_T type, void *cb_function)
{
    boolean registered = TRUE;

    /* Enter the critical section */
    (void)gs_lock(&mn_cb_crit_sect);

    if( (cb_function != NULL) && (client < MN_MAX_CLIENT))
    {
/*lint -e611 */
       if((client == MN_CLIENT_CM) && (type == MN_AOC_RESPONSE))
       {
           mn_cb_services[MN_CLIENT_CM].client = client;
           mn_cb_services[MN_CLIENT_CM].type = type;
           mn_cb_services[MN_CLIENT_CM].cb_function.aoc_response = (mn_cm_cb_T)(cb_function);
       }
       else if((client == MN_CLIENT_DS) && (type == MN_SYNC_NOTIFICATION))
       {
           mn_cb_services[MN_CLIENT_DS].client = client;
           mn_cb_services[MN_CLIENT_DS].type = type;
           mn_cb_services[MN_CLIENT_DS].cb_function.sync_notify = (mn_sync_notify_cb_T)cb_function;
       }
       else if((client == MN_CLIENT_ATCOP) && (type == MN_SYNC_NOTIFICATION))
       {
           mn_cb_services[MN_CLIENT_ATCOP].client = client;
           mn_cb_services[MN_CLIENT_ATCOP].type = type;
           mn_cb_services[MN_CLIENT_ATCOP].cb_function.sync_notify = (mn_sync_notify_cb_T)cb_function;
       }
       else
       {
           registered = FALSE;

           MSG_ERROR_DS_3(MN_SUB,"=MNCNM= MN register CB: client = %d, type = %d, cb = %x", client, type, cb_function);  
       }
/*lint +e611 */
    }
    else
    {
        registered = FALSE;
    }

    /* exit critical section */
    (void)gs_unlock(&mn_cb_crit_sect); 

    return registered;
}
#endif /* FEATURE_NAS_MN_CB_SERVICES */

/*
 * Function name :  cm_init_conn_ids_crit_sect
 * -------------
 *
 * Description : 
 * -----------     
 *
 *  Initialize the semaphore that is used by the 
 *  the CM or UI task to modify MN global data inorder
 *  to allocate or deallocate the connection ID.
 *
 * Parameters: None
 * ----------  
 *
 * Returns: Reserved connection_id, if available. Else, 
 * --------   UNKNOWN_CONNECTION_ID.
 *
 */

void cm_init_conn_ids_crit_sect(void)
{
   /* Initialize the critical section */
   (void)gs_attach_lock(&cm_conn_ids_crit_sect);
}


/*************** Begin   mn_cnm_main ********************************************
 *
 *  Function name: mncnm_main()
 *  ------------------------------------
 *
 *  Description:
 *  ------------
 *  This function is the entry point for the MN-CNM task. It calls
 * initialisation functions to perform task initialisation for the CNM and MN
 * tasks, then sits in an infinite loop waiting for messages on the input
 * queue. Messages are examined to determine if they are intended for the MN
 * or the CNM half on the basis of message set, with the exception of timer
 * messages, which are routed on the basis of the timer id in the message.
 *
 *  Parameters:
 *  -----------
 *  None
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

void mn_cnm_init_before_task_start(void)
{
    /* Initialize command queue */
    if(q_init(&mn_cnm_cmd_q))
    {
        MSG_ERROR("MN_CNM not initialized!",&mn_cnm_cmd_q,0,0); /* ! tocheck: why its not Fatal */
    }
    if(q_init(&mn_cnm_internal_cmd_q))
    {
        MSG_ERROR("MN_CNM internal Q not initialized!",&mn_cnm_internal_cmd_q,0,0);
    }
    
    /* Initialize gs message passing for this queue */
    if (gs_init_for_send_only(GS_QUEUE_MN_CM,
          (gs_put_handler_ptr)cnm_send_routine) != GS_SUCCESS)
    {
        MSG_ERROR_DS_0(MN_SUB,"=MNCNM= MN_CNM not initialized!");
    }

   cm_init_conn_ids_crit_sect();  /* initialized semaphore used to access mn_call_information[] */ 

   ghdi_mvs_init(); /* Initialise gsvoice library */

   /* MN call back function services */
#ifdef FEATURE_NAS_MN_CB_SERVICES 
   mn_cb_init_crit_sect();  /* MN call back function services */ 
   memset(&mn_cb_services[0], NULL, sizeof(mn_cb_services_T) * (int)MN_MAX_CLIENT);
#endif /* FEATURE_NAS_MN_CB_SERVICES */

}


/*************** Begin   mn_cnm_main ********************************************
 *
 *  Function name: mncnm_main()
 *  ------------------------------------
 *
 *  Description:
 *  ------------
 *  This function is the entry point for the MN-CNM task. It calls
 * initialisation functions to perform task initialisation for the CNM and MN
 * tasks, then sits in an infinite loop waiting for messages on the input
 * queue. Messages are examined to determine if they are intended for the MN
 * or the CNM half on the basis of message set, with the exception of timer
 * messages, which are routed on the basis of the timer id in the message.
 *
 *  Parameters:
 *  -----------
 *  None
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

void mn_cnm_main(dword argc)
{
    
    static rex_sigs_type   sigs;

#ifndef FEATURE_MODEM_RCINIT
    mn_cnm_init_before_task_start();
#endif

    mn_tcb_ptr = rex_self();

#ifndef FEATURE_CNM_UNIT_TEST
    /* Register with DOG HB new API */
    mn_cnm_dog_rpt_var = dog_hb_register_rex((rex_sigs_type) MN_CNM_DOGRPT_TIMER_SIG);
    
#endif

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
    mn_dsda_as_id = MN_AS_ID_1; /* To initialize stack 1*/
#endif
    CNM_initialise_CNM_data();
    MN_initialise_MN_data();

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
    mn_dsda_as_id = MN_AS_ID_2; /* To initialize stack 2*/
    CNM_initialise_CNM_data();
    MN_initialise_MN_data();
#endif


#ifdef FEATURE_MODEM_RCINIT
   rcinit_handshake_startup();
#else
    /* Clear signals */
    (void) rex_clr_sigs(mn_tcb_ptr, (rex_sigs_type)~0);
#ifndef FEATURE_MODEM_RCINIT_PHASE2
    tmc_task_start();
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */
#endif

#ifndef FEATURE_CNM_UNIT_TEST

    /* Report to the Dog HB */
    dog_hb_report(DOG_MN_CNM_RPT);


#endif

    FOREVER
    {
        /* Wait on signal */


        sigs = rex_wait(MN_CNM_CMD_Q_SIG|MN_CNM_DOGRPT_TIMER_SIG|MN_CNM_CMD_INTERNAL_Q_SIG 
#ifdef TEST_FRAMEWORK
                       #error code not present
#endif /*TEST_FRAMEWORK*/
                       );

        
        /* First process messages from internal queue */
        if(sigs & MN_CNM_CMD_INTERNAL_Q_SIG)
        {
            
            /* Clear signals */
            (void) rex_clr_sigs( mn_tcb_ptr,MN_CNM_CMD_INTERNAL_Q_SIG);
            
            /*
            ** READ INPUT MESSAGE QUEUE
            */
            mn_cnm_process_internal_messages();
            
        }   /* if sigs & MN_CNM... */

        /* If no internal messages, then process messages from external tasks */
        if(sigs & MN_CNM_CMD_Q_SIG)
        {
            
            /* Clear signals */
            (void) rex_clr_sigs( mn_tcb_ptr,MN_CNM_CMD_Q_SIG);
            
            /*
            ** READ INPUT MESSAGE QUEUE
            */
            mn_cnm_process_external_messages();
            
        }   /* if sigs & MN_CNM... */

#ifndef FEATURE_CNM_UNIT_TEST
        /* Check if report timer signal was set.  If set then
           clear signal, report to DOG, set the timer again and proceed */
        if(sigs & MN_CNM_DOGRPT_TIMER_SIG)
        {
           /* Clear the watchdog signal */
           (void) rex_clr_sigs( mn_tcb_ptr, MN_CNM_DOGRPT_TIMER_SIG);
           dog_hb_report(DOG_MN_CNM_RPT);
        }
#endif

#ifdef TEST_FRAMEWORK
        #error code not present
#endif

                      
    }    /* FOREVER  */
                  
}    /* End of mn_cnm_main  */

/*************** End of   mn_cnm_main ******************************************/

/*************** Begin   cnm_send_routine **************************************
 *
 *  Function name: cnm_send_routine()
 *  ------------------------------------
 *
 *  Description:
 *  ------------
 *  This is the REG-specific routine used when other layers call gs_send_message
 *
 *  Parameters:
 *  -----------
 *  IN message
 *
 *  Returns:
 *  --------
 *  GS_STATUS - GS_SUCCESS if sent, GS_FAILURE otherwise
 *
 ******************************************************************************/

gs_status_T cnm_send_routine( void *msg_p )
{
    word          message_length;
    void         *cmd_ptr;

    /* Get message length */
    GET_IMH_LEN(message_length, msg_p); 

    /* Add in header size */
    message_length += sizeof(IMH_T);

    /* Get command buffer from destination task */
    if ((cmd_ptr = mn_cnm_get_cmd_buf()) != NULL)
    {
        /* Copy data to command buffer */
        memscpy(&((mn_cnm_cmd_type*)cmd_ptr)->cmd, sizeof(((mn_cnm_cmd_type*)cmd_ptr)->cmd) ,(byte*)msg_p, message_length);

        /* Put message in destination task queue */
        mn_cnm_put_cmd((mn_cnm_cmd_type*) cmd_ptr);

        return GS_SUCCESS;
    }

    return GS_FAILURE;
} /* End of cnm_send_routine  */

/*************** End of   cnm_send_routine ************************************/

/*************** Begin   cnm_internal_send_routine **************************************
 *
 *  Function name: cnm_internal_send_routine()
 *  ------------------------------------
 *
 *  Description:
 *  ------------
 *  This is the CNM internal message posting routine used for messages internal to MNCNM.
 *
 *  Parameters:
 *  -----------
 *  IN message
 *
 *  Returns:
 *  --------
 *  GS_STATUS - GS_SUCCESS if sent, GS_FAILURE otherwise
 *
 ******************************************************************************/

gs_status_T cnm_internal_send_routine( void *msg_p )
{
    word          message_length;
    void         *cmd_ptr;

    /* Get message length */
    GET_IMH_LEN(message_length, msg_p); 

    /* Add in header size */
    message_length += sizeof(IMH_T);

    /* Get command buffer from destination task */
    if ((cmd_ptr = mn_cnm_get_cmd_buf()) != NULL)
    {
        /* Copy data to command buffer */
        memscpy(&((mn_cnm_cmd_type*)cmd_ptr)->cmd, sizeof(((mn_cnm_cmd_type*)cmd_ptr)->cmd) ,(byte*)msg_p, message_length);

        /* Put message in destination task queue */
        mn_cnm_put_internal_cmd((mn_cnm_cmd_type*) cmd_ptr);

        return GS_SUCCESS;
    }

    return GS_FAILURE;
} /* End of cnm_send_routine  */

/*************** End of   cnm_internal_send_routine ************************************/

/*************** Begin of   mn_cnm_process_internal_messages*************************************/
/*
 * Function name :  mn_cnm_process_internal_messages()
 * -------------
 *
 * Description : This function processes messgaes to MN_CNM task from itself. 
 *
 * Uses :
 * ----
 *
 * Parameters:
 * ----------
 *
 * Returns:
 * --------
 *
 */
 
void mn_cnm_process_internal_messages(void)
{
     gs_status_T           status;     
     IMH_T                 *msg_header = NULL;     
     unsigned int          message_length;
     mn_cnm_cmd_type       *cmd_ptr;
     byte                  *message_p = NULL;     
     
     status = GS_SUCCESS;



#ifdef FEATURE_MODEM_HEAP
     message_p = (byte*)modem_mem_calloc(1, sizeof( mn_cnm_cmd_type ), MODEM_MEM_CLIENT_NAS);
#else
     message_p = message;
#endif
     MSG_HIGH("mn_cnm_process_internal_messages, message_p = %d",message_p,0,0);
     if(message_p == NULL)
     {
         ERR("Memory ALLOC failure", 0, 0, 0); 
         return;
     }
     /* Create variable which points to message as if its an IMH */
     msg_header = (IMH_T *)message_p; 

     while((cmd_ptr = mn_cnm_cmd_internal_q_get()) != NULL)
     {
         
         GET_IMH_LEN(message_length,((IMH_T *)&cmd_ptr->cmd));
#ifdef FEATURE_DUAL_SIM
         if(cmd_ptr->cmd.hdr.message_set == MS_MN_CC)
         {
           if(MNCNM_IS_SINGLE_STACK)
           {
             MSG_HIGH_MN_DS(MN_SUB,"=MNCNM= =MN= MS: %d   MSG_ID: %d AS_ID_%d", cmd_ptr->cmd.hdr.message_set, cmd_ptr->cmd.hdr.message_id, mn_as_id+1);
           }
#ifdef FEATURE_DUAL_ACTIVE
           else
           {
             MSG_HIGH_MN_DS(MN_SUB,"=MNCNM= =MN= MS: %d  MSG_ID: %d AS_ID_%d", cmd_ptr->cmd.hdr.message_set, cmd_ptr->cmd.hdr.message_id, mn_dsda_as_id+1);
           }
#endif
         }
         else
         {
           dword trail_bytes = 0;
           byte *trail_bytes_loc_p = (byte *) &(cmd_ptr->cmd);
           trail_bytes_loc_p += (message_length + sizeof(IMH_T)-sizeof(dword));
           memscpy(&trail_bytes, sizeof(dword) ,trail_bytes_loc_p, sizeof(dword));
           MSG_HIGH_MN_DS(MN_SUB,"=MNCNM= =MN= MS: %d   MSG_ID: %d ...0x%x", cmd_ptr->cmd.hdr.message_set, cmd_ptr->cmd.hdr.message_id, trail_bytes);
         }
#endif

         memset (message_p, 0x00, sizeof(mn_cnm_cmd_type));      
         /* Copy data from command queue to local buffer */
         memscpy(message_p, sizeof( mn_cnm_cmd_type ) ,&cmd_ptr->cmd, (unsigned int)(message_length + sizeof(IMH_T)));
         
         /* Deallocate Command Buffer */
         mn_cnm_cmd_q_free(cmd_ptr);

         if ( status == GS_SUCCESS  
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
               && mn_cnm_validate_and_set_as_id( (IMH_T *) msg_header)
#endif
             )
         {                    
             switch (msg_header->message_set)
             {
             case MS_CC_MM:
             case MS_CC_MM_PS:
                 CNM_route_CNM_messages( (IMH_T *) msg_header );
                 break;
                 
             case MS_MN_CM:
             case MS_MN_UASMS:
                 MN_handle_MN_messaging( (IMH_T *) msg_header);
                 break;
                 
             case MS_MN_CC:
                 
             /*
             * messages between MN and CC - need to establish
             * the destination based on the message id
                 */
                 
                 if (msg_header->message_id < 0x40 )
                 {
                     
                 /*
                 * message is going to CM
                     */
                     
                     CNM_route_CNM_messages( (IMH_T *) msg_header);
                     
                 }
                 else
                 {
                     MN_handle_MN_messaging( (IMH_T *) msg_header);
                 }
                 break;
                 
             case MS_MN_SS:
                 
             /*
             * need to decide on destination
                 */
                 
                 if (msg_header->message_id < 0x80 )
                 {
                     
                 /*
                 * message is going to CM
                     */
                     
                     CNM_route_CNM_messages( (IMH_T *) msg_header);
                 }
                 else
                 {
                     MN_handle_MN_messaging( (IMH_T *) msg_header);
                 }
                 break;
                 
                 
             case MS_MN_SMS:
                 
             /*
             * need to decide on destination
                 */
                 
                 if (msg_header->message_id < 0x80 )
                 {
                     
                 /*
                 * message is going to CM
                     */
                     
                     CNM_route_CNM_messages( (IMH_T *) msg_header);
                 }
                 else
                 {
                     MN_handle_MN_messaging( (IMH_T *) msg_header);
                 }
                 break;
                 
#if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))                 
             case MS_TIMER:
                 
             /*
             * need to use timer id to decide on message
             * destination
                 */
                 
                 if (msg_header->message_id == MID_TIMER_EXPIRY)
                 {
                     gs_queue_id_T queue_id = GS_QUEUE_MN_CM;
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
                     if(mn_dsda_as_id == (mn_dsda_as_id_e_type)SYS_MODEM_AS_ID_2)
                     {
                       queue_id = GS_QUEUE_MN_CM_1;
                     }
#endif
                     gs_clear_timer(queue_id, ((timer_expiry_T *)msg_header)->timer_id);

                     if ((((timer_expiry_T *)msg_header)->timer_id >= MN_CC_TIMER) &&
                         (((timer_expiry_T *)msg_header)->timer_id < 
                                            (MN_SMS_TIMER+MAXNO_SMS_TRANSACTIONS)))
                     {
                         /* timer message for MN */
                         MN_handle_MN_messaging( (IMH_T *) msg_header);
                     }
                     else
                     {
                         /* * message is going to CM */
                         CNM_route_CNM_messages( (IMH_T *) msg_header);
                     }
                 }
                 else
                 {
                     ERR("Unknown timer expiration, message_id = %d\n",
                         (dword) msg_header->message_id, 0, 0);
                 }
                 break;
#endif /*(defined(FEATURE_GSM) || defined(FEATURE_WCDMA))*/
                 
             case MS_MN_REG:
                 MN_handle_MN_messaging( (IMH_T *) msg_header);
                 break;
                 
             default:
                 ERR("Unexpected message set, message_set = %d\n",
                     (dword) msg_header->message_set, 0, 0);
                 break;
             } /* switch msg_header... */
                    
         }   /* if status... */
         else
         {
             ERR("Bad message read, status = %d\n", (dword) status, 0, 0);
         }   /* if status... */
                
     }   /* while cmd_ptr... */
#ifdef FEATURE_MODEM_HEAP
     if(message_p != NULL)
     {
       modem_mem_free(message_p, MODEM_MEM_CLIENT_NAS);
     }
#endif
     message_p = NULL;

}
/*************** Begin of   mn_cnm_process_external_messages *************************************/
/*
 * Function name :  mn_cnm_process_external_messages()
 * -------------
 *
 * Description : This function processes messgaes to MN_CNM task from other tasks. 
 *               i.e. It does not process messages posted by this task onto itself.
 * -----------
 *
 * Uses :
 * ----
 *
 * Parameters:
 * ----------
 *
 * Returns:
 * --------
 *
 */
 
void mn_cnm_process_external_messages(void)
{
     gs_status_T           status;     
     IMH_T                 *msg_header = NULL;     
     unsigned int          message_length;
     mn_cnm_cmd_type       *cmd_ptr;
     byte                  *message_p = NULL;
     
     status = GS_SUCCESS;

#ifdef FEATURE_MODEM_HEAP
     message_p = (byte*)modem_mem_calloc(1, sizeof( mn_cnm_cmd_type ), MODEM_MEM_CLIENT_NAS);
#else
     message_p = message;
#endif
     if(message_p == NULL)
     {
         ERR("Memory ALLOC failure", 0, 0, 0); 
         return;
     }
     MSG_HIGH("mn_cnm_process_external_messages, message_p = %d",message_p,0,0);
     /* Create variable which points to message as if its an IMH */
     msg_header = (IMH_T *)message_p; 

     while((cmd_ptr = mn_cnm_cmd_q_get()) != NULL)
     {
         
         GET_IMH_LEN(message_length,((IMH_T *)&cmd_ptr->cmd));
#ifdef FEATURE_DUAL_SIM
         if(cmd_ptr->cmd.hdr.message_set == MS_MN_CC)
         {
           if(MNCNM_IS_SINGLE_STACK)
           {
             MSG_HIGH_DS_3(MN_SUB,"=MNCNM= =MN= MS: %d   MSG_ID: %d AS_ID_%d", cmd_ptr->cmd.hdr.message_set, cmd_ptr->cmd.hdr.message_id, mn_as_id+1);
           }
#ifdef FEATURE_DUAL_ACTIVE
           else
           {
             MSG_HIGH_DS_3(MN_SUB,"=MNCNM= =MN= MS: %d  MSG_ID: %d AS_ID_%d", cmd_ptr->cmd.hdr.message_set, cmd_ptr->cmd.hdr.message_id, mn_dsda_as_id+1);
           }
#endif
         }
         else
         {
           dword trail_bytes = 0;
           byte *trail_bytes_loc_p = (byte *) &(cmd_ptr->cmd);
           trail_bytes_loc_p += (message_length + sizeof(IMH_T)-sizeof(dword));
           memscpy(&trail_bytes, sizeof(dword) ,trail_bytes_loc_p, sizeof(dword));
           MSG_HIGH_DS_3(MN_SUB,"=MNCNM= =MN= MS: %d   MSG_ID: %d ...0x%x", cmd_ptr->cmd.hdr.message_set, cmd_ptr->cmd.hdr.message_id, trail_bytes);
         }
#endif

         memset (message_p, 0x00, sizeof(mn_cnm_cmd_type));      
         /* Copy data from command queue to local buffer */
         memscpy(message_p, sizeof( mn_cnm_cmd_type ) ,&cmd_ptr->cmd, (unsigned int)(message_length + sizeof(IMH_T)));
         
         /* Deallocate Command Buffer */
         mn_cnm_cmd_q_free(cmd_ptr);

         if ( status == GS_SUCCESS  
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
               && mn_cnm_validate_and_set_as_id( (IMH_T *) msg_header)
#endif
             )
         {                    
             switch (msg_header->message_set)
             {
             case MS_CC_MM:
             case MS_CC_MM_PS:
                 CNM_route_CNM_messages( (IMH_T *) msg_header );
                 break;
                 
             case MS_MN_CM:
             case MS_MN_UASMS:
                 MN_handle_MN_messaging( (IMH_T *) msg_header);
                 break;
                 
             case MS_MN_CC:
                 
             /*
             * messages between MN and CC - need to establish
             * the destination based on the message id
                 */
                 
                 if (msg_header->message_id < 0x40 )
                 {
                     
                 /*
                 * message is going to CM
                     */
                     
                     CNM_route_CNM_messages( (IMH_T *) msg_header);
                     
                 }
                 else
                 {
                     MN_handle_MN_messaging( (IMH_T *) msg_header);
                 }
                 break;
                 
             case MS_MN_SS:
                 
             /*
             * need to decide on destination
                 */
                 
                 if (msg_header->message_id < 0x80 )
                 {
                     
                 /*
                 * message is going to CM
                     */
                     
                     CNM_route_CNM_messages( (IMH_T *) msg_header);
                 }
                 else
                 {
                     MN_handle_MN_messaging( (IMH_T *) msg_header);
                 }
                 break;
                 
                 
             case MS_MN_SMS:
                 
             /*
             * need to decide on destination
                 */
                 
                 if (msg_header->message_id < 0x80 )
                 {
                     
                 /*
                 * message is going to CM
                     */
                     
                     CNM_route_CNM_messages( (IMH_T *) msg_header);
                 }
                 else
                 {
                     MN_handle_MN_messaging( (IMH_T *) msg_header);
                 }
                 break;
                 
#if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))                 
             case MS_TIMER:
                 
             /*
             * need to use timer id to decide on message
             * destination
                 */
                 
                 if (msg_header->message_id == MID_TIMER_EXPIRY)
                 {
                     gs_queue_id_T queue_id = GS_QUEUE_MN_CM;
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
                     if(mn_dsda_as_id == (mn_dsda_as_id_e_type)SYS_MODEM_AS_ID_2)
                     {
                       queue_id = GS_QUEUE_MN_CM_1;
                     }
#endif
                     gs_clear_timer(queue_id, ((timer_expiry_T *)msg_header)->timer_id);

                     if ((((timer_expiry_T *)msg_header)->timer_id >= MN_CC_TIMER) &&
                         (((timer_expiry_T *)msg_header)->timer_id < 
                                            (MN_SMS_TIMER+MAXNO_SMS_TRANSACTIONS)))
                     {
                         /* timer message for MN */
                         MN_handle_MN_messaging( (IMH_T *) msg_header);
                     }
                     else
                     {
                         /* * message is going to CM */
                         CNM_route_CNM_messages( (IMH_T *) msg_header);
                     }
                 }
                 else
                 {
                     ERR("Unknown timer expiration, message_id = %d\n",
                         (dword) msg_header->message_id, 0, 0);
                 }
                 break;
#endif /*(defined(FEATURE_GSM) || defined(FEATURE_WCDMA))*/
                 
             case MS_MN_REG:
                 MN_handle_MN_messaging( (IMH_T *) msg_header);
                 break;
                 
             default:
                 ERR("Unexpected message set, message_set = %d\n",
                     (dword) msg_header->message_set, 0, 0);
                 break;
             } /* switch msg_header... */
                    
         }   /* if status... */
         else
         {
             ERR("Bad message read, status = %d\n", (dword) status, 0, 0);
         }   /* if status... */

	 /* After processing any external message, just check if there is any message
	  * in internal high priority queue */

         mn_cnm_process_internal_messages();

     }   /* while cmd_ptr... */
#ifdef FEATURE_MODEM_HEAP
     if(message_p != NULL)
     {
       modem_mem_free(message_p, MODEM_MEM_CLIENT_NAS);
     }
#endif
     message_p = NULL;    

       
}


/*
 * Function name :  mn_cnm_cmd_q_get()
 * -------------
 *
 * Description :
 * -----------
 *
 * Uses :
 * ----
 *
 * Parameters:
 * ----------
 *
 * Returns:
 * --------
 *
 */

static mn_cnm_cmd_type *mn_cnm_cmd_q_get(void)
{

   /* Read next item from queue */
   return (mn_cnm_cmd_type*)q_get(&mn_cnm_cmd_q);
   
}

/*
 * Function name :  mn_cnm_cmd_internal_q_get()
 * -------------
 *
 * Description :
 * -----------
 *
 * Uses :
 * ----
 *
 * Parameters:
 * ----------
 *
 * Returns:
 * --------
 *
 */

static mn_cnm_cmd_type *mn_cnm_cmd_internal_q_get(void)
{

   /* Read next item from queue */
   return (mn_cnm_cmd_type*)q_get(&mn_cnm_internal_cmd_q);
   
}


/*
 * Function name :  mn_cnm_cmd_q_free()
 * -------------
 *
 * Description :
 * -----------
 *
 * Uses :
 * ----
 *
 * Parameters:
 * ----------
 *
 * Returns:
 * --------
 *
 */

static void mn_cnm_cmd_q_free(mn_cnm_cmd_type *cmd_ptr)
{

   /* Free the memory for the command */
#ifndef FEATURE_MODEM_HEAP
   gs_free( cmd_ptr );
#else
   modem_mem_free(cmd_ptr, MODEM_MEM_CLIENT_NAS);
#endif
}



/*
 * Function name :  mn_cnm_validate_and_set_as_id()
 * -------------
 *
 * Description :
 * -----------
 *
 * Uses : Every message in DSDA goes through this function for MN_CNM task.
 *        This function sets the dsda_as_id after extracting it from message and validating it.
 * ----
 *
 * Parameters: pointer to message
 * ----------
 *
 * Returns: 
 *       TRUE -> if message has valid as_id
 *       else FALSE 
 * --------
 *
 */

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
static boolean mn_cnm_validate_and_set_as_id(IMH_T *msg_header)
{

   sys_modem_as_id_e_type local_as_id = SYS_MODEM_AS_ID_NONE;
   boolean success = FALSE;
   if(mn_dsda_as_id !=0)
   {
     MSG_HIGH_DS_3(MN_SUB,"=MNCNM= current mn_dsda_as_id value = %d, msg_set = %d, msg_id = %d",mn_dsda_as_id,msg_header->message_set,msg_header->message_id);
   }
   if(MNCNM_IS_SINGLE_STACK)
   {
     mn_dsda_as_id = 0;
	 success = TRUE;
   }
   else
   {
      switch (msg_header->message_set)
      {
        case MS_CC_MM:
        case MS_CC_MM_PS:
          {
            switch(msg_header->message_id)
            {
              case MMCNM_DATA_IND:
                {
                  unsigned int  length;
                  GET_IMH_LEN (length, msg_header);
                  local_as_id = ((mmcnm_data_ind_T *)msg_header)->data[length-3];
                }
                break;
              case MMCNM_SRVCC_HANDOVER_COMPLETE_IND:              
                local_as_id = ((mmcnm_srvcc_handover_complete_ind_T*)((void *)msg_header))->as_id;
                break;
              case MMCNM_REL_REQ:
                local_as_id = ((mmcnm_rel_req_T*)((void *)msg_header))->as_id;
                break;
              case MMCC_REEST_CNF:
                local_as_id = ((mmcc_reest_cnf_T *)((void *)msg_header))->as_id;
                break;
              case MMCC_REEST_REJ:
                local_as_id = ((mmcc_reest_rej_T *)((void *)msg_header))->as_id;
                break;
              case MMCNM_REL_IND:
                local_as_id = ((mmcnm_rel_ind_T *)((void *)msg_header))->as_id;
                break;
              case MMCNM_REL_CNF:
                local_as_id = ((mmcnm_rel_cnf_T *)((void *)msg_header))->as_id;
                break;
              case MMCC_SYNC_IND:
                local_as_id = ((mmcc_sync_ind_T *)((void *)msg_header))->as_id;
                break;
              case MMCNM_EST_CNF:
                local_as_id = ((mmcnm_est_cnf_T *)((void *)msg_header))->as_id;
                break;
              case MMCNM_EST_REJ:
                local_as_id = ((mmcnm_est_rej_T *)((void *)msg_header))->as_id;
                break;
              /* Ignore MMCNM_PROMPT_IND as of now */
              case MMCC_SIG_CONN_REL_IND:
                local_as_id = ((mmcc_sig_conn_rel_ind_T *)((void *)msg_header))->as_id;
                break;
   
            }
          }
          break;
   
        case MS_MN_CM:
        case MS_MN_UASMS:
          {
            switch (msg_header->message_id)
            {
              case CM_MO_NORMAL_CALL_REQ:
                local_as_id = ((cm_mo_normal_call_req_T *)((void *)msg_header))->as_id;
                break;
              case CM_MO_EMERGENCY_CALL_REQ:
                local_as_id = ((cm_mo_emergency_call_req_T *)((void *)msg_header))->as_id;
                break;
              case CM_START_DTMF_REQ:
                local_as_id = ((cm_start_dtmf_req_T *)((void *)msg_header))->as_id;
                break;
              case CM_STOP_DTMF_REQ:
                local_as_id = ((cm_stop_dtmf_req_T *)((void *)msg_header))->as_id;
                break;
              case CM_0_SEND_REQ:
              case CM_1_SEND_REQ:
              case CM_2_SEND_REQ:
              case CM_3_SEND_REQ:
              case CM_4_SEND_REQ:
                local_as_id = ((cm_n_send_req_T *)((void *)msg_header))->as_id;
                break;
              case CM_1_X_SEND_REQ:
              case CM_2_X_SEND_REQ:
              case CM_MO_END_X_CALL_REQ:
                local_as_id = ((cm_n_x_send_req_T *)((void *)msg_header))->as_id;
                break;
              case CM_ASID_2_VSID_MAPPING_IND:
                success = TRUE; /* This message doesn't has an as_id */
                break;
              case CM_TRANSFER_SRVCC_CALL_CONTEXT_RES:
                success = TRUE;
                break;
              case CM_4_X_SEND_REQ:
                local_as_id = ((cm_4_x_send_req_T *)((void *)msg_header))->as_id;
                break;
              case CM_MO_END_CALL_REQ:
                local_as_id = ((cm_mo_end_call_req_T *)((void *)msg_header))->as_id;
                break;
              case CM_ABORT_CC_REQ:
                local_as_id = ((cm_abort_cc_req_T *)((void *)msg_header))->as_id;
                break;
   #ifdef FEATURE_UUS
              case CM_MO_USER_DATA_REQ:
                local_as_id = ((cm_mo_user_data_req_T *)((void *)msg_header))->as_id;
                break;
   #endif
   #if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
              case CM_MO_MODIFY_REQ:
                local_as_id = ((cm_mo_modify_req_T *)((void *)msg_header))->as_id;
                break;
              case CM_MT_MODIFY_RES:
                local_as_id = ((cm_mt_modify_res_T *)((void *)msg_header))->as_id;
                break;
   #endif
              case CM_MT_CALL_RES:
                local_as_id = ((cm_mt_call_res_T *)((void *)msg_header))->as_id;
                break;
              case CM_MT_SETUP_RES:
                local_as_id = ((cm_mt_setup_res_T *)((void *)msg_header))->as_id;
                break;
              case CM_REGISTER_SS_REQ:
                local_as_id = ((cm_register_ss_req_T *)((void *)msg_header))->as_id;
                break;
              case CM_ERASE_SS_REQ:
                local_as_id = ((cm_erase_ss_req_T *)((void *)msg_header))->as_id;
                break;
              case CM_ACTIVATE_SS_REQ:
                local_as_id = ((cm_activate_ss_req_T *)((void *)msg_header))->as_id;
                break;
              case CM_DEACTIVATE_SS_REQ:
                local_as_id = ((cm_deactivate_ss_req_T *)((void *)msg_header))->as_id;
                break;
              case CM_INTERROGATE_SS_REQ:
                local_as_id = ((cm_interrogate_ss_req_T *)((void *)msg_header))->as_id;
                break;
              case CM_REGISTER_PASSWORD_REQ:
                local_as_id = ((cm_register_password_req_T *)((void *)msg_header))->as_id;
                break;
              case CM_PROCESS_UNSTRUCTURED_SS_DATA:
                local_as_id = ((cm_process_unstructured_ss_data_T *)((void *)msg_header))->as_id;
                break;
              case CM_PROCESS_UNSTRUCTURED_SS_REQ:
                local_as_id = ((cm_process_unstructured_ss_req_T *)((void *)msg_header))->as_id;
                break;
              case CM_RELEASE_SS_REQ:
                  local_as_id = ((cm_release_ss_req_T *)((void *)msg_header))->as_id;
                  break;
              case CM_ABORT_SS_REQ:
                  local_as_id = ((cm_abort_ss_req_T *)((void *)msg_header))->as_id;
                  break;
              case CM_UNSTRUCTURED_SS_NOTIFY_RES:
                  local_as_id = ((cm_unstructured_ss_notify_res_T *)((void *)msg_header))->as_id;
                  break;
              case CM_UNSTRUCTURED_SS_RES:
                  local_as_id = ((cm_unstructured_ss_res_T *)((void *)msg_header))->as_id;
                  break;
              case CM_GET_PASSWORD_RES:
                  local_as_id = ((cm_get_password_res_T *)((void *)msg_header))->as_id;
                  break;
              case CM_MT_CALL_SS_RES:
                  local_as_id = ((cm_mt_call_ss_res_T *)((void *)msg_header))->as_id;
                  break;
   
   #if defined ( FEATURE_CM_MMGPS ) || defined ( FEATURE_CM_LCS )
              case CM_LCS_MOLR_REQ:
                  local_as_id = ((cm_lcs_molr_req_T *)((void *)msg_header))->as_id;
                  break;
              case CM_LCS_MOLR_COMPLETED:
                  local_as_id = ((cm_lcs_molr_compl_T *)((void *)msg_header))->as_id;
                  break;
              case CM_LCS_LOCATION_NOTIFICATION_RES:
                  local_as_id = ((cm_lcs_location_notification_res_T *)((void *)msg_header))->as_id;
                  break;
   #endif
              case UASMS_SM_DATA_REQ:
                  local_as_id = ((SM_RL_DATA_REQ_T *)((void *)msg_header))->as_id;
                  break;
              case UASMS_MEMORY_AVAIL_NOTIFICATION_REQ:
                  local_as_id = ((SM_RL_MEMORY_AVAILABLE_REQ_T *)((void *)msg_header))->as_id;
                  break;
              case UASMS_ABORT_SM_REQ:
                  local_as_id = ((SM_RL_ABORT_SM_REQ_T *)((void *)msg_header))->as_id;
                  break;
              case UASMS_SM_REPORT_REQ:
                  local_as_id = ((SM_RL_REPORT_REQ_T *)((void *)msg_header))->as_id;
                  break;
              case UASMS_SM_LINK_CONTROL_REQ:
                  local_as_id = ((SM_RL_LINK_CONTROL_REQ_T *)((void *)msg_header))->as_id;
                  break;
              case UASMS_SM_UPDATE_TIMERF:
                  local_as_id = ((SM_RL_UPDATE_TIMERF_T *)((void *)msg_header))->as_id;
                  break;
   
              /*
                           Ignore CM_5_SEND_REQ, CM_RECALL_RSP, CM_ERASE_CC_ENTRY_REQ and CM_PROMPT_RSP for now.
                        */
            }
          }
          break;
   
        case MS_MN_CC:
          {
            switch (msg_header->message_id)
            {
              case MNCC_ALERT_IND:
                  local_as_id = ((MNCC_ALERT_IND_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_CALL_HOLD_CNF:
                  local_as_id = ((MNCC_CALL_HOLD_CNF_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_CALL_HOLD_REJ:
                  local_as_id = ((MNCC_CALL_HOLD_REJ_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_CALL_PROC_IND:
                  local_as_id = ((MNCC_CALL_PROC_IND_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_CALL_RETRIEVE_CNF:
                  local_as_id = ((MNCC_CALL_RETRIEVE_CNF_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_CALL_RETRIEVE_REJ:
                  local_as_id = ((MNCC_CALL_RETRIEVE_REJ_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_DISC_IND:
                  local_as_id = ((MNCC_DISC_IND_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_ERR_IND:
                  local_as_id = ((MNCC_ERR_IND_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_FACILITY_IND:
                  local_as_id = ((MNCC_FACILITY_IND_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_NOTIFY_DEFERRED_IND:
                  local_as_id = ((MNCC_NOTIFY_DEFERRED_IND_T *)((void *)msg_header))->as_id;
                  break;
    #ifdef FEATURE_UUS
              case MNCC_USER_DATA_DEFERRED_IND:
                  local_as_id = ((MNCC_USER_DATA_DEFERRED_IND_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_USER_USER_DATA_IND:
                  local_as_id = ((MNCC_USER_USER_DATA_IND_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_CONGESTION_IND:
                  local_as_id = ((MNCC_CONGESTION_IND_T *)((void *)msg_header))->as_id;
                  break;
    #endif
    #if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
              case MNCC_MODIFY_CNF:
                  local_as_id = ((MNCC_MODIFY_CNF_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_MODIFY_IND:
                  local_as_id = ((MNCC_MODIFY_IND_T *)((void *)msg_header))->as_id;
                  break;
    #endif
              case MNCC_NOTIFY_IND:
                  local_as_id = ((MNCC_NOTIFY_IND_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_PROGRESS_IND:
                  local_as_id = ((MNCC_PROGRESS_IND_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_REEST_IND:
                  local_as_id = ((MNCC_REEST_IND_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_REJ_IND:
                  local_as_id = ((MNCC_REJ_IND_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_REL_CNF:
                  local_as_id = ((MNCC_REL_CNF_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_REL_IND:
                  local_as_id = ((MNCC_REL_IND_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_SETUP_CNF:
                  local_as_id = ((MNCC_SETUP_CNF_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_SETUP_COMPL_IND:
                  local_as_id = ((MNCC_SETUP_COMPL_IND_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_SETUP_IND:
                  local_as_id = ((MNCC_SETUP_IND_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_START_DTMF_CNF:
                  local_as_id = ((MNCC_START_DTMF_CNF_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_STOP_DTMF_CNF:
                  local_as_id = ((MNCC_STOP_DTMF_CNF_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_SYNC_IND:
                  local_as_id = ((MNCC_SYNC_IND_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_RAB_REL_IND:
                  local_as_id = ((MNCC_RAB_REL_IND_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_CS_SIG_CONN_REL_IND:
                  local_as_id = ((MNCC_CS_SIG_CONN_REL_IND_T *)((void *)msg_header))->as_id;
                  break;
    
    #ifdef FEATURE_CCBS
              case MNCC_RECALL_IND:
                  local_as_id = ((MNCC_RECALL_IND_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_CC_EST_IND:
                  local_as_id = ((MNCC_CC_EST_IND_T *)((void *)msg_header))->as_id;
                  break;
    #endif /* FEATURE_CCBS */
              case MNCC_SRVCC_HANDOVER_COMPLETE_IND:
                  local_as_id = ((MNCC_SRVCC_HANDOVER_COMPLETE_IND_T *)((void *)msg_header))->as_id;
                  break;

			  case MNCC_SRVCC_HANDOVER_FAIL_REQ:
                   local_as_id = ((MNCC_SRVCC_HANDOVER_FAIL_REQ_T *)((void *)msg_header))->as_id;
                   break;
   
              case MNCC_ALERT_REQ:
                  local_as_id = ((MNCC_ALERT_REQ_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_CALL_CONF_REQ:
                  local_as_id = ((MNCC_CALL_CONF_REQ_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_CALL_HOLD_REQ:
                  local_as_id = ((MNCC_CALL_HOLD_REQ_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_CALL_RETRIEVE_REQ:
                  local_as_id = ((MNCC_CALL_RETRIEVE_REQ_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_DISC_REQ:
                  local_as_id = ((MNCC_DISC_REQ_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_FACILITY_REQ:
                  local_as_id = ((MNCC_FACILITY_REQ_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_REJ_REQ:
                  local_as_id = ((MNCC_REJ_REQ_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_REL_REQ:
                  local_as_id = ((MNCC_REL_REQ_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_REL_COMPL_REQ:
                  local_as_id = ((MNCC_REL_COMPL_REQ_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_MODIFY_REQ:
                  local_as_id = ((MNCC_MODIFY_REQ_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_MODIFY_RES:
                  local_as_id = ((MNCC_MODIFY_RES_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_NOTIFY_REQ:
                  MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Unexpected message MNCC_NOTIFY_REQ");
                  break;
              case MNCC_SETUP_COMPL_REQ:
                  local_as_id = ((MNCC_SETUP_COMPL_REQ_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_SETUP_COMPL_RES:
                  local_as_id = ((MNCC_SETUP_COMPL_RES_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_SETUP_REQ:
                  local_as_id = ((MNCC_SETUP_REQ_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_SETUP_RES:
                  local_as_id = ((MNCC_SETUP_RES_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_START_DTMF_REQ:
                  local_as_id = ((MNCC_START_DTMF_REQ_T *)((void *)msg_header))->as_id;
                  break;
              case MNCC_STOP_DTMF_REQ:
                  local_as_id = ((MNCC_STOP_DTMF_REQ_T *)((void *)msg_header))->as_id;
                  break;
   #ifdef FEATURE_UUS
              case MNCC_USER_USER_DATA_REQ:
                  local_as_id = ((MNCC_USER_USER_DATA_REQ_T *)((void *)msg_header))->as_id;
                  break;
   #endif
              case MNCC_MODIFY_ACCEPT:
                  local_as_id = ((MNCC_MODIFY_ACCEPT_T *)((void *)msg_header))->as_id;
                  break;
            }
          }
          break;
   
         case MS_MN_SS:
          {
            switch (msg_header->message_id)
            {
              case MNSS_BEGIN_IND:
                  local_as_id = ((MNSS_BEGIN_IND_T *)((void *)msg_header))->as_id;
                  break;
              case MNSS_END_IND:
                  local_as_id = ((MNSS_END_IND_T *)((void *)msg_header))->as_id;
                  break;
              case MNSS_FACILITY_IND:
                  local_as_id = ((MNSS_FACILITY_IND_T *)((void *)msg_header))->as_id;
                  break;
              case MNSS_REJ_IND:
                  local_as_id = ((MNSS_REJ_IND_T *)((void *)msg_header))->as_id;
                  break;
   
              case MNSS_BEGIN_REQ:
                  local_as_id = ((MNSS_BEGIN_REQ_T *)((void *)msg_header))->as_id;
                  break;
              case MNSS_FACILITY_REQ:
                  local_as_id = ((MNSS_FACILITY_REQ_T *)((void *)msg_header))->as_id;
                  break;
              case MNSS_END_REQ:
                  local_as_id = ((MNSS_END_REQ_T *)((void *)msg_header))->as_id;
                  break;
            }
          }
          break;
   
         case MS_MN_SMS:
          {
            switch (msg_header->message_id)
            {
              case MNSMS_DATA_IND:
                  local_as_id = ((MNSMS_DATA_IND_T *)((void *)msg_header))->as_id;
                  break;
              case MNSMS_EST_IND:
                  local_as_id = ((MNSMS_EST_IND_T *)((void *)msg_header))->as_id;
                  break;
              case MNSMS_ERROR_IND:
                  local_as_id = ((MNSMS_ERROR_IND_T *)((void *)msg_header))->as_id;
                  break;
              case MNSMS_CPACK_IND:
                  local_as_id = ((MNSMS_CPACK_IND_T *)((void *)msg_header))->as_id;
                  break;
   
              case MNSMS_DATA_REQ:
                  local_as_id = ((MNSMS_DATA_REQ_T *)((void *)msg_header))->as_id;
                  break;
              case MNSMS_ABORT_REQ:
                  local_as_id = ((MNSMS_ABORT_REQ_T *)((void *)msg_header))->as_id;
                  break;
              case MNSMS_REL_REQ:
                  local_as_id = ((MNSMS_REL_REQ_T *)((void *)msg_header))->as_id;
                  break;
              case MNSMS_EST_REQ:
                  local_as_id = ((MNSMS_EST_REQ_T *)((void *)msg_header))->as_id;
                  break;
   
            }
          }
          break;
   
         case MS_TIMER:
          {
            if (msg_header->message_id == MID_TIMER_EXPIRY)
            {
              /* Extract AS ID from Most significant bit in Timer ID */
              local_as_id = (sys_modem_as_id_e_type)((((timer_expiry_T *)msg_header)->timer_id >> MNCNM_TIMER_ID_SIM_POS) & 0x01);
              MSG_HIGH_DS_2(MN_SUB,"=MNCNM= @Debug! timer_id = %d, after processing, local_as_id = %d",((timer_expiry_T *)msg_header)->timer_id,local_as_id);
              ((timer_expiry_T *)msg_header)->timer_id &= ~(0x01 << (MNCNM_TIMER_ID_SIM_POS));
              MSG_HIGH_DS_1(MN_SUB,"=MNCNM= @Debug! timer_id = %d after removing as_id",((timer_expiry_T *)msg_header)->timer_id);
            }
            else
            {
              MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Unexpected timer type %d", msg_header->message_id );
            } 
            break;
          }
          break;
   
      }
   
      /* Note coding needs to be done if msg set is MS_TIMER */
   
      if(success == FALSE)
      {
         success = mn_cnm_validate_as_id(local_as_id);
         if(success == TRUE)
         {
            MSG_HIGH_DS_3(MN_SUB,"=MNCNM= Setting mn_dsda_as_id to %d, msg_set = %d, msg_id = %d",local_as_id,msg_header->message_set,msg_header->message_id);
            mn_dsda_as_id = (mn_dsda_as_id_e_type)local_as_id;
         }
         else
         {
            MSG_ERROR_DS_3(MN_SUB,"=MNCNM=  Can't set mn_dsda_as_id, Invalid AS ID:%d - Ignoring message MS: %d   MSG_ID: %d", 
                     mn_dsda_as_id,msg_header->message_set, msg_header->message_id);
         }
      }
      else
      {
        
        MSG_HIGH_DS_2(MN_SUB,"=MNCNM= Message doesn't has an as_id, msg_set = %d, msg_id = %d",msg_header->message_set,msg_header->message_id);
      }
   
   }
   return success;

}

/*
 * Function name :  mn_cnm_validate_as_id()
 * -------------
 *
 * Description :
 * -----------
 *
 * Uses : 
 *        This function validates the as_id is valid
 * ----
 *
 * Parameters: as_id
 * ----------
 *
 * Returns: 
 *       TRUE -> if valid as_id
 *       else FALSE 
 * --------
 *
 */


static boolean mn_cnm_validate_as_id(sys_modem_as_id_e_type local_as_id)
{

  if(local_as_id == SYS_MODEM_AS_ID_1 || local_as_id == SYS_MODEM_AS_ID_2
    || local_as_id == SYS_MODEM_AS_ID_3)
    return TRUE;
  else
    return FALSE;

}

#endif


