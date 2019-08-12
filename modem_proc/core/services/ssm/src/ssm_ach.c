/*=============================================================================

             S E C U R E  S E R V I C E S   M O D U L E

GENERAL DESCRIPTION
  Implements SSM Override Command Handler Functionality

Copyright (c) 2012 - 2014 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/ssm/src/ssm_ach.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/02/14   psu     Update diag macros using optimized versions.
04/02/13   din     Replaced memcpy with memscpy.
11/05/12   din     File created.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "ssm_ach_v.h"
#include "ssm_task_v.h"
#include "fs_public.h"
#include "sfs_api.h"
#include "queue.h"
#include "ssm_v.h"
#include <stringl/stringl.h>


/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/

#define SSM_ACH_FILE_ROOT_DIRECTORY               "/ssm"
#define SSM_ACH_FILE_SUB_DIRECTORY                "/ssm/ach"
#define SSM_ACH_AUTHENTICATION_COOKIE_FILE        "/ssm/ach/authen.cookie"

/* Due to absence of stat functionality to verify if the file is not present, it is 
 * important to read some content from file to validate if the file is present */
#define SSM_ACH_AUTHENTICATION_COOKIE_CONTENT     "Authentication Cookie"
#define SSM_ACH_AUTHENTICATION_COOKIE_CONTENT_LEN 21


/* Look up pointer for ssm tcb */
static rex_tcb_type* ssm_tcb_ptr=NULL;

/* Static data variables indicating state of cookie */
/* Static data variable indicating atuhentication is present in sfs
 * Default value should be TRUE just to make sure DCI is not ON before 
 * initializing with the function ssm_ach_delayed_init. The function 
 * ssm_ach_delayed_init should assign correct value to the variable.
 * ssm_ach_delayed_init is called indirectly from RCINIT group 2 or 
 * in registration context when a client registers with ssm och  */
static boolean ssm_ach_authentication_cookie_present    = TRUE;

/*-------------------------Callback Registration----------------------------- */

#define SSM_ACH_MAX_CB_REGISTRATION  5

static uint16 ssm_ach_cb_registration_count ;

static ssm_ach_register_cb_type  ssm_ach_cb_register[SSM_ACH_MAX_CB_REGISTRATION];


/*----------------------------Command Queue--------------------------------- */

/* Queue to store commands to be processed through a task */
static q_type                            ssm_ach_cmd_q;

typedef struct 
{
  /* Link for the next queue */
  q_link_type                            link;

  /* SSM key handler structure */
  ssm_ach_cmd_struct_t                   cmd_req;

} ssm_ach_queue_node_struct;


/* -------------------------------Mutex------------------------------------- */
/* Pointer to the mutex object for protecting critical-section of ssm ach code*/
static qurt_mutex_t ssm_ach_mutex;
static qurt_mutex_t *pssm_ach_mutex = NULL;


/* MACRO for locking/unlocking the qmutex*/
#define SSM_ACH_INTLOCK()          \
  if ( NULL == pssm_ach_mutex )              \
  {                                      \
    qurt_pimutex_init(&ssm_ach_mutex);       \
    pssm_ach_mutex = &ssm_ach_mutex;             \
  }                                      \
  qurt_pimutex_lock(&ssm_ach_mutex); 
  
#define SSM_ACH_INTFREE()          \
  qurt_pimutex_unlock(&ssm_ach_mutex); 


/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/

#ifdef FEATURE_SSM_TEST_DEBUG

extern ssm_ach_err_t ssm_ach_remove_authentication_cookie
(
   ssm_ach_cmd_struct_t   * cmd_req
);

#endif



/**
Gets Authentication Cookie status 

@param[in]  
None

@return 
SSM_ACH_AUTHENTICATION_COOKIE_PRESENT     -  Authentication Cookie Present 
SSM_ACH_AUTHENTICATION_COOKIE_NOT_PRESENT -  Authentication Cookie Not Present

@dependencies
None

*/

ssm_ach_authentication_coookie_status_t ssm_ach_get_authentication_cookie_status(void)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( ssm_ach_authentication_cookie_present )
    return SSM_ACH_AUTHENTICATION_COOKIE_PRESENT;

  return SSM_ACH_AUTHENTICATION_COOKIE_NOT_PRESENT;

} /* ssm_ach_get_authentication_cookie_status */




/**
Sets Authentication Cookie status 

@param[in]  
None

@return 
SSM_ACH_AUTHENTICATION_COOKIE_PRESENT     -  Authentication Cookie Present 
SSM_ACH_AUTHENTICATION_COOKIE_NOT_PRESENT -  Authentication Cookie Not Present

@dependencies
None

*/

void ssm_ach_set_authentication_cookie_status
(
  boolean status
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ssm_ach_authentication_cookie_present = status;

} /* ssm_ach_set_authentication_cookie_status */




/**
Updates Status of ACH through Callback

@param[in]  
status   -   Status of the ACH 

@return 
None

@dependencies
None

*/

void ssm_ach_update_status_thru_cb
(
  ssm_ach_authentication_coookie_status_t status
)
{
  uint16 iter;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  SSM_ACH_INTLOCK()

  /* Iter through callabck table to find duplicate */
  for ( iter =0; iter < ssm_ach_cb_registration_count; iter ++ )
  {
    if ( ssm_ach_cb_register[iter] != NULL )
    {
       ssm_ach_cb_register[iter]( status );
    }
  }
  SSM_ACH_INTFREE()

} /* ssm_ach_update_status_thru_cb */



/**
Initializes SSM authentication cookie global variables

@param[in]  
None

@return 
None 

@dependencies
None

*/

void ssm_ach_init(void)
{
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
 
  if ( NULL == pssm_ach_mutex )              
  {                                      
    qurt_pimutex_init(&ssm_ach_mutex);       
    pssm_ach_mutex = &ssm_ach_mutex;             
  }
  
  ssm_tcb_ptr = rcinit_lookup_rextask("ssm");

  if ( ssm_tcb_ptr == NULL )
  {
    SSM_ERR_FATAL(" SSM task tcb not found", 0 ,0, 0 );
  }

  /* Initialize Queue */
  q_init( &ssm_ach_cmd_q );
  
} /* ssm_ach_init */


/**
Initializes SSM authentication cookie .
The Initialization sequence is delayed since it needs to be performed 
only once sfs initialies or is initialized from RCINIT Group 2

@param[in]  
None

@return 
None 

@dependencies
None

*/

void ssm_ach_delayed_init(void)
{
  
  int fp;
  static boolean ssm_ach_delayed_init = FALSE;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
 
  if ( ssm_ach_delayed_init == TRUE )
    return;

  /* Check if Authentication Cookie is present */
  /* Get file handle for SSM key handler file and check if it is valid */

  #ifndef FEATURE_SSM_USE_EFS_TEST
  fp = sfs_open( SSM_ACH_AUTHENTICATION_COOKIE_FILE, O_RDONLY );
  if ( fp == 0 )
  {
  #else
  fp = efs_open( SSM_ACH_AUTHENTICATION_COOKIE_FILE, O_RDONLY  );
  if ( fp < 0 )
  {
  #endif
    ssm_ach_set_authentication_cookie_status( FALSE ) ;

    SSM_MSG_MED_1( " Authentication Cookie File Not Present %d ", efs_errno); 

  }
  else
  {
    ssm_ach_set_authentication_cookie_status( TRUE ); 
    ssm_ach_delayed_init = TRUE; 

    #ifndef FEATURE_SSM_USE_EFS_TEST
    sfs_close( fp );
    #else
    efs_close( fp );
    #endif

  }

  /* Update Override status through callback */
  ssm_ach_update_status_thru_cb ( ssm_ach_get_authentication_cookie_status() );

  
} /* ssm_ach_delayed_init */



/**
Registers for updates for ACH through callback 

@param[in]  
None

@return 
E_SSM_ACH_SUCCESS -  Registration Successful
E_SSM_ACH_FAILED  -  Registration Failed

@dependencies
None

*/

ssm_ach_err_t ssm_ach_register_cb
(
  ssm_ach_register_cb_type cb
)
{
  uint16 iter;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* If callback is NULL, discard the value */
  if ( cb == NULL )
    return E_SSM_ACH_FAILED;
  
  SSM_ACH_INTLOCK()
  
  /* Iter through callabck table to find duplicate */
  for ( iter =0; iter < ssm_ach_cb_registration_count; iter ++ )
  {
    if ( ssm_ach_cb_register[iter] == cb )
    {
       SSM_ACH_INTFREE()
       return E_SSM_ACH_SUCCESS;
    }
  }
  if ( ssm_ach_cb_registration_count < SSM_ACH_MAX_CB_REGISTRATION )
  {
    /* Save callback */
    ssm_ach_cb_register[ssm_ach_cb_registration_count++]=cb;
    
    SSM_ACH_INTFREE()

    return E_SSM_ACH_SUCCESS;
  }
  
  SSM_ACH_INTFREE()
  return E_SSM_ACH_FAILED;

} /* ssm_ach_register_cb */



/**
Enqueues ACH Command request to the queue

@param[in]  
cmd  -  Structure allocated by the client for ACH commands
        Should be deallocated after the call to the function.
        Only Deallocate the cmd structure and not the memory pointed by 
	uint8 * signature within cmd.

@return 
ssm_ach_err_t  - E_SSM_ACH_FAILED        Operation failed
                 E_SSM_ACH_IN_PROGRESS   Operation in Progress 

@dependencies
None

*/

ssm_ach_err_t ssm_ach_request
(
  /** Callback to inform about command status */
  ssm_ach_cmd_struct_t      * cmd
)
{
  
  ssm_ach_queue_node_struct  * temp;

  uint32 size ;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( cmd == NULL )
  {
    return E_SSM_ACH_FAILED;
  }

  /* Allocate space for request to be queued up */
  temp = ( ssm_ach_queue_node_struct  *)malloc( 
		                    sizeof(ssm_ach_queue_node_struct));

  if ( temp == NULL )
  {
    return E_SSM_ACH_DYNAMIC_ALLOCATION_FAILED;
  }

  /* Copy the data from cmd_req to temp->cmd_req */
  size = memscpy( (void *) &( temp->cmd_req), sizeof( ssm_ach_cmd_struct_t ),
	             (void *) cmd, sizeof( ssm_ach_cmd_struct_t ));

  if ( size != sizeof( ssm_ach_cmd_struct_t ) )
  {
    free ( temp );
    return E_SSM_ACH_FAILED;
  }

  /* Link temp to ssm_ach_req_q */
  (void) q_link( temp, &temp->link );
    
  /* Enqueue node in ssm_ach_cmd_q */
  q_put( &ssm_ach_cmd_q, &temp->link);

  /* Signal ssm_task to handle the request */
  rex_set_sigs(ssm_tcb_ptr, SSM_ACH_PROCESS_SIG );

  /* Return status of the requests */
  return E_SSM_ACH_IN_PROGRESS; 

} /* ssm_ach_request */



/**
Adds Authenctication Cookie.. Creates a SFS file.
Due to absence of stat functionality to verify if the file is not present, it is 
important to read/write some content from file to validate if the file is present.

@param[in]  
cmd_req   -  Structure allocated by the task in which key handler is processed and 
             deallocated after the call to the function.

@return 
None

@dependencies
None

*/

static ssm_ach_err_t ssm_ach_add_authentication_cookie
(
   ssm_ach_cmd_struct_t   * cmd_req
)
{
  int fp;
  int result;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check to avoid addition of authetication cookie if it is already done */
  if ( ssm_ach_get_authentication_cookie_status() == 
       SSM_ACH_AUTHENTICATION_COOKIE_PRESENT 
       && ssm_ach_cb_registration_count != 0 )
  {
    return E_SSM_ACH_AUTHENTICAION_COOKIE_ALREADY_PRESENT;  
  }
     
  /* Create SSM directory in the filesystem */
  /* efs_mkdir returns 0 if the directory is not present in the file system and 
     it can create efs directory successfully. It returns -1 if it fails or 
     if the directory is present in the filesystem */
  
  result = efs_mkdir (SSM_ACH_FILE_ROOT_DIRECTORY, 0755 );
  if ( result != 0 )
  {
    SSM_MSG_HIGH ("SSM : EFS Directory creation failed ");
  }

  #ifndef FEATURE_SSM_USE_EFS_TEST
  /* Create a sub-directory within SSM directory in the filesystem */
  result = sfs_mkdir ( SSM_ACH_FILE_SUB_DIRECTORY );
  #else
  result = efs_mkdir( SSM_ACH_FILE_SUB_DIRECTORY , 0755);
  #endif
  if ( result != 0 )
  {
    SSM_MSG_HIGH_1("SSM : SFS Directory creation failed %d ", efs_errno);
  }
  
  /* Get file handle for SSM key handler file and check if it is valid */
  #ifndef FEATURE_SSM_USE_EFS_TEST
  fp = sfs_open( SSM_ACH_AUTHENTICATION_COOKIE_FILE, 
		 O_CREAT | O_TRUNC | O_WRONLY );
  if ( fp == 0 )
  {
  #else
  fp = efs_open( SSM_ACH_AUTHENTICATION_COOKIE_FILE, 
		 O_CREAT | O_TRUNC | O_WRONLY );
  if ( fp < 0 )
  {
  #endif
    /* Early return */
    SSM_MSG_ERROR_1( " Authentication Cookie File open failed %d ", efs_errno);
     
    return E_SSM_ACH_FILE_OPEN_FAILED;
  }

  /* Write to the file */
  #ifndef FEATURE_SSM_USE_EFS_TEST
  result = sfs_write( fp, (char *)SSM_ACH_AUTHENTICATION_COOKIE_CONTENT, 
		      SSM_ACH_AUTHENTICATION_COOKIE_CONTENT_LEN );
  #else
  result = efs_write( fp, SSM_ACH_AUTHENTICATION_COOKIE_CONTENT, 
		      SSM_ACH_AUTHENTICATION_COOKIE_CONTENT_LEN );
  #endif

  if ( result != SSM_ACH_AUTHENTICATION_COOKIE_CONTENT_LEN )
  {
    /* Early return */
    SSM_MSG_ERROR_1( " Cannot write to Authentication Cookie File %d ", efs_errno);
     
    #ifndef FEATURE_SSM_USE_EFS_TEST
    sfs_close( fp );
    #else
    efs_close( fp );
    #endif
    
    return E_SSM_ACH_FILE_WRITE_FAILED;
  }

  #ifndef FEATURE_SSM_USE_EFS_TEST
  sfs_close( fp );
  #else
  efs_close( fp );
  #endif

  /* Set Authentication cokkie present as TRUE */
  ssm_ach_set_authentication_cookie_status( TRUE ); 

  /* Update Override status through callback */
  ssm_ach_update_status_thru_cb ( ssm_ach_get_authentication_cookie_status() );

  return E_SSM_ACH_SUCCESS;

} /* ssm_ach_add_authentication_cookie */



/**
Processes request queued to ACH in a serialized manner through a task.
To be called from SSM task.

@param[in]  
None

@return 
none

@dependencies
None

*/

void ssm_ach_process_request(void)
{
  /* To be fetched from the ACH command queue */
	
  ssm_ach_queue_node_struct * temp;

  ssm_ach_cmd_struct_t      * cmd_req;

  ssm_ach_err_t               err;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Fetch Node from Key Handler request queue */
  temp = (ssm_ach_queue_node_struct *)q_get( &ssm_ach_cmd_q );

  while( temp!= NULL )
  {

    cmd_req = &(temp->cmd_req);

    if ( cmd_req  == NULL )
    {
      /** Clear up allocated space */
      free(temp);
    
      /* Fetch Node from Command Handler request queue */
      temp = (ssm_ach_queue_node_struct *)q_get( &ssm_ach_cmd_q );

      continue;
    }

    switch(cmd_req->cmd)
    {
      case SSM_ACH_ADD_AUTHENTICATION_COOKIE:
      {
        err = ssm_ach_add_authentication_cookie(cmd_req);
        if ( cmd_req->cb )
        {
          cmd_req->cb(err, cmd_req->user_data );
        }
        break;
      }
      #ifdef FEATURE_SSM_TEST_DEBUG
      case SSM_ACH_REMOVE_AUTHENTICATION_COOKIE:
      {
          
        err = ssm_ach_remove_authentication_cookie(cmd_req);
        if ( cmd_req->cb )
        {
          cmd_req->cb(err, cmd_req->user_data );
        }
        break;
      }
      #endif /* FEATURE_SSM_TEST_DEBUG */ 	
      default:
      break;
    }
    
    /** Clear up allocated space */
    free( temp );
    
    /* Fetch Node from Key Handler request queue */
    temp = (ssm_ach_queue_node_struct *)q_get( &ssm_ach_cmd_q );
  }
  
} /* ssm_ach_process_request */



