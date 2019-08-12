/*=============================================================================

                 err_qdi.c

GENERAL DESCRIPTION
   Implements QDI layer for Err that goes into main image

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2014 - 15
by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.
 
$Header: //components/rel/core.mpss/3.5.c12.3/debugtools/err/src/err_qdi.c#1 $ 
$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/01/15   abh     Added changes for extended smem logging
08/27/14   din     Replaced snprintf with mutex free calls.
07/31/14   din     File created to support Error on User PD.

=============================================================================*/


/*****************************************************************************/
/*                           INCLUDE FILES                                   */
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stringl/stringl.h>
#include "err_qdi.h"
#include "pd_mon_restart.h"
#include "err.h"
#include "tms_utils.h"
#include "err_smem_log.h"
#include "timer.h"

/*****************************************************************************/
/*                         MACRO DECLARATIONS                                */
/*****************************************************************************/

/* Timeout value for pdtimeout timer, Unit in Msecs */
#define ERR_QDI_PDTIMEOUT_TIMER_PERIOD_DEFAULT  2000

/*****************************************************************************/
/*                         DATA DECLARATIONS                                 */
/*****************************************************************************/
typedef struct err_qdi_opener_struct 
{
  /* Should be the first member */
  qurt_qdi_obj_t                 qdiobj;
  err_qdi_internal_s             user_pd_data;
  struct err_qdi_opener_struct   *next;
}err_qdi_opener_s;

typedef struct
{
  qurt_mutex_t     mutex;
  uint32           pd_callback_timeout_msec;
  uint32           pd_exit_timeout_msec;
  boolean          subsystem_pdr_enabled;
  err_qdi_opener_s *list;
}err_qdi_s;

static err_qdi_s err_qdi;


/* MACRO for locking/unlocking the qmutex*/
#define ERR_QDI_MUTEX_LOCK()  \
   qurt_pimutex_lock(&err_qdi.mutex); 
  
#define ERR_QDI_MUTEX_FREE()  \
   qurt_pimutex_unlock(&err_qdi.mutex); 

#define ERR_QDI_MUTEX_INIT() \
   qurt_pimutex_init ( &err_qdi.mutex );



/*****************************************************************************/
/*                      FUNCTION DECLARATIONS                                */
/*****************************************************************************/
extern void err_get_timetick(DalTimetickTime64Type * tick);
extern void err_qdi_pdr_records_user_err_info(err_qdi_internal_s * dataobj);
extern void err_qdi_pdr_records_init(void);
extern void err_qdi_dalcfg_get_pd_timeout
( 
  uint32 * pd_timeout_cb_msec, 
  uint32 * pd_timeout_exit_msec 
);

int err_qdi_invoke(int client_handle,
                          qurt_qdi_obj_t *pobj,
                          int method,
                          qurt_qdi_arg_t a1,
                          qurt_qdi_arg_t a2,
                          qurt_qdi_arg_t a3,
                          qurt_qdi_arg_t a4,
                          qurt_qdi_arg_t a5,
                          qurt_qdi_arg_t a6,
                          qurt_qdi_arg_t a7,
                          qurt_qdi_arg_t a8,
                          qurt_qdi_arg_t a9);


/*=============================================================================

FUNCTION ERR_QDI_PROCESS_PDTIMEOUT_TIMER_EXPIRY

DESCRIPTION
  Crashes the system in the event of user PD error callbacks taking more than 
  expected time or during PD exit failure

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None.

=============================================================================*/

void err_qdi_process_pdtimeout_timer_expiry( timer_cb_data_type data )
{
  err_qdi_internal_s * dataobj;
  char                 temp[ERR_QDI_PFR_LEN];
  uint32               buf_len;

  err_qdi_opener_s * clntobj = (err_qdi_opener_s *)(data);
  
  if( clntobj == NULL )
  {
    ERR_FATAL("QDI layer Misbehaved, Shouldn't reach here ", 0,0,0);
    return ;
  }
  
  ERR_QDI_MUTEX_LOCK();
  
  dataobj = &(clntobj->user_pd_data);
   
  buf_len = tms_utils_fmt(temp, ERR_QDI_PFR_LEN ,"PID %u : %s ", 
               (unsigned int )dataobj->pid, dataobj->pfr );
    
  if ( dataobj->state == ERR_QDI_USERPD_ERR_INITIATED )
  {
    ERR_QDI_MUTEX_FREE(); 
    /* This branch will not return */
    ERR_FATAL_AUX_MSG(" USER PD crashed, it's callbacks are stuck, check coredump.err.aux_msg",
		     temp, buf_len);	
  }
  else
  {
    ERR_QDI_MUTEX_FREE();  
    /* This branch will not return */
    ERR_FATAL_AUX_MSG(" USER PD crashed, PD_exit is stuck, check coredump.err.aux_msg",
		     temp, buf_len);	
  }
} /* err_qdi_process_pdtimeout_timer_expiry */


/*=============================================================================

FUNCTION ERR_QDI_TERMINATE

DESCRIPTION
 Releases the Qdi object that corresponding to a process that got destroyed

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None.

=============================================================================*/
void err_qdi_terminate(qurt_qdi_obj_t *obj)
{
  err_qdi_opener_s *temp, *clntobj = (err_qdi_opener_s *)obj;

  err_qdi_internal_s * dataobj;

  if( clntobj == NULL )
  {
    ERR_FATAL("QDI layer Misbehaved, Shouldn't reach here ", 0,0,0);
    return ;
  }
  
  ERR_QDI_MUTEX_LOCK();
  
  dataobj = &(clntobj->user_pd_data);

  /* Clear the pdtimeout timer if set */
  if ( dataobj->pdtimeout_timer_set == TRUE )
  {
    (void)timer_clr_64( &(dataobj->pdtimeout_timer), T_MSEC );
    dataobj->pdtimeout_timer_set = FALSE;
  }

  if ( err_qdi.list == NULL  )
  {
    ERR_QDI_MUTEX_FREE();
    return;
  }

  if ( err_qdi.list != NULL && err_qdi.list == clntobj )
  {
    err_qdi.list = clntobj->next;
    free(clntobj);
    ERR_QDI_MUTEX_FREE();
    return;
  }
  
  for (temp = err_qdi.list; NULL != temp; temp = temp->next)
  {
    if ( temp->next == clntobj )
    {
      temp->next = clntobj->next;
      free(clntobj);
      ERR_QDI_MUTEX_FREE();
      return;
    }
  }

  ERR_QDI_MUTEX_FREE();

  /* Should not reach here */
  ERR_FATAL("Release Handle not found in the list", 0,0,0);

}/* err_qdi_terminate */


/*=============================================================================

FUNCTION ERR_QDI_SEND_PDR_INFO

DESCRIPTION
 Updates PDR policy to Root PD , if there is a change.

DEPENDENCIES
  None

RETURN VALUE
 ERR_QDI_SUCCESS - for success
 ERR_QDI_FAIL    - for failure

SIDE EFFECTS
  None

=============================================================================*/
err_qdi_e err_qdi_send_pdr_info
(
  err_qdi_opener_s * clntobj, 
  boolean            user_pdr_enabled
)
{
  if( clntobj == NULL )
  {
    ERR_FATAL("QDI layer Misbehaved, Shouldn't reach here ", 0,0,0);
    return ERR_QDI_NOT_INITIALIZED;
  }

  ERR_QDI_MUTEX_LOCK(); 

  clntobj->user_pd_data.pdr_enabled = user_pdr_enabled;

  ERR_QDI_MUTEX_FREE();
  
  return ERR_QDI_SUCCESS;

} /* err_qdi_send_pdr_info */
	
/*=============================================================================

FUNCTION ERR_QDI_INFORM_PFR

DESCRIPTION
 Communicates PFR to Root PD in case of error/exceptions in User PD

DEPENDENCIES
  None

RETURN VALUE
 ERR_QDI_SUCCESS - for success
 ERR_QDI_FAIL    - for failure

SIDE EFFECTS
  None

=============================================================================*/
err_qdi_e err_qdi_inform_pfr
(
   int                client_handle,
   err_qdi_opener_s   *clntobj, 
   uint32             pfr_len, 
   char               *pfr
)
{
  err_qdi_internal_s * dataobj;
  int                  result;
  
  if( clntobj == NULL )
  {
    ERR_FATAL("QDI layer Misbehaved, Shouldn't reach here ", 0,0,0);
    /* Deliberate */
    return ERR_QDI_NOT_INITIALIZED;
  }

  ERR_QDI_MUTEX_LOCK();
  
  dataobj = &(clntobj->user_pd_data);

  /* Not Checking for err_initialized. If this routine is called, 
   * pfr would be updated */

  dataobj->pfr_len = pfr_len > ERR_QDI_PFR_LEN ? ERR_QDI_PFR_LEN : pfr_len;

  result = qurt_qdi_copy_from_user(client_handle, dataobj->pfr, 
                                   pfr, dataobj->pfr_len);

  if (result)
  {
    ERR_QDI_MUTEX_FREE();
    return ERR_QDI_INVALID_PARAMETERS;
  }

  /* Guarantee NULL termination */
  if ( dataobj->pfr_len >= 1 )
   dataobj->pfr[dataobj->pfr_len -1] =0;

  err_get_timetick(&(dataobj->start_time));

  /* Set the timer to cover timeout cases for user PD 
     error callbacks failure */
  timer_set_64( &(dataobj->pdtimeout_timer), 
		  err_qdi.pd_callback_timeout_msec, 0, T_MSEC  );

  dataobj->pdtimeout_timer_set = TRUE;

  dataobj->state = ERR_QDI_USERPD_ERR_INITIATED;

  ERR_QDI_MUTEX_FREE();

  return ERR_QDI_SUCCESS; 

} /* err_qdi_inform_pfr */


/*=============================================================================

FUNCTION ERR_QDI_ERR_HANDLING_DONE

DESCRIPTION
 Communicates to Root PD that handling routine on User PD is complete.

DEPENDENCIES
  None

RETURN VALUE
 ERR_QDI_SUCCESS - for success
 ERR_QDI_FAIL    - for failure

SIDE EFFECTS
  None

=============================================================================*/
err_qdi_e err_qdi_err_handling_done
(
  err_qdi_opener_s   *clntobj
)
{
  err_qdi_internal_s * dataobj;
  int                  buf_len;
  char                 temp[ERR_QDI_PFR_LEN];
  
  if( clntobj == NULL )
  {
    ERR_FATAL("QDI layer Misbehaved, Shouldn't reach here ", 0,0,0);
    /* Deliberate */
    return ERR_QDI_NOT_INITIALIZED;
  }

  ERR_QDI_MUTEX_LOCK();
  
  dataobj = &(clntobj->user_pd_data);
  
  /* State should be ERR_QDI_USERPD_INITIALIZED or ERR_QDI_USERPD_ERR_INITIATED */
  if(dataobj->state < ERR_QDI_USERPD_INITIALIZED)
  {
    ERR_QDI_MUTEX_FREE();
    return ERR_QDI_NOT_INITIALIZED;
  } 

  /* Clear the pdtimeout timer if set */
  if ( dataobj->pdtimeout_timer_set == TRUE )
  {
    (void)timer_clr_64( &(dataobj->pdtimeout_timer), T_MSEC );
    dataobj->pdtimeout_timer_set = FALSE;
  }

  err_get_timetick(&(dataobj->end_time));

  /* Mark the flag to indicate that the SMEM buffer is updated already.
     * Also, we dont want to rewrite the smem buffer after it has already
     * been written once.
     */
  err_info_to_smem_buffer_written();

  dataobj->state = ERR_QDI_USERPD_ERR_HANDLING_COMPLETED;

  if ( !( err_qdi.subsystem_pdr_enabled && dataobj->pdr_enabled) )
  {
    buf_len = tms_utils_fmt(temp, ERR_QDI_PFR_LEN ,"PID %u : %s ", 
                  (unsigned int )dataobj->pid, dataobj->pfr );

    /* This branch will not return */
    ERR_FATAL_AUX_MSG("USER PD crashed, see coredump.aux_msg for more details", 
		            temp, buf_len);

  }
  else
  {
    dataobj->state = ERR_QDI_USERPD_ERR_PD_EXIT_STARTED;
    
    //TODO : Add process exit notification to clear the pdtimeout timer.
    //int qurt_event_register(int type, int value, qurt_signal_t *signal, unsigned int mask, void *data, unsigned int data_size);
    
    /* Record PDR info */
    err_qdi_pdr_records_user_err_info( dataobj);
  
    /* Set the timer to cover timeout cases for PD exit pdtimeouts */
    //timer_set_64( &(dataobj->pdtimeout_timer), err_qdi.pd_exit_timeout_msec, 0, T_MSEC  );

    //dataobj->pdtimeout_timer_set = TRUE;
    
    ERR_QDI_MUTEX_FREE();
    
    if ( dataobj->pfr_len >= 1 )
    {
      MSG_SPRINTF_2(MSG_SSID_TMS, MSG_LEGACY_HIGH, " PID %u crashed : %s", 
                  (unsigned int )dataobj->pid, dataobj->pfr );
    }
  }

  return ERR_QDI_SUCCESS;

} /* err_qdi_err_handling_done */

/*=============================================================================

FUNCTION ERR_QDI_OPEN

DESCRIPTION
 Creates err driver object for the client and initializes required details.

DEPENDENCIES
  None

RETURN VALUE
 QURT_EOK if the driver obj is defined and set correctly.

SIDE EFFECTS
  None

=============================================================================*/
static int err_qdi_open(int client_handle)
{
   err_qdi_internal_s * dataobj;
   qurt_sysenv_procname_t pn;
   int rc = -1;
   err_qdi_opener_s *clntobj = calloc(1, sizeof(err_qdi_opener_s));
   
   if ( clntobj == NULL )
   {
     return -1;
   }

   /* QDI_OS_SYSENV is the standard method for requesting 
    * this type of information */
   /* QDI_HANDLE_LOCAL_CLIENT means that this request originated locally */
   /* QURT_SYSENV_PROCNAME_TYPE is the specific piece of data being requested 
    * -- to fill in a qurt_sysenv_procname_t structure */
   /* &pn is the structure we want filled in */

   if (0 > (rc = qurt_qdi_handle_invoke(client_handle, QDI_OS_SYSENV, 
				        QDI_HANDLE_LOCAL_CLIENT, 
					QURT_SYSENV_PROCNAME_TYPE, &pn)) )
   {
      MSG_2(MSG_SSID_TMS, MSG_LEGACY_HIGH, 
		      "Cannot obtain client info %d %d", pn.asid, rc);
      return rc;
   }

   ERR_QDI_MUTEX_LOCK();
   
   clntobj->qdiobj.invoke    = err_qdi_invoke;
   clntobj->qdiobj.refcnt    = QDI_REFCNT_INIT;
   clntobj->qdiobj.release   = err_qdi_terminate;

   dataobj = &(clntobj->user_pd_data);
   
   /* Populate PD name and PID */
   strlcpy(dataobj->pd_name, pn.name, ERR_QDI_DEVICE_NAME_LEN);
   dataobj->pid = pn.asid;
   dataobj->state = ERR_QDI_USERPD_INITIALIZED;

   /* Insert at the head of the list */
   clntobj->next = err_qdi.list;
   err_qdi.list= clntobj;

   timer_def_osal( &(dataobj->pdtimeout_timer), NULL, 
                   TIMER_FUNC1_CB_TYPE, 
		   &err_qdi_process_pdtimeout_timer_expiry, (uint32)clntobj );

   ERR_QDI_MUTEX_FREE();
   
   return qurt_qdi_handle_create_from_obj_t(client_handle, &clntobj->qdiobj);

} /* err_qdi_open */

/*=============================================================================

FUNCTION ERR_QDI_INVOKE

DESCRIPTION
 This function contains the switch which maps to all methods

DEPENDENCIES
  None

RETURN VALUE


SIDE EFFECTS
  None

=============================================================================*/
int err_qdi_invoke(int client_handle,
                          qurt_qdi_obj_t *pobj,
                          int method,
                          qurt_qdi_arg_t a1,
                          qurt_qdi_arg_t a2,
                          qurt_qdi_arg_t a3,
                          qurt_qdi_arg_t a4,
                          qurt_qdi_arg_t a5,
                          qurt_qdi_arg_t a6,
                          qurt_qdi_arg_t a7,
                          qurt_qdi_arg_t a8,
                          qurt_qdi_arg_t a9)
{
   /* Cannot be used for QDI_OPEN */
   err_qdi_opener_s* clntobj = (err_qdi_opener_s*)pobj;
   
   switch(method)
   {
      case QDI_OPEN: 
         return err_qdi_open(client_handle);
      
      case ERR_QDI_SEND_PDR_INFO:
	 return err_qdi_send_pdr_info(clntobj, a1.num);
			 
      case ERR_QDI_INFORM_PFR:  
         return err_qdi_inform_pfr(client_handle, clntobj, a1.num, a2.ptr );

      case ERR_QDI_ERR_HANDLING_DONE:  
         return err_qdi_err_handling_done(clntobj);

      case QDI_CLOSE:
	 //err_qdi_terminate(pobj);
	 // No break;

      default:
         return qurt_qdi_method_default(client_handle, pobj, method,
                                     a1, a2, a3, a4, a5, a6, a7, a8, a9);
   }
} /* err_qdi_invoke */


/*=============================================================================

FUNCTION ERR_QDI_INIT

DESCRIPTION
 Registers with QDI Framework for ERR QDI
 
DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None

=============================================================================*/
void err_qdi_release(qurt_qdi_obj_t *obj)
{
  
}

/*=============================================================================

FUNCTION ERR_QDI_INIT

DESCRIPTION
 Registers with QDI Framework for ERR QDI
 
DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None

=============================================================================*/

static qurt_qdi_obj_t err_qdi_opener_obj;

void err_qdi_init 
(
  void
) 
{
   /* Initialize the mutex here */
   ERR_QDI_MUTEX_INIT();

   err_qdi_opener_obj.invoke    = err_qdi_invoke;
   err_qdi_opener_obj.refcnt    = QDI_REFCNT_INIT;
   err_qdi_opener_obj.release   = err_qdi_release;
   
   qurt_qdi_register_devname(ERR_DRIVER_NAME, &err_qdi_opener_obj);

   if (pd_mon_restart_dalcfg_check() )
   {
     err_qdi.subsystem_pdr_enabled = TRUE;
   }

   err_qdi_dalcfg_get_pd_timeout(&err_qdi.pd_callback_timeout_msec,
		                 &err_qdi.pd_exit_timeout_msec );

   if ( err_qdi.pd_callback_timeout_msec == 0 )
   {
     err_qdi.pd_callback_timeout_msec = ERR_QDI_PDTIMEOUT_TIMER_PERIOD_DEFAULT;
   }

   if ( err_qdi.pd_exit_timeout_msec == 0 )
   {
     err_qdi.pd_exit_timeout_msec = ERR_QDI_PDTIMEOUT_TIMER_PERIOD_DEFAULT;
   } 
   
   err_qdi_pdr_records_init();

} /* err_qdi_init */
