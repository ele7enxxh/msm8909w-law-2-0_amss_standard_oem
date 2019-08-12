/*=============================================================================

             S E C U R E  S E R V I C E S   M O D U L E

GENERAL DESCRIPTION
  Implements SSM Key Handler Functionality

Copyright (c) 2012 - 2014 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/ssm/src/ssm_key_handler.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/02/14   psu     Update diag macros using optimized versions.
04/02/13   din     Replaced memcpy with memscpy.
11/05/12   din     File created.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "ssm_key_handler_v.h"
#include "ssm_task_v.h"
#include "fs_public.h"
#include "sfs_api.h"
#include "queue.h"
#include "ssm_v.h"
#include "ssm_ach.h"
#include <stringl/stringl.h>


/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/

#define SSM_KEY_HANDLER_FILE_ROOT_DIRECTORY     "/ssm"
#define SSM_KEY_HANDLER_FILE_SUB_DIRECTORY      "/ssm/key_handler/"


/* Look up pointer for ssm tcb */
static rex_tcb_type* ssm_tcb_ptr=NULL;

/* Queue to store request to key handler */
static q_type                            ssm_key_handler_req_q;


typedef struct 
{
  /* Link for the next queue */
  q_link_type                            link;

  /* SSM key handler structure */
  ssm_key_handler_struct_t               key_req;

} ssm_key_handler_queue_node_struct;


/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/

#ifdef FEATURE_SSM_TEST_DEBUG

extern ssm_key_handler_err_t ssm_key_handler_remove_key
(
   ssm_key_handler_struct_t   * key_req
);

#endif /* FEATURE_SSM_TEST_DEBUG */


/**
Initializes key handler and its global variable

@param[in]  
None

@return 
None 

@dependencies
None

*/

void ssm_key_handler_init(void)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ssm_tcb_ptr = rcinit_lookup_rextask("ssm");

  if ( ssm_tcb_ptr == NULL )
  {
    SSM_ERR_FATAL(" SSM task tcb not found", 0 ,0, 0 );
  }

  q_init(&ssm_key_handler_req_q);

} /* ssm_key_handler_init */




/**
Enqueues Key Handler request to the queue

@param[in]  
*key_req   -  Structure allocated by the client pertaining to key based on key request.
             Should be deallocated after the call to the function.

@return 
ssm_key_handler_err_t  - E_SSM_KEY_HANDLER_FAILED        Operation failed
                         E_SSM_KEY_HANDLER_IN_PROGRESS   Operation in Progress 

@dependencies
None

*/

ssm_key_handler_err_t ssm_key_handler_request
(
   ssm_key_handler_struct_t   * key_req
)
{
  /* To be queued to the key handler */
	
  ssm_key_handler_queue_node_struct * temp;

  ssm_key_handler_struct_t        * key_req_temp;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( key_req == NULL )
  {
    return E_SSM_KEY_HANDLER_FAILED;
  }

  /* Allocate space for request to be queued up */
  temp = (ssm_key_handler_queue_node_struct *)malloc( 
		                   sizeof(ssm_key_handler_queue_node_struct) );

  /* Check if temp is valid */
  if ( temp == NULL )
  {
    /* Early return */
    return E_SSM_KEY_HANDLER_DYNAMIC_ALLOCATION_FAILED;
  }

  key_req_temp = &(temp->key_req);

  /* Copy the data from key_req to temp->key_req */
  if ( sizeof ( ssm_key_handler_struct_t ) != memscpy( (void *) key_req_temp, 
			                    sizeof ( ssm_key_handler_struct_t ),
	                   (void *) key_req, sizeof( ssm_key_handler_struct_t ) ))
  {
    free ( temp );

    /* Early return */
    return E_SSM_KEY_HANDLER_FAILED;
  }
  
  if (  key_req->key ) 
  {
    /* Check if key is valid */
    if ( key_req->info.len_key == 0 )
    {
      free ( temp );

      /* Early return */
      return E_SSM_KEY_HANDLER_INVALID_INPUT;
    }

    key_req_temp->key = (uint8 *) malloc ( key_req->info.len_key );

    /* Check if key_req_temp->key is valid */
    if ( key_req_temp->key == NULL )
    {
      free( temp );
      
      /* Early return */
      return E_SSM_KEY_HANDLER_DYNAMIC_ALLOCATION_FAILED;
    }
    if ( key_req->info.len_key != memscpy( (void *) key_req_temp->key, key_req->info.len_key, 
	                    (void *) key_req->key, key_req->info.len_key ) )
    {
      free ( key_req_temp->key );
      free ( temp );

      /* Early return */
      return E_SSM_KEY_HANDLER_FAILED;
    }
  }

  /* Link temp to ssm_key_handler_req_q */
  (void) q_link( temp, &temp->link );
    
  /* Enqueue node in ssm_key_handler_req_q */
  q_put( &ssm_key_handler_req_q, &temp->link);

  /* Signal ssm_task to handle the request */
  rex_set_sigs( ssm_tcb_ptr, SSM_KEY_HANDLER_PROCESS_SIG );

  /* Return status of the requests */
  return E_SSM_KEY_HANDLER_IN_PROGRESS; 

} /* ssm_key_handler_request */



/**
Get keys from the secure key handler. The function is meant for synchronous call

@param[in]  
key_req   -  Structure allocated by the caller and deallocated after the call 
             to the function. key ( part of key_req ) should be deallocated 
	     by the user. Its a blocking call and uses sfs operation and 
	     hence mutex shouldn't be used around the call.
	     

@return 
None

@dependencies
None

*/

ssm_key_handler_err_t ssm_key_handler_get_key
(
   ssm_key_handler_struct_t   * key_req
)
{
  int fp;
  int result;

  char filename[120];

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Validate input  */
  if ( key_req == NULL
       || key_req->info.usage >= SSM_KEY_HANDLER_USAGE_MAX
       || key_req->info.source >= SSM_KEY_HANDLER_SRC_MAX
       || key_req->info.crypto_algo >= SSM_KEY_HANDLER_CRYPTO_ALGO_MAX ) 
  {
    /* Early return */
    SSM_MSG_ERROR ( " Key handler Invalid Argument " );
     
    return E_SSM_KEY_HANDLER_INVALID_INPUT;
  }
  
  result = snprintf( filename, sizeof(filename), 
            SSM_KEY_HANDLER_FILE_SUB_DIRECTORY"kh_%d_%d_%d.key" , 
                           key_req->info.usage,
			   key_req->info.crypto_algo,
			   key_req->info.source );

  if ( result <= 0)
  {
    /* Early return */
    SSM_MSG_ERROR ( " File name creation failed " );
     
    return E_SSM_KEY_HANDLER_FAILED;
  }
  
  
  /* Get file handle for SSM key handler file and check if it is valid */
  #ifndef FEATURE_SSM_USE_EFS_TEST
  fp = sfs_open( filename , O_RDONLY );
  if ( fp == 0 )
  #else
  fp = efs_open( filename, O_RDONLY  );
  if ( fp < 0 )
  #endif
  {
    /* Early return */
    SSM_MSG_ERROR_1 ( " Key Handler File open failed %d ", efs_errno );
     
    key_req->key = NULL;
     
    return E_SSM_KEY_HANDLER_KEY_NOT_FOUND ;
  }

  /* Read key info from the file */
  #ifndef FEATURE_SSM_USE_EFS_TEST
  result = sfs_read( fp, (char *) &key_req->info, sizeof( ssm_key_handler_info_t) );
  #else
  result = efs_read( fp, &key_req->info, sizeof( ssm_key_handler_info_t) );
  #endif

  if ( result != sizeof( ssm_key_handler_info_t) || result ==0 || result == -1 )
  {
    /* Early return */
    SSM_MSG_ERROR_1 ( " Cannot read from Key handler File %d ", efs_errno );
    
    key_req->key = NULL;

    #ifndef FEATURE_SSM_USE_EFS_TEST
    sfs_close( fp );
    #else
    efs_close( fp );
    #endif
    
    return E_SSM_KEY_HANDLER_FILE_READ_FAILED ;
  }

  /* Check if key is valid */
  if ( key_req->info.len_key == 0 )
  {
    #ifndef FEATURE_SSM_USE_EFS_TEST
    sfs_close( fp );
    #else
    efs_close( fp );
    #endif

    /* Early return */
    return E_SSM_KEY_HANDLER_FAILED;
  }
    
  /* This allocated memory will be freed by key handler */
  key_req->key = ( uint8 * )malloc(key_req->info.len_key);

  if ( key_req->key == NULL )
  {
    #ifndef FEATURE_SSM_USE_EFS_TEST
    sfs_close( fp );
    #else
    efs_close( fp );
    #endif

    return E_SSM_KEY_HANDLER_DYNAMIC_ALLOCATION_FAILED;
  }

  /* Read key from the file */
  #ifndef FEATURE_SSM_USE_EFS_TEST
  result = sfs_read( fp, (char *)key_req->key, key_req->info.len_key );
  #else
  result = efs_read( fp, key_req->key, key_req->info.len_key );
  #endif

  if ( result != key_req->info.len_key || result == 0 || result == -1 )
  {
    /* Early return */
    SSM_MSG_ERROR_1 ( " Cannot read from Key handler File %d ", efs_errno );
   
    #ifndef FEATURE_SSM_USE_EFS_TEST
    sfs_close( fp );
    #else
    efs_close( fp );
    #endif

    free ( key_req->key);
    key_req->key = NULL;

    return E_SSM_KEY_HANDLER_FILE_READ_FAILED;

  }

  #ifndef FEATURE_SSM_USE_EFS_TEST
  sfs_close( fp );
  #else
  efs_close( fp );
  #endif

  SSM_MSG_LOW ( " Key handler Get Key Succeeded  " );

  return E_SSM_KEY_HANDLER_SUCCESS;

} /* ssm_key_handler_get_key */



/**
Adds key to the secure file

@param[in]  
key_req   -  Structure allocated by the task in which key handler is processed and 
             deallocated after the call to the function.

@return 
None

@dependencies
None

*/

static ssm_key_handler_err_t ssm_key_handler_add_key
(
   ssm_key_handler_struct_t   * key_req
)
{
  int fp;
  int result;

  char filename[120];

  ssm_key_handler_struct_t  get_key_req;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( ssm_ach_get_authentication_cookie_status() == 
		               SSM_ACH_AUTHENTICATION_COOKIE_PRESENT )
  {
    return E_SSM_KEY_HANDLER_NOT_ALLOWED;
  }

  /* Validate input .. key_req cannot be NULL */
  if ( key_req->info.usage >= SSM_KEY_HANDLER_USAGE_MAX
       || key_req->info.source >= SSM_KEY_HANDLER_SRC_MAX
       || key_req->info.crypto_algo >= SSM_KEY_HANDLER_CRYPTO_ALGO_MAX 
       || key_req->key == NULL ) 
  {
    /* Early return */
    SSM_MSG_ERROR ( " Key handler Invalid Argument " );
     
    return E_SSM_KEY_HANDLER_INVALID_INPUT;
  }

  /* NOTE: get_key_req.key will be allocated by the get key operation if it finds 
   * a key */
  memscpy( &get_key_req, sizeof ( ssm_key_handler_struct_t ), 
		  key_req , sizeof ( ssm_key_handler_struct_t ));

  /* Check if key is already present, if so avoid writing to it */
  if ( ssm_key_handler_get_key( &get_key_req ) == E_SSM_KEY_HANDLER_SUCCESS )
  {
    /** Clear up allocated space */
    if ( get_key_req.key )
    {
      /* Zero out the key before freeing it up */
      secure_memset ( get_key_req.key, 0 , get_key_req.info.len_key );

      free( get_key_req.key );
    }
    
    return E_SSM_KEY_HANDLER_KEY_ALREADY_PRESENT;
  }
    
  /* Create SSM directory in the filesystem */
  /* efs_mkdir returns 0 if the directory is not present in the file system.
     It returns -1 if it fails or 
     if the directory is present in the filesystem */
  
  result = efs_mkdir (SSM_KEY_HANDLER_FILE_ROOT_DIRECTORY, 0755 );
  if ( result != 0 )
  {
    SSM_MSG_HIGH_1("SSM : SFS Directory creation failed %d ", efs_errno );  
  }

  #ifndef FEATURE_SSM_USE_EFS_TEST
  /* Create a sub-directory within SSM directory in the filesystem */
  result = sfs_mkdir ( SSM_KEY_HANDLER_FILE_SUB_DIRECTORY );
  #else
  result = efs_mkdir( SSM_KEY_HANDLER_FILE_SUB_DIRECTORY , 0755);
  #endif
  if ( result != 0 )
  {
    SSM_MSG_HIGH_1 ("SSM : SFS Directory creation failed %d ", efs_errno);
  }

  result = snprintf( filename, sizeof(filename), 
            SSM_KEY_HANDLER_FILE_SUB_DIRECTORY"kh_%d_%d_%d.key" , 
                           key_req->info.usage,
			   key_req->info.crypto_algo,
			   key_req->info.source );

  if ( result <= 0)
  {
    /* Early return */
    SSM_MSG_ERROR ( " File name creation failed " );
     
    return E_SSM_KEY_HANDLER_FAILED;
  }
  
  /* Get file handle for SSM key handler file and check if it is valid */
  #ifndef FEATURE_SSM_USE_EFS_TEST
  fp = sfs_open( filename, O_CREAT | O_TRUNC | O_WRONLY );
  if ( fp == 0 )
  {
  #else
  fp = efs_open( filename, O_CREAT | O_TRUNC | O_WRONLY  );
  if ( fp < 0 )
  {
  #endif
    /* Early return */
    SSM_MSG_ERROR_1 ( " Key handler File open failed %d ", efs_errno );
     
    return E_SSM_KEY_HANDLER_FILE_OPEN_FAILED;
  }

  /* Write to the file */
  #ifndef FEATURE_SSM_USE_EFS_TEST
  result = sfs_write( fp, (char *)&key_req->info, sizeof( ssm_key_handler_info_t) );
  #else
  result = efs_write( fp, &key_req->info, sizeof( ssm_key_handler_info_t) );
  #endif

  if ( result != sizeof( ssm_key_handler_info_t) )
  {
    /* Early return */
    SSM_MSG_ERROR_1 ( " Cannot write to Key handler File %d ", efs_errno );
     
    #ifndef FEATURE_SSM_USE_EFS_TEST
    sfs_close( fp );
    #else
    efs_close( fp );
    #endif

    /* Delete the secure file */
    #ifndef FEATURE_SSM_USE_EFS_TEST
    if ( E_SUCCESS!=  sfs_rm( filename ) )
    #else
    if ( 0 != efs_unlink( filename ) )
    #endif
    {
      /* Early return */
      SSM_MSG_ERROR_1( " Key Write failed, also recovery failed %d ", efs_errno );
    }
    
    return E_SSM_KEY_HANDLER_FILE_WRITE_FAILED ;
  }

  /* Write to the file */
  #ifndef FEATURE_SSM_USE_EFS_TEST
  result = sfs_write( fp, (char *)key_req->key, key_req->info.len_key );
  #else
  result = efs_write( fp, key_req->key, key_req->info.len_key );
  #endif

  if ( result != key_req->info.len_key )
  {
    /* Early return */
    SSM_MSG_ERROR_1( " Cannot write to Key handler File %d ", efs_errno );
     
    #ifndef FEATURE_SSM_USE_EFS_TEST
    sfs_close( fp );
    #else
    efs_close( fp );
    #endif

    /* Delete the secure file */
    #ifndef FEATURE_SSM_USE_EFS_TEST
    if ( E_SUCCESS!=  sfs_rm( filename ) )
    #else
    if ( 0 != efs_unlink( filename ) )
    #endif
    {
      /* Early return */
      SSM_MSG_ERROR_1( " Key Write failed, also recovery failed %d ", efs_errno );
    }
    
    return E_SSM_KEY_HANDLER_FILE_WRITE_FAILED;   
  }

  #ifndef FEATURE_SSM_USE_EFS_TEST
  sfs_close( fp );
  #else
  efs_close( fp );
  #endif

  SSM_MSG_LOW ( " Key handler Add Key Succeeded  " );

  return E_SSM_KEY_HANDLER_SUCCESS ;

} /* ssm_key_handler_add_key */




/**
Processes request queued to Key Handler in a serialized manner through a task.

@param[in]  
None

@return 
none

@dependencies
None

*/

void ssm_key_handler_process_request(void)
{
  /* To be queued to the key handler */
	
  ssm_key_handler_queue_node_struct * temp;

  ssm_key_handler_struct_t          *key_req;

  ssm_key_handler_err_t             err;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Fetch Node from Key Handler request queue */
  temp = (ssm_key_handler_queue_node_struct *)q_get( &ssm_key_handler_req_q );

  while( temp!= NULL )
  {

    key_req = &(temp->key_req);

    if ( key_req  == NULL )
    {
      /** Clear up allocated space */
      free(temp);
    
      /* Fetch Node from Key Handler request queue */
      temp = (ssm_key_handler_queue_node_struct *)q_get( &ssm_key_handler_req_q );

      continue;
    }
    
    switch(key_req->req)
    {
      case SSM_KEY_HANDLER_ADD_KEY:
      {
        err = ssm_key_handler_add_key( key_req );
	/* Update status of the client */
        if ( key_req->cb )
        key_req->cb ( err,
		    key_req->req,
                    NULL,
		    NULL,
		    key_req->user_data ); 
        break;
      }
      case SSM_KEY_HANDLER_GET_KEY:
      {
        err = ssm_key_handler_get_key( key_req );
        /* Update status of the client */
        if ( key_req->cb )
        key_req->cb ( err,
		    key_req->req,
                    &(key_req->info),
		    key_req->key,
		    key_req->user_data ); 
        break;
      }
      case SSM_KEY_HANDLER_REMOVE_KEY:
      {
        /* Donot enable this feature .. it becomes a security concern'
	 as key can be deleted */
        #ifdef FEATURE_SSM_TEST_DEBUG
        err = ssm_key_handler_remove_key( key_req );
	/* Update status of the client */
        if ( key_req->cb )
        key_req->cb ( err,
		    key_req->req,
                    NULL,
		    NULL,
		    key_req->user_data ); 
        #endif
        break;
      }
      default:
       break;

    }


    /** Clear up allocated space */
    if ( key_req->key )
    {
      /* Zero out the key before freeing it up */
      secure_memset ( key_req->key, 0 , key_req->info.len_key );

      free( key_req->key );
    }
    free(temp);
    
    /* Fetch Node from Key Handler request queue */
    temp = (ssm_key_handler_queue_node_struct *)q_get( &ssm_key_handler_req_q );
  }

  
} /* ssm_key_handler_process_request */




