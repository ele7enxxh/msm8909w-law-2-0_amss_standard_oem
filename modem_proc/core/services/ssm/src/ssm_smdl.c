/**
@file ssm_smdl.c
@brief
This module implements the Secured Services Functionality.
*/
/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The SSM_mainpage.dox file contains all file/group descriptions that
      are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      SSM_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the SSM group 
      description in the SSM_mainpage.dox file. 
=============================================================================*/

/*=============================================================================
  Copyright (c) 2012 - 2014 by QUALCOMM Technologies Incorporated. 
  All rights reserved.
  Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                 S E C U R E   S E R V I C E S  M O D U L E

GENERAL DESCRIPTION
  Implements SSM IPC

EXTERNALIZED FUNCTIONS

  INITIALIZATION AND SEQUENCING REQUIREMENTS

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/ssm/src/ssm_smdl.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/02/14   psu     Update diag macros using optimized versions.
04/29/13   din     Changes pertaining to ADSP were done.
08/24/12   din     Removed warnings pertaining to insertion of task.h
07/20/12   din     Removed key exchange code from SSM.
01/20/12   ab      Added Key Exchange Try call when SMD channel gets established
09/03/11   din     File created.

=============================================================================*/

/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "smd_lite.h"
#include "ssm_task_v.h"
#include "ssm_smdl_v.h"
#include "ssm_ipc_v.h"
#include "ssm_v.h"
#include "queue.h"
#ifdef FEATURE_SSM_REX_API_PRESENT
#include "rex.h"
#else /*  FEATURE_SSM_REX_API_PRESENT */
#include "qurt.h"
#endif /* FEATURE_SSM_REX_API_PRESENT */
#include <stdlib.h>

/* Port names */
#define SSM_NAME_MODEM_APPS "SSM_RTR_MODEM_APPS"
#define SSM_NAME_MODEM_ADSP "SSM_RTR_MODEM_ADSP"

/* Default SMD Edge */
#define SSM_SMDL_EDGE_MODEM_APPS  SMD_APPS_MODEM
#define SSM_SMDL_EDGE_MODEM_ADSP  SMD_MODEM_QDSP

/* Default FIFO Size */
#define SSM_SMDL_DEFAULT_FIFO SMD_STANDARD_FIFO

/* Default open flags Currently only packet mode is supported */
#define SSM_SMDL_DEFAULT_MODE SMDL_OPEN_FLAGS_MODE_PACKET

/* If only default parameters are required, then this can be used */
#define SSM_SMDL_DEFAULT_PARAM {	\
  SSM_SMDL_DEFAULT_NAME,		\
  SSM_SMDL_DEFAULT_EDGE,		\
  SSM_SMDL_DEFAULT_FIFO,		\
  SSM_SMDL_DEFAULT_MODE		\
}


/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/

/* SSM SMDL structure */
typedef struct 
{
  /* Handle for smdl channel */
  smdl_handle_type   handle;

  /* Id */
  uint16             id;
  
  /* Indicates if this smdl handle is valid */
  boolean            valid;

  /* Indicates if this smdl handle is ready for transfer */
  boolean            transfer_ready;

  /* Originator proc for this smdl handle */
  ssm_proc_enum_t    src_proc;

  /* Destionation proc for this smdl handle */
  ssm_proc_enum_t    dest_proc;

} ssm_smdl_handle_t;

static ssm_smdl_handle_t * ssm_smdl_handle_struct;

#ifdef FEATURE_SSM_REX_API_PRESENT
/* Look up pointer for ssm tcb */
static rex_tcb_type* ssm_tcb_ptr=NULL;
#else /* FEATURE_SSM_REX_API_PRESENT */
extern qurt_anysignal_t  ssm_sig_qurt;
#endif /* FEATURE_SSM_REX_API_PRESENT */

typedef struct 
{
  /* Link for the next queue */
  q_link_type                            link;

  /* Id  */
  uint32                                 id;

} ssm_queue_node_struct_t;

/* Queue to store client id to be processed */
static q_type                            ssm_smdl_id_q;



/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/

/** 
Get SMDL Port Connection Status

@param[in] proc                    Proc whose connection status needs to be determined

@return
TRUE or FALSE

@dependencies
None.

*/

boolean ssm_is_smdl_ready
( 
  ssm_proc_enum_t proc 
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( ssm_smdl_handle_struct == NULL )
    return FALSE;
  
  return ssm_smdl_handle_struct[proc].transfer_ready;

} /* ssm_is_smdl_ready */

/** 
Handle SMDL read information

@param[in] proc                    Proc from which buffer should be read

@return
ssm_err_t -- E_SSM_SUCCESS if success, E_SSM_FAIL if failure

@dependencies
None.

*/

ssm_err_t ssm_smdl_handle_rx(void)
{
  /* Read buffer*/
  char read_buffer[SSM_SMDL_RX_LENGTH];
  
  int32 read_length;
  
  ssm_proc_enum_t proc;

  ssm_queue_node_struct_t * temp_node;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
	
  SSM_MSG_MED (" Function ssm_smdl_handle_rx " );

  if ( ssm_smdl_handle_struct == NULL )
    return E_SSM_FAIL;

  /* Get proc information from the queue */
  temp_node = q_get( &ssm_smdl_id_q );
  while ( temp_node != NULL )
  {  
    proc = temp_node->id;

    /* Free up space allocted by queue node */
    free ( temp_node );

    if ( proc >= SSM_PROC_MODEM && proc < SSM_PROC_MAX )
    {
      /* Check if SMDL is initialized */
      if ( ssm_smdl_handle_struct[proc].transfer_ready == TRUE )
      {    
  
        SSM_MSG_LOW_1 (" Function ssm_smdl_handle_rx from PROC %d", proc );
  
        /* Try reading from smdl port */
        if ( ( read_length = smdl_read( ssm_smdl_handle_struct[proc].handle, 
                       SSM_SMDL_RX_LENGTH, 
                       read_buffer, SMDL_READ_FLAGS_NONE )) > 0 )
        {
          SSM_MSG_HIGH_1 (" Length read from SMDL %d ", read_length);
     
          /* Process read buffer */
          ssm_ipc_decode_ipc_message( proc, read_buffer, read_length );
        
        }
      }
    }

    temp_node = q_get( &ssm_smdl_id_q );
  }
  

  return E_SSM_SUCCESS;

} /* ssm_smdl_handle_rx */


/** 
Transfer message through smdl

@param[in] proc                    Proc to  which buffer should be transferred
@param[in] write_buffer            Buffer to be written
@param[in] length                  length of the buffer

@return
ssm_err_t -- E_SSM_SUCCESS if success, E_SSM_FAIL if failure

@dependencies
None.

*/

ssm_err_t ssm_smdl_handle_tx
( 
  ssm_proc_enum_t proc,
  const void * write_buffer, 
  uint32 length
)
{
  SSM_MSG_MED_2 (" Function ssm_smdl write to Proc %d of length %d ", proc, length);

  if ( ssm_smdl_handle_struct == NULL )
    return E_SSM_FAIL;

  if ( length >= SSM_SMDL_RX_LENGTH )
    return E_SSM_FAIL;
  
  /* Check if SMDL is initialized */
  if ( ssm_smdl_handle_struct[proc].transfer_ready == FALSE )
     return E_SSM_FAIL;
  
  /* Write into smdl port */
  if ( 0 < smdl_write( ssm_smdl_handle_struct[proc].handle, length, write_buffer, 
		       SMDL_WRITE_FLAGS_NONE ) )
  {
    return E_SSM_SUCCESS;
  }

  SSM_MSG_ERROR (" smdl write failed " );
  
  return E_SSM_FAIL;

} /* ssm_smdl_handle_tx */


/** 
SMDL event handler

@param[in] handle                  Handle type
@param[in] event                   Type of event ot be handled
@param[in] cb_data                 callback data


@return
None

@dependencies
None.

*/

static void ssm_smdl_event_notify
(
  smdl_handle_type handle,
  smdl_event_type  event,
  void             *cb_data
)
{
  uint16 id =0;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
   
  id = *((uint16 * )cb_data);

  /* Analyze Event */
  switch(event)
  {
    case SMDL_EVENT_OPEN:
      ssm_smdl_handle_struct[id].transfer_ready = TRUE;
  
      #ifndef FEATURE_SSM_PERIPHERAL_SERVER_PROC
        qurt_anysignal_set( &ssm_sig_qurt, SSM_PARSE_PERM_FILE_SIG );
      #endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */
	    
      SSM_MSG_LOW (" smdl port open event " );
      
      break;
    
    case SMDL_EVENT_READ:
      SSM_MSG_LOW (" smdl port read event " );
      if ( ssm_smdl_handle_struct[id].transfer_ready == TRUE )
      {
        ssm_queue_node_struct_t * temp_node = malloc( sizeof ( ssm_queue_node_struct_t ) );
	
	if ( temp_node == NULL )
        {
	  return;
	}

	temp_node->id = id;
          
	/* Link temp_node to ssm_smdl_id_q */
        (void) q_link( temp_node, &temp_node->link );
    
        /* Enqueue node in ssm_smdl_id_q */
        q_put( &ssm_smdl_id_q, &temp_node->link);

        SSM_MSG_LOW (" smdl port read event " );
	#ifdef FEATURE_SSM_REX_API_PRESENT
	rex_set_sigs(ssm_tcb_ptr, SSM_SMDL_READ_SIG );
        #else /* FEATURE_SSM_REX_API_PRESENT */
          qurt_anysignal_set( &ssm_sig_qurt, SSM_SMDL_READ_SIG );
        #endif /* FEATURE_SSM_REX_API_PRESENT */
      }
      break;

    case SMDL_EVENT_WRITE:
      break;

    case SMDL_EVENT_CLOSE_COMPLETE:
    case SMDL_EVENT_REMOTE_CLOSE:

      SSM_MSG_ERROR (" smdl port close event " );

      ssm_smdl_handle_struct[id].transfer_ready = FALSE;
      break;
    
    default:
      break;
  }
} /* ssm_smdl_event_notify */

/** 
Initialize smdl port

@return
ssm_err_t  -- E_SSM_SUCCESS if success, E_SSM_FAIL if failure

@dependencies
None.

*/

ssm_err_t ssm_smdl_init(void)
{
  
  ssm_proc_enum_t iter =0;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
	
  /* Initialize task information */
  #ifdef FEATURE_SSM_REX_API_PRESENT
  ssm_tcb_ptr = rcinit_lookup_rextask("ssm");
   
  if ( ssm_tcb_ptr == NULL )
  {
    SSM_ERR_FATAL(" SSM task tcb not found", 0 ,0, 0 );
  }
   
  #endif /* FEATURE_SSM_REX_API_PRESENT */

  ssm_smdl_handle_struct = malloc ( sizeof(ssm_smdl_handle_t) * SSM_PROC_MAX );

  if ( ssm_smdl_handle_struct == NULL )
    return E_SSM_FAIL;

  /* Initialize queue */
  q_init( &ssm_smdl_id_q );

  /* Initialize SSM SMDL structure */
  for ( iter = SSM_PROC_MODEM; iter < SSM_PROC_MAX; iter++ )
  {
    ssm_smdl_handle_struct[iter].id = (uint16)iter;
    ssm_smdl_handle_struct[iter].src_proc = SSM_PROC_CURRENT;
    ssm_smdl_handle_struct[iter].dest_proc = iter;

    if ( ssm_smdl_handle_struct[iter].src_proc == 
	  ssm_smdl_handle_struct[iter].dest_proc )
    {
       ssm_smdl_handle_struct[iter].valid = FALSE;  
    }
    else
    {
        ssm_smdl_handle_struct[iter].valid = TRUE;
    }
    
    ssm_smdl_handle_struct[iter].transfer_ready = FALSE;
  }

  #ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC
  /* Request SMDL to open a port */
  ssm_smdl_handle_struct[SSM_PROC_APPS].handle = smdl_open( SSM_NAME_MODEM_APPS,
                                         SSM_SMDL_EDGE_MODEM_APPS,
			      SSM_SMDL_DEFAULT_MODE,
		              SSM_SMDL_DEFAULT_FIFO,
			      ssm_smdl_event_notify,
			                 (void * ) &(ssm_smdl_handle_struct[SSM_PROC_APPS].id)
			    );

  ssm_smdl_handle_struct[SSM_PROC_ADSP].handle = smdl_open( SSM_NAME_MODEM_ADSP,
                                         SSM_SMDL_EDGE_MODEM_ADSP,
			                 SSM_SMDL_DEFAULT_MODE,
		                         SSM_SMDL_DEFAULT_FIFO,
			                 ssm_smdl_event_notify,
			                 (void * ) &(ssm_smdl_handle_struct[SSM_PROC_ADSP].id)
			               );

  #elif FEATURE_SSM_ADSP_PROC
  ssm_smdl_handle_struct[SSM_PROC_MODEM].handle = smdl_open( SSM_NAME_MODEM_ADSP,
                                         SSM_SMDL_EDGE_MODEM_ADSP,
			                 SSM_SMDL_DEFAULT_MODE,
		                         SSM_SMDL_DEFAULT_FIFO,
			                 ssm_smdl_event_notify,
			                 (void * ) &(ssm_smdl_handle_struct[SSM_PROC_MODEM].id)
			               );

  #elif FEATURE_SSM_RIVA_PROC
  ssm_smdl_handle_struct[SSM_PROC_MODEM].handle = smdl_open( SSM_NAME_MODEM_RIVA,
                                         SSM_SMDL_EDGE_MODEM_RIVA,
			                 SSM_SMDL_DEFAULT_MODE,
		                         SSM_SMDL_DEFAULT_FIFO,
			                 ssm_smdl_event_notify,
			                 (void * ) &(ssm_smdl_handle_struct[SSM_PROC_MODEM].id)
			               );

  #endif 
  



  return E_SSM_SUCCESS;

} /* ssm_smdl_init */

