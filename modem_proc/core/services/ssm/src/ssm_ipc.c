/*=============================================================================

             S E C U R E  S E R V I C E S   M O D U L E

GENERAL DESCRIPTION
  Implements SSM IPC

EXTERNALIZED FUNCTIONS

  INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2011 - 2014 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/ssm/src/ssm_ipc.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/18/14   din     Message is sent to peripheral only if SMDL channel is ready.
04/02/14   psu     Update diag macros using optimized versions.
04/29/13   din     Changes pertaining to ADSP were done.
04/02/13   din     Replaced memcpy with memscpy.
07/20/12   din     Removed key exchange code from SSM.
09/01/11   din     File created.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "ssm_v.h"
#include "ssm_ipc_v.h"
#include "ssm_smdl_v.h"
#include "ssm_mode_v.h"
#include <stringl/stringl.h>
#include <stdlib.h>
#include "secapi.h"


/*=============================================================================

                           DATA ATTRIBUTTES

=============================================================================*/



/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/

/** 

A common function to encode and send ipcs to peripheral processors. The function 
encrypts the data in SSM IPC format and sends it to specified processor.

@param[in] dest_proc                      Proc to which the IPC should be sent
@param[in] ipc                            Ipc to be sent
@param[in] err                            Error code to be sent
@param[in] message                        Message to be sent
@param[in] len_message                    length of message

@return
ssm_err_t                                 E_SSM_SUCCESS if suceeds.

@dependencies
None.

*/

static ssm_err_t ssm_ipc_encode_send_peripheral
(
  ssm_proc_enum_t        dest_proc,
  ssm_ipc_req_t          ipc,
  ssm_err_t              err,
  char *                 message,
  uint32                 len_message
)
{

  /* Message buffer which be ecnyrpted */
  char * temp;

  /* Length of temp buffer */
  uint32 len_temp;

  /* Encrypted temp buffer */
  char * temp_enc;

  /* Length of encrypted temp buffer */
  uint32 len_temp_enc;

  /* Temp IPC buffer which is sent */
  char * temp_ipc; 

  /* secapi proc info */
  secapi_sc_ss_e_type     ssid;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check if destination proc smdl connection is established */
  if (! ssm_is_smdl_ready(dest_proc) )
  {
    return E_SSM_FAIL;
  }

  /* Allocate space for temp buffer */
  temp = malloc ( len_message + SSM_RAW_IPC_HEADER_LENGTH );

  if ( temp == NULL )
  {
    SSM_MSG_ERROR_1 ( "Encode : Malloc Failed for length %d ", 
		     len_message + SSM_RAW_IPC_HEADER_LENGTH);
    return E_SSM_DATA_MALLOC_FAILED;
  }

  /* Encode header in temp buffer */
  /* snprint makes last character as NULL */
  snprintf ( temp, SSM_RAW_IPC_HEADER_LENGTH, "%10u|%10u|%10u|%10u|", ipc, 
             SSM_PROC_CURRENT, dest_proc, err );

  if ( message == NULL )
  {
    len_temp = SSM_RAW_IPC_HEADER_LENGTH - 1;
  }
  else
  {
    len_temp = len_message + SSM_RAW_IPC_HEADER_LENGTH;
    temp[SSM_RAW_IPC_HEADER_LENGTH -1 ] = '|';

    /* Populate temp buffer with message */
    if ( len_message != 
       memscpy( temp + SSM_RAW_IPC_HEADER_LENGTH, len_message, 
		               message, len_message ))
    {
      free(temp);
      SSM_MSG_ERROR ( "Encode : Memscpy Failed ");
      return E_SSM_FAIL;
    }
  }

  /* Calculate length of encrypted buffer */
  len_temp_enc = len_message + SSM_RAW_IPC_HEADER_LENGTH
	         + SSM_IPC_ENC_ADD_BUFFER_LENGTH;

  /* Allocate space for temporary encryted buffer */
  temp_enc = malloc ( len_temp_enc );

  if ( temp_enc == NULL )
  {
    free(temp);
    SSM_MSG_ERROR_1 ( "Encode : Malloc Failed for length %d ", 
		     len_temp_enc);
    return E_SSM_DATA_MALLOC_FAILED;
  }

  if ( dest_proc == SSM_PROC_MODEM )
    ssid = SC_SSID_MPSS;
  else 
    ssid = SC_SSID_LPASS;

  /* Encrypt temp buffer */
  if ( E_SUCCESS != secapi_secure_message( ssid, SC_CID_SSM,
                                          (uint8 * )temp, 
					  len_temp , 
	                                  (uint8 *)temp_enc, 
					  &len_temp_enc) )
  {
    SSM_MSG_ERROR_2 ( "Encode : Secapi failed input_len = %d output_len = %d", 
		     len_temp, len_temp_enc);
    
    free ( temp );
    free ( temp_enc );
    return E_SSM_SECAPI_FAILED;
  }

  /* Free temp as it is no longer required */
  free ( temp );
  
  /* Allocate space for temporary IPC buffer */
  temp_ipc = malloc ( SSM_ENC_IPC_HEADER_LENGTH + len_temp_enc );

  if ( temp_ipc == NULL )
  {
    free(temp_enc);
    SSM_MSG_ERROR_1( "Encode : Malloc Failed for length %d ", 
		     SSM_ENC_IPC_HEADER_LENGTH + len_temp_enc);
    return E_SSM_DATA_MALLOC_FAILED;
  }

  /* Encode header in temp IPC buffer */
  snprintf ( temp_ipc, SSM_ENC_IPC_HEADER_LENGTH + 1, "%10u|", ipc ); 

  /* Populate temp ipc with encrypted buffer */
  if ( len_temp_enc != memscpy( temp_ipc + SSM_ENC_IPC_HEADER_LENGTH, len_temp_enc, 
		               temp_enc, len_temp_enc ))
  {
    SSM_MSG_ERROR ( "Encode : Memscpy Failed ");
    
    free( temp_enc );
    free( temp_ipc );
    return E_SSM_FAIL;
  }

  /* Free temp_enc as it is no longer required */
  free ( temp_enc );

  /* Send the message to proc over smdl */
  if ( E_SSM_SUCCESS != ssm_smdl_handle_tx( dest_proc, temp_ipc, 
			                    len_temp_enc + SSM_ENC_IPC_HEADER_LENGTH) )
  { 
     SSM_MSG_ERROR_1 ( "Encode : SMDL trasnfer failed %d", 
		     len_temp_enc + SSM_ENC_IPC_HEADER_LENGTH);
     
     free( temp_ipc );
     return E_SSM_FAIL;
  }

  /* Free up the memory */
  free( temp_ipc );

  return E_SSM_SUCCESS;

} /* ssm_ipc_encode_send_peripheral */


#ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC

/** 

A common function to encode and send ipcs to apps processor. 

@param[in] ipc                            Ipc to be sent
@param[in] err                            Error code to be sent

@return
ssm_err_t                                 E_SSM_SUCCESS if suceeds.

@dependencies
None.

*/

static ssm_err_t ssm_ipc_encode_send_apps
(
  ssm_ipc_req_t          ipc,
  ssm_err_t              err
)
{

  /* Message buffer which be ecnyrpted */
  char * temp;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Allocate space for temp buffer */
  temp = malloc ( SSM_RAW_IPC_HEADER_LENGTH_APPS );

  if ( temp == NULL )
  {
    SSM_MSG_ERROR_1 ( "Encode : Malloc Failed for length %d ", 
		     SSM_RAW_IPC_HEADER_LENGTH_APPS);
    
    return E_SSM_DATA_MALLOC_FAILED;
  }

  /* Encode header in temp buffer */
  snprintf ( temp, SSM_RAW_IPC_HEADER_LENGTH_APPS, "%10u|%10u", ipc, err );

  temp[SSM_RAW_IPC_HEADER_LENGTH_APPS -1] = '|';

  /* Send the message to proc over smdl */
  if ( E_SSM_SUCCESS != ssm_smdl_handle_tx( SSM_PROC_APPS, temp, 
			                    SSM_RAW_IPC_HEADER_LENGTH_APPS) )
  { 
     SSM_MSG_ERROR_1 ( "Encode : SMDL trasnfer failed %d", 
		     SSM_RAW_IPC_HEADER_LENGTH_APPS);
     free( temp );
     return E_SSM_FAIL;
  }

  /* Free up the memory */
  free( temp );

  return E_SSM_SUCCESS;

} /* ssm_ipc_encode_send_apps */

#endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */

/** 

A common function to encode and send ipcs . 

@param[in] dest_proc                      Proc to which the IPC should be sent
@param[in] ipc                            Ipc to be sent
@param[in] err                            Error code to be sent
@param[in] message                        Message to be sent
@param[in] len_message                    length of message

@return
ssm_err_t                                 E_SSM_SUCCESS if suceeds.

@dependencies
None.

*/

ssm_err_t ssm_ipc_encode_send
(
  ssm_proc_enum_t        dest_proc,
  ssm_ipc_req_t          ipc,
  ssm_err_t              err,
  char *                 message,
  uint32                 len_message
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  #ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC
  /* Sending updates to SSM apps driver over secure channel between 
   * modem and TZ is useless since apps driver cannot be trusted. */
  if ( dest_proc == SSM_PROC_APPS )
    return ssm_ipc_encode_send_apps( ipc, err );
  #endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */

  return ssm_ipc_encode_send_peripheral ( dest_proc,
                                          ipc,
                                          err,
                                          message,
					  len_message );
} /* ssm_ipc_encode_send */


/** 

Helper internal function for ssm_decode_ipc_message. It converts ascii component
of IPC message of fixed size SSM_SINGLE_MESSAGE_CODE_LENGTH into uint32.

@param[in] decode                         Decode message
@param[in] len                            Length of decode message
@param[in] iter                           Iterator location 
@param[out] ret_val                       decoded uint32 value

@return
uint32                                    length of decode buffer read

@dependencies
None.

*/

uint16 ssm_ipc_decode_break_ipc_helper
( 
  char *             decode,
  uint32             len_decode,
  uint16             iter,
  uint32 *           ret_val
)
{
  /* Length of decode buffer read */
  uint16  len = 0 ;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  *ret_val= 0;

  if ( len_decode < SSM_SINGLE_MESSAGE_CODE_LENGTH )
    return 0;

  while( iter + len < len_decode && decode[ iter + len ] != '|' )
  {
    if ( decode[ iter + len ] >= '0' && decode[ iter + len ] <= '9' )
      *ret_val = *ret_val * 10 + (decode[ iter + len ] - '0');
    len++;
  }

  if ( len != SSM_SINGLE_MESSAGE_CODE_LENGTH )
  {
    ret_val =0;
    return 0;
  }	  

  return len;
  
} /* ssm_ipc_decode_break_ipc_helper */


/** 

Helper function to fetch contents from a string buffer. It stops 
searching after a string type is found.

@param[in, out] types_val                 Structure with decode info
@param[in] num_types_val                  length of structure
@param[in] message                        message that needs to be decoded
@param[in] len_message                    length of message
@param[out] iter_message                  total length read

@return
ssm_err_t                                 E_SSM_SUCCESS if it succeeds

@dependencies
None.

*/

ssm_err_t ssm_ipc_decode_break_ipc
( 
  ssm_types_decode_info * types_val,
  uint16                  num_types_val,
  char *                  message,
  uint16                  len_message,
  uint32 *                iter_message
)
{
  uint32 iter_num_types_val =0;
  uint16 len_read;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Validate the length of the message */
  if ( ( num_types_val - 1 ) * SSM_SINGLE_MESSAGE_WITH_DELIM_LENGTH > 
       len_message )
  {
    SSM_MSG_ERROR_2(" ssm_ipc_decode_break_ipc length check failed %d %d" ,
		    ( num_types_val - 1 ) * SSM_SINGLE_MESSAGE_WITH_DELIM_LENGTH , 
		    len_message);
    return E_SSM_MSG_DECODE_ERR;
  }
 
  /* Parse the string */
  while ( iter_num_types_val < num_types_val && *iter_message < len_message )
  {
    /* Determine the type expected */
    switch ( types_val[iter_num_types_val].type )
    {
      case SSM_TYPE_UINT32 :
      {
        /* Type is uint32 */
	len_read = ssm_ipc_decode_break_ipc_helper( message, len_message, 
			                            *iter_message, 
			  &(types_val[iter_num_types_val].val.uint32_val) );

	if ( len_read == 0 )
	{
	  SSM_MSG_ERROR(" ssm_ipc_decode_break_ipc Decode failed");
	  return E_SSM_FAIL;
	}

        *iter_message += len_read + 1;

	break;
      }
      case SSM_TYPE_BOOLEAN :
      {
        /* Type is boolean */
	types_val[iter_num_types_val].val.boolean_val = TRUE;
	
	if ( message[*iter_message + SSM_SINGLE_MESSAGE_CODE_LENGTH - 1 ] == '0' )
	  types_val[iter_num_types_val].val.boolean_val = FALSE;

	*iter_message += SSM_SINGLE_MESSAGE_WITH_DELIM_LENGTH;
	
	break;
      }
      case SSM_TYPE_STRING :
      {
        /* Type is string */
	/* Assumes that previous value is the length of data, 
           Currently valid only for SSM_MTOP_PERM_FILE */
	if ( types_val[iter_num_types_val-1].val.uint32_val + *iter_message != 
             len_message )
	{
          SSM_MSG_ERROR_2(" ssm_ipc_decode_break_ipc String length verification failed" ,
		          types_val[iter_num_types_val-1].val.uint32_val + *iter_message, 
			  len_message);

	  return E_SSM_MSG_DECODE_ERR;
	}

	if ( types_val[iter_num_types_val -1].val.uint32_val == 0 )
	{
	  types_val[iter_num_types_val].val.string_val = NULL;
	  break;
	}

        types_val[iter_num_types_val].val.string_val 
                     = malloc( types_val[iter_num_types_val-1].val.uint32_val ); 

	if ( types_val[iter_num_types_val].val.string_val == NULL )
	{
	  return E_SSM_DATA_MALLOC_FAILED;
	}

	if ( types_val[iter_num_types_val-1].val.uint32_val != 
	     memscpy( types_val[iter_num_types_val].val.string_val, 
	              types_val[iter_num_types_val-1].val.uint32_val,
	              message + *iter_message,
	              types_val[iter_num_types_val-1].val.uint32_val ))
	{
	  free( types_val[iter_num_types_val].val.string_val );

	  return E_SSM_MSG_DECODE_ERR;
	}

	*iter_message += types_val[iter_num_types_val-1].val.uint32_val;

	break;
      }
      case SSM_TYPE_UINT8_MAX :
      {
        return E_SSM_FAIL;
      }
    } 
	  
    iter_num_types_val++;
  }

  if ( iter_num_types_val == num_types_val )
    return E_SSM_SUCCESS;

  return E_SSM_FAIL;

} /* ssm_ipc_decode_break_ipc */


/** 

Process the decoded messages based on type of request

@param[in] src_proc                       IPC origin proc
@param[in] ipc_req                        Type of request
@param[in] error_code                     error code sent with IPC
@param[in] message_len                    Length of decoded message
@param[in] message                        Decoded message which is further processed

@return
ssm_err_t -- E_SSM_SUCCESS if success

@dependencies
None.

*/

static ssm_err_t ssm_ipc_process_decode_message
(
  ssm_proc_enum_t      src_proc,
  ssm_ipc_req_t        ipc_req,
  ssm_err_t            error_code,
  uint32               message_len, 
  char *               message 
)
{
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  #if 0
	
  SSM_MSG_MED_2 ( " Function : ssm_process_decode_message \
		  src_proc = %d ipc_req = %d ", 
		  src_proc, ipc_req );

  
  SSM_MSG_MED_2 ( " Function : ssm_process_decode_message \
		  error_code = %d message_len = %d ", 
		  error_code, message_len);

  #endif 

  /* Process request based on request type */
  switch(ipc_req)
  {

    /* Mode Update for Modem */
    case SSM_ATOM_MODE_UPDATE :
    {
      SSM_MSG_HIGH( "Decode : SSM_ATOM_MODE_UPDATE ");
       
      #ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC
        /* Call into mode update status */
        ssm_mode_update_mode( src_proc, error_code, message, message_len );   
      #else /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */
        return E_SSM_INVALID_DATA; 
      #endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */   
      break;
    }

    /* Mode update for peripheral */
    case SSM_MTOP_MODE_UPDATE:
    {
       SSM_MSG_HIGH( "Decode : SSM_MTOP_MODE_UPDATE ");
       
      #ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC
        return E_SSM_INVALID_DATA; 
      #else /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */
        /* Call into mode update status */
        ssm_mode_update_mode( src_proc, error_code, message, message_len );
      #endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */
      break;
    }


    /* Mode update Status from peripheral */
    case SSM_PTOM_MODE_UPDATE_STATUS:
    {
       SSM_MSG_HIGH( "Decode : SSM_PTOM_MODE_UPDATE_STATUS ");
       
      #ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC
        /* Call into mode update status */
        ssm_mode_ptom_mode_update_status( src_proc, error_code );
      #else /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */
        return E_SSM_INVALID_DATA; 
      #endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */
      break;
    }

    /* Mode update status from Modem to Apps*/
    case SSM_MTOA_MODE_UPDATE_STATUS:
    {
       SSM_MSG_HIGH( "Decode : SSM_MTOA_MODE_UPDATE_STATUS ");
       
       /* To be handled on apps */
       return E_SSM_INVALID_DATA; 
    }

    /* Accept request from Peripheral processor */
    case SSM_PTOM_REQ_PERM_FILE :
    {
      #ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC
	ssm_parser_ptom_req_perm_file( src_proc, message, message_len );	    
      #else /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */
	return E_SSM_INVALID_DATA; 
      #endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */

      break;
    }

  
    /* Invalid IPC request for Modem */
    case SSM_MTOP_PERM_FILE :
    { 
       SSM_MSG_HIGH( "Decode : SSM_MTOP_PERM_FILE ");
       
       #ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC
         /* To be handled on LPASS */
         return E_SSM_INVALID_DATA;
       #else /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */
	 return ssm_parser_mtop_perm_file( src_proc, error_code, 
			                   message, message_len );
       #endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */

       break; 
    }

    /* Message from peripheral to Modem updating status of perm file exchange */
    case SSM_PTOM_PERM_FILE_STATUS :
    {
       #ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC
	 ssm_parser_ptom_perm_file_status( src_proc, error_code, 
			                   message, message_len );
       #else /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */
         return E_SSM_INVALID_DATA; 
       #endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */
       
      break;
    }

    /* Return INVALID REQUEST */
    default : 
    {
      SSM_MSG_ERROR( " Decode : INVALID REQUEST ");
      return E_SSM_INVALID_DATA;
    }
    
  } /* switch */

  /* Return Success */
  return E_SSM_SUCCESS;

} /* ssm_process_decode_message */


/** 

Processes decoded messages based on type of request

@param[in] proc                           IPC origin proc
@param[in] decode                         Decode message
@param[in] bytes_read                     Length of decode message

@return
void

@dependencies
None.

*/

void ssm_ipc_decode_ipc_message
( 
  ssm_proc_enum_t    proc,
  char *             decode,
  uint32             bytes_read
)
{
  
  /* To iterate through the array */
  uint16                   iter =0 ;

  /* Stores type of SSM IPC request */
  ssm_ipc_req_t            ipc_req, ipc_req_enc;

  /* Stores source and destination proc from ipc */
  ssm_proc_enum_t          src_proc, dest_proc;

  /* Stores error code from ipc */
  ssm_err_t                error_code; 

  /* length read from ssm_decode_ipc_message_helper */
  uint32                   len_read =0 ;
  
  /* Buffer after decode withot headers is decrypted */
  char *                   temp;

  /* Length of temp buffer */
  uint32                   len_temp;

  /* Decode info to help break ipc */
  ssm_types_decode_info decode_info[4];

  /* secapi proc info */
  secapi_sc_ss_e_type     ssid;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  SSM_MSG_MED( " Function : ssm_decode_ipc_message ");

  if ( bytes_read <= SSM_ENC_IPC_HEADER_LENGTH )
  {
    SSM_MSG_HIGH_2( " Decode : Early return \
                    iter = %d bytes_read = %d ", \
         	    iter, bytes_read);

    #ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC
    if ( proc == SSM_PROC_APPS )
    {
      ssm_ipc_encode_send_apps( SSM_MTOA_MODE_UPDATE_STATUS, 
		                E_SSM_MSG_DECODE_ERR);
    }
    #endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */

    return;
  }

  /* Get ipc_req from decode buffer */
  len_read = ssm_ipc_decode_break_ipc_helper( decode, bytes_read, iter, 
	     (uint32 *)&ipc_req );

  if ( len_read == 0 )
  {
    SSM_MSG_HIGH( " Decode :  Decode failed ");

    #ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC
    if ( proc == SSM_PROC_APPS )
    {
      ssm_ipc_encode_send_apps( SSM_MTOA_MODE_UPDATE_STATUS, 
		                E_SSM_MSG_DECODE_ERR);
    }
    #endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */
    
    return;
  }

  if ( ipc_req <= SSM_IPC_MIN &&  ipc_req>= SSM_IPC_MAX )
  {
    SSM_MSG_HIGH( " Decode :  IPC bounds Check failed ");

    #ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC
    if ( proc == SSM_PROC_APPS )
    {
      ssm_ipc_encode_send_apps( SSM_MTOA_MODE_UPDATE_STATUS, 
		                E_SSM_MSG_DECODE_ERR);
    }
    #endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */

    return;
  } 

  iter += len_read ;

  /* Return if delimiter not present in decode message */
  if ( iter == bytes_read )
  {
    SSM_MSG_HIGH_2( " Decode : Early return \
                    iter = %d bytes_read = %d ", \
         	    iter, bytes_read);

    #ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC
    if ( proc == SSM_PROC_APPS )
    {
      ssm_ipc_encode_send_apps( SSM_MTOA_MODE_UPDATE_STATUS, 
		                E_SSM_MSG_DECODE_ERR);
    }
    #endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */
    return ;
  }

  /* Skip delimiter */
  iter++;

  /* Allocate memory for temp */
  temp = malloc ( bytes_read - SSM_ENC_IPC_HEADER_LENGTH );

  len_temp = bytes_read - SSM_ENC_IPC_HEADER_LENGTH;

  if ( temp == NULL )
  {
    SSM_MSG_ERROR_1( " Decode : Malloc Failed for length %d " , 
		     len_temp);

    #ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC
    if ( proc == SSM_PROC_APPS )
    {
      ssm_ipc_encode_send_apps( SSM_MTOA_MODE_UPDATE_STATUS, 
		                E_SSM_MSG_DECODE_ERR);
    }
    #endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */
    return;
  }
  
  if ( proc == SSM_PROC_APPS )
    ssid = SC_SSID_TZ;
  else if ( proc == SSM_PROC_MODEM )
    ssid = SC_SSID_MPSS;
  else 
    ssid = SC_SSID_LPASS;

  /* Decrypt ipc message */
  if ( E_SUCCESS != 
       secapi_authenticate_decrypt_message( ssid, SC_CID_SSM, 
                                (uint8 *)( decode + iter ), 
                                bytes_read - iter, 
                                (uint8 *)temp,
				&len_temp ) )
  {
    SSM_MSG_ERROR(" Decode Buffer Decryption failed ");

    free( temp );

    #ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC
    if ( proc == SSM_PROC_APPS )
    {
      ssm_ipc_encode_send_apps( SSM_MTOA_MODE_UPDATE_STATUS, 
		                E_SSM_SECAPI_FAILED );
    }
    #endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */
    return ;
  }

  /* Populate Decode info */
  decode_info[0].type = SSM_TYPE_UINT32 ; /* for ipc_req_enc */
  decode_info[1].type = SSM_TYPE_UINT32 ; /* for src_proc */
  decode_info[2].type = SSM_TYPE_UINT32 ; /* for dest_proc */
  decode_info[3].type = SSM_TYPE_UINT32 ; /* for error_code */ 

  len_read =0; 

  if ( E_SSM_SUCCESS != 
		  ssm_ipc_decode_break_ipc( &(decode_info[0]), 4, temp, 
                                            len_temp, &len_read ) )  
  {
    SSM_MSG_HIGH( " Decode :  Decode failed ");
    
    free( temp );

    #ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC
    if ( proc == SSM_PROC_APPS )
    {
      ssm_ipc_encode_send_apps( SSM_MTOA_MODE_UPDATE_STATUS, 
		                E_SSM_MSG_DECODE_ERR);
    }
    #endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */
    return;
  }
  
  ipc_req_enc = (ssm_ipc_req_t )      decode_info[0].val.uint32_val;
  src_proc    = (ssm_proc_enum_t )    decode_info[1].val.uint32_val;
  dest_proc   = (ssm_proc_enum_t )    decode_info[2].val.uint32_val;
  error_code  = (ssm_err_t ) decode_info[3].val.uint32_val;

  iter += len_read ;

  if ( ipc_req_enc != ipc_req )
  {
    SSM_MSG_ERROR_2(" Unexpected IPC Message ipc_req= %d ipc_req_enc = %d", 
		    ipc_req, ipc_req_enc);
    free( temp );

    #ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC
    if ( proc == SSM_PROC_APPS )
    {
      ssm_ipc_encode_send_apps( SSM_MTOA_MODE_UPDATE_STATUS, 
		                E_SSM_MSG_DECODE_ERR);
    }
    #endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */
    return ;
  }

  if ( proc != src_proc )
  {
    SSM_MSG_ERROR_2(" Unexpected IPC Message proc= %d src_proc = %d", 
		    proc, src_proc);
    free( temp );
    
    #ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC
    if ( proc == SSM_PROC_APPS )
    {
      ssm_ipc_encode_send_apps( SSM_MTOA_MODE_UPDATE_STATUS, 
		                E_SSM_MSG_DECODE_ERR);
    }
    #endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */
    return ;
  }

  if ( SSM_PROC_CURRENT != dest_proc )
  {
    SSM_MSG_ERROR_2(" Unexpected IPC Message proc= %d dest_proc = %d", 
		    SSM_PROC_CURRENT, dest_proc);
    free( temp );

    #ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC
    if ( proc == SSM_PROC_APPS )
    {
      ssm_ipc_encode_send_apps( SSM_MTOA_MODE_UPDATE_STATUS, 
		                E_SSM_MSG_DECODE_ERR);
    }
    #endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */
    return ;
  }

   /* Check if there is anything else in the buffer still to be read */
  if ( len_read <= len_temp )
  {
    iter++;

    /* Process ipc messages */
    ssm_ipc_process_decode_message( src_proc, ipc_req, error_code, 
		                len_temp - len_read, temp + len_read );
  }
  else
  {
    /* Process ipc messages */
    ssm_ipc_process_decode_message( src_proc, ipc_req, error_code, 0 , NULL );
  }

  /* Free space allocated */
  free( temp );

} /* ssm_ipc_decode_ipc_message */
