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

$Header: //components/rel/core.mpss/3.5.c12.3/services/ssm/src/ssm_och.c#1 $

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
#include "ssm_och_v.h"
#include "ssm_task_v.h"
#include "ssm_key_handler.h"
#include "fs_public.h"
#include "sfs_api.h"
#include "queue.h"
#include "ssm_v.h"
#include "PrngML.h"
#include "secapi.h"
#include <stringl/stringl.h>


/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/

#define SSM_OCH_FILE_ROOT_DIRECTORY               "/ssm"
#define SSM_OCH_FILE_SUB_DIRECTORY                "/ssm/och"
#define SSM_OCH_OVERRIDE_COOKIE_FILE              "/ssm/och/override.cookie"

/* The Contents of override cookie must be checked before validating the cookie.
 * This is to stop attacker who might have seen the cookie value on one device being
 * able to copy it to another device (without access to SFS). Since SFS strings are
 * encrypted with a unique per-device key, the existence of a file that can be
 * properly accessed is sufficient to show that the file belongs to this device.
 */
#define SSM_OCH_OVERRIDE_COOKIE_CONTENT           "Override Cookie must match"
#define SSM_OCH_OVERRIDE_COOKIE_CONTENT_LEN       26

/* Look up pointer for ssm tcb */
static rex_tcb_type* ssm_tcb_ptr=NULL;

/* Static data variable indicating override cookie is present in sfs
 * is set to TRUE only if contents of the cookie matches
 * SSM_OCH_OVERRIDE_COOKIE_CONTENT */
static boolean ssm_och_override_cookie_present          = FALSE;

/* Static data variable indicating override requested
 * True only when
 * 1) User enables Tempoprary overirde and it succeeds
 * 2) User enables Permanent override and writing cookie fails
 * 3) User enables Permanent override and writing cookie succeds */
static boolean ssm_och_override_done                    = FALSE;

/* Length of SHA 1 Hashed Output */
#define LEN_HASHED_OUTPUT 20

/* Length of random nonce */
#define LEN_RANDOM_NONCE 32

/* Static data variable holding random nonce */
static uint8 random_nonce[ LEN_RANDOM_NONCE ];

/* Static data variable indicating state of random nonce */
static boolean random_nonce_valid = FALSE;

/*-------------------------Callback Registration----------------------------- */

#define SSM_OCH_MAX_CB_REGISTRATION  5

static uint16 ssm_och_cb_registration_count = 0;

static ssm_och_register_cb_type  ssm_och_cb_register[SSM_OCH_MAX_CB_REGISTRATION];


/*----------------------------Command Queue--------------------------------- */

/* Queue to store commands to be processed through a task */
static q_type                            ssm_och_cmd_q;

typedef struct
{
  /* Link for the next queue */
  q_link_type                            link;

  /* SSM key handler structure */
  ssm_och_cmd_struct_t                   cmd_req;

} ssm_och_queue_node_struct;


/* -------------------------------Mutex------------------------------------- */
/* Pointer to the mutex object for protecting critical-section of ssm och code*/
static qurt_mutex_t ssm_och_mutex;
static qurt_mutex_t *pssm_och_mutex = NULL;


/* MACRO for locking/unlocking the qmutex */
#define SSM_OCH_INTLOCK()          \
  if ( NULL == pssm_och_mutex )              \
  {                                      \
    qurt_pimutex_init(&ssm_och_mutex);       \
    pssm_och_mutex = &ssm_och_mutex;             \
  }                                      \
  qurt_pimutex_lock(&ssm_och_mutex);

#define SSM_OCH_INTFREE()          \
  qurt_pimutex_unlock(&ssm_och_mutex);



/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/

#ifdef FEATURE_SSM_TEST_DEBUG

extern void ssm_och_sign_random_nonce
(
   ssm_och_cmd_struct_t   * cmd_req
);

#endif /* FEATURE_SSM_TEST_DEBUG */



/**
Set random nonce  valid

@param[in]
status           Status of randon nonce valid

@return
None

@dependencies
None

*/

void ssm_och_set_random_nonce_valid
(
  boolean status
)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Set Random nonce valid field */
  random_nonce_valid = status;

} /* ssm_och_set_random_nonce_valid */




/**
Gets override status

@param[in]
None

@return
SSM_OCH_OVERRIDE_SUCCESSFUL    -   Override allowed
SSM_OCH_OVERRIDE_NOT_SUCCESSFUL -  Override not allowed

@dependencies
None

*/

ssm_och_ovrr_status_t ssm_och_get_override_status(void)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( ssm_ach_get_authentication_cookie_status() !=
		  SSM_ACH_AUTHENTICATION_COOKIE_PRESENT )
    return SSM_OCH_OVERRIDE_SUCCESSFUL;

  if ( ssm_och_override_done || ssm_och_override_cookie_present )
    return SSM_OCH_OVERRIDE_SUCCESSFUL;

  return SSM_OCH_OVERRIDE_NOT_SUCCESSFUL;

} /* ssm_och_get_override_status */



/**
Updates Status of Override Command Handler through Callback

@param[in]
status   -   Status of the Override Command Handler

@return
None

@dependencies
None

*/

static void ssm_och_update_status_thru_cb
(
  boolean status
)
{
  uint16 iter;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  SSM_OCH_INTLOCK()

  /* Iter through callabck table to find duplicate */
  for ( iter =0; iter < ssm_och_cb_registration_count; iter ++ )
  {
    if ( ssm_och_cb_register[iter] != NULL )
    {
       ssm_och_cb_register[iter]( status );
    }
  }
  SSM_OCH_INTFREE()

} /* ssm_och_update_status_thru_cb */



/**
Initializes SSM override command handler file reads.
The Initialization sequence is delayed since it needs to be performed
only once sfs initialies or is initialized from RCINIT Group 0

@param[in]
None

@return
None

@dependencies
None

*/

void ssm_och_delayed_init(void)
{

  int fp;
  uint8 read[SSM_OCH_OVERRIDE_COOKIE_CONTENT_LEN];
  int result;

  static boolean ssm_och_delayed_init = FALSE;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( ssm_och_delayed_init == TRUE )
    return;

  /* Initialize ACH module */
  ssm_ach_delayed_init();

  /* Check if Override Cookie is present and check if contents matches
   * SSM_OCH_OVERRIDE_COOKIE_CONTENT */

  #ifndef FEATURE_SSM_USE_EFS_TEST
  fp = sfs_open( SSM_OCH_OVERRIDE_COOKIE_FILE, O_RDONLY );
  if ( fp == 0 )
  {
  #else
  fp = efs_open( SSM_OCH_OVERRIDE_COOKIE_FILE, O_RDONLY  );
  if ( fp < 0  )
  {
  #endif
    ssm_och_override_cookie_present = FALSE;

    /* Update Override status through callback */
    ssm_och_update_status_thru_cb ( ssm_och_get_override_status() );

    SSM_MSG_MED_1 ( " Override Cookie File Not Present %d ", efs_errno);

    return;
  }


  #ifndef FEATURE_SSM_USE_EFS_TEST
  result = sfs_read( fp, (char *) read,
                      SSM_OCH_OVERRIDE_COOKIE_CONTENT_LEN );
  #else
  result = efs_read( fp, read,
                      SSM_OCH_OVERRIDE_COOKIE_CONTENT_LEN );
  #endif

  if ( result != SSM_OCH_OVERRIDE_COOKIE_CONTENT_LEN )
  {
    ssm_och_override_cookie_present = FALSE;

    /* Early return */
    SSM_MSG_ERROR_1 ( " Cannot Read from Override Cookie File %d", efs_errno);

    #ifndef FEATURE_SSM_USE_EFS_TEST
    sfs_close( fp );
    #else
    efs_close( fp );
    #endif

    /* Update Override status through callback */
    ssm_och_update_status_thru_cb ( ssm_och_get_override_status() );

    return ;
  }

  if ( 0 != memcmp( SSM_OCH_OVERRIDE_COOKIE_CONTENT, read,
                    SSM_OCH_OVERRIDE_COOKIE_CONTENT_LEN) )
  {
    ssm_och_override_cookie_present = FALSE;

    #ifndef FEATURE_SSM_USE_EFS_TEST
    sfs_close( fp );
    #else
    efs_close( fp );
    #endif

    /* Update Override status through callback */
    ssm_och_update_status_thru_cb ( ssm_och_get_override_status() );

    return;

  }

  ssm_och_override_cookie_present = TRUE;
  ssm_och_delayed_init = TRUE;

  #ifndef FEATURE_SSM_USE_EFS_TEST
  sfs_close( fp );
  #else
  efs_close( fp );
  #endif

  /* Update Override status through callback */
  ssm_och_update_status_thru_cb ( ssm_och_get_override_status() );


} /* ssm_och_delayed_init */



/**
Registers for updates for override handler through callback

@param[in]
None

@return
E_SSM_OCH_SUCCESS -  Registration Successful
E_SSM_OCH_FAILED  -  Registration Failed

@dependencies
None

*/

ssm_och_err_t ssm_och_register_cb
(
  ssm_och_register_cb_type cb
)
{
  uint16 iter;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* If callback is NULL, discard the value */
  if ( cb == NULL )
    return E_SSM_OCH_FAILED;

  SSM_OCH_INTLOCK()

  /* Iter through callabck table to find duplicate */
  for ( iter =0; iter < ssm_och_cb_registration_count; iter ++ )
  {
    if ( ssm_och_cb_register[iter] == cb )
    {
       SSM_OCH_INTFREE()
       return E_SSM_OCH_SUCCESS;
    }
  }
  if ( ssm_och_cb_registration_count < SSM_OCH_MAX_CB_REGISTRATION )
  {
    /* Save callback */
    ssm_och_cb_register[ssm_och_cb_registration_count++]=cb;

    SSM_OCH_INTFREE()

    /* Initialize Cookie reads */
    ssm_och_delayed_init();

    return E_SSM_OCH_SUCCESS;
  }

  SSM_OCH_INTFREE()
  return E_SSM_OCH_FAILED;

} /* ssm_och_register_cb */



/**
Callback from ACH about status authentication cookie

@param[in]
status   -   Status of the ACH

@return
None

@dependencies
None

*/

void ssm_och_ach_cb
(
  ssm_ach_authentication_coookie_status_t status
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ssm_och_update_status_thru_cb( ssm_och_get_override_status() );

} /* ssm_och_ach_cb*/



/**
Initializes SSM override command handler module and its global variable

@param[in]
None

@return
None

@dependencies
None

*/

void ssm_och_init(void)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  if ( NULL == pssm_och_mutex )
  {
    qurt_pimutex_init(&ssm_och_mutex);
    pssm_och_mutex = &ssm_och_mutex;
  }


  ssm_tcb_ptr = rcinit_lookup_rextask("ssm");

  if ( ssm_tcb_ptr == NULL )
  {
    SSM_ERR_FATAL(" SSM task tcb not found", 0 ,0, 0 );
  }

  /* Initialize Queue */
  q_init( &ssm_och_cmd_q );

  /* Initialize for callback updates from ach */
  if ( E_SSM_ACH_SUCCESS != ssm_ach_register_cb ( ssm_och_ach_cb ) )
  {
    SSM_MSG_ERROR (" Registration with ACH failed ");
  }

} /* ssm_och_init */



/**
Enqueues OCH Command  request to the queue

@param[in]
cmd  -  Structure allocated by the client for OCH commands
        Should be deallocated after the call to the function.
        Only Deallocate the cmd structure and not the memory pointed by
	uint8 * signature within cmd.

@return
ssm_och_err_t  - E_SSM_OCH_FAILED        Operation failed
                 E_SSM_OCH_IN_PROGRESS   Operation in Progress

@dependencies
None

*/

ssm_och_err_t ssm_och_request
(
  /** Pointer to structure allocated by clients for OCH commands */
  ssm_och_cmd_struct_t      * cmd
)
{

  ssm_och_queue_node_struct  * temp;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( cmd == NULL )
  {
    return E_SSM_OCH_FAILED;
  }

  /* Allocate space for request to be queued up */
  temp = ( ssm_och_queue_node_struct  *)malloc(
		                    sizeof(ssm_och_queue_node_struct));

  if ( temp == NULL )
  {
    return E_SSM_OCH_DYNAMIC_ALLOCATION_FAILED;
  }

  /* Copy the data from cmd_req to temp->cmd_req */
  if ( sizeof( ssm_och_cmd_struct_t ) != memscpy( (void *) &( temp->cmd_req),
		   sizeof( ssm_och_cmd_struct_t ),
		   (void *) cmd, sizeof( ssm_och_cmd_struct_t )) )
  {
     free ( temp );
     return E_SSM_OCH_FAILED;
  }

  if ( cmd->ovrr_cmd.cmd == SSM_OCH_TEMP_OVERRIDE
    || cmd->ovrr_cmd.cmd == SSM_OCH_PERM_OVERRIDE  )
  {
    if ( cmd->ovrr_cmd.signature )
    {
       temp->cmd_req.ovrr_cmd.signature = (uint8 *) malloc
	                                 ( temp->cmd_req.ovrr_cmd.len_sig );

       if (  temp->cmd_req.ovrr_cmd.signature == NULL )
       {
         free ( temp );
	 return E_SSM_OCH_DYNAMIC_ALLOCATION_FAILED;
       }

       if ( temp->cmd_req.ovrr_cmd.len_sig !=
		       memscpy( (void *) temp->cmd_req.ovrr_cmd.signature,
		       temp->cmd_req.ovrr_cmd.len_sig,
		       cmd->ovrr_cmd.signature, temp->cmd_req.ovrr_cmd.len_sig  ) )
       {
	 free ( temp );
         return E_SSM_OCH_FAILED;
       }
    }
  }

  #ifdef FEATURE_SSM_TEST_DEBUG
  else if ( cmd->sign_cmd.cmd == SSM_OCH_SIGN_RANDOM_NONCE )
  {
    if ( cmd->sign_cmd.mod )
    {
       temp->cmd_req.sign_cmd.mod = (uint8 *) malloc
	                                 ( temp->cmd_req.sign_cmd.mod_len );

       if (  temp->cmd_req.sign_cmd.mod == NULL )
       {
         free ( temp );
	 return E_SSM_OCH_DYNAMIC_ALLOCATION_FAILED;
       }

       if ( temp->cmd_req.sign_cmd.mod_len !=
		       memscpy( (void *) temp->cmd_req.sign_cmd.mod,
		       temp->cmd_req.sign_cmd.mod_len, cmd->sign_cmd.mod,
		       temp->cmd_req.sign_cmd.mod_len  ) )
       {
         free ( temp->cmd_req.sign_cmd.mod );
	     free ( temp );
         return E_SSM_OCH_FAILED;
       }
    }

    if ( cmd->sign_cmd.exp )
    {
       temp->cmd_req.sign_cmd.exp = (uint8 *) malloc
	                                 ( temp->cmd_req.sign_cmd.exp_len );

       if (  temp->cmd_req.sign_cmd.exp == NULL )
       {
         free ( temp->cmd_req.sign_cmd.mod );
	     free ( temp );
	     return E_SSM_OCH_DYNAMIC_ALLOCATION_FAILED;
       }

       if ( temp->cmd_req.sign_cmd.exp_len !=
		       memscpy( (void *) temp->cmd_req.sign_cmd.exp,
		       temp->cmd_req.sign_cmd.exp_len, cmd->sign_cmd.exp,
		       temp->cmd_req.sign_cmd.exp_len  ) )
       {
	     free ( temp->cmd_req.sign_cmd.exp );
	     free ( temp->cmd_req.sign_cmd.mod );
	     free ( temp );
         return E_SSM_OCH_FAILED;
       }
    }
  }
  #endif /* FEATURE_SSM_TEST_DEBUG */


  /* Link temp to ssm_och_req_q */
  (void) q_link( temp, &temp->link );

  /* Enqueue node in ssm_och_cmd_q */
  q_put( &ssm_och_cmd_q, &temp->link);

  /* Signal ssm_task to handle the request */
  rex_set_sigs(ssm_tcb_ptr, SSM_OCH_PROCESS_SIG );

  /* Return status of the requests */
  return E_SSM_OCH_IN_PROGRESS;

} /* ssm_och_request */


 
/**
Function generator for SHA 1 hash of  ( ( 0 or 1) || ranodn_nonce )


@param[in]
  boolean   -            Indicates if it is meant for temp override
                         or permanent override


@param[out]
  uint8 *                Hashed output of length of 20 bytes

@return
ssm_och_err_t

@dependencies
None

*/

ssm_och_err_t ssm_och_get_hash_output_random_nonce
(
  boolean                      temp_override,
  uint8 *                      hash_output

)
{
  secapi_handle_type           handle;
  secerrno_enum_type           _errno;
  secapi_hsh_param_data_type   hash_param;
  uint8 *                      hash_input;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( hash_output == NULL )
  {
    return E_SSM_OCH_FAILED;
  }

  /* Allocate space for SHA1 input , it it fails , bail out */
  hash_input = (uint8 *) malloc( 1 + LEN_RANDOM_NONCE );

  if ( hash_input == NULL )
  {
    return E_SSM_OCH_DYNAMIC_ALLOCATION_FAILED;
  }

  /* Get 8 bit value for depending on override requested */
  if ( temp_override )
  {
    hash_input[0] = 0;
  }
  else
  {
    hash_input[0] = 1;
  }

  SSM_OCH_INTLOCK()

  if ( random_nonce_valid == FALSE )
  {
    SSM_OCH_INTFREE()
    free( hash_input );
    return E_SSM_OCH_RANDOM_NONCE_INVALID;
  }

  /* Update hash_input with random nonce */
  if ( LEN_RANDOM_NONCE !=
          memscpy( hash_input + 1, LEN_RANDOM_NONCE, random_nonce, LEN_RANDOM_NONCE ))
  {
    SSM_OCH_INTFREE()
    free( hash_input );
    return E_SSM_OCH_RANDOM_NONCE_INVALID;
  }

  /* Set Random Nonce to be invalid */
  ssm_och_set_random_nonce_valid( FALSE );

  SSM_OCH_INTFREE()

  /* Get an instance for SHA1 */
  _errno = secapi_new (&handle, SECAPI_SHA);
  if ( _errno )
  {
    free( hash_input );
    return E_SSM_OCH_SEC_API_FAILED;
  }

  /* Set mode for SHA1 operation on ( 8 bit value || Random Nonce ) */
  hash_param.mode = SECAPI_HSH_INPUT_MODE_ATOMIC;
  _errno = secapi_hsh_set_param( handle, SECAPI_SHA, SECAPI_HSH_INPUT_MODE,
		  &hash_param );

  if ( _errno != E_SUCCESS )
  {
    free( hash_input );
    return E_SSM_OCH_SEC_API_FAILED;
  }

  /* Get SHA1 hash value for ( 8 bit value || Random Nonce )  */
  _errno = secapi_hsh_create_digest( handle, SECAPI_SHA, (uint8 *) hash_input,
		                     1 + LEN_RANDOM_NONCE, hash_output );

  /* Free up hash input as this is no longer needed */
  free( hash_input );

  if ( _errno != E_SUCCESS )
  {
    return E_SSM_OCH_SEC_API_FAILED;
  }

  /* Delete instance for SHA1 handle */
  _errno = secapi_delete( &handle );

  if ( _errno != E_SUCCESS )
  {
    return E_SSM_OCH_SEC_API_FAILED;
  }

  return E_SSM_OCH_SUCCESS;

} /* ssm_och_get_hash_output_random_nonce */



/**
Function check for signature valid

@param[in]
  uint8 * sig                 Signature
  uint32 len_sig             Length of signature
  uint8 * key                 Key
  uint32 len_key             Lenth of key

@return
ssm_och_err_t

@dependencies
None

*/

ssm_och_err_t ssm_och_check_sig_valid
(
   /** Signature */
   uint8 * sig,

   /** Length of the signature */
   uint32 len_sig,

   /** Public key */
   uint8 * key,

   /** Length of public key */
   uint16 len_key,

   /** Indicate permanenet or Temporary override */
   boolean temp_override

)
{
  uint8 * mod, *exp ;

  uint16 *mod_len, exp_len=0;

  secapi_handle_type           handle;
  secerrno_enum_type           _errno;
  secapi_pkx_param_data_type   rsa_param;

  uint8 *                      hash_output;
  uint16                       authenticate_out_len;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Allocate space for hash_output, it it fails , bail out */
  hash_output = (uint8 *) malloc( LEN_HASHED_OUTPUT );

  if ( hash_output == NULL )
  {
    /* Zero out before freeing up */
    secure_memset( sig , 0 , len_sig );
    free ( sig );
    return E_SSM_OCH_DYNAMIC_ALLOCATION_FAILED;
  }

  /* Get Hash Output of ( ( 0 or 1 ) || random_nonce ) */
  ssm_och_err_t err = ssm_och_get_hash_output_random_nonce( temp_override,
		      hash_output );

  if ( err != E_SSM_OCH_SUCCESS || hash_output == NULL )
  {
    /* Zero out before freeing up */
    secure_memset( sig , 0 , len_sig );
    free ( sig );
    free ( hash_output );

    return err;
  }

  /* Get mod_len and exp_len */
  /* Assume that key stored in key handler has its first 2 bytes as the
   * length of modulus' */
  if ( len_key < 2 )
  {
    /* Zero out before freeing up */
    secure_memset( sig , 0 , len_sig );
    free ( sig );
    free ( hash_output );

    return E_SSM_OCH_INVALID_KEY;
  }

  /* Cast Mod_len to the first bytes of the key */
  mod_len = (uint16 *) key;

  /* Check if (mod_len ) is smaller than (length of the key - 2) */
  if ( *mod_len  >= (len_key - 2) )
  {
    /* Zero out before freeing up */
    secure_memset( sig , 0 , len_sig );
    free ( sig );
    free ( hash_output );

    return E_SSM_OCH_INVALID_KEY;
  }

  /* Calculate exponent length */
  exp_len = len_key- *mod_len - 2;

  /* Allocate space for mod, it it fails , bail out */
  mod = ( uint8 * ) malloc(*mod_len);

  if ( mod == NULL )
  {
    /* Zero out before freeing up */
    secure_memset( sig , 0 , len_sig );
    free ( sig );
    free ( hash_output );

    return E_SSM_OCH_DYNAMIC_ALLOCATION_FAILED;
  }

  /* Allocate space for exp, it it fails , bail out */
  exp = ( uint8 * ) malloc(exp_len);

  if ( exp == NULL )
  {
    /* Zero out before freeing up */
    secure_memset( sig , 0 , len_sig );
    free ( sig );
    free ( hash_output );
    free ( mod );

    return E_SSM_OCH_DYNAMIC_ALLOCATION_FAILED;
  }

  /* Update mod and exp  */
  if ( *mod_len != memscpy ( mod, *mod_len, key + 2,  *mod_len  ))
  {
    /* Zero out before freeing up */
    secure_memset( sig , 0 , len_sig );
    free ( sig );
    free ( hash_output );
    free ( mod );
    free ( exp );

    return E_SSM_OCH_FAILED;
  }

  if ( exp_len != memscpy ( exp, exp_len, key + *mod_len +2 , exp_len ) )
  {
    /* Zero out before freeing up */
    secure_memset( sig , 0 , len_sig );
    free ( sig );
    free ( hash_output );
    free ( mod );
    free ( exp );

    return E_SSM_OCH_FAILED;
  }

  /* Update input length of plain message to RSA function */
  authenticate_out_len = LEN_HASHED_OUTPUT;

  /* Create instance for RSA handle */
  _errno = secapi_new( &handle, SECAPI_RSA );

  if (_errno != E_SUCCESS)
  {
    /* Zero out before freeing up */
    secure_memset( sig , 0 , len_sig );
    free ( sig );
    free ( hash_output );
    free ( mod );
    secure_memset( exp , 0 , exp_len );
    free( exp );


    SSM_MSG_ERROR_1( "SSM : RSA instance (%d)", _errno);
    return E_SSM_OCH_FAILED;
  }

  /* Load  modulus */
  rsa_param.rsa.key.modulus_size = *mod_len;
  rsa_param.rsa.key.modulus_ptr  = mod;
  _errno = secapi_pkx_set_param( handle,
                                 SECAPI_RSA,
                                 SECAPI_PKX_RSA_MODULUS,
                                 &rsa_param);
  if (_errno != E_SUCCESS)
  {
    /* Zero out before freeing up */
    secure_memset( sig , 0 , len_sig );
    free ( sig );
    free ( hash_output );
    free ( mod );
    secure_memset( exp , 0 , exp_len );
    free( exp );

    SSM_MSG_ERROR_1( " SSM : RSA modulus (%d)", _errno);
    return E_SSM_OCH_SEC_API_FAILED;
  }

  /* Load exponent e */
  rsa_param.rsa.key.key_size       = exp_len;
  rsa_param.rsa.key.public_key_ptr = exp;
  _errno = secapi_pkx_set_param( handle,
                                 SECAPI_RSA,
                                 SECAPI_PKX_RSA_PUBLIC_KEY,
                                 &rsa_param );
  if (_errno != E_SUCCESS)
  {
    /* Zero out before freeing up */
    secure_memset( sig , 0 , len_sig );
    free ( sig );
    free ( hash_output );
    free ( mod );
    secure_memset( exp , 0 , exp_len );
    free( exp );

    SSM_MSG_ERROR_1( " SSM : RSA public key (%d)", _errno);
    return E_SSM_OCH_SEC_API_FAILED;
  }

  /* Set up execution mode. */
  /* Assume blocking mode execution - no callback */
  rsa_param.common.exec_mode = SECAPI_SYNC_EXEC;
  _errno = secapi_pkx_set_param( handle,
                                 SECAPI_RSA,
                                 SECAPI_EXEC_MODE,
                                 &rsa_param );
  if (_errno != E_SUCCESS)
  {
    /* Zero out before freeing up */
    secure_memset( sig , 0 , len_sig );
    free ( sig );
    free ( hash_output );
    free ( mod );
    secure_memset( exp , 0 , exp_len );
    free( exp );

    SSM_MSG_ERROR_1( "SSM : RSA mode (%d)", _errno);
    return E_SSM_OCH_SEC_API_FAILED;
  }

  /* Set up execution platform. */
  rsa_param.common.exec_platform =
	               SECAPI_EXEC_PLATFORM_ACCELERATOR_PREFERRED;
  _errno = secapi_pkx_set_param( handle,
                                 SECAPI_RSA,
                                 SECAPI_EXEC_PLATFORM,
                                 &rsa_param );
  if (_errno != E_SUCCESS)
  {
    /* Zero out before freeing up */
    secure_memset( sig , 0 , len_sig );
    free ( sig );
    free ( hash_output );
    free ( mod );
    secure_memset( exp , 0 , exp_len );
    free( exp );

    SSM_MSG_ERROR_1( "SSM : RSA platform (%d)", _errno);
    return E_SSM_OCH_SEC_API_FAILED;
  }

  /* Call into Authentication Algorithm to verify the Signautre  */
  _errno = secapi_pkx_authenticate( handle,
                                    SECAPI_RSA,
				    SECAPI_PKX_RSA_PADDING_SIG_PSS,
				    (uint8 *) sig,
				    len_sig,
				    hash_output,
				    &authenticate_out_len );

  /* Free up Dynamically allocated space */
  /* Zero out before freeing up */
  secure_memset( sig , 0 , len_sig );
  free ( sig );
  free ( hash_output );
  free ( mod );
  secure_memset( exp , 0 , exp_len );
  free( exp );

  if (_errno != E_SUCCESS)
  {
    SSM_MSG_ERROR_1( "SSM : RSA platform (%d)", _errno);
    return E_SSM_OCH_SIGNATURE_VERIFICATION_FAILED;
  }

  return E_SSM_OCH_SUCCESS;

} /* ssm_och_check_sig_valid */




/**
Callback for Temp overide

@param[in]
  ssm_key_handler_err_t         err    Depicts status of key handler request
  ssm_key_handler_req_t         req    SSM Key Handler Operation
  ssm_key_handler_info_t        *info   SSM Key Handler info
  uint8                         *key   SSM Key Handler key
  ssm_key_handler_cb_data_type  data   Contains the OCH callbakc


@return
None

@dependencies
None

*/


static void ssm_och_temp_override_cb
(
  /** Error type : Depicts status of key handler request */
  ssm_key_handler_err_t         err,

  /** SSM Key Handler Operation */
  ssm_key_handler_req_t         req,

  /** SSM Key Handler info */
  ssm_key_handler_info_t        *info,

  /** SSM Key Handler key */
  uint8                         *key,

  /** SSM key Handler cb data */
  ssm_key_handler_cb_data_type   data
)
{
  ssm_och_cmd_struct_t           *cmd_req;

  ssm_och_err_t                  err_och;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  /* Update client through callback stored in data member */
  cmd_req = (ssm_och_cmd_struct_t *)data;
  if ( cmd_req == NULL )
  {
    SSM_MSG_ERROR( " Expect some data from key handler ");
    return;
  }


  if ( err != E_SSM_KEY_HANDLER_SUCCESS )
  {
     if( cmd_req->ovrr_cmd.cb )
       cmd_req->ovrr_cmd.cb ( E_SSM_OCH_INVALID_KEY, 0 , NULL,
		              cmd_req->ovrr_cmd.user_data ) ;

     free(cmd_req->ovrr_cmd.signature);
     free(cmd_req);

     return;
  }

  if ( key == NULL || info == NULL )
  {
     if( cmd_req->ovrr_cmd.cb )
       cmd_req->ovrr_cmd.cb ( E_SSM_OCH_INVALID_KEY, 0 , NULL,
		              cmd_req->ovrr_cmd.user_data ) ;

     free(cmd_req->ovrr_cmd.signature);
     free(cmd_req);

     return;
  }



  /* In the callback, checks if signature is valid */
  err_och = ssm_och_check_sig_valid( cmd_req->ovrr_cmd.signature,
		                     cmd_req->ovrr_cmd.len_sig,
		                     key, info->len_key, TRUE );
  if ( E_SSM_OCH_SUCCESS == err_och )
  {
    /* If signature is valid, set ssm_och_override_done as TRUE */
    ssm_och_override_done = TRUE;

    if( cmd_req->ovrr_cmd.cb )
       cmd_req->ovrr_cmd.cb ( err_och, 0 , NULL,
		              cmd_req->ovrr_cmd.user_data ) ;

  }
  else
  {
    /* If signature is invalid, set ssm_och_override_done as FALSE */

    ssm_och_override_done = FALSE ;

    if( cmd_req->ovrr_cmd.cb )
       cmd_req->ovrr_cmd.cb ( err_och, 0 , NULL,
		              cmd_req->ovrr_cmd.user_data ) ;
  }

  /* Free up allocated space */
  free( cmd_req );

  /* Update Override status through callback */
  ssm_och_update_status_thru_cb ( ssm_och_get_override_status() );

} /* ssm_och_temp_override_cb */




/**
Temporary override

@param[in]
cmd_req   -  Structure allocated by the task in which key handler is processed and
             deallocated after the call to the function.

@return
None

@dependencies
None

*/

static void ssm_och_temp_override
(
   ssm_och_cmd_struct_t   * cmd_req
)
{

  /* Key handler request structure */
  ssm_key_handler_struct_t  key_request;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Update SSM Key Handler Key structure to get a key */
  key_request.req = SSM_KEY_HANDLER_GET_KEY;
  key_request.cb  = ssm_och_temp_override_cb;
  key_request.info.usage = SSM_KEY_HANDLER_USAGE_OVERRIDE ;
  key_request.info.source = cmd_req->ovrr_cmd.source ;
  key_request.info.crypto_algo =
	  SSM_KEY_HANDLER_CRYPTO_ALGO_RSA_PKCS_1_PSS_VERIFICATION_KEY ;
  key_request.info.len_key  = 0;
  key_request.key = NULL;
  key_request.user_data = (void * ) cmd_req;

  /* Submit the request to Key Handler */
  if ( E_SSM_KEY_HANDLER_FAILED == ssm_key_handler_request( &key_request ))
  {
    if ( cmd_req->ovrr_cmd.cb )
     cmd_req->ovrr_cmd.cb ( E_SSM_OCH_FAILED, 0 , NULL,
		            cmd_req->ovrr_cmd.user_data );
  }

} /* ssm_och_temp_override */



/**
Callback for Perm overide

@param[in]
  ssm_key_handler_err_t         err    Depicts status of key handler request
  ssm_key_handler_req_t         req    SSM Key Handler Operation
  ssm_key_handler_info_t        *info   SSM Key Handler info
  uint8                         *key   SSM Key Handler key
  ssm_key_handler_cb_data_type  data   Contains the OCH callbakc


@return
None

@dependencies
None

*/


static void ssm_och_perm_override_cb
(
  /** Error type : Depicts status of key handler request */
  ssm_key_handler_err_t         err,

  /** SSM Key Handler Operation */
  ssm_key_handler_req_t         req,

  /** SSM Key Handler info */
  ssm_key_handler_info_t        *info,

  /** SSM Key Handler key */
  uint8                          *key,

  /** SSM key Handler cb data */
  ssm_key_handler_cb_data_type   data
)
{
  ssm_och_cmd_struct_t           *cmd_req;

  ssm_och_err_t                  err_och;

  int                            result;

  int                            fp;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  /* Update client through callback stored in data member */
  cmd_req = (ssm_och_cmd_struct_t *)data;

  if ( cmd_req == NULL )
  {
    SSM_MSG_ERROR( " Expect some data from key handler " );
    return;
  }

  if ( err != E_SSM_KEY_HANDLER_SUCCESS )
  {
     if( cmd_req->ovrr_cmd.cb )
       cmd_req->ovrr_cmd.cb ( E_SSM_OCH_INVALID_KEY, 0 , NULL,
		              cmd_req->ovrr_cmd.user_data ) ;

     free(cmd_req->ovrr_cmd.signature);
     free(cmd_req);

     return;
  }

  if ( key == NULL || info == NULL )
  {
     if( cmd_req->ovrr_cmd.cb )
       cmd_req->ovrr_cmd.cb ( E_SSM_OCH_INVALID_KEY, 0 , NULL,
		              cmd_req->ovrr_cmd.user_data ) ;

     free(cmd_req->ovrr_cmd.signature);
     free(cmd_req);

     return;
  }

  /* In the callback, checks if signature is valid */
  err_och = ssm_och_check_sig_valid( cmd_req->ovrr_cmd.signature,
		                     cmd_req->ovrr_cmd.len_sig,
		                     key, info->len_key, FALSE );

  if ( E_SSM_OCH_SUCCESS == err_och  )
  {
    /* If signature is valid, set ssm_och_override_done as TRUE */
    ssm_och_override_done = TRUE;

    /* Update Override status through callback */
    ssm_och_update_status_thru_cb ( ssm_och_get_override_status() );

    /* Provision override Cookie */

    /* Create SSM directory in the filesystem */
    /* efs_mkdir returns 0 if the directory is not present in the file system and
       it can create efs directory successfully. It returns -1 if it fails or
       if the directory is present in the filesystem */

    result = efs_mkdir (SSM_OCH_FILE_ROOT_DIRECTORY, 0755 );
    if ( result != 0 )
    {
      SSM_MSG_HIGH_1 ("SSM : EFS Directory creation failed %d", efs_errno);
    }

    #ifndef FEATURE_SSM_USE_EFS_TEST
    /* Create a sub-directory within SSM directory in the filesystem */
    result = sfs_mkdir ( SSM_OCH_FILE_SUB_DIRECTORY );
    #else
    result = efs_mkdir( SSM_OCH_FILE_SUB_DIRECTORY , 0755);
    #endif

    if ( result != 0 )
    {
      SSM_MSG_HIGH_1 ("SSM : SFS Directory creation failed %d", efs_errno);
    }

    /* Get file handle for SSM key handler file and check if it is valid */
    #ifndef FEATURE_SSM_USE_EFS_TEST
    fp = sfs_open( SSM_OCH_OVERRIDE_COOKIE_FILE, O_CREAT | O_TRUNC | O_WRONLY );
    if ( fp == 0 )
    {
    #else
    fp = efs_open( SSM_OCH_OVERRIDE_COOKIE_FILE, O_CREAT | O_TRUNC | O_WRONLY );
    if ( fp < 0 )
    {
    #endif
      /* Early return */
      SSM_MSG_ERROR_1 ( " Override Cookie File open failed %d", efs_errno);

      /* Update status of the client */
      if ( cmd_req->ovrr_cmd.cb )
        cmd_req->ovrr_cmd.cb ( E_SSM_OCH_OVERRIDE_SUCCEEDED_BUT_COOKIE_CREATE_FAILED,
			       0 , NULL, cmd_req->ovrr_cmd.user_data );

      /* Free up allocated space */
      free( cmd_req );

      return ;
    }

    /* Write to the file */
    #ifndef FEATURE_SSM_USE_EFS_TEST
    result = sfs_write( fp, (char *)SSM_OCH_OVERRIDE_COOKIE_CONTENT,
                        SSM_OCH_OVERRIDE_COOKIE_CONTENT_LEN );
    #else
    result = efs_write( fp, SSM_OCH_OVERRIDE_COOKIE_CONTENT,
                        SSM_OCH_OVERRIDE_COOKIE_CONTENT_LEN );
    #endif

    if ( result != SSM_OCH_OVERRIDE_COOKIE_CONTENT_LEN )
    {
      /* Early return */
      SSM_MSG_ERROR_1 ( " Cannot write to Override Cookie File %d", efs_errno);

      /* Update status of the client */
      if ( cmd_req->ovrr_cmd.cb )
        cmd_req->ovrr_cmd.cb ( E_SSM_OCH_OVERRIDE_SUCCEEDED_BUT_COOKIE_WRITE_FAILED ,
			       0 , NULL, cmd_req->ovrr_cmd.user_data  );

      #ifndef FEATURE_SSM_USE_EFS_TEST
      sfs_close( fp );
      #else
      efs_close( fp );
      #endif

      /* Delete the Override Cookie in failure Case */
      #ifndef FEATURE_SSM_USE_EFS_TEST
      if ( E_SUCCESS!=  sfs_rm( SSM_OCH_OVERRIDE_COOKIE_FILE ) )
      #else
      if ( 0 != efs_unlink( SSM_OCH_OVERRIDE_COOKIE_FILE ) )
      #endif
      {
        /* Early return */
        SSM_MSG_ERROR_1 ( " File remove failed %d", efs_errno);

      }

      /* Free up allocated space */
      free( cmd_req );

      return ;
    }

    ssm_och_override_cookie_present = TRUE;

    #ifndef FEATURE_SSM_USE_EFS_TEST
    sfs_close( fp );
    #else
    efs_close( fp );
    #endif

  }
  else
  {
    /* If signature is invalid, set ssm_och_override_done as FALSE */
    ssm_och_override_done = FALSE ;

  }

  if( cmd_req->ovrr_cmd.cb )
       cmd_req->ovrr_cmd.cb ( err_och, 0 , NULL, cmd_req->ovrr_cmd.user_data ) ;

  /* Update Override status through callback */
  ssm_och_update_status_thru_cb ( ssm_och_get_override_status() );

  /* Free up allocated space */
  free( cmd_req );


} /* ssm_och_perm_override_cb */



/**
Permanent override

@param[in]
cmd_req   -  Structure allocated by the task in which key handler is processed and
             deallocated after the call to the function.

@return
None

@dependencies
None

*/

static void ssm_och_perm_override
(
   ssm_och_cmd_struct_t   * cmd_req
)
{

  /* Key handler request structure */
  ssm_key_handler_struct_t  key_request;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Update SSM Key Handler Key structure to get a key */
  key_request.req = SSM_KEY_HANDLER_GET_KEY;
  key_request.cb  = ssm_och_perm_override_cb;
  key_request.info.usage = SSM_KEY_HANDLER_USAGE_OVERRIDE ;
  key_request.info.source = cmd_req->ovrr_cmd.source ;
  key_request.info.crypto_algo =
	  SSM_KEY_HANDLER_CRYPTO_ALGO_RSA_PKCS_1_PSS_VERIFICATION_KEY ;
  key_request.info.len_key  = 0;
  key_request.key = NULL;
  key_request.user_data = (void * ) cmd_req;

  /* Submit the request to Key Handler */
  if ( E_SSM_KEY_HANDLER_FAILED == ssm_key_handler_request( &key_request ))
  {
    if ( cmd_req->ovrr_cmd.cb )
     cmd_req->ovrr_cmd.cb ( E_SSM_OCH_FAILED, 0 , NULL, cmd_req->ovrr_cmd.user_data );
  }

} /* ssm_och_perm_override */




/**
Cancel override

@param[in]
cmd_req   -  Structure allocated by the task in which key handler is processed and
             deallocated after the call to the function.

@return
None

@dependencies
None

*/

static void ssm_och_cancel_override
(
   ssm_och_cmd_struct_t   * cmd_req
)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Set ssm_och_override_done as FALSE */
  ssm_och_override_done = FALSE;

  if ( ssm_och_override_cookie_present == TRUE )
  {
    /* Delete the authentication */
    #ifndef FEATURE_SSM_USE_EFS_TEST
    if ( E_SUCCESS!=  sfs_rm( SSM_OCH_OVERRIDE_COOKIE_FILE ) )
    #else
    if ( 0 != efs_unlink( SSM_OCH_OVERRIDE_COOKIE_FILE ) )
    #endif
    {
      /* Early return */
      SSM_MSG_ERROR_1 ( " File remove failed %d", efs_errno);

      /* Update status of the client */
      if ( cmd_req->ovrr_cmd.cb )
        cmd_req->ovrr_cmd.cb ( E_SSM_OCH_FILE_REMOVE_FAILED, 0 , NULL,
		               cmd_req->ovrr_cmd.user_data );

      /* Update Override status through callback */
      ssm_och_update_status_thru_cb ( ssm_och_get_override_status() );

      return ;
    }

    ssm_och_override_cookie_present = FALSE;
  }

  /* Update Override status through callback */
  ssm_och_update_status_thru_cb ( ssm_och_get_override_status() );

  /* Update client through callback */
  if( cmd_req->ovrr_cmd.cb )
    cmd_req->ovrr_cmd.cb ( E_SSM_OCH_SUCCESS, 0 , NULL,
		           cmd_req->ovrr_cmd.user_data ) ;

} /* ssm_och_cancel_override */




/**
Processes request queued to OCH in a serialized manner through a task.
To be called from SSM task.

@param[in]
None

@return
none

@dependencies
None

*/

void ssm_och_process_request(void)
{
  /* To be fetched from the OCH command queue */

  ssm_och_queue_node_struct * temp;

  ssm_och_cmd_struct_t      * cmd_req;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Fetch Node from Key Handler request queue */
  temp = (ssm_och_queue_node_struct *)q_get( &ssm_och_cmd_q );

  while( temp!= NULL )
  {

    cmd_req = &(temp->cmd_req);

    if ( cmd_req  == NULL )
    {
      /** Clear up allocated space */
      free(temp);

      /* Fetch Node from Command Handler request queue */
      temp = (ssm_och_queue_node_struct *)q_get( &ssm_och_cmd_q );

      continue;
    }

    switch(cmd_req->ovrr_cmd.cmd)
    {
      case SSM_OCH_TEMP_OVERRIDE:
      {
	/* The below operation involve 2 different callbacks .. Hence a cmd_req structure is being allocated */
	ssm_och_cmd_struct_t *cmd_req1 = malloc( sizeof(ssm_och_cmd_struct_t) );
	if ( cmd_req1 == NULL )
	{
	  if ( cmd_req->ovrr_cmd.cb )
            cmd_req->ovrr_cmd.cb ( E_SSM_OCH_DYNAMIC_ALLOCATION_FAILED, 0 , NULL,
			           cmd_req->ovrr_cmd.user_data );
	  break;
	}
	if ( sizeof(ssm_och_cmd_struct_t) !=
	     memscpy( cmd_req1, sizeof(ssm_och_cmd_struct_t), cmd_req, sizeof(ssm_och_cmd_struct_t) ) )
	{
	  free ( cmd_req1 );
	  if ( cmd_req->ovrr_cmd.cb )
            cmd_req->ovrr_cmd.cb ( E_SSM_OCH_FAILED, 0 , NULL,
			           cmd_req->ovrr_cmd.user_data );
	  break;
	}
	ssm_och_temp_override( cmd_req1 );
        break;
      }
      case SSM_OCH_PERM_OVERRIDE:
      {
	/* The below operation involve 2 different callbacks .. Hence a cmd_req structure is being allocated */
	ssm_och_cmd_struct_t *cmd_req1 = malloc( sizeof(ssm_och_cmd_struct_t) );
        if ( cmd_req1 == NULL )
	{
	  if ( cmd_req->ovrr_cmd.cb )
            cmd_req->ovrr_cmd.cb ( E_SSM_OCH_DYNAMIC_ALLOCATION_FAILED, 0 , NULL,
			           cmd_req->ovrr_cmd.user_data );
	  break;
	}

	if ( sizeof(ssm_och_cmd_struct_t) !=
	     memscpy( cmd_req1, sizeof(ssm_och_cmd_struct_t), cmd_req, sizeof(ssm_och_cmd_struct_t) ) )
	{
	  free ( cmd_req1 );
	  if ( cmd_req->ovrr_cmd.cb )
            cmd_req->ovrr_cmd.cb ( E_SSM_OCH_FAILED, 0 , NULL,
			           cmd_req->ovrr_cmd.user_data );
	  break;
	}
	ssm_och_perm_override( cmd_req1 );
        break;
      }
      case SSM_OCH_CANCEL_OVERRIDE:
      {
	ssm_och_cancel_override(cmd_req);
        if ( cmd_req->ovrr_cmd.signature )
          free( cmd_req->ovrr_cmd.signature );
        break;
      }
      default:
       break;
    }

    #ifdef FEATURE_SSM_TEST_DEBUG
    switch(cmd_req->sign_cmd.cmd)
    {
      case SSM_OCH_SIGN_RANDOM_NONCE :
      {

	ssm_och_sign_random_nonce(cmd_req);
	if ( cmd_req->sign_cmd.mod )
	{
          /* Zero out the key before freeing it up */
          secure_memset ( cmd_req->sign_cmd.mod, 0 , cmd_req->sign_cmd.mod_len );

          free( cmd_req->sign_cmd.mod );
	  break;
	}
        if ( cmd_req->sign_cmd.exp )
	{
	  /* Zero out the key before freeing it up */
          secure_memset ( cmd_req->sign_cmd.exp, 0 , cmd_req->sign_cmd.exp_len );

	  free( cmd_req->sign_cmd.exp );
	}

        break;
      }

      default:
       break;
    }
    #endif

    /** Clear up allocated space */
    free( temp );

    /* Fetch Node from Command Handler request queue */
    temp = (ssm_och_queue_node_struct *)q_get( &ssm_och_cmd_q );
  }

} /* ssm_och_process_request */



/**
Gets random nonce of size 256 bits

@param[out]
random nonce  -  Structure allocated by the client to get the random nonce and
                 pass the address of the string for the random nonce to be copied

@return
ssm_och_err_t  - E_SSM_OCH_FAILED        Operation failed
                 E_SSM_OCH_SUCCESS    Operation in Progress

@dependencies
None

*/

ssm_och_err_t ssm_och_get_random_nonce
(
  /* Random Nonce */
   uint8  * random_nonce_ptr
)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( random_nonce_ptr == NULL )
  {
     return E_SSM_OCH_FAILED;
  }

  SSM_OCH_INTLOCK()

  if ( random_nonce_valid == TRUE )
  {
    if ( LEN_RANDOM_NONCE != memscpy( random_nonce_ptr, LEN_RANDOM_NONCE,
			      &random_nonce, LEN_RANDOM_NONCE ) )
    {
      SSM_OCH_INTFREE()
      return E_SSM_OCH_FAILED;
    }

    SSM_OCH_INTFREE()
    return E_SSM_OCH_SUCCESS;
  }

  SSM_OCH_INTFREE()

  if( PRNGML_ERROR_NONE != PrngML_getdata(((uint8*)random_nonce_ptr),
			   LEN_RANDOM_NONCE ) )
  {
    return E_SSM_OCH_FAILED;
  }

  SSM_OCH_INTLOCK()

  if ( LEN_RANDOM_NONCE != memscpy( &random_nonce, LEN_RANDOM_NONCE,
			   random_nonce_ptr , LEN_RANDOM_NONCE ) )
  {
    SSM_OCH_INTFREE()
    return E_SSM_OCH_FAILED;
  }

  ssm_och_set_random_nonce_valid( TRUE );

  SSM_OCH_INTFREE()

  return E_SSM_OCH_SUCCESS;

} /* ssm_och_get_random_nonce */


